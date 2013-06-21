#ifndef GITWS_COMMANDLINE_FLAG
#define GITWS_COMMANDLINE_FLAG

#include <string>

namespace ssvcl
{
	const std::string flagPrefixShort{"-"};
	const std::string flagPrefixLong{"--"};

	class Flag
	{
		private:
			std::string shortName, longName;
			bool active{false};

		public:
			Flag(const std::string& mShortName, const std::string& mLongName);

			void setActive(bool mActive);
			bool isActive();
			bool hasName(const std::string& mName);

			const std::string& getShortName();
			const std::string& getLongName();
			std::string getShortNameWithPrefix();
			std::string getLongNameWithPrefix();
			std::string getFlagString();
	};
}

#endif
