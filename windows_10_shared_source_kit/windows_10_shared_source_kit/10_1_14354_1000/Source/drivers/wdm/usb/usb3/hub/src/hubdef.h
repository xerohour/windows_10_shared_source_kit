/*++
Copyright (c) Microsoft Corporation

Module Name:

    hubdef.h

Abstract:

    USB Spec. Defined Structures and Values for the Hub

--*/

#pragma once

//
// USB Hub Class Requests
// USB 2.0 Spec. 11.24.2
// USB 3.0 Spec. 10.14.2
//
typedef enum _USB_HUB_CLASS_REQUEST {
    GET_STATUS          =  0,
    CLEAR_FEATURE       =  1,
    SET_FEATURE         =  3,
    GET_DESCRIPTOR      =  6,
    SET_DESCRIPTOR      =  7,
    CLEAR_TT_BUFFER     =  8,
    RESET_TT            =  9,
    GET_TT_STATE        = 10,
    STOP_TT             = 11,
    SET_HUB_DEPTH       = 12,
    GET_PORT_ERR_COUNT  = 13
} USB_HUB_CLASS_REQUEST, *PUSB_HUB_CLASS_REQUEST;


//
// USB Standard Endpoint Feature Selectors
// USB 3.0 Spec. 9.4
// USB 2.0 Spec. 9.4
//
typedef enum _USB_STANDARD_ENDPOINT_FEATURE_SELECTOR {
    ENDPOINT_HALT = 0,
} USB_STANDARD_ENDPOINT_FEATURE_SELECTOR, *PUSB_STANDARD_ENDPOINT_FEATURE_SELECTOR;


//
// USB Standard Interface Feature Selectors
// USB 3.0 Spec. 9.4
// USB 2.0 Spec. 9.4
//
typedef enum _USB_STANDARD_INTERFACE_FEATURE_SELECTOR {
    FUNCTION_SUSPEND = 0,
} USB_STANDARD_INTERFACE_FEATURE_SELECTOR, *PUSB_STANDARD_INTERFACE_FEATURE_SELECTOR;

typedef union _FUNCTION_SUSPEND_OPTIONS {

    UCHAR       AsUchar;
    struct {
        UCHAR   Suspend:1;
        UCHAR   RemoteWake:1;
        UCHAR   Reserved:6;
    };     
} FUNCTION_SUSPEND_OPTIONS, *PFUNCTION_SUSPEND_OPTIONS;

//
// USB Standard Device Feature Selectors
// USB 3.0 Spec. 9.4
// USB 2.0 Spec. 9.4
//
typedef enum _USB_STANDARD_DEVICE_FEATURE_SELECTOR {
    DEVICE_REMOTE_WAKEUP =  1,
    TEST_MODE            =  2,
    U1_ENABLE            = 48,
    U2_ENABLE            = 49,
    LTM_ENABLE           = 50,
} USB_STANDARD_DEVICE_FEATURE_SELECTOR, *PUSB_STANDARD_DEVICE_FEATURE_SELECTOR;


typedef union _HUB_DESCRIPTOR {
    USB_HUB_DESCRIPTOR             Hub20Descriptor;
    USB_30_HUB_DESCRIPTOR          Hub30Descriptor;
} HUB_DESCRIPTOR, *PHUB_DESCRIPTOR;


typedef union _USB20_HUB_CHAR_BITS {

    USHORT  AsUshort16;
    struct {
        USHORT  PortPowerSwitched:1;
        USHORT  Reserved_1:1;
        USHORT  IsCompoundDevice:1;
        USHORT  PerPortOvercurrent:1;
        USHORT  Reserved_4:1;
        USHORT  TT_ThinkTime:2;
        USHORT  PortIndicators:1;
        USHORT  Unused:8;
    };

} USB20_HUB_CHAR_BITS, *PUSB20_HUB_CHAR_BITS;

typedef union _USB30_HUB_CHAR_BITS {

    USHORT  AsUshort16;
    struct {
        USHORT  PortPowerSwitched:1;
        USHORT  Reserved_1:1;
        USHORT  IsCompundDevice:1;
        USHORT  OvercurrentProectionMode:2;
        USHORT  Unused:11;
    };

} USB30_HUB_CHAR_BITS, *PUSB30_HUB_CHAR_BITS;

//
// The time it takes to transmit a TP (20 symbol times) 
// at 5Gbps is 40ns
//
#define USB3_5GBPS_TP_LINK_DELAY_IN_NANO_SECONDS    40 
//
// Constants used in calculation of LPM latencies
//
#define HUB_PORT_2_PORT_EXIT_LATENCY_IN_NANOSECONDS         1000
#define T_HUB_DELAY_IN_NANOSECONDS                          400
#define FORWARDING_DELAY_DUE_TO_ONE_MAXIMUM_PACKET_SIZE_IN_NANOSECONDS   2100
#define T_PING_RESPONSE_IN_NANO_SECONDS                     400

