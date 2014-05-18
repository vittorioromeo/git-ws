// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "git-ws/GitWs/GitWs.hpp"

int main(int argc, char* argv[])
{
	std::vector<std::string> args; for(int i{1}; i < argc; ++i) args.emplace_back(argv[i]);
	try { gitws::GitWs{args}; } catch(std::runtime_error mException) { ssvu::lo() << mException.what() << std::endl; return 1; }
	ssvu::saveLogToFile("log.txt"); return 0;
}
