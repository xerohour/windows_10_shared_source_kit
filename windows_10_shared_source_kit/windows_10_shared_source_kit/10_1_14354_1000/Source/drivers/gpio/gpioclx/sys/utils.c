/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    utils.c

Abstract:

    This file implements all the utility routines used by the GPIO class extension.


Environment:

    Kernel mode

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "pch.h"
#include <string.h>
#include <secintr.h>
#include "trace.h"
#include <acpi\aml.h>

#ifdef EVENT_TRACING
#include "utils.tmh"
#endif

//
// -------------------------------------------------------------------- Defines
//

#define ENUM_CHILDREN_MINIMUM_SIZE (sizeof(ACPI_ENUM_CHILDREN_OUTPUT_BUFFER))
#define INITIAL_CONTROL_METHOD_OUTPUT_SIZE (0x200) // 512B
#define PIN_PRINT_BUFFER_SIZE (128)

// {4F248F40-D5E2-499F-834C-27758EA1CD3F}
DEFINE_GUID(GPIO_CONTROLLER_DSM_GUID,
            0x4F248F40,
            0xD5E2, 0x499F,
            0x83, 0x4C, 0x27, 0x75, 0x8E, 0xA1, 0xCD, 0x3F);

//
// ----------------------------------------------------------------- Prototypes
//

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopConvertAsciiStringToInteger (
    __in_bcount(Length) PCSTR String,
    __in ULONG Length,
    __in ULONG Base,
    __out PULONG Value
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
ACCESS_MASK
GpioUtilGetAccessMaskFromRequest (
    __in WDF_REQUEST_PARAMETERS *RequestParameters
    );

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpioUtilTracePrintPinNumbersEx (
    __in ULONG DebugPrintLevel,
    __in PGPIO_BANK_ENTRY GpioBank,
    __in_ecount(PinCount) PPIN_NUMBER PinNumberTable,
    __in ULONG PinCount
    );

//
// -------------------------------------------------------------------- Pragmas
//

#pragma alloc_text(PAGE, GpiopConvertAsciiStringToInteger)
#pragma alloc_text(PAGE, GpioUtilEnumerateNamespaceChildren)
#pragma alloc_text(PAGE, GpioUtilEvaluateAcpiMethod)
#pragma alloc_text(PAGE, GpioUtilGetOpenModeFromRequest)
#pragma alloc_text(PAGE, GpioUtilGetAccessMaskFromRequest)
#pragma alloc_text(PAGE, GpioUtilGetResourceName)
#pragma alloc_text(PAGE, GpioUtilGetIdFromFileName)
#pragma alloc_text(PAGE, GpioUtilIsBiosGpioDescriptorValid)
#pragma alloc_text(PAGE, GpioUtilIsBiosFunctionConfigDescriptorValid)
#pragma alloc_text(PAGE, GpioUtilIsChildObjectDefined)
#pragma alloc_text(PAGE, GpioUtilIsDsmFunctionSupported)
#pragma alloc_text(PAGE, GpioUtilPrepareInputParametersForDsmMethod)
#pragma alloc_text(PAGE, GpioUtilParseNamespaceEventPins)
#pragma alloc_text(PAGE, GpioUtilQueryRegistryFlags)
#pragma alloc_text(PAGE, GpioUtilTracePrintPinNumbers)
#pragma alloc_text(PAGE, GpioUtilTracePrintPinNumbersEx)

//
// ------------------------------------------------------------------ Functions
//

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpiopConvertAsciiStringToInteger (
    __in_bcount(Length) PCSTR String,
    __in ULONG Length,
    __in ULONG Base,
    __out PULONG Value
    )

/*++

Routine Description:

    This routine converts string to integer. Most of the routine has been lifted
    from RtlUnicodeStringToInteger(). The problem with using
    RtlUnicodeStringToInteger() is that it does not fail even when passing in
    bad strings (e.g. "LM"). It returns a value of 0x0 in such cases but
    unfortunately that is valid in GPIO case.

    N.B. Doesn't deal with leading or trailing spaces OR +/- prefix.

Arguments:

    String - Supplies the ASCII string to convert.

    Length - Supplies the length of the string (including NULL).

    Base - Supplies the base for input string.

    Value - Supplies a pointer that returns the converted value.

Return Value:

    STATUS_SUCCESS if the conversion was successful (i.e. all characters)
        could be converted.

    STATUS_INVALID_PARAMETER otherwise.

--*/

{

    CHAR Character;
    ULONG Count;
    ULONG Digit;
    ULONG Result;
    ULONG Shift;
    NTSTATUS Status;

    PAGED_CODE();

    Result = 0;
    if (Base == 16) {
        Shift = 4;

    } else if (Base == 10) {
        Shift = 0;

    } else {
        Status = STATUS_INVALID_PARAMETER;
        goto ConvertUnicodeStringToIntegerEnd;
    }

    Status = STATUS_SUCCESS;
    for (Count = 0; Count < Length - sizeof(CHAR); Count += 1) {
        Character = String[Count];
        if (Character >= L'0' && Character <= L'9') {
            Digit = Character - L'0';
        } else if (Character >= L'A' && Character <= L'F') {
            Digit = Character - L'A' + 10;
        } else if (Character >= L'a' && Character <= L'f') {
            Digit = Character - L'a' + 10;
        } else {
            Status = STATUS_INVALID_PARAMETER;
            break;
        }

        if (Digit >= Base) {
            Status = STATUS_INVALID_PARAMETER;
            break;
        }

        if (Shift == 0) {
            Result = (Base * Result) + Digit;
        } else {
            Result = (Result << Shift) | Digit;
        }
    }

ConvertUnicodeStringToIntegerEnd:
    *Value = Result;
    return Status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilEnumerateNamespaceChildren (
    __in PDEVICE_EXTENSION GpioExtension,
    __deref_out_bcount(*ReturnBufferSize)
        PACPI_ENUM_CHILDREN_OUTPUT_BUFFER *ReturnBuffer,

    __out_opt PULONG ReturnBufferSize
    )

/*++

Routine Description:

    This routine populates a list of names for all the children in the ACPI
    namespace. This routine only requests ACPI evaluate the immediate children.

Arguments:

    Device - Supplies a handle to the framework device object.

    ReturnBuffer - Supplies a pointer to a variable that receives the
        buffer address containing ACPI children names. The caller is responsible
        for freeing the buffer once it is done. The buffer is allocated from
        paged pool.

    ReturnBufferSize - Supplies a variable that receives the buffer size.

Return Value:

    NTSTATUS code.

--*/

{

    WDFDEVICE Device;
    ACPI_ENUM_CHILDREN_INPUT_BUFFER InputBuffer;
    WDF_MEMORY_DESCRIPTOR InputDescriptor;
    WDFIOTARGET IoTarget;
    PACPI_ENUM_CHILDREN_OUTPUT_BUFFER OutputBuffer;
    WDF_MEMORY_DESCRIPTOR OutputDescriptor;
    ACPI_ENUM_CHILDREN_OUTPUT_BUFFER OutputSizeBuffer;
    ULONG RequiredSize;
    ULONG_PTR SizeReturned;
    NTSTATUS  Status;

    PAGED_CODE();

    //
    // Set the IO target. The IO target is the default underlying device object,
    // which is ACPI in this case.
    //

    Device = GpioExtension->Device;
    IoTarget = WdfDeviceGetIoTarget(Device);

    //
    // Initialize input and output arguments for IOCTL evaluation.
    //

    RtlZeroMemory(&InputBuffer, sizeof(InputBuffer));
    InputBuffer.Signature = ACPI_ENUM_CHILDREN_INPUT_BUFFER_SIGNATURE;
    InputBuffer.Flags = ENUM_CHILDREN_IMMEDIATE_ONLY;
    RtlZeroMemory(&OutputSizeBuffer, sizeof(OutputSizeBuffer));

    //
    // Initialize the input and output buffer descriptors.
    //

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&InputDescriptor,
                                      (PVOID)&InputBuffer,
                                      sizeof(InputBuffer));

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&OutputDescriptor,
                                      (PVOID)&OutputSizeBuffer,
                                      sizeof(OutputSizeBuffer));

    //
    // Send the IOCTL the first time to determine the size of the output
    // buffer needed.
    //

    Status = WdfIoTargetSendIoctlSynchronously(IoTarget,
                                               NULL,
                                               IOCTL_ACPI_ENUM_CHILDREN,
                                               &InputDescriptor,
                                               &OutputDescriptor,
                                               NULL,
                                               &SizeReturned);

    //
    // If the status is not STATUS_BUFFER_OVERFLOW, then something failed.
    // Simply bail out.
    //

    if (Status != STATUS_BUFFER_OVERFLOW) {

        GPIO_ASSERT(NT_SUCCESS(Status) == FALSE);

        TraceEvents(GpioExtension->LogHandle,
                    Error,
                    DBG_ACPIEVT,
                    "%s: Unexpected return value returned from ACPI driver! "
                    "Status = %#x.\n",
                    __FUNCTION__,
                    Status);

        Status = STATUS_ACPI_INVALID_DATA;
        goto EnumerateNamespaceChildrenEnd;
    }

    //
    // There should be at least one child, since the GPIO controller itself
    // will be returned as the first child. If there are no children, then
    // something is wrong and thus simply bail out.
    //
    // N.B.The NumberOfChildren returned by ACPI actually contains the
    //     required size when the status returned is STATUS_BUFFER_OVERFLOW.
    //

    if ((OutputSizeBuffer.Signature != ACPI_ENUM_CHILDREN_OUTPUT_BUFFER_SIGNATURE) ||
        (OutputSizeBuffer.NumberOfChildren < ENUM_CHILDREN_MINIMUM_SIZE)) {

        Status = STATUS_ACPI_INVALID_DATA;
        TraceEvents(GpioExtension->LogHandle,
                    Error,
                    DBG_ACPIEVT,
                    "%s: Unrecognized output buffer returned from ACPI driver! "
                    "Signature = %#x, Size = %#x, Status = %#x.\n",
                    __FUNCTION__,
                    OutputSizeBuffer.Signature,
                    OutputSizeBuffer.NumberOfChildren,
                    Status);

        goto EnumerateNamespaceChildrenEnd;
    }

    //
    // Allocate a buffer to hold all the children.
    //

    RequiredSize = OutputSizeBuffer.NumberOfChildren;
    OutputBuffer = (PACPI_ENUM_CHILDREN_OUTPUT_BUFFER)
                   GPIO_ALLOCATE_POOL(PagedPool, RequiredSize);

    if (OutputBuffer == NULL) {
        Status = STATUS_NO_MEMORY;
        TraceEvents(GpioExtension->LogHandle,
                    Error,
                    DBG_ACPIEVT,
                    "%s: Failed to allocate memory for ACPI output buffer! "
                    "Status = %#x.\n",
                    __FUNCTION__,
                    Status);

        goto EnumerateNamespaceChildrenEnd;
    }

    //
    // Initialize the output buffer and the WDF descriptor.
    //

    RtlZeroMemory(OutputBuffer, RequiredSize);
    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&OutputDescriptor,
                                      (PVOID)OutputBuffer,
                                      RequiredSize);

    Status = WdfIoTargetSendIoctlSynchronously(IoTarget,
                                               NULL,
                                               IOCTL_ACPI_ENUM_CHILDREN,
                                               &InputDescriptor,
                                               &OutputDescriptor,
                                               NULL,
                                               &SizeReturned);

    if (!NT_SUCCESS(Status)) {
        TraceEvents(GpioExtension->LogHandle,
                    Error,
                    DBG_ACPIEVT,
                    "%s: IOCTL_ACPI_ENUM_CHILDREN failed! Status = %#x.\n",
                    __FUNCTION__,
                    Status);

        GPIO_FREE_POOL(OutputBuffer);
        OutputBuffer = NULL;

        goto EnumerateNamespaceChildrenEnd;
    }

    if ((OutputBuffer->Signature != ACPI_ENUM_CHILDREN_OUTPUT_BUFFER_SIGNATURE) ||
        (OutputBuffer->NumberOfChildren == 0) ||
        (SizeReturned != RequiredSize)) {

        TraceEvents(GpioExtension->LogHandle,
                    Error,
                    DBG_ACPIEVT,
                    "%s: Unrecognized output buffer returned from ACPI driver! "
                    "Signature = %#x, Children = %#x, Size = %#x, Reqd = %#x.\n",
                    __FUNCTION__,
                    OutputBuffer->Signature,
                    OutputBuffer->NumberOfChildren,
                    (ULONG)SizeReturned,
                    RequiredSize);

        GPIO_FREE_POOL(OutputBuffer);
        OutputBuffer = NULL;

        Status = STATUS_ACPI_INVALID_DATA;
        goto EnumerateNamespaceChildrenEnd;
    }

    *ReturnBuffer = OutputBuffer;
    if (ARGUMENT_PRESENT(ReturnBufferSize) != FALSE) {
        *ReturnBufferSize = (ULONG)SizeReturned;
    }

EnumerateNamespaceChildrenEnd:
    return Status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilEvaluateAcpiMethod (
    __in WDFDEVICE Device,
    __in ULONG MethodName,
    __in_opt PVOID EvalInputBuffer,
    __deref_out_bcount(*ReturnBufferSize) PVOID *ReturnBuffer,
    __out_opt PULONG ReturnBufferSize
    )

/*

Routine Description:

    This function sends an IRP to ACPI to evaluate a method.

    We know that ACPI must be in the device stack (either as a bus or filter
    driver) since GPIO Controllers are always defined in the ACPI namespace.

Arguments:

    Device - Supplies a handle to the framework device object.

    MethodName - Supplies a packed string identifying the method.

    InputBuffer - Supplies arguments for the method. If specified, the method
        name must match MethodName.

    ReturnBuffer - Supplies a pointer to receive the return value(s) from
        the method.

    ReturnBufferSize - Supplies a pointer to receive the size of the data
        returned.

Return Value:

    NTSTATUS code.

--*/

{

    UCHAR Attempts;
    PACPI_EVAL_INPUT_BUFFER InputBuffer;
    ULONG InputBufferLength;
    WDF_MEMORY_DESCRIPTOR InputDescriptor;
    WDFIOTARGET IoTarget;
    PACPI_EVAL_OUTPUT_BUFFER OutputBuffer;
    ULONG OutputBufferLength;
    WDF_MEMORY_DESCRIPTOR OutputDescriptor;
    ULONG_PTR SizeReturned;
    ACPI_EVAL_INPUT_BUFFER SmallInputBuffer;
    NTSTATUS Status;

    PAGED_CODE();

    OutputBuffer = NULL;

    //
    // Build input buffer if one was not passed in.
    //

    if (EvalInputBuffer == NULL) {
        if (MethodName == 0) {
            Status = STATUS_INVALID_PARAMETER_1;
            goto EvaluateAcpiMethodEnd;
        }

        SmallInputBuffer.Signature = ACPI_EVAL_INPUT_BUFFER_SIGNATURE;
        SmallInputBuffer.MethodNameAsUlong = MethodName;

        InputBuffer = &SmallInputBuffer;
        InputBufferLength = sizeof(ACPI_EVAL_INPUT_BUFFER);

    } else {

        InputBuffer = EvalInputBuffer;

        //
        // Calculate input buffer size.
        //

        switch(((PACPI_EVAL_INPUT_BUFFER)EvalInputBuffer)->Signature) {
        case ACPI_EVAL_INPUT_BUFFER_SIGNATURE:
            InputBufferLength = sizeof(ACPI_EVAL_INPUT_BUFFER);
            break;

        case ACPI_EVAL_INPUT_BUFFER_SIMPLE_INTEGER_SIGNATURE:
            InputBufferLength = sizeof(ACPI_EVAL_INPUT_BUFFER_SIMPLE_INTEGER);
            break;

        case ACPI_EVAL_INPUT_BUFFER_SIMPLE_STRING_SIGNATURE:
            InputBufferLength = sizeof(ACPI_EVAL_INPUT_BUFFER_SIMPLE_STRING) +
                ((PACPI_EVAL_INPUT_BUFFER_SIMPLE_STRING)EvalInputBuffer)->StringLength - 1;
            break;

        case ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE:
            InputBufferLength = ((PACPI_EVAL_INPUT_BUFFER_COMPLEX)EvalInputBuffer)->Size;
            break;

        default:
            Status = STATUS_INVALID_PARAMETER_2;
            goto EvaluateAcpiMethodEnd;
        }
    }

    //
    // Set the IO target and initial size for the output buffer to be allocated.
    // The IO target is the default underlying device object, which is ACPI
    // in this case.
    //

    Attempts = 0;
    IoTarget = WdfDeviceGetIoTarget(Device);
    OutputBufferLength = INITIAL_CONTROL_METHOD_OUTPUT_SIZE;

    //
    // Set the input buffer.
    //

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&InputDescriptor,
                                      (PVOID)InputBuffer,
                                      InputBufferLength);

    do {

        OutputBuffer = GPIO_ALLOCATE_POOL(PagedPool, OutputBufferLength);
        if (OutputBuffer == NULL) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto EvaluateAcpiMethodEnd;
        }

        WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(&OutputDescriptor,
                                          (PVOID)OutputBuffer,
                                          OutputBufferLength);

        Status = WdfIoTargetSendIoctlSynchronously(IoTarget,
                                                   NULL,
                                                   IOCTL_ACPI_EVAL_METHOD,
                                                   &InputDescriptor,
                                                   &OutputDescriptor,
                                                   NULL,
                                                   &SizeReturned);

        //
        // If the output buffer was insufficient, then re-allocate one with
        // appropriate size and retry.
        //

        if (Status == STATUS_BUFFER_OVERFLOW) {
            OutputBufferLength = OutputBuffer->Length;
            GPIO_FREE_POOL(OutputBuffer);
            OutputBuffer = NULL;
        }

        Attempts += 1;

    } while ((Status == STATUS_BUFFER_OVERFLOW) && (Attempts < 2));

    //
    // If successful and data returned, return data to caller. If the method
    // returned no data, then set the return values to NULL.
    //

    if (NT_SUCCESS(Status)) {
        if (SizeReturned > 0) {

            GPIO_ASSERT(SizeReturned >=
                        sizeof(ACPI_EVAL_OUTPUT_BUFFER) - sizeof(ACPI_METHOD_ARGUMENT));

            GPIO_ASSERT(OutputBuffer->Signature == ACPI_EVAL_OUTPUT_BUFFER_SIGNATURE);

            *ReturnBuffer = OutputBuffer;
            if (ARGUMENT_PRESENT(ReturnBufferSize) != FALSE) {
                *ReturnBufferSize = (ULONG)SizeReturned;
            }

        } else {
            *ReturnBuffer = NULL;
            if (ARGUMENT_PRESENT(ReturnBufferSize) != FALSE) {
                *ReturnBufferSize = 0;
            }
        }
    }

    //
    // If the method execution failed, then free up the resources.
    //

EvaluateAcpiMethodEnd:
    if (!NT_SUCCESS(Status) && (OutputBuffer != NULL)) {
        GPIO_FREE_POOL(OutputBuffer);
    }

    return Status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilIsDsmFunctionSupported (
    __in WDFDEVICE Device,
    __in ULONG FunctionIndex,
    __in ULONG FunctionRevision,
    __out PBOOLEAN Supported
    )

/*++

Routine Description:

    This routine is invoked to check if a specific function for a _DSM method
    is supported.

    It will bugcheck if the _DSM method exists but returns malformed output.

Arguments:

    Device - Supplies a handle to the framework device object.

    FunctionIndex - Supplies the function index to check for.

    FunctionRevision - Indicates the revision of the function of interest.

    Supported - Supplies a pointer to a boolean that on return will indicate
        if the given function is supported.

Return Value:

    STATUS_OBJECT_NAME_NOT_FOUND if the _DSM method does not exist. This status
        may be returned for other reasons as well.

    STATUS_INSUFFICIENT_RESOURCES on failure to allocate memory.

    STATUS_SUCCESS if the method evaluates correctly and the output parameter
        if formatted correctly.

    Otherwise a status code from a function call.

--*/

{

    PACPI_EVAL_OUTPUT_BUFFER OutputBuffer;
    ULONG OutputBufferSize;
    PACPI_EVAL_INPUT_BUFFER_COMPLEX ParametersBuffer;
    NTSTATUS Status;

    PAGED_CODE();

    OutputBuffer = NULL;
    ParametersBuffer = NULL;

    *Supported = FALSE;

    //
    // When querying, the query is phrased in terms of "give me an array of bits
    // where each bit represents a function for which there is support at this
    // revision level."  I.e. "Revision 1" could return an entirely disjoint
    // bit field than "Revision 2."
    //

    Status = GpioUtilPrepareInputParametersForDsmMethod(
                GPIO_CONTROLLER_DSM_QUERY_FUNCTION_INDEX,
                FunctionRevision,
                &ParametersBuffer,
                NULL);

    if (!NT_SUCCESS(Status)) {
        goto IsDsmFunctionSupportedEnd;
    }

    //
    // Invoke a helper function to send an IOCTL to ACPI to evaluate this
    // control method.
    //

    Status = GpioUtilEvaluateAcpiMethod(Device,
                                        ParametersBuffer->MethodNameAsUlong,
                                        ParametersBuffer,
                                        &OutputBuffer,
                                        &OutputBufferSize);

    //
    // N.B. If _DSM does not exist, STATUS_OBJECT_NAME_NOT_FOUND is returned
    // here.
    //

    if (!NT_SUCCESS(Status)) {
        goto IsDsmFunctionSupportedEnd;
    }

    //
    // Exactly one Buffer must be returned.
    //

    if ((OutputBuffer == NULL) || (OutputBuffer->Count != 1)) {
        KeBugCheckEx(GPIO_CONTROLLER_DRIVER_ERROR,
                     DSM_OUTPUT_MALFORMED,
                     (ULONG_PTR)GPIO_CONTROLLER_DSM_REVISION,
                     (ULONG_PTR)GPIO_CONTROLLER_DSM_QUERY_FUNCTION_INDEX,
                     1);
    }

    //
    // The Buffer must contain at least one bit. Since we are returned bytes,
    // rounding up implies that it must contain at least one byte.
    //

    GPIO_ASSERT(OutputBufferSize >= sizeof(ACPI_EVAL_OUTPUT_BUFFER));

    if ((OutputBuffer->Argument[0].Type != ACPI_METHOD_ARGUMENT_BUFFER ||
        (OutputBuffer->Argument[0].DataLength == 0))) {

       KeBugCheckEx(GPIO_CONTROLLER_DRIVER_ERROR,
                    DSM_OUTPUT_MALFORMED,
                    (ULONG_PTR)GPIO_CONTROLLER_DSM_REVISION,
                    (ULONG_PTR)GPIO_CONTROLLER_DSM_QUERY_FUNCTION_INDEX,
                    2);
    }

    Status = STATUS_SUCCESS;

    if ((FunctionIndex / 8) < OutputBuffer->Argument[0].DataLength) {
        if ((OutputBuffer->Argument[0].Data[FunctionIndex / 8] &
             (1 << (FunctionIndex % 8))) != 0) {

            *Supported = TRUE;
        }
    }

IsDsmFunctionSupportedEnd:
    if (ParametersBuffer != NULL) {
        GPIO_FREE_POOL(ParametersBuffer);
    }

    if (OutputBuffer != NULL) {
        GPIO_FREE_POOL(OutputBuffer);
    }

    return Status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilPrepareInputParametersForDsmMethod (
    __in ULONG FunctionIndex,
    __in ULONG FunctionRevision,
    __deref_out_bcount(*ReturnBufferSize) PACPI_EVAL_INPUT_BUFFER_COMPLEX *ReturnBuffer,
    __out_opt PULONG ReturnBufferSize
    )

/*++

Routine Description:

    This routine is invoked for initializing an input parameter blob
    for evaluation of a _DSM control method defined in the scope of
    a GPIO controller.

Arguments:

    FunctionIndex - Supplies the value of a function index for an input
        parameter.

    FunctionRevision - Supplies the version of the function.

    ReturnBuffer - Supplies a pointer to receive the input parameter blob.

    ReturnBufferSize - Supplies a pointer to receive the size of the data
        blob returned.

Return Value:

    NTSTATUS code.

--*/

{

    PACPI_METHOD_ARGUMENT Argument;
    PACPI_EVAL_INPUT_BUFFER_COMPLEX ParametersBuffer;
    ULONG ParametersBufferSize;

    PAGED_CODE();

    ParametersBufferSize = sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX) +
                           (sizeof(GUID) - sizeof(ULONG)) +
                           (sizeof(ACPI_METHOD_ARGUMENT) *
                            (GPIO_CONTROLLER_DSM_METHOD_ARGUMENTS_COUNT - 1));

    ParametersBuffer = GPIO_ALLOCATE_POOL(PagedPool, ParametersBufferSize);
    if (ParametersBuffer == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlZeroMemory(ParametersBuffer, ParametersBufferSize);
    ParametersBuffer->Signature = ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE;
    ParametersBuffer->MethodNameAsUlong = (ULONG)'MSD_';
    ParametersBuffer->Size = ParametersBufferSize;
    ParametersBuffer->ArgumentCount = GPIO_CONTROLLER_DSM_METHOD_ARGUMENTS_COUNT;

    //
    // Argument 0: UUID.
    //

    Argument = &ParametersBuffer->Argument[0];
    ACPI_METHOD_SET_ARGUMENT_BUFFER(Argument, &GPIO_CONTROLLER_DSM_GUID, sizeof(GUID));

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
    //             for GPIO.
    //

    Argument = ACPI_METHOD_NEXT_ARGUMENT(Argument);
    Argument->Type = ACPI_METHOD_ARGUMENT_PACKAGE;
    Argument->DataLength = sizeof(ULONG);
    Argument->Argument = 0;

    GPIO_ASSERT((PVOID)ACPI_METHOD_NEXT_ARGUMENT(Argument) ==
                Add2Ptr(ParametersBuffer, ParametersBufferSize));

    *ReturnBuffer = ParametersBuffer;
    if (ARGUMENT_PRESENT(ReturnBufferSize) != FALSE) {
        *ReturnBufferSize = ParametersBufferSize;
    }

    return STATUS_SUCCESS;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilGetResourceName (
    __in_bcount(FIELD_OFFSET(RH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER,
                             ConnectionProperties) +
                RhDescriptor->PropertiesLength)
        PRH_QUERY_CONNECTION_PROPERTIES_OUTPUT_BUFFER RhDescriptor,
    __out __drv_at(BiosName->Buffer, __drv_allocatesMem(Mem))
        PUNICODE_STRING BiosName
    )

/*++

Routine Description:

    This routine cracks the supplied descriptor and returns the resource
    name specified within the descriptor.

Arguments:

    RhDescriptor - Supplies the GPIO interrupt descriptor to be translated.

    BiosName - Supplies a pointer to a variable that receives a newly
        allocated UNICODE_STRING buffer containing the BIOS name. The
        caller is responsible for freeing the buffer.

Return Value:

    NTSTATUS code.

--*/

{

    ANSI_STRING AnsiBiosName;
    size_t AnsiBiosNameLength;
    ULONG BiosNameSize;
    PPNP_GPIO_INTERRUPT_IO_DESCRIPTOR InterruptDescriptor;
    USHORT Length;
    PCHAR LocalBiosName;
    ULONG RemainingLength;
    NTSTATUS Status;
    UCHAR Type;
    BOOLEAN Valid;

    PAGED_CODE();

    AnsiBiosNameLength = 0;
    RtlZeroMemory(BiosName, sizeof(*BiosName));
    LocalBiosName = NULL;

    if (RhDescriptor->PropertiesLength > MAX_USHORT) {
        Status = STATUS_INVALID_BUFFER_SIZE;
        goto GetResourceNameEnd;
    }

    Length = (USHORT)RhDescriptor->PropertiesLength;

    //
    // Get the descriptor type.
    //

    Status = STATUS_UNSUCCESSFUL;
    if (Length < sizeof(UCHAR)) {
        Status = STATUS_INVALID_BUFFER_SIZE;
        goto GetResourceNameEnd;
    }

    Type = *(PUCHAR)(RhDescriptor->ConnectionProperties);
    switch (Type) {

    case GPIO_INTERRUPT_IO_DESCRIPTOR:
        InterruptDescriptor = (PPNP_GPIO_INTERRUPT_IO_DESCRIPTOR)
                              RhDescriptor->ConnectionProperties;

        Valid = GpioUtilIsBiosGpioDescriptorValid(InterruptDescriptor, Length);
        if (Valid == FALSE) {
            Status = STATUS_UNSUCCESSFUL;
            goto GetResourceNameEnd;
        }

        NT_ASSERT(InterruptDescriptor->ResourceSourceOffset < Length);
        NT_ASSERT(InterruptDescriptor->VendorDataOffset <= Length);

        RemainingLength = InterruptDescriptor->VendorDataOffset -
                              InterruptDescriptor->ResourceSourceOffset;

        LocalBiosName = Add2Ptr(InterruptDescriptor,
                                InterruptDescriptor->ResourceSourceOffset);

        Status = RtlStringCbLengthA(LocalBiosName,
                                    RemainingLength,
                                    &AnsiBiosNameLength);

        break;

    default:
        Status = STATUS_NOT_SUPPORTED;
        break;
    }

    if (!NT_SUCCESS(Status)) {
        goto GetResourceNameEnd;
    }

    if (AnsiBiosNameLength > UNICODE_STRING_MAX_CHARS) {
        Status = STATUS_INVALID_BUFFER_SIZE;
        goto GetResourceNameEnd;
    }

    AnsiBiosName.Buffer = LocalBiosName;
    AnsiBiosName.MaximumLength = (USHORT)AnsiBiosNameLength;;
    AnsiBiosName.Length = (USHORT)AnsiBiosNameLength;

    //
    // The BIOS name is accessed at DIRQL level (by the GPIO hub) and hence
    // the memory is allocated from non-paged pool.
    //

    BiosNameSize = RtlAnsiStringToUnicodeSize(&AnsiBiosName);
    NT_ASSERT(BiosNameSize <= UNICODE_STRING_MAX_BYTES);
    BiosName->MaximumLength = (USHORT)BiosNameSize;
    BiosName->Buffer = GPIO_ALLOCATE_POOL(NonPagedPoolNx,
                                          BiosName->MaximumLength);

    if (BiosName->Buffer == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        BiosName->MaximumLength = 0;
        goto GetResourceNameEnd;
    }

    Status = RtlAnsiStringToUnicodeString(BiosName, &AnsiBiosName, FALSE);
    if (!NT_SUCCESS(Status)) {
        goto GetResourceNameEnd;
    }

GetResourceNameEnd:
    if (!NT_SUCCESS(Status)) {
        if (BiosName->Buffer != NULL) {
            GPIO_FREE_POOL(BiosName->Buffer);
            RtlZeroMemory(BiosName, sizeof(*BiosName));
        }
    }

    return Status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
ACCESS_MASK
GpioUtilGetAccessMaskFromRequest (
    __in WDF_REQUEST_PARAMETERS *RequestParameters
    )

/*++

Routine Description:

    This routine obtains the generic right bits (bits 28 to 31) in the access
    mask based on the other bits (bits 0 to 27) in it.

Arguments:

    RequestParameters - Supplies a pointer to a structure containing the
        WDF request parameters.

Return Value:

    Access_MASK - In the returned Access mask, only generic rights bits (bits
        28 to 31) may be set as 1. All other bits are 0s.

--*/

{

    ACCESS_MASK Access;
    ACCESS_MASK DesiredAccess;
    PGENERIC_MAPPING GenericMapping;

    PAGED_CODE();

    Access = 0;
    DesiredAccess =
        RequestParameters->Parameters.Create.SecurityContext->DesiredAccess;

    GenericMapping = IoGetFileObjectGenericMapping();
    if ((GenericMapping->GenericRead & ~DesiredAccess) == 0) {
        Access |= GENERIC_READ;
    }

    if ((GenericMapping->GenericWrite & ~DesiredAccess) == 0) {
        Access |= GENERIC_WRITE;
    }

    if ((GenericMapping->GenericExecute & ~DesiredAccess) == 0) {
        Access |= GENERIC_EXECUTE;
    }

    if ((GenericMapping->GenericAll & ~DesiredAccess) == 0) {
        Access |= GENERIC_ALL;
    }

    return Access;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
GPIO_CONNECT_IO_PINS_MODE
GpioUtilGetOpenModeFromRequest (
    __in WDF_REQUEST_PARAMETERS *RequestParameters,
    __in UCHAR IoRestriction,
    __out PBOOLEAN ReadWriteMode
    )

/*++

Routine Description:

    This routine obtains the mode in which the GPIO pins are being opened
    based on the WDF request parameters.

Arguments:

    RequestParameters - Supplies a pointer to a structure containing the
        WDF request parameters.

    IoRestriction - Supplies the IO restriction value from the BIOS descriptor.

    ReadWriteMode - Supplies a pointer that receives whether the pin is
        being opened in read-write mode (TRUE) or not (FALSE).

Return Value:

    ULONG - GPIO connect mode (input/output or invalid).

--*/

{

    ACCESS_MASK AccessType;
    ULONG ConnectMode;

    PAGED_CODE();

    ConnectMode = ConnectModeInvalid;
    *ReadWriteMode = FALSE;
    AccessType = GpioUtilGetAccessMaskFromRequest(RequestParameters);
    if ((AccessType & ~(GENERIC_READ | GENERIC_WRITE)) != 0) {
        goto GetOpenModeFromRequestEnd;
    }

    //
    // If the pins are being opened for either read (input) or write (output),
    // then set the appropriate connect mode. Opening the pins for both
    // read and write is not supported.
    //

    if (((AccessType & GENERIC_READ) != 0) &&
        ((AccessType & GENERIC_WRITE) == 0)) {

        ConnectMode = ConnectModeInput;
    }

    if (((AccessType & GENERIC_WRITE) != 0) &&
        ((AccessType & GENERIC_READ) == 0)) {

        ConnectMode = ConnectModeOutput;
    }

    //
    // For pins opened in read-write mode, map the initial mode as output
    // unless there is an IO restriction on the pin to be input-only.
    //
    // Output is chosen by default because the connect operation at the
    // controller is delayed until an actual write is issued (delayed write).
    //

    if (((AccessType & GENERIC_WRITE) != 0) &&
        ((AccessType & GENERIC_READ) != 0)) {

        *ReadWriteMode = TRUE;
        if (IoRestriction == GPIO_IORESTRICTION_INPUT_ONLY) {
            ConnectMode = ConnectModeInput;

        } else {
            ConnectMode = ConnectModeOutput;
        }
    }

GetOpenModeFromRequestEnd:
    return ConnectMode;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilGetIdFromFileName (
    __in PCUNICODE_STRING FileName,
    __out PLARGE_INTEGER ConnectionIdOut
    )

/*++

Routine Description:

    This routine parses the trailing name of the file object to determine
    the Connection ID associated with the request.

Arguments:

    FileName - Supplies the name of the file being opened.

    ConnectionIdOut - Supplies a pointer to a variable that receives the
        connection ID associated with the request.

Return Value:

    NTSTATUS code.

--*/

{

    LARGE_INTEGER ConnectionId;
    UNICODE_STRING FileIdPart;
    NTSTATUS Status;

    PAGED_CODE();

    RtlCopyMemory(&FileIdPart, FileName, sizeof(FileIdPart));

    //
    // Skip leading backslash
    //

    while (FileIdPart.Length > 0 &&
           FileIdPart.Buffer[0] == L'\\') {

        ++FileIdPart.Buffer;
        FileIdPart.Length -= sizeof(WCHAR);
        FileIdPart.MaximumLength -= sizeof(WCHAR);
    }

    Status = RESOURCE_HUB_ID_FROM_FILE_NAME(FileIdPart.Buffer,
                                            &ConnectionId);

    if (NT_SUCCESS(Status)) {
        *ConnectionIdOut = ConnectionId;
    }

    return Status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
BOOLEAN
GpioUtilIsBiosFunctionConfigDescriptorValid (
    __in_bcount(DescriptorLength)
        PPNP_FUNCTION_CONFIG_DESCRIPTOR Descriptor,
    __in ULONG DescriptorLength
    )

/*++

Routine Description:

    This routine determines whether the supplied BIOS function config
    descriptor is valid or not.

Arguments:

    Descriptor - Supplies a pointer to the BIOS function config descriptor.

    DescriptorLength - Supplies the length of the BIOS descriptor.

Return Value:

    NTSTATUS code.

--*/

{

    ULONG FieldLength;
    size_t NameLength;
    USHORT PinTableOffset;
    USHORT ResourceSourceOffset;
    BOOLEAN Valid;
    USHORT VendorDataOffset;

    PAGED_CODE();

    //
    // The descriptor must have the correct tag and meet the minimum length
    // requirements.
    //

    Valid = FALSE;
    if ((DescriptorLength < sizeof(PNP_FUNCTION_CONFIG_DESCRIPTOR)) ||
        (Descriptor->Tag != FUNCTION_CONFIG_DESCRIPTOR)) {

        goto IsBiosFunctionConfigDescriptorValidEnd;
    }

    //
    // The pin table offset and resource string offset must be lower than the
    // descriptor length. The vendor data offset can be lower or equal to the
    // descriptor length (vendor data can be NULL). Also the offset must
    // satisfy:
    //      Pin table Offset < Resource String Offset < Vendor Data Offset
    //

    PinTableOffset = Descriptor->PinTableOffset;
    ResourceSourceOffset = Descriptor->ResourceSourceOffset;
    VendorDataOffset = Descriptor->VendorDataOffset;
    if ((PinTableOffset >= DescriptorLength) ||
        (ResourceSourceOffset >= DescriptorLength) ||
        (VendorDataOffset > DescriptorLength) ||
        (PinTableOffset >= ResourceSourceOffset) ||
        (ResourceSourceOffset >= VendorDataOffset)) {

        goto IsBiosFunctionConfigDescriptorValidEnd;
    }

    //
    // The revision field should be at least the minimum value.
    //

    if (Descriptor->Revision < 
        PNP_FUNCTION_CONFIG_DESCRIPTOR_REVISION_MINIMUM) {

        goto IsBiosFunctionConfigDescriptorValidEnd;
    }

    //
    // The pin table offset must describe atleast one pin.
    //

    FieldLength = ResourceSourceOffset - PinTableOffset;
    if ((FieldLength < sizeof(USHORT)) ||
        ((FieldLength % sizeof(USHORT)) != 0)) {

        goto IsBiosFunctionConfigDescriptorValidEnd;
    }

    //
    // The resource string cannot be just NULL (it must be NULL-terminated
    // though) or cannot exceed the maximum number of characters.
    //

    NameLength = VendorDataOffset - ResourceSourceOffset;
    if ((NameLength < sizeof(UCHAR)) ||
        (NameLength >= (UNICODE_STRING_MAX_CHARS + sizeof(ANSI_NULL)))) {

        goto IsBiosFunctionConfigDescriptorValidEnd;
    }

    Valid = TRUE;

IsBiosFunctionConfigDescriptorValidEnd:
    return Valid;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
BOOLEAN
GpioUtilIsBiosGpioDescriptorValid (
    __in_bcount(DescriptorLength)
        PPNP_GPIO_INTERRUPT_IO_DESCRIPTOR Descriptor,
    __in ULONG DescriptorLength
    )

/*++

Routine Description:

    This routine determines whether the supplied BIOS GPIO descriptor (either
    interrupt or IO) is valid or not. Note this routine only support the
    GPIO descriptors in the new format.

Arguments:

    Descriptor - Supplies a pointer to the BIOS GPIO descriptor.

    DescriptorLength - Supplies the length of the BIOS descriptor.

Return Value:

    NTSTATUS code.

--*/

{

    ULONG FieldLength;
    size_t NameLength;
    USHORT PinTableOffset;
    USHORT ResourceSourceOffset;
    USHORT Type;
    BOOLEAN Valid;
    USHORT VendorDataOffset;

    PAGED_CODE();

    //
    // The descriptor must have the correct tag and meet the minimum length
    // requirements.
    //

    Valid = FALSE;
    if ((DescriptorLength < sizeof(PNP_GPIO_INTERRUPT_IO_DESCRIPTOR)) ||
        (Descriptor->Tag != GPIO_INTERRUPT_IO_DESCRIPTOR)) {

        goto IsBiosGpioDescriptorValidEnd;
    }

    //
    // The pin table offset and resource string offset must be lower than the
    // descriptor length. The vendor data offset can be lower or equal to the
    // descriptor length (vendor data can be NULL). Also the offset must
    // satisfy:
    //      Pin table Offset < Resource String Offset < Vendor Data Offset
    //

    PinTableOffset = Descriptor->PinTableOffset;
    ResourceSourceOffset = Descriptor->ResourceSourceOffset;
    VendorDataOffset = Descriptor->VendorDataOffset;
    if ((PinTableOffset >= DescriptorLength) ||
        (ResourceSourceOffset >= DescriptorLength) ||
        (VendorDataOffset > DescriptorLength) ||
        (PinTableOffset >= ResourceSourceOffset) ||
        (ResourceSourceOffset >= VendorDataOffset)) {

        goto IsBiosGpioDescriptorValidEnd;
    }

    //
    // The revision field should be 1 and descriptor type field should be
    // either interrupt or IO.
    //

    Type = Descriptor->DescriptorType;
    if ((Descriptor->Revision < PNP_GPIO_IRQ_DESCRIPTOR_REVISION_MINIMUM) ||
        ((Type != PNP_GPIO_IRQ_DESCRIPTOR_TYPE_INTERRUPT) &&
         (Type != PNP_GPIO_IRQ_DESCRIPTOR_TYPE_IO))) {

        goto IsBiosGpioDescriptorValidEnd;
    }

    //
    // The pin table offset must describe atleast one pin.
    //

    FieldLength = ResourceSourceOffset - PinTableOffset;
    if ((FieldLength < sizeof(USHORT)) ||
        ((FieldLength % sizeof(USHORT)) != 0)) {

        goto IsBiosGpioDescriptorValidEnd;
    }

    //
    // The resource string cannot be just NULL (it must be NULL-terminated
    // though) or cannot exceed the maximum number of characters.
    //

    NameLength = VendorDataOffset - ResourceSourceOffset;
    if ((NameLength < sizeof(UCHAR)) ||
        (NameLength >= (UNICODE_STRING_MAX_CHARS + sizeof(ANSI_NULL)))) {

        goto IsBiosGpioDescriptorValidEnd;
    }

    Valid = TRUE;

IsBiosGpioDescriptorValidEnd:
    return Valid;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilIsChildObjectDefined (
    __in PACPI_ENUM_CHILDREN_OUTPUT_BUFFER EnumChildrenBuffer,
    __in ULONG EnumChildrenBufferSize,
    __in ULONG ChildObjectName
    )

/*

Routine Description:

    This function parses the ACPI namespace children list looking whether a
    child with the specified name exists or not.

Arguments:

    EnumChildrenBuffer - Supplies a buffer containing ACPI namespace child
        names.

    EnumChildrenBufferSize - Supplies the size of ACPI namespace child buffer.

    ChildObjectName - Supplies the name of the child (in ULONG format).

Return Value:

    STATUS_SUCCESS if the _EVT method is defined in the namespace.
    STATUS_OBJECT_NAME_NOT_FOUND if _EVT method is not defined in the namespace.
    STATUS_ACPI_INVALID_ARGTYPE if the parsing was unsuccessful.

--*/

{

    PACPI_ENUM_CHILD AcpiEnumChild;
    ULONG Count;
    ULONG Index;
    ULONG MethodName;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // If the input buffer is NULL, then treat it as success (no-op) and bail
    // out.
    //

    if ((EnumChildrenBuffer == NULL) || (EnumChildrenBufferSize == 0x0)) {
        Status = STATUS_ACPI_INVALID_ARGTYPE;
        goto IsChildObjectDefinedEnd;
    }

    //
    // Skip the first child since ACPI returns the video device itself as the
    // first "child".
    //

    AcpiEnumChild = &EnumChildrenBuffer->Children[0];
    for (Count = 1; Count < EnumChildrenBuffer->NumberOfChildren; Count++) {

        //
        // Proceed to the next ACPI child. We do this first to skip the adapter
        // itself (see comment above).
        //

        AcpiEnumChild = ACPI_ENUM_CHILD_NEXT(AcpiEnumChild);

        //
        // If this child has children of it's own, then it's not a method
        // and thus should be ignored.
        //

        if ((AcpiEnumChild->Flags & ACPI_OBJECT_HAS_CHILDREN) != 0) {
            continue;
        }

        //
        // Skip over children that are not in the _Lxx or _Exx form.
        //

        if (AcpiEnumChild->NameLength < 5) {
            continue;
        }

        //
        // The method name must be of the _Lxx or _Exx form. Note ACPI returns
        // the fully qualified name for the children. Thus skip over to the
        // last four characters.
        //
        // N.B. The name length is inclusive of NULL.
        //

        Index = AcpiEnumChild->NameLength - 5;
        MethodName = *(PULONG)&AcpiEnumChild->Name[Index];
        if (MethodName == ChildObjectName) {
            break;
        }
    }

    if (Count == EnumChildrenBuffer->NumberOfChildren) {
        Status = STATUS_OBJECT_NAME_NOT_FOUND;

    } else {
        Status = STATUS_SUCCESS;
    }

IsChildObjectDefinedEnd:
    return Status;
}


_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilParseNamespaceEventPins (
    __in PACPI_ENUM_CHILDREN_OUTPUT_BUFFER EnumChildrenBuffer,
    __in ULONG EnumChildrenBufferSize,
    __inout_ecount_opt(EdgePinListInputSize) PULONG EdgePinList,
    __in ULONG EdgePinListInputSize,
    __out PULONG EdgePinListOutputCount,
    __inout_ecount_opt(LevelPinListInputSize) PULONG LevelPinList,
    __in ULONG LevelPinListInputSize,
    __out PULONG LevelPinListOutputCount
    )

/*

Routine Description:

    This function filters the ACPI namespace children list looking for _Exx
    or _Lxx children. It fills in the caller-supplied edge and level pin
    buffers with the pin numbers that correspond to the _Exx/_Lxx methods.

Arguments:

    EnumChildrenBuffer - Supplies a buffer containing ACPI namespace child
        names.

    EnumChildrenBufferSize - Supplies the size of ACPI namespace child buffer.

    EdgePinList - Supplies a pointer to a buffer that is filled with all the
        edge pins.

    EdgePinListInputSize - Supplies the total size (ELEMENT COUNT) of the
        edge pin buffer.

    EdgePinListOutputCount - Supplies a variable that receives the total
        number of edge pins.

    LevelPinList - Supplies a pointer to a buffer that is filled with all the
        level pins.

    LevelPinListInputSize - Supplies the total size (ELEMENT COUNT) of the
        level pin buffer.

    LevelPinListOutputCount - Supplies a variable that receives the total
        number of level pins.

Return Value:

    STATUS_SUCCESS if the parsing was successful.
    STATUS_BUFFER_TOO_SMALL if the edge/level buffer size is insufficient.
    STATUS_ACPI_INVALID_ARGTYPE if the parsing was unsuccessful.

--*/

{

    PACPI_ENUM_CHILD AcpiEnumChild;
    ULONG Count;
    PUCHAR MethodName;
    ULONG Index;
    NTSTATUS Status;
    ULONG Value;

    PAGED_CODE();

    *EdgePinListOutputCount = 0x0;
    *LevelPinListOutputCount = 0x0;

    //
    // If the input buffer is NULL, then treat it as success (no-op) and bail
    // out.
    //

    if ((EnumChildrenBuffer == NULL) || (EnumChildrenBufferSize == 0x0)) {
        Status = STATUS_ACPI_INVALID_ARGTYPE;
        goto ParseNamespaceEventPinsEnd;
    }

    //
    // Skip the first child since ACPI returns the video device itself as the
    // first "child".
    //

    AcpiEnumChild = &EnumChildrenBuffer->Children[0];
    for (Count = 1; Count < EnumChildrenBuffer->NumberOfChildren; Count++) {

        //
        // Proceed to the next ACPI child. We do this first to skip the adapter
        // itself (see comment above).
        //

        AcpiEnumChild = ACPI_ENUM_CHILD_NEXT(AcpiEnumChild);

        //
        // If this child has children of it's own, then it's not a method
        // and thus should be ignored.
        //

        if ((AcpiEnumChild->Flags & ACPI_OBJECT_HAS_CHILDREN) != 0) {
            continue;
        }

        //
        // Skip over children that are not in the _Lxx or _Exx form.
        //

        if (AcpiEnumChild->NameLength < 5) {
            continue;
        }

        //
        // The method name must be of the _Lxx or _Exx form. Note ACPI returns
        // the fully qualified name for the children. Thus skip over to the
        // last four characters.
        //
        // N.B. The name length is inclusive of NULL.
        //

        Index = AcpiEnumChild->NameLength - 5;
        MethodName = (PUCHAR)&AcpiEnumChild->Name[Index];
        if ((MethodName[0] == '_') &&
            ((MethodName[1] == 'E') || (MethodName[1] == 'L'))) {

            //
            // Extract the "xx" part from the name and attempt to convert it
            // to an integer. If it is not a valid integer, then ignore this
            // method. Otherwise record the pin number.
            //

            Status = GpiopConvertAsciiStringToInteger((PCSTR)&MethodName[2],
                                                      3,
                                                      16,
                                                      &Value);

            if (!NT_SUCCESS(Status)) {
                continue;
            }

            GPIO_ASSERT(Value <= 0xFF);

            //
            // Set the pin number in the appropriate list and increment the
            // count.
            //

            if (MethodName[1] == 'E') {
                if ((ARGUMENT_PRESENT(EdgePinList) != FALSE) &&
                    (*EdgePinListOutputCount < EdgePinListInputSize)) {

                    EdgePinList[*EdgePinListOutputCount] = Value;
                }

                *EdgePinListOutputCount += 1;

            } else if (MethodName[1] == 'L') {
                if ((ARGUMENT_PRESENT(LevelPinList) != FALSE) &&
                    (*LevelPinListOutputCount < LevelPinListInputSize)) {

                    LevelPinList[*LevelPinListOutputCount] = Value;
                }

                *LevelPinListOutputCount += 1;
            }
        }
    }

    if ((*EdgePinListOutputCount > EdgePinListInputSize) ||
        (*LevelPinListOutputCount > LevelPinListInputSize)) {

        Status = STATUS_BUFFER_TOO_SMALL;

    } else {
        Status = STATUS_SUCCESS;
    }

ParseNamespaceEventPinsEnd:
    return Status;
}

VOID
GpioUtilUpdatePinValue (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in PIN_NUMBER PinNumber,
    __in OPERATION_REGISTER_TYPE Type,
    __in BOOLEAN SetOperation
    )

/*++

Routine Description:

    This is a utility routine to set or clear bits within a register or bitmap
    buffer.

    N.B. This routine can be called at DIRQL level.

Arguments:

    GpioBank - Supplies a pointer to the GPIO bank.

    PinNumber - Supplies the relative pin number on which the operation should
        be performed. ALL_GPIO_PINS if it should be performed on all pins.

    Type - Supplies the register which should be operated upon.

    SetOperation - Supplies whether this is a set (TRUE) or clear operation.

Return Value:

    None.

--*/

{

    ULONG AbsolutePin;
    PGPIO_ACPI_EVENT_DATA EventData;
    PDEVICE_EXTENSION GpioExtension;
    PGPIO_INTERRUPT_DATA InterruptData;
    ULONG64 Value;

    GpioExtension = GpioBank->GpioExtension;
    EventData = &GpioExtension->EventData;
    InterruptData = &GpioBank->InterruptData;

    if (SetOperation != FALSE) {

        //
        // Set the pin within the specified register or bitmap.
        //

        switch (Type) {

        case IntRegisterTypeEnableMask:
            if (PinNumber != ALL_GPIO_PINS) {
                InterruptData->EnableRegister |= ((ULONG64)1 << PinNumber);

            } else {
                InterruptData->EnableRegister = MAX_ULONG64;
            }

            break;

        case IntRegisterTypeInterruptMask:
            if (PinNumber != ALL_GPIO_PINS) {
                InterruptData->InterruptMask |= ((ULONG64)1 << PinNumber);

            } else {
                InterruptData->InterruptMask = MAX_ULONG64;
            }

            break;

        case IntRegisterTypePendingStatusClearMask:
            if (PinNumber != ALL_GPIO_PINS) {
                Value = ((ULONG64)1 << PinNumber);
                InterruptData->PendingStatusClearMask |= Value;

            } else {
                InterruptData->PendingStatusClearMask = MAX_ULONG64;
            }

            break;

        case IntRegisterTypeDelayPendingStatusClearMask:
            if (PinNumber != ALL_GPIO_PINS) {
                Value = ((ULONG64)1 << PinNumber);
                InterruptData->DelayPendingStatusClearMask |= Value;

            } else {
                InterruptData->DelayPendingStatusClearMask = MAX_ULONG64;
            }

            break;

        case AcpiRegisterTypePendingEvents:
            if (PinNumber != ALL_GPIO_PINS) {
                AbsolutePin = GPIO_RELATIVE_TO_ABSOLUTE_PIN(GpioBank, PinNumber);
                RtlSetBit(&EventData->PendingEventsBitmap, AbsolutePin);

            } else {
                AbsolutePin = GPIO_RELATIVE_TO_ABSOLUTE_PIN(GpioBank, 0);
                RtlSetBits(&EventData->PendingEventsBitmap,
                           AbsolutePin,
                           GpioBank->PinCount);
            }

            break;

        default:

            GPIO_ASSERT(FALSE);

            break;

        }

    } else {

        //
        // Clear the pin within the specified register or bitmap.
        //

        switch (Type) {

        case IntRegisterTypeEnableMask:
            if (PinNumber != ALL_GPIO_PINS) {
                InterruptData->EnableRegister &= ~((ULONG64)1 << PinNumber);

            } else {
                InterruptData->EnableRegister = 0;
            }

            break;

        case IntRegisterTypeInterruptMask:
            if (PinNumber != ALL_GPIO_PINS) {
                InterruptData->InterruptMask &= ~((ULONG64)1 << PinNumber);

            } else {
                InterruptData->InterruptMask = 0;
            }

            break;

        case IntRegisterTypePendingStatusClearMask:
            if (PinNumber != ALL_GPIO_PINS) {
                Value = ((ULONG64)1 << PinNumber);
                InterruptData->PendingStatusClearMask &= ~Value;

            } else {
                InterruptData->PendingStatusClearMask = 0;
            }

            break;

        case IntRegisterTypeDelayPendingStatusClearMask:
            if (PinNumber != ALL_GPIO_PINS) {
                Value = ((ULONG64)1 << PinNumber);
                InterruptData->DelayPendingStatusClearMask &= ~Value;

            } else {
                InterruptData->DelayPendingStatusClearMask = 0;
            }

            break;

        case AcpiRegisterTypePendingEvents:
            if (PinNumber != ALL_GPIO_PINS) {
                AbsolutePin = GPIO_RELATIVE_TO_ABSOLUTE_PIN(GpioBank, PinNumber);
                RtlClearBit(&EventData->PendingEventsBitmap, AbsolutePin);

            } else {
                AbsolutePin = GPIO_RELATIVE_TO_ABSOLUTE_PIN(GpioBank, 0);
                RtlClearBits(&EventData->PendingEventsBitmap,
                             AbsolutePin,
                             GpioBank->PinCount);
            }

            break;

        default:

            GPIO_ASSERT(FALSE);

            break;
        }
    }

    return;
}

//
// ---------------------------------------------------------- Registry Functions
//

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
GpioUtilQueryRegistryFlags (
    __inout_opt WDFKEY *ParametersKey,
    __in_opt WDFDRIVER WdfDriver,
    __in PUNICODE_STRING RegistryPath,
    __in PCUNICODE_STRING KeyValueName,
    __out PULONG OptionValue
    )

/*++

Routine Description:

    This routine retrieves GPIO driver-wide flags from the registry. Path is
    HKLM\CCS\SERVICES\GPIOClx0101\Parameters\<key>

Arguments:

    ParametersKey - Supplies an optional pointer that provides a handle to
        the registry to use. If the handle is NULL, then on output, this
        parameters receives a handle to the "Parameters" key. The caller is
        responsible for closing the handle later on.

    WdfDriver - Supplies an optional handle to the WDF driver object. This is
        required if *ParametersKey == NULL.

    RegistryPath - Supplies a pointer to the driver specific registry key.

    KeyValueName - Supplies the key-value name to be queried.

    OptionValue - Supplies a pointer that receives the registry value.

Return Value:

    Status

--*/

{

    BOOLEAN CloseKey;
    WDFKEY Key;
    NTSTATUS Status;

    PAGED_CODE();

    CloseKey = FALSE;
    Key = NULL;
    if ((ParametersKey != NULL) && (*ParametersKey != NULL)) {
        Key = *ParametersKey;

    } else {
        Status = WdfDriverOpenParametersRegistryKey(WdfDriver,
                                                    KEY_READ,
                                                    WDF_NO_OBJECT_ATTRIBUTES,
                                                    &Key);

        if (!NT_SUCCESS(Status)) {
            TraceEvents(GpioLogHandle,
                        Error,
                        DBG_INIT,
                        "%s: Failed to open parameters registry key! "
                        "Path = %S\\%S, Status= %#x.\n",
                        __FUNCTION__,
                        RegistryPath->Buffer,
                        L"Parameters",
                        Status);

            goto QueryRegistryFlagsEnd;
        }

        CloseKey = TRUE;
    }

    Status = WdfRegistryQueryULong(Key, KeyValueName, OptionValue);
    if (!NT_SUCCESS(Status) && (Status != STATUS_OBJECT_NAME_NOT_FOUND)) {
        TraceEvents(GpioLogHandle,
                    Error,
                    DBG_INIT,
                    "%s: Failed to query registry flags! Path = %S\\%S, "
                    "ValueKey = %S, Status= %#x.\n",
                    __FUNCTION__,
                    RegistryPath->Buffer,
                    L"Parameters",
                    KeyValueName->Buffer,
                    Status);

        goto QueryRegistryFlagsEnd;

    } else if (NT_SUCCESS(Status)) {
        TraceEvents(GpioLogHandle,
                    Info,
                    DBG_INIT,
                    "%s: Successfully queried registry flags! Path = %S\\%S, "
                    "ValueKey = %S, Value = %#x.\n",
                    __FUNCTION__,
                    RegistryPath->Buffer,
                    L"Parameters",
                    KeyValueName->Buffer,
                    *OptionValue);
    }

    //
    // On success return the parameter key to caller if desired.
    //

QueryRegistryFlagsEnd:
    if (NT_SUCCESS(Status) &&
        (ParametersKey != NULL) &&
        (*ParametersKey != NULL)) {

        *ParametersKey = Key;

    } else if ((CloseKey != FALSE) && (Key != NULL)) {
        WdfRegistryClose(Key);
    }

    return Status;
}

//
// ---------------------------------------------------------- Tracing Functions
//

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpioUtilTracePrintPinNumbers (
    __in PGPIO_BANK_ENTRY GpioBank,
    __in_ecount(PinCount) PPIN_NUMBER PinNumberTable,
    __in ULONG PinCount
    )

/*++

Routine Description:

    This routine prints the pin numbers for diagnostic purposes.

Arguments:

    GpioBank - Supplies a pointer to the GPIO bank.

    PinNumberTable - Supplies an array containing the pins to be printed.

    PinCount - Supplies a count of entries in the pin number table.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    GpioUtilTracePrintPinNumbersEx(Info,
                                   GpioBank,
                                   PinNumberTable,
                                   PinCount);

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
GpioUtilTracePrintPinNumbersEx (
    __in ULONG DebugPrintLevel,
    __in PGPIO_BANK_ENTRY GpioBank,
    __in_ecount(PinCount) PPIN_NUMBER PinNumberTable,
    __in ULONG PinCount
    )

/*++

Routine Description:

    This routine prints the pin numbers for diagnostic purposes.

Arguments:

    DebugPrintLevel - Supplies the debug print level.

    DebugPrintFlag - Supplies the flag controlling the tracing.

    GpioBank - Supplies a pointer to the GPIO bank.

    PinNumberTable - Supplies an array containing the pins to be printed.

    PinCount - Supplies a count of entries in the pin number table.

Return Value:

    None.

--*/

{

    PSTR Buffer;
    PSTR FormatString;
    USHORT Index;
    size_t LengthRemaining;
    PIN_NUMBER PinNumber;
    NTSTATUS Status;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(DebugPrintLevel);

    if ((PinCount == 0) || (PinNumberTable == NULL)) {
        TraceEvents(GpioBank->GpioExtension->LogHandle,
                    Info,
                    DBG_IO,
                    "PinCount = %#x, PinNumbers = [ -- NULL -- ]\n",
                    PinCount);

        return;
    }

    if (PinCount == 1) {
        PinNumber = GPIO_RELATIVE_TO_ABSOLUTE_PIN(GpioBank,
                                                  (PinNumberTable[0]));

        TraceEvents(GpioBank->GpioExtension->LogHandle,
                    Info,
                    DBG_IO,
                    "PinCount = %#x, PinNumbers = [%#x]\n",
                    PinCount,
                    PinNumber);

        //
        // For multi-pin case, convert the pin numbers to a single string
        // and print it.
        //
        // The buffer is allocated only in the multi-pin case to avoid
        // consuming stack space in the most common case (single pin).
        //

    } else {
        CHAR PinString[PIN_PRINT_BUFFER_SIZE];

        Buffer = (PSTR)&PinString[0];
        LengthRemaining = sizeof(PinString) -  sizeof(CHAR);
        PinString[PIN_PRINT_BUFFER_SIZE - 1] = ANSI_NULL;
        Status = STATUS_SUCCESS;
        for (Index = 0; Index < PinCount; Index += 1) {
            PinNumber =
                GPIO_RELATIVE_TO_ABSOLUTE_PIN(GpioBank,
                                              (PinNumberTable[Index]));

            if (Index < PinCount - 1) {
                FormatString = "%#x, ";

            } else {
                FormatString = "%#x";
            }

            Status = RtlStringCbPrintfExA((PCHAR)Buffer,
                                          LengthRemaining,
                                          &Buffer,
                                          &LengthRemaining,
                                          0,
                                          FormatString,
                                          PinNumber);

            if (!NT_SUCCESS(Status)) {
                break;
            }
        }

        //
        // If the buffer was adequate, then print the pin number string.
        // Otherwise, fallback to printing one pin at a time (takes up multiple
        // log entries).
        //

        if (NT_SUCCESS(Status)) {
            TraceEvents(GpioBank->GpioExtension->LogHandle,
                        Info,
                        DBG_IO,
                        "PinCount = %#x, PinNumbers = [%s]",
                        PinCount,
                        PinString);

        } else {
            for (Index = 0; Index < PinCount; Index += 1) {
                PinNumber =
                    GPIO_RELATIVE_TO_ABSOLUTE_PIN(GpioBank,
                                                  (PinNumberTable[Index]));

                if (Index < (PinCount - 1)) {
                    TraceEvents(GpioBank->GpioExtension->LogHandle,
                                Info,
                                DBG_IO,
                                "%#x,",
                                PinNumber);

                } else {
                    TraceEvents(GpioBank->GpioExtension->LogHandle,
                                Info,
                                DBG_IO,
                                "%#x]\n",
                                PinNumber);
                }
            }
        }
    }

    return;
}


