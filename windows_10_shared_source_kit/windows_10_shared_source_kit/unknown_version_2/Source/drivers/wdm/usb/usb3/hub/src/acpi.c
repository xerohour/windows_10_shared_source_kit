/*++
Copyright (c) Microsoft Corporation

Module Name:

    acpi.c

Abstract:

    ACPI functions.

Author:

Environment:

    Kernel mode only.

--*/

#include "pch.h"
#include <acpiioct.h>
#include "acpi.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text (PAGE, HUBACPI_EvalAcpiMethodEx)
#pragma alloc_text (PAGE, HUBACPI_EnumChildren)
#pragma alloc_text (PAGE, HUBACPI_GetAcpiPortAttributes)
#pragma alloc_text (PAGE, HUBACPI_EvaluateDsmMethod)
#endif

//
// define field offset mapping for UPC package structure
//
const struct _ACPI_UPC_PACKAGE_FIELD {
    ULONG Offset;
    ULONG Size;
} acpiUpcPackageFields[] = {
    { FIELD_OFFSET(ACPI_UPC_PACKAGE, PortIsConnectable), sizeof(UCHAR) },
    { FIELD_OFFSET(ACPI_UPC_PACKAGE, PortConnectorType), sizeof(UCHAR) },
    { FIELD_OFFSET(ACPI_UPC_PACKAGE, Reserved0),         sizeof(ULONG) },
    { FIELD_OFFSET(ACPI_UPC_PACKAGE, Reserved1),         sizeof(ULONG) },
};



__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBACPI_EvaluateDsmMethod(
    __in
        PHUB_FDO_CONTEXT            HubFdoContext,
    __in
        PANSI_STRING                AcpiDeviceName,
    __in 
        const GUID *                Guid,
    __in
        ULONG                       RevisionID,
    __in 
        ULONG                       FunctionIndex,
    __in
        WDFMEMORY                   AcpiEvalOutputBufferMemory
    )
/*++

Routine Description:

    This routine evaluates a _DSM method.

Arguments:


Return Value:

    NTSTATUS

--*/
{
    NTSTATUS                            status;
    WDFMEMORY                           acpiEvalInputBufferMemory;
    PACPI_EVAL_INPUT_BUFFER_COMPLEX_EX  acpiEvalInputBuffer;
    SIZE_T                              acpiEvalInputBufferSize;
    PACPI_EVAL_OUTPUT_BUFFER            acpiEvalOutputBuffer;
    WDFREQUEST                          request;
    WDFIOTARGET                         target;
    WDF_REQUEST_SEND_OPTIONS            options;
    WDF_OBJECT_ATTRIBUTES               attributes;
    PACPI_METHOD_ARGUMENT               argument;

    PAGED_CODE ();

    TRY {
        
        request = NULL;

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = HubFdoContext->HubFdoDevice;

        acpiEvalInputBuffer = NULL;
        acpiEvalInputBufferSize = FIELD_OFFSET(ACPI_EVAL_INPUT_BUFFER_COMPLEX_EX, Argument) +  
                                  ACPI_METHOD_ARGUMENT_LENGTH(sizeof(GUID)) +  
                                  ACPI_METHOD_ARGUMENT_LENGTH(sizeof(ULONG)) +  
                                  ACPI_METHOD_ARGUMENT_LENGTH(sizeof(ULONG)) +  
                                  ACPI_METHOD_ARGUMENT_LENGTH(sizeof(ULONG));
        
        status = WdfMemoryCreate(&attributes,
                                 NonPagedPool,
                                 USBHUB3_HUB_TAG,
                                 acpiEvalInputBufferSize,
                                 &acpiEvalInputBufferMemory,
                                 (PVOID)&acpiEvalInputBuffer);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Error allocating memory for ACPI_EVAL_INPUT_BUFFER_COMPLEX_EX %!STATUS!",
                       status);
            LEAVE;
        }
        
        //
        // verify that device name, delimiter, method name, and zero terminator
        // will fit into the input buffer method name field
        //
        if ((AcpiDeviceName->Length + 1 + sizeof(ULONG) + 1) > sizeof(acpiEvalInputBuffer->MethodName)) {

            status = STATUS_INVALID_PARAMETER;

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Input buffer for IOCTL_ACPI_EVAL_METHOD_EX is too small");
            LEAVE;
        }

        //
        // initialize input buffer
        //
        RtlZeroMemory(acpiEvalInputBuffer, acpiEvalInputBufferSize);

        acpiEvalInputBuffer->Signature = ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE_EX;
        acpiEvalInputBuffer->Size = sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX_EX);
        acpiEvalInputBuffer->ArgumentCount = 4;

        //
        // build full method name from device name and four byte method name
        //
        RtlStringCbCopyNA(acpiEvalInputBuffer->MethodName,
                          sizeof(acpiEvalInputBuffer->MethodName),
                          AcpiDeviceName->Buffer,
                          AcpiDeviceName->Length);

        RtlStringCbCatNA(acpiEvalInputBuffer->MethodName,
                         sizeof(acpiEvalInputBuffer->MethodName),
                         ".",
                         sizeof(UCHAR));
                       
        RtlStringCbCatNA(acpiEvalInputBuffer->MethodName,
                         sizeof(acpiEvalInputBuffer->MethodName),
                         "_DSM",
                         sizeof(ULONG));

        target = WdfDeviceGetIoTarget(HubFdoContext->HubFdoDevice);

        request = NULL;
        status = WdfRequestCreate(WDF_NO_OBJECT_ATTRIBUTES, 
                                  target, 
                                  &request);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Failed to create request for IOCTL_ACPI_EVAL_METHOD_EX %!STATUS!",
                       status);
            LEAVE;
        }

        argument = &acpiEvalInputBuffer->Argument[0];
        ACPI_METHOD_SET_ARGUMENT_BUFFER(argument,
                                        Guid,
                                        sizeof(GUID));

        argument = ACPI_METHOD_NEXT_ARGUMENT(argument);
        ACPI_METHOD_SET_ARGUMENT_INTEGER(argument, RevisionID);

        argument = ACPI_METHOD_NEXT_ARGUMENT(argument);
        ACPI_METHOD_SET_ARGUMENT_INTEGER(argument, FunctionIndex);

        argument = ACPI_METHOD_NEXT_ARGUMENT(argument);
        argument->Type = ACPI_METHOD_ARGUMENT_PACKAGE;
        argument->DataLength = 0;

        //
        // Build the I/O request
        //
        status = WdfIoTargetFormatRequestForInternalIoctl(target,
                                                          request,
                                                          IOCTL_ACPI_EVAL_METHOD_EX,
                                                          acpiEvalInputBufferMemory,
                                                          NULL,
                                                          AcpiEvalOutputBufferMemory,
                                                          NULL);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Failed to format IOCTL_ACPI_EVAL_METHOD_EX request %!STATUS!",
                       status);
            LEAVE;
        }

        WDF_REQUEST_SEND_OPTIONS_INIT(&options, 
                                      WDF_REQUEST_SEND_OPTION_SYNCHRONOUS);

        if (WdfRequestSend(request, target, &options) == FALSE) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Failed to send IOCTL_ACPI_EVAL_METHOD_EX");
        }

        status = WdfRequestGetStatus(request);

        if (!NT_SUCCESS(status) && (status !=  STATUS_NO_SUCH_DEVICE)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "IOCTL_ACPI_EVAL_METHOD_EX request failed %!STATUS!",
                       status);
            LEAVE;
        }

        acpiEvalOutputBuffer = (PACPI_EVAL_OUTPUT_BUFFER)WdfMemoryGetBuffer(AcpiEvalOutputBufferMemory, NULL);

        //
        // verify returned output buffer
        //
        if ((acpiEvalOutputBuffer->Signature != ACPI_EVAL_OUTPUT_BUFFER_SIGNATURE) ||
            (acpiEvalOutputBuffer->Count == 0)) {
            
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Returned output buffer for IOCTL_ACPI_EVAL_METHOD_EX is not valid");            
            
            status = STATUS_ACPI_INVALID_DATA;
            LEAVE;
        }
    
    } FINALLY {

        if (request != NULL) {
            WdfObjectDelete(request);
        }

        if (acpiEvalInputBufferMemory != NULL) {
            WdfObjectDelete(acpiEvalInputBufferMemory);
        }
    }
    
    return status;    
}



__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBACPI_EvalAcpiMethodEx (
    __in
        PHUB_FDO_CONTEXT            HubFdoContext,
    __in
        PANSI_STRING                AcpiDeviceName,
    __in    
        ULONG                       AcpiMethodName,
    __in
        WDFMEMORY                   AcpiEvalOutputBufferMemory
    )
/*++

Routine Description:

    This routine evaluates any ACPI method for a device in the ACPI namespace.

    This is accomplished by sending a request down the hub driver stack for the
    ACPI filter to handle.

Arguments:


Return Value:

    NTSTATUS

--*/
{
    NTSTATUS                            status;
    WDFMEMORY                           acpiEvalInputBufferMemory;
    PACPI_EVAL_INPUT_BUFFER_COMPLEX_EX  acpiEvalInputBuffer;
    PACPI_EVAL_OUTPUT_BUFFER            acpiEvalOutputBuffer;
    WDFREQUEST                          request;
    WDFIOTARGET                         target;
    WDF_REQUEST_SEND_OPTIONS            options;
    WDF_OBJECT_ATTRIBUTES               attributes;

    PAGED_CODE ();

    TRY {
        
        request = NULL;

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = HubFdoContext->HubFdoDevice;

        acpiEvalInputBuffer = NULL;
        status =
            WdfMemoryCreate(&attributes,
                            NonPagedPool,
                            USBHUB3_HUB_TAG,
                            sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX_EX),
                            &acpiEvalInputBufferMemory,
                            (PVOID)&acpiEvalInputBuffer);
 
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Error allocating memory for ACPI_EVAL_INPUT_BUFFER_COMPLEX_EX %!STATUS!",
                       status);
            LEAVE;
        }
        
        //
        // verify that device name, delimiter, method name, and zero terminator
        // will fit into the input buffer method name field
        //
        if ((AcpiDeviceName->Length + 1 + sizeof(ULONG) + 1) > sizeof(acpiEvalInputBuffer->MethodName)) {

            status = STATUS_INVALID_PARAMETER;

            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Input buffer for IOCTL_ACPI_EVAL_METHOD_EX is too small");
            LEAVE;
        }

        //
        // initialize input buffer
        //
        RtlZeroMemory(acpiEvalInputBuffer, sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX_EX));

        acpiEvalInputBuffer->Signature = ACPI_EVAL_INPUT_BUFFER_COMPLEX_SIGNATURE_EX;
        acpiEvalInputBuffer->Size = sizeof(ACPI_EVAL_INPUT_BUFFER_COMPLEX_EX);

        //
        // build full method name from device name and four byte method name
        //
        RtlStringCbCopyNA(acpiEvalInputBuffer->MethodName,
                          sizeof(acpiEvalInputBuffer->MethodName),
                          AcpiDeviceName->Buffer,
                          AcpiDeviceName->Length);

        RtlStringCbCatNA(acpiEvalInputBuffer->MethodName,
                         sizeof(acpiEvalInputBuffer->MethodName),
                         ".",
                         sizeof(UCHAR));
                       
        RtlStringCbCatNA(acpiEvalInputBuffer->MethodName,
                         sizeof(acpiEvalInputBuffer->MethodName),
                         (STRSAFE_PCNZCH)&AcpiMethodName,
                         sizeof(ULONG));

        target = WdfDeviceGetIoTarget(HubFdoContext->HubFdoDevice);

        request = NULL;
        status = WdfRequestCreate(WDF_NO_OBJECT_ATTRIBUTES, 
                                  target, 
                                  &request);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Failed to create request for IOCTL_ACPI_EVAL_METHOD_EX %!STATUS!",
                       status);
            LEAVE;
        }

        //
        // Build the I/O request
        //
        status = WdfIoTargetFormatRequestForInternalIoctl(target,
                                                          request,
                                                          IOCTL_ACPI_EVAL_METHOD_EX,
                                                          acpiEvalInputBufferMemory,
                                                          NULL,
                                                          AcpiEvalOutputBufferMemory,
                                                          NULL);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Failed to format IOCTL_ACPI_EVAL_METHOD_EX request %!STATUS!",
                       status);
            LEAVE;
        }

        WDF_REQUEST_SEND_OPTIONS_INIT(&options, 
                                      WDF_REQUEST_SEND_OPTION_SYNCHRONOUS);

        if (WdfRequestSend(request, target, &options) == FALSE) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Failed to send IOCTL_ACPI_EVAL_METHOD_EX");
        }

        status = WdfRequestGetStatus(request);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "IOCTL_ACPI_EVAL_METHOD_EX request failed %!STATUS!",
                       status);
            LEAVE;
        }

        acpiEvalOutputBuffer = (PACPI_EVAL_OUTPUT_BUFFER)WdfMemoryGetBuffer(AcpiEvalOutputBufferMemory, NULL);

        //
        // verify returned output buffer
        //
        if ((acpiEvalOutputBuffer->Signature != ACPI_EVAL_OUTPUT_BUFFER_SIGNATURE) ||
            (acpiEvalOutputBuffer->Count == 0)) {
            
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Returned output buffer for IOCTL_ACPI_EVAL_METHOD_EX is not valid");            
            
            status = STATUS_ACPI_INVALID_DATA;
            LEAVE;
        }
    
    } FINALLY {

        if (request != NULL) {
            WdfObjectDelete(request);
        }

        if (acpiEvalInputBufferMemory != NULL) {
            WdfObjectDelete(acpiEvalInputBufferMemory);
        }
    }
    
    return status;    
}


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBACPI_EnumChildren(
    __in
        PHUB_FDO_CONTEXT                    HubFdoContext,
    __in
        WDFMEMORY                           AcpiEnumOutputBufferMemory
    )
/*++

Routine Description:

    This routine enumerates child devices in the ACPI namespace under the specified hub.
    
    This is accomplished by sending a request down the hub driver stack for the
    ACPI filter to handle.

    This function is called twice.  Once to determine the size needed for the output buffer, and then 
    again to enumerate the children.

Arguments:


Return Value:

   STATUS_SUCCESS - Children were successfully enumerated.
   STATUS_BUFFER_OVERFLOW - Expected when querying for the size of the output buffer.
   STATUS_NOT_SUPPORTED - Expected if host controller or hub is not in ACPI namespace.
   Other errors indicate an unexpected failure.

--*/
{
    NTSTATUS                            status;
    PACPI_ENUM_CHILDREN_INPUT_BUFFER    acpiEnumInputBuffer;
    PACPI_ENUM_CHILDREN_OUTPUT_BUFFER   acpiEnumOutputBuffer;
    WDFMEMORY                           acpiEnumInputBufferMemory;
    WDFREQUEST                          request;
    WDFIOTARGET                         target;
    WDF_REQUEST_SEND_OPTIONS            options;
    WDF_OBJECT_ATTRIBUTES               attributes;
    size_t                              acpiEnumOutputSize;

    PAGED_CODE ();

    TRY {

        request = NULL;
        
        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = HubFdoContext->HubFdoDevice;

        acpiEnumInputBuffer = NULL;
        status =
            WdfMemoryCreate(&attributes,
                            NonPagedPool,
                            USBHUB3_HUB_TAG,
                            sizeof(ACPI_ENUM_CHILDREN_INPUT_BUFFER),
                            &acpiEnumInputBufferMemory,
                            (PVOID)&acpiEnumInputBuffer);
 
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Error allocating memory for ACPI_ENUM_CHILDREN_INPUT_BUFFER %!STATUS!",
                       status);
            LEAVE;
        }
        //
        // initialize input buffer
        //
        RtlZeroMemory(acpiEnumInputBuffer, sizeof(ACPI_ENUM_CHILDREN_INPUT_BUFFER));

        acpiEnumInputBuffer->Signature = ACPI_ENUM_CHILDREN_INPUT_BUFFER_SIGNATURE;
        acpiEnumInputBuffer->Flags = ENUM_CHILDREN_IMMEDIATE_ONLY;

        target = WdfDeviceGetIoTarget(HubFdoContext->HubFdoDevice);

        request = NULL;
        status = WdfRequestCreate(WDF_NO_OBJECT_ATTRIBUTES, 
                                  target, 
                                  &request);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Failed to create request for IOCTL_ACPI_ENUM_CHILDREN %!STATUS!",
                       status);
            LEAVE;
        }

        //
        // Build the I/O request
        //
        status = WdfIoTargetFormatRequestForInternalIoctl(target,
                                                          request,
                                                          IOCTL_ACPI_ENUM_CHILDREN,
                                                          acpiEnumInputBufferMemory,
                                                          NULL,
                                                          AcpiEnumOutputBufferMemory,
                                                          NULL);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Failed to format IOCTL_ACPI_ENUM_CHILDREN request %!STATUS!",
                       status);
            LEAVE;
        }

        WDF_REQUEST_SEND_OPTIONS_INIT(&options, 
                                      WDF_REQUEST_SEND_OPTION_SYNCHRONOUS);

        if (WdfRequestSend(request, target, &options) == FALSE) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Failed to send IOCTL_ACPI_ENUM_CHILDREN");
        }

        status = WdfRequestGetStatus(request);

        if (!NT_SUCCESS(status)) {
            if ((status != STATUS_BUFFER_OVERFLOW) && (status != STATUS_NOT_SUPPORTED)) {
                DbgTraceTo(HubFdoContext->IfrLog,
                           TL_ERROR,
                           Hub,
                           "IOCTL_ACPI_ENUM_CHILDREN request failed %!STATUS!",
                           status);
                LEAVE;
            }

            if (status == STATUS_NOT_SUPPORTED) {
                LEAVE;
            }
        }

        acpiEnumOutputBuffer = 
            (PACPI_ENUM_CHILDREN_OUTPUT_BUFFER)WdfMemoryGetBuffer(AcpiEnumOutputBufferMemory, 
                                                                  &acpiEnumOutputSize);
        
        //
        // verify returned output buffer based on supplied buffer size, where an
        // unextended buffer indicates that the required size is being queried
        //
        if (acpiEnumOutputSize == sizeof(ACPI_ENUM_CHILDREN_OUTPUT_BUFFER)) {
            
            if (status != STATUS_BUFFER_OVERFLOW) {
                
                DbgTraceTo(HubFdoContext->IfrLog,
                           TL_ERROR,
                           Hub,
                           "Buffer overflow returned for IOCTL_ACPI_ENUM_CHILDREN request %!STATUS!",
                           status);
                
                status = STATUS_ACPI_INVALID_DATA;
                LEAVE;
            }

            //
            // required buffer size is returned in the NumberOfChildren field
            //
            if ((acpiEnumOutputBuffer->Signature != ACPI_ENUM_CHILDREN_OUTPUT_BUFFER_SIGNATURE) ||
                (acpiEnumOutputBuffer->NumberOfChildren < sizeof(ACPI_ENUM_CHILDREN_OUTPUT_BUFFER))) {

                DbgTraceTo(HubFdoContext->IfrLog,
                           TL_ERROR,
                           Hub,
                           "Invalid data returned for IOCTL_ACPI_ENUM_CHILDREN request");
                
                status = STATUS_ACPI_INVALID_DATA;
                LEAVE;
            }
        
        } else {
        
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            //
            // should expect at least one child or something is wrong
            //
            if ((acpiEnumOutputBuffer->Signature != ACPI_ENUM_CHILDREN_OUTPUT_BUFFER_SIGNATURE) ||
                (acpiEnumOutputBuffer->NumberOfChildren == 0) ||
                (WdfRequestGetInformation(request) != acpiEnumOutputSize)) {
            
                DbgTraceTo(HubFdoContext->IfrLog,
                           TL_ERROR,
                           Hub,
                           "No children returned for IOCTL_ACPI_ENUM_CHILDREN request");

                status = STATUS_ACPI_INVALID_DATA;
                LEAVE;
            }
        }

    } FINALLY {

        if (request != NULL) {
            WdfObjectDelete(request);
        }

        if (acpiEnumInputBufferMemory != NULL) {
            WdfObjectDelete(acpiEnumInputBufferMemory);
        }
    }
    
    return status;    
}


__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
HUBACPI_GetAcpiPortAttributes(
    __in
        PHUB_FDO_CONTEXT                HubFdoContext
    )
/*++

Routine Description:

    Retrieve extra information about the hub ports from ACPI and store in the hub FDO context.

Arguments:

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS                            status;
    PACPI_ENUM_CHILDREN_OUTPUT_BUFFER   acpiEnumOutputSizeBuffer;
    WDFMEMORY                           acpiEnumOutputSizeBufferMemory;
    PACPI_ENUM_CHILDREN_OUTPUT_BUFFER   acpiEnumOutputBuffer;
    WDFMEMORY                           acpiEnumOutputBufferMemory;
    ULONG                               acpiEnumOutputSize;
    PACPI_EVAL_OUTPUT_BUFFER            acpiEvalOutputBuffer;
    WDFMEMORY                           acpiEvalOutputBufferMemory;
    ULONG                               acpiEvalOutputSize;
    PACPI_ENUM_CHILD                    acpiEnumChild;
    ULONG                               childIndex;
    ANSI_STRING                         acpiDeviceName;
    USHORT                              port;
    PPORT_CONTEXT                       portContext;
    PACPI_METHOD_ARGUMENT               acpiPackageElement;
    ULONG                               elementIndex;
    PUCHAR                              acpiUpcPackageField;
    PACPI_PLD_BUFFER                    acpiPldBuffer;
    WDF_OBJECT_ATTRIBUTES               attributes;
    BOOLEAN                             matchFound;

    PAGED_CODE ();

    TRY {

        acpiEnumOutputBufferMemory = NULL;
        acpiEvalOutputBufferMemory = NULL;
        acpiEnumOutputSizeBufferMemory = NULL;

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = HubFdoContext->HubFdoDevice;

        status =
            WdfMemoryCreate(&attributes,
                            NonPagedPool,
                            USBHUB3_HUB_TAG,
                            sizeof(ACPI_ENUM_CHILDREN_OUTPUT_BUFFER),
                            &acpiEnumOutputSizeBufferMemory,
                            (PVOID)&acpiEnumOutputSizeBuffer);
 
        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Error allocating memory for ACPI_ENUM_CHILDREN_OUTPUT_BUFFER %!STATUS!",
                       status);
            LEAVE;
        }

        //
        // query size of output buffer required to enumerate ACPI device children
        //
        status = HUBACPI_EnumChildren(HubFdoContext, acpiEnumOutputSizeBufferMemory);

        if (status == STATUS_NOT_SUPPORTED) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_INFO,
                       Hub,
                       "Non-ACPI Hub");
            LEAVE;
        }

        if (status != STATUS_BUFFER_OVERFLOW) {
            LEAVE;
        }

        DbgTraceTo(HubFdoContext->IfrLog,
                   TL_INFO,
                   Hub,
                   "ACPI Hub");

        //
        // allocate output buffer to hold all enumerated ACPI device children
        //
        acpiEnumOutputSize = acpiEnumOutputSizeBuffer->NumberOfChildren;

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = HubFdoContext->HubFdoDevice;

        status =
            WdfMemoryCreate(&attributes,
                            NonPagedPool,
                            USBHUB3_HUB_TAG,
                            acpiEnumOutputSize,
                            &acpiEnumOutputBufferMemory,
                            (PVOID)&acpiEnumOutputBuffer);
        

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Unable to allocate memory for ACPI children");
            LEAVE;
        }

        //
        // enumerate ACPI device children
        //
        status = HUBACPI_EnumChildren(HubFdoContext, acpiEnumOutputBufferMemory);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Second call to HUBACPI_EnumChildren failed %!STATUS!",
                       status);
            LEAVE;
        }

        //
        // allocate output buffer to hold arbitrary results from method evaluation
        //
        acpiEvalOutputSize = sizeof(ACPI_EVAL_OUTPUT_BUFFER) + ACPI_METHOD_OUTPUT_BUFFER_SIZE;

        WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        attributes.ParentObject = HubFdoContext->HubFdoDevice;

        status =
            WdfMemoryCreate(&attributes,
                            NonPagedPool,
                            USBHUB3_HUB_TAG,
                            acpiEvalOutputSize,
                            &acpiEvalOutputBufferMemory,
                            (PVOID)&acpiEvalOutputBuffer);

        if (!NT_SUCCESS(status)) {
            DbgTraceTo(HubFdoContext->IfrLog,
                       TL_ERROR,
                       Hub,
                       "Unable to allocate memory for method evaluation");
            LEAVE;
        }

        //
        // skip over first ACPI device child that represents the hub
        //
        acpiEnumChild = &acpiEnumOutputBuffer->Children[0];

        for (childIndex = 1;
             childIndex < acpiEnumOutputBuffer->NumberOfChildren;
             childIndex++) {
        
            //
            // get next ACPI child
            //
            acpiEnumChild = ACPI_ENUM_CHILD_NEXT(acpiEnumChild);

            //
            // skip ACPI child without any children since there is no ADR object
            //
            if ((acpiEnumChild->Flags & ACPI_OBJECT_HAS_CHILDREN) == 0) {
                continue;
            }

            if (acpiEnumChild->NameLength == 0) {
                continue;
            }

            //
            // initialize ACPI child device name
            //
            acpiDeviceName.Buffer = acpiEnumChild->Name;
            acpiDeviceName.MaximumLength = (USHORT)acpiEnumChild->NameLength;
            acpiDeviceName.Length = acpiDeviceName.MaximumLength - 1;

            //
            // evaluate ADR method on ACPI child
            //
            status = HUBACPI_EvalAcpiMethodEx(HubFdoContext,
                                              &acpiDeviceName,
                                              (ULONG)'RDA_',
                                              acpiEvalOutputBufferMemory);

            if (!NT_SUCCESS(status)) {
                status = STATUS_SUCCESS;
                continue;
            }

            //
            // validate that output buffer contains an integer argument
            //
            if ((acpiEvalOutputBuffer->Count == 0) ||
                (acpiEvalOutputBuffer->Argument[0].Type != ACPI_METHOD_ARGUMENT_INTEGER)) {
            
                continue;
            }

            //
            // map ADR address value to a valid port on the hub
            //
            port = (USHORT)acpiEvalOutputBuffer->Argument[0].Argument;

            if ((port < 1) || (port > HubFdoContext->HubInformation.NumberOfPorts)) {
                DbgTraceTo(HubFdoContext->IfrLog,
                           TL_ERROR,
                           HwcPlatform,
                           "HW_COMPLIANCE_PLATFORM: ACPI port device has invalid port number %d",
                           port);
                continue;
            }

            //
            // Find the port context for the port
            //
            matchFound = FALSE;

            FOR_ALL_IN_LIST(PORT_CONTEXT,
                            &HubFdoContext->MuxContext.PSMListHead,
                            MuxContext.ListEntry,
                            portContext) {

                if (portContext->PortInformation.PortNumber == port) {
                    matchFound = TRUE;
                    break;
                }
            }

            //
            // If we didn't find a matching port context, go to the next port object
            //
            if (matchFound == FALSE) {
                continue;
            }

            status = HUBACPI_EvaluateDsmMethod(HubFdoContext,
                                               &acpiDeviceName,
                                               &GUID_USB_ACPI_DSM,
                                               USB_ACPI_DSM_REVISION_0,
                                               USB_ACPI_DSM_INTERCONNECT_INDEX,
                                               acpiEvalOutputBufferMemory);

            if (NT_SUCCESS(status)) {
                if (acpiEvalOutputBuffer->Argument[0].Type != ACPI_METHOD_ARGUMENT_INTEGER) {

                    DbgTraceTo(HubFdoContext->IfrLog,
                               TL_ERROR,
                               HwcPlatform,
                               "HW_COMPLIANCE_PLATFORM: ACPI _DSM method for port %d returned invalid data type",
                               port);
                } else { 
                    ULONG   interconnectType;

                    interconnectType = (USHORT)acpiEvalOutputBuffer->Argument[0].Argument;
                    portContext->PortInformation.InterconnectType = interconnectType;

                    switch (interconnectType) {
                    case USB_ACPI_DSM_INTERCONNECT_TYPE_STANDARD:
                        break;

                    case USB_ACPI_DSM_INTERCONNECT_TYPE_HSIC:
                        DbgTraceTo(HubFdoContext->IfrLog,
                                   TL_INFO,
                                   Hub,
                                   "Port %d is HSIC",
                                   port);
                        break;

                    case USB_ACPI_DSM_INTERCONNECT_TYPE_SSIC:
                        DbgTraceTo(HubFdoContext->IfrLog,
                                   TL_INFO,
                                   Hub,
                                   "Port %d is SSIC",
                                   port);
                        break;

                    default:
                        DbgTraceTo(HubFdoContext->IfrLog,
                                   TL_ERROR,
                                   HwcPlatform,
                                   "HW_COMPLIANCE_PLATFORM: Unknown ACPI _DSM interconnect type %d",
                                   interconnectType);
                    }
                }
            }
            
            //
            // evaluate UPC (USB port capabilities) method on ACPI child
            //
            status = HUBACPI_EvalAcpiMethodEx(HubFdoContext,
                                              &acpiDeviceName,
                                              (ULONG)'CPU_',
                                              acpiEvalOutputBufferMemory);

            if (!NT_SUCCESS(status)) {

                DbgTraceTo(HubFdoContext->IfrLog,
                           TL_INFO,
                           Hub,
                           "Port %d does not have a _UPC method",
                           port);

                EventWriteUSBHUB3_ETW_EVENT_HUB_PORT_ACPI_UPC_FAILURE(
                    0,
                    HubFdoContext->HubFdoDevice,
                    port,
                    status);
            
                status = STATUS_SUCCESS;
                continue;
            }

            //
            // validate that output buffer contains package arguments
            //
            if (acpiEvalOutputBuffer->Count == 0) {

                EventWriteUSBHUB3_ETW_EVENT_HUB_PORT_ACPI_UPC_FAILURE(
                    0,
                    HubFdoContext->HubFdoDevice,
                    port,
                    (ULONG)STATUS_ACPI_INCORRECT_ARGUMENT_COUNT);

                DbgTraceTo(HubFdoContext->IfrLog,
                           TL_ERROR,
                           HwcPlatform,
                           "HW_COMPLIANCE_PLATFORM: Port %d has invalid _UPC; the argument count in the output buffer is 0",
                           port);
                continue;
            }

            //
            // traverse package elements to populate UPC package fields
            //
            for (acpiPackageElement = acpiEvalOutputBuffer->Argument,
                 elementIndex = 0;
                 ((PUCHAR)acpiPackageElement < ((PUCHAR)acpiEvalOutputBuffer + acpiEvalOutputBuffer->Length)) &&
                  (elementIndex < acpiEvalOutputBuffer->Count);
                 acpiPackageElement = ACPI_METHOD_NEXT_ARGUMENT(acpiPackageElement),
                 elementIndex++) {
            
                //
                // check for excess package elements
                //
                if (elementIndex >= RTL_NUMBER_OF_V1(acpiUpcPackageFields)) {
                    status = STATUS_ACPI_INCORRECT_ARGUMENT_COUNT;
                    break;
                }

                //
                // package elements should all be integers
                //
                if (acpiPackageElement->Type != ACPI_METHOD_ARGUMENT_INTEGER) {
                    status = STATUS_ACPI_INVALID_ARGTYPE;
                    break;
                }

                //
                // get offset into UPC package field
                //
                __analysis_assume(acpiUpcPackageFields[elementIndex].Offset <= sizeof(ACPI_UPC_PACKAGE) - sizeof(ULONG));

                acpiUpcPackageField = (PUCHAR)&portContext->AcpiUpcPackage + acpiUpcPackageFields[elementIndex].Offset;

                //
                // store package element data in UPC package field
                //
                if (acpiUpcPackageFields[elementIndex].Size == sizeof(UCHAR)) {
        
                    *acpiUpcPackageField = (UCHAR)(acpiPackageElement->Argument & 0xFF);
            
                } else if (acpiUpcPackageFields[elementIndex].Size == sizeof(ULONG)) {
                    
                    *(PULONG)acpiUpcPackageField = acpiPackageElement->Argument;
            
                } else {
                
                    status = STATUS_ACPI_INVALID_DATA;
                    break;
                }
            }

            if (!NT_SUCCESS(status)) {

                DbgTraceTo(HubFdoContext->IfrLog,
                           TL_ERROR,
                           HwcPlatform,
                           "HW_COMPLIANCE_PLATFORM: Port %d has _UPC with invalid package elements: %!STATUS!",
                           port,
                           status);

                EventWriteUSBHUB3_ETW_EVENT_HUB_PORT_ACPI_UPC_FAILURE(
                    0,
                    HubFdoContext->HubFdoDevice,
                    port,
                    status);

                status = STATUS_SUCCESS;
                continue;
            }

            INTERLOCKED_SET_BIT_FIELD(PORT_FLAGS, &portContext->PortInformation.PortFlags, PortFlagAcpiUpcValid);

            if (portContext->AcpiUpcPackage.PortIsConnectable == 0) {
    
                //
                // port is not connectable, so any device connected to it is not removable
                //
                INTERLOCKED_CLEAR_BIT_FIELD(PORT_FLAGS, &portContext->PortInformation.PortFlags, PortFlagRemovable);
   
            } else {

                //
                // Assume it is removable, unless _PLD says otherwise
                //
                INTERLOCKED_SET_BIT_FIELD(PORT_FLAGS, &portContext->PortInformation.PortFlags, PortFlagRemovable);
            }

            if (portContext->AcpiUpcPackage.PortConnectorType == ACPI_UPC_TYPE_C_USB_20_AND_SS_WITHOUT_SWITCH) {
                INTERLOCKED_SET_BIT_FIELD(PORT_FLAGS, &portContext->PortInformation.PortFlags, PortFlagTypeCWithoutSwitch);
            }

            //
            // evaluate PLD (physical location descriptor) method on ACPI child
            //
            status = HUBACPI_EvalAcpiMethodEx(HubFdoContext,
                                              &acpiDeviceName,
                                              (ULONG)'DLP_',
                                              acpiEvalOutputBufferMemory);

            if (!NT_SUCCESS(status)) {
                
                DbgTraceTo(HubFdoContext->IfrLog,
                           TL_INFO,
                           Hub,
                           "Port %d does not have a _PLD method: %!STATUS!",
                           port,
                           status);


                EventWriteUSBHUB3_ETW_EVENT_HUB_PORT_ACPI_PLD_FAILURE(
                    0,
                    HubFdoContext->HubFdoDevice,
                    port,
                    status);
                
                status = STATUS_SUCCESS;
                continue;
            }

            //
            // validate that output buffer contains a buffer argument
            //
            if ((acpiEvalOutputBuffer->Count == 0) ||
                (acpiEvalOutputBuffer->Argument[0].Type != ACPI_METHOD_ARGUMENT_BUFFER) ||
                (acpiEvalOutputBuffer->Argument[0].DataLength < sizeof(ACPI_PLD_BUFFER))) {

                DbgTraceTo(HubFdoContext->IfrLog,
                           TL_ERROR,
                           HwcPlatform,
                           "HW_COMPLIANCE_PLATFORM: Port %d has invalid _PLD",
                           port);

                EventWriteUSBHUB3_ETW_EVENT_HUB_PORT_ACPI_PLD_FAILURE(
                    0,
                    HubFdoContext->HubFdoDevice,
                    port,
                    status);

                continue;
            }
       
            //
            // map PLD buffer bitfield over buffer data
            //
            acpiPldBuffer = (PACPI_PLD_BUFFER)acpiEvalOutputBuffer->Argument[0].Data;

            portContext->AcpiPldPackage = *acpiPldBuffer;            
            INTERLOCKED_SET_BIT_FIELD(PORT_FLAGS, &portContext->PortInformation.PortFlags, PortFlagAcpiPldValid);

            //
            // device on this port cannot be removed if the port itself is not
            // visible to the user, even if it is identified as connectable by UPC
            //
            if (acpiPldBuffer->UserVisible == 0) {
                INTERLOCKED_CLEAR_BIT_FIELD(PORT_FLAGS, &portContext->PortInformation.PortFlags, PortFlagRemovable);
            }

            //
            // At least one port has a valid _PLD, so mark the hub as being in the ACPI namespace
            //
            INTERLOCKED_SET_BIT_FIELD(HUB_FLAGS, &HubFdoContext->HubInformation.HubFlags, InAcpiNamespace);
        }

    } FINALLY {

        //
        // free buffers
        //
        if (acpiEnumOutputBufferMemory != NULL) {
            WdfObjectDelete(acpiEnumOutputBufferMemory);
        }
    
        if (acpiEvalOutputBufferMemory != NULL) {
            WdfObjectDelete(acpiEvalOutputBufferMemory);
        }

        if (acpiEnumOutputSizeBufferMemory != NULL) {
            WdfObjectDelete(acpiEnumOutputSizeBufferMemory);
        }

    }
    
    return STATUS_SUCCESS;
}