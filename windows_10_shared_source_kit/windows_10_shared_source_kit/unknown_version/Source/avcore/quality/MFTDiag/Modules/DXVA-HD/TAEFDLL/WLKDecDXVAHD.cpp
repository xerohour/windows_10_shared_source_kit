#include "stdafx.h"
#include "WexString.h"
#include "WLKDecDXVAHD.h"
#include "DXVAHDTests.h"
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

namespace WLKDecDXVAHDTAEF
{
    static const WCHAR g_pszDefaultLogName[] = L"WLKDecDXVAHDTAEF.log";
};

CWLKDecDXVAHDTAEF::CWLKDecDXVAHDTAEF(void):
    CMFTDTaefBase()
{
    m_pszDefaultLogFileName = WLKDecDXVAHDTAEF::g_pszDefaultLogName;
}

CWLKDecDXVAHDTAEF::~CWLKDecDXVAHDTAEF(void)
{
    // Nothing to do
}

bool CWLKDecDXVAHDTAEF::InitializeTestClass(void)
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
            if(FAILED(InitializeDXVAHDTestTable(&m_pXMLConfig)))
            {
                Log(HIGH, L"ERROR>> Failed to initialize Test Table");
                bContinueTests = false;
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
    }while(false);

    return bContinueTests;
}

bool CWLKDecDXVAHDTAEF::FinalizeTestClass(void)
{
    return FinalizeBaseTestClass();
}

void CWLKDecDXVAHDTAEF::XVPTest1(void)
{
    VERIFY_SUCCEEDED(RunTest(1));
}

void CWLKDecDXVAHDTAEF::XVPTest2(void)
{
    VERIFY_SUCCEEDED(RunTest(2));
}

void CWLKDecDXVAHDTAEF::XVPTest3(void)
{
    VERIFY_SUCCEEDED(RunTest(3));
}

void CWLKDecDXVAHDTAEF::XVPTest4(void)
{
    VERIFY_SUCCEEDED(RunTest(4));
}

void CWLKDecDXVAHDTAEF::XVPTest5(void)
{
    VERIFY_SUCCEEDED(RunTest(5));
}

void CWLKDecDXVAHDTAEF::XVPTest6(void)
{
    VERIFY_SUCCEEDED(RunTest(6));
}

void CWLKDecDXVAHDTAEF::XVPTest10(void)
{
    VERIFY_SUCCEEDED(RunTest(10));
}

void CWLKDecDXVAHDTAEF::XVPTest11(void)
{
    VERIFY_SUCCEEDED(RunTest(11));
}

void CWLKDecDXVAHDTAEF::XVPTest92(void)
{
    VERIFY_SUCCEEDED(RunTest(92));
}

void CWLKDecDXVAHDTAEF::XVPTest109(void)
{
    VERIFY_SUCCEEDED(RunTest(109));
}

void CWLKDecDXVAHDTAEF::XVPTest110(void)
{
    VERIFY_SUCCEEDED(RunTest(110));
}

void CWLKDecDXVAHDTAEF::CustomXML(void)
{
    VERIFY_SUCCEEDED(RunCustomXML());
}

HRESULT CWLKDecDXVAHDTAEF::RunTestsWithIDs(
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

HRESULT CWLKDecDXVAHDTAEF::RunActualTest(
    const DWORD dwTestID, 
    const GUID* pGUIDMFTUnderTest, 
    const WCHAR* pszHWURL)
{
    HRESULT hr  = S_OK;

    do
    {
        hr = RunDXVAHDTest(dwTestID, this, m_pXMLConfig);
        if(FAILED(hr))
        {
            break;
        }
    }while(false);

    return hr;
}

#pragma prefast(push)
#pragma prefast(disable:28718, "disabling warning for test code")

HRESULT CWLKDecDXVAHDTAEF::GetGUIDsToTest(
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

HRESULT CWLKDecDXVAHDTAEF::VerifyHWRegKeys(void)
{
    return S_OK;
}

#pragma prefast(pop)
