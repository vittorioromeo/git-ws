// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

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

			Flag& operator=(bool mActive);
			operator bool() const;

			bool hasName(const std::string& mName);

			const std::string& getShortName();
			const std::string& getLongName();
			std::string getShortNameWithPrefix();
			std::string getLongNameWithPrefix();
			std::string getFlagString();
	};
}

#endif
