// brightness.cpp : Defines the entry point for the console application.
//

//#include "stdafx.h"


#include "brightnesspch.h"
#include "initguid.h"
#include "WMIHelper.h"

DEFINE_GUID( GUID_DISPLAY_BRIGHTNESS, 0xADED5E82, 0xB909, 0x4619, 0x99, 0x49, 0xF5, 0xD7, 0x1D, 0xAC, 0x0B, 0xCB );
#define OFFSET_TO_PTR(Base, Offset) ((PBYTE)((PBYTE)Base + Offset))

LPTSTR szTestCasePrefix[3] = {TEXT("Wmi WBEM"), TEXT("CAPI"), TEXT("Legacy")};

CVidWttLog g_Log;
BOOL g_bSkipSetBrightness = FALSE;

HRESULT RunTest(CBrightnessTest *pTest);
HRESULT CompareLevels(CWMIBrightnessTest* pWmiTest, CLegacyBrightnessTest* pLegacyTest);

HRESULT Log(DWORD dwLevel, LPCTSTR szFormat, ... )
{
    USES_CONVERSION;
    
    HRESULT hr = S_OK;
    TCHAR *szLogLine = NULL;
    LPTSTR pszDataBaseName = NULL;
    LPTSTR pszBugID = NULL;

    szLogLine = new TCHAR[MAX_LOGLINE];
    if(szLogLine == NULL)
        return E_OUTOFMEMORY;

    ZeroMemory(szLogLine, MAX_LOGLINE);

    va_list va;

    va_start(va, szFormat);
    StringCchVPrintf(szLogLine, MAX_LOGLINE-1, szFormat, va);
    va_end(va);

    __try
    {
        switch(dwLevel)
        {
        case LOG_START_TEST:
            hr = g_Log.StartTest(T2W(szLogLine));
            break;
        case LOG_END_TEST:
            hr = g_Log.EndTest(T2W(szLogLine));
            break;
        case LOG_SKIP_TEST:
            hr = g_Log.SkipTest(T2W(szLogLine));
            break;
        case LOG_MSG:
            hr = g_Log.LogMsg(T2W(szLogLine));
            break;
        case LOG_WARNING:
            hr = g_Log.LogWarning(T2W(szLogLine));
            break;
        case LOG_ERROR:
            hr = g_Log.LogError(T2W(szLogLine));
            break;
        case LOG_KNOWN_BUG:
            //szFormat is of the format <Database Name:Bug ID>
            pszDataBaseName = _tcstok(szLogLine, TEXT(":"));
            pszBugID = _tcstok(NULL, TEXT("\0"));
            
            hr = g_Log.KnownBug(_tcstol(pszBugID, NULL, 10), T2W(pszDataBaseName));
            break;
        }
    }
    __except(GetExceptionCode() == STATUS_STACK_OVERFLOW)
    {
        _resetstkoflw();
        return E_FAIL;
    }

    if(szLogLine)
    {
        delete[] szLogLine;
    }

    return hr;
}

/*
HRESULT SkipTest(LPCTSTR szFormat, ... )
{
    USES_CONVERSION;
    
    LPTSTR szFullTestCaseName = new TCHAR[MAX_LOGLINE];
    if(szFullTestCaseName == NULL)
    {
        return E_OUTOFMEMORY;
    }

    ZeroMemory(szFullTestCaseName, MAX_LOGLINE);

    va_list va;

    va_start(va, szFormat);
    StringCchVPrintf(szFullTestCaseName, MAX_LOGLINE-1, szFormat, va);
    va_end(va);
    
    __try
    {
        g_Log.SkipTest(T2W(szFullTestCaseName));
    }
    __except(GetExceptionCode() == STATUS_STACK_OVERFLOW)
    {
        _resetstkoflw();
        printf("Exception in T2W conversion. The test will not log a skip correctly!\n");
    }
    delete[] szFullTestCaseName;
    
    return S_OK;
}
*/

HRESULT DisplayProperty(IWbemClassObject *pObj, LPCWSTR wszProp)
{
    HRESULT hr = S_OK;
    BSTR strProp = NULL;
    VARIANT vProp;
    
    strProp = SysAllocString(wszProp);
    VariantInit(&vProp);
    if(strProp == NULL)
    {
        hr = E_OUTOFMEMORY;
    }
    else
    {
        hr = pObj->Get(strProp, 0, &vProp, 0, 0);
        SysFreeString(strProp);
        if(SUCCEEDED(hr))
        {
            wprintf(L"%s: %s\n", wszProp, V_BSTR(&vProp));
        }
        else
        {
            Log(LOG_ERROR, TEXT("Failed to get the property %S. Error = %08x"), wszProp, hr);
        }
    }

    VariantClear(&vProp);
    return hr;
}

HRESULT IsBrightnessSupported()
{
    HRESULT hr = S_OK;
    
    //Report the make and model of the machine
    CWMIHelper WMIHelper;
    CWMIBrightnessTest WMITest;
    IWbemServices* pWMISvc = NULL;
    IWbemClassObject *pObj = NULL;
    IEnumWbemClassObject *pObjs = NULL;
    BSTR Query = NULL;
    BSTR Lang = NULL;
    BSTR strProp = NULL;
    ULONG ulReturned = 0;
    VARIANT vProp;

    hr = WMIHelper.Init(TEXT("root\\CIMV2"), NULL);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, TEXT("Failed to initialize Wmi"));
        return hr;
    }
    
    pWMISvc = WMIHelper.GetWMIServices();

    //Win32_ComputerSystem.Manufacturer .Model .Name
    Query = SysAllocString(L"SELECT * from Win32_ComputerSystem");
    if(Query == NULL)
    {
        Log(LOG_ERROR, TEXT("Out of Memory"));
        return E_OUTOFMEMORY;
    }
    Lang = SysAllocString(L"WQL");
    if(Lang == NULL)
    {
        Log(LOG_ERROR, TEXT("Out of Memory"));
        hr = E_OUTOFMEMORY;
        goto Done;
    }

    hr = pWMISvc->ExecQuery(Lang, Query, WBEM_FLAG_FORWARD_ONLY, NULL, &pObjs);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, TEXT("Failed to execute the query. Error = 0x%08x"), hr);
    
        goto Done;
    }
    hr = pObjs->Next(WBEM_INFINITE, 1, &pObj, &ulReturned);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, TEXT("IEnumWbemClassObject::Next failed. Error = 0x%08x"), hr);
        goto Done;
    }
    else if(hr == S_FALSE)
    {
        Log(LOG_WARNING, TEXT("No instance of Win32_ComputerSystem was found"));
        goto Done;
    }
        
    DisplayProperty(pObj, L"Manufacturer");

    DisplayProperty(pObj, L"Model");
    
    DisplayProperty(pObj, L"Name");

    //Query WMI to find out if a brightness control interface is present
    hr = WMITest.Init(NULL);

    if(S_FALSE == hr)
    {
        _tprintf(TEXT("Brightness controls not supported\n"));
    }
    else if(FAILED(hr))
    {
        _tprintf(TEXT("Initialization failed. Error = %08x"), hr);
    }
    else
    {
        _tprintf(TEXT("Brightness controls supported\n"));
        hr = S_OK;
    }

Done:
    if(pObjs)
    {
        pObjs->Release();
    }
    if(Lang)
    {
        SysFreeString(Lang);
    }
    if(Query)
    {
        SysFreeString(Query);
    }

    WMITest.Cleanup();

    return hr;
}


// Video settings (from ntpoapi.h)
// --------------
//
// Specifies the subgroup which will contain all of the video
// settings for a single policy.
//
DEFINE_GUID( GUID_VIDEO_SUBGROUP, 0x7516B95F, 0xF776, 0x4464, 0x8C, 0x53, 0x06, 0x16, 0x7F, 0x40, 0xCC, 0x99 );
//
// Specifies (in seconds) how long we wait after the last user input has been
// recieved before we dim the video.
//
DEFINE_GUID( GUID_VIDEO_DIM_TIMEOUT, 0x17aaa29b, 0x8b43, 0x4b94, 0xaa, 0xfe, 0x35, 0xf6, 0x4d, 0xaa, 0xf1, 0xee);




int __cdecl _tmain(int argc, _TCHAR* argv[])
{
    HRESULT hr = S_OK;
    LPTSTR pszCmdLine = NULL;
    LPTSTR pszSwitch = NULL;
    BOOL bLegacySupported = true;
    TCHAR szTestCase[MAX_PATH];
    DWORD dwCount = 0;
    CoInitialize(NULL);

    DWORD dwAcDimTimeSec = 0;
    DWORD dwDcDimTimeSec = 0;
    DWORD dwDimTimeSec = 3600;
    bool bManageDimTimeout = false;
    LPGUID pguidActiveScheme = nullptr;

    //Initialize the log object
    hr = g_Log.Initialize(L"VidWttLogger.xsl");
    if(FAILED(hr))
    {
        printf("Failed to initialize log\n");
    }
    
    //Log the machine information
    hr = g_Log.LogMachineInfo();
    if(FAILED(hr))
    {
        printf("Failed to log machine info\n");
    }

    //	On Win7 the test shouldn't change a dim timeout, as a dimmed state doesn't affect SetBrightness functionality.
    //  On Win8 and later the test should make sure, that a monitor didn't go in a dimmed state during the test, otherwize SetBrighntess
    //	functionaly would be affected, as OS would reduce a maximum allowed brightness to a value specified for a dimmed level.
    OSVERSIONINFO OSVersionInfo = {0};
    OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (GetVersionEx(&OSVersionInfo) == FALSE)
    {
        StringCchPrintf(szTestCase, ARRAYSIZE(szTestCase), TEXT("Can't get OS version. Dim timeout won't be changed"));
        Log(LOG_WARNING, szTestCase);
    }
    else
    {
        bManageDimTimeout = OSVersionInfo.dwBuildNumber  >= 8000;
    }

    //check for the /support switch
    pszCmdLine = GetCommandLine();
    _tcsupr(pszCmdLine);
    
    pszSwitch = _tcsstr(pszCmdLine, TEXT("/SUPPORT"));

    StringCchPrintf(szTestCase, ARRAYSIZE(szTestCase), TEXT("Is Brightness supported"));
    Log(LOG_START_TEST, szTestCase);
    HRESULT  hrSupport = IsBrightnessSupported();
    if(S_OK == hrSupport)
    {
        Log(LOG_MSG, TEXT("%s: Brightness supported"), szTestCase);
    }
    else
    {
        Log(LOG_MSG, TEXT("%s: Brightness not supported"), szTestCase);
    }
    Log(LOG_END_TEST, szTestCase);


    if(S_OK == hrSupport)
    {
        CBrightnessTest* pTestObjects[3];
        CWMIBrightnessTest wmiBrightness;
        BYTE bInitial = 0;
        StringCchPrintf(szTestCase, ARRAYSIZE(szTestCase), TEXT("Get Initial Brightness"));
        Log(LOG_START_TEST, szTestCase);
        
        for(UINT Index =0; Index < 3; Index++)
        {
            pTestObjects[Index] = NULL;
        }

        //Get the initial brightness level
        hr = wmiBrightness.Init(&g_Log);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get current brightness"));
        }
        dwCount = 0;
        hr = wmiBrightness.GetCurrentBrightness(dwCount, &bInitial);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to get current brightness"));
        }
        Log(LOG_END_TEST, szTestCase);
        
        pTestObjects[0] = new CWMIBrightnessTest();
        if(!pTestObjects[0])
        {
            printf("Failed to create a CWMIBrightnessTest object. Out of memory\n");
            return 1;
        }

        pTestObjects[1] = new CWMIBrightnessCAPITest();
        if(!pTestObjects[1])
        {
            printf("Failed to create a CWMIBrightnessCAPITest object. Out of memory\n");
            return 1;
        }

        pTestObjects[2] = new CLegacyBrightnessTest();
        if(!pTestObjects[2])
        {
            printf("Failed to create a CLegacyBrightnessTest object. Out of memory\n");
            return 1;
        }

        //----- Modify Dim timeout values, if needed -------

        //  Save currect Dim timeout values
        if (bManageDimTimeout)
        {
            DWORD dwResult = PowerGetActiveScheme(NULL, &pguidActiveScheme);
            if ((dwResult != ERROR_SUCCESS) || (pguidActiveScheme == nullptr))
            {
                bManageDimTimeout = false;
                StringCchPrintf(szTestCase, ARRAYSIZE(szTestCase), TEXT("Can't get active power scheme. Dim timeout won't be changed"));
                Log(LOG_WARNING, szTestCase);
            }
            else
            {
            
                DWORD dwResultDC = PowerReadDCValueIndex(NULL,
                                                 pguidActiveScheme,
                                                 &GUID_VIDEO_SUBGROUP,
                                                 &GUID_VIDEO_DIM_TIMEOUT,
                                                 &dwDcDimTimeSec);
                DWORD dwResultAC = PowerReadACValueIndex(NULL,
                                                 pguidActiveScheme,
                                                 &GUID_VIDEO_SUBGROUP,
                                                 &GUID_VIDEO_DIM_TIMEOUT,
                                                 &dwAcDimTimeSec);
                if (dwResultDC != ERROR_SUCCESS ||
                    dwResultAC != ERROR_SUCCESS)
                {
                     bManageDimTimeout = false;
                     StringCchPrintf(szTestCase, ARRAYSIZE(szTestCase), TEXT("Can't read Dim timeout values. Dim timeout won't be changed"));
                     Log(LOG_WARNING, szTestCase);
                     LocalFree(pguidActiveScheme);
                     pguidActiveScheme = nullptr;
                }
            }
        }
    
        //  Set desired Dim timeout values
        if (bManageDimTimeout)
        {
            // Undimm the monitor by emulating a key press (space).
            INPUT input[1] = {0};  
            input[0].type = 1;  
            input[0].ki.wVk = VK_SPACE;
            // Send key down.      
            input[0].ki.dwFlags = 0; //KEYEVENTF_KEYDOWN;  
            SendInput(1, input, sizeof(INPUT));  
            // Send key up.      
            input[0].ki.dwFlags = KEYEVENTF_KEYUP;  
            SendInput(1, input, sizeof(INPUT));  
            Sleep(3000);

            DWORD dwResultDC = PowerWriteDCValueIndex(NULL,
                                                    pguidActiveScheme,
                                                    &GUID_VIDEO_SUBGROUP,
                                                    &GUID_VIDEO_DIM_TIMEOUT,
                                                    dwDimTimeSec);
            DWORD dwResultAC = PowerWriteACValueIndex(NULL,
                                                    pguidActiveScheme,
                                                    &GUID_VIDEO_SUBGROUP,
                                                    &GUID_VIDEO_DIM_TIMEOUT,
                                                    dwDimTimeSec);
            if (dwResultDC != ERROR_SUCCESS ||
                dwResultAC != ERROR_SUCCESS)
            {
                bManageDimTimeout = false;
                StringCchPrintf(szTestCase, ARRAYSIZE(szTestCase), TEXT("Can't write Dim timeout values. Dim timeout won't be changed"));
                Log(LOG_WARNING, szTestCase);
                LocalFree(pguidActiveScheme);
                pguidActiveScheme = nullptr;
            }
            else
            {
                // set changed policy
                DWORD dwResult = PowerSetActiveScheme(NULL,pguidActiveScheme);
                if (dwResultDC != ERROR_SUCCESS)
                {
                    bManageDimTimeout = false;
                    StringCchPrintf(szTestCase, ARRAYSIZE(szTestCase), TEXT("Can't set active power scheme. Dim timeout won't be changed"));
                    Log(LOG_WARNING, szTestCase);
                    LocalFree(pguidActiveScheme);
                    pguidActiveScheme = nullptr;
                }
            }
    	}
        //-------------------------------------------------------

        for(int nIndex = 0 ; nIndex < 3; nIndex++)
        {
            StringCchPrintf(szTestCase, ARRAYSIZE(szTestCase), TEXT("%s: Brightness support"), szTestCasePrefix[nIndex]);
            //Run the test
            Log(LOG_START_TEST, szTestCase);
            hr = pTestObjects[nIndex]->Init(&g_Log);
        
            if(S_FALSE == hr)
            {
                Log(LOG_MSG, TEXT("%s: Brightness controls not reported"), szTestCasePrefix[nIndex]);
                Log(LOG_SKIP_TEST, TEXT("%s"), szTestCase);
            }
            else if(FAILED(hr))
            {
                Log(LOG_ERROR, TEXT("%s: Failed to initialize the test. hr = %08x"), szTestCasePrefix[nIndex], hr);
            }
            else if(S_OK == hr)
            {
                pTestObjects[nIndex]->SetSupport(TRUE);
                Log(LOG_MSG, TEXT("%s: Brightness controls reported"), szTestCasePrefix[nIndex]);
            }
            Log(LOG_END_TEST, szTestCase);

            if(S_OK == hr)
            {
                RunTest(pTestObjects[nIndex]);
            }
        }

/*
        if(pTestObjects[0]->GetSupport() && pTestObjects[2]->GetSupport())
        {
            g_Log.StartTest(L"Compare the brightness levels reported by WMI and legacy IOCTLs");
            CompareLevels((CWMIBrightnessTest*)pTestObjects[0], (CLegacyBrightnessTest*)pTestObjects[2]);
            g_Log.EndTest(L"Compare the brightness levels reported by WMI and legacy IOCTLs");
        }
        else
        {
            Log(LOG_MSG, TEXT("Either Wmi or legacy brightness controls were not reported. Skipping the comparison test"));
            Log(LOG_SKIP_TEST, TEXT("%s"), TEXT("Compare the brightness levels reported by WMI and legacy IOCTLs"));
        }
*/
        g_Log.StartTest(L"Wmi C API Brightness Event test");

        pTestObjects[1]->GetMonitorCount(&dwCount);
        if(0 == dwCount)
        {
            Log(LOG_MSG, TEXT("No monitors reported brightness"));
            Log(LOG_SKIP_TEST, TEXT("%s"), TEXT("Wmi C API Brightness Event test"));
        }
        for(DWORD i = 0 ; i < dwCount ; i++)
        {
            hr = ((CWMIBrightnessCAPITest*)pTestObjects[1])->TestBrightnessEvent(i);
            if(FAILED(hr))
            {
                Log(LOG_ERROR, TEXT("C API brightness Event test failed. hr = %08x"), hr);
            }
        }
        g_Log.EndTest(L"Wmi C API Brightness Event test");

        StringCchPrintf(szTestCase, ARRAYSIZE(szTestCase), TEXT("Revert to original brightness level"));
        Log(LOG_START_TEST, szTestCase);
        dwCount = 0;
        hr = wmiBrightness.SetLevel(dwCount, bInitial);
        if(FAILED(hr))
        {
            Log(LOG_ERROR, TEXT("Failed to revert to original brightness level = %d"), bInitial);	
        }
        Log(LOG_END_TEST, szTestCase);
        for(UINT Index =0; Index < 3; Index++)
        {
            if(pTestObjects[Index])
            {
                delete pTestObjects[Index];
            }
        }

        wmiBrightness.Cleanup();

        // Restore Dim timeout values, if needed
        if (bManageDimTimeout)
        {
            DWORD dwResultDC = PowerWriteDCValueIndex(NULL,
                                                    pguidActiveScheme,
                                                    &GUID_VIDEO_SUBGROUP,
                                                    &GUID_VIDEO_DIM_TIMEOUT,
                                                    dwDcDimTimeSec);
            DWORD dwResultAC = PowerWriteACValueIndex(NULL,
                                                    pguidActiveScheme,
                                                    &GUID_VIDEO_SUBGROUP,
                                                    &GUID_VIDEO_DIM_TIMEOUT,
                                                    dwAcDimTimeSec);
            if (dwResultDC != ERROR_SUCCESS ||
                dwResultAC != ERROR_SUCCESS)
            {
                StringCchPrintf(szTestCase, ARRAYSIZE(szTestCase), TEXT("Can't write Dim timeout values. Dim timeout won't be restored"));
                Log(LOG_WARNING, szTestCase);
            }
            else
            {
                // set changed policy
                DWORD dwResult = PowerSetActiveScheme(NULL,pguidActiveScheme);
                if (dwResultDC != ERROR_SUCCESS)
                {
                    StringCchPrintf(szTestCase, ARRAYSIZE(szTestCase), TEXT("Can't set active power scheme. Dim timeout won't be restored"));
                    Log(LOG_WARNING, szTestCase);
                }
            }
            LocalFree(pguidActiveScheme);
            pguidActiveScheme = nullptr;
        }
    }

    g_Log.Close();



    CoUninitialize();
    return 0;
}

HRESULT VerifyLevel(CBrightnessTest* pTest, DWORD dwMonitor,  BYTE bExpectedLevel)
{
    Sleep(500);
    BYTE bCurrentLevel = 0;
    HRESULT hr = pTest->GetCurrentBrightness(dwMonitor, &bCurrentLevel);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, TEXT("Failed to get current brightness level"));
        return hr;
    }
    else
    {
        if(bExpectedLevel != bCurrentLevel)
        {
            Log(LOG_ERROR, TEXT("Unexpected current level. Expecting %d. Current Level %d"), bExpectedLevel, bCurrentLevel);
            return E_FAIL;
        }
    }
    return hr;
}

HRESULT SetAndVerifyLevel(CBrightnessTest* pTest, DWORD dwMonitor, BYTE bSetLevel, BYTE bExpectedLevel)
{
    BYTE bCurrentLevel = 0;
    HRESULT hr = pTest->SetLevel(dwMonitor, bSetLevel);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, TEXT("Failed to set brightness level = %d"), bSetLevel);
    }
    else
    {
        hr = VerifyLevel(pTest, dwMonitor, bExpectedLevel);
    }
    return hr;
}

HRESULT SetALSAndVerifyLevel(CBrightnessTest* pTest, DWORD dwMonitor, BYTE bSetLevel, BYTE bExpectedLevel)
{
    BYTE bCurrentLevel = 0;
    HRESULT hr = pTest->SetALSLevel(dwMonitor, bSetLevel);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, TEXT("Failed to set brightness level = %d"), bSetLevel);
    }
    else
    {
        hr = VerifyLevel(pTest, dwMonitor, bExpectedLevel);
    }
    return hr;
}


HRESULT IsACPower(BOOL& bACPower)
{
    SYSTEM_POWER_STATUS powerStatus;
    
    if(!GetSystemPowerStatus(&powerStatus))
    {
        return E_FAIL;
    }

    if(255 == powerStatus.ACLineStatus)
    {
        return E_FAIL;
    }

    bACPower = powerStatus.ACLineStatus;

    return S_OK;

}

HRESULT RunTest(CBrightnessTest* pTest)
{
    DWORD dwCount = 0;
    BYTE bLevel = 0;
    BYTE bMin = 0;
    BYTE bMax = 0;
    BYTE bLower = 0;
    BYTE bUpper = 0;
;
    LONG lLevels = 0;
    BYTE abLevels[256];
    HRESULT hr = S_OK;
    LONG x, y;

    pTest->GetMonitorCount(&dwCount);
    if(0 == dwCount)
    {
        Log(LOG_ERROR, TEXT("No monitors reported brightness"));
        return E_FAIL;
    }
    for(DWORD i = 0 ; i < dwCount ; i++)
    {

        Log(LOG_START_TEST, TEXT("[Monitor %d] %s GetSupportedLevels"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
        hr = pTest->GetSupportedLevels(i, abLevels, ARRAYSIZE(abLevels), &lLevels);
        if(FAILED(hr))
        {
            Log(LOG_END_TEST, TEXT("[Monitor %d] %s GetSupportedLevels"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
            return hr;
        }
        Log(LOG_END_TEST, TEXT("[Monitor %d] %s GetSupportedLevels"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);

        //Verify that all the levels are sorted in an ascending order

        if(pTest->GetType() != TT_LEGACY)
        {
            Log(LOG_START_TEST, TEXT("[Monitor %d] %s Brightness levels sorted in an ascending order"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);

            for(x = 0; x <= lLevels-2 ; x++)
            {
                if(abLevels[x] > abLevels[x+1])
                {
                    Log(LOG_ERROR, TEXT("Out of order brightness levels found. %d appears before %d"), abLevels[x], abLevels[x+1]);
                }
            }
            Log(LOG_END_TEST, TEXT("[Monitor %d] %s Brightness levels sorted in an ascending order"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);


            //Verify that all the levels are unique
            Log(LOG_START_TEST, TEXT("[Monitor %d] %s Brightness levels are unique"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
            for(x = 0; x <= lLevels-2 ; x++)
            {
                for(y = x+1; y<= lLevels - 1; y++)
                {
                    if(abLevels[x] == abLevels[y])
                    {
                        Log(LOG_ERROR, TEXT("Duplicate level found at indices %d and %d"), x, y);
                    }
                }
            }
            Log(LOG_END_TEST, TEXT("[Monitor %d] %s Brightness levels are unique"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
        }

        Log(LOG_START_TEST, TEXT("[Monitor %d] %s SetBrightnessLevels"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
        for(int j = 0; j < lLevels; j++)
        {
            Log(LOG_MSG, TEXT("Levels[%d] = %d"), j, abLevels[j]);
            SetAndVerifyLevel(pTest, i, abLevels[j], abLevels[j]);
        }
        
        Log(LOG_END_TEST, TEXT("[Monitor %d] %s SetBrightnessLevels"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);

        if(pTest->GetType() != TT_LEGACY)
        {
            //Verify that intermediate values b/w reported brightness levels can be set
            //The values are rounded to the closest supported value. If two values are equally close, then the lower one is taken
            if(lLevels > 0)
            {
                bMin = abLevels[0];
                bMax = abLevels[lLevels-1];
            }
            Log(LOG_START_TEST, TEXT("[Monitor %d] %s Intermediate brightness levels. Lowest level minus one"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
            if(lLevels > 0 && (bMin > 0))
            {
                SetAndVerifyLevel(pTest, i, bMin-1, bMin);
            }
            else
            {
                Log(LOG_SKIP_TEST, TEXT("[Monitor %d] %s Intermediate brightness levels. Lowest level minus one"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
            }
            Log(LOG_END_TEST, TEXT("[Monitor %d] %s Intermediate brightness levels. Lowest level minus one"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);

            Log(LOG_START_TEST, TEXT("[Monitor %d] %s Intermediate brightness levels. Highest level plus one"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
            if(lLevels > 0)
            {
                SetAndVerifyLevel(pTest, i, bMax+1, bMax);
            }
            else
            {
                Log(LOG_SKIP_TEST, TEXT("[Monitor %d] %s Intermediate brightness levels. Highest level plus one"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
            }
            Log(LOG_END_TEST, TEXT("[Monitor %d] %s Intermediate brightness levels. Highest level plus one"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);

            //For two consecutive brightness levels, L being the lower one and U being the upper one:
            //if they are consecutive integers skip the following test cases
            //if they have a mid point, the set it and verify that L is the resultant brightness.
            //if they have a difference of two or more set L+1 and B-1. The former should result in L and the latter in B
            for(x = 0; x <= lLevels-2 ; x++)
            {
                bLower = abLevels[x];
                bUpper = abLevels[x+1];
                
                Log(LOG_START_TEST, TEXT("[Monitor %d] %s Set the middle value between two consecutive reported brightness levels. Lower = %d, Upper = %d"), dwCount, szTestCasePrefix[pTest->GetType() - 1], bLower, bUpper);
                if((bUpper == (bLower+1))  ||//Consecutive intergers
                    (((bUpper+bLower)%2) != 0)) //no mid point
                {
                    Log(LOG_SKIP_TEST, TEXT("[Monitor %d] %s Set the middle value between two consecutive reported brightness levels. Lower = %d, Upper = %d"), dwCount, szTestCasePrefix[pTest->GetType() - 1], bLower, bUpper);
                }
                else
                {
                    BYTE bMid = (bLower+bUpper)/2;
                    SetAndVerifyLevel(pTest, i, bMid, bUpper);
                }
                Log(LOG_END_TEST, TEXT("[Monitor %d] %s Set the middle value between two consecutive reported brightness levels. Lower = %d, Upper = %d"), dwCount, szTestCasePrefix[pTest->GetType() - 1], bLower, bUpper);

                Log(LOG_START_TEST, TEXT("[Monitor %d] %s Given two consecutive reported levels. try setting lower+1. Lower = %d, Upper = %d"), dwCount, szTestCasePrefix[pTest->GetType() - 1], bLower, bUpper);
                if((bUpper - bLower) > 2)
                {
                    //The difference b/w the levels is more than two integers. Try setting lower+1 and upper -1
                    SetAndVerifyLevel(pTest, i, bLower+1, bLower);
                }
                else
                {
                    Log(LOG_SKIP_TEST, TEXT("[Monitor %d] %s Given two consecutive reported levels. try setting lower+1. Lower = %d, Upper = %d"), dwCount, szTestCasePrefix[pTest->GetType() - 1], bLower, bUpper);
                }
                Log(LOG_END_TEST, TEXT("[Monitor %d] %s Given two consecutive reported levels. try setting lower+1. Lower = %d, Upper = %d"), dwCount, szTestCasePrefix[pTest->GetType() - 1], bLower, bUpper);

                Log(LOG_START_TEST, TEXT("[Monitor %d] %s Given two consecutive reported levels. try setting upper-1. Lower = %d, Upper = %d"), dwCount, szTestCasePrefix[pTest->GetType() - 1], bLower, bUpper);
                if((bUpper - bLower) > 2)
                {
                    SetAndVerifyLevel(pTest, i, bUpper-1, bUpper);
                }
                else
                {
                    Log(LOG_SKIP_TEST, TEXT("[Monitor %d] %s Given two consecutive reported levels. try setting upper-1. Lower = %d, Upper = %d"), dwCount, szTestCasePrefix[pTest->GetType() - 1], bLower, bUpper);
                }
                
                Log(LOG_END_TEST, TEXT("[Monitor %d] %s Given two consecutive reported levels. try setting upper-1. Lower = %d, Upper = %d"), dwCount, szTestCasePrefix[pTest->GetType() - 1], bLower, bUpper);
            }


            BOOL bOnAC;
            DWORD dwSize = 0;
            BYTE L1, L2;
            BOOL bFoundL1 = FALSE;
            BOOL bFoundL2 = FALSE;
            
            Log(LOG_START_TEST, TEXT("[Monitor %d] %s ALS Brightness"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);

            
            //Pick two distinct brightness levels
            for(LONG lBrightnessIndex = 0 ; lBrightnessIndex< lLevels; lBrightnessIndex++)
            {
                L1 = abLevels[0];
                bFoundL1 = TRUE;
                if(bFoundL1 && !bFoundL2 && (L1 != abLevels[lBrightnessIndex]))
                {
                    L2 = abLevels[lBrightnessIndex];
                    bFoundL2 = TRUE;
                }
                if(bFoundL1 && bFoundL2)
                {
                    //found two unique brightness levels different from the policy level
                    break;
                }
            }

            printf("L1 = %d, L2 = %d\n", L1, L2);


            if(!bFoundL1 || !bFoundL2)
            {
                //we were unable to find two unique brightness levels
                Log(LOG_ERROR, TEXT("Failed to find two unique brightness levels"));
                Log(LOG_END_TEST, TEXT("[Monitor %d] %s ALS Brightness"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
                return hr;

            }

            //Set ALS state to false
            hr = pTest->SetALSState(i, FALSE);
            if(FAILED(hr))
            {
                Log(LOG_ERROR, TEXT("Failed to set the ALS state to FALSE"));
                Log(LOG_END_TEST, TEXT("[Monitor %d] %s ALS Brightness"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
                return hr;
            }

            hr = SetAndVerifyLevel(pTest, i, L1, L1);
            if(FAILED(hr))
            {
                Log(LOG_END_TEST, TEXT("[Monitor %d] %s ALS Brightness"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
                return hr;
            }

            //Set ALS brightness to L2 -> Verify that brightness stays at L1
            hr = SetALSAndVerifyLevel(pTest, i, L2, L1);
            if(FAILED(hr))
            {
                Log(LOG_END_TEST, TEXT("[Monitor %d] %s ALS Brightness"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
                return hr;
            }
            
            //Set ALS state to true ->Verify brightness stays at L1
            hr = pTest->SetALSState(i, TRUE);
            if(FAILED(hr))
            {
                Log(LOG_ERROR, TEXT("Failed to set the ALS state to TRUE"));
                Log(LOG_END_TEST, TEXT("[Monitor %d] %s ALS Brightness"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
                return hr;
            }
            
            hr = VerifyLevel(pTest, i, L1);
            if(FAILED(hr))
            {
                Log(LOG_END_TEST, TEXT("[Monitor %d] %s ALS Brightness"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
                return hr;
            }

            //Set ALS brightness to L2 -> Verify that brightness changes to L2
            hr = SetALSAndVerifyLevel(pTest, i, L2, L2);
            if(FAILED(hr))
            {
                Log(LOG_END_TEST, TEXT("[Monitor %d] %s ALS Brightness"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
                return hr;
            }

            hr = pTest->SetALSState(i, FALSE);
            if(FAILED(hr))
            {
                Log(LOG_ERROR, TEXT("Failed to set the ALS state to FALSE"));
                Log(LOG_END_TEST, TEXT("[Monitor %d] %s ALS Brightness"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);
                return hr;
            }
            

            Log(LOG_END_TEST, TEXT("[Monitor %d] %s ALS Brightness"), dwCount, szTestCasePrefix[pTest->GetType() - 1]);


        }


    }
    

    return S_OK;
}


//This function retrives the brightness levels reported by the legacy IOCTLs and the Wmi brightness interface and compares to see if they are same
HRESULT CompareLevels(CWMIBrightnessTest* pWmiTest, CLegacyBrightnessTest* pLegacyTest)
{
    LONG lLegacyLevels = 0;
    LONG lWmiLevels = 0;
    BYTE abLegacyLevels[256];
    BYTE abWmiLevels[256];
    BOOL bWmiLevels = FALSE;
    DWORD dwCount = 0;

    pLegacyTest->GetMonitorCount(&dwCount);
    if(0 == dwCount)
    {
        Log(LOG_ERROR, TEXT("Legacy: No monitors reported brightness"));
        return E_FAIL;
    }

    HRESULT hr = pLegacyTest->GetSupportedLevels((DWORD)0, abLegacyLevels, ARRAYSIZE(abLegacyLevels), &lLegacyLevels);
    if(FAILED(hr))
    {
        Log(LOG_ERROR, TEXT("Legacy: Failed to get the supported levels"));
        return hr;
    }

    for(DWORD i = 0 ; i < dwCount ; i++)
    {
    hr = pWmiTest->GetSupportedLevels(i, abWmiLevels, ARRAYSIZE(abWmiLevels), &lWmiLevels);
        if(SUCCEEDED(hr))
        {
            bWmiLevels = TRUE;
            break;
        }
    }
    if(!bWmiLevels)
    {
        Log(LOG_ERROR, TEXT("Wmi: No monitors reported brightness"));
        return E_FAIL;
    }

    //Compare the number of brightness levels reported.
    if((lWmiLevels) != lLegacyLevels)
    {
        Log(LOG_ERROR, TEXT("The number of brighntess levels reported by Wmi and Legacy should match. Wmi = %d, Legacy = %d"), lWmiLevels, lLegacyLevels);
        return E_FAIL;
    }

    for(LONG j = 0 ; j < lLegacyLevels; j++)
    {
        if(abWmiLevels[j] != abLegacyLevels[j])
        {
            Log(LOG_ERROR, TEXT("Brighntess level index %d reported by Wmi and Legacy did not match. Wmi = %d, Legacy = %d"), 
                j, abWmiLevels[j], abLegacyLevels[j]);
            hr = E_FAIL;
        }
    }
    
    return hr;
}
