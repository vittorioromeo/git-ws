// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <SSVUtils/SSVUtils.h>
#include "git-ws/CommandLine/ArgPackBase.h"

using namespace std;
using namespace ssvu;

namespace ssvcl
{
	ArgPackBase::ArgPackBase(const string& mDescription) : description{mDescription}, infinite{true} { }
	ArgPackBase::ArgPackBase(const string& mDescription, unsigned int mMin, unsigned int mMax) : description{mDescription}, infinite{false}, min{mMin}, max{mMax} { }
	ArgPackBase::~ArgPackBase() { }

	const string& ArgPackBase::getDescription() const { return description; }
	string ArgPackBase::getArgString() const { return "pack:<(" + description + ")" + "[" + toStr(min) + "/" + (infinite ? "+infinity" : toStr(max)) + "])>"; }

	bool ArgPackBase::isInfinite() const { return infinite; }
	unsigned int ArgPackBase::getMin() const { return min; }
	unsigned int ArgPackBase::getMax() const { return max; }
}

