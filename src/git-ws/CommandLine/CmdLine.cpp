// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <deque>
#include <stdexcept>
#include <SSVUtils/SSVUtils.h>
#include "git-ws/CommandLine/Cmd.h"
#include "git-ws/CommandLine/Flag.h"
#include "git-ws/CommandLine/CmdLine.h"
#include "git-ws/CommandLine/ArgBase.h"
#include "git-ws/CommandLine/ArgPackBase.h"

using namespace std;
using namespace ssvu;

namespace ssvcl
{
	Cmd& CmdLine::findCmd(const string& mName) const
	{
		for(const auto& c : cmds) if(c->hasName(mName)) return *c;
		throw runtime_error("No command with name <" + mName + ">");
	}
	Cmd& CmdLine::create(const initializer_list<string>& mNames) { auto result(new Cmd{mNames}); cmds.push_back(result); return *result; }
	void CmdLine::parseCmdLine(const vector<string>& mArgs)
	{
		deque<string> args{begin(mArgs), end(mArgs)};

		Cmd& cmd(findCmd(args.front()));
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
		for(unsigned int i{cmd.getOptArgCount()}; i > 0; --i)
		{
			if(args.empty()) break;
			cOptArgs.push_back(args.front());
			if(cOptArgs.size() > cmd.getOptArgCount()) throw runtime_error("Incorrect number of optargs for command " + cmd.getNamesString() + " , correct number is '" + toStr(cmd.getOptArgCount()) + "'");
			args.pop_front();
		}

		// Check for argpacks
		for(unsigned int i{0}; i < cmd.getArgPackCount(); ++i)
		{
			auto& argPack(*cmd.getArgPacks()[i]);
			vector<string> toPack;

			if(argPack.isInfinite())
			{
				if(i != cmd.getArgPackCount() -1) throw runtime_error("Infinite argpacks must be last");
				while(!args.empty()) { toPack.push_back(args.front()); args.pop_front(); }
			}
			else
			{
				if(args.size() < argPack.getMin()) throw runtime_error("Not enough args for finite argpack");

				unsigned int clampedCount{getClamped<unsigned int>(args.size(), 0, argPack.getMax())};
				for(unsigned int iS{0}; iS < clampedCount; ++iS) { toPack.push_back(args.front()); args.pop_front(); }
			}

			argPack.set(toPack);
		}


		if(!args.empty()) throw runtime_error("Too many arguments!");

		for(unsigned int i{0}; i < cArgs.size(); ++i) cmd.setArgValue(i, cArgs[i]);
		for(unsigned int i{0}; i < cOptArgs.size(); ++i) cmd.setOptArgValue(i, cOptArgs[i]);
		for(const auto& f : cFlags) cmd.activateFlag(f);
		cmd();
	}
	const vector<Cmd*>& CmdLine::getCmds() const { return cmds; }
}
