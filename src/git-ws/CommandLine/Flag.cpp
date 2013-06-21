#include "git-ws/CommandLine/Flag.h"

using namespace std;

namespace ssvcl
{
	Flag::Flag(const string& mShortName, const string& mLongName) : shortName{mShortName}, longName{mLongName} { }

	void Flag::setActive(bool mActive) { active = mActive; }
	bool Flag::isActive() { return active; }
	bool Flag::hasName(const string& mName) { return mName == flagPrefixShort + shortName || mName == flagPrefixLong + longName; }

	const string&Flag::getShortName() { return shortName; }
	const string&Flag::getLongName() { return longName; }

	string Flag::getShortNameWithPrefix() { return flagPrefixShort + shortName; }
	string Flag::getLongNameWithPrefix() { return flagPrefixLong + longName; }
	string Flag::getFlagString() { return "[" + getShortNameWithPrefix() + " || " + getLongNameWithPrefix() + "]"; }
}
