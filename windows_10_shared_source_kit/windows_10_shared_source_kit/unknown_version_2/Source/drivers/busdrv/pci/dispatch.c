/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    dispatch.c

Abstract:

    This module contains all of the data variables that are used for
    dispatching IRPs in the PCI Driver. The major Irp tables might
    be assigned as follows:

                                   +-- PCI Bus ---------IRP---+
                                   | FDO: PciFdoDispatchTable |
                                   | PDO:                     |
                                   +--------------------------+

                        +-- PCI Bus ---------IRP---+
                        | FDO: PciFdoDispatchTable |
                        | PDO: PciPdoDispatchTable |
                        +--------------------------+

 +-- PCI Device -----------IRP---+  +-- Cardbus Device -------IRP---+
 | FDO:                          |  | FDO:                          |
 | PDO: PciPdoDispatchTable      |  | PDO:                          |
 +-------------------------------+  +-------------------------------+















--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

IO_COMPLETION_ROUTINE PciSetEventCompletion;

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciDispatchSystemControl)
    #pragma alloc_text(PAGE, PciDispatchCreate)
    #pragma alloc_text(PAGE, PciCallDownIrpStack)
#endif

// --------------------------------------------------------------------- Globals

extern const PCI_MN_DISPATCH_ENTRY PciDevice_DispatchPnpTable[];
extern const PCI_MN_DISPATCH_ENTRY PciDevice_DispatchPowerTable[];
extern const PCI_MN_DISPATCH_ENTRY PciBus_DispatchPnpTable[];
extern const PCI_MN_DISPATCH_ENTRY PciBus_DispatchPowerTable[];

// ------------------------------------------------------------------- Functions

//
// Disable prefast warnings for annotation inconsistency.  These warnings 
// should be disabled until the definition of DRIVER_DISPATCH can be 
// updated to match this annotation.
//

#pragma prefast(push)
#pragma prefast(disable: __WARNING_UNMATCHED_DECL_ANNO)
#pragma prefast(disable: __WARNING_UNMATCHED_DEFN_ANNO)

NTSTATUS
PciDispatchCreate(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    )

/*++

Routine Description:

    This routine is the dispatch for IRP_MJ_CREATE. It is called when a device
    handle is opened. Only control device supports this IRP. The driver fails
    the request for other devices.

Arguments:

    DeviceObject - Supplies the device object.

    Irp - Supplies the irp.

Return Value:

    NTSTATUS

--*/

{
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();

    PCI_ASSERT(DeviceObject->DeviceExtension != NULL);

    if (((PCONTROL_DEVICE_EXTENSION)DeviceObject->DeviceExtension)->Signature !=
        PCI_CONTROLDEVICE_SIGNATURE) {

        status = STATUS_INVALID_DEVICE_REQUEST;
    }

    Irp->IoStatus.Information = 0;
    PciCompleteRequest(Irp, status);

    return status;
}

NTSTATUS
PciDispatchPnpPower(
    __in PDEVICE_OBJECT DeviceObject,
    __inout PIRP Irp
    )
/*++

Routine Description:

    This is the top level dispatch routine for both PnP and Power IRPs.
    The handlers for each minor code are in tables, as are the
    dispatch "styles" for these handlers.  These styles indicate whether
    the IRP should be handled on the way down or up the stack, or if
    the IRP should be unconditionally completed after the handler is
    called (always true for PDO handlers) or if control of the IRP
    should be explicitly handed over to the dispatch routine.

    Control device is not a pnp device and should not receive IRP_MJ_PNP/POWER.

Arguments:

    DeviceObject - The device object this IRP is directed at.

    Irp - The IRP to dispatch.

Return Value:

    NT Status code.

--*/
{
    PCPCI_MN_DISPATCH_ENTRY minorTable, dispatchTableEntry;
    PCI_MN_DISPATCH_FUNCTION dispatchHandler;
    PPCI_COMMON_EXTENSION deviceExtension;
    PCI_DISPATCH_STYLE dispatchStyle;
    PIO_STACK_LOCATION irpSp;
    BOOLEAN IsBusExtension;
    ULONG minorTableMax;
    ULONG majorFunction;
    BOOLEAN NeedAccessConfigSpace;
    BOOLEAN passDown;
    NTSTATUS status;

    NON_PAGED_CODE();

    PCI_ASSERT(PCI_DEVICE_OBJECT(DeviceObject));

    irpSp = IoGetCurrentIrpStackLocation(Irp);

    IsBusExtension = FALSE;
    deviceExtension = ((PPCI_COMMON_EXTENSION)(DeviceObject->DeviceExtension));

    if (deviceExtension->DeviceState == PciDeleted) {

        //
        // We should not be getting IRPs. Fail the invalid request.
        //
        status = STATUS_NO_SUCH_DEVICE;
        passDown = FALSE;
        goto exit;
    }

    //
    // Get the correct IRP handler.
    //
    majorFunction = irpSp->MajorFunction;

    switch(majorFunction) {

        case IRP_MJ_PNP:

            if (PCI_BUS_EXTENSION(deviceExtension)) {
                IsBusExtension = TRUE;
                minorTable = PciBus_DispatchPnpTable;
            } else {
                minorTable = PciDevice_DispatchPnpTable;
            }

            minorTableMax = PCI_MAX_MINOR_PNP_IRP;
            break;

        case IRP_MJ_POWER:

            if (PCI_BUS_EXTENSION(deviceExtension)) {
                IsBusExtension = TRUE;
                minorTable = PciBus_DispatchPowerTable;
            } else {
                minorTable = PciDevice_DispatchPowerTable;
            }

            minorTableMax = PCI_MAX_MINOR_POWER_IRP;
            break;

        default:

            PCI_ASSERT(FALSE);
            passDown = FALSE;
            status = STATUS_INVALID_PARAMETER;
            goto exit;
    }

    //
    // Grab the appropriate dispatch handler from the table. The last chance
    // handler is always at the end of the table. Grab the dispatch style too.
    //

    dispatchTableEntry = (irpSp->MinorFunction <= minorTableMax) 
                            ? &minorTable[irpSp->MinorFunction] 
                            : &minorTable[minorTableMax+1];

    dispatchStyle   = dispatchTableEntry->DispatchStyle;
    dispatchHandler = dispatchTableEntry->DispatchFunction;
    NeedAccessConfigSpace = dispatchTableEntry->RequireConfigSpaceAccess;

    if (DebugBreakOnIrp(DeviceObject, irpSp)) {
        KdBreakPoint();
    }

#if 0
    //
    // Intentionally noisy trace point
    //

    TraceLoggingWrite(
        PciTraceLoggingProvider,
        "PciDispatchPnpPower",
        TraceLoggingString("PCI Dispatcher:"),
        TraceLoggingInt32(irpSp->MajorFunction),
        TraceLoggingInt32(irpSp->MinorFunction),
        TraceLoggingInt32(dispatchStyle),
        TraceLoggingPointer(DeviceObject),
        TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES)
        );
#endif

    //
    // For now, if handlers want to see the IRP after completion, pass it down
    // synchronously. Later we can get more fancy.
    //

    if (dispatchStyle == IRP_UPWARD) {

        status = PciCallDownIrpStack(deviceExtension, Irp);
        if (!NT_SUCCESS(status) && (status != STATUS_NOT_SUPPORTED)) {
            passDown = FALSE;
            goto exit;            
        }
    }

    //
    // Call the handler
    //
    DebugDumpIrpStack(irpSp, deviceExtension, minorTableMax);

    if ((NeedAccessConfigSpace != FALSE) && (IsBusExtension == FALSE)) {
        if (((PPCI_DEVICE)deviceExtension)->Parent != NULL) {
            PciBus_PowerReference(((PPCI_DEVICE)deviceExtension)->Parent,
                                  PciBusPowerReasonChildNeedsConfigAccess);
        }
    }

    __analysis_assume(dispatchHandler != NULL);
    status = (dispatchHandler)(Irp, irpSp, deviceExtension);

    if ((NeedAccessConfigSpace != FALSE) && (IsBusExtension == FALSE)) {
        if (((PPCI_DEVICE)deviceExtension)->Parent != NULL) {
            PciBus_PowerDereference(((PPCI_DEVICE)deviceExtension)->Parent,
                                    PciBusPowerReasonChildNeedsConfigAccess);
        }
    }

    DebugDumpIrpStatus(majorFunction, status);

    if (status == STATUS_PENDING) {

        return status;
    }

    //
    // Post-op. Update IRP status and send Irp along it's way iff appropriate.
    //
    switch (dispatchStyle) {

        //
        // For this style, the IRP is being handled entirely our handler. Touch
        // nothing.
        //
        case IRP_DISPATCH:
            return status;

        //
        // For this style, the IRP status will be appropriately updated iff
        // status != STATUS_NOT_SUPPORTED. The IRP will be completed or
        // passed down appropriately.
        //
        case IRP_DOWNWARD:
            passDown = TRUE;
            break;

        //
        // For this style, the IRP will be completed and have it's status
        // appropriately updated iff status != STATUS_NOT_SUPPORTED
        //
        case IRP_COMPLETE:
            passDown = FALSE;
            break;

        //
        // For this style, the IRP status will be appropriately updated iff
        // status != STATUS_NOT_SUPPORTED. The IRP has already been sent down,
        // and must be completed.
        //
        case IRP_UPWARD:
            passDown = FALSE;
            break;

        default:
            PCI_ASSERT(FALSE);
            passDown = FALSE;
            break;
    }

exit:

    //
    // STATUS_NOT_SUPPORTED is used as a signal that the dispatch handler
    // does not know what to do with the IRP. In that case, we must leave
    // the status untouched, otherwise we update it. In both cases, return
    // the correct status value.
    //
    if (status == STATUS_NOT_SUPPORTED) {

        status = Irp->IoStatus.Status;
    }

    Irp->IoStatus.Status = status;

    if (passDown && (NT_SUCCESS(status) || (status == STATUS_NOT_SUPPORTED))) {
        status = PciPassIrp(deviceExtension, Irp);

    } else {
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }

    return status;
}

NTSTATUS
PciDispatchSystemControl(
    __in PDEVICE_OBJECT DeviceObject,
    __inout PIRP Irp
    )
/*++

Routine Description:

    This routine dispatches the IRP_MJ_SYSTEM_CONTROL IRP.
    Since PCI doesn't do anything with this IRP, this
    routine just passes the IRP down (for an FDO) or
    completes it untouched (for a PDO).

    The control device is not reported to Pnp and it has no PDO. The driver
    should fail the request for control device.

Arguments:

    DeviceObject - Pointer to the device object for which this IRP applies.

    Irp - Pointer to the IRP to dispatch.

Return Value:

    NT status.

--*/
{
    PPCI_COMMON_EXTENSION extension;
    NTSTATUS status;

    PAGED_CODE();

    PCI_ASSERT(PCI_DEVICE_OBJECT(DeviceObject));

    extension = ((PPCI_COMMON_EXTENSION)(DeviceObject->DeviceExtension));

    PCI_ASSERT(extension != NULL);

    if (extension->Signature == PCI_CONTROLDEVICE_SIGNATURE) {

        //
        // We don't support WMI for the control device.
        //

        status = STATUS_INVALID_DEVICE_REQUEST;
        PciCompleteRequest(Irp, status);
        return status;
    }

    if (extension->DeviceState == PciDeleted) {

        //
        // We should not be getting IRPs. Fail the invalid request.
        //
        status = STATUS_NO_SUCH_DEVICE;
        PciCompleteRequest(Irp, status);

        return status;
    }

    if (PCI_BUS_EXTENSION(extension)) {

        status = PciPassIrp(extension, Irp);

    } else {

        status = Irp->IoStatus.Status;
        PciCompleteRequest(Irp, status);
    }

    return status;
}

NTSTATUS
PciDispatchDeviceControl(
    __in PDEVICE_OBJECT DeviceObject,
    __inout PIRP Irp
    )
/*++

Routine Description:

    This routine handles the IRP_MJ_DEVICE_CONTROL IRP. The PCI driver supports
    IOCTLs for the control device only. For other devices the routine will
    complete the irp with a failure (if PDO) or pass it untouched (if FDO).

Arguments:

    DeviceObject - Pointer to the device object for which this IRP applies.

    Irp - Pointer to the IRP to dispatch.

Return Value:

    NT status.

--*/
{
    PPCI_COMMON_EXTENSION extension;
    NTSTATUS status;

    NON_PAGED_CODE();

    PCI_ASSERT(PCI_DEVICE_OBJECT(DeviceObject));

    extension = ((PPCI_COMMON_EXTENSION)(DeviceObject->DeviceExtension));

    if (extension && extension->Signature == PCI_CONTROLDEVICE_SIGNATURE) {
        status = PciControl_DeviceControl(DeviceObject, Irp);
        return status;
    }

    if (extension->DeviceState == PciDeleted) {

        //
        // We should not be getting IRPs. Fail the invalid request.
        //
        status = STATUS_NO_SUCH_DEVICE;
        PciCompleteRequest(Irp, status);

        return status;
    }

    if (PCI_BUS_EXTENSION(extension)) {

        status = PciPassIrp(extension, Irp);

    } else {

        status = STATUS_INVALID_DEVICE_REQUEST;
        PciCompleteRequest(Irp, status);
    }

    return status;
}

#pragma prefast(pop)

NTSTATUS
PciPassIrp(
    __in PPCI_COMMON_EXTENSION DeviceExtension,
    __inout PIRP Irp
    )
/*++

Description:

    Given an FDO, pass the IRP to the next device object in the
    device stack.  This is the PDO if there are no lower level
    filters.

    Note: This routine is used only if we do not expect to do
    any further processing on this IRP at this level.

    Note: For Power IRPs, the next power IRP is *not* started.

Arguments:

    DeviceObject - Pointer to the device object for which this IRP applies.

    Irp - Pointer to the IRP to dispatch.

Return Value:

    NT Status code.

--*/
{
    PPCI_BUS bus;

    NON_PAGED_CODE();

    TRACE(DISPATCH_TRACE_LEVEL, "PciPassIrp ...\n");

    PCI_ASSERT(PCI_BUS_EXTENSION(DeviceExtension));

    bus = (PPCI_BUS)DeviceExtension;

    //
    // Call the PDO driver with the request.
    //
    IoSkipCurrentIrpStackLocation(Irp);
    return IoCallDriver(bus->AttachedDeviceObject, Irp);
}

NTSTATUS
PciCallDownIrpStack(
    __in PPCI_COMMON_EXTENSION DeviceExtension,
    __inout PIRP Irp
    )
/*++

Description:

    Pass the IRP to the next device object in the device stack.  This
    routine is used when more processing is required at this level on
    this IRP on the way back up.

Arguments:

    DeviceObject - Pointer to the device object for which this IRP applies.

    Irp - Pointer to the IRP to dispatch.

Return Value:

    NT Status code.

--*/
{
    NTSTATUS status;
    KEVENT event;
    PPCI_BUS bus;

    PAGED_CODE();

    TRACE(DISPATCH_TRACE_LEVEL, "PciCallDownIrpStack ...\n");

    PCI_ASSERT(PCI_BUS_EXTENSION(DeviceExtension));

    bus = (PPCI_BUS)DeviceExtension;

    KeInitializeEvent(&event, SynchronizationEvent, FALSE);

    //
    // Set our completion routine
    //
    IoCopyCurrentIrpStackLocationToNext(Irp);
    IoSetCompletionRoutine(Irp,
                           PciSetEventCompletion,
                           &event,
                           TRUE,
                           TRUE,
                           TRUE
                           );

    status = IoCallDriver(bus->AttachedDeviceObject, Irp);

    //
    // If we did things asynchronously then wait on our event
    //
    if (status == STATUS_PENDING) {

        //
        // We do a KernelMode wait so that our stack where the event is
        // doesn't get paged out!
        //
        KeWaitForSingleObject(&event, Executive, KernelMode, FALSE, NULL);

        status = Irp->IoStatus.Status;
    }

    return status;
}

NTSTATUS
PciIrpNotSupported(
    __in PIRP Irp,
    __in PIO_STACK_LOCATION IrpSp,
    __in PPCI_COMMON_EXTENSION DeviceExtension
    )
{
    NON_PAGED_CODE();

    UNREFERENCED_PARAMETER(Irp);
    UNREFERENCED_PARAMETER(IrpSp);
    UNREFERENCED_PARAMETER(DeviceExtension);

    return STATUS_NOT_SUPPORTED;
}

_Function_class_(IO_COMPLETION_ROUTINE)
_IRQL_requires_same_
NTSTATUS
PciSetEventCompletion(
    _In_ PDEVICE_OBJECT DeviceObject,
    _In_ PIRP Irp,
    _In_reads_opt_(_Inexpressible_("varies")) PVOID Context
    )
/*++

Routine Description:

    This routine is used as a completion routine when an IRP is passed
    down the stack but more processing must be done on the way back up.
    The effect of using this as a completion routine is that the IRP
    will not be destroyed in IoCompleteRequest as called by the lower
    level object.  The event which is a KEVENT is signaled to allow
    processing to continue

Arguments:

    DeviceObject - Supplies the device object

    Irp - The IRP we are processing

    Event - Supplies the event to be signaled

Return Value:

    STATUS_MORE_PROCESSING_REQUIRED

--*/
{
    PKEVENT Event = (PKEVENT)Context;

    NON_PAGED_CODE();

    PCI_ASSERT(Event);
    _Analysis_assume_(Event != NULL);

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(Irp);

    KeSetEvent(Event, IO_NO_INCREMENT, FALSE);
    return STATUS_MORE_PROCESSING_REQUIRED;
}
