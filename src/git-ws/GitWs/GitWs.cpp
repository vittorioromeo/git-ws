// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <cstdio>
#include <iostream>
#include "git-ws/GitWs/GitWs.h"

using namespace std;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvcl;

namespace gitws
{
	vector<string> GitWs::getAheadRepoPaths()
	{
		vector<string> result;
		for(const auto& p : repoPaths)
		{
			bool isAhead{false};
			for(auto& f : runShInPath(p, "git status -sb")) if(f.find("ahead") != string::npos) { isAhead = true; break; }
			if(isAhead) result.push_back(p);
		}
		return result;
	}
	vector<string> GitWs::getChangedRepoPaths()
	{
		vector<string> result;
		for(const auto& p : repoPaths) if(!runShInPath(p, "git diff-index --name-only --ignore-submodules HEAD --").empty()) result.push_back(p);
		return result;
	}

	vector<string> GitWs::runShInPath(const string& mPath, const string& mCommand)
	{
		FILE* pipe{popen(string{"(cd " + mPath + ";" + mCommand + ")"}.c_str(), "r")};
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
	void GitWs::runShInRepos(const vector<string>& mRepoPaths, const string& mCommand, bool mPrintEmpty)
	{
		for(const auto& p : mRepoPaths)
		{
			vector<string> files{runShInPath(p, mCommand)};

			if(files.empty() && !mPrintEmpty) continue;

			log("<<" + p + ">>", "Repo");
			for(auto& f : files) log(f, ">");
			log("", "----"); log("");
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
				log("Git-ws help");
				log("");

				if(!flagVerbose)
				{
					for(const auto& c : cmdLine.getCmds())
					{
						log(c->getNamesString() + " " + c->getArgsString() + " " + c->getOptArgsString() + " " + c->getFlagsString() + " " + c->getArgPacksString());
						log("");
					}
				}
				else
				{
					for(const auto& c : cmdLine.getCmds())
					{
						log("");
						log(c->getNamesString() + " " + c->getArgsString() + " " + c->getOptArgsString() + " " + c->getFlagsString() + " " + c->getArgPacksString());
						log("");
						log(c->getHelpString());
					}
				}
			}

			auto& c(cmdLine.findCmd(optArg.get()));
			log("");
			log(c.getNamesString() + " " + c.getArgsString() + " " + c.getOptArgsString() + " " + c.getFlagsString() + " " + c.getArgPacksString());
			log("");
			log(c.getHelpString());
		};
	}
	void GitWs::initCmdPush()
	{
		auto& cmd(cmdLine.create({"push"}));
		cmd.setDescription("Pushes every git repo.");

		auto& flagForce(cmd.createFlag("f", "force"));
		flagForce.setBriefDescription("Forced pull?");

		auto& flagAhead(cmd.createFlag("a", "ahead-only"));
		flagAhead.setBriefDescription("Run the command only in folders where repos are ahead of the remote?");

		cmd += [&]
		{
			runShInRepos(flagAhead ? getAheadRepoPaths() : repoPaths, flagForce ? "git push --all -f" : "git push --all");
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
			auto currentRepoPaths(flagChanged ? getChangedRepoPaths() : repoPaths);
			if(flagStash) runShInRepos(currentRepoPaths, "git stash");
			if(flagForce) runShInRepos(currentRepoPaths, "git checkout -f");
			runShInRepos(currentRepoPaths, "git pull");
		};
	}
	void GitWs::initCmdSubmodule()
	{
		auto& cmd(cmdLine.create({"sub", "submodule"}));
		cmd.setDescription("Work with git submodules in every repo.");

		auto& arg(cmd.createArg<string>());
		arg.setName("Action");
		arg.setBriefDescription("Action to run for every submodule. Can be 'push', 'pull' or 'au'.");
		arg.setDescription("'push' commits all changes in the repo and pushes them to the remote. Do not run this unless all non-submodule changes have been taken care of!\n'pull' recursively pulls the latest submodules from the remote.\n'au' calls 'pull' then 'push' in succession.");

		auto& flagChanged(cmd.createFlag("c", "changed-only"));
		flagChanged.setBriefDescription("Run the command only in folders where repos have changes?");

		cmd += [&]
		{
			auto currentRepoPaths(flagChanged ? getChangedRepoPaths() : repoPaths);
			if(arg.get() == "push") runShInRepos(currentRepoPaths, "git commit -am 'automated submodule update'; git push");
			else if(arg.get() == "pull") runShInRepos(currentRepoPaths, "git submodule foreach git stash; git submodule foreach git pull origin master --recurse-submodules");
			else if(arg.get() == "au")
			{
				runShInRepos(currentRepoPaths, "git submodule foreach git stash; git submodule foreach git pull origin master --recurse-submodules");
				runShInRepos(currentRepoPaths, "git commit -am 'automated submodule update'; git push");
			}
		};
	}
	void GitWs::initCmdStatus()
	{
		auto& cmd(cmdLine.create({"st", "status"}));
		cmd.setDescription("Prints the status of all repos.");

		auto& showAllFlag(cmd.createFlag("a", "showall"));
		showAllFlag.setBriefDescription("Print empty messages?");

		cmd += [&]{ runShInRepos(repoPaths, "git status -s --ignore-submodules=dirty", showAllFlag); };
	}
	void GitWs::initCmdGitg()
	{
		auto& cmd(cmdLine.create({"gitg"}));
		cmd.setDescription("Open the gitg gui application in every repo folder.");

		auto& flagChanged(cmd.createFlag("c", "changed-only"));
		flagChanged.setBriefDescription("Open gitg only in folders where repos have changes?");

		cmd += [&]
		{
			auto currentRepoPaths(flagChanged ? getChangedRepoPaths() : repoPaths);
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
			if(flagChanged && flagAhead) { log("-c and -a are mutually exclusive"); return; }

			auto currentRepoPaths(flagChanged ? getChangedRepoPaths() : repoPaths);
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
			log("<<ALL REPO PATHS>>", "----");					for(const auto& p : repoPaths) log(p);				log("", "----"); log(""); log("");
			log("<<CHANGED REPO PATHS (can commit)>>", "----");	for(const auto& p : getChangedRepoPaths()) log(p);	log("", "----"); log(""); log("");
			log("<<AHEAD REPO PATHS (can push)>>", "----");		for(const auto& p : getAheadRepoPaths()) log(p);	log("", "----"); log(""); log("");
		};
	}
	void GitWs::initCmdVarTest()
	{
		auto& cmd(cmdLine.create({"vartest"}));
		auto& argPack(cmd.createFiniteArgPack<int>(2, 5));
		argPack.setName("Numbers to add");
		argPack.setBriefDescription("This is an argument pack, min2 max5, of numbers to add.");
		argPack.setDescription(R"(ites of the word in classical literature, discovered the undoubtable source. Lorem Ipsum comes from sections 1.10.32 a)");

		auto& argPack2(cmd.createFiniteArgPack<int>(1, 3));
		auto& argPack3(cmd.createInfiniteArgPack<int>());
		cmd += [&]
		{
			int result = 0;
			int result2 = 1;
			int result3 = 0;
			for(auto& i : argPack) result += i;
			for(auto& i : argPack2) result2 *= i;
			for(auto& i : argPack3) result3 -= i;
			log(toStr(result) + " " + toStr(result2) + " " + toStr(result3));
		};
	}

	void GitWs::initRepoPaths()
	{
		for(auto& p : getScan<Mode::Single, Type::Folder>("./")) if(exists(p + "/.git/")) repoPaths.push_back(p);
	}
	void GitWs::initCmds() { initCmdHelp(); initCmdPush(); initCmdPull(); initCmdSubmodule(); initCmdStatus(); initCmdGitg(); initCmdDo(); initCmdQuery(); /*initCmdVarTest();*/ }

	GitWs::GitWs(const vector<string>& mCommandLine) { initRepoPaths(); initCmds(); cmdLine.parseCmdLine(mCommandLine); }
}
