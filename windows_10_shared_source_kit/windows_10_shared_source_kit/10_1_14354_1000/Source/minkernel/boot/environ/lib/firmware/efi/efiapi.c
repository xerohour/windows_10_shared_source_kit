/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efiapi.c

Abstract:

    Wrappers for EFI API routines.

    All efi call must be made in physical mode.  When a boot environment
    application runs in virtual mode, pointers need to be modified and fixed
    before every EFI call.  Additionally firmware pointers/addresses need to be
    virtually mapped for the caller to read contents.  The wrappers intend to
    hide as much of that work as possible from the caller.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "bootlibp.h"

//
// -------------------------------------------------------------------- Pragmas
//

#pragma warning(disable:4152)      // Function pointer to data pointer.

//
// ---------------------------------------------------------- Variable Services
//

NTSTATUS
EfiGetVariable (
    __in PCWSTR VariableName,
    __in const EFI_GUID *VendorGuid,
    __out_opt PULONG Attributes,
    __inout UINTN *DataSize,
    __out_bcount_opt(*DataSize) PVOID Data
    )

/*++

Routine Description:

    This routine is a wrapper for the EFI service, GetVariable which will
    get the vendor data stored in NVRAM.

Arguments:

    VariableName - Null-terminated unicode string that is the name of the
                   vendor's variable.

    VendorGuid - A unique identifier for the vendor.  The namespace is
                 partitioned by vendor to avoid namespace collision.
                 This value specifies the partition containing the variable.

    Attributes - Specifies the access conditions for the variable.  Possible
                 values include:

                 EFI_VARIABLE_NON_VOLATILE - Variable is persistent across
                                             reboot.

                 EFI_VARIABLE_BOOTSERVICE_ACCESS - Variable can only be
                                                   accessed during boot
                                                   services.

                 EFI_VARIABLE_RUNTIME_ACCESS - Variable can be accessed after
                                               ExitBootServices has been
                                               called.

    DataSize - On input, contains the size (in bytes) of Data.  On output,
               contains the size of the data returned in Data.

    Data - Buffer to receive the contents of the variable.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NOT_FOUND if variable was not found.
    STATUS_BUFFER_TOO_SMALL if DataSize is too small for the result.
    STATUS_INVALID_PARAMETER if one of the parameters was invalid.

--*/

{

    UINT32 Attrib;
    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS DataPa = {0};
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS VariableNamePa = {0};
    PHYSICAL_ADDRESS VendorGuidPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(Data, &DataPa);
        Data = (PVOID)(ULONG_PTR)DataPa.QuadPart;
        BlMmTranslateVirtualAddress(VariableName, &VariableNamePa);
        VariableName = (PVOID)(ULONG_PTR)VariableNamePa.QuadPart;
        BlMmTranslateVirtualAddress(VendorGuid, &VendorGuidPa);
        VendorGuid = (EFI_GUID *)(ULONG_PTR)VendorGuidPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested service call.
    //

    EfiStatus = EfiRT->GetVariable((PWSTR)VariableName,    // const_cast
                                   (EFI_GUID*)VendorGuid,  // const_cast
                                   &Attrib,
                                   DataSize,
                                   Data);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    //
    // Return attributes.
    //

    if (Attributes != NULL) {
        *Attributes = Attrib;
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiSetVariable (
    __in PCWSTR VariableName,
    __in const EFI_GUID *VendorGuid,
    __in ULONG Attributes,
    __in UINTN DataSize,
    __in_bcount_opt(DataSize) PVOID Data
    )

/*++

Routine Description:

    This routine is a wrapper for the EFI service, SetVariable which will
    store vendor data in non-volatile storage.

Arguments:

    VariableName - Null-terminated unicode string that is the name of the
                   vendor's variable.

    VendorGuid - A unique identifier for the vendor.  The namespace is
                 partitioned by vendor to avoid namespace collision.  This
                 value specifies the partition containing the variable.

    Attributes - Specifies the access conditions for the variable.  Possible
                 values include:

                 EFI_VARIABLE_NON_VOLATILE - Variable is persistent across
                                             reboot.

                 EFI_VARIABLE_BOOTSERVICE_ACCESS - Variable can only be
                                                   accessed during boot
                                                   services.

                 EFI_VARIABLE_RUNTIME_ACCESS - Variable can be accessed after
                                               ExitBootServices has been
                                               called.

    DataSize - The size (in bytes) of the data buffer.

    Data - The data to store under the variable name, VariableName.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if one of the parameters was invalid.
    STATUS_INSUFFICIENT_RESOURCES if there is not enough storage to hold
        the contents of the variable.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS DataPa;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS VariableNamePa = {0};
    PHYSICAL_ADDRESS VendorGuidPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        if (Data != NULL) {
            BlMmTranslateVirtualAddress(Data, &DataPa);
            Data = (PVOID)(ULONG_PTR)DataPa.QuadPart;
        }

        BlMmTranslateVirtualAddress(VariableName, &VariableNamePa);
        VariableName = (PVOID)(ULONG_PTR)VariableNamePa.QuadPart;
        BlMmTranslateVirtualAddress(VendorGuid, &VendorGuidPa);
        VendorGuid = (EFI_GUID *)(ULONG_PTR)VendorGuidPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested service call.
    //

    EfiStatus = EfiRT->SetVariable((PWSTR)VariableName,     // const_cast
                                   (EFI_GUID*)VendorGuid,   // const_cast
                                   Attributes,
                                   DataSize,
                                   Data);

    //
    // Restore the original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiGetNextVariableName (
    __inout UINTN *VariableNameSize,
    __inout_bcount(*VariableNameSize) PWCHAR VariableName,
    __inout EFI_GUID *VendorGuid
    )

/*++

Routine Description:

    This routine is a wrapper for the EFI service GetNextVariableName, which
    will enumerate the names and guids of the vendor data stored in NVRAM.

Arguments:

    VariableNameSize - On input, contains the size of the VariableName buffer,
         and on output, contains the size of the returned VariableName.

    VariableName - Null-terminated unicode string that on input supplies the
         last VariableName that was returned by EfiGetNextVariableName. On
         output, returns the null-terminated string of the current variable
         name.

    VendorGuid - On input, supplies the last VendorGuid that was returned by
         EfiGetNextVariableName. On output, returns the vendor guid of the
         current variable.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NOT_FOUND if the next variable was not found.

    STATUS_BUFFER_TOO_SMALL if VariableNameSize is too small for the result.

    STATUS_INVALID_PARAMETER if one of the parameters was invalid.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS VariableNamePa = {0};
    PHYSICAL_ADDRESS VariableNameSizePa = {0};
    PHYSICAL_ADDRESS VendorGuidPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(VariableName, &VariableNamePa);
        VariableName = (PWCHAR)(ULONG_PTR)VariableNamePa.QuadPart;
        BlMmTranslateVirtualAddress(VariableNameSize, &VariableNameSizePa);
        VariableNameSize = (UINTN*)(ULONG_PTR)VariableNameSizePa.QuadPart;
        BlMmTranslateVirtualAddress(VendorGuid, &VendorGuidPa);
        VendorGuid = (EFI_GUID *)(ULONG_PTR)VendorGuidPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested service call.
    //

    EfiStatus = EfiRT->GetNextVariableName(VariableNameSize,
                                           VariableName,
                                           VendorGuid);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiUpdateCapsule (
    __inout_ecount(CapsuleCount) EFI_CAPSULE_HEADER **CapsuleHeaderArray,
    __in UINTN CapsuleCount,
    __in_opt EFI_PHYSICAL_ADDRESS ScatterGatherList
    )

/*++

Routine Description:

    This routine supplies the firmware with capsule updates.

Arguments:

    CapsuleHeaderArray - Supplies an array of virtual pointers to the capsules
        being passed into the update capsule routine.

    CapsuleCount - Supplies the number of capsules in the capsule header array.

    ScatterGatherList - Supplies the physical address of a set of capsule block
        descriptors that describe the physical location of the capules being
        updated.

Return Value:

    NT status code.

--*/

{

    PHYSICAL_ADDRESS CapsuleHeaderPa = {0};
    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    ULONG Index;

    if (EfiRT->UpdateCapsule == NULL) {
        return STATUS_NOT_IMPLEMENTED;
    }

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        for (Index = 0; Index < CapsuleCount; Index += 1) {
            BlMmTranslateVirtualAddress(CapsuleHeaderArray[Index],
                                        &CapsuleHeaderPa);

            CapsuleHeaderArray[Index] =
                (EFI_CAPSULE_HEADER *)(ULONG_PTR)CapsuleHeaderPa.QuadPart;
        }

        BlMmTranslateVirtualAddress(CapsuleHeaderArray, &CapsuleHeaderPa);
        CapsuleHeaderArray =
            (EFI_CAPSULE_HEADER **)(ULONG_PTR)CapsuleHeaderPa.QuadPart;

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested service call.
    //

    EfiStatus = EfiRT->UpdateCapsule(CapsuleHeaderArray,
                                     CapsuleCount,
                                     ScatterGatherList);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

//
// ------------------------------------------------------------- Event Services
//

NTSTATUS
EfiWaitForEvent (
    __in UINTN NumberOfEvents,
    __in_ecount(NumberOfEvents) EFI_EVENT *Event,
    __out UINTN *Index
    )

/*++

Routine Description:

    This routine is a wrapper for the EFI Service, WaitForEvent which
    will stop execution until an event is signaled.

    TODO: A change was made for x86 to disable interrupts when executing
          with address translation.  This change is temporary and breaks
          any callers to EfiWaitForEvent with a timer event (currently none).

          The change attempts to prevent the firmware from preempting a
          boot application and referencing unmapped memory.  To properly
          handle this case, a real page fault handler is required.
Arguments:

    NumberOfEvents - The number of events in the Event array.

    Event - An array of EFI_EVENTs.  Each element describes an event to or
            condition to trigger execution.

    Index - A pointer to the index in the Event array which signaled
            execution.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if the Event array contained an entry with an
        invalid type or if the event which signaled execution has a
        notification function.
    STATUS_NOT_SUPPORTED if the current TPL is not TPL_APPLICATION.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS EventPa = {0};
    PHYSICAL_ADDRESS IndexPa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(Event, &EventPa);
        Event = (PVOID)(ULONG_PTR)EventPa.QuadPart;
        BlMmTranslateVirtualAddress(Index, &IndexPa);
        Index = (UINTN *)(ULONG_PTR)IndexPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested WaitForEvent call.
    //

    EfiStatus = EfiBS->WaitForEvent(NumberOfEvents, Event, Index);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiCheckEvent (
    __in EFI_EVENT Event
    )

/*++

Routine Description:

    This routine is a wrapper for the EFI boot service, CheckEvent.  This
    service checks the status of an EFI event.  If the event is in a signaled
    state, the routine will return successfully.

Arguments:

    EventHandle - Supplies an EFI event.

Return Value:

    NT status code.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    EFI_EVENT LocalEvent;

    //
    // N.B. An EFI event is a pointer.  Copy the event into a local stack
    //      variable.  The stack is mapped one-to-one and will be accessible
    //      by the firmware.
    //

    LocalEvent = Event;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested CheckEvent call.
    //

    EfiStatus = EfiBS->CheckEvent(LocalEvent);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiCloseEvent (
    __in EFI_EVENT Event
    )

/*++

Routine Description:

    This routine is a wrapper for the EFI boot service, CloseEvent.  This
    service closes an event.

Arguments:

    EventHandle - Supplies an EFI event.

Return Value:

    NT status code.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    EFI_EVENT LocalEvent;

    //
    // N.B. An EFI event is a pointer.  Copy the event into a local stack
    //      variable.  The stack is mapped one-to-one and will be accessible
    //      by the firmware.
    //

    LocalEvent = Event;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested Close call.
    //

    EfiStatus = EfiBS->CloseEvent(LocalEvent);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiCreateEvent (
    __in UINT32 Type,
    __in EFI_TPL NotifyTpl,
    __in_opt EFI_EVENT_NOTIFY NotifyFunction,
    __in_opt PVOID NotifyContext,
    __out EFI_EVENT *Event
    )

/*++

Routine Description:

    This routine is a wrapper for the EFI boot service, CreateEvent.  This
    service creates an EFI event.

Arguments:

    Type - Supplies the type of event to create and its mode and attributes.

    NotifyTpl - Supplies the task priority level of the event notification.

    NotifyFunction - Supplies an optional pointer to the event's notification
        function.

    NotifyContext - Supplies an optional pointer to context to supply to the
        optional notification function.

    Event - Supplies a pointer to a variable that receives the EFI event.

Return Value:

    NT status code.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS PhysicalAddress = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware calls.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        if (ARGUMENT_PRESENT(NotifyFunction) != FALSE) {
            BlMmTranslateVirtualAddress(NotifyFunction, &PhysicalAddress);
            NotifyFunction = (PVOID)(ULONG_PTR)PhysicalAddress.QuadPart;
        }

        if (ARGUMENT_PRESENT(NotifyContext) != FALSE) {
            BlMmTranslateVirtualAddress(NotifyContext, &PhysicalAddress);
            NotifyContext = (PVOID)(ULONG_PTR)PhysicalAddress.QuadPart;
        }

        BlMmTranslateVirtualAddress(Event, &PhysicalAddress);
        Event = (PVOID)(ULONG_PTR)PhysicalAddress.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested CreateEvent call.
    //

    EfiStatus = EfiBS->CreateEvent(Type,
                                   NotifyTpl,
                                   NotifyFunction,
                                   NotifyContext,
                                   Event);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiSetTimer (
    __in EFI_EVENT Event,
    __in EFI_TIMER_DELAY Type,
    __in UINT64 TriggerTime
    )

/*++

Routine Description:

    This routine is a wrapper for the EFI boot service, SetTimer.  This
    service set the type of timer and the trigger time for a timer event.

Arguments:

    Event - Supplies a timer event.

    Type - Supplies the type of timer.

    TriggerTime - Supplies the number of 100ns units until the timer expires.

Return Value:

    NT status code.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    EFI_EVENT LocalEvent;

    //
    // N.B. An EFI event is a pointer.  Copy the event into a local stack
    //      variable.  The stack is mapped one-to-one and will be accessible
    //      by the firmware.
    //

    LocalEvent = Event;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested SetTimer call.
    //

    EfiStatus = EfiBS->SetTimer(LocalEvent, Type, TriggerTime);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

//
//
// ------------------------------------------------------------- Image Services
//

NTSTATUS
EfiExitBootServices (
    __in UINTN MapKey
    )

/*++

Routine Description:

    Wrapper for the EFI Service call, ExitBootServices.  This routine
    terminates all EFI boot time services.  On successful return,
    the OS loader is in complete control of the system.

Arguments:

    MapKey - Key to the latest firmware memory map.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if MapKey is invalid.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested ExitBootServices call.
    //

    EfiStatus = EfiBS->ExitBootServices(EfiImageHandle, MapKey);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiLoadImage (
    __in EFI_HANDLE ParentImageHandle,
    __in EFI_DEVICE_PATH *FilePath,
    __in_bcount_opt(SourceSize) PVOID SourceBuffer,
    __in UINTN SourceSize,
    __out EFI_HANDLE *ImageHandle
    )

/*++

Routine Description:

    This routine is a wrapper for loading an EFI Image into memory.

Arguments:

    ParentImageHandle - Supplies the caller's image handle value.

    FilePath - Supplies a pointer to the file path from which the image is to
        be loaded.

    SourceBuffer - Supplies a pointer to the memory location containing the
        copy of the image to be loaded. This is an optional value and is only
        required for memory-to-memory load operations.

    SourceSize - Supplies the size in bytes of the SourceBuffer.

    ImageHandle - Supplies a pointer to a variable that receives the handle to
        the loaded image on success.

Return Value:

    STATUS_SUCCESS if the image was successfully loaded into memory.

    STATUS_NOT_FOUND if the specified path to the file was not found.

    STATUS_INVALID_PARAMETER if any of the parameters has an invalid value.

    STATUS_NOT_SUPPORTED if the image type is not supported or

    STATUS_INSUFFICIENT_RESOURCES if sufficient resources do not exist to load
        the image into memory.

    STATUS_DRIVER_UNABLE_TO_LOAD if the image format is corrupt.

    STATUS_IO_DEVICE_ERROR if the device returned a read error.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS FilePathPa;
    PHYSICAL_ADDRESS ImageHandlePa;
    PHYSICAL_ADDRESS SourceBufferPa;

    if ((FilePath == NULL) || (ImageHandle == NULL)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(FilePath, &FilePathPa);
        FilePath = (EFI_DEVICE_PATH *)(ULONG_PTR)FilePathPa.QuadPart;
        if (ARGUMENT_PRESENT(SourceBuffer) != FALSE) {
            BlMmTranslateVirtualAddress(SourceBuffer, &SourceBufferPa);
            SourceBuffer = (PVOID)(ULONG_PTR)SourceBufferPa.QuadPart;
        }

        BlMmTranslateVirtualAddress(ImageHandle, &ImageHandlePa);
        ImageHandle = (EFI_HANDLE *)(ULONG_PTR)ImageHandlePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Invoke the firmware routine to load the image into memory.
    //

    EfiStatus = EfiBS->LoadImage(FALSE,
                                 ParentImageHandle,
                                 FilePath,
                                 SourceBuffer,
                                 SourceSize,
                                 ImageHandle);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiUnloadImage (
    __in EFI_HANDLE ImageHandle
    )

/*++

Routine Description:

    This routine is a wrapper for unloading an EFI Image.

Arguments:

    ImageHandle - Supplies the image handle to be unloaded.

Return Value:

    STATUS_SUCCESS if the image was successfully unloaded.

    STATUS_INVALID_PARAMETER if any of the parameters has an invalid value.

    STATUS_NOT_SUPPORTED if the image has been started, and does not support
        unload.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;

    if (ImageHandle == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Invoke the firmware routine to unload the image from memory.
    //

    EfiStatus = EfiBS->UnloadImage(ImageHandle);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiStartImage (
    __in EFI_HANDLE ImageHandle,
    __out_opt UINTN *ExitDataSize,
    __out_opt CHAR16 **ExitData
    )

/*++

Routine Description:

    This routine is a wrapper for transferring control to a loaded image.

Arguments:

    ImageHandle - Supplies a handle to the image to be started.

    ExitDataSize - Supplies a pointer to a variable that receives the size
        (in bytes) of data inside ExitData.

    ExitData - Supplies a pointer to a variable that receives the address of
        the data buffer containing the reason for image's exit.

Return Value:

    STATUS_INVALID_PARAMETER if ImageHandle is not a handle to an unstarted
        image.

    Appropriate NTSTATUS code otherwise.


--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS ExitDataPa;
    PHYSICAL_ADDRESS ExitDataSizePa;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        if (ARGUMENT_PRESENT(ExitDataSize) != FALSE) {
            BlMmTranslateVirtualAddress(ExitDataSize, &ExitDataSizePa);
            ExitDataSize = (UINTN *)(ULONG_PTR)ExitDataSizePa.QuadPart;
        }

        if (ARGUMENT_PRESENT(ExitData) != FALSE) {
            BlMmTranslateVirtualAddress(ExitData, &ExitDataPa);
            ExitData = (CHAR16 **)(ULONG_PTR)ExitDataPa.QuadPart;
        }

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Make the requested StartImage call.
    //

    EfiStatus = EfiBS->StartImage(ImageHandle, ExitDataSize, ExitData);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

//
// ------------------------------------------------------------ Memory Services
//

NTSTATUS
EfiGetMemoryMap (
    __inout UINTN *MemoryMapSize,
    __inout_bcount_opt(*MemoryMapSize) EFI_MEMORY_DESCRIPTOR *MemoryMap,
    __out UINTN *MapKey,
    __out UINTN *DescriptorSize,
    __out UINT32 *DescriptorVersion
    )

/*++

Routine Description:

    Wrapper for the EFI Service call, GetMemoryMap.  This routine returns
    the current firmware memory map.

Arguments:

    MemoryMapSize - A pointer to the size (in bytes) of the MemoryMap
                    buffer.  On output, this pointer contains the size of
                    the buffer returned by the firmware.  If the caller
                    specified buffer is too small, the required buffer size
                    will be returned.

    MemoryMap - A pointer to a buffer to return the firmware memory map.
                The resultant output is an array of EFI_MEMORY_DESCRIPTORS.

    MapKey - A pointer to a memory location in which the firmware will return
             a key associated with the memory map returned.

    DescriptorSize - A pointer to a memory location in which the firmware
                     will return the size (in bytes) of an individual
                     EFI_MEMORY_DESCRIPTOR.

    DescriptorVersion - A pointer to a memory location in which the firmware
                        will return the version number associated with the
                        definition of a EFI_MEMORY_DESCRIPTOR.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_BUFFER_TOO_SMALL if the MemoryMap buffer was too small.
    STATUS_INVALID_PARAMETER if one of the parameters has an invalid value.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS DescriptorSizePa = {0};
    PHYSICAL_ADDRESS DescriptorVersionPa = {0};
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS MapKeyPa = {0};
    PHYSICAL_ADDRESS MemoryMapPa = {0};
    PHYSICAL_ADDRESS MemoryMapSizePa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(MemoryMapSize, &MemoryMapSizePa);
        MemoryMapSize = (PVOID)(ULONG_PTR)MemoryMapSizePa.QuadPart;
        BlMmTranslateVirtualAddress(MemoryMap, &MemoryMapPa);
        MemoryMap = (PVOID)(ULONG_PTR)MemoryMapPa.QuadPart;
        BlMmTranslateVirtualAddress(DescriptorSize, &DescriptorSizePa);
        DescriptorSize = (PVOID)(ULONG_PTR)DescriptorSizePa.QuadPart;
        BlMmTranslateVirtualAddress(DescriptorVersion, &DescriptorVersionPa);
        DescriptorVersion = (PVOID)(ULONG_PTR)DescriptorVersionPa.QuadPart;
        BlMmTranslateVirtualAddress(MapKey, &MapKeyPa);
        MapKey = (PVOID)(ULONG_PTR)MapKeyPa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = EfiBS->GetMemoryMap(MemoryMapSize,
                                    MemoryMap,
                                    MapKey,
                                    DescriptorSize,
                                    DescriptorVersion);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiAllocatePages (
    __in EFI_ALLOCATE_TYPE Type,
    __in EFI_MEMORY_TYPE MemoryType,
    __in UINTN Pages,
    __inout EFI_PHYSICAL_ADDRESS *Memory
    )

/*++

Routine Description:

    Wrapper for the EFI Service call, AllocatePages.  This service will
    allocate and reserve a contiguous physical address range for the
    caller.

Arguments:

    Type - The type of allocation to perform.  Possible values include :

        AllocateAnyPages - Allocates any available range of pages that
                           satisfies the request.

        AllocateMaxAddress - Allocates any availabe range of pages whose
                             uppermost address is less than or equal to the
                             address pointed to by Memory.

        Allocate - Allocates pages starting at the address pointed to by
                   Memory.

    MemoryType - The type of memory to allocated.

    Pages - The number of contiguous 4KB pages to allocate.

    Memory - Pointer to a physical address.  On input, the meaning of this
             pointer varies according to Type (see above).  On output, the
             address is set to the base page of the range that was allocated.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INSUFFICIENT_RESOURCES when the requested pages could not be
        allocated.
    STATUS_NOT_FOUND when the requires pages cound not be found.
    STATUS_INVALID_PARAMETER if one of the parameters has an invalid value.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = EfiBS->AllocatePages(Type, MemoryType, Pages, Memory);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiFreePages (
    __in EFI_PHYSICAL_ADDRESS Memory,
    __in UINTN Pages
    )

/*++

Routine Description:

    Wrapper for the EFI Service call, FreePages.  This service will free
    any pages allocated using the EFI Service call, AllocatePages.

Arguments:

    Memory - The base address of the physical address range to free.

    Pages - The number of contiguous 4KB pages to free.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NOT_FOUND when the requested memory pages were not allocated with
        EFI.AllocatePages.
    STATUS_INVALID_PARAMETER if Memory is not page aligned or the specified
        number of pages is invalid.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = EfiBS->FreePages(Memory, Pages);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

//
// ----------------------------------------------------------- Runtime Services
//

NTSTATUS
EfiGetTime (
    __out EFI_TIME *EfiTime
    )

/*++

Routine Description:

    Reads the system time from the EFI firmware.

Arguements:

    EfiTime - Returns the current system time value.

Return Value:

    STATUS_SUCCESS, or an appropriate error status.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS EfiTimePa = {0};

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlMmTranslateVirtualAddress(EfiTime, &EfiTimePa);
        EfiTime = (EFI_TIME *)(ULONG_PTR)EfiTimePa.QuadPart;
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = EfiRT->GetTime(EfiTime, NULL);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

VOID
EfiResetSystem (
    __in EFI_RESET_TYPE ResetType,
    __in EFI_STATUS ResetStatus,
    __in UINTN DataSize,
    __in_bcount_opt(DataSize) PWCHAR ResetData OPTIONAL
    )

/*++

Routine Description:

    Wrapper for the EFI Runtime Service, ResetSystem.  This service will
    reset the entire platform, including all processors and devices.

Arguements:

    ResetType - Specifies the type of reset to perform.  Possible values
        include:

        EfiResetCold - Causes a system-wide reset, resetting all internal
            system circuitry to initial state.

        EfiResetWarm - Causes a system-wide initialization.  The processors
            are set to their initial state.

        EfiResetShutdown - Causes the system to shotdown.

    ResetStatus - The status code for teh reset.

    DataSize - The size (in bytes) of ResetData.

    ResetData - A NULL terminated Unicode string which is optionally followed
        by binary data.  When used, this string can describe the cause of the
        reset.

Return Value:

    None.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    PHYSICAL_ADDRESS ResetDataPa;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical address.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        if (ResetData != NULL) {
            BlMmTranslateVirtualAddress(ResetData, &ResetDataPa);
            ResetData = (PWCHAR)(ULONG_PTR)ResetDataPa.QuadPart;
        }

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    //
    // Perform EFI service.
    //

    EfiRT->ResetSystem(ResetType, ResetStatus, DataSize, ResetData);

    //
    // Should not return.
    //

    ASSERT(FALSE);
}

NTSTATUS
EfiSetVirtualAddressMap (
    __in UINTN MemoryMapSize,
    __in UINTN DescriptorSize,
    __in UINT32 DescriptorVersion,
    __in_bcount(MemoryMapSize) EFI_MEMORY_DESCRIPTOR *MemoryMap
    )

/*++

Routine Description:

    This routine invokes the SetVirtualAddressMap function to virtualize
    the EFI runtime services.

Arguments:

    MemoryMapSize - Supplies the size (in bytes) of the MemoryMap buffer.

    DescriptorSize - Supplies the size of individual descriptors within the
        MemoryMap.

    DescriptorVersion - Supplies the version for EFI memory descriptors.

    VirtualMemoryMap - Supplies a pointer to a list of descriptors describing
        the virtual addresses for EFI runtime services.

Return Value:

    NT status code.

--*/

{

    EFI_STATUS EfiStatus;

    //
    // This routine is called after boot services have been terminated. Hence
    // address translation state is not changed.
    //

    EfiStatus = EfiRT->SetVirtualAddressMap(MemoryMapSize,
                                            DescriptorSize,
                                            DescriptorVersion,
                                            MemoryMap);

    return EfiGetNtStatusCode(EfiStatus);
}

//
// ------------------------------------------------ Miscellaneous Boot Services
//

NTSTATUS
EfiConnectController (
    __in EFI_HANDLE ControllerHandle,
    __in_opt EFI_HANDLE *DriverImageHandle,
    __in_opt EFI_DEVICE_PATH_PROTOCOL *RemainingDevicePath,
    __in BOOLEAN Recursive
    )

/*++

Routine Description:

    This routine is a wrapper for the UEFI Connect Controller API.

Arguments:

    ControllerHandle - Supplies the handle of the controller to connect.

    DriverImageHandle - Supplies an optional driver image handle for the
        conect.

    RemaininDevicePath - Supplies an optional device path for the remaining
        devices.

    Recursive - Supplies a boolean indicating if the connect operation is
        recursive.

Return Value:

    NT status code.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;

    //
    // Validate parameters.  Connect controller was introduced in EFI v1.10.
    // Currently only block IO devices are supported.
    //

    if (EfiST->Hdr.Revision == EFI_1_02_SYSTEM_TABLE_REVISION) {
        Status = STATUS_NOT_SUPPORTED;
        goto ConnectControllerEnd;
    }

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical address.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        if (ARGUMENT_PRESENT(DriverImageHandle) != FALSE) {
            BlMmTranslateVirtualAddress(DriverImageHandle, &PhysicalAddress);
            DriverImageHandle =
                (EFI_HANDLE *)(ULONG_PTR)PhysicalAddress.QuadPart;
        }

        if (ARGUMENT_PRESENT(RemainingDevicePath) != FALSE) {
            BlMmTranslateVirtualAddress(RemainingDevicePath, &PhysicalAddress);
            RemainingDevicePath = (EFI_DEVICE_PATH_PROTOCOL *)
                                      (ULONG_PTR)PhysicalAddress.QuadPart;
        }

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = EfiBS->ConnectController(ControllerHandle,
                                         DriverImageHandle,
                                         RemainingDevicePath,
                                         Recursive);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    Status = EfiGetNtStatusCode(EfiStatus);

ConnectControllerEnd:
    return Status;
}

NTSTATUS
EfiDisconnectController (
    __in EFI_HANDLE Handle,
    __in_opt EFI_HANDLE DriverImageHandle,
    __in_opt EFI_HANDLE ChildHandle
    )

/*++

Routine Description:

    This function uninstalls the Protocol on Handle. All EFI drivers
    which have opened the protocol will be unloaded.

Arguments:

    ControllerHandle - The handle of the controller from which driver(s)
        are to be disconnected.

    DriverImageHandle - The driver to disconnect from ControllerHandle.
        If DriverImageHandle is NULL, then all the drivers currently
        managing ControllerHandle are disconnected from ControllerHandle.

    ChildHandle - The handle of the child to destroy. If ChildHandle is
        NULL, then all the children of ControllerHandle are destroyed
        before the drivers are disconnected from ControllerHandle.

Return Value:

    STATUS_SUCCESS if operation completed successfully.

--*/

{
    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making the firmware call.
    // Translate all virtual addresses into their repective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = EfiBS->DisconnectController(Handle, DriverImageHandle, ChildHandle);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiSetWatchdogTimer (
    __in UINTN Timeout,
    __in UINT64 WatchdogCode,
    __in UINTN DataSize,
    __in_opt PWCHAR WatchdogData
    )

/*++

Routine Description:

    This routine is a wrapper for setting the EFI system watchdog timer.

Arguments:

    Timeout - Supplies the number of seconds that the watchdog timer should
        be set to. A value of zero disables the watchdog timer.

    WatchdogCode - Supplies the numeric code to log on a watchdog timer timeout
        event.

    DataSize - Supplies the size (in bytes) of the WatchDogData buffer.

    WatchdogData - Supplies a pointer to a null-terminated string optionally
        followed by additional binary data. The string is a description that
        the call may use to further indicate the reason to be logged with a
        watchdog event.

Return Value:

    STATUS_SUCCESS if the watchdog timeout was successfully set.

    STATUS_INVALID_PARAMETER if the supplied WatchdogCode is invalid.

    STATUS_NOT_UNSUPPORTED if the system does not support a watchdog timer.

    STATUS_IO_DEVICE_ERROR if the watch dog timer could not be programmed due
        to a hardware error.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;
    PHYSICAL_ADDRESS WatchdogDataPa;

    //
    // If watchdog timer is being set, ensure that the boot application is not
    // using a firmware reserved watchdog code (0x0000 to 0xFFFF).
    //

    if ((Timeout != 0) && (WatchdogCode <= 0xFFFF)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    // Translate all virtual addresses to their respective physical addresses.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        if (ARGUMENT_PRESENT(WatchdogData) != FALSE) {
            BlMmTranslateVirtualAddress(WatchdogData, &WatchdogDataPa);
            WatchdogData = (PVOID)(UINTN)WatchdogDataPa.QuadPart;
        }

        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = EfiBS->SetWatchdogTimer(Timeout,
                                        WatchdogCode,
                                        DataSize,
                                        WatchdogData);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

NTSTATUS
EfiStall (
    __in UINTN Duration
    )

/*++

Routine Description:

    This routine is a wrapper for stalling the execution on the processor for
    the specified amount of time.

Arguments:

    Duration - Supplies the number of microseconds the execution should be
        stalled.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    EFI_STATUS EfiStatus;

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    EfiStatus = EfiBS->Stall(Duration);

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);
    }

    return EfiGetNtStatusCode(EfiStatus);
}

