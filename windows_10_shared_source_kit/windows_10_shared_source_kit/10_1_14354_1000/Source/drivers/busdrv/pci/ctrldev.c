/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    ctrldev.c

Abstract:

    This module contains functions for a control device path.  This device
    object is used for sending IOCTLs to the driver itself, rather than any of
    the specific PCI devices in the system.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// ------------------------------------------------------------------ Prototypes

NTSTATUS
PciQueryDeviceUsage(
    _In_ const PIRP Irp
    );

// --------------------------------------------------------------------- Pragmas
#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, PciCreateControlDevice)
#pragma alloc_text(PAGE, PciCleanupCtrlDevExt)
#pragma alloc_text(PAGE, PciControl_DeviceControl)
#pragma alloc_text(PAGE, PciQueryDeviceUsage)
#endif


// --------------------------------------------------------------------- Globals

PDEVICE_OBJECT PciControlDevice;

// ------------------------------------------------------------------- Functions


NTSTATUS
PciQueryDeviceUsage(
    _In_ const PIRP Irp
    )

/*++

Routine Description:

    This routine responds to IOCTL_PCIDRIVE_QUERY_DEVICE_USAGE.  The purpose
    is to allow the virtualization stack (Hyper-V) to know if any particular
    device is on one of the special file paths, so that it can display feedback
    to a user telling them why the device can't be marked as offline, and
    available to a guest VM.

Arguments:

    Irp - The IRP that is the IOCTL.

Return Value:

    NTSTATUS

--*/

{
    PPCI_QUERY_DEVICE_USAGE_INPUT InputBuffer;
    PIO_STACK_LOCATION IrpSp;
    PPCI_QUERY_DEVICE_USAGE_OUTPUT OutputBuffer;
    NTSTATUS Status;

    PAGED_CODE();

    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    InputBuffer = (PPCI_QUERY_DEVICE_USAGE_INPUT)
                  Irp->AssociatedIrp.SystemBuffer;

    OutputBuffer = (PPCI_QUERY_DEVICE_USAGE_OUTPUT)
                   Irp->AssociatedIrp.SystemBuffer;

    if (IrpSp->Parameters.DeviceIoControl.InputBufferLength <
        sizeof(PCI_QUERY_DEVICE_USAGE_INPUT)) {

        Status = STATUS_INVALID_PARAMETER;
        goto cleanup;
    }

    if (IrpSp->Parameters.DeviceIoControl.OutputBufferLength <
        sizeof(PCI_QUERY_DEVICE_USAGE_OUTPUT)) {

        Status = STATUS_INVALID_PARAMETER;
        goto cleanup;
    }

    Status = PciQueryDeviceUsageInTree(InputBuffer->BusNumber,
        InputBuffer->DeviceNumber, InputBuffer->FunctionNumber, OutputBuffer);

    if (!NT_SUCCESS(Status)) {
        ERROR("QueryDeviceUsage(bus:%d, dev:%d, func:%d) failed (0x%08X)\n",
            InputBuffer->BusNumber, InputBuffer->DeviceNumber,
            InputBuffer->FunctionNumber, Status);

        goto cleanup;
    }

    Irp->IoStatus.Information = sizeof(PCI_QUERY_DEVICE_USAGE_OUTPUT);
    Status = STATUS_SUCCESS;

cleanup:

    return Status;
}


VOID
PciCleanupCtrlDevExt(
    _Inout_ PCONTROL_DEVICE_EXTENSION Extension
    )

/*++

Routine Description:

    This routine cleans up the PCI control device extension.

Arguments:

    Extension - Supplies the device extension to cleanup.

Return Value:

    None.

--*/

{
    PAGED_CODE();

    PCI_ASSERT(Extension != NULL);

    if (Extension->MultiSzProxyDevicePath != NULL) {
        PciFreePool(Extension->MultiSzProxyDevicePath);
    }

    RtlZeroMemory(Extension, sizeof(CONTROL_DEVICE_EXTENSION));
}


NTSTATUS
PciCreateControlDevice(
    VOID
    )

/*++

Routine Description:

    This routine creates a control device object for the PCI driver.  This
    device object is used for sending IOCTLs to the driver itself, rather than
    any of the specific PCI devices in the system.

Arguments:

    None.

Return Value:

    NT status.

--*/

{
    UNICODE_STRING DeviceName;
    UNICODE_STRING DosName;
    PCONTROL_DEVICE_EXTENSION Extension;
    DECLARE_CONST_UNICODE_STRING(Sddl, L"D:P(A;;GA;;;SY)(A;;GA;;;BA)(A;;GA;;;NS)");
    NTSTATUS Status;

    PAGED_CODE();

    RtlInitUnicodeString(&DeviceName, DD_PCI_CONTROL_DEVICE_NAME);
    Extension = NULL;
    Status = STATUS_SUCCESS;

    Status = IoCreateDeviceSecure(PciDriverObject,
                                  sizeof(CONTROL_DEVICE_EXTENSION),
                                  &DeviceName,  // DeviceName, Optional.
                                  FILE_DEVICE_UNKNOWN,  // DeviceType
                                  FILE_DEVICE_SECURE_OPEN, //Characteristics
                                  FALSE,    // Exclusive
                                  &Sddl,
                                  NULL,  // DeviceClassGuid
                                  &PciControlDevice);

    if (!NT_SUCCESS(Status)) {
        goto cleanup;
    }

    RtlInitUnicodeString(&DosName, DD_PCI_CONTROL_SYMBOLIC_LINK);
    IoCreateSymbolicLink(&DosName, &DeviceName);

    Extension = (PCONTROL_DEVICE_EXTENSION)PciControlDevice->DeviceExtension;
    RtlZeroMemory(Extension, sizeof(CONTROL_DEVICE_EXTENSION));
    KeInitializeEvent(&Extension->ProxyDeviceLock, SynchronizationEvent, TRUE);
    Extension->Signature = PCI_CONTROLDEVICE_SIGNATURE;
    Status = VmProxyReadDevicePathsFromRegistry(Extension);
    if (!NT_SUCCESS(Status)) {
        goto cleanup;
    }

    //
    // We can receive IRPs now...
    //

    PciControlDevice->Flags &= ~DO_DEVICE_INITIALIZING;

    Status = STATUS_SUCCESS;

cleanup:

    if (!NT_SUCCESS(Status) && PciControlDevice != NULL) {
        PciCleanupCtrlDevExt(
            (PCONTROL_DEVICE_EXTENSION)PciControlDevice->DeviceExtension);

        IoDeleteDevice(PciControlDevice);
        PciControlDevice = NULL;
    }

    return Status;
}

NTSTATUS
PciControl_DeviceControl(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP           Irp
    )

/*++

Routine Description:

    This routine handles the IRP_MJ_DEVICE_CONTROL IRP for PCI control device.

Arguments:

    DeviceObject - Pointer to the device object for which this IRP applies.

    Irp - Pointer to the IRP to dispatch.

Return Value:

    NT status.

--*/

{
    PCONTROL_DEVICE_EXTENSION Extension;
    PIO_STACK_LOCATION  IrpSp;
    NTSTATUS Status;

    PAGED_CODE();

    Extension = (PCONTROL_DEVICE_EXTENSION) DeviceObject->DeviceExtension;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    Status = STATUS_SUCCESS;

    switch (IrpSp->Parameters.DeviceIoControl.IoControlCode) {

        case IOCTL_PCIDRIVE_ADD_VMPROXYPATH:
            Status = VmProxyAddRemovePath(Extension, Irp, TRUE);
            break;

        case IOCTL_PCIDRIVE_REMOVE_VMPROXYPATH:
            Status = VmProxyAddRemovePath(Extension, Irp, FALSE);
            break;

        case IOCTL_PCIDRIVE_CLEAR_VMPROXYPATH:
            Status = VmProxyClearPath(Extension);
            break;

        case IOCTL_PCIDRIVE_QUERY_DEVICE_USAGE:
            Status = PciQueryDeviceUsage(Irp);
            break;

        default:
            //
            // Unrecognized IOCTL. Fail the invalid request.
            //
            Status = STATUS_INVALID_DEVICE_REQUEST;
            break;
    }

    PciCompleteRequest(Irp, Status);

    return Status;
}
