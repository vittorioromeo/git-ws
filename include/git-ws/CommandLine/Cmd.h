// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef GITWS_COMMANDLINE_CMD
#define GITWS_COMMANDLINE_CMD

#include <string>
#include <vector>
#include <functional>

namespace ssvcl
{
	class Flag;
	class ArgBase;
	class ArgPackBase;
	template<typename T> class Arg;
	template<typename T> class OptArg;
	template<typename T> class ArgPack;

	class Cmd
	{
		private:
			std::vector<std::string> names;
			std::vector<ArgBase*> args; // owned
			std::vector<ArgBase*> optArgs; // owned
			std::vector<ArgPackBase*> argPacks; // owned
			std::vector<Flag*> flags; // owned
			std::function<void()> func;

			Flag& findFlag(const std::string& mName);

		public:
			Cmd(const std::initializer_list<std::string>& mNames);
			~Cmd();

			Cmd& operator+=(std::function<void()> mFunc);
			Cmd& operator()();

			template<typename T> Arg<T>& createArg() { auto result(new Arg<T>()); args.push_back(result); return *result; }
			template<typename T> OptArg<T>& createOptArg(T mDefaultValue) { auto result(new OptArg<T>(mDefaultValue)); optArgs.push_back(result); return *result; }
			Flag& createFlag(const std::string& mShortName, const std::string& mLongName);
			template<typename T> ArgPack<T>& createFiniteArgPack(unsigned int mMin, unsigned int mMax) { auto result(new ArgPack<T>(mMin, mMax)); argPacks.push_back(result); return *result; }
			template<typename T> ArgPack<T>& createInfiniteArgPack() { auto result(new ArgPack<T>); argPacks.push_back(result); return *result; }

			void setArgValue(unsigned int mIndex, const std::string& mValue);
			void setOptArgValue(unsigned int mIndex, const std::string& mValue);
			void setArgPackValues(unsigned int mIndex, const std::vector<std::string>& mValues);

			void activateFlag(const std::string& mName);

			bool hasName(const std::string& mName) const;
			bool isFlagActive(unsigned int mIndex) const;

			unsigned int getArgCount() const;
			unsigned int getOptArgCount() const;
			unsigned int getFlagCount() const;
			unsigned int getArgPackCount() const;
			const std::vector<std::string>& getNames() const;
			const std::vector<ArgBase*>& getArgs() const;
			const std::vector<ArgBase*>& getOptArgs() const;
			const std::vector<ArgPackBase*>& getArgPacks() const;
			const std::vector<Flag*>& getFlags() const;

			std::string getNamesString() const;
			std::string getArgsString() const;
			std::string getOptArgsString() const;
			std::string getArgPacksString() const;
			std::string getFlagsString() const;

			std::string getHelpString() const;
	};
}

#endif
