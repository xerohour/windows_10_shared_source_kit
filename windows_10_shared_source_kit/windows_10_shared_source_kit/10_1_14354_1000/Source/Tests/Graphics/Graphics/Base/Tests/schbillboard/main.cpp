//--------------------------------------------------------------------------------------
// File: main.cpp
//
// A modified version of the billboard sample
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
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

#include "dx9AddGraphics.h"
#include "dx9AddWindow.h"
#include "dx9SceneGraph.h"

#include "TestRunner.h"
#include "testcomponent.h"
#include "disabledwm.h"
#include "generatepnpstop.h"

#include "billboard.h"
#include "pager.h"
#include "badapp.h"
#include "preempt.h"
#include "priority.h"
#include "allocoverhead.h"
#include "overcommit.h"
#include "schsecurity.h"

                 
using namespace std;

const wchar_t* DESC = L"Billboard sample app";

// If we want to log events anywhere in this application, we can now that we have
// a etwProvider created globally.
CProvider etwProvider;

extern void BuildPagerCommandOptions(CTestRunner* _pRunner, std::wstring& nodeStr);
extern void BuildPagerSyncCommandOptions(CTestRunner* _pRunner, std::wstring& nodeStr);
extern void BuildPreempterCommandOptions(CTestRunner* _pRunner, std::wstring& nodeStr);
extern void BuildPreempterCommandOptions(CTestRunner* _pRunner, std::wstring& nodeStr);
extern void BuildPreemptCommandOptions(CTestRunner* _pRunner, std::wstring& nodeStr);
extern void BuildAutoPreemptCommandOptions(CTestRunner* _pRunner, std::wstring& nodeStr);


//--------------------------------------------------------------------------------------
// Name: Go()
// Desc: Execute the Test
//       This can throw an exception, but really shouldn't
//--------------------------------------------------------------------------------------
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
    Runner.SetRootComponentDesc(DESC);

    Runner.DescribeCommandLineUsage(L"Pager",
                                    L"Set for memory paging operations");
    bool bPager = Runner.IsCmdLineSet(L"Pager");
    Runner.DescribeCommandLineUsage(L"PagerSync",
                                    L"Set for memory paging operations");
    bool bPagerSync = Runner.IsCmdLineSet(L"PagerSync");

    Runner.DescribeCommandLineUsage(L"Preempter",
                                    L"Used to generate preemption events");
    bool bPreempter = Runner.IsCmdLineSet(L"Preempter");


    Runner.DescribeCommandLineUsage(L"Badapp",
                                    L"Set if the app should use a large GPU time for each command buffer");
    
    bool bBadApp = Runner.IsCmdLineSet(L"Badapp");
    
    Runner.DescribeCommandLineUsage(L"Preempt",
                                    L"Set for preemption testing");
    
    bool bPreempt = Runner.IsCmdLineSet(L"Preempt");
    
    Runner.DescribeCommandLineUsage(L"AutoPreempt",
                                    L"Set for Auto preemption testing");

    bool bAutoPreempt = Runner.IsCmdLineSet(L"AutoPreempt");

    Runner.DescribeCommandLineUsage(L"Priority",
                                    L"The GPU priority of all devices in this process");
    
    bool bAllocOverhead = Runner.IsCmdLineSet(L"AllocOverhead");
    
    Runner.DescribeCommandLineUsage(L"AllocOverhead",
                                    L"Set if allocation cpu overhead should be measured");

    bool bOverCommit = Runner.IsCmdLineSet(L"OverCommit");
    
    Runner.DescribeCommandLineUsage(L"OverCommit",
                                    L"Set if a test case that over commits to video memory should be run");
    
    bool bSecurity = Runner.IsCmdLineSet(L"Security");

    Runner.DescribeCommandLineUsage(L"Security",
                                    L"Set to test security code for base GPU priority");
    
    bool bDisableDWM = Runner.IsCmdLineSet(L"DisableDWM");
    Runner.DescribeCommandLineUsage(L"DisableDWM",
                                    L"Set to Disable DWM before continuing.");

    // Next we add our hierarchy of components, let's start with nothing.
    std::wstring nodeStr = L"";

    if( bDisableDWM )
    {
        // If the user asks for DWM to be turned off, add that to our collection of tasks before 
        // anything else.
        nodeStr += L"DisableDWM";
        Runner.AddNode<CDisableDWM>( nodeStr.c_str() );

        nodeStr += L"/gpupriority";
        Runner.AddNode<CPriority>( nodeStr.c_str() );
    }
    else
    {
        // Leave DWM alone, thus we'll add our first node with not slash.
        nodeStr += L"gpupriority";
        Runner.AddNode<CPriority>( nodeStr.c_str() );
    }

    bool bPriority = Runner.IsCmdLineSet(L"Priority");
    if(bPriority)
    {
        const std::vector< std::wstring >& StringPriorities = Runner.GetCmdLineValues(L"Priority");
        std::vector< UINT > Priorities;
        for(size_t i = 0; i < StringPriorities.size(); i++)
        {
            Priorities.push_back( FromString<UINT>(StringPriorities[i].c_str()) );
        }
        
        Runner.SetParamTestDomainSingleValue<bool>(nodeStr.c_str(), L"SetPriority", true);
        Runner.SetParamTestDomain<UINT>(nodeStr.c_str(), L"Priority", Priorities.begin(), Priorities.end());
    }
    
    nodeStr += L"/grafix";
    Runner.AddNode<CDx9AddGraphics>( nodeStr.c_str() );
    nodeStr += L"/window";
    Runner.AddNode<CDx9AddWindow>( nodeStr.c_str() );

    SetupDX9CommandLine(Runner, nodeStr.c_str() );


    Runner.LinkParamToCmdLine(nodeStr.c_str(),
                              L"Width",
                              L"Width",
                              L"The width of the rendering window" );

    Runner.LinkParamToCmdLine(nodeStr.c_str(),
                              L"Height",
                              L"Height",
                              L"The height of the rendering window" );


    Runner.DescribeCommandLineUsage(L"VBlank",L"If set, then the app will wait for vertical blank before present operations");
    if(Runner.IsCmdLineSet(L"VBlank"))
    {
        Runner.SetParamTestDomainSingleValue<UINT>(nodeStr.c_str(), L"PresentInterval", D3DPRESENT_INTERVAL_ONE);
    }

    if(bBadApp)
    {
        nodeStr += L"/badapp";
        Runner.AddNode<CBadApp>( nodeStr.c_str() );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"TotalTime",
                                   L"Time",
                                   L"The number of seconds to run for" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"FillRate",
                                   L"Fill",
                                   L"The number of pixels to fill with each DrawPrimitive call (in millions)" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"Increase",
                                   L"Increase",
                                   L"If true, the number of pixels draw per call is increased linearly (starting at 0, going to FillRate), with time" );
    }
    else if(bPreempter)
    {
        BuildPreempterCommandOptions(&Runner,nodeStr);
    }
    else if(bPreempt)
    {
        BuildPreemptCommandOptions(&Runner,nodeStr);
    }
    else if(bAutoPreempt)
    {
        BuildAutoPreemptCommandOptions(&Runner,nodeStr);
    }
    else if(bPager)
    {
        BuildPagerCommandOptions(&Runner,nodeStr);
    }
    else if(bPagerSync)
    {
        BuildPagerSyncCommandOptions(&Runner,nodeStr);
    }
    else if(bAllocOverhead)
    {
        nodeStr += L"/allocoverhead";
        Runner.AddNode<CAllocOverhead>(nodeStr.c_str());
        
        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"OutputFile",
                                  L"Output",
                                  L"The name of the logfile to create");
    }
    else if(bOverCommit)
    {
        nodeStr += L"/overcommit";
        Runner.AddNode<COverCommit>(nodeStr.c_str());

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"Size",
                                  L"Size",
                                  L"The number of megabytes to allocate and gradually use");
        
        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"Time",
                                  L"Time",
                                  L"The number of seconds to run for");

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"Random",
                                  L"Random",
                                  L"If true, then the surfaces will be accessed in a random order, otherwise they will be accessed in a round-robin order");

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"Increase",
                                  L"Increase",
                                  L"If true, then the working set increases from 0 to Size over time, otherwise it stays constat at Size");
    }
    else if(bSecurity)
    {
        Runner.AddNode<CSchSecurity>(L"security");
    }
    else
    {
        nodeStr += L"/billboard";
        Runner.AddNode<CBillBoard>( nodeStr.c_str() );

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"Timeout",
                                  L"Time",
                                  L"The number of seconds to run for" );

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"UseXProc",
                                  L"XProc",
                                  L"Set to true if cross-process resource sharing should be used");

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"CPUPriorityChangeFreq",
                                  L"CPUPriorityFreq",
                                  L"The frequency with which the CPU priority of this app changes (0 to disable)");

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"SleepTime",
                                  L"Sleep",
                                  L"The number of miliseconds to sleep between each frame");

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"TextureScale",
                                  L"TextureScale",
                                  L"The amount to scale the width and height of each texture");

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"Foreground",
                                  L"Foreground",
                                  L"If true, the window is placed in the foreground");

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"Present",
                                  L"Present",
                                  L"If false, then the rendering window will be minimized, so that present operations are clipped");

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"FullScreenFreq",
                                  L"FullScreenFreq",
                                  L"If non-0, then this specifies the frequency with which the app switches between windowed and full-screen modes" );

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"Windowed",
                                  L"Windowed",
                                  L"Determines if the app starts out in windowed for full-screen mode" );

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"GPUPriority",
                                  L"VaryPriorityOffset",
                                  L"If true, the GPU priority offset of the app will be varied from -5 to 7" );

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"VaryFrameLatency",
                                  L"VaryFrameLatency",
                                  L"If true, the maximum frame latency will vary from 0 to 255 over time" );

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"SyncKeyString",  // Variable name
                                  L"SyncWith",       // CommandLine string
                                  L"Unique string to define global shared memory name" );

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"CountOfProcesses",  // Variable name
                                  L"ProcCount",       // CommandLine string
                                  L"Number of applications that will be monitored including manager" );

        Runner.LinkParamToCmdLine(nodeStr.c_str(),
                                  L"QuadrantLoc",  // Variable name
                                  L"Quadrant",       // CommandLine string
                                  L"Number either 1,2,3 or 4 - denotes window quadrant to use" );
        
    }
    
    Runner.Run();
}


//--------------------------------------------------------------------------------------
// Name: main()
// Desc: Entry point.
//       This calls Go() and catches any exception thrown by it
//--------------------------------------------------------------------------------------
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
        cout << "Caught an exception at the highest level\n";
    }    
}

