//*****************************************************************************
// Copyright (C) Microsoft Corporation, 2005.
//
// File:         MediaEngineTest.cpp
//
// History:
//  5/18/05      Created as part of the Codec Pack Automation BVT
//               suite.
//
//*****************************************************************************
//
#include "MFTDMediaEngineTest.h"
#include <CAutoLock.h>
#include <shellscalingapi.h>
#include <tlhelp32.h>
#include <dxcompare.h>
#include "FindHCKContentFile.h"
#include <lmcons.h>
#define SECURITY_WIN32
#include <security.h>

using namespace WEX::Common;
using namespace WEX::TestExecution;

#define MAX_STRING_LEN 2048
#define TOTAL_SLEEP_TIMEOUT 300000

PROPERTYKEY MFPKEY_MediaProtectionClassName =  { { 0x78e3a99c, 0x53db, 0x41b0, 0x93, 0x1f, 0x70, 0x94, 0x60, 0x16, 0x2a, 0x3}, 0x3 };   
PROPERTYKEY MFPKEY_MediaProtectionPackageName =  { { 0x4d19d6d7, 0xa14b, 0x4e4e, 0xb1, 0xab, 0x99, 0xb2, 0x62, 0x68, 0xef, 0xbd}, 0x3 };   

#define ONE_SECOND_IN_HUNDRED_NANOSECONDS 10000000

DWORD WINAPI FrameStepThread(LPVOID lpParameter);

#define NUM_OF_INSTANCES_SWITCH _T("-numMultipleInstances")
HRESULT ProcessConfigStringLocal(TCHAR* pszString, DWORD* pdwNumInstances, TCHAR*** pppszConfigStrings, CSmartLogger logger);
HRESULT SafeAllocateConfigStringsLocal(DWORD dwNumInstances, TCHAR*** pppszConfigStrings);
void    SafeFreeConfigStringsLocal(DWORD* pdwNumInstances, TCHAR*** pppszConfigStrings);

#ifndef BUILD_MINWIN
HRESULT MediaEngineTestEntryPoint(LPCTSTR pszInput, LPCTSTR pszOutput, LPCTSTR pszParams, LPCTSTR pszTestType,
    DWORD dwTestID, LPCTSTR pszTestTitle, LPCTSTR pszTestDLLName, LPCTSTR pszKID, LPCTSTR pszContentKey, LogType eLogType)
{

    HRESULT hr = S_OK;
    DWORD               dwNumTests = 0;
    CMediaEngineTest**   meTests = NULL;
    CTestHandler*       pTestHandler = NULL;
    TCHAR**             ppConfigStrings = NULL;
    CSmartLogger        logger;
    MPOCapablities      isMPOCapable = notMPOCapable;
    BOOL                bFullScreenMPO = TRUE;
    DWORD               dwLeft = 0;
    DWORD               dwTop = 0;
    DWORD               dwWidth = 0;
    DWORD               dwHeight = 0;
    // Find out how many overlay planes does the system support (N)
    DXGI_MULTIPLANE_OVERLAY_CAPS overlayCaps;

    do
    {
        logger.SetLogType(eLogType);

        if (_tcsicmp(pszTestType, _T("MPO")) == 0)
        {
            hr = GetOverlayCaps(&overlayCaps, &dwLeft, &dwTop, &dwWidth, &dwHeight, logger);
            if (FAILED(hr))
            {
                logger.Log(_T("ERROR>> GetOverlayCaps failed (hr=0x%x)"), hr);
                break;
            }
            else
            {
                logger.Log(_T("INFO>> Number of Overlay planes supported by the system are %d"), overlayCaps.MaxPlanes);
                logger.Log(_T("INFO>> Number of RGB overlay planes supported by the system are %d"), overlayCaps.Overlay.MaxRGBPlanes);
                logger.Log(_T("INFO>> Number of YUV overlay planes supported by the system are %d"), overlayCaps.Overlay.MaxYUVPlanes);

                UINT caps = overlayCaps.Overlay.OverlayCaps;
                if (overlayCaps.MaxPlanes <= 1)
                {
                    isMPOCapable = notMPOCapable;
                }
                else if (overlayCaps.Overlay.MaxYUVPlanes > 1)
                {
                    isMPOCapable = yuvMPOCapable;
                }
                else if (overlayCaps.Overlay.MaxRGBPlanes > 1)
                {
                    isMPOCapable = rgbMPOCapable;
                }
                else
                {
                    isMPOCapable = notMPOCapable;
                }
            }
        }

        hr = ProcessConfigString((TCHAR*)pszParams, &dwNumTests, &ppConfigStrings, eLogType);
        if(FAILED(hr))
        {
            logger.Log(_T("ERROR>> Failed to process config string (hr=0x%x)"), hr);
            break;
        }
        else if(hr == S_FALSE)
        {
            // -numMultipleInstances was not specified, this is a single instance test
            dwNumTests = 1;

            hr = SafeAllocateConfigStrings(1, &ppConfigStrings);
            if(FAILED(hr))
            {
                logger.Log(_T("ERROR>> Can't allocate memory for config string array"));
                break;
            }

            ppConfigStrings[0] = _tcsdup(pszParams);
            if(ppConfigStrings[0] == NULL)
            {
                hr = E_OUTOFMEMORY;
                logger.Log(_T("ERROR>> Can't allocate memory for config string array"));
                break;
            }
        }
        else if( isMPOCapable != notMPOCapable )
        {
            //if it gets here means -numMultipleInstances was specified

            bFullScreenMPO = FALSE;
            //TODO: remove later on support for more than 2 side by side windows
            //dwNumTests = min(overlayCaps.MaxYUVPlanes, dwNumTests);

        }

        meTests = new CMediaEngineTest*[dwNumTests];
        if(meTests == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        memset(meTests, NULL, sizeof(CMediaEngineTest*) * dwNumTests);

        for(DWORD i = 0; i < dwNumTests; i++)
        {
            meTests[i] = new CMediaEngineTest;
            if(meTests[i] == NULL)
            {
                hr = E_OUTOFMEMORY;
                logger.Log(_T("ERROR>> Out of memory, can't allocate Media Engine Test class"));
                break;
            }
        }

        pTestHandler = new CTestHandler;
        if(pTestHandler == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        pTestHandler->SetLogType(eLogType);

        //Call the methods of to initialize and execute a particular test case until done.

        CTestHandler::ITest** ppTests = NULL;

        do
        {
            dwWidth = dwWidth/dwNumTests;
            for(DWORD i = 0; i < dwNumTests; i++)
            {
                hr = meTests[i]->SetGenericTestSettings(pszInput, pszOutput, ppConfigStrings[i],
                    pszTestType, dwTestID, pszTestTitle, pszTestDLLName, eLogType);

                if (SUCCEEDED(hr) && _tcsicmp(pszTestType, _T("DRM")) == 0)
                {
                    hr = meTests[i]->SetDRMSettings(pszKID, pszContentKey);
                }
                else if (SUCCEEDED(hr) && _tcsicmp(pszTestType, _T("MPO")) == 0)
                {
                    if (isMPOCapable != notMPOCapable)
                    {
                        hr = meTests[i]->SetMPOSettings(overlayCaps, isMPOCapable, bFullScreenMPO, dwLeft, dwTop, dwWidth, dwHeight);
                        dwLeft += dwWidth;
                    }                    
                }

                if(FAILED(hr))
                {
                    logger.Log(_T("ERROR>> Failed to set test settings (hr=0x%x)"), hr);
                    break;
                }
            }

            if(FAILED(hr))
            {
                break;
            }

            ppTests = new CTestHandler::ITest*[dwNumTests];
            if(ppTests == NULL)
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            for (DWORD i = 0; i < dwNumTests; i++)
            {
                ppTests[i] = meTests[i];
            }

            hr = pTestHandler->AddTests(dwNumTests, ppTests);
            if(FAILED(hr))
            {
                logger.Log(_T("ERROR>> Failed to add test to test handler (hr=0x%x)"), hr);
                break;
            }

            hr = pTestHandler->ExecuteTests();
            
            if(FAILED(hr))
            {
                logger.Log(_T("ERROR>> Failed in execute tests (hr=0x%x)"), hr);
                break;
            }
        }while(FALSE);

        SAFEDELETE(ppTests);        

    }while(FALSE);

    if(meTests != NULL)
    {
        for(DWORD i = 0; i < dwNumTests; i++)
        {
            SAFEDELETES(meTests[i]);
        }

        SAFEDELETE(meTests);
    }

    SAFEDELETES(pTestHandler);

    SafeFreeConfigStrings(&dwNumTests, &ppConfigStrings);

    return hr;
}

#endif

HRESULT MediaEngineTestEntryPointMobile(LPCTSTR pszInput, LPCTSTR pszOutput, LPCTSTR pszParams, LPCTSTR pszTestType,
    DWORD dwTestID, LPCTSTR pszTestTitle, LPCTSTR pszTestDLLName, LPCTSTR pszKID, LPCTSTR pszContentKey, LogType eLogType)

{

    HRESULT hr = S_OK;

    DWORD               dwNumTests = 0;
    CMediaEngineTest*  meTests = NULL;
    TCHAR**             ppConfigStrings = NULL;
    DWORD               dwLeft = 0;
    DWORD               dwTop = 0;
    DWORD               dwWidth = 0;
    DWORD               dwHeight = 0;
    BOOL                fVisVal = FALSE;
    CSmartLogger        logger;
    MPOCapablities      isMPOCapable = notMPOCapable;
    BOOL                bFullScreenMPO = TRUE;
    // Find out how many overlay planes does the system support (N)
    DXGI_MULTIPLANE_OVERLAY_CAPS overlayCaps;

    DXCompareInfo comparisonInfo[2];

    double fFailPassRatio = 0.0;
    double fPSNRThreshold = 40.0;

    do
    {
        if (_tcsicmp(pszTestType, _T("MPO")) == 0)
        {
            hr = GetOverlayCaps(&overlayCaps, &dwLeft, &dwTop, &dwWidth, &dwHeight, logger);
            if (FAILED(hr))
            {
                logger.Log(_T("ERROR>> GetOverlayCaps failed (hr=0x%x)"), hr);
                break;
            }
            else
            {
                logger.Log(_T("INFO>> Number of Overlay planes supported by the system are %d"), overlayCaps.MaxPlanes);
                logger.Log(_T("INFO>> Number of RGB overlay planes supported by the system are %d"), overlayCaps.Overlay.MaxRGBPlanes);
                logger.Log(_T("INFO>> Number of YUV overlay planes supported by the system are %d"), overlayCaps.Overlay.MaxYUVPlanes);

                UINT caps = overlayCaps.Overlay.OverlayCaps;
                if (overlayCaps.MaxPlanes <= 1)
                {
                    isMPOCapable = notMPOCapable;
                }
                else if (overlayCaps.Overlay.MaxYUVPlanes > 1)
                {
                    isMPOCapable = yuvMPOCapable;
                }
                else if (overlayCaps.Overlay.MaxRGBPlanes > 1)
                {
                    isMPOCapable = rgbMPOCapable;
                }
                else
                {
                    isMPOCapable = notMPOCapable;
                }
            }
        }

        dwNumTests = 1;

        hr = SafeAllocateConfigStringsLocal(1, &ppConfigStrings);
        if (FAILED(hr))
        {
            logger.Log(_T("ERROR>> Can't allocate memory for config string array (hr=0x%x)"), hr);
            break;
        }
        ppConfigStrings[0] = _tcsdup(pszParams);

        if (ppConfigStrings == NULL)
        {
            hr = E_OUTOFMEMORY;
            logger.Log(_T("ERROR>> Can't allocate memory for config string array (hr=0x%x)"), hr);
            break;
        }

        meTests = new CMediaEngineTest;
        if (meTests == NULL)
        {
            hr = E_OUTOFMEMORY;
            logger.Log(_T("ERROR>> Out of memory, can't allocate Media Engine Test class (hr=0x%x)"), hr);
            break;
        }

        meTests->SetStrictTypeCheck(TRUE);

        hr = meTests->SetGenericTestSettings(pszInput, pszOutput, ppConfigStrings[0], pszTestType, dwTestID, pszTestTitle, pszTestDLLName, eLogType);
        if (FAILED(hr))
        {
            logger.Log(_T("ERROR>> Failed to set test settings (hr=0x%x)"), hr);
            break;
        }

        if (SUCCEEDED(hr) && _tcsicmp(pszTestType, _T("DRM")) == 0)
        {
            hr = meTests->SetDRMSettings(pszKID, pszContentKey);
        }
        else if (SUCCEEDED(hr) && _tcsicmp(pszTestType, _T("MPO")) == 0)
        {
            if (isMPOCapable != notMPOCapable)
            {
                hr = meTests->SetMPOSettings(overlayCaps, isMPOCapable, bFullScreenMPO, dwLeft, dwTop, dwWidth, dwHeight);
            }
        }

        if (FAILED(hr))
        {
            logger.Log(_T("ERROR>> Failed to set additional test settings (hr=0x%x)"), hr);
            break;
        }

        hr = meTests->CTBSetupTest();
        if (FAILED(hr))
        {
            break;
        }

        fVisVal = meTests->IsVisValTest();

        if (fVisVal)
        {
            //do not delete the input file after the first pass in case of VisVal tests. 
            //It is needed for the second pass.
            meTests->SetKeepfiles(true);

            meTests->SetDXVA(true);
        }        
        else
        {           
            meTests->SetDXVA(meTests->GetDXVA());
        }

        hr = meTests->CTBExecuteTest();
        if (FAILED(hr))
        {
            logger.Log(_T("ERROR>> Test Failed With Error (hr=0x%x)"), hr);
            break;
        }

        if (fVisVal)
        {
            hr = meTests->GetDXComparionInfo(comparisonInfo[0]);
            if (FAILED(hr))
            {
                logger.Log( _T("ERROR> GetDXComparionInfo Failed With Error (hr=0x%x)\n"), hr);
                break;
            }
        }


    } while (FALSE);


    SAFEDELETES(meTests);

    if (fVisVal)
    {
        do
        {
            logger.Log(_T("COMMENT>> Visual Validation . Running test again with SW \n"));
            meTests = new CMediaEngineTest;
            if (meTests == NULL)
            {
                hr = E_OUTOFMEMORY;
                logger.Log(_T("ERROR>> Out of memory, can't allocate Media Engine Test class (hr=0x%x)\n"), hr);
                break;
            }

            hr = meTests->SetGenericTestSettings(pszInput, pszOutput, ppConfigStrings[0], pszTestType, dwTestID, pszTestTitle, pszTestDLLName, eLogType);
            if (FAILED(hr))
            {
                logger.Log(_T("ERROR>> Failed to set test settings (hr=0x%x)"), hr);
                break;
            }

            hr = meTests->CTBSetupTest();
            if (FAILED(hr))
            {
                logger.Log(_T("ERROR>> Failed to set test settings (hr=0x%x)"), hr);
                break;
            }
            hr = meTests->SetDXVA(false);
            if (FAILED(hr))
            {
                logger.Log(_T("ERROR>> Failed to set DXVA Off . Skipping SW Pass   (hr=0x%x)"), hr);
                break;
            }

            hr = meTests->CTBExecuteTest();
            if (FAILED(hr))
            {
                logger.Log(_T("ERROR>> Failed In Test execution with SW  decoder (hr=0x%x)"), hr);
                break;
            }

            hr = meTests->GetDXComparionInfo(comparisonInfo[1]);
            if (FAILED(hr))
            {
                logger.Log( _T("ERROR>> GetDXComparionInfo Failed With Error (hr=0x%x)\n"), hr);
                break;

            }

            SAFEDELETES(meTests);

            CDXCompare dxCompare;
            dxCompare.SetWexLogger();

            DWORD dwFileIndex = 0;
            DWORD dwFailedFramesListLength = 0;
            TCHAR *pszFailedFramesList = NULL;

            TCHAR ppszFrameFileNames1[MAX_FILES][MAX_FILE_LENGTH];
            TCHAR ppszFrameFileNames2[MAX_FILES][MAX_FILE_LENGTH];

            WEX::TestExecution::RuntimeParameters::TryGetValue<double>(L"FailPassRatio", fFailPassRatio);
            WEX::TestExecution::RuntimeParameters::TryGetValue<double>(L"PSNRThreshold", fPSNRThreshold);


            MultiByteToWideChar(CP_ACP, 0, comparisonInfo[0].ppszFrameFileNames[dwFileIndex], MAX_FILE_LENGTH*sizeof(char), ppszFrameFileNames1[dwFileIndex], MAX_FILE_LENGTH*sizeof(TCHAR));
            MultiByteToWideChar(CP_ACP, 0, comparisonInfo[1].ppszFrameFileNames[dwFileIndex], MAX_FILE_LENGTH*sizeof(char), ppszFrameFileNames2[dwFileIndex], MAX_FILE_LENGTH*sizeof(TCHAR));

            hr = dxCompare.ValidateData(
                ppszFrameFileNames1[dwFileIndex],
                ppszFrameFileNames2[dwFileIndex],
                comparisonInfo[0].dwWidth,
                comparisonInfo[0].dwHeight,
                comparisonInfo[0].subType,
                0,
                fPSNRThreshold,
                fFailPassRatio,
                TRUE
                );
            if (FAILED(hr))
            {
                logger.Log( _T("ERROR>> ValidateData failed (hr=0x%x)"), hr);

                hr = dxCompare.GetFailedFramesListAsString(FALSE, NULL, &dwFailedFramesListLength);
                if (FAILED(hr))
                {
                    logger.Log(_T("ERROR>> Unable to get failed frames list (hr=0x%x)\n"), hr);
                    break;
                }

                dwFailedFramesListLength += 1; // for trailing '\0'

                pszFailedFramesList = new TCHAR[dwFailedFramesListLength];
                if (pszFailedFramesList == NULL)
                {
                    logger.Log(_T("ERROR>> Unable to get failed frames list (hr=0x%x)\n"), hr);
                    hr = E_OUTOFMEMORY;
                    break;
                }

                SecureZeroMemory(pszFailedFramesList, sizeof(TCHAR) * dwFailedFramesListLength);

                hr = dxCompare.GetFailedFramesListAsString(FALSE, pszFailedFramesList, &dwFailedFramesListLength);

                if (hr != S_OK)
                {
                    logger.Log(_T("ERROR>> Unable to get failed frames list (hr=0x%x)\n"), hr);
                    break;
                }

                logger.Log( _T("Failed Frames: %s"), pszFailedFramesList);

                SAFEDELETE(pszFailedFramesList);
            }
            else
            {
                bool bKeepFiles = false;
                WEX::TestExecution::RuntimeParameters::TryGetValue<bool>(L"keepfiles", bKeepFiles);
                if (bKeepFiles == false)
                {      
                    logger.Log(_T("Deleting file: %s"), ppszFrameFileNames1[dwFileIndex]);
                    if (0 == DeleteFileW(ppszFrameFileNames1[dwFileIndex]))
                    {
                        logger.Log(_T("ERROR>> %s was not deleted! (Error Code: 0x%x)"), ppszFrameFileNames1[dwFileIndex], GetLastError());
                    }

                    logger.Log(_T("Deleting file: %s"), ppszFrameFileNames2[dwFileIndex]);
                    if (0 == DeleteFileW(ppszFrameFileNames2[dwFileIndex]))
                    {
                        logger.Log(_T("ERROR>> %s was not deleted! (Error Code: 0x%x)"), ppszFrameFileNames2[dwFileIndex], GetLastError());
                    }
                }

            }

        } while (FALSE);
    }

    SafeFreeConfigStringsLocal(&dwNumTests, &ppConfigStrings);    

    return hr;
}


CMediaEngineTest::CMediaEngineTest(): CBaseTest()
{
    m_DXVA = TRUE;
    m_cRef = 1;
    m_protectedPlay = FALSE;
    m_hr = S_OK;
    m_hPBCompleted = NULL;
    m_hFrameStepCompleted = NULL;
    m_Mode = Playback_DComp;
    m_pCompDevice = NULL;
    m_pCompVisual = NULL; 
    m_pDevContent = NULL;
    m_pDX11Device = NULL;
    m_pDXGIManager = NULL;
    m_pHwnd = NULL;
    m_pMediaEngine = NULL;
    m_pMediaEngineEx = NULL;
    m_pPlaybackWindow = NULL; 
    m_pCompTarget = NULL;
    m_fVisVal = FALSE;
    m_dwTimeout         = 1800000; 
    m_bMPOTest = false;
    m_isMPOCapable = notMPOCapable;
    m_pMPOStatusCallback = NULL;
    m_pDRMStatusCallback = NULL;
    m_bStopUpdateThread = FALSE;
    m_hUpdateLogThread = NULL;
    m_bETWInitialized = FALSE;

    wcscpy(m_pszPrevFileName, L"");
    wcscpy(m_pszCurrentFileName, L"");

    m_bUpdateOverlayInformation = false;
    m_bFirstPass = true;

    m_bMPOCS = false;

    m_dwCSIntervalTime = 0;
    m_dwCSTimeout = 0;

    m_uliNextCSTime.QuadPart = 0;

    m_bInterlaced = FALSE;

    m_hr = S_OK;

    m_pSampleCallback = NULL;

    m_ullLastCount = 0;

    m_dwLeft = 0;
    m_dwTop = 0;
    m_dwWidth = 0;
    m_dwHeight = 0;

    m_bFullScreen = TRUE;

    m_dwRotationCurrent = 0;
    m_dwRotationCount = 0;
    m_dwRotationCurrentCount = 0;
    m_dwRotationIntervalSeconds = DEFAULT_DEVICE_ROTATION_INTERVAL;
    m_uliNextRotationTime.QuadPart = 0;

    m_dwPSNRThreshold = 40;

    m_fPlaying = FALSE;
    m_fSeeking = FALSE;
    m_fEOS = FALSE;
    m_fStopFrameStep = TRUE;

    m_FrameStepThreadHandle = NULL;
    m_flStartPos = 0.0f;
    m_flSeek  = 0.0f;
    m_fDuration = 0.0f;
    m_flRate = 0;
    m_dwFrames =-1;
    m_dwFrameInterval = 0;
    m_nFrames = 0;
    m_ResetToken = 0;
    m_bSeek = FALSE;
    m_fD3DAWARE = FALSE;
    m_dwFileCount = 1;

    m_dwWidths = new DWORD[m_dwFileCount];
    if (m_dwWidths == NULL)
    {
        m_hr = E_OUTOFMEMORY;
    }

    m_dwHeights = new DWORD[m_dwFileCount];
    if (m_dwHeights == NULL)
    {
        m_hr = E_OUTOFMEMORY;
    }

    memset(&m_ComparisonInfo, 0, sizeof(m_ComparisonInfo));

    memset(m_wszLAURL, 0, MAX_PATH*sizeof(WCHAR));
    memset(m_wszKID, 0, MAX_PATH*sizeof(WCHAR));

    m_fd = -1;

    m_pwszContentFileName = NULL;
    m_pwszContentSourcePath = NULL;

    m_bHWDRMEnabled = FALSE;

    m_bDumpFrame = TRUE;

}
CMediaEngineTest::~CMediaEngineTest()
{
    m_fStopFrameStep = TRUE;
    SAFEDELETES(m_pPlaybackWindow);
    SAFEDELETES(m_pHwnd);
    SAFERELEASE(m_pCompVisual);
    SAFERELEASE(m_pCompTarget);
    SAFERELEASE(m_pCompDevice);
    SAFERELEASE(m_pDX11Device);
    SAFERELEASE(m_pDevContent);
    SAFERELEASE(m_pDXGIManager);
    if (m_hPBCompleted)
    {
        CloseHandle(m_hPBCompleted);
    }	

    if (m_hFrameStepCompleted)
    {
        CloseHandle(m_hFrameStepCompleted);
    }

    SAFERELEASE(m_pMediaEngineEx);
    SAFERELEASE(m_pMediaEngine); 
    SAFERELEASE(m_pSampleCallback);
    SAFEDELETE(m_dwWidths);
    SAFEDELETE(m_dwHeights);

    if (-1 != m_fd)
    {
        _close(m_fd);
        m_fd = -1;
    }


    SAFEFREE(m_pwszContentFileName);
    SAFEFREE(m_pwszContentSourcePath);
}

BOOL CMediaEngineTest::IsVisValTest(void)
{
    return m_fVisVal;
}

HRESULT CMediaEngineTest::SetKeepfiles(bool bKeepFiles)
{
    m_bKeepFiles = bKeepFiles;
    return S_OK;
}

bool CMediaEngineTest::GetDXVA()
{
    return (bool)m_DXVA;
}

HRESULT CMediaEngineTest::SetDXVA(bool fDxva)
{
    HRESULT hr = S_OK;
    HKEY hk = NULL;
    const TCHAR *lpcszREGKEY = L"SOFTWARE\\Microsoft\\SCrunch\\CodecPack\\MSDVD";
    const TCHAR *lpcszREGKEY2 = L"SOFTWARE\\Wow6432Node\\Microsoft\\SCrunch\\CodecPack\\MSDVD";

    const TCHAR *lpcszWMVREGKEY = L"SOFTWARE\\Microsoft\\SCrunch\\WMVideo";

    const TCHAR *lpcszHEVCREGKEY = L"SOFTWARE\\Microsoft\\SCrunch\\H265";


    DWORD dwValue = 0;
    LONG lResult = 0;

    do
    {
        if (!fDxva)
        {

            lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                lpcszREGKEY,
                0,
                NULL,
                REG_OPTION_NON_VOLATILE,
                KEY_ALL_ACCESS,
                NULL,
                &hk,
                NULL);

            if (lResult != ERROR_SUCCESS || hk == NULL)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("ERROR	>>\t Unable to Open the registry key for setting the SW fallback 0x%x\n"), hr);
                break;
            }
            Log(_T("INFO	>>\t Created Registry Key with handle 0x%p\n"), hk);
            if (RegSetValueEx(hk,_T("DXVA2"),0,REG_DWORD,(BYTE*)&dwValue,sizeof(DWORD)) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("ERROR	>>\t Unable to Set the registry key for DXVA2 setting the SW fallback 0x%x\n"), hr);
                break;
            }

            Log(_T("INFO	>>\t Set value DXVA for Registry Key with handle 0x%p\n"), hk);
            if (RegSetValueEx(hk,_T("DXVA"),0,REG_DWORD,(BYTE*)&dwValue,sizeof(DWORD)) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("ERROR	>>\t Unable to Set the registry key for DXVA setting the SW fallback 0x%x\n"), hr);
                break;
            }
            Log(_T("INFO	>>\t Set value DXVA2 for Registry Key with handle 0x%p\n"), hk);
            RegFlushKey(hk);
            RegCloseKey(hk);
            hk = NULL;

            if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                lpcszREGKEY2,
                0,
                NULL,
                REG_OPTION_NON_VOLATILE,
                KEY_ALL_ACCESS,
                NULL,
                &hk, NULL) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("ERROR  >>\t Unable to Open the registry key for setting the SW fallback 0x%x\n"), hr);
                break;
            }

            if (RegSetValueEx(hk,_T("DXVA2"),0,REG_DWORD,(BYTE*)&dwValue,sizeof(DWORD)) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("ERROR  >>\t Unable to Set the registry key for DXVA2 setting the SW fallback0x%x\n"), hr);
                break;
            }
            if (RegSetValueExW(hk, _T("DXVA"), 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD)) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("ERROR  >>\t Unable to Set the registry key for DXVA setting the SW fallback 0x%x\n"), hr);
                break;
            }
            RegFlushKey(hk);
            RegCloseKey(hk);
            hk = NULL;

            if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                lpcszWMVREGKEY,
                0,
                NULL,
                REG_OPTION_NON_VOLATILE,
                KEY_ALL_ACCESS,
                NULL,
                &hk, NULL) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("ERROR  >>\t Unable to Open the registry key for setting the SW fallback 0x%x\n"), hr);
                break;
            }

            if (RegSetValueEx(hk,_T("DXVA1"),0,REG_DWORD,(BYTE*)&dwValue,sizeof(DWORD)) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("ERROR  >>\t Unable to Set the registry key for DXVA2 setting the SW fallback0x%x\n"), hr);
                break;
            }
            if (RegSetValueExW(hk, _T("DXVA"), 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD)) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("ERROR  >>\t Unable to Set the registry key for DXVA setting the SW fallback 0x%x\n"), hr);
                break;
            }
            RegFlushKey(hk);
            RegCloseKey(hk);
            hk = NULL;



            if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                lpcszHEVCREGKEY,
                0,
                NULL,
                REG_OPTION_NON_VOLATILE,
                KEY_ALL_ACCESS,
                NULL,
                &hk, NULL) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("ERROR  >>\t Unable to Open the registry key for setting the SW fallback 0x%x\n"), hr);
                break;
            }

            if (RegSetValueEx(hk, _T("DXVA1"), 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD)) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("ERROR  >>\t Unable to Set the registry key for DXVA2 setting the SW fallback0x%x\n"), hr);
                break;
            }
            if (RegSetValueExW(hk, _T("DXVA"), 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD)) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("ERROR  >>\t Unable to Set the registry key for DXVA setting the SW fallback 0x%x\n"), hr);
                break;
            }
            RegFlushKey(hk);
            RegCloseKey(hk);
            hk = NULL;
        }

        else
        {
            if (RegDeleteKey(HKEY_LOCAL_MACHINE, lpcszREGKEY) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("WARNING  >>\t Could Not Delete DXVA Reg Key %s with error 0x%x"), lpcszREGKEY, hr);
                break;
            }
            if (RegDeleteKey(HKEY_LOCAL_MACHINE, lpcszREGKEY2) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("WARNING  >>\t Could Not Delete DXVA Reg Key %s with error 0x%x"), lpcszREGKEY2, hr);
                break;
            }
            if (RegDeleteKey(HKEY_LOCAL_MACHINE, lpcszWMVREGKEY) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("WARNING  >>\t Could Not Delete DXVA Reg Key %s with error 0x%x"), lpcszWMVREGKEY, hr);
                break;
            }

            if (RegDeleteKey(HKEY_LOCAL_MACHINE, lpcszHEVCREGKEY) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("WARNING  >>\t Could Not Delete DXVA Reg Key %s with error 0x%x"), lpcszHEVCREGKEY, hr);
                break;
            }
        }
    } while (false);

    m_fD3DAWARE = fDxva;
    return hr;
}

HRESULT CMediaEngineTest::SetStrictTypeCheck(bool fCheck)
{
    HRESULT hr = S_OK;
    HKEY hk = NULL;
    const TCHAR *lpcszREGKEY = L"SOFTWARE\\Microsoft\\Windows Media Foundation\\Platform\\XVP";
    DWORD dwValue = 1;
    LONG lResult = 0;

    do
    {
        if (fCheck)
        {
            lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                                     lpcszREGKEY,
                                     0,
                                     NULL,
                                     REG_OPTION_NON_VOLATILE,
                                     KEY_ALL_ACCESS,
                                     NULL,
                                     &hk,
                                     NULL);

            if (lResult != ERROR_SUCCESS || hk == NULL)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("ERROR	>>\t Unable to Open the registry key for setting XVP Strict Type Check 0x%x\n"), hr);
                break;
            }
            Log(_T("INFO	>>\t Created Registry Key with handle 0x%p\n"), hk);
            if (RegSetValueEx(hk, _T("UseStrictTypeCheck"), 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD)) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("ERROR	>>\t Unable to Set the registry key for StrictTypeCheck 0x%x\n"), hr);
                break;
            }
            RegFlushKey(hk);
            RegCloseKey(hk);
            hk = NULL;
        }

        else
        {
            if (RegDeleteKey(HKEY_LOCAL_MACHINE, lpcszREGKEY) != ERROR_SUCCESS)
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("WARNING  >>\t Could Not Delete Strict Type Check Reg Key %s with error 0x%x"), lpcszREGKEY, hr);
                break;
            }
        }
    } while (false);
    return hr;
}
//MPO

HRESULT GetOverlayCaps(DXGI_MULTIPLANE_OVERLAY_CAPS* pOverlayCaps, DWORD *pdwLeft, DWORD* pdwTop, DWORD* pdwWidth, DWORD* pdwHeight, CSmartLogger logger)
{
    D3D_FEATURE_LEVEL featureLevels[] = 
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
    };

    CComPtr<ID3D11Device> d3d11Device;
    D3D_FEATURE_LEVEL featureLevelSupported;
    CComPtr<ID3D11DeviceContext> d3d11DeviceContext;
    HRESULT hr = S_OK;

    HWND *pHwnd = NULL;
    CMFPlaybackWindow* pPlaybackWindow=NULL;


    do
    {
        if((NULL == pOverlayCaps) || (NULL == pdwLeft)  || (NULL == pdwTop) || (NULL == pdwWidth) || (NULL == pdwHeight) )
        {
            logger.Log(_T("ERROR>> pOverlayCaps is NULL"));
            hr = E_INVALIDARG;
            break;
        }

        hr = MakeProcessDpiAware(logger);
        if(FAILED(hr))
        {
            logger.Log(_T("ERROR>> MakeProcessDpiAware failed (hr=0x%x)"), hr);
            break;
        }        

        hr = ::D3D11CreateDevice(
            nullptr, 
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            D3D11_CREATE_DEVICE_SINGLETHREADED |                                // Set this flag to increase the performance as this device is accessed from this thread only
            D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS |      // Set this flag so that multiple threads won't be created for rasterization as there is no rasterization
            D3D11_CREATE_DEVICE_BGRA_SUPPORT |                                  // Set this flag to be able to create the swap chain with DXGI_FORMAT_B8G8R8A8_UNORM
            D3D11_CREATE_DEVICE_PREVENT_ALTERING_LAYER_SETTINGS_FROM_REGISTRY,  // Set this flag to ignore the registry setting that turns on the debug layer
            featureLevels,
            sizeof(featureLevels)/sizeof(D3D_FEATURE_LEVEL),
            D3D11_SDK_VERSION, 
            &d3d11Device,
            &featureLevelSupported,
            &d3d11DeviceContext);
        if(FAILED(hr))
        {
            logger.Log(_T("ERROR>> D3D11CreateDevice failed (hr=0x%x)"), hr);
            break;
        }


        CComPtr<ID3D11Device1> d3d11Device1;
        hr = d3d11Device->QueryInterface(__uuidof(ID3D11Device1), (void**)&d3d11Device1);
        if(FAILED(hr))
        {
            logger.Log(_T("ERROR>> QueryInterface for ID3D11Device1 failed (hr=0x%x)"), hr);
            break;
        }

        CComPtr<IDXGIDevice> dxgiDevice;
        hr = d3d11Device->QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgiDevice);
        if(FAILED(hr))
        {
            logger.Log(_T("ERROR>> QueryInterface for ID3D11Device1 failed (hr=0x%x)"), hr);
            break;
        }

        CComPtr<IDXGIAdapter> dxgiAdapter;
        hr = dxgiDevice->GetAdapter(&dxgiAdapter);
        if(FAILED(hr))
        {
            logger.Log(_T("ERROR>> GetAdapter failed (hr=0x%x)"), hr);
            break;
        }

        CComPtr<IDXGIFactory> dxgiFactory;
        hr = dxgiAdapter->GetParent(__uuidof( IDXGIFactory ), (void**)&dxgiFactory);
        if(FAILED(hr))
        {
            logger.Log(_T("ERROR>> GetParent failed (hr=0x%x)"), hr);
            break;
        }

        CComPtr<IDXGIFactory2> dxgiFactory2;
        hr = dxgiFactory->QueryInterface(__uuidof(IDXGIFactory2), (void**)&dxgiFactory2);
        if(FAILED(hr))
        {
            logger.Log(_T("ERROR>> QueryInterface for IDXGIFactory2 failed (hr=0x%x)"), hr);
            break;
        }

        CComPtr<IDXGIOutput> dxgiOutput;
        hr = dxgiAdapter->EnumOutputs(0, &dxgiOutput);
        if(FAILED(hr))
        {
            logger.Log(_T("ERROR>> EnumOutputs failed (hr=0x%x)"), hr);
            break;
        }

        DXGI_OUTPUT_DESC dxgiOutputDesc;
        hr = dxgiOutput->GetDesc(&dxgiOutputDesc);
        if(FAILED(hr))
        {
            logger.Log(_T("ERROR>> GetDesc failed (hr=0x%x)"), hr);
            break;
        }

        logger.Log(_T("INFO>> Creating window"));
        pHwnd = new HWND;
        if( NULL == pHwnd )
        {
            hr = E_OUTOFMEMORY;
            logger.Log(_T("INFO>> Failed in creating playback window:0x%x"), hr);
            break;
        }

        MFRegisterWindowClass();
        pPlaybackWindow = new CMFPlaybackWindow();
        if( NULL == pPlaybackWindow )
        {
            hr = E_OUTOFMEMORY;
            logger.Log(_T("INFO>> Failed in creating CMFPlaybackWindow:0x%x"), hr);
            break;
        }

        hr = pPlaybackWindow->CreatePlaybackWindow(TRUE, pHwnd);
        if (hr != S_OK)
        {
            logger.Log(_T("INFO>> Failed in CreatePlaybackWindow:0x%x"), hr);
            break;
        }

        hr = pPlaybackWindow->SetFullScreen(TRUE);
        if (hr != S_OK)
        {
            logger.Log(_T("INFO>> Failed in SetFullScreen:0x%x"), hr);
            break;
        }

        long xPos, yPos, pcx, pcy;
        xPos = dxgiOutputDesc.DesktopCoordinates.left;
        yPos = dxgiOutputDesc.DesktopCoordinates.top;
        pcx = dxgiOutputDesc.DesktopCoordinates.right - dxgiOutputDesc.DesktopCoordinates.left;
        pcy = dxgiOutputDesc.DesktopCoordinates.bottom - dxgiOutputDesc.DesktopCoordinates.top;

        logger.Log(_T("INFO>> xPos=%ld, yPos=%ld, pcx=%ld, pcy=%ld"), xPos, yPos, pcx, pcy);


        logger.Log(_T("INFO>> Creating a swap chain in full-screen exclusive mode"));
        DXGI_SWAP_CHAIN_DESC1 Desc1 = {0};
        Desc1.Width                 = pcx;
        Desc1.Height                = pcy;
        Desc1.Format                = DXGI_FORMAT_B8G8R8A8_UNORM;
        Desc1.Stereo                = FALSE;
        Desc1.SampleDesc.Count      = 1;
        Desc1.SampleDesc.Quality    = 0;
        Desc1.BufferUsage           = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_BACK_BUFFER;
        Desc1.BufferCount           = 2;
        Desc1.Scaling               = DXGI_SCALING_STRETCH;
        Desc1.SwapEffect            = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        Desc1.AlphaMode             = DXGI_ALPHA_MODE_IGNORE;
        Desc1.Flags                 = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc = {0};
        fullscreenDesc.RefreshRate.Numerator = 0;
        fullscreenDesc.RefreshRate.Denominator = 1;
        fullscreenDesc.ScanlineOrdering   = DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE;
        fullscreenDesc.Scaling            = DXGI_MODE_SCALING_UNSPECIFIED;
        fullscreenDesc.Windowed           = FALSE;
        hr = dxgiFactory->MakeWindowAssociation(*pHwnd, 0);
        if(FAILED(hr))
        {
            logger.Log(_T("ERROR>> MakeWindowAssociation failed (hr=0x%x)"), hr);
            break;
        }

        CComPtr<IDXGISwapChain1> pSwapChain1;
        hr = dxgiFactory2->CreateSwapChainForHwnd(d3d11Device1, *pHwnd, &Desc1, &fullscreenDesc, nullptr, &pSwapChain1);
        if(FAILED(hr))
        {
            logger.Log(_T("ERROR>> CreateSwapChainForHwnd failed (hr=0x%x)"), hr);
            break;
        }

        CComPtr<IDXGISwapChainDWM1> dxgiSwapChainDwm1;
        hr = pSwapChain1->QueryInterface(__uuidof(IDXGISwapChainDWM1), (void**)&dxgiSwapChainDwm1);
        if(FAILED(hr))
        {
            logger.Log(_T("ERROR>> QueryInterface for IDXGISwapChainDWM1 failed (hr=0x%x)"), hr);
            pSwapChain1->SetFullscreenState(FALSE, NULL);
            break;
        }

        DXGI_MULTIPLANE_OVERLAY_CAPS overlayCaps;
        ZeroMemory(&overlayCaps, sizeof(overlayCaps));
        hr = dxgiSwapChainDwm1->GetMultiplaneOverlayCaps(&overlayCaps);
        if(FAILED(hr))
        {
            logger.Log(_T("ERROR>> GetMultiplaneOverlayCaps failed (hr=0x%x)"), hr);
            pSwapChain1->SetFullscreenState(FALSE, NULL);
            break;
        }

        *pOverlayCaps = overlayCaps;
        *pdwLeft = xPos;
        *pdwTop = yPos;
        *pdwWidth = pcx;
        *pdwHeight = pcy;

        logger.Log(_T("INFO>> left=%ld, top=%ld, width=%ld, height=%ld"), *pdwLeft, *pdwTop, *pdwWidth, *pdwHeight);

        pSwapChain1->SetFullscreenState(FALSE, NULL);

    }
    while(FALSE);

    SAFEDELETES(pPlaybackWindow);
    SAFEDELETES(pHwnd);   

    return hr;
}

HRESULT MakeProcessDpiAware(CSmartLogger logger)
{
    HRESULT hr = S_OK;

    // Check if the TAEF process is per-monitor DPI aware
    HMODULE libHandle = NULL;

    do
    {
        libHandle = LoadLibraryEx(L"SHcore.dll", NULL, 0);
        if (NULL == libHandle)
        {
            logger.Log(_T("ERROR>> LoadLibrary failed (hr=0x%x)"), HRESULT_FROM_WIN32(GetLastError()));
            break;
        }

        PROCESS_DPI_AWARENESS processDpiAwareness = PROCESS_DPI_UNAWARE;
        typedef HRESULT(WINAPI *GetProcessDPIAwareness)(HANDLE hprocess, PROCESS_DPI_AWARENESS *value);
        typedef HRESULT(WINAPI *SetProcessDPIAwareness)(PROCESS_DPI_AWARENESS value);

        GetProcessDPIAwareness pfnGetProcessDPIAwareness = reinterpret_cast<GetProcessDPIAwareness>(GetProcAddress(libHandle, "GetProcessDpiAwareness"));
        if (NULL == pfnGetProcessDPIAwareness)
        {
            logger.Log(_T("ERROR>> GetProcAddress for GetProcessDPIAwareness failed (hr=0x%x)"), HRESULT_FROM_WIN32(GetLastError()));
            break;
        }

        SetProcessDPIAwareness pfnSetProcessDPIAwareness = reinterpret_cast<SetProcessDPIAwareness>(GetProcAddress(libHandle, "SetProcessDpiAwareness"));
        if (NULL == pfnSetProcessDPIAwareness)
        {
            logger.Log(_T("ERROR>> GetProcAddress for SetProcessDPIAwareness failed (hr=0x%x)"), HRESULT_FROM_WIN32(GetLastError()));
            break;
        }


        hr = pfnGetProcessDPIAwareness(nullptr, &processDpiAwareness);
        if (FAILED(hr))
        {
            logger.Log(_T("ERROR>> GetProcessDpiAwareness failed (hr=0x%x)"), hr);
            break;
        }

        if (PROCESS_PER_MONITOR_DPI_AWARE == processDpiAwareness)
        {
            logger.Log(_T("INFO>> The process is already Per-Monitor DPI aware"));
        }
        else
        {
            logger.Log(_T("INFO>> The process is not Per-Monitor DPI aware"));

            // If not, make it per-monitor DPI aware as the tests use DXGI API to query desktop resolution which is in device coordinates
            hr = pfnSetProcessDPIAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
            if (FAILED(hr))
            {
                logger.Log(_T("ERROR>> SetProcessDpiAwareness failed (hr=0x%x)"), hr);
                break;
            }

            processDpiAwareness = PROCESS_DPI_UNAWARE;
            hr = pfnGetProcessDPIAwareness(nullptr, &processDpiAwareness);
            if (FAILED(hr))
            {
                logger.Log(_T("ERROR>> GetProcessDpiAwareness failed (hr=0x%x)"), hr);
                break;
            }

            if (PROCESS_PER_MONITOR_DPI_AWARE != processDpiAwareness)
            {
                logger.Log(_T("ERROR>> PROCESS_PER_MONITOR_DPI_AWARE != processDpiAwareness "));
                break;
            }
        }
    } while (FALSE);

    FreeLibrary(libHandle);

    return hr;
}

HRESULT CMediaEngineTest::CallbackEntry(IMFSample*  pSample)
{
    HRESULT hr = S_OK;
    UINT32 bInterlaced = FALSE;

    do
    {
        if(pSample == NULL)
        {
            hr = E_INVALIDARG;
            break;
        }

        hr = pSample->GetUINT32(MFSampleExtension_Interlaced, &bInterlaced);
        if(FAILED(hr))
        {
            if(hr != MF_E_ATTRIBUTENOTFOUND)
            {
                Log(_T("ERROR>> Failed in GetUINT32 MFSampleExtension_Interlaced on pSample , (hr=0x%x)"), hr);
                break;
            }
            else
            {         
                hr = S_OK;
            }            
        }

        m_bInterlaced = bInterlaced;   
    }while(false);

    return hr;
}


HRESULT CMediaEngineTest::RGBOverlayExpected(bool* pbRGB)
{
    HRESULT hr = S_OK;
    bool bRGB = false;

    IMFMediaType* pMediaType = NULL;

    GUID subType = GUID_NULL;

    if (NULL == pbRGB)
    {
        return E_INVALIDARG;
    }

    do
    {
        //if content is interlaced
        if(m_bInterlaced == TRUE)
        {
            bRGB = true;
            if(m_bFirstPass && m_bUpdateOverlayInformation)
            {
                Log(_T("INFO>> interlaced content =TRUE"));
            }
            break;
        }

    } while (FALSE);

    *pbRGB = bRGB;
    SAFERELEASE(pMediaType);

    return hr;
}

HRESULT CMediaEngineTest::EnableEventMonitor(void)
{
    HRESULT hr = S_OK;

    GUID    guidProvider = GUID_NULL;

    if (m_bHWDRMEnabled)
    {
        guidProvider = GUID_MFHWDRMETW;
    }
    else
    {
        guidProvider = GUID_DWMETW;
    }


    do
    {
        hr = m_etwController.EnableProvider(
            &guidProvider
            );
        if (FAILED(hr))
        {
            Log(_T("ERROR>> Failed to enable ETW provider."));
            break;
        }

        if (m_bHWDRMEnabled)
        {
            Log(_T("INFO>> Registering callback for HWDRM ETW events."));
            m_pDRMStatusCallback = new CHWDRM_DEVICECALLETWCallback();
            if (m_pDRMStatusCallback == NULL)
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            hr = m_etwConsumer.AddCallback(
                &guidProvider,
                m_pDRMStatusCallback
                );
        }
        else
        {
            Log(_T("INFO>> Registering callback for MPO ETW events."));
            m_pMPOStatusCallback = new COVERLAY_PRESENTETWCallback();
            if (m_pMPOStatusCallback == NULL)
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            hr = m_etwConsumer.AddCallback(
                &guidProvider,
                m_pMPOStatusCallback
                );
            if (FAILED(hr))
            {
                break;
            }
        }

    } while (false);

    return hr;
}


HRESULT CMediaEngineTest::UpdateEventsInformation(void)
{
    HRESULT hr = S_OK;

    if (!m_bHWDRMEnabled)
    {
        bool bRGB = false;
        WCHAR pwszOverlayText[MAX_PATH] = { 0 };
        WCHAR pwszLogText[MAX_PATH] = { 0 };

        ULONGLONG ullCount = 0;

        COVERLAY_PRESENTETWCallback::OverlayState overlayState;

        float fDstRectWidth = 0;
        float fDstRectHeight = 0;
        float fSrcRectWidth = 0;
        float fSrcRectHeight = 0;

        SYSTEM_INFO sysInfo = { 0 };

        DWORD dwVideoWidth = 0;
        DWORD dwVideoHeight = 0;

        do
        {
            if ((m_pMPOStatusCallback == NULL) || (m_bUpdateOverlayInformation == false) || (m_pMPOStatusCallback->IsActive() == FALSE))
            {
                hr = S_FALSE;
                break;
            }

            ullCount = m_pMPOStatusCallback->GetCount();
            if (m_ullLastCount != ullCount)
            {
                m_ullLastCount = ullCount;
            }
            else
            {
                hr = S_FALSE;
                break;
            }

            if (m_isMPOCapable == rgbMPOCapable)
            {
                bRGB = true;
            }
            else
            {
                hr = RGBOverlayExpected(&bRGB);
                if (FAILED(hr))
                {
                    Log(_T("ERROR>> Failed in RGBOverlayExpected, (hr=0x%x)"), hr);
                    break;
                }
            }

            hr = m_pMPOStatusCallback->GetOverlayState(&overlayState);
            if (FAILED(hr))
            {
                break;
            }

            //Does this plane belong to me?
            if ((overlayState.DstRectLeft >= m_dwLeft)
                && (overlayState.DstRectTop >= m_dwTop)
                && (overlayState.DstRectRight <= (m_dwLeft + m_dwWidth))
                && (overlayState.DstRectBottom <= (m_dwTop + m_dwHeight)))
            {
                /*
                •	A single non-fullscreen plane that can be stretched or shrunk according to the caps.
                •	Two side by side videos if they report more than 1 YUV plane, but these should have little to no shrinking.
                •	If they don’t support stretching, the dest rect must have an even width.
                */
                if (bRGB == false)
                {
                    fDstRectWidth = static_cast<float>(overlayState.DstRectRight - overlayState.DstRectLeft);
                    fDstRectHeight = static_cast<float>(overlayState.DstRectBottom - overlayState.DstRectTop);
                    fSrcRectWidth = static_cast<float>(overlayState.SrcRectRight - overlayState.SrcRectLeft);
                    fSrcRectHeight = static_cast<float>(overlayState.SrcRectBottom - overlayState.SrcRectTop);

                    if (fDstRectWidth / fSrcRectWidth < m_overlayCaps.Overlay.MaxShrinkFactor)
                    {
                        Log(_T("INFO>> Horizontal shrinkage is less that MaxShrinkFactor"));
                        bRGB = true;
                    }
                    else if (fDstRectHeight / fSrcRectHeight < m_overlayCaps.Overlay.MaxShrinkFactor)
                    {
                        Log(_T("INFO>> Vertical shrinkage is less that MaxShrinkFactor"));
                        bRGB = true;
                    }
                    else if (((static_cast<UINT32>(fDstRectWidth)& 0x1)) && ((m_overlayCaps.Overlay.OverlayCaps & DXGI_MULTIPLANE_OVERLAY_FEATURE_CAPS_BILINEAR_FILTER) == 0) && ((m_overlayCaps.Overlay.OverlayCaps & DXGI_MULTIPLANE_OVERLAY_FEATURE_CAPS_HIGH_FILTER) == 0))
                    {
                        Log(_T("INFO>> Odd width and no stretch"));
                        bRGB = true;
                    }
                }


                //if the is baytrail, non full screen app and there is scaling, there is going to be no MPO support and thats expected
                GetNativeSystemInfo(&sysInfo);
                if (sysInfo.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_ARM)
                {
                    if (m_bFullScreen == FALSE && ((fDstRectWidth != fSrcRectWidth) || (fDstRectHeight != fSrcRectHeight)))
                    {
                        Log(_T("INFO>> On Baytrail platform, in non-fullscreen mode and stretching, hence not expecting MPO."));

                        m_bStopUpdateThread = TRUE;
                        m_isMPOCapable = notMPOCapable;
                    }
                }
            }
            switch (overlayState.dwOverlayState)
            {
            case COVERLAY_PRESENTETWCallback::eOverlay_RGB:
            {
                swprintf_s(
                    pwszOverlayText,
                    MAX_PATH,
                    L"RGB (%u)",
                    m_pMPOStatusCallback->GetOverlayTransitionCount()
                    );
                if (!bRGB)
                {
                    swprintf_s(
                        pwszLogText,
                        MAX_PATH,
                        L"ERROR>> DWM Expected YUV overlay but got %s",
                        pwszOverlayText
                        );
                    hr = E_FAIL;
                }
                else
                {
                    swprintf_s(
                        pwszLogText,
                        MAX_PATH,
                        L"INFO>> DWM Expected RGB overlay and got some %s",
                        pwszOverlayText
                        );
                }
            }
                break;
            case COVERLAY_PRESENTETWCallback::eOverlay_YUV:
            {
                swprintf_s(
                    pwszOverlayText,
                    MAX_PATH,
                    L"YUV (%u)",
                    m_pMPOStatusCallback->GetOverlayTransitionCount()
                    );
                if (bRGB)
                {
                    swprintf_s(
                        pwszLogText,
                        MAX_PATH,
                        L"INFO>> DWM Expected RGB overlay but got %s",
                        pwszOverlayText
                        );
                }
                else
                {
                    swprintf_s(
                        pwszLogText,
                        MAX_PATH,
                        L"INFO>> DWM Expected YUV overlay and got some %s",
                        pwszOverlayText
                        );
                }
            }
                break;
            case COVERLAY_PRESENTETWCallback::eOverlay_None:
            {
                swprintf_s(
                    pwszOverlayText,
                    MAX_PATH,
                    L"None (%u)",
                    m_pMPOStatusCallback->GetOverlayTransitionCount()
                    );

                swprintf_s(
                    pwszLogText,
                    MAX_PATH,
                    L"ERROR>> DWM Expected overlay but got %s",
                    pwszOverlayText
                    );


                hr = E_FAIL;
            }
                break;
            };

        } while (FALSE);

        if (FAILED(hr))
        {

            //Logged the failure once for this file. 
            //Wait till file changes
            Log(_T("INFO>> window   Right = %d, Left = %d, Bottom = %d, DstRectTop = %d"), m_dwLeft + m_dwWidth, m_dwLeft, m_dwTop + m_dwHeight, m_dwTop);
            Log(_T("INFO>> overlayState MaxShrinkFactor = %2.2f"), m_overlayCaps.Overlay.MaxShrinkFactor);
            Log(_T("INFO>> overlayState DstRectRight = %d, DstRectLeft = %d, SrcRectRight=%d, SrcRectLeft=%d"), overlayState.DstRectRight, overlayState.DstRectLeft, overlayState.SrcRectRight, overlayState.SrcRectLeft);
            Log(_T("INFO>> overlayState DstRectBottom = %d, DstRectTop = %d, SrcRectBottom=%d, SrcRectTop=%d"), overlayState.DstRectBottom, overlayState.DstRectTop, overlayState.SrcRectBottom, overlayState.SrcRectTop);

            Log(pwszLogText);
            m_bUpdateOverlayInformation = false;
        }
        else
        {
            if ((hr != S_FALSE) && m_bFirstPass)
            {
                Log(pwszLogText);
                m_bFirstPass = false;
            }

            if (hr == S_FALSE)
            {
                //if the is baytrail, non full screen app and there is scaling, there is going to be no MPO support and thats expected
                GetNativeSystemInfo(&sysInfo);
                if (sysInfo.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_ARM)
                {
                    hr = m_pMediaEngine->GetNativeVideoSize(&dwVideoWidth, &dwVideoHeight);
                    if (FAILED(hr))
                    {
                        Log(_T("ERROR>> Failed in GetNativeVideoSize, (hr=0x%x)"), hr);
                    }

                    if (m_bFullScreen == FALSE && ((m_dwWidth != dwVideoWidth) || (m_dwHeight != dwVideoHeight)))
                    {
                        Log(_T("INFO>> On Baytrail platform, in non-fullscreen mode and stretching, hence not expecting MPO."));

                        m_bStopUpdateThread = TRUE;
                        m_isMPOCapable = notMPOCapable;
                    }
                }
            }
        }
    }

    return hr;
}


DWORD WINAPI UpdateLogThread(
    void*   pParam)
{
    HRESULT             hr = S_OK;
    CMediaEngineTest*   pTest = NULL;

    do
    {
        if (pParam == NULL)
        {
            break;
        }

        pTest = (CMediaEngineTest*)pParam;

        while (pTest->ShouldStopUpdateThread() == FALSE)
        {
            hr = pTest->UpdateEventsInformation();
            if (FAILED(hr))
            {
                pTest->SetTestStatus(hr);
                break;
            }
        }
    } while (false);

    return hr;
}


void CMediaEngineTest::SetTestStatus(HRESULT hr)
{
    m_hr = hr;
}


BOOL CMediaEngineTest::ShouldStopUpdateThread(void)
{
    return m_bStopUpdateThread;
}


HRESULT CMediaEngineTest::StartUpdateThread(void)
{
    HRESULT hr = S_OK;

    do
    {
        StopUpdateThread();

        m_bStopUpdateThread = FALSE;

        m_hUpdateLogThread = CreateThread(
            NULL,
            0,
            &UpdateLogThread,
            this,
            0,
            NULL
            );
        if (m_hUpdateLogThread == NULL)
        {
            hr = E_FAIL;
            break;
        }
    } while (false);

    return hr;
}

HRESULT CMediaEngineTest::StopUpdateThread(void)
{
    HRESULT hr = S_OK;

    do
    {
        if (m_hUpdateLogThread == NULL)
        {
            break;
        }

        m_bStopUpdateThread = TRUE;

        if (WaitForSingleObject(
            m_hUpdateLogThread,
            10000
            ) != WAIT_OBJECT_0)
        {
            TerminateThread(
                m_hUpdateLogThread,
                0
                );
        }

        SAFECLOSEHANDLE(m_hUpdateLogThread);
    } while (false);

    return hr;
}


HRESULT CMediaEngineTest::InitializeETW()
{
    HRESULT hr = S_OK;

    do
    {
        WCHAR*  pwszControllerName = L"PlaybackController";
        DWORD dwETLNameLen;
        WCHAR* pwszETLName = NULL;

        dwETLNameLen = _scwprintf(
            L"%s.etl",
            pwszControllerName
            );

        dwETLNameLen++; // Account for the NULL

        pwszETLName = new WCHAR[dwETLNameLen];
        if (pwszETLName == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        swprintf_s(
            pwszETLName,
            dwETLNameLen,
            L"%s.etl",
            pwszControllerName
            );

        hr = StartUpdateThread();
        if (FAILED(hr))
        {
            Log(_T("ERROR>> Failed to start ETW update thread."));
            break;
        }

        DWORD dwMaxETLFileSize = 0; 

        #ifdef BUILD_MINWIN
            dwMaxETLFileSize = 100; // Set maximum ETL file size to 100MB for phone.
        #endif

        hr = m_etwController.ConfigureSession(
            pwszControllerName,
            pwszETLName,
            TRUE,
            dwMaxETLFileSize
            );
        if (FAILED(hr))
        {
            Log(_T("ERROR>> Failed to configure ETW session."));
            break;
        }

        hr = m_etwController.StartSession();
        if (FAILED(hr))
        {
            Log(_T("ERROR>> Failed to start ETW session."));
            break;
        }


        hr = EnableEventMonitor();
        if (FAILED(hr))
        {
            Log(_T("ERROR>> Failed in EnableEventMonitor."));
            break;
        }


        hr = m_etwConsumer.StartRealTimeConsumer(
            pwszControllerName
            );
        if (FAILED(hr))
        {
            break;
        }

        m_bETWInitialized = TRUE;
    } while (FALSE);

    return hr;
}


//PlaybackCallback 
HRESULT CMediaEngineTest::HandlePlaybackEvent(UINT message)
{
    HRESULT hr = S_OK;
    switch(message)
    {
    case WM_MFSESSION_PLAY:
        {
            Log(_T("INFO    >>\t HandlePlaybackEvent, WM_MFSESSION_PLAY (message=0x%x\n)"),message);
            if(m_pMediaEngine != NULL)
            {
                hr = m_pMediaEngine->Play();

                if(FAILED(hr))
                {
                    m_hr = hr;
                    Log(_T("ERROR>> Failed in playing media engine, (hr=0x%x\n)"),hr);
                }
            }               
        }
        break;
    case WM_MFSESSION_PAUSE:
        {
            Log(_T("INFO    >>\t HandlePlaybackEvent, WM_MFSESSION_PAUSE (message=0x%x\n)"),message);
            if(m_pMediaEngine != NULL)
            {
                hr = m_pMediaEngine->Pause();
                if(FAILED(hr))
                {
                    m_hr = hr;
                    Log(_T("ERROR>> Failed to Pause the media engine, (hr=0x%x\n)"),hr);
                }

            }
        }
        break;    

    case WM_MFSESSION_CLOSE:
        {
            Log(_T("INFO    >>\t HandlePlaybackEvent, WM_MFSESSION_CLOSE (message=0x%x\n)"),message);
            if(m_pMediaEngine != NULL)
            {
                hr = m_pMediaEngine->Pause();
                if(FAILED(hr))
                {
                    m_hr = hr;
                    if (hr != MF_E_SHUTDOWN)
                    {
                        Log(_T("ERROR>> Failed to Pause the media engine, (hr=0x%x\n)"), hr);
                    }
                }
            }
            SignalEndTest();
        }
        break;

    default:
        Log(_T("INFO    >>\t HandlePlaybackEvent, UNKNOWN Message (message=0x%x\n)"),message);
        break;
    }

    return hr;
}

//IMFMediaEngineNotify
STDMETHODIMP CMediaEngineTest::QueryInterface(__in REFIID riid, __out LPVOID* ppvObject)
{
    HRESULT hr = S_OK;
    do
    {
        if (riid == __uuidof(IMFMediaEngineNotify) || riid == __uuidof(IUnknown))
        {
            IMFMediaEngineNotify* pObj = this;
            *ppvObject = pObj;
        }
        else
        {
            *ppvObject = nullptr;
            hr = (E_NOINTERFACE);
            break;
        }
        AddRef();
    }while(false);
    return hr;
}      
STDMETHODIMP_(ULONG) CMediaEngineTest::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}
STDMETHODIMP_(ULONG) CMediaEngineTest::Release()
{
    LONG cRef = InterlockedDecrement(&m_cRef);
    if (cRef == 0)
    {
        delete this;
    }
    return cRef;
}
STDMETHODIMP CMediaEngineTest::EventNotify(DWORD event, DWORD_PTR param1, DWORD param2)
{
    HRESULT hr = S_OK;

    switch (event) 
    {
    case MF_MEDIA_ENGINE_EVENT_LOADEDMETADATA:
        {
            Log(_T("INFO   >>MF_MEDIA_ENGINE_EVENT_LOADEDMETADATA "));

            if (m_pMediaEngine) {
                hr = OnLoaded();
                if(FAILED(hr))
                {
                    m_hr = hr;
                    Log(_T("WARNNING>> Failed in OnLoaded, (hr=0x%x)"),hr);
                    hr = OnError();            
                }
            }
        }
        break;
    case MF_MEDIA_ENGINE_EVENT_LOADSTART:
        {
            Log(_T("INFO   >>\t Load start called 0x%x\n"),hr); 
        }
        break;
    case MF_MEDIA_ENGINE_EVENT_PROGRESS:
        {
            Log(_T("INFO   >>\tSource Loading progress 0x%x\n"),hr); 
        } 
        break; 

    case MF_MEDIA_ENGINE_EVENT_ABORT:
        {
            Log(_T("INFO   >>\tSource Loading progress aborted 0x%x\n"),hr); 
        } 
        break; 
    case MF_MEDIA_ENGINE_EVENT_STALLED:
        {
            Log(_T("INFO   >>\tLoad stalled 0x%x\n"),hr); 
        } 
        break; 

    case MF_MEDIA_ENGINE_EVENT_FIRSTFRAMEREADY:
        Log(_T("INFO   >>MF_MEDIA_ENGINE_EVENT_FIRSTFRAMEREADY \n"));

        if (m_pMediaEngine) {
            hr = OnFirstFrameReady();
            if (FAILED(hr))
            {
                m_hr = hr;
                Log(_T("WARNNING>> Failed in OnFirstFrameReady, (hr=0x%x)\n"), hr);
                hr = OnError();
            }
        }
        break;


    case MF_MEDIA_ENGINE_EVENT_CANPLAYTHROUGH:
    case MF_MEDIA_ENGINE_EVENT_CANPLAY:
        Log(_T("INFO   >>MF_MEDIA_ENGINE_EVENT_CANPLAY or MF_MEDIA_ENGINE_EVENT_CANPLAYTHROUGH \n"));

        if (m_pMediaEngine) {
            hr = OnCanPlay();
            if(FAILED(hr))
            {
                m_hr = hr;
                Log(_T("WARNNING>> Failed in OnCanPlay, (hr=0x%x)\n"),hr);            
                hr = OnError();
            }
        }
        break;


    case MF_MEDIA_ENGINE_EVENT_ENDED:
        Log(_T("INFO   >>MF_MEDIA_ENGINE_EVENT_ENDED \n"));

        hr = OnEnded();
        if(FAILED(hr))
        {            
            m_hr = hr;
            Log(_T("WARNNING>> Failed in OnEnded, (hr=0x%x) \n"),hr);
            hr = OnError();
        }
        break;

    case MF_MEDIA_ENGINE_EVENT_ERROR:
        m_hr = (HRESULT)param2;
        Log(_T("WARNNING>> Error event called, (hr=0x%x)\n"),hr);
        hr = OnError();
        break;
    }
    return hr;
}

HRESULT CMediaEngineTest::DesktopMPOTestSetup()
{
    HRESULT hr = S_OK;
    HKEY hkDesktopMPO = NULL;

    do
    {
        // set MPO regkey appropriately
        if (m_bMPOTest || (m_Mode > 4 && m_Mode < 8))
        {
            long  lretCode = RegCreateKeyEx(HKEY_LOCAL_MACHINE, DESKTOPMPO_REGKEYLOC, 0, NULL, 0,
                KEY_ALL_ACCESS, NULL, &hkDesktopMPO, NULL);
            if (lretCode != ERROR_SUCCESS)
            {
                if (lretCode == ERROR_ACCESS_DENIED)
                {
                    Log(L"ERROR>> Access denied to open/create desktop MPO regkey, run test elevated");
                }
                else
                {
                    Log(L"ERROR>> Unable to open/create desktop MPO regkey. Error = %d", lretCode);
                }
                hr = E_FAIL;
                break;
            }

            if (m_bMPOTest || m_Mode == Playback_MPOVV)
            {
                DWORD value = 1;
                if (RegSetValueEx(hkDesktopMPO, DESKTOPMPO_REGKEYNAME, 0, REG_DWORD,
                    (const BYTE*)&value, sizeof(value)) != ERROR_SUCCESS)
                {
                    Log(L"ERROR>> Unable to set desktop MPO regkey to 1");
                    hr = E_FAIL;
                    break;
                }
            }
            else if (m_Mode == Playback_MPOVV_DXVAHD || m_Mode == Playback_MPOVV_Soft)
            {
                DWORD value = 0;
                if (RegSetValueEx(hkDesktopMPO, DESKTOPMPO_REGKEYNAME, 0, REG_DWORD,
                    (const BYTE*)&value, sizeof(value)) != ERROR_SUCCESS)
                {
                    Log(L"ERROR>> Unable to set desktop MPO regkey to 0");
                    hr = E_FAIL;
                    break;
                }
            }

            // restart DWM

            // first add priviledge to current process
            HANDLE hToken;
            LUID luid;
            TOKEN_PRIVILEGES tkp;
            OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken);
            LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid);
            tkp.PrivilegeCount = 1;
            tkp.Privileges[0].Luid = luid;
            tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            AdjustTokenPrivileges(hToken, false, &tkp, sizeof(tkp), NULL, NULL);
            CloseHandle(hToken);

            // kill DWM
            PROCESSENTRY32 entry;
            entry.dwSize = sizeof(PROCESSENTRY32);
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
            if (Process32First(snapshot, &entry) == TRUE)
            {
                while (Process32Next(snapshot, &entry) == TRUE)
                {
                    if (_tcsicmp(entry.szExeFile, L"dwm.exe") == 0)
                    {
                        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
                        if (hProcess == NULL)
                        {
                            DWORD error = GetLastError();
                            Log(L"ERROR>> Failed to open DWM process, error = %d", error);
                            hr = E_FAIL;
                            break;
                        }

                        if (0 == TerminateProcess(hProcess, 0))
                        {
                            DWORD error = GetLastError();
                            Log(L"ERROR>> Failed to terminate DWM process, error = %d", error);
                            hr = E_FAIL;
                            break;
                        }

                        CloseHandle(hProcess);
                    }
                }
            }
            else
            {
                Log(L"ERROR>> Failed finding DWM process");
                hr = E_FAIL;
                break;
            }
            CloseHandle(snapshot);
        }
    } while (false);

    RegCloseKey(hkDesktopMPO);
    return hr;
}

HRESULT CMediaEngineTest::InitializeMediaEngine()
{
    HRESULT hr = S_OK;
    IMFAttributes* pAttrib = NULL;

    do
    {
#ifndef BUILD_MINWIN
        hr = DesktopMPOTestSetup();
        if (FAILED(hr))
        {
            return hr;
        }
#endif

        if (m_protectedPlay)
        {
            IMFContentProtectionDevice *pContentProtectionDevice = NULL;
            CLSID systemID = { 0xF4637010, 0x03C3, 0x42CD, { 0xB9, 0x32, 0xB4, 0x8A, 0xDF, 0x3A, 0x6A, 0x54 } };


            if (SUCCEEDED(MFCreateContentProtectionDevice(systemID, &pContentProtectionDevice)) &&
                pContentProtectionDevice)
            {
                m_bHWDRMEnabled = TRUE;
                Log(_T("INFO>>    HW DRM supported, verifying HW DRM playback."));
            }
            else
            {
                m_bHWDRMEnabled = FALSE;
                Log(_T("INFO>>    HW DRM not supported, verifying SW DRM playback."));
            }
            SAFERELEASE(pContentProtectionDevice);
        }


        CreateTimeout(m_dwTimeout);
        hr = CreateMediaEngineAttribute(NULL, DXGI_FORMAT_B8G8R8A8_UNORM, this, &pAttrib);
        if (FAILED(hr))
        {
            Log(_T("ERROR>> Failed in creating media engine attributes, (hr=0x%x)"), hr);
            break;
        }

        hr = MFCreateMediaEngine(0, pAttrib);
        if (FAILED(hr))
        {
            Log(_T("ERROR>> Failed in creating media engine, (hr=0x%x)"), hr);
            break;
        }

        hr = m_pMediaEngine->SetSource(m_pszInputFile);
        if (FAILED(hr))
        {
            Log(_T("ERROR>> Failed in setting source to media engine, (hr=0x%x)"), hr);
            break;
        }
               
    }while(false);
    SAFERELEASE(pAttrib);

    return hr;
}


HRESULT CMediaEngineTest::SetProperties(LPCTSTR pszPropertyName, LPCTSTR pszPropertyValues)
{

    HRESULT hr = S_OK;

    do
    {
        if (_tcsicmp(pszPropertyName, _T("input")) == 0)
        {
            SAFEFREE(m_pszInputFile);

            m_pszInputFile = (TCHAR*)malloc(sizeof(TCHAR)* MAX_STRING_LEN);
            if (m_pszInputFile == NULL)
            {
                Log(_T("ERROR>> Out of memory, can't allocate input file name"));
                hr = E_OUTOFMEMORY;
                break;
            }

            if (_stprintf_s(m_pszInputFile, MAX_STRING_LEN, _T("%s"), pszPropertyValues) <= 0)
            {
                Log(_T("ERROR>> Failed to copy input file name %s"), pszPropertyValues);
                hr = E_FAIL;
            }
        }
        else 
        {
            __super::SetProperties(pszPropertyName, pszPropertyValues);
        }
    } while (false);


    return hr;
}

HRESULT CMediaEngineTest::SetGenericTestSettings(LPCTSTR pszInputFileName, LPCTSTR pszOutputFileName, LPCTSTR pszConfig, LPCTSTR pszTestType, DWORD dwTestID, LPCTSTR pszTestTitle, LPCTSTR pszTestDLL, LogType eLogType)
{
    HRESULT hr = S_OK;

    do
    {
        SetLogType(eLogType);
        hr = SetProperties(_T("input"), pszInputFileName);
        if (hr != S_OK)
        {
            hr = E_INVALIDARG;
            break;
        }

        hr = SetProperties(_T("output"), pszOutputFileName);
        if (hr != S_OK)
        {
            hr = E_INVALIDARG;
            break;
        }

        hr = SetProperties(_T("params"), pszConfig);
        if (hr != S_OK)
        {
            hr = E_INVALIDARG;
            break;
        }

        WEX::TestExecution::RuntimeParameters::TryGetValue<bool>(L"keepfiles", m_bKeepFiles);

    }while(false);

    return hr;
}

HRESULT CMediaEngineTest::SetDRMSettings(LPCTSTR pszKID, LPCTSTR pszContentKey)
{
    HRESULT hr = S_OK;

    TCHAR LA_URL[MAX_PATH] = L"http://playready.directtaps.net/pr/svc/rightsmanager.asmx?PlayRight=1&UseSimpleNonPersistentLicense=1&ContentKey=";
    m_protectedPlay = TRUE;
    StringCchPrintf(m_wszLAURL, MAX_PATH, (L"%s%s"), LA_URL, pszContentKey);
    StringCchPrintf(m_wszKID, MAX_PATH, (L"%s"), pszKID);

    return hr;
}

HRESULT CMediaEngineTest::SetMPOSettings(DXGI_MULTIPLANE_OVERLAY_CAPS overlayCaps, MPOCapablities isMPOCapable, BOOL bFullScreen, DWORD dwLeft, DWORD dwTop, DWORD dwWidth, DWORD dwHeight)
{
    HRESULT hr = S_OK;

    m_overlayCaps = overlayCaps;
    m_bFullScreen = bFullScreen;

    m_dwLeft = dwLeft;
    m_dwTop = dwTop;
    m_dwWidth = dwWidth;
    m_dwHeight = dwHeight;

    m_isMPOCapable = isMPOCapable;
    m_bMPOTest = TRUE;
    m_bUpdateOverlayInformation = true;
    return hr;
}

HRESULT CMediaEngineTest::CreateMediaEngineAttribute(__in_opt IUnknown* pExtension,          // optional Media Engine extension
                                                     __in DXGI_FORMAT d3dRenderTarget,        // D3D render target format to use
                                                     __in IMFMediaEngineNotify* pAppCallback,// Callback to notify
                                                     __deref_out IMFAttributes** meAttrib
                                                     )
{
    HRESULT hr = S_OK; 
    IMFAttributes* pAttrib;
    D3D_FEATURE_LEVEL FeatureLevel;
    CTestCPM *pCPM = NULL;
    IPropertyStore* pMP4PropStore = NULL;

    do{
        hr = MFCreateAttributes(&pAttrib, 5); //for dcomp we need one more

        if (FAILED(hr))
        {
            Log(_T("ERROR>> Failed in MFCreateAttributes, (hr=0x%x)"), hr);
            break;
        }
        if (pExtension != NULL)
        {
            hr = pAttrib->SetUnknown(MF_MEDIA_ENGINE_EXTENSION, pExtension);
            if (FAILED(hr))
            {
                Log(_T("ERROR>> Failed in Setting MF_MEDIA_ENGINE_EXTENSION on MF Attribute , (hr=0x%x)"), hr);
                break;
            }
        }

        hr = CreateDX11Device(&FeatureLevel);
        if (FAILED(hr))
        {
            Log(_T("ERROR>> Failed in CreateDX11Device , (hr=0x%x)"), hr);
            break;
        }
        hr = MFCreateDXGIDeviceManager(&m_ResetToken, &m_pDXGIManager);
        if (FAILED(hr))
        {
            Log(_T("ERROR>> Failed in MFCreateDXGIDeviceManager , (hr=0x%x)"), hr);
            break;
        }

        hr = m_pDXGIManager->ResetDevice(m_pDX11Device, m_ResetToken);
        if (FAILED(hr))
        {
            Log(_T("ERROR>> Failed in m_pDXGIManager->ResetDevice , (hr=0x%x)"), hr);
            break;
        }
        hr = pAttrib->SetUnknown(MF_MEDIA_ENGINE_DXGI_MANAGER, m_pDXGIManager);
        if (FAILED(hr))
        {
            Log(_T("ERROR>> Failed in Setting MF_MEDIA_ENGINE_DXGI_MANAGER on MF Attribute , (hr=0x%x)"), hr);
            break;
        }

        if (m_protectedPlay)
        {
            hr = pAttrib->SetUINT32(MF_MEDIA_ENGINE_CONTENT_PROTECTION_FLAGS, MF_MEDIA_ENGINE_ENABLE_PROTECTED_CONTENT);
            if (FAILED(hr))
            {
                Log(_T("ERROR>> Failed in Setting MF_MEDIA_ENGINE_CONTENT_PROTECTION_FLAGS on MF Attribute , (hr=0x%x)"), hr);
                break;
            }

            //  Set a CPM            
            CTestCPM::CreateInstance(&pCPM);
            if (FAILED(hr))
            {
                Log(_T("ERROR>> Failed to create Content Protection Manager, (hr=0x%x)"), hr);
                break;
            }            

            if (m_bHWDRMEnabled)
            {
                Log(_T("INFO>> Enabling HW DRM mode in CPM."));
                pCPM->EnableHWDRMMode();
            }

            if (m_wszLAURL && m_wszKID)
            {
                pCPM->EnableLAURLOverride(TRUE);
                pCPM->SetLAURL(m_wszLAURL);
                pCPM->SetKID(m_wszKID);
            }
            else
            {
                hr = E_INVALIDARG;
                Log(_T("ERROR>> Override input not provided correctly, (hr=0x%x)"), hr);
                break; 
            }
        }
#ifdef BUILD_MINWIN

        hr = pAttrib->SetUnknown(__uuidof(ID3D11Device), m_pDX11Device);
        if(FAILED(hr))
        {
            Log(_T("ERROR   >>\t Failed in Setting D3D11 Device  (hr=0x%x)"),hr);
            break;  
        }
#endif
        if (m_Mode == Playback_DComp)
        {
            hr = CreateCompVisual(&m_pCompVisual);
            if (FAILED(hr))
            {
                Log(_T("ERROR  >>\t Failed in CreateCompVisual , (hr=0x%x)\n"), hr);
                break;
            }

            //We only create the Dcomp visual here in mobile systems and not set it because we need it to go in frame server mode initally and then set the DCOMP visual.

            if (m_protectedPlay)
            {
                hr = SetDRMAttributes(&pMP4PropStore);
                if (FAILED(hr))
                {
                    Log(L"Failed to set DRM Properties for playing Protected Media using ME pipeline");
                    break;
                }

                hr = pAttrib->SetUnknown(MF_MEDIA_ENGINE_SOURCE_RESOLVER_CONFIG_STORE, pMP4PropStore);
                if (FAILED(hr))
                {
                    Log(_T("ERROR>> Failed in Setting MF_MEDIA_ENGINE_OPM_HWND on MF Attribute , (hr=0x%x)"), hr);
                    break;
                }

                hr = pAttrib->SetUINT64(MF_MEDIA_ENGINE_OPM_HWND, (UINT64)*m_pHwnd);
                if (FAILED(hr))
                {
                    Log(_T("ERROR>> Failed in Setting MF_MEDIA_ENGINE_OPM_HWND on MF Attribute , (hr=0x%x)"), hr);
                    break;
                }
            }
            else
            {
                hr = pAttrib->SetUnknown(MF_MEDIA_ENGINE_PLAYBACK_VISUAL, m_pCompVisual);
                if (FAILED(hr))
                {
                    Log(_T("ERROR>> Failed in Setting MF_MEDIA_ENGINE_PLAYBACK_VISUAL on MF Attribute , (hr=0x%x)"), hr);
                    break;
                }
            }
        }
#ifndef BUILD_MINWIN
        if(m_Mode==Playback_HWND)
        {
            hr = CreatePlaybackWindow();
            if (FAILED(hr))
            {
                Log(_T("ERROR>> Failed in CreatePlaybackWindow , (hr=0x%x)"), hr);
                break;
            }

            hr = pAttrib->SetUINT64(MF_MEDIA_ENGINE_PLAYBACK_HWND, (UINT64)*m_pHwnd);
            if (FAILED(hr))
            {
                Log(_T("ERROR>> Failed in Setting MF_MEDIA_ENGINE_PLAYBACK_HWND on MF Attribute , (hr=0x%x)"), hr);
                break;
            }

        }
#endif

        hr = pAttrib->SetUnknown(MF_MEDIA_ENGINE_CONTENT_PROTECTION_MANAGER, static_cast<IMFContentProtectionManager *>(pCPM));

        if (FAILED(hr))
        {
            Log(_T("ERROR>> Failed in Setting MF_MEDIA_ENGINE_CONTENT_PROTECTION_MANAGER_FLAGS on MF Attribute, (hr=0x%x)"), hr);
            break;
        }

        if (d3dRenderTarget != DXGI_FORMAT_UNKNOWN)
        {
            hr = pAttrib->SetUINT32(MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT,d3dRenderTarget);
            if(FAILED(hr))
            {
                Log(_T("ERROR  >>\t Failed in Setting MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT on MF Attribute , (hr=0x%x)\n"),hr);
                break;  
            }
        }

        if (pAppCallback != NULL)
        {
            hr = pAttrib->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, pAppCallback);
            if(FAILED(hr))
            {
                Log(_T("ERROR  >>\t Failed in Setting MF_MEDIA_ENGINE_CALLBACK on MF Attribute , (hr=0x%x)\n"),hr);
                break;  
            }
        }		
        break;

    }while(false);

    *meAttrib = pAttrib;
    (*meAttrib)->AddRef();

    SAFERELEASE(pAttrib);
    SAFERELEASE(pCPM);
    SAFERELEASE(pMP4PropStore);
    return hr;
}
HRESULT WINAPI CMediaEngineTest::MFCreateMediaEngine(__in DWORD dwCreationFlags,__in IMFAttributes* pAttrib)
{
    HRESULT hr = S_OK;
    IMFMediaEngineClassFactory* pClassF = NULL;

    hr = CoInitializeEx( NULL, COINIT_MULTITHREADED );
    if(S_OK != hr && S_FALSE != hr)
    {
        return hr;
    }

    do{

        hr = CoCreateInstance(
            CLSID_MFMediaEngineClassFactory,
            NULL,
            CLSCTX_INPROC_SERVER,
            IID_PPV_ARGS(&pClassF)
            );
        if(FAILED(hr))
        {
            Log(_T("ERROR>> Failed in creating MFMediaEngineClass Factory, (hr=0x%x)"),hr);
            break;  
        }
        if(pClassF == NULL)
        {
            hr = E_FAIL;
            break;
        }
        pClassF->AddRef();
        hr = pClassF->CreateInstance(dwCreationFlags, pAttrib, &m_pMediaEngine);
        if(FAILED(hr))
        {
            Log(_T("ERROR>> Failed in creating MediaEngine , (hr=0x%x)"),hr);
            break;  
        }
        if(m_pMediaEngine == NULL)
        {
            hr = E_FAIL;
            break;
        }
        hr = m_pMediaEngine->QueryInterface(IID_PPV_ARGS(&m_pMediaEngineEx));
        if(FAILED(hr))
        {
            Log(_T("ERROR>> Failed in querying IMFMediaEngineEx from mediaengine, (hr=0x%x)"),hr);
            break;  
        }
        if(m_pMediaEngineEx == NULL)
        {
            hr = E_FAIL;
            break;
        }
        if (m_Mode == Playback_FrameServer)
        {
            hr = SetPlaybackRate(0.0);
            if (FAILED(hr))
            {
                break;
            }
        }
        break;
    }while(false);
    SAFERELEASE(pClassF);

    CoUninitialize();

    return hr;
}
HRESULT CMediaEngineTest::CreatePlaybackWindow(void)
{
    HRESULT hr = S_OK;
    do
    {
        if((m_pHwnd != NULL) && (m_pPlaybackWindow != NULL))
        {
            break;
        }
        SAFEDELETES(m_pHwnd);
        m_pHwnd = new HWND;
        if( NULL == m_pHwnd )
        {
            hr = E_OUTOFMEMORY;
            Log(_T("INFO>> Failed in creating playback window:0x%x"), hr);
            break;
        }

        MFRegisterWindowClass();
        SAFEDELETES(m_pPlaybackWindow);
        m_pPlaybackWindow = new CMFPlaybackWindow();
        m_pPlaybackWindow->SetCallback(this);
        if( NULL == m_pPlaybackWindow )
        {
            hr = E_OUTOFMEMORY;
            Log(_T("INFO>> Failed in creating CMFPlaybackWindow:0x%x"), hr);
            break;
        }

        hr = m_pPlaybackWindow->CreatePlaybackWindow(TRUE, m_pHwnd);
        if (hr != S_OK)
        {
            Log(_T("INFO>> Failed in CreatePlaybackWindow:0x%x"), hr);
            break;
        }

#ifndef BUILD_MINWIN
        hr = m_bMPOTest || (m_Mode > 4 && m_Mode < 8) ? m_pPlaybackWindow->SetFullScreen(m_bFullScreen) : m_pPlaybackWindow->SetFullScreen(FALSE);
        if (hr != S_OK)
        {
            Log(_T("INFO>> Failed in SetFullScreen:0x%x"), hr);
            break;
        }
#endif
#ifndef BUILD_MINWIN
        m_dwWidth = 640;
        m_dwHeight = 480;
        Log(_T("INFO>> ReSizing the Window :left = %d, top=%d, width=%d, height=%d\n"), m_dwLeft, m_dwTop, m_dwWidth, m_dwHeight);
        m_pPlaybackWindow->SetFullScreen(m_bFullScreen) ;
        hr = m_pPlaybackWindow->ReSize(m_dwLeft, m_dwTop, m_dwWidth, m_dwHeight);
        if (hr != S_OK)
        {
            Log(_T("INFO>> Failed in ReSize:0x%x\n"), hr);
            break;
        }
#endif
        //reset the window sizes
        //the height should be specified
        if(m_bMPOTest && m_bFullScreen == FALSE)
        {
            Log(_T("INFO>> ReSizing the Window :0x%x\n"), hr);
            hr = m_pPlaybackWindow->ReSize(m_dwLeft, m_dwTop, m_dwWidth, m_dwHeight);
            if (hr != S_OK)
            {
                Log(_T("INFO>> Failed in ReSize:0x%x\n"), hr);
                break;
            }
        }

    }while(false);
    return hr;
}
HRESULT CMediaEngineTest::CreateDX11Device(__out D3D_FEATURE_LEVEL* pFeatureLevel)
{
    BOOL fResult = FALSE;
    HRESULT                         hr              = S_OK;
    ID3D10Multithread*  pMultithread = NULL;
    D3D_DRIVER_TYPE DriverType = D3D_DRIVER_TYPE_HARDWARE;
    static const D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_1,

        D3D_FEATURE_LEVEL_11_0,  
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,

        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };
    do {

        if(!m_DXVA)
        {
#ifndef BUILD_MINWIN
            DriverType = D3D_DRIVER_TYPE_WARP;
#endif
            Log(_T("COMMENT  >>\t Trying to create D3D11 driver using SW , driver type is (0x%x)\n"), DriverType);
        }
        else
        {
            Log(_T("COMMENT  >>\t Trying to create D3D11 driver using HW , driver type is (0x%x)\n"), DriverType);
        }
        hr = D3D11CreateDevice(
            nullptr,
            DriverType,
            nullptr,
            D3D11_CREATE_DEVICE_VIDEO_SUPPORT | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            levels,
            ARRAYSIZE(levels),
            D3D11_SDK_VERSION,
            &m_pDX11Device,
            pFeatureLevel,
            &m_pDevContent
            );
        if(FAILED(hr))
        {
            Log(_T("ERROR>> Failed in D3D11CreateDevice , (hr=0x%x)\n"),hr);
            break;  
        }
        if(m_pDX11Device == NULL || m_pDevContent == NULL )
        {
            hr = E_FAIL;
            break;
        }
        hr = (m_pDX11Device)->QueryInterface(IID_PPV_ARGS(&pMultithread));
        if(FAILED(hr))
        {
            Log(_T("ERROR>> Failed in DX11Device Query interface , (hr=0x%x)\n"),hr);
            break;  
        }
        if(pMultithread == NULL)
        {
            hr = E_FAIL;
            break;
        }
        fResult = pMultithread->SetMultithreadProtected(TRUE);
        if(!fResult)
        {
            Log(_T("WARNING\t>>  Multithreading not turned on prior to  calling pMultithread->SetMultithreadProtected ,Method returned (hr=0x%d)\n"),fResult);
            break;
        }
    } while(FALSE);
    SAFERELEASE(pMultithread);
    return hr;
}
HRESULT CMediaEngineTest::CreateCompVisual(IDCompositionVisual **ppCompVisual)
{
    HRESULT         hr  = S_OK;

    do {

        hr = DCompositionCreateDevice(nullptr, __uuidof(IDCompositionDevice), reinterpret_cast<void**>(&m_pCompDevice));
        if(FAILED(hr))
        {
            Log(_T("ERROR>> Failed in DCompositionCreateDevice , (hr=0x%x)\n"),hr);
            break;  
        }

        if(m_pCompDevice == NULL)
        {
            hr = E_FAIL;
            break;
        }

        hr = CreatePlaybackWindow();

        if(FAILED(hr))
        {
            Log(_T("ERROR>> Failed in CreatePlaybackWindow , (hr=0x%x)\n"),hr);
            break;  
        }
        hr = m_pCompDevice->CreateTargetForHwnd(*m_pHwnd, true, &m_pCompTarget);
        if(FAILED(hr))
        {
            Log(_T("ERROR>> Failed in m_pCompDevice->CreateTargetForHwnd  , (hr=0x%x)\n"),hr);
            break;  
        }
        if(m_pCompTarget == NULL)
        {
            hr = E_FAIL;
            break;
        }
        hr = m_pCompDevice->CreateVisual(reinterpret_cast<IDCompositionVisual **>(&m_pCompVisual));
        if(FAILED(hr))
        {
            Log(_T("ERROR>> Failed in creating DComp Visual  , (hr=0x%x)\n"),hr);
            break;  
        }
        if(m_pCompVisual == NULL)
        {
            hr = E_FAIL;
            break;
        }
        hr = m_pCompTarget->SetRoot(m_pCompVisual);
        if(FAILED(hr))
        {
            Log(_T("ERROR>> Failed in pCompTarget->SetRoot  , (hr=0x%x)"),hr);
            break;  
        }
    } while (FALSE);

    return hr;
}

//CBaseTest
//Parses the input parameters string and initializes all the input paramters 
HRESULT CMediaEngineTest::InitializeInputParams(){

    HRESULT            hr           = S_OK;
    TCHAR              **argv       = NULL;
    TCHAR              **argvs      = NULL;
    TCHAR              *pszCmdLine  = NULL;
    int                argc         = 0;

    String  sContentSourcePath;

    srand((UINT)time(NULL));

    do
    {
        pszCmdLine = _tcsdup( m_pszInputParams );
        if( NULL == pszCmdLine )
        {
            hr = E_OUTOFMEMORY;
            break;
        }
        argv = CmdLine2Argv( pszCmdLine, &argc );
        argvs = argv;
        if( NULL == argv || 0 >= argc )
        {
            break;
        }
        while( argc-- )
        {
            if( **argv == '-' || **argv == '/' )
            {
                if ( 0 == _tcsicmp(*argv, _T("-input") ) )
                {
                    if( argc )
                    {
                        argc--;
                        argv++;

                        SAFEFREE(m_pszInputFile);

                        m_pszInputFile = (TCHAR*)malloc(sizeof(TCHAR)* MAX_STRING_LEN);
                        if (m_pszInputFile == NULL)
                        {
                            Log(_T("ERROR>> Out of memory, can't allocate input file name"));
                            hr = E_OUTOFMEMORY;
                            break;
                        }

                        if (_stprintf_s(m_pszInputFile, MAX_STRING_LEN, _T("%s"), *argv) <= 0)
                        {
                            Log(_T("ERROR>> Failed to copy input file name %s"), *argv);
                            hr = E_FAIL;
                        }
                    }
                    else
                    {
                        Log(L"-input must be followed by the input file");
                    }
                }
                else if (0 == _tcsicmp(*argv, _T("-MPO")))
                {
                    m_bMPOTest = TRUE;
                }                
                else if (0 == _tcsicmp(*argv, _T("-cstimeout")))
                {
                    m_bMPOCS = TRUE;

                    if( argc )
                    {
                        argc--;
                        argv++;

                        m_dwCSTimeout = _tstol(*argv);
                    }
                    else
                    {
                        Log(L"-cstimeout must be followed by the timeout in secs");
                        hr = E_FAIL;
                    }
                }
                else if (0 == _tcsicmp(*argv, _T("-csintervaltime")))
                {
                    m_bMPOCS = TRUE;

                    if( argc )
                    {
                        argc--;
                        argv++;
                        m_dwCSIntervalTime = _tstol(*argv);
                    }
                    else
                    {
                        Log(L"-csintervaltime must be followed by the interval in secs");
                        hr = E_FAIL;
                    }
                }
                else if (0 == _tcsicmp(*argv, _T("-rotate")))
                {
                    if(m_dwRotationCount + 1 > MAX_TEST_DEVICE_ROTATIONS)
                    {
                        Log(L"ERROR>> Cannot try more than %d rotations in test", MAX_TEST_DEVICE_ROTATIONS);
                        hr = E_FAIL;
                        break;
                    }

                    if( argc )
                    {
                        while( argc && (*(argv + 1))[0] != '-')
                        {
                            argc--;
                            argv++;

                            if (m_dwRotationCount + 1 > MAX_TEST_DEVICE_ROTATIONS)
                            {
                                Log(L"ERROR>> Cannot try more than %d rotations in test", MAX_TEST_DEVICE_ROTATIONS);
                                hr = E_FAIL;
                                break;
                            }
                            m_pdwRotationTarget[m_dwRotationCount] = _tstoi(*argv);

                            if(m_pdwRotationTarget[m_dwRotationCount] > 3)
                            {
                                Log(L"ERROR>> Must assign rotation of 0, 1, 2, or 3, '%d' not valid", m_pdwRotationTarget[m_dwRotationCount]);
                                hr = E_FAIL;
                                break;
                            }

                            m_dwRotationCount++;
                        }
                    }
                    else
                    {
                        Log(L"ERROR>> -rotate must be followed by the target rotation in degrees");
                        hr = E_FAIL;
                    }                    
                }
                else if (0 == _tcsicmp(*argv, _T("-rotateinterval")))
                {
                    if( argc )
                    {
                        argc--;
                        argv++;

                        m_dwRotationIntervalSeconds = _tstoi(*argv);
                    }
                    else
                    {
                        Log(L"ERROR>> -rotateinterval must be followed by the interval in seconds between rotations");
                        hr = E_FAIL;
                    }
                }
                else if(0 == _tcsicmp (*argv, _T("-dxva")))
                {
                    m_DXVA = TRUE;
                }                
                else if(0 == _tcsicmp (*argv, _T("-soft")))
                {
                    m_DXVA = FALSE;                
                }
                else if(0 == _tcsicmp (*argv, _T("-DCOMP")))
                {
                    m_Mode = Playback_DComp;
                }
                else if(0 == _tcsicmp (*argv, _T("-HWND")))
                {
                    m_Mode = Playback_HWND;
                }
                else if(0 == _tcsicmp (*argv, _T("-FrameServer")))
                {
                    m_Mode = Playback_FrameServer;
                }
                else if (0 == _tcsicmp(*argv, _T("-NoDumpFrame")))
                {
                    m_bDumpFrame = FALSE;
                }
                else if(0 == _tcsicmp (*argv, _T("-protectedPlay")))
                {
                    m_protectedPlay = TRUE;
                }
                else if(0 == _tcsicmp (*argv, _T("-frames"))){

                    if( argc ){
                        argc--;
                        argv++;
                        m_dwFrames = _tstoi(*argv);
                    } else {
                        hr = E_FAIL;
                    }
                }
                else if (0 == _tcsicmp(*argv, _T("-frameInterval"))){

                    if (argc){
                        argc--;
                        argv++;
                        m_dwFrameInterval = _tstoi(*argv);
                    }
                    else {
                        hr = E_FAIL;
                    }
                }

                else if (0 == _tcsicmp(*argv, _T("-visval"))){
                    m_fVisVal = TRUE;
                }
                else if(0 == _tcsicmp (*argv, _T("-tout")))
                {
                    if( argc )
                    {
                        argc--;
                        argv++;
                        m_dwTimeout = _tstol(*argv);
                    }
                    else
                    {
                        hr = E_FAIL;
                    }
                }
                else if(0 == _tcsicmp (*argv, _T("-keepFiles")))
                {
                    m_bKeepFiles = true;
                }
                else if(0 == _tcsicmp(*argv, _T("-collectETW")))
                {
                    if( argc )
                    {
                        argc--;
                        argv++;

                        m_pszETWFileName = _tcsdup(*argv);
                        if(m_pszETWFileName == NULL)
                        {
                            hr = E_OUTOFMEMORY;
                            break;
                        }

                        m_bUseCpkETW = TRUE;
                    }
                    else
                    {
                        hr = E_FAIL;
                        Log(_T("ERROR>> -collectETW must be followed by the file name\n"));
                        break;
                    }
                }
                else if(0 == _tcsicmp(*argv, _T("-etwLevel")))
                {
                    if( argc )
                    {
                        argc--;
                        argv++;

                        hr = SetETWLevel(*argv);
                        if(FAILED(hr))
                        {
                            break;
                        }
                    }
                    else
                    {
                        hr = E_FAIL;
                        Log(_T("ERROR>> -etwLevel must be followed by the ETW level\n"));
                        break;
                    }
                }
                else if(0 == _tcsicmp(*argv, _T("-ETWFilters")))
                {
                    if(argc)
                    {
                        argc--;
                        argv++;

                        if(m_pszETWFilters == NULL)
                        {
                            m_pszETWFilters = _tcsdup(*argv);
                            if(m_pszETWFilters == NULL)
                            {
                                hr = E_OUTOFMEMORY;
                                break;
                            }
                        }
                        else
                        {
                            DWORD dwETWFiltersLen = _tcslen(m_pszETWFilters) + _tcslen(*argv) + 2;
                            TCHAR* pszETWFilters = (TCHAR*)malloc(sizeof(TCHAR) * dwETWFiltersLen);
                            if(pszETWFilters == NULL)
                            {
                                hr = E_OUTOFMEMORY;
                                break;
                            }

                            _stprintf_s(pszETWFilters, dwETWFiltersLen, _T("%s,%s"), m_pszETWFilters, *argv);

                            free(m_pszETWFilters);

                            m_pszETWFilters = pszETWFilters;
                        }
                    }
                    else
                    {
                        hr = E_FAIL;
                        Log(_T("ERROR>> -ETWFilters must be followed by the event list"));
                        break;
                    }
                }
                else if(0 == _tcsicmp(*argv, _T("-realTimeETWFilters")))
                {
                    if(argc)
                    {
                        argc--;
                        argv++;

                        m_bRealTimeConsumer = TRUE;

                        if(m_pszETWFilters == NULL)
                        {
                            m_pszETWFilters = _tcsdup(*argv);
                            if(m_pszETWFilters == NULL)
                            {
                                hr = E_OUTOFMEMORY;
                                break;
                            }
                        }
                        else
                        {
                            DWORD dwETWFiltersLen = _tcslen(m_pszETWFilters) + _tcslen(*argv) + 2;
                            TCHAR* pszETWFilters = (TCHAR*)malloc(sizeof(TCHAR) * dwETWFiltersLen);
                            if(pszETWFilters == NULL)
                            {
                                hr = E_OUTOFMEMORY;
                                break;
                            }

                            _stprintf_s(pszETWFilters, dwETWFiltersLen, _T("%s,%s"), m_pszETWFilters, *argv);

                            free(m_pszETWFilters);

                            m_pszETWFilters = pszETWFilters;
                        }
                    }
                    else
                    {
                        hr = E_FAIL;
                        Log(_T("ERROR>> -realTimeETWFilters must be followed by the event list"));
                        break;
                    }
                }                
                else if (0 == _tcsicmp(*argv, _T("-VisValMPO")))
                {
                    m_Mode = Playback_MPOVV;
                    m_bFullScreen = true;
                }
                else if (0 == _tcsicmp(*argv, _T("-VisValMPODXVAHD")))
                {
                    m_Mode = Playback_MPOVV_DXVAHD;
                    m_bFullScreen = true;
                }
                else if (0 == _tcsicmp(*argv, _T("-VisValMPOSoft")))
                {
                    m_Mode = Playback_MPOVV_Soft;
                    m_bFullScreen = true;
                }
                else if (0 == _tcsicmp(*argv, _T("-PSNR")))
                {
                    if (argc)
                    {
                        argc--;
                        argv++;

                        m_dwPSNRThreshold = _tstol(*argv);
                    }
                    else
                    {
                        hr = E_FAIL;
                        Log(_T("ERROR>> -PSNR must be followed by the PSNR threshold for MPO capture comparison"));
                        break;
                    }
                }

                argv++;
            }
            else
            {
                argv++;
            }

            if( S_OK != hr )
            {
                break;
            }
        }

        //ContentSource
        if (SUCCEEDED(RuntimeParameters::TryGetValue(
            L"ContentSource",
            sContentSourcePath)))
        {
            m_pwszContentSourcePath = _wcsdup(
                sContentSourcePath
                );
            if (m_pwszContentSourcePath == NULL)
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            g_pwszHCKContentFileSearchPath = m_pwszContentSourcePath;

            hr = FindHCKContentFile(
                (const WCHAR*) m_pszInputFile,
                &m_pwszContentFileName
                );
            if (FAILED(hr))
            {
                Log(_T("ERROR>> Failed to find input source file '%s' (hr=0x%x)"), (const WCHAR*) m_pszInputFile, hr);
                // Don't attempt to delete the test file as it doesn't exist.
                m_bKeepFiles = true;

                Log(_T("ERROR>> Ensure that the HLK Supplemental Media Content folders (DXVAContent, HMFTContent etc) have been copied to 'C:\\Program Files (x86)\\Windows Kits\\10\\Hardware Lab Kit\\Tests\\' on the HLK controller."));
                
                if (FAILED(CheckContentShareAccess()))
                {
                    wchar_t currentUserName[UNLEN + 1];
                    DWORD currentUserNameLength = UNLEN + 1;

                    if (GetUserNameEx(NameSamCompatible, currentUserName, &currentUserNameLength))
                    {
                        Log(_T("ERROR>> Can't access content share at: '%s'. Ensure that the network configuration on the HLK client/controller allows the user '%s' on the client to access the share."), (const WCHAR*)g_pwszHCKContentFileSearchPath, (const WCHAR*)currentUserName);
                    }
                    else
                    {
                        Log(_T("ERROR>> Can't access content share at: '%s'. Ensure that the network configuration on the HLK client/controller allows the client to access the share."), (const WCHAR*)g_pwszHCKContentFileSearchPath);
                    }
                }
                else
                {
                    Log(_T("ERROR>> Content share ('%s') is accessible, but the content file ('%s') was not found at that location."), (const WCHAR*)g_pwszHCKContentFileSearchPath, (const WCHAR*)m_pszInputFile);
                }

                break;
            }

            SAFEFREE(m_pszInputFile);
            m_pszInputFile = _wcsdup(m_pwszContentFileName);
        }

        Log(_T("INFO>> Input File name = %s"), m_pszInputFile);

    }while (FALSE);

    if (SUCCEEDED(hr))
    {
        if (m_DXVA)
        {
            Log(L"INFO>> Using DXVA (-dxva)");
        }
        else
        {
            Log(L"INFO>> Using software (WARP)");
        }

        if (m_Mode == Playback_HWND)
        {
            Log(L"INFO>> Using HWND (-HWND)");
        }
        if (m_Mode == Playback_DComp)
        {
            Log(L"INFO>> Using DCOMP (-DCOMP)");
        }
    }

    SAFEDELETE( argvs );
    SAFEFREE  ( pszCmdLine );

    return hr;
}
HRESULT CMediaEngineTest::CTBSetupTest(void)
{
    HRESULT hr = S_OK;

    do
    {
        hr = InitializeInputParams();
        if(hr != S_OK)
        {
            Log(_T("ERROR>> Failed in CTBSetupTest  , (hr=0x%x)"),hr);
            break;
        }
    }while(false);

    return hr;
}
HRESULT CMediaEngineTest::CTBExecuteTest(void)
{
    HRESULT hr = S_OK;
    bool bExecuteTest = false;
    do
    {
#ifndef BUILD_MINWIN
        bExecuteTest = (m_bMPOTest == false) || ((m_bMPOTest == true) && (m_isMPOCapable != notMPOCapable))? true : false;
#else
        bExecuteTest = TRUE;
#endif
        if(bExecuteTest)
        {
            m_hPBCompleted = CreateEvent(nullptr, FALSE, FALSE, nullptr);
            if (nullptr == m_hPBCompleted) 
            {
                hr = HRESULT_FROM_WIN32(GetLastError());
                Log(_T("ERROR>> Failed in creating signal event, (hr=0x%x)"),hr);
                break;
            }        

            if (m_Mode == Playback_FrameServer)
            {
                m_hFrameStepCompleted = CreateEvent(nullptr, FALSE, FALSE, nullptr);
                if (nullptr == m_hFrameStepCompleted)
                {
                    hr = HRESULT_FROM_WIN32(GetLastError());
                    Log(_T("ERROR>> Failed in creating frame step completion signal event, (hr=0x%x)"), hr);
                    break;
                }
            }

            hr = InitializeMediaEngine();

            if(FAILED(hr))
            {
                Log(_T("ERROR>> Failed in InitializeMediaEngine  , (hr=0x%x)"),hr);
                break;  
            }

            GetOutputFile(m_pszTempOutFile, MAX_PATH);
            hr = GenerateNewFileName(m_fD3DAWARE);
            if (FAILED(hr))
            {
                Log(_T("ERROR  >>\t Failed in generating new file name for dumping yuv data"));
                break;
            }
        }
        else
        {
            Log(_T("INFO>> Cannot execute MPO test on system which doesnt support MPO"));
            hr = E_ABORT;
        }

        //Wait for timer thread to complete in frameserver mode
        if (m_Mode == Playback_FrameServer)
        {
            HANDLE handles[2];
            handles[0] = m_hPBCompleted;
            handles[1] = m_hFrameStepCompleted;

            WaitForMultipleObjects(2, handles, TRUE, INFINITE);
        }
        else
        {
            WaitForSingleObject(m_hPBCompleted, INFINITE);
        }
        
        hr = m_pMediaEngine->Shutdown();
        if (FAILED(hr))
        {
            Log(L"\n Failed in Shutdown(),%x\n", hr);
        }

        hr = m_hr;

    }while(false);
    Log(_T("INFO   >>\tExecuting test completed. Final error is 0x%x\n"),hr);
    return hr;
}

HRESULT CMediaEngineTest::OnLoaded()
{
    HRESULT hr = S_OK;
    HANDLE hCompSurf = NULL;
    IUnknown *pSurfHandle = NULL;
    RECT rc;

    do
    {
        if(m_Mode == Playback_DComp)
        {
            #ifdef BUILD_MINWIN
                // Test is fullscreen on phone.
                rc.right = GetSystemMetrics(SM_CXSCREEN);
                rc.bottom = GetSystemMetrics(SM_CYSCREEN);
                rc.left = 0;
                rc.top = 0;
            #else
                if (!GetClientRect(*m_pHwnd, &rc))
                {
                    hr = E_FAIL;
                    Log(_T("ERROR>> Failed in getting rectangle for the playback window, (hr=0x%x)"), hr);
                    break;
                }
            #endif

            if(m_pMediaEngineEx != NULL && m_pCompDevice != NULL)
            {
                if(m_protectedPlay)
                {
                    hr = m_pMediaEngineEx->EnableWindowlessSwapchainMode(TRUE);
                    if(FAILED(hr))
                    {
                        Log(_T("ERROR>> Failed to EnableWindowlessSwapchainMode : (hr=0x%x) "),  m_hr);
                        break;
                    }
                }

                hr = m_pMediaEngineEx->UpdateVideoStream(NULL, &rc, NULL);
                if(FAILED(hr))
                {
                    Log(_T("ERROR>> Failed while setting the video size on Mediaengine : (hr=0x%x) "),  m_hr);
                    break;
                }

                if(m_protectedPlay)
                {                        
                    hr = m_pMediaEngineEx->GetVideoSwapchainHandle(&hCompSurf);
                    if(FAILED(hr))
                    {
                        Log(_T("ERROR>> Failed to GetVideoSwapchainHandle : (hr=0x%x) "),  m_hr);
                        break;
                    }

                    hr = m_pCompDevice->CreateSurfaceFromHandle(hCompSurf, &pSurfHandle);
                    if(FAILED(hr))
                    {
                        Log(_T("ERROR>> Failed to CreateSurfaceFromHandle : (hr=0x%x) "),  m_hr);
                        break;
                    }

                    hr = m_pCompVisual->SetContent(pSurfHandle);
                    if(FAILED(hr))
                    {
                        Log(_T("ERROR>> Failed to SetContent : (hr=0x%x) "),  m_hr);
                        break;
                    }
                }                    

                hr = m_pCompDevice->Commit();
                if(FAILED(hr))
                {
                    Log(_T("ERROR>> Failed in m_pCompDevice->Commit : (hr=0x%x) "),  m_hr);
                    break;
                }                    
            }
        }
    } while(FALSE);
     
    if(NULL != hCompSurf)
    {
        CloseHandle(hCompSurf);
    }

    SAFERELEASE(pSurfHandle);

    return hr;
}

HRESULT CMediaEngineTest::OnCanPlay()
{
    HRESULT hr = S_OK;

    do
    {
        if(m_Mode != Playback_FrameServer)
        {
            hr = m_pMediaEngine->Play();
            if (FAILED(hr))
            {
                m_hr = hr;
                Log(_T("ERROR  >>\t MediaEngine Failed in Starting Playback  , (hr=0x%x)\n"), hr);
                break;
            }


            if ((m_bMPOTest || m_bHWDRMEnabled) && !m_bETWInitialized)
            {
                hr = InitializeETW();
                if (FAILED(hr))
                {
                    m_hr = hr;
                    Log(L"Failed in InitializeETW ");
                    break;
                }
            }
            Log(_T("INFO>>Playback Started"));
        }

    }while(FALSE);

    return hr;
}

HRESULT CMediaEngineTest::OnFirstFrameReady()
{
    if (m_Mode == Playback_FrameServer)
    {
        if (m_pMediaEngine->HasVideo() && m_fStopFrameStep)
        {
            StartFrameStep();
        }
    }

    return S_OK;
}


HRESULT CMediaEngineTest::OnEnded()
{
    HRESULT hr = S_OK;

    if(m_bMPOTest && (m_isMPOCapable != notMPOCapable))
    {
        if(0 == m_ullLastCount)
        {
            Log(L"ERROR>> No MPO ETW events found");
            hr = E_FAIL;
            m_hr = E_FAIL;
        }
    }

    if (m_bHWDRMEnabled)
    {
        if (m_pDRMStatusCallback != NULL)
        {
            ULONG numDecryptCalls;
            m_pDRMStatusCallback->GetStats(&hr, &numDecryptCalls);
            m_hr = hr;

            if (FAILED(hr))
            {
                Log(_T("ERROR>> HW DRM function call failed, (hr=0x%x)"), hr);
            }
            else if (numDecryptCalls == 0)
            {
                Log(L"ERROR>> No HW DRM decrypt call ETW events found");
                hr = E_FAIL;
                m_hr = E_FAIL;
            }
        }
    }

    Log(_T("INFO>>Playback Completed"));

    SignalEndTest();

    return hr;
}

HRESULT CMediaEngineTest::OnError()
{
    HRESULT hr = S_OK;

    if (m_hr != MF_E_TOPO_CODEC_NOT_FOUND)
    {
        Log(_T("ERROR>> ME ERROR: (hr=0x%x) "), m_hr);
    }
    else
    {
        Log(_T("INFO>> Test skipped because codec is not found "));
        m_hr = S_OK;
    }

    SignalEndTest();
    return hr;
}

HRESULT CMediaEngineTest::OnSeekEvent()
{
    HRESULT hr = S_OK;

    do {
        if(!SetEvent(m_hSeekCompleted)) {
            hr = HRESULT_FROM_WIN32(GetLastError());
            Log(_T("ERROR  >>\t Could not set seek completed event, (hr=0x%x)\n"),hr);
            break;
        }
    } while(false);

    return hr;
}
// Get the current playback position.
double CMediaEngineTest::GetPlaybackPosition()
{
    if (m_pMediaEngine)
    {
        return m_pMediaEngine->GetCurrentTime();
    }
    else
    {
        return 0;
    }
}

// Seek to a new playback position.
HRESULT CMediaEngineTest::SetPlaybackPosition(double pos)
{
    HRESULT hr = S_OK;

    if (m_pMediaEngine)
    {
        hr = m_pMediaEngine->SetCurrentTime(pos);
    }

    return hr;
}


HRESULT CMediaEngineTest::SignalEndTest()
{	
    HRESULT hr = S_OK;



    // Do not delete callbacks, they are deleted by the consumer
    if (m_bMPOTest || m_bHWDRMEnabled)
    {
        m_etwController.StopSession();
        StopUpdateThread();
        m_bETWInitialized = FALSE;
    }

    if (m_Mode == Playback_FrameServer)
    {
        StopFrameStep();
    }

    if(m_hPBCompleted)
    {
        SetEvent(m_hPBCompleted);	
    }


    return hr;
}

HRESULT SetPropString(LPCWSTR pszVal, PROPVARIANT * pv)
{
    pv->vt = VT_LPWSTR;

    if(pszVal)
    {
        long cch = wcslen(pszVal)+1;
        pv->pwszVal = (LPWSTR)CoTaskMemAlloc(cch * sizeof(WCHAR));
        if(pv->pwszVal == NULL)
        {
            return E_OUTOFMEMORY;
        }
        CopyMemory(pv->pwszVal, pszVal, cch*sizeof(WCHAR));
    }
    else
    {
        pv->pwszVal = NULL;
    }
    return S_OK;
}

HRESULT CMediaEngineTest::SetDRMAttributes(IPropertyStore **pSourcePS)
{

    PROPVARIANT propvarsysid  = {0};
    PROPVARIANT propvarcid  = {0};
    PROPVARIANT propvarclassid  = {0};
    PROPVARIANT propvarpkgid  = {0};
    HRESULT hr = S_OK;

    //CLSID systemID = {0xF4637010,0x03C3,0x42CD,{0xB9,0x32,0xB4,0x8A,0xDF,0x3A,0x6A,0x54}}; //CLSID_PlayReadySystemID
    CLSID systemID = {0x9443b73f, 0xaadd, 0x43d4, {0xbe, 0xe8, 0x97, 0xfa, 0x8d, 0x7c, 0xf6, 0x4f}};//CLSID_PlayReadySystemIDExtended
    GUID containerGuid = {0x9A04F079,0x9840,0x4286,{0xAB,0x92,0xE6,0x5B,0xE0,0x88,0x5F,0x95}}; 	
    //  Media Protection SystemId (required, existing API)
    //  Using the PlayReady GUID

    PropVariantInit(&propvarsysid);
    PropVariantInit(&propvarcid);
    PropVariantInit(&propvarclassid);
    PropVariantInit(&propvarpkgid);

    do{
        propvarsysid.vt     = VT_CLSID;
        propvarsysid.puuid  = (CLSID*)CoTaskMemAlloc(sizeof(CLSID));
        if(propvarsysid.puuid == NULL)
        {
            hr = E_OUTOFMEMORY;
            Log(L"Failed to allocate GUID for MFPKEY_MediaProtectionSystemId");
            break;
        }

        memcpy_s(
            propvarsysid.puuid,
            sizeof(CLSID),
            &systemID,
            sizeof(systemID)
            );

        hr =  PSCreateMemoryPropertyStore( IID_PPV_ARGS( pSourcePS ) ) ;
        if(FAILED(hr))
        {
            Log(L"Failed To Query IPropertyStore (hr=0x%x) interface from Source", hr);
            break;  
        }

        hr = (*pSourcePS)->SetValue( MFPKEY_MediaProtectionSystemId, propvarsysid ) ;
        if(FAILED(hr))
        {
            Log(L"Failed To set MediaProtection System ID (hr=0x%x)", hr);
            break;  
        }

        propvarcid.vt       = VT_CLSID;
        propvarcid.puuid    = (CLSID*)CoTaskMemAlloc(sizeof(CLSID));
        if(propvarcid.puuid == NULL)
        {
            hr = E_OUTOFMEMORY;
            Log(L"Failed to allocate GUID for MFPKEY_MediaProtectionContainerGuid");
            break;
        }

        memcpy_s(
            propvarcid.puuid,
            sizeof(CLSID),
            &containerGuid,
            sizeof(containerGuid)
            );

        hr =  (*pSourcePS)->SetValue( MFPKEY_MediaProtectionContainerGuid, propvarcid ) ;
        if(FAILED(hr))
        {
            Log(L"Failed to set MFPKEY_MediaProtectionContainerGuid (hr=0x%x)", hr);
            break;  
        }

        SetPropString(L"Windows.Media.Protection.PlayReady.PlayReadyWinRTTrustedInput", &propvarclassid);
        hr =  (*pSourcePS)->SetValue( MFPKEY_MediaProtectionClassName, propvarclassid ) ;

        if(FAILED(hr))
        {
            Log(L"Failed to set MFPKEY_MediaProtectionClassName (hr=0x%x)", hr);
            break;  
        }

    }while(FALSE);

    PropVariantClear(&propvarsysid);
    PropVariantClear(&propvarcid);
    PropVariantClear(&propvarclassid);
    PropVariantClear(&propvarpkgid);

    return hr;
}

CMediaEngineTestSampleCallbackMFT::CMediaEngineTestSampleCallbackMFT()
{
}

CMediaEngineTestSampleCallbackMFT::~CMediaEngineTestSampleCallbackMFT()
{
}

HRESULT CMediaEngineTestSampleCallbackMFT::CheckMediaType(
    IMFMediaType*   pMT
    )
{
    HRESULT hr = S_OK;
    GUID subType = GUID_NULL;

    do
    {
        if(pMT == NULL)
        {
            hr = E_INVALIDARG;
            break;
        }

        hr = pMT->GetGUID(MF_MT_SUBTYPE, &subType);
        if(FAILED(hr))
        {
            break;
        }

        //support uncompressed media types
        if((subType != MFVideoFormat_RGB8) 
            && (subType != MFVideoFormat_RGB565) 
            && (subType != MFVideoFormat_RGB555) 
            && (subType != MFVideoFormat_RGB24) 
            && (subType != MFVideoFormat_RGB32) 
            && (subType != MFVideoFormat_I420) 
            && (subType != MFVideoFormat_IYUV) 
            && (subType != MFVideoFormat_YV12) 
            && (subType != MFVideoFormat_YUY2) 
            && (subType != MFVideoFormat_UYVY) 
            && (subType != MFVideoFormat_YVYU) 
            && (subType != MFVideoFormat_NV12) 
            && (subType != MFVideoFormat_420O)) 
        {
            hr = MF_E_INVALIDMEDIATYPE;
            break;
        }


    }while(FALSE);

    return hr;
}

HRESULT CMediaEngineTestSampleCallbackMFT::GetAttributes(
    IMFAttributes** ppAttributes)
{
    HRESULT hr = S_OK;

    IMFAttributes *pAttributes = NULL;
    const UINT32 cElements = 3;  // Starting size.

    do
    {
        // Create the empty attribute store.
        hr = MFCreateAttributes(&pAttributes, cElements);
        if(FAILED(hr))
        {
            break;
        }

        // Set the MF_SA_D3D_AWARE attribute
        hr = pAttributes->SetUINT32(
            MF_SA_D3D_AWARE,
            TRUE
            );
        if(FAILED(hr))
        {
            break;
        }

        // Set the MF_SA_D3D11_AWARE attribute
        hr = pAttributes->SetUINT32(
            MF_SA_D3D11_AWARE,
            TRUE
            );
        if(FAILED(hr))
        {
            break;
        }

        // Set the MF_SA_REQUIRED_SAMPLE_COUNT attribute
        hr = pAttributes->SetUINT32(
            MF_SA_REQUIRED_SAMPLE_COUNT,
            7
            );
        if(FAILED(hr))
        {
            break;
        }

        *ppAttributes = pAttributes;
        (*ppAttributes)->AddRef();    

    }while(FALSE);

    SAFERELEASE(pAttributes);

    return hr;
}

HRESULT CMediaEngineTestSampleCallbackMFT::GetInputAvailableType(
    DWORD           dwInputStreamID,
    DWORD           dwTypeIndex,
    IMFMediaType**  ppType
    )
{
    HRESULT hr = S_OK;
    MFSampleCallback::CAutoLock   lock(&m_csLock);

    //support uncompressed media types
    GUID uncompressedSubtyes[] = {
        MFVideoFormat_420O,
        MFVideoFormat_NV12,
        MFVideoFormat_RGB8,
        MFVideoFormat_RGB565, 
        MFVideoFormat_RGB555, 
        MFVideoFormat_RGB24, 
        MFVideoFormat_RGB32, 
        MFVideoFormat_I420, 
        MFVideoFormat_IYUV, 
        MFVideoFormat_YV12, 
        MFVideoFormat_YUY2, 
        MFVideoFormat_UYVY, 
        MFVideoFormat_YVYU,
    };

    DWORD numTypes = sizeof(uncompressedSubtyes)/sizeof(GUID) + 1;

    do
    {
        if(ppType == NULL)
        {
            hr = E_POINTER;
            break;
        }

        if(dwInputStreamID > 0)
        {
            hr = MF_E_INVALIDSTREAMNUMBER;
            break;
        }

        if(dwTypeIndex > numTypes)
        {
            hr = MF_E_NO_MORE_TYPES;
            break;
        }

        hr = MFCreateMediaType(
            ppType
            );
        if(FAILED(hr))
        {
            break;
        }

        if(m_pOutputMT == NULL)
        {
            hr = MFCreateMediaType(
                ppType
                );
            if(FAILED(hr))
            {
                hr = MF_E_NO_MORE_TYPES;
                break;
            }

            hr = (*ppType)->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Video);
            if (FAILED(hr))
            {
                hr = MF_E_NO_MORE_TYPES;
                break;
            }



            hr = (*ppType)->SetGUID(MF_MT_SUBTYPE, uncompressedSubtyes[dwTypeIndex]);
            if (FAILED(hr))
            {
                hr = MF_E_NO_MORE_TYPES;
                break;
            }
        }
        else
        {
            hr = m_pOutputMT->CopyAllItems(*ppType);
            if(FAILED(hr))
            {
                break;
            }
        }				
    }while(false);

    return hr;
}

HRESULT CMediaEngineTest::CreateTexture(ID3D11Device *ppD3D11Device, ID3D11Texture2D **ppTexture2D,BOOL bStaging, BOOL bRead)
{
    HRESULT hr = S_OK;
    D3D11_TEXTURE2D_DESC textureDesc = {0};
    DWORD dwWidth = 0;
    DWORD  dwHeight = 0;

    do 
    {
        hr =  m_pMediaEngine->GetNativeVideoSize(&dwWidth,&dwHeight); //get again...
        if(FAILED(hr))
        {
            Log(L"\n Failed to get Native Size of the video and Heights and widths are not defined\n");
            break;
        }

        m_rcTarget.right = dwWidth;
        m_rcTarget.left = 0;
        m_rcTarget.top = 0;
        m_rcTarget.bottom = dwHeight;

        if(textureDesc.Width==0  || textureDesc.Height==0 || dwWidth!=textureDesc.Width || dwHeight!=textureDesc.Height)
        {
            textureDesc.Width = dwWidth;
            textureDesc.Height = dwHeight;

        }

        textureDesc.MipLevels = 1;
        textureDesc.ArraySize = 1;
        textureDesc.Format =  DXGI_FORMAT_B8G8R8A8_UNORM;              
        textureDesc.MiscFlags = D3D11_RESOURCE_MISC_SHARED;  //NEEDED TO HAVE SHARED RESOURCE BETWEEN MEDIA ENGINE AND D3D
        textureDesc.SampleDesc.Count = 1;
        textureDesc.SampleDesc.Quality =  0;
        if(!bRead) //if its render target
        {
            textureDesc.CPUAccessFlags = 0;     
        }
        else 
        {
            textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
        }
        if(!bStaging)
        {
            textureDesc.Usage =     D3D11_USAGE_DEFAULT;
            textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
        }
        else
        {
            textureDesc.Usage = D3D11_USAGE_STAGING;
        }  

        hr = ppD3D11Device->CreateTexture2D(&textureDesc, NULL, ppTexture2D);
        if(FAILED(hr))
        {
            Log(L"\n Texture Creation failed, width and heights are %d and %d \n",textureDesc.Width,textureDesc.Height);
            break;
        }  

    } while(false);

    return hr;
}
HRESULT CMediaEngineTest::StartFrameStep()
{
    CComPtr<IDXGIFactory1> spFactory;
    CComPtr<IDXGIAdapter> spAdapter;
    HRESULT hr = S_OK;
    DWORD ThreadID;

    do{
        if (m_FrameStepThreadHandle != NULL)
        {
            Log(L"\n Failed in start timer err because handle is not null\n");   
            hr = E_FAIL;
            break;
        }

        hr = CreateDXGIFactory1(IID_PPV_ARGS(&spFactory));
        if(FAILED(hr))
        {
            Log(L"\n Failed in start timer err because faield to created dxgi factory,%x\n",hr); 
            break;
        } 

        hr =  spFactory->EnumAdapters(0, &spAdapter);
        if(FAILED(hr))
        {
            Log(L"\n Failed in start timer err because enum adapters,%x\n",hr); 
            break;
        }

        hr = spAdapter->EnumOutputs(0, &m_spDXGIOutput);
        if(FAILED(hr))
        {
            Log(L"\n Failed in start timer Output adapters failed,%x\n",hr); 
            break;
        }

        m_fStopFrameStep = FALSE;

        m_FrameStepThreadHandle = CreateThread(nullptr, 0, FrameStepThread, this, 0, &ThreadID);
        if (m_FrameStepThreadHandle == NULL)
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            Log(L"\n Failed in start frame step, thread handle is null,%x\n",hr); 
            break; 
        }

    }while(false); 

    return hr;
} 

HRESULT CMediaEngineTest::StopFrameStep()
{
    m_fStopFrameStep = TRUE;
    m_fPlaying = FALSE;

    //StartFrameStep was never called. 
    if(m_FrameStepThreadHandle == NULL)
    {
        SetEvent(m_hFrameStepCompleted);
    }
        
    return S_OK;
}

DWORD WINAPI FrameStepThread(LPVOID lpParameter)
{

    CMediaEngineTest *pThis = reinterpret_cast<CMediaEngineTest *>(lpParameter);

    SetThreadPriority(GetCurrentThread(),THREAD_PRIORITY_ABOVE_NORMAL); 

    pThis->FrameStep();

    return 0;
}


void CMediaEngineTest::FrameStep(void)
{
    HRESULT hr = S_OK;
    LONGLONG pts = 0;
    int totalSleepTime = 0;
    
    for ( ;; )
    {
        if (m_fStopFrameStep)
        {
            break;
        }

        if (!m_pMediaEngine->IsEnded())
        {
            hr = m_pMediaEngine->OnVideoStreamTick(&pts);
            while ((hr != S_OK) && (totalSleepTime < TOTAL_SLEEP_TIMEOUT))
            {
                //it is possible that MF_MEDIA_ENGINE_EVENT_ENDED gets called after FrameStep.
                if (m_pMediaEngine->IsEnded())
                {
                    hr = S_OK;
                    break;
                }

                // Give framestep time to complete
                Sleep(50);
                totalSleepTime += 50;

                hr = m_pMediaEngine->OnVideoStreamTick(&pts);
            }
            

            if (hr != S_OK)
            {
                hr = E_FAIL;
                Log(_T("ERROR  >>\t m_pMediaEngineEx->OnVideoStreamTick did not return S_OK in the past 5 mins, (hr=0x%x)\n"), hr);
                SignalEndTest();
                break;
            }

#if 0
            Log(L"presentation time of the frame %ld\n", pts);
#endif

            hr = OnTransferFrame();
            if (SUCCEEDED(hr))
            {
                m_nFrames++;
            }
            else
            {
                SignalEndTest();
                break;
            }
           
            hr = m_pMediaEngineEx->FrameStep(TRUE);
            if (FAILED(hr))
            {
                Log(_T("ERROR  >>\t m_pMediaEngineEx->FrameStep failed, (hr=0x%x)\n"), hr);
                break;
            }
        }


        if (m_nFrames > m_dwFrames)
        {
            SignalEndTest();
            break;
        }

        totalSleepTime = 0;
    }

    m_hr = hr;
    SetEvent(m_hFrameStepCompleted);

    return;
}


HRESULT CMediaEngineTest::OnTransferFrame()
{
    HRESULT hr = S_OK;
    ID3D11DeviceContext *pD3DImmediateContext = nullptr;
    ID3D11Texture2D  *pD3D11Texture2D = NULL;
    ID3D11Texture2D  *pD3D11StagingTexture2D = NULL;
    RECT rcTarget;
    D3D11_TEXTURE2D_DESC Desc = { 0 };
    DWORD dwWidth;

    MFVideoNormalizedRect rect;
    rect.bottom = 1.0;
    rect.left = 0.0;

    do
    {
        if (m_pMediaEngine != nullptr)
        {
            hr = CreateTexture(m_pDX11Device, &pD3D11Texture2D, FALSE, FALSE); //Render target 
            if (FAILED(hr))
            {
                Log(L"\n Create Texture Failed for OnTransferFrame %x and file count is %d\n", hr, m_dwFileCount);
                break;
            }

            hr = CreateTexture(m_pDX11Device, &pD3D11StagingTexture2D, TRUE, TRUE); //Staging
            if (FAILED(hr))
            {
                Log(L"\n Create  Staging Texture Failed for OnTransferFrame %x and file count is %d\n", hr, m_dwFileCount);
                break;
            }

            hr = m_pMediaEngine->TransferVideoFrame(pD3D11Texture2D, nullptr, &m_rcTarget, nullptr);
            if (FAILED(hr))
            {
                Log(L"\n Failed in Tranferring the frame %x\n", hr);
                break;
            }

            if (m_bDumpFrame)
            {

                pD3D11Texture2D->GetDesc(&Desc);

                D3D11_MAPPED_SUBRESOURCE lockedRect = { 0 };
                DWORD dwFlags = 0;
                D3D11_MAP MapType = D3D11_MAP_READ;
                m_pDX11Device->GetImmediateContext(&pD3DImmediateContext);
                if (NULL == pD3DImmediateContext)
                {
                    Log(L"\n Immidiate contenxt is null\n");
                    break;
                }

                pD3DImmediateContext->CopyResource(pD3D11StagingTexture2D, pD3D11Texture2D);
                pD3DImmediateContext->Flush();

                hr = pD3DImmediateContext->Map(pD3D11StagingTexture2D, 0, MapType, dwFlags, &lockedRect);
                if (FAILED(hr))
                {
                    Log(L"\n mapping failed %x\n", hr);
                    break;
                }

                unsigned char *pData = (unsigned char*)lockedRect.pData;
                DWORD dwPitch = lockedRect.RowPitch;
                DWORD dwHeight = Desc.Height;
                DX_FORMAT subtype = FORMAT_ARGB32;

                switch (Desc.Format)
                {
                case DXGI_FORMAT_R8G8B8A8_UNORM:
                    subtype = FORMAT_RGB32;
                    break;
                case DXGI_FORMAT_B8G8R8A8_UNORM:
                    subtype = FORMAT_ARGB32;
                    break;
                default:
                    hr = E_INVALIDARG;
                }

                if (SUCCEEDED(hr))
                {
                    if (m_dwFrameInterval == 0 || (m_nFrames % m_dwFrameInterval) == 0)
                    {
                        hr = WriteDXVAFrameToFile(pData, dwPitch, dwHeight, subtype);
                        m_ComparisonInfo.dwHeight = dwHeight;
                        m_ComparisonInfo.dwWidth = dwPitch / 4; //4 bytes of data per pixel.
                        m_ComparisonInfo.subType = MEDIASUBTYPE_ARGB32;
                    }
                }

                if (FAILED(hr))
                {
                    Log(L"Writing of frames failed, %x\n", hr);
                    break;
                }
            }
        }

    } while (false);

    if (NULL != pD3DImmediateContext)
    {
        pD3DImmediateContext->Unmap(pD3D11Texture2D, 0);
    }
    SAFE_RELEASE(pD3DImmediateContext);
    SAFE_RELEASE(pD3D11Texture2D);
    SAFE_RELEASE(pD3D11StagingTexture2D);

    return hr;
}

HRESULT CMediaEngineTest::GetDXComparionInfo(DXCompareInfo& comparisonInfo)
{

    comparisonInfo.dwHeight = m_ComparisonInfo.dwHeight;
    comparisonInfo.dwWidth = m_ComparisonInfo.dwWidth;

    comparisonInfo.dwFileCount = m_ComparisonInfo.dwFileCount;
    for (DWORD i = 0; i < comparisonInfo.dwFileCount; i++)
    {
        strcpy(comparisonInfo.ppszFrameFileNames[i], m_ComparisonInfo.ppszFrameFileNames[i]);
    }

    comparisonInfo.subType = m_ComparisonInfo.subType;
    return S_OK;
}

HRESULT CMediaEngineTest::WriteDXVAFrameToFile(PBYTE pData, int nPitch, DWORD dwHeight, DX_FORMAT subtype)
{

    HRESULT hr = S_OK;
    if (!pData)
    {
        Log(L"\n Data passed is null ");
        return E_POINTER;
    }

    PBYTE pSrc = pData;
    unsigned int i = 0;
    DWORD dwHeights = 0;
    DWORD dwWidths = 0;
    DWORD dwWidthToUse;
    DWORD dwHeightToUse;

    if (-1 == m_fd)
    {
        if (m_fD3DAWARE)
        {
            m_fd = _open(m_ppszDXVAFileNamesWithDecoration[m_dwFileCount - 1], _O_BINARY | _O_CREAT | _O_TRUNC | _O_WRONLY, _S_IREAD | _S_IWRITE);
        }

        else
        {
            m_fd = _open(m_ppszNonDXVAFileNamesWithDecoration[m_dwFileCount - 1], _O_BINARY | _O_CREAT | _O_TRUNC | _O_WRONLY, _S_IREAD | _S_IWRITE);
        }

    }

    if (m_fd == -1)
    {
        hr = HRESULT_FROM_WIN32(GetLastError());
        if (m_fD3DAWARE)
        {
            Log(L"\n Opening DXVA file failed with %x\n", hr);
        }
        else
        {
            Log(L"\n Opening NonDXVA file failed with %x\n", hr);
        }
        return hr;
    }
    if (-1 != m_fd)
    {
        hr = m_pMediaEngine->GetNativeVideoSize(&dwWidths, &dwHeights); //get again...
        if (FAILED(hr))
        {
            Log(L"\n Failed to get Native Size of the video and Heights and widths are not defined\n");
            hr = HRESULT_FROM_WIN32(GetLastError());
            return hr;
        }

        if (dwWidths != m_dwWidths[m_dwFileCount - 1] || dwHeights != m_dwHeights[m_dwFileCount - 1])
        {
            dwWidthToUse = dwWidths;
            dwHeightToUse = dwHeights;
        }
        else
        {
            dwWidthToUse = m_dwWidths[m_dwFileCount - 1];
            dwHeightToUse = m_dwHeights[m_dwFileCount - 1];
        }

#if 0
        Log(L"Width x Height = %d, %d\n", nPitch / 4, dwHeightToUse);
#endif

        switch (subtype)
        {
            /* only support ARGB32 for now */
        case FORMAT_ARGB32:
        case FORMAT_RGB32:
            {
                for (i = 0; i < dwHeightToUse; i++)
                {
                    _write(m_fd, pSrc, nPitch);
                    pSrc += nPitch;
                }

                break;
            }

        default:
            {
                hr = E_FAIL;

            }
        }
    }

    return hr;
}

HRESULT CMediaEngineTest::GenerateNewFileName(BOOL bDXVA)
{
    HRESULT hr = S_OK;
    DWORD dwFileNameSize = 0;
    char *pszFileName = NULL;
    char *pszFileNameTemp = NULL;
    char *pszFileNameBase = NULL;
    char *pszFileNameExt = NULL;

    do
    {
        int nLength = WideCharToMultiByte(CP_ACP, 0, m_pszTempOutFile, -1, NULL, 0, NULL, NULL);

        pszFileNameTemp = new char[nLength + 1];
        if (pszFileNameTemp == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;

        }

        if (0 == WideCharToMultiByte(CP_ACP, 0, (WCHAR *)((void *)m_pszTempOutFile), nLength, pszFileNameTemp, nLength + 1, NULL, NULL))
        {

            hr = E_OUTOFMEMORY;
            break;
        }

        pszFileNameBase = pszFileNameTemp;
        pszFileNameExt = strrchr(pszFileNameTemp, '.');


        if (pszFileNameExt == NULL)
        {
            dwFileNameSize = _scprintf("%s_%d_%dx%d", pszFileNameBase, m_dwFileCount - 1, m_dwWidths[m_dwFileCount - 1], m_dwHeights[m_dwFileCount - 1]);
        }
        else
        {
            pszFileNameExt[0] = '\0';
            pszFileNameExt++;

            dwFileNameSize = _scprintf("%s_%d_%dx%d.%s", pszFileNameBase, m_dwFileCount - 1, m_dwWidths[m_dwFileCount - 1], m_dwHeights[m_dwFileCount - 1], pszFileNameExt);
        }

        pszFileName = new char[dwFileNameSize + 1];
        if (pszFileName == NULL)
        {

            hr = E_OUTOFMEMORY;
            break;
        }

        if (pszFileNameExt == NULL)
        {
            _snprintf_s(pszFileName, dwFileNameSize + 1, dwFileNameSize, "%s_%d_%dx%d", pszFileNameBase, m_dwFileCount - 1, m_dwWidths[m_dwFileCount - 1], m_dwHeights[m_dwFileCount - 1]);
        }
        else
        {
            _snprintf_s(pszFileName, dwFileNameSize + 1, dwFileNameSize, "%s_%u_%ux%u.%s", pszFileNameBase, m_dwFileCount - 1, m_dwWidths[m_dwFileCount - 1], m_dwHeights[m_dwFileCount - 1], pszFileNameExt);
        }

        if (strlen(pszFileName) == 0)
        {
            hr = E_FAIL;
            break;
        }

        m_ComparisonInfo.dwFileCount = m_dwFileCount;

        if (m_fD3DAWARE)
        {
            strcpy(m_ppszDXVAFileNamesWithDecoration[m_dwFileCount - 1], pszFileName);
            strcat(m_ppszDXVAFileNamesWithDecoration[m_dwFileCount - 1], "_DXVA.rgb");

            strcpy(m_ComparisonInfo.ppszFrameFileNames[m_dwFileCount - 1], m_ppszDXVAFileNamesWithDecoration[m_dwFileCount - 1]);
        }
        else
        {
            strcpy(m_ppszNonDXVAFileNamesWithDecoration[m_dwFileCount - 1], pszFileName);
            strcat(m_ppszNonDXVAFileNamesWithDecoration[m_dwFileCount - 1], "_NOT_DXVA.rgb");

            strcpy(m_ComparisonInfo.ppszFrameFileNames[m_dwFileCount - 1], m_ppszNonDXVAFileNamesWithDecoration[m_dwFileCount - 1]);
        }
        pszFileName = NULL;
    }

    while (FALSE);

    SAFE_ARRAY_DELETE(pszFileNameTemp);
    SAFE_ARRAY_DELETE(pszFileName);

    return hr;
}

// Seek to a new playback rate.
HRESULT CMediaEngineTest::SetPlaybackRate(float rate)
{
    HRESULT hr = S_OK; 
    do
    {
        if (m_pMediaEngine)
        {
            hr =  m_pMediaEngine->SetPlaybackRate(rate);
            if(FAILED(hr))
            {
                break;
            } 

        }
    }while(false);
    return hr;
}


HRESULT SafeAllocateConfigStringsLocal(DWORD dwNumInstances, TCHAR*** pppszConfigStrings)
{
    HRESULT hr = S_OK;

    do
    {
        if(pppszConfigStrings == NULL)
        {
            hr = E_POINTER;
            break;
        }

        if(dwNumInstances == 0)
        {
            hr = E_INVALIDARG;
            break;
        }

        *pppszConfigStrings = (TCHAR**)malloc(sizeof(TCHAR*) * dwNumInstances);
        if(*pppszConfigStrings == NULL)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        ZeroMemory(*pppszConfigStrings, sizeof(TCHAR*) * dwNumInstances);
    }while(false);

    return hr;
}

HRESULT ProcessConfigStringLocal(TCHAR* pszString, DWORD* pdwNumInstances, TCHAR*** pppszConfigStrings, CSmartLogger logger)
{
    HRESULT hr = S_OK;

    TCHAR* pszStartOfConfig = NULL;
    TCHAR* pszEndOfConfig = NULL;

    do
    {
        if((pszString == NULL) || (pdwNumInstances == NULL) || (pppszConfigStrings == NULL))
        {
            hr = E_POINTER;
            break;
        }

        *pdwNumInstances = 0;
        *pppszConfigStrings = NULL;

        pszStartOfConfig = _tcsstr(pszString, _T("-"));
        if(pszStartOfConfig == NULL)
        {
            hr = S_FALSE;
            break;
        }

        if(_tcsnicmp(pszStartOfConfig, NUM_OF_INSTANCES_SWITCH, _tcslen(NUM_OF_INSTANCES_SWITCH)) != 0)
        {
            hr = S_FALSE;
            break;
        }

        pszStartOfConfig += _tcslen(NUM_OF_INSTANCES_SWITCH); // Move past the switch

        if (_stscanf_s(pszStartOfConfig, _T("%u"), pdwNumInstances) != 1)
        {
            hr = E_INVALIDARG;
            logger.Log(_T("ERROR  >>\t %s is not a valid number of instances"), pszStartOfConfig);
            break;
        }

        hr = SafeAllocateConfigStringsLocal(*pdwNumInstances, pppszConfigStrings);
        if(FAILED(hr))
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        for(DWORD i = 0; i < *pdwNumInstances; i++)
        {
            pszStartOfConfig = _tcsstr(pszStartOfConfig, _T("{"));
            if(pszStartOfConfig == NULL)
            {
                hr = E_INVALIDARG;
                logger.Log(_T("ERROR  >>\t Can't find start of config string"));
                break;
            }

            pszEndOfConfig = pszStartOfConfig + 1;

            DWORD dwStringLenLeft = _tcslen(pszEndOfConfig);
            DWORD dwLevel = 1;

            while(true)
            {
                if(dwStringLenLeft == 0)
                {
                    hr = E_INVALIDARG;
                    logger.Log(_T("ERROR  >>\t Coun't find matching '}' for config string"));
                    break;
                }

                if(*pszEndOfConfig == _T('{'))
                {
                    dwLevel++;
                }
                else if(*pszEndOfConfig == _T('}'))
                {
                    dwLevel--;

                    if(dwLevel == 0)
                    {
                        // We found the end of our config string
                        DWORD dwStringLen = (DWORD)((pszEndOfConfig - pszStartOfConfig) - 1); // The -1 accounts for the {} + \0

                        (*pppszConfigStrings)[i] = (TCHAR*)malloc(sizeof(TCHAR) * dwStringLen);
                        if((*pppszConfigStrings)[i] == NULL)
                        {
                            hr = E_OUTOFMEMORY;
                            break;
                        }

                        _tcsncpy_s((*pppszConfigStrings)[i], dwStringLen, pszStartOfConfig + 1, dwStringLen - 1);

                        pszStartOfConfig = pszEndOfConfig + 1;
                        break;
                    }
                }

                pszEndOfConfig++;
                dwStringLenLeft--;
            }

            if(FAILED(hr))
            {
                break;
            }
        }
    }while(false);

    if(FAILED(hr))
    {
        SafeFreeConfigStringsLocal(pdwNumInstances, pppszConfigStrings);
    }

    return hr;
}

void SafeFreeConfigStringsLocal(DWORD* pdwNumInstances, TCHAR*** pppszConfigStrings)
{
    if((pppszConfigStrings != NULL) && (pdwNumInstances != NULL))
    {
        if(*pppszConfigStrings != NULL)
        {
            for(DWORD i = 0; i < *pdwNumInstances; i++)
            {
                free((*pppszConfigStrings)[i]);
            }

            free(*pppszConfigStrings);
            *pppszConfigStrings = NULL;

            *pdwNumInstances = 0;
        }
    }
}


