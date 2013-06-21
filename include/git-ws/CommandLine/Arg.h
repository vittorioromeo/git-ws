// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef GITWS_COMMANDLINE_ARG
#define GITWS_COMMANDLINE_ARG

#include <string>
#include "git-ws/CommandLine/Parser.h"

namespace ssvcl
{
	template<typename T> class Arg : public ArgBase
	{
		protected:
			T value;

		public:
			Arg(const std::string& mDescription) : ArgBase{mDescription} { }

			T get() const { return value; }
			void set(const std::string& mValue) override { value = Parser<T>::parse(mValue); }
	};

	template<typename T> class OptArg : public Arg<T>
	{
		private:
			T defaultValue;
			bool active{false};

		public:
			OptArg(T mDefaultValue, const std::string& mDescription) :  Arg<T>{mDescription}, defaultValue{mDefaultValue} { }

			T get() const { return active ? this->value : defaultValue; }
			void set(const std::string& mValue) override { Arg<T>::set(mValue); active = true; }

			operator bool() const { return active; }
	};
}

#endif
