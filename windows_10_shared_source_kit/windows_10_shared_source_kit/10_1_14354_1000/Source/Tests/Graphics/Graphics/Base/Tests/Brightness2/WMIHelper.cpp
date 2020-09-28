#include <wbemcli.h>
#include <Wbemidl.h>
#include <atlbase.h>
#include <strsafe.h>

#include "WexTestClass.h"

#include "WMIHelper.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;
using namespace WEX::Common;

///////////////////////////////////////////////////////////////////////////////////////
//		CWMIHelper class
///////////////////////////////////////////////////////////////////////////////////////
CWMIHelper::CWMIHelper()
{
	m_pLocator = NULL;
	m_pServices = NULL;
	m_bInitialized = FALSE;
    CoInitialize(NULL);
}

CWMIHelper::~CWMIHelper()
{
	if(NULL != m_pServices)
	{
		m_pServices->Release();
		m_pServices = NULL;
	}
	if(NULL != m_pLocator)
	{
		m_pLocator->Release();
		m_pLocator = NULL;
	}
    CoUninitialize();
}


//szPath contains the name of the target machine with leading \\ for a 
//remote WMI connection.
//If szPath is NULL, then the local machine is used.
HRESULT CWMIHelper::Init(LPCTSTR szNamespace, LPCTSTR szPath)
{
	USES_CONVERSION;

	LPTSTR szFullPath = NULL;
	HRESULT hr = S_OK;
	BSTR bstrPath = NULL;
	size_t size = 0;
	size_t pathSize = 0;
	    
	if(szPath)
	{
		hr = StringCchLength(szPath, STRSAFE_MAX_CCH, &size);
		if(FAILED(hr))
		{
			return hr;
		}
		size++; //for the \ following the machine name
	}

	pathSize = size;
	hr = StringCchLength(szNamespace, STRSAFE_MAX_CCH, &size);
	if(FAILED(hr))
		return hr;

	pathSize += (size + 1);

	szFullPath = new TCHAR[pathSize];
	if(NULL == szFullPath)
	{
        Log::Comment(String().Format(L"CWMIHelper::Init: Out Of memory"));
		return E_OUTOFMEMORY;
	}
	ZeroMemory(szFullPath, pathSize);

	if(szPath)
	{
		hr = StringCchPrintf(szFullPath, pathSize, TEXT("%s\\"),szPath);
	}

	hr = StringCchCat(szFullPath, pathSize, szNamespace);

	hr =  CoInitializeSecurity(NULL, -1, NULL, NULL,
			RPC_C_AUTHN_LEVEL_CONNECT, 
			RPC_C_IMP_LEVEL_IDENTIFY, 
			NULL, EOAC_NONE, NULL
			);

	if (FAILED(hr) && (RPC_E_TOO_LATE != hr))
	{
        Log::Comment(String().Format(L"CWMIHelper::Init: CoInitializeSecurity failed. Error = %x\n", hr));
		delete[] szFullPath;
		return hr;
	}

	hr = CoCreateInstance(CLSID_WbemLocator, 0, 
		CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID *) &m_pLocator);
	
	if (FAILED(hr))
	{
        Log::Comment(String().Format(L"CWMIHelper::Init: CoCreateInstance failed. Error = %x\n", hr));
		delete[] szFullPath;
		return hr;
	}

	bstrPath = SysAllocString(T2OLE(szFullPath));

    if (!bstrPath)
	{
        Log::Comment(String().Format(L"CWMIHelper::Init: Out Of memory. bstrPath"));
		hr = E_FAIL;
   		delete[] szFullPath;
		return hr;
	}

	hr = m_pLocator->ConnectServer(bstrPath, NULL, NULL, 0, NULL, 0, 0, &m_pServices);

	SysFreeString(bstrPath);

	if (FAILED(hr))
	{
        Log::Comment(String().Format(L"CWMIHelper::Init: Failed to connect to the WMI service. Please check the path. hr = 0x%X", hr));
		m_pLocator->Release();
		m_pLocator = NULL;
		delete[] szFullPath;
		return hr;
	}

	// Set the proxy so that impersonation of the client occurs.
	hr = CoSetProxyBlanket(m_pServices, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL,
			RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL,EOAC_NONE);

	if (FAILED(hr))
	{
        Log::Comment(String().Format(L"CWMIHelper::Init: CoSetProxyBlanket failed. Error = %x\n", hr));
		m_pServices->Release();
		m_pServices = NULL;
		m_pLocator->Release();     
		m_pLocator = NULL;
		delete[] szFullPath;
		return hr;
	}

	delete[] szFullPath;
	m_bInitialized = TRUE;
	return S_OK;
}

HRESULT CWMIHelper::GetWmiObject(IWbemClassObject** ppWMIObject, LPCTSTR szClassName, LPCTSTR szInstanceName)
{
	USES_CONVERSION;

	if(!m_bInitialized)
	{
        Log::Comment(String().Format(L"CWMIHelper::GetWmiObject - Called without initializing the WMIHelper object."));
		return E_FAIL;
	}

	HRESULT hr = S_OK;
	LPTSTR szPath = NULL;
	LPTSTR pInstanceName = NULL;
	size_t size = 0;
	size_t pathSize = 0;
	IEnumWbemClassObject *pEnum = NULL;
	ULONG ulReturned = 0;
	BSTR Lang = NULL;
	BSTR Path = NULL;

	if((NULL == ppWMIObject) ||
		(NULL == szClassName) || (NULL == szInstanceName))
	{
		return E_INVALIDARG;
	}


	//Create the query
	StringCchLength(szClassName, STRSAFE_MAX_CCH, &pathSize);

	StringCchLength(TEXT(".InstanceName=\'%s\'"), 19, &size);

	pathSize += size;

	hr = StringCchLength(szInstanceName, STRSAFE_MAX_CCH, &size);
	if(FAILED(hr))
	{
		return hr;
	}

	pathSize += (size + 1);

	szPath = new TCHAR[pathSize];
	if(NULL == szPath)
	{
		return E_OUTOFMEMORY;
	}

	StringCchPrintf(szPath, pathSize, TEXT("%s.InstanceName=\'%s\'"), szClassName,
		szInstanceName);

	Path = SysAllocString(T2OLE(szPath));
    if (!Path)
	{
		hr = E_FAIL;
		goto GWO_CLEANUP;
	}

	hr =  m_pServices->GetObject(Path, WBEM_FLAG_RETURN_WBEM_COMPLETE, NULL, ppWMIObject, NULL);

GWO_CLEANUP:
	if(Path)
	{
		SysFreeString(Path);
	}
	if(FAILED(hr))
	{
		if(pInstanceName)
		{
			delete[] pInstanceName;
		}
		if(*ppWMIObject)
		{
			(*ppWMIObject)->Release();
			*ppWMIObject = NULL;
		}
	}
	if(szPath)
	{
		delete[] szPath;
	}
	return hr;
}