// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

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
		const string& cmdName{mArgs[0]};
		Cmd& cmd{findCommand(cmdName)};

		vector<string> cArgs, cFlags;
		for(unsigned int i{1}; i < mArgs.size(); ++i)
		{
			const string& s{mArgs[i]};
			(startsWith(s, flagPrefixShort) || startsWith(s, flagPrefixLong)) ? cFlags.push_back(s) : cArgs.push_back(s);
		}

		if(cArgs.size() != cmd.getArgCount()) throw runtime_error("Incorrect number of arguments for command " + cmd.getNamesString() + " , correct number is '" + ssvu::toStr(cmd.getArgCount()) + "'");

		for(unsigned int i{0}; i < cArgs.size(); ++i) cmd.setArgValue(i, cArgs[i]);
		for(const auto& f : cFlags) cmd.activateFlag(f);
		cmd.execute();
	}
}
