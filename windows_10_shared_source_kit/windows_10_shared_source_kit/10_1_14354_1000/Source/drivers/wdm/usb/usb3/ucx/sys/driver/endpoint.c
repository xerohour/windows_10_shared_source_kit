/*++
Copyright (c) Microsoft Corporation

Module Name:

    endpoint.c

Abstract:

    USB Controller Extension Driver

--*/
#include "pch.h"

#include "endpoint.tmh"

#define KB (1024)
#define MB (1024 * KB)

//                                                 EndpointTransferType...,
//                                           {Unknown,     Ctrl,     Bulk,     Rsvd,  Intr};
ULONG g_LowSpeed_MaximumTransferSizeTable[]   =  { 0,   4 * KB,   4 * MB,        0,  4 * MB};
ULONG g_FullSpeed_MaximumTransferSizeTable[]  =  { 0,   4 * KB,   4 * MB,        0,  4 * MB};
ULONG g_HighSpeed_MaximumTransferSizeTable[]  =  { 0,  64 * KB,   4 * MB,        0,  4 * MB};
ULONG g_SuperSpeed_MaximumTransferSizeTable[] =  { 0,  64 * KB,  32 * MB,        0,  4 * MB};


PULONG g_EndpointMaximumTransferSizeTable[] =
{
    g_LowSpeed_MaximumTransferSizeTable,
    g_FullSpeed_MaximumTransferSizeTable,
    g_HighSpeed_MaximumTransferSizeTable,
    g_SuperSpeed_MaximumTransferSizeTable
};

__drv_requiresIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
DefaultEndpoint_Create(
    __in
        UCXUSBDEVICE       Hub,
    __in
        UCXUSBDEVICE       UsbDevice,
    __in
        ULONG              MaxPacketSize,
    __out
        UCXENDPOINT       *DefaultEndpoint
    )
/*++

Routine Description:

    This Query interface function is called by the hub driver to create
    a default endpoint object.

Arguments:

    Hub - The handle to the UCXUSBDEVICE representing the hub that called this
        function

    UsbDevice - The handle to the UCXUSBDEVICE for which the default endpoint is
        being created.

    MaxPacketSize - The MaxPacket size of the default endpoint.

    DefaultEndpoint - Output - A pointer to a location that recieves a handle to the
        created default endpoint

Return Value:

    NT_SUCCESS status if a default object is successfully created

--*/
{
    NTSTATUS                      status;
    PUCXUSBDEVICE_PRIVCONTEXT     usbDeviceContext;
    PUCXENDPOINT_PRIVCONTEXT      ucxEndpointContext;
    UCXCONTROLLER                 ucxController;
    UCXENDPOINT_INIT              ucxEndpointInit;

    UNREFERENCED_PARAMETER(Hub);

    TRY {

        usbDeviceContext = UcxUsbDeviceGetContext(UsbDevice);

        ucxController = usbDeviceContext->UcxControllerContext->UcxController;

        RtlZeroMemory(&ucxEndpointInit, sizeof(ucxEndpointInit));
        ucxEndpointInit.Sig = SigUcxEndpointInit;
        ucxEndpointInit.UcxUsbDevice = UsbDevice;
        ucxEndpointInit.EndpointTransferType = EndpointTransferTypeControl;

        ucxEndpointInit.EndpointType = UcxEndpointTypeDefault;

        //
        // Create a state machine for non-root hub endpoints
        //
        ucxEndpointInit.CreateStateMachine = TRUE;

        status = usbDeviceContext->EvtCallbacks.EvtUsbDeviceDefaultEndpointAdd(ucxController,
                                                                               UsbDevice,
                                                                               MaxPacketSize,
                                                                               &ucxEndpointInit);
        if (!NT_SUCCESS(status)) {
            LogError(FlagHubInterface, "EvtUsbDeviceDefaultEndpointAdd Failed %!STATUS!", status);
            LEAVE;
        }

        ucxEndpointContext = UcxEndpointGetContext(ucxEndpointInit.UcxEndpoint);
        if (ucxEndpointContext->UcxPipeInfo.Queue == NULL) {
            LogError(FlagEndpoint, "EvtUsbDeviceDefaultEndpointAdd forgot to set the Queue for EndpointContext %p",
                     ucxEndpointContext);
            status = STATUS_INTERNAL_ERROR;
            Controller_VerifierBreak(ucxEndpointContext->UcxControllerContext,
                                     "HCD Forgot to set the WDF Queue on the endpoint");
            LEAVE;
        }

    } FINALLY {

        if (NT_SUCCESS(status)) {

            *DefaultEndpoint = ucxEndpointInit.UcxEndpoint;

            Etw_UcxEndpointCreate(NULL, UcxEndpointGetContext(ucxEndpointInit.UcxEndpoint));

        } else {

            *DefaultEndpoint = NULL;

            if (ucxEndpointInit.UcxEndpoint != NULL) {
                Endpoint_Delete(Hub, UsbDevice, ucxEndpointInit.UcxEndpoint);
            }

        }

    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
Endpoint_Create(
    __in
        UCXUSBDEVICE                                  Hub,
    __in
        UCXUSBDEVICE                                  UsbDevice,
    __in
        PUSB_ENDPOINT_DESCRIPTOR                      EndpointDescriptor,
    __in
        ULONG                                         EndpointDescriptorBufferLength,
    __in_opt
        PUSB_SUPERSPEED_ENDPOINT_COMPANION_DESCRIPTOR SuperSpeedEndpointCompanionDescriptor,
    __out
        UCXENDPOINT                                  *Endpoint
    )
/*++

Routine Description:

    This Query interface function is called by the hub driver to create
    a endpoint object.

Arguments:

    Hub - The handle to the UCXUSBDEVICE representing the hub that called this
        function

    UsbDevice - The handle to the UCXUSBDEVICE for which the endpoint is being
        created.

    EndpointDescriptor - The endpoint descriptor pointer.

    Endpoint - Output - A pointer to a location that recieves a handle to the
        created default endpoint

Return Value:

    NT_SUCCESS status if a default object is successfully created

--*/
{
    NTSTATUS                                                 status;
    PUCXUSBDEVICE_PRIVCONTEXT                                usbDeviceContext;
    PUCXENDPOINT_PRIVCONTEXT                                 ucxEndpointContext;
    UCXCONTROLLER                                            ucxController;
    UCXENDPOINT_INIT                                         ucxEndpointInit;
    PUSB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR  superSpeedPlusIsochEndpointCompanionDescriptor;

    UNREFERENCED_PARAMETER(Hub);

    TRY {

        usbDeviceContext = UcxUsbDeviceGetContext(UsbDevice);

        ucxController = usbDeviceContext->UcxControllerContext->UcxController;

        RtlZeroMemory(&ucxEndpointInit, sizeof(ucxEndpointInit));
        ucxEndpointInit.Sig = SigUcxEndpointInit;
        ucxEndpointInit.UcxUsbDevice = UsbDevice;
        ucxEndpointInit.EndpointTransferType
            = EndpointGetTransferTypeFromDescriptor(EndpointDescriptor);

        if (EndpointDescriptor->wMaxPacketSize == 0) {
            ucxEndpointInit.EndpointType = UcxEndpointTypeZeroBw;
        }

        //
        // Create a state machine for non-root hub endpoints
        //
        ucxEndpointInit.CreateStateMachine = TRUE;

        RtlCopyMemory(&ucxEndpointInit.EndpointDescriptor,
                      EndpointDescriptor,
                      sizeof(ucxEndpointInit.EndpointDescriptor));
        if (SuperSpeedEndpointCompanionDescriptor != NULL) {
            RtlCopyMemory(&ucxEndpointInit.SuperSpeedEndpointCompanionDescriptor,
                          SuperSpeedEndpointCompanionDescriptor,
                          sizeof(ucxEndpointInit.SuperSpeedEndpointCompanionDescriptor));

            if (((EndpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK) == USB_ENDPOINT_TYPE_ISOCHRONOUS) &&
                (SuperSpeedEndpointCompanionDescriptor->bmAttributes.Isochronous.SspCompanion != 0)) {

                superSpeedPlusIsochEndpointCompanionDescriptor =
                    (PUSB_SUPERSPEEDPLUS_ISOCH_ENDPOINT_COMPANION_DESCRIPTOR)
                    ((PCHAR)SuperSpeedEndpointCompanionDescriptor + SuperSpeedEndpointCompanionDescriptor->bLength);

                RtlCopyMemory(&ucxEndpointInit.SuperSpeedPlusIsochEndpointCompanionDescriptor,
                    superSpeedPlusIsochEndpointCompanionDescriptor,
                    sizeof(ucxEndpointInit.SuperSpeedPlusIsochEndpointCompanionDescriptor));
            }
        }

        status = usbDeviceContext->EvtCallbacks.EvtUsbDeviceEndpointAdd(
            ucxController,
            UsbDevice,
            EndpointDescriptor,
            EndpointDescriptorBufferLength,
            SuperSpeedEndpointCompanionDescriptor,
            &ucxEndpointInit);

        if (!NT_SUCCESS(status)) {
            LogError(FlagHubInterface, "EvtUsbDeviceEndpointAdd Failed %!STATUS!", status);
            LEAVE;
        }

        ucxEndpointContext = UcxEndpointGetContext(ucxEndpointInit.UcxEndpoint);
        if (ucxEndpointContext->UcxPipeInfo.Queue == NULL) {
            LogError(FlagEndpoint, "EvtUsbDeviceDefaultEndpointAdd forgot to set the Queue for EndpointContext %p",
                     ucxEndpointContext);
            status = STATUS_INTERNAL_ERROR;
            Controller_VerifierBreak(ucxEndpointContext->UcxControllerContext,
                                     "HCD Forgot to set the WDF Queue on the endpoint");
            LEAVE;
        }


    } FINALLY {

        if (NT_SUCCESS(status)) {

            *Endpoint = ucxEndpointInit.UcxEndpoint;

            Etw_UcxEndpointCreate(NULL, UcxEndpointGetContext(ucxEndpointInit.UcxEndpoint));

        } else {

            *Endpoint = NULL;

            if (ucxEndpointInit.UcxEndpoint != NULL) {
                Endpoint_Delete(Hub, UsbDevice, ucxEndpointInit.UcxEndpoint);
            }

        }

    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Endpoint_Delete(
    __in
        UCXUSBDEVICE       Hub,
    __in
        UCXUSBDEVICE       UsbDevice,
    __in
        UCXENDPOINT        Endpoint
)
/*++

Routine Description:

    This Query interface function is called by the hub driver to delete
    an endpoint object.

Arguments:

    Hub - The handle to the UCXUSBDEVICE representing the hub that called this
        function

    UsbDevice - A handle to the UsbDevice whose endpoint is being deleted.

    Endpoint - The handle to the endpoint / default endpoint that is being
        deleted.

Comments:

    The endpoint object is reference counted. Thus calling delete does not
    necessarily mean that the endpoint object memory will be freed. The memory
    will be freed, when the refrence count goes to zero.


--*/
{
    UNREFERENCED_PARAMETER(Hub);
    UNREFERENCED_PARAMETER(UsbDevice);

    TRY {

        Endpoint_EventAdd(UcxEndpointGetContext(Endpoint), UcxEndpointEventDelete);

    } FINALLY {

    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_ExposedToClient(
    __in
        UCXENDPOINT        Endpoint
)
/*++

Routine Description:

    This Query interface function is called by the hub driver to inform UCX that the
    pipe handle corresponding to endpoint has been Exposed to the client.

Arguments:

    Hub - The handle to the UCXUSBDEVICE representing the hub that called this
        function

    UsbDevice - A handle to the UsbDevice whose endpoint is being deleted.

    Endpoint - The handle to the endpoint that was Exposed to the client.

--*/
{
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext;

    TRY {

        ucxEndpointContext = UcxEndpointGetContext(Endpoint);
        ucxEndpointContext->ExposedToClient = TRUE;

    } FINALLY {

    }
}

USBD_PIPE_HANDLE
Endpoint_GetUsbdPipeHandle(
    __in
        UCXENDPOINT        Endpoint
    )
/*++

Routine Description:

    This Query interface function is called by the hub driver to get the
    usbd pipe handle associated with the UCXENDPOINT.

Arguments:

    Endpoint - The handle to the endpoint for which the USBD_PIPE_HANDLE is requested.

--*/
{
    PUCXENDPOINT_PRIVCONTEXT ucxEndpointContext;
    USBD_PIPE_HANDLE         usbdPipeHandle;

    TRY {

        ucxEndpointContext = UcxEndpointGetContext(Endpoint);
        usbdPipeHandle = &ucxEndpointContext->UcxPipeInfo;

    } FINALLY {

    }

    return usbdPipeHandle;
}

ULONG
Endpoint_CalculateMaximumTransferSize(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT DeviceContext,
    __in
        PUCXENDPOINT_INIT         EndpointInit
    )

/*++

Routine Description:

    This function will calculate the maximum transfer size for a given endpoint

Arguments:


--*/
{
    ULONG                       maxTransferSize;
    ULONG                       maxPacketSize;
    USB_HIGH_SPEED_MAXPACKET    muxPacket;

    TRY {

        if (EndpointInit->EndpointTransferType == EndpointTransferTypeIsochronous) {

            if (DeviceContext->UsbVersionNumber < 0x250) {
                muxPacket.us = EndpointInit->EndpointDescriptor.wMaxPacketSize;
                maxPacketSize = muxPacket.MaxPacket * (muxPacket.HSmux + 1);
            } else {
                maxPacketSize = EndpointInit->EndpointDescriptor.wMaxPacketSize;
            }
            switch (DeviceContext->UcxUsbDeviceInfo.DeviceSpeed) {
            case UsbFullSpeed:
                maxTransferSize = 256 * maxPacketSize;
                break;
            case UsbHighSpeed:
                maxTransferSize = 1024 * maxPacketSize;
                break;
            case UsbSuperSpeed:
                maxTransferSize = 1024 *
                    ((EndpointInit->SuperSpeedEndpointCompanionDescriptor.bmAttributes.Isochronous.SspCompanion) ?
                     EndpointInit->SuperSpeedPlusIsochEndpointCompanionDescriptor.dwBytesPerInterval :
                     EndpointInit->SuperSpeedEndpointCompanionDescriptor.wBytesPerInterval);
                break;
            default:
                maxTransferSize = 256 * maxPacketSize;
                break;
            }

        } else {

            maxTransferSize = g_EndpointMaximumTransferSizeTable[DeviceContext->UcxUsbDeviceInfo.DeviceSpeed][EndpointInit->EndpointTransferType];

            if ((EndpointInit->EndpointTransferType == EndpointTransferTypeControl) &&
                ((DeviceContext->UcxUsbDeviceInfo.DeviceSpeed == UsbLowSpeed) ||
                 (DeviceContext->UcxUsbDeviceInfo.DeviceSpeed == UsbFullSpeed))) {

                if (Registry_QueryAllow64KLowOrFullSpeedControlTransfersFlag()) {
                    
                    maxTransferSize = 64 * KB;
                }
            }
        }

    } FINALLY {

    }

    return maxTransferSize;
}

ULONG
Endpoint_GetMaximumTransferSize(
    __in
        UCXENDPOINT         Endpoint
    )
/*++

Routine Description:

    This Query interface function is called by the hub driver to get the
    Maximum Transfer Size for the endpoint associated with the UCXENDPOINT.

Arguments:

    Endpoint - The handle to the endpoint for which the Maximum TransferSize is requested.

--*/
{
    PUCXENDPOINT_PRIVCONTEXT ucxEndpointContext;
    ULONG                    maxTransferSize;

    TRY {

        ucxEndpointContext = UcxEndpointGetContext(Endpoint);
        maxTransferSize = ucxEndpointContext->UcxPipeInfo.MaximumTransferSize;

    } FINALLY {

    }

    return maxTransferSize;
}

VOID
EXPORT(UcxDefaultEndpointInitSetEventCallbacks) (
    __in_opt
        PUCX_GLOBALS                            Globals,
    __inout
        PUCXENDPOINT_INIT                       EndpointInit,
    __in
        PUCX_DEFAULT_ENDPOINT_EVENT_CALLBACKS   EventCallbacks
)
/*++

Routine Description:

    This export function sets Default Endpoint Event Callbacks in an EndpointInit structure

Arguments:

    Globals - Driver globals of the controller driver. This information is not
        being used.

    EndpointInit - A pointer to the UCXENDPOINT_INIT structure.

    EventCallbacks - A pointer to a structure that has all the default endpoint event
        callbacks.

Return Value:

    VOID
--*/
{
    UNREFERENCED_PARAMETER(Globals);

    TRY {

        //
        // Validate the size of the structure. For any future minor versions,
        // this check should include checks for all previous sizes.
        //
        if (EventCallbacks->Size != sizeof(UCX_DEFAULT_ENDPOINT_EVENT_CALLBACKS)) {
            NT_ASSERTMSG("Invalid size", FALSE);
        }
        if (EndpointInit->EndpointType != UcxEndpointTypeDefault) {
            NT_ASSERTMSG("Invalid Endpoint Type", FALSE);
        }

        EndpointInit->EvtCallbacks.EvtDefaultEndpointUpdate =
            EventCallbacks->EvtDefaultEndpointUpdate;
        EndpointInit->EvtCallbacks.EvtEndpointPurge = EventCallbacks->EvtEndpointPurge;
        EndpointInit->EvtCallbacks.EvtEndpointStart = EventCallbacks->EvtEndpointStart;
        EndpointInit->EvtCallbacks.EvtEndpointAbort = EventCallbacks->EvtEndpointAbort;
        EndpointInit->EvtCallbacks.EvtEndpointOkToCancelTransfers =
            EventCallbacks->EvtEndpointOkToCancelTransfers;

        EndpointInit->EvtCallbacks.EvtEndpointEnableForwardProgress
            = EventCallbacks->EvtEndpointEnableForwardProgress;

    } FINALLY {

    }
}

VOID
EXPORT(UcxEndpointInitSetEventCallbacks) (
    __in_opt
        PUCX_GLOBALS                  Globals,
    __inout
        PUCXENDPOINT_INIT             EndpointInit,
    __in
        PUCX_ENDPOINT_EVENT_CALLBACKS EventCallbacks
)
/*++

Routine Description:

    This export function sets Endpoint Event Callbacks in an EndpointInit structure

Arguments:

    Globals - Driver globals of the controller driver. This information is not
        being used.

    EndpointInit - A pointer to the UCXENDPOINT_INIT structure.

    EventCallbacks - A pointer to a structure that has all the endpoint event
        callbacks.

Return Value:

    VOID
--*/
{
    UNREFERENCED_PARAMETER(Globals);

    TRY {

        //
        // Validate the size of the structure. For any future minor versions,
        // this check should include checks for all previous sizes.
        //
        if (EventCallbacks->Size != sizeof(UCX_ENDPOINT_EVENT_CALLBACKS)) {
            NT_ASSERTMSG("Invalid size", FALSE);
        }

        if (EndpointInit->EndpointType == UcxEndpointTypeDefault) {
            NT_ASSERTMSG("Invalid Endpoint Type", FALSE);
        }

        EndpointInit->EvtCallbacks.EvtEndpointReset = EventCallbacks->EvtEndpointReset;
        EndpointInit->EvtCallbacks.EvtEndpointPurge = EventCallbacks->EvtEndpointPurge;
        EndpointInit->EvtCallbacks.EvtEndpointStart = EventCallbacks->EvtEndpointStart;
        EndpointInit->EvtCallbacks.EvtEndpointAbort = EventCallbacks->EvtEndpointAbort;
        EndpointInit->EvtCallbacks.EvtEndpointOkToCancelTransfers =
            EventCallbacks->EvtEndpointOkToCancelTransfers;
        EndpointInit->EvtCallbacks.EvtEndpointStaticStreamsAdd =
            EventCallbacks->EvtEndpointStaticStreamsAdd;
        EndpointInit->EvtCallbacks.EvtEndpointStaticStreamsEnable =
            EventCallbacks->EvtEndpointStaticStreamsEnable;
        EndpointInit->EvtCallbacks.EvtEndpointStaticStreamsDisable =
            EventCallbacks->EvtEndpointStaticStreamsDisable;
        EndpointInit->EvtCallbacks.EvtEndpointEnableForwardProgress
            = EventCallbacks->EvtEndpointEnableForwardProgress;

    } FINALLY {

    }
}

VOID
EXPORT(UcxEndpointSetWdfIoQueue) (
    __in_opt
        PUCX_GLOBALS        Globals,
    __in
        UCXENDPOINT         UcxEndpoint,
    __in
        WDFQUEUE            WdfQueue
)
/*++

Routine Description:

    This export function set a WdfQueue on an Endpoint object.
    This routine can only be called from the EvtEndpointCreate and EvtDefaultEndpointCreate
    callbacks.
    The client driver must call this routine exactly once for each endpoint

Arguments:

    Globals - Driver globals of the controller driver. This information is not
        being used.

    EndpointInit - A handle to the UCXENDPOINT structure.

    WdfQueue - A handle to the Wdf Queue that is to be set in the EndpointInit
        structure.

Return Value:

    VOID
--*/
{
    PUCXENDPOINT_PRIVCONTEXT ucxEndpointContext;

    UNREFERENCED_PARAMETER(Globals);

    TRY {

        ucxEndpointContext = UcxEndpointGetContext(UcxEndpoint);
        if (ucxEndpointContext->UcxPipeInfo.Queue) {
            LogError(FlagEndpoint, "UcxEndpointSetWdfIoQueue can only be called once, EndpointContext %p, Last Set Queue %p, Queue Being Set %p",
                     ucxEndpointContext, ucxEndpointContext->UcxPipeInfo.Queue, WdfQueue);
            LEAVE;
        }

        WdfObjectReferenceWithTag(WdfQueue, TAG(EXPORT(UcxEndpointSetWdfIoQueue)));
        ucxEndpointContext->UcxPipeInfo.Queue = WdfQueue;

    } FINALLY {

    }
}

VOID
Endpoint_EvtDestroyCallback(
    __in WDFOBJECT UcxEndpoint
    )
/*++

Routine Description:

    This is an Event Callback function called when the endpoint object's
    reference count go to 0
    This function may be called if there is an init failure, with a zeored
    context. (This is due to the way Ucx_WdfObjectCreateWith2Attributes works.)

Arguments:

    UcxEndpoint - Handle to the endpoint object

--*/
{
    PUCXENDPOINT_PRIVCONTEXT ucxEndpointContext;

    TRY {
        ucxEndpointContext = UcxEndpointGetContext(UcxEndpoint);

        if (ucxEndpointContext->SmEngineContextInitialized) {
            StateMachineEngine_ReleaseResources(&ucxEndpointContext->SmEngineContext);
            ucxEndpointContext->SmEngineContextInitialized = FALSE;
        }

        if (ucxEndpointContext->UcxPipeInfo.Queue != NULL) {
            WdfObjectDereferenceWithTag(ucxEndpointContext->UcxPipeInfo.Queue,
                                        TAG(EXPORT(UcxEndpointSetWdfIoQueue)));
            ucxEndpointContext->UcxPipeInfo.Queue = NULL;
        }

        //
        // Mark the pipe handle as invalid (in case the memory is not immediately reused)
        //
        ucxEndpointContext->UcxPipeInfo.Self = NULL;

        //
        // Release reference to parent USB device that was acquired during create.
        // NULL check protects against zeroed context, in which case no reference was ever taken.
        //
        if (ucxEndpointContext->UcxUsbDeviceContext != NULL) {

            WdfObjectDereferenceWithTag(ucxEndpointContext->UcxUsbDeviceContext->UcxUsbDevice,
                                        TAG(EXPORT(UcxEndpointCreate)));
        }

    } FINALLY {

    }

}

_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
EXPORT(UcxEndpointCreate) (
    __in_opt
        PUCX_GLOBALS                  Globals,
    __in
        UCXUSBDEVICE                  UcxUsbDevice,
    __deref_inout_opt
        PUCXENDPOINT_INIT            *EndpointInit,
    __in_opt
        PWDF_OBJECT_ATTRIBUTES        Attributes,
    __out
        UCXENDPOINT                  *Endpoint
    )
/*++

Routine Description:

    This export function creates an endpoint object

Arguments:

    Globals - Driver globals of the controller driver. This information is not
        being used.

    UcxUsbDevice - The handle to the UsbDevice for which the endpoint is being
        created

    EndpointInit - Pointer to an init structure describing various
        information for the endpoint creation.

    Attributes - Pointer to the object attributes structure

    Endpoint - Output - Pointer to a location that will recieve a handle to the
        endpoint object

Return Value:

    NT_SUCCESS status if a endpoint object is  successfully created

--*/
{
    NTSTATUS                          status;
    UCXENDPOINT                       ucxEndpoint = NULL;
    PUCXUSBDEVICE_PRIVCONTEXT         ucxUsbDeviceContext;
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext = NULL;
    PUCXCONTROLLER_PRIVCONTEXT        ucxControllerContext;
    WDF_OBJECT_ATTRIBUTES             objectAttributes;
    USB_DEVICE_SPEED                  usbDeviceSpeed;
    KIRQL                             irql;

    UNREFERENCED_PARAMETER(Globals);

    TRY {

        //
        // Parameter Validation
        //

        //ASSERT(!"Add the necessary Parameter Validation");

        //
        // Create ucxUsbDevice object, using the Attributes given to us by
        // the controller driver to us. If the controller driver didnt give us
        // the attributes, use your own attributes.
        //

        *Endpoint = NULL;



        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&objectAttributes,
                                                UCXENDPOINT_PRIVCONTEXT);

        objectAttributes.ParentObject = (*EndpointInit)->UcxUsbDevice;
        objectAttributes.EvtDestroyCallback = Endpoint_EvtDestroyCallback;

        status = Ucx_WdfObjectCreateWith2Attributes(&objectAttributes,
                                                    Attributes,
                                                    &ucxEndpoint);

        if (!NT_SUCCESS(status)) {
            LogError(FlagEndpoint, "WdfObjectCreate Failed %!STATUS!", status);
            LEAVE;
        }

        //
        // Endpoint needs to access the parent device context, even after cleanup
        //
        WdfObjectReferenceWithTag(UcxUsbDevice, TAG(EXPORT(UcxEndpointCreate)));

        ucxEndpointContext = UcxEndpointGetContext(ucxEndpoint);

        ucxUsbDeviceContext = UcxUsbDeviceGetContext(UcxUsbDevice);

        ucxControllerContext = ucxUsbDeviceContext->UcxControllerContext;

        LogInfo(FlagEndpoint, "UcxControllerContext 0x%p, UcxUsbDevice 0x%p, UcxEndpoint 0x%p, UcxEndpointContext 0x%p",
                ucxUsbDeviceContext->UcxControllerContext, UcxUsbDevice, ucxEndpoint, ucxEndpointContext);

        //
        // Initialize and populate UcxUsbDeviceContext
        //
        ucxEndpointContext->Sig = SigUcxEndpointContext;
        ucxEndpointContext->UcxEndpoint = ucxEndpoint;
        ucxEndpointContext->UcxControllerContext = ucxUsbDeviceContext->UcxControllerContext;
        ucxEndpointContext->UcxUsbDeviceContext = ucxUsbDeviceContext;

        ucxEndpointContext->UcxPipeInfo.EndpointType = (*EndpointInit)->EndpointType;
        ucxEndpointContext->UcxPipeInfo.Self = &ucxEndpointContext->UcxPipeInfo;
        ucxEndpointContext->UcxPipeInfo.MaximumTransferSize =
            Endpoint_CalculateMaximumTransferSize(ucxUsbDeviceContext,
                                                  *EndpointInit);

        RtlCopyMemory(&ucxEndpointContext->EvtCallbacks,
                      &(*EndpointInit)->EvtCallbacks,
                      sizeof(UCX_ENDPOINT_ALL_EVENT_CALLBACKS));

        if ((*EndpointInit)->CreateStateMachine == TRUE) {

            //
            // Initialize The State Machine.
            //
            status = StateMachineEngine_Init(ucxEndpointContext->UcxControllerContext,
                                             &ucxEndpointContext->SmEngineContext,
                                             ucxEndpoint,
                                             ucxEndpointContext,
                                             UcxEndpointStateCreatedIndex,
                                             UcxEndpointStateTable,
                                             Etw_UcxEndpointStateMachineEvent,
                                             Etw_UcxEndpointStateMachineTransition);

            if (!NT_SUCCESS(status)) {
                LogError(FlagEndpoint, "StateMachineEngine_Init Failed, ucxEndpointContext 0x%p, %!STATUS!",
                         ucxEndpointContext, status);
                LEAVE;
            }

            ucxEndpointContext->SmEngineContext.SpecialHandlingForAbortUrb = 1;
            ucxEndpointContext->SmEngineContextInitialized = TRUE;

            //
            // We need to keep endpoint alive longer than when hub calls
            // EndpointDelete (which it does for SM endpoints only). Thus take
            // an additional reference to be released by the state machine.
            //
            WdfObjectReferenceWithTag(ucxEndpoint, TAG(EXPORT(UcxEndpointCreate)));

        } else {

            ucxEndpointContext->SmEngineContextInitialized = FALSE;
        }

        //
        // If it is the default endpoint we need to save the endpoint information
        // in the ucxUsbDeviceContext too.
        //
        if ((*EndpointInit)->EndpointType == UcxEndpointTypeDefault) {
            NT_ASSERT(ucxUsbDeviceContext->DefaultEndpointContext == NULL);
            NT_ASSERT(ucxUsbDeviceContext->DefaultPipeInfo == NULL);
            ucxUsbDeviceContext->DefaultEndpointContext = ucxEndpointContext;
            ucxUsbDeviceContext->DefaultPipeInfo = &ucxEndpointContext->UcxPipeInfo;

        }

        ucxEndpointContext->UcxPipeInfo.EndpointTransferType = (*EndpointInit)->EndpointTransferType;

        RtlCopyMemory(&ucxEndpointContext->EndpointDescriptor,
                      &((*EndpointInit)->EndpointDescriptor),
                      sizeof(ucxEndpointContext->EndpointDescriptor));

        if (ucxEndpointContext->UcxPipeInfo.EndpointTransferType != EndpointTransferTypeControl) {
            if (USB_ENDPOINT_DIRECTION_IN(ucxEndpointContext->EndpointDescriptor.bEndpointAddress)) {
                ucxEndpointContext->UcxPipeInfo.IsEndpointDirectionIn = TRUE;
            } else {
                ucxEndpointContext->UcxPipeInfo.IsEndpointDirectionIn = FALSE;
            }
        }

        //
        // Set the MaximumIsochPacketCount value as per the USBPORT_IsochTransfer for the
        // USB2.0 stack
        //
        if (ucxEndpointContext->UcxPipeInfo.EndpointTransferType ==
            EndpointTransferTypeIsochronous){
            usbDeviceSpeed = ucxEndpointContext->UcxUsbDeviceContext->UcxUsbDeviceInfo.DeviceSpeed;
            if (usbDeviceSpeed == UsbLowSpeed || usbDeviceSpeed == UsbFullSpeed) {
                ucxEndpointContext->UcxPipeInfo.MaximumIsochPacketCount = 255;
                ucxEndpointContext->UcxPipeInfo.IsochPeriodInMicroFrames = 8;
            } else {
                ucxEndpointContext->UcxPipeInfo.MaximumIsochPacketCount = 1024;
                //
                // For HighSpeed & SuperSpeed, IsochPeriod = 2 ^ (bInterval - 1)
                //
                ucxEndpointContext->UcxPipeInfo.IsochPeriodInMicroFrames
                    = 1 << (ucxEndpointContext->EndpointDescriptor.bInterval - 1);

            }
        }

        //
        // Determine whether or not a Clear TT Buffer request should be
        // issued to the upstream TT hub (if any) if a transfer is canceled
        // on this endpoint.
        //
        ucxEndpointContext->ClearTtBufferOnTransferCancel = ucxControllerContext->ClearTtBufferOnAsyncTransferCancel;

        if ((ucxEndpointContext->UcxPipeInfo.EndpointTransferType == EndpointTransferTypeInterrupt) ||
            (ucxEndpointContext->UcxPipeInfo.EndpointTransferType == EndpointTransferTypeIsochronous)) {

            //
            // We only need to do a Clear TT Buffer for Bulk / Control Pipes
            //
            ucxEndpointContext->ClearTtBufferOnTransferCancel = FALSE;
        }

        if ((ucxUsbDeviceContext->UcxUsbDeviceInfo.DeviceSpeed != UsbLowSpeed) &&
            (ucxUsbDeviceContext->UcxUsbDeviceInfo.DeviceSpeed != UsbFullSpeed)) {

            //
            // Clear TT Buffer only applicable for Low or Full Speed Devices
            //
            ucxEndpointContext->ClearTtBufferOnTransferCancel = FALSE;
        }

        if (ucxUsbDeviceContext->UcxUsbDeviceInfo.TtHub == NULL) {

            //
            // There is no TTHub, So no Clear TT Buffer is needed.
            //
            ucxEndpointContext->ClearTtBufferOnTransferCancel = FALSE;
        }

        InitializeListEntry(&ucxEndpointContext->UsbDeviceDisconnectListEntryTemp);
        InitializeListEntry(&ucxEndpointContext->ControllerResetListEntryTemp);
        InitializeListEntry(&ucxEndpointContext->EndpointListEntry);
        InitializeListEntry(&ucxEndpointContext->EndpointListEntryTemp);
        InitializeListEntry(&ucxEndpointContext->EndpointListEntryForInvalidHandleTraking);

        //
        // UcxDevice needs to track all its endpoints, so add this into that
        // list holding a global lock.
        // Note: We could have used a Per UcxUsbDevice Lock here, however it
        // makes implementation of UsbDeviceDisconnect routine complicated.
        // Since this is not the hot path, using a simpler solution
        //

        KeAcquireSpinLock(&ucxControllerContext->TopologyLock, &irql);

        //
        // If the device has been disconnected, there is no use to
        // create the endpoint.
        //
        if (ucxUsbDeviceContext->Disconnected == FALSE) {

            InsertHeadList(&ucxUsbDeviceContext->EndpointListHead,
                           &ucxEndpointContext->EndpointListEntry);

            ucxControllerContext->ChildEndpointListCount++;
        } else {
            status = STATUS_DEVICE_NOT_CONNECTED;
        }

        KeReleaseSpinLock(&ucxControllerContext->TopologyLock, irql);

        if (!NT_SUCCESS(status)) {
            LEAVE;
        }

        if (ucxUsbDeviceContext->AllowIoOnInvalidPipeHandles) {

            KeAcquireSpinLock(&ucxUsbDeviceContext->InvalidHandleTrakingLock, &irql);

            //
            // Due to a small race condition, we must check if the endpoint is already on the
            // tracking list. This could have happened if a client opened a USBD handle with
            // verifier enabled between our dropping the TopologyLock and acquiring
            // InvalidHandleTrakingLock.
            //
            if (IsListEntryNULL(&ucxEndpointContext->EndpointListEntryForInvalidHandleTraking)) {

                InsertTailList(&ucxUsbDeviceContext->EndpointListHeadForInvalidHandleTracking,
                               &ucxEndpointContext->EndpointListEntryForInvalidHandleTraking);
            }

            KeReleaseSpinLock(&ucxUsbDeviceContext->InvalidHandleTrakingLock, irql);
        }

    } FINALLY {

        if (!NT_SUCCESS(status)) {

            if (ucxEndpoint != NULL) {

                if (ucxEndpointContext->SmEngineContextInitialized == TRUE) {

                    //
                    // State machine will not run to completion, so we must drop the EP reference
                    // explicitly
                    //
                    WdfObjectDereferenceWithTag(ucxEndpoint, TAG(EXPORT(UcxEndpointCreate)));
                }

                WdfObjectDelete(ucxEndpoint);
            }

        } else {
            *Endpoint = ucxEndpoint;
            (*EndpointInit)->UcxEndpoint = ucxEndpoint;
            *EndpointInit = NULL;
        }

        LogVerbose(FlagEndpoint, "UcxEndpointCreate 0x%!STATUS!", status);

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
UCXSSTREAMS
EXPORT(UcxEndpointGetStaticStreamsReferenced) (
    __in_opt
        PUCX_GLOBALS                  Globals,
    __in
        UCXENDPOINT                   UcxEndpoint,
    __in
        PVOID                         Tag
    )
/*++

Routine Description:

    This export function returns a referenced Static Streams Object for the endpoint

Arguments:

    Globals - Driver globals of the controller driver. This information is not
        being used.

    UcxUsbDevice - The handle to the Endpoint for which the the static streams object is requested

    Tag - Tag to be passed into WdfObjectReferenceWithTag

Return Value:

    UCXSSTREAMS if the stream object is opened with the Endpoint, NULL otherwise

--*/
{
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext;
    KIRQL                             irql;
    UCXSSTREAMS                       sStreams = NULL;

    UNREFERENCED_PARAMETER(Globals);

    TRY {

        ucxEndpointContext = UcxEndpointGetContext(UcxEndpoint);

        if (ucxEndpointContext->UcxSStreamsContext == NULL) {
            //
            // No need to acquire the lock in the most common case.
            //
            LEAVE;
        }

        KeAcquireSpinLock(&ucxEndpointContext->UcxControllerContext->TopologyLock, &irql);

        if (ucxEndpointContext->UcxSStreamsContext != NULL) {
            WdfObjectReferenceWithTag(ucxEndpointContext->UcxSStreamsContext->UcxSStreams, Tag);
            sStreams = ucxEndpointContext->UcxSStreamsContext->UcxSStreams;
        }

        KeReleaseSpinLock(&ucxEndpointContext->UcxControllerContext->TopologyLock, irql);

    } FINALLY {

    }

    return sStreams;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EXPORT(UcxEndpointPurgeComplete) (
    __in_opt
        PUCX_GLOBALS                  Globals,
    __in
        UCXENDPOINT                   Endpoint
    )
/*++

Routine Description:

    This export function is called to notify Purge comption on an endpoint object

Arguments:

    Globals - Driver globals of the controller driver. This information is not
        being used.

    Endpoint - The handle to the endpoint object

Return Value:

--*/
{
    PUCXENDPOINT_PRIVCONTEXT ucxEndpointContext;

    UNREFERENCED_PARAMETER(Globals);

    TRY {
        ucxEndpointContext = UcxEndpointGetContext(Endpoint);

        Endpoint_EventAdd(ucxEndpointContext, UcxEndpointEventPurgeComplete);
    } FINALLY {


    }

}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EXPORT(UcxEndpointAbortComplete) (
    __in_opt
        PUCX_GLOBALS                  Globals,
    __in
        UCXENDPOINT                   Endpoint
    )
/*++

Routine Description:

    This export function is called to notify Abort comption on an endpoint object

Arguments:

    Globals - Driver globals of the controller driver. This information is not
        being used.

    Endpoint - The handle to the endpoint object

Return Value:

--*/
{
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext;

    UNREFERENCED_PARAMETER(Globals);

    TRY {
        ucxEndpointContext = UcxEndpointGetContext(Endpoint);

        Endpoint_EventAdd(ucxEndpointContext, UcxEndpointEventAbortComplete);
    } FINALLY {


    }

}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EXPORT(UcxEndpointNoPingResponseError) (
    __in_opt
        PUCX_GLOBALS                  Globals,
    __in
        UCXENDPOINT                   Endpoint
    )
/*++

Routine Description:

    This export function is called to notify No Ping Response Error for a transfer on
    an endpoint object

Arguments:

    Globals - Driver globals of the controller driver. This information is not
        being used.

    Endpoint - The handle to the endpoint object

Return Value:

--*/
{
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext;

    UNREFERENCED_PARAMETER(Globals);

    TRY {
        ucxEndpointContext = UcxEndpointGetContext(Endpoint);
        ucxEndpointContext->UcxUsbDeviceContext->PendingNoPingResponseError = 1;

    } FINALLY {


    }

}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EXPORT(UcxEndpointNeedToCancelTransfers) (
    __in_opt
        PUCX_GLOBALS                  Globals,
    __in
        UCXENDPOINT                   Endpoint
    )
/*++

Routine Description:

    This export function is called by HCD before it cancels transfers on the wire.

Arguments:

    Globals - Driver globals of the controller driver. This information is not
        being used.

    Endpoint - The handle to the endpoint object

Commnets:

    If needed, UCX coordinates with the HUB to send a Clear TT buffer command to the TT Hub.

    Once that is Once That is done, Ucx calls the EvtEndpointOkToCancelTransfers

Return Value:

--*/
{
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext;
    PUCXUSBDEVICE_PRIVCONTEXT         ucxUsbDeviceContext;
    UCXUSBDEVICE                      ttHub;
    PUCXUSBDEVICE_PRIVCONTEXT         ttHubContext;
    ULONG                             ttHubDepth;
    ULONG                             ttHubPortNumber;


    UNREFERENCED_PARAMETER(Globals);

    TRY {
        ucxEndpointContext = UcxEndpointGetContext(Endpoint);

        if (!ucxEndpointContext->ClearTtBufferOnTransferCancel) {

            //
            // No need to do a Clear TT Buffer for this endpoint.
            //
            ucxEndpointContext->EvtCallbacks.EvtEndpointOkToCancelTransfers(Endpoint);
            LEAVE;
        }

        //
        // The current expected value of OkToCancelTransferCount is 0.
        // Before we call into Hcd with EvtEndpointOkToCancelTransfers, we want the following two
        // conditions met:
        // * The callback HubQICallbackClearTTBuffer has returned.
        // * Hub has called ClearTTBufferComplete
        //

        if (0 != InterlockedCompareExchange(&ucxEndpointContext->OkToCancelTransfersSyncEvent,
                                            2, 0)){

            //
            // The OkToCancelTransferCount values must have been 0, and it was not.
            // This is a bug in HCD driver. It called UcxEndpointNeedToCancelTransfers
            // more than once without waiting for EvtEndpointOkToCancelTransfers.
            //
            LogError(FlagEndpoint, "HCD called UcxEndpointNeedToCancelTransfers more than once, 0x%p", Endpoint);
            NT_ASSERTMSG("HCD called UcxEndpointNeedToCancelTransfers more than once", FALSE);
            LEAVE;
        }

        WdfObjectReferenceWithTag(Endpoint, TAG(EXPORT(UcxEndpointNeedToCancelTransfers)));

        ucxUsbDeviceContext = ucxEndpointContext->UcxUsbDeviceContext;
        ttHub = ucxUsbDeviceContext->UcxUsbDeviceInfo.TtHub;
        ttHubContext = UcxUsbDeviceGetContext(ttHub);
        ttHubDepth = ucxUsbDeviceContext->UcxUsbDeviceInfo.PortPath.TTHubDepth;
        ttHubPortNumber = ucxUsbDeviceContext->UcxUsbDeviceInfo.PortPath.PortPath[ttHubDepth];

        ttHubContext->HubQICallbackClearTTBuffer(ttHubContext->HubQIContext,
                                                 ucxUsbDeviceContext->ContextFromHub,
                                                 Endpoint,
                                                 ucxEndpointContext->EndpointDescriptor.bEndpointAddress,
                                                 ttHubPortNumber);

        Endpoint_TryToCallEvtEndpointOkToCancelTransfers(ucxEndpointContext);

    } FINALLY {


    }

}


VOID
Endpoint_FindAndCompleteAbortUrbIfNotAlreadyCancelled(
    __in
        PUCXENDPOINT_PRIVCONTEXT        UcxEndpointContext
    )
/*++

Routine Description:

    This routine is called when an Abort operation is completed.

    All abort transfer, while being handled, are forwarded to a AbortPipeCsq.

    This routine retrieves the Irp from the Csq, and completes it.

Arguments:

    UcxEndpointContext - Ucx context of the endpoint object, for which the abort request is being
        completed. .

Return Value:

--*/
{
    PURB                              urb;
    PIRP                              abortIrp;
    PUCXCONTROLLER_PRIVCONTEXT        ucxControllerContext;
    PIO_STACK_LOCATION                irpStack;


    TRY {

        ucxControllerContext = UcxEndpointContext->UcxControllerContext;

        abortIrp = IoCsqRemoveIrp(&ucxControllerContext->AbortPipeIrpCsq,
                                  &UcxEndpointContext->AbortPipeIrpCsqContext);

        if (NULL == abortIrp) {
            LEAVE;
        }

        irpStack = IoGetCurrentIrpStackLocation(abortIrp);
        urb = (PURB)irpStack->Parameters.Others.Argument1;

        //
        // COMPAT: Abort Urb is completed by the 2.0 stack from
        // a DPC. To preserve compat, complete non-XRB abort URBs
        // on DISPATCH_LEVEL
        //
        Xrb_IoCompleteRequest_OnDispatchIfNonXrb(abortIrp, urb, STATUS_SUCCESS, USBD_STATUS_SUCCESS);

    } FINALLY {

    }
    return;

}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Endpoint_StaticStreamsEnableUrb(
    __in
        PUCXENDPOINT_PRIVCONTEXT UcxEndpointContext,
    __in
        WDFDEVICE                RHPdo,
    __in
        PXRB                     Xrb,
    __in
        PIRP                     Irp
)
/*++

Routine Description:

    This function is called when we recieve a StaticStreamsEnable Urb from the client driver.

    This routine creates a new UCXSSTREAMS object.

Arguments:

    UcxEndpointContext - Ucx context of the endpoint object, for which streams are being enabled.

    Urb - The Enable static streams urb.

    Irp - Irp representing the Urb

Return Value:

    VOID

Comments:

    The function must be called at PASSIVE_LEVEL

--*/
{
    BOOLEAN                                     completeIrp;
    NTSTATUS                                    status;
    WDFQUEUE                                    targetQueue;
    PRHPDO_CONTEXT                              rhPdoContext;
    UCXSSTREAMS                                 streams;
    PUCXSSTREAMS_PRIVCONTEXT                    ucxSStreamsContext;
    KIRQL                                       irql;
    PURB                                        urb;
    PUSBDI_HANDLE_DATA                          usbdiHandleData;


    TRY {

        completeIrp = FALSE;

        urb = &Xrb->Urb;
        usbdiHandleData = Xrb->XrbHeader.UsbdiHandleData;
        if (KeGetCurrentIrql() != PASSIVE_LEVEL) {

            if (usbdiHandleData->UsbVerifierEnabled) {
                Usb3_KeBugCheckEx(USB3_VERIFIER_BUGCODE_OPEN_STATIC_STREAMS_WITH_IRQL_TOO_HIGH,
                                  KeGetCurrentIrql(),
                                  Irp,
                                  usbdiHandleData->DeviceObject);
            }

            LogError(FlagEndpoint, "Called at irql higher than passive (%d)", KeGetCurrentIrql());
            status = STATUS_INVALID_PARAMETER;
            completeIrp = TRUE;
            LEAVE;
        }

        if (FALSE == usbdiHandleData->StreamSupportGranted) {

            if (usbdiHandleData->UsbVerifierEnabled) {
                Usb3_KeBugCheckEx(USB3_VERIFIER_BUGCODE_OPEN_STATIC_STREAMS_WITHOUT_QUERYING_FOR_CAPABILITY,
                                  Irp,
                                  urb,
                                  usbdiHandleData->DeviceObject);
            }

            LogError(FlagEndpoint, "Streams support not granted");
            status = STATUS_INVALID_PARAMETER;
            completeIrp = TRUE;
            LEAVE;
        }

        if (urb->UrbOpenStaticStreams.NumberOfStreams > usbdiHandleData->StreamCountGranted ||
            urb->UrbOpenStaticStreams.NumberOfStreams == 0) {
            if (usbdiHandleData->UsbVerifierEnabled) {
                Usb3_KeBugCheckEx(USB3_VERIFIER_BUGCODE_OPEN_STATIC_STREAMS_INVALID_STREAM_COUNT,
                                  usbdiHandleData->StreamCountGranted,
                                  urb->UrbOpenStaticStreams.NumberOfStreams,
                                  urb);
            }

            LogError(FlagEndpoint, "Invalid Number Of Streams %d, Maximum should be %d",
                     urb->UrbOpenStaticStreams.NumberOfStreams, usbdiHandleData->StreamCountGranted);
            status = STATUS_INVALID_PARAMETER;
            completeIrp = TRUE;
            LEAVE;
        }

        if (Ucx_FailDueToVerifierSetting(usbdiHandleData->UsbVerifierFailEnableStaticStreams)) {

            status = Ucx_GetRandomErrorNTSTATUS();
            LogInfo(FlagEndpoint, "UsbVerifier failing Open Static streams, Status %!STATUS!", status);
            completeIrp = TRUE;
            LEAVE;
        }

        if (InterlockedIncrement(&UcxEndpointContext->StreamsEnabled) != 1) {

            if (usbdiHandleData->UsbVerifierEnabled) {
                Usb3_KeBugCheckEx(USB3_VERIFIER_BUGCODE_STATIC_STREAMS_ALREADY_OPEN,
                                  Irp,
                                  urb,
                                  usbdiHandleData->DeviceObject);
            }

            InterlockedDecrement(&UcxEndpointContext->StreamsEnabled);
            LogError(FlagEndpoint, "Streams are already enabled");
            status = STATUS_INVALID_DEVICE_STATE;
            completeIrp = TRUE;
            LEAVE;
        }

        //
        // Create the static streams object first
        //
        status = StaticStreams_Create(UcxEndpointContext,
                                      urb->UrbOpenStaticStreams.NumberOfStreams,
                                      &streams);

        if (!NT_SUCCESS(status)) {
            InterlockedDecrement(&UcxEndpointContext->StreamsEnabled);
            LogError(FlagEndpoint, "StaticStreamsCreate Failed %!STATUS!", status);
            completeIrp = TRUE;
            LEAVE;
        }

        ucxSStreamsContext = UcxStaticStreamsGetContext(streams);
        KeAcquireSpinLock(&UcxEndpointContext->UcxControllerContext->TopologyLock, &irql);
        NT_ASSERT(UcxEndpointContext->UcxSStreamsContext == NULL);
        UcxEndpointContext->UcxSStreamsContext = ucxSStreamsContext;
        KeReleaseSpinLock(&UcxEndpointContext->UcxControllerContext->TopologyLock, irql);

        //
        // Initialize the URB status to USBD_STATUS_PENDING.
        //
        urb->UrbHeader.Status = USBD_STATUS_PENDING;

        rhPdoContext = RHPdoGetContext(RHPdo);
        targetQueue = rhPdoContext->UcxControllerContext->QueueUsbDeviceMgmt;

        status = UcxForwardIrpToQueueWithCompletion(RHPdo,
                                                    Irp,
                                                    Xrb_UrbCompletion,
                                                    urb,
                                                    targetQueue);

    } FINALLY {

        if (completeIrp) {
            Xrb_IoCompleteRequest(Irp, urb, status, NTSTATUS_To_USBD_STATUS(status));
        }

    }

    return status;

}

__drv_maxIRQL(DISPATCH_LEVEL)
NTSTATUS
Endpoint_StaticStreamsDisableUrb(
    __in
        PUCXENDPOINT_PRIVCONTEXT UcxEndpointContext,
    __in
        WDFDEVICE                RHPdo,
    __in
        PXRB                     Xrb,
    __in
        PIRP                     Irp
)
/*++

Routine Description:

    This function is called when we recieve a StaticStreamsDiable Urb from the client driver.

Arguments:

    UcxEndpointContext - Ucx context of the endpoint object, for which streams are being enabled.

    Xrb - The Enable static streams urb.

    Irp - Irp representing the Urb

Return Value:

    VOID

Comments:

    The function must be called at PASSIVE_LEVEL

--*/
{
    BOOLEAN                                     completeIrp;
    NTSTATUS                                    status;
    WDFQUEUE                                    targetQueue;
    PRHPDO_CONTEXT                              rhPdoContext;

    TRY {

        completeIrp = FALSE;

        if (KeGetCurrentIrql() != PASSIVE_LEVEL) {
            LogError(FlagEndpoint, "Called at irql higher than passive (%d)",KeGetCurrentIrql());
            status = STATUS_INVALID_PARAMETER;
            completeIrp = TRUE;
            LEAVE;
        }

        if (NULL == UcxEndpointContext->UcxSStreamsContext) {
            LogError(FlagEndpoint, "No streams are enabled");
            status = STATUS_INVALID_DEVICE_STATE;
            completeIrp = TRUE;
            LEAVE;
        }

        //
        // Initialize the URB status to USBD_STATUS_PENDING.
        //
        Xrb->Urb.UrbHeader.Status = USBD_STATUS_PENDING;

        rhPdoContext = RHPdoGetContext(RHPdo);
        targetQueue = rhPdoContext->UcxControllerContext->QueueUsbDeviceMgmt;

        status = UcxForwardIrpToQueueWithCompletion(RHPdo,
                                                    Irp,
                                                    Xrb_UrbCompletion,
                                                    &Xrb->Urb,
                                                    targetQueue);


    } FINALLY {

        if (completeIrp) {
            Xrb_IoCompleteRequest(Irp, &Xrb->Urb, status, NTSTATUS_To_USBD_STATUS(status));
        }

    }

    return status;
}


VOID
Endpoint_ResetCompleteFromHcd(
    __in
        PIRP                    Irp,
    __in
        PENDPOINT_RESET         EndpointReset
   )
/*++

Routine Description:

    This routine is called when the controller driver completes an
    IOCTL_INTERNAL_USB ENDPOINT_RESET request.

Arguments:

    Irp - Endpoints Configure Irp

    EndpointReset - A pointer to the EndpointReset structure in the
    ioctl.

--*/
{
    UCXENDPOINT                       ucxEndpoint;
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext;

    TRY {

        ucxEndpoint = EndpointReset->Endpoint;
        ucxEndpointContext = UcxEndpointGetContext(ucxEndpoint);

        NT_ASSERT(ucxEndpointContext->PendingResetIrp == NULL);
        ucxEndpointContext->PendingResetIrp = Irp;

        LogVerbose(FlagUsbDevice,"Pending Irp 0x%p", Irp);

        //
        // enqueue a UcxEndpointEventEnable event for the default endpoint
        //

        Endpoint_EventAdd(ucxEndpointContext,
                          UcxEndpointEventEndpointResetComplete);

    } FINALLY {

    }
    return;
}


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_StaticStreamsEnableUrbCompleteFromHcd(
    __in
        PUCXENDPOINT_PRIVCONTEXT UcxEndpointContext,
    __in
        PURB                     Urb,
    __in
        PIRP                     Irp
)
/*++

Routine Description:

    This function is called when hcd completes the Enable Streams request.

    This routine fills the information about the streams in the Urb. It then schedules an event
    into the endpoint state machine to complete the urb

Arguments:

    UcxEndpointContext - Ucx context of the endpoint object, for which streams are being enabled.

    Urb - The Enable static streams urb.

    Irp - irp representing the urb

Return Value:

    VOID

--*/
{
    ULONG                    arrayIndex;
    PUCXSSTREAMS_PRIVCONTEXT ucxSStreamsContext;
    PUSBD_STREAM_INFORMATION streamInfo;
    NTSTATUS                 status;
    KIRQL                    irql;

    TRY {

        ucxSStreamsContext = UcxEndpointContext->UcxSStreamsContext;
        status = Irp->IoStatus.Status;

        if (!NT_SUCCESS(status)) {
            //
            // Need to delete the streams object we created earlier.
            //
            LogError(FlagEndpoint, "StreamsEnable Failed by HCD %!STATUS!", status);
            KeAcquireSpinLock(&UcxEndpointContext->UcxControllerContext->TopologyLock, &irql);
            UcxEndpointContext->UcxSStreamsContext = NULL;
            KeReleaseSpinLock(&UcxEndpointContext->UcxControllerContext->TopologyLock, irql);
            WdfObjectDelete(ucxSStreamsContext->UcxSStreams);
            LEAVE;
        }

        //
        // Walk through each stream and fill its infomation in the Urb.
        //

        NT_ASSERT(ucxSStreamsContext->NumberOfStreams ==
                  Urb->UrbOpenStaticStreams.NumberOfStreams);

        for (arrayIndex=0;
             arrayIndex < ucxSStreamsContext->NumberOfStreams;
             arrayIndex++) {

            streamInfo =  &Urb->UrbOpenStaticStreams.Streams[arrayIndex];
            streamInfo->PipeHandle = &ucxSStreamsContext->StreamContexts[arrayIndex].StreamPipeInfo;
            streamInfo->StreamID = ucxSStreamsContext->StreamContexts[arrayIndex].StreamID;
            streamInfo->MaximumTransferSize =
                ucxSStreamsContext->StreamContexts[arrayIndex].StreamPipeInfo.MaximumTransferSize;
            streamInfo->PipeFlags = 0;

        }

        Etw_UcxStaticStreamsCreate(NULL, ucxSStreamsContext);

    } FINALLY {
        if (UcxEndpointContext->OpenStaticStreamsFailedDueToControllerReset == 1) {
            //
            // We did not submit the Open Static streams operation to the endpoint state
            // machine due to controller reset process.
            // Thus we do not notify the endpoint state machine on the completion path either.
            //
            UcxEndpointContext->OpenStaticStreamsFailedDueToControllerReset = 0;
            InterlockedDecrement(&UcxEndpointContext->StreamsEnabled);
            Xrb_IoCompleteRequest_StatusAlreadySet(Irp);
        } else {
            //
            // Queue an event in the Endpoint state machine to complete that request.
            //
            UcxEndpointContext->PendingStreamsEnableIrp = Irp;
            Endpoint_EventAdd(UcxEndpointContext, UcxEndpointEventStaticStreamsEnableComplete);
        }
    }

}


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_StaticStreamsDisableUrbComplete(
    __in
        PUCXENDPOINT_PRIVCONTEXT UcxEndpointContext,
    __in
        PIRP                     Irp
)
/*++

Routine Description:

    This function is called when hcd completes the Disable Streams request.

    It then schedules an event into the endpoint state machine to complete the urb

Arguments:

    UcxEndpointContext - Ucx context of the endpoint object, for which streams are being disabled.

    Urb - The Disable static streams urb.

    Irp - irp representing the urb

Return Value:

    VOID

--*/
{
    TRY {

        UcxEndpointContext->PendingStreamsDisableIrp = Irp;
        Endpoint_EventAdd(UcxEndpointContext, UcxEndpointEventStaticStreamsDisableComplete);

    } FINALLY {

    }

}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_TryToCallEvtEndpointOkToCancelTransfers(
    __in
        PUCXENDPOINT_PRIVCONTEXT UcxEndpointContext
)
/*++

Routine Description:

    This routine is called to see if it is Ok to call EvtEndpointOkToCancelTransfers

Arguments:

    UcxEndpointContext - Ucx context of the endpoint object, for which streams are being disabled.


Return Value:

    VOID

--*/
{
    TRY {

        if (InterlockedDecrement(&UcxEndpointContext->OkToCancelTransfersSyncEvent) == 0) {
            NT_ASSERT(UcxEndpointContext->UcxUsbDeviceContext->PendingDelete == FALSE);
            UcxEndpointContext->EvtCallbacks.EvtEndpointOkToCancelTransfers(
                UcxEndpointContext->UcxEndpoint);
            WdfObjectDereferenceWithTag(UcxEndpointContext->UcxEndpoint, TAG(EXPORT(UcxEndpointNeedToCancelTransfers)));
        }
    } FINALLY {

    }

}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Endpoint_ClearTTBufferComplete(
    __in
        UCXUSBDEVICE Hub,
    __in
        UCXENDPOINT  Endpoint
)
/*++

Routine Description:

    This routine is called by the hub driver to report that a ClearTTBuffer
    complete operation has been completed.

Arguments:

    Hub - Unused - TT Hub

    Endpoint - Endpoint of the device.

Return Value:

    VOID

--*/
{
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext;

    UNREFERENCED_PARAMETER(Hub);

    TRY {

        ucxEndpointContext = UcxEndpointGetContext(Endpoint);

        Endpoint_TryToCallEvtEndpointOkToCancelTransfers(ucxEndpointContext);

    } FINALLY {

    }

}


