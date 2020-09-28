/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    worker.c

Abstract:

    emergency worker thread and its hub interface

Environment:

    kernel mode only

--*/

#include "ntosp.h"
#include "zwapi.h"
#include "wmilib.h"
#include "usb.h"
#include "hubbusif.h"
#include "dbg.h"
#define USB20_API
#include "usbioctl.h"
#include "usbboot.h"

#pragma warning(push)
#pragma warning(disable: 4200) //  zero-sized array in struct/union
#pragma warning(disable: 4201) //  zero-sized array in struct/union
#pragma warning(disable: 4214) //  bit field types other than int

#include "usbhcdi.h"
#include <acpiioct.h>
#include "usbport.h"

#pragma warning(pop)


#define WHILE_TRUE \
__pragma(warning(disable: 4127)) while(1) __pragma(warning(default: 4127))

#ifdef USBPORT_WPP
#include "worker.tmh"
#endif

#define USBPORT_MAX_EMERGENCY_WORKER_THREAD 1

typedef
BOOLEAN
(*PFN_IO_TRY_QUEUE_WORKITEM)(
    __inout PIO_WORKITEM IoWorkItem,
    __in PIO_WORKITEM_ROUTINE_EX WorkerRoutine,
    __in WORK_QUEUE_TYPE QueueType,
    __in_opt __drv_aliasesMem PVOID Context
    );

typedef struct _USB_WORKITEM {
    LIST_ENTRY List;
    PIO_WORKITEM_ROUTINE_EX Routine;
    PVOID IoObject;
    PVOID Context;
} USB_WORKITEM, *PUSB_WORKITEM;

typedef struct _USB_IRP_WORKITEM {
    USB_WORKITEM_HANDLE IoWorkItem;
    PUSB_IRP_WORKER_ROUTINE IrpWorkerRoutine;
    PIRP Irp;
    BOOLEAN Reserved;
} USB_IRP_WORKITEM, *PUSB_IRP_WORKITEM;

KEVENT EmergencyWorkerThreadEvent;
PKTHREAD EmergencyWorkerThread[USBPORT_MAX_EMERGENCY_WORKER_THREAD] = {0};
BOOLEAN EmergencyWorkerThreadCreated = FALSE;
PUSB_IRP_WORKITEM EmergencyWorkItem = NULL;
BOOLEAN EmergencyWorkItemInUse = FALSE;
KQUEUE EmergencyWorkQueue;
LIST_ENTRY DeferredIrps;
KSPIN_LOCK DeferredIrpsSpinLock;
KSTART_ROUTINE USBPORT_WorkerStart;
PFN_IO_TRY_QUEUE_WORKITEM FnIoTryQueueWorkItem = NULL;


USB_WORKITEM_HANDLE
USBPORT_AllocateWorkItem(
    __in PDEVICE_OBJECT DeviceObject,
    __in BOOLEAN ForwardProgress
    )
/*++

Routine Description:

    Wrapper of IoAllocateWorkItem

Arguments:

    DeviceObject - device for which the IO workitem is to be allocated

    ForwardProgress - flag indicating whether forward progress support is
        required.

Return Value:

    A wrapper IO workitem with an IO_WORKITEM at the beginning followed
    by fields used by the emergency worker thread

--*/
{
    PIO_WORKITEM ioWorkItem = NULL;
    ULONG sizeOfWorkItem;

    sizeOfWorkItem = IoSizeofWorkItem()+(ForwardProgress?sizeof(USB_WORKITEM):0);
    ALLOC_POOL_Z(ioWorkItem, NonPagedPoolNx, sizeOfWorkItem);
    if (ioWorkItem && DeviceObject) {
        IoInitializeWorkItem(DeviceObject, ioWorkItem);
    }

    return (USB_WORKITEM_HANDLE)ioWorkItem;
}


VOID
USBPORT_FreeWorkItem(
    __in USB_WORKITEM_HANDLE IoWorkItem
    )
/*++

Routine Description:

    Workitem deallocation routine

Arguments:

    IoWorkItem - Supplies a pointer to the work item to be freed

Return Value:

    None

--*/
{
    IoUninitializeWorkItem((PIO_WORKITEM)IoWorkItem);
    UsbFreePool((PVOID)IoWorkItem);
}


PUSB_IRP_WORKITEM
USBPORT_AllocateIrpWorkItem(
    )
/*++

Routine Description:

    Allocate a USB IRP work item

Arguments:

    None

Return Value:

    NULL if there was insufficient memory to allocate a work item, otherwise
    return the initialized work item.

--*/
{
    PUSB_IRP_WORKITEM irpWorkItem = NULL;
    USB_WORKITEM_HANDLE ioWorkItem = NULL;

    // Allocate a new IO workitem structure.
    ALLOC_POOL_Z(irpWorkItem, NonPagedPoolNx, sizeof(*irpWorkItem));
    ioWorkItem = USBPORT_AllocateWorkItem(NULL, TRUE);

    if (!irpWorkItem || !ioWorkItem) {

        // Allocation failed, clean up

        if (irpWorkItem) {
            UsbFreePool(irpWorkItem);
            irpWorkItem = NULL;
        }

        if (ioWorkItem) {
            USBPORT_FreeWorkItem(ioWorkItem);
            ioWorkItem = NULL;
        }

    } else {

        irpWorkItem->IoWorkItem = ioWorkItem;
        irpWorkItem->Reserved = FALSE;

    }

    return irpWorkItem;
}


VOID
USBPORT_FreeIrpWorkItem(
    __in PUSB_IRP_WORKITEM IrpWorkItem
    )
/*++

Routine Description:

    This function is called to free a work item when it is no longer
    needed.

Arguments:

    IrpWorkItem - Supplies a pointer to a USB_IRP_WORKITEM to be freed.

Return Value:

    None

--*/
{
    USBPORT_FreeWorkItem(IrpWorkItem->IoWorkItem);
    UsbFreePool(IrpWorkItem);
}


VOID
USBPORT_WorkerStart (
    __in PVOID Parameter
    )
/*++

Routine Description:

    Dedicated USB worker thread that is used as a backup to KmTP when KmTP
    can't make progress.

Arguments:

    Parameter - Unused.

Return Value:

    None

--*/
{
    PLIST_ENTRY le;
    PUSB_WORKITEM usbWorkItem;
    PVOID ioObject;
    PVOID context;
    PIO_WORKITEM_ROUTINE_EX routine;

    WHILE_TRUE {
        le = KeRemoveQueue(&EmergencyWorkQueue, KernelMode, NULL);
        if (le == (PLIST_ENTRY)STATUS_ABANDONED) {
            continue;
        }

        usbWorkItem = CONTAINING_RECORD(le, USB_WORKITEM, List);

        ioObject = usbWorkItem->IoObject;
        context = usbWorkItem->Context;
        routine = usbWorkItem->Routine;

        // Call the workitem routine
        routine(ioObject,
                context,
                (PIO_WORKITEM)((ULONG_PTR)usbWorkItem-IoSizeofWorkItem()));

        ObDereferenceObject(ioObject);
    }

    UNREFERENCED_PARAMETER(Parameter);
}


VOID
USBPORT_QueueWorkItem(
    __in PDEVICE_OBJECT DeviceObject,
    __in USB_WORKITEM_HANDLE IoWorkItem,
    __in PIO_WORKITEM_ROUTINE_EX WorkerRoutine,
    __in WORK_QUEUE_TYPE QueueType,
    __in_opt PVOID Context,
    __in BOOLEAN ForwardProgress
    )
/*++

Routine Description:

    Wrapper of IoTryQueueWorkItem

Arguments:

    IoWorkItem - Supplies a pointer to the work item to add the the queue.
        This structure must have been allocated via IoAllocateWorkItem()

    WorkerRoutine - Supplies a pointer to the routine that is to be called
        in system thread context.

    QueueType - Specifies the type of work queue that the work item
        should be placed in.

    Context - Supplies the context parameter for the callback routine.

    ForwardProgress - flag indicating whether forward progress support is
        required.

Return Value:

    None

--*/
{
    BOOLEAN queued = FALSE;

    if (ForwardProgress) {

    // Try KmTP first
#ifndef USB_FORCE_EMERGENCY_WORKER_THREAD
        if (FnIoTryQueueWorkItem) {
            queued = (*FnIoTryQueueWorkItem)((PIO_WORKITEM)IoWorkItem,
                                             WorkerRoutine,
                                             QueueType,
                                             Context);
        }
#endif

        if (!queued && EmergencyWorkerThreadCreated) {

            // Reinitialize the workitem using the USB_WORKITEM structure
            PUSB_WORKITEM usbWorkItem =
                (PUSB_WORKITEM)((ULONG_PTR)IoWorkItem+IoSizeofWorkItem());

            ObReferenceObject(DeviceObject);
            usbWorkItem->IoObject = DeviceObject;
            usbWorkItem->Routine = WorkerRoutine;
            usbWorkItem->Context = Context;

            // Queuing to KmTP failed, use our worker thread
            InitializeListHead(&usbWorkItem->List);
            (VOID)KeInsertQueue(&EmergencyWorkQueue, &usbWorkItem->List);
            queued = TRUE;
         }

    }

    // In the case where our emergency workerthread is not created
    // just queue the workitem to the system worker thread. We might
    // enter into a deadlock state but there is not much we can do
    // without the emergency worker thread.
    if (!queued)
    {
        IoQueueWorkItemEx((PIO_WORKITEM)IoWorkItem,
                          WorkerRoutine,
                          QueueType,
                          Context);
    }
}


VOID
USBPORT_ProcessIrpWorkItem(
    __in PDEVICE_OBJECT DeviceObject,
    __in PUSB_IRP_WORKITEM IrpWorkItem,
    __in PIO_WORKITEM IoWorkItem
    )

/*++

Routine Description:

    This function calls the IRP worker routine.

Arguments:

    DeviceObject - Supplies a pointer to a DEVICE_OBJECT to associate with
                   the workitem.
    IrpWorkItem - Supplies a pointer to a USB_IRP_WORKITEM to be processed.
    IoWorkItem - Supplies a pointer to the IO_WORKITEM associated with the
                 USB_IRP_WORKITEM.

Return Value:

    None

--*/

{
    IrpWorkItem->IrpWorkerRoutine(DeviceObject, IrpWorkItem->Irp);

    // Now we can dereference the device object, since its code is no longer
    // being executed for this work item.
    ObDereferenceObject(DeviceObject);

    // Release the workitem
    if (!IrpWorkItem->Reserved) {

        USBPORT_FreeIrpWorkItem(IrpWorkItem);

    } else {

        // We are about to release the reserved workitem. This has to be done
        // under the protection of DeferredIrpsSpinLock or a held IRP might get
        // orphaned.

        KIRQL irql;
        PIRP irp = NULL;
        PDEVICE_OBJECT devObj = NULL;
        PUSB_IRP_WORKER_ROUTINE irpWorkerRoutine = NULL;

        KeAcquireSpinLock(&DeferredIrpsSpinLock, &irql);

        // Release a held Irp, if there is any
        if (!IsListEmpty(&DeferredIrps)) {
            PLIST_ENTRY le;

            le = RemoveHeadList(&DeferredIrps);
            irp = CONTAINING_RECORD(le, IRP, Tail.Overlay.ListEntry);
            devObj = (PDEVICE_OBJECT)irp->Tail.Overlay.DriverContext[0];
            irpWorkerRoutine =
                (PUSB_IRP_WORKER_ROUTINE)(ULONG_PTR)irp->Tail.Overlay.DriverContext[1];
        } else {
            // Deferred queue is emptry return the emergency workitem
            EmergencyWorkItemInUse = FALSE;
        }

        KeReleaseSpinLock(&DeferredIrpsSpinLock, irql);

        // If there is an IRP on the deferred queue, make sure
        // it gets run next using the emergency workitem or in
        // the extremely rare case it might be starved if the
        // system always gets into low-resource situations whenever
        // it tries to run through USBPORT_DeferIrpProcessing.

        if (irp) {
            // Reinitialize the workitem
            IrpWorkItem->IrpWorkerRoutine = irpWorkerRoutine;
            IrpWorkItem->Irp = irp;

            // Queue the workitem to the worker thread
            USBPORT_QueueWorkItem(devObj,
                                  IrpWorkItem->IoWorkItem,
                                  USBPORT_ProcessIrpWorkItem,
                                  DelayedWorkQueue,
                                  IrpWorkItem,
                                  TRUE
                                  );
        }
    }

}


VOID
USBPORT_DeferIrpProcessing(
    __in PDEVICE_OBJECT DeviceObject,
    __in PUSB_IRP_WORKER_ROUTINE IrpWorkerRoutine,
    __in PIRP Irp
    )
/*++

Routine Description:

    This function defers the processing of an IRP to either KmTP or our own
    emergency worker thread if KmTP is unable to process the work item.

Arguments:

    DeviceObject - Supplies a pointer to a device object.
    IrpWorkerRoutine - Supplies a pointer to the real IRP processing routine
    Irp - Supplies a pointer to the IRP whose processing is to be deferred to a
          worker thread

Return Value:

    None

--*/
{
    PUSB_IRP_WORKITEM irpWorkItem = NULL;
    KIRQL irql;

    // Pend the IRP
    IoMarkIrpPending(Irp);

    // Reference the device object
    ObReferenceObject(DeviceObject);

#ifndef USB_FORCE_MEMORY_STRESS
    // Create a workitem for the IRP
    irpWorkItem = USBPORT_AllocateIrpWorkItem();
#endif

    if (!irpWorkItem) {

        KeAcquireSpinLock(&DeferredIrpsSpinLock, &irql);

        // We just failed to allocate a workitem and now have to fall back on
        // the reserved one. Do this under the protection of DeferredIrpSpinLock
        // to prevent an IRP from being orphaned on DeferredIrps

        if (!EmergencyWorkItemInUse) {
            irpWorkItem = EmergencyWorkItem;
            EmergencyWorkItemInUse = TRUE;
        }

        if (!irpWorkItem) {

            // If the reserved workitem is unavailable append the current
            // IRP to the end of the deferred queue

            Irp->Tail.Overlay.DriverContext[0] = DeviceObject;
            Irp->Tail.Overlay.DriverContext[1] = (PVOID)(ULONG_PTR)IrpWorkerRoutine;
            InitializeListHead(&Irp->Tail.Overlay.ListEntry);

            InsertTailList(&DeferredIrps, &Irp->Tail.Overlay.ListEntry);
        }

        KeReleaseSpinLock(&DeferredIrpsSpinLock, irql);
    }

    if (irpWorkItem) {

        // Initialize the workitem
        IoInitializeWorkItem(DeviceObject, (PIO_WORKITEM)irpWorkItem->IoWorkItem);
        irpWorkItem->IrpWorkerRoutine = IrpWorkerRoutine;
        irpWorkItem->Irp = Irp;

        // Queue the workitem to the worker thread
        USBPORT_QueueWorkItem(DeviceObject,
                              irpWorkItem->IoWorkItem,
                              USBPORT_ProcessIrpWorkItem,
                              DelayedWorkQueue,
                              irpWorkItem,
                              TRUE
                              );
    }
}


VOID
USBPORT_PreInitializeEmergencyWorkerThread(
    )
/*++

Routine Description:

    This function initializes the mutex protecting USB emergency worker
    thread and its associated data structures

Arguments:

    None

Return Value:

    None

--*/
{
    UNICODE_STRING fnName;

    // Resolve IoTryQueueWorkItem
    RtlInitUnicodeString(&fnName, L"IoTryQueueWorkItem");
    FnIoTryQueueWorkItem = (PFN_IO_TRY_QUEUE_WORKITEM)(ULONG_PTR)MmGetSystemRoutineAddress(&fnName);

    KeInitializeEvent(&EmergencyWorkerThreadEvent, SynchronizationEvent, TRUE);
}


NTSTATUS
USBPORT_InitializeEmergencyWorkerThread(
    )
/*++

Routine Description:

    This function initializes USB emergency worker thread and its
    associated data structures.

Arguments:

    None

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS nts = STATUS_SUCCESS;
    HANDLE threadHandle = NULL;
    OBJECT_ATTRIBUTES oa;
    ULONG i;

    KeWaitForSingleObject(&EmergencyWorkerThreadEvent,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);

    if (!EmergencyWorkerThreadCreated) {

        // Create a reserved work item
        if (!EmergencyWorkItem) {
            EmergencyWorkItem = USBPORT_AllocateIrpWorkItem();
            if (!EmergencyWorkItem) {
                nts = STATUS_INSUFFICIENT_RESOURCES;
            } else {
                EmergencyWorkItem->Reserved = TRUE;

                KeInitializeQueue(&EmergencyWorkQueue, 0);
                InitializeListHead(&DeferredIrps);
                KeInitializeSpinLock(&DeferredIrpsSpinLock);
            }
        }

        if (NT_SUCCESS(nts)) {
            for (i = 0; i < USBPORT_MAX_EMERGENCY_WORKER_THREAD; i++) {

                if (!EmergencyWorkerThread[i]) {

                    // Create the worker thread
                    InitializeObjectAttributes(&oa, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);
                    nts = PsCreateSystemThread(&threadHandle,
                                               0,
                                               &oa,
                                               NULL,
                                               NULL,
                                               USBPORT_WorkerStart,
                                               NULL);

                    if (!NT_SUCCESS(nts)) {
                        break;
                    }

                    nts = ObReferenceObjectByHandle(threadHandle,
                                                    THREAD_ALL_ACCESS,
                                                    NULL,
                                                    KernelMode,
                                                    &EmergencyWorkerThread[i],
                                                    NULL);
                    ZwClose(threadHandle);
                    if (!NT_SUCCESS(nts)) {
                        break;
                    }
                }
            }

            EmergencyWorkerThreadCreated = (i==USBPORT_MAX_EMERGENCY_WORKER_THREAD);
        }

    }

    KeSetEvent(&EmergencyWorkerThreadEvent, 0, FALSE);

    return nts;
}



