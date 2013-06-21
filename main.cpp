#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <stdexcept>
#include <SSVUtils/SSVUtils.h>

using namespace std;
using namespace ssvu;
using namespace ssvu::FileSystem;

const string flagPrefixShort{"-"};
const string flagPrefixLong{"--"};

template<typename T> struct CLValueParser;
template<> struct CLValueParser<int>
{
	inline static int parse(const string& mString)
	{
		try { return stoi(mString); }
		catch(...) { throw runtime_error("Cannot convert '" + mString + "' to int"); }
	}
};
template<> struct CLValueParser<string>
{
	inline static string parse(const string& mString) { return mString; }
};

class CLArgBase
{
	private:
		string description;

	public:
		CLArgBase(const string& mDescription) : description{mDescription} { }

		virtual ~CLArgBase() { }
		virtual void set(const string& mString) = 0;

		const string& getDescription() { return description; }
		string getArgString() { return "(" + description + ")"; }
};
template<typename T> class CLArg : public CLArgBase
{
	private:
		T value;

	public:
		CLArg(const string& mDescription) : CLArgBase{mDescription} { }

		T get() const { return value; }
		void set(const string& mValue) override { value = CLValueParser<T>::parse(mValue); }
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

		const string& getShortName() { return shortName; }
		const string& getLongName() { return longName; }
		string getShortNameWithPrefix() { return flagPrefixShort + shortName; }
		string getLongNameWithPrefix() { return flagPrefixLong + longName; }
		string getFlagString() { return "[" + getShortNameWithPrefix() + " || " + getLongNameWithPrefix() + "]"; }
};

class CLCommand
{
	private:
		vector<string> names;
		vector<CLArgBase*> args; // owned
		vector<CLFlag*> flags; // owned
		function<void()> func;

		CLFlag& findFlag(const string& mName)
		{
			for(const auto& f : flags) if(f->hasName(mName)) return *f;
			throw runtime_error("No flag with name '" + mName + "' in command " + getNamesString());
		}

	public:
		CLCommand(initializer_list<string> mNames) : names{mNames} { }
		~CLCommand()
		{
			for(const auto& a : args) delete a;
			for(const auto& f : flags) delete f;
		}

		template<typename T> CLArg<T>& createArg(const string& mDescription) { auto result(new CLArg<T>(mDescription)); args.push_back(result); return *result; }
		void setArgValue(unsigned int mIndex, const string& mValue) { args[mIndex]->set(mValue); }
		unsigned int getArgCount() { return args.size(); }

		CLFlag& createFlag(const string& mShortName, const string& mLongName) { auto result(new CLFlag{mShortName, mLongName}); flags.push_back(result); return *result; }
		bool isFlagActive(unsigned int mIndex) { return flags[mIndex]->isActive(); }
		unsigned int getFlagCount() { return flags.size(); }
		void activateFlag(const string& mName) { findFlag(mName).setActive(true); }

		bool hasName(const string& mName) { return contains(names, mName); }
		void execute() { func(); }
		void setFunc(function<void()> mFunc) { func = mFunc; }

		const vector<string>& getNames() { return names; }
		const vector<CLArgBase*>& getArgs() { return args; }
		const vector<CLFlag*>& getFlags() { return flags; }

		string getNamesString()
		{
			string result{"<"};
			for(unsigned int i{0}; i < names.size(); ++i)
			{
				result.append(names[i]);
				if(i < names.size() - 1) result.append(" || ");
			}
			result.append(">");
			return result;
		}
		string getArgsString()
		{
			string result;
			for(unsigned int i{0}; i < args.size(); ++i)
			{
				result.append(args[i]->getArgString());
				if(i < args.size() - 1) result.append(" ");
			}
			return result;
		}
		string getFlagsString()
		{
			string result;
			for(unsigned int i{0}; i < flags.size(); ++i)
			{
				result.append(flags[i]->getFlagString());
				if(i < flags.size() - 1) result.append(" ");
			}
			return result;
		}
};

class CLMain
{
	private:
		vector<CLCommand*> commands;

	public:
		CLCommand& findCommand(const string& mName)
		{
			for(const auto& c : commands) if(c->hasName(mName)) return *c;
			throw runtime_error("No command with name <" + mName + ">");
		}

		CLCommand& create(initializer_list<string> mNames) { auto result(new CLCommand{mNames}); commands.push_back(result); return *result; }

		void parseCommandLine(const vector<string>& mArgs)
		{
			const string& cmdName{mArgs[0]};
			CLCommand& cmd{findCommand(cmdName)};

			vector<string> cArgs, cFlags;
			for(unsigned int i{1}; i < mArgs.size(); ++i)
			{
				const string& s{mArgs[i]};
				(startsWith(s, flagPrefixShort) || startsWith(s, flagPrefixLong)) ? cFlags.push_back(s) : cArgs.push_back(s);
			}

			if(cArgs.size() != cmd.getArgCount()) throw runtime_error("Incorrect number of arguments for command " + cmd.getNamesString() + " , correct number is '" + toStr(cmd.getArgCount()) + "'");

			for(unsigned int i{0}; i < cArgs.size(); ++i) cmd.setArgValue(i, cArgs[i]);
			for(const auto& f : cFlags) cmd.activateFlag(f);
			cmd.execute();
		}
};

vector<string> repoFolders;
CLMain m;

void runCommandInRepos(const string& mCommand)
{
	for(const auto& f : repoFolders)
	{
		log(f);
		string cmd{"(cd " + f + ";" + mCommand + ")"};
		system(cmd.c_str());
		log("");
	}
}

void initHelp()
{
	auto& helpCommand(m.create({"?", "help"}));
	auto& argCommandName(helpCommand.createArg<string>("command name"));
	helpCommand.setFunc([&]
	{
		auto& command(m.findCommand(argCommandName.get()));
		log("Command " + command.getNamesString() + " usage:");
		log(command.getNamesString() + " " + command.getArgsString() + " " + command.getFlagsString());
	});
}

void initPush()
{
	auto& pushCmd(m.create({"push"}));
	auto& forceFlag(pushCmd.createFlag("f", "force"));
	pushCmd.setFunc([&]
	{
		runCommandInRepos(forceFlag.isActive() ? "git push -f" : "git push");
	});
}

void initPull()
{
	auto& pullCmd(m.create({"pull"}));
	auto& stashFlag(pullCmd.createFlag("s", "stash"));
	pullCmd.setFunc([&]
	{
		runCommandInRepos(stashFlag.isActive() ? "git stash; git pull" : "git pull");
	});
}

void initStatus()
{
	m.create({"status"}).setFunc([&]{ runCommandInRepos("git status -s"); });
}

void initSubmodule()
{
	auto& submoduleCmd(m.create({"sub", "submodule"}));
	auto& arg(submoduleCmd.createArg<string>("submodule action"));
	submoduleCmd.setFunc([&]
	{
		if(arg.get() == "push") runCommandInRepos("git commit -am 'automated submodule update'; git push");
		else if(arg.get() == "pull") runCommandInRepos("git submodule foreach git stash; git submodule foreach git pull origin master --recurse-submodules");
		else if(arg.get() == "au")
		{
			runCommandInRepos("git submodule foreach git stash; git submodule foreach git pull origin master --recurse-submodules");
			runCommandInRepos("git commit -am 'automated submodule update'; git push");
		}
	});
}

void initGitg()
{
	m.create({"gitg"}).setFunc([&]{ runCommandInRepos("gitg&"); });
}

void initCommands() { initHelp(); initPush(); initPull(); initStatus(); initSubmodule(); initGitg(); }

int main(int argc, char* argv[])
{
	for(auto& cf : getScan<Mode::Single, Type::Folder>("./")) if(exists(cf + "/.git/")) repoFolders.push_back(cf);

	initCommands();

	vector<string> args;
	for(int i{1}; i < argc; ++i) args.push_back(toStr(argv[i]));

	try{ m.parseCommandLine(args); }
	catch(runtime_error mException) { log(mException.what()); return 1; }

	return 0;
}

