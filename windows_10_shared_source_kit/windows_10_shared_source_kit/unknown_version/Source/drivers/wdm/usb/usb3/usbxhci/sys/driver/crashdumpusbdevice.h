/*++
Copyright (c) Microsoft Corporation

Module Name:

    crashdumpusbdevice.h

Abstract:

    USB XHCI Crashdump write module.

--*/

#define CRASHDUMP_CONFIGURABLE_ENDPOINTS_BITMASK    0xFFFFFFFC

#define CRASHDUMP_HUB_PORT_RESET_WAIT_COUNT         1000        // 20s
#define CRASHDUMP_HUB_PORT_RESET_WAIT_TIME          20000       // 20ms

#define CRASHDUMP_HUB_PORT_CONNECT_WAIT_COUNT       5           // 500ms
#define CRASHDUMP_HUB_PORT_CONNECT_WAIT_TIME        100000      // 100ms

#define CRASHDUMP_HUB_PORT_POWER_WAIT_TIME          300000      // 300ms

typedef struct _CRASHDUMP_USBDEVICE_DATA {

    HANDLE                  DumperHandle;
    HANDLE                  RegisterHandle;
    HANDLE                  CommandHandle;
    HANDLE                  UsbDeviceHandle;

    CRASHDUMP_BUFFER_DATA   DeviceContextBuffer;

    UCHAR                   SlotId;
    PVOID                   OutputDeviceContext;
    PSLOT_CONTEXT64         OutputSlotContext;
    SLOT_CONTEXT64          SlotContext;

    HANDLE                  EndpointHandle[MAX_DEVICE_CONTEXT_INDEX];

    BOOLEAN                 IsHub;
    UCHAR                   ConfigurationId;
    UCHAR                   InterfaceId;
    UCHAR                   AlternateSettingId;

    ULONG                   PortPathDepth;

} CRASHDUMP_USBDEVICE_DATA, *PCRASHDUMP_USBDEVICE_DATA;

//
// Public functions.
//
_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Crashdump_UsbDevice_Initialize(
    __inout
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        HANDLE                      DumperHandle,
    __in_opt
        PUSB_DUMP_DEVICE_INFO       DumpDeviceInfo,
    __in
        UCHAR                       SlotId,
    __in
        PVOID                       DeviceContext,
    __in
        HANDLE                      UsbDeviceHandle,
    __in 
        ULONG                       PortPathDepth
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
ULONG
Crashdump_UsbDevice_GetRequiredBufferSize(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    );

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_PrepareForHibernate(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_ReConfigureOnControllerReset(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in 
        CRASHDUMP_PORT_TYPE         PortType
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_ReConfigureOnPortReset(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in 
        CRASHDUMP_PORT_TYPE         PortType
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_Configure(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in 
        CRASHDUMP_PORT_TYPE         PortType
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_ConfigureEndpoints(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        ULONG                       EndpointSelectionBitMask,
    __in
        BOOLEAN                     ReConfigure
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_ResetPort(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        ULONG                       PortNumber,
    __in
        CRASHDUMP_PORT_TYPE         PortType
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_SetPortPower(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        ULONG                       PortNumber
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_GetPortType(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        ULONG                       PortNumber,
    __out
        CRASHDUMP_PORT_TYPE         *PortType
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_SuspendPort(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        ULONG                       PortNumber
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Crashdump_UsbDevice_WriteDoorBell(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        ULONG                       EndpointDCI,
    __in
        ULONG                       StreamId
    )
{
    DOORBELL_REGISTER   doorbellRegister;

    doorbellRegister.AsUlong32 = 0;
    doorbellRegister.Target = EndpointDCI;
    doorbellRegister.StreamId = StreamId;

    Register_WriteUlong(
        &Crashdump_Register_GetDoorbellRegister(UsbDeviceData->RegisterHandle, UsbDeviceData->SlotId)->AsUlong32,
        doorbellRegister.AsUlong32);

    CrashdumpLogInfo("Ringing doorbell: SlotId: %u, EndpointDCI: %u", UsbDeviceData->SlotId, EndpointDCI);
}

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
VOID
Crashdump_UsbDevice_SetEndpointHandle(
    __inout
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        ULONG                       EndpointDCI,
    __in __drv_aliasesMem
        HANDLE                      EndpointHandle
    )
{
    UsbDeviceData->EndpointHandle[EndpointDCI] = EndpointHandle;
}

__checkReturn
__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
HANDLE
Crashdump_UsbDevice_GetEndpointHandle(
    __inout
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        ULONG                       EndpointDCI
    )
{
    return UsbDeviceData->EndpointHandle[EndpointDCI];
}

__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
ULONG
Crashdump_UsbDevice_GetSlotId(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    )
{
    return UsbDeviceData->SlotId;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
NTSTATUS
Crashdump_UsbDevice_SendControlTransfer(
    __in
        PCRASHDUMP_USBDEVICE_DATA       UsbDeviceData,
    __in
        PUSB_DEFAULT_PIPE_SETUP_PACKET  SetupPacket,
    __in
        ULONG                           DataBufferLength,
    __in_opt
        PPHYSICAL_ADDRESS               DataBuffer
    )
{
    return Crashdump_Endpoint_SendControlTransfer(UsbDeviceData->EndpointHandle[1],
                                                  SetupPacket,
                                                  DataBufferLength,
                                                  DataBuffer);
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
NTSTATUS
Crashdump_UsbDevice_SendBulkTransfer(
    __in
        PCRASHDUMP_USBDEVICE_DATA       UsbDeviceData,
    __in
        UCHAR                           EndpointDCI,
    __in
        PURB_BULK_OR_INTERRUPT_TRANSFER BulkUrb,
    __in
        PPHYSICAL_ADDRESS               TransferPAArray,
    __in
        BOOLEAN                         IsTransferAsync
    )
{
    return Crashdump_Endpoint_SendBulkTransfer(UsbDeviceData->EndpointHandle[EndpointDCI],
                                               BulkUrb,
                                               TransferPAArray,
                                               IsTransferAsync);
}

//
// Private functions.
//
__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_SendEnableSlotCommand(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_SendResetDeviceCommand(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_SendAddressDeviceCommand(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_Cleanup(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
ULONG
Crashdump_UsbDevice_GetSlotState(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    )
{
    return UsbDeviceData->OutputSlotContext->SlotState;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Crashdump_UsbDevice_SetOutputSlotContext(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        PVOID                       DeviceContext
    )
{
    if (Crashdump_Register_GetContextSize(UsbDeviceData->RegisterHandle) == REGISTER_CONTEXTSIZE64) {

        UsbDeviceData->OutputSlotContext =
            UsbDevice_DeviceContext64GetSlot((PDEVICE_CONTEXT64)DeviceContext);

    } else {

        UsbDeviceData->OutputSlotContext =
            (PSLOT_CONTEXT64)
                UsbDevice_DeviceContext32GetSlot((PDEVICE_CONTEXT32)DeviceContext);
    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
FORCEINLINE
VOID
Crashdump_UsbDevice_SetSlotContext(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        PVOID                       DeviceContext
    )
{
    if (Crashdump_Register_GetContextSize(UsbDeviceData->RegisterHandle) == REGISTER_CONTEXTSIZE64) {

        RtlCopyMemory(&UsbDeviceData->SlotContext,
                      UsbDevice_DeviceContext64GetSlot((PDEVICE_CONTEXT64)DeviceContext),
                      sizeof(SLOT_CONTEXT64));

    } else {

        RtlCopyMemory(&UsbDeviceData->SlotContext,
                      UsbDevice_DeviceContext32GetSlot((PDEVICE_CONTEXT32)DeviceContext),
                      sizeof(SLOT_CONTEXT32));
    }
}

__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Crashdump_UsbDevice_LogSlotContext(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    )
{
    CrashdumpLogInfo("Slot %u: RH %u, RS %u, Speed %u, IsHub %u, NumEntries %u, Addr %u, State %u",
                     Crashdump_UsbDevice_GetSlotId(UsbDeviceData),
                     UsbDeviceData->OutputSlotContext->RootHubPortNumber,
                     UsbDeviceData->OutputSlotContext->RouteString,
                     UsbDeviceData->OutputSlotContext->Speed,
                     UsbDeviceData->OutputSlotContext->Hub,
                     UsbDeviceData->OutputSlotContext->ContextEntries,
                     UsbDeviceData->OutputSlotContext->UsbDeviceAddress,
                     Crashdump_UsbDevice_GetSlotState(UsbDeviceData));
}

__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Crashdump_UsbDevice_LogPortStatus(
    __in
        ULONG                       PortNumber,
    __in
        PUSB_PORT_STATUS_AND_CHANGE PortSC
    )
{
    CrashdumpLogInfo("Port %u: CCS: %u, PE: %u, OC: %u, PR: %u, PP: %u, HS: %u",
                     PortNumber,
                     PortSC->PortStatus.Usb20PortStatus.CurrentConnectStatus,
                     PortSC->PortStatus.Usb20PortStatus.PortEnabledDisabled,
                     PortSC->PortStatus.Usb20PortStatus.OverCurrent,
                     PortSC->PortStatus.Usb20PortStatus.Reset,
                     PortSC->PortStatus.Usb20PortStatus.PortPower,
                     PortSC->PortStatus.Usb20PortStatus.HighSpeedDeviceAttached);

    CrashdumpLogInfo("PortChange %u: CSC: %u, PEDC: %u, OCC: %u, PRC: %u",
                     PortNumber,
                     PortSC->PortChange.Usb20PortChange.ConnectStatusChange,
                     PortSC->PortChange.Usb20PortChange.PortEnableDisableChange,
                     PortSC->PortChange.Usb20PortChange.OverCurrentIndicatorChange,
                     PortSC->PortChange.Usb20PortChange.ResetChange);
}

__drv_requiresIRQL(DISPATCH_LEVEL)
FORCEINLINE
VOID
Crashdump_UsbDevice_LogDeviceDescriptor(
    __in
        PUSB_DEVICE_DESCRIPTOR      DeviceDescriptor
    )
{
    CrashdumpLogInfo("%u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u, %u",
                     DeviceDescriptor->bLength,
                     DeviceDescriptor->bDescriptorType,
                     DeviceDescriptor->bcdUSB,
                     DeviceDescriptor->bDeviceClass,
                     DeviceDescriptor->bDeviceSubClass,
                     DeviceDescriptor->bDeviceProtocol,
                     DeviceDescriptor->bMaxPacketSize0,
                     DeviceDescriptor->idVendor,
                     DeviceDescriptor->idProduct,
                     DeviceDescriptor->bcdDevice,
                     DeviceDescriptor->iManufacturer,
                     DeviceDescriptor->iProduct,
                     DeviceDescriptor->iSerialNumber,
                     DeviceDescriptor->bNumConfigurations);
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_GetPortStatus(
    __in
        PCRASHDUMP_USBDEVICE_DATA       UsbDeviceData,
    __in
        ULONG                           PortNumber,
    __out
        PUSB_PORT_STATUS_AND_CHANGE*    PortSC
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_ChangePortFeature(
    __in
        PCRASHDUMP_USBDEVICE_DATA       UsbDeviceData,
    __in
        ULONG                           PortNumber,
    __in
        USB_PORT_FEATURE_SELECTOR       PortFeature,
    __in
        UCHAR                           Request
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_GetDeviceDescriptor(
    __in
        PCRASHDUMP_USBDEVICE_DATA       UsbDeviceData,
    __out
        PUSB_DEVICE_DESCRIPTOR*         DeviceDescriptor
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_ConfigureEndpointsForSaveState(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    );

