/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    acpi.cpp

Abstract:

   Contains definitions for ACPI support functions

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel-mode Driver Framework

--*/

#include "internal.h"

#include "acpi.tmh"

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, AcpiInitialize)
    #pragma alloc_text(PAGE, _AcpiGetDeviceInformation)
    #pragma alloc_text(PAGE, _AcpiGetDeviceMethod)
    #pragma alloc_text(PAGE, _AcpiDsmSupported)
    #pragma alloc_text(PAGE, _AcpiPrepareInputParametersForDsm)
#endif


NTSTATUS
AcpiInitialize(
    _In_ PDEVICE_CONTEXT    FxDeviceContext
    )
/*++
 
  Routine Description:

    This routine retrieves ACPI specific settings for the
    device

  Arguments:

    FxDeviceContext - Pointer to the current device context 

  Return Value:

    NTSTATUS Status indicating success or failure

--*/
{
    FuncEntry(TRACE_FLAG_ACPI);

    NTSTATUS status;

    PAGED_CODE();

    //
    // Retrieve HID specific data stored in a _DSM
    //
    status = ::_AcpiGetDeviceMethod(FxDeviceContext);

    if (!NT_SUCCESS(status))
    {
        EventWrite_HIDI2C_ENUMERATION_ACPI_FAILURE(NULL, FxDeviceContext->FxDevice, status);
        DeviceReportPnpProblem(FxDeviceContext->FxDevice, HIDI2C_ACPI_ERROR);
    }

    FuncExit(TRACE_FLAG_ACPI);
    return status;
}


NTSTATUS
_AcpiGetDeviceInformation(
    _In_ PDEVICE_CONTEXT    FxDeviceContext
    )
/*++
 
  Routine Description:

    This routine retrieves general device information pertaining to the
    device by calling IOCTL_ACPI_GET_DEVICE_INFORMATION

  Arguments:

    FxDeviceContext - Pointer to the current device context 

  Return Value:

    NTSTATUS Status indicating success or failure

--*/
{
    //
    // This function is deprecated per v1.0 spec where
    // vendorId, productId are stored in the descriptor
    //

    NTSTATUS status;

    PAGED_CODE();
    
    //
    // Use the defualt IO target as the ACPI target
    //
    WDFIOTARGET acpiIoTarget;
    acpiIoTarget = WdfDeviceGetIoTarget(FxDeviceContext->FxDevice);
    
    //
    // Declare the buffer that holds ACPI device information
    //
    PACPI_DEVICE_INFORMATION_OUTPUT_BUFFER acpiInfo = NULL;
    
    ULONG acpiInfoSize;
    acpiInfoSize = sizeof(ACPI_DEVICE_INFORMATION_OUTPUT_BUFFER) +
                   EXPECTED_IOCTL_ACPI_GET_DEVICE_INFORMATION_STRING_LENGTH;

    acpiInfo = (PACPI_DEVICE_INFORMATION_OUTPUT_BUFFER)
               HIDI2C_ALLOCATE_POOL(NonPagedPoolNx, acpiInfoSize);

    if (acpiInfo == NULL) 
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_ACPI,
                "Failed allocating memory for ACPI output buffer "
                "status:%!STATUS!",
                status);
        goto exit;
    }

    WDF_MEMORY_DESCRIPTOR outputDescriptor;
    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(
                        &outputDescriptor,
                        (PVOID)acpiInfo,
                        acpiInfoSize);

    //
    // Send the ACPI IOCTL to get Device Information
    //
    status = WdfIoTargetSendIoctlSynchronously(
                                acpiIoTarget,
                                NULL,
                                IOCTL_ACPI_GET_DEVICE_INFORMATION,
                                NULL,
                                &outputDescriptor,
                                NULL,
                                NULL);
    
    if (!NT_SUCCESS(status))
    {
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_ACPI,
                "Failed sending IOCTL_ACPI_GET_DEVICE_INFORMATION "
                "status:%!STATUS!",
                status);
        goto exit;
    }

    //
    // Verify the method signature
    //
    if (acpiInfo->Signature != IOCTL_ACPI_GET_DEVICE_INFORMATION_SIGNATURE)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_ACPI,
                "Incorrect signature for ACPI_GET_DEVICE_INFORMATION status:%!STATUS!",
                status);
        goto exit;
    }

    //
    // Parse the ACPI output buffer and get the VendorId/DeviceId/Revision in
    // and store the values into the device context using the following mapping:

    // ACPI     |      HID
    // --------------------
    // _HID     |      VendorID & ProductID
    // _HRV     |      Revision

    //
    // USB-HID VID/PID format: VID: 0x045E PID:0x0768
    // ACPI Vendor ID format: "MSFT0001"
    // Current ACPI->USB mapping used is: VID=MS, PID=01
    //
        
    //PBYTE pVendorIdOffset = ((PBYTE) acpiInfo + acpiInfo->VendorIdStringOffset);
    //PBYTE pDeviceIdOffset = ((PBYTE) acpiInfo + acpiInfo->DeviceIdStringOffset);
    //
    //USHORT vendorIdLength = acpiInfo->DeviceIdStringOffset - acpiInfo->VendorIdStringOffset;
    //USHORT deviceIdLength = acpiInfo->VendorStringLength - vendorIdLength;


    // Copy the first two-bytes of the Vendor ID portion
    //RtlCopyMemory(&FxDeviceContext->AcpiSettings.VendorId, 
    //              pVendorIdOffset,
    //              vendorIdLength);

    // Copy the last two-bytes of the Device ID portion
    //RtlCopyMemory(&FxDeviceContext->AcpiSettings.ProductId, 
    //              pDeviceIdOffset + 2, 
    //              deviceIdLength);
    //
    //// Revision is _HRV
    //FxDeviceContext->AcpiSettings.Revision  = acpiInfo->HardwareRevision;

    //TraceEvents(
    //        TRACE_LEVEL_INFORMATION,
    //        TRACE_FLAG_ACPI,
    //        "ACPI Device Information VendorId:0x%x, ProductId=0x%x, Revision=%u ",
    //        FxDeviceContext->AcpiSettings.VendorId,
    //        FxDeviceContext->AcpiSettings.ProductId,
    //        FxDeviceContext->AcpiSettings.Revision);

exit:
    if(!NT_SUCCESS(status))
    {
        TraceLoggingACPI(
            status, 
            FxDeviceContext->FxDevice);  
    }
    
    if (acpiInfo != NULL)
        HIDI2C_FREE_POOL(acpiInfo);

    return status;
}


NTSTATUS
_AcpiGetDeviceMethod(
    _In_ PDEVICE_CONTEXT    FxDeviceContext
    )
/*++
 
  Routine Description:

    This routine retrieves other HID specific data related to the
    device by calling IOCTL_ACPI_EVAL_METHOD

  Arguments:

    FxDeviceContext - Pointer to the current device context 

  Return Value:

    NTSTATUS Status indicating success or failure

--*/
{
    NTSTATUS status;

    PAGED_CODE();

    //
    // Evaluating a _DSM method is a two-step process. In the first step check
    // if a DSM function for returning HID DATA is supported. If TRUE, then
    // evaluate this function and return an appropriate result to the caller.
    //
    
    BOOLEAN                         isSupported = FALSE;
    PACPI_EVAL_INPUT_BUFFER_COMPLEX acpiInput   = NULL;
    ULONG                           acpiInputSize;
    ACPI_EVAL_OUTPUT_BUFFER         acpiOutput;
    
    status = ::_AcpiDsmSupported(
                    FxDeviceContext,
                    ACPI_DSM_HIDI2C_FUNCTION,
                    &isSupported);

    if (!NT_SUCCESS(status)) 
    {
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_ACPI,
                "Check for DSM support returned %c "
                "status:%!STATUS!",
                isSupported,
                status);
        goto exit;
    }

    if (isSupported == FALSE)
    {
        status = STATUS_NOT_SUPPORTED;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_ACPI,
                "Check for DSM support returned %c "
                "status:%!STATUS!",
                isSupported,
                status);
        goto exit;
    }

    //
    // Now evaluate the method for real
    // 

    acpiInputSize = sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX) +
                            (sizeof(ACPI_METHOD_ARGUMENT) *
                            (ACPI_DSM_INPUT_ARGUMENTS_COUNT - 1)) +
                            sizeof(GUID);
    
    acpiInput = (PACPI_EVAL_INPUT_BUFFER_COMPLEX) HIDI2C_ALLOCATE_POOL(PagedPool, acpiInputSize);

    if (acpiInput == NULL) 
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_ACPI,
                "Failed allocating memory for ACPI Input buffer "
                "status:%!STATUS!",
                status);
        goto exit;
    }

    //
    // Fill in the input parameters for executing _DSM method
    // 
    ::_AcpiPrepareInputParametersForDsm(
                    acpiInput,
                    acpiInputSize,
                    ACPI_DSM_HIDI2C_FUNCTION);
    
    RtlZeroMemory(&acpiOutput, sizeof(ACPI_EVAL_OUTPUT_BUFFER));

    //
    // Initialize the input/output buffers/memory descriptors
    //

    WDF_MEMORY_DESCRIPTOR  inputDescriptor;
    WDF_MEMORY_DESCRIPTOR  outputDescriptor;

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(
                            &inputDescriptor,
                            (PVOID)acpiInput,
                            acpiInputSize);
    
    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(
                        &outputDescriptor,
                        (PVOID)&acpiOutput,
                        sizeof(ACPI_EVAL_OUTPUT_BUFFER));

    // Use the default IO target as the ACPI target
    
    WDFIOTARGET acpiIoTarget;
    acpiIoTarget = WdfDeviceGetIoTarget(FxDeviceContext->FxDevice);

    NT_ASSERTMSG("ACPI IO target is NULL", acpiIoTarget != NULL);

    //
    // Send the ACPI IOCTL to get Device Information
    //
    status = WdfIoTargetSendIoctlSynchronously(
                                acpiIoTarget,
                                NULL,
                                IOCTL_ACPI_EVAL_METHOD,
                                &inputDescriptor,
                                &outputDescriptor,
                                NULL,
                                NULL);

    if (!NT_SUCCESS(status)) 
    {
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_ACPI,
                "Failed executing DSM method IOCTL "
                "status:%!STATUS!",
                status);
        goto exit;
    }

    if ((acpiOutput.Argument[0].Type != ACPI_METHOD_ARGUMENT_INTEGER) ||
        (acpiOutput.Argument[0].DataLength == 0)) 
    {
        status = STATUS_UNSUCCESSFUL;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_ACPI,
                "Incorrect parameters returned for DSM method "
                "status:%!STATUS!",
                status);
        goto exit;
    }

    // Parse the argument to retrieve the HID Descriptor Address
    // and save it in the Device Context

    FxDeviceContext->AcpiSettings.HidDescriptorAddress = (USHORT) acpiOutput.Argument[0].Data[0];

    TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_FLAG_ACPI,
            "HID Descriptor offset:0x%x retrieved from ACPI",
            FxDeviceContext->AcpiSettings.HidDescriptorAddress);

exit:
    if(!NT_SUCCESS(status))
    {
        TraceLoggingACPI(
            status, 
            FxDeviceContext->FxDevice);
    }

    if (acpiInput != NULL)
        HIDI2C_FREE_POOL(acpiInput);

    return status;
}

NTSTATUS
_AcpiDsmSupported(
        _In_ PDEVICE_CONTEXT    FxDeviceContext,
        _In_ ULONG              FunctionIndex,
        _Out_ PBOOLEAN          Supported
        )
/*++

Routine Description:

    This routine is invoked to check if a specific function for a _DSM method
    is supported.

Arguments:

    FxDeviceContext         - Pointer to the WDF Device Context.

    FunctionIndex -         Supplies the function index to check for.

    Supported -         Supplies a pointer to a boolean that on return will indicate
        if the given function is supported.

Return Value:

    NTSTATUS        Status indicating success or failure

--*/
{
    NTSTATUS status;

    ACPI_EVAL_OUTPUT_BUFFER         acpiOutput;
    PACPI_EVAL_INPUT_BUFFER_COMPLEX acpiInput;
    ULONG                           acpiInputSize;
    
    PAGED_CODE();

    BOOLEAN support = FALSE;
    
    acpiInputSize = sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX) +
                    (sizeof(ACPI_METHOD_ARGUMENT) * (ACPI_DSM_INPUT_ARGUMENTS_COUNT - 1)) +
                    sizeof(GUID);

    acpiInput = (PACPI_EVAL_INPUT_BUFFER_COMPLEX) HIDI2C_ALLOCATE_POOL(PagedPool, acpiInputSize);

    if (acpiInput == NULL) 
    {
        status = STATUS_INSUFFICIENT_RESOURCES;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_ACPI,
                "Failed allocating memory for ACPI input buffer "
                "status:%!STATUS!",
                status);
        goto exit;
    }

    //
    // Fill in the Input parameters for the _DSM method
    //
    ::_AcpiPrepareInputParametersForDsm(
                    acpiInput,
                    acpiInputSize,
                    ACPI_DSM_QUERY_FUNCTION);

    RtlZeroMemory(&acpiOutput, sizeof(ACPI_EVAL_OUTPUT_BUFFER));

    WDF_MEMORY_DESCRIPTOR  inputDescriptor;
    WDF_MEMORY_DESCRIPTOR  outputDescriptor;
    
    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(
                        &inputDescriptor,
                        (PVOID)acpiInput,
                        acpiInputSize);

    WDF_MEMORY_DESCRIPTOR_INIT_BUFFER(
                        &outputDescriptor,
                        (PVOID)&acpiOutput,
                        sizeof(ACPI_EVAL_OUTPUT_BUFFER));

    // Use the default IO target as the ACPI target
    
    WDFIOTARGET acpiIoTarget;
    acpiIoTarget = WdfDeviceGetIoTarget(FxDeviceContext->FxDevice);

    NT_ASSERTMSG("ACPI IO target is NULL", acpiIoTarget != NULL);

    //
    // Send the ACPI IOCTL to get Device Supported Methods
    //
    status = WdfIoTargetSendIoctlSynchronously(
                                acpiIoTarget,
                                NULL,
                                IOCTL_ACPI_EVAL_METHOD,
                                &inputDescriptor,
                                &outputDescriptor,
                                NULL,
                                NULL);

    if (!NT_SUCCESS(status)) 
    {
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_ACPI,
                "Failed sending DSM function query IOCTL "
                "status:%!STATUS!",
                status);
        goto exit;
    }

    if ((acpiOutput.Argument[0].Type != ACPI_METHOD_ARGUMENT_BUFFER) ||
        (acpiOutput.Argument[0].DataLength == 0)) 
    {
        status = STATUS_INVALID_PARAMETER;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_ACPI,
                "Incorrect return parameters for DSM function query IOCTL"
                "status:%!STATUS!",
                status);
        goto exit;
    }

    //
    // Check if the function is supported
    //
    status = STATUS_SUCCESS;
    if ((acpiOutput.Argument[0].Data[0] & (1 << FunctionIndex)) != 0) 
    { 
        support = TRUE;
        TraceEvents(
                TRACE_LEVEL_INFORMATION,
                TRACE_FLAG_ACPI,
                "Found supported DSM function:%d",
                FunctionIndex);
    }

exit:
    if(!NT_SUCCESS(status))
    {
        TraceLoggingACPI(
            status, 
            FxDeviceContext->FxDevice);
    }

    if (acpiInput != NULL) 
    {
        HIDI2C_FREE_POOL(acpiInput);
    }

    *Supported = support;
    return status;
}


VOID
_AcpiPrepareInputParametersForDsm(
    _Inout_ PACPI_EVAL_INPUT_BUFFER_COMPLEX ParametersBuffer,
    _In_ ULONG ParametersBufferSize,
    _In_ ULONG FunctionIndex
    )
/*++

Routine Description:

    This routine is invoked for initializing an input parameter buffer
    for evaluation of a _DSM control method defined in the ACPI namespace

Arguments:

    ParameterBuffer - Supplies a pointer to an input buffer.

    ParameterBufferSize - Supplies the length for an input buffer.

    FunctionIndex - Supplies the value of a function index for an input
        parameter.

Return Value:

    None.

--*/
{
    PACPI_METHOD_ARGUMENT pArgument;

    PAGED_CODE();

    NT_ASSERTMSG("ACPI input buffer is NULL", ParametersBuffer != NULL);

    RtlZeroMemory(ParametersBuffer, ParametersBufferSize);
    
    ParametersBuffer->Signature = ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE;
    ParametersBuffer->MethodNameAsUlong = (ULONG)'MSD_';
    ParametersBuffer->Size = ParametersBufferSize;
    ParametersBuffer->ArgumentCount = ACPI_DSM_INPUT_ARGUMENTS_COUNT;

    //
    // Argument 0: UUID.
    //

    pArgument = &ParametersBuffer->Argument[0];
    ACPI_METHOD_SET_ARGUMENT_BUFFER(pArgument, &ACPI_DSM_GUID_HIDI2C, sizeof(GUID));

    //
    // Argument 1: Revision.
    //

    pArgument = ACPI_METHOD_NEXT_ARGUMENT(pArgument);
    ACPI_METHOD_SET_ARGUMENT_INTEGER(pArgument, ACPI_DSM_REVISION);

    //
    // Argument 2: Function index.
    //

    pArgument = ACPI_METHOD_NEXT_ARGUMENT(pArgument);
    ACPI_METHOD_SET_ARGUMENT_INTEGER(pArgument, FunctionIndex);

    //
    // Argument 3: Empty package for all functions in the _DSM definitions
    //

    pArgument = ACPI_METHOD_NEXT_ARGUMENT(pArgument);
    pArgument->Type = ACPI_METHOD_ARGUMENT_PACKAGE;
    pArgument->DataLength = sizeof(ULONG);
    pArgument->Argument = 0;
    
    return;
}
