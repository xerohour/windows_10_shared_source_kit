//
//    Copyright (C) Microsoft.  All rights reserved.
//
#ifndef __UCX_H__
#define __UCX_H__

//
// This is a header file for ucx.c file
//

#include "UcxClass.h"
#include "triage-ucx01000.h"

#define EXPORT(a) _##a

#define UCX_TAG  'DCHU'

DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_UNLOAD Ucx_EvtDriverUnload;

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Ucx_ClassInitialize(VOID);

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Ucx_ClassDeinitialize(VOID);

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Ucx_ClassBindClient(
    __in
        PWDF_CLASS_BIND_INFO    WdfClassBindInfo,
    __in
        PWDF_COMPONENT_GLOBALS  WdfComponentGlobals
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Ucx_ClassUnbindClient(
    __in
        PWDF_CLASS_BIND_INFO    WdfClassBindInfo,
    __in
        PWDF_COMPONENT_GLOBALS  WdfComponentGlobals
    );
    
typedef struct _WDFDRIVER_UCX_CONTEXT {
    UCX_SIG                 Sig;
    WDFDEVICE               UcxWdfDevice;
    LIST_ENTRY              UcxControllerListHead;
    KSPIN_LOCK              UcxControllerListLock;
    ULONG                   UcxControllerListCount;
    BOOLEAN                 SleepStudyEnabled;
} WDFDRIVER_UCX_CONTEXT, *PWDFDRIVER_UCX_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WDFDRIVER_UCX_CONTEXT,
                                   WdfDriverGetUcxContext);

extern PWDFDRIVER_UCX_CONTEXT g_WdfDriverUcxContext ;

typedef struct _UCXCONTROLLER_PRIVCONTEXT * PUCXCONTROLLER_PRIVCONTEXT;
typedef struct _WDFDEVICE_UCX_CONTEXT {
    UCX_SIG                 Sig;
    PUCXCONTROLLER_PRIVCONTEXT
                            UcxControllerContext;
} WDFDEVICE_UCX_CONTEXT, *PWDFDEVICE_UCX_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(WDFDEVICE_UCX_CONTEXT, 
                                   WdfDeviceGetUcxContext);


_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
EXPORT(UcxControllerCreate)(
    __in_opt 
        PUCX_GLOBALS                  Globals,
    __in 
        WDFDEVICE                     Device,
    __in 
        PUCX_CONTROLLER_CONFIG        Config,
    __in_opt 
        PWDF_OBJECT_ATTRIBUTES        Attributes,
    __out 
        UCXCONTROLLER                *Controller
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EXPORT(UcxControllerNeedsReset)(
    __in_opt 
        PUCX_GLOBALS                  Globals,
    __in 
        UCXCONTROLLER                 Controller
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EXPORT(UcxControllerResetComplete)(
    __in_opt 
        PUCX_GLOBALS                        Globals,
    __in 
        UCXCONTROLLER                       Controller,
    __in
        PUCX_CONTROLLER_RESET_COMPLETE_INFO UcxControllerResetCompleteInfo
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EXPORT(UcxControllerSetFailed)(
    __in_opt 
        PUCX_GLOBALS                        Globals,
    __in 
        UCXCONTROLLER                       Controller
    );

__drv_maxIRQL(DISPATCH_LEVEL)
_Must_inspect_result_
BOOLEAN
EXPORT(UcxIoDeviceControl)(
    __in_opt 
        PUCX_GLOBALS                    Globals,
    __in
        WDFDEVICE                       Device,
    __in 
        WDFREQUEST                      Request,
    __in 
        size_t                          OutputBufferLength,
    __in  
        size_t                          InputBufferLength,
    __in
        ULONG                           IoControlCode
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
EXPORT(UcxRootHubCreate)(
    __in_opt 
        PUCX_GLOBALS               Globals,
    __in 
        UCXCONTROLLER              UcxController,
    __in 
        PUCX_ROOTHUB_CONFIG        Config,
    __in_opt 
        PWDF_OBJECT_ATTRIBUTES     Attributes,
    __out
        UCXROOTHUB                *RootHub
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EXPORT(UcxRootHubPortChanged)(
    __in_opt 
        PUCX_GLOBALS               Globals,
    __in 
        UCXROOTHUB                 UcxRootHub
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
EXPORT(UcxUsbDeviceCreate)(
    __in_opt 
        PUCX_GLOBALS               Globals,
    __in 
        UCXCONTROLLER              UcxController,
    __deref_inout_opt 
        PUCXUSBDEVICE_INIT        *UsbDeviceInit,    
    __in_opt 
        PWDF_OBJECT_ATTRIBUTES     Attributes,
    __out 
        UCXUSBDEVICE              *UsbDevice
    );

VOID
EXPORT(UcxUsbDeviceInitSetEventCallbacks) (
    __in_opt 
        PUCX_GLOBALS                            Globals,
    __inout
        PUCXUSBDEVICE_INIT                      UsbDeviceInit,
    __in
        PUCX_USBDEVICE_EVENT_CALLBACKS          EventCallbacks
);

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EXPORT(UcxUsbDeviceRemoteWakeNotification)(
    __in_opt 
        PUCX_GLOBALS               Globals,
    __in 
        UCXUSBDEVICE               UsbDevice,
    __in
        ULONG                      Interface
    );

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
EXPORT(UcxEndpointCreate) (
    __in_opt 
        PUCX_GLOBALS                  Globals,
    __in 
        UCXUSBDEVICE                  UcxUsbDevice,
    __deref_inout_opt 
        PUCXENDPOINT_INIT            *EndpointInit,    
    __in_opt
        PWDF_OBJECT_ATTRIBUTES        Attributes,
    __out
        UCXENDPOINT                  *Endpoint
    );

__drv_maxIRQL(DISPATCH_LEVEL)
UCXSSTREAMS
EXPORT(UcxEndpointGetStaticStreamsReferenced) (
    __in_opt 
        PUCX_GLOBALS                  Globals,
    __in 
        UCXENDPOINT                   UcxEndpoint,
    __in 
        PVOID                         Tag
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EXPORT(UcxEndpointPurgeComplete) (
    __in_opt 
        PUCX_GLOBALS                  Globals,
    __in 
        UCXENDPOINT                   Endpoint
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EXPORT(UcxEndpointStartComplete) (
    __in_opt 
        PUCX_GLOBALS                  Globals,
    __in 
        UCXENDPOINT                   Endpoint
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EXPORT(UcxEndpointAbortComplete) (
    __in_opt 
        PUCX_GLOBALS                  Globals,
    __in 
        UCXENDPOINT                   Endpoint
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EXPORT(UcxEndpointNoPingResponseError) (
    __in_opt 
        PUCX_GLOBALS                  Globals,
    __in 
        UCXENDPOINT                   Endpoint
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EXPORT(UcxEndpointNeedToCancelTransfers) (
    __in_opt 
        PUCX_GLOBALS                  Globals,
    __in 
        UCXENDPOINT                   Endpoint
    );

VOID
EXPORT(UcxEndpointInitSetEventCallbacks) (
    __in_opt 
        PUCX_GLOBALS                  Globals,
    __inout 
        PUCXENDPOINT_INIT             EndpointInit,
    __in 
        PUCX_ENDPOINT_EVENT_CALLBACKS EventCallbacks
);

VOID
EXPORT(UcxDefaultEndpointInitSetEventCallbacks) (
    __in_opt 
        PUCX_GLOBALS                            Globals,
    __inout 
        PUCXENDPOINT_INIT                       EndpointInit,
    __in 
        PUCX_DEFAULT_ENDPOINT_EVENT_CALLBACKS   EventCallbacks
);

VOID
EXPORT(UcxEndpointSetWdfIoQueue) (
    __in_opt 
        PUCX_GLOBALS        Globals,
    __in 
        UCXENDPOINT         Endpoint,
    __in 
        WDFQUEUE            WdfQueue
);

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
EXPORT(UcxStaticStreamsCreate) (
    __in_opt 
        PUCX_GLOBALS               Globals,
    __in 
        UCXENDPOINT                Endpoint,
    __deref_inout_opt 
        PUCXSSTREAMS_INIT         *SStreamsInit,
    __in_opt
        PWDF_OBJECT_ATTRIBUTES     Attributes,
    __out
        UCXSSTREAMS               *SStreams
    );

VOID
EXPORT(UcxStaticStreamsSetStreamInfo) (
    __in_opt 
        PUCX_GLOBALS            Globals,
    __in 
        UCXSSTREAMS             SStreams,
    __in    
        PSTREAM_INFO            StreamInfo
);

NTSTATUS
Urb_USBPORTStyle_ProcessURB(
    __in 
        WDFDEVICE               RHPdo,
    __in
        PIRP                    Irp,
    __in 
        PURB                    Urb
    );

#define VERIFY_EXPORT_FUNC(__array__, __exportfunc__) (ASSERT((__array__)[__exportfunc__##Index] == (PFN_UCX_CLASS) EXPORT(__exportfunc__)))

_Must_inspect_result_
FORCEINLINE
NTSTATUS
Ucx_WdfObjectCreateWith2Attributes( 
    __in 
        PWDF_OBJECT_ATTRIBUTES ObjectAttributes1,
    __in_opt 
        PWDF_OBJECT_ATTRIBUTES ObjectAttributes2,
    __out 
        WDFOBJECT*             Object
)
{
    NTSTATUS  status;
    WDFOBJECT localObject;

    TRY {
        
        localObject = NULL;

        status = WdfObjectCreate(ObjectAttributes1, &localObject);
        
        if (! NT_SUCCESS(status)) {
            LEAVE;
        }

        if (WDF_NO_OBJECT_ATTRIBUTES != ObjectAttributes2) {

            status = WdfObjectAllocateContext(localObject,
                                              ObjectAttributes2,
                                              NULL);
            if (! NT_SUCCESS(status)) {
                LEAVE;
            }
        }

    } FINALLY {

        if (! NT_SUCCESS(status)) {

            if (localObject != NULL) {
                WdfObjectDelete(localObject);
            }

            *Object = NULL;
        
        } else {
            *Object = localObject;
        }

    }

    return status;

}

FORCEINLINE
PVOID
Ucx_WdfRequestGetArg1(
    __in 
        WDFREQUEST              Request
    )
/*++

Routine Description:

    Returns The PIO_STACK_LOCATION->Parmeters.Others.Arg1
    
Arguments:

    Request - Wdf Request for which Arg1 is requested

Return Value:

    Arg1

--*/
{
    
    WDF_REQUEST_PARAMETERS  requestParams;
    
    WDF_REQUEST_PARAMETERS_INIT(&requestParams);
    
    WdfRequestGetParameters(Request, &requestParams);

    return requestParams.Parameters.Others.Arg1;
}

FORCEINLINE
VOID
Ucx_WdfRequestGetArgs(
    __in
        WDFREQUEST              Request,
    __out_opt
        PVOID                  *Arg1,
    __out_opt
        PVOID                  *Arg2,
    __out_opt
        PVOID                  *Arg4
    )
/*++

Routine Description:

    Returns The PIO_STACK_LOCATION->Parmeters.Others.Arg1, Arg2, Arg4

Arguments:

    Request - Wdf Request for which Args are requested
    
    Arg1 - output - pointer to location where the Value of Arg1 will be stored. It is optional.

    Arg2 - output - pointer to location where the Value of Arg2 will be stored. It is optional.
    
    Arg4 - output - pointer to location where the Value of Arg4 will be stored. It is optional.

--*/
{
    TRY {
        WDF_REQUEST_PARAMETERS  requestParams;

        WDF_REQUEST_PARAMETERS_INIT(&requestParams);

        WdfRequestGetParameters(Request, &requestParams);

        if (Arg1 != NULL) { 
            *Arg1 = requestParams.Parameters.Others.Arg1;
        }

        if (Arg2 != NULL) { 
            *Arg2 = requestParams.Parameters.Others.Arg2;
        }

        if (Arg4 != NULL) { 
            *Arg4 = requestParams.Parameters.Others.Arg4;
        }
    } FINALLY {

    }

    return;
}

FORCEINLINE
VOID
UcxPointerNotNull(
    PVOID Ptr
    ) 
{
    if (NULL == Ptr) {
        //
        // Induce a Verifier Bugcheck here
        //
    }
}   

//
// NOTE: Since the type of field Flags can be different, thus 
// not making the following macro's as inline functions
//
#define SET_FLAG(Flags, Bit)    ((Flags) |= (Bit))
#define CLEAR_FLAG(Flags, Bit)  ((Flags) &= ~(Bit))
#define TEST_FLAG(Flags, Bit)   ((Flags) & (Bit))

typedef
NTSTATUS  
(*PFN_IO_GET_ACTIVITY_ID_IRP) (
    __in
        PIRP    Irp,
    __out
        LPGUID  Guid
    );  

PFN_IO_GET_ACTIVITY_ID_IRP      g_IoGetActivityIdIrp;

typedef
NTSTATUS  
(*PFN_IO_SET_ACTIVITY_ID_IRP) (
    __inout
        PIRP    Irp,
    __in_opt
        LPGUID  Guid
    );  

PFN_IO_SET_ACTIVITY_ID_IRP      g_IoSetActivityIdIrp;

typedef
VOID
(*PFN_IO_FREE_ACTIVITY_ID_IRP) (
    __inout
        PIRP    Irp
    );  

PFN_IO_FREE_ACTIVITY_ID_IRP      g_IoFreeActivityIdIrp;

FORCEINLINE
NTSTATUS
Ucx_GetActivityIdIrp (
    __in
        PIRP    Irp,
    __out
        LPGUID  Guid
    )
/*++

Routine Description:

    Wrapper for IoGetActivityIdIrp

Returns:

    NTSTATUS

--*/
{
    NTSTATUS    status;

    TRY {

        if (g_IoGetActivityIdIrp == NULL) {
            status = STATUS_NOT_FOUND;
            LEAVE;
        }

        status = g_IoGetActivityIdIrp(Irp, Guid);

    } FINALLY {
    
    }

    return status;
}

FORCEINLINE
NTSTATUS  
Ucx_SetActivityIdIrp (
    __inout
        PIRP    Irp,
    __in_opt
        LPGUID  Guid
    )
/*++

Routine Description:

    Wrapper for IoSetActivityIdIrp

Returns:

    NTSTATUS

--*/
{
    NTSTATUS    status;

    TRY {

        if (g_IoSetActivityIdIrp == NULL) {
            status = STATUS_NOT_SUPPORTED;
            LEAVE;
        }

        status = g_IoSetActivityIdIrp(Irp, Guid);

    } FINALLY {
    
    }

    return status;
}

FORCEINLINE
VOID
Ucx_FreeActivityIdIrp (
    __inout
        PIRP    Irp
    )
/*++

Routine Description:

    Wrapper for IoFreetActivityIdIrp

Returns:

    VOID

--*/
{

    TRY {

        if (g_IoSetActivityIdIrp == NULL) {
            LEAVE;
        }

        g_IoFreeActivityIdIrp(Irp);

    } FINALLY {
    
    }
}

typedef
BOOLEAN
(*PFN_IO_TRY_QUEUE_WORKITEM)(
    __inout 
        PIO_WORKITEM              IoWorkItem,
    __in 
        PIO_WORKITEM_ROUTINE_EX   WorkerRoutine,
    __in 
        WORK_QUEUE_TYPE           QueueType,
    __in_opt __drv_aliasesMem 
        PVOID                     Context
    );

extern PFN_IO_TRY_QUEUE_WORKITEM       g_IoTryQueueWorkItem;

FORCEINLINE
__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Ucx_IoTryQueueWorkItem(
    __inout 
        PIO_WORKITEM              IoWorkItem,
    __in 
        PIO_WORKITEM_ROUTINE_EX   WorkerRoutine,
    __in 
        WORK_QUEUE_TYPE           QueueType,
    __in_opt __drv_aliasesMem 
        PVOID                     Context
    )
{
    BOOLEAN queued;

    if (g_IoTryQueueWorkItem == NULL) {
        queued = FALSE;
    } else {
        queued = g_IoTryQueueWorkItem(IoWorkItem, WorkerRoutine, QueueType, Context);        
    }

    return queued;
}


BOOLEAN
Ucx_FailDueToVerifierSetting(
    ULONG   VerifierSetting
    );

ULONG
Ucx_NonZeroRandomWithMax(
    ULONG   MaxRandom
    );


NTSTATUS
Ucx_GetRandomErrorNTSTATUS();

VOID
Ucx_DbgBreak(
    __in
        PCSTR                       ReasonForBreak
    );

#endif // __UCX_H__
