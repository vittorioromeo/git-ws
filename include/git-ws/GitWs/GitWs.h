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
	enum class RepoStatus{None, CanCommit, DirtySubmodules};

	std::vector<std::string> runShInPath(const std::string& mPath, const std::string& mCommand);

	struct RepoData
	{
		std::string path, currentBranch;
		RepoData(const std::string& mPath, const std::string& mCurrentBranch) : path{mPath}, currentBranch{mCurrentBranch} { }

		RepoStatus getCommitStatus() const;
		bool getCanPush() const;
		bool getCanPull() const;
		bool getSubmodulesBehind() const;
	};

	class GitWs
	{
		private:
			std::vector<RepoData> repoDatas;
			ssvcl::CmdLine cmdLine;

			std::vector<std::string> getAllRepoPaths();
			std::vector<std::string> getChangedRepoPaths();
			std::vector<std::string> getBehindSMRepoPaths();
			std::vector<std::string> getAheadRepoPaths();

			void runShInRepos(const std::vector<std::string>& mRepoPaths, const std::string& mCommand, bool mPrintEmpty = false);

			void initCmdHelp();
			void initCmdPush();
			void initCmdPull();
			void initCmdSubmodule();
			void initCmdStatus();
			void initCmdGitg();
			void initCmdDo();
			void initCmdQuery();

			void initRepoDatas();
			void initCmds();

		public:
			GitWs(const std::vector<std::string>& mCommandLine);
	};
}

#endif
