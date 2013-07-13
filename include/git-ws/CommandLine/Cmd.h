// Copyright (c) 2013 Vittorio Romeo
// License: Academic Free License ("AFL") v. 3.0
// AFL License page: http://opensource.org/licenses/AFL-3.0

#ifndef GITWS_COMMANDLINE_CMD
#define GITWS_COMMANDLINE_CMD

#include <string>
#include <vector>
#include <functional>
#include <memory>

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
			std::vector<std::unique_ptr<ArgBase>> args; // owned
			std::vector<std::unique_ptr<ArgBase>> optArgs; // owned
			std::vector<std::unique_ptr<ArgPackBase>> argPacks; // owned
			std::vector<std::unique_ptr<Flag>> flags; // owned
			std::function<void()> func;
			std::string description;

			Flag& findFlag(const std::string& mName);

		public:
			Cmd(const std::initializer_list<std::string>& mNames);

			Cmd& operator+=(std::function<void()> mFunc);
			Cmd& operator()();

			template<typename T> Arg<T>& createArg() { auto result(new Arg<T>()); args.push_back(std::unique_ptr<Arg<T>>(result)); return *result; }
			template<typename T> OptArg<T>& createOptArg(T mDefaultValue) { auto result(new OptArg<T>(mDefaultValue)); optArgs.push_back(std::unique_ptr<OptArg<T>>(result)); return *result; }
			Flag& createFlag(const std::string& mShortName, const std::string& mLongName);
			template<typename T> ArgPack<T>& createFiniteArgPack(unsigned int mMin, unsigned int mMax) { auto result(new ArgPack<T>(mMin, mMax)); argPacks.push_back(std::unique_ptr<ArgPack<T>>(result)); return *result; }
			template<typename T> ArgPack<T>& createInfiniteArgPack() { auto result(new ArgPack<T>); argPacks.push_back(std::unique_ptr<ArgPack<T>>(result)); return *result; }

			void setArgValue(unsigned int mIndex, const std::string& mValue);
			void setOptArgValue(unsigned int mIndex, const std::string& mValue);
			void setArgPackValues(unsigned int mIndex, const std::vector<std::string>& mValues);

			void activateFlag(const std::string& mName);

			bool hasName(const std::string& mName) const;
			bool isFlagActive(unsigned int mIndex) const;

			inline unsigned int getArgCount() const										{ return args.size(); }
			inline unsigned int getOptArgCount() const									{ return optArgs.size(); }
			inline unsigned int getArgPackCount() const									{ return argPacks.size(); }
			inline unsigned int getFlagCount() const									{ return flags.size(); }
			inline const std::vector<std::string>& getNames() const						{ return names; }
			inline const std::vector<std::unique_ptr<ArgBase>>& getArgs() const			{ return args; }
			inline const std::vector<std::unique_ptr<ArgBase>>& getOptArgs() const		{ return optArgs; }
			inline const std::vector<std::unique_ptr<ArgPackBase>>& getArgPacks() const	{ return argPacks; }
			inline const std::vector<std::unique_ptr<Flag>>& getFlags() const			{ return flags; }

			std::string getNamesString() const;
			std::string getArgsString() const;
			std::string getOptArgsString() const;
			std::string getArgPacksString() const;
			std::string getFlagsString() const;

			std::string getHelpString() const;

			inline void setDescription(const std::string& mDescription)	{ description = mDescription; }
			inline const std::string& getDescription() const			{ return description; }
	};
}

#endif
