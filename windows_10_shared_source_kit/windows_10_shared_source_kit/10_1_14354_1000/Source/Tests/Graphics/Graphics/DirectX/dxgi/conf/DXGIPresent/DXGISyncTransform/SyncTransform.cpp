#include "TestApp.h"
#include "SyncTransform.h"

#pragma warning(push, 4)

bool CMyTestApp::InitTestGroups()
{
    return ADD_TEST(_T("StretchFactorX"), SyncTransform::StretchFactorX)&&
           ADD_TEST(_T("StretchFactorY"), SyncTransform::StretchFactorY) &&
           ADD_TEST(_T("OffsetX"), SyncTransform::OffsetX) &&
           ADD_TEST(_T("OffsetY"), SyncTransform::OffsetY) &&
           ADD_TEST(_T("StretchAndOffset"), SyncTransform::StretchOffsetCombo) &&
           ADD_TEST(_T("PlainRotation"), SyncTransform::PlainRotation) &&
           ADD_TEST(_T("RotationFactor"), SyncTransform::RotationFactor) &&
           ADD_TEST(_T("DirtyAndScrollRects"), SyncTransform::DirtyScroll) &&
           ADD_TEST(_T("PlainSourceSize"), SyncTransform::PlainSourceSize) &&
           ADD_TEST(_T("AllCombined"), SyncTransform::AllCombined) &&
           ADD_TEST(_T("OrderRotSrcSizeMtx"), SyncTransform::OrderRotSrcSizeMtx) &&
           ADD_TEST(_T("OrderRotSrcSize"), SyncTransform::OrderRotSrcSize) &&
           ADD_TEST(_T("OrderRotOffset"), SyncTransform::OrderRotOffset) &&
           ADD_TEST(_T("OrderRotStretch"), SyncTransform::OrderRotStretch) &&
           ADD_TEST(_T("OrderRotStretchOff"), SyncTransform::OrderRotStretchOff) &&
           ADD_TEST(_T("OrderSrcSizeOff"), SyncTransform::OrderSrcSizeOff) &&
           ADD_TEST(_T("OrderSrcSizeStretch"), SyncTransform::OrderSrcSizeStretch) &&
           ADD_TEST(_T("OrderSrcSizeStretchOff"), SyncTransform::OrderSrcSizeStretchOff) &&
           ADD_TEST(_T("SpecialCases"), SyncTransform::SpecialCases);
}

void CMyTestApp::InitOptionVariables()
{
    // Call the base class implementation.
    CDXGITestFramework::InitOptionVariables();

    // Register test-specific debug options.
    const int CategoryDebug = AddOptionCategory(_T("Debug"));

    #define OptVar(t1, t2, addr, val) RegisterOptionVariable( t1 , t2 , addr , val , OPTION_VALUE_CONSTANT , CategoryDebug )

    OptVar(_T("save"), _T("Saves DDS images for all failing tests in the current directory."), &m_saveDDS, false);
    OptVar(_T("saveBMP"), _T("Saves bitmap images for all failing tests in the current directory."), &m_saveBMP, false);
    OptVar(_T("RTL"), _T("Adds WS_EX_LAYOUTRTL to extended style for window creation"), &m_rtl, false);
    OptVar(_T("NoLowResolution"), _T("FS tests only. Removes low resolution video modes from test matrix"), &m_noLowResolution, false);
    OptVar(_T("UseDesktopResolution"), _T("FS tests only. Use Desktop Resolution modes "), &m_useDesktopResolution, false);
    OptVar(_T("CapMultiSampleQuality"), _T("MS (multisample) tests only. Caps NumQualityLevels"), &m_CapMultiSampleQuality, 0x80000000);
    OptVar(_T("MinResolutionSet"), _T("[default:true, false]: false will disable filtering resolutions to a minimum set. This will not override other test settings and switches."), &m_MinResolutionSet, true);
    OptVar(_T("MinMultiSampleSet"), _T("[default:true, false]: false will disable filtering multiSampleQuality and multiSampleCounts to a minimum set. This will not override other test settings and switches."), &m_MinMultiSampleSet, true);
    RegisterOptionVariable<UINT>(_T("DebugPause"), _T("FS tests only. Sleeps in between presents and GetDisplaySurfaceData for debugging."), &m_debugPause, false, OPTION_VALUE_CONSTANT, CategoryDebug, _T("-DebugPause:SleepTime in ms"));

    #undef OptVar
}

bool CMyTestApp::Setup()
{
    // Restrict to 1 type of adapter per test.
    if (m_DXGIOptions.AdapterType[0] == TEST_DXGI_ADAPTER_TYPE_ALL) m_DXGIOptions.AdapterType[0] = TEST_DXGI_ADAPTER_TYPE_FULL_HW | TEST_DXGI_ADAPTER_TYPE_FULL_SW;

    //
    // Enumerate User/GDI display devices.
    //
    for(m_cDisplayDevices = 0; m_cDisplayDevices < ARRAYSIZE(m_DisplayDeviceModes); ++m_cDisplayDevices)
    {
        ZeroMemory(&m_DisplayDevices[m_cDisplayDevices], sizeof(m_DisplayDevices[m_cDisplayDevices]));
        m_DisplayDevices[m_cDisplayDevices].cb = sizeof(m_DisplayDevices[m_cDisplayDevices]);

        if (!EnumDisplayDevices(NULL, m_cDisplayDevices, &m_DisplayDevices[m_cDisplayDevices], 0))
        {
            return true;
        }

        // Skip devices that are not part of the desktop.
        if (!(m_DisplayDevices[m_cDisplayDevices].StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP))
        {
            continue;
        }

        // Get the device's current mode.
        m_DisplayDeviceModes[m_cDisplayDevices].dmSize = sizeof(m_DisplayDeviceModes[m_cDisplayDevices]);
        m_DisplayDeviceModes[m_cDisplayDevices].dmSpecVersion = DM_SPECVERSION;
        if (!EnumDisplaySettings(m_DisplayDevices[m_cDisplayDevices].DeviceName, ENUM_CURRENT_SETTINGS, &m_DisplayDeviceModes[m_cDisplayDevices]))
        {
            WriteToLog(_T("Setup: EnumDisplaySettings failed."));
            return false;
        }
    }

    return true;
}

void CMyTestApp::Cleanup()
{
    for(unsigned i = 0; i < m_cDisplayDevices; ++i)
    {
        // Skip devices that are not part of the desktop.
        if(m_DisplayDevices[i].StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
        {
            // Restore the device to its original mode.
            const auto r = ChangeDisplaySettingsEx(m_DisplayDevices[i].DeviceName, &m_DisplayDeviceModes[i], NULL, CDS_UPDATEREGISTRY, NULL);
            if (DISP_CHANGE_SUCCESSFUL != r)
            {
                WriteToLogF(_T("Cleanup: ChangeDisplaySettings failed with error %d while restoring mode on display device %u."), r, i);
            }
        }
    }
}

CMyTestApp g_Application;

#pragma warning(pop)