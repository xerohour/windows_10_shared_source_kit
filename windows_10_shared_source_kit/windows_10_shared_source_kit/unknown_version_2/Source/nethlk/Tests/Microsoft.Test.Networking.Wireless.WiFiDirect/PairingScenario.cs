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