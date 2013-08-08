// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <deque>
#include <stdexcept>
#include <limits>
#include <SSVUtils/SSVUtils.h>
#include "git-ws/CommandLine/Cmd.h"
#include "git-ws/CommandLine/Elements/Flag.h"
#include "git-ws/CommandLine/CmdLine.h"
#include "git-ws/CommandLine/Elements/Bases/ArgBase.h"
#include "git-ws/CommandLine/Elements/Bases/ArgPackBase.h"

using namespace std;
using namespace ssvu;

namespace ssvcl
{
	Cmd& CmdLine::findCmd(const string& mName) const
	{
		for(const auto& c : cmds) if(c->hasName(mName)) return *c;

		pair<unsigned int, string> closestMatch{numeric_limits<unsigned int>::max(), ""};

		for(const auto& c : cmds)
			for(const auto& n : c->getNames())
			{
				const auto& dist(getLevenshteinDistance(n, mName));
				if(dist < closestMatch.first) closestMatch = {dist, n};
			}

		throw runtime_error("No command with name <" + mName + ">\nDid you mean <" + closestMatch.second + ">?");
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
		for(auto i(cmd.getArgCount()); i > 0; --i)
		{
			if(args.empty()) throw runtime_error("Incorrect number of args for command " + cmd.getNamesString() + " , correct number is '" + toStr(cmd.getArgCount()) + "'");
			cArgs.push_back(args.front());
			args.pop_front();
		}

		// Remaining string in args must be optargs
		vector<string> cOptArgs;
		for(auto i(cmd.getOptArgCount()); i > 0; --i)
		{
			if(args.empty()) break;
			cOptArgs.push_back(args.front());
			if(cOptArgs.size() > cmd.getOptArgCount()) throw runtime_error("Incorrect number of optargs for command " + cmd.getNamesString() + " , correct number is '" + toStr(cmd.getOptArgCount()) + "'");
			args.pop_front();
		}

		// Check for argpacks
		for(auto i(0u); i < cmd.getArgPackCount(); ++i)
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
				for(auto iS(0u); iS < clampedCount; ++iS) { toPack.push_back(args.front()); args.pop_front(); }
			}

			argPack.set(toPack);
		}


		if(!args.empty()) throw runtime_error("Too many arguments!");

		for(auto i(0u); i < cArgs.size(); ++i) cmd.setArgValue(i, cArgs[i]);
		for(auto i(0u); i < cOptArgs.size(); ++i) cmd.setOptArgValue(i, cOptArgs[i]);
		for(const auto& f : cFlags) cmd.activateFlag(f);
		cmd();
	}
	const vector<Cmd*>& CmdLine::getCmds() const { return cmds; }
}
