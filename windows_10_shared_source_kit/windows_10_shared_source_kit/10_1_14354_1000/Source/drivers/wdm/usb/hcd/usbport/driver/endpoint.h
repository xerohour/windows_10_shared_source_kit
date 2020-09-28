//
//    Copyright (C) Microsoft.  All rights reserved.
//
NTSTATUS
USBPORT_OpenEndpointRefList(
    PDEVICE_OBJECT HcFdo,
    PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_CloseEndpointRefList(
    PDEVICE_OBJECT HcFdo,
    PHCD_ENDPOINT Endpoint
    );

NTSTATUS
USBPORT_iReferenceEndpointObject(
    PDEVICE_OBJECT HcFdo,
    PHCD_ENDPOINT Endpoint,
    PUSBOBJ_REF EpRefHandle,
    PVOID Object,
    ULONG Tag
    );

NTSTATUS
USBPORT_ReferenceEndpoint(
    PDEVICE_OBJECT HcFdo,
    PHCD_ENDPOINT Endpoint,
    PUSBOBJ_REF *EpRefHandle,
    PVOID Object,
    ULONG Tag
    );

NTSTATUS
USBPORT_iReferenceEndpoint(
    PDEVICE_OBJECT HcFdo,
    PHCD_ENDPOINT Endpoint,
    PUSBOBJ_REF *EpRefHandle,
    PVOID Object,
    ULONG Tag
    );

NTSTATUS
USBPORT_ReferenceAllEndpoints(
    PDEVICE_OBJECT  FdoDeviceObject,
    ULONG           Tag,
    PLIST_ENTRY     ListHead
    );

VOID
USBPORT_iDereferenceEndpoint(
    PDEVICE_OBJECT HcFdo,
    PUSBOBJ_REF EpRefHandle
    );

VOID
USBPORT_DereferenceEndpoint(
    PDEVICE_OBJECT HcFdo,
    PUSBOBJ_REF EpRefHandle
    );

PHCD_ENDPOINT
USBPORT_EndpointFromHandle(
    PUSBOBJ_REF EpRefHandle
    );

HC_GLOBAL_EP_STATE
USBPORT_SetGlobalEndpointState(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __in HC_GLOBAL_EP_EVENT Event
    );

HC_GLOBAL_EP_STATE
USBPORT_SetGlobalEndpointStateAndWait(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __in HC_GLOBAL_EP_EVENT Event,
    __in MP_ENDPOINT_STATE TransitionState
    );

HC_GLOBAL_EP_STATE
USBPORT_iSetGlobalEndpointState(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __in HC_GLOBAL_EP_EVENT Event
    );

HC_GLOBAL_EP_STATE
USBPORT_iSetGlobalEndpointStateTx(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __in HC_GLOBAL_EP_EVENT Event,
    __inout_opt PHCD_TRANSFER_CONTEXT Transfer,
    __out_opt USB_MINIPORT_STATUS *MpStatus,
    __inout_opt PULONG BytesTransferred
    );

HC_GLOBAL_EP_STATE
USBPORT_SetEp_ACTIVE(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    );

HC_GLOBAL_EP_STATE
USBPORT_SetEp_CLOSED(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    );

HC_GLOBAL_EP_STATE
USBPORT_SetEp_PAUSE(
    __inout PDEVICE_OBJECT HcFdo,
    __in HC_GLOBAL_EP_EVENT Event,
    __inout PHCD_ENDPOINT Endpoint
    );

HC_GLOBAL_EP_STATE
USBPORT_SetEp_REMOVE(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_ProcessNeoStateChangeList(
    PDEVICE_OBJECT HcFdo
    );

VOID
USBPORT_AcquireEndpointLock(
    __in PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PEP_LOCK_HANDLE EpLockHandle,
    __in ULONG Tag
    );

VOID
USBPORT_ReleaseEndpointLock(
    __in PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __in PEP_LOCK_HANDLE EpLockHandle
    );

VOID
USBPORT_WaitEpStateEvent(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __in MP_ENDPOINT_STATE State
    );

VOID
USBPORT_SignalEpStateEvent(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __in MP_ENDPOINT_STATE State
    );

VOID
USBPORT_ResetEpStateEvent(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint,
    __in MP_ENDPOINT_STATE State
    );

VOID
USBPORT_iEndpointRemoveStateList(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    );

NTSTATUS
USBPORT_iEndpointInsertStateList(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    );

MP_ENDPOINT_STATE
USBPORT_GetMpEndpointState(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    );

_Requires_lock_not_held_(((PDEVICE_EXTENSION)HcFdo->DeviceExtension)->Fdo.EndpointListSpin)
_Acquires_lock_(((PDEVICE_EXTENSION)HcFdo->DeviceExtension)->Fdo.EndpointListSpin)
_IRQL_requires_max_(DISPATCH_LEVEL)
_IRQL_raises_(DISPATCH_LEVEL)
VOID
USBPORT_AcquireEpListLock(
    _Inout_ PDEVICE_OBJECT HcFdo,
    _Out_ __deref _IRQL_saves_ PKIRQL Irql
    );

_Requires_lock_held_(((PDEVICE_EXTENSION)HcFdo->DeviceExtension)->Fdo.EndpointListSpin)
_Releases_lock_(((PDEVICE_EXTENSION)HcFdo->DeviceExtension)->Fdo.EndpointListSpin)
_IRQL_requires_(DISPATCH_LEVEL)
VOID
USBPORT_ReleaseEpListLock(
    _Inout_ PDEVICE_OBJECT HcFdo,
    _In_ _IRQL_restores_ KIRQL Irql
    );

MP_ENDPOINT_STATE
USBPORT_CheckMpEnpointState(
    __inout PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_iAssertEndpoint(
    __in PDEVICE_OBJECT HcFdo,
    __inout PHCD_ENDPOINT Endpoint
    );
