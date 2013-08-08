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

	Cmd& Cmd::operator()() { func(); return *this; }
	Cmd& Cmd::operator+=(std::function<void()> mFunc) { func = mFunc; return *this; }

	Flag& Cmd::createFlag(const string& mShortName, const string& mLongName) { auto result(new Flag{mShortName, mLongName}); flags.push_back(unique_ptr<Flag>(result)); return *result; }

	void Cmd::setArgValue(unsigned int mIndex, const string& mValue)	{ args[mIndex]->set(mValue); }
	void Cmd::setOptArgValue(unsigned int mIndex, const string& mValue)	{ optArgs[mIndex]->set(mValue); }

	void Cmd::activateFlag(const string& mName) { findFlag(mName) = true; }

	bool Cmd::hasName(const string& mName) const		{ return contains(names, mName); }
	bool Cmd::isFlagActive(unsigned int mIndex) const	{ return *flags[mIndex]; }

	string Cmd::getNamesString() const
	{
		string result{"<"};
		for(auto i(0u); i < names.size(); ++i)
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
		for(auto i(0u); i < args.size(); ++i)
		{
			result.append(args[i]->getUsageString());
			if(i < args.size() - 1) result.append(" ");
		}
		return result;
	}
	string Cmd::getOptArgsString() const
	{
		string result;
		for(auto i(0u); i < optArgs.size(); ++i)
		{
			result.append(optArgs[i]->getUsageString());
			if(i < optArgs.size() - 1) result.append(" ");
		}
		return result;
	}
	string Cmd::getArgPacksString() const
	{
		string result;
		for(auto i(0u); i < argPacks.size(); ++i)
		{
			result.append(argPacks[i]->getUsageString());
			if(i < argPacks.size() - 1) result.append(" ");
		}
		return result;
	}
	string Cmd::getFlagsString() const
	{
		string result;
		for(auto i(0u); i < flags.size(); ++i)
		{
			result.append(flags[i]->getUsageString());
			if(i < flags.size() - 1) result.append(" ");
		}
		return result;
	}

	string Cmd::getHelpString() const
	{
		string result;

		if(!description.empty())
		{
			result += ">>" + description;
			result += "\n\n";
		}

		if(!args.empty()) result += "\t" "Required arguments:" "\n";
		for(const auto& a : args) result += a->getHelpString();
		if(!args.empty()) result += "\n";

		if(!optArgs.empty()) result += "\t" "Optional arguments:" "\n";
		for(const auto& a : optArgs) result += a->getHelpString();
		if(!optArgs.empty()) result += "\n";

		if(!argPacks.empty()) result += "\t" "Argument packs:" "\n";
		for(const auto& p : argPacks) result += p->getHelpString();
		if(!argPacks.empty()) result += "\n";

		if(!flags.empty()) result += "\t" "Flags:" "\n";
		for(const auto& f : flags) result += f->getHelpString();

		return result;
	}
}
