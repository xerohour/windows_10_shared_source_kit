/*++
Copyright (c) Microsoft Corporation

Module Name:

    usbdevice.c

Abstract:

    USB Controller Extension Driver

--*/

#include "pch.h"

#include "usbdevice.tmh"



_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
UsbDevice_Create(
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        PUSBDEVICE_INFO    UsbDeviceCreateInfo,
    __out 
        UCXUSBDEVICE      *UsbDevice
)
/*++

Routine Description:

    This Query interface function is called by the hub driver to create
    a UsbDevice object.
        
Arguments:

    Hub - The handle to the UCXUSBDEVICE representing the hub that called this 
        function
        
    UsbDeviceCreateInfo - Pointer to a structure that has information about this
        usbdevice
        
    UsbDevice - Output - A pointer to a location that recieves a handle to the 
        created usbdevice

Return Value: 
    
    NT_SUCCESS status if a usbdevice object is successfully created
    
--*/
{
    NTSTATUS                      status;
    PUCXUSBDEVICE_PRIVCONTEXT     hubContext;
    UCXCONTROLLER                 ucxController;
    PUCXCONTROLLER_PRIVCONTEXT    ucxControllerContext;
    UCXUSBDEVICE_INIT             ucxUsbDeviceInit;
    PUCXUSBDEVICE_INFO            ucxUsbDeviceInfo;
    ULONG                         portPathDepth;

    TRY {
        
        hubContext = UcxUsbDeviceGetContext(Hub);

        NT_ASSERT(IsTypeHub(hubContext->UsbDeviceType));
        
        ucxControllerContext = hubContext->UcxControllerContext;

        ucxController = ucxControllerContext->UcxController;

        RtlZeroMemory(&ucxUsbDeviceInit, sizeof(ucxUsbDeviceInit));
        ucxUsbDeviceInit.Sig = SigUcxUsbDeviceInit;
        ucxUsbDeviceInit.ParentHub = Hub;
        ucxUsbDeviceInit.ContextFromHub = UsbDeviceCreateInfo->Context;
        
        ucxUsbDeviceInfo = & ucxUsbDeviceInit.UcxUsbDeviceInfo;
        RtlZeroMemory(ucxUsbDeviceInfo, sizeof(*ucxUsbDeviceInfo));
        ucxUsbDeviceInfo->Size = sizeof(*ucxUsbDeviceInfo);
        ucxUsbDeviceInfo->DeviceSpeed = UsbDeviceCreateInfo->DeviceSpeed;

        //
        // Build the port path. 
        //

        if (IsTypeRootHub(hubContext->UsbDeviceType)) {
            //
            // The device is directly plugged into roothub.
            //
            ucxUsbDeviceInfo->PortPath.Size = sizeof(ucxUsbDeviceInfo->PortPath);
            ucxUsbDeviceInfo->PortPath.PortPathDepth = 1;
            ucxUsbDeviceInfo->PortPath.PortPath[0] = UsbDeviceCreateInfo->PortNumber;

        } else {

            //
            // Port Path is Parent's Port path + information about the port
            //
            RtlCopyMemory(&ucxUsbDeviceInfo->PortPath, 
                          &hubContext->UcxUsbDeviceInfo.PortPath, 
                          sizeof(ucxUsbDeviceInfo->PortPath));

            ucxUsbDeviceInfo->PortPath.PortPathDepth++;
            portPathDepth = ucxUsbDeviceInfo->PortPath.PortPathDepth;

            if (portPathDepth > MAX_USB_DEVICE_DEPTH) {
                LogError(FlagHubInterface, "Cannot create a device at depth %d", portPathDepth);
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }

            ucxUsbDeviceInfo->PortPath.PortPath[ portPathDepth-1 ] 
                = UsbDeviceCreateInfo->PortNumber;

            if (ucxUsbDeviceInfo->DeviceSpeed == UsbFullSpeed ||
                ucxUsbDeviceInfo->DeviceSpeed == UsbLowSpeed) {

                //
                // This is a LS / FS Device. We must set the TT Hub Information
                //
                if (hubContext->UcxUsbDeviceInfo.DeviceSpeed == UsbHighSpeed) {
                    //
                    // This FS/LS Device is plugged into a HS Hub.
                    // Thus the parent hub is the TTHub for this device. 
                    //
                    ucxUsbDeviceInfo->PortPath.TTHubDepth 
                        = hubContext->UcxUsbDeviceInfo.PortPath.PortPathDepth;
                    ucxUsbDeviceInfo->TtHub = Hub;

                } else {
                    //
                    // This is a FS/LS Device, and the Parent is a FS hub.
                    // Thus the Parent Hub's TTHub is the TTHub for this device
                    //
                    ucxUsbDeviceInfo->PortPath.TTHubDepth 
                        = hubContext->UcxUsbDeviceInfo.PortPath.TTHubDepth;
                    ucxUsbDeviceInfo->TtHub = hubContext->UcxUsbDeviceInfo.TtHub;
                }
            }
        }

        if (Controller_ResetReferenceAcquire(ucxControllerContext)) {
            
            status = ucxControllerContext->Config.EvtControllerUsbDeviceAdd(ucxController,
                                                                            ucxUsbDeviceInfo,
                                                                            &ucxUsbDeviceInit);            
            Controller_ResetReferenceRelease(ucxControllerContext);

        } else {

            status = STATUS_NO_SUCH_DEVICE;
            LogInfo(FlagUsbDevice, "Controller_ResetReferenceAcquire failed");

        }

        if (!NT_SUCCESS(status)) {
            LogError(FlagHubInterface, "EvtControllerUsbDeviceAdd Failed %!STATUS!", status);
            LEAVE;
        }

    } FINALLY {

        if (NT_SUCCESS(status)) {
            
            *UsbDevice = ucxUsbDeviceInit.UcxUsbDevice;
        
            Etw_UcxDeviceCreate(NULL, UcxUsbDeviceGetContext(ucxUsbDeviceInit.UcxUsbDevice));

        } else {

            *UsbDevice = NULL;

            if (ucxUsbDeviceInit.UcxUsbDevice) {
                UsbDevice_Delete(Hub, ucxUsbDeviceInit.UcxUsbDevice);
            }
        }

    }

    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
UsbDevice_Delete(
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        UCXUSBDEVICE       UsbDevice
)
/*++

Routine Description:

    This Query interface function is called by the hub driver to delete
    a UsbDevice object.
        
Arguments:

    Hub - The handle to the UCXUSBDEVICE representing the hub that called this 
        function
        
    UsbDevice - A pointer to usbdevice that is to be deleted.
    
Comments:
    
    The usbdevice object is reference counted. Thus calling delete does not 
    necessarily mean that the usbdevice object memory will be freed. The memory
    will be freed, when the refrence count goes to zero.

--*/
{
    PUCXUSBDEVICE_PRIVCONTEXT         usbDeviceContext;
    PUCXCONTROLLER_PRIVCONTEXT        ucxControllerContext;
    KIRQL                             irql;
    LIST_ENTRY                        staleEndpointListHead;
    PUCXENDPOINT_PRIVCONTEXT          currUcxEndpointContext;
    PUCXENDPOINT_PRIVCONTEXT          nextUcxEndpointContext;
    PUSBDI_HANDLE_DATA                currUsbdiHandleData;

    UNREFERENCED_PARAMETER(Hub);

    TRY {
        
        usbDeviceContext = UcxUsbDeviceGetContext(UsbDevice);
        ucxControllerContext = usbDeviceContext->UcxControllerContext;
        
        KeAcquireSpinLock(&usbDeviceContext->USBDInterfaceListLock, &irql);
        NT_ASSERTMSG("Client Did not Unregister, Memory Leak will occur", 
                     IsListEmpty(&usbDeviceContext->USBDInterfaceListHead));

        if (IsListEmpty(&usbDeviceContext->USBDInterfaceListHead) == FALSE) {

            FOR_ALL_IN_LIST(USBDI_HANDLE_DATA,
                            &usbDeviceContext->USBDInterfaceListHead,
                            USBDInterfaceListEntry,
                            currUsbdiHandleData) {
                if (ucxControllerContext->DriverVerifierEnabled
                    || currUsbdiHandleData->UsbVerifierEnabled) {
                    Usb3_KeBugCheckEx(USB3_VERIFIER_BUGCODE_CLIENT_LEAKED_USBD_HANDLE,
                                      currUsbdiHandleData,
                                      currUsbdiHandleData->DeviceObject,
                                      0);
                }
                
                //
                // Release the reference on the Device object. 
                // Ideally this reference should be dropped when the client driver
                // unregisters. If verifier is enabled, we would have bugchecked by now. 
                //
                ObDereferenceObjectWithTag(currUsbdiHandleData->DeviceObject, 
                                           TAG_ULONG(RootHub_USBDInterfaceV1Unregister));

            }
        }

        KeReleaseSpinLock(&usbDeviceContext->USBDInterfaceListLock, irql);

        usbDeviceContext->PendingDelete = TRUE;

        KeAcquireSpinLock(&ucxControllerContext->TopologyLock, &irql);
        
        TRANSFER_LIST(&staleEndpointListHead, &usbDeviceContext->StaleEndpointListHead);
        
        KeReleaseSpinLock(&ucxControllerContext->TopologyLock, irql);

        //
        // For each stale endpoint, add the DeleteStale to its state machine and forget about it;
        // DeleteStale will cause it to tear down. (If there are any non-stale enpdoints, they will
        // now skip the stale state due to PendingDelete == TRUE on their way to teardown.)
        //
        FOR_ALL_IN_LIST_SAFE(UCXENDPOINT_PRIVCONTEXT, 
                             &staleEndpointListHead,
                             EndpointListEntry,
                             currUcxEndpointContext,
                             nextUcxEndpointContext) {
            
            RemoveEntryList(&currUcxEndpointContext->EndpointListEntry);            
            InitializeListEntry(&currUcxEndpointContext->EndpointListEntry);
            Endpoint_EventAdd(currUcxEndpointContext, UcxEndpointEventDeleteStale);

        }

        Etw_UcxDeviceDelete(NULL, usbDeviceContext);

        WdfObjectDelete(UsbDevice);

    } FINALLY {

    }
}

VOID
UsbDevice_EvtDestroyCallback(
    __in WDFOBJECT UcxUsbDevice
    )
/*++

Routine Description:

    This is an Event Callback function called when the device object's reference count
    This function may be called if there is an init failure, with a zeored
    context. (This is due to the way Ucx_WdfObjectCreateWith2Attributes works.)
    
Arguments:

    UcxEndpoint - Handle to the endpoint object

--*/
{
    PUCXUSBDEVICE_PRIVCONTEXT         usbDeviceContext;
    PUCXCONTROLLER_PRIVCONTEXT        ucxControllerContext;
    KIRQL                             irql;

    TRY {

        usbDeviceContext = UcxUsbDeviceGetContext(UcxUsbDevice);

        if (usbDeviceContext->UcxUsbDevice == NULL) {

            //
            // We are being called on init failure, context is all zeroes
            //
            LEAVE;
        }

        ucxControllerContext = usbDeviceContext->UcxControllerContext;

        //
        // Tell the parent device this child is gone, removing this device from the tree
        //
        KeAcquireSpinLock(&ucxControllerContext->TopologyLock, &irql);
        
        if (FALSE == IsListEntryNULL(&usbDeviceContext->ChildListEntry)) {
            RemoveEntryList(&usbDeviceContext->ChildListEntry);
            InitializeListEntry(&usbDeviceContext->ChildListEntry);
            ucxControllerContext->ChildListCount--;
        }

        KeReleaseSpinLock(&ucxControllerContext->TopologyLock, irql);

        //
        // Release reference on parent device
        //
        WdfObjectDereferenceWithTag(usbDeviceContext->ParentHub, TAG(EXPORT(UcxUsbDeviceCreate)));

    } FINALLY {

    }
}

VOID
UsbDevice_EnableInvalidHandleTracking(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT         UcxUsbDeviceContext
)
/*++

Routine Description:

    This routine is called to enable strict validation for a device. 
    
    As a part of the strict validation we would do the following: 
    
    * Maintain a Locked list of all endpoints for a device. This list is protected by
      a lock at the UcxUsbDevice level (rather than the Topology Lock that is maintained at the
      UcxController level)
    * Whenever IO is recieved on a pipe, this list is verified to ensure that IO is recieved
      on a valid handle.

Arguments:

    UsbDeviceContext - A pointer to the ucx context for the device. 
    
Comments:
    
    It is not expected to recieve this request after the hub driver has created a bunch of 
    endpoints for this device. 
    
    The only endpoint that can be created is the default endpoint.
--*/
{
    PUCXENDPOINT_PRIVCONTEXT          currUcxEndpointContext;
    KIRQL                             irql1;
    KIRQL                             irql2;

    TRY {
        
        //
        // !! IMPORTANT !!
        // Must acquire locks in this order everywhere (every place that needs both)
        //
        KeAcquireSpinLock(&UcxUsbDeviceContext->UcxControllerContext->TopologyLock, &irql1);
        KeAcquireSpinLock(&UcxUsbDeviceContext->InvalidHandleTrakingLock, &irql2);

        if (UcxUsbDeviceContext->AllowIoOnInvalidPipeHandles) {

            //
            // Tracking has already been enabled, don't duplicate work. Tracking cannot be disabled.
            //
            LEAVE;
        }

        //
        // Create a new list of endpoints to be protected by this device lock rather than the
        // controller-wide topology lock. Because we also set AllowIoOnInvalidPipeHandles, future
        // updates to EndpointListHead will also explicitly update
        // EndpointListHeadForInvalidHandleTracking.
        //
        NT_ASSERT(IsListEmpty(&UcxUsbDeviceContext->EndpointListHeadForInvalidHandleTracking));

        FOR_ALL_IN_LIST(UCXENDPOINT_PRIVCONTEXT, 
                        &UcxUsbDeviceContext->EndpointListHead,
                        EndpointListEntry,
                        currUcxEndpointContext) {

            InsertTailList(&UcxUsbDeviceContext->EndpointListHeadForInvalidHandleTracking,
                           &currUcxEndpointContext->EndpointListEntryForInvalidHandleTraking);
        }

        UcxUsbDeviceContext->AllowIoOnInvalidPipeHandles = TRUE;

    } FINALLY {

        KeReleaseSpinLock(&UcxUsbDeviceContext->InvalidHandleTrakingLock, irql2);
        KeReleaseSpinLock(&UcxUsbDeviceContext->UcxControllerContext->TopologyLock, irql1);
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_Disconnect(
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        ULONG              PortNumber
)
/*++

Routine Description:

    This Query interface function is called by the hub driver to report a UsbDevice has been
    disconnected. 

    UCX may also call this function internally if the HCD says the device and its endpoints are
    going away.

    The primary purpose of this call is to tell UCX to Purge all IO for the deleted device
    and all its children (if any). It does not purge root hub IO.

Arguments:

    Hub - The handle to the UCXUSBDEVICE representing the hub that called this 
        function
        
    PortNumber - The port number to which the disconnected device was connected. As always, the 
       value 0 means that the hub was disconnected
    
Comments:
    

--*/
{
    PUCXUSBDEVICE_PRIVCONTEXT         usbDeviceContextForHub;
    PUCXUSBDEVICE_PRIVCONTEXT         currUcxUsbDeviceContext;
    PUCXCONTROLLER_PRIVCONTEXT        ucxControllerContext;
    LIST_ENTRY                        endpointListHead;
    PUCXENDPOINT_PRIVCONTEXT          currUcxEndpointContext;
    PUCXENDPOINT_PRIVCONTEXT          nextUcxEndpointContext;
    PUSB_DEVICE_PORT_PATH             portPath;
    KIRQL                             irql;

    TRY {
        
        usbDeviceContextForHub = UcxUsbDeviceGetContext(Hub);

        ucxControllerContext = usbDeviceContextForHub->UcxControllerContext;

        InitializeListHead(&endpointListHead);

        KeAcquireSpinLock(&ucxControllerContext->TopologyLock, &irql);
        
        if (PortNumber == 0) {
            //
            // PortNumber value of 0 means that the hub itself has been disconnected
            //
            
            //
            // Walk the tree of all devices downstream of this disconnected device
            // (including the disconnected device), and build a list of endpoints
            // to be Purged.
            //
            UsbDevice_WalkTree(usbDeviceContextForHub,
                               FALSE,
                               UsbDevice_DisconnectCallback,
                               &endpointListHead);

        } else {

            //
            // Loop through all the childern of the Hub looking for the ones on the
            // requested port number. Note that there could be multiple of them 
            // for the same port number since hub starts to enumerate a new device
            // on a port before the previous one is totally destroyed. However only
            // one child would really require the Disconnect Call, remaining would 
            // already be in the disconnected state. 
            //
            FOR_ALL_IN_LIST(UCXUSBDEVICE_PRIVCONTEXT, 
                            &usbDeviceContextForHub->ChildListHead,
                            ChildListEntry,
                            currUcxUsbDeviceContext) {


                //
                // Find the port number of the current device, and compare with the port
                // number of the disconnected device
                //
                portPath = &currUcxUsbDeviceContext->UcxUsbDeviceInfo.PortPath;
                if (portPath->PortPath[portPath->PortPathDepth - 1] != PortNumber) {
                    continue;
                }


                //
                // Walk the tree of all devices downstream of this disconnected device
                // (including the disconnected device), and build a list of endpoints
                // to be Purged.
                //
                UsbDevice_WalkTree(currUcxUsbDeviceContext,
                                   FALSE,
                                   UsbDevice_DisconnectCallback,
                                   &endpointListHead);

            }

        }

        KeReleaseSpinLock(&ucxControllerContext->TopologyLock, irql);

        // For each endpoint in the temporary list, remove it from the list
        // call EndpointPurgeOnDisconnect, reference will be released when the 
        // EndpointPurgeOnDisconnect operation competes asynchronously

        FOR_ALL_IN_LIST_SAFE(UCXENDPOINT_PRIVCONTEXT, 
                             &endpointListHead,
                             UsbDeviceDisconnectListEntryTemp,
                             currUcxEndpointContext,
                             nextUcxEndpointContext) {

            RemoveEntryList(&currUcxEndpointContext->UsbDeviceDisconnectListEntryTemp);

            InitializeListEntry(&currUcxEndpointContext->UsbDeviceDisconnectListEntryTemp);

            //
            // The hub driver cannot disconnect the root hub, but UCX can. If we are disconnecting
            // the root hub, there are statemachine-less root hub endpoints on endpointListHead;
            // pass over them as they need to be purged outside this function.
            //
            if (currUcxEndpointContext->SmEngineContextInitialized) {

                Endpoint_EventAdd(currUcxEndpointContext, UcxEndpointEventDisconnectFromHub);
            }

            WdfObjectDereferenceWithTag(currUcxEndpointContext->UcxEndpoint,
                                        TAG(UsbDevice_DisconnectCallback));
        }
            
        
    } FINALLY {

    }
    return;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
UsbDevice_SetPdoInformation(
    __in 
        UCXUSBDEVICE       Hub,
    __in 
        UCXUSBDEVICE       UsbDevice,
    __in
        PDEVICE_OBJECT     Pdo
)
/*++

Routine Description:

    This Query interface function is called by the hub driver to report
    pdo information for UsbDevice. 

    The purpose of this call is only to cache the Device Object for diagnostic purposes. 

Arguments:

    Hub - The handle to the UCXUSBDEVICE representing the hub that called this 
        function
        
    UsbDevice - A pointer to usbdevice that is disconnected

    Pdo - The PDEVICE_OBJECT for that device.
        
Comments:
    

--*/
{
    PUCXUSBDEVICE_PRIVCONTEXT         usbDeviceContext;

    UNREFERENCED_PARAMETER(Hub);

    TRY {
        
        usbDeviceContext = UcxUsbDeviceGetContext(UsbDevice);

        NT_ASSERT(usbDeviceContext->Pdo == NULL);
        usbDeviceContext->Pdo = Pdo;
        
    } FINALLY {

    }
}

VOID
EXPORT(UcxUsbDeviceInitSetEventCallbacks) (
    __in_opt 
        PUCX_GLOBALS                            Globals,
    __inout
        PUCXUSBDEVICE_INIT                      UsbDeviceInit,
    __in
        PUCX_USBDEVICE_EVENT_CALLBACKS          EventCallbacks
)
/*++

Routine Description:

    This export function sets UsbDevice Event Callbacks in an UsbDeviceInit structure 
    
Arguments:

    Globals - Driver globals of the controller driver. This information is not
        being used. 
    
    UsbDeviceInit - A pointer to the UCXUSBDEVICE_INIT structure. 
    
    EventCallbacks - A pointer to a structure that has the usbdevice event 
        callbacks.

Return Value:

    VOID    
--*/
{
    UNREFERENCED_PARAMETER(Globals);

    TRY {

        //
        // During TH2, in order to support Device Suspend and Resume notifications to
        // UCX clients, a new UCX_USBDEVICE_EVENT_CALLBACKS_RESERVED structure is introduced 
        // that extends the current UCX_USBDEVICE_EVENT_CALLBACKS structure. Currently UCX
        // does not generate version specific header files based on its major and minor versions
        // and adding new fields to this structure without changing the version number would
        // hinder our ability to move later to StubGen and such tools to generate version specific 
        // header files. When such a move is made, _RESERVED structure should be deprecated in 
        // favor of incrementing the minor version and having separate version specific header files.
        //
        
        //
        // Validate the size of the structure. For any future minor versions, 
        // this check should include checks for all previous sizes. 
        //
        if ((EventCallbacks->Size != sizeof(UCX_USBDEVICE_EVENT_CALLBACKS)) &&
            (EventCallbacks->Size != sizeof(UCX_USBDEVICE_EVENT_CALLBACKS_RESERVED))) {
                
            NT_ASSERTMSG("BUGCHECK: Invalid size", FALSE);
        }

        RtlCopyMemory(&UsbDeviceInit->EvtCallbacks,
                      EventCallbacks,
                      EventCallbacks->Size);
        
    } FINALLY {

    }
}


_Must_inspect_result_
__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
EXPORT(UcxUsbDeviceCreate)(
    __in_opt 
        PUCX_GLOBALS               Globals,
    __in 
        UCXCONTROLLER              UcxController,
    __deref_inout_opt 
        PUCXUSBDEVICE_INIT        *UsbDeviceInit,    
    __in_opt 
        PWDF_OBJECT_ATTRIBUTES     Attributes,
    __out 
        UCXUSBDEVICE              *UsbDevice
    )
/*++

Routine Description:

    This export function creates an usbdevice object 
    
Arguments:

    Globals - Driver globals of the controller driver. This information is not
        being used. 
    
    UcxController - The handle to the controller for which the usbdevice is being
        created
    
    UsbDeviceInit - Pointer to an init structure describing various
        information for the usbdevice creation.
    
    Attributes - Pointer to the object attributes structure 
    
    UsbDevice - Output - Pointer to a location that will recieve a handle to the 
        usbdevice object

Return Value:

    NT_SUCCESS status if a endpoint object is  successfully created
    
--*/
{
    NTSTATUS                          status;
    UCXUSBDEVICE                      ucxUsbDevice = NULL;
    PUCXUSBDEVICE_PRIVCONTEXT         ucxUsbDeviceContext;
    PUCXUSBDEVICE_PRIVCONTEXT         parentUcxUsbDeviceContext;
    PUCXCONTROLLER_PRIVCONTEXT        ucxControllerContext;
    WDF_OBJECT_ATTRIBUTES             objectAttributes;
    PWDF_OBJECT_ATTRIBUTES            objectAttributes1;
    PWDF_OBJECT_ATTRIBUTES            objectAttributes2;
    KIRQL                             irql;

    UNREFERENCED_PARAMETER(Globals);

    TRY {
        //
        // Parameter Validation
        //

        //NT_ASSERT(!"Add the necessary Parameter Validation");
        
        //
        // Create ucxUsbDevice object, using the Attributes given to us by 
        // the controller driver to us. If the controller driver didnt give us
        // the attributes, use your own attributes.
        //
        
        *UsbDevice = NULL;
        parentUcxUsbDeviceContext = UcxUsbDeviceGetContext((*UsbDeviceInit)->ParentHub);
        ucxControllerContext = UcxControllerGetContext(UcxController);

        WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&objectAttributes,
                                                UCXUSBDEVICE_PRIVCONTEXT);

        objectAttributes.EvtDestroyCallback = UsbDevice_EvtDestroyCallback;

        if (Attributes != WDF_NO_OBJECT_ATTRIBUTES) {
            
            //
            // UCXUSBDEVICE is to be parented to the Parent Hub UCXUSBDEVICE
            //
            Attributes->ParentObject = (*UsbDeviceInit)->ParentHub;
            
            objectAttributes1 = Attributes;
            objectAttributes2 = &objectAttributes;
        
        } else {

            //
            // Controller Driver did not provide an attributes structure, 
            // use our own.
            //
            
            objectAttributes.ParentObject = (*UsbDeviceInit)->ParentHub;
            
            objectAttributes1 = &objectAttributes;
            objectAttributes2 = WDF_NO_OBJECT_ATTRIBUTES;

        }

        status = Ucx_WdfObjectCreateWith2Attributes(objectAttributes1, 
                                                    objectAttributes2,
                                                    &ucxUsbDevice);
        
        if (!NT_SUCCESS(status)) { 
            LogError(FlagUsbDevice, "WdfObjectCreate Failed %!STATUS!", status);
            LEAVE;
        }

        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);

        //
        // Initialize and populate UcxUsbDeviceContext
        //
        ucxUsbDeviceContext->Sig = SigUcxUsbDeviceContext;
        ucxUsbDeviceContext->UcxUsbDevice = ucxUsbDevice;
        ucxUsbDeviceContext->UcxControllerContext = ucxControllerContext;
        ucxUsbDeviceContext->ParentHub = (*UsbDeviceInit)->ParentHub;
        ucxUsbDeviceContext->ContextFromHub = (*UsbDeviceInit)->ContextFromHub;
        
        //
        // Hold a reference to parent device since the device tree must be preserved past
        // WdfObjectDelete up until destroy. This is in order to allow deleted endpoints to
        // receive controller-global events to finish their teardown.
        //
        WdfObjectReferenceWithTag(ucxUsbDeviceContext->ParentHub, TAG(EXPORT(UcxUsbDeviceCreate)));

        ucxUsbDeviceContext->Disconnected = FALSE;
        ucxUsbDeviceContext->DeprogrammedDueToControllerReset = FALSE;
        ucxUsbDeviceContext->AllowChildrenToExitTreePurge = TRUE;
        ucxUsbDeviceContext->AllowEndpointsToExitTreePurge = TRUE;

        RtlCopyMemory(&ucxUsbDeviceContext->UcxUsbDeviceInfo,
                      &(*UsbDeviceInit)->UcxUsbDeviceInfo,
                      sizeof(ucxUsbDeviceContext->UcxUsbDeviceInfo));

        if (((*UsbDeviceInit)->EvtCallbacks.Size) == sizeof(UCX_USBDEVICE_EVENT_CALLBACKS_RESERVED)) {
            
            RtlCopyMemory(&ucxUsbDeviceContext->EvtCallbacks, 
                          &(*UsbDeviceInit)->EvtCallbacks,
                          sizeof(UCX_USBDEVICE_EVENT_CALLBACKS_RESERVED));
                          
        } else {

            RtlCopyMemory(&ucxUsbDeviceContext->EvtCallbacks, 
                          &(*UsbDeviceInit)->EvtCallbacks,
                          sizeof(UCX_USBDEVICE_EVENT_CALLBACKS));
                          
            ucxUsbDeviceContext->EvtCallbacks.EvtUsbDeviceSuspend = NULL;
            ucxUsbDeviceContext->EvtCallbacks.EvtUsbDeviceResume = NULL;
        }

        RtlCopyMemory(&ucxUsbDeviceContext->EvtCallbacks, 
                      &(*UsbDeviceInit)->EvtCallbacks,
                      sizeof(UCX_USBDEVICE_EVENT_CALLBACKS));
        
        InitializeListHead(&ucxUsbDeviceContext->EndpointListHead);
        InitializeListHead(&ucxUsbDeviceContext->StaleEndpointListHead);
        InitializeListHead(&ucxUsbDeviceContext->ChildListHead);
        InitializeListEntry(&ucxUsbDeviceContext->ChildListEntry);
        
        KeInitializeSpinLock(&ucxUsbDeviceContext->RemoteWakeLock);
        
        KeInitializeSpinLock(&ucxUsbDeviceContext->USBDInterfaceListLock);
        InitializeListHead(&ucxUsbDeviceContext->USBDInterfaceListHead);

        KeInitializeSpinLock(&ucxUsbDeviceContext->InvalidHandleTrakingLock);
        InitializeListHead(&ucxUsbDeviceContext->EndpointListHeadForInvalidHandleTracking);

        //
        // UcxDevice needs to track all its child devices, so add this into that 
        // list holding a global lock.
        // Note: We could have used a Per UcxUsbDevice Lock here, however it 
        // makes implementation of UsbDeviceDisconnect routine complicated.
        // Since this is not the hot path, using a simpler solution
        //
        KeAcquireSpinLock(&ucxControllerContext->TopologyLock, &irql);

        if (parentUcxUsbDeviceContext->Disconnected) {

            //
            // If the parent device has been disconnected, there is no use to create this child.
            //
            status = STATUS_NO_SUCH_DEVICE;
            LogInfo(FlagUsbDevice, "Failing device create since parent 0x%p is disconnected %!STATUS!",
                    parentUcxUsbDeviceContext->UcxUsbDevice, status);

        } else {

            InsertTailList(&parentUcxUsbDeviceContext->ChildListHead, 
                           &ucxUsbDeviceContext->ChildListEntry); 

            ucxControllerContext->ChildListCount++;

            //
            // Propagate any ongoing tree purge from the parent.
            //
            ucxUsbDeviceContext->AllowChildrenToExitTreePurge =
                parentUcxUsbDeviceContext->AllowChildrenToExitTreePurge;
            ucxUsbDeviceContext->AllowEndpointsToExitTreePurge =
                parentUcxUsbDeviceContext->AllowChildrenToExitTreePurge;
        }

        KeReleaseSpinLock(&ucxControllerContext->TopologyLock, irql);

        if (!NT_SUCCESS(status)) {

            LEAVE;
        }

        LogInfo(FlagUsbDevice, "UcxUsbDevice 0x%p, UcxUsbDeviceContext 0x%p", 
                ucxUsbDevice, ucxUsbDeviceContext);


    } FINALLY {

        if (!NT_SUCCESS(status)) {
            
            if (ucxUsbDevice != NULL) {
                WdfObjectDelete(ucxUsbDevice);
            }

        } else {
            *UsbDevice = ucxUsbDevice;
            (*UsbDeviceInit)->UcxUsbDevice = ucxUsbDevice;
            *UsbDeviceInit = NULL;
        }
        
        LogVerbose(FlagUsbDevice, "UcxUsbDeviceCreate 0x%x", status);

    }

    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
EXPORT(UcxUsbDeviceRemoteWakeNotification)(
    __in_opt 
        PUCX_GLOBALS               Globals,
    __in 
        UCXUSBDEVICE               UsbDevice,
    __in
        ULONG                      Interface
    )
/*++

Routine Description:

    This export function is called by the HCD when it recieves a Remote wake signal from a 
    usbdevice 
    
Arguments:

    Globals - Driver globals of the controller driver. This information is not
        being used. 
    
    UsbDevice - The handle to the usbdevice for which the remote wake is recieved.
    
    Interface - The interface number that sent the remote wake notification.

Return Value:

Comments:
    This function completes a matching Remote Wake Request that is pending from the hub driver / 
    ccgp driver. If no such request is found, this notifiation is ignored.
    
--*/
{
    KIRQL                             irql;
    PUCXFUNCTION_INFO                 ucxFunctionInfo;
    PUCXUSBDEVICE_PRIVCONTEXT         ucxUsbDeviceContext;
    WDFREQUEST                        request;
    BOOLEAN                           locked =  FALSE;
    BOOLEAN                           found;
    PREMOTE_WAKE_REQUEST_INFO         remoteWakeRequestInfo;
    ULONG                             index;
    NTSTATUS                          status;
    
    UNREFERENCED_PARAMETER(Globals);

    TRY {
        
        ucxUsbDeviceContext = UcxUsbDeviceGetContext(UsbDevice);
        
        KeAcquireSpinLock(&ucxUsbDeviceContext->RemoteWakeLock, &irql);
        locked = TRUE;
        
        if (NULL == ucxUsbDeviceContext->UcxFunctionInfoWdfMemory) {
        
            //
            // The device is probably not a multifunction device. 
            //
            
            remoteWakeRequestInfo = &ucxUsbDeviceContext->UsbDeviceRemoteWakeRequestInfo;
            
            if (NULL == remoteWakeRequestInfo->PendingRequest) {
                LogWarning(FlagUsbDevice, 
                           "Remote wake from HCD, but no Request, Device Context 0x%p", 
                           ucxUsbDeviceContext);
                LEAVE;
            }
            
            if (remoteWakeRequestInfo->RequestParams->Interface != Interface) {

                LogError(FlagUsbDevice,
                         "Remote wake from HCD, but no relevant Request, Device Context 0x%p, Interface Waiting for Remote wake %d, Interface that has got wake %d",
                         ucxUsbDeviceContext, remoteWakeRequestInfo->RequestParams->Interface, Interface);
                LEAVE;
            }

        } else {

            //
            // The device is a multifunction device. 
            // We need to walk the array of all functions, and see for what function
            // have we recieved a remote wake notification for
            //
            found = FALSE;
            remoteWakeRequestInfo = NULL;
            for (index = 0; index < ucxUsbDeviceContext->NumberOfFunctions; index++) {
                ucxFunctionInfo = &ucxUsbDeviceContext->UcxFunctionInfo[index];
                remoteWakeRequestInfo = &ucxFunctionInfo->FunctionRemoteWakeRequestInfo;

                if (NULL == remoteWakeRequestInfo->PendingRequest) {
                    continue;
                }

                //
                // Walk through the list of all the interfaces that can wake up. 
                //
                if (remoteWakeRequestInfo->RequestParams->Interface == Interface) {
                    
                    found = TRUE;
                    break;
                }
            }

            if (FALSE == found) {
                LogWarning(FlagUsbDevice, 
                           "Remote wake from HCD, but no Request for interface %d, Device Context0x%p",
                           Interface, ucxUsbDeviceContext);
                LEAVE;
            }
            
        }
        
        status = WdfRequestUnmarkCancelable(remoteWakeRequestInfo->PendingRequest);

        if (!NT_SUCCESS(status)) {
            //
            // Let the cancel routine run and handle this.
            //
            LogInfo(FlagUsbDevice, 
                    "For UcxUsbDeviceContext 0x%p, Request 0x%p could not be Unmarked Cancellable,Status %!STATUS!",
                    ucxUsbDeviceContext, remoteWakeRequestInfo->PendingRequest, status);
            LEAVE;
        }

        //
        // We have found the request we are interested in. 
        // Pull it out of the deviceContext and release the lock.
        //
        request = remoteWakeRequestInfo->PendingRequest;

        remoteWakeRequestInfo->PendingRequest = NULL;
        remoteWakeRequestInfo->RequestParams = NULL;

        KeReleaseSpinLock(&ucxUsbDeviceContext->RemoteWakeLock, irql);
        locked = FALSE;

        WdfRequestComplete(request, STATUS_SUCCESS);

    } FINALLY {
        if (locked) {
            KeReleaseSpinLock(&ucxUsbDeviceContext->RemoteWakeLock, irql);
        }

    }
}


__drv_mustHold(SpinLock)
__drv_at(UcxUsbDeviceContext->Disconnected, __range(==,FALSE))
VOID
UsbDevice_DisconnectCallback(
    __in
      PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in
      PLIST_ENTRY                   EndpointList
    ) 
/*++

Routine Description:

    When the hub driver reports a disconnect on a child device/hub, this routine is called for every 
    device that has been disconnected. 
    
    (As per current implementation, this routine is called maximum once for a particular device)
    
    In this routine, we walk the list of all endpoints of the deivce, reference them and add them to 
    a list.
Arguments:

    UcxUsbDeviceContext - Context of the usbdevice that has been disconnected. 
    
    EndpointList - List head of the list to which all the endpoints must be added. 

Return Value:

    VOID

Comments:
    
--*/
{
    PUCXENDPOINT_PRIVCONTEXT currUcxEndpointContext;

    TRY {
        
        NT_ASSERT(FALSE == UcxUsbDeviceContext->Disconnected);

        UcxUsbDeviceContext->Disconnected = TRUE;
        
        FOR_ALL_IN_LIST(UCXENDPOINT_PRIVCONTEXT, 
                        &UcxUsbDeviceContext->EndpointListHead,
                        EndpointListEntry,
                        currUcxEndpointContext) {
            
            NT_ASSERT(IsListEntryNULL(&currUcxEndpointContext->UsbDeviceDisconnectListEntryTemp));
            
            //
            // Take a reference since we are going to walk this list of
            // endpoints outside the TopologyLock
            //

            WdfObjectReferenceWithTag(currUcxEndpointContext->UcxEndpoint,
                                      TAG(UsbDevice_DisconnectCallback));

            //
            // NOTE: 
            // To prevent a kernel stack overflow, it is important to call Purge for the child
            // before calling purge for the parent. 
            // As per implementation of UsbDevice_WalkTree, the callback is first called for
            // a parent node then for its children.  
            // However we want to build a list with children first then the parent. 
            // Thus it is important to use InsertHeadList here and not InsertTailList. 
            // For more information on the kernel stack overflow issue, please refer to the 
            // comment in UcxControllerSetFailed routine.
            //
            InsertHeadList(EndpointList, 
                           &currUcxEndpointContext->UsbDeviceDisconnectListEntryTemp);
            
        }
        
    } FINALLY {

    }
    return;
}

__drv_mustHold(SpinLock)
VOID
UsbDevice_PrepareForControllerResetCallback(
    __in
      PUCXUSBDEVICE_PRIVCONTEXT                     UcxUsbDeviceContext,
    __in
      PUSBDEVICE_CONTROLLER_RESET_CALLBACK_CONTEXT  ControllerResetCallbackContext
    ) 
/*++

Routine Description:

Arguments:

    UcxUsbDeviceContext - Context of the usbdevice 
    
    EndpointList - List head of the list to which all the endpoints must be added. 

Return Value:

    VOID

Comments:
    
--*/
{
    PUCXENDPOINT_PRIVCONTEXT currUcxEndpointContext;

    TRY {
        
        //
        // Skip for RootHub
        //
        if (IsTypeRootHub(UcxUsbDeviceContext->UsbDeviceType)) {
            LEAVE;
        }

        //
        // Record that the device has been de-programmed. 
        //
        LogInfo(FlagUsbDevice, "Setting DeprogrammedDueToControllerReset for UcxDevice 0x%p, Context 0x%p",
                UcxUsbDeviceContext->UcxUsbDevice, UcxUsbDeviceContext);
        UcxUsbDeviceContext->DeprogrammedDueToControllerReset = TRUE;
        
        FOR_ALL_IN_LIST(UCXENDPOINT_PRIVCONTEXT, 
                        &UcxUsbDeviceContext->EndpointListHead,
                        EndpointListEntry,
                        currUcxEndpointContext) {
            
            NT_ASSERT(IsListEntryNULL(&currUcxEndpointContext->ControllerResetListEntryTemp));
            
            //
            // Take a reference since we are going to walk this list of
            // endpoints outside the TopologyLock
            //

            WdfObjectReferenceWithTag(currUcxEndpointContext->UcxEndpoint,
                                      TAG(UsbDevice_PrepareForControllerResetCallback));

            InsertTailList(&ControllerResetCallbackContext->EndpointList, 
                           &currUcxEndpointContext->ControllerResetListEntryTemp);
            ControllerResetCallbackContext->EndpointCount++;
            
        }
        
    } FINALLY {

    }
    return;
}

__drv_mustHold(SpinLock)
VOID
UsbDevice_NotifyControllerResetCompleteCallback(
    __in
      PUCXUSBDEVICE_PRIVCONTEXT                     UcxUsbDeviceContext,
    __in
      PUSBDEVICE_CONTROLLER_RESET_CALLBACK_CONTEXT  ControllerResetCallbackContext
    ) 
/*++

Routine Description:

Arguments:

    UcxUsbDeviceContext - Context of the usbdevice 
    
    EndpointList - List head of the list to which all the endpoints must be added. 

Return Value:

    VOID

Comments:
    
--*/
{
    PUCXENDPOINT_PRIVCONTEXT currUcxEndpointContext;

    TRY {
        
        //
        // Skip for RootHub
        //
        if (IsTypeRootHub(UcxUsbDeviceContext->UsbDeviceType)) {
            LEAVE;
        }

        FOR_ALL_IN_LIST(UCXENDPOINT_PRIVCONTEXT, 
                        &UcxUsbDeviceContext->EndpointListHead,
                        EndpointListEntry,
                        currUcxEndpointContext) {
            
            NT_ASSERT(IsListEntryNULL(&currUcxEndpointContext->ControllerResetListEntryTemp));
            
            //
            // Take a reference since we are going to walk this list of
            // endpoints outside the TopologyLock
            //

            WdfObjectReferenceWithTag(currUcxEndpointContext->UcxEndpoint,
                                      TAG(UsbDevice_NotifyControllerResetCompleteCallback));

            InsertTailList(&ControllerResetCallbackContext->EndpointList, 
                           &currUcxEndpointContext->ControllerResetListEntryTemp);
            ControllerResetCallbackContext->EndpointCount++;
            
        }
        
    } FINALLY {

    }
    return;
}

#if 0
__drv_mustHold(SpinLock)
VOID
UsbDevice_SetDeprogrammed(
    __in
      PUCXUSBDEVICE_PRIVCONTEXT                     UcxUsbDeviceContext,
    __in_opt
      PVOID                                         UnusedContext
    ) 
/*++

Routine Description:

    This routine is called to set the information that a device has been deprogrammed from the
    controller.

Arguments:

    UcxUsbDeviceContext - Context of the usbdevice 
    
Return Value:

    VOID

Comments:
    
--*/
{
    UNREFERENCED_PARAMETER(UnusedContext);

    TRY {
        
        //
        // Skip for RootHub
        //
        if (IsTypeRootHub(UcxUsbDeviceContext->UsbDeviceType)) {
            LEAVE;
        }

        //
        // Record that the device has been de-programmed. 
        //
        LogInfo(FlagUsbDevice, "Setting Deprogrammed for UcxDevice 0x%p, Context 0x%p",
                UcxUsbDeviceContext->UcxUsbDevice, UcxUsbDeviceContext);
        UcxUsbDeviceContext->DeprogrammedDueToControllerReset = TRUE;
        
    } FINALLY {

    }
    return;
}
#endif

__drv_mustHold(SpinLock)
VOID
UsbDevice_WalkTree(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT               UcxUsbDeviceContext,
    __in
        BOOLEAN                                 WalkDisconnectedTree,
    __in 
        PFN_USBDEVICE_WALKTREE_CALLBACK         Callback,
    __in_opt 
        PVOID                                   Context
    )
/*++

Routine Description:

    This routine calls the Input Callback routine for the the passed in UsbDevice. It also calls
    recursively does the same for all its children. 
    
    This results in a pre-order depth-first traversal.

Arguments:

    UcxUsbDeviceContext - Context of the usbdevice for which the Callback routine must be called. 
    
    Callback - A callback routine that must be called for this UcxUsbDeviceContext and all its 
        children.
        
    WalkDisconnectedTree - BOOLEAN indicating whether a disconnected tree must be walked or not.
    
    Context - Context to be passed in the Callback routine. 
    
Return Value:

    VOID

--*/
{

    PUCXUSBDEVICE_PRIVCONTEXT currUcxUsbDeviceContext;

    TRY {

        if ((WalkDisconnectedTree == FALSE) && UcxUsbDeviceContext->Disconnected){
            LEAVE;
        }

        //
        // Call the callback for current node.
        //
        (*Callback)(UcxUsbDeviceContext, Context);
        
        //
        // Walk through the list of children of the device, and for each child
        // recursively call this routine.
        //        
        FOR_ALL_IN_LIST(UCXUSBDEVICE_PRIVCONTEXT, 
                        &UcxUsbDeviceContext->ChildListHead,
                        ChildListEntry,
                        currUcxUsbDeviceContext) {

            UsbDevice_WalkTree(currUcxUsbDeviceContext, 
                               WalkDisconnectedTree, 
                               Callback, 
                               Context);

        }

    } FINALLY {

    }

    return;
}
     
VOID
UsbDevice_PendingOperationCompleteByEndpoint(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT UcxUsbDeviceContext
    )
/*++

Routine Description:

    This routine is called by the endpoint state machine when it complets 
    a pending operation. The pending operation could be one of many: 
        Purge, Start, PrepareDisableFromHUb, UsbDeviceResetFromHub, EnableCompletion etc. 
        
    This routine evaluates if it is the last endpoint to complete the 
    pending operation. If yes, it calls the PendingOperationCompletion routine.

Arguments:

    UcxUsbDeviceContext - Context of the usbdevice 

--*/
{

    PFN_UCXUSBDEVICE_PENDING_OPERATION_COMPLETION  PendingOperationCompletion;
    TRY {

        if (0 == InterlockedDecrement(&UcxUsbDeviceContext->PendingOperations)) {

            PendingOperationCompletion = UcxUsbDeviceContext->PendingOperationCompletion;
            UcxUsbDeviceContext->PendingOperationCompletion = NULL;

            LogVerbose(FlagUsbDevice, "Completing Pending Operation for UcxUsbDevice 0x%p", 
                       UcxUsbDeviceContext->UcxUsbDevice );

            (*PendingOperationCompletion)(UcxUsbDeviceContext);

        }

    } FINALLY {

    }
    return;

}

VOID
UsbDevice_CallAddEventForAllEndpointsWithPendingOperationSet(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in 
        SM_ENGINE_EVENT               Event
)
/*++

Routine Description:

    This routine schedule asynchronous purge / start operation for each endpoint of a UsbDevice
        
Arguments:
    
    UcxUsbDeviceContext - Context of the usbdevice
    
    Event - Event to be scheduled for each endpoint of the usbdevice. 
    
Return Value:

    VOID
    
--*/
{
    PUCXENDPOINT_PRIVCONTEXT          currUcxEndpointContext;
    PUCXENDPOINT_PRIVCONTEXT          nextUcxEndpointContext;
    ULONG                             numberOfEndpoints;
    LIST_ENTRY                        endpointListHead;
    KIRQL                             irql;
    //PVOID                             tag; 

    TRY {

        numberOfEndpoints = 0;

        InitializeListHead(&endpointListHead);
        //tag = (PVOID) UcxUsbDeviceContext->PendingOperationCompletion;
        
        KeAcquireSpinLock(&UcxUsbDeviceContext->UcxControllerContext->TopologyLock, &irql);

        FOR_ALL_IN_LIST(UCXENDPOINT_PRIVCONTEXT, 
                        &UcxUsbDeviceContext->EndpointListHead,
                        EndpointListEntry,
                        currUcxEndpointContext) {

            numberOfEndpoints++;
            
            NT_ASSERT(IsListEntryNULL(&currUcxEndpointContext->EndpointListEntryTemp));

            InsertTailList(&endpointListHead, &currUcxEndpointContext->EndpointListEntryTemp);

            WdfObjectReferenceWithTag(
                currUcxEndpointContext->UcxEndpoint,
                TAG(UsbDevice_CallAddEventForAllEndpointsWithPendingOperationSet));
        }
        
        KeReleaseSpinLock(&UcxUsbDeviceContext->UcxControllerContext->TopologyLock, irql);

        UcxUsbDeviceContext->PendingOperations = numberOfEndpoints;
        
        LogVerbose(FlagUsbDevice, 
                   "Scheduling Pending Operation for UcxUsbDevice 0x%p, with #endpoints %d", 
                   UcxUsbDeviceContext->UcxUsbDevice, numberOfEndpoints);
        //
        // For each endpoint in the list, and add the Event to its state 
        // machine.
        //
        FOR_ALL_IN_LIST_SAFE(UCXENDPOINT_PRIVCONTEXT, 
                             &endpointListHead,
                             EndpointListEntryTemp,
                             currUcxEndpointContext,
                             nextUcxEndpointContext) {
            RemoveEntryList(&currUcxEndpointContext->EndpointListEntryTemp);
            InitializeListEntry(&currUcxEndpointContext->EndpointListEntryTemp);

            Endpoint_EventAdd(currUcxEndpointContext, Event);

            WdfObjectDereferenceWithTag(
                currUcxEndpointContext->UcxEndpoint, 
                TAG(UsbDevice_CallAddEventForAllEndpointsWithPendingOperationSet));

        }

    } FINALLY {

    }
    return;
}

VOID
UsbDevice_ForwardEndpointsConfigureToHcd(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT UcxUsbDeviceContext
   )
/*++

Routine Description:
    This routine is called when all the individual state machines complete
    the processing PrepareDisableFromHub event
    
Arguments:
    
    UcxUsbDeviceContext - Context of the usbdevice
    
Return Value:

    VOID
    
--*/
{
    WDFREQUEST    request;
    
    TRY {
        
        NT_ASSERT(UcxUsbDeviceContext->PendingEndpointsConfigureRequest != NULL);

        request = UcxUsbDeviceContext->PendingEndpointsConfigureRequest;
        UcxUsbDeviceContext->PendingEndpointsConfigureRequest = NULL;

        LogVerbose(FlagUsbDevice, "Fwd to HCD Request 0x%p", request);

        UcxUsbDeviceContext->EvtCallbacks.EvtUsbDeviceEndpointsConfigure(
            UcxUsbDeviceContext->UcxControllerContext->UcxController, 
            request);
        
    } FINALLY {

    }
    return;

}

VOID
UsbDevice_PendEndpointsConfigureDueToControllerReset(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT UcxUsbDeviceContext
   )
/*++

Routine Description:
    This routine is called when all the individual state machines complete
    the processing PrepareDisableFromHubAfterPrepareControllerReset event
    
Arguments:
    
    UcxUsbDeviceContext - Context of the usbdevice
    
Return Value:

    VOID
    
--*/
{
    WDFREQUEST                    request;
    PUCXCONTROLLER_PRIVCONTEXT    ucxControllerContext;

    
    TRY {
        
        NT_ASSERT(UcxUsbDeviceContext->PendingEndpointsConfigureRequest != NULL);

        request = UcxUsbDeviceContext->PendingEndpointsConfigureRequest;
        UcxUsbDeviceContext->PendingEndpointsConfigureRequest = NULL;

        LogVerbose(FlagUsbDevice,
                   "Forwarding Endpoints Configure Request 0x%p to QueuePendUsbDeviceMgmtDuringControllerReset", 
                   request);
        
        ucxControllerContext = UcxUsbDeviceContext->UcxControllerContext;

        WdfRequestForwardToIoQueue( 
            request,
            ucxControllerContext->QueuePendUsbDeviceMgmtDuringControllerReset);
    
    } FINALLY {

    }
    return;

}

VOID
UsbDevice_ForwardResetToHcd(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT UcxUsbDeviceContext
   )
/*++

Routine Description:
    This routine is called when all the individual state machines complete
    the processing UsbDeviceResetFromHub

Arguments:
    
    UcxUsbDeviceContext - Context of the usbdevice
    
Return Value:

    VOID
    
--*/
{
    WDFREQUEST    request;
    
    TRY {
        
        NT_ASSERT(UcxUsbDeviceContext->PendingUsbDeviceResetRequest != NULL);

        request = UcxUsbDeviceContext->PendingUsbDeviceResetRequest;
        UcxUsbDeviceContext->PendingUsbDeviceResetRequest = NULL;

        LogVerbose(FlagUsbDevice, "Fwd to HCD Request 0x%p", request);

        UcxUsbDeviceContext->EvtCallbacks.EvtUsbDeviceReset(
            UcxUsbDeviceContext->UcxControllerContext->UcxController, 
            request);
        
    } FINALLY {

    }
    return;

}

VOID
UsbDevice_PendResetRequestDueToControllerReset(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT UcxUsbDeviceContext
   )
/*++

Routine Description:
    This routine is called when all the individual state machines complete
    the processing UsbDeviceResetFromHubAfterPrepareControllerReset

Arguments:
    
    UcxUsbDeviceContext - Context of the usbdevice
    
Return Value:

    VOID
    
--*/
{
    WDFREQUEST                   request;
    PUCXCONTROLLER_PRIVCONTEXT   ucxControllerContext;


    
    TRY {
        
        NT_ASSERT(UcxUsbDeviceContext->PendingUsbDeviceResetRequest != NULL);

        request = UcxUsbDeviceContext->PendingUsbDeviceResetRequest;
        UcxUsbDeviceContext->PendingUsbDeviceResetRequest = NULL;

        LogVerbose(FlagUsbDevice,
                   "Forwarding Usb device reset Request 0x%p to QueuePendUsbDeviceMgmtDuringControllerReset", 
                   request);
        
        ucxControllerContext = UcxUsbDeviceContext->UcxControllerContext;

        WdfRequestForwardToIoQueue( 
            request,
            ucxControllerContext->QueuePendUsbDeviceMgmtDuringControllerReset);
    
    } FINALLY {

    }
    return;

}


VOID
UsbDevice_ForwardDisableToHcd(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT UcxUsbDeviceContext
   )
/*++

Routine Description:
    This routine is called when all the individual state machines complete
    the processing PrepareDisableFromHub

Arguments:
    
    UcxUsbDeviceContext - Context of the usbdevice
    
Return Value:

    VOID
    
--*/
{
    WDFREQUEST    request;
    
    TRY {
        
        NT_ASSERT(UcxUsbDeviceContext->PendingUsbDeviceDisableRequest != NULL);

        request = UcxUsbDeviceContext->PendingUsbDeviceDisableRequest;
        UcxUsbDeviceContext->PendingUsbDeviceDisableRequest = NULL;

        LogVerbose(FlagUsbDevice, "Fwd to HCD Request 0x%p", request);

        UcxUsbDeviceContext->EvtCallbacks.EvtUsbDeviceDisable(
            UcxUsbDeviceContext->UcxControllerContext->UcxController, 
            request);
        
    } FINALLY {

    }
    return;

}

VOID
UsbDevice_PendDisableRequestDueToControllerReset(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT UcxUsbDeviceContext
   )
/*++

Routine Description:
    This routine is called when all the individual state machines complete
    the processing PrepareDisableFromHub

Arguments:
    
    UcxUsbDeviceContext - Context of the usbdevice
    
Return Value:

    VOID
    
--*/
{
    WDFREQUEST                   request;
    PUCXCONTROLLER_PRIVCONTEXT   ucxControllerContext;

    
    TRY {
        
        NT_ASSERT(UcxUsbDeviceContext->PendingUsbDeviceDisableRequest != NULL);

        request = UcxUsbDeviceContext->PendingUsbDeviceDisableRequest;
        UcxUsbDeviceContext->PendingUsbDeviceDisableRequest = NULL;

        LogVerbose(FlagUsbDevice,
                   "Forwarding UsbDevice Disable Request 0x%p to QueuePendUsbDeviceMgmtDuringControllerReset", 
                   request);
        
        ucxControllerContext = UcxUsbDeviceContext->UcxControllerContext;

        WdfRequestForwardToIoQueue( 
            request,
            ucxControllerContext->QueuePendUsbDeviceMgmtDuringControllerReset);
    
    } FINALLY {

    }
    return;

}


VOID
UsbDevice_CompleteUsbDeviceStartToHub(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT UcxUsbDeviceContext
   )
/*++

Routine Description:
    This routine is called when all the individual state machines complete
    the processing StartFromHub

Arguments:
    
    UcxUsbDeviceContext - Context of the usbdevice
    
Return Value:

    VOID
    
--*/
{
    WDFREQUEST    request;
    
    TRY {
        
        NT_ASSERT(UcxUsbDeviceContext->PendingUsbDeviceStartRequest != NULL);

        request = UcxUsbDeviceContext->PendingUsbDeviceStartRequest;
        UcxUsbDeviceContext->PendingUsbDeviceStartRequest = NULL;

        LogVerbose(FlagUsbDevice, "Completing Request 0x%p", request);

        WdfRequestComplete(request, STATUS_SUCCESS);
        
    } FINALLY {

    }
    return;

}

VOID
UsbDevice_CompleteUsbDevicePurgeToHub(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT UcxUsbDeviceContext
   )
/*++

Routine Description:
    This routine is called when all the individual state machines complete
    the processing Purge

Arguments:
    
    UcxUsbDeviceContext - Context of the usbdevice
    
Return Value:

    VOID
    
--*/
{
    WDFREQUEST    request;
    
    TRY {
        
        NT_ASSERT(UcxUsbDeviceContext->PendingUsbDevicePurgeRequest != NULL);

        request = UcxUsbDeviceContext->PendingUsbDevicePurgeRequest;
        UcxUsbDeviceContext->PendingUsbDevicePurgeRequest = NULL;

        LogVerbose(FlagUsbDevice, "Completing Request 0x%p", request);
        
        WdfRequestComplete(request, STATUS_SUCCESS);
        
    } FINALLY {

    }
    return;

}

VOID
UsbDevice_CompleteUsbDeviceAbortToHub(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT UcxUsbDeviceContext
   )
/*++

Routine Description:
    This routine is called when all the individual state machines complete
    the processing Purge

Arguments:
    
    UcxUsbDeviceContext - Context of the usbdevice
    
Return Value:

    VOID
    
--*/
{
    WDFREQUEST    request;
    
    TRY {
        
        NT_ASSERT(UcxUsbDeviceContext->PendingUsbDeviceAbortRequest != NULL);

        request = UcxUsbDeviceContext->PendingUsbDeviceAbortRequest;
        UcxUsbDeviceContext->PendingUsbDeviceAbortRequest = NULL;

        LogVerbose(FlagUsbDevice, "Completing Request 0x%p", request);
        
        WdfRequestComplete(request, STATUS_SUCCESS);
        
    } FINALLY {

    }
    return;

}

VOID
UsbDevice_CompleteEnable(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT UcxUsbDeviceContext
   )
/*++

Routine Description:

    This routine is called when all the individual state machines complete
    the processing of an operation such as: 
        
        EnableCompleteFromHcd
  
Arguments:
    
    UcxUsbDeviceContext - Context of the usbdevice
    
Return Value:

    VOID
    
--*/
{
    PIRP irp;
    
    TRY {

        irp = UcxUsbDeviceContext->PendingUsbDeviceEnableIrp;
        UcxUsbDeviceContext->PendingUsbDeviceEnableIrp = NULL;

        if (UcxUsbDeviceContext->DeprogrammedDueToControllerReset) {
            //
            // Regardless of the status why which the HCD completed the Enable request, 
            // we must now fail it. (Since the controller has been reset).
            //
            irp->IoStatus.Status = STATUS_NO_SUCH_DEVICE;
        } else {
            irp->IoStatus.Status = STATUS_SUCCESS;
        }
        irp->IoStatus.Information = 0;
        
        LogVerbose(FlagUsbDevice, "Completing Irp 0x%p", irp);
        
        IoCompleteRequest(irp, IO_NO_INCREMENT);        

    } FINALLY {

    }
    return;

}

VOID
UsbDevice_CompleteEndpointsConfigure(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT UcxUsbDeviceContext
   )
/*++

Routine Description:

    This routine is called when all the individual state machines complete
    the processing of an operation such as: 
        
        EnableCompleteFromHcd
    
Arguments:
    
    UcxUsbDeviceContext - Context of the usbdevice
    
Return Value:

    VOID
    
--*/
{
    PIRP irp;
    
    TRY {

        irp = UcxUsbDeviceContext->PendingEndpointsConfigureIrp;
        UcxUsbDeviceContext->PendingEndpointsConfigureIrp = NULL;
        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = 0;
        
        LogVerbose(FlagUsbDevice, "Completing Irp 0x%p", irp);
        
        IoCompleteRequest(irp, IO_NO_INCREMENT);        

    } FINALLY {

    }
    return;

}


VOID
UsbDevice_CompleteResetIrp(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT UcxUsbDeviceContext
   )
/*++

Routine Description:

    This routine is called when all the individual state machines complete
    the processing of an operation such as: 
        
        ResetCompleteFromHcd
    
Arguments:
    
    UcxUsbDeviceContext - Context of the usbdevice
    
Return Value:

    VOID
    
--*/
{
    PIRP  irp;
    KIRQL irql;
    
    TRY {

        KeAcquireSpinLock(&UcxUsbDeviceContext->UcxControllerContext->TopologyLock, &irql);

        if (UcxUsbDeviceContext->AllowEndpointsToExitTreePurge == TRUE &&
            UcxUsbDeviceContext->AllowChildrenToExitTreePurge == FALSE) {

            //
            // Now that reset is done and we got endpoints out of tree purge, child devices are
            // allowed to exit tree purge.
            //
            UcxUsbDeviceContext->AllowChildrenToExitTreePurge = TRUE;
        }

        KeReleaseSpinLock(&UcxUsbDeviceContext->UcxControllerContext->TopologyLock, irql);

        irp = UcxUsbDeviceContext->PendingUsbDeviceResetIrp;
        UcxUsbDeviceContext->PendingUsbDeviceResetIrp = NULL;
        irp->IoStatus.Status = STATUS_SUCCESS;
        irp->IoStatus.Information = 0;
        
        LogVerbose(FlagUsbDevice, "Completing Irp 0x%p", irp);
        
        IoCompleteRequest(irp, IO_NO_INCREMENT);        

    } FINALLY {

    }
    return;

}


VOID
UsbDevice_ResetFromHub(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in
        WDFREQUEST                    Request,
    __in
        BOOLEAN                       AfterControllerResetInitiated
   )
/*++

Routine Description:

    This routine is called when the hub driver sends an
    IOCTL_INTERNAL_USB USBDEVICE_RESET request.
    
    This routine walks through the list of endpoints of the device and schedules a ResetFromHub
    event to each one's endpoint state machine. 
    
    Once the endpoint state machine for each one is done processing this, 
    we can send this request to the HCD

Arguments:

    UcxUsbDeviceContext - A pointer to UcxUsbDeviceContext
    
    Request - Reset request
    
    AfterControllerResetInitiated - Boolean indicating whether this request is being handled after 
       Controller reset operation has been intiated, and this device is still deprogrammed in the 
       hardware.
    
--*/
{
    UCXENDPOINT                                     ucxEndpoint;
    PUCXENDPOINT_PRIVCONTEXT                        ucxEndpointContext;
    PUSBDEVICE_RESET                                usbDeviceReset;
    KIRQL                                           irql;
    PUCXUSBDEVICE_PRIVCONTEXT                       parentUcxUsbDeviceContext;
    PFN_UCXUSBDEVICE_PENDING_OPERATION_COMPLETION   pendingOperationCompletion;

    TRY {

        parentUcxUsbDeviceContext = UcxUsbDeviceGetContext(UcxUsbDeviceContext->ParentHub);

        usbDeviceReset = (PUSBDEVICE_RESET) Ucx_WdfRequestGetArg1(Request);

        KeAcquireSpinLock(&UcxUsbDeviceContext->UcxControllerContext->TopologyLock, &irql);

        if (UcxUsbDeviceContext->AllowEndpointsToExitTreePurge == FALSE) {

            //
            // If we've been tree purged, we're allowed to exit tree purge at reset time if the
            // parent has already completed reset after tree purge (or if the parent was not tree
            // purged).
            //
            if (parentUcxUsbDeviceContext->AllowChildrenToExitTreePurge) {

                UcxUsbDeviceContext->AllowEndpointsToExitTreePurge = TRUE;
            }
        }

        KeReleaseSpinLock(&UcxUsbDeviceContext->UcxControllerContext->TopologyLock, irql);

        NT_ASSERT(UcxUsbDeviceContext->PendingUsbDeviceResetRequest == NULL);
        NT_ASSERT(UcxUsbDeviceContext->PendingOperationCompletion == NULL);

        if (AfterControllerResetInitiated) {
            pendingOperationCompletion = UsbDevice_PendResetRequestDueToControllerReset;
        } else {
            pendingOperationCompletion = UsbDevice_ForwardResetToHcd;
        }

        LogVerbose(FlagUsbDevice,"Pending Request 0x%p, waiting for UsbDeviceForwardResetToHcd", 
                   Request);
        UcxUsbDeviceContext->PendingUsbDeviceResetRequest = Request;
        UcxUsbDeviceContext->PendingOperationCompletion = pendingOperationCompletion;
        UcxUsbDeviceContext->PendingOperations = usbDeviceReset->EndpointsToDisableCount + 1;

        //
        // Walk through the list of all endpoints being disabled
        // enqueue a UcxEndpointEventUsbDeviceResetFromHub event.
        //
        UsbDevice_AddEventForEndpoints(usbDeviceReset->EndpointsToDisable,
                                       usbDeviceReset->EndpointsToDisableCount,
                                       UcxEndpointEventUsbDeviceResetFromHub);

        //
        // Also do the same for the default endpoint
        //
        ucxEndpoint = usbDeviceReset->DefaultEndpoint;
        ucxEndpointContext = UcxEndpointGetContext(ucxEndpoint);
        Endpoint_EventAdd(ucxEndpointContext,
                          UcxEndpointEventUsbDeviceResetFromHub);

    } FINALLY {

    }
    return;
}

VOID
UsbDevice_EndpointsConfigureFromHub(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT         UcxUsbDeviceContext,
    __in
        WDFREQUEST                        Request,
    __in
        BOOLEAN                           AfterControllerResetInitiated
    )
/*++

Routine Description:

    This routine is called when the hub driver sends an
    IOCTL_INTERNAL_USB ENDPOINTS_CONFIGURE request.
    
    This routine walks through the list of endpoints in this ioctl that are 
    being deconfigured, and schedules a DeconfigureFromHub event to each one's 
    endpoint state machine. 
    
    Once the endpoint state machine for each one is done processing this, 
    we can send this request to the HCD (if Controller Reset is not initiated for this 
    device) else we will just complete the EndpointsConfigure request

Arguments:

    UcxUsbDeviceContext - A pointer to UcxUsbDeviceContext
    
    Request - EndpointsConfigure request
    
    AfterControllerResetInitiated - Boolean indicating whether this request is being handled after 
       Controller reset operation has been intiated, and this device is still deprogrammed in the 
       hardware.
    
    
--*/
{
    PENDPOINTS_CONFIGURE                            endpointsConfigure;
    PFN_UCXUSBDEVICE_PENDING_OPERATION_COMPLETION   pendingOperationCompletion;

    TRY {
        
        endpointsConfigure = (PENDPOINTS_CONFIGURE) Ucx_WdfRequestGetArg1(Request);

        if (endpointsConfigure->EndpointsToDisableCount == 0) {

            //
            // There are no endpoints to disable, so UCX doesnt need to
            // do anything here.
            //
            if (AfterControllerResetInitiated) {

                LogInfo(FlagUsbDevice, "Fail EndpointsConfigure due to Controller Reset 0x%p", Request);
                
                WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);

            } else {

                LogVerbose(FlagUsbDevice, "Fwd to HCD, Request 0x%p", Request);

                UcxUsbDeviceContext->EvtCallbacks.EvtUsbDeviceEndpointsConfigure(
                    UcxUsbDeviceContext->UcxControllerContext->UcxController, 
                    Request);

            }
            LEAVE;
        }
        
        NT_ASSERT(UcxUsbDeviceContext->PendingEndpointsConfigureRequest == NULL);
        NT_ASSERT(UcxUsbDeviceContext->PendingOperationCompletion == NULL);

        if (AfterControllerResetInitiated) {
            pendingOperationCompletion = UsbDevice_PendEndpointsConfigureDueToControllerReset;
        } else {
            pendingOperationCompletion = UsbDevice_ForwardEndpointsConfigureToHcd;
        }
        
        LogVerbose(FlagUsbDevice,
                   "Pending Request 0x%p, waiting for UsbDeviceForwardEndpointsConfigureToHcd", 
                   Request);
        UcxUsbDeviceContext->PendingEndpointsConfigureRequest = Request;
        UcxUsbDeviceContext->PendingOperationCompletion = pendingOperationCompletion;
        UcxUsbDeviceContext->PendingOperations = endpointsConfigure->EndpointsToDisableCount;

        //
        // Walk through the list of all endpoints being disbled
        // enqueue a UcxEndpointEventPrepareDisableFromHub event.
        // Note: It is important to store the EndpointsToDisableCount in a local variable, since 
        // as soon as we schedule the event for the last endpoint in the EndpointsToDisable list
        // the irp may get completed and thus EndpointsConfigure structure will no longer be valid.
        //
        UsbDevice_AddEventForEndpoints(endpointsConfigure->EndpointsToDisable,
                                       endpointsConfigure->EndpointsToDisableCount,
                                       UcxEndpointEventPrepareDisableFromHub);
        
    } FINALLY {

    }
    return;
}


VOID
UsbDevice_EndpointsConfigureCompleteFromHcd(
    __in
        PIRP                    Irp, 
    __in
        PENDPOINTS_CONFIGURE    EndpointsConfigure
   )
/*++

Routine Description:

    This routine is called when the controller driver completes an
    IOCTL_INTERNAL_USB ENDPOINTS_CONFIGURE request.
    
    This routine walks through the list of endpoints in this ioctl, and calls
    the appropiate Event for each endpoints state machine.

Arguments:

    Irp - Endpoints Configure Irp
    
    EndpointsConfigure - A pointer to the ENDPOINTS_CONFIGURE structure in the
    ioctl.
    
--*/
{
    UCXUSBDEVICE                      ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT         ucxUsbDeviceContext;

    TRY {

        if (!NT_SUCCESS(Irp->IoStatus.Status)) {
            //
            // Complete the Irp here
            //
            LogError(FlagUsbDevice, "Irp 0x%p failed %!STATUS!", Irp, Irp->IoStatus.Status);
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            LEAVE;
        }

        ucxUsbDevice = EndpointsConfigure->UsbDevice;
        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);


        if (EndpointsConfigure->EndpointsToEnableCount == 0) {

            //
            // There are no endpoints to enable, so UCX doesnt need to
            // do anything here.
            //
            Irp->IoStatus.Status = STATUS_SUCCESS;
            Irp->IoStatus.Information = 0;

            LogVerbose(FlagUsbDevice, "Completing Irp 0x%p", Irp);

            IoCompleteRequest(Irp, IO_NO_INCREMENT);

            LEAVE;
        }
        
        NT_ASSERT(ucxUsbDeviceContext->PendingEndpointsConfigureIrp == NULL);
        NT_ASSERT(ucxUsbDeviceContext->PendingOperationCompletion == NULL);
        
        LogVerbose(FlagUsbDevice,
                   "Pending Irp 0x%p, waiting for UsbDeviceCompleteEndpointsConfigureToHub", Irp);
        ucxUsbDeviceContext->PendingEndpointsConfigureIrp = Irp;
        ucxUsbDeviceContext->PendingOperationCompletion = UsbDevice_CompleteEndpointsConfigure;
        ucxUsbDeviceContext->PendingOperations = EndpointsConfigure->EndpointsToEnableCount;

        //
        // Walk through the list of all endpoints being enabled and 
        // enqueue a UcxEndpointEventConfigureCompleteFromHcd event. 
        // Note: It is important to store the EndpointsToEnableCount in a local variable, since 
        // as soon as we schedule the event for the last endpoint in the EndpointsToEnable list
        // the irp may get completed and thus EndpointsConfigure structure will no longer be valid.
        //
        UsbDevice_AddEventForEndpoints(EndpointsConfigure->EndpointsToEnable,
                                       EndpointsConfigure->EndpointsToEnableCount,
                                       UcxEndpointEventEnableComplete);

    } FINALLY {

    }
    return; 
}


VOID
UsbDevice_DefaultEndpointEnableCompleteFromHcd(
    __in
        PIRP                    Irp, 
    __in
        PUSBDEVICE_ENABLE       UsbDeviceEnable
   )
/*++

Routine Description:

    This routine is called when the controller driver completes an
    IOCTL_INTERNAL_USB USBDEVICE_ENABLE request.

Arguments:

    Irp - Endpoints Configure Irp
    
    UsbDeviceEnable - A pointer to the UsbDeviceEnable structure in the
    ioctl.
    
--*/
{
    UCXUSBDEVICE              ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT ucxUsbDeviceContext;
    UCXENDPOINT               ucxEndpoint;
    PUCXENDPOINT_PRIVCONTEXT  ucxEndpointContext;

    TRY {

        if (!NT_SUCCESS(Irp->IoStatus.Status)) {
            //
            // Complete the Irp here
            //
            LogError(FlagUsbDevice, "Irp 0x%p failed %!STATUS!", Irp, Irp->IoStatus.Status);
            
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            LEAVE;
        }

        ucxUsbDevice = UsbDeviceEnable->UsbDevice;
        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
        
        NT_ASSERT(ucxUsbDeviceContext->PendingUsbDeviceEnableIrp == NULL);
        NT_ASSERT(ucxUsbDeviceContext->PendingOperationCompletion == NULL);

        LogVerbose(FlagUsbDevice,"Pending Irp 0x%p, waiting for UsbDeviceCompleteEnable", Irp);
        
        ucxUsbDeviceContext->PendingOperationCompletion = UsbDevice_CompleteEnable;
        ucxUsbDeviceContext->PendingOperations = 1;
        ucxUsbDeviceContext->PendingUsbDeviceEnableIrp = Irp;
        Irp = NULL;

        ucxUsbDeviceContext->Enabled = TRUE;
        
        //
        // enqueue a UcxEndpointEventEnable event for the default endpoint
        //

        ucxEndpoint = UsbDeviceEnable->DefaultEndpoint;
        ucxEndpointContext = UcxEndpointGetContext(ucxEndpoint);

        Endpoint_EventAdd(ucxEndpointContext, 
                          UcxEndpointEventEnableComplete);

    } FINALLY {

    }
    return; 
}


VOID
UsbDevice_ResetCompleteFromHcd(
    __in
        PIRP                    Irp, 
    __in 
        PUSBDEVICE_RESET        UsbDeviceReset
   )
/*++

Routine Description:

    This routine is called when the controller driver completes an
    IOCTL_INTERNAL_USB USBDEVICE_RESET request.

Arguments:

    Irp - Device Reset Irp
    
    UsbDeviceReset - A pointer to the UsbDeviceReset structure in the
    ioctl.
    
--*/
{
    UCXUSBDEVICE                      ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT         ucxUsbDeviceContext;
    UCXENDPOINT                       ucxEndpoint;
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext;

    TRY {

        if (!NT_SUCCESS(Irp->IoStatus.Status)) {

            NT_ASSERTMSG("HCD is not allowed to fail this request", FALSE);
            LogError(FlagUsbDevice, "Irp 0x%p failed %!STATUS!", Irp, Irp->IoStatus.Status);

            //
            // Ignoring
            //
        }

        ucxUsbDevice = UsbDeviceReset->UsbDevice;
        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);

        NT_ASSERT(ucxUsbDeviceContext->PendingUsbDeviceResetIrp == NULL);
        NT_ASSERT(ucxUsbDeviceContext->PendingOperationCompletion == NULL);

        LogVerbose(FlagUsbDevice,"Pending Irp 0x%p, waiting for UsbDeviceCompleteReset", Irp);

        ucxUsbDeviceContext->PendingOperationCompletion = UsbDevice_CompleteResetIrp;
        ucxUsbDeviceContext->PendingOperations = 1;
        ucxUsbDeviceContext->PendingUsbDeviceResetIrp = Irp;
        Irp = NULL;

        //
        // enqueue a UcxEndpointEventEnable event for the default endpoint
        //

        ucxEndpoint = UsbDeviceReset->DefaultEndpoint;
        ucxEndpointContext = UcxEndpointGetContext(ucxEndpoint);

        Endpoint_EventAdd(ucxEndpointContext, 
                          UcxEndpointEventUsbDeviceResetComplete);

    } FINALLY {

    }
    return; 
}

VOID
UsbDevice_DisableFromHub(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in
        WDFREQUEST                    Request,
    __in
        BOOLEAN                       AfterControllerResetInitiated
   )
/*++

Routine Description:

    This routine is called when the hub driver sends an 
    IOCTL_INTERNAL_USB USBDEVICE_DISABLE request.

    This routine sends schedules a disable event in the default endpoint state machine.
    
Arguments:

    UcxUsbDeviceContext - A pointer to UcxUsbDeviceContext
    
    Request - Disable request

    AfterControllerResetInitiated - Boolean indicating whether this request is being handled after 
       Controller reset operation has been intiated, and this device is still deprogrammed in the 
       hardware.

--*/
{

    PUCXENDPOINT_PRIVCONTEXT                        ucxEndpointContext;
    PUSBDEVICE_DISABLE                              usbDeviceDisable;
    PFN_UCXUSBDEVICE_PENDING_OPERATION_COMPLETION   pendingOperationCompletion;

    TRY {

        usbDeviceDisable = (PUSBDEVICE_DISABLE) Ucx_WdfRequestGetArg1(Request);

        ucxEndpointContext = UcxEndpointGetContext(usbDeviceDisable->DefaultEndpoint);

        NT_ASSERT(UcxUsbDeviceContext->PendingUsbDeviceDisableRequest == NULL);
        NT_ASSERT(UcxUsbDeviceContext->PendingOperationCompletion == NULL);
        
        if (AfterControllerResetInitiated) {
            pendingOperationCompletion = UsbDevice_PendDisableRequestDueToControllerReset;
        } else {
            pendingOperationCompletion = UsbDevice_ForwardDisableToHcd;
        }

        LogVerbose(FlagUsbDevice,"Pending Request 0x%p, waiting for UsbDeviceForwardDisableToHcd", 
                   Request);
        
        UcxUsbDeviceContext->Enabled = FALSE;
        
        UcxUsbDeviceContext->PendingUsbDeviceDisableRequest = Request;
        UcxUsbDeviceContext->PendingOperationCompletion = pendingOperationCompletion;
        UcxUsbDeviceContext->PendingOperations = 1;

        Endpoint_EventAdd(ucxEndpointContext, UcxEndpointEventPrepareDisableFromHub);
        
    } FINALLY {

    }
    return; 
}

VOID
UsbDevice_StartFromHub(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in
        WDFREQUEST                    Request
   )
/*++

Routine Description:

    This routine is called when the hub driver sends an 
    IOCTL_INTERNAL_USB_USBDEVICE_STARTIO request.

    This routine queues a start event to all the endpoints of the device. 
      
Arguments:

    UcxUsbDeviceContext - A pointer to UcxUsbDeviceContext
    
    Request - Start request

--*/
{
    TRY {
        NT_ASSERT(UcxUsbDeviceContext->PendingUsbDeviceStartRequest == NULL);
        NT_ASSERT(UcxUsbDeviceContext->PendingOperationCompletion == NULL);

        //
        // IOCTL_INTERNAL_USB_USBDEVICE_STARTIO is called exclusively by the hub
        // driver when the device is being resumed from suspend and hence no additional
        // hint is required currently to propagate the resume notification to the UCX
        // client.
        //
        // TODO: Make this dependent on the hint provided by the hub driver in case
        // START_IO can get called in scenarios other than during resume.
        //
        
        if (UcxUsbDeviceContext->EvtCallbacks.EvtUsbDeviceResume) {

            UcxUsbDeviceContext->EvtCallbacks.EvtUsbDeviceResume(
                UcxUsbDeviceContext->UcxControllerContext->UcxController,
                UcxUsbDeviceContext->UcxUsbDevice);
        }
        
        LogVerbose(FlagUsbDevice,
                   "Pending Request 0x%p, waiting for UsbDeviceCompleteUsbDeviceStartToHub", 
                   Request);
        
        UcxUsbDeviceContext->PendingUsbDeviceStartRequest = Request;
        UcxUsbDeviceContext->PendingOperationCompletion = UsbDevice_CompleteUsbDeviceStartToHub;
        
        UsbDevice_CallAddEventForAllEndpointsWithPendingOperationSet(UcxUsbDeviceContext,
                                                                     UcxEndpointEventStartFromHub);

    } FINALLY {

    }
}

VOID
UsbDevice_PurgeFromHub(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in
        WDFREQUEST                    Request,
    __in
        BOOLEAN                       OnSuspend
   )
/*++

Routine Description:

    This routine is called when the hub driver sends an 
    IOCTL_INTERNAL_USB_USBDEVICE_PURGEIO request.

    This routine queues a purge event to all the endpoints of the device. 
      
Arguments:

    UcxUsbDeviceContext - A pointer to UcxUsbDeviceContext
    
    Request - purge request

--*/
{
    TRY {
        NT_ASSERT(UcxUsbDeviceContext->PendingUsbDevicePurgeRequest == NULL);
        NT_ASSERT(UcxUsbDeviceContext->PendingOperationCompletion == NULL);
        
        if (OnSuspend) {
            
            if (UcxUsbDeviceContext->EvtCallbacks.EvtUsbDeviceSuspend) {

                UcxUsbDeviceContext->EvtCallbacks.EvtUsbDeviceSuspend(
                    UcxUsbDeviceContext->UcxControllerContext->UcxController,
                    UcxUsbDeviceContext->UcxUsbDevice);
            }
        }

        LogVerbose(FlagUsbDevice, 
                   "Pending Request 0x%p, waiting for UsbDeviceCompleteUsbDevicePurgeToHub", 
                   Request);
        
        UcxUsbDeviceContext->PendingUsbDevicePurgeRequest = Request;
        UcxUsbDeviceContext->PendingOperationCompletion = UsbDevice_CompleteUsbDevicePurgeToHub;

        UsbDevice_CallAddEventForAllEndpointsWithPendingOperationSet(UcxUsbDeviceContext,
                                                                     UcxEndpointEventPurgeFromHub);
    } FINALLY {

    }
}

VOID
UsbDevice_AbortFromHub(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in
        WDFREQUEST                    Request
   )
/*++

Routine Description:

    This routine is called when the hub driver sends an 
    IOCTL_INTERNAL_USB_USBDEVICE_ABORTIO request.

    This routine queues a purge event to all the endpoints of the device. 
      
Arguments:

    UcxUsbDeviceContext - A pointer to UcxUsbDeviceContext
    
    Request - purge request

--*/
{
    TRY {
        NT_ASSERT(UcxUsbDeviceContext->PendingUsbDeviceAbortRequest == NULL);
        NT_ASSERT(UcxUsbDeviceContext->PendingOperationCompletion == NULL);

        LogVerbose(FlagUsbDevice, 
                   "Pending Request 0x%p, waiting for UsbDeviceCompleteUsbDeviceAbortToHub", 
                   Request);
        
        UcxUsbDeviceContext->PendingUsbDevicePurgeRequest = Request;
        UcxUsbDeviceContext->PendingOperationCompletion = UsbDevice_CompleteUsbDeviceAbortToHub;

        UsbDevice_CallAddEventForAllEndpointsWithPendingOperationSet(UcxUsbDeviceContext,
                                                                     UcxEndpointEventAbortFromHub);
    } FINALLY {

    }
}

VOID
UsbDevice_TreePurgeFromHub(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT     UcxUsbDeviceContext,
    __in
        WDFREQUEST                    Request
   )
/*++

Routine Description:

    This routine is called when the hub driver sends an 
    IOCTL_INTERNAL_USB_USBDEVICE_TREE_PURGEIO request.

    This routine queues a TreePurge event to all endpoints downstream of the device and sets up the
    completion.
      
Arguments:

    UcxUsbDeviceContext - A pointer to UcxUsbDeviceContext
    
    Request - TreePurge request

--*/
{
    PUCXCONTROLLER_PRIVCONTEXT  ucxControllerContext;
    KIRQL                       irql;
    USBDEVICE_TREE_PURGE_CALLBACK_CONTEXT
                                callbackContext;
    PUCXENDPOINT_PRIVCONTEXT    currUcxEndpointContext;
    PUCXENDPOINT_PRIVCONTEXT    nextUcxEndpointContext;

    TRY {

        ucxControllerContext = UcxUsbDeviceContext->UcxControllerContext;

        USBDEVICE_TREE_PURGE_CALLBACK_CONTEXT_INIT(&callbackContext);

        NT_ASSERT(ucxControllerContext->PendingTreePurgeRequest == NULL);
        NT_ASSERT(ucxControllerContext->PendingTreePurgeEndpointCount == 0);

        //
        // Protect against topology changes while we walk the tree
        //
        KeAcquireSpinLock(&ucxControllerContext->TopologyLock, &irql);

        //
        // TRUE -> Walk disconnected tree. If a device has been disconnected but its endpoints
        // haven't made it all the way to purge yet, we need to wait for them to enter purge.
        //
        UsbDevice_WalkTree(UcxUsbDeviceContext,
                           TRUE,
                           UsbDevice_TreePurgeCallback,
                           &callbackContext);

        KeReleaseSpinLock(&ucxControllerContext->TopologyLock, irql);

        ucxControllerContext->PendingTreePurgeRequest = Request;
        ucxControllerContext->PendingTreePurgeEndpointCount = callbackContext.EndpointCount;

        //
        // Queue TreePurge event to each state machine. Each one must then become purged if it's not
        // already purged. We'll know we can complete the request when the number of EP SMs
        // acknowledging tree purge equals the number of EP SMs we queued
        // UcxEndpointEventTreePurgeFromHub to.
        //
        FOR_ALL_IN_LIST_SAFE(UCXENDPOINT_PRIVCONTEXT, 
                             &callbackContext.EndpointList,
                             UsbDeviceTreePurgeListEntryTemp,
                             currUcxEndpointContext,
                             nextUcxEndpointContext) {

            RemoveEntryList(&currUcxEndpointContext->UsbDeviceTreePurgeListEntryTemp);

            InitializeListEntry(&currUcxEndpointContext->UsbDeviceTreePurgeListEntryTemp);

            Endpoint_EventAdd(currUcxEndpointContext, UcxEndpointEventTreePurgeFromHub);

            WdfObjectDereferenceWithTag(currUcxEndpointContext->UcxEndpoint,
                                        TAG(UsbDevice_TreePurgeCallback));
        }

    } FINALLY {

    }
    return;
}

__drv_mustHold(SpinLock)
VOID
UsbDevice_TreePurgeCallback(
    __in
      PUCXUSBDEVICE_PRIVCONTEXT                     UcxUsbDeviceContext,
    __inout
      PUSBDEVICE_TREE_PURGE_CALLBACK_CONTEXT        TreePurgeCallbackContext
    )
/*++

Routine Description:

    When the hub driver tree purges a device, this function is called on the device and every
    descendant of the device.

    In this routine, we walk the list of all endpoints of the deivce, reference them, add them
    to a list, and add the count of endpoints to the running total.

Arguments:

    UcxUsbDeviceContext - Context of the usbdevice that is going to be purged
    
    Callback context - Information on the current tree purge operation

Return Value:

    VOID

--*/
{
    PUCXENDPOINT_PRIVCONTEXT currUcxEndpointContext;

    TRY {

        //
        // Endpoints of devices downstream of the root must be kept in a purged I/O state.
        // The hub driver will eventually unpurge the tree in parent-first hierarchical order, but
        // descendants might first attempt to exit purge for other reasons due to the lack of
        // cross-device synchronization in the hub driver.
        //
        UcxUsbDeviceContext->AllowChildrenToExitTreePurge = FALSE;
        UcxUsbDeviceContext->AllowEndpointsToExitTreePurge = FALSE;

        FOR_ALL_IN_LIST(UCXENDPOINT_PRIVCONTEXT, 
                        &UcxUsbDeviceContext->EndpointListHead,
                        EndpointListEntry,
                        currUcxEndpointContext) {

            NT_ASSERT(IsListEntryNULL(&currUcxEndpointContext->UsbDeviceTreePurgeListEntryTemp));

            //
            // Take a reference since we are going to walk this list of
            // endpoints outside the TopologyLock
            //

            WdfObjectReferenceWithTag(currUcxEndpointContext->UcxEndpoint,
                                      TAG(UsbDevice_TreePurgeCallback));

            InsertTailList(&TreePurgeCallbackContext->EndpointList, 
                           &currUcxEndpointContext->UsbDeviceTreePurgeListEntryTemp);
            TreePurgeCallbackContext->EndpointCount++;
        }

    } FINALLY {

    }
    return;
}

VOID
UsbDevice_PendingTreePurgeCompleteByEndpoint(
    __in
        PUCXENDPOINT_PRIVCONTEXT  UcxEndpointContext
    )
/*++
Routine Description:

    Complete the controller's pending tree purge if this is the last endpoint to finish.

    This is a similar concept to UsbDevice_PendingOperationCompleteByEndpoint, though
    PendingOperations are scoped to a single device and PendingTreePurge is 1 per controller.

Return Value:

    SmEngineEventNull
--*/
{
    PUCXCONTROLLER_PRIVCONTEXT  ucxControllerContext;

    TRY {

        ucxControllerContext = UcxEndpointContext->UcxControllerContext;

        if (0 == InterlockedDecrement(&ucxControllerContext->PendingTreePurgeEndpointCount)) {

            //
            // This was the last endpoint downstream of the controller needing to perform tree
            // purge.
            //
            WdfRequestComplete(ucxControllerContext->PendingTreePurgeRequest, STATUS_SUCCESS);
            ucxControllerContext->PendingTreePurgeRequest = NULL;

            //
            // Start the queue again to allow another request to come in (these requests are
            // serialized).
            //
            WdfIoQueueStart(ucxControllerContext->QueueTreePurge);
        }

    } FINALLY {

    }
    return;
}

#define REGISTER_COMPOSITE_DEVICE_VERSION_0100 0x100

VOID
UsbDevice_RegisterCompositeDevice(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT       UcxUsbDeviceContext,
    __in 
        PREGISTER_COMPOSITE_DEVICE      RegisterCompositeDevice,
    __out_ecount(RegisterCompositeDevice->FunctionCount)
        PUSBD_FUNCTION_HANDLE           UsbdFunctionHandles,
    __in
        WDFREQUEST                      Request
   )
/*++

Routine Description:
    
    This routine registers a composite driver and provides a list of function handles for each
    function that composite device has. 

Arguments:

    UcxUsbDeviceContext - Ucx context for the UCXUSBDEVICE object
    
    RegisterCompositeDevice - Pointer to the REGISTER_COMPOSITE_DEVICE Structure.
    
    UsbdFunctionHandles - Pointer to the buffer where this function will fill the
         USBD_FUNCTION_HANDLESs
         
    Request - WDFREQUEST representing this request.         
    
--*/
{
    PUCXFUNCTION_INFO       ucxFunctionInfo;
    WDFMEMORY               ucxFunctionInfoWdfMemory = NULL;
    NTSTATUS                status;
    ULONG                   numberOfFunctions;
    WDF_OBJECT_ATTRIBUTES   objectAttributes;
    ULONG                   index;
    KIRQL                   irql;

    TRY {
        
        if (NULL == RegisterCompositeDevice || NULL ==  UsbdFunctionHandles) {
            LogError(FlagUsbDevice, "RegisterCompositeDevice or UsbdFunctionHandles cant be NULL");
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        if (RegisterCompositeDevice->Size != sizeof(REGISTER_COMPOSITE_DEVICE)) {
            LogError(FlagUsbDevice, "RegisterCompositeDevice 0x%p Size Expected %d, Actual %d", 
                     RegisterCompositeDevice, sizeof(REGISTER_COMPOSITE_DEVICE), RegisterCompositeDevice->Size);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;                     
        }

        if (RegisterCompositeDevice->Version != REGISTER_COMPOSITE_DEVICE_VERSION_0100) {
            LogError(FlagUsbDevice, "RegisterCompositeDevice 0x%p Version Expected 0x100, Actual 0x%x",
                     RegisterCompositeDevice, RegisterCompositeDevice->Version);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }
        numberOfFunctions = RegisterCompositeDevice->FunctionCount;

        if (numberOfFunctions == 0  || numberOfFunctions > MAX_COMPOSITE_DRIVER_FUNCTION_COUNT) {
            LogError(FlagUsbDevice, "FunctionCount %d value invalid ", numberOfFunctions);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        WDF_OBJECT_ATTRIBUTES_INIT(&objectAttributes);
        objectAttributes.ParentObject = UcxUsbDeviceContext->UcxUsbDevice;

        status = WdfMemoryCreate(&objectAttributes,
                                 NonPagedPool,
                                 UCX_TAG,
                                 sizeof(UCXFUNCTION_INFO) * numberOfFunctions,
                                 &ucxFunctionInfoWdfMemory,
                                 &ucxFunctionInfo);

        if (!NT_SUCCESS(status)) {
            LogError(FlagUsbDevice, "Could not allocate memory for UCXFUNCTION_INFO");
            LEAVE;
        }

        RtlZeroMemory(ucxFunctionInfo, sizeof(UCXFUNCTION_INFO) * numberOfFunctions);
        
        KeAcquireSpinLock(&UcxUsbDeviceContext->RemoteWakeLock, &irql);
        
        if (NULL != UcxUsbDeviceContext->UcxFunctionInfoWdfMemory) {
            KeReleaseSpinLock(&UcxUsbDeviceContext->RemoteWakeLock, irql);
            LogError(FlagUsbDevice, "UcxFunctionInfoWdfMemory 0x%p is not NULL", 
                     UcxUsbDeviceContext->UcxFunctionInfoWdfMemory);
            status = STATUS_INVALID_DEVICE_STATE;
            LEAVE;
        }

        UcxUsbDeviceContext->NumberOfFunctions = numberOfFunctions;
        UcxUsbDeviceContext->UcxFunctionInfo = ucxFunctionInfo;
        UcxUsbDeviceContext->UcxFunctionInfoWdfMemory = ucxFunctionInfoWdfMemory;
        ucxFunctionInfoWdfMemory = NULL;
        KeReleaseSpinLock(&UcxUsbDeviceContext->RemoteWakeLock, irql);
        
        for (index = 0; index < numberOfFunctions; index++) {
            ucxFunctionInfo[index].Index = index;
            ucxFunctionInfo[index].Sig = SigUcxFunctionInfo;
            ucxFunctionInfo[index].UcxUsbDeviceContext = UcxUsbDeviceContext;

            UsbdFunctionHandles[index] = (USBD_FUNCTION_HANDLE) &ucxFunctionInfo[index];
        }

        status = STATUS_SUCCESS;

    } FINALLY {
        
        if (!NT_SUCCESS(status)) {
            if (ucxFunctionInfoWdfMemory != NULL) {
                WdfObjectDelete(ucxFunctionInfoWdfMemory);
            }
        }
        WdfRequestComplete(Request, status);
    }
}
   
VOID
UsbDevice_UnregisterCompositeDevice(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT       UcxUsbDeviceContext,
    __in
        WDFREQUEST                      Request
   )
/*++

Routine Description:

    This routine un-registers a composite driver that previously registered by calling the 
    UsbDevice_RegisterCompositeDevice function. 
    
    Upon calling this routine, all the function handles returned previously by the 
    UsbDevice_RegisterCompositeDevice become invalid.  

Arguments:

    UcxUsbDeviceContext - Ucx context for the UCXUSBDEVICE object
    
    RegisterCompositeDevice - Pointer to the REGISTER_COMPOSITE_DEVICE Structure.
    
    UsbdFunctionHandles - Pointer to the buffer where this function will fill the
         USBD_FUNCTION_HANDLESs
         
    Request - WDFREQUEST representing this request.         
    
--*/
{
    NTSTATUS               status;
    KIRQL                  irql;
    WDFMEMORY              ucxFunctionInfoWdfMemory;
    PUCXFUNCTION_INFO      ucxFunctionInfo;
    ULONG                  index;
    ULONG                  numberOfFunctions;

    TRY {
        
        KeAcquireSpinLock(&UcxUsbDeviceContext->RemoteWakeLock, &irql);
        
        if (NULL == UcxUsbDeviceContext->UcxFunctionInfoWdfMemory) {
            KeReleaseSpinLock(&UcxUsbDeviceContext->RemoteWakeLock, irql);
            LogError(FlagUsbDevice, "UcxUsbDeviceContext->UcxFunctionInfoWdfMemory is NULL");
            status = STATUS_INVALID_DEVICE_STATE;
            LEAVE;
        }

        numberOfFunctions = UcxUsbDeviceContext->NumberOfFunctions;

        ucxFunctionInfoWdfMemory = UcxUsbDeviceContext->UcxFunctionInfoWdfMemory;
        ucxFunctionInfo = UcxUsbDeviceContext->UcxFunctionInfo;
        
        UcxUsbDeviceContext->UcxFunctionInfoWdfMemory = NULL;
        UcxUsbDeviceContext->UcxFunctionInfo = NULL;
        UcxUsbDeviceContext->NumberOfFunctions = 0;

        KeReleaseSpinLock(&UcxUsbDeviceContext->RemoteWakeLock, irql);

        for (index = 0; index < numberOfFunctions; index++) {
            NT_ASSERT(NULL == 
                      ucxFunctionInfo[index].FunctionRemoteWakeRequestInfo.PendingRequest);
        }
        
        WdfObjectDelete(ucxFunctionInfoWdfMemory);

        status = STATUS_SUCCESS;

    } FINALLY {
        WdfRequestComplete(Request, status);
    }
}

VOID
UsbDevice_SetFunctionData(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT       UcxUsbDeviceContext,
    __in 
        PUSB_FUNCTION_DATA              UsbFunctionData,
    __in
        WDFREQUEST                      Request
   )
/*++

Routine Description:

    This routine sets information about a particular function of the Composite driver. 

Arguments:

    UcxUsbDeviceContext - Ucx context for the UCXUSBDEVICE object
    
    RegisterCompositeDevice - Pointer to the REGISTER_COMPOSITE_DEVICE Structure.
    
    UsbdFunctionHandles - Pointer to the buffer where this function will fill the
         USBD_FUNCTION_HANDLESs
         
    Request - WDFREQUEST representing this request.         
    
--*/
{
    NTSTATUS               status;
    KIRQL                  irql;
    PUCXFUNCTION_INFO      ucxFunctionInfo;
    TRY {

        KeAcquireSpinLock(&UcxUsbDeviceContext->RemoteWakeLock, &irql);

        ucxFunctionInfo = (PUCXFUNCTION_INFO) UsbFunctionData->UsbdFunctionHandle;
        ucxFunctionInfo->ClientPdo = UsbFunctionData->PhysicalDeviceObject;

        KeReleaseSpinLock(&UcxUsbDeviceContext->RemoteWakeLock, irql);

        status = STATUS_SUCCESS;

    } FINALLY {
        WdfRequestComplete(Request, status);
    }
}


VOID
UsbDevice_EvtRemoteWakeRequestCancel(
    WDFREQUEST Request
   )
/*++

Routine Description:

    This routine cancels the remote wake notifcation sent earlier for a particular function / 
    device. 
    
Arguments:

    Request - Remote wake WDFREQUEST that is being cancelled.         
    
--*/
{
    PUCXUSBDEVICE_PRIVCONTEXT                   ucxUsbDeviceContext;
    PREQUEST_REMOTE_WAKE_NOTIFICATION           remoteWakeNotification;
    PUCXFUNCTION_INFO                           ucxFunctionInfo;
    KIRQL                                       irql;
    UCXUSBDEVICE                                ucxUsbDevice;

    TRY {
        
        Ucx_WdfRequestGetArgs(Request, &remoteWakeNotification, &ucxUsbDevice, NULL);

        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);

        KeAcquireSpinLock(&ucxUsbDeviceContext->RemoteWakeLock, &irql);

        if (NULL == remoteWakeNotification->UsbdFunctionHandle) {
            NT_ASSERT(ucxUsbDeviceContext->UsbDeviceRemoteWakeRequestInfo.PendingRequest == Request);
            ucxUsbDeviceContext->UsbDeviceRemoteWakeRequestInfo.PendingRequest = NULL;
            ucxUsbDeviceContext->UsbDeviceRemoteWakeRequestInfo.RequestParams = NULL;
        
        } else {

            if (ucxUsbDeviceContext->UcxFunctionInfoWdfMemory == NULL) {
                //
                // This is a case where the client already de-registered, while a reqeust
                // was pending. Handle it gracefully
                //
            } else {
                ucxFunctionInfo = (PUCXFUNCTION_INFO)remoteWakeNotification->UsbdFunctionHandle;
                NT_ASSERT(ucxFunctionInfo->FunctionRemoteWakeRequestInfo.PendingRequest == Request);
                ucxFunctionInfo->FunctionRemoteWakeRequestInfo.PendingRequest = NULL;
                ucxFunctionInfo->FunctionRemoteWakeRequestInfo.RequestParams = NULL;
            }
        }

        KeReleaseSpinLock(&ucxUsbDeviceContext->RemoteWakeLock, irql);

    } FINALLY {

        WdfRequestComplete(Request, STATUS_CANCELLED);
    }
}

VOID
UsbDevice_RemoteWakeNotificationRequest(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT                 UcxUsbDeviceContext,
    __in 
        PREQUEST_REMOTE_WAKE_NOTIFICATION         RemoteWakeNotification,
    __in
        WDFREQUEST                                Request
   )
/*++

Routine Description:

    This routine handles the Remote Wake notifcation for a particular function / device. 
    
    This routine sets a cancel routine on this Request, and stores the request for a future 
    remote wake notification. 

Arguments:

    UcxUsbDeviceContext - Ucx context for the UCXUSBDEVICE object
    
    RemoteWakeNotificaiton - Pointer to the REQUEST_REMOTE_WAKE_NOTIFICATION Structure.
         
    Request - WDFREQUEST representing this request.         
    
--*/
{
    NTSTATUS                  status;
    KIRQL                     irql = 0;
    PUCXFUNCTION_INFO         ucxFunctionInfo;
    PREMOTE_WAKE_REQUEST_INFO remoteWakeRequestInfo;
    BOOLEAN                   locked = FALSE;
    TRY {
        
        if (NULL == RemoteWakeNotification) {
            status = STATUS_INVALID_PARAMETER;
            LogError(FlagUsbDevice,
                     "Invalid Request, 0x%p, Has a Null RemoteWakeNotification", Request);
            LEAVE;
        }

        KeAcquireSpinLock(&UcxUsbDeviceContext->RemoteWakeLock, &irql);
        locked = TRUE;
        

        if (NULL == UcxUsbDeviceContext->UcxFunctionInfoWdfMemory) {
        
            //
            // The device is probably not a multifunction device. 
            //

            if (NULL != RemoteWakeNotification->UsbdFunctionHandle) {
            
                status = STATUS_INVALID_PARAMETER;
                LogError(FlagUsbDevice, 
                         "UsbdFunctionHandle can only be specified for a Driver that registered itself as a Composite driver, Request 0x%p, Device Context 0x%p", 
                         Request, UcxUsbDeviceContext);
                LEAVE;
            }
            
            remoteWakeRequestInfo = &UcxUsbDeviceContext->UsbDeviceRemoteWakeRequestInfo;
            
        } else {

            //
            // The device is a multifunction device. 
            //
            
            if (NULL == RemoteWakeNotification->UsbdFunctionHandle) {
            
                status = STATUS_INVALID_PARAMETER;
                LogError(FlagUsbDevice, 
                         "UsbdFunctionHandle must be specified for a Driver that registered itself as a Composite driver, Request 0x%p, Device Context 0x%p", 
                         Request, UcxUsbDeviceContext);
                LEAVE;
            }

            ucxFunctionInfo = (PUCXFUNCTION_INFO) RemoteWakeNotification->UsbdFunctionHandle;        
            remoteWakeRequestInfo = &ucxFunctionInfo->FunctionRemoteWakeRequestInfo;

        }

        if (NULL != remoteWakeRequestInfo->PendingRequest) {
            status = STATUS_INVALID_DEVICE_STATE;
            LogError(FlagUsbDevice, 
                     "RemoteWakeNotification Request (0x%p) exists, so new one is invalid (0x%p), Device context 0x%p", 
                     remoteWakeRequestInfo->PendingRequest, 
                     Request, UcxUsbDeviceContext);
            LEAVE;
        }

        status = WdfRequestMarkCancelableEx(Request, UsbDevice_EvtRemoteWakeRequestCancel);

        if (!NT_SUCCESS(status)) {
            LogError(FlagUsbDevice, 
                     "WdfRequestMarkCancelableEx failed Device Context 0x%p, Request 0x%p, Status %!STATUS!",
                     UcxUsbDeviceContext, Request, status);
            LEAVE;
        }

        remoteWakeRequestInfo->PendingRequest = Request;
        remoteWakeRequestInfo->RequestParams = RemoteWakeNotification;

        KeReleaseSpinLock(&UcxUsbDeviceContext->RemoteWakeLock, irql);
        locked = FALSE;

        status = STATUS_SUCCESS;

    } FINALLY {
        if (locked) {
            KeReleaseSpinLock(&UcxUsbDeviceContext->RemoteWakeLock, irql);
        }

        if (!NT_SUCCESS(status)) {
            WdfRequestComplete(Request, status);
        }
    }
}

FORCEINLINE
VOID
UsbDevice_FailMgmtRequest(
    __in
        PUCXUSBDEVICE_PRIVCONTEXT UcxUsbDeviceContext,
    __in
        WDFREQUEST                Request,
    __in
        ULONG                     IoControlCode
    )
/*++

Routine Description:

    This event callback function fails the ioctls to manage the device.
    This is done basically on / after a controller reset operation.
    Some of these ioctls cannot be completed to the hub with a failure status.

    These include:
        IOCTL_INTERNAL_USB_USBDEVICE_ENABLE
        IOCTL_INTERNAL_USB_USBDEVICE_RESET
        IOCTL_INTERNAL_USB_USBDEVICE_ADDRESS
        IOCTL_INTERNAL_USB_USBDEVICE_UPDATE
        IOCTL_INTERNAL_USB_USBDEVICE_HUB_INFO
        IOCTL_INTERNAL_USB_USBDEVICE_DISABLE
        IOCTL_INTERNAL_USB_ENDPOINTS_CONFIGURE
        IOCTL_INTERNAL_USB_DEFAULT_ENDPOINT_UPDATE
        IOCTL_INTERNAL_USB_ENDPOINT_RESET
        IOCTL_INTERNAL_USB_USBDEVICE_PURGEIO
        IOCTL_INTERNAL_USB_USBDEVICE_STARTIO
        IOCTL_INTERNAL_USB_USBDEVICE_TREE_PURGEIO
        IOCTL_INTERNAL_USB_USBDEVICE_ABORTIO
        IOCTL_INTERNAL_USB_SUBMIT_URB:
            URB_FUNCTION_OPEN_STATIC_STREAMS & URB_FUNCTION_CLOSE_STATIC_STREAMS

Arguments:

    WdfRequest - A handle to the wdfrequest on this queue.

    UsbDeviceMgmtHeader - Pointer to the common header for all these ioctls.

    IoControlCode - Describles the Ioctl code.

Return Value:

    VOID

--*/
{
    PURB                              urb;
    PUCXPIPE_INFO                     endpointPipeInfo;
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext;

    TRY {

        switch (IoControlCode) {

        case IOCTL_INTERNAL_USB_USBDEVICE_PURGEIO:

            UsbDevice_PurgeFromHub(UcxUsbDeviceContext, Request, FALSE);
            break;

        case IOCTL_INTERNAL_USB_USBDEVICE_ABORTIO:

            UsbDevice_AbortFromHub(UcxUsbDeviceContext, Request);
            break;

        case IOCTL_INTERNAL_USB_USBDEVICE_TREE_PURGEIO:
            UsbDevice_TreePurgeFromHub(UcxUsbDeviceContext, Request);
            break;

        case IOCTL_INTERNAL_USB_SUBMIT_URB:
            urb = (PURB) Ucx_WdfRequestGetArg1(Request);

            if (urb->UrbHeader.Function == URB_FUNCTION_OPEN_STATIC_STREAMS) {

                endpointPipeInfo = urb->UrbPipeRequest.PipeHandle;
                ucxEndpointContext = UcxEndpointGetContextFromEndpointPipeInfo(endpointPipeInfo);
                
                NT_ASSERT(ucxEndpointContext->OpenStaticStreamsFailedDueToControllerReset == 0);
                ucxEndpointContext->OpenStaticStreamsFailedDueToControllerReset = 1;
                
                LogError(FlagUsbDevice,
                         "Cannot Open Streams due to Controller Reset 0x%p",  urb);
                WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);

            } else if (urb->UrbHeader.Function == URB_FUNCTION_CLOSE_STATIC_STREAMS) {

                endpointPipeInfo = urb->UrbPipeRequest.PipeHandle;
                ucxEndpointContext = UcxEndpointGetContextFromEndpointPipeInfo(endpointPipeInfo);
                NT_ASSERT(ucxEndpointContext->PendingStreamsDisableRequest == NULL);
                ucxEndpointContext->PendingStreamsDisableRequest = Request;
                Endpoint_EventAdd(ucxEndpointContext, UcxEndpointEventStaticStreamsDisableFromClient);

            } else {
                LogError(FlagController,
                         "An URB function %d was unexpected in this function",
                         urb->UrbHeader.Function);
                WdfRequestComplete(Request, STATUS_INVALID_DEVICE_REQUEST);
            }
            break;

        case IOCTL_INTERNAL_USB_USBDEVICE_RESET:

            NT_ASSERT(UcxUsbDeviceContext->UsbDeviceResetFailedDueToControllerReset == 0);
            UcxUsbDeviceContext->UsbDeviceResetFailedDueToControllerReset = 1;

            UsbDevice_ResetFromHub(UcxUsbDeviceContext, Request, TRUE);

            LEAVE;

        case IOCTL_INTERNAL_USB_USBDEVICE_DISABLE:

            UsbDevice_DisableFromHub(UcxUsbDeviceContext, Request, TRUE);
            break;

        case IOCTL_INTERNAL_USB_ENDPOINTS_CONFIGURE:

            UsbDevice_EndpointsConfigureFromHub(UcxUsbDeviceContext, Request, TRUE);
            break;

        case IOCTL_INTERNAL_USB_ENDPOINT_RESET:

            NT_ASSERT(UcxUsbDeviceContext->EndpointResetFailedDueToControllerReset == 0);
            UcxUsbDeviceContext->EndpointResetFailedDueToControllerReset = 1;
            WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
            break;

        default:
            WdfRequestComplete(Request, STATUS_NO_SUCH_DEVICE);
            break;

        }

    } FINALLY {

    }

}

VOID
UsbDevice_EvtTreePurgeIoInternalDeviceControl(
    WDFQUEUE                WdfQueue,
    WDFREQUEST              Request,
    size_t                  OutputBufferLength,
    size_t                  InputBufferLength,
    ULONG                   IoControlCode
    )
/*++

Routine Description:

    This event callback function handles the TreePurge IOCTL.

    We must forward the request to the normal device management queue, while preventing further,
    concurrent TreePurge IOCTLs.
    
        IOCTL_INTERNAL_USB_USBDEVICE_TREE_PURGEIO

Arguments:

    WdfQueue - A handle to the queue object that handles the above mentioned ioctls

    WdfRequest - A handle to the wdfrequest on this queue.

    OutputBufferLength - Not used

    InputBufferLength - Not used

    IoControlCode - Describles the Ioctl code.

Return Value:

    VOID

--*/
{

    UCXCONTROLLER                     ucxController;
    PUSBDEVICE_MGMT_HEADER            usbDeviceMgmtHeader = NULL;
    UCXUSBDEVICE                      ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT         ucxUsbDeviceContext;
    PUCXCONTROLLER_PRIVCONTEXT        ucxControllerContext;
    NTSTATUS                          status;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);
    
    TRY {
        NT_ASSERT(IoControlCode == IOCTL_INTERNAL_USB_USBDEVICE_TREE_PURGEIO);

        usbDeviceMgmtHeader = (PUSBDEVICE_MGMT_HEADER) Ucx_WdfRequestGetArg1(Request); 
        ucxUsbDevice = usbDeviceMgmtHeader->UsbDevice;

        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
        ucxControllerContext = ucxUsbDeviceContext->UcxControllerContext;
        ucxController = ucxControllerContext->UcxController;

        LogVerbose(FlagController, "%!UCX_IOCTL! WdfRequest 0x%p, UcxController 0x%p, UcxDevice 0x%p", 
                   IoControlCode,  Request, ucxController, ucxUsbDevice);

        switch (IoControlCode) {
        case IOCTL_INTERNAL_USB_USBDEVICE_TREE_PURGEIO:

            //
            // This request is serialized: We must not handle further requests on this queue until
            // we complete this request. However, the main handling is on QueueUsbDeviceMgmt since
            // that queue limits the number of concurrent calls into the HCD. If we didn't stop the
            // tree purge queue for the duration of the request, another tree purge request would
            // pass right through here and be handled immediately (if QueueUsbDeviceMgmt handles
            // more than 1 concurrent request).
            //
            WdfIoQueueStop(WdfQueue, NULL, NULL);

            status = WdfRequestForwardToIoQueue(Request, ucxControllerContext->QueueUsbDeviceMgmt);

            if (!NT_SUCCESS(status)) {

                //
                // Return success status (failure not allowed for this IOCTL) and restart the queue
                // Note: reference to _IOCTL_INTERNAL_USB_USBDEVICE_TREE_PURGEIO to keep the type 
                // information in the symbol file.
                //
                LogError(FlagController,
                         "%!UCX_IOCTL! failed fwd to DeviceMgmt, UcxController 0x%p, UcxDevice 0x%p",
                         _IOCTL_INTERNAL_USB_USBDEVICE_TREE_PURGEIO, ucxController, ucxUsbDevice);

                WdfRequestComplete(Request, STATUS_SUCCESS);
                WdfIoQueueStart(WdfQueue);
            }

            break;

        default:
            LogWarning(FlagController, "A IOCTL %d was unexpected in this function", IoControlCode);
            WdfRequestComplete(Request, STATUS_INVALID_DEVICE_REQUEST);
            break;
        }

    } FINALLY {

    }

    return;
}

VOID
UsbDevice_EvtMgmtIoInternalDeviceControl(
    WDFQUEUE                WdfQueue,
    WDFREQUEST              Request,
    size_t                  OutputBufferLength,
    size_t                  InputBufferLength,
    ULONG                   IoControlCode
    )
/*++

Routine Description:

    This event callback function handles the ioctls to manage the device.
    This routine basically deliver's the ioctls to the the controller driver 
    via correct callback.
    
    These include: 
        IOCTL_INTERNAL_USB_USBDEVICE_ENABLE
        IOCTL_INTERNAL_USB_USBDEVICE_RESET
        IOCTL_INTERNAL_USB_USBDEVICE_ADDRESS
        IOCTL_INTERNAL_USB_USBDEVICE_UPDATE
        IOCTL_INTERNAL_USB_USBDEVICE_HUB_INFO
        IOCTL_INTERNAL_USB_USBDEVICE_DISABLE
        IOCTL_INTERNAL_USB_ENDPOINTS_CONFIGURE
        IOCTL_INTERNAL_USB_DEFAULT_ENDPOINT_UPDATE
        IOCTL_INTERNAL_USB_ENDPOINT_RESET
        IOCTL_INTERNAL_USB_USBDEVICE_PURGEIO
        IOCTL_INTERNAL_USB_USBDEVICE_STARTIO
        IOCTL_INTERNAL_USB_USBDEVICE_TREE_PURGEIO
        IOCTL_INTERNAL_USB_USBDEVICE_ABORTIO
        IOCTL_INTERNAL_USB_SUBMIT_URB: 
            URB_FUNCTION_OPEN_STATIC_STREAMS & URB_FUNCTION_CLOSE_STATIC_STREAMS
    
Arguments:

    WdfQueue - A handle to the queue object that handles the above mentioned ioctls

    WdfRequest - A handle to the wdfrequest on this queue.

    OutputBufferLength - Not used
    
    InputBufferLength - Not used
    
    IoControlCode - Describles the Ioctl code.

Return Value:

    VOID

--*/
{

    UCXCONTROLLER                     ucxController;
    UCXENDPOINT                       ucxEndpoint;
    PUSBDEVICE_MGMT_HEADER            usbDeviceMgmtHeader = NULL;
    UCXUSBDEVICE                      ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT         ucxUsbDeviceContext;
    PUCXCONTROLLER_PRIVCONTEXT        ucxControllerContext;
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext;
    PUSBDEVICE_UPDATE                 usbDeviceUpdate;
    PENDPOINT_RESET                   endpointReset;
    BOOLEAN                           resetReferenceAcquired;
    PUSBDEVICE_PURGEIO                usbDevicePurgeIo;
    PUSBDEVICE_ABORTIO                usbDeviceAbortIo;
    PUSBDEVICE_TREE_PURGEIO           usbDeviceTreePurgeIo;
    PUSBDEVICE_STARTIO                usbDeviceStartIo;
    PURB                              urb = NULL;
    PUCXPIPE_INFO                     endpointPipeInfo;

    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(WdfQueue);
    
    TRY {
        NT_ASSERT (
            IoControlCode == IOCTL_INTERNAL_USB_USBDEVICE_ENABLE ||
            IoControlCode == IOCTL_INTERNAL_USB_USBDEVICE_RESET ||
            IoControlCode == IOCTL_INTERNAL_USB_USBDEVICE_ADDRESS ||
            IoControlCode == IOCTL_INTERNAL_USB_USBDEVICE_UPDATE ||
            IoControlCode == IOCTL_INTERNAL_USB_USBDEVICE_HUB_INFO ||
            IoControlCode == IOCTL_INTERNAL_USB_USBDEVICE_DISABLE ||
            IoControlCode == IOCTL_INTERNAL_USB_ENDPOINTS_CONFIGURE ||
            IoControlCode == IOCTL_INTERNAL_USB_DEFAULT_ENDPOINT_UPDATE ||
            IoControlCode == IOCTL_INTERNAL_USB_ENDPOINT_RESET ||
            IoControlCode == IOCTL_INTERNAL_USB_USBDEVICE_PURGEIO ||
            IoControlCode == IOCTL_INTERNAL_USB_USBDEVICE_STARTIO ||
            IoControlCode == IOCTL_INTERNAL_USB_USBDEVICE_TREE_PURGEIO ||
            IoControlCode == IOCTL_INTERNAL_USB_USBDEVICE_ABORTIO ||
            IoControlCode == IOCTL_INTERNAL_USB_SUBMIT_URB
           );

        if (IoControlCode == IOCTL_INTERNAL_USB_SUBMIT_URB) {
            urb = (PURB) Ucx_WdfRequestGetArg1(Request);
            ucxUsbDevice = (UCXUSBDEVICE) urb->UrbHeader.UsbdDeviceHandle;
        } else {
            usbDeviceMgmtHeader = (PUSBDEVICE_MGMT_HEADER) Ucx_WdfRequestGetArg1(Request); 
            ucxUsbDevice = usbDeviceMgmtHeader->UsbDevice;
        }

        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);
        ucxControllerContext = ucxUsbDeviceContext->UcxControllerContext;
        ucxController = ucxControllerContext->UcxController;

        resetReferenceAcquired = Controller_ResetReferenceAcquire(ucxControllerContext);

        if (FALSE == resetReferenceAcquired) {
            
            LogInfo(FlagController, "Ignoring %!UCX_IOCTL! due to controller reset, WdfRequest 0x%p, UcxController 0x%p, UcxDevice 0x%p", 
                    IoControlCode,  Request, ucxController, 
                    ucxUsbDevice);
            UsbDevice_FailMgmtRequest(ucxUsbDeviceContext, 
                                      Request, 
                                      IoControlCode);
            LEAVE;        
        }

        if (ucxUsbDeviceContext->DeprogrammedDueToControllerReset) {

            //
            // If the ucxUsbDeviceContext got deprogrammed, only ioctl we will handle is 
            // IOCTL_INTERNAL_USB_USBDEVICE_ENABLE
            //
            if (IoControlCode == IOCTL_INTERNAL_USB_USBDEVICE_ENABLE) {

                LogInfo(FlagUsbDevice, "Clearing DeprogrammedDueToControllerReset for UcxDevice 0x%p, Context 0x%p",
                        ucxUsbDevice, ucxUsbDeviceContext);
                ucxUsbDeviceContext->DeprogrammedDueToControllerReset = FALSE;
            
            } else {
                //
                // Fail this IOCTL
                //
                LogInfo(FlagController, "Ignoring %!UCX_IOCTL! since the device has been deprogrammed, WdfRequest 0x%p, UcxController 0x%p, UcxDevice 0x%p", 
                        IoControlCode,  Request, ucxController, 
                        ucxUsbDevice);
                UsbDevice_FailMgmtRequest(ucxUsbDeviceContext,
                                          Request, 
                                          IoControlCode);
                LEAVE;
            }
        
        }

        LogVerbose(FlagController, "%!UCX_IOCTL! WdfRequest 0x%p, UcxController 0x%p, UcxDevice 0x%p", 
                   IoControlCode,  Request, ucxController, ucxUsbDevice);

        switch (IoControlCode) {
        case IOCTL_INTERNAL_USB_USBDEVICE_PURGEIO: 
            usbDevicePurgeIo = (PUSBDEVICE_PURGEIO) Ucx_WdfRequestGetArg1(Request);
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(usbDevicePurgeIo->UsbDevice);
            UsbDevice_PurgeFromHub(ucxUsbDeviceContext, Request, usbDevicePurgeIo->OnSuspend);
            break;

        case IOCTL_INTERNAL_USB_USBDEVICE_ABORTIO: 
            usbDeviceAbortIo = (PUSBDEVICE_ABORTIO) Ucx_WdfRequestGetArg1(Request);
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(usbDeviceAbortIo->UsbDevice);
            UsbDevice_AbortFromHub(ucxUsbDeviceContext, Request);
            break;

        case IOCTL_INTERNAL_USB_USBDEVICE_TREE_PURGEIO: 
            usbDeviceTreePurgeIo = (PUSBDEVICE_TREE_PURGEIO) Ucx_WdfRequestGetArg1(Request);
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(usbDeviceTreePurgeIo->UsbDevice);
            UsbDevice_TreePurgeFromHub(ucxUsbDeviceContext, Request);
            break;

        case IOCTL_INTERNAL_USB_USBDEVICE_STARTIO:
            usbDeviceStartIo = (PUSBDEVICE_STARTIO) Ucx_WdfRequestGetArg1(Request);
            ucxUsbDeviceContext = UcxUsbDeviceGetContext(usbDeviceStartIo->UsbDevice);
            UsbDevice_StartFromHub(ucxUsbDeviceContext, Request);
            break;

        case IOCTL_INTERNAL_USB_SUBMIT_URB: 
            
            if (urb->UrbHeader.Function == URB_FUNCTION_OPEN_STATIC_STREAMS) {
                endpointPipeInfo = urb->UrbOpenStaticStreams.PipeHandle;
                ucxEndpointContext = UcxEndpointGetContextFromEndpointPipeInfo(endpointPipeInfo);
                NT_ASSERT(ucxEndpointContext->PendingStreamsEnableRequest == NULL);
                ucxEndpointContext->PendingStreamsEnableRequest = Request;
                Endpoint_EventAdd(ucxEndpointContext, UcxEndpointEventStaticStreamsEnableFromClient);

            } else if (urb->UrbHeader.Function == URB_FUNCTION_CLOSE_STATIC_STREAMS) {
                
                endpointPipeInfo = urb->UrbPipeRequest.PipeHandle;
                ucxEndpointContext = UcxEndpointGetContextFromEndpointPipeInfo(endpointPipeInfo);
                NT_ASSERT(ucxEndpointContext->PendingStreamsDisableRequest == NULL);
                ucxEndpointContext->PendingStreamsDisableRequest = Request;
                Endpoint_EventAdd(ucxEndpointContext, UcxEndpointEventStaticStreamsDisableFromClient);

            } else {
                LogError(FlagController, 
                         "An URB function %d was unexpected in this function", 
                         urb->UrbHeader.Function);
                WdfRequestComplete(Request, STATUS_INVALID_DEVICE_REQUEST);
            }
            break;

        case IOCTL_INTERNAL_USB_USBDEVICE_RESET: 
            UsbDevice_ResetFromHub(ucxUsbDeviceContext, Request, FALSE);
            break;
        
        case IOCTL_INTERNAL_USB_USBDEVICE_ADDRESS: 
            ucxUsbDeviceContext->EvtCallbacks.EvtUsbDeviceAddress(ucxController, Request);
            break;
        
        case IOCTL_INTERNAL_USB_USBDEVICE_UPDATE: 
            usbDeviceUpdate = (PUSBDEVICE_UPDATE) usbDeviceMgmtHeader;            
            if ((usbDeviceUpdate->Flags.UpdateIsHub == 1) && usbDeviceUpdate->IsHub) {
                ucxUsbDeviceContext->UsbDeviceType = UsbDeviceTypeHub;
            }
            
            if (usbDeviceUpdate->Flags.UpdateAllowIoOnInvalidPipeHandles == 1) {
                UsbDevice_EnableInvalidHandleTracking(ucxUsbDeviceContext);
            }

            if (usbDeviceUpdate->Flags.UpdateDeviceDescriptor == 1) {
                ucxUsbDeviceContext->UsbVersionNumber = usbDeviceUpdate->DeviceDescriptor->bcdUSB;
            }

            ucxUsbDeviceContext->EvtCallbacks.EvtUsbDeviceUpdate(ucxController, Request);
            break;
        
        case IOCTL_INTERNAL_USB_USBDEVICE_HUB_INFO: 
            ucxUsbDeviceContext->EvtCallbacks.EvtUsbDeviceHubInfo(ucxController, Request);
            break;
        
        case IOCTL_INTERNAL_USB_DEFAULT_ENDPOINT_UPDATE: 
            ucxEndpointContext = ucxUsbDeviceContext->DefaultEndpointContext;
            ucxEndpointContext->EvtCallbacks.EvtDefaultEndpointUpdate(ucxController, Request);
            break;
        
        case IOCTL_INTERNAL_USB_ENDPOINT_RESET: 
            //
            // We need to proces this request, before we can forward it
            // to the HCD
            //
            endpointReset = (PENDPOINT_RESET) usbDeviceMgmtHeader;
            ucxEndpoint = endpointReset->Endpoint;
            ucxEndpointContext = UcxEndpointGetContext(ucxEndpoint);
            
            NT_ASSERT(ucxEndpointContext->PendingResetRequest == NULL);
            ucxEndpointContext->PendingResetRequest = Request;
            Endpoint_EventAdd(ucxEndpointContext, UcxEndpointEventEndpointResetFromHub);
            break;
        
        case IOCTL_INTERNAL_USB_USBDEVICE_ENABLE: 
            ucxUsbDeviceContext->EvtCallbacks.EvtUsbDeviceEnable(ucxController, Request);
            break;
        
        case IOCTL_INTERNAL_USB_USBDEVICE_DISABLE:  
            //
            // We need to proces this request, before we can forward it
            // to the HCD
            //
            UsbDevice_DisableFromHub(ucxUsbDeviceContext, Request, FALSE);
            break;

        case IOCTL_INTERNAL_USB_ENDPOINTS_CONFIGURE: 
            //
            // We need to proces this request, before we can forward it
            // to the HCD
            //
            UsbDevice_EndpointsConfigureFromHub(ucxUsbDeviceContext, Request, FALSE);
            break;
        
        default:
            LogWarning(FlagController, "A IOCTL %d was unexpected in this function", IoControlCode);
            WdfRequestComplete(Request, STATUS_INVALID_DEVICE_REQUEST);
            break;
        }
    } FINALLY {

        if (resetReferenceAcquired) {
            Controller_ResetReferenceRelease(ucxControllerContext);
        }

    }

    return;
}

VOID
UsbDevice_EvtFailMgmtIoInternalDeviceControl(
    WDFQUEUE                WdfQueue,
    WDFREQUEST              Request,
    size_t                  OutputBufferLength,
    size_t                  InputBufferLength,
    ULONG                   IoControlCode
    )
/*++

Routine Description:

Arguments:

    WdfQueue - A handle to the queue object that handles the above mentioned ioctls

    WdfRequest - A handle to the wdfrequest on this queue.

    OutputBufferLength - Not used
    
    InputBufferLength - Not used
    
    IoControlCode - Describles the Ioctl code.

Return Value:

    VOID

--*/
{

    PENDPOINTS_CONFIGURE              endpointsConfigure;
    NTSTATUS                          completionStatus;
    PURB                              urb;
    UCXUSBDEVICE                      ucxUsbDevice;
    PUCXUSBDEVICE_PRIVCONTEXT         ucxUsbDeviceContext;
    
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);
    UNREFERENCED_PARAMETER(WdfQueue);
    
    TRY {
        NT_ASSERT (
            IoControlCode == IOCTL_INTERNAL_USB_USBDEVICE_RESET ||
            IoControlCode == IOCTL_INTERNAL_USB_USBDEVICE_DISABLE ||
            IoControlCode == IOCTL_INTERNAL_USB_ENDPOINTS_CONFIGURE ||
            IoControlCode == IOCTL_INTERNAL_USB_SUBMIT_URB
           );

        switch (IoControlCode) {
        case IOCTL_INTERNAL_USB_USBDEVICE_RESET: 
            LogInfo(FlagUsbDevice, "Complete Reset Request due to controller reset 0x%p", Request);

            completionStatus = STATUS_SUCCESS;
            break;
        
        case IOCTL_INTERNAL_USB_USBDEVICE_DISABLE:  
            LogInfo(FlagUsbDevice, "Completing UsbDevice Disable Request due to controller reset 0x%p", Request);
            completionStatus = STATUS_SUCCESS;
            break;

        case IOCTL_INTERNAL_USB_ENDPOINTS_CONFIGURE: 
            LogInfo(FlagUsbDevice, "Completing HCD Endpoints Configuire Request due to controller reset, 0x%p", Request);
            
            endpointsConfigure = (PENDPOINTS_CONFIGURE) Ucx_WdfRequestGetArg1(Request);
            
            if (endpointsConfigure->EndpointsToEnableCount != 0) {
                completionStatus = STATUS_NO_SUCH_DEVICE;
            } else {
                completionStatus = STATUS_SUCCESS;
            }
            break;
        
        case IOCTL_INTERNAL_USB_SUBMIT_URB:
            //
            // Disable Static Streams 
            //
            urb = (PURB) Ucx_WdfRequestGetArg1(Request);

            if (urb->UrbHeader.Function == URB_FUNCTION_CLOSE_STATIC_STREAMS) {

                LogInfo(FlagUsbDevice, "Completing CLOSE Static Streams due to controller reset, 0x%p", Request);
                
                ucxUsbDevice = (UCXUSBDEVICE) urb->UrbHeader.UsbdDeviceHandle;
                ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);

                if (ucxUsbDeviceContext->FailCloseStaticStreams) {
                    completionStatus = STATUS_NO_SUCH_DEVICE;
                    ucxUsbDeviceContext->FailCloseStaticStreams = 0;
                } else {
                    completionStatus = STATUS_SUCCESS;
                }

            } else {
                
                LogError(FlagController, 
                         "An URB function %d was unexpected in this function", 
                         urb->UrbHeader.Function);
                
                completionStatus = STATUS_NO_SUCH_DEVICE;
                NT_ASSERTMSG("Unexpected Urb", FALSE);
            }
            
            break;

        default:
            LogWarning(FlagController, "A IOCTL %d was unexpected in this function", IoControlCode);
            NT_ASSERTMSG("Unexpected Ioctl", FALSE);
            completionStatus = STATUS_INVALID_DEVICE_REQUEST;
            break;
        }

    } FINALLY {

        WdfRequestComplete(Request, completionStatus);

    }

    return;
}

VOID
UsbDevice_AddEventForEndpoints(
    __in_ecount(EndpointCount)
        UCXENDPOINT*                                     EndpointsArray,
    __in
        ULONG                                            EndpointCount,
    __in 
        SM_ENGINE_EVENT                                  Event
)
/*++

Routine Description:

    This routine schedule asynchronous operation for endpoints of a UsbDevice
        
Arguments:
    
    UcxUsbDeviceContext - Context of the usbdevice
    
    Event - Event to be scheduled for each endpoint of the usbdevice. 
    
    EndpointsArray - Array of endpoints
    
    EndpointCount - Number of Endpoints in the array.
    
Return Value:

    VOID
    
--*/
{

    ULONG                             index;
    UCXENDPOINT                       ucxEndpoint;
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext;

    TRY {

        for (index = 0; index < EndpointCount; index++) {

            ucxEndpoint = EndpointsArray[index];
            ucxEndpointContext = UcxEndpointGetContext(ucxEndpoint);

            Endpoint_EventAdd(ucxEndpointContext, Event);

        }
        
    } FINALLY {

    }
    return;
}

#define MAXIMUM_FORWARD_PROGRESS_TRANSFER_SIZE   (512*1024)

NTSTATUS
UsbDevice_EnableForwardProgress(
    __in 
        PUCXUSBDEVICE_PRIVCONTEXT                 UcxUsbDeviceContext,
    __in
        PUSB_FORWARD_PROGRESS_INFO                ForwardProgressInfo
    )
/*
This routine enables forward progress for a device. 

This routine must be called with a controller level EnableForwardProgressMutex held
*/
{
    NTSTATUS                          status;
    PUCXPIPE_INFO                     defaultPipe;
    PUCXPIPE_INFO                     pipe;
    PUCXENDPOINT_PRIVCONTEXT          ucxEndpointContext;
    ULONG                             index;

    TRY {

        defaultPipe = &UcxUsbDeviceContext->DefaultEndpointContext->UcxPipeInfo;

        //
        // Validate the ControlPipeMaxForwardProgressTransferSize
        //
        if (ForwardProgressInfo->ControlPipeMaxForwardProgressTransferSize 
            > MAXIMUM_FORWARD_PROGRESS_TRANSFER_SIZE) {
            LogError(FlagUsbDevice, "FPInfo %p, too large ControlPipeMaxForwardProgressTransferSize %d", ForwardProgressInfo, ForwardProgressInfo->ControlPipeMaxForwardProgressTransferSize);
            status = STATUS_INVALID_PARAMETER;
            LEAVE;
        }

        //
        // First Ensure that pipe is not already provisioned for forward progress.
        //

        if (ForwardProgressInfo->ControlPipeMaxForwardProgressTransferSize != 0) {
            if (defaultPipe->ForwardProgressEnabled) {
                //
                // Default pipe is already provisioned for forward progress
                //
                LogError(FlagUsbDevice, 
                         "FPInfo %p, Pipe %p is already provisioned for forward progress", ForwardProgressInfo, defaultPipe);
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }
        }

        for (index = 0; index < ForwardProgressInfo->NumberOfPipes; index++) {
            
            //
            // For each pipe Validate MaxForwardProgressTransferSize
            //
            if ((ForwardProgressInfo->Pipes[index].MaxForwardProgressTransferSize == 0) ||
                (ForwardProgressInfo->Pipes[index].MaxForwardProgressTransferSize >
                 MAXIMUM_FORWARD_PROGRESS_TRANSFER_SIZE)) {
                LogError(FlagUsbDevice, "FPInfo %p ->Pipes[%d].MaxForwardProgressTransferSize %d is invalid", ForwardProgressInfo, index, ForwardProgressInfo->Pipes[index].MaxForwardProgressTransferSize);
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }
            
            //
            // Ensure that pipe is not already provisioned for forward progress.
            //

            pipe = (PUCXPIPE_INFO) ForwardProgressInfo->Pipes[index].PipeHandle;
            if (pipe->ForwardProgressEnabled) {
                
                //
                // pipe is already provisioned for forward progress
                //
                LogError(FlagUsbDevice, 
                         "FPInfo %p, Pipe %p is already provisioned for forward progress", ForwardProgressInfo, pipe);
                status = STATUS_INVALID_PARAMETER;
                LEAVE;
            }
        }

        if (ForwardProgressInfo->ControlPipeMaxForwardProgressTransferSize != 0) {
            if ((defaultPipe->ForwardProgressResourcesAllocatedByHcd == TRUE) &&
                (defaultPipe->ForwardProgressMaxTransferSize <= ForwardProgressInfo->ControlPipeMaxForwardProgressTransferSize)) {
                //
                // Do Nothing. The forward progress resources are already allocated.
                //
            } else {
                
                ucxEndpointContext = UcxEndpointGetContextFromEndpointPipeInfo(defaultPipe);

                if (NULL == ucxEndpointContext->EvtCallbacks.EvtEndpointEnableForwardProgress) {
                    LogError(FlagUsbDevice,
                             "FPInfo %p, Pipe %p doesnt support Forward progress", ForwardProgressInfo, defaultPipe);
                    status = STATUS_NOT_SUPPORTED;
                    LEAVE;
                }
                status = ucxEndpointContext->EvtCallbacks.EvtEndpointEnableForwardProgress(
                    ucxEndpointContext->UcxEndpoint,
                    ForwardProgressInfo->ControlPipeMaxForwardProgressTransferSize);

                if (!NT_SUCCESS(status)) {
                    LogError(FlagUsbDevice,
                             "FPInfo %p, Pipe %p couldnt be provisioned %!STATUS!", ForwardProgressInfo, defaultPipe, status);
                    LEAVE;
                }

                defaultPipe->ForwardProgressResourcesAllocatedByHcd = TRUE;
                defaultPipe->ForwardProgressMaxTransferSize = ForwardProgressInfo->ControlPipeMaxForwardProgressTransferSize;
            }
        }

        for (index = 0; index < ForwardProgressInfo->NumberOfPipes; index++) {
            pipe = (PUCXPIPE_INFO) ForwardProgressInfo->Pipes[index].PipeHandle;
        
            ucxEndpointContext = UcxEndpointGetContextFromEndpointPipeInfo(pipe);

            if (NULL == ucxEndpointContext->EvtCallbacks.EvtEndpointEnableForwardProgress) {
                LogError(FlagUsbDevice,
                         "FPInfo %p, Pipe %p doesnt support Forward progress", ForwardProgressInfo, pipe);
                status = STATUS_NOT_SUPPORTED;
                LEAVE;
            }
            
            status = ucxEndpointContext->EvtCallbacks.EvtEndpointEnableForwardProgress(
                ucxEndpointContext->UcxEndpoint,
                ForwardProgressInfo->Pipes[index].MaxForwardProgressTransferSize);

            if (!NT_SUCCESS(status)) {
                LogError(FlagUsbDevice,
                         "FPInfo %p, Pipe %p couldnt be provisioned %!STATUS!", ForwardProgressInfo, pipe, status);
                LEAVE;
            }

            pipe->ForwardProgressResourcesAllocatedByHcd = TRUE;
            pipe->ForwardProgressMaxTransferSize = ForwardProgressInfo->Pipes[index].MaxForwardProgressTransferSize;
        }
        
        //
        // Set the ForwardProgressEnabled for each endpoint. Dont add a failure path after this 
        // point
        //
        if (ForwardProgressInfo->ControlPipeMaxForwardProgressTransferSize != 0) {
            defaultPipe->ForwardProgressEnabled = TRUE;
        }
        for (index = 0; index < ForwardProgressInfo->NumberOfPipes; index++) {
            pipe = (PUCXPIPE_INFO) ForwardProgressInfo->Pipes[index].PipeHandle;
            pipe->ForwardProgressEnabled = TRUE;
        }
        //
        // DO NOT ADD A FAILURE PATH AFTER THIS.. IF YOU DO, YOU NEED TO reset teh 
        // ForwardProgressEnabled back to false. 
        //

        status = STATUS_SUCCESS;

    } FINALLY {

    }
    
    return status;
}

__drv_maxIRQL(DISPATCH_LEVEL)
BOOLEAN
UsbDevice_IsDeviceDisconnected(
    __in 
        UCXUSBDEVICE       Device
)
/*++

Routine Description:

    This Query interface function is called by the hub driver to find
    out if the device or one of its parent hubs have been reported 
    disconnected by the hub. 

Arguments:

    Device - The handle to the UCXUSBDEVICE representing the hub that called this 
        function

Comments:
    

--*/
{
    PUCXUSBDEVICE_PRIVCONTEXT         usbDeviceContext;  

    TRY {
        
        usbDeviceContext = UcxUsbDeviceGetContext(Device);

    } FINALLY {

    }

    return usbDeviceContext->Disconnected;
}
