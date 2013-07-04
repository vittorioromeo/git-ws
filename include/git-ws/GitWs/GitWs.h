// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef GITWS_GITWS
#define GITWS_GITWS

#include <string>
#include <vector>
#include <SSVUtils/SSVUtils.h>
#include "git-ws/CommandLine/CommandLine.h"

namespace gitws
{
	class GitWs
	{
		private:
			std::vector<std::string> repoPaths;
			ssvcl::CmdLine cmdLine;

			std::vector<std::string> getAheadRepoPaths();
			std::vector<std::string> getChangedRepoPaths();

			std::vector<std::string> runShInPath(const std::string& mPath, const std::string& mCommand);
			void runShInRepos(const std::vector<std::string>& mRepoPaths, const std::string& mCommand, bool mPrintEmpty = false);

			void initCmdHelp();
			void initCmdPush();
			void initCmdPull();
			void initCmdSubmodule();
			void initCmdStatus();
			void initCmdGitg();
			void initCmdDo();
			void initCmdQuery();
			void initCmdVarTest();

			void initRepoPaths();
			void initCmds();

		public:
			GitWs(const std::vector<std::string>& mCommandLine);
	};
}

#endif
