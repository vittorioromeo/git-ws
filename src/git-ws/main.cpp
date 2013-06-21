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
	CmdLine cmdline;

	void runShInRepos(const string& mCommand)
	{
		for(const auto& p : repoPaths)
		{
			log("<<" + p + ">>", "Repo");

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

			for(auto& f : files) log(f, ">");
			log("", "----");
			log("");
		}
	}

	void initCmdHelp()
	{
		auto& cmd(cmdline.create({"?", "help"}));
		auto& arg(cmd.createArg<string>("command name"));
		cmd += [&]
		{
			auto& c(cmdline.findCommand(arg.get()));
			log(c.getNamesString(), "Command help");
			log(c.getNamesString() + " " + c.getArgsString() + " " + c.getFlagsString());
		};
	}
	void initCmdPush()
	{
		auto& cmd(cmdline.create({"push"}));
		auto& flagForce(cmd.createFlag("f", "force"));
		cmd += [&]{ runShInRepos(flagForce ? "git push -f" : "git push"); };
	}
	void initCmdPull()
	{
		auto& cmd(cmdline.create({"pull"}));
		auto& flagStash(cmd.createFlag("s", "stash"));
		cmd += [&]{ runShInRepos(flagStash ? "git stash; git pull" : "git pull"); };
	}
	void initCmdSubmodule()
	{
		auto& cmd(cmdline.create({"sub", "submodule"}));
		auto& arg(cmd.createArg<string>("submodule action"));
		cmd += [&]
		{
			if(arg.get() == "push") runShInRepos("git commit -am 'automated submodule update'; git push");
			else if(arg.get() == "pull") runShInRepos("git submodule foreach git stash; git submodule foreach git pull origin master --recurse-submodules");
			else if(arg.get() == "au")
			{
				runShInRepos("git submodule foreach git stash; git submodule foreach git pull origin master --recurse-submodules");
				runShInRepos("git commit -am 'automated submodule update'; git push");
				runShInRepos("git submodule update");
			}
		};
	}
	void initCmdStatus() { cmdline.create({"status"}) += [&]{ runShInRepos("git status -s --ignore-submodules=dirty"); }; }
	void initCmdGitg() { cmdline.create({"gitg"}) += [&]{ runShInRepos("gitg&"); }; }

	void initRepoPaths() { for(auto& p : getScan<Mode::Single, Type::Folder>("./")) if(exists(p + "/.git/")) repoPaths.push_back(p); }
	void initCmds() { initCmdHelp(); initCmdPush(); initCmdPull(); initCmdSubmodule(); initCmdStatus(); initCmdGitg(); }

	GitWs() { initRepoPaths(); initCmds(); }
};

int main(int argc, char* argv[])
{
	vector<string> args;
	for(int i{1}; i < argc; ++i) args.push_back(toStr(argv[i]));

	try{ GitWs{}.cmdline.parseCommandLine(args); }
	catch(runtime_error mException) { log(mException.what()); return 1; }

	return 0;
}

