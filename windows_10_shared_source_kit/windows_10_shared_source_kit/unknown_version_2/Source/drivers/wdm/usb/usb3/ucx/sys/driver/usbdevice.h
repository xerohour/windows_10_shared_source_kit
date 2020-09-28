//
//    Copyright (C) Microsoft.  All rights reserved.
//
typedef enum _USBDEVICE_TYPE {
    UsbDeviceTypeHub = 0x1,
    UsbDeviceTypeRootHub = 0x2
} USBDEVICE_TYPE;

FORCEINLINE
BOOLEAN
IsTypeHub(
    USBDEVICE_TYPE UsbDeviceType
    )
/*++

Routine Description:

    Checks if the USBDEVICE_TYPE is a Hub type    

Arguments:

    UsbDeviceType - The type of UCXUSBDEVICE    

Return Value:

    BOOLEAN returns TRUE if the UsbDeviceType is hub type
--*/
{
    return ((UsbDeviceType & (UsbDeviceTypeHub | UsbDeviceTypeRootHub)) != 0);
}

FORCEINLINE
BOOLEAN
IsTypeRootHub(
    USBDEVICE_TYPE UsbDeviceType
    )
/*++

Routine Description:

    Checks if the USBDEVICE_TYPE is a RootHub type    

Arguments:

    UsbDeviceType - The type of UCXUSBDEVICE    

Return Value:

    BOOLEAN returns TRUE if the UsbDeviceType is roothub type
--*/
{
    return ((UsbDeviceType & UsbDeviceTypeRootHub) != 0);
}

#define MAX_COMPOSITE_DRIVER_FUNCTION_COUNT 255

typedef struct _UCXUSBDEVICE_PRIVCONTEXT *PUCXUSBDEVICE_PRIVCONTEXT;
typedef struct _UCXPIPE_INFO *PUCXPIPE_INFO;

typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(*PFN_UCXUSBDEVICE_PENDING_OPERATION_COMPLETION)(
    __in PUCXUSBDEVICE_PRIVCONTEXT
   );


typedef struct _REMOTE_WAKE_REQUEST_INFO {
    WDFREQUEST                           PendingRequest;
    PREQUEST_REMOTE_WAKE_NOTIFICATION    RequestParams;
} REMOTE_WAKE_REQUEST_INFO, *PREMOTE_WAKE_REQUEST_INFO;

typedef struct _UCXFUNCTION_INFO {
    //
    // Keep the First Field as Index, since UsbdFunctionHandleGetIndex routine depends on it. 
    //
    ULONG                                Index;
    UCX_SIG                              Sig;
    PUCXUSBDEVICE_PRIVCONTEXT            UcxUsbDeviceContext;
    PDEVICE_OBJECT                       ClientPdo;
    REMOTE_WAKE_REQUEST_INFO             FunctionRemoteWakeRequestInfo;
} UCXFUNCTION_INFO, *PUCXFUNCTION_INFO;

typedef struct _UCXENDPOINT_PRIVCONTEXT *PUCXENDPOINT_PRIVCONTEXT;
typedef struct _UCXUSBDEVICE_PRIVCONTEXT {
    
    UCX_SIG                                               Sig;
    USBDEVICE_TYPE                                        UsbDeviceType;
    ULONG                                                 UsbVersionNumber;
    UCXUSBDEVICE                                          UcxUsbDevice;
    PUCXCONTROLLER_PRIVCONTEXT                            UcxControllerContext;
    UCXUSBDEVICE                                          ParentHub;
    USB_DEVICE_CONTEXT                                    ContextFromHub;
    //
    // Note: ChildListHead, ChildListEntry, EndpointListHead are protected 
    // by a global Topology Lock. This is done to simplify handling of 
    // UsbDeviceDisconnect (which needs to walk a list of all devices and endpoints)
    // This does not impact hot path.
    // 

    //
    // List Head to connect all child devices of this UcxUsbDevice
    //
    LIST_ENTRY                                            ChildListHead;
    //
    // List Entry that links with it's parent's ChildListHead
    //
    LIST_ENTRY                                            ChildListEntry;
    //
    // List Entry to connect all endpoints of a device. 
    //
    LIST_ENTRY                                            EndpointListHead;
    LIST_ENTRY                                            StaleEndpointListHead;
    
    //
    // List Entry to be used only if strict validation of pipe handles is required for this device.
    // !! IMPORTANT !!
    // If you need to hold both TopologyLock and InvalidHandleTrakingLock, acquire TopologyLock
    // first.
    //
    LIST_ENTRY                                            EndpointListHeadForInvalidHandleTracking;

    _Has_lock_level_(EndpointTrackingLockLevel)
    KSPIN_LOCK                                            InvalidHandleTrakingLock;

    UCX_USBDEVICE_EVENT_CALLBACKS_RESERVED                EvtCallbacks;
    PUCXENDPOINT_PRIVCONTEXT                              DefaultEndpointContext;
    PUCXPIPE_INFO                                         DefaultPipeInfo;
    
    UCXUSBDEVICE_INFO                                     UcxUsbDeviceInfo;
    PDEVICE_OBJECT                                        Pdo;

    //
    // Information on function handles
    //
    ULONG                                                 NumberOfFunctions;
    PUCXFUNCTION_INFO                                     UcxFunctionInfo;
    WDFMEMORY                                             UcxFunctionInfoWdfMemory;
    KSPIN_LOCK                                            RemoteWakeLock;
    REMOTE_WAKE_REQUEST_INFO                              UsbDeviceRemoteWakeRequestInfo;

    //
    // Information temporarily to synchronize with individual endpoint 
    // state machines.
    //
    PFN_UCXUSBDEVICE_PENDING_OPERATION_COMPLETION         PendingOperationCompletion;
    LONG                                                  PendingOperations;

    LONG                                                  PendingNoPingResponseError;

    //
    // Endpoint state machines for the device collectively have at most one pending hub IRP at a
    // time (either in IRP or WDFREQUEST form).
    //
    union {
        PIRP                                              PendingUsbDeviceEnableIrp;
        PIRP                                              PendingUsbDeviceResetIrp;
        PIRP                                              PendingUsbDeviceStartIrp;
        PIRP                                              PendingUsbDevicePurgeIrp;
        PIRP                                              PendingEndpointsConfigureIrp;
        WDFREQUEST                                        PendingEndpointsConfigureRequest;
        WDFREQUEST                                        PendingUsbDeviceResetRequest;
        WDFREQUEST                                        PendingUsbDeviceDisableRequest;
        WDFREQUEST                                        PendingUsbDeviceStartRequest;
        WDFREQUEST                                        PendingUsbDevicePurgeRequest;
        WDFREQUEST                                        PendingUsbDeviceAbortRequest;
    };

    ULONG                                                 FailCloseStaticStreams:1;
    ULONG                                                 UsbDeviceResetFailedDueToControllerReset:1;
    ULONG                                                 EndpointResetFailedDueToControllerReset:1;
    //
    // Information related to current state of the device
    //
    BOOLEAN                                               Disconnected;
    BOOLEAN                                               Enabled;
    BOOLEAN                                               DeprogrammedDueToControllerReset;
    BOOLEAN                                               AllowChildrenToExitTreePurge;
    BOOLEAN                                               AllowEndpointsToExitTreePurge;
    
    //
    // Do strict validation for this device, e.g. Lock and Walk list of endpoints to validatate 
    // the pipe handle. Invalid pipe I/O will be failed, but it is "allowed" in the sense that it
    // won't bugcheck.
    //
    BOOLEAN                                               AllowIoOnInvalidPipeHandles;

    //
    // A boolean to track the pending delete state
    //
    BOOLEAN                                               PendingDelete;
    //
    // Count of tx failures for this device. Used for debugging only.
    //
    ULONG                                                 TransferFailureCount;
    
    //
    // Query Interface Information From Hub
    //
    USB_HUB_CONTEXT                                       HubQIContext;
    PFN_CLEAR_TT_BUFFER                                   HubQICallbackClearTTBuffer;
    PFN_NO_PING_RESPONSE                                  HubQICallbackNoPingResponse;

    //
    // Information about how many USBD registerations are active for this device
    //
    KSPIN_LOCK                                            USBDInterfaceListLock; 
    LIST_ENTRY                                            USBDInterfaceListHead;

} UCXUSBDEVICE_PRIVCONTEXT, *PUCXUSBDEVICE_PRIVCONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(UCXUSBDEVICE_PRIVCONTEXT, 
                                   UcxUsbDeviceGetContext);

EVT_WDF_OBJECT_CONTEXT_DESTROY UsbDevice_EvtDestroyCallback;

typedef struct _UCXUSBDEVICE_INIT {
    UCX_SIG                           Sig;
    UCXUSBDEVICE                      ParentHub;
    UCXUSBDEVICE                      UcxUsbDevice;
    UCXUSBDEVICE_INFO                 UcxUsbDeviceInfo;    
    USB_DEVICE_CONTEXT                ContextFromHub;
    UCX_USBDEVICE_EVENT_CALLBACKS_RESERVED     
                                      EvtCallbacks;
} UCXUSBDEVICE_INIT, *PUCXUSBDEVICE_INIT;


_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
UsbDevice_Create(
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        PUSBDEVICE_INFO    UsbDeviceCreateInfo,
    __out 
        UCXUSBDEVICE      *UsbDevice
);

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
UsbDevice_Delete(
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        UCXUSBDEVICE       UsbDevice
);

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_Disconnect(
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        ULONG              PortNumber
);

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_SetPdoInformation(
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        UCXUSBDEVICE       UsbDevice,
    __in
        PDEVICE_OBJECT     Pdo
);

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
UsbDevice_IsDeviceDisconnected(
    __in 
        UCXUSBDEVICE       Device
);

typedef
__drv_mustHold(SpinLock)
VOID
(*PFN_USBDEVICE_WALKTREE_CALLBACK) (
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT   UcxUsbDeviceContext,
    __in_opt
        PVOID                       Context
    );


__drv_mustHold(SpinLock)
VOID
UsbDevice_WalkTree(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT               UcxUsbDeviceContext,
    __in
        BOOLEAN                                 WalkDisconnectedTree,
    __in 
        PFN_USBDEVICE_WALKTREE_CALLBACK         Callback,
    __in_opt 
        PVOID                                   Context
    );

__drv_mustHold(SpinLock)
__drv_at(UcxUsbDeviceContext->Disconnected, __range(==,FALSE))
VOID
UsbDevice_DisconnectCallback(
    __in
      PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in
      PLIST_ENTRY                   EndpointList
    );

typedef struct _USBDEVICE_TREE_PURGE_CALLBACK_CONTEXT {
    LIST_ENTRY                      EndpointList;
    LONG                            EndpointCount;
} USBDEVICE_TREE_PURGE_CALLBACK_CONTEXT, *PUSBDEVICE_TREE_PURGE_CALLBACK_CONTEXT;

FORCEINLINE
VOID
USBDEVICE_TREE_PURGE_CALLBACK_CONTEXT_INIT(
    __out
        PUSBDEVICE_TREE_PURGE_CALLBACK_CONTEXT Context
    )
{
    RtlZeroMemory(Context, sizeof(*Context));
    InitializeListHead(&Context->EndpointList);
}

__drv_mustHold(SpinLock)
VOID
UsbDevice_TreePurgeCallback(
    __in
      PUCXUSBDEVICE_PRIVCONTEXT                     UcxUsbDeviceContext,
    __inout
      PUSBDEVICE_TREE_PURGE_CALLBACK_CONTEXT        TreePurgeCallbackContext
    );

typedef struct _USBDEVICE_CONTROLLER_RESET_CALLBACK_CONTEXT {
    LIST_ENTRY                      EndpointList;
    ULONG                           EndpointCount;
} USBDEVICE_CONTROLLER_RESET_CALLBACK_CONTEXT, *PUSBDEVICE_CONTROLLER_RESET_CALLBACK_CONTEXT;

FORCEINLINE
VOID
USBDEVICE_CONTROLLER_RESET_CALLBACK_CONTEXT_INIT(
    __out
        PUSBDEVICE_CONTROLLER_RESET_CALLBACK_CONTEXT Context
    )
{
    RtlZeroMemory(Context, sizeof(*Context));
    InitializeListHead(&Context->EndpointList);
}


__drv_mustHold(SpinLock)
VOID
UsbDevice_PrepareForControllerResetCallback(
    __in
      PUCXUSBDEVICE_PRIVCONTEXT                     UcxUsbDeviceContext,
    __in
      PUSBDEVICE_CONTROLLER_RESET_CALLBACK_CONTEXT  ControllerResetCallbackContext
    );

__drv_mustHold(SpinLock)
VOID
UsbDevice_NotifyControllerResetCompleteCallback(
    __in
      PUCXUSBDEVICE_PRIVCONTEXT                     UcxUsbDeviceContext,
    __in
      PUSBDEVICE_CONTROLLER_RESET_CALLBACK_CONTEXT  ControllerResetCallbackContext
    );

#if 0
__drv_mustHold(SpinLock)
VOID
UsbDevice_SetDeprogrammed(
    __in
      PUCXUSBDEVICE_PRIVCONTEXT                     UcxUsbDeviceContext,
    __in_opt
      PVOID                                         UnusedContext
    );
#endif

VOID
UsbDevice_EndpointsConfigureCompleteFromHcd(
    __in
        PIRP                    Irp, 
    __in
        PENDPOINTS_CONFIGURE    EndpointsConfigure
    );

VOID
UsbDevice_DefaultEndpointEnableCompleteFromHcd(
    __in
        PIRP                    Irp, 
    __in
        PUSBDEVICE_ENABLE       UsbDeviceEnable
    );

VOID
UsbDevice_ResetCompleteFromHcd(
    __in
        PIRP                    Irp, 
    __in 
        PUSBDEVICE_RESET        UsbDeviceReset
    );

VOID
UsbDevice_EndpointsConfigureFromHub(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT         UcxUsbDeviceContext,
    __in
        WDFREQUEST                        Request,
    __in
        BOOLEAN                           AfterControllerResetInitiated
    );

VOID
UsbDevice_DisableFromHub(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in
        WDFREQUEST                    Request,
    __in
        BOOLEAN                       AfterControllerResetInitiated
    );

VOID
UsbDevice_PurgeFromHub(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in
        WDFREQUEST                    Request,
    __in
        BOOLEAN                       OnSuspend
    );

VOID
UsbDevice_StartFromHub(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in
        WDFREQUEST                    Request
    );

VOID
UsbDevice_ResetFromHub(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in
        WDFREQUEST                    Request,
    __in
        BOOLEAN                       AfterControllerResetInitiated
    );

VOID
UsbDevice_TreePurgeFromHub(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in
        WDFREQUEST                    Request
   );

VOID
UsbDevice_PendingOperationCompleteByEndpoint( 
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT UcxUsbDeviceContext
    );

VOID
UsbDevice_PendingTreePurgeCompleteByEndpoint(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    );

VOID
UsbDevice_RegisterCompositeDevice( 
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT       UcxUsbDeviceContext,
    __in 
        PREGISTER_COMPOSITE_DEVICE      RegisterCompositeDevice,
    __out_ecount(RegisterCompositeDevice->FunctionCount)
        PUSBD_FUNCTION_HANDLE           UsbdFunctionHandles,
    __in
        WDFREQUEST                      Request
    );

VOID
UsbDevice_UnregisterCompositeDevice(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT       UcxUsbDeviceContext,
    __in
        WDFREQUEST                      Request
    );

VOID
UsbDevice_SetFunctionData(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT       UcxUsbDeviceContext,
    __in 
        PUSB_FUNCTION_DATA              UsbFunctionData,
    __in
        WDFREQUEST                      Request
    );

EVT_WDF_REQUEST_CANCEL UsbDevice_EvtRemoteWakeRequestCancel;

VOID
UsbDevice_RemoteWakeNotificationRequest(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT                 UcxUsbDeviceContext,
    __in 
        PREQUEST_REMOTE_WAKE_NOTIFICATION         RemoteWakeNotification,
    __in
        WDFREQUEST                                Request
    );

EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL UsbDevice_EvtMgmtIoInternalDeviceControl;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL UsbDevice_EvtTreePurgeIoInternalDeviceControl;
EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL UsbDevice_EvtFailMgmtIoInternalDeviceControl;

VOID
UsbDevice_AddEventForEndpoints(
    __in_ecount(EndpointCount)
        UCXENDPOINT*                                     EndpointsArray,
    __in
        ULONG                                            EndpointCount,
    __in 
        SM_ENGINE_EVENT                                  Event
    );

VOID
UsbDevice_EnableInvalidHandleTracking(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT         UcxUsbDeviceContext
    );

NTSTATUS
UsbDevice_EnableForwardProgress(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT                 UcxUsbDevice,
    __in
        PUSB_FORWARD_PROGRESS_INFO                ForwardProgressInfo
    );

FORCEINLINE
VOID
UsbDevice_NoPingResponse(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT                 UcxUsbDeviceContext
    )
{
    PUCXUSBDEVICE_PRIVCONTEXT  hubContext;
    TRY {
        hubContext = UcxUsbDeviceGetContext(UcxUsbDeviceContext->ParentHub);
        hubContext->HubQICallbackNoPingResponse(hubContext->HubQIContext,
                                                UcxUsbDeviceContext->ContextFromHub);
    } FINALLY {
    }
}
