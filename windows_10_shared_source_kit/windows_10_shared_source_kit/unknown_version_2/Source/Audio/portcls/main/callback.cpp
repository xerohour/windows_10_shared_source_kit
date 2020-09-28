/*****************************************************************************
 * callback.cpp - Generic unload safe callbacks (where possible)
 *****************************************************************************
 * Copyright (c) Microsoft.  All rights reserved.
 */

#include "private.h"

IO_WORKITEM_ROUTINE EnqueuedIoWorkItemCallback;
KDEFERRED_ROUTINE EnqueuedDpcCallback;

#pragma code_seg()

NTSTATUS
CallbackEnqueue(
    IN OUT  PVOID                   *pCallbackHandle OPTIONAL,
    IN      PFNQUEUED_CALLBACK      CallbackRoutine,
    IN      PDEVICE_OBJECT          DeviceObject,
    IN      PVOID                   Context,
    IN      KIRQL                   Irql,
    IN      ULONG                   Flags
    )
{
    PQUEUED_CALLBACK_ITEM pQueuedCallbackItem;

    //
    // Check the flags we understand. If it's not understood, and this is the
    // class of flags support is required for, bail immediately.
    //
    if ((Flags & (~EQCM_SUPPORTED_FLAGS)) & EQCM_SUPPORT_OR_FAIL_FLAGS) {

        return STATUS_NOT_SUPPORTED;
    }

    if ((Irql != PASSIVE_LEVEL) && (Irql != DISPATCH_LEVEL)) {

        ASSERT(0);
        return STATUS_NOT_SUPPORTED;
    }

    pQueuedCallbackItem = (PQUEUED_CALLBACK_ITEM) ExAllocatePoolWithTag(
        ((KeGetCurrentIrql() == PASSIVE_LEVEL) && (Irql == PASSIVE_LEVEL)) ?
            PagedPool : NonPagedPoolNx,
        sizeof(QUEUED_CALLBACK_ITEM),
        'bCcP'
        );  //  'PcCb'

    if (pQueuedCallbackItem) {

        pQueuedCallbackItem->IoWorkItem = IoAllocateWorkItem(
            DeviceObject
            );

        if (pQueuedCallbackItem->IoWorkItem == NULL) {

            ExFreePool(pQueuedCallbackItem);
            pQueuedCallbackItem = NULL;
        }
    }

    if (ARGUMENT_PRESENT(pCallbackHandle)) {
        *pCallbackHandle = pQueuedCallbackItem;
    }

    if (pQueuedCallbackItem) {

        pQueuedCallbackItem->QueuedCallback = CallbackRoutine;
        pQueuedCallbackItem->DeviceObject   = DeviceObject;
        pQueuedCallbackItem->Context        = Context;
        pQueuedCallbackItem->Flags          = Flags;
        pQueuedCallbackItem->Irql           = Irql;
        pQueuedCallbackItem->Enqueued       = 1;

        if ((!(Flags&EQCF_DIFFERENT_THREAD_REQUIRED)) &&
            (KeGetCurrentIrql() == Irql) && (Irql == PASSIVE_LEVEL))
        {
            EnqueuedIoWorkItemCallback(DeviceObject, (PVOID) pQueuedCallbackItem);
        } else {

            if (Irql == PASSIVE_LEVEL) {

                IoQueueWorkItem(
                    pQueuedCallbackItem->IoWorkItem,
                    EnqueuedIoWorkItemCallback,
                    DelayedWorkQueue,
                    pQueuedCallbackItem
                    );

            } else {

                ASSERT(Irql == DISPATCH_LEVEL);
                KeInitializeDpc(
                    &pQueuedCallbackItem->Dpc,
                    EnqueuedDpcCallback,
                    pQueuedCallbackItem
                    );

                KeInsertQueueDpc(
                    &pQueuedCallbackItem->Dpc,
                     NULL,
                     NULL
                     );
            }
        }

        return STATUS_SUCCESS;

    } else {

        return STATUS_INSUFFICIENT_RESOURCES;
    }
}

NTSTATUS
CallbackCancel(
    IN      PVOID   pCallbackHandle
    )
{
    PQUEUED_CALLBACK_ITEM   pQueuedCallbackItem;

    pQueuedCallbackItem = (PQUEUED_CALLBACK_ITEM) pCallbackHandle;

    if (InterlockedExchange(&pQueuedCallbackItem->Enqueued, 0) == 1) {

        //
        // We got it. If it's DPC, also try to yank it from the queue.
        //
        if (pQueuedCallbackItem->Irql == DISPATCH_LEVEL) {

            KeRemoveQueueDpc(&pQueuedCallbackItem->Dpc);
        }

        return STATUS_SUCCESS;
    } else {

        //
        // Caller beat us to it...
        //
        return STATUS_UNSUCCESSFUL;
    }
}

VOID
CallbackFree(
    IN      PVOID   pCallbackHandle
    )
{
    PQUEUED_CALLBACK_ITEM   pQueuedCallbackItem;

    pQueuedCallbackItem = (PQUEUED_CALLBACK_ITEM) pCallbackHandle;

    ASSERT(pQueuedCallbackItem->Enqueued == 0);

    IoFreeWorkItem(pQueuedCallbackItem->IoWorkItem);
    ExFreePool(pQueuedCallbackItem);
}

VOID
EnqueuedDpcCallback(
    IN PKDPC Dpc,
    IN PVOID DeferredContext,
    IN PVOID SystemArgument1,
    IN PVOID SystemArgument2
    )
{
    PQUEUED_CALLBACK_ITEM   pQueuedCallbackItem;
    QUEUED_CALLBACK_RETURN  returnValue;

    pQueuedCallbackItem = (PQUEUED_CALLBACK_ITEM) DeferredContext;

    if (InterlockedExchange(&pQueuedCallbackItem->Enqueued, 0) == 1) {

        returnValue = pQueuedCallbackItem->QueuedCallback(
            pQueuedCallbackItem->DeviceObject,
            pQueuedCallbackItem->Context
            );

    } else {

        returnValue = QUEUED_CALLBACK_RETAIN;
    }

    switch(returnValue) {

        case QUEUED_CALLBACK_FREE:

            CallbackFree((PVOID) pQueuedCallbackItem);
            break;

        case QUEUED_CALLBACK_RETAIN:

            //
            // Nothing to do in this case, in fact we don't dare touch anything
            // in the structure lest it be already freed.
            //
            break;

        default:
            ASSERT(0);
            break;
    }
}

#pragma code_seg("PAGE")

VOID
EnqueuedIoWorkItemCallback(
    __in  PDEVICE_OBJECT  DeviceObject,
    __in_opt  __drv_aliasesMem PVOID           Context
    )
{
    PQUEUED_CALLBACK_ITEM   pQueuedCallbackItem;
    QUEUED_CALLBACK_RETURN  returnValue;

    PAGED_CODE();

    pQueuedCallbackItem = (PQUEUED_CALLBACK_ITEM) Context;

    if(pQueuedCallbackItem != NULL)
    {
        ASSERT(pQueuedCallbackItem->DeviceObject == DeviceObject);

        if (InterlockedExchange(&pQueuedCallbackItem->Enqueued, 0) == 1) {

            returnValue = pQueuedCallbackItem->QueuedCallback(
                pQueuedCallbackItem->DeviceObject,
                pQueuedCallbackItem->Context
                );

        } else {

            returnValue = QUEUED_CALLBACK_RETAIN;
        }

        switch(returnValue) {

            case QUEUED_CALLBACK_FREE:

                CallbackFree((PVOID) pQueuedCallbackItem);
                break;

            case QUEUED_CALLBACK_RETAIN:

                //
                // Nothing to do in this case, in fact we don't dare touch anything
                // in the structure lest it be already freed.
                //
                break;

            default:
                ASSERT(0);
                break;
        }
    }
}


