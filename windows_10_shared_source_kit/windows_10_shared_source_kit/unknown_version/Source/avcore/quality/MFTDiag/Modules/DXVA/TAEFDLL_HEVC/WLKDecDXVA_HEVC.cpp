#include "stdafx.h"
#include "WexString.h"
#include "WLKDecDXVA_HEVC.h"
#include "DXVATests.h"
#include "FindHCKContentFile.h"

using namespace WEX::Logging;
using namespace WEX::Common;
using namespace WEX::TestExecution;

#define MFTDIAG_LOG_FILE_NAME L"MFTDiag.log"
#define MAX_LINE_BUFFER 10240
#define DEFAULT_LOG_STRING_LEN 512

#define SAFEDELETES(x) \
	{ \
		if((x) != NULL) \
		{ \
			delete (x); \
			(x) = NULL; \
		} \
	}

#define SAFEDELETE(x) \
	{ \
		if((x) != NULL) \
		{ \
			delete[] (x); \
			(x) = NULL; \
		} \
	}

#define SAFERELEASE(x) \
	{ \
		if((x) != NULL) \
		{ \
			(x)->Release(); \
			(x) = NULL; \
		} \
	}

#define SAFEFREE(x) \
	{ \
		if((x) != NULL) \
		{ \
			free(x); \
			(x) = NULL; \
		} \
	}

#define SAFECLOSE(x) \
	{ \
		if((x) != NULL) \
		{ \
			fclose(x); \
			(x) = NULL; \
		} \
	}

#define SAFEFREEBSTR(x) \
    { \
		if((x) != NULL) \
		{ \
			SysFreeString(x); \
			(x) = NULL; \
		} \
	}

namespace WLKDecDXVA_HEVC_TAEF
{
    static const WCHAR g_pszDefaultLogName[] = L"CWLKDecDXVA_HEVC_TAEF.log";
};

CWLKDecDXVA_HEVC_TAEF::CWLKDecDXVA_HEVC_TAEF(void):
    CMFTDTaefBase()
{
    m_pszDefaultLogFileName = WLKDecDXVA_HEVC_TAEF::g_pszDefaultLogName;
}

CWLKDecDXVA_HEVC_TAEF::~CWLKDecDXVA_HEVC_TAEF(void)
{
    // Nothing to do
}

bool CWLKDecDXVA_HEVC_TAEF::InitializeTestClass(void)
{
    bool    bContinueTests = true;
    String  sString;

    do
    {
        if(m_pszLogString == NULL)
        {
            m_dwCurrLogStringLen = DEFAULT_LOG_STRING_LEN;

            m_pszLogString = (WCHAR*)malloc(sizeof(WCHAR) * m_dwCurrLogStringLen);
            if(m_pszLogString == NULL)
            {
                bContinueTests = false;
                Log::Comment(L"ERROR>> Out of memory, can't allocate logging string");
                break;
            }
        }

        if(m_fpLog == NULL)
        {
            InitializeLogFile();
        }

        if(m_pXMLConfig == NULL)
        {
            if(FAILED(InitializeDXVATestTable(&m_pXMLConfig)))
            {
                Log(HIGH, L"ERROR>> Failed to initialize Test Table");
                bContinueTests = false;
                break;
            }
        }

        if(SUCCEEDED(RuntimeParameters::TryGetValue(L"VerifyEncryption", sString)))
        {
            if(_wcsicmp((const WCHAR*)sString, L"TRUE") == 0)
            {
                g_bVerifyEncryption = TRUE;
            }
            else if(_wcsicmp((const WCHAR*)sString, L"FALSE") == 0)
            {
                g_bVerifyEncryption = FALSE;
            }
            else
            {
                Log(HIGH, L"ERROR>> %s is not a valid setting for VerfiyEncryption");
                bContinueTests  = false;
                break;
            }
        }

        if(SUCCEEDED(RuntimeParameters::TryGetValue(
            L"ContentSource", 
            sString)))
        {
            m_pwszContentSourcePath = _wcsdup(
                sString
                );
            if(m_pwszContentSourcePath == NULL)
            {
                bContinueTests  = false;
                Log::Comment(L"ERROR>> Out of memory, can't allocate content search path string");
            }

            g_pwszHCKContentFileSearchPath  = m_pwszContentSourcePath;
        }

        if(SUCCEEDED(RuntimeParameters::TryGetValue(
            L"CleanupOutput", 
            sString)))
        {
            if(_wcsicmp((const WCHAR*)sString, L"TRUE") == 0)
            {
                CCMFTDiagGlobalParams::g_pParams->bAlwaysCleanOutput    = TRUE;
            }
        }

        if(SUCCEEDED(RuntimeParameters::TryGetValue(
            L"DisableDXVA", 
            sString)))
        {
            if(_wcsicmp((const WCHAR*)sString, L"TRUE") == 0)
            {
                g_dxvamodeDefault   = MFTOPOLOGY_DXVA_NONE;
            }
        }
    }while(false);

    return bContinueTests;
}

bool CWLKDecDXVA_HEVC_TAEF::FinalizeTestClass(void)
{
    return FinalizeBaseTestClass();
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest50(void)
{
    VERIFY_SUCCEEDED(RunTest(50));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest51(void)
{
    VERIFY_SUCCEEDED(RunTest(51));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest52(void)
{
    VERIFY_SUCCEEDED(RunTest(52));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest53(void)
{
    VERIFY_SUCCEEDED(RunTest(53));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest54(void)
{
    VERIFY_SUCCEEDED(RunTest(54));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest55(void)
{
    VERIFY_SUCCEEDED(RunTest(55));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest56(void)
{
    VERIFY_SUCCEEDED(RunTest(56));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest57(void)
{
    VERIFY_SUCCEEDED(RunTest(57));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest58(void)
{
    VERIFY_SUCCEEDED(RunTest(58));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest59(void)
{
    VERIFY_SUCCEEDED(RunTest(59));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest60(void)
{
    VERIFY_SUCCEEDED(RunTest(60));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest61(void)
{
    VERIFY_SUCCEEDED(RunTest(61));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest62(void)
{
    VERIFY_SUCCEEDED(RunTest(62));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest63(void)
{
    VERIFY_SUCCEEDED(RunTest(63));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest64(void)
{
    VERIFY_SUCCEEDED(RunTest(64));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCPlaybackTest65(void)
{
    VERIFY_SUCCEEDED(RunTest(65));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCFuzzTest120(void)
{
    VERIFY_SUCCEEDED(RunTest(120)); 
}

void CWLKDecDXVA_HEVC_TAEF::HEVCFuzzTest121(void)
{
    VERIFY_SUCCEEDED(RunTest(121)); 
}

void CWLKDecDXVA_HEVC_TAEF::HEVCFuzzTest122(void)
{
    VERIFY_SUCCEEDED(RunTest(122)); 
}

void CWLKDecDXVA_HEVC_TAEF::HEVCFuzzTest123(void)
{
    VERIFY_SUCCEEDED(RunTest(123)); 
}

void CWLKDecDXVA_HEVC_TAEF::HEVCFuzzTest124(void)
{
    VERIFY_SUCCEEDED(RunTest(124)); 
}

void CWLKDecDXVA_HEVC_TAEF::HEVCFuzzTest125(void)
{
    VERIFY_SUCCEEDED(RunTest(125)); 
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest230(void)
{
    VERIFY_SUCCEEDED(RunTest(230));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest231(void)
{
    VERIFY_SUCCEEDED(RunTest(231));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest232(void)
{
    VERIFY_SUCCEEDED(RunTest(232));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest233(void)
{
    VERIFY_SUCCEEDED(RunTest(233));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest234(void)
{
    VERIFY_SUCCEEDED(RunTest(234));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest235(void)
{
    VERIFY_SUCCEEDED(RunTest(235));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest236(void)
{
    VERIFY_SUCCEEDED(RunTest(236));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest237(void)
{
    VERIFY_SUCCEEDED(RunTest(237));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest238(void)
{
    VERIFY_SUCCEEDED(RunTest(238));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest239(void)
{
    VERIFY_SUCCEEDED(RunTest(239));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest240(void)
{
    VERIFY_SUCCEEDED(RunTest(240));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest241(void)
{
    VERIFY_SUCCEEDED(RunTest(241));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest242(void)
{
    VERIFY_SUCCEEDED(RunTest(242));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest243(void)
{
    VERIFY_SUCCEEDED(RunTest(243));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest244(void)
{
    VERIFY_SUCCEEDED(RunTest(244));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest245(void)
{
    VERIFY_SUCCEEDED(RunTest(245));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest246(void)
{
    VERIFY_SUCCEEDED(RunTest(246));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest247(void)
{
    VERIFY_SUCCEEDED(RunTest(247));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest248(void)
{
    VERIFY_SUCCEEDED(RunTest(248));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest249(void)
{
    VERIFY_SUCCEEDED(RunTest(249));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest250(void)
{
    VERIFY_SUCCEEDED(RunTest(250));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest251(void)
{
    VERIFY_SUCCEEDED(RunTest(251));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest252(void)
{
    VERIFY_SUCCEEDED(RunTest(252));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest253(void)
{
    VERIFY_SUCCEEDED(RunTest(253));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest254(void)
{
    VERIFY_SUCCEEDED(RunTest(254));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest255(void)
{
    VERIFY_SUCCEEDED(RunTest(255));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest256(void)
{
    VERIFY_SUCCEEDED(RunTest(256));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest257(void)
{
    VERIFY_SUCCEEDED(RunTest(257));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCQualityTest258(void)
{
    VERIFY_SUCCEEDED(RunTest(258));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCStressTest310(void)
{
    VERIFY_SUCCEEDED(RunTest(310));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCStressTest311(void)
{
    VERIFY_SUCCEEDED(RunTest(311));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCStressTest312(void)
{
    VERIFY_SUCCEEDED(RunTest(312));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCMultiInstanceTest410(void)
{
    VERIFY_SUCCEEDED(RunTest(410));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCMultiInstanceTest411(void)
{
    VERIFY_SUCCEEDED(RunTest(411));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCMultiInstanceTest412(void)
{
    VERIFY_SUCCEEDED(RunTest(412));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCMultiInstanceTest413(void)
{
    VERIFY_SUCCEEDED(RunTest(413));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCMultiInstanceTest414(void)
{
    VERIFY_SUCCEEDED(RunTest(414));
}

void CWLKDecDXVA_HEVC_TAEF::HEVCMultiInstanceTest415(void)
{
    VERIFY_SUCCEEDED(RunTest(415));
}

void CWLKDecDXVA_HEVC_TAEF::CustomXML(void)
{
    VERIFY_SUCCEEDED(RunCustomXML());
}

HRESULT CWLKDecDXVA_HEVC_TAEF::RunTestsWithIDs(
    const DWORD*    pdwTestIDs,
    const DWORD     dwNumTestIDs)
{
    HRESULT hr = S_OK;

    for(DWORD dwCurrTestIndex = 0; dwCurrTestIndex < dwNumTestIDs; dwCurrTestIndex++)
    {
        if(FAILED(RunTest(pdwTestIDs[dwCurrTestIndex])))
        {
            hr = E_FAIL;
        }
    }

    VERIFY_SUCCEEDED(hr);

    return hr;
}

HRESULT CWLKDecDXVA_HEVC_TAEF::RunActualTest(
    const DWORD dwTestID, 
    const GUID* pGUIDMFTUnderTest, 
    const WCHAR* pszHWURL)
{
    HRESULT hr  = S_OK;

    do
    {
        hr = RunDXVATest(dwTestID, this, m_pXMLConfig);
        if(FAILED(hr))
        {
            break;
        }
    }while(false);

    return hr;
}

#pragma prefast(push)
#pragma prefast(disable:28718, "disabling warning for test code")

HRESULT CWLKDecDXVA_HEVC_TAEF::GetGUIDsToTest(
    const DWORD dwTestID, 
    GUID pGUIDsToTest[], 
    WCHAR* pppszHWURLs[], 
    const DWORD dwMaxGUIDsToTest, 
    DWORD *pdwNumGUIDs)
{
    HRESULT hr = S_OK;

    do
    {
        if(pdwNumGUIDs == NULL)
        {
            hr = E_POINTER;
            break;
        }

        (*pdwNumGUIDs) = 1;
    }while(false);

    return hr;
}

HRESULT CWLKDecDXVA_HEVC_TAEF::VerifyHWRegKeys(void)
{
    return S_OK;
}

#pragma prefast(pop)
