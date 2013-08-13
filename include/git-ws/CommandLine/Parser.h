// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef GITWS_COMMANDLINE_PARSER
#define GITWS_COMMANDLINE_PARSER

#include <vector>
#include <string>
#include <stdexcept>
#include <SSVUtils/SSVUtils.h>

namespace ssvcl
{
	template<typename T> struct Parser;
	template<> struct Parser<int>
	{
		inline static int parse(const std::string& mString)
		{
			try { return stoi(mString); }
			catch(...) { throw std::runtime_error("Cannot convert '" + mString + "' to int"); }
		}
	};
	template<> struct Parser<std::string>
	{
		inline static std::string parse(const std::string& mString) { return mString; }
	};
	template<> struct Parser<bool>
	{
		static std::vector<std::string> trueValues{"y", "yes", "on", "true", "1"};
		static std::vector<std::string> falseValues{"n", "no", "off", "false", "0"};
		inline static std::string parse(const std::string& mString)
		{
			return mString;
		}
	};
}

#endif
