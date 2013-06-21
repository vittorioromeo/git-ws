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
			const std::string& getDescription();
			std::string getArgString();
	};
}

#endif
