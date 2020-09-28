//
// Copyright(C) Microsoft.All rights reserved.
// 
// UtilityTestComponents.cpp
//
// Description:
//
// Utility Test Class Implementations
//


#include "StdAfx.h"
#include "ntpoapi.h"
#include "initguid.h"
#include "lpe.h"
#include "powrprof.h"
#include "AudioOffload.h"
#include "QueryMachineInfo.h"
#include "AudioGlitchTracer.h"

//******************************************************************************
//  CCSBaseTest class implementation methods
//******************************************************************************
CCSBaseTest::CCSBaseTest(BOOL bGlitchTest)
:  m_heScreenOn(NULL), m_heScreenOff(NULL), m_heLowPowerEpochEnter(NULL), m_heLowPowerEpochExit(NULL),
  m_bGlitchTest(bGlitchTest)
{
    m_PowerNotifyStruct[0].pGUID = &GUID_MONITOR_POWER_ON;
    m_PowerNotifyStruct[0].hPowerNotify = NULL;

    m_PowerNotifyStruct[1].pGUID = &GUID_LOW_POWER_EPOCH;
    m_PowerNotifyStruct[1].hPowerNotify = NULL;
}

// ----------------------------------------------------------
CCSBaseTest::~CCSBaseTest()
{
}

// ----------------------------------------------------------
BOOL CCSBaseTest::Initialize()
{
    HRESULT hr = S_OK;
    DWORD dwResult = FNS_PASS;
    CComPtr<IBasicLog> spLogger = NULL;

    //  Get an IBasicLog
    hr = g_IShell->GetBasicLog(&spLogger);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Unable to create IBasicLog!");
        return FALSE;
    }

    // create power notification events
    m_heScreenOn = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( NULL == m_heScreenOn )
    {
        XLOG( XFAIL, eError, L"FAIL: Failed to Create Screen On Event.  (error=%u)", GetLastError() );
        return FALSE;
    }

    m_heScreenOff = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( NULL == m_heScreenOff )
    {
        XLOG( XFAIL, eError, L"FAIL: Failed to Create Screen Off Event.  (error=%u)", GetLastError() );
        return FALSE;
    }

    m_heLowPowerEpochEnter = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( NULL == m_heLowPowerEpochEnter )
    {
        XLOG( XFAIL, eError, L"FAIL: Failed to Create Low Power Epoch Enter Event.  (error=%u)", GetLastError() );
        return FALSE;
    }

    m_heLowPowerEpochExit = CreateEvent( NULL, FALSE, FALSE, NULL );
    if( NULL == m_heLowPowerEpochExit )
    {
        XLOG( XFAIL, eError, L"FAIL: Failed to Create Low Power Epoch Exit Event.  (error=%u)", GetLastError() );
        return FALSE;
    }

    // Register for the power broadcast notifications
    dwResult = RegisterForPowerBroadcasts();
    if( FNS_PASS != dwResult )
    {
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------
VOID CCSBaseTest::CleanUp()
{
    // logging is completed in UnregisterForPowerBroadcasts
    // ignoring return value
    (void) UnregisterForPowerBroadcasts();
    SAFE_CLOSE_HANDLE(m_heScreenOn);
    SAFE_CLOSE_HANDLE(m_heScreenOff);
    SAFE_CLOSE_HANDLE(m_heLowPowerEpochEnter);
    SAFE_CLOSE_HANDLE(m_heLowPowerEpochExit);
}

// ----------------------------------------------------------
DWORD CCSBaseTest::RunDuringTest(INT nSyncronous)
{
    BOOL fRes = TRUE;

    XLOG( XMSG, eInfo1, L"Connected Standby Test: Test During Connected Standby" );

    if( SYNC == nSyncronous )
    {
        // TODO: ensure that the audio device is in low power S0 by listening to Kernel PoFx ETW Events
        // now, just sleep for 60 seconds
        Sleep( 60000 );
    }
    else if( ASYNC == nSyncronous )
    {
        //Just sleep for 60 seconds
        Sleep( 60000 );
    }

    return BoolToFns(fRes);
}

// ----------------------------------------------------------
BOOL CCSBaseTest::EnterConnectedStandby()
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    DWORD waitRes = WAIT_FAILED;

    XLOG( XMSG, eInfo1, L"Connected Standby Test: Enter Connected Standby" );

    // There are three actions that cause an AOAC capable machine to enter CS:
    // - Power Button has been pressed
    // - Monitor Powers Off
    // - Start menu Sleep button action
    // Going to use the L"Monitor Powers Off" approach

    ResetEvent( m_heScreenOff );
    ResetEvent( m_heLowPowerEpochEnter );
    ntStatus = NtPowerInformation( ScreenOff, NULL, 0, NULL, 0 );
    if( !NT_SUCCESS(ntStatus) )
    {
        XLOG( XFAIL, eError, L"FAIL: Failed to turn screen off.  (error=%u)", RtlNtStatusToDosError(ntStatus) );
        return FALSE;
    }

    // wait sixty seconds for the screen to turn off
    waitRes = WaitForSingleObject( m_heScreenOff, 60000 );
    if( WAIT_TIMEOUT == waitRes )
    {
        XLOG( XFAIL, eError, L"FAIL: System failed to turn off monitor within sixty seconds" );
        return FALSE;
    }
    else if( WAIT_OBJECT_0 != waitRes )
    {
        XLOG( XFAIL, eError, L"FAIL: Error occured while waiting for Monitor Off Power Event.  (error=%u)", GetLastError() );
        return FALSE;
    }

    // wait 2 minutes for the system to enter low power epoch mode
    WaitForSingleObject( m_heLowPowerEpochEnter, 120000 );
    if( WAIT_TIMEOUT == waitRes )
    {
        XLOG( XFAIL, eError, L"FAIL: System failed to enter Low Power Epoch within two minutes" );
        return FALSE;
    }
    else if( WAIT_OBJECT_0 != waitRes )
    {
        XLOG( XFAIL, eError, L"FAIL: Error occured while waiting for Low Power Epoch Power Event.  (error=%u)", GetLastError() );
        return FALSE;
    }

    return TRUE;
}

#define GPIO_BUTTON_POWER 0
#define InputEventFlag_DownAndUp 0x0005
typedef HRESULT (*PFNApiInjectInitialize)();
typedef HRESULT (*PFNApiInjectUninitialize)();

typedef HRESULT (*PFNApiInjectButtonEvent)(
    IN WORD wEventFlag,         // Use enum InputEventFlag
    IN WORD wButtonType);       // Use enum GPIOBUTTONS_BUTTON_TYPE

typedef UINT (WINAPI *PFNSendInput)(
    __in UINT cInputs,                     // number of input in the array
    __in_ecount(cInputs) LPINPUT pInputs,  // array of inputs
    __in int cbSize);                      // sizeof(INPUT)

BOOL CCSBaseTest::SendKeyInput()
{
    BOOL result = FALSE;
    // First try using SendInput
    HMODULE hUser32 = LoadLibrary(L"user32.dll");
    if( hUser32 )
    {
        // Use SendInput
        PFNSendInput pfnSendInput = (PFNSendInput)GetProcAddress( hUser32, "SendInput" );
        if( pfnSendInput )
        {
            INPUT inputs[2];

            inputs[0].type = inputs[1].type = INPUT_KEYBOARD;
            inputs[0].ki.wVk = inputs[1].ki.wVk = VK_SPACE;
            inputs[0].ki.dwFlags = 0;
            inputs[1].ki.dwFlags = KEYEVENTF_KEYUP;

            pfnSendInput( ARRAYSIZE(inputs), inputs, sizeof(INPUT) );
            result = TRUE;
        }
        FreeLibrary( hUser32 );
    }
    
    if (!result)
    {
        // SendInput didn't work, use ApiInjectButtonEvent
        HMODULE hii = LoadLibrary(L"InputInjection.dll");
        if( hii )
        {
            PFNApiInjectInitialize pfnApiInjectInitialize = (PFNApiInjectInitialize)GetProcAddress( hii, "ApiInjectInitialize" );
            PFNApiInjectUninitialize pfnApiInjectUninitialize = (PFNApiInjectUninitialize)GetProcAddress( hii, "ApiInjectUninitialize" );
            PFNApiInjectButtonEvent pfnApiInjectButtonEvent = (PFNApiInjectButtonEvent)GetProcAddress( hii, "ApiInjectButtonEvent" );
            if( pfnApiInjectInitialize && pfnApiInjectUninitialize && pfnApiInjectButtonEvent )
            {
                pfnApiInjectInitialize();
                pfnApiInjectButtonEvent( InputEventFlag_DownAndUp, GPIO_BUTTON_POWER );
                pfnApiInjectUninitialize();
                result = TRUE;
            }
            FreeLibrary(hii);
        }
    }
    return result;    
}

// ----------------------------------------------------------
BOOL CCSBaseTest::ExitConnectedStandby()
{
    DWORD waitRes = WAIT_FAILED;

    XLOG( XMSG, eInfo1, L"Connected Standby Test: Exit Connected Standby" );

    // There are four actions that cause an AOAC capable machine to exit CS:
    // - Power Button has been pressed
    // - Windows Button being Pressed
    // - Direct user input (keyboard press)
    // - Monitor Power On (DisplayRequest)
    // Going to use the L"Direct user input (keyboard press) approach

    ResetEvent( m_heScreenOn );
    ResetEvent( m_heLowPowerEpochExit );

    if( !SendKeyInput() )
    {
        XLOG( XSKIP, eError, L"SKIP: Device doesn't support known wake-up methods" );
        return FALSE;
    }

    waitRes = WaitForSingleObject( m_heLowPowerEpochExit, 120000 );
    if( WAIT_TIMEOUT == waitRes )
    {
        XLOG( XFAIL, eError, L"FAIL: System failed to exit Low Power Epoch within two minutes" );
        return FALSE;
    }
    else if( WAIT_OBJECT_0 != waitRes )
    {
        XLOG( XFAIL, eError, L"FAIL: Error occured while waiting for Low Power Epoch Power Event.  (error=%u)", GetLastError() );
        return FALSE;
    }
    
    waitRes = WaitForSingleObject( m_heScreenOn, 60000 );
    if( WAIT_TIMEOUT == waitRes )
    {
        XLOG( XFAIL, eError, L"FAIL: System failed to turn on monitor within sixty seconds" );
        return FALSE;
    }
    else if( WAIT_OBJECT_0 != waitRes )
    {
        XLOG( XFAIL, eError, L"FAIL: Error occured while waiting for Monitor On Power Event.  (error=%u)", GetLastError() );
        return FALSE;
    }

    return TRUE;
}

// ----------------------------------------------------------
DWORD CCSBaseTest::RegisterForPowerBroadcasts()
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
            XLOG( XBLOCK, eError, L"BLOCK: Failed to Register for %s Power Setting Notifications.  (error=%u)",
                GetPowerGuidName( *(m_PowerNotifyStruct[i].pGUID) ), dwResult );

            return FNS_FAIL;
        }

        XLOG( XMSG, eInfo1, L"Successfully Registered for the %s Power Setting Notification.",
            GetPowerGuidName( *(m_PowerNotifyStruct[i].pGUID) ) );
    }

    return FNS_PASS;
}

// ----------------------------------------------------------
DWORD CCSBaseTest::UnregisterForPowerBroadcasts()
{
    DWORD dwResult = FNS_PASS;

    for( size_t i = 0; i < ARRAYSIZE(m_PowerNotifyStruct); i++ )
    {
        if( NULL != m_PowerNotifyStruct[i].hPowerNotify )
        {
            dwResult = PowerSettingUnregisterNotification( m_PowerNotifyStruct[i].hPowerNotify );
            if( ERROR_SUCCESS != dwResult )
            {
                XLOG( XBLOCK, eError, L"BLOCK: Failed to Unregister for %s Power Setting Notifications.  (error=%u)",
                    GetPowerGuidName( *(m_PowerNotifyStruct[i].pGUID) ), dwResult );

                return FNS_FAIL;
            }

            m_PowerNotifyStruct[i].hPowerNotify = NULL;

            XLOG( XMSG, eInfo1, L"Successfully Unregistered for the %s Power Setting Notification.",
                GetPowerGuidName( *(m_PowerNotifyStruct[i].pGUID) ) );
        }
    }

    return FNS_PASS;
}

// ----------------------------------------------------------
ULONG CCSBaseTest::DoPowerNotificationCallback( _In_ ULONG Type, _In_ PVOID Setting )
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
            XLOG( XBLOCK, eError, L"BLOCK: Monitor Power On Notification has an invalid data length.  Expected: ULONG (%Iu), Actual: %u.",
                sizeof(ULONG), pPowerBroadcastSetting->DataLength );

            return ERROR_INVALID_PARAMETER;
        }
        else if( pPowerBroadcastSetting->Data == nullptr )
        {
            // error, expect the data variable to be a valid pointer to a ULONG
            XLOG( XBLOCK, eError, L"BLOCK: Monitor Power On Notification has an invalid data pointer.  Data: 0x%p",
                pPowerBroadcastSetting->Data );

            return ERROR_INVALID_PARAMETER;
        }

        ulMonitorPower = *((ULONG *)(pPowerBroadcastSetting->Data));

        if( ulMonitorPower == 0 )
        {
            XLOG( XMSG, eInfo1, L"Power Notification: Monitor Power On -- OFF." );

            if( NULL != m_heScreenOff )
            {
                SetEvent( m_heScreenOff );
            }
        }
        else
        {
            XLOG( XMSG, eInfo1, L"Power Notification: Monitor Power On -- ON." );

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
            XLOG( XBLOCK, eError, L"BLOCK: Low Power Epoch Notification has an invalid data length.  Expected: ULONG (%Iu), Actual: %u.",
                sizeof(ULONG), pPowerBroadcastSetting->DataLength );

            return ERROR_INVALID_PARAMETER;
        }
        else if( pPowerBroadcastSetting->Data == nullptr )
        {
            // error, expect the data variable to be a valid pointer to a ULONG
            XLOG( XBLOCK, eError, L"BLOCK: Low Power Epoch Notification has an invalid data pointer.  Data: 0x%p",
                pPowerBroadcastSetting->Data );

            return ERROR_INVALID_PARAMETER;
        }

        ulLowPowerEpoch = *((ULONG *)(pPowerBroadcastSetting->Data));

        if( ulLowPowerEpoch == 0 )
        {
            XLOG( XMSG, eInfo1, L"Power Notification: Low Power Epoch -- EXIT." );

            if( NULL != m_heLowPowerEpochExit )
            {
                SetEvent( m_heLowPowerEpochExit );
            }
        }
        else
        {
            XLOG( XMSG, eInfo1, L"Power Notification: Low Power Epoch -- ENTER." );

            if( NULL != m_heLowPowerEpochEnter )
            {
                SetEvent( m_heLowPowerEpochEnter );
            }
        }
    }

    return ERROR_SUCCESS;
}

// ----------------------------------------------------------
ULONG CCSBaseTest::PowerNotificationCallback( _In_opt_ PVOID Context, _In_ ULONG Type, _In_ PVOID Setting )
{
    CCSBaseTest* pThis = (CCSBaseTest *)Context;

    if( nullptr == pThis )
    {
        return ERROR_INVALID_HANDLE;
    }

    return pThis->DoPowerNotificationCallback( Type, Setting );
}

// ----------------------------------------------------------
LPCTSTR CCSBaseTest::GetPowerGuidName( GUID powerGuid )
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

HRESULT CCSBaseTest::SetupAndStartTracing(_Outptr_result_nullonfailure_ LPWSTR* ppwszTraceName)
{
    HRESULT hr;
    CComPtr<IBasicLog> spLogger;
    bool audioGlitchTracingInitialized = false;
    CComHeapPtr<WCHAR> temp;

    *ppwszTraceName = NULL;
    
    //  Get an IBasicLog
    hr = g_IShell->GetBasicLog(&spLogger);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Unable to create IBasicLog!");
        goto ERR;
    }

    hr = AudioGlitchTracer::Initialize(spLogger);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Initializing Tracer Library failed (%s).", GetHRString(hr));
        goto ERR;
    }
    audioGlitchTracingInitialized = true;

    hr = AudioGlitchTracer::GenerateTraceName(&temp);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Generating Trace Name failed (%s).", GetHRString(hr));
        goto ERR;
    }

    // start capturing ETW trace info
    hr = AudioGlitchTracer::StartETW(temp);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Error starting ETW tracing (%s).", GetHRString(hr));
        goto ERR;
    }

    *ppwszTraceName = temp.Detach();
    return S_OK;
    
ERR:
    if (audioGlitchTracingInitialized)
    {
        AudioGlitchTracer::Cleanup();
    }
    
    return hr;
}

HRESULT CCSBaseTest::StopProcessAndCleanupTracing(LPCWSTR pwszTraceName)
{
    HRESULT hr = S_OK;
    UINT64 nGlitchEvents = 0;
    // Stop capturing ETW trace info
    hr = AudioGlitchTracer::StopETW(pwszTraceName);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Error stopping ETW tracing (%s).", GetHRString(hr));
        goto CLEANUP;
    }

    // analyze the trace info to determine the number of glitches
    hr = AudioGlitchTracer::ProcessETWTrace(pwszTraceName, &nGlitchEvents);
    if (FAILED(hr))
    {
        XLOG(XFAIL, eError, L"FAIL: Error analyzing ETL file (%s)", GetHRString(hr));
        goto CLEANUP;
    }

    if (0 == nGlitchEvents)
    {
        // delete the etw logs
        hr = AudioGlitchTracer::DeleteETWTraceFiles(pwszTraceName);
        if (FAILED(hr))
        {
            // log a warning but don't fail the test
            XLOG(XWARN, eError, L"WARNING: Error deleting ETL files for trace\"%s\" (%s)",
                pwszTraceName, GetHRString(hr));
        }
    }
    else
    {
        XLOG(XFAIL, eInfo1, L"FAIL: ETW trace information for this test can be found in %s_merged.etl", pwszTraceName);
    }

CLEANUP:
    AudioGlitchTracer::Cleanup();
    return hr;
}
