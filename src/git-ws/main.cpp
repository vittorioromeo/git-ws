#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <SSVUtils/SSVUtils.h>
#include "git-ws/CommandLine/Parser.h"
#include "git-ws/CommandLine/ArgBase.h"
#include "git-ws/CommandLine/Arg.h"
#include "git-ws/CommandLine/Flag.h"
#include "git-ws/CommandLine/Cmd.h"
#include "git-ws/CommandLine/CmdLine.h"

using namespace std;
using namespace ssvu;
using namespace ssvu::FileSystem;
using namespace ssvcl;

vector<string> repoFolders;
CmdLine m;

void runCommandInRepos(const string& mCommand)
{
	for(const auto& f : repoFolders)
	{
		log("	" + f);
		system(string{"(cd " + f + ";" + mCommand + ")"}.c_str());
		log("");
	}
}

void initHelp()
{
	auto& helpCommand(m.create({"?", "help"}));
	auto& argCommandName(helpCommand.createArg<string>("command name"));
	helpCommand.setFunc([&]
	{
		auto& command(m.findCommand(argCommandName.get()));
		log("Command " + command.getNamesString() + " usage:");
		log(command.getNamesString() + " " + command.getArgsString() + " " + command.getFlagsString());
	});
}

void initPush()
{
	auto& pushCmd(m.create({"push"}));
	auto& forceFlag(pushCmd.createFlag("f", "force"));
	pushCmd.setFunc([&]
	{
		runCommandInRepos(forceFlag.isActive() ? "git push -f" : "git push");
	});
}

void initPull()
{
	auto& pullCmd(m.create({"pull"}));
	auto& stashFlag(pullCmd.createFlag("s", "stash"));
	pullCmd.setFunc([&]
	{
		runCommandInRepos(stashFlag.isActive() ? "git stash; git pull" : "git pull");
	});
}

void initStatus()
{
	m.create({"status"}).setFunc([&]{ runCommandInRepos("git status -s --ignore-submodules=dirty"); });
}

void initSubmodule()
{
	auto& submoduleCmd(m.create({"sub", "submodule"}));
	auto& arg(submoduleCmd.createArg<string>("submodule action"));
	submoduleCmd.setFunc([&]
	{
		if(arg.get() == "push") runCommandInRepos("git commit -am 'automated submodule update'; git push");
		else if(arg.get() == "pull") runCommandInRepos("git submodule foreach git stash; git submodule foreach git pull origin master --recurse-submodules");
		else if(arg.get() == "au")
		{
			runCommandInRepos("git submodule foreach git stash; git submodule foreach git pull origin master --recurse-submodules");
			runCommandInRepos("git commit -am 'automated submodule update'; git push");
			runCommandInRepos("git submodule update");
		}
	});
}

void initGitg()
{
	m.create({"gitg"}).setFunc([&]{ runCommandInRepos("gitg&"); });
}

void initCommands() { initHelp(); initPush(); initPull(); initStatus(); initSubmodule(); initGitg(); }

int main(int argc, char* argv[])
{
	for(auto& cf : getScan<Mode::Single, Type::Folder>("./")) if(exists(cf + "/.git/")) repoFolders.push_back(cf);

	initCommands();

	vector<string> args;
	for(int i{1}; i < argc; ++i) args.push_back(toStr(argv[i]));

	try{ m.parseCommandLine(args); }
	catch(runtime_error mException) { log(mException.what()); return 1; }

	return 0;
}

