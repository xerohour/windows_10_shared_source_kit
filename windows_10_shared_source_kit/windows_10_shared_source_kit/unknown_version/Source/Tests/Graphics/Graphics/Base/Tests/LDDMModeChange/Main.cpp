#include "testcomponentspch.h"
#include "basevid.h"
#include "RandomModeChange.h"
#include "RandomMultiMon.h"
#include "UnmanagedPrimarySurface.h"
#include "SharedSurfaceLock.h"

#include <time.h>

const wchar_t* DESC = L"This is the LDDM Mode Change Test.  It verifies that drivers are stable during the full range attach/detach and mode change operations";

using namespace std;

/*
  Execute the Test
  This can throw an exception, but really shouldn't
*/
void Go();
DWORD GetDisplaysAvailable();
void SetRandomDeviceOrder(const wchar_t* szName, CTestRunner* Runner);

/*
  Entry point.
  This calls Go() and catches any exception thrown by it
*/
void __cdecl main()
{    
    srand((unsigned)time(NULL));

    try
    {
        Go();
    }
    catch(std::exception& e)
    {
        cout << e.what();
    } 
}


/*
  This sets up the test
*/
void Go()
{
	/*
      The test runner contains all the test components,
      It parses command line options, adds components, and executes them
    */

    CTestRunner Runner;

    //in BVT mode, we only run a subset (so that it goes faster)
    Runner.DescribeCommandLineUsage(L"bvt", L"Runs the test in BVT mode (runs much faster)");
    bool bBVTMode = Runner.IsCmdLineSet(L"bvt");


    Runner.SetRootComponentDesc(DESC);

    //Set assertion guid
    Runner.SetTestGUID(DISPLAY_STRESS_GUID);


    //Check if we are in LDDM mode

    {
        bool GetLDDMRegistryVersion(unsigned int * uiRegVer);
        unsigned int uiRegVersion = 0;
        if (!GetLDDMRegistryVersion(&uiRegVersion))
        {
            Runner.AddNode<CNullComponent>(L"XDDM_Skip");
            Runner.Run();
            return;
        }
    }


    Runner.AddNode<CRestoreDispModeComponent>(L"Restore");


    Runner.AddNode<CProcessFork>        (L"Restore/ProcessFork");

    if(bBVTMode)
    { 
        cout << "BVT Test Mode" << endl;
        
        Runner.AddNode<CLoop>                       (L"Restore/ProcessFork/SharedSurfaceLoop1");
        Runner.SetParamTestDomainRange<UINT>        (L"Restore/ProcessFork/SharedSurfaceLoop1", L"Index", 0, 20, 1);

        Runner.AddNode<CI3DDeviceComponent>         (L"Restore/ProcessFork/SharedSurfaceLoop1/SharedSurfaceDevice1");
        Runner.SetParamTestDomainSingleValue<bool>  (L"Restore/ProcessFork/SharedSurfaceLoop1/SharedSurfaceDevice1", L"Windowed", true);

        Runner.AddNode<CSharedSurfaceLock>          (L"Restore/ProcessFork/SharedSurfaceLoop1/SharedSurfaceDevice1/SharedSurfaceLock1");

        //don't do draw primitive test case in the BVT version
        //because they are invalid

        Runner.AddNode<CRandomModeChange>           (L"Restore/ProcessFork/RandomModeChange1");
        Runner.SetParamTestDomainRange<int>         (L"Restore/ProcessFork/RandomModeChange1", L"Counter", 1, 5, 1);

        //Because this mode does mode changes while creating devices
        //device creation can legitmately fail
        Runner.DoNotFail(L"Restore/ProcessFork/SharedSurfaceLoop1/SharedSurfaceDevice1");
    }
    else
    {
        //Because this mode randomly enables and disables displays
        //it can generate false failures in the device and rendering components
        //so we disable failures.  This mode is only for stability testing.
        Runner.DoNotFail();

        std::wstring NodeName;
        bool bFST = Runner.IsCmdLineSet(L"fst");

        Runner.DescribeCommandLineUsage(L"fst",
                        L"Set if the test runs as Feature Stress test");

        //According to the FST requirement we can only have upto 5 processes so we'll randomly skip one 
        //of the nodes that fork off a new process to adhere to the limit. Also, we'll create only one component 
        //of each type. For non-FST test runs, we will create two nodes of each type to test multiple components
        //trying to execute the same code path, and we will not skip any nodes.
        
        DWORD dwSkipIndex = bFST ? (rand()%5 + 1) : 0;

        DWORD NumberOfComponents = bFST ? 1 : 2;
        for(DWORD i = 1; i <= NumberOfComponents; i++)
        {
            if(dwSkipIndex != 1)
            {
                NodeName = L"Restore/ProcessFork/RandomModeChange";
                NodeName += ToString<DWORD>(i);
                Runner.AddNode<CRandomModeChange> (NodeName.c_str());
                Runner.SetParamTestDomainRange<int> (NodeName.c_str(), L"Counter", 1, 200, 1);
            }
            if(dwSkipIndex != 2)
            {
                NodeName = L"Restore/ProcessFork/UnmanagedPrimary";
                NodeName += ToString<DWORD>(i);
                Runner.AddNode<CUnmanagedPrimarySurface> (NodeName.c_str());
                Runner.SetParamTestDomainRange<int> (NodeName.c_str(), L"Counter", 1, 200, 1);
                Runner.SetParamTestDomainSingleValue<int> (NodeName.c_str(), L"Delay", 3000 );
            }
            if(dwSkipIndex != 3)
            {
                NodeName = L"Restore/ProcessFork/SharedSurfaceLoop";
                NodeName += ToString<DWORD>(i);
                Runner.AddNode<CLoop> (NodeName.c_str());
                Runner.SetParamTestDomainRange<UINT>(NodeName.c_str(), L"Index", 0, 150, 1);

                NodeName += L"/SharedSurfaceDevice";
                NodeName += ToString<DWORD>(i);
                Runner.AddNode<CI3DDeviceComponent> (NodeName.c_str());
                Runner.SetParamTestDomainSingleValue<bool>(NodeName.c_str(), L"Windowed", true);
                SetRandomDeviceOrder(NodeName.c_str(), &Runner);

                NodeName += L"/SharedSurfaceLock";
                NodeName += ToString<DWORD>(i);
                Runner.AddNode<CSharedSurfaceLock>  (NodeName.c_str());
            }
            if(dwSkipIndex != 4)
            {
                NodeName = L"Restore/ProcessFork/DrawPrimitiveLoop";
                NodeName += ToString<DWORD>(i);
                Runner.AddNode<CLoop>               (NodeName.c_str());
                Runner.SetParamTestDomainRange<UINT>(NodeName.c_str(), L"Index", 0, 250, 1);

                NodeName += L"/DrawPrimitiveDevice";
                NodeName += ToString<DWORD>(i);
                Runner.AddNode<CI3DDeviceComponent> (NodeName.c_str());
                Runner.SetParamTestDomainSingleValue<bool>(NodeName.c_str(), L"Windowed", true);

                NodeName += L"/DrawPrimitive";
                NodeName += ToString<DWORD>(i);
                Runner.AddNode<CDrawPrimitive>      (NodeName.c_str());
                Runner.SetParamTestDomainSingleValue<UINT>(NodeName.c_str(), L"Frames", 250);

            }
            if(dwSkipIndex != 5)
            {
                NodeName = L"Restore/ProcessFork/RandomMultiMon";
                NodeName += ToString<DWORD>(i);
                Runner.AddNode<CRandomMultiMon> (NodeName.c_str());
                Runner.SetParamTestDomainRange<int>(NodeName.c_str(), L"Counter", 1, 200, 1);
            }
        }
    }


    Runner.Run();
}

void SetRandomDeviceOrder(const wchar_t* szName, CTestRunner* Runner)
{
    DWORD NumDisplays = GetDisplaysAvailable();

    std::vector<int> BaseList;
    for(DWORD i = 0; i < NumDisplays; i++)
        BaseList.push_back(i);

    std::list<UINT> RandomList;
    for(size_t i = 0; i < BaseList.size(); i++)
    {
        int number = rand() % BaseList.size();
        if(BaseList[number] == -1)
            i--;
        else
        {
            RandomList.push_back(BaseList[number]);
            BaseList[number] = -1;
        }
    }

    Runner->SetParamTestDomain<UINT>(szName, L"Monitor", RandomList.begin(), RandomList.end());
}

DWORD GetDisplaysAvailable()
{
    DWORD DisplaysAvailable = 0;

    DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);

    for(DWORD DeviceNumber = 0; EnumDisplayDevices(NULL, DeviceNumber, &dd, 0); ++DeviceNumber)
    {
        if(!(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
            ++DisplaysAvailable;
    }

    return DisplaysAvailable;
}


