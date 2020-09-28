/*++
Copyright (c) Microsoft Corporation

Module Name:

    controller.c

Abstract:

    USB Controller Extension Driver

--*/

#include "triage-ucx01000.h"

//
// Define lock ordering for SAL/ESPC.
//
_Create_lock_level_(TopologyLockLevel);
_Create_lock_level_(EndpointTrackingLockLevel);

typedef struct _UCX_FORWARD_PROGRESS_WORKITEM {
    ULONG                                   Signature;
    LIST_ENTRY                              ListEntry;
    PUCXCONTROLLER_PRIVCONTEXT              UcxControllerContext;
    PDEVICE_OBJECT                          WdmDeviceObject;
    PIO_WORKITEM                            WdmWorkItem;    
    PUCX_FORWARD_PROGRESS_WORKITEM_CALLBACK Callback;
    PVOID                                   CallbackContext;
    KSPIN_LOCK                              RefCountLock;
    ULONG                                   RefCount;
    KEVENT                                  CompletionEvent;
    UCX_FORWARD_PROGRESS_WORKITEM_FLAGS     Flags;
} UCX_FORWARD_PROGRESS_WORKITEM, *PUCX_FORWARD_PROGRESS_WORKITEM;

typedef struct _UCXROOTHUB_PRIVCONTEXT * PUCXROOTHUB_PRIVCONTEXT;

typedef struct _UCXCONTROLLER_PRIVCONTEXT {
    UCX_SIG                 Sig;
    ULONG                   PadToOffset8;
    PUCX_TRIAGE_INFO        UcxTriageInfo;
    WDFDEVICE               ParentWdfDevice;
    LIST_ENTRY              UcxControllerListEntry;
    UCXCONTROLLER           UcxController;
    UCX_CONTROLLER_CONFIG   Config;
    WDFQUEUE                QueueAddress0Ownership;
    WDFQUEUE                QueueUsbDeviceMgmt;
    WDFQUEUE                QueuePendUsbDeviceMgmtDuringControllerReset;
    WDFQUEUE                QueueTreePurge;
    WDFQUEUE                QueueUcxDefault;
    WDFSTRING               SymbolicNameForHostControllerInterface;
    PUCXROOTHUB_PRIVCONTEXT UcxRootHubContext;
    DEVICE_CAPABILITIES     HcCaps;

    IO_CSQ                  AbortPipeIrpCsq;
    KSPIN_LOCK              AbortPipeIrpCsqLock;

    //
    // A lock used to control list of Children (Endpoints or Devices) 
    // for any UcxDevice object
    //
    _Has_lock_level_(TopologyLockLevel)
    KSPIN_LOCK              TopologyLock;

    //
    // Total number of all UcxUsbDevice children linked to the child
    // device tree under this controller, including the root hub.
    // Protected by TopologyLock.
    //
    ULONG                   ChildListCount;

    //
    // Total number of all UcxEndpoints linked to all UcxUsbDevice
    // children linked to the child device tree under this controller,
    // including the root hub.  Protected by TopologyLock.
    //
    ULONG                   ChildEndpointListCount;

    SM_ENGINE_CONTEXT       SmEngineContext;
    KEVENT                  RHPdoMayExitD0;
    LONG                    WasDeviceProgrammingLost;
    LONG                    WasRootHubReset;

    KEVENT                  ControllerResetCompleteProcessed;
    
    ULONG                   CachedFrameNumber;
    KSPIN_LOCK              ResetLock;
    ULONG                   ResetGenerationCount;
    LONG                    ResetReferenceCount;    
    BOOLEAN                 ResetInProgress;
    BOOLEAN                 ResetInitiated;

    LONG                    ControllerFailed;
    
    BOOLEAN                 SmEngineContextInitalized;
    //
    // Keeps track if driver verifier is enable on for the HCD
    //
    BOOLEAN                 DriverVerifierEnabled;

    LONG                    PendingPrepareForResetOperations;

    KSPIN_LOCK              SystemThreadQueueLock;
    LIST_ENTRY              SystemThreadQueue;
    PKTHREAD                SystemThread;
    KEVENT                  SystemThreadWorkPending;
    
    KMUTEX                  EnableForwardProgressMutex;
    BOOLEAN                 ForwardProgressEnabledInHcd;

    //
    // This flag indicates that a Clear TT Buffer request should be
    // issued to a TT hub if a transfer is canceled on a control or bulk
    // endpoint on a low speed or full speed device downstream of the TT
    // hub.
    //
    BOOLEAN                 ClearTtBufferOnAsyncTransferCancel;

    //
    // SystemThreadWorkPaused is used to block RHPdo's D0 Exit if system thread is actively 
    // doing something. At this point, it is just for sanity check.
    //
    BOOLEAN                 SystemThreadPaused;
    BOOLEAN                 SystemThreadTerminate;
    BOOLEAN                 RHPdoInD0;
    BOOLEAN                 WaitOnControllerResetComplete;

    //
    // COMPAT:
    // Fields needed to support for Compatibility
    //
    USB_CONTROLLER_FLAVOR   ControllerFlavor;
    ULONG                   FakeFrameNumber;

    //
    // Information about how many USBD registerations are active
    //
    LONG                    USBDInterfaceCount;

    //
    // Track TreePurge of multiple devices.
    // TreePurge is serialized controller-wide, so we need only track 1.
    //
    WDFREQUEST              PendingTreePurgeRequest;
    LONG                    PendingTreePurgeEndpointCount;

} UCXCONTROLLER_PRIVCONTEXT, *PUCXCONTROLLER_PRIVCONTEXT;

C_ASSERT(FIELD_OFFSET(UCXCONTROLLER_PRIVCONTEXT, Sig) == 0);
C_ASSERT(FIELD_OFFSET(UCXCONTROLLER_PRIVCONTEXT, UcxTriageInfo) == UCX_TRIAGE_INFO_OFFSET);


WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(UCXCONTROLLER_PRIVCONTEXT, 
                                   UcxControllerGetContext);
 
typedef struct _ADDRESS0_OWNERSHIP_QUEUE_CONTEXT {

    UCX_SIG                 Sig;
    UCXCONTROLLER           UcxController;
    UCXUSBDEVICE            CurrentOwnerHub;
    // TODO: PostWin8: Replace CurrentOwnerUsbDevice with Port Number, since this is
    // essentially a port operation and a usb device may not have been created yet. 
    UCXUSBDEVICE            CurrentOwnerUsbDevice;

} ADDRESS0_OWNERSHIP_QUEUE_CONTEXT, *PADDRESS0_OWNERSHIP_QUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(ADDRESS0_OWNERSHIP_QUEUE_CONTEXT,
                                   Address0OwnershipQueueGetContext);

#define ADDRESS0_OWNERSHIP_TOTAL_FORWARD_PROGRESS_REQUESTS 1
#define USBDEVICEMGMT_TOTAL_FORWARD_PROGRESS_REQUESTS 1
#define TREE_PURGE_TOTAL_FORWARD_PROGRESS_REQUESTS 1
#define UCXDEFAULT_TOTAL_FORWARD_PROGRESS_REQUESTS 1

EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL Controller_EvtDefaultIoInternalDeviceControl;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL  Controller_EvtAddress0OwnershipIoInternalDeviceControl;

EVT_WDF_IO_WDM_IRP_FOR_FORWARD_PROGRESS Controller_EvtDefaultIoWdfIrpForForwardProgress;

VOID
Controller_Address0OwnershipRelease(
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        UCXUSBDEVICE       UsbDevice
    );


BOOLEAN
Controller_AreSuperspeedStreamsSupported(
    __in
        UCXUSBDEVICE        Hub
    );

BOOLEAN
Controller_QueryClearTtBufferOnAsyncTransferCancel(
    __in
        PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    );

KSTART_ROUTINE         Controller_SystemThread;
IO_WORKITEM_ROUTINE_EX Controller_ForwardProgressWorkItemWdmCallback;

PUCX_FORWARD_PROGRESS_WORKITEM
Controller_ForwardProgressWorkItemAllocateFromHub(
    __in
        UCXUSBDEVICE                            Hub,
    __in
        PDEVICE_OBJECT                          WdmDeviceObject,
    __in
        ULONG                                   Flags
    );

VOID
Controller_ForwardProgressWorkItemDelete(
    __in
        PUCX_FORWARD_PROGRESS_WORKITEM   UcxForwardProgressWorkItem
    );

PUCX_FORWARD_PROGRESS_WORKITEM 
Controller_ForwardProgressWorkItemAllocate(
    __in
        PUCXCONTROLLER_PRIVCONTEXT              UcxControllerContext,
    __in
        PDEVICE_OBJECT                          WdmDeviceObject,
    __in
        ULONG                                   Flags
    );

VOID
Controller_ForwardProgressWorkItemEnqueue(
    __in 
        PUCX_FORWARD_PROGRESS_WORKITEM          UcxForwardProgressWorkItem,
    __in
        PUCX_FORWARD_PROGRESS_WORKITEM_CALLBACK Callback,
    __in_opt
        PVOID                                   CallbackContext,
    __in
        UCX_FORWARD_PROGRESS_ENQUEUE_OPTIONS    ForwardProgressEnqueueOptions
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_ForwardProgressWorkItemFlush(
    __in
        PUCX_FORWARD_PROGRESS_WORKITEM UcxForwardProgressWorkitem
    );

EVT_WDF_OBJECT_CONTEXT_DESTROY Controller_EvtDestroyCallback;

#define USB_FRAME_NUBMER_MASK 0x7FF

VOID
Controller_QueryUsbCapability(
    __in 
        PUCXCONTROLLER_PRIVCONTEXT                UcxControllerContext,
    __in 
        PQUERY_USB_CAPABILITY                     UsbCapability,
    __out_bcount(UsbCapability->OutputBufferLength)
        PUCHAR                                    OutputBuffer,
    __in
        WDFREQUEST                                Request
    );

VOID
Controller_GetDumpData(
    __in 
        PUCXCONTROLLER_PRIVCONTEXT                UcxControllerContext,
    __in
        UCXUSBDEVICE                              UcxUsbDevice,
    __in
        PUSB_DUMP_DEVICE_INFO                     DumpDeviceInfo,
    __inout
        PUSB_DUMP_CONTROLLER_INFO                 DumpControllerInfo,
    __inout
        WDFREQUEST                                Request
    );

VOID
Controller_FreeDumpData(
    __in 
        PUCXCONTROLLER_PRIVCONTEXT                UcxControllerContext,
    __inout
        PUSB_DUMP_CONTROLLER_INFO                 DumpControllerInfo,
    __inout
        WDFREQUEST                                Request
    );

NTSTATUS
Controller_EnableForwardProgress(
    __in 
        PUCXCONTROLLER_PRIVCONTEXT                UcxControllerContext,
    __in
        PUSB_FORWARD_PROGRESS_INFO                ForwardProgressInfo
    );

FORCEINLINE
VOID
Controller_EventAdd(
    __in
        PUCXCONTROLLER_PRIVCONTEXT    UcxControllerContext,
    __in
        SM_ENGINE_EVENT               Event               
    )
/*++

Routine Description:

    This routine handles adding and processing new events

Arguments:

    UcxControllerContext - Ucx context of the endpoint object
    
    Event - Event that is to be added to the ControllerReset State Machine.
    
Return: 
 
    VOID    
--*/

{
    StateMachineEngine_EventAdd(&UcxControllerContext->SmEngineContext,
                                (SM_ENGINE_EVENT)Event);
    
} // Controller_EventAdd

FORCEINLINE
VOID
Controller_NoActiveResetReferences(
    __in
        PUCXCONTROLLER_PRIVCONTEXT     UcxControllerContext
    )
/*++

Routine Description:

    This function is called when it is guranteed that all reset references have been released.
    At this point it is ok to go ahead and continue the process of controller reset. 
    
Arguments:

    UcxControllerContext - pointer to the context for UCXCONTROLLER
    
Return Value:
    
    VOID
--*/
{
    TRY {
        
        Controller_EventAdd(UcxControllerContext,
                            ControllerResetEventNoActiveResetReferences);
        
    } FINALLY {
    }

}

FORCEINLINE
VOID
Controller_ResetReferenceRelease( 
    __in
        PUCXCONTROLLER_PRIVCONTEXT     UcxControllerContext 
    )
/*++

Routine Description:

    This routine is called by a caller that acquired a ResetReference by calling
    Controller_ResetReferenceAcquire
    
    This routine does few interesting this:
        If the ref count goes to 0, it implies all the ResetReferences has been released.
        
        It should be noted that multiple threads reach this ref count 0 condition, and we only 
        need one of the threads to do the reset process.
    
Arguments:

    UcxControllerContext - pointer to the context for UCXCONTROLLER
    
Return Value:
    
    VOID
--*/
{
    ULONG   resetGenerationCount;
    KIRQL   irql;

    TRY {
        
        resetGenerationCount = UcxControllerContext->ResetGenerationCount;

        if (0 != InterlockedDecrement(&UcxControllerContext->ResetReferenceCount)) {
            LEAVE;
        }

        KeAcquireSpinLock(&UcxControllerContext->ResetLock, &irql);
        
        if (UcxControllerContext->ResetInitiated) {
            //
            // Looks like the reset process has already been initiated.
            //
            KeReleaseSpinLock(&UcxControllerContext->ResetLock, irql);
            LEAVE;
        }
        
        if (resetGenerationCount != UcxControllerContext->ResetGenerationCount) {
            
            //
            // Looks like the reset process is already initiated and completed.
            //
            KeReleaseSpinLock(&UcxControllerContext->ResetLock, irql);
            LEAVE;
        }

        UcxControllerContext->ResetInitiated = TRUE;

        KeReleaseSpinLock(&UcxControllerContext->ResetLock, irql);
        
        Controller_NoActiveResetReferences(UcxControllerContext);
        
    } FINALLY {
    }
}

FORCEINLINE
BOOLEAN 
Controller_ResetReferenceAcquire( 
    __in
        PUCXCONTROLLER_PRIVCONTEXT     UcxControllerContext
    )
/*++

Routine Description:

    This function allows the caller to synchronize with the ControllerReset process.

    A caller that needs to be mutually exclusive with the controller reset must try and acquire
    this reference. 
    If the call succeeds, the controller reset will not happen until the caller calls
    Controller_ResetReferenceRelease
    
Arguments:

    UcxControllerContext - pointer to the context for UCXCONTROLLER
    
Return Value:
    
    BOOLEAN : 
        FALSE implies that controller reset might be happening
        TRUE implies that a controller reset will not happen until the call calls 
            Controller_ResetReferenceRelease
    
--*/
{
    BOOLEAN referenceAcquired;
    
    TRY {
        
        InterlockedIncrement(&UcxControllerContext->ResetReferenceCount);
        
        if (UcxControllerContext->ResetInProgress) {
            referenceAcquired = FALSE;            
            Controller_ResetReferenceRelease(UcxControllerContext);
        } else {
            referenceAcquired = TRUE;
        }

    } FINALLY {
    }

    return referenceAcquired;
}

FORCEINLINE
VOID
Controller_SetResetInProgress(
    __in
        PUCXCONTROLLER_PRIVCONTEXT     UcxControllerContext
    )
/*++

Routine Description:

    This routine is called by controller reset state machine to set a global bit saying 
    controller reset is in progress.  

Arguments:

    UcxControllerContext - pointer to the context for UCXCONTROLLER
    
Return Value:
    
    VOID
--*/
{
    KIRQL irql;

    TRY {
        KeAcquireSpinLock(&UcxControllerContext->ResetLock, &irql);
        
        NT_ASSERT(UcxControllerContext->ResetInProgress == FALSE);
        NT_ASSERT(UcxControllerContext->ResetInitiated == FALSE);
        
        UcxControllerContext->ResetInProgress = TRUE;
        
        KeReleaseSpinLock(&UcxControllerContext->ResetLock, irql);

    } FINALLY {
    }
}

FORCEINLINE
VOID
Controller_ClearResetInProgress(
    __in
        PUCXCONTROLLER_PRIVCONTEXT     UcxControllerContext
    )
{
    KIRQL irql;
    TRY {
        
        KeAcquireSpinLock(&UcxControllerContext->ResetLock, &irql);

        UcxControllerContext->ResetGenerationCount++;  
        UcxControllerContext->ResetInProgress = FALSE;
        UcxControllerContext->ResetInitiated = FALSE;

        InterlockedIncrement(&UcxControllerContext->ResetReferenceCount);

        KeReleaseSpinLock(&UcxControllerContext->ResetLock, irql);
    
    } FINALLY {

    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_PrepareUsbDevicesAndEndpointsForControllerReset(
    __in 
        PUCXCONTROLLER_PRIVCONTEXT       UcxControllerContext
);

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_NotifyUsbDevicesAndEndpointsOfControllerResetComplete(
    __in 
        PUCXCONTROLLER_PRIVCONTEXT       UcxControllerContext
);

VOID
Controller_PendingPrepareForResetOperationComplete(
    __in 
        PUCXCONTROLLER_PRIVCONTEXT       UcxControllerContext 
    );

IO_CSQ_INSERT_IRP               Controller_CsqInsertAbortPipeIrp;
IO_CSQ_REMOVE_IRP               Controller_CsqRemoveAbortPipeIrp;
IO_CSQ_PEEK_NEXT_IRP            Controller_CsqPeekNextAbortPipeIrp;
IO_CSQ_ACQUIRE_LOCK             Controller_CsqAcquireAbortPipeIrpLock;
IO_CSQ_RELEASE_LOCK             Controller_CsqReleaseAbortPipeIrpLock;
IO_CSQ_COMPLETE_CANCELED_IRP    Controller_CsqCompleteCancelledAbortPipeIrp;

NTSTATUS
Controller_GetCurrentFrameNumber(
    __in 
        PUCXCONTROLLER_PRIVCONTEXT UcxControllerContext,
    __out 
        PULONG                     CurrentFrame
    );

FORCEINLINE
VOID
Controller_VerifierBreak(
    __in
        PUCXCONTROLLER_PRIVCONTEXT UcxControllerContext,
    __in
        LPSTR                      Reason
    )
{
    UNREFERENCED_PARAMETER(Reason);
    if (UcxControllerContext->DriverVerifierEnabled) {
        DbgBreakPoint();
    } else {
        NT_ASSERTMSG("Inspect Local Variable for Reason of this break", FALSE);
    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_StopIdleSynchronous(
    __in
        UCXUSBDEVICE                  Hub,
    __in
        PCONTROLLER_STOP_IDLE_CONTEXT StopIdleContext
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_ResumeIdle(
    __in
        UCXUSBDEVICE                  Hub,
    __in
        PCONTROLLER_STOP_IDLE_CONTEXT StopIdleContext
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_GetInfo(
    __in
        UCXUSBDEVICE                Hub,
    __out
        PUCX_CONTROLLER_INFO        ControllerInfo
    );

