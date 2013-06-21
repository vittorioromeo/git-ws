#include "git-ws/CommandLine/Flag.h"

using namespace std;

namespace ssvcl
{
	Flag::Flag(const string& mShortName, const string& mLongName) : shortName{mShortName}, longName{mLongName} { }

	Flag& Flag::operator=(bool mActive) { active = mActive; return *this; }
	Flag::operator bool() const { return active; }

	bool Flag::hasName(const string& mName) { return mName == flagPrefixShort + shortName || mName == flagPrefixLong + longName; }

	const string&Flag::getShortName() { return shortName; }
	const string&Flag::getLongName() { return longName; }

	string Flag::getShortNameWithPrefix() { return flagPrefixShort + shortName; }
	string Flag::getLongNameWithPrefix() { return flagPrefixLong + longName; }
	string Flag::getFlagString() { return "[" + getShortNameWithPrefix() + " || " + getLongNameWithPrefix() + "]"; }
}
