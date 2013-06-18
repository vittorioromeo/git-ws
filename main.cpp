#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <SSVUtils/SSVUtils.h>

using namespace std;
using namespace ssvu;

struct CLArgBase
{
	virtual void setValue(const string& mString) = 0;
};

template<typename T> struct CLArg : CLArgBase { };
template<> struct CLArg<int> : CLArgBase
{
	int value;
	void setValue(const string& mString) override { value = stoi(mString); }
};

struct CLCommand
{
	vector<string> names;
	vector<CLArgBase*> args;
	function<void()> func;
};

struct CLMain
{
	vector<CLCommand> commands;
};

void showHelp()
{
	log("what");
}

int main(int argc, char* argv[])
{
	for(int i{0}; i < argc; ++i) log(toStr(argv[i]));

	CLCommand sum{{"sum"}};

	CLArg<int> arg1;
	CLArg<int> arg2;

	sum.args.push_back(&arg1);
	sum.args.push_back(&arg2);

	sum.args[0]->setValue("1");
	sum.args[1]->setValue("3");

	sum.func = [&]
	{
		int i = arg1.value + arg2.value;
		log(toStr(i));
	};

	sum.func();

	return 0;
}
