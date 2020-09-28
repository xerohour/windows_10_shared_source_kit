/******************************Module*Header*******************************
* Module Name: main.cpp
*
* Implementation of a test app that invokes the VidMm
* memory segment CPU access suspension.
*
* Author: Steve Pronovost [spronovo]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#include "basevid.h"
#include "Sleepnode.h"

void Go();

class CSuspendCpuAccessTest : public CGenericI3DComponent
{
public:
    CSuspendCpuAccessTest();
    
protected:
    virtual void RunTest();
    virtual void EndTest();
};


void __cdecl main()
{
    try
    {
        Go();
    }
    catch(std::exception& e)
    {
        std::cout << e.what() << std::endl;
    }
}

void Go()
{
    CTestRunner Runner;
    bool bBVT = false;

    //Set assertion guid
    Runner.SetTestGUID(DISPLAY_GENERIC_GUID);

    Runner.SetRootComponentDesc(L"This test invokes the VidMm CPU access suspension functionnality");
    
    Runner.SetTestName(L"LDDM Suspend CPU Access");
    
    Runner.DescribeCommandLineUsage(L"BVT", L"Set to run in BVT suite");
    
    if(Runner.IsCmdLineSet(L"BVT"))
    {
        bBVT = true;
    }
 
    Runner.AddNode<CI3DDeviceComponent>(L"Device");
    SetupDeviceCommandLine(Runner, L"Device");
    Runner.SetParamTestDomainSingleValue<bool>(L"Device", L"Windowed", true);

    UINT TimesToExecute = 60;

    if(bBVT)
    {
        TimesToExecute = 10;
    }

    Runner.AddNode<CLoop>                (L"Device/Loop");
    Runner.SetParamTestDomainRange< UINT >(L"Device/Loop", L"Index", 0, TimesToExecute, 1 ); //execute the loop 100 times

    Runner.AddNode<CSleepNode>            (L"Device/Loop/Sleep");
    Runner.SetParamTestDomainSingleValue<UINT>(L"Device/Loop/Sleep", L"SleepTime", 3000 );  // 3 sec

    // Test runs for nLoop*nSleepTime msec. Under the current settnings the component runs for 3 min.

    Runner.AddNode<CSuspendCpuAccessTest>(L"Device/Loop/SuspendCpuAccess");
    
    Runner.SetParamTestDomainSingleValue<bool>(L"Device/Loop/SuspendCpuAccess", L"BVT", bBVT);

    Runner.Run();
}

CSuspendCpuAccessTest::CSuspendCpuAccessTest()
{
    SetDescription(L"This test invokes the VidMm CPU access suspension functionnality");
    AddParam<bool>(L"BVT", L"Will run in bvt mode");
}

void CSuspendCpuAccessTest::RunTest()
{
    D3DKMT_VIDMM_ESCAPE VidMMEscape;
    memset(&VidMMEscape, 0, sizeof(VidMMEscape));
    VidMMEscape.Type = D3DKMT_VIDMMESCAPETYPE_SUSPEND_CPU_ACCESS_TEST;
    
    D3DKMT_ESCAPETYPE EscapeType = D3DKMT_ESCAPE_VIDMM;
    void* pPrivateDriverData = &VidMMEscape;
    UINT nPrivateDriverDataSize = sizeof(VidMMEscape);

    D3DDDI_ESCAPEFLAGS Flags;
    Flags.Value = 0;
    Flags.HardwareAccess = 1;
    
    HRESULT hr = m_pDevice->Escape(EscapeType,
                                   Flags,
                                   pPrivateDriverData,
                                   nPrivateDriverDataSize);

    if(FAILED(hr))
    {
        if(hr != D3DDDIERR_WASSTILLDRAWING)
        {
            std::wstring strError = L"Suspend CPU Access test failed hr = ";
            strError += ToString(hr);
            m_pLog->Fail(strError.c_str());
        }
    }
}


void CSuspendCpuAccessTest::EndTest()
{
}

