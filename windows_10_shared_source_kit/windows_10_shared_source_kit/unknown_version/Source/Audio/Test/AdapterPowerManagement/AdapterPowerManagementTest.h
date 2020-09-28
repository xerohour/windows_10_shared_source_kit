//--------------------------------------------------------------------------
//
//  File: AdapterPowerManagementTest.h
//
// Copyright (C) Microsoft. All rights reserved.
//
//  History:
//      02/14/2012    CaWerner       Initial implementation.
//
//--------------------------------------------------------------------------
#pragma once

#include "common.h"
#include "callcontroller.h"
#include "fmcontroller.h"
#include <wdtf.h>
#include <WDTFSimulatedBatterySystemAction.h>

class AdapterPowerManagementTest
{

public:

protected:

    struct PowerNotifyStruct
    {
        const GUID* pGUID;
        HPOWERNOTIFY hPowerNotify;
    };

    HRESULT VerifyEvents( PortClsEventLogger* pPortClsEventLogger, const PowerPolicyEvent* eventVerifyArray, size_t numExpectedEvents );
    HRESULT DisableAllAudioDevnodes();
    HRESULT RestoreAllAudioDevnodes();
    HRESULT DeviceUnderTestIncreasePinCount( IAudioClient** pAudioClient, bool bIsCSTest = false );
    HRESULT DeviceUnderTestPowerState( DEVICE_POWER_STATE devicePowerState );
    HRESULT DelayConservationIdleTime();
    HRESULT GetCallController( _In_ CallController *pCallController );
    HRESULT GetFmController( _In_ FmController *pFmController );
    HRESULT DoesSystemSupportAoAc( bool* isAoAc );
    HRESULT IsSystemOnDC( bool* onDC );
    HRESULT EnterConnectedStandby();
    HRESULT ExitConnectedStandby();
    HRESULT RegisterForPowerBroadcasts();
    HRESULT UnregisterForPowerBroadcasts();
    LPCWSTR GetDevicePowerStateName( DEVICE_POWER_STATE devicePowerState );
    LPCWSTR GetPowerGuidName( GUID powerGuid );
    LPCWSTR GetEventTypeString( const PowerPolicyEvent* pPowerPolicyEvent );
    LPCWSTR GetEventValueString( const PowerPolicyEvent* pPowerPolicyEvent );
    LPCWSTR GetEventCompleteString( const PowerPolicyEvent* pPowerPolicyEvent );
    HRESULT LogEvent( const PowerPolicyEvent* pPowerPolicyEvent );
    ULONG DoPowerNotificationCallback( _In_ ULONG Type, _In_ PVOID Setting );

    static ULONG PowerNotificationCallback( _In_opt_ PVOID Context, _In_ ULONG Type, _In_ PVOID Setting );

    HRESULT SetupSimBatt();
    HRESULT CleanupSimBatt();

    CComPtr<IWDTF2> m_wdtf;
    CComPtr<IWDTFSimulatedBatterySystemAction> m_pSimulatedBattery;

    CComPtr<IBasicLog> m_spBasicLog;
    PowerNotifyStruct m_PowerNotifyStruct[2];
    HANDLE m_hLowPowerEpochExit;
    HANDLE m_hLowPowerEpochEnter;
    HANDLE m_hScreenOff;
    HANDLE m_hScreenOn;

};

class AdapterPowerManagementTestETW : public AdapterPowerManagementTest, public WEX::TestClass<AdapterPowerManagementTestETW>
{
    
    BEGIN_TEST_CLASS(AdapterPowerManagementTestETW)
    END_TEST_CLASS()

    TEST_CLASS_SETUP(SetupClass);
    TEST_CLASS_CLEANUP(CleanupClass);

    TEST_METHOD_SETUP(SetupTest);
    TEST_METHOD_CLEANUP(CleanupTest);

    //
    //  IAdapterPowerManagement3 Test Cases
    //

    BEGIN_TEST_METHOD(PowerChangeState3_S0_D0toD3toD0)
        TEST_METHOD_PROPERTY( L"TestClassification:Scope", L"Feature" )
        TEST_METHOD_PROPERTY( L"ResourceSelection", L"@Id = 'HDAUDIO*'" )
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(PowerChangeState3_ConnectedStandby_D0toD3toD0)
        TEST_METHOD_PROPERTY( L"TestClassification:Scope", L"Feature" )
        TEST_METHOD_PROPERTY( L"ResourceSelection", L"@Id = 'HDAUDIO*'" )
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(PowerChangeState3_EnterExitConnectedStandby_D0)
        TEST_METHOD_PROPERTY( L"TestClassification:Scope", L"Feature" )
        TEST_METHOD_PROPERTY( L"ResourceSelection", L"@Id = 'HDAUDIO*'" )
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(PowerChangeState3_EnterExitConnectedStandby_D3)
        TEST_METHOD_PROPERTY( L"TestClassification:Scope", L"Feature" )
        TEST_METHOD_PROPERTY( L"ResourceSelection", L"@Id = 'HDAUDIO*'" )
    END_TEST_METHOD()

private:
        void PowerChangeState_S0_D0toD3toD0(BOOL bVerifyEvents);
        void PowerChangeState_ConnectedStandby_D0toD3toD0(BOOL bVerifyEvents);
        void PowerChangeState_EnterExitConnectedStandby_D0(BOOL bVerifyEvents);
        void PowerChangeState_EnterExitConnectedStandby_D3(BOOL bVerifyEvents);
};

class AdapterPowerManagementTestMobile : public AdapterPowerManagementTest, public WEX::TestClass<AdapterPowerManagementTestMobile>
{
    
    BEGIN_TEST_CLASS(AdapterPowerManagementTestMobile)
    END_TEST_CLASS()

    TEST_CLASS_SETUP(SetupClassMobile);
    TEST_CLASS_CLEANUP(CleanupClassMobile);

    TEST_METHOD_SETUP(SetupTestMobile);
    TEST_METHOD_CLEANUP(CleanupTestMobile);

    //
    //  IAdapterPowerManagement Test Cases
    //

    BEGIN_TEST_METHOD(CellularPowerChangeState_D3toD0toD3)
        TEST_METHOD_PROPERTY( L"TestClassification:Scope", L"HCK" )
        TEST_METHOD_PROPERTY( L"ResourceSelection", L"@Id = '*'" )
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(FmPowerChangeState_D3toD0toD3)
        TEST_METHOD_PROPERTY( L"TestClassification:Scope", L"HCK" )
        TEST_METHOD_PROPERTY( L"ResourceSelection", L"@Id = '*'" )
    END_TEST_METHOD()

    BEGIN_TEST_METHOD(Fm_Cellular_Interaction_PowerChangeState_D3toD0toD3)
        TEST_METHOD_PROPERTY( L"TestClassification:Scope", L"HCK" )
        TEST_METHOD_PROPERTY( L"ResourceSelection", L"@Id = '*'" )
    END_TEST_METHOD()

private:
};


