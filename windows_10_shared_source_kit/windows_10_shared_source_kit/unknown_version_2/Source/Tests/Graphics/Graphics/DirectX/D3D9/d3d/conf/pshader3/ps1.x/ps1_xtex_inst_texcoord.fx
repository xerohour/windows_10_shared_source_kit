rwardProgressWorkItem->WdmWorkItem, 
                                  Controller_ForwardProgressWorkItemWdmCallback, 
                                  DelayedWorkQueue, 
                                  UcxForwardProgressWorkItem);        
            } else {
                KeAcquireSpinLock(&ucxControllerContext->SystemThreadQueueLock, &irql);

                InsertTailList(&ucxControllerContext->SystemThreadQueue,
                               &UcxForwardProgressWorkItem->ListEntry);

                KeSetEvent(&ucxControllerContext->SystemThreadWorkPending, 0, FALSE);

                KeReleaseSpinLock(&ucxControllerContext->SystemThreadQueueLock, irql);
            }
        }

    } FINALLY {

    }
}

PUCX_FORWARD_PROGRESS_WORKITEM 
Controller_ForwardProgressWorkItemAllocateFromHub(
    __in
        UCXUSBDEVICE                            Hub,
    __in
        PDEVICE_OBJECT                          WdmDeviceObject,
    __in
        ULONG                                   Flags
    )
/*++

Routine Description:

    This routine is a wrapper around Controller_ForwardProgressWorkItemAllocate.
    Please see description of Controller_ForwardProgressWorkItemAllcoate for details.
    
Arguments:

    Hub - The Hub that is initializing the workitem
    
    See Controller_ForwardProgressWorkItemAllocate for remaining params
    
--*/
{
    PUCXUSBDEVICE_PRIVCONTEXT           ucxUsbDeviceContext;

    TRY {
        ucxUsbDeviceContext = UcxUsbDeviceGetContext(Hub);
        
        return Controller_ForwardProgressWorkItemAllocate(ucxUsbDeviceContext->UcxControllerContext,
                                                          WdmDeviceObject,
                                                          Flags);
    } FINALLY {
    }

}


PUCX_FORWARD_PROGRESS_WORKITEM 
Controller_ForwardProgressWorkItemAllocate(
    __in
        PUCXCONTROLLER_PRIVCONTEXT              UcxControllerContext,
    __in
        PDEVICE_OBJECT                          WdmDeviceObject,
    __in
        ULONG                                   Flags
    )
/*++

Routine Description:

    This routine allocates and initializes a Forward Progress workitem.
    
Arguments:

    Hub - The Hub that is initializing the workitem
    
    UcxForwardProgressWorkItem - Pointer to the structure holding inforamtion about the forward
       progress workitem.
       
    WdmDeviceObject - Wdm Device Object of the module requesting the Init operation
    
    WdmWorkItem - Called initialized Workitem. 
    
    Flags - Optional flags

--*/
{
    PUCX_FORWARD_PROGRESS_WORKITEM      ucxWorkItem;
    PIO_WORKITEM                        wdmWorkItem;

    TRY {

        ucxWorkItem = ExAllocatePoolWithTag(NonPagedPool,
                                            sizeof(UCX_FORWARD_PROGRESS_WORKITEM),
                                            UCX_TAG);
        if (ucxWorkItem == NULL) {
            LogError(FlagController, "ExAllocatePoolWithTag failed");
            LEAVE;
        }

        wdmWorkItem = IoAllocateWorkItem(WdmDeviceObject);
        
        if (wdmWorkItem == NULL) {
            LogError(FlagController, "IoAllocateWorkItem failed");
            ExFreePoolWithTag(ucxWorkItem,
                              UCX_TAG);
            ucxWorkItem = NULL;        
            LEAVE;
        }
        
        RtlZeroMemory(ucxWorkItem, sizeof(*ucxWorkItem));
        ucxWorkItem->Signature = SigUcxForwardProgressWorkItem;
        InitializeListEntry(&ucxWorkItem->ListEntry);
        KeInitializeEvent(&ucxWorkItem->CompletionEvent,
                          NotificationEvent,
                          TRUE);
        KeInitializeSpinLock(&ucxWorkItem->RefCountLock);
        ucxWorkItem->WdmDeviceObject = WdmDeviceObject;
        ucxWorkItem->WdmWorkItem = wdmWorkItem;
        ucxWorkItem->UcxControllerContext = UcxControllerContext;
        
        if (TEST_FLAG(Flags, UCX_FORWARD_PROGRESS_WORKITEM_FLAG_NEED_FLUSH_FUNCTIONALITY)) {
            ucxWorkItem->Flags.NeedsFlushFunctionality = 1;
        }
    
    } FINALLY {

    }
    return ucxWorkItem;

}

VOID
Controller_ForwardProgressWorkItemDelete(
    __in
        PUCX_FORWARD_PROGRESS_WORKITEM   UcxForwardProgressWorkItem
    )
/*++

Routine Description:

    This routine cleans up a Forward Progress workitem.
    
    If flush functionality is enabled, Controller_ForwardProgressWorkItemFlush 
    must be called before calling this routine. 
    
    If flush functionality is not enabled, this routine may be called from the callback of the
    forward progress workitem.
    
Arguments:
    
    UcxForwardProgressWorkItem - Pointer to the structure holding inforamtion about the forward
       progress workitem.
       
--*/
{
    TRY {

        IoFreeWorkItem(UcxForwardProgressWorkItem->WdmWorkItem);

        ExFreePoolWithTag(UcxForwardProgressWorkItem,
                          UCX_TAG);
    
    } FINALLY {

    }
}

VOID
Controller_ForwardProgressWorkItemCallback(
    __in
        PUCXCONTROLLER_PRIVCONTEXT       UcxControllerContext,
    __in
        PUCX_FORWARD_PROGRESS_WORKITEM   UcxForwardProgressWorkItem
    )
/*++

Routine Description:

    This routine is the callback that is called either by the system thread or the wdm workitem, 
    for a UCX_FORWARD_PROGRESS_WORKITEM. 
    
    From this routine we call the callback of the module that queued a UCX_FORWARD_PROGRESS_WORKITEM
    
    [Optionally], we also set an event that is required by the hub driver for the flush 
    functionality.         

Arguments:

    UcxControllerContext - Context of the controller
    UcxForwardProgressWorkItem - Pointer to the structure holding inforamtion about the forward
       progress workitem.
         

--*/
{
    BOOLEAN                                 flushFunctionality;
    PDEVICE_OBJECT                          deviceObject;
    KIRQL                                   irql;
    PUCX_FORWARD_PROGRESS_WORKITEM_CALLBACK callback;
    PVOID                                   callbackContext;