/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efitcg.c

Abstract:

    Wrappers for EFI Trusted Computing Group interface.

    The TCG routines provide the ability to query TPM status; perform simple
    hashing; Log events; and pass-through of TPM commands to the TPM.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "bootlibp.h"

// ----------------------------------------------------------------- Structures

typedef struct _EFI_TCG_STATUSCHECK_OUT {
    TCG_EFI_BOOT_SERVICE_CAPABILITY ProtocolCapability;
    UINT32 TCGFeatureFlags;
    EFI_PHYSICAL_ADDRESS EventLogLocation;
    EFI_PHYSICAL_ADDRESS LastEvent;
} EFI_TCG_STATUSCHECK_OUT, *PEFI_TCG_STATUSCHECK_OUT;

/*++

EFI_TCG_PROTOCOL.StatusCheck Output Parameters

    This structure contains a local copy of all the output parameters of the
    StatusCheck function that are singularly mapped to a physical address.

    The fields map 1:1 with EfiTcgStatusCheck wrapper function.

--*/

typedef struct _EFI_TCG_HASH_LOG_EXTEND_EVENT_OUT {
    UINT32 EventNumber;
    EFI_PHYSICAL_ADDRESS EventLogLastEntry;
} EFI_TCG_HASH_LOG_EXTEND_EVENT_OUT, *PEFI_TCG_HASH_LOG_EXTEND_EVENT_OUT;

/*++

EFI_TCG_PROTOCOL.StatusCheck Output Parameters

    This structure contains a local copy of all the output parameters of the
    HashLogExtendEvent function that are singularly mapped to a physical
    address.

    The fields map 1:1 with EfiTcgHashLogExtendEvent wrapper function.

--*/

// ------------------------------------------------------------------ Functions

NTSTATUS
EfiTcgStatusCheck (
    __in EFI_TCG_PROTOCOL *TcgProtocol,
    __out TCG_EFI_BOOT_SERVICE_CAPABILITY *ProtocolCapability,
    __out UINT32 *TCGFeatureFlags,
    __out_opt PHYSICAL_ADDRESS *EventLogLocation,
    __out_opt PHYSICAL_ADDRESS *LastEvent
    )

/*++

Routine Description:

    This routine is a wrapper for EFI_TCG_PROTOCOL.StatusCheck, which will
    provide the status of the TPM and the TCG interface capabilities.

Arguments:

    TcgProtocol - Supplies an Interface for the TCG routines, returned from
        OpenProtocol / LocateHandle.

    ProtocolCapability - Supplies a pointer to a variable that receives
        the TCG_EFI_BOOT_SERVICE_CAPABILITY information. This structure
        indicates TCG compliance and capabilities of the firmware and interface.

    TCGFeatureFlags - Supplies a pointer to a variable that receives
        additional TCG features are enabled by the firmware. Currently no
        feature flags are defined and this value will be 0. Specification
        updates describing these will be at
        http://www.trustedcomputinggroup.org.

    EventLogLocation - Supplies an optional pointer to a variable that receives
        the physical location of the event log in memory.

    LastEvent - Supplies an optional pointer to a variable that receives the
        physical location of the first byte of the last event in the event log.
        This value will be zero if the event log is empty. This value will be
        exactly equal to EventLogLocation if the event log has exactly one
        entry.

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
    EFI_TCG_STATUSCHECK_OUT OutParams;
    PHYSICAL_ADDRESS OutParamsPa;
    PEFI_TCG_STATUSCHECK_OUT OutParamsPointer;

    RtlZeroMemory(&OutParams, sizeof(OutParams));
    OutParams.ProtocolCapability.Size = sizeof(OutParams.ProtocolCapability);
    OutParamsPointer = &OutParams;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate the virtual pointer to the TcgProtocol interface and the
    // combined output parameters.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(TcgProtocol, &InterfacePa);
        TcgProtocol = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlMmTranslateVirtualAddress(OutParamsPointer, &OutParamsPa);
        OutParamsPointer = (PVOID)(UINTN)OutParamsPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested TcgProtocol.StatusCheck call.
    //

    EfiStatus = TcgProtocol->StatusCheck(TcgProtocol,
                                         &OutParamsPointer->ProtocolCapability,
                                         &OutParamsPointer->TCGFeatureFlags,
                                         &OutParamsPointer->EventLogLocation,
                                         &OutParamsPointer->LastEvent);

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

    *ProtocolCapability = OutParams.ProtocolCapability;
    *TCGFeatureFlags = OutParams.TCGFeatureFlags;

    //
    // Optional parameters.
    //

    if (EventLogLocation != NULL) {
        EventLogLocation->QuadPart =
            (ULONGLONG)OutParamsPointer->EventLogLocation;

    }

    if (LastEvent != NULL) {
        LastEvent->QuadPart = (ULONGLONG)OutParamsPointer->LastEvent;
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiTcgPassThroughToTpm (
    __in EFI_TCG_PROTOCOL *TcgProtocol,
    __in UINT32 TpmInputParamterBlockSize,
    __in PVOID TpmInputParamterBlock,
    __in UINT32 TpmOutputParameterBlockSize,
    __in PVOID TpmOutputParameterBlock
    )

/*++

Routine Description:

    This routine is a wrapper for EFI_TCG_PROTOCOL.PassThroughToTpm, which will
    allow the caller to package up a TPM request, that the firmware will send
    on the callers behalf. Although the contents of input and output are
    well defined by the TCG; the communication method chosen by the firmware
    will depend upon the version of the TPM and what state the TPM is in.

Arguments:

    TcgProtocol - Supplies an Interface for the TCG routines, returned from
        OpenProtocol / LocateHandle.

    TpmInputParamterBlockSize - Supplies the size of the data to send to the
        TPM.

    TpmInputParamterBlock - Supplies a pointer to the data to send to the TPM.
        This must be a well formed request packet.

    TpmOutputParamterBlockSize - Supplies the size of the buffer to receive the
        TPM reply. Note that if this buffer is too large, the command may fail
        due to firmware implementation.

    TpmOutputParamterBlock - Supplies a pointer to the buffer to receive the
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
    // Translate the virtual pointer to the TcgProtocol address and the TPM
    // input/output buffers.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(TcgProtocol, &InterfacePa);
        TcgProtocol = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlMmTranslateVirtualAddress(TpmInputParamterBlock, &InputPa);
        TpmInputParamterBlock = (PVOID)(UINTN)InputPa.QuadPart;
        BlMmTranslateVirtualAddress(TpmOutputParameterBlock, &OutputPa);
        TpmOutputParameterBlock = (PVOID)(UINTN)OutputPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested TcgProtocol.PassThroughToTpm call.
    //

    EfiStatus = TcgProtocol->PassThroughToTPM(TcgProtocol,
                                              TpmInputParamterBlockSize,
                                              TpmInputParamterBlock,
                                              TpmOutputParameterBlockSize,
                                              TpmOutputParameterBlock);

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
EfiTcgHashLogExtendEvent (
    __in EFI_TCG_PROTOCOL *TcgProtocol,
    __in PVOID HashData,
    __in UINT64 HashDataLen,
    __in TCG_ALGORITHM_ID AlgorithmId,
    __inout TCG_PCR_EVENT *TCGLogData,
    __out_opt UINT32 *EventNumber,
    __out_opt PHYSICAL_ADDRESS *EventLogLastEntry
    )

/*++

Routine Description:

    This routine is a wrapper for EFI_TCG_PROTOCOL.HashLogExtendEvent, which
    combines a sequence of common operations when measuring code and events.
    The data pointed to by HashData is hashed; it is written to a PCR (extend)
    and a related entry is written to the log to indicate what was measured.

Arguments:

    TcgProtocol - Supplies an Interface for the TCG routines, returned from
        OpenProtocol / LocateHandle.

    HashData - Supplies a pointer to the data to be hashed & logged.

    HashDataLen - Supplies the length of the data to be hashed & logged.

    AlgorithmId - Supplies the algorithm to use (currently only one is
        supported).

    TCGLogData - Supplies a pointer to the entry to be written to the TCG log.
        This entry will be updated with the hash on success.

    EventNumber - Supplies a pointer to a variable that is set to the event
        number of the event written to the event log.

    EventLogLastEntry - Supplies an optional pointer to a variable that
        receives the physical location of the first byte of the last event in
        the event log that was just logged by this function.
        This value will be the very first physical address of the event log
        if the event log was previously empty.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_IO_DEVICE_ERROR if the command was unsuccessful.

    STATUS_INVALID_PARAMETER if one or more of the parameters are incorrect.

    STATUS_BUFFER_TOO_SMALL if the event entry is too small.

    STATUS_NOT_FOUND if the component is not running.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS HashDataPa = {0};
    PHYSICAL_ADDRESS InterfacePa = {0};
    PHYSICAL_ADDRESS LogDataPa = {0};
    EFI_TCG_HASH_LOG_EXTEND_EVENT_OUT OutParams;
    PHYSICAL_ADDRESS OutParamsPa;
    PEFI_TCG_HASH_LOG_EXTEND_EVENT_OUT OutParamsPointer;

    RtlZeroMemory(&OutParams, sizeof(OutParams));
    OutParamsPointer = &OutParams;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    //
    // Translate the virtual pointer to the TcgProtocol address, the data block
    // to be hashed, the log data and the event number output parameter. The
    // block of data to be hashed must also be mapped as well as the log data
    // and the pointer to the location that will contain the resulting event
    // number.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(TcgProtocol, &InterfacePa);
        TcgProtocol = (PVOID)(UINTN)InterfacePa.QuadPart;
        BlMmTranslateVirtualAddress(HashData, &HashDataPa);
        HashData = (PVOID)(UINTN)HashDataPa.QuadPart;
        BlMmTranslateVirtualAddress(TCGLogData, &LogDataPa);
        TCGLogData = (PVOID)(UINTN)LogDataPa.QuadPart;
        BlMmTranslateVirtualAddress(OutParamsPointer, &OutParamsPa);
        OutParamsPointer = (PVOID)(UINTN)OutParamsPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested TcgProtocol.HashLogExtendEvent call.
    //

    EfiStatus = TcgProtocol->HashLogExtendEvent(TcgProtocol,
                                                (EFI_PHYSICAL_ADDRESS)HashData,
                                                HashDataLen,
                                                AlgorithmId,
                                                TCGLogData,
                                                &OutParamsPointer->EventNumber,
                                                &OutParamsPointer->EventLogLastEntry);

    //
    // Restore original execution context. None of the pointers that were
    // remapped above are valid after this point.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    //
    // Handle out parameters.
    //

    if (ARGUMENT_PRESENT(EventNumber)) {
        *EventNumber = OutParams.EventNumber;
    }

    if (ARGUMENT_PRESENT(EventLogLastEntry)) {
        EventLogLastEntry->QuadPart = (ULONGLONG)OutParams.EventLogLastEntry;
    }

    return EfiGetNtStatusCode(EfiStatus);
}

