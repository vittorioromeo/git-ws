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
	CmdLine cmdLine;

	void runShInRepos(const string& mCommand, bool mPrintEmpty = false)
	{
		for(const auto& p : repoPaths)
		{
			FILE* pipe{popen(string{"(cd " + p + ";" + mCommand + ")"}.c_str(), "r")};
			char buffer[1000];
			string file;
			vector<string> files;
			while(fgets(buffer, sizeof(buffer), pipe) != NULL)
			{
				file = buffer;
				files.push_back(file.substr(0, file.size() - 1));
			}
			pclose(pipe);

			if(files.empty() && !mPrintEmpty) continue;

			log("<<" + p + ">>", "Repo");
			for(auto& f : files) log(f, ">");
			log("", "----"); log("");
		}
	}

	void initCmdHelp()
	{
		auto& cmd(cmdLine.create({"?", "help"}));
		auto& arg(cmd.createArg<string>("command name"));
		cmd += [&]
		{
			auto& c(cmdLine.findCommand(arg.get()));
			log(c.getNamesString(), "Command help");
			log(c.getNamesString() + " " + c.getArgsString() + " " + c.getOptArgsString() + " " + c.getFlagsString());
		};
	}
	void initCmdPush()
	{
		auto& cmd(cmdLine.create({"push"}));
		auto& flagForce(cmd.createFlag("f", "force"));
		cmd += [&]{ runShInRepos(flagForce ? "git push -f" : "git push"); };
	}
	void initCmdPull()
	{
		auto& cmd(cmdLine.create({"pull"}));
		auto& flagStash(cmd.createFlag("s", "stash"));
		auto& flagForce(cmd.createFlag("f", "force-checkout"));
		cmd += [&]
		{
			if(flagStash) runShInRepos("git stash");
			if(flagForce) runShInRepos("git checkout -f");
			runShInRepos("git pull");
		};
	}
	void initCmdSubmodule()
	{
		auto& cmd(cmdLine.create({"sub", "submodule"}));
		auto& arg(cmd.createArg<string>("submodule action"));
		cmd += [&]
		{
			if(arg.get() == "push") runShInRepos("git commit -am 'automated submodule update'; git push");
			else if(arg.get() == "pull") runShInRepos("git submodule foreach git stash; git submodule foreach git pull origin master --recurse-submodules");
			else if(arg.get() == "au")
			{
				runShInRepos("git submodule foreach git stash; git submodule foreach git pull origin master --recurse-submodules");
				runShInRepos("git commit -am 'automated submodule update'; git push");
			}
		};
	}
	void initCmdStatus()
	{
		auto& cmd(cmdLine.create({"st", "status"}));
		auto& showAllFlag(cmd.createFlag("a", "showall"));
		cmd += [&]{ runShInRepos("git status -s --ignore-submodules=dirty", showAllFlag); };
	}
	void initCmdGitg() { cmdLine.create({"gitg"}) += [&]{ runShInRepos("gitg&"); }; }
	void initCmdDo()
	{
		auto& cmd(cmdLine.create({"do"}));
		auto& arg(cmd.createArg<string>("command to execute"));
		cmd += [&]{ runShInRepos(arg.get()); };
	}

	void initRepoPaths() { for(auto& p : getScan<Mode::Single, Type::Folder>("./")) if(exists(p + "/.git/")) repoPaths.push_back(p); }
	void initCmds() { initCmdHelp(); initCmdPush(); initCmdPull(); initCmdSubmodule(); initCmdStatus(); initCmdGitg(); initCmdDo(); }

	GitWs() { initRepoPaths(); initCmds(); }
};

int main(int argc, char* argv[])
{
	vector<string> args;
	for(int i{1}; i < argc; ++i) args.push_back(argv[i]);

	try{ GitWs{}.cmdLine.parseCommandLine(args); }
	catch(runtime_error mException) { log(mException.what()); return 1; }

	return 0;
}

