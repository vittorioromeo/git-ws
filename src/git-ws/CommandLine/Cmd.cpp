// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <SSVUtils/SSVUtils.h>
#include "git-ws/CommandLine/ArgBase.h"
#include "git-ws/CommandLine/Arg.h"
#include "git-ws/CommandLine/Flag.h"
#include "git-ws/CommandLine/Cmd.h"

using namespace std;
using namespace ssvu;

namespace ssvcl
{
	Cmd::Cmd(initializer_list<string> mNames) : names{mNames} { }
	Cmd::~Cmd() { for(const auto& a : args) delete a; for(const auto& f : flags) delete f; }

	Flag& Cmd::findFlag(const string& mName)
	{
		for(const auto& f : flags) if(f->hasName(mName)) return *f;
		throw runtime_error("No flag with name '" + mName + "' in command " + getNamesString());
	}

	void Cmd::setArgValue(unsigned int mIndex, const string& mValue) { args[mIndex]->set(mValue); }
	unsigned int Cmd::getArgCount() { return args.size(); }

	Flag&Cmd::createFlag(const string& mShortName, const string& mLongName) { auto result(new Flag{mShortName, mLongName}); flags.push_back(result); return *result; }
	unsigned int Cmd::getFlagCount() { return flags.size(); }
	bool Cmd::isFlagActive(unsigned int mIndex) { return *flags[mIndex]; }
	void Cmd::activateFlag(const string& mName) { findFlag(mName) = true; }

	bool Cmd::hasName(const string& mName) { return contains(names, mName); }

	void Cmd::execute() { func(); }
	Cmd& Cmd::operator+=(std::function<void()> mFunc) { func = mFunc; return *this; }

	const vector<string>&Cmd::getNames() { return names; }
	const vector<ArgBase*>&Cmd::getArgs() { return args; }
	const vector<Flag*>&Cmd::getFlags() { return flags; }

	string Cmd::getNamesString()
	{
		string result{"<"};
		for(unsigned int i{0}; i < names.size(); ++i)
		{
			result.append(names[i]);
			if(i < names.size() - 1) result.append(" || ");
		}
		result.append(">");
		return result;
	}
	string Cmd::getArgsString()
	{
		string result;
		for(unsigned int i{0}; i < args.size(); ++i)
		{
			result.append(args[i]->getArgString());
			if(i < args.size() - 1) result.append(" ");
		}
		return result;
	}
	string Cmd::getFlagsString()
	{
		string result;
		for(unsigned int i{0}; i < flags.size(); ++i)
		{
			result.append(flags[i]->getFlagString());
			if(i < flags.size() - 1) result.append(" ");
		}
		return result;
	}
}
