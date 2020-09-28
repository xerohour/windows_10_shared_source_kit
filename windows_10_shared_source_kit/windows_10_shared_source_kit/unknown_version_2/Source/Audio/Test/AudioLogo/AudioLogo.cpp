//--------------------------------------------------------------------------
//
//  File: AudioLogo.cpp
//
//  Copyright (C) Microsoft. All rights reserved.
//
//  Abstract:
//      This is the skeleton for the test application.
//
//  Contents:
//      NewTestModule()
//      CAudioLogoDevice::CAudioLogoDevice()
//      CAudioLogoDeviceTestModule::CAudioLogoDeviceTestModule
//      CAudioLogoDeviceTestModule::~CAudioLogoDeviceTestModule()
//      CAudioLogoDeviceTestModule::OnInitialUpdate()
//      CAudioLogoDeviceTestModule::SetTestDevice()
//      CAudioLogoDeviceTestModule::RunTest()
//
//
//--------------------------------------------------------------------------
#include <initguid.h>
#include "Common.h"
#include <MMSystem.h>
#include <DevTrans.h>
#include <DevPKey.h>
#include <hrstring.h>
#include <atlstr.h>
#include <TokenString.h>
#include <CPropVariant.h>

using namespace DevTrans;

// ------------------------------------------------------------------------------
// App static name
#define DT_ENDPOINT_RENDER      0x1
#define DT_ENDPOINT_CAPTURE     0x2
#define DT_DEVICE_OTHER         0x4

#ifndef BUILD_TAEF
static LPCWSTR g_szModuleName = L"Generic Audio Logo Test - 1.0";
ITestShell* g_IShell;
#else
CTaefShellStub g_IShellObject;
CTaefShellStub * g_IShell = &g_IShellObject;
IBasicLog * g_pBasicLog = NULL;

using namespace WEX::Common;
using namespace WEX::TestExecution;
#endif

CAudioLogoDeviceTestModule     *g_pAudioLogoDeviceTestModule = NULL;

// ------------------------------------------------------------------------------
// Shell98 test definition
//typedef struct dv_dvinfo_tag
//{
//    LPTSTR      pszCaseID;      // shell    - test case ID
//    LPTSTR      pszName;        // shell    - test case name
//    LPARAM      lpCaseFlags;    // shell    - test case flags
//    LPARAM      lpDeviceFlags;  // shell    - device type flags
//    LPARAM      lpSTFlags;      // selftest - server/cable/input flags
//    LPARAM      lpUser;         // User-defined field
//    TESTFNPROC  pfnTest;        // shell    - Pointer to the test case function
//    LPTSTR      pszGuid;        // shell    - test case GUID identifier
//} DV_TCINFO, *PDV_TCINFO;

BEGIN_TEST_CASE_INFO(AudioLogoTaef, CAudioLogoDeviceTestModule)
//                     [TEST ID]   [Test Case Name]                   [TEST CASE FLAGS]  [DEVICE TYPE FLAGS]                                  [NA] [N/A] [TEST CASE FUNCTION POINTER]                  [TEST CASE GUID]
    DEVICE_TEST_PARENT( L"",    L"Generic",                                           0, 0,                                                      0, 0, NULL,                                 L"{69e636e7-8d13-4926-acec-1e245d4acd0f}")
    DEVICE_TEST_METHOD( L"0.0", L" Verify all endpoints available",                   0, DT_DEVICE_OTHER,                                        0, 0, TC_VerifyAllEndpointsAvailable,       L"{1528d811-2287-4a92-bda8-907c5ad6b39d}")
    DEVICE_TEST_METHOD( L"0.1", L" Verify Bluetooth Presence",                        0, DT_DEVICE_OTHER,                                        0, 0, TC_Verify_BTH,                        L"{6EB063A5-EE96-431B-8CEA-84DC44156182}")
    DEVICE_TEST_METHOD( L"0.2", L" Verify Microphone Array RAW format",               0, DT_ENDPOINT_CAPTURE,                                    0, 0, TC_VerifyMicArrayRawFormat,           L"{04EDCB6F-77DE-4FEC-DEFE-B4E3D4E0DD49}")
    DEVICE_TEST_PARENT( L"",    L"Audio Render",                                      0, 0,                                                      0, 0, NULL,                                 L"{393ff98b-95d8-4080-937a-7b63a469a035}")
    DEVICE_TEST_METHOD( L"1.0", L" Render to all endpoints",                          0, DT_DEVICE_OTHER,                                        0, 0, TC_RenderToAllEndpoints,              L"{aa73ec80-65a9-4f07-9ce5-f6d08551407f}")
    DEVICE_TEST_METHOD( L"1.1", L" Verify mute on compressed endpoints",              0, DT_ENDPOINT_RENDER,                                     0, 0, TC_VerifyMuteOnCompressedEndpoint,    L"{08cb3c3e-31c7-467a-8e8c-0ad4fc438129}")
    DEVICE_TEST_PARENT( L"",    L"Pull Mode",                                         0, DT_ENDPOINT_RENDER,                                     0, 0, NULL,                                 L"{f18b2873-0e96-46a9-a513-78027fa583ef}")
    DEVICE_TEST_METHOD( L"2.0", L" Render Exclusive",                                 0, DT_ENDPOINT_RENDER,                                     0, 0, TC_RenderExclusivePullMode,           L"{10fa3531-3923-4092-b756-3b85da5ffb84}")
    DEVICE_TEST_METHOD( L"2.1", L" Capture Exclusive",                                0, DT_ENDPOINT_CAPTURE,                                    0, 0, TC_CaptureExclusivePullMode,          L"{c77fb921-eaed-4c75-8edf-de46b02b1291}")
    DEVICE_TEST_PARENT( L"",    L"Glitch Testing",                                    0, 0,                                                      0, 0, NULL,                                 L"{4129F8D8-BF04-412C-B389-3B485962EBAE}")
    DEVICE_TEST_METHOD( L"3.0", L" Verify n-stream render is glitch free",            0, DT_ENDPOINT_RENDER,                                     0, 0, TC_VerifyMultiRenderGlitchFree,       L"{f4e9a926-fcb3-4984-ac05-cea487fbac93}")
    DEVICE_TEST_METHOD( L"3.1", L" Verify n-stream capture is glitch free",           0, DT_ENDPOINT_CAPTURE,                                    0, 0, TC_VerifyMultiCaptureGlitchFree,      L"{CC68438A-F064-467A-A8FF-D49620A534DC}")
    DEVICE_TEST_METHOD( L"3.2", L" Verify n-stream stressed render is glitch free",   0, DT_ENDPOINT_RENDER,                                     0, 0, TC_VerifyStressedRenderGlitchFree,    L"{C34D871B-5BCF-4DA7-9046-92284272097A}")
    DEVICE_TEST_METHOD( L"3.3", L" Verify n-stream stressed capture is glitch free",  0, DT_ENDPOINT_CAPTURE,                                    0, 0, TC_VerifyStressedCaptureGlitchFree,   L"{06464A4E-13F9-409F-878A-CFA17CECE13E}")
END_TEST_CASE_INFO(AudioLogoTaef)

#ifndef BUILD_TAEF
// ------------------------------------------------------------------------------
// Shell98 extern
CTestModule* WINAPI NewTestModule
(
    CTestShell* pShell,
    HINSTANCE   hInstDLL
)
{
    g_IShell = (ITestShell*)pShell;
    g_pAudioLogoDeviceTestModule = new CAudioLogoDeviceTestModule(pShell, hInstDLL);
    return (CTestModule*)g_pAudioLogoDeviceTestModule;
}   //   NewTestModule()
#endif

//--------------------------------------------------------------------------;
//
//  CAudioLogoDevice::CAudioLogoDevice
//
//  Description:
//      Generic constructor for AudioLogo device.
//
//  Arguments:
//      LPCWSTR pszName: Name of device.
//
//      LPCWSTR pszId: (Pnp) Id of device.
//
//      LPARAM lpType: Type of device (Most likely DT_DEVICE_OTHER).
//
//      HRESULT &hr: HRESULT to bubble out.
//
//  Return (None):
//
//
//--------------------------------------------------------------------------;
CAudioLogoDevice::CAudioLogoDevice
(
    LPCWSTR      pszName,
    LPCWSTR      pszId,
    LPARAM      lpType,
    HRESULT    &hr
):
    m_pIEndpoint(NULL)
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
}   // CAudioLogoDevice::CAudioLogoDevice()


//--------------------------------------------------------------------------;
//
//  CAudioLogoDevice::CAudioLogoDevice
//
//  Description:
//      Constructor for AudioLogo device (endpoints).
//
//  Arguments:
//      IMMDevice *pIEndpoint: Endpoint from device enumerator.
//
//      HRESULT &hr: HRESULT to bubble out.
//
//  Return (None):
//
//
//--------------------------------------------------------------------------;
CAudioLogoDevice::CAudioLogoDevice
(
    IMMDevice      *pIEndpoint,
    HRESULT        &hr
):
    m_pIEndpoint(pIEndpoint)
{
    CComPtr<IMMDevice>          pDevnode = NULL;
    CComPtr<IMMEndpoint>        pIEP = NULL;
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

    if (S_OK != (hr = GetEndpointDevnode(pIEndpoint, &pDevnode)))
    {
        XLOG(XFAIL, eError, L"Failed GetEndpointDevnode (hr = %s)", GetHRString(hr));
        return;
    }

    if (S_OK != (hr = GetDevnodeInstanceId(&pszInstanceId, pDevnode)))
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

    if (S_OK != (hr = GetEndpointDisplayNameW(&pszName, pIEndpoint)))
    {
        XLOG(XFAIL, eError, L"Failed to get endpoint display name (hr = %s)", GetHRString(hr));
        return;
    }

    if (S_OK != (hr = GetEndpointDescriptionW(&pszDesc, pIEndpoint)))
    {
        XLOG(XFAIL, eError, L"Failed to get endpoint description (hr = %s)", GetHRString(hr));
        return;
    }
    
    hr = StringCbPrintfW(
                m_szRep,
                sizeof(m_szRep),
                L"%ls (%ls)",
                (LPCWSTR)pszDesc,
                (LPCWSTR)pszName);

    return;
}   // CAudioLogoDevice::CAudioLogoDevice()

//--------------------------------------------------------------------------;
//
//  CAudioLogoDevice::DoesDeviceMatch
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
//
//--------------------------------------------------------------------------;

BOOL CAudioLogoDevice::DoesDeviceMatch(const WCHAR *szSelectedId)
{
    if (DT_DEVICE_OTHER == m_lpType)
    {
        // always match generic device
        return TRUE;
    }
    else if (0 == _wcsicmp(m_szPnPId, szSelectedId))    
    {
        // try to match the device instance ID of the selected device.    
        return TRUE;
    }

    return FALSE;
}

//--------------------------------------------------------------------------;
//
//  CAudioLogoDeviceTestModule::CAudioLogoDeviceTestModule
//
//  Description:
//      Constructor for AudioLogo Test Module for Shell98
//
//  Arguments:
//      CTestShell *pShell: Standard Shell98 parameters
//
//      HINSTANCE  hInstDll: Standard Shell98 parameters
//
//  Return (None):
//
//
//--------------------------------------------------------------------------;
#ifndef BUILD_TAEF
CAudioLogoDeviceTestModule::CAudioLogoDeviceTestModule
(
    CTestShell     *pShell,
    HINSTANCE       hInstDll
):
    CDeviceTestModule(pShell, hInstDll),
    m_pDevice(NULL)
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
CAudioLogoDeviceTestModule::CAudioLogoDeviceTestModule ( DV_TCINFO * ptcInfo ):
CDeviceTestModule(),
    m_pDevice(NULL)
{
    m_rgTestCaseInfo = ptcInfo;
    m_cTestCases     = 1;
    
    // Do we have a TAEF parameter to select the device?
    String targetDevice;
    if (SUCCEEDED(RuntimeParameters::TryGetValue(L"InstanceId", targetDevice)))
    {
        g_IShell->SetTestDevice(targetDevice);
    }
}
#endif

//--------------------------------------------------------------------------;
//
//  CAudioLogoDeviceTestModule::~CAudioLogoDeviceTestModule
//
//  Description:
//      Destructor for AudioLogo Test Module for Shell98
//
//  Arguments:
//      None.
//
//  Return (None):
//
//
//--------------------------------------------------------------------------;
CAudioLogoDeviceTestModule::~CAudioLogoDeviceTestModule(VOID)
{
}   //  CAudioLogoDeviceTestModule::~CAudioLogoDeviceTestModule()


//--------------------------------------------------------------------------;
//
//  DWORD CAudioLogoDeviceTestModule::OnInitialUpdate
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
//
//--------------------------------------------------------------------------;
DWORD CAudioLogoDeviceTestModule::OnInitialUpdate(VOID)
{
    CComPtr<IMMDeviceEnumerator>    pDevEnum = NULL;
    CComPtr<IMMDeviceCollection>    pEndpoints = NULL;
    CAudioLogoDevice               *pDevice = NULL;
    UINT                            iEndpoint;
    HRESULT                         hr = S_OK;

    //  Adding generic device...
    hr = S_OK;

    pDevice = new CAudioLogoDevice(L"Generic Audio Device", L"Generic Pnp Id", DT_DEVICE_OTHER, hr);

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

        pDevice = new CAudioLogoDevice(pIEndpoint, hr);

        if (NULL == pDevice)
        {
            return (FNS_FAIL);
        }

        if (S_OK != hr)
        {
            XLOG(XMSG, eError, L"Failed to create an instance of CAudioLogoDevice for audio endpoint device at index %d (hr = %s).", (iEndpoint - 1), GetHRString(hr));
            delete pDevice;
            return (FNS_FAIL);
        }

        pDevice->m_fSelected = TRUE;
        g_IShell->AddDevice(pDevice);
    }

    return FNS_PASS;
}   //   CAudioLogoDeviceTestModule::OnInitialUpdate()


//--------------------------------------------------------------------------;
//
//  VOID CAudioLogoDeviceTestModule::SetTestDevice
//
//  Description:
//      Callback function for Shell98 apps for setting the device.
//
//  Arguments:
//      CDeviceDescriptor *pDevice: Device set by Shell98
//
//  Return (None):
//
//
//--------------------------------------------------------------------------;
VOID CAudioLogoDeviceTestModule::SetTestDevice
(
    CDeviceDescriptor      *pDevice
)
{
    m_pDevice = (CAudioLogoDevice*)pDevice;
}   //   CAudioLogoDeviceTestModule::SetTestDevice()


//--------------------------------------------------------------------------;
//
//  DWORD CAudioLogoDeviceTestModule::RunTest
//
//  Description:
//      Wrapper for running each test case.
//
//  Arguments:
//      DWORD dwTestCase: Index of test case.
//
//  Return (None):
//
//
//--------------------------------------------------------------------------;
DWORD CAudioLogoDeviceTestModule::RunTest
(
    DWORD   dwTestCase
)
{
    DWORD dwRes = FNS_PASS;
    dwRes = ((PDV_TCINFO)m_rgTestCaseInfo)[dwTestCase].pfnTest();
    return dwRes;
}   //   CAudioLogoDeviceTestModule::RunTest()
