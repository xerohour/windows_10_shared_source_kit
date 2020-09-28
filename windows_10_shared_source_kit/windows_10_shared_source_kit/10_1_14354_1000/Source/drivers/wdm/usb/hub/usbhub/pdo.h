//
//    Copyright (C) Microsoft.  All rights reserved.
//
/* generated prototypes from file: pdo.c */


NTSTATUS
UsbhIncPdoIoCount(
    PDEVICE_OBJECT Pdo,
    PVOID Object,
    ULONG TagSig,
    ULONG UserTag
    );


VOID
UsbhDecPdoIoCount(
    PDEVICE_OBJECT Pdo,
    PVOID Object
    );


NTSTATUS
UsbhPdoSymbolicLink(
    BOOLEAN CreateFlag,
    PDEVICE_OBJECT Pdo,
    LPGUID lpGuid
    );


NTSTATUS
UsbhPdoPnp_QueryInterface(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );

NTSTATUS
UsbhPdoPnp_QueryResourceRequirements(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPnp_DoDefault(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoSuccess(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoInternalDeviceControl(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhFdoRecordFailure(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoDeviceControl(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoSystemControl(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPnp_StartDevice(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


VOID
UsbhPdoRemoveCleanup(
    PDEVICE_OBJECT Pdo
    );


NTSTATUS
UsbhPdoPnp_StopDevice(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPnp_QueryCapabilities(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPnp_QueryDeviceRelations(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPnp_EnablePdo(
    PDEVICE_OBJECT Pdo
    );


NTSTATUS
UsbhPdoPnp_QueryId(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPnp_QueryDeviceText(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPnp_DeviceUsageNotification(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPnp_QueryBusInfo(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPnp_Success(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPnp_SurpriseRemoveDevice(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


NTSTATUS
UsbhPdoPnp_RemoveDevice(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );

VOID
UsbhReleasePdoStateLock(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    );


VOID
UsbhDeletePdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    HUB_PDO_HWPNP_STATE PdoState,
    HUB_PDO_HWPNP_EVENT PnpEvent,
    PSTATE_CONTEXT StateContext
    );


NTSTATUS
UsbhCreatePdo(
    PDEVICE_OBJECT HubFdo,
    PHUB_PORT_DATA PortData
    );


NTSTATUS
UsbhSyncSendCommandToDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    PUCHAR Buffer,
    PUSHORT BufferLength,
    ULONG MillisecondTimeout,
    USBD_STATUS *ReturnUsbdStatus
    );


NTSTATUS
UsbhGetStringFromDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USBD_STATUS *UsbdStatus,
    PUCHAR Buffer,
    PUSHORT BufferLengthBytes,
    USHORT LanguageId,
    UCHAR StringIndex
    );


NTSTATUS
UsbhGetQualifierDescriptorFromDevice(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USBD_STATUS *UsbdStatus,
    PUCHAR Buffer,
    PUSHORT BufferLengthBytes
    );


NTSTATUS
UsbhEnableDeviceForWake(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USBD_STATUS *UsbdStatus
    );


NTSTATUS
UsbhDisableDeviceForWake(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USBD_STATUS *UsbdStatus
    );


NTSTATUS
UsbhCloseDeviceConfiguration(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    USBD_STATUS *UsbdStatus
    );

NTSTATUS
UsbhPdoCreateSymbolicLink(
    PDEVICE_OBJECT Pdo,
    LPGUID lpGuid
    );

VOID
UsbhPdoDeleteSymbolicLink(
    PDEVICE_OBJECT Pdo
    );

VOID
UsbhDerefPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PVOID Object,
    ULONG Tag,
    PHGSL Lock
    );

#if DBG
#endif

VOID
UsbhResetPortData(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHGSL Lock
    );


NTSTATUS
UsbhPdoPnp_QueryPnpDeviceState(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );


PDEVICE_OBJECT
UsbhRefPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PVOID Object,
    ULONG Tag,
    PHGSL Lock
    );


VOID
UsbhSetPortData(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PHGSL Lock
    );


PDEVICE_OBJECT
UsbhLatchPdo(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    PVOID Object,
    ULONG Tag
    );


VOID
UsbhUnlatchPdo(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PVOID Object,
    ULONG Tag
    );


PDEVICE_OBJECT
UsbhGetHubFdo(
    PDEVICE_OBJECT Pdo,
    PHGSL Lock
    );


VOID
UsbhRemoveAndDeletePdo(
    PDEVICE_OBJECT Pdo
    );

USBD_CHILD_STATUS
UsbhInsertDeviceIntoGlobalList(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    );

PUSB_DEVICE_HANDLE
UsbhRefPdoDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PVOID Object,
    ULONG Tag
    );

VOID
UsbhDerefPdoDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE DevHandle,
    PVOID Object,
    ULONG Tag
    );

NTSTATUS
UsbhLinkPdoDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PUSB_DEVICE_HANDLE DevHandle
    );

VOID
UsbhUnlinkPdoDeviceHandle(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    ULONG LogTag,
    BOOLEAN RemoveHandle
    );

VOID
UsbhDeleteOrphanPdo(
    PDEVICE_OBJECT Pdo
    );

NTSTATUS
UsbhGetBosDescriptor(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PULONG BufferLength
    );

NTSTATUS
UsbhGetDeviceDescriptor(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    );

NTSTATUS
UsbhGetConfigDescriptor(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    );

BOOLEAN
UsbhPdo_IsDeviceExternal(
    PDEVICE_OBJECT Pdo
);

NTSTATUS
UsbhPdoPnp_DeviceEnumerated(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    );

VOID
UsbhPdo_ReportPnPFailureProblem(
    PDEVICE_OBJECT Pdo
    );

VOID
UsbhPdo_ResetDriverProblemDesc(
    PDEVICE_OBJECT Pdo
    );

SET_D3COLD_SUPPORT                              UsbhD3ColdSupportInterfaceSetD3ColdSupport;
GET_IDLE_WAKE_INFO                              UsbhD3ColdSupportInterfaceGetIdleWakeInfo;
GET_D3COLD_CAPABILITY                           UsbhD3ColdSupportInterfaceGetD3ColdCapability;
GET_D3COLD_CAPABILITY                           UsbhD3ColdSupportInterfaceGetD3ColdBusDriverSupport;
GET_D3COLD_LAST_TRANSITION_STATUS               UsbhD3ColdSupportInterfaceGetLastTransitionStatus;

VOID
UsbhD3ColdSupportInterfaceReference (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context
    );


VOID
UsbhD3ColdSupportInterfaceDereference (
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context
    );

VOID
UsbhPublishBillboardDetails (
    _In_ PDEVICE_OBJECT Pdo
    );

VOID
UsbhBillboardCleanup (
    _In_ PDEVICE_OBJECT Pdo
    );

VOID
UsbhPublishDualRoleFeaturesProperty(
    _In_ PDEVICE_OBJECT Pdo
    );