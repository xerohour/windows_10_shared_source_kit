//--------------------------------------------------------------------------;
//
//  File: AdapterPowerManagement3.cpp
//
// Copyright (C) Microsoft. All rights reserved.
//
//  History:
//      02/14/2012    CaWerner       Initial implementation.
//
//--------------------------------------------------------------------------;

#include "common.h"
#include <audioclient.h>
#include <evntcons.h>
#include <EtwLogger.h>
#include <itestresource.h>
#include <resourcelist.h>

using namespace WEX::Logging;
using namespace WEX::TestExecution;

#include "SetupApiHelper.h"
#include "DeviceSupport.h"
#include "PortClsEventLogger.h"
#include "AdapterPowerManagementTest.h"
#include "AdapterPowerManagement3.h"

#include <lpe.h>

bool AdapterPowerManagementTestETW::SetupClass()
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

    // Disable all the audio devnodes.
    // Each devnode will be re-enabled when a test is run against that specific devnode.
    if( !VERIFY_SUCCEEDED( DisableAllAudioDevnodes() ) ) { return false; }

    LOG(m_spBasicLog, L"Sleeping 30 seconds, make sure everything is disabled.");
    Sleep(30000);
    return true;
}

bool AdapterPowerManagementTestETW::CleanupClass()
{
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

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

    // Re-Enable all the audio devnodes we previously disabled.
    if( !VERIFY_SUCCEEDED( RestoreAllAudioDevnodes() ) ) { return false; }

    LOG(m_spBasicLog, L"Sleeping 30 seconds, make sure everything is enabled.");
    Sleep(30000);
    return true;
}

bool AdapterPowerManagementTestETW::SetupTest()
{
    LOG(m_spBasicLog, L"In Test Setup...");
    UINT numDevices = Resources::Count();

    for( UINT i = 0; i < numDevices; i++ )
    {
        CComPtr<ITestResource> spTestResource;
        AudioDevnode* pAudioDevnode = nullptr;

        if( !VERIFY_SUCCEEDED( Resources::Item( i, &spTestResource ) ) ) { return false; }
        if( !VERIFY_IS_NOT_NULL( pAudioDevnode = static_cast<AudioDevnode *>(spTestResource.p) ) ) { return false; }

        if( !VERIFY_SUCCEEDED( pAudioDevnode->Enable() ) ) { return false; }
        CComBSTR spDeviceInstanceId;

        if( !VERIFY_SUCCEEDED(spTestResource->GetValue( CComBSTR(TestResourceProperty::c_szId), &spDeviceInstanceId.m_str ) ) ) { return false; }
        bool bEnabled = true;
        if ( !VERIFY_SUCCEEDED(pAudioDevnode->IsEnabled(&bEnabled)))  { return false; }
        if ( !VERIFY_ARE_EQUAL(bEnabled, true))  { return false; }
        LOG( m_spBasicLog, L"Audio Devnode Enbled: %s", spDeviceInstanceId.m_str );
    }

    LOG(m_spBasicLog, L"Sleeping 30 seconds, make sure everything is enabled as expected.");
    Sleep(30000);

    return true;
}

bool AdapterPowerManagementTestETW::CleanupTest()
{
    LOG(m_spBasicLog, L"In Test Cleanup...");
    UINT numDevices = Resources::Count();

    for( UINT i = 0; i < numDevices; i++ )
    {
        CComPtr<ITestResource> spTestResource;
        AudioDevnode* pAudioDevnode = nullptr;

        if( !VERIFY_SUCCEEDED( Resources::Item( i, &spTestResource ) ) ) { return false; }
        if( !VERIFY_IS_NOT_NULL( pAudioDevnode = static_cast<AudioDevnode *>(spTestResource.p) ) ) { return false; }
        if( !VERIFY_SUCCEEDED( pAudioDevnode->Disable() ) ) { return false; }
        CComBSTR spDeviceInstanceId;

        if( !VERIFY_SUCCEEDED(spTestResource->GetValue( CComBSTR(TestResourceProperty::c_szId), &spDeviceInstanceId.m_str ) ) ) { return false; }
        bool bDisabled = true;
        if ( !VERIFY_SUCCEEDED(pAudioDevnode->IsDisabled(&bDisabled))) { return false; }
        if ( !VERIFY_ARE_EQUAL(bDisabled, true))  { return false; }
        LOG( m_spBasicLog, L"Audio Devnode Disabled: %s", spDeviceInstanceId.m_str );
    }

    return true;
}

void AdapterPowerManagementTestETW::PowerChangeState3_S0_D0toD3toD0()
{
    HRESULT hr = S_OK;
    bool isAoAc = false;
    bool onDC = false;
    CComPtr<IAudioClient> spAudioClient;

    // Prerequisite #1: System Must Support AoAc
    if( !VERIFY_SUCCEEDED( DoesSystemSupportAoAc( &isAoAc ) ) ) { return; }
    if (!isAoAc)
    {
        SKIP( m_spBasicLog, L"SKIP: System does not support AOAC. Skipping Screen-off test." );
        return;
    }

    // Prerequisite #2: System Must Be on DC Power
    if( !VERIFY_SUCCEEDED( IsSystemOnDC( &onDC ) ) ) { return; }
    if( !VERIFY_IS_TRUE( onDC, L"System is on DC Power" ) ) { return; }

    // Prerequisite #3: Audio Devnode Must Support D3
    hr = DeviceUnderTestPowerState( PowerDeviceD3 );
    if( E_UNEXPECTED == hr )
    {
        SKIP( m_spBasicLog, L"SKIP: Device did not enter PowerDeviceD3.  Possible codec does not support EPSS." );

        return;
    }

    // Prerequisite #4: Audio Devnode Must be in D0
    if( !VERIFY_SUCCEEDED( DeviceUnderTestIncreasePinCount( &spAudioClient ) ) ) { return; }
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) { return; }

    // start collecting ETW from PortCls
    PortClsEventLogger PCEventLogger( m_spBasicLog );
    if( !VERIFY_SUCCEEDED( PCEventLogger.Initialize( L"PowerChangeState3_S0_D0toD3toD0" ) ) ) { return; }
    if( !VERIFY_SUCCEEDED( PCEventLogger.Start() ) ) { return; }

    // go to D3 by closing the Audio Client (D0->D3)
    spAudioClient.Release();
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD3 ) ) ) { return; }

    // go to D0 by re-opening an Audio Client (D3->D0)
    if( !VERIFY_SUCCEEDED( DeviceUnderTestIncreasePinCount( &spAudioClient ) ) ) { return; }
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) { return; }

    // stop collecting ETW from PortCls
    if( !VERIFY_SUCCEEDED( PCEventLogger.Stop() ) ) { return; }

    // validate the scenario by comparing the events collected against what was expected
    VERIFY_SUCCEEDED( VerifyEvents( &PCEventLogger, Verify_S0_D0toD3toD0, ARRAYSIZE(Verify_S0_D0toD3toD0) ) );
}

void AdapterPowerManagementTestETW::PowerChangeState3_ConnectedStandby_D0toD3toD0()
{
    HRESULT hr = S_OK;
    bool isAoAc = false;
    bool onDC = false;
    CComPtr<IAudioClient> spAudioClient;

    // Prerequisite #1: System Must Support AoAc
    if( !VERIFY_SUCCEEDED( DoesSystemSupportAoAc( &isAoAc ) ) ) { return; }
    if (!isAoAc)
    {
        SKIP( m_spBasicLog, L"SKIP: System does not support AOAC. Skipping Screen-off test." );
        return;
    }

    // Prerequisite #2: System Must Be on DC Power
    if( !VERIFY_SUCCEEDED( IsSystemOnDC( &onDC ) ) ) { return; }
    if( !VERIFY_IS_TRUE( onDC, L"System is on DC Power" ) ) { return; }

    // Prerequisite #3: Audio Devnode Must Support D3
    hr = DeviceUnderTestPowerState( PowerDeviceD3 );
    if( E_UNEXPECTED == hr )
    {
        SKIP( m_spBasicLog, L"SKIP: Device did not enter PowerDeviceD3.  Possible codec does not support EPSS." );

        return;
    }

    // Prerequisite #4: Audio Devnode Must be in D0
    if( !VERIFY_SUCCEEDED( DeviceUnderTestIncreasePinCount( &spAudioClient ) ) ) { return; }
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) { return; }

    // Register for Power Broadcast Events
    if( !VERIFY_SUCCEEDED( RegisterForPowerBroadcasts() ) ) { return; }
    OnExit unregisterPowerBroadcasts( [&]()
    {
        (void) UnregisterForPowerBroadcasts();
    } );

    // enter Connected Standby
    if( !VERIFY_SUCCEEDED( EnterConnectedStandby() ) ) { return; }

    // start collecting ETW from PortCls
    PortClsEventLogger PCEventLogger( m_spBasicLog );
    if( !VERIFY_SUCCEEDED( PCEventLogger.Initialize( L"PowerChangeState3_ConnectedStandby_D0toD3toD0" ) ) ) { return; }
    if( !VERIFY_SUCCEEDED( PCEventLogger.Start() ) ) { return; }

    // go to D3 by closing the Audio Client (D0->D3)
    spAudioClient.Release();
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD3 ) ) ) { return; }

    // go to D0 by re-opening an Audio Client (D3->D0)
    if( !VERIFY_SUCCEEDED( DeviceUnderTestIncreasePinCount( &spAudioClient ) ) ) { return; }
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) { return; }

    // stop collecting ETW from PortCls
    if( !VERIFY_SUCCEEDED( PCEventLogger.Stop() ) ) { return; }

    // exit Connected Standby
    if( !VERIFY_SUCCEEDED( ExitConnectedStandby() ) ) { return; }

    // Unregister for Power Broadcast Events
    if( !VERIFY_SUCCEEDED( UnregisterForPowerBroadcasts() ) ) { return; }

    // validate the scenario by comparing the events collected against what was expected
    VERIFY_SUCCEEDED( VerifyEvents( &PCEventLogger, Verify_ConnectedStandby_D0toD3toD0, ARRAYSIZE(Verify_ConnectedStandby_D0toD3toD0) ) );
}

void AdapterPowerManagementTestETW::PowerChangeState3_EnterExitConnectedStandby_D0()
{
    bool isAoAc = false;
    bool onDC = false;
    CComPtr<IAudioClient> spAudioClient;

    // Prerequisite #1: System Must Support AoAc
    if( !VERIFY_SUCCEEDED( DoesSystemSupportAoAc( &isAoAc ) ) ) { return; }
    if (!isAoAc)
    {
        SKIP( m_spBasicLog, L"SKIP: System does not support AOAC. Skipping Screen-off test." );
        return;
    }

    // Prerequisite #2: System Must Be on DC Power
    if( !VERIFY_SUCCEEDED( IsSystemOnDC( &onDC ) ) ) { return; }
    if( !VERIFY_IS_TRUE( onDC, L"System is on DC Power" ) ) { return; }

    // Prerequisite #3: Audio Devnode Must be in D0
    Sleep( 1000 ); // BUGBUG: waiting a second for the MMDevices to become active (add listening routine in SetupTest)
    if( !VERIFY_SUCCEEDED( DeviceUnderTestIncreasePinCount( &spAudioClient ) ) ) { return; }
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) { return; }

    // Register for Power Broadcast Events
    if( !VERIFY_SUCCEEDED( RegisterForPowerBroadcasts() ) ) { return; }
    OnExit unregisterPowerBroadcasts( [&]()
    {
        (void) UnregisterForPowerBroadcasts();
    } );

    // start collecting ETW from PortCls
    PortClsEventLogger PCEventLogger( m_spBasicLog );
    if( !VERIFY_SUCCEEDED( PCEventLogger.Initialize( L"PowerChangeState3_EnterExitConnectedStandby_D0" ) ) ) { return; }
    if( !VERIFY_SUCCEEDED( PCEventLogger.Start() ) ) { return; }

    // enter Connected Standby
    if( !VERIFY_SUCCEEDED( EnterConnectedStandby() ) ) { return; }

    Sleep( 10000 );

    // exit Connected Standby
    if( !VERIFY_SUCCEEDED( ExitConnectedStandby() ) ) { return; }

    // stop collecting ETW from PortCls
    if( !VERIFY_SUCCEEDED( PCEventLogger.Stop() ) ) { return; }

    // Unregister for Power Broadcast Events
    if( !VERIFY_SUCCEEDED( UnregisterForPowerBroadcasts() ) ) { return; }

    // validate the scenario by comparing the events collected against what was expected
    VERIFY_SUCCEEDED( VerifyEvents( &PCEventLogger, Verify_EnterExitConnectedStandby_D0, ARRAYSIZE(Verify_EnterExitConnectedStandby_D0) ) );
}

void AdapterPowerManagementTestETW::PowerChangeState3_EnterExitConnectedStandby_D3()
{
    HRESULT hr = S_OK;
    bool isAoAc = false;
    bool onDC = false;

    // Prerequisite #1: System Must Support AoAc
    if( !VERIFY_SUCCEEDED( DoesSystemSupportAoAc( &isAoAc ) ) ) { return; }
    if (!isAoAc)
    {
        SKIP( m_spBasicLog, L"SKIP: System does not support AOAC. Skipping Screen-off test." );
        return;
    }

    // Prerequisite #2: System Must Be on DC Power
    if( !VERIFY_SUCCEEDED( IsSystemOnDC( &onDC ) ) ) { return; }
    if( !VERIFY_IS_TRUE( onDC, L"System is on DC Power" ) ) { return; }

    // Prerequisite #3: Audio Devnode Must be in D3
    hr = DeviceUnderTestPowerState( PowerDeviceD3 );
    if( E_UNEXPECTED == hr )
    {
        SKIP( m_spBasicLog, L"SKIP: Device did not enter PowerDeviceD3.  Possible codec does not support EPSS." );

        return;
    }

    // Register for Power Broadcast Events
    if( !VERIFY_SUCCEEDED( RegisterForPowerBroadcasts() ) ) { return; }
    OnExit unregisterPowerBroadcasts( [&]()
    {
        (void) UnregisterForPowerBroadcasts();
    } );

    // start collecting ETW from PortCls
    PortClsEventLogger PCEventLogger( m_spBasicLog );
    if( !VERIFY_SUCCEEDED( PCEventLogger.Initialize( L"PowerChangeState3_EnterExitConnectedStandby_D3" ) ) ) { return; }
    if( !VERIFY_SUCCEEDED( PCEventLogger.Start() ) ) { return; }

    // enter Connected Standby
    if( !VERIFY_SUCCEEDED( EnterConnectedStandby() ) ) { return; }

    Sleep( 10000 );

    // exit Connected Standby
    if( !VERIFY_SUCCEEDED( ExitConnectedStandby() ) ) { return; }

    // stop collecting ETW from PortCls
    if( !VERIFY_SUCCEEDED( PCEventLogger.Stop() ) ) { return; }

    // Unregister for Power Broadcast Events
    if( !VERIFY_SUCCEEDED( UnregisterForPowerBroadcasts() ) ) { return; }

    // validate the scenario by comparing the events collected against what was expected
    VERIFY_SUCCEEDED( VerifyEvents( &PCEventLogger, Verify_EnterExitConnectedStandby_D3, ARRAYSIZE(Verify_EnterExitConnectedStandby_D3) ) );
}


bool AdapterPowerManagementTestMobile::SetupClassMobile()
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

    if( !VERIFY_SUCCEEDED( SetupSimBatt() ) ) { return false; }

    return true;
}

bool AdapterPowerManagementTestMobile::CleanupClassMobile()
{
    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

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

    if( !VERIFY_SUCCEEDED( CleanupSimBatt() ) ) { return false; }

    return true;
}

bool AdapterPowerManagementTestMobile::SetupTestMobile()
{
    return true;
}

bool AdapterPowerManagementTestMobile::CleanupTestMobile()
{
    return true;
}

void ResetAllCalls(CallController *callController)
{
    UINT providerCount = 0;
    UINT supportedProviderId[MAX_PROVIDERIDS] = {0};

    VERIFY_SUCCEEDED(callController->RetrieveProviderId(supportedProviderId, &providerCount));

    for (UINT i = 0; i < providerCount; i++)
    {
        KSTELEPHONY_CALLINFO callInfo = {};

        VERIFY_SUCCEEDED(callController->GetCallInfo(supportedProviderId[i], &callInfo));

        if (callInfo.CallState != TELEPHONY_CALLSTATE_DISABLED)
        {
            VERIFY_SUCCEEDED(callController->DisableCellular(supportedProviderId[i], callInfo.CallType));
        }
    }
}

void AdapterPowerManagementTestMobile::CellularPowerChangeState_D3toD0toD3()
{
    HRESULT hr = S_OK;
    bool isAoAc = false;
    bool onDC = false;
    CComPtr<IAudioClient> spAudioClient;
    CallController callController;
    UINT supportedProviderId[MAX_PROVIDERIDS] = {0};
    UINT providerCount = 0;

    // System Must Support AoAc
    if( !VERIFY_SUCCEEDED( DoesSystemSupportAoAc( &isAoAc ) ) ) { return; }
    if (!isAoAc)
    {
        SKIP( m_spBasicLog, L"SKIP: System does not support AOAC. Skipping Screen-off test." );
        return;
    }

    // System Must Be on DC Power
    if( !VERIFY_SUCCEEDED( IsSystemOnDC( &onDC ) ) ) { return; }
    if( !VERIFY_IS_TRUE( onDC, L"System is on DC Power" ) ) { return; }

    // must have cellular support
    hr = GetCallController( &callController);
    if( FAILED(hr))
    {
        SKIP( m_spBasicLog, L"SKIP: Device under test does not support cellular." );
        return;
    }

    // Must register for Power Broadcast Events for EnterConnectedStandby to work
    if( !VERIFY_SUCCEEDED( RegisterForPowerBroadcasts() ) ) { return; }
    OnExit unregisterPowerBroadcasts( [&]()
    {
        (void) UnregisterForPowerBroadcasts();
    } );

    ResetAllCalls(&callController);

    if( !VERIFY_SUCCEEDED( callController.RetrieveProviderId(supportedProviderId, &providerCount) ) ) { return; }

    // Device must be in standby
    if( !VERIFY_SUCCEEDED( EnterConnectedStandby() ) ) { return; }

    // Audio Devnode Must Support D3
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD3 ) ) ) { goto cleanup; }

    // enable cellular
    if( !VERIFY_SUCCEEDED( callController.EnableCellular(supportedProviderId[0], TELEPHONY_CALLTYPE_CIRCUITSWITCHED) ) ) { goto cleanup; }

    // verify D0
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) { goto cleanup; }

    if( !VERIFY_SUCCEEDED( DelayConservationIdleTime() ) ) { goto cleanup; }

    // verify D0
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) { goto cleanup; }

    // disable cellular
    if( !VERIFY_SUCCEEDED( callController.DisableCellular(supportedProviderId[0], TELEPHONY_CALLTYPE_CIRCUITSWITCHED) ) ) { goto cleanup; }

    // should now go back to D3
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD3 ) ) ) { goto cleanup; }

cleanup:

    VERIFY_SUCCEEDED( ExitConnectedStandby() );
    ResetAllCalls(&callController);

}

void AdapterPowerManagementTestMobile::FmPowerChangeState_D3toD0toD3()
{
    HRESULT hr = S_OK;
    bool isAoAc = false;
    bool onDC = false;
    CComPtr<IAudioClient> spAudioClient;
    FmController FmController;

    // System Must Support AoAc
    if( !VERIFY_SUCCEEDED( DoesSystemSupportAoAc( &isAoAc ) ) ) { return; }
    if (!isAoAc)
    {
        SKIP( m_spBasicLog, L"SKIP: System does not support AOAC. Skipping Screen-off test." );
        return;
    }

    // System Must Be on DC Power
    if( !VERIFY_SUCCEEDED( IsSystemOnDC( &onDC ) ) ) { return; }
    if( !VERIFY_IS_TRUE( onDC, L"System is on DC Power" ) ) { return; }

    // must have fm support
    hr = GetFmController( &FmController);
    if( FAILED(hr))
    {
        SKIP( m_spBasicLog, L"SKIP: Device under test does not support Fm." );
        return;
    }

    // Must register for Power Broadcast Events for EnterConnectedStandby to work
    if( !VERIFY_SUCCEEDED( RegisterForPowerBroadcasts() ) ) { return; }
    OnExit unregisterPowerBroadcasts( [&]()
    {
        (void) UnregisterForPowerBroadcasts();
    } );

    if( !VERIFY_SUCCEEDED( FmController.SetFmState(FALSE) ) ) { return; }

    // Device must be in standby
    if( !VERIFY_SUCCEEDED( EnterConnectedStandby() ) ) { return; }

    // Audio Devnode Must Support D3
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD3 ) ) ) { goto cleanup; }

    // enable Fm
    if( !VERIFY_SUCCEEDED( FmController.SetFmState(TRUE) ) ) { goto cleanup; }

    // verify D0
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) { goto cleanup; }

    if( !VERIFY_SUCCEEDED( DelayConservationIdleTime() ) ) { goto cleanup; }

    // verify D0
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) { goto cleanup; }

    // disable FM
    if( !VERIFY_SUCCEEDED( FmController.SetFmState(FALSE) ) ) { goto cleanup; }

    // should now go back to D3
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD3 ) ) ) { goto cleanup; }

cleanup:
    VERIFY_SUCCEEDED( ExitConnectedStandby() );
    VERIFY_SUCCEEDED( FmController.SetFmState(FALSE) );

}

void AdapterPowerManagementTestMobile::Fm_Cellular_Interaction_PowerChangeState_D3toD0toD3()
{
    HRESULT hr = S_OK;
    bool isAoAc = false;
    bool onDC = false;
    CComPtr<IAudioClient> spAudioClient;
    CallController callController;
    FmController FmController;
    UINT supportedProviderId[MAX_PROVIDERIDS] = {0};
    UINT providerCount = 0;

    // System Must Support AoAc
    if( !VERIFY_SUCCEEDED( DoesSystemSupportAoAc( &isAoAc ) ) ) { return; }
    if (!isAoAc)
    {
        SKIP( m_spBasicLog, L"SKIP: System does not support AOAC. Skipping Screen-off test." );
        return;
    }

    // System Must Be on DC Power
    if( !VERIFY_SUCCEEDED( IsSystemOnDC( &onDC ) ) ) { return; }
    if( !VERIFY_IS_TRUE( onDC, L"System is on DC Power" ) ) { return; }

    // must have cellular support
    hr = GetCallController( &callController);
    if( FAILED(hr))
    {
        SKIP( m_spBasicLog, L"SKIP: Device under test does not support cellular." );
        return;
    }

    // must also have fm support
    hr = GetFmController( &FmController);
    if( FAILED(hr))
    {
        SKIP( m_spBasicLog, L"SKIP: Device under test does not support Fm." );
        return;
    }

    if( !VERIFY_SUCCEEDED( callController.RetrieveProviderId(supportedProviderId, &providerCount) ) ) { return; }

    // Must register for Power Broadcast Events for EnterConnectedStandby to work
    if( !VERIFY_SUCCEEDED( RegisterForPowerBroadcasts() ) ) { return; }
    OnExit unregisterPowerBroadcasts( [&]()
    {
        (void) UnregisterForPowerBroadcasts();
    } );

    ResetAllCalls(&callController);

    // Device must be in standby
    if( !VERIFY_SUCCEEDED( EnterConnectedStandby() ) ) { return; }

    // Audio Devnode Must Support D3
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD3 ) ) ) { goto cleanup; }

    // enable cellular
    if( !VERIFY_SUCCEEDED( callController.EnableCellular(supportedProviderId[0], TELEPHONY_CALLTYPE_CIRCUITSWITCHED) ) ) { goto cleanup; }

    // verify D0
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) { goto cleanup; }

    // enable Fm
    if( !VERIFY_SUCCEEDED( FmController.SetFmState(TRUE) ) ) { goto cleanup; }

    // verify still D0
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) { goto cleanup; }

    if( !VERIFY_SUCCEEDED( DelayConservationIdleTime() ) ) { goto cleanup; }

    // verify D0
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) { goto cleanup; }

    // disable cellular
    if( !VERIFY_SUCCEEDED( callController.DisableCellular(supportedProviderId[0], TELEPHONY_CALLTYPE_CIRCUITSWITCHED) ) ) { goto cleanup; }

    if( !VERIFY_SUCCEEDED( DelayConservationIdleTime() ) ) { goto cleanup; }

    // we should remain in D0 after the idle period expires
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD0 ) ) ) { goto cleanup; }

    // disable FM
    if( !VERIFY_SUCCEEDED( FmController.SetFmState(FALSE) ) ) { goto cleanup; }

    if( !VERIFY_SUCCEEDED( DelayConservationIdleTime() ) ) { goto cleanup; }

    // should now go back to D3 once the idle period expires
    if( !VERIFY_SUCCEEDED( DeviceUnderTestPowerState( PowerDeviceD3 ) ) ) { goto cleanup; }

cleanup:

    VERIFY_SUCCEEDED( ExitConnectedStandby() );
    ResetAllCalls(&callController);
    VERIFY_SUCCEEDED( FmController.SetFmState(FALSE) );
}

