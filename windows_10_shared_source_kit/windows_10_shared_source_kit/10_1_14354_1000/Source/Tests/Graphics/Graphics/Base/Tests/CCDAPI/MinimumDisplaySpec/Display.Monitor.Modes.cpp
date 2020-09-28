#include "Display.Monitor.Modes.h"
#include "MonitorEdidInfo.h"
#include <algorithm>


DisplayMonitorModes::DisplayMonitorModes( LoggerWrapper* pLogger )
    : EdidTestBase( pLogger )
{
}

TestResult::Type DisplayMonitorModes::ExecuteTestCase()
{
    // Figure out if this is an internal test or a digital test.
    switch( m_TestCasePath.targetInfo.outputTechnology )
    {
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EMBEDDED:
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_UDI_EMBEDDED:
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL:
        {
            // get the native resolution and make sure it's at least 1024x768 60hz progressive
            DEVMODE dm;
            memset( &dm, 0, sizeof(dm) );
            dm.dmSize = sizeof(dm);

            #define ENUM_NATIVE_SETTINGS -3
            if( !EnumDisplaySettingsEx( m_GDISource.DeviceName, ENUM_NATIVE_SETTINGS, &dm, 0 ) )
            {
                LOG_ERROR( E_FAIL, _T("Failed to get the native resolution for the internal display %s"), m_GDISource.DeviceName );
                return TestResult::Fail;
            }

            // swap the dimensions if the device prefers portrait instead of landscape
            if( dm.dmPelsHeight > dm.dmPelsWidth )
            {
                LOG_MESSAGE( _T("Swapping dimensions for portrait device.") );
                UINT32 temp = dm.dmPelsHeight;
                dm.dmPelsHeight = dm.dmPelsWidth;
                dm.dmPelsWidth = temp;
            }

            TestResult::Type result = TestResult::Pass;
            // Verify that the dimensions of the device meet the minimum resolution requirement
            if( dm.dmPelsWidth < 1024 )
            {
                LOG_ERROR( E_FAIL, _T("Display native width of %d does not meet the minimum requirement of %d."), dm.dmPelsWidth, 1024 );
                result = TestResult::Fail;
            }

            if( dm.dmPelsHeight < 768 )
            {
                LOG_ERROR( E_FAIL, _T("Display native height of %d does not meet the minimum requirement of %d."), dm.dmPelsHeight, 768 );
                result = TestResult::Fail;
            }

            if( dm.dmDisplayFlags & DM_INTERLACED )
            {
                LOG_ERROR( E_FAIL, _T("Display native scan line ordering cannot be interlaced.") );
                result = TestResult::Fail;
            }

            // allow 59 to sheild against TVs causing false failures
            if( dm.dmDisplayFrequency < 59 )
            {
                LOG_ERROR( E_FAIL, _T("Display native refresh rate cannot be less than 60, found %d."), dm.dmDisplayFrequency );
                result = TestResult::Fail;
            }

            if( TestResult::Pass != result )
            {
                return result;
            }

            break;
        }

        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HD15:
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DVI:
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_HDMI:
        case DISPLAYCONFIG_OUTPUT_TECHNOLOGY_DISPLAYPORT_EXTERNAL:
        {
            LOG_MESSAGE( _T("Adapter Name: %s\nAdapter String: %s\nAdapter ID: %s\n\n"),
                m_GDISource.DeviceName,
                m_GDISource.DeviceString,
                m_GDISource.DeviceID,
                m_GDISource.DeviceKey );

            LOG_MESSAGE( _T("Monitor Name: %s\nMonitor String: %s\nMonitor Interface Path: %s\n\n"),
                m_GDITarget.DeviceName,
                m_GDITarget.DeviceString,
                m_GDITarget.DeviceID,
                m_GDITarget.DeviceKey );
    
            UCHAR edid[128];
            memset( edid, 0, sizeof(edid) );

            HRESULT hr = DEVX::GRFX::EnumEdid( m_pLogger, &m_GDITarget, edid, 0 );
            if( FAILED(hr) )
            {
                LOG_ERROR( hr, _T("Failed to get the base edid for %s"), m_GDITarget.DeviceName );
                return TestResult::Fail;
            }

            // CHECK HCK requirement
            TestResult::Type result = TestResult::Pass;
            if( ! (edid[0x23] & 0x20) )
            {
                LOG_ERROR(
                        E_FAIL, 
                        _T("Monitor %s fails to support required resolution.\n")
                        _T("Established timings EDID byte %dh, bit %d, %d x %d at %d Hz %s"),
                        m_GDITarget.DeviceName,
                        23,
                        5,
                        640,
                        480,
                        60,
                        _T("progressive") );
                result = TestResult::Fail;
            }

            if( ! (edid[0x23] & 0x1) )
            {
                LOG_ERROR(
                        E_FAIL, 
                        _T("Monitor %s fails to support required resolution.\n")
                        _T("Established timings EDID byte %dh, bit %d, %d x %d at %d Hz %s"),
                        m_GDITarget.DeviceName,
                        23,
                        0,
                        800,
                        600,
                        60,
                        _T("progressive") );
                result = TestResult::Fail;
            }

            if( ! (edid[0x24] & 0x8) )
            {
                LOG_ERROR(
                        E_FAIL, 
                        _T("Monitor %s fails to support required resolution.\n")
                        _T("Established timings EDID byte %dh, bit %d, %d x %d at %d Hz %s"),
                        m_GDITarget.DeviceName,
                        24,
                        3,
                        1024,
                        768,
                        60,
                        _T("progressive") );
                result = TestResult::Fail;
            }

            // log detailed information whenever the test case fails
            if( result != TestResult::Pass )
            {
                { // log the edid in question
                    TCHAR buf[1024];
                    UINT32 x = 0;
                    for( UINT32 i = 0; i < ARRAY_SIZE(edid); ++i )
                    {
                        if( i != 0 )
                        {
                            if( 0 == i % 8 )
                            {
                                x += _stprintf_s( 
                                    buf + x, 
                                    ARRAY_SIZE(buf) - x, 
                                    _T("\n") );
                            }
                            else
                            {
                                x += _stprintf_s( 
                                    buf + x, 
                                    ARRAY_SIZE(buf) - x, 
                                    _T(" ") );
                            }
                        }

                        x += _stprintf_s( 
                            buf + x, 
                            ARRAY_SIZE(buf) - x, 
                            _T("%02X"),
                            edid[i] );
                    }

                    LOG_MESSAGE( buf );
                }

                // provide more detail on the display
                DEVX::GRFX::LogMonitorDetails( m_pLogger, &m_GDITarget );

                return result;
            }

            break;
        }

        default:
            LOG_MESSAGE( _T("Test cannot verify adapter: %s target: 0x%X with connector %s"), 
                    ToString( m_AdapterLuid ).c_str(),
                    m_TargetId,
                    ccd::str::ToString( m_TestCasePath.targetInfo.outputTechnology ).c_str() ); 
            return TestResult::Skip;
    }

    return TestResult::Pass;
}

tstring DisplayMonitorModes::GetTestName()
{
    return _T("Verify each display meets resolution support requirements.");
}

