// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "git-ws/GitWs/GitWs.hpp"

int main(int argc, char* argv[])
{
	try { gitws::GitWs{argc, argv}; } catch(const std::runtime_error& mEx) { ssvu::lo() << mEx.what() << std::endl; return 1; }
	ssvu::saveLogToFile("log.txt"); return 0;
}
