//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
// Copyright(C) Microsoft.All rights reserved.
//
//  File:       gaudit.cpp
//
//
//--------------------------------------------------------------------------

#include "common.h"
#include "strsafe.h"
#include "drmstruct.h"
#include <shlwapi.h>

// to add timebomb - add line to sources - C_DEFINES=$(C_DEFINES) -DTIME_BOMB
#ifdef TIMEBOMB
#define DAYS_UNTIL_EXPIRATION   180 // chage days limit as per requirement. For UAA it was asked to have 6 months limits
#include <timebomb.h>
#endif

// ------------------------------------------------------------------------------
#define SZSHELLAPP  "General Audio Test 1.10"

// ------------------------------------------------------------------------------
// Shell98 globals.
CGauditModule*              g_pGauditModule;
CFramework                    g_Framework;
ULONG                        g_nIndentLevel = 0;
static  LPSTR szModuleName = SZSHELLAPP;

#ifndef BUILD_TAEF
ITestShell*             g_IShell;
#else 
#include <basicloghelper.h>

#define g_pModule g_pGauditModule

CTaefShellStub g_IShellObject;
CTaefShellStub * g_IShell = &g_IShellObject;
CTaefShellStub * g_pShell = g_IShell;
IBasicLog * g_pBasicLog = NULL;
using namespace WEX::Common;
using namespace WEX::TestExecution;
#endif

#include "testcases.h"

// ------------------------------------------------------------------------------
// Gaudit extended test information
KSDATAFORMAT_WAVEFORMATEX AudioPinFormat;
KSDATAFORMAT_DSOUND DsoundPinFormat;
KSDATAFORMAT MidiPinFormat;  
ULONG PinType;

#define SZDEVICES         "MSFTDevicesFlag"
DWORD g_Devices = 0;        // used for the device selection depeding upon the value set into profile
//-------------------------------------------------------------------------------
// Algorithm function from framework.cpp
extern DWORD TestAlgorithm(PGAUDITTESTFCT, PGAUDITPARAMS);

#ifndef BUILD_TAEF
// ------------------------------------------------------------------------------
// Shell98 extern
CTestModule* WINAPI
NewTestModule
(
    CTestShell* pShell,
    HINSTANCE   hInstDLL
)
{
    g_IShell = (ITestShell*)pShell;
    g_pGauditModule = new CGauditModule(pShell, hInstDLL);
    return (CTestModule*)g_pGauditModule;
}
#endif

//--------------------------------------------------------------------------
//  CGauditDevice constructor
CGauditDevice::CGauditDevice(CKsFilter* pFilter, DWORD dwFlags)
{
    m_dwFlags  = dwFlags;    
    m_pFilter = pFilter;
    _tcsncpy(m_szRep, pFilter->m_szFriendlyName, 256);
    m_lpType = 0;
    m_fSelected = 1;
    _tcsncpy(m_szPnPId, pFilter->m_szInstanceID, 256);
}

//--------------------------------------------------------------------------
//  CGauditDevice desstructor
CGauditDevice::~CGauditDevice()
{
    _ASSERT(m_pFilter);
    delete m_pFilter;
}
// code for removing duplicate filters from the list of filters
inline BOOL AreDuplicateFilters(CKsFilter* p1,CKsFilter* p2)
{
    return
    (
        (p1 != NULL) &&
        (p2 != NULL) &&
        (p1 != p2) &&
        (strcmp(p1->m_szFilterName, p2->m_szFilterName) == 0)
    );
}
void RemoveFilters(TTList<CKsFilter>* pFiltersList) {
    TTNode<CKsFilter>* pnFilter;
    TTNode<CKsFilter>*   pnNext;
    CKsFilter*          pFilter;

    TTNode<CKsFilter>*   pnDup;
    TTNode<CKsFilter>*   pnDupNext;
    CKsFilter*          pDup;

    pnFilter = pFiltersList->GetHead();
    while (pnFilter)    {
        pFilter = pnFilter->pData;

        pnDup = pFiltersList->GetHead();
        while (pnDup)        {
            pDup = pnDup->pData;
            pnDupNext = pFiltersList->GetNext(pnDup);
            if (AreDuplicateFilters(pDup, pFilter)) {     //there is a condition in AreDuplicateFilters which will make sure that we are not comparing with one itself 
                pFiltersList->Remove(pnDup);
                delete pDup;
            }
            pnDup = pnDupNext;
        }
        pnFilter = pFiltersList->GetNext(pnFilter);
    }
    pnFilter = pFiltersList->GetHead();

    struct FilterDesc {
        LPCTSTR szFriendlyName;
        LPCTSTR szHardwareID;
    };

    FilterDesc KnownInternalMicrosoftFilters[] = {
#if (NTDDI_VERSION >= NTDDI_VISTA)
        // Vista and future
        {   _T("Microsoft Streaming Tee/Sink-to-Sink Converter"),
            _T("sw\\{cfd669f1-9bc2-11d0-8299-0000f822fe8a}")
        },
#endif
#if (NTDDI_WINXP <= NTDDI_VERSION && NTDDI_VERSION < NTDDI_VISTA)
        // XP-only
        {   _T("Microsoft Kernel Audio Splitter"),
            _T("sw\\{2f412ab5-ed3a-4590-ab24-b0ce2aa77d3c}")
        },
        {   _T("Microsoft WINMM WDM Audio Compatibility Driver"),
            _T("sw\\{cd171de3-69e5-11d2-b56d-0000f8754380}")
        },
        {   _T("Microsoft Kernel DLS Synthesizer"),
            _T("sw\\{8c07dd50-7a8d-11d2-8f8c-00c04fbf8fef}")
        },
        {   _T("Microsoft Kernel GS Wavetable Synthesizer"),
            _T("sw\\{6c1b9f60-c0a9-11d0-96d8-00aa0051e51d}")
        },
        {   _T("Microsoft Kernel Wave Audio Mixer"),
            _T("sw\\{b7eafdc0-a680-11d0-96d8-00aa0051e51d}")
        },
        {   _T("Microsoft Kernel System Audio Device"),
            _T("sw\\{a7c7a5b0-5af3-11d1-9ced-00a024bf0407}")
        },
        {   _T("Microsoft Kernel Acoustic Echo Canceller"),
            _T("sw\\{4245ff73-1db4-11d2-86e4-98ae20524153}")
        },
#endif
        // XP and Vista and future
        {   _T("Microsoft Kernel DRM Audio Descrambler"),
            _T("sw\\{eec12db6-ad9c-4168-8658-b03daef417fe}")
        },
        {   _T("Microsoft Kernel Audio Splitter"),
            _T("sw\\{eec12db6-ad9c-4168-8658-b03daef417fe}")
        },
        {   _T("Microsoft Streaming Clock Proxy"),
            _T("sw\\{97ebaacc-95bd-11d0-a3ea-00a0c9223196}")
        },
        {   _T("Microsoft Streaming Quality Manager Proxy"),
            _T("sw\\{ddf4358e-bb2c-11d0-a42f-00a0c9223196}")
        },
        {   _T("Microsoft Streaming Service Proxy"),
            _T("sw\\{96e080c7-143c-11d1-b40f-00a0c9223196}")
        }
    };

    while (pnFilter)    {
        pFilter = pnFilter->pData;
        pnDupNext = pFiltersList->GetNext(pnFilter);
        BOOL fIsInternalMicrosoftFilter = FALSE;

        // check for known Microsoft filters
        for (int i = 0; i < ARRAYSIZE(KnownInternalMicrosoftFilters); i++) {
            FilterDesc fd = KnownInternalMicrosoftFilters[i];

            // the friendly name and the hardware ID have to match
            if (
#pragma prefast(suppress:38001, "The string under consideration is not localized")
                (0 == StrCmpI(fd.szFriendlyName, pFilter->m_szFriendlyName)) &&
                (0 == StrCmpI(fd.szHardwareID, pFilter->m_szHardwareID))
            ) {
                fIsInternalMicrosoftFilter = TRUE;
                break; // for
            }
        }

        if (
            (1 == g_Devices && !fIsInternalMicrosoftFilter) || // we only want to test internal MS filters
            (0 == g_Devices && fIsInternalMicrosoftFilter) // we want to test everything but internal MS filters
        ) {
            pFiltersList->Remove(pnFilter);
            delete pFilter;
        }
        pnFilter = pnDupNext;
    }
}
//--------------------------------------------------------------------------
//  CGauditModule constructor
#ifndef BUILD_TAEF
CGauditModule::CGauditModule(CTestShell *pShell, HINSTANCE hInstDll) : 
               CDeviceTestModule(pShell, hInstDll),
               m_pDevice(NULL),
               m_hInstance(0)
{
    m_dwAppID        = 200;       
    m_hInstance       = hInstDll;
    m_pstrModuleName = szModuleName;
    m_nIconID          = APPICON;

    m_rgTestCaseInfo    = rgTestCaseInfo;
    m_cTestCases          = ARRAY_ELEMENTS(rgTestCaseInfo);
    m_dwWHQLChapter  = WHQL_CHAPTER_AUDIO;

    m_hKsUserDll    = NULL;
    m_fnCreateClock = NULL;
    m_fnCreatePin = NULL;
    
} // CGauditModule
#else
CGauditModule::CGauditModule(DV_TCINFO * ptcInfo) : 
               CDeviceTestModule(),
			   m_pDevice(NULL),
			   m_hInstance(0)
{

    m_rgTestCaseInfo = ptcInfo;
    m_cTestCases     = 1;

    m_hKsUserDll    = NULL;
    m_fnCreateClock = NULL;
    m_fnCreatePin   = NULL;
    
    // Do we have a TAEF parameter to select the device?
    String targetDevice;
    if (SUCCEEDED(RuntimeParameters::TryGetValue(L"InstanceId", targetDevice)))
    {
        g_IShell->SetTestDevice(targetDevice);
    }
    
} // CGauditModule
#endif

//--------------------------------------------------------------------------
//  ~CGauditModule destructor

CGauditModule::~CGauditModule(void)
{
    KslCloseKsLib();
    if (m_hKsUserDll)
        FreeLibrary(m_hKsUserDll);
}

//--------------------------------------------------------------------------
//  CGauditModule::OnInitialUpdate handler

DWORD CGauditModule::OnInitialUpdate(void)
{

#ifndef WHQL
    SLOG(eInfo1, "Contact stroaud for any problem with this application.");        
#endif

    if (!StaticAllocatorInit())
    {
        SLOG(eError, "FATAL ERROR: Failed initializing static allocator");
          return FNS_FAIL;
    }

    if (!KslInitKsLib())
    {
        SLOG(eError, "FATAL ERROR: Failed initializing kslib");
          return FNS_FAIL;
    }

    // hook shell to kslib
    CComPtr<IBasicLog> pBasicLog;
    g_IShell->GetBasicLog(&pBasicLog);
    KslRegisterLog(pBasicLog);

//        g_Devices = 0;      // this will be used to select device from the profile
    LoadSettings();
        
    if (!EnumerateDevices())
    {
        SLOG(eError, "FATAL ERROR: Failed to enumerate devices");
          return FNS_FAIL;
    }


    TCHAR szFullDllPath[ MAX_PATH];
       
    UINT rVal = GetSystemDirectory( szFullDllPath, MAX_PATH);
    if (!rVal)
        return FNS_FAIL;
    if (SUCCEEDED( StringCchCat( szFullDllPath, rVal+1 + 11, _T("\\ksuser.dll") )))  {
        m_hKsUserDll = LoadLibrary(szFullDllPath);
    } 
    else 
        return FNS_FAIL;
       
    if (!m_hKsUserDll)
    {
        SLOG(eError, "FATAL ERROR: Failed loading ksuser.dll");
        return FNS_FAIL;
    }

    m_fnCreatePin = (KSCREATEPIN)GetProcAddress(m_hKsUserDll, TEXT("KsCreatePin"));
    m_fnCreateClock = (KSCREATECLOCK)GetProcAddress(m_hKsUserDll, TEXT("KsCreateClock"));

    if (!m_fnCreateClock || !m_fnCreatePin)
    {
        SLOG(eError, "FATAL ERROR: Ksuser.dll export functions have changed");
        return FNS_FAIL;
    }
// Disabling alignment execption for 64 bity os
    #ifdef _WIN64
    SetErrorMode(SEM_NOALIGNMENTFAULTEXCEPT);
    #endif
       
    return FNS_PASS;
}

//--------------------------------------------------------------------------
//  CGauditModule::SetTestDevice handler

void CGauditModule::SetTestDevice(CDeviceDescriptor* pDevice)
{
    m_pDevice = (CGauditDevice*)pDevice;
}

//--------------------------------------------------------------------------
//  CGauditModule::EnumerateDevices - device discovery 

BOOL CGauditModule::EnumerateDevices()
{
    BOOL fResult = FALSE;
    TTList<CKsFilter> Filters;
    TTNode<CKsFilter>* pnFilter;

    
    BOOL fRes;

#if (NTDDI_VERSION < NTDDI_VISTA)
    LPGUID guidSysaudio[]   = { const_cast<LPGUID>(&KSCATEGORY_SYSAUDIO) };
    LPGUID guidWdmaud[]    = { const_cast<LPGUID>(&KSCATEGORY_WDMAUD) };
#endif
    LPGUID guidAudio[]         = { const_cast<LPGUID>(&KSCATEGORY_AUDIO) };
    SLOG (eInfo2,"value of the device flag = %d", g_Devices);
    if (g_Devices)  
        SLOG (eInfo2, "Gaudit will run on only MSFT components");
    else
        SLOG (eInfo2, "Gaudit will run on only external party drivers");    
    // Enumerate audio devices
    if (!KslEnumFilters(&Filters, ePCMAudio, guidAudio, 1))
        goto cleanup;

    // Enumerate audio devices for Topology Nodes / Pins
    if (!KslEnumFilters(&Filters, eTopology, guidAudio, 1, TRUE, TRUE, FALSE))
        goto cleanup;

    // Enumerate audio devices for DMusic Nodes / Pins
    if (!KslEnumFilters(&Filters, eDMusic, guidAudio, 1, TRUE, TRUE, FALSE))
        goto cleanup;
    RemoveFilters (&Filters);
    FOR_EACH_FILTER(Filters, pnFilter)
        CGauditDevice* pDevice = new CGauditDevice(pnFilter->pData, 0);
        if (!pDevice)
            goto cleanup;
        g_IShell->AddDevice(pDevice);
    END_FOR_EACH()
    Filters.Empty();

#if (NTDDI_VERSION < NTDDI_VISTA)
    // Enumerate sysaudio devices
   if (!KslEnumFilters(&Filters, ePCMAudio, guidSysaudio, 1, FALSE, FALSE, TRUE))
        goto cleanup;

   RemoveFilters (&Filters);        
    _ASSERT(Filters.GetCount() == 1);
    FOR_EACH_FILTER(Filters, pnFilter)
        CGauditDevice* pDevice = new CGauditDevice(pnFilter->pData, SYSAUDIO_DEVICE);
     if (!pDevice)
            goto cleanup;
        g_IShell->AddDevice(pDevice);
    END_FOR_EACH()
    Filters.Empty();
    // Enumerate wdmaud devices
    if (!KslEnumFilters(&Filters, ePCMAudio, guidWdmaud, 1, FALSE, FALSE, TRUE))
        goto cleanup;

    RemoveFilters (&Filters);        
    // Parse wdmaud 
    FOR_EACH_FILTER(Filters, pnFilter)
        CGauditDevice* pDevice = new CGauditDevice(pnFilter->pData, WDMAUD_DEVICE);
        if (!pDevice)
            goto cleanup;
        g_IShell->AddDevice(pDevice);
    END_FOR_EACH()
#endif

    fResult = TRUE;

cleanup:
    if (!fResult)
        CLEANUP_FILTER_LIST(Filters)

    return fResult;
}

//--------------------------------------------------------------------------
//  CGauditModule::EndTestRun 

DWORD   CGauditModule::EndTestRun(void)
{

    return FNS_PASS;
}
//--------------------------------------------------------------------------
//  CGauditModule::StartTestRun 

DWORD   CGauditModule::StartTestRun(void)
{
    return FNS_PASS;
}

//--------------------------------------------------------------------------
//  CGauditModule::LoadSettings 

void  CGauditModule::LoadSettings(void)
{
        USES_CONVERSION;
#ifndef BUILD_TAEF
        g_Devices = g_IShell->GetProfileInt(SZDEVICES, g_Devices, TRUE );
#endif

}



//--------------------------------------------------------------------------
//  CGauditModule::RunTest

DWORD   CGauditModule::RunTest(DWORD dwTestCase)
{
    GAUDITPARAMS params = { 0 };

    // We don't care about shell98 stuff for indent
    g_nIndentLevel = 0;

    _ASSERT(dwTestCase < m_cTestCases);
    DV_TCINFO * pCase = &((DV_TCINFO*)m_rgTestCaseInfo)[dwTestCase];

    // set test definition
    m_pTestDef = (PGAUDITTESTDEF)(pCase->lpUser);

    params.cbOutSize = m_pTestDef->cbOutSize;

    // fill as much information as it is available
    switch (m_pTestDef->ulGauditFlags & MASK_GOF)
    {
    case GOF_PROP:
        params.Prop.Set = m_pTestDef->KsProp.guidPropSet;
        params.Prop.Id  = m_pTestDef->KsProp.ulPropId;
        break;
    case GOF_ROOT:
        _ASSERT(FALSE);
        break;
    case GOF_IOCTL:
        params.Ioctl = m_pTestDef->Ioctl.ulIoctl;
        break;
    default:
        _ASSERT(FALSE);
    }

    // call algorithm passing the right function 
    return TestAlgorithm((PGAUDITTESTFCT)pCase->pfnTest, &params);
}

CFramework* CGauditModule::GetFramework() const { return &g_Framework; }

//--------------------------------------------------------------------------
// CFramework implementation
CFramework::CFramework() 
{ 
    Reset(); 
}

void CFramework::Reset()
{
    m_ulDeviceId = (ULONG)-1;
    m_pFilter = NULL; 
    m_pPin = NULL; 
    m_pNode = NULL;                   
}
void CFramework::NewFilter(CKsFilter* pFilter)
{
    Reset();
    m_pFilter = pFilter;
}

void CFramework::NewPin(CKsPin* ppin)
{
    CKsFilter* pSavedFilter = m_pFilter;
    Reset();
    m_pFilter = pSavedFilter;
    m_pPin = ppin;
}

void CFramework::NewNode(CKsNode* pnode)
{
    CKsFilter* pSavedFilter = m_pFilter;
    Reset();
    m_pFilter = pSavedFilter;
    m_pNode = pnode;
}

void CFramework::NewDevice(ULONG DeviceId)
{
    CKsFilter* pSavedFilter = m_pFilter;
    Reset();
    m_pFilter = pSavedFilter;
    m_ulDeviceId = DeviceId;
}

void CFramework::NewNodePin(CKsNode* pNode)
{
    CKsFilter* pSavedFilter = m_pFilter;
    CKsPin* pSavedPin = m_pPin;
    Reset();
    m_pFilter = pSavedFilter;
    m_pPin = pSavedPin;
    m_pNode = pNode;
}
