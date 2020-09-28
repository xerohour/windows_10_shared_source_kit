/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    usbhub.h

Abstract:

    Standard hub definitions from the core specification

Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/

#ifndef   __USBHUB_H__
#define   __USBHUB_H__

typedef union _USB_HUB_CLEAR_TT_BUFFER_VALUE {

    USHORT                  us;
    struct {
        USHORT   EndpointNumber:4;     // 0 - 3
        USHORT   DeviceAddress:7;      // 4 - 10
        USHORT   EndpointType:2;       // 11 - 12
        USHORT   Reserved_MBZ:2;       // 13-14

        USHORT   Direction_1forIN:1;   // 15
    };

} USB_HUB_CLEAR_TT_BUFFER_VALUE, *PUSB_HUB_CLEAR_TT_BUFFER_VALUE;

C_ASSERT((sizeof(USB_HUB_CLEAR_TT_BUFFER_VALUE) == sizeof(USHORT)));



/*
    Standard hub feature selectors
    USB 2.0  - 11.24.2
*/

// hub
#define C_HUB_LOCAL_POWER   0
#define C_HUB_OVER_CURRENT  1

// ports
#define PORT_CONNECTION     0
#define PORT_ENABLE         1
#define PORT_SUSPEND        2
#define PORT_OVER_CURRENT   3
#define PORT_RESET          4
#define PORT_POWER          8
#define PORT_LOW_SPEED      9
#define C_PORT_CONNECT      16
#define C_PORT_ENABLE       17
#define C_PORT_SUSPEND      18
#define C_PORT_OVER_CURRENT 19
#define C_PORT_RESET        20
#define PORT_INDICATOR      22


// 0 Color set automatically, as defined in Table 11-6
#define PortLED_Auto    0x0000
#define PortLED_Amber   0x0100
#define PortLED_Green   0x0200
#define PortLED_Off     0x0300


#define VALID_HW_CHANGE_BITS         0x001F
#define UNUSED_HW_CHANGE_BITS        0xFFE0

/*
    // hardware port status definitions -- these are what the hub returns
    // they can be defined only by the USB specification.
    Port change and status USB 2.0 - 11.24.2.7
*/

typedef union _USB_HUB_HW_PORT_CHANGE {

    USHORT                  us;
    struct {
        USHORT   ConnectChange:1;     // 0
        USHORT   EnableChange:1;      // 1
        USHORT   SuspendChange:1;     // 2
        USHORT   OvercurrentChange:1; // 3

        USHORT   ResetChange:1;       // 4
        USHORT   Reserved_do_not_use:11;// 5-15
    };

} USB_HUB_HW_PORT_CHANGE, *PUSB_HUB_HW_PORT_CHANGE;

C_ASSERT((sizeof(USB_HUB_HW_PORT_CHANGE) == sizeof(USHORT)));

typedef union _USB_HUB_HW_PORT_STATUS {

    USHORT                  us;
    struct {
        USHORT   Connect:1;     // 0
        USHORT   Enable:1;      // 1
        USHORT   Suspend:1;     // 2
        USHORT   Overcurrent:1; // 3
        USHORT   Reset:1;       // 4
        USHORT   Reserved1:3;   // 5-7
        USHORT   Power:1;       // 8
        USHORT   LowSpeed:1;    // 9
        USHORT   HighSpeed:1;   // 10
        USHORT   Reserved2:5;   // 11-15
    };

} USB_HUB_HW_PORT_STATUS, *PUSB_HUB_HW_PORT_STATUS;

C_ASSERT((sizeof(USB_HUB_HW_PORT_STATUS) == sizeof(USHORT)));


typedef struct _USB_HUB_PORT_STATE {

    USB_HUB_HW_PORT_STATUS Status;
    USB_HUB_HW_PORT_CHANGE Change;

} USB_HUB_PORT_STATE, *PUSB_HUB_PORT_STATE;

C_ASSERT((sizeof(USB_HUB_PORT_STATE) == sizeof(ULONG)));

typedef struct _USB_HUB_STATE {

    USB_HUB_STATUS Status;
    USB_HUB_CHANGE Change;

} USB_HUB_STATE, *PUSB_HUB_STATE;

C_ASSERT((sizeof(USB_HUB_STATE) == sizeof(ULONG)));


typedef struct _USB_HW_STATE {

    USHORT Status;
    USHORT Change;

} USB_HW_STATE, *PUSB_HW_STATE;

C_ASSERT((sizeof(USB_HW_STATE) == sizeof(ULONG)));


/* Includes Microsoft Extensions*/

// note: this is subject to change, these attributes are being defined
// in the ACPI namespace but they could also be reported by an external hub

typedef union _USB_EXTENDED_PORT_ATTRIBUTES {

    ULONG                  ul;
    struct {
        ULONG   NoConnector:1;     // 0
        ULONG   SharedUsb2:1;
        ULONG   MiniA_Connector:1;
        ULONG   Oem_Connector:1;

        ULONG   DeviceNotRemovable:1;
        ULONG   AcpiUpcValid:1;
        ULONG   AcpiPldValid:1;
        ULONG   Reserved:25;
    };

} USB_EXTENDED_PORT_ATTRIBUTES, *PUSB_EXTENDED_PORT_ATTRIBUTES;

C_ASSERT((sizeof(USB_EXTENDED_PORT_ATTRIBUTES) == sizeof(ULONG)));

#define USB_PORTATTR_NO_CONNECTOR_BIT       0x00000001
#define USB_PORTATTR_SHARED_USB2_BIT        0x00000002
#define USB_PORTATTR_MINI_CONNECTOR_BIT     0x00000004
#define USB_PORTATTR_OEM_CONNECTOR_BIT      0x00000008
#define USB_PORTATTR_NOT_REMOVABLE_BIT      0x00000010
#define USB_PORTATTR_ACPI_UPC_VALID_BIT     0x00000020
#define USB_PORTATTR_ACPI_PLD_VALID_BIT     0x00000040


/* dynamic attributes */
#define USB_PORTATTR_OWNED_BY_CC_BIT        0x01000000
#define USB_PORTATTR_NO_OVERCURRENT_UI_BIT  0x02000000

#define INITIAL_SS_TIMEOUT_FOR_HUBS_IN_MS       2000
#define DEFAULT_SS_TIMEOUT_FOR_HUBS_IN_MS       25

#endif //__USBHUB_H__

