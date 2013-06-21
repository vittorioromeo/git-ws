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
