#include "stdafx.h"
#include "MFTDTranscodeTaef.h"
#include "MFTCompsessionTests.h"

using namespace WEX::Logging;

namespace MFTDTranscode
{
    static const WCHAR g_pszDefaultLogName[] = L"MFTDiag_Transcode.log";
};

CMFTDiagTranscodeTAEF::CMFTDiagTranscodeTAEF(void)
{
    m_pszDefaultLogFileName = MFTDTranscode::g_pszDefaultLogName;

    m_dwVerifyHWKeys        = (VERIFY_HW_DECODE_REG_KEY | VERIFY_HW_ENCODE_REG_KEY | VERIFY_HW_AT_LEAST_ONE);
}

CMFTDiagTranscodeTAEF::~CMFTDiagTranscodeTAEF(void)
{
    // Nothing to do
}

bool CMFTDiagTranscodeTAEF::InitializeTestClass(void)
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
            if(FAILED(InitializeTranscodeTestTable(&m_pXMLConfig)))
            {
                Log(HIGH, L"ERROR>> Failed to initialize Test Table");
                bContinueTests = false;
                break;
            }
        }
    }while(false);

    return bContinueTests;
}

bool CMFTDiagTranscodeTAEF::FinalizeTestClass(void)
{
    return FinalizeBaseTestClass();
}

void CMFTDiagTranscodeTAEF::TranscodeTest1(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(1)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest2(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(2)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest3(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(3)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest4(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(4)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest5(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(5)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest6(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(6)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest7(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(7)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest8(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(8)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest9(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(9)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest10(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(10)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest11(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(11)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest12(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(12)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest13(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(13)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest14(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(14)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest15(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(15)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest16(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(16)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest17(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(17)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest18(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(18)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest19(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(19)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest20(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(20)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest21(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(21)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest22(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(22)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest23(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(23)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest24(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(24)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest25(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(25)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest26(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(26)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest27(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(27)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest28(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(28)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest29(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(29)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest30(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(30)); 
} 
  
void CMFTDiagTranscodeTAEF::TranscodeTest31(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(31)); 
} 

void CMFTDiagTranscodeTAEF::MITranscodeTest32(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(32)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest33(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(33)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest34(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(34)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest35(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(35)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest36(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(36)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest37(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(37)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest38(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(38)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest39(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(39)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest40(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(40)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest41(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(41)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest42(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(42)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest43(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(43)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest44(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(44)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest45(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(45)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest46(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(46)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest47(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(47)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest48(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(48)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest49(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(49)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest50(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(50)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest51(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(51)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest52(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(52)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest53(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(53)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest54(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(54)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest55(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(55)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest56(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(56)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest57(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(57)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest58(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(58)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest59(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(59)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest60(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(60)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest61(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(61)); 
} 
  
void CMFTDiagTranscodeTAEF::MITranscodeTest62(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(62)); 
}

void CMFTDiagTranscodeTAEF::CustomXML(void)
{
    VERIFY_SUCCEEDED(RunCustomXML());
}

HRESULT CMFTDiagTranscodeTAEF::RunActualTest(const DWORD dwTestID, const GUID* pGUIDMFTUnderTest, const WCHAR* pszHWURL)
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

        hr = RunTranscodeTest(dwTestID, this, m_pXMLConfig, pActivate);
    }while(false);

    if(pActivate != NULL)
    {
        pActivate->ShutdownObject();

        pActivate->Release();
    }

    return hr;
}
