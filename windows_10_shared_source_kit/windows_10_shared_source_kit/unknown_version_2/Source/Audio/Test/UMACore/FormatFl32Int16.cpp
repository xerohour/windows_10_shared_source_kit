 regardless of whether ObReferenceObjectByHandle
            // failed / succeeded.
            //
            ZwClose(systemThread);

            if (!NT_SUCCESS(status)) {

                LogError(FlagController, "ObReferenceObjectByHandle Failed %!STATUS!", status);

                UcxControllerContext->SystemThread = NULL;
                LEAVE;
            }
        
        }

        //
        // Inform the Hcd that forward progress should be enabled at the controller level.
        // Forward progress can get enabled for more than one device hosted by this
        // controller, but the Hcd should not be informed more than once.
        //
        if ((UcxControllerContext->ForwardProgressEnabledInHcd == FALSE) &&
            (UcxControllerContext->Config.EvtControllerEnableForwardProgress != NULL)) {

            status = UcxControllerContext->Config.EvtControllerEnableForwardProgress(
                         UcxControllerContext->UcxController);

            if (!NT_SUCCESS(status)) {
                LogError(FlagController,
                         "EvtControllerEnableForwardProgress failed for controller context(0x%p), status %!STATUS!",
                         UcxControllerContext, status);
                LEAVE;
            }

            UcxControllerContext->ForwardProgressEnabledInHcd = TRUE;
        }

        ucxUsbDevice = ForwardProgressInfo->DeviceHandle;
        ucxUsbDeviceContext = UcxUsbDeviceGetContext(ucxUsbDevice);

        if (IsTypeRootHub(ucxUsbDeviceContext->UsbDeviceType)) {
            status = RootHub_EnableForwardProgress(UCXUSBDEVICE_TO_UCXROOTHUB(ucxUsbDevice),
                                                   ForwardProgressInfo);
        } else {
            status = UsbDevice_EnableForwardProgress(ucxUsbDeviceContext, ForwardProgressInfo);
        }

        if (!NT_SUCCESS(status)) {
            LogError(FlagController, "UsbDevice_EnableForwardProgress Failed %!STATUS!", status);
            LEAVE;
        }

    } FINALLY {
        
        if (mutexAcquired) {
            KeReleaseMutex(&UcxControllerContext->EnableForwardProgressMutex, FALSE);
        }

    }
    return status;
}


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_PrepareUsbDevicesAndEndpointsForControllerReset(
    __in 
        PUCXCONTROLLER_PRIVCONTEXT       UcxControllerContext
)
/*++

Routine Description:

    This routine is called by the controller reset state machine to inform all devices of 
    the controller reset process. 

Arguments:

    UcxControllerContext - The Ucx Controller Context
    
--*/
{
    USBDEVICE_CONTROLLER_RESET_CALLBACK_CONTEXT  controllerResetCallbackContext;
    PUCXENDPOINT_PRIVCONTEXT                     currUcxEndpointContext;
    PUCXENDPOINT_PRIVCONTEXT                     nextUcxEndpointContext;
    KIRQL                                        irql;

    TRY {
        
        USBDEVICE_CONTROLLER_RESET_CALLBACK_CONTEXT_INIT(&controllerResetCallbackContext);

        KeAcquireSpinLock(&UcxControllerContext->TopologyLock, &irql);
        
        //
        // Walk the tree of all devices downstream of this disconnected device
        // (including the disconnected device), and build a list of endpoints
        //
        UsbDevice_WalkTree(UcxControllerContext->UcxRootHubContext->UcxUsbDeviceContextOfUcxRootHub,
                           TRUE,
                           UsbDevice_PrepareForControllerResetCallback,
                           &controllerResetCallbackContext);

        KeReleaseSpinLock(&UcxControllerContext->TopologyLock, irql);

        // For each endpoint in the temporary list, remove it from the list
        // call EndpointPurgeOnDisconnect, reference will be released when the 
        // EndpointPurgeOnDisconnect operation competes asynchronously

        NT_ASSERT(UcxControllerContext->PendingPrepareForResetOperations == 0);
        UcxControllerContext->PendingPrepareForResetOperations 
            = controllerResetCallbackContext.EndpointCount;

        FOR_ALL_IN_LIST_SAFE(UCXENDPOINT_PRIVCONTEXT, 
                             &controllerResetCallbackContext.EndpointList,
                             ControllerResetListEntryTemp,
                             currUcxEndpointContext,
                             nextUcxEndpointContext) {

            RemoveEntryList(&currUcxEndpointContext->ControllerResetListEntryTemp);

            InitializeListEntry(&currUcxEndpointContext->ControllerResetListEntryTemp);

            Endpoint_EventAdd(currUcxEndpointContext, UcxEndpointEventPrepareForControllerReset);

            WdfObjectDereferenceWithTag(currUcxEndpointContext->UcxEndpoint,
                                        TAG(UsbDevice_PrepareForControllerResetCallback));

        }

        if (controllerResetCallbackContext.EndpointCount == 0) {
            //
            // There were no endpoints, manually queue the ready event in the state machine
            //
            Controller_EventAdd(UcxControllerContext,
                                ControllerResetEventUsbDevicesAndEndpointsReadyForControllerReset);
        }
            
        
    } FINALLY {

    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
Controller_NotifyUsbDevicesAndEndpointsOfControllerResetComplete(
    __in 
        PUCXCONTROLLER_PRIVCONTEXT       UcxControllerContext
)
/*++

Routine Description:

    This routine is called by the controller reset state machine to inform all devices of 
    the completion of controller reset process. 

Arguments:

    UcxControllerContext - The Ucx Controller Context
    
--*/
{
    USBDEVICE_CONTROLLER_RESET_CALLBACK_CONTEXT  controllerResetCallbackContext;
    PUCXENDPOINT_PRIVCONTEXT                     currUcxEndpointContext;
    PUCXENDPOINT_PRIVCONTEXT                     nextUcxEndpointContext;
    KIRQL                                        irql;

    TRY {
        
        USBDEVICE_CONTROLLER_RESET_CALLBACK_CONTEXT_INIT(&controllerResetCallbackContext);

        KeAcquireSpinLock(&UcxControllerContext->TopologyLock, &irql);
        
        //
        // Walk the tree of all devices downstream of this disconnected device
        // (including the disconnected device), and build a list of endpoints
        //
        UsbDevice_WalkTree(UcxControllerContext->UcxRootHubContext->UcxUsbDeviceContextOfUcxRootHub,
                           TRUE,
                           UsbDevice_NotifyControllerResetCompleteCallback,
                           &controllerResetCallbackContext);

        KeReleaseSpinLock(&UcxControllerContext->TopologyLock, irql);

        // For each endpoint in the temporary list, remove it from the list
        // call and notify it of Controller Reset completion

        FOR_ALL_IN_LIST_SAFE(UCXENDPOINT_PRIVCONTEXT, 
                             &controllerResetCallbackContext.EndpointList,
                             ControllerResetListEntryTemp,
                             currUcxEndpointContext,
                             nextUcxEndpointContext) {

            RemoveEntryList(&currUcxEndpointContext->ControllerResetListEntryTemp);

            InitializeListEntry(&currUcxEndpointContext->ControllerResetListEntryTemp);

            Endpoint_EventAdd(currUcxEndpointContext, UcxEndpointEventControllerResetComplete);

            WdfObjectDereferenceWithTag(currUcxEndpointContext->UcxEndpoint,
                                        TAG(UsbDevice_NotifyControllerResetCompleteCallback));

        }

    } FINALLY {

    }
}

VOID
Controller_PendingPrepareForResetOperationComplete(
    __in 
        PUCXCONTROLLER_PRIVCONTEXT       UcxControllerContext 
    )
/*++

Routine Description:

    This routine is called by the endpoint state machine when it complets 
    a pending prepare for reset operation.  
        
    This routine evaluates if it is the last endpoint to complete the 
    pending operation. If yes, it queues an event into the controller reset state machine.

Arguments:

    UcxControllerContext - Context of the controller 

--*/
{
    
    TRY {

        if (0 == InterlockedDecrement(&UcxControllerContext->PendingPrepareForResetOperations)) {
        
            Controller_EventAdd(UcxControllerContext,
                                ControllerResetEventUsbDevicesAndEndpointsReadyForControllerReset);

        }

    } FINALLY {

    }
    return;

}

VOID
Controller_ForwardProgressWorkItemEnqueue(
    __in 
        PUCX_FORWARD_PROGRESS_WORKITEM          UcxForwardProgressWorkItem,
    __in
        PUCX_FORWARD_PROGRESS_WORKITEM_CALLBACK Callback,
    __in_opt
        PVOID                                   CallbackContext,
    __in
        UCX_FORWARD_PROGRESS_ENQUEUE_OPTIONS    ForwardProgressEnqueueOptions
    )
/*++

Routine Description:

    This routine is called to enqueue a Forward Progress workitem.
    
    This routine first tries to queue the Wdm Workitem, and if that fails, it will schedule the 
    task to the System Thread
    
Arguments:

    UcxControllerContext - Context of the controller
    UcxForwardProgressWorkItem - Pointer to the structure holding inforamtion about the forward
       progress workitem.
         

--*/
{
    BOOLEAN                          queued;
    KIRQL                            irql;
    PUCXCONTROLLER_PRIVCONTEXT       ucxControllerContext;
    
    TRY {

        ucxControllerContext = UcxForwardProgressWorkItem->UcxControllerContext;

        NT_ASSERT(UcxForwardProgressWorkItem->Flags.Queued == 0);
        NT_ASSERT(UcxForwardProgressWorkItem->Callback == NULL);
        NT_ASSERT(UcxForwardProgressWorkItem->CallbackContext == NULL);

        UcxForwardProgressWorkItem->Callback = Callback;
        UcxForwardProgressWorkItem->CallbackContext = CallbackContext;

        UcxForwardProgressWorkItem->Flags.Queued = 1;

        if (UcxForwardProgressWorkItem->Flags.NeedsFlushFunctionality) {
            KeAcquireSpinLock(&UcxForwardProgressWorkItem->RefCountLock, &irql);
            UcxForwardProgressWorkItem->RefCount++;
            KeClearEvent(&UcxForwardProgressWorkItem->CompletionEvent);
            KeReleaseSpinLock(&UcxForwardProgressWorkItem->RefCountLock, irql);
        }

        queued = Ucx_IoTryQueueWorkItem(UcxForwardProgressWorkItem->WdmWorkItem,
                                        Controller_ForwardProgressWorkItemWdmCallback,
                                        DelayedWorkQueue,
                                        UcxForwardProgressWorkItem);

        if (queued == FALSE) {

            if ((ForwardProgressEnqueueOptions == UCX_FORWARD_PROGRESS_NOT_REQUIRED) ||
                (ucxControllerContext->SystemThread == NULL)) {
                IoQueueWorkItemEx(UcxFo