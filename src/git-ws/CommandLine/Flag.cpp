// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "git-ws/CommandLine/Elements/Flag.h"

using namespace std;

namespace ssvcl
{
	Flag::Flag(const string& mShortName, const string& mLongName) : shortName{mShortName}, longName{mLongName} { }

	Flag& Flag::operator=(bool mActive) { active = mActive; return *this; }
	Flag::operator bool() const { return active; }

	bool Flag::hasName(const string& mName) const { return mName == getShortNameWithPrefix() || mName == getLongNameWithPrefix(); }

	const string& Flag::getShortName() const	{ return shortName; }
	const string& Flag::getLongName() const		{ return longName; }
	string Flag::getShortNameWithPrefix() const	{ return flagPrefixShort + shortName; }
	string Flag::getLongNameWithPrefix() const	{ return flagPrefixLong + longName; }
	string Flag::getFlagString() const			{ return "[" + getShortNameWithPrefix() + " || " + getLongNameWithPrefix() + "]"; }
}
