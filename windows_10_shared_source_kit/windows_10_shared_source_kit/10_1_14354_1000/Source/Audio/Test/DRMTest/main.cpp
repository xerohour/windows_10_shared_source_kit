//--------------------------------------------------------------------------;
//
//  File: main.cpp
//
// Copyright (C) Microsoft. All rights reserved.
//
//  Abstract:
//      Contains the basic necessities for the shell.
//
//  Contents:
//
//  History:
//      9/14/99    alpers   Started
//
//--------------------------------------------------------------------------;
//#include <windows.h>
#ifdef TIME_BOMB
#define DAYS_UNTIL_EXPIRATION 240
#include <timebomb.h>
#endif

#include "common.h"

// Profile Strings
#define     SZRUNNINGINTHELAB       _T("RunningInLab")
#define     SZRUNNINGINTHELABW        L"RunningInLab"

#define     SZDEFAULTDRIVERLISTFILE _T("DRMDRIVER.TXT")

#ifdef BUILD_TAEF

#include <basicloghelper.h>

BEGIN_MODULE()
    MODULE_PROPERTY(L"Feature", L"Core Audio - DRMTest")
END_MODULE()

HRESULT hrModuleCoInit = E_FAIL;

MODULE_SETUP(SetupSuite);
MODULE_CLEANUP(CleanupSuite);

bool SetupSuite()
{
    hrModuleCoInit = VERIFY_SUCCEEDED_RETURN(
        CoInitializeEx(NULL, COINIT_MULTITHREADED)
        );

    return true;
}

bool CleanupSuite()
{
    if(SUCCEEDED(hrModuleCoInit))
    {
        CoUninitialize();
    }

    return true;
}

#define g_pModule g_pDRMTest

CTaefShellStub g_IShellObject;
CTaefShellStub * g_IShell = &g_IShellObject;
CTaefShellStub * g_pShell = g_IShell;
IBasicLog * g_pBasicLog = NULL;

using namespace WEX::Common;
using namespace WEX::TestExecution;

#else

// Default Formats
WAVEFORMATEX g_DefaultWFX =
{
    WAVE_FORMAT_PCM,
    2,
    48000,
    192000,
    4,
    16,
    0
};
#endif

OSVERSIONINFOEX g_OSVersionInfo;
DWORD           g_dwDRMLevelThisOS;
EDRMCapableOS   g_OS = eOSNoDRM;

// Default authentication.
#define DEFAULT_CHECKDRIVERIMAGES       FALSE
#define DEFAULT_MINDRMLEVEL             1000
#define DEFAULT_PARSECATALOGFILE        FALSE
#define DEFAULT_REQUIRESIGNEDCATALOG    TRUE
#define DEFAULT_DRIVERFILECACHE         _T("C:\\gooddriver.txt")
#ifdef WHQL
#define DEFAULT_ALLOWTESTCERTIFICATE    TRUE
#else
#define DEFAULT_ALLOWTESTCERTIFICATE    FALSE
#endif

// we want device render endpoints
#define DV_REND     DEVICE_RENDER | DEV_OR

// ================================================================================================
// _THE_ table of test cases
// ------------------------------------------------------------------------------------
BEGIN_TEST_CASE_INFO(DRMTest, CDRMTest)
    DEVICE_TEST_PARENT( _T("")   , _T("Miniport"),                                                0, 0,        0, 0, NULL,                                           _T("{C3611E7F-4C7C-439F-A7ED-15DC815C140D}"))
    DEVICE_TEST_PARENT( _T("")   , _T(" DrmProperties"),                                          0, 0,        0, 0, NULL,                                           _T("{B2614063-74B7-4CC5-9A2C-5BEFE094EE4D}"))
    DEVICE_TEST_METHOD( _T("1")  , _T("  IDrmAudioStream"),                                       0, DV_REND,  0, 0, Test_Miniport_SetContentId,                     _T("{3D767FF7-B649-4759-8AD0-C1693496E90C}"))
    DEVICE_TEST_PARENT( _T("")   , _T(" Imports"),                                                0, 0,        0, 0, NULL,                                           _T("{e30135f5-eae0-4e12-9259-3a3c1b23d955}"))
    DEVICE_TEST_METHOD( _T("2")  , _T("  DrmForwardContentToFileObject"),                         0, DV_REND,  0, 0, Test_DrmForwardContentToFileObject,             _T("{08f1893c-3c3e-4971-82be-865cb5542147}"))
    DEVICE_TEST_PARENT( _T("")   , _T(" Authentication"),                                         0, 0,        0, 0, NULL,                                           _T("{D501CDE5-78B2-4F3C-8C00-61B26CE51D06}"))
    DEVICE_TEST_METHOD( _T("3")  , _T("  Check Driver Image"),                                    0, DV_REND,  0, 0, Test_Authentication_Image,                      _T("{261F7652-CBD7-42D8-93CC-A5AE6A838416}"))
//    DEVICE_TEST_METHOD( _T("4")  , _T("  Check Driver Stack"),                                    0, DV_REND,  0, 0, Test_Authentication_Image,                      _T("{D95B384E-EF05-4E0D-9D71-F04E8DE048D3}"))
#ifndef WHQL
    DEVICE_TEST_METHOD( _T("5")  , _T("  Check Full Authentication"),                             0, DV_REND,  0, 0, Test_Authentication_Authenticate,               _T("{567FE436-941B-4193-A54D-824B370E0563}"))
    DEVICE_TEST_METHOD( _T("6")  , _T("  Check MinDrmLevel"),                                     0, DV_REND,  0, 0, Test_Authentication_MinDrmLevel,                _T("{57A33046-2B87-41C3-8170-F08DC33A858C}"))
#endif //!WHQL
    DEVICE_TEST_METHOD( _T("7")  , _T("  Build Cert List"),                                       0, DV_REND,  0, 0, Test_Authentication_CertList,                   _T("{D8FEF980-653B-4BA0-8878-051974CB5B08}"))
    DEVICE_TEST_METHOD( _T("8")  , _T("  Find all drivers"),                                      0, DV_REND,  0, 0, Test_Authentication_FindAllDrivers,             _T("{5F2231C9-7F79-469D-BD86-D6B3BE93AB58}"))
    DEVICE_TEST_METHOD( _T("9")  , _T("  Check DisableProtectedAudioDG"),                         0, DV_REND,  0, 0, Test_CheckDisableProtectedAudioDG,              _T("{7A5BEC46-C49C-4F39-B70E-48B552A4B9F4}"))
#ifdef WHQL
    DEVICE_TEST_METHOD( _T("10") , _T("  Check For Test Signed Drivers"),                         0, DV_REND,  0, 0, Test_NoTestSignedDrivers,                       _T("{0A860CB3-0E63-4386-8263-42788FEEB457}"))
#endif // WHQL
END_TEST_CASE_INFO(DRMTest);


// ================================================================================================
//

// ----------------------------------------------------------------------------------
// Shell98 / ModMgr98 stuff
// ----------------------------------------------------------------------------------
PCDRMTest                       g_pDRMTest;

#ifndef BUILD_TAEF
static  LPTSTR                  szModuleName = _T("Digital Rights Management Audio Test");
ITestShell*                     g_IShell;

CTestModule* WINAPI
NewTestModule
(
    CTestShell* pShell,
    HINSTANCE   hInstDLL
)
{
    g_IShell = (ITestShell*)pShell;
    g_pDRMTest = new CDRMTest(pShell, hInstDLL);
    return (CTestModule*)g_pDRMTest;
}
#endif

#ifndef BUILD_TAEF
CDRMTest::CDRMTest
(
    CTestShell* pShell,
    HINSTANCE   hInstDLL
) 
:   CDeviceTestModule(pShell, hInstDLL),
#else
CDRMTest::CDRMTest
(
    DV_TCINFO * ptcInfo
) 
:   CDeviceTestModule(),
#endif
    m_pKsFDRM(NULL),
    m_pKsFKmixer(NULL),
    m_pKsFSysAudio(NULL),
    m_fDriversRealSigned(FALSE),        // See Initialize for important comments !!!!!!!!!
    m_bSystemTest(FALSE)
{

#ifndef BUILD_TAEF

    m_nIconID = APPICON;
    m_dwAppID = 40;
    m_dwWHQLChapter = WHQL_CHAPTER_AUDIO;
    m_pstrModuleName = szModuleName;
    m_rgTestCaseInfo = rgTestCaseInfo;
    m_cTestCases = ARRAY_ELEMENTS(rgTestCaseInfo);

    if(NULL == pShell->m_pszHwIdTestDeviceSelect ||
        0 == lstrcmp(_T(""), pShell->m_pszHwIdTestDeviceSelect))
    {
        m_bSystemTest = TRUE;
    }

#else
    m_rgTestCaseInfo = ptcInfo;
    m_cTestCases = 1;

    // Do we have a TAEF parameter to select the device?
    String targetDevice;
    if (SUCCEEDED(RuntimeParameters::TryGetValue(L"InstanceId", targetDevice)))
    {
        if (!SUCCEEDED(g_IShell->SetTestDevice(targetDevice)))
        {
            XLOG(XMSG, eError, L"Could not store InstanceId");
        }
    }
    else
    {
        m_bSystemTest = TRUE;
    }

#endif

    m_fTestingDevices = TRUE;

    _tcscpy_s(m_szDrmDriverListFile, 
              sizeof(m_szDrmDriverListFile)/sizeof(m_szDrmDriverListFile[0]), 
              SZDEFAULTDRIVERLISTFILE);

    ZeroMemory(&m_DefaultAuthenticate, sizeof(m_DefaultAuthenticate));
    m_DefaultAuthenticate.CheckDriverImages = DEFAULT_CHECKDRIVERIMAGES;
    m_DefaultAuthenticate.DriverFileCache = DEFAULT_DRIVERFILECACHE;
    m_DefaultAuthenticate.MinDRMLevel = DEFAULT_MINDRMLEVEL;
    m_DefaultAuthenticate.ParseCatalogFile = DEFAULT_PARSECATALOGFILE;
    m_DefaultAuthenticate.RequireSignedCatalog = DEFAULT_REQUIRESIGNEDCATALOG;
    m_DefaultAuthenticate.AllowTestCertificate = DEFAULT_ALLOWTESTCERTIFICATE;
    m_DefaultAuthenticate.MaxFailures = MAX_FAILURES;
    m_DefaultAuthenticate.NumFailures = 0;
    ZeroMemory(&m_DefaultAuthenticate.DriverFailures, 
               sizeof(*m_DefaultAuthenticate.DriverFailures) * MAX_FAILURES);

    m_fRestore = FALSE;

} // CDRMTest

CDRMTest::~CDRMTest
(
    void
)
{
    DestroyFilters();

    DestroyList(&m_listDrmDrivers);
    
    KslCloseKsLib();
}

//--------------------------------------------------------------------------;
//
//  CreateAudioDevices
//
//  Description:
//      Use the list of Render and Capture Devices to add devices to 
//      Test Shell
//
//  Arguments:
//      (void)
//
//  Return:
//      (BOOL)
//
//--------------------------------------------------------------------------;
BOOL
CDRMTest::CreateAudioDevices()
{
    //
    // Create a list of device abstraction using render and capture devices
    //
    PCAudioDevice                   pDevice;
    UINT                            uintWaveOut = 0;
    UINT                            uintWaveIn = 0;
    TTNode<CKsFilter>               *pnKSFilter = NULL;
    CKsFilter                       *pKSFilter = NULL;

    pnKSFilter = m_listRender.GetHead();

    while (pnKSFilter)
    {        
        pKSFilter = pnKSFilter->pData;      
        pDevice = new CAudioDevice();
        if (pDevice)
        {
            //
            // Find which wave or global filter is representing this device.
            // as well as waveX id
            //
            if (pDevice->Initialize(pKSFilter))
            {
                XLOG(XMSG, eInfo1, _T("\nNew Device Created"));
                pDevice->DumpProperties();

                // add to list of devices that will be used throughout the life of this app
                m_listAudioDevices.AddTail(pDevice);
                g_IShell->AddDevice(pDevice);
            }
            else
            {
                SafeDelete(pDevice);
            }//if (pDevice->Initialize(...
        }//if (pDevice)
        pnKSFilter = m_listRender.GetNext(pnKSFilter);
    }//while (pnKSFilter)    

    return !m_listAudioDevices.IsEmpty();
} // CDRMTest::CreateAudioDevices()

//--------------------------------------------------------------------------;
//
//  DestroyFilters
//
//  Description:
//      Empties and frees all audio filter related information
//
//  Arguments:
//      (void)
//
//  Return:
//      (void)
//
//--------------------------------------------------------------------------;
void
CDRMTest::DestroyFilters()
{
    TTNode <CKsFilter>       *pNode;
    TTNode <CAudioDevice>    *pAudioNode;

    SafeDelete(m_pKsFDRM);
    SafeDelete(m_pKsFKmixer);
    SafeDelete(m_pKsFSysAudio);

    DestroyList(&m_listSysAudioDevices);
    DestroyList(&m_listRender);

    m_listAudioDevices.Empty();

} // DestroyFilters

#ifndef BUILD_TAEF
#define IS_CHECKED(b)       (Button_GetCheck(GetDlgItem(hwnd, (b))) == BST_CHECKED)
#define CHECK_BUTTON(b, f)  Button_SetCheck(GetDlgItem(hwnd, (b)), (f) ? BST_CHECKED : BST_UNCHECKED)
//--------------------------------------------------------------------------;
//
//  DRMSettingDialogProc
//
//  Description:
//      DialogProc for DRMSettings dialog.
//
//  Arguments:
//      (void)
//
//  Return:
//      (void)
//
//--------------------------------------------------------------------------;
INT_PTR CALLBACK 
CDRMTest::DRMSettingDialogProc
(
    HWND hwnd, 
    UINT msg, 
    WPARAM wparam, 
    LPARAM lparam
)
{
    TCHAR szTemp[MAX_PATH];

    static PCDRMTest pTest;

    switch (msg)
    {
        case WM_INITDIALOG:
        {
            pTest = (PCDRMTest) lparam;
            CHECK_BUTTON(IDC_AUTHPARSECATALOGFILE,      pTest->m_DefaultAuthenticate.ParseCatalogFile);
            CHECK_BUTTON(IDC_AUTHREQUIRESIGNEDCATALOG,  pTest->m_DefaultAuthenticate.RequireSignedCatalog);
            CHECK_BUTTON(IDC_AUTHCHECKDRIVERIMAGES,     pTest->m_DefaultAuthenticate.CheckDriverImages);
            CHECK_BUTTON(IDC_AUTHALLOWTESTCERTIFICATE,  pTest->m_DefaultAuthenticate.AllowTestCertificate);

            _stprintf(szTemp, _T("%d"), pTest->m_DefaultAuthenticate.MinDRMLevel);
            Edit_SetText(GetDlgItem(hwnd, IDC_AUTHMINDRMLEVEL), szTemp);
            
            _stprintf(szTemp, _T("%d"), g_DefaultWFX.nChannels);
            Edit_SetText(GetDlgItem(hwnd, IDC_FORMATCHANNELS), szTemp);
            _stprintf(szTemp, _T("%d"), g_DefaultWFX.nSamplesPerSec);
            Edit_SetText(GetDlgItem(hwnd, IDC_FORMATSAMPLERATE), szTemp);
            _stprintf(szTemp, _T("%d"), g_DefaultWFX.wBitsPerSample);
            Edit_SetText(GetDlgItem(hwnd, IDC_FORMATBITS), szTemp);

            CHECK_BUTTON(IDC_DRIVERSREALSIGNED,             pTest->m_fDriversRealSigned);
        }
        break;

        case WM_COMMAND:
            switch( LOWORD(wparam) )
            {
                case IDOK:
                case IDAPPLY:
                    pTest->m_DefaultAuthenticate.ParseCatalogFile       = IS_CHECKED(IDC_AUTHPARSECATALOGFILE);
                    pTest->m_DefaultAuthenticate.RequireSignedCatalog   = IS_CHECKED(IDC_AUTHREQUIRESIGNEDCATALOG);
                    pTest->m_DefaultAuthenticate.CheckDriverImages      = IS_CHECKED(IDC_AUTHCHECKDRIVERIMAGES);
                    pTest->m_DefaultAuthenticate.AllowTestCertificate   = IS_CHECKED(IDC_AUTHALLOWTESTCERTIFICATE);
                    
                    Edit_GetText(GetDlgItem(hwnd, IDC_AUTHMINDRMLEVEL), szTemp, MAX_PATH);
                    pTest->m_DefaultAuthenticate.MinDRMLevel = _tstoi(szTemp);
                    g_dwDRMLevelThisOS = pTest->m_DefaultAuthenticate.MinDRMLevel;
                                
                    Edit_GetText(GetDlgItem(hwnd, IDC_FORMATCHANNELS), szTemp, MAX_PATH);
                    g_DefaultWFX.nChannels = (WORD) _tstoi(szTemp);
                    Edit_GetText(GetDlgItem(hwnd, IDC_FORMATSAMPLERATE), szTemp, MAX_PATH);
                    g_DefaultWFX.nSamplesPerSec = _tstoi(szTemp);
                    Edit_GetText(GetDlgItem(hwnd, IDC_FORMATBITS), szTemp, MAX_PATH);
                    g_DefaultWFX.wBitsPerSample = (WORD) _tstoi(szTemp);

                    pTest->m_fDriversRealSigned     = IS_CHECKED(IDC_DRIVERSREALSIGNED);

                    if (LOWORD(wparam) == IDOK)
                    {
                        EndDialog( hwnd, IDOK );
                    }
                    break;

                case IDCANCEL:
                    EndDialog( hwnd, IDCANCEL );
                    break;
            }
            break;

        default:
            return FALSE;
    }

    return TRUE;
} // DRMSettingDialogProc
#endif

//--------------------------------------------------------------------------;
//
//  EndTestCase
//
//  Description:
//      
//
//  Arguments:
//      (void)
//
//  Return:
//      (void)
//
//--------------------------------------------------------------------------;
DWORD                       
CDRMTest::EndTestCase(void)
{
    LogDRMKStats();
    
    return FNS_PASS;
} // EndTestCase

//--------------------------------------------------------------------------;
//
//  Initialize
//
//  Description:
//      Shell98 standard initialization
//
//  Arguments:
//      (void)
//
//  Return:
//      (DWORD)
//
//--------------------------------------------------------------------------;
DWORD
CDRMTest::Initialize
(
    void
)
{
    // 20041117 - jwexler - we need to do this to make sure the logfile gets put in the right place
    //  when executing the test from RunOnce after reboot
    // Probably not the best solution, but I don't see any adverse affects, so it should be ok.
    // If necessary, we could make it #ifdef WHQL only
#ifndef BUILD_TAEF
    SetCurrentDirectory(g_pShell->m_szSourcePath);

#ifndef WHQL
    // enabling this generates dirty logging....
    MxRegisterLog(g_IShell);
#endif // !WHQL
#endif

    CComPtr<IBasicLog> pBasicLog;
    g_IShell->GetBasicLog(&pBasicLog);
    KslRegisterLog(pBasicLog);

    g_OSVersionInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((LPOSVERSIONINFO)&g_OSVersionInfo);

    NtVersion os(g_OSVersionInfo.dwMajorVersion, g_OSVersionInfo.dwMinorVersion);
    
    if(NtVersion(6, 2) <= os)
    {
        // we'll treat all newer versions of the OS as Windows 8 for future compatibility
        g_OS = eOSWindows8;
        g_dwDRMLevelThisOS = DRM_LEVEL_WIN8;
        m_DefaultAuthenticate.MinDRMLevel = DRM_LEVEL_WIN8;
    }
    else if(NtVersion(6, 1) == os)
    {
        g_OS = eOSWindows7;
        g_dwDRMLevelThisOS = DRM_LEVEL_WIN7;
        m_DefaultAuthenticate.MinDRMLevel = DRM_LEVEL_WIN7;
    }
    else if(NtVersion(6, 0) == os)
    {
        g_OS = eOSWindowsVista;
        g_dwDRMLevelThisOS = DRM_LEVEL_VISTA;
        m_DefaultAuthenticate.MinDRMLevel = DRM_LEVEL_VISTA;
    }
    else if (NtVersion(5, 1) <= os)
    {
        g_OS = eOSWindowsXP;
        g_dwDRMLevelThisOS = DRM_LEVEL_WINXP;
        m_DefaultAuthenticate.MinDRMLevel = DRM_LEVEL_WINXP;
    }
    else
    {
        XLOG(XMSG, eInfo1, _T("Unrecognized OS: %u.%u"), os.major, os.minor);
        return FNS_FAIL;
    }

    m_fDriversRealSigned = IsOSRealSigned();
    
    return FNS_PASS;
} // Initialize

//--------------------------------------------------------------------------;
//
//  EnumerateDevices (Vista)
//
//  Description:
//      Initializes the list of filters for the test (for OS Vista and beyond).
//
//  Arguments:
//      (void)
//
//  Return:
//      (BOOL)
//
//--------------------------------------------------------------------------;
BOOL
CDRMTest::EnumerateDevices()
{
    BOOL                    fResult = TRUE;
    BOOL                    fFound = FALSE;
    PCKsFilter              pKsFilter;
    TTNode <CKsFilter>*     pNodeFilter;       
    TTList <CKsFilter>      listFilters;
    LPGUID                  arpguidCategories[2];
    TCHAR                   szName[MAX_PATH];
    
    try
    {
        // Clean old filters.
        DestroyFilters();

        // Find DRM filter.
        XLOG(XMSG, eInfo1, _T("\nEnumerating KSCATEGORY_DRM_DESCRAMBLE"));
        arpguidCategories[0] = (LPGUID)&KSCATEGORY_DRM_DESCRAMBLE;

        if (!KslEnumFilters(&listFilters, ePCMAudio, arpguidCategories,
                            1, FALSE, FALSE, FALSE))
        {
            throw(_T("Failed find DRM filter"));
        }

        if (!listFilters.IsEmpty())
        {

            pNodeFilter = listFilters.GetHead();
            m_pKsFDRM = (PCPCMAudioFilter) pNodeFilter->pData;        
            pNodeFilter->pData = NULL;

            if (!m_pKsFDRM)
            {
                throw(_T("Found no DRM filter using SetupAPI"));
            }

            XLOG(XMSG, eInfo1, _T("    %s"), m_pKsFDRM->m_szFilterName);

            DestroyList(&listFilters);
        }
        else
        {
            XLOG(XMSG, eInfo1, _T("Found no DRM filter"));
        }


        // Find renderer filters. Pick the first wave filter.
        XLOG(XMSG, eInfo1, _T("\nEnumerating KSCATEGORY_AUDIO and KSCATEGORY_RENDER"));
        arpguidCategories[0] = (LPGUID)&KSCATEGORY_AUDIO;
        arpguidCategories[1] = (LPGUID)&KSCATEGORY_RENDER;

        if (!KslEnumFilters(&m_listRender, ePCMAudio, arpguidCategories,
                            2, FALSE, FALSE, FALSE))
        {
            throw(_T("Failed to find Renderer filter"));
        }

        fFound = !m_listRender.IsEmpty();
        if (!fFound)
        {
            if (m_bSystemTest)
            {
                // For system test, we require rendering endpoint existing in the system
                throw(_T("Found no renderer filter"));
            }
            else
            {
                // For device test, it's allowed to have only capturing endpoint since partner 
                // might only develop his codec with capture device support only
                // Then in each test case, we will skip the test if there is no rendering filter
                fFound = TRUE;
                fResult = TRUE;
            }
        }
        else
        {
            XLOG(XMSG, eInfo1, _T("\nCreating audio devices for Test Shell"));
            if (!CreateAudioDevices())
            {
                throw(_T("Cannot create Audio Devices"));
            }
        }
    }
    catch (LPTSTR szError)
    {
        XLOG(XFAIL, eError, _T("%s"), szError);
        fResult = FALSE;
    }
        
    fResult = fResult && fFound;
    listFilters.Empty();
    SBARLOG(_T(""));

    return fResult;
} // CDRMTest::EnumerateDevices()

//--------------------------------------------------------------------------;
//
//  IsInboxDRMCompliantDriver (was IsDRMCompliant)
//
//  Description:
//      Compare the Device name to the list of DRM compliant in box 
//  Millennium or Whistler drivers.
//  This list should be updated.
//      For WHQL all drivers are considered DRM conpliant.
//
//  Arguments:
//      LPSTR                   szFriendlyName
//
//  Return:
//      (BOOL)
//
//--------------------------------------------------------------------------;
BOOL 
CDRMTest::IsInboxDRMCompliantDriver
(
    LPTSTR                      szDriverFileName,   // was szFriendlyName  (10/24/2001)
    PDRMRIGHTS                  pRights,            /*= NULL*/
    DWORD                       dwDrmLevel          /*= 0 */
)
{
#ifdef WHQL
    return TRUE;
#else
    TTNode<DRIVER_PROPERTY>* pnDriver = NULL;
    DRIVER_PROPERTY*        pDriver = NULL;
    int                     nStrLen = _tcslen(szDriverFileName);
    LPTSTR                  pchBinary = szDriverFileName + nStrLen - 1;

    while ((pchBinary > szDriverFileName) && (*(pchBinary - 1) != _T('\\')))
        pchBinary--;

    pnDriver = m_listDrmDrivers.GetHead();

    while (pnDriver)
    {
        pDriver = pnDriver->pData;
        pnDriver = m_listDrmDrivers.GetNext(pnDriver);

        if (pDriver)
        {
            if (!_tcsicmp(pDriver->szDriverName, pchBinary))
            {
                if (dwDrmLevel <= pDriver->DrmLevel)
                {
                    if (pRights)
                    {
                        if ((!COPYRIGHT((*pRights)) || pDriver->CanScmsCopyrighted) &&
                            (!DIGITAL((*pRights)) || pDriver->CanDigitalOutputDisable))
                        {
                            return TRUE;
                        }
                    }
                    else
                    {
                        // Do not care about rights.
                        return TRUE;
                    }
                }
            }
        }
    }

    return FALSE;
#endif
} // IsDRMCompliant

#ifndef BUILD_TAEF
//--------------------------------------------------------------------------;
//
//  ProcessMenuItem
//
//  Description:
//
//  Arguments:
//      (void)
//
//  Return:
//      (void)
//
//--------------------------------------------------------------------------;
void
CDRMTest::ProcessMenuItem
(
    DWORD   nID, 
    HWND    hWndCtl, 
    DWORD   codeNotify,
    HMENU   hMenu
)
{
    switch(nID)
    {
        case IDM_SHOWSPECTRUM:
            break;

        case IDM_ENUMERATEDRIVERS:
            EnumerateDevices();
            break;

        case IDM_FINDDIGITAL:
            break;

        case IDM_DRMSETTINGS:
            DialogBoxParam
            (
                m_hInstance, 
                MAKEINTRESOURCE( IDD_DRMSETTINGS  ), 
                NULL, 
                DRMSettingDialogProc,
                (LPARAM) this
            );
            break;

        case IDM_DRMDRIVERLIST:
            ::MessageBox(
                g_IShell->m_hwndShell, 
                _T("Edit DRMDRIVER.TXT, Save and Click OK"), 
                _T("Updating Driver List"), 
                MB_OK);
            if (!UpdateDriverList())
            {
                ::MessageBox(
                    g_IShell->m_hwndShell, 
                    _T("Could not update driver file list. Check DrmDriver.Txt"),
                    _T("Error"),
                    MB_OK);
            }
            break;
            
        default:
            ::MessageBox(
                g_IShell->m_hwndShell, 
                _T("Unexpected ProcessMenuItem argument"),
                _T("Error"),
                MB_OK);
            break;
    }
} // ProcessMenuItem
#endif

//--------------------------------------------------------------------------;
//
//  OnInitialUpdate
//
//  Description:
//      Shell98 standard OnInitialUpdate
//
//  Arguments:
//      (void)
//
//  Return:
//      (DWORD)
//
//--------------------------------------------------------------------------;
DWORD
CDRMTest::OnInitialUpdate
(
    void
)
{
    DWORD nResult = FNS_PASS; //assign FNS_FAIL in cases of catastrophic failures

    // Log some stuff
#ifndef BUILD_TAEF
    XLOG(XMSG, eWarn1, m_pstrModuleName);
    XLOG(XMSG, eWarn1, _T("AppID = %d"), m_dwAppID);
    XLOG(XMSG, eWarn1, _T(""));
#endif

    CComPtr<IBasicLog> pLog;
    g_IShell->GetBasicLog(&pLog);

    //check if the test is running under specific circumstances and skip
    if(QueryMachineInfo::IsCodeCoverage(pLog))
    {
        XLOG(XSKIP, eFatalError, _T("Code Coverage should not run during DRM test execution. "));
        XLOG(XSKIP, eFatalError, _T("Skipping the tests as to not provide incorrect test results."));
        return FNS_SKIPPED;
    }

    if(QueryMachineInfo::IsKernelDebug(pLog))
    {
        XLOG(XFAIL, eFatalError, _T("Kernel Debuggers should not run during DRM test execution. "));
        XLOG(XFAIL, eFatalError, _T("Failing the tests as to not provide incorrect test results."));
        return FNS_FAIL;
    }

    if(QueryMachineInfo::IsDriverVerifier(pLog))
    {
        XLOG(XFAIL, eFatalError, _T("Driver Verifier should not run during DRM test execution. "));
        XLOG(XFAIL, eFatalError, _T("Failing the tests as to not provide incorrect test results."));
        return FNS_FAIL;
    }

    if(QueryMachineInfo::IsWow64(pLog))
    {
        // Per StanPenn, Windows 8 Bugs 234193
        // DRM plug-in components and OS components running in a Protected Environment (PE)
        // on 64-bit OSes are always 64 bit.
        // There is no support for 32-bit components.
        // 32-bit apps will be talking by proxy through to mfpmp or audiodg running as 64 bit
        //
        // And indeed, DRMKAuthenticate fails with DRM_SYSERR from a WOW64 process.
        XLOG(XSKIP, eFatalError, _T("DRMTest should not run WOW64."));
        XLOG(XSKIP, eFatalError, _T("Skipping the tests as to not provide incorrect test results."));
        return FNS_SKIPPED;
    }

#ifndef BUILD_TAEF
    // Parse DRMTest specific cmdline
    if (g_pShell->m_pszAppCmdLine)
    {
        _tcslwr(g_pShell->m_pszAppCmdLine);        
        m_fRestore = _tcsstr(g_pShell->m_pszAppCmdLine, _T("restore")) ? TRUE : FALSE;
    }

    XLOG(XMSG, eInfo1, _T("g_pShell->m_pszSettingsFile == %s"), g_pShell->m_pszSettingsFile);
    XLOG(XMSG, eInfo1, _T("g_pShell->m_szSourcePath == %s"), g_pShell->m_szSourcePath);

#endif

#ifndef BUILD_TAEF
    
    g_IShell->InstallMenuItem( _T("&Options"), _T("separator"), 0);    
    g_IShell->InstallMenuItem( _T("&Options"), _T("Show Spectrum"), IDM_SHOWSPECTRUM);

#ifndef WHQL
    g_IShell->InstallMenuItem( _T("&Options"), _T("Find Digital Recording"), IDM_FINDDIGITAL);
    g_IShell->InstallMenuItem( _T("&Options"), _T("separator"), 0);
    g_IShell->InstallMenuItem( _T("&Options"), _T("Enumerate Drivers"), IDM_ENUMERATEDRIVERS);
    g_IShell->InstallMenuItem( _T("&Options"), _T("Update Driver List"), IDM_DRMDRIVERLIST);
    g_IShell->InstallMenuItem( _T("&Options"), _T("separator"), 0);
    g_IShell->InstallMenuItem( _T("&Options"), _T("DRM Settings"), IDM_DRMSETTINGS);

    UpdateDriverList();
#endif // WHQL

#else // BUILD_TAEF
    // no message handler on taef to handle creating the list,
    // so do it now.
    UpdateDriverList();
#endif

    // enumerate devices
    if (!KslInitKsLib())
    {
        XLOG(XFAIL, eError, _T("Cannot initialize kslib"));
        nResult = FNS_FAIL;
    }
    if (!EnumerateDevices())
    {
        XLOG(XMSG, eError, _T("No device is enumerated"));
    }

    XLOG(XMSG, eInfo1, _T("DriversRealSigned : %s"), g_pDRMTest->m_fDriversRealSigned ? _T("TRUE") : _T("FALSE"));

    return nResult;
} // OnInitialUpdate

// ------------------------------------------------------------------------------
void
CDRMTest::SetTestDevice
(
    CDeviceDescriptor* pDevice
)
{
    m_pDeviceUnderTest = (CAudioDevice*)pDevice;

#ifndef WHQL
    BOOL fIsListedAsDRMCertified = 
        IsInboxDRMCompliantDriver(m_pDeviceUnderTest->m_pFilter->m_szBinary);   // was m_szRep (10/24/2001);

    if (fIsListedAsDRMCertified)
        XLOG(XMSG, eInfo1, _T("%s:%s is listed in DRMDRIVER.TXT as being DRM compliant\n"), m_pDeviceUnderTest->m_pFilter->m_szBinary, m_pDeviceUnderTest->m_szRep);
    else
        XLOG(XMSG, eError, _T("%s:%s is NOT listed in DRMDRIVER.TXT as being DRM compliant\n"), m_pDeviceUnderTest->m_pFilter->m_szBinary, m_pDeviceUnderTest->m_szRep);
#endif
}

//--------------------------------------------------------------------------;
//
//  UpdateDriverList
//
//  Description:
//      
//
//  Arguments:
//      
//
//  Return:
//      (BOOL)
//
//--------------------------------------------------------------------------;
BOOL
CDRMTest::UpdateDriverList()
{
    TCHAR   szFileName[MAX_PATH];
    PTCHAR  pchTemp;
    FILE    *file;
    TCHAR   szLine[MAX_PATH];
    LPCTSTR strSection = NULL;
    BOOL    fFoundSection = FALSE;

    //
    // Make file name
    //
    GetModuleFileName(NULL, szFileName, MAX_PATH);
    pchTemp = _tcsrchr(szFileName, '\\');
    *(pchTemp + 1) = 0;
    _tcscat(szFileName, m_szDrmDriverListFile);

    XLOG(XMSG, eInfo1, _T("\n============================================================================="));    
    XLOG(XMSG, eInfo1, _T("      Driver List from %s"), m_szDrmDriverListFile);

    // look for section string [*] depending on the OS
    if (VER_PLATFORM_WIN32_NT != g_OSVersionInfo.dwPlatformId)
    {
        XLOG(XMSG, eError,
            _T("OSVersionInfo.dwPlatformId is %u; ")
            _T("DRM Test is only supported on systems where dwPlatformId is VER_PLATFORM_WIN32_NT"),
            g_OSVersionInfo.dwPlatformId);
        return FALSE;
    }

    NtVersion os(g_OSVersionInfo.dwMajorVersion, g_OSVersionInfo.dwMinorVersion);

    if (os < NtVersion(5, 1))
    {
        XLOG(XMSG, eError, _T("OS is NT %u.%u; DRM Test is not supported on versions less than NT 5.1"), os.major, os.minor);
        return FALSE;
    }

    if (os < NtVersion(6, 0))
    {
        strSection = _T("[XPSP2]");
    }
    else if (NtVersion(6, 0) == os)
    {
        strSection = _T("[Vista]");        
    }
    else if (NtVersion(6, 1) == os)
    {
        strSection = _T("[Windows 7]");        
    }
    else
    {
        // for future compatibility we'll use this section for future versions of the OS
        strSection = _T("[Windows 8]");
    }

    //
    // first, empty the list
    //
    DestroyList(&m_listDrmDrivers);

    //
    // Open file.
    //
    file = _tfopen(szFileName, _T("rt"));
    if (file)
    {
        // look for section
        while (_fgetts(szLine, MAX_PATH, file) && !fFoundSection)
        {
            // remove carriage return
            szLine[_tcslen(szLine) - 1] = 0;

            if (_tcslen(szLine))
            {
                if (_tcscmp(szLine, strSection) == 0)
                {
                    fFoundSection = TRUE;

                    // read each line in the section
                    while (_fgetts(szLine, MAX_PATH, file))
                    {
                        szLine[_tcslen(szLine) - 1] = 0;

                        // must end section with blank line
                        if (*szLine == 0)
                            break;

                        // ignore comments
                        if (*szLine == ';')
                            continue;

                        if (_tcslen(szLine))
                        {
                            DRIVER_PROPERTY* pDriver = new DRIVER_PROPERTY;

                            pchTemp = _tcstok(szLine, _T(","));
                            _tcscpy(pDriver->szDriverName, pchTemp);
                            pchTemp = _tcstok(NULL, _T(","));
                            pDriver->DrmLevel = _tstoi(pchTemp);
                            pchTemp = _tcstok(NULL, _T(","));
                            pDriver->CanScmsCopyrighted = _tstoi(pchTemp);
                            pchTemp = _tcstok(NULL, _T(","));
                            pDriver->CanDigitalOutputDisable = _tstoi(pchTemp);

                            XLOG(XMSG, eInfo1, _T("              - %s %d %d %d"), 
                                pDriver->szDriverName,
                                pDriver->DrmLevel,
                                pDriver->CanScmsCopyrighted,
                                pDriver->CanDigitalOutputDisable
                                ); 

                            m_listDrmDrivers.AddTail(pDriver);
                        }
                    }
                }
            }
        }

        fclose(file);

        return TRUE;
    }
    else
    {
        XLOG(XMSG, eError, _T("%s not found"), szFileName);
    }

    return FALSE;
} // UpdateDriverList

//--------------------------------------------------------------------------;
//
//  DestroyList
//
//  Description:
//      Deletes all elements of the given filter list and empties it.
//
//  Arguments:
//      TTList <CKsFilter> *pList
//
//  Return:
//      (void)
//
//--------------------------------------------------------------------------;
template <class T>
void 
DestroyList
(
    TTList <T>                   *pList
)
{
    TTNode <T>                   *pNode;

    pNode = pList->GetHead();
    while (pNode)
    {
        SafeDelete(pNode->pData);
        pNode = pList->GetNext(pNode);
    }

    pList->Empty();
} // DestroyList

template void DestroyList<CKsPin> (TTList <CKsPin> *);
template void DestroyList<CPCMAudioFilter> (TTList <CPCMAudioFilter> *);
template void DestroyList<CPCMAudioPin> (TTList <CPCMAudioPin> *);

HRESULT RetrieveBLOBFromFile(LPCTSTR pszFileName, DWORD *pcb, BYTE** ppb)
{
    HRESULT    hr=S_OK;
    HANDLE    hFile=NULL;
    DWORD    dwBytesRead=0;

    if(!pcb || !ppb || !pszFileName)
        return E_INVALIDARG;

    *ppb=NULL;
    *pcb=0;

    hFile = CreateFile(pszFileName,
                       GENERIC_READ,
                       FILE_SHARE_READ,
                       NULL,                   // lpsa
                       OPEN_EXISTING,
                       FILE_ATTRIBUTE_NORMAL,
                       NULL);
    if (INVALID_HANDLE_VALUE == hFile )
        return HRESULT_FROM_WIN32(GetLastError());

    *pcb = GetFileSize(hFile, NULL);
    if ( 0xffffffff == *pcb )
        return HRESULT_FROM_WIN32(GetLastError());

    *ppb = new BYTE[*pcb];

    if(!(*ppb))
        return E_OUTOFMEMORY;

     // Read the pkcs7 message
     if(!ReadFile(hFile,
                  *ppb,
                  *pcb,
                  &dwBytesRead,
                  NULL) ||
           dwBytesRead != *pcb)
     {
            delete(*ppb);
            *ppb=NULL;
            CloseHandle(hFile);
            return HRESULT_FROM_WIN32(GetLastError());;
     }

     //close the file handle
    CloseHandle(hFile);

    return S_OK;
}

//
// functions to check if the OS is real signed
//
//------------------------------------------------------------------------------------
//
// Attempt to read as a file containing an encoded CTL.
//
//------------------------------------------------------------------------------------
HCERTSTORE OpenEncodedCTL (LPTSTR pszStoreFilename)
{
    HCERTSTORE hStore;
    BYTE *pbEncoded;
    DWORD cbEncoded;
    DWORD dwMsgAndCertEncodingType = PKCS_7_ASN_ENCODING | X509_ASN_ENCODING;

    if (S_OK != RetrieveBLOBFromFile(pszStoreFilename, &cbEncoded, &pbEncoded))
        return NULL;

    if (NULL == (hStore = CertOpenStore(
            CERT_STORE_PROV_MEMORY,
            0,                      // dwEncodingType
            0,                      // hCryptProv
            0,                      // dwFlags
            NULL                    // pvPara
            )))
        goto CommonReturn;

    if (!CertAddEncodedCTLToStore(
            hStore,
            dwMsgAndCertEncodingType, 
            pbEncoded,
            cbEncoded,
            CERT_STORE_ADD_ALWAYS,
            NULL                    // ppCtlContext
            )) 
    {
        CertCloseStore(hStore, 0);
        hStore = NULL;
    }

CommonReturn:
    delete pbEncoded;

    return hStore;
}

BOOL IsOSRealSigned()
{
// Below is a hack to determine whether the given OS is production, or
// test. It's based on the existance of an nt5.cat file. That file never
// exists on phone. The result of failing is that we'll always allow test signed
// drivers in the HLK running on phone.
    BOOL fRealSigned = FALSE;
    BOOL fRes = TRUE;

    PCERT_NAME_INFO pInfo = NULL;
    PCCERT_CHAIN_CONTEXT pChainContext = NULL;
    PCCERT_CONTEXT pCertSigner = NULL;
    PCCTL_CONTEXT pCtlContext = NULL;
    HANDLE hCertStore = NULL;

    TCHAR pszWindowsDir[MAX_PATH];
    GetWindowsDirectory(pszWindowsDir, MAX_PATH);

    _tcscat(pszWindowsDir, _T("\\system32\\catroot\\{F750E6C3-38EE-11D1-85E5-00C04FC295EE}\\nt5.cat"));

    // Create a store with the cat's encoded CTL
    hCertStore = OpenEncodedCTL(pszWindowsDir);
    if (!hCertStore)
    {
        // the store doesn't exist on phone, so failing to open it is expected.
        // failing here just means we allow for test signed drivers.
        // XLOG(XMSG, eError, _T("Failed to open the store!"));
        goto DONE;
    }
    
    // get the CTL
    pCtlContext = CertEnumCTLsInStore(hCertStore, NULL);
    if (!pCtlContext)
    {
        XLOG(XMSG, eError, _T("Failed to enumerate a CTL!"));
        goto DONE;
    }

    // get the signer certificate    
    fRes = CryptMsgGetAndVerifySigner(pCtlContext->hCryptMsg, 
                                      0, // 1
                                      NULL, // (HCERTSTORE*)&pCtlContext->hCertStore, 
                                      CMSG_SIGNER_ONLY_FLAG, 
                                      &pCertSigner, 
                                      NULL);
    if (!fRes)
    {
        XLOG(XMSG, eError, _T("Failed CryptMsgGetAndVerifySigner!"));
        goto DONE;
    }

    // Get the certificate chain
    CERT_ENHKEY_USAGE EnhkeyUsage;
    CERT_USAGE_MATCH CertUsage;
    CERT_CHAIN_PARA ChainPara;    

    EnhkeyUsage.cUsageIdentifier = 0;
    EnhkeyUsage.rgpszUsageIdentifier = NULL;
    CertUsage.dwType = USAGE_MATCH_TYPE_AND;
    CertUsage.Usage = EnhkeyUsage;
    ChainPara.cbSize = sizeof(CERT_CHAIN_PARA);
    ChainPara.RequestedUsage = CertUsage;
    fRes = CertGetCertificateChain(NULL, 
                                   pCertSigner, 
                                   NULL, 
                                   hCertStore, //NULL, 
                                   &ChainPara, 
                                   0, 
                                   NULL, 
                                   &pChainContext);
    if (!fRes)
    {
        XLOG(XMSG, eError, _T("Failed CertGetCertificateChain!"));
        goto DONE;
    }

    // Get the root certificate
    PCERT_SIMPLE_CHAIN pSimpleChain = pChainContext->rgpChain[pChainContext->cChain-1];
    PCERT_CHAIN_ELEMENT pCertChainElement = pSimpleChain->rgpElement[pSimpleChain->cElement-1];
    PCCERT_CONTEXT pCertRoot = pCertChainElement->pCertContext;

    // Decode Issuer information
    DWORD cbInfo = 0;
    CryptDecodeObjectEx(pCertRoot->dwCertEncodingType, 
                        X509_NAME, //lpszStructType, 
                        pCertRoot->pCertInfo->Issuer.pbData, 
                        pCertRoot->pCertInfo->Issuer.cbData, 
                        0, // dwFlags, 
                        NULL, //pDecodePara
                        NULL, // pInfo, 
                        &cbInfo);
    pInfo = (PCERT_NAME_INFO)new BYTE[cbInfo];
    if(NULL == pInfo)
    {
        XLOG(XMSG, eError, _T("FAIL: Unable to allocate memory."));
        goto DONE;
    }
    fRes = CryptDecodeObjectEx(pCertRoot->dwCertEncodingType, 
                               X509_NAME, //lpszStructType,
                               pCertRoot->pCertInfo->Issuer.pbData, 
                               pCertRoot->pCertInfo->Issuer.cbData, 
                               0, //dwFlags, 
                               NULL, //pDecodePara
                               pInfo, 
                               &cbInfo);
    if (!fRes)
    {
        XLOG(XMSG, eError, _T("Failed CryptDecodeObjectEx!"));
        goto DONE;
    }

    // Cycle through the RDNs/attributes to find the issuer's name
    UINT i = 0, j = 0;
    PCERT_RDN pRDN = NULL;
    PCERT_RDN_ATTR pAttr = NULL;
    
    for (i = 0, pRDN = pInfo->rgRDN; i < pInfo->cRDN; i++, pRDN++)
    {
        for (j = 0, pAttr = pRDN->rgRDNAttr; j < pRDN->cRDNAttr; j++, pAttr++)
        {
            if (!strcmp(pAttr->pszObjId, szOID_COMMON_NAME))
            {
                break;
            }
        }
    }

    if(NULL != pAttr)
    {
        if (!strcmp((LPCSTR)pAttr->Value.pbData, "Microsoft Root Authority")
            || !strcmp((LPCSTR)pAttr->Value.pbData, "Microsoft Root Certificate Authority")
            || !strcmp((LPCSTR)pAttr->Value.pbData, "Microsoft Root Certificate Authority 2010")
            )
        {
            fRealSigned = TRUE;
        }
    }

DONE:
    delete[] pInfo;
    if (pChainContext)  { CertFreeCertificateChain(pChainContext); }
    if (pCertSigner)    { CertFreeCertificateContext(pCertSigner); }
    if (pCtlContext)    { CertFreeCTLContext(pCtlContext); }
    if (hCertStore)     { CertCloseStore(hCertStore, 0); }

    return fRealSigned;
}

DWORD
CDRMTest::RunTest
(
    DWORD   dwTestID
)
{
    DWORD       dw;

    PDV_TCINFO rgTestCaseInfo = (PDV_TCINFO)m_rgTestCaseInfo;

#ifndef BUILD_TAEF
    m_pShell->IncrementIndent();
#endif

    dw = ((rgTestCaseInfo[dwTestID].pfnTest)());

#ifndef BUILD_TAEF
    m_pShell->DecrementIndent();
#endif

    return(dw);
}

