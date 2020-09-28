/*++
Copyright (c) Microsoft Corporation

Module Name:

    endpoint.c

Abstract:

    USB XHCI Driver

--*/

#include "pch.h"

#include "endpoint.tmh"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, Endpoint_UcxEvtUsbDeviceDefaultEndpointAdd)
#pragma alloc_text(PAGE, Endpoint_UcxEvtUsbDeviceEndpointAdd)
#pragma alloc_text(PAGE, Endpoint_SetType)
#pragma alloc_text(PAGE, Endpoint_SetContextIndex)
#pragma alloc_text(PAGE, Endpoint_Create)
#endif

extern PSTATE_ENTRY ESMStateTable[];

NTSTATUS
Endpoint_UcxEvtUsbDeviceDefaultEndpointAdd(
    UCXCONTROLLER       UcxController,
    UCXUSBDEVICE        UcxUsbDevice,
    ULONG               MaxPacketSize,
    PUCXENDPOINT_INIT   UcxEndpointInit
    )
/*++

Routine Description:

    This function creates the default endpoint.

--*/
{
    NTSTATUS                                status;
    UCX_DEFAULT_ENDPOINT_EVENT_CALLBACKS    ucxDefaultEndpointEventCallbacks;
    USB_ENDPOINT_DESCRIPTOR                 usbEndpointDescriptor;

    PAGED_CODE();

    TRY {

        LogInfo(GetControllerData(UcxController)->RecorderLog, Endpoint,
                "%d.1: MaxPacketSize %d",
                UsbDevice_GetSlotId(GetUsbDeviceData(UcxUsbDevice)), MaxPacketSize);

        //
        // The default control endpoint does not have a usb endpoint descriptor on
        // the device. This code creates its own usb endpoint descriptor describing
        // the default control endpoint. This is done to allow code reuse throughout
        // the endpoint object.
        //
        RtlZeroMemory(&usbEndpointDescriptor, sizeof(USB_ENDPOINT_DESCRIPTOR));
        usbEndpointDescriptor.bLength = sizeof(usbEndpointDescriptor);
        usbEndpointDescriptor.bDescriptorType = USB_ENDPOINT_DESCRIPTOR_TYPE;
        usbEndpointDescriptor.wMaxPacketSize = (USHORT)MaxPacketSize;

        UCX_DEFAULT_ENDPOINT_EVENT_CALLBACKS_INIT(&ucxDefaultEndpointEventCallbacks,
                                                  Endpoint_UcxEvtEndpointPurge,
                                                  Endpoint_UcxEvtEndpointStart,
                                                  Endpoint_UcxEvtEndpointAbort,
                                                  Endpoint_UcxEvtEndpointOkToCancelTransfers,
                                                  Endpoint_UcxEvtDefaultEndpointUpdate,
                                                  Endpoint_UcxEvtEndpointEnableForwardProgress);

        UcxDefaultEndpointInitSetEventCallbacks(UcxEndpointInit, &ucxDefaultEndpointEventCallbacks);

        status = Endpoint_Create(UcxController,
                                 UcxUsbDevice,
                                 UcxEndpointInit,
                                 &usbEndpointDescriptor,
                                 NULL,
                                 NULL);

        if (!NT_SUCCESS(status)) {

            LogError(GetControllerData(UcxController)->RecorderLog, Endpoint,
                     "%d.1: Endpoint_Create Failed %!STATUS!",
                     UsbDevice_GetSlotId(GetUsbDeviceData(UcxUsbDevice)), status);
            LEAVE;
        }

    } FINALLY {

    }

    return status;
}

NTSTATUS
Endpoint_UcxEvtUsbDeviceEndpointAdd(
    UCXCONTROLLER                                               UcxController,
    UCXUSBDEVICE                                                UcxUsbDevice,
    PUSB_ENDPOINT_DESCRIPTOR                                    UsbEndpointDescriptor,
    ULONG                                                       UsbEndpointDescriptorBufferLength,
    PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR               SuperSpeedEndpointCompanionDescriptor,
    PUCXENDPOINT_INIT                                           UcxEndpointInit
    )
/*++

Routine Description:

    This function creates an endpoint.

--*/
{
    NTSTATUS                                                 status;
    UCX_ENDPOINT_EVENT_CALLBACKS                             ucxEndpointEventCallbacks;
    PUSB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR  superSpeedPlusIsochEndpointCompanionDescriptor;

    UNREFERENCED_PARAMETER(UsbEndpointDescriptorBufferLength);

    PAGED_CODE();

    TRY {

        UCX_ENDPOINT_EVENT_CALLBACKS_INIT(&ucxEndpointEventCallbacks,
                                          Endpoint_UcxEvtEndpointPurge,
                                          Endpoint_UcxEvtEndpointStart,
                                          Endpoint_UcxEvtEndpointAbort,
                                          Endpoint_UcxEvtEndpointReset,
                                          Endpoint_UcxEvtEndpointOkToCancelTransfers,
                                          Endpoint_UcxEvtEndpointStaticStreamsAdd,
                                          Endpoint_UcxEvtEndpointStaticStreamsEnable,
                                          Endpoint_UcxEvtEndpointStaticStreamsDisable,
                                          Endpoint_UcxEvtEndpointEnableForwardProgress);

        UcxEndpointInitSetEventCallbacks(UcxEndpointInit, &ucxEndpointEventCallbacks);

        superSpeedPlusIsochEndpointCompanionDescriptor = NULL;

        if ((SuperSpeedEndpointCompanionDescriptor != NULL) &&
            ((UsbEndpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK) == USB_ENDPOINT_TYPE_ISOCHRONOUS) &&
            (SuperSpeedEndpointCompanionDescriptor->bmAttributes.Isochronous.SspCompanion != 0)) {

            superSpeedPlusIsochEndpointCompanionDescriptor =
                (PUSB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR)
                 ((PCHAR)SuperSpeedEndpointCompanionDescriptor + SuperSpeedEndpointCompanionDescriptor->bLength);

        }

        status = Endpoint_Create(UcxController,
                                 UcxUsbDevice,
                                 UcxEndpointInit,
                                 UsbEndpointDescriptor,
                                 SuperSpeedEndpointCompanionDescriptor,
                                 superSpeedPlusIsochEndpointCompanionDescriptor);

        if (!NT_SUCCESS(status)) {

            LogError(GetControllerData(UcxController)->RecorderLog, Endpoint,
                     "%d.%d: Endpoint_Create Failed %!STATUS!",
                     UsbDevice_GetSlotId(GetUsbDeviceData(UcxUsbDevice)),
                     EndpointAddressToDCI(UsbEndpointDescriptor->bEndpointAddress),
                     status);
            LEAVE;
        }

    } FINALLY {

    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Endpoint_SetType(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    PAGED_CODE();

    //
    // The endpoint type used is derived from the xHCI specs, which has
    // a unique value for each type and direction. The type is stored
    // in the lower two bits of the bmAttributes in the endpoint descriptor.
    //
    switch (EndpointData->UsbEndpointDescriptor.bmAttributes & USB_ENDPOINT_TYPE_MASK) {

    case USB_ENDPOINT_TYPE_ISOCHRONOUS:
        EndpointData->Type = EndpointType_IsochOut;
        break;

    case USB_ENDPOINT_TYPE_BULK:
        EndpointData->Type = EndpointType_BulkOut;
        break;

    case USB_ENDPOINT_TYPE_INTERRUPT:
        EndpointData->Type = EndpointType_InterruptOut;
        break;

    case USB_ENDPOINT_TYPE_CONTROL:
    default:
        EndpointData->Type = EndpointType_NotValid;
        break;
    }

    //
    // The endpoint direction is stored in bit 7 of bEndpointAddress in
    // the endpoint descriptor. The xHCI endpoint type has unique values
    // based on the type and direction. The endpoint type value is +4 from
    // out to in endpoints of the same type. For the control endpoint, it is
    // +4 from the not valid endpoint type.
    //
    if ((USB_ENDPOINT_DIRECTION_IN(EndpointData->UsbEndpointDescriptor.bEndpointAddress)) ||
        (EndpointData->Type == EndpointType_NotValid)) {

        EndpointData->Type += 4;
    }

    return;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Endpoint_SetContextIndex(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    PAGED_CODE();

    //
    // See section 4.5.1 Device Context Index
    //
    // The range of DCI values is 0 to 31.
    // The DCI of the Slot Context is 0.
    // For Device Context Indices 1-31, the following rules apply:
    //
    // 1) For Isoch, Interrupt, or Bulk type endpoints the DCI is
    //    calculated from the Endpoint Number and Direction with the
    //    following formula:
    //
    //        DCI = (Endpoint Number * 2) + Direction
    //
    //    where Direction = '0' for OUT endpoints and '1' for IN endpoints.
    //
    // 2) For Control type endpoints:
    //
    //        DCI = (Endpoint Number * 2) + 1
    //
    switch (Endpoint_GetType(EndpointData)) {

    case EndpointType_IsochOut:
    case EndpointType_BulkOut:
    case EndpointType_InterruptOut:
        EndpointData->ContextIndex = Endpoint_GetEndpointNumber(EndpointData) * 2;
        break;

    case EndpointType_Control:
    case EndpointType_IsochIn:
    case EndpointType_BulkIn:
    case EndpointType_InterruptIn:
    default:
        EndpointData->ContextIndex = Endpoint_GetEndpointNumber(EndpointData) * 2 + 1;
        break;
    }

    NT_ASSERT((EndpointData->ContextIndex >= 1) && (EndpointData->ContextIndex <= 31));
    return;
}

_Must_inspect_result_
NTSTATUS
Endpoint_CreateClearStallContext(
    __in
        PENDPOINT_DATA  EndpointData
    )
/*++

Routine Description:

    This function is called for controllers that need the workaround applied which calls
    ConfigureEndpoint instead of SetTRDQPointer after StopEndpoint.  This routine allocates
    everything necessary to send a ClearEndpointStall control request to the device in order
    to reset its sequence number.

--*/
{
    NTSTATUS                status;
    PCLEAR_STALL_CONTEXT    clearStallContext;
    WDF_OBJECT_ATTRIBUTES   attributes;

    TRY {

        clearStallContext = ExAllocatePoolWithTag(NonPagedPool,
                                                  sizeof(CLEAR_STALL_CONTEXT),
                                                  DriverAllocationTag);
        if (clearStallContext == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        RtlZeroMemory(clearStallContext,
                      sizeof(CLEAR_STALL_CONTEXT));

        clearStallContext->Irp = IoAllocateIrp(CLEAR_STALL_IRP_STACK_SIZE, FALSE);
        if (clearStallContext->Irp == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&attributes,
                                                REQUEST_DATA);
        
        status = WdfRequestCreate(&attributes,
                                  NULL,
                                  &clearStallContext->Request);
        if (!NT_SUCCESS(status)) {
            LEAVE;
        }
            
    } FINALLY {

        if (!NT_SUCCESS(status)) {
        
            if (clearStallContext) {
                if (clearStallContext->Irp) {
                    IoFreeIrp(clearStallContext->Irp);
                    clearStallContext->Irp = NULL;
                }

                if (clearStallContext->Request) {
                    WdfObjectDelete(clearStallContext->Request);
                    clearStallContext->Request = NULL;
                }
                ExFreePoolWithTag(clearStallContext,DriverAllocationTag);

            }
        } else {
            EndpointData->ClearStallContext = clearStallContext;
        }

    }

    return status;
}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Endpoint_Create(
    __in
        UCXCONTROLLER               UcxController,
    __in
        UCXUSBDEVICE                UcxUsbDevice,
    __in
        PUCXENDPOINT_INIT           UcxEndpointInit,
    __in
        PUSB_ENDPOINT_DESCRIPTOR    UsbEndpointDescriptor,
    __in_opt
        PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR
                                    UsbSuperSpeedEndpointCompanionDescriptor,
    __in_opt
        PUSB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR
                                    UsbSuperSpeedPlusIsochEndpointCompanionDescriptor
    )
/*++

Routine Description:

    This function creates a UCX endpoint object.

--*/
{
    NTSTATUS                    status;
    HANDLE                      controllerHandle;
    WDF_OBJECT_ATTRIBUTES       wdfAttributes;
    UCXENDPOINT                 ucxEndpoint;
    PENDPOINT_DATA              endpointData;
    WDF_TIMER_CONFIG            wdfTimerConfig;

    PAGED_CODE();

    TRY {

        controllerHandle = GetControllerData(UcxController);

        LogInfo(Controller_GetRecorderLog(controllerHandle), Endpoint,
                "%d.%d: UsbEndpointDescriptor 0x%p Address 0x%02x Type %d MaxPacketSize %d %!FUNC!",
                UsbDevice_GetSlotId(GetUsbDeviceData(UcxUsbDevice)),
                EndpointAddressToDCI(UsbEndpointDescriptor->bEndpointAddress),
                UsbEndpointDescriptor,
                UsbEndpointDescriptor->bEndpointAddress,
                UsbEndpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK,
                UsbEndpointDescriptor->wMaxPacketSize & USB_ENDPOINT_MAXPACKETSIZE_MASK);

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, ENDPOINT_DATA);

        wdfAttributes.EvtCleanupCallback = Endpoint_EvtEndpointCleanupCallback;

        status = UcxEndpointCreate(UcxUsbDevice,
                                   &UcxEndpointInit,
                                   &wdfAttributes,
                                   &ucxEndpoint);

        if (!NT_SUCCESS(status)) {
            LogError(Controller_GetRecorderLog(controllerHandle), Endpoint,
                     "%d.%d: UcxEndpointCreate Failed %!STATUS!",
                     UsbDevice_GetSlotId(GetUsbDeviceData(UcxUsbDevice)),
                     EndpointAddressToDCI(UsbEndpointDescriptor->bEndpointAddress),
                     status);
            LEAVE;
        }

        LogInfo(Controller_GetRecorderLog(controllerHandle), Endpoint,
                "%d.%d: ucxEndpoint 0x%p created",
                UsbDevice_GetSlotId(GetUsbDeviceData(UcxUsbDevice)),
                EndpointAddressToDCI(UsbEndpointDescriptor->bEndpointAddress),
                ucxEndpoint);

        endpointData = GetEndpointData(ucxEndpoint);

        KeInitializeEvent(&endpointData->ControllerResetCompleteAcknowledgeEvent,
                          NotificationEvent,
                          FALSE);

        //
        // Create the timer required for the State Machine.
        //
        WDF_TIMER_CONFIG_INIT(&wdfTimerConfig, Endpoint_WdfEvtStateMachineTimer);

        WDF_OBJECT_ATTRIBUTES_INIT(&wdfAttributes);
        wdfAttributes.ParentObject = ucxEndpoint;

        status = WdfTimerCreate(&wdfTimerConfig, &wdfAttributes, &endpointData->SmContext.Timer);

        if (!NT_SUCCESS(status)) {
            LogError(Controller_GetRecorderLog(controllerHandle), Endpoint,
                     "%d.%d: WdfTimerCreate (SmContext.Timer) Failed %!STATUS!",
                     UsbDevice_GetSlotId(GetUsbDeviceData(UcxUsbDevice)),
                     EndpointAddressToDCI(UsbEndpointDescriptor->bEndpointAddress),
                     status);
            LEAVE;
        }

        endpointData->SmContext.WorkItem =
            IoAllocateWorkItem(Controller_GetWdmDeviceObject(controllerHandle));

        if (endpointData->SmContext.WorkItem == NULL) {
            LogError(Controller_GetRecorderLog(controllerHandle), Endpoint,
                     "%d.%d: IoAllocateWorkItem (SmContext.Timer) Failed",
                     UsbDevice_GetSlotId(GetUsbDeviceData(UcxUsbDevice)),
                     EndpointAddressToDCI(UsbEndpointDescriptor->bEndpointAddress));
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        endpointData->SmContext.CurrentState[0].EsmState = StartIndexEsm;
        endpointData->SmContext.CurrentStateDepth = 0;
        endpointData->SmContext.StateTable = ESMStateTable;
        endpointData->SmContext.StartIndex = StartIndexEsm;
        endpointData->SmContext.ParentContext = endpointData;

        //
        // The context associated with the UCX endpoint object points to
        // the endpoint data stored with the WDF queue.
        //

        endpointData->ControllerHandle    = controllerHandle;
        endpointData->UcxUsbDevice        = UcxUsbDevice;
        endpointData->UsbDeviceHandle     = GetUsbDeviceData(UcxUsbDevice);
        endpointData->StaticStreamsData   = NULL;
        endpointData->ClearStallContext   = NULL;

        RtlCopyMemory(&endpointData->UsbEndpointDescriptor,
                      UsbEndpointDescriptor,
                      sizeof(endpointData->UsbEndpointDescriptor));

        if (UsbSuperSpeedEndpointCompanionDescriptor != NULL) {

            RtlCopyMemory(&endpointData->UsbSuperSpeedEndpointCompanionDescriptor,
                          UsbSuperSpeedEndpointCompanionDescriptor,
                          sizeof(endpointData->UsbSuperSpeedEndpointCompanionDescriptor));
        }

        if (UsbSuperSpeedPlusIsochEndpointCompanionDescriptor != NULL) {

            RtlCopyMemory(&endpointData->UsbSuperSpeedPlusIsochEndpointCompanionDescriptor,
                          UsbSuperSpeedPlusIsochEndpointCompanionDescriptor,
                          sizeof(endpointData->UsbSuperSpeedPlusIsochEndpointCompanionDescriptor));
        }

        Endpoint_SetType(endpointData);
        Endpoint_SetContextIndex(endpointData);

        //
        // Only SuperSpeed Bulk endpoints with a non-zero MaxStreams value in their
        // SuperSpeed Endpoint Companion descriptor are streams capable endpoints.
        //
        if ((UsbDevice_GetDeviceSpeed(endpointData->UsbDeviceHandle) == UsbSuperSpeed) &&
            (Endpoint_GetTransferType(endpointData) == USB_ENDPOINT_TYPE_BULK) &&
            (Endpoint_GetMaxStreams(endpointData) > 0)) {

            endpointData->StreamsCapable = TRUE;

            if (Controller_GetDeviceFlags(endpointData->ControllerHandle).OnStopConfigureStreamEndpoint) {
                status = Endpoint_CreateClearStallContext(endpointData);
                if (!NT_SUCCESS(status)) {

                    LogError(Controller_GetRecorderLog(controllerHandle), Endpoint,
                             "%d.%d: Endpoint_CreateClearStallContext Failed %!STATUS!",
                             UsbDevice_GetSlotId(GetUsbDeviceData(UcxUsbDevice)),
                             Endpoint_GetContextIndex(endpointData), status);
                    LEAVE;
                }
            }

        } else {

            endpointData->StreamsCapable = FALSE;
        }

        //
        // Create a transfer ring for the endpoint.
        //
        status = TR_Create(controllerHandle,
                           endpointData,
                           ucxEndpoint,
                           endpointData->StreamsCapable ? 1 : 0,
                           &endpointData->TransferRingHandle);

        if (!NT_SUCCESS(status)) {
            LogError(Controller_GetRecorderLog(controllerHandle), Endpoint,
                     "%d.%d: TR_Create Failed %!STATUS!",
                     UsbDevice_GetSlotId(GetUsbDeviceData(UcxUsbDevice)),
                     Endpoint_GetContextIndex(endpointData), status);
            LEAVE;
        }

        //
        // Below operations must only be done on success.
        //
        UcxEndpointSetWdfIoQueue(ucxEndpoint, TR_GetWdfQueue(endpointData->TransferRingHandle));

        //
        // If this is the default control endpoint, then tell the usb device of
        // the endpoint's handle.
        //
        if (endpointData->ContextIndex == DEFAULT_ENDPOINT_CONTEXT_INDEX) {

            UsbDevice_SetEndpointHandle(endpointData->UsbDeviceHandle,
                                        endpointData->ContextIndex,
                                        endpointData);
        }

        //
        // UcxEndpoint field is set last as it is the field that is checked by
        // Endpoint_EvtEndpointCleanupCallback() to determine whether the ENDPOINT_DATA
        // is completely initialized. If it is not set the cleanup is skipped.
        //
        endpointData->UcxEndpoint = ucxEndpoint;

        //
        // Insert this endpoint into the list of all endpoints that
        // exist for this device.
        //
        UsbDevice_AddEndpointToDeviceEndpointList(endpointData->UsbDeviceHandle,
            ucxEndpoint,
            &endpointData->EndpointListEntry);

        Etw_EndpointCreate(NULL, endpointData);

    } FINALLY {

    }

    return status;
}

VOID
Endpoint_EvtEndpointCleanupCallback(
    WDFOBJECT       UcxEndpoint
    )
/*++

Routine Description:

    The framework calls the callback function when either the framework
    or a driver attempts to delete the object.

--*/
{
    PENDPOINT_DATA  endpointData;

    TRY {

        endpointData = GetEndpointData(UcxEndpoint);

        if (endpointData->UcxEndpoint != UcxEndpoint) {

            //
            // Something may have failed after UcxEndpointCreate() and
            // Endpoint_Create() initialization did not complete.
            //
            LEAVE;
        }

        LogInfo(Controller_GetRecorderLog(endpointData->ControllerHandle), Endpoint,
                "%d.%d: UcxEndpoint 0x%p, EndpointData 0x%p, UsbDeviceHandle 0x%p",
                UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(endpointData), endpointData->UcxEndpoint,
                endpointData, endpointData->UsbDeviceHandle);

        if (endpointData->ContextIndex != DEFAULT_ENDPOINT_CONTEXT_INDEX) {

            //
            // If the endpoint handle is cached in the device data
            // endpoint handle array then clear the endpoint handle from
            // the array.
            //
            if (endpointData == UsbDevice_GetEndpointHandle(endpointData->UsbDeviceHandle, endpointData->ContextIndex)) {

                UsbDevice_ClearEndpointHandle(endpointData->UsbDeviceHandle,
                                              endpointData->ContextIndex,
                                              endpointData);
            }
        }

        if (endpointData->ClearStallContext) {

            if (endpointData->ClearStallContext->Irp) {
                IoFreeIrp(endpointData->ClearStallContext->Irp);
                endpointData->ClearStallContext->Irp = NULL;
            }

            if (endpointData->ClearStallContext->Request) {
                WdfObjectDelete(endpointData->ClearStallContext->Request);
                endpointData->ClearStallContext->Request = NULL;
            }

            ExFreePoolWithTag(endpointData->ClearStallContext,DriverAllocationTag);
        }

        if (endpointData->SmContext.WorkItem != NULL) {
            IoFreeWorkItem(endpointData->SmContext.WorkItem);
            endpointData->SmContext.WorkItem = NULL;
        }

        //
        // Remove this endpoint from the list of all endpoints that
        // exist for this device.
        //
        UsbDevice_RemoveEndpointFromDeviceEndpointList(endpointData->UsbDeviceHandle,
            endpointData->UcxEndpoint,
            &endpointData->EndpointListEntry);

        Etw_EndpointDelete(NULL, endpointData);

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_SetLogIdentifier(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    CHAR logIdentifier[RECORDER_LOG_IDENTIFIER_MAX_CHARS + sizeof(NULL)];

    if (EndpointData->RecorderLog) {

        RtlStringCchPrintfA((PCHAR)&logIdentifier,
                            sizeof(logIdentifier),
                            "%02d SLT%02d DCI%02d",
                            Controller_GetInstanceNumber(EndpointData->ControllerHandle),
                            UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                            EndpointData->ContextIndex);

        WppRecorderLogSetIdentifier(EndpointData->RecorderLog, (PSTR)&logIdentifier);
    }

    return;
}

_Must_inspect_result_
__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Endpoint_Enable(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    NTSTATUS                    status;
    HANDLE                      commonBufferHandle;
    UCXSSTREAMS                 ucxStaticStreams;
    PSTATIC_STREAMS_DATA        staticStreamsData;
    PSTREAM_CONTEXT             streamContext;
    ULONG                       streamContextArraySize;
    ULONG                       i;
    RECORDER_LOG_CREATE_PARAMS  recorderLogCreateParams;

    TRY {

        NT_ASSERT(EndpointData->ControllerHandle != NULL);
        NT_ASSERT(EndpointData->TransferRingHandle != NULL);

        commonBufferHandle =
            Controller_GetCommonBufferHandle(EndpointData->ControllerHandle);

        LogInfo(Controller_GetRecorderLog(EndpointData->ControllerHandle), Endpoint,
                "%d.%d: UcxEndpoint 0x%p", UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(EndpointData), EndpointData->UcxEndpoint);

        //
        // It is possible that client driver disabled streams while the endpoint
        // is in the disabled state (e.g. while controller reset recovery is in
        // progress). For this reason, rely on UCX to know whether streams are
        // currently enabled for this endpoint.
        //
        ucxStaticStreams =
            UcxEndpointGetStaticStreamsReferenced(EndpointData->UcxEndpoint,
                                                  TAG(Endpoint_Enable));

        if (EndpointData->StreamsCapable) {

            if (ucxStaticStreams == NULL) {

                //
                // If endpoint is streams capable, but client driver has not enabled
                // streams on this endpoint yet, configure stream with Id=1 as default
                // stream. If forward progress is enabled, resources for this default
                // stream may already be allocated.
                //
                if (EndpointData->DefaultStaticStreamsData != NULL) {

                    staticStreamsData = EndpointData->DefaultStaticStreamsData;

                } else {

                    EndpointData->DefaultStaticStreamsData = ExAllocatePoolWithTag(NonPagedPool,
                                                                            sizeof(STATIC_STREAMS_DATA),
                                                                            DriverAllocationTag);

                    if (EndpointData->DefaultStaticStreamsData == NULL) {
                        status = STATUS_INSUFFICIENT_RESOURCES;
                        LEAVE;
                    }

                    EndpointData->StaticStreamsData = EndpointData->DefaultStaticStreamsData;
                    staticStreamsData = EndpointData->DefaultStaticStreamsData;

                    RtlZeroMemory(staticStreamsData, sizeof(STATIC_STREAMS_DATA));

                    staticStreamsData->EndpointData         = EndpointData;
                    staticStreamsData->NumberOfStreams      = 1;
                    staticStreamsData->MaxPrimaryStreams    = 1;

                    //
                    // StreamHandles must be setup as soon as EndpointData->StaticStreamsData and
                    // EndpointData->NumberOfStreams are set, as the reset of the code depends on
                    // that.
                    //
                    Endpoint_SetStreamHandle(EndpointData, 1, EndpointData->TransferRingHandle);

                    streamContextArraySize = (1ULL << (staticStreamsData->MaxPrimaryStreams + 1)) *
                                             sizeof(STREAM_CONTEXT);

                    staticStreamsData->StreamContextArray =
                        CommonBuffer_AcquireBuffer(commonBufferHandle,
                                                   streamContextArraySize,
                                                   EndpointData,
                                                   ENDPOINT_ALLOC_TAG1);

                    if (staticStreamsData->StreamContextArray == NULL) {
                        status = STATUS_INSUFFICIENT_RESOURCES;
                        LEAVE;
                    }
                }

                //
                // Enable the default stream transfer ring.
                //
                status = TR_Enable(EndpointData->TransferRingHandle);

                if (!NT_SUCCESS(status)) {

                    LogError(Controller_GetRecorderLog(EndpointData->ControllerHandle), Endpoint,
                             "%d.%d.0: TR_Enable Failed %!STATUS!",
                             UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                             Endpoint_GetContextIndex(EndpointData), status);
                    LEAVE;
                }

                //
                // Initialize the default Stream Context Array entry to
                // point to the transfer ring for the default stream.
                //
                streamContext = Endpoint_GetStreamContext(EndpointData, 1);
                streamContext->DequeuePointer = TR_GetDequeuePointer(EndpointData->TransferRingHandle);

            } else {

                staticStreamsData = GetStaticStreamsData(ucxStaticStreams);

                NT_ASSERT(EndpointData->StaticStreamsData == NULL);
                EndpointData->StaticStreamsData = staticStreamsData;

                if (staticStreamsData->StreamContextArray == NULL) {

                    //
                    // The Primary Stream Array contains 2**(MaxPStreams+1)
                    // entries.  Calculate that size.
                    //
                    streamContextArraySize = (1ULL << (staticStreamsData->MaxPrimaryStreams + 1)) *
                                             sizeof(STREAM_CONTEXT);

                    staticStreamsData->StreamContextArray =
                        CommonBuffer_AcquireBuffer(commonBufferHandle,
                                                   streamContextArraySize,
                                                   EndpointData,
                                                   ENDPOINT_ALLOC_TAG1);

                    if (staticStreamsData->StreamContextArray == NULL) {
                        status = STATUS_INSUFFICIENT_RESOURCES;
                        LEAVE;
                    }
                }

                for (i = 1; i <= staticStreamsData->NumberOfStreams; i += 1) {

                    //
                    // Enable the non-default stream transfer ring.
                    //
                    status = TR_Enable(Endpoint_GetStreamHandle(EndpointData, i));

                    if (!NT_SUCCESS(status)) {

                        LogError(Controller_GetRecorderLog(EndpointData->ControllerHandle), Endpoint,
                                 "%d.%d.%d: TR_Enable Failed %!STATUS!",
                                 UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                                 Endpoint_GetContextIndex(EndpointData), i+1, status);
                        LEAVE;
                    }

                    //
                    // Initialize the Stream Context Array entry for the
                    // non-default stream to point to the transfer ring
                    // for the non-default stream.
                    //
                    streamContext = Endpoint_GetStreamContext(EndpointData, i);
                    streamContext->DequeuePointer = Endpoint_GetDequeuePointer(EndpointData, i);
                }
            }

        } else {

            //
            // Enable the non-stream transfer ring.
            //
            status = TR_Enable(EndpointData->TransferRingHandle);

            if (!NT_SUCCESS(status)) {

                LogError(Controller_GetRecorderLog(EndpointData->ControllerHandle), Endpoint,
                         "%d.%d.0: TR_Enable Failed %!STATUS!",
                         UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                         Endpoint_GetContextIndex(EndpointData), status);
                LEAVE;
            }
        }

        //
        // Create the recorder log for the endpoint. This log is used for all logging to
        // and endpoint and its associate transfer rings. For generalized logging, the
        // recorder log for the controller is used. If the call to create a recorder log
        // for the transfer ring fails, then the controller log is used.
        //
        //
        // NOTE: this log should not be used for any logging that occurs during or prior to
        //       an enable, or during or after a disable endpoint/transfer ring.
        //

        RECORDER_LOG_CREATE_PARAMS_INIT(&recorderLogCreateParams, NULL);
#if defined (DBG)
        recorderLogCreateParams.TotalBufferSize = 8192;
        recorderLogCreateParams.ErrorPartitionSize = 200;
#else
        recorderLogCreateParams.TotalBufferSize = 1024;
        recorderLogCreateParams.ErrorPartitionSize = 200;
#endif

        RtlStringCchPrintfA(recorderLogCreateParams.LogIdentifier,
                            sizeof(recorderLogCreateParams.LogIdentifier),
                            "%02d SLT%02d DCI%02d",
                            Controller_GetInstanceNumber(EndpointData->ControllerHandle),
                            UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                            EndpointData->ContextIndex);

        if (STATUS_SUCCESS != WppRecorderLogCreate(&recorderLogCreateParams,
                                                   &EndpointData->RecorderLog)) {

            EndpointData->RecorderLog =
                Controller_GetRecorderLog(EndpointData->ControllerHandle);
        }

        status = STATUS_SUCCESS;

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            Endpoint_Disable_Internal(EndpointData, TRUE);
        } else {
            ESM_AddEsmEvent(EndpointData, EsmEventEnabled);
        }

        //
        // Release the reference to the ucxStaticStreams object acquired by
        // the call to UcxEndpointGetStaticStreamsReferenced.
        //
        if (ucxStaticStreams != NULL) {
            WdfObjectDereferenceWithTag(ucxStaticStreams, TAG(Endpoint_Enable));
        }
    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_Disable(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        BOOLEAN         OnDeviceDisable
    )
{
    NT_ASSERT(EndpointData->EndpointResetRequest == NULL);

    Endpoint_Disable_Internal(EndpointData, OnDeviceDisable);

    ESM_AddEsmEvent(EndpointData, EsmEventDisabled);
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_Disable_Internal(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        BOOLEAN         OnDeviceDisable
    )
/*++

Routine Description:

    This function is called to destroy an endpoint.

--*/
{
    HANDLE                  commonBufferHandle;
    PSTATIC_STREAMS_DATA    staticStreamsData;
    ULONG                   i;
    BOOLEAN                 freeResources;

    TRY {

        commonBufferHandle =
            Controller_GetCommonBufferHandle(EndpointData->ControllerHandle);

        LogInfo(Controller_GetRecorderLog(EndpointData->ControllerHandle), Endpoint,
                "%d.%d: UcxEndpoint 0x%p OnDeviceDisable %!bool!",
                UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(EndpointData),
                EndpointData->UcxEndpoint, OnDeviceDisable);

        //
        // If forward progress is enabled, then only free resources if the device
        // is disabled.
        //
        if ((EndpointData->ForwardProgressEnabled) && (OnDeviceDisable == FALSE)) {
            freeResources = FALSE;
        } else {
            freeResources = TRUE;
        }

        //
        // Disable all transfer rings. It is ok to call TR_Disable even if
        // TR_Enable was not called for a given transfer ring.
        //
        if (EndpointData->StreamsCapable) {

            staticStreamsData = EndpointData->StaticStreamsData;

            if (staticStreamsData != NULL) {

                for (i = 1; i <= staticStreamsData->NumberOfStreams; i += 1) {

                    TR_Disable(Endpoint_GetStreamHandle(EndpointData, i), freeResources);
                }
            }

        } else {

            TR_Disable(EndpointData->TransferRingHandle, freeResources);
        }

        if (freeResources == FALSE) {
            LEAVE;
        }

        if (EndpointData->StreamsCapable) {

            staticStreamsData = EndpointData->StaticStreamsData;

            if (staticStreamsData != NULL) {

                if (staticStreamsData->StreamContextArray != NULL) {
                    CommonBuffer_ReleaseBuffer(commonBufferHandle, staticStreamsData->StreamContextArray);
                    staticStreamsData->StreamContextArray = NULL;
                }

                if (staticStreamsData == EndpointData->DefaultStaticStreamsData) {
                    ExFreePoolWithTag(staticStreamsData, DriverAllocationTag);
                    EndpointData->DefaultStaticStreamsData = NULL;
                }

                EndpointData->StaticStreamsData = NULL;
            }
        }

        if ((EndpointData->RecorderLog != NULL) &&
            (EndpointData->RecorderLog != Controller_GetRecorderLog(EndpointData->ControllerHandle))) {
            WppRecorderLogDelete(EndpointData->RecorderLog);
        }
        EndpointData->RecorderLog = NULL;

    } FINALLY {

    }

    return;
}

VOID
Endpoint_EvaluateContextCompletion(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This function is called on an Evaluate Context command completion event.

Arguments:

    EventTRB - To be used for logging Hardware verifier events


--*/
{
    NTSTATUS                status;
    PENDPOINT_UPDATE_DATA   endpointUpdateData;
    HANDLE                  commonBufferHandle;

    UNREFERENCED_PARAMETER(EventTRB);

    TRY {

        endpointUpdateData = (PENDPOINT_UPDATE_DATA)Crb->ObjectHandle;

        commonBufferHandle =
            Controller_GetCommonBufferHandle(endpointUpdateData->EndpointData->ControllerHandle);

        CommonBuffer_ReleaseBuffer(commonBufferHandle, endpointUpdateData->InputBufferData);
        endpointUpdateData->InputBufferData = NULL;

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the evaluate
            // context command effectively failed.
            //

            LogInfo(Controller_GetRecorderLog(endpointUpdateData->EndpointData->ControllerHandle), Endpoint,
                    "%d.%d: Update MaxPacketSize - controller gone",
                    UsbDevice_GetSlotId(endpointUpdateData->EndpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointUpdateData->EndpointData));
            status = STATUS_UNSUCCESSFUL;

        } else if (Crb->CompletionCode == TRB_COMPLETION_CODE_SUCCESS) {

            status = STATUS_SUCCESS;

            endpointUpdateData->EndpointData->UsbEndpointDescriptor.wMaxPacketSize =
                (USHORT)endpointUpdateData->MaxPacketSize;

            LogInfo(Controller_GetRecorderLog(endpointUpdateData->EndpointData->ControllerHandle), Endpoint,
                    "%d.%d: Update MaxPacketSize %d Success",
                    UsbDevice_GetSlotId(endpointUpdateData->EndpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointUpdateData->EndpointData),
                    endpointUpdateData->MaxPacketSize);

            Etw_EndpointUpdate(NULL, endpointUpdateData->EndpointData);

        } else {

            LogError(Controller_GetRecorderLog(endpointUpdateData->EndpointData->ControllerHandle), Endpoint,
                     "%d.%d: Failed to update MaxPacketSize %d : %!TrbCompletionCode!",
                     UsbDevice_GetSlotId(endpointUpdateData->EndpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointUpdateData->EndpointData),
                     endpointUpdateData->MaxPacketSize,
                     (TRB_COMPLETION_CODE)Crb->CompletionCode);
            status = STATUS_UNSUCCESSFUL;
        }

    } FINALLY {

        WdfRequestComplete(WdfObjectContextGetObject(endpointUpdateData), status);
    }

    return;
}

VOID
Endpoint_UcxEvtDefaultEndpointUpdate(
    UCXCONTROLLER   UcxController,
    WDFREQUEST      WdfRequest
    )
{

    NTSTATUS                    status;
    PENDPOINT_DATA              endpointData;
    PDEFAULT_ENDPOINT_UPDATE    defaultEndpointUpdate;
    HANDLE                      registerHandle;
    HANDLE                      commonBufferHandle;
    HANDLE                      commandHandle;
    WDF_REQUEST_PARAMETERS      wdfRequestParams;
    PENDPOINT_UPDATE_DATA       endpointUpdateData;
    PINPUT_CONTEXT64            inputContext;
    PINPUT_CONTEXT32            inputContext32;
    PENDPOINT_CONTEXT64         endpointContext;
    PCRB                        crb;

    UNREFERENCED_PARAMETER(UcxController);

    TRY {

        //
        // Unpack the request to pull out the Urb
        //
        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        defaultEndpointUpdate = (PDEFAULT_ENDPOINT_UPDATE)wdfRequestParams.Parameters.Others.Arg1;

        //
        // Update the cached max packet size in the endpoint descriptor. The default
        // endpoint doesn't normally have an endpoint descriptor, but the endpoint
        // object created a version to represent the default endpoint descriptor.
        // This allows more code reuse regardless of the endpoint type.
        //
        endpointData = GetEndpointData(defaultEndpointUpdate->DefaultEndpoint);

        LogInfo(Controller_GetRecorderLog(endpointData->ControllerHandle), Endpoint,
                "%d.%d: defaultEndpointUpdate->MaxPacketSize %d",
                UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(endpointData),
                defaultEndpointUpdate->MaxPacketSize);

        registerHandle = Controller_GetRegisterHandle(endpointData->ControllerHandle);
        commonBufferHandle = Controller_GetCommonBufferHandle(endpointData->ControllerHandle);
        commandHandle = Controller_GetCommandHandle(endpointData->ControllerHandle);

        endpointUpdateData = &(GetRequestData(WdfRequest)->EndpointUpdateData);
        RtlZeroMemory(endpointUpdateData, sizeof(ENDPOINT_UPDATE_DATA));

        endpointUpdateData->EndpointData = endpointData;
        endpointUpdateData->MaxPacketSize = defaultEndpointUpdate->MaxPacketSize;

        if (REGISTER_CONTEXTSIZE64 == Register_GetContextSize(registerHandle)) {

            endpointUpdateData->InputBufferData =
                CommonBuffer_AcquireBuffer(commonBufferHandle,
                                           sizeof(INPUT_CONTEXT64),
                                           endpointData,
                                           ENDPOINT_ALLOC_TAG1);

        } else {

            endpointUpdateData->InputBufferData =
                CommonBuffer_AcquireBuffer(commonBufferHandle,
                                           sizeof(INPUT_CONTEXT32),
                                           endpointData,
                                           ENDPOINT_ALLOC_TAG1);
        }

        if (endpointUpdateData->InputBufferData == NULL) {

            LogError(Controller_GetRecorderLog(endpointData->ControllerHandle), Endpoint,
                     "%d.%d: Failed to acquire endpointUpdateData->InputBufferData",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData));

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

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
        inputContext = (PINPUT_CONTEXT64)endpointUpdateData->InputBufferData->VirtualAddress;

        if (Register_GetContextSize(registerHandle) == REGISTER_CONTEXTSIZE64) {
            endpointContext = &inputContext->EndpointContext[0];
        } else {
            inputContext32 = (PINPUT_CONTEXT32)inputContext;
            endpointContext = (PENDPOINT_CONTEXT64)&inputContext32->EndpointContext[0];
        }

        //
        // Only update the default control endpoint.
        //
        inputContext->InputControlContext.AddContextFlags.A1 = 1;

        endpointContext->EndpointType      = Endpoint_GetType(endpointData);
        endpointContext->MaxPacketSize     = endpointUpdateData->MaxPacketSize;
        endpointContext->MaxBurstSize      = 0;
        endpointContext->Interval          = 0;
        endpointContext->MaxPrimaryStreams = 0;
        endpointContext->Mult              = 0;
        endpointContext->ErrorCount        = Endpoint_GetErrorCount(endpointData);
        endpointContext->ForceEvent        = 0;
        endpointContext->DequeuePointer    = Endpoint_GetDequeuePointer(endpointData, 0);

        Endpoint_SetMaxPayloadPerServiceOpportunity(endpointData,
                                                    endpointContext->MaxPacketSize);

        Endpoint_InitializeTransferRing(endpointData, 0);

        //
        // Build and send the address device command to address the device.
        //
        crb = &endpointUpdateData->Crb;
        
        RtlZeroMemory(crb, sizeof(CRB));

        crb->CommandCallback                                = Endpoint_EvaluateContextCompletion;
        crb->ObjectHandle                                   = (HANDLE)endpointUpdateData;
        crb->Trb.EvaluateContextCommand.Type                = TRB_TYPE_EVALUATE_CONTEXT_COMMAND;
        crb->Trb.EvaluateContextCommand.SlotId              = UsbDevice_GetSlotId(endpointData->UsbDeviceHandle);
        crb->Trb.EvaluateContextCommand.InputContextPointer = (ULONG64)endpointUpdateData->InputBufferData->LogicalAddress.QuadPart;
        crb->InputContextVA                                 = inputContext;

        Command_SendCommand(commandHandle, crb);

        status = STATUS_SUCCESS;

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (endpointUpdateData->InputBufferData != NULL) {
                CommonBuffer_ReleaseBuffer(commonBufferHandle, endpointUpdateData->InputBufferData);
                endpointUpdateData->InputBufferData = NULL;
            }

            WdfRequestComplete(WdfRequest, status);
        }
    }

    return;
}

VOID
Endpoint_WdfEvtPurgeComplete(
    WDFQUEUE    WdfQueue,
    WDFCONTEXT  WdfContext
    )
{
    UCXENDPOINT             ucxEndpoint;
    PENDPOINT_DATA          endpointData;
    PSTATIC_STREAMS_DATA    staticStreamsData;
    LONG                    currentValue;

    UNREFERENCED_PARAMETER(WdfQueue);

    TRY {

        ucxEndpoint = WdfContext;
        endpointData = GetEndpointData(ucxEndpoint);

        if ((endpointData->StreamsCapable) && (endpointData->StaticStreamsData != NULL)) {

            staticStreamsData = endpointData->StaticStreamsData;

            currentValue = InterlockedDecrement((PLONG)&staticStreamsData->PurgeCount);

            if (currentValue == 0) {

                LogInfo(endpointData->RecorderLog, Endpoint,
                        "%d.%d: Purge UcxEndpoint 0x%p Complete (Streams)",
                        UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                        Endpoint_GetContextIndex(endpointData), ucxEndpoint);

                UcxEndpointPurgeComplete(ucxEndpoint);
            }

        } else {

            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Purge UcxEndpoint 0x%p Complete",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData), ucxEndpoint);
            UcxEndpointPurgeComplete(ucxEndpoint);
        }

    } FINALLY {

    }

    return;
}

VOID
Endpoint_UcxEvtEndpointPurge(
    UCXCONTROLLER   UcxController,
    UCXENDPOINT     UcxEndpoint
    )
{
    PENDPOINT_DATA          endpointData;
    PSTATIC_STREAMS_DATA    staticStreamsData;
    ULONG                   i;

    UNREFERENCED_PARAMETER(UcxController);

    TRY {

        endpointData = GetEndpointData(UcxEndpoint);

        //
        // It is possible that the endpoint is StreamsCapable but StaticStreamsData
        // is NULL if the endpoint has been disabled before this Purge call was
        // received. UCX is allowed to call Purge after the endpoint is disabled. In
        // that case, just purge the default queue of the endpoint.
        //
        if ((endpointData->StreamsCapable) && (endpointData->StaticStreamsData != NULL)) {

            staticStreamsData = endpointData->StaticStreamsData;

            staticStreamsData->PurgeCount = staticStreamsData->NumberOfStreams;

            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Purge UcxEndpoint 0x%p Start: NumberOfStreams %d",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData),
                    UcxEndpoint, staticStreamsData->NumberOfStreams);

            for (i = 1; i <= staticStreamsData->NumberOfStreams; i += 1) {

                WdfIoQueuePurge(TR_GetWdfQueue(Endpoint_GetStreamHandle(endpointData, i)),
                                Endpoint_WdfEvtPurgeComplete,
                                UcxEndpoint);
            }

        } else {

            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Purge UcxEndpoint 0x%p Start",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData), UcxEndpoint);

            WdfIoQueuePurge(TR_GetWdfQueue(endpointData->TransferRingHandle),
                            Endpoint_WdfEvtPurgeComplete,
                            UcxEndpoint);
        }

    } FINALLY {

    }

    return;
}

VOID
Endpoint_UcxEvtEndpointStart(
    UCXCONTROLLER   UcxController,
    UCXENDPOINT     UcxEndpoint
    )
{
    PENDPOINT_DATA          endpointData;
    PSTATIC_STREAMS_DATA    staticStreamsData;
    ULONG                   i;

    UNREFERENCED_PARAMETER(UcxController);

    TRY {

        endpointData = GetEndpointData(UcxEndpoint);

        if (endpointData->StreamsCapable) {

            staticStreamsData = endpointData->StaticStreamsData;

            for (i = 1; i <= staticStreamsData->NumberOfStreams; i += 1) {

                WdfIoQueueStart(TR_GetWdfQueue(Endpoint_GetStreamHandle(endpointData, i)));
            }

        } else {

            WdfIoQueueStart(TR_GetWdfQueue(endpointData->TransferRingHandle));
        }

    } FINALLY {

    }

    return;
}

VOID
Endpoint_WdfEvtAbortComplete(
    WDFQUEUE    WdfQueue,
    WDFCONTEXT  WdfContext
    )
{
    UCXENDPOINT             ucxEndpoint;
    PENDPOINT_DATA          endpointData;
    PSTATIC_STREAMS_DATA    staticStreamsData;
    LONG                    currentValue;

    UNREFERENCED_PARAMETER(WdfQueue);

    TRY {

        ucxEndpoint = WdfContext;
        endpointData = GetEndpointData(ucxEndpoint);

        if (endpointData->StreamsCapable) {

            staticStreamsData = endpointData->StaticStreamsData;

            currentValue = InterlockedDecrement((PLONG)&staticStreamsData->PurgeCount);

            if (currentValue == 0) {

                LogInfo(endpointData->RecorderLog, Endpoint,
                        "%d.%d: Abort UcxEndpoint 0x%p Complete (Streams)",
                        UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                        Endpoint_GetContextIndex(endpointData), ucxEndpoint);

                UcxEndpointAbortComplete(ucxEndpoint);
            }

        } else {

            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Abort UcxEndpoint 0x%p Complete",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData), ucxEndpoint);

            UcxEndpointAbortComplete(ucxEndpoint);
        }

    } FINALLY {

    }

    return;
}

VOID
Endpoint_UcxEvtEndpointAbort(
    UCXCONTROLLER   UcxController,
    UCXENDPOINT     UcxEndpoint
    )
/*++

Routine Description:

    This function aborts pending queued requests. This includes both requests
    that have been presented to the driver and requests that are still on the
    queue.

    Determine the number of requests present on the queue and presented to the
    driver. These are the requests that will be cancelled. Any new requests
    added to the queue will not be aborted.

--*/
{
    PENDPOINT_DATA          endpointData;
    PSTATIC_STREAMS_DATA    staticStreamsData;
    ULONG                   i;

    UNREFERENCED_PARAMETER(UcxController);

    TRY {

        endpointData = GetEndpointData(UcxEndpoint);

        if (endpointData->StreamsCapable) {

            staticStreamsData = endpointData->StaticStreamsData;

            staticStreamsData->PurgeCount = staticStreamsData->NumberOfStreams;

            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Abort UcxEndpoint 0x%p Start: NumberOfStreams %d",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData),
                    UcxEndpoint, staticStreamsData->NumberOfStreams);

            for (i = 1; i <= staticStreamsData->NumberOfStreams; i += 1) {

                WdfIoQueueStopAndPurge(TR_GetWdfQueue(Endpoint_GetStreamHandle(endpointData, i)),
                                       Endpoint_WdfEvtAbortComplete,
                                       UcxEndpoint);
            }

        } else {

            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Abort UcxEndpoint 0x%p Start",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData), UcxEndpoint);

            WdfIoQueueStopAndPurge(TR_GetWdfQueue(endpointData->TransferRingHandle),
                                   Endpoint_WdfEvtAbortComplete,
                                   UcxEndpoint);
        }

    } FINALLY {

    }

    return;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Endpoint_StreamsOnResetSetDequeuePointerComplete(
    __in
        PENDPOINT_RESET_DATA    EndpointResetData,
    __in
        ULONG                   StreamId
    )
/*++

Routine Description:

    This function is called for each stream after the dequeue pointer has been updated,
    following an endpoint reset. Once dequeue pointers have been updated for all streams,
    this function will transition the stream state machine to the ResetComplete state.

--*/
{
    PENDPOINT_DATA          endpointData;
    PSTATIC_STREAMS_DATA    staticStreamsData;
    ULONG                   updateCount;

    TRY {

        endpointData = EndpointResetData->EndpointData;
        staticStreamsData = endpointData->StaticStreamsData;

        updateCount = (ULONG)InterlockedIncrement(&staticStreamsData->UpdateCount);

        LogVerbose(endpointData->RecorderLog, Endpoint,
                   "%d.%d.%d: UpdateCount %d",
                   UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                   Endpoint_GetContextIndex(endpointData), StreamId, updateCount);

        NT_ASSERT(updateCount <= staticStreamsData->NumberOfStreams);

        if (updateCount == staticStreamsData->NumberOfStreams) {

            endpointData->EndpointResetCompletionStatus = STATUS_SUCCESS;
            ESM_AddEsmEvent(endpointData, EsmEventCommandCompletionSuccess);
        }

    } FINALLY {

    }

    return;
}

VOID
Endpoint_OnResetSetDequeuePointerCompletion(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This function is called on an endpoint reset command completion event that was
    issued for a reset endpoint request.

Arguments:

    EventTRB - Used for logging Hardware verifier events

--*/
{
    PENDPOINT_RESET_DATA    endpointResetData;
    PENDPOINT_DATA          endpointData;
    NTSTATUS                status;

    TRY {

        endpointResetData = (PENDPOINT_RESET_DATA)Crb->ObjectHandle;
        endpointData = endpointResetData->EndpointData;

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the set dequeue
            // pointer command effectively failed.  The failure in this
            // case can be ignored.
            //
            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Set dequeue pointer for endpoint - controller gone",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData));

            status = STATUS_UNSUCCESSFUL;

        } else if (Crb->CompletionCode != TRB_COMPLETION_CODE_SUCCESS) {

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: Set dequeue pointer for endpoint failed : %!TrbCompletionCode!",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData),
                     (TRB_COMPLETION_CODE)Crb->CompletionCode);
            
            Controller_HwVerifierBreakIfEnabled(
                endpointData->ControllerHandle,
                endpointData->UcxUsbDevice,
                endpointData->UcxEndpoint,
                ControllerHwVerifierSetDequeuePointerAfterEndpointResetFailed,
                "Set Dequeue pointer command following a Reset Endpoint command failed",
                &Crb->Trb,
                EventTRB);

            Controller_ReportFatalError(endpointData->ControllerHandle,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_CONTEXT_ERROR_AFTER_RESET_SET_DEQUEUE,
                                        endpointData->UsbDeviceHandle,
                                        endpointData,
                                        EventTRB);

            status = STATUS_UNSUCCESSFUL;

        } else {

            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d.%d: OnResetSetDequeuePointer Complete success",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData),
                    Crb->Trb.SetDequeuePointerCommand.StreamId);

            //
            // For streams endpoint, the request will be completed after dequeue
            // pointers for all streams that need updating have been updated.
            //
            if (endpointData->StreamsCapable) {

                Endpoint_StreamsOnResetSetDequeuePointerComplete(
                    endpointResetData,
                    Crb->Trb.SetDequeuePointerCommand.StreamId);

            } else {

                endpointData->EndpointResetCompletionStatus = STATUS_SUCCESS;
                ESM_AddEsmEvent(endpointData, EsmEventCommandCompletionSuccess);
            }

            status = STATUS_SUCCESS;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            endpointData->EndpointResetCompletionStatus = status;
            Endpoint_ReportCommandFailure(endpointData);
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Endpoint_IsTransferRingEmpty(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ULONG           StreamId
    )
/*++

Routine Description:

    This routine is supposed to check whether the hardware DequeuPointer matches
    the software EnqueuePointer. But today it is being more conservative and only
    returns TRUE if both DequeuePointer and EnqueuePointer point to the beginning
    of the current segment.

    TODO: PostWin8: See if the right implementation can be done safely.

--*/
{
    LONG64              currentSegmentBase;
    PHYSICAL_ADDRESS    dequeuePointer;
    LONG64              enqueuePointer;
    HANDLE              transferRingHandle;

    transferRingHandle = Endpoint_GetStreamHandle(EndpointData, StreamId);

    dequeuePointer     = Endpoint_GetStreamContextDequeuePointer(EndpointData, StreamId);
    currentSegmentBase = TR_GetCurrentSegmentLA(transferRingHandle);
    enqueuePointer     = TR_GetEnqueuePointerLA(transferRingHandle);

    if ((dequeuePointer.QuadPart == currentSegmentBase) && (dequeuePointer.QuadPart == enqueuePointer)) {
        return TRUE;
    } else {
        return FALSE;
    }
}

VOID
Endpoint_OnResetSetDequeuePointer(
    __in
        PENDPOINT_RESET_DATA    endpointResetData,
    __in
        ULONG                   StreamId
    )
/*++

Routine Description:

    This function is called to send a set dequeue pointer command when a reset
    endpoint request is received.

--*/
{
    PCRB            crb;
    PENDPOINT_DATA  endpointData;

    TRY {

        endpointData = endpointResetData->EndpointData;

        if (endpointData->StreamsCapable) {

            if (Endpoint_IsTransferRingEmpty(endpointData, StreamId)) {
                Endpoint_StreamsOnResetSetDequeuePointerComplete(endpointResetData, StreamId);
                LEAVE;
            }

            crb = &endpointData->StaticStreamsData->StaticStreamData[StreamId - 1].Crb;

        } else {

            crb = &endpointData->Crb;
        }

        RtlZeroMemory(crb, sizeof(CRB));

        crb->CommandCallback                             = Endpoint_OnResetSetDequeuePointerCompletion;
        crb->ObjectHandle                                = (HANDLE)endpointResetData;
        crb->Trb.SetDequeuePointerCommand.Type           = TRB_TYPE_SET_DEQUEUE_POINTER_COMMAND;
        crb->Trb.SetDequeuePointerCommand.DequeuePointer = Endpoint_GetDequeuePointer(endpointData, StreamId);
        crb->Trb.SetDequeuePointerCommand.EndpointId     = Endpoint_GetContextIndex(endpointData);
        crb->Trb.SetDequeuePointerCommand.SlotId         = UsbDevice_GetSlotId(endpointData->UsbDeviceHandle);

        if (endpointData->StreamsCapable) {
            crb->Trb.SetDequeuePointerCommand.StreamId = StreamId;
        }

        Command_SendCommand(Controller_GetCommandHandle(endpointData->ControllerHandle), crb);

    } FINALLY {

    }

    return;
}

VOID
Endpoint_OnResetEndpointResetCompletion(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This function is called on an endpoint reset command completion event that was
    issued for a reset endpoint request.

Arguments:

    EventTRB - Used for logging Hardware verifier events

--*/
{
    PENDPOINT_RESET_DATA    endpointResetData;
    PENDPOINT_DATA          endpointData;
    ULONG                   endpointState;
    PSTATIC_STREAMS_DATA    staticStreamsData;
    NTSTATUS                status;
    ULONG                   streamId;

    TRY {

        endpointResetData = (PENDPOINT_RESET_DATA)Crb->ObjectHandle;
        endpointData = endpointResetData->EndpointData;

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the set dequeue
            // pointer command effectively failed.  The failure in this
            // case can be ignored.
            //
            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Endpoint reset - controller gone",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData));

            status = STATUS_UNSUCCESSFUL;

        } else if (Crb->CompletionCode != TRB_COMPLETION_CODE_SUCCESS) {

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: Endpoint reset failed : %!TrbCompletionCode!",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData),
                     (TRB_COMPLETION_CODE)Crb->CompletionCode);
            
            Controller_HwVerifierBreakIfEnabled(
                endpointData->ControllerHandle,
                endpointData->UcxUsbDevice,
                endpointData->UcxEndpoint,
                ControllerHwVerifierEndpointResetFailed,
                "Endpoint Reset Command failed",
                &Crb->Trb,
                EventTRB);

            Controller_ReportFatalError(endpointData->ControllerHandle,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_ENDPOINT_ERROR_AFTER_RESET,
                                        endpointData->UsbDeviceHandle,
                                        endpointData,
                                        NULL);

            status = STATUS_UNSUCCESSFUL;

        } else {

            endpointState = UsbDevice_GetEndpointState(endpointData->UsbDeviceHandle,
                                                       endpointData->ContextIndex);

            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Endpoint reset success. %!EndpointState!",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData),
                    (ENDPOINT_STATE)endpointState);

            NT_ASSERTMSG("After endpoint reset, endpoint state should be stopped",
                         (endpointState == ENDPOINT_STATE_STOPPED));

            if (endpointData->StreamsCapable) {

                staticStreamsData = endpointData->StaticStreamsData;
                staticStreamsData->UpdateCount = 0;

                for (streamId = 1; streamId <= endpointData->StaticStreamsData->NumberOfStreams; streamId += 1) {
                    Endpoint_OnResetSetDequeuePointer(endpointResetData, streamId);
                }

            } else {

                Endpoint_OnResetSetDequeuePointer(endpointResetData, 0);
            }

            status = STATUS_SUCCESS;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            endpointData->EndpointResetCompletionStatus = status;
            Endpoint_ReportCommandFailure(endpointData);
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_OnResetEndpointConfigure(
    __in
        PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    This function is called for an endpoint reset request, to reset the data toggle and sequence
    number, if we receive an endpoint reset request and the endpoint didn't actually halt.

--*/
{
    NTSTATUS                status;
    HANDLE                  commandHandle;
    HANDLE                  registerHandle;
    HANDLE                  commonBufferHandle;
    PENDPOINT_CONTEXT64     newEndpointContext;
    PENDPOINT_CONTEXT64     orgEndpointContext;
    ULONG                   deviceContextIndex;
    PENDPOINT_RESET_DATA    endpointResetData;

    TRY {

        commandHandle      = Controller_GetCommandHandle(EndpointData->ControllerHandle);
        registerHandle     = Controller_GetRegisterHandle(EndpointData->ControllerHandle);
        commonBufferHandle = Controller_GetCommonBufferHandle(EndpointData->ControllerHandle);
        endpointResetData  = &(GetRequestData(EndpointData->EndpointResetRequest)->EndpointResetData);
        deviceContextIndex = Endpoint_GetContextIndex(EndpointData);

        //
        // Allocate the zero initialized common buffer that will be used
        // as the Input Device Context for the TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND.
        //
        // If the HCCPARAMS register Context Size (CSZ) bit is set to
        // '1' then the xHC uses 64-byte Context data structures.  If
        // this bit is cleared to '0' then the xHC uses 32-byte Context
        // data structures.
        //
        if (REGISTER_CONTEXTSIZE64 == Register_GetContextSize(registerHandle)) {

            orgEndpointContext = UsbDevice_DeviceContext64GetEndpoint(
                UsbDevice_GetDeviceContext64(EndpointData->UsbDeviceHandle),
                deviceContextIndex);

            endpointResetData->InputBufferData =
                CommonBuffer_AcquireBuffer(commonBufferHandle,
                                           sizeof(INPUT_CONTEXT64),
                                           EndpointData,
                                           ENDPOINT_ALLOC_TAG2);
        } else {

            orgEndpointContext = (PENDPOINT_CONTEXT64)UsbDevice_DeviceContext32GetEndpoint(
                UsbDevice_GetDeviceContext32(EndpointData->UsbDeviceHandle),
                deviceContextIndex);

            endpointResetData->InputBufferData =
                CommonBuffer_AcquireBuffer(commonBufferHandle,
                                           sizeof(INPUT_CONTEXT32),
                                           EndpointData,
                                           ENDPOINT_ALLOC_TAG2);
        }

        if (endpointResetData->InputBufferData == NULL) {

            LogError(EndpointData->RecorderLog, Endpoint,
                     "%d.%d: Failed to acquire endpointResetData->InputBufferData",
                     UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(EndpointData));

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        if (EndpointData->StreamsCapable) {

            //
            // Since we want to ensure the dequeue pointers are up to date, we must first drop the
            // endpoint to reclaim ownership of the Stream Context Array (SCA).
            // We could do this with just 1 configure, but it would require an extra allocation of a
            // new SCA.
            // Currently we also Set Dequeue Pointer on active streams, however if we can eliminate
            // that (TBD), then (2x) Configure Endpoint will be a more economical use of commands.
            //
            LogInfo(EndpointData->RecorderLog, Endpoint,
                    "%d.%d: Initiating drop EP, re-init SCA, re-add EP since this is a streams EP",
                    UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(EndpointData));

            Endpoint_SetUpConfigureEndpointCommand(EndpointData,
                                                   EndpointConfigureOperation_Drop,
                                                   Endpoint_OnResetEndpointConfigureCompletion_EpDropped,
                                                   (HANDLE)endpointResetData,
                                                   endpointResetData->InputBufferData,
                                                   orgEndpointContext,
                                                   NULL,
                                                   &endpointResetData->Crb);

        } else {

            //
            // For non-stream endpoint, we can configure endpoint and update dequeue pointer with
            // just 1 command (drop + add endpoint).
            //
            LogInfo(EndpointData->RecorderLog, Endpoint,
                    "%d.%d: Sending single drop + add EP command since this is a non-stream EP",
                    UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(EndpointData));

            Endpoint_SetUpConfigureEndpointCommand(EndpointData,
                                                   EndpointConfigureOperation_DropAndAdd,
                                                   Endpoint_OnResetEndpointConfigureCompletion,
                                                   (HANDLE)endpointResetData,
                                                   endpointResetData->InputBufferData,
                                                   orgEndpointContext,
                                                   &newEndpointContext,
                                                   &endpointResetData->Crb);

            //
            // Initialize the transfer ring to it's initial values. This requires that the
            // transfer ring is empty, but that is also a requirement before issuing
            // the configure endpoints command.
            //
            Endpoint_InitializeTransferRing(EndpointData, 0);

            newEndpointContext->DequeuePointer = Endpoint_GetDequeuePointer(EndpointData, 0);
        }

        Command_SendCommand(commandHandle, &endpointResetData->Crb);

        status = STATUS_SUCCESS;

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            NT_ASSERT(status == STATUS_INSUFFICIENT_RESOURCES);

            EndpointData->EndpointResetCompletionStatus = status;
            ESM_AddEsmEvent(EndpointData, EsmEventCommonBufferAllocationFailure);
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_OnResetEndpointConfigureCompletion_EpDropped(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    Streams endpoint has been dropped temporarily. Update dequeue pointers and re-add the
    streams endpoint.

Arguments:

    EventTRB - To be used for logging Hardware verifier events

--*/
{
    ULONG                   i;
    PENDPOINT_RESET_DATA    endpointResetData;
    PENDPOINT_DATA          endpointData;
    HANDLE                  commonBufferHandle;
    HANDLE                  commandHandle;
    PENDPOINT_CONTEXT64     newEndpointContext;
    PSTREAM_CONTEXT         streamContext;
    BOOLEAN                 completeRequest;

    UNREFERENCED_PARAMETER(EventTRB);

    TRY {

        endpointResetData = (PENDPOINT_RESET_DATA)Crb->ObjectHandle;
        endpointData = endpointResetData->EndpointData;

        NT_ASSERT(endpointData->StreamsCapable);

        commonBufferHandle = Controller_GetCommonBufferHandle(endpointData->ControllerHandle);
        commandHandle = Controller_GetCommandHandle(endpointData->ControllerHandle);

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the endpoint configure
            // command effectively failed.  The failure in this case can be ignored.
            //
            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Configure endpoint on reset (drop) - controller gone",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData));

            completeRequest = TRUE;
            LEAVE;

        } else if (Crb->CompletionCode != TRB_COMPLETION_CODE_SUCCESS) {

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: Configure endpoint on reset failed (drop): %!TrbCompletionCode!",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData),
                     (TRB_COMPLETION_CODE)Crb->CompletionCode);

            NT_ASSERTMSG("Configure endpoint on reset failed (drop)", 0);

            Controller_ReportFatalError(endpointData->ControllerHandle,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_ENDPOINTS_DECONFIGURE_FAILED,
                                        endpointData->UsbDeviceHandle,
                                        endpointData,
                                        NULL);

            completeRequest = TRUE;
            LEAVE;
        }

        LogInfo(endpointData->RecorderLog, Endpoint,
                "%d.%d: Configure endpoint on reset success (dropped now re-adding)",
                UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(endpointData));

        //
        // Initialize the transfer rings to their initial values. This requires that the
        // transfer rings are empty, but that is also a requirement before issuing
        // the configure endpoints command. Then write the latest dequeue pointers to the SCA.
        //
        for (i = 1; i <= endpointData->StaticStreamsData->NumberOfStreams; i += 1) {

            Endpoint_InitializeTransferRing(endpointData, i);

            streamContext = Endpoint_GetStreamContext(endpointData, i);
            streamContext->DequeuePointer = Endpoint_GetDequeuePointer(endpointData, i);
        }

        Endpoint_SetUpConfigureEndpointCommand(endpointData,
                                               EndpointConfigureOperation_Add,
                                               Endpoint_OnResetEndpointConfigureCompletion,
                                               (HANDLE)endpointResetData,
                                               endpointResetData->InputBufferData,
                                               NULL,
                                               &newEndpointContext,
                                               &endpointResetData->Crb);

        //
        // We stashed the last Output Endpoint Context in the Input Endpoint Context, so it is
        // already mostly set up.
        //
        newEndpointContext->DequeuePointer = Endpoint_GetDequeuePointer(endpointData, 0);

        Command_SendCommand(commandHandle, &endpointResetData->Crb);

        completeRequest = FALSE;

    } FINALLY {

        if (completeRequest) {

            CommonBuffer_ReleaseBuffer(commonBufferHandle, endpointResetData->InputBufferData);
            endpointResetData->InputBufferData = NULL;

            //
            // WIN8 behavior: Reset Request was succeeded on command failure.
            //
            endpointData->EndpointResetCompletionStatus = STATUS_SUCCESS;
            Endpoint_ReportCommandFailure(endpointData);
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_OnResetEndpointConfigureCompletion(
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
    TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND and was originated on
    a reset endpoint request.

    This can be called either after a single drop + add configure command, or after separate
    drop and add configure commands (for a streams endpoint).

Arguments:

    EventTRB - To be used for logging Hardware verifier events

--*/
{
    PENDPOINT_RESET_DATA    endpointResetData;
    PENDPOINT_DATA          endpointData;
    HANDLE                  commonBufferHandle;
    NTSTATUS                status;

    UNREFERENCED_PARAMETER(EventTRB);

    TRY {

        endpointResetData = (PENDPOINT_RESET_DATA)Crb->ObjectHandle;
        endpointData = endpointResetData->EndpointData;

        commonBufferHandle = Controller_GetCommonBufferHandle(endpointData->ControllerHandle);

        CommonBuffer_ReleaseBuffer(commonBufferHandle, endpointResetData->InputBufferData);
        endpointResetData->InputBufferData = NULL;

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the endpoint configure
            // command effectively failed.  The failure in this case can be ignored.
            //
            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Configure endpoint on reset (add) - controller gone",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData));

            status = STATUS_UNSUCCESSFUL;

        } else if (Crb->CompletionCode == TRB_COMPLETION_CODE_SUCCESS) {

            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Configure endpoint on reset success (add)",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData));

            status = STATUS_SUCCESS;

        } else {

            //
            // This failure is unexpected. Most likely something is wrong in the controller
            // requiring reset. (There is also a rare case where another new endpoint on the
            // controller can sneak in and get resources needed by a temporarily-dropped stream
            // endpoint.)
            //
            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: Configure endpoint on reset failed (add): %!TrbCompletionCode!",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData),
                     (TRB_COMPLETION_CODE)Crb->CompletionCode);

            NT_ASSERTMSG("Configure endpoint on reset failed (add)", 0);

            if (endpointData->StreamsCapable) {

                Controller_ReportFatalError(endpointData->ControllerHandle,
                                            Controller_FatalErrorReset,
                                            USB3_WER_BUGCODE_USBXHCI_ENDPOINTS_RECONFIGURE_FAILED_ON_RESET,
                                            endpointData->UsbDeviceHandle,
                                            endpointData,
                                            NULL);

            } else {

                Controller_ReportFatalError(endpointData->ControllerHandle,
                                            Controller_FatalErrorReset,
                                            USB3_WER_BUGCODE_USBXHCI_ENDPOINTS_RECONFIGURE_FAILED,
                                            endpointData->UsbDeviceHandle,
                                            endpointData,
                                            NULL);
            }

            status = STATUS_UNSUCCESSFUL;
        }

    } FINALLY {

        //
        // WIN8 behavior: Reset Request was succeeded even if command failed.
        //
        endpointData->EndpointResetCompletionStatus = STATUS_SUCCESS;

        if (NT_SUCCESS(status)) {
            ESM_AddEsmEvent(endpointData, EsmEventCommandCompletionSuccess);
        } else {
            Endpoint_ReportCommandFailure(endpointData);
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
Endpoint_AreTransfersPending(
    __in
        PENDPOINT_DATA  EndpointData
    )
{
    ULONG   i;
    BOOLEAN transfersArePending;

    if (EndpointData->StreamsCapable) {

        for (i = 1; i <= EndpointData->StaticStreamsData->NumberOfStreams; i += 1) {

            transfersArePending = TR_AreTransfersPending(Endpoint_GetStreamHandle(EndpointData, i));

            if (transfersArePending != FALSE) {
                return TRUE;
            }
        }

        return FALSE;

    } else {

        return TR_AreTransfersPending(EndpointData->TransferRingHandle);
    }
}

VOID
Endpoint_UcxEvtEndpointReset(
    UCXCONTROLLER   UcxController,
    UCXENDPOINT     UcxEndpoint,
    WDFREQUEST      WdfRequest
    )
/*++

Routine Description:

    This function is called by UCX when a request to reset the endpoint is received.

--*/
{
    PENDPOINT_DATA          endpointData;
    PENDPOINT_RESET_DATA    endpointResetData;
    ULONG                   completeRequest;

    UNREFERENCED_PARAMETER(UcxController);

    endpointData = GetEndpointData(UcxEndpoint);

    endpointResetData = &(GetRequestData(WdfRequest)->EndpointResetData);
    RtlZeroMemory(endpointResetData, sizeof(ENDPOINT_RESET_DATA));

    INTERLOCKED_GET_AND_TOGGLE_BIT_FIELD(ENDPOINT_FLAGS,
                                         &endpointData->Flags,
                                         OkToCompleteEndpointResetRequest,
                                         &completeRequest);

    if (completeRequest != 0) {

        LogInfo(endpointData->RecorderLog, Endpoint,
                "%d.%d: WdfRequest 0x%p Endpoint reset completed with %!STATUS!",
                UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(endpointData),
                WdfRequest, STATUS_SUCCESS);

        WdfRequestComplete(WdfRequest, STATUS_SUCCESS);

    } else {

        endpointResetData->EndpointData = endpointData;

        endpointData->EndpointResetRequest          = WdfRequest;
        endpointData->EndpointResetCompletionStatus = STATUS_PENDING;

        ESM_AddEsmEvent(endpointData, EsmEventResetRequestFromClient);
    }
}

VOID
Endpoint_UcxEvtEndpointOkToCancelTransfers(
    UCXENDPOINT     UcxEndpoint
    )
{
    PENDPOINT_DATA endpointData;

    endpointData = GetEndpointData(UcxEndpoint);

    ESM_AddEsmEvent(endpointData, EsmEventOkToCancelTransfers);
}

NTSTATUS
Endpoint_UcxEvtEndpointEnableForwardProgress(
    UCXENDPOINT     UcxEndpoint,
    ULONG           MaximumTransferSize
    )
/*++

Routine Description:

    This function is called by UCX when to enable forward progress a particular endpoint.
    XHCI must ensure that it has allocated enough resources to handle 1 transfer upto
    MaximumTransferSize.

    This function may be called more than once for the same endpoint if the MaximumTransferSize
    increases. Multiple calls to this routine does not imply that XHCI has to support more than 1 transfer for
    this endpoint. UCX will call this routine multiple times for an endpoint only if
    MaximumTransferSize increases.

    This function will be called at passive

--*/
{
    NTSTATUS        status;
    PENDPOINT_DATA  endpointData;

    TRY {

        endpointData = GetEndpointData(UcxEndpoint);

        NT_ASSERT(endpointData->ForwardProgressEnabled == FALSE);

        if (endpointData->StreamsCapable) {

            //
            // For a stream enabled endpoint, resources only need to
            // be pre-allocated for the first stream (StreamId=1).
            //
            status = TR_EnableForwardProgress(Endpoint_GetStreamHandle(endpointData, 1),
                                              MaximumTransferSize);

            if (!NT_SUCCESS(status)) {

                LogWarning(endpointData->RecorderLog, Endpoint,
                           "%d.%d.1: TR_EnableForwardProgress Failed %!STATUS!",
                           UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                           Endpoint_GetContextIndex(endpointData), status);
                LEAVE;
            }

        } else {

            status = TR_EnableForwardProgress(endpointData->TransferRingHandle,
                                              MaximumTransferSize);

            if (!NT_SUCCESS(status)) {

                LogWarning(endpointData->RecorderLog, Endpoint,
                           "%d.%d.0: TR_EnableForwardProgress Failed %!STATUS!",
                           UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                           Endpoint_GetContextIndex(endpointData), status);

                LEAVE;
            }
        }

    } FINALLY {

        if (NT_SUCCESS(status)) {
            endpointData->ForwardProgressEnabled = TRUE;
        }
    }

    return status;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Endpoint_StreamsOnCancelSetDequeuePointerComplete(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ULONG           StreamId
    )
/*++

Routine Description:

    This function is called for each stream after the dequeue pointer has been updated.
    After the dequeue pointer has been updated for all streams, notify the state machine
    in order to transition the stream state to disabled.

--*/
{
    PSTATIC_STREAMS_DATA    staticStreamsData;
    ULONG                   updateCount;

    staticStreamsData = EndpointData->StaticStreamsData;

    updateCount = (ULONG)InterlockedIncrement(&staticStreamsData->UpdateCount);

    LogVerbose(EndpointData->RecorderLog, Endpoint,
               "%d.%d.%d: UpdateCount %d",
               UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
               Endpoint_GetContextIndex(EndpointData), StreamId, updateCount);

    NT_ASSERT(updateCount <= staticStreamsData->NumberOfStreams);

    if (updateCount == staticStreamsData->NumberOfStreams) {

        ESM_AddEsmEvent(EndpointData, EsmEventCommandCompletionSuccess);
    }

    return;
}

VOID
Endpoint_OnCancelSetDequeuePointerCompletion(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This function is called on a set dequeue pointer command completion event.

Arguments:

    EventTRB - Used for logging Hardware verifier events

--*/
{
    PENDPOINT_DATA  endpointData;
    NTSTATUS        status;

    TRY {

        endpointData = (PENDPOINT_DATA)Crb->ObjectHandle;

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the set dequeue
            // pointer command effectively failed.  The failure in this
            // case can be ignored.
            //
            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d.%d: controller gone",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData),
                    Crb->Trb.SetDequeuePointerCommand.StreamId);

            status = STATUS_UNSUCCESSFUL;
            LEAVE;

        } else if (Crb->CompletionCode != TRB_COMPLETION_CODE_SUCCESS) {

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d.%d: OnCancelSetDequeuePointer Failed %!TrbCompletionCode!",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData),
                     Crb->Trb.SetDequeuePointerCommand.StreamId,
                     (TRB_COMPLETION_CODE)Crb->CompletionCode);

            Controller_HwVerifierBreakIfEnabled(
                endpointData->ControllerHandle,
                endpointData->UcxUsbDevice,
                endpointData->UcxEndpoint,
                ControllerHwVerifierSetDequeuePointerAfterStopEndpointFailed,
                "Set Dequeue Pointer command following a Stop Endpoint command failed",
                &Crb->Trb,
                EventTRB);

            Controller_ReportFatalError(endpointData->ControllerHandle,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_CONTEXT_ERROR_AFTER_SET_DEQUEUE,
                                        endpointData->UsbDeviceHandle,
                                        endpointData,
                                        NULL);

            status = STATUS_UNSUCCESSFUL;
            LEAVE;

        } else {

            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d.%d: OnCancelSetDequeuePointer Complete %!TrbCompletionCode!",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData),
                    Crb->Trb.SetDequeuePointerCommand.StreamId,
                    (TRB_COMPLETION_CODE)Crb->CompletionCode);

            if (endpointData->StreamsCapable) {

                Endpoint_StreamsOnCancelSetDequeuePointerComplete(
                    endpointData,
                    Crb->Trb.SetDequeuePointerCommand.StreamId);

            } else {

                ESM_AddEsmEvent(endpointData, EsmEventCommandCompletionSuccess);
            }

            status = STATUS_SUCCESS;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            Endpoint_ReportCommandFailure(endpointData);
        }
    }

    return;
}

VOID
Endpoint_OnCancelSetDequeuePointer(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ULONG           StreamId
    )
/*++

Routine Description:

    This function is called to send a set dequeue pointer command.

--*/
{
    HANDLE  commandHandle;
    PCRB    crb;

    TRY {

        commandHandle = Controller_GetCommandHandle(EndpointData->ControllerHandle);

        if (EndpointData->StreamsCapable) {

            //
            // If the controller needed the workaround to issue a ConfigureEndpoint
            // on stop, SetDequeuePointer is required to be sent for each stream.
            //
            if (Endpoint_IsTransferRingEmpty(EndpointData, StreamId) &&
                (Controller_GetDeviceFlags(EndpointData->ControllerHandle).OnStopConfigureStreamEndpoint == 0)) {

                Endpoint_StreamsOnCancelSetDequeuePointerComplete(EndpointData, StreamId);
                LEAVE;
            }

            crb = &EndpointData->StaticStreamsData->StaticStreamData[StreamId - 1].Crb;

        } else {

            crb = &EndpointData->Crb;
        }

        Endpoint_InitializeTransferRing(EndpointData, StreamId);

        RtlZeroMemory(crb, sizeof(*crb));

        crb->CommandCallback                             = Endpoint_OnCancelSetDequeuePointerCompletion;
        crb->ObjectHandle                                = (HANDLE)EndpointData;
        crb->Trb.SetDequeuePointerCommand.Type           = TRB_TYPE_SET_DEQUEUE_POINTER_COMMAND;
        crb->Trb.SetDequeuePointerCommand.DequeuePointer = Endpoint_GetDequeuePointer(EndpointData, StreamId);
        crb->Trb.SetDequeuePointerCommand.EndpointId     = Endpoint_GetContextIndex(EndpointData);
        crb->Trb.SetDequeuePointerCommand.SlotId         = UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle);

        if (EndpointData->StreamsCapable) {
            crb->Trb.SetDequeuePointerCommand.StreamId = StreamId;
        }

        LogInfo(EndpointData->RecorderLog, Endpoint,
                "%d.%d.%d: OnCancelSetDequeuePointer Begin : xhciDequeuePointer 0x%I64x",
                crb->Trb.SetDequeuePointerCommand.SlotId,
                crb->Trb.SetDequeuePointerCommand.EndpointId, StreamId,
                crb->Trb.SetDequeuePointerCommand.DequeuePointer);

        Command_SendCommand(commandHandle, crb);

    } FINALLY {

    }

    return;
}

IO_COMPLETION_ROUTINE Endpoint_ClearStallTransferCompletion;

NTSTATUS
Endpoint_ClearStallTransferCompletion(
    PDEVICE_OBJECT      DeviceObject,
    PIRP                Irp,
    PVOID               Context
    )
/*++

Routine Description:

    This is the completion routine for the internally created control transfer IRP
    that is sent when calling ConfigureEndpoint instead of SetTRDQPointer on StopEndpoint.

--*/
{
    PENDPOINT_DATA endpointData;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    TRY {

        endpointData = (PENDPOINT_DATA)Context;

        __analysis_assume(endpointData != NULL);

        LogInfo(endpointData->RecorderLog, Endpoint,
                "%d.%d: ClearStall command request completed (WDFREQUEST 0x%p, status %!STATUS!)",
                UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(endpointData),
                endpointData->ClearStallContext->Request,
                Irp->IoStatus.Status);

        //
        // We don't really care about the transfer completion.  If the request failed
        // then the SequenceNumber for the endpoint in the device has likely not been reset.
        // This will result in the next transfer either pending indefinitely, or completing
        // with a transaction error.  In the case of the transfer pending indefinitely, this
        // is the same behavior that the client would see if we indicated an error here.  The 
        // client driver may eventually time out the transfer and initiate error recovery.  If
        // the transfer completed with a transaction error, the client driver will initiate
        // appropriate error recovery.
        // 
        // At this point, the stop command is known to have succeeded, so just complete the 
        // stop process successfully.
        // 
        ESM_AddEsmEvent(endpointData, EsmEventCommandCompletionSuccess);
        
    } FINALLY {

    }

    return STATUS_MORE_PROCESSING_REQUIRED;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_SendClearStallTransfer(
    __in
        PENDPOINT_DATA  EndpointData
    )
/*++

Routine Description:

    This routine initializes and sends a request to the control endpoint of the 
    device containing the specified endpoint data.  This request is a ClearFeature(EndpointStall),
    and needs to be sent in order to ensure that the sequence numbers are in sync between the controller
    and the device.

--*/
{
    WDF_REQUEST_REUSE_PARAMS        reuseParams;
    PUSB_DEFAULT_PIPE_SETUP_PACKET  setupPacket;
    PIO_STACK_LOCATION              irpStack;
    struct _URB_CONTROL_TRANSFER_EX *urb;
    NTSTATUS                        status;
    WDFQUEUE                        queue;
    PENDPOINT_DATA                  controlEndpoint;
    PIRP                            irp;

    TRY {
        urb = &EndpointData->ClearStallContext->Urb;
        irp = EndpointData->ClearStallContext->Irp;
        IoReuseIrp(irp, STATUS_SUCCESS);

        setupPacket =
            (PUSB_DEFAULT_PIPE_SETUP_PACKET) urb->SetupPacket;

        setupPacket->bmRequestType.Recipient = BMREQUEST_TO_ENDPOINT;
        setupPacket->bmRequestType.Type = BMREQUEST_STANDARD;
        setupPacket->bmRequestType.Dir = BMREQUEST_HOST_TO_DEVICE;

        setupPacket->bRequest = USB_REQUEST_CLEAR_FEATURE;
        setupPacket->wValue.W = 0;
        setupPacket->wIndex.W = EndpointData->UsbEndpointDescriptor.bEndpointAddress;
        setupPacket->wLength = 0;

        urb->Hdr.Length = sizeof(struct _URB_CONTROL_TRANSFER_EX);
        urb->Hdr.Function = URB_FUNCTION_CONTROL_TRANSFER_EX;

        urb->TransferFlags = USBD_DEFAULT_PIPE_TRANSFER;

        urb->TransferFlags |= USBD_FORWARD_PROGRESS_TRANSFER;
        
        urb->TransferBufferLength = 0;
        urb->TransferBuffer = NULL;
        urb->TransferBufferMDL = NULL;
        
        urb->Timeout = CONTROL_TRANSFER_TIMEOUT_MS;

        irpStack = IoGetNextIrpStackLocation(irp);
        
        RtlZeroMemory(irpStack,  sizeof(IO_STACK_LOCATION));
        irpStack->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;
        irpStack->Parameters.DeviceIoControl.IoControlCode = IOCTL_INTERNAL_USB_SUBMIT_URB;
        irpStack->Parameters.Others.Argument1 = urb;
    
        IoSetNextIrpStackLocation(irp);
        
        IoCopyCurrentIrpStackLocationToNext(irp);

        IoSetCompletionRoutineSmart(WdfDeviceWdmGetDeviceObject(Controller_GetWdfDeviceObject(EndpointData->ControllerHandle)),
                                    irp, 
                                    Endpoint_ClearStallTransferCompletion, 
                                    EndpointData,
                                    TRUE, TRUE, TRUE);
        IoSetNextIrpStackLocation(irp);

        controlEndpoint = UsbDevice_GetEndpointHandle(EndpointData->UsbDeviceHandle,1);

        if (controlEndpoint == NULL) {
            NT_ASSERT(controlEndpoint);
            LogError(EndpointData->RecorderLog, Endpoint,
                     "%d.%d: Failed to issue ClearStall command to the control endpoint of the device (WDFREQUEST 0x%p)",
                     UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(EndpointData),
                     EndpointData->ClearStallContext->Request);            
            LEAVE;
        }

        queue = TR_GetWdfQueue(controlEndpoint->TransferRingHandle);

        WDF_REQUEST_REUSE_PARAMS_INIT(&reuseParams, 
                                      WDF_REQUEST_REUSE_SET_NEW_IRP | WDF_REQUEST_REUSE_MUST_COMPLETE, 
                                      STATUS_SUCCESS);

        reuseParams.NewIrp = irp;
        WdfRequestReuse(EndpointData->ClearStallContext->Request, &reuseParams);


        LogInfo(EndpointData->RecorderLog, Endpoint,
                "%d.%d: Issuing ClearStall command to the control endpoint of the device (WDFREQUEST 0x%p)",
                UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(EndpointData),
                EndpointData->ClearStallContext->Request);

        status = WdfRequestForwardToIoQueue(EndpointData->ClearStallContext->Request,
                                            queue);
        if (!NT_SUCCESS(status)) {
            WdfRequestComplete(EndpointData->ClearStallContext->Request, status);
        }

    } FINALLY {

    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_SetUpConfigureEndpointCommand(
    __inout
        PENDPOINT_DATA         EndpointData,
    __in
        ENDPOINT_CONFIGURE_OPERATION
                               ConfigureType,
    __in
        PCOMMAND_CALLBACK      CommandCallback,
    __in
        HANDLE                 CallbackContext,
    __in
        PBUFFER_DATA           InputContextCommonBuffer,
    __in_opt
        PENDPOINT_CONTEXT64    EndpointContextToCopy,
    __out_opt
        PENDPOINT_CONTEXT64  * EndpointContext,
    __out
        PCRB                   Crb
    )
/*++

Routine Description:

    Helper function that prepares a Configure Endpoint command (CRB and the Input Context it points
    to).

Arguments:

    EndpointData - Endpoint to configure

    ConfigureType - Whether to add or drop the endpoint (or both at once)

    CommandCallback - Callback for command completion

    CallbackContext - Context for command completion callback

    InputContextCommonBuffer - Memory in which this function sets up the Input Context array. Also
                               points the command's InputContextPointer to this memory. Allocated
                               and optionally initialized by caller; this function writes the Input
                               Control Context, Input Slot Context, and optionally, the endpoint's
                               Input Endpoint Context.

    EndpointContextToCopy - If specified, this 32- or 64-bit context is copied into the correct
                            context in the Input Context. Otherwise, the EP contexts are not touched.

    EndpointContext - Returns the location of this endpoint's Input Endpoint Context so the caller
                      can modify it. Optional.

    Crb - Location to set up Configure Endpoint CRB

Return Value:

    NTSTATUS

--*/
{
    HANDLE              registerHandle;
    PINPUT_CONTEXT64    inputContext;
    PSLOT_CONTEXT64     newSlotContext;
    PSLOT_CONTEXT64     orgSlotContext;
    PENDPOINT_CONTEXT64 newEndpointContext;
    ULONG               deviceContextIndex;

    TRY {

        registerHandle = Controller_GetRegisterHandle(EndpointData->ControllerHandle);

        inputContext = (PINPUT_CONTEXT64)InputContextCommonBuffer->VirtualAddress;

        deviceContextIndex = Endpoint_GetContextIndex(EndpointData);

        //
        // Initialize the Input Device Context.
        //
        // See section 6.2.5 Input Context in the XHCI specification.
        // See also section 4.6.6 Configure Endpoint
        //
        NT_ASSERT(inputContext != NULL);

        //
        // The Add Context flag A1 and Drop Context flags D0 and D1 of
        // the Input Control Context (in the Input Context) shall be
        // cleared to '0'. Endpoint 0 Context does not apply to the
        // Configure Endpoint Command and shall be ignored by the xHC.
        // A0 shall be set to '1' and refer to section 6.2.2.2 for the
        // Slot Context fields used by the Configure Endpoint Command.
        //

        if (Register_GetContextSize(registerHandle) == REGISTER_CONTEXTSIZE64) {

            RtlZeroMemory(&inputContext->InputControlContext, sizeof(INPUT_CONTROL_CONTEXT64));

            newSlotContext = UsbDevice_InputContext64GetSlot(inputContext);

            orgSlotContext = UsbDevice_DeviceContext64GetSlot(
                UsbDevice_GetDeviceContext64(EndpointData->UsbDeviceHandle));

            newEndpointContext = UsbDevice_InputContext64GetEndpoint(
                inputContext,
                deviceContextIndex);

            RtlCopyMemory(newSlotContext, orgSlotContext, sizeof(SLOT_CONTEXT64));

            if (EndpointContextToCopy != NULL) {
                RtlCopyMemory(newEndpointContext, EndpointContextToCopy, sizeof(ENDPOINT_CONTEXT64));
            }

            if (EndpointContext != NULL) {
                *EndpointContext = newEndpointContext;
            }

        } else {

            RtlZeroMemory(&inputContext->InputControlContext, sizeof(INPUT_CONTROL_CONTEXT32));

            newSlotContext = (PSLOT_CONTEXT64)UsbDevice_InputContext32GetSlot(
                (PINPUT_CONTEXT32)inputContext);

            orgSlotContext = (PSLOT_CONTEXT64)UsbDevice_DeviceContext32GetSlot(
                UsbDevice_GetDeviceContext32(EndpointData->UsbDeviceHandle));

            newEndpointContext = (PENDPOINT_CONTEXT64)UsbDevice_InputContext32GetEndpoint(
                (PINPUT_CONTEXT32)inputContext,
                deviceContextIndex);

            RtlCopyMemory(newSlotContext, orgSlotContext, sizeof(SLOT_CONTEXT32));

            if (EndpointContextToCopy != NULL) {
                RtlCopyMemory(newEndpointContext, EndpointContextToCopy, sizeof(ENDPOINT_CONTEXT32));
            }

            if (EndpointContext != NULL) {
                *EndpointContext = newEndpointContext;
            }
        }

        //
        // Since, on EndpointConfigureOperation_Drop, we drop an endpoint temporarily, then if that
        // has run, the controller may have decreased the value of ContextEntries in the output slot
        // context. Instead of reusing the controller's ContextEntries value, always set it to the
        // cached value so that this command doesn't inadvertently drop the last endpoint(s).
        //
        // TODO: PostWin8: Setting this could violate spec (ambiguous) if we drop the last endpoint
        // but this ContextEntries count indicates it's still valid. Cleaner solution: serialize
        // these operations per device. If we hit more issues then this extra work is needed.
        //
        newSlotContext->ContextEntries = UsbDevice_GetNumContextEntries(EndpointData->UsbDeviceHandle);

        inputContext->InputControlContext.AddContextFlags.A0 = 1;

        if (ConfigureType == EndpointConfigureOperation_Add) {

            inputContext->InputControlContext.AddContextFlags.AsUlong32 |= (1 << deviceContextIndex);

        } else if (ConfigureType == EndpointConfigureOperation_Drop) {

            inputContext->InputControlContext.DropContextFlags.AsUlong32 |= (1 << deviceContextIndex);

        } else if (ConfigureType == EndpointConfigureOperation_DropAndAdd) {
            
            inputContext->InputControlContext.DropContextFlags.AsUlong32 |= (1 << deviceContextIndex);
            inputContext->InputControlContext.AddContextFlags.AsUlong32 |= (1 << deviceContextIndex);

        } else {

            NT_ASSERTMSG("Unexpected ENDPOINT_CONFIGURE enum value", 0);
        }

        //
        // Disable it for now.
        //
#if 0
        if (Register_GetConfigurationInformation(registerHandle)) {
            inputContext->InputControlContext.ConfigurationValue = UsbDevice_GetConfigurationValue(EndpointData->UsbDeviceHandle);
            inputContext->InputControlContext.AlternateSetting = UsbDevice_GetAlternateSetting(EndpointData->UsbDeviceHandle);
            inputContext->InputControlContext.InterfaceNumber = UsbDevice_GetInterfaceNumber(EndpointData->UsbDeviceHandle);
        }
#endif

        //
        // Set up Configure Endpoint CRB
        //
        RtlZeroMemory(Crb, sizeof(*Crb));

        Crb->CommandCallback                                  = CommandCallback;
        Crb->ObjectHandle                                     = CallbackContext;
        Crb->Trb.ConfigureEndpointCommand.Type                = TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND;
        Crb->Trb.ConfigureEndpointCommand.Deconfigure         = 0;
        Crb->Trb.ConfigureEndpointCommand.SlotId              = UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle);
        Crb->Trb.ConfigureEndpointCommand.InputContextPointer = (ULONG64)InputContextCommonBuffer->LogicalAddress.QuadPart;
        Crb->InputContextVA                                   = inputContext;

    } FINALLY {

    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_OnCancelEndpointConfigure(
    __in
        PENDPOINT_DATA EndpointData
    )
/*++

Routine Description:

    This function is called after an endpoint stop for a stream enabled
    endpoint if the OnStopConfigureStreamEndpoint flag is set.
    It will send 2 configure endpoint commands (drop and add), updating the
    TR dequeue pointers.

--*/
{
    NTSTATUS            status;
    HANDLE              commandHandle;
    HANDLE              registerHandle;
    HANDLE              commonBufferHandle;
    PENDPOINT_CONTEXT64 orgEndpointContext;
    ULONG               deviceContextIndex;

    TRY {

        NT_ASSERT(EndpointData->StreamsCapable);

        commandHandle      = Controller_GetCommandHandle(EndpointData->ControllerHandle);
        registerHandle     = Controller_GetRegisterHandle(EndpointData->ControllerHandle);
        commonBufferHandle = Controller_GetCommonBufferHandle(EndpointData->ControllerHandle);
        deviceContextIndex = Endpoint_GetContextIndex(EndpointData);

        LogInfo(EndpointData->RecorderLog, Endpoint,
                "%d.%d: Reconfiguring Stream Endpoint after Stop Endpoint command completion",
                UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(EndpointData));

        //
        // Allocate the zero initialized common buffer that will be used
        // as the Input Device Context for the TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND.
        //
        // If the HCCPARAMS register Context Size (CSZ) bit is set to
        // '1' then the xHC uses 64-byte Context data structures.  If
        // this bit is cleared to '0' then the xHC uses 32-byte Context
        // data structures.
        //
        if (REGISTER_CONTEXTSIZE64 == Register_GetContextSize(registerHandle)) {

            orgEndpointContext = UsbDevice_DeviceContext64GetEndpoint(
                UsbDevice_GetDeviceContext64(EndpointData->UsbDeviceHandle),
                deviceContextIndex);

            EndpointData->InputBufferData =
                CommonBuffer_AcquireBuffer(commonBufferHandle,
                                           sizeof(INPUT_CONTEXT64),
                                           EndpointData,
                                           ENDPOINT_ALLOC_TAG2);
        } else {

            orgEndpointContext = (PENDPOINT_CONTEXT64)UsbDevice_DeviceContext32GetEndpoint(
                UsbDevice_GetDeviceContext32(EndpointData->UsbDeviceHandle),
                deviceContextIndex);

            EndpointData->InputBufferData =
                CommonBuffer_AcquireBuffer(commonBufferHandle,
                                           sizeof(INPUT_CONTEXT32),
                                           EndpointData,
                                           ENDPOINT_ALLOC_TAG2);
        }

        if (EndpointData->InputBufferData == NULL) {

            LogError(EndpointData->RecorderLog, Endpoint,
                     "%d.%d: Failed to acquire EndpointData->InputBufferData",
                     UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(EndpointData));

            //
            // We must reset the controller since it is required to do so when the endpoint goes to
            // a failed state. The failure here is rare.
            //
            Controller_ReportFatalError(EndpointData->ControllerHandle,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_NO_LIVE_DUMP,
                                        EndpointData->UsbDeviceHandle,
                                        EndpointData,
                                        NULL);

            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        //
        // First deconfigure the endpoint to take back ownership of the Stream Context Array (SCA).
        //
        // Since this is a stream enabled endpoint, the dequeue pointers are stored in the Stream
        // Context Array (SCA), which is currently owned by the host controller--we can't modify
        // it yet. Also save off the current Output Endpoint Context (orgEndpointContext) as a base
        // for writing the Input Endpoint Context later. The xHC must ignore the Input Endpoint
        // Context on drop, so we just store it there.
        //
        // We could do this with just 1 configure, but it would require an extra allocation of a
        // new SCA.
        //
        Endpoint_SetUpConfigureEndpointCommand(EndpointData,
                                               EndpointConfigureOperation_Drop,
                                               Endpoint_OnCancelEndpointConfigureCompletion_EpDropped,
                                               (HANDLE)EndpointData,
                                               EndpointData->InputBufferData,
                                               orgEndpointContext,
                                               NULL,
                                               &EndpointData->Crb);

        Command_SendCommand(commandHandle, &EndpointData->Crb);

        status = STATUS_SUCCESS;

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            NT_ASSERT(status == STATUS_INSUFFICIENT_RESOURCES);
            Endpoint_ReportCommandFailure(EndpointData);
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_OnCancelEndpointConfigureCompletion_EpDropped(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    Streams endpoint has been dropped temporarily. Update dequeue pointers and re-add the
    streams endpoint.

Arguments:

    EventTRB - To be used for logging Hardware verifier events

--*/
{
    NTSTATUS            status;
    PENDPOINT_DATA      endpointData;
    PENDPOINT_CONTEXT64 newEndpointContext;
    HANDLE              commonBufferHandle;
    HANDLE              commandHandle;

    UNREFERENCED_PARAMETER(EventTRB);

    TRY {

        endpointData = (PENDPOINT_DATA)Crb->ObjectHandle;

        NT_ASSERT(endpointData->StreamsCapable);

        commonBufferHandle = Controller_GetCommonBufferHandle(endpointData->ControllerHandle);
        commandHandle = Controller_GetCommandHandle(endpointData->ControllerHandle);

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the endpoint configure
            // command effectively failed.  The failure in this case can be ignored.
            //
            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Configure endpoint on stop (drop) - controller gone",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData));

            status = STATUS_UNSUCCESSFUL;
            LEAVE;

        } else if (Crb->CompletionCode != TRB_COMPLETION_CODE_SUCCESS) {

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: Configure endpoint on stop failed (drop): %!TrbCompletionCode!",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData),
                     (TRB_COMPLETION_CODE)Crb->CompletionCode);

            NT_ASSERTMSG("Configure endpoint on stop failed (drop)", 0);

            Controller_ReportFatalError(endpointData->ControllerHandle,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_ENDPOINTS_DECONFIGURE_FAILED,
                                        endpointData->UsbDeviceHandle,
                                        endpointData,
                                        NULL);

            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        LogInfo(endpointData->RecorderLog, Endpoint,
                "%d.%d: Configure endpoint on stop success (dropped now re-adding)",
                UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(endpointData));

        //
        // At this point, the stopped transfer event (and any other outstanding transfer events) 
        // may not have been processed.  When they are processed the driver will walk the 
        // transfer rings to identify the dequeue pointer and calculate the bytes transferred.
        // Since this may still need to happen, the transfer rings need to remain intact until then.
        // This means the ConfigureEndpointCommand is going to configure the endpoint with the same
        // stream context array contents as when it was deconfigured.  Later, SetTRDQPointer commands
        // will be sent for each stream, re-initializing the transfer rings.
        //

        Endpoint_SetUpConfigureEndpointCommand(endpointData,
                                               EndpointConfigureOperation_Add,
                                               Endpoint_OnCancelEndpointConfigureCompletion,
                                               (HANDLE)endpointData,
                                               endpointData->InputBufferData,
                                               NULL,
                                               &newEndpointContext,
                                               &endpointData->Crb);

        //
        // We stashed the last Output Endpoint Context in the Input Endpoint Context, so it is
        // already mostly set up.
        //
        newEndpointContext->DequeuePointer = Endpoint_GetDequeuePointer(endpointData, 0);

        Command_SendCommand(commandHandle, &endpointData->Crb);

        status = STATUS_SUCCESS;

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            CommonBuffer_ReleaseBuffer(commonBufferHandle, endpointData->InputBufferData);
            endpointData->InputBufferData = NULL;

            Endpoint_ReportCommandFailure(endpointData);
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_OnCancelEndpointConfigureCompletion(
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
    TRB_TYPE_CONFIGURE_ENDPOINT_COMMAND and was originated on
    a stop endpoint request for a stream enabled
    endpoint if the OnStopConfigureStreamEndpoint flag is set.

Arguments:

    EventTRB - To be used for logging Hardware verifier events

--*/
{
    NTSTATUS        status;
    PENDPOINT_DATA  endpointData;
    HANDLE          commonBufferHandle;

    UNREFERENCED_PARAMETER(EventTRB);

    TRY {

        endpointData = (PENDPOINT_DATA)Crb->ObjectHandle;

        NT_ASSERT(endpointData->StreamsCapable);

        commonBufferHandle = Controller_GetCommonBufferHandle(endpointData->ControllerHandle);

        CommonBuffer_ReleaseBuffer(commonBufferHandle, endpointData->InputBufferData);
        endpointData->InputBufferData = NULL;

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the endpoint configure
            // command effectively failed.  The failure in this case can be ignored.
            //
            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Configure endpoint on stop (add) - controller gone",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData));

            status = STATUS_UNSUCCESSFUL;

        } else if (Crb->CompletionCode == TRB_COMPLETION_CODE_SUCCESS) {

            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Configure endpoint on stop success (add)",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData));

            //
            // Now the sequence number for this endpoint has been reset, so we
            // need to inform the device
            // 
            Endpoint_SendClearStallTransfer(endpointData);
            status = STATUS_SUCCESS;

        } else {

            //
            // This failure is unexpected. Most likely something is wrong in the controller
            // requiring reset. (There is also a rare case where another new endpoint on the
            // controller can sneak in and get resources needed by this stream endpoint.)
            //
            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: Configure endpoint on stop failed (add): %!TrbCompletionCode!",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData),
                     (TRB_COMPLETION_CODE)Crb->CompletionCode);

            NT_ASSERTMSG("Configure endpoint on stop failed (add)", 0);

            Controller_ReportFatalError(endpointData->ControllerHandle,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_ENDPOINTS_RECONFIGURE_FAILED_ON_CANCEL,
                                        endpointData->UsbDeviceHandle,
                                        endpointData,
                                        NULL);

            status = STATUS_UNSUCCESSFUL;
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            Endpoint_ReportCommandFailure(endpointData);
        }
    }

    return;
}

VOID
Endpoint_OnCancelStopCompletion(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This function is called on a stop endpoint command completion event.

Arguments:

    EventTRB - To be used for logging Hardware verifier events

--*/
{
    PENDPOINT_DATA      endpointData;
    ULONG               endpointState;
    ULONG64             xhciDequeuePointer;

    TRY {

        endpointData = (PENDPOINT_DATA)Crb->ObjectHandle;

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the endpoint is
            // effectively stopped so go ahead and cancel the endpoint
            // transfers now.
            //
            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: controller gone - cancel transfers",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData));

            Endpoint_ReportCommandFailure(endpointData);
            LEAVE;
        }

        endpointState =
            UsbDevice_GetEndpointState(endpointData->UsbDeviceHandle,
                                       Endpoint_GetContextIndex(endpointData));

        xhciDequeuePointer =
            UsbDevice_GetXhciEndpointDequeuePointer(endpointData->UsbDeviceHandle,
                                                    Endpoint_GetContextIndex(endpointData));

        LogInfo(endpointData->RecorderLog, Endpoint,
                "%d.%d: OnCancelStop Complete %!TrbCompletionCode! : %!EndpointState! xhciDequeuePointer 0x%I64x",
                UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(endpointData),
                (TRB_COMPLETION_CODE)Crb->CompletionCode,
                (ENDPOINT_STATE)endpointState,
                xhciDequeuePointer);

        switch (Crb->CompletionCode) {

        case TRB_COMPLETION_CODE_SUCCESS:

            if (endpointState != ENDPOINT_STATE_STOPPED) {

                LogError(endpointData->RecorderLog, Endpoint,
                         "%d.%d: Endpoint not stopped after successful stop endpoint completion: %!EndpointState!",
                         UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                         Endpoint_GetContextIndex(endpointData),
                         (ENDPOINT_STATE)endpointState);
                
                Controller_ReportFatalError(endpointData->ControllerHandle,
                                            Controller_FatalErrorReset,
                                            USB3_WER_BUGCODE_USBXHCI_ENDPOINT_NOT_STOPPED_AFTER_STOP_ENDPOINT_SUCCESS,
                                            endpointData->UsbDeviceHandle,
                                            endpointData,
                                            NULL);

                Endpoint_ReportCommandFailure(endpointData);

            } else {

                ESM_AddEsmEvent(endpointData, EsmEventCommandCompletionSuccess);
            }
            break;

        case TRB_COMPLETION_CODE_CONTEXT_STATE_ERROR:

            switch (endpointState) {

            case ENDPOINT_STATE_RUNNING:

                //
                // The xHC had an internal race between seeing the doorbell ring and processing
                // or the stop endpoint command. Although the doorbell was rung by software
                // before issuing the stop endpoint command, the xHC saw the stop endpoint
                // command first and failed it because the endpoint was in the Stopped state
                // at the time. Now, when we look, the xHC has seen the doorbell ring, and has
                // transitioned the endpoint to the Runnnig state. In this case, a second stop
                // endpoint command can 
                //
                ESM_AddEsmEvent(endpointData, EsmEventStopEndpointFailureWithEndpointRunning);
                break;

            case ENDPOINT_STATE_STOPPED:

                //
                // Software will only issue the Stop Endpoint command if it has rung the doorbell
                // atleast once (i.e. the endpoint is in the running state from software's
                // perspective). But it has been observed that some controllers see the stop
                // endpoint command before the doorbell ring, and fail the command with context
                // state error because the endpoint is in the Stopped state. If this happens, we
                // the endpoint is expected to transition to the Running state whenever the xHC
                // processes the doorbell ring. So we issue a Stop Endpoint command repeatedly until
                // the xHC completes it with success.
                //
                ESM_AddEsmEvent(endpointData, EsmEventStopEndpointFailureWithEndpointStopped);
                break;

            case ENDPOINT_STATE_HALTED:

                if (Endpoint_GetTransferType(endpointData) != USB_ENDPOINT_TYPE_ISOCHRONOUS) {

                    //
                    // The EP was halted while trying to stop the endpoint.
                    //
                    ESM_AddEsmEvent(endpointData, EsmEventStopEndpointFailureWithEndpointHalted);

                } else {






                    Controller_ReportFatalError(endpointData->ControllerHandle,
                                                Controller_FatalErrorReset,
                                                USB3_WER_BUGCODE_USBXHCI_ENDPOINT_ERROR_AFTER_STOP,
                                                endpointData->UsbDeviceHandle,
                                                endpointData,
                                                NULL);

                    Endpoint_ReportCommandFailure(endpointData);
                }
                break;

            default:

                Controller_ReportFatalError(endpointData->ControllerHandle,
                                            Controller_FatalErrorReset,
                                            USB3_WER_BUGCODE_USBXHCI_CONTEXT_ERROR_AFTER_STOP,
                                            endpointData->UsbDeviceHandle,
                                            endpointData,
                                            NULL);

                Endpoint_ReportCommandFailure(endpointData);
                break;
            }
            break;

        case TRB_COMPLETION_CODE_SLOT_NOT_ENABLED_ERROR:
        default:

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: Unhandled CompletionCode %!TrbCompletionCode!",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData),
                     (TRB_COMPLETION_CODE)Crb->CompletionCode);
            
            Controller_HwVerifierBreakIfEnabled(
                endpointData->ControllerHandle,
                endpointData->UcxUsbDevice,
                endpointData->UcxEndpoint,
                ControllerHwVerifierStopEndpointFailed,
                "Stop Endpoint Command failed",
                &Crb->Trb,
                EventTRB);
            
            Controller_ReportFatalError(endpointData->ControllerHandle,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_ENDPOINT_ERROR_AFTER_STOP,
                                        endpointData->UsbDeviceHandle,
                                        endpointData,
                                        NULL);

            Endpoint_ReportCommandFailure(endpointData);
            break;
        }

    } FINALLY {

    }

    return;
}

VOID
Endpoint_ControlEndpointResetSetDequeuePointerCompletion(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This function is called on an endpoint reset command completion event that was
    issued for a transfer request.

Arguments:

    EventTRB - Used for logging Hardware verifier events

--*/
{
    PENDPOINT_DATA  endpointData;
    BOOLEAN         onError;

    TRY {

        endpointData = (PENDPOINT_DATA)(Crb->ObjectHandle);

        onError = TRUE;

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the set dequeue
            // pointer command effectively failed.  The failure in this
            // case can be ignored.
            //
            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: controller gone",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData));

        } else if (Crb->CompletionCode == TRB_COMPLETION_CODE_SUCCESS) {

            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: OnStallSetDequeuePointer Complete with %!TrbCompletionCode!",
                    (TRB_COMPLETION_CODE)Crb->CompletionCode,
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData));

            onError = FALSE;

        } else {

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: OnStallSetDequeuePointer Failed with %!TrbCompletionCode!",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData),
                     (TRB_COMPLETION_CODE)Crb->CompletionCode);
            
            Controller_HwVerifierBreakIfEnabled(
                endpointData->ControllerHandle,
                endpointData->UcxUsbDevice,
                endpointData->UcxEndpoint,
                ControllerHwVerifierSetDequeuePointerAfterEndpointResetFailed,
                "Set Dequeue pointer command following a Reset Endpoint command failed",
                &Crb->Trb,
                EventTRB);

            Controller_ReportFatalError(endpointData->ControllerHandle,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_CONTEXT_ERROR_AFTER_STALL_SET_DEQUEUE,
                                        endpointData->UsbDeviceHandle,
                                        endpointData,
                                        NULL);
        }

    } FINALLY {

        if (onError) {
            Endpoint_ReportCommandFailure(endpointData);
        } else {
            endpointData->EndpointResetCompletionStatus = STATUS_SUCCESS;
            ESM_AddEsmEvent(endpointData, EsmEventCommandCompletionSuccess);
        }
    }

    return;
}

VOID
Endpoint_ControlEndpointResetCompletion(
    __in
        PCRB                    Crb,
    __in
        CRB_COMPLETION_STATUS   CrbCompletionStatus,
    __in_opt
        PTRB                    EventTRB
    )
/*++

Routine Description:

    This function is called to send a set dequeue pointer command when a stall is
    received on a transfer request.

Arguments:

    EventTRB - Used for logging Hardware verifier events

--*/
{
    NTSTATUS        status;
    PENDPOINT_DATA  endpointData;
    HANDLE          commandHandle;

    TRY {

        endpointData = (PENDPOINT_DATA)(Crb->ObjectHandle);

        if (CrbCompletionStatus == CrbCompletionControllerGone) {

            //
            // If the controller has been removed then the reset
            // endpoint command effectively failed.
            //
            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Endpoint reset - controller gone",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData));

            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        //
        // If the endpoint state was not halted, then the reset endpoint returns a context
        // state error. When this happens, continue with the dequeue pointer update.
        //
        if (Crb->CompletionCode != TRB_COMPLETION_CODE_SUCCESS) {

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: Endpoint reset failed: %!TrbCompletionCode!",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData),
                     (TRB_COMPLETION_CODE)Crb->CompletionCode);

            Controller_HwVerifierBreakIfEnabled(
                endpointData->ControllerHandle,
                endpointData->UcxUsbDevice,
                endpointData->UcxEndpoint,
                ControllerHwVerifierEndpointResetFailed,
                "Endpoint Reset Command failed",
                &Crb->Trb,
                EventTRB);

            Controller_ReportFatalError(endpointData->ControllerHandle,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_CONTEXT_ERROR_AFTER_STALL_RESET_EP,
                                        endpointData->UsbDeviceHandle,
                                        endpointData,
                                        NULL);

            status = STATUS_UNSUCCESSFUL;
            LEAVE;
        }

        commandHandle = Controller_GetCommandHandle(endpointData->ControllerHandle);

        TR_InitializeTransferRing(endpointData->TransferRingHandle);

        RtlZeroMemory(Crb, sizeof(CRB));

        Crb->CommandCallback                             = Endpoint_ControlEndpointResetSetDequeuePointerCompletion;
        Crb->ObjectHandle                                = (HANDLE)endpointData;
        Crb->Trb.SetDequeuePointerCommand.Type           = TRB_TYPE_SET_DEQUEUE_POINTER_COMMAND;
        Crb->Trb.SetDequeuePointerCommand.DequeuePointer = Endpoint_GetDequeuePointer(endpointData, 0);
        Crb->Trb.SetDequeuePointerCommand.EndpointId     = Endpoint_GetContextIndex(endpointData);
        Crb->Trb.SetDequeuePointerCommand.SlotId         = UsbDevice_GetSlotId(endpointData->UsbDeviceHandle);

        LogInfo(endpointData->RecorderLog, Endpoint,
                "%d.%d: Reset Success - OnStallSetDequeuePointer Begin : xhciDequeuePointer 0x%I64x",
                Crb->Trb.SetDequeuePointerCommand.SlotId,
                Crb->Trb.SetDequeuePointerCommand.EndpointId,
                Crb->Trb.SetDequeuePointerCommand.DequeuePointer);

        Command_SendCommand(commandHandle, Crb);

        status = STATUS_SUCCESS;

    } FINALLY {

        if (!NT_SUCCESS(status)) {
            
            Endpoint_ReportCommandFailure(endpointData);
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_InitializeTransferRing(
    __in
        PENDPOINT_DATA  EndpointData,
    __in_opt
        ULONG           StreamId
    )
/*++

Routine Description:

    Wraps TR_InitializeTransferRing. Transfer ring must be empty.

--*/
{
    if (EndpointData->StreamsCapable) {

        NT_ASSERT(StreamId != 0);

        if (Controller_GetDeviceFlags(EndpointData->ControllerHandle).StreamEDTLAReinitNeeded) {
            Endpoint_ResetStreamContextEdtla(EndpointData,StreamId);
        }

        TR_InitializeTransferRing(Endpoint_GetStreamHandle(EndpointData, StreamId));

    } else {

        TR_InitializeTransferRing(EndpointData->TransferRingHandle);
    }

    return;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Endpoint_UnhandledCompletionCode(
    __in
        ULONG   CompletionCode
    )
/*++

    This function returns TRUE if the completion code is unhandled by the xHCI driver.
    Endpoint_HaltedCompletionCode contains a list of all completion codes. This function
    handles those that lead to an error or unknown state.

--*/
{
    if ((CompletionCode == TRB_COMPLETION_CODE_TRB_ERROR) ||
        (CompletionCode == TRB_COMPLETION_CODE_UNDEFINED_ERROR) ||
        ((CompletionCode >= TRB_COMPLETION_CODE_VENDOR_ERROR_MIN) &&
         (CompletionCode <= TRB_COMPLETION_CODE_VENDOR_ERROR_MAX))) {

        return(TRUE);

    } else {

        return(FALSE);
    }
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Endpoint_HaltedCompletionCode(
    __in
        ULONG   CompletionCode
    )
/*++

    This function returns TRUE if the completion code results in an endpoint halted
    state. Otherwise, it returns FALSE. The below is a list of all TRB completion
    codes along with the corresponding endpoint state, if applicable.

#define TRB_COMPLETION_CODE_SUCCESS                             1       running
#define TRB_COMPLETION_CODE_DATA_BUFFER_ERROR                   2       halted
#define TRB_COMPLETION_CODE_BABBLE_DETECTED_ERROR               3       halted
#define TRB_COMPLETION_CODE_USB_TRANSACTION_ERROR               4       halted
#define TRB_COMPLETION_CODE_TRB_ERROR                           5       error
#define TRB_COMPLETION_CODE_STALL_ERROR                         6       halted
#define TRB_COMPLETION_CODE_RESOURCE_ERROR                      7       not applicable
#define TRB_COMPLETION_CODE_BANDWIDTH_ERROR                     8       not applicable
#define TRB_COMPLETION_CODE_NO_SLOTS_AVAILABLE_ERROR            9       not applicable
#define TRB_COMPLETION_CODE_INVALID_STREAM_TYPE_ERROR           10      halted
#define TRB_COMPLETION_CODE_SLOT_NOT_ENABLED_ERROR              11      not applicable
#define TRB_COMPLETION_CODE_ENDPOINT_NOT_ENABLED_ERROR          12      not applicable
#define TRB_COMPLETION_CODE_SHORT_PACKET                        13      running
#define TRB_COMPLETION_CODE_RING_UNDERRUN                       14      not applicable
#define TRB_COMPLETION_CODE_RING_OVERRUN                        15      not applicable
#define TRB_COMPLETION_CODE_VF_EVENT_RING_FULL_ERROR            16      not applicable
#define TRB_COMPLETION_CODE_PARAMETER_ERROR                     17      not applicable
#define TRB_COMPLETION_CODE_BANDWIDTH_OVERRUN_ERROR             18      not applicable
#define TRB_COMPLETION_CODE_CONTEXT_STATE_ERROR                 19      not applicable
#define TRB_COMPLETION_CODE_NO_PING_RESPONSE_ERROR              20      not applicable
#define TRB_COMPLETION_CODE_EVENT_RING_FULL_ERROR               21      not applicable
#define TRB_COMPLETION_CODE_INCOMPATIBLE_DEVICE_ERROR           22      not applicable
#define TRB_COMPLETION_CODE_MISSED_SERVICE_ERROR                23      not applicable
#define TRB_COMPLETION_CODE_COMMAND_RING_STOPPED                24      not applicable
#define TRB_COMPLETION_CODE_COMMAND_ABORTED                     25      not applicable
#define TRB_COMPLETION_CODE_STOPPED                             26      stopped
#define TRB_COMPLETION_CODE_STOPPED_LENGTH_INVALID              27      stopped
#define TRB_COMPLETION_CODE_STOPPED_SHORT_PACKET                28      stopped
#define TRB_COMPLETION_CODE_MAX_EXIT_LATENCY_TOO_LARGE_ERROR    29      not applicable
#define TRB_COMPLETION_CODE_ISOCH_BUFFER_OVERRUN                31      not applicable
#define TRB_COMPLETION_CODE_EVENT_LOST_ERROR                    32      halted*
#define TRB_COMPLETION_CODE_UNDEFINED_ERROR                     33      unknown
#define TRB_COMPLETION_CODE_INVALID_STREAM_ID_ERROR             34      halted
#define TRB_COMPLETION_CODE_SECONDARY_BANDWIDTH_ERROR           35      not applicable
#define TRB_COMPLETION_CODE_SPLIT_TRANSACTION_ERROR             36      halted

* halted if due to TD related events

--*/
{
    BOOLEAN haltedEvent;

    if ((CompletionCode == TRB_COMPLETION_CODE_DATA_BUFFER_ERROR) ||
        (CompletionCode == TRB_COMPLETION_CODE_BABBLE_DETECTED_ERROR) ||
        (CompletionCode == TRB_COMPLETION_CODE_USB_TRANSACTION_ERROR) ||
        (CompletionCode == TRB_COMPLETION_CODE_STALL_ERROR) ||
        (CompletionCode == TRB_COMPLETION_CODE_INVALID_STREAM_TYPE_ERROR) ||
        (CompletionCode == TRB_COMPLETION_CODE_EVENT_LOST_ERROR) ||
        (CompletionCode == TRB_COMPLETION_CODE_INVALID_STREAM_ID_ERROR) ||
        (CompletionCode == TRB_COMPLETION_CODE_SPLIT_TRANSACTION_ERROR)) {

        haltedEvent = TRUE;

    } else {

        haltedEvent = FALSE;
    }

    return haltedEvent;
}

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Endpoint_StoppedCompletionCode(
    __in
        ULONG   CompletionCode
    )
{
    return ((CompletionCode == TRB_COMPLETION_CODE_STOPPED) ||
            (CompletionCode == TRB_COMPLETION_CODE_STOPPED_SHORT_PACKET) ||
            (CompletionCode == TRB_COMPLETION_CODE_STOPPED_LENGTH_INVALID));
}

__drv_requiresIRQL(DISPATCH_LEVEL)
VOID
Endpoint_TransferEventHandler(
    __in
        PTRANSFER_EVENT_TRB TransferEventTrb,
    __in
        PENDPOINT_DATA      EndpointData
    )
/*++

Routine Description:

    The function handles a completed transfer event. Note that this function
    handles transfer events where an error occurred on the transfer.

--*/
{
    PSTATIC_STREAMS_DATA    staticStreamsData;
    ULONG                   i;
    BOOLEAN                 tdFound;
    BOOLEAN                 pointsToNoOpTRB;

    TRY {

        //
        // If this is an unhandled completion code, initiate non-pnp error recovery.
        //
        if (Endpoint_UnhandledCompletionCode(TransferEventTrb->CompletionCode)) {

            Controller_HwVerifierBreakIfEnabled(
                EndpointData->ControllerHandle,
                EndpointData->UcxUsbDevice,
                EndpointData->UcxEndpoint,
                ControllerHwVerifierUnrecognizedTransferEventTrbCompletionCode,
                "Unrecognized completion code in Transfer Event TRB",
                NULL,
                &(EndpointData->Crb.Trb));

            
            Controller_ReportFatalError(EndpointData->ControllerHandle,
                                        Controller_FatalErrorReset,
                                        USB3_WER_BUGCODE_USBXHCI_UNHANDLED_TRANSFER_EVENT_TRB_COMPLETION_CODE,
                                        EndpointData->UsbDeviceHandle,
                                        EndpointData,
                                        NULL);
            LEAVE;
        }

        if (!EndpointData->StreamsCapable) {

            TR_TransferEventHandler(TransferEventTrb, EndpointData->TransferRingHandle);
            LEAVE;
        }

        //
        // Streams case is the complex one. There is the SCA the event could point into, or the
        // event could be for any stream on this endpoint.
        //
        staticStreamsData = EndpointData->StaticStreamsData;
        tdFound = FALSE;

        if (TransferEventTrb->Pointer.QuadPart == 0) {

            //
            // This is the case where the TRB pointer is 0, which means that a failure
            // occurred that can not be associated with a TD.
            //
            LogInfo(EndpointData->RecorderLog, Endpoint,
                    "%d.%d: TransferEventTrb 0x%p Pointer == 0 %!TrbCompletionCode! : Set TRB Processed for all streams",
                    UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(EndpointData),
                    TransferEventTrb, (TRB_COMPLETION_CODE)TransferEventTrb->CompletionCode);

        } else if ((TransferEventTrb->Pointer.QuadPart >= Endpoint_GetStreamContextArrayLA(EndpointData)) &&
                   (TransferEventTrb->Pointer.QuadPart <
                    (Endpoint_GetStreamContextArrayLA(EndpointData) + staticStreamsData->StreamContextArray->Size))) {

            //
            // This is the case where the TRB pointer points to the stream context array.
            //
            LogInfo(EndpointData->RecorderLog, Endpoint,
                    "%d.%d: TransferEventTrb 0x%p Pointer = StreamContextArray %!TrbCompletionCode! : Set TRB Processed for all streams",
                    UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(EndpointData),
                    TransferEventTrb, (TRB_COMPLETION_CODE)TransferEventTrb->CompletionCode);

        } else {

            //
            // Iterate through each transfer ring and attempt to match the transfer event
            // TRB to a request on a transfer ring. The TD should always be found, except
            // in the case where this is a FSE pointing to a dequeue pointer (end of transfer
            // ring).
            //
            // If ValidateTransferEventPointer is set, we run this loop even when
            // EventData == 1, to be safe from derferencing a bad pointer. This is a performance
            // hit in the hot path for controllers with this flag set.
            //
            for (i = 1; i <= staticStreamsData->NumberOfStreams; i += 1) {

                tdFound = TR_TransferEventHandler(TransferEventTrb,
                                                  Endpoint_GetStreamHandle(EndpointData, i));

                if (tdFound) {

                    LogVerbose(EndpointData->RecorderLog, Endpoint,
                               "%d.%d.%d: Staging data found",
                               UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                               Endpoint_GetContextIndex(EndpointData), i+1);
                    break;
                }
            }

            //
            // If the TD was found, then leave.
            //
            if (tdFound) {
                LEAVE;
            }

            if (TransferEventTrb->EventData == 1) {

                //
                // We expect the pointer to match a pending STAGING_DATA on some stream whenever
                // EventData == 1, and do not tolerate duplicate events in this case.
                //
                LogError(EndpointData->RecorderLog, Endpoint,
                          "%d.%d: Staging data not valid TRB.Pointer 0x%I64x (stream EP)",
                          UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                          Endpoint_GetContextIndex(EndpointData),
                          TransferEventTrb->Pointer.QuadPart);

                NT_ASSERTMSG("Failed to validate stagingData for stream transfer event", 0);

                Controller_ReportFatalError(EndpointData->ControllerHandle,
                                            Controller_FatalErrorReset,
                                            USB3_WER_BUGCODE_USBXHCI_INVALID_TRANSFER_EVENT_PTR_ED_1,
                                            EndpointData->UsbDeviceHandle,
                                            EndpointData,
                                            NULL);

                LEAVE;
            }

            if ((TransferEventTrb->CompletionCode == TRB_COMPLETION_CODE_STOPPED) ||
                (TransferEventTrb->CompletionCode == TRB_COMPLETION_CODE_STOPPED_LENGTH_INVALID) ||
                (TransferEventTrb->CompletionCode == TRB_COMPLETION_CODE_STOPPED_SHORT_PACKET)) {

                //
                // Historically we have just taken this to mean TrbProcessed, without being able to
                // find the Transfer Event TRB pointer pointing somewhere vaild in any stream. This
                // is probably a hardware error and should be flagged. For now let's assert and,
                // going forward, we should do controller reset just like the "else" case, if the
                // assert isn't hit in any valid situations.
                //
                NT_ASSERTMSG("No TRB found corresponding to stopped transfer event", 0);

                LogError(EndpointData->RecorderLog, Endpoint,
                           "%d.%d: HW_COMPLIANCE: Set TRB Processed for all streams for FSE, TRB pointer not matched. (No TRB found corresponding to stopped transfer event)",
                           UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                           Endpoint_GetContextIndex(EndpointData));

            } else {

                //
                // The TD wasn't found for any stream. This shouldn't happen, since the TRB
                // pointer should have either matched a TD or received an FSE.
                //
                // Perform EventData=0 duplicate transfer event detection.
                // The main purpose of duplicate detection is to differentiate the live dump
                // buckets. Also allows us to avoid resetting controller too often if a controller
                // is known to send duplicates.
                //
                if (Endpoint_Stream_IsTransferEventLikelyDuplicate(TransferEventTrb,
                                                                   EndpointData,
                                                                   &pointsToNoOpTRB)) {

                    if (Controller_GetDeviceFlags(EndpointData->ControllerHandle).DropDuplicateEd0TransferEvents) {

                        //
                        // Probable duplicate event and we don't want to reset the controller too
                        // often. Drop the event.
                        //
                        LogWarning(EndpointData->RecorderLog, Endpoint,
                                   "%d.%d: Dropping invalid transfer event, likely duplicate TRB.Pointer 0x%I64x",
                                   UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                                   Endpoint_GetContextIndex(EndpointData),
                                   TransferEventTrb->Pointer.QuadPart);

                    } else if (pointsToNoOpTRB &&
                               Endpoint_StoppedCompletionCode(TransferEventTrb->CompletionCode)) {

                        LogWarning(EndpointData->RecorderLog, Endpoint,
                                   "%d.%d: Dropping Stopped Transfer Event pointing to No-Op TRB at 0x%I64x",
                                   UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                                   Endpoint_GetContextIndex(EndpointData),
                                   TransferEventTrb->Pointer.QuadPart);

                    } else {

                        //
                        // Probably a duplicate event, but invalid nonetheless.
                        //
                        NT_ASSERTMSG("Likely duplicate stream transfer event", 0);

                        LogError(EndpointData->RecorderLog, Endpoint,
                                 "%d.%d: Invalid transfer event, likely duplicate TRB.Pointer 0x%I64x",
                                 UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                                 Endpoint_GetContextIndex(EndpointData),
                                 TransferEventTrb->Pointer.QuadPart);

                        Controller_ReportFatalError(
                            EndpointData->ControllerHandle,
                            Controller_FatalErrorReset,
                            USB3_WER_BUGCODE_USBXHCI_INVALID_TRANSFER_EVENT_PTR_ED_0_DUPLICATE,
                            EndpointData->UsbDeviceHandle,
                            EndpointData,
                            NULL);
                    }

                } else {

                    //
                    // Doesn't look like a duplicate. No idea what to make of this event.
                    //
                    NT_ASSERTMSG("Failed to find stagingData for stream transfer event", 0);

                    LogError(EndpointData->RecorderLog, Endpoint,
                             "%d.%d: Invalid transfer event TRB.Pointer 0x%I64x",
                             UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                             Endpoint_GetContextIndex(EndpointData),
                             TransferEventTrb->Pointer.QuadPart);

                    Controller_ReportFatalError(
                        EndpointData->ControllerHandle,
                        Controller_FatalErrorReset,
                        USB3_WER_BUGCODE_USBXHCI_INVALID_TRANSFER_EVENT_PTR_ED_0,
                        EndpointData->UsbDeviceHandle,
                        EndpointData,
                        NULL);
                }

                LEAVE;
            }
        }

        //
        // Any code at this point should only run if the TD was not found for the stream endpoint,
        // but also the event is not completely unrecognized.
        //
        NT_ASSERT(tdFound == FALSE);

        //
        // If this is a halted completion code, then set the pending reset event and
        // purge all stream WDF queues. The queues will stay purged until the endpoint
        // is reset. Until the endpoint is reset, all requests presented to the queue
        // are completed with the halted completion code value.
        //
        if (Endpoint_HaltedCompletionCode(TransferEventTrb->CompletionCode)) {

            Endpoint_TR_HaltedCompletionCodeReceived(EndpointData,
                                                     TransferEventTrb->CompletionCode,
                                                     FALSE);

        } else if (Endpoint_StoppedCompletionCode(TransferEventTrb->CompletionCode)) {

            Endpoint_TR_FSEReceived(EndpointData);

        } else {

            NT_ASSERTMSG("Unexpected TRB Completion code", 0);
        }

    } FINALLY {

    }

    return;
}

//
// STREAM Additions
//

__drv_requiresIRQL(DISPATCH_LEVEL)
BOOLEAN
Endpoint_Stream_IsTransferEventLikelyDuplicate(
    __in
        PTRANSFER_EVENT_TRB TransferEventTrb,
    __in
        PENDPOINT_DATA      EndpointData,
    __out
        PBOOLEAN            PointsToNoOpTRB
    )
/*++

Routine Description:

    Determine if the EventData==0 TransferEventTrb is likely a duplicate, or totally bogus.

Arguments:

    TransferEventTrb - Transfer Event TRB with EventData == 0

    EndpointData - Stream-enabled endpoint

Return Value:

    TRUE if we think the completion is a duplicate
    FALSE otherwise (we still don't know what to make of the completion)

--*/
{
    BOOLEAN                 likelyDuplicate;
    PSTATIC_STREAMS_DATA    staticStreamsData;
    HANDLE                  transferRingHandle;
    ULONG                   i;

    TRY {

        likelyDuplicate   = FALSE;
        *PointsToNoOpTRB  = FALSE;
        staticStreamsData = EndpointData->StaticStreamsData;

        //
        // Ask each stream if this might be a duplicate for that stream.
        //
        for (i = 1; i <= staticStreamsData->NumberOfStreams; i += 1) {

            transferRingHandle = Endpoint_GetStreamHandle(EndpointData, i);

            likelyDuplicate = TR_IsTransferEventLikelyDuplicate(transferRingHandle,
                                                                TransferEventTrb->Pointer,
                                                                PointsToNoOpTRB);

            if (likelyDuplicate) {

                break;
            }
        }

    } FINALLY {

    }

    return likelyDuplicate;
}

__drv_maxIRQL(DISPATCH_LEVEL)
ULONG64
Endpoint_GetDequeuePointer(
    __in
        PENDPOINT_DATA  EndpointData,
    __in
        ULONG           StreamId
    )
/*++

Return Value:

    This routine returns the DequeuePointer value to be set in the SetDequeuePointerCommand TRB
    or in the Endpoint Context.

    Caller should specify the StreamId as 0 to retrieve the DequeuePointer value to be set in
    the EndpointContext, in which case this routine will return the StreamContextArray LA for
    a stream capable endpoint.

--*/
{
    if (EndpointData->StreamsCapable) {

        if (StreamId == 0) {
            return Endpoint_GetStreamContextArrayLA(EndpointData);
        } else {
            return TR_GetDequeuePointer(Endpoint_GetStreamHandle(EndpointData, StreamId));
        }

    } else {

        return TR_GetDequeuePointer(EndpointData->TransferRingHandle);
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
ULONG
Endpoint_GetMaxPrimaryStreams(
    __in
        PENDPOINT_DATA  EndpointData
    )
/*++

Return Value:

    A value of '0' indicates that Streams are not supported by this
    endpoint.

    A value of '1' to '15' indicates that the Primary Stream ID Width is
    (MaxPStreams+1) and the Primary Stream Array contains
    2**(MaxPStreams+1) entries.

    This value is used as the Endpoint Context Max Primary Streams value.

--*/
{
    if (EndpointData->StreamsCapable) {

        return EndpointData->StaticStreamsData->MaxPrimaryStreams;

    } else {

        return 0;
    }
}

NTSTATUS
Endpoint_UcxEvtEndpointStaticStreamsAdd(
    UCXENDPOINT         UcxEndpoint,
    ULONG               NumberOfStreams,
    PUCXSSTREAMS_INIT   UcxStaticStreamsInit
    )
/*++

Routine Description:

    This routine handles the creation of the non-default streams
    associated with a URB_FUNCTION_OPEN_STATIC_STREAMS request.

    If the non-default streams are successfully created they are not enabled
    until a subsequent call to Endpoint_UcxEvtEndpointStaticStreamsEnable().

Return Value:

    STATUS_SUCCESS if the non-default streams are successfully created.

--*/
{
    NTSTATUS                    status;
    PENDPOINT_DATA              endpointData;
    STREAM_INFO                 streamInfo;
    ULONG                       streamId;
    WDF_OBJECT_ATTRIBUTES       wdfAttributes;
    UCXSSTREAMS                 ucxStaticStreams;
    PSTATIC_STREAMS_DATA        staticStreamsData;
    ULONG                       i;

    TRY {

        endpointData = GetEndpointData(UcxEndpoint);

        LogInfo(endpointData->RecorderLog, Endpoint,
                "%d.%d: Request to add %d streams",
                UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(endpointData), NumberOfStreams);

        if (endpointData->StreamsCapable == FALSE) {

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: Endpoint is not Streams Capable",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData));

            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        if ((NumberOfStreams == 0) ||
            (NumberOfStreams > Register_GetSupportedNumberOfStreams(
                                   Controller_GetRegisterHandle(endpointData->ControllerHandle)))) {

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: Invalid Number Of Streams %d",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData), NumberOfStreams);

            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        //
        // Create the UCXSSTREAMS object.
        //
        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&wdfAttributes, STATIC_STREAMS_DATA);

        wdfAttributes.ContextSizeOverride =
            FIELD_OFFSET(STATIC_STREAMS_DATA, StaticStreamData) +
            (NumberOfStreams * RTL_FIELD_SIZE(STATIC_STREAMS_DATA, StaticStreamData[0]));

        wdfAttributes.EvtCleanupCallback = Endpoint_EvtStaticStreamsCleanupCallback;

        status = UcxStaticStreamsCreate(UcxEndpoint,
                                        &UcxStaticStreamsInit,
                                        &wdfAttributes,
                                        &ucxStaticStreams);

        if (!NT_SUCCESS(status)) {

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: UcxStaticStreamsCreate Failed %!STATUS!",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData), status);
            LEAVE;
        }

        //
        // Initialize the context data for the UCXSSTREAMS object.
        //
        staticStreamsData = GetStaticStreamsData(ucxStaticStreams);

        staticStreamsData->EndpointData    = endpointData;
        staticStreamsData->NumberOfStreams = NumberOfStreams;

        //
        // The Primary Stream Array contains 2**(MaxPStreams+1)
        // entries.  A StreamId value of 0 is reserved.
        //
        // NumberOfStreams     MaxPrimaryStreams
        //   1 -   3           1
        //   4 -   7           2
        //   8 -  15           3
        //  16 -  31           4
        //  32 -  63           5
        //  64 - 127           6
        // 128 - 255           7
        //
        if (NumberOfStreams < 4) {
            staticStreamsData->MaxPrimaryStreams = 1;
        }  else if (NumberOfStreams < 8) {
            staticStreamsData->MaxPrimaryStreams = 2;
        }  else if (NumberOfStreams < 16) {
            staticStreamsData->MaxPrimaryStreams = 3;
        }  else if (NumberOfStreams < 32) {
            staticStreamsData->MaxPrimaryStreams = 4;
        }  else if (NumberOfStreams < 64) {
            staticStreamsData->MaxPrimaryStreams = 5;
        }  else if (NumberOfStreams < 128) {
            staticStreamsData->MaxPrimaryStreams = 6;
        }  else {
            staticStreamsData->MaxPrimaryStreams = 7;
        }

        for (i = 0, streamId = 1; i < NumberOfStreams; i += 1, streamId += 1) {

            status = TR_Create(endpointData->ControllerHandle,
                               endpointData,
                               ucxStaticStreams,
                               streamId,
                               &staticStreamsData->StaticStreamData[i].TransferRingHandle);

            if (!NT_SUCCESS(status)) {

                LogError(endpointData->RecorderLog, Endpoint,
                         "%d.%d: TR_Create(%d) Failed %!STATUS!",
                         UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                         Endpoint_GetContextIndex(endpointData), i, status);
                LEAVE;
            }

            STREAM_INFO_INIT(&streamInfo,
                             TR_GetWdfQueue(staticStreamsData->StaticStreamData[i].TransferRingHandle),
                             streamId);

            UcxStaticStreamsSetStreamInfo(ucxStaticStreams, &streamInfo);
        }

        LogInfo(endpointData->RecorderLog, Endpoint,
                "%d.%d: Request to add %d streams success",
                UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(endpointData), NumberOfStreams);

        status = STATUS_SUCCESS;

    } FINALLY {

    }

    return status;
}

VOID
Endpoint_EvtStaticStreamsCleanupCallback(
    WDFOBJECT UcxStaticStreams
    )
/*++

Routine Description:

    This routine is invoked when the UcxStaticStreams object is deleted.

--*/
{
    PSTATIC_STREAMS_DATA    staticStreamsData;
    PENDPOINT_DATA          endpointData;
    HANDLE                  commonBufferHandle;

    staticStreamsData  = GetStaticStreamsData(UcxStaticStreams);
    endpointData       = staticStreamsData->EndpointData;
    commonBufferHandle = Controller_GetCommonBufferHandle(endpointData->ControllerHandle);

    LogInfo(endpointData->RecorderLog, Endpoint,
            "%d.%d: EndpointData 0x%p, StaticStreamsData 0x%p, UcxStaticStreams 0x%p",
            UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
            Endpoint_GetContextIndex(endpointData),
            endpointData, staticStreamsData, UcxStaticStreams);

    if (staticStreamsData->StreamContextArray != NULL) {
        CommonBuffer_ReleaseBuffer(commonBufferHandle, staticStreamsData->StreamContextArray);
        staticStreamsData->StreamContextArray = NULL;
    }

    return;
}

USBDEVICE_COMPLETION_ROUTINE Endpoint_EndpointStaticStreamsEnableCompletion;

VOID
Endpoint_UcxEvtEndpointStaticStreamsEnable(
    UCXENDPOINT UcxEndpoint,
    UCXSSTREAMS UcxStaticStreams,
    WDFREQUEST  WdfRequest
    )
/*++

Routine Description:

    This routine handles a URB_FUNCTION_OPEN_STATIC_STREAMS request
    to enable the non-default streams on an endpoint which were
    previously added by Endpoint_UcxEvtEndpointStaticStreamsAdd().

Return Value:

    None directly from this routine.

    The request completion status in the WdfRequest and the USBD_STATUS
    in the urb header.

--*/
{
    WDF_REQUEST_PARAMETERS              wdfRequestParams;
    PURB                                urb;
    PSTATIC_STREAMS_DATA                staticStreamsData;
    PENDPOINT_DATA                      endpointData;
    HANDLE                              commonBufferHandle;
    PENDPOINT_STATIC_STREAMS_CONFIGURE  endpointStaticStreamsConfigure;
    ULONG                               streamContextArraySize;
    PSTREAM_CONTEXT                     streamContext;
    ULONG                               i;
    NTSTATUS                            status;
    ULONG                               queueRequests;
    ULONG                               driverRequests;

    TRY {

        UNREFERENCED_PARAMETER(UcxEndpoint);

        //
        // Retreive the URB_FUNCTION_OPEN_STATIC_STREAMS request URB
        // from the WdfRequest.
        //
        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        urb                = (PURB)wdfRequestParams.Parameters.Others.Arg1;
        staticStreamsData  = GetStaticStreamsData(UcxStaticStreams);
        endpointData       = staticStreamsData->EndpointData;
        commonBufferHandle = Controller_GetCommonBufferHandle(endpointData->ControllerHandle);

        LogInfo(endpointData->RecorderLog, Endpoint,
                "%d.%d: Request to enable %d streams",
                UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(endpointData), staticStreamsData->NumberOfStreams);

        //
        // Disable the default queue. This is done by draining the queue, which prevents
        // new requests from being added to the queue. Then check to see if there are
        // pending requests on the queue. If there are, then fail the streams enable.
        //
        WdfIoQueueDrain(TR_GetWdfQueue(endpointData->TransferRingHandle), NULL, NULL);

        WdfIoQueueGetState(TR_GetWdfQueue(endpointData->TransferRingHandle),
                           &queueRequests,
                           &driverRequests);

        if ((queueRequests != 0) || (driverRequests != 0)) {

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: Requests pending on queue: queueRequests %d driverRequests %d",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData), queueRequests, driverRequests);

            NT_ASSERTMSG("Stream enable failed, requests pending on queue", 0);
            urb->UrbHeader.Status = USBD_STATUS_INVALID_PARAMETER;
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        if ((staticStreamsData->NumberOfStreams == 0) ||
            (staticStreamsData->NumberOfStreams > Register_GetSupportedNumberOfStreams(Controller_GetRegisterHandle(endpointData->ControllerHandle)))) {

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: Invalid Number Of Streams %d",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData), staticStreamsData->NumberOfStreams);

            urb->UrbHeader.Status = USBD_STATUS_INVALID_PARAMETER;
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        // Save the PSTATIC_STREAMS_DATA in our context for this
        // WdfRequest.
        //
        endpointStaticStreamsConfigure = &(GetRequestData(WdfRequest)->EndpointStaticStreamsConfigure);
        endpointStaticStreamsConfigure->StaticStreamsData = staticStreamsData;

        //
        // The Primary Stream Array contains 2**(MaxPStreams+1) entries.
        // Calculate that size.
        //
        streamContextArraySize = ((1ULL << (staticStreamsData->MaxPrimaryStreams + 1)) * sizeof(STREAM_CONTEXT));

        //
        // Allocate a Stream Context Array for the non-default stream
        // transfer rings.
        //
        staticStreamsData->StreamContextArray =
            CommonBuffer_AcquireBuffer(commonBufferHandle,
                                       streamContextArraySize,
                                       endpointData,
                                       ENDPOINT_ALLOC_TAG1);

        if (staticStreamsData->StreamContextArray == NULL) {

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: CommonBuffer_AcquireBuffer Failed",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData));

            urb->UrbHeader.Status = USBD_STATUS_INSUFFICIENT_RESOURCES;
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        //
        // Update the ENDPOINT_DATA with the PSTATIC_STREAMS_DATA so
        // that it is already in place for use during
        // UsbDevice_ReconfigureEndpoint().
        //
        endpointData->StaticStreamsData = staticStreamsData;

        for (i = 1; i <= staticStreamsData->NumberOfStreams; i += 1) {

            //
            // Enable the non-default stream transfer ring.
            //
            status = TR_Enable(Endpoint_GetStreamHandle(endpointData, i));

            if (!NT_SUCCESS(status)) {

                LogError(endpointData->RecorderLog, Endpoint,
                         "%d.%d: TR_Enable(%d) Failed %!STATUS!",
                         UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                         Endpoint_GetContextIndex(endpointData), i, status);

                urb->UrbHeader.Status = USBD_STATUS_INSUFFICIENT_RESOURCES;
                LEAVE;
            }

            //
            // Initialize the Stream Context Array entry for the
            // non-default stream to point to the transfer ring
            // for the non-default stream.
            //
            streamContext = Endpoint_GetStreamContext(endpointData, i);
            streamContext->DequeuePointer = Endpoint_GetDequeuePointer(endpointData, i);
        }

        //
        // Enable the non-default streams in the Endpoint Context /
        // Stream Context Array.
        //
        UsbDevice_ReconfigureEndpoint(endpointData->UsbDeviceHandle,
                                      endpointData,
                                      Endpoint_EndpointStaticStreamsEnableCompletion,
                                      WdfRequest);

        //
        // Defer final processing and WdfRequest completion until
        // Endpoint_EndpointStaticStreamsEnableCompletion() executes.
        //

        status = STATUS_SUCCESS;

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            //
            // Disable the non-default stream transfer rings.
            //
            for (i = 1; i <= staticStreamsData->NumberOfStreams; i += 1) {

                TR_Disable(staticStreamsData->StaticStreamData[i - 1].TransferRingHandle, TRUE);
            }

            //
            // Switch back to the default static streams data.
            //
            endpointData->StaticStreamsData = endpointData->DefaultStaticStreamsData;

            //
            // Deallocate the Stream Context Array for the non-default
            // stream transfer rings.
            //
            if (staticStreamsData->StreamContextArray != NULL) {
                CommonBuffer_ReleaseBuffer(commonBufferHandle, staticStreamsData->StreamContextArray);
                staticStreamsData->StreamContextArray = NULL;
            }

            //
            // Reenable the default queue for the endpoint.
            //
            WdfIoQueueStart(TR_GetWdfQueue(endpointData->TransferRingHandle));

            WdfRequestComplete(WdfRequest, status);
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_EndpointStaticStreamsEnableCompletion(
    __in
        PVOID       CompletionContext,
    __in
        NTSTATUS    CompletionStatus
    )
/*++

Routine Description:

    This is the completion routine passed to UsbDevice_ReconfigureEndpoint()
    by Endpoint_UcxEvtEndpointStaticStreamsEnable().

Return Value:

    None directly from this routine.

    The request completion status in the WdfRequest and the USBD_STATUS
    in the urb header.

--*/
{
    WDFREQUEST                          wdfRequest;
    WDF_REQUEST_PARAMETERS              wdfRequestParams;
    PURB                                urb;
    PENDPOINT_STATIC_STREAMS_CONFIGURE  endpointStaticStreamsConfigure;
    PSTATIC_STREAMS_DATA                staticStreamsData;
    PENDPOINT_DATA                      endpointData;
    HANDLE                              commonBufferHandle;
    ULONG                               i;

    TRY {

        wdfRequest = CompletionContext;

        //
        // Retreive the URB_FUNCTION_OPEN_STATIC_STREAMS request URB
        // from the WdfRequest.
        //
        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(wdfRequest, &wdfRequestParams);

        urb = (PURB)wdfRequestParams.Parameters.Others.Arg1;

        //
        // Retrieve the PSTATIC_STREAMS_DATA from our context for this
        // WdfRequest.
        //
        endpointStaticStreamsConfigure = &(GetRequestData(wdfRequest)->EndpointStaticStreamsConfigure);
        staticStreamsData              = endpointStaticStreamsConfigure->StaticStreamsData;
        endpointData                   = staticStreamsData->EndpointData;
        commonBufferHandle             = Controller_GetCommonBufferHandle(endpointData->ControllerHandle);

        if (NT_SUCCESS(CompletionStatus)) {

            //
            // Disable the default stream transfer ring.
            //
            TR_Disable(endpointData->TransferRingHandle, TRUE);

            //
            // Deallocate the default static streams data and point to the new one.
            //
            CommonBuffer_ReleaseBuffer(commonBufferHandle,
                                       endpointData->DefaultStaticStreamsData->StreamContextArray);

            ExFreePoolWithTag(endpointData->DefaultStaticStreamsData, DriverAllocationTag);

            endpointData->DefaultStaticStreamsData = NULL;

            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Request to enable %d streams success",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData), staticStreamsData->NumberOfStreams);

            urb->UrbHeader.Status = USBD_STATUS_SUCCESS;

        } else {

            //
            // Disable the non-default stream transfer rings.
            //
            for (i = 1; i <= staticStreamsData->NumberOfStreams; i += 1) {

                TR_Disable(staticStreamsData->StaticStreamData[i - 1].TransferRingHandle, TRUE);
            }

            //
            // Deallocate the Stream Context Array for the non-default
            // stream transfer rings.
            //
            if (staticStreamsData->StreamContextArray != NULL) {
                CommonBuffer_ReleaseBuffer(commonBufferHandle, staticStreamsData->StreamContextArray);
                staticStreamsData->StreamContextArray = NULL;
            }

            //
            // Switch back to the default static streams data.
            //
            endpointData->StaticStreamsData = endpointData->DefaultStaticStreamsData;

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: Request to disable %d streams Failed %!STATUS!",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData),
                     staticStreamsData->NumberOfStreams, CompletionStatus);

            urb->UrbHeader.Status = USBD_STATUS_INTERNAL_HC_ERROR;

            //
            // Reenable the default queue for the endpoint.
            //
            WdfIoQueueStart(TR_GetWdfQueue(endpointData->TransferRingHandle));
        }

    } FINALLY {

        if (NT_SUCCESS(CompletionStatus)) {
            ESM_AddEsmEvent(endpointData, EsmEventStreamsEnabled);
        }

        WdfRequestComplete(wdfRequest, CompletionStatus);
    }

    return;
}

USBDEVICE_COMPLETION_ROUTINE Endpoint_EndpointStaticStreamsDisableCompletion;

VOID
Endpoint_UcxEvtEndpointStaticStreamsDisable(
    UCXENDPOINT UcxEndpoint,
    UCXSSTREAMS UcxStaticStreams,
    WDFREQUEST  WdfRequest
    )
/*++

Routine Description:

    This routine handles a URB_FUNCTION_CLOSE_STATIC_STREAMS request
    to disnable the non-default streams on an endpoint which were
    previously enabled by Endpoint_UcxEvtEndpointStaticStreamsEnable().

Return Value:

    None directly from this routine.

    The request completion status in the WdfRequest and the USBD_STATUS
    in the urb header.

--*/
{
    WDF_REQUEST_PARAMETERS              wdfRequestParams;
    PURB                                urb;
    PSTATIC_STREAMS_DATA                staticStreamsData;
    PENDPOINT_DATA                      endpointData;
    HANDLE                              commonBufferHandle;
    ULONG                               streamContextArraySize;
    PSTREAM_CONTEXT                     streamContext;
    PENDPOINT_STATIC_STREAMS_CONFIGURE  endpointStaticStreamsConfigure;
    NTSTATUS                            status;

    UNREFERENCED_PARAMETER(UcxEndpoint);

    TRY {

        //
        // Retreive the URB_FUNCTION_OPEN_STATIC_STREAMS request URB
        // from the WdfRequest.
        //
        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(WdfRequest, &wdfRequestParams);

        urb                = (PURB)wdfRequestParams.Parameters.Others.Arg1;
        staticStreamsData  = GetStaticStreamsData(UcxStaticStreams);
        endpointData       = staticStreamsData->EndpointData;
        commonBufferHandle = Controller_GetCommonBufferHandle(endpointData->ControllerHandle);

        LogInfo(endpointData->RecorderLog, Endpoint,
                "%d.%d: Request to disable %d streams",
                UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(endpointData), staticStreamsData->NumberOfStreams);

        //
        // Save the PSTATIC_STREAMS_DATA in our context for this
        // WdfRequest.
        //
        endpointStaticStreamsConfigure = &(GetRequestData(WdfRequest)->EndpointStaticStreamsConfigure);
        endpointStaticStreamsConfigure->StaticStreamsData = staticStreamsData;

        endpointData->DefaultStaticStreamsData = ExAllocatePoolWithTag(NonPagedPool,
                                                                       sizeof(STATIC_STREAMS_DATA),
                                                                       DriverAllocationTag);

        if (endpointData->DefaultStaticStreamsData == NULL) {
            urb->UrbHeader.Status = USBD_STATUS_INSUFFICIENT_RESOURCES;
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        RtlZeroMemory(endpointData->DefaultStaticStreamsData, sizeof(STATIC_STREAMS_DATA));

        endpointData->DefaultStaticStreamsData->EndpointData      = endpointData;
        endpointData->DefaultStaticStreamsData->NumberOfStreams   = 1;
        endpointData->DefaultStaticStreamsData->MaxPrimaryStreams = 1;

        streamContextArraySize = (1ULL << (staticStreamsData->MaxPrimaryStreams + 1)) *
                                 sizeof(STREAM_CONTEXT);

        endpointData->DefaultStaticStreamsData->StreamContextArray =
            CommonBuffer_AcquireBuffer(commonBufferHandle,
                                       streamContextArraySize,
                                       endpointData,
                                       ENDPOINT_ALLOC_TAG1);

        if (endpointData->DefaultStaticStreamsData->StreamContextArray == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        //
        // Switch to the new default static streams data for use during
        // UsbDevice_ReconfigureEndpoint().
        //
        endpointData->StaticStreamsData = endpointData->DefaultStaticStreamsData;

        Endpoint_SetStreamHandle(endpointData, 1, endpointData->TransferRingHandle);

        //
        // Re-enable the default stream transfer ring.
        //
        status = TR_Enable(endpointData->TransferRingHandle);

        if (!NT_SUCCESS(status)) {

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: TR_Enable Failed %!STATUS!",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData), status);

            urb->UrbHeader.Status = USBD_STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        //
        // Initialize the default Stream Context Array entry to
        // point to the transfer ring for the default stream.
        //
        streamContext = Endpoint_GetStreamContext(endpointData, 1);
        streamContext->DequeuePointer = TR_GetDequeuePointer(endpointData->TransferRingHandle);

        //
        // Disable the non-default streams and enable the default stream with Id=1.
        //
        UsbDevice_ReconfigureEndpoint(endpointData->UsbDeviceHandle,
                                      endpointData,
                                      Endpoint_EndpointStaticStreamsDisableCompletion,
                                      WdfRequest);

        //
        // Defer final processing and WdfRequest completion until
        // Endpoint_EndpointStaticStreamsEnableCompletion() executes.
        //

        status = STATUS_SUCCESS;

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (endpointData->DefaultStaticStreamsData != NULL) {

                if (endpointData->DefaultStaticStreamsData->StreamContextArray != NULL) {

                    CommonBuffer_ReleaseBuffer(commonBufferHandle,
                                               endpointData->DefaultStaticStreamsData->StreamContextArray);
                }

                ExFreePoolWithTag(endpointData->DefaultStaticStreamsData, DriverAllocationTag);

                endpointData->DefaultStaticStreamsData = NULL;
            }

            WdfRequestComplete(WdfRequest, status);
        }
    }

    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_EndpointStaticStreamsDisableCompletion(
    __in
        PVOID       CompletionContext,
    __in
        NTSTATUS    CompletionStatus
    )
/*++

Routine Description:

    This is the completion routine passed to UsbDevice_ReconfigureEndpoint()
    by Endpoint_UcxEvtEndpointStaticStreamsDisable().

Return Value:

    None directly from this routine.

    The request completion status in the WdfRequest and the USBD_STATUS
    in the urb header.

--*/
{
    WDFREQUEST                          wdfRequest;
    WDF_REQUEST_PARAMETERS              wdfRequestParams;
    PURB                                urb;
    PENDPOINT_STATIC_STREAMS_CONFIGURE  endpointStaticStreamsConfigure;
    PSTATIC_STREAMS_DATA                staticStreamsData;
    PENDPOINT_DATA                      endpointData;
    HANDLE                              commonBufferHandle;
    ULONG                               i;

    TRY {

        wdfRequest = CompletionContext;

        //
        // Retreive the URB_FUNCTION_OPEN_STATIC_STREAMS request URB
        // from the WdfRequest.
        //
        WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
        WdfRequestGetParameters(wdfRequest, &wdfRequestParams);

        urb = (PURB)wdfRequestParams.Parameters.Others.Arg1;

        //
        // Retrieve the PSTATIC_STREAMS_DATA from our context for this
        // WdfRequest.
        //
        endpointStaticStreamsConfigure = &(GetRequestData(wdfRequest)->EndpointStaticStreamsConfigure);
        staticStreamsData              = endpointStaticStreamsConfigure->StaticStreamsData;
        endpointData                   = staticStreamsData->EndpointData;
        commonBufferHandle             = Controller_GetCommonBufferHandle(endpointData->ControllerHandle);

        if (NT_SUCCESS(CompletionStatus)) {

            //
            // Disable the non-default stream transfer rings.
            //
            for (i = 1; i <= staticStreamsData->NumberOfStreams; i += 1) {

                TR_Disable(staticStreamsData->StaticStreamData[i - 1].TransferRingHandle, TRUE);
            }

            //
            // Deallocate the Stream Context Array for the non-default
            // stream transfer rings.
            //
            if (staticStreamsData->StreamContextArray != NULL) {
                CommonBuffer_ReleaseBuffer(commonBufferHandle, staticStreamsData->StreamContextArray);
                staticStreamsData->StreamContextArray = NULL;
            }

            //
            // Reenable the default queue for the endpoint.
            //
            WdfIoQueueStart(TR_GetWdfQueue(endpointData->TransferRingHandle));

            LogInfo(endpointData->RecorderLog, Endpoint,
                    "%d.%d: Request to disable %d streams success",
                    UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                    Endpoint_GetContextIndex(endpointData), staticStreamsData->NumberOfStreams);

            urb->UrbHeader.Status = USBD_STATUS_SUCCESS;

        } else {

            //
            // Restore the PSTATIC_STREAMS_DATA in the ENDPOINT_DATA since the
            // non-default stream disable failed.
            //
            endpointData->StaticStreamsData = staticStreamsData;

            CommonBuffer_ReleaseBuffer(commonBufferHandle,
                                       endpointData->DefaultStaticStreamsData->StreamContextArray);

            ExFreePoolWithTag(endpointData->DefaultStaticStreamsData, DriverAllocationTag);

            endpointData->DefaultStaticStreamsData = NULL;

            LogError(endpointData->RecorderLog, Endpoint,
                     "%d.%d: Request to disable %d streams Failed %!STATUS!",
                     UsbDevice_GetSlotId(endpointData->UsbDeviceHandle),
                     Endpoint_GetContextIndex(endpointData),
                     staticStreamsData->NumberOfStreams, CompletionStatus);

            urb->UrbHeader.Status = USBD_STATUS_INTERNAL_HC_ERROR;
        }

    } FINALLY {

        if (NT_SUCCESS(CompletionStatus)) {
            ESM_AddEsmEvent(endpointData, EsmEventStreamsDisabled);
        }

        WdfRequestComplete(wdfRequest, CompletionStatus);
    }

    return;
}

//
// Endpoint State machine related helper routines.
//

VOID
Endpoint_WdfEvtStateMachineTimer(
    WDFTIMER WdfTimer
    )
{
    PENDPOINT_DATA  endpointData;
    UCXENDPOINT     ucxEndpoint;

    ucxEndpoint  = WdfTimerGetParentObject(WdfTimer);
    endpointData = GetEndpointData(ucxEndpoint);

    ESM_AddEsmEvent(endpointData, EsmEventTimerFired);
}

_Use_decl_annotations_
VOID
Endpoint_SM_CompleteResetRequestFromClient(
    PENDPOINT_DATA EndpointData
    )
{
    NTSTATUS    status;
    WDFREQUEST  wdfRequest;

    INTERLOCKED_CLEAR_BIT_FIELD(ENDPOINT_FLAGS,
                                &EndpointData->Flags,
                                OkToCompleteEndpointResetRequest);

    NT_ASSERT(EndpointData->EndpointResetCompletionStatus != STATUS_PENDING);

    status = EndpointData->EndpointResetCompletionStatus;
    EndpointData->EndpointResetCompletionStatus = STATUS_PENDING;

    wdfRequest = EndpointData->EndpointResetRequest;
    EndpointData->EndpointResetRequest = NULL;

    LogInfo(EndpointData->RecorderLog, Endpoint,
            "%d.%d: WdfRequest 0x%p Endpoint reset completed with %!STATUS!",
            UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
            Endpoint_GetContextIndex(EndpointData),
            wdfRequest, status);

    WdfRequestComplete(wdfRequest, status);
}

_Use_decl_annotations_
VOID
Endpoint_SM_OkToCompleteEndpointResetRequest(
    PENDPOINT_DATA EndpointData
    )
{
    ULONG completeRequest;

    INTERLOCKED_GET_AND_TOGGLE_BIT_FIELD(ENDPOINT_FLAGS,
                                         &EndpointData->Flags,
                                         OkToCompleteEndpointResetRequest,
                                         &completeRequest);

    if (completeRequest != 0) {
        Endpoint_SM_CompleteResetRequestFromClient(EndpointData);
    }
}

_Use_decl_annotations_
ESM_EVENT
Endpoint_SM_IsEndpointTypeControl(
    PENDPOINT_DATA EndpointData
    )
{
    if (Endpoint_GetType(EndpointData) == EndpointType_Control) {
        return EsmEventYes;
    } else {
        return EsmEventNo;
    }
}

_Use_decl_annotations_
VOID
Endpoint_SM_NotifyTransferRingsOkToReclaimTransfersOnCancel(
    PENDPOINT_DATA EndpointData
    )
{
    Endpoint_MUX_SendNotificationToTransferRings(EndpointData, TR_OkToReclaimTransfersOnCancel);
}

_Use_decl_annotations_
VOID
Endpoint_SM_NotifyTransferRingsReclaimTransfers(
    PENDPOINT_DATA EndpointData
    )
{
    Endpoint_MUX_SendNotificationToTransferRings(EndpointData, TR_ReclaimTransfers);
}

_Use_decl_annotations_
VOID
Endpoint_SM_NotifyTransferRingsEndpointHalted(
    PENDPOINT_DATA EndpointData
    )
{
    Endpoint_MUX_SendNotificationToTransferRings(EndpointData, TR_EndpointHalted);
}

_Use_decl_annotations_
VOID
Endpoint_SM_ReportFSE(
    PENDPOINT_DATA EndpointData
    )
{
    ULONG reportFSE;

    INTERLOCKED_GET_AND_TOGGLE_BIT_FIELD(ENDPOINT_FLAGS,
                                         &EndpointData->Flags,
                                         OkToReportFSE,
                                         &reportFSE);

    if (reportFSE) {
        ESM_AddEsmEvent(EndpointData, EsmEventFSEReceived);
    }
}

_Use_decl_annotations_
VOID
Endpoint_SM_DropFSE(
    PENDPOINT_DATA EndpointData
    )
{
    INTERLOCKED_SET_BIT_FIELD(ENDPOINT_FLAGS, &EndpointData->Flags, DropFSE);
}

_Use_decl_annotations_
VOID
Endpoint_SM_NotifyTransferRingsProcessExpectedEventTRBs(
    PENDPOINT_DATA EndpointData
    )
{
    Endpoint_MUX_SendNotificationToTransferRings(EndpointData, TR_ProcessExpectedEventTRBs);
}

_Use_decl_annotations_
VOID
Endpoint_SM_NotifyTransferRingsFSEReceived(
    PENDPOINT_DATA EndpointData
    )
{
    Endpoint_MUX_SendNotificationToTransferRings(EndpointData, TR_FSEReceived);
}

_Use_decl_annotations_
VOID
Endpoint_SM_NotifyTransferRingsEndpointResetFromClient(
    PENDPOINT_DATA EndpointData
    )
{
    Endpoint_MUX_SendNotificationToTransferRings(EndpointData, TR_EndpointResetFromClient);
}

_Use_decl_annotations_
VOID
Endpoint_SM_NotifyUCXCancelTransfers(
    PENDPOINT_DATA EndpointData
    )
{
    UcxEndpointNeedToCancelTransfers(Endpoint_GetUcxEndpoint(EndpointData));
}

_Use_decl_annotations_
VOID
Endpoint_SM_ResetControlEndpoint(
    PENDPOINT_DATA EndpointData
    )
{
    HANDLE  commandHandle;
    PCRB    crb;

    commandHandle = Controller_GetCommandHandle(EndpointData->ControllerHandle);

    crb = &EndpointData->Crb;

    RtlZeroMemory(crb, sizeof(CRB));

    crb->CommandCallback                                = Endpoint_ControlEndpointResetCompletion;
    crb->ObjectHandle                                   = (HANDLE)EndpointData;
    crb->Trb.ResetEndpointCommand.Type                  = TRB_TYPE_RESET_ENDPOINT_COMMAND;
    crb->Trb.ResetEndpointCommand.TransferStatePreserve = 0;
    crb->Trb.ResetEndpointCommand.EndpointId            = Endpoint_GetContextIndex(EndpointData);
    crb->Trb.ResetEndpointCommand.SlotId                = UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle);

    Command_SendCommand(commandHandle, crb);

    return;
}

_Use_decl_annotations_
VOID
Endpoint_SM_ResetEndpoint(
    PENDPOINT_DATA EndpointData
    )
{
    HANDLE                  commandHandle;
    PCRB                    crb;
    PENDPOINT_RESET_DATA    endpointResetData;
    WDF_REQUEST_PARAMETERS  wdfRequestParams;
    PENDPOINT_RESET         endpointReset;

    WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
    WdfRequestGetParameters(EndpointData->EndpointResetRequest, &wdfRequestParams);

    endpointReset     = (PENDPOINT_RESET)wdfRequestParams.Parameters.Others.Arg1;
    endpointResetData = &(GetRequestData(EndpointData->EndpointResetRequest)->EndpointResetData);
    commandHandle     = Controller_GetCommandHandle(EndpointData->ControllerHandle);
    crb               = &endpointResetData->Crb;

    RtlZeroMemory(crb, sizeof(CRB));

    crb->CommandCallback                     = Endpoint_OnResetEndpointResetCompletion;
    crb->ObjectHandle                        = (HANDLE)endpointResetData;
    crb->Trb.ResetEndpointCommand.Type       = TRB_TYPE_RESET_ENDPOINT_COMMAND;
    crb->Trb.ResetEndpointCommand.EndpointId = Endpoint_GetContextIndex(EndpointData);
    crb->Trb.ResetEndpointCommand.SlotId     = UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle);

    if (endpointReset->Flags & FlagEndpointResetPreserveTransferState) {
        crb->Trb.ResetEndpointCommand.TransferStatePreserve = 1;
    } else {
        crb->Trb.ResetEndpointCommand.TransferStatePreserve = 0;
    }

    Command_SendCommand(commandHandle, crb);

    return;
}

_Use_decl_annotations_
VOID
Endpoint_SM_SendStopEndpointCommand(
    PENDPOINT_DATA EndpointData
    )
{
    HANDLE commandHandle;
    PCRB   crb;

    commandHandle = Controller_GetCommandHandle(EndpointData->ControllerHandle);
    crb           = &EndpointData->Crb;

    RtlZeroMemory(crb, sizeof(CRB));

    crb->CommandCallback                    = Endpoint_OnCancelStopCompletion;
    crb->ObjectHandle                       = (HANDLE)EndpointData;
    crb->Trb.StopEndpointCommand.Type       = TRB_TYPE_STOP_ENDPOINT_COMMAND;
    crb->Trb.StopEndpointCommand.EndpointId = Endpoint_GetContextIndex(EndpointData);
    crb->Trb.StopEndpointCommand.SlotId     = UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle);

    LogInfo(EndpointData->RecorderLog, Endpoint, "%d.%d: OnCancelStop Begin",
            crb->Trb.StopEndpointCommand.SlotId,
            crb->Trb.StopEndpointCommand.EndpointId);

    Command_SendCommand(commandHandle, crb);

    return;
}

_Use_decl_annotations_
VOID
Endpoint_SM_StartMapping(
    PENDPOINT_DATA EndpointData
    )
{
    PSTATIC_STREAMS_DATA staticStreamsData;

    InterlockedExchange((volatile LONG*)&EndpointData->Flags.AsUlong32, 0);

    if (EndpointData->StreamsCapable) {

        staticStreamsData = EndpointData->StaticStreamsData;

        staticStreamsData->HaltedCompletionCode = TRB_COMPLETION_CODE_INVALID;
    }

    Endpoint_MUX_SendNotificationToTransferRings(EndpointData, TR_StartMapping);
}

_Use_decl_annotations_
VOID
Endpoint_SM_StopMapping(
    PENDPOINT_DATA EndpointData
    )
{
    Endpoint_MUX_SendNotificationToTransferRings(EndpointData, TR_StopMapping);
}

_Use_decl_annotations_
VOID
Endpoint_SM_Start2SecondTimer(
    PENDPOINT_DATA EndpointData
    )
{
    WdfTimerStart(EndpointData->SmContext.Timer, WDF_REL_TIMEOUT_IN_SEC(2));
}

_Use_decl_annotations_
VOID
Endpoint_SM_Start200MSTimer(
    PENDPOINT_DATA EndpointData
    )
{
    WdfTimerStart(EndpointData->SmContext.Timer, WDF_REL_TIMEOUT_IN_MS(200));
}

_Use_decl_annotations_
VOID
Endpoint_SM_Start500MSTimer(
    PENDPOINT_DATA EndpointData
    )
{
    WdfTimerStart(EndpointData->SmContext.Timer, WDF_REL_TIMEOUT_IN_MS(500));
}

_Use_decl_annotations_
VOID
Endpoint_SM_UpdateTransferRingDequeuePointers(
    PENDPOINT_DATA EndpointData
    )
{
    ULONG                   streamId;
    PSTATIC_STREAMS_DATA    staticStreamsData;

    if (EndpointData->StreamsCapable) {

        staticStreamsData = EndpointData->StaticStreamsData;
        staticStreamsData->UpdateCount = 0;

        for (streamId = 1; streamId <= EndpointData->StaticStreamsData->NumberOfStreams; streamId += 1) {
            Endpoint_OnCancelSetDequeuePointer(EndpointData, streamId);
        }

    } else {

        Endpoint_OnCancelSetDequeuePointer(EndpointData, 0);
    }
}

_Use_decl_annotations_
ESM_EVENT
Endpoint_SM_WasDoorbellRungSinceMappingStart(
    PENDPOINT_DATA EndpointData
    )
{
    BOOLEAN doorbellWasRung;

    doorbellWasRung =
        Endpoint_MUX_SendQueryToTransferRings(EndpointData, TR_WasDoorbellRungSinceMappingStart);

    if (doorbellWasRung) {
        return EsmEventYes;
    } else {
        return EsmEventNo;
    }
}

_Use_decl_annotations_
ESM_EVENT
Endpoint_SM_IsUseSingleInterrupterFlagSet(
    PENDPOINT_DATA EndpointData
    )
{
    if (Controller_GetDeviceFlags(EndpointData->ControllerHandle).UseSingleInterrupter) {
        return EsmEventYes;
    } else {
        return EsmEventNo;
    }
}

_Use_decl_annotations_
ESM_EVENT
Endpoint_SM_IsDelayInitialEndpointStopCommandFlagSet(
    PENDPOINT_DATA EndpointData
    )
{
    if (Controller_GetDeviceFlags(EndpointData->ControllerHandle).DelayInitialEndpointStopCommand) {
        return EsmEventYes;
    } else {
        return EsmEventNo;
    }
}

_Use_decl_annotations_
ESM_EVENT
Endpoint_SM_IsIgnoreContextErrorOnEPStopFlagSet(
    PENDPOINT_DATA EndpointData
    )
{
    if (Controller_GetDeviceFlags(EndpointData->ControllerHandle).IgnoreContextErrorOnEPStop) {
        NT_ASSERT(Controller_GetDeviceFlags(EndpointData->ControllerHandle).UseSingleInterrupter);
        return EsmEventYes;
    } else {
        return EsmEventNo;
    }
}

_Use_decl_annotations_
VOID
Endpoint_SM_ResetStopEndpointReissueCount(
    PENDPOINT_DATA EndpointData
    )
{
    EndpointData->StopReissueCount = 0;
}

_Use_decl_annotations_
VOID
Endpoint_SM_IncrementStopEndpointReissueCount(
    PENDPOINT_DATA EndpointData
    )
{
    EndpointData->StopReissueCount += 1;
}

_Use_decl_annotations_
ESM_EVENT
Endpoint_SM_IsStopEndpointReissueCountLimitReached(
    PENDPOINT_DATA EndpointData
    )
{
    if (EndpointData->StopReissueCount < ENDPOINT_STOP_REISSUE_MAX_COUNT) {
        return EsmEventNo;
    } else {
        return EsmEventYes;
    }
}

_Use_decl_annotations_
VOID
Endpoint_SM_PurgeTransferRingQueuesIfNeeded(
    PENDPOINT_DATA EndpointData
    )
{
    ULONG                   i;
    PSTATIC_STREAMS_DATA    staticStreamsData;

    if (EndpointData->Flags.PurgeTransferRingQueuesAfterHalt) {

        NT_ASSERT(EndpointData->StreamsCapable);

        staticStreamsData = EndpointData->StaticStreamsData;

        for (i = 1; i <= staticStreamsData->NumberOfStreams; i += 1) {

            WdfIoQueuePurge(
                TR_GetWdfQueue(Endpoint_GetStreamHandle(EndpointData, i)),
                NULL,
                NULL);
        }
    }
}

_Use_decl_annotations_
VOID
Endpoint_SM_RestartTransferRingQueuesIfNeeded(
    PENDPOINT_DATA EndpointData
    )
{
    ULONG                   i;
    PSTATIC_STREAMS_DATA    staticStreamsData;

    if (EndpointData->Flags.PurgeTransferRingQueuesAfterHalt) {

        NT_ASSERT(EndpointData->StreamsCapable);

        staticStreamsData = EndpointData->StaticStreamsData;

        for (i = 1; i <= staticStreamsData->NumberOfStreams; i += 1) {

            WdfIoQueueStart(TR_GetWdfQueue(Endpoint_GetStreamHandle(EndpointData, i)));
        }
    }
}

_Use_decl_annotations_
VOID
Endpoint_SM_ReconfigureEndpoint(
    PENDPOINT_DATA EndpointData
    )
{
    Endpoint_OnResetEndpointConfigure(EndpointData);
}

_Use_decl_annotations_
VOID
Endpoint_SM_ReconfigureEndpointAfterStop(
    PENDPOINT_DATA EndpointData
    )
{
    Endpoint_OnCancelEndpointConfigure(EndpointData);
}

_Use_decl_annotations_
ESM_EVENT
Endpoint_SM_ShouldReconfigureEndpointAfterStop(
    PENDPOINT_DATA EndpointData
    )
{
    if (EndpointData->StreamsCapable &&
        Controller_GetDeviceFlags(EndpointData->ControllerHandle).OnStopConfigureStreamEndpoint) {

        return EsmEventYes;

    } else {

        return EsmEventNo;
    }
}

_Use_decl_annotations_
ESM_EVENT
Endpoint_SM_ShouldReconfigureEndpointOnResetWhenNotHalted(
    PENDPOINT_DATA EndpointData
    )
{
    WDF_REQUEST_PARAMETERS  wdfRequestParams;
    PENDPOINT_RESET         endpointReset;
    WDFREQUEST              wdfRequest;
    BOOLEAN                 reconfigureEndpoint;

    reconfigureEndpoint = FALSE;
    wdfRequest = EndpointData->EndpointResetRequest;

    WDF_REQUEST_PARAMETERS_INIT(&wdfRequestParams);
    WdfRequestGetParameters(wdfRequest, &wdfRequestParams);

    endpointReset = (PENDPOINT_RESET)wdfRequestParams.Parameters.Others.Arg1;

    LogInfo(EndpointData->RecorderLog, Endpoint,
            "%d.%d: UcxEndpoint 0x%p WdfRequest 0x%p Flags 0x%x",
            UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
            Endpoint_GetContextIndex(EndpointData),
            EndpointData->UcxEndpoint, wdfRequest, (ULONG)endpointReset->Flags);

    if ((endpointReset->Flags & FlagEndpointResetPreserveTransferState) ||
        ((Endpoint_GetTransferType(EndpointData) == USB_ENDPOINT_TYPE_CONTROL) ||
         (Endpoint_GetTransferType(EndpointData) == USB_ENDPOINT_TYPE_ISOCHRONOUS))) {

        //
        // If the transfer state is preserved or this is a control/isoch endpoint,
        // then there is nothing to do. The control/isoch endpoint handling is
        // consistent with the USB2 stack.
        //
        EndpointData->EndpointResetCompletionStatus = STATUS_SUCCESS;

    } else if (Endpoint_AreTransfersPending(EndpointData) == FALSE) {

        //
        // The endpoint is not halted. In this case, a reset endpoint cannot be submitted
        // since that is only allowed when the endpoint is halted. Unfortunately, the only
        // other way to reset the data toggle/sequence number is to issue an endpoints
        // configure command for this endpoint and before that is done, the endpoint
        // must be idle.
        //
        reconfigureEndpoint = TRUE;

    } else if (EndpointData->StreamsCapable) {

        //
        // Reset request for stream endpoint with outstanding transfers will be failed.
        //
        EndpointData->EndpointResetCompletionStatus = STATUS_UNSUCCESSFUL;

    } else {

        //
        // For the 2.0 stack, it is a written rule that no transfers should be pending
        // when the endpoint is reset. This was not enforced. The 2.0 stack always
        // resets the data toggle on the host/device, regardless if there are transfers
        // pending. For the xHCI stack, complete the reset pipe successfully with no action.
        //
        // Note: in the future, if reset pipe needs to be supported with pending transfers
        // in the non-halted endpoint case, then the xHCI driver would need to stop the
        // endpoint and requeue all pending requests before issuing an endpoint configure.
        //
        LogInfo(EndpointData->RecorderLog, Endpoint,
                "%d.%d: Transfer Ring 0x%p is not empty",
                UsbDevice_GetSlotId(EndpointData->UsbDeviceHandle),
                Endpoint_GetContextIndex(EndpointData),
                EndpointData->TransferRingHandle);

        EndpointData->EndpointResetCompletionStatus = STATUS_SUCCESS;
    }

    if (reconfigureEndpoint) {
        return EsmEventYes;
    } else {
        return EsmEventNo;
    }
}

_Use_decl_annotations_
ESM_EVENT
Endpoint_SM_ShouldResetEndpointAfterHalt(
    PENDPOINT_DATA EndpointData
    )
{
    if (EndpointData->StreamsCapable &&
        (Endpoint_AreTransfersPending(EndpointData) != FALSE)) {

        //
        // Reset request for stream endpoint with outstanding transfers will be failed.
        //
        EndpointData->EndpointResetCompletionStatus = STATUS_UNSUCCESSFUL;

        return EsmEventNo;

    } else {

        return EsmEventYes;
    }
}

_Use_decl_annotations_
VOID
Endpoint_SM_RequestControllerResetDueToRepeatedStopEndpointFailure(
    PENDPOINT_DATA EndpointData
    )
{

    LogFatal(EndpointData->RecorderLog, Endpoint,
            "HW_COMPLIANCE: UcxEndpoint 0x%p: Stop Endpoint command repeatedly failed with context state error",
            EndpointData->UcxEndpoint);

    Controller_HwVerifierBreakIfEnabled(
        EndpointData->ControllerHandle,
        EndpointData->UcxUsbDevice,
        EndpointData->UcxEndpoint,
        ControllerHwVerifierEndpointStopFailedContextStateError,
        "Stop Endpoint command repeatedly failed with context state error",
        NULL,
        NULL);
    
    Controller_ReportFatalError(EndpointData->ControllerHandle,
                                Controller_FatalErrorReset,
                                USB3_WER_BUGCODE_USBXHCI_CONTEXT_ERROR_AFTER_STOP,
                                EndpointData->UsbDeviceHandle,
                                EndpointData,
                                NULL);
}

_Use_decl_annotations_
ESM_EVENT
Endpoint_SM_AssertAndSimulateFSEReceived(
    PENDPOINT_DATA EndpointData
    )
{
    UNREFERENCED_PARAMETER(EndpointData);

    LogError(EndpointData->RecorderLog, Endpoint,
            "HW_COMPLIANCE: UcxEndpoint 0x%p: Force Stopped Event not received within 2 seconds after endpoint stop completion",
            EndpointData->UcxEndpoint);
    NT_ASSERTMSG("Force Stopped Event not received within 2 seconds after endpoint stop completion.", 0);

    return EsmEventFSEReceived;
}

_Use_decl_annotations_
ESM_EVENT
Endpoint_SM_AssertAndSimulateExpectedEventTRBsProcessedAfterStop(
    PENDPOINT_DATA EndpointData
    )
{
    UNREFERENCED_PARAMETER(EndpointData);

    LogError(EndpointData->RecorderLog, Endpoint,
            "HW_COMPLIANCE: UcxEndpoint 0x%p: Expected Event TRBs not processed within 2 seconds after endpoint stop completion",
            EndpointData->UcxEndpoint);


    NT_ASSERTMSG("Expected Event TRBs not processed within 2 seconds after endpoint stop completion.", 0);

    return EsmEventExpectedEventTRBsProcessed;
}

_Use_decl_annotations_
ESM_EVENT
Endpoint_SM_AssertAndSimulateExpectedEventTRBsProcessedAfterHalt(
    PENDPOINT_DATA EndpointData
    )
{
    UNREFERENCED_PARAMETER(EndpointData);
    
    LogError(EndpointData->RecorderLog, Endpoint,
            "HW_COMPLIANCE: UcxEndpoint 0x%p: Expected Event TRBs not processed within 2 seconds after endpoint halt",
            EndpointData->UcxEndpoint);

    NT_ASSERTMSG("Expected Event TRBs not processed within 2 seconds after endpoint halt.", 0);

    return EsmEventExpectedEventTRBsProcessed;
}

_Use_decl_annotations_
ESM_EVENT
Endpoint_SM_AssertAndSimulateEndpointHaltReceived(
    PENDPOINT_DATA EndpointData
    )
{
    UNREFERENCED_PARAMETER(EndpointData);
    
    LogError(EndpointData->RecorderLog, Endpoint,
            "HW_COMPLIANCE: UcxEndpoint 0x%p: Transfer Event TRB with halted completion code was not received within 2 seconds after endpoint halt",
            EndpointData->UcxEndpoint);

    NT_ASSERTMSG("Transfer Event TRB with halted completion code not received within 2 seconds after endpoint halt.", 0);

    return EsmEventHaltedCompletionCodeReceived;
}

_Use_decl_annotations_
VOID
Endpoint_SM_AcknowledgeControllerResetComplete(
    PENDPOINT_DATA EndpointData
    )
{
    ULONG setEvent;

    INTERLOCKED_GET_AND_TOGGLE_BIT_FIELD(ENDPOINT_FLAGS,
                                         &EndpointData->Flags,
                                         ControllerResetCompleteAcknowledged,
                                         &setEvent);

    if (setEvent) {
        KeSetEvent(&EndpointData->ControllerResetCompleteAcknowledgeEvent, IO_NO_INCREMENT, FALSE);
    }
}
