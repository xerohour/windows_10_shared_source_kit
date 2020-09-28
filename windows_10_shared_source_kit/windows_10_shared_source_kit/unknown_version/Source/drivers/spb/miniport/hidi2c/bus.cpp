/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    bus.cpp

Abstract:

   Contains definitions for SPB function and callbacks

Author:

    Arvind Aiyar March 2011

Environment:

    Kernel-mode Driver Framework

--*/

#include "internal.h"

#include "bus.tmh"

NTSTATUS
SpbInitialize(
    _In_ PDEVICE_CONTEXT    FxDeviceContext,
    _In_ WDFCMRESLIST       FxResourcesRaw,
    _In_ WDFCMRESLIST       FxResourcesTranslated
    )
/*++
 
  Routine Description:

    This routine parses the list of SPB resources needed to find the
    controller driver. If successful, it opens an I/O
    target to the controller driver.

  Arguments:

    FxDeviceContext - Pointer to the current device context 
    FxResourcesRaw - list of translated hardware resources that 
        the PnP manager has assigned to the device
    FxResourcesTranslated - list of raw hardware resources that 
        the PnP manager has assigned to the device

  Return Value:

    NTSTATUS Status indicating success or failure

--*/
{
    FuncEntry(TRACE_FLAG_SPB);

    UNREFERENCED_PARAMETER(FxResourcesRaw);
    
    NTSTATUS status = STATUS_SUCCESS;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR pDescriptor = NULL;

    ULONG interruptIndex = 0;
    
    //
    // Check for SPB(I2C) and Interrupt resources
    //
    {
        BOOLEAN connFound = FALSE;
        BOOLEAN interruptFound = FALSE;
        
        ULONG resourceCount = WdfCmResourceListGetCount(FxResourcesTranslated);

        for (ULONG i=0; ((connFound == FALSE) || (interruptFound == FALSE)) && (i < resourceCount); i++)
        {
            pDescriptor = WdfCmResourceListGetDescriptor(FxResourcesTranslated, i);

            NT_ASSERTMSG("Resource descriptor is NULL", pDescriptor != NULL);

            switch(pDescriptor->Type)
            {
                case CmResourceTypeConnection:

                    //
                    // Check for I2C resource
                    //
                    if (pDescriptor->u.Connection.Class == CM_RESOURCE_CONNECTION_CLASS_SERIAL &&
                        pDescriptor->u.Connection.Type == CM_RESOURCE_CONNECTION_TYPE_SERIAL_I2C)
                    {
                        FxDeviceContext->SpbConnectionId.LowPart = pDescriptor->u.Connection.IdLowPart;
                        FxDeviceContext->SpbConnectionId.HighPart = pDescriptor->u.Connection.IdHighPart; 

                        connFound = TRUE;

                        TraceEvents(
                                TRACE_LEVEL_INFORMATION,
                                TRACE_FLAG_SPB,
                                "I2C resource found with connection id: 0x%llx",
                                FxDeviceContext->SpbConnectionId.QuadPart);
                    }

                    break;
                    
                    //
                    // Check for I2C resource
                    //
                case CmResourceTypeInterrupt:

                    interruptFound = TRUE;
                    interruptIndex = i;
                    
                    TraceEvents(
                            TRACE_LEVEL_INFORMATION,
                            TRACE_FLAG_SPB,
                            "Interrupt resource found at index: %d",interruptIndex);

                    break;

                default:
                    // Don't care about other descriptors
                    break;
            }
        }
    
        //
        // Fail if either connection or interrupt resource
        // was not found
        //
        if ((connFound == FALSE) || (interruptFound == FALSE))
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "Failed finding required resources "
                "status:%!STATUS!",
                status);
            
            DeviceReportPnpProblem(FxDeviceContext->FxDevice, HIDI2C_RESOURCE_MISSING_ERROR);

            goto exit;
        }
    }

    //
    // Create the SPB I/O target
    //
    {
        WDF_OBJECT_ATTRIBUTES targetAttributes;
        WDF_OBJECT_ATTRIBUTES_INIT(&targetAttributes);
        
        status = WdfIoTargetCreate(
                        FxDeviceContext->FxDevice,
                        &targetAttributes,
                        &FxDeviceContext->SpbIoTarget);

        if (!NT_SUCCESS(status)) 
        {
            TraceEvents(
                    TRACE_LEVEL_ERROR,
                    TRACE_FLAG_SPB,
                    "WdfIoTargetCreate failed creating SPB target "
                    "status:%!STATUS!",
                    status);

            WdfDeleteObjectSafe(FxDeviceContext->SpbIoTarget);
            goto exit;
        }

        //
        // Use the connectionId to create the full device path
        //
        DECLARE_UNICODE_STRING_SIZE(spbDevicePath, RESOURCE_HUB_PATH_SIZE);

        status = RESOURCE_HUB_CREATE_PATH_FROM_ID(
                                    &spbDevicePath,
                                    FxDeviceContext->SpbConnectionId.LowPart,
                                    FxDeviceContext->SpbConnectionId.HighPart);

        if (!NT_SUCCESS(status))
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "ResourceHub failed to create device path (%wZ) "
                "status:%!STATUS!",
                &spbDevicePath,
                status);
         
            goto exit;
        }

        //
        // Open the SPB IoTarget
        //
        WDF_IO_TARGET_OPEN_PARAMS  openParams;
        WDF_IO_TARGET_OPEN_PARAMS_INIT_OPEN_BY_NAME(
                                    &openParams,
                                    &spbDevicePath,
                                    (GENERIC_READ | GENERIC_WRITE));

        openParams.ShareAccess = 0;
        openParams.CreateDisposition = FILE_OPEN;
        openParams.FileAttributes = FILE_ATTRIBUTE_NORMAL;

        status = WdfIoTargetOpen(
                        FxDeviceContext->SpbIoTarget,
                        &openParams);

        if (!NT_SUCCESS(status)) 
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "WdfIoTargetOpen failed to open SPB target:0x%p "
                "status:%!STATUS!",
                FxDeviceContext->SpbIoTarget,
                status);

            goto exit;
        }
    }

    //
    // Create a SPB request for writes and reads to the SpbIoTarget
    //
    {
        //
        // Associate a request context with the request
        // SpbRequest is used for sending R/W/IOCTL to the SpbIoTarget, parent it such
        //
        WDF_OBJECT_ATTRIBUTES requestAttributes;
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&requestAttributes, REQUEST_CONTEXT);

        requestAttributes.ParentObject = FxDeviceContext->SpbIoTarget;
    
        status = WdfRequestCreate(
                            &requestAttributes,
                            FxDeviceContext->SpbIoTarget,
                            &FxDeviceContext->SpbRequest);

        if (!NT_SUCCESS(status))
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "WdfRequestCreate failed creating SPB request "
                "status:%!STATUS!",
                status);

            goto exit;
        }

        //
        // Initialize the request context with default values
        //
        PREQUEST_CONTEXT pRequestContext = GetRequestContext(FxDeviceContext->SpbRequest);
        pRequestContext->FxDevice = FxDeviceContext->FxDevice;
        pRequestContext->FxMemory = NULL;
    }
    
    //
    // Create an interrupt object for passive-level interrupt handling. 
    // The ISR is responsible for quiescing the interrupt
    // and the work-item does any "additional" work
    
    WDF_OBJECT_ATTRIBUTES lockAttributes;
    
    {
        //
        // Create a (passive-level) waitlock for the interrupt
        //
        WDFWAITLOCK interruptLock;
        WDF_OBJECT_ATTRIBUTES_INIT(&lockAttributes);

        lockAttributes.ParentObject = FxDeviceContext->FxDevice;

        status = WdfWaitLockCreate(
                        &lockAttributes,
                        &interruptLock);

        if (!NT_SUCCESS(status))
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_INIT,
                "WdfWaitLockCreate failed creating interrupt lock "
                "status:%!STATUS!",
                status);
            
            goto exit;
        }
        
        //
        // Create the passive-level interrupt
        // No DPC, PassiveHandling, InterruptWorkItem, WaitLock
        //
        WDF_INTERRUPT_CONFIG interruptConfig;

        WDF_INTERRUPT_CONFIG_INIT(
                        &interruptConfig,
                        OnInterruptIsr,
                        NULL);

        interruptConfig.PassiveHandling = TRUE;
        interruptConfig.WaitLock = interruptLock;
        interruptConfig.InterruptTranslated =  WdfCmResourceListGetDescriptor(
                                                    FxResourcesTranslated, 
                                                    interruptIndex);
        interruptConfig.InterruptRaw = WdfCmResourceListGetDescriptor(
                                                    FxResourcesRaw, 
                                                    interruptIndex);

        interruptConfig.EvtInterruptDpc = OnInterruptDpc;

        status = WdfInterruptCreate(
                        FxDeviceContext->FxDevice,
                        &interruptConfig,
                        WDF_NO_OBJECT_ATTRIBUTES,
                        &FxDeviceContext->ReadInterrupt);

        if (!NT_SUCCESS(status))
        {
            TraceEvents(
                    TRACE_LEVEL_ERROR, 
                    TRACE_FLAG_INIT,
                    "WdfInterruptCreate failed creating interrupt "
                    "status:%!STATUS!",
                    status);

            goto exit;
        }
    }

    //
    // Create a Spin Lock for synchronizing multiple interrupt
    // DPCs / workitems that may run in different thread contexts
    // and on different CPUs
    //
    {
        WDF_OBJECT_ATTRIBUTES_INIT(&lockAttributes);
        lockAttributes.ParentObject = FxDeviceContext->FxDevice;;

        status = WdfSpinLockCreate(
                        &lockAttributes,
                        &FxDeviceContext->InProgressLock);

        if (!NT_SUCCESS(status))
        {
            TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_INIT,
                "WdfSpinLockCreate failed creating inprogress spinlock "
                "status:%!STATUS!",
                status);

            goto exit;
        }
    }


exit:
    
    if (!NT_SUCCESS(status))
    {
        EventWrite_HIDI2C_ENUMERATION_SPB_FAILURE(NULL, FxDeviceContext->FxDevice, status);

        TraceLoggingBusDevice(
            status, 
            FxDeviceContext->FxDevice);
    }
    
    FuncExit(TRACE_FLAG_SPB);
    return status;
}

VOID
SpbDestroy(
    _In_  PDEVICE_CONTEXT  FxDeviceContext
    )
/*++
 
  Routine Description:

    This routine closes the SPB I/O target and 
    releases SPB related resources

  Arguments:

    FxDeviceContext - Pointer to the current device context 

  Return Value:

    None

--*/
{
    FuncEntry(TRACE_FLAG_SPB);

    //
    // Delete the pre-allocated SPB request 
    //
    WdfDeleteObjectSafe(FxDeviceContext->SpbRequest);

    //
    // Delete the IO Target 
    // Note:WdfObjectDelete will also close the target
    //
    WdfDeleteObjectSafe(FxDeviceContext->SpbIoTarget);

    TraceEvents(
            TRACE_LEVEL_INFORMATION,
            TRACE_FLAG_SPB,
            "WdfObjectDelete closed and deleted SpbIoTarget");

    FuncExit(TRACE_FLAG_SPB);
    return;
}

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
SpbWrite(
    _In_                    WDFIOTARGET FxIoTarget,
    _In_                    USHORT      RegisterAddress,
    _In_reads_(DataLength)  PBYTE       Data,
    _In_                    ULONG       DataLength,
    _In_                    ULONG       Timeout
    )
/*++
 
  Routine Description:

    This routine forwards a write request to the SPB I/O target

  Arguments:

    FxIoTarget        The SPB IoTarget to which to send the IOCTL/Read/Write
    RegisterAddress   The Register address to write to
    Data              The byte buffer containing the data to be written
    DataLength        The length of the byte buffer data
    Timeout           The timeout associated with this transfer
                        Default is HIDI2C_REQUEST_DEFAULT_TIMEOUT second 
                        0 means no timeout

  Return Value:

    NTSTATUS Status indicating success or failure

--*/
{
    NTSTATUS status;

    NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    WDFMEMORY memoryWrite = NULL;

    if (Data == NULL || DataLength <= 0)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "SpbWrite failed parameters Data:%p Length:%lu status:%!STATUS!",
                Data,
                DataLength,
                status);

        goto exit;
    }
    
    //
    // A SPB write-write is a single write request with the register
    // and data combined in one buffer. So we need to allocate memory 
    // for the size of a register + data length

    PBYTE pBuffer;
    ULONG bufferLength = REG_SIZE + DataLength;
    
    status = WdfMemoryCreate(
                    WDF_NO_OBJECT_ATTRIBUTES,
                    NonPagedPoolNx,
                    HIDI2C_POOL_TAG,
                    bufferLength,
                    &memoryWrite,
                    (PVOID*)&pBuffer);

    if (!NT_SUCCESS(status))
    {
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "WdfMemoryCreate failed allocating memory buffer for SpbWrite"
                "status:%!STATUS!",
                status);

        goto exit;
    }

    WDF_MEMORY_DESCRIPTOR  memoryDescriptor;
    WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(
                        &memoryDescriptor,
                        memoryWrite,
                        NULL);

    //
    // Fill in the buffer: address followed by data
    //
    RtlCopyMemory(pBuffer, &RegisterAddress, REG_SIZE);
    RtlCopyMemory((pBuffer + REG_SIZE), Data, DataLength);

    //
    // Do not set a timeout if the timeout parameter is 0
    //
    ULONG_PTR bytesWritten;

    if (Timeout == 0) 
    {
        //
        // Send the request synchronously without a timeout
        //
        status = WdfIoTargetSendWriteSynchronously(
                                FxIoTarget,
                                NULL,
                                &memoryDescriptor,
                                NULL,
                                NULL,
                                &bytesWritten);

        if (!NT_SUCCESS(status))
        {
            TraceEvents(
                    TRACE_LEVEL_ERROR,
                    TRACE_FLAG_SPB,
                    "WdfIoTargetSendWriteSynchronously failed sending SpbWrite without a timeout"
                    "bytes:%lu status:%!STATUS!",
                    (ULONG) bytesWritten,
                    status);

            goto exit;
        }
    }
    else
    {
    
        WDF_REQUEST_SEND_OPTIONS sendOptions;
        WDF_REQUEST_SEND_OPTIONS_INIT(&sendOptions, WDF_REQUEST_SEND_OPTION_TIMEOUT);
        sendOptions.Timeout = WDF_REL_TIMEOUT_IN_SEC(Timeout);

        //
        // Send the request synchronously with the timeout set
        //
        status = WdfIoTargetSendWriteSynchronously(
                                FxIoTarget,
                                NULL,
                                &memoryDescriptor,
                                NULL,
                                &sendOptions,
                                &bytesWritten);

        if (!NT_SUCCESS(status))
        {
            TraceEvents(
                    TRACE_LEVEL_ERROR,
                    TRACE_FLAG_SPB,
                    "WdfIoTargetSendWriteSynchronously failed sending SpbWrite with timeout"
                    "bytes:%lu status:%!STATUS!",
                    (ULONG) bytesWritten,
                    status);

            goto exit;
        }
    }
    //
    // Check if this is a "short transaction" i.e. the sequence
    // resulted in lesser bytes returned than expected
    // Note: For a Write, we should never expect to see a 
    // short transaction.
    //
    ULONG expectedLength = REG_SIZE + DataLength;
    if (bytesWritten != expectedLength)
    {
        status = STATUS_DEVICE_PROTOCOL_ERROR;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "WdfIoTargetSendWriteSynchronously returned with 0x%lu bytes expected:0x%lu bytes "
                "status:%!STATUS!",
                (ULONG) bytesWritten,
                expectedLength,
                status);

        goto exit;
    }

exit:
    if(!NT_SUCCESS(status))
    {
        TraceLoggingBusDevice(
            status, 
            WdfIoTargetGetDevice(FxIoTarget));
    }

    WdfDeleteObjectSafe(memoryWrite);
    return status;
}

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
SpbWritelessRead(
    _In_                        WDFIOTARGET FxIoTarget,
    _In_                        WDFREQUEST  FxRequest,
    _Out_writes_(DataLength)    PBYTE       Data,
    _In_                        ULONG       DataLength,
    _In_opt_                    LPGUID      ActivityId
    )
/*++
 
  Routine Description:

    This routine forwards a writeless (fast) read request to the SPB I/O target
    It is assumed that the address to read from is the last read address or 
    the 'default' read address

  Arguments:

    FxIoTarget        The SPB IoTarget to which to send the IOCTL/Read/Write
    FxRequest         The SPB Request used to send down to the SPB controller
    Data              The output byte buffer containing the data read
    DataLength        The length of the byte buffer data

  Return Value:

    NTSTATUS Status indicating success or failure

--*/
{
    NTSTATUS status;

    NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    if (Data == NULL || DataLength <= 0)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "SpbWritelessRead failed parameters Data:%p Length:%lu status:%!STATUS!",
                Data,
                DataLength,
                status);

        goto exit;
    }

    PREQUEST_CONTEXT pRequestContext = GetRequestContext(FxRequest);
    WDFMEMORY inputReportMemory = pRequestContext->FxMemory;

    status = WdfMemoryAssignBuffer(
                        inputReportMemory,
                        Data,
                        DataLength);

    if (!NT_SUCCESS(status))
    {
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "WdfMemoryAssignBuffer failed assigning input report buffer "
                "status:%!STATUS!",
                status);

        goto exit;
    }
 
    WDF_MEMORY_DESCRIPTOR memoryDescriptor;
    WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(
                        &memoryDescriptor,
                        inputReportMemory,
                        NULL);

    //
    // Re-use the existing SPB Request and set a timeout value
    //
    
    WDF_REQUEST_REUSE_PARAMS    reuseParams;
    WDF_REQUEST_REUSE_PARAMS_INIT(&reuseParams, WDF_REQUEST_REUSE_NO_FLAGS, STATUS_NOT_SUPPORTED);
    WdfRequestReuse(FxRequest, &reuseParams);

    WDF_REQUEST_SEND_OPTIONS sendOptions;
    WDF_REQUEST_SEND_OPTIONS_INIT(&sendOptions, WDF_REQUEST_SEND_OPTION_TIMEOUT);
    sendOptions.Timeout = WDF_REL_TIMEOUT_IN_SEC(HIDI2C_REQUEST_DEFAULT_TIMEOUT);

    //
    // Set the activityId on the SPB request that is being sent down 
    // and log the ETW event
    //
    IoSetActivityIdIrp(WdfRequestWdmGetIrp(FxRequest), ActivityId);
    EventWrite_HIDI2C_IO_SPBREAD_DISPATCH(ActivityId,
                                          FxIoTarget,
                                          FxRequest);
    
    //
    // Send the SPB read
    //
    ULONG_PTR bytes = 0;
    status = WdfIoTargetSendReadSynchronously(
                            FxIoTarget,
                            FxRequest,
                            &memoryDescriptor,
                            NULL,
                            &sendOptions,
                            &bytes);
    
    if (!NT_SUCCESS(status))
    {
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "Failed sending SPB Read bytes:%lu status:%!STATUS!",
                (ULONG)bytes,
                status);

        goto exit;
    }

    //
    // Log the ETW read complete event
    //
    EventWrite_HIDI2C_IO_SPBREAD_COMPLETE(ActivityId,
                                          FxIoTarget,
                                          FxRequest);
    
exit:
    if(!NT_SUCCESS(status))
    {
        TraceLoggingBusDevice(
            status, 
            WdfIoTargetGetDevice(FxIoTarget));
    }

    return status;
}


_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
SpbRead(
    _In_                        WDFIOTARGET FxIoTarget,
    _In_                        USHORT      RegisterAddress,
    _Out_writes_(DataLength)    PBYTE       Data,
    _In_                        ULONG       DataLength,
    _In_                        ULONG       DelayUs,
    _In_                        ULONG       Timeout
    )
/*++
 
  Routine Description:

    This routine forwards a read request to the SPB I/O target

  Arguments:

    FxIoTarget        The SPB IoTarget to which to send the IOCTL/Read/Write
    RegisterAddress   The Register Address to read from
    Data              The output byte buffer containing the data read
    DataLength        The length of the byte buffer data
    DelayUs           The delay between calls
    Timeout           The timeout associated with this transfer
                        Default is HIDI2C_REQUEST_DEFAULT_TIMEOUT second
                        0 means no timeout

  Return Value:

    NTSTATUS Status indicating success or failure

--*/
{
    NTSTATUS status;

    NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    if (Data == NULL || DataLength <= 0)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "SpbRead failed parameters Data:%p Length:%lu status:%!STATUS!",
                Data,
                DataLength,
                status);

        goto exit;
    }

    //
    // Build the SPB sequence
    //
    const ULONG transfers = 2;
    SPB_TRANSFER_LIST_AND_ENTRIES(transfers)    sequence;
    SPB_TRANSFER_LIST_INIT(&(sequence.List), transfers);

    {
        //
        // PreFAST cannot figure out the SPB_TRANSFER_LIST_ENTRY
        // "struct hack" size but using an index variable quiets 
        // the warning. This is a false positive from OACR.
        // 

        ULONG index = 0;
        sequence.List.Transfers[index] = SPB_TRANSFER_LIST_ENTRY_INIT_SIMPLE(
                                                SpbTransferDirectionToDevice,
                                                0,
                                                &RegisterAddress,
                                                REG_SIZE);

        sequence.List.Transfers[index + 1] = SPB_TRANSFER_LIST_ENTRY_INIT_SIMPLE(
                                                SpbTransferDirectionFromDevice,
                                                DelayUs,
                                                Data,
                                                DataLength);
    }

    //
    // Send the read as a Sequence request to the SPB target
    //
    ULONG bytesReturned = 0;
    status = ::_SpbSequence(FxIoTarget, &sequence, sizeof(sequence), &bytesReturned, Timeout);

    if (!NT_SUCCESS(status)) 
    {
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "SpbSequence failed sending a sequence "
                "status:%!STATUS!",
                status);

        goto exit;
    }

    //
    // Check if this is a "short transaction" i.e. the sequence
    // resulted in lesser bytes read than expected
    // Note: Since we can have multiple TLCs with variable report sizes
    // and we read inputReportMaxLength, the best we can do is 
    // ensure it is a non-zero report i.e. atleast has the 2 bytes for report length

    ULONG expectedLength = REG_SIZE;
    if (bytesReturned < expectedLength)
    {
        status = STATUS_DEVICE_PROTOCOL_ERROR;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "SpbSequence returned with 0x%lu bytes expected:0x%lu bytes "
                "status:%!STATUS!",
                bytesReturned,
                expectedLength,
                status);

        goto exit;
    }
    
exit:
    if(!NT_SUCCESS(status))
    {
        TraceLoggingBusDevice(
            status, 
            WdfIoTargetGetDevice(FxIoTarget));
    }
    return status;
}

_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
SpbWriteWrite(
    _In_                            WDFIOTARGET FxIoTarget,
    _In_                            USHORT      RegisterAddressFirst,
    _In_reads_(DataLengthFirst)     PBYTE       DataFirst,
    _In_                            USHORT      DataLengthFirst,
    _In_                            USHORT      RegisterAddressSecond,
    _In_reads_(DataLengthSecond)    PBYTE       DataSecond,
    _In_                            USHORT      DataLengthSecond,
    _In_                            ULONG       /* DelayUs */
    )
/*++
 
  Routine Description:

    This routine forwards a write-write sequence to the SPB I/O target

  Arguments:

    FxIoTarget      -       The SPB IoTarget to which to send the IOCTL/Read/Write
    RegisterAddressFirst    The First Register address to write to
    DataFirst               The first byte buffer containing the data
    DataLengthFirst         The length of the first byte buffer data
    RegisterAddressSecond   The Second Register address to write to
    DataSecond              The second byte buffer containing the data
    DataLengthSecond        The length of the second byte buffer data
    DelayUs                 The delay between calls

  Return Value:

    NTSTATUS Status indicating success or failure

--*/
{
    NTSTATUS status;

    NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    if (DataFirst == NULL  || DataLengthFirst <= 0 ||
        DataSecond == NULL || DataLengthSecond <= 0)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "SpbWriteWrite failed parameters DataFirst:%p DataLengthFirst:%lu "
                "DataSecond:%p DataLengthSecond:%lu status:%!STATUS!",
                DataFirst,
                DataLengthFirst,
                DataSecond,
                DataLengthSecond,
                status);

        goto exit;
    }

    //
    // Compact the adjacent writes of the two register accesses into a 
    // single write transfer list entry without restarts between them.
    //
    const ULONG bufferListEntries = 4;
    SPB_TRANSFER_BUFFER_LIST_ENTRY BufferListFirst[bufferListEntries];
    BufferListFirst[0].Buffer   =   &RegisterAddressFirst;
    BufferListFirst[0].BufferCb =   REG_SIZE;
    BufferListFirst[1].Buffer   =   DataFirst;
    BufferListFirst[1].BufferCb =   DataLengthFirst;
    BufferListFirst[2].Buffer   =   &RegisterAddressSecond;
    BufferListFirst[2].BufferCb =   REG_SIZE;
    BufferListFirst[3].Buffer   =   DataSecond;
    BufferListFirst[3].BufferCb =   DataLengthSecond;

    //
    // Build the SPB sequence
    //
    const ULONG transfers = 1;
    SPB_TRANSFER_LIST sequence;
    SPB_TRANSFER_LIST_INIT(&sequence, transfers);
    
    {
        sequence.Transfers[0] = SPB_TRANSFER_LIST_ENTRY_INIT_BUFFER_LIST(
                                    SpbTransferDirectionToDevice,   // Transfer Direction
                                    0,                              // Delay (1st write has no delay)
                                    BufferListFirst,                // Pointer to buffer
                                    bufferListEntries);             // Length of buffer
    }

    //
    // Send the read as a Sequence request to the SPB target
    // 
    ULONG bytesReturned = 0;
    status = ::_SpbSequence(FxIoTarget, &sequence, sizeof(sequence), &bytesReturned);

    if (!NT_SUCCESS(status)) 
    {
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "SpbSequence failed sending a sequence "
                "status:%!STATUS!",
                status);

        goto exit;
    }

    //
    // Check if this is a "short transaction" i.e. the sequence
    // resulted in lesser bytes returned than expected
    // Note: For a Write, we should never expect to see a 
    // short transaction.
    //
    ULONG expectedLength = REG_SIZE + DataLengthFirst + REG_SIZE + DataLengthSecond;
    if (bytesReturned != expectedLength)
    {
        status = STATUS_DEVICE_PROTOCOL_ERROR;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "SpbSequence returned with 0x%lu bytes expected:0x%lu bytes "
                "status:%!STATUS!",
                bytesReturned,
                expectedLength,
                status);

        goto exit;
    }

exit:
    if(!NT_SUCCESS(status))
    {
        TraceLoggingBusDevice(
            status, 
            WdfIoTargetGetDevice(FxIoTarget));
    }
    return status;
}


_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
SpbWriteRead(
    _In_                            WDFIOTARGET     FxIoTarget,
    _In_                            USHORT          RegisterAddressFirst,
    _In_reads_(DataLengthFirst)     PBYTE           DataFirst,
    _In_                            USHORT          DataLengthFirst,
    _In_                            USHORT          RegisterAddressSecond,
    _Out_writes_(DataLengthSecond)  PBYTE           DataSecond,
    _In_                            USHORT          DataLengthSecond,
    _In_                            ULONG           DelayUs
    )
/*++
 
  Routine Description:

    This routine forwards a write-read sequence to the SPB I/O target

  Arguments:

    FxIoTarget      -       The SPB IoTarget to which to send the IOCTL/Read/Write
    RegisterAddressFirst    The First Register address to write to
    DataFirst               The first byte buffer containing the data
    DataLengthFirst         The length of the first byte buffer data
    RegisterAddressSecond   The Second Register address to write to
    DataSecond              The second byte buffer containing the data
    DataLengthSecond        The length of the second byte buffer data
    DelayUs                 The delay between calls

  Return Value:

    NTSTATUS Status indicating success or failure

--*/
{
    NTSTATUS status;

    NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    if (DataFirst == NULL  || DataLengthFirst <= 0 || 
        DataSecond == NULL || DataLengthSecond <= 0)
    {
        status = STATUS_INVALID_PARAMETER;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "SpbWriteRead failed parameters DataFirst:%p DataLengthFirst:%lu "
                "DataSecond:%p DataLengthSecond:%lu status:%!STATUS!",
                DataFirst,
                DataLengthFirst,
                DataSecond,
                DataLengthSecond,
                status);

        goto exit;
    }
    
    //
    // Compact the adjacent writes of the two register accesses into a 
    // single write transfer list entry without restarts between them.
    //
    const ULONG bufferListEntries = 3;
    SPB_TRANSFER_BUFFER_LIST_ENTRY BufferListFirst[bufferListEntries];
    BufferListFirst[0].Buffer   =   &RegisterAddressFirst;
    BufferListFirst[0].BufferCb =   REG_SIZE;
    BufferListFirst[1].Buffer   =   DataFirst;
    BufferListFirst[1].BufferCb =   DataLengthFirst;
    BufferListFirst[2].Buffer   =   &RegisterAddressSecond;
    BufferListFirst[2].BufferCb =   REG_SIZE;
    
    //
    // Build the SPB sequence
    //
    const ULONG transfers = 2;
    SPB_TRANSFER_LIST_AND_ENTRIES(transfers)    sequence;
    SPB_TRANSFER_LIST_INIT(&(sequence.List), transfers);
    
    {
        //
        // PreFAST cannot figure out the SPB_TRANSFER_LIST_ENTRY
        // "struct hack" size but using an index variable quiets 
        // the warning. This is a false positive from OACR.
        // 

        ULONG index = 0;

        sequence.List.Transfers[index] = SPB_TRANSFER_LIST_ENTRY_INIT_BUFFER_LIST(
                                                SpbTransferDirectionToDevice,
                                                0,
                                                BufferListFirst,
                                                bufferListEntries);

        sequence.List.Transfers[index + 1] = SPB_TRANSFER_LIST_ENTRY_INIT_SIMPLE(
                                                SpbTransferDirectionFromDevice,
                                                DelayUs,
                                                DataSecond,
                                                DataLengthSecond);
    }

    //
    // Send the read as a Sequence request to the SPB target
    // 
    ULONG bytesReturned = 0;
    status = ::_SpbSequence(FxIoTarget, &sequence, sizeof(sequence), &bytesReturned);

    if (!NT_SUCCESS(status))
    {
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "SpbSequence failed sending a sequence "
                "status:%!STATUS!",
                status);

        goto exit;
    }

    //
    // Check if this is a "short transaction" i.e. the sequence
    // resulted in lesser bytes transmitted/received than expected
    //
    ULONG expectedLength = REG_SIZE + DataLengthFirst + REG_SIZE;
    if (bytesReturned < expectedLength)
    {
        status = STATUS_DEVICE_PROTOCOL_ERROR;
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "SpbSequence returned with 0x%lu bytes expected:0x%lu bytes "
                "status:%!STATUS!",
                bytesReturned,
                expectedLength,
                status);

        goto exit;
    }

exit:
    if(!NT_SUCCESS(status))
    {
        TraceLoggingBusDevice(
            status, 
            WdfIoTargetGetDevice(FxIoTarget));
    }

    return status;
}


_IRQL_requires_max_(PASSIVE_LEVEL)
NTSTATUS
_SpbSequence(
    _In_                        WDFIOTARGET FxIoTarget,
    _In_reads_(SequenceLength)  PVOID       Sequence,
    _In_                        SIZE_T      SequenceLength,
    _Out_                       PULONG      BytesReturned,
    _In_                        ULONG       Timeout
    )
/*++
 
  Routine Description:

    This routine forwards a sequence request to the SPB I/O target

  Arguments:

    FxIoTarget      - The SPB IoTarget to which to send the IOCTL/Read/Write
    Sequence        - Pointer to a list of sequence transfers
    SequenceLength  - Length of sequence transfers
    BytesReturned   - The number of bytes transferred in the actual transaction
    Timeout         - The timeout associated with this transfer
                        Default is HIDI2C_REQUEST_DEFAULT_TIMEOUT second
                        0 means no timeout

  Return Value:

    NTSTATUS Status indicating success or failure

--*/
{
    NTSTATUS status;

    NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    //
    // Create preallocated WDFMEMORY.
    //    
    WDFMEMORY memorySequence = NULL;

    WDF_OBJECT_ATTRIBUTES attributes;
    WDF_OBJECT_ATTRIBUTES_INIT(&attributes);
        
    status = WdfMemoryCreatePreallocated(
                            &attributes,
                            Sequence,
                            SequenceLength,
                            &memorySequence);

    if (!NT_SUCCESS(status))
    {
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "WdfMemoryCreatePreallocated failed creating memory for sequence"
                "status:%!STATUS!",
                status);

        goto exit;
    }

    WDF_MEMORY_DESCRIPTOR memoryDescriptor;
    WDF_MEMORY_DESCRIPTOR_INIT_HANDLE(
                        &memoryDescriptor,
                        memorySequence,
                        NULL);
   
    ULONG_PTR bytes = 0;

    if (Timeout == 0) 
    {
        //
        // Send the SPB sequence IOCTL without a timeout set
        //
        
        status = WdfIoTargetSendIoctlSynchronously(
                                FxIoTarget,
                                NULL,
                                IOCTL_SPB_EXECUTE_SEQUENCE,
                                &memoryDescriptor,
                                NULL,
                                NULL,
                                &bytes);
    } 
    else 
    {
        //
        // Set a request timeout
        //
        WDF_REQUEST_SEND_OPTIONS sendOptions;
        WDF_REQUEST_SEND_OPTIONS_INIT(&sendOptions, WDF_REQUEST_SEND_OPTION_TIMEOUT);
        sendOptions.Timeout = WDF_REL_TIMEOUT_IN_SEC(Timeout);
        
        //
        // Send the SPB sequence IOCTL.
        //
        
        status = WdfIoTargetSendIoctlSynchronously(
                                FxIoTarget,
                                NULL,
                                IOCTL_SPB_EXECUTE_SEQUENCE,
                                &memoryDescriptor,
                                NULL,
                                &sendOptions,
                                &bytes);
    }

    if (!NT_SUCCESS(status)) 
    {
        TraceEvents(
                TRACE_LEVEL_ERROR,
                TRACE_FLAG_SPB,
                "Failed sending SPB Sequence IOCTL bytes:%lu status:%!STATUS!",
                (ULONG)bytes,
                status);

        goto exit;
    }

    // 
    // Copy the number of bytes transmitted over the bus
    // The controller needs to support querying for actual bytes 
    // for each transaction
    //
    
    *BytesReturned = (ULONG)bytes;

exit:
    if(!NT_SUCCESS(status))
    {
        TraceLoggingBusDevice(
            status, 
            WdfIoTargetGetDevice(FxIoTarget));
    }
    WdfDeleteObjectSafe(memorySequence);
    return status;
}
