//
//    Copyright (C) Microsoft.  All rights reserved.
//
#pragma once

//
// Limited to a UInt16 field
//
#define MAXIMUM_PARTIAL_DATA_TRANSFER_LENGTH    256
#define MAXIMUM_FULL_DATA_TRANSFER_LENGTH       4096

VOID
Etw_UcxControllerRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    );

VOID
Etw_UcxDeviceTreeRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    );


typedef struct _ETW_UCXENDPOINT_TABLE_ENTRY {

    UCXUSBDEVICE                    UcxUsbDevice;
    PUCXENDPOINT_PRIVCONTEXT        UcxEndpointContext;
    PUCXSSTREAMS_PRIVCONTEXT        UcxSStreamsContext;

} ETW_UCXENDPOINT_TABLE_ENTRY, *PETW_UCXENDPOINT_TABLE_ENTRY;

typedef struct _ETW_UCXDEVICE_RUNDOWN_CALLBACK_CONTEXT {

    //
    // Table of all UcxDevice children of a controller.
    //
    PUCXUSBDEVICE_PRIVCONTEXT      *UcxDeviceTable;
    ULONG                           UcxDeviceTableCount;
    ULONG                           UcxDeviceTableIndex;

    //
    // Table of all UcxEndpoints of all UcxDevice children of a
    // controller.
    //
    PETW_UCXENDPOINT_TABLE_ENTRY    UcxEndpointTable;
    ULONG                           UcxEndpointTableCount;
    ULONG                           UcxEndpointTableIndex;

} ETW_UCXDEVICE_RUNDOWN_CALLBACK_CONTEXT, *PETW_UCXDEVICE_RUNDOWN_CALLBACK_CONTEXT;

__drv_mustHold(SpinLock)
VOID
Etw_UcxDeviceTreeRundownWalkCallback(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT   UcxUsbDeviceContext,
    __in
        PETW_UCXDEVICE_RUNDOWN_CALLBACK_CONTEXT UcxDeviceRundownCallbackContext
    );

VOID
Etw_UcxDeviceRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        PUCXUSBDEVICE_PRIVCONTEXT   UcxUsbDeviceContext
    );

VOID
Etw_UcxEndpointRundown(
    __in
        LPCGUID                     ActivityId,
    __in
        UCXUSBDEVICE                UcxUsbDevice,
    __in
        PUCXENDPOINT_PRIVCONTEXT    UcxEndpointContext,
    __in_opt
        PUCXSSTREAMS_PRIVCONTEXT    UcxSStreamsContext
    );

VOID
Etw_UcxControllerCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    );

VOID
Etw_UcxControllerDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXCONTROLLER_PRIVCONTEXT  UcxControllerContext
    );

VOID
Etw_UcxControllerStateMachineTransition(
    __in_opt
        LPCGUID               ActivityId,
    __in 
        UCXCONTROLLER         Controller,
    __in
        PSM_ENGINE_CONTEXT    SmEngineContext,
    __in 
        SM_ENGINE_EVENT       Event,
    __in
        SM_ENGINE_STATE       State
    );

VOID
Etw_UcxControllerStateMachineEvent(
    __in_opt
        LPCGUID               ActivityId,
    __in 
        UCXCONTROLLER         Controller,
    __in
        PSM_ENGINE_CONTEXT    SmEngineContext,
    __in 
        SM_ENGINE_EVENT       Event
    );

VOID
Etw_UcxRootHubCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXROOTHUB_PRIVCONTEXT     UcxRootHubContext
    );

VOID
Etw_UcxRootHubDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXROOTHUB_PRIVCONTEXT     UcxRootHubContext
    );

VOID
Etw_UcxRootHubPrepare(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXROOTHUB_PRIVCONTEXT     UcxRootHubContext
    );

VOID
Etw_UcxRootHubRelease(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXROOTHUB_PRIVCONTEXT     UcxRootHubContext
    );

VOID
Etw_UcxDeviceCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXUSBDEVICE_PRIVCONTEXT   UcxUsbDeviceContext
    );

VOID
Etw_UcxDeviceDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXUSBDEVICE_PRIVCONTEXT   UcxUsbDeviceContext
    );

VOID
Etw_UcxEndpointCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXENDPOINT_PRIVCONTEXT    UcxEndpointContext
    );

VOID
Etw_UcxEndpointDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXENDPOINT_PRIVCONTEXT    UcxEndpointContext
    );

VOID
Etw_UcxEndpointStateMachineTransition(
    __in_opt
        LPCGUID               ActivityId,
    __in 
        UCXENDPOINT           Endpoint,
    __in
        PSM_ENGINE_CONTEXT    SmEngineContext,
    __in 
        SM_ENGINE_EVENT       Event,
    __in
        SM_ENGINE_STATE       State
    );

VOID
Etw_UcxEndpointStateMachineEvent(
    __in_opt
        LPCGUID               ActivityId,
    __in 
        UCXENDPOINT           Endpoint,
    __in
        PSM_ENGINE_CONTEXT    SmEngineContext,
    __in 
        SM_ENGINE_EVENT       Event
    );

VOID
Etw_UcxStaticStreamsCreate(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXSSTREAMS_PRIVCONTEXT    UcxSStreamsContext
    );

VOID
Etw_UcxStaticStreamsDelete(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXSSTREAMS_PRIVCONTEXT    UcxSStreamsContext
    );

VOID
Etw_UcxRootHubInitiatingWake(
    __in_opt
        LPCGUID                     ActivityId,
    __in
        PUCXROOTHUB_PRIVCONTEXT     UcxRootHubContext
    );

VOID
Etw_XrbComplete(
    __in
        PIRP            Irp,
    __in
        PTRANSFER_URB   TransferUrb
    );

VOID
Etw_UrbComplete(
    __in
        PIRP            Irp,
    __in
        PTRANSFER_URB   TransferUrb
    );

VOID
Etw_ControlTransferUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    );

VOID
Etw_ControlTransferExUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    );

VOID
Etw_BulkTransferXrbDispatch(
    __in
        PIRP            Irp,
    __in
        PURB            Urb
    );

VOID
Etw_BulkTransferUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    );

VOID
Etw_IsochTransferUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    );

VOID
Etw_GetSetDescriptorUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    );

VOID
Etw_SetClearFeatureUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    );

VOID
Etw_GetStatusUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    );

VOID
Etw_ClassVendorUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    );

VOID
Etw_GetMsFeatureUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    );

VOID
Etw_GetInterfaceUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    );

VOID
Etw_GetConfigurationUrbDispatch(
    __in
        PIRP                Irp,
    __in
        PURB                Urb
    );

