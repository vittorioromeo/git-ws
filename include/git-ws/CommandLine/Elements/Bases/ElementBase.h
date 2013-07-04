// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef GITWS_COMMANDLINE_ELEMENTS_BASES_ELEMENTBASE
#define GITWS_COMMANDLINE_ELEMENTS_BASES_ELEMENTBASE

#include <string>

namespace ssvcl
{
	class ElementBase
	{
		private:
			std::string description;

		public:
			virtual ~ElementBase() { }

			inline void setDescription(const std::string& mDescription) { description = mDescription; }

			inline const std::string& getDescription() const { return description; }
			inline virtual std::string getUsageString() const = 0;
	};
}

#endif
