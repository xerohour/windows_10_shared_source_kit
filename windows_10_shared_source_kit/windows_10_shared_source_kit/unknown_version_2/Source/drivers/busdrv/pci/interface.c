 Description:

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
///---------------------------------------------------------------------------------------------------------------------
/// <copyright company="Microsoft">
///     Copyright (C) Microsoft. All rights reserved.
/// </copyright>
///---------------------------------------------------------------------------------------------------------------------
using System;
using System.Diagnostics;

using Microsoft.Test.Networking.Wireless;

namespace Microsoft.Test.Networking.Wireless.WiFiDirect
{
    internal enum PairingScenarioType
    {
        Invitation,
        GoNegotiationDutBecomesGo,
        GoNegotiationDutBecomesClient,
        JoinExistingGo
    }

    internal class PairingScenarioResult
    {
        public PairingScenarioResult(bool scenarioSucceeded)
        {
            ScenarioSucceeded = scenarioSucceeded;
        }

        public bool ScenarioSucceeded {get; private set;}
    }

    internal class PairingScenario
    {
        public PairingScenario(
            WiFiDirectTestController localWFDController,
            WiFiDirectTestController remoteWFDController,
            PairingScenarioType pairingScenarioType,
            DOT11_WPS_CONFIG_METHOD configMethod,
            bool isPersistent,
            bool runDataPathValidation
            )
        {
            this.succeeded = false;
            this.localWFDController = localWFDController;
            this.remoteWFDController = remoteWFDController;
            this.pairingScenarioType = pairingScenarioType;

            if (configMethod != DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_DISPLAY &&
                configMethod != DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_PUSHBUTTON &&
                configMethod != DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_KEYPAD)
            {
                throw new ArgumentException("Invalid value for configMethod.  Expect pin display, pushbutton, or keypad.", "configMethod");
            }
            this.configMethod = configMethod;
            this.isPersistent = isPersistent;
            this.runDataPathValidation = runDataPathValidation;
        }

        public PairingScenarioResult Execute()
        {
            ExecuteInternal();

            return new PairingScenarioResult(succeeded);
        }

        private bool succeeded;
        private WiFiDirectTestController localWFDController;
        private WiFiDirectTestController remoteWFDController;
        private PairingScenarioType pairingScenarioType;
        private DOT11_WPS_CONFIG_METHOD configMethod;
        private bool isPersistent;
        private bool runDataPathValidation;

        private const uint pairingScenarioDiscoveryTimeoutMs = 10000;

        private void ExecuteInternal()
        {
            try
            {
                // For non-pushbutton pairing, generate a new passkey and make sure both test controllers have it.
                if (configMethod != DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_PUSHBUTTON)
                {
                    remoteWFDController.PassKey = localWFDController.GenerateAndStoreNewPassKey();
                }

                // Prepare the remote device to receive the connection request.
                remoteWFDController.AcceptNextGroupRequest(
                    localWFDController.DeviceAddress,
                    (pairingScenarioType == PairingScenarioType.GoNegotiationDutBecomesGo) ? ((byte) 0) : ((byte) 14),
                    GetAcceptConfigMethod()
                    );

                // Begin by performing a targeted discovery for the remote device.
                bool discoverySucceeded = PerformTargetedDiscovery();
                if (!discoverySucceeded)
                {
                    return;
                }

                WFD_PAIR_WITH_DEVICE_PREFERENCE pairWithDevicePreference;

                switch(this.pairingScenarioType)
                {
                    case PairingScenarioType.JoinExistingGo:
                        pairWithDevicePreference = WFD_PAIR_WITH_DEVICE_PREFERENCE.WFD_PAIRING_PREFER_NONE;
                        break;

                    case PairingScenarioType.Invitation:
                        pairWithDevicePreference = WFD_PAIR_WITH_DEVICE_PREFERENCE.WFD_PAIRING_PREFER_INVITATION;
                        break;

                    case PairingScenarioType.GoNegotiationDutBecomesGo:
                    case PairingScenarioType.GoNegotiationDutBecomesClient:
                        pairWithDevicePreference = WFD_PAIR_WITH_DEVICE_PREFERENCE.WFD_PAIRING_PREFER_GO_NEGOTIATION;
                        break;

                    default:
                        throw new Exception("Cannot map pairing scenario to pairing preference.");
                }

                Stopwatch stopwatch = new Stopwatch();
                stopwatch.Start();

                // Pair with the target device.
                WiFiDirectTestLogger.Log("Starting pairing with device {0} ({1})", remoteWFDController.DeviceAddress, remoteWFDController.MachineName);
                localWFDController.PairWithDevice(
                    remoteWFDController.DeviceAddress,
                    pairWithDevicePreference,
                    (pairingScenarioType == PairingScenarioType.GoNegotiationDutBecomesClient) ? ((byte) 0) : ((byte) 14),
                    configMethod,
                    isPersistent
                    );
                WiFiDirectTestLogger.Log("Pairing successful", remoteWFDController.DeviceAddress, remoteWFDController.MachineName);

                stopwatch.Stop();
                WiFiDirectTestLogger.Log("Pairing completed in {0} ms.", stopwatch.ElapsedMilliseconds);

                bool sessionStateValid = VerifySessionState();
                if (!sessionStateValid)
                {
                    return;
                }

                if (this.runDataPathValidation)
                {
                    WiFiDirectTestUtilities.RunDataPathValidation(localWFDController, remoteWFDController);
                }

                succeeded = true;
            }
            catch(Exception e)
            {
                WiFiDirectTestLogger.Error("Caught exception while executing pairing scenario: {0}", e);
            }
        }

        private DOT11_WPS_CONFIG_METHOD GetAcceptConfigMethod()
        {
            DOT11_WPS_CONFIG_METHOD acceptConfigMethod = DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_PUSHBUTTON;
            switch(configMethod)
            {
                case DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_DISPLAY:
                    acceptConfigMethod = DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_KEYPAD;
                    break;
                case DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_KEYPAD:
                    acceptConfigMethod = DOT11_WPS_CONFIG_METHOD.DOT11_WPS_CONFIG_METHOD_DISPLAY;
                    break;
            }
            return acceptConfigMethod;
        }

        private bool PerformTargetedDiscovery()
        {
            DiscoveryScenarioType discoveryScenarioType;

            switch(pairingScenarioType)
            {
                case PairingScenarioType.JoinExistingGo:
                    discoveryScenarioType = DiscoveryScenarioType.DiscoverAsGo;
                    break;

                case PairingScenarioType.Invitation:
                case PairingScenarioType.GoNegotiationDutBecomesGo:
                case PairingScenarioType.GoNegotiationDutBecomesClient:
                    discoveryScenarioType = DiscoveryScenarioType.DiscoverAsDevice;
                    break;

                default:
                    throw new Exception("Cannot map pairing scenario to discovery scenario.");
            }

            DiscoveryScenario discoveryScenario = new DiscoveryScenario(
                localWFDController,
                remoteWFDController,
                discoveryScenarioType,
                true, // isTargettedDiscovery
                WFD_DISCOVER_TYPE.wfd_discover_type_auto,
                pairingScenarioDiscoveryTimeoutMs,
                false // discoverTestIe
                );
            DiscoveryScenarioResult discoveryScenarioResult = discoveryScenario.Execute();

            if (!discoveryScenarioResult.ScenarioSucceeded)
            {
                WiFiDirectTestLogger.Error("Targetted discovery failed for device {0} ({1}).  Aborting pairing operation.", remoteWFDController.DeviceAddress, remoteWFDController.MachineName);
            }

            return discoveryScenarioResult.ScenarioSucceeded;
        }

        private bool VerifySessionState()
        {
            WfdGlobalSessionState globalSessionState = localWFDController.QueryGlobalSessionState();

            if ( globalSessionState.Sessions.Length != 1 )
            {
                WiFiDirectTestLogger.Error("Expected a single active session.  Current session count = {0}", globalSessionState.Sessions.Length );
                return false;
            }

            WFD_ROLE_TYPE expectedRole;
            switch(pairingScenarioType)
            {
                case PairingScenarioType.Invitation:
                case PairingScenarioType.GoNegotiationDutBecomesGo:
                    expectedRole = WFD_ROLE_TYPE.WFD_ROLE_TYPE_GROUP_OWNER;
                    break;

                case PairingScenarioType.GoNegotiationDutBecomesClient:
                case PairingScenarioType.JoinExistingGo:
                    expectedRole = WFD_ROLE_TYPE.WFD_ROLE_TYPE_CLIENT;
                    break;

                default:
                    throw new Exception("Unable to map pairing scenario type to role type.");
            }

            if ( globalSessionState.Sessions[0].Role != expectedRole )
            {
                WiFiDirectTestLogger.Error("DUT became the WFD {0}.  Expected {1}", MapRoleToString(globalSessionState.Sessions[0].Role), MapRoleToString(expectedRole));
                return false;
            }

            return true;
        }

        private string MapRoleToString(WFD_ROLE_TYPE roleType)
        {
            return (roleType == WFD_ROLE_TYPE.WFD_ROLE_TYPE_GROUP_OWNER) ? "GO" : "Client";
        }
    }
}
//
//    Copyright (C) Microsoft.  All rights reserved.
//
#include <windows.h>

#include <ntverp.h>

#define VER_FILETYPE                VFT_DRV
#define VER_FILESUBTYPE             VFT2_DRV_SOUND
#define VER_FILEDESCRIPTION_STR     "Port Class (Class Driver for Port/Miniport Devices)"
#define VER_INTERNALNAME_STR        "portcls.sys"
#define VER_ORIGINALFILENAME_STR    "portcls.sys"

#include "common.ver"

#include "portclsEvents.rc"

PortclsMof MOFDATA portcls.bmf
/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    interface.c

Abstract:

    IRP_MN_QUERY_INTERFACE lives here.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "interface.tmh"

// --------------------------------------------------------------------- Defines

#define PciCompareGuid(a,b)                                         \
    (RtlEqualMemory((PVOID)(a), (PVOID)(b), sizeof(GUID)))

#define LOCATION_INTERFACE_VERSION 1

//
// The length - in characters - of the Multi-Sz strings returned from the
// location interface. count one extra character for the MultiSz second terminator
//
#define LOCATION_INTERFACE_DEVICE_STRING_COUNT (sizeof "PCI(XXXX)" + 1)
#define LOCATION_INTERFACE_ROOT_STRING_COUNT (sizeof "PCIROOT(XXXXXX)" + 1)

#define DEVICE_PRESENT_INTERFACE_MINSIZE     FIELD_OFFSET(PCI_DEVICE_PRESENT_INTERFACE, IsDevicePresentEx)

#define PCI_TRANSLATOR_INTERFACE_VERSION 0
#define TR_IRQ_INVALID_VECTOR 0xffffffff

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

PCI_INTERFACE_CONSTRUCTOR PciLocationInterface_Constructor;

VOID
PciLocationInterface_Reference(
    __inout PVOID Context
    );

VOID
PciLocationInterface_Dereference(
    __inout PVOID Context
    );

NTSTATUS
PciLocationInterface_GetLocationStrings(
    __inout PVOID Context,
    __out PWCHAR *LocationStrings
    );

PCI_INTERFACE_CONSTRUCTOR PciDevicePresentInterface_Constructor;

VOID
PciDevicePresentInterface_Reference(
    __inout PVOID Context
    );

VOID
PciDevicePresentInterface_Dereference(
    __inout PVOID Context
    );

PCI_IS_DEVICE_PRESENT PciDevicePresentInterface_DevicePresent;
PCI_IS_DEVICE_PRESENT_EX PciDevicePresentInterface_DevicePresentEx;

BOOLEAN
PciCheckDevicePresence(
    __in_opt PPCI_DEVICE Device,
    __in PPCI_DEVICE_PRESENCE_PARAMETERS Parameters
    );

BOOLEAN
PciDevicePresentOnBus(
    __inout PPCI_BUS Bus,
    __in_opt PPCI_DEVICE Device,
    __in PPCI_DEVICE_PRESENCE_PARAMETERS Parameters
    );

PCI_INTERFACE_CONSTRUCTOR PciCardbusInterface_Constructor;

VOID
PciCardbusInterface_Reference(
    __inout PVOID Context
    );

VOID
PciCardbusInterface_Dereference(
    __inout PVOID Context
    );

NTSTATUS
PciCardbusInterface_GetLocation(
    __in PDEVICE_OBJECT Pdo,
    __out PULONG Bus,
    __out PUCHAR DeviceNumber,
    __out PUCHAR FunctionNumber,
    __out BOOLEAN *OnDebugPath
    );

PCI_INTERFACE_CONSTRUCTOR PciTranslatorInterface_Constructor;

//
// Routines describing crashdump functions interface.
//

PCI_INTERFACE_CONSTRUCTOR PciCrashdumpFunctions_Constructor;

VOID
PciCrashdumpFunctions_Reference(
    __inout PVOID Context
    );

VOID
PciCrashdumpFunctions_Dereference(
    __inout PVOID Context
    );

NTSTATUS
PciCrashdumpFunctions_PowerOn (
    _In_opt_ PVOID Context
    );

_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR PciReenumerateSelfInterface_Constructor;

_IRQL_requires_(PASSIVE_LEVEL)
PCI_INTERFACE_CONSTRUCTOR PciDeviceResetInterface_Constructor;

PCI_INTERFACE_CONSTRUCTOR PciSecurityInterface_Constructor;
PCI_SET_ACS PciSecurityInterface_SetAcs;

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciQueryInterface)
    #pragma alloc_text(PAGE, PciRefDereferenceNoop)
    #pragma alloc_text(PAGE, PciLocationInterface_Constructor)
    #pragma alloc_text(PAGE, PciLocationInterface_GetLocationStrings)
    #pragma alloc_text(PAGE, PciDevicePresentInterface_Constructor)
    #pragma alloc_text(PAGE, PciDevicePresentInterface_DevicePresent)
    #pragma alloc_text(PAGE, PciDevicePresentInterface_DevicePresentEx)
    #pragma alloc_text(PAGE, PciCheckDevicePresence)
    #pragma alloc_text(PAGE, PciDevicePresentOnBus)
    #pragma alloc_text(PAGE, PciCardbusInterface_Constructor)
    #pragma alloc_text(PAGE, PciCardbusInterface_GetLocation)
    #pragma alloc_text(PAGE, PciTranslatorInterface_Constructor)
    #pragma alloc_text(PAGE, PciCrashdumpFunctions_Constructor)
    #pragma alloc_text(PAGE, PciReenumerateSelfInterface_Constructor)
    #pragma alloc_text(PAGE, PciDeviceResetInterface_Constructor)
    #pragma alloc_text(PAGE, PciSecurityInterface_Constructor)
    #pragma alloc_text(PAGE, PciSecurityInterface_SetAcs)
#endif

// --------------------------------------------------------------------- Globals

extern const PCI_INTERFACE PciArbiterInterface;
extern const PCI_INTERFACE PciBusHandlerInterface;
extern const PCI_INTERFACE PciAgpTargetInterface;
extern const PCI_INTERFACE PciRoutingInterface;
extern const PCI_INTERFACE PciLegacyDeviceDetectionInterface;
extern const PCI_INTERFACE PciPmeInterface;
extern const PCI_INTERFACE PciDevicePresentInterface;
extern const PCI_INTERFACE PciNativeIdeInterface;
extern const PCI_INTERFACE PciBridgeInterface;
extern const PCI_INTERFACE ExpressLinkQuiescentInterface;
extern const PCI_INTERFACE ExpressRootPortInterface;
extern const PCI_INTERFACE PciMsiXTableConfigInterface;
extern const PCI_INTERFACE PciVirtualizationInterface;
extern const PCI_INTERFACE PciD3ColdSupportInterface;
extern const PCI_INTERFACE PciIommuBusInterfaceInterface;

const PCI_INTERFACE PciLocationInterface = {
    &GUID_PNP_LOCATION_INTERFACE,           // InterfaceType
    sizeof(PNP_LOCATION_INTERFACE),         // MinSize
    LOCATION_INTERFACE_VERSION,             // MinVersion
    LOCATION_INTERFACE_VERSION,             // MaxVersion
    PCIIF_PDO | PCIIF_FDO | PCIIF_ROOT,     // Flags - supported on PDOs and root FDOs
    PciInterface_Location,                  // Signature
    PciLocationInterface_Constructor        // Constructor
};

const PCI_INTERFACE PciDevicePresentInterface = {
    &GUID_PCI_DEVICE_PRESENT_INTERFACE,     // InterfaceType
    DEVICE_PRESENT_INTERFACE_MINSIZE,       // MinSize
    PCI_DEVICE_PRESENT_INTERFACE_VERSION,   // MinVersion
    PCI_DEVICE_PRESENT_INTERFACE_VERSION,   // MaxVersion
    PCIIF_PDO,                              // Flags
    PciInterface_DevicePresent,             // Signature
    PciDevicePresentInterface_Constructor   // Constructor
};

const PCI_INTERFACE PciCardbusPrivateInterface = {
    &GUID_PCI_CARDBUS_INTERFACE_PRIVATE,    // InterfaceType
    sizeof(PCI_CARDBUS_INTERFACE_PRIVATE),  // MinSize
    PCI_CB_INTRF_VERSION,                   // MinVersion
    PCI_CB_INTRF_VERSION,                   // MaxVersion
    PCIIF_PDO,                              // Flags
    PciInterface_PciCb,                     // Signature
    PciCardbusInterface_Constructor         // Constructor
};

const PCI_INTERFACE PciTranslatorInterface = {
    &GUID_TRANSLATOR_INTERFACE_STANDARD,    // InterfaceType
    sizeof(TRANSLATOR_INTERFACE),           // MinSize
    PCI_TRANSLATOR_INTERFACE_VERSION,       // MinVersion
    PCI_TRANSLATOR_INTERFACE_VERSION,       // MaxVersion
    PCIIF_FDO,                              // Flags
    PciInterface_Translator,                // Signature
    PciTranslatorInterface_Constructor      // Constructor
};

const PCI_INTERFACE PciCrashdumpFunctionsInterface = {
    &GUID_QUERY_CRASHDUMP_FUNCTIONS,        // InterfaceType
    sizeof(CRASHDUMP_FUNCTIONS_INTERFACE),  // MinSize
    1,                                      // MinVersion
    1,                                      // MaxVersion
    PCIIF_PDO | PCIIF_FDO | PCIIF_ROOT,     // Flags
    PciInterface_CrashdumpFunctions,        // Signature
    PciCrashdumpFunctions_Constructor       // Constructor
};

const PCI_INTERFACE PciReenumerateSelfInterface = {
    &GUID_REENUMERATE_SELF_INTERFACE_STANDARD,      // InterfaceType
    sizeof(REENUMERATE_SELF_INTERFACE_STANDARD),    // MinSize
    1,                                              // MinVersion
    1,                                              // MaxVersion
    PCIIF_PDO,                                      // Flags
    PciInterface_ReenumerateSelf,                   // Signature
    PciReenumerateSelfInterface_Constructor         // Constructor
};

const PCI_INTERFACE PciDeviceResetStandardInterface = {
    &GUID_DEVICE_RESET_INTERFACE_STANDARD,          // InterfaceType
    sizeof(DEVICE_RESET_INTERFACE_STANDARD),        // MinSize
    DEVICE_RESET_INTERFACE_VERSION,                 // MinVersion
    DEVICE_RESET_INTERFACE_VERSION,                 // MaxVersion
    PCIIF_PDO,                                      // Flags
    PciInterface_DeviceResetStandard,               // Signature
    PciDeviceResetInterface_Constructor             // Constructor
};

const PCI_INTERFACE PciSecurityInterface = {
    &GUID_PCI_SECURITY_INTERFACE,                   // InterfaceType
    sizeof(PCI_SECURITY_INTERFACE),                 // MinSize
    PCI_SECURITY_INTERFACE_VERSION,                 // MinVersion
    PCI_SECURITY_INTERFACE_VERSION,                 // MaxVersion
    PCIIF_PDO,                                      // Flags
    PciInterface_SecurityInterface,                 // Signature
    PciSecurityInterface_Constructor                // Constructor
};

const PCI_INTERFACE * const PciInterfaces[] = {
    &PciArbiterInterface,
    &PciBusHandlerInterface,
    &PciAgpTargetInterface,
    &PciRoutingInterface,
    &PciCardbusPrivateInterface,
    &PciLegacyDeviceDetectionInterface,
    &PciPmeInterface,
    &PciDevicePresentInterface,
    &PciNativeIdeInterface,
    &PciLocationInterface,
    &PciBridgeInterface,
    &ExpressLinkQuiescentInterface,
    &ExpressRootPortInterface,
    &PciMsiXTableConfigInterface,
    &PciVirtualizationInterface,
    &PciD3ColdSupportInterface,
    &PciCrashdumpFunctionsInterface,
    &PciReenumerateSelfInterface,
    &PciIommuBusInterfaceInterface,
    &PciDeviceResetStandardInterface,
    &PciSecurityInterface,
    NULL
};

//
// Reference count for the PCI global crashdump functions interface.
//

volatile LONG PciCrashdumpFunctionsReferenceCount;

//
// These are the interfaces we supply only if nobody underneath
// us (the HAL) does.
//
const PCPCI_INTERFACE PciInterfacesLastResort[] = {
    &PciTranslatorInterface,
    NULL
};

// -------------------------------------------------------------- General Interface Handler

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciQueryInterface(
    __in PVOID DeviceExtension,
    __in LPCGUID InterfaceType,
    __in USHORT Size,
    __in USHORT Version,
    __in PVOID InterfaceSpecificData,
    __out_bcount(Size) PINTERFACE InterfaceReturn,
    __in BOOLEAN LastChance,
    __in NTSTATUS CurrentIrpStatus
    )
/*++

Routine Description:

    This routine searches through the PciInterfaces table looking for
    an interface entry that matches the parameters.

Arguments:

    DeviceExtension - The device extension that received the interface request.
        Some interfaces are allowable only from the PDO level, and some only
        from the FDO level.

    InterfaceType - A GUID representing the requested interface.

    Size - The size of the buffer provided by the caller to fill
        in with the interface.

    Version - The version of the interface requested.

    InterfaceSpecificData - Interface-specific data provided by the caller.

    InterfaceReturn - The buffer (of size Size) to fill in with the matching
        interface.

    LastChance - Whether to provide "last chance" interfaces.  Last chance
        interfaces should only be provided by the FDO after passing the
        request down to the PDO level and seeing that the PDO does not
        provide the interface itself.  Currently this is only used for
        interrupt translators.

    CurrentIrpStatus - This status indicates whether another driver in the stack
        has already filled in the interface.

Return Value:

    NT Status code.

--*/
{
    UNICODE_STRING guidString;
    const PCPCI_INTERFACE *interfaceEntry;
    const PCPCI_INTERFACE *interfaceTable;
    PCPCI_INTERFACE interface;
    NTSTATUS status;
    PPCI_BUS bus;

    PAGED_CODE();

    status = DebugStringFromGuid(InterfaceType, &guidString);
    if (NT_SUCCESS(status)) {

        TRACE(INTERFACE_TRACE_LEVEL,
              "PciQueryInterface TYPE = %wZ\n",
              &guidString
              );

        TRACE(INTERFACE_TRACE_LEVEL,
              "Size = %d, Version = %d, InterfaceData = %p, LastChance = %s\n",
              Size,
              Version,
              InterfaceSpecificData,
              LastChance ? "TRUE" : "FALSE"
              );

        RtlFreeUnicodeString(&guidString);
    }

    //
    // Try to locate the requested interface in the PCI driver's set
    // of exported interfaces.
    //
    // Note - we do not allow last chance interfaces (ie mock translators) for
    // machines where we assign bus numbers
    //
    if (LastChance) {

        interfaceTable = PciInterfacesLastResort;
    } else {
        interfaceTable = PciInterfaces;
    }

    for (interfaceEntry = interfaceTable; *interfaceEntry; interfaceEntry++) {

        interface = *interfaceEntry;

        //
        // Check if this interface is allowed to be used from this
        // device object type.  PCIIF_PDO means the interface is only
        // allowed at the PDO level, PCIIF_FDO means only at the FDO level.
        // PCIIF_ROOT must be used in conjunction with PCIIF_FDO, but
        // means that the interface can only be used from roots.
        //
        if (PCI_DEVICE_EXTENSION(DeviceExtension)) {

            if ((interface->Flags & PCIIF_PDO) == 0) {

                status = DebugStringFromGuid(interface->InterfaceType, &guidString);
                if (NT_SUCCESS(status)) {
                    TRACE(INTERFACE_TRACE_LEVEL,
                          "\tguid = %wZ only for PDOs\n",
                          &guidString
                          );
                    RtlFreeUnicodeString(&guidString);
                }

                continue;
            }

        } else {

            if ((interface->Flags & PCIIF_FDO) == 0) {

                status = DebugStringFromGuid(interface->InterfaceType, &guidString);
                if (NT_SUCCESS(status)) {
                    TRACE(INTERFACE_TRACE_LEVEL,
                          "\tguid = %wZ only for FDOs\n",
                          &guidString
                          );
                    RtlFreeUnicodeString(&guidString);
                }

                continue;
            }

            if (interface->Flags & PCIIF_ROOT) {

                bus = (PPCI_BUS)DeviceExtension;

                if (!PCI_ROOT_BUS(bus)) {

                    status = DebugStringFromGuid(interface->InterfaceType, &guidString);
                    if (NT_SUCCESS(status)) {
                        TRACE(INTERFACE_TRACE_LEVEL,
                              "\tguid = %wZ only for ROOT\n",
                              &guidString
                              );
                        RtlFreeUnicodeString(&guidString);
                    }

                    continue;
                }
            }
        }

        status = DebugStringFromGuid(interface->InterfaceType, &guidString);
        if (NT_SUCCESS(status)) {
            TRACE(INTERFACE_TRACE_LEVEL,
                  "\tguid = %wZ okay for device type\n",
                  &guidString
                  );
            RtlFreeUnicodeString(&guidString);
        }

        //
        // Check for the appropriate GUID then verify version numbers
        // and size.
        //
        if ((PciCompareGuid(InterfaceType, interface->InterfaceType)) &&
            (Version >= interface->MinVersion) &&
            (Version <= interface->MaxVersion) &&
            (Size >= interface->MinSize)) {

            //
            // We have a possible hit.  Check to see if the interface
            // itself agrees.
            //
            status = interface->Constructor(
                DeviceExtension,
                InterfaceSpecificData,
                Version,
                Size,
                InterfaceReturn,
                CurrentIrpStatus == STATUS_SUCCESS ? TRUE : FALSE
                );
            if (NT_SUCCESS(status)) {

                //
                // We found and allocated an interface, reference it
                // and get out of the loop.
                //
                InterfaceReturn->InterfaceReference(InterfaceReturn->Context);

                TRACE(INTERFACE_TRACE_LEVEL, "-- returning SUCCESS\n");

                return status;

            } else {

                TRACE(INTERFACE_TRACE_LEVEL, "\t\tContructor returned %08lx\n", status);
            }
        } else {

            TRACE(INTERFACE_TRACE_LEVEL, "\t\tguid doesn't match.\n");
        }
    }

    //
    // Failed to find the requested interface.
    //
    TRACE(INTERFACE_TRACE_LEVEL, "-- FAILED TO FIND INTERFACE\n");

    return STATUS_NOT_SUPPORTED;
}

VOID
PciRefDereferenceNoop(
    __in PVOID Context
    )
{
    UNREFERENCED_PARAMETER(Context);

    PAGED_CODE();
    return;
}

// -------------------------------------------------------------- Location Interface

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciLocationInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    This routine constructs a PNP_LOCATION_INTERFACE.

Arguments:

    DeviceExtension - An extension pointer.

    InterfaceSpecificData - Unused.

    Version - Interface version.

    Size - Size of the PNP_LOCATION_INTERFACE interface object.

    InterfaceReturn - The interface object pointer to be filled in.

    Filtered - Not supported for this interface.

Return Value:

    Returns NTSTATUS.

--*/
{
    PPNP_LOCATION_INTERFACE interface;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(PNP_LOCATION_INTERFACE)) {
        return STATUS_UNSUCCESSFUL;
    }

    interface = (PPNP_LOCATION_INTERFACE)InterfaceReturn;
    interface->Size = sizeof(PNP_LOCATION_INTERFACE);
    interface->Version = LOCATION_INTERFACE_VERSION;
    interface->Context = DeviceExtension;
    interface->InterfaceReference = PciLocationInterface_Reference;
    interface->InterfaceDereference = PciLocationInterface_Dereference;
    interface->GetLocationString = PciLocationInterface_GetLocationStrings;

    return STATUS_SUCCESS;
}

VOID
PciLocationInterface_Reference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine increments outstanding reference count for this interface

Arguments:

    PCI_DEVICE or PCI_BUS

Return Value:

    None

--*/
{
    PPCI_COMMON_EXTENSION extension = (PPCI_COMMON_EXTENSION)Context;

    if (PCI_DEVICE_EXTENSION(extension)) {

        PPCI_DEVICE device = (PPCI_DEVICE)extension;
        InterlockedIncrement(&device->PciLocationInterfaceCount);

    } else {

        PPCI_BUS bus = (PPCI_BUS)extension;
        InterlockedIncrement(&bus->PciLocationInterfaceCount);
    }

    return;
}

VOID
PciLocationInterface_Dereference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine decrements outstanding reference count for this interface

Arguments:

    PCI_DEVICE or PCI_BUS

Return Value:

    None

--*/
{
    PPCI_COMMON_EXTENSION extension = (PPCI_COMMON_EXTENSION)Context;

    if (PCI_DEVICE_EXTENSION(extension)) {

        PPCI_DEVICE device = (PPCI_DEVICE)extension;
        InterlockedDecrement(&device->PciLocationInterfaceCount);

    } else {

        PPCI_BUS bus = (PPCI_BUS)extension;
        InterlockedDecrement(&bus->PciLocationInterfaceCount);
    }

    return;
}

NTSTATUS
PciLocationInterface_GetLocationStrings(
    __inout PVOID Context,
    __out PWCHAR *LocationStrings
    )
/*++

Routine Description:

    This routine allocates, fills in, and returns a Multi-Sz string
    containing the bus-relative location identifier string for the
    given device.

    For a PCI device, this is "PCI(XXYY)", where XX is the device
    number of the device, and YY is the function number of the device.

    For a PCI root bus, this is PCIROOT(XX), where XX is the bus number
    of the root bus.  This relies on the fact that bus numbers of root
    buses will not change, which is believed to be a safe assumption
    for some time to come.

    This interface is permitted to return a Multi-Sz containing
    multiple strings describing the same device, but in this
    first implementation, only the single strings listed above
    will be returned from the interface.  The string must still
    be in the format of a Multi-Sz, however, meaning a double-NULL
    terminator is required.

Arguments:

    Context - extension pointer.

Return Value:

    NTSTATUS code.

--*/
{
    PPCI_COMMON_EXTENSION extension;
    PPCI_DEVICE device;
    PPCI_BUS bus;
    SIZE_T remainingChars;
    PWCHAR stringBuffer;
    NTSTATUS status;

    PAGED_CODE();

    *LocationStrings = NULL;

    extension = (PPCI_COMMON_EXTENSION)Context;

    if (PCI_DEVICE_EXTENSION(extension)) {

        device = (PPCI_DEVICE)extension;

        stringBuffer = PciAllocatePool(
                            PagedPool,
                            LOCATION_INTERFACE_DEVICE_STRING_COUNT*sizeof(WCHAR)
                            );
        if (!stringBuffer) {
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        //
        // The location string for a PCI device is "PCI(XXYY)"
        // where XX is the device number and YY is the function number
        // We use the STRSAFE_FILL_BEHIND_NULL flag to ensure the unused portion
        // of the buffer is filled with 0s which null terminates the multsz
        //
        status = RtlStringCchPrintfExW(stringBuffer,
                                       LOCATION_INTERFACE_DEVICE_STRING_COUNT,
                                       NULL,
                                       &remainingChars,
                                       STRSAFE_FILL_BEHIND_NULL,
                                       L"PCI(%.2X%.2X)",
                                       device->Slot.u.bits.DeviceNumber,
                                       device->Slot.u.bits.FunctionNumber
                                       );
        PCI_ASSERT(NT_SUCCESS(status));
        UNREFERENCED_PARAMETER(status);

        //
        // Make sure there was room for the multisz termination NUL
        // N.B. remainingChars counts the NUL termination of the regular string
        // as being available so we need to ensure 2 chars are left for the 2 NULS
        //
        PCI_ASSERT(remainingChars >= 2);

        *LocationStrings = stringBuffer;
        return STATUS_SUCCESS;

    } else {

        bus = (PPCI_BUS)extension;

        if (PCI_ROOT_BUS(bus)) {

            stringBuffer = PciAllocatePool(
                                PagedPool,
                                LOCATION_INTERFACE_ROOT_STRING_COUNT*sizeof(WCHAR)
                                );
            if (!stringBuffer) {
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            //
            // The location string for a PCI root is "PCIROOT(X)"
            // where X is the bus number of root bus.
            // We use the STRSAFE_FILL_BEHIND_NULL flag to ensure the unused portion
            // of the buffer is filled with 0s which null terminates the multsz
            //
            status = RtlStringCchPrintfExW(stringBuffer,
                                           LOCATION_INTERFACE_ROOT_STRING_COUNT,
                                           NULL,
                                           &remainingChars,
                                           STRSAFE_FILL_BEHIND_NULL,
                                           L"PCIROOT(%X)",
                                           bus->SecondaryBusNumber
                                           );
            PCI_ASSERT(NT_SUCCESS(status));
            UNREFERENCED_PARAMETER(status);

            //
            // Make sure there was room for the multisz termination NUL
            // N.B. remainingChars counts the NUL terminatiun of the regular string
            // as being available so we need to ensure 2 chars are left for the 2 NULS
            //
            PCI_ASSERT(remainingChars >= 2);

            *LocationStrings = stringBuffer;

            //
            // returning STATUS_TRANSLATION_COMPLETE indicates that PnP shouldn't
            // query for this interface any further up the tree.  Stop here.
            //
            return STATUS_TRANSLATION_COMPLETE;

        } else {

            //
            // In the interface constructor, we specified that this interface
            // is only valid for root FDOs.  If we get here, we've been asked
            // to fill in this interface for a P-P bridge FDO, which is illegal.
            //
            ERROR("PciLocationInterface_GetLocationStrings: not a root bus.\n");
            return STATUS_INVALID_PARAMETER;
        }
    }
}

// -------------------------------------------------------------- Device Present Interface

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciDevicePresentInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    This routine constructs a PCI_DEVICE_PRESENT_INTERFACE.

Arguments:

    DeviceExtension - An extension pointer.

    InterfaceSpecificData - Unused.

    Version - Interface version.

    Size - Size of the PCI_DEVICE_PRESENT_INTERFACE interface object.

    InterfaceReturn - The interface object pointer to be filled in.

    Filtered - Not supported for this interface.

Return Value:

    Returns NTSTATUS.

--*/
{
    PPCI_DEVICE_PRESENT_INTERFACE interface;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < DEVICE_PRESENT_INTERFACE_MINSIZE) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Have already verified that the InterfaceReturn variable
    // points to an area in memory large enough to contain a
    // PCI_DEVICE_PRESENT_INTERFACE.  Fill it in for the caller.
    //
    interface = (PPCI_DEVICE_PRESENT_INTERFACE)InterfaceRetur