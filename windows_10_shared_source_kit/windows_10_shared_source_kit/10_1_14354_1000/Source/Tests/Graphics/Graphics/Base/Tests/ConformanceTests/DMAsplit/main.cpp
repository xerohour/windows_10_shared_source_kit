#include "DMAsplit.h"

using namespace std;

const wchar_t* DESC = L"This application tests DMA buffer splitting.";


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
        
    /*
      Next we add our hierarchy of components
    */
    
    /*
      At the top level, we add a Device component which creates
      a 3D device and a window (if necessary)
    */
    Runner.AddNode<CI3DDeviceComponent>(L"Device");

    SetupDeviceCommandLine(Runner, L"Device");

    Runner.AddNode<CTestDMASplit>(L"Device/DMAsplit");

    //only run in windowed mode
    Runner.SetParamTestDomainSingleValue<bool>(L"Device", L"Windowed", true);


    /*
      Now that we have setup our test, execute it
    */
    Runner.Run();
}
