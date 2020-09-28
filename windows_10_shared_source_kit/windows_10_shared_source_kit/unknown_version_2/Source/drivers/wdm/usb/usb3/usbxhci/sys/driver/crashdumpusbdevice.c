/*++
Copyright (c) Microsoft Corporation

Module Name:

    crashdumpusbdevice.c

Abstract:

    USB XHCI Crashdump write module.

--*/

#include "pch.h"

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Crashdump_UsbDevice_Initialize(
    __inout
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        HANDLE                      DumperHandle,
    __in_opt
        PUSB_DUMP_DEVICE_INFO       DumpDeviceInfo,
    __in
        UCHAR                       SlotId,
    __in
        PVOID                       DeviceContext,
    __in
        HANDLE                      UsbDeviceHandle,
    __in
        ULONG                       PortPathDepth
    )
{
    NTSTATUS    status;

    TRY {

        UsbDeviceData->DumperHandle = DumperHandle;
        UsbDeviceData->RegisterHandle = Crashdump_GetRegisterHandle(DumperHandle);
        UsbDeviceData->CommandHandle = Crashdump_GetCommandHandle(DumperHandle);
        UsbDeviceData->SlotId = SlotId;
        UsbDeviceData->UsbDeviceHandle = UsbDeviceHandle;
        UsbDeviceData->OutputDeviceContext = DeviceContext;
        UsbDeviceData->PortPathDepth = PortPathDepth;

        Crashdump_UsbDevice_SetOutputSlotContext(UsbDeviceData, DeviceContext);
        Crashdump_UsbDevice_SetSlotContext(UsbDeviceData, DeviceContext);

        //
        // Device can be the target storage device or a hub. Cache the information
        // needed to reconfigure the device on a reset.
        //

        //
        // For just the storage device DumpDeviceInfo would not be NULL.
        // For the hubs in the path to the storage device DumpDeviceInfo would be NULL
        //
        if (DumpDeviceInfo != NULL) {
            
            UsbDeviceData->ConfigurationId = DumpDeviceInfo->ConfigurationId;
            UsbDeviceData->InterfaceId = DumpDeviceInfo->InterfaceId;
            UsbDeviceData->AlternateSettingId = DumpDeviceInfo->AlternateSettingId;

        } else {

            UsbDeviceData->IsHub = TRUE;
            UsbDeviceData->ConfigurationId = 1;
            UsbDeviceData->InterfaceId = 0;
            UsbDeviceData->AlternateSettingId = 0;
        }

        //
        // Acquire CommonBuffers.
        //
        if (Crashdump_Register_GetContextSize(UsbDeviceData->RegisterHandle) == REGISTER_CONTEXTSIZE64) {

            status = Crashdump_CommonBufferAcquire(UsbDeviceData->DumperHandle,
                                                   sizeof(DEVICE_CONTEXT64),
                                                   &UsbDeviceData->DeviceContextBuffer);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

        } else {

            status = Crashdump_CommonBufferAcquire(UsbDeviceData->DumperHandle,
                                                   sizeof(DEVICE_CONTEXT32),
                                                   &UsbDeviceData->DeviceContextBuffer);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

    } FINALLY {

    }

    return status;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_PrepareForHibernate(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    )
/*++

Routine Description:

    This routine will ensure that the device and its endpoints are in the
    Configured state, and acquire any DMA buffers needed for dump write.

--*/
{
    NTSTATUS    status;
    ULONG       endpointDCI;
    HANDLE      crashdumpEndpoint;

    TRY {

        CrashdumpLogInfo("Crashdump_UsbDevice_PrepareForHibernate: begin");

        RtlZeroMemory(UsbDeviceData->DeviceContextBuffer.VirtualAddress,
                      UsbDeviceData->DeviceContextBuffer.Size);

        for (endpointDCI = 1; endpointDCI < MAX_DEVICE_CONTEXT_INDEX; endpointDCI++) {

            crashdumpEndpoint = Crashdump_UsbDevice_GetEndpointHandle(UsbDeviceData, endpointDCI);
            if (crashdumpEndpoint == NULL) {
                continue;
            }

            if (Crashdump_Register_GetContextSize(UsbDeviceData->RegisterHandle) == REGISTER_CONTEXTSIZE64) {
                Crashdump_BufferMarkForHibernate(UsbDeviceData->OutputDeviceContext,
                                                 sizeof(DEVICE_CONTEXT64));
            } else {
                Crashdump_BufferMarkForHibernate(UsbDeviceData->OutputDeviceContext,
                                                 sizeof(DEVICE_CONTEXT32));
            }

            Crashdump_BufferMarkForHibernate(crashdumpEndpoint,
                                             sizeof(CRASHDUMP_ENDPOINT_DATA));

            status = Crashdump_Endpoint_PrepareForHibernate(crashdumpEndpoint);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

        status = STATUS_SUCCESS;

    } FINALLY {

        CrashdumpLogInfo("Crashdump_UsbDevice_PrepareForHibernate: end 0x%X", status);
    }

    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_ReConfigureOnControllerReset(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in 
        CRASHDUMP_PORT_TYPE         PortType
    )
/*++

Routine Description:

    This routine will reconfigure the Crashdump device and its endpoints after
    a Controller Reset.

--*/
{
    NTSTATUS                status;
    ULONG                   endpointDCI;
    HANDLE                  crashdumpEndpoint;
    DUMP_POWER_DOWN_TYPE    powerDownType;

    TRY {

        CrashdumpLogInfo("Crashdump_UsbDevice_ReConfigureOnControllerReset: begin");

        //
        // This function will update the SlotId field. After that, install a new output
        // device context, and update the output slot context pointer.
        //

        status = Crashdump_UsbDevice_SendEnableSlotCommand(UsbDeviceData);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        Crashdump_SetDeviceContext(UsbDeviceData->DumperHandle,
                                   UsbDeviceData->SlotId,
                                   &UsbDeviceData->DeviceContextBuffer.LogicalAddress);

        Crashdump_UsbDevice_SetOutputSlotContext(UsbDeviceData,
                                                 UsbDeviceData->DeviceContextBuffer.VirtualAddress);

        //
        // Update the output endpoint context pointers.
        //

        for (endpointDCI = 1; endpointDCI < MAX_DEVICE_CONTEXT_INDEX; endpointDCI++) {

            crashdumpEndpoint = Crashdump_UsbDevice_GetEndpointHandle(UsbDeviceData, endpointDCI);
            if (crashdumpEndpoint == NULL) {
                continue;
            }

            //
            // If we are resetting the controller in the failure path for configuring
            // the device for hibernate we need to lock the endpoint data down
            //
            powerDownType = Crashdump_GetPowerDownType(UsbDeviceData->DumperHandle);

            if (powerDownType == DumpPowerDownTypeHibernateWriteStart ||
                powerDownType == DumpPowerDownTypeHybridSleep) {

                Crashdump_BufferMarkForHibernate(crashdumpEndpoint, sizeof(CRASHDUMP_ENDPOINT_DATA));
            }

            //
            // TransferTag and CurrentSegmentIndex need to reinitialized once we
            // resume using the hibernate stack. 
            // Doing this for crashdump is safe as these values are reset to what
            // they already should be
            //
            Crashdump_Endpoint_ReInitializeForMultiStageResume(crashdumpEndpoint);

            Crashdump_Endpoint_SetOutputEndpointContext(crashdumpEndpoint,
                                                        UsbDeviceData->DeviceContextBuffer.VirtualAddress);
        }

        status = Crashdump_UsbDevice_Configure(UsbDeviceData, PortType);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

    } FINALLY {

        CrashdumpLogInfo("Crashdump_UsbDevice_ReConfigureOnControllerReset: end 0x%X", status);
    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_ReConfigureOnPortReset(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in 
        CRASHDUMP_PORT_TYPE         PortType
    )
/*++

Routine Description:

    This routine will reconfigure the Crashdump device and its endpoints after
    a Port Reset.

--*/
{
    NTSTATUS    status;

    TRY {

        CrashdumpLogInfo("Crashdump_UsbDevice_ReConfigureOnPortReset: begin");

        Crashdump_UsbDevice_LogSlotContext(UsbDeviceData);

        status = Crashdump_UsbDevice_SendResetDeviceCommand(UsbDeviceData);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        status = Crashdump_UsbDevice_Configure(UsbDeviceData, PortType);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

    } FINALLY {

        CrashdumpLogInfo("Crashdump_UsbDevice_ReConfigureOnPortReset: end 0x%X", status);
    }

    return status;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_Configure(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in 
        CRASHDUMP_PORT_TYPE         PortType
    )
/*++

Routine Description:

    This routine will reconfigure the Crashdump device and its endpoints after
    a Port Reset.

--*/
{
    NTSTATUS                        status;
    USB_DEFAULT_PIPE_SETUP_PACKET   setupPacket;
    PUSB_DEVICE_DESCRIPTOR          deviceDescriptor;

    TRY {

        CrashdumpLogInfo("Crashdump_UsbDevice_Configure: begin");

        status = Crashdump_UsbDevice_SendAddressDeviceCommand(UsbDeviceData);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        Crashdump_UsbDevice_LogSlotContext(UsbDeviceData);

        status = Crashdump_UsbDevice_GetDeviceDescriptor(UsbDeviceData, &deviceDescriptor);
        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("Crashdump_UsbDevice_GetDeviceDescriptor: failed error = 0x%X", status);
            LEAVE;
        }

        //
        // Select Configuration.
        //
        INIT_SETUP_PACKET(setupPacket,
                          USB_REQUEST_SET_CONFIGURATION,        // bRequest
                          BMREQUEST_HOST_TO_DEVICE,             // Dir
                          BMREQUEST_TO_DEVICE,                  // Recipient
                          BMREQUEST_STANDARD,                   // Type
                          UsbDeviceData->ConfigurationId,       // wValue
                          0,                                    // wIndex
                          0);                                   // wLength

        status = Crashdump_UsbDevice_SendControlTransfer(UsbDeviceData, &setupPacket, 0, NULL);
        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("USB_REQUEST_SET_CONFIGURATION: failed error = 0x%X", status);
            LEAVE;
        }

        //
        // Select Alternate Setting for UASP.
        //
        if (UsbDeviceData->AlternateSettingId != 0) {

            INIT_SETUP_PACKET(setupPacket,
                              USB_REQUEST_SET_INTERFACE,            // bRequest
                              BMREQUEST_HOST_TO_DEVICE,             // Dir
                              BMREQUEST_TO_INTERFACE,               // Recipient
                              BMREQUEST_STANDARD,                   // Type
                              UsbDeviceData->AlternateSettingId,    // wValue
                              UsbDeviceData->InterfaceId,           // wIndex
                              0);                                   // wLength

            status = Crashdump_UsbDevice_SendControlTransfer(UsbDeviceData, &setupPacket, 0, NULL);
            if (!NT_SUCCESS(status)) {
                CrashdumpLogError("USB_REQUEST_SET_INTERFACE: failed error = 0x%X", status);
                LEAVE;
            }
        }

        //
        // We can work with just the control endpoint in hubs
        //
        if (UsbDeviceData->IsHub == FALSE) {

            status = Crashdump_UsbDevice_ConfigureEndpoints(UsbDeviceData,
                                                            CRASHDUMP_CONFIGURABLE_ENDPOINTS_BITMASK,
                                                            FALSE);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        } else {
            //
            // We need to set hub depth for non-root 3.0 hubs
            //
            if (PortType == CrashdumpPortType30) {
                INIT_SETUP_PACKET(setupPacket,
                                  USB_REQUEST_SET_HUB_DEPTH,                    // bRequest
                                  BMREQUEST_HOST_TO_DEVICE,                     // Dir
                                  BMREQUEST_TO_DEVICE,                          // Recipient
                                  BMREQUEST_CLASS,                              // Type
                                  (USHORT)(UsbDeviceData->PortPathDepth-1),     // wValue
                                  0,                                            // wIndex
                                  0);                                           // wLength

                status = Crashdump_UsbDevice_SendControlTransfer(UsbDeviceData, &setupPacket, 0, NULL);
                if (!NT_SUCCESS(status)) {
                    CrashdumpLogError("USB_REQUEST_SET_HUB_DEPTH: failed depth = %u error = 0x%X", 
                                        (UsbDeviceData->PortPathDepth-1), 
                                        status);
                    LEAVE;
                }
            }
        }
    } FINALLY {

        Crashdump_UsbDevice_LogSlotContext(UsbDeviceData);
        CrashdumpLogInfo("Crashdump_UsbDevice_Configure: end 0x%X", status);
    }

    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_SendEnableSlotCommand(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    )
/*++

Routine Description:


Reference:


--*/
{
    NTSTATUS                        status;
    TRB                             trb;
    COMMAND_COMPLETION_EVENT_TRB    completionEvent;

    TRY {

        CrashdumpLogInfo("Crashdump_UsbDevice_SendEnableSlotCommand: begin");

        RtlZeroMemory(&trb, sizeof(TRB));

        trb.EnableSlotCommand.Type = TRB_TYPE_ENABLE_SLOT_COMMAND;

        status = Crashdump_Command_SendCommand(UsbDeviceData->CommandHandle,
                                               &trb,
                                               &completionEvent);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        UsbDeviceData->SlotId = (UCHAR)completionEvent.SlotId;

        CrashdumpLogInfo("SlotId is %u", UsbDeviceData->SlotId);

    } FINALLY {

        CrashdumpLogInfo("Crashdump_UsbDevice_SendEnableSlotCommand: end 0x%X", status);
    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_SendResetDeviceCommand(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    )
/*++

Routine Description:

    This routine informs the xHC that the USB device has been reset. On
    successful completion of this command, the device slot will be in
    the Default state, the device address will be set to '0', the
    default control endpoint will be in the Stopped state, and all other
    endpoints will be in the Disabled state.

Reference:

    4.5.3       - Slot States.
    4.6.11      - Reset Device.
    4.8.3       - Endpoint Context State.
    6.4.3.10    - Reset Device Command TRB.

--*/
{
    NTSTATUS    status;
    TRB         trb;

    TRY {

        CrashdumpLogInfo("Crashdump_UsbDevice_SendResetDeviceCommand: begin");

        RtlZeroMemory(&trb, sizeof(TRB));

        trb.ResetDeviceCommand.Type = TRB_TYPE_RESET_DEVICE_COMMAND;
        trb.ResetDeviceCommand.SlotId = UsbDeviceData->SlotId;

        status = Crashdump_Command_SendCommand(UsbDeviceData->CommandHandle, &trb, NULL);

        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("SlotId %u: Reset device command failed", UsbDeviceData->SlotId);
            LEAVE;
        }

        if (Crashdump_UsbDevice_GetSlotState(UsbDeviceData) != SLOT_STATE_DEFAULT) {

            CrashdumpLogError("SlotId %u: After reset device command, slot state is not default, it is %u",
                              UsbDeviceData->SlotId,
                              Crashdump_UsbDevice_GetSlotState(UsbDeviceData));

            status = STATUS_ADAPTER_HARDWARE_ERROR;
            LEAVE;
        }

    } FINALLY {

        CrashdumpLogInfo("Crashdump_UsbDevice_SendResetDeviceCommand: end 0x%X", status);
    }

    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_SendAddressDeviceCommand(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    )
/*++

Routine Description:

    This routine transitions the device slot from Default to the Configured
    state by issuing an Address Device command to the xHC with BSR=0.

Reference:

    4.3.4       - Address Assignment.
    4.5.3       - Slot States.
    4.6.5       - Address Device.
    6.2.3.1     - Address Device Command Usage.
    6.2.5       - Input Context.
    6.4.3.4     - Address Device Command TRB.

--*/
{
    NTSTATUS                status;
    TRB                     trb;
    HANDLE                  defaultEndpointHandle;
    PCRASHDUMP_BUFFER_DATA  inputDeviceContextBuffer;
    PINPUT_CONTEXT64        inputContext;
    PSLOT_CONTEXT64         inputSlotContext;
    PENDPOINT_CONTEXT64     inputEndpointContext;
    PENDPOINT_CONTEXT64     endpointContext;

    TRY {

        CrashdumpLogInfo("Crashdump_UsbDevice_SendAddressDeviceCommand: begin");

        inputDeviceContextBuffer = Crashdump_GetInputDeviceContextBuffer(UsbDeviceData->DumperHandle);

        inputContext = (PINPUT_CONTEXT64)inputDeviceContextBuffer->VirtualAddress;

        inputContext->InputControlContext.AddContextFlags.A0 = 1;
        inputContext->InputControlContext.AddContextFlags.A1 = 1;

        if (Crashdump_Register_GetContextSize(UsbDeviceData->RegisterHandle) == REGISTER_CONTEXTSIZE64) {

            inputSlotContext     = UsbDevice_InputContext64GetSlot(inputContext);
            inputEndpointContext = UsbDevice_InputContext64GetDefaultEndpoint(inputContext);

        } else {

            inputSlotContext =
                (PSLOT_CONTEXT64)UsbDevice_InputContext32GetSlot((PINPUT_CONTEXT32)inputContext);
            inputEndpointContext =
                (PENDPOINT_CONTEXT64)UsbDevice_InputContext32GetDefaultEndpoint((PINPUT_CONTEXT32)inputContext);
        }

        //
        // Slot Context.
        //
        inputSlotContext->RouteString = UsbDeviceData->SlotContext.RouteString;
        inputSlotContext->Speed = UsbDeviceData->SlotContext.Speed;
        inputSlotContext->RootHubPortNumber = UsbDeviceData->SlotContext.RootHubPortNumber;
        inputSlotContext->ContextEntries = 1;

        //
        // Default Control Endpoint Context.
        //

        defaultEndpointHandle = Crashdump_UsbDevice_GetEndpointHandle(UsbDeviceData, 1);

        endpointContext =
            Crashdump_Endpoint_GetEndpointContext(defaultEndpointHandle);

        inputEndpointContext->EndpointType = endpointContext->EndpointType;
        inputEndpointContext->MaxPacketSize = endpointContext->MaxPacketSize;
        inputEndpointContext->ErrorCount = endpointContext->ErrorCount;
        inputEndpointContext->DequeuePointer =
            Crashdump_Endpoint_GetStreamOrTransferRingLA(defaultEndpointHandle);
        inputEndpointContext->DequeueCycleState =
            Crashdump_Endpoint_GetDequeueCycleState(defaultEndpointHandle);

        //
        // Prepare and send the command TRB.
        //
        RtlZeroMemory(&trb, sizeof(TRB));

        trb.AddressDeviceCommand.Type = TRB_TYPE_ADDRESS_DEVICE_COMMAND;
        trb.AddressDeviceCommand.SlotId = UsbDeviceData->SlotId;
        trb.AddressDeviceCommand.BlockSetAddressRequest = 0;
        trb.AddressDeviceCommand.InputContextPointer = (ULONG64)inputDeviceContextBuffer->LogicalAddress.QuadPart;

        status = Crashdump_Command_SendCommand(UsbDeviceData->CommandHandle, &trb, NULL);

        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("SlotId %u: Address device command failed", UsbDeviceData->SlotId);
            LEAVE;
        }

        Crashdump_UsbDevice_LogSlotContext(UsbDeviceData);

        if (Crashdump_UsbDevice_GetSlotState(UsbDeviceData) != SLOT_STATE_ADDRESSED) {

            CrashdumpLogError("SlotId %u: After address device command, slot state is not addressed, it is %u",
                              UsbDeviceData->SlotId,
                              Crashdump_UsbDevice_GetSlotState(UsbDeviceData));
            status = STATUS_ADAPTER_HARDWARE_ERROR;
            LEAVE;
        }

    } FINALLY {

        CrashdumpLogInfo("Crashdump_UsbDevice_SendAddressDeviceCommand: end 0x%X", status);
    }

    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_ConfigureEndpoints(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        ULONG                       EndpointSelectionBitMask,
    __in
        BOOLEAN                     ReConfigure
    )
/*++

Routine Description:


Reference:

    4.3.5       - Device Configuration.
    4.5.3       - Slot States.
    4.11.4.5    - Configure Endpoint Command TRB.
    6.2.2.2     - Slot Context - Configure Endpoint Command Usage.
    6.2.3.2     - Endpoint Context - Configure Endpoint Command Usage.

--*/
{
    NTSTATUS                status;
    TRB                     trb;
    PCRASHDUMP_BUFFER_DATA  inputDeviceContextBuffer;
    PINPUT_CONTEXT64        inputContext;
    PSLOT_CONTEXT64         inputSlotContext;
    PENDPOINT_CONTEXT64     inputEndpointContext;
    PENDPOINT_CONTEXT64     endpointContext;
    ULONG                   endpointDCI;
    ULONG                   maxDCI;
    HANDLE                  crashdumpEndpoint;

    TRY {

        CrashdumpLogInfo("Crashdump_UsbDevice_ConfigureEndpoints: begin");

        inputDeviceContextBuffer = Crashdump_GetInputDeviceContextBuffer(UsbDeviceData->DumperHandle);

        inputContext = (PINPUT_CONTEXT64)inputDeviceContextBuffer->VirtualAddress;
        inputContext->InputControlContext.AddContextFlags.A0 = 1;

        //
        // Disable it for now.
        //
#if 0
        if (Crashdump_Register_GetConfigurationInformation(UsbDeviceData->RegisterHandle)) {
            inputContext->InputControlContext.ConfigurationValue = UsbDeviceData->ConfigurationId;
            inputContext->InputControlContext.AlternateSetting = UsbDeviceData->AlternateSettingId;
            inputContext->InputControlContext.InterfaceNumber = UsbDeviceData->InterfaceId;
        }
#endif

        maxDCI = 0;

        for (endpointDCI = 2; endpointDCI < MAX_DEVICE_CONTEXT_INDEX; endpointDCI++) {

            //
            // Skip if caller did not ask to configure this endpoint.
            //
            if ((EndpointSelectionBitMask & (1 << endpointDCI)) == 0) {
                continue;
            }

            //
            // Skip if an endpoint with this DCI does not exist.
            //
            crashdumpEndpoint = Crashdump_UsbDevice_GetEndpointHandle(UsbDeviceData, endpointDCI);
            if (crashdumpEndpoint == NULL) {
                continue;
            }

            if (Crashdump_Register_GetContextSize(UsbDeviceData->RegisterHandle) == REGISTER_CONTEXTSIZE64) {

                inputEndpointContext = UsbDevice_InputContext64GetEndpoint(
                    inputContext,
                    endpointDCI);

            } else {

                inputEndpointContext = (PENDPOINT_CONTEXT64)UsbDevice_InputContext32GetEndpoint(
                    (PINPUT_CONTEXT32)inputContext,
                    endpointDCI);
            }

            endpointContext = Crashdump_Endpoint_GetEndpointContext(crashdumpEndpoint);

            inputEndpointContext->EndpointType = endpointContext->EndpointType;
            inputEndpointContext->MaxPacketSize = endpointContext->MaxPacketSize;
            inputEndpointContext->ErrorCount = endpointContext->ErrorCount;
            inputEndpointContext->MaxBurstSize = endpointContext->MaxBurstSize;
            inputEndpointContext->AverageTrbLength = endpointContext->AverageTrbLength;

            inputEndpointContext->DequeuePointer =
                Crashdump_Endpoint_GetStreamOrTransferRingLA(crashdumpEndpoint);

            //
            // If endpoint is streams capable, only one stream will be used. The DequeueCycleState
            // should be 0 for stream capable endpoint because in that case the endpointContext
            // points to a stream context array and not the transfer ring.
            //
            if (endpointContext->MaxPrimaryStreams == 0) {
                inputEndpointContext->MaxPrimaryStreams = 0;
                inputEndpointContext->LinearStreamArray = 0;
                inputEndpointContext->DequeueCycleState = Crashdump_Endpoint_GetDequeueCycleState(crashdumpEndpoint);
            } else {
                inputEndpointContext->MaxPrimaryStreams = 1;
                inputEndpointContext->LinearStreamArray = 1;
                inputEndpointContext->DequeueCycleState = 0;
            }

            inputContext->InputControlContext.AddContextFlags.AsUlong32 |= (1 << endpointDCI);

            if (ReConfigure) {
                inputContext->InputControlContext.DropContextFlags.AsUlong32 |= (1 << endpointDCI);
            }

            maxDCI = endpointDCI;
        }

        //
        // Update the context entry count in the slot context.
        //
        if (Crashdump_Register_GetContextSize(UsbDeviceData->RegisterHandle) == REGISTER_CONTEXTSIZE64) {
            inputSlotContext = UsbDevice_InputContext64GetSlot(inputContext);
        } else {
            inputSlotContext =
                (PSLOT_CONTEXT64)UsbDevice_InputContext32GetSlot((PINPUT_CONTEXT32)inputContext);
        }

        NT_ASSERT(maxDCI != 0);

        inputSlotContext->ContextEntries = maxDCI;
        inputSlotContext->Hub = UsbDeviceData->SlotContext.Hub;
        inputSlotContext->NumberOfPorts = UsbDeviceData->SlotContext.NumberOfPorts;

        //
        // Prepare and send the command TRB.
        //
        RtlZeroMemory(&trb, sizeof(TRB));

        trb.ConfigureEndpointCommand.Type = TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND;
        trb.ConfigureEndpointCommand.Deconfigure = 0;
        trb.ConfigureEndpointCommand.SlotId = UsbDeviceData->SlotId;
        trb.ConfigureEndpointCommand.InputContextPointer = (ULONG64)inputDeviceContextBuffer->LogicalAddress.QuadPart;

        status = Crashdump_Command_SendCommand(UsbDeviceData->CommandHandle, &trb, NULL);

        if (!NT_SUCCESS(status)) {
            CrashdumpLogError("SlotId %u: Configure Endpoint command failed", UsbDeviceData->SlotId);
            LEAVE;
        }

        if (Crashdump_UsbDevice_GetSlotState(UsbDeviceData) != SLOT_STATE_CONFIGURED) {

            CrashdumpLogError("SlotId %u: After reset device command, slot state is not configured, it is %u",
                              UsbDeviceData->SlotId,
                              Crashdump_UsbDevice_GetSlotState(UsbDeviceData));

            status = STATUS_ADAPTER_HARDWARE_ERROR;
            LEAVE;
        }

    } FINALLY {

        CrashdumpLogInfo("Crashdump_UsbDevice_ConfigureEndpoints: end 0x%X", status);
    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_ResetPort(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        ULONG                       PortNumber,
    __in
        CRASHDUMP_PORT_TYPE         PortType
    )
/*++

Routine Description:


Reference:

                - 

--*/
{
    NTSTATUS                        status;
    ULONG                           retryCount;
    PUSB_PORT_STATUS_AND_CHANGE     portSC;

    TRY {

        CrashdumpLogInfo("Crashdump_UsbDevice_ResetPort: begin: port %u", PortNumber);

        NT_ASSERT(UsbDeviceData->IsHub);

        retryCount = 0;

        if (PortType == CrashdumpPortType20) {
            do {

                //
                // Debounce.
                //
                status = Crashdump_UsbDevice_GetPortStatus(UsbDeviceData, PortNumber, &portSC);
                if (!NT_SUCCESS(status)) {
                    LEAVE;
                }

                if (portSC->PortChange.Usb20PortChange.ConnectStatusChange == 1) {

                    status = Crashdump_UsbDevice_ChangePortFeature(UsbDeviceData,
                                                                   PortNumber,
                                                                   C_PORT_CONNECTION,
                                                                   USB_REQUEST_CLEAR_FEATURE);
                    if (!NT_SUCCESS(status)) {
                        CrashdumpLogInfo("Clearing Connect Status Change failed with 0x%X", status);
                    }
                }
                
                if (portSC->PortStatus.Usb20PortStatus.CurrentConnectStatus == 1) {
                    break;
                }

                KeStallExecutionProcessor(CRASHDUMP_HUB_PORT_CONNECT_WAIT_TIME);

                retryCount++;

            } while (retryCount < CRASHDUMP_HUB_PORT_CONNECT_WAIT_COUNT);

            if (portSC->PortStatus.Usb20PortStatus.CurrentConnectStatus != 1) {
                status = STATUS_ADAPTER_HARDWARE_ERROR;
                LEAVE;
            }

            //
            // Send port reset command to the hub.
            //
            status = Crashdump_UsbDevice_ChangePortFeature(UsbDeviceData,
                                                           PortNumber,
                                                           PORT_RESET,
                                                           USB_REQUEST_SET_FEATURE);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        } else {
            //
            // Send warm port reset command to the hub.
            //
            status = Crashdump_UsbDevice_ChangePortFeature(UsbDeviceData,
                                                           PortNumber,
                                                           BH_PORT_RESET,
                                                           USB_REQUEST_SET_FEATURE);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }
        
        KeStallExecutionProcessor(CRASHDUMP_HUB_PORT_RESET_WAIT_TIME);

        //
        // Verify that the port reset completed successfully.
        //
        for (retryCount = 0; retryCount < CRASHDUMP_HUB_PORT_RESET_WAIT_COUNT; retryCount++) {

            status = Crashdump_UsbDevice_GetPortStatus(UsbDeviceData, PortNumber, &portSC);
            if (!NT_SUCCESS(status)) {
                LEAVE;
            }

            if ((portSC->PortStatus.Usb20PortStatus.Reset == 0) &&
                (portSC->PortChange.Usb20PortChange.ResetChange == 1)) {

                CrashdumpLogInfo("Port reset took %u retries and %u ms to complete",
                                 (retryCount + 1),
                                 ((retryCount + 1) * CRASHDUMP_HUB_PORT_RESET_WAIT_TIME) / CRASHDUMP_HUB_PORT_RESET_WAIT_COUNT);

                break;
            }

            KeStallExecutionProcessor(CRASHDUMP_HUB_PORT_RESET_WAIT_TIME);
        }

        if (retryCount == CRASHDUMP_HUB_PORT_RESET_WAIT_COUNT) {

            CrashdumpLogInfo("Port reset did not complete after %u retries and %u ms",
                             retryCount,
                             (retryCount * CRASHDUMP_HUB_PORT_RESET_WAIT_TIME) / 1000);

            status = STATUS_ADAPTER_HARDWARE_ERROR;
            LEAVE;
        }

        status = STATUS_SUCCESS;

    } FINALLY {

        CrashdumpLogInfo("Crashdump_UsbDevice_ResetPort: end 0x%X", status);
    }

    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_GetPortStatus(
    __in
        PCRASHDUMP_USBDEVICE_DATA       UsbDeviceData,
    __in
        ULONG                           PortNumber,
    __out
        PUSB_PORT_STATUS_AND_CHANGE*    PortSC
    )
{
    NTSTATUS                        status;
    PCRASHDUMP_BUFFER_DATA          controlTransferBuffer;
    USB_DEFAULT_PIPE_SETUP_PACKET   setupPacket;
    PUSB_PORT_STATUS_AND_CHANGE     portSCBuffer;

    TRY {

        CrashdumpLogInfo("Crashdump_UsbDevice_GetPortStatus: begin: port %u", PortNumber);

        controlTransferBuffer = Crashdump_GetControlTransferBuffer(UsbDeviceData->DumperHandle);

        NT_ASSERT(controlTransferBuffer->Size >= sizeof(USB_PORT_STATUS_AND_CHANGE));

        portSCBuffer = (PUSB_PORT_STATUS_AND_CHANGE)controlTransferBuffer->VirtualAddress;

        INIT_SETUP_PACKET(setupPacket,
                          USB_REQUEST_GET_STATUS,               // bRequest
                          BMREQUEST_DEVICE_TO_HOST,             // Dir
                          BMREQUEST_TO_OTHER,                   // Recipient
                          BMREQUEST_CLASS,                      // Type
                          0,                                    // wValue
                          (USHORT)PortNumber,                   // wIndex
                          sizeof(USB_PORT_STATUS_AND_CHANGE));  // wLength

        status = Crashdump_UsbDevice_SendControlTransfer(UsbDeviceData,
                                                         &setupPacket,
                                                         sizeof(USB_PORT_STATUS_AND_CHANGE),
                                                         &controlTransferBuffer->LogicalAddress);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        Crashdump_UsbDevice_LogPortStatus(PortNumber, portSCBuffer);

        *PortSC = portSCBuffer;

    } FINALLY {

        CrashdumpLogInfo("Crashdump_UsbDevice_GetPortStatus: end 0x%X", status);
    }

    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_ChangePortFeature(
    __in
        PCRASHDUMP_USBDEVICE_DATA       UsbDeviceData,
    __in
        ULONG                           PortNumber,
    __in
        USB_PORT_FEATURE_SELECTOR       PortFeature,
    __in
        UCHAR                           Request
    )
{
    NTSTATUS                        status;
    USB_DEFAULT_PIPE_SETUP_PACKET   setupPacket;

    TRY {

        CrashdumpLogInfo("Crashdump_UsbDevice_ChangePortFeature: begin: port: %u, feature: %u, request: %u",
                         PortNumber,
                         PortFeature,
                         Request);

        INIT_SETUP_PACKET(setupPacket,
                          Request,                              // bRequest
                          BMREQUEST_HOST_TO_DEVICE,             // Dir
                          BMREQUEST_TO_OTHER,                   // Recipient
                          BMREQUEST_CLASS,                      // Type
                          PortFeature,                          // wValue
                          (USHORT)PortNumber,                   // wIndex
                          0);                                   // wLength

        status = Crashdump_UsbDevice_SendControlTransfer(UsbDeviceData, &setupPacket, 0, NULL);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

    } FINALLY {

        CrashdumpLogInfo("Crashdump_UsbDevice_ChangePortFeature: end 0x%X", status);
    }

    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_GetDeviceDescriptor(
    __in
        PCRASHDUMP_USBDEVICE_DATA       UsbDeviceData,
    __out
        PUSB_DEVICE_DESCRIPTOR*         DeviceDescriptor
    )
{
    NTSTATUS                        status;
    PCRASHDUMP_BUFFER_DATA          controlTransferBuffer;
    USB_DEFAULT_PIPE_SETUP_PACKET   setupPacket;
    PUSB_DEVICE_DESCRIPTOR          deviceDescriptorBuffer;

    TRY {

        CrashdumpLogInfo("Crashdump_UsbDevice_GetDeviceDescriptor: begin");

        controlTransferBuffer = Crashdump_GetControlTransferBuffer(UsbDeviceData->DumperHandle);

        NT_ASSERT(controlTransferBuffer->Size >= sizeof(USB_DEVICE_DESCRIPTOR));

        deviceDescriptorBuffer =
            (PUSB_DEVICE_DESCRIPTOR)controlTransferBuffer->VirtualAddress;

        INIT_SETUP_PACKET(setupPacket,
                          USB_REQUEST_GET_DESCRIPTOR,                                           // bRequest
                          BMREQUEST_DEVICE_TO_HOST,                                             // Dir
                          BMREQUEST_TO_DEVICE,                                                  // Recipient
                          BMREQUEST_STANDARD,                                                   // Type
                          USB_DESCRIPTOR_MAKE_TYPE_AND_INDEX(USB_DEVICE_DESCRIPTOR_TYPE, 0),    // wValue
                          0,                                                                    // wIndex
                          USB_DEFAULT_MAX_PACKET);                                              // wLength

        status = Crashdump_UsbDevice_SendControlTransfer(UsbDeviceData,
                                                         &setupPacket,
                                                         sizeof(USB_DEVICE_DESCRIPTOR),
                                                         &controlTransferBuffer->LogicalAddress);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        Crashdump_UsbDevice_LogDeviceDescriptor(deviceDescriptorBuffer);

        *DeviceDescriptor = deviceDescriptorBuffer;

    } FINALLY {

        CrashdumpLogInfo("Crashdump_UsbDevice_GetDeviceDescriptor: end 0x%X", status);
    }

    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_SetPortPower(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        ULONG                       PortNumber
    )
/*++

Routine Description:


Reference:

                - 

--*/
{
    NTSTATUS                        status;

    TRY {

        CrashdumpLogInfo("Crashdump_UsbDevice_SetPortPower: begin: port %u", PortNumber);

        NT_ASSERT(UsbDeviceData->IsHub);

        //
        // Set the port power bit.
        //
        status = Crashdump_UsbDevice_ChangePortFeature(UsbDeviceData,
                                                       PortNumber,
                                                       PORT_POWER,
                                                       USB_REQUEST_SET_FEATURE);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        KeStallExecutionProcessor(CRASHDUMP_HUB_PORT_POWER_WAIT_TIME);
    } FINALLY {
        CrashdumpLogInfo("Crashdump_UsbDevice_SetPortPower: end 0x%X", status);
    }

    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_GetPortType(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        ULONG                       PortNumber,
    __out
        CRASHDUMP_PORT_TYPE         *PortType
    )
/*++

Routine Description:
    Given a port number on a hub, this routine return the port type

Reference:

                - 

--*/
{
    
    PUSB_PORT_STATUS_AND_CHANGE     portSC;
    NTSTATUS                        status;

    TRY {
        CrashdumpLogInfo("Crashdump_UsbDevice_GetPortType: begin: port %u", PortNumber);
        status = Crashdump_UsbDevice_GetPortStatus(UsbDeviceData,
                                                   PortNumber,
                                                   &portSC);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        // Assumption here is that we will never need to write dump to a full speed
        // or a low speed device
        if (portSC->PortStatus.Usb20PortStatus.HighSpeedDeviceAttached) {
            *PortType = CrashdumpPortType20;
        } else {
            *PortType = CrashdumpPortType30;
        }
    } FINALLY {
        CrashdumpLogInfo("Crashdump_UsbDevice_GetPortType: end 0x%X", status);
    }

    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_SuspendPort(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData,
    __in
        ULONG                       PortNumber
    )
{
    NTSTATUS                        status;
    CRASHDUMP_PORT_TYPE             portType;
    USB_DEFAULT_PIPE_SETUP_PACKET   setupPacket;

    TRY {
        status = Crashdump_UsbDevice_GetPortType(UsbDeviceData,
                                                 PortNumber,
                                                 &portType);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        //
        // For the non-root hub, we need to send suspend port for 2.0 ports
        // and for 3.0 ports we need to set the port link state to U3
        //
        if (portType == CrashdumpPortType20) {
            status = Crashdump_UsbDevice_ChangePortFeature(UsbDeviceData,
                                                           PortNumber,
                                                           PORT_SUSPEND,
                                                           USB_REQUEST_SET_FEATURE);
            if (!NT_SUCCESS(status)) {
                CrashdumpLogError("Crashdump_UsbDevice_SuspendPort: Crashdump_UsbDevice_ChangePortFeature failed with 0x%X",status);
                LEAVE;
            }
        } else {
            INIT_SETUP_PACKET(setupPacket,
                              USB_REQUEST_SET_FEATURE,              // bRequest
                              BMREQUEST_HOST_TO_DEVICE,             // Dir
                              BMREQUEST_TO_OTHER,                   // Recipient
                              BMREQUEST_CLASS,                      // Type
                              PORT_LINK_STATE,                      // wValue
                              0,                                    // wIndex
                              0);                                   // wLength

            setupPacket.wIndex.LowByte = (UCHAR)PortNumber;
            //
            // HiByte indicates the link state. For suspend, 
            // we need to put the link into U3
            //
            setupPacket.wIndex.HiByte = PORT_LINK_STATE_U3;
            
            status = Crashdump_UsbDevice_SendControlTransfer(UsbDeviceData,
                                                             &setupPacket,
                                                             0,
                                                             NULL);
            if (!NT_SUCCESS(status)) {
                CrashdumpLogError("Crashdump_UsbDevice_SuspendPort: Crashdump_UsbDevice_SendControlTransfer failed with 0x%X",status);
                LEAVE;
            }

        }
    } FINALLY{
    }
    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_Cleanup(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    )
/*++

Routine Description:


--*/
{
    NTSTATUS    status;
    ULONG       endpointDCI;
    HANDLE      crashdumpEndpoint;

    TRY {

        CrashdumpLogInfo("Crashdump_UsbDevice_Cleanup: begin");

        RtlZeroMemory(UsbDeviceData->DeviceContextBuffer.VirtualAddress,
                      UsbDeviceData->DeviceContextBuffer.Size);

        for (endpointDCI = 1; endpointDCI < MAX_DEVICE_CONTEXT_INDEX; endpointDCI++) {

            crashdumpEndpoint = Crashdump_UsbDevice_GetEndpointHandle(UsbDeviceData, endpointDCI);
            if (crashdumpEndpoint == NULL) {
                continue;
            }

            status = Crashdump_Endpoint_Cleanup(crashdumpEndpoint);
            if (!NT_SUCCESS(status)) {
                CrashdumpLogError("Crashdump_Endpoint_Cleanup failed with 0x%X",status);
                LEAVE;
            }
        }

        status = STATUS_SUCCESS;

    } FINALLY {

        CrashdumpLogInfo("Crashdump_UsbDevice_Cleanup: end 0x%X", status);
    }

    return status;
}

__checkReturn
__drv_requiresIRQL(DISPATCH_LEVEL)
NTSTATUS
Crashdump_UsbDevice_ConfigureEndpointsForSaveState(
    __in
        PCRASHDUMP_USBDEVICE_DATA   UsbDeviceData
    )
/*++

Routine Description:

    This routine restores the DequeuePointer values for each of the endpoints of
    the device to those of the runtime stack, before the save state operation. This
    allows the runtime stack to just continue where it left off.

Reference:

    4.3.5       - Device Configuration.
    4.5.3       - Slot States.
    4.11.4.5    - Configure Endpoint Command TRB.
    6.2.2.2     - Slot Context - Configure Endpoint Command Usage.
    6.2.3.2     - Endpoint Context - Configure Endpoint Command Usage.

--*/
{
    NTSTATUS    status;
    TRB         trb;
    ULONG       endpointDCI;
    HANDLE      endpoint;
    HANDLE      crashdumpEndpoint;
    
    TRY {

        status = STATUS_UNSUCCESSFUL;

        for (endpointDCI = 1; endpointDCI < MAX_DEVICE_CONTEXT_INDEX; endpointDCI += 1) {
        
            endpoint = UsbDevice_GetEndpointHandle(UsbDeviceData->UsbDeviceHandle, endpointDCI);

            // 
            // Runtime stack may have more endpoints configured for the device than the crashdump
            // stack in case of composite device. Only restore the DequeuePointers for the endpoints
            // that were used by the crashdump stack.
            //
            crashdumpEndpoint = Crashdump_UsbDevice_GetEndpointHandle(UsbDeviceData, endpointDCI);
            if (crashdumpEndpoint == NULL) {
                continue;
            }

            NT_ASSERT(endpoint != NULL);
        
            RtlZeroMemory(&trb, sizeof(TRB));
        
            trb.SetDequeuePointerCommand.Type           = TRB_TYPE_SET_DEQUEUE_POINTER_COMMAND;
            trb.SetDequeuePointerCommand.DequeuePointer = Endpoint_GetDequeuePointer(endpoint, 1);
            trb.SetDequeuePointerCommand.EndpointId     = endpointDCI;
            trb.SetDequeuePointerCommand.SlotId         = UsbDeviceData->SlotId;

            //
            // For a stream endpoint, only restore the DequeuePointer for the first stream since that
            // is the only stream used by the crashdump stack.
            //
            if (Endpoint_GetMaxPrimaryStreams(endpoint) != 0 ) {
                trb.SetDequeuePointerCommand.StreamId = 1;
            }

            status = Crashdump_Command_SendCommand(UsbDeviceData->CommandHandle, &trb, NULL);

            if (!NT_SUCCESS(status)) {
                LEAVE;
            }
        }

    } FINALLY {

        CrashdumpLogInfo("Crashdump_UsbDevice_ConfigureEndpointsForSaveState: end 0x%X", status);
    }

    return status;
}

