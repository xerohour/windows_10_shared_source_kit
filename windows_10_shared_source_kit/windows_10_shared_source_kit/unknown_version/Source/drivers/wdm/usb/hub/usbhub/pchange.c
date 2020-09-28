/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    pchange.c

     12-25-2003

Abstract:

   Port Change (pcq) state machine:

The state machine below applies to the queue only. The actual status changes are dispatched to
the Port_Event state machine.

Port Change Queue is instanced once for each port on a hub.  The change queue holds the
notifications 'events' that originate from both hardware and software for the hubs ports.
These are things like connect and disconnect etc.


Locks:
      PortChangeListSpin - one lock for each queue to serialize state transitions.

Wait Event:
      PortChangeListDone -

States:


Pcq_Error
Used for tracking tracing unhandled error paths

Pcq_Run_Idle 
When queue is enabled and empty.

Pcq_Pause  
State prior to run, messages can be queued but will not serviced until the pcq is started.
                
Pcq_Run_wQueued 
Worker successfully queued.

Pcq_Run_wBusy 
pcq is actively processing requests.

*Pcq_Run_wBusy_Signalled 
queue was signalled while pcq is actively processing requests.

Pcq_Stop_wQueued, Pcq_Stop_wBusy,  Pcq_Stop_wBusy_Signal
requesting worker to finish and queue to empty.
      
Pcq_Stop 
queue is halted, no new requests can be queued. **Port may or may not be suspended.  This is state is used to 
temporarily halt the queue for things like power management.
PortChangeListDone is signalled ->Pcq_Stop
                                
Pcq_PortSuspended 
port has been placed in the 'USB' suspend state. This is used to indiacte a port is 'selectively' suspended.
PortChangeListDone is signalled ->Pcq_PortSuspended

Pcq_Stop_Suspended 
If all the bus enters 'global' suspend then all the selective suspended ports on the hub will be into 
the 'stop_suspended' state.

*Pcq_HW_Stop_Wait
*Pcq_HW_Stop
queue is halted due to hardware failure or disconnect, no new requests can be queued. Queue can not be re-started 
from this state.
--------------------------------------------------------------------------------------------------------------------
--------------------------------------------------------------------------------------------------------------------
Events: (PCE_)

...................................................................................................................

PCE_ResumeDone  (Usbh_PCE_ResumeDone_Action)

This event signals that a thread waiting to suspend may continue.  It is used to resolve the case 
where a suspend request collides with a resume request.

...................................................................................................................
PCE_Enable  (Usbh_PCE_Enable_Action)
starts the pcq processing messages.

...................................................................................................................
PCE_wChange  (Usbh_PCE_wChange_Action)
called to de-queue a message from the hardware chnage queue.

...................................................................................................................
PCE_Disable (Usbh_PCE_Disable_Action)
stops the pcq processing messages. Attempts to put the pcq in Pcq_Stop State.

...................................................................................................................
PCE_Resume
used to transition out of Pcq_Stop or Pcq_PortSuspended -- will issue a USB resume request on the port if it is 
Pcq_PortSuspended state.

...................................................................................................................
PCE_QueueChange (Usbh_PCE_QueueChange_Action)
inserts a port status change event to the pcq.  These are hardware events like reset, connect or suspend changes 

...................................................................................................................
PCE_wRun (Usbh_PCE_wRun_Action)
worker runtine begins, triggers the pcq to dispatch its queued messages.
                                
...................................................................................................................
PCE_wDone (Usbh_PCE_wDone_Action)
worker queue routine ends, typically sent when the pcq goes empty.

...................................................................................................................
PCE_SoftDisconnect 
sent to indicate that software (AKA pnp) needs to be disconnected from the port.  This breaks the PnP association 
with the physical device connected to the port if one exits.

...................................................................................................................
PCE_BusReset 
Sent to reset the queues to the pause state, used after power management and hardware reset

...................................................................................................................
PCE_Suspend (Usbh_PCE_Suspend_Action)
Queues a hardware request to suspend the port.  This routine waits for outstanding reset and resume requests to 
complete before attempting the suspend

...................................................................................................................
PCE_BusDisconnect 
sent when the entire hub needs to disconnect the devices due to error or because the hub was removed.

...................................................................................................................
Event irql:

   PCE_Enable         ANY
   PCE_Disable        PASSIVE
   PCE_Resume         ANY
   PCE_QueueChange    ANY
   PCE_wRun           ANY
   PCE_wDone          ANY
   PCE_wBusy          ANY
   PCE_SoftDisconnect PASSIVE
   PCE_BusReset       ANY
   PCE_Suspend        PASSIVE
   PCE_BusDisconnect  ANY,
   PCE_HardwareWake   ANY
   
...................................................................................................................
*******************************************************************************************************************
PortChangeListDone
EVENT
-------------------------------------------------------------------------------------------------------------------
*RESET

event                   current state               next state
PCE_Resume              Pcq_Stop_Suspended          Pcq_Wait_Resume  
                        Pcq_PortSuspended           Pcq_Wait_Resume
                        Pcq_Stop                    Pcq_Pause

PCE_Enable              Pcq_Wait_Resume             Pcq_Run_wQueued, Pcq_Run_Idle
                        Pcq_Pause                   Pcq_Run_wQueued, Pcq_Run_Idle     

PCE_SD_Resume           Pcq_Pause                   Pcq_Run_Idle
                        Pcq_Stop                    Pcq_Run_Idle

PCE_HardwareWake        Pcq_PortSuspended           Pcq_Run_Idle

-------------------------------------------------------------------------------------------------------------------
*SIGNAL

event                   current state               next state
PCE_Disable             Pcq_PortSuspended           Pcq_Stop_Suspended    

                        Pcq_Wait_Resume             Pcq_Stop
                        Pcq_Pause                   Pcq_Stop
                        Pcq_Run_Idle                Pcq_Stop
                        
                        Pcq_HW_Stop                 Pcq_HW_Stop 
                        Pcq_Stop                    Pcq_Stop
                        Pcq_Stop_Suspended          Pcq_Stop_Suspended    

PCE_wDone               Pcq_Suspend_wBusy           Pcq_PortSuspended
                        Pcq_Stop_wBusy_Signal       Pcq_Stop
                        Pcq_Stop_wBusy              Pcq_Stop
                        Pcq_HW_Stop_wBusy           Pcq_HW_Stop

PCE_HW_Stop
                        Pcq_Stop_Suspended          Pcq_HW_Stop
                        Pcq_Stop                    "
                        Pcq_Run_wBusy_Signal        "
                        Pcq_Stop_wBusy_Signal       "
                        Pcq_Wait_Resume             "
                        Pcq_Pause                   "
                        Pcq_PortSuspended           "
                        Pcq_Run_Idle                "      
-------------------------------------------------------------------------------------------------------------------
*WAIT    

PCE_Disable             waits in all cases

...................................................................................................................
*******************************************************************************************************************
PortResumeEvent
EVENT

-------------------------------------------------------------------------------------------------------------------
*RESET

event                   current state               next state
PCE_Resume              Pcq_Stop_Suspended          Pcq_Wait_Resume
                        Pcq_PortSuspended           Pcq_Wait_Resume
                        Pcq_Stop                    Pcq_Pause
                        
-------------------------------------------------------------------------------------------------------------------
*SIGNAL

event                   current state               next state
PCE_Resume              Pcq_Pause                   Pcq_Pause  
                        Pcq_Run_wQueued             Pcq_Run_wQueued
                        Pcq_Run_Idle                Pcq_Run_Idle    
                        Pcq_Run_wBusy               Pcq_Run_wBusy
                        Pcq_Run_wBusy_Signal        Pcq_Run_wBusy_Signal
                        Pcq_Closed                  Pcq_Closed         
                        Pcq_HW_Stop                 Pcq_HW_Stop  

PCE_ResumeTimeout       signalled in all states except
                            Pcq_Pause
                            Pcq_Run_wBusy
                            Pcq_Run_wQueued
                            
PCE_BusReset             Pcq_Stop_Suspended         Pcq_Pause
                         Pcq_PortSuspended          Pcq_Pause
                         Pcq_Stop                   Pcq_Pause
                            

Also signalled whenever a SUSPEND_CHANGE_BIT change event is freed (UsbhFreePortChangeQueueObject). 
                        
-------------------------------------------------------------------------------------------------------------------
*WAIT    

event                   current state               next state
                        
...................................................................................................................
*******************************************************************************************************************

...................................................................................................................                        
...................................................................................................................
PortSuspendEvent
EVENT

-------------------------------------------------------------------------------------------------------------------
*RESET

event                   current state               next state
PCE_Suspend             all

-------------------------------------------------------------------------------------------------------------------
*SIGNAL
UsbhSignalSuspendEvent

event                   current state               next state
PCE_Resume              Pcq_Pause                   Pcq_Pause  

PCE_wChange 
    Enum1.REQUEST_SUSPEND_BIT
    Enum2.REQUEST_SUSPEND_BIT
    HubErr_PowerBit_Dropped

PCE_Suspend             Pcq_Stop                    Pcq_Stop_Suspended
                        Pcq_HW_Stop                 Pcq_HW_Stop    




-------------------------------------------------------------------------------------------------------------------
*WAIT    

event                   current state               next state
                        
...................................................................................................................
...................................................................................................................


Event generating functions:
    Usbh<EventName>

Event Action Functions:
    Usbh_<EventName>_Action

Flush = FlushPortChange
InsertQCH = InsertQueueChange
PS=0 = ResetEvent(PortSuspend)
PS=1 = SetEvent(PortSuspend)
WaitPS  WaitEvent(PortSuspend)

CQ=0 = ResetEvent(ChangeQueueDone)
CQ=1 = SetEvent(ChangeQueueDone)
WaitCQ  WaitEvent(ChangeQueueDone)

PR=0 = ResetEvent(PortResume)
PR=1 = SetEvent(PortResume)
WaitPR  WaitEvent(PortResume)

Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/

#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "pchange.tmh"
#endif

CCHAR usbfile_pchange_c[] = "pchange.c";

#define USBFILE usbfile_pchange_c


//#define PCQ_TRAP() NT_ASSERT(FALSE) 
#define PCQ_TRAP()

#define UsbhAssertPcqLock(fdo, pd) UsbhAssert((fdo), (pd)->PortChangeListLock == 1)

ULONG
UsbhWaitConnect(
    PDEVICE_OBJECT HubFdo,
    USB_HUB_PORT_EVENT Event,
    UCHAR PortStatusBits,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhEnumerate1(
    PDEVICE_OBJECT HubFdo,
    USB_HUB_PORT_EVENT Event,
    UCHAR PortStatusBits,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

ULONG
UsbhEnumerate2(
    PDEVICE_OBJECT HubFdo,
    USB_HUB_PORT_EVENT Event,
    UCHAR PortStatusBits,
    PHUB_PORT_DATA PortData,
    PDEVICE_OBJECT Pdo
    );

VOID
UsbhUnlockPcqWithTag(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    KIRQL Irql,
    ULONG Tag
    )
{
    PG_STATE_LOG_ENTRY pcqLog;

    UsbhAssert(HubFdo, PortData->PortChangeListLock);
    LOG(HubFdo, LOG_POWER, 'pq--', PortData->Pcq_State, PN(PortData));
    PortData->PortChangeListLock = 0;

    pcqLog = &PortData->PcqLog[PortData->PcqLogIdx];
    pcqLog->NextState = PortData->Pcq_State;
    if (Tag) {
        pcqLog->Tag = Tag;
    }        
    
    KeReleaseSpinLock(&PortData->PortChangeListSpin, Irql);
}


VOID
UsbhUnlockPcq(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    KIRQL Irql
    )
{
    UsbhUnlockPcqWithTag(HubFdo, PortData, Irql, 0);
}


HUB_PCQ_STATE
UsbhLockPcqWithTag(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    HUB_PCQ_EVENT PcqEvent,
    KIRQL *Irql,
    ULONG Tag
    )
{
    PG_STATE_LOG_ENTRY pcqLog;

    KeAcquireSpinLock(&PortData->PortChangeListSpin, Irql);
    UsbhAssert(HubFdo, PortData->PortChangeListLock == 0);
    PortData->PortChangeListLock = 1;

    LOG(HubFdo, LOG_POWER, 'pq++', PcqEvent, PN(PortData));
    LOG(HubFdo, LOG_POWER, 'pq++', PortData->Pcq_State, PN(PortData));
    PortData->PcqLogIdx=(PortData->PcqLogIdx+1) % G_NUMPCQ_STATE_LOG_ENTRIES;
    pcqLog = &PortData->PcqLog[PortData->PcqLogIdx];

    pcqLog->Event = PcqEvent;
    pcqLog->CurrentState = PortData->Pcq_State;
    pcqLog->NextState = 0xffffffff;
    pcqLog->Tag = Tag;
    pcqLog->PcqEv_Suspend = 0;
    pcqLog->PcqEv_Resume = 0;
    pcqLog->PcqEv_ChDone = 0;
    pcqLog->PcqEv_DriverReset = 0;
      
    UsbhAssert(HubFdo, PcqEvent);
    UsbhAssert(HubFdo, PortData->Pcq_State);
    // return the locked in state
    return PortData->Pcq_State;
}


HUB_PCQ_STATE
UsbhLockPcq(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    HUB_PCQ_EVENT PcqEvent,
    KIRQL *Irql
    )
{
    return UsbhLockPcqWithTag(HubFdo, PortData, PcqEvent, Irql, 0);
}


VOID
UsbhSetPcqEventStatus(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PCQEV_FIELD EventField,
    PCQEV_STATUS EventStatus
    )
/*
    update the event status for the current log entry
*/
{
    PG_STATE_LOG_ENTRY pcqLog;

    UsbhAssert(HubFdo, PortData->PortChangeListLock);
    LOG(HubFdo, LOG_POWER, 'pqev', PortData->Pcq_State, PN(PortData));
   
    pcqLog = &PortData->PcqLog[PortData->PcqLogIdx];

    switch (EventField) {
    case PcqEvSuspend:
        pcqLog->PcqEv_Suspend = EventStatus;
        break;
    case PcqEvResume:
        pcqLog->PcqEv_Resume = EventStatus;
        break;
    case PcqEvChDone:
        pcqLog->PcqEv_ChDone = EventStatus;
        break;
    case PcqEvDriverReset:
        pcqLog->PcqEv_DriverReset = EventStatus;
        break;
    default:
        UsbhAssertFailure(HubFdo, PortData);
    }        
   
}


VOID
UsbhSetPcqState(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    HUB_PCQ_STATE NewState
    )
{
    PG_STATE_LOG_ENTRY pcqLog;

    UsbhAssert(HubFdo, PortData->PortChangeListLock);

    pcqLog = &PortData->PcqLog[PortData->PcqLogIdx];
    pcqLog->NextState = PortData->Pcq_State = NewState;
}


PVOID
UsbhDispatch_PortChangeQueueEventEx(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    HUB_PCQ_EVENT PcqEvent,
    PSTATE_CONTEXT Context,
    USB_HUB_PORT_EVENT PortEventData,
    PCQ_OBJECT_TYPE PcqObjectType,
    PVOID PcqObject,
    PVOID Parameter
    )
/*++

Routine Description:

Dispatcher routine for port change_queue transitions.  This is the state machine for the
port change queue only.


Arguments:

Irql: This routine may be called at any irql but some of the events must run at passive
    level.  The irql supported is defined by the event so we check in the calling routine.

Parameter: This is a generic pointer, the type is defined by the particular message.

Return Value:

    none.

--*/
{
    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", PcqEvent));
    LOG(HubFdo, LOG_POWER, 'pqDP', Context, PcqEvent);

    switch (PcqEvent) {
    case PCE_Enable:
        Usbh_PCE_Enable_Action(HubFdo, Pd, Context);
        break;

    case PCE_Disable:
        Usbh_PCE_Disable_Action(HubFdo, Pd, Context);
        break;

    case PCE_Resume:
        Usbh_PCE_Resume_Action(HubFdo, Pd, Context);
        break;

    case PCE_QueueChange:
        Usbh_PCE_QueueChange_Action(HubFdo, Pd, Context, PortEventData,
                PcqObjectType, PcqObject);
        break;

    case PCE_wDone:
        Usbh_PCE_wDone_Action(HubFdo, Pd, Context);
        break;

    case PCE_wRun:
        Usbh_PCE_wRun_Action(HubFdo, Pd, Context);
        break;

    case PCE_SoftDisconnect:
        Usbh_PCE_SoftDisconnect_Action(HubFdo, Pd, Context);
        break;

    case PCE_HardwareWake:
        Usbh_PCE_HardwareWake_Action(HubFdo, Pd, Context);
        break;

    case PCE_BusReset:
        Usbh_PCE_BusReset_Action(HubFdo, Pd, Context);
        break;

    case PCE_Suspend:
        Usbh_PCE_Suspend_Action(HubFdo, Pd, Context, Parameter);
        break;

    case PCE_BusDisconnect:
        Usbh_PCE_BusDisconnect_Action(HubFdo, Pd, Context);
        break;

    case PCE_ResumeTimeout:
        Usbh_PCE_ResumeTimeout_Action(HubFdo, Pd, Context);
        break;

    case PCE_QueueDriverReset:
        Usbh_PCE_QueueDriverReset_Action(HubFdo, Pd, Context, PortEventData,
                PcqObjectType, PcqObject);
        break;        

    case PCE_wChange:
        Parameter = Usbh_PCE_wChange_Action(HubFdo, Pd, Context);
        break;  

    case PCE_Check:
        Parameter = Usbh_PCE_Check_Action(HubFdo, Pd, Context, Parameter);
        break;

    case PCE_psPAUSED:
        Usbh_PCE_psPAUSED_Action(HubFdo, Pd, Context);
        break;

    case PCE_psSUSPEND:
        Usbh_PCE_psSUSPEND_Action(HubFdo, Pd, Context);
        break;
                
    case PCE_SD_Resume:
        Usbh_PCE_SD_Resume_Action(HubFdo, Pd, Context);
        break;

    case PCE_ResumeDone:
        Usbh_PCE_ResumeDone_Action(HubFdo, Pd, Context);
        break; 

    case PCE_HW_Stop:
        Usbh_PCE_HW_Stop_Action(HubFdo, Pd, Context);
        break;          

    case PCE_Close:
        Usbh_PCE_Close_Action(HubFdo, Pd, Context);
        break;       

    case PCE_wChangeERROR:
        Usbh_PCE_wChangeERROR_Action(HubFdo, Pd, Context, PortEventData, PcqObjectType, PcqObject);
        break;               
        

    default:
        UsbhAssertFailure(HubFdo, Context);

    }

    return Parameter;
}


VOID
UsbhDispatch_PortChangeQueueEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    HUB_PCQ_EVENT PcqEvent,
    PSTATE_CONTEXT Context,
    USB_HUB_PORT_EVENT PortEventData,
    PCQ_OBJECT_TYPE PcqObjectType,
    PVOID PcqObject
    )
{

    PVOID p_tmp;

    // this is the wrapper for the legacy function (older version) so we don't expect a return
    // value here.
    
    p_tmp = UsbhDispatch_PortChangeQueueEventEx(HubFdo,
                                        Pd,
                                        PcqEvent,
                                        Context,
                                        PortEventData,
                                        PcqObjectType,
                                        PcqObject,
                                        NULL);

    UsbhAssert(HubFdo, p_tmp == NULL);
    
}

VOID
UsbhDispatch_PortChangeQueueNullEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    HUB_PCQ_EVENT PcqEvent,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

Call the pcq Dispatch with a NULL event

Arguments:

Irql: This routine may be called at any irql but some of the events must run at passive
    level.

Return Value:

    none.

--*/
{

    USB_HUB_PORT_EVENT pevNull;
    
    pevNull.ul = 0;

    UsbhDispatch_PortChangeQueueEvent(HubFdo, Pd, PcqEvent, Context, pevNull, PcqObj_NoType, NULL);

}


PVOID
UsbhDispatch_PortChangeQueueNullEventEx(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd,
    HUB_PCQ_EVENT PcqEvent,
    PSTATE_CONTEXT Context,
    PVOID Parameter
    )
/*++

Routine Description:

Call the pcq Dispatch with a NULL event

Arguments:

Irql: This routine may be called at any irql but some of the events must run at passive
    level.

Return Value:

    none.

--*/
{

    USB_HUB_PORT_EVENT pevNull;
    pevNull.ul = 0;

    Parameter = UsbhDispatch_PortChangeQueueEventEx(HubFdo, 
                                            Pd, 
                                            PcqEvent, 
                                            Context, 
                                            pevNull, 
                                            PcqObj_NoType, 
                                            NULL,
                                            Parameter);
    
    return Parameter;

}


VOID
Usbh_PCE_HardwareWake_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_HardwareWake event for all states.

    called by UsbhPCE_HardwareWake

    This is used to resume a port that is in the Pcq_Suspend state when a hardware event
    occurs.  It has no effect on any other state.  We use this to wake the port change up
    if it is suspended but still processing requests.

    This is called by the change processor before queueing an event.  This is intended as a
    bottom up function where the resume state has changed in hardware.  Software resume
    is initiated with a PCE_Resume event.

    **This routine does not trigger resume signalling on the port.

Arguments:

Irql: ANY

Return Value:

    none.

--*/
{
    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;

    hubExt = FdoExt(HubFdo);

    state = UsbhLockPcq(HubFdo, PortData, PCE_HardwareWake, &irql);
    LOG(HubFdo, LOG_PCQ, '12p0', state, PN(PortData));

    switch(state) {

    case Pcq_Run_Idle:
    case Pcq_Run_wBusy:
    case Pcq_Run_wQueued:
    case Pcq_Run_wBusy_Signal:        
        LOG(HubFdo, LOG_PCQ, '12p1', state, PN(PortData));
        // these are a nop because the hub is already 'awake'
        UsbhSetPcqState(HubFdo, PortData,  state);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_PortSuspended:
        LOG(HubFdo, LOG_PCQ, '12p2', state, PN(PortData));
        // abandon the suspend state
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Run_Idle);
        PortData->CurrentState = PS_WAIT_CONNECT;

        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Reset);
        RESET_EVENT(&PortData->PortChangeListDone);
      
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Stop:
    case Pcq_HW_Stop:        
    case Pcq_HW_Stop_wBusy:        
    case Pcq_HW_Stop_wQueued:         
    case Pcq_Stop_wBusy:        
    case Pcq_Stop_wBusy_Signal:      
    case Pcq_Stop_wQueued:          
    case Pcq_Stop_Suspended:
        // hub is stopping, probably unplug or overcurrent
        LOG(HubFdo, LOG_PCQ, '12p3', state, PN(PortData));

        UsbhSetPcqState(HubFdo, PortData, state);

        // port may already be suspended as a result of system power management, in this case we won't 
        // try to wake up 
        //UsbhAssert(HubFdo,  PortData->CurrentState != PS_SUSPENDED);
        if (PortData->CurrentState == PS_SUSPENDED) {
            
            LOG(HubFdo, LOG_PCQ, '12pi', state, PN(PortData));
            TEST_TRAP();
            UsbhUnlockPcq(HubFdo, PortData, irql);
            
        } else {            
            UsbhUnlockPcq(HubFdo, PortData, irql);
        }            
        break;

    case Pcq_Suspend_wBusy:       
    case Pcq_Suspend_wBusy_Signal:       
        // hub signalled us while we were suspending
        LOG(HubFdo, LOG_PCQ, '12p4', state, PN(PortData));
       
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Suspend_wBusy_Signal);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Suspend_wQueued:       
        // hub signaled us while suspend opertaion was queued, attempt to remove it and return to idle state
        LOG(HubFdo, LOG_PCQ, '12p5', state, PN(PortData));
        UsbhRemoveQueuedSuspend(HubFdo, PortData);
        // transition to normal wait state
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Run_wQueued);
        
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Reset);
        RESET_EVENT(&PortData->PortChangeListDone);
      
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;        

    case Pcq_Closed:
    case Pcq_Wait_Resume:
    case Pcq_Pause:
        // It's possible to get a status change when transitioning from Pcq_Stop -> Pcq_Pause -> Pcq_Run_Idle 
        // after handling an overcurrent.  Simply ignore the hardware wake event.
        LOG(HubFdo, LOG_PCQ, '12p6', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  state);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;
    
    default:
        LOG(HubFdo, LOG_PCQ, '12pX', state, PN(PortData));
        UsbhAssertFailure(HubFdo, Context);
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }

}



VOID
Usbh_PCE_Disable_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_Disable event for all states

    called by UsbhPCE_Disable

Arguments:

Irql: PASSIVE

Return Value:

    none.

--*/
{
    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;

    hubExt = FdoExt(HubFdo);
    UsbhAssertPassive();

    state = UsbhLockPcq(HubFdo, PortData, PCE_Disable, &irql);

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p empty \n",  PN(PortData), PortData));
    LOG(HubFdo, LOG_PCQ, '_2p0', state, PN(PortData));

    switch(state) {
    case Pcq_PortSuspended:
        // suspended -- go directly to stop
        LOG(HubFdo, LOG_PCQ, '_2p1', state, PN(PortData));
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Signal);
        UsbhSetPcqState(HubFdo, PortData, Pcq_Stop_Suspended);
        UsbhFlushPortChangeQueue(HubFdo, PortData);
        SET_EVENT(&PortData->PortChangeListDone);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Wait_Resume:
    case Pcq_Pause:
    case Pcq_Run_Idle:
        // not busy -- go directly to stop
        LOG(HubFdo, LOG_PCQ, '_2p2', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData, Pcq_Stop);
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Signal);
        UsbhFlushPortChangeQueue(HubFdo, PortData);
        SET_EVENT(&PortData->PortChangeListDone);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Run_wQueued:
        // we must wait for outstanding requests to finish
        LOG(HubFdo, LOG_PCQ, '_2p3', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Stop_wQueued);
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Wait);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    
    case Pcq_Stop_wBusy:            
    case Pcq_Run_wBusy:
        // we must wait for outstanding requests to finish
        LOG(HubFdo, LOG_PCQ, '_2p4', state, PN(PortData));
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Wait);
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Stop_wBusy);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;        

    case Pcq_HW_Stop:
    case Pcq_Stop:
    case Pcq_Stop_Suspended:        
        LOG(HubFdo, LOG_PCQ, '_2p5', state, PN(PortData));
        UsbhFlushPortChangeQueue(HubFdo, PortData);
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Signal);
        SET_EVENT(&PortData->PortChangeListDone);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;
     
    case Pcq_Suspend_wQueued:        
//*** This should remove any suspend request in the queue
        UsbhRemoveQueuedSuspend(HubFdo, PortData);

        LOG(HubFdo, LOG_PCQ, '_2p6', state, PN(PortData));
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Wait);
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Stop_wQueued);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Suspend_wBusy:       
//*** This should remove any suspend request in the queue        
        UsbhRemoveQueuedSuspend(HubFdo, PortData);

        // the suspend request to the port may have already been issued so it may not be in the queue,
        // if it is not we signal it here
        UsbhiSignalSuspendEvent(HubFdo, PortData, FALSE);
        
        LOG(HubFdo, LOG_PCQ, '_2p7', state, PN(PortData));
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Wait);
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Stop_wBusy);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;                

   case Pcq_Suspend_wBusy_Signal:       
//*** This should remove any suspend request in the queue        
        UsbhRemoveQueuedSuspend(HubFdo, PortData);

        // the suspend request to the port may have already been issued so it may not be in the queue,
        // if it is not we signal it here
        UsbhiSignalSuspendEvent(HubFdo, PortData, FALSE);
         
        LOG(HubFdo, LOG_PCQ, '_2p7', state, PN(PortData));
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Wait);
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Stop_wBusy_Signal);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;    

    case Pcq_Stop_wQueued:
    case Pcq_HW_Stop_wBusy:
    case Pcq_HW_Stop_wQueued:
        // we must wait for outstanding requests to finish
        LOG(HubFdo, LOG_PCQ, '_2p8', state, PN(PortData));
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Wait);
        UsbhSetPcqState(HubFdo, PortData,  state);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;
        
    case Pcq_Stop_wBusy_Signal:
    case Pcq_Run_wBusy_Signal:
        // we must wait for outstanding requests to finish
        LOG(HubFdo, LOG_PCQ, '_2p9', state, PN(PortData));
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Wait);
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Stop_wBusy_Signal);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;        

    default:
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Wait);
        UsbhAssertFailure(HubFdo, Context);
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }

    DbgTrace((UsbhDebugTrace, "#%d Waiting for changelist\n", PN(PortData)));
    LOG(HubFdo, LOG_PCQ, '+wPC', 0, PN(PortData));
    //WAIT_EVENT(&PortData->PortChangeListDone);
    UsbhWaitEventWithTimeoutEx(HubFdo, &PortData->PortChangeListDone, USBHUB_X9F_TIMEOUT, '+wPC', 
                    Timeout_PCE_Disable_Action, PortData);

    LOG(HubFdo, LOG_PCQ, '-wPC', 0, PN(PortData));
    
}


VOID
Usbh_PCE_SoftDisconnect_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_SoftDisconnect event for all states

    called by UsbhPCE_SoftDisconnect

    This event disconnects the device on the port.  If the change queue  is paused
    or stopped the port is considered 'hardware gone' otherwise this is treated as a
    'port cycle'.

    This routine is called as a result of a surprise remove message.  The hardware may or
    may not be gone at this point -- we must check

Arguments:

Irql: PASSIVE

Return Value:

    none.

--*/
{
    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;

    hubExt = FdoExt(HubFdo);
    UsbhAssertPassive();

    state = UsbhLockPcq(HubFdo, PortData, PCE_SoftDisconnect, &irql);

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p empty \n",  PN(PortData), PortData));
    LOG(HubFdo, LOG_PCQ, '_8p0', state, PN(PortData));

    switch(state) {
    case Pcq_PortSuspended:
        // return to the connect wait state paused
        // this is basically the same as a reset
        LOG(HubFdo, LOG_PCQ, '_8p1', state, PN(PortData));

        PortData->CurrentState = PS_WAIT_CONNECT;
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Pause);
        UsbhUnlockPcq(HubFdo, PortData, irql);

        UsbhSoftDisconnectPdo(HubFdo, PdoEv_PdoSoftDisconnect1, Context);
        break;

    // not processing changes so we can drop it directly
    case Pcq_Pause:
    case Pcq_Stop:
    case Pcq_Stop_Suspended:        
    case Pcq_HW_Stop:
    case Pcq_Wait_Resume:
    case Pcq_Closed:                   

        LOG(HubFdo, LOG_PCQ, '_8p2', state, PN(PortData));

        // not processing changes so we can drop it directly without going
        // thru the enum engine.
        UsbhUnlockPcq(HubFdo, PortData, irql);

        UsbhSoftDisconnectPdo(HubFdo, PdoEv_PdoSoftDisconnect2, Context);
        break;

    // The queue is currently running, we insert a pause request then wait
    // for the enum state to reach PS_PAUSED.  

    // we do this by stopping the queue
    case Pcq_Run_wBusy:
    case Pcq_Run_wBusy_Signal:        
    case Pcq_Run_Idle:
    case Pcq_Run_wQueued:
        LOG(HubFdo, LOG_PCQ, '_8p3', state, PN(PortData));
        UsbhUnlockPcq(HubFdo, PortData, irql);
         
        UsbhPCE_Disable(HubFdo, PN(PortData), Context);

        LOG(HubFdo, LOG_PCQ, '_8p4', Context, PN(PortData));

        // drop the device
        UsbhSoftDisconnectPdo(HubFdo, PdoEv_PdoSoftDisconnect3, Context);

        // return to 'wait_for_connect' state
        UsbhPCE_SD_Resume(HubFdo, PortData, Context);
        break;

    case Pcq_Stop_wQueued:
    case Pcq_Stop_wBusy:
    case Pcq_Stop_wBusy_Signal:        
    case Pcq_Suspend_wBusy:
    case Pcq_Suspend_wQueued:
    case Pcq_Suspend_wBusy_Signal:
    case Pcq_HW_Stop_wQueued:
    case Pcq_HW_Stop_wBusy:
        LOG(HubFdo, LOG_PCQ, '_8p5', state, PN(PortData));
        UsbhFlushPortChangeQueue(HubFdo, PortData);

        UsbhUnlockPcq(HubFdo, PortData, irql);

        UsbhSoftDisconnectPdo(HubFdo, PdoEv_PdoSoftDisconnect4, Context);
        break;

    default:
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
        UsbhAssertFailure(HubFdo, Context);
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }

}


VOID
Usbh_PCE_Resume_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_Resume(3) event for all states.

    This is the message power and reset code uses to restart the queues and resume the
    port if necessary.

    This routine is also used to start the queues at init time.

    called by UsbhPCE_Resume
    
Arguments:

Irql: ANY

Return Value:

    none.

--*/
{
    HUB_PCQ_STATE state, newState;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;
    USB_HUB_PORT_EVENT pev;
    PSSH_BUSY_HANDLE bh;
      
    hubExt = FdoExt(HubFdo);
    UsbhAssertPassive();

    // lock out selective suspend during resume process
    bh = UsbhIncHubBusy(HubFdo, Context, HubFdo, UsbhPcqResume_Tag, NO_RESUME_HUB);
    
    state = UsbhLockPcq(HubFdo, PortData, PCE_Resume, &irql);
    LOG(HubFdo, LOG_PCQ, '_3p0', state, PN(PortData));

    switch(state) {

    case Pcq_Stop_Suspended:
    case Pcq_PortSuspended:
        // insert the resume message
        pev.ul = 0;
        pev.RequestPortResume = 1;

        // port resume is pending
        LOG(HubFdo, LOG_PCQ, '_3p1', state, PN(PortData));
       
        UsbhSetResumePending(HubFdo, PortData);
      
        // port itself may not be suspended but this is OK since we are queuing the request
        //UsbhAssert(HubFdo, PortData->CurrentState == PS_SUSPENDED);

        UsbhSetPcqState(HubFdo, PortData,  Pcq_Wait_Resume);
        UsbhInsertQueuePortChange(HubFdo, PortData->PortNumber, &pev);
        
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Reset);
        RESET_EVENT(&PortData->PortChangeListDone);
        

        UsbhUnlockPcqWithTag(HubFdo, PortData, irql, pev.ul);
        
        UsbhDecHubBusy(HubFdo, Context, bh);
   
        break;

    case Pcq_Stop:  
        // insert the resume message to detect any disconnect we may have missed
        pev.ul = 0;
        pev.RequestPortResume = 1;

        // port resume is pending
        LOG(HubFdo, LOG_PCQ, '_3p2', state, PN(PortData));
        UsbhSetResumePending(HubFdo, PortData);
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvResume, PcqEv_Reset);
        // port itself may not be suspended but this is OK since we are queuing the request
        //UsbhAssert(HubFdo, PortData->CurrentState == PS_SUSPENDED);
        PortData->CurrentState = PS_WAIT_CONNECT;
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Pause);
        
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Reset);
        RESET_EVENT(&PortData->PortChangeListDone);
        
        UsbhInsertQueuePortChange(HubFdo, PortData->PortNumber, &pev);

        UsbhUnlockPcqWithTag(HubFdo, PortData, irql, pev.ul);

        UsbhDecHubBusy(HubFdo, Context, bh);
        break;
        
    case Pcq_Pause:
    //case Pcq_Stop:

        UsbhAssert(HubFdo, PortData->CurrentState != PS_SUSPENDED);
        LOG(HubFdo, LOG_PCQ, '_3p3', state, PN(PortData));

        // signal anyone waiting on resume
        UsbhiSignalResumeEvent(HubFdo, PortData, FALSE);
      
        // return to connect/disconnect wait state
        PortData->CurrentState = PS_WAIT_CONNECT;
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Pause);
        UsbhUnlockPcq(HubFdo, PortData, irql);

        UsbhDecHubBusy(HubFdo, Context, bh);
        break;

    case Pcq_Suspend_wQueued: 
    case Pcq_Suspend_wBusy:       
    case Pcq_Suspend_wBusy_Signal:       
        // abort the suspend is in progress and return to the running state
        if (state == Pcq_Suspend_wQueued) {
            newState = Pcq_Run_wQueued;
        } else if (state == Pcq_Suspend_wBusy) {
            newState = Pcq_Run_wBusy;
        } else {
            newState = Pcq_Run_wBusy_Signal;
        }
       
        UsbhRemoveQueuedSuspend(HubFdo, PortData);
        UsbhiSignalSuspendEvent(HubFdo, PortData, FALSE);

        LOG(HubFdo, LOG_PCQ, '_3p4', newState, PN(PortData));

        // Port may already have suspended, in which case we need to queue a resume request
        if (PortData->CurrentState == PS_SUSPENDED) {
            // insert the resume message
            pev.ul = 0;
            pev.RequestPortResume = 1;

            LOG(HubFdo, LOG_PCQ, '_3pR', state, PN(PortData));
      
            UsbhSetResumePending(HubFdo, PortData);

            UsbhInsertQueuePortChange(HubFdo, PortData->PortNumber, &pev);
        }
               
        UsbhSetPcqState(HubFdo, PortData,  newState);
        UsbhUnlockPcq(HubFdo, PortData, irql);

        UsbhDecHubBusy(HubFdo, Context, bh);
        break; 

    case Pcq_Run_wQueued:
    case Pcq_Run_Idle:     
    case Pcq_Run_wBusy:    
    case Pcq_Run_wBusy_Signal:  
        // port is already 'resumed'
        UsbhiSignalResumeEvent(HubFdo, PortData, FALSE);

        LOG(HubFdo, LOG_PCQ, '_3p5', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  state);
        UsbhUnlockPcq(HubFdo, PortData, irql);

        UsbhDecHubBusy(HubFdo, Context, bh);
        break;

    case Pcq_Closed:                  
    case Pcq_HW_Stop:  
    case Pcq_HW_Stop_wQueued:
    case Pcq_HW_Stop_wBusy:
        // port is closed, pdo may be holding hub in mem.
        UsbhiSignalResumeEvent(HubFdo, PortData, FALSE);

        LOG(HubFdo, LOG_PCQ, '_3p6', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  state);
        UsbhUnlockPcq(HubFdo, PortData, irql);

        UsbhDecHubBusy(HubFdo, Context, bh);
        break;

    case Pcq_Stop_wQueued:
    case Pcq_Stop_wBusy:
    case Pcq_Stop_wBusy_Signal:
        // insert the resume message to detect any disconnect we may have missed
        pev.ul = 0;
        pev.RequestPortResume = 1;

        LOG(HubFdo, LOG_PCQ, '_3p7', state, PN(PortData));

        UsbhSetResumePending(HubFdo, PortData);
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvResume, PcqEv_Reset);
        UsbhInsertQueuePortChange(HubFdo, PortData->PortNumber, &pev);
        UsbhUnlockPcqWithTag(HubFdo, PortData, irql, pev.ul);
        UsbhDecHubBusy(HubFdo, Context, bh);
        break;

    case Pcq_Wait_Resume:
        // Resume is already pending, so ignore this event
        LOG(HubFdo, LOG_PCQ, '_3p8', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  state);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        UsbhDecHubBusy(HubFdo, Context, bh);
        break;

    default:
        LOG(HubFdo, LOG_PCQ, '_3pX', state, PN(PortData));

        UsbhAssert(HubFdo,  PortData->CurrentState != PS_SUSPENDED);
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
        UsbhAssertFailure(HubFdo, Context);
        UsbhUnlockPcq(HubFdo, PortData, irql);

        UsbhDecHubBusy(HubFdo, Context, bh);
    }

}

    
VOID
Usbh_PCE_ResumeDone_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_ResumeDone(21) event for all states.

   
    called by UsbhPCE_ResumeDone
    
Arguments:

Irql: ANY

Return Value:

    none.

--*/
{
    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;

    hubExt = FdoExt(HubFdo);
    UsbhAssertPassive();

    state = UsbhLockPcq(HubFdo, PortData, PCE_ResumeDone, &irql);
    LOG(HubFdo, LOG_PCQ, '21p0', state, PN(PortData));

    switch(state) {

    case Pcq_Run_Idle:
    case Pcq_Run_wBusy:        
    case Pcq_Run_wQueued:    
    case Pcq_Run_wBusy_Signal:        
        LOG(HubFdo, LOG_PCQ, '21p1', state, PN(PortData));

        // if are in the running state then do the suspend
        UsbhSetPcqState(HubFdo, PortData,  state);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        
        UsbhPCE_Suspend(HubFdo, Context, PN(PortData));
        break;

    default:
        LOG(HubFdo, LOG_PCQ, '21pX', state, PN(PortData));

        UsbhAssert(HubFdo,  PortData->CurrentState != PS_SUSPENDED);
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
        //UsbhAssertFailure(HubFdo, Context);
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }

}



VOID
Usbh_PCE_ResumeTimeout_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_ResumeTimeout(14) event for all states.

    Indicates that the port has not resumed in a reasonable time.  This could be due to stress 
    factors like DMA verifier or hardware failure.

    

Arguments:

Irql: ANY

Return Value:

    none.

--*/
{
    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;

    hubExt = FdoExt(HubFdo);
    UsbhAssertPassive();

    state = UsbhLockPcq(HubFdo, PortData, PCE_ResumeTimeout, &irql);
    LOG(HubFdo, LOG_PCQ, '14p0', state, PN(PortData));
    // Test #2002
    LOG(HubFdo, LOG_PCQ, '14p1', state, PN(PortData));

    switch(state) {

    case Pcq_Pause:

        // see if the resume has been dispatched. 
        // if it has not the this is a nop and we wait for another pass.
        
        LOG(HubFdo, LOG_PCQ, '14p2', PortData->CurrentState, PN(PortData));

        if (PortData->CurrentState != PS_SUSPENDED) {
            // port waiting on resume, time out the resume and flush the queue
            LOG(HubFdo, LOG_PCQ, '14p3', 0, PN(PortData));

            PortData->ResumeTimeout = TRUE;
            UsbhFlushPortChangeQueue(HubFdo, PortData);
            
            // clear the event, release the resume thread to recover the port
            UsbhiSignalResumeEvent(HubFdo, PortData, TRUE);
        }             

        LOG(HubFdo, LOG_PCQ, '14p4', 0, PN(PortData));

        UsbhSetPcqState(HubFdo, PortData,  state);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Run_wBusy:
    case Pcq_Run_wBusy_Signal:
    case Pcq_Run_wQueued:        

        // port could be blocked trying to handle a connect change
        // in this case we signal the resume event, as any connect change 
        // supercedes a resume event
        if (PortData->ConnectChangeDispatched) {
            PortData->ResumeTimeout = TRUE;
            UsbhiSignalResumeEvent(HubFdo, PortData, TRUE);
        }

        // this state indicates we are still in the process doing a resume 
        // on the port -- again this is to handle the system under load case.
        // in this situation we leave resume pending and the resuming thread will re-wait.
        LOG(HubFdo, LOG_PCQ, '14p5', state, PN(PortData));
        PortData->ResumeTimeout = FALSE;
        
        UsbhSetPcqState(HubFdo, PortData,  state);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;
        
    default:

        // if we are not in the paused state then timeout the resume
        // we have at least had a chance to issue the command
        LOG(HubFdo, LOG_PCQ, '14p6', PortData->CurrentState, PN(PortData));

        UsbhAssert(HubFdo,  PortData->CurrentState != PS_SUSPENDED);

        PortData->ResumeTimeout = TRUE;
        UsbhFlushPortChangeQueue(HubFdo, PortData);
        
        // clear the event, release the resume thread to recover the port
        UsbhiSignalResumeEvent(HubFdo, PortData, FALSE);
        
        UsbhSetPcqState(HubFdo, PortData,  state);
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }

}



VOID
Usbh_PCE_Enable_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_Enable(1) event for all states.

    called by UsbhPCE_Enable

Arguments:

Irql: ANY

Return Value:

    none.

--*/
{
    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;

    hubExt = FdoExt(HubFdo);

    state = UsbhLockPcq(HubFdo, PortData, PCE_Enable, &irql);
    LOG(HubFdo, LOG_PCQ, '01p0', state, PN(PortData));

    switch(state) {
    case Pcq_Wait_Resume:    
    case Pcq_Pause:

        RESET_EVENT(&PortData->PortChangeListDone);
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Reset);
        if (IsListEmpty(&PortData->PortChangeList)) {

            DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p empty \n",  PN(PortData), PortData));
            LOG(HubFdo, LOG_PCQ, 'enMT', 0, PN(PortData));
            UsbhSetPcqState(HubFdo, PortData,  Pcq_Run_Idle);
            UsbhUnlockPcq(HubFdo, PortData, irql);

        } else {
            DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p signal \n",  PN(PortData), PortData));
            LOG(HubFdo, LOG_PCQ, 'enSG', 0, PN(PortData));
            UsbhSetPcqState(HubFdo, PortData,  Pcq_Run_wQueued);
            UsbhUnlockPcq(HubFdo, PortData, irql);
            UsbhHubQueueProcessChangeWorker(HubFdo, PortData);
        }
        break;

    // these are all safe to ignore
    case Pcq_HW_Stop:
    case Pcq_HW_Stop_wQueued:
    case Pcq_HW_Stop_wBusy:
    case Pcq_Closed:        
    case Pcq_Run_Idle:
    case Pcq_Run_wBusy:
    case Pcq_Run_wQueued:
    case Pcq_Run_wBusy_Signal:   
    case Pcq_Suspend_wBusy:              
    case Pcq_Suspend_wQueued:       
    case Pcq_Suspend_wBusy_Signal:   
        LOG(HubFdo, LOG_PCQ, '01p2', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  state);
        // already enabled
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    default:
        LOG(HubFdo, LOG_PCQ, '01pX', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
        UsbhAssertFailure(HubFdo, Context);
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }

}


VOID
Usbh_PCE_BusDisconnect_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_BusDisconnect(11) event for all states

    This is only valid from the pause state -- it is used by the ESD code
    to drop the devices on a malfunctioning hub.

Arguments:

Irql: ANY

Return Value:

    none.

--*/
{
    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;
    PDEVICE_OBJECT pdo;

    hubExt = FdoExt(HubFdo);
    UsbhAssertPassive();

    state = UsbhLockPcq(HubFdo, PortData, PCE_BusDisconnect, &irql);
    LOG(HubFdo, LOG_PCQ, '11p0', state, PN(PortData));

    switch(state) {
    case Pcq_Run_Idle:   
        TEST_TRAP();
        
    case Pcq_Stop:
    case Pcq_Pause:
        LOG(HubFdo, LOG_PCQ, '11p1', state, PN(PortData));

        UsbhSetPcqState(HubFdo, PortData,  state);
        UsbhUnlockPcq(HubFdo, PortData, irql);
         // this may return NULL if the PDO has been disconnected or not created yet
        pdo = UsbhLatchPdo(HubFdo, PortData->PortNumber, NULL, PdoLatch_PCE_Disconnect_Tag);
        if (pdo) {
            // Unblock D0 for D3Cold device in case it is disconnected
            if (PdoExt(pdo)->PdoFlags.InD3Cold == 1) {
                SET_EVENT(&PdoExt(pdo)->D3ColdReconnectEvent);
            }

            UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatch_PCE_Disconnect_Tag);
            Usbh_Disconnect_PdoEvent(HubFdo, pdo, PdoEv_PceDisconnect, Context);
        }
        break;

     case Pcq_Stop_Suspended:
     case Pcq_PortSuspended:
        LOG(HubFdo, LOG_PCQ, '11p2', state, PN(PortData));

        UsbhiSignalResumeEvent(HubFdo, PortData, FALSE);
        PortData->CurrentState = PS_WAIT_CONNECT;
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Pause);
        UsbhUnlockPcq(HubFdo, PortData, irql);

         // this may return NULL if the PDO has been disconnected or not created yet
        pdo = UsbhLatchPdo(HubFdo, PortData->PortNumber, NULL, PdoLatch_PCE_Disconnect_Tag);
        if (pdo) {
            // Unblock D0 for D3Cold device in case it is disconnected
            if (PdoExt(pdo)->PdoFlags.InD3Cold == 1) {
                SET_EVENT(&PdoExt(pdo)->D3ColdReconnectEvent);
            }

            UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatch_PCE_Disconnect_Tag);
            Usbh_Disconnect_PdoEvent(HubFdo, pdo, PdoEv_PceDisconnect, Context);
        }
        break;

    default:
        // this is treated as a nop for all other states, the caller does not know if the queue is 
        // really paused, if it is not the the device was probably disconnected.
        LOG(HubFdo, LOG_PCQ, '11pX', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  state);
        //UsbhAssertFailure(HubFdo, Context);
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }

}


VOID
Usbh_PCE_Suspend_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context,
    PNTSTATUS Parameter
    )
/*++

Routine Description:

    Handle PCE_Suspend event for all states. This routine attempts to suspend the assocaited port. 

    **This is a synchronous opertaion -- the calling thread blocks here.
    
    called by UsbhPCE_Suspend

Arguments:

Irql: PASSIVE

Return Value:

    none.

--*/
{
    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;
    USB_HUB_PORT_EVENT pev;
    NTSTATUS nts=STATUS_SUCCESS;
    
    hubExt = FdoExt(HubFdo);
    UsbhAssertPassive();

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! >\n",  PN(PortData)));

    UsbhWaitEventWithTimeoutEx(HubFdo, &PortData->DriverResetEvent, USBHUB_X9F_TIMEOUT, '10pA',
                    Timeout_PCE_Suspend_Action0, PortData);

    state = UsbhLockPcq(HubFdo, PortData, PCE_Suspend, &irql);

        // Only suspend the port if it is not in the process of handling a connect change  
    if ((PortData->ConnectChangeQueued || PortData->ConnectChangeDispatched)) {
        *Parameter = STATUS_DEVICE_NOT_CONNECTED;
        UsbhiSignalSuspendEvent(HubFdo, PortData, FALSE);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        return;
    }

    PortData->PcqSuspendKthread = KeGetCurrentThread();
    
    LOG(HubFdo, LOG_PCQ, '10p0', state, PN(PortData));
     // break before we blow the stack
    UsbhAssert(HubFdo, Context->PCE_Suspend_stack < 2);
    InterlockedIncrement(&Context->PCE_Suspend_stack);
    
    UsbhSetSuspendPending(HubFdo, PortData);
    
    switch(state) {

    case Pcq_Stop_Suspended:
    case Pcq_PortSuspended:
        LOG(HubFdo, LOG_PCQ, '10p1', state, PN(PortData));
        PortData->PcqSuspendKthread = NULL;
        // already suspend, this is a nop
        UsbhiSignalSuspendEvent(HubFdo, PortData, FALSE);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

   
        
    case Pcq_Stop:
        LOG(HubFdo, LOG_PCQ, '10p2', state, PN(PortData));
        PortData->PcqSuspendKthread = NULL;
        //NOTE: this path is hit if we may have a device with no driver.
        
        // we do a sync suspend for this case.

        // dump all existing entries in the queue 
        UsbhFlushPortChangeQueue(HubFdo, PortData);
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Stop_Suspended);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        
        // issue a synchronous suspend and set the port state to 'suspended'
        // this routine will also update the pcLog in the PortData structure
        PortData->CurrentState =
            UsbhHubSyncSuspendPortEvent(HubFdo, Context, PortData);

        // suspend is complete
        UsbhSignalSuspendEvent(HubFdo, PortData, FALSE);
        
        break;

    // this is the running case used for selective suspend.  the operation is dispatch thru the hardware change queue
    case Pcq_Run_wBusy:
    case Pcq_Run_wBusy_Signal:        
        
        LOG(HubFdo, LOG_PCQ, '10p3', state, PN(PortData));
        PortData->PcqSuspendKthread = NULL;
        pev.ul = 0;
        pev.RequestPortSuspend = 1;            
        // insert the suspend request in the queue
        nts = UsbhInsertQueuePortChangeObject(HubFdo, PN(PortData), &pev,
                    PcqObj_NoType, NULL);
            
        if (NT_SUCCESS(nts)) {
            UsbhSetPcqState(HubFdo, PortData,  Pcq_Suspend_wBusy_Signal);
        } else {            
            UsbhiSignalSuspendEvent(HubFdo, PortData, FALSE);
        }
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;
            
    case Pcq_Run_wQueued:
        LOG(HubFdo, LOG_PCQ, '10p4', state, PN(PortData));

        pev.ul = 0;
        pev.RequestPortSuspend = 1;
    
        // insert the suspend request in the queue
        nts = UsbhInsertQueuePortChangeObject(HubFdo, PN(PortData), &pev,
                    PcqObj_NoType, NULL);
        if (NT_SUCCESS(nts)) {
            UsbhSetPcqState(HubFdo, PortData,  Pcq_Suspend_wQueued);
        } else {
            UsbhiSignalSuspendEvent(HubFdo, PortData, FALSE);
        }
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Run_Idle:
        LOG(HubFdo, LOG_PCQ, '10p5', state, PN(PortData));
        PortData->PcqSuspendKthread = NULL;
        
        pev.ul = 0;
        pev.RequestPortSuspend = 1;
        // insert the suspend request in the queue
        nts = UsbhInsertQueuePortChangeObject(HubFdo, PN(PortData), &pev,
                    PcqObj_NoType, NULL);

        if (NT_SUCCESS(nts)) {

            DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! Queue Worker\n",  PN(PortData)));
            LOG(HubFdo, LOG_HUB, 'qiP1', PortData, PN(PortData));

            UsbhSetPcqState(HubFdo, PortData,  Pcq_Suspend_wQueued);

            UsbhUnlockPcq(HubFdo, PortData, irql);

            UsbhHubQueueProcessChangeWorker(HubFdo, PortData);

        } else {
            LOG(HubFdo, LOG_HUB, 'qiP2', PortData, PN(PortData));

            UsbhiSignalSuspendEvent(HubFdo, PortData, FALSE);
            // ignore the change if we cannot allocate it
            DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! Q change Failed %!STATUS!\n",
                PN(PortData), nts));
            UsbhUnlockPcq(HubFdo, PortData, irql);
        }
        
        break; 

    case Pcq_Pause:
        UsbhiSignalSuspendEvent(HubFdo, PortData, FALSE);
        
        LOG(HubFdo, LOG_PCQ, '10p6', state, PN(PortData));
        PortData->PcqSuspendKthread = NULL;
        
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;        

    case Pcq_Wait_Resume:
        // this allows us to detect when a suspend collides with a resume
        // wait for current resume to complete
        LOG(HubFdo, LOG_PCQ, '10p7', state, PN(PortData));
        
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvResume, PcqEv_Wait);
        UsbhUnlockPcq(HubFdo, PortData, irql);

        // wait for the current resume to finish
        //WAIT_EVENT(&PortData->PortResumeEvent);  
        UsbhWaitEventWithTimeoutEx(HubFdo, &PortData->PortResumeEvent, USBHUB_X9F_TIMEOUT, '10p7',  
                    Timeout_PCE_Suspend_Action1, PortData);

        PortData->PcqSuspendKthread = NULL;
        //re-enter suspending the port
        UsbhPCE_ResumeDone(HubFdo, PDC(PortData), PN(PortData));
        break;        
        
    case Pcq_Suspend_wBusy:
    case Pcq_Suspend_wQueued:
    case Pcq_Suspend_wBusy_Signal:

        // suspend is already in progress
        LOG(HubFdo, LOG_PCQ, '10p8', state, PN(PortData));
        PortData->PcqSuspendKthread = NULL;
        UsbhSetPcqState(HubFdo, PortData,  state);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_HW_Stop_wBusy:
    case Pcq_HW_Stop_wQueued: 
    case Pcq_Stop_wBusy:
    case Pcq_Stop_wBusy_Signal:
    case Pcq_Stop_wQueued:   
        LOG(HubFdo, LOG_PCQ, '10p9', state, PN(PortData));
        PortData->PcqSuspendKthread = NULL;
        UsbhSetPcqState(HubFdo, PortData,  state);
        UsbhUnlockPcq(HubFdo, PortData, irql);

        // wait on stop request then re-enter
        //WAIT_EVENT(&PortData->PortChangeListDone);
        UsbhWaitEventWithTimeoutEx(HubFdo, &PortData->PortChangeListDone, USBHUB_X9F_TIMEOUT, '10p9', 
                    Timeout_PCE_Suspend_Action2, PortData);

        UsbhPCE_Suspend(HubFdo, Context, PN(PortData));

        InterlockedDecrement(&Context->PCE_Suspend_stack);
        return;

    case Pcq_HW_Stop:
        LOG(HubFdo, LOG_PCQ, '10pa', state, PN(PortData));
        PortData->PcqSuspendKthread = NULL;
        //NOTE: this path is hit if we may have a device with no driver.
        
        // dump all existing entries in the queue 
        UsbhFlushPortChangeQueue(HubFdo, PortData);
        UsbhSetPcqState(HubFdo, PortData,  Pcq_HW_Stop);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        
        // suspend is complete
        UsbhSignalSuspendEvent(HubFdo, PortData, FALSE);
        break;

    case Pcq_Closed:
        // Port is closed, so simply signal suspend event
        UsbhUnlockPcq(HubFdo, PortData, irql);
        UsbhSignalSuspendEvent(HubFdo, PortData, FALSE);
        break;

    default:
        LOG(HubFdo, LOG_PCQ, '10pX', state, PN(PortData));

        PortData->PcqSuspendKthread = NULL;
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
        UsbhAssertFailure(HubFdo, Context);
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }

    //WAIT_EVENT(&PortData->PortSuspendEvent);
    UsbhWaitEventWithTimeoutEx(HubFdo, &PortData->PortSuspendEvent, USBHUB_X9F_TIMEOUT, '10px',
                    Timeout_PCE_Suspend_Action3, PortData);

    if ((hubExt->FdoFlags.HubIsRoot) && (hubExt->HcPciProgIf == USB_HC_TYPE_EHCI)) {
        // EHCI 4.3.1 requires a 10ms delay after root port has completed suspend before it can be resumed
        UsbhWait(HubFdo, 10);
    }

    InterlockedDecrement(&Context->PCE_Suspend_stack);

    *Parameter = nts;
}


VOID
Usbh_PCE_BusReset_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )   
/*++

Routine Description:

    Handle PCE_BusReset(9) event for all states, reset the queues and abandon the
    suspend state.

    called by UsbhPCE_BusReset

Arguments:

Irql: ANY

Return Value:

    none.

--*/
{
    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;

    hubExt = FdoExt(HubFdo);

    state = UsbhLockPcq(HubFdo, PortData, PCE_BusReset, &irql);
    LOG(HubFdo, LOG_PCQ, '09p0', state, PN(PortData));

    switch(state) {
    case Pcq_Stop_Suspended:        
    case Pcq_PortSuspended:
        LOG(HubFdo, LOG_PCQ, '09p1', state, PN(PortData));

        // signal anyone waiting
        UsbhiSignalResumeEvent(HubFdo, PortData, FALSE);

        PortData->CurrentState = PS_WAIT_CONNECT;
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Pause);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Pause:
        LOG(HubFdo, LOG_PCQ, '09p2', state, PN(PortData));

        // already paused, this is a flush
        UsbhSetPcqState(HubFdo, PortData, state);
        UsbhFlushPortChangeQueue(HubFdo, PortData);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Stop:
        LOG(HubFdo, LOG_PCQ, '09p3', state, PN(PortData));

//        UsbhAssert(HubFdo, PortData->CurrentState != PS_SUSPENDED);

        UsbhiSignalResumeEvent(HubFdo, PortData, FALSE);
        // return to connect/disconnect wait state
        PortData->CurrentState = PS_WAIT_CONNECT;
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Pause);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Run_Idle:
    case Pcq_Run_wQueued:
    case Pcq_Run_wBusy:
    case Pcq_Run_wBusy_Signal:
        LOG(HubFdo, LOG_PCQ, '09p4', state, PN(PortData));

        // already reset. I hit this case running regression tests with PMTE
        TEST_TRAP();
        UsbhSetPcqState(HubFdo, PortData, state);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_HW_Stop:        
    case Pcq_HW_Stop_wQueued:  
    case Pcq_HW_Stop_wBusy:
        // Hub is already disconnect or PnP removed
        LOG(HubFdo, LOG_PCQ, '09p5', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData, state);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;


    default:
        LOG(HubFdo, LOG_PCQ, '09pX', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
        UsbhAssertFailure(HubFdo, Context);
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }

}



VOID
Usbh_PCE_QueueChange_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context,
    USB_HUB_PORT_EVENT PortEventData,
    PCQ_OBJECT_TYPE PcqObjectType,
    PVOID PcqObject
    )
/*++

Routine Description:

    Handle PCE_QueueChange event for all states

    This routine inserts a change event to the queue for a specified port.

    If the queue is suspended the request is queued and the queue is immediatly
    flushed -- flushing will process events differently (ie cleanup).

    If the queue is stopped the request is ignored.

Arguments:

Irql: ANY

Return Value:

    none.

--*/
{

    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;
    NTSTATUS nts;
    USHORT portNumber;

    hubExt = FdoExt(HubFdo);

    //lock the queue for insertion ofthe event
    state = UsbhLockPcqWithTag(HubFdo, PortData, PCE_QueueChange, &irql, PortEventData.ul);
    portNumber = PN(PortData);

    LOG(HubFdo, LOG_PCQ, '_4p0', state, PN(PortData));
    LOG(HubFdo, LOG_PCQ, 'Qch0', PortData, PortData->PortNumber);
    LOG(HubFdo, LOG_PCQ, 'Qch0', PortData, PortEventData.ul);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p >\n", PN(PortData), Context));
    UsbhAssert(HubFdo, PortData->PortNumber == portNumber);

    if (PortEventData.PauseChange) {
        UsbhSetPausePending(HubFdo, PortData);
    }

    nts = STATUS_INVALID_PARAMETER;

    switch(state) {
    case Pcq_PortSuspended:
        
        LOG(HubFdo, LOG_PCQ, '_4p1', 0, PortData->PortNumber);
        // insert the change and flush it
        // this will clear the event allow the processing of the event
        // for special cases
        UsbhSetPcqState(HubFdo, PortData, Pcq_PortSuspended);
        nts = UsbhInsertQueuePortChangeObject(HubFdo, portNumber, &PortEventData,
                    PcqObjectType, PcqObject);
        UsbhFlushPortChangeQueue(HubFdo, PortData);

        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Run_Idle:
       
        // queue the change and schedule the workitem
        nts = UsbhInsertQueuePortChangeObject(HubFdo, portNumber, &PortEventData,
            PcqObjectType, PcqObject);
        
        LOG(HubFdo, LOG_PCQ, '_4p2', nts, PortData->PortNumber);
         
        if (NT_SUCCESS(nts)) {

            DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! Queue Worker\n",  portNumber));
            LOG(HubFdo, LOG_HUB, 'qiP1', PortData, portNumber);


            UsbhSetPcqState(HubFdo, PortData,  Pcq_Run_wQueued);

            UsbhUnlockPcq(HubFdo, PortData, irql);

            UsbhHubQueueProcessChangeWorker(HubFdo, PortData);

        } else {
            LOG(HubFdo, LOG_HUB, 'qiP2', PortData, portNumber);

            // ignore the change if we cannot allocate it
            DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! Q change Failed %!STATUS!\n",
                portNumber, nts));

            if (PcqObjectType == PcqObj_Pdo) {
                // release any pending driver reset
                PDEVICE_OBJECT pdo;

                LOG(HubFdo, LOG_PCQ, 'qipx', PcqObject, PortData->PortNumber);
                // latched Pdo for reset -- signal the event
                pdo = PcqObject;
                UsbhAssert(HubFdo, PdoExt(pdo)->SyncDeviceResetEvent);
                Usbh_iSignalSyncDeviceReset(HubFdo, pdo, PortData, STATUS_UNSUCCESSFUL);
            }
            // release any timeouts since we do not queue them
            if (PcqObjectType == PcqObj_Timeout) {
                PPCQ_TIMEOUT_OBJECT tmoObj;

                tmoObj = PcqObject;
                UsbhFreePortChangeQueueTimeoutObject(HubFdo, PortData, tmoObj, TRUE);
            }            
            UsbhUnlockPcq(HubFdo, PortData, irql);
        }
        break;

    case Pcq_Run_wQueued:
        LOG(HubFdo, LOG_PCQ, '_4p3', 0, PortData->PortNumber);
        // worker is running or queued
        DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! wQueued, wBusy\n", portNumber));

        nts = UsbhInsertQueuePortChangeObject(HubFdo, portNumber, &PortEventData,
                    PcqObjectType, PcqObject);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;


    case Pcq_Run_wBusy_Signal:
    case Pcq_Run_wBusy:
        LOG(HubFdo, LOG_PCQ, '_4p4', 0, PortData->PortNumber);
        // worker is running or queued
        DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! wQueued, wBusy\n", portNumber));

        nts = UsbhInsertQueuePortChangeObject(HubFdo, portNumber, &PortEventData,
                    PcqObjectType, PcqObject);

        UsbhSetPcqState(HubFdo, PortData,  Pcq_Run_wBusy_Signal);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_HW_Stop:                   
    case Pcq_HW_Stop_wQueued:  
    case Pcq_HW_Stop_wBusy:
    case Pcq_Stop_Suspended:
    case Pcq_Stop_wBusy:    
    case Pcq_Stop_wBusy_Signal:              
    case Pcq_Stop_wQueued:
    case Pcq_Stop:
    case Pcq_Closed:                   
        LOG(HubFdo, LOG_PCQ, '_4p5', PcqObjectType, PortData->PortNumber);
        //
        // This happens events are queued during stop -- (like port reset timeout).
        //
        if (PcqObjectType == PcqObj_Pdo) {
            // release any pending driver reset
            PDEVICE_OBJECT pdo;

            LOG(HubFdo, LOG_PCQ, '_4px', PcqObject, PortData->PortNumber);
        
            // latched Pdo for reset -- signal the event
            pdo = PcqObject;
            
            UsbhAssert(HubFdo, PdoExt(pdo)->SyncDeviceResetEvent);
            Usbh_iSignalSyncDeviceReset(HubFdo, pdo, PortData, STATUS_UNSUCCESSFUL);

        }

        // release any timeouts since we do not queue them
        if (PcqObjectType == PcqObj_Timeout) {
            PPCQ_TIMEOUT_OBJECT tmoObj;

            tmoObj = PcqObject;
            UsbhFreePortChangeQueueTimeoutObject(HubFdo, PortData, tmoObj, TRUE);
        }            
            
        DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! Cancel, Disabled\n", portNumber));
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Wait_Resume:
    case Pcq_Pause:
        LOG(HubFdo, LOG_PCQ, '_4p6', 0, PortData->PortNumber);
        // queue changes but don't run the worker
        DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! Enabled\n", portNumber));
        nts = UsbhInsertQueuePortChangeObject(HubFdo, portNumber, &PortEventData,
                    PcqObjectType, PcqObject);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Suspend_wBusy:  
    case Pcq_Suspend_wBusy_Signal:      
        LOG(HubFdo, LOG_PCQ, '_4p7', 0, PortData->PortNumber);
        nts = UsbhInsertQueuePortChangeObject(HubFdo, portNumber, &PortEventData,
                    PcqObjectType, PcqObject);

        UsbhSetPcqState(HubFdo, PortData,  Pcq_Suspend_wBusy_Signal);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;
        
    case Pcq_Suspend_wQueued:
        LOG(HubFdo, LOG_PCQ, '_4p8', 0, PortData->PortNumber);
        nts = UsbhInsertQueuePortChangeObject(HubFdo, portNumber, &PortEventData,
                    PcqObjectType, PcqObject);

        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;   

    default:
        LOG(HubFdo, LOG_PCQ, '_4pX', 0, PortData->PortNumber);
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
        UsbhAssertFailure(HubFdo, Context);
        UsbhUnlockPcq(HubFdo, PortData, irql);

   }

   DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! <<%!PCSTATE!\n",
                       portNumber, state));

}


VOID
Usbh_PCE_QueueDriverReset_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context,
    USB_HUB_PORT_EVENT PortEventData,
    PCQ_OBJECT_TYPE PcqObjectType,
    PVOID PcqObject
    )
/*++

Routine Description:

    Handle PCE_QueueDriverReset(15) event for all states

    This routine inserts a DRIVER_RESET change event to the queue for a specified port.


Arguments:

Irql: ANY

Return Value:

    none.

--*/
{

    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;
    NTSTATUS nts;
    USHORT portNumber;
    PDEVICE_OBJECT pdo;
    
    hubExt = FdoExt(HubFdo);

    state = UsbhLockPcqWithTag(HubFdo, PortData, PCE_QueueDriverReset, &irql, PortEventData.ul);

    if (PortData->ConnectChangeQueued || PortData->ConnectChangeDispatched ||
        (PortData->CurrentState == PS_ENUMERATE_1) || 
        (PortData->CurrentState == PS_ENUMERATE_2) ) {
        // Don't allow a driver reset to be queued if a connect change is
        // already in the queue, currently being processed, or the port is in enumeration

        pdo = PcqObject;
        UsbhAssert(HubFdo, PdoExt(pdo)->SyncDeviceResetEvent);
        Usbh_iSignalSyncDeviceReset(HubFdo, pdo, PortData, STATUS_UNSUCCESSFUL);
        UsbhUnlockPcq(HubFdo, PortData, irql);

        return;
    }

    portNumber = PN(PortData);

    LOG(HubFdo, LOG_PCQ, '15p0', state, PN(PortData));
    LOG(HubFdo, LOG_PCQ, '15p1', PortData, PortData->PortNumber);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p >\n", PN(PortData), Context));
    UsbhAssert(HubFdo, PortData->PortNumber == portNumber);

    Usbh_iSetDriverResetPending(HubFdo, PortData);

    nts = STATUS_INVALID_PARAMETER;

    switch(state) {
    case Pcq_Stop:        
    case Pcq_Stop_Suspended:        
    case Pcq_PortSuspended:
    case Pcq_Pause:        
    case Pcq_Closed:        
    case Pcq_Stop_wQueued:
    case Pcq_Stop_wBusy:
    case Pcq_Stop_wBusy_Signal:
    case Pcq_HW_Stop:
    case Pcq_HW_Stop_wQueued:
    case Pcq_HW_Stop_wBusy:
    case Pcq_Suspend_wBusy:
    case Pcq_Suspend_wQueued:
    case Pcq_Suspend_wBusy_Signal:
    case Pcq_Wait_Resume:
        // port must be in the run state to be reset
        LOG(HubFdo, LOG_PCQ, '15p1', state, PN(PortData));
        pdo = PcqObject;
        UsbhAssert(HubFdo, PdoExt(pdo)->SyncDeviceResetEvent);
        Usbh_iSignalSyncDeviceReset(HubFdo, pdo, PortData, STATUS_UNSUCCESSFUL);

        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Run_Idle:
        LOG(HubFdo, LOG_PCQ, '15p2', state, PN(PortData));
        // queue the change
        nts = UsbhInsertQueuePortChangeObject(HubFdo, portNumber, &PortEventData,
            PcqObjectType, PcqObject);

        if (NT_SUCCESS(nts)) {

            DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! Queue Worker\n",  portNumber));
            LOG(HubFdo, LOG_HUB, '15p3', PortData, portNumber);


            UsbhSetPcqState(HubFdo, PortData,  Pcq_Run_wQueued);

            UsbhUnlockPcq(HubFdo, PortData, irql);

            UsbhHubQueueProcessChangeWorker(HubFdo, PortData);

        } else {
            LOG(HubFdo, LOG_HUB, '15p4', PortData, portNumber);
            // ignore the change if we cannot allocate it
            DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! Q change Failed %!STATUS!\n",
                portNumber, nts));

            // cannot queue the change, fail the request 
            pdo = PcqObject;
            UsbhAssert(HubFdo, PdoExt(pdo)->SyncDeviceResetEvent);
            Usbh_iSignalSyncDeviceReset(HubFdo, pdo, PortData, STATUS_UNSUCCESSFUL);

            UsbhUnlockPcq(HubFdo, PortData, irql);
        }
        break;

    case Pcq_Run_wQueued:
    case Pcq_Run_wBusy_Signal:
         LOG(HubFdo, LOG_HUB, '15p5', PortData, portNumber);
        // worker is running or queued
        DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! wQueued, wBusy\n", portNumber));

        nts = UsbhInsertQueuePortChangeObject(HubFdo, portNumber, &PortEventData,
                    PcqObjectType, PcqObject);

        if (!NT_SUCCESS(nts)) {
            // cannot queue the change, fail the request 
            pdo = PcqObject;
            UsbhAssert(HubFdo, PdoExt(pdo)->SyncDeviceResetEvent);
            Usbh_iSignalSyncDeviceReset(HubFdo, pdo, PortData, STATUS_UNSUCCESSFUL);
        }

        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

     case Pcq_Run_wBusy:
         LOG(HubFdo, LOG_HUB, '15p6', PortData, portNumber);
        // worker is running or queued
        DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! wQueued, wBusy\n", portNumber));

        nts = UsbhInsertQueuePortChangeObject(HubFdo, portNumber, &PortEventData,
                    PcqObjectType, PcqObject);

        if (!NT_SUCCESS(nts)) {
            // cannot queue the change, fail the request 
            pdo = PcqObject;
            UsbhAssert(HubFdo, PdoExt(pdo)->SyncDeviceResetEvent);
            Usbh_iSignalSyncDeviceReset(HubFdo, pdo, PortData, STATUS_UNSUCCESSFUL);
        }

        UsbhSetPcqState(HubFdo, PortData,  Pcq_Run_wBusy_Signal);

        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;     
        
    default:
       
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
        UsbhAssertFailure(HubFdo, Context);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;
    }
    
    DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! <<%!PCSTATE!\n",
                       portNumber, state));

}


VOID
Usbh_PCE_wRun_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_wRun(5) event for all states.
    
    called by UsbhPCE_Run

Arguments:

Irql: ANY

Return Value:

    none.

--*/
{

    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;
    NTSTATUS nts;
    USHORT portNumber;

    hubExt = FdoExt(HubFdo);
    UsbhAssert(HubFdo, PortData->Sig == SIG_HUBPORTDATA);
    portNumber = PN(PortData);

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p >\n", PN(PortData), Context));
    UsbhAssert(HubFdo, PortData->PortNumber == portNumber);

    // dequeue and process changes
    state = UsbhLockPcq(HubFdo, PortData, PCE_wRun, &irql);

    LOG(HubFdo, LOG_PCQ, '_5p0', state, PortData->PortNumber);

    nts = STATUS_UNSUCCESSFUL;

    switch(state) {
    case Pcq_Suspend_wBusy_Signal:     
    case Pcq_Run_wBusy_Signal:
        // another instance running
        LOG(HubFdo, LOG_PCQ, '_5p1', state, PN(PortData));
       
        UsbhAssert(HubFdo, !IsListEmpty(&PortData->PortChangeList));
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;
        
    case Pcq_Run_wBusy:
        // another instance running
        LOG(HubFdo, LOG_PCQ, '_5p2', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Run_wBusy_Signal);
        // list may be empty if a suspend operation was canceled 
        //UsbhAssert(HubFdo, !IsListEmpty(&PortData->PortChangeList));
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Suspend_wBusy:
        // another instance running
        LOG(HubFdo, LOG_PCQ, '_5p2', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Suspend_wBusy_Signal);
        UsbhAssert(HubFdo, !IsListEmpty(&PortData->PortChangeList));
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;        

    case Pcq_Stop_wBusy:
        // another instance running
        LOG(HubFdo, LOG_PCQ, '_5p2', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Stop_wBusy_Signal);
        UsbhAssert(HubFdo, !IsListEmpty(&PortData->PortChangeList));
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;              
        
    case Pcq_Run_wQueued:
        LOG(HubFdo, LOG_PCQ, '_5p3', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Run_wBusy);
        // list may be empty if a suspend operation was canceled 
        //UsbhAssert(HubFdo, !IsListEmpty(&PortData->PortChangeList));
        UsbhAssert(HubFdo, PortData->PcqKthread == NULL);
        PortData->PcqKthread = KeGetCurrentThread();
        UsbhUnlockPcq(HubFdo, PortData, irql);
        // run the changes
        UsbhHubRunPortChangeQueue(HubFdo, PortData, Context, &irql);
        break;

     case Pcq_Suspend_wQueued:
        LOG(HubFdo, LOG_PCQ, '_5p4', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Suspend_wBusy);
        // it is OK for suspend worker to be queued with the list empty. 
        //UsbhAssert(HubFdo, !IsListEmpty(&PortData->PortChangeList));
        UsbhAssert(HubFdo, PortData->PcqKthread == NULL);
        PortData->PcqKthread = KeGetCurrentThread();
        UsbhUnlockPcq(HubFdo, PortData, irql);
        // run the changes
        UsbhHubRunPortChangeQueue(HubFdo, PortData, Context, &irql);
        break;        

    case Pcq_Stop_wQueued:
        LOG(HubFdo, LOG_PCQ, '_5p5', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Stop_wBusy);
        UsbhAssert(HubFdo, !IsListEmpty(&PortData->PortChangeList));
        UsbhAssert(HubFdo, PortData->PcqKthread == NULL);
        PortData->PcqKthread = KeGetCurrentThread();
        UsbhUnlockPcq(HubFdo, PortData, irql);
        // run the queue to drain events in an orderly fashion.
        UsbhHubRunPortChangeQueue(HubFdo, PortData, Context, &irql);
        break;
        
    case Pcq_HW_Stop_wBusy:
        // another instance running
        LOG(HubFdo, LOG_PCQ, '_5p6', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  Pcq_HW_Stop_wBusy);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;               

     case Pcq_HW_Stop_wQueued:        
        LOG(HubFdo, LOG_PCQ, '_5p7', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  Pcq_HW_Stop_wBusy);
        UsbhAssert(HubFdo, !IsListEmpty(&PortData->PortChangeList));
        UsbhAssert(HubFdo, PortData->PcqKthread == NULL);
        PortData->PcqKthread = KeGetCurrentThread();
        UsbhUnlockPcq(HubFdo, PortData, irql);
        // run the queue to drain events in an orderly fashion.
        UsbhHubRunPortChangeQueue(HubFdo, PortData, Context, &irql);
        break;

    default:
        LOG(HubFdo, LOG_PCQ, '_5p8', state, PN(PortData));
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Closed:
    case Pcq_Stop:
        RESET_EVENT(&PortData->PortChangeListInsertEvent);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Error:
        RESET_EVENT(&PortData->PortChangeListInsertEvent);
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
        UsbhAssertFailure(HubFdo, Context);
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }

}


VOID
Usbh_PCE_HW_Stop_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_HW_Stop(17) event for all states

    called by called by UsbhPCE_HW_Stop
Arguments:

Irql: ANY

Return Value:

    none.

--*/
{

    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;
    USHORT portNumber;

    hubExt = FdoExt(HubFdo);
    UsbhAssert(HubFdo, PortData->Sig == SIG_HUBPORTDATA);
    portNumber = PN(PortData);

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p >\n", PN(PortData), Context));
    UsbhAssert(HubFdo, PortData->PortNumber == portNumber);

    // dequeue and process changes
    state = UsbhLockPcq(HubFdo, PortData, PCE_HW_Stop, &irql);
    LOG(HubFdo, LOG_PCQ, '17p0', state, PortData->PortNumber);

    switch(state) {

    case Pcq_Stop_wBusy:
    case Pcq_Suspend_wBusy:
    case Pcq_Suspend_wBusy_Signal:
    case Pcq_Run_wBusy:
    case Pcq_Run_wBusy_Signal:
    case Pcq_Stop_wBusy_Signal:
        LOG(HubFdo, LOG_PCQ, '17p1', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  Pcq_HW_Stop_wBusy);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Stop_Suspended: 
    case Pcq_Stop:
    case Pcq_Wait_Resume:
    case Pcq_Pause:        
    case Pcq_PortSuspended:
    case Pcq_Run_Idle:        
        // put the change queue in the permanent stop state
        LOG(HubFdo, LOG_PCQ, '17p2', state, PN(PortData));
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Signal);
        UsbhSetPcqState(HubFdo, PortData, Pcq_HW_Stop);
        UsbhFlushPortChangeQueue(HubFdo, PortData);
        SET_EVENT(&PortData->PortChangeListDone);
        UsbhiSignalResumeEvent(HubFdo, PortData, FALSE);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Stop_wQueued:
    case Pcq_Suspend_wQueued:
    case Pcq_Run_wQueued:
        // we must wait for outstanding requests to finish
        LOG(HubFdo, LOG_PCQ, '17p3', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  Pcq_HW_Stop_wQueued);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_HW_Stop:                   
    case Pcq_HW_Stop_wQueued:  
    case Pcq_HW_Stop_wBusy:
    case Pcq_Closed:
        // we've already received this event previously, so ignore it this time.
        LOG(HubFdo, LOG_PCQ, '17p4', state, PN(PortData));
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    default:
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
        UsbhAssertFailure(HubFdo, Context);
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }

}


VOID
Usbh_PCE_Close_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_Close(22) event for all states

    called by called by UsbhPCE_Close

    
Arguments:

Irql: ANY

Return Value:

    none.

--*/
{

    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;
    USHORT portNumber;

    hubExt = FdoExt(HubFdo);
    UsbhAssert(HubFdo, PortData->Sig == SIG_HUBPORTDATA);
    portNumber = PN(PortData);

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p >\n", PN(PortData), Context));
    UsbhAssert(HubFdo, PortData->PortNumber == portNumber);
    UsbhAssertPassive();
     
    // dequeue and process changes
    state = UsbhLockPcq(HubFdo, PortData, PCE_Close, &irql);
    // break before we blow the stack
    UsbhAssert(HubFdo, Context->PCE_Close_stack < 2);
    InterlockedIncrement(&Context->PCE_Close_stack);
    LOG(HubFdo, LOG_PCQ, '22p0', state, PortData->PortNumber);

    switch(state) {

    case Pcq_Pause:
    case Pcq_HW_Stop:        
    case Pcq_Stop:
        LOG(HubFdo, LOG_PCQ, '22p1', state, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Closed);
        UsbhFlushPortChangeQueue(HubFdo, PortData);
        UsbhAssert(HubFdo, IsListEmpty(&PortData->PortChangeList));
        UsbhUnlockPcq(HubFdo, PortData, irql);
        
        // Wait for any stray work-items to complete
        UsbhHubDereferenceProcessChangeWorkItemAndWait(HubFdo, PortData);
        break;

    case Pcq_HW_Stop_wQueued:     
    case Pcq_HW_Stop_wBusy:       
        // seem to hit this case with cycler, we need to wait for these events to run or we risk
        // accessing the pd after it is freed.
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Wait);
        
        UsbhUnlockPcq(HubFdo, PortData, irql);

        LOG(HubFdo, LOG_PCQ, '22p2', state, PN(PortData));
        //WAIT_EVENT(&PortData->PortChangeListDone);
        UsbhWaitEventWithTimeoutEx(HubFdo, &PortData->PortChangeListDone, USBHUB_X9F_TIMEOUT, '22p2', 
                    Timeout_PCE_Close_Action, PortData);

        // re-enter
        UsbhPCE_Close(HubFdo, PortData, Context);

        InterlockedDecrement(&Context->PCE_Close_stack);
        return;


    default:
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
        UsbhAssertFailure(HubFdo, Context);
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }

    InterlockedDecrement(&Context->PCE_Close_stack);
    return;
}


VOID
Usbh_PCE_wDone_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_wDone(6) event for all states

    called by UsbhPCE_wDone

    This is called when the worker thread exits after 'running' the queue.

Arguments:

Irql: ANY

Return Value:

    none.

--*/
{

    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;
    USHORT portNumber;

    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, PortData->Sig == SIG_HUBPORTDATA);
    portNumber = PN(PortData);

    LOG(HubFdo, LOG_PCQ, '_6p0', PortData, PortData->PortNumber);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p >\n", PN(PortData), Context));
    UsbhAssert(HubFdo, PortData->PortNumber == portNumber);

    // dequeue and process changes
    state = UsbhLockPcq(HubFdo, PortData, PCE_wDone, &irql);
    LOG(HubFdo, LOG_PCQ, '_6p1', state, PN(PortData));

    switch(state) {
    case Pcq_Stop_wBusy_Signal:
        LOG(HubFdo, LOG_PCQ, '_6p2', PortData, PN(PortData));
        DbgTrace((UsbhDebugTrace, "#%d %!FUNC! Signal PortChangeListDone\n", PN(PortData)));
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Signal);
        SET_EVENT(&PortData->PortChangeListDone);

        UsbhSetPcqState(HubFdo, PortData,  Pcq_Stop);
        UsbhFlushPortChangeQueue(HubFdo, PortData);

        // reset port_change_queue current thread to detect rentancy bugs
        PortData->PcqKthread = NULL;
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Stop_wBusy:
        // this means we got into the stop state while the queue was running
        TEST_TRAP();
        LOG(HubFdo, LOG_PCQ, '_6p3', 0, PN(PortData));
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Signal);
        SET_EVENT(&PortData->PortChangeListDone);
        PortData->PcqKthread = NULL;
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Stop);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Run_wBusy:
        UsbhAssert(HubFdo, IsListEmpty(&PortData->PortChangeList));

        DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p empty \n",  PN(PortData), PortData));
        LOG(HubFdo, LOG_PCQ, '_6p4', 0, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData, Pcq_Run_Idle);
        PortData->PcqKthread = NULL;
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;
        
     case Pcq_Run_wBusy_Signal:

        // event was inserted while previous worker was exiting
        DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p signal \n",  PN(PortData), PortData));
        LOG(HubFdo, LOG_PCQ, '_6p5', 0, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData, Pcq_Run_wQueued);
        PortData->PcqKthread = NULL; // this instance will exit
        UsbhUnlockPcq(HubFdo, PortData, irql);

        UsbhHubQueueProcessChangeWorker(HubFdo, PortData);
        break;

    case Pcq_Suspend_wBusy:
        LOG(HubFdo, LOG_PCQ, '_6p6', PortData, PN(PortData));
        DbgTrace((UsbhDebugTrace, "#%d %!FUNC! Signal PortChangeListDone\n", PN(PortData)));
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Signal);
        SET_EVENT(&PortData->PortChangeListDone);
        PortData->PcqKthread = NULL;
        UsbhSetPcqState(HubFdo, PortData,  Pcq_PortSuspended);

        UsbhiSignalSuspendEvent(HubFdo, PortData, FALSE);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    case Pcq_Suspend_wBusy_Signal:
        // a change was queued while exiting, queue the the worker to run again (same as run case for now). 
       
        DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p signal \n",  PN(PortData), PortData));
        LOG(HubFdo, LOG_PCQ, '_6p7', 0, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData, Pcq_Suspend_wQueued);
        PortData->PcqKthread = NULL;
        UsbhUnlockPcq(HubFdo, PortData, irql);
      
        UsbhHubQueueProcessChangeWorker(HubFdo, PortData);
        break;
        
    case Pcq_HW_Stop_wBusy:
        
        LOG(HubFdo, LOG_PCQ, '_6p2', PortData, PN(PortData));
        DbgTrace((UsbhDebugTrace, "#%d %!FUNC! Signal PortChangeListDone\n", PN(PortData)));
        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Signal);
        SET_EVENT(&PortData->PortChangeListDone);

        UsbhSetPcqState(HubFdo, PortData,  Pcq_HW_Stop);
        UsbhFlushPortChangeQueue(HubFdo, PortData);

        // reset port_change_queue current thread to detect rentancy bugs
        PortData->PcqKthread = NULL;
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;        
        
    case Pcq_Stop:
    case Pcq_Run_wQueued:
    case Pcq_PortSuspended:
    default:
        LOG(HubFdo, LOG_PCQ, '_6pX', PortData, PN(PortData));
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
        UsbhAssertFailure(HubFdo, Context);
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }

}


PVOID
Usbh_PCE_wChange_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_wChange(16) event for all states

    called by UsbhPCE_wChange

Arguments:

Irql: ANY

Return Value:

    pointer to next entru in the queue or NULL

--*/
{

    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;
    USHORT portNumber;
    PLIST_ENTRY le = NULL;
    PPORT_CHANGE_CONTEXT pcd;
        
    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, PortData->Sig == SIG_HUBPORTDATA);
    portNumber = PN(PortData);

    LOG(HubFdo, LOG_PCQ, '16p0', PortData, PortData->PortNumber);
   
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p >\n", PN(PortData), Context));
    UsbhAssert(HubFdo, PortData->PortNumber == portNumber);

    // dequeue and process changes
    state = UsbhLockPcq(HubFdo, PortData, PCE_wChange, &irql);

    if (!IsListEmpty(&PortData->PortChangeList)) {
        
        switch(state) {
        case Pcq_Run_wBusy:
        case Pcq_Run_wBusy_Signal: 
        case Pcq_Suspend_wBusy:
        case Pcq_Suspend_wBusy_Signal: 
            le = RemoveHeadList(&PortData->PortChangeList);
            pcd = CONTAINING_RECORD(le,
                                   PORT_CHANGE_CONTEXT,
                                   ChangeLink);
            
            if ((PortData->PD_Flags.ReSuspendPort == 1) && (pcd->PortEvent.ConnectChange == 1)) {
                // Special case to avoid deadlock during re-suspend of port during hub resume from SS
               
                // Remove the suspend request from the queue and signal completion of suspend
                // This will allow the stack to unwind and complete resume of the hub
                UsbhRemoveQueuedSuspend(HubFdo, PortData);

                // The suspend request may have already been dispatched.  Signal the suspend event
                // here to handle this case.
                UsbhiSignalSuspendEvent(HubFdo, PortData, FALSE);

                // Re-insert connect change into head of PCQ
                InsertHeadList(&PortData->PortChangeList,
                               &pcd->ChangeLink);

                PortData->ConnectChangeQueued = TRUE;

                // Return a NULL event to end current thread's processing of the PCQ
                le = NULL;

                // Return PCQ to running/signaled signaled state.  This will trigger
                // scheduling separate work-item to process the connect change.
                UsbhSetPcqState(HubFdo, PortData,  Pcq_Run_wBusy_Signal);
            } else {            
                switch(state) {
                case Pcq_Run_wBusy:
                case Pcq_Run_wBusy_Signal: 
                    UsbhSetPcqState(HubFdo, PortData,  Pcq_Run_wBusy);
                    break;
        
                case Pcq_Suspend_wBusy:
                case Pcq_Suspend_wBusy_Signal: 
                    UsbhSetPcqState(HubFdo, PortData,  Pcq_Suspend_wBusy);
                    break;
                }
            }
            UsbhUnlockPcqWithTag(HubFdo, PortData, irql, pcd->PortEvent.ul);
            break; 

        case Pcq_Stop_wBusy_Signal:
        case Pcq_Stop_wBusy:
            le = RemoveHeadList(&PortData->PortChangeList);
            pcd = CONTAINING_RECORD(le,
                                   PORT_CHANGE_CONTEXT,
                                   ChangeLink);
            
            UsbhSetPcqState(HubFdo, PortData,  Pcq_Stop_wBusy);
            UsbhUnlockPcqWithTag(HubFdo, PortData, irql, pcd->PortEvent.ul);
            break;

        case Pcq_HW_Stop_wBusy:   
            le = RemoveHeadList(&PortData->PortChangeList);
            pcd = CONTAINING_RECORD(le,
                                   PORT_CHANGE_CONTEXT,
                                   ChangeLink);
            
            UsbhSetPcqState(HubFdo, PortData,  Pcq_HW_Stop_wBusy);
            UsbhUnlockPcqWithTag(HubFdo, PortData, irql, pcd->PortEvent.ul);
            break;
            
        default:
            UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
            UsbhAssertFailure(HubFdo, Context);
            UsbhUnlockPcq(HubFdo, PortData, irql);
            break;
        } 

    }   else {
    
        RESET_EVENT(&PortData->PortChangeListInsertEvent);

        switch(state) {
        case Pcq_Run_wBusy:
        case Pcq_Run_wBusy_Signal: 
            le = NULL;
            UsbhSetPcqState(HubFdo, PortData,  Pcq_Run_wBusy);
            UsbhUnlockPcq(HubFdo, PortData, irql);
            break;

        case Pcq_Suspend_wBusy:
        case Pcq_Suspend_wBusy_Signal: 
            le = NULL;
            UsbhSetPcqState(HubFdo, PortData,  Pcq_Suspend_wBusy);
            UsbhUnlockPcq(HubFdo, PortData, irql);
            break; 

        case Pcq_HW_Stop_wBusy:
        case Pcq_Stop_wBusy:
            //UsbhSetPcqState(HubFdo, PortData,  Pcq_Stop_wBusy);
            UsbhUnlockPcq(HubFdo, PortData, irql);
            break;

        case Pcq_Stop_wBusy_Signal:
            UsbhSetPcqState(HubFdo, PortData,  Pcq_Stop_wBusy);
            UsbhUnlockPcq(HubFdo, PortData, irql);
            break;

        default:
            UsbhSetPcqState(HubFdo, PortData,  Pcq_Error);
            UsbhAssertFailure(HubFdo, Context); 
            UsbhUnlockPcq(HubFdo, PortData, irql);
            break;
        } 
    }        

    return le;
}


PVOID
Usbh_PCE_Check_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context,
    PULONG Parameter
    )
/*++

Routine Description:

    Handle PCE_Check event for all states

    called by UsbhPCE_Check

Arguments:

    Parameter -- returns the state of the queue, non-zero if stopped

Irql: ANY

Return Value:

    none
    

--*/
{

    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;
    USHORT portNumber;
    
    hubExt = FdoExt(HubFdo);

    *Parameter = 0;
    
    UsbhAssert(HubFdo, PortData->Sig == SIG_HUBPORTDATA);
    portNumber = PN(PortData);

    LOG(HubFdo, LOG_PCQ, '15p0', PortData, PortData->PortNumber);
   
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p >\n", PN(PortData), Context));
    UsbhAssert(HubFdo, PortData->PortNumber == portNumber);

    // dequeue and process changes
    state = UsbhLockPcq(HubFdo, PortData, PCE_Check, &irql);

    switch(state) {
    case Pcq_Stop:
    case Pcq_Stop_Suspended: 
    case Pcq_Stop_wBusy:   
    case Pcq_Stop_wBusy_Signal:              
    case Pcq_Stop_wQueued:      
    case Pcq_HW_Stop_wBusy:   
    case Pcq_HW_Stop_wQueued:            
        // queue is stopped
        *Parameter = 1;
        UsbhSetPcqState(HubFdo, PortData,  state);
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;
        
    default:
        // other states return zero (busy)
        UsbhSetPcqState(HubFdo, PortData,  state);
        *Parameter = 0;
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;
    } 

    return Parameter;
   
}


VOID
Usbh_PCE_psPAUSED_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_psPAUSED event for all states

    called by UsbhPCE_psPAUSED

Arguments:

Irql: ANY

Return Value:

    none   

--*/
{

    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;
    USHORT portNumber;
    PDEVICE_OBJECT pdo;
    
    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, PortData->Sig == SIG_HUBPORTDATA);
    portNumber = PN(PortData);

    LOG(HubFdo, LOG_PCQ, '19p0', PortData, PortData->PortNumber);
   
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p >\n", PN(PortData), Context));
    UsbhAssert(HubFdo, PortData->PortNumber == portNumber);

    state = UsbhLockPcq(HubFdo, PortData, PCE_psPAUSED, &irql);

    switch(state) {

    case Pcq_HW_Stop_wQueued: 
    case Pcq_Stop_wQueued: 
    case Pcq_Stop_wBusy:     
    case Pcq_Stop:        
        // if a driver reset is still pending, attempt to release it here.

        UsbhSetPcqState(HubFdo, PortData,  state);
        UsbhSignalPauseEvent(HubFdo, PortData);

        UsbhFlushPortChangeQueue(HubFdo, PortData);      

        UsbhUnlockPcq(HubFdo, PortData, irql);

       
        // this may return NULL if the PDO has been disconnected or not created yet
        pdo = UsbhLatchPdo(HubFdo, PortData->PortNumber, NULL, PdoBusLatch_Tag);
        LOG(HubFdo, LOG_PCQ, '19pa', PortData, pdo);

        if (pdo) {
            KIRQL reset_irql;
            BOOLEAN signal_reset = FALSE;
            
            PortData->PdoLatched = 1;
            
            // see if a driver reset is in progress
            KeAcquireSpinLock(&hubExt->SyncDeviceResetSpin, &reset_irql);
            if (PdoExt(pdo)->SyncDeviceResetEvent) {

               
                signal_reset = TRUE;

                LOG(HubFdo, LOG_BUS, '19pb', pdo, 0);
                
                KeReleaseSpinLock(&hubExt->SyncDeviceResetSpin, reset_irql);

                // attempt to cancel the reset timeout DPC if there is one
                UsbhCancelResetTimeout(HubFdo, PortData, CANCEL_RESET_IN_PROGRESS);

                 //PCQ_TRAP();
                // note: the queued reset if riunning will be in the context of the PortData structure PDC(PortData).
                //
                // PortData-CurrentSC is NULL here since it is only set when the the queue is processing changes for 
                // functions in (connect,enum1, and enum2). We need release the buslock held by any reset request that 
                // is in progress for this PortData structure so we use PDC(PortData).
                
                UsbhAssert(HubFdo, PDC(PortData) == Context);
                // release the enum lock only if we hold it
                UsbhReleaseEnumBusLockEx(HubFdo, Context, PortData->PortNumber, FALSE);

            } else {
                KeReleaseSpinLock(&hubExt->SyncDeviceResetSpin, reset_irql);
            }

            if (signal_reset) {
                UsbhSignalSyncDeviceReset(HubFdo, pdo, PortData, STATUS_UNSUCCESSFUL);
            }                
            // NOTE: once the PDO is unlatched we must take care not to access
            // it further
            BUS_UNLATCH_PDO(HubFdo, pdo, PortData);
        }

       
        break;
    
    default:
        UsbhFlushPortChangeQueue(HubFdo, PortData);      

        switch (state) {
            case Pcq_Suspend_wBusy_Signal:
                UsbhSetPcqState(HubFdo, PortData,  Pcq_Suspend_wBusy);
                break;
   
            case Pcq_Stop_wBusy_Signal:
                UsbhSetPcqState(HubFdo, PortData,  Pcq_Stop_wBusy);
                break;
            
            case Pcq_Run_wBusy_Signal:
                UsbhSetPcqState(HubFdo, PortData,  Pcq_Run_wBusy);
                break;
        }
        
        UsbhSignalPauseEvent(HubFdo, PortData);
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }

    
}


VOID
Usbh_PCE_wChangeERROR_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context,
    USB_HUB_PORT_EVENT PortEventData,
    PCQ_OBJECT_TYPE PcqObjectType,
    PVOID PcqObject
    )
/*++

Routine Description:

    Handle PCE_wChangeERROR event for all states

    called by UsbhPCE_wChangeERROR

    This event is really just used for error tracking and logging. It is called when PCE_wChange 
    detects an error and pauses the queue. This can result in the hub hanging or bugcheck 0x9f up
    on the PCE_resume or PCE_Suspend events.

Arguments:

Irql: ANY

Return Value:

    none   

--*/
{

    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;
    USHORT portNumber;
    
    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, PortData->Sig == SIG_HUBPORTDATA);
    portNumber = PN(PortData);

    LOG(HubFdo, LOG_PCQ, '23p0', PortData, PortData->PortNumber);
   
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p >\n", PN(PortData), Context));
    UsbhAssert(HubFdo, PortData->PortNumber == portNumber);

    state = UsbhLockPcq(HubFdo, PortData, PCE_wChangeERROR, &irql);

    switch(state) {

    case Pcq_Stop:   
    default:        
        //
        // if we pause the queue we are probably preparing for a remove -- release any threads waiting on
        // the resume here.
        //
        UsbhUnlockPcq(HubFdo, PortData, irql);
        
        // if the event is a resume, signal resume
        if (PortEventData.RequestPortResume) {
            UsbhSignalResumeEvent(HubFdo, PortData, FALSE);
        }    

        if (PortEventData.RequestPortSuspend) {
            UsbhSignalSuspendEvent(HubFdo, PortData, FALSE);
        } 

        if (PortEventData.DriverResetChange) {
            PDEVICE_OBJECT pdo;

            UsbhAssert(HubFdo, PcqObject != NULL);
            pdo = PcqObject;
            LOG(HubFdo, LOG_PCQ, '23p1', pdo, PortData->PortNumber);
   
            UsbhSignalSyncDeviceReset(HubFdo, pdo, PortData, NT_DEVICE_DISCONNECTED_STATUS);
        } 
        
        break;
    }

    
}



VOID
Usbh_PCE_psSUSPEND_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_psSUSPEND event for all states

    called by UsbhPCE_psSUSPEND

Arguments:

Irql: ANY

Return Value:

    none   

--*/
{

    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;
    USHORT portNumber;
    
    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, PortData->Sig == SIG_HUBPORTDATA);
    portNumber = PN(PortData);

    LOG(HubFdo, LOG_PCQ, '18p0', PortData, PortData->PortNumber);
   
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p >\n", PN(PortData), Context));
    UsbhAssert(HubFdo, PortData->PortNumber == portNumber);

    state = UsbhLockPcq(HubFdo, PortData, PCE_psSUSPEND, &irql);

    switch(state) {
    
    default:
        UsbhFlushPortChangeQueue(HubFdo, PortData);      
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }

    
}


VOID
Usbh_PCE_SD_Resume_Action(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT Context
    )
/*++

Routine Description:

    Handle PCE_SD_Resume event for all states

    called by UsbhPCE_SD_Resume

    This ahndles a port resume associated with a soft-disconnect.

Arguments:

Irql: ANY

Return Value:

    none   

--*/
{

    HUB_PCQ_STATE state;
    PDEVICE_EXTENSION_HUB hubExt;
    KIRQL irql;
    USHORT portNumber;
    
    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, PortData->Sig == SIG_HUBPORTDATA);
    portNumber = PN(PortData);

    LOG(HubFdo, LOG_PCQ, '18p0', PortData, PortData->PortNumber);
   
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p >\n", PN(PortData), Context));
    UsbhAssert(HubFdo, PortData->PortNumber == portNumber);

    state = UsbhLockPcq(HubFdo, PortData, PCE_SD_Resume, &irql);
    
    switch(state) {

    
    case Pcq_Pause:
    case Pcq_Stop:

        UsbhAssert(HubFdo, PortData->CurrentState != PS_SUSPENDED);
        LOG(HubFdo, LOG_PCQ, '18p1', state, PN(PortData));

        UsbhFlushPortChangeQueue(HubFdo, PortData);
        
        // return to connect/disconnect wait state
        PortData->CurrentState = PS_WAIT_CONNECT;
        UsbhSetPcqState(HubFdo, PortData,  Pcq_Run_Idle);

        UsbhSetPcqEventStatus(HubFdo, PortData, PcqEvChDone, PcqEv_Reset);
        RESET_EVENT(&PortData->PortChangeListDone);
       
        UsbhUnlockPcq(HubFdo, PortData, irql);
        break;

    
        
    default:
        UsbhAssert(HubFdo,  PortData->CurrentState != PS_SUSPENDED);
        UsbhSetPcqState(HubFdo, PortData,  state);
        //UsbhAssertFailure(HubFdo, Context);
        UsbhUnlockPcq(HubFdo, PortData, irql);
    }


    
}




NTSTATUS
UsbhInsertQueuePortChangeObject(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PUSB_HUB_PORT_EVENT PortEvent,
    PCQ_OBJECT_TYPE PcqObjectType,
    PVOID PcqObject
    )
/*++

Routine Description:

    Queue the change for processing by the hub state machine.
    The change lock must be Acquired before calling this function,
    this is used internally by the event dispatch function.

Irql: DPC

Arguments:

    HubFdo - Hub in question

    PortNumber - (1) based port number on the hub

    PortEvent - pointer to event to insert in queue.

    PcqObject - optional object associated with the request.

Return Value:

    nt status, error if the change buffer cannot be allocated.

    NOTE: the calling routines in this case will retry to queue the
        same message again so we take no action other than to report
        the error.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PPORT_CHANGE_CONTEXT pcData;
    PHUB_PORT_DATA pd;
    NTSTATUS nts = STATUS_INSUFFICIENT_RESOURCES;

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!>\n", PortNumber));
    hubExt = FdoExt(HubFdo);

    UsbhAllocatePool_Z(pcData, NonPagedPool, sizeof(struct _PORT_CHANGE_CONTEXT));

    if (pcData) {
        pd = UsbhGetPortData(HubFdo, PortNumber);

        if (pd) {
            UsbhAssert(HubFdo, PortEvent->ul != 0);
            UsbhAssertPcqLock(HubFdo, pd);

            pcData->PortNumber = PortNumber;
            pcData->PortEvent.ul = PortEvent->ul;
            pcData->Sig = SIG_PCDATA;
            pcData->FrameInserted = Usbh_UsbdReadFrameCounter(HubFdo);
            pcData->Object = PcqObject;
            pcData->ObjectType = PcqObjectType;

            LOG(HubFdo, LOG_PCQ, 'qPCD', pd, pcData->PortEvent.ul);
            DbgTrace((UsbhDebugTrace,"#%d Q-Insert-Change :%08.8x\n",
                PortNumber, pcData->PortEvent.ul));

            nts = STATUS_SUCCESS;
            
            if (PcqObjectType == PcqObj_Pdo) {

                PDEVICE_OBJECT pdo;

                pdo = UsbhLatchPdo(HubFdo,
                                   PortNumber,
                                   pcData,
                                   PdoLatchInsertQueuePortChange);

                 //UsbhAssert(NULL, pdo == PcqObject);
                if (pdo == NULL) {
                    // don't queue the request if we fail to latch
                    nts = STATUS_INVALID_PARAMETER;                   

                } else if (pdo != PcqObject) {
                    // Don't queue the request if we latch a different
                    // PDO, and unlatch the different PDO since there
                    // will be no request queued to unlatch the PDO
                    // later.
                    nts = STATUS_INVALID_PARAMETER;                   

                    UsbhUnlatchPdo(HubFdo,
                                   pdo,
                                   pcData,
                                   PdoLatchInsertQueuePortChange);
                }
            }

            if (NT_SUCCESS(nts)) {
                if ((pcData->PortEvent.SuspendChange == 1) || 
                    (pcData->PortEvent.ResetChange == 1) ||
                    (pcData->PortEvent.EnableChange == 1)) {
                    InterlockedIncrement(&pd->QueuedPortChangeCount);
                }
                
                if (pcData->PortEvent.ConnectChange != 1) {                
                    InsertTailList(&pd->PortChangeList,
                                   &pcData->ChangeLink);

                    // Notify any waiting thread that there are 
                    // port change events to be processed.
                    SET_EVENT(&pd->PortChangeListInsertEvent);
                }
                // Special case for connect changes.  Check if there is already one in the queue,
                // in which case we can ignore this one.  This will throttle change changes and
                // prevent connect change storms from overwhelming the port change queue.
                else if (!pd->ConnectChangeQueued) {
                    pd->ConnectChangeQueued = TRUE;
                    InsertTailList(&pd->PortChangeList,
                                   &pcData->ChangeLink);

                    // Notify any waiting thread that there are 
                    // port change events to be processed.
                    SET_EVENT(&pd->PortChangeListInsertEvent);
                } else {
                    // Event not inserted the queue, 
                    // so free the port change context
                    UsbhFreePool(pcData);
                }
            } else {
                 UsbhFreePool(pcData);
            }

        } else {
            UsbhFreePool(pcData);

            nts = STATUS_INVALID_PARAMETER;
        }
    }

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC! %!STATUS!<\n", PortNumber, nts));

    return nts;
}


NTSTATUS
UsbhInsertQueuePortChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PUSB_HUB_PORT_EVENT PortEvent
    )
/*++

Routine Description:

    See UsbhInsertQueuePortChangeObject

    calls UsbhInsertQueuePortChangeObject with NULL object

Irql: DPC

Arguments:

Return Value:

    nt status, error if the change buffer cannot be allocated.

--*/
{
    return UsbhInsertQueuePortChangeObject(HubFdo, PortNumber, PortEvent, PcqObj_NoType, NULL);

}


VOID
UsbhInitPortChangeList(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA Pd
    )
/*++

Routine Description:

    Initializes the change engine for a port.

    list is initially in -> Pcq_Pause

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    LOG(HubFdo, LOG_PCQ, 'inPC', 0, PN(Pd));
    DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! \n", PN(Pd)));
    hubExt = FdoExt(HubFdo);

    INIT_SYNC_EVENT_SIGNALED(&Pd->EnableDisableLock);
    INIT_SYNC_EVENT_SIGNALED(&Pd->PortChangeQueueLock);
    INIT_EVENT_SIGNALED(&Pd->PortChangeListDone);
    INIT_EVENT_NOT_SIGNALED(&Pd->PortChangeListInsertEvent);
    INIT_EVENT_SIGNALED(&Pd->PortPausedEvent);
    // these event toggle with the software suspend resume operations
    INIT_EVENT_SIGNALED(&Pd->PortResumeEvent);
    INIT_EVENT_SIGNALED(&Pd->PortSuspendEvent);
    INIT_EVENT_SIGNALED(&Pd->DriverResetEvent);

    INIT_SYNC_EVENT_SIGNALED(&Pd->OvercurrentLock);

    Pd->PortHwPendingState = PortHw_Waiting;
    Pd->PcLogEntrySize = sizeof(struct _PORT_CHANGE_LOG_ENTRY);
        
    KeInitializeSpinLock(&Pd->PortChangeListSpin);
    Pd->PortChangeListLock = 0;
    InitializeListHead(&Pd->PortChangeList);
  
    Pd->QueuedPortChangeCount = 0;
    Pd->ConnectChangeQueued = FALSE;

    // initial state is pause
    Pd->Pcq_State = Pcq_Pause;

}


PPORT_CHANGE_LOG_ENTRY
Usbh_PcLogEntry(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PVOID Pdo,
    ULONG PortEventUlong,
    ULONG FrameInserted,
    USHORT HwPortStatus,
    UCHAR PortStatusBits,
    UCHAR FlushFlag
    )
/*++

Routine Description:

   Advances to the next entry and initializes an enum state log entry.

Irql: DPC

Arguments:

    PcData - PcData to log this entry

Return Value:

    pointer to the 'current entry'

--*/
{
    PPORT_CHANGE_LOG_ENTRY pcLog;

    // update the log so we know what we threw away.
    PortData->PcLogIdx=(PortData->PcLogIdx+1) % PORT_ENUM_LOG_LEN;
    pcLog = &PortData->PcLog[PortData->PcLogIdx];

    pcLog->CurrentState = PortData->CurrentState;
    pcLog->Change = PortEventUlong;
    pcLog->PortStatusBits = PortStatusBits;
    pcLog->HwPortStatus = HwPortStatus;
    pcLog->Pdo = Pdo;
    pcLog->FlushFlag = FlushFlag;
    pcLog->UsbTimeIdx = FrameInserted;
    pcLog->ValidEntry = 1;
    if (FlushFlag == 0) {
        pcLog->FrameAge = Usbh_UsbdReadFrameCounter(HubFdo) - FrameInserted;
    }

    return pcLog;

}


VOID
UsbhFreePortChangeQueueTimeoutObject(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PPCQ_TIMEOUT_OBJECT TmoObj,
    BOOLEAN SignalCompletionEvent
    )
/*++

Routine Description:

   frees the reset timeout change

Irql: DPC

Arguments:

Return Value:

    none
    
--*/    
{

    KIRQL rs_irql;
    PDEVICE_EXTENSION_HUB  hubExt;

    LOG(HubFdo, LOG_PCQ, 'ftPC', 0, PN(PortData));
    DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! \n", PN(PortData)));
    
    hubExt = FdoExt(HubFdo);
    
    KeAcquireSpinLock(&hubExt->PortResetSpin, &rs_irql);
    // this object is inserted in the queue when the timeout DPC fires so we assert the hw state on the port.
    UsbhAssert(HubFdo, PortData->PortHwPendingState == PortHw_TimeoutPending);
    
    // this should be true if the timeout is in the queue,
    // if it is not we missed resetting this field or we missed removing an old timeout from the queue.
    UsbhAssert(HubFdo, PortData->ResetTimerPending == TmoObj);
   
    PortData->ResetTimerPending = NULL;
    PortData->PortHwPendingState = PortHw_Waiting;

    SET_EVENT(&PortData->ResetTimeoutEvent);
    KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);

    if ( (PortData->PnpPdo) && (SignalCompletionEvent) ){
        Usbh_iSignalSyncDeviceReset(HubFdo, (PDEVICE_OBJECT)PortData->PnpPdo, PortData, STATUS_UNSUCCESSFUL);
    }

    UsbhFreePool(TmoObj);
    
}


VOID
UsbhFreePortChangeQueueObject(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PPORT_CHANGE_CONTEXT PcData,
    BOOLEAN SignalCompletionEvent
    )
/*++

Routine Description:

    Flush(free) a specific change (pcData) object

Irql: DPC

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PPORT_CHANGE_LOG_ENTRY pcLog;

    LOG(HubFdo, LOG_PCQ, 'uiPC', 0, PN(PortData));
    DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! \n", PN(PortData)));
    hubExt = FdoExt(HubFdo);

    UsbhAssertPcqLock(HubFdo, PortData);
    
    UsbhAssert(HubFdo, PcData->Sig == SIG_PCDATA);
    UsbhAssert(HubFdo, PortData->PortNumber == PcData->PortNumber);

    // update the log so we know what we threw away.
    pcLog = Usbh_PcLogEntry(HubFdo,
                PortData,
                NULL,
                PcData->PortEvent.ul,
                0,
                PortData->CurrentPortStatus.us,
                0,
                1);

    if (SignalCompletionEvent) {
        if (PcData->PortEvent.ul == SUSPEND_CHANGE_BIT) {
            //test #157
            UsbhiSignalResumeEvent(HubFdo, PortData, FALSE);
        }


        if (PcData->PortEvent.ul == REQUEST_RESUME_BIT) {
            // should not be seeing this
            TEST_TRAP();
            //UsbhiSignalResumeEvent(HubFdo, PortData);
        }

        if (PcData->PortEvent.ul == PAUSE_CHANGE_BIT) {
            // we need to signal pause if we process the
            // pause event.
            UsbhSignalPauseEvent(HubFdo, PortData);
        }

        if (PcData->PortEvent.ul == SUSPEND_CHANGE_BIT) {
       
            UsbhiSignalSuspendEvent(HubFdo, PortData, FALSE);
        }
    }

    // process any objects in the queue when we flush
    switch (PcData->ObjectType) {
    case PcqObj_NoType:
        UsbhAssert(HubFdo, PcData->Object == NULL);
        break;

    case PcqObj_Timeout:
       
        {
        PPCQ_TIMEOUT_OBJECT tmoObj;

        tmoObj = PcData->Object;
        PcData->Object = NULL;
        
        UsbhFreePortChangeQueueTimeoutObject(HubFdo, PortData, tmoObj, SignalCompletionEvent);
       
        }
        break;

    case PcqObj_Pdo:
        {
        PDEVICE_OBJECT pdo;

        pdo = PcData->Object;
        // calling PdoExt() validates the PDO which is what we want
        UsbhAssert(HubFdo, PdoExt(pdo)->SyncDeviceResetEvent);
        if (SignalCompletionEvent) {
            Usbh_iSignalSyncDeviceReset(HubFdo, pdo, PortData, STATUS_UNSUCCESSFUL);
        }
        
        }
        break;
    }

    if (PcData->ObjectType == PcqObj_Pdo) {

        PDEVICE_OBJECT pdo;

        pdo = PcData->Object;

        UsbhUnlatchPdo(HubFdo,
                       pdo,
                       PcData,
                       PdoLatchInsertQueuePortChange);
    }

    LOG(HubFdo, LOG_PCQ, 'frPD', PortData, PcData);

    UsbhFreePool(PcData);

}

VOID
UsbhFlushPortChangeQueue(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    )
/*++

Routine Description:

    Converse of UsbhInitPortChangeList.  This is called when the changelist is
    in 'Stop' to free any resources associated with it.

    Since changes can be queued before init this routine ensures changes are freed
    in the event the list is never enabled. This is also an optimization that dumps
    the changes when we shutdown,unload or otherwise abandon the HW signalling events
    as appropriate.

    The pcq lock must be acquired before calling this routine.

Irql: DPC

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PLIST_ENTRY le;
    PPORT_CHANGE_CONTEXT pcData;
    
    DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! \n", PN(PortData)));
    hubExt = FdoExt(HubFdo);

    UsbhAssertPcqLock(HubFdo, PortData);
    
    while (!IsListEmpty(&PortData->PortChangeList)) {

        le = RemoveHeadList(&PortData->PortChangeList);

        pcData = CONTAINING_RECORD(le,
                                   PORT_CHANGE_CONTEXT,
                                   ChangeLink);

        UsbhAssert(HubFdo, pcData->Sig == SIG_PCDATA);

        UsbhFreePortChangeQueueObject(HubFdo, PortData, pcData, TRUE);
        
    }

    PortData->QueuedPortChangeCount = 0;
    PortData->ConnectChangeQueued = FALSE;
}


VOID
UsbhPCE_Enable(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    )
/*++

Routine Description:

    PCE_Enable(1)

    Enables the change engine for a port. calls Usbh_PCE_Enable_Action

    Normally the queue enters the run or run idle state from here.


Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! \n",  PortNumber));
    LOG(HubFdo, LOG_PCQ, 'PE01', 0, PortNumber);

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        // This will activate the pcq any messages queued will be processed now.

        UsbhDispatch_PortChangeQueueNullEvent(HubFdo, pd, PCE_Enable, StateContext);
    }

}

VOID
UsbhPCE_wRun(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    PHUB_PORT_DATA Pd
    )
/*++

Routine Description:

    PCE_wRun(5)
    Runs the port change queue, calls Usbh_PCE_wRun_Action
    
Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! \n",  Pd->PortNumber));
    
    UsbhDispatch_PortChangeQueueNullEvent(HubFdo, Pd, PCE_wRun, StateContext);

    SET_EVENT(&Pd->PortChangeQueueLock);
}


VOID
UsbhPCE_wDone(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    PHUB_PORT_DATA Pd
    )
/*++

Routine Description:

    PCE_wDone(6)
    
    signals the port change queue is empty, calls Usbh_PCE_wDone_Action

    
Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! \n",  Pd->PortNumber));
    LOG(HubFdo, LOG_PCQ, 'PE06', 0, Pd->PortNumber);
    
    UsbhDispatch_PortChangeQueueNullEvent(HubFdo, Pd, PCE_wDone, StateContext);

}


VOID
UsbhPCE_Resume(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    )
/*++

Routine Description:

    PCE_Resume(3)
    Resumes the change engine for a port.

    This is called on a resume request either by the hub or function driver.

    calls Usbh_PCE_Resume_Action

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! \n",  PortNumber));
    LOG(HubFdo, LOG_PCQ, 'PE03', 0, PortNumber);

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        // 're-pause' the ports before enable, this is a nop if they are already paused
        // if the port is suspended (PS_SUSPENDED) a resume request will be queued so that
        // it will execute on the next transition to run.
        UsbhDispatch_PortChangeQueueNullEvent(HubFdo, pd, PCE_Resume, StateContext);
    }

}


VOID
UsbhPCE_ResumeDone(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    )
/*++

Routine Description:

    PCE_ResumeDone(21)
    Called to indicate WaitResume has completed.  This is used if a suspend request is made before the 
    current resume request is completed.  This is only called by the Suspend thread that is blocled on the 
    pending resume.
   
    calls Usbh_PCE_ResumeDone_Action

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! \n",  PortNumber));
    LOG(HubFdo, LOG_PCQ, 'PE21', 0, PortNumber);

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
       
        UsbhDispatch_PortChangeQueueNullEvent(HubFdo, pd, PCE_ResumeDone, StateContext);
    }

}



VOID
UsbhPCE_ResumeTimeout(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    )
/*++

Routine Description:

    PCE_ResumeTimeout(14)

    calls Usbh_PCE_ResumeTimeout_Action
     
    Timeout waiting on a port resume.  This routine evaluates the state of the hardware and
    signals the resume event if appropriate.

    This is necessary to properly time out the resume under stress xonditions where the system
    may be running very slowly.

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! \n",  PortNumber));
    LOG(HubFdo, LOG_PCQ, 'PE14', 0, PortNumber);

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        // 're-pause' the ports before enable, this is a nop if they are already paused
        // if the port is suspended (PS_SUSPENDED) a resume request will be queued so that
        // it will execute on the next transition to run.
        UsbhDispatch_PortChangeQueueNullEvent(HubFdo, pd, PCE_ResumeTimeout, StateContext);
    }

}


VOID
UsbhPCE_HardwareWake(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    )
/*++

Routine Description:

    PCE_HardwareWake(12)
    Resumes the change engine for a port.

    This is called on a resume request either by the hub or function driver.

    calls Usbh_PCE_HardwareWake_Action

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! \n",  PortNumber));
    LOG(HubFdo, LOG_PCQ, 'PE12', 0, PortNumber);

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        // 're-pause' the ports before enable, this is a nop if they are already paused
        // if the port is suspended (PS_SUSPENDED) a resume request will be queued so that
        // it will execute on the next transition to run.
        UsbhDispatch_PortChangeQueueNullEvent(HubFdo, pd, PCE_HardwareWake, StateContext);
    }

}


VOID
UsbhPCE_BusReset(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    )
/*++

Routine Description:

    PCE_BusReset(9)
    
    Reset the port change list to pause state -- we use this for restart
    and error recovery.

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! \n",  PortNumber));
    LOG(HubFdo, LOG_PCQ, 'PE09', 0, PortNumber);


    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        UsbhDispatch_PortChangeQueueNullEvent(HubFdo, pd, PCE_BusReset, StateContext);
    }

}


NTSTATUS
UsbhPCE_Suspend(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    )
/*++

Routine Description:

    PCE_Suspend(10)

    Transitions the queue and assocaited USB port to the suspend state.

    calls Usbh_PCE_Suspend_Action
   
Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;
    NTSTATUS nts = STATUS_DEVICE_NOT_CONNECTED;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! \n",  PortNumber));
    LOG(HubFdo, LOG_PCQ, 'PE10', 0, PortNumber);


    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        UsbhDispatch_PortChangeQueueNullEventEx(HubFdo, pd, PCE_Suspend, StateContext, &nts);
    }
    return nts;
}


VOID
UsbhPCE_BusDisconnect(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext,
    USHORT PortNumber
    )
/*++

Routine Description:

    PCE_BusDisconnect(11)
    
    Disconnect PDO from the port.

    calls Usbh_PCE_BusDisconnect_Action

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! \n",  PortNumber));
    LOG(HubFdo, LOG_PCQ, 'PE11', 0, PortNumber);

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        UsbhDispatch_PortChangeQueueNullEvent(HubFdo, pd, PCE_BusDisconnect, StateContext);
    }

}



VOID
UsbhPCE_Disable(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    PCE_Disable(2)

    Disables the change list, waits for the queues to drain.

    calls Usbh_PCE_Disable_Action
    
Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p \n", PortNumber, StateContext));
    LOG(HubFdo, LOG_PCQ, 'PE02', 0, PortNumber);

    UsbhAssertPassive();

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        PDEVICE_OBJECT pdo;

        UsbhDispatch_PortChangeQueueNullEvent(HubFdo, pd, PCE_Disable, StateContext);

        pdo = UsbhLatchPdo(HubFdo, pd->PortNumber, NULL, PdoBusLatch_Tag);

        if (pdo) {
            //Insure that enumeration is cancelled on the port
            pd->PdoLatched = 1;
            pd->CurrentSc = PDC(pd);

            UsbhCancelEnumeration(HubFdo, pd->CurrentState, pd, pdo);

            pd->CurrentSc = NULL;
            
            if (pd->PdoLatched) {
                BUS_UNLATCH_PDO(HubFdo, pdo, pd);
            } else {
                // Enumeration was in-progress and cancelled.  Return port state to PS_WAIT_CONNECT so
                // device will enumerate on resume.
                pd->CurrentState = PS_WAIT_CONNECT;
            }
        }
    }
}


VOID
UsbhPCE_Close(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    PCE_Close(22)

    Called when hub is going to unload, asserts that queues are empty and not running

    calls Usbh_PCE_Close_Action
    
Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p \n", PN(PortData), StateContext));
    LOG(HubFdo, LOG_PCQ, 'PE22', 0, PN(PortData));

    UsbhAssertPassive();

    UsbhDispatch_PortChangeQueueNullEvent(HubFdo, PortData, PCE_Close, StateContext);

    // make sure we cleaned up. Release lock if we own it.
    UsbhReleaseEnumBusLockEx(HubFdo, PDC(PortData), PortData->PortNumber, FALSE);
}



VOID
UsbhPCE_HW_Stop(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    PCE_HW_Stop(17)

    Disables the change list, waits for the queues to drain.

    calls Usbh_PCE_HW_Stop_Action
    
Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p \n", PortNumber, StateContext));
    LOG(HubFdo, LOG_PCQ, 'PE17', 0, PortNumber);

    UsbhAssertPassive();

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        UsbhDispatch_PortChangeQueueNullEvent(HubFdo, pd, PCE_HW_Stop, StateContext);
    }

    // make sure we cleaned up. Release lock if we own it.
    UsbhReleaseEnumBusLockEx(HubFdo, StateContext, PortNumber, FALSE);
}


VOID
UsbhPCE_SD_Resume(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    PCE_SD_Resume(13)
    
    Restarts the pcq after a soft disconnect.

    calls Usbh_PCE_SD_Resume_Action
    
Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p \n", PN(PortData), StateContext));
    LOG(HubFdo, LOG_PCQ, 'PE13', 0, PN(PortData));

    UsbhAssertPassive();
    
    UsbhDispatch_PortChangeQueueNullEvent(HubFdo, PortData, PCE_SD_Resume, StateContext);

  
}

VOID
UsbhPCE_psSUSPEND(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    PCE_psSUSPEND(18)

    Restarts the pcq after a soft disconnect.

    calls Usbh_PCE_psSUSPEND_Action
    
Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p \n", PN(PortData), StateContext));
    LOG(HubFdo, LOG_PCQ, 'PE18', 0, PN(PortData));

    UsbhAssertPassive();
    
    UsbhDispatch_PortChangeQueueNullEvent(HubFdo, PortData, PCE_psSUSPEND, StateContext);
   
}


VOID
UsbhPCE_psPAUSED(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    PCE_psPAUSED(19)
    
    signals the hardware event queue has entered PS_PAUSED.

    calls Usbh_PCE_psPAUSED_Action
    
Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p \n", PN(PortData), StateContext));
    LOG(HubFdo, LOG_PCQ, 'PE19', 0, PN(PortData));

    UsbhAssertPassive();
    
    UsbhDispatch_PortChangeQueueNullEvent(HubFdo, PortData, PCE_psPAUSED, StateContext);
  
}


VOID
UsbhPCE_wChangeERROR(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    USB_HUB_PORT_EVENT PortEventData,
    PSTATE_CONTEXT StateContext,
    PCQ_OBJECT_TYPE PcqObjectType,
    PVOID PcqObject
    )
/*++

Routine Description:

    PCE_wChangeERROR(23)
    
    calls Usbh_PCE_wChangeERROR_Action
    
Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p \n", PN(PortData), StateContext));
    LOG(HubFdo, LOG_PCQ, 'PE23', 0, PN(PortData));

    UsbhAssertPassive();
    
    UsbhDispatch_PortChangeQueueEvent(HubFdo, PortData, PCE_wChangeERROR, StateContext, PortEventData, PcqObjectType,
        PcqObject);
   
}


ULONG
UsbhPCE_Check(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    PCE_Check(20)

    Checks the status of the queue.

    calls Usbh_PCE_Check_Action
    
Irql: PASSIVE

Arguments:

Return Value:

    non-zero if queue is stopped (not processing requests)

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    ULONG q_state = 0; // report running
    PVOID parameter;
    
    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p \n", PN(PortData), StateContext));
    LOG(HubFdo, LOG_PCQ, 'PE20', 0, PN(PortData));

    UsbhAssertPassive();
    //PCQ_TRAP();
    
    parameter = UsbhDispatch_PortChangeQueueNullEventEx(HubFdo, PortData, PCE_Check, StateContext, &q_state);
    UsbhAssert(HubFdo, parameter == &q_state);
        
    return q_state;
}


PVOID
UsbhPCE_wChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    PCE_wChange(16)

    calls Usbh_PCE_wChange_Action
    
Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;
    PLIST_ENTRY le = NULL;
    
    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p \n", PortNumber, StateContext));
    LOG(HubFdo, LOG_PCQ, 'PE16', 0, PortNumber);

    UsbhAssertPassive();

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        le = UsbhDispatch_PortChangeQueueNullEventEx(HubFdo, pd, PCE_wChange, StateContext, &le);
    }

    return le;
}



VOID
UsbhPCE_QueueChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    USB_HUB_PORT_EVENT PortEventData,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    PCE_QueueChange(4)
    
    Queues changes for a port from the bus engine, the hubs interrupt pipe, internal requests
    or function drivers.

    calls Usbh_PCE_QueueChange_Action

Irql: ANY

Arguments:

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;

    hubExt = FdoExt(HubFdo);

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! >\n",  PortNumber));

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        UsbhDispatch_PortChangeQueueEvent(HubFdo, pd, PCE_QueueChange, StateContext, PortEventData,
            PcqObj_NoType, NULL);
    }

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!<\n", PortNumber));
}


VOID
UsbhPCE_QueueChangeObject(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    USB_HUB_PORT_EVENT PortEventData,
    PSTATE_CONTEXT Sc,
    PCQ_OBJECT_TYPE PcqObjectType,
    PVOID PcqObject
    )
/*++

Routine Description:

    Queues changes for a port from the bus engine, the hubs interrupt pipe, internal requests
    or function drivers.

    calls Usbh_PCE_QueueChange_Action

Irql: ANY

Arguments:

    PortNumber - (1) based port number

    PortEventData - event data, this is the vent we are inserting.

    Sc - context for the request.

    PcqObjectType - object type we are inserting

    PcqObject - object to associate with the change.  This will be linked to the event in the queue

Return Value:

    none.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;

    hubExt = FdoExt(HubFdo);

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! >\n",  PortNumber));

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        UsbhDispatch_PortChangeQueueEvent(HubFdo,
                                          pd,
                                          PCE_QueueChange,
                                          Sc,
                                          PortEventData,
                                          PcqObjectType,
                                          PcqObject);
    }

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!<\n", PortNumber));
}


VOID
UsbhQueuePauseChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

   Queues a PAUSE message to the specified port.

Irql: PASSIVE

Arguments:

Return Value:

    none.

--*/
{
    USB_HUB_PORT_EVENT pev;
    PHUB_PORT_DATA pd;

    LOG(HubFdo, LOG_PCQ, 'iww1', StateContext, PortNumber);
    // we have been unplugged, take us back to PS_WAIT_CONNECT but
    // don't bother trying to talk to the hub anymore.
    pev.ul = 0;
    pev.PauseChange = 1;

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! >\n",  PortNumber));

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        UsbhDispatch_PortChangeQueueEvent(HubFdo, pd, PCE_QueueChange,
            StateContext, pev, PcqObj_NoType, NULL);
    }

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!<\n", PortNumber));
}


VOID
UsbhPCE_QueueDriverReset(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    USB_HUB_PORT_EVENT PortEventData,
    PSTATE_CONTEXT Sc,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

   Inserts a driver reset (internal and external) into the change queue.  By passing 
   it thru the change enigine we can prevent driver resets from being queued at bad 
   times.

   calls Usbh_PCE_QueueDriverReset_Action

Irql: PASSIVE

Arguments:

    PortNumber - (1) based port number

    PortEventData - event data, this is the vent we are inserting.

    Sc - context for the request.

    Pdo - object to associate with the change, in this case teh PDO.  

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! \n",  PortNumber));
    LOG(HubFdo, LOG_PCQ, 'toPR', 0, PortNumber);

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        UsbhDispatch_PortChangeQueueEvent(HubFdo, pd, PCE_QueueDriverReset, Sc, 
            PortEventData, PcqObj_Pdo, Pdo);
    }
}


VOID
UsbhPCE_SoftDisconnect(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

   Queues a SOFT DISCONNECT message to the specified port, this is used when Pnp decides
   a device is disconnected but it really is not.

   calls Usbh_PCE_SoftDisconnect_Action

Irql: PASSIVE

Arguments:

Return Value:

    none.

--*/
{
    PHUB_PORT_DATA pd;
    USB_HUB_PORT_EVENT pev;

    UsbhAssertPassive();
    LOG(HubFdo, LOG_PCQ, 'sfd', StateContext, PortNumber);

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! >\n",  PortNumber));
    pev.ul = 0;

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        UsbhDispatch_PortChangeQueueEvent(HubFdo, pd, PCE_SoftDisconnect,
            StateContext, pev, PcqObj_NoType, NULL);
    }

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!<\n", PortNumber));
}



VOID
UsbhQueueSoftConnectChange(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PSTATE_CONTEXT StateContext,
    BOOLEAN DisablePort
    )
/*++

Routine Description:

   Queues a CONNECT message to the specified port.  Can be used to 'simulate' a connection
   on the specified port.

   The caller must specify if the port is to be disabled -- if the calling routine knows that
   the ports are in a known state (like initialization) then the disable call may be skipped.

   The port should always be disabled though.

   This routine is the only rotine that should be used to queue artifical connect changes as
   these can conflict with real connect changes in hardware.  Care should always be taken when
   queueing these.

Irql: PASSIVE

Arguments:

    PortNumber - port number to queue the connect

    StateContext - context request is coming from

    DisablePort - flag to indicate port should be disabled.  A true connect change always shows
            the port as disabled so if we are cycling the port in software we need to disable the
            port as well.

Return Value:

    none.

--*/
{
    USB_HUB_PORT_EVENT pev;
    PHUB_PORT_DATA pd;
    BOOLEAN portDisabled;
    NTSTATUS status;
    USBD_STATUS usbdStatus;

    // queue the change message to the bus engine.
    pev.ul = 0;
    pev.ConnectChange = 1;

    portDisabled = TRUE;
    status = STATUS_SUCCESS;
    usbdStatus = USBD_STATUS_SUCCESS;

    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! >\n",  PortNumber));
    UsbhAssertPassive();

    pd = UsbhGetPortData(HubFdo, PortNumber);
    if (pd) {
        // in order to properly simulate a connect change the port must show disabled
        // so we do this now unless we know it is not necessary
        if (DisablePort) {
            UsbhDisablePort(HubFdo, pd);

            //
            // On root hubs the port status may not show as disabled by the time the disable
            // request completes.  For root ports check the port status
            // to verify the port is disabled, and insert a 10ms delay if it is
            // not.  If the port is still not disabled after the delay, initiate a hard
            // reset of the hub.
            //            
            if (FdoExt(HubFdo)->FdoFlags.HubIsRoot) {
                USB_HUB_PORT_STATE portState;
                    
                //
                // Check if the port is disabled
                //
                status = UsbhQueryPortState(HubFdo,
                                            pd->PortNumber,
                                            &portState,
                                            &usbdStatus);

                if (NT_SUCCESS(status)) {
                    
                    if (portState.Status.Enable == 1) {

                        //
                        // Port is not yet disabled.  Delay for 10ms and check again
                        //
                        UsbhWait(HubFdo, 10);
    
                        status = UsbhQueryPortState(HubFdo,
                                                    pd->PortNumber,
                                                    &portState,
                                                    &usbdStatus);

                        if (!NT_SUCCESS(status) || (portState.Status.Enable == 1)) {
                            portDisabled = FALSE;
                        }
                    }
                } else {
                    portDisabled = FALSE;
                }
            }
        }
        
        if (portDisabled == TRUE) {
            UsbhDispatch_PortChangeQueueEvent(HubFdo, pd, PCE_QueueChange,
                    StateContext, pev, PcqObj_NoType, NULL);
        } else {
            // Port never disabled or an error occurred on the port status request.
            // Queue a reset of the hub.
            HUB_EXCEPTION(USBFILE, HubFdo, pd->PortNumber,
                          HubErr_PortDisableFailed_Reset,
                          status, usbdStatus, NULL, 0);
        }
    }

    DbgTrace((UsbhDebugTrace,"#%d %!FUNC!<\n", PortNumber));
}


VOID
UsbhRemoveQueuedSuspend(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    )
/*++

Routine Description:

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PLIST_ENTRY le;
    PPORT_CHANGE_CONTEXT pcData;

    LOG(HubFdo, LOG_PCQ, 'uiPC', 0, PN(PortData));
    DbgTrace((UsbhDebugTrace,"#%d  %!FUNC! \n", PN(PortData)));
    hubExt = FdoExt(HubFdo);

    UsbhAssertPcqLock(HubFdo, PortData);

    le = PortData->PortChangeList.Flink;
    
    while (le != &PortData->PortChangeList) {

        pcData = CONTAINING_RECORD(le,
                                   PORT_CHANGE_CONTEXT,
                                   ChangeLink);

        UsbhAssert(HubFdo, pcData->Sig == SIG_PCDATA);
        UsbhAssert(HubFdo, PortData->PortNumber == pcData->PortNumber);
        
        if (pcData->PortEvent.ul == REQUEST_SUSPEND_BIT) {
            LOG(HubFdo, LOG_PCQ, 'flSU', PortData, pcData);
            TEST_TRAP();

            RemoveEntryList(le);
            UsbhFreePool(pcData);
            
            UsbhiSignalSuspendEvent(HubFdo, PortData, TRUE);
            break;
        }

        le = le->Flink;
    }

}


ULONG
UsbhHubSyncSuspendPortEvent(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT Sc,
    PHUB_PORT_DATA PortData
    )
/*++

Routine Description:

   Suspends the port synchronously if the change queue is stopped.

   This routine is only be called when the queue is in a stopped or paused state.

Irql: PASSIVE

Arguments:

    PortData - port where event takes place

Return Value:

    the next port state entered by the enum engine.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    UCHAR portStatusBits = 0;
    NTSTATUS nts;
    USBD_STATUS usbdStatus;
    USHORT portNumber;
    PDEVICE_OBJECT pdo;
    PPORT_CHANGE_LOG_ENTRY pcLog;
    USB_HUB_PORT_STATE portState;
    ULONG ps;

    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, PortData->Sig == SIG_HUBPORTDATA);
    portNumber = PN(PortData);

    LOG(HubFdo, LOG_PCQ, 'pev+', PortData, PortData->PortNumber);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p >\n", PN(PortData), 0));
    UsbhAssert(HubFdo, PortData->PortNumber == portNumber);

    ps = PortData->CurrentState;

    do {

        nts = UsbhQueryPortState(HubFdo,
                                 PortData->PortNumber,
                                 &portState,
                                 &usbdStatus);

        if (NT_ERROR(nts)) {
            portState.Status.us = 0;
            portState.Change.us = 0;

            // if this doesn't work leave the port in its current state.
            break;
        }

        // we actually checked this earlier but it may have changed
        if (portState.Status.Connect == 0) {
            // no point if the connect status is clear
            break;
        }

        pdo = UsbhLatchPdo(HubFdo, PortData->PortNumber, NULL, PdoLatchSyncSuspend_Tag);
        if (pdo) {
             portStatusBits = UsbhGetPortStatusBits(HubFdo, PortData, &portState, pdo);
        }

        PortData->CurrentPortStatus = portState.Status;

        pcLog = Usbh_PcLogEntry(HubFdo,
                    PortData,
                    pdo,
                    DIRECT_SUSPEND,
                    0,
                    PortData->CurrentPortStatus.us,
                    portStatusBits,
                    0);
        if (pdo) {
            UsbhUnlatchPdo(HubFdo, pdo, NULL, PdoLatchSyncSuspend_Tag);
        }

        // issue a synchronous suspend and set the port state
        // to 'suspended'
        nts = UsbhSuspendPort(HubFdo, PortData->PortNumber);

        if (NT_ERROR(nts)) {
            // remain in current state -- we may be disconnected
            // jd HUBEX
            break;
        }

        ps = PS_SUSPENDED;

    } WHILE (0);

    return ps;

}


ULONG
UsbhHubDispatchPortEvent(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData,
    PPORT_CHANGE_CONTEXT PcData,
    PUSB_HUB_PORT_STATE PortState
    )
/*++

Routine Description:

    Processes the change request queue for a port.  This routine loops processes one
    event.

Irql: PASSIVE

Arguments:

    PortData - port where event takes place

    PcData - the port event to process

    PortState - current hardware state of the port at the time this routine is called.

Return Value:

    the next port state entered by the enum engine.

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    UCHAR portStatusBits;
    USHORT portNumber;
    PDEVICE_OBJECT pdo;
    PPORT_CHANGE_LOG_ENTRY pcLog;
    PDEVICE_EXTENSION_PDO pdoExt;

    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, PortData->Sig == SIG_HUBPORTDATA);
    portNumber = PN(PortData);

    LOG(HubFdo, LOG_PCQ, 'pev+', PortData, PortData->PortNumber);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p >\n", PN(PortData), 0));
    UsbhAssert(HubFdo, PortData->PortNumber == portNumber);

    UsbhAssert(HubFdo, PcData->Sig == SIG_PCDATA);
    UsbhAssert(HubFdo, PortData->PortNumber == PcData->PortNumber);
    LOG(HubFdo, LOG_PCQ, 'pcPN', PortData->PortNumber, 0);
    LOG(HubFdo, LOG_PCQ, 'pcDD', PortData, PcData);

    // this may return NULL if the PDO has been disconnected or not created yet
    pdo = UsbhLatchPdo(HubFdo, PortData->PortNumber, NULL, PdoBusLatch_Tag);
    if (pdo) {
        // this must only be changed by UsbhPortDisconnect bus routines. The
        // port state routines use this routine to signal this function that they
        // unlatched the PDO
        PortData->PdoLatched = 1;
    }

    // event trapping, we use this to debug
    if (hubExt->TrapEnable &&
        hubExt->TrapPort == (ULONG) PortData->PortNumber &&
        hubExt->TrapEvent == PcData->PortEvent.ul) {

        UsbhKdPrint((0, "<Trapping Port Event> Fdo: %p Pdo %p\n", HubFdo, pdo));
        UsbhKdPrint((0, "Port: %d Event %08.8x\n", hubExt->TrapPort, PcData->PortEvent.ul));
        NT_ASSERTMSG("Port Event Trap", FALSE);
    }

    portStatusBits = UsbhGetPortStatusBits(HubFdo, PortData, PortState, pdo);
    PortData->CurrentPortStatus = PortState->Status;

    LOG(HubFdo, LOG_PCQ, '*BIT', PortData->CurrentPortStatus.us,
        portStatusBits);
    LOG(HubFdo, LOG_PCQ, '*BT2', PortData->CurrentState,
        PortData->PortNumber);
    LOG(HubFdo, LOG_PCQ, '*BT3', PcData->PortEvent.ul,
        portStatusBits);
    DbgTrace((UsbhDebugTrace, "#%d CurrentPortStatus 0x%x\n", PN(PortData),
        PortData->CurrentPortStatus.us));
    DbgTrace((UsbhDebugTrace, "#%d CurrentState 0x%x\n", PN(PortData),
        PortData->CurrentState));
    DbgTrace((UsbhDebugTrace, "#%d PortEvent 0x%x\n", PN(PortData), PcData->PortEvent.ul));
    DbgTrace((UsbhDebugTrace, "#%d portStatusBits 0x%x\n", PN(PortData), portStatusBits));



    // special case port power drop in cases other than over_current change
    //
    // if port status indicates no power than no other status bits can be valid other
    // than overcurrent can be valid.
    //
    // if the connect bit is set we clear it
    if (PortData->CurrentPortStatus.Power == 0 &&
        PortData->CurrentPortStatus.Overcurrent == 0 &&
        PcData->PortEvent.OvercurrentChange == 0 &&
        PcData->PortEvent.PauseChange == 0) {
        // PS_*
        //      *
        //        ****

        //
        // special case port power drop in cases other than over_current change
        //
        // check the port power status bit.  There is only one
        // legitimate case for this to drop and that is over-current.

        // However, we may see this bit clear prior to receiving the overcurrent
        // indication.  If we do then the status bits are suspect, we correct
        // this by overriding the connect bit so that anything seen with power
        // off reads as not connected.  This will cause the device to be disconnected
        // from PnPs point of view.

        //NOTE:
        // should we choose to keep the PDO around we can map the portstatus bits to
        // something else.

        DbgTrace((UsbhDebugTrace, "#%d Power Drop \n", PN(PortData)));

        LOG(HubFdo, LOG_PCQ, 'powX', portStatusBits, PcData->PortEvent.ul);
        //portStatusBits &= ~(PORT_CONNECT_BIT | PORT_ENABLE_BIT);
        portStatusBits &= ~(PORT_ENABLE_BIT);

        // this may occur during a resume so signal any pending resume/suspend event
        // on this port

        UsbhSignalResumeEvent(HubFdo, PortData, FALSE);

        UsbhSignalSuspendEvent(HubFdo, PortData, FALSE);

        HUB_EXCEPTION(USBFILE,  HubFdo, PortData->PortNumber,
                HubErr_PowerBit_Dropped,
                0, 0, PcData, sizeof(struct _PORT_CHANGE_CONTEXT ));

    }

    pcLog = Usbh_PcLogEntry(HubFdo,
                    PortData,
                    pdo,
                    PcData->PortEvent.ul,
                    PcData->FrameInserted,
                    PortData->CurrentPortStatus.us,
                    portStatusBits,
                    0);



/*
    pcLog->CurrentState = PortData->CurrentState;
    pcLog->Change = PcData->PortEvent.ul;
    pcLog->PortStatusBits = portStatusBits;
    pcLog->HwPortStatus = PortData->CurrentPortStatus.us;
    pcLog->Pdo = pdo;
    pcLog->UsbTimeIdx=1;
    pcLog->FrameAge = Usbh_UsbdReadFrameCounter(HubFdo) - PcData->FrameInserted;
*/


    do  {

        // For boot/paging/hiber path, we need to throw a hub reset here if the
        // connect bit on the boot port goes to zero and the event being 
        // received is a Driver Reset or Overcurrent
        if (pdo) {
            pdoExt = PdoExt(pdo);
            if ((pdoExt->IsBootDevice) &&
                (PortData->CurrentPortStatus.Connect == 0)) {

                if (!PcData->PortEvent.DriverResetChange && 
                    !PcData->PortEvent.TimeoutChange &&
                    !PcData->PortEvent.OvercurrentChange) {
                    break;
                }
                
                if (PcData->PortEvent.DriverResetChange || PcData->PortEvent.TimeoutChange) {
                    // signal pd event only in this case
                    LOG(HubFdo, LOG_PCQ, 'bdSR', PcData->Object, PcData->PortEvent.ul);
                    UsbhSignalSyncDeviceReset(HubFdo, pdo, PortData, STATUS_UNSUCCESSFUL);
                }
                
                LOG(HubFdo, LOG_PCQ, 'bdRL', 0, PcData->PortEvent.ul);
                // We need to release the bus lock if it was acquired 
                // so that we can recover 
                if (hubExt->UsbEnumLockPort == PN(PortData) &&
                    hubExt->UsbEnumLockContext == PortData->CurrentSc) {
                    UsbhReleaseEnumBusLock(HubFdo, PortData->CurrentSc, PN(PortData));
                }
                
                HUB_RESET_EXCEPTION(USBFILE, HubFdo, 0, HubErr_HardError_Reset,
                          STATUS_UNSUCCESSFUL, USBD_STATUS_HUB_INTERNAL_ERROR, NULL, 0);

                break;

            //
            // Consume connect changes for devices in D3Cold
            //
            } else if (pdoExt->PdoFlags.D3ColdEnabled == 1) {

                WAIT_EVENT(&pdoExt->D3ColdLock);
                
                if ((pdoExt->PdoFlags.InD3Cold == 1) && 
                    (PcData->PortEvent.ConnectChange == 1)) {
                
                    LOG(HubFdo, LOG_PCQ, 'D3PQ', pdoExt->PdoFlags.D3ColdWaitForReconnect, PortData->CurrentPortStatus.us);
                           
                        // Indicate that we need to wait for reconnect before completing D0 for the PDO
                    pdoExt->PdoFlags.D3ColdWaitForReconnect = 1;
                    pdoExt->PdoFlags.ResetOnLastResume = 1;

                    if (PortData->CurrentPortStatus.Connect == 0) {
                        // Device is disconnected, so block D0 for the PDO
                        RESET_EVENT(&pdoExt->D3ColdReconnectEvent);
                    } else {
                        // Device is connected, so unblock D0 for the PDO
                        SET_EVENT(&pdoExt->D3ColdReconnectEvent);
                    }
                    SET_EVENT(&pdoExt->D3ColdLock);
                    break;
                }
                SET_EVENT(&pdoExt->D3ColdLock);
            }
        }

        // filter out driver reset requests that are not for the current PDO
        if (PcData->PortEvent.DriverResetChange &&
            PcData->Object != pdo) {

            UsbhAssert(HubFdo, PcData->ObjectType == PcqObj_Pdo);
            LOG(HubFdo, LOG_PCQ, 'drvR', PcData->Object, PcData->PortEvent.ul);

            // signal reset on the PDO object in the queue in the queue (NOT the current PDO)
            UsbhSignalSyncDeviceReset(HubFdo, PcData->Object, PortData, NT_DEVICE_DISCONNECTED_STATUS);

            // keep current state
            break;
        }

        // process the change based on the current state
        DbgTrace((UsbhDebugTrace, "#%d Process Change \n", PN(PortData)));

        switch (PortData->CurrentState) {
        case PS_SUSPENDED:
            // we behave same as pause until we see a resume request
            PortData->CurrentState =
                UsbhHandleSuspend(HubFdo,
                                  PcData->PortEvent,
                                  portStatusBits,
                                  PortData,
                                  pdo);


            break;

        case PS_WAIT_CONNECT:
            PortData->CurrentState =
                UsbhWaitConnect(HubFdo,
                                PcData->PortEvent,
                                portStatusBits,
                                PortData,
                                pdo);
            break;
        case PS_ENUMERATE_1:
            PortData->CurrentState =
                UsbhEnumerate1(HubFdo,
                                PcData->PortEvent,
                                portStatusBits,
                                PortData,
                                pdo);
            break;
        case PS_ENUMERATE_2:
            PortData->CurrentState =
                UsbhEnumerate2(HubFdo,
                                PcData->PortEvent,
                                portStatusBits,
                                PortData,
                                pdo);
            break;

        // some type of hardware error has occurred, we have stopped
        // processing changes -- this case allows any subsequent
        // changes to be ignored.

        case PS_HARDERROR:
            if (PcData->PortEvent.OvercurrentChange) {
                // we are already in error state
                TEST_TRAP();
                PortData->CurrentState = UsbhHandleOvercurrent(HubFdo,
                                    PS_HARDERROR,
                                    portStatusBits,
                                    PortData,
                                    pdo);
            }
            PortData->CurrentState = PS_HARDERROR;
            DbgTrace((UsbhDebugTrace, "harderror flush change\n"));
            LOG(HubFdo, LOG_PCQ, 'errF', PortData->CurrentPortStatus.us,
                    portStatusBits);
            break;

        // port is paused
        // all changes need to be flushed.
        case PS_PAUSED:
            if (PcData->PortEvent.OvercurrentChange) {
                // not sure how we would see this after pause but we need
                // to deal with it
                TEST_TRAP();
                PortData->CurrentState = UsbhHandleOvercurrent(HubFdo,
                                    PS_PAUSED,
                                    portStatusBits,
                                    PortData,
                                    pdo);
            } else {
  
                PortData->CurrentState = PS_PAUSED;

                // signal events as appropriate
                // if this is a driver reset or suspend we need to
                // signal any waiting threads
                if (PcData->PortEvent.DriverResetChange) {
                    UsbhSignalSyncDeviceReset(HubFdo, pdo, PortData, STATUS_UNSUCCESSFUL);
                }

                if (PcData->PortEvent.TimeoutChange) {
                    UsbhSignalSyncDeviceReset(HubFdo, pdo, PortData, STATUS_UNSUCCESSFUL);
                }

                if (PcData->PortEvent.RequestPortSuspend) {
                    UsbhSignalSuspendEvent(HubFdo, PortData, FALSE);
                }

                if (PcData->PortEvent.RequestPortResume) {
                    UsbhSignalResumeEvent(HubFdo, PortData, FALSE);
                        
                    // check for disconnected PDO
                    if (portStatusBits == 0x01) {
                        //PCQ_TRAP();                                
                        UsbhAcquireEnumBusLock(HubFdo, PortData->CurrentSc, PortData->PortNumber);

                        UsbhDropDevice(HubFdo, PortData, pdo);
                    }   
                    
                }
            }
            // proceed as normal
            DbgTrace((UsbhDebugTrace, "pause flush change\n"));
            LOG(HubFdo, LOG_PCQ, 'pauF', PortData->CurrentPortStatus.us,
                    portStatusBits);
            break;
        }
        break;

    } WHILE (0);


    if (PortData->PdoLatched) {
        // NOTE: once the PDO is unlatched we must take care not to access
        // it further
        BUS_UNLATCH_PDO(HubFdo, pdo, PortData);
    }

    LOG(HubFdo, LOG_PCQ, 'pev-', PortData, 0);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! <\n", PN(PortData)));

    return PortData->CurrentState;
}


ULONG
UsbhHubProcessTimeoutObj(
    PDEVICE_OBJECT   HubFdo,
    PHUB_PORT_DATA PortData,
    PPORT_CHANGE_CONTEXT PcData,
    PUSB_HUB_PORT_STATE PortState
    )
{
    PPCQ_TIMEOUT_OBJECT tmoObj;
    KIRQL rs_irql;
    PDEVICE_EXTENSION_HUB  hubExt;
    ULONG newPortState;
    
    hubExt = FdoExt(HubFdo);

    UsbhFatalAssert(HubFdo,PortData->Sig == SIG_HUBPORTDATA, HubFdo);

    newPortState = PS_HARDERROR;

    do { 
        
        if (PcData == NULL) {
            break;
        }  
        
        if (PortData == NULL) {
            UsbhFreePool(PcData->Object);
            break;
        }        

        newPortState = PortData->CurrentState;
        
        if (PortState == NULL) {
            UsbhFreePool(PcData->Object);
            break;
        }       

        KeAcquireSpinLock(&hubExt->PortResetSpin, &rs_irql);
        LOG(PortData->HubFdo, LOG_PCQ, 'PCmo', PcData->Object, PortData->PortNumber);
        UsbhAssert(HubFdo, ((PPCQ_TIMEOUT_OBJECT)PcData->Object)->Sig == SIG_TMOOBJ);
        UsbhAssert(HubFdo, PortData->PortHwPendingState == PortHw_TimeoutPending);
        // if it is in the queue AND still current then the timeout fired
        // and this event is valid.
        tmoObj = PortData->ResetTimerPending;
        if (tmoObj == PcData->Object) {
            PortData->ResetTimerPending = NULL;
            PortData->PortHwPendingState = PortHw_Waiting;
            //tmoObj = InterlockedCompareExchangePointer(&PortData->ResetTimerPending,
            //                                           NULL,
            //                                           pcData->Object);

            KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);

            UsbhAssert(HubFdo, tmoObj->Sig == SIG_TMOOBJ);
            // timeout valid, process it
            LOG(PortData->HubFdo, LOG_PCQ, 'tmoV', tmoObj, PcData->Object);
            // test #41
            newPortState = UsbhHubDispatchPortEvent(HubFdo, PortData, PcData, PortState);
            
        } else {
            LOG(PortData->HubFdo, LOG_PCQ, 'PCm1', PcData->Object, PortData->PortNumber);
            //PCQ_TRAP();
            KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);
        }
        
        UsbhFreePool(PcData->Object);
        
     } WHILE (0);


     return newPortState;
}


VOID
UsbhHubRunPortChangeQueue(
    PDEVICE_OBJECT   HubFdo,
    PHUB_PORT_DATA PortData,
    PVOID Context,
    PKIRQL Irql
    )
/*++

Routine Description:

    Processes the change request queue for a port.  This routine loops until the queue goes empty.  It removes 
    entries from the queue and dispatches them to the port enum state machine.  If an error is encountered a pause 
    messages is inserted in front of the current message -- this allows the current operation to be canceled.

    The queue runs regardless of state -- when it exits a PCE_wDone message is sent.
    The queue will exit early if it detects either PS_HARDERROR or PS_PAUSED states for the
    port.

    ** A major chnage from the eralier version.  This routine is not re-entarant but it no longer runs with the PCQ 
    state lock held allowing it to dispatch messages to the state machine more easily.
        

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PLIST_ENTRY le;
    PPORT_CHANGE_CONTEXT pcData;
    USB_HUB_PORT_STATE portState;
    NTSTATUS nts;
    USBD_STATUS usbdStatus;
    USHORT portNumber;
    ULONG newPortState;
    
    hubExt = FdoExt(HubFdo);

    UsbhFatalAssert(HubFdo,Context != NULL, HubFdo);
    UsbhFatalAssert(HubFdo,PortData->Sig == SIG_HUBPORTDATA, HubFdo);
    portNumber = PN(PortData);

    LOG(HubFdo, LOG_PCQ, 'pchW', PortData, PortData->PortNumber);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p >\n", PN(PortData), Context));
    UsbhAssert(HubFdo, PortData->PortNumber == portNumber);

    //UsbhAssertPcqLock(HubFdo, PortData);
    //pcq_state = PortData->Pcq_State;
    // this means we were re-entered
    UsbhAssert(HubFdo, PortData->Pcq_Busy == 0);
    PortData->Pcq_Busy = 1;

    WHILE_TRUE {

        le = UsbhPCE_wChange(HubFdo, PN(PortData), Context); 

        if (le == NULL) {
            break;
        }            
        PortData->CurrentSc = Context;
        UsbhAssert(HubFdo, Context);
        PortData->CurrentSc->ScThread = KeGetCurrentThread();

        //UsbhUnlockPcq(HubFdo, PortData, *Irql);

        pcData = CONTAINING_RECORD(le,
                                   PORT_CHANGE_CONTEXT,
                                   ChangeLink);

        UsbhAssert(HubFdo, pcData->Sig == SIG_PCDATA);
        UsbhAssert(HubFdo, PortData->PortNumber == pcData->PortNumber);
        LOG(HubFdo, LOG_PCQ, 'pcPN', PortData->PortNumber, 0);
        LOG(HubFdo, LOG_PCQ, 'pcDD', PortData, pcData);

        if ((pcData->PortEvent.SuspendChange == 1) || 
            (pcData->PortEvent.ResetChange == 1) ||
            (pcData->PortEvent.EnableChange == 1)) {
            InterlockedDecrement(&PortData->QueuedPortChangeCount);
        }

        // See if we dequeued a connect change
        if (pcData->PortEvent.ConnectChange == 1) {
            // Allow another connect change to be queued
            // This is to prevent connect change storms from
            // overwhelming the port change queue
            PortData->ConnectChangeDispatched = TRUE;
            PortData->ConnectChangeQueued = FALSE;
        }

        // get the current port status bits
        nts = UsbhQueryPortState(HubFdo,
                                 PortData->PortNumber,
                                 &portState,
                                 &usbdStatus);

        if (NT_ERROR(nts)) {
            portState.Status.us = 0;
            portState.Change.us = 0;
        }
        
        if (NT_ERROR(nts)) {
            
            PORT_CHANGE_CONTEXT pauseEvent;

            // if we do this we convert the current event to a pause and dump the change.
            // The call to UsbhPCE_wChangeERROR will attempt to cleanup. 
            if (pcData->ObjectType == PcqObj_Timeout) {
                // the timeout must be processed before we pause since the pause will 
                // attempt to cancel any operations in progress (including this timeout).

                newPortState = UsbhHubProcessTimeoutObj(HubFdo, PortData, pcData, &portState);

                UsbhPCE_wChangeERROR(HubFdo, PortData, pcData->PortEvent, Context, PcqObj_NoType, NULL);
            } else if (pcData->ObjectType == PcqObj_Pdo) {
                // driver reset we need to signal the pdo
                
                UsbhPCE_wChangeERROR(HubFdo, PortData, pcData->PortEvent, Context, PcqObj_Pdo, pcData->Object);
                
            } else {
                
                UsbhPCE_wChangeERROR(HubFdo, PortData, pcData->PortEvent, Context, PcqObj_NoType, NULL);
            
            }

            pauseEvent.Sig = SIG_PCDATA;
            pauseEvent.PortNumber = PortData->PortNumber;
            pauseEvent.PortEvent.ul = 0;
            pauseEvent.PortEvent.PauseChange = 1;

            // the hub may be unplugged, flush all remaining changes and
            // by inserting a pause message.

            // dispatch the pause message to cancel operations in progress,
            // use current port pcq_state if it is valid.
            newPortState = UsbhHubDispatchPortEvent(HubFdo, PortData, &pauseEvent, &portState);

            if (NT_ERROR(nts) && !Usb_Disconnected(nts) && !hubExt->IsBootDevice) {

                // we cannot process the change without status,
                // throw an exception and reset the hub
                HUB_RESET_EXCEPTION(USBFILE, HubFdo, PortData->PortNumber,
                       HubErr_TransactionFailure_Reset,
                       nts, usbdStatus, pcData, sizeof(struct _PORT_CHANGE_CONTEXT));
            }
            
        } else if (pcData->ObjectType == PcqObj_Timeout) { 
            // if this is a timeout then we need to see if it is still valid.  The timeout
            // may have fired just as the reset completed so it may have been queued behind
            // the reset complete.  This can happen when debug output changes how long the
            // reset takes to complete or be processed.

            newPortState = UsbhHubProcessTimeoutObj(HubFdo, PortData, pcData, &portState);
#if 0            
            PPCQ_TIMEOUT_OBJECT tmoObj;
            KIRQL rs_irql;
            
            KeAcquireSpinLock(&hubExt->PortResetSpin, &rs_irql);
            LOG(PortData->HubFdo, LOG_PCQ, 'PCmo', pcData->Object, PortData->PortNumber);
            UsbhAssert(HubFdo, ((PPCQ_TIMEOUT_OBJECT)pcData->Object)->Sig == SIG_TMOOBJ);
            UsbhAssert(HubFdo, PortData->PortHwPendingState == PortHw_TimeoutPending);
            // if it is in the queue AND still current then the timeout fired
            // and this event is valid.
            tmoObj = PortData->ResetTimerPending;
            if (tmoObj == pcData->Object) {
                PortData->ResetTimerPending = NULL;
                PortData->PortHwPendingState = PortHw_Waiting;
                //tmoObj = InterlockedCompareExchangePointer(&PortData->ResetTimerPending,
                //                                           NULL,
                //                                           pcData->Object);

                KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);

                UsbhAssert(HubFdo, tmoObj->Sig == SIG_TMOOBJ);
                // timeout valid, process it
                LOG(PortData->HubFdo, LOG_PCQ, 'tmoV', tmoObj, pcData->Object);
                // test #41
                newPortState = UsbhHubDispatchPortEvent(HubFdo, PortData, pcData, &portState);
            } else {
                LOG(PortData->HubFdo, LOG_PCQ, 'PCm1', pcData->Object, PortData->PortNumber);
                //PCQ_TRAP();
                KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);
            }
            UsbhFreePool(pcData->Object);
#endif
        } else {

            newPortState = UsbhHubDispatchPortEvent(HubFdo, PortData, pcData, &portState);
        }

        PortData->ConnectChangeDispatched = FALSE;
        
        if (pcData->ObjectType == PcqObj_Pdo) {

            PDEVICE_OBJECT pdo;

            pdo = pcData->Object;

            UsbhUnlatchPdo(HubFdo,
                           pdo,
                           pcData,
                           PdoLatchInsertQueuePortChange);
        }

        UsbhFreePool(pcData);

        // this event is complete 
        
        PortData->CurrentSc = NULL;

        if (newPortState == PS_SUSPENDED) {
            // suspend state exit but leave changes queued
            UsbhPCE_psSUSPEND(HubFdo, PortData, Context);  
            break;
        }

        if (PortData->QueuedPortChangeCount > MAX_QUEUED_PORT_CHANGE_COUNT) {
            
            // The hub is generating spurious port changes and needs to be reset.
            // The HUB_RESET_EXCEPTION macro will block further change events from
            // being queued until after the hub is reset.  
            HUB_RESET_EXCEPTION(USBFILE, HubFdo, 0, HubErr_PortChangeStorm,
                                STATUS_UNSUCCESSFUL, USBD_STATUS_HUB_INTERNAL_ERROR, NULL, 0);
        
            // Setting newPortState to PS_HARDERROR will cause the port to be paused, 
            // which will flush pending changes events and reset QueuedPortChangeCount
            // to 0.
            newPortState = PS_HARDERROR;
        }
        
        // early exit for pause & hw errors.  In both of these cases
        // the hardware will be reset so we exit.
        if (newPortState == PS_HARDERROR ||
            newPortState == PS_PAUSED) {

            if (newPortState == PS_HARDERROR) {
                // if hardware error we exit so that we disable as quickly as possible
                // port should be disabled -- we can dump the queue
                PortData->CurrentState = PS_PAUSED;
            }
            UsbhPCE_psPAUSED(HubFdo, PortData, Context);  
            
            // exit while loop after clearing all changes -- we are now stopped
            break;
        }
    
    }

    PortData->Pcq_Busy = 0;
    UsbhPCE_wDone(HubFdo, Context, PortData);
    
    LOG(HubFdo, LOG_PCQ, 'qwk-', PortData, 0);
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! <\n", PN(PortData)));
}


VOID
UsbhHubProcessChangeWorker(
    PDEVICE_OBJECT   HubFdo,
    PVOID Context
    )
/*++

Routine Description:

    Worker callback: Processes the change request queue for a port from a kernel
    workitem.

Irql: PASSIVE

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PHUB_PORT_DATA pd;
    PSSH_BUSY_HANDLE bh;
    ULONG PortNumber;
    LARGE_INTEGER tmoLg, timeout;
    LARGE_INTEGER timerIncrement;
    NTSTATUS nts;
    KIRQL Irql;
        
    UsbhAssertPassive();
    hubExt = FdoExt(HubFdo);

    pd = Context;
    PortNumber = PN(pd);

    UsbhFatalAssert(HubFdo,pd!=NULL, HubFdo);
    
    DbgTrace((UsbhDebugTrace, "#%d %!FUNC! %p \n",  PortNumber, pd));

    bh = UsbhIncHubBusy(HubFdo, PDC(pd), pd, UsbhBusyPCH_Tag, NO_RESUME_HUB);

    timerIncrement = RtlConvertLongToLargeInteger(KeQueryTimeIncrement() - 1);

    tmoLg = RtlConvertLongToLargeInteger(USBHUB_X9F_TIMEOUT);
    tmoLg = RtlExtendedIntegerMultiply(tmoLg, 10000);
    tmoLg = RtlLargeIntegerAdd(tmoLg, timerIncrement);

    timeout = RtlExtendedIntegerMultiply(tmoLg, -1);

    // Synchronize with UsbhWaitEventWithTimeoutEx
    nts = KeWaitForSingleObject(&pd->PortChangeQueueLock,
                                Executive,
                                KernelMode,
                                FALSE,
                                &timeout);

    if (nts == STATUS_TIMEOUT) {
        // We never got ownership of the port change queue lock, which
        // means UsbhWaitEventWithTimeoutEx has been processing the port change
        // queue for more than 60 seconds.  Bugcheck so we can
        // diagnose what the problem is.
        UsbhTrapFatalTimeout_x9f(HubFdo, pd->TimeoutCode ? pd->TimeoutCode : Timeout_ProcessPCQ_Action, pd);
    }

    // Change the worker state from pending to running
    KeAcquireSpinLock(&pd->PortChangeWorkerSpin, &Irql);
    UsbhAssert(HubFdo, pd->PortChangeWorkerState == PortChangeWorker_Pending);
    pd->PortChangeWorkerState = PortChangeWorker_Running;
    KeReleaseSpinLock(&pd->PortChangeWorkerSpin, Irql);

    LOG(HubFdo, LOG_PCQ, 'HPCW', pd, PortNumber);

    UsbhPCE_wRun(HubFdo, PDC(pd), pd);

    KeAcquireSpinLock(&pd->PortChangeWorkerSpin, &Irql);
    LOG(HubFdo, LOG_PCQ, 'HPCd', pd->PortChangeWorkerState, PortNumber);
    switch (pd->PortChangeWorkerState) {
        case PortChangeWorker_Running:
            // Worker thread has not been requeued, so transition to idle and signal the worker done event
            pd->PortChangeWorkerState = PortChangeWorker_Idle;
            break;
        
        case PortChangeWorker_Pending:
            // A new work item has been queued for this port, so do nothing
            break;

        case PortChangeWorker_Idle:
            // Another work-item was queued and ran to completion between when we 
            // ran finished running the queue and when we acquired the spinlock.
            // Nothing to do in this case.
            break;
    }
    KeReleaseSpinLock(&pd->PortChangeWorkerSpin, Irql);

    UsbhDecHubBusy(HubFdo, PDC(pd), bh);

    UsbhHubDereferenceProcessChangeWorkItem(HubFdo, pd);
}

VOID
UsbhHubQueueProcessChangeWorker(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    )
/*++

Routine Description:

    Queue the work item to process the port change queue
    workitem.

Irql: <= DISPATCH

Arguments:
    
    PortData - Port Data for the port being processed

Return Value:

    none

--*/
{
    KIRQL Irql;

    KeAcquireSpinLock(&PortData->PortChangeWorkerSpin, &Irql);

    if (PortData->PortChangeWorkerState != PortChangeWorker_Pending) {
        PortData->PortChangeWorkerState = PortChangeWorker_Pending;
        LOG(PortData->HubFdo, LOG_PCQ, 'QPCW', PortData, 0);

        UsbhHubReferenceProcessChangeWorkItem(HubFdo, PortData);

        IoQueueWorkItem(PortData->PortChangeWorkItem,
                        UsbhHubProcessChangeWorker,
                        DelayedWorkQueue,
                        PortData);
    }
    KeReleaseSpinLock(&PortData->PortChangeWorkerSpin, Irql);

}

VOID
UsbhHubReferenceProcessChangeWorkItem(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    )
{
    UsbhAssert(HubFdo, PortData->PortChangeWorkItemReference >= 1);

    InterlockedIncrement(&PortData->PortChangeWorkItemReference);
}

VOID
UsbhHubDereferenceProcessChangeWorkItem(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    )
{
    LONG ReferenceCount;
    
    UsbhAssert(HubFdo, PortData->PortChangeWorkItemReference >= 1);

    ReferenceCount = InterlockedDecrement(&PortData->PortChangeWorkItemReference);

    if (ReferenceCount == 0) {
        SET_EVENT(&PortData->PortChangeWorkerDone);
    }

}

VOID
UsbhHubDereferenceProcessChangeWorkItemAndWait(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    )
{
    UsbhHubDereferenceProcessChangeWorkItem(HubFdo, PortData);
    WAIT_EVENT(&PortData->PortChangeWorkerDone);
}
