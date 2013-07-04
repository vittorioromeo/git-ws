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
			typedef typename std::vector<T>::iterator iterator;
			typedef typename std::vector<T>::const_iterator const_iterator;

			bool infinite;
			unsigned int count;
			std::vector<T> values;

		public:
			ArgPack() { }
			ArgPack(unsigned int mMin, unsigned int mMax) : ArgPackBase{mMin, mMax} { }

			inline void set(const std::vector<std::string>& mValues) override
			{
				for(const auto& v : mValues) values.push_back(Parser<T>::parse(v));
			}

			inline iterator begin()					{ return values.begin(); }
			inline iterator end()					{ return values.end(); }
			inline const_iterator begin() const		{ return values.begin(); }
			inline const_iterator end() const		{ return values.end(); }
			inline const iterator cbegin() const	{ return values.cbegin(); }
			inline const iterator cend() const		{ return values.cend(); }
	};
}

#endif
