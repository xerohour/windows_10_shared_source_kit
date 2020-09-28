#include "testrunner.h"
#include "PnPMultiModeSwitchTest.h"

using namespace std;

const wchar_t * desc = L"This tests the behaviour of apps when a PnP Stop is generated when transitioning from windowed mode to full-screen mode and vice-versa.";

/* Prototypes */

/*
Execute the test
*/
void Go();

/*
Entry point
This calls Go()
*/
void __cdecl main()
{
	try
	{
		Go();
	}catch(std::exception& e)
	{
		cout << e.what();
	}
	catch(...)
	{
		cout << "Caught an exception at the highest level\n";
	}
}

void Go()
{

	/*
	The test runner contains all the test components,
	It parses command line options, adds components, and executes them
	*/
	CTestRunner Runner;

	/*
	First, tell the test runner the description for the test application
	(This will appear in the spec)
	*/
	Runner.SetRootComponentDesc(desc);
	Runner.AddNode<CPnPStopLogMachineInfo>(L"PnPStopLogMachineInfo");

	Runner.AddNode<CPnPCheckForStartedIHVDevice>(L"CheckForStartedIHVDevice");

	Runner.AddNode<CModeSwitch>(L"CheckForStartedIHVDevice/ModeSwitch");

	bool bRepetitions = Runner.IsCmdLineSet(L"Repetitions");
	UINT numRepetitions;

	if(bRepetitions)
	{
		const std::vector< std::wstring >& Values = Runner.GetCmdLineValues(L"Repetitions");
		numRepetitions = FromString<int>(Values[0].c_str());
		Runner.SetParamTestDomainSingleValue<UINT>(L"CheckForStartedIHVDevice/ModeSwitch", L"Repetitions", numRepetitions);
	}

	/*
	Now that we have set up the tests, Execute the test cases
	*/
	Runner.Run();
}