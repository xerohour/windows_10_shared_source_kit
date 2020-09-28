#define STRSAFE_NO_DEPRECATE
#include <etwConsumersRequire.h>

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

#include <Provider.h>
#include "TestRunner.h"
#include "testcomponent.h"

#include "BaseVid.h"
#include "SurfboardComponent.h"

using namespace std;


const wchar_t* DESC = L"This is surfboard, a test for creating surfaces.";

// setup our event capturing infrastructure
CController etwController;
CDMAStandardTime etwDMAStandardTimeConsumer(&etwController,false,false); // measure time spent running on hardware
CProvider etwProvider;


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
        // Just add the consumers to the global 
        etwController.AddConsumer(&etwDMAStandardTimeConsumer);
        etwController.StartProcessing();

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
    etwController.StopProcessing();
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
    UINT iTextureCount = MAX_TEXTURE_COUNT;
    Runner.DescribeCommandLineUsage(L"bvt", L"Runs the test in BVT mode (runs much faster)");
    if (Runner.IsCmdLineSet(L"bvt") ||
        Runner.IsCmdLineSet(L"-bvt"))
    {
        iTextureCount = BVT_TEXTURE_COUNT;
    }

    /*
      First, tell the test runner the description for the test application
      (This will appear in the spec)
    */
    Runner.SetRootComponentDesc(DESC);
        
    /*
      Next we add our hierarchy of components
    */

    /*
      First add a component that will change our display mode

      This worked until asserts were reenabled
    */
    //Runner.AddNode<CDisplayModeComponent>("Mode");
    
    // Setup a node string for the Surfboard
    std::wstring nodeStr = L"";

    /*
      At the next level, we add a Device component which creates
      a 3D device and a window (if necessary)
    */
    nodeStr += L"Device";
    Runner.AddNode<CI3DDeviceComponent>( nodeStr.c_str() );

    //Setup standard cmd line options
    SetupDeviceCommandLine(Runner, L"Device");


    bool bPreemptionShader = Runner.IsCmdLineSet(L"Preempt");

    Runner.DescribeCommandLineUsage(L"Preempt",
                                    L"Set for preemption testing using a shader via the command line");

    bool bGrowTriangleList = Runner.IsCmdLineSet(L"GrowTriangleList");

    Runner.DescribeCommandLineUsage(L"Grow triangle list preemption test",
                                    L"Set for preemption testing automatically growing triangle list");

    bool bGrowTriangleSize = Runner.IsCmdLineSet(L"GrowTriangleSize");

    Runner.DescribeCommandLineUsage(L"Grow triangle size preemption test",
                                    L"Set for preemption testing automatically growing triangle size");

    bool bGrowPrimitiveCount = Runner.IsCmdLineSet(L"GrowPrimitiveCount");

    Runner.DescribeCommandLineUsage(L"Grow Primitive count preemption test",
                                    L"Set for preemption testing automatically growing Primitive count in DMA buffer");

    if( bPreemptionShader )
    {
        nodeStr += L"/PreemptionShader";
        Runner.AddNode<CPreemptionShader>(nodeStr.c_str());
    }
    else if (bGrowTriangleList)
    {
        nodeStr += L"/GrowTriangleList";
        Runner.AddNode<CGrowTriangleList>(nodeStr.c_str());
    }
    else if (bGrowTriangleSize)
    {
        nodeStr += L"/GrowTriangleSize";
        Runner.AddNode<CGrowTriangleSize>(nodeStr.c_str());
    }
    else if (bGrowPrimitiveCount)
    {
        nodeStr += L"/GrowPrimitiveCount";
        Runner.AddNode<CGrowPrimitiveCount>(nodeStr.c_str());
    }
    else
    {
        nodeStr += L"/Manager";
        Runner.AddNode<CSurfboardManager>(nodeStr.c_str());
        nodeStr += L"/Surfboard";
        Runner.AddNode<CSurfboardComponent>(nodeStr.c_str());

        Runner.SetParamTestDomainSingleValue<UINT>(nodeStr.c_str(), L"TextureCount", iTextureCount);
        Runner.SetParamTestDomainRange<UINT>(nodeStr.c_str(), L"OpCode", 0, 8*iTextureCount, 50);

        Runner.SetCycleMethod(nodeStr.c_str(), CTestRunner::ALL);
    }

    // These tests all run off the same (similar) command line.
    if( bPreemptionShader || bGrowTriangleList || bGrowTriangleSize || bGrowPrimitiveCount)
    {
        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"TotalTime",  // Variable name
                                   L"Time",       // CommandLine string
                                   L"The number of seconds to run for" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"nTriangles",
                                   L"Triangles",  // Number of triangles for each DrawPrimitive call
                                   L"The number of triangles" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"nppPrimitive",
                                   L"ppPrimitive", // number of pixels for each triangle
                                   L"number of pixels per Primitive" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"nppFrame",
                                   L"ppFrame",  // Number of DrawPrimitive calls to make before rendering
                                   L"number of primitives per frame" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"nShaderInstructions",
                                   L"ShaderInstructions", // Number of times to loop through shader.
                                   L"number of instructions for shader" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"nIterations",
                                   L"Iterations", // Number of times to loop
                                   L"number of times to loop on data" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"bWantCSVFile",
                                   L"WantCSVFile",
                                   L"Write data to a csv file" );

        Runner.LinkParamToCmdLine( nodeStr.c_str(),
                                   L"bDWM",
                                   L"DWM",
                                   L"Run on non-preemption aware hardware" );
    }

    /*
      Now that we have setup our test, execute it
    */
    Runner.Run();
}

