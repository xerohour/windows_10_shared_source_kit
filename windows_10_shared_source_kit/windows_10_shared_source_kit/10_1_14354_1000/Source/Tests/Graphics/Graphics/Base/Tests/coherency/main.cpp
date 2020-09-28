/******************************Module*Header*******************************
* Module Name: main.cpp
*
* Implementation of a test app that invokes the LDDM
* aperture segment cache coherency test
*
* Author: Blake Pelton [blakep]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#include "basevid.h"

void Go();

class CCoherencyTest : public CGenericI3DComponent
{
public:
    CCoherencyTest();
    
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

    //Set assertion guid
    Runner.SetTestGUID(DISPLAY_GENERIC_GUID);

    Runner.SetRootComponentDesc(L"This test inovkes the LDDM aperture segment cache coherency test");
    
    Runner.SetTestName(L"LDDM cache coherency test");

    Runner.AddNode<CI3DDeviceComponent>(L"Device");
    SetupDeviceCommandLine(Runner, L"Device");
    
    Runner.AddNode<CCoherencyTest>(L"Device/Coherency");

    Runner.Run();
}

CCoherencyTest::CCoherencyTest()
{
    SetDescription(L"This components invokes the LDDM aperture segment cache coherency test");
}

void CCoherencyTest::RunTest()
{
    D3DKMT_VIDMM_ESCAPE VidMMEscape;
    memset(&VidMMEscape, 0, sizeof(VidMMEscape));
    VidMMEscape.Type = D3DKMT_VIDMMESCAPETYPE_RUN_COHERENCY_TEST;
    
    D3DKMT_ESCAPETYPE EscapeType = D3DKMT_ESCAPE_VIDMM;
    void* pPrivateDriverData = &VidMMEscape;
    UINT nPrivateDriverDataSize = sizeof(VidMMEscape);

    D3DDDI_ESCAPEFLAGS Flags;
    Flags.Value = 0;//no need to acquire exclusive access
    
    HRESULT hr = m_pDevice->Escape(EscapeType,
                                   Flags,
                                   pPrivateDriverData,
                                   nPrivateDriverDataSize);

    if(FAILED(hr))
    {
        throw std::exception("Coherency test failed");
    }
}


void CCoherencyTest::EndTest()
{
}

