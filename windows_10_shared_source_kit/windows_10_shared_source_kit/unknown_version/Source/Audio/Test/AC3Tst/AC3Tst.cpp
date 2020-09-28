// ------------------------------------------------------------------------------
//
// Copyright (C) Microsoft. All rights reserved.
//
// Module Name:
//
//  AC3Tst.cpp
//
// Abstract:
//
//  Implementation file for most shell functionality, including most CAc3Test 
//  methods.  Test cases should NOT be put into this file.
//



// -------------------------------------------------------------------------------


#include "AC3Tst.h"
#include "resource.h"
#include <kslib.h>
#include <mmreg.h>
#include <mmdeviceapi.h>
#include <BasicLogHelper.h>

// ------------------------------------------------------------------------------
// Globals
WAVERESOURCE    g_wrAC3;
WAVERESOURCE    g_wrPCM;
OSVERSIONINFO   g_OSVersionInfo = { 0 };
BOOL            g_fOnNT = FALSE;

// ------------------------------------------------------------------------------
// The table of test cases
// ------------------------------------------------------------------------------
/*  For reference (see dvmodule.h)
typedef struct dv_tcinfo_tag
{
	LPSTR       pszCaseID;      // shell    - test case ID
    LPSTR       pszName;        // shell    - test case name
    LPARAM      lpCaseFlags;    // shell    - test case flags
    LPARAM      lpDeviceFlags;  // shell    - device type flags
    LPARAM      lpSTFlags;      // selftest - server/cable/input flags
    LPARAM      lpUser;         // User-defined field
    TESTFNPROC  pfnTest;        // shell    - Pointer to the test case function
    LPSTR       pszGuid;        // shell    - test case GUID identifier
} DV_TCINFO, *PDV_TCINFO;
*/
DV_TCINFO rgTestCaseInfo[] =
{
    { "",     "KS Pin Tests",             0,  0,            0,  0,  NULL,                      "{1386014c-28b7-442d-9e64-82c3aa211423}" },
    { "1.1",  " DataRanges",              0,  DEV_KSRENDER, 0,  0,  Tc_DataRanges,             "{5e25b35c-8aa9-4a92-9ef5-a72c87a17f47}" },
    { "1.2",  " KsCreatePin",             0,  DEV_KSRENDER, 0,  0,  Tc_KsCreatePin,            "{ea9ec222-0865-4ccd-ac36-fe4450cfb222}" },
    { "1.3",  " KsCreatePin Multiple",    0,  DEV_KSRENDER, 0,  0,  Tc_KsCreatePinMulti,       "{e6c5f955-cd53-481f-9599-5468ab9ce540}" },
    { "1.4",  " WriteStream",             0,  DEV_KSRENDER, 0,  0,  Tc_KsWriteStream,          "{9f2be9a4-a50b-49d6-8402-4634b9a6288f}" },
    { "1.5",  " KS_STATE",                0,  DEV_KSRENDER, 0,  0,  Tc_KsState,                "{c3464104-a07b-427a-bbc6-92d636a68e6d}" },
    { "1.6",  " GetPosition",             0,  DEV_KSRENDER, 0,  0,  Tc_KsGetPosition,          "{9579b136-9b92-4f2c-b43d-c963384e19d3}" },
    { "1.8",  " Pause/Restart Test",      0,  DEV_KSRENDER, 0,  0,  Tc_Regression183493,       "{fe1929d6-da65-42a9-b9e7-683250ab061e}" },
    
    // this doesn't seem to be a valid test case.  See Windows 447039
    //
    // rsaad - 2006-06-08
    // After discussing the matter with kencoope and referring to bug 447039 
    // and the tracking bug for this issue 1273323, this test has been deprecated 
    // because SetPosition() is not being supported anymore by all the following 
    // drivers: Wave RT, WavePCI, USB Audio
    //{ "1.7",  " GetSetPosition",          0,  DEV_KSRENDER, 0,  0,  Tc_KsGetSetPosition,       "{6dc1e99d-60f8-4ad4-84b6-98af597e9c52}" },

    { "",     "waveOut Tests",            0,  0,            0,  0,  NULL,                      "{554dd77c-6026-4941-a33c-b80b1f8070cc}" },
    { "2.1",  " Open",                    0,  DEV_WAVEOUT,  0,  0,  Tc_woOpen,                 "{250d5218-154d-4457-b01a-e3fe2afb8cf8}" },
    { "2.2",  " Open Multiple",           0,  DEV_WAVEOUT,  0,  0,  Tc_woOpenMulti,            "{20938378-b789-4809-947d-105659c2162e}" },
    { "2.3",  " Write",                   0,  DEV_WAVEOUT,  0,  0,  Tc_woWrite,                "{2e5a1341-c566-42d3-84da-d7e1218681e6}" },
#ifndef WHQL
    { "2.4",  " Write looped",            0,  DEV_WAVEOUT,  0,  0,  Tc_woWriteLooped,          "{43e3df7a-098a-47b9-8943-72947ec01f3d}" },
#endif
    { "2.6",  " Pause / Restart",         0,  DEV_WAVEOUT,  0,  0,  Tc_woPause,                "{e7b300cb-c655-47db-a0ca-6cf57f16c825}" },
    { "2.7",  " Reset",                   0,  DEV_WAVEOUT,  0,  0,  Tc_woReset,                "{875ff6ae-7700-4127-afa1-02f9d8f75580}" },
    { "2.8",  " waveOutGetPosition",      0,  DEV_WAVEOUT,  0,  0,  Tc_woGetPosition,          "{27098452-4440-4f42-8785-f647eb23fa8b}" },
#ifndef WHQL
    { "2.9",  " PCM and AC3 (AC3 first)", 0,  DEV_WAVEOUT,  0,  0,  Tc_woOpenAC3thenPCM,       "{53bcb6c9-b962-4c5e-952a-54f075b33ad4}" },
    { "2.10", " PCM and AC3 (PCM first)", 0,  DEV_WAVEOUT,  0,  0,  Tc_woOpenPCMthenAC3,       "{6d26befc-a26b-4588-a358-c863bdf1a909}" },
#endif

    // rsaad - 2007-01-17
    // In Vista, DSound tests don't test any direct hardware paths anymore. DSound is only
    // a software layer similar to Wave, Mixer and other APIs. These tests will thus be removed
    // from AC3Test.
    //{ "",     "DirectSound Tests",        0,  0,            0,  0,  NULL,                      "{4ee4aaf0-0371-47ac-aa15-79645ea851e7}" },
    //{ "3.1a", " CreateSoundBuffer (1)",   0,  DEV_DS,       0,  0,  Tc_CreateSoundBufferPrimary,   "{593c8bb6-552c-42bf-bd1d-a18731136c6b}" },
    //{ "3.1b", " CreateSoundBuffer (2)",   0,  DEV_DS,       0,  0,  Tc_CreateSoundBufferSecondary, "{05c98a98-5196-43e6-920e-0d6a106cba61}" },
    //{ "3.2",  " DuplicateSoundBuffer",    0,  DEV_DS,       0,  0,  Tc_DuplicateSoundBuffer,   "{994a884f-0b38-4a7c-b321-cc9dc1156024}" },
    //{ "3.3",  " Play",                    0,  DEV_DS,       0,  0,  Tc_Play,                   "{8838edc0-fcbc-472b-bd63-4f95b884ed28}" },
    //{ "3.4",  " Stop",                    0,  DEV_DS,       0,  0,  Tc_Stop,                   "{660090d2-5e38-485e-a9ec-067e16f10a67}" },
    //{ "3.5",  " GetCurrentPosition",      0,  DEV_DS,       0,  0,  Tc_GetCurrentPosition,     "{aa688425-93a1-4821-86c7-429ff657fb44}" },
    //{ "3.6",  " Get/SetCurrentPosition",  0,  DEV_DS,       0,  0,  Tc_GetSetCurrentPosition,  "{a7a5f749-ed9c-41b3-82dc-d578e6ee6f56}" },
    //{ "3.8",  " Get/SetPan",              0,  DEV_DS,       0,  0,  Tc_GetSetPan,              "{f8b6f1be-6fd9-44a4-a174-48db05a134a1}" },
    //{ "3.9",  " Get/SetVolume",           0,  DEV_DS,       0,  0,  Tc_GetSetVolume,           "{dd9115d2-f703-4d86-ba6e-f8a68c2ec916}" },
    //{ "3.10", " GetStatus",               0,  DEV_DS,       0,  0,  Tc_GetStatus,              "{c0958de2-ff90-4ffd-a168-7a23be3839b6}" },
//#ifndef WHQL
    //{ "3.11", " PCM and AC3 (AC3 first)", 0,  DEV_DS,       0,  0,  Tc_CreateBufferAC3thenPCM, "{2cc4b18c-d834-4eca-906b-9f9e87ede545}" },
    //{ "3.12", " PCM and AC3 (PCM first)", 0,  DEV_DS,       0,  0,  Tc_CreateBufferPCMthenAC3, "{5e032f7e-2ac2-4b23-83ce-8bc53d16eb53}" },
//#endif
};


//---------------------------------------------------------------------------
//  Shell98 / ModMgr98 stuff
// ------------------------------------------------------------------------------
// Notes:
//     The only thing that should need to be changed in this area is the 
//     derived class name, the module name, and the App ID.
// ------------------------------------------------------------------------------
static  LPSTR szModuleName = "AC-3 Driver Test";
static  DWORD dwAppID      = 34;

ITestShell* g_IShell;
CAc3Test*   g_pAC3Tst;

//---------------------------------------------------------------------------
// Function:   NewTestModule
// Purpose:    Called as the shell loads to setup the derived test module 
//             class inside the shell
// Arguments:  pShell      - Pointer to shell for tests' access
//             hInstDLL    -
// Returns:    CTestModule* to this test module class 
//---------------------------------------------------------------------------

CTestModule* WINAPI
NewTestModule
(
    CTestShell* pShell,
    HINSTANCE   hInstDLL
)
{
    g_IShell = (ITestShell*)pShell;
    g_pAC3Tst = new CAc3Test(pShell, hInstDLL);
    return (CTestModule*)g_pAC3Tst;
}


//---------------------------------------------------------------------------
// Function:   CAc3Test::CAc3Test
// Purpose:    Constructor for derived module class.  
// Arguments:  pShell      - Pointer to shell for tests' access
//             hInstance   -
// Returns:    None 
// Notes:      Additional Initialization for this module should _NOT_ be put here 
//---------------------------------------------------------------------------

CAc3Test::CAc3Test
(
    CTestShell* pShell,
    HINSTANCE   hInstance
)
 :  CDeviceTestModule(pShell, hInstance),
    m_pCurDevice(NULL),
    m_fLogPositions(TRUE),
    m_fGotKS(FALSE)
{
    m_dwAppID        = dwAppID;  
    m_pstrModuleName = szModuleName;
    m_rgTestCaseInfo = rgTestCaseInfo;
    m_cTestCases     = ARRAY_ELEMENTS(rgTestCaseInfo);
    m_nIconID        = APPICON;
}

//---------------------------------------------------------------------------
// Function:   CAc3Test::~CAc3Test
// Purpose:    Destructor for derived module class.  
// Arguments:  None
// Returns:    None 
// Notes:      Any clean up from the Initialize function should be put here.
//---------------------------------------------------------------------------
CAc3Test::~CAc3Test
(
    void
)
{
    // clean up kslib
    KslCloseKsLib();

    SafeLocalFree(g_wrAC3.pData);
    SafeLocalFree(g_wrPCM.pData);

    return;
}

//---------------------------------------------------------------------------
// Function:   CAc3Test::Initialize
// Purpose:    Initialize derived module class  
// Arguments:  None
// Returns:    0 = Success, otherwise failure 
// Notes:      Additional Initialization for this module should be put here 
//             Note that the shell window has not been created at this point
//             so you may not do any logging, or add menu items, etc.
//             Any initialization of that sort should be done in OnInitialUpdate
//---------------------------------------------------------------------------
DWORD
CAc3Test::Initialize
(
    void
)
{
    g_OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
#pragma warning(suppress:28159)
    /*
        warning 28159: Consider using 'IsWindows*' instead of 'GetVersionExA'. Reason: Deprecated. Use VerifyVersionInfo* or IsWindows* macros from VersionHelpers.
        problem occurs in function 'CAc3Test::Initialize'
    */
    GetVersionEx(&g_OSVersionInfo);

    g_fOnNT = (VER_PLATFORM_WIN32_NT == g_OSVersionInfo.dwPlatformId);

    return 0;
}

// ----------------------------------------------------------------------------------
// Purpose:    Initialize derived module class  
// Arguments:  None
// Returns:    0 = Success, otherwise failure 
// Notes:      Initialization for this module that requires logging should be put here 
// ----------------------------------------------------------------------------------
DWORD
CAc3Test::OnInitialUpdate
(
    void
)
{
    // Log some stuff
    SLOG(eWarn1, m_pstrModuleName);
    SLOG(eWarn1, "AppID = %d", m_dwAppID);
    SLOG(eWarn1, "");

    // init kslib
    m_fGotKS = KslInitKsLib();
    if (m_fGotKS) {
        IBasicLog* pBasicLog;
        g_IShell->GetBasicLog(&pBasicLog);
        if (NULL != pBasicLog) {
            KslRegisterLog(pBasicLog);
            pBasicLog->Release();
        }
    }
    else
        LOG(eWarn1, "Failed to initialize KsLib.  Testing will still be done through DirectSound and WaveOut APIs...");


    // enumerate the devices that this app will test
    EnumerateDevices();

    // load ac3.wav
    ZeroMemory(&g_wrAC3, sizeof(WAVERESOURCE));
    LoadWaveResource(g_IShell->m_hInstance, &g_wrAC3, WR_AC3);

    ZeroMemory(&g_wrPCM, sizeof(WAVERESOURCE));
    LoadWaveResource(g_IShell->m_hInstance, &g_wrPCM, WR_PCM);

    // 
    g_IShell->InstallMenuItem("&Options", "Log GetPosition results", IDM_LOG_POSITIONS);
    CheckMenuItem(GetMenu(g_IShell->m_hwndShell), IDM_LOG_POSITIONS, MF_BYCOMMAND | (m_fLogPositions ? MF_CHECKED : MF_UNCHECKED));

    return 0;
}

// ------------------------------------------------------------------------------
void
CAc3Test::ProcessMenuItem
( 
    DWORD   nID, 
    HWND    /*hWndCtl*/, 
    DWORD   /*codeNotify*/, 
    HMENU   hMenu
)
{
    switch (nID)
    {
        case IDM_LOG_POSITIONS:
        {
            m_fLogPositions = !m_fLogPositions;

            CheckMenuItem(hMenu, IDM_LOG_POSITIONS, MF_BYCOMMAND | (m_fLogPositions ? MF_CHECKED : MF_UNCHECKED));
            break;
        }
    }
}
// ----------------------------------------------------------------------------------
// Purpose:    Sets the current "Device" to be tested
// Arguments:  None
// Returns:    void
// Notes:      This is called during a test run as the shell loops through all selected
//             "devices".
// ----------------------------------------------------------------------------------
void    
CAc3Test::SetTestDevice
(
    CDeviceDescriptor* pDevice
)
{
    m_pCurDevice = pDevice;
}


// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
BOOL CALLBACK DSCallbackPopulate
(
	const GUID FAR		*lpGuid,
	LPCSTR				szDesc,
	LPCSTR				/*szModule*/,
	LPVOID				/*lpContext*/
)
{
    if (lpGuid)
    {
        CPCMPinDev* pKsPinDev = NULL;

        char szFilter[256];

        // temporarily store the corresponding filter name in the szPnPId
        GetKsFilterNameOfDSoundDevice((LPGUID)lpGuid, szFilter, 256);

        pKsPinDev = g_pAC3Tst->FindKsDevWithFilterName(szFilter);
        if (pKsPinDev)
        {
            CDSoundDev* pDSoundDev = new CDSoundDev((LPGUID)lpGuid, szDesc);

            if (pDSoundDev)
            {
                g_IShell->AddDevice(pDSoundDev);

                strcpy_s(pDSoundDev->m_szPnPId, pKsPinDev->m_szPnPId);
                pDSoundDev->m_cPossibleInstances = pKsPinDev->m_pPin->m_Descriptor.CInstancesGlobal.PossibleCount;
                pDSoundDev->m_fHaveVolumePan = FALSE;            // BUGBUG:  figure this out from pKsPinDev
            }
        }
    }

    return TRUE;
}

// ------------------------------------------------------------------------------

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
CPCMPinDev*
CAc3Test::FindKsDevWithFilterName
(
    LPCSTR pstrFilterName
)
{
    CPCMPinDev*         pPinDev;
    TTNode<CPCMPinDev>* pnPinDev;

    pnPinDev = m_listPinDevs.GetHead();

    while (pnPinDev)
    {
        pPinDev = pnPinDev->pData;
        pnPinDev = m_listPinDevs.GetNext(pnPinDev);

        if (pPinDev && (_stricmp(pPinDev->m_pPin->m_pFilter->m_szFilterName, pstrFilterName) == 0))
            return pPinDev;
    }

    return NULL;
}


// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
CPCMPinDev*
CAc3Test::FindKsDevWithDevNode
(
    DWORD_PTR   dwDevNodeID
)
{
    CPCMPinDev*         pPinDev;
    TTNode<CPCMPinDev>* pnPinDev;

    pnPinDev = m_listPinDevs.GetHead();

    while (pnPinDev)
    {
        pPinDev = pnPinDev->pData;
        pnPinDev = m_listPinDevs.GetNext(pnPinDev);

        if (pPinDev && (pPinDev->m_pPin->m_pFilter->m_dwDevInst == dwDevNodeID))
            return pPinDev;
    }

    return NULL;
}

// ------------------------------------------------------------------------------
// ------------------------------------------------------------------------------
CDSoundDev::CDSoundDev
(
    LPGUID pGuid, 
    LPCSTR strRep
)
 :  m_pGuid((LPGUID)pGuid),
    m_cPossibleInstances(1),
    m_fHaveVolumePan(FALSE)
{
    sprintf_s(m_szRep, "%s (DirectSound)", strRep);
    m_szPnPId[0] = 0;
    m_fSelected = TRUE;
    m_lpType = DEV_DSOUND;
}

CWinMMDev::CWinMMDev
(
    UINT    nID, 
    LPCSTR  strRep
)
 :  m_uDevID(nID),
    m_cPossibleInstances(1)
{
    sprintf_s(m_szRep, "%s (waveOut[%d])", strRep, nID);
    m_szPnPId[0] = 0;
    m_fSelected = TRUE;
    m_lpType = DEV_WAVEOUT;
}

CPCMPinDev::CPCMPinDev
(
    CKsPin* pPin
)
 :  m_pPin(pPin)
{
    sprintf_s(m_szRep, "KS Pin #%d on %s", m_pPin->m_nId, m_pPin->m_pFilter->m_szFriendlyName);
    memset(m_szPnPId, 0, sizeof(m_szPnPId));
    int len = sizeof(m_szRep)/sizeof(m_szRep[0]);
    strncpy_s(m_szPnPId, m_pPin->m_pFilter->m_szInstanceID, len - 1);
    m_fSelected = TRUE;
    m_lpType = DEV_KSRENDER;
}

// ----------------------------------------------------------------------------------
// Purpose:    Run prerequisite before running test funcion
// Arguments:  None
// Returns:    void
// Notes:      This is called during a test run as the shell loops through all test cases
// ----------------------------------------------------------------------------------
DWORD
CAc3Test::RunTest
(
    DWORD   dwTestID
)
{
    DWORD       dw;
    
    PDV_TCINFO rgTestCaseInfo = (PDV_TCINFO)m_rgTestCaseInfo;

    m_pShell->IncrementIndent();

    dw = ((rgTestCaseInfo[dwTestID].pfnTest)());

    m_pShell->DecrementIndent();

    return(dw);
}

// ------------------------------------------------------------------------------
// Purpose:    builds a list of devices to be tested
// Arguments:  None
// Returns:    void
// Notes:      This can be a bit tricky if you are testing multiple device types...
// ------------------------------------------------------------------------------
BOOL    
CAc3Test::EnumerateDevices
(
    void
)
{
    //
    // enumerate KS filters ~~~~~~~~~~~~~~~~~~
    //


    if (m_fGotKS)
    {
        /*BOOL                fRes;*/
        BOOL                fIsViableAC3;
        TTList<CKsFilter>   listFilters;
        LPGUID              arpguidCategories[2];
        ULONG               cNonSysaudio;

        arpguidCategories[0] = (LPGUID)&KSCATEGORY_AUDIO;
        arpguidCategories[1] = (LPGUID)&KSCATEGORY_RENDER;
        // 20051012 - jwexler - change from eUnknown to ePCMAudio so that we can 
        //  easily determine if it is an RTPort filter.
        /*fRes = */KslEnumFilters(&listFilters, ePCMAudio, arpguidCategories, 2);

        cNonSysaudio = listFilters.GetCount();
#if (NTDDI_VERSION < NTDDI_VISTA) //For Non-LH OS
    #ifndef WHQL
        arpguidCategories[0] = (LPGUID)&KSCATEGORY_AUDIO_DEVICE;
        /*fRes = */KslEnumFilters(&listFilters, eUnknown, arpguidCategories, 1);
    #endif
#endif

        // 
        // search through filter list looking for viable AC-3 pins
        //
        TTNode<CKsFilter>*  pnFilter;
        TTNode<CKsFilter>*  pnFilterNext;
        CKsFilter*          pFilter;
        ULONG               nFilter = 0;

        pnFilter = listFilters.GetHead();
        while (pnFilter)
        {
            pFilter = pnFilter->pData;
            pnFilterNext = listFilters.GetNext(pnFilter);

            // grovel through pins looking for one that does AC-3
            TTNode<CKsPin>* pnPin;
            TTNode<CKsPin>* pnPinNext;
            CKsPin*         pPin;

            pnPin = pFilter->m_listRenderSinkPins.GetHead();
            while(pnPin)
            {
                pPin = pnPin->pData;
                pnPinNext = pFilter->m_listRenderSinkPins.GetNext(pnPin);

                // grovel through data ranges
                PBYTE               pbRange = (PBYTE)pPin->m_Descriptor.pDataRanges;
                PKSDATARANGE_AUDIO  pRangeAudio;
                ULONG               nRange;

                for (nRange = 0; nRange < pPin->m_Descriptor.cDataRanges; nRange ++)
                {
                    // cast pbRange to make it useful
                    pRangeAudio = (PKSDATARANGE_AUDIO)pbRange;

                    // get next range
                    pbRange += pRangeAudio->DataRange.FormatSize;

                    // weed out undesirable ranges
                    fIsViableAC3 = 
                        (
                            IsEqualGUIDAligned(pRangeAudio->DataRange.MajorFormat, KSDATAFORMAT_TYPE_AUDIO)                 &&
                            (
                                IsEqualGUIDAligned(pRangeAudio->DataRange.Specifier,   KSDATAFORMAT_SPECIFIER_WAVEFORMATEX) ||
                                IsEqualGUIDAligned(pRangeAudio->DataRange.Specifier,   KSDATAFORMAT_SPECIFIER_DSOUND)
                            )                                                                                               &&
                            IS_VALID_WAVEFORMATEX_GUID(&pRangeAudio->DataRange.SubFormat)                                   &&
                            (EXTRACT_WAVEFORMATEX_ID(&pRangeAudio->DataRange.SubFormat) == WAVE_FORMAT_DOLBY_AC3_SPDIF)
                        );

                    if (fIsViableAC3)
                    {
                        // user m_UserDef to signify that this is a viable pin
                        pPin->m_UserDef = (LPARAM)TRUE;
                        pPin->m_pFilter->m_UserDef = (LPARAM)TRUE;

                        CPCMPinDev* pPinDev = new CPCMPinDev(pPin);

                        if (pPinDev)
                        {
                            g_IShell->AddDevice(pPinDev);
                            m_listPinDevs.AddTail(pPinDev);
                        }

                        if (nFilter >= cNonSysaudio)
                            strcat_s(pPinDev->m_szRep, _countof(pPinDev->m_szRep), " (via Sysaudio)");

                        break;
                    }
                }

                pnPin = pnPinNext;
            }

            // this filter doesn't have any viable pins on it.  Delete it.
            if (! (BOOL)pFilter->m_UserDef)
            {
                delete pFilter;
                listFilters.Remove(pnFilter);
            }

            pnFilter = pnFilterNext;

            nFilter++;
        }
    }

    //
    // waveout
    //
    MMRESULT        mmr;
    CWinMMDev*      pwwDevice;
    CPCMPinDev*     pKsPinDev;
    ULONG           cWaveOutDevs;
    ULONG           nWaveOut;

    cWaveOutDevs = waveOutGetNumDevs();
    for (nWaveOut = 0; nWaveOut < cWaveOutDevs; nWaveOut++)
    {
        WAVEOUTCAPS woc;

        mmr = waveOutGetDevCaps(nWaveOut, &woc, sizeof(woc));
        if (MMSYSERR_NOERROR == mmr)
        {
            pKsPinDev = NULL;
    
            if (g_fOnNT)
            {
                char szFilter[256];

                if (GetKsFilterNameOfMMDevice(nWaveOut, eMMwaveOut, szFilter, 256))
                {
                    pKsPinDev = FindKsDevWithFilterName(szFilter);
                }
            }
            else
            {
                DWORD_PTR   dwDevNodeId = 0;

                if (GetDevNodeOfMMDevice(nWaveOut, eMMwaveOut, &dwDevNodeId))
                {
                    pKsPinDev = FindKsDevWithDevNode(dwDevNodeId);
                }
            }

            if (pKsPinDev)
            {
                pwwDevice = new CWinMMDev(nWaveOut, woc.szPname);

                if (pwwDevice)
                {
                    g_IShell->AddDevice(pwwDevice);

                    strcpy_s(pwwDevice->m_szPnPId, pKsPinDev->m_szPnPId);
                    pwwDevice->m_cPossibleInstances = pKsPinDev->m_pPin->m_Descriptor.CInstancesGlobal.PossibleCount;
                }
            }
        }
    }

    //
    // dsound
    //
    DirectSoundEnumerate((LPDSENUMCALLBACK)DSCallbackPopulate, NULL);

    return TRUE;
}
