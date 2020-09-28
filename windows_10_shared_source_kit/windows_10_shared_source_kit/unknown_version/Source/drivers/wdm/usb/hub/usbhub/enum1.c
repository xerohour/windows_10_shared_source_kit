/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    enum1.c

     3-10-2002

Abstract:

   Contains the dispatch table for the PS_ENUMERATE_1 state

Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/


#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "enum1.tmh"
#endif

CCHAR usbfile_enum1_c[] = "enum1.c";

#define USBFILE usbfile_enum1_c


/*
#define CONNECT_CHANGE_BIT        0x0001
#define ENABLE_CHANGE_BIT         0x0002
#define SUSPEND_CHANGE_BIT        0x0004
#define OVERCURRENT_CHANGE_BIT    0x0008
#define RESET_CHANGE_BIT          0x0010
#define TIMEOUT_CHANGE_BIT        0x0020
#define INVALID_CHANGE_BIT        0x0040

===========================================================================

PS_ENUMERATE_1    -- wait for first reset
    CONNECT_CHANGE
        ceosp
        00001 --->UsbhReset1DropDevice
        10001 --->UsbhReset1Debounce
        11001 --->UsbhReset1DebounceError------------------>PS_ENUMERATE_1


    RESET_CHANGE
        ceosp
              **reset completed, port is enabled
        11001 --->UsbhReset1Complete----------------------->PS_ENUMERATE_2
              ** reset completed, device still connected but port not
              ** enabled
        10001 --->UsbhReset1CompleteNotEnabled------------->PS_WAIT_CONNECT
                                  retry-------------------->PS_ENUMERATE_1
              ** device dropped during reset, a connect change will
              follow but the PDO will be gone.
        00001 --->UsbhReset1DropDevice--------------------->PS_WAIT_CONNECT
              ** overcurrent dropped the device
        00101 --->UsbhReset1DropDevice--------------------->PS_WAIT_CONNECT
              ** port is suspended after reset; misbehaving HW
              ** we drop the device in the case
        10011 --->UsbhReset1DropDevice--------------------->PS_WAIT_CONNECT
        11011 --->UsbhReset1DropDevice--------------------->PS_WAIT_CONNECT
        01001 --->UsbhHardErrorReset1BadEnable------------->PS_HARDERROR
              ** overcurrent triggered after reset completes
                treat as reset failure 
        10101 --->UsbhReset1CompleteOvercurrent------------>PS_ENUMERATE_1
        
    ENABLE_CHANGE

        ceosp
              ** we let these timeout or progress to the next state
                 they can be side effects of reset failing or an unstable
                 connection.
        00001 --->UsbhReset1EnIgnore----------------------->PS_ENUMERATE_1
        10001 --->UsbhReset1EnIgnore----------------------->PS_ENUMERATE_1
        11001 --->UsbhReset1EnIgnore----------------------->PS_ENUMERATE_1

    OVERCURRENT_CHANGE
        ceosp

    SUSPEND_CHANGE
        ceosp

    TIMEOUT_CHANGE
        ceosp
              **reset attempt timed out
        10001 --->UsbhReset1Timeout------------------------->PS_WAIT_CONNECT
                                   retry-------------------->PS_ENUMERATE_1
              **device dropped off the bus during reset, this is normal for
              **a FS device connected to root of a USB 2.0 controller
        00001 --->UsbhReset1DropDevice---------------------->PS_WAIT_CONNECT
              **reset timed out but port is enabled
        11001 --->UsbhReset1TimeoutBadEnable---------------->PS_WAIT_CONNECT

============================================================================
*/

const HUB_EVENT_DISPATCH UsbhEN1_ConnectChange[32] = {
         0x00, NULL,
         0x01, UsbhReset1DropDevice,
         0x02, NULL,
         0x03, NULL,
         0x04, NULL,
         0x05, NULL,
         0x06, NULL,
         0x07, NULL,

         0x08, NULL,
         0x09, NULL,
         0x0a, NULL,
         0x0b, NULL,
         0x0c, NULL,
         0x0d, NULL,
         0x0e, NULL,
         0x0f, NULL,

         0x10, NULL,
         0x11, UsbhReset1Debounce,
         0x12, NULL,
         0x13, NULL,
         0x14, NULL,
         0x15, NULL,
         0x16, NULL,
         0x17, NULL,

         0x18, NULL,
         0x19, UsbhReset1DebounceError,
         0x1a, NULL,
         0x1b, NULL,
         0x1c, NULL,
         0x1d, NULL,
         0x1e, NULL,
         0x1f, NULL,
};


const HUB_EVENT_DISPATCH UsbhEN1_ResetChange[32] = {
         0x00, NULL,
         0x01, UsbhReset1DropDevice,
         0x02, NULL,
         0x03, NULL,
         0x04, NULL,
         0x05, UsbhReset1DropDevice,
         0x06, NULL,
         0x07, NULL,

         0x08, NULL,
         0x09, UsbhHardErrorReset1BadEnable,
         0x0a, NULL,
         0x0b, NULL,
         0x0c, NULL,
         0x0d, NULL,
         0x0e, NULL,
         0x0f, NULL,

         0x10, NULL,
         0x11, UsbhReset1CompleteNotEnabled,
         0x12, NULL,
         0x13, UsbhReset1DropDevice,
         0x14, NULL,
         0x15, UsbhReset1CompleteOvercurrent,
         0x16, NULL,
         0x17, NULL,

         0x18, NULL,
         0x19, UsbhReset1Complete,
         0x1a, NULL,
         0x1b, UsbhReset1DropDevice,
         0x1c, NULL,
         0x1d, NULL,
         0x1e, NULL,
         0x1f, NULL,
};


const HUB_EVENT_DISPATCH UsbhEN1_EnableChange[32] = {
         0x00, NULL,
         0x01, UsbhReset1EnIgnore,
         0x02, NULL,
         0x03, NULL,
         0x04, NULL,
         0x05, NULL,
         0x06, NULL,
         0x07, NULL,

         0x08, NULL,
         0x09, NULL,
         0x0a, NULL,
         0x0b, NULL,
         0x0c, NULL,
         0x0d, NULL,
         0x0e, NULL,
         0x0f, NULL,

         0x10, NULL,
         0x11, UsbhReset1EnIgnore,
         0x12, NULL,
         0x13, NULL,
         0x14, NULL,
         0x15, NULL,
         0x16, NULL,
         0x17, NULL,

         0x18, NULL,
         0x19, UsbhReset1EnIgnore,
         0x1a, NULL,
         0x1b, NULL,
         0x1c, NULL,
         0x1d, NULL,
         0x1e, NULL,
         0x1f, NULL,
};

/*
const HUB_EVENT_DISPATCH UsbhEN1_OvercurrentChange[32] = {
         0x00, NULL,
         0x01, NULL,
         0x02, NULL,
         0x03, NULL,
         0x04, NULL,
         0x05, NULL,
         0x06, NULL,
         0x07, NULL,

         0x08, NULL,
         0x09, NULL,
         0x0a, NULL,
         0x0b, NULL,
         0x0c, NULL,
         0x0d, NULL,
         0x0e, NULL,
         0x0f, NULL,

         0x10, NULL,
         0x11, NULL,
         0x12, NULL,
         0x13, NULL,
         0x14, NULL,
         0x15, NULL,
         0x16, NULL,
         0x17, NULL,

         0x18, NULL,
         0x19, NULL,
         0x1a, NULL,
         0x1b, NULL,
         0x1c, NULL,
         0x1d, NULL,
         0x1e, NULL,
         0x1f, NULL,
};
*/

const HUB_EVENT_DISPATCH UsbhEN1_SuspendChange[32] = {
         0x00, NULL,
         0x01, NULL,
         0x02, NULL,
         0x03, NULL,
         0x04, NULL,
         0x05, NULL,
         0x06, NULL,
         0x07, NULL,

         0x08, NULL,
         0x09, NULL,
         0x0a, NULL,
         0x0b, NULL,
         0x0c, NULL,
         0x0d, NULL,
         0x0e, NULL,
         0x0f, NULL,

         0x10, NULL,
         0x11, UsbhReset1EnIgnore, //jd
         0x12, NULL,
         0x13, NULL,
         0x14, NULL,
         0x15, NULL,
         0x16, NULL,
         0x17, NULL,

         0x18, NULL,
         0x19, NULL,
         0x1a, NULL,
         0x1b, NULL,
         0x1c, NULL,
         0x1d, NULL,
         0x1e, NULL,
         0x1f, NULL,
};


const HUB_EVENT_DISPATCH UsbhEN1_TimeoutChange[32] = {
         0x00, NULL,
         0x01, UsbhReset1DropDeviceTimeout,
         0x02, NULL,
         0x03, NULL,
         0x04, NULL,
         0x05, NULL,
         0x06, NULL,
         0x07, NULL,

         0x08, NULL,
         0x09, NULL,
         0x0a, NULL,
         0x0b, NULL,
         0x0c, NULL,
         0x0d, NULL,
         0x0e, NULL,
         0x0f, NULL,

         0x10, NULL,
         0x11, UsbhReset1Timeout,
         0x12, NULL,
         0x13, NULL,
         0x14, NULL,
         0x15, NULL,
         0x16, NULL,
         0x17, NULL,

         0x18, NULL,
         0x19, UsbhReset1TimeoutBadEnable,
         0x1a, NULL,
         0x1b, NULL,
         0x1c, NULL,
         0x1d, NULL,
         0x1e, NULL,
         0x1f, NULL,
};


VOID
UsbhEN1Trap(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    ULONG ChangeBits,
    UCHAR PortStatusBits,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Trap unhandled port events.  These are treated as non-fatal for now.  The default behavior ignore the change which 
    seems to work at least 50% of the time.

Irql: PASSIVE

Arguments:

Return Value:

    none


--*/
{
    UsbhKdPrint((0, ">Invalid Change Case: ENUMERATE_1\n"));
    UsbhKdPrint((0, ">ChangeBits: %08.8x PortStatusBits x%02.2x\n",
        ChangeBits, PortStatusBits));
    
    TEST_TRAP();
}



ULONG
UsbhEnumerate1(
    PDEVICE_OBJECT HubFdo,
    USB_HUB_PORT_EVENT Event,
    UCHAR PortStatusBits,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle the PS_ENUMERATE_1 state for a port

Irql: PASSIVE

Arguments:

Return Value:

    next state


--*/
{
    // default is to remain in the current state
    ULONG nextState = PortData->CurrentState;

    LOG(HubFdo, LOG_BUS, 'ENU1', PortData, Event.ul);

    switch (Event.ul) {
    case CONNECT_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'EN1c', PortStatusBits, 0);
        if (UsbhEN1_ConnectChange[PortStatusBits].Handler) {
            nextState =
               UsbhEN1_ConnectChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhEN1Trap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

    case RESET_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'EN1r', PortStatusBits, 0);
        if (UsbhEN1_ResetChange[PortStatusBits].Handler) {
            nextState =
                UsbhEN1_ResetChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhEN1Trap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

   case ENABLE_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'EN1e', PortStatusBits, 0);
        if (UsbhEN1_EnableChange[PortStatusBits].Handler) {
            nextState =
                UsbhEN1_EnableChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhEN1Trap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

   case SUSPEND_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'EN1s', PortStatusBits, 0);
        if (UsbhEN1_SuspendChange[PortStatusBits].Handler) {
            nextState =
                UsbhEN1_SuspendChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhEN1Trap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

   case TIMEOUT_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'EN1t', PortStatusBits, 0);
        if (UsbhEN1_TimeoutChange[PortStatusBits].Handler) {
            nextState =
                UsbhEN1_TimeoutChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhEN1Trap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

    case DRIVER_RESET_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'EN1x', PortStatusBits, 0);
        nextState = UsbhFailDriverResetPort(HubFdo, PortData, Pdo);
        break;

    case OVERCURRENT_CHANGE_BIT:
        // this is handled in the dispatch function
        LOG(HubFdo, LOG_BUS, 'EN1v', PortStatusBits, 0);
        nextState = UsbhHandleOvercurrent(HubFdo, PS_ENUMERATE_1, PortStatusBits, PortData, Pdo);

        break;

    case REQUEST_SUSPEND_BIT:
        // treat same as pause
        TEST_TRAP();
        // cancel any outstanding activity

        nextState = UsbhCancelEnumeration(HubFdo, PS_ENUMERATE_1, PortData, Pdo);
        UsbhSignalSuspendEvent(HubFdo, PortData, FALSE);
        break;

    case PAUSE_CHANGE_BIT:
        // port is spinning down.
        LOG(HubFdo, LOG_BUS, 'EN1p', PortStatusBits, 0);
        // cancel any outstanding activity

        nextState = UsbhCancelEnumeration(HubFdo, PS_ENUMERATE_1, PortData, Pdo);

        break;

    default:
        // multiple change bits are invalid
        UsbhEN1Trap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
    }

    return nextState;
}



