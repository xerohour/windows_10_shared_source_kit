#include "stdafx.h"
#include "MFTDPlaybackTaef.h"
#include "MFTPlaybackTests.h"

using namespace WEX::Logging;

namespace MFTDPlayback
{
    static const WCHAR g_pszDefaultLogName[]        = L"MFTDiag_Playback.log";
};

namespace MFTPlaybackNS
{
    extern const WCHAR* g_pwszCMDLine    = L"te.exe MFTDPlayback_Taef.dll /select:@ChildTest='TRUE' /inproc /p:Child=TRUE /p:NoLogFile=TRUE /p:Instance=%u /p:ContextName=%s /p:ContextSize=%u";
};

CMFTDiagPlaybackTAEF::CMFTDiagPlaybackTAEF(void)
{
    m_pszDefaultLogFileName = MFTDPlayback::g_pszDefaultLogName;

    m_dwVerifyHWKeys        = VERIFY_HW_DECODE_REG_KEY;
}

CMFTDiagPlaybackTAEF::~CMFTDiagPlaybackTAEF(void)
{
    // Nothing to do
}

bool CMFTDiagPlaybackTAEF::InitializeTestClass(void)
{
    bool bContinueTests = false;

    do
    {
        bContinueTests = InitializeBaseTestClass();
        if(bContinueTests == false)
        {
            break;
        }

        if(m_pXMLConfig == NULL)
        {
            if(FAILED(InitializePlaybackTestTable(&m_pXMLConfig)))
            {
                Log(HIGH, L"ERROR>> Failed to initialize Test Table");
                bContinueTests = false;
                break;
            }
        }
    }while(false);

    return bContinueTests;
}

bool CMFTDiagPlaybackTAEF::FinalizeTestClass(void)
{
    return FinalizeBaseTestClass();
}

void CMFTDiagPlaybackTAEF::PlaybackTest110(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(110)); 
} 
  
void CMFTDiagPlaybackTAEF::PlaybackTest111(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(111)); 
} 
  
void CMFTDiagPlaybackTAEF::PlaybackTest118(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(118)); 
} 
  
void CMFTDiagPlaybackTAEF::PlaybackTest125(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(125)); 
} 
  
void CMFTDiagPlaybackTAEF::PlaybackTest129(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(129)); 
} 
  
void CMFTDiagPlaybackTAEF::PlaybackTest131(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(131)); 
} 

void CMFTDiagPlaybackTAEF::PlaybackTest150(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(150)); 
} 

void CMFTDiagPlaybackTAEF::PlaybackTest151(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(151)); 
} 

void CMFTDiagPlaybackTAEF::PlaybackTest152(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(152)); 
} 

void CMFTDiagPlaybackTAEF::PlaybackTest153(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(153)); 
} 

void CMFTDiagPlaybackTAEF::PlaybackTest154(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(154)); 
} 

void CMFTDiagPlaybackTAEF::PlaybackTest155(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(155)); 
} 

void CMFTDiagPlaybackTAEF::PlaybackTest156(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(156)); 
} 

void CMFTDiagPlaybackTAEF::PlaybackTest157(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(157)); 
} 

void CMFTDiagPlaybackTAEF::PlaybackTest421(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(421)); 
} 

void CMFTDiagPlaybackTAEF::PlaybackTest432(void)
{
    VERIFY_SUCCEEDED(RunTest(432));
}

void CMFTDiagPlaybackTAEF::PlaybackTest444(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(444)); 
}

void CMFTDiagPlaybackTAEF::PlaybackTest450(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(450)); 
} 

void CMFTDiagPlaybackTAEF::CustomXML(void)
{
    VERIFY_SUCCEEDED(RunCustomXML());
}

void CMFTDiagPlaybackTAEF::ChildTest(void)
{
    VERIFY_SUCCEEDED(RunChildTest());
}

HRESULT CMFTDiagPlaybackTAEF::RunActualTest(const DWORD dwTestID, const GUID* pGUIDMFTUnderTest, const WCHAR* pszHWURL)
{
    HRESULT hr = S_OK;
    IMFActivate* pActivate = NULL;

    do
    {
        hr = m_pActivateHelper->InitializeActivateByGUID(pGUIDMFTUnderTest, pszHWURL);
        if(FAILED(hr))
        {
            Log(HIGH, L"ERROR>> MFActivateHelper class failed to find GUID");
            break;
        }

        hr = m_pActivateHelper->GetActivate(pGUIDMFTUnderTest, pszHWURL, &pActivate);
        if(FAILED(hr))
        {
            Log(HIGH, L"ERROR>> Failed to get IMFActivate from ActivateHelper class (hr=0x%x)", hr);
            break;
        }

        hr = RunPlaybackTest(dwTestID, this, m_pXMLConfig, pActivate);
    }while(false);

    if(pActivate != NULL)
    {
        pActivate->ShutdownObject();

        pActivate->Release();
    }

    return hr;
}

HRESULT CMFTDiagPlaybackTAEF::RunActualChildTest(
    const DWORD     dwInstance,
    const WCHAR*    pwszContextName,
    const DWORD     dwContextSize)
{
    return RunChildPlaybackTest(
        this,
        dwInstance,
        pwszContextName,
        dwContextSize
        );
}
