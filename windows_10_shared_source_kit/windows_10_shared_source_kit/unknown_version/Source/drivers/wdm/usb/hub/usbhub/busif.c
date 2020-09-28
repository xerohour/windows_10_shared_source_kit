
/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    bus.c

     3-10-2002

Abstract:

    Provides an interface to the underlying USBPORT stack through PnP
    BusInterfaces.

    This module contains code to link to both the USBDI bus interface and
    USBHUB bus interface.  Once linked we call the bus interface functions
    through the hub FDO.

Module:

    Entry points:

         PNP event                       Function
    --------------------------------------------------
    Entry -->linked to DriverUnload     NULL
    Unload-->linked to DriverEntry      NULL
    Start -->linked to PNP START        NULL
    Stop  -->linked to PNP STOP         NULL
    Alloc -->linked to AddDevice        UsbhBusIfAddDevice
    Free  -->linked to PNP REMOVE       UsbhBusIfRemoveDevice

Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/


#include "hubdef.h"
#include "pch.h"

#ifdef HUB_WPP
#include "busif.tmh"
#endif

CCHAR usbfile_busif_c[] = "busif.c";

#define USBFILE usbfile_busif_c


#define LOCATION_INTERFACE_VERSION 1

//
// The length - in characters - of the Multi-Sz strings returned from
// the location interface.  Count one extra character for the MultiSz
// second terminator
//
#define LOCATION_INTERFACE_DEVICE_STRING_COUNT (sizeof "USB(XXX)" + 1)



NTSTATUS
UsbhGetBusInterface(
    PDEVICE_OBJECT HubFdo,
    __in_bcount(Size) PVOID BusInterface,
    CONST GUID *InterfaceType,
    USHORT Size,
    USHORT Version
    )
/*++

Routine Description:

Arguments:

Return Value:

    returns success if given interface fetched and meets the give criteria

--*/
{
    PIO_STACK_LOCATION stk;
    PIRP irp;
    NTSTATUS nts;
    KEVENT event;
    PDEVICE_EXTENSION_HUB  hubExt;
    PINTERFACE busI;

    hubExt = FdoExt(HubFdo);

    UsbhAssert(HubFdo, Size > sizeof(struct _INTERFACE));

    UsbhAllocatePool_Z(busI, PagedPool, Size);
    if (!busI) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    irp = IoAllocateIrp(hubExt->TopOfStackDeviceObject->StackSize, FALSE);

    if (!irp) {
        UsbhFreePool(busI);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // All PnP IRP's need the Status field initialized to STATUS_NOT_SUPPORTED.
    irp->IoStatus.Status = STATUS_NOT_SUPPORTED;

    KeInitializeEvent(&event, NotificationEvent, FALSE);

    UsbhSetCompletionRoutine(HubFdo,
                             irp,
                             UsbhDeferIrpCompletion,
                             &event,
                             TRUE,
                             TRUE,
                             TRUE);

    stk = IoGetNextIrpStackLocation(irp);
    UsbhAssert(HubFdo, stk);
    stk->MajorFunction= IRP_MJ_PNP;
    stk->MinorFunction= IRP_MN_QUERY_INTERFACE;

    stk->Parameters.QueryInterface.Interface = busI;
    stk->Parameters.QueryInterface.InterfaceSpecificData = NULL;
    stk->Parameters.QueryInterface.InterfaceType = InterfaceType;
    stk->Parameters.QueryInterface.Size = Size;
    stk->Parameters.QueryInterface.Version = Version;

    nts = IoCallDriver(hubExt->TopOfStackDeviceObject, irp);

    if (nts == STATUS_PENDING) {

        KeWaitForSingleObject(
            &event,
            Suspended,
            KernelMode,
            FALSE,
            NULL);

        nts = irp->IoStatus.Status;
    }

    if (NT_SUCCESS(nts)) {
        // we have a bus interface.

        // check that we have the minimum bus interface version we need to run
        if (busI->Version < Version ||
            busI->Size < Size) {

            nts = STATUS_INVALID_PARAMETER;
            // can't use it, return error
            HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_BadBusIf,
                          nts, 0, busI,
                          sizeof(struct _INTERFACE));

            //dref the interface because we won't use it
            busI->InterfaceDereference(busI->Context);

        } else {
            // use our own size value, we know that the returned data is at
            // least this big
            RtlCopyMemory(BusInterface, busI, Size);
        }

    }

    IoFreeIrp(irp);
    UsbhFreePool(busI);

    LOG(HubFdo, LOG_HUB, 'Gbsi', 0, nts);

    return nts;
}


NTSTATUS
UsbhBusIfAddDevice(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Allocs what we need for the bus interfaces and connects to it

    called in context of AddDevice

Arguments:

Irql: PASSIVE

Return Value:

    nt status

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;
    NTSTATUS    nts;
    NTSTATUS    ntstatus = 0;
    USHORT      usbbusif_size;
    USHORT      usbbusif_version;

    UsbhAssertPassive();

    hubExt = FdoExt(HubFdo);

    nts = STATUS_SUCCESS;

    // Connect to the hub bus interface, minimum version 8, try for
    // version 9 first.
    //
    usbbusif_size = sizeof(struct _USB_BUS_INTERFACE_HUB_V9);
    usbbusif_version = USB_BUSIF_HUB_VERSION_9;

    while (NT_SUCCESS(nts)) {

        LOG(HubFdo, LOG_HUB, 'Gih1', usbbusif_size, usbbusif_version);

        nts = UsbhGetBusInterface(HubFdo,
                                  &hubExt->HubBusInterface,
                                  &USB_BUS_INTERFACE_HUB_GUID,
                                  usbbusif_size,
                                  usbbusif_version);

        LOG(HubFdo, LOG_HUB, 'Gih2', nts, usbbusif_version);

        if (NT_SUCCESS(nts)) {
            hubExt->FdoFlags.HubBusIfReferenced = 1;
            break;

        } else {

            HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_NoHubBusIf, nts, 0, NULL, 0);

            // Switch to version 8 and retry.
            //
            if (usbbusif_version == USB_BUSIF_HUB_VERSION_9) {

                usbbusif_size = sizeof(struct _USB_BUS_INTERFACE_HUB_V8);
                usbbusif_version = USB_BUSIF_HUB_VERSION_8;

                nts = STATUS_SUCCESS;
            }
        }
    }

    // Connect to the USBDI bus interface, minimum version 2, try for
    // version 3 first.
    //
    usbbusif_size = sizeof(struct _USB_BUS_INTERFACE_USBDI_V3);
    usbbusif_version = USB_BUSIF_USBDI_VERSION_3;

    while (NT_SUCCESS(nts)) {

        LOG(HubFdo, LOG_HUB, 'Giu1', usbbusif_size, usbbusif_version);

        nts = UsbhGetBusInterface(HubFdo,
                                  &hubExt->UsbdBusInterface,
                                  &USB_BUS_INTERFACE_USBDI_GUID,
                                  usbbusif_size,
                                  usbbusif_version);

        LOG(HubFdo, LOG_HUB, 'Giu2', nts, usbbusif_version);

        if (NT_SUCCESS(nts)) {
            hubExt->FdoFlags.UsbdBusIfReferenced = 1;
            break;

        } else {

            HUB_EXCEPTION(USBFILE,  HubFdo, 0, HubErr_NoUsbdiBusIf, nts, 0, NULL, 0);

            // Switch to version 2 and retry.
            //
            if (usbbusif_version == USB_BUSIF_USBDI_VERSION_3) {

                usbbusif_size = sizeof(struct _USB_BUS_INTERFACE_USBDI_V2);
                usbbusif_version = USB_BUSIF_USBDI_VERSION_2;

                nts = STATUS_SUCCESS;
            }
        }
    }

    if (NT_SUCCESS(nts)) {
        // set the IsRoot flag now that we have access to the bus interface
        BOOLEAN isRoot;

        // determine if we are the root hub.
        // The only way to know for sure is to ask the bus driver,
        // we must do this now since other modules may need this info.

        nts =  Usbh_HubIsRoot(HubFdo, &isRoot);
        hubExt->FdoFlags.HubIsRoot = isRoot ? 1 : 0;

        // remember the version we fetched
        hubExt->UsbdBusifVersion = usbbusif_version;
    }

    //
    // Connect to the MINIDUMP bus interface.
    //

    usbbusif_size = sizeof(struct _USB_BUS_INTERFACE_HUB_MINIDUMP);
    usbbusif_version = USB_BUSIF_HUB_MIDUMP_VERSION_0;

    if (NT_SUCCESS(nts)) {

        LOG(HubFdo, LOG_HUB, 'Gim1', usbbusif_size, usbbusif_version);

        ntstatus = UsbhGetBusInterface(HubFdo,
                                       &hubExt->HubMiniDumpInterface,
                                       &USB_BUS_INTERFACE_HUB_MINIDUMP_GUID,
                                       usbbusif_size,
                                       usbbusif_version);

        LOG(HubFdo, LOG_HUB, 'Gim2', nts, usbbusif_version);

        if (NT_SUCCESS(ntstatus)) {
            hubExt->FdoFlags.BusIfMiniDumpReferenced = 1;
        } else {

            // We are here because of the Old UsbPort and New UsbHub.
            // Remember.. we are not doing much with the ntstatus
            // for now.  The lack of the minidump interface by itself
            // is not fatal so just ignore this failure.
        }
    }


    //
    // Connect to the hub selective suspend bus interface.
    //
    usbbusif_size = sizeof(struct _USB_BUS_INTERFACE_HUB_SELECTIVE_SUSPEND);
    usbbusif_version = USB_BUSIF_HUB_SS_VERSION_0;

    if (NT_SUCCESS(nts)) {

        LOG(HubFdo, LOG_HUB, 'Gis1', usbbusif_size, usbbusif_version);

        ntstatus = UsbhGetBusInterface(HubFdo,
                                       &hubExt->HubSSInterface,
                                       &USB_BUS_INTERFACE_HUB_SS_GUID,
                                       usbbusif_size,
                                       usbbusif_version);

        LOG(HubFdo, LOG_HUB, 'Gis2', nts, usbbusif_version);

        if (NT_SUCCESS(ntstatus)) {
            hubExt->FdoFlags.BusIfSelectiveSuspendReferenced = 1;
        } 
    }

    //
    // Connect to the Forward Progress bus interface.
    //

    usbbusif_size = sizeof(USB_BUS_INTERFACE_HUB_FORWARD_PROGRESS);
    usbbusif_version = USB_BUSIF_HUB_FORWARD_PROGRESS_VERSION_0;

    if (NT_SUCCESS(nts)) {

        LOG(HubFdo, LOG_HUB, 'Gfp1', usbbusif_size, usbbusif_version);

        nts = UsbhGetBusInterface(HubFdo,
                                  &hubExt->HubFPInterface,
                                  &USB_BUS_INTERFACE_HUB_FORWARD_PROGRESS_GUID,
                                  usbbusif_size,
                                  usbbusif_version);

        LOG(HubFdo, LOG_HUB, 'Gfp2', nts, usbbusif_version);

        if (NT_SUCCESS(nts)) {
            hubExt->FdoFlags.BusIfForwardProgressReferenced = 1;
        }
    }

    LOG(HubFdo, LOG_HUB, 'bsif', 0, nts);

    return nts;
}

NTSTATUS
UsbhBusifGetSSHub(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )
/*++

Routine Description:

Arguments:

Return Value:

    NT status code.

--*/
{
    //PVOID interfaceData = NULL;
    USHORT requestedSize, requestedVersion;
    NTSTATUS ntStatus;
    PIO_STACK_LOCATION irpStack = NULL;
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    requestedSize = irpStack->Parameters.QueryInterface.Size;
    requestedVersion = irpStack->Parameters.QueryInterface.Version;

    // assume success
    ntStatus = STATUS_SUCCESS;

    if (requestedVersion >= USB_BUSIF_HUB_SS_VERSION_0 ) {
        PUSB_BUS_INTERFACE_HUB_SELECTIVE_SUSPEND busInterface0;

        busInterface0 = (PUSB_BUS_INTERFACE_HUB_SELECTIVE_SUSPEND)
            irpStack->Parameters.QueryInterface.Interface;

        busInterface0->BusContext = hubExt;
        busInterface0->InterfaceReference = UsbhBusIfSSReference;
        busInterface0->InterfaceDereference = UsbhBusIfSSDereference;
        busInterface0->SuspendHub = UsbhBusIf_SuspendChildHub;
        busInterface0->ResumeHub = UsbhBusIf_ResumeChildHub;

        busInterface0->Size = sizeof(USB_BUS_INTERFACE_HUB_SELECTIVE_SUSPEND);
        busInterface0->Version = USB_BUSIF_HUB_SS_VERSION_0;
    }

    if (NT_SUCCESS(ntStatus)) {
        UsbhBusIfSSReference(hubExt);
    }

    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;
    
    return ntStatus;
}

VOID
UsbhBusIfSSReference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine increments outstanding reference count for this interface

Arguments:

    Context - Hub device extension

Return Value:

    None

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = Context;

    InterlockedIncrement((PLONG)&hubExt->SSH_InterfaceCount);
}


VOID
UsbhBusIfSSDereference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine decrements outstanding reference count for this interface

Arguments:

    Context - Hub device extension

Return Value:

    None

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = Context;

    UsbhAssert(NULL, hubExt->SSH_InterfaceCount != 0);
    InterlockedDecrement((PLONG)&hubExt->SSH_InterfaceCount);
}

NTSTATUS
UsbhBusIf_SuspendChildHub(
    PDEVICE_OBJECT Pdo
    )
{
    PDEVICE_OBJECT HubFdo, latch_pdo;
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;

    pdoExt = PdoExt(Pdo);
    ASSERT_PDOEXT(pdoExt);
    
    HubFdo = pdoExt->HubFdo;
    
    hubExt = FdoExt(HubFdo);
    ASSERT_FDOEXT(hubExt);

    if (!pdoExt->PdoFlags.DeviceIsHub) {
        return STATUS_UNSUCCESSFUL;
    }

    latch_pdo = UsbhLatchPdo(HubFdo, pdoExt->PortNumber, NULL, PdoBusIfSuspendHub_Tag);

    if (!latch_pdo) {
        return STATUS_DEVICE_NOT_READY;
    }
    
    nts = UsbhSyncSuspendPdoPort(PDO_CONTEXT(Pdo), Pdo, FALSE);

    if (NT_SUCCESS(nts)) {
        //
        // Even though we are not using D IRPs for hub selective
        // suspend, we are changing the power state of the PDO.
        // This enables a power user to look at the power state 
        // of the hub from device manager and determine if it is
        // suspended
        //
        UsbhSet_Pdo_Dx(Pdo,PowerDeviceD2);
        UsbhSetPdoIdleReady(HubFdo, Pdo, SSH_API_CONTEXT(HubFdo));
    }
    
    UsbhUnlatchPdo(HubFdo, latch_pdo, NULL, PdoBusIfSuspendHub_Tag);
    
    return nts;
}

NTSTATUS
UsbhBusIf_ResumeChildHub(
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    This routine will will resume a child hub PDO that is selectively suspended.

Arguments:

    Pdo - PDO for child hub

Return Value:

    NT status code
++*/
{
    PDEVICE_OBJECT HubFdo, latch_pdo;
    PDEVICE_EXTENSION_PDO pdoExt;
    PDEVICE_EXTENSION_HUB hubExt;
    NTSTATUS nts;
    PSSH_BUSY_HANDLE bh;
    PSTATE_CONTEXT sc;

    UsbhAssertPassive();
    
    pdoExt = PdoExt(Pdo);
    ASSERT_PDOEXT(pdoExt);
    
    HubFdo = pdoExt->HubFdo;
    
    hubExt = FdoExt(HubFdo);
    ASSERT_FDOEXT(hubExt);

    // Make sure the child PDO is a hub
    if (!pdoExt->PdoFlags.DeviceIsHub) {
        return STATUS_UNSUCCESSFUL;
    }

    sc = SSH_API_CONTEXT(HubFdo);

    // Prevent the parent hub from suspending if it is currently active
    bh = UsbhIncHubBusy(HubFdo, sc, HubFdo, UsbhSshResumeChildHub_Tag, NO_RESUME_HUB);
    
    // Check if the parent hub is currently active
    nts = Usbh_SSH_Event(HubFdo, SSH_Event_GetPortStatus, sc);
    
    if (NT_ERROR(nts)) {
        // Hub is not active, so dispatch resume event.
        Usbh_SSH_Event(HubFdo, SSH_Event_ResumeHub, SSH_API_CONTEXT(HubFdo));
    }

    if (NT_SUCCESS(nts) || (hubExt->SSH_FailedToSuspend)) {
        // Parent hub is already active. Propagate resume down to the child hub
        latch_pdo = UsbhLatchPdo(HubFdo, pdoExt->PortNumber, NULL, PdoBusIfSuspendHub_Tag);

        if (!latch_pdo) {
            UsbhDecHubBusy(HubFdo, sc, bh);
            return STATUS_DEVICE_NOT_READY;
        }
    
        // Resume the hub
        nts = UsbhSyncResumePort(HubFdo, SSH_API_CONTEXT(HubFdo), pdoExt->PortNumber);

        if (NT_SUCCESS(nts)) {
            UsbhSshSetPortsBusyState(HubFdo, pdoExt->PortNumber, PORT_BUSY, RESUME_HUB);
        }

        // Propagate the resume downstream of the port
        nts = UsbhSshPropagateResume(HubFdo, SSH_API_CONTEXT(HubFdo), latch_pdo);

        UsbhUnlatchPdo(HubFdo, latch_pdo, NULL, PdoBusIfResumeHub_Tag);
    }

    UsbhClearPdoIdleReady(HubFdo, Pdo, SSH_API_CONTEXT(HubFdo));

    UsbhDecHubBusy(HubFdo, sc, bh);

    return nts;
}

VOID
UsbhBusIfRemoveDevice(
    PDEVICE_OBJECT HubFdo,
    PSTATE_CONTEXT StateContext
    )
/*++

Routine Description:

    Frees bus interfaces resources

    called in context of IRP_MN_REMOVE, undoes alloc

Arguments:

Irql: PASSIVE

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB  hubExt;

    hubExt = FdoExt(HubFdo);

    if (hubExt->FdoFlags.HubBusIfReferenced) {
         Usbh_HubInterfaceDereference(HubFdo);
    }

    if (hubExt->FdoFlags.UsbdBusIfReferenced) {
         Usbh_UsbdInterfaceDereference(HubFdo, hubExt->UsbdBusInterface.BusContext);
    }

    if (hubExt->FdoFlags.BusIfMiniDumpReferenced) {
         Usbh_MiniDumpInterfaceDereference(HubFdo);
    }

    if (hubExt->FdoFlags.BusIfSelectiveSuspendReferenced) {
         Usbh_SsInterfaceDereference(HubFdo);
    }

    if (hubExt->FdoFlags.BusIfForwardProgressReferenced) {
         Usbh_ForwardProgressInterfaceDereference(HubFdo);
    }

    hubExt->FdoFlags.UsbdBusIfReferenced =
        hubExt->FdoFlags.HubBusIfReferenced = 0;

    hubExt->FdoFlags.BusIfMiniDumpReferenced = 0;

    hubExt->FdoFlags.BusIfSelectiveSuspendReferenced = 0;
    
    hubExt->FdoFlags.BusIfForwardProgressReferenced = 0;
}


NTSTATUS
UsbhQuerySSstate(
    PDEVICE_OBJECT HubFdo,
    PBOOLEAN SelectiveSuspendEnabled
    )
/*++

Routine Description:

    Queries the global selective suspend state for the host controller.  If disabled, it will
    disable both hub selective suspend and device selective suspend (idle notification IRPs).

Arguments:

Irql: PASSIVE

Return Value:

    nt status, SelectiveSuspendEnabled unchanged if error

    state of SelectiveSuspend.

--*/
{
    USB_CONTROLLER_INFORMATION_0 controllerInfo;
    ULONG dataLen = 0;
    NTSTATUS nts;

    controllerInfo.InformationLevel = 0;

    nts = Usbh_HubGetControllerInformation(HubFdo,
                                           &controllerInfo,
                                           sizeof(struct _USB_CONTROLLER_INFORMATION_0),
                                           &dataLen);
    UsbhAssert(HubFdo, dataLen);

    if (NT_SUCCESS(nts)) {
        if (dataLen && dataLen >= sizeof(struct _USB_CONTROLLER_INFORMATION_0)) {
            *SelectiveSuspendEnabled = controllerInfo.SelectiveSuspendEnabled;
        } else {
            nts = STATUS_INVALID_PARAMETER;
        }
    }
    return nts;
}

NTSTATUS
UsbhBusIf_GetContainerIdForPort(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    LPGUID ContainerId
    )
/*++

Routine Description:

    Queries the host controller miniport driver for a Container ID 
    for a device attached to the specified port.

Arguments:

    HubFdo - FDO for the root hub 
    
    PortNumber - Port number the device is attached to
    
    ContainerId - Pointer to the GUID that will contain the Container ID on return.

Irql: PASSIVE

Return Value:

    nt status, STATUS_SUCCESS if ContainerId contains a valid Conainter ID.

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_HUB_BUS_INTERFACE bus;
    NTSTATUS nts;

    hubExt = FdoExt(HubFdo);
    ASSERT_FDOEXT(hubExt);

    bus = &hubExt->HubBusInterface;

    LOG(HubFdo, LOG_BUS, 'uGCI', bus, PortNumber);

    if (!hubExt->FdoFlags.HubIsRoot) {
        UsbhAssertMsg(HubFdo, "GetContainerIdForPort only valid for RootHub", FALSE);
        nts = STATUS_INVALID_PARAMETER;
    } else if (!bus->GetContainerIdForPort) {
        UsbhAssertMsg(HubFdo, "GetContainerIdForPort not implemented", FALSE);
        nts =  STATUS_NOT_IMPLEMENTED;
    } else {
        nts = bus->GetContainerIdForPort(
                    bus->BusContext,
                    PortNumber,
                    ContainerId
                    );
    }
    
    return nts;
}

VOID
UsbhBusIf_SetContainerIdForPort(
    PDEVICE_OBJECT HubFdo,
    USHORT PortNumber,
    LPGUID ContainerId
    )
/*++

Routine Description:

    Informs the host controller miniport driver what the Container ID is
    for a device attached to the specified port.

Arguments:

    HubFdo - FDO for the root hub 
    
    PortNumber - Port number the device is attached to
    
    ContainerId - Pointer to the GUID that contains the Container ID.

Irql: PASSIVE

Return Value:

    none

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_HUB_BUS_INTERFACE bus;

    hubExt = FdoExt(HubFdo);
    ASSERT_FDOEXT(hubExt);

    bus = &hubExt->HubBusInterface;

    LOG(HubFdo, LOG_BUS, 'uSCI', bus, PortNumber);

    if ((!hubExt->FdoFlags.HubIsRoot) || (!bus->SetContainerIdForPort)) {
        UsbhAssertMsg(HubFdo, "SetContainerIdForPort operation not supported", FALSE);
        return;
    }
    
    bus->SetContainerIdForPort(
            bus->BusContext,
            PortNumber,
            ContainerId
            );
}



NTSTATUS
UsbhBusifGetLocationInterface(
    PDEVICE_OBJECT  HubFdo,
    PDEVICE_OBJECT  Pdo,
    PIRP            Irp
    )
/*++

Routine Description:

    This routine constructs a PNP_LOCATION_INTERFACE.

Arguments:

Return Value:

    Returns NTSTATUS.

--*/
{
    PDEVICE_EXTENSION_PDO   pdoExt;
    PIO_STACK_LOCATION      irpStack;
    USHORT                  requestedSize;
    PPNP_LOCATION_INTERFACE interface = NULL;
    NTSTATUS                ntStatus;

    pdoExt = PdoExt(Pdo);

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    requestedSize = irpStack->Parameters.QueryInterface.Size;

    if (requestedSize >= sizeof(PNP_LOCATION_INTERFACE)) {

        interface = (PPNP_LOCATION_INTERFACE)
            irpStack->Parameters.QueryInterface.Interface;

        interface->Size = sizeof(PNP_LOCATION_INTERFACE);
        interface->Version = LOCATION_INTERFACE_VERSION;
        interface->Context = pdoExt;
        interface->InterfaceReference = UsbhBusIfLocationReference;
        interface->InterfaceDereference = UsbhBusIfLocationDereference;
        interface->GetLocationString = UsbhBusIfLocationGetLocationStrings;

        ntStatus = STATUS_SUCCESS;

    } else {

        ntStatus = STATUS_UNSUCCESSFUL;
    }

    if (NT_SUCCESS(ntStatus)) {
        interface->InterfaceReference(interface->Context);
    }

    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;
    
    return ntStatus;
}

VOID
UsbhBusIfLocationReference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine increments outstanding reference count for this interface

Arguments:

    Context - Hub PDO device extension

Return Value:

    None

--*/
{
    PDEVICE_EXTENSION_PDO  pdoExt;
    PDEVICE_EXTENSION_HUB  hubExt;
    
    pdoExt = Context;
    ASSERT_PDOEXT(pdoExt);

    hubExt = FdoExt(pdoExt->HubFdo);
    ASSERT_FDOEXT(hubExt);

    InterlockedIncrement((PLONG)&hubExt->LocationInterfaceCount);
}


VOID
UsbhBusIfLocationDereference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine decrements outstanding reference count for this interface

Arguments:

    Context - Hub PDO device extension

Return Value:

    None

--*/
{
    PDEVICE_EXTENSION_PDO  pdoExt;
    PDEVICE_EXTENSION_HUB  hubExt;
    
    pdoExt = Context;
    ASSERT_PDOEXT(pdoExt);

    hubExt = FdoExt(pdoExt->HubFdo);
    ASSERT_FDOEXT(hubExt);

    UsbhAssert(NULL, hubExt->LocationInterfaceCount != 0);
    InterlockedDecrement((PLONG)&hubExt->LocationInterfaceCount);
}

NTSTATUS
UsbhBusIfLocationGetLocationStrings(
    __inout PVOID Context,
    __out PWCHAR *LocationStrings
    )
/*++

Routine Description:

    This routine allocates, fills in, and returns a Multi-Sz string
    containing the bus-relative location identifier string for the
    given device.
    
    The location string for a USB device is "USB(X)" where X is the USB
    hub port number to which the device is immediately connected.

    This interface is permitted to return a Multi-Sz containing
    multiple strings describing the same device, but in this
    first implementation, only the single strings listed above
    will be returned from the interface.  The string must still
    be in the format of a Multi-Sz, however, meaning a double-NULL
    terminator is required.

Arguments:

    Context - extension pointer.

Return Value:

    NTSTATUS code.

--*/
{
    PDEVICE_EXTENSION_PDO   pdoExt;
    size_t                  remainingChars;
    PWCHAR                  stringBuffer;
    NTSTATUS                status;
    
    *LocationStrings = NULL;

    pdoExt = Context;

    UsbhAllocatePool_Z(stringBuffer,
                       PagedPool,
                       LOCATION_INTERFACE_DEVICE_STRING_COUNT*sizeof(WCHAR));

    if (!stringBuffer) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
        
    // The location string for a USB device is "USB(X)" where X is the
    // USB hub port number to which the device is immediately connected.
    //
    // We use the STRSAFE_FILL_BEHIND_NULL flag to ensure the unused
    // portion of the buffer is filled with 0s which null terminates the
    // multsz
    //
    status = RtlStringCchPrintfExW(stringBuffer, 
                                   LOCATION_INTERFACE_DEVICE_STRING_COUNT,
                                   NULL,
                                   &remainingChars,
                                   STRSAFE_FILL_BEHIND_NULL,
                                   L"USB(%d)",
                                   pdoExt->PortNumber);

    UsbhAssert(NULL, NT_SUCCESS(status));

    //
    // Make sure there was room for the multisz termination NUL 
    // N.B. remainingChars counts the NUL termination of the regular string
    // as being available so we need to ensure 2 chars are left for the 2 NULS
    //
    UsbhAssert(NULL, remainingChars >= 2);

    *LocationStrings = stringBuffer;

    return STATUS_SUCCESS;    
}


VOID
UsbhPdoAbortAllDevicePipes(
    PDEVICE_OBJECT HubFdo,
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    This routine is used to synchronously abort any outstanding transfer
    requests on all pipe handles of a PDO device.

Arguments:

    Pdo - a latched Pdo

Return Value:

    none
--*/
{
    PUSB_DEVICE_HANDLE  dh;

    dh = UsbhRefPdoDeviceHandle(HubFdo, Pdo, Pdo, Devh_AbortAllDevicePipes_Tag);

    if (dh) {

        PDEVICE_EXTENSION_HUB   hubExt;
        PUSB_HUB_BUS_INTERFACE  bus;
    
        hubExt = FdoExt(HubFdo);

        bus = &(hubExt->HubBusInterface);

        if (bus->AbortAllDevicePipes) {

            bus->AbortAllDevicePipes(bus->BusContext,
                                     dh);

        }

        UsbhDerefPdoDeviceHandle(HubFdo, dh, Pdo, Devh_AbortAllDevicePipes_Tag);
    }
}


USHORT
UsbhBusIf_GetDebugPortNumber(
    PDEVICE_OBJECT HubFdo
    )
/*++

Routine Description:

    Queries the host controller miniport driver for the debug port number, if any.

Arguments:

    HubFdo - FDO for the root hub 
    
Irql: PASSIVE

Return Value:

    The debug port number, 0 if it does not exist.

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_HUB_BUS_INTERFACE bus;

    hubExt = FdoExt(HubFdo);
    ASSERT_FDOEXT(hubExt);

    bus = &hubExt->HubBusInterface;

    LOG(HubFdo, LOG_BUS, 'uGDP', bus, 0);

    if (!hubExt->FdoFlags.HubIsRoot) {
        return 0;
    } else if (!bus->GetDebugPortNumber) {
        UsbhAssertMsg(HubFdo, "GetDebugPortNumber not implemented", FALSE);
        return 0;
    } else {
        return bus->GetDebugPortNumber(bus->BusContext);
    }
}


VOID
UsbhBusIf_SetMsOs20Support(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE DeviceHandle,
    PUSB_DEVICE_DESCRIPTOR DeviceDescriptor,
    UCHAR MsOs20VendorCode
    )
/*++

Routine Description:

    Notify USBPORT that this device supports MS OS 2.0 Descriptors

Arguments:

    HubFdo - Parent hub for the device 

    DeviceHandle - Device handle to update

    DeviceDescriptor - USB device descriptor to update the device handle with

    MsOs20VendorCode - Vendor code used to retrieve the MS OS 2.0 Descriptor set
    
Irql: PASSIVE

Return Value:

    None

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_HUB_BUS_INTERFACE bus;

    hubExt = FdoExt(HubFdo);
    ASSERT_FDOEXT(hubExt);

    bus = &hubExt->HubBusInterface;

    LOG(HubFdo, LOG_BUS, 'uSDD', bus, 0);

    if (!bus->SetMsOs20Support) {
        UsbhAssertMsg(HubFdo, "SeteMsOs20Support not implemented", FALSE);
        return;
    } else {
        bus->SetMsOs20Support(bus->BusContext, DeviceHandle, DeviceDescriptor, MsOs20VendorCode);
    }
}


VOID
UsbhBusIf_SetClearRootPortResumeTime(
    PDEVICE_OBJECT HubFdo,
    PUSB_DEVICE_HANDLE DeviceHandle,
    BOOLEAN SetTime,
    UCHAR ResumeTime
    )
/*++

Routine Description:

    Sets or clears the root port's resume time

Arguments:

    HubFdo - Parent hub for the device 

    DeviceHandle - Device handle to update

    SetTime - TRUE if a new resume time should be set, FALSE if it should revert to the default

    ResumeTime - Resume time to be set if SetTime is TRUE
    
Irql: PASSIVE

Return Value:

    None

--*/
{
    PDEVICE_EXTENSION_HUB hubExt;
    PUSB_HUB_BUS_INTERFACE bus;

    hubExt = FdoExt(HubFdo);
    ASSERT_FDOEXT(hubExt);

    bus = &hubExt->HubBusInterface;

    LOG(HubFdo, LOG_BUS, 'uSCR', bus, 0);

    if (!bus->SetClearRootPortResumeTime) {
        UsbhAssertMsg(HubFdo, "SetClearRootPortResumeTime not implemented", FALSE);
        return;
    } else {
        bus->SetClearRootPortResumeTime(bus->BusContext, DeviceHandle, SetTime, ResumeTime);
    }
}
