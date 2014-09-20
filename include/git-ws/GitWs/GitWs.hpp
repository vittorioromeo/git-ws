// Copyright (c) 2013-2014 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef GITWS_GITWS
#define GITWS_GITWS

#include <SSVUtils/Core/Core.hpp>
#include <SSVUtils/CommandLine/CommandLine.hpp>

namespace gitws
{
	class Repo
	{
		public:
			enum class Status{None, CanCommit, DirtySM};

		public:
			inline auto run(const std::string& mStr) const
			{
				std::vector<std::string> result;
				std::string toRun{"(cd " + path.getStr() + " && " + mStr + ")"}, file;
				FILE* pipe{popen(toRun.c_str(), "r")};
				char buffer[1000];
				while(fgets(buffer, sizeof(buffer), pipe) != NULL)
				{
					file = buffer;
					result.emplace_back(file.substr(0, file.size() - 1));
				}
				pclose(pipe);

				return result;
			}

		private:
			ssvufs::Path path;
			std::string branch;

			inline auto runGetBranch() const							{ return run("git rev-parse --abbrev-ref HEAD")[0]; }
			inline auto runRevList(const std::string& mBranch) const	{ return run("git rev-list HEAD...origin/" + mBranch + " --ignore-submodules --count")[0]; }
			inline auto runGetFetch() const								{ return run("git fetch --dry-run 2>&1"); }
			inline auto runGetFetchSM() const							{ return run("git submodule foreach git fetch --dry-run 2>&1"); }
			inline bool runHasDiffIndex(bool mIgnoreSubmodules = false) const
			{
				return run(std::string{"git diff-index --name-only"} + (mIgnoreSubmodules ? " --ignore-submodules" : "")  + " HEAD --").empty();
			}

		public:
			inline Repo(const ssvufs::Path& mPath) : path{mPath}, branch{runGetBranch()} { }

			inline void runSMPull() const
			{
				run(R"(git submodule update --recursive --remote --init &&
					git submodule foreach git reset --hard &&
					git submodule foreach git checkout origin master &&
					git submodule foreach git rebase origin master &&
					git submodule foreach git pull -f origin master --recurse-submodules)");
			}
			inline void runSMPush() const { run(R"(git commit -am 'automated submodule update' && git push)"); }

			inline const auto& getPath() const		{ return path; }
			inline const auto& getBranch() const	{ return branch; }

			inline bool canPush() const { return std::stoi(runRevList(branch)) > 0; }
			inline bool canPull() const { return !runGetFetch().empty(); }
			inline bool canCommit() const { return !runHasDiffIndex(true); }
			inline auto getStatus(bool mIgnoreSubmodules) const
			{
				if(!runHasDiffIndex(true)) return Status::CanCommit;
				if(!mIgnoreSubmodules && !runHasDiffIndex(false)) return Status::DirtySM;
				return Status::None;
			}
			inline bool getSubmodulesBehind() const
			{
				for(const auto& s : runGetFetchSM()) { if(ssvu::beginsWith(s, "Entering")) continue; return true; }
				return false;
			}
	};

	class GitWs
	{
		private:
			std::vector<Repo> repos;
			ssvu::CommandLine::CmdLine cmdLine;

			inline auto getBehindSMRepos() const
			{
				std::vector<Repo> result;
				for(const auto& rd : repos)
				{
					const auto& cs(rd.getStatus(false));
					if((rd.getSubmodulesBehind() || cs == Repo::Status::DirtySM) && cs != Repo::Status::CanCommit && !rd.canPush()) result.emplace_back(rd);
				}
				return result;
			}
			inline auto getChangedRepos(bool mIgnoreSubmodules) const	{ std::vector<Repo> result; for(const auto& rd : repos) if(rd.getStatus(mIgnoreSubmodules) == Repo::Status::CanCommit) result.emplace_back(rd); return result; }
			inline auto getAheadRepos() const							{ std::vector<Repo> result; for(const auto& rd : repos) if(rd.canPush()) result.emplace_back(rd); return result; }

			void runInRepos(const std::vector<Repo>& mRepos, const std::string& mCommand, bool mPrintEmpty = false);

			void initCmdHelp();
			void initCmdPush();
			void initCmdPull();
			void initCmdSubmodule();
			void initCmdStatus();
			void initCmdGitg();
			void initCmdDo();
			void initCmdQuery();

			inline void initRepoDatas()	{ for(auto& p : ssvufs::getScan<ssvufs::Mode::Single, ssvufs::Type::Folder>("./")) if(ssvufs::Path{p + "/.git/"}.exists<ssvufs::Type::Folder>()) repos.emplace_back(p); }
			inline void initCmds()		{ initCmdHelp(); initCmdPush(); initCmdPull(); initCmdSubmodule(); initCmdStatus(); initCmdGitg(); initCmdDo(); initCmdQuery(); }

		public:
			inline GitWs(const std::vector<std::string>& mCommandLine) { initRepoDatas(); initCmds(); cmdLine.parseCmdLine(mCommandLine); }
	};
}

#endif
