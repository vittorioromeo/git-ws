// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef GITWS_COMMANDLINE_OPTARG
#define GITWS_COMMANDLINE_OPTARG

#include <string>
#include "git-ws/CommandLine/Parser.h"

#include <vector>

namespace ssvcl
{
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

	template<typename T> class ArgPack : public ArgPackBase
	{
		private:
			bool infinite;
			unsigned int count;
			std::vector<T> values;

		public:
			ArgPack(const std::string& mDescription) : ArgPackBase{mDescription} { }
			ArgPack(const std::string& mDescription, unsigned int mMin, unsigned int mMax) : ArgPackBase{mDescription, mMin, mMax} { }

			void set(const std::vector<std::string>& mStrings) override
			{
				for(auto& v : mStrings) values.push_back(Parser<T>::parse(v));
			}
			const std::vector<T>& getValues() const { return values; }
	};
}

#endif
