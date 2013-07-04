// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef GITWS_COMMANDLINE_ELEMENTS_OPTARG
#define GITWS_COMMANDLINE_ELEMENTS_OPTARG

#include <string>
#include "git-ws/CommandLine/Parser.h"

namespace ssvcl
{
	template<typename T> class OptArg : public Arg<T>
	{
		private:
			T defaultValue;
			bool active{false};

		public:
			OptArg(T mDefaultValue) : defaultValue{mDefaultValue} { }

			inline operator bool() const { return active; }

			inline void set(const std::string& mValue) override { Arg<T>::set(mValue); active = true; }

			inline T get() const { return active ? Arg<T>::get() : defaultValue; }
	};
}

#endif
