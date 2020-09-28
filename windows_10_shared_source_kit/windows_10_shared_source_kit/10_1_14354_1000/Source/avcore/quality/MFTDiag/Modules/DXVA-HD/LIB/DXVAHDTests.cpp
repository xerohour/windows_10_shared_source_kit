#include "stdafx.h"
#include "DXVAHDTests.h"
#include "Resource.h"
#include "FindHCKContentFile.h"
#include "XVPProcEntrypoint.h"
#include "CILoggerSmartLogger.h"
#include "testcommon.h"


#define OUT_DIR_NAME                L".\\DXVAHDOutput\\"
#define OUTPUTFILETESTNAME          L"DXVAHDTest"
#define OUTPUTFILEFORMATSTRING      L"_%u_Instance_%03u.DUMP"
#define DUMPDIRECTORY               L".\\DXVAHDVV\\"
#define DELETEFILESONSUCCEESSNAME   L"VVDeleteOnSuccess"
#define DELETEFILESONFAILURENAME    L"VVDeleteOnFailure"
#define XVP_DEFAULT_TEST_TYPE       L"Streaming"

#define DEFAULT_PSNRTHRESHOLD       50.0
#define DEFAULT_FAILPASSRATIO       0.1

enum eRenderingMode
{
    RENDER_DX9  = 0x00000001,
    RENDER_DX11 = 0x00000002,
    RENDER_BOTH = (RENDER_DX9 | RENDER_DX11),
};

/*********************************/
// Define globals required by XVPProcMFT2.dll

DBTestInfo* g_pDBTestInfo   = NULL;
DWORD       g_dwLastTestID  = 0;

/*********************************/

#ifdef  SAFERELEASE
#undef  SAFERELEASE
#endif
#define SAFERELEASE(x) \
    { \
        if( NULL != (x)) \
        { \
            (x)->Release(); \
            (x) = NULL; \
        } \
    }

#ifdef SAFEDELETES
#undef SAFEDELETES
#endif
#define SAFEDELETES(x) \
    { \
        if( NULL != (x)) \
        { \
            delete (x); \
            (x) = NULL; \
        } \
    }

#ifdef SAFEDELETE
#undef SAFEDELETE
#endif SAFEDELETE
#define SAFEDELETE(x) \
    { \
        if( NULL != (x)) \
        { \
            delete[] (x); \
            (x) = NULL; \
        } \
    }

#ifdef  SAFEFREE
#undef  SAFEFREE
#endif
#define SAFEFREE(x) \
    { \
        if( NULL != (x)) \
        { \
            free (x); \
            (x) = NULL; \
        } \
    }

#define SAFECLOSE(x) \
    { \
        if( NULL != (x)) \
        { \
            fclose(x); \
            (x) = NULL; \
        } \
    }

#define SAFECOTASKMEMFREE(x) \
    { \
        if( NULL != (x)) \
        { \
            CoTaskMemFree(x); \
            (x) = NULL; \
        } \
    } \

CDXVAHDTestContext::CDXVAHDTestContext(IConfig* pConfig, IMFActivate* pActivate): 
    CMIContext(pConfig, pActivate)
{
    dwSize          = sizeof(CDXVAHDTestContext);
    dwRenderingMode = RENDER_DX11;
    dwTimeoutSec    = 0;
    lfVVThreshold   = DEFAULT_PSNRTHRESHOLD;
    lfFailPassRatio = DEFAULT_FAILPASSRATIO;
}

CDXVAHDTestContext::~CDXVAHDTestContext(void)
{
    // Nothing to do
}

HRESULT InitializeDXVAHDTestTable(CXmlRootConfig** ppXMLRootConfig)
{
    HRESULT hr = S_OK;
    HGLOBAL hResource = NULL;
    CXmlRootConfig* pConfig = NULL;

    do
    {
        if(ppXMLRootConfig == NULL)
        {
            hr = E_POINTER;
            break;
        }

        pConfig = new CXmlRootConfig();
        if(pConfig == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        HMODULE hMod = NULL;

        if(GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            (TCHAR*)&InitializeDXVAHDTestTable, &hMod) == FALSE)
        {
            hr = E_FAIL;
            break;
        }

        HRSRC hRsrc = FindResource(hMod, MAKEINTRESOURCE(IDR_DXVATESTSCONFIG), L"Config");
        if(hRsrc == NULL)
        {
            hr = E_FAIL;
            break;
        }

        hResource = LoadResource(hMod, hRsrc);
        if(hResource == NULL)
        {
            hr = E_FAIL;
            break;
        }

        char* pszXML = (char*) LockResource(hResource);
        if(pszXML == NULL)
        {
            hr = E_FAIL;
            break;
        }

        DWORD dwXML = SizeofResource(hMod, hRsrc);
        if(dwXML == 0)
        {
            hr = E_FAIL;
            break;
        }

        int nConvertedLength = MultiByteToWideChar(CP_ACP, 0, pszXML, dwXML, NULL, NULL);

        CComBSTR bstrXML(nConvertedLength);

        if(MultiByteToWideChar(CP_ACP, 0, pszXML, dwXML, bstrXML, nConvertedLength) == 0)
        {
            hr = E_FAIL;
            break;
        }

        hr = pConfig->Init(bstrXML);
        if(FAILED(hr))
        {
            break;
        }

        *ppXMLRootConfig = pConfig;
    }while(false);

    if(hResource != NULL)
    {
        FreeResource(hResource);
    }

    if(FAILED(hr))
    {
        SAFEDELETES(pConfig);
    }

    return hr;
}

HRESULT RunDXVAHDVisValTest(ILogger* pLogger, CMIContext* pDXVAHDContext)
{
    HRESULT                 hr                  = S_OK;
    CString                 sDumpFileName       = L"";
    const WCHAR*            pwszFileNameNoPath  = NULL;
    CILoggerSmartLogger*    pSmartLogger        = NULL;

    do
    {
        if( (pLogger == NULL)           ||
            (pDXVAHDContext == NULL)    )
        {
            hr = E_POINTER;
            break;
        }

        CDXVAHDTestContext* pDXVAHDTestContext  = (CDXVAHDTestContext*)pDXVAHDContext;

        pwszFileNameNoPath  = wcsrchr((const WCHAR*)pDXVAHDTestContext->csInFileName, L'\\');
        if(pwszFileNameNoPath == NULL)
        {
            pwszFileNameNoPath = (const WCHAR*)pDXVAHDTestContext->csInFileName;
        }

        sDumpFileName   = DUMPDIRECTORY;
        sDumpFileName   += pwszFileNameNoPath;
        sDumpFileName   += L"_VisValDXVAHDTest_HW.dump";

        if(CreateDirectory(DUMPDIRECTORY, NULL) == FALSE)
        {
            DWORD dwError = GetLastError();
            if(dwError != ERROR_ALREADY_EXISTS)
            {
                pLogger->Log(HIGH, L"Failed to create Dump Directory, can't proceed with VV Dump (Error: %u)", dwError);
                break;
            }
        }

        hr              = CILoggerSmartLogger::CreateInstance(
            pLogger,
            &pSmartLogger
            );

        hr = XVPTestEntryPoint(
            (const WCHAR*)(pDXVAHDTestContext->csInFileName), 
            (const WCHAR*)sDumpFileName,
            (const WCHAR*)(pDXVAHDTestContext->csConfig),
            XVP_DEFAULT_TEST_TYPE,
            0,
            L"",
            L"",
            CUSTOM_LOG,
            pSmartLogger,
            TRUE
            );
    }while(false);

    SAFERELEASE(pSmartLogger);

    return hr;
}

HRESULT RunDXVAHDTest(DWORD dwTestID, ILogger* pLogger,  CXmlRootConfig* pXMLConfig)
{
    if(pLogger == NULL || pXMLConfig == NULL)
    {
        return E_POINTER;
    }
 
    HRESULT             hr                          = S_OK;
    CXmlConfig*         pTestConfig                 = NULL;
    CString             cstrTestID;
    CString             cstrTestName;
    CDXVAHDTestContext  DXVAHDTestContext(NULL, NULL);
    //CScreenIdleControl  ScreenIdle;
    WCHAR*              pwszContentFileName         = NULL;

    do
    {
        if(pLogger == NULL)
        {
            hr = E_POINTER;
            break;
        }

        /*hr = ScreenIdle.PreventScreenFromPoweringOff();
        if(FAILED(hr))
        {
			pLogger->Log(NORMAL, L"ERROR>> Failed to set prevent screen from idling off (hr=0x%x : %s)", hr, StringFromMFHRESULT(hr));
            break;
        }*/

        pLogger->BeginTestCase(0, dwTestID);
        cstrTestID.Format(_T("%u"), dwTestID);

        pTestConfig = pXMLConfig->get_TestConfig(cstrTestID);
        if(pTestConfig == NULL)
        {
            pLogger->Log(HIGH, _T("ERROR>> Test %u not found"), dwTestID);
            hr = E_FAIL;
            break;
        }

        pTestConfig->GetString(L"Name", cstrTestName);
        pLogger->Log(FILE_ONLY, L"Test Name: %s", cstrTestName);

        pTestConfig->GetString(L"InputFile", DXVAHDTestContext.csInFileName);
        pTestConfig->GetString(L"Category", DXVAHDTestContext.csTestCategory);
        pTestConfig->GetString(L"Config", DXVAHDTestContext.csConfig);

        hr = FindHCKContentFile(
            (const WCHAR*)DXVAHDTestContext.csInFileName,
            &pwszContentFileName
            );
        if(FAILED(hr))
        {
			pLogger->Log(HIGH, L"ERROR>> Failed to find input source file '%s' (hr=0x%x : %s)", (const WCHAR*)DXVAHDTestContext.csInFileName, hr, StringFromMFHRESULT(hr));
            break;
        }

#pragma prefast(push)
#pragma prefast(disable:26006, "disabling warning for test code")
        DXVAHDTestContext.csInFileName  = pwszContentFileName;
#pragma prefast(pop)

        if(_wcsicmp((const WCHAR*)DXVAHDTestContext.csTestCategory, L"VisVal") == 0)
        {
            hr = RunDXVAHDVisValTest(pLogger, (CMIContext*)&DXVAHDTestContext);
        }
        else
        {
            pLogger->Log(HIGH, L"%s is not a recognized test category", (const WCHAR*)DXVAHDTestContext.csTestCategory);
            hr = E_FAIL;
            break;
        }
        if(FAILED(hr))
        {
			pLogger->Log(NORMAL, _T("ERROR>> Test %u Failed hr=0x%x (%s)"), dwTestID, hr, StringFromMFHRESULT(hr));
            pLogger->EndTestCase(TEST_FAILED);
        }
        else
        {
            pLogger->Log(NORMAL, _T("SUCCESS>> Test %u Passed"), dwTestID);
            pLogger->EndTestCase(TEST_PASSES);
        }
    }while(FALSE);

    SAFEDELETES(pTestConfig);
    SAFEFREE(pwszContentFileName);

    return hr;
}
