// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <deque>
#include <SSVUtils/SSVUtils.h>
#include "git-ws/CommandLine/Cmd.h"
#include "git-ws/CommandLine/Flag.h"
#include "git-ws/CommandLine/CmdLine.h"

using namespace std;
using namespace ssvu;

namespace ssvcl
{
	Cmd& CmdLine::findCommand(const string& mName)
	{
		for(const auto& c : commands) if(c->hasName(mName)) return *c;
		throw runtime_error("No command with name <" + mName + ">");
	}
	Cmd& CmdLine::create(initializer_list<string> mNames) { auto result(new Cmd{mNames}); commands.push_back(result); return *result; }
	void CmdLine::parseCommandLine(const vector<string>& mArgs)
	{
		deque<string> args{begin(mArgs), end(mArgs)};

		Cmd& cmd{findCommand(args.front())};
		args.pop_front();

		// Find all flags, put them in cFlags, remove them from mArgs
		vector<string> cFlags;
		for(const auto& s : args)
		{
			if(startsWith(s, flagPrefixShort) || startsWith(s, flagPrefixLong))
			{
				cFlags.push_back(s);
				if(cFlags.size() > cmd.getFlagCount()) throw runtime_error("Incorrect number of flags for command " + cmd.getNamesString() + " , correct number is '" + toStr(cmd.getFlagCount()) + "'");
			}
		}
		for(const auto& f : cFlags) eraseRemove(args, f);

		// Find args, put them in cArgs
		vector<string> cArgs;
		for(unsigned int i{cmd.getArgCount()}; i > 0; --i)
		{
			if(args.empty()) throw runtime_error("Incorrect number of args for command " + cmd.getNamesString() + " , correct number is '" + toStr(cmd.getArgCount()) + "'");
			cArgs.push_back(args.front());
			args.pop_front();
		}

		// Remaining string in args must be optargs
		vector<string> cOptArgs;
		while(!args.empty())
		{
			cOptArgs.push_back(args.front());
			if(cOptArgs.size() > cmd.getOptArgCount()) throw runtime_error("Incorrect number of optargs for command " + cmd.getNamesString() + " , correct number is '" + toStr(cmd.getOptArgCount()) + "'");
			args.pop_front();
		}

		for(unsigned int i{0}; i < cArgs.size(); ++i) cmd.setArgValue(i, cArgs[i]);
		for(unsigned int i{0}; i < cOptArgs.size(); ++i) cmd.setOptArgValue(i, cOptArgs[i]);
		for(const auto& f : cFlags) cmd.activateFlag(f);
		cmd.execute();
	}
}
