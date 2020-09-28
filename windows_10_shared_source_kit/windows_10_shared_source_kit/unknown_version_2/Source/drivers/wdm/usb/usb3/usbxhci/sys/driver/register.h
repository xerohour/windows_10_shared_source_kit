/*++
Copyright (c) Microsoft Corporation

Module Name:

    register.h

Abstract:

    USB XHCI Driver

--*/

#pragma once

#define AFFINITY_MASK(n)                        (((ULONG_PTR)1) << (n))

#define DEFAULT_HC_BIOS_WAIT_COUNT              20
#define DEFAULT_HC_BIOS_WAIT_TIME_MS            100     // 100ms
#define DEFAULT_HC_BIOS_WAIT_TIME_US            100000  // 100ms

#define DEFAULT_CNR_WAIT_COUNT                  100
#define DEFAULT_CNR_WAIT_TIME                   100     // 100ms

#define DEFAULT_RESET_LOOP_WAIT_COUNT           100
#define DEFAULT_RESET_LOOP_WAIT_TIME_MAX        16      // 16ms
#define HW_COMPLIANCE_XHC_RESET_WARN_MS         50

#define DEFAULT_STOP_WAIT_COUNT                 16
#define DEFAULT_STOP_WAIT_TIME_MS               1       // 1ms

#define DEFAULT_STATE_SAVE_WAIT_COUNT           20
#define DEFAULT_STATE_SAVE_WAIT_TIME            200     // 200microseconds

#define DEFAULT_STATE_RESTORE_WAIT_COUNT        20
#define DEFAULT_STATE_RESTORE_WAIT_TIME         200     // 200microseconds

#define REGISTER_CONTEXTSIZE32                  0
#define REGISTER_CONTEXTSIZE64                  1

//
// Used to get the address of the byte containing the CycleBit within
// a TRB. Cannot use FIELD_OFFSET since CycleBit is a bitfield.
//
#define CYCLEBIT_BYTE_OFFSET_IN_TRB             12

typedef struct _REGISTER_DATA {

    WDFOBJECT                   WdfRegister;

    HANDLE                      ControllerHandle;

    BOOLEAN                     RegistersMapped;
    ULONG                       RegisterMapLength;
    PCAPABILITY_REGISTERS       CapabilityRegisters;
    POPERATIONAL_REGISTERS      OperationalRegisters;
    PRUNTIME_REGISTERS          RuntimeRegisters;
    PDOORBELL_REGISTER          DoorbellRegisters;
    PEXTENDED_CAPABILITY        ExtendedCapabilityRegisters;
    PUSBLEGSUPCAPABILITY        UsbLegSupCapability;
    PDEBUG_CAPABILITY           DebugCapability;

    //
    // Values from the xHCI Host Controller Capability Registers.  Some
    // of these values are translated from the raw xHCI controller
    // values.
    //
    // 5.3.3 Structural Parameters 1 (HCSPARAMS1)
    //
    ULONG                       SupportedNumberOfDeviceSlots;
    ULONG                       SupportedNumberOfInterrupters;
    ULONG                       SupportedNumberOfPorts;
    //
    // 5.3.4 Structural Parameters 2 (HCSPARAMS2)
    //
    ULONG                       MaxEventRingSegments;
    ULONG                       MaxScratchpadBuffers;
    //
    // 5.3.5 Structural Parameters 3 (HCSPARAMS3)
    //
    USHORT                      U1DeviceExitLatency;
    USHORT                      U2DeviceExitLatency;
    //
    // 5.3.6 Capability Parameters (HCCPARAMS1,HCCPARAMS2)
    //
    HCCPARAMS                   CachedHccParams;
    HCCPARAMS2                  CachedHccParams2;

    ULONG                       SupportedNumberOfStreams;

    //
    // Only used when EnableCHTNonArchitecturalRegisterSaveRestore workaround
    // is applied.
    //
    ULONG                       CHTSavedRegisterBits;

} REGISTER_DATA, *PREGISTER_DATA;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(REGISTER_DATA, GetRegisterData)

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
Register_Create(
    __in
        WDFDEVICE   WdfDevice,
    __in
        HANDLE      ControllerHandle,
    __inout
        PHANDLE     ObjectHandle
    );

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
Register_PrepareHardware(
    __in
        PREGISTER_DATA  RegisterData,
    __in
        WDFCMRESLIST    WdfResourcesTranslated
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
Register_ReleaseHardware(
    __in
        PREGISTER_DATA  RegisterData
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
Register_D0EntryPreRestoreState(
    __in
        PREGISTER_DATA RegisterData
    );

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
Register_D0Entry(
    __in
        PREGISTER_DATA          RegisterData,
    __in
        WDF_POWER_DEVICE_STATE  WdfPreviousState
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
Register_D0ExitPostSaveSate(
    __in
        PREGISTER_DATA RegisterData
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
Register_SetClearSSICPortUnused(
    __in
        PREGISTER_DATA          RegisterData,
    __in
        BOOLEAN                 OnPowerDown
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
Register_SaveRestoreCHTNonArchitecturalRegisters(
    __in
        PREGISTER_DATA  RegisterData,
    __in
        BOOLEAN         Save
    );

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
Register_ParseCapabilityRegister(
    __in
        PREGISTER_DATA  RegisterData
    );

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
Register_BiosHandoff(
    __in
        PREGISTER_DATA  RegisterData
    );

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
Register_ControllerStop(
    __in
        PREGISTER_DATA  RegisterData
    );

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
Register_ControllerReset(
    __in
        PREGISTER_DATA  RegisterData,
    __in
        BOOLEAN         ResetWhenRunning
    );

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
Register_WaitForControllerReady(
    __in
        PREGISTER_DATA  RegisterData
    );

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
PEXTENDED_CAPABILITY
Register_FindFirstExtendedCapability(
    __in
        PREGISTER_DATA          RegisterData,
    __in
        UCHAR                   CapabilityID
    );

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
PEXTENDED_CAPABILITY
Register_FindNextExtendedCapability(
    __in
        PREGISTER_DATA          RegisterData,
    __in
        PEXTENDED_CAPABILITY    ExtendedCapabilityPointer,
    __in
        UCHAR                   CapabilityID
    );

_IRQL_requires_(PASSIVE_LEVEL)
ULONG
Register_GetExtendedCapabilityTotalSize(
    __in
        PREGISTER_DATA          RegisterData
    );

_IRQL_requires_(PASSIVE_LEVEL)
VOID
Register_GetAllExtendedCapability(
    __in
        PREGISTER_DATA          RegisterData,
    __in
        PEXTENDED_CAPABILITY    ExtendedCapabilityBuffer
    );

FORCEINLINE
UCHAR
Register_ReadUchar(
    __in
        PUCHAR  Register
    )
{
    return READ_REGISTER_UCHAR(Register);
}

FORCEINLINE
VOID
Register_WriteUchar(
    __in
        PUCHAR  Register,
    __in
        UCHAR   Value
    )
{
    WRITE_REGISTER_UCHAR(Register, Value);
    return;
}

FORCEINLINE
VOID
Register_ToggleTrbCycleBit(
    __in
        PTRB    Trb
    )
/*

Routine Description:

    This routine toggles the CycleBit in the supplied Trb. It utilizes the
    WRITE_REGISTER_UCHAR macro to update the CycleBit to introduce a barrier
    before the code that precedes the CycleBit update.

*/
{
    PUCHAR cycleBitByteAddress;
    UCHAR  cycleBitByteValue;

    cycleBitByteAddress = (PUCHAR)Trb + CYCLEBIT_BYTE_OFFSET_IN_TRB;

    cycleBitByteValue = *cycleBitByteAddress;
    cycleBitByteValue ^= 0x01;

    Register_WriteUchar(cycleBitByteAddress, cycleBitByteValue);

    return;
}

FORCEINLINE
ULONG
Register_ReadUlong(
    __in
        PULONG  Register
    )
{
    return READ_REGISTER_ULONG(Register);
}

FORCEINLINE
VOID
Register_WriteUlong(
    __in
        PULONG  Register,
    __in
        ULONG   Value
    )
{
    WRITE_REGISTER_ULONG(Register, Value);
    return;
}

FORCEINLINE
ULONG64
Register_ReadUlong64(
    __in
        PULONG64    Register,
    __in
        BOOLEAN     Use32BitAccess
    )
{
    ULARGE_INTEGER  tempValue;

#if defined (_WIN64)
    {
        if (Use32BitAccess == FALSE) {

            tempValue.QuadPart = READ_REGISTER_ULONG64(Register);

        } else {

            PULONG  tempRegister = (PULONG)Register;

            tempValue.LowPart  = READ_REGISTER_ULONG(tempRegister);
            tempValue.HighPart = READ_REGISTER_ULONG(tempRegister+1);
        }
    }
#else
    {
        PULONG  tempRegister = (PULONG)Register;

        UNREFERENCED_PARAMETER(Use32BitAccess);

        tempValue.LowPart  = READ_REGISTER_ULONG(tempRegister);
        tempValue.HighPart = READ_REGISTER_ULONG(tempRegister+1);
    }
#endif

    return tempValue.QuadPart;
}

FORCEINLINE
VOID
Register_WriteUlong64(
    __in
        PULONG64    Register,
    __in
        ULONG64     Value,
    __in
        BOOLEAN     Use32BitAccess
    )
{
#if defined (_WIN64)
    {
        if (Use32BitAccess == FALSE) {

            WRITE_REGISTER_ULONG64(Register, Value);

        } else {

            PULONG          tempRegister = (PULONG)Register;
            ULARGE_INTEGER  tempValue;

            tempValue.QuadPart = Value;

            WRITE_REGISTER_ULONG(tempRegister, tempValue.LowPart);
            WRITE_REGISTER_ULONG(tempRegister+1, tempValue.HighPart);
        }
    }
#else
    {
        PULONG          tempRegister = (PULONG)Register;
        ULARGE_INTEGER  tempValue;

        UNREFERENCED_PARAMETER(Use32BitAccess);

        tempValue.QuadPart = Value;

        WRITE_REGISTER_ULONG(tempRegister, tempValue.LowPart);
        WRITE_REGISTER_ULONG(tempRegister+1, tempValue.HighPart);
    }
#endif
    return;
}

_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
VOID
Register_ReadBufferUlong(
    __in_ecount(Count) __drv_nonConstant PULONG Register,
    __out_ecount_full(Count) PULONG Buffer,
    __in ULONG Count
    )
{
    READ_REGISTER_BUFFER_ULONG(Register, Buffer, Count);
    return;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
FORCEINLINE
PCAPABILITY_REGISTERS
Register_GetCapabilityRegisters(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->CapabilityRegisters;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
FORCEINLINE
POPERATIONAL_REGISTERS
Register_GetOperationalRegisters(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->OperationalRegisters;
}

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
PCRCR
Register_GetCommandRingControlRegister(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return &RegisterData->OperationalRegisters->CommandRingControl;
}

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
FORCEINLINE
PDOORBELL_REGISTER
Register_GetDoorbellRegister(
    __in
        PREGISTER_DATA  RegisterData,
    __in
        UCHAR           SlotId
    )
{
    return &RegisterData->DoorbellRegisters[SlotId];
}

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
ULONG
Register_GetContextSize(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->CachedHccParams.ContextSize;
}

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
ULONG
Register_GetStoppedShortPacket(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->CachedHccParams.StoppedShortPacketCapability;
}

_Must_inspect_result_
_IRQL_requires_max_(PASSIVE_LEVEL)
FORCEINLINE
ULONG
Register_GetConfigureEndpointCommandMaxExitLatency(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->CachedHccParams2.ConfigureEndpointCommandMaxExitLatencyCapability;
}

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
FORCEINLINE
ULONG
Register_GetConfigurationInformation(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->CachedHccParams2.ConfigurationInformationCapability;
}

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
FORCEINLINE
ULONG
Register_GetLargeESITPayload(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->CachedHccParams2.LargeESITPayloadCapability;
}

_Must_inspect_result_
_IRQL_requires_max_(DISPATCH_LEVEL)
FORCEINLINE
ULONG
Register_GetExtendedTBC(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->CachedHccParams2.ExtendedTBCCapability;
}

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
ULONG
Register_GetNumberOfPorts(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->SupportedNumberOfPorts;
}

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
ULONG
Register_GetPortPowerControl(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->CachedHccParams.PortPowerControl;
}

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
ULONG
Register_GetContiguousFrameIDCapability(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    if (Controller_GetDeviceFlags(RegisterData->ControllerHandle).IgnoreContiguousFrameIDCapability) {
        
        return 0;
        
    } else {
        
        return RegisterData->CachedHccParams.ContiguousFrameIDCapability;
    }
}

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
ULONG
Register_GetSupportedNumberOfStreams(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->SupportedNumberOfStreams;
}

__drv_maxIRQL(DISPATCH_LEVEL)
FORCEINLINE
PPORT_REGISTER_SET
Register_GetPortRegisterSet(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return (PPORT_REGISTER_SET)(((PUCHAR)RegisterData->OperationalRegisters) + 0x400);
}

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
ULONG
Register_GetSupportedEventRingSegmentCount(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->MaxEventRingSegments;
}

FORCEINLINE
ULONG
Register_Get11BitFrameNumber(
    __in
        PREGISTER_DATA  RegisterData,
    __in
        ULONG           Increment
    )
/*
Routine Description:

This routine returns the 11 bit frame number (not micro-frame number) value read from the hardware
register.

Increment : A value to be added to the Microframe Index before returning the frame number.

*/
{
    ULONG   newFrameNumber;

    newFrameNumber = Register_ReadUlong((PULONG)&RegisterData->RuntimeRegisters->MicroFrameIndex);

    newFrameNumber += Increment;
    
    newFrameNumber >>= MFINDEX_UFRAME_SHIFT;
    newFrameNumber &= FRAME_INDEX_MASK;

    return newFrameNumber;
}

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
PINTERRUPTER_REGISTER_SET
Register_GetInterrupterRegisterSet(
    __in
        PREGISTER_DATA  RegisterData,
    __in
        ULONG           InterrupterNumber
    )
{
    return &RegisterData->RuntimeRegisters->InterrupterRegisters[InterrupterNumber];
}

_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
ULONG
Register_GetSupportedNumberOfDeviceSlots(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->SupportedNumberOfDeviceSlots;
}

_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
ULONG
Register_GetSupportedNumberOfInterrupters(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->SupportedNumberOfInterrupters;
}

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
USHORT
Register_GetU1DeviceExitLatency(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->U1DeviceExitLatency;
}

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
USHORT
Register_GetU2DeviceExitLatency(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->U2DeviceExitLatency;
}

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
ULONG
Register_GetMaxScratchpadBuffers(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->MaxScratchpadBuffers;
}

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
ULONG
Register_GetPageSize(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    UNREFERENCED_PARAMETER(RegisterData);

    //
    // Currently only support 4k page size. This is validate when parsing the xHCI registers.
    //
    return 4096;
}

_Must_inspect_result_
_IRQL_requires_(PASSIVE_LEVEL)
FORCEINLINE
ULONG
Register_GetAddressingCapability(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    HCCPARAMS   hccParams;

    hccParams.AsUlong32 =
        Register_ReadUlong(&RegisterData->CapabilityRegisters->CapabilityParameters.AsUlong32);

    return hccParams.AddressingCapability;
}


_IRQL_requires_max_(DISPATCH_LEVEL)
FORCEINLINE
BOOLEAN
Register_AreRegistersMapped(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->RegistersMapped;
}

_IRQL_requires_max_(DISPATCH_LEVEL)
FORCEINLINE
PUSBLEGSUPCAPABILITY
Register_GetLegacySupportCapability(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    return RegisterData->UsbLegSupCapability;
}

FORCEINLINE
ULONG
Register_ReadDebugPortNumber(
    __in
        PREGISTER_DATA  RegisterData
    )
{
    DC_STATUS           dcStatus;
    PDEBUG_CAPABILITY   debugCapability;

    debugCapability = RegisterData->DebugCapability;

    dcStatus.AsUlong32 = Register_ReadUlong(&debugCapability->Status.AsUlong32);

    return dcStatus.DebugPortNumber;
}
