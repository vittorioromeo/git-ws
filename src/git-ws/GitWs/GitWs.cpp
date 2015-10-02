// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <future>
#include "git-ws/GitWs/GitWs.hpp"

using namespace std;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvucl;

namespace gitws
{
string getBriefHelp(const Cmd& mCmd)
{
    return mCmd.getNamesStr() + " " + mCmd.getStr<EType::Arg>() + " " +
           mCmd.getStr<EType::ArgOpt>() + " " + mCmd.getStr<EType::Flag>() +
           " " + mCmd.getStr<EType::ArgPack>();
}

void GitWs::runInRepos(
const vector<Repo>& mRepos, const string& mCommand, bool mPrintEmpty)
{
    for(const auto& r : mRepos) {
        vector<string> files{r.run(mCommand)};
        if(files.empty() && !mPrintEmpty) continue;

        lo(r.getPath()) << "\n|\n";
        for(auto& f : files) lo() << "| -> " << f << "\n";
        lo() << "|\nL" << hr(-1) << "\n\n";
    }
}

void GitWs::initCmdHelp()
{
    auto& cmd(ctx.create({"?", "help"}));
    cmd.setDesc("Show help for all commands or a single command.");

    auto& argOpt(cmd.create<ArgOpt<string>>(""));
    argOpt.setName("Command name");
    argOpt.setBriefDesc("Name of the command to get help for.");
    argOpt.setDesc("Leave blank to get general help.");

    auto& flagVerbose(cmd.create<Flag>("v", "verbose"));
    flagVerbose.setBriefDesc("Verbose general help?");

    cmd += [&]
    {
        if(!argOpt) {
            lo("git-ws help") << "\n\n";
            for(const auto& c : ctx.getCmds())
                lo() << getBriefHelp(*c) << "\n"
                     << (flagVerbose ? c->getHelpStr() : "");
            lo() << "\n";
        }
        else
        {
            auto& c(ctx.findCmd(argOpt.get()));
            lo() << "\n" << getBriefHelp(c) << "\n" << c.getHelpStr();
        }

        lo().flush();
    };
}
void GitWs::initCmdPush()
{
    auto& cmd(ctx.create({"push"}));
    cmd.setDesc("Pushes every git repo.");

    auto& flagForce(cmd.create<Flag>("f", "force"));
    flagForce.setBriefDesc("Forced push?");

    auto& flagAll(cmd.create<Flag>("a", "all"));
    flagAll.setBriefDesc(
    "Run the command in all repos (even non-ahead ones), for all branches.");

    cmd += [&]
    {
        for(const auto& rd : repos) {
            if(!flagAll && !rd.canPush()) continue;

            string toRun{"git push"};
            if(flagAll) toRun += " --all";
            if(flagForce) toRun += " -f";
            if(!flagAll) toRun += " origin " + rd.getBranch();
            rd.run(toRun);
        }
    };
}
void GitWs::initCmdPull()
{
    auto& cmd(ctx.create({"pull"}));
    cmd.setDesc("Pulls every git repo.");

    auto& flagStash(cmd.create<Flag>("s", "stash"));
    flagStash.setBriefDesc("Stash all changes before pulling?");

    auto& flagForce(cmd.create<Flag>("f", "force-checkout"));
    flagForce.setBriefDesc("Run a force checkout before pulling?");

    auto& flagChanged(cmd.create<Flag>("c", "changed-only"));
    flagChanged.setBriefDesc(
    "Run the command only in folders where repos have changes?");

    cmd += [&]
    {
        auto currentRepos(flagChanged ? getChangedRepos(true) : repos);
        if(flagStash) runInRepos(currentRepos, "git stash");
        if(flagForce) runInRepos(currentRepos, "git checkout -f");
        runInRepos(currentRepos, "git pull");
    };
}
void GitWs::initCmdSubmodule()
{
    auto& cmd(ctx.create({"sub", "submodule"}));
    cmd.setDesc(
    "Work with git submodules in every repo with dirty submodules.");

    auto& arg(cmd.create<Arg<string>>());
    arg.setName("Action");
    arg.setBriefDesc(
    "Action to run for every submodule. Can be 'pull' or 'au'.");
    arg.setDesc(
    "'push' recursively pushes all submodules to the remote\n'pull' "
    "recursively pulls the latest submodules from the remote, discarding any "
    "change.\n'au' calls 'pull' then pushes in succession.");

    auto& flagAll(cmd.create<Flag>("a", "all"));
    flagAll.setBriefDesc("Run the command in all repos?");

    cmd += [&]
    {
        auto currentRepos(flagAll ? repos : getBehindSMRepos());

        if(arg.get() == "pull" || arg.get() == "au")
            for(const auto& r : currentRepos) r.runSMPull();
        if(arg.get() == "push" || arg.get() == "au")
            for(const auto& r : currentRepos) r.runSMPush();
    };
}
void GitWs::initCmdStatus()
{
    auto& cmd(ctx.create({"st", "status"}));
    cmd.setDesc("Prints the status of all repos.");

    auto& showAllFlag(cmd.create<Flag>("a", "all"));
    showAllFlag.setBriefDesc("Print empty messages?");

    cmd += [&]
    {
        runInRepos(
        repos, "git status -s --ignore-submodules=dirty", showAllFlag);
    };
}
void GitWs::initCmdGitg()
{
    auto& cmd(ctx.create({"gitg"}));
    cmd.setDesc("Open the gitg gui application in every repo folder.");

    auto& flagAll(cmd.create<Flag>("a", "all"));
    flagAll.setBriefDesc("Run the command in all repos?");

    cmd += [&]
    {
        auto currentRepos(flagAll ? repos : getChangedRepos(true));
        runInRepos(currentRepos, "gitg -c&");
    };
}
void GitWs::initCmdDo()
{
    auto& cmd(ctx.create({"do"}));
    cmd.setDesc("Runs a shell command in every repo folder.");

    auto& arg(cmd.create<Arg<string>>());
    arg.setName("Command to run");
    arg.setBriefDesc(
    "This is the command that will be called in every repo folder.");
    arg.setDesc("Consider wrapping a more complex command with quotes.");

    auto& flagChanged(cmd.create<Flag>("c", "changed-only"));
    flagChanged.setBriefDesc(
    "Run the command only in folders where repos have changes?");

    auto& flagAhead(cmd.create<Flag>("a", "ahead-only"));
    flagAhead.setBriefDesc(
    "Run the command only in folders where repos are ahead of the remote?");

    cmd += [&]
    {
        if(flagChanged && flagAhead) {
            lo() << "-c and -a are mutually exclusive"
                 << "\n";
            return;
        }

        auto currentRepos(flagChanged ? getChangedRepos(true) : repos);
        if(flagAhead) currentRepos = getAheadRepos();

        runInRepos(currentRepos, arg.get());
    };
}
void GitWs::initCmdQuery()
{
    auto& cmd(ctx.create({"query"}));
    cmd.setDesc(
    "Queries the status of all the repos, returning whether they are changed "
    "or ahead.");

    auto& flagDisplayAll(cmd.create<Flag>("a", "display-all"));
    flagDisplayAll.setBriefDesc(
    "Display status of all repositories, even the ones without changes.");

    auto& flagCheckSubmodules(cmd.create<Flag>("s", "check-submodules"));
    flagCheckSubmodules.setBriefDesc(
    "Check if submodules are outdated or dirty. (Very time consuming)");

    cmd += [&]
    {
        for(const auto& rd : repos) {
            bool foundAnything{false};

            ostringstream s;
            s << setw(25) << left << rd.getPath() << setw(15)
              << " ~ " + rd.getBranch() << flush;

            {
                if(rd.getStatus(true) == Repo::Status::CanCommit) {
                    s << setw(15) << left << " (can commit)";
                    foundAnything = true;
                }
                if(rd.canPush()) {
                    s << setw(15) << left << " (can push)";
                    foundAnything = true;
                }
                if(rd.canPull()) {
                    s << setw(15) << left << " (can pull)";
                    foundAnything = true;
                }

                if(!flagCheckSubmodules) continue;
                if(rd.getStatus(false) == Repo::Status::DirtySM) {
                    s << setw(15) << left << " (dirty submodules)";
                    foundAnything = true;
                }
                if(rd.getSubmodulesBehind()) {
                    s << setw(15) << left << " (outdated submodules)";
                    foundAnything = true;
                }
            }

            if(foundAnything || flagDisplayAll) cout << s.str() << "\n";
        }
    };
}
}
