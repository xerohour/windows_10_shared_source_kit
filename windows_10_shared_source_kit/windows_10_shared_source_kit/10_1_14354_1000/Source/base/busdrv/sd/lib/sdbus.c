/*++

Copyright (c) 2002  Microsoft Corporation

Module Name:

    sdbus.c

Abstract:

Author:

    Neil Sandlin (neilsa) 1-Jan-02

Environment:

    Kernel mode only.

--*/
#include <ntddk.h>
#include "ntddsd.h"
#include "sdbuslib.h"
#include <dontuse.h>

//
// Prototypes
//

NTSTATUS
SdbusIoctlCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PKEVENT pdoIoCompletedEvent
    );


//
// External entry points
//


NTSTATUS
SdBusOpenInterface(
    IN PDEVICE_OBJECT Pdo,
    IN OUT PSDBUS_INTERFACE_STANDARD Interface,
    IN USHORT Size,
    IN USHORT Version
    )
{
    NTSTATUS             status = STATUS_INVALID_PARAMETER;
    KEVENT               event;
    IO_STATUS_BLOCK      statusBlock = {0};
    PIRP                 irp;
    PIO_STACK_LOCATION   irpSp;
    PDEVICE_OBJECT       topmostDevice = NULL;

    try {
        topmostDevice = IoGetAttachedDeviceReference(Pdo);

        KeInitializeEvent(&event, NotificationEvent, FALSE);

        irp = IoBuildSynchronousFsdRequest(IRP_MJ_PNP,
                                           topmostDevice,
                                           NULL,
                                           0,
                                           0,
                                           &event,
                                           &statusBlock);

        if (!irp) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            leave;
        }

        irp->IoStatus.Status = STATUS_NOT_SUPPORTED ;
        irp->IoStatus.Information = 0;

        irpSp = IoGetNextIrpStackLocation(irp);

        irpSp->MinorFunction = IRP_MN_QUERY_INTERFACE;

        irpSp->Parameters.QueryInterface.InterfaceType= &GUID_SDBUS_INTERFACE_STANDARD;
        irpSp->Parameters.QueryInterface.Size = Size;
        irpSp->Parameters.QueryInterface.Version = Version;
        irpSp->Parameters.QueryInterface.Interface = (PINTERFACE) Interface;

        status = IoCallDriver(topmostDevice, irp);

        if (status == STATUS_PENDING) {
            KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
            status = statusBlock.Status;
        }
    } finally {
        ObDereferenceObject(topmostDevice);
    }
    return status;
}



NTSTATUS
SdBusSubmitRequest(
    IN PSDBUS_INTERFACE_CONTEXT InterfaceContext,
    IN PSDBUS_REQUEST_PACKET Sdrp
    )
{
    NTSTATUS status;
    PIRP irp;
    IO_STATUS_BLOCK statusBlock = {0};
    KEVENT event;

    if ((InterfaceContext == NULL) ||
        (InterfaceContext->Signature != SDBUS_INTERFACE_SIGNATURE) ||
        (InterfaceContext->TargetObject == NULL)) {
        return STATUS_DELETE_PENDING;
    }


    irp = IoAllocateIrp(InterfaceContext->TargetObject->StackSize, FALSE);
    if (!irp) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    irp->UserIosb = &statusBlock;

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    status = SdBusSubmitRequestAsync(InterfaceContext,
                                     Sdrp,
                                     irp,
                                     SdbusIoctlCompletion,
                                     &event);

    if (status == STATUS_PENDING) {
        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);
        status = statusBlock.Status;
        Sdrp->Information = statusBlock.Information;
    }

    IoFreeIrp(irp);
    return status;
}



NTSTATUS
SdBusSubmitRequestAsync(
    IN PSDBUS_INTERFACE_CONTEXT InterfaceContext,
    IN PSDBUS_REQUEST_PACKET Sdrp,
    IN PIRP Irp,
    IN PIO_COMPLETION_ROUTINE CompletionRoutine,
    IN PVOID UserContext
    )
{
    IO_STATUS_BLOCK statusBlock;
    NTSTATUS status;
    PIO_STACK_LOCATION irpSp;

    irpSp = IoGetNextIrpStackLocation(Irp);
    irpSp->MajorFunction = IRP_MJ_INTERNAL_DEVICE_CONTROL;

    irpSp->Parameters.DeviceIoControl.OutputBufferLength = 0;
    irpSp->Parameters.DeviceIoControl.InputBufferLength = sizeof(SDBUS_REQUEST_PACKET);
    irpSp->Parameters.DeviceIoControl.IoControlCode = IOCTL_SD_SUBMIT_REQUEST;
    irpSp->Parameters.DeviceIoControl.Type3InputBuffer = Sdrp;

    IoSetCompletionRoutine(Irp,
                           CompletionRoutine,
                           UserContext,
                           TRUE,
                           TRUE,
                           TRUE);

    if ((InterfaceContext == NULL) ||
        (InterfaceContext->Signature != SDBUS_INTERFACE_SIGNATURE) ||
        (InterfaceContext->TargetObject == NULL)) {

        IoSetNextIrpStackLocation(Irp);
        Irp->IoStatus.Status = status = STATUS_DELETE_PENDING;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }

    status = IoCallDriver(InterfaceContext->TargetObject, Irp);

    return status;

}



NTSTATUS
SdbusIoctlCompletion(
    IN PDEVICE_OBJECT DeviceObject,
    IN PIRP Irp,
    IN PKEVENT pdoIoCompletedEvent
    )
{
    Irp->UserIosb->Status = Irp->IoStatus.Status;
    Irp->UserIosb->Information = Irp->IoStatus.Information;
    KeSetEvent(pdoIoCompletedEvent, IO_NO_INCREMENT, FALSE);
    return STATUS_MORE_PROCESSING_REQUIRED;
}

