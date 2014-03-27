// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <cstdio>
#include <iostream>
#include <iomanip>
#include <future>
#include "git-ws/GitWs/GitWs.hpp"

using namespace std;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvu::CommandLine;

namespace gitws
{
	string getBriefHelp(const Cmd& mCmd) { return mCmd.getNamesStr() + " " + mCmd.getArgsStr() + " " + mCmd.getOptArgsStr() + " " + mCmd.getFlagsStr() + " " + mCmd.getArgPacksStr(); }

	void GitWs::runInRepos(const vector<Repo>& mRepos, const string& mCommand, bool mPrintEmpty)
	{
		for(const auto& r : mRepos)
		{
			vector<string> files{r.run(mCommand)};
			if(files.empty() && !mPrintEmpty) continue;

			lo(r.getPath()) << "\n|\n";
			for(auto& f : files) lo() << "| -> " << f << "\n";
			lo() << "|" << "\n";
			lo("------------") << "\n\n";
		}
	}

	void GitWs::initCmdHelp()
	{
		auto& cmd(cmdLine.create({"?", "help"}));
		cmd.setDesc("Show help for all commands or a single command.");

		auto& optArg(cmd.createOptArg<string>(""));
		optArg.setName("Command name");
		optArg.setBriefDesc("Name of the command to get help for.");
		optArg.setDesc("Leave blank to get general help.");

		auto& flagVerbose(cmd.createFlag("v", "verbose"));
		flagVerbose.setBriefDesc("Verbose general help?");

		cmd += [&]
		{
			if(!optArg)
			{
				lo("git-ws help") << "\n\n";
				for(const auto& c : cmdLine.getCmds()) lo() << getBriefHelp(*c) << "\n" << (flagVerbose ? c->getHelpStr() : "");
			}

			auto& c(cmdLine.findCmd(optArg.get()));
			lo() << "\n" << getBriefHelp(c) << "\n" << c.getHelpStr();
			lo().flush();
		};
	}
	void GitWs::initCmdPush()
	{
		auto& cmd(cmdLine.create({"push"}));
		cmd.setDesc("Pushes every git repo.");

		auto& flagForce(cmd.createFlag("f", "force"));
		flagForce.setBriefDesc("Forced push?");

		auto& flagAll(cmd.createFlag("a", "all"));
		flagAll.setBriefDesc("Run the command in all repos (even non-ahead ones), for all branches.");

		cmd += [&]
		{
			for(const auto& rd : repos)
			{
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
		auto& cmd(cmdLine.create({"pull"}));
		cmd.setDesc("Pulls every git repo.");

		auto& flagStash(cmd.createFlag("s", "stash"));
		flagStash.setBriefDesc("Stash all changes before pulling?");

		auto& flagForce(cmd.createFlag("f", "force-checkout"));
		flagForce.setBriefDesc("Run a force checkout before pulling?");

		auto& flagChanged(cmd.createFlag("c", "changed-only"));
		flagChanged.setBriefDesc("Run the command only in folders where repos have changes?");

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
		auto& cmd(cmdLine.create({"sub", "submodule"}));
		cmd.setDesc("Work with git submodules in every repo with dirty submodules.");

		auto& arg(cmd.createArg<string>());
		arg.setName("Action");
		arg.setBriefDesc("Action to run for every submodule. Can be 'pull' or 'au'.");
		arg.setDesc("'push' recursively pushes all submodules to the remote\n'pull' recursively pulls the latest submodules from the remote, discarding any change.\n'au' calls 'pull' then pushes in succession.");

		auto& flagAll(cmd.createFlag("a", "all"));
		flagAll.setBriefDesc("Run the command in all repos?");

		cmd += [&]
		{
			auto currentRepos(flagAll ? repos : getBehindSMRepos());

			if(arg.get() == "pull" || arg.get() == "au") for(const auto& r : currentRepos) r.runSMPull();
			if(arg.get() == "push" || arg.get() == "au") for(const auto& r : currentRepos) r.runSMPush();
		};
	}
	void GitWs::initCmdStatus()
	{
		auto& cmd(cmdLine.create({"st", "status"}));
		cmd.setDesc("Prints the status of all repos.");

		auto& showAllFlag(cmd.createFlag("a", "all"));
		showAllFlag.setBriefDesc("Print empty messages?");

		cmd += [&]{ runInRepos(repos, "git status -s --ignore-submodules=dirty", showAllFlag); };
	}
	void GitWs::initCmdGitg()
	{
		auto& cmd(cmdLine.create({"gitg"}));
		cmd.setDesc("Open the gitg gui application in every repo folder.");

		auto& flagAll(cmd.createFlag("a", "all"));
		flagAll.setBriefDesc("Run the command in all repos?");

		cmd += [&]
		{
			auto currentRepos(flagAll ? repos : getChangedRepos(true));
			runInRepos(currentRepos, "gitg -c&");
		};
	}
	void GitWs::initCmdDo()
	{
		auto& cmd(cmdLine.create({"do"}));
		cmd.setDesc("Runs a shell command in every repo folder.");

		auto& arg(cmd.createArg<string>());
		arg.setName("Command to run");
		arg.setBriefDesc("This is the command that will be called in every repo folder.");
		arg.setDesc("Consider wrapping a more complex command with quotes.");

		auto& flagChanged(cmd.createFlag("c", "changed-only"));
		flagChanged.setBriefDesc("Run the command only in folders where repos have changes?");

		auto& flagAhead(cmd.createFlag("a", "ahead-only"));
		flagAhead.setBriefDesc("Run the command only in folders where repos are ahead of the remote?");

		cmd += [&]
		{
			if(flagChanged && flagAhead) { lo() << "-c and -a are mutually exclusive" << endl; return; }

			auto currentRepos(flagChanged ? getChangedRepos(true) : repos);
			if(flagAhead) currentRepos = getAheadRepos();

			runInRepos(currentRepos, arg.get());
		};
	}
	void GitWs::initCmdQuery()
	{
		auto& cmd(cmdLine.create({"query"}));
		cmd.setDesc("Queries the status of all the repos, returning whether they are changed or ahead.");
		cmd += [&]
		{
			vector<future<string>> futures;

			for(const auto& rd : repos)
			{
				auto f(async(launch::async, [&]
				{
					ostringstream s;
					s << setw(25) << left << rd.getPath() << setw(15) << " ~ " + rd.getBranch() << flush;

					{
						auto a0 = async(launch::async, [&]{ if(rd.getStatus(true) == Repo::Status::CanCommit)	s << setw(15) << left << "(can commit)" << flush; });
						auto a1 = async(launch::async, [&]{ if(rd.canPush())									s << setw(15) << left << "(can push)" << flush; });
						auto a2 = async(launch::async, [&]{ if(rd.getStatus(false) == Repo::Status::DirtySM)	s << setw(15) << left << "(dirty submodules)" << flush; });
						auto a3 = async(launch::async, [&]{ if(rd.canPull())									s << setw(15) << left << "(can pull)" << flush; });
						auto a4 = async(launch::async, [&]{ if(rd.getSubmodulesBehind())						s << setw(15) << left << "(outdated submodules)"; });
					}

					return s.str();
				}));

				futures.emplace_back(move(f));
			}

			for(auto& f : futures) cout << f.get() << endl;
		};
	}
}
