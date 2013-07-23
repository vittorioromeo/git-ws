// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <cstdio>
#include <iostream>
#include <iomanip>
#include "git-ws/GitWs/GitWs.h"

using namespace std;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvcl;

namespace gitws
{
	vector<string> runShInPath(const string& mPath, const string& mCommand)
	{
		string toRun{"(cd " + mPath + ";" + mCommand + ")"};
		FILE* pipe{popen(toRun.c_str(), "r")};
		char buffer[1000];
		string file; vector<string> files;
		while(fgets(buffer, sizeof(buffer), pipe) != NULL)
		{
			file = buffer;
			files.push_back(file.substr(0, file.size() - 1));
		}
		pclose(pipe);

		return files;
	}

	RepoStatus RepoData::getCommitStatus() const
	{
		if(!runShInPath(path, "git diff-index --name-only --ignore-submodules HEAD --").empty()) return RepoStatus::CanCommit;
		if(!runShInPath(path, "git diff-index --name-only HEAD --").empty()) return RepoStatus::DirtySubmodules;
		return RepoStatus::None;
	}
	bool RepoData::getCanPush() const
	{
		return stoi(runShInPath(path, "git rev-list HEAD...origin/" + currentBranch + " --ignore-submodules --count")[0]) > 0;
	}
	bool RepoData::getCanPull() const
	{
		return !runShInPath(path, "git fetch --dry-run 2>&1").empty();
	}
	bool RepoData::getSubmodulesBehind() const
	{
		for(const auto& s : runShInPath(path, "git submodule foreach git fetch --dry-run 2>&1"))
		{
			if(startsWith(s, "Entering")) continue;
			return true;
		}

		return false;
	}

	vector<string> GitWs::getAllRepoPaths()
	{
		vector<string> result;
		for(const auto& rd : repoDatas) result.push_back(rd.path);
		return result;
	}
	vector<string> GitWs::getChangedRepoPaths()
	{
		vector<string> result;
		for(const auto& rd : repoDatas) if(rd.getCommitStatus() == RepoStatus::CanCommit) result.push_back(rd.path);
		return result;
	}
	vector<string> GitWs::getBehindSMRepoPaths()
	{
		vector<string> result;
		for(const auto& rd : repoDatas) if(rd.getSubmodulesBehind() || rd.getCommitStatus() == RepoStatus::DirtySubmodules) result.push_back(rd.path);
		return result;
	}
	vector<string> GitWs::getAheadRepoPaths()
	{
		vector<string> result;
		for(const auto& rd : repoDatas) if(rd.getCanPush()) result.push_back(rd.path);
		return result;
	}


	void GitWs::runShInRepos(const vector<string>& mRepoPaths, const string& mCommand, bool mPrintEmpty)
	{
		for(const auto& p : mRepoPaths)
		{
			vector<string> files{runShInPath(p, mCommand)};

			if(files.empty() && !mPrintEmpty) continue;

			lo << lt(p) << endl;
			for(auto& f : files) lo << ">" << f << endl;
			lo << lt("----") << endl << endl;
		}
	}

	void GitWs::initCmdHelp()
	{
		auto& cmd(cmdLine.create({"?", "help"}));
		cmd.setDescription("Show help for all commands or a single command.");

		auto& optArg(cmd.createOptArg<string>(""));
		optArg.setName("Command name");
		optArg.setBriefDescription("Name of the command to get help for.");
		optArg.setDescription("Leave blank to get general help.");

		auto& flagVerbose(cmd.createFlag("v", "verbose"));
		flagVerbose.setBriefDescription("Verbose general help?");

		cmd += [&]
		{
			if(!optArg)
			{
				lo << lt("git-ws help") << endl;
				lo << endl;

				if(!flagVerbose)
				{
					for(const auto& c : cmdLine.getCmds())
					{
						lo << (c->getNamesString() + " " + c->getArgsString() + " " + c->getOptArgsString() + " " + c->getFlagsString() + " " + c->getArgPacksString());
						lo << endl;
					}
				}
				else
				{
					for(const auto& c : cmdLine.getCmds())
					{
						lo << endl;
						lo << (c->getNamesString() + " " + c->getArgsString() + " " + c->getOptArgsString() + " " + c->getFlagsString() + " " + c->getArgPacksString());
						lo << endl;
						lo << (c->getHelpString());
					}
				}
			}

			auto& c(cmdLine.findCmd(optArg.get()));
			lo << endl;
			lo << (c.getNamesString() + " " + c.getArgsString() + " " + c.getOptArgsString() + " " + c.getFlagsString() + " " + c.getArgPacksString());
			lo << endl;
			lo << (c.getHelpString());
		};
	}
	void GitWs::initCmdPush()
	{
		auto& cmd(cmdLine.create({"push"}));
		cmd.setDescription("Pushes every git repo.");

		auto& flagForce(cmd.createFlag("f", "force"));
		flagForce.setBriefDescription("Forced push?");

		auto& flagAll(cmd.createFlag("a", "all"));
		flagAll.setBriefDescription("Run the command in all repos (even non-ahead ones), for all branches.");

		cmd += [&]
		{
			for(const auto& rd : repoDatas)
			{
				if(!flagAll && !rd.getCanPush()) continue;

				string toRun{"git push"};
				if(flagAll) toRun += " --all";
				if(flagForce) toRun += " -f";
				if(!flagAll) toRun += " origin " + rd.currentBranch;
				runShInPath(rd.path, toRun);
			}
		};
	}
	void GitWs::initCmdPull()
	{
		auto& cmd(cmdLine.create({"pull"}));
		cmd.setDescription("Pulls every git repo.");

		auto& flagStash(cmd.createFlag("s", "stash"));
		flagStash.setBriefDescription("Stash all changes before pulling?");

		auto& flagForce(cmd.createFlag("f", "force-checkout"));
		flagForce.setBriefDescription("Run a force checkout before pulling?");

		auto& flagChanged(cmd.createFlag("c", "changed-only"));
		flagChanged.setBriefDescription("Run the command only in folders where repos have changes?");

		cmd += [&]
		{
			auto currentRepoPaths(flagChanged ? getChangedRepoPaths() : getAllRepoPaths());
			if(flagStash) runShInRepos(currentRepoPaths, "git stash");
			if(flagForce) runShInRepos(currentRepoPaths, "git checkout -f");
			runShInRepos(currentRepoPaths, "git pull");
		};
	}
	void GitWs::initCmdSubmodule()
	{
		auto& cmd(cmdLine.create({"sub", "submodule"}));
		cmd.setDescription("Work with git submodules in every repo with dirty submodules.");

		auto& arg(cmd.createArg<string>());
		arg.setName("Action");
		arg.setBriefDescription("Action to run for every submodule. Can be 'pull' or 'au'.");
		arg.setDescription("'pull' recursively pulls the latest submodules from the remote, discarding any change.\n'au' calls 'pull' then pushes in succession.");

		auto& flagAll(cmd.createFlag("a", "all"));
		flagAll.setBriefDescription("Run the command in all repos?");

		cmd += [&]
		{
			auto currentRepoPaths(flagAll ? getAllRepoPaths() : getBehindSMRepoPaths());

			if(arg.get() == "pull" || arg.get() == "au")
			{
				runShInRepos(currentRepoPaths, "git submodule update --recursive --remote --init");
				runShInRepos(currentRepoPaths, "git submodule foreach git reset --hard");
				runShInRepos(currentRepoPaths, "git submodule foreach git checkout origin master");
				runShInRepos(currentRepoPaths, "git submodule foreach git rebase origin master");
				runShInRepos(currentRepoPaths, "git submodule foreach git pull -f origin master --recurse-submodules");
			}
			if(arg.get() == "au") runShInRepos(currentRepoPaths, "git commit -am 'automated submodule update'; git push");
		};
	}
	void GitWs::initCmdStatus()
	{
		auto& cmd(cmdLine.create({"st", "status"}));
		cmd.setDescription("Prints the status of all repos.");

		auto& showAllFlag(cmd.createFlag("a", "showall"));
		showAllFlag.setBriefDescription("Print empty messages?");

		cmd += [&]{ runShInRepos(getAllRepoPaths(), "git status -s --ignore-submodules=dirty", showAllFlag); };
	}
	void GitWs::initCmdGitg()
	{
		auto& cmd(cmdLine.create({"gitg"}));
		cmd.setDescription("Open the gitg gui application in every repo folder.");

		auto& flagChanged(cmd.createFlag("c", "changed-only"));
		flagChanged.setBriefDescription("Open gitg only in folders where repos have changes?");

		cmd += [&]
		{
			auto currentRepoPaths(flagChanged ? getChangedRepoPaths() : getAllRepoPaths());
			runShInRepos(currentRepoPaths, "gitg -c&");
		};
	}
	void GitWs::initCmdDo()
	{
		auto& cmd(cmdLine.create({"do"}));
		cmd.setDescription("Runs a shell command in every repo folder.");

		auto& arg(cmd.createArg<string>());
		arg.setName("Command to run");
		arg.setBriefDescription("This is the command that will be called in every repo folder.");
		arg.setDescription("Consider wrapping a more complex command with quotes.");

		auto& flagChanged(cmd.createFlag("c", "changed-only"));
		flagChanged.setBriefDescription("Run the command only in folders where repos have changes?");

		auto& flagAhead(cmd.createFlag("a", "ahead-only"));
		flagAhead.setBriefDescription("Run the command only in folders where repos are ahead of the remote?");

		cmd += [&]
		{
			if(flagChanged && flagAhead) { lo << "-c and -a are mutually exclusive" << endl; return; }

			auto currentRepoPaths(flagChanged ? getChangedRepoPaths() : getAllRepoPaths());
			if(flagAhead) currentRepoPaths = getAheadRepoPaths();

			runShInRepos(currentRepoPaths, arg.get());
		};
	}
	void GitWs::initCmdQuery()
	{
		auto& cmd(cmdLine.create({"query"}));
		cmd.setDescription("Queries the status of all the repos, returning whether they are changed or ahead.");
		cmd += [&]
		{
			for(const auto& rd : repoDatas)
			{
				cout << setw(25) << left << rd.path << setw(15) << " ~ " + rd.currentBranch;
				if(rd.getCommitStatus() == RepoStatus::CanCommit) cout << setw(15) << left << "(can commit)";
				if(rd.getCanPush()) cout << setw(15) << left << "(can push)";
				if(rd.getCanPull()) cout << setw(15) << left << "(can pull)";
				if(rd.getCommitStatus() == RepoStatus::DirtySubmodules) cout << setw(15) << left << "(dirty submodules)";
				if(rd.getSubmodulesBehind()) cout << setw(15) << left << "(outdated submodules)";
				cout << endl;
			}
		};
	}
	void GitWs::initRepoDatas() { for(auto& p : getScan<Mode::Single, Type::Folder>("./")) if(exists(p + "/.git/")) repoDatas.emplace_back(p, runShInPath(p, "git rev-parse --abbrev-ref HEAD")[0]); }
	void GitWs::initCmds() { initCmdHelp(); initCmdPush(); initCmdPull(); initCmdSubmodule(); initCmdStatus(); initCmdGitg(); initCmdDo(); initCmdQuery(); }

	GitWs::GitWs(const vector<string>& mCommandLine) { initRepoDatas(); initCmds(); cmdLine.parseCmdLine(mCommandLine); }
}
