// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include <string>
#include <vector>
#include <stdexcept>
#include <SSVUtils/SSVUtils.h>
#include "git-ws/GitWs/GitWs.h"

using namespace std;
using namespace ssvu;
using namespace gitws;

int main(int argc, char* argv[])
{
	vector<string> args; for(int i{1}; i < argc; ++i) args.emplace_back(argv[i]);

	try { GitWs{args}; }
	catch(runtime_error mException) { lo << mException.what() << endl; return 1; }

	saveLogToFile("log.txt");

	return 0;
}
