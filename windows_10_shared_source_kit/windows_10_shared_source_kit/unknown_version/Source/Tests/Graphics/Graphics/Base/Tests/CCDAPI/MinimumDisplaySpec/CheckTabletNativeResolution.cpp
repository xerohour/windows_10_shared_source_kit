#include "CheckTabletNativeResolution.h"
#include <regex>
#include <shellapi.h>
#include <functional>

typedef std::basic_string<TCHAR> tstring;
typedef std::basic_regex<TCHAR> tregex;
typedef std::match_results<tstring::const_iterator> tsmatch;

CheckTabletNativeResolution::CheckTabletNativeResolution( LoggerWrapper* pLogger )
    : AbstractTest( pLogger )
{
    ccd::Log::SetLogMessagePFN( &LogMessage );
    ccd::Log::SetLogWarningPFN( &LogWarning );
    ccd::Log::SetLogErrorPFN( &LogError );
}

void CheckTabletNativeResolution::InitializeParameters()
{
	UINT32 width = 1366u;
	UINT32 height = 768u;

	// command line parameters to line up against test case parameters
	typedef std::function<bool( tstring const& )> Func;
	std::pair<TCHAR const*, Func> params[] = 
	{
		std::make_pair( _T("MinimumDisplayPixelWidth"), Func( [&]( tstring const& str ) -> bool { return FromString( &width, str ); } ) ),
		std::make_pair( _T("MinimumDisplayPixelHeight"), Func( [&]( tstring const& str ) -> bool { return FromString( &height, str ); } ) )
	};

	// create the test case parameters
    CTestCaseParameter<UINT32>* pWidthParam = AddParameter<UINT32>( params[0].first, &m_MinimumDisplayPixelWidth );
    CTestCaseParameter<UINT32>* pHeightParam = AddParameter<UINT32>( params[1].first, &m_MinimumDisplayPixelHeight );

	// parentheticals are used for capture groups.
    tregex param( _T("^/p:(\\w+)=(\\w+)$") );

	// scan the command line for parameters and apply any that we come across
    if( m_ppArgv )
    {
        tsmatch matches;
        for( int i = 1; i < m_Argc; ++i )
        {
            tstring arg( m_ppArgv[i] );
            if( std::regex_match( arg, matches, param ) )
            {
                for( size_t j = 0; j < (sizeof(params)/sizeof(*params)); ++j )
                {
                    if( 0 == _tcsicmp( params[j].first, matches[1].str().c_str() ) )
                    {
						(params[j].second)( matches[2].str() );
                        break;
                    }
                }
            }
        }
    }

    testfactor::RFactor rfWidth = AddParameterValue<UINT32>( pWidthParam, width );
    testfactor::RFactor rfHeight = AddParameterValue<UINT32>( pHeightParam, height );
    testfactor::RFactor testcase = rfWidth * rfHeight;
    SetRootTestFactor( testcase );
}

TestResult::Type CheckTabletNativeResolution::SetupTest()
{
    m_DisplayTopology.PushDisplaySettings( QDC_ONLY_ACTIVE_PATHS );
    return TestResult::Pass;
}

TestResult::Type CheckTabletNativeResolution::SetupTestCase()
{
    return TestResult::Pass;
}

TestResult::Type CheckTabletNativeResolution::ExecuteTestCase()
{
    // Switch to Internal display only
    if( DISP_CHANGE_SUCCESSFUL != m_DisplayTopology.SetDisplayConfig( 0, nullptr, 0, nullptr, DISPLAYCONFIG_TOPOLOGY_INTERNAL | SDC_APPLY ) )
    {
        return TestResult::Fail;
    }

    // get the native resolution of the internal panel
    DISPLAY_DEVICE dd;
    memset( &dd, 0, sizeof(dd) );
    dd.cb = sizeof(dd);

    DEVMODE dm;
    memset( &dm, 0, sizeof(dm) );
    dm.dmSize = sizeof(dm);
    bool found = false;

    for( DWORD i = 0; EnumDisplayDevices( nullptr, i, &dd, 0 ); ++i )
    {
        if( dd.StateFlags & DISPLAY_DEVICE_ACTIVE && dd.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE )
        {
            #define ENUM_NATIVE_SETTINGS -3
            if( !EnumDisplaySettingsEx( dd.DeviceName, ENUM_NATIVE_SETTINGS, &dm, 0 ) )
            {
                LOG_ERROR( E_FAIL, _T("Failed to get the native resolution for the internal display %s"), dd.DeviceName );
                return TestResult::Fail;
            }
            found = true;
            break;
        }
    }

    if( !found )
    {
        LOG_ERROR( E_FAIL, _T("Failed to find the internal active display.") );
        return TestResult::Fail;
    }

    // swap the dimensions if the device prefers portrait instead of landscape
    if( dm.dmPelsHeight > dm.dmPelsWidth )
    {
        LOG_MESSAGE( _T("Swapping dimensions for portrait device.") );
        UINT32 temp = m_MinimumDisplayPixelWidth;
        m_MinimumDisplayPixelWidth = m_MinimumDisplayPixelHeight;
        m_MinimumDisplayPixelHeight = temp;
    }

    TestResult::Type result = TestResult::Pass;
    // Verify that the dimensions of the device meet the minimum resolution requirement
    if( dm.dmPelsWidth < m_MinimumDisplayPixelWidth )
    {
        LOG_ERROR( E_FAIL, _T("Display native width of %d does not meet the minimum requirement of %d."), dm.dmPelsWidth, m_MinimumDisplayPixelWidth );
        result = TestResult::Fail;
    }

    if( dm.dmPelsHeight < m_MinimumDisplayPixelHeight )
    {
        LOG_ERROR( E_FAIL, _T("Display native height of %d does not meet the minimum requirement of %d."), dm.dmPelsHeight, m_MinimumDisplayPixelHeight );
        result = TestResult::Fail;
    }

    if( TestResult::Pass != result )
    {
        return result;
    }

    // Test the native resolution
    DEVMODE mode;
    memset( &mode, 0, sizeof(mode) );
    mode.dmSize = sizeof(mode);
    mode.dmPelsWidth = dm.dmPelsWidth;
    mode.dmPelsHeight = dm.dmPelsHeight;
    mode.dmFields |= DM_PELSWIDTH | DM_PELSHEIGHT;

    LONG ret = m_DisplayTopology.ChangeDisplaySettingsEx(
            dd.DeviceName,
            &mode,
            0,
            CDS_RESET,
            0 );

    if( ret != DISP_CHANGE_SUCCESSFUL )
    {
        // error message already provided
        return TestResult::Fail;
    }
    return TestResult::Pass;
}

void CheckTabletNativeResolution::CleanupTestCase()
{
}

void CheckTabletNativeResolution::CleanupTest()
{
    m_DisplayTopology.PopDisplaySettings( QDC_ONLY_ACTIVE_PATHS );
}

tstring CheckTabletNativeResolution::GetTestName()
{
    return _T("Verify primary/internal tablet display minimum resolution requirements.");
}
