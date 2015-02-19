// Copyright (c) 2013-2015 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#include "git-ws/GitWs/GitWs.hpp"

int main(int argc, char* argv[])
{
	try
	{
		gitws::GitWs{argc, argv};
	}
	catch(const ssvucl::Exception::Base& mEx)
	{
		ssvu::lo(mEx.getTitle()) << mEx.what() << std::endl;
		return 1;
	}
	catch(const std::runtime_error& mEx)
	{
		ssvu::lo("Runtime error") << mEx.what() << std::endl;
		return 1;
	}
	catch(...)
	{
		return 1;
	}

	return 0;
}
