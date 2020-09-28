/*++

Copyright (c) 2004  Microsoft Corporation

Module Name:

    prototyp.h

Abstract:

    put all the prototypes in one header

Environment:

    Kernel & user mode

Revision History:

    02-25-00 : created
    01-12-04 : fix bugs

--*/

#ifndef   __PROTOTYP_H__
#define   __PROTOTYP_H__

#define FRAME_COUNT_WAIT 5

PUSBPORT_MINIPORT_DRIVER
USBPORT_FindMiniport(
    PDRIVER_OBJECT DriverObject
    );

VOID
USBPORT_RestoreConfigFlag(
    PDEVICE_OBJECT HcFdo
    );

VOID
USBPORT_CheckAndArmHcForWake(
    PDEVICE_OBJECT FdoDeviceObject
    );

USB_BUSIFFN_WAIT_ASYNC_POWERUP USBPORTBUSIF_WaitAsyncPowerUp;

VOID
USBPORT_SetHcFdoPowerState(
    PDEVICE_OBJECT HcFdo,
    PVOID ContextData,
    HC_FDO_POWERSTATE NewFdoPowerState,
    HC_FDO_EVENT FdoPowerEvent
    );

DRIVER_INITIALIZE DriverEntry;

DRIVER_UNLOAD USBPORT_Unload;

NTSTATUS
USBPORT_GetPhysicalDeviceObjectName(
    PDEVICE_OBJECT  PhysicalDeviceObject,
    PUNICODE_STRING Name
    );

DRIVER_ADD_DEVICE USBPORT_PnPAddDevice;

NTSTATUS
USBPORT_InitStateVars(
    PDEVICE_OBJECT FdoDeviceObject
    );

__drv_dispatchType(IRP_MJ_PNP)
DRIVER_DISPATCH USBPORT_DispatchPnP;

__drv_dispatchType(IRP_MJ_CREATE)
__drv_dispatchType(IRP_MJ_CLOSE)
__drv_dispatchType(IRP_MJ_DEVICE_CONTROL)
__drv_dispatchType(IRP_MJ_SYSTEM_CONTROL)
__drv_dispatchType(IRP_MJ_INTERNAL_DEVICE_CONTROL)
__drv_dispatchType(IRP_MJ_POWER)
DRIVER_DISPATCH USBPORT_Dispatch;

RTL_QUERY_REGISTRY_ROUTINE USBPORT_GetConfigValue;

NTSTATUS
USBPORT_PdoPnP(
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    );

NTSTATUS
USBPORT_FdoPnP(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    );

NTSTATUS
USBPORT_DeferredStartDevice(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    );

NTSTATUS
USBPORT_StopDevice(
     PDEVICE_OBJECT FdoDeviceObject
     );

NTSTATUS
USBPORT_StartDevice(
     PDEVICE_OBJECT FdoDeviceObject,
     PHC_RESOURCES HcResources
     );

BOOLEAN
USBPORT_iValidDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle
    );

BOOLEAN
USBPORT_ValidatePipeHandle(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle
    );

NTSTATUS
USBPORT_SetUSBDError(
    PURB Urb,
    USBD_STATUS UsbdStatus
    );

NTSTATUS
USBPORT_PassIrp(
    __inout PDEVICE_OBJECT DeviceObject,
    __in_opt PIO_COMPLETION_ROUTINE CompletionRoutine,
    __in_opt PVOID Context,
    __in BOOLEAN InvokeOnSuccess,
    __in BOOLEAN InvokeOnError,
    __in BOOLEAN InvokeOnCancel,
    __in PIRP Irp,
    __inout_opt PUSB_IOREQUEST_CONTEXT UsbIoRequest
    );

NTSTATUS
USBPORT_SetRegistryKeyValueForPdo(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in BOOLEAN SoftwareBranch,
    __in ULONG Type,
    __in_bcount(KeyNameStringLength) PCWSTR KeyNameString,
    __in ULONG KeyNameStringLength,
    __inout_bcount(DataLength) PVOID Data,
    __in ULONG DataLength
    );

NTSTATUS
USBPORT_GetRegistryKeyValueForPdo(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in BOOLEAN SoftwareBranch,
    __in_bcount(KeyNameStringLength) PCWSTR KeyNameString,
    __in ULONG KeyNameStringLength,
    __inout_bcount(DataLength) PVOID Data,
    __in ULONG DataLength
    );

NTSTATUS
USBPORT_CreateRootHubPdo(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT *RootHubPdo
    );

NTSTATUS
USBPORT_CreateRootHubDeviceHandle(
    PDEVICE_OBJECT PdoDeviceObject
    );

NTSTATUS
USBPORT_BusQueryBusTime(
    PVOID BusContext,
    PULONG CurrentFrame
    );

NTSTATUS
USBPORT_BusSubmitIsoOutUrb(
    PVOID BusContext,
    PURB Urb
    );

VOID
USBPORT_BusGetUSBDIVersion(
    PVOID BusContext,
    PUSBD_VERSION_INFORMATION VersionInformation,
    PULONG HcdCapabilities
    );

NTSTATUS
USBPORT_MakeRootHubPdoName(
    PDEVICE_OBJECT FdoDeviceObject,
    PUNICODE_STRING PdoNameUnicodeString,
    ULONG Index
    );

PWCHAR
USBPORT_GetIdString(
    PDEVICE_OBJECT FdoDeviceObject,
    USHORT Vid,
    USHORT Pid,
    USHORT Rev
    );

USBD_STATUS
USBPORT_AllocTransfer(
    PDEVICE_OBJECT FdoDeviceObject,
    PTRANSFER_URB Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PIRP Irp,
    PKEVENT CompleteEvent,
    ULONG MillisecTimeout
    );

VOID
USBPORT_FreeUsbAddress(
    PDEVICE_OBJECT FdoDeviceObject,
    USHORT DeviceAddress
    );

NTSTATUS
USBPORT_AllocateUsbAddress(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSHORT AssignedAddress
    );

NTSTATUS
USBPORT_SendCommand(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    PVOID Buffer,
    ULONG BufferLength,
    PULONG BytesReturned,
    USBD_STATUS *UsbdStatus
    );

USBD_STATUS
USBPORT_MiniportStatus_TO_USBDStatus(
    USB_MINIPORT_STATUS mpStatus
    );
#define MPSTATUS_TO_USBSTATUS(s) USBPORT_MiniportStatus_TO_USBDStatus((s))

NTSTATUS
USBPORT_MiniportStatus_TO_NtStatus(
    USB_MINIPORT_STATUS mpStatus
    );
#define MPSTATUS_TO_NTSTATUS(s) USBPORT_MiniportStatus_TO_NtStatus((s))

RHSTATUS
USBPORT_MiniportStatus_TO_RHStatus(
    USB_MINIPORT_STATUS mpStatus
    );
#define MPSTATUS_TO_RHSTATUS(s) USBPORT_MiniportStatus_TO_RHStatus((s))

USBD_STATUS
USBPORT_RHStatus_TO_USBDStatus(
    USB_MINIPORT_STATUS rhStatus
    );

USBD_STATUS
USBPORT_NtStatus_TO_UsbdStatus(
    LONG NtStatus
    );


#define RHSTATUS_TO_USBDSTATUS(s) USBPORT_RHStatus_TO_USBDStatus((s))


NTSTATUS
USBPORT_FdoDeviceControlIrp(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    );

NTSTATUS
USBPORT_PdoDeviceControlIrp(
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    );

NTSTATUS
USBPORT_FdoInternalDeviceControlIrp(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    );

NTSTATUS
USBPORT_PdoInternalDeviceControlIrp(
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    );

NTSTATUS
USBPORT_ProcessURB(
    PDEVICE_OBJECT PdoDeviceObject,
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PURB Urb,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    );

VOID
USBPORT_CancelTransfer(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    );

KSERVICE_ROUTINE USBPORT_InterruptService;

VOID
USBPORT_HalFreeCommonBuffer(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBPORT_COMMON_BUFFER CommonBuffer
    );

VOID
USBPORT_HalFreeCommonBufferToFreeList(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBPORT_COMMON_BUFFER CommonBuffer
    );

PUSBPORT_COMMON_BUFFER
USBPORT_HalAllocateCommonBuffer(
    PDEVICE_OBJECT FdoDeviceObject,
    ULONG NumberOfBytes
    );

USB_MINIPORT_STATUS
USBPORTSVC_GetMiniportRegistryKeyValue(
    __inout PDEVICE_DATA DeviceData,
    BOOLEAN SoftwareBranch,
    __in_bcount(KeyNameStringLength) PCWSTR KeyNameString,
    ULONG KeyNameStringLength,
    __inout PVOID Data,
    ULONG DataLength
    );

USB_MINIPORT_STATUS
USBPORTSVC_ReadWriteConfigSpace(
    PDEVICE_DATA DeviceData,
    BOOLEAN Read,
    PVOID Buffer,
    ULONG Offset,
    ULONG Length
    );

NTSTATUS
USBPORT_InternalOpenInterface(
    PURB Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_CONFIG_HANDLE ConfigHandle,
    PUSBD_INTERFACE_INFORMATION InterfaceInformation,
    OUT PUSBD_INTERFACE_HANDLE_I *InterfaceHandle,
    BOOLEAN SendSetInterfaceCommand
    );

PUSBD_CONFIG_HANDLE
USBPORT_InitializeConfigurationHandle(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor
    );

NTSTATUS
USBPORT_SelectInterface(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandleNotUsed,
    PHCD_ENDPOINT       EndpointNotUsed,
    LPCGUID             ActivityId
    );

NTSTATUS
USBPORT_SelectConfiguration(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandleNotUsed,
    PHCD_ENDPOINT       EndpointNotUsed,
    LPCGUID             ActivityId
    );

VOID
USBPORT_InternalCloseConfiguration(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    ULONG Flags
    );

PUSB_INTERFACE_DESCRIPTOR
USBPORT_InternalParseConfigurationDescriptor(
    PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor,
    UCHAR InterfaceNumber,
    UCHAR AlternateSetting,
    PBOOLEAN HasAlternateSettings
    );

NTSTATUS
USBPORT_OpenEndpoint(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_PIPE_HANDLE_I PipeHandle,
    USBD_STATUS *UsbdStatus,
    BOOLEAN IsDefaultPipe
    );

VOID
USBPORT_CloseEndpoint(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT DeviceObject,
    PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_SimulateInterruptDpc(
    PKDPC Dpc,
    PVOID DeferredContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2
    );

VOID
USBPORT_SimulateInterrupt(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_RootHub_EndpointWorker(
    PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_RootHub_RemoveDevice(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject
    );

RHSTATUS
USBPORT_RootHub_Endpoint0(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_TRANSFER_CONTEXT Transfer
    );

RHSTATUS
USBPORT_RootHub_Endpoint1(
    __in PDEVICE_OBJECT HcFdo,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __in PVOID IntPipeDataBuffer,
    __in ULONG IntPipeDataBufferLength
    );

VOID
USBPORT_PollEndpoint(
    PHCD_ENDPOINT Endpoint
    );

RHSTATUS
USBPORT_RootHub_ClassCommand(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    PUCHAR Buffer,
    PULONG BufferLength
    );

RHSTATUS
USBPORT_RootHub_StandardCommand(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    PUCHAR Buffer,
    PULONG BufferLength
    );

NTSTATUS
USBPORT_QueryCapabilities(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_CAPABILITIES DeviceCapabilities
    );

VOID
USBPORT_AddPipeHandle(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandle
    );

VOID
USBPORT_FlushCancelList(
    PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_SetEndpointState(
    PHCD_ENDPOINT Endpoint,
    MP_ENDPOINT_STATE State,
    PEP_LOCK_HANDLE EpLock
    );


KDEFERRED_ROUTINE USBPORT_IsrDpc;

VOID
USBPORT_SetDeviceHandleState(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    USBD_DEVH_STATE NewState,
    USBD_DEVH_EVENT Event
    );

VOID
USBPORTSVC_CompleteIsoTransfer(
    PDEVICE_DATA DeviceData,
    PDEVICE_DATA EndpointData,
    PTRANSFER_PARAMETERS TransferParameters,
    PMINIPORT_ISO_TRANSFER IsoTransfer
    );

VOID
USBPORTSVC_RequestAsyncCallback(
    PDEVICE_DATA DeviceData,
    ULONG MilliSeconds,
    PVOID Context,
    ULONG ContextLength,
    PMINIPORT_CALLBACK CallbackFunction
    );

BOOLEAN
USBPORT_RootHubDisabled(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_FlushDoneTransferList(
    PDEVICE_OBJECT FdoDeviceObject
    );

BOOLEAN
USBPORT_CoreEndpointWorker(
    PHCD_ENDPOINT Endpoint,
    ULONG Flags
    );

NTSTATUS
USBPORT_TtFromBusContext(
    PDEVICE_OBJECT HcFdo,
    PBUS_CONTEXT BusContext,
    PTRANSACTION_TRANSLATOR *ReturnTt
    );

VOID
USBPORT_BusContextReleaseTt(
    PDEVICE_OBJECT HcFdo,
    PBUS_CONTEXT BusContext,
    PTRANSACTION_TRANSLATOR Tt
    );

NTSTATUS
USBPORT_GetBusInterface(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    );

NTSTATUS
USBPORT_GetBusInterfaceHub(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    );

NTSTATUS
USBPORT_GetBusInterfaceHubSS(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    );

NTSTATUS
USBPORT_BusifGetLocationInterface(
    PDEVICE_OBJECT  HubFdo,
    PDEVICE_OBJECT  Pdo,
    PIRP            Irp
    );

NTSTATUS
USBPORTBUSIF_SuspendRootHub(
    PDEVICE_OBJECT Pdo
    );

NTSTATUS
USBPORTBUSIF_ResumeRootHub(
    PDEVICE_OBJECT Pdo
    );

NTSTATUS
USBPORT_GetBusMiniDumpInterface(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    );

NTSTATUS
USBPORT_GetBusForwardProgressInterface(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    );

VOID
USBPORTBUSIF_SetUsbPortMiniDumpFlags(
    PVOID BusContext
    );

USB_BUSIFFN_SET_DEVICE_FLAG USBPORTBUSIF_SetDeviceFlag;

VOID
USBPORT_FlushAllEndpoints(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_UserGetDriverVersion(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_DRIVER_VERSION_PARAMETERS Parameters
    );

USB_BUSIFFN_CREATE_USB_DEVICE USBPORTBUSIF_CreateUsbDevice;

NTSTATUS
USBPORT_CompletePdoD0(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    );

USB_BUSIFFN_SET_BUS_WAKE_MODE USBPORTBUSIF_SetBusSystemWakeMode;

USB_BUSIFFN_CALC_PIPE_BANDWIDTH USBPORTBUSIF_CaculatePipeBandwidth;

USB_BUSIFFN_CREATE_USB_DEVICE_EX USBPORTBUSIF_CreateUsbDeviceEx;

USB_BUSIFFN_CREATE_USB_DEVICE_V7 USBPORTBUSIF_CreateUsbDeviceV7;

USB_BUSIFFN_REMOVE_USB_DEVICE USBPORTBUSIF_RemoveUsbDevice;

NTSTATUS
USBPORT_InitializeDevice(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_ID_ERROR_INFORMATION IdErrInfo
    );

USB_BUSIFFN_IS_ROOT USBPORTBUSIF_HubIsRoot;

USB_BUSIFFN_ACQUIRE_SEMAPHORE USBPORTBUSIF_AcquireBusLock;

USB_BUSIFFN_RELEASE_SEMAPHORE USBPORTBUSIF_ReleaseBusLock;

USB_BUSIFFN_GET_CONTAINER_ID_FOR_PORT USBPORTBUSIF_GetContainerIdForPort;

USB_BUSIFFN_SET_CONTAINER_ID_FOR_PORT USBPORTBUSIF_SetContainerIdForPort;

USB_BUSIFFN_ABORT_ALL_DEVICE_PIPES USBPORTBUSIF_AbortAllDevicePipes;

USB_BUSIFFN_SET_DEVICE_ERRATA_FLAG USBPORTBUSIF_SetDeviceErrataFlag;

USB_BUSIFFN_GET_DEBUG_PORT_NUMBER USBPORTBUSIF_GetDebugPortNumber;

USB_BUSIFFN_SET_MSOS20_SUPPORT USBPORTBUSIF_SetMsOs20Support;

USB_BUSIFFN_SET_CLEAR_ROOT_PORT_RESUME_TIME USBPORTBUSIF_SetClearRootPortResumeTime;


NTSTATUS
USBPORT_CreateDevice(
    PUSBD_DEVICE_HANDLE *DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE HubDeviceHandle,
    USHORT PortStatus,
    USHORT PortNumber,
    PUSB_CD_ERROR_INFORMATION CdErrInfo
    );

IO_COMPLETION_ROUTINE USBPORT_DeferIrpCompletion;

VOID
USBPORTSVC_InvalidateRootHub(
    PDEVICE_DATA DeviceData
    );

RHSTATUS
USBPORT_RootHub_PortRequest(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_DEFAULT_PIPE_SETUP_PACKET SetupPacket,
    PORT_OPERATION PortOperation
    );

VOID
USBPORT_ComputeRootHubDeviceCaps(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject
    );

VOID
USBPORT_FlushMapTransferList(
    PDEVICE_OBJECT FdoDeviceObject
    );

IO_COMPLETION_ROUTINE USBPORT_FdoSystemPowerIrpCompletion;

IO_COMPLETION_ROUTINE USBPORT_FdoDevicePowerIrpCompletion;

NTSTATUS
USBPORT_FdoPowerIrp(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    );

NTSTATUS
USBPORT_PdoPowerIrp(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    );

VOID
USBPORT_DmaEndpointWorker(
    PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_TransferFlushDpc(
    PKDPC Dpc,
    PVOID DeferredContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2
    );

VOID
USBPORT_InvalidateController(
    PDEVICE_OBJECT FdoDeviceObject,
    USB_CONTROLLER_STATE ControllerState
    );

VOID
USBPORT_FlushPendingList(
    PHCD_ENDPOINT Endpoint,
    LONG Count
    );

ULONG
USBPORT_CalculateUsbBandwidthEp(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_ENDPOINT Endpoint
    );

ULONG
USBPORT_CalculateUsbBandwidth(
    PDEVICE_OBJECT FdoDeviceObject,
    ULONG MaxPacketSize,
    DEVICE_SPEED DeviceSpeed,
    ENDPOINT_TRANSFER_TYPE TransferType,
    ENDPOINT_TRANSFER_DIRECTION Direction
    );

KDEFERRED_ROUTINE USBPORT_SurpriseRemoveDpc;

KDEFERRED_ROUTINE USBPORT_HcResetDpc;

VOID
USBPORTSVC_CompleteTransfer(
    PDEVICE_DATA DeviceData,
    PDEVICE_DATA EndpointData,
    PTRANSFER_PARAMETERS TransferParameters,
    USBD_STATUS UsbdStatus,
    ULONG BytesTransferred
    );

VOID
USBPORTSVC_InvalidateEndpoint(
    PDEVICE_DATA DeviceData,
    PENDPOINT_DATA EndpointData
    );

VOID
USBPORTSVC_InvalidateEndpoint2(
    PDEVICE_DATA DeviceData,
    PENDPOINT_DATA EndpointData,
    BOOLEAN Irq
    );

USB_BUSIFFN_INITIALIZE_USB_DEVICE USBPORTBUSIF_InitializeUsbDevice;

USB_BUSIFFN_INITIALIZE_USB_DEVICE_EX USBPORTBUSIF_InitializeUsbDeviceEx;

USB_BUSIFFN_GET_USB_DESCRIPTORS USBPORTBUSIF_GetUsbDescriptors;

NTSTATUS
USBPORT_GetUsbDescriptor(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    UCHAR DescriptorType,
    PUCHAR DescriptorBuffer,
    PULONG DescriptorBufferLength
    );

VOID
USBPORT_Wait(
    PDEVICE_OBJECT FdoDeviceObject,
    ULONG Milliseconds
    );

VOID
USBPORT_InitializeHighResTimer(
    );

BOOLEAN
USBPORT_HighResWait(
    ULONG Milliseconds
    );

PVOID
USBPORT_AllocateHighResTimer(
    _In_ PEXT_CALLBACK Callback,
    _In_opt_ PVOID Context
    );

VOID
USBPORT_SetHighResTimer(
    _In_ PVOID HighResTimer,
    _In_ LONG Milliseconds
    );

VOID
USBPORT_FreeHighResTimer(
    _In_ PVOID HighResTimer,
    _In_ BOOLEAN Synchronous
    );

BOOLEAN
USBPORT_CancelHighResTimer(
    _In_ PVOID HighResTimer
    );

BOOLEAN
USBPORT_FindUrbInList(
    PTRANSFER_URB Urb,
    PLIST_ENTRY ListHead
    );


VOID
USBPORTSVC_LogEntry(
    __inout PDEVICE_DATA DeviceData,
    ULONG Mask,
    ULONG Sig,
    ULONG_PTR Info1,
    ULONG_PTR Info2,
    ULONG_PTR Info3
    );

PUCHAR
USBPORTSVC_MapHwPhysicalToVirtual(
    HW_32BIT_PHYSICAL_ADDRESS HwPhysicalAddress,
    PDEVICE_DATA DeviceData,
    PENDPOINT_DATA EndpointData
    );

VOID
USBPORT_FlushAbortList(
    PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_AbortEndpoint(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_ENDPOINT Endpoint,
    PIRP Irp
    );

VOID
USBPORT_NeoRemoveDevice(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_AbortAllTransfers(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    BOOLEAN DeviceGone
    );

VOID
USBPORT_AbortAllTransfersAndCloseAllPipes(
    PDEVICE_OBJECT  FdoDeviceObject,
    USBD_DEVH_EVENT Event
    );

VOID
USBPORT_InvalidateEndpoint(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_ENDPOINT Endpoint,
    ULONG IEPflags
    );

VOID
USBPORT_FlushClosedEndpointList(
    PDEVICE_OBJECT FdoDeviceObject
    );

PIRP
USBPORT_FindActiveTransferIrp(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    );


NTSTATUS
USBPORT_QueryD3ColdInterface(
    PDEVICE_OBJECT FdoDeviceObject
    );

NTSTATUS
USBPORT_QueryPciBusInterface(
    PDEVICE_OBJECT FdoDeviceObject
    );

NTSTATUS
USBPORT_ReadWriteConfigSpace(
    PDEVICE_OBJECT FdoDeviceObject,
    BOOLEAN Read,
    __in_bcount(Length)
    PVOID Buffer,
    ULONG Offset,
    ULONG Length
    );

NTSTATUS
USBPORT_UsbFdoUserIoctl(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PULONG BytesReturned
    );

VOID
USBPORT_UserSendOnePacket(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PPACKET_PARAMETERS PacketParameters
    );

VOID
USBPORT_UserRefreshHctRegKeys(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT RootHubPdo,
    PUSBUSER_REQUEST_HEADER Header,
    PULONG Flags
    );

#define USBPORT_ReadConfigSpace(fdo, buffer, offset, length) \
    USBPORT_ReadWriteConfigSpace((fdo), TRUE, (buffer), (offset), (length))

#define USBPORT_WriteConfigSpace(fdo, buffer, offset, length) \
    USBPORT_ReadWriteConfigSpace((fdo), FALSE, (buffer), (offset), (length))

BOOLEAN
USBPORT_ValidateConfigurtionDescriptor(
    PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor,
    USBD_STATUS *UsbdStatus
    );


VOID
USBPORT_UserGetControllerInfo_0(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_CONTROLLER_INFO_0 ControllerInfo_0
    );

VOID
USBPORT_UserRawResetPort(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT RootHubPdo,
    PUSBUSER_REQUEST_HEADER Header,
    PRAW_RESET_PORT_PARAMETERS Parameters
    );

VOID
USBPORT_GetRootPortStatus(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PRAW_ROOTPORT_PARAMETERS Parameters
    );

VOID
USBPORT_UserSetRootPortFeature(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PRAW_ROOTPORT_FEATURE Parameters
    );

VOID
USBPORT_UserClearRootPortFeature(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PRAW_ROOTPORT_FEATURE Parameters
    );

VOID
USBPORT_InitDM_Timer(
    PDEVICE_OBJECT  FdoDeviceObject
    );

VOID
USBPORT_StartDM_Timer(
    PDEVICE_OBJECT  FdoDeviceObject
    );

VOID
USBPORT_SuspendDM_Timer(
    PDEVICE_OBJECT  FdoDeviceObject
    );

VOID
USBPORT_ResumeDM_Timer(
    PDEVICE_OBJECT  FdoDeviceObject
    );

VOID
USBPORT_StopDM_Timer(
    PDEVICE_OBJECT  FdoDeviceObject
    );

VOID
USBPORT_IsrDpcWorker(
    PDEVICE_OBJECT FdoDeviceObject,
    BOOLEAN HcInterrupt
    );

VOID
USBPORT_InvalidateRootHub(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_HcQueueWakeDpc(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_HcWakeDpc(
    PKDPC Dpc,
    PVOID DeferredContext,
    PVOID SystemArgument1,
    PVOID SystemArgument2
    );

VOID
USBPORT_UserGetControllerKey(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_UNICODE_NAME ControllerKey
    );

VOID
USBPORT_UserPassThru(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_PASS_THRU_PARAMETERS PassThru
    );

DRIVER_CANCEL USBPORT_CancelPendingWakeIrp;

VOID
USBPORT_TurnUsbControllerOff(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_TurnUsbControllerOn(
     PDEVICE_OBJECT FdoDeviceObject
     );

PHC_POWER_STATE
USBPORT_GetHcPowerState(
    PDEVICE_OBJECT FdoDeviceObject,
    PHC_POWER_STATE_TABLE HcPowerStateTbl,
    SYSTEM_POWER_STATE SystemState
    );

VOID
USBPORT_ComputeHcPowerStates(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_CAPABILITIES HcDeviceCaps,
    PHC_POWER_STATE_TABLE HcPowerStateTbl
    );

REQUEST_POWER_COMPLETE USBPORT_PoRequestCompletionD0;
REQUEST_POWER_COMPLETE USBPORT_PoRequestCompletionDx;

USB_BUSIFFN_RESTORE_DEVICE USBPORTBUSIF_RestoreUsbDevice;

MP_ENDPOINT_STATUS
USBPORT_GetEndpointStatus(
    PHCD_ENDPOINT Endpoint,
    PEP_LOCK_HANDLE EpLock
    );

NTSTATUS
USBPORT_CloneDevice(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE OldDeviceHandle,
    PUSBD_DEVICE_HANDLE NewDeviceHandle
    );

VOID
USBPORT_SuspendController(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_RemoveDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    USBD_DEVH_EVENT Event,
    BOOLEAN LockDeviceConfigs,
    BOOLEAN DeviceGone
    );

NTSTATUS
USBPORT_InitRootHubDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_DEVICE_DESCRIPTOR  RootHubDeviceDescriptor,
    PUSBD_DEVICE_HANDLE RootHubDeviceHandle
    );

VOID
USBPORT_StopRootHubPdo(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject
    );

VOID
USBPORT_ResumeController(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_UserPowerInformation(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_POWER_INFO PowerInformation
    );

USBD_STATUS
USBPORT_InitializeIsoTransfer(
    PDEVICE_OBJECT FdoDeviceObject,
    PTRANSFER_URB Urb,
    PHCD_TRANSFER_CONTEXT Transfer
    );

BOOLEAN
USBPORT_InTextmodeSetup(
    VOID
    );

NTSTATUS
USBPORT_IsCompanionController(
    PDEVICE_OBJECT FdoDeviceObject,
    PBOOLEAN       ReturnResult
    );

USB_CONTROLLER_FLAVOR
USBPORT_GetHcFlavor(
    PDEVICE_OBJECT FdoDeviceObject,
    USHORT PciVendorId,
    USHORT PciProductId,
    UCHAR PciRevision
    );

USB_CONTROLLER_FLAVOR
USBPORT_GetHcFlavor_FromACPI(
    PDEVICE_OBJECT FdoDeviceObject,
    PACPI_DEVICE_INFORMATION_OUTPUT_BUFFER AcpiDeviceInformation
    );

VOID
USBPORT_FlushRhCallbacks(
    PDEVICE_OBJECT Fdo
    );

ULONG
ReadUsb2CfgFlag(
    PDEVICE_OBJECT Fdo
    );

VOID
USBPORT_InvokeRhCallback(
    PDEVICE_OBJECT Fdo
    );

VOID
USBPORT_ClosePipe(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_PIPE_HANDLE_I PipeHandle
    );

PUSBD_INTERFACE_HANDLE_I
USBPORT_GetInterfaceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_CONFIG_HANDLE ConfigurationHandle,
    UCHAR InterfaceNumber
    );

USBD_STATUS
USBPORT_InitializeInterfaceInformation(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_INTERFACE_INFORMATION InterfaceInformation,
    PUSBD_CONFIG_HANDLE ConfigHandle
    );

NTSTATUS
USBPORT_GetBusInterfaceUSBDI(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PIRP Irp
    );

PVOID
USBPORT_GetDeviceBusContext(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PVOID HubBusContext
    );

USB_BUSIFFN_GET_DEVICE_BUSCONTEXT USBPORTBUSIF_GetDeviceBusContext;

ULONG
USBPORT_ComputeTotalBandwidth(
    PDEVICE_OBJECT FdoDeviceObject,
    PVOID BusContext
    );

USB_MINIPORT_STATUS
USBPORT_NtStatus_TO_MiniportStatus(
    LONG NtStatus
    );

BOOLEAN
USBPORT_AllocateBandwidthUSB11(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_FreeBandwidthUSB11(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_FreeBandwidthUSB20(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_UserGetBandwidthInformation(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_BANDWIDTH_INFO BandwidthInfo
    );

PTRANSACTION_TRANSLATOR
USBPORT_GetTt(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    USHORT PortNumber,
    PVOID Object,
    ULONG Tag
    );

VOID
USBPORT_iShiftReferenceTt(
    PDEVICE_OBJECT FdoDeviceObject,
    PTRANSACTION_TRANSLATOR Tt,
    PVOID OldObject,
    PVOID NewObject,
    ULONG Tag
    );

NTSTATUS
USBPORT_InitializeTT(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE HubDeviceHandle,
    USHORT Port
    );

VOID
USBPORT_RemoveTT(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE HsHubDeviceHandle,
    PTRANSACTION_TRANSLATOR Tt
    );

VOID
USBPORT_RemoveTTs(
    PDEVICE_OBJECT      FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle
    );

VOID
USBPORTSVC_Wait(
    PDEVICE_DATA DeviceData,
    ULONG MillisecondsToWait
    );

KDEFERRED_ROUTINE USBPORT_AsyncTimerDpc;

NTSTATUS
USBPORTSVC_RequestAsyncCallbackEx(
    PDEVICE_DATA DeviceData,
    ULONG MilliSeconds,
    PVOID Context,
    ULONG ContextLength,
    PMINIPORT_CALLBACK CallbackFunction,
    PVOID *CbHandle,
    BOOLEAN RootHubPortFunction
    );

BOOLEAN
USBPORTSVC_CancelAsyncCallback(
    PDEVICE_DATA DeviceData,
    PVOID CbHandle,
    BOOLEAN RootHubPortFunction
    );

VOID
USBPORT_CancelAllAsyncTimerCallbacks(
    __in PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORTSVC_EtwEnableCallback (
    __in LPCGUID SourceId,
    __in ULONG IsEnabled,
    __in UCHAR Level,
    __in ULONGLONG MatchAnyKeyword,
    __in ULONGLONG MatchAllKeyword,
    __in_opt PEVENT_FILTER_DESCRIPTOR FilterData,
    __in_opt PVOID CallbackContext
    );

VOID
USBPORT_RundownAllHostControllers (
    VOID
    );

VOID
USBPORT_RundownHostController (
    PDEVICE_OBJECT FdoDeviceObject
    );

NTSTATUS
USBPORTSVC_EtwWrite(
    __in PDEVICE_DATA DeviceData,
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in_opt LPCGUID ActivityId,
    __in ULONG UserDataCount,
    __in_ecount_opt(UserDataCount) PEVENT_DATA_DESCRIPTOR  UserData
    );

BOOLEAN
USBPORTSVC_IgnoreResumeSignaling(
    __in PDEVICE_DATA DeviceData
    );

NTSTATUS
USBPORT_EtwWriteHc(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId
    );

NTSTATUS
USBPORT_EtwWriteHcData1(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1
    );

NTSTATUS
USBPORT_EtwWriteHcData2(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2
    );

NTSTATUS
USBPORT_EtwWriteHcData3(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3
    );

NTSTATUS
USBPORT_EtwWriteHcData4(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4
    );

NTSTATUS
USBPORT_EtwWriteHcData5(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4,
    __in const VOID*            DataPtr5,
    __in ULONG                  DataSize5
    );

NTSTATUS
USBPORT_EtwWriteHcDevice(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId
    );

NTSTATUS
USBPORT_EtwWriteHcDeviceData1(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1
    );

NTSTATUS
USBPORT_EtwWriteHcDeviceData2(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2
    );

NTSTATUS
USBPORT_EtwWriteHcDeviceData3(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3
    );

NTSTATUS
USBPORT_EtwWriteHcDeviceData4(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4
    );

NTSTATUS
USBPORT_EtwWriteHcDeviceData6(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4,
    __in const VOID*            DataPtr5,
    __in ULONG                  DataSize5,
    __in const VOID*            DataPtr6,
    __in ULONG                  DataSize6
    );

NTSTATUS
USBPORT_EtwWriteHcDeviceEndpoint(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PHCD_ENDPOINT          Endpoint,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId
    );

NTSTATUS
USBPORT_EtwWriteHcDeviceEndpointData2(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PHCD_ENDPOINT          Endpoint,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2
    );

NTSTATUS
USBPORT_EtwWriteHcDeviceEndpointData3(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PHCD_ENDPOINT          Endpoint,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3
    );

NTSTATUS
USBPORT_EtwWriteHcDeviceEndpointData4(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PHCD_ENDPOINT          Endpoint,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4
    );

NTSTATUS
USBPORT_EtwWriteHcDeviceEndpointData5(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PHCD_ENDPOINT          Endpoint,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4,
    __in const VOID*            DataPtr5,
    __in ULONG                  DataSize5
    );

NTSTATUS
USBPORT_EtwWriteHcDeviceEndpointData6(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PHCD_ENDPOINT          Endpoint,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4,
    __in const VOID*            DataPtr5,
    __in ULONG                  DataSize5,
    __in const VOID*            DataPtr6,
    __in ULONG                  DataSize6
    );

ULONG
USBPORT_GetDeviceCount(
    PDEVICE_OBJECT FdoDeviceObject
    );

NTSTATUS
USBPORT_ReferenceTt(
    PDEVICE_OBJECT FdoDeviceObject,
    PTRANSACTION_TRANSLATOR Tt,
    PVOID Object,
    ULONG Tag
    );

VOID
USBPORT_DereferenceTt(
    PDEVICE_OBJECT FdoDeviceObject,
    PTRANSACTION_TRANSLATOR Tt,
    PVOID Object,
    ULONG Tag
    );

VOID
USBPORT_UserGetBusStatistics0(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_BUS_STATISTICS_0 BusStatistics0
    );

ULONG
USBPORT_ComputeAllocatedBandwidth(
    PDEVICE_OBJECT FdoDeviceObject,
    PVOID BusContext
    );

VOID
USBPORT_EndpointTimeout(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_ENDPOINT Endpoint
    );


NTSTATUS
USBPORT_LegacyGetUnicodeName(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PULONG BytesReturned
    );

NTSTATUS
USBPORT_GetSymbolicName(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT DeviceObject,
    PUNICODE_STRING SymbolicNameUnicodeString
    );

VOID
USBPORT_UserGetRootHubName(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PUSB_UNICODE_NAME RootHubName
    );

USB_BUSIFFN_GET_DEVICE_INFORMATION USBPORTBUSIF_BusQueryDeviceInformation;

NTSTATUS
USBPORT_IdleNotificationRequest(
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    );

VOID
USBPORT_CompletePdoWaitWake(
    PDEVICE_OBJECT FdoDeviceObject
    );


VOID
USBPORT_CompleteHubPdoWaitWake(
    PDEVICE_OBJECT FdoDeviceObject,
    LONG Wake_Status
    );


VOID
USBPORT_CompletePendingIdleIrp(
    PDEVICE_OBJECT PdoDeviceObject
    );

VOID
USBPORT_RestoreController(
     PDEVICE_OBJECT FdoDeviceObject
     );

VOID
USBPORT_ErrorCompleteIsoTransfer(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_ENDPOINT Endpoiint,
    PHCD_TRANSFER_CONTEXT Transfer,
    PEP_LOCK_HANDLE EpLock
    );

BOOLEAN
USBPORT_SelectiveSuspendRegistry(
    PDEVICE_OBJECT FdoDeviceObject
    );

PUSBD_DEVICE_HANDLE
USBPORT_GetDeviceHandle(
    PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_DisarmHcForWake(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_AssertTransferUrb(
    __inout PTRANSFER_URB Urb
    );

VOID
USBPORTSVC_InvalidateController(
    PDEVICE_DATA DeviceData,
    USB_CONTROLLER_STATE ControllerState
    );

VOID
USBPORTSVC_BugCheck(
    PDEVICE_DATA DeviceData
    );

VOID
USBPORT_DbgAcquireSpinLock(
    PDEVICE_OBJECT FdoDeviceObject,
    PKSPIN_LOCK  SpinLock,
    PKIRQL OldIrql
    );

VOID
USBPORT_DbgReleaseSpinLock(
    PDEVICE_OBJECT FdoDeviceObject,
    PKSPIN_LOCK  SpinLock,
    KIRQL NewIrql
    );

VOID
USBPORT_PowerFault(
    PDEVICE_OBJECT FdoDeviceObject,
    PUCHAR MessageText
    );

USB_BUSIFFN_GET_CONTROLLER_INFORMATION USBPORTBUSIF_GetControllerInformation;

USB_BUSIFFN_CONTROLLER_SELECTIVE_SUSPEND USBPORTBUSIF_ControllerSelectiveSuspend;

USB_BUSIFFN_GET_ROOTHUB_SYM_NAME USBPORTBUSIF_GetRootHubSymbolicName;

USB_BUSIFFN_GET_EXTENDED_HUB_INFO USBPORTBUSIF_GetExtendedHubInformation;

VOID
USBPORT_BeginTransmitTriggerPacket(
    __inout PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_EndTransmitTriggerPacket(
    __inout PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_CatcTrap(
    __inout PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_RebalanceEndpoint(
    PDEVICE_OBJECT FdoDeviceObject,
    PLIST_ENTRY EndpointList
    );

NTSTATUS
USBPORT_GetGlobalHacks(
     PDEVICE_OBJECT FdoDeviceObject,
     PULONG GlobalDisableSS,
     PULONG GlobalDisableCCDetect,
     PULONG EnPMDebug,
     PULONG ForceHcD3NoWakeArm
     );

NTSTATUS
USBPORT_InitializeHsHub(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE HubDeviceHandle,
    ULONG TtCount
    );

USB_BUSIFFN_INITIALIZE_20HUB USBPORTBUSIF_InitailizeUsb2Hub;

VOID
USBPORTSVC_NotifyDoubleBuffer(
    PDEVICE_DATA DeviceData,
    PTRANSFER_PARAMETERS TransferParameters,
    PVOID DbSystemAddress,
    ULONG DbLength
    );

VOID
USBPORT_FlushAdapterDBs(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_TRANSFER_CONTEXT Transfer
    );

BOOLEAN
USBPORT_IsDeviceHighSpeed(
    PVOID BusContext
    );

VOID
USBPORT_UpdateAllocatedBw(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_UpdateAllocatedBwTt(
    PTRANSACTION_TRANSLATOR Tt
    );

USB_BUSIFFN_FLUSH_TRANSFERS USBPORTBUSIF_FlushTransfers;

USBD_STATUS
USBPORT_FlushIsoTransfer(
    PDEVICE_OBJECT FdoDeviceObject,
    PHCD_ENDPOINT Endpoint,
    PTRANSFER_PARAMETERS TransferParameters,
    PMINIPORT_ISO_TRANSFER IsoTransfer
    );

VOID
USBPORT_DoSetPowerD0(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_QueuePowerWorkItem(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    );

NTSTATUS
USBPORT_DoFdoDevicePowerState(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    );

NTSTATUS
USBPORT_DoFdoSystemPowerState(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    );

USB_BUSIFFN_ROOTHUB_INIT_NOTIFY USBPORTBUSIF_RootHubInitNotification;

VOID
USBPORT_SynchronizeControllersStart(
    PDEVICE_OBJECT FdoDeviceObject
    );

PDEVICE_OBJECT
USBPORT_FindUSB2Controller(
    PDEVICE_OBJECT CcFdoDeviceObject
    );

VOID
USBPORT_RegisterUSB2fdo(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_RegisterUSB1fdo(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_DeregisterUSB2fdo(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_DeregisterUSB1fdo(
    PDEVICE_OBJECT FdoDeviceObject
    );

PDEVICE_RELATIONS
USBPORT_FindCompanionControllers(
    PDEVICE_OBJECT Usb2FdoDeviceObject,
    BOOLEAN ReferenceObjects,
    BOOLEAN ReturnFdo
    );

VOID
USBPORT_UserSetRootPortFeature(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PRAW_ROOTPORT_FEATURE Parameters
    );

VOID
USBPORT_UserClearRootPortFeature(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBUSER_REQUEST_HEADER Header,
    PRAW_ROOTPORT_FEATURE Parameters
    );

USB_MINIPORT_STATUS
USBPORT_RootHub_PowerUsb2Port(
    PDEVICE_OBJECT FdoDeviceObject,
    USHORT Port
    );

NTSTATUS
USBPORT_BusEnumLogEntry(
    PVOID BusContext,
    ULONG DriverTag,
    ULONG EnumTag,
    ULONG P1,
    ULONG P2
    );

BOOLEAN
USBPORT_ValidateRootPortApi(
    PDEVICE_OBJECT FdoDeviceObject,
    ULONG PortNumber
    );

BOOLEAN
USBPORT_Check_DCA_Enabled(
    PDEVICE_OBJECT FdoDeviceObject
    );

BOOLEAN
USBPORT_Check_ForcePortsHighSpeed_Enabled(
    PDEVICE_OBJECT FdoDeviceObject
    );

BOOLEAN
USBPORT_ForcePortsHighSpeed_KeyEnabled(
    PDEVICE_OBJECT FdoDeviceObject
    );

USB_MINIPORT_STATUS
USBPORT_RootHub_PowerAndChirpAllCcPorts(
    PDEVICE_OBJECT FdoDeviceObject
    );

RHSTATUS
USBPORT_RootHub_HubRequest(
    PDEVICE_OBJECT FdoDeviceObject,
    PORT_OPERATION PortOperation
    );

USB_BUSIFFN_SET_DEVHANDLE_DATA USBPORTBUSIF_SetDeviceHandleData;

RHSTATUS
USBPORT_RootHub_HubRequest(
    PDEVICE_OBJECT FdoDeviceObject,
    PORT_OPERATION PortOperation
    );

NTSTATUS
USBPORT_ProcessHcWakeIrp(
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp,
    PUSB_IOREQUEST_CONTEXT UsbIoRequest
    );

IO_COMPLETION_ROUTINE USBPORT_HcWakeIrp_Io_Completion;

REQUEST_POWER_COMPLETE USBPORT_HcWakeIrp_Po_Completion;

VOID
USBPORT_DisarmHcForWake(
    PDEVICE_OBJECT FdoDeviceObject
    );

NTSTATUS
USBPORT_ArmHcForWake(
    PDEVICE_OBJECT FdoDeviceObject,
    BOOLEAN        WakeFromS0
    );

VOID
USBPORT_FlushCachedRegistryKeys(
    PDEVICE_OBJECT FdoDeviceObject
    );

NTSTATUS
USBPORT_GetCachedRegistryKeyValueForPdo(
    __in PDEVICE_OBJECT FdoDeviceObject,
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in BOOLEAN SoftwareBranch,
    __in_bcount(KeyNameStringLength) PCWSTR KeyNameString,
    __in ULONG KeyNameStringLength,
    __in_bcount(DataLength) PVOID Data,
    __in ULONG DataLength
    );

VOID
USBPORT_MpInterrupts(
    PDEVICE_OBJECT FdoDeviceObject,
    BOOLEAN Enable
    );

VOID
USBPORT_DoRootHubCallback(
    PDEVICE_OBJECT FdoDeviceObject
    );

NTSTATUS
USBPORT_IoctlGetHubName(
    PDEVICE_OBJECT PdoDeviceObject,
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    );

VOID
USBPORT_InsertIdleIrp(
    PIO_CSQ Csq,
    PIRP Irp
    );

VOID
USBPORT_RemoveIdleIrp(
    PIO_CSQ Csq,
    PIRP Irp
    );

PIRP
USBPORT_PeekNextIdleIrp(
    PIO_CSQ Csq,
    PIRP Irp,
    PVOID PeekContext
    );

VOID
USBPORT_AcquireIdleLock(
    PIO_CSQ Csq,
    PKIRQL  Irql
    );

VOID
USBPORT_ReleaseIdleLock(
    PIO_CSQ Csq,
    KIRQL Irql
    );

VOID
USBPORT_CompleteCanceledIdleIrp(
    PIO_CSQ Csq,
    PIRP Irp
    );

VOID
USBPORT_RemoveIdleIrp(
    PIO_CSQ Csq,
    PIRP Irp
    );

VOID
USBPORT_DoIdleNotificationCallback(
    PVOID Context
    );

VOID
USBPORT_SynchronizeRootHubCallback(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT Usb2Fdo
    );

VOID
USBPORT_ClearBusContextList(
    PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORT_FreeBugCheck(
    PKBUGCHECK_REASON_CALLBACK_RECORD BugCheckRecord
);

PKBUGCHECK_REASON_CALLBACK_RECORD
USBPORT_InitBugCheck(
    PKBUGCHECK_REASON_CALLBACK_ROUTINE BugCheckCallback
);

VOID
USBPORT_SetMiniDumpData(
    __inout PDEVICE_OBJECT FdoDeviceObject,
    __in ULONG DataLength,
    __in_bcount_opt(DataLength) PUCHAR DataBuffer
    );

VOID
USBPORT_ClearMiniDumpData(
    __inout PDEVICE_OBJECT FdoDeviceObject
    );

USB_BUSIFFN_GET_DEVICE_PERFORMANCE_INFO USBPORTBUSIF_GetDevicePerformanceInfo;

IO_WORKITEM_ROUTINE USBPORT_UsbPowerWorker;

NTSTATUS
USBPORT_QueuePowerWorker(
    PDEVICE_OBJECT          FdoDeviceObject,
    PIRP                    SystemPowerIrp,
    USB_POWER_WORK_PHASE    PowerWorkPhase
    );

VOID
USBPORT_SynchronizeUsbControllersResume(
    PDEVICE_OBJECT FdoDeviceObject
    );

USB_MINIPORT_STATUS
USBPORT_SyncPowerAndChirpUsb2Ports(
    PDEVICE_OBJECT Usb2Fdo
    );

VOID
USBPORT_ResumeUsbController(
    PDEVICE_OBJECT FdoDeviceObject
    );

NTSTATUS
USBPORT_NeoCreateDevice(
    PUSBD_DEVICE_HANDLE         *NewDeviceHandle,
    PDEVICE_OBJECT              FdoDeviceObject,
    PUSBD_DEVICE_HANDLE         HsHubDeviceHandle,
    USHORT                      PortStatus,
    PUSB_PORT_PATH              PortPath,
    PUSB_CD_ERROR_INFORMATION   CdErrInfo,
    USHORT                      TtPortNumber,
    PDEVICE_OBJECT              PdoDeviceObject,
    PUNICODE_STRING             PhysicalDeviceObjectName
    );

NTSTATUS
USBPORT_ReferenceAllDeviceHandles(
    PDEVICE_OBJECT  FdoDeviceObject,
    PVOID           Object,
    PIRP            Irp,
    ULONG           Tag,
    BOOLEAN         PendingFlag,
    PLIST_ENTRY     ListHead
    );

NTSTATUS
USBPORT_iReferenceDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PVOID Object,
    ULONG Tag
    );

NTSTATUS
USBPORT_rReferenceDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PVOID Object,
    PIRP Irp,
    ULONG Tag
    );


USBD_DEVH_STATE
USBPORT_GetDeviceHandleState(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle
    );

VOID
USBPORT_DereferenceDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PVOID Object,
    ULONG Tag,
    BOOLEAN PendingFlag
    );

VOID
USBPORT_DeleteDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE Devh,
    USBD_DEVH_EVENT Event
    );

PUSBD_DEVICE_HANDLE
USBPORT_AllocDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject
    );

USB_BUSIFFN_REF_DEVICE_HANDLE USBPORTBUSIF_RefDeviceHandle;

USB_BUSIFFN_DEREF_DEVICE_HANDLE USBPORTBUSIF_DerefDeviceHandle;

BOOLEAN
USBPORT_NeoValidDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    ULONG ValidMask,
    PVOID ReferenceObj,
    PIRP Irp,
    ULONG Tag,
    BOOLEAN PendingFlag
    );


BOOLEAN
USBPORT_UrbValidDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PVOID Urb,
    PIRP Irp
    );

NTSTATUS
USBPORT_GetRootHubDeviceHandle(
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    );

VOID
USBPORT_FreeDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE Devh
    );

USB_BUSIFFN_SET_DEVICE_HANDLE_IDLE_READY_STATE USBPORTBUSIF_SetDevH_IdleReadyState;

VOID
USBPORT_BusContextEvent(
    PBUS_CONTEXT Bc,
    USBPORT_BUS_CONTEXT_EVENT Event
    );

VOID
USBPORT_ScanDeviceHandleDeletedList(
    PDEVICE_OBJECT FdoDeviceObject
    );

USBPORT_PNP_STATE
USBPORT_SetHcPnpState(
    PDEVICE_OBJECT HcFdo,
    USBPORT_PNP_STATE NewPnpState,
    USBPORT_PNP_EVENT PnpEvent
    );

USBPORT_PNP_STATE
USBPORT_SetRhPnpState(
    PDEVICE_OBJECT HcFdo,
    USBPORT_PNP_STATE NewPnpState,
    USBPORT_PNP_EVENT PnpEvent
    );


NTSTATUS
USBPORT_RegisterAndEnableDeviceInterface(
    PDEVICE_EXTENSION Ext,
    PDEVICE_OBJECT Pdo,
    CONST GUID *Guid
    );

NTSTATUS
USBPORT_RegisterDeviceInterface(
    PDEVICE_EXTENSION Ext,
    PDEVICE_OBJECT Pdo,
    CONST GUID *Guid
    );

VOID
USBPORT_DeregisterDeviceInterface(
    PDEVICE_EXTENSION Ext
    );

NTSTATUS
USBPORT_EnableDeviceInterface(
    PDEVICE_EXTENSION Ext,
    PDEVICE_OBJECT Pdo
    );

VOID
USBPORT_DisableDeviceInterface(
    PDEVICE_EXTENSION Ext
    );

VOID
USBPORT_HubFailSetMiniDumpData(
    __inout PDEVICE_OBJECT FdoDeviceObject
    );

VOID
USBPORTBUSIF_UsbdBusGetUSBDIVersion(
    __in_opt PVOID BusContext,
    __out_opt PUSBD_VERSION_INFORMATION VersionInformation,
    __out_opt PULONG HcdCapabilities
    );

NTSTATUS
USBPORTBUSIF_UsbdBusQueryBusTime(
    __in_opt PVOID BusContext,
    __out_opt PULONG CurrentFrame
    );

BOOLEAN
USBPORTBUSIF_UsbdIsDeviceHighSpeed(
    __in PVOID BusContext
    );

NTSTATUS
USBPORTBUSIF_UsbdBusEnumLogEntry(
    __in_opt PVOID BusContext,
    __in ULONG DriverTag,
    __in ULONG EnumTag,
    __in ULONG P1,
    __in ULONG P2
    );

NTSTATUS
USBPORTBUSIF_UsbdQueryControllerType(
    __in_opt PVOID BusContext,
    __out_opt PULONG HcdiOptionFlags,
    __out_opt PUSHORT PciVendorId,
    __out_opt PUSHORT PciDeviceId,
    __out_opt PUCHAR PciClass,
    __out_opt PUCHAR PciSubClass,
    __out_opt PUCHAR PciRevisionId,
    __out_opt PUCHAR PciProgIf
    );

NTSTATUS
USBPORTBUSIF_UsbdBusSubmitIsoOutUrb(
    __in_opt PVOID BusContext,
    __in_opt PURB Urb
    );

NTSTATUS
USBPORTBUSIF_UsbdBusQueryBusInformation(
    __in_opt PVOID BusContext,
    __in ULONG Level,
    PVOID BusInformationBuffer,
    __out_opt PULONG BusInformationBufferLength,
    __out_opt PULONG BusInformationActulaLength
    );

NTSTATUS
USBPORT_MakeHcdDeviceName(
    PUNICODE_STRING DeviceNameUnicodeString,
    ULONG Idx
    );

NTSTATUS
USBPORT_CreateLegacyFdoSymbolicLink(
    PDEVICE_OBJECT FdoDeviceObject
    );

ULONG
USBPORT_GetTotal20Bandwidth(
    PDEVICE_OBJECT FdoDeviceObject
    );

NTSTATUS
USBPORT_EndpointInsertAttendList(
    PDEVICE_OBJECT HcFdo,
    PHCD_ENDPOINT Endpoint
    );

VOID
USBPORT_FreeControllerCommonBuffers(
    PDEVICE_OBJECT HcFdo
    );

NTSTATUS
USBPORT_AllocateControllerCommonBuffers(
    PDEVICE_OBJECT HcFdo,
    PHC_RESOURCES HcResources
    );


NTSTATUS
USBPORT_IoctlGetTopologyAddress(
    PDEVICE_OBJECT PdoDeviceObject,
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    );

ULONG
USBPORT_MapS3toS4(
    PDEVICE_EXTENSION FdoDevExt
    );

BOOLEAN
USBPORT_IsHybridSleepState(
    PDEVICE_OBJECT HcFdo,
    PIRP SystemPowerIrp,
    PSYSTEM_POWER_STATE EffectiveSystemState
    );

NTSTATUS
USBPORTBUSIF_UsbdBusQueryBusTimeEx(
    __in_opt PVOID BusContext,
    __out_opt PULONG CurrentMicroFrame
    );

VOID
USBPORT_ClearTtCommandCallback(
    PDEVICE_OBJECT HcFdo,
    PTRANSFER_URB Urb,
    PVOID UrbCallbackContext,
    LONG NtStatus
    );

VOID
USBPORT_ClearTtBufferAndWait(
    PDEVICE_OBJECT HcFdo,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PHCD_ENDPOINT Endpoint
    );

NTSTATUS
USBPORT_ClearTtBuffer(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PHCD_ENDPOINT Endpoint,
    PKEVENT CommandCompleteEvent
    );

VOID
USBPORT_SignalInterrupt(
    __in PDEVICE_OBJECT FdoDeviceObject
    );

NTSTATUS
USBPORT_RefValidDeviceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PVOID Object,
    PIRP Irp,
    ULONG Tag
    );

NTSTATUS
USBPORT_AllocateForwardProgressContext(
    __in PDEVICE_OBJECT HcFdo,
    __in PUSB_FORWARD_PROGRESS_INFO FpInfo
    );

NTSTATUS
USBPORT_SendSynchronousUsbIoctlRequest(
    PDEVICE_OBJECT HcFdo,
    ULONG Ioctl
    );

ULONG
USBPORT_SetDevH_IdleReadyState(
    PDEVICE_OBJECT HcFdo,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    ULONG NewIdleReadyState
    );


NTSTATUS
USBPORT_IoctlGetDumpPointers(
    PDEVICE_OBJECT PdoDeviceObject,
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    );


NTSTATUS
USBPORT_IoctlFreeDumpPointers(
    PDEVICE_OBJECT PdoDeviceObject,
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    );

NTSTATUS
USBPORT_IoctlNotifyForwardProgress(
    PDEVICE_OBJECT PdoDeviceObject,
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    );

VOID
USBPORT_PreInitializeEmergencyWorkerThread(
    );

NTSTATUS
USBPORT_InitializeEmergencyWorkerThread(
    );

USB_WORKITEM_HANDLE
USBPORT_AllocateWorkItem(
    __in PDEVICE_OBJECT DeviceObject,
    __in BOOLEAN ForwardProgress
    );

VOID
USBPORT_FreeWorkItem(
    __in USB_WORKITEM_HANDLE IoWorkItem
    );

VOID
USBPORT_QueueWorkItem(
    __in PDEVICE_OBJECT DeviceObject,
    __in USB_WORKITEM_HANDLE IoWorkItem,
    __in PIO_WORKITEM_ROUTINE_EX WorkerRoutine,
    __in WORK_QUEUE_TYPE QueueType,
    __in_opt PVOID Context,
    __in BOOLEAN ForwardProgress
    );

VOID
USBPORT_DeferIrpProcessing(
    __in PDEVICE_OBJECT DeviceObject,
    __in PUSB_IRP_WORKER_ROUTINE IrpWorkerRoutine,
    __in PIRP Irp
    );

ULARGE_INTEGER
USBPort_GetTimeStampInMicroSeconds(
        );

NTSTATUS
USBPORT_EtwWriteCodeExecutionTime(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PETW_EXECUTION_METRICS ExecutionMetrics,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr,
    __in ULONG                  DataSize
    );

NTSTATUS
USBPORT_EtwWriteDmaExecutionTime(
    __in PDEVICE_OBJECT         FdoDeviceObject,
    __in PUSBD_DEVICE_HANDLE    DeviceHandle,
    __in PHCD_ENDPOINT          Endpoint,
    __in PETW_EXECUTION_METRICS ExecutionMetrics,
    __in PCEVENT_DESCRIPTOR     EventDescriptor,
    __in_opt LPCGUID            ActivityId,
    __in const VOID*            DataPtr1,
    __in ULONG                  DataSize1,
    __in const VOID*            DataPtr2,
    __in ULONG                  DataSize2,
    __in const VOID*            DataPtr3,
    __in ULONG                  DataSize3,
    __in const VOID*            DataPtr4,
    __in ULONG                  DataSize4,
    __in const VOID*            DataPtr5,
    __in ULONG                  DataSize5,
    __in PCWSTR                 Tag
    );

VOID
USBPORT_SetSpecialAcpiDevFunc(
    __in PDEVICE_EXTENSION FdoDevExt
    );

BOOLEAN
USBPORT_IsEnumeratorEqual(
    __in PDEVICE_EXTENSION FdoDevExt,
    __in LPCWSTR           ExpectedName
    );

#include "endpoint.h"
#include "roothub.h"
#include "usb2.h"
#include "miniport.h"
#include "core.h"
#include "xdpc.h"
#include "cancel.h"
#include "xsplit.h"
#include "lookaside.h"

#ifdef EN_TEST_CODE

NTSTATUS
USBPORT_QueueTestWorkerItem(
    PDEVICE_OBJECT HcFdo
    );

VOID
USBPORT_StopDevice_Delay(
    PDEVICE_OBJECT HcFdo
    );

#endif //EN_TEST_CODE

#endif /* __PROTOTYP_H__ */

