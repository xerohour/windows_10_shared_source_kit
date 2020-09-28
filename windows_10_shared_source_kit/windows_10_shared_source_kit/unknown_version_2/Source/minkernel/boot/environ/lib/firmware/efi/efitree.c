/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efitree.c

Abstract:

    Wrappers for EFI TrEE interface.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "bootlibp.h"

// ------------------------------------------------------------------ Structures
typedef struct _EFI_TREE_GET_EVENT_LOG_OUT {
    EFI_PHYSICAL_ADDRESS EventLogLocation;
    EFI_PHYSICAL_ADDRESS EventLogLastEntry;
    BOOLEAN EventLogTruncated;
} EFI_TREE_GET_EVENT_LOG_OUT, *PEFI_TREE_GET_EVENT_LOG_OUT;

/*++

EFI_TREE_PROTOCOL.GetEventLog Output Parameters

    This structure contains a local copy of all the output parameters of the
    GetEventLog function that are singularly mapped to a physical address.

    The fields map 1:1 with EfiTreeGetEventLog wrapper function.

--*/

typedef struct _EFI_TREE_GET_ACTIVE_PCR_BANKS_OUT
{
    UINT32 ActivePcrBanks;
} EFI_TREE_GET_ACTIVE_PCR_BANKS_OUT, *PEFI_TREE_GET_ACTIVE_PCR_BANKS_OUT;

/*++

EFI_TREE_PROTOCOL.GetActivePcrBanks Output Parameters

    This structure contains a local copy of all the output parameters of the
    GetActivePcrBanks function that are singularly mapped to a physical address.

    The fields map 1:1 with EfiTreeGetActivePcrBanks wrapper function.

--*/

// ------------------------------------------------------------------ Functions

NTSTATUS
EfiTreeGetCapability (
    __in EFI_TREE_PROTOCOL *TreeProtocol,
    __out TREE_BOOT_SERVICE_CAPABILITY *ProtocolCapability
    )

/*++

Routine Description:

    This routine is a wrapper for EFI_TREE_PROTOCOL.StatusCheck, which will
    provide the status of the TPM and the TrEE interface capabilities.

Arguments:

    TreeProtocol - Supplies an Interface for the TrEE routines, returned from
        OpenProtocol / LocateHandle.

    ProtocolCapability - Supplies a pointer to a variable that receives
        the TREE_BOOT_SERVICE_CAPABILITY information. This structure
        indicates TrEE compliance and capabilities of the firmware and interface.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_IO_DEVICE_ERROR if the command was unsuccessful.

    STATUS_INVALID_PARAMETER if one or more of the parameters are incorrect.

    STATUS_BUFFER_TOO_SMALL if the capability buffer is too small.

    STATUS_NOT_FOUND if the component is not running.

--*/

{
    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};
    TREE_BOOT_SERVICE_CAPABILITY OutParams;
    PHYSICAL_ADDRESS OutParamsPa;
    TREE_BOOT_SERVICE_CAPABILITY* OutParamsPointer;

    RtlZeroMemory(&OutParams, sizeof(OutParams));
    OutParams.Size = sizeof(TREE_BOOT_SERVICE_CAPABILITY);
    OutParamsPointer = &OutParams;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate the virtual pointer to the TreeProtocol interface and the
    // combined output parameters.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(TreeProtocol, &InterfacePa);
        TreeProtocol = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlMmTranslateVirtualAddress(OutParamsPointer, &OutParamsPa);
        OutParamsPointer = (PVOID)(UINTN)OutParamsPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested TreeProtocol.GetCapability call.
    //

    EfiStatus = TreeProtocol->GetCapability(
                                  TreeProtocol,
                                  OutParamsPointer);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    //
    // Copy parameters that were requested. The physical address is returned
    // instead of a mapped address for the event log as the caller may desire
    // to know what the physical address is.
    //

    *ProtocolCapability = OutParams;

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiTreeGetEventLog(
    __in EFI_TREE_PROTOCOL *TreeProtocol,
    __in TREE_EVENT_LOG_FORMAT EventLogFormat,
    __out_opt PHYSICAL_ADDRESS *EventLogLocation,
    __out_opt PHYSICAL_ADDRESS *EventLogLastEntry,
    __out_opt BOOLEAN *EventLogTruncated
    )

/*++

Routine Description:

    This routine is a wrapper for EFI_TREE_PROTOCOL.StatusCheck, which will
    provide the status of the TPM and the TrEE interface capabilities.

Arguments:

    TreeProtocol - Supplies an Interface for the TrEE routines, returned from
        OpenProtocol / LocateHandle.

    EventLogFormat - Supplies type of the event log requested.

    EventLogLocation - Supplies an optional pointer to a variable that receives
        the physical location of the event log in memory.

    EventLogLastEntry - Supplies an optional pointer to a variable that receives
        the physical location of the first byte of the last event in the event
        log. If the Event Log contains more than one entry, this is a pointer to
        the address of the start of the last entry in the event log in memory.

    EventLogTruncated - Supplies an optional pointer to a variable that receives
        indication if the Event Log is missing at least one entry because
        an event would have exceeded the area allocated for events.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_IO_DEVICE_ERROR if the command was unsuccessful.

    STATUS_INVALID_PARAMETER if one or more of the parameters are incorrect.

    STATUS_BUFFER_TOO_SMALL if the capability buffer is too small.

    STATUS_NOT_FOUND if the component is not running.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};
    EFI_TREE_GET_EVENT_LOG_OUT OutParams;
    PHYSICAL_ADDRESS OutParamsPa;
    PEFI_TREE_GET_EVENT_LOG_OUT OutParamsPointer;

    RtlZeroMemory(&OutParams, sizeof(OutParams));
    OutParamsPointer = &OutParams;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate the virtual pointer to the TreeProtocol interface and the
    // combined output parameters.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(TreeProtocol, &InterfacePa);
        TreeProtocol = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlMmTranslateVirtualAddress(OutParamsPointer, &OutParamsPa);
        OutParamsPointer = (PVOID)(UINTN)OutParamsPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested TreeProtocol.GetEventLog call.
    //

    EfiStatus = TreeProtocol->GetEventLog(TreeProtocol,
                                          EventLogFormat,
                                          &OutParamsPointer->EventLogLocation,
                                          &OutParamsPointer->EventLogLastEntry,
                                          &OutParamsPointer->EventLogTruncated);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    //
    // Copy parameters that were requested. The physical address is returned
    // instead of a mapped address for the event log as the caller may desire
    // to know what the physical address is.
    //

    if (EventLogLocation != NULL) {
        EventLogLocation->QuadPart =
            (ULONGLONG)OutParamsPointer->EventLogLocation;
    }

    if (EventLogLastEntry != NULL) {
        EventLogLastEntry->QuadPart =
            (ULONGLONG)OutParamsPointer->EventLogLastEntry;
    }

    if (EventLogTruncated != NULL) {
        *EventLogTruncated = OutParamsPointer->EventLogTruncated;
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiTreeHashLogExtendEvent (
    __in EFI_TREE_PROTOCOL *TreeProtocol,
    __in UINT64 Flags,
    __in PVOID DataToHash,
    __in UINT64 DataToHashLen,
    __in TrEE_EVENT *Event
    )

/*++

Routine Description:

    This routine is a wrapper for EFI_TREE_PROTOCOL.HashLogExtendEvent, which
    combines a sequence of common operations when measuring code and events.
    The data pointed to by HashData is hashed; it is written to a PCR (extend)
    and a related entry is written to the log to indicate what was measured.

Arguments:

    TreeProtocol - Supplies an Interface for the TrEE routines, returned from
        OpenProtocol / LocateHandle.

    Flags - Supplies a bitmask providing additional information.

    DataToHash - Supplies a pointer the data buffer to be hashed.

    DataToHashLen - Supplies the length in bytes of the buffer referenced by
        DataToHash.

    Event - Supplies a pointer to data buffer containing information about
        the event.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_IO_DEVICE_ERROR if the command was unsuccessful.

    STATUS_INVALID_PARAMETER if one or more of the parameters are incorrect.

    STATUS_NOT_FOUND if the component is not running.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS DataToHashPa = {0};
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS EventPa = {0};
    PHYSICAL_ADDRESS InterfacePa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    //
    // Translate the virtual pointer to the TreeProtocol address, the data block
    // to be hashed, the log data and the event number output parameter. The
    // block of data to be hashed must also be mapped as well as the log data
    // and the pointer to the location that will contain the resulting event
    // number.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(TreeProtocol, &InterfacePa);
        TreeProtocol = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlMmTranslateVirtualAddress(DataToHash, &DataToHashPa);
        DataToHash = (PVOID)(UINTN)DataToHashPa.QuadPart;
        BlMmTranslateVirtualAddress(Event, &EventPa);
        Event = (TrEE_EVENT*)(UINTN)EventPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested TreeProtocol.HashLogExtendEvent call.
    //

    EfiStatus = TreeProtocol->HashLogExtendEvent(
                                  TreeProtocol,
                                  Flags,
                                  (EFI_PHYSICAL_ADDRESS)DataToHash,
                                  DataToHashLen,
                                  Event);
                              
    //
    // Restore original execution context. None of the pointers that were
    // remapped above are valid after this point.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiTreeSubmitCommand (
    __in EFI_TREE_PROTOCOL *TreeProtocol,
    __in UINT32 InputParamterBlockSize,
    __in PVOID InputParamterBlock,
    __in UINT32 OutputParameterBlockSize,
    __in PVOID OutputParameterBlock
    )

/*++

Routine Description:

    This routine is a wrapper for EFI_TREE_PROTOCOL.PassThroughToTpm, which will
    allow the caller to package up a TPM request, that the firmware will send
    on the callers behalf. Although the contents of input and output are
    well defined by the TrEE; the communication method chosen by the firmware
    will depend upon the version of the TPM and what state the TPM is in.

Arguments:

    TreeProtocol - Supplies an Interface for the TrEE routines, returned from
        OpenProtocol / LocateHandle.

    InputParamterBlockSize - Supplies the size of the data to send to the TPM.

    InputParamterBlock - Supplies a pointer to the data to send to the TPM.
        This must be a well formed request packet.

    OutputParamterBlockSize - Supplies the size of the buffer to receive the
        TPM reply. Note that if this buffer is too large, the command may fail
        due to firmware implementation.

    OutputParamterBlock - Supplies a pointer to the buffer to receive the
        TPM reply. The actual length of the reply is encoded in the reply
        buffer itself.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_IO_DEVICE_ERROR if the command was unsuccessful.

    STATUS_INVALID_PARAMETER if one or more of the parameters are incorrect.

    STATUS_BUFFER_TOO_SMALL if the output buffer is too small.

    STATUS_NOT_FOUND if the component is not running.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InputPa = {0};
    PHYSICAL_ADDRESS InterfacePa = {0};
    PHYSICAL_ADDRESS OutputPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate the virtual pointer to the TreeProtocol address and the TPM
    // input/output buffers.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(TreeProtocol, &InterfacePa);
        TreeProtocol = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlMmTranslateVirtualAddress(InputParamterBlock, &InputPa);
        InputParamterBlock = (PVOID)(UINTN)InputPa.QuadPart;
        BlMmTranslateVirtualAddress(OutputParameterBlock, &OutputPa);
        OutputParameterBlock = (PVOID)(UINTN)OutputPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested TreeProtocol.SubmitCommand call.
    //

    EfiStatus = TreeProtocol->SubmitCommand(TreeProtocol,
                                            InputParamterBlockSize,
                                            InputParamterBlock,
                                            OutputParameterBlockSize,
                                            OutputParameterBlock);

    //
    // Restore original execution context. The TpmInputParameterBlock and
    // TpmOutputParameterBlock pointers are invalid after this call.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiTreeSetActivePcrBanks (
    __in EFI_TREE_PROTOCOL *TreeProtocol,
    __in UINT32 ActivePcrBanks
    )

/*++

Routine Description:

    This routine is a wrapper for EFI_TREE_PROTOCOL.SetActivePcrBanks, which will
    request a change of the active PCR banks in the TPM. The supplied argument is a
    bitmap of PCR banks that should be activated. 

Arguments:

    TreeProtocol - Supplies an Interface for the TrEE routines, returned from
        OpenProtocol / LocateHandle.

    ActivePcrBanks - Supplies the bitmask of requested active PCR banks.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_IO_DEVICE_ERROR if the command was unsuccessful.

    STATUS_INVALID_PARAMETER if one or more of the parameters are incorrect.

    STATUS_BUFFER_TOO_SMALL if the output buffer is too small.

    STATUS_NOT_FOUND if the component is not running.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the caller.
    // Translate the virtual pointer to the TreeProtocol address and the TPM
    // input/output buffers.
    //
    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(TreeProtocol, &InterfacePa);
        TreeProtocol = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested TreeProtocol.SetActivePcrBanks call.
    //
    EfiStatus = TreeProtocol->SetActivePcrBanks(TreeProtocol,
                                                ActivePcrBanks);

    //
    // Restore original execution context.
    //
    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiTreeGetActivePcrBanks (
    __in EFI_TREE_PROTOCOL *TreeProtocol,
    __in UINT32 *ActivePcrBanks
    )

/*++

Routine Description:

    This routine is a wrapper for EFI_TREE_PROTOCOL.GetActivePcrBanks, which will
    retrieve a bitmap of currently active PCR banks. The provided argument will 
    receive the bitmap if the call is successful.

Arguments:

    TreeProtocol - Supplies an Interface for the TrEE routines, returned from
        OpenProtocol / LocateHandle.

    ActivePcrBanks - On success, will receive the bitmap with active PCR banks.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_IO_DEVICE_ERROR if the command was unsuccessful.

    STATUS_INVALID_PARAMETER if one or more of the parameters are incorrect.

    STATUS_BUFFER_TOO_SMALL if the output buffer is too small.

    STATUS_NOT_FOUND if the component is not running.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS InterfacePa = {0};
    EFI_TREE_GET_ACTIVE_PCR_BANKS_OUT OutParams;
    PHYSICAL_ADDRESS OutParamsPa;
    PEFI_TREE_GET_ACTIVE_PCR_BANKS_OUT OutParamsPointer;

    RtlZeroMemory(&OutParams, sizeof(OutParams));
    OutParamsPointer = &OutParams;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the caller.
    // Translate the virtual pointer to the TreeProtocol address and the TPM
    // input/output buffers.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(TreeProtocol, &InterfacePa);
        TreeProtocol = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
        BlMmTranslateVirtualAddress(OutParamsPointer, &OutParamsPa);
        OutParamsPointer = (PVOID)(UINTN)OutParamsPa.QuadPart;
    }

    //
    // Make the requested TreeProtocol.GetActivePcrBanks call.
    //

    EfiStatus = TreeProtocol->GetActivePcrBanks(TreeProtocol,
                                                &OutParamsPointer->ActivePcrBanks);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    //
    // Copy parameters that were requested. The physical address is returned
    // instead of a mapped address for the event log as the caller may desire
    // to know what the physical address is.
    //

    if (ActivePcrBanks != NULL) {
        *ActivePcrBanks = OutParamsPointer->ActivePcrBanks;
    }

    return EfiGetNtStatusCode(EfiStatus);
}

