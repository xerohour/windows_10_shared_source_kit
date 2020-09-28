#if !defined(_NTSTATUS_)
typedef long NTSTATUS;
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)
#endif
#include "windows.h"
#include "d3dkmthk.h"
#include "CheckAdapterReportedModes.h"
#include "XMLDataMapping.h"
#include "CustomTestFactors.h"
#include <tchar.h>
#include "MonitorEdidInfo.h"
#include "stringutils.h"

using namespace DEVX;
using namespace DEVX::GRFX;

namespace
{
    static const wchar_t g_GoldModeList[] =
        L"<RequiredDisplayModes>"
        L"    <SpecList>"
        L"        <!-- all under 148.5 MHz pixel frequency -->"
        L"        <Spec version=\"CEA-861-E\" date=\"2008.7.10\">"
        L"            <!-- p -->"
        L"            <ModeList>"
        L"                <Mode width=\"1280\" height=\"720\" freq=\"59.94\"/>"
        L"                <Mode width=\"1280\" height=\"720\" freq=\"60\"/>"
        L"                <Mode width=\"1280\" height=\"720\" freq=\"50\"/>"
        L"                <!-- p or i -->"
        L"                <Mode width=\"1920\" height=\"1080\" freq=\"59.94\"/>"
        L"                <Mode width=\"1920\" height=\"1080\" freq=\"60\"/>"
        L"                <Mode width=\"1920\" height=\"1080\" freq=\"50\"/>"
        L"                <Mode width=\"1920\" height=\"1080\" freq=\"23.97\"/>"
        L"                <Mode width=\"1920\" height=\"1080\" freq=\"24\"/>"
        L"                <Mode width=\"1920\" height=\"1080\" freq=\"25\"/>"
        L"                <Mode width=\"1920\" height=\"1080\" freq=\"29.97\"/>"
        L"                <Mode width=\"1920\" height=\"1080\" freq=\"30\"/>"
        L"            </ModeList>"
        L"        </Spec>"
        L"        <Spec version=\"VESA V1.0 Rev 11\" date=\"2007.5.1\">"
        L"            <ModeList>"
        L"                <Mode width=\"1024\" height=\"768\" freq=\"43\"/>"
        L"                <Mode width=\"1024\" height=\"768\" freq=\"60\"/>"
        L"                <Mode width=\"1024\" height=\"768\" freq=\"70\"/>"
        L"                <Mode width=\"1024\" height=\"768\" freq=\"75\"/>"
        L"                <Mode width=\"1024\" height=\"768\" freq=\"85\"/>"
        L"                <Mode width=\"1024\" height=\"768\" freq=\"120\"/>"
        L""         
        L"                <Mode width=\"1152\" height=\"864\" freq=\"75\"/>"
        L""         
        L"                <Mode width=\"1280\" height=\"768\" freq=\"60\"/>"
        L"                <Mode width=\"1280\" height=\"768\" freq=\"75\"/>"
        L"                <Mode width=\"1280\" height=\"768\" freq=\"85\"/>"
        L"                <Mode width=\"1280\" height=\"768\" freq=\"120\"/>"
        L""         
        L"                <Mode width=\"1280\" height=\"960\" freq=\"60\"/>"
        L"                <Mode width=\"1280\" height=\"960\" freq=\"85\"/>"
        L""         
        L"                <Mode width=\"1280\" height=\"1024\" freq=\"60\"/>"
        L"                <Mode width=\"1280\" height=\"1024\" freq=\"75\"/>"
        L""         
        L"                <Mode width=\"1400\" height=\"1050\" freq=\"60\"/>"
        L""         
        L"                <Mode width=\"1400\" height=\"900\" freq=\"60\"/>"
        L"                <Mode width=\"1400\" height=\"900\" freq=\"75\"/>"
        L""         
        L"                <Mode width=\"1680\" height=\"1050\" freq=\"60\"/>"
        L"            </ModeList>"
        L"        </Spec>"
        L"        <Spec version=\"VESA V1.0 Rev 11\" date=\"2007.11.30\">"
        L"            <ModeList>"
        L"                <Mode width=\"1366\" height=\"768\" freq=\"60\"/>"
        L"                <Mode width=\"1920\" height=\"1080\" freq=\"60\"/>"
        L"            </ModeList>"
        L"        </Spec>"
        L"    </SpecList>"
        L"</RequiredDisplayModes>";

}

// if the test requires multimon, make sure it's available.
bool DoesConfigMeetMultiMonRequirements()
{
    for( int i = 1; i < __argc; ++i )
    {
        if( 0 == _tcsicmp( __targv[i], _T("-MultiMon") ) )
        {
            DISPLAY_DEVICE dd;
            memset( &dd, 0, sizeof(dd) );
            dd.cb = sizeof(dd);
            int monCount = 0;
            for( DWORD j = 0; EnumDisplayDevices( nullptr, j, &dd, 0 ); ++j )
            {
                if( dd.StateFlags & DISPLAY_DEVICE_ACTIVE )
                {
                    ++monCount;
                }
            }
            if( monCount < 2 )
            {
                return false;
            }
        }
    }

    return true;
}

struct CheckAdapterReportedModes::Internals
{
    ResMapFreqSet m_SpecList;
};

CheckAdapterReportedModes::CheckAdapterReportedModes( LoggerWrapper* pLogger )
    : EdidTestBase(pLogger)
    , m_pInternals( nullptr )
{
    ccd::Log::SetLogMessagePFN( &LogMessage );
    ccd::Log::SetLogWarningPFN( &LogWarning );
    ccd::Log::SetLogErrorPFN( &LogError );
}

struct DisplayMode
{
    UINT32 m_Width;
    UINT32 m_Height;
    float m_VRefresh;
};

struct DisplayModeSpec
{
    std::wstring m_SpecVersion;
    std::wstring m_Date;
    std::vector<DisplayMode> m_Modes;
};

struct RequiredDisplayModes
{
    std::vector<DisplayModeSpec> m_Specs;
};

TestResult::Type CheckAdapterReportedModes::SetupTest()
{
    TestResult::Type result = EdidTestBase::SetupTest();
    if( result != TestResult::Pass )
    {
        return result;
    }

    if( !DoesConfigMeetMultiMonRequirements() )
    {
        LOG_MESSAGE( _T("Test machine doesn't meet multimon config requirements.") );
        return TestResult::Skip;
    }
    
    static bool loaded = false;
    if( !loaded )
    {
        xdm::InitBasicTypes();

        xdm::struct_s< DisplayMode >::info().SetName( L"Mode" )
            .prop_data( L"width", &DisplayMode::m_Width )
            .prop_data( L"height", &DisplayMode::m_Height )
            .prop_data( L"freq", &DisplayMode::m_VRefresh );

        xdm::vector_s< xdm::struct_s<DisplayMode> >::info().SetName( L"ModeList" );

        xdm::struct_s< DisplayModeSpec >::info().SetName( L"Spec" )
            .prop_data( L"version", &DisplayModeSpec::m_SpecVersion )
            .prop_data( L"data", &DisplayModeSpec::m_Date )
            .tag< xdm::vector_s< xdm::struct_s<DisplayMode> >>( L"ModeList", &DisplayModeSpec::m_Modes );

        xdm::vector_s< xdm::struct_s<DisplayModeSpec> >::info().SetName( L"SpecList" );

        xdm::struct_s< RequiredDisplayModes >::info().SetName( L"RequiredDisplayModes" )
            .tag< xdm::vector_s< xdm::struct_s<DisplayModeSpec> > >( L"SpecList", &RequiredDisplayModes::m_Specs );

        loaded = true;
    }

    if( !m_pInternals )
    {
        RequiredDisplayModes modes;

        if( !xdm::LoadXMLData( _T("RequiredDisplayModes"), xdm::struct_s<RequiredDisplayModes>(modes), g_GoldModeList ) )
        {
            return TestResult::Fail;
        }

        m_pInternals = new Internals();

        for( auto it = modes.m_Specs.begin(), eit = modes.m_Specs.end();
            it != eit; ++it )
        {
            for( auto mIt = it->m_Modes.begin(), mEIt = it->m_Modes.end();
                mIt != mEIt; ++mIt )
            {
                Resolution r = { (USHORT)mIt->m_Height, (USHORT)mIt->m_Width };
                // insert both high and low refreshrates to manage rounding
                UINT32 high = (UINT32)ceil(mIt->m_VRefresh);
                UINT32 low = (UINT32)floor(mIt->m_VRefresh);
                m_pInternals->m_SpecList[r].m_VerticalRefreshSet.insert( high );
                m_pInternals->m_SpecList[r].m_VerticalRefreshSet.insert( low );
            }
        }
    }

    return TestResult::Pass;
}

BOOL CALLBACK MonitorEnumProc(
  __in  HMONITOR hMonitor,
  __in  HDC hdcMonitor,
  __in  LPRECT lprcMonitor,
  __in  LPARAM dwData
)
{
    MONITORINFOEX info;
    memset( &info, 0, sizeof(info) );
    info.cbSize = sizeof(info);
    GetMonitorInfo( hMonitor, &info );
    _tprintf_s( _T("%s\n"), info.szDevice );
    return TRUE;
}

TestResult::Type GetDisplayModeList( DISPLAY_DEVICE const* pDisplay, ResMapFreqSet* pEdidResSet )
{
    DEVMODE dm;
    memset( &dm, 0, sizeof(dm) );
    dm.dmSize = sizeof(dm);

    for( DWORD i = 0; EnumDisplaySettingsEx( pDisplay->DeviceName, i, &dm, 0 ); ++i )
    {
        Resolution r = { (USHORT)dm.dmPelsHeight, (USHORT)dm.dmPelsWidth };
        (*pEdidResSet)[r].m_VerticalRefreshSet.insert( dm.dmDisplayFrequency );
    }

    return TestResult::Pass;
}

void print( char const* pStr, ResMapFreqSet const* pMap )
{
    for( auto resIt = pMap->begin(), resEIt = pMap->end();
        resIt != resEIt; ++resIt )
    {
        for( auto refIt = resIt->second.m_VerticalRefreshSet.begin(), refEIt = resIt->second.m_VerticalRefreshSet.end();
            refIt != refEIt; ++refIt )
        {
            printf( "%s: %dx%d @ %f\n", pStr, resIt->first.x, resIt->first.y, *refIt );
        }
    }
}

TestResult::Type CheckAdapterReportedModes::Verify( DISPLAY_DEVICE const* pAdapter, DISPLAY_DEVICE const* pDisplay )
{
    LOG_MESSAGE( _T("Validating Adapter:\n\t%s\n\t%s\n\t%s\n\t\t%s\n\t\t%S\n\t\t%s"),
        pAdapter->DeviceName,
        pAdapter->DeviceString,
        pAdapter->DeviceID,
        pDisplay->DeviceName,
        pDisplay->DeviceString,
        pDisplay->DeviceID );

    // get the supported list from the edid
    ResMapFreqSet edidResolutions;
    TestResult::Type result = GetMonitorEdidModeList( m_pLogger, pDisplay, &edidResolutions );
    if( result != TestResult::Pass )
    {
        return result;
    }

    // get the supported list from the adapter
    ResMapFreqSet displayResolutions;
    result = GetDisplayModeList( pAdapter, &displayResolutions );
    if( result != TestResult::Pass )
    {
        return result;
    }

    // now check that everything worked as expected.

    // find the intersect between the spec and the edid.
    ResMapFreqSet requiredResolutions;
    for( auto it = m_pInternals->m_SpecList.begin(), eit = m_pInternals->m_SpecList.end(); it != eit; ++it )
    {
        auto edidIt = edidResolutions.find( it->first );
        if( edidIt == edidResolutions.end() )
        { 
            continue;
        }

        for( auto refIt = it->second.m_VerticalRefreshSet.begin(), refEIt = it->second.m_VerticalRefreshSet.end();
            refIt != refEIt; ++refIt )
        {
            auto edidRefIt = edidIt->second.m_VerticalRefreshSet.find( *refIt );
            if( edidRefIt != edidIt->second.m_VerticalRefreshSet.end() )
            {
                requiredResolutions[it->first].m_VerticalRefreshSet.insert( *refIt );
            }
        }
    }

    // now make sure that the adapter reported modes include these spec required and edid supported display modes.
    for( auto resIt = requiredResolutions.begin(), resEIt = requiredResolutions.end();
        resIt != resEIt; ++resIt )
    {
        auto repRes = displayResolutions.find( resIt->first );
        if( repRes == displayResolutions.end() )
        {
            LOG_ERROR(
                E_FAIL,
                _T("Adapter failed to report required resolution %dx%d on display %s which is supported in the EDID."),
                (UINT32)resIt->first.x,
                (UINT32)resIt->first.y,
                pAdapter->DeviceName );
            result = TestResult::Fail;
        }
        else
        {
            for( auto refIt = resIt->second.m_VerticalRefreshSet.begin(), refEIt = resIt->second.m_VerticalRefreshSet.end();
                refIt != refEIt; ++refIt )
            {
                // Check for matches with a tolerance of +/- 1 Hz. This is due to complications from how rounding
                // is handled for TV-compatible modes. There are some cases that should match, but due to the way
                // EnumDisplaySettingsEx rounds, they may not be an exact match.
                for( auto TargetRate = *refIt - 1; TargetRate <= *refIt + 1; TargetRate++ )
                {
                    if( repRes->second.m_VerticalRefreshSet.count(TargetRate) != 0 )
                    {
                        resIt->second.m_SupportStatus = true;
                    }
                }
                if( resIt->second.m_SupportStatus )
                {
                    break;
                }
            }

            if( false == resIt->second.m_SupportStatus )
            {
                tstringstream wss;
                wss << _T("Expected one of the following refresh rates to be available: ");
                for( auto refIt = resIt->second.m_VerticalRefreshSet.begin(), refEIt = resIt->second.m_VerticalRefreshSet.end();
                    refIt != refEIt; ++refIt )
                {
                    UINT32 v = (UINT32)*refIt;
                    if( refIt != resIt->second.m_VerticalRefreshSet.begin() )
                    {
                        wss << _T(", ");
                    }
                    wss << v;
                }

                // report missing mode
                LOG_ERROR( 
                    E_FAIL,
                    _T("Adapter failed to report required mode supported by the display: %dx%d.  %s"),
                    (UINT32)resIt->first.x,
                    (UINT32)resIt->first.y,
                    wss.str().c_str() );
                result = TestResult::Fail;
            }
        }
    }

    return result;
}

TestResult::Type CheckAdapterReportedModes::ExecuteTestCase()
{
    return Verify( &m_GDISource, &m_GDITarget );
}

void CheckAdapterReportedModes::CleanupTest()
{
    delete m_pInternals;
    m_pInternals = nullptr;

    EdidTestBase::CleanupTest();
}

tstring CheckAdapterReportedModes::GetTestName()
{
    return _T("Check Adapter Reported Modes.");
}
