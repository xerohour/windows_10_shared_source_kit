#include "AbstractTest.h"
#include <sstream>
#include <tuple>
#include "CheckAdapterReportedModes.h"
#include "CheckColorDepth.h"
#include "CheckPostAdapterBIOSColorDepth.h"
#include "Check10by7.h"
#include "CheckDualResolution.h"
#include "CheckTabletNativeResolution.h"
#include "Display.Monitor.Modes.h"

template<typename Test>
void run_test( LoggerWrapper* pLog, int argc, TCHAR *const* ppArgv )
{
    Test t( pLog );
    t.m_ppArgv = ppArgv;
    t.m_Argc = argc;
    t.RunTest();
}

namespace 
{
    LoggerWrapper* s_pLog = nullptr;
}

void LogMessage( wchar_t const* FMT, ... )
{
    va_list args;
    va_start( args, FMT );
    std::wstring str = FormatStringVATS( FMT, args );
    if( s_pLog )
    {
        s_pLog->m_pLogger->Trace( WTT_LVL_MSG, s_pLog->m_Handle, str.c_str() );
    }
    va_end(args);
}

void LogWarning( 
        wchar_t const* filename,
        UINT line,
        wchar_t const* FMT,
        ... )
{
    va_list args;
    va_start( args, FMT );
    std::wstring str = FormatStringVATS( FMT, args );
    if( s_pLog )
    {
        s_pLog->m_pLogger->Trace( WTT_LVL_WARN, s_pLog->m_Handle, filename, line, str.c_str() );
    }
    va_end(args);
}

void LogError( 
    DWORD error, 
    wchar_t const* filename, 
    wchar_t const* function, 
    UINT line, 
    wchar_t const* FMT, 
    ... )
{
    va_list args;
    va_start( args, FMT );
    std::wstring str = FormatStringVATS( FMT, args );
    if( s_pLog )
    {
        s_pLog->m_WTTResult = WTT_TESTCASE_RESULT_FAIL;

        if( s_pLog->m_bEnableDebugBreakOnFailure )
        {
            __debugbreak();
        }
        s_pLog->m_pLogger->Trace( 
                WTT_LVL_ERR, 
                s_pLog->m_Handle, 
                error, 
                WTT_ERROR_TYPE_HRESULT, 
                filename, 
                line, 
                str.c_str() );
    }
    va_end(args);
}

int __cdecl _tmain( int argc, TCHAR ** argv )
{
    CWTTLogger logger;
    bool bEdidCheck = false;
    bool bBiosCheck = false;
    bool bFilterCheck = false;
    bool bMinResCheck = false;
    bool bMinDualResCheck = false;
    bool bCheckTabletNativeResolution = false;
    bool bDisplayMonitorModes = false;

    LoggerWrapper wLog;
    wLog.m_pLogger = &logger;
    wLog.m_Handle = 0;
    wLog.m_WTTResult = WTT_TESTCASE_RESULT_PASS;
    wLog.m_bEnableDebugBreakOnFailure = false;

    tstring loggerString;
    tstring logName = _T("MinSpec.wtl");

    {
        bool bShowHelp = false;
        bool bEnableLogToConsole = false;
        bool bEnableLogToDebugger = false;


        typedef std::function<bool(int&, int, TCHAR**)> Func;

        const std::tuple<TCHAR const*, bool*, Func, TCHAR const*> pSwitch[] = 
        {
            std::make_tuple( _T("-?"), &bShowHelp, Func(), _T("Show this help message.") ),
            std::make_tuple( _T("-help"), &bShowHelp, Func(), _T("Show this help message.") ),
            std::make_tuple( _T("/help"), &bShowHelp, Func(), _T("Show this help message.") ),
            std::make_tuple( _T("/?"), &bShowHelp, Func(), _T("Show this help message.") ),
            std::make_tuple( _T("-debug"), &wLog.m_bEnableDebugBreakOnFailure, Func(), _T("Enable debug break on failure.") ),
            std::make_tuple( _T("-logToConsole"), &bEnableLogToConsole, Func(), _T("Enable logging to console.") ),
            std::make_tuple( _T("-logToDebugger"), &bEnableLogToDebugger, Func(), _T("Enable logging to the debugger.") ),
            std::make_tuple( _T("-EdidCheck"), &bEdidCheck, Func(), _T("Adapter reported modes match EDID WHQL test.") ),
            std::make_tuple( _T("-BiosCheck"), &bBiosCheck, Func(), _T("Adapter Bios reported modes not filtered WHQL test.") ),
            std::make_tuple( _T("-FilterCheck"), &bFilterCheck, Func(), _T("Adapter reported modes not filtered WHQL test.") ),
            std::make_tuple( _T("-MinResCheck"), &bMinResCheck, Func(), _T("Check that the minimum WHQL 1024 x 768 resolution is supported on the primary display.") ),
            std::make_tuple( _T("-MinDualResCheck"), &bMinDualResCheck, Func(), _T("Check that the minimum WHQL multi display resolution is supported.") ),
            std::make_tuple( _T("-MinTabletResCheck"), &bCheckTabletNativeResolution, Func(), _T("Check that the minimum WHQL tablet display native resolution is supported.") ),
            std::make_tuple( _T("-DisplayMonitorModes"), &bDisplayMonitorModes, Func(), _T("Check that the monitors support the required display modes.") ),
            std::make_tuple( _T("-logName"), nullptr, Func( [&logName]( int& i, int argc, TCHAR** argv ) -> bool
                {
                    if( ++i < argc )
                    {
                        logName = argv[i];
                        return true;
                    }
                    return false;
                } ), _T("Change the log file name.") )
        };

        for( int i = 1; i < argc; ++i )
        {
            bool found = false;
            for( UINT j = 0; j < sizeof(pSwitch)/sizeof(*pSwitch) && i < argc; ++j )
            {
                if( 0 == _tcsicmp( std::get<0>(pSwitch[j]), argv[i] ) )
                {
                    found = true;
                    if( std::get<1>(pSwitch[j]) )
                    {
                        *std::get<1>(pSwitch[j]) = true;
                    }

                    Func f = std::get<2>(pSwitch[j]);
                    if( f )
                    {
                        if( !f( i, argc, argv ) )
                        {
                            return -1;
                        }
                    }
                    break;
                }
            }

            // escape parameters for tests
            if( 0 == _tcsnicmp( argv[i], _T("/p:"), 3 ) )
            {
                found = true;
            }

            if( !found )
            {
                _ftprintf_s( stderr, _T("Unknown switch: %s\n"), argv[i] );
            }
        }

        if( bShowHelp )
        {
            _ftprintf_s( stdout, _T("Usage:\n") );
            for( int i = 0; i < ARRAY_SIZE(pSwitch); ++i )
            {
                _ftprintf_s( stdout, _T("\t%s\t%s\n"), std::get<0>(pSwitch[i]), std::get<3>(pSwitch[i]) );
            }

            return 0;
        }
        
        tstringstream s;
        s << _T("$LogFile:file=");
        s << logName;
        s << _T(",writemode=overwrite,encoding=UNICODE");
        
        if( bEnableLogToConsole || (bEnableLogToDebugger && IsDebuggerPresent()) )
        {
            bool empty = true;
            //s << _T("(");
            if( bEnableLogToConsole )
            {
                empty = false;
                s << _T(";$Console");
            }
            if( bEnableLogToDebugger && IsDebuggerPresent() )
            {
                if( !empty )
                {
                   // s << _T(";");
                }
                s << _T(";$Debugger");
            }
            //s << _T(")");
        }

        loggerString = s.str();
    }

    HRESULT hr;
    if( FAILED(hr = logger.CreateLogDevice( const_cast<TCHAR*>(loggerString.c_str()), &wLog.m_Handle )) )
    {
        if( wLog.m_bEnableDebugBreakOnFailure )
        {
            __debugbreak();
        }
        _ftprintf_s( stderr, _T("Create WTT log device failed: 0x%08X\n"), hr );
        Sleep(4000);
        return -1;
    }

    // TODO - make these tests modular (dll) so that the runner doesn't need to know
    // anything more than the interface details.
    if( bEdidCheck )
    {
        run_test<CheckAdapterReportedModes>( &wLog, argc, argv );
    }

    if( bFilterCheck )
    {
        run_test<CheckColorDepth>( &wLog, argc, argv );
    }

    if( bBiosCheck )
    {
        run_test<CheckPostAdapterBIOSColorDepth>( &wLog, argc, argv );
    }

    if( bMinResCheck )
    {
        run_test<Check10by7>( &wLog, argc, argv );
    }

    if( bMinDualResCheck )
    {
        run_test<CheckDualResolution>( &wLog, argc, argv );
    }

    if( bCheckTabletNativeResolution )
    {
        run_test<CheckTabletNativeResolution>( &wLog, argc, argv );
    }

    if( bDisplayMonitorModes )
    {
        run_test<DisplayMonitorModes>( &wLog, argc, argv );
    }

    if( FAILED(hr = logger.Trace( WTT_LVL_ROLLUP, NULL, 0, 0, 0, 0, 0 )) )
    {
        logger.Trace( WTT_LVL_ERR, wLog.m_Handle, hr, WTT_ERROR_TYPE_HRESULT, __TFILE__, __LINE__, _T("Failed to rollup results.") );
        _ftprintf( stderr, _T("Failed to rollup results: 0x%08X\n"), hr );
        Sleep(4000);
    }

    if( FAILED(hr = logger.CloseLogDevice( const_cast<wchar_t*>(loggerString.c_str()), wLog.m_Handle )) )
    {
        if( wLog.m_bEnableDebugBreakOnFailure )
        {
            __debugbreak();
        }
        _ftprintf_s( stderr, _T("Close WTT log device failed: 0x%08X\n"), hr );
        Sleep(4000);
        return -1;
    }

    wLog.m_Handle = 0;

    return 0;
}
