/*++
Copyright (c) Microsoft Corporation

Module Name:

    usbdevice.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

//
// TODO: PostWin8 Define in a better place.
//
#define MAX_DEVICE_CONTEXT_INDEX                32

//
// Public definitions.
//
typedef
__drv_maxIRQL(DISPATCH_LEVEL)
VOID
(USBDEVICE_COMPLETION_ROUTINE) (
    __in
        PVOID               CompletionContext,
    __in
        NTSTATUS            CompletionStatus
    );

typedef USBDEVICE_COMPLETION_ROUTINE *PUSBDEVICE_COMPLETION_ROUTINE;

typedef struct _USBDEVICE_DATA {

    UCXUSBDEVICE                UcxUsbDevice;

    HANDLE                      ControllerHandle;

    UCXUSBDEVICE_INFO           UsbDeviceInfo;

    //
    // Links this device into the list of all devices that exist for
    // this controller.
    //
    LIST_ENTRY                  DeviceListEntry;

    //
    // List of all endpoints that exist for this device.
    //
    KSPIN_LOCK                  EndpointListLock;
    LIST_ENTRY                  EndpointListHead;
    ULONG                       EndpointListCount;

    USB_DEVICE_DESCRIPTOR       DeviceDescriptor;

    //
    // See 4.5.3 Slot States, Figure 10: Slot State Diagram
    //
    // SlotEnabled == FALSE if the Device Slot state is Disabled.
    // SlotEnabled == TRUE  if the Device Slot state is Enabled,
    // Default, Addressed, or Configured.
    //
    BOOLEAN                     SlotEnabled;

    UCHAR                       SlotId;

    BOOLEAN                     IsHub;
    ULONG                       NumberOfPorts;
    ULONG                       NumberOfTTs;
    ULONG                       TTThinkTime;

    ULONG                       ContextEntriesInUse;

    PDOORBELL_REGISTER          DoorbellRegister;

    HANDLE                      EndpointHandle[MAX_DEVICE_CONTEXT_INDEX];

    //
    // This buffer is owned by the controller once the device is programmed into it.
    // It is pre-allocated and kept for the lifetime of the usbdevice object instead
    // of allocating in EvtEnable and deallocating in EvtDisable, for the following
    // reason: In forward progress case, EvtEnable can be invoked more than once if
    // controller reset was done, but EvtEnable should not fail on the second callback
    // because of forward progress guarantee.
    //
    PBUFFER_DATA                DeviceContextBuffer;

    //
    // This buffer is used to send commands like address_device, configure_endpoint
    // etc. It is pre-allocated and kept for the lifetime of the usbdevice object
    // for the following reason: EvtEndpointsConfigure UCX callback requires that
    // it should not be failed if endpoints are being deconfigured. But sending the
    // configure_endpoint command requires input device context buffer, which could
    // cause allocation failure if allocated dynamically.
    //
    PBUFFER_DATA                InputDeviceContextBuffer;

    //
    // The CRB and WdfRequest are stored in the UsbDevice data. This
    // is only safe if there is one pending command at a given time.
    //
    WDFREQUEST                  WdfRequest;
    BOOLEAN                     WdfRequestMustSucceed;

    CRB                         Crb;
    PUSBDEVICE_COMPLETION_ROUTINE   CrbCompletionRoutine;
    PVOID                       CrbCompletionContext;

    BOOLEAN                     Is20LpmEnabled;
    BOOLEAN                     IsRootPortResumeTimeSet;

    UCHAR                       ConfigurationValue;
    UCHAR                       AlternateSetting;
    UCHAR                       InterfaceNumber;

} USBDEVICE_DATA, *PUSBDEVICE_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(USBDEVICE_DATA, GetUsbDeviceData)

typedef struct _USBDEVICE_ENDPOINTS_CONFIGURE_DATA {

    //
    // TRUE if and only if the EndpointsToEnableCount is non-zero and an
    // Endpoint_Enable() call failed prior to issuing the Configure
    // Endpoint Command command, and the EndpointsToDisableCount is
    // non-zero.  This will result in a issuing the first and only
    // Configure Endpoint Command command to disable the endpoints in
    // the EndpointsToDisable[] array.
    //
    BOOLEAN                     EndpointEnableFailed;

    //
    // TRUE if and only if EndpointsToEnableCount is non-zero and all
    // Endpoint_Enable() calls were successful but the first Configure
    // Endpoint Command command failed, and the EndpointsToDisableCount
    // is non-zero.  This will result in a issuing a second Configure
    // Endpoint Command command to disable the endpoints in the
    // EndpointsToDisable[] array.
    //
    BOOLEAN                     ConfigureEndpointCommandFailed;

} USBDEVICE_ENDPOINTS_CONFIGURE_DATA, *PUSBDEVICE_ENDPOINTS_CONFIGURE_DATA;

//
// Public functions.
//
EVT_UCX_CONTROLLER_USBDEVICE_ADD UsbDevice_UcxEvtDeviceAdd;

EVT_WDF_OBJECT_CONTEXT_CLEANUP UsbDevice_EvtUsbDeviceCleanupCallback;

EVT_UCX_USBDEVICE_ENABLE UsbDevice_UcxEvtEnable;

COMMAND_CALLBACK UsbDevice_EnableCompletion;

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
UsbDevice_SetAddress(
    __in
        PUSBDEVICE_DATA UsbDeviceData,
    __in
        BOOLEAN         BlockSetAddressRequest
    );

COMMAND_CALLBACK UsbDevice_SetAddressCompletion;

EVT_UCX_USBDEVICE_ADDRESS UsbDevice_UcxEvtAddress;

EVT_UCX_USBDEVICE_UPDATE UsbDevice_UcxEvtUpdate;

EVT_UCX_USBDEVICE_HUB_INFO UsbDevice_UcxEvtHubInfo;

EVT_UCX_USBDEVICE_ENDPOINTS_CONFIGURE UsbDevice_UcxEvtEndpointsConfigure;

COMMAND_CALLBACK UsbDevice_EndpointsConfigureCompletion;

EVT_UCX_USBDEVICE_DISABLE UsbDevice_UcxEvtDisable;

COMMAND_CALLBACK UsbDevice_DisableCompletionReturnFailure;

COMMAND_CALLBACK UsbDevice_DisableCompletionReturnSuccess;

COMMAND_CALLBACK UsbDevice_OnResetDisableCompletion;

typedef enum _DISABLE_SLOT_NEXT_ACTION {

    DisableSlotNextAction_Complete,
    DisableSlotNextAction_CompleteWithFailure,
    DisableSlotNextAction_EnableSlot

} DISABLE_SLOT_NEXT_ACTION;

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_DisableCompletion(
    __in
        PCRB                        Crb,
    __in
        CRB_COMPLETION_STATUS       CrbCompletionStatus,
    __in
        DISABLE_SLOT_NEXT_ACTION
                                    NextAction
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_SetDeviceDisabled(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_ControllerGone(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
UsbDevice_ControllerResetPreReset(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
UsbDevice_ControllerResetPostReset(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    );

EVT_UCX_USBDEVICE_RESET UsbDevice_UcxEvtReset;

COMMAND_CALLBACK UsbDevice_DeviceResetCompletion;

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_TransferEventHandler(
    __in
        PUSBDEVICE_DATA     UsbDeviceData,
    __in
        PTRANSFER_EVENT_TRB TransferEventTrb
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_DeviceNotificationEventHandler(
    __in
        PUSBDEVICE_DATA UsbDeviceData,
    __in
        PTRB            Trb
    );

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
USB_DEVICE_SPEED
UsbDevice_GetDeviceSpeed(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    )
{
    return UsbDeviceData->UsbDeviceInfo.DeviceSpeed;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
PUSB_DEVICE_PORT_PATH
UsbDevice_GetPortPath(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    )
{
    return &UsbDeviceData->UsbDeviceInfo.PortPath;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
UCHAR
UsbDevice_GetSlotId(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    )
{
    return UsbDeviceData->SlotId;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
PBUFFER_DATA
UsbDevice_GetDeviceContextBuffer(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    )
{
    return UsbDeviceData->DeviceContextBuffer;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
VOID
UsbDevice_WriteDoorbell(
    __in
        PUSBDEVICE_DATA UsbDeviceData,
    __in
        ULONG           Target,
    __in
        ULONG           StreamId
    )
{
    DOORBELL_REGISTER   DoorbellRegister;

    DoorbellRegister.AsUlong32 = 0;
    DoorbellRegister.Target = Target;
    DoorbellRegister.StreamId = StreamId;

    Register_WriteUlong(&UsbDeviceData->DoorbellRegister->AsUlong32, DoorbellRegister.AsUlong32);
    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
__inline
VOID
UsbDevice_SetEndpointHandle(
    __in
        PUSBDEVICE_DATA UsbDeviceData,
    __in
        ULONG           ContextIndex,
    __in
        HANDLE          EndpointHandle
    )
{
    NT_ASSERT((UsbDeviceData->EndpointHandle[ContextIndex] == NULL) ||
              (UsbDeviceData->EndpointHandle[ContextIndex] == EndpointHandle));
    UsbDeviceData->EndpointHandle[ContextIndex] = EndpointHandle;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
__inline
VOID
UsbDevice_ClearEndpointHandle(
    __in
        PUSBDEVICE_DATA UsbDeviceData,
    __in
        ULONG           ContextIndex,
    __in
        HANDLE          EndpointHandle
    )
{
    UNREFERENCED_PARAMETER(EndpointHandle);

    NT_ASSERT((UsbDeviceData->EndpointHandle[ContextIndex] == NULL) ||
              (UsbDeviceData->EndpointHandle[ContextIndex] == EndpointHandle));
    UsbDeviceData->EndpointHandle[ContextIndex] = NULL;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
__inline
HANDLE
UsbDevice_GetEndpointHandle(
    __in
        PUSBDEVICE_DATA UsbDeviceData,
    __in
        ULONG           ContextIndex
    )
{
    return UsbDeviceData->EndpointHandle[ContextIndex];
}

__forceinline
PSLOT_CONTEXT32
UsbDevice_DeviceContext32GetSlot(
    __in
        PDEVICE_CONTEXT32    DeviceContext32
    )
/*++

Routine Description:

    Given a pointer to a 32-byte sized Device Context (see 6.2.1 Device
    Context) this routine returns a pointer to the 32-byte sized Slot
    Context (see 6.2.2 Slot Context) contained inside the Device Context.

--*/
{
    return &DeviceContext32->SlotContext;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
PDEVICE_CONTEXT32
UsbDevice_GetDeviceContext32(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    )
{
    return (PDEVICE_CONTEXT32)UsbDeviceData->DeviceContextBuffer->VirtualAddress;
}

__drv_maxIRQL(DISPATCH_LEVEL)
__inline
PDEVICE_CONTEXT64
UsbDevice_GetDeviceContext64(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    )
{
    return (PDEVICE_CONTEXT64)UsbDeviceData->DeviceContextBuffer->VirtualAddress;
}

__forceinline
UCHAR
UsbDevice_GetConfigurationValue(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    )
{
    return UsbDeviceData->ConfigurationValue;
}

__forceinline
UCHAR
UsbDevice_GetAlternateSetting(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    )
{
    return UsbDeviceData->AlternateSetting;
}

__forceinline
UCHAR
UsbDevice_GetInterfaceNumber(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    )
{
    return UsbDeviceData->InterfaceNumber;
}

__forceinline
PSLOT_CONTEXT64
UsbDevice_DeviceContext64GetSlot(
    __in
        PDEVICE_CONTEXT64    DeviceContext64
    )
/*++

Routine Description:

    Given a pointer to a 64-byte sized Device Context (see 6.2.1 Device
    Context) this routine returns a pointer to the 64-byte sized Slot
    Context (see 6.2.2 Slot Context) contained inside the Device Context.

--*/
{
    return &DeviceContext64->SlotContext;
}

__forceinline
PENDPOINT_CONTEXT32
UsbDevice_DeviceContext32GetEndpoint(
    __in
        PDEVICE_CONTEXT32   DeviceContext32,
    __in
        ULONG               DeviceContextIndex
    )
/*++

Routine Description:

    Given a pointer to a 32-byte sized Device Context (see 6.2.1 Device
    Context) and an Endpoint handle this routine returns a pointer to
    the 32-byte sized Endpoint Context (see 6.2.3 Endpoint Context) for
    the specified endpoint contained inside the Device Context.

--*/
{
    return &DeviceContext32->EndpointContext[DeviceContextIndex - 1];
}

__forceinline
PENDPOINT_CONTEXT64
UsbDevice_DeviceContext64GetEndpoint(
    __in
        PDEVICE_CONTEXT64   DeviceContext64,
    __in
        ULONG               DeviceContextIndex
    )
/*++

Routine Description:

    Given a pointer to a 64-byte sized Device Context (see 6.2.1 Device
    Context) and an Endpoint handle this routine returns a pointer to
    the 64-byte sized Endpoint Context (see 6.2.3 Endpoint Context) for
    the specified endpoint contained inside the Device Context.

--*/
{
    return &DeviceContext64->EndpointContext[DeviceContextIndex - 1];
}

__forceinline
PSLOT_CONTEXT32
UsbDevice_InputContext32GetSlot(
    __in
        PINPUT_CONTEXT32    InputContext32
    )
/*++

Routine Description:

    Given a pointer to a 32-byte sized Input Context (see 6.2.5 Input
    Context) this routine returns a pointer to the 32-byte sized Slot
    Context (see 6.2.2 Slot Context) contained inside the Input Context.

--*/
{
    return &InputContext32->SlotContext;
}

__forceinline
PSLOT_CONTEXT64
UsbDevice_InputContext64GetSlot(
    __in
        PINPUT_CONTEXT64    InputContext64
    )
/*++

Routine Description:

    Given a pointer to a 64-byte sized Input Context (see 6.2.5 Input
    Context) this routine returns a pointer to the 64-byte sized Slot
    Context (see 6.2.2 Slot Context) contained inside the Input Context.

--*/
{
    return &InputContext64->SlotContext;
}

__forceinline
PENDPOINT_CONTEXT32
UsbDevice_InputContext32GetDefaultEndpoint(
    __in
        PINPUT_CONTEXT32    InputContext32
    )
/*++

Routine Description:

    Given a pointer to a 32-byte sized Input Context (see 6.2.5 Input
    Context) this routine returns a pointer to the 32-byte sized
    Endpoint Context (see 6.2.3 Endpoint Context) for the default
    endpoint 0 contained inside the Input Context.

--*/
{
    return &InputContext32->EndpointContext[0];
}

__forceinline
PENDPOINT_CONTEXT64
UsbDevice_InputContext64GetDefaultEndpoint(
    __in
        PINPUT_CONTEXT64    InputContext64
    )
/*++

Routine Description:

    Given a pointer to a 64-byte sized Input Context (see 6.2.5 Input
    Context) this routine returns a pointer to the 64-byte sized
    Endpoint Context (see 6.2.3 Endpoint Context) for the default
    endpoint 0 contained inside the Input Context.

--*/
{
    return &InputContext64->EndpointContext[0];
}

__forceinline
PENDPOINT_CONTEXT32
UsbDevice_InputContext32GetEndpoint(
    __in
        PINPUT_CONTEXT32    InputContext32,
    __in
        ULONG               DeviceContextIndex
    )
/*++

Routine Description:

    Given a pointer to a 32-byte sized Input Context (see 6.2.5 Input
    Context) and a device context index this routine returns a pointer to
    the 32-byte sized Endpoint Context (see 6.2.3 Endpoint Context) for
    the specified endpoint contained inside the Input Context.

--*/
{
    return &InputContext32->EndpointContext[DeviceContextIndex - 1];
}

__forceinline
PENDPOINT_CONTEXT64
UsbDevice_InputContext64GetEndpoint(
    __in
        PINPUT_CONTEXT64    InputContext64,
    __in
        ULONG               DeviceContextIndex
    )
/*++

Routine Description:

    Given a pointer to a 64-byte sized Input Context (see 6.2.5 Input
    Context) and a device context index this routine returns a pointer to
    the 64-byte sized Endpoint Context (see 6.2.3 Endpoint Context) for
    the specified endpoint contained inside the Input Context.

--*/
{
    return &InputContext64->EndpointContext[DeviceContextIndex - 1];
}

FORCEINLINE
BOOLEAN
UsbDevice_IsBehindTTHub(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    )
{
    return (UsbDeviceData->UsbDeviceInfo.PortPath.TTHubDepth != 0);
}

ULONG
UsbDevice_GetEndpointState(
    __in
        PUSBDEVICE_DATA UsbDeviceData,
    __in
        ULONG           ContextIndex
    );

ULONG64
UsbDevice_GetXhciEndpointDequeuePointer(
    __in
        PUSBDEVICE_DATA UsbDeviceData,
    __in
        ULONG           ContextIndex
    );

VOID
UsbDevice_ReconfigureEndpoint(
    __in
        PUSBDEVICE_DATA                 UsbDeviceData,
    __in
        HANDLE                          Endpoint,
    __in
        PUSBDEVICE_COMPLETION_ROUTINE   CompletionRoutine,
    __in
        PVOID                           CompletionContext
    );

COMMAND_CALLBACK UsbDevice_ConfigureEndpointCompletion;

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_AddEndpointToDeviceEndpointList(
    __in
        PUSBDEVICE_DATA     UsbDeviceData,
    __in
        UCXENDPOINT         UcxEndpoint,
    __in
        PLIST_ENTRY         EndpointListEntry
    );

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_RemoveEndpointFromDeviceEndpointList(
    __in
        PUSBDEVICE_DATA     UsbDeviceData,
    __in
        UCXENDPOINT         UcxEndpoint,
    __in
        PLIST_ENTRY         EndpointListEntry
    );

VOID
UsbDevice_InitializeEndpointContext(
    PUSBDEVICE_DATA         UsbDeviceData,
    HANDLE                  Endpoint,
    PENDPOINT_CONTEXT64     EndpointContext
    );

ULONG
UsbDevice_GetNumContextEntries(
    __in
        PUSBDEVICE_DATA     UsbDeviceData
    );

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
USHORT
UsbDevice_GetVendorId(
    __in
        PUSBDEVICE_DATA     UsbDeviceData
    )
{
    return UsbDeviceData->DeviceDescriptor.idVendor;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
USHORT
UsbDevice_GetProductId(
__in
    PUSBDEVICE_DATA     UsbDeviceData
)
{
    return UsbDeviceData->DeviceDescriptor.idProduct;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
ULONG
UsbDevice_GetPortPathDepth(
__in
    PUSBDEVICE_DATA     UsbDeviceData
)
{
    return UsbDeviceData->UsbDeviceInfo.PortPath.PortPathDepth;
}