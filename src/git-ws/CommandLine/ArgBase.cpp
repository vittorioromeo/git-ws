#include "git-ws/CommandLine/ArgBase.h"

using namespace std;

namespace ssvcl
{
	ArgBase::ArgBase(const string& mDescription) : description{mDescription} { }
	ArgBase::~ArgBase() { }
	const string& ArgBase::getDescription() { return description; }
	string ArgBase::getArgString() { return "(" + description + ")"; }
}

