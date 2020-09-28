// Copyright (C) Microsoft. All rights reserved.
#ifndef _DRMTEST_INC
#define _DRMTEST_INC

#include <dvmodule.h>

///////////////////////////////////////////////////////////////////////////////
// Resources 
#define DRMTEST_MENU_ITEM       2000
#define IDM_SHOWSPECTRUM        DRMTEST_MENU_ITEM + 1
#define IDM_FINDDIGITAL         DRMTEST_MENU_ITEM + 2
#define IDM_ENUMERATEDRIVERS    DRMTEST_MENU_ITEM + 3
#define IDM_DRMSETTINGS         DRMTEST_MENU_ITEM + 4
#define IDM_DRMDRIVERLIST       DRMTEST_MENU_ITEM + 5

///////////////////////////////////////////////////////////////////////////////
// TYPEDEF
typedef struct
{
    TCHAR                       szDriverName[MAX_PATH];
    DWORD                       DrmLevel;
    DWORD                       CanScmsCopyrighted;
    DWORD                       CanDigitalOutputDisable;
} DRIVER_PROPERTY;

typedef struct
{
	WAVEFORMATEX            wfx;
	WORD                    wReserved;
	ULONG                   ulContentId;
	WAVEFORMATEXTENSIBLE    wfxSecure;
} DRMWAVEFORMATEXTENSIBLE, *PDRMWAVEFORMATEXTENSIBLE;


///////////////////////////////////////////////////////////////////////////////
// DEFINES
#define INVALID_DRM_HANDLE      0xFFFF

#define TEST_BUFFER_MS          220
#define TEST_LONG_BUFFER_MS     30000

#define ANALYZER_INIT_MS        1000

#define DRM_LEVEL_WINXP         1200
#define DRM_LEVEL_VISTA         1200
#define DRM_LEVEL_WIN7          1300
#define DRM_LEVEL_WIN8          1300

///////////////////////////////////////////////////////////////////////////////
// Class declaration
class CDRMTest : public CDeviceTestModule
{
public:
#ifndef BUILD_TAEF
    CDRMTest(CTestShell* pShell, HINSTANCE hInstDLL);
#else
    CDRMTest(DV_TCINFO * ptcInfo);
#endif
    virtual ~CDRMTest(void);

public:
    DWORD                       EndTestCase(void);
    DWORD                       RunTest(DWORD dwTestID);
    DWORD                       Initialize(void);
    BOOL                        EnumerateDevices();
#ifndef BUILD_TAEF
    virtual void                ProcessMenuItem(DWORD nID, HWND hWndCtl, DWORD codeNotify, HMENU hMenu);
#endif
    DWORD                       OnInitialUpdate(void);

    void                        SetTestDevice(CDeviceDescriptor* pDevice);

                                // mitchr 10/24/2001, changed name from IsDRMCompliant
    BOOL                        IsInboxDRMCompliantDriver
                                (
                                    LPTSTR szDriverFileName, 
                                    PDRMRIGHTS pRights = NULL, 
                                    DWORD dwDrmLevel = 0
                                 );
        
public:
    // Related filters.
    PCPCMAudioFilter            m_pKsFDRM;
    PCPCMAudioFilter            m_pKsFKmixer;
    PCSysAudio                  m_pKsFSysAudio;

    TTList<CKsFilter>           m_listRender;             // list of KSCATEGORY_RENDER
    TTList<CKsFilter>           m_listSysAudioDevices;    // list of KSCATEGORY_AUDIO_DEVICE

    TTList<CAudioDevice>        m_listAudioDevices;       // list of abstract audio devices.
    CAudioDevice*               m_pDeviceUnderTest;

    BOOL                        m_bSystemTest;

    BOOL                        m_fSuppressCertsRequirement;

    BOOL                        m_fDriversRealSigned;

    // These are default settings for stream creation.
    AUTHPARAM                   m_DefaultAuthenticate;

    TCHAR                       m_szDrmDriverListFile[MAX_PATH];
    TTList<DRIVER_PROPERTY>     m_listDrmDrivers;

protected:
    BOOL                        CreateAudioDevices();
    void                        DestroyFilters();
    void                        FindDigitalRecording();

    BOOL                        UpdateDriverList();

    static INT_PTR CALLBACK     DRMSettingDialogProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:
    BOOL                        m_fRestore;
};

///////////////////////////////////////////////////////////////////////////////
// TYPEDEF
typedef CDRMTest*               PCDRMTest;
typedef DWORD(WINAPI *KSUPROC)(HANDLE, PKSPIN_CONNECT, DWORD, PHANDLE);

///////////////////////////////////////////////////////////////////////////////
// FUNCTIONS.
template <class T> void DestroyList(TTList <T> *pList);

BOOL IsOSRealSigned();

///////////////////////////////////////////////////////////////////////////////
// EXTERN 
extern PCDRMTest                g_pDRMTest;
extern OSVERSIONINFOEX          g_OSVersionInfo;
extern DWORD                    g_dwDRMLevelThisOS;
extern TCHAR                    g_szOSDriverFileCache[MAX_PATH];
extern WAVEFORMATEX             g_DefaultWFX;

typedef enum 
{ 
    eOSNoDRM, 
    eOSWindowsXP, 
    eOSWindowsVista, 
    eOSWindows7,
    eOSWindows8
} EDRMCapableOS;

extern EDRMCapableOS            g_OS;


///////////////////////////////////////////////////////////////////////////////
// adapter.cpp
HRESULT IsDrmInterfaceImplemented(PCAudioDevice pDevice);

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
#endif // #ifndef _DRMTEST_INC
