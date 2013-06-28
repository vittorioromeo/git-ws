// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <cstdio>
#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <SSVUtils/SSVUtils.h>
#include "git-ws/CommandLine/CommandLine.h"

using namespace std;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvcl;

struct GitWs
{
	vector<string> repoPaths;
	vector<string> changedRepoPaths;
	CmdLine cmdLine;

	vector<string> runShInPath(const string& mPath, const string& mCommand)
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

	void runShInRepos(const vector<string>& mRepoPaths, const string& mCommand, bool mPrintEmpty = false)
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

	void initCmdHelp()
	{
		auto& cmd(cmdLine.create({"?", "help"}));
		auto& optArg(cmd.createOptArg<string>("", "command name"));
		cmd += [&]
		{
			if(!optArg)
			{
				log("Git-ws help");
				log("");

				for(const auto& c : cmdLine.getCmds())
				{
					log(c->getNamesString(), "Command help");
					log(c->getNamesString() + " " + c->getArgsString() + " " + c->getOptArgsString() + " " + c->getFlagsString());
					log("");
				}
			}

			auto& c(cmdLine.findCmd(optArg.get()));
			log(c.getNamesString(), "Command help");
			log(c.getNamesString() + " " + c.getArgsString() + " " + c.getOptArgsString() + " " + c.getFlagsString());
		};
	}
	void initCmdPush()
	{
		auto& cmd(cmdLine.create({"push"}));
		auto& flagForce(cmd.createFlag("f", "force"));
		auto& flagChanged(cmd.createFlag("c", "changed-only"));
		cmd += [&]
		{
			auto& currentRepoPaths(flagChanged ? changedRepoPaths : repoPaths);
			runShInRepos(currentRepoPaths, flagForce ? "git push -f" : "git push");
		};
	}
	void initCmdPull()
	{
		auto& cmd(cmdLine.create({"pull"}));
		auto& flagStash(cmd.createFlag("s", "stash"));
		auto& flagForce(cmd.createFlag("f", "force-checkout"));
		auto& flagChanged(cmd.createFlag("c", "changed-only"));
		cmd += [&]
		{
			auto& currentRepoPaths(flagChanged ? changedRepoPaths : repoPaths);
			if(flagStash) runShInRepos(currentRepoPaths, "git stash");
			if(flagForce) runShInRepos(currentRepoPaths, "git checkout -f");
			runShInRepos(currentRepoPaths, "git pull");
		};
	}
	void initCmdSubmodule()
	{
		auto& cmd(cmdLine.create({"sub", "submodule"}));
		auto& arg(cmd.createArg<string>("submodule action"));
		auto& flagChanged(cmd.createFlag("c", "changed-only"));
		cmd += [&]
		{
			auto& currentRepoPaths(flagChanged ? changedRepoPaths : repoPaths);
			if(arg.get() == "push") runShInRepos(currentRepoPaths, "git commit -am 'automated submodule update'; git push");
			else if(arg.get() == "pull") runShInRepos(currentRepoPaths, "git submodule foreach git stash; git submodule foreach git pull origin master --recurse-submodules");
			else if(arg.get() == "au")
			{
				runShInRepos(currentRepoPaths, "git submodule foreach git stash; git submodule foreach git pull origin master --recurse-submodules");
				runShInRepos(currentRepoPaths, "git commit -am 'automated submodule update'; git push");
			}
		};
	}
	void initCmdStatus()
	{
		auto& cmd(cmdLine.create({"st", "status"}));
		auto& showAllFlag(cmd.createFlag("a", "showall"));
		cmd += [&]{ runShInRepos(repoPaths, "git status -s --ignore-submodules=dirty", showAllFlag); };
	}
	void initCmdGitg()
	{
		auto& cmd(cmdLine.create({"gitg"}));
		auto& flagChanged(cmd.createFlag("c", "changed-only"));
		cmd += [&]
		{
			auto& currentRepoPaths(flagChanged ? changedRepoPaths : repoPaths);
			runShInRepos(currentRepoPaths, "gitg&");
		};
	}
	void initCmdDo()
	{
		auto& cmd(cmdLine.create({"do"}));
		auto& arg(cmd.createArg<string>("command to execute"));
		auto& flagChanged(cmd.createFlag("c", "changed-only"));
		cmd += [&]
		{
			auto& currentRepoPaths(flagChanged ? changedRepoPaths : repoPaths);
			runShInRepos(currentRepoPaths, arg.get());
		};
	}

	void initRepoPaths()
	{
		for(auto& p : getScan<Mode::Single, Type::Folder>("./"))
			if(exists(p + "/.git/"))
			{
				repoPaths.push_back(p);
				if(!runShInPath(p, "git diff-index --name-only --ignore-submodules HEAD --").empty()) changedRepoPaths.push_back(p);
			}
	}
	void initCmds() { initCmdHelp(); initCmdPush(); initCmdPull(); initCmdSubmodule(); initCmdStatus(); initCmdGitg(); initCmdDo(); }

	GitWs() { initRepoPaths(); initCmds(); }
};

int main(int argc, char* argv[])
{
	vector<string> args;
	for(int i{1}; i < argc; ++i) args.push_back(argv[i]);

	try{ GitWs{}.cmdLine.parseCmdLine(args); }
	catch(runtime_error mException) { log(mException.what()); return 1; }

	return 0;
}

