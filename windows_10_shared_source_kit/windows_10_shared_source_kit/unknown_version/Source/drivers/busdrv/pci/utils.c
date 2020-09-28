/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    utils.c

Abstract:

    This module contains assorted utility functions for PCI.SYS.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "buspower.h"
#include "utils.tmh"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes
// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciSendPnpIrp)
    #pragma alloc_text(PAGE, PciSendIoctl)
    #pragma alloc_text(PAGE, PciGetDeviceProperty)
    #pragma alloc_text(PAGE, PciOpenKey)
    #pragma alloc_text(PAGE, PciGetRegistryValue)
    #pragma alloc_text(PAGE, PciUnicodeStringStrStr)
    #pragma alloc_text(PAGE, PciLocalDeviceUsage)
    #pragma alloc_text(PAGE, PciIsDeviceFeatureEnabled)
    #pragma alloc_text(PAGE, PciIsDsmFunctionSupported)
    #pragma alloc_text(PAGE, PciPrepareInputParametersForDsmMethod)
    #pragma alloc_text(PAGE, PciSendIoctlForDsmEvaluation)
    #pragma alloc_text(PAGE, PciEvaluateDsmMethodToIgnoreBootConfig)
    #pragma alloc_text(PAGE, PciEvaluateDsmMethodForLtr)
    #pragma alloc_text(PAGE, PciEvaluateDsmMethodForObff)
    #pragma alloc_text(PAGE, PciEvaluateDsmMethodForDelayOnSxResume)
    #pragma alloc_text(PAGE, PciEvaluateDsmMethodForDelayOptimization)
    #pragma alloc_text(PAGE, PciIsNonHotplugBridgeWithNoChildren)
#endif

// --------------------------------------------------------------------- Globals
// ------------------------------------------------------------------- Functions

NTSTATUS
PciSendPnpIrp(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIO_STACK_LOCATION Location,
    __out_opt PULONG_PTR Information
    )
/*++

Routine Description:

    This builds and send a pnp irp to a device.

Arguments:

    DeviceObject - The a device in the device stack the irp is to be sent to -
        the top of the device stack is always found and the irp sent there first.

    Location - The initial stack location to use - contains the IRP minor code
        and any parameters

    Information - If provided contains the final value of the irps information
        field.

Return Value:

    The final status of the completed irp or an error if the irp couldn't be sent

--*/
{
    NTSTATUS status;
    PIRP irp = NULL;
    PIO_STACK_LOCATION irpStack;
    PDEVICE_OBJECT targetDevice = NULL;
    KEVENT irpCompleted;
    IO_STATUS_BLOCK statusBlock;

    PAGED_CODE();

    PCI_ASSERT(Location->MajorFunction == IRP_MJ_PNP);

    //
    // Find out where we are sending the irp
    //
    targetDevice = IoGetAttachedDeviceReference(DeviceObject);

    //
    // Get an IRP
    //
    KeInitializeEvent(&irpCompleted, SynchronizationEvent, FALSE);

    irp = IoBuildSynchronousFsdRequest(IRP_MJ_PNP,
                                       targetDevice,
                                       NULL,    // Buffer
                                       0,       // Length
                                       0,       // StartingOffset
                                       &irpCompleted,
                                       &statusBlock
                                       );
    if (!irp) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }

    irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
    irp->IoStatus.Information = 0;

    //
    // Initialize the stack location
    //
    irpStack = IoGetNextIrpStackLocation(irp);

    PCI_ASSERT(irpStack->MajorFunction == IRP_MJ_PNP);

    irpStack->MinorFunction = Location->MinorFunction;
    irpStack->Parameters = Location->Parameters;

    //
    // Call the driver and wait for completion
    //
    status = IoCallDriver(targetDevice, irp);

    if (status == STATUS_PENDING) {

        KeWaitForSingleObject(&irpCompleted, Executive, KernelMode, FALSE, NULL);
        status = statusBlock.Status;
    }
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    //
    // Return the information
    //
    if (ARGUMENT_PRESENT(Information)) {
        *Information = statusBlock.Information;
    }

    ObDereferenceObject(targetDevice);

    PCI_ASSERT((status == STATUS_PENDING) || (status == statusBlock.Status));

    return statusBlock.Status;

cleanup:

    if (targetDevice) {
        ObDereferenceObject(targetDevice);
    }

    return status;
}

NTSTATUS
PciSendIoctl(
    __in PDEVICE_OBJECT Device,
    __in ULONG IoctlCode,
    __in_bcount_opt(InputBufferLength) PVOID InputBuffer,
    __in ULONG InputBufferLength,
    __out_bcount_opt(OutputBufferLength) PVOID OutputBuffer,
    __in ULONG OutputBufferLength
    )
/*++

Routine Description:

    Builds and send an IOCTL to a device and return the results

Arguments:

    Device - a device on the device stack to receive the IOCTL - the
             irp is always sent to the top of the stack

    IoctlCode - the IOCTL to run

    InputBuffer - arguments to the IOCTL

    InputBufferLength - length in bytes of the InputBuffer

    OutputBuffer - data returned by the IOCTL

    OnputBufferLength - the size in bytes of the OutputBuffer

Return Value:

    Status

--*/
{
    NTSTATUS status;
    IO_STATUS_BLOCK ioStatus;
    KEVENT event;
    PIRP irp;
    PDEVICE_OBJECT targetDevice = NULL;

    PAGED_CODE();

    KeInitializeEvent(&event, SynchronizationEvent, FALSE);

    //
    // Get the top of the stack to send the IRP to
    //
    targetDevice = IoGetAttachedDeviceReference(Device);

    if (!targetDevice) {
        status = STATUS_INVALID_PARAMETER;
        goto exit;
    }

    //
    // Get Io to build the IRP for us
    //
    irp = IoBuildDeviceIoControlRequest(IoctlCode,
                                        targetDevice,
                                        InputBuffer,
                                        InputBufferLength,
                                        OutputBuffer,
                                        OutputBufferLength,
                                        FALSE, // InternalDeviceIoControl
                                        &event,
                                        &ioStatus
                                        );
    if (!irp) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    //
    // Send the IRP and wait for it to complete
    //
    status = IoCallDriver(targetDevice, irp);

    if (status == STATUS_PENDING) {
        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
        status = ioStatus.Status;
    }

exit:

    if (targetDevice) {
        ObDereferenceObject(targetDevice);
    }

    return status;
}

VOID
NullResourceRequirement (
    _Inout_ PIO_RESOURCE_DESCRIPTOR Descriptor
    )
/*++

Routine Description:

    This routine changes an I/O resource descriptor to a "null" one, which
    will be ignored in arbitration.  This happens when we want to squash an
    unneeded bridge window, line-based interrupt or somesuch.

    It's important to also set the flags and "option" to zero, too, to keep
    any user-mode clients of the cfgmgr32.dll from accidentally interpreting
    these fields.

Arguments:

    Descriptor - A part of an I/O resource requirements list.

Return Value:

    none

--*/
{
    Descriptor->Type = CmResourceTypeNull;
    Descriptor->Flags = 0;
    Descriptor->Option = 0;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciGetDeviceProperty(
    _In_ PDEVICE_OBJECT PhysicalDeviceObject,
    _In_ DEVICE_REGISTRY_PROPERTY DeviceProperty,
    _Out_
    _At_(*PropertyBuffer, __drv_allocatesMem(Mem))
         PVOID *PropertyBuffer
    )
/*++

Routine Description:

    Retrieves a device property from IoGetDeviceProperty.

Arguments:

    PhysicalDeviceObject - The PDO to get the property of.

    DeviceProperty - Which property to retrieve.

    PropertyBuffer - Buffer to store the property data.

Return Value:

    NT Status code.

--*/
{
    NTSTATUS status;
    NTSTATUS expected;
    ULONG length = 0;
    ULONG length2 = 0;
    PVOID buffer;

    PAGED_CODE();

    //
    // Two passes, first pass, find out what size buffer
    // is needed.
    //
    status = IoGetDeviceProperty(
                 PhysicalDeviceObject,
                 DeviceProperty,
                 0,
                 NULL,
                 &length
                 );

    expected = STATUS_BUFFER_TOO_SMALL;

    if (status == expected) {

        //
        // Good, now get a buffer.
        //
        buffer = PciAllocatePool(PagedPool | POOL_COLD_ALLOCATION, length);

        if (buffer == NULL) {

            WARN("Failed to allocate DeviceProperty buffer (%d bytes).\n", length);

            status = STATUS_INSUFFICIENT_RESOURCES;

        } else {

            //
            // This time, do it for real.
            //
            status = IoGetDeviceProperty(
                         PhysicalDeviceObject,
                         DeviceProperty,
                         length,
                         buffer,
                         &length2
                         );

            if (NT_SUCCESS(status)) {
                PCI_ASSERT(length == length2);

                //
                // Return the buffer containing the requested device
                // property to the caller.
                //
                *PropertyBuffer = buffer;
                return STATUS_SUCCESS;
            }
            expected = STATUS_SUCCESS;
            PciFreePool(buffer);
        }
    }

    //
    // We can get here from an error in IoGetDeviceProperty, or if
    // the return length from the device property is 0, which is legal.
    // In both cases, we don't have a buffer to return to the user.
    // If it is legal for a given device property to return a 0 length
    // buffer, the caller of this routine needs to comprehend that.
    //
    *PropertyBuffer = NULL;

    return status;
}

_Success_(return != FALSE)
BOOLEAN
PciOpenKey(
    _In_ PWSTR KeyName,
    _In_opt_ HANDLE ParentHandle,
    _In_ ACCESS_MASK Access,
    _Out_ PHANDLE Handle,
    _Out_opt_ PNTSTATUS Status
    )
/*++

Routine Description:

    Open a registry key.

Arguments:

    KeyName - Name of the key to be opened.

    ParentHandle - Pointer to the parent handle (OPTIONAL)

    Access - The access to open the key with.

    Handle - Pointer to a handle to recieve the opened key.

    Status - On exit, will point to the return value from ZwOpenKey.

Return Value:

    TRUE is key successfully opened, FALSE otherwise.

--*/
{
    OBJECT_ATTRIBUTES nameAttributes;
    UNICODE_STRING nameString;
    NTSTATUS localStatus;

    PAGED_CODE();

    RtlInitUnicodeString(&nameString, KeyName);

    InitializeObjectAttributes(&nameAttributes,
                               &nameString,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               ParentHandle,
                               (PSECURITY_DESCRIPTOR)NULL
                               );
    localStatus = ZwOpenKey(Handle,
                            Access,
                            &nameAttributes
                            );

    if (ARGUMENT_PRESENT(Status)) {

        *Status = localStatus;
    }

    //
    // Return status converted to a boolean, TRUE if
    // successful.
    //
    return NT_SUCCESS(localStatus);
}

NTSTATUS
PciGetRegistryValue(
    __in PWSTR ValueName,
    __in_opt PWSTR KeyName,
    __in_opt HANDLE ParentHandle,
    __in ULONG Type,
    __deref_out_bcount(*Length) PVOID *Buffer,
    __out PULONG Length
    )
/*++

Routine Description:

    Gets a value from an open registry key.

Arguments:

    ValueName - Name of the value to retrieve.

    KeyName - Name of the key this value resides in, relative to ParentHandle.

    ParentHandle - Pointer to the parent handle.

    Type - The type of the registry value.

    Buffer - On successful exit, will point to a buffer allocated by
        this routine to contain the registry value.

    Length - On successful exit, will contain the length of Buffer.

Return Value:

    NT Status code.

--*/
{
    NTSTATUS status;
    HANDLE keyHandle = NULL;
    ULONG neededLength = 0;
    ULONG actualLength = 0;
    UNICODE_STRING unicodeValueName;
    PKEY_VALUE_PARTIAL_INFORMATION info = NULL;

    PAGED_CODE();

    *Buffer = NULL;

    if (!PciOpenKey(KeyName, ParentHandle, KEY_READ, &keyHandle, &status)) {
        __analysis_assume(!NT_SUCCESS(status));
        goto exit;
    }

    RtlInitUnicodeString(&unicodeValueName, ValueName);

    //
    // Find out how much memory we need for this.
    //
    status = ZwQueryValueKey(
                 keyHandle,
                 &unicodeValueName,
                 KeyValuePartialInformation,
                 NULL,
                 0,
                 &neededLength
                 );

    if (status != STATUS_BUFFER_TOO_SMALL) {
        if (NT_SUCCESS(status)) {
            ERROR("PciGetRegistryValue: Regkey query unexpectedly succeeded w/ length zero supplied.\n");
            status = STATUS_UNSUCCESSFUL;
        }

        goto exit;
    }

    PCI_ASSERT(neededLength != 0);

    //
    // Get memory to return the data in.  Note this includes
    // a header that we really don't want.
    //
    info = PciAllocatePool(PagedPool | POOL_COLD_ALLOCATION, neededLength);
    if (info == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    //
    // Get the data.
    //
    status = ZwQueryValueKey(
             keyHandle,
             &unicodeValueName,
             KeyValuePartialInformation,
             info,
             neededLength,
             &actualLength
             );
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Make sure the data is the correcty type
    //
    if (info->Type != Type) {
        status = STATUS_INVALID_PARAMETER;
        goto exit;
    }

    PCI_ASSERT(neededLength == actualLength);

    //
    // Subtract out the header size and get memory for just
    // the data we want.
    //
    neededLength -= FIELD_OFFSET(KEY_VALUE_PARTIAL_INFORMATION, Data);

    *Buffer = PciAllocatePool(
                  PagedPool | POOL_COLD_ALLOCATION,
                  neededLength
                  );
    if (*Buffer == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    //
    // Copy data sans header.
    //
    RtlCopyMemory(*Buffer, info->Data, neededLength);


    if (Length) {
        *Length = neededLength;
    }

exit:

    if (keyHandle) {
        ZwClose(keyHandle);
    }
    if (info) {
        PciFreePool(info);
    }
    return status;
}


NTSTATUS
PciDeviceCallCritical(
    __in PPCI_DEVICE Device,
    __in PCRITICALROUTINE CallbackRoutine,
    __inout_opt PVOID CallbackContext,
    __in BOOLEAN OnDebugPath
    )

/*++

Routine Description:

    This helper routine implements callbacks to critical routines, optionally
    through KeIpiGenericCallroutine, and optionally acquiring the debugger
    lock.
    This function is split out to a nonpaged block to ensure that the
    code doesn't cross a page boundary while the debugger lock is held,
    as that would cause a page fault at high IRQL.

Arguments:

    Device - Provides a pointer to the PCI device.

    CallbackRoutine - Provides a pointer to the routine to be called.

    CallbackContext - Provides a context pointer to be passed to the routine.

    OnDebugPath - Provides a boolean specifying whether the debugger
        lock should be held around the callback.

Return Value:

    Status value returned by the callback.

--*/

{

    KIRQL OldIrql;
    PCI_CRITICAL_ROUTINE_CONTEXT RoutineContext;
    NTSTATUS Status;

    NON_PAGED_CODE();

    if ((Device->HackFlags & PCI_HACK_CRITICAL_DEVICE) || OnDebugPath) {
        RoutineContext.Gate = 1;
        RoutineContext.Barrier = 1;
        RoutineContext.Routine = CallbackRoutine;
        RoutineContext.Extension = Device;
        RoutineContext.Context = CallbackContext;
        OldIrql = PASSIVE_LEVEL;
        if (OnDebugPath != FALSE) {
            KdAcquireDebuggerLock(&OldIrql);
        }

        KeIpiGenericCall(PciExecuteCriticalSystemRoutine,
                         (ULONG_PTR)&RoutineContext);

        Status = RoutineContext.Status;
        if (OnDebugPath != FALSE) {
            KdReleaseDebuggerLock(OldIrql);
        }
    } else {
        Status = CallbackRoutine(Device, CallbackContext);
    }

    return Status;
}

_IRQL_requires_same_
__drv_functionClass(KIPI_BROADCAST_WORKER)
_IRQL_requires_(IPI_LEVEL)
ULONG_PTR
PciExecuteCriticalSystemRoutine (
    _In_ ULONG_PTR Context
    )
/*++

Routine Description:

    This routine is called in the context of KeIpiGenericCall, which
    executes it on all processors.  It is used to execute
    a critical routine which needs all processors synchronized, such
    as probing the BARs of a device that could not otherwise be turned off.
    Only one context parameter is allowed in this routine, so it must
    contain both the routine to execute and any context that routine
    requires.

    When this routine is entered, it is guaranteed that all processors will
    already have been targeted with an IPI, and will be running at IPI_LEVEL.
    All processors will either be running this routine, or will be about to
    enter the routine.  No arbitrary threads can possibly be running.  No
    devices can interrupt the execution of this routine, since IPI_LEVEL is
    above all device IRQLs.

    Because this routine runs at IPI_LEVEL, no debug prints, asserts or other
    debugging can occur in this function without hanging MP machines.

Arguments:

    Context - the context passed into the call to KeIpiGenericCall.
        It contains the critical routine to execute, any context required
        in that routine and a gate and a barrier to ensure that the critical routine
        is executed on only one processor, even though this function is
        executed on all processors.

Return Value:

    VOID

--*/
{
    PPCI_CRITICAL_ROUTINE_CONTEXT routineContext;
    NTSTATUS status;

    NON_PAGED_CODE();

    routineContext = (PPCI_CRITICAL_ROUTINE_CONTEXT)Context;

    //
    // The Gate parameter in the routineContext is preinitialized
    // to 1, meaning that the first processor to reach this point
    // in the routine will decrement it to 0, and succeed the if
    // statement.
    //
    if (InterlockedDecrement(&routineContext->Gate) == 0) {

        //
        // This is only executed on one processor.
        //
        status = routineContext->Routine(routineContext->Extension,
                                         routineContext->Context
                                         );

        routineContext->Status = status;

        //
        // Free other processors.
        //
        routineContext->Barrier = 0;

    } else {

        //
        // Wait for gated function to complete.
        //
        while (routineContext->Barrier != 0) {
            PAUSE_PROCESSOR;
        }
    }

    return (ULONG_PTR)0;
}

BOOLEAN
PciUnicodeStringStrStr(
    __in PUNICODE_STRING SearchString,
    __in PUNICODE_STRING SubString,
    __in BOOLEAN CaseInsensitive
    )
/*++

Routine Description:

    This routine is a counted string version of strstr and searchs for any
    instance of SubString within SearchString.

Arguments:

    SearchString - String to search within

    SubString - String to search for

    CaseInsensitive - If TRUE indicates that a the comparison should be case
        insensitive

Return Value:

    TRUE if SubString is contained within SearchString, FALSE otherwise.

--*/
{
    USHORT searchIndex, searchCount, subCount;
    UNICODE_STRING currentSearchString;

    PAGED_CODE();

    searchCount = SearchString->Length / sizeof(WCHAR);
    subCount = SubString->Length / sizeof(WCHAR);

    if (SearchString->Length < SubString->Length) {
        return FALSE;
    }

    currentSearchString.Buffer = SearchString->Buffer;
    currentSearchString.MaximumLength = SearchString->MaximumLength;

    //
    // Set the length of the potential match string to the same length as
    // SubString so we can use RtlEqualUnicodeString to compare them.
    //

    currentSearchString.Length = SubString->Length;

    //
    // Iterate through the search string until we are less than searchCount
    // characters from the end since the SubString can't possibly fit.
    //

    for (searchIndex = 0; searchIndex <= searchCount - subCount; searchIndex++) {

        //
        // Now see if our substring is located at this position.
        //

        if(RtlEqualUnicodeString(SubString, &currentSearchString, CaseInsensitive)) {

            return TRUE;
        }

        //
        // Advance one character in the currentSearchString and decrement maximum
        // length accordingly
        //
        currentSearchString.Buffer++;
        currentSearchString.MaximumLength -= sizeof(WCHAR);
    }

    return FALSE;
}

NTSTATUS
PciLocalDeviceUsage(
    __inout PPCI_DEVICE_USAGE DeviceUsage,
    __in DEVICE_USAGE_NOTIFICATION_TYPE NotificationType,
    __in BOOLEAN InPath
    )
{
    __deref_volatile PLONG addend;
    LONG increment;
    LONG junk;

    PAGED_CODE();

    increment = InPath  ? 1 : -1;
    switch (NotificationType) {
        case DeviceUsageTypePaging:

            addend = &DeviceUsage->Paging;
            break;

        case DeviceUsageTypeHibernation:

            addend = &DeviceUsage->Hibernate;
            break;

        case DeviceUsageTypeDumpFile:

            addend = &DeviceUsage->CrashDump;
            break;

        case DeviceUsageTypeBoot:
            return STATUS_SUCCESS;

        default:
            return STATUS_NOT_SUPPORTED;
    }

    junk = InterlockedExchangeAdd (addend, increment);

    if (increment == -1) {
        PCI_ASSERT(junk > 0);
        UNREFERENCED_PARAMETER(junk);
    }

    return STATUS_SUCCESS;
}

PCI_FEATURE_STATE
PciIsDeviceFeatureEnabled (
    __in PPCI_DEVICE Device,
    __in PWSTR FeatureName
    )

/*++

Description:

    Check if the specified feature is enabled on this device.

Arguments:

    Device - Supplies the device being checked for this feature.

    FeatureName - Supplies the name of the feature being checked.

Return Value:

    TRUE is returned if the feature is enabled for this device,
    FALSE otherwise.

--*/

{

    HANDLE DeviceHandle;
    PCI_FEATURE_STATE Result;
    NTSTATUS Status;
    PULONG ValueBuffer;
    ULONG ValueLength;

    PAGED_CODE();

    DeviceHandle = NULL;
    Result = PciFeatureStateInvalid;
    ValueBuffer = NULL;
    ValueLength = 0;

    //
    // Open the key belonging to this device object in the registry.
    //

    Status = IoOpenDeviceRegistryKey(Device->DeviceObject,
                                     PLUGPLAY_REGKEY_DEVICE,
                                     KEY_READ,
                                     &DeviceHandle);

    if (!NT_SUCCESS(Status)) {
        goto PciIsDeviceFeatureEnabledExit;
    }

    //
    // Get whether or not the specified feature is enabled on this
    // device.
    //

    Status = PciGetRegistryValue(FeatureName,
                                 PCI_DEVICE_FEATURE_BITS,
                                 DeviceHandle,
                                 REG_DWORD,
                                 &ValueBuffer,
                                 &ValueLength);

    if (!NT_SUCCESS(Status)) {
        Result = PciFeatureStateUndefined;
        goto PciIsDeviceFeatureEnabledExit;
    }

    if (ValueLength == sizeof(ULONG)) {
        if (*ValueBuffer != 0) {
            Result = PciFeatureStateEnabled;
        } else {
            Result = PciFeatureStateDisabled;
        }
    }

    PciFreePool(ValueBuffer);

PciIsDeviceFeatureEnabledExit:

    if (DeviceHandle != NULL) {
        ZwClose(DeviceHandle);
    }

    return Result;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciIsDsmFunctionSupported (
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ ULONG FunctionIndex,
    _In_ ULONG FunctionRevision,
    _Out_ PBOOLEAN Supported
    )

/*++

Routine Description:

    This routine is invoked to check if a specific function for a _DSM method
    is supported.

Arguments:

    DeviceObject - Supplies a pointer to the next lower device object.

    FunctionIndex - Supplies the function index to check for.

    FunctionRevision - Indicates the revision of the function of interest.

    Supported - Supplies a pointer to a boolean that on return will indicate
        if the given function is supported.

Return Value:

    STATUS_INSUFFICIENT_RESOURCES on failure to allocate memory.

    STATUS_UNSUCCESSFUL if the output parameter is incorrectly formatted.

    STATUS_SUCCESS if the method evaluates correctly and the output parameter
        if formatted correctly.

    Otherwise a status code from a function call.

--*/

{

    ACPI_EVAL_OUTPUT_BUFFER OutputBuffer;
    PACPI_EVAL_INPUT_BUFFER_COMPLEX ParametersBuffer;
    ULONG ParametersBufferSize;
    NTSTATUS Status;

    PAGED_CODE();

    *Supported = FALSE;
    ParametersBufferSize = sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX) +
                           (sizeof(ACPI_METHOD_ARGUMENT) *
                            (DSM_METHOD_INPUT_ARGUMENTS_COUNT - 1)) +
                           sizeof(GUID);

    ParametersBuffer = PciAllocatePool(PagedPool, ParametersBufferSize);
    if (ParametersBuffer == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // When querying, the query is phrased in terms of "give me an array of bits
    // where each bit represents a function for which there is support at this
    // revision level."  I.e. "Revision 1" could return an entirely disjoint
    // bit field than "Revision 2."
    //

    PciPrepareInputParametersForDsmMethod(ParametersBuffer,
                                          ParametersBufferSize,
                                          PCI_DSM_METHOD_QUERY_FUNCTION,
                                          FunctionRevision,
                                          &PCI_DSM_UUID);

    //
    // Initialize an output buffer.
    //

    RtlZeroMemory(&OutputBuffer, sizeof(ACPI_EVAL_OUTPUT_BUFFER));

    //
    // Invoke a helper function to send an IOCTL to ACPI to evaluate this
    // control method.
    //

    Status = PciSendIoctlForDsmEvaluation(DeviceObject,
                                          ParametersBuffer,
                                          ParametersBufferSize,
                                          &OutputBuffer,
                                          sizeof(ACPI_EVAL_OUTPUT_BUFFER));

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    if (OutputBuffer.Argument[0].Type != ACPI_METHOD_ARGUMENT_BUFFER) {
        Status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    if (OutputBuffer.Argument[0].DataLength < (FunctionIndex / 8) + 1) {
        Status = STATUS_INFO_LENGTH_MISMATCH;
        goto Exit;
    }

    Status = STATUS_SUCCESS;
    if ((OutputBuffer.Argument[0].Data[FunctionIndex / 8] & (1 << (FunctionIndex % 8))) != 0) {
        *Supported = TRUE;
    }

Exit:

    //
    // STATUS_INVALID_DEVICE_REQUEST is ACPI's way of saying that the method
    // didn't exist.
    //

    if (!NT_SUCCESS(Status) && (Status != STATUS_INVALID_DEVICE_REQUEST)) {
        if (PCI_BUS_EXTENSION(DeviceObject->DeviceExtension)) {
            TraceEvents(((PPCI_BUS)DeviceObject->DeviceExtension)->BlackBoxHandle,
                        TRACE_LEVEL_WARNING,
                        DBG_ENUM,
                        "_DSM(query,%!DSM_FUNCTION!,ver.%d) %!STATUS!",
                        FunctionIndex,
                        FunctionRevision,
                        Status);
        } else {
            PCI_ASSERT(PCI_DEVICE_EXTENSION(DeviceObject->DeviceExtension));
            TraceEvents(((PPCI_DEVICE)DeviceObject->DeviceExtension)->Parent->BlackBoxHandle,
                        TRACE_LEVEL_WARNING,
                        DBG_ENUM,
                        "(%x:%x) _DSM(query,%!DSM_FUNCTION!,ver.%d) %!STATUS!",
                        ((PPCI_DEVICE)DeviceObject->DeviceExtension)->Slot.u.bits.DeviceNumber,
                        ((PPCI_DEVICE)DeviceObject->DeviceExtension)->Slot.u.bits.FunctionNumber,
                        FunctionIndex,
                        FunctionRevision,
                        Status);
        }
    }

    if (ParametersBuffer != NULL) {
        PciFreePool(ParametersBuffer);
    }

    return Status;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciPrepareInputParametersForDsmMethod (
    _In_ PACPI_EVAL_INPUT_BUFFER_COMPLEX ParametersBuffer,
    _In_ ULONG ParametersBufferSize,
    _In_ ULONG FunctionIndex,
    _In_ ULONG FunctionRevision,
    _In_ LPCGUID Guid
    )

/*++

Routine Description:

    This routine is invoked for initializing an input parameter blob
    for evaluation of a _DSM control method defined in the scope of
    a PCI device.

Arguments:

    ParameterBuffer - Supplies a pointer to an input buffer.

    ParameterBufferSize - Supplies the length for an input buffer.

    FunctionIndex - Supplies the value of a function index for an input
        parameter.

    FunctionRevision - The version of the function.

Return Value:

    None.

--*/

{

    PACPI_METHOD_ARGUMENT Argument;

    PAGED_CODE();

    RtlZeroMemory(ParametersBuffer, ParametersBufferSize);
    ParametersBuffer->Signature = ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE;
    ParametersBuffer->MethodNameAsUlong = (ULONG)'MSD_';
    ParametersBuffer->Size = ParametersBufferSize;
    ParametersBuffer->ArgumentCount = DSM_METHOD_INPUT_ARGUMENTS_COUNT;

    //
    // Argument 0: UUID.
    //

    Argument = &ParametersBuffer->Argument[0];
    ACPI_METHOD_SET_ARGUMENT_BUFFER(Argument, Guid, sizeof(GUID));

    //
    // Argument 1: Revision.
    //

    Argument = ACPI_METHOD_NEXT_ARGUMENT(Argument);
    ACPI_METHOD_SET_ARGUMENT_INTEGER(Argument, FunctionRevision);

    //
    // Argument 2: Function index.
    //

    Argument = ACPI_METHOD_NEXT_ARGUMENT(Argument);
    ACPI_METHOD_SET_ARGUMENT_INTEGER(Argument, FunctionIndex);

    //
    // Argument 3: Empty package for all functions in the _DSM definitions
    //             for PCI as per the PCI firmware spec.
    //

    Argument = ACPI_METHOD_NEXT_ARGUMENT(Argument);
    Argument->Type = ACPI_METHOD_ARGUMENT_PACKAGE;
    Argument->DataLength = sizeof(ULONG);
    Argument->Argument = 0;
    return;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciSendIoctlForDsmEvaluation (
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_reads_bytes_(InputBufferSize) PVOID InputBuffer,
    _In_ ULONG InputBufferSize,
    _Out_writes_bytes_(OutputBufferSize) PVOID OutputBuffer,
    _In_ ULONG OutputBufferSize
    )

/*++

Routine Description:

    This routine is invoked to allocate and send an IOCTL for _DSM method
    evaluation to the highest level device object in the stack.

Arguments:

    DeviceObject - Supplies a pointer to the device object this IOCTL will be
        sent to.

    InputBuffer - Supplies a pointer to an input buffer

    InputBufferLength - Supplies the length for an input buffer.

    OutputBuffer - Supplies a pointer to an output buffer.

    OutputBufferLength - Supplies the length for an output buffer.

Return Value:

    STATUS_INSUFFICIENT_RESOURCES on failure to allocate an Irp.

    Otherwise a status code returned from a method evaluation.

--*/

{

    PIRP Irp;
    KEVENT IrpCompleted;
    NTSTATUS Status;
    IO_STATUS_BLOCK StatusBlock;
    PDEVICE_OBJECT TargetDevice;

    PAGED_CODE();
    TargetDevice = IoGetAttachedDeviceReference(DeviceObject);

    PCI_ASSERT(TargetDevice != NULL);

    KeInitializeEvent(&IrpCompleted, NotificationEvent, FALSE);

    Irp = IoBuildDeviceIoControlRequest(IOCTL_ACPI_EVAL_METHOD,
                                        TargetDevice,
                                        InputBuffer,
                                        InputBufferSize,
                                        OutputBuffer,
                                        OutputBufferSize,
                                        FALSE,
                                        &IrpCompleted,
                                        &StatusBlock);

    if (Irp == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    Status = IoCallDriver(TargetDevice, Irp);
    if (Status == STATUS_PENDING) {
        KeWaitForSingleObject(&IrpCompleted,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);

        Status = StatusBlock.Status;
    }

Exit:

    ObDereferenceObject(TargetDevice);
    return Status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciEvaluateDsmMethodToIgnoreBootConfig (
    _In_ PDEVICE_OBJECT DeviceObject,
    _Out_ PBOOLEAN IgnoreBootConfig
    )

/*++

Routine Description:

    This routine is invoked to evaluate a _DSM method defined in the scope
    of a PCI device. This method is defined by the platform firmware as a
    hint for an OS to ignore the boot configuration for a device. This is to
    leverage an advance OS feature of PCI device configuration and multi
    level rebalance.

Arguments:

    DeviceObject - Supplies a pointer to the next lower device object.

    IgnoreBootConfig - Supplies a pointer for a boolean that indicates
        if a device boot configuration can be safely ignored.

Return Value:

    STATUS_INSUFFICIENT_RESOURCES on failure to allocate memory.

    STATUS_UNSUCCESSFUL if the output parameter from _DSM method evaluation
        is incorrectly formatted.

    Otherwise a status code returned from a function call.

--*/

{

    ACPI_EVAL_OUTPUT_BUFFER OutputBuffer;
    PACPI_EVAL_INPUT_BUFFER_COMPLEX ParametersBuffer;
    ULONG ParametersBufferSize;
    NTSTATUS Status;
    BOOLEAN Supported;

    PAGED_CODE();

    *IgnoreBootConfig = FALSE;
    ParametersBuffer = NULL;

    //
    // Evaluating a _DSM method is a two-step process. In the first step check
    // if a DSM function for ignoring boot configs is supported. If TRUE, then
    // evaluate this function and return an appropriate result to the caller.
    //

    Supported = FALSE;
    Status = PciIsDsmFunctionSupported(DeviceObject,
                                       PCI_DSM_METHOD_IGNORE_BOOT_CONFIG,
                                       PCI_DSM_METHOD_IGNORE_BOOT_CONFIG_REV,
                                       &Supported);

    if (!NT_SUCCESS(Status) || (Supported == FALSE)) {
        goto Exit;
    }

    //
    // Evaluate this method for real.
    //

    ParametersBufferSize = sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX) +
                           (sizeof(ACPI_METHOD_ARGUMENT) *
                            (DSM_METHOD_INPUT_ARGUMENTS_COUNT - 1)) +
                           sizeof(GUID);

    ParametersBuffer = PciAllocatePool(PagedPool, ParametersBufferSize);
    if (ParametersBuffer == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    PciPrepareInputParametersForDsmMethod(ParametersBuffer,
                                          ParametersBufferSize,
                                          PCI_DSM_METHOD_IGNORE_BOOT_CONFIG,
                                          PCI_DSM_METHOD_IGNORE_BOOT_CONFIG_REV,
                                          &PCI_DSM_UUID);

    //
    // Initialize an output buffer.
    //

    RtlZeroMemory(&OutputBuffer, sizeof(ACPI_EVAL_OUTPUT_BUFFER));

    //
    // Invoke a helper function to send an IOCTL to ACPI to evaluate this
    // control method.
    //

    Status = PciSendIoctlForDsmEvaluation(DeviceObject,
                                          ParametersBuffer,
                                          ParametersBufferSize,
                                          &OutputBuffer,
                                          sizeof(ACPI_EVAL_OUTPUT_BUFFER));

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    if ((OutputBuffer.Argument[0].Type != ACPI_METHOD_ARGUMENT_INTEGER) ||
        (OutputBuffer.Argument[0].Argument == 0)) {
        if (PCI_BUS_EXTENSION(DeviceObject->DeviceExtension)) {

            TraceEvents(((PPCI_BUS)DeviceObject->DeviceExtension)->BlackBoxHandle,
                        TRACE_LEVEL_WARNING,
                        DBG_ENUM,
                        "_DSM(ignore boot config) returned a non-integer."
                        "Type: 0x%04x, Argument: 0x%x.",
                        OutputBuffer.Argument[0].Type,
                        OutputBuffer.Argument[0].Argument);

        } else {
            PCI_ASSERT(PCI_DEVICE_EXTENSION(DeviceObject->DeviceExtension));

            TraceEvents(((PPCI_DEVICE)DeviceObject->DeviceExtension)->Parent->BlackBoxHandle,
                        TRACE_LEVEL_WARNING,
                        DBG_ENUM,
                        "(%x:%x) _DSM(ignore boot config) returned a a non-integer."
                        "Type: 0x%04x, Argument: 0x%x.",
                        ((PPCI_DEVICE)DeviceObject->DeviceExtension)->Slot.u.bits.DeviceNumber,
                        ((PPCI_DEVICE)DeviceObject->DeviceExtension)->Slot.u.bits.FunctionNumber,
                        OutputBuffer.Argument[0].Type,
                        OutputBuffer.Argument[0].Argument);
        }
        goto Exit;
    }

    *IgnoreBootConfig = TRUE;

Exit:

    if (ParametersBuffer != NULL) {
        PciFreePool(ParametersBuffer);
    }

    if (NT_SUCCESS(Status)) {
        TraceLoggingWrite(
            PciTraceLoggingProvider, 
            "PciIgnoreBootConfigInformation",
            TraceLoggingString((*IgnoreBootConfig)?
                               "Yes": "No", "PCI Ignore BootConfig"),
            TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES)
        );
    }

    return Status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciEvaluateDsmMethodForLtr (
    _In_ PDEVICE_OBJECT DeviceObject,
    _Out_ PPCI_EXPRESS_LTR_MAX_LATENCY_REGISTER LtrValues
    )

/*++

Routine Description:

    This routine is invoked to evaluate a _DSM method defined in the scope
    of a PCIe downstream port. This method is defined in the PCI Firmware
    specification.  The subfunction for LTR allows the BIOS to tell the OS what
    good LTR values would be for a PCI Express endpoint under a downstream port.
    This comes in handy if the device is hot-plugged, as the BIOS has no
    opportunity to prepopulate those values during hot-plug.  It's also possibly
    convenient for the BIOS to skip prepopulating those values, in the name of
    fast boot.

Arguments:

    DeviceObject - Supplies a pointer to the next lower device object.

    LtrValues - Supplies a pointer to be filled in with the max latency values.

Return Value:

    STATUS_INSUFFICIENT_RESOURCES on failure to allocate memory.

    STATUS_UNSUCCESSFUL if the output parameter from _DSM method evaluation
        is incorrectly formatted.

    Otherwise a status code returned from a function call.

--*/

{

#define LTR_OUTPUT_BUF_SIZE (sizeof(ACPI_EVAL_OUTPUT_BUFFER) + (sizeof(ACPI_METHOD_ARGUMENT) * 3))
    ULONG PackageIndex;
    PACPI_EVAL_INPUT_BUFFER_COMPLEX ParametersBuffer;
    ULONG ParametersBufferSize;
    PACPI_EVAL_OUTPUT_BUFFER OutputBuffer;
    UCHAR StackBuffer[LTR_OUTPUT_BUF_SIZE];
    NTSTATUS Status;
    BOOLEAN Supported;

    PAGED_CODE();

    LtrValues->AsULONG = 0;
    ParametersBuffer = NULL;

    //
    // Evaluating a _DSM method is a two-step process. In the first step check
    // if a DSM function for LTR is supported. If TRUE, then evaluate this
    // function and return an appropriate result to the caller.
    //

    Supported = FALSE;
    Status = PciIsDsmFunctionSupported(DeviceObject,
                                       PCI_DSM_METHOD_LATENCY_TOLERANCE,
                                       PCI_DSM_METHOD_LATENCY_TOLERANCE_REV,
                                       &Supported);

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    if (Supported == FALSE) {
        Status = STATUS_NOT_SUPPORTED;
        goto Exit;
    }

    //
    // Evaluate this method for real.
    //

    ParametersBufferSize = sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX) +
                           (sizeof(ACPI_METHOD_ARGUMENT) *
                            (DSM_METHOD_INPUT_ARGUMENTS_COUNT - 1)) +
                           sizeof(GUID);

    ParametersBuffer = PciAllocatePool(PagedPool, ParametersBufferSize);
    if (ParametersBuffer == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    PciPrepareInputParametersForDsmMethod(ParametersBuffer,
                                          ParametersBufferSize,
                                          PCI_DSM_METHOD_LATENCY_TOLERANCE,
                                          PCI_DSM_METHOD_LATENCY_TOLERANCE_REV,
                                          &PCI_DSM_UUID);

    //
    // Initialize an output buffer.  This function returns a package of four
    // parameters.
    //

    OutputBuffer = (PACPI_EVAL_OUTPUT_BUFFER)StackBuffer;
    RtlZeroMemory(OutputBuffer, LTR_OUTPUT_BUF_SIZE);

    //
    // Invoke a helper function to send an IOCTL to ACPI to evaluate this
    // control method.
    //

    Status = PciSendIoctlForDsmEvaluation(DeviceObject,
                                          ParametersBuffer,
                                          ParametersBufferSize,
                                          OutputBuffer,
                                          LTR_OUTPUT_BUF_SIZE);

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    //
    // The return package must contain only integers.
    //

    for (PackageIndex = 0; PackageIndex < 4; PackageIndex++) {
        if (OutputBuffer->Argument[PackageIndex].Type != ACPI_METHOD_ARGUMENT_INTEGER) {
            if (PCI_BUS_EXTENSION(DeviceObject->DeviceExtension)) {

                TraceEvents(((PPCI_BUS)DeviceObject->DeviceExtension)->BlackBoxHandle,
                            TRACE_LEVEL_WARNING,
                            DBG_ENUM,
                            "_DSM(LTR) returned a package containing non-integers."
                            "Type: 0x%04x.",
                            OutputBuffer->Argument[PackageIndex].Type);

            } else {
                PCI_ASSERT(PCI_DEVICE_EXTENSION(DeviceObject->DeviceExtension));

                TraceEvents(((PPCI_DEVICE)DeviceObject->DeviceExtension)->Parent->BlackBoxHandle,
                            TRACE_LEVEL_WARNING,
                            DBG_ENUM,
                            "(%x:%x) _DSM(LTR) returned a package containing non-integers."
                            "Type: 0x%04x.",
                            ((PPCI_DEVICE)DeviceObject->DeviceExtension)->Slot.u.bits.DeviceNumber,
                            ((PPCI_DEVICE)DeviceObject->DeviceExtension)->Slot.u.bits.FunctionNumber,
                            OutputBuffer->Argument[PackageIndex].Type);
            }
            goto Exit;
        }
    }

    LtrValues->MaxSnoopLatencyScale = *((PUCHAR)OutputBuffer->Argument[0].Data);
    LtrValues->MaxSnoopLatencyValue = *((PUSHORT)OutputBuffer->Argument[1].Data);
    LtrValues->MaxNoSnoopLatencyScale = *((PUCHAR)OutputBuffer->Argument[2].Data);
    LtrValues->MaxNoSnoopLatencyValue = *((PUSHORT)OutputBuffer->Argument[3].Data);

Exit:

    if (ParametersBuffer != NULL) {
        PciFreePool(ParametersBuffer);
    }

    return Status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciEvaluateDsmMethodForObff (
    _In_ PDEVICE_OBJECT DeviceObject,
    _Out_ PPCI_FIRMWARE_BUS_CAPS BusCaps
    )

/*++

Routine Description:

    This routine is invoked to evaluate a _DSM method defined in the scope
    of an Express upstream port. It evaluates the "Bus Capabilities" part of the
    PCI firmware _DSM, and reports whether the firmware wants OBFF to be enabled
    on devices below this port, using the "wake message."

    See section 4.6.4.1.3 of the PCI Firmware Specification 3.1 for more
    details.

Arguments:

    DeviceObject - Supplies a pointer to the next lower device object.

    BusCaps - Out parameter to be filled in with what the firmware had to say
        about this bus.

Return Value:

    STATUS_INSUFFICIENT_RESOURCES on failure to allocate memory.

    STATUS_UNSUCCESSFUL if the output parameter from _DSM method evaluation
        is incorrectly formatted.

    Otherwise a status code returned from a function call.

--*/

{

#define OBFF_OUTBUF_SIZE \
    (sizeof(ACPI_EVAL_OUTPUT_BUFFER) + \
     sizeof(ACPI_METHOD_ARGUMENT) + \
     sizeof(PCI_FIRMWARE_BUS_CAPS_RETURN_BUFFER))

    PACPI_EVAL_INPUT_BUFFER_COMPLEX ParametersBuffer;
    ULONG ParametersBufferSize;
    PACPI_EVAL_OUTPUT_BUFFER OutputBuffer;
    PPCI_FIRMWARE_BUS_CAPS_RETURN_BUFFER RetBuf;
    UCHAR StackBuffer[OBFF_OUTBUF_SIZE];
    NTSTATUS Status;
    BOOLEAN Supported;

    PAGED_CODE();

    ParametersBuffer = NULL;

    //
    // Evaluating a _DSM method is a two-step process. In the first step check
    // if a DSM function for bus capabilities is supported. If TRUE, then
    // evaluate this function and return an appropriate result to the caller.
    //

    Supported = FALSE;
    Status = PciIsDsmFunctionSupported(DeviceObject,
                                       PCI_DSM_METHOD_BUS_CAPABILITIES,
                                       PCI_DSM_METHOD_BUS_CAPABILITIES_REV,
                                       &Supported);

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    if (Supported == FALSE) {
        Status = STATUS_NOT_SUPPORTED;
        goto Exit;
    }

    //
    // Evaluate this method for real.
    //

    ParametersBufferSize = sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX) +
                           (sizeof(ACPI_METHOD_ARGUMENT) *
                            (DSM_METHOD_INPUT_ARGUMENTS_COUNT - 1)) +
                           sizeof(GUID);

    ParametersBuffer = PciAllocatePool(PagedPool, ParametersBufferSize);
    if (ParametersBuffer == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    PciPrepareInputParametersForDsmMethod(ParametersBuffer,
                                          ParametersBufferSize,
                                          PCI_DSM_METHOD_BUS_CAPABILITIES,
                                          PCI_DSM_METHOD_BUS_CAPABILITIES_REV,
                                          &PCI_DSM_UUID);

    //
    // Initialize an output buffer.  This function returns a package of four
    // parameters.
    //

    OutputBuffer = (PACPI_EVAL_OUTPUT_BUFFER)StackBuffer;
    RtlZeroMemory(OutputBuffer, OBFF_OUTBUF_SIZE);

    //
    // Invoke a helper function to send an IOCTL to ACPI to evaluate this
    // control method.
    //

    Status = PciSendIoctlForDsmEvaluation(DeviceObject,
                                          ParametersBuffer,
                                          ParametersBufferSize,
                                          OutputBuffer,
                                          OBFF_OUTBUF_SIZE);

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    //
    // The first element in the return packate must be of type integer.
    //

    if (OutputBuffer->Argument[0].Type != ACPI_METHOD_ARGUMENT_INTEGER) {
        ERROR("BIOS supplied _DSM for bus capabilities but didn't return a package with an integer status");
        Status = STATUS_INVALID_PARAMETER_1;
        goto Exit;
    }

    //
    // Zero indicates that the method failed.
    //

    if (OutputBuffer->Argument[0].Data[0] == 0) {
        Status = STATUS_INVALID_PARAMETER_2;
        goto Exit;
    }

    //
    // Indexing into Argument[1] works here because Argument[0] was an integer,
    // which means that Argument[0] happens not to have any extra data appended
    // to it.
    //

    if (OutputBuffer->Argument[1].Type != ACPI_METHOD_ARGUMENT_BUFFER) {
        ERROR("BIOS supplied _DSM for bus capabilities but didn't return a package with a buffer in the second element");
        Status = STATUS_INVALID_PARAMETER_3;
        goto Exit;
    }

    if (OutputBuffer->Argument[1].DataLength < sizeof(*RetBuf)) {
        ERROR("BIOS supplied _DSM for bus capabilities that returned a buffer that was too small.");
        Status = STATUS_INVALID_PARAMETER_4;
        goto Exit;
    }

    RetBuf = (PPCI_FIRMWARE_BUS_CAPS_RETURN_BUFFER)OutputBuffer->Argument[1].Data;

    //
    // Zero here indicates that the method succeeded (go figure.)
    //

    if (RetBuf->Status != 0) {
        ERROR("BIOS supplied _DSM for bus capabilities returned a buffer with status set to Ones");
        Status = STATUS_INVALID_PARAMETER_5;
        goto Exit;
    }

    //
    // The firmware is supposed to say what kind of bus that it's talking about.
    // Only PCI (1) is currently defined.
    //

    if (RetBuf->Caps.Type != 1) {
        ERROR("BIOS supplied _DSM for bus capabilities for some bus type other than PCI");
        Status = STATUS_INVALID_PARAMETER_6;
        goto Exit;
    }

    //
    // This overly complicated firmware spec says that there's another length
    // embedded in here.
    //

    if (RetBuf->Caps.Length < sizeof(*BusCaps)) {
        ERROR("BIOS supplied _DSM for bus capabilities returned a buffer that says that its too small");
        Status = STATUS_INVALID_PARAMETER_7;
        goto Exit;
    }

    RtlCopyMemory(BusCaps,
                  &RetBuf->Caps,
                  sizeof(*BusCaps));

    Status = STATUS_SUCCESS;

Exit:

    if (!NT_SUCCESS(Status) && (Status != STATUS_INVALID_DEVICE_REQUEST)) {
        if (PCI_BUS_EXTENSION(DeviceObject->DeviceExtension)) {
            TraceEvents(((PPCI_BUS)DeviceObject->DeviceExtension)->BlackBoxHandle,
                        TRACE_LEVEL_WARNING,
                        DBG_ENUM,
                        "_DSM(OBFF) %!STATUS!",
                        Status);
        } else {
            PCI_ASSERT(PCI_DEVICE_EXTENSION(DeviceObject->DeviceExtension));
            TraceEvents(((PPCI_DEVICE)DeviceObject->DeviceExtension)->Parent->BlackBoxHandle,
                        TRACE_LEVEL_WARNING,
                        DBG_ENUM,
                        "(%x:%x) _DSM(OBFF) %!STATUS!",
                        ((PPCI_DEVICE)DeviceObject->DeviceExtension)->Slot.u.bits.DeviceNumber,
                        ((PPCI_DEVICE)DeviceObject->DeviceExtension)->Slot.u.bits.FunctionNumber,
                        Status);
        }
    }

    if (ParametersBuffer != NULL) {
        PciFreePool(ParametersBuffer);
    }

    return Status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciEvaluateDsmMethodForDelayOnSxResume (
    _In_ PDEVICE_OBJECT DeviceObject,
    _Out_ PBOOLEAN DelayOnSxResume
    )

/*++

Routine Description:

    This routine is invoked to evaluate a _DSM method defined in the scope
    of an Express root bus. It evaluates whether the PCI root bus requires 
    any additional delay before issuing the first config access to a 
    downstream device.
 
Arguments:

    DeviceObject - Supplies a pointer to the next lower device object.

    DelayOnSxResume - Out parameter to be filled in

Return Value:

    STATUS_INSUFFICIENT_RESOURCES on failure to allocate memory.

    STATUS_UNSUCCESSFUL if the output parameter from _DSM method evaluation
        is incorrectly formatted.

    Otherwise a status code returned from a function call.

--*/

{

    ACPI_EVAL_OUTPUT_BUFFER OutputBuffer;
    PACPI_EVAL_INPUT_BUFFER_COMPLEX ParametersBuffer;
    ULONG ParametersBufferSize;
    NTSTATUS Status;
    BOOLEAN Supported;

    PAGED_CODE();

    *DelayOnSxResume = TRUE;
    ParametersBuffer = NULL;

    //
    // This routine should only be called on root busses.
    //

    if ((PCI_BUS_EXTENSION(DeviceObject->DeviceExtension) == FALSE) || 
        (PCI_ROOT_BUS((PPCI_BUS)DeviceObject->DeviceExtension) == FALSE)) {

        PCI_ASSERT(FALSE);

        return STATUS_INVALID_PARAMETER;
    }

    //
    // Evaluating a _DSM method is a two-step process. In the first step check
    // if a DSM function for DelayOnSxResume is supported. If TRUE, then
    // evaluate this function and return an appropriate result to the caller.
    //

    Supported = FALSE;
    Status = PciIsDsmFunctionSupported(DeviceObject,
                                       PCI_DSM_METHOD_DELAY_ON_SX_RESUME,
                                       PCI_DSM_METHOD_DELAY_ON_SX_RESUME_REV,
                                       &Supported);

    if (!NT_SUCCESS(Status) || (Supported == FALSE)) {
        goto Exit;
    }

    //
    // Evaluate this method for real.
    //

    ParametersBufferSize = sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX) +
                           (sizeof(ACPI_METHOD_ARGUMENT) *
                            (DSM_METHOD_INPUT_ARGUMENTS_COUNT - 1)) +
                           sizeof(GUID);

    ParametersBuffer = PciAllocatePool(PagedPool, ParametersBufferSize);
    if (ParametersBuffer == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    PciPrepareInputParametersForDsmMethod(ParametersBuffer,
                                          ParametersBufferSize,
                                          PCI_DSM_METHOD_DELAY_ON_SX_RESUME,
                                          PCI_DSM_METHOD_DELAY_ON_SX_RESUME_REV,
                                          &PCI_DSM_UUID);

    //
    // Initialize an output buffer.
    //

    RtlZeroMemory(&OutputBuffer, sizeof(ACPI_EVAL_OUTPUT_BUFFER));

    //
    // Invoke a helper function to send an IOCTL to ACPI to evaluate this
    // control method.
    //

    Status = PciSendIoctlForDsmEvaluation(DeviceObject,
                                          ParametersBuffer,
                                          ParametersBufferSize,
                                          &OutputBuffer,
                                          sizeof(ACPI_EVAL_OUTPUT_BUFFER));

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    if (OutputBuffer.Argument[0].Type != ACPI_METHOD_ARGUMENT_INTEGER) {
        TraceEvents(((PPCI_BUS)DeviceObject->DeviceExtension)->BlackBoxHandle,
                    TRACE_LEVEL_WARNING,
                    DBG_ENUM,
                    "_DSM(DelayOnSxResume) returned a non-integer");

    } else if (OutputBuffer.Argument[0].Argument == 0) {

        //
        // The OS needs to manage the delay
        // 

        *DelayOnSxResume = TRUE;
    } else if (OutputBuffer.Argument[0].Argument == 1) {

        //
        // The FW manages the delay
        // 

        *DelayOnSxResume = FALSE;
    } else {

        //
        // According to the spec, this DSM should return either a 0 or a 1. 
        // On FRE builds, assume the delay is necessary, but catch non-compliant
        // FW on CHK builds.
        //

        PCI_ASSERT(FALSE);
        *DelayOnSxResume = TRUE;
    }

Exit:

    if (ParametersBuffer != NULL) {
        PciFreePool(ParametersBuffer);
    }

    return Status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciEvaluateDsmMethodForDelayOptimization (
    _In_ PDEVICE_OBJECT DeviceObject,
    _Out_ PULONG DelayInfo
    )

/*++

Routine Description:

    This routine is invoked to evaluate a _DSM method defined in the scope
    of a PCIe device. This method is defined in the PCI Firmware
    specification.  It defines delays that have been optimized for 
    the platform.

Arguments:

    DeviceObject - Supplies a pointer to the next lower device object.

    DelayInfo - Supplies a pointer to be filled in with the optimized delays.

Return Value:

    STATUS_INSUFFICIENT_RESOURCES on failure to allocate memory.

    STATUS_UNSUCCESSFUL if the output parameter from _DSM method evaluation
        is incorrectly formatted.

    Otherwise a status code returned from a function call.

--*/

{

#define DELAY_OUTPUT_BUF_SIZE (sizeof(ACPI_EVAL_OUTPUT_BUFFER) + (sizeof(ACPI_METHOD_ARGUMENT) * 5))
    ULONG PackageIndex;
    PACPI_EVAL_INPUT_BUFFER_COMPLEX ParametersBuffer;
    ULONG ParametersBufferSize;
    PACPI_EVAL_OUTPUT_BUFFER OutputBuffer;
    UCHAR StackBuffer[DELAY_OUTPUT_BUF_SIZE];
    NTSTATUS Status;
    BOOLEAN Supported;

    PAGED_CODE();
    ParametersBuffer = NULL;

    //
    // Evaluating a _DSM method is a two-step process. In the first step check
    // if a DSM function for DelayOptimization is supported. If TRUE, then evaluate this
    // function and return an appropriate result to the caller.
    //

    Supported = FALSE;
    Status = PciIsDsmFunctionSupported(DeviceObject,
                                       PCI_DSM_METHOD_DELAY_OPTIMIZATION,
                                       PCI_DSM_METHOD_DELAY_OPTIMIZATION_REV,
                                       &Supported);

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    if (Supported == FALSE) {
        Status = STATUS_NOT_SUPPORTED;
        goto Exit;
    }

    //
    // Evaluate this method for real.
    //

    ParametersBufferSize = sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX) +
                           (sizeof(ACPI_METHOD_ARGUMENT) *
                            (DSM_METHOD_INPUT_ARGUMENTS_COUNT - 1)) +
                           sizeof(GUID);

    ParametersBuffer = PciAllocatePool(PagedPool, ParametersBufferSize);
    if (ParametersBuffer == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    PciPrepareInputParametersForDsmMethod(ParametersBuffer,
                                          ParametersBufferSize,
                                          PCI_DSM_METHOD_DELAY_OPTIMIZATION,
                                          PCI_DSM_METHOD_DELAY_OPTIMIZATION_REV,
                                          &PCI_DSM_UUID);

    //
    // Initialize an output buffer.  This function returns a package of four
    // parameters.
    //

    OutputBuffer = (PACPI_EVAL_OUTPUT_BUFFER)StackBuffer;
    RtlZeroMemory(OutputBuffer, DELAY_OUTPUT_BUF_SIZE);

    //
    // Invoke a helper function to send an IOCTL to ACPI to evaluate this
    // control method.
    //

    Status = PciSendIoctlForDsmEvaluation(DeviceObject,
                                          ParametersBuffer,
                                          ParametersBufferSize,
                                          OutputBuffer,
                                          DELAY_OUTPUT_BUF_SIZE);

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    //
    // The return package must contain only integers.
    //

    for (PackageIndex = 0; PackageIndex < 5; PackageIndex++) {
        if (OutputBuffer->Argument[PackageIndex].Type != ACPI_METHOD_ARGUMENT_INTEGER) {
            if (PCI_BUS_EXTENSION(DeviceObject->DeviceExtension)) {
                TraceEvents(((PPCI_BUS)DeviceObject->DeviceExtension)->BlackBoxHandle,
                            TRACE_LEVEL_WARNING,
                            DBG_ENUM,
                            "_DSM(DelayOpt) returned a package containing non-integers");
            } else {
                PCI_ASSERT(PCI_DEVICE_EXTENSION(DeviceObject->DeviceExtension));
                TraceEvents(((PPCI_DEVICE)DeviceObject->DeviceExtension)->Parent->BlackBoxHandle,
                            TRACE_LEVEL_WARNING,
                            DBG_ENUM,
                            "(%x:%x) _DSM(DelayOpt) returned a package containing non-integers",
                            ((PPCI_DEVICE)DeviceObject->DeviceExtension)->Slot.u.bits.DeviceNumber,
                            ((PPCI_DEVICE)DeviceObject->DeviceExtension)->Slot.u.bits.FunctionNumber);
            }
            goto Exit;
        }
    }

    //
    // It is illegal to specify delays that are longer then the base spec. Ignore
    // any value that would increase the delay.
    //

    if (OutputBuffer->Argument[0].Argument <= DelayInfo[ResetTime]) {
        DelayInfo[ResetTime] = OutputBuffer->Argument[0].Argument;
    }

    if (OutputBuffer->Argument[1].Argument <= DelayInfo[DLUpTime]) {
        DelayInfo[DLUpTime] = OutputBuffer->Argument[1].Argument;
    }

    if (OutputBuffer->Argument[2].Argument <= DelayInfo[FLRResetTime]) {
        DelayInfo[FLRResetTime] = OutputBuffer->Argument[2].Argument;
    }

    if (OutputBuffer->Argument[3].Argument <= DelayInfo[D3hotToD0Time]) {
        DelayInfo[D3hotToD0Time] = OutputBuffer->Argument[3].Argument;
    }

    if (OutputBuffer->Argument[4].Argument <= DelayInfo[VFEnableTime]) {
        DelayInfo[VFEnableTime] = OutputBuffer->Argument[4].Argument;
    }

Exit:

    if (ParametersBuffer != NULL) {
        PciFreePool(ParametersBuffer);
    }

    return Status;
}

BOOLEAN
PciIsNonHotplugBridgeWithNoChildren(
    __in PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine determines whether the passed in device is a bridge that
    has no child and is not hotplug capable. If so, this bridge will not be
    enumerated to save bus numbers.

Arguments:

    Device - Provides the device to test.

Return Value:

    TRUE if the device is a not hotplug capable bridge with no child;
    FALSE otherwise.

--*/

{

    PPCI_BUS BusWithHint;
    PPCI_CHILDREN_BUSNUM_HINT ChildBusNumHint;
    PLIST_ENTRY Entry;
    BOOLEAN FoundHint;
    PPCI_BUS ParentBus;
    UCHAR Primary;
    UCHAR Secondary;
    UCHAR Subordinate;

    //
    // Only consider bridges.
    //

    if (Device->HeaderType != PCI_BRIDGE_TYPE) {
        return FALSE;
    }

    ParentBus = Device->Parent;
    if (ParentBus == NULL) {

        PCI_ASSERT(ParentBus != NULL);

        return FALSE;
    }

    Primary = (UCHAR)ParentBus->SecondaryBusNumber;

    //
    // Read the primary/secondary/subordinate bus numbers from the register,
    // if they are not 0, then this bridge has been configured, which means
    // it should be enumerated. Otherwise, check to see if there exists any
    // hint about whether this bridge should be enumerated.
    //

    PciReadConfigRegister(Device,
                          bridgeCommon.PrimaryBus,
                          &Primary);

    PciReadConfigRegister(Device,
                          bridgeCommon.SecondaryBus,
                          &Secondary);

    PciReadConfigRegister(Device,
                          bridgeCommon.SubordinateBus,
                          &Subordinate);

    FoundHint = FALSE;
    if (Secondary == 0 && Subordinate == 0) {
        Primary = (UCHAR)ParentBus->SecondaryBusNumber;

        //
        // The hints are created during PciSpreadBridgesHotplug, which indicates
        // the secondary and subordinate bus number that should be assigned to
        // each of the bridges. Note that the hint of the child bridge will
        // always be stored under its closest ancestor bus with hints.
        //

        BusWithHint = ParentBus;
        while (BusWithHint != NULL &&
               (IsListEmpty(&BusWithHint->ChildrenBusNumHints))) {

            BusWithHint = BusWithHint->ParentBus;
        }

        //
        // If no hint is found, then the bridge might not have been configured
        // yet, return FALSE to create the PDO and wait for the bridge to be
        // configured later.
        //

        if (BusWithHint == NULL) {
            return FALSE;
        }

        //
        // Look for hints.
        //

        for (Entry = BusWithHint->ChildrenBusNumHints.Flink;
             Entry != &BusWithHint->ChildrenBusNumHints;
             Entry = Entry->Flink) {

            ChildBusNumHint = CONTAINING_RECORD(Entry,
                                                PCI_CHILDREN_BUSNUM_HINT,
                                                ListEntry);

            if (ChildBusNumHint->BusNumber != Primary ||
                ChildBusNumHint->Slot.u.AsULONG != Device->Slot.u.AsULONG) {
                continue;
            }

            FoundHint = TRUE;
            Secondary = ChildBusNumHint->Secondary;
            Subordinate = ChildBusNumHint->Subordinate;
            break;
        }

        if (Secondary != 0 || Subordinate != 0) {

            //
            // If the hint indicates that the bridge should be assigned some
            // nonzero secondary and subordinate bus number, this means the
            // bridge should be enumerated.
            //

            return FALSE;
        } else if (FoundHint != FALSE) {

            //
            // For the case where the bridges are already configured
            // (given that the hints are created), but the hint indicates that
            // the secondary and subordinate bus number for this bridge are
            // both 0, this means the bridge is determined to have no child
            // nor is it hotplugable - just return TRUE to ignore it.
            //

            return TRUE;
        } else {

            //
            // If no hint data structure is found, this means the bridge is not
            // yet configured. Just enumerate it.
            //

            return FALSE;
        }

    } else {
        return FALSE;
    }
}
