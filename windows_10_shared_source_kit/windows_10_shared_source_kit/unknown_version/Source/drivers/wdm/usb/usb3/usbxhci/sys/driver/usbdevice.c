/*++
Copyright (c) Microsoft Corporation

Module Name:

    usbdevice.c

Abstract:

    USB XHCI Driver

--*/

#include "pch.h"

#include "usbdevice.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, UsbDevice_UcxEvtDeviceAdd)
#endif

NTSTATUS
UsbDevice_UcxEvtDeviceAdd(
    UCXCONTROLLER       UcxController,
    PUCXUSBDEVICE_INFO  UsbDeviceInfo,
    PUCXUSBDEVICE_INIT  UsbDeviceInit
    )
/*++

Routine Description:

    This routine is a callback which is called by the class extension
    when a new usb device is detected.  This callback is registered
    with the class extension when the controller object is created via
    calling UcxControllerCreate().

    This routine creates a new usb device object and registers the
    various usb device object callback routines by calling
    UcxUsbDeviceCreate().

    This routine also allocates the common buffer that will be used as
    the Device Context (see section 6.2.1 of the XHCI specification)
    for the usb device.

--*/
{
    NTSTATUS                        status;
    HANDLE                          controllerHandle;
    HANDLE                          commonBufferHandle;
    HANDLE                          registerHandle;
    UCX_USBDEVICE_EVENT_CALLBACKS   ucxUsbDeviceEventCallbacks;
    WDF_OBJECT_ATTRIBUTES           wdfAttributes;
    UCXUSBDEVICE                    ucxUsbDevice;
    PUSBDEVICE_DATA                 usbDeviceData;

    PAGED_CODE();

    TRY {

        controllerHandle = GetControllerData(UcxController);
        commonBufferHandle = Controller_GetCommonBufferHandle(controllerHandle);
        registerHandle = Controller_GetRegisterHandle(controllerHandle);

        //
        // Set the event callbacks for the USB device.
        //
        UCX_USBDEVICE_EVENT_CALLBACKS_INIT(&ucxUsbDeviceEventCallbacks,
                                           UsbDevice_UcxEvtEndpointsConfigure,
                                           UsbDevice_UcxEvtEnable,
                                           UsbDevice_UcxEvtDisable,
                                           UsbDevice_UcxEvtReset,
                                           UsbDevice_UcxEvtAddress,
                                           UsbDevice_UcxEvtUpdate,
                                           UsbDevice_UcxEvtHubInfo,
                                           Endpoint_UcxEvtUsbDeviceDefaultEndpointAdd,
                                           Endpoint_UcxEvtUsbDeviceEndpointAdd);

        UcxUsbDeviceInitSetEventCallbacks(UsbDeviceInit, &ucxUsbDeviceEventCallbacks);

        //
        // Create the UCX USB device object.
        //
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, USBDEVICE_DATA);

        wdfAttributes.EvtCleanupCallback = UsbDevice_EvtUsbDeviceCleanupCallback;

        status = UcxUsbDeviceCreate(UcxController,
                                    &UsbDeviceInit,
                                    &wdfAttributes,
                                    &ucxUsbDevice);

        if (!NT_SUCCESS(status)) {

            LogError(Controller_GetRecorderLog(controllerHandle), UsbDevice,
                     "0: UcxUsbDeviceCreate Failed %!STATUS!", status);
            LEAVE;
        }

        LogInfo(Controller_GetRecorderLog(controllerHandle), UsbDevice,
                "0: ucxUsbDevice 0x%p", ucxUsbDevice);

        //
        // Retrieve the USBDEVICE_DATA pointer from the newly created
        // UCXUSBDEVICE object and store in it the currently available
        // information about the new usb device.
        //
        // Some information about the new device is currently unknown
        // and will be updated when UsbDevice_UcxEvtUpdate() is called
        // later.
        //
        usbDeviceData = GetUsbDeviceData(ucxUsbDevice);

        usbDeviceData->UcxUsbDevice = ucxUsbDevice;
        usbDeviceData->ControllerHandle = controllerHandle;

        RtlCopyMemory(&usbDeviceData->UsbDeviceInfo, UsbDeviceInfo, sizeof(*UsbDeviceInfo));

        //
        // Allocate the zero initialied common buffer that will be used
        // as the Device Context.  If the HCCPARAMS register Context
        // Size (CSZ) bit is set to '1' then the xHC uses 64-byte
        // Context data structures.  If this bit is cleared to '0' then
        // the xHC uses 32-byte Context data structures.
        //
        if (Register_GetContextSize(registerHandle) == REGISTER_CONTEXTSIZE64) {

            usbDeviceData->DeviceContextBuffer = CommonBuffer_AcquireBuffer(commonBufferHandle,
                                                                            sizeof(DEVICE_CONTEXT64),
                                                                            usbDeviceData,
                                                                            USBDEVICE_ALLOC_TAG1);
        } else {

            usbDeviceData->DeviceContextBuffer = CommonBuffer_AcquireBuffer(commonBufferHandle,
                                                                            sizeof(DEVICE_CONTEXT32),
                                                                            usbDeviceData,
                                                                            USBDEVICE_ALLOC_TAG1);
        }

        if (usbDeviceData->DeviceContextBuffer == NULL) {

            LogError(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                     "0: ucxUsbDevice 0x%p: Failed to acquire deviceContextBuffer", ucxUsbDevice);
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        //
        // Allocate common buffer that will be used as the input device context
        // buffer for commands like address_device, configure_endpoint etc.
        //
        if (Register_GetContextSize(registerHandle) == REGISTER_CONTEXTSIZE64) {

            usbDeviceData->InputDeviceContextBuffer = CommonBuffer_AcquireBuffer(commonBufferHandle,
                                                                                 sizeof(INPUT_CONTEXT64),
                                                                                 usbDeviceData,
                                                                                 USBDEVICE_ALLOC_TAG2);
        } else {

            usbDeviceData->InputDeviceContextBuffer = CommonBuffer_AcquireBuffer(commonBufferHandle,
                                                                                 sizeof(INPUT_CONTEXT32),
                                                                                 usbDeviceData,
                                                                                 USBDEVICE_ALLOC_TAG2);
        }

        if (usbDeviceData->InputDeviceContextBuffer == NULL) {

            LogError(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                     "0: ucxUsbDevice 0x%p: Failed to acquire InputDeviceContextBuffer", ucxUsbDevice);
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        //
        // Initialize the list of endpoints for this device.
        //
        KeInitializeSpinLock(&usbDeviceData->EndpointListLock);
        InitializeListHead(&usbDeviceData->EndpointListHead);
        usbDeviceData->EndpointListCount = 0;

        //
        // Insert this device into the list of all devices that exist
        // for this controller.
        //
        Controller_AddDeviceToControllerDeviceList(controllerHandle,
            ucxUsbDevice,
            &usbDeviceData->DeviceListEntry);

        Etw_DeviceCreate(NULL, usbDeviceData);

        LogInfo(Controller_GetRecorderLog(controllerHandle), UsbDevice,
                "0: DeviceSpeed %d TtHub 0x%p PortPath[0] %d",
                usbDeviceData->UsbDeviceInfo.DeviceSpeed,
                usbDeviceData->UsbDeviceInfo.TtHub,
                usbDeviceData->UsbDeviceInfo.PortPath.PortPath[0]);

    } FINALLY {

    }

    return status;
}

VOID
UsbDevice_EvtUsbDeviceCleanupCallback(
    WDFOBJECT           UcxUsbDevice
    )
/*++

Routine Description:

Return Value:

--*/
{
    PUSBDEVICE_DATA     usbDeviceData;

    TRY {

        usbDeviceData = GetUsbDeviceData(UcxUsbDevice);

        if (usbDeviceData->UcxUsbDevice != UcxUsbDevice) {

            //
            // UcxUsbDeviceCreate() may have failed and
            // UsbDevice_UcxEvtDeviceAdd() initialization did not
            // complete.
            //
            LEAVE;
        }

        NT_ASSERT(usbDeviceData->SlotId == 0);

        LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                "%d: USB Device 0x%p UsbDeviceData 0x%p DeviceContextBuffer 0x%p",
                usbDeviceData->SlotId, UcxUsbDevice, usbDeviceData, usbDeviceData->DeviceContextBuffer);

        if (usbDeviceData->DeviceContextBuffer != NULL) {

            CommonBuffer_ReleaseBuffer(Controller_GetCommonBufferHandle(usbDeviceData->ControllerHandle),
                                       usbDeviceData->DeviceContextBuffer);
        }

        if (usbDeviceData->InputDeviceContextBuffer != NULL) {

            CommonBuffer_ReleaseBuffer(Controller_GetCommonBufferHandle(usbDeviceData->ControllerHandle),
                                       usbDeviceData->InputDeviceContextBuffer);
        }

        //
        // Remove this device from the list of all devices that exist
        // for this controller.
        //
        if (IsListEntryNULL(&usbDeviceData->DeviceListEntry) == FALSE) {
            Controller_RemoveDeviceFromControllerDeviceList(usbDeviceData->ControllerHandle,
                usbDeviceData->UcxUsbDevice,
                &usbDeviceData->DeviceListEntry);
        }

        Etw_DeviceDelete(NULL, usbDeviceData);

    } FINALLY {

    }

    return;
}

VOID
UsbDevice_UcxEvtEnable(
    UCXCONTROLLER   UcxController,
    WDFREQUEST      WdfRequest
    )
/*++

Routine Description:

    This routine is a callback which is called by the class extension
    after a new usb device is created to program the information about
    the device and its default control endpoint into the controller.

    Upon completion of this request the controller should be prepared to
    accept and schedule transfers on the usb device's default control
    endpoint.

    The device is currently in the Disabled state and an attempt will be
    made to transition the device to the Enabled state by issuing a
    TRB_TYPE_ENABLE_SLOT_COMMAND to the controller for the device.

    See section 4.5.3 Slot States and Figure 10: Slot State Diagram in
    the XHCI specification.

    See section 4.6.3 Enable Slot for a detailed description of the
    Enable Slot command.

Arguments:

    UcxController - The UCX controller object.

    WdfRequest - The WDF request object for the IOCTL_INTERNAL_USB_USBDEVICE_ENABLE request.

Return Value:

    None directly from this routine.

    The completion status in the WdfRequest, which may be completed asynchronously.

--*/
{
    NTSTATUS                status;
    WDF_REQUEST_PARAMETERS  wdfRequestParams;
    PUSBDEVICE_ENABLE       usbDeviceEnable;
    PUSBDEVICE_DATA         usbDeviceData;
    HANDLE                  commandHandle;
    BOOLEAN                 defaultEndpointEnabled = FALSE;
    BOOLEAN                 freeResources;
    BOOLEAN                 completeRequest;

    UNREFERENCED_PARAMETER(UcxController);

    TRY {

        //
        // Retrieve the USBDEVICE_ENABLE pointer from the
        // IOCTL_INTERNAL_USB_USBDEVICE_ENABLE WdfRequest.
        //
        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);
        usbDeviceEnable = (PUSBDEVICE_ENABLE)wdfRequestParams.Parameters.Others.Arg1;

        //
        // Retrieve the USBDEVICE_DATA pointer from the UCXUSBDEVICE in
        // the USBDEVICE_MGMT_HEADER of the USBDEVICE_ENABLE.
        //
        usbDeviceData = GetUsbDeviceData(usbDeviceEnable->UsbDevice);
        NT_ASSERT(usbDeviceData != NULL);

        commandHandle = Controller_GetCommandHandle(usbDeviceData->ControllerHandle);
        NT_ASSERT(commandHandle != NULL);

        NT_ASSERT(usbDeviceData->WdfRequest == NULL);
        NT_ASSERT(usbDeviceData->SlotEnabled == FALSE);
        NT_ASSERT(usbDeviceData->SlotId == 0);

        //
        // Enable the default control endpoint.
        //
        status = Endpoint_Enable(usbDeviceData->EndpointHandle[1]);

        if (!NT_SUCCESS(status)) {

            LogError(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                     "0: Endpoint_Enable Failed %!STATUS!", status);
            freeResources = TRUE;
            completeRequest = TRUE;
            LEAVE;

        } else {

            defaultEndpointEnabled = TRUE;
        }

        //
        // If controller is gone, there is a race between this UCX callback processing
        // a request, and UsbDevice_ControllerGone trying to complete outstanding
        // requests. This check here minimizes the window in which that race exists,
        // which is a sufficiently robust low risk fix for Windows 8.
        //
        if (Controller_IsControllerAccessible(usbDeviceData->ControllerHandle) == FALSE) {
            status = STATUS_NO_SUCH_DEVICE;
            freeResources = TRUE;
            completeRequest = TRUE;
            LEAVE;
        }

        //
        // Store the IOCTL_INTERNAL_USB_USBDEVICE_ENABLE WdfRequest in
        // the USBDEVICE_DATA.  In the successful case this WdfRequest
        // will be asynchronously completed by UsbDevice_SetAddressCompletion().
        //
        usbDeviceData->WdfRequest = WdfRequest;
        usbDeviceData->WdfRequestMustSucceed = FALSE;

        //
        // See section 4.3.2 Device Slot Assignment in the XHCI
        // specification.
        //
        // The first operation that software shall perform after
        // detecting a device attach event and resetting the port is to
        // obtain a Device Slot for the device by issuing an Enable Slot
        // Command to the xHC through the Command Ring. The Enable Slot
        // Command returns a Slot ID that is selected by the host
        // controller. Refer to section 4.6.3 for a detailed description
        // of the Enable Slot command.
        //
        RtlZeroMemory(&usbDeviceData->Crb, sizeof(usbDeviceData->Crb));
        usbDeviceData->Crb.CommandCallback = UsbDevice_EnableCompletion;
        usbDeviceData->Crb.ObjectHandle = (HANDLE)usbDeviceData;
        usbDeviceData->Crb.Trb.EnableSlotCommand.Type = TRB_TYPE_ENABLE_SLOT_COMMAND;

        Command_SendCommand(commandHandle, &usbDeviceData->Crb);

        //
        // Resources will be freed and the request will be completed
        // by UsbDevice_EnableCompletion().
        //
        freeResources = FALSE;
        completeRequest = FALSE;

    } FINALLY {

        if (freeResources) {

            if (defaultEndpointEnabled == TRUE) {

                Endpoint_Disable(usbDeviceData->EndpointHandle[1], TRUE);
            }
        }

        if (completeRequest) {

            usbDeviceData->WdfRequest = NULL;
            WdfRequestComplete(WdfRequest, status);
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_EnableCompletion(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This routine is called on a command completion event for a
    TRB_TYPE_ENABLE_SLOT_COMMAND which was issued by
    UsbDevice_UcxEvtEnable().

    If a device slot was successfully enabled for the usb device then
    the device is currently in the Enabled state and an attempt will be
    made to transition the device to the Default state by issuing a
    TRB_TYPE_ADDRESS_DEVICE_COMMAND to the controller for the device
    with the Block Set Address Request (BSR) flag set to '1'.

    See section 4.5.3 Slot States and Figure 10: Slot State Diagram in
    the XHCI specification.

Arguments:

    EventTRB - Used for logging Hardware verifier events

--*/
{
    NTSTATUS        status;
    PUSBDEVICE_DATA usbDeviceData;
    HANDLE          registerHandle;
    HANDLE          deviceSlotHandle;
    WDFREQUEST      wdfRequest;
    BOOLEAN         freeResources;
    BOOLEAN         completeRequest;

    TRY {

        usbDeviceData = (PUSBDEVICE_DATA)Crb->ObjectHandle;
        registerHandle = Controller_GetRegisterHandle(usbDeviceData->ControllerHandle);
        deviceSlotHandle = Controller_GetDeviceSlotHandle(usbDeviceData->ControllerHandle);

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the enable slot
            // command effectively failed.
            //
            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "0: USB Device 0x%p - controller gone - enable slot failed",
                    usbDeviceData->UcxUsbDevice);

            if (usbDeviceData->WdfRequestMustSucceed) {
                status = STATUS_SUCCESS;
            } else {
                status = STATUS_UNSUCCESSFUL;
            }

            freeResources = TRUE;
            completeRequest = TRUE;
            LEAVE;
        }

        if (Crb->CompletionCode == TRB_COMPLETION_CODE_SUCCESS) {

            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p slot enabled", Crb->SlotId, usbDeviceData->UcxUsbDevice);

            usbDeviceData->SlotEnabled = TRUE;

            //
            // On successful completion of the TRB_TYPE_ENABLE_SLOT_COMMAND
            // the slot id of the newly allocated Device Slot for the
            // usb device is returned in the Command Completion Event
            // TRB Slot ID field.
            //
            // Store the slot id of the newly allocated Device Slot in
            // the USBDEVICE_DATA for the device.
            //
            usbDeviceData->SlotId = Crb->SlotId;

            //
            // Set the log identifier for the endpoint. This is done after the slot id is
            // retrieved, otherwise the incorrect slot number is displayed in the log id.
            //
            Endpoint_SetLogIdentifier(usbDeviceData->EndpointHandle[1]);

            //
            // Set the Device Context in the device slot array as
            // indexed by the newly allocated slot id.
            //
            // The common buffer for the Device Context was previously
            // allocated by UsbDevice_UcxEvtDeviceAdd().
            //
            // This should never fail.  If it does fail, then something
            // internal to xHCI or the USB3 stack went wrong and that
            // needs to be understood. The xHCI driver won't free the
            // slot in this case and the device slot will be leaked.
            //
            RtlZeroMemory(usbDeviceData->DeviceContextBuffer->VirtualAddress,
                          usbDeviceData->DeviceContextBuffer->Size);

            status = DeviceSlot_SetDeviceContext(deviceSlotHandle,
                                                 (HANDLE)usbDeviceData,
                                                 usbDeviceData->SlotId,
                                                 usbDeviceData->DeviceContextBuffer->LogicalAddress);

            if (!NT_SUCCESS(status)) {

                LogError(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                         "%d: DeviceSlot_SetDeviceContext Failed %!STATUS!",
                         usbDeviceData->SlotId, status);

                Controller_HwVerifierBreakIfEnabled(
                    usbDeviceData->ControllerHandle,
                    usbDeviceData->UcxUsbDevice,
                    NULL,
                    ControllerHwVerifierEnableSlotReturnedBadSlotId,
                    "Enable Slot command returned a SlotId value that is already in use",
                    &Crb->Trb,
                    EventTRB);

                Controller_ReportFatalError(usbDeviceData->ControllerHandle,
                                            Controller_FatalErrorReset,
                                            USB3_WER_BUGCODE_USBXHCI_BAD_SLOTID,
                                            NULL,
                                            NULL,
                                            NULL);

                freeResources = FALSE;
                completeRequest = FALSE;                
                LEAVE;
            }

            //
            // Store the doorbell register that is used by this device.
            // This is done after the slot id is retrieved, since the
            // index to the doorbell register is based on the slot id
            // value.
            //
            usbDeviceData->DoorbellRegister =
                Register_GetDoorbellRegister(registerHandle, usbDeviceData->SlotId);

            //
            // Attempt to transition the device to the Default state
            // with the default control endpoint enabled by issuing a
            // TRB_TYPE_ADDRESS_DEVICE_COMMAND to the controller for the
            // device with the Block Set Address Request (BSR) flag set
            // to '1'.
            //
            status = UsbDevice_SetAddress(usbDeviceData, TRUE);

            if (!NT_SUCCESS(status)) {

                HANDLE  commandHandle;

                LogError(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                         "%d: UsbDevice_SetAddress Failed %!STATUS!", usbDeviceData->SlotId, status);

                commandHandle = Controller_GetCommandHandle(usbDeviceData->ControllerHandle);
                NT_ASSERT(commandHandle != NULL);

                //
                // The Enable Slot / Address Device (BSR=1) sequence is
                // considered an all or nothing atomic operation for the
                // IOCTL_INTERNAL_USB_USBDEVICE_ENABLE request.  If the
                // Enable Slot command was successful but the Address
                // Device command was not then the device slot must be
                // returned to the disabled state.
                //
                // Refer to section 4.6.4 for a detailed description of the
                // Disable Slot command.
                //
                RtlZeroMemory(&usbDeviceData->Crb, sizeof(usbDeviceData->Crb));
                
                if (usbDeviceData->WdfRequestMustSucceed) {
                    //
                    // Invoke controller reset recovery  
                    //
                    Controller_ReportFatalError(usbDeviceData->ControllerHandle,
                                                Controller_FatalErrorReset,
                                                USB3_WER_BUGCODE_USBXHCI_SET_ADDRESS_FAILED_BSR1,
                                                NULL,
                                                NULL,
                                                NULL);
                    
                } else {
                    usbDeviceData->Crb.CommandCallback = UsbDevice_DisableCompletionReturnFailure;
                    usbDeviceData->Crb.ObjectHandle = (HANDLE)usbDeviceData;
                    usbDeviceData->Crb.Trb.DisableSlotCommand.Type = TRB_TYPE_DISABLE_SLOT_COMMAND;
                    usbDeviceData->Crb.Trb.DisableSlotCommand.SlotId = usbDeviceData->SlotId;
                    
                    Command_SendCommand(commandHandle, &usbDeviceData->Crb);
                }

                //
                // Resources will be freed and the request will be
                // completed by UsbDevice_DisableCompletionReturnFailure().
                //
                freeResources = FALSE;
                completeRequest = FALSE;
            }

            //
            // Resources will be freed and the request will be completed
            // by UsbDevice_SetAddressCompletion().
            //
            freeResources = FALSE;
            completeRequest = FALSE;

        } else {

            LogWarning(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                       "0: Failed to enable slot: %!TrbCompletionCode!",
                       (TRB_COMPLETION_CODE)Crb->CompletionCode);

            if (Crb->CompletionCode == TRB_COMPLETION_CODE_NO_SLOTS_AVAILABLE_ERROR) {
                WDF_REQUEST_PARAMETERS  wdfRequestParams;
                PUSBDEVICE_ENABLE       usbDeviceEnable;

                //
                // Retrieve the USBDEVICE_ENABLE pointer from the
                // IOCTL_INTERNAL_USB_USBDEVICE_ENABLE WdfRequest.
                //
                WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
                WdfRequestGetParameters(usbDeviceData->WdfRequest, &wdfRequestParams);
                usbDeviceEnable = (PUSBDEVICE_ENABLE)wdfRequestParams.Parameters.Others.Arg1;

                usbDeviceEnable->FailureFlags.InsufficientHardwareResourcesForDevice = 1;
            }

            if (usbDeviceData->WdfRequestMustSucceed) {
                //
                // Invoke Controller Reset here, since as per UCX-HCD interface, 
                // cannot fail the Reset Device request, and the hardware should not 
                // be failing the Enable Slot command in this particular case
                //
                Controller_ReportFatalError(usbDeviceData->ControllerHandle,
                                            Controller_FatalErrorReset,
                                            USB3_WER_BUGCODE_USBXHCI_ENABLE_SLOT_FAILED,
                                            NULL,
                                            NULL,
                                            NULL);
                completeRequest = FALSE;
            } else {
                completeRequest = TRUE;
            }

            status = STATUS_UNSUCCESSFUL;
            freeResources = TRUE;
            LEAVE;
        }

    } FINALLY {

        if (freeResources == TRUE) {

            Endpoint_Disable(usbDeviceData->EndpointHandle[1], TRUE);
        }

        if (completeRequest) {

            wdfRequest = usbDeviceData->WdfRequest;
            usbDeviceData->WdfRequest = NULL;
            WdfRequestComplete(wdfRequest, status);
        }
    }

    return;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
UsbDevice_SetAddress(
    __in
        PUSBDEVICE_DATA UsbDeviceData,
    __in
        BOOLEAN         BlockSetAddressRequest
    )
/*++

Routine Description:

    This routine builds and issues a TRB_TYPE_ADDRESS_DEVICE_COMMAND to
    the controller to:

    (1) Transition the device to the Default state if the
    BlockSetAddressRequest argument is TRUE in the case of handling an
    IOCTL_INTERNAL_USB_USBDEVICE_ENABLE request via
    UsbDevice_UcxEvtEnable().

    (2) Transition the device to the Addressed state if
    the BlockSetAddressRequest argument is FALSE in the case of
    handling an IOCTL_INTERNAL_USB_USBDEVICE_ADDRESS request via
    UsbDevice_UcxEvtAddress().

--*/
{
    NTSTATUS                status;
    HANDLE                  registerHandle;
    HANDLE                  commandHandle;
    PINPUT_CONTEXT64        inputContext;
    PSLOT_CONTEXT64         slotContext;
    PENDPOINT_CONTEXT64     endpointContext;
    PUCXUSBDEVICE_INFO      usbDeviceInfo;
    ULONG                   i;
    ULONG                   routeString;

    TRY {

        registerHandle = Controller_GetRegisterHandle(UsbDeviceData->ControllerHandle);
        commandHandle = Controller_GetCommandHandle(UsbDeviceData->ControllerHandle);

        //
        // Initialize the Input Device Context.
        //
        // See section 6.2.5 Input Context in the XHCI specification.
        // See also section 4.6.5 Address Device.
        //
        // This code needs to deal with both 32 and 64 byte contexts. The 64 byte
        // context include additional padding at the end of the context.
        //
        // NOTE: The location of all fields referenced by the xHCI driver are in
        //       the same location for both 32 byte and 64 byte contexts, assuming
        //       that they are referenced at the slot/endpoint context level.
        //
        // If the note wasn't true, then this code would not be safe.
        //
        // This code casts to the 64 byte context for both 32 and 64 byte contexts and
        // references sub-contexts based on the context size supported by the controller.
        //
        RtlZeroMemory(UsbDeviceData->InputDeviceContextBuffer->VirtualAddress,
                      UsbDeviceData->InputDeviceContextBuffer->Size);

        inputContext = (PINPUT_CONTEXT64)UsbDeviceData->InputDeviceContextBuffer->VirtualAddress;

        if (REGISTER_CONTEXTSIZE64 == Register_GetContextSize(registerHandle)) {

            slotContext     = UsbDevice_InputContext64GetSlot(inputContext);
            endpointContext = UsbDevice_InputContext64GetDefaultEndpoint(inputContext);

        } else {

            slotContext     = (PSLOT_CONTEXT64)UsbDevice_InputContext32GetSlot((PINPUT_CONTEXT32)inputContext);
            endpointContext = (PENDPOINT_CONTEXT64)UsbDevice_InputContext32GetDefaultEndpoint((PINPUT_CONTEXT32)inputContext);
        }

        //
        // The Add Context flags A0 and A1 (the Add Context flags for
        // the Slot Context and the Endpoint 0 Context) of the Input
        // Control Context shall be set to '1', and all remaining Add
        // Context and Drop Context flags shall all be cleared to '0'.
        //
        inputContext->InputControlContext.AddContextFlags.A0 = 1;
        inputContext->InputControlContext.AddContextFlags.A1 = 1;

        UsbDeviceData->ContextEntriesInUse = inputContext->InputControlContext.AddContextFlags.AsUlong32;

        //
        // System software shall initialize Slot Context and Endpoint
        // Context 0 entries of the Input Context. All other Endpoint
        // Contexts in the Input Context shall be ignored by the xHC
        // during the execution of this command.
        //
        // All fields of the Input Context Slot Context data structure
        // shall define valid values, except for the Slot State and USB
        // Device Address fields which shall be cleared to '0'.
        //

        usbDeviceInfo = &UsbDeviceData->UsbDeviceInfo;

        //
        // See 8.9 Route String Field in the USB 3.0 specification
        //
        routeString = 0;

        //
        // PortPath[0] is stored in RootHubPortNumber.
        // Start at PortPath[1] for RouteString.
        //
        for (i = 1; i < usbDeviceInfo->PortPath.PortPathDepth; i++) {

            ULONG   hubPort;

            hubPort = usbDeviceInfo->PortPath.PortPath[i];

            if (hubPort > 15) {
                hubPort = 15;
            }

            routeString |= (hubPort << ((i - 1) * 4));
        }

        slotContext->RouteString = routeString;

        switch (usbDeviceInfo->DeviceSpeed) {
        case UsbLowSpeed:
            slotContext->Speed = SLOT_SPEED_FLAVOR_LOW_SPEED_DEVICE;
            break;
        case UsbFullSpeed:
            slotContext->Speed = SLOT_SPEED_FLAVOR_FULL_SPEED_DEVICE;
            break;
        case UsbHighSpeed:
            slotContext->Speed = SLOT_SPEED_FLAVOR_HIGH_SPEED_DEVICE;
            break;
        case UsbSuperSpeed:
            slotContext->Speed = SLOT_SPEED_FLAVOR_ENHANCED_SUPER_SPEED_DEVICE;
            break;
        default:
            LogError(Controller_GetRecorderLog(UsbDeviceData->ControllerHandle), UsbDevice,
                     "%d: Invalid Device Speed %d", UsbDeviceData->SlotId,
                     usbDeviceInfo->DeviceSpeed);
            NT_ASSERTMSG("Invalid Device Speed", 0);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        slotContext->ContextEntries     = 1;

        slotContext->RootHubPortNumber  = usbDeviceInfo->PortPath.PortPath[0];

        if (usbDeviceInfo->TtHub != NULL) {

            PUSBDEVICE_DATA ttHubUsbDeviceData;

            NT_ASSERT(usbDeviceInfo->PortPath.TTHubDepth != 0);

            ttHubUsbDeviceData = GetUsbDeviceData(usbDeviceInfo->TtHub);

            NT_ASSERT(ttHubUsbDeviceData->IsHub);

            if (ttHubUsbDeviceData->NumberOfTTs > 1) {
                slotContext->MultiTT = 1;
            }

            slotContext->TTHubSlotId = UsbDevice_GetSlotId(ttHubUsbDeviceData);
        }

        //
        // A TT hub plugged into a root hub port should have a
        // TTHubDepth value of 1.  Then the port number to which the TT
        // hub is attached would be UsbDeviceInfo.PortPath.PortPath[0]
        // and the port number through which this device is attached to
        // the TT hub would be UsbDeviceInfo.PortPath.PortPath[1].  If
        // the value of TTHubDepth is 0 then there is no TT hub for this
        // device.
        //
        if (usbDeviceInfo->PortPath.TTHubDepth != 0) {

            NT_ASSERT(usbDeviceInfo->TtHub != NULL);

            slotContext->TTPortNumber =
                usbDeviceInfo->PortPath.PortPath[usbDeviceInfo->PortPath.TTHubDepth];
        } else {
            slotContext->TTPortNumber = 0;
        }

        //
        // The Endpoint 0 Context data structure in the Input Context
        // shall define valid values for the TR Dequeue Pointer, EP
        // Type, Force Event (FE), Error Count (CErr), and Max Packet
        // Size fields.
        //
        // The MaxPStreams, Max Burst Size, and EP State values shall be
        // cleared to '0'.
        //
        endpointContext->EndpointType = Endpoint_GetType(UsbDeviceData->EndpointHandle[1]);
        endpointContext->MaxPacketSize = Endpoint_GetMaxPacketSize(UsbDeviceData->EndpointHandle[1]);

        endpointContext->ErrorCount = Endpoint_GetErrorCount(UsbDeviceData->EndpointHandle[1]);

        Endpoint_SetMaxPayloadPerServiceOpportunity(
            UsbDeviceData->EndpointHandle[1],
            endpointContext->MaxPacketSize);

        //
        // Initialize the DequeuePointer before the DequeueCycleState as initializing
        // the DequeuePointer overwrites the DequeueCycleState field.
        //
        endpointContext->DequeuePointer = Endpoint_GetDequeuePointer(UsbDeviceData->EndpointHandle[1], 0);

        LogInfo(Controller_GetRecorderLog(UsbDeviceData->ControllerHandle), UsbDevice,
                "%d: UsbDeviceData->InputDeviceContextBuffer 0x%p inputContext 0x%p",
                UsbDeviceData->SlotId, UsbDeviceData->InputDeviceContextBuffer, inputContext);

        //
        // Build and issue the TRB_TYPE_ADDRESS_DEVICE_COMMAND to the
        // controller to transition the device to the Addressed state if
        // the BlockSetAddressRequest argument is FALSE or to transition
        // the device to the Default state if the BlockSetAddressRequest
        // argument is TRUE.
        //
        RtlZeroMemory(&UsbDeviceData->Crb, sizeof(UsbDeviceData->Crb));
        UsbDeviceData->Crb.CommandCallback = UsbDevice_SetAddressCompletion;
        UsbDeviceData->Crb.ObjectHandle = (HANDLE)UsbDeviceData;

        UsbDeviceData->Crb.Trb.AddressDeviceCommand.Type = TRB_TYPE_ADDRESS_DEVICE_COMMAND;
        UsbDeviceData->Crb.Trb.AddressDeviceCommand.BlockSetAddressRequest = BlockSetAddressRequest;
        UsbDeviceData->Crb.Trb.AddressDeviceCommand.SlotId = UsbDeviceData->SlotId;
        UsbDeviceData->Crb.Trb.AddressDeviceCommand.InputContextPointer =
            (ULONG64)UsbDeviceData->InputDeviceContextBuffer->LogicalAddress.QuadPart;
        UsbDeviceData->Crb.InputContextVA = inputContext;

        Command_SendCommand(commandHandle, &UsbDeviceData->Crb);

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_SetAddressCompletion(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This routine is called on a command completion event for a
    TRB_TYPE_ADDRESS_DEVICE_COMMAND which was issued by
    UsbDevice_SetAddress().

    If the TRB_TYPE_ADDRESS_DEVICE_COMMAND completed successfully for
    the usb device then the device is currently in the Default state if
    the Block Set Address Request (BSR) flag was set to '1' or in the
    Addressed state if the Block Set Address Request (BSR) flag was set
    to '0'.

    See section 4.5.3 Slot States and Figure 10: Slot State Diagram in
    the XHCI specification.

Arguments:

    EventTRB - Used for logging Hardware verifier events

--*/
{
    NTSTATUS        status;
    PUSBDEVICE_DATA usbDeviceData;
    WDFREQUEST      wdfRequest;
    BOOLEAN         completeRequest;

    TRY {

        usbDeviceData = (PUSBDEVICE_DATA)Crb->ObjectHandle;

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the address
            // device command effectively failed.
            //
            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p - controller gone - address device failed",
                    usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice);

            if (usbDeviceData->Crb.Trb.AddressDeviceCommand.BlockSetAddressRequest != FALSE) {
                //
                // The Enable Slot / Address Device (BSR=1) sequence is
                // considered an all or nothing atomic operation for the
                // IOCTL_INTERNAL_USB_USBDEVICE_ENABLE request.  If the
                // Enable Slot command was successful but the Address
                // Device command was not then the device slot must be
                // returned to the disabled state.
                //
                // If the controller is gone the device slot is
                // effectively already in the disabled state but its
                // resources must still be freed.
                //

                UsbDevice_SetDeviceDisabled(usbDeviceData);
                completeRequest = FALSE;
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            } 

            if (usbDeviceData->WdfRequestMustSucceed) {
                status = STATUS_SUCCESS;
            } else {
                status = STATUS_UNSUCCESSFUL;
            }
            completeRequest = TRUE;
            LEAVE;
        }

        if (Crb->CompletionCode == TRB_COMPLETION_CODE_SUCCESS) {

            if (usbDeviceData->Crb.Trb.AddressDeviceCommand.BlockSetAddressRequest == FALSE) {

                //
                // If the Block Set Address Reset bit was set to '0'
                // then this was a UCX address device request and the
                // device is now in the Addressed state.  In this case
                // the USB device address needs to be returned in the
                // UCX address device request.
                //
                PUSBDEVICE_ADDRESS      usbDeviceAddress;
                WDF_REQUEST_PARAMETERS  wdfRequestParams;
                PSLOT_CONTEXT64         slotContext;

                WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
                WdfRequestGetParameters(usbDeviceData->WdfRequest, &wdfRequestParams);
                usbDeviceAddress = (PUSBDEVICE_ADDRESS)wdfRequestParams.Parameters.Others.Arg1;

                //
                // The slot context is always the first context in the device context.
                //
                slotContext = (PSLOT_CONTEXT64)usbDeviceData->DeviceContextBuffer->VirtualAddress;

                usbDeviceAddress->Address = slotContext->UsbDeviceAddress;

                LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                        "%d: USB Device 0x%p addressed with USB address 0x%x",
                        Crb->SlotId, usbDeviceData->UcxUsbDevice, usbDeviceAddress->Address);

            } else {

                //
                // If the Block Set Address Reset bit was set to '1'
                // then this was a UCX enable device request and the
                // device is now in the Default state.
                //
                LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                        "%d: USB Device 0x%p addressed", Crb->SlotId, usbDeviceData->UcxUsbDevice);
            }

            status = STATUS_SUCCESS;
            completeRequest = TRUE;

        } else {

            LogError(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                     "%d: USB Device 0x%p BSR %d Failed to address device: %!TrbCompletionCode!",
                     usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice,
                     usbDeviceData->Crb.Trb.AddressDeviceCommand.BlockSetAddressRequest,
                     (TRB_COMPLETION_CODE)Crb->CompletionCode);

            if (usbDeviceData->Crb.Trb.AddressDeviceCommand.BlockSetAddressRequest == FALSE) {

                //
                // The Address Device command failed but the device
                // slot will remain enabled until a subsequent
                // IOCTL_INTERNAL_USB_USBDEVICE_DISABLE request is
                // received.
                //
                status = STATUS_UNSUCCESSFUL;
                completeRequest = TRUE;

            } else {

                HANDLE  commandHandle;

                commandHandle = Controller_GetCommandHandle(usbDeviceData->ControllerHandle);
                NT_ASSERT(commandHandle != NULL);

                if (usbDeviceData->WdfRequestMustSucceed) {
                    
                    Controller_HwVerifierBreakIfEnabled(
                        usbDeviceData->ControllerHandle,
                        usbDeviceData->UcxUsbDevice,
                        NULL,
                        ControllerHwVerifierSetAddressBSR1Failed,
                        "Set Address Command with BSR=1 failed",
                        &Crb->Trb,
                        EventTRB);

                    Controller_ReportFatalError(usbDeviceData->ControllerHandle,
                                                Controller_FatalErrorReset,
                                                USB3_WER_BUGCODE_USBXHCI_SET_ADDRESS_FAILED_BSR1,
                                                NULL,
                                                NULL,
                                                NULL);
                
                } else {
                    //
                    // The Enable Slot / Address Device (BSR=1) sequence is
                    // considered an all or nothing atomic operation for the
                    // IOCTL_INTERNAL_USB_USBDEVICE_ENABLE request.  If the
                    // Enable Slot command was successful but the Address
                    // Device command was not then the device slot must be
                    // returned to the disabled state.
                    //
                    // Refer to section 4.6.4 for a detailed description of the
                    // Disable Slot command.
                    //
                    RtlZeroMemory(&usbDeviceData->Crb, sizeof(usbDeviceData->Crb));
                    usbDeviceData->Crb.CommandCallback = UsbDevice_DisableCompletionReturnFailure;
                    usbDeviceData->Crb.ObjectHandle = (HANDLE)usbDeviceData;
                    usbDeviceData->Crb.Trb.DisableSlotCommand.Type = TRB_TYPE_DISABLE_SLOT_COMMAND;
                    usbDeviceData->Crb.Trb.DisableSlotCommand.SlotId = usbDeviceData->SlotId;

                    Command_SendCommand(commandHandle, &usbDeviceData->Crb);

                }

                status = STATUS_SUCCESS;
                
                //
                // The request will be completed by UsbDevice_DisableCompletionReturnFailure().
                //
                completeRequest = FALSE;
            }
        }

    } FINALLY {

        if (completeRequest) {

            wdfRequest = usbDeviceData->WdfRequest;
            usbDeviceData->WdfRequest = NULL;
            WdfRequestComplete(wdfRequest, status);
        }
    }

    return;
}

VOID
UsbDevice_UcxEvtAddress(
    UCXCONTROLLER   UcxController,
    WDFREQUEST      WdfRequest
    )
/*++

Routine Description:

    This is the event handler called by UCX to address the usb device.

    The device should currently be in the Default state and an attempt
    will be made to transition the device to the Addressed state by
    issuing a TRB_TYPE_ENABLE_SLOT_COMMAND to the controller for the
    device.

    See section 4.5.3 Slot States and Figure 10: Slot State Diagram in
    the XHCI specification.

    See section 4.6.5 Address Device for a detailed description of the
    Address Device command.

Arguments:

    UcxController - The UCX controller object.

    WdfRequest - The WDF request object for the IOCTL_INTERNAL_USB_USBDEVICE_ADDRESS request.

Return Value:

    None directly from this routine.

    The completion status in the WdfRequest, which may be completed asynchronously.

--*/
{
    NTSTATUS                status;
    WDF_REQUEST_PARAMETERS  wdfRequestParams;
    PUSBDEVICE_DATA         usbDeviceData;
    PUSBDEVICE_ADDRESS      usbDeviceAddress;

    UNREFERENCED_PARAMETER(UcxController);

    TRY {

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        usbDeviceAddress = (PUSBDEVICE_ADDRESS)wdfRequestParams.Parameters.Others.Arg1;
        usbDeviceData = GetUsbDeviceData(usbDeviceAddress->UsbDevice);
        NT_ASSERT(usbDeviceData != NULL);

        NT_ASSERT(usbDeviceData->WdfRequest == NULL);

        LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                "%d: USB Device 0x%p set device address requested",
                usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice);

        if (usbDeviceData->SlotEnabled == FALSE) {
            LogWarning(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                       "%d: USB Device 0x%p device slot is disabled",
                       usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice);
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        //
        // If controller is gone, there is a race between this UCX callback processing
        // a request, and UsbDevice_ControllerGone trying to complete outstanding
        // requests. This check here minimizes the window in which that race exists,
        // which is a sufficiently robust low risk fix for Windows 8.
        //
        if (Controller_IsControllerAccessible(usbDeviceData->ControllerHandle) == FALSE) {
            status = STATUS_NO_SUCH_DEVICE;
            LEAVE;
        }

        usbDeviceData->WdfRequest = WdfRequest;
        usbDeviceData->WdfRequestMustSucceed = FALSE;

        //
        // Attempt to transition the device to the Addressed state by
        // issuing a TRB_TYPE_ADDRESS_DEVICE_COMMAND to the controller
        // for the device with the Block Set Address Request (BSR) flag
        // set to '0'.
        //
        status = UsbDevice_SetAddress(usbDeviceData, FALSE);

        if (!NT_SUCCESS(status)) {
            LogWarning(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                       "%d: UsbDevice_SetAddress Failed %!STATUS!", usbDeviceData->SlotId, status);
            LEAVE;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            usbDeviceData->WdfRequest = NULL;
            WdfRequestComplete(WdfRequest, status);
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_UpdateUsbDevice(
    __in
        PUSBDEVICE_DATA     UsbDeviceData,
    __in
        PUSBDEVICE_UPDATE   UsbDeviceUpdate
    )
{
    HANDLE      rootHubHandle;

    TRY {

        if (UsbDeviceUpdate->Flags.UpdateIsHub == 1) {
            if (UsbDeviceUpdate->IsHub) {
                UsbDeviceData->IsHub = TRUE;
            }
        }

        //
        // Save a copy of the USB_DEVICE_DESCRIPTOR in our USBDEVICE_DATA
        // context data.
        //
        if (UsbDeviceUpdate->Flags.UpdateDeviceDescriptor == 1) {

            RtlCopyMemory(&UsbDeviceData->DeviceDescriptor,
                          UsbDeviceUpdate->DeviceDescriptor,
                          sizeof(*UsbDeviceUpdate->DeviceDescriptor));
        }

        //
        // See if we need to update the 2.0 LPM state
        //
        if (UsbDeviceUpdate->Flags.Update20HardwareLpmParameters == 1) {

            if (UsbDeviceUpdate->Usb20HardwareLpmParameters.HardwareLpmEnable) {
                UsbDeviceData->Is20LpmEnabled = TRUE;
            } else {
                UsbDeviceData->Is20LpmEnabled = FALSE;
            }
            rootHubHandle = Controller_GetRootHubHandle(UsbDeviceData->ControllerHandle);

            NT_ASSERT(UsbDeviceData->IsHub == FALSE);

            RootHub_Update20HardwareLpmParameters(rootHubHandle,
                                                  UsbDeviceData->UsbDeviceInfo.PortPath.PortPath[0],
                                                  UsbDeviceUpdate->Usb20HardwareLpmParameters,
                                                  UsbDeviceData->SlotId);
        }

        if (UsbDeviceUpdate->Flags.UpdateRootPortResumeTime) {
            rootHubHandle = Controller_GetRootHubHandle(UsbDeviceData->ControllerHandle);

            RootHub_SetPortResumeTime(rootHubHandle,
                                      UsbDeviceData->UsbDeviceInfo.PortPath.PortPath[0],
                                      UsbDeviceUpdate->RootPortResumeTime);

            UsbDeviceData->IsRootPortResumeTimeSet = TRUE;
        }

    } FINALLY {

        Etw_DeviceUpdate(NULL, UsbDeviceData);

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_UpdateCompletion(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This routine is called on a command completion event for a
    TRB_TYPE_EVALUATE_CONTEXT_COMMAND which was issued by
    UsbDevice_UcxEvtUpdate().

Arguments:

    EventTRB - To be used for logging Hardware verifier events

--*/
{
    NTSTATUS                status;
    PUSBDEVICE_DATA         usbDeviceData;
    PUSBDEVICE_UPDATE       usbDeviceUpdate;
    WDFREQUEST              wdfRequest;
    WDF_REQUEST_PARAMETERS  wdfRequestParams;

    UNREFERENCED_PARAMETER(EventTRB);

    TRY {

        usbDeviceData = (PUSBDEVICE_DATA)Crb->ObjectHandle;

        wdfRequest = usbDeviceData->WdfRequest;

        //
        // Retrieve the USBDEVICE_UPDATE pointer from the
        // IOCTL_INTERNAL_USB_USBDEVICE_UPDATE WdfRequest.
        //
        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(wdfRequest, &wdfRequestParams);
        usbDeviceUpdate = (PUSBDEVICE_UPDATE)wdfRequestParams.Parameters.Others.Arg1;

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p - controller gone",
                    usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice);

            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        if (Crb->CompletionCode == TRB_COMPLETION_CODE_SUCCESS) {

            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p MaxExitLatency updated to 0x%x",
                    Crb->SlotId, usbDeviceData->UcxUsbDevice, usbDeviceUpdate->MaxExitLatency);

            //
            // The MaxExitLatency was successfully updated. Update any other flags that may
            // have been specified on the USB device update request.
            //
            UsbDevice_UpdateUsbDevice(usbDeviceData, usbDeviceUpdate);

            status = STATUS_SUCCESS;

        } else {

            LogWarning(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                       "%d: USB Device 0x%p failed to update MaxExitLatency to 0x%x %!TrbCompletionCode!",
                       Crb->SlotId, usbDeviceData->UcxUsbDevice, usbDeviceUpdate->MaxExitLatency,
                       (TRB_COMPLETION_CODE)Crb->CompletionCode);

            if (Crb->CompletionCode == TRB_COMPLETION_CODE_MAX_EXIT_LATENCY_TOO_LARGE_ERROR) {
                usbDeviceUpdate->FailureFlags.MaxExitLatencyTooLarge = 1;
            }

            status = STATUS_UNSUCCESSFUL;
        }

    } FINALLY {

        usbDeviceData->WdfRequest = NULL;
        WdfRequestComplete(wdfRequest, status);
    }

    return;
}

VOID
UsbDevice_UcxEvtUpdate(
    UCXCONTROLLER   UcxController,
    WDFREQUEST      WdfRequest
    )
/*++

Routine Description:

Arguments:

    UcxController - The UCX controller object.

    WdfRequest - The WDF request object for the IOCTL_INTERNAL_USB_USBDEVICE_UPDATE request.

Return Value:

    None directly from this routine.

    The completion status in the WdfRequest.

--*/
{
    NTSTATUS                status;
    BOOLEAN                 completeRequest;
    WDF_REQUEST_PARAMETERS  wdfRequestParams;
    PUSBDEVICE_UPDATE       usbDeviceUpdate;
    PUSBDEVICE_DATA         usbDeviceData;
    HANDLE                  commandHandle;
    HANDLE                  registerHandle;
    PINPUT_CONTEXT64        inputContext;
    PSLOT_CONTEXT64         slotContext;

    UNREFERENCED_PARAMETER(UcxController);

    TRY {

        status = STATUS_SUCCESS;
        completeRequest = TRUE;

        //
        // Retrieve the USBDEVICE_UPDATE pointer from the
        // IOCTL_INTERNAL_USB_USBDEVICE_UPDATE WdfRequest.
        //
        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);
        usbDeviceUpdate = (PUSBDEVICE_UPDATE)wdfRequestParams.Parameters.Others.Arg1;

        //
        // Retrieve the USBDEVICE_DATA pointer from the UCXUSBDEVICE in the
        // USBDEVICE_MGMT_HEADER of the USBDEVICE_UPDATE.
        //
        usbDeviceData = GetUsbDeviceData(usbDeviceUpdate->UsbDevice);

        commandHandle = Controller_GetCommandHandle(usbDeviceData->ControllerHandle);
        registerHandle = Controller_GetRegisterHandle(usbDeviceData->ControllerHandle);

        //
        // The Flag.UpdateMaxExitLatency requires an evaluate context command sent to the
        // xHCI controller in order to update the max exit latency value in the slot context.
        //
        // This flag is update first and then subsequent flags are handled on successful completion
        // of the evaluate context command. If this flag is not specified, the other flags are
        // handled immediately and the request completed within this function.
        //
        if (usbDeviceUpdate->Flags.UpdateMaxExitLatency == 1) {

            //
            // Initialize the Input Device Context.
            //
            RtlZeroMemory(usbDeviceData->InputDeviceContextBuffer->VirtualAddress,
                          usbDeviceData->InputDeviceContextBuffer->Size);

            inputContext = (PINPUT_CONTEXT64)usbDeviceData->InputDeviceContextBuffer->VirtualAddress;

            if (REGISTER_CONTEXTSIZE64 == Register_GetContextSize(registerHandle)) {

                slotContext = UsbDevice_InputContext64GetSlot(inputContext);

            } else {

                PINPUT_CONTEXT32    inputContext32 = (PINPUT_CONTEXT32)inputContext;

                slotContext = (PSLOT_CONTEXT64)UsbDevice_InputContext32GetSlot(inputContext32);
            }

            //
            // Specify that the slot context is to be updated.
            //
            inputContext->InputControlContext.AddContextFlags.A0 = 1;

            //
            // Only the Interrupter Target and Max Exit Latency fields are updated on an
            // Evaulate Context Command, from section 6.2.2.3 in xHCI rev 1.0.
            //
            slotContext->MaxExitLatency = usbDeviceUpdate->MaxExitLatency;
            slotContext->InterrupterTarget = 0;

            //
            // Initialize and send the CRB for the evaluate context command.
            //
            RtlZeroMemory(&usbDeviceData->Crb, sizeof(usbDeviceData->Crb));
            usbDeviceData->Crb.CommandCallback = UsbDevice_UpdateCompletion;
            usbDeviceData->Crb.ObjectHandle = (HANDLE)usbDeviceData;

            usbDeviceData->Crb.Trb.ConfigureEndpointCommand.Type = TRB_TYPE_EVALUATE_CONTEXT_COMMAND;
            usbDeviceData->Crb.Trb.ConfigureEndpointCommand.SlotId = usbDeviceData->SlotId;
            usbDeviceData->Crb.Trb.ConfigureEndpointCommand.InputContextPointer =
                (ULONG64)usbDeviceData->InputDeviceContextBuffer->LogicalAddress.QuadPart;
            usbDeviceData->Crb.InputContextVA = inputContext;

            //
            // If controller is gone, there is a race between this UCX callback processing
            // a request, and UsbDevice_ControllerGone trying to complete outstanding
            // requests. This check here minimizes the window in which that race exists,
            // which is a sufficiently robust low risk fix for Windows 8.
            //
            if (Controller_IsControllerAccessible(usbDeviceData->ControllerHandle) == FALSE) {
                status = STATUS_NO_SUCH_DEVICE;
                LEAVE;
            }

            //
            // Store the IOCTL_INTERNAL_USB_USBDEVICE_UPDATE WdfRequest in
            // the USBDEVICE_DATA. In the successful case this WdfRequest
            // will be asynchronously completed by completion routine.
            //
            usbDeviceData->WdfRequest = WdfRequest;
            usbDeviceData->WdfRequestMustSucceed = FALSE;

            Command_SendCommand(commandHandle, &usbDeviceData->Crb);

            //
            // The command was successfully sent, do not complete the request in
            // this function, instead it is completed in the command completion callback.
            //
            completeRequest = FALSE;

        } else {

            //
            // All flags, excluding the update of the max exit latency, always succeed.
            //
            UsbDevice_UpdateUsbDevice(usbDeviceData, usbDeviceUpdate);
        }

    } FINALLY {

        if (completeRequest) {
            usbDeviceData->WdfRequest = NULL;
            WdfRequestComplete(WdfRequest, status);
        }
    }

    return;
}

VOID
UsbDevice_UcxEvtHubInfo(
    UCXCONTROLLER   UcxController,
    WDFREQUEST      WdfRequest
    )
/*++

Routine Description:

Arguments:

    UcxController - The UCX controller object.

    WdfRequest - The WDF request object for the IOCTL_INTERNAL_USB_USBDEVICE_HUB_INFO request.

Return Value:

    None directly from this routine.

    The completion status in the WdfRequest.

--*/
{
    WDF_REQUEST_PARAMETERS  wdfRequestParams;
    PUSBDEVICE_HUB_INFO     hubInfo;
    PUSBDEVICE_DATA         usbDeviceData;

    UNREFERENCED_PARAMETER(UcxController);

    //
    // Retrieve the USBDEVICE_HUB_INFO pointer from the
    // IOCTL_INTERNAL_USB_USBDEVICE_HUB_INFO WdfRequest.
    //
    WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
    WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

    hubInfo = (PUSBDEVICE_HUB_INFO)wdfRequestParams.Parameters.Others.Arg1;

    //
    // Retrieve the USBDEVICE_DATA pointer from the UCXUSBDEVICE in the
    // USBDEVICE_MGMT_HEADER of the USBDEVICE_HUB_INFO.
    //
    usbDeviceData = GetUsbDeviceData(hubInfo->UsbDevice);

    NT_ASSERT(usbDeviceData->IsHub);

    //
    // Copy the hub information passed down in the USBDEVICE_HUB_INFO
    // into the USBDEVICE_DATA.
    //
    usbDeviceData->NumberOfPorts = hubInfo->NumberOfPorts;
    usbDeviceData->NumberOfTTs   = hubInfo->NumberOfTTs;
    usbDeviceData->TTThinkTime   = hubInfo->TTThinkTime;

    Etw_DeviceUpdate(NULL, usbDeviceData);

    WdfRequestComplete(WdfRequest, STATUS_SUCCESS);

    return;
}

VOID
UsbDevice_InitializeEndpointContext(
    PUSBDEVICE_DATA         UsbDeviceData,
    HANDLE                  Endpoint,
    PENDPOINT_CONTEXT64     EndpointContext
    )
/*++

Routine Description:

    This routine initializes an Input Endpoint Context for a Configure
    Endpoint command.

    On return the Input Endpoint Context should be initialized such
    that all of the following are true:

    (1) The values of the Max Packet Size, Max Burst Size, and the
    Interval are considered within range for endpoint type and the speed
    of the device

    (2) If MaxPStreams > 0, then the TR Dequeue Pointer field points to
    an array of valid Stream Contexts, or if MaxPStreams = 0, then the
    TR Dequeue Pointer field points to a Transfer Ring,

    (3) Ehe EP State field = Disabled

    (4) All other fields are within their valid range of values.


Arguments:

    UsbDeviceData - The device on which the endpoint is being
    configured.

    Endpoint - The endpoint which is being configured.

    EndpointContext - The Input Endpoint Context which which is
    initialized for the device endpoint.

Return Value:

    None

--*/
{
    UCHAR   bInterval;
    ULONG   maxEndpointServiceTimeIntervalPayload = 0;

    EndpointContext->EndpointType = Endpoint_GetType(Endpoint);
    EndpointContext->MaxPacketSize = Endpoint_GetMaxPacketSize(Endpoint);
    EndpointContext->ErrorCount = Endpoint_GetErrorCount(Endpoint);

    bInterval = Endpoint_GetInterval(Endpoint);

    if (UsbDeviceData->UsbDeviceInfo.DeviceSpeed == UsbLowSpeed) {

        if ((EndpointContext->EndpointType == EndpointType_InterruptIn) ||
            (EndpointContext->EndpointType == EndpointType_InterruptOut)) {

            //
            // Low Speed Interrupt:
            // bInterval    Period in 1ms frames            XHCI Interval
            // 0-15         8                               6
            // 16-31        16                              7
            // 32-255       32                              8
            //
            if (bInterval < 16) {
                EndpointContext->Interval = 6;
            } else if (bInterval < 32) {
                EndpointContext->Interval = 7;
            } else {
                EndpointContext->Interval = 8;
            }

            maxEndpointServiceTimeIntervalPayload = EndpointContext->MaxPacketSize;
            EndpointContext->MaxEndpointServiceTimeIntervalPayload = maxEndpointServiceTimeIntervalPayload;
            EndpointContext->MaxEndpointServiceTimeIntervalPayloadHi = 0;
        }

        Endpoint_SetMaxPayloadPerServiceOpportunity(
            Endpoint,
            EndpointContext->MaxPacketSize);

    } else if (UsbDeviceData->UsbDeviceInfo.DeviceSpeed == UsbFullSpeed) {

        if ((EndpointContext->EndpointType == EndpointType_InterruptIn) ||
            (EndpointContext->EndpointType == EndpointType_InterruptOut)) {

            //
            // Full Speed Interrupt:
            // bInterval    Period in 1ms frames            XHCI Interval
            // 0-1          1                               3
            // 2-3          2                               4
            // 4-7          4                               5
            // 8-15         8                               6
            // 16-31        16                              7
            // 32-255       32                              8
            //
            if (bInterval < 2) {
                EndpointContext->Interval = 3;
            } else if (bInterval < 4) {
                EndpointContext->Interval = 4;
            } else if (bInterval < 8) {
                EndpointContext->Interval = 5;
            } else if (bInterval < 16) {
                EndpointContext->Interval = 6;
            } else if (bInterval < 32) {
                EndpointContext->Interval = 7;
            } else {
                EndpointContext->Interval = 8;
            }

            maxEndpointServiceTimeIntervalPayload = EndpointContext->MaxPacketSize;
            EndpointContext->MaxEndpointServiceTimeIntervalPayload = maxEndpointServiceTimeIntervalPayload;
            EndpointContext->MaxEndpointServiceTimeIntervalPayloadHi = 0;

        } else if ((EndpointContext->EndpointType == EndpointType_IsochIn) ||
                   (EndpointContext->EndpointType == EndpointType_IsochOut)) {

            //
            // Full Speed Isochronous:
            // bInterval    Period in 1ms frames            XHCI Interval
            // 0-255        1                               3
            //
            EndpointContext->Interval = 3;

            maxEndpointServiceTimeIntervalPayload = EndpointContext->MaxPacketSize;
            EndpointContext->MaxEndpointServiceTimeIntervalPayload = maxEndpointServiceTimeIntervalPayload;
            EndpointContext->MaxEndpointServiceTimeIntervalPayloadHi = 0;

        }

        Endpoint_SetMaxPayloadPerServiceOpportunity(
            Endpoint,
            EndpointContext->MaxPacketSize);

    } else if (UsbDeviceData->UsbDeviceInfo.DeviceSpeed == UsbHighSpeed) {

        if ((EndpointContext->EndpointType == EndpointType_InterruptIn)  ||
            (EndpointContext->EndpointType == EndpointType_InterruptOut) ||
            (EndpointContext->EndpointType == EndpointType_IsochIn)      ||
            (EndpointContext->EndpointType == EndpointType_IsochOut)) {

            //
            // High Speed Interrupt and Isochronous:
            // bInterval    Period in 125ms microframes     XHCI Interval
            // 0-1          1                               0
            // 2            2                               1
            // 3            4                               2
            // 4            8                               3
            // 5            16                              4
            // 6-255        32                              5
            //
            if (bInterval < 2) {
                EndpointContext->Interval = 0;
            } else if (bInterval < 6) {
                EndpointContext->Interval = bInterval - 1;
            } else {
                EndpointContext->Interval = 5;
            }

            //
            // 6.2.3.4 Max Burst Size
            //
            // For High-Speed isochronous and interrupt endpoints the
            // MaxBurstSize field shall be set to the number of
            // additional transaction opportunities per microframe, i.e.
            // the value defined in bits 12:11 of the USB2 Endpoint
            // Descriptor wMaxPacketSize field.
            //
            EndpointContext->MaxBurstSize = Endpoint_GetTransactionsPerMicroframe(Endpoint);

            //
            // 4.14.2 Periodic Transfer Ring Scheduling
            //
            // Software shall define the maximum periodic payload per
            // ESIT as follows for USB2 periodic endpoints:
            //
            // Max ESIT Payload in Bytes = Max Packet Size * (Max Burst Size+1).
            //
            maxEndpointServiceTimeIntervalPayload = EndpointContext->MaxPacketSize * (EndpointContext->MaxBurstSize + 1);
            EndpointContext->MaxEndpointServiceTimeIntervalPayload = maxEndpointServiceTimeIntervalPayload;
            EndpointContext->MaxEndpointServiceTimeIntervalPayloadHi = 0;
        }

        Endpoint_SetMaxPayloadPerServiceOpportunity(
            Endpoint,
            (EndpointContext->MaxPacketSize * (EndpointContext->MaxBurstSize + 1)));

    } else if (UsbDeviceData->UsbDeviceInfo.DeviceSpeed == UsbSuperSpeed) {

        if ((EndpointContext->EndpointType == EndpointType_InterruptIn)  ||
            (EndpointContext->EndpointType == EndpointType_InterruptOut) ||
            (EndpointContext->EndpointType == EndpointType_IsochIn)      ||
            (EndpointContext->EndpointType == EndpointType_IsochOut)) {

            //
            // Super Speed Interrupt and Isochronous:
            // bInterval    Period in 125ms microframes     XHCI Interval
            // 0-1          1                               0
            // 2            2                               1
            // 3            4                               2
            // 4            8                               3
            // 5            16                              4
            // 6-255        32                              5
            //
            if (bInterval < 2) {
                EndpointContext->Interval = 0;
            } else if (bInterval < 6) {
                EndpointContext->Interval = bInterval - 1;
            } else {
                EndpointContext->Interval = 5;
            }

            //
            // 4.14.2 Periodic Transfer Ring Scheduling
            //
            // Software shall define the maximum periodic payload per ESIT as follows
            // for SS/SSP periodic endpoints:
            //
            // Max ESIT Payload in Bytes = SuperSpeed Endpoint Companion Descriptor:wBytesPerInterval
            // or SuperSpeedPlus Isochronous Endpoint Companion Descriptor:dwBytesPerInterval.
            //
            if (Endpoint_IsochEndpointSSP(Endpoint)) {
                maxEndpointServiceTimeIntervalPayload = Endpoint_GetBytesPerIntervalForSSP(Endpoint);
                EndpointContext->Mult = 0;
            } else {
                maxEndpointServiceTimeIntervalPayload = Endpoint_GetBytesPerIntervalForSS(Endpoint);
                EndpointContext->Mult = Endpoint_GetMultForSS(Endpoint);
            }

            EndpointContext->MaxEndpointServiceTimeIntervalPayload = maxEndpointServiceTimeIntervalPayload & 0xFFFFu;
            EndpointContext->MaxEndpointServiceTimeIntervalPayloadHi = (maxEndpointServiceTimeIntervalPayload >> 16) & 0xFFu;

        }

        if ((EndpointContext->EndpointType == EndpointType_IsochIn) ||
            (EndpointContext->EndpointType == EndpointType_IsochOut)) {

            Endpoint_SetMaxPayloadPerServiceOpportunity(
                Endpoint,
                maxEndpointServiceTimeIntervalPayload);

        } else {

            Endpoint_SetMaxPayloadPerServiceOpportunity(
                Endpoint,
                (EndpointContext->MaxPacketSize * (EndpointContext->MaxBurstSize + 1)));
        }

        //
        // 6.2.3.4 Max Burst Size
        //
        // For SuperSpeed endpoints this field shall be set to the value
        // defined in the bMaxBurst field of the SuperSpeed Endpoint
        // Companion Descriptor. Refer to section 8.6.8 of the USB3
        // Specification.
        //
        EndpointContext->MaxBurstSize = Endpoint_GetMaxBurst(Endpoint);
    }

    //
    // Limit the Interval field value of the Endpoint Context to a
    // maximum value of 7 (16ms period) if necessary due to controller
    // errata.
    //
    if ((EndpointContext->Interval > 7) &&
        Controller_GetDeviceFlags(UsbDeviceData->ControllerHandle).MaximumEndpointIntervalValue7) {
        EndpointContext->Interval = 7;
    }

    Endpoint_SetBurstSize(Endpoint, EndpointContext->MaxBurstSize);

    //
    // 4.14.1.1 System Bus Bandwidth Scheduling
    //
    // The Average TRB Length field is computed by dividing the average
    // TD Transfer Size by the average number of TRBs that are used to
    // describe a TD, including Link, No Op, and Event Data TRBs.
    //
    // For lack of a better metric use the Max ESIT Payload as the value
    // of the Average TRB Length, assuming that an Interrupt transfer is
    // likely to be no larger than the amount of data transfered in any
    // service interval.  Then divide that value by 2 as there will both
    // a Transfer TRB and an Event TRB for each transfer.
    //
    EndpointContext->AverageTrbLength = maxEndpointServiceTimeIntervalPayload / 2;

    //
    // If the value of this field is '0', then the TR Dequeue Pointer
    // field shall point to a Transfer Ring. If this field is > '0' then
    // the TR Dequeue Pointer field shall point to a Primary Stream
    // Context Array. Refer to section 4.12 for more information.
    //
    // A value of '0' indicates that Streams are not supported by this
    // endpoint and the Endpoint Context TR Dequeue Pointer field
    // references a Transfer Ring.
    //
    // A value of '1' to '15' indicates that the Primary Stream ID Width
    // is (MaxPstreams+1) and the Primary Stream Array contains
    // 2**(MaxPStreams+1) entries.
    //
    EndpointContext->MaxPrimaryStreams = Endpoint_GetMaxPrimaryStreams(Endpoint);

    //
    // Initialize the DequeuePointer before the DequeueCycleState as
    // initializing the DequeuePointer overwrites the DequeueCycleState
    // field.
    //
    EndpointContext->DequeuePointer = Endpoint_GetDequeuePointer(Endpoint, 0);

    if (EndpointContext->MaxPrimaryStreams == 0) {
        EndpointContext->LinearStreamArray = 0;
    } else {
        EndpointContext->LinearStreamArray = 1;
    }

    return;
}

VOID
UsbDevice_UcxEvtEndpointsConfigure(
    UCXCONTROLLER   UcxController,
    WDFREQUEST      WdfRequest
    )
/*++

Routine Description:

    This function configures the endpoints for a usb device.

Arguments:

    UcxController - The UCX controller object.

    WdfRequest - The WDF request object for the IOCTL_INTERNAL_USB_ENDPOINTS_CONFIGURE request.

Return Value:

    None directly from this routine.

    The completion status in the WdfRequest, which may be completed asynchronously.

--*/
{
    WDF_REQUEST_PARAMETERS  wdfRequestParams;
    PUSBDEVICE_ENDPOINTS_CONFIGURE_DATA
                            endpointsConfigureData;
    PENDPOINTS_CONFIGURE    endpointsConfigure;
    PUSBDEVICE_DATA         usbDeviceData;
    HANDLE                  registerHandle;
    HANDLE                  commandHandle;
    PINPUT_CONTEXT64        inputContext;
    PSLOT_CONTEXT64         slotContext;
    PENDPOINT_CONTEXT64     endpointContext;
    UCXENDPOINT             ucxEndpoint;
    HANDLE                  endpoint;
    ULONG                   deviceContextIndex;
    ULONG                   i;
    BOOLEAN                 endpointEnableFailure;
    NTSTATUS                status;
    BOOLEAN                 completeRequest;

    UNREFERENCED_PARAMETER(UcxController);

    TRY {

        completeRequest = TRUE;

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        //
        // Retrieve a pointer to our private context data structure for
        // this WdfRequest.
        //
        endpointsConfigureData = &(GetRequestData(WdfRequest)->UsbDeviceEndpointsConfigureData);
        endpointsConfigureData->EndpointEnableFailed = FALSE;
        endpointsConfigureData->ConfigureEndpointCommandFailed = FALSE;

        endpointsConfigure = (PENDPOINTS_CONFIGURE)wdfRequestParams.Parameters.Others.Arg1;
        usbDeviceData = GetUsbDeviceData(endpointsConfigure->UsbDevice);
        NT_ASSERT(usbDeviceData != NULL);

        registerHandle = Controller_GetRegisterHandle(usbDeviceData->ControllerHandle);
        NT_ASSERT(registerHandle != NULL);

        commandHandle = Controller_GetCommandHandle(usbDeviceData->ControllerHandle);
        NT_ASSERT(commandHandle != NULL);

        NT_ASSERT(usbDeviceData->WdfRequest == NULL);

        LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                "%d: EndpointsToEnableCount %d EndpointsToDisableCount %d EndpointsEnabledAndUnchangedCount %d",
                usbDeviceData->SlotId,
                endpointsConfigure->EndpointsToEnableCount,
                endpointsConfigure->EndpointsToDisableCount,
                endpointsConfigure->EndpointsEnabledAndUnchangedCount);

        if (usbDeviceData->SlotEnabled == FALSE) {

            LogWarning(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                       "%d: USB Device 0x%p device slot is disabled",
                       usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice);

            if (endpointsConfigure->EndpointsToEnableCount > 0) {

                //
                // Cannot enable endpoints if the device slot is
                // disabled.  Complete the request with failure.
                //
                status = STATUS_UNSUCCESSFUL;
                LEAVE;

            } else {

                //
                // No need to disable endpoints in the host controller
                // if the device slot is disabled.  Complete the request
                // with success.
                //
                status = STATUS_SUCCESS;
                LEAVE;
            }
        }

        //
        // If controller is gone, there is a race between this UCX callback processing
        // a request, and UsbDevice_ControllerGone trying to complete outstanding
        // requests. This check here minimizes the window in which that race exists,
        // which is a sufficiently robust low risk fix for Windows 8.
        //
        // Do what UsbDevice_EndpointsConfigureCompletion would do in controller gone
        // case.
        //
        if (Controller_IsControllerAccessible(usbDeviceData->ControllerHandle) == FALSE) {

            if (endpointsConfigure->EndpointsToEnableCount > 0) {
                status = STATUS_NO_SUCH_DEVICE;
            } else {
                status = STATUS_SUCCESS;
            }

            for (i = 0; i < endpointsConfigure->EndpointsToDisableCount; i++) {

                endpoint = Endpoint_GetEndpointHandle(endpointsConfigure->EndpointsToDisable[i]);

                Endpoint_Disable(endpoint, FALSE);
            }

            LEAVE;
        }

        usbDeviceData->WdfRequest = WdfRequest;
        usbDeviceData->WdfRequestMustSucceed = (endpointsConfigure->EndpointsToEnableCount == 0) ? TRUE : FALSE;

        //
        // Initialize the Input Device Context.
        //
        // See section 6.2.5 Input Context in the XHCI specification.
        // See also section 4.6.6 Configure Endpoint
        //
        RtlZeroMemory(usbDeviceData->InputDeviceContextBuffer->VirtualAddress,
                      usbDeviceData->InputDeviceContextBuffer->Size);

        inputContext = (PINPUT_CONTEXT64)usbDeviceData->InputDeviceContextBuffer->VirtualAddress;
        NT_ASSERT(inputContext != NULL);

        if (REGISTER_CONTEXTSIZE64 == Register_GetContextSize(registerHandle)) {

            slotContext     = UsbDevice_InputContext64GetSlot(inputContext);

        } else {

            PINPUT_CONTEXT32    inputContext32 = (PINPUT_CONTEXT32)inputContext;

            slotContext     = (PSLOT_CONTEXT64)UsbDevice_InputContext32GetSlot(inputContext32);
        }

        //
        // The Add Context flag A1 and Drop Context flags D0 and D1 of
        // the Input Control Context (in the Input Context) shall be
        // cleared to '0'. Endpoint 0 Context does not apply to the
        // Configure Endpoint Command and shall be ignored by the xHC.
        // A0 shall be set to '1' and refer to section 6.2.2.2 for the
        // Slot Context fields used by the Configure Endpoint Command.
        //
        inputContext->InputControlContext.AddContextFlags.A0 = 1;

        //
        // 6.2.2.2 Configure Endpoint Command Usage
        //
        // The Hub field shall also be initialized. If Hub = '1' and
        // Speed = High-Speed ('3'), then the TT Think Time and Multi-
        // TT (MTT) fields shall be initialized. Refer to Table 52 and
        // Table 53 for the specific initialization values of these
        // fields. If Hub = '1', then the Number of Ports field shall be
        // initialized, else Number of Ports = '0'.
        //
        if (usbDeviceData->IsHub) {

            slotContext->Hub = 1;
            slotContext->NumberOfPorts = usbDeviceData->NumberOfPorts;

            if (usbDeviceData->UsbDeviceInfo.DeviceSpeed == UsbHighSpeed) {

                if (usbDeviceData->NumberOfTTs > 1) {
                    slotContext->MultiTT = 1;
                }

                slotContext->TTThinkTime = usbDeviceData->TTThinkTime;
            }
        }

        //
        // Notify the endpoint object to enable each of the endpoints
        // in the EndpointsToEnable[] array.
        //
        endpointEnableFailure = FALSE;
        status = STATUS_SUCCESS;

        for (i = 0; i < endpointsConfigure->EndpointsToEnableCount; i++) {

            ucxEndpoint = endpointsConfigure->EndpointsToEnable[i];
            endpoint = Endpoint_GetEndpointHandle(ucxEndpoint);

            //
            // Notify the endpoint object to enable the endpoint.
            //
            status = Endpoint_Enable(endpoint);

            if (!NT_SUCCESS(status)) {

                LogError(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                         "%d.%d: Endpoint_Enable Failed %!STATUS!",
                         usbDeviceData->SlotId, Endpoint_GetContextIndex(endpoint), status);

                //
                // If enabling one of the EndpointsToEnable[] failed then
                // disable all of the EndpointsToEnable[] which were
                // successfully enabled.
                //
                while (i > 0) {

                    i--;
                    ucxEndpoint = endpointsConfigure->EndpointsToEnable[i];
                    endpoint = Endpoint_GetEndpointHandle(ucxEndpoint);

                    //
                    // Notify the endpoint object to disable the endpoint.
                    //
                    Endpoint_Disable(endpoint, FALSE);
                }

                endpointEnableFailure = TRUE;
                break;
            }
        }

        if (endpointEnableFailure == TRUE) {

            if (endpointsConfigure->EndpointsToDisableCount == 0) {

                //
                // If the EndpointsToDisable[] array is empty then
                // there is no need to issue the Configure Endpoint
                // Command since no endpoints will be either enabled or
                // disabled at this point.
                //
                LEAVE;

            } else {

                //
                // TRUE if and only if the EndpointsToEnableCount is
                // non-zero and an Endpoint_Enable() call failed prior
                // to issuing the Configure Endpoint Command command,
                // and the EndpointsToDisableCount is non-zero.  This
                // will result in a issuing the first and only Configure
                // Endpoint Command command to disable the endpoints in
                // the EndpointsToDisable[] array.
                //
                endpointsConfigureData->EndpointEnableFailed = TRUE;

                LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                        "%d: Request 0x%p for USB Device 0x%p EndpointEnableFailed",
                        usbDeviceData->SlotId, WdfRequest, usbDeviceData->UcxUsbDevice);
            }

        } else {

            //
            // 6.2.3.2 Configure Endpoint Command Usage
            //
            // The Configure Endpoint Command does not reference the Input
            // or Output Endpoint 0 Context (DCI = 1).
            //
            // Any other Endpoint Context (DCI = 2-31) may be referenced by
            // the Configure Endpoint Command.  An Input Endpoint Context is
            // considered "valid" by the Configure Endpoint Command if the
            // Add Context flag is '1' and:
            //
            // (1) The values of the Max Packet Size, Max Burst Size, and the
            // Interval are considered within range for endpoint type and
            // the speed of the device
            //
            // (2) If MaxPStreams > 0, then the TR Dequeue Pointer field
            // points to an array of valid Stream Contexts, or if
            // MaxPStreams = 0, then the TR Dequeue Pointer field points to
            // a Transfer Ring,
            //
            // (3) Ehe EP State field = Disabled
            // within their valid range of values.
            //
            //
            // (4) All other fields are within their valid range of values.
            //
            // Items (1) - (4) are initialized by UsbDevice_InitializeEndpointContext().
            //
            for (i = 0; i < endpointsConfigure->EndpointsToEnableCount; i++) {

                ucxEndpoint = endpointsConfigure->EndpointsToEnable[i];
                endpoint = Endpoint_GetEndpointHandle(ucxEndpoint);

                deviceContextIndex = Endpoint_GetContextIndex(endpoint);

                if (REGISTER_CONTEXTSIZE64 == Register_GetContextSize(registerHandle)) {

                    endpointContext = UsbDevice_InputContext64GetEndpoint(
                        inputContext,
                        deviceContextIndex);

                } else {

                    endpointContext = (PENDPOINT_CONTEXT64)UsbDevice_InputContext32GetEndpoint(
                        (PINPUT_CONTEXT32)inputContext,
                        deviceContextIndex);
                }

                //
                // Initialize the Input Endpoint Context.
                //
                UsbDevice_InitializeEndpointContext(usbDeviceData, endpoint, endpointContext);

                //
                // Set the Add Context flag for this endpoint.
                //
                inputContext->InputControlContext.AddContextFlags.AsUlong32 |=
                    (1 << deviceContextIndex);

                LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                        "%d.%d: Enable Endpoint Type %d MaxPacketSize %4d Interval %d",
                        usbDeviceData->SlotId, deviceContextIndex,
                        endpointContext->EndpointType,
                        endpointContext->MaxPacketSize,
                        endpointContext->Interval);
            }
        }

        for (i = 0; i < endpointsConfigure->EndpointsToDisableCount; i++) {

            ucxEndpoint = endpointsConfigure->EndpointsToDisable[i];
            endpoint = Endpoint_GetEndpointHandle(ucxEndpoint);
            deviceContextIndex = Endpoint_GetContextIndex(endpoint);

            if (REGISTER_CONTEXTSIZE64 == Register_GetContextSize(registerHandle)) {

                endpointContext = UsbDevice_DeviceContext64GetEndpoint(
                    (PDEVICE_CONTEXT64)usbDeviceData->DeviceContextBuffer->VirtualAddress,
                    deviceContextIndex);

            } else {

                endpointContext = (PENDPOINT_CONTEXT64)UsbDevice_DeviceContext32GetEndpoint(
                    (PDEVICE_CONTEXT32)usbDeviceData->DeviceContextBuffer->VirtualAddress,
                    deviceContextIndex);
            }

            //
            // Set the Drop Context flag for this endpoint.
            //
            inputContext->InputControlContext.DropContextFlags.AsUlong32 |=
                (1 << deviceContextIndex);

            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d.%d: Disable Endpoint Type %d MaxPacketSize %4d Interval %d State %d",
                    usbDeviceData->SlotId, deviceContextIndex,
                    endpointContext->EndpointType,
                    endpointContext->MaxPacketSize,
                    endpointContext->Interval,
                    endpointContext->EndpointState);
        }

        //
        // Disable it for now.
        //
#if 0
        if (Register_GetConfigurationInformation(registerHandle)) {
            inputContext->InputControlContext.ConfigurationValue = endpointsConfigure->ConfigurationValue;
            inputContext->InputControlContext.AlternateSetting = endpointsConfigure->AlternateSetting;
            inputContext->InputControlContext.InterfaceNumber = endpointsConfigure->InterfaceNumber;
            usbDeviceData->ConfigurationValue = endpointsConfigure->ConfigurationValue;
            usbDeviceData->AlternateSetting = endpointsConfigure->AlternateSetting;
            usbDeviceData->InterfaceNumber = endpointsConfigure->InterfaceNumber;
        }
#endif

        //
        // 6.2.2.2 Configure Endpoint Command Usage
        //
        // A 'valid' Input Slot Context for a Configure Endpoint Command
        // requires the Context Entries field to be initialized to the
        // index of the last valid Endpoint Context that is defined by
        // the target configuration.
        //
        usbDeviceData->ContextEntriesInUse &= ~inputContext->InputControlContext.DropContextFlags.AsUlong32;
        usbDeviceData->ContextEntriesInUse |=  inputContext->InputControlContext.AddContextFlags.AsUlong32;

        for (i = 0; i < 32; i++) {
            if (usbDeviceData->ContextEntriesInUse & (1 << i)) {
                slotContext->ContextEntries = i;
            }
        }

        LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                "%d: ContextEntries %2d", usbDeviceData->SlotId, slotContext->ContextEntries);

        //
        // Build and issue the TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND to the
        // controller.
        //
        RtlZeroMemory(&usbDeviceData->Crb, sizeof(usbDeviceData->Crb));

        usbDeviceData->Crb.CommandCallback = UsbDevice_EndpointsConfigureCompletion;
        usbDeviceData->Crb.ObjectHandle = (HANDLE)usbDeviceData;

        usbDeviceData->Crb.Trb.ConfigureEndpointCommand.Type = TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND;
        usbDeviceData->Crb.Trb.ConfigureEndpointCommand.Deconfigure = 0;
        usbDeviceData->Crb.Trb.ConfigureEndpointCommand.SlotId = usbDeviceData->SlotId;
        usbDeviceData->Crb.Trb.ConfigureEndpointCommand.InputContextPointer =
            (ULONG64)usbDeviceData->InputDeviceContextBuffer->LogicalAddress.QuadPart;
        usbDeviceData->Crb.InputContextVA = inputContext;

        #if TEST_MODE
        if (g_RANDOM_FAILURE_TEST_CHANGE_COMMAND_TYPE) {
            if (endpointsConfigure->EndpointsToEnableCount == 0) {
                if (Xhci_ShouldFail(10)) {
                    usbDeviceData->Crb.Trb.DisableSlotCommand.Type = TRB_TYPE_SETUP_STAGE;
                }
            }
        }
        #endif

        Command_SendCommand(commandHandle, &usbDeviceData->Crb);

        //
        // The WdfRequest will be completed by UsbDevice_EndpointsConfigureCompletion()
        // when it executes as the CommandCallback.
        //
        completeRequest = FALSE;

    } FINALLY {

        if (completeRequest) {

            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: Completing request 0x%p for USB Device 0x%p with %!STATUS!",
                    usbDeviceData->SlotId, WdfRequest, usbDeviceData->UcxUsbDevice, status);

            usbDeviceData->WdfRequest = NULL;
            WdfRequestComplete(WdfRequest, status);
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_EndpointsConfigureCompletion(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This routine is called on a command completion event for a
    TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND which was issued by
    UsbDevice_UcxEvtEndpointsConfigure().

    If the TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND completed successfully
    for the usb device then the device is currently in the Addressed
    state if the Deconfigure (DC) flag was set to '1' or in the
    Configured state if the Deconfigure (DC) flag was set to '0'.

    See section 4.5.3 Slot States and Figure 10: Slot State Diagram in
    the XHCI specification.

Arguments:

    EventTRB - Used for logging Hardware verifier events

--*/
{
    NTSTATUS                status;
    PUSBDEVICE_DATA         usbDeviceData;
    HANDLE                  registerHandle;
    HANDLE                  commandHandle;
    WDFREQUEST              wdfRequest;
    ULONG                   i;
    WDF_REQUEST_PARAMETERS  wdfRequestParams;
    PUSBDEVICE_ENDPOINTS_CONFIGURE_DATA
                            endpointsConfigureData;
    PENDPOINTS_CONFIGURE    endpointsConfigure;
    HANDLE                  endpointHandle;
    PINPUT_CONTEXT64        inputContext;
    PSLOT_CONTEXT64         slotContext;
    BOOLEAN                 setEndpointsToEnable = FALSE;
    BOOLEAN                 disableEndpointsToEnable = FALSE;
    BOOLEAN                 disableEndpointsToDisable = FALSE;
    BOOLEAN                 completeRequest = FALSE;

    TRY {

        usbDeviceData = (PUSBDEVICE_DATA)Crb->ObjectHandle;
        registerHandle = Controller_GetRegisterHandle(usbDeviceData->ControllerHandle);
        commandHandle = Controller_GetCommandHandle(usbDeviceData->ControllerHandle);

        wdfRequest = usbDeviceData->WdfRequest;

        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(wdfRequest, &wdfRequestParams);

        //
        // Retrieve a pointer to our private context data structure for
        // this WdfRequest.
        //
        endpointsConfigureData = &(GetRequestData(wdfRequest)->UsbDeviceEndpointsConfigureData);

        endpointsConfigure = (PENDPOINTS_CONFIGURE)wdfRequestParams.Parameters.Others.Arg1;

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then all endpoints are
            // effectively disabled now in the host controller.
            //
            // Notify the endpoint object to disable each of the
            // endpoints in the EndpointsToDisable[] array.
            //
            disableEndpointsToDisable = TRUE;

            if (endpointsConfigure->EndpointsToEnableCount == 0) {

                LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                        "%d: USB Device 0x%p - controller gone - %d endpoints disabled",
                        usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice,
                        endpointsConfigure->EndpointsToDisableCount);

                disableEndpointsToEnable = FALSE;
                completeRequest = TRUE;
                status = STATUS_SUCCESS;
                LEAVE;

            } else {

                LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                        "%d: USB Device 0x%p - controller gone - %d endpoints not enabled",
                        usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice,
                        endpointsConfigure->EndpointsToEnableCount);

                disableEndpointsToEnable = TRUE;
                completeRequest = TRUE;
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }
        }

        if (REGISTER_CONTEXTSIZE64 == Register_GetContextSize(registerHandle)) {

            slotContext = UsbDevice_DeviceContext64GetSlot(
                (PDEVICE_CONTEXT64)usbDeviceData->DeviceContextBuffer->VirtualAddress);

        } else {

            slotContext = (PSLOT_CONTEXT64)UsbDevice_DeviceContext32GetSlot(
                (PDEVICE_CONTEXT32)usbDeviceData->DeviceContextBuffer->VirtualAddress);
        }

        if (Crb->CompletionCode == TRB_COMPLETION_CODE_SUCCESS) {

            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p %!SlotState! endpoint configuration succeeded",
                    Crb->SlotId, usbDeviceData->UcxUsbDevice, (SLOT_STATE)slotContext->SlotState);
            
            endpointsConfigure->ExitLatencyDelta = Crb->CompletionParameter;

            //
            // If no endpoints have been configured (or all endpoints
            // have been deconfigured) the SlotState should now be
            // SLOT_STATE_ADDRESSED.
            //
            // If any endpoints have been configured the SlotState
            // should now be SLOT_STATE_CONFIGURED.
            //
            NT_ASSERT((slotContext->SlotState == SLOT_STATE_ADDRESSED) ||
                      (slotContext->SlotState == SLOT_STATE_CONFIGURED));

            //
            // Notify the endpoint object to disable each of the
            // endpoints in the EndpointsToDisable[] array.
            //
            disableEndpointsToDisable = TRUE;

            if ((endpointsConfigureData->EndpointEnableFailed == FALSE) &&
                (endpointsConfigureData->ConfigureEndpointCommandFailed == FALSE)) {

                //
                // Successfully enabled EndpointsToEnable[] (if any) and
                // successfully disabled EndpointsToDisable[] (if any).
                //
                setEndpointsToEnable = TRUE;
                disableEndpointsToEnable = FALSE;
                completeRequest = TRUE;
                status = STATUS_SUCCESS;
                LEAVE;

            } else {

                //
                // Expect only one of these to be TRUE, not both.
                //
                NT_ASSERT((endpointsConfigureData->EndpointEnableFailed == FALSE) ||
                          (endpointsConfigureData->ConfigureEndpointCommandFailed == FALSE));

                //
                // If (endpointsConfigureData->EndpointEnableFailed == TRUE)
                //
                // then UsbDevice_UcxEvtEndpointsConfigure() failed to
                // enable the endpoints in EndpointsToEnable[].  Then
                // the first and only command to disable the endpoints
                // in EndpointsToEnable[] succeeded.
                //
                // If (endpointsConfigureData->ConfigureEndpointCommandFailed == TRUE)
                //
                // then the first command to enable the endpoints in
                // EndpointsToEnable[] failed.  The endpoints in
                // EndpointsToEnable[] were disabled after the first
                // command failed.  Then the second command to disable
                // the endpoints in EndpointsToDisable[] succeeded.
                //
                // In either case the endpoints in EndpointsToEnable[]
                // are already disabled.  Now disable the endpoints in
                // EndpointsToDisable[] and complete the request with
                // failure status.
                //
                disableEndpointsToEnable = FALSE;
                completeRequest = TRUE;
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

        } else if (Crb->CompletionCode == TRB_COMPLETION_CODE_SLOT_NOT_ENABLED_ERROR) {

            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p Slot Not Enabled Error",
                    Crb->SlotId, usbDeviceData->UcxUsbDevice);

            //
            // To investigate: Can this code path still be hit now that
            // the (usbDeviceData->SlotEnabled == FALSE) code path
            // exists in UsbDevice_UcxEvtEndpointsConfigure()?
            // (See WIN8 changes 388475 and 392025).
            //
            NT_ASSERTMSG("Why was this code path hit?", 0);

            //
            // If the SlotState is not enabled then all endpoints for
            // the Device Slot are effectively disabled now in the host
            // controller.
            //
            // Notify the endpoint object to disable each of the
            // endpoints in the EndpointsToDisable[] array.
            //
            disableEndpointsToDisable = TRUE;

            if (endpointsConfigure->EndpointsToEnableCount > 0) {

                //
                // Cannot enable endpoints if the device slot is
                // disabled.  Complete the request with failure.
                //
                disableEndpointsToEnable = TRUE;
                completeRequest = TRUE;
                status = STATUS_UNSUCCESSFUL;
                LEAVE;

            } else {

                //
                // No need to disable endpoints in the host controller
                // if the device slot is disabled.  Complete the request
                // with success.
                //
                disableEndpointsToEnable = FALSE;
                completeRequest = TRUE;
                status = STATUS_SUCCESS;
                LEAVE;
            }

        } else {

            //
            // Normally this Configure Endpoint Command failure path is
            // only expected to occur when attempting to enable
            // endpoints with the error completion code
            // TRB_COMPLETION_CODE_BANDWIDTH_ERROR.
            //

            LogError(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                     "%d: USB Device 0x%p endpoint configuration failed %!TrbCompletionCode!",
                     Crb->SlotId, usbDeviceData->UcxUsbDevice, (TRB_COMPLETION_CODE)Crb->CompletionCode);

            //
            // Check for completion codes that should be propagated to the WdfRequest.
            //
            // Note, if EndpointsToEnableCount is 0, we will reset the controller and complete the
            // request later with STATUS_SUCCESS (disable request must succeed). Given we are
            // returning success in that case, we must not set any FailureFlags, else it could be
            // interpreted as failure.
            //
            if (Crb->CompletionCode == TRB_COMPLETION_CODE_BANDWIDTH_ERROR ||
                Crb->CompletionCode == TRB_COMPLETION_CODE_SECONDARY_BANDWIDTH_ERROR) {

                if (endpointsConfigure->EndpointsToEnableCount != 0) {

                    endpointsConfigure->FailureFlags.InsufficientBandwidth = 1;
                }

            } else if (Crb->CompletionCode == TRB_COMPLETION_CODE_RESOURCE_ERROR) {

                if (endpointsConfigure->EndpointsToEnableCount != 0) {

                    endpointsConfigure->FailureFlags.InsufficientHardwareResourcesForEndpoints = 1;
                }
            } else if (Crb->CompletionCode == TRB_COMPLETION_CODE_MAX_EXIT_LATENCY_TOO_LARGE_ERROR) {

                if (endpointsConfigure->EndpointsToEnableCount != 0) {

                    endpointsConfigure->FailureFlags.MaxExitLatencyTooLarge = 1;
                }
                endpointsConfigure->ExitLatencyDelta = Crb->CompletionParameter;
            }       


            //
            // The cause of any parameter errors should be debugged.
            //
            NT_ASSERT(Crb->CompletionCode != TRB_COMPLETION_CODE_PARAMETER_ERROR);

            if (endpointsConfigure->EndpointsToDisableCount == 0) {

                //
                // Failed to enable the endpoints in the
                // EndpointsToEnable[] array.  Notify the endpoint
                // object to disable each of the endpoints in the
                // EndpointsToEnable[] array.
                //
                // Since the EndpointsToDisable[] array is empty just
                // complete the request with failure.
                //
                disableEndpointsToEnable = TRUE;
                disableEndpointsToDisable = FALSE;
                completeRequest = TRUE;
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            if (endpointsConfigure->EndpointsToEnableCount == 0) {

                //
                // No endpoints were attempted to be enabled in the
                // first Configure Endpoint Command.
                //
                // The Configure Endpoint Command failed while only
                // attempting to disable the endpoints in the
                // EndpointsToDisable[].  This case is not expected.
                //
                disableEndpointsToEnable = FALSE;
                disableEndpointsToDisable = FALSE;
                
                //
                // Reset the controller
                //
                Controller_HwVerifierBreakIfEnabled(
                    usbDeviceData->ControllerHandle,
                    usbDeviceData->UcxUsbDevice,
                    NULL,
                    ControllerHwVerifierEndpointsDeconfigureFailed,
                    "Configure Endpoints command failed when only disabling endpoints",
                    &Crb->Trb,
                    EventTRB);

                
                Controller_ReportFatalError(usbDeviceData->ControllerHandle,
                                            Controller_FatalErrorReset,
                                            USB3_WER_BUGCODE_USBXHCI_ENDPOINTS_DECONFIGURE_FAILED,
                                            NULL,
                                            NULL,
                                            NULL);

                completeRequest = FALSE;
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            if (endpointsConfigureData->EndpointEnableFailed == TRUE) {

                //
                // No endpoints were attempted to be enabled in the
                // first Configure Endpoint Command due to an
                // Endpoint_Enable() call failure in
                // UsbDevice_UcxEvtEndpointsConfigure().
                //
                // The Configure Endpoint Command failed while only
                // attempting to disable the endpoints in the
                // EndpointsToDisable[].  This case is not expected.
                //
                disableEndpointsToEnable = FALSE;
                disableEndpointsToDisable = FALSE;
                completeRequest = FALSE;
                
                //
                // Reset the controller
                //
                Controller_HwVerifierBreakIfEnabled(
                    usbDeviceData->ControllerHandle,
                    usbDeviceData->UcxUsbDevice,
                    NULL,
                    ControllerHwVerifierEndpointsDeconfigureFailed,
                    "Configure Endpoints command failed when only disabling endpoints",
                    &Crb->Trb,
                    EventTRB);
                
                Controller_ReportFatalError(usbDeviceData->ControllerHandle,
                                            Controller_FatalErrorReset,
                                            USB3_WER_BUGCODE_USBXHCI_ENDPOINTS_DECONFIGURE_FAILED,
                                            NULL,
                                            NULL,
                                            NULL);
                
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            if (endpointsConfigureData->ConfigureEndpointCommandFailed == TRUE) {

                //
                // No endpoints were attempted to be enabled in the
                // second Configure Endpoint Command due the failure to
                // enable endpoints during the first Configure Endpoint
                // Command.
                //
                // The Configure Endpoint Command failed while only
                // attempting to disable the endpoints in the
                // EndpointsToDisable[].  This case is not expected.
                //
                disableEndpointsToEnable = FALSE;
                disableEndpointsToDisable = FALSE;
                
                //
                // Reset the controller
                //
                Controller_HwVerifierBreakIfEnabled(
                    usbDeviceData->ControllerHandle,
                    usbDeviceData->UcxUsbDevice,
                    NULL,
                    ControllerHwVerifierEndpointsDeconfigureFailed,
                    "Configure Endpoints command failed when only disabling endpoints",
                    &Crb->Trb,
                    EventTRB);
                
                Controller_ReportFatalError(usbDeviceData->ControllerHandle,
                                            Controller_FatalErrorReset,
                                            USB3_WER_BUGCODE_USBXHCI_ENDPOINTS_DECONFIGURE_FAILED,
                                            NULL,
                                            NULL,
                                            NULL);
                
                completeRequest = FALSE;
                status = STATUS_UNSUCCESSFUL;
                LEAVE;
            }

            //
            // Prepare to issue a second Configure Endpoint Command due
            // the failure to enable endpoints during the first
            // Configure Endpoint Command.
            //
            NT_ASSERT(endpointsConfigure->EndpointsToEnableCount > 0);
            NT_ASSERT(endpointsConfigure->EndpointsToDisableCount > 0);

            endpointsConfigureData->ConfigureEndpointCommandFailed = TRUE;

            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: Request 0x%p for USB Device 0x%p ConfigureEndpointCommandFailed",
                    usbDeviceData->SlotId, wdfRequest, usbDeviceData->UcxUsbDevice);

            //
            // Notify the endpoint object to disable each of the
            // endpoints in the EndpointsToEnable[] array.  Do this now
            // before issuing the Configure Endpoint Command since the
            // WdfRequest could possibly be completed synchronously by
            // the CommandCallback when calling Command_SendCommand().
            //
            for (i = 0; i < endpointsConfigure->EndpointsToEnableCount; i++) {

                endpointHandle = Endpoint_GetEndpointHandle(endpointsConfigure->EndpointsToEnable[i]);
                Endpoint_Disable(endpointHandle, FALSE);
            }

            //
            // Reinitialize the Input Device Context for the
            // TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND.  This time only set
            // the Drop Context flags for the endpoints in the
            // EndpointsToDisable[] array.  Do not set the Add Context
            // flags for the endpoints in the EndpointsToEnable[]
            // array.
            //
            inputContext = (PINPUT_CONTEXT64)usbDeviceData->InputDeviceContextBuffer->VirtualAddress;

            if (REGISTER_CONTEXTSIZE64 == Register_GetContextSize(registerHandle)) {
                RtlZeroMemory(inputContext, sizeof(INPUT_CONTEXT64));
                slotContext     = UsbDevice_InputContext64GetSlot(inputContext);
            } else {
                PINPUT_CONTEXT32    inputContext32 = (PINPUT_CONTEXT32)inputContext;
                RtlZeroMemory(inputContext, sizeof(INPUT_CONTEXT32));
                slotContext     = (PSLOT_CONTEXT64)UsbDevice_InputContext32GetSlot(inputContext32);
            }

            inputContext->InputControlContext.AddContextFlags.A0 = 1;

            if (usbDeviceData->IsHub) {
                slotContext->Hub = 1;
                slotContext->NumberOfPorts = usbDeviceData->NumberOfPorts;
                if (usbDeviceData->UsbDeviceInfo.DeviceSpeed == UsbHighSpeed) {
                    if (usbDeviceData->NumberOfTTs > 1) {
                        slotContext->MultiTT = 1;
                    }
                    slotContext->TTThinkTime = usbDeviceData->TTThinkTime;
                }
            }

            for (i = 0; i < endpointsConfigure->EndpointsToDisableCount; i++) {
                UCXENDPOINT ucxEndpoint = endpointsConfigure->EndpointsToDisable[i];
                HANDLE      endpoint = Endpoint_GetEndpointHandle(ucxEndpoint);
                ULONG       deviceContextIndex = Endpoint_GetContextIndex(endpoint);

                //
                // Set the Drop Context flag for this endpoint.
                //
                inputContext->InputControlContext.DropContextFlags.AsUlong32 |=
                    (1 << deviceContextIndex);
            }

            //
            // Disable it for now.
            //
#if 0
            if (Register_GetConfigurationInformation(registerHandle)) {
                inputContext->InputControlContext.ConfigurationValue = endpointsConfigure->ConfigurationValue;
                inputContext->InputControlContext.AlternateSetting = endpointsConfigure->AlternateSetting;
                inputContext->InputControlContext.InterfaceNumber = endpointsConfigure->InterfaceNumber;
            }
#endif

            //
            // Build and issue the TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND to the
            // controller.
            //
            RtlZeroMemory(&usbDeviceData->Crb, sizeof(usbDeviceData->Crb));

            usbDeviceData->Crb.CommandCallback = UsbDevice_EndpointsConfigureCompletion;
            usbDeviceData->Crb.ObjectHandle = (HANDLE)usbDeviceData;

            usbDeviceData->Crb.Trb.ConfigureEndpointCommand.Type = TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND;
            usbDeviceData->Crb.Trb.ConfigureEndpointCommand.Deconfigure = 0;
            usbDeviceData->Crb.Trb.ConfigureEndpointCommand.SlotId = usbDeviceData->SlotId;
            usbDeviceData->Crb.Trb.ConfigureEndpointCommand.InputContextPointer =
                (ULONG64)usbDeviceData->InputDeviceContextBuffer->LogicalAddress.QuadPart;
            usbDeviceData->Crb.InputContextVA = inputContext;

            #if TEST_MODE
            if (g_RANDOM_FAILURE_TEST_CHANGE_COMMAND_TYPE) {
                if (Xhci_ShouldFail(10)) {
                    usbDeviceData->Crb.Trb.DisableSlotCommand.Type = TRB_TYPE_SETUP_STAGE;
                }
            }
            #endif
            
            Command_SendCommand(commandHandle, &usbDeviceData->Crb);

            status = STATUS_SUCCESS;

            //
            // The WdfRequest will be completed by this routine when it
            // executes a second time as the CommandCallback.
            //
            disableEndpointsToEnable = FALSE;
            disableEndpointsToDisable = FALSE;
            completeRequest = FALSE;
            LEAVE;
        }

    } FINALLY {

        if (disableEndpointsToDisable == TRUE) {

            for (i = 0; i < endpointsConfigure->EndpointsToDisableCount; i++) {

                endpointHandle = Endpoint_GetEndpointHandle(endpointsConfigure->EndpointsToDisable[i]);

                //
                // Notify the endpoint object to disable each of the
                // endpoints in the EndpointsToDisable[] array.
                //
                Endpoint_Disable(endpointHandle, FALSE);
            }
        }

        if (disableEndpointsToEnable == TRUE) {

            //
            // Notify the endpoint object to disable each of the
            // endpoints in the EndpointsToEnable[] array.
            //
            for (i = 0; i < endpointsConfigure->EndpointsToEnableCount; i++) {

                endpointHandle = Endpoint_GetEndpointHandle(endpointsConfigure->EndpointsToEnable[i]);
                Endpoint_Disable(endpointHandle, FALSE);
            }
        }

        if (setEndpointsToEnable == TRUE) {

            NT_ASSERT(disableEndpointsToEnable == FALSE);

            //
            // Cache the endpoint handles in the array of endpoints.
            //
            for (i = 0; i < endpointsConfigure->EndpointsToEnableCount; i++) {

                endpointHandle = Endpoint_GetEndpointHandle(endpointsConfigure->EndpointsToEnable[i]);

                //
                // The client is configuring the endpoint with a new handle. In this case,
                // the previous endpoint needs to be disabled before allowing the new
                // endpoint handle.
                //
                if ((usbDeviceData->EndpointHandle[Endpoint_GetContextIndex(endpointHandle)] != NULL) &&
                    (usbDeviceData->EndpointHandle[Endpoint_GetContextIndex(endpointHandle)] != endpointHandle)) {
                    Endpoint_Disable(usbDeviceData->EndpointHandle[Endpoint_GetContextIndex(endpointHandle)], TRUE);
                    usbDeviceData->EndpointHandle[Endpoint_GetContextIndex(endpointHandle)] = NULL;
                }

                UsbDevice_SetEndpointHandle(usbDeviceData,
                                            Endpoint_GetContextIndex(endpointHandle),
                                            endpointHandle);
            }
        }

        if (completeRequest) {

            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: Completing request 0x%p for USB Device 0x%p with %!STATUS!",
                    usbDeviceData->SlotId, wdfRequest, usbDeviceData->UcxUsbDevice, status);

            usbDeviceData->WdfRequest = NULL;
            WdfRequestComplete(wdfRequest, status);
        }
    }

    return;
}

VOID
UsbDevice_UcxEvtDisable(
    UCXCONTROLLER   UcxController,
    WDFREQUEST      WdfRequest
    )
/*++

Routine Description:

    This routine is a callback which is called by the class extension to
    inform the controller driver that it can remove the information
    about the specified device from the controller in order to release
    resources that it may be consuming.

    Upon completion of this request, the only routine that the class
    extension will call that references this UsbDevice would be
    UsbDevice_UcxEvtEnable in order to reprogram it into the controller.
    While a device is disabled there will be no transfers scheduled for
    it.

    The device should currently be in either the Enabled, Default,
    Addressed, or Configured state and an attempt will be made to
    transition the device to the Disabled state by issuing a
    TRB_TYPE_DISABLE_SLOT_COMMAND to the controller for the device.

    See section 4.5.3 Slot States and Figure 10: Slot State Diagram in
    the XHCI specification.

    Refer to section 4.6.4 for a detailed description of the Disable
    Slot command.

Arguments:

    UcxController - The UCX controller object.

    WdfRequest - The WDF request object for the IOCTL_INTERNAL_USB_USBDEVICE_DISABLE request.

Return Value:

    None directly from this routine.

    The completion status in the WdfRequest, which may be completed asynchronously.

--*/
{
    WDF_REQUEST_PARAMETERS  wdfRequestParams;
    PUSBDEVICE_DISABLE      usbDeviceDisable;
    PUSBDEVICE_DATA         usbDeviceData;
    HANDLE                  commandHandle;
    ULONG                   i;

    UNREFERENCED_PARAMETER(UcxController);

    TRY {

        //
        // Retrieve the USBDEVICE_DISABLE pointer from the
        // IOCTL_INTERNAL_USB_USBDEVICE_DISABLE WdfRequest.
        //
        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);
        usbDeviceDisable = (PUSBDEVICE_DISABLE)wdfRequestParams.Parameters.Others.Arg1;

        //
        // Retrieve the USBDEVICE_DATA pointer from the UCXUSBDEVICE in
        // the USBDEVICE_MGMT_HEADER of the USBDEVICE_DISABLE.
        //
        usbDeviceData = GetUsbDeviceData(usbDeviceDisable->UsbDevice);
        NT_ASSERT(usbDeviceData != NULL);

        commandHandle = Controller_GetCommandHandle(usbDeviceData->ControllerHandle);
        NT_ASSERT(commandHandle != NULL);

        NT_ASSERT(usbDeviceData->WdfRequest == NULL);

        LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                "%d: USB Device 0x%p device slot disable requested",
                usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice);

        if (usbDeviceData->SlotEnabled == FALSE) {
            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p device slot is already disabled",
                    usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice);

            //
            // Disable the default control endpoint.
            //
            Endpoint_Disable(usbDeviceData->EndpointHandle[1], TRUE);

            //
            // Disable all endpoints other than the default control endpoint
            // and clear their endpoint handles.
            //
            for (i = 2; i < MAX_DEVICE_CONTEXT_INDEX; i++) {

                if (usbDeviceData->EndpointHandle[i] != NULL) {

                    Endpoint_Disable(usbDeviceData->EndpointHandle[i], TRUE);
                    usbDeviceData->EndpointHandle[i] = NULL;
                }
            }

            WdfRequestComplete(WdfRequest, STATUS_SUCCESS);
            LEAVE;
        }

        //
        // Need to inform the root hub that this slot is getting disabled, in case
        // it is programmed into one of the ports
        //
        //
        if (usbDeviceData->Is20LpmEnabled) {
            HANDLE  rootHubHandle;

            rootHubHandle = Controller_GetRootHubHandle(usbDeviceData->ControllerHandle);

            RootHub_DisableLPMForSlot(rootHubHandle,
                                      usbDeviceData->UsbDeviceInfo.PortPath.PortPath[0],
                                      usbDeviceData->SlotId);
        }

        if (usbDeviceData->IsRootPortResumeTimeSet) {
            HANDLE  rootHubHandle;

            rootHubHandle = Controller_GetRootHubHandle(usbDeviceData->ControllerHandle);

            RootHub_ClearPortResumeTime(rootHubHandle, 
                                        usbDeviceData->UsbDeviceInfo.PortPath.PortPath[0]);
        }

        //
        // If controller is gone, there is a race between this UCX callback processing
        // a request, and UsbDevice_ControllerGone trying to complete outstanding
        // requests. This check here minimizes the window in which that race exists,
        // which is a sufficiently robust low risk fix for Windows 8.
        //
        if (Controller_IsControllerAccessible(usbDeviceData->ControllerHandle) == FALSE) {
            UsbDevice_SetDeviceDisabled(usbDeviceData);
            WdfRequestComplete(WdfRequest, STATUS_SUCCESS);
            LEAVE;
        }

        //
        // Store the IOCTL_INTERNAL_USB_USBDEVICE_DISABLE WdfRequest in
        // the USBDEVICE_DATA.  In the successful case this WdfRequest
        // will be asynchronously completed by UsbDevice_DisableCompletion().
        //
        usbDeviceData->WdfRequest = WdfRequest;
        usbDeviceData->WdfRequestMustSucceed = TRUE;

        //
        // Refer to section 4.6.4 for a detailed description of the
        // Disable Slot command.
        //
        RtlZeroMemory(&usbDeviceData->Crb, sizeof(usbDeviceData->Crb));
        usbDeviceData->Crb.CommandCallback = UsbDevice_DisableCompletionReturnSuccess;
        usbDeviceData->Crb.ObjectHandle = (HANDLE)usbDeviceData;
        usbDeviceData->Crb.Trb.DisableSlotCommand.Type = TRB_TYPE_DISABLE_SLOT_COMMAND;
        usbDeviceData->Crb.Trb.DisableSlotCommand.SlotId = usbDeviceData->SlotId;

        #if TEST_MODE
        if (g_RANDOM_FAILURE_TEST_CHANGE_COMMAND_TYPE) {
            if (Xhci_ShouldFail(10)) {
                usbDeviceData->Crb.Trb.DisableSlotCommand.Type = TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND;
            }
        }
        #endif
        
        Command_SendCommand(commandHandle, &usbDeviceData->Crb);

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_DisableCompletionReturnFailure(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This routine is called on a command completion event for a
    TRB_TYPE_DISABLE_SLOT_COMMAND which was issued by a failure code
    path in UsbDevice_EnableCompletion() or
    UsbDevice_SetAddressCompletion().

Arguments:

    EventTRB - To be used for logging Hardware verifier events

--*/
{
    UNREFERENCED_PARAMETER(EventTRB);

    UsbDevice_DisableCompletion(Crb,
                                CrbCompletionStatus,
                                DisableSlotNextAction_CompleteWithFailure);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_DisableCompletionReturnSuccess(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This routine is called on a command completion event for a
    TRB_TYPE_DISABLE_SLOT_COMMAND which was issued by
    UsbDevice_UcxEvtDisable().

Arguments:

    EventTRB - To be used for logging Hardware verifier events

--*/
{
    UNREFERENCED_PARAMETER(EventTRB);

    UsbDevice_DisableCompletion(Crb,
                                CrbCompletionStatus,
                                DisableSlotNextAction_Complete);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_OnResetDisableCompletion(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This routine is called on a command completion event for a
    TRB_TYPE_DISABLE_SLOT_COMMAND which was issued by
    UsbDevice_UcxEvtReset().

Arguments:

    EventTRB - To be used for logging Hardware verifier events

--*/
{
    UNREFERENCED_PARAMETER(EventTRB);

    UsbDevice_DisableCompletion(Crb,
                                CrbCompletionStatus,
                                DisableSlotNextAction_EnableSlot);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_DisableCompletion(
    __in
        PCRB                        Crb,
    __in
        CRB_COMPLETION_STATUS       CrbCompletionStatus,
    __in
        DISABLE_SLOT_NEXT_ACTION
                                    NextAction
    )
/*++

Routine Description:

    If a device slot was successfully disabled for the usb device then
    the device is now currently in the Disabled state.

    See section 4.5.3 Slot States and Figure 10: Slot State Diagram in
    the XHCI specification.

    Refer to section 4.6.4 for a detailed description of the Disable
    Slot command.

--*/
{
    NTSTATUS        status;
    PUSBDEVICE_DATA usbDeviceData;
    HANDLE          deviceSlotHandle;
    WDFREQUEST      wdfRequest;
    ULONG           i;
    BOOLEAN         completeRequestOnError;

    TRY {

        completeRequestOnError = TRUE;

        usbDeviceData = (PUSBDEVICE_DATA)Crb->ObjectHandle;

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the device is
            // effectively disabled now.
            //
            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p - controller gone - device slot disable succeeds",
                    usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice);

            UsbDevice_SetDeviceDisabled(usbDeviceData);

            completeRequestOnError = FALSE;
            status = STATUS_UNSUCCESSFUL;

        } else if (Crb->CompletionCode == TRB_COMPLETION_CODE_SUCCESS ||
                   Crb->CompletionCode == TRB_COMPLETION_CODE_SLOT_NOT_ENABLED_ERROR) {

            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p device slot disable succeeded",
                    Crb->SlotId, usbDeviceData->UcxUsbDevice);

            deviceSlotHandle = Controller_GetDeviceSlotHandle(usbDeviceData->ControllerHandle);

            if (NextAction != DisableSlotNextAction_EnableSlot) {
                //
                // Disable the default control endpoint. Note: do not clear the
                // endpoint handle. This cannot be done safely for the default
                // control endpoint, since it is created with the device and
                // must be present for the life of the device.
                //
                Endpoint_Disable(usbDeviceData->EndpointHandle[1], TRUE);

            }

            //
            // Disable all endpoints other than the default control endpoint
            // and clear their endpoint handles.
            //
            
            for (i = 2; i < MAX_DEVICE_CONTEXT_INDEX; i++) {
                
                if (usbDeviceData->EndpointHandle[i] != NULL) {
                    
                    Endpoint_Disable(usbDeviceData->EndpointHandle[i], TRUE);
                    usbDeviceData->EndpointHandle[i] = NULL;
                }
            }

            //
            // On successful completion of the TRB_TYPE_DISABLE_SLOT_COMMAND
            // the slot is no longer considered valid by the xHC and
            // software can free the Device Context, Transfer Ring,
            // Stream Context Array, etc. data structures associated
            // with the slot.
            //
            // This should never fail.  If it does fail, then something
            // internal to xHCI or the USB3 stack went wrong and that
            // needs to be understood. The xHCI driver won't free the
            // slot in this case and the device slot will be leaked.
            //
            DeviceSlot_ClearDeviceContext(deviceSlotHandle,
                                          (HANDLE)usbDeviceData,
                                          usbDeviceData->SlotId);

            usbDeviceData->SlotEnabled = FALSE;
            usbDeviceData->SlotId = 0;
            usbDeviceData->DoorbellRegister = NULL;

            status = STATUS_SUCCESS;

        } else {

            LogError(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                     "%d: USB Device 0x%p device slot disable failed %!TrbCompletionCode!",
                     Crb->SlotId, usbDeviceData->UcxUsbDevice,
                     (TRB_COMPLETION_CODE)Crb->CompletionCode);
            
            //
            // Invoke Controller Reset recovery here
            //
            Controller_HwVerifierBreakIfEnabled(
                usbDeviceData->ControllerHandle,
                usbDeviceData->UcxUsbDevice,
                NULL,
                ControllerHwVerifierDisableSlotCommandFailed,
                "Disable Slot Command failed",
                &Crb->Trb,
                NULL);

            Controller_ReportFatalError(usbDeviceData->ControllerHandle,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_DISABLE_SLOT_FAILED,
                                        NULL,
                                        NULL,
                                        NULL);
            completeRequestOnError = FALSE;

            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

    } FINALLY {

        if (NT_SUCCESS(status) &&
            (NextAction == DisableSlotNextAction_EnableSlot)) {

            RtlZeroMemory(&usbDeviceData->Crb, sizeof(usbDeviceData->Crb));
            usbDeviceData->Crb.CommandCallback = UsbDevice_EnableCompletion;
            usbDeviceData->Crb.ObjectHandle = (HANDLE)usbDeviceData;
            usbDeviceData->Crb.Trb.EnableSlotCommand.Type = TRB_TYPE_ENABLE_SLOT_COMMAND;

            #if TEST_MODE
            if (g_RANDOM_FAILURE_TEST_CHANGE_COMMAND_TYPE) {
                if (Xhci_ShouldFail(10)) {
                    usbDeviceData->Crb.Trb.DisableSlotCommand.Type = TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND;
                }
            }
            #endif
            
            Command_SendCommand(Controller_GetCommandHandle(usbDeviceData->ControllerHandle),
                                &usbDeviceData->Crb);

        } else {

            if (NextAction == DisableSlotNextAction_CompleteWithFailure) {
                status = STATUS_UNSUCCESSFUL;
            }
            
            if (completeRequestOnError) {
                
                //
                // Complete the WDF request object for the
                // IOCTL_INTERNAL_USB_USBDEVICE_DISABLE request here.
                //

                wdfRequest = usbDeviceData->WdfRequest;
                usbDeviceData->WdfRequest = NULL;
                WdfRequestComplete(wdfRequest, status);
            }
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_SetDeviceDisabled(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    )
/*++

Routine Description:

    This routine marks the device as Disabled.

--*/
{
    HANDLE                  deviceSlotHandle;
    ULONG                   i;
    NTSTATUS                status;
    WDFREQUEST              wdfRequest;

    TRY {

        LogInfo(Controller_GetRecorderLog(UsbDeviceData->ControllerHandle), UsbDevice,
                "%d: USB Device 0x%p set disabled",
                UsbDeviceData->SlotId, UsbDeviceData->UcxUsbDevice);

        deviceSlotHandle = Controller_GetDeviceSlotHandle(UsbDeviceData->ControllerHandle);

        //
        // Disable the default control endpoint. Note: do not clear the
        // endpoint handle. This cannot be done safely for the default
        // control endpoint, since it is created with the device and
        // must be present for the life of the device.
        //
        Endpoint_Disable(UsbDeviceData->EndpointHandle[1], TRUE);

        //
        // Disable all endpoints other than the default control endpoint
        // and clear their endpoint handles.
        //
        for (i = 2; i < MAX_DEVICE_CONTEXT_INDEX; i++) {

            if (UsbDeviceData->EndpointHandle[i] != NULL) {

                Endpoint_Disable(UsbDeviceData->EndpointHandle[i], TRUE);
                UsbDeviceData->EndpointHandle[i] = NULL;
            }
        }

        DeviceSlot_ClearDeviceContext(deviceSlotHandle,
                                      (HANDLE)UsbDeviceData,
                                      UsbDeviceData->SlotId);

        wdfRequest = UsbDeviceData->WdfRequest;
        if (UsbDeviceData->WdfRequestMustSucceed) {
            status = STATUS_SUCCESS;
        } else {
            // In a normal case the Device is disabled only when UsbDevice_Diable is called.
            // When that happens, the UsbDeviceData->WdfRequestMustSucceed is set to true.
            // The fact that we have disabled the device for a failable device management
            // operation (UsbDeviceData->WdfRequest), indicates that something wrong happened. 
            // Thus we need to fail UsbDeviceData->WdfRequest. 
            // An example on how this code may be invoked is: 
            // UsbDevice_Enable - Enable Slot succeeded, Set Address Failed, thus we have to
            // disable the slot.
            status = STATUS_UNSUCCESSFUL;
        }
        
        UsbDeviceData->WdfRequest = NULL;
        UsbDeviceData->SlotEnabled = FALSE;
        UsbDeviceData->SlotId = 0;
        UsbDeviceData->DoorbellRegister = NULL;

        if (wdfRequest) {
            WdfRequestComplete(wdfRequest, status);
        }

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_ControllerGone(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    )
/*++

Routine Description:

    This routine completes any pending device management requests.

--*/
{
    ULONG       i;
    NTSTATUS    status;
    WDFREQUEST  wdfRequest;

    TRY {

        for (i = 1; i < MAX_DEVICE_CONTEXT_INDEX; i += 1) {

            if (UsbDeviceData->EndpointHandle[i] != NULL) {

                Endpoint_ControllerSurpriseRemoved(UsbDeviceData->EndpointHandle[i]);
            }
        }

        if (UsbDeviceData->WdfRequest) {            
            if (UsbDeviceData->WdfRequestMustSucceed) {
                status = STATUS_SUCCESS;
            } else {
                status = STATUS_INTERNAL_ERROR;
            }
            
            wdfRequest = UsbDeviceData->WdfRequest;
            UsbDeviceData->WdfRequest = NULL;
            WdfRequestComplete(wdfRequest, status);
        }

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
UsbDevice_ControllerResetPreReset(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    )
{
    ULONG i;

    for (i = 1; i < MAX_DEVICE_CONTEXT_INDEX; i += 1) {

        if (UsbDeviceData->EndpointHandle[i] != NULL) {

            Endpoint_ControllerResetStart(UsbDeviceData->EndpointHandle[i]);
        }
    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
UsbDevice_ControllerResetPostReset(
    __in
        PUSBDEVICE_DATA UsbDeviceData
    )
{
    ULONG i;

    for (i = 1; i < MAX_DEVICE_CONTEXT_INDEX; i += 1) {

        if (UsbDeviceData->EndpointHandle[i] != NULL) {

            Endpoint_ControllerResetComplete(UsbDeviceData->EndpointHandle[i]);
        }
    }
}

VOID
UsbDevice_UcxEvtReset(
    UCXCONTROLLER   UcxController,
    WDFREQUEST      WdfRequest
    )
/*++

Routine Description:

    This routine is a callback which is called by the class extension to
    inform the controller driver that the port to which the device is
    attached has been reset.

    The device should currently be in either the Default, Addressed, or
    Configured state and an attempt will be made to transition the
    device to the Default state by issuing a TRB_TYPE_RESET_DEVICE_COMMAND
    to the controller for the device.

    See section 4.5.3 Slot States and Figure 10: Slot State Diagram in
    the XHCI specification.

    Refer to section 4.6.11 for a detailed description of the Reset
    Device command.

Arguments:

    UcxController - The UCX controller object.

    WdfRequest - The WDF request object for the IOCTL_INTERNAL_USB_USBDEVICE_RESET request.

Return Value:

    None directly from this routine.

    The completion status in the WdfRequest, which may be completed asynchronously.

--*/
{
    WDF_REQUEST_PARAMETERS  wdfRequestParams;
    PUSBDEVICE_RESET        usbDeviceReset;
    PUSBDEVICE_DATA         usbDeviceData;
    HANDLE                  registerHandle;
    HANDLE                  commandHandle;
    PSLOT_CONTEXT64         slotContext;
    ULONG                   i;
    ADD_CONTEXT_FLAGS       addContextFlags;

    UNREFERENCED_PARAMETER(UcxController);

    TRY {

        //
        // Retrieve the USBDEVICE_RESET pointer from the
        // IOCTL_INTERNAL_USB_USBDEVICE_RESET WdfRequest.
        //
        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);
        usbDeviceReset = (PUSBDEVICE_RESET)wdfRequestParams.Parameters.Others.Arg1;

        //
        // Retrieve the USBDEVICE_DATA pointer from the UCXUSBDEVICE in
        // the USBDEVICE_MGMT_HEADER of the USBDEVICE_RESET.
        //
        usbDeviceData = GetUsbDeviceData(usbDeviceReset->UsbDevice);
        NT_ASSERT(usbDeviceData != NULL);

        registerHandle = Controller_GetRegisterHandle(usbDeviceData->ControllerHandle);
        NT_ASSERT(registerHandle != NULL);

        commandHandle = Controller_GetCommandHandle(usbDeviceData->ControllerHandle);
        NT_ASSERT(commandHandle != NULL);

        NT_ASSERT(usbDeviceData->WdfRequest == NULL);

        if (usbDeviceData->SlotEnabled == FALSE) {

            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p device slot is disabled",
                    usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice);

            //
            // IOCTL_INTERNAL_USB_USBDEVICE_RESET is never supposed to
            // fail.
            //
            WdfRequestComplete(WdfRequest, STATUS_SUCCESS);
            LEAVE;
        }

        if (REGISTER_CONTEXTSIZE64 == Register_GetContextSize(registerHandle)) {

            slotContext = UsbDevice_DeviceContext64GetSlot(
                (PDEVICE_CONTEXT64)usbDeviceData->DeviceContextBuffer->VirtualAddress);

        } else {

            slotContext = (PSLOT_CONTEXT64)UsbDevice_DeviceContext32GetSlot(
                (PDEVICE_CONTEXT32)usbDeviceData->DeviceContextBuffer->VirtualAddress);
        }

        LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                "%d: USB Device 0x%p %!SlotState! reset device requested",
                usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice, (SLOT_STATE)slotContext->SlotState);

        if ((slotContext->SlotState == SLOT_STATE_DISABLED_OR_ENABLED) ||
            (slotContext->SlotState == SLOT_STATE_DEFAULT)) {

            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p device slot is not Addressed or Configured",
                    usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice);

            //
            // Treat the Reset Device request as a NOP if the Slot
            // State is not Addressed or Configured.
            //
            WdfRequestComplete(WdfRequest, STATUS_SUCCESS);
            LEAVE;
        }

        //
        // If controller is gone, there is a race between this UCX callback processing
        // a request, and UsbDevice_ControllerGone trying to complete outstanding
        // requests. This check here minimizes the window in which that race exists,
        // which is a sufficiently robust low risk fix for Windows 8.
        //
        if (Controller_IsControllerAccessible(usbDeviceData->ControllerHandle) == FALSE) {

            //
            // Deconfigure all non-default control endpoints. UCX/HUB expects
            // all endpoints are deconfigured prior to completion of the reset,
            // leaving the device in the enabled state.
            //
            for (i = 2; i < MAX_DEVICE_CONTEXT_INDEX; i++) {

                if (usbDeviceData->EndpointHandle[i] != NULL) {

                    Endpoint_Disable(usbDeviceData->EndpointHandle[i], FALSE);
                }
            }

            //
            // After a reset the Slot Context and the Endpoint 0
            // Context are the only contexts in use.
            //
            addContextFlags.AsUlong32 = 0;
            addContextFlags.A0 = 1;
            addContextFlags.A1 = 1;
            usbDeviceData->ContextEntriesInUse = addContextFlags.AsUlong32;

            WdfRequestComplete(WdfRequest, STATUS_SUCCESS);
            LEAVE;
        }

        //
        // Store the IOCTL_INTERNAL_USB_USBDEVICE_RESET WdfRequest in
        // the USBDEVICE_DATA.  In the successful case this WdfRequest
        // will be asynchronously completed by UsbDevice_DeviceResetCompletion().
        //
        usbDeviceData->WdfRequest = WdfRequest;
        usbDeviceData->WdfRequestMustSucceed = TRUE;

        if (Controller_GetDeviceFlags(usbDeviceData->ControllerHandle).OnResetDisableEnable) {

            usbDeviceData->Crb.CommandCallback = UsbDevice_OnResetDisableCompletion;
            usbDeviceData->Crb.ObjectHandle = (HANDLE)usbDeviceData;
            usbDeviceData->Crb.Trb.DisableSlotCommand.Type = TRB_TYPE_DISABLE_SLOT_COMMAND;
            usbDeviceData->Crb.Trb.DisableSlotCommand.SlotId = usbDeviceData->SlotId;

        } else {

            //
            // Refer to section 4.6.11 for a detailed description of the
            // Reset Device command.
            //
            usbDeviceData->Crb.CommandCallback = UsbDevice_DeviceResetCompletion;
            usbDeviceData->Crb.ObjectHandle = (HANDLE)usbDeviceData;
            usbDeviceData->Crb.Trb.ResetDeviceCommand.Type = TRB_TYPE_RESET_DEVICE_COMMAND;
            usbDeviceData->Crb.Trb.ResetDeviceCommand.SlotId = usbDeviceData->SlotId;
        }

        #if TEST_MODE
        if (g_RANDOM_FAILURE_TEST_CHANGE_COMMAND_TYPE) {
            if (Xhci_ShouldFail(10)) {
                usbDeviceData->Crb.Trb.DisableSlotCommand.Type = TRB_TYPE_SETUP_STAGE;
            }
        }
        #endif
        
        Command_SendCommand(commandHandle, &usbDeviceData->Crb);

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_DeviceResetCompletion(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This routine is called on a command completion event for a
    TRB_TYPE_RESET_DEVICE_COMMAND which was issued by
    UsbDevice_UcxEvtReset().

    If a device slot was successfully reset for the usb device then
    the device is now currently in the Default state.

    See section 4.5.3 Slot States and Figure 10: Slot State Diagram in
    the XHCI specification.

    Refer to section 4.6.11 for a detailed description of the Reset
    Device command.

Arguments:

    EventTRB - Used for logging Hardware verifier events

--*/
{
    NTSTATUS            status;
    PUSBDEVICE_DATA     usbDeviceData;
    WDFREQUEST          wdfRequest;
    ULONG               i;
    ADD_CONTEXT_FLAGS   addContextFlags;
    BOOLEAN             completeRequest;

    TRY {

        completeRequest = TRUE;

        usbDeviceData = (PUSBDEVICE_DATA)Crb->ObjectHandle;

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the device is
            // effectively reset now.
            //
            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p - controller gone - device reset succeeds",
                    usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice);

            //
            // Deconfigure all non-default control endpoints. UCX/HUB expects
            // all endpoints are deconfigured prior to completion of the reset,
            // leaving the device in the enabled state.
            //
            for (i = 2; i < MAX_DEVICE_CONTEXT_INDEX; i++) {

                if (usbDeviceData->EndpointHandle[i] != NULL) {

                    Endpoint_Disable(usbDeviceData->EndpointHandle[i], FALSE);
                }
            }

            //
            // After a reset the Slot Context and the Endpoint 0
            // Context are the only contexts in use.
            //
            addContextFlags.AsUlong32 = 0;
            addContextFlags.A0 = 1;
            addContextFlags.A1 = 1;
            usbDeviceData->ContextEntriesInUse = addContextFlags.AsUlong32;

            status = STATUS_SUCCESS;

        } else if (Crb->CompletionCode == TRB_COMPLETION_CODE_SUCCESS) {

            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p device reset succeeded",
                    Crb->SlotId, usbDeviceData->UcxUsbDevice);

            //
            // On some controllers, it has been observed that the default endpoint went
            // to disabled state after a device reset. This assert is placed here to catch
            // that.
            //
            if(UsbDevice_GetEndpointState(usbDeviceData, 1) == ENDPOINT_STATE_DISABLED) {
                LogError(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "HW_COMPLIANCE: %d: After device reset, found default endpoint in disabled state. (%!TrbCompletionCode!, %!CrbCompletionStatus!)",
                    Crb->SlotId, (TRB_COMPLETION_CODE)Crb->CompletionCode, (CRB_COMPLETION_STATUS)CrbCompletionStatus);
                NT_ASSERTMSG("After device reset, default Endpoint is disabled", FALSE);
            }
            //
            // Deconfigure all non-default control endpoints. UCX/HUB expects
            // all endpoints are deconfigured prior to completion of the reset,
            // leaving the device in the enabled state.
            //
            for (i = 2; i < MAX_DEVICE_CONTEXT_INDEX; i++) {

                if (usbDeviceData->EndpointHandle[i] != NULL) {

                    Endpoint_Disable(usbDeviceData->EndpointHandle[i], FALSE);
                }
            }

            //
            // After a reset the Slot Context and the Endpoint 0
            // Context are the only contexts in use.
            //
            addContextFlags.AsUlong32 = 0;
            addContextFlags.A0 = 1;
            addContextFlags.A1 = 1;
            usbDeviceData->ContextEntriesInUse = addContextFlags.AsUlong32;

            status = STATUS_SUCCESS;

        } else {

            LogError(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                     "%d: HW_COMPLIANCE: Reset Device Command Failed: USB Device 0x%p, %!TrbCompletionCode!, %!CrbCompletionStatus!",
                     Crb->SlotId, usbDeviceData->UcxUsbDevice, (TRB_COMPLETION_CODE)Crb->CompletionCode, (CRB_COMPLETION_STATUS)CrbCompletionStatus);

            Controller_HwVerifierBreakIfEnabled(
                usbDeviceData->ControllerHandle,
                usbDeviceData->UcxUsbDevice,
                NULL,
                ControllerHwVerifierResetDeviceCommandFailed,
                "Reset Device Command failed",
                &Crb->Trb,
                EventTRB);

            Controller_ReportFatalError(usbDeviceData->ControllerHandle,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_USBDEVICE_RESET_FAILED,
                                        NULL,
                                        NULL,
                                        NULL);

            completeRequest = FALSE;
            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

    } FINALLY {

        //
        // Complete the WDF request object for the
        // IOCTL_INTERNAL_USB_USBDEVICE_RESET request here.
        //
        if (completeRequest) {
            wdfRequest = usbDeviceData->WdfRequest;
            usbDeviceData->WdfRequest = NULL;
            WdfRequestComplete(wdfRequest, status);
        }
    }

    return;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_TransferEventHandler(
    __in
        PUSBDEVICE_DATA     UsbDeviceData,
    __in
        PTRANSFER_EVENT_TRB TransferEventTrb
    )
/*++

Routine Description:

    The function handles a completed transfer event. Note that this function
    handles transfer events where an error occurred on the transfer.

--*/
{
    PTD_TRACKING_DATA tdTrackingData;

    LogVerbose(Controller_GetRecorderLog(UsbDeviceData->ControllerHandle), UsbDevice,
               "%d.%d: Dispatching TransferEventTrb 0x%p with handle 0x%p",
               TransferEventTrb->SlotId, TransferEventTrb->EndpointId, TransferEventTrb,
               UsbDeviceData->EndpointHandle[TransferEventTrb->EndpointId]);

    tdTrackingData = (PTD_TRACKING_DATA)&TransferEventTrb->Pointer;

    //
    // There have been cases where the endpoint has been deconfigured - due to hw issue -
    // but a transfer event TRB is still received. In that case, gracefully ignore this
    // transfer event TRB.
    //
    if (UsbDeviceData->EndpointHandle[TransferEventTrb->EndpointId]) {

        Endpoint_TransferEventHandler(TransferEventTrb,
                                      UsbDeviceData->EndpointHandle[TransferEventTrb->EndpointId]);

    } else if (tdTrackingData->EndpointType != USB_ENDPOINT_TYPE_ISOCHRONOUS) {

        LogWarning(Controller_GetRecorderLog(UsbDeviceData->ControllerHandle), UsbDevice,
                   "%d.%d: TRB 0x%p references NULL Endpoint",
                   TransferEventTrb->SlotId, TransferEventTrb->EndpointId, TransferEventTrb);
        NT_ASSERTMSG("EndpointId references NULL endpoint", 0);
    }

    return;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_DeviceNotificationEventHandler(
    __in
        PUSBDEVICE_DATA UsbDeviceData,
    __in
        PTRB            Trb
    )
/*++

Routine Description:

    This function handles a Device Notification Event TRB.

    The only expected Device Notification is a Function Wake Device
    Notification which is handled by notifying UCX that a function
    interface performed a remote wake operation.

--*/
{
    if (Trb->DeviceNotificationEvent.NotificationType ==
        DEVICE_NOTIFICATION_TYPE_FUNCTION_WAKE) {

        LogInfo(Controller_GetRecorderLog(UsbDeviceData->ControllerHandle), UsbDevice,
                "%d: Function Wake Device Notification: USB Device 0x%p Interface %d",
                UsbDeviceData->SlotId, UsbDeviceData->UcxUsbDevice,
                Trb->DeviceNotificationEventFunctionWake.Interface);

        UcxUsbDeviceRemoteWakeNotification(UsbDeviceData->UcxUsbDevice,
                                           Trb->DeviceNotificationEventFunctionWake.Interface);
    }

    return;
}

ULONG
UsbDevice_GetEndpointState(
    __in
        PUSBDEVICE_DATA UsbDeviceData,
    __in
        ULONG           ContextIndex
    )
{
    HANDLE              registerHandle;
    PENDPOINT_CONTEXT32 endpointContext;

    registerHandle = Controller_GetRegisterHandle(UsbDeviceData->ControllerHandle);

    if (REGISTER_CONTEXTSIZE64 == Register_GetContextSize(registerHandle)) {

        PDEVICE_CONTEXT64   deviceContext64;

        deviceContext64 = (PDEVICE_CONTEXT64)UsbDeviceData->DeviceContextBuffer->VirtualAddress;
        endpointContext = (PENDPOINT_CONTEXT32)&deviceContext64->EndpointContext[ContextIndex-1];

    } else {

        PDEVICE_CONTEXT32   deviceContext32;

        deviceContext32 = (PDEVICE_CONTEXT32)UsbDeviceData->DeviceContextBuffer->VirtualAddress;
        endpointContext = (PENDPOINT_CONTEXT32)&deviceContext32->EndpointContext[ContextIndex-1];
    }

    return endpointContext->EndpointState;
}

ULONG64
UsbDevice_GetXhciEndpointDequeuePointer(
    __in
        PUSBDEVICE_DATA UsbDeviceData,
    __in
        ULONG           ContextIndex
    )
{
    HANDLE              registerHandle;
    PENDPOINT_CONTEXT32 endpointContext;

    registerHandle = Controller_GetRegisterHandle(UsbDeviceData->ControllerHandle);

    if (REGISTER_CONTEXTSIZE64 == Register_GetContextSize(registerHandle)) {

        PDEVICE_CONTEXT64   deviceContext64;

        deviceContext64 = (PDEVICE_CONTEXT64)UsbDeviceData->DeviceContextBuffer->VirtualAddress;
        endpointContext = (PENDPOINT_CONTEXT32)&deviceContext64->EndpointContext[ContextIndex-1];

    } else {

        PDEVICE_CONTEXT32   deviceContext32;

        deviceContext32 = (PDEVICE_CONTEXT32)UsbDeviceData->DeviceContextBuffer->VirtualAddress;
        endpointContext = (PENDPOINT_CONTEXT32)&deviceContext32->EndpointContext[ContextIndex-1];
    }

    return endpointContext->DequeuePointer;
}

VOID
UsbDevice_ReconfigureEndpoint(
    __in
        PUSBDEVICE_DATA                 UsbDeviceData,
    __in
        HANDLE                          Endpoint,
    __in
        PUSBDEVICE_COMPLETION_ROUTINE   CompletionRoutine,
    __in
        PVOID                           CompletionContext
    )
/*++

Routine Description:

    This routine issues a TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND for the
    single specified endpoint for the primary purpose of enabling or
    disabling the non-default streams in the Endpoint Context / Stream
    Context Array.

Arguments:

Return Value:

    None directly from this routine.

    The completion status to the CompletionRoutine, which may be completed asynchronously.

--*/
{
    HANDLE                  registerHandle;
    HANDLE                  commandHandle;
    PINPUT_CONTEXT64        inputContext;
    PSLOT_CONTEXT64         slotContext;
    PENDPOINT_CONTEXT64     endpointContext;
    ULONG                   deviceContextIndex;
    ULONG                   i;
    NTSTATUS                status;

    TRY {

        registerHandle = Controller_GetRegisterHandle(UsbDeviceData->ControllerHandle);
        NT_ASSERT(registerHandle != NULL);

        commandHandle = Controller_GetCommandHandle(UsbDeviceData->ControllerHandle);
        NT_ASSERT(commandHandle != NULL);

        LogInfo(Controller_GetRecorderLog(UsbDeviceData->ControllerHandle), UsbDevice,
                "%u: Reconfiguring Endpoint %u",
                UsbDeviceData->SlotId, Endpoint_GetContextIndex(Endpoint));

        //
        // Initialize the Input Device Context.
        //
        // See section 6.2.5 Input Context in the XHCI specification.
        // See also section 4.6.6 Configure Endpoint
        //
        RtlZeroMemory(UsbDeviceData->InputDeviceContextBuffer->VirtualAddress,
                      UsbDeviceData->InputDeviceContextBuffer->Size);

        inputContext = (PINPUT_CONTEXT64)UsbDeviceData->InputDeviceContextBuffer->VirtualAddress;

        if (REGISTER_CONTEXTSIZE64 == Register_GetContextSize(registerHandle)) {

            slotContext     = UsbDevice_InputContext64GetSlot(inputContext);

        } else {

            PINPUT_CONTEXT32    inputContext32 = (PINPUT_CONTEXT32)inputContext;

            slotContext     = (PSLOT_CONTEXT64)UsbDevice_InputContext32GetSlot(inputContext32);
        }

        //
        // 4.6.6 Configure Endpoint
        //
        // The Add Context flag A1 and Drop Context flags D0 and D1 of
        // the Input Control Context (in the Input Context) shall be
        // cleared to '0'. Endpoint 0 Context does not apply to the
        // Configure Endpoint Command and shall be ignored by the xHC.
        // A0 shall be set to '1' and refer to section 6.2.2.2 for the
        // Slot Context fields used by the Configure Endpoint Command.
        // The state of the remaining Add Context and Drop Context flags
        // depend on the specific endpoints affected by the command.
        //
        inputContext->InputControlContext.AddContextFlags.A0 = 1;

        deviceContextIndex = Endpoint_GetContextIndex(Endpoint);

        if (REGISTER_CONTEXTSIZE64 == Register_GetContextSize(registerHandle)) {

            endpointContext = UsbDevice_InputContext64GetEndpoint(
                inputContext,
                deviceContextIndex);

        } else {

            endpointContext = (PENDPOINT_CONTEXT64)UsbDevice_InputContext32GetEndpoint(
                (PINPUT_CONTEXT32)inputContext,
                deviceContextIndex);
        }

        //
        // Initialize the Input Endpoint Context.
        //
        UsbDevice_InitializeEndpointContext(UsbDeviceData, Endpoint, endpointContext);

        //
        // 6.2.2.2 Configure Endpoint Command Usage
        //
        // A 'valid' Input Slot Context for a Configure Endpoint Command
        // requires the Context Entries field to be initialized to the
        // index of the last valid Endpoint Context that is defined by
        // the target configuration.
        //
        for (i = 0; i < 32; i++) {
            if (UsbDeviceData->ContextEntriesInUse & (1 << i)) {
                slotContext->ContextEntries = i;
            }
        }

        inputContext->InputControlContext.AddContextFlags.AsUlong32 |=
            (1 << deviceContextIndex);

        inputContext->InputControlContext.DropContextFlags.AsUlong32 |=
            (1 << deviceContextIndex);

        //
        // Disable it for now.
        //
#if 0
        if (Register_GetConfigurationInformation(registerHandle)) {
            inputContext->InputControlContext.ConfigurationValue = UsbDeviceData->ConfigurationValue;
            inputContext->InputControlContext.AlternateSetting = UsbDeviceData->AlternateSetting;
            inputContext->InputControlContext.InterfaceNumber = UsbDeviceData->InterfaceNumber;
        }
#endif

        //
        // Build and issue the TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND to the
        // controller.
        //
        RtlZeroMemory(&UsbDeviceData->Crb, sizeof(UsbDeviceData->Crb));
        UsbDeviceData->Crb.CommandCallback = UsbDevice_ConfigureEndpointCompletion;
        UsbDeviceData->Crb.ObjectHandle = (HANDLE)UsbDeviceData;

        UsbDeviceData->Crb.Trb.ConfigureEndpointCommand.Type = TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND;
        UsbDeviceData->Crb.Trb.ConfigureEndpointCommand.Deconfigure = 0;
        UsbDeviceData->Crb.Trb.ConfigureEndpointCommand.SlotId = UsbDeviceData->SlotId;
        UsbDeviceData->Crb.Trb.ConfigureEndpointCommand.InputContextPointer =
            (ULONG64)UsbDeviceData->InputDeviceContextBuffer->LogicalAddress.QuadPart;
        UsbDeviceData->Crb.InputContextVA = inputContext;

        UsbDeviceData->CrbCompletionRoutine = CompletionRoutine;
        UsbDeviceData->CrbCompletionContext = CompletionContext;

        Command_SendCommand(commandHandle, &UsbDeviceData->Crb);

        status = STATUS_SUCCESS;

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            UsbDeviceData->CrbCompletionRoutine = NULL;
            UsbDeviceData->CrbCompletionContext = NULL;

            (*CompletionRoutine)(CompletionContext, status);
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_ConfigureEndpointCompletion(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This routine is the TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND command
    completion routine used by UsbDevice_ReconfigureEndpoint().

Arguments:

    EventTRB - Used for logging Hardware verifier events

Return Value:

    None directly from this routine.

    The completion status to the Crb CompletionRoutine.

--*/
{
    NTSTATUS                        status;
    PUSBDEVICE_DATA                 usbDeviceData;
    PUSBDEVICE_COMPLETION_ROUTINE   completionRoutine;
    PVOID                           completionContext;

    UNREFERENCED_PARAMETER(EventTRB);

    TRY {

        usbDeviceData = (PUSBDEVICE_DATA)Crb->ObjectHandle;

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the configure
            // endpoint command effectively failed.
            //
            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p - controller gone - configured endpoint failed",
                    usbDeviceData->SlotId, usbDeviceData->UcxUsbDevice);

            status = STATUS_UNSUCCESSFUL;

        } else if (Crb->CompletionCode == TRB_COMPLETION_CODE_SUCCESS) {

            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p - configure endpoint succeeded",
                    Crb->SlotId, usbDeviceData->UcxUsbDevice);

            status = STATUS_SUCCESS;

        } else {

            LogInfo(Controller_GetRecorderLog(usbDeviceData->ControllerHandle), UsbDevice,
                    "%d: USB Device 0x%p - configure endpoint failed",
                    Crb->SlotId, usbDeviceData->UcxUsbDevice);

            status = STATUS_UNSUCCESSFUL;
        }

    } FINALLY {

        completionRoutine = usbDeviceData->CrbCompletionRoutine;
        usbDeviceData->CrbCompletionRoutine = NULL;

        completionContext = usbDeviceData->CrbCompletionContext;
        usbDeviceData->CrbCompletionContext = NULL;

        (*completionRoutine)(completionContext, status);
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_AddEndpointToDeviceEndpointList(
    __in
        PUSBDEVICE_DATA     UsbDeviceData,
    __in
        UCXENDPOINT         UcxEndpoint,
    __in
        PLIST_ENTRY         EndpointListEntry
    )
{
    KIRQL   irql;

    //
    // Add a reference on the device when an endpoint is inserted into
    // the list of all endpoints that exist for this device.
    //
    WdfObjectReferenceWithTag(UsbDeviceData->UcxUsbDevice,
                              TAG(UsbDevice_AddEndpointToDeviceEndpointList));

    //
    // Add a reference on the endpoint when it is inserted into the list
    // of all endpoints that exist for this device.
    //
    WdfObjectReferenceWithTag(UcxEndpoint,
                              TAG(UsbDevice_AddEndpointToDeviceEndpointList));

    //
    // Insert this endpoint into the list of all endpoints that exist
    // for this device.
    //
    KeAcquireSpinLock(&UsbDeviceData->EndpointListLock, &irql);

    InsertTailList(&UsbDeviceData->EndpointListHead, EndpointListEntry);

    UsbDeviceData->EndpointListCount++;

    KeReleaseSpinLock(&UsbDeviceData->EndpointListLock, irql);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_RemoveEndpointFromDeviceEndpointList(
    __in
        PUSBDEVICE_DATA     UsbDeviceData,
    __in
        UCXENDPOINT         UcxEndpoint,
    __in
        PLIST_ENTRY         EndpointListEntry
    )
{
    KIRQL   irql;

    //
    // Remove this endpoint from the list of all endpoints that exist
    // for this device.
    //
    KeAcquireSpinLock(&UsbDeviceData->EndpointListLock, &irql);

    RemoveEntryList(EndpointListEntry);

    UsbDeviceData->EndpointListCount--;

    KeReleaseSpinLock(&UsbDeviceData->EndpointListLock, irql);

    //
    // Release the reference on the endpoint when it is removed from the
    // list of all endpoints that exist for this device.
    //
    WdfObjectDereferenceWithTag(UcxEndpoint,
                                TAG(UsbDevice_AddEndpointToDeviceEndpointList));

    //
    // Release the reference on the device when an endpoint is removed
    // from the list of all endpoints that exist for this device.
    //
    WdfObjectDereferenceWithTag(UsbDeviceData->UcxUsbDevice,
                                TAG(UsbDevice_AddEndpointToDeviceEndpointList));
}

ULONG
UsbDevice_GetNumContextEntries(
    __in
        PUSBDEVICE_DATA     UsbDeviceData
    )
{
    ULONG i;
    ULONG contextEntries;

    TRY {

        contextEntries = 0;

        for (i = 0; i < 32; i++) {
            if (UsbDeviceData->ContextEntriesInUse & (1 << i)) {
                contextEntries = i;
            }
        }

        //
        // There should at least be a device context entry
        //
        NT_ASSERT(contextEntries != 0);

    } FINALLY {

    }

    return contextEntries;
}
