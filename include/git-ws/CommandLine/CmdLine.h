// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef GITWS_COMMANDLINE_CMDLINE
#define GITWS_COMMANDLINE_CMDLINE

#include <string>
#include <vector>

namespace ssvcl
{
	class Cmd;

	class CmdLine
	{
		private:
			std::vector<Cmd*> commands;

		public:
			Cmd& findCommand(const std::string& mName);
			Cmd& create(std::initializer_list<std::string> mNames);
			void parseCommandLine(const std::vector<std::string>& mArgs);
	};
}

#endif
