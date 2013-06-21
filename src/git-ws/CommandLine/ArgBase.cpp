// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "git-ws/CommandLine/ArgBase.h"

using namespace std;

namespace ssvcl
{
	ArgBase::ArgBase(const string& mDescription) : description{mDescription} { }
	ArgBase::~ArgBase() { }
	const string& ArgBase::getDescription() { return description; }
	string ArgBase::getArgString() { return "(" + description + ")"; }
}

