#ifndef GITWS_COMMANDLINE_ARG
#define GITWS_COMMANDLINE_ARG

#include <string>
#include "git-ws/CommandLine/Parser.h"

namespace ssvcl
{
	template<typename T> class Arg : public ArgBase
	{
		private:
			T value;

		public:
			Arg(const std::string& mDescription) : ArgBase{mDescription} { }

			T get() const { return value; }
			void set(const std::string& mValue) override { value = Parser<T>::parse(mValue); }
	};
}

#endif
