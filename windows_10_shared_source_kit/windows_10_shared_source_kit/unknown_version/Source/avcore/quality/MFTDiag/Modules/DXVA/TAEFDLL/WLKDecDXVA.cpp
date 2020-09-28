#include "stdafx.h"
#include "WexString.h"
#include "WLKDecDXVA.h"
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

namespace WLKDecDXVATAEF
{
    static const WCHAR g_pszDefaultLogName[] = L"WLKDecDXVATAEF.log";
};

CWLKDecDXVATAEF::CWLKDecDXVATAEF(void):
    CMFTDTaefBase()
{
    m_pszDefaultLogFileName = WLKDecDXVATAEF::g_pszDefaultLogName;
}

CWLKDecDXVATAEF::~CWLKDecDXVATAEF(void)
{
    // Nothing to do
}

bool CWLKDecDXVATAEF::InitializeTestClass(void)
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

bool CWLKDecDXVATAEF::FinalizeTestClass(void)
{
    return FinalizeBaseTestClass();
}

void CWLKDecDXVATAEF::H264PlaybackTest1(void)
{
    VERIFY_SUCCEEDED(RunTest(1)); 
}

void CWLKDecDXVATAEF::H264PlaybackTest2(void)
{
    VERIFY_SUCCEEDED(RunTest(2));
}

void CWLKDecDXVATAEF::H264PlaybackTest3(void)
{
    VERIFY_SUCCEEDED(RunTest(3));
}

void CWLKDecDXVATAEF::H264PlaybackTest4(void)
{
    VERIFY_SUCCEEDED(RunTest(4));
}

void CWLKDecDXVATAEF::H264PlaybackTest5(void)
{
    VERIFY_SUCCEEDED(RunTest(5));
}

void CWLKDecDXVATAEF::H264PlaybackTest6(void)
{
    VERIFY_SUCCEEDED(RunTest(6));
}

void CWLKDecDXVATAEF::H264PlaybackTest7(void)
{
    VERIFY_SUCCEEDED(RunTest(7));
}

void CWLKDecDXVATAEF::H264PlaybackTest8(void)
{
    VERIFY_SUCCEEDED(RunTest(8));
}

void CWLKDecDXVATAEF::H264PlaybackTest9(void)
{
    VERIFY_SUCCEEDED(RunTest(9));
}

void CWLKDecDXVATAEF::H264PlaybackTest10(void)
{
    VERIFY_SUCCEEDED(RunTest(10));
}

void CWLKDecDXVATAEF::H264PlaybackTest11(void)
{
    VERIFY_SUCCEEDED(RunTest(11));
}

void CWLKDecDXVATAEF::H264PlaybackTest12(void)
{
    VERIFY_SUCCEEDED(RunTest(12));
}

void CWLKDecDXVATAEF::H264PlaybackTest13(void)
{
    VERIFY_SUCCEEDED(RunTest(13));
}

void CWLKDecDXVATAEF::H264PlaybackTest14(void)
{
    VERIFY_SUCCEEDED(RunTest(14));
}

void CWLKDecDXVATAEF::H264PlaybackTest15(void)
{
    VERIFY_SUCCEEDED(RunTest(15));
}

void CWLKDecDXVATAEF::H264PlaybackTest16(void)
{
    VERIFY_SUCCEEDED(RunTest(16));
}

void CWLKDecDXVATAEF::H264PlaybackTest17(void)
{
    VERIFY_SUCCEEDED(RunTest(17));
}

void CWLKDecDXVATAEF::H264PlaybackTest18(void)
{
    VERIFY_SUCCEEDED(RunTest(18));
}

void CWLKDecDXVATAEF::H264PlaybackTest19(void)
{
    VERIFY_SUCCEEDED(RunTest(19));
}

void CWLKDecDXVATAEF::MPEG2PlaybackTest20(void)
{
    VERIFY_SUCCEEDED(RunTest(20));
}

void CWLKDecDXVATAEF::WMVPlaybackTest32(void)
{
    VERIFY_SUCCEEDED(RunTest(32));
}

void CWLKDecDXVATAEF::WMVPlaybackTest33(void)
{
    VERIFY_SUCCEEDED(RunTest(33));
}

void CWLKDecDXVATAEF::WMVPlaybackTest34(void)
{
    VERIFY_SUCCEEDED(RunTest(34));
}

void CWLKDecDXVATAEF::WMVPlaybackTest35(void)
{
    VERIFY_SUCCEEDED(RunTest(35));
}

void CWLKDecDXVATAEF::WMVPlaybackTest36(void)
{
    VERIFY_SUCCEEDED(RunTest(36));
}

void CWLKDecDXVATAEF::WMVPlaybackTest37(void)
{
    VERIFY_SUCCEEDED(RunTest(37));
}

void CWLKDecDXVATAEF::WMVPlaybackTest38(void)
{
    VERIFY_SUCCEEDED(RunTest(38));
}

void CWLKDecDXVATAEF::WMVPlaybackTest39(void)
{
    VERIFY_SUCCEEDED(RunTest(39));
}

void CWLKDecDXVATAEF::WMVPlaybackTest40(void)
{
    VERIFY_SUCCEEDED(RunTest(40));
}

void CWLKDecDXVATAEF::WMVPlaybackTest41(void)
{
    VERIFY_SUCCEEDED(RunTest(41));
}

void CWLKDecDXVATAEF::WMVPlaybackTest42(void)
{
    VERIFY_SUCCEEDED(RunTest(42));
}

void CWLKDecDXVATAEF::WMVPlaybackTest43(void)
{
    VERIFY_SUCCEEDED(RunTest(43));
}

void CWLKDecDXVATAEF::H264FuzzTest100(void)
{
    VERIFY_SUCCEEDED(RunTest(100));
}

void CWLKDecDXVATAEF::H264FuzzTest101(void)
{
    VERIFY_SUCCEEDED(RunTest(101));
}

void CWLKDecDXVATAEF::H264FuzzTest102(void)
{
    VERIFY_SUCCEEDED(RunTest(102));
}

void CWLKDecDXVATAEF::MPEG2FuzzTest107(void)
{
    VERIFY_SUCCEEDED(RunTest(107));
}

void CWLKDecDXVATAEF::MPEG2FuzzTest108(void)
{
    VERIFY_SUCCEEDED(RunTest(108));
}

void CWLKDecDXVATAEF::MPEG2FuzzTest109(void)
{
    VERIFY_SUCCEEDED(RunTest(109));
}

void CWLKDecDXVATAEF::WMVFuzzTest114(void)
{
    VERIFY_SUCCEEDED(RunTest(114));
}

void CWLKDecDXVATAEF::WMVFuzzTest115(void)
{
    VERIFY_SUCCEEDED(RunTest(115));
}

void CWLKDecDXVATAEF::WMVFuzzTest116(void)
{
    VERIFY_SUCCEEDED(RunTest(116));
}

void CWLKDecDXVATAEF::H264QualityTest200(void)
{
    VERIFY_SUCCEEDED(RunTest(200));
}

void CWLKDecDXVATAEF::H264QualityTest201(void)
{
    VERIFY_SUCCEEDED(RunTest(201));
}

void CWLKDecDXVATAEF::H264QualityTest202(void)
{
    VERIFY_SUCCEEDED(RunTest(202));
}

void CWLKDecDXVATAEF::H264QualityTest203(void)
{
    VERIFY_SUCCEEDED(RunTest(203));
}

void CWLKDecDXVATAEF::H264QualityTest204(void)
{
    VERIFY_SUCCEEDED(RunTest(204));
}

void CWLKDecDXVATAEF::H264QualityTest205(void)
{
    VERIFY_SUCCEEDED(RunTest(205));
}

void CWLKDecDXVATAEF::MPEG2QualityTest206(void)
{
    VERIFY_SUCCEEDED(RunTest(206));
}

void CWLKDecDXVATAEF::MPEG2QualityTest207(void)
{
    VERIFY_SUCCEEDED(RunTest(207));
}

void CWLKDecDXVATAEF::MPEG2QualityTest211(void)
{
    VERIFY_SUCCEEDED(RunTest(211));
}

void CWLKDecDXVATAEF::WMVQualityTest212(void)
{
    VERIFY_SUCCEEDED(RunTest(212));
}

void CWLKDecDXVATAEF::WMVQualityTest213(void)
{
    VERIFY_SUCCEEDED(RunTest(213));
}

void CWLKDecDXVATAEF::WMVQualityTest214(void)
{
    VERIFY_SUCCEEDED(RunTest(214));
}

void CWLKDecDXVATAEF::WMVQualityTest215(void)
{
    VERIFY_SUCCEEDED(RunTest(215));
}

void CWLKDecDXVATAEF::WMVQualityTest216(void)
{
    VERIFY_SUCCEEDED(RunTest(216));
}

void CWLKDecDXVATAEF::WMVQualityTest217(void)
{
    VERIFY_SUCCEEDED(RunTest(217));
}

void CWLKDecDXVATAEF::WMVQualityTest218(void)
{
    VERIFY_SUCCEEDED(RunTest(218));
}

void CWLKDecDXVATAEF::WMVQualityTest219(void)
{
    VERIFY_SUCCEEDED(RunTest(219));
}

void CWLKDecDXVATAEF::WMVQualityTest220(void)
{
    VERIFY_SUCCEEDED(RunTest(220));
}

void CWLKDecDXVATAEF::WMVQualityTest221(void)
{
    VERIFY_SUCCEEDED(RunTest(221));
}

void CWLKDecDXVATAEF::H264QualityTest299(void)
{
    VERIFY_SUCCEEDED(RunTest(299));
}

void CWLKDecDXVATAEF::H264StressTest300(void)
{
    VERIFY_SUCCEEDED(RunTest(300));
}

void CWLKDecDXVATAEF::H264MultiInstanceTest400(void)
{
    VERIFY_SUCCEEDED(RunTest(400));
}

void CWLKDecDXVATAEF::WMVMultiInstanceTest405(void)
{
    VERIFY_SUCCEEDED(RunTest(405));
}


void CWLKDecDXVATAEF::MPEG2PlaybackTest21(void)
{
    VERIFY_SUCCEEDED(RunTest(21)); 
}

void CWLKDecDXVATAEF::MPEG2PlaybackTest22(void)
{
    VERIFY_SUCCEEDED(RunTest(22)); 
}

void CWLKDecDXVATAEF::MPEG2PlaybackTest23(void)
{
    VERIFY_SUCCEEDED(RunTest(23)); 
}

void CWLKDecDXVATAEF::MPEG2PlaybackTest24(void)
{
    VERIFY_SUCCEEDED(RunTest(24)); 
}

void CWLKDecDXVATAEF::MPEG2PlaybackTest25(void)
{
    VERIFY_SUCCEEDED(RunTest(25)); 
}

void CWLKDecDXVATAEF::MPEG2PlaybackTest27(void)
{
    VERIFY_SUCCEEDED(RunTest(27)); 
}

void CWLKDecDXVATAEF::MPEG2PlaybackTest28(void)
{
    VERIFY_SUCCEEDED(RunTest(28)); 
}

void CWLKDecDXVATAEF::MPEG2PlaybackTest30(void)
{
    VERIFY_SUCCEEDED(RunTest(30)); 
}

void CWLKDecDXVATAEF::MPEG2PlaybackTest31(void)
{
    VERIFY_SUCCEEDED(RunTest(31)); 
}

void CWLKDecDXVATAEF::MPEG2StressTest303(void)
{
    VERIFY_SUCCEEDED(RunTest(303)); 
}

void CWLKDecDXVATAEF::WMVStressTest305(void)
{
    VERIFY_SUCCEEDED(RunTest(305)); 
}

void CWLKDecDXVATAEF::MPEG2MultiInstanceTest403(void)
{
    VERIFY_SUCCEEDED(RunTest(403)); 
}

void CWLKDecDXVATAEF::WMVMultiInstanceTest406(void)
{
    VERIFY_SUCCEEDED(RunTest(406)); 
}

void CWLKDecDXVATAEF::CustomXML(void)
{
    VERIFY_SUCCEEDED(RunCustomXML());
}

HRESULT CWLKDecDXVATAEF::RunTestsWithIDs(
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

HRESULT CWLKDecDXVATAEF::RunActualTest(
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

HRESULT CWLKDecDXVATAEF::GetGUIDsToTest(
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

#pragma prefast(pop)

HRESULT CWLKDecDXVATAEF::VerifyHWRegKeys(void)
{
    return S_OK;
}
