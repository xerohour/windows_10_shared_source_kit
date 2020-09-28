#include "CausePreemption.h"
#include "BackBufferCompare.h"
#include "TestPreemption.h"
#include "Reference.h"
#include "Image.h"
#include "pixel.h"
#include <Winbase.h>
#include <stdlib.h>

using namespace std;

const wchar_t* DESC = L"This application tests preemption.";


/*
  Execute the Test
  This can throw an exception, but really shouldn't
*/
void Go();

/*
  Entry point.
  This calls Go() and catches any exception thrown by it
*/
void __cdecl main()
{    
    try
    {
        Go();
    }
    catch(std::exception& e)
    {
        cout << e.what();
    }
    catch(...)
    {
        cout << "Caught an exception at the highest level";
    }    
}


/*
  This sets up the test
*/
void Go()
{
    /*
        The test runner contains all the test components.
        It parses command line options, adds components, and executes them
        Using CTestRunnerBaseVid runner versus standard CTestRunner will
        provide aperture corruption check at the end of the test
    */
    CTestRunnerBaseVid Runner;

    /*
      First, tell the test runner the description for the test application
      (This will appear in the spec)
    */
    Runner.SetRootComponentDesc(DESC);
        
    Runner.AddNode<CProcessFork>(L"fork");

    Runner.AddNode<CI3DDeviceComponent>(L"fork/Device0");
    SetupDeviceCommandLine(Runner, L"fork/Device0");

    Runner.AddNode<CReference>(L"fork/Device0/Reference");

    Runner.AddNode<CWaitNode>(L"fork/Device0/Reference/WaitNode");
    Runner.SetParamTestDomainSingleValue<UINT>( L"fork/Device0/Reference/WaitNode", L"Timeout", 60*60 );//60 minutes wait
    

    /*
        Create all the processes that will now run in parallel
     */
    WCHAR wsz[1024];
    wstring wsPath;

    for(int i=0;i<3;i++)
    {
        swprintf( wsz, L"fork/Device%d", i+1 );
        wsPath = wsz;
        Runner.AddNode<CI3DDeviceComponent>(wsPath.c_str()); 
        SetupDeviceCommandLine(Runner, wsPath.c_str());

        Runner.SetParamTestDomainSingleValue<bool>(wsPath.c_str(), L"Windowed", true);

        wsPath += L"/WaitNode";
        Runner.AddNode<CWaitNode>(wsPath.c_str());
        Runner.SetParamTestDomainSingleValue<UINT>( wsPath.c_str(), L"Timeout", 60*60 );//60 minutes wait

        swprintf( wsz, L"/CausePreemption%d", i+1 );
        wsPath += wsz;
        Runner.AddNode<CCausePreemption>( wsPath.c_str());
    }

    Runner.AddNode<CTestPreemption>(L"fork/Device0/Reference/WaitNode/TestPreemption");

    /*
        only run in windowed mode
     */
    Runner.SetParamTestDomainSingleValue<bool>(L"fork/Device0", L"Windowed", true);
    
    /*
      Now that we have setup our test, execute it
    */
    Runner.Run();
}

