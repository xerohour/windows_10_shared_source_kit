/*++
Copyright (c) Microsoft Corporation

Module Name:

    crashdumpregister.h

Abstract:

    USB XHCI Crashdump write module.

--*/

#define CRASHDUMP_CONTROLLER_STOP_WAIT_COUNT    80      // 16ms
#define CRASHDUMP_CONTROLLER_STOP_WAIT_TIME     200     // 200us

#define CRASHDUMP_CONTROLLER_RESET_WAIT_COUNT   5000    // 1s
#define CRASHDUMP_CONTROLLER_RESET_WAIT_TIME    200     // 200us

#define CRASHDUMP_PORT_RESET_WAIT_COUNT         1000    // 500ms
#define CRASHDUMP_PORT_RESET_WAIT_TIME          500     // 500us

#define CRASHDUMP_PORT_LINK_STATE_WAIT_COUNT    20000   // 4s
#define CRASHDUMP_PORT_LINK_STATE_WAIT_TIME     200     // 200us

typedef struct _CRASHDUMP_REGISTER_DATA {

    HANDLE                      RegisterHandle;
    KSE_XHCI_CONTROLLER_FLAGS   DeviceFlags;
    ULONG                       MaximumDeviceSlots;
    ULONG                       MaxScratchpadBuffers;
    CRASHDUMP_BUFFER_DATA       ScratchpadBufferArray;
    LIST_ENTRY                  ScatchpadBufferDatas;

} CRASHDUMP_REGISTER_DATA, *PCRASHDUMP_REGISTER_DATA;

//
// Public functions.
//
__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Crashdump_Register_Initialize(
    __inout
        PCRASHDUMP_REGISTER_DATA    RegisterData,
    __in
        HANDLE                      RegisterHandle,
    __in
        HANDLE                      ControllerHandle
    );

VOID
Crashdump_Register_PrepareForHibernate(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Register_VerifyRegisterState(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Register_StopController(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    );

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Register_StartController(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Register_ResetController(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Crashdump_Register_LogRHPortInfo(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData,
    __in
        ULONG                       PortNumber
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Register_ResetPort(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData,
    __in
        ULONG                       PortNumber,
    __in
        CRASHDUMP_PORT_TYPE         PortType
    );

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Crashdump_Register_UpdateDeviceSlotInfo(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData,
    __in
        ULONG                       NumberOfDeviceSlots,
    __in
        CRASHDUMP_BUFFER_DATA       DeviceContextBase
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Register_SuspendPort(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData,
    __in
        ULONG                       PortNumber
    );

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_Register_SaveStateForHybridSleep(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    );

__checkReturn
NTSTATUS
Crashdump_Register_BiosHandoff(
    __in
    PCRASHDUMP_REGISTER_DATA    RegisterData
    );

FORCEINLINE
PINTERRUPTER_REGISTER_SET
Crashdump_Register_GetPrimaryInterrupterRegister(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    )
{
    return Register_GetInterrupterRegisterSet(RegisterData->RegisterHandle, 0);
}

FORCEINLINE
KSE_XHCI_CONTROLLER_FLAGS
Crashdump_Register_GetDeviceFlags(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    )
{
    return RegisterData->DeviceFlags;
}

FORCEINLINE
PCRCR
Crashdump_Register_GetCommandRingControlRegister(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    )
{
    return Register_GetCommandRingControlRegister(RegisterData->RegisterHandle);
}

FORCEINLINE
PDOORBELL_REGISTER
Crashdump_Register_GetDoorbellRegister(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData,
    __in
        UCHAR                       SlotId
    )
{
    return Register_GetDoorbellRegister(RegisterData->RegisterHandle, SlotId);
}

FORCEINLINE
ULONG
Crashdump_Register_GetContextSize(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    )
{
    return Register_GetContextSize(RegisterData->RegisterHandle);
}

FORCEINLINE
ULONG
Crashdump_Register_GetConfigurationInformation(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    )
{
    return Register_GetConfigurationInformation(RegisterData->RegisterHandle);
}

FORCEINLINE
HCLENGTHVERSION
Crashdump_Register_GetVersionInfo(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    )
{
    HCLENGTHVERSION         lengthVersion;
    PCAPABILITY_REGISTERS   capabilityRegisters;

    capabilityRegisters = Register_GetCapabilityRegisters(RegisterData->RegisterHandle);

    lengthVersion.AsUlong32 =
        Register_ReadUlong(&capabilityRegisters->LengthVersion.AsUlong32);

    return lengthVersion;
}

FORCEINLINE
ULONG
Crashdump_Register_GetMaxScratchpadBuffers(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    )
{
    return RegisterData->MaxScratchpadBuffers;
}

FORCEINLINE
ULONG
Crashdump_Register_GetSupportedNumberOfDeviceSlots(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    )
{
    return RegisterData->MaximumDeviceSlots;
}

FORCEINLINE
VOID
Crashdump_Register_GetPortSC(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData,
    __in
        ULONG                       PortNumber,
    __out
        PPORTSC                     PortSC
    )
{
    PULONG              portSCRegister;
    PPORT_REGISTER_SET  portRegisterSet;

    portRegisterSet = Register_GetPortRegisterSet(RegisterData->RegisterHandle);
    portSCRegister = &portRegisterSet[PortNumber-1].PortStatusAndControl.AsUlong32;
    PortSC->AsUlong32 = Register_ReadUlong(portSCRegister);
}

FORCEINLINE
VOID
Crashdump_Register_DisableWrapEvent(
    __in
        PCRASHDUMP_REGISTER_DATA    RegisterData
    )
{
    USBCMD                  usbCommand;
    POPERATIONAL_REGISTERS  operationalRegisters;

    operationalRegisters = Register_GetOperationalRegisters(RegisterData->RegisterHandle);

    usbCommand.AsUlong32 = Register_ReadUlong(&operationalRegisters->UsbCommand.AsUlong32);
    usbCommand.EnableWrapEvent = 0;
    Register_WriteUlong(&operationalRegisters->UsbCommand.AsUlong32, usbCommand.AsUlong32);

    // 
    // 16ms is the delay for halt
    //
    KeStallExecutionProcessor(CRASHDUMP_CONTROLLER_STOP_WAIT_TIME*CRASHDUMP_CONTROLLER_STOP_WAIT_COUNT);
}

