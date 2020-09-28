#include "Check10by7.h"
#include <regex>
#include <shellapi.h>
#include <functional>

typedef std::basic_string<TCHAR> tstring;
typedef std::basic_regex<TCHAR> tregex;
typedef std::match_results<tstring::const_iterator> tsmatch;

Check10by7::Check10by7( LoggerWrapper* pLogger )
    : AbstractTest( pLogger )
{
    ccd::Log::SetLogMessagePFN( &LogMessage );
    ccd::Log::SetLogWarningPFN( &LogWarning );
    ccd::Log::SetLogErrorPFN( &LogError );
}

void Check10by7::InitializeParameters()
{
	UINT32 width = 1024u;
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

TestResult::Type Check10by7::SetupTest()
{
    // Check if the CCD API is supported - requires WDDM
    {
        UINT32 paths = 0;
        UINT32 modes = 0;
        if( ERROR_NOT_SUPPORTED == GetDisplayConfigBufferSizes( QDC_ONLY_ACTIVE_PATHS, &paths, &modes ) )
        {
            OSVERSIONINFOEX info;
            memset( &info, 0, sizeof(info) );
            info.dwOSVersionInfoSize = sizeof(info);

            if( !GetVersionEx( (OSVERSIONINFO*)&info ) )
            {
                LOG_ERROR( HRESULT_FROM_WIN32(GetLastError()), _T("Failed to get OS version info.") );
                return TestResult::Fail;
            }
            
            // make sure that we only skip if the OS is Windows Server 2008 R2 with XDDM driver
            if( info.dwMajorVersion == 6 
                && info.dwMinorVersion == 1
                && info.wProductType != VER_NT_WORKSTATION )
            {
                LOG_MESSAGE( _T("Skipping test that requires CCD support which isn't available on the current system.") );
                return TestResult::Skip;
            }

            // Otherwise we don't know what this scenario is and so need to 'Discover' it.
            LOG_ERROR( ERROR_NOT_SUPPORTED, _T("This test requires WDDM support. Please verify the system supports WDDM and rerun the test.") );
            return TestResult::Fail;
        }
    }

    m_DisplayTopology.PushDisplaySettings( QDC_ONLY_ACTIVE_PATHS );
    return TestResult::Pass;
}

TestResult::Type Check10by7::SetupTestCase()
{
    return TestResult::Pass;
}

TestResult::Type Check10by7::ExecuteTestCase()
{
    // Switch to Internal display only
    if( DISP_CHANGE_SUCCESSFUL != m_DisplayTopology.SetDisplayConfig( 0, nullptr, 0, nullptr, DISPLAYCONFIG_TOPOLOGY_INTERNAL | SDC_APPLY ) )
    {
        return TestResult::Fail;
    }

    ccd::DisplaySettings ds;
    if( ERROR_SUCCESS != m_DisplayTopology.WrappedQueryDisplayConfig( QDC_ONLY_ACTIVE_PATHS, &ds ) )
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
                LOG_ERROR( E_FAIL, _T("Failed to get the native resolution for the internal display %s on connector %s"), 
                        dd.DeviceName,
                        ccd::str::ToString( ds.m_pPaths[0].targetInfo.outputTechnology ).c_str()
                        );
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

void Check10by7::CleanupTestCase()
{
}

void Check10by7::CleanupTest()
{
    m_DisplayTopology.PopDisplaySettings( QDC_ONLY_ACTIVE_PATHS );
}

tstring Check10by7::GetTestName()
{
    return _T("Verify primary/internal display minimum resolution requirements.");
}
