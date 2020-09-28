//--------------------------------------------------------------------------;
//
//  File: AdapterPowerManagementTest.cpp
//
// Copyright (C) Microsoft. All rights reserved.
//
//  History:
//      02/14/2012    CaWerner       Initial implementation.
//
//--------------------------------------------------------------------------;

#include "common.h"
#include <itestresource.h>
#include <resourcelist.h>
#include <audioclient.h>
#include <evntcons.h>
#include <ETWLogger.h>

#include "mmsystem.h"
#include "DevTrans.h"

HRESULT g_hrCoInit = E_FAIL;

BEGIN_MODULE()
    MODULE_PROPERTY(L"TestResourceDependent", L"true")
END_MODULE()

using namespace WEX::Logging;
using namespace WEX::TestExecution;
using namespace DevTrans;

#include <devobj.h>
#include "SetupApiHelper.h"
#include "DeviceSupport.h"
#include "PortClsEventLogger.h"
#include "AdapterPowerManagementTest.h"

#include <initguid.h>
#include <lpe.h>

#include <SleepResume.h>

MODULE_SETUP(AdapterPowerManagementSetup);

//Release stuff acquired in WDMAudioSetup
MODULE_CLEANUP(AdapterPowerManagementCleanup);

bool AdapterPowerManagementSetup()
{
    g_hrCoInit = CoInitializeEx(NULL, COINIT_MULTITHREADED);
    return SUCCEEDED(g_hrCoInit);
}

bool AdapterPowerManagementCleanup()
{
    if (SUCCEEDED(g_hrCoInit))
    {
        CoUninitialize();
    }

    return true;
}

HRESULT AdapterPowerManagementTest::VerifyEvents( PortClsEventLogger* pPortClsEventLogger, const PowerPolicyEvent* eventVerifyArray,
    size_t numExpectedEvents )
{
    HRESULT hr = S_OK;
    UINT numCollectedEvents = 0;
    ULONGLONG deviceObjectPointer = 0;
    bool isValid = true;

    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    if( !VERIFY_SUCCEEDED( hr = pPortClsEventLogger->GetEventCount( &numCollectedEvents ) ) ) { return hr; }

    LOG( m_spBasicLog, L"Collected %u Events", numCollectedEvents );

    for( UINT x = 0; x < numCollectedEvents; x++ )
    {
        PowerPolicyEvent powerPolicyEvent;

        if( !VERIFY_SUCCEEDED( hr = pPortClsEventLogger->GetPowerPolicyEvent( x, &powerPolicyEvent ) ) ) { return hr; }
        LogEvent( &powerPolicyEvent );
    }

    if( !VERIFY_ARE_EQUAL( numExpectedEvents, numCollectedEvents ) )
    {
        LOG( m_spBasicLog, L"Expected %Iu Events.  Actual %u", numExpectedEvents, numCollectedEvents );
        for( UINT i = 0; i < numCollectedEvents; i++ )
        {
            PowerPolicyEvent powerPolicyEvent;

            if( !VERIFY_SUCCEEDED( hr = pPortClsEventLogger->GetPowerPolicyEvent( i, &powerPolicyEvent ) ) ) { return hr; }
            VERIFY_SUCCEEDED( LogEvent( &powerPolicyEvent ) );
        }

        return E_FAIL;
    }

    for( UINT i = 0; i < numExpectedEvents; i++ )
    {
        PowerPolicyEvent powerPolicyEvent;
        bool localValid = true;

        if( !VERIFY_SUCCEEDED( hr = pPortClsEventLogger->GetPowerPolicyEvent( i, &powerPolicyEvent ) ) ) { return hr; }

        if( i != 0 )
        {
            if( !VERIFY_ARE_EQUAL( deviceObjectPointer, powerPolicyEvent.DeviceObjectPtr ) ) { localValid = false; }
        }
        else
        {
            deviceObjectPointer = powerPolicyEvent.DeviceObjectPtr;
        }

        if( !VERIFY_ARE_EQUAL( eventVerifyArray[i].EventType, powerPolicyEvent.EventType ) ) { isValid = false; }
        if( !VERIFY_ARE_EQUAL( eventVerifyArray[i].Value, powerPolicyEvent.Value ) ) { isValid = false; }
        if( !VERIFY_ARE_EQUAL( eventVerifyArray[i].Complete, powerPolicyEvent.Complete ) ) { isValid = false; }

        if( !localValid )
        {
            LOG( m_spBasicLog, L"Event #%u - Expected:", i );
            VERIFY_SUCCEEDED( LogEvent( &eventVerifyArray[i] ) );
            LOG( m_spBasicLog, L"Event #%u - Actual:", i );
            VERIFY_SUCCEEDED( LogEvent( &powerPolicyEvent ) );

            isValid = false;
        }
    }

    return (isValid ? S_OK : E_FAIL);
}

HRESULT AdapterPowerManagementTest::DisableAllAudioDevnodes()
{
    HRESULT hr = S_OK;
    UINT numDevices = g_pResourceList->Count();

    for( UINT i = 0; i < numDevices; i++ )
    {
        CComPtr<ITestResource> spTestResource;
        AudioDevnode* pAudioDevnode = nullptr;
        bool isEnabled = true;

        if( !VERIFY_SUCCEEDED( hr = g_pResourceList->Item( i, &spTestResource ) ) ) { return hr; }
        if( !VERIFY_IS_NOT_NULL( pAudioDevnode = static_cast<AudioDevnode *>(spTestResource.p) ) ) { return E_POINTER; }

        if( !VERIFY_SUCCEEDED( hr = pAudioDevnode->IsEnabled( &isEnabled ) ) ) { return hr; }

        if( isEnabled )
        {
            CComBSTR spDeviceInstanceId;

            if( !VERIFY_SUCCEEDED(
                hr = spTestResource->GetValue( CComBSTR(TestResourceProperty::c_szId), &spDeviceInstanceId.m_str ) ) )
            { return hr; }

            if( !VERIFY_SUCCEEDED( hr = pAudioDevnode->Disable() ) ) { return hr; }

            LOG( m_spBasicLog, L"Audio Devnode Disabled: %s", spDeviceInstanceId.m_str );
        }
    }

    return S_OK;
}

HRESULT AdapterPowerManagementTest::RestoreAllAudioDevnodes()
{
    HRESULT hr = S_OK;
    UINT numDevices = g_pResourceList->Count();

    for( UINT i = 0; i < numDevices; i++ )
    {
        CComPtr<ITestResource> spTestResource;
        AudioDevnode* pAudioDevnode = nullptr;
        CComBSTR spDeviceInstanceId;

        if( !VERIFY_SUCCEEDED( hr = g_pResourceList->Item( i, &spTestResource ) ) ) { return hr; }
        if( !VERIFY_IS_NOT_NULL( pAudioDevnode = static_cast<AudioDevnode *>(spTestResource.p) ) ) { return E_POINTER; }

        if( !VERIFY_SUCCEEDED(
            hr = spTestResource->GetValue( CComBSTR(TestResourceProperty::c_szId), &spDeviceInstanceId.m_str ) ) )
        { return hr; }

        if( !VERIFY_SUCCEEDED( hr = pAudioDevnode->RestoreDevnodeState() ) ) { return hr; }

        LOG( m_spBasicLog, L"Audio Devnode Restored: %s", spDeviceInstanceId.m_str );
    }

    return hr;
}

HRESULT AdapterPowerManagementTest::DeviceUnderTestIncreasePinCount( IAudioClient** pAudioClient, bool bIsCSTest)
{
    HRESULT hr = S_OK;
    CComPtr<IMMDevice> spDevnodeAsMMDevice;
    CComPtr<IMMDevice> spMMDevice;
    CComPtr<IAudioClient> spAudioClient;
    CComPtr<IMMDeviceCollection> spDevCollection;
    CComHeapPtr<WAVEFORMATEX> spWfx;
    REFERENCE_TIME tenMillisecondsIn100NS = 100000;
    CComPtr<ITestResource> spTestResource;
    size_t index = 0;
    CComBSTR spDeviceInstanceId;
    
    if( !VERIFY_ARE_EQUAL( Resources::Count(), index + 1 ) ) { return E_UNEXPECTED; }
    if( !VERIFY_SUCCEEDED( hr = Resources::Item( index, &spTestResource ) ) ) { return hr; }
    
    if( !VERIFY_SUCCEEDED(
        hr = spTestResource->GetValue( CComBSTR(TestResourceProperty::c_szId), &spDeviceInstanceId.m_str ) ) )
    { return hr; }

    if( !VERIFY_SUCCEEDED( hr = InstanceIdToDevnode(spDeviceInstanceId.m_str, &spDevnodeAsMMDevice) ) ) { return hr; } 

    if( !VERIFY_SUCCEEDED( hr = GetDevnodeEndpoints(spDevnodeAsMMDevice, DEVICE_STATE_ACTIVE, &spDevCollection) ) ) { return hr; } 

    UINT cDevices = 0;
    if( !VERIFY_SUCCEEDED( hr = spDevCollection->GetCount( &cDevices) ) ) { return hr; }
    if ( 0 == cDevices && bIsCSTest )
    {
        CComPtr<IMMDeviceCollection> spUnpluggedDevCollection;
        UINT cUnpluggedDevices = 0;
        CComPtr<IMMDevice> spCurrUnpluggedMMDevice;
        bool bAllUnpluggedDigital = true;

        if( !VERIFY_SUCCEEDED( hr = GetDevnodeEndpoints( spDevnodeAsMMDevice, DEVICE_STATE_UNPLUGGED, &spUnpluggedDevCollection) ) ) { return hr; } 
        if( !VERIFY_SUCCEEDED( hr = spUnpluggedDevCollection->GetCount( &cUnpluggedDevices) ) ) { return hr; }
       
        //if all unplugged endpoints are digital, skip. Otherwise, fail and prompt to plug in non digital 
        for ( UINT i = 0; i < cUnpluggedDevices; i++ )
        {
            EndpointFormFactor eFormFactor = EndpointFormFactor::UnknownFormFactor;
            if ( !VERIFY_SUCCEEDED(hr = spUnpluggedDevCollection->Item(0, &spCurrUnpluggedMMDevice) ) ) { return hr; }
            if ( !VERIFY_SUCCEEDED(hr = GetEndpointFormFactor(&eFormFactor, spCurrUnpluggedMMDevice) ) ) { return hr; }
            bAllUnpluggedDigital = bAllUnpluggedDigital && ( EndpointFormFactor::DigitalAudioDisplayDevice == eFormFactor );
        }

        if ( bAllUnpluggedDigital )
        {
            SKIP(m_spBasicLog, L"All unplugged endpoints for %s are digital.\nSkipping CS tests due to the display turning off", 
                spDeviceInstanceId.m_str );
            return S_FALSE;
        }
        else
        {
            ERR(m_spBasicLog, L"There were 0 active endpoints on the system for device %s, and not all were digital.\nPlease plug in an endpoint and try again.",
                spDeviceInstanceId.m_str );
            return E_FAIL;
        }
    }
    else if ( 0 == cDevices )
    {   ERR( m_spBasicLog, L"There were 0 active endpoints for device: %s", spDeviceInstanceId.m_str );
        return E_UNEXPECTED;
    }
    

    // we really don't care which device we stream to on the devnode, so just take the 1st one. 
    if( !VERIFY_SUCCEEDED( hr = spDevCollection->Item(0, &spMMDevice) ) ) { return hr; }

    if( !VERIFY_SUCCEEDED( hr = spMMDevice->Activate( __uuidof(IAudioClient), CLSCTX_ALL, NULL, (void **)&spAudioClient ) ) ) { return hr; }

    if( !VERIFY_SUCCEEDED( hr = spAudioClient->GetMixFormat( &spWfx ) ) ) { return hr; }

    if( !VERIFY_SUCCEEDED( hr = spAudioClient->Initialize( AUDCLNT_SHAREMODE_SHARED,
                                                           AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                                                           tenMillisecondsIn100NS,
                                                           0,
                                                           spWfx,
                                                           NULL ) ) ) { return hr; }

    *pAudioClient = spAudioClient.Detach();

    return S_OK;
}

HRESULT AdapterPowerManagementTest::DelayConservationIdleTime()
{
    HRESULT hr = S_OK;
    CComPtr<ITestResource> spTestResource;
    AudioDevnode* pAudioDevnode = nullptr;
    size_t index = 0;
    ULONG secsConservationIdleTime = 0;

    if( !VERIFY_ARE_EQUAL( Resources::Count(), index + 1 ) ) { return E_UNEXPECTED; }
    if( !VERIFY_SUCCEEDED( hr = Resources::Item( index, &spTestResource ) ) ) { return hr; }
    if( !VERIFY_IS_NOT_NULL( pAudioDevnode = static_cast<AudioDevnode *>(spTestResource.p) ) ) { return E_POINTER; }

    if( !VERIFY_SUCCEEDED( hr = pAudioDevnode->GetConservationIdleTime( &secsConservationIdleTime ) ) ) { return E_POINTER; }

    LOG( m_spBasicLog, "Waiting %u ms.", (secsConservationIdleTime * 1000) + 5000 );

    Sleep( (secsConservationIdleTime * 1000) + 5000 );

    return S_OK;
}

HRESULT AdapterPowerManagementTest::DeviceUnderTestPowerState( DEVICE_POWER_STATE devicePowerState )
{
    HRESULT hr = S_OK;
    CComPtr<ITestResource> spTestResource;
    AudioDevnode* pAudioDevnode = nullptr;
    DEVICE_POWER_STATE currentDState = PowerDeviceUnspecified;
    size_t index = 0;
    ULONG secsConservationIdleTime = 0;

    if( !VERIFY_ARE_EQUAL( Resources::Count(), index + 1 ) ) { return E_UNEXPECTED; }
    if( !VERIFY_SUCCEEDED( hr = Resources::Item( index, &spTestResource ) ) ) { return hr; }
    if( !VERIFY_IS_NOT_NULL( pAudioDevnode = static_cast<AudioDevnode *>(spTestResource.p) ) ) { return E_POINTER; }

    // Fetch the current device state
    if( !VERIFY_SUCCEEDED( hr = pAudioDevnode->GetDevicePowerState( &currentDState ) ) ) { return hr; }

    LOG( m_spBasicLog, L"Device Power State: %s", GetDevicePowerStateName(currentDState) );

    if( devicePowerState != currentDState )
    {
        switch( devicePowerState )
        {
            case PowerDeviceD0:
                LOG( m_spBasicLog, L"ERROR: Device is in %s, but we require PowerDeviceD0.  Test Case should have opened a pin to force the PowerDeviceD0 state.",
                    GetDevicePowerStateName(currentDState) );
                hr = E_UNEXPECTED;

                break;

            case PowerDeviceD3:
                // wait the idle time
                DelayConservationIdleTime();

                if( !VERIFY_SUCCEEDED( hr = pAudioDevnode->GetDevicePowerState( &currentDState ) ) ) { return hr; }
                if( devicePowerState != currentDState )
                {
                    LOG( m_spBasicLog, L"ERROR: Waited %u ms for the device to transition to PowerDeviceD3.  But, the device is in %s.",
                        (secsConservationIdleTime * 1000) + 5000, GetDevicePowerStateName(currentDState) );

                    hr = E_UNEXPECTED;
                }
                else
                {
                    LOG( m_spBasicLog, L"Device Power State: %s", GetDevicePowerStateName(currentDState) );
                }

                break;

            default:
                LOG( m_spBasicLog, L"ERROR: Unexpected Device Power State: %s", GetDevicePowerStateName(devicePowerState) );
                hr = E_UNEXPECTED;
        }
    }

    return hr;
}

HRESULT AdapterPowerManagementTest::GetCallController( _In_ CallController *pCallController )
{
    HRESULT hr = S_OK;
    CComPtr<IMMDevice> spDevnodeAsMMDevice;
    CComPtr<ITestResource> spTestResource;
    CComBSTR spDeviceInstanceId;
    CComPtr<IMMDeviceCollection> spDevCollection;
    UINT cDevices = 0;
    size_t index = 0;
    BOOL bEndpointFound = FALSE;

    if (NULL == pCallController)
    {
        return E_POINTER;
    }

    // get the device collection from the requested devnode resource list
    if( !VERIFY_ARE_EQUAL( Resources::Count(), index + 1 ) ) { return E_UNEXPECTED; }
    if( !VERIFY_SUCCEEDED( hr = Resources::Item( index, &spTestResource ) ) ) { return hr; }
    if( !VERIFY_SUCCEEDED( hr = spTestResource->GetValue( CComBSTR(TestResourceProperty::c_szId), &spDeviceInstanceId.m_str ) ) ){ return hr; }
    if( !VERIFY_SUCCEEDED( hr = InstanceIdToDevnode(spDeviceInstanceId.m_str, &spDevnodeAsMMDevice) ) ) { return hr; } 

    // Now that we have more "classic" mmdevices, look for active or unplugged endpoints and try to activate call controller on it
    if( !VERIFY_SUCCEEDED( hr = GetDevnodeEndpoints(spDevnodeAsMMDevice, DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED, &spDevCollection) ) ) { return hr; } 
    if( !VERIFY_SUCCEEDED( hr = spDevCollection->GetCount( &cDevices) ) ) { return hr; }

    // we now have a collection of all unplugged endpoints, look for any that are for cellular
    for (UINT j = 0; j < cDevices; j++)
    {
        CComPtr<IMMDevice> spCurrEndpoint;

        if ( !VERIFY_SUCCEEDED( hr = spDevCollection->Item(j, &spCurrEndpoint) ) ) { return hr; }

        hr = pCallController->Initialize(spCurrEndpoint);

        if ( SUCCEEDED(hr) )
        {
            // need to continue through for all of the endpoints, but at this point we're OK saying
            // we succeeded
            bEndpointFound = TRUE;
        }
    }

    // pass if we found any cellular endpoints, fail if we didn't
    return bEndpointFound?S_OK:E_FAIL;
}

HRESULT AdapterPowerManagementTest::GetFmController( _In_ FmController *pFmController )
{
    HRESULT hr = S_OK;
    CComPtr<IMMDevice> spDevnodeAsMMDevice;
    CComPtr<ITestResource> spTestResource;
    CComBSTR spDeviceInstanceId;
    CComPtr<IMMDeviceCollection> spDevCollection;
    UINT cDevices = 0;
    size_t index = 0;

    if (NULL == pFmController)
    {
        return E_POINTER;
    }

    // get the device collection from the requested devnode resource list
    if( !VERIFY_ARE_EQUAL( Resources::Count(), index + 1 ) ) { return E_UNEXPECTED; }
    if( !VERIFY_SUCCEEDED( hr = Resources::Item( index, &spTestResource ) ) ) { return hr; }
    if( !VERIFY_SUCCEEDED( hr = spTestResource->GetValue( CComBSTR(TestResourceProperty::c_szId), &spDeviceInstanceId.m_str ) ) ){ return hr; }
    if( !VERIFY_SUCCEEDED( hr = InstanceIdToDevnode(spDeviceInstanceId.m_str, &spDevnodeAsMMDevice) ) ) { return hr; } 

    // Now that we have more "classic" mmdevices, look for active or unplugged endpoints and try to activate call controller on it
    if( !VERIFY_SUCCEEDED( hr = GetDevnodeEndpoints(spDevnodeAsMMDevice, DEVICE_STATE_ACTIVE | DEVICE_STATE_UNPLUGGED, &spDevCollection) ) ) { return hr; } 
    if( !VERIFY_SUCCEEDED( hr = spDevCollection->GetCount( &cDevices) ) ) { return hr; }

    // we now have a collection of all unplugged endpoints, look for any that are for cellular
    for (UINT j = 0; j < cDevices; j++)
    {
        CComPtr<IMMDevice> spCurrEndpoint;

        if ( !VERIFY_SUCCEEDED( hr = spDevCollection->Item(j, &spCurrEndpoint) ) ) { return hr; }

        hr = pFmController->Initialize(spCurrEndpoint);

        // once we successfully initialze the FM controller, we can return.
        if ( SUCCEEDED(hr) )
        {
            return S_OK;
        }
    }

    return E_FAIL;
}


HRESULT AdapterPowerManagementTest::DoesSystemSupportAoAc( bool* isAoAc )
{
    LONG ntStatus = 0;
    SYSTEM_POWER_CAPABILITIES systemPowerCapabilities = {0};

    *isAoAc = false;

    if( !VERIFY_WIN32_SUCCEEDED( ntStatus = CallNtPowerInformation(
                                                SystemPowerCapabilities,
                                                NULL,
                                                0,
                                                &systemPowerCapabilities,
                                                sizeof(SYSTEM_POWER_CAPABILITIES) ) ) )
    {
        LOG( m_spBasicLog, L"CallNtPowerInformation Failed with NTSTATUS: %u", ntStatus );

        return HRESULT_FROM_NT( ntStatus );
    }

    *isAoAc = (systemPowerCapabilities.AoAc != FALSE);

    return S_OK;
}

HRESULT AdapterPowerManagementTest::IsSystemOnDC( bool* onDC )
{
    SYSTEM_POWER_STATUS systemPowerStatus = {0};

    *onDC = false;

    if( !VERIFY_IS_TRUE( FALSE != GetSystemPowerStatus( &systemPowerStatus ) ) )
    {
        DWORD error = GetLastError();
        LOG( m_spBasicLog, L"GetSystemPowerStatus Failed with error: %u", error );

        return HRESULT_FROM_WIN32( error );
    }

    *onDC = (systemPowerStatus.ACLineStatus == AC_LINE_OFFLINE);

    return S_OK;
}

HRESULT AdapterPowerManagementTest::EnterConnectedStandby()
{
    LONG ntStatus = 0;
    DWORD waitResult = WAIT_FAILED;

    (void) ResetEvent( m_hScreenOff );
    (void) ResetEvent( m_hLowPowerEpochEnter );

    // Step 1. Tell the system to go to Connected Standby
    ntStatus = NtPowerInformation( ScreenOff, NULL, 0, NULL, 0 );
    if( !NT_SUCCESS(ntStatus) )
    {
        LOG( m_spBasicLog, L"ERROR: NtPowerInformation Failed with NTSTATUS: %u", ntStatus );

        return HRESULT_FROM_NT(ntStatus);
    }

    // Step 2. Wait for the Monitor Off event
    waitResult = WaitForSingleObject( m_hScreenOff, 60000 );
    if( WAIT_TIMEOUT == waitResult )
    {
        LOG( m_spBasicLog, L"ERROR: System failed to turn off monitor within sixty seconds" );

        return E_FAIL;
    }
    else if( WAIT_OBJECT_0 != waitResult )
    {
        DWORD error = GetLastError();
        LOG( m_spBasicLog, L"ERROR: Error occured while waiting for Monitor Off Power Event.  (error=%u)", error );

        return HRESULT_FROM_WIN32( error );
    }

    // Step 3. Wait for the Low Power Epoch event
    waitResult = WaitForSingleObject( m_hLowPowerEpochEnter, 120000 );
    if( WAIT_TIMEOUT == waitResult )
    {
        LOG( m_spBasicLog, L"ERROR: System failed to enter Low Power Epoch within two minutes" );

        return E_FAIL;
    }
    else if( WAIT_OBJECT_0 != waitResult )
    {
        DWORD error = GetLastError();
        LOG( m_spBasicLog, L"ERROR: Error occured while waiting for Low Power Epoch Power Event.  (error=%u)", error );

        return HRESULT_FROM_WIN32( error );
    }

    return S_OK;
}

HRESULT AdapterPowerManagementTest::ExitConnectedStandby()
{
    DWORD waitResult = WAIT_FAILED;

    (void) ResetEvent( m_hLowPowerEpochExit );
    (void) ResetEvent( m_hScreenOn );

    // Step 1. Tell the system to exit Connected Standby
    if (!SendKeyInputToExitConnectedStandby())
    {
        LOG( m_spBasicLog, L"FAIL: System has no way to exit Connected Standby" );
        return E_FAIL;
    }

    // Step 2. Wait for the Low Power Epoch event
    waitResult = WaitForSingleObject( m_hLowPowerEpochExit, 120000 );
    if( WAIT_OBJECT_0 != waitResult )
    {
        DWORD error = GetLastError();
        LOG( m_spBasicLog, L"ERROR: Error occured while waiting for Low Power Epoch Power Event.  (result=%u, error=%u)",
            waitResult, error );

        return HRESULT_FROM_WIN32( error );
    }

    // Step 3. Wait for the Monitor On event
    waitResult = WaitForSingleObject( m_hScreenOn, 60000 );
    if( WAIT_OBJECT_0 != waitResult )
    {
        DWORD error = GetLastError();
        LOG( m_spBasicLog, L"ERROR: Error occured while waiting for Monitor On Power Event.  (error=%u)", error );

        return HRESULT_FROM_WIN32( error );
    }

    return S_OK;
}

LPCWSTR AdapterPowerManagementTest::GetDevicePowerStateName( DEVICE_POWER_STATE devicePowerState )
{
    switch( devicePowerState )
    {
        case PowerDeviceD0:
            return L"PowerDeviceD0";

        case PowerDeviceD1:
            return L"PowerDeviceD1";

        case PowerDeviceD2:
            return L"PowerDeviceD2";

        case PowerDeviceD3:
            return L"PowerDeviceD3";

        case PowerDeviceUnspecified:
            return L"PowerDeviceUnspecified";

        case PowerDeviceMaximum:
            return L"PowerDeviceMaximum";

        default:
            return L"Unknown Power Device State";
    }
}

LPCWSTR AdapterPowerManagementTest::GetPowerGuidName( GUID powerGuid )
{
    if( IsEqualGUID( GUID_MONITOR_POWER_ON, powerGuid ) )
    {
        return L"GUID_MONITOR_POWER_ON";
    }

    if( IsEqualGUID( GUID_LOW_POWER_EPOCH, powerGuid ) )
    {
        return L"GUID_LOW_POWER_EPOCH";
    }

    return L"Unknown Power Registration GUID";
}

LPCWSTR AdapterPowerManagementTest::GetEventTypeString( const PowerPolicyEvent* pPowerPolicyEvent )
{
    switch( pPowerPolicyEvent->EventType )
    {
        case PC_ETYPE_PWR_POLICY_LPE_NOTIFICATION:
            return L"Low Power Epoch Notification";

        case PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE:
            return L"Exit Latency Change";

        default:
            return L"Unknown Event Type";
    }
}

LPCWSTR AdapterPowerManagementTest::GetEventValueString( const PowerPolicyEvent* pPowerPolicyEvent )
{
    if( pPowerPolicyEvent->EventType == PC_ETYPE_PWR_POLICY_LPE_NOTIFICATION )
    {
        switch( pPowerPolicyEvent->Value )
        {
            case EXITING_CS:
                return L"Exiting CS";

            case ENTERING_CS:
                return L"Entering CS";

            default:
                return L"Unknown LPE Value";
        }
    }
    else if( pPowerPolicyEvent->EventType == PC_ETYPE_PWR_POLICY_D3_EXIT_LATENCY_CHANGE )
    {
        switch( pPowerPolicyEvent->Value )
        {
            case PcExitLatencyInstant:
                return L"PcExitLatencyInstant";

            case PcExitLatencyFast:
                return L"PcExitLatencyFast";

            case PcExitLatencyResponsive:
                return L"PcExitLatencyResponsive";

            default:
                return L"Unknown Exit Latency Value";
        }
    }

    return L"Unknown Event Type";
}

LPCWSTR AdapterPowerManagementTest::GetEventCompleteString( const PowerPolicyEvent* pPowerPolicyEvent )
{
    switch( pPowerPolicyEvent->Complete )
    {
        case FALSE:
            return L"Beginning";

        case TRUE:
            return L"Completed";

        default:
            return L"Unknown Complete Value";
    }
}

HRESULT AdapterPowerManagementTest::LogEvent( const PowerPolicyEvent* pPowerPolicyEvent )
{
    LOG( m_spBasicLog, L"[%s] [%I64u] [%s] [%s]",
        GetEventTypeString(pPowerPolicyEvent), pPowerPolicyEvent->DeviceObjectPtr,
        GetEventValueString(pPowerPolicyEvent), GetEventCompleteString(pPowerPolicyEvent) );

    return S_OK;
}

HRESULT AdapterPowerManagementTest::RegisterForPowerBroadcasts()
{
    DWORD dwResult = ERROR_SUCCESS;
    DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS subscribeParams = { NULL, NULL };

    subscribeParams.Callback = (PDEVICE_NOTIFY_CALLBACK_ROUTINE)PowerNotificationCallback;
    subscribeParams.Context = this;

    for( size_t i = 0; i < ARRAYSIZE(m_PowerNotifyStruct); i++ )
    {
        dwResult = PowerSettingRegisterNotification( m_PowerNotifyStruct[i].pGUID, DEVICE_NOTIFY_CALLBACK, &subscribeParams,
            &(m_PowerNotifyStruct[i].hPowerNotify) );
        if( ERROR_SUCCESS != dwResult )
        {
            LOG( m_spBasicLog, L"ERROR: Failed to Register for %s Power Setting Notifications.  (error=%u)",
                GetPowerGuidName( *(m_PowerNotifyStruct[i].pGUID) ), dwResult );

            return E_UNEXPECTED;
        }
    }

    return S_OK;
}

HRESULT AdapterPowerManagementTest::UnregisterForPowerBroadcasts()
{
    DWORD dwResult = ERROR_SUCCESS;

    for( size_t i = 0; i < ARRAYSIZE(m_PowerNotifyStruct); i++ )
    {
        if( NULL != m_PowerNotifyStruct[i].hPowerNotify )
        {
            dwResult = PowerSettingUnregisterNotification( m_PowerNotifyStruct[i].hPowerNotify );
            if( ERROR_SUCCESS != dwResult )
            {
                LOG( m_spBasicLog, L"ERROR: Failed to Unregister for %s Power Setting Notifications.  (error=%u)",
                    GetPowerGuidName( *(m_PowerNotifyStruct[i].pGUID) ), dwResult );

                return E_UNEXPECTED;
            }

            m_PowerNotifyStruct[i].hPowerNotify = NULL;
        }
    }

    return S_OK;
}

ULONG AdapterPowerManagementTest::PowerNotificationCallback( _In_opt_ PVOID Context, _In_ ULONG Type, _In_ PVOID Setting )
{
    AdapterPowerManagementTest* pThis = (AdapterPowerManagementTest *)Context;

    if( nullptr == pThis )
    {
        return ERROR_INVALID_HANDLE;
    }

    return pThis->DoPowerNotificationCallback( Type, Setting );
}

ULONG AdapterPowerManagementTest::DoPowerNotificationCallback( _In_ ULONG Type, _In_ PVOID Setting )
{
    PPOWERBROADCAST_SETTING pPowerBroadcastSetting = (PPOWERBROADCAST_SETTING)Setting;
    ULONG ulLowPowerEpoch = (ULONG)-1;
    ULONG ulMonitorPower = (ULONG)-1;

    if( PBT_POWERSETTINGCHANGE != Type )
    {
        return ERROR_SUCCESS;
    }

    if( IsEqualGUID( pPowerBroadcastSetting->PowerSetting, GUID_MONITOR_POWER_ON ) )
    {
        if( pPowerBroadcastSetting->DataLength != sizeof(ULONG) )
        {
            // error, expect the data to be of size ULONG
            LOG( m_spBasicLog, L"ERROR: Monitor Power On Notification has an invalid data length.  Expected: ULONG (%Iu), Actual: %u.",
                sizeof(ULONG), pPowerBroadcastSetting->DataLength );

            return ERROR_INVALID_PARAMETER;
        }
        else if( pPowerBroadcastSetting->Data == nullptr )
        {
            // error, expect the data variable to be a valid pointer to a ULONG
            LOG( m_spBasicLog, "ERROR: Monitor Power On Notification has an invalid data pointer.  Data: 0x%p",
                pPowerBroadcastSetting->Data );

            return ERROR_INVALID_PARAMETER;
        }

        ulMonitorPower = *((ULONG *)(pPowerBroadcastSetting->Data));

        if( ulMonitorPower == 0 )
        {
            LOG( m_spBasicLog, L"Power Notification: Monitor Power On -- OFF." );

            if( NULL != m_hScreenOff )
            {
                SetEvent( m_hScreenOff );
            }
        }
        else
        {
            LOG( m_spBasicLog, L"Power Notification: Monitor Power On -- ON." );

            if( NULL != m_hScreenOn )
            {
                SetEvent( m_hScreenOn );
            }
        }
    }
    else if( IsEqualGUID( pPowerBroadcastSetting->PowerSetting, GUID_LOW_POWER_EPOCH ) )
    {
        if( pPowerBroadcastSetting->DataLength != sizeof(ULONG) )
        {
            // error, expect the data to be of size ULONG
            LOG( m_spBasicLog, "ERROR: Low Power Epoch Notification has an invalid data length.  Expected: ULONG (%Iu), Actual: %u.",
                sizeof(ULONG), pPowerBroadcastSetting->DataLength );

            return ERROR_INVALID_PARAMETER;
        }
        else if( pPowerBroadcastSetting->Data == nullptr )
        {
            // error, expect the data variable to be a valid pointer to a ULONG
            LOG( m_spBasicLog, "ERROR: Low Power Epoch Notification has an invalid data pointer.  Data: 0x%p",
                pPowerBroadcastSetting->Data );

            return ERROR_INVALID_PARAMETER;
        }

        ulLowPowerEpoch = *((ULONG *)(pPowerBroadcastSetting->Data));

        if( ulLowPowerEpoch == 0 )
        {
            LOG( m_spBasicLog, L"Power Notification: Low Power Epoch -- EXIT." );

            if( NULL != m_hLowPowerEpochExit )
            {
                SetEvent( m_hLowPowerEpochExit );
            }
        }
        else
        {
            LOG( m_spBasicLog, L"Power Notification: Low Power Epoch -- ENTER." );

            if( NULL != m_hLowPowerEpochEnter )
            {
                SetEvent( m_hLowPowerEpochEnter );
            }
        }
    }

    return ERROR_SUCCESS;
}

HRESULT AdapterPowerManagementTest::SetupSimBatt()
{
    //setup simbatt
    HRESULT hr = S_OK;
    CComPtr<IWDTFSystemDepot2> sysDepot;
    CComPtr<IWDTFTarget2> sysTarget;
    CComBSTR simBatIfName(L"SimulatedBatterySystem");
    VARIANT_BOOL vbSimbattSuccess = VARIANT_TRUE;

    if( !VERIFY_SUCCEEDED( hr = m_wdtf.CoCreateInstance( __uuidof( WDTF2 ), nullptr, CLSCTX_INPROC_SERVER) ) )  { return hr; }
    if( !VERIFY_SUCCEEDED( hr = m_wdtf->get_SystemDepot( &sysDepot ) ) ) { return hr; }
    if( !VERIFY_SUCCEEDED( hr = sysDepot->get_ThisSystem( &sysTarget ) ) ) { return hr; }
    if( !VERIFY_SUCCEEDED( hr = sysTarget->GetInterface(
                                                        CComBSTR("SimulatedBatterySystem"), 
                                                        CComVariant(), 
                                                        CComVariant(), 
                                                        (IWDTFAction2**)&m_pSimulatedBattery) ) )  { return hr; }


    if( !VERIFY_SUCCEEDED( hr = m_pSimulatedBattery->EnableSimulatedBattery(&vbSimbattSuccess) ) )  { return hr; }
    if (!VERIFY_ARE_EQUAL( VARIANT_TRUE, vbSimbattSuccess) )  { return E_FAIL; }

    if( !VERIFY_SUCCEEDED( hr = m_pSimulatedBattery->DisableRealBatteries(&vbSimbattSuccess) ) )  { return hr; }
    if (!VERIFY_ARE_EQUAL( VARIANT_TRUE, vbSimbattSuccess) ) { return E_FAIL; }

    if( !VERIFY_SUCCEEDED( hr = m_pSimulatedBattery->SetSimulatedBatteryToDC(&vbSimbattSuccess) ) ) { return hr; }
    if (!VERIFY_ARE_EQUAL( VARIANT_TRUE, vbSimbattSuccess) ) { return E_FAIL; }

    //sleep to ensure that the system has time to adjust to new power supply, recommended by power SDET
    Sleep(2000);

    return hr;
}

HRESULT AdapterPowerManagementTest::CleanupSimBatt()
{
    HRESULT hr = S_OK;
    //remove simbatt
    VARIANT_BOOL vbSimbattSuccess = VARIANT_FALSE;
    
    if( !VERIFY_SUCCEEDED( hr = m_pSimulatedBattery->SetSimulatedBatteryToAC(&vbSimbattSuccess) ) ) { goto Cleanup; }
    if (!VERIFY_ARE_EQUAL( VARIANT_TRUE, vbSimbattSuccess) ) { hr = E_FAIL; goto Cleanup; }

    if( !VERIFY_SUCCEEDED( hr = m_pSimulatedBattery->EnableRealBatteries(&vbSimbattSuccess) ) ) { goto Cleanup; }
    if (!VERIFY_ARE_EQUAL( VARIANT_TRUE, vbSimbattSuccess) )  { hr = E_FAIL; goto Cleanup; }

    if( !VERIFY_SUCCEEDED( hr = m_pSimulatedBattery->DisableSimulatedBattery(&vbSimbattSuccess) ) ) { goto Cleanup; }
    if (!VERIFY_ARE_EQUAL( VARIANT_TRUE, vbSimbattSuccess) )  { hr = E_FAIL; goto Cleanup; }

    //sleep to ensure that the system has time to adjust to new power supply, recommended by power SDET
    Sleep(2000);

Cleanup:
  
    m_pSimulatedBattery.Release();
    m_wdtf.Release();

    return hr;
}

