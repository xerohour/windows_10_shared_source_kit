//
// Copyright(C) Microsoft.All rights reserved.
//
// Lullaby.cpp
//
// Description:
//
//  Shell98 Module Implementation for Lullaby Test Application
//

#include "stdafx.h"
#include <limits.h>
#include <StrSafe.h>
#include "DevTrans.h"
#include <atlstr.h>
#include "powrprof.h"
#include "avendpointkeys.h"

#ifndef BUILD_TAEF
static LPCWSTR g_szModuleName = L"Device Power Test - Lullaby";
ITestShell* g_IShell;
#else
CTaefShellStub g_IShellObject;
CTaefShellStub * g_IShell = &g_IShellObject;
IBasicLog * g_pBasicLog = NULL;

using namespace WEX::Common;
using namespace WEX::TestExecution;
#endif
CLullabyDeviceTestModule * g_pLullabyDeviceTestModule = NULL;

// ------------------------------------------------------------------------------
// App static name
#define DT_ENDPOINT_RENDER      0x1
#define DT_ENDPOINT_CAPTURE     0x2
#define DT_DEVICE_OTHER         0x4

//
// Test Case Info Structure
//
BEGIN_TEST_CASE_INFO(LullabyTaef, CLullabyDeviceTestModule)
//                       [TEST ID]   [Test Case Name]                                                      [TEST CASE FLAGS]    [DEVICE TYPE FLAGS]                       [NA]  [N/A]  [TEST CASE FUNCTION POINTER]                            [TEST CASE GUID]
    DEVICE_TEST_PARENT( L"",         L"Lullaby Test",                                                       0,                   0,                                        0,    NULL,  NULL,                                              L"{070085fd-b8f7-401a-9b7a-6eccf2e6dc70}")
    DEVICE_TEST_PARENT( L"",         L" Suspend(S3)",                                                       0,                   0,                                        0,    NULL,  NULL,                                              L"{868C1FED-9CA7-4a4c-8DBF-F5D1274BBC6F}")
    DEVICE_TEST_METHOD( L"1.1",      L"  WASAPI Render Sync",                                               0,                   DT_ENDPOINT_RENDER,                       0,    NULL,  TC_WASAPI_Render_Sync_S3,                          L"{8F7D96B8-B7F5-4d79-A6AB-AEB460216CBF}")
    DEVICE_TEST_METHOD( L"1.2",      L"  WASAPI Render Async",                                              0,                   DT_ENDPOINT_RENDER,                       0,    NULL,  TC_WASAPI_Render_Async_S3,                         L"{071D5429-043A-4846-9C47-23A7E7E749EE}")
    DEVICE_TEST_METHOD( L"1.3",      L"  WASAPI Render Offload Sync",                                       0,                   DT_ENDPOINT_RENDER,                       0,    NULL,  TC_WASAPI_Render_Offload_Sync_S3,                  L"{DA34C7AF-0D89-430A-8EB5-7C68EB5CCD5C}")
    DEVICE_TEST_METHOD( L"1.4",      L"  WASAPI Render Offload Async",                                      0,                   DT_ENDPOINT_RENDER,                       0,    NULL,  TC_WASAPI_Render_Offload_Async_S3,                 L"{C71C5C57-8B19-4192-B177-87F11B0912E2}")
    DEVICE_TEST_METHOD( L"1.5",      L"  WASAPI Capture Sync",                                              0,                   DT_ENDPOINT_CAPTURE,                      0,    NULL,  TC_WASAPI_Capture_Sync_S3,                         L"{E8A0CA00-2C99-495d-B026-482CE65DBDA7}")
    DEVICE_TEST_METHOD( L"1.6",      L"  WASAPI Capture Async",                                             0,                   DT_ENDPOINT_CAPTURE,                      0,    NULL,  TC_WASAPI_Capture_Async_S3,                        L"{0E79581C-9708-4c68-9294-9D1CD8DE1201}")
    DEVICE_TEST_PARENT( L"",         L" Hibernate(SH)",                                                     0,                   0,                                        0,    NULL,  NULL,                                              L"{A6A4975D-A7CD-4a6f-9B60-1C27BAC79481}")
    DEVICE_TEST_METHOD( L"2.1",      L"  WASAPI Render Sync",                                               0,                   DT_ENDPOINT_RENDER,                       0,    NULL,  TC_WASAPI_Render_Sync_SH,                          L"{EF584D7A-6D18-4b13-BB47-8A312E6BCB40}")
    DEVICE_TEST_METHOD( L"2.2",      L"  WASAPI Render Async",                                              0,                   DT_ENDPOINT_RENDER,                       0,    NULL,  TC_WASAPI_Render_Async_SH,                         L"{7C655B69-EB06-46db-9B0B-6D96D338D7FB}")
    DEVICE_TEST_METHOD( L"2.3",      L"  WASAPI Render Offload Sync",                                       0,                   DT_ENDPOINT_RENDER,                       0,    NULL,  TC_WASAPI_Render_Offload_Sync_SH,                  L"{DA7F7BD1-526E-49F4-9EA3-AD50C09AFAF5}")
    DEVICE_TEST_METHOD( L"2.4",      L"  WASAPI Render Offload Async",                                      0,                   DT_ENDPOINT_RENDER,                       0,    NULL,  TC_WASAPI_Render_Offload_Async_SH,                 L"{1173DE2C-00FC-4326-970B-342B19FB5747}")
    DEVICE_TEST_METHOD( L"2.5",      L"  WASAPI Capture Sync",                                              0,                   DT_ENDPOINT_CAPTURE,                      0,    NULL,  TC_WASAPI_Capture_Sync_SH,                         L"{4BC3CB32-0E63-42b8-B5CB-0DCB0FC59ABB}")
    DEVICE_TEST_METHOD( L"2.6",      L"  WASAPI Capture Async",                                             0,                   DT_ENDPOINT_CAPTURE,                      0,    NULL,  TC_WASAPI_Capture_Async_SH,                        L"{91E9BFC2-5228-4a95-BB04-D9927D4866FB}")
    DEVICE_TEST_PARENT( L"",         L" Connected Standby(CS)",                                             0,                   0,                                        0,    NULL,  NULL,                                              L"{80B2AF5F-DC71-4E8B-DB1A-231440627229}")
    DEVICE_TEST_METHOD( L"3.1",      L"  WASAPI Render Sync",                                               0,                   DT_ENDPOINT_RENDER,                       0,    NULL,  TC_WASAPI_Render_Sync_CS,                          L"{878C36A7-A4DA-4ACE-A816-DD822D72D9BF}")
    DEVICE_TEST_METHOD( L"3.2",      L"  WASAPI Render Async",                                              0,                   DT_ENDPOINT_RENDER,                       0,    NULL,  TC_WASAPI_Render_Async_CS,                         L"{77DFC609-8159-44DA-90B7-77270DC09869}")
    DEVICE_TEST_METHOD( L"3.3",      L"  WASAPI Render Offload Sync",                                       0,                   DT_ENDPOINT_RENDER,                       0,    NULL,  TC_WASAPI_Render_Offload_Sync_CS,                  L"{23FE85D7-52DC-47DF-8088-1CC619BFB440}")
    DEVICE_TEST_METHOD( L"3.4",      L"  WASAPI Render Offload Async",                                      0,                   DT_ENDPOINT_RENDER,                       0,    NULL,  TC_WASAPI_Render_Offload_Async_CS,                 L"{8A53209E-1363-462B-8E5D-C9772B155638}")
    DEVICE_TEST_METHOD( L"3.5",      L"  WASAPI Capture Sync",                                              0,                   DT_ENDPOINT_CAPTURE,                      0,    NULL,  TC_WASAPI_Capture_Sync_CS,                         L"{10BBDC90-1A64-406F-83BC-C955F9302DAE}")
    DEVICE_TEST_METHOD( L"3.6",      L"  WASAPI Capture Async",                                             0,                   DT_ENDPOINT_CAPTURE,                      0,    NULL,  TC_WASAPI_Capture_Async_CS,                        L"{D9A5459A-542A-4B55-B9A4-C69B046C2A2D}")
    DEVICE_TEST_PARENT( L"",         L" Glitch Testing",                                                    0,                   0,                                        0,    NULL,  NULL,                                              L"{43D02B9F-8A97-4E8F-81C4-6617FC74FB64}")
    /*DEVICE_TEST_METHOD( L"4.1",      L"  WASAPI Render Async Glitch",                                       0,                   DT_ENDPOINT_RENDER,                       0,    NULL,  TC_WASAPI_Render_Async_CS_Glitch,                  L"{AB2A2984-6EA7-4125-A976-5DB1F6D2D7FA}")*/
    DEVICE_TEST_METHOD( L"4.2",      L"  WASAPI Render Offload Async Glitch",                               0,                   DT_ENDPOINT_RENDER,                       0,    NULL,  TC_WASAPI_Render_Offload_Async_CS_Glitch,          L"{9E425679-5B92-480E-3516-EF3DD9E13C99}")
    DEVICE_TEST_METHOD( L"4.3",      L"  WASAPI Capture Async Glitch",                                      0,                   DT_ENDPOINT_CAPTURE,                      0,    NULL,  TC_WASAPI_Capture_Async_CS_Glitch,                 L"{EC888DED-679D-462F-9B29-A9FA3D2CC9AE}")
    DEVICE_TEST_PARENT( L"",         L" Bluetooth Presence Testing",                                        0,                   0,                                        0,    NULL,  NULL,                                              L"{065455C0-95BC-4B6E-0A1B-B1C2D6C366B1}")
    DEVICE_TEST_METHOD( L"4.1",      L"  Verify Bth Audio Presence",                                        0,                   DT_DEVICE_OTHER,                          0,    NULL,  TC_Verify_BTH,                                     L"{30EDEA37-9753-40FB-B33B-2334E740C883}")
    DEVICE_TEST_PARENT( L"",         L" Lullaby Testing",                                                   0,                   0,                                        0,    NULL,  NULL,                                              L"{C7FA50EE-CC85-4429-B34C-4015E318764B}")
    DEVICE_TEST_METHOD( L"5.1",      L"  Verify SleepAndResume",                                            0,                   DT_DEVICE_OTHER,                          0,    NULL,  TC_Verify_SleepResume,                             L"{B758920A-E7C6-4355-96A5-43E77ED10799}")
END_TEST_CASE_INFO(LullabyTaef)

#ifndef BUILD_TAEF
// ------------------------------------------------------------------------------
// Shell98 extern
CTestModule* WINAPI NewTestModule ( CTestShell* pShell, HINSTANCE   hInstDLL )
{
    g_IShell = (ITestShell*)pShell;
    g_pLullabyDeviceTestModule = new CLullabyDeviceTestModule(pShell, hInstDLL);
    return (CTestModule*)g_pLullabyDeviceTestModule;
}
#endif

//--------------------------------------------------------------------------;
//  CLullabyDevice::CLullabyDevice
//
//  Description:
//      Generic constructor for Lullaby device.
//
//  Arguments:
//      LPCWSTR pszName: Name of device.
//      LPCWSTR pszId: (Pnp) Id of device.
//      LPARAM lpType: Type of device (Most likely DT_DEVICE_OTHER).
//      HRESULT &hr: HRESULT to bubble out.
//
//  Return (None):
//
//--------------------------------------------------------------------------;
CLullabyDevice::CLullabyDevice ( LPCWSTR pszName, LPCWSTR pszId, LPARAM lpType, HRESULT &hr ):
    m_pwszEndpointId(NULL)
{
    if (FAILED(hr))
    {
        return;
    }

    if (S_OK != (hr = StringCbCopy(m_szRep, sizeof(m_szRep), pszName)))
    {
        return;
    }

    hr = StringCbCopy(m_szPnPId, sizeof(m_szPnPId), pszId);

    m_lpType = lpType;
}


//--------------------------------------------------------------------------;
//  CLullabyDevice::CLullabyDevice
//
//  Description:
//      Constructor for Lullaby device (endpoints).
//
//  Arguments:
//      IMMDevice *pIEndpoint: Endpoint from device enumerator.
//      HRESULT &hr: HRESULT to bubble out.
//
//  Return (None):
//
//--------------------------------------------------------------------------;
CLullabyDevice::CLullabyDevice ( IMMDevice *pIEndpoint, HRESULT &hr ):
    m_pwszEndpointId(NULL)
{
    CComPtr<IMMDevice>          pDevnode = NULL;
    CComPtr<IMMEndpoint>        pIEP = NULL;
    CComPtr<IPropertyStore>     pStore = NULL;
    CComHeapPtr<WCHAR>          pszName;
    CComHeapPtr<WCHAR>          pszDesc;
    CComHeapPtr<WCHAR>          pszInstanceId;
    EDataFlow                   df;

    if (FAILED(hr))
    {
        return;
    }

    hr = pIEndpoint->QueryInterface(__uuidof(IMMEndpoint), (void**)&pIEP);
    if (S_OK != hr)
    {
        XLOG(XFAIL, eError, L"Failed to query for IMMEndpoint from IMMDevice (hr = %s)", GetHRString(hr));
        return;
    }

    if (S_OK != (hr = pIEP->GetDataFlow(&df)))
    {
        XLOG(XFAIL, eError, L"Failed to get IMMEndpoint device dataflow (hr = %s)", GetHRString(hr));
        return;
    }

    if (eRender == df)
    {
        m_lpType = DT_ENDPOINT_RENDER;
    }
    else if (eCapture == df)
    {
        m_lpType = DT_ENDPOINT_CAPTURE;
    }
    else
    {
        XLOG(XFAIL, eError, L"Data flow (%d) is unexpected", df);
        hr = E_INVALIDARG;
        return;
    }

    if (S_OK != (hr = DevTrans::GetEndpointDevnode(pIEndpoint, &pDevnode)))
    {
        XLOG(XFAIL, eError, L"Failed GetEndpointDevnode (hr = %s)", GetHRString(hr));
        return;
    }

    if (S_OK != (hr = DevTrans::GetDevnodeInstanceId(&pszInstanceId, pDevnode)))
    {
        XLOG(XFAIL, eError, L"Failed to get device instance ID of the devnode of the current endpoint (hr = %s)", GetHRString(hr));
        return;
    }

    hr = StringCbCopy(m_szPnPId, sizeof(m_szPnPId), pszInstanceId);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"Failed to copy instance ID (hr = %s)", GetHRString(hr));
        return;
    }

    if (S_OK != (hr = DevTrans::GetEndpointDisplayNameW(&pszName, pIEndpoint)))
    {
        XLOG(XFAIL, eError, L"Failed to get endpoint display name (hr = %s)", GetHRString(hr));
        return;
    }

    if (S_OK != (hr = DevTrans::GetEndpointDescriptionW(&pszDesc, pIEndpoint)))
    {
        XLOG(XFAIL, eError, L"Failed to get endpoint description (hr = %s)", GetHRString(hr));
        return;
    }

    //get endpoint ID from IMMDevice
    if (S_OK != (hr = pIEndpoint->GetId(&m_pwszEndpointId)))
    {
        XLOG(XFAIL, eError, L"Failed to get endpoint id (hr = %s)", GetHRString(hr));
        return;
    }

    hr = StringCbPrintfW( m_szRep, sizeof(m_szRep), L"%ls (%ls)", (LPCWSTR)pszDesc, (LPCWSTR)pszName);
    return;
}

CLullabyDevice::~CLullabyDevice()
{
    if (NULL != m_pwszEndpointId)
    {
        CoTaskMemFree(m_pwszEndpointId);
    }
}

//--------------------------------------------------------------------------;
//  CLullabyDevice::DoesDeviceMatch
// 
//  Description:
//      Override CDeviceDescriptor's implementation to match endpoints and 
//      device instance ID.
//
//  Arguments:
//      const WCHAR * szID: selected device by the command line
//
//  Return (None):
//
//--------------------------------------------------------------------------;
BOOL CLullabyDevice::DoesDeviceMatch(const WCHAR *szSelectedId)
{
    if (0 == _wcsicmp(m_szPnPId, szSelectedId))
    {
        // try to match the device instance ID of the selected device.    
        return TRUE;
    }

    return FALSE;
}

//--------------------------------------------------------------------------;
//  CLullabyDevice::IsBluetoothDevice
// 
//  Description:
//      Returns whether a device is Bluetooth.
//
//  Arguments: (None)
//
//  Return (None):
//
//--------------------------------------------------------------------------;
BOOL CLullabyDevice::IsBluetoothDevice()
{
    CComPtr<IMMDevice> spDevice = NULL;
    CComPtr<IPropertyStore> spPropStore = NULL;
    PROPVARIANT var;
    BOOL retVal = FALSE;

    HRESULT hr = GetEndpoint(&spDevice);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: GetEndpoint encountered error (hr = %s).", GetHRString(hr));
        return FALSE;
    }

    hr = spDevice->OpenPropertyStore(STGM_READ, &spPropStore);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Failed to open endpoint property store (hr = %s).", GetHRString(hr));
        return FALSE;
    }

    PropVariantInit(&var);
    hr = spPropStore->GetValue(PKEY_Endpoint_IsBluetooth, &var);
    if (SUCCEEDED(hr) && (var.vt == VT_BOOL) && var.boolVal)
    {
        retVal = TRUE;
    }

    PropVariantClear(&var);
    return retVal;
}

//--------------------------------------------------------------------------;
//  CLullabyDeviceTestModule::CLullabyDeviceTestModule
//
//  Description:
//      Constructor for Lullaby Test Module for Shell98
//
//  Arguments:
//      CTestShell *pShell: Standard Shell98 parameters
//      HINSTANCE  hInstDll: Standard Shell98 parameters
//
//  Return (None):
//
//--------------------------------------------------------------------------;
#ifndef BUILD_TAEF
CLullabyDeviceTestModule::CLullabyDeviceTestModule ( CTestShell *pShell, HINSTANCE hInstDll ):
CDeviceTestModule(pShell, hInstDll),
    m_pDevice(NULL),
    m_fInternal(false),
    m_dwSavedACValue(0),
    m_dwSavedDCValue(0)
{
    m_dwAppID        = 1000;       
    m_hInstance      = hInstDll;
    m_pstrModuleName = g_szModuleName;
    m_nIconID        = APPICON;

    m_rgTestCaseInfo = rgTestCaseInfo;
    m_cTestCases     = ARRAY_ELEMENTS(rgTestCaseInfo);
    m_dwWHQLChapter  = WHQL_CHAPTER_AUDIO; 
}
#else
CLullabyDeviceTestModule::CLullabyDeviceTestModule ( DV_TCINFO * ptcInfo ):
CDeviceTestModule(),
    m_pDevice(NULL),
    m_fInternal(false),
    m_dwSavedACValue(0),
    m_dwSavedDCValue(0)
{
    m_rgTestCaseInfo = ptcInfo;
    m_cTestCases     = 1;
}
#endif

//--------------------------------------------------------------------------;
//  CLullabyDeviceTestModule::~CLullabyDeviceTestModule
//
//  Description:
//      Destructor for Lullaby Test Module for Shell98
//
//  Arguments:
//      None.
//
//  Return (None):
//
//--------------------------------------------------------------------------;
CLullabyDeviceTestModule::~CLullabyDeviceTestModule(VOID)
{
    RestoreWakeTimerSettings();
}

BOOL CLullabyDeviceTestModule::EnableWakeTimers()
{
    DWORD dwRet = ERROR_SUCCESS;
    BOOL bSuccess = FALSE;
    //get the power scheme
    GUID* pActivePowerScheme = NULL;
    dwRet = PowerGetActiveScheme(NULL, &pActivePowerScheme);
    if (ERROR_SUCCESS != dwRet)
    {
        XLOG(XFAIL, eError, L"Failed getting active power scheme (%d)", dwRet);
        goto EXIT;
    }

    //read the AC and DC values and save for restoring later
    dwRet = PowerReadACValueIndex(NULL, 
            pActivePowerScheme,
            &GUID_SLEEP_SUBGROUP,
            &GUID_ALLOW_RTC_WAKE,
            &m_dwSavedACValue);
    if (ERROR_SUCCESS != dwRet)
    {
        XLOG(XFAIL, eError, L"Failed reading AC power index for wake timers (%d)", dwRet);
        goto EXIT;
    }

    dwRet = PowerReadDCValueIndex(NULL, 
            pActivePowerScheme,
            &GUID_SLEEP_SUBGROUP,
            &GUID_ALLOW_RTC_WAKE,
            &m_dwSavedDCValue);

    if (ERROR_SUCCESS != dwRet)
    {
        XLOG(XFAIL, eError, L"Failed reading DC power index for wake timers (%d)", dwRet);
        goto EXIT;
    }

    //set the new values, overwriting is fine
    dwRet = PowerWriteACValueIndex(NULL,
            pActivePowerScheme,
            &GUID_SLEEP_SUBGROUP,
            &GUID_ALLOW_RTC_WAKE,
            TRUE);
    if (ERROR_SUCCESS != dwRet)
    {
        XLOG(XFAIL, eError, L"Failed setting AC power index for wake timers to ON (%d)", dwRet);
        goto EXIT;
    }

    dwRet = PowerWriteDCValueIndex(NULL,
            pActivePowerScheme,
            &GUID_SLEEP_SUBGROUP,
            &GUID_ALLOW_RTC_WAKE,
            TRUE);
    if (ERROR_SUCCESS != dwRet)
    {
        XLOG(XFAIL, eError, L"Failed setting DC power index for wake timers to ON (%d)", dwRet);
        goto EXIT;
    }

    //finalize so the new settings are applied right now
    dwRet = PowerSetActiveScheme(NULL, pActivePowerScheme);
    if (ERROR_SUCCESS != dwRet)
    {
        XLOG(XFAIL, eError, L"Failed setting active power scheme (%d)", dwRet);
        goto EXIT;
    }

    bSuccess = TRUE;
EXIT:
    if (NULL != pActivePowerScheme)
    {
        LocalFree(pActivePowerScheme);
    }

    return bSuccess;
}

VOID CLullabyDeviceTestModule::RestoreWakeTimerSettings()
{
    DWORD dwRet = ERROR_SUCCESS;

    //get the power scheme
    GUID* pActivePowerScheme = NULL;
    dwRet = PowerGetActiveScheme(NULL, &pActivePowerScheme);
    if (ERROR_SUCCESS != dwRet)
    {
        XLOG(XFAIL, eError, L"Failed getting active power scheme (%d)", dwRet);
        goto EXIT;
    }

    //set the original values
    dwRet = PowerWriteACValueIndex(NULL,
        pActivePowerScheme,
        &GUID_SLEEP_SUBGROUP,
        &GUID_ALLOW_RTC_WAKE,
        m_dwSavedACValue);
    if (ERROR_SUCCESS != dwRet)
    {
        XLOG(XFAIL, eError, L"Failed setting AC power index for wake timers to ON (%d)", dwRet);
        goto EXIT;
    }

    dwRet = PowerWriteDCValueIndex(NULL,
        pActivePowerScheme,
        &GUID_SLEEP_SUBGROUP,
        &GUID_ALLOW_RTC_WAKE,
        m_dwSavedDCValue);
    if (ERROR_SUCCESS != dwRet)
    {
        XLOG(XFAIL, eError, L"Failed setting DC power index for wake timers to ON (%d)", dwRet);
        goto EXIT;
    }

    //finalize so the new settings are applied right now
    dwRet = PowerSetActiveScheme(NULL, pActivePowerScheme);
    if (ERROR_SUCCESS != dwRet)
    {
        XLOG(XFAIL, eError, L"Failed setting active power scheme (%d)", dwRet);
        goto EXIT;
    }

EXIT:
    if (NULL != pActivePowerScheme)
    {
        LocalFree(pActivePowerScheme);
    }
}

//--------------------------------------------------------------------------;
//  DWORD CLullabyDeviceTestModule::OnInitialUpdate
//
//  Description:
//      Shell98 app's *real* entry point.
//
//  Arguments:
//      None.
//
//  Return (DWORD):
//      FNS_PASS if successful.
//
//--------------------------------------------------------------------------;
DWORD CLullabyDeviceTestModule::OnInitialUpdate(VOID)
{
    CComPtr<IMMDeviceEnumerator>    pDevEnum = NULL;
    CComPtr<IMMDeviceCollection>    pEndpoints = NULL;
    CLullabyDevice               *pDevice = NULL;
    UINT                            iEndpoint;
    HRESULT                         hr = S_OK;

    if (FALSE == EnableWakeTimers())
    {
        return (FNS_FAIL);
    }

    //  Adding generic device...
    pDevice = new CLullabyDevice(L"Generic Audio Device", L"Generic Pnp Id", DT_DEVICE_OTHER, hr);

    if (NULL == pDevice)
    {
        return (FNS_FAIL);
    }

    if (S_OK != hr)
    {
        XLOG(XMSG, eError, L"Failed creating generic audio device (hr = %s).", GetHRString(hr));
        return (FNS_FAIL);
    }

    pDevice->m_fSelected = TRUE;
    g_IShell->AddDevice(pDevice);

    if (S_OK != (hr = pDevEnum.CoCreateInstance(__uuidof(MMDeviceEnumerator))))
    {
        XLOG(XMSG, eError, L"Failed creating MMDeviceEnumerator (hr = %s).", GetHRString(hr));
        return (FNS_FAIL);
    }

    hr = pDevEnum->EnumAudioEndpoints(eAll, DEVICE_STATE_ACTIVE, &pEndpoints);

    if (S_OK != hr)
    {
        XLOG(XMSG, eError, L"Failed to enumerate active and unplugged audio endpoint device (hr = %s).", GetHRString(hr));
        return (FNS_FAIL);
    }

    if (S_OK != (hr = pEndpoints->GetCount(&iEndpoint)))
    {
        XLOG(XMSG, eError, L"Failed to get count of audio endpoint device (hr = %s).", GetHRString(hr));
        return (FNS_FAIL);
    }

    for (; iEndpoint; iEndpoint--)
    {
        CComPtr<IMMDevice>      pIEndpoint = NULL;

        if (S_OK != (hr = pEndpoints->Item(iEndpoint - 1, &pIEndpoint)))
        {
            XLOG(XMSG, eError, L"Failed to get the audio endpoint endpoint device at index %d (hr = %s).", (iEndpoint - 1), GetHRString(hr));
            return (FNS_FAIL);
        }

        pDevice = new CLullabyDevice(pIEndpoint, hr);

        if (NULL == pDevice)
        {
            return (FNS_FAIL);
        }

        if (S_OK != hr)
        {
            XLOG(XMSG, eError, L"Failed to create an instance of CLullabyDevice for audio endpoint device at index %d (hr = %s).", (iEndpoint - 1), GetHRString(hr));
            delete pDevice;
            return (FNS_FAIL);
        }

#ifdef BUILD_TAEF
        // Do we have a TAEF parameter to select the device?
        String targetDevice;
        if (SUCCEEDED(RuntimeParameters::TryGetValue(L"InstanceId", targetDevice)))
        {
            if (!pDevice->DoesDeviceMatch(targetDevice))
            {
                delete pDevice;
                continue;
            }
        }
#endif
        pDevice->m_fSelected = TRUE;
        g_IShell->AddDevice(pDevice);
    }

    return FNS_PASS;
}

//--------------------------------------------------------------------------;
//  VOID CLullabyDeviceTestModule::SetTestDevice
//
//  Description:
//      Callback function for Shell98 apps for setting the device.
//
//  Arguments:
//      CDeviceDescriptor *pDevice: Device set by Shell98
//
//  Return (None):
//
//--------------------------------------------------------------------------;
VOID CLullabyDeviceTestModule::SetTestDevice ( CDeviceDescriptor *pDevice )
{
    m_pDevice = (CLullabyDevice*)pDevice;
}

//--------------------------------------------------------------------------;
//  DWORD CLullabyDeviceTestModule::RunTest
//
//  Description:
//      Wrapper for running each test case.
//
//  Arguments:
//      DWORD dwTestCase: Index of test case.
//
//  Return (None):
//
//--------------------------------------------------------------------------;
DWORD CLullabyDeviceTestModule::RunTest ( DWORD dwTestCase )
{
    DWORD dwRes = FNS_PASS;
    dwRes = ((PDV_TCINFO)m_rgTestCaseInfo)[dwTestCase].pfnTest();
    return dwRes;
}

HRESULT CLullabyDevice::GetEndpoint(IMMDevice** ppDevice)
{
    if (NULL == ppDevice)
    {
        return E_POINTER;
    }

    HRESULT hr = S_OK;
    CComPtr<IMMDeviceEnumerator> spMMDevEnum;
    hr = spMMDevEnum.CoCreateInstance(__uuidof(MMDeviceEnumerator));
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"Failed to cocreate IMMDeviceEnumerator (hr = %s).", GetHRString(hr));
        return hr;
    }

    //retrieve MMDevice from Endpoint Id stored in device list setup
    hr = spMMDevEnum->GetDevice(this->m_pwszEndpointId, ppDevice);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"Failed to get IMMDevice for Endpoint Id: %s (hr = %s).", m_pwszEndpointId, GetHRString(hr));
        return hr;
    }

    XLOG(XMSG, eInfo1, L"Endpoint ID (%s).", m_pwszEndpointId);

    return hr;
}
