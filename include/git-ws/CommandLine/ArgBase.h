// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef GITWS_COMMANDLINE_ARGBASE
#define GITWS_COMMANDLINE_ARGBASE

#include <string>

namespace ssvcl
{
	class ArgBase
	{
		private:
			std::string description;

		public:
			ArgBase(const std::string& mDescription);
			virtual ~ArgBase();

			virtual void set(const std::string& mString) = 0;
			const std::string& getDescription() const;
			std::string getArgString() const;
	};
}

#endif
