#define STRSAFE_NO_DEPRECATE
#include <etwConsumersRequire.h>
#include <provider.h>

#include <lddmcoreeventdefs.h>
#include <d3dkmthk.h>
#include <d3dkmthk_p.h>
#include <d3dukmdt.h>
#include <dmmdiag.h>
#include <dxetwevt.hxx>

#include <DxgKernelConsumer.h>
#include <DxgKernelLegConsumer.h>
#include <Controller.h>
#include <etwStatistics.h>
#include <DXGKernelStatistics.h>

#include <etwLibEvents.h>

#include "TestRunner.h"
#include "testcomponent.h"

#include "BaseVid.h"

#include <CrossProcSharing.h>

using namespace std;

#include "ManagerClient.h"
#include "PreemptionEvent.h"


// If we want to log events anywhere in this application, we can now that we have
// a etwProvider created globally.
CProvider etwProvider;

const wchar_t* DESC = L"SchManager listens for events while other tests are running on the system.";

//  Execute the Test
//  This can throw an exception, but really shouldn't
void Go();


//  Entry point.
//  This calls Go() and catches any exception thrown by it
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



// This sets up the tests. There are two basic tests so far:
//
// Either "Manager" or "Client" on the command line:
//    
//     This will run the 'unittest' configuration that lets you see how this class
// works. If you run two versions of the binary, one should be the manager and the
// other should be the client. See the ManagerClient.cpp file for more details.
//
// ListenForPreemptionEvent on the command line:
//
//    This will instruct the binary to listen for Preemption Events. See the associated
// class.
//
void Go()
{
    // The test runner contains all the test components,
    // It parses command line options, adds components, and executes them
    CTestRunner Runner;

    // First, tell the test runner the description for the test application
    // (This will appear in the spec)
    Runner.SetRootComponentDesc(DESC);
        
    std::wstring nodeStr = L"";

    // At the next level, we add a Device component which creates
    // a 3D device and a window (if necessary)

    nodeStr += L"Device";
    Runner.AddNode<CI3DDeviceComponent>( nodeStr.c_str() );

    //Setup standard cmd line options
    SetupDeviceCommandLine(Runner, L"Device");

    // Now look for friendly words on the command line

    bool bManager = Runner.IsCmdLineSet(L"Manager");
    Runner.DescribeCommandLineUsage(L"Manager",
                                    L"Counts specific events on the system");

    bool bClient = Runner.IsCmdLineSet(L"Client");
    Runner.DescribeCommandLineUsage(L"Client",
                                    L"Generates specific events on the system while expecting a manager");

    bool bListenForPreemptionEvent = Runner.IsCmdLineSet(L"ListenForPreemptionEvent");
    Runner.DescribeCommandLineUsage(L"ListenForPreemptionEvent",
                                    L"Performs xProcSharing while listening for specific preemption events");

    bool bListenForPreemptionTime = Runner.IsCmdLineSet(L"ListenForPreemptionTime");
    Runner.DescribeCommandLineUsage(L"ListenForPreemptionTime",
                                    L"Performs xProcSharing while counting and timing preemption events (execution time)");

    bool bListenForTime = Runner.IsCmdLineSet(L"ListenForTime");
    Runner.DescribeCommandLineUsage(L"ListenForTime",
                                    L"Performs xProcSharing while counting execution time");

    // If we're being a simple manager or client, perform that function.
    if( bManager || bClient )
    {
        if( bClient && bManager )
        {
            // Can't be used this way.
            PATH_TRAP();
            throw wexception(L"This binary cannot be both a Manager and a Client at the same time!");
        }
        if( bManager )
        {
            nodeStr += L"/XProcManager";
            Runner.AddNode<CXProcManager>(nodeStr.c_str());
        }
        else
        {
            nodeStr += L"/XProcClient";
            Runner.AddNode<CXProcClient>(nodeStr.c_str());
        }
    }
    else if( bListenForPreemptionEvent )
    {
        // Want to perform xProc functionality listening for specific preemption events.
        nodeStr += L"/XProcListenForPreemptionEvent";
        Runner.AddNode<CXProcListenForPreemptionEvent>(nodeStr.c_str());

        // Links for PreemptionCounter
        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"RecordingTime",  // Variable name
                                   L"Time",       // CommandLine string
                                   L"Number of seconds to listen for the events" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"LoopIterations",  // Variable name
                                   L"Loop",       // CommandLine string
                                   L"Number of times to retry calling the workhorse client" );

#define LINK_FOR_COUNTER(_Name_) Runner.LinkParamToCmdLine( nodeStr.c_str(),L"v" L#_Name_,L#_Name_, L"Type of Preemption Event to count" )

        // Now add in our preemption events command line options. We can only listen for 
        // one type at a time but this is not policed.
        LINK_FOR_COUNTER(Success);
        LINK_FOR_COUNTER(MissNoCommand);
        LINK_FOR_COUNTER(MissNotEnabled);
        LINK_FOR_COUNTER(MissNextFence);
        LINK_FOR_COUNTER(SuccessNextFence);
        LINK_FOR_COUNTER(MissPagingCommand);
        LINK_FOR_COUNTER(SuccessPagingCommand);
        LINK_FOR_COUNTER(MissSplittedCommand);
        LINK_FOR_COUNTER(SuccessSplittedCommand);
        LINK_FOR_COUNTER(MissFenceCommand);
        LINK_FOR_COUNTER(MissRenderPendingFlip);
        LINK_FOR_COUNTER(SuccessRenderPendingFlip);
        LINK_FOR_COUNTER(MissNotMakingProgress);
        LINK_FOR_COUNTER(MissLessPriority);
        LINK_FOR_COUNTER(MissRemainingQuantum);
        LINK_FOR_COUNTER(MissRemainingPreemptionQuantum);
        LINK_FOR_COUNTER(MissAlreadyPreempting);
        LINK_FOR_COUNTER(MissGlobalBlock);
        LINK_FOR_COUNTER(SuccessGlobalBlock);
        LINK_FOR_COUNTER(MissAlreadyRunning);
        LINK_FOR_COUNTER(SuccessHigherInProcPriority);

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"bTrackChangeEvents",
                                   L"TrackChangeEvents",
                                   L"If true, we track ChangeInProcessPriority events" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"bWantIHVHardware",  // Variable name
                                   L"WantIHVHardware",       // CommandLine string
                                   L"Skip if we're not running in IHV hardware" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"bStability",  // Variable name
                                   L"Stability",       // CommandLine string
                                   L"Run as stability test, don't fail on low counts" );
    }
    else if( bListenForPreemptionTime )
    {
        // Want to perform xProc functionality counting preemption events and execution time.
        nodeStr += L"/XProcListenForPreemptionTime";
        Runner.AddNode<CXProcListenForPreemptionTime>(nodeStr.c_str());

        // Links for PreemptionCounter
        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"RecordingTime",  // Variable name
                                   L"Time",       // CommandLine string
                                   L"Number of seconds to listen for the events" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"LoopIterations",  // Variable name
                                   L"Loop",       // CommandLine string
                                   L"Number of times to retry calling the workhorse client" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"PoliceMS",  // Variable name
                                   L"PoliceMS",       // CommandLine string
                                   L"Fail if maximum millisecond threshold is exceeded" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"EnforcePreemptionSupport",  // Variable name
                                   L"EnforcePreemptionSupport",  // CommandLine string
                                   L"Fail if hardware doesn't support preemption" );
    }
    else if( bListenForTime )
    {
        // Want to perform xProc functionality counting preemption events and execution time.
        nodeStr += L"/XProcListenForTime";
        Runner.AddNode<CXProcListenForTime>(nodeStr.c_str());

        // Links for PreemptionCounter
        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"RecordingTime",  // Variable name
                                   L"Time",       // CommandLine string
                                   L"Number of seconds to listen for the events" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"LoopIterations",  // Variable name
                                   L"Loop",       // CommandLine string
                                   L"Number of times to retry calling the workhorse client" );
    }
    else
    {
        PATH_TRAP();
        throw wexception(L"Missing main option on command line! See sources.");
    }

    // In all cases, we'll need to know how many binaries are playing together and what
    // the unique string is that they are using.
    Runner.LinkParamToCmdLine( nodeStr.c_str(),
                               L"SyncKeyString",  // Variable name
                               L"SyncWith",       // CommandLine string
                               L"Unique string to define global shared memory name" );

    Runner.LinkParamToCmdLine( nodeStr.c_str(),
                               L"CountOfProcesses",  // Variable name
                               L"ProcCount",       // CommandLine string
                               L"Number of applications that will be monitored including manager" );

    Runner.LinkParamToCmdLine(nodeStr.c_str(),
                              L"QuadrantLoc",  // Variable name
                              L"Quadrant",       // CommandLine string
                              L"Number either 1,2,3 or 4 - denotes window quadrant to use" );

    // Now that we have setup our test, execute it
    Runner.Run();
}



