// Copyright(C) Microsoft.All rights reserved.

// CSHelperClass.cpp

#pragma once
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <shell98.h>

#include <module98.h>
#include <tchar.h>
#include <dvmodule.h>
// ATL stuff
#include <atlbase.h>
// Audio Core Headers
#include <MMDeviceApi.h>
#include <mmsystem.h>

#include <WexTestClass.h>

#include <strsafe.h>
#include <basiclog.h>
#include <BasicLogHelper.h>
#include <basicprintf.h>
#include <powrprof.h>

#include <initguid.h>
#include <lpe.h>

#include "AudioOffload.h"
#include "QueryMachineInfo.h"
#include "log.h"
#include "DevTrans.h"

#include "CSHelperClass.h"

#include <SleepResume.h>

using namespace WEX::Logging;
using namespace WEX::TestExecution;

void SAFE_CLOSE_HANDLE(HANDLE& h)
{
    if((NULL != h) &&
       (INVALID_HANDLE_VALUE != h))
    {
        CloseHandle(h);
        h = INVALID_HANDLE_VALUE;
    }       
}

HRESULT IsEndpointUnderTestDigitalDisplay(IBasicLog* pLogger, IMMDevice* pMMDevice, BOOL& bIsDigital)
{
    HRESULT hr = S_OK;
    
    if (NULL == pMMDevice)
    {
        ERR( L"FAIL: IMMDevice under test was null!");
        return E_INVALIDARG;
    }

    EndpointFormFactor eFormFactor = EndpointFormFactor::UnknownFormFactor;
    if (!VERIFY_SUCCEEDED(DevTrans::GetEndpointFormFactor(&eFormFactor, pMMDevice)))
    {
        ERR( L"Failed getting endpoint form factor with [hr = 0x%x].", hr);
        return hr;
    }

    bIsDigital = (EndpointFormFactor::DigitalAudioDisplayDevice == eFormFactor);

    return hr;
}



// ----------------------------------------------------------
CSHelperClass::CSHelperClass(IBasicLog* pLogger)
{
    DWORD dwResult = FNS_PASS;

    // create power notification events
    m_heScreenOn = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( NULL == m_heScreenOn )
    {
        ERR( L"FAIL: Failed to Create Screen On Event.  (error=%u)", GetLastError() );
        return;
    }

    m_heScreenOff = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( NULL == m_heScreenOff )
    {
        ERR( L"FAIL: Failed to Create Screen Off Event.  (error=%u)", GetLastError() );
        return;
    }

    m_heLowPowerEpochEnter = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( NULL == m_heLowPowerEpochEnter )
    {
        ERR( L"FAIL: Failed to Create Low Power Epoch Enter Event.  (error=%u)", GetLastError() );
        return;
    }

    m_heLowPowerEpochExit = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( NULL == m_heLowPowerEpochExit )
    {
        ERR( L"FAIL: Failed to Create Low Power Epoch Exit Event.  (error=%u)", GetLastError() );
        return;
    }

    m_PowerNotifyStruct[0].pGUID = &GUID_MONITOR_POWER_ON;
    m_PowerNotifyStruct[0].hPowerNotify = NULL;

    m_PowerNotifyStruct[1].pGUID = &GUID_LOW_POWER_EPOCH;
    m_PowerNotifyStruct[1].hPowerNotify = NULL;

    // Register for the power broadcast notifications
    dwResult = RegisterForPowerBroadcasts(pLogger);
    if( FNS_PASS != dwResult )
    {
        ERR( L"FAIL: Failed to register for Power Broadcasts [rc = 0x%x].", dwResult);
    }

    return;
}

// ----------------------------------------------------------
CSHelperClass::~CSHelperClass()
{
    // logging is completed in UnregisterForPowerBroadcasts
    UnregisterForPowerBroadcasts();
    SAFE_CLOSE_HANDLE(m_heScreenOn);
    SAFE_CLOSE_HANDLE(m_heScreenOff);
    SAFE_CLOSE_HANDLE(m_heLowPowerEpochEnter);
    SAFE_CLOSE_HANDLE(m_heLowPowerEpochExit);
}

   
// ----------------------------------------------------------
BOOL CSHelperClass::EnterConnectedStandby(IBasicLog* pLogger)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    DWORD waitRes = WAIT_FAILED;

    LOG( L"Connected Standby Test: Enter Connected Standby" );

    // There are three actions that cause an AOAC capable machine to enter CS:
    // - Power Button has been pressed
    // - Monitor Powers Off
    // - Start menu Sleep button action
    // Going to use the "Monitor Powers Off" approach

    ResetEvent( m_heScreenOff );
    ResetEvent( m_heLowPowerEpochEnter );
    ntStatus = NtPowerInformation( ScreenOff, NULL, 0, NULL, 0 );
    if( !NT_SUCCESS(ntStatus) )
    {
        ERR( L"FAIL: Failed to turn screen off.  (error = 0x%x)", ntStatus );
        return FALSE;
    }

    // wait sixty seconds for the screen to turn off
    waitRes = WaitForSingleObject( m_heScreenOff, 60000 );
    if( WAIT_TIMEOUT == waitRes )
    {
        ERR( L"FAIL: System failed to turn off monitor within sixty seconds" );
        return FALSE;
    }
    else if( WAIT_OBJECT_0 != waitRes )
    {
        ERR( L"FAIL: Error occured while waiting for Monitor Off Power Event.  (error=%u)", GetLastError() );
        return FALSE;
    }

    // wait 2 minutes for the system to enter low power epoch mode
    WaitForSingleObject( m_heLowPowerEpochEnter, 120000 );
    if( WAIT_TIMEOUT == waitRes )
    {
        ERR( L"FAIL: System failed to enter Low Power Epoch within two minutes" );
        return FALSE;
    }
    else if( WAIT_OBJECT_0 != waitRes )
    {
        ERR( L"FAIL: Error occured while waiting for Low Power Epoch Power Event.  (error=%u)", GetLastError() );
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------
BOOL CSHelperClass::ExitConnectedStandby(IBasicLog* pLogger)
{
    DWORD waitRes = WAIT_FAILED;

    LOG( L"Connected Standby Test: Exit Connected Standby" );

    ResetEvent( m_heScreenOn );
    ResetEvent( m_heLowPowerEpochExit );

    if (!SendKeyInputToExitConnectedStandby())
    {
        ERR( L"FAIL: System has no way to exit Connected Standby" );
        return FALSE;
    }

    waitRes = WaitForSingleObject( m_heLowPowerEpochExit, 120000 );
    if( WAIT_TIMEOUT == waitRes )
    {
        ERR( L"FAIL: System failed to exit Low Power Epoch within two minutes" );
        return FALSE;
    }
    else if( WAIT_OBJECT_0 != waitRes )
    {
        ERR( L"FAIL: Error occured while waiting for Low Power Epoch Power Event.  (error=%u)", GetLastError() );
        return FALSE;
    }
    
    waitRes = WaitForSingleObject( m_heScreenOn, 60000 );
    if( WAIT_TIMEOUT == waitRes )
    {
        ERR( L"FAIL: System failed to turn on monitor within sixty seconds" );
        return FALSE;
    }
    else if( WAIT_OBJECT_0 != waitRes )
    {
        ERR( L"FAIL: Error occured while waiting for Monitor On Power Event.  (error=%u)", GetLastError() );
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------
DWORD CSHelperClass::RegisterForPowerBroadcasts(IBasicLog* pLogger)
{
    DWORD dwResult = FNS_PASS;
    DEVICE_NOTIFY_SUBSCRIBE_PARAMETERS subscribeParams = {0};

    subscribeParams.Callback = (PDEVICE_NOTIFY_CALLBACK_ROUTINE)PowerNotificationCallback;
    subscribeParams.Context = this;

    for( size_t i = 0; i < ARRAYSIZE(m_PowerNotifyStruct); i++ )
    {
        dwResult = PowerSettingRegisterNotification( m_PowerNotifyStruct[i].pGUID, DEVICE_NOTIFY_CALLBACK, &subscribeParams,
            &(m_PowerNotifyStruct[i].hPowerNotify) );
        if( ERROR_SUCCESS != dwResult )
        {
            ERR( L"BLOCK: Failed to Register for %s Power Setting Notifications.  (error=%u)",
                GetPowerGuidName( *(m_PowerNotifyStruct[i].pGUID) ), dwResult );

            return FNS_FAIL;
        }

        LOG( L"Successfully Registered for the %s Power Setting Notification.",
            GetPowerGuidName( *(m_PowerNotifyStruct[i].pGUID) ) );
    }

    return FNS_PASS;
}

// ----------------------------------------------------------
VOID CSHelperClass::UnregisterForPowerBroadcasts()
{
    DWORD dwResult = FNS_PASS;

    CComPtr<IBasicLog> pLogger;
    if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return; }

    for( size_t i = 0; i < ARRAYSIZE(m_PowerNotifyStruct); i++ )
    {
        if( NULL != m_PowerNotifyStruct[i].hPowerNotify )
        {
            dwResult = PowerSettingUnregisterNotification( m_PowerNotifyStruct[i].hPowerNotify );
            if( ERROR_SUCCESS != dwResult )
            {
                ERR( L"BLOCK: Failed to Unregister for %s Power Setting Notifications.  (error=%u)",
                    GetPowerGuidName( *(m_PowerNotifyStruct[i].pGUID) ), dwResult );

                return;
            }

            m_PowerNotifyStruct[i].hPowerNotify = NULL;

            LOG( L"Successfully Unregistered for the %s Power Setting Notification.",
                GetPowerGuidName( *(m_PowerNotifyStruct[i].pGUID) ) );
        }
    }

    return;
}

// ----------------------------------------------------------
ULONG CSHelperClass::DoPowerNotificationCallback( _In_ ULONG Type, _In_ PVOID Setting )
{
    PPOWERBROADCAST_SETTING pPowerBroadcastSetting = (PPOWERBROADCAST_SETTING)Setting;
    ULONG ulLowPowerEpoch = (ULONG)-1;
    ULONG ulMonitorPower = (ULONG)-1;

    CComPtr<IBasicLog> pLogger;
    if (!VERIFY_SUCCEEDED(CreateWexBasicLog(&pLogger))) { return ERROR_INVALID_PARAMETER; }

    if( PBT_POWERSETTINGCHANGE != Type )
    {
        return ERROR_SUCCESS;
    }

    if( IsEqualGUID( pPowerBroadcastSetting->PowerSetting, GUID_MONITOR_POWER_ON ) )
    {
        if( pPowerBroadcastSetting->DataLength != sizeof(ULONG) )
        {
            // error, expect the data to be of size ULONG
            ERR( L"BLOCK: Monitor Power On Notification has an invalid data length.  Expected: ULONG (%Iu), Actual: %u.",
                sizeof(ULONG), pPowerBroadcastSetting->DataLength );

            return ERROR_INVALID_PARAMETER;
        }
        else if( pPowerBroadcastSetting->Data == nullptr )
        {
            // error, expect the data variable to be a valid pointer to a ULONG
            ERR( L"BLOCK: Monitor Power On Notification has an invalid data pointer.  Data: 0x%p",
                pPowerBroadcastSetting->Data );

            return ERROR_INVALID_PARAMETER;
        }

        ulMonitorPower = *((ULONG *)(pPowerBroadcastSetting->Data));

        if( ulMonitorPower == 0 )
        {
            LOG( L"Power Notification: Monitor Power On -- OFF." );

            if( NULL != m_heScreenOff )
            {
                SetEvent( m_heScreenOff );
            }
        }
        else
        {
            LOG( L"Power Notification: Monitor Power On -- ON." );

            if( NULL != m_heScreenOn )
            {
                SetEvent( m_heScreenOn );
            }
        }
    }
    else if( IsEqualGUID( pPowerBroadcastSetting->PowerSetting, GUID_LOW_POWER_EPOCH ) )
    {
        if( pPowerBroadcastSetting->DataLength != sizeof(ULONG) )
        {
            // error, expect the data to be of size ULONG
            ERR( L"BLOCK: Low Power Epoch Notification has an invalid data length.  Expected: ULONG (%Iu), Actual: %u.",
                sizeof(ULONG), pPowerBroadcastSetting->DataLength );

            return ERROR_INVALID_PARAMETER;
        }
        else if( pPowerBroadcastSetting->Data == nullptr )
        {
            // error, expect the data variable to be a valid pointer to a ULONG
            ERR( L"BLOCK: Low Power Epoch Notification has an invalid data pointer.  Data: 0x%p",
                pPowerBroadcastSetting->Data );

            return ERROR_INVALID_PARAMETER;
        }

        ulLowPowerEpoch = *((ULONG *)(pPowerBroadcastSetting->Data));

        if( ulLowPowerEpoch == 0 )
        {
            LOG( L"Power Notification: Low Power Epoch -- EXIT." );

            if( NULL != m_heLowPowerEpochExit )
            {
                SetEvent( m_heLowPowerEpochExit );
            }
        }
        else
        {
            LOG( L"Power Notification: Low Power Epoch -- ENTER." );

            if( NULL != m_heLowPowerEpochEnter )
            {
                SetEvent( m_heLowPowerEpochEnter );
            }
        }
    }

    return ERROR_SUCCESS;
}

// ----------------------------------------------------------
ULONG CSHelperClass::PowerNotificationCallback( _In_opt_ PVOID Context, _In_ ULONG Type, _In_ PVOID Setting )
{
    CSHelperClass* pThis = (CSHelperClass *)Context;

    if( nullptr == pThis )
    {
        return ERROR_INVALID_HANDLE;
    }

    return pThis->DoPowerNotificationCallback( Type, Setting );
}

// ----------------------------------------------------------
LPCTSTR CSHelperClass::GetPowerGuidName( GUID powerGuid )
{
    if( IsEqualGUID( GUID_MONITOR_POWER_ON, powerGuid ) )
    {
        return TEXT("GUID_MONITOR_POWER_ON");
    }

    if( IsEqualGUID( GUID_LOW_POWER_EPOCH, powerGuid ) )
    {
        return TEXT("GUID_LOW_POWER_EPOCH");
    }

    return TEXT("Unknown Power Registration GUID");
}

