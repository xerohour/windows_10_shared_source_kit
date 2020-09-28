#include <windows.h>
#include <basetsd.h>

#include <initguid.h>
#include <d3d10_1.h>
#include <d3d10.h>
#include <dxgi.h>

#include "TestApp.h"
#include "PresentWinCore.h"

//////////////////////////////////////////////////////////////////////////
// CMyTestApp - An instance of a single object derived from CWGFTestFramework
// is required by the framework.
//////////////////////////////////////////////////////////////////////////

bool CMyTestApp::InitTestGroups()
{
    // Register test class with the group manager.
    /*
    if (!ADD_TEST(_T("BBDimensions"), clsBBDimensions)) return false;
    return true;
    */

    if ( !ADD_TEST( _T("WinPresent"), CPresentWinConvertStretchCore)) return false;

    if ( !ADD_TEST( _T("WinPresentSmallWindow"),  PresentWinCSSmallWindow)) return false;

    if ( !ADD_TEST( _T("WinPresentClippingWindows"), CPresentWinClippingWindows)) return false;

    if ( !ADD_TEST( _T("WinPresentChildWindows"), CPresentWinChildWindows)) return false;

    if ( !ADD_TEST( _T("WinPresentScrollAndDirtyRects"), CPresentWinScrollAndDirtyRects)) return false;

    if ( !ADD_TEST( _T("WinPresentMS"), CPresentWinConvertStretchMS)) return false;

    if ( !ADD_TEST( _T("FSPresent"), CPresentFS)) return false;

    if ( !ADD_TEST( _T("FSPresentMS"), CPresentFSMS)) return false;

    if ( !ADD_TEST( _T("FSPresentRotation"), CPresentFSRotation)) return false;

    if ( !ADD_TEST( _T("FSPresentMulti"), CPresentFSMulti)) return false;



    return true;
}


//--------------------------------------------------------------------

void CMyTestApp::InitOptionVariables()
{
    //
    // Call the base class implementation.
    //

    CDXGITestFramework::InitOptionVariables();


    //
    // Register test-specific debug options.
    //

    int CategoryDebug = AddOptionCategory(_T("Debug"));


    RegisterOptionVariable
        (
        _T("save"),
        _T("Saves DDS images for all failing tests in the current directory."),
        &m_saveDDS,
        false,
        OPTION_VALUE_CONSTANT,
        CategoryDebug
        );


    RegisterOptionVariable
        (
        _T("saveBMP"),
        _T("Saves bitmap images for all failing tests in the current directory."),
        &m_saveBMP,
        false,
        OPTION_VALUE_CONSTANT,
        CategoryDebug
        );


    RegisterOptionVariable<UINT>
        (
        _T("DebugPause"),
        _T("FS tests only. Sleeps in between presents and " \
        "GetDisplaySurfaceData for debugging."),
        &m_debugPause,
        (UINT)0,
        OPTION_VALUE_CONSTANT,
        CategoryDebug,
        _T("-DebugPause:SleepTime in ms")
        );


    RegisterOptionVariable
        (
        _T("RTL"),
        _T("Adds WS_EX_LAYOUTRTL to extended style for window creation"),
        &m_rtl,
        false,
        OPTION_VALUE_CONSTANT,
        CategoryDebug
        );


    RegisterOptionVariable
        (
        _T("NoLowResolution"),
        _T("FS tests only. Removes low resolution video modes " \
        "from test matrix"),
        &m_noLowResolution,
        false,
        OPTION_VALUE_CONSTANT,
        CategoryDebug
        );

    RegisterOptionVariable
        (
        _T("UseDesktopResolution"),
        _T("FS tests only. Use Desktop Resolution modes "),
        &m_useDesktopResolution,
        false,
        OPTION_VALUE_CONSTANT,
        CategoryDebug
        );

    RegisterOptionVariable
        (
        _T("CapMultiSampleQuality"),
        _T("MS (multisample) tests only. Caps NumQualityLevels"),
        &m_CapMultiSampleQuality,
        0x80000000,
        OPTION_VALUE_CONSTANT,
        CategoryDebug
        );

    RegisterOptionVariable
        (
        _T("MinResolutionSet"),
        _T("[default:true, false]: false will disable filtering resolutions to a minimum set."\
        "This will not override other test settings and switches."),
        &m_MinResolutionSet,
        true,
        OPTION_VALUE_CONSTANT,
        CategoryDebug
        );

    RegisterOptionVariable
        (
        _T("MinMultiSampleSet"),
        _T("[default:true, false]: false will disable filtering multiSampleQuality and multiSampleCounts to a minimum set."\
        "This will not override other test settings and switches."),
        &m_MinMultiSampleSet,
        true,
        OPTION_VALUE_CONSTANT,
        CategoryDebug
        );

    RegisterOptionVariable
    (
        _T("DFlip"),
        _T("Provides a way to specify only DirectFlip Testcases should be run, "\
           "only Windowed Tets groups are applicable."),
        &m_bDFlip,
        false,
        0,
        0,
        _T("DXGIPresent.exe -DFlip [testgroup]")
    );
}


bool CMyTestApp::Setup()
{
    //
    // Initialize locals.
    //

    bool bOverallResult = false;


	//
	// Mark our process as DPI-aware so as to prevent DWM
	// from lying to us about the desktop resolution.
	//

	SetProcessDPIAware();


	// Restrict to 1 type of adapter per test.
	if ( m_DXGIOptions.AdapterType[0] == TEST_DXGI_ADAPTER_TYPE_ALL )
		m_DXGIOptions.AdapterType[0] = TEST_DXGI_ADAPTER_TYPE_FULL_HW | TEST_DXGI_ADAPTER_TYPE_FULL_SW;

    //GetShell()->GetRenderWindow(0)->SetDeviceWindowFlags(DEVICEFLAGS_RESIZABLE);


    //
    // Enumerate User/GDI display devices.
    //

    // For each display device ordinal...
    for
        (
        m_cDisplayDevices = 0;
    m_cDisplayDevices < ARRAYSIZE( m_DisplayDeviceModes );
    ++m_cDisplayDevices
        )
    {
        // ...enumerate the device at this ordinal...
        ZeroMemory( &m_DisplayDevices[ m_cDisplayDevices ], sizeof( m_DisplayDevices[ m_cDisplayDevices ] ) );
        m_DisplayDevices[ m_cDisplayDevices ].cb = sizeof( m_DisplayDevices[ m_cDisplayDevices ] );

        BOOL bEnumerateDeviceResult = EnumDisplayDevices
            (
            NULL,
            m_cDisplayDevices,
            &m_DisplayDevices[ m_cDisplayDevices ],
            0
            );

        // ...and if there wasn't one...
        if ( bEnumerateDeviceResult != TRUE )
        {
            // ...then we're done with the enumeration.
            break;
        };


        // Skip devices that are not part of the desktop.
        if
            (
            ( m_DisplayDevices[ m_cDisplayDevices ].StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) == 0
            )
        {
            continue;
        };


        // Get the device's current mode.
        m_DisplayDeviceModes[ m_cDisplayDevices ].dmSize = sizeof( m_DisplayDeviceModes[ m_cDisplayDevices ] );
        m_DisplayDeviceModes[ m_cDisplayDevices ].dmSpecVersion = DM_SPECVERSION;

        BOOL bEnumDisplaySettingsResult = EnumDisplaySettings
            (
            m_DisplayDevices[ m_cDisplayDevices ].DeviceName,
            ENUM_CURRENT_SETTINGS,
            &m_DisplayDeviceModes[ m_cDisplayDevices ]
        );

        if ( bEnumDisplaySettingsResult != TRUE )
        {
            WriteToLog
                (
                "Setup:  EnumDisplaySettings failed."
                );

            goto Cleanup;
        };
    };

    bOverallResult = true;


Cleanup:
    return bOverallResult;
};


void CMyTestApp::Cleanup()
{
    //
    // Restore device modes.
    //

    for
        (
        DWORD iCurrentDisplayDevice = 0;
    iCurrentDisplayDevice < m_cDisplayDevices;
    ++iCurrentDisplayDevice
        )
    {
        // Skip devices that are not part of the desktop.
        if
            (
            ( m_DisplayDevices[ iCurrentDisplayDevice ].StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP ) == 0
            )
        {
            continue;
        };


        // Restore the device to its original mode.
        LONG RestoreDisplayDeviceModeResult = ChangeDisplaySettingsEx
            (
            m_DisplayDevices[ iCurrentDisplayDevice ].DeviceName,
            &m_DisplayDeviceModes[ iCurrentDisplayDevice ],
            NULL,
            CDS_UPDATEREGISTRY,
            NULL
            );

        if ( RestoreDisplayDeviceModeResult != DISP_CHANGE_SUCCESSFUL )
        {
            WriteToLogF
                (
                "Cleanup:  ChangeDisplaySettings failed with error %d while restoring mode on display device %u.",
                RestoreDisplayDeviceModeResult,
                iCurrentDisplayDevice
                );
        };
    };
};


CMyTestApp g_Application;
