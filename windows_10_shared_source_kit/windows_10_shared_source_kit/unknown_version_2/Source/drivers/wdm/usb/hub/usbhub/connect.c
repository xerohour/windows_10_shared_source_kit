/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    connect.c

     3-10-2002

Abstract:

   Contains the dispatch table for the PS_WAIT_CONNECT state

Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/


#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "connect.tmh"
#endif

CCHAR usbfile_connect_c[] = "connect.c";

#define USBFILE usbfile_connect_c


/*
#define CONNECT_CHANGE_BIT        0x0001
#define ENABLE_CHANGE_BIT         0x0002
#define SUSPEND_CHANGE_BIT        0x0004
#define OVERCURRENT_CHANGE_BIT    0x0008
#define RESET_CHANGE_BIT          0x0010
#define TIMEOUT_CHANGE_BIT        0x0020
#define DRIVER_RESET_CHANGE       0x0040



===========================================================================

PS_WAIT_CONNECT    -- waiting for connect/disconnect
    CONNECT_CHANGE
        ceosp
            **device disappeared
        00000 --->UsbhPortIgnoreChange-------------------->PS_WAIT_CONNECT
            **device physically disconnected
        00001 --->UsbhPortDisconnect---------------------->PS_WAIT_CONNECT
            **new device connected
        10000 --->UsbhPortConnect------------------------->PS_ENUMERATE_1
            **device connected,disconnected rapidly (Percy test)
            ** or device driver cycles the port
        10001 --->UsbhPortCycle--------------------------->PS_ENUMERATE_1
        invalid ------------------------------------------>PS_WAIT_CONNECT
      

    RESET_CHANGE
            ** device removed before reset completed
        00000 --->UsbhPortIgnoreChange-------------------->PS_WAIT_CONNECT
        10001 --->UsbhHardErrorInvalidData---------------->PS_HARDERROR
        10001 --->UsbhHardErrorInvalidData---------------->PS_HARDERROR
        10000 --->UsbhPortConnect------------------------->PS_WAIT_CONNECT
        invalid ------------------------------------------>PS_WAIT_CONNECT
             ** reset completed after we droped the device, can happen if 
                reset process takes incredibly long.
        11000 --->UsbhPortRecycle--------------------------->PS_WAIT_CONNECT
    ENABLE_CHANGE
        ceosp
            **HC disable port, frame babble -- this is a HW error
            ** this also occurs if we initiate the disable in response to
            either some type of error or a soft remove of the hub.
        10001 -->UsbhPortDisable-------------------------->PS_WAIT_CONNECT
            **if PDO is zero this is a stale change which can ignore. There is
            ** no danger since the enable bit shows clear.
        10000 -->UsbhPortIgnoreChange--------------------->PS_WAIT_CONNECT
        invalid ------------------------------------------>PS_WAIT_CONNECT

    OVERCURRENT_CHANGE
        ceosp
            **device generated overcurrent
        10101 --->UsbhPdoOvercurrent---------------------->PS_WAIT_CONNECT
            **spontaneous overcurrent
        00100 --- TBD ----->PS_WAIT_CONNECT
            **overcurrent before enumeration
        10100 --- TBD ----->PS_WAIT_CONNECT
        invalid ------------------------------------------>PS_WAIT_CONNECT

    SUSPEND_CHANGE
        ceosp
            **port resume completed
        11001 --->UsbhPortResume-------------------------->PS_WAIT_CONNECT

            ** invalid bits, root hub was removed, wait for timeout
            **Test #2004
        11111 --->UsbhPortWaitForResumeTimeout
            ** connect change (enable bit clears) allow resume to timeout
            **Test #2005
            **Test #2007
        10001 --->UsbhPortWaitForResumeTimeout
            ** failure to clear suspend bit, wait for timeout
            ** Test #2006
        10011 --->UsbhPortWaitForResumeTimeout
            ** device disconnected during resume
        00001 --->UsbhPortWaitForResumeTimeout

    TIMEOUT_CHANGE
        ceosp
            ** device removed before timeout completed
        00000 --->UsbhPortIgnoreChange-------------------->PS_WAIT_CONNECT

    DRIVER_RESET_CHANGE
        ceosp
            ** device driver is requesting a reset for this USB device.
            * device controller or hhub cleared enable -- maybe due to babble
        10001 --->UsbhDriverResetPort--------------------->PS_WAIT_CONNECT
            * device stopped functioning
        11001 --->UsbhDriverResetPort--------------------->PS_WAIT_CONNECT
            * these are nonsense or invalid cases.
                we see 11111 if the root hub/HC is removed.
            * We fail this request to reset in these cases.
        00001 --->UsbhFailDriverResetPort----------------->PS_WAIT_CONNECT
        00011
        00101
        00111
        01001
        01011
        01101
        01111
        10101
        10111
        11011
        11101
        11111
            * Driver reset while port is supended.  This is sent by the power
                code to recover a device that does not resume.  Technically
                we are not supposed to drive reset on a suspended port but
                the device is messed up anyway.
        10011 --->UsbhDriverResetPort--------------------->PS_WAIT_CONNECT
============================================================================
*/

const HUB_EVENT_DISPATCH UsbhWC_ConnectChange[32] = {
         0x00, UsbhPortIgnoreChange,
         0x01, UsbhPortDisconnect,
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

         0x10, UsbhPortConnect,
         0x11, UsbhPortCycle,
         0x12, NULL,
         0x13, NULL,
         0x14, NULL,
         0x15, NULL,
         0x16, NULL,
         0x17, NULL,

         0x18, UsbhPortRecycle,
         0x19, NULL,
         0x1a, NULL,
         0x1b, NULL,
         0x1c, NULL,
         0x1d, NULL,
         0x1e, NULL,
         0x1f, NULL,
};


const HUB_EVENT_DISPATCH UsbhWC_ResetChange[32] = {
         0x00, UsbhPortIgnoreChange,
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

         0x10, UsbhPortConnect,
         0x11, UsbhHardErrorInvalidData,
         0x12, NULL,
         0x13, NULL,
         0x14, NULL,
         0x15, NULL,
         0x16, NULL,
         0x17, NULL,

         0x18, UsbhPortRecycle,
         0x19, UsbhHardErrorInvalidData,
         0x1a, NULL,
         0x1b, NULL,
         0x1c, NULL,
         0x1d, NULL,
         0x1e, NULL,
         0x1f, NULL,
};


const HUB_EVENT_DISPATCH UsbhWC_EnableChange[32] = {
         0x00, UsbhPortIgnoreChange,
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

         0x10, UsbhPortIgnoreChange,
         0x11, UsbhPortDisable,
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
const HUB_EVENT_DISPATCH UsbhWC_OvercurrentChange[32] = {
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
         0x15, UsbhPortOvercurrent,
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

const HUB_EVENT_DISPATCH UsbhWC_SuspendChange[32] = {
         0x00, NULL,
         0x01, UsbhPortWaitForResumeTimeout,
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
         0x11, UsbhPortWaitForResumeTimeout,
         0x12, NULL,
         0x13, UsbhPortWaitForResumeTimeout,
         0x14, NULL,
         0x15, NULL,
         0x16, NULL,
         0x17, NULL,

         0x18, NULL,
         0x19, UsbhPortResumeComplete,
         0x1a, NULL,
         0x1b, UsbhPortResumeFailed,
         0x1c, NULL,
         0x1d, NULL,
         0x1e, NULL,
         0x1f, UsbhPortWaitForResumeTimeout,
};


const HUB_EVENT_DISPATCH UsbhWC_TimeoutChange[32] = {
         0x00, UsbhPortIgnoreChange,
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

const HUB_EVENT_DISPATCH UsbhWC_DriverResetChange[32] = {
        //*note1 - denotes no PDO present, this case is bypassed by a check
        // in UsbhHubDispatchPortEvent which validates that the pdo
        // tied to the reset request is still the current PDO
         0x00, NULL,
         0x01, UsbhFailDriverResetPort, // this is seen if the drops just aftet the reset is issued
         0x02, NULL, // note1
         0x03, UsbhFailDriverResetPort,
         0x04, NULL, // note1
         0x05, UsbhFailDriverResetPort,
         0x06, NULL, // note1
         0x07, UsbhFailDriverResetPort,

         0x08, NULL, // note1
         0x09, UsbhFailDriverResetPort,
         0x0a, NULL, // note1
         0x0b, UsbhFailDriverResetPort,
         0x0c, NULL, // note1
         0x0d, UsbhFailDriverResetPort,
         0x0e, NULL, // note1
         0x0f, UsbhFailDriverResetPort,

         0x10, NULL, // note1
         0x11, UsbhDriverResetPort,
         0x12, NULL, // note1
         0x13, UsbhDriverResetPort,
         0x14, NULL, // note1
         0x15, UsbhFailDriverResetPort,
         0x16, NULL, // note1
         0x17, UsbhFailDriverResetPort,

         0x18, NULL, // note1
         0x19, UsbhDriverResetPort,
         0x1a, NULL, // note1
         0x1b, UsbhFailDriverResetPort,
         0x1c, NULL, // note1
         0x1d, UsbhFailDriverResetPort,
         0x1e, NULL, // note1
         0x1f, UsbhFailDriverResetPort,
};



VOID
UsbhWCTrap(
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
    UsbhKdPrint((0, ">Invalid Change Case: WAIT_CONNECT\n"));
    UsbhKdPrint((0, ">ChangeBits: %08.8x PortStatusBits x%02.2x\n",
        ChangeBits, PortStatusBits));
    UsbhKdPrint((0, ">PortData: %p \n", PortData));
    UsbhKdPrint((0, ">Pdo: %p \n", Pdo));
    TEST_TRAP();
}


ULONG
UsbhWaitConnect(
    PDEVICE_OBJECT HubFdo,
    USB_HUB_PORT_EVENT Event,
    UCHAR PortStatusBits,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Handle the PS_WAIT_CONNECT state for a port

Irql: PASSIVE

Arguments:

Return Value:

    next state


--*/
{
    // default is to remain in the current state
    ULONG nextState = PortData->CurrentState;

    LOG(HubFdo, LOG_BUS, 'CONN', PortData, Event.ul);
    DbgTrace((UsbhDebugTrace, "#%d CONNECT portStatusBits status_bits:0x%1.1x change:0x%4.4x\n",
        PN(PortData),
        PortStatusBits,
        Event.ul));

    switch (Event.ul) {
    case REQUEST_SUSPEND_BIT:

        LOG(HubFdo, LOG_BUS, 'CONS', PortStatusBits, 0);

        nextState = UsbhRequestPortSuspend(HubFdo,
                                           Event,
                                           PortStatusBits,
                                           PortData,
                                           Pdo);
        break;

    case CONNECT_CHANGE_BIT:

        LOG(HubFdo, LOG_BUS, 'CONc', PortStatusBits, 0);

        if (UsbhWC_ConnectChange[PortStatusBits].Handler) {
            nextState =
               UsbhWC_ConnectChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhWCTrap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

    case RESET_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'CONr', PortStatusBits, 0);
        if (UsbhWC_ResetChange[PortStatusBits].Handler) {
            nextState =
                UsbhWC_ResetChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhWCTrap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

   case SUSPEND_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'CONs', PortStatusBits, 0);
        if (UsbhWC_SuspendChange[PortStatusBits].Handler) {
            nextState =
                UsbhWC_SuspendChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhWCTrap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

   case TIMEOUT_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'CONt', PortStatusBits, 0);
        if (UsbhWC_TimeoutChange[PortStatusBits].Handler) {
            nextState =
                UsbhWC_TimeoutChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhWCTrap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

   case ENABLE_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'CONe', PortStatusBits, 0);
        if (UsbhWC_EnableChange[PortStatusBits].Handler) {
            nextState =
                UsbhWC_EnableChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhWCTrap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

    case OVERCURRENT_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'CONv', PortStatusBits, 0);
        nextState = UsbhHandleOvercurrent(HubFdo, PS_WAIT_CONNECT, PortStatusBits, PortData, Pdo);

        break;

   case DRIVER_RESET_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'CONx', PortStatusBits, 0);
        if (UsbhWC_DriverResetChange[PortStatusBits].Handler) {
            nextState =
                UsbhWC_DriverResetChange[PortStatusBits].Handler(HubFdo, PortData, Pdo);
        } else {
            UsbhWCTrap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
        }
        break;

    
    case PAUSE_CHANGE_BIT:
        LOG(HubFdo, LOG_BUS, 'CONp', PortStatusBits, 0);
        
        nextState = UsbhWcCancelEnumeration(HubFdo, PortData, Pdo);
        break;

    case REQUEST_RESUME_BIT:
        LOG(HubFdo, LOG_BUS, 'CONm', PortStatusBits, 0);
        
        
                   
        
        UsbhSignalResumeEvent(HubFdo, PortData, FALSE);            
        //nextState = PS_WAIT_CONNECT;
        break;

    default:
        // multiple change bits are invalid
        UsbhWCTrap(HubFdo, PortData, Event.ul, PortStatusBits, Pdo);
    }

    return nextState;
}



