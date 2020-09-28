/******************************Module*Header*******************************
* Module Name: main.cpp
*
* Implementation of a test app that invokes one of 2 dxgkrnl tests
* 1.  Unmap to dummy page test (ensures that IHVs correctly implement the unmap to dummy page test)
* 2.  Aperture corruption check (fails if the dummy page has been corrupted)
*
* Author: Blake Pelton [blakep]
*
* Copyright (c) Microsoft Corporation.  All rights reserved.
**************************************************************************/
#include "basevid.h"

void Go();

class CApertureTest : public CGenericI3DComponent
{
public:
    CApertureTest();
    
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

    Runner.SetRootComponentDesc(L"This test inovkes the LDDM unmap to dummy page test or the aperture corruption check");
    
    Runner.SetTestName(L"LDDM cache coherency test");

    Runner.AddNode<CI3DDeviceComponent>(L"Device");
    SetupDeviceCommandLine(Runner, L"Device");

    Runner.SetParamTestDomainSingleValue<bool>(L"Device", L"Windowed", true);
    
    Runner.AddNode<CApertureTest>(L"Device/Test");

    const bool Unmap = Runner.IsCmdLineSet(L"Unmap");
    const bool Corruption = Runner.IsCmdLineSet(L"Corruption");
    

    if((Unmap && Corruption) || (!Unmap && !Corruption))
    {
        throw std::exception("Exactly one of Unamp or Corruption must be specified");
    }

    Runner.SetParamTestDomainSingleValue<bool>(L"Device/Test", L"Unmap", Unmap);
    Runner.SetParamTestDomainSingleValue<bool>(L"Device/Test", L"Corruption", Corruption);
    

    Runner.Run();
}

CApertureTest::CApertureTest()
{
    SetDescription(L"This components invokes the LDDM unmap to dummy page test or the aperture corruption check");
    AddParam<bool>(L"Unmap", L"If true, then the unmap to dummy page test will be performed");
    AddParam<bool>(L"Corruption", L"If true, then the corruption check will be run");
}

void CApertureTest::RunTest()
{
    const bool Unmap      = GetParam<bool>(L"Unmap");
    const bool Corruption = GetParam<bool>(L"Corruption");

    D3DKMT_VIDMM_ESCAPE VidMMEscape;
    memset(&VidMMEscape, 0, sizeof(VidMMEscape));

    if(Unmap)
    {
        VidMMEscape.Type = D3DKMT_VIDMMESCAPETYPE_RUN_UNMAP_TO_DUMMY_PAGE_TEST;
    }
    else if(Corruption)
    {
        VidMMEscape.Type = D3DKMT_VIDMMESCAPETYPE_APERTURE_CORRUPTION_CHECK;
    }
    else
    {
        throw std::exception("No arguments specified");
    }
    
    
    
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
        std::wstring sErrorMsg(L"Test failed.  ");
        if(Unmap)
        {
            sErrorMsg += L"Driver has not implemented the unmap to dummy page operation.";
        }
        else
        {
            sErrorMsg += L"Corruption on a dummy page has been detected.  This means that the GPU wrote to an unmapped aperture range during a previous test";
        }

        m_pLog->Fail(sErrorMsg.c_str());
    }
}


void CApertureTest::EndTest()
{
}

