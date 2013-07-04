// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef GITWS_COMMANDLINE_ELEMENTS_ARGPACK
#define GITWS_COMMANDLINE_ELEMENTS_ARGPACK

#include <string>
#include <vector>
#include "git-ws/CommandLine/Parser.h"

namespace ssvcl
{
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

			typedef typename std::vector<T>::iterator iterator;
			typedef typename std::vector<T>::const_iterator const_iterator;

			iterator begin()				{ return values.begin(); }
			const_iterator begin() const	{ return values.begin(); }
			const iterator cbegin() const	{ return values.cbegin(); }
			iterator end()					{ return values.end(); }
			const_iterator end() const		{ return values.end(); }
			const iterator cend() const		{ return values.cend(); }
	};
}

#endif
