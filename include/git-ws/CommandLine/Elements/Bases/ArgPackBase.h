// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef GITWS_COMMANDLINE_ELEMENTS_BASES_ARGPACKBASE
#define GITWS_COMMANDLINE_ELEMENTS_BASES_ARGPACKBASE

#include <string>
#include <vector>
#include <SSVUtils/SSVUtils.h>
#include "git-ws/CommandLine/Elements/Bases/ElementBase.h"

namespace ssvcl
{
	class ArgPackBase : public ElementBase
	{
		protected:
			bool infinite;
			unsigned int min, max;

		public:
			ArgPackBase() : infinite{true} { }
			ArgPackBase(unsigned int mMin, unsigned int mMax) : infinite{false}, min{mMin}, max{mMax} { }
			virtual ~ArgPackBase() { }

			virtual void set(const std::vector<std::string>& mStrings) = 0;

			inline bool isInfinite() const		{ return infinite; }
			inline unsigned int getMin() const	{ return min; }
			inline unsigned int getMax() const	{ return max; }
			inline std::string getUsageStr() const override
			{
				return "(PACK " + getName() + " " + "[" + ssvu::toStr(min) + "/" + (infinite ? "..." : ssvu::toStr(max)) + "])";
			}
	};
}

#endif
