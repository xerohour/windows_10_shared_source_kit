/*++
Copyright (c) Microsoft Corporation

Module Name:

    hsmfunc.h

Abstract:

    Support functions for HSM

--*/


#pragma once

//
// Types of live dump reports
//
typedef enum _HUB_WER_REPORT_TYPE{
    HubWerSuccessfulReset = 0,
    HubWerFailedReset,
    HubWerFailedEnumeration
} HUB_WER_REPORT_TYPE, *PHUB_WER_REPORT_TYPE;

//
// Work item context for live dump worker thread
//
typedef struct _WER_WORKITEM_CONTEXT {
    PHUB_FDO_CONTEXT HubFdoContext;
    HUB_WER_REPORT_TYPE WerReportType;
    ULONG SubReason;
} WER_WORKITEM_CONTEXT, *PWER_WORKITEM_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WER_WORKITEM_CONTEXT, HUBMISC_GetWerWorkItemContext);

#if DBG

//
// The variables are used for testing live dump creation
//
extern volatile BOOLEAN g_TriggerLiveDump;
extern volatile HUB_WER_REPORT_TYPE g_TriggerLiveDumpReportType;

//
// Test flag to set in the debugger to cause a 0x9F
//
extern volatile BOOLEAN g_TriggerPowerStateFailureBugcheck;

#endif

_Must_inspect_result_
__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBMISC_InitializeHsm (
    __in
        PHUB_FDO_CONTEXT   HubFdoContext
    );


_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
HUBMISC_ControlTransfer(
    __in
        PHUB_FDO_CONTEXT                    HubFdoContext,
    __in_opt
        UCXUSBDEVICE                        UsbDevice,
    __in
        PVOID                               Context,
    __in
        PCONTROL_REQUEST                    ControlRequest,
    __in
        PFN_WDF_REQUEST_COMPLETION_ROUTINE  CompletionRoutine,
    __in_bcount_opt(TransferBufferLength)
        PVOID                               TransferBuffer,
    __in size_t
                                            TransferBufferLength,
    __in BOOLEAN
                                            ShortXferOk,
    __in
        BOOLEAN                             EnsureForwardProgress        
    );


FORCEINLINE
VOID
HUBMISC_RequestReuse(
    __in
        WDFREQUEST      WdfRequest
    );


VOID
HUBMISC_WaitForSignal (
    __in
        PKEVENT         Event,
    __in 
        PCSTR           ReasonForWaiting,
    __in 
        WDFOBJECT       Handle
    );


VOID
HUBMISC_QueryKseDeviceFlags (
    __in
        PCHAR                   VendorId,
    __in
        PCHAR                   DeviceId,
    __in
        PCHAR                   RevisionId,
    __out
        PKSE_USB_DEVICE_FLAGS   DeviceFlagsVidPidRev,
    __out
        PKSE_USB_DEVICE_FLAGS   DeviceFlagsVidPid,
    __in
        BOOLEAN                 IsRootHub,
    __in
        RECORDER_LOG            IfrLog
    );


BOOLEAN
HUBMISC_IsBitSet(
    __in 
        PVOID   Bitmap,
    __in
        ULONG   BitNumber
    );


__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
HUBMISC_DeleteWdfObjectIfNotNull(
    __in
        WDFOBJECT *             WdfObject
    );


NTSTATUS
HUBMISC_GetDeviceInterfacePath(
    __in
        LPCGUID                     InterfaceGuid,
    __in
        WDFDEVICE                   Device,
    __out
        WDFSTRING                   *DeviceInterfacePath,
    __in
        RECORDER_LOG                IfrLog
    );


VOID
HUBMISC_DbgBreak(
    __in 
        PCSTR                       ReasonForBreak    
    );  


NTSTATUS
HUBMISC_GetActivityIdIrp (
    __in
        PIRP    Irp,
    __out
        LPGUID  Guid
    );


NTSTATUS  
HUBMISC_SetActivityIdIrp (
    __inout
        PIRP    Irp,
    __in_opt
        LPGUID  Guid
    );


NTSTATUS
HUBMISC_VerifyCallerIsAdmin(
    __in
        RECORDER_LOG    IfrLog
    );


BOOLEAN
HUBMISC_IsDeviceSuperSpeedPlusCapableOrHigher (
    __in
        PDEVICE_CONTEXT        DeviceContext
    );


BOOLEAN
HUBMISC_IsDeviceOperatingAtSuperSpeedPlusOrHigher (
    __in
        PDEVICE_CONTEXT        DeviceContext
    );


_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
PROOTHUB_20PORT_INFO
HUBMISC_GetRootHub20PortInfo (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext,
    __in
        USHORT                  Index
    );


_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
PROOTHUB_30PORT_INFO_EX
HUBMISC_GetRootHub30PortInfo (
    __in
        PHUB_FDO_CONTEXT        HubFdoContext,
    __in
        USHORT                  Index
    );


PWCHAR
HUBMISC_StripSymbolicNamePrefix (
    __in
        PUNICODE_STRING     SymbolicLinkNameUnicode,
    __out
        size_t              *SymbolicLinkNameLength
    );


__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
HUBMISC_FreeBufferIfNotNull(
    __in
        PVOID               Buffer,
    __in
        ULONG               Tag
    );


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
HUBMISC_InitializeBitmap (
    __out 
        PRTL_BITMAP         BitMapHeader,
    __in __drv_aliasesMem 
        PULONG              BitMapBuffer,
    __in 
        ULONG               SizeOfBitMap
    );


__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
PUCX_FORWARD_PROGRESS_WORKITEM
HUBMISC_AllocateWorkerItem(
    __in
        PHUB_FDO_CONTEXT                HubFdoContext,
    __in
        ULONG                           Flags
    )
/*++

Routine Description:

    Wrapper function for allocating and initializing a ucx worker item.

Arguments:
    HubFdoContext 
    Flags - The ony valid flag is UCX_FORWARD_PROGRESS_WORKITEM_FLAG_NEED_FLUSH_FUNCTIONALITY
            that tells UCX that for this worker item, hub will be needing the flush 
            functionality i.e. ability to wait for any queued worker item to finish
Returns:

    Worker Item created by UCX, Null if it fails

--*/
{
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;
    PUCX_FORWARD_PROGRESS_WORKITEM  workItem;

    TRY {
        
        hubControllerStackInterface = &HubFdoContext->HubControllerStackInterface;

        NT_ASSERT(hubControllerStackInterface->ForwardProgressWorkItemAllocate != NULL);

        
        workItem = hubControllerStackInterface->ForwardProgressWorkItemAllocate(
            HubFdoContext->HubParentInterface.Hub,
            WdfDeviceWdmGetDeviceObject(HubFdoContext->HubFdoDevice),
            Flags);


    } FINALLY {

    }

    return workItem;
}


__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID      
HUBMISC_DeleteWorkerItem(
    __in
        PHUB_FDO_CONTEXT                HubFdoContext,
    __in
        PUCX_FORWARD_PROGRESS_WORKITEM  WorkItem
    )
/*++

Routine Description:

    Wrapper function for deleting a ucx worker item.
    

Arguments:
    HubFdoContext 
    WorkItem - Pointer returned from an earlier call to HUBMISC_AllocateWorkerItem

Returns:

--*/
{
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;

    TRY {
        
        hubControllerStackInterface = &HubFdoContext->HubControllerStackInterface;

        NT_ASSERT(hubControllerStackInterface->ForwardProgressWorkItemDelete != NULL);

        
        hubControllerStackInterface->ForwardProgressWorkItemDelete(WorkItem);


    } FINALLY {

    }

}


__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
HUBMISC_EnqueueWorkerItem(
    __in
        PHUB_FDO_CONTEXT                        HubFdoContext,
    __in 
        PUCX_FORWARD_PROGRESS_WORKITEM          WorkItem,
    __in
        PUCX_FORWARD_PROGRESS_WORKITEM_CALLBACK Callback,
    __in_opt
        PVOID                                   CallbackContext,
    __in
        BOOLEAN                                 ForwardProgressRequired
    )
/*++

Routine Description:

    Wrapper function for Enqueuing a ucx worker item.

Arguments:
    HubFdoContext 
    WorkItem - Pointer returned from an earlier call to HUBMISC_AllocateWorkerItem
    Callback - Function that needs to be run at passive level 
    CallbackContext - Context that will be passed back in the Callback
    ForwardProgressNotRequired - Informs UCX whether this worker item
                                 really needs forward progres. Depending on
                                 this value, UCX will decide whether it needs
                                 to schedule a system thread to avoid paging
    
Returns:

    

--*/
{
    PHUB_CONTROLLERSTACK_INTERFACE          hubControllerStackInterface;
    UCX_FORWARD_PROGRESS_ENQUEUE_OPTIONS    forwardProgressEnqueueOptions;                                  


    TRY {
        
        hubControllerStackInterface = &HubFdoContext->HubControllerStackInterface;

        NT_ASSERT(hubControllerStackInterface->ForwardProgressWorkItemEnqueue != NULL);
        
        if (ForwardProgressRequired) {
            forwardProgressEnqueueOptions = UCX_FORWARD_PROGRESS_DEFAULT_OPTIONS;
        } else {
            forwardProgressEnqueueOptions = UCX_FORWARD_PROGRESS_NOT_REQUIRED;
        }
        hubControllerStackInterface->ForwardProgressWorkItemEnqueue(WorkItem,
                                                                    Callback,
                                                                    CallbackContext,
                                                                    forwardProgressEnqueueOptions);
    } FINALLY {

    }

}


__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
HUBMISC_AddEnabledEndpointsToPendingDisableList(
    __in
        PDEVICE_CONTEXT         DeviceContext,
    __in
        PINTERFACE_HANDLE       InterfaceHandle
    )
/*++

Routine Description:

Returns:


--*/
{
    ULONG               i;
    PPIPE_HANDLE        pipeHandle;

    TRY {

        //
        // Walk the endpoint handles for this interface and add the enabed endpoints to the pending disable list
        // Also add already disabled endpoints to the remain unchanged list
        //
        for (i=0; i<InterfaceHandle->NumberOfEndpoints; i++) {
                    
            pipeHandle = &InterfaceHandle->PipeHandle[i];
                    
            switch (pipeHandle->UcxEndpointState) {
            case UcxEndpointStateEnabled: 
                pipeHandle->UcxEndpointState = UcxEndpointStatePendingDisable;
                DeviceContext->EndpointsToBeDisabled[DeviceContext->EndpointsToBeDisabledCount++] = 
                    pipeHandle->UcxEndpoint;
                break;

            case UcxEndpointStateDisabled:
                DeviceContext->EndpointsToRemainUnchanged[DeviceContext->EndpointsToRemainUnchangedCount++] = 
                    pipeHandle->UcxEndpoint;
                break;

            case UcxEndpointStateNotCreated:
                break;
            
            default:
                //
                // Only valid endpoint states are UcxEndpointStateEnabled, UcxEndpointStateDisabled
                // and UcxEndpointStateNotCreated
                //
                NT_ASSERTMSG("Unexpected endpoint state", FALSE);
            }
        }
    } FINALLY {
    }
}



__drv_maxIRQL(PASSIVE_LEVEL)
FORCEINLINE
VOID
HUBMISC_FlushWorkerItem(
    __in
        PHUB_FDO_CONTEXT                        HubFdoContext,
    __in 
        PUCX_FORWARD_PROGRESS_WORKITEM          WorkItem
    )
/*++

Routine Description:

    Wrapper function for flushing any enqueued UCX worker items
    
Returns:


--*/
{
    PHUB_CONTROLLERSTACK_INTERFACE  hubControllerStackInterface;

    TRY {
        
        hubControllerStackInterface = &HubFdoContext->HubControllerStackInterface;

        NT_ASSERT(hubControllerStackInterface->ForwardProgressWorkItemFlush != NULL);
        
        hubControllerStackInterface->ForwardProgressWorkItemFlush(WorkItem);

    } FINALLY {

    }

}     

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
HUBMISC_DeleteAndReleaseDsmReferenceOnWdfObjectIfNotNull(
    __in
        WDFOBJECT *             WdfObject
    );


NTSTATUS
HUBMISC_CreateWerReport(
    __in
        PHUB_FDO_CONTEXT    HubFdoContext,
    __in
        HUB_WER_REPORT_TYPE WerReportType,
    __in
        ULONG               SubReason
    );


VOID
HUBMISC_SetSqmEnumerationData (
    __in
        PDEVICE_CONTEXT        DeviceContext
    );


VOID
HUBMISC_ReportPnPFailureProblem(
    __in
        WDFDEVICE   Device,
    __in
        ULONG       MsgId
    );

VOID
HUBMISC_ConvertUsbDeviceIdsToString(
    __in
        PUSB_DEVICE_DESCRIPTOR          DeviceDescriptor,
    __out_ecount(MAX_VENDOR_ID_STRING_LENGTH)
        PCHAR                           VendorId,
    __out_ecount(MAX_DEVICE_ID_STRING_LENGTH)
        PCHAR                           DeviceId,
    __out_ecount(MAX_REVISION_ID_STRING_LENGTH)
        PCHAR                           RevisionId
    );

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
HUBMISC_QueryKseGlobalFlags (
    __in 
        PHUB_GLOBALS     HubGlobals
    );

_Must_inspect_result_
NTSTATUS
HUBMISC_CheckForDuplicateEndpointAddresses(
    __in
        PDEVICE_CONTEXT DeviceContext
    );

_Must_inspect_result_
NTSTATUS
HUBMISC_CheckForSharedEndpoints (
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in
        PINTERFACE_HANDLE   InterfaceHandle1,
    __in
        PINTERFACE_HANDLE   InterfaceHandle2
    );

_Must_inspect_result_
BOOLEAN
HUBMISC_IsEndpointDescriptorOfEndpoint0PresentInCurrentConfiguration(
    __in
        PDEVICE_CONTEXT DeviceContext
    );

_Must_inspect_result_
BOOLEAN
HUBMISC_IsEndpointDescriptorOfEndpoint0PresentInInterface(
    __in
        PDEVICE_CONTEXT     DeviceContext,
    __in
        PINTERFACE_HANDLE   InterfaceHandle
    );

#if DBG

VOID
HUBMISC_InducePowerStateFailureBugcheck();

VOID
HUBMISC_InduceLiveDumpCreation(
    __in
        PHUB_FDO_CONTEXT HubFdoContext,
        ULONG            SubReason
    );

#endif

EVT_WDF_WORKITEM HUBMISC_WerReportWorkItem;

