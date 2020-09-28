/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    battery.h

Abstract:

    Common header for the battery subsystem

Environment:

    Boot
    

--*/

#pragma once

#include <BatteryCharging.h>

//
// Are we above threshold?
//
#define IS_ABOVE_THRESHOLD( _t_, _b_)           ( _b_ >= _t_)

//
// Timeout value during threshold charging, in seconds
// 
#define THRESHOLD_TIMEOUT                       ( 300 * SECONDS )  

//
// Default battery threshold value for booting into the main OS
//
#define BATTERY_CHARGING_BOOT_THRESHOLD         ( 10 )

//
// The OEM should not set a threshold limit more than this value
// 
#define BATTERY_CHARGING_BOOT_THRESHOLD_LIMIT   ( 50 )

//
// Minimum allowed Rated Capacity of the battery in mAh
//
#define MINIMUM_BATTERY_RATED_CAPACITY          ( 100 )

//
// Maximum allowed Rated Capacity of the battery in mAh
//
#define MAXIMUM_BATTERY_RATED_CAPACITY          ( 10000 )

//
// Skip charging threshold value
//
#define BATTERY_SKIP_CHARGING                   ( 0xFF )

//
// From the UEFI Battery Charging functional spec (last updated March 8
// 2013), we have the following cases:
//  * Charging Downstream Port -> 1.5A
//  * Standard Downstream Port -> 500mA
//  * Standard Downstream Port (3.0) -> 900mA
//  * Dedicated Charging Port -> 1.5A
//  * Suspended Port -> 100mA
// These current values are in mA
//
#define CURRENT_CHARGING_DOWNSTREAM             ( 1500 )
#define CURRENT_STANDARD_DOWNSTREAM             ( 500 )
#define CURRENT_STANDARD_DOWNSTREAM_30          ( 900 )
#define CURRENT_DEDICATED_CHARGING              ( 1500 )
#define CURRENT_SUSPENDED                       ( CURRENT_STANDARD_DOWNSTREAM )


//
// MIN and MAX values we expect from the battery protocol
//
#define CHARGE_MAX                              ( 100 )
#define CHARGE_MIN                              ( 0 )

//
// Default UNKNOWN values for certain metrics
//
#define UNKNOWN_CURRENT_INTO_BATTERY                ( -8888 )
#define UNKNOWN_VOLTAGE_ACROSS_BATTERY_TERMINALS    ( 0 )
#define UNKNOWN_VOLTAGE_OVER_USB_CABLE              ( 0 )
#define UNKNOWN_CURRENT_THROUGH_USB_CABLE           ( 0 )
#define UNKNOWN_BATTERY_TEMPERATURE                 ( -8888 )

//
// Charge timeout
//
// #define CHARGE_TIMEOUT_ENABLED
#undef CHARGE_TIMEOUT_ENABLED

//
// Time in seconds about how often the battery will be
// polled to get an update about the state of charge
// This is in seconds
//
#define BATTERY_POLL_INTERVAL                   ( 50 )

#define IS_POLL_TIME( __x__ )                   ( __x__ % (BATTERY_POLL_INTERVAL) == 0 )


//
// This is the delta to be applied on top of the threshold retrieved from the BCD
// when the phone determines it needs to boot into the Update OS
//
#define UPDATE_OS_THRESHOLD_DELTA               ( 8 )

#define GET_UPDATE_OS_THRESHOLD( __x__ )        ( __x__ + UPDATE_OS_THRESHOLD_DELTA)

//
// Continue loop counter limit
//
#define SANITY_CHECK_LOOP_LIMIT                 ( 1 )

//
// Scancode for the power button
// Open issue: this should be changed to BL_SCAN_SUSPEND once
// the changes make to mobilecore
//
#define POWER_BUTTON                            ( BL_SCAN_SUSPEND )

#define POWER_BUTTON_LONG_PRESS_DELAY           ( 2 * SECONDS )

#define CHARGE_PAUSE_DELAY                      ( 300 * SECONDS )

// Overrides

#define DEFAULT_BATTERY_CHARGING_MODE           ( FALSE )
#define DEFAULT_CONNECTED_TO_SOURCE             ( FALSE )
#define IS_GOING_TO_BATTERY_CHARGING_MODE       ( FALSE )

//
// This UI is calculated as an index into the ChargeGraphics array.
// The array is a sequence of [ NOPLUG, PLUG ] elements, starting from
// BATTERY_0 (empty battery), i.e [BATTERY_0, BATTERY_PLUG_0]. 
// Each duple following that is an increase of 10% of the battery. 
//
// The UI is based on the Main OS System Tray data model available at:
// \src\media\shell\product\shellevents\systemtraydatamodel.cpp
//
// In particular, this is the mapping:
// 
//        Battery %            Image Level             Index [PLUG, NOPLUG]
//          [0]                 BATTERY_0                   [0,1]
//          [1,9]               BATTERY_1                   [2,3]
//          [10,19]             BATTERY_2                   [4,5]
//          [20,29]             BATTERY_3                   [6,7]
//          [30,39]             BATTERY_4                   [8,9]
//          [40,49]             BATTERY_5                   [10,11]
//          [50,59]             BATTEYR_6                   [12,13]
//          [60,69]             BATTERY_7                   [14,15]
//          [70,79]             BATTERY_8                   [16,17]
//          [80,89]             BATTERY_9                   [18,19]
//          [90,99]             BATTERY_10                  [20,21]
//          [100]               BATTERY_11                  [22,23]
//
// N.B. the case of 0 needs to be handled separately. For all other cases, 
// the following macro will work. 
//
#define CHARGING_UI_NOPLUG( _b_ )       ( ( _b_ / 10 + 1 ) * 2 )
#define CHARGING_UI_PLUG( _b_ )         ( ( _b_ / 10 + 1 ) * 2 + 1 )



#define CONVERT_TO_CYCLES(__Seconds__, __Frequency__) \
                (__Seconds__ * (__Frequency__ / (1000 * 1000)));
                                
                                
//                                
// Different states that the charging application
// can be in
//
typedef enum _CHARGE_STATE
{
    StateDefault,
    StateChargingUntilThreshold,
    StateBatteryChargingMode,
    StateUserInvokedChargeUntilThreshold
} CHARGE_STATE;
    
    
//
// This enum is used to translate from platform-specific operation status into
// charge app specific status
//    
typedef enum _CHARGE_STATUS
{
    None = 0,
    Success,
    Overheat,
    VoltageOutOfRange,
    CurrentOutOfRange,
    Timeout,
    Aborted,
    DeviceError,
    ExtremeCold,
    ChargingNotSupported,
    BatteryNotDetected,
    SourceNotDetected,
    SourceVoltageInvalid,
    SourceCurrentInvalid,
    RequestShutdown,
    RequestReboot,
    Pause

} CHARGE_STATUS;
    
        

NTSTATUS Sleep( UINT32 time );

NTSTATUS BatteryChargingProtocolInit();

NTSTATUS UpdateBatteryStatus();

NTSTATUS ChargeBattery(UINT32 MaximumCurrent, UINT32 TargetStateOfCharge);

NTSTATUS Charge(UINT32 StateOfCharge, CHARGE_STATE State);

NTSTATUS BatteryEventHandler();

NTSTATUS CloseCompletionTokenEvent();

NTSTATUS DestroyBatterySubsystem();
