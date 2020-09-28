//--------------------------------------------------------------------------;
//
//  File: DeviceResponsivenessTest.cpp
//
// Copyright (C) Microsoft. All rights reserved.
//
//  History:
//      01/02/2012    Rbeber       Initial implementation.
//      06/23/2014    SMunday      Migrate test code to AVCore depot.
//
//--------------------------------------------------------------------------;

#include "common.h"
#include <audioclient.h>
#include <evntcons.h>
#include <itestresource.h>
#include <resourcelist.h>
#include "ETWLogger.h"
using namespace WEX::Logging;
using namespace WEX::TestExecution;
#include "PortClsEventLogger.h"
#include "SetupApiHelper.h"
#include "DeviceSupport.h"

#include <wdtf.h>
#include <wdtfsystemaction.h>
#include <WDTFSimulatedBatterySystemAction.h>
#include <lpe.h>

#include "AdapterPowerManagementTest.h"
#include "AdapterPowerManagement3.h"
#include "DeviceResponsivenessTest.h"

const UINT uiResponseFastSleepPeriodMs = 30;
const UINT uiResponseResponsiveSleepPeriodMs = 300;
const UINT uiGraceTime = 3000;
const UINT uiInitialWaitTimeMS = 30000;

bool DeviceResponsivenessTest::SetupClass()
{
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    m_PowerNotifyStruct[0].pGUID = &GUID_MONITOR_POWER_ON;
    m_PowerNotifyStruct[0].hPowerNotify = NULL;

    m_PowerNotifyStruct[1].pGUID = &GUID_LOW_POWER_EPOCH;
    m_PowerNotifyStruct[1].hPowerNotify = NULL;

    if( !VERIFY_SUCCEEDED( CreateWexBasicLog( &m_spBasicLog ) ) ) { return false; }

    if( !VERIFY_IS_NOT_NULL( m_hLowPowerEpochExit = CreateEvent( NULL, FALSE, FALSE, NULL ) ) ) { return false; }

    if( !VERIFY_IS_NOT_NULL( m_hLowPowerEpochEnter = CreateEvent( NULL, FALSE, FALSE, NULL ) ) ) { return false; }

    if( !VERIFY_IS_NOT_NULL( m_hScreenOff = CreateEvent( NULL, FALSE, FALSE, NULL ) ) ) { return false; }

    if( !VERIFY_IS_NOT_NULL( m_hScreenOn = CreateEvent( NULL, FALSE, FALSE, NULL ) ) ) { return false; }

    if( !VERIFY_SUCCEEDED( SetupSimBatt())) { return false; }

    return true;
}

bool DeviceResponsivenessTest::CleanupClass()
{
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);
    bool returnValue = true;

    if( NULL != m_hScreenOn )
    {
        CloseHandle( m_hScreenOn );
        m_hScreenOn = NULL;
    }

    if( NULL != m_hScreenOff )
    {
        CloseHandle( m_hScreenOff );
        m_hScreenOff = NULL;
    }

    if( NULL != m_hLowPowerEpochEnter )
    {
        CloseHandle( m_hLowPowerEpochEnter );
        m_hLowPowerEpochEnter = NULL;
    }

    if( NULL != m_hLowPowerEpochExit )
    {
        CloseHandle( m_hLowPowerEpochExit );
        m_hLowPowerEpochExit = NULL;
    }
    
    if( !VERIFY_SUCCEEDED( CleanupSimBatt()) ) 
    {   
        returnValue = false;
    }

    return returnValue;
}

bool DeviceResponsivenessTest::SetupTest()
{
    return true;
}

bool DeviceResponsivenessTest::CleanupTest()
{
    return true;
}

void DeviceResponsivenessTest::PowerChangeHCK_ScreenOn_ResponseFast()
{
    HRESULT hr = S_OK;
    CComPtr<IAudioClient> spAudioClient;

    //All audio devices enter D3 after 30s of inactivity regardless of AC/DC
    Sleep(uiInitialWaitTimeMS);

    // Audio Devnode Must be in D3
    hr = DeviceUnderTestPowerState( PowerDeviceD3 );
    if( E_UNEXPECTED == hr )
    {
        SKIP( m_spBasicLog, L"SKIP: Device did not enter PowerDeviceD3.  Possible codec does not support EPSS." );
        return;
    }

    Sleep( 1000 ); // BUGBUG: waiting a second for the MMDevices to become active (add listening routine in SetupTest)

    if( !VERIFY_SUCCEEDED( DeviceUnderTestIncreasePinCount( &spAudioClient ) ) ) { return; }
    Sleep(uiResponseFastSleepPeriodMs);
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) 
    {
        LOG( m_spBasicLog, "ERROR: Device was not in D0 power state within %d ms of playing a stream in screen on.",
            uiResponseFastSleepPeriodMs);

        //sleep a few seconds and see if it is in D0 at all.
        Sleep(uiGraceTime);
        if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) 
        {
            LOG( m_spBasicLog, "ERROR: Device was still not in D0 power state within %d ms of playing a stream in screen on after initial failure.",
                uiResponseFastSleepPeriodMs + uiGraceTime);
        }
        return; 
    }
    
    return;
}

void DeviceResponsivenessTest::PowerChangeHCK_ScreenOff_ResponseResponsive()
{
    HRESULT hr = S_OK;
    bool isAoAc = false;
    CComPtr<IAudioClient> spAudioClient;
    bool bLogLater = false;

    // Prerequisite #1: System Must Support AoAc
    if( !VERIFY_SUCCEEDED( DoesSystemSupportAoAc( &isAoAc ) ) ) { return; }
    if (!isAoAc)
    {
        SKIP( m_spBasicLog, L"SKIP: System does not support AOAC. Skipping Screen-off test." );
        return;
    }

    //All audio devices enter D3 after 30s of inactivity regardless of AC/DC
    Sleep(uiInitialWaitTimeMS);

    // Prerequisite #3: Audio Devnode Must be in D3
    hr = DeviceUnderTestPowerState( PowerDeviceD3 );
    if( E_UNEXPECTED == hr )
    {
        SKIP( m_spBasicLog, L"SKIP: Device did not enter PowerDeviceD3.  Possible codec does not support EPSS." );

        return;
    }

    Sleep( 1000 ); // BUGBUG: waiting a second for the MMDevices to become active (add listening routine in SetupTest)

    // Register for Power Broadcast Events
    if( !VERIFY_SUCCEEDED( RegisterForPowerBroadcasts() ) ) { return; }
    OnExit unregisterPowerBroadcasts( [&]()
    {
        (void) UnregisterForPowerBroadcasts();
    } );

    // enter Connected Standby
    if( !VERIFY_SUCCEEDED( EnterConnectedStandby() ) ) { return; }

    // go to D0 by re-opening an Audio Client (D3->D0)
    if( !VERIFY_SUCCEEDED(hr = DeviceUnderTestIncreasePinCount( &spAudioClient, true ) ) ) 
    { 
        goto CLEANUP;
    }
    if (hr == S_FALSE)
    {   //s_false only returned from skipping due to all digital endpoints on DUT
        goto CLEANUP; 
    }

    Sleep(uiResponseResponsiveSleepPeriodMs);
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) 
    { 
        bLogLater = true;
    }

CLEANUP:
    // exit Connected Standby
    VERIFY_SUCCEEDED( ExitConnectedStandby() );

    // Unregister for Power Broadcast Events
    VERIFY_SUCCEEDED( UnregisterForPowerBroadcasts() );

    if (bLogLater)
    {
        ERR( m_spBasicLog, "ERROR: Device was not in D0 power state within %d ms of playing a stream in screen off.", 
            uiResponseResponsiveSleepPeriodMs);
    }

    return;
}
