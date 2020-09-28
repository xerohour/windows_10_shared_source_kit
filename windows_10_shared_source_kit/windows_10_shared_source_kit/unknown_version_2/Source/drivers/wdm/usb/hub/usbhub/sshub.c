/*
Module: SSHUB.C


name: USB Hub Selective Suspend

    Copyright (c) Microsoft Corporation. All rights reserved

created:

    1-2-2008

Abstract:

    This code implements the Selective Suspend for Hubs (SSH) functionality in USBHUB.SYS.

Module Entry Points:

<pre>
         PNP event                       Function
    -----------------------------------------------------------
    Entry  -->linked to DriverEntry      NULL
    Unload -->linked to Unload           NULL
    Start  -->linked to PNP START        NULL
    Stop   -->linked to PNP STOP         NULL
    Add    -->linked to AddDevice        NULL
    Remove -->linked to PNP REMOVE       NULL

** begin ssh doc


** end ssh doc

</pre>

*/


#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "sshub.tmh"
#endif

CCHAR usbfile_sshub_c[] = "sshub.c";

#define USBFILE usbfile_sshub_c

// Define a GUID for USB power policy
//
// {48E6B7A6-50F5-4782-A5D4-53BB8F07E226}
DEFINE_GUID(GUID_USB_SETTING_SELECTIVE_SUSPEND, 0x48e6b7a6, 0x50f5, 0x4782, 0xa5, 0xd4, 0x53, 0xbb, 0x8f, 0x7, 0xe2, 0x26);

__drv_maxIRQL(PASSIVE_LEVEL)
SSH_HUB_STATE
UsbhLockSSH(
    __inout PSTATE_CONTEXT Sc,
    __in __drv_strictTypeMatch(__drv_typeConst)
    SSH_EVENT Event
    )
/*++

Routine Description:

Serializes Hub Selective Suspend state transitions.

Arguments:

    Sc - State Context structure for hub selective suspend

    Event - Event for the hub SS state machine

Irql: PASSIVE

Return Value:

   returns the current SSH state

--*/

{
    PDEVICE_EXTENSION_HUB hubExt;
    PG_STATE_LOG_ENTRY log, curLog;
    ULONG curIdx;

    hubExt = FdoExt(Sc->HubFdo);

    UsbhAssertPassive();

    WAIT_EVENT(&hubExt->SSH_Lock);

    UsbhAssert(Sc->HubFdo, Sc->SSH_Sig == UNSIG);
    hubExt->SSH_CurrentSC = Sc;
    Sc->SSH_Sig = SIG_SSH_CONTEXT;
    Sc->SSH_Event = Event;
    Sc->ScThread = KeGetCurrentThread();

    // if there is no change in state info from the previous entry then we skip the entry to avoid
    // flooding the log
    curIdx = (hubExt->SSH_LogIdx) % G_NUM_STATE_LOG_ENTRIES;
    curLog =  &hubExt->SSH_Log[curIdx];

    if (curLog->Event != (ULONG)Event ||
        curLog->CurrentState != (ULONG)hubExt->SSH_State) {

        // update state history
        hubExt->SSH_LogIdx = (hubExt->SSH_LogIdx+1) % G_NUM_STATE_LOG_ENTRIES;
        log = &hubExt->SSH_Log[hubExt->SSH_LogIdx];

        log->Event = Event;
        log->CurrentState = hubExt->SSH_State;
    }

    return  hubExt->SSH_State;
}

__drv_maxIRQL(PASSIVE_LEVEL)
PSSH_BUSY_HANDLE
UsbhIncHubBusy(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc,
    __in PVOID Object,
    __in ULONG Tag,
    __in __drv_strictTypeMatch(__drv_typeConst)
    RESUME_HUB_FLAG ResumeHubFlag
    )
/*++

Routine Description:

    Log a request that keeps hub from going idle and entering selective suspend.

    A separate lock is used to protect the busy list so that this routine
    can be called more freely.

    The busy list prevents selective suspend as long as it is not empty.  Since
    elements the elements on the list allow us to setermine why a particular may
    be blocking selective suspend.


Arguments:

    Object - object associated with this ref

    Tag - used to identify this instance and the object type

Irql: PASSIVE

Return Value:

    A BusyHandle is returned -- this is a small structure that represents the
        object or event that is keeping us 'busy'.  The handle must be passed
        to the coresponding dec routine.  If the hub cannot be ref'ed as busy
        a NULL is returned and the SSH engine is disabled until reboot.

--*/
{
    PSSH_BUSY_HANDLE bh;
    PDEVICE_EXTENSION_HUB hubExt;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    // Indicate to the idle timer that the hub has been active since the last timer callback
    hubExt->SSH_HubActivity = TRUE;

    WAIT_EVENT(&hubExt->SshBusyListLock);

    LOG(HubFdo, LOG_SSH, 'HInc', ResumeHubFlag, hubExt->BusyListEnabled);

    if (!hubExt->BusyListEnabled) {
        SET_EVENT(&hubExt->SshBusyListLock);
        return NULL;
    }

#ifndef USB_FORCE_MEMORY_STRESS
    UsbhAllocatePool_Z(bh, NonPagedPool, sizeof(struct _SSH_BUSY_HANDLE));
#endif

    if (bh) {
        bh->Sig = SIG_SSH_BUSY_HANDLE;
        bh->Object = Object;
        bh->Tag = Tag;
        InsertTailList(&hubExt->SshBusyListHead, &bh->BusyLink);
        UsbhAssert(NULL, bh != SSH_DEFAULT_BUSY_HANDLE);
    } else {
        bh = SSH_DEFAULT_BUSY_HANDLE;
        hubExt->SshBusyRefCountNoRes++;
    }

    SET_EVENT(&hubExt->SshBusyListLock);

    if (ResumeHubFlag == RESUME_HUB) {
        Usbh_SSH_Event(HubFdo, SSH_Event_ResumeHub, Sc);
    }

    return bh;
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhDecHubBusy(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc,
    __inout_opt PSSH_BUSY_HANDLE BusyHandle
    )
/*++

Routine Description:

    Log a request that releases hub for idle detect (SSH) associated
    with a given BusyHandle.

Arguments:

    BusyHandle - handle acquired by a call to UsbhIncHubBusy.

    Sc - state context incrementing the busy count

Irql: PASSIVE

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    hubExt->SSH_HubActivity = TRUE;

    WAIT_EVENT(&hubExt->SshBusyListLock);

    LOG(HubFdo, LOG_SSH, 'HDec', BusyHandle, hubExt->BusyListEnabled);

    if (!hubExt->BusyListEnabled) {
        SET_EVENT(&hubExt->SshBusyListLock);
        return;
    }

    if (BusyHandle == SSH_DEFAULT_BUSY_HANDLE) {
        UsbhAssert(HubFdo, hubExt->SshBusyRefCountNoRes);
        if (hubExt->SshBusyRefCountNoRes) {
            hubExt->SshBusyRefCountNoRes--;
        }
    } else if (BusyHandle) {
        UsbhAssert(HubFdo, BusyHandle->Sig == SIG_SSH_BUSY_HANDLE);
        RemoveEntryList(&BusyHandle->BusyLink);
        InitializeListHead(&BusyHandle->BusyLink);
        UsbhFreePool(BusyHandle);
    }
    
    SET_EVENT(&hubExt->SshBusyListLock);
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhFreeBusyList(
    __in PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Frees all entries on the busy list

Arguments:

    HubFdo - Device object for the hub 

Irql: PASSIVE

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PSSH_BUSY_HANDLE BusyHandle;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    hubExt->SSH_HubActivity = TRUE;

    WAIT_EVENT(&hubExt->SshBusyListLock);

    LOG(HubFdo, LOG_SSH, 'SFBL', 0, hubExt->BusyListEnabled);

    hubExt->BusyListEnabled = FALSE;

    while (!IsListEmpty(&hubExt->SshBusyListHead)) {
        PLIST_ENTRY le;

        le = RemoveHeadList(&hubExt->SshBusyListHead);

        BusyHandle = (PSSH_BUSY_HANDLE) CONTAINING_RECORD(
                le,
                struct _SSH_BUSY_HANDLE,
                BusyLink);
    
        LOG(HubFdo, LOG_SSH, 'FBL-', 0, BusyHandle);

        UsbhAssert(HubFdo, BusyHandle->Sig == SIG_SSH_BUSY_HANDLE);
        UsbhFreePool(BusyHandle);
    }
    
    // Zero out the busy handles
    hubExt->BusBusyHandle = NULL;
    hubExt->SshSxBusyHandle = NULL;
    hubExt->SshDisabledBusyHandle = NULL;
    hubExt->SshPortsBusyHandle = NULL;
    hubExt->SshBusyRefCountNoRes = 0;

    // zero out the port busy mask
    hubExt->PortBusyMask[0] = hubExt->PortBusyMask[1] = hubExt->PortBusyMask[2] = hubExt->PortBusyMask[3] = 0;

    SET_EVENT(&hubExt->SshBusyListLock);
}


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhUnlockSSH(
    __inout PSTATE_CONTEXT Sc,
    __in __drv_strictTypeMatch(__drv_typeConst)
    SSH_HUB_STATE NewState
    )
/*++

Routine Description:

Release SSH lock and complete state transition

Arguments:

    Context - ptr to a bus state context -- this will be the context that currently
        holds the lock.

    NewState - new state to transition too before releasing the lock.

Irql: PASSIVE

Return Value:

   none

--*/

{
    PDEVICE_EXTENSION_HUB hubExt;
    PG_STATE_LOG_ENTRY log;

    hubExt = FdoExt(Sc->HubFdo);

    log = &hubExt->SSH_Log[hubExt->SSH_LogIdx];

    log->NextState = hubExt->SSH_State = NewState;

    UsbhAssert(Sc->HubFdo, Sc->SSH_Sig == SIG_SSH_CONTEXT);
    Sc->SSH_Sig = UNSIG;
    hubExt->SSH_CurrentSC = NULL;
    KeReleaseSemaphore(&hubExt->SSH_Lock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);
}


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_SSH_Event(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_strictTypeMatch(__drv_typeConst)
    SSH_EVENT SSH_Event,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

Dispatch routine for hub selective suspend events.

Arguments:

Irql: PASSIVE

Return Value:

    Status for this event.  All events are handled synchronously.

--*/
{
    NTSTATUS nts;
    SSH_HUB_STATE state;

    DbgTrace((UsbhDebugTrace,"%!FUNC! Event:%d>\n", SSH_Event));
    UsbhAssertPassive();

    state = UsbhLockSSH(Sc, SSH_Event);

    // Dispatch the event to the appropriate state handler
    // The handler is responsible for unlocking the state machine before returning
    switch (state) {
    case SSH_State_HubActive:
        nts = Usbh_SSH_HubActive(HubFdo, SSH_Event, Sc);
        break;

    case SSH_State_HubPendingSuspend:
        nts = Usbh_SSH_HubPendingSuspend(HubFdo, SSH_Event, Sc);
        break;

    case SSH_State_HubPendingResumeAfterSuspend:
        nts = Usbh_SSH_HubPendingResumeAfterSuspend(HubFdo, SSH_Event, Sc);
        break;

    case SSH_State_HubSuspended:
        nts = Usbh_SSH_HubSuspended(HubFdo, SSH_Event, Sc);
        break;

    case SSH_State_HubPendingResume:
        nts = Usbh_SSH_HubPendingResume(HubFdo, SSH_Event, Sc);
        break;

    case SSH_State_HubFailedResume:
        nts = Usbh_SSH_HubFailedResume(HubFdo, SSH_Event, Sc);
        break;

    default:
        nts = STATUS_UNSUCCESSFUL;
        UsbhUnlockSSH(Sc, state);
        UsbhAssertFailure(HubFdo, Sc);

    }

    return nts;
}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_SSH_HubActive(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_strictTypeMatch(__drv_typeConst)
    SSH_EVENT SSH_Event,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

Hub Selective Suspend State Machine handler for the Active state.

Arguments:
    HubFdo - FDO for the hub.

    SSH_Event - Event sent to the Hub Selective Suspend State Machine.

    Sc - State context structure for the hub selective suspend.

Irql: PASSIVE

Return Value:

    Status for this event.  All events are handled synchronously.

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts=STATUS_SUCCESS;

    DbgTrace((UsbhDebugTrace,"%!FUNC!\n"));
    UsbhAssertPassive();
    UsbhAssertSshLocked(HubFdo);

    hubExt = FdoExt(HubFdo);

    switch (SSH_Event) {
    case SSH_Event_IdleTimer:
        // Check if the hub and its children are idle
        nts = UsbhSsh_CheckHubIdle(HubFdo, Sc);

        if NT_SUCCESS(nts) {
            // Hub is idle; transition to the Pending Suspend state
            // and initiate hub suspend

            // Resume event must be reset prior to dropping the state machine lock
            RESET_EVENT(&hubExt->SSH_ResumeEvent);
            
            hubExt->SSH_FailedToSuspend = FALSE;

            UsbhUnlockSSH(Sc, SSH_State_HubPendingSuspend);
            UsbhSshSuspendHub(HubFdo, Sc);
        } else {
            UsbhUnlockSSH(Sc, SSH_State_HubActive);

            WAIT_EVENT(&hubExt->SSH_BusyListLock);
        
            if ((hubExt->SshPortsBusyHandle == NULL) &&
                (hubExt->SshSxBusyHandle == NULL) &&
                (hubExt->SshDisabledBusyHandle == NULL)) {
                
                UsbhEnableTimerObject(HubFdo,
                                      &hubExt->SSH_Timer,
                                      hubExt->IdleTimeoutInMilliSeconds,
                                      0,
                                      SSH_API_CONTEXT(HubFdo),
                                      SIG_SSH_WORKER,
                                      0);

            }

            KeReleaseSemaphore(&hubExt->SSH_BusyListLock,
                               LOW_REALTIME_PRIORITY,
                               1,
                               FALSE);
        }
        break;

    case SSH_Event_SuspendHubSucceeded:
        // Should not receive this event when the hub is active
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubActive);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_SuspendHubFailed:
        // Should not receive this event when the hub is active
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubActive);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_HardReset:
        // Hard reset only allowed when hub is active and not
        // in a low Sx state
        if (hubExt->SshSxBusyHandle) {
            nts = STATUS_UNSUCCESSFUL;
        } else {
            nts = STATUS_SUCCESS;
        }
        UsbhUnlockSSH(Sc, SSH_State_HubActive);
        break;

    case SSH_Event_GetPortStatus:
        // Hub is active, so get port status is allowed
        UsbhUnlockSSH(Sc, SSH_State_HubActive);
        nts = STATUS_SUCCESS;
        break;
    
    case SSH_Event_ResumeHubComplete:
        // Should not receive this event when the hub is active
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubActive);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_ResumeHub:
        // Hub already active
         hubExt->SSH_HubActivity = TRUE;
        UsbhUnlockSSH(Sc, SSH_State_HubActive);
        nts = STATUS_SUCCESS;
        break;    

    case SSH_Event_PropagateResume:
        // Hub already active
        UsbhUnlockSSH(Sc, SSH_State_HubActive);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_ResumeHubFailed:
        // Should not receive this event when the hub is active
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubActive);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    default:
        // All events should be explicitly handled, so an unknown event is an error
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubActive);
        nts = STATUS_UNSUCCESSFUL;
        break;    
    }

    return nts;
}


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
UsbhSsh_CheckPortChangeQueuesIdle(
    __in PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

Determines if the hub's port change queues are empty.

Arguments:
    HubFdo - FDO for the hub.

Irql: PASSIVE

Return Value:

    STATUS_SUCCESS - Queues are empty.
    STATUS_UNSUCCESSFUL - Queues are not empty.

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PHUB_PORT_DATA portData;
    USHORT port;
    KIRQL irql;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    for(port=1; port <= N_PORTS(HubFdo); port++) {
        portData = UsbhGetPortData(HubFdo, port);

        KeAcquireSpinLock(&portData->PortChangeListSpin, &irql);
        portData->PortChangeListLock = 1;

        if (!IsListEmpty(&portData->PortChangeList)) {
            portData->PortChangeListLock = 0;
            KeReleaseSpinLock(&portData->PortChangeListSpin, irql);
            return STATUS_UNSUCCESSFUL;
        }

        portData->PortChangeListLock = 0;
        KeReleaseSpinLock(&portData->PortChangeListSpin, irql);
    }

    return STATUS_SUCCESS;
}


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
UsbhSsh_CheckHubIdle(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

Determines if the hub can be suspended.

Arguments:
    HubFdo - FDO for the hub.

    Sc - State context structure for the hub selective suspend.

Irql: PASSIVE

Return Value:

    Status for this event.

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    // If any hub activity since the last timer callback, 
    // the busy list is not empty, PDOs are pending surprise removal,
    // or a hard reset of the hub is pending, we cannot suspend the hub
    if ( (hubExt->SSH_HubActivity) || 
         !IsListEmpty(&hubExt->SshBusyListHead) ||
         hubExt->SshBusyRefCountNoRes ||
         !IsListEmpty(&hubExt->PdoRemovedListHead) ||
         (hubExt->FdoFlags.HubHardResetPending) ||
         (hubExt->IsrWorkerReference > 1) ||
         !NT_SUCCESS(UsbhSsh_CheckPortChangeQueuesIdle(HubFdo)) ) {
        nts = STATUS_UNSUCCESSFUL;
    } else {
        nts = STATUS_SUCCESS;
    }

    // Reset the hub activity flag
    hubExt->SSH_HubActivity = FALSE;

    return nts;
}


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_SSH_HubPendingSuspend(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_strictTypeMatch(__drv_typeConst)
    SSH_EVENT SSH_Event,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

Hub Selective Suspend State Machine handler for the PendingSuspend state.

Arguments:
    HubFdo - FDO for the hub.

    SSH_Event - Event sent to the Hub Selective Suspend State Machine.

    Sc - State context structure for the hub selective suspend.

Irql: PASSIVE

Return Value:

    Status for this event.  All events are handled synchronously.

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    SSH_HUB_STATE state;
    NTSTATUS nts=STATUS_SUCCESS;

    DbgTrace((UsbhDebugTrace,"%!FUNC!\n"));
    UsbhAssertPassive();
    UsbhAssertSshLocked(HubFdo);

    hubExt = FdoExt(HubFdo);

    switch (SSH_Event) {
    case SSH_Event_IdleTimer:
        // Already suspending the hub
        UsbhUnlockSSH(Sc, SSH_State_HubPendingSuspend);
        nts = STATUS_SUCCESS;
        break;    

    case SSH_Event_SuspendHubSucceeded:
        // Hub is now in the suspended state
        UsbhUnlockSSH(Sc, SSH_State_HubSuspended);
        nts = STATUS_SUCCESS;
        break;    

    case SSH_Event_SuspendHubFailed:
        // Hub suspend failed, return to the active state
        UsbhUnlockSSH(Sc, SSH_State_HubActive);
        nts = STATUS_SUCCESS;
        break;    
    
    case SSH_Event_HardReset:
        // Hub is suspending, so reset recovery is not allowed
        UsbhUnlockSSH(Sc, SSH_State_HubPendingSuspend);
        nts = STATUS_UNSUCCESSFUL;
        break;

    case SSH_Event_GetPortStatus:
        // Hub is suspending, so get port status is not allowed
        UsbhUnlockSSH(Sc, SSH_State_HubPendingSuspend);
        nts = STATUS_UNSUCCESSFUL;
        break;

    case SSH_Event_ResumeHub:
        // Resume event has occured for the hub while it is suspending.
        // Transition to the PendingResumeAfterSuspend state.  Hub will be 
        // resumed after hub suspend has completed.
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResumeAfterSuspend);
        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_RESUME_START, 0, STATUS_SUCCESS);

        WAIT_EVENT(&hubExt->SSH_ResumeEvent);

        // See if the resume event was successful or not
        state = UsbhLockSSH(Sc, SSH_Event);
        if (state == SSH_State_HubFailedResume) {
            nts = STATUS_UNSUCCESSFUL;
        } else {
            nts = STATUS_SUCCESS;
        }
        UsbhUnlockSSH(Sc, state);
        break;    

    case SSH_Event_ResumeHubComplete:
        // Should not receive resume complete event when the hub is suspending
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubPendingSuspend);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_PropagateResume:
        // Should not propagate resume event when the hub is suspending
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubPendingSuspend);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_ResumeHubFailed:
        // Should not receive resume complete event when the hub is suspending
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubPendingSuspend);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    default:
        // All events should be explicitly handled, so an unknown event is an error
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubPendingSuspend);
        nts = STATUS_UNSUCCESSFUL;
        break;    
    }

    return nts;
}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_SSH_HubPendingResumeAfterSuspend(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_strictTypeMatch(__drv_typeConst)
    SSH_EVENT SSH_Event,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

Hub Selective Suspend State Machine handler for the PendingResumeAfterSuspend state.

Arguments:
    HubFdo - FDO for the hub.

    SSH_Event - Event sent to the Hub Selective Suspend State Machine.

    Sc - State context structure for the hub selective suspend.

Irql: PASSIVE

Return Value:

    Status for this event.  All events are handled synchronously.

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    SSH_HUB_STATE state;
    NTSTATUS nts=STATUS_SUCCESS;

    DbgTrace((UsbhDebugTrace,"%!FUNC!\n"));
    UsbhAssertPassive();
    UsbhAssertSshLocked(HubFdo);

    hubExt = FdoExt(HubFdo);

    switch (SSH_Event) {
    case SSH_Event_IdleTimer:
        // Already in the hub suspend process
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResumeAfterSuspend);
        nts = STATUS_SUCCESS;
        break;    

    case SSH_Event_SuspendHubSucceeded:
        // Hub suspend completed; now resume the hub
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResume);
        UsbhSshResumeUpstream(HubFdo, Sc);
        nts = STATUS_SUCCESS;
        break;    

    case SSH_Event_SuspendHubFailed:
        // Hub suspend failed for some reason; transition back to the active state
        UsbhUnlockSSH(Sc, SSH_State_HubActive);
        if (!hubExt->SSH_FailedToSuspend) {
            UsbhSshResumeDownstream(HubFdo, Sc, FALSE);
        }
        nts = STATUS_SUCCESS;
        break;    
    
    case SSH_Event_HardReset:
        // Hub is suspending, so reset recovery is not allowed
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResumeAfterSuspend);
        nts = STATUS_UNSUCCESSFUL;
        break;

    case SSH_Event_GetPortStatus:
        // Hub is suspending, so get port status is not allowed
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResumeAfterSuspend);
        nts = STATUS_UNSUCCESSFUL;
        break;

    case SSH_Event_ResumeHub:
        // Already waiting for resume after suspend is complete, so wait until pending resume event completes
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResumeAfterSuspend);
        
        WAIT_EVENT(&hubExt->SSH_ResumeEvent);

        // See if the resume was successful or not
        state = UsbhLockSSH(Sc, SSH_Event);
        if (state == SSH_State_HubFailedResume) {
            nts = STATUS_UNSUCCESSFUL;
        } else {
            nts = STATUS_SUCCESS;
        }
        UsbhUnlockSSH(Sc, state);
        break;    

    case SSH_Event_ResumeHubComplete:
        // Should not receive this event when we are waiting for suspend to complete
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResumeAfterSuspend);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_PropagateResume:
        // Should not receive resume complete event when the hub is suspending
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResumeAfterSuspend);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_ResumeHubFailed:
        // Should not receive this event when we are waiting for suspend to complete
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResumeAfterSuspend);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    default:
        // All events should be explicitly handled, so an unknown event is an error
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResumeAfterSuspend);
        nts = STATUS_UNSUCCESSFUL;
        break;    
    }

    return nts;
}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_SSH_HubSuspended(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_strictTypeMatch(__drv_typeConst)
    SSH_EVENT SSH_Event,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

Hub Selective Suspend State Machine handler for the HubSuspended state.

Arguments:
    HubFdo - FDO for the hub.

    SSH_Event - Event sent to the Hub Selective Suspend State Machine.

    Sc - State context structure for the hub selective suspend.

Irql: PASSIVE

Return Value:

    Status for this event.  All events are handled synchronously.

--*/
{
    NTSTATUS nts;

    DbgTrace((UsbhDebugTrace,"%!FUNC!\n"));
    UsbhAssertPassive();
    UsbhAssertSshLocked(HubFdo);

    switch (SSH_Event) {
    case SSH_Event_IdleTimer:
        // Hub is already suspended
        UsbhUnlockSSH(Sc, SSH_State_HubSuspended);
        nts = STATUS_SUCCESS;
        break;    

    case SSH_Event_SuspendHubSucceeded:
        // Should not receive this event when we are suspended
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubSuspended);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_SuspendHubFailed:
        // Should not receive this event when we are suspended
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubSuspended);
        nts = STATUS_UNSUCCESSFUL;
        break;    
    
    case SSH_Event_HardReset:
        // Hub is suspended, so reset recovery is not allowed
        UsbhUnlockSSH(Sc, SSH_State_HubSuspended);
        nts = STATUS_UNSUCCESSFUL;
        break;

    case SSH_Event_GetPortStatus:
        // Hub is suspended, so get port status is not allowed
        UsbhUnlockSSH(Sc, SSH_State_HubSuspended);
        nts = STATUS_UNSUCCESSFUL;
        break;

    case SSH_Event_ResumeHub:
        // Resume the hub
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResume);
        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_RESUME_START, 0, STATUS_SUCCESS);
        nts = UsbhSshResumeUpstream(HubFdo, Sc);
        if (!NT_SUCCESS(nts)) {
            //
            // If the UpStream resume did not succeed then mark
            // the resume as failed and unblock the state machine.
            //
            Usbh_SSH_Event(Sc->HubFdo, SSH_Event_ResumeHubFailed, Sc);
        }

        break;    

    case SSH_Event_ResumeHubComplete:
        // Should not receive this event when we are suspended
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubSuspended);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_PropagateResume:
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResume);
        UsbhSshResumeDownstream(HubFdo, Sc, TRUE);
        nts = STATUS_SUCCESS;  
        break;    

    case SSH_Event_ResumeHubFailed:
        // Should not receive this event when we are suspended
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubSuspended);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    default:
        // All events should be explicitly handled, so an unknown event is an error
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubSuspended);
        nts = STATUS_UNSUCCESSFUL;
        break;    
    }

    return nts;
}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_SSH_HubPendingResume(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_strictTypeMatch(__drv_typeConst)
    SSH_EVENT SSH_Event,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

Hub Selective Suspend State Machine handler for the PendingResume state.

Arguments:
    HubFdo - FDO for the hub.

    SSH_Event - Event sent to the Hub Selective Suspend State Machine.

    Sc - State context structure for the hub selective suspend.

Irql: PASSIVE

Return Value:

    Status for this event.  All events are handled synchronously.

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    SSH_HUB_STATE state;
    NTSTATUS nts=STATUS_SUCCESS;

    DbgTrace((UsbhDebugTrace,"%!FUNC!\n"));
    UsbhAssertPassive();
    UsbhAssertSshLocked(HubFdo);

    hubExt = FdoExt(HubFdo);

    switch (SSH_Event) {
    case SSH_Event_IdleTimer:
        // Hub is resuming
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResume);
        nts = STATUS_SUCCESS;
        break;    

    case SSH_Event_SuspendHubSucceeded:
        // Should not receive this event when we are resuming
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResume);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_SuspendHubFailed:
        // Should not receive this event when we are resuming
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResume);
        nts = STATUS_UNSUCCESSFUL;
        break;    
    
    case SSH_Event_HardReset:
        // Hub is resuming, so reset recovery is not allowed
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResume);
        nts = STATUS_UNSUCCESSFUL;
        break;

    case SSH_Event_GetPortStatus:
        // Hub is resuming, so get port status is not allowed
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResume);
        nts = STATUS_UNSUCCESSFUL;
        break;

    case SSH_Event_ResumeHub:
        // Hub is already resuming, so wait until the pending resume event completes
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResume);
        
        WAIT_EVENT(&hubExt->SSH_ResumeEvent);

        // See if the resume event was successful or not
        state = UsbhLockSSH(Sc, SSH_Event);
        if (state == SSH_State_HubFailedResume) {
            nts = STATUS_UNSUCCESSFUL;
        } else {
            nts = STATUS_SUCCESS;
        }
        UsbhUnlockSSH(Sc, state);
        break;    

    case SSH_Event_ResumeHubComplete:
        // Hub resumed, so transition to active state
        UsbhUnlockSSH(Sc, SSH_State_HubActive);
        nts = STATUS_SUCCESS;
        break;    

    case SSH_Event_PropagateResume:
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResume);
        UsbhSshResumeDownstream(HubFdo, Sc, TRUE);
        nts = STATUS_SUCCESS; 
        break;    

    case SSH_Event_ResumeHubFailed:
        // Hub failed to resume, so transition to HubFailedResume state
        UsbhUnlockSSH(Sc, SSH_State_HubFailedResume);
        
        // Complete the hub's wait-wake IRP
        UsbhDisarmHubForWakeDetect(HubFdo);

        nts = STATUS_SUCCESS;
        break;    

    default:
        // All events should be explicitly handled, so an unknown event is an error
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubPendingResume);
        nts = STATUS_UNSUCCESSFUL;
        break;    
    }

    return nts;
}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_SSH_HubFailedResume(
    __in PDEVICE_OBJECT HubFdo,
    __in __drv_strictTypeMatch(__drv_typeConst)
    SSH_EVENT SSH_Event,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

Hub Selective Suspend State Machine handler for the FailedResume state.

Arguments:
    HubFdo - FDO for the hub.

    SSH_Event - Event sent to the Hub Selective Suspend State Machine.

    Sc - State context structure for the hub selective suspend.

Irql: PASSIVE

Return Value:

    Status for this event.  All events are handled synchronously.

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts=STATUS_SUCCESS;

    DbgTrace((UsbhDebugTrace,"%!FUNC!\n"));
    UsbhAssertPassive();
    UsbhAssertSshLocked(HubFdo);

    hubExt = FdoExt(HubFdo);

    switch (SSH_Event) {
    case SSH_Event_IdleTimer:
        // Hub is in a failed state, so return an error
        UsbhUnlockSSH(Sc, SSH_State_HubFailedResume);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_SuspendHubSucceeded:
        // Hub is in a failed state, so return an error
        UsbhUnlockSSH(Sc, SSH_State_HubFailedResume);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_SuspendHubFailed:
        // Hub is in a failed state, so return an error
        UsbhUnlockSSH(Sc, SSH_State_HubFailedResume);
        nts = STATUS_UNSUCCESSFUL;
        break;    
    
    case SSH_Event_HardReset:
        // Hub is in a failed state, so return an error
        UsbhUnlockSSH(Sc, SSH_State_HubFailedResume);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_GetPortStatus:
        // Hub is in a failed state, so return an error
        UsbhUnlockSSH(Sc, SSH_State_HubFailedResume);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_ResumeHub:
        // Hub is in a failed state, so return an error
        UsbhUnlockSSH(Sc, SSH_State_HubFailedResume);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_ResumeHubComplete:
        // Hub is in a failed state, so return an error
        UsbhUnlockSSH(Sc, SSH_State_HubFailedResume);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_PropagateResume:
        // Hub is in a failed state, so return an error
        UsbhUnlockSSH(Sc, SSH_State_HubFailedResume);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    case SSH_Event_ResumeHubFailed:
        // Hub is in a failed state, so return an error
        UsbhUnlockSSH(Sc, SSH_State_HubFailedResume);
        nts = STATUS_UNSUCCESSFUL;
        break;    

    default:
        // All events should be explicitly handled, so an unknown event is an error
        UsbhAssertFailure(HubFdo, Sc);
        UsbhUnlockSSH(Sc, SSH_State_HubFailedResume);
        nts = STATUS_UNSUCCESSFUL;
        break;    
    }

    return nts;
}




__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhHubSSH_AddDeviceInit(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    pnp start handler for hub selective suspend.

Arguments:

Irql: PASSIVE

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;

    LOG(HubFdo, LOG_SSH, 'ssAd', 0, Sc);
    DbgTrace((UsbhDebugTrace,"%!FUNC! \n"));

    hubExt = FdoExt(Sc->HubFdo);

    InitializeListHead(&hubExt->SshBusyListHead);
    INIT_SYNC_EVENT_SIGNALED(&hubExt->SshBusyListLock);
    INIT_SYNC_EVENT_SIGNALED(&hubExt->SshEnableDisableLock);

    UsbhInitializeTimerObject(HubFdo,
                              UsbhHubSSH_Timer,
                              &hubExt->SSH_Timer,
                              'SSHt');

    KeInitializeSemaphore(&hubExt->SSH_Lock, 1, 1);
    KeInitializeSemaphore(&hubExt->SSH_BusyListLock, 1, 1);
    INIT_EVENT_SIGNALED(&hubExt->SSH_WorkerDoneEvent);
    INIT_EVENT_SIGNALED(&hubExt->SSH_ResumeEvent);

    hubExt->SSH_EnabledStatus = SSH_ENABLED_BY_DEFAULT;
    hubExt->SSH_State = SSH_State_HubActive;
}


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
UsbhHubSSH_PnpStart(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    pnp start handler for hub selective suspend.

Arguments:

Irql: PASSIVE

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    BOOLEAN selectiveSuspendEnabled;
    NTSTATUS nts;

    UsbhAssertPassive();

    LOG(HubFdo, LOG_SSH, 'ssSt', 0, Sc);
    DbgTrace((UsbhDebugTrace,"%!FUNC! \n"));

    hubExt = FdoExt(Sc->HubFdo);

    hubExt->BusyListEnabled = TRUE;

    if (hubExt->SSH_EnabledStatus == SSH_DISABLED_VIA_WMI) {
        // SS is disabled via registry
        UsbhSshEnableDisable(HubFdo, hubExt->SSH_EnabledStatus);
    }

    // Query the state of selective suspend in the port driver
    nts = UsbhQuerySSstate(HubFdo, &selectiveSuspendEnabled);
    if NT_SUCCESS(nts) {
        if (!selectiveSuspendEnabled) {
            UsbhSshEnableDisable(HubFdo, SSH_DISABLED_VIA_USBPORT);
        } else {
            UsbhSshEnableDisable(HubFdo, SSH_ENABLED_BY_DEFAULT);
        }
    }


    // mark the bus as busy to start with, this will be cleared by the bus engine(busm)
    UsbhSshBusBusy(HubFdo, Sc);

    hubExt->SSH_HubActivity = TRUE;

    //
    // It is worth noting that we are not using the 
    // value stored in IdleTimeout in the hub extension 
    // for the timeout here. Instead we are keeping the 
    // legacy default value of 2 seconds here. This is 
    //  to limit the regression risk. 
    //
    UsbhEnableTimerObject(Sc->HubFdo,
                          &hubExt->SSH_Timer,
                          INITIAL_SS_TIMEOUT_FOR_HUBS_IN_MS,   
                          0,
                          Sc,
                          SIG_SSH_WORKER,
                          0);

    return STATUS_SUCCESS;

}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhHubSSH_PnpStop(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

   Stops Selective Suspend and frees resources in pnp context.

Arguments:

Irql: PASSIVE

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    LOG(HubFdo, LOG_SSH, 'ssSp', 0, Sc);
    DbgTrace((UsbhDebugTrace,"%!FUNC! \n"));

    hubExt = FdoExt(Sc->HubFdo);

    // Insure the hub is active
    UsbhSshBusBusy(HubFdo, Sc);

    UsbhDisableTimerObject(HubFdo, &hubExt->SSH_Timer);

    UsbhSshBusIdle(HubFdo, Sc);

    UsbhFreeBusyList(HubFdo);    
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshDisabled(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

   Hub selective suspend is disabled.  Increment the busy list so hub suspend never occurs.

   We only allow one 'busy handle' outstanding for this purpose.  If one was already present
   then we free the new one.

Arguments:

Irql: PASSIVE

Return Value:

   none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PSSH_BUSY_HANDLE oldBh, bh;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    bh = UsbhIncHubBusy(HubFdo, Sc, HubFdo, UsbhSshDisabled_Tag, RESUME_HUB);

    WAIT_EVENT(&hubExt->SSH_BusyListLock);

    oldBh = InterlockedCompareExchangePointer(&hubExt->SshDisabledBusyHandle,
                                               bh,
                                               NULL);

    LOG(HubFdo, LOG_SSH, 'SHD+', oldBh, bh);
    if (oldBh != NULL) {
        LOG(HubFdo, LOG_SSH, 'SD2-', oldBh, bh);
        UsbhDecHubBusy(HubFdo, Sc, bh);
    }

    UsbhDisableTimerObject(HubFdo, &hubExt->SSH_Timer);

    KeReleaseSemaphore(&hubExt->SSH_BusyListLock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshEnabled(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

   Hub Selective Suspend is enabled. Allows the hub to suspend when it is goes idle.

Arguments:

Irql: PASSIVE

Return Value:
--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PSSH_BUSY_HANDLE bh;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    WAIT_EVENT(&hubExt->SSH_BusyListLock);

    bh = InterlockedExchangePointer(&hubExt->SshDisabledBusyHandle, NULL);

    LOG(HubFdo, LOG_SSH, 'SHD-', 0, bh);
    UsbhDecHubBusy(HubFdo, Sc, bh);

    UsbhEnableTimerObject(Sc->HubFdo,
                          &hubExt->SSH_Timer,
                          INITIAL_SS_TIMEOUT_FOR_HUBS_IN_MS,   
                          0,
                          Sc,
                          SIG_SSH_WORKER,
                          0);

    KeReleaseSemaphore(&hubExt->SSH_BusyListLock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshEnterSx(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

   System is entering a low-power system state.  Increment the busy list so hub is resumed if suspended.

   We only allow one 'Sx busy handle' outstanding for this purpose.  If one was already present
   then we free the new one.

Arguments:

Irql: PASSIVE

Return Value:

   none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PSSH_BUSY_HANDLE oldBh, bh;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    bh = UsbhIncHubBusy(HubFdo, Sc, HubFdo, UsbhSshEnterSx_Tag, RESUME_HUB);

    WAIT_EVENT(&hubExt->SSH_BusyListLock);

    oldBh = InterlockedCompareExchangePointer(&hubExt->SshSxBusyHandle,
                                               bh,
                                               NULL);

    LOG(HubFdo, LOG_SSH, 'HSx+', oldBh, bh);
    if (oldBh != NULL) {
        LOG(HubFdo, LOG_SSH, 'HSx2', oldBh, bh);
        UsbhDecHubBusy(HubFdo, Sc, bh);
    }
    
    UsbhDisableTimerObject(HubFdo, &hubExt->SSH_Timer);
    
    KeReleaseSemaphore(&hubExt->SSH_BusyListLock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshExitSx(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

   System is exiting lower-power system state. Allow the hub to suspend when it is goes idle.

Arguments:

Irql: PASSIVE

Return Value:
--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PSSH_BUSY_HANDLE bh;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    WAIT_EVENT(&hubExt->SSH_BusyListLock);

    bh = InterlockedExchangePointer(&hubExt->SshSxBusyHandle, NULL);

    LOG(HubFdo, LOG_SSH, 'HSx-', 0, bh);
    UsbhDecHubBusy(HubFdo, Sc, bh);

    UsbhEnableTimerObject(Sc->HubFdo,
                          &hubExt->SSH_Timer,
                          INITIAL_SS_TIMEOUT_FOR_HUBS_IN_MS,   
                          0,
                          Sc,
                          SIG_SSH_WORKER,
                          0);

    KeReleaseSemaphore(&hubExt->SSH_BusyListLock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);
}


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshPortsBusy(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc,
    __in __drv_strictTypeMatch(__drv_typeConst)
    RESUME_HUB_FLAG ResumeHubFlag
    )
/*++

Routine Description:

   One or more PDOs attached to the hub are no longer idle.  Increment the busy list so hub is 
   resumed if suspended, and hub selective suspend will not occur.

   We only allow one 'Ports busy handle' outstanding for this purpose.  If one was already present
   then we free the new one.

Arguments:

Irql: PASSIVE

Return Value:

   none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PSSH_BUSY_HANDLE oldBh, bh;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    bh = UsbhIncHubBusy(HubFdo, Sc, HubFdo, UsbhSshPortsBusy_Tag, ResumeHubFlag);
    oldBh = InterlockedCompareExchangePointer(&hubExt->SshPortsBusyHandle,
                                               bh,
                                               NULL);

    LOG(HubFdo, LOG_SSH, 'HSP+', oldBh, bh);
    if (oldBh != NULL) {
        LOG(HubFdo, LOG_SSH, 'HSP2', oldBh, bh);
        UsbhDecHubBusy(HubFdo, Sc, bh);
    }

    return;
}


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshPortsIdle(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

   All attached devices are in a low power state. Allow the hub to suspend when it is goes idle.

Arguments:

Irql: PASSIVE

Return Value:
--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    PSSH_BUSY_HANDLE bh;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    
    bh = InterlockedExchangePointer(&hubExt->SshPortsBusyHandle, NULL);

    LOG(HubFdo, LOG_SSH, 'HSP-', 0, bh);
    UsbhDecHubBusy(HubFdo, Sc, bh);

    return;
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshSetPortsBusyState(
    __in PDEVICE_OBJECT HubFdo,
    __in USHORT PortNumber,
    __in __drv_strictTypeMatch(__drv_typeConst)
    PORT_BUSY_STATE PortBusyState,
    __in __drv_strictTypeMatch(__drv_typeConst)
    RESUME_HUB_FLAG ResumeHubFlag
    )
/*++

Routine Description:

   Tracks the device power state for all attached PDOs.  Increments or decrements the hub busy list as appropriate.

Arguments:
    HubFdo - Fdo for the hub

    PortNumber - Port number for the attached PDO

    PortBusyState - Idle state of the device

    ResumeHubFlag - Determines if the hub should be resumed on transition to busy.  


Irql: PASSIVE

Return Value:
--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    ULONG oldBusyMask, newBusyMask;
    USHORT MaskIndex, MaskBit;

    UsbhAssertPassive();

    //We allow 1-127 possible ports on the hub
    if ((PortNumber == 0) || (PortNumber > 127)) {
        UsbhAssertMsg(HubFdo, "PortNumber must be from 1-127 range", FALSE);
        return;
    }

    hubExt = FdoExt(HubFdo);

    WAIT_EVENT(&hubExt->SSH_BusyListLock);

    // A 128-bit bitmask (PortBusyMask[]) is used to represent the coelesced idle state of the hub's ports.
    // Each bit represents a port on the hub, numbered 1 to the number of ports on the hub.
    // If a device is attached and in D0, the corresponding bit is set to 1.  
    // No device attached, or the device is attached but in a low-power state sets the bit to 0.
    // A non-zero value for the bitmask indicates one or more of the attached
    // devices is not idle, and the hub must remain active.

    oldBusyMask = hubExt->PortBusyMask[0] || hubExt->PortBusyMask[1] || hubExt->PortBusyMask[2] || hubExt->PortBusyMask[3];
    MaskIndex = PortNumber / 32;
    MaskBit = PortNumber % 32;

    switch (PortBusyState) {
    case PORT_BUSY:
        // Set the port's bit
        hubExt->PortBusyMask[MaskIndex] |= (1<<MaskBit);
    
        LOG(HubFdo, LOG_SSH, 'SsPB', oldBusyMask, PortNumber);
        
        if (!oldBusyMask) {
            // We have transitioned from all ports being idle to one or more ports
            // being busy state.  Increment the hub busy list.
            UsbhSshPortsBusy(HubFdo, SSH_API_CONTEXT(HubFdo), ResumeHubFlag);
            UsbhDisableTimerObject(HubFdo, &hubExt->SSH_Timer);
        }
        break;

    case PORT_IDLE:
        // Clear the port's bit
        hubExt->PortBusyMask[MaskIndex] &= (~(1<<MaskBit));
        newBusyMask = hubExt->PortBusyMask[0] || hubExt->PortBusyMask[1] || hubExt->PortBusyMask[2] || hubExt->PortBusyMask[3];
                
        LOG(HubFdo, LOG_SSH, 'SsPI', oldBusyMask, PortNumber);

        if (oldBusyMask && !newBusyMask) {
            // We have transitioned from one or more ports being busy to all ports being idle,
            // so decrement the hub busy list and allow the hub to go idle
            UsbhSshPortsIdle(HubFdo, SSH_API_CONTEXT(HubFdo));
            UsbhEnableTimerObject(HubFdo,
                                  &hubExt->SSH_Timer,
                                  hubExt->IdleTimeoutInMilliSeconds,
                                  0,
                                  SSH_API_CONTEXT(HubFdo),
                                  SIG_SSH_WORKER,
                                  0);
           
        }
        break;

    default:
        // invalid port busy state
        UsbhAssertMsg(HubFdo, "Invalid port busy state", FALSE);
    }

    KeReleaseSemaphore(&hubExt->SSH_BusyListLock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);

}


VOID
UsbhSShResumeWorker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context
    )
 /*++

Routine Description:

    This is the worker routine that resumes the hub.

Arguments:

Return Value:

    none

--*/
{
    UsbhAssertPassive();

    LOG(HubFdo, LOG_SSH, 'SsRW', HubFdo, PortNumber);
    Usbh_SSH_Event(HubFdo, SSH_Event_ResumeHub, Context);
}

VOID
UsbhHubSSH_Worker(
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context
    )
 /*++

Routine Description:

    This is the worker routine that 'checks for idle' .

Arguments:

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;

    hubExt = FdoExt(HubFdo);

    UsbhAssertPassive();

    Usbh_SSH_Event(HubFdo, SSH_Event_IdleTimer, Context);

    SET_EVENT(&hubExt->SSH_WorkerDoneEvent);
}



ULONG
UsbhHubSSH_Timer(
    PHUB_TIMER_OBJECT TimerObject,
    PDEVICE_OBJECT HubFdo,
    ULONG PortNumber,
    PVOID Context,
    ULONG Parameter1,
    ULONG Parameter2
    )
/*++

Routine Description:

    Queues the SSH idle worker routine.

Arguments:

Irql: DISPATCH

Return Value:

    nonzero = retry

--*/

{
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);
    // queue worker and reschedule,
    // the timer is always re-queued -- if the worker fails to be queued it will
    // be retried on the next timer event.

    if (UsbhEventSignaled(&hubExt->SSH_WorkerDoneEvent)) {
        // no worker pending

        RESET_EVENT(&hubExt->SSH_WorkerDoneEvent);
        nts = UsbhQueueWorkItem(HubFdo,
                                DelayedWorkQueue,
                                UsbhHubSSH_Worker,
                                SSH_TIMER_CONTEXT(HubFdo),
                                0,
                                SIG_SSH_WORKER);

        if (NT_ERROR(nts)) {
            SET_EVENT(&hubExt->SSH_WorkerDoneEvent);
        }
    }

    // do not requeue the timer.
    return 0;
}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
UsbhSshPropagateResume(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc,
    __in PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Propagates the resume IOCTL to the driver stack for a given PDO.

    This message is handled by the device stack FDO (hub).  It is sent to the top
    of the device stack for the PDO.

Arguments:

    Pdo - latched Pdo

Irql: DISPATCH

Return Value:

    STATUS_SUCCESS if we are able to propagate the request.


--*/
{
    PDEVICE_EXTENSION_PDO pdoExt;
    NTSTATUS nts = STATUS_SUCCESS;

    pdoExt = PdoExt(Pdo);
    ASSERT_PDOEXT(pdoExt);
    
    UsbhAssertPassive();

    LOG(HubFdo, LOG_SSH, 'SsPR', HubFdo, Pdo);

    if (pdoExt->PdoFlags.DeviceIsHub) {
        HUB_PDO_HWPNP_STATE curState;

        nts = UsbhSendSynchronousUsbIoctlRequest(HubFdo,
                                                 Pdo,
                                                 IOCTL_INTERNAL_USB_PROPAGATE_RESUME);

        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_RESUME_PROPAGATE, pdoExt->PortNumber, nts);

        UsbhAcquirePdoStateLock(HubFdo, Sc, PdoEv_SShResume);
        curState = GET_PDO_HWPNPSTATE(Pdo);
        UsbhReleasePdoStateLock(HubFdo, Sc);

        // Only mark the port as busy if the device is still present
        switch (curState) {
        case Pdo_Created:
        case Pdo_HwPresent:
        case Pdo_PnpRefHwPresent:
            //
            // Even though we are not using D IRPs for hub selective
            // suspend, we are changing the power state of the PDO.
            // This enables a power user to look at the power state 
            // of the hub from device manager and determine if it is
            // suspended
            //
            UsbhSet_Pdo_Dx(Pdo,PowerDeviceD0);
            break;
        }
    }

    return nts;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
UsbhSshResumePort(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc,
    __inout PHUB_PORT_DATA Pd
    )
/*++

Routine Description:

    Performs a resume to one of the hubs ports.

    If: no device
        return success;
    else
        resume the port and propagate the resume message downstream 

Arguments:

    Pd = PortData for the port in question.

Irql: DISPATCH

Return Value:

    STATUS_SUCCESS if we are able to propagate the request.

--*/
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_OBJECT latch_pdo;
    ULONG ps = 0;

    UsbhAssertPassive();
    DbgTrace((UsbhDebugTrace,">%!FUNC!\n"));

    hubExt = FdoExt(HubFdo);

    do {
        NTSTATUS resume_nts;

        latch_pdo = UsbhLatchPdo(HubFdo, Pd->PortNumber, NULL, PdoGlobalSuspend_Tag);

        if (latch_pdo == NULL) {
            LOG(HubFdo, LOG_SSH, 'ssR5', Pd->PortNumber, 0);

            nts = STATUS_SUCCESS;
            break;
        }

        UsbhKdPrint((1, "%p<PDO SSH resume port[%d] %p HUB# %d>\n", HubFdo, Pd->PortNumber,
            latch_pdo, hubExt->HubNumber));

        LOG(HubFdo, LOG_SSH, 'ssR2', Pd->PortNumber, latch_pdo);

        // wait for the resume to complete

        // we have a fairly large timeout here since under certian stress conditions (verifier and
        // dma checking) we see a timeout due to the fact that the actual resume has not been
        // dispatched to the device.

        LOG(HubFdo, LOG_SSH, 'ssR3', 0, Pd->PortNumber);

        do {

            KIRQL rs_irql;

             // validate our upstream status before attempting to wait
            resume_nts = UsbhFdoCheckUpstreamConnectionState(HubFdo, &ps);

            if (NT_ERROR(resume_nts)) {
                LOG(HubFdo, LOG_SSH, 'ssRX', Pd->PortNumber, resume_nts);
                break;
            }

            KeAcquireSpinLock(&hubExt->PortResetSpin, &rs_irql);
            if (Pd->PortHwPendingState == PortHw_ResumePending) {
                KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);

                resume_nts = UsbhWaitEventWithTimeout(HubFdo, &Pd->PortResumeEvent, 500, 'ssRT');
            } else {
                // port not waiting on resume
                KeReleaseSpinLock(&hubExt->PortResetSpin, rs_irql);
                resume_nts = STATUS_SUCCESS;
            }

            if (resume_nts == STATUS_TIMEOUT) {
                // we handle this case better by dispatching a separate timeout event into the queue and
                // waiting again this allows us to detect the case where the system is running very slowly.
                // the timout will reset the event to the appropriate state so we can wait on it again.
                UsbhPCE_ResumeTimeout(HubFdo, Sc, Pd->PortNumber);
                continue;
            }

        } while (resume_nts == STATUS_TIMEOUT);

        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_RESUME_PORT, Pd->PortNumber, resume_nts);

        // port resumed or disconnected, now propagate the resume if necessary thru the next hub.
        // we always propagate the message so that the child can clean up.

        // this is the same codepath as 'selective' suspend thru power.
        nts = UsbhSshPropagateResume(HubFdo, Sc, latch_pdo);

        LOG(HubFdo, LOG_SSH, 'ssR4', Pd->PortNumber, latch_pdo);

        UsbhKdPrint((1, "%p>PDO SSH resume port[%d] %p %08.8x HUB# %d>\n",
            HubFdo, Pd->PortNumber, latch_pdo, nts, hubExt->HubNumber));

        UsbhUnlatchPdo(HubFdo, latch_pdo, NULL, PdoGlobalSuspend_Tag);

    } WHILE (0);

    return nts;

}


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
UsbhSshSuspendHub(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    Issue a suspend for this hub.
    
Arguments:


Irql: PASSIVE

Return Value:

    none

--*/
{
    NTSTATUS nts;
    HUB_BUS_STATE bs;
    PDEVICE_EXTENSION_HUB hubExt;
    ULONG ps = 0;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    nts = UsbhFdoCheckUpstreamConnectionState(HubFdo, &ps);

    if (NT_ERROR(nts)) {
        // Hub is gone.  Send suspend failure.
        LOG(HubFdo, LOG_SSH, 'SuSx', 0, nts);
        hubExt->SSH_FailedToSuspend = TRUE;
        Usbh_SSH_Event(HubFdo, SSH_Event_SuspendHubFailed, Sc);
        SET_EVENT(&hubExt->SSH_ResumeEvent);
        return STATUS_UNSUCCESSFUL;
    }

    LOG(HubFdo, LOG_SSH, 'SusH', Sc, 0);

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_SUSPEND_START, 0, STATUS_SUCCESS);

    UsbhArmHubWakeOnConnect(HubFdo);

    bs = UsbhSyncBusPause(HubFdo, Sc, PauseReason_SelectiveSuspend);

    if (bs != BS_BusPause) {
        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_SUSPEND_BUS_PAUSE_FAILED, 0, STATUS_UNSUCCESSFUL);

        // Pause request failed, so disarm hub and return failure.
        UsbhDisarmHubWakeOnConnect(HubFdo);
        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_SUSPEND_COMPLETE, 0, STATUS_UNSUCCESSFUL);
        hubExt->SSH_FailedToSuspend = TRUE;
        Usbh_SSH_Event(HubFdo, SSH_Event_SuspendHubFailed, Sc);
        SET_EVENT(&hubExt->SSH_ResumeEvent);
        return STATUS_UNSUCCESSFUL;
    }
    
    // Suspend the hub
    nts = Usbh_BusIf_SuspendHub(HubFdo);

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_SUSPEND_COMPLETE, 0, nts);
    if (!NT_SUCCESS(nts)) {
        // Unable to suspend the hub's upstream port.
        // Resume the bus and return to the active state.
        UsbhSyncBusResume(HubFdo, Sc);
        UsbhDisarmHubWakeOnConnect(HubFdo);
        Usbh_SSH_Event(HubFdo, SSH_Event_SuspendHubFailed, Sc);
        SET_EVENT(&hubExt->SSH_ResumeEvent);
        HUB_EXCEPTION(USBFILE, HubFdo, 0, SsErr_FailedSshSuspendHub, nts, 0, NULL, 0);

        UsbhEnableTimerObject(HubFdo,
                              &hubExt->SSH_Timer,
                              hubExt->IdleTimeoutInMilliSeconds,
                              0,
                              SSH_API_CONTEXT(HubFdo),
                              SIG_SSH_WORKER,
                              0);

    } else {
        UsbhDisableTimerObject(HubFdo, &hubExt->SSH_Timer);
        Usbh_SSH_Event(HubFdo, SSH_Event_SuspendHubSucceeded, Sc);
    }

    return nts;
}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
UsbhSshResumeUpstream(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc
    )
/*++

Routine Description:

    Reflects the resume request upstream.  
    This function is synchronous.
    
Arguments:

Irql: PASSIVE

Return Value:

    none

--*/
{
    NTSTATUS nts;
    ULONG ps = 0;
    PDEVICE_EXTENSION_HUB hubExt;
    SSH_HUB_STATE state;
    PSSH_BUSY_HANDLE bh;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    LOG(HubFdo, LOG_SSH, 'RsUp', Sc, 0);

    // validate our upstream status
    nts = UsbhFdoCheckUpstreamConnectionState(HubFdo, &ps);

    // Insure the hub does not re-suspend before we check the hub state
    bh = UsbhIncHubBusy(HubFdo, Sc, HubFdo, UsbhSshResumeUpstream_Tag, NO_RESUME_HUB);

    if NT_SUCCESS(nts) {
        // Propagate the resume request upstream to our parent.  The parent
        // will in turn propagate resume downstream.
        nts = Usbh_BusIf_ResumeHub(HubFdo);
    }

    UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_RESUME_UPSTREAM, 0, nts);

    // It's possible the resume did not propagate back down to this hub instance.
    // Check the state of the hub and verify it's still not pending resume.
    WAIT_EVENT(&hubExt->SSH_Lock);
    state = hubExt->SSH_State;
    KeReleaseSemaphore(&hubExt->SSH_Lock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);

    if (state == SSH_State_HubPendingResume) {
        // Hub is still pending resume, indicating the resume didn't propagate back down.
        // Signal resume completion and return.
        Usbh_SSH_Event(HubFdo, SSH_Event_ResumeHubFailed, Sc);
        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_RESUME_COMPLETE, 0, nts);
        SET_EVENT(&hubExt->SSH_ResumeEvent);
        HUB_EXCEPTION(USBFILE, HubFdo, 0, SsErr_FailedSshResumeHub, nts, 0, NULL, 0);
    }

    if (bh) {
        UsbhDecHubBusy(HubFdo, Sc, bh);
    }

    LOG(HubFdo, LOG_SSH, 'RsUx', 0, nts);

    return nts;
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshResumeDownstream(
    __in PDEVICE_OBJECT HubFdo,
    __inout PSTATE_CONTEXT Sc,
    __in BOOLEAN ResumeBus
    )
/*++

Routine Description:

    Propagates the resume to all downstream devices.  
    This function is synchronous.
    
Arguments:

Irql: PASSIVE

Return Value:

    none

--*/
{
    NTSTATUS nts;
    USHORT p;
    PHUB_PORT_DATA pd;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_EXTENSION_PDO pdoExt = NULL;
    HUB_PDO_POWER_STATE pdoPowerState = {0};
    PDEVICE_OBJECT pdo;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    
    LOG(HubFdo, LOG_SSH, 'RsDn', Sc, 0);


    for(p=1; p <= N_PORTS(HubFdo); p++) {
        pd = UsbhGetPortData(HubFdo, p);
        if (pd) {
            pdo = pd->PnpPdo;
            if (pdo) {
                pdoExt = PdoExt(pdo);

                //
                // Check if a D3Cold device was powered off during suspend
                //
                WAIT_EVENT(&pdoExt->D3ColdLock);
                if (pdoExt->PdoFlags.InD3Cold == 1) {
                    
                    USB_HUB_PORT_STATE ps;
                    USBD_STATUS usbdStatus;

                    nts = UsbhQueryPortState(HubFdo,
                                             p,
                                             &ps,
                                             &usbdStatus);

                    LOG(HubFdo, LOG_SSH, 'SSD3', nts, ps.Status.us);

                    if (NT_SUCCESS(nts)) {
                        if (ps.Status.Enable == 0) {

                            pdoExt->PdoFlags.D3ColdWaitForReconnect = 1;
                            pdoExt->PdoFlags.ResetOnLastResume = 1;

                            if (ps.Status.Connect == 0) {
                            // Block D0 until device reconnects
                                RESET_EVENT(&pdoExt->D3ColdReconnectEvent);
                            } else {
                                SET_EVENT(&pdoExt->D3ColdReconnectEvent);
                            }
                        }
                    }
                }
                SET_EVENT(&pdoExt->D3ColdLock);
            }
        }
    }

    if (ResumeBus == TRUE) {
        // At this point the upstream port has been resumed.  Now resume the hub's bus state machine
        UsbhSyncBusResume(HubFdo, Sc);
    }

    UsbhDisarmHubWakeOnConnect(HubFdo);
    
    // Loop through all the ports and propagate the resume downstream
    for(p=1; p <= N_PORTS(HubFdo); p++) {
        pd = UsbhGetPortData(HubFdo, p);
        if (pd) {
            pdo = pd->PnpPdo;
            if (pdo) {
                pdoExt = PdoExt(pdo);
                pdoPowerState = GET_PDO_POWER_STATE(pdo);

                // Skip resume if D3Cold device is not connected
                if (pdoExt->PdoFlags.D3ColdWaitForReconnect == 1) {
                    continue;
                }
            }

            nts = UsbhSshResumePort(HubFdo, Sc, pd);            
            if (NT_SUCCESS(nts) &&
                pdo &&
                !(pdoExt->PdoFlags.DeviceIsHub) && 
                !(pdoExt->PdoFlags.DeviceArmedForWake) &&
                (pdoPowerState == Pdo_Dx)) {

                // Device is not armed for wake and not requesting D0, so put it back into suspend
                pd->PD_Flags.ReSuspendPort = 1;    
                nts = UsbhSyncSuspendPdoPort(PDO_CONTEXT(pdo), pdo, FALSE);
                pd->PD_Flags.ReSuspendPort = 0;    
            }

            if (NT_ERROR(nts) && !Usb_Disconnected(nts)) {
                HUB_EXCEPTION(USBFILE, HubFdo, p, SsErr_FailedSshResumeOnPort, nts, 0, NULL, 0);
                TEST_TRAP();
            }
        }
    }

    if (ResumeBus == TRUE) {

        // At this point the hub and all its downstream devices not in D3 should be resumed.
        // Transition to the active state and signal resume
        Usbh_SSH_Event(HubFdo, SSH_Event_ResumeHubComplete, Sc);
        UsbhEtwLogHubEvent(HubFdo, &USBHUB_ETW_EVENT_HUB_RESUME_COMPLETE, 0, STATUS_SUCCESS);
        SET_EVENT(&hubExt->SSH_ResumeEvent);

        UsbhEnableTimerObject(HubFdo,
                              &hubExt->SSH_Timer,
                              hubExt->IdleTimeoutInMilliSeconds, 
                              0,
                              Sc,
                              SIG_SSH_WORKER,
                              0);

    }
}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_BusIf_SuspendHub(
    __in PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Sends a request to the parent driver to suspend the upstream port.
    
Arguments:


Irql: PASSIVE

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_BUS_INTERFACE_HUB_SELECTIVE_SUSPEND SSH_Interface;
    NTSTATUS nts;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);
    SSH_Interface = &hubExt->HubSSInterface;

    if (!SSH_Interface->SuspendHub) {
        UsbhAssertMsg(HubFdo, "SuspendHub function should not be NULL", FALSE);
        nts = STATUS_NOT_SUPPORTED;
    } else {
        UsbhKdPrint((1, "Hub #%d FDO (%p) *suspend hub**\n", hubExt->HubNumber, HubFdo));
        nts = SSH_Interface->SuspendHub(hubExt->PhysicalDeviceObject);
    }
    return nts;
}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
Usbh_BusIf_ResumeHub(
    __in PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Sends a request to the parent driver to resume the upstream port.
    
Arguments:


Irql: PASSIVE

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_BUS_INTERFACE_HUB_SELECTIVE_SUSPEND SS_Interface;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);
    SS_Interface = &hubExt->HubSSInterface;

    if (!SS_Interface->ResumeHub) {
        UsbhAssertMsg(HubFdo, "ResumeHub function should not be NULL", FALSE);
        return STATUS_NOT_SUPPORTED;
    } else {
        UsbhKdPrint((1, "Hub #%d FDO (%p) *resume hub**\n", hubExt->HubNumber, HubFdo));
        nts = SS_Interface->ResumeHub(hubExt->PhysicalDeviceObject);
    }
    return nts;
}


/*

typedef NTSTATUS (*PPOWER_SETTING_CALLBACK) (
    IN CONST LPGUID SettingGuid,
    IN PVOID        Value,
    IN ULONG        ValueLength,
    IN PVOID        Context
);

#if (NTDDI_VERSION >= NTDDI_VISTA)
NTKERNELAPI
NTSTATUS
PoRegisterPowerSettingCallback (
    __in_opt PDEVICE_OBJECT         DeviceObject,
    __in LPCGUID                    SettingGuid,
    __in PPOWER_SETTING_CALLBACK    Callback,
    __in_opt PVOID                  Context,
    __out_opt PVOID                 * Handle
    );

GUID_HIBERNATE_FASTS4_POLICY

*/
NTSTATUS
UsbhPowerCallback(
    LPCGUID SettingGuid,
    PVOID NewValue,
    ULONG ValueLength,
    PVOID Context
    );

VOID
UsbhRegisterPowerCallback(
    PDEVICE_OBJECT HubFdo
    )
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;

    UsbhAssertPassive();
    DbgTrace((UsbhDebugTrace,">%!FUNC!\n"));

    hubExt = FdoExt(HubFdo);

    nts = PoRegisterPowerSettingCallback(HubFdo,
                                         &GUID_USB_SETTING_SELECTIVE_SUSPEND,
                                         UsbhPowerCallback,
                                         hubExt,
                                         &hubExt->PowerCbHandle);

    if (NT_SUCCESS(nts)) {
        hubExt->PowerCbRegistered = 1;
    }

}

VOID
UsbhUnregisterPowerCallback(
    PDEVICE_OBJECT HubFdo
    )
{
    NTSTATUS nts;
    PDEVICE_EXTENSION_HUB hubExt;

    UsbhAssertPassive();
    DbgTrace((UsbhDebugTrace,">%!FUNC!\n"));

    hubExt = FdoExt(HubFdo);

    if (hubExt->PowerCbRegistered){

        nts = PoUnregisterPowerSettingCallback(hubExt->PowerCbHandle);

        if (NT_SUCCESS(nts)) {
            hubExt->PowerCbRegistered = 0;
        }
    }

}

NTSTATUS
UsbhPowerCallback(
    LPCGUID SettingGuid,
    PULONG NewValue,
    ULONG ValueLength,
    PVOID Context
    )
{
    NTSTATUS nts = STATUS_INVALID_PARAMETER;
    PDEVICE_EXTENSION_HUB hubExt;
    PDEVICE_OBJECT HubFdo;

    UsbhAssertPassive();
    DbgTrace((UsbhDebugTrace,">%!FUNC!\n"));

    hubExt = Context;
    HubFdo = hubExt->FunctionalDeviceObject;

    do {

        if (hubExt == NULL) {
            nts = STATUS_INVALID_PARAMETER;
            break;
        }

        UsbhAssert(NULL, hubExt->ExtensionType == EXTENSION_TYPE_HUB);

        if (RtlCompareMemory(SettingGuid,
                             (LPGUID) &GUID_USB_SETTING_SELECTIVE_SUSPEND,
                             sizeof(GUID)) != sizeof(GUID)) {

           break;
        }

        if (ValueLength != sizeof(ULONG) || NewValue == NULL) {
            nts = STATUS_INVALID_PARAMETER;
            break;
        }

        if (*NewValue != 0) {
            hubExt->PowerSaverMode = power_pincher;

        } else {
            hubExt->PowerSaverMode = power_perf;
        }
        
        nts = STATUS_SUCCESS;
        
        // reset selective suspend based on power save mode
        switch (hubExt->PowerSaverMode) {
        case power_pincher:
            UsbhSshEnableDisable(HubFdo, SSH_ENABLED_VIA_POWER_POLICY);
            break;

        default:
            UsbhSshEnableDisable(HubFdo, SSH_DISABLED_VIA_POWER_POLICY);
            break;
        }

    }  WHILE (0);

    return nts;
}


__drv_maxIRQL(PASSIVE_LEVEL)
VOID
UsbhSshEnableDisable(
    __in PDEVICE_OBJECT HubFdo,
    __in SSH_ENABLE_STATUS SshEnabledStatus
    )
/*++

Routine Description:

    Enables/disables the hub SSH state machine.

    The priority of enable/disable for hub selective suspend is as follows:

        1. Disabled via USBPORT (also disables device idle)
        2. Enabled/disabled via WMI ("Allow this device to enter a
           lower power state" checkbox in Device Manager).
        3. Enabled/disabled by system power policy.
        4. Enabled by default.
    
Arguments:
    HubFdo - FDO for the hub.
    
    SshEnabledStatus - Enable status for hub selective suspend.


Irql: PASSIVE

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    WAIT_EVENT(&hubExt->SshEnableDisableLock);

    // Insure that the system power policy does not override the user selected
    // selective suspend policy from Device Manager.
    switch (hubExt->SSH_EnabledStatus) {
    case SSH_ENABLED_BY_DEFAULT:
    case SSH_ENABLED_VIA_POWER_POLICY:
    case SSH_DISABLED_VIA_POWER_POLICY:

        // If current policy is via global power policy or default, allow it to be 
        // overridden
        switch (SshEnabledStatus) {
        case SSH_ENABLED_VIA_POWER_POLICY:
        case SSH_ENABLED_VIA_WMI:
            UsbhSshEnabled(HubFdo, SSH_API_CONTEXT(HubFdo));
            hubExt->SSH_EnabledStatus = SshEnabledStatus;
            break;
        case SSH_DISABLED_VIA_POWER_POLICY:
        case SSH_DISABLED_VIA_WMI:
        case SSH_DISABLED_VIA_USBPORT:
            UsbhSshDisabled(HubFdo, SSH_API_CONTEXT(HubFdo));
            hubExt->SSH_EnabledStatus = SshEnabledStatus;
            break;
        }
        break;

    // Nothing can override disable via USBPORT
    case SSH_DISABLED_VIA_USBPORT:
        UsbhSshDisabled(HubFdo, SSH_API_CONTEXT(HubFdo));
        break;

    case SSH_ENABLED_VIA_WMI:
    case SSH_DISABLED_VIA_WMI:
        // If current policy is set via WMI, only another WMI SS event can override it
        switch (SshEnabledStatus) {
        case SSH_ENABLED_VIA_WMI:
            UsbhSshEnabled(HubFdo, SSH_API_CONTEXT(HubFdo));
            hubExt->SSH_EnabledStatus = SshEnabledStatus;
            break;
        case SSH_DISABLED_VIA_WMI:
        case SSH_DISABLED_VIA_USBPORT:
            UsbhSshDisabled(HubFdo, SSH_API_CONTEXT(HubFdo));
            hubExt->SSH_EnabledStatus = SshEnabledStatus;
            break;
        }
        break;
    }
    SET_EVENT(&hubExt->SshEnableDisableLock);
}
