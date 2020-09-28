
    
    UNREFERENCED_PARAMETER(UcxControllerContext);

    TRY {
        
        NT_ASSERT(UcxForwardProgressWorkItem->Flags.Queued == 1);
        UcxForwardProgressWorkItem->Flags.Queued = 0;

        flushFunctionality = 
            (UcxForwardProgressWorkItem->Flags.NeedsFlushFunctionality == 1) ? TRUE : FALSE;

        deviceObject = UcxForwardProgressWorkItem->WdmDeviceObject;

        callback = UcxForwardProgressWorkItem->Callback;
        callbackContext = UcxForwardProgressWorkItem->CallbackContext;
        UcxForwardProgressWorkItem->Callback = NULL;
        UcxForwardProgressWorkItem->CallbackContext = NULL;
        UcxForwardProgressWorkItem->Flags.Queued = 0;

        //
        // Reference the deviceObject before calling the callback (since the callback
        // can be for the usbhub3 driver)
        // This is done to prevent the situation where the hub driver gets unloaded before returning
        // from this call. 
        //
        ObReferenceObject(deviceObject);  
        
        callback(deviceObject,
                 callbackContext,
                 UcxForwardProgressWorkItem);

        ObDereferenceObject(deviceObject);

        if (flushFunctionality) {
            //
            // In this case it is OK to touch UcxForwardProgressWorkItem since, 
            // Caller must wait for the CompletionEvent to be set before freeing this structure.
            //
            KeAcquireSpinLock(&UcxForwardProgressWorkItem->RefCountLock, &irql);

            NT_ASSERT(UcxForwardProgressWorkItem->RefCount > 0);

            UcxForwardProgressWorkItem->RefCount--;

            if (UcxForwardProgressWorkItem->RefCount == 0) {
                //
                // Signal an event indicating that this UcxForwardProgressWorkItem is complete
                // and has not been requeued either. 
                //
                KeSetEvent(&UcxForwardProgressWorkItem->CompletionEvent, 0, FALSE);
            }

            KeReleaseSpinLock(&UcxForwardProgressWorkItem->RefCountLock, irql);
        }

    } FINALLY {

    }
}

__drv_requiresIRQL(PASSIVE_LEVEL)
VOID
Controller_ForwardProgressWorkItemFlush(
    __in
        PUCX_FORWARD_PROGRESS_WORKITEM UcxForwardProgressWorkitem
    )
/*++

Routine Description:

    The function is called to ensure that any pending callbacks enqueued using the  
    forward progress workitem have been called and have run to completion. 
    
    This routine must only be called if Flush functionality was requested when the 
    forward progress workitem was allocated. 
    
    This routine must not be called from the callback thread, or it would cause a deadlock 
    
    If flush functionlity was requested at the time of allocating the Forward Progress workitem, 
    the Controller_ForwardProgressWorkItemFlush must be called before deleting the 
    workitem.
    
    
Arguments:

    UcxForwardProgressWorkItem - Pointer to the structure holding inforamtion about the forward
       progress workitem.
         

--*/
{
    KIRQL                                   irql;

    TRY {
            
        if (UcxForwardProgressWorkitem->Flags.NeedsFlushFunctionality == 0) {
            NT_ASSERTMSG("Flush Feature not enabled", FALSE);
            LEAVE;
        }

        KeWaitForSingleObject(&UcxForwardProgressWorkitem->CompletionEvent,
                      Executive,
                      KernelMode,
                      FALSE,
                      NULL);

        //
        // Controller_ForwardProgressWorkItemCallback sets the CompletionEvent event that 
        // we were waiting on. That routine still needs to drop the SpinLock. 
        // Acquire and Release a Lock to ensure that before we return from this routine, 
        // the Controller_ForwardProgressWorkItemCallback routine has infact dropped the spinlock,
        // and is no longer going to touch the UcxForwardProgressWorkitem structure.
        //

        KeAcquireSpinLock(&UcxForwardProgressWorkitem->RefCountLock, &irql);
        KeReleaseSpinLock(&UcxForwardProgressWorkitem->RefCountLock, irql);

    } FINALLY {

    }
}



VOID
Controller_ForwardProgressWorkItemWdmCallback(
    __in
        PDEVICE_OBJECT                 DeviceObject,
    __in_opt
        PUCX_FORWARD_PROGRESS_WORKITEM UcxForwardProgressWorkItem,
    __in
        PIO_WORKITEM                   IoWorkItem
    )
{
    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(IoWorkItem);

    Controller_ForwardProgressWorkItemCallback(UcxForwardProgressWorkItem->UcxControllerContext,
                                               UcxForwardProgressWorkItem);
}


VOID
Controller_SystemThread(
    PVOID UcxControllerContextAsPVOID
    )
/*++

Routine Description:

    This routine is a system thread. 
     
    One System Thread is scheduled per controller. This routine ensures forward progress. 
        
Arguments:

    UcxControllerContext - Context of the controller

--*/
{
    PUCXCONTROLLER_PRIVCONTEXT     ucxControllerContext;
    PLIST_ENTRY                    listEntry;
    PUCX_FORWARD_PROGRESS_WORKITEM ucxForwardProgressWorkitem;
    KIRQL                          irql;

    TRY {
        
        ucxControllerContext = (PUCXCONTROLLER_PRIVCONTEXT) UcxControllerContextAsPVOID;

        do {

            if (KeReadStateEvent(&ucxControllerContext->SystemThreadWorkPending) == 0) {
                ucxControllerContext->SystemThreadPaused = TRUE;
            }

            KeWaitForSingleObject(&ucxControllerContext->SystemThreadWorkPending,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);
            
            NT_ASSERT(ucxControllerContext->RHPdoInD0 || ucxControllerContext->SystemThreadTerminate);
            
            ucxControllerContext->SystemThreadPaused = FALSE;

            KeAcquireSpinLock(&ucxControllerContext->SystemThreadQueueLock, &irql);
            
            if (ucxControllerContext->SystemThreadTerminate) {
                NT_ASSERT(IsListEmpty(&ucxControllerContext->SystemThreadQueue));
                KeReleaseSpinLock(&ucxControllerContext->SystemThreadQueueLock, irql);
                PsTerminateSystemThread(STATUS_SUCCESS);
                break;
            }

            NT_ASSERT(FALSE == IsListEmpty(&ucxControllerContext->SystemThreadQueue));
            listEntry = RemoveHeadList(&ucxControllerContext->SystemThreadQueue);
            if (IsListEmpty(&ucxControllerContext->SystemThreadQueue)) {
                KeClearEvent(&ucxControllerContext->SystemThreadWorkPending);
            }
            
            KeReleaseSpinLock(&ucxControllerContext->SystemThreadQueueLock, irql);

            ucxForwardProgressWorkitem = CONTAINING_RECORD(listEntry,
                                                           UCX_FORWARD_PROGRESS_WORKITEM,
                                                           ListEntry);

            Controller_ForwardProgressWorkItemCallback(ucxControllerContext,
                                                       ucxForwardProgressWorkitem);

        } WHILE(TRUE);

    } FINALLY {

    }
}   

NTSTATUS
Controller_GetCurrentFrameNumber(
    __in 
        PUCXCONTROLLER_PRIVCONTEXT UcxControllerContext,
    __out 
        PULONG                     CurrentFrame
    )
/*++

Routine Description:

    Returns the value of the current USB frame counter or 'clock'.

Arguments:

    CurrentFrame - pointer to receive the current frame number.

Return Value:

    NT status code.

--*/
{
    NTSTATUS                   status;
    UCXCONTROLLER              ucxController;

    TRY {
        
        NT_ASSERT(CurrentFrame != NULL);
        
        ucxController = UcxControllerContext->UcxController;

        if (UcxControllerContext->Config.EvtControllerGetCurrentFrameNumber != NULL) {

            if (Controller_ResetReferenceAcquire(UcxControllerContext)) {
                status = (*UcxControllerContext->
                          Config.EvtControllerGetCurrentFrameNumber)(ucxController,
                                                                     CurrentFrame);
                UcxControllerContext->CachedFrameNumber = *CurrentFrame;
                LogVerbose(FlagController, "EvtControllerGetCurrentFrameNumber %d, %!STATUS!",
                          *CurrentFrame, status);
                Controller_ResetReferenceRelease(UcxControllerContext);
            } else {
                status = STATUS_SUCCESS;
                *CurrentFrame = UcxControllerContext->CachedFrameNumber;
                LogInfo(FlagController, 
                        "Faking EvtControllerGetCurrentFrameNumber while controller reset %d",
                        *CurrentFrame);                
            }

        } else {
            //
            // Compat: 
            // Todo: PostWin8: Before we make UCX-HCD Public
            // Need to figure out a story for this before we make UCX-HCD 
            // interface public
            //
            NT_ASSERTMSG("Compat Risk - Need to implement this", 0);
            status = STATUS_NOT_SUPPORTED;
        }


    } FINALLY {

    }
 
    return status;
}

__drv_requiresIRQL(PASSIVE_LEVEL)
NTSTATUS
Controller_StopIdleSynchronous(
    __in
        UCXUSBDEVICE                  H