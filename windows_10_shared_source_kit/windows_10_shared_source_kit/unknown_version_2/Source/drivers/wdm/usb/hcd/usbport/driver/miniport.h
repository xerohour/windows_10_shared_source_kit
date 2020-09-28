//
//    Copyright (C) Microsoft.  All rights reserved.
//


VOID
MPf_FlushInterrupts(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

ULONG
MPf_CheckHwSync(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    );

NTSTATUS
MPf_UsbxAddDevice(
    PDEVICE_EXTENSION HcFdoExt
    );

USB_MINIPORT_STATUS
MPf_UsbxOpenEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR Tt,
    __inout_opt USBD_STATUS *UsbdStatus
    );

USB_MINIPORT_STATUS
MPf_UsbxPokeEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR Tt
    );

USB_MINIPORT_STATUS
MPf_UsbxInitHsbTransactionTranslator(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in PTRANSACTION_TRANSLATOR Tt
    );

USB_MINIPORT_STATUS
MPf_UsbxInitHsbEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __in PTRANSACTION_TRANSLATOR Tt
    );

ULONG
MPf_UsbxQueryTtBandwidthData(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PTRANSACTION_TRANSLATOR Tt,
    __in ULONG Frame
    );

ULONG
MPf_UsbxQueryBandwidthData(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in ULONG Frame,
    __in ULONG Microframe
    );

ULONG
MPf_UsbxQueryEpBandwidthData(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout_opt PTRANSACTION_TRANSLATOR Tt,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
MPf_NotifyTransferQueueState(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in ULONG AsyncTransferCount,
    __in ULONG PeriodicTransferCount
    );

VOID
MPf_TakePortControl(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

VOID
MPf_ReleasePortControl(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

RHSTATUS
MPf_SetWakeOnConnect(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in ULONG EnableFlag
    );

ULONG
MPf_ReadCfgFlag(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

USB_MINIPORT_STATUS
MPf_ResumeController(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

VOID
MPf_SuspendController(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in BOOLEAN EnableRH_IRQ
    );

USB_MINIPORT_STATUS
MPf_PreStartController(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHC_RESOURCES HcResources,
    __in ULONG StartFlags
    );

USB_MINIPORT_STATUS
MPf_StartController(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHC_RESOURCES HcResources,
    __in ULONG StartFlags
    );

MP_ENDPOINT_STATE
MPf_GetEndpointState(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    );

MP_ENDPOINT_STATUS
MPf_GetEndpointStatus(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
MPf_SetEndpointStatus(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __in MP_ENDPOINT_STATUS EpStatus
    );

VOID
MPf_PollEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
MPf_ResetController(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

VOID
MPf_ResetControllerDontReinit(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

VOID
MPf_PostResetController(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

VOID
MPf_ReinitController(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

VOID
MPf_CheckController(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

VOID
MPf_InterruptNextSOF(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

ULONG
MPf_Get32BitFrameNumber(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

VOID
MPf_SetEndpointDataToggle(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __in ULONG ToggleValue
    );


VOID
MPf_AbortTransfer(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer,
    __inout PULONG BytesTransferred
    );

VOID
MPf_PollController(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

BOOLEAN
MPf_InterruptService(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

ULONG
MPf_InterruptDpcEx(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in BOOLEAN EnableInterrupts
    );

VOID
MPf_StopController(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in BOOLEAN HardwarePresent
    );

VOID
MPf_StopControllerWithInterrupts(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

VOID
MPf_CloseEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    );

USB_MINIPORT_STATUS
MPf_OpenEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt USBD_STATUS *UsbdStatus
    );

VOID
MPf_DbgFreeEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID 
MPf_HaltController(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

VOID
MPf_RebalanceEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    );

USB_MINIPORT_STATUS
MPx_RebalanceEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
MPf_SetEndpointState(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __in MP_ENDPOINT_STATE NewEpState
    );

ULONG
MPx_MpRevision(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

NTSTATUS
MPx_InitializeHsbController(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

VOID
MPx_QueryEndpointRequirements(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __out   PENDPOINT_REQUIREMENTS Requirements
    );

USB_MINIPORT_STATUS
MPx_OpenEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt USBD_STATUS *UsbdStatus
   );

USB_MINIPORT_STATUS
MPx_PokeEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR Tt
    );

USB_MINIPORT_STATUS
MPx_SubmitTransfer(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PHCD_TRANSFER_CONTEXT Transfer
    );

ULONG
MPx_QueryBandwidthData(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in ULONG Frame,
    __in ULONG Microframe
    );

ULONG
MPx_QueryTtBandwidthData(
    PDEVICE_EXTENSION HcFdoExt,
    PTRANSACTION_TRANSLATOR Tt,
    ULONG Frame
    );

ULONG
MPf_Get32BitMicroFrameNumber(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

ULONG
MPx_QueryEpBandwidthData(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    );

NTSTATUS
MPx_InitializeHsbEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR TransactionTranslator
    );

NTSTATUS
MPx_InitializeHsbTt(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PTRANSACTION_TRANSLATOR TransactionTranslator
    );

NTSTATUS
MPx_InitializeHsbController(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

VOID
MPx_InitializeHsb(
    VOID
    );

BOOLEAN
MPx_AllocateBandwidth(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
MPx_FreeBandwidth(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    );

VOID
MPf_CloseDevHandle(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PUSBD_DEVICE_HANDLE DevH
    );

VOID
MPf_OpenDevHandle(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PUSBD_DEVICE_HANDLE DevH
    );

VOID
MPf_DeleteDeviceData(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

VOID
MPf_CreateDeviceData(
    __inout PDEVICE_EXTENSION HcFdoExt
    );

NTSTATUS
MPf_GetContainerIdForPort(
    __in PDEVICE_EXTENSION HcFdoExt,
    __in USHORT PortNumber,
    __inout LPGUID ContainerId
    );

VOID
MPf_SetContainerIdForPort(
    __in PDEVICE_EXTENSION HcFdoExt,
    __in USHORT PortNumber,
    __in LPGUID ContainerId
    );

NTSTATUS
MPf_GetHcCapabilities(
    __inout PDEVICE_EXTENSION   HcFdoExt
    );

USHORT
MPf_GetDebugPortNumber(
    __inout PDEVICE_EXTENSION   HcFdoExt
    );

BOOLEAN
MPf_CheckInvalidateRootHub(
    __inout PDEVICE_EXTENSION   HcFdoExt
    );

VOID
MPf_SetClearPortResumeTime(
    __inout PDEVICE_EXTENSION   HcFdoExt,
    __in USHORT PortNumber,
    __in BOOLEAN SetTime,
    __in UCHAR ResumeTime
    );
