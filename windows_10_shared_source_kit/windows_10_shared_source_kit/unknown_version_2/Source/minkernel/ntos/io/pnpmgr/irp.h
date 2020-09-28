/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    irp.h

Abstract:

    This module implements the sending of PNP IRPs.

Author:

    Santosh Jodh (santoshj) 08-10-2004

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

typedef struct _PNP_DIAGNOSTIC_COMPLETION_CONTEXT {

    PDRIVER_OBJECT  DriverObject;
    KEVENT          Event;
    NTSTATUS        Status;

} PNP_DIAGNOSTIC_COMPLETION_CONTEXT, *PPNP_DIAGNOSTIC_COMPLETION_CONTEXT;

NTSTATUS
PnpQueryDeviceText (
    __in PDEVICE_OBJECT DeviceObject,
    __in DEVICE_TEXT_TYPE DeviceTextType,
    __in LCID POINTER_ALIGNMENT LocaleId,
    __deref_out PWSTR *Description
   );

NTSTATUS
PpIrpQueryCapabilities(
    __in PDEVICE_OBJECT DeviceObject,
    __out PDEVICE_CAPABILITIES Capabilities
    );

NTSTATUS
PpIrpQueryResourceRequirements(
    __in PDEVICE_OBJECT DeviceObject,
    __out PIO_RESOURCE_REQUIREMENTS_LIST *Requirements
   );

NTSTATUS
PnpIrpQueryID (
    __in PDEVICE_OBJECT DeviceObject,
    __in BUS_QUERY_ID_TYPE IDType,
    __deref_out PWSTR *ID
    );

NTSTATUS
PpIrpQueryBusInformation(
    __in PDEVICE_OBJECT DeviceObject,
    __out PPNP_BUS_INFORMATION *BusInfo
    );

NTSTATUS
PiIrpQueryRemoveDevice (
    __in PDEVICE_OBJECT TargetDevice,
    __in_opt PDRIVER_OBJECT *FailingDriver
    );

NTSTATUS
IopQueryReconfiguration(
    __in UCHAR Request,
    __in PDEVICE_OBJECT DeviceObject
    );

NTSTATUS
IopQueryResourceHandlerInterface(
    __in RESOURCE_HANDLER_TYPE HandlerType,
    __in PDEVICE_OBJECT DeviceObject,
    __in UCHAR ResourceType,
    __out PVOID *Interface
    );

NTSTATUS
IopEjectDevice(
    __in PDEVICE_OBJECT DeviceObject,
    __inout PPENDING_RELATIONS_LIST_ENTRY PendingEntry
    );

VOID
IopCancelPendingEject(
    __in PPENDING_RELATIONS_LIST_ENTRY Entry
    );

NTSTATUS
IopRemoveDevice(
    __in PDEVICE_OBJECT TargetDevice,
    __in ULONG IrpMinorCode
    );

NTSTATUS
IopQueryDeviceState(
    __in PDEVICE_OBJECT DeviceObject,
    __out PPNP_DEVICE_STATE DeviceState
    );

NTSTATUS
IopQueryDeviceResources(
    __in PDEVICE_OBJECT DeviceObject,
    __in ULONG ResourceType,
    __out PVOID *Resource,
    __out ULONG *Length
    );

NTSTATUS
IopQueryLegacyBusInformation (
    __in PDEVICE_OBJECT DeviceObject,
    __out_opt LPGUID InterfaceGuid           ,
    __out_opt INTERFACE_TYPE *InterfaceType  ,
    __out_opt ULONG *BusNumber
    );

VOID
IopIncDisableableDepends(
    __inout PDEVICE_NODE DeviceNode
    );

VOID
IopDecDisableableDepends(
    __inout PDEVICE_NODE DeviceNode
    );

NTSTATUS
IopQueryDockRemovalInterface(
    __in      PDEVICE_OBJECT  DeviceObject,
    __inout  PDOCK_INTERFACE *DockInterface
    );

NTSTATUS
PnpAsynchronousCall (
    __in      PDEVICE_OBJECT           TargetDevice,
    __in      PIO_STACK_LOCATION       TopStackLocation,
    __in_opt  PIO_COMPLETION_ROUTINE   CompletionRoutine,
    __in_opt  PVOID                    CompletionContext
    );

IO_COMPLETION_ROUTINE PnpDeviceCompletionRoutine;

NTSTATUS
PnpStartDevice (
    __in      PDEVICE_OBJECT            Device,
    __in_opt  PIO_COMPLETION_ROUTINE    CompletionRoutine,
    __in_opt  PVOID                     Context
    );

IO_COMPLETION_ROUTINE IopDeviceEjectComplete;

NTSTATUS
IopFilterResourceRequirementsCall(
    __in PDEVICE_OBJECT DeviceObject,
    __in_opt PIO_RESOURCE_REQUIREMENTS_LIST ResReqList,
    __out PVOID *Information
    );

IO_COMPLETION_ROUTINE PnpDiagnosticCompletionRoutine;

NTSTATUS
PnpIrpDeviceEnumerated(
    __in PDEVICE_OBJECT DeviceObject
    );

KINLINE
VOID
PnpInitializeStackLocation (
    __inout PIO_STACK_LOCATION   IrpSp,
    __in    UCHAR                MinorFunction
    )

/*++

Description

    This function initializes the top stack location for all PNP IRPs.

Parameters

    IrpSp - Pointer to the top stack location for this IRP

    MinorFunction - Specific minor function code for IRP_MJ_PNP

Return

    None.

--*/

{
    PAGED_CODE();

    PNP_ASSERT(IrpSp);

    //
    // Clear everything.
    //
    RtlZeroMemory(IrpSp, sizeof(IO_STACK_LOCATION));

    //
    // All our IRPs share the same major function code.
    //
    IrpSp->MajorFunction = IRP_MJ_PNP;

    //
    // Set the specified minor function code.
    //
    IrpSp->MinorFunction = MinorFunction;

    return;
}

KINLINE
NTSTATUS
PnpSendIrp (
    __in     PDEVICE_OBJECT          DeviceObject,
    __inout  PIO_STACK_LOCATION      TopStackLocation,
    __in_opt ULONG_PTR               Information,
    __in_opt PIO_COMPLETION_ROUTINE  CompletionRoutine,
    __in_opt PVOID                   CompletionContext
    )

/*++

Description

    This function sends the specified IRP_MJ_PNP to the device.

Parameters

    DeviceNode - Pointer to the device being targetted

    TopStackLocation - Pointer to the top stack location for this IRP

    Information - Default return information

    CompletionRoutine - Completion routine for asynchronous IRPs

    CompletionContext - Context for the completion routine. If CompletionRoutine is NULL,
                        this recieves the information returned by the IRP.

Return

    NTSTATUS.

--*/

{
    LOGICAL restoreAffinity;
    NTSTATUS status;
    GROUP_AFFINITY oldAffinity;

    PAGED_CODE();

    PNP_ASSERT(DeviceObject);
    PNP_ASSERT(TopStackLocation);
    PNP_ASSERT(TopStackLocation->MajorFunction == IRP_MJ_PNP);

    //
    // For devices in or close to a NUMA node, we send PNP IRPs to drivers on one of the
    // processors in that NUMA node. This is to provide benefit of proximity transparently
    // to drivers that are not NUMA aware.
    //
    restoreAffinity = PnpSetDeviceAffinityThread(DeviceObject, &oldAffinity);

    //
    // Send the actual IRP.
    //
    if (CompletionRoutine) {
        status = PnpAsynchronousCall(DeviceObject,
                                     TopStackLocation,
                                     CompletionRoutine,
                                     CompletionContext);

    } else {
        status = IopSynchronousCall(DeviceObject,
                                    TopStackLocation,
                                    (Information)? STATUS_SUCCESS : STATUS_NOT_SUPPORTED,
                                    Information,
                                    (PULONG_PTR)CompletionContext);
    }

    if (restoreAffinity != FALSE) {

        //
        // Return to the original processor if affinity was changed before calling the
        // driver.
        //

        KeRevertToUserGroupAffinityThread(&oldAffinity);
    }

    return status;
}
