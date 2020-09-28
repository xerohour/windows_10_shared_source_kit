/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    enum2.c

     3-10-2002

Abstract:

   Contains the dispatch table for the PS_ENUMERATE_2 state

Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/


#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "enum2.tmh"
#endif

CCHAR usbfile_enum2_c[] = "enum2.c";

#define USBFILE usbfile_enum2_c


/*
#define CONNECT_CHANGE_BIT        0x0001
#define ENABLE_CHANGE_BIT         0x0002
#define SUSPEND_CHANGE_BIT        0x0004
#define OVERCURRENT_CHANGE_BIT    0x0008
#define RESET_CHANGE_BIT          0x0010
#define TIMEOUT_CHANGE_BIT        0x0020
#define INVALID_CHANGE_BIT        0x0040

===========================================================================

PS_ENUMERATE_2    -- wait for second reset
    CONNECT_CHANGE
        ceosp
        00001 -->UsbhReset2DropDevice---------------------->PS_ENUMERATE_2
            ** device probably dropped during reset, we treat this as a
               reset failure.  The reset will eventually time-out
        10001 -->UsbhReset2CycleDevice--------------------->PS_WAIT_CONNECT

    RESET_CHANGE
        ceosp
              **reset completed, port is enabled
        11001 --->UsbhReset2Complete----------------------->PS_WAIT_CONNECT
              **device dropped off the bus during a reset
        00001 --->UsbhReset2DropDevice--------------------->PS_ENUMERATE_2
              ** enable bit clear -- connection is unstable
                 the babble boy creates this condition
        10001 --->UsbhReset2ErrorWaitForTimeoutDevice------->PS_ENUMERATE_2


    ENABLE_CHANGE
        ceosp
        00001 -->UsbhReset2DropDevice---------------------->PS_ENUMERATE_2
            ** unstable connection
        10001 -->UsbhReset2ErrorWaitForTimeoutDevice------->PS_ENUMERATE_2

    OVERCURRENT_CHANGE
        ceosp

    SUSPEND_CHANGE
        ceosp

    TIMEOUT_CHANGE
        ceosp
        10001 -->UsbhReset2DeviceTimeout----------------->PS_ENUMERATE_1
        00001 -->UsbhReset2DropDeviceTimeout------------->WAIT_CONNECT

============================================================================
*/

const HUB_EVENT_DISPATCH UsbhEN2_ConnectChange[32] = {
         0x00, NULL,
         0x01, UsbhReset2DropDevice,
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
         0x11, UsbhReset2CycleDevice,
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


const HUB_EVENT_DISPATCH UsbhEN2_ResetChange[32] = {
         0x00, NULL,
         0x01, UsbhReset2DropDevice,
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
         0x11, UsbhReset2ErrorWaitForTimeoutDevice,
         0x12, NULL,
         0x13, NULL,
         0x14, NULL,
         0x15, NULL,
         0x16, NULL,
         0x17, NULL,

         0x18, NULL,
         0x19, UsbhReset2Complete,
         0x1a, NULL,
         0x1b, NULL,
         0x1c, NULL,
         0x1d, NULL,
         0x1e, NULL,
         0x1f, NULL,
};


const HUB_EVENT_DISPATCH UsbhEN2_EnableChange[32] = {
         0x00, NULL,
         0x01, UsbhReset2DropDevice,
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
         0x11, UsbhReset2ErrorWaitForTimeoutDevice,
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

/*
const HUB_EVENT_DISPATCH UsbhEN2_OvercurrentChange[32] = {
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

const HUB_EVENT_DISPATCH UsbhEN2_SuspendChange[32] = {
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


const HUB_EVENT_DISPATCH UsbhEN2_TimeoutChange[32] = {
         0x00, NULL,
         0x01, UsbhReset2DropDeviceTimeout,
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
         0x11, UsbhReset2Timeout,
         0x12, NULL,
         0x13, NULL,
         0x14, NULL,
         0x15, NULL,
         0x16, NULL,
         0x17, NULL,

         0x18, NULL,
         0x19, UsbhReset2TimeoutBadEnable,
         0x1a, NULL,
         0x1b, NULL,
         0x1c, NULL,
         0x1d, NULL,
         0x1e, NULL,
         0x1f, NULL,
};


VOID
UsbhEN2Trap(
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
    UsbhKdPrint((0, ">Invalid Change Case: ENUMERATE_2\n"));
    UsbhKdPrint((0, ">ChangeBits: %08.8x PortStatusBits x%02.2x\n",
        ChangeBits, PortStatusBits));

    TEST_TRAP();
   
}


ULONG
UsbhEnumerate2(
    PDEVICE_OBJECT HubFdo,
    USB_HUB_PORT_EVENT Event,
    UCHAR PortStatusBits,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle the PS_ENUMERATE_2 state for a port

Irql: PASSIVE

Arguments:

Return Value:

    next state


--*/
{
    // default is to remain in the current state
    ULONG nextState = PortData->CurrentState;

    LOG(HubFdo, LOG_BUS, 'ENU2', PortData, Event.ul);

    switch (Event.ul) {
    case CONNECT_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'EN2c', PortStatusBits, 0);
        if (UsbhEN2_ConnectChange[PortStatusBits].Handler) {
            nextState =
               UsbhEN2_ConnectChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhEN2Trap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

    case RESET_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'EN2r', PortStatusBits, 0);
        if (UsbhEN2_ResetChange[PortStatusBits].Handler) {
            nextState =
                UsbhEN2_ResetChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhEN2Trap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

    case ENABLE_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'EN2e', PortStatusBits, 0);
        if (UsbhEN2_EnableChange[PortStatusBits].Handler) {
            nextState =
                UsbhEN2_EnableChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhEN2Trap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

   case SUSPEND_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'EN2s', PortStatusBits, 0);
        if (UsbhEN2_SuspendChange[PortStatusBits].Handler) {
            nextState =
                UsbhEN2_SuspendChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhEN2Trap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

   case TIMEOUT_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'EN2t', PortStatusBits, 0);
        if (UsbhEN2_TimeoutChange[PortStatusBits].Handler) {
            nextState =
                UsbhEN2_TimeoutChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhEN2Trap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

   case OVERCURRENT_CHANGE_BIT:
        // this is handled in the dispatch function
        LOG(HubFdo, LOG_BUS, 'EN2v', PortStatusBits, 0);
        nextState = UsbhHandleOvercurrent(HubFdo, PS_ENUMERATE_2, PortStatusBits, PortData, Pdo);
        break;

    case DRIVER_RESET_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'EN2x', PortStatusBits, 0);
        nextState = UsbhFailDriverResetPort(HubFdo, PortData, Pdo);
        break;

    case REQUEST_SUSPEND_BIT:
        // treat same as pause
        TEST_TRAP();
        // cancel any outstanding activity

        nextState = UsbhCancelEnumeration(HubFdo, PS_ENUMERATE_2, PortData, Pdo);
        UsbhSignalSuspendEvent(HubFdo, PortData, FALSE);
        break;


    case PAUSE_CHANGE_BIT:
        // port is spinning down.
        LOG(HubFdo, LOG_BUS, 'EN2p', PortStatusBits, 0);
        // cancel any outstanding activity

        UsbhAssertPassive();

        nextState = UsbhCancelEnumeration(HubFdo, PS_ENUMERATE_2, PortData, Pdo);

        break;

    default:
        // multiple change bits are invalid
        UsbhEN2Trap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
    }

    return nextState;
}



