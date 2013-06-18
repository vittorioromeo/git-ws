#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <SSVUtils/SSVUtils.h>

using namespace std;
using namespace ssvu;

const string flagPrefixShort{"-"};
const string flagPrefixLong{"--"};

template<typename T> struct CLValueParser;
template<> struct CLValueParser<int> { inline static int parse(const string& mString) { return stoi(mString); } };
template<> struct CLValueParser<string> { inline static string parse(const string& mString) { return mString; } };

struct CLArgBase
{
	virtual ~CLArgBase() { }
	virtual void setValue(const string& mString) = 0;
};
template<typename T> class CLArg : public CLArgBase
{
	private:
		T value;

	public:
		T getValue() { return value; }
		void setValue(const string& mValue) override { value = CLValueParser<T>::parse(mValue); }
};

class CLFlag
{
	private:
		string shortName, longName;
		bool active{false};

	public:
		CLFlag(const string& mShortName, const string& mLongName) : shortName{mShortName}, longName{mLongName} { }
		void setActive(bool mActive) { active = mActive; }
		bool isActive() { return active; }
		bool hasName(const string& mName) { return mName == flagPrefixShort + shortName || mName == flagPrefixLong + longName; }
};

class CLCommand
{
	private:
		vector<string> names;
		vector<CLArgBase*> args;
		vector<CLFlag*> flags;
		function<void()> func;

		CLFlag* findFlag(const string& mName)
		{
			for(const auto& f : flags) if(f->hasName(mName)) return f;
			throw runtime_error("No flag with name <" + mName + ">");
		}

	public:
		CLCommand(initializer_list<string> mNames) : names{mNames} { }
		~CLCommand()
		{
			for(const auto& a : args) delete a;
			for(const auto& f : flags) delete f;
		}

		template<typename T> CLArg<T>& createArg() { auto result(new CLArg<T>); args.push_back(result); return *result; }
		void setArgValue(unsigned int mIndex, const string& mValue) { args[mIndex]->setValue(mValue); }
		template<typename T> T getArgValue(unsigned int mIndex) { return static_cast<CLArg<T>*>(args[mIndex])->getValue(); }
		unsigned int getArgCount() { return args.size(); }

		CLFlag& createFlag(const string& mShortName, const string& mLongName) { auto result(new CLFlag{mShortName, mLongName}); flags.push_back(result); return *result; }
		bool isFlagActive(unsigned int mIndex) { return flags[mIndex]->isActive(); }
		unsigned int getFlagCount() { return flags.size(); }
		void activateFlag(const string& mName) { findFlag(mName)->setActive(true); }

		bool hasName(const string& mName) { return contains(names, mName); }
		void execute() { func(); }
		void setFunc(function<void()> mFunc) { func = mFunc; }
};

class CLMain
{
	private:
		vector<CLCommand*> commands;

		CLCommand* findCommand(const string& mName)
		{
			for(const auto& c : commands) if(c->hasName(mName)) return c;
			throw runtime_error("No command with name <" + mName + ">");
		}

	public:
		CLCommand& createCommand(initializer_list<string> mNames) { auto result(new CLCommand{mNames}); commands.push_back(result); return *result; }

		void parseCommandLine(const vector<string>& mArgs)
		{
			const string& cmdName{mArgs[0]};
			CLCommand* cmd;

			try { cmd = findCommand(cmdName); }
			catch(runtime_error mException) { log(mException.what()); return; }

			vector<string> cArgs, cFlags;
			for(unsigned int i{1}; i < mArgs.size(); ++i)
			{
				const string& s{mArgs[i]};
				if(startsWith(s, flagPrefixShort) || startsWith(s, flagPrefixLong)) cFlags.push_back(s);
				else cArgs.push_back(s);
			}

			unsigned int argCount{cmd->getArgCount()};
			if(cArgs.size() != argCount) { log("Incorrect number of arguments for command <" + cmdName + ">, correct number is <" + toStr(argCount) + ">", "CLMain::parseCommandLine"); return; }

			log("Setting arguments values of command <" + cmdName + ">", "CLMain::parseCommandLine");
			for(unsigned int i{0}; i < cArgs.size(); ++i)
			{
				try { cmd->setArgValue(i, cArgs[i]); }
				catch(...) { log("Error setting argument number <" + toStr(i) + "> for command <" + cmdName + ">", "CLMain::parseCommandLine"); return; }
			}

			for(const auto& f : cFlags)
			{
				try { cmd->activateFlag(f); }
				catch(runtime_error mException) { log(mException.what()); return; }
			}

			log("Executing <" + cmdName + ">", "CLMain::parseCommandLine");
			cmd->execute();
		}
};

int main(int argc, char* argv[])
{
	vector<string> args;
	for(int i{1}; i < argc; ++i) args.push_back(toStr(argv[i]));

	CLMain main;

	auto& sum(main.createCommand({"sum"}));
	sum.createArg<int>();
	sum.createArg<int>();
	sum.createFlag("d", "double");
	sum.setFunc([&]
	{
		int i = sum.getArgValue<int>(0) + sum.getArgValue<int>(1);
		if(sum.isFlagActive(0)) i *= 2;
		log(toStr(i));
	});

	main.parseCommandLine(args);

	return 0;
}
