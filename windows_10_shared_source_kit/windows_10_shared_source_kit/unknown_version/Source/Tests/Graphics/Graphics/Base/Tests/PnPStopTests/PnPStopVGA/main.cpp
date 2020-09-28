#include "testrunner.h"
#include "PnPStopVGA.h"

using namespace std;

const wchar_t * desc = L"This application generates a PnPStop and checks if the system is in VGA mode.";

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

    Runner.AddNode<CPnPStopVGA>(L"CheckForStartedIHVDevice/PnPStopVGA");

    bool bUMPC = Runner.IsCmdLineSet(L"UMPC");
    if(bUMPC)
    {
        Runner.SetParamTestDomainSingleValue<bool>(L"CheckForStartedIHVDevice/PnPStopVGA", L"UMPC", bUMPC);
    }

    /*
    Now that we have set up the tests, Execute the test cases
    */
    Runner.Run();
}

