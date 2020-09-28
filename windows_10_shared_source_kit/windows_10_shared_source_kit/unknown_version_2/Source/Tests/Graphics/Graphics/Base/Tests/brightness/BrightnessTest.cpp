#include "brightnesspch.h"
#include <devioctl.h>
#include <Ntddvdeo.h>
#include <wbemidl.h>
#include <strsafe.h>
#include "WmiInstance.hpp"

////////////////////////////////////////////////////////////////////////
// CBrightnessTest class
////////////////////////////////////////////////////////////////////////
CBrightnessTest::CBrightnessTest()
{
    m_bInitialized = FALSE;
    m_Type = TT_UNINITIALIZED;
    m_bBrightnessSupported = FALSE;
}

CBrightnessTest::~CBrightnessTest()
{
    Cleanup();
    m_bInitialized = FALSE;
}
void CBrightnessTest::Cleanup()
{
    //Release all the monitor interfaces in the list
    PMONITOR_LIST_ENTRY pTemp;

    while(!IsListEmptyM(&Monitors))
    {
        pTemp = RemoveHeadListM(&Monitors);
        if(pTemp)
        {
            if(pTemp->pData)
            {
                if(pTemp->nType == LEGACY_BRIGHT_NESS_TEST)
                {
                    CloseHandle((HANDLE)(pTemp->pData));
                }
                else
                {
                    delete[] pTemp->pData;
                }
            }
            delete pTemp;
        }
    }
}

////////////////////////////////////////////////////////////////////////
// CWMIBrightnessTest class
////////////////////////////////////////////////////////////////////////
CWMIBrightnessTest::CWMIBrightnessTest()
{
    m_Type = TT_WBEM;
}

CWMIBrightnessTest::~CWMIBrightnessTest()
{
    
}

HRESULT CWMIBrightnessTest::Init(CVidWttLog* pLog)
{
    USES_CONVERSION;

    CBrightnessTest::Init(pLog);

    HRESULT hr = S_OK;
    IWbemLocator *pLoc = NULL;
    IWbemClassObject *pObj = NULL;
    IEnumWbemClassObject *pObjs = NULL;
    IWbemServices *pSvc = NULL;
    BSTR bstrPath = NULL;
    BSTR Query = NULL;
    BSTR Lang = NULL;
    BSTR strProp = NULL;
    SAFEARRAY* psaLevels = NULL;
    VARIANT v;
    VARIANT vPath;
    VARIANT vInstance;
    int nLevels = 0;
    CHAR uLevel;
    ULONG ulReturned = 0;
    PMONITOR_LIST_ENTRY pMonitor = NULL;
    BOOL bInstanceWithNoData = FALSE;
    size_t pathSize = 0;
    BOOL bValidInstance = FALSE;

    hr = m_WMIHelper.Init(TEXT("root\\WMI"), NULL);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, TEXT("Failed to initialize Wmi"));
        return hr;
    }

    pSvc = m_WMIHelper.GetWMIServices();
    if(!pSvc)
    {
        return E_FAIL;
    }

    Query = SysAllocString(L"SELECT * from WmiMonitorBrightness");
    if(Query == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto CLEANUP_IWMI;
    }
    
    Lang = SysAllocString(L"WQL");
    if(Lang == NULL)
    {
        hr = E_OUTOFMEMORY;
        goto CLEANUP_IWMI;
    }

    hr = pSvc->ExecQuery(Lang, Query, WBEM_FLAG_FORWARD_ONLY, NULL, &pObjs);
    if(FAILED(hr))
    {
        if(hr == wbemErrInvalidClass)
        {
            Log(LOG_WARNING, TEXT("WmiMonitorBrightness class not registered. The monitor.sys driver is probably missing."), hr);
            hr = S_FALSE;
        }
        else if(hr == wbemErrNotSupported)
        {
            //No instances of brightness controls found
            hr = S_FALSE;
        }
        goto CLEANUP_IWMI;
    }
    hr = pObjs->Next(WBEM_INFINITE, 1, &pObj, &ulReturned);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, TEXT("IEnumWbemClassObject::Next failed. Error = 0x%08x"), hr);
        goto CLEANUP_IWMI;
    }
    else if(hr == S_FALSE)
    {
        goto CLEANUP_IWMI;
    }
    
    VariantInit(&v);
    VariantInit(&vPath);
    VariantInit(&vInstance);

    while(SUCCEEDED(hr) && (hr != S_FALSE))
    {
        hr = pObj->Get(L"__CLASS", 0, &v, NULL, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get the __CLASS property. Error = 0x%08x"), hr);
            goto CLEANUP_IWMI;
        }
        
        hr = pObj->Get(L"__PATH", 0, &vPath, NULL, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get the __PATH property. Error = 0x%08x"), hr);
            goto CLEANUP_IWMI;
        }

        hr = pObj->Get(L"InstanceName", 0, &vInstance, NULL, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get the __PATH property. Error = 0x%08x"), hr);
            goto CLEANUP_IWMI;
        }

        strProp = SysAllocString(L"Levels");
        if(strProp == NULL)
        {
            hr = E_OUTOFMEMORY;
            goto CLEANUP_IWMI;
        }

        VariantClear(&v);
        hr = pObj->Get(strProp, 0, &v, 0, 0);
        SysFreeString(strProp);
        if(SUCCEEDED(hr) && (V_VT(&v) == VT_I4))
        {
            HRESULT hRes = ReportsBrightnessLevels(&v, &vPath);
            VariantClear(&v);
            VariantClear(&vPath);

            if(!hRes)
            {
                goto CLEANUP_IWMI;
            }
            else
            {
                hr = CreateNewMonitor(&pMonitor, vInstance);
                VariantClear(&vInstance);

                if(FAILED(hr))
                {
                    goto CLEANUP_IWMI;
                }
                
                InsertTailListM(&Monitors, pMonitor);

                bValidInstance = TRUE;
            }
            hr = pObjs->Next(WBEM_INFINITE, 1, &pObj, &ulReturned);	
        }
        else
        {
            Log(LOG_ERROR, TEXT("Failed to get the %s property. Error = 0x%08x"), strProp, hr);
        }
    }

CLEANUP_IWMI:
    if(FAILED(hr))
    {
        if(hr == E_OUTOFMEMORY)
        {
            Log(LOG_ERROR, TEXT("Out of memory. Error = 0x%08x"), hr);
        }
        
        Cleanup();
    }
    if(bstrPath != NULL)
    {
        SysFreeString(bstrPath);
    }
    if(Query != NULL)
    {
        SysFreeString(Query);
    }
    if(Lang != NULL)
    {
        SysFreeString(Lang);
    }
    if(pLoc != NULL)
    {
        pLoc->Release();
    }
    if(bValidInstance && (hr == S_FALSE))
    {
        hr = S_OK;
    }

    return hr;    	
}

HRESULT CWMIBrightnessTest::CreateNewMonitor(PMONITOR_LIST_ENTRY* ppMonitor, VARIANT vInstance)
{
    USES_CONVERSION;
    
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pMonitor = NULL;
    LPTSTR szInstance = NULL;
    LPTSTR szNewInstance = NULL;
    size_t size = 0;

    if((NULL == ppMonitor))
    {
        return E_INVALIDARG;
    }

    pMonitor = new MONITOR_LIST_ENTRY;
    if(NULL == pMonitor)
    {
        hr = E_OUTOFMEMORY;
        goto CNM_CLEANUP;
    }
    __try
    {
        szInstance = OLE2T(V_BSTR(&vInstance));
    }
    __except(GetExceptionCode() == STATUS_STACK_OVERFLOW)
    {
        _resetstkoflw();
        Log(LOG_ERROR, TEXT("Stack Overflow!"));
        hr = E_FAIL;
        goto CNM_CLEANUP;
    }
    hr = StringCchLength(szInstance, STRSAFE_MAX_CCH, &size);
    if(FAILED(hr))
    {
        goto CNM_CLEANUP;
    }

    szNewInstance = new TCHAR[size+1];
    if(!szNewInstance)
    {
        hr = E_OUTOFMEMORY;
        goto CNM_CLEANUP;
    }

    StringCchCopy(szNewInstance, size+1 , szInstance);
    
    pMonitor->pData = szNewInstance;
    pMonitor->nType = BRIGHTNESS_TEST;
    *ppMonitor = pMonitor;
CNM_CLEANUP:
    if(FAILED(hr))
    {
        if(pMonitor)
        {
            delete pMonitor;
        }
        if(szNewInstance)
        {
            delete[] szNewInstance;
        }
    }
    return hr;
}
BOOL CWMIBrightnessTest::ReportsBrightnessLevels(VARIANT* pvValue, VARIANT* pvPath)
{
    USES_CONVERSION;

    if(V_I4(pvValue) == 0)
    {
        __try
        {
            Log(LOG_ERROR, TEXT("0 brightness levels found for the instance : %s"), W2T(V_BSTR(pvPath)));
        }
        __except(GetExceptionCode() == STATUS_STACK_OVERFLOW)
        {
            _resetstkoflw();
        }
        return FALSE;
    }
    else
    {
        return TRUE;
    }
}

HRESULT CWMIBrightnessTest::GetCurrentBrightness(DWORD dwMonitorIndex, BYTE* pbLevel)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pEntry = NULL;
    IWbemClassObject* pMonitor = NULL;
    
    if(pbLevel == NULL)
        return E_INVALIDARG;

    //Retrieve the monitor from the list of monitors
    if(IsListEmptyM(&Monitors))
    {
        return E_FAIL;
    }
    hr = GetHeadListItemM(&Monitors, dwMonitorIndex, &pEntry);
    if(FAILED(hr))
        return hr;
    
    return GetCurrentBrightness(((LPCSTR)pEntry->pData), pbLevel);

}

HRESULT CWMIBrightnessTest::GetCurrentBrightness(LPCSTR szInstanceName, BYTE* pbLevel)
{
    USES_CONVERSION;
    
    BSTR strProp = NULL;
    VARIANT varPath;
    VARIANT varBrightness;
    HRESULT hr = S_OK;
    IWbemClassObject* pMonitor = NULL;

    if((szInstanceName == NULL) || (pbLevel == NULL))
        return E_INVALIDARG;

    hr = m_WMIHelper.GetWmiObject(&pMonitor, TEXT("WmiMonitorBrightness"), szInstanceName);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, TEXT("GetWmiObject failed for the path %s. hr = %08x"), szInstanceName, hr);
        return hr;
    }
        
    VariantInit(&varPath);
    hr = pMonitor->Get(L"__PATH", 0, &varPath, NULL, NULL);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, TEXT("Failed to get the object __PATH property"));
        return hr;
    }

    VariantInit(&varBrightness);
    strProp = SysAllocString(L"CurrentBrightness");
    if(strProp == NULL)
    {
        return E_OUTOFMEMORY;
    }
    hr = pMonitor->Get(strProp, 0, &varBrightness, 0, 0);

    if(SUCCEEDED(hr) && (V_VT(&varBrightness) == VT_UI1))
    {
        *pbLevel = V_UI1(&varBrightness);
        /*__try
        {
            Log(LOG_MSG, TEXT("CurrentBrightness level for %s is %d"), W2T(V_BSTR(&varPath)), *pbLevel);
        }
        __except(GetExceptionCode() == STATUS_STACK_OVERFLOW)
        {
            _resetstkoflw();			
        }
        //wprintf(L"\n%s : %d\n", strProp, V_UI1(&v));
        */
    }
    else
    {
        __try
        {
            Log(LOG_ERROR, TEXT("Failed to get the CurrentBrightness value for %s"), W2T(V_BSTR(&varPath)));
        }
        __except(GetExceptionCode() == STATUS_STACK_OVERFLOW)
        {
            _resetstkoflw();		
        }
    }

    pMonitor->Release();
    SysFreeString(strProp);
    VariantClear(&varPath);
    VariantClear(&varBrightness);
    return hr;
}

HRESULT CWMIBrightnessTest::GetSupportedLevels(DWORD dwMonitorIndex, __out_ecount(cSize)LPBYTE pabLevels, size_t cSize, LONG* plLevels)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pEntry = NULL;
    IWbemClassObject* pMonitor = NULL;
    
    if(plLevels == NULL)
        return E_INVALIDARG;
    
    //Retrieve the monitor from the list of monitors
    if(IsListEmptyM(&Monitors))
    {
        return E_FAIL;
    }
    hr = GetHeadListItemM(&Monitors, dwMonitorIndex, &pEntry);
    if(FAILED(hr))
        return hr;
    
    return GetSupportedLevels(((LPCSTR)pEntry->pData), pabLevels, cSize, plLevels);	
}

HRESULT CWMIBrightnessTest::GetSupportedLevels(LPCSTR szInstanceName, __out_ecount(cSize)LPBYTE pabLevels, size_t cSize, LONG* plLevels)
{
    USES_CONVERSION;
    
    BSTR strProp = NULL;
    VARIANT varPath;
    VariantInit(&varPath);
    VARIANT varLevels;
    VariantInit(&varLevels);
    VARIANT varLevel;
    VariantInit(&varLevel);
    long lLevels = 0;
    long lBound = 0;
    SAFEARRAY* psaLevels = NULL;
    BYTE bLevel;
    HRESULT hr = S_OK;
    IWbemClassObject* pMonitor = NULL;

    if((szInstanceName == NULL) || (plLevels == NULL))
        return E_INVALIDARG;
    
    //Clear the input array
    ZeroMemory(pabLevels, sizeof(BYTE)*cSize);

    hr = m_WMIHelper.GetWmiObject(&pMonitor, TEXT("WmiMonitorBrightness"), szInstanceName);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, TEXT("GetWmiObject failed for the path %s. hr = %08x"), szInstanceName, hr);
        return hr;
    }
    
    hr = pMonitor->Get(L"__PATH", 0, &varPath, NULL, NULL);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, TEXT("Failed to get the object __PATH property"));
        return hr;
    }

    strProp = SysAllocString(L"Levels");
    if(strProp == NULL)
    {
        return E_OUTOFMEMORY;
    }
    
    hr = pMonitor->Get(strProp, 0, &varLevels, 0, 0);
    SysFreeString(strProp);
    if(SUCCEEDED(hr) && (V_VT(&varLevels) == VT_I4))
    {
        //wprintf(L"%s (%d) %d\n", strProp, varLevels.vt, V_I4(&v));
        
        lLevels = V_I4(&varLevels);
        strProp = SysAllocString(L"Level");
        if(strProp == NULL)
        {
            return E_OUTOFMEMORY;
        }
        hr = pMonitor->Get(strProp, 0, &varLevel, 0, 0);
        SysFreeString(strProp);
        if(SUCCEEDED(hr) && (V_VT(&varLevel) == (VT_ARRAY | VT_UI1)))
        {
            psaLevels = V_ARRAY(&varLevel);

            for(long i = 0; (i < lLevels) && (i < (long)cSize) ; i++)
            {
                hr = SafeArrayGetElement(psaLevels, &i, &bLevel);
                if(FAILED(hr))
                {
                    break;
                }
                pabLevels[i] = bLevel;
            }
        }
        else
        {
            Log(LOG_ERROR, TEXT("Failed to get the Level property"));
        }
        
    }
    else
    {
        Log(LOG_ERROR, TEXT("Failed to get the Levels property"));
    }
    
    if(FAILED(hr))
    {
        ZeroMemory(psaLevels, sizeof(BYTE)*cSize);
        *plLevels = 0;
    }
    else
    {
        *plLevels = lLevels;
    }

    pMonitor->Release();
    VariantClear(&varPath);
    VariantClear(&varLevels);
    VariantClear(&varLevel);

    return hr;
}

HRESULT CWMIBrightnessTest::SetLevel(DWORD dwMonitorIndex, BYTE bLevel)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pEntry = NULL;
    IWbemClassObject* pMonitor = NULL;
    
    //Retrieve the monitor from the list of monitors
    if(IsListEmptyM(&Monitors))
    {
        return E_FAIL;
    }
    hr = GetHeadListItemM(&Monitors, dwMonitorIndex, &pEntry);
    if(FAILED(hr))
        return hr;
    
    return SetLevel(((LPCSTR)pEntry->pData), bLevel);

}

HRESULT CWMIBrightnessTest::SetLevel(LPCSTR szInstanceName, BYTE bLevel)
{
    USES_CONVERSION;

    HRESULT hr = S_OK;
    
    IWbemLocator* pLocator = NULL;
    IWbemServices* pNamespace = 0;
    IWbemClassObject* pClass = NULL;
    IWbemClassObject* pInClass = NULL;
    IWbemClassObject* pInInst = NULL;
    IWbemClassObject* pBrightnessMethods = NULL;
    IWbemClassObject *pBrightnessMethodsClass = NULL;
    IWbemClassObject* pMonitor = NULL;

    BSTR bstrClassPath = NULL;
    BSTR bstrMethodName = NULL;
    VARIANT vPath;
    VariantInit(&vPath);

    __try
    {
        IWbemServices *pSvc = m_WMIHelper.GetWMIServices();
        if(!pSvc)
        {
            Log(LOG_ERROR, TEXT("CWMIBrightnessTest::SetLevel - Failed to get IWBemServices"));
            return E_FAIL;
        }

        bstrClassPath = SysAllocString(L"WmiMonitorBrightnessMethods");
        if(bstrClassPath == NULL)
        {
            return E_OUTOFMEMORY;
        }

        BSTR strProp = NULL;
        VARIANT varPath;
        VARIANT varBrightness;

        if(szInstanceName == NULL)
            return E_INVALIDARG;

        hr = m_WMIHelper.GetWmiObject(&pMonitor, TEXT("WmiMonitorBrightness"), szInstanceName);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("GetWmiObject failed for the path %s. hr = %08x"), szInstanceName, hr);
            goto WMI_SL;
        }

        VariantInit(&varPath);
        hr = pMonitor->Get(L"InstanceName", 0, &varPath, NULL, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get the object Instance property"));
            return hr;
        }

        hr = m_WMIHelper.GetWmiObject(&pBrightnessMethods, TEXT("WmiMonitorBrightnessMethods"), OLE2T(V_BSTR(&varPath)));
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("GetWmiObject failed for the path %s. hr = %08x"), OLE2T(V_BSTR(&varPath)), hr);
            goto WMI_SL;
        }

        //Get the path of the object
        hr = pBrightnessMethods->Get(L"__PATH", 0, &vPath, NULL, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get the __PATH property for the instance %s. hr = %08x"), OLE2T(V_BSTR(&varPath)), hr);
            goto WMI_SL;
        }

//		Log(LOG_MSG, TEXT("__PATH = %s"), W2T(V_BSTR(&vPath)));
        
        bstrMethodName = SysAllocString(T2OLE(TEXT("WmiSetBrightness")));
        if(NULL == bstrMethodName)
        {
            hr = E_OUTOFMEMORY;
            goto WMI_SL;
        }

        hr = pSvc->GetObject(bstrClassPath, 0, NULL, &pBrightnessMethodsClass, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get the WmiMonitorBrightnessMethods Class. hr = %08x"), hr);
            goto WMI_SL;
        }
        if(NULL == pBrightnessMethodsClass)
        {
            Log(LOG_ERROR, TEXT("pBrightnessMethodsClass is NULL although GetObject succeeded"));
            hr = E_FAIL;
            goto WMI_SL;
        }
        
        hr = pBrightnessMethodsClass->GetMethod(L"WmiSetBrightness", 0, &pInClass, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("GetMethod failed for the method WmiSetBrightness. hr = %08x"), hr);
            goto WMI_SL;
        }

        hr = pInClass->SpawnInstance(0, &pInInst);
        if (SUCCEEDED(hr))
        {
            VARIANT vBrightness, vTimeout;

            // Create the values for the in parameters
            VariantInit(&vBrightness);
            vBrightness.vt = VT_UI1;
            vBrightness.bVal = bLevel;

            VariantInit(&vTimeout);
            vTimeout.vt = VT_I4;
            vTimeout.ullVal = 0;


            // Store the value for the in parameters
            hr = pInInst->Put(L"Brightness", 0, &vBrightness, CIM_UINT8);
            if(FAILED(hr))
            {
                Log(LOG_ERROR, TEXT("Failed to put the Brightness input parameter. hr = %08x"), hr);
                goto WMI_SL;
            }
            

            hr = pInInst->Put(L"Timeout", 0, &vTimeout, CIM_UINT32);
            if(FAILED(hr))
            {
                Log(LOG_ERROR, TEXT("Failed to put the Timeout input parameter. hr = %08x"), hr);
                goto WMI_SL;
            }
            hr = pSvc->ExecMethod(V_BSTR(&vPath), bstrMethodName, 0, NULL, pInInst, NULL, NULL);
            if(FAILED(hr))
            {
                Log(LOG_ERROR, TEXT("Failed to execute the method WmiSetBrightness for the instance %s. hr = %08x"), OLE2T(V_BSTR(&vPath)), hr);
                goto WMI_SL;
            }

            VariantClear(&vBrightness);
            VariantClear(&vTimeout);
        }

        VariantClear(&varPath);
        VariantClear(&varBrightness);
    }
    __except(GetExceptionCode() == STATUS_STACK_OVERFLOW)
    {
        _resetstkoflw();
        hr = ERROR_STACK_OVERFLOW;
    }


WMI_SL:
    if(bstrClassPath)
    {
        SysFreeString(bstrClassPath);
    }
    if(bstrMethodName)
    {
        SysFreeString(bstrMethodName);
    }
    if(pBrightnessMethods)
    {
        pBrightnessMethods->Release();
        pBrightnessMethods = NULL;
    }

    if(pInClass)
    {
        pInClass->Release();
    }
    if(pMonitor)
    {
        pMonitor->Release();
    }

    VariantClear(&vPath);

    return hr;
}

HRESULT CWMIBrightnessTest::SetALSLevel(DWORD dwMonitorIndex, BYTE bLevel)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pEntry = NULL;
    IWbemClassObject* pMonitor = NULL;
    
    //Retrieve the monitor from the list of monitors
    if(IsListEmptyM(&Monitors))
    {
        return E_FAIL;
    }
    hr = GetHeadListItemM(&Monitors, dwMonitorIndex, &pEntry);
    if(FAILED(hr))
        return hr;
    
    return SetALSLevel(((LPCSTR)pEntry->pData), bLevel);

}

HRESULT CWMIBrightnessTest::SetALSLevel(LPCSTR szInstanceName, BYTE bLevel)
{
    USES_CONVERSION;

    HRESULT hr = S_OK;
    
    IWbemLocator* pLocator = NULL;
    IWbemServices* pNamespace = 0;
    IWbemClassObject* pClass = NULL;
    IWbemClassObject* pInClass = NULL;
    IWbemClassObject* pInInst = NULL;
    IWbemClassObject* pBrightnessMethods = NULL;
    IWbemClassObject *pBrightnessMethodsClass = NULL;
    IWbemClassObject* pMonitor = NULL;

    BSTR bstrClassPath = NULL;
    BSTR bstrMethodName = NULL;
    VARIANT vPath;

    __try
    {
        IWbemServices *pSvc = m_WMIHelper.GetWMIServices();
        if(!pSvc)
        {
            Log(LOG_ERROR, TEXT("CWMIBrightnessTest::SetALSLevel - Failed to get IWBemServices"));
            return E_FAIL;
        }

        bstrClassPath = SysAllocString(L"WmiMonitorBrightnessMethods");
        if(bstrClassPath == NULL)
        {
            return E_OUTOFMEMORY;
        }

        BSTR strProp = NULL;
        VARIANT varPath;
        VARIANT varBrightness;

        if(szInstanceName == NULL)
            return E_INVALIDARG;

        hr = m_WMIHelper.GetWmiObject(&pMonitor, TEXT("WmiMonitorBrightness"), szInstanceName);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("GetWmiObject failed for the path %s. hr = %08x"), szInstanceName, hr);
            goto WMI_SAL;
        }

        VariantInit(&varPath);
        hr = pMonitor->Get(L"InstanceName", 0, &varPath, NULL, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get the object Instance property"));
            return hr;
        }

        hr = m_WMIHelper.GetWmiObject(&pBrightnessMethods, TEXT("WmiMonitorBrightnessMethods"), OLE2T(V_BSTR(&varPath)));
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("GetWmiObject failed for the path %s. hr = %08x"), OLE2T(V_BSTR(&varPath)), hr);
            goto WMI_SAL;
        }

        //Get the path of the object
        VariantInit(&vPath);
        hr = pBrightnessMethods->Get(L"__PATH", 0, &vPath, NULL, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get the __PATH property for the instance %s. hr = %08x"), OLE2T(V_BSTR(&varPath)), hr);
            goto WMI_SAL;
        }

//		Log(LOG_MSG, TEXT("__PATH = %s"), W2T(V_BSTR(&vPath)));
        
        bstrMethodName = SysAllocString(T2OLE(TEXT("WmiSetALSBrightness")));
        if(NULL == bstrMethodName)
        {
            hr = E_OUTOFMEMORY;
            goto WMI_SAL;
        }

        hr = pSvc->GetObject(bstrClassPath, 0, NULL, &pBrightnessMethodsClass, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get the WmiMonitorBrightnessMethods Class. hr = %08x"), hr);
            goto WMI_SAL;
        }
        if(NULL == pBrightnessMethodsClass)
        {
            Log(LOG_ERROR, TEXT("pBrightnessMethodsClass is NULL although GetObject succeeded"));
            hr = E_FAIL;
            goto WMI_SAL;
        }
        
        hr = pBrightnessMethodsClass->GetMethod(L"WmiSetALSBrightness", 0, &pInClass, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("GetMethod failed for the method WmiSetALSBrightness. hr = %08x"), hr);
            goto WMI_SAL;
        }

        hr = pInClass->SpawnInstance(0, &pInInst);
        if (SUCCEEDED(hr))
        {
            VARIANT vBrightness;

            // Create the values for the in parameters
            VariantInit(&vBrightness);
            vBrightness.vt = VT_UI1;
            vBrightness.bVal = bLevel;


            // Store the value for the in parameters
            hr = pInInst->Put(L"Brightness", 0, &vBrightness, CIM_UINT8);
            if(FAILED(hr))
            {
                Log(LOG_ERROR, TEXT("Failed to put the Brightness input parameter. hr = %08x"), hr);
                goto WMI_SAL;
            }
            
            hr = pSvc->ExecMethod(V_BSTR(&vPath), bstrMethodName, 0, NULL, pInInst, NULL, NULL);
            if(FAILED(hr))
            {
                Log(LOG_ERROR, TEXT("Failed to execute the method WmiSetALSBrightness for the instance %s. hr = %08x"), OLE2T(V_BSTR(&vPath)), hr);
                goto WMI_SAL;
            }

            VariantClear(&vPath);
            VariantClear(&vBrightness);
        }
        VariantClear(&varPath);
        VariantClear(&varBrightness);
   }
    __except(GetExceptionCode() == STATUS_STACK_OVERFLOW)
    {
        _resetstkoflw();
        hr = ERROR_STACK_OVERFLOW;
    }


WMI_SAL:
    if(bstrClassPath)
    {
        SysFreeString(bstrClassPath);
    }
    if(bstrMethodName)
    {
        SysFreeString(bstrMethodName);
    }
    if(pBrightnessMethods)
    {
        pBrightnessMethods->Release();
        pBrightnessMethods = NULL;
    }

    if(pInClass)
    {
        pInClass->Release();
    }

    if(pMonitor)
    {
        pMonitor->Release();
    }

    VariantClear(&vPath);
    
    return hr;
}

HRESULT CWMIBrightnessTest::SetALSState(DWORD dwMonitorIndex, BOOLEAN bState)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pEntry = NULL;
    IWbemClassObject* pMonitor = NULL;
    
    //Retrieve the monitor from the list of monitors
    if(IsListEmptyM(&Monitors))
    {
        return E_FAIL;
    }
    hr = GetHeadListItemM(&Monitors, dwMonitorIndex, &pEntry);
    if(FAILED(hr))
        return hr;
    
    return SetALSState(((LPCSTR)pEntry->pData), bState);

}

HRESULT CWMIBrightnessTest::SetALSState(LPCSTR szInstanceName, BOOLEAN bState)
{
    USES_CONVERSION;

    HRESULT hr = S_OK;
    
    IWbemLocator* pLocator = NULL;
    IWbemServices* pNamespace = 0;
    IWbemClassObject* pClass = NULL;
    IWbemClassObject* pInClass = NULL;
    IWbemClassObject* pInInst = NULL;
    IWbemClassObject* pBrightnessMethods = NULL;
    IWbemClassObject *pBrightnessMethodsClass = NULL;
    IWbemClassObject* pMonitor = NULL;

    BSTR bstrClassPath = NULL;
    BSTR bstrMethodName = NULL;
    VARIANT vPath;
    VariantInit(&vPath);

    __try
    {
        IWbemServices *pSvc = m_WMIHelper.GetWMIServices();
        if(!pSvc)
        {
            Log(LOG_ERROR, TEXT("CWMIBrightnessTest::SetALSLevel - Failed to get IWBemServices"));
            return E_FAIL;
        }

        bstrClassPath = SysAllocString(L"WmiMonitorBrightnessMethods");
        if(bstrClassPath == NULL)
        {
            return E_OUTOFMEMORY;
        }

        BSTR strProp = NULL;
        VARIANT varPath;

        if(szInstanceName == NULL)
            return E_INVALIDARG;

        hr = m_WMIHelper.GetWmiObject(&pMonitor, TEXT("WmiMonitorBrightness"), szInstanceName);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("GetWmiObject failed for the path %s. hr = %08x"), szInstanceName, hr);
            goto WMI_SABS;
        }

        VariantInit(&varPath);
        hr = pMonitor->Get(L"InstanceName", 0, &varPath, NULL, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get the object Instance property"));
            return hr;
        }

        hr = m_WMIHelper.GetWmiObject(&pBrightnessMethods, TEXT("WmiMonitorBrightnessMethods"), OLE2T(V_BSTR(&varPath)));
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("GetWmiObject failed for the path %s. hr = %08x"), OLE2T(V_BSTR(&varPath)), hr);
            goto WMI_SABS;
        }

        //Get the path of the object
        hr = pBrightnessMethods->Get(L"__PATH", 0, &vPath, NULL, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get the __PATH property for the instance %s. hr = %08x"), OLE2T(V_BSTR(&varPath)), hr);
            goto WMI_SABS;
        }

//		Log(LOG_MSG, TEXT("__PATH = %s"), W2T(V_BSTR(&vPath)));
        
        bstrMethodName = SysAllocString(T2OLE(TEXT("WmiSetALSBrightnessState")));
        if(NULL == bstrMethodName)
        {
            hr = E_OUTOFMEMORY;
            goto WMI_SABS;
        }

        hr = pSvc->GetObject(bstrClassPath, 0, NULL, &pBrightnessMethodsClass, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get the WmiMonitorBrightnessMethods Class. hr = %08x"), hr);
            goto WMI_SABS;
        }
        if(NULL == pBrightnessMethodsClass)
        {
            Log(LOG_ERROR, TEXT("pBrightnessMethodsClass is NULL although GetObject succeeded"));
            hr = E_FAIL;
            goto WMI_SABS;
        }
        
        hr = pBrightnessMethodsClass->GetMethod(L"WmiSetALSBrightnessState", 0, &pInClass, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("GetMethod failed for the method WmiSetALSBrightness. hr = %08x"), hr);
            goto WMI_SABS;
        }

        hr = pInClass->SpawnInstance(0, &pInInst);
        if (SUCCEEDED(hr))
        {
            VARIANT vState;

            // Create the values for the in parameters
            VariantInit(&vState);
            vState.vt = VT_BOOL;
            vState.boolVal = bState;


            // Store the value for the in parameters
            hr = pInInst->Put(L"State", 0, &vState, CIM_BOOLEAN);
            if(FAILED(hr))
            {
                Log(LOG_ERROR, TEXT("Failed to put the Brightness input parameter. hr = %08x"), hr);
                goto WMI_SABS;
            }
            
            hr = pSvc->ExecMethod(V_BSTR(&vPath), bstrMethodName, 0, NULL, pInInst, NULL, NULL);
            if(FAILED(hr))
            {
                Log(LOG_ERROR, TEXT("Failed to execute the method WmiSetALSBrightnessState for the instance %s. hr = %08x"), OLE2T(V_BSTR(&vPath)), hr);
                goto WMI_SABS;
            }
            VariantClear(&vState);
        }
        VariantClear(&varPath);
    }
    __except(GetExceptionCode() == STATUS_STACK_OVERFLOW)
    {
        _resetstkoflw();
        hr = ERROR_STACK_OVERFLOW;
    }


WMI_SABS:
    if(bstrClassPath)
    {
        SysFreeString(bstrClassPath);
    }
    if(bstrMethodName)
    {
        SysFreeString(bstrMethodName);
    }
    if(pBrightnessMethods)
    {
        pBrightnessMethods->Release();
        pBrightnessMethods = NULL;
    }

    if(pInClass)
    {
        pInClass->Release();
    }

    if(pMonitor)
    {
        pMonitor->Release();
    }

    VariantClear(&vPath);
    
    return hr;
}

HRESULT CWMIBrightnessTest::RevertToPolicyBrightness(DWORD dwMonitorIndex)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pEntry = NULL;
    IWbemClassObject* pMonitor = NULL;
    
    //Retrieve the monitor from the list of monitors
    if(IsListEmptyM(&Monitors))
    {
        return E_FAIL;
    }
    hr = GetHeadListItemM(&Monitors, dwMonitorIndex, &pEntry);
    if(FAILED(hr))
        return hr;
    
    return RevertToPolicyBrightness((LPCSTR)pEntry->pData);

}

HRESULT CWMIBrightnessTest::RevertToPolicyBrightness(LPCSTR szInstanceName)
{
    USES_CONVERSION;

    HRESULT hr = S_OK;
    
    IWbemLocator* pLocator = NULL;
    IWbemServices* pNamespace = 0;
    IWbemClassObject* pClass = NULL;
    IWbemClassObject* pInClass = NULL;
    IWbemClassObject* pInInst = NULL;
    IWbemClassObject* pBrightnessMethods = NULL;
    IWbemClassObject *pBrightnessMethodsClass = NULL;
    IWbemClassObject* pMonitor = NULL;

    BSTR bstrClassPath = NULL;
    BSTR bstrMethodName = NULL;
    VARIANT vPath;
    VariantInit(&vPath);

    __try
    {
        IWbemServices *pSvc = m_WMIHelper.GetWMIServices();
        if(!pSvc)
        {
            Log(LOG_ERROR, TEXT("CWMIBrightnessTest::SetALSLevel - Failed to get IWBemServices"));
            return E_FAIL;
        }

        bstrClassPath = SysAllocString(L"WmiMonitorBrightnessMethods");
        if(bstrClassPath == NULL)
        {
            return E_OUTOFMEMORY;
        }

        BSTR strProp = NULL;
        VARIANT varPath;

        if(szInstanceName == NULL)
            return E_INVALIDARG;

        hr = m_WMIHelper.GetWmiObject(&pMonitor, TEXT("WmiMonitorBrightness"), szInstanceName);
        if(FAILED(hr))
    {
        Log(LOG_ERROR, TEXT("GetWmiObject failed for the path %s. hr = %08x"), szInstanceName, hr);
        goto WMI_RTPB;
    }

        VariantInit(&varPath);
        hr = pMonitor->Get(L"InstanceName", 0, &varPath, NULL, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get the object Instance property"));
            return hr;
        }

        hr = m_WMIHelper.GetWmiObject(&pBrightnessMethods, TEXT("WmiMonitorBrightnessMethods"), OLE2T(V_BSTR(&varPath)));
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("GetWmiObject failed for the path %s. hr = %08x"), OLE2T(V_BSTR(&varPath)), hr);
            goto WMI_RTPB;
        }

        //Get the path of the object
        hr = pBrightnessMethods->Get(L"__PATH", 0, &vPath, NULL, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get the __PATH property for the instance %s. hr = %08x"), OLE2T(V_BSTR(&varPath)), hr);
            goto WMI_RTPB;
        }

//		Log(LOG_MSG, TEXT("__PATH = %s"), W2T(V_BSTR(&vPath)));
        
        bstrMethodName = SysAllocString(T2OLE(TEXT("WmiRevertToPolicyBrightness")));
        if(NULL == bstrMethodName)
        {
            hr = E_OUTOFMEMORY;
            goto WMI_RTPB;
        }

        hr = pSvc->GetObject(bstrClassPath, 0, NULL, &pBrightnessMethodsClass, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get the WmiMonitorBrightnessMethods Class. hr = %08x"), hr);
            goto WMI_RTPB;
        }
        if(NULL == pBrightnessMethodsClass)
        {
            Log(LOG_ERROR, TEXT("pBrightnessMethodsClass is NULL although GetObject succeeded"));
            hr = E_FAIL;
            goto WMI_RTPB;
        }
        
/*		hr = pBrightnessMethodsClass->GetMethod(L"WmiRevertToPolicyBrightness", 0, &pInClass, NULL);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("GetMethod failed for the method WmiRevertToPolicyBrightness. hr = %08x"), hr);
            goto WMI_RTPB;
        }

        hr = pInClass->SpawnInstance(0, &pInInst);
        if (SUCCEEDED(hr))*/
        {
            hr = pSvc->ExecMethod(V_BSTR(&vPath), bstrMethodName, 0, NULL, NULL, NULL, NULL);
            if(FAILED(hr))
            {
                Log(LOG_ERROR, TEXT("Failed to execute the method WmiRevertToPolicyBrightness for the instance %s. hr = %08x"), OLE2T(V_BSTR(&vPath)), hr);
                goto WMI_RTPB;
            }

        }
    }
    __except(GetExceptionCode() == STATUS_STACK_OVERFLOW)
    {
        _resetstkoflw();
        hr = ERROR_STACK_OVERFLOW;
    }


WMI_RTPB:
    if(bstrClassPath)
    {
        SysFreeString(bstrClassPath);
    }
    if(bstrMethodName)
    {
        SysFreeString(bstrMethodName);
    }
    if(pBrightnessMethods)
    {
        pBrightnessMethods->Release();
        pBrightnessMethods = NULL;
    }
    
    VariantClear(&vPath);

    return hr;
}
////////////////////////////////////////////////////////////////////////
// CWMIBrightnessCAPITest class
////////////////////////////////////////////////////////////////////////
CWMIBrightnessCAPITest::CWMIBrightnessCAPITest()
{
    m_Type = TT_CAPI;
}

CWMIBrightnessCAPITest::~CWMIBrightnessCAPITest()
{
    
}

HRESULT CWMIBrightnessCAPITest::Init(CVidWttLog* pLog)
{
    USES_CONVERSION;

    CBrightnessTest::Init(pLog);

    string_vector v;
    string* str;
    BOOL res;
    ULONG size;
    BOOL bValidInstance = FALSE;
    PMONITOR_LIST_ENTRY pMonitor = NULL;
    HRESULT hr = S_OK;

    v = GetInstanceNames(&WmiMonitorBrightness_GUID);

    if(v.size() < 1)
    {
        return S_FALSE;
        
    }
    for (ULONG i  = 0; i < v.size(); i++) 
    {
        pMonitor = new MONITOR_LIST_ENTRY;
        if(pMonitor == NULL)
        {
        Log(LOG_ERROR, TEXT("Out of memory"));
        return E_OUTOFMEMORY;
        }
        str = new string(v[i].c_str());
        pMonitor->pData = str;
        pMonitor->nType = BRIGHTNESS_CAPI_TEST;
        InsertTailListM(&Monitors, pMonitor);
    }
    v.clear();

    return hr;    	
}

string_vector CWMIBrightnessCAPITest::GetInstanceNames(const GUID* Guid)
{
    string_vector v;
    HANDLE   hDataBlock;
    LPBYTE buffer;
    DWORD size, ret;

    size = 0x100;
    buffer = NULL;

    //  Get the entire Wnode
    //
    if (WmiOpenBlock((LPGUID) Guid, 0, &hDataBlock) != ERROR_SUCCESS) {
        return v;
    }

    do 
    {
        if (buffer != NULL) 
        {
            delete[] buffer;
            buffer = NULL;
        }

        buffer = new BYTE[size];
        ZeroMemory(buffer, size);

        ret = WmiQueryAllData(hDataBlock, &size, buffer);
    } while (ret == ERROR_INSUFFICIENT_BUFFER);

    WmiCloseBlock(hDataBlock);

    if (ret != ERROR_SUCCESS) 
    {
        goto Done;
    }

    LPBYTE cur = buffer;

    while (cur != NULL) 
    {
        PWNODE_ALL_DATA pWnode;
        ULONG *offsets, i;

        pWnode = (PWNODE_ALL_DATA) cur;
        offsets = (PULONG) OFFSET_TO_PTR(pWnode,
            pWnode->OffsetInstanceNameOffsets);

        for (i = 0; i < pWnode->InstanceCount; i++) 
        {
            CHAR* buf;
            USHORT length;

            length = *(PUSHORT) OFFSET_TO_PTR(pWnode, offsets[i]);
            buf = (CHAR*) OFFSET_TO_PTR(pWnode, offsets[i] + sizeof(USHORT));
            

            v.push_back(string(buf, length));
        }

        if (pWnode->WnodeHeader.Linkage == 0) 
        {
            cur = NULL;
        }
        else 
        {
            cur += pWnode->WnodeHeader.Linkage;
        }
    }

Done:
    if (buffer != NULL) 
    {
        delete[] buffer;
    }

    return v;
}

HRESULT CWMIBrightnessCAPITest::GetCurrentBrightness(DWORD dwMonitorIndex, BYTE* pbLevel)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pEntry = NULL;
    
    if(pbLevel == NULL)
        return E_INVALIDARG;

    //Retrieve the monitor from the list of monitors
    if(IsListEmptyM(&Monitors))
    {
        return E_FAIL;
    }
    hr = GetHeadListItemM(&Monitors, dwMonitorIndex, &pEntry);
    if(FAILED(hr))
        return hr;
    
    return GetCurrentBrightness(((string*)pEntry->pData)->c_str(), pbLevel);

}

HRESULT CWMIBrightnessCAPITest::GetCurrentBrightness(LPCSTR szInstanceName, BYTE* pbLevel)
{
    HRESULT hr = S_OK;
    BOOL res;
    CWmiInstance<WmiMonitorBrightness> instance;
    WmiMonitorBrightness* pMonitorBrightness = NULL;

    res = instance.Open(szInstanceName, WmiMonitorBrightness_GUID, TRUE);
    
    if (res == FALSE) 
    {
        Log(LOG_ERROR, ("Could not find an instance of WmiMonitorBrightness with ID %s\n"), szInstanceName);
        hr = E_FAIL;
        goto GCB_INIT_CLEANUP;
    }

    res = instance.Get(&pMonitorBrightness);
    if(res == FALSE)
    {
        Log(LOG_ERROR, ("Failed to get the Wmi data for the WmiMonitorBrightness instance %s\n"), szInstanceName);
        hr = E_FAIL;
        goto GCB_INIT_CLEANUP;
    }

    *pbLevel = pMonitorBrightness->CurrentBrightness;

GCB_INIT_CLEANUP:
    if(instance.IsOpened())
    {
        instance.Close();
    }
    return hr;
}

HRESULT CWMIBrightnessCAPITest::GetSupportedLevels(DWORD dwMonitorIndex, __out_ecount(cSize)LPBYTE pabLevels, size_t cSize, LONG* plLevels)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pEntry = NULL;
    
    if(plLevels == NULL)
        return E_INVALIDARG;
    
    //Retrieve the monitor from the list of monitors
    if(IsListEmptyM(&Monitors))
    {
        return E_FAIL;
    }
    hr = GetHeadListItemM(&Monitors, dwMonitorIndex, &pEntry);
    if(FAILED(hr))
        return hr;
    
    return GetSupportedLevels(((string*)pEntry->pData)->c_str(), pabLevels, cSize, plLevels);


}

HRESULT CWMIBrightnessCAPITest::GetSupportedLevels(LPCTSTR szInstanceName, __out_ecount(cSize)LPBYTE pabLevels, size_t cSize, LONG* plLevels)
{
    HRESULT hr = S_OK;
    CWmiInstance<WmiMonitorBrightness> instance;
    WmiMonitorBrightness* pMonitorBrightness = NULL;
    BOOL res;

    res = instance.Open(szInstanceName, WmiMonitorBrightness_GUID, TRUE);
    
    if (res == FALSE) 
    {
        Log(LOG_ERROR, ("Could not find an instance of WmiMonitorBrightness with ID %s\n"), szInstanceName);
        hr = E_FAIL;
        goto GSL_INIT_CLEANUP;
    }

    res = instance.Get(&pMonitorBrightness);
    if(res == FALSE)
    {
        Log(LOG_ERROR, ("Failed to get the Wmi data for the WmiMonitorBrightness instance %s\n"), szInstanceName);
        hr = E_FAIL;
        goto GSL_INIT_CLEANUP;
    }

    *plLevels= pMonitorBrightness->Levels;

    for(ULONG i = 0; (i < pMonitorBrightness->Levels) && (i < cSize); i++)
    {
        if(FAILED(hr))
        {
            break;
        }
        pabLevels[i] = pMonitorBrightness->Level[i];
    }
    
GSL_INIT_CLEANUP:
    if(instance.IsOpened())
    {
        instance.Close();
    }
    return hr;
}

HRESULT CWMIBrightnessCAPITest::SetLevel(DWORD dwMonitorIndex, BYTE bLevel)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pEntry = NULL;
    IWbemClassObject* pMonitor = NULL;
    
    //Retrieve the monitor from the list of monitors
    if(IsListEmptyM(&Monitors))
    {
        return E_FAIL;
    }
    hr = GetHeadListItemM(&Monitors, dwMonitorIndex, &pEntry);
    if(FAILED(hr))
        return hr;
    
    return SetLevel(((string*)pEntry->pData)->c_str(), bLevel);

}

HRESULT CWMIBrightnessCAPITest::SetLevel(LPCSTR szInstanceName, BYTE bLevel)
{
    HRESULT hr = S_OK;
    CWmiInstance<ULONG> instance; //the type here does not matter
    BOOL res;
    WmiSetBrightness_IN input;

    res = instance.Open(szInstanceName, WmiMonitorBrightnessMethods_GUID, TRUE);
    
    if (res == FALSE) 
    {
        Log(LOG_ERROR, ("Could not find an instance of WmiMonitorBrightnessMethods with ID %s\n"), szInstanceName);
        hr = E_FAIL;
        goto SLC_INIT_CLEANUP;
    }

    input.Brightness = bLevel;
    input.Timeout = 0;

    res = instance.ExecuteMethod(WmiSetBrightness,
         &input,
         sizeof(input),
         NULL,
         NULL);
    if (res == FALSE) 
    {
        Log(LOG_ERROR, ("Failed to execute the WmiSetBrightness method for the instance %s\n"), szInstanceName);
        hr = E_FAIL;
        goto SLC_INIT_CLEANUP;
    }

SLC_INIT_CLEANUP:
    if(instance.IsOpened())
    {
        instance.Close();
    }
    return hr;
}


HRESULT CWMIBrightnessCAPITest::SetALSLevel(DWORD dwMonitorIndex, BYTE bLevel)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pEntry = NULL;
    IWbemClassObject* pMonitor = NULL;
    
    //Retrieve the monitor from the list of monitors
    if(IsListEmptyM(&Monitors))
    {
        return E_FAIL;
    }
    hr = GetHeadListItemM(&Monitors, dwMonitorIndex, &pEntry);
    if(FAILED(hr))
        return hr;
    
    return SetALSLevel(((string*)pEntry->pData)->c_str(), bLevel);

}

HRESULT CWMIBrightnessCAPITest::SetALSLevel(LPCSTR szInstanceName, BYTE bLevel)
{
    HRESULT hr = S_OK;
    CWmiInstance<ULONG> instance; //the type here does not matter
    BOOL res;
    WmiSetALSBrightness_IN input;

    res = instance.Open(szInstanceName, WmiMonitorBrightnessMethods_GUID, TRUE);
    
    if (res == FALSE) 
    {
        Log(LOG_ERROR, ("Could not find an instance of WmiMonitorBrightnessMethods with ID %s\n"), szInstanceName);
        hr = E_FAIL;
        goto SALC_INIT_CLEANUP;
    }

    input.Brightness = bLevel;

    res = instance.ExecuteMethod(WmiSetALSBrightness,
         &input,
         sizeof(input),
         NULL,
         NULL);
    if (res == FALSE) 
    {
        Log(LOG_ERROR, ("Failed to execute the WmiSetALSBrightness method for the instance %s\n"), szInstanceName);
        hr = E_FAIL;
        goto SALC_INIT_CLEANUP;
    }

SALC_INIT_CLEANUP:
    if(instance.IsOpened())
    {
        instance.Close();
    }
    return hr;
}

HRESULT CWMIBrightnessCAPITest::SetALSState(DWORD dwMonitorIndex, BOOLEAN bState)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pEntry = NULL;
    IWbemClassObject* pMonitor = NULL;
    
    //Retrieve the monitor from the list of monitors
    if(IsListEmptyM(&Monitors))
    {
        return E_FAIL;
    }
    hr = GetHeadListItemM(&Monitors, dwMonitorIndex, &pEntry);
    if(FAILED(hr))
        return hr;
    
    return SetALSState(((string*)pEntry->pData)->c_str(), bState);

}

HRESULT CWMIBrightnessCAPITest::SetALSState(LPCSTR szInstanceName, BOOLEAN bState)
{
    HRESULT hr = S_OK;
    CWmiInstance<ULONG> instance; //the type here does not matter
    BOOL res;
    WmiSetALSBrightnessState_IN input;

    res = instance.Open(szInstanceName, WmiMonitorBrightnessMethods_GUID, TRUE);
    
    if (res == FALSE) 
    {
        Log(LOG_ERROR, ("Could not find an instance of WmiMonitorBrightnessMethods with ID %s\n"), szInstanceName);
        hr = E_FAIL;
        goto SALSC_INIT_CLEANUP;
    }

    input.State = bState;

    res = instance.ExecuteMethod(WmiSetALSBrightnessState,
         &input,
         sizeof(input),
         NULL,
         NULL);
    if (res == FALSE) 
    {
        Log(LOG_ERROR, ("Failed to execute the WmiSetALSBrightnessState method for the instance %s\n"), szInstanceName);
        hr = E_FAIL;
        goto SALSC_INIT_CLEANUP;
    }

SALSC_INIT_CLEANUP:
    if(instance.IsOpened())
    {
        instance.Close();
    }
    return hr;
}

HRESULT CWMIBrightnessCAPITest::RevertToPolicyBrightness(DWORD dwMonitorIndex)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pEntry = NULL;
    IWbemClassObject* pMonitor = NULL;
    
    //Retrieve the monitor from the list of monitors
    if(IsListEmptyM(&Monitors))
    {
        return E_FAIL;
    }
    hr = GetHeadListItemM(&Monitors, dwMonitorIndex, &pEntry);
    if(FAILED(hr))
        return hr;
    
    return RevertToPolicyBrightness(((string*)pEntry->pData)->c_str());

}

HRESULT CWMIBrightnessCAPITest::RevertToPolicyBrightness(LPCSTR szInstanceName)
{
    HRESULT hr = S_OK;
    CWmiInstance<ULONG> instance; //the type here does not matter
    BOOL res;

    res = instance.Open(szInstanceName, WmiMonitorBrightnessMethods_GUID, TRUE);
    
    if (res == FALSE) 
    {
        Log(LOG_ERROR, ("Could not find an instance of WmiMonitorBrightnessMethods with ID %s\n"), szInstanceName);
        hr = E_FAIL;
        goto RTPB_CAPI_CLEANUP;
    }

    res = instance.ExecuteMethod(WmiRevertToPolicyBrightness,
         NULL,
         0,
         NULL,
         NULL);
    if (res == FALSE) 
    {
        Log(LOG_ERROR, ("Failed to execute the WmiRevertToPolicyBrightness method for the instance %s\n"), szInstanceName);
        hr = E_FAIL;
        goto RTPB_CAPI_CLEANUP;
    }

RTPB_CAPI_CLEANUP:
    if(instance.IsOpened())
    {
        instance.Close();
    }
    return hr;
}


VOID EventFiredCallback(
    PWNODE_HEADER WnodeHeader,
    ULONG_PTR Context
    )
{
    PWNODE_SINGLE_INSTANCE pNode;
    USHORT length;
    WCHAR* buf;

    pNode = (PWNODE_SINGLE_INSTANCE)WnodeHeader;

    length = *(PUSHORT)OFFSET_TO_PTR(pNode, pNode->OffsetInstanceName);
    buf = (WCHAR*) OFFSET_TO_PTR(pNode, pNode->OffsetInstanceName + sizeof(USHORT));

    wstring instance(buf, length);

    if (memcmp(&WmiMonitorBrightnessEvent_GUID,
               &WnodeHeader->Guid,
               sizeof(GUID)) == 0) 
    {
        SetEvent((HANDLE) Context);
    }
}

HRESULT CWMIBrightnessCAPITest::TestBrightnessEvent(DWORD dwMonitorIndex)
{
    string_vector v;
    BOOL res;

    ULONG result;
    HANDLE event;
    BYTE abLevels[256];
    LONG lLevels = 0;	
    BYTE bCurrentLevel = 0;
    BYTE bLevelToSet = 0;
    HRESULT hr = GetSupportedLevels(dwMonitorIndex, abLevels, ARRAYSIZE(abLevels), &lLevels);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, ("Could not get the supported levels for monitor %d"), dwMonitorIndex);
        return hr;
    }

    hr = GetCurrentBrightness(dwMonitorIndex, &bCurrentLevel);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, ("Could not get the current brightness level for monitor %d"), dwMonitorIndex);
        return hr;
    }

    //if any levels reported, create an event and try to set a reported level  different from the current level to see if it generates an event.

    event = CreateEvent(NULL, FALSE, FALSE, NULL);
    if (event == NULL) 
    {
        Log(LOG_ERROR, ("Failed to create an event object. Monitor Index %d"), dwMonitorIndex);
        return E_FAIL;
    }
    
    int i = 0;
    bLevelToSet = bCurrentLevel;
    for(i = lLevels-1 ;i >= 0; i--)
    {
        if(abLevels[i] != bCurrentLevel)
        {
            bLevelToSet = abLevels[i];
            break;
        }
    }

    if(bCurrentLevel == bLevelToSet)
    {
        Log(LOG_ERROR, ("Failed to find a brightness level different from the current brightness level = %d. Monitor Index %d"), bCurrentLevel, dwMonitorIndex);
        return E_FAIL;
    }

    Log(LOG_MSG, ("Brightness test level for the brightness event = %d"), bLevelToSet);
    

    result = WmiNotificationRegistration(
        (LPGUID) &WmiMonitorBrightnessEvent_GUID,
        TRUE,
        EventFiredCallback,
        (ULONG_PTR) event,
        NOTIFICATION_CALLBACK_DIRECT
        );

    if (result != ERROR_SUCCESS) 
    {
        goto Done;
    }

    hr = SetLevel(dwMonitorIndex, bLevelToSet);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, ("SetLevel for the level %d failed for moniter %d"), bLevelToSet, dwMonitorIndex);
        goto Done;
    }
    
    // 3 seconds should be enough
    result = WaitForSingleObject(event, 3000);
    //    ASSERT(result == WAIT_OBJECT_0);

    if (result != WAIT_OBJECT_0) 
    {
        Log(LOG_ERROR, ("Timed out waiting for the brightness event. Monitor Index%d"), dwMonitorIndex);
        hr = E_FAIL;
        goto Done;
    }
    
    WmiNotificationRegistration(
        (LPGUID) &WmiMonitorBrightnessEvent_GUID,
        FALSE,
        EventFiredCallback,
        (ULONG_PTR) event,
        NOTIFICATION_CALLBACK_DIRECT
        );

    //Verify that the current brightness level is the one we set
    BYTE bLevelTemp;
    hr = GetCurrentBrightness(dwMonitorIndex, &bLevelTemp);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, ("Could not get the current brightness level for monitor %d"), dwMonitorIndex);
        goto Done;
    }
    if(bLevelTemp != bLevelToSet)
    {
        Log(LOG_ERROR, ("Current brightness level after the event fired does not match the one expected for the monitor %d. Current Level = %d, Expected Level = %d"), 
            dwMonitorIndex, bLevelTemp, bLevelToSet);
        goto Done;
    }
    
    //restore the original brightness level
    hr = SetLevel(dwMonitorIndex, bCurrentLevel);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, ("Failed to restore brightness level %d for moniter %d"), bCurrentLevel, dwMonitorIndex);
        goto Done;
    }
    

Done:
    CloseHandle(event);
    return hr;
}

////////////////////////////////////////////////////////////////////////
// CLegacyBrightnessTest class
////////////////////////////////////////////////////////////////////////
CLegacyBrightnessTest::CLegacyBrightnessTest()
{
    m_Type = TT_LEGACY;
}

CLegacyBrightnessTest::~CLegacyBrightnessTest()
{
    Cleanup();
}

HRESULT CLegacyBrightnessTest::Init(CVidWttLog* pLog)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pMonitor = NULL;
    
    CBrightnessTest::Init(pLog);

    HANDLE hLCD = CreateFile("\\\\.\\LCD",GENERIC_READ,
                        FILE_SHARE_READ,
                        NULL,	
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL);


    if (INVALID_HANDLE_VALUE == hLCD)
    {
        return S_FALSE;
    }
    else
    {
        Log(LOG_MSG, TEXT("Opened the \\\\.\\LCD device. Handle = 0x%08x"), hLCD);
    }

    pMonitor = new MONITOR_LIST_ENTRY;
    if(pMonitor == NULL)
    {
        CloseHandle(hLCD);
        Log(LOG_ERROR, TEXT("Out of memory"));
        return E_OUTOFMEMORY;
    }

    pMonitor->pData = hLCD;
    pMonitor->nType = LEGACY_BRIGHT_NESS_TEST;
    InsertTailListM(&Monitors, pMonitor);

    return hr;
}


HRESULT CLegacyBrightnessTest::GetCurrentBrightness(DWORD dwMonitorIndex, BYTE* pbLevel)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pEntry = NULL;
    IWbemClassObject* pMonitor = NULL;
    
    if(pbLevel == NULL)
        return E_INVALIDARG;

    //Retrieve the monitor from the list of monitors
    if(IsListEmptyM(&Monitors))
    {
        return E_FAIL;
    }
    hr = GetHeadListItemM(&Monitors, dwMonitorIndex, &pEntry);
    if(FAILED(hr))
        return hr;
    
    return GetCurrentBrightness(((HANDLE)pEntry->pData), pbLevel);

}

HRESULT CLegacyBrightnessTest::GetCurrentBrightness(HANDLE hMonitor, BYTE* pbLevel)
{
    HRESULT hr = S_OK;
    DISPLAY_BRIGHTNESS db;
    DWORD dwBuffer = 0;
    BOOL bRet;

    if((hMonitor == NULL) || (pbLevel == NULL))
        return E_INVALIDARG;

    bRet = DeviceIoControl((HANDLE) hMonitor,                // handle to device
                    IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS,  // dwIoControlCode
                    NULL,                    // lpInBuffer
                    0,                               // nInBufferSize
                    (LPVOID) &db,            // output buffer
                    sizeof(DISPLAY_BRIGHTNESS),          // size of output buffer
                    (LPDWORD) &dwBuffer,       // number of bytes returned
                    (LPOVERLAPPED)NULL      // OVERLAPPED structure
                    );
    if(0 == bRet)
    {
        Log(LOG_ERROR, TEXT("IOCTL_VIDEO_QUERY_DISPLAY_BRIGHTNESS failed. Error = %d"), GetLastError());
        return E_FAIL;
    }
    else
    {
        *pbLevel = db.ucDCBrightness;	
        return S_OK;
    }
}

HRESULT CLegacyBrightnessTest::GetSupportedLevels(DWORD dwMonitorIndex, __out_ecount(cSize)LPBYTE pabLevels, size_t cSize, LONG* plLevels)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pEntry = NULL;
    IWbemClassObject* pMonitor = NULL;
    
    if(plLevels == NULL)
        return E_INVALIDARG;
    
    //Retrieve the monitor from the list of monitors
    if(IsListEmptyM(&Monitors))
    {
        return E_FAIL;
    }
    hr = GetHeadListItemM(&Monitors, dwMonitorIndex, &pEntry);
    if(FAILED(hr))
        return hr;
    
    return GetSupportedLevels(((HANDLE)pEntry->pData), pabLevels, cSize, plLevels);	
}

HRESULT CLegacyBrightnessTest::GetSupportedLevels(HANDLE hMonitor, __out_ecount(cSize)LPBYTE pabLevels, size_t cSize, LONG* plLevels)
{
    HRESULT hr = S_OK;
    BOOL bRet;

    if((hMonitor == NULL) || (plLevels == NULL))
        return E_INVALIDARG;
    //Clear the input array
    ZeroMemory(pabLevels, sizeof(BYTE)*cSize);

    bRet = DeviceIoControl(hMonitor,                // handle to device
                            IOCTL_VIDEO_QUERY_SUPPORTED_BRIGHTNESS,  // dwIoControlCode
                            NULL,                            // lpInBuffer
                            0,                               // nInBufferSize
                            (LPVOID) pabLevels,            // output buffer
                            cSize,          // size of output buffer
                            (LPDWORD) plLevels,       // number of bytes returned
                            (LPOVERLAPPED)NULL      // OVERLAPPED structure
                            );

    if(0 == bRet)
    {
        Log(LOG_ERROR, TEXT("IOCTL_VIDEO_QUERY_SUPPORTED_BRIGHTNESS failed. Error = %d"), GetLastError());
        return E_FAIL;
    }
    if(0 == *plLevels)
    {
        Log(LOG_ERROR, TEXT("0 brightness levels returned by IOCTL_VIDEO_QUERY_SUPPORTED_BRIGHTNESS"));
        return E_FAIL;
    }
    else
    {
        return S_OK;
    }
}

HRESULT CLegacyBrightnessTest::SetLevel(DWORD dwMonitorIndex, BYTE bLevel)
{
    HRESULT hr = S_OK;
    PMONITOR_LIST_ENTRY pEntry = NULL;
    IWbemClassObject* pMonitor = NULL;
    
    //Retrieve the monitor from the list of monitors
    if(IsListEmptyM(&Monitors))
    {
        return E_FAIL;
    }
    hr = GetHeadListItemM(&Monitors, dwMonitorIndex, &pEntry);
    if(FAILED(hr))
        return hr;
    
    return SetLevel(((HANDLE)pEntry->pData), bLevel);

}

HRESULT CLegacyBrightnessTest::SetLevel(HANDLE hMonitor, BYTE bLevel)
{
    DISPLAY_BRIGHTNESS db;
    BOOL bRet;
    DWORD dwBytesReturned = 0;

    if(hMonitor == NULL)
        return E_INVALIDARG;

    db.ucACBrightness = bLevel;
    db.ucDCBrightness = bLevel;
    db.ucDisplayPolicy = DISPLAYPOLICY_BOTH;
    
    bRet = DeviceIoControl((HANDLE) hMonitor,               // handle to device
                            IOCTL_VIDEO_SET_DISPLAY_BRIGHTNESS,  // dwIoControlCode
                            &db,                    // lpInBuffer
                            sizeof(DISPLAY_BRIGHTNESS),                               // nInBufferSize
                            (LPVOID) NULL,            // output buffer
                            0,          // size of output buffer
                            (LPDWORD) &dwBytesReturned,       // number of bytes returned
                            (LPOVERLAPPED)NULL      // OVERLAPPED structure
                            );
    if(0 == bRet)
    {
        Log(LOG_ERROR, TEXT("Failed to set the brightness level. Level = %d. Error = %d"),
            GetLastError());
        return E_FAIL;
    }
    else
    {
        return S_OK;
    }
}
