// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <SSVUtils/SSVUtils.h>
#include "git-ws/CommandLine/Elements/Bases/ArgBase.h"
#include "git-ws/CommandLine/Elements/Bases/ArgPackBase.h"
#include "git-ws/CommandLine/Elements/Arg.h"
#include "git-ws/CommandLine/Elements/Flag.h"
#include "git-ws/CommandLine/Cmd.h"

using namespace std;
using namespace ssvu;

namespace ssvcl
{
	Flag& Cmd::findFlag(const string& mName)
	{
		for(const auto& f : flags) if(f->hasName(mName)) return *f;
		throw runtime_error("No flag with name '" + mName + "' in command " + getNamesString());
	}

	Cmd::Cmd(const initializer_list<string>& mNames) : names{mNames} { }
	Cmd::~Cmd() { for(const auto& a : args) delete a; for(const auto& a : optArgs) delete a; for(const auto& f : flags) delete f; }

	Cmd& Cmd::operator()() { func(); return *this; }
	Cmd& Cmd::operator+=(std::function<void()> mFunc) { func = mFunc; return *this; }

	Flag& Cmd::createFlag(const string& mShortName, const string& mLongName) { auto result(new Flag{mShortName, mLongName}); flags.push_back(result); return *result; }

	void Cmd::setArgValue(unsigned int mIndex, const string& mValue)	{ args[mIndex]->set(mValue); }
	void Cmd::setOptArgValue(unsigned int mIndex, const string& mValue)	{ optArgs[mIndex]->set(mValue); }

	void Cmd::activateFlag(const string& mName) { findFlag(mName) = true; }

	bool Cmd::hasName(const string& mName) const		{ return contains(names, mName); }
	bool Cmd::isFlagActive(unsigned int mIndex) const	{ return *flags[mIndex]; }

	unsigned int Cmd::getArgCount() const					{ return args.size(); }
	unsigned int Cmd::getOptArgCount() const				{ return optArgs.size(); }
	unsigned int Cmd::getArgPackCount() const				{ return argPacks.size(); }
	unsigned int Cmd::getFlagCount() const					{ return flags.size(); }
	const vector<string>& Cmd::getNames() const				{ return names; }
	const vector<ArgBase*>& Cmd::getArgs() const			{ return args; }
	const vector<ArgBase*>& Cmd::getOptArgs() const			{ return optArgs; }
	const vector<ArgPackBase*>& Cmd::getArgPacks() const	{ return argPacks; }
	const vector<Flag*>& Cmd::getFlags() const				{ return flags; }

	string Cmd::getNamesString() const
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
	string Cmd::getArgsString() const
	{
		string result;
		for(unsigned int i{0}; i < args.size(); ++i)
		{
			result.append(args[i]->getUsageString());
			if(i < args.size() - 1) result.append(" ");
		}
		return result;
	}
	string Cmd::getOptArgsString() const
	{
		string result;
		for(unsigned int i{0}; i < optArgs.size(); ++i)
		{
			result.append("optional:" + optArgs[i]->getUsageString());
			if(i < optArgs.size() - 1) result.append(" ");
		}
		return result;
	}
	string Cmd::getArgPacksString() const
	{
		string result;
		for(unsigned int i{0}; i < argPacks.size(); ++i)
		{
			result.append(argPacks[i]->getUsageString());
			if(i < argPacks.size() - 1) result.append(" ");
		}
		return result;
	}
	string Cmd::getFlagsString() const
	{
		string result;
		for(unsigned int i{0}; i < flags.size(); ++i)
		{
			result.append(flags[i]->getUsageString());
			if(i < flags.size() - 1) result.append(" ");
		}
		return result;
	}
}
