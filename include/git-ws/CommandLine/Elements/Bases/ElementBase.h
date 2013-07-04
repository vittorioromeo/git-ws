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
			std::string name;
			std::string briefDescription;
			std::string description;

		public:
			virtual ~ElementBase() { }

			inline void setName(const std::string& mName)							{ name = mName; }
			inline void setBriefDescription(const std::string& mBriefDescription)	{ briefDescription = mBriefDescription; }
			inline void setDescription(const std::string& mDescription)				{ description = mDescription; }

			inline const std::string& getName() const				{ return name; }
			inline const std::string& getBriefDescription() const	{ return briefDescription; }
			inline const std::string& getDescription() const		{ return description; }
			inline virtual std::string getUsageString() const		{ return ""; }
			inline std::string getHelpString()
			{
				std::string result;

				if(!name.empty()) result += "\t" + name;
				if(!briefDescription.empty()) result += "\n" + briefDescription;
				if(!description.empty()) result += "\n" + description;

				std::string usageString{this->getUsageString()};
				if(!usageString.empty()) result += "\n" + usageString;

				return result;
			}
	};
}

#endif
