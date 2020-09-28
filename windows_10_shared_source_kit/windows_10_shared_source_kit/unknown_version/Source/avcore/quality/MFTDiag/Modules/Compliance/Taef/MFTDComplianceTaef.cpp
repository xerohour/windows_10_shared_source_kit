#include "stdafx.h"
#include "MFTDComplianceTaef.h"
#include "MFTDCompliance.h"

using namespace WEX::Logging;

namespace MFTDCompliance
{
    static const WCHAR g_pszDefaultLogName[] = L"MFTDiag_Compliance.log";
};

CMFTDiagComplianceTAEF::CMFTDiagComplianceTAEF(void)
{
    m_pszDefaultLogFileName = MFTDCompliance::g_pszDefaultLogName;
    
    m_dwVerifyHWKeys        = (VERIFY_HW_DECODE_REG_KEY | VERIFY_HW_ENCODE_REG_KEY | VERIFY_HW_AT_LEAST_ONE);
}

CMFTDiagComplianceTAEF::~CMFTDiagComplianceTAEF(void)
{
    // Nothing to do
}

bool CMFTDiagComplianceTAEF::InitializeTestClass(void)
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
            if(FAILED(InitializeComplianceTestTable(&m_pXMLConfig)))
            {
                Log(HIGH, L"ERROR>> Failed to initialize Test Table");
                bContinueTests = false;
                break;
            }
        }
    }while(false);

    return bContinueTests;
}

bool CMFTDiagComplianceTAEF::FinalizeTestClass(void)
{
    return FinalizeBaseTestClass();
}

void CMFTDiagComplianceTAEF::ComplianceDecodeTest13(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(13)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceDecodeTest14(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(14)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceDecodeTest15(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(15)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceDecodeTest16(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(16)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceDecodeTest17(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(17)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceDecodeTest18(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(18)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest30(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(30)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest31(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(31)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest32(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(32)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest33(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(33)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest34(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(34)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest35(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(35)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest36(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(36)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest37(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(37)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest38(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(38)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest39(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(39)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest40(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(40)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest41(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(41)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest42(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(42)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest43(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(43)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest44(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(44)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest45(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(45)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest46(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(46)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest47(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(47)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest48(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(48)); 
} 
  
void CMFTDiagComplianceTAEF::ComplianceEnccodeTest49(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(49)); 
} 

void CMFTDiagComplianceTAEF::ComplianceEnccodeTest50(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(50)); 
} 

void CMFTDiagComplianceTAEF::ComplianceEnccodeTest51(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(51)); 
} 

void CMFTDiagComplianceTAEF::ComplianceEnccodeTest60(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(60)); 
} 

void CMFTDiagComplianceTAEF::ComplianceEnccodeTest61(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(61)); 
} 

void CMFTDiagComplianceTAEF::ComplianceEnccodeTest62(void) 
{ 
    VERIFY_SUCCEEDED(RunTest(62)); 
} 

void CMFTDiagComplianceTAEF::CustomXML(void)
{
    VERIFY_SUCCEEDED(RunCustomXML());
}

HRESULT CMFTDiagComplianceTAEF::RunActualTest(const DWORD dwTestID, const GUID* pGUIDMFTUnderTest, const WCHAR* pszHWURL)
{
    return RunComplianceTest(dwTestID, this, m_pXMLConfig, pGUIDMFTUnderTest, pszHWURL);
}