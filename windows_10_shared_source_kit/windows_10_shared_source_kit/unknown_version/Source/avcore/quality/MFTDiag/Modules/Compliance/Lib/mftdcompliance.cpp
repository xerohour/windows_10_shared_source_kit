#include "stdafx.h"
#include "Resource.h"
#include "MFTComplianceStress.h"
#include "MFTDCompliance.h"
#include "MFTDiagCodecHelper.h"
#include "lookup.h"
#include "VerifyTestSupported.h"
#include "FindHCKContentFile.h"

// Forward declarations
HRESULT VerifyTestApplies(ILogger* pLogger, IConfig* pConfig, const GUID* pGUID, const WCHAR* pszHWURL);

#ifdef SAFEFREE
#undef SAFEFREE
#endif

#ifdef SAFEDELETES
#undef SAFEDELETES
#endif

#ifdef SAFERELEASE
#undef SAFERELEASE
#endif

#define SAFEFREE(x) \
	{ \
		if((x) != NULL) \
		{ \
			free(x); \
			(x) = NULL; \
		} \
	}

#define SAFEDELETES(x) \
	{ \
		if((x) != NULL) \
		{ \
			delete (x); \
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

static ILogger* g_pLogger = NULL;

#pragma prefast(push)
#pragma prefast(disable:28718, "disabling warning for test code")

int HWMFTLog(wchar_t* pszOutput)
{
	do
	{
		if(NULL == g_pLogger)
		{
			break;
		}
 
		g_pLogger->Log(HIGH, _T("%s"), pszOutput);

	}while(FALSE);

	return 0;
}

#pragma prefast(pop)

HRESULT InitializeComplianceTestTable(CXmlRootConfig** ppXMLRootConfig)
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
			(TCHAR*)&InitializeComplianceTestTable, &hMod) == FALSE)
		{
			hr = E_FAIL;
			break;
		}

		HRSRC hRsrc = FindResource(hMod, MAKEINTRESOURCE(IDR_COMPLIANCECONFIG), L"Config");
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

HRESULT RunComplianceTest(const DWORD dwTestID, ILogger* pLogger, CXmlRootConfig* pXMLConfig, const GUID* pGUIDToTest, 
						  const WCHAR* pszHWURL)
{
	HRESULT             hr                  = S_OK;
	CXmlConfig*         pTestConfig         = NULL;
	CMFTAPITest*        pMFTAPITest         = NULL;
	WCHAR*              pwszContentFileName = NULL;
    //CScreenIdleControl  ScreenIdle;

	hr = MFStartup(MF_VERSION);
	if(FAILED(hr))
	{
		return hr;
	}

	do
	{
		if((pLogger == NULL) || (pXMLConfig == NULL) || (pGUIDToTest == NULL))
		{
			hr = E_POINTER;
			break;
		}

        /*hr = ScreenIdle.PreventScreenFromPoweringOff();
        if(FAILED(hr))
        {
            pLogger->Log(NORMAL, L"ERROR>> Failed to set prevent screen from idling off (hr=0x%x)", hr);
            break;
        }*/

		g_pLogger = pLogger;

		pLogger->BeginTestCase(0, dwTestID);

		CString cstrTestID;
		cstrTestID.Format(_T("%u"), dwTestID);

		pTestConfig = pXMLConfig->get_TestConfig(cstrTestID);
		if(pTestConfig == NULL)
		{
			pLogger->Log(HIGH, _T("ERROR>> Test %u not found"), dwTestID);
			hr = E_FAIL;
			break;
		}

		hr = VerifyTestApplies(pLogger, pTestConfig, pGUIDToTest, pszHWURL);
		if(FAILED(hr) || (hr == S_SKIPPED))
		{
			break; 
		}

		CString cstrInputFile;
		hr = pTestConfig->GetString(L"InputFile", cstrInputFile);
		if(FAILED(hr))
		{
			pLogger->Log(HIGH, _T("ERROR>> Could not get input file from the xml"));
			hr = E_FAIL;
			break;
		}

		hr = FindHCKContentFile(
			(const WCHAR*)cstrInputFile,
			&pwszContentFileName
			);
		if(FAILED(hr))
		{
			pLogger->Log(HIGH, L"ERROR>> Failed to find input source file '%s' (hr=0x%x)", (const WCHAR*)cstrInputFile, hr);
			break;
		}

		pMFTAPITest = new CMFTAPITest();
		if(pMFTAPITest == NULL)
		{
			pLogger->Log(HIGH, _T("ERROR>> Out of memory, can't create compliance class"));
			hr = E_OUTOFMEMORY;
			break;
		}
		hr = pMFTAPITest->ParseInputParams(NULL,0);
		if(FAILED(hr))
		{
			break;
		}
		hr = pMFTAPITest->SetExternalLogger(&HWMFTLog);
		if(FAILED(hr))
		{
			break;
		}

		hr = pMFTAPITest->SetInputFile(pwszContentFileName);
		if(FAILED(hr))
		{
			pLogger->Log(HIGH, _T("ERROR>> Failed in CMFTAPITest::SetInputFile"));
			break;
		}

		hr = pMFTAPITest->SetComponentToActivate(*pGUIDToTest);
		if(FAILED(hr))
		{
			pLogger->Log(HIGH, _T("ERROR>> Failed in CMFTAPITest::SetComponentToActivate"));
			break;
		}

		hr = pMFTAPITest->SetHWURL(pszHWURL);
		if(FAILED(hr))
		{
			pLogger->Log(HIGH, _T("ERROR>> Failed in CMFTAPITest::SetHWURL"));
			break;
		}

		CString cstrMFTEnumCategory;
		hr = pTestConfig->GetString(L"MFTEnumCategory", cstrMFTEnumCategory);
		if(hr == S_OK)
		{
			hr = pMFTAPITest->SetCategory((LPCTSTR)cstrMFTEnumCategory);
			if(FAILED(hr))
			{
				pLogger->Log(HIGH, _T("ERROR>> Failed in CMFTAPITest::SetCategory"));
				hr = E_FAIL;
				break;
			}
		}

		hr = S_OK;

		pMFTAPITest->TurnOffMediaTypeVerification();

		pMFTAPITest->SetForceDX11(TRUE);

		COMPLETION_TYPE eCompletionType = TEST_PASSES;

		hr = pMFTAPITest->RunTest();
		if(FAILED(hr))
		{
			pLogger->Log(NORMAL, _T("ERROR>> Test %u Failed"), dwTestID);
			eCompletionType = TEST_FAILED;
		}
		else if(hr == S_SKIPPED)
		{
			eCompletionType = TEST_SKIPPED;
		}
		else if(hr == S_FALSE)
		{
			eCompletionType = TEST_MFTNOTUSED;

			pLogger->Log(NORMAL, _T("SUCCESS>> Test %u Passed, but Target MFT was not found"), dwTestID);
		}
		else
		{
			pLogger->Log(NORMAL, _T("SUCCESS>> Test %u Passed"), dwTestID);
		}

		pLogger->EndTestCase(eCompletionType);
	}while(false);

	SAFERELEASE(pTestConfig);
	SAFEDELETES(pMFTAPITest);
	SAFEFREE(pwszContentFileName);

	MFShutdown();

	return hr;   
}

HRESULT VerifyTestApplies(ILogger* pLogger, IConfig* pConfig, const GUID* pGUID, const WCHAR* pszHWURL)
{
	HRESULT                 hr              = S_OK;
	IMFActivate*            pActivate       = NULL;
	IMFAttributes*          pAttr           = NULL;
	IMFAttributes*          pTestAttributes = NULL;
	MFT_REGISTER_TYPE_INFO* pMediaTypes     = NULL;

	do
	{
		if((pLogger == NULL) || (pConfig == NULL) || (pGUID == NULL))
		{
			hr = E_POINTER;
			break;
		}

		hr = GetActivateForGUID(pGUID, pszHWURL, &pActivate);
		if(FAILED(hr))
		{
			pLogger->Log(HIGH, L"Unable to find MFT for specified GUID");
			break;
		}

		hr = pActivate->QueryInterface(IID_IMFAttributes, (void**)&pAttr);
		if(FAILED(hr))
		{
			pLogger->Log(HIGH, L"MF ERROR>> Query Interface(IID_IMFAttributes) failed :0x%x", hr);
			break;
		}
		if(pAttr == NULL)
		{
			hr = E_POINTER;
			pLogger->Log(HIGH, L"MF ERROR>> IMFAttribute is null :0x%x", hr);
			break;
		}

		GUID guidCategory = GUID_NULL;
		hr = pAttr->GetGUID(MF_TRANSFORM_CATEGORY_Attribute, &guidCategory);
		if(FAILED(hr))
		{
		   pLogger->Log(HIGH, L"MF ERROR>> MF_TRANSFORM_CATEGORY_Attribute not found :0x%x", hr);
			break;
		}

		CString csMFTEnumCategory;
		if(SUCCEEDED(pConfig->GetString(L"MFTEnumCategory", csMFTEnumCategory)))
		{
			GUID guidCategoryUnderTest = GUID_NULL;

			hr = GUIDFromCategoryString(csMFTEnumCategory, &guidCategoryUnderTest);
			if(FAILED(hr))
			{
				pLogger->Log(HIGH, L"ERROR>> %s is not a valid category string", (const WCHAR*)csMFTEnumCategory);
				break;
			}

			if(guidCategoryUnderTest != guidCategory)
			{
				pLogger->Log(NORMAL, L"INFO>> This test is not applicable as the component category does not match");
				hr = S_SKIPPED;
				break;
			}
		}

		UINT32 un32MediaTypeSize = 0;

		hr = pAttr->GetBlobSize(MFT_INPUT_TYPES_Attributes, &un32MediaTypeSize);
		if(FAILED(hr))
		{
			pLogger->Log(HIGH, L"MF ERROR>> MFT_INPUT_TYPES_Attributes not found: 0x%x", hr);
			break;
		}

		pMediaTypes = (MFT_REGISTER_TYPE_INFO*)malloc(sizeof(BYTE) * un32MediaTypeSize);
		if(pMediaTypes == NULL)
		{
			hr = E_OUTOFMEMORY;
			pLogger->Log(HIGH, L"ERROR>> Out of memory, can't allocate MediaType info blob");
			break;
		}

		hr = MFCreateAttributes(&pTestAttributes, 1);
		if(FAILED(hr))
		{
			pLogger->Log(HIGH, L"MF ERROR>> Failed to allocate Test Attribute Store: 0x%x", hr);
			break;
		}

		hr = pAttr->GetBlob(MFT_INPUT_TYPES_Attributes, (BYTE*)pMediaTypes, un32MediaTypeSize, NULL);
		if(FAILED(hr))
		{
			pLogger->Log(HIGH, L"MF ERROR>> Failed to get MFT_INPUT_TYPES_Attributes: 0x%x", hr);
			break;
		}

		if(FAILED(pConfig->GetAttributesFromNode(L"InputType", pTestAttributes)))
		{
			pLogger->Log(HIGH, L"TEST ERROR>> Failed to get InputType attribute from XML");
			hr = E_FAIL;
			break;
		}

		hr = VerifyTestSupported(pMediaTypes, un32MediaTypeSize / sizeof(MFT_REGISTER_TYPE_INFO), pTestAttributes);
		if(hr != S_OK)
		{
			if(hr == S_FALSE)
			{
				hr = S_SKIPPED;
				pLogger->Log(NORMAL, L"INFO>> This test is not applicable as the component doesn't support the media type");
				break;
			}
			else
			{
				pLogger->Log(HIGH, L"WARNING>> Media Type support verification failed: 0x%x", hr);
				pLogger->Log(HIGH, L"WARNING>> Test will be run");
				hr = S_OK;
			}
		}
	}while(false);

	SAFERELEASE(pAttr);
	SAFERELEASE(pTestAttributes);
	SAFERELEASE(pActivate);
	SAFEFREE(pMediaTypes);

	return hr;
}
