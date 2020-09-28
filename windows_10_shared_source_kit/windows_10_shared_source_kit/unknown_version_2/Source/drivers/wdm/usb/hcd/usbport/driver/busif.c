/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    busif.c

Abstract:

    bus interface for the usbport driver

    this is where we export the routines that create
    and remove devices on the bus for use by the hub
    driver.

Environment:

    kernel mode only

Notes:

    Margin=120

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs
    08-23-04 : revised bus_context and device handle references

--*/

// Need to include ntddk.h for PNP_LOCATION_INTERFACE
//
#include "ntddk.h"
#include "common.h"

#ifdef USBPORT_WPP
#include "busif.tmh"
#endif

#define LOCATION_INTERFACE_VERSION 1

//
// The length - in characters - of the Multi-Sz strings returned from
// the location interface.  Count one extra character for the MultiSz
// second terminator
//
#define LOCATION_INTERFACE_DEVICE_STRING_COUNT (sizeof "USBROOT(0)" + 1)

NTSTATUS
USBPORT_GetBusContext(
    PBUS_CONTEXT *BusContext,
    PDEVICE_OBJECT PdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle
    )
/*++

Routine Description:

    Returns the bus context matching the given handle passed in. Bus contexts
    may be referenced multiple times for the same device handle.

    If a context is not found a new one is created for the device.

Arguments:

Return Value:

--*/
{
    KIRQL oldIrql;
    NTSTATUS ntStatus = STATUS_BOGUS;
    PLIST_ENTRY busEntry = NULL;
    PBUS_CONTEXT busCon = NULL;
    PDEVICE_EXTENSION rhDevExt = NULL, devExt = NULL;

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    GET_DEVICE_EXT(devExt, rhDevExt->HcFdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->Fdo.BusContextSpin, &oldIrql);

    ASSERT_DEVICE_HANDLE(DeviceHandle);

    for(busEntry = devExt->Fdo.BusContextHead.Flink;
        busEntry != &devExt->Fdo.BusContextHead;
        busEntry = busEntry->Flink) {
        busCon = CONTAINING_RECORD(busEntry,
                                   BUS_CONTEXT,
                                   NextEntry);
        ASSERT_BUS_CONTEXT(busCon);
        if(DeviceHandle == busCon->DeviceHandle) {
            break;
        }
        busCon = NULL;
    }
    KeReleaseSpinLock(&devExt->Fdo.BusContextSpin, oldIrql);

    if(NULL != busCon) {
        ntStatus = STATUS_SUCCESS;
        goto Exit;
    }

    // allocate a new bus context structure
    ALLOC_POOL_Z(busCon, NonPagedPoolNx, sizeof(BUS_CONTEXT));
    if(NULL == busCon) {
        ntStatus = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }
    busCon->BusCxtSig = SIG_USBPORT_BUS_CONTEXT;

    //jd
    // these device objects should be referenced as well

    busCon->RhPdo = PdoDeviceObject;
    busCon->HcFdo = rhDevExt->HcFdoDeviceObject;

    //jd
    // take a remlock here
    busCon->BcState = Bc_Invalid;

    // reference the device handle
    ntStatus = USBPORT_iReferenceDeviceHandle(busCon->HcFdo,
                                              DeviceHandle,
                                              busCon,
                                              DEVH_BUS_CONTEXT_TAG); //
    if (NT_ERROR(ntStatus)) {
        UsbFreePool(busCon);
        busCon = NULL;
        goto Exit;
    }

    busCon->DeviceHandle = DeviceHandle;

    USBPORT_BusContextEvent(busCon, EvBc_Create);

    ntStatus = STATUS_SUCCESS;

Exit:

    *BusContext = busCon;
    return ntStatus;
}


PDEVICE_OBJECT
USBPORT_PdoFromBusContext(
    PVOID BusContext
    )
/*++

Routine Description:

Arguments:

Return Value:

    NT status code.

--*/
{
    ASSERT_BUS_CONTEXT(BusContext);
    return (((PBUS_CONTEXT)BusContext)->RhPdo);
}


NTSTATUS
USBPORT_TtFromBusContext(
    PDEVICE_OBJECT HcFdo,
    PBUS_CONTEXT BusContext,
    PTRANSACTION_TRANSLATOR *ReturnTt
    )
/*++

Routine Description:

    returns the tt linked to a bus context

Arguments:

    BusContext - handle to the device we export thru QUERY_BUS_INTERFACE
            it references the device handle

    ReturnTt - pointer filled in with the tt for the device.
            if the device does not have a TT success is returned and this field is NULL.

Return Value:

    NT status code.

--*/
{
    PUSBD_DEVICE_HANDLE devH;
    KIRQL irql;
    PTRANSACTION_TRANSLATOR tt = NULL;
    NTSTATUS nts = STATUS_SUCCESS;
    PDEVICE_EXTENSION devExt = NULL;

    ASSERT_BUS_CONTEXT(BusContext);

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);


    KeAcquireSpinLock(&devExt->Fdo.BusContextSpin, &irql);

    devH = BusContext->DeviceHandle;

    if (devH) {
        ASSERT_DEVICE_HANDLE(devH);

        tt = devH->TtHandle;

        if (tt) {
            nts = USBPORT_ReferenceTt(HcFdo,
                                       tt,
                                       BusContext,
                                       TT_BUSCONTEXT_TAG); //bsTT
            if (NT_ERROR(nts)) {
                tt = NULL;
            }

        }
    }
    KeReleaseSpinLock(&devExt->Fdo.BusContextSpin, irql);

    *ReturnTt = tt;

    return nts;

}


VOID
USBPORT_BusContextReleaseTt(
    PDEVICE_OBJECT HcFdo,
    PBUS_CONTEXT BusContext,
    PTRANSACTION_TRANSLATOR Tt
    )
/*++

Routine Description:

    Releases a tt ref'ed by USBPORT_TtFromBusContext

Arguments:

    BusContext - handle to the device we export thru QUERY_BUS_INTERFACE
            it references the device handle

    Tt - tt to release

Return Value:

    NT status code.

--*/
{
    KIRQL irql;
    PDEVICE_EXTENSION devExt = NULL;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    ASSERT_BUS_CONTEXT(BusContext);

    KeAcquireSpinLock(&devExt->Fdo.BusContextSpin, &irql);

    USBPORT_DereferenceTt(HcFdo,
                          Tt,
                          BusContext,
                          TT_BUSCONTEXT_TAG); //bsTT

    KeReleaseSpinLock(&devExt->Fdo.BusContextSpin, irql);
}


VOID
USBPORT_MiniDumpInterfaceReference(
    PVOID BusContext
    )
/*++

Routine Description:

    Interface function exported thru IRP_MN_QUERY_INTERFACE for
    USB_BUS_INTERFACE_HUB_MINIDUMP_GUID.

Arguments:

Return Value:

--*/
{
    PBUS_CONTEXT bc = (PBUS_CONTEXT)BusContext;

    USBPORT_BusContextEvent(bc, EvBc_UsbMinid_Ref);

    return;
}


VOID
USBPORT_MiniDumpInterfaceDereference(
    PVOID BusContext
    )
/*++

Routine Description:

    Interface function exported thru IRP_MN_QUERY_INTERFACE for
    USB_BUS_INTERFACE_HUB_MINIDUMP_GUID.

Arguments:

Return Value:

--*/
{
    PBUS_CONTEXT bc = (PBUS_CONTEXT)BusContext;

    USBPORT_BusContextEvent(bc, EvBc_UsbMinid_Deref);

    return;
}


VOID
USBPORT_ForwardProgressInterfaceReference(
    PVOID BusContext
    )
/*++

Routine Description:

    Interface function exported thru IRP_MN_QUERY_INTERFACE for
    USB_BUS_INTERFACE_HUB_FORWARD_PROGRESS_GUID.

Arguments:

Return Value:

--*/
{
    PBUS_CONTEXT bc = (PBUS_CONTEXT)BusContext;

    USBPORT_BusContextEvent(bc, EvBc_UsbFp_Ref);

    return;
}


VOID
USBPORT_ForwardProgressInterfaceDereference(
    PVOID BusContext
    )
/*++

Routine Description:

    Interface function exported thru IRP_MN_QUERY_INTERFACE for
    USB_BUS_INTERFACE_HUB_FORWARD_PROGRESS_GUID.

Arguments:

Return Value:

--*/
{
    PBUS_CONTEXT bc = (PBUS_CONTEXT)BusContext;

    USBPORT_BusContextEvent(bc, EvBc_UsbFp_Deref);

    return;
}


VOID
USBPORT_HubSelectiveSuspendInterfaceReference(
    PVOID BusContext
    )
/*++

Routine Description:

    Interface function exported thru IRP_MN_QUERY_INTERFACE for
    USB_BUS_INTERFACE_HUB_SS_GUID.

Arguments:

Return Value:

--*/
{
    PBUS_CONTEXT bc = (PBUS_CONTEXT)BusContext;

    USBPORT_BusContextEvent(bc, EvBc_UsbHubSS_Ref);

    return;
}


VOID
USBPORT_HubSelectiveSuspendInterfaceDereference(
    PVOID BusContext
    )
/*++

Routine Description:

    Interface function exported thru IRP_MN_QUERY_INTERFACE for
    USB_BUS_INTERFACE_HUB_SS_GUID.

Arguments:

Return Value:

--*/
{
    PBUS_CONTEXT bc = (PBUS_CONTEXT)BusContext;

    USBPORT_BusContextEvent(bc, EvBc_UsbHubSS_Deref);

    return;
}


VOID
USBPORT_HubBusInterfaceReference(
    PVOID BusContext
    )
/*++

Routine Description:

    Interface function exported thru pnp

Arguments:

Return Value:

--*/
{
    PBUS_CONTEXT bc = (PBUS_CONTEXT)BusContext;

    USBPORT_BusContextEvent(bc, EvBc_Usbh_Ref);

    return;
}


VOID
USBPORT_HubBusInterfaceDereference(
    PVOID BusContext
    )
/*++

Routine Description:

    Interface function exported thru pnp

Arguments:

Return Value:

--*/
{
    PBUS_CONTEXT bc = (PBUS_CONTEXT)BusContext;

    USBPORT_BusContextEvent(bc, EvBc_Usbh_Deref);

    return;
}


VOID
USBPORTBUSIF_UsbdBusInterfaceReference(
    __inout PVOID BusContext
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PBUS_CONTEXT bc = (PBUS_CONTEXT)BusContext;

    USBPORT_BusContextEvent(bc, EvBc_Usbd_Ref);

    return;
}


VOID
USBPORTBUSIF_UsbdBusInterfaceDereference(
    __inout PVOID BusContext
    )
/*++

Routine Description:

Arguments:

Return Value:

--*/
{
    PBUS_CONTEXT bc = (PBUS_CONTEXT)BusContext;

    USBPORT_BusContextEvent(bc, EvBc_Usbd_Deref);

    return;
}


NTSTATUS
USBPORT_GetBusInterface(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

Arguments:

Return Value:

    NT status code.

--*/
{
    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus;
    PVOID interfaceData;

    irpStack = IoGetCurrentIrpStackLocation(Irp);

    interfaceData = irpStack->Parameters.QueryInterface.InterfaceSpecificData;

    // assume success
    ntStatus = STATUS_SUCCESS;

    // validate version, size and GUID
    if (RtlCompareMemory(irpStack->Parameters.QueryInterface.InterfaceType,
                         &USB_BUS_INTERFACE_HUB_GUID,
                         sizeof(GUID)) == sizeof(GUID)) {

        ntStatus = USBPORT_GetBusInterfaceHub(FdoDeviceObject,
                                              PdoDeviceObject,
                                              Irp);
    } else if (RtlCompareMemory(irpStack->Parameters.QueryInterface.InterfaceType,
               &USB_BUS_INTERFACE_USBDI_GUID,
               sizeof(GUID)) == sizeof(GUID)) {

        ntStatus = USBPORT_GetBusInterfaceUSBDI(FdoDeviceObject,
                                                PdoDeviceObject,
                                                interfaceData,
                                                Irp);
    } else if (RtlCompareMemory(irpStack->Parameters.QueryInterface.InterfaceType,
               &USB_BUS_INTERFACE_HUB_MINIDUMP_GUID,
               sizeof(GUID)) == sizeof(GUID)) {

        ntStatus = USBPORT_GetBusMiniDumpInterface(FdoDeviceObject,
                                                   PdoDeviceObject,
                                                   Irp);
    } else if (RtlCompareMemory(irpStack->Parameters.QueryInterface.InterfaceType,
               &USB_BUS_INTERFACE_HUB_SS_GUID,
               sizeof(GUID)) == sizeof(GUID)) {

        ntStatus = USBPORT_GetBusInterfaceHubSS(FdoDeviceObject,
                                                PdoDeviceObject,
                                                Irp);

    } else if (RtlCompareMemory(irpStack->Parameters.QueryInterface.InterfaceType,
               &GUID_PNP_LOCATION_INTERFACE,
               sizeof(GUID)) == sizeof(GUID)) {

        ntStatus = USBPORT_BusifGetLocationInterface(FdoDeviceObject,
                                                     PdoDeviceObject,
                                                     Irp);

    } else if (RtlCompareMemory(irpStack->Parameters.QueryInterface.InterfaceType,
               &USB_BUS_INTERFACE_HUB_FORWARD_PROGRESS_GUID,
               sizeof(GUID)) == sizeof(GUID)) {

        ntStatus = USBPORT_GetBusForwardProgressInterface(FdoDeviceObject,
                                                          PdoDeviceObject,
                                                          Irp);
    } else {

        //
        // Don't change the status of an IRP that isn't understand.
        //
        ntStatus = Irp->IoStatus.Status;
    }

    return ntStatus;
}


NTSTATUS
USBPORT_GetBusInterfaceHubSS(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    IRP_MN_QUERY_INTERFACE handler for USB_BUS_INTERFACE_HUB_SS_GUID.

Arguments:

Return Value:

    NT status code.

--*/
{
    USHORT requestedVersion;
    NTSTATUS ntStatus;
    PBUS_CONTEXT busContext = NULL;
    PDEVICE_EXTENSION rhDevExt = NULL;
    PIO_STACK_LOCATION irpStack = NULL;
    PUSBD_DEVICE_HANDLE deviceHandle = NULL;

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    requestedVersion = irpStack->Parameters.QueryInterface.Version;
    deviceHandle = irpStack->Parameters.QueryInterface.InterfaceSpecificData;

    if (NULL == deviceHandle) {
        deviceHandle = rhDevExt->Pdo.RootHubDeviceHandle;
    }

    ASSERT_DEVICE_HANDLE(deviceHandle);

    ntStatus = USBPORT_GetBusContext(&busContext, PdoDeviceObject, deviceHandle);
    if (!NT_SUCCESS(ntStatus)) {
        return ntStatus;
    }

    ASSERT_BUS_CONTEXT(busContext);

    // assume success
    ntStatus = STATUS_SUCCESS;

    if (requestedVersion >= USB_BUSIF_HUB_SS_VERSION_0) {

        PUSB_BUS_INTERFACE_HUB_SELECTIVE_SUSPEND HubSSBusInterface;

        HubSSBusInterface = (PUSB_BUS_INTERFACE_HUB_SELECTIVE_SUSPEND)
            irpStack->Parameters.QueryInterface.Interface;

        HubSSBusInterface->BusContext                =   busContext;
        HubSSBusInterface->InterfaceReference        =   USBPORT_HubSelectiveSuspendInterfaceReference;
        HubSSBusInterface->InterfaceDereference      =   USBPORT_HubSelectiveSuspendInterfaceDereference;
        HubSSBusInterface->SuspendHub                =   USBPORTBUSIF_SuspendRootHub;
        HubSSBusInterface->ResumeHub                 =   USBPORTBUSIF_ResumeRootHub;
        HubSSBusInterface->Size                      =   sizeof(USB_BUS_INTERFACE_HUB_SELECTIVE_SUSPEND);
        HubSSBusInterface->Version                   =   USB_BUSIF_HUB_SS_VERSION_0;
    }

    if (NT_SUCCESS(ntStatus)) {
        USBPORT_HubSelectiveSuspendInterfaceReference(busContext);
    }

    return ntStatus;
}


NTSTATUS
USBPORTBUSIF_SuspendRootHub(
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Interface function exported thru IRP_MN_QUERY_INTERFACE for
    USB_BUS_INTERFACE_HUB_SELECTIVE_SUSPEND_GUID.

    Called by the USBHUB driver when it is going to suspend the root hub.

Arguments:

Return Value:

--*/
{
    PDEVICE_EXTENSION rhDevExt, devExt;
    NTSTATUS nts;
    PUSBD_DEVICE_HANDLE rhDevHandle;
    KIRQL irql;
    POWER_STATE powerState;

    GET_DEVICE_EXT(rhDevExt, Pdo);
    ASSERT_PDOEXT(rhDevExt);
    GET_DEVICE_EXT(devExt, rhDevExt->HcFdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // Don't allow suspend if diagnostics mode is on or there are
	// orphaned devices
    if ((devExt->Fdo.Flags.HctDiagMode) ||
        (devExt->Fdo.OrphanedDeviceCount)) {
        nts = STATUS_UNSUCCESSFUL;
    } else {
        LOGENTRY(devExt->HcFdoDeviceObject, LOG_TRANSFER, 'SuRH', Pdo, 0, 0);

        KeWaitForSingleObject(&rhDevExt->Pdo.SSLock,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);

        rhDevExt->Pdo.SelectiveSuspend = TRUE;

        //
        // Note that this is a change in behavior for Win8.  In selective suspend, we used
        // to not fail or cancel transfers, however, starting in Win8, we are enabling powering
        // the controller off.  In this case, hardware registers are inaccessible, so we need
        // to ensure that there is no pending IO or new IO attempted.
        //

        // There should be no transfers on the HW at time of suspend.
        // ** FailUrbs will cause URB apis to be failed in the Dispatch routine
        USBPORT_ACQUIRE_DM_LOCK(devExt, irql);
        devExt->Fdo.DmTimerFlags.FailUrbs = 1;
        USBPORT_RELEASE_DM_LOCK(devExt, irql);

        // flushes all active transfers off the hardware.  The FailUrbs flag will block new
        // transfers from being queued
        USBPORT_Core_FlushController(devExt->HcFdoDeviceObject);

        // force transition pending endpoints
        USBPORT_ProcessNeoStateChangeList(devExt->HcFdoDeviceObject);

        // Suspend the controller
        //
        MPf_SuspendController(devExt, TRUE);

        //
        // Controller is suspended.  Now we can tell PO that we are idle.
        //
        USBPORT_RequestRuntimePMTransitionToIdle(devExt->HcFdoDeviceObject);

        KeReleaseSemaphore(&rhDevExt->Pdo.SSLock,
                           LOW_REALTIME_PRIORITY,
                           1,
                           FALSE);

        rhDevHandle = rhDevExt->Pdo.RootHubDeviceHandle;

        // Note that the controller is now suspended in the
        // Fdo.PowerFlags
        //
        USBPORT_AcquireSpinLock(rhDevExt->HcFdoDeviceObject,
                                &devExt->Fdo.CoreFunctionSpin, &irql);

        devExt->Fdo.PowerFlags.Suspended = 1;

        USBPORT_ReleaseSpinLock(rhDevExt->HcFdoDeviceObject,
                                &devExt->Fdo.CoreFunctionSpin, irql);

        // ref the device handle before accessing it
        nts = USBPORT_iReferenceDeviceHandle(rhDevExt->HcFdoDeviceObject,
                                             rhDevHandle,
                                             Pdo,
                                             DEVH_RH_CA_IDLE_TAG); //rhID

        // handle may not be valid if root hub was removed or stopped
        if (NT_SUCCESS(nts)) {
            // mark the root hub as idle
            USBPORT_SetDevH_IdleReadyState(rhDevExt->HcFdoDeviceObject,
                                           rhDevHandle,
                                           1);

            USBPORT_DereferenceDeviceHandle(rhDevExt->HcFdoDeviceObject,
                                            rhDevHandle,
                                            Pdo,
                                            DEVH_RH_CA_IDLE_TAG,  //rhID
                                            FALSE);
        }
        nts = STATUS_SUCCESS;
    }

    if (NT_SUCCESS(nts)) {

        if (MPf_CheckInvalidateRootHub(devExt) == TRUE) {
            USBPORT_Ev_Rh_IntrEp_Invalidate(rhDevExt->HcFdoDeviceObject);
        }

        //
        // Even though we are not using D IRPs for hub selective
        // suspend, we are changing the power state of the PDO.
        // This enables a power user to look at the power state
        // of the hub from device manager and determine if it is
        // suspended
        //
        powerState.DeviceState = PowerDeviceD2;
        PoSetPowerState(Pdo, DevicePowerState, powerState);
    }

    return nts;
}


NTSTATUS
USBPORTBUSIF_ResumeRootHub(
    PDEVICE_OBJECT Pdo
    )
/*++

Routine Description:

    Interface function exported thru IRP_MN_QUERY_INTERFACE for
    USB_BUS_INTERFACE_HUB_MINIDUMP_GUID.

    Called by the USBHUB driver when it is going to resume the root hub.

Arguments:

Return Value:

--*/
{
    PDEVICE_OBJECT HcFdo;
    PDEVICE_EXTENSION rhDevExt, devExt;
    NTSTATUS nts = STATUS_SUCCESS;
    PUSBD_DEVICE_HANDLE rhDevHandle;
    USB_MINIPORT_STATUS mpStatus = USBMP_STATUS_SUCCESS;
    KIRQL irql;
    POWER_STATE powerState;

    GET_DEVICE_EXT(rhDevExt, Pdo);
    ASSERT_PDOEXT(rhDevExt);

    HcFdo = rhDevExt->HcFdoDeviceObject;
    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    // Note that the controller is no longer suspended in the
    // Fdo.PowerFlags
    //
    USBPORT_AcquireSpinLock(rhDevExt->HcFdoDeviceObject,
                            &devExt->Fdo.CoreFunctionSpin, &irql);

    devExt->Fdo.PowerFlags.Suspended = 0;

    USBPORT_ReleaseSpinLock(rhDevExt->HcFdoDeviceObject,
                            &devExt->Fdo.CoreFunctionSpin, irql);

    KeWaitForSingleObject(&rhDevExt->Pdo.SSLock,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);

    //
    // before resuming the controller i.e. touching the hardware
    // its time to transition to runtime active. this call is blocking
    //

    USBPORT_RequestRuntimePMTransitionToActive(devExt->HcFdoDeviceObject);

    USBPORT_Core_ResumeAllEndpoints(HcFdo);

    // Resume the controller
    //
    mpStatus = MPf_ResumeController(devExt);

    rhDevExt->Pdo.SelectiveSuspend = FALSE;


    USBPORT_ACQUIRE_DM_LOCK(devExt, irql);
    devExt->Fdo.DmTimerFlags.FailUrbs = 0;
    USBPORT_RELEASE_DM_LOCK(devExt, irql);

    KeReleaseSemaphore(&rhDevExt->Pdo.SSLock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);

    rhDevHandle = rhDevExt->Pdo.RootHubDeviceHandle;

    // ref the device handle before accessing it
    nts = USBPORT_iReferenceDeviceHandle(rhDevExt->HcFdoDeviceObject,
                                         rhDevHandle,
                                         Pdo,
                                         DEVH_RH_CA_IDLE_TAG); //rhID

    // handle may not be valid if root hub was removed or stopped
    if (NT_SUCCESS(nts)) {
        // mark the root hub as not idle
        USBPORT_SetDevH_IdleReadyState(rhDevExt->HcFdoDeviceObject,
                                       rhDevHandle,
                                       0);

        USBPORT_DereferenceDeviceHandle(rhDevExt->HcFdoDeviceObject,
                                        rhDevHandle,
                                        Pdo,
                                        DEVH_RH_CA_IDLE_TAG, //rhID
                                        FALSE);
    }

    LOGENTRY(HcFdo, LOG_MISC, 'BiRH', 0, 0, mpStatus);

    if (mpStatus != USBMP_STATUS_SUCCESS) {

        //
        // Controller resume fails!  This means the hardware is not
        // responding for some reason.  It could be removed or just
        // not functioning.
        //
        KIRQL cf_irql;

        USBPORT_AcquireSpinLock(HcFdo, &devExt->Fdo.CoreFunctionSpin, &cf_irql);
        devExt->Fdo.PowerFlags.MpRestartFailed = 1;
        USBPORT_ReleaseSpinLock(HcFdo, &devExt->Fdo.CoreFunctionSpin, cf_irql);

        IoInvalidateDeviceState(devExt->Fdo.PhysicalDeviceObject);

        nts = USBPORT_MiniportStatus_TO_NtStatus(mpStatus);
    }
    else {
        // Propagate resume to all ports
        nts = USBPORT_SendSynchronousUsbIoctlRequest(HcFdo, IOCTL_INTERNAL_USB_PROPAGATE_RESUME);
    }

    //
    // Even though we are not using D IRPs for hub selective
    // suspend, we are changing the power state of the PDO.
    // This enables a power user to look at the power state
    // of the hub from device manager and determine if it is
    // suspended
    //
    powerState.DeviceState = PowerDeviceD0;
    PoSetPowerState(Pdo, DevicePowerState, powerState);

    return nts;
}


NTSTATUS
USBPORT_SendSynchronousUsbIoctlRequest(
    PDEVICE_OBJECT HcFdo,
    ULONG Ioctl
    )
/*++

Routine Description:

    Synchronously sends an IOCTL api to the the top of the device stack for a USB PDO.
    Such APIs are typically targeted at the hub FDO but could be targeted at the PDOs.
    The important thing to keep in mind is that the function and filter drivers in will
    see this request.

    This routine will allocate the irp and reference the device stack across the call.

    This routine is used to send requests from the root hub down the stack to all
    leaf nodes.

Arguments:

Return Value:

    ntstatus code returned from the call

--*/
{
    NTSTATUS nts;
    KEVENT event = {0};
    PIRP irp;
    IO_STATUS_BLOCK ioStatus;
    PDEVICE_EXTENSION devExt;
    PDEVICE_OBJECT topOfDeviceStack;
    PDEVICE_OBJECT rhPdo = NULL;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);

    do {
        rhPdo = USBPORT_RefRootHubPdo(HcFdo, REF_RhPDO_SSP);
        if (rhPdo == NULL) {
            nts = STATUS_NO_SUCH_DEVICE;
            break;
        }

        topOfDeviceStack = IoGetAttachedDeviceReference(rhPdo);

        if (topOfDeviceStack == NULL) {
            nts = STATUS_NO_SUCH_DEVICE;
            break;
        }

        KeInitializeEvent(&event, NotificationEvent, USBD_EVENT_STATE_NOT_SIGNALED);

        USBPORT_ASSERT(topOfDeviceStack);
        irp = IoBuildDeviceIoControlRequest(Ioctl,
                                            topOfDeviceStack,
                                            NULL,
                                            0,
                                            NULL,
                                            0,
                                            TRUE,  // INTERNAL
                                            &event,
                                            &ioStatus);

        if (irp == NULL) {
            ObDereferenceObject(topOfDeviceStack);
            nts = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        // send packet directly to bus driver
        nts = IoCallDriver(topOfDeviceStack, irp);

        if (nts == STATUS_PENDING) {
           KeWaitForSingleObject(&event,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);
        } else {
            ioStatus.Status = nts;
        }

        nts = ioStatus.Status;

        //
        ObDereferenceObject(topOfDeviceStack);

    } WHILE (0);

    if (rhPdo) {
        USBPORT_DerefRootHubPdo(HcFdo, rhPdo, REF_RhPDO_SSP);
    }

    return nts;
}


NTSTATUS
USBPORT_GetBusMiniDumpInterface(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    IRP_MN_QUERY_INTERFACE handler for USB_BUS_INTERFACE_HUB_MINIDUMP_GUID.

Arguments:

Return Value:

    NT status code.

--*/
{
    USHORT requestedVersion;
    NTSTATUS ntStatus;
    PBUS_CONTEXT busContext = NULL;
    PDEVICE_EXTENSION rhDevExt = NULL;
    PIO_STACK_LOCATION irpStack = NULL;
    PUSBD_DEVICE_HANDLE deviceHandle = NULL;

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    requestedVersion = irpStack->Parameters.QueryInterface.Version;
    deviceHandle = irpStack->Parameters.QueryInterface.InterfaceSpecificData;

    if (NULL == deviceHandle) {
        deviceHandle = rhDevExt->Pdo.RootHubDeviceHandle;
    }

    ASSERT_DEVICE_HANDLE(deviceHandle);

    ntStatus = USBPORT_GetBusContext(&busContext, PdoDeviceObject, deviceHandle);
    if (!NT_SUCCESS(ntStatus)) {
        return ntStatus;
    }

    ASSERT_BUS_CONTEXT(busContext);

    // assume success
    ntStatus = STATUS_SUCCESS;

    if (requestedVersion >= USB_BUSIF_HUB_MIDUMP_VERSION_0) {

        PUSB_BUS_INTERFACE_HUB_MINIDUMP MiniDumpBusInterface;

        MiniDumpBusInterface = (PUSB_BUS_INTERFACE_HUB_MINIDUMP)
            irpStack->Parameters.QueryInterface.Interface;

        MiniDumpBusInterface->BusContext                =   busContext;
        MiniDumpBusInterface->InterfaceReference        =   USBPORT_MiniDumpInterfaceReference;
        MiniDumpBusInterface->InterfaceDereference      =   USBPORT_MiniDumpInterfaceDereference;
        MiniDumpBusInterface->SetUsbPortMiniDumpFlags   =   USBPORTBUSIF_SetUsbPortMiniDumpFlags;
        MiniDumpBusInterface->Size                      =   sizeof(USB_BUS_INTERFACE_HUB_MINIDUMP);
        MiniDumpBusInterface->Version                   =   USB_BUSIF_HUB_MIDUMP_VERSION_0;
    }

    if (NT_SUCCESS(ntStatus)) {
        USBPORT_MiniDumpInterfaceReference(busContext);
    }

    return ntStatus;
}


NTSTATUS
USBPORT_GetBusForwardProgressInterface(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    IRP_MN_QUERY_INTERFACE handler for USB_BUS_INTERFACE_HUB_MINIDUMP_GUID.

Arguments:

Return Value:

    NT status code.

--*/
{
    USHORT requestedVersion;
    NTSTATUS ntStatus;
    PBUS_CONTEXT busContext = NULL;
    PDEVICE_EXTENSION rhDevExt = NULL;
    PIO_STACK_LOCATION irpStack = NULL;
    PUSBD_DEVICE_HANDLE deviceHandle = NULL;

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    requestedVersion = irpStack->Parameters.QueryInterface.Version;
    deviceHandle = irpStack->Parameters.QueryInterface.InterfaceSpecificData;

    if (NULL == deviceHandle) {
        deviceHandle = rhDevExt->Pdo.RootHubDeviceHandle;
    }

    ASSERT_DEVICE_HANDLE(deviceHandle);

    ntStatus = USBPORT_GetBusContext(&busContext, PdoDeviceObject, deviceHandle);
    if (!NT_SUCCESS(ntStatus)) {
        return ntStatus;
    }

    ASSERT_BUS_CONTEXT(busContext);

    // assume success
    ntStatus = STATUS_SUCCESS;

    if (requestedVersion >= USB_BUSIF_HUB_FORWARD_PROGRESS_VERSION_0) {

        PUSB_BUS_INTERFACE_HUB_FORWARD_PROGRESS fpInterface;

        fpInterface = (PUSB_BUS_INTERFACE_HUB_FORWARD_PROGRESS)
            irpStack->Parameters.QueryInterface.Interface;

        fpInterface->BusContext                =   busContext;
        fpInterface->InterfaceReference        =   USBPORT_ForwardProgressInterfaceReference;
        fpInterface->InterfaceDereference      =   USBPORT_ForwardProgressInterfaceDereference;
        fpInterface->AllocateWorkItem          =   USBPORT_AllocateWorkItem;
        fpInterface->FreeWorkItem              =   USBPORT_FreeWorkItem;
        fpInterface->QueueWorkItem             =   USBPORT_QueueWorkItem;
        fpInterface->DeferIrpProcessing        =   USBPORT_DeferIrpProcessing;
        fpInterface->Size                      =   sizeof(USB_BUS_INTERFACE_HUB_FORWARD_PROGRESS);
        fpInterface->Version                   =   USB_BUSIF_HUB_FORWARD_PROGRESS_VERSION_0;
    }

    if (NT_SUCCESS(ntStatus)) {
        USBPORT_ForwardProgressInterfaceReference(busContext);
    }

    return ntStatus;
}


VOID
USBPORTBUSIF_SetUsbPortMiniDumpFlags(
    PVOID BusContext
    )
/*++

Routine Description:

    Interface function exported thru IRP_MN_QUERY_INTERFACE for
    USB_BUS_INTERFACE_HUB_MINIDUMP_GUID.

    Called by the USBHUB driver when it is going to explicity bugcheck
    to notify the USBPORT driver that it should save debugging
    information in the minidump for the host controller to which the
    USBHUB device causing the bugcheck is attached.

Arguments:

Return Value:

--*/
{

    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    USBPORT_HubFailSetMiniDumpData(rhDevExt->HcFdoDeviceObject);
}


NTSTATUS
USBPORT_GetBusInterfaceHub(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject,
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
    USHORT requestedVersion;
    NTSTATUS ntStatus;
    PBUS_CONTEXT busContext = NULL;
    PDEVICE_EXTENSION rhDevExt = NULL;
    PIO_STACK_LOCATION irpStack = NULL;
    PUSBD_DEVICE_HANDLE deviceHandle = NULL;

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    requestedVersion = irpStack->Parameters.QueryInterface.Version;
    deviceHandle = irpStack->Parameters.QueryInterface.InterfaceSpecificData;

    if(NULL == deviceHandle) {
        deviceHandle = rhDevExt->Pdo.RootHubDeviceHandle;
    }

    ASSERT_DEVICE_HANDLE(deviceHandle);

    ntStatus = USBPORT_GetBusContext(&busContext, PdoDeviceObject, deviceHandle);
    if(!NT_SUCCESS(ntStatus)) {
        return ntStatus;
    }

    ASSERT_BUS_CONTEXT(busContext);

    // assume success
    ntStatus = STATUS_SUCCESS;

    if (requestedVersion >= USB_BUSIF_HUB_VERSION_0) {

        PUSB_BUS_INTERFACE_HUB_V0 busInterface0;

        busInterface0 = (PUSB_BUS_INTERFACE_HUB_V0)
            irpStack->Parameters.QueryInterface.Interface;

        busInterface0->BusContext =
            busContext;
        busInterface0->InterfaceReference =
            USBPORT_HubBusInterfaceReference;
        busInterface0->InterfaceDereference =
            USBPORT_HubBusInterfaceDereference;

        busInterface0->Size = sizeof(USB_BUS_INTERFACE_HUB_V0);
        busInterface0->Version = USB_BUSIF_HUB_VERSION_0;
    }

    if (requestedVersion >= USB_BUSIF_HUB_VERSION_1) {

        PUSB_BUS_INTERFACE_HUB_V1 busInterface1;

        busInterface1 = (PUSB_BUS_INTERFACE_HUB_V1)
            irpStack->Parameters.QueryInterface.Interface;

        busInterface1->CreateUsbDevice =
            USBPORTBUSIF_CreateUsbDevice;
        busInterface1->InitializeUsbDevice =
            USBPORTBUSIF_InitializeUsbDevice;
        busInterface1->GetUsbDescriptors =
            USBPORTBUSIF_GetUsbDescriptors;
        busInterface1->RemoveUsbDevice =
            USBPORTBUSIF_RemoveUsbDevice;
        busInterface1->RestoreUsbDevice =
            USBPORTBUSIF_RestoreUsbDevice;
        busInterface1->QueryDeviceInformation =
            USBPORTBUSIF_BusQueryDeviceInformation;

        busInterface1->Size = sizeof(USB_BUS_INTERFACE_HUB_V1);
        busInterface1->Version = USB_BUSIF_HUB_VERSION_1;
    }

    // note that version 2 is a superset of version 1
    if (requestedVersion >= USB_BUSIF_HUB_VERSION_2) {

        PUSB_BUS_INTERFACE_HUB_V2 busInterface2;

        busInterface2 = (PUSB_BUS_INTERFACE_HUB_V2)
            irpStack->Parameters.QueryInterface.Interface;

        busInterface2->GetControllerInformation =
            USBPORTBUSIF_GetControllerInformation;
        busInterface2->ControllerSelectiveSuspend =
            USBPORTBUSIF_ControllerSelectiveSuspend;
        busInterface2->GetExtendedHubInformation =
            USBPORTBUSIF_GetExtendedHubInformation;
        busInterface2->GetRootHubSymbolicName =
            USBPORTBUSIF_GetRootHubSymbolicName;
        busInterface2->GetDeviceBusContext =
            USBPORTBUSIF_GetDeviceBusContext;
        busInterface2->Initialize20Hub =
            USBPORTBUSIF_InitailizeUsb2Hub;

        busInterface2->Size = sizeof(USB_BUS_INTERFACE_HUB_V2);
        busInterface2->Version = USB_BUSIF_HUB_VERSION_2;
    }

    // note that version 3 is a superset of version 2 & 1
    if (requestedVersion >= USB_BUSIF_HUB_VERSION_3) {

        PUSB_BUS_INTERFACE_HUB_V3 busInterface3;

        busInterface3 = (PUSB_BUS_INTERFACE_HUB_V3)
            irpStack->Parameters.QueryInterface.Interface;

        busInterface3->RootHubInitNotification =
            USBPORTBUSIF_RootHubInitNotification;

        busInterface3->Size = sizeof(USB_BUS_INTERFACE_HUB_V3);
        busInterface3->Version = USB_BUSIF_HUB_VERSION_3;
    }

     // note that version 4 is a superset of version 3,2 & 1
    if (requestedVersion >= USB_BUSIF_HUB_VERSION_4) {

        PUSB_BUS_INTERFACE_HUB_V4 busInterface4;

        busInterface4 = (PUSB_BUS_INTERFACE_HUB_V4)
            irpStack->Parameters.QueryInterface.Interface;

        busInterface4->FlushTransfers =
            USBPORTBUSIF_FlushTransfers;



        busInterface4->Size = sizeof(USB_BUS_INTERFACE_HUB_V4);
        busInterface4->Version = USB_BUSIF_HUB_VERSION_4;

    }

    if (requestedVersion >= USB_BUSIF_HUB_VERSION_5) {

        PUSB_BUS_INTERFACE_HUB_V5 busInterface5;

        busInterface5 = (PUSB_BUS_INTERFACE_HUB_V5)
            irpStack->Parameters.QueryInterface.Interface;

        busInterface5->SetDeviceHandleData =
            USBPORTBUSIF_SetDeviceHandleData;

        busInterface5->Size = sizeof(USB_BUS_INTERFACE_HUB_V5);
        busInterface5->Version = USB_BUSIF_HUB_VERSION_5;

    }

    if (requestedVersion >= USB_BUSIF_HUB_VERSION_6) {

        PUSB_BUS_INTERFACE_HUB_V6 busInterface6;

        busInterface6 = (PUSB_BUS_INTERFACE_HUB_V6)
            irpStack->Parameters.QueryInterface.Interface;

        busInterface6->HubIsRoot =
            USBPORTBUSIF_HubIsRoot;
        busInterface6->CreateUsbDevice =
            USBPORTBUSIF_CreateUsbDeviceEx;
        busInterface6->InitializeUsbDevice =
            USBPORTBUSIF_InitializeUsbDeviceEx;
        busInterface6->AcquireBusSemaphore =
            USBPORTBUSIF_AcquireBusLock;
        busInterface6->ReleaseBusSemaphore =
            USBPORTBUSIF_ReleaseBusLock;
        busInterface6->CaculatePipeBandwidth =
            USBPORTBUSIF_CaculatePipeBandwidth;
        busInterface6->SetBusSystemWakeMode =
            USBPORTBUSIF_SetBusSystemWakeMode;
        busInterface6->SetDeviceFlag =
            USBPORTBUSIF_SetDeviceFlag;
        busInterface6->HubTestPoint = NULL;
        busInterface6->GetDevicePerformanceInfo =
            USBPORTBUSIF_GetDevicePerformanceInfo;
        busInterface6->WaitAsyncPowerUp =
            USBPORTBUSIF_WaitAsyncPowerUp;
        busInterface6->RefDeviceHandle =
            USBPORTBUSIF_RefDeviceHandle;
        busInterface6->DerefDeviceHandle =
            USBPORTBUSIF_DerefDeviceHandle;
        busInterface6->SetDeviceHandleIdleReadyState =
            USBPORTBUSIF_SetDevH_IdleReadyState;

        busInterface6->Size = sizeof(USB_BUS_INTERFACE_HUB_V6);
        busInterface6->Version = USB_BUSIF_HUB_VERSION_6;

    }

    if (requestedVersion >= USB_BUSIF_HUB_VERSION_7) {

        PUSB_BUS_INTERFACE_HUB_V7 busInterface7;

        busInterface7 = (PUSB_BUS_INTERFACE_HUB_V7)
            irpStack->Parameters.QueryInterface.Interface;

        busInterface7->CreateUsbDeviceV7 =
            USBPORTBUSIF_CreateUsbDeviceV7;

        busInterface7->GetContainerIdForPort =
            USBPORTBUSIF_GetContainerIdForPort;

        busInterface7->SetContainerIdForPort =
            USBPORTBUSIF_SetContainerIdForPort;

        busInterface7->AbortAllDevicePipes =
            USBPORTBUSIF_AbortAllDevicePipes;

        busInterface7->SetDeviceErrataFlag =
            USBPORTBUSIF_SetDeviceErrataFlag;

        busInterface7->Size = sizeof(USB_BUS_INTERFACE_HUB_V7);
        busInterface7->Version = USB_BUSIF_HUB_VERSION_7;

    }

    if (requestedVersion >= USB_BUSIF_HUB_VERSION_8) {

        PUSB_BUS_INTERFACE_HUB_V8 busInterface8;

        busInterface8 = (PUSB_BUS_INTERFACE_HUB_V8)
            irpStack->Parameters.QueryInterface.Interface;

        busInterface8->GetDebugPortNumber =
            USBPORTBUSIF_GetDebugPortNumber;

        busInterface8->Size = sizeof(USB_BUS_INTERFACE_HUB_V8);
        busInterface8->Version = USB_BUSIF_HUB_VERSION_8;

    }

    if (requestedVersion >= USB_BUSIF_HUB_VERSION_9) {

        PUSB_BUS_INTERFACE_HUB_V9 busInterface9;

        busInterface9 = (PUSB_BUS_INTERFACE_HUB_V9)
            irpStack->Parameters.QueryInterface.Interface;

        busInterface9->SetMsOs20Support =
            USBPORTBUSIF_SetMsOs20Support;

        busInterface9->SetClearRootPortResumeTime =
            USBPORTBUSIF_SetClearRootPortResumeTime;

        busInterface9->Size = sizeof(USB_BUS_INTERFACE_HUB_V9);
        busInterface9->Version = USB_BUSIF_HUB_VERSION_9;

    }


    if (NT_SUCCESS(ntStatus)) {
        USBPORT_HubBusInterfaceReference(busContext);
    }

    return ntStatus;
}


VOID
USBPORTBUSIF_AcquireBusLock(
    PVOID BusContext
    )
/*++

Routine Description:

Service exported for use by the hub driver

Acquires or releases the global bus semaphore.  This semaphore is used by the
hub to serialize parts of enumeration.  The semaphore is held by the hub as
long as a device is at address zero (0).

IRQL = PASSIVE

Arguments:

Return Value:

    NT status code.

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt, devExt;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    GET_DEVICE_EXT(devExt, rhDevExt->HcFdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    //USBPORT_ASSERT(devExt->Fdo.HubBusInterfaceReference);

    KeWaitForSingleObject(&devExt->Fdo.BusLock,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);



    return;
}


VOID
USBPORTBUSIF_ReleaseBusLock(
    PVOID BusContext
    )
/*++

Routine Description:

Service exported for use by the hub driver

Acquires or releases the global bus semaphore.  This semaphore is used by the
hub to serialize parts of enumeration.  The semaphore is held by the hub as
long as a device is at address zero (0).

IRQL = PASSIVE

Arguments:

Return Value:

    NT status code.

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt, devExt;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    GET_DEVICE_EXT(devExt, rhDevExt->HcFdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    //USBPORT_ASSERT(devExt->Fdo.HubBusInterfaceReference);

    KeReleaseSemaphore(&devExt->Fdo.BusLock,
                               LOW_REALTIME_PRIORITY,
                               1,
                               FALSE);

    return;
}


VOID
USBPORTBUSIF_SetBusSystemWakeMode(
    PVOID BusContext,
    ULONG Mode
    )
/*++

Routine Description:

Service exported for use by the hub driver

This routine will enable resume signaling on the USB to wake the system.  The
hub driver calls this function if 1 or more USB devices are 'enabled' to wake
the system.

IRQL = PASSIVE

Arguments:

Return Value:

    NT status code.

--*/
{

    PBUS_CONTEXT busContext = BusContext;
    PDEVICE_EXTENSION rhDevExt;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(rhDevExt, busContext->RhPdo);
    ASSERT_PDOEXT(rhDevExt);

    if (Mode) {
       busContext->WakeState = BusCtx_WakeUsb;
    } else {
       busContext->WakeState = BusCtx_No_WakeUsb;
    }

    //
    // Ensure that the controller is powered up before we touch the hardware
    //
    USBPORT_RequestRuntimePMTransitionToActive(rhDevExt->HcFdoDeviceObject);

    //jd->vinma if you want to arm the bus by default this
    // code will disrm as appropriate -- we would then be in line
    // with the 'arm early arm often' policy.
    USBPORT_CheckAndArmHcForWake(rhDevExt->HcFdoDeviceObject);

    USBPORT_RequestRuntimePMTransitionToIdle(rhDevExt->HcFdoDeviceObject);

    return;
}


VOID
USBPORTBUSIF_SetDeviceFlag(
    PVOID BusContext,
    GUID *DeviceFlagGuid,
    PVOID ValueData,
    ULONG ValueLingth
    )
/*++

Routine Description:

Service exported for use by the hub driver

This routine is called by the hub driver to tell us about any hk flags that may
exist in the registry or elsewhere.

The hk flags are guid based and documented in reg.c.html in the hub driver project

IRQL = PASSIVE

Arguments:

Return Value:

    NT status code.

--*/
{
    ASSERT_PASSIVE();

    // there are not any we care about yet

    return;
}


ULONG
USBPORTBUSIF_CaculatePipeBandwidth(
    PVOID BusContext,
    PUSBD_PIPE_INFORMATION PipeInfo,
    USB_DEVICE_SPEED DeviceSpeed
    )
/*++

Routine Description:

    caclculate the bandwidth for a pipe given only information from the
    pipeinfo structure.  That is the pipe handle and the endpoint may not be
    valid.

    For high speed devices and for devices thru a TT this calculation is

Arguments:

Return Value:

    bandwidth required in bits/sec.
    returns 0 if the PipeInfo->PipeType or DeviceSpeed input values are invalid

--*/
{

    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt;
    ULONG bw;
    ENDPOINT_TRANSFER_TYPE epType;
    DEVICE_SPEED epSpeed;
    ENDPOINT_TRANSFER_DIRECTION epDir;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    // map parameters
    switch(PipeInfo->PipeType) {
    case UsbdPipeTypeControl:
        epType = Control;
        break;
    case UsbdPipeTypeIsochronous:
        epType = Isochronous;
        break;
    case UsbdPipeTypeBulk:
        epType = Bulk;
        break;
    case UsbdPipeTypeInterrupt:
        epType = Interrupt;
        break;
    default: return 0;
    }

    switch(DeviceSpeed) {
    case UsbLowSpeed:
        epSpeed = LowSpeed;
        break;
    case UsbFullSpeed:
        epSpeed = FullSpeed;
        break;
    case UsbHighSpeed:
        epSpeed = HighSpeed;
        break;
    default: return 0;
    }

    if (USB_ENDPOINT_DIRECTION_IN(PipeInfo->EndpointAddress)) {
        epDir = In;
    } else {
        epDir = Out;
    }

    bw = USBPORT_CalculateUsbBandwidth(rhDevExt->HcFdoDeviceObject,
                                       PipeInfo->MaximumPacketSize,
                                       epSpeed,
                                       epType,
                                       epDir);


    return bw*1000;
}


NTSTATUS
USBPORTBUSIF_CreateUsbDevice(
    PVOID BusContext,
    PUSB_DEVICE_HANDLE *NewDeviceHandle,
    PUSB_DEVICE_HANDLE HubDeviceHandle,
    USHORT PortStatus,
    USHORT PortNumber
    )
/*++
Routine Description:

no longer supported

--*/
{

    return STATUS_NOT_SUPPORTED;

}


NTSTATUS
USBPORTBUSIF_CreateUsbDeviceEx(
    PVOID                       BusContext,
    PUSB_DEVICE_HANDLE          *NewDeviceHandle,
    PUSB_DEVICE_HANDLE          HsHubDeviceHandle,
    USHORT                      PortStatus,
    USHORT                      PortNumber,
    PUSB_CD_ERROR_INFORMATION   CdErrorInfo,
    USHORT                      TtPortNumber
    )
/*++
Routine Description:

Service exported for use by the hub driver

Called for each new device on the USB bus, this function sets up
the internal data structures a port driver needs to keep track of the device.

IRQL = PASSIVE_LEVEL

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - A pointer to return the handle of the new device to the hub.
The value returned here is passed in the DeviceHandle field of all URBs
passed to the USB port driver and references this device.

HubDeviceHandle - device handle for the hub creating the device.  This is
used to keep track which TT is associated with the device if the device is
connected to a USB 2.0 hub. For NEW_HUB this must be the hub handle that owns
the TTs.

PortStatus - port status bits (see chap 11 of USB core specification). These
are the bits read from the port status register after the first USB reset has
completed.

PortNumber - Not Used

Return Value:

    NT status code.


--*/
{
    NTSTATUS nts;
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt;
    PUSBD_DEVICE_HANDLE deviceHandle;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    //
    // Ensure that the controller is powered up before we touch the hardware
    //
    USBPORT_RequestRuntimePMTransitionToActive(rhDevExt->HcFdoDeviceObject);

    nts =
        USBPORT_NeoCreateDevice(&deviceHandle,
                                rhDevExt->HcFdoDeviceObject,
                                HsHubDeviceHandle,
                                PortStatus,
                                NULL,           // PortPath
                                CdErrorInfo,
                                TtPortNumber,
                                NULL,           // PdoDeviceObject
                                NULL);          // PhysicalDeviceObjectName


    if (NT_SUCCESS(nts)) {
        // create the handle with a reference to the buscontext.
        // It is released when it is removed by the hub or there is
        // an implied release of the handle (e.g. ini failure)
        USBPORT_iReferenceDeviceHandle(rhDevExt->HcFdoDeviceObject,
                                       deviceHandle,
                                       BusContext,
                                       DEVH_CREATE_DEV_TAG); //creD

        *NewDeviceHandle = (PUSB_DEVICE_HANDLE) deviceHandle;

        USBPORT_ASSERT(deviceHandle);


    } else {
        USBPORT_ASSERT(deviceHandle == NULL);
        *NewDeviceHandle = NULL;
    }

    USBPORT_RequestRuntimePMTransitionToIdle(rhDevExt->HcFdoDeviceObject);

    return nts;
}


NTSTATUS
USBPORTBUSIF_CreateUsbDeviceV7(
    PVOID                       BusContext,
    PUSB_DEVICE_HANDLE          *NewDeviceHandle,
    PUSB_DEVICE_HANDLE          HsHubDeviceHandle,
    USHORT                      PortStatus,
    PUSB_PORT_PATH              PortPath,
    PUSB_CD_ERROR_INFORMATION   CdErrorInfo,
    USHORT                      TtPortNumber,
    PDEVICE_OBJECT              PdoDeviceObject,
    PUNICODE_STRING             PhysicalDeviceObjectName
    )
/*++
Routine Description:

Service exported for use by the hub driver

Called for each new device on the USB bus, this function sets up
the internal data structures a port driver needs to keep track of the device.

IRQL = PASSIVE_LEVEL

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - A pointer to return the handle of the new device to the hub.
The value returned here is passed in the DeviceHandle field of all URBs
passed to the USB port driver and references this device.

HubDeviceHandle - device handle for the hub creating the device.  This is
used to keep track which TT is associated with the device if the device is
connected to a USB 2.0 hub. For NEW_HUB this must be the hub handle that owns
the TTs.

PortStatus - port status bits (see chap 11 of USB core specification). These
are the bits read from the port status register after the first USB reset has
completed.

PortPath - List of 1-based port numbers on the chain of hubs through which a
device is attached.

Return Value:

    NT status code.


--*/
{
    NTSTATUS nts;
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt;
    PUSBD_DEVICE_HANDLE deviceHandle;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);
    //
    // Ensure that the controller is powered up before we touch the hardware
    //
    USBPORT_RequestRuntimePMTransitionToActive(rhDevExt->HcFdoDeviceObject);

    nts =
        USBPORT_NeoCreateDevice(&deviceHandle,
                                rhDevExt->HcFdoDeviceObject,
                                HsHubDeviceHandle,
                                PortStatus,
                                PortPath,
                                CdErrorInfo,
                                TtPortNumber,
                                PdoDeviceObject,
                                PhysicalDeviceObjectName);

    if (NT_SUCCESS(nts)) {
        // create the handle with a reference to the buscontext.
        // It is released when it is removed by the hub or there is
        // an implied release of the handle (e.g. ini failure)
        USBPORT_iReferenceDeviceHandle(rhDevExt->HcFdoDeviceObject,
                                       deviceHandle,
                                       BusContext,
                                       DEVH_CREATE_DEV_TAG); //creD

        *NewDeviceHandle = (PUSB_DEVICE_HANDLE) deviceHandle;

        USBPORT_ASSERT(deviceHandle);


    } else {
        USBPORT_ASSERT(deviceHandle == NULL);
        *NewDeviceHandle = NULL;
    }

    USBPORT_RequestRuntimePMTransitionToIdle(rhDevExt->HcFdoDeviceObject);
    return nts;
}


NTSTATUS
USBPORTBUSIF_RootHubInitNotification(
    PVOID BusContext,
    PVOID CallbackContext,
    PRH_INIT_CALLBACK CallbackRoutine
    )
/*++

Routine Description:

*    This is where we hold the enumeration of the CC root hubs until
*    the USB 2.0 controller has initialized.

A notification request issued by usbhub.sys when it is loaded for the root
hub to synchronize enumeration with one or more USB 2.0 companion controllers
.  The port driver calls the notification function when it is ok to enumerate
devices.  The purpose of this function is to delay enumeration on USB
companion controllers until the associated USB 2.0 controller starts.  This
minimizes the number of times the device will move from one bus to the other
during boot.

IRQL = ANY

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

CallBackContext - A hub driver specific context value to be passed to the
notification callback function.

CallbackFunction - Function to call when it is OK to enumerate devices.  The
callback is called at raised IRQL (DPC_LEVEL).

typedef VOID
    (__stdcall *PRH_INIT_CALLBACK) (
        IN PVOID
        );

Return Value:

    NT status code.

    If a callback is registered when this routine is called, it is replaced
    with the new one and STATUS_CANCELED is returned.  This allows the hub
    driver to detect if the callback is still pending.

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_OBJECT fdoDeviceObject;
    PDEVICE_EXTENSION rhDevExt, devExt;
    KIRQL irql;
    PRH_INIT_CALLBACK oldCb; //cb is old calback

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;
    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    // remember the callback
    KeAcquireSpinLock(&devExt->Fdo.HcSyncSpin, &irql);
    rhDevExt->Pdo.HubInitContext = CallbackContext;
    oldCb = rhDevExt->Pdo.HubInitCallback;
    rhDevExt->Pdo.HubInitCallback = CallbackRoutine;
    KeReleaseSpinLock(&devExt->Fdo.HcSyncSpin, irql);


    if (oldCb) {
        return STATUS_CANCELLED;
    } else {
        // perform a flush here in case this is not a USB2 or USB11cc
        USBPORT_FlushRhCallbacks(fdoDeviceObject);
        return STATUS_SUCCESS;
    }

}


NTSTATUS
USBPORTBUSIF_InitailizeUsb2Hub(
    PVOID BusContext,
    PUSB_DEVICE_HANDLE HubDeviceHandle,
    ULONG TtCount
    )
/*++

Routine Description:

Service exported for use by the hub driver

Initialize internal USBPORT structures for a USB 2.0 hub, called during the
hub start process.  This routine sets up the budgeter code for the TTs.

IRQL = ANY

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

HubDeviceHandle - device handle for this hub.

TtCount - Transaction translator count, this is either 1 or equal to the
number of downstream ports depending on what the hub reported in its
configuration descriptor.

Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus;
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    LOGENTRY(rhDevExt->HcFdoDeviceObject,
        LOG_MISC, 'i2hb', BusContext, HubDeviceHandle, TtCount);

    ntStatus =
        USBPORT_InitializeHsHub(rhDevExt->HcFdoDeviceObject,
                                HubDeviceHandle,
                                TtCount);

    return ntStatus;
}


NTSTATUS
USBPORTBUSIF_RefDeviceHandle(
    PVOID BusContext,
    PUSB_DEVICE_HANDLE DeviceHandle,
    PVOID Object,
    ULONG Tag
    )
/*++

Routine Description:

Service exported for use by the hub driver

Allows the hub driver to add additional references to a port device handle.

IRQL = ANY

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - device handle fto ref.

Object - object associated with this reference.

Tag - optional tag associated with the reference -- good for debugging.

Return Value:

    NT status code.

--*/
{
    NTSTATUS nts;
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    LOGENTRY(rhDevExt->HcFdoDeviceObject,
        LOG_MISC, 'rfDH', BusContext, DeviceHandle, Object);


    nts = USBPORT_rReferenceDeviceHandle(rhDevExt->HcFdoDeviceObject,
                                         DeviceHandle,
                                         Object,
                                         NULL, // no irp
                                         Tag);

    return nts;
}


VOID
USBPORTBUSIF_DerefDeviceHandle(
    PVOID BusContext,
    PUSB_DEVICE_HANDLE DeviceHandle,
    PVOID Object,
    ULONG Tag
    )
/*++

Routine Description:

Service exported for use by the hub driver

Releases a reference acquired with USBPORTBUSIF_RefDeviceHandle

IRQL = ANY

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - device handle to deref.

Object - object associated with this reference.

Return Value:

    none.

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);


    LOGENTRY(rhDevExt->HcFdoDeviceObject,
           LOG_MISC, 'dfDH', Tag, DeviceHandle, Object);

    USBPORT_DereferenceDeviceHandle(rhDevExt->HcFdoDeviceObject,
                                    DeviceHandle,
                                    Object,
                                    Tag,
                                    TRUE);

    return;
}


NTSTATUS
USBPORTBUSIF_InitializeUsbDevice(
    PVOID BusContext,
    PUSB_DEVICE_HANDLE DeviceHandle
    )
/*++

no longer supported

--*/
{

    return STATUS_NOT_SUPPORTED;

}


NTSTATUS
USBPORTBUSIF_InitializeUsbDeviceEx(
    PVOID BusContext,
    PUSB_DEVICE_HANDLE DeviceHandle,
    PUSB_ID_ERROR_INFORMATION IdErrInfo
    )
/*++

Routine Description:

Service exported for use by the hub driver

This function finalizes the initialization of the DeviceHandle returned from
CreateUsbDevice and assigns the device a USB address.

IRQL = PASSIVE_LEVEL

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle -The handle of the new device on the hub returned from
CreateUsbDevice.  The value returned here is passed in the DeviceHandle field
of all URBs passed to the USB port driver and references this device.


Arguments:

Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus;
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    //
    // Ensure that the controller is powered up before we touch the hardware
    //
    USBPORT_RequestRuntimePMTransitionToActive(rhDevExt->HcFdoDeviceObject);

    ntStatus =
        USBPORT_InitializeDevice(DeviceHandle,
                                 rhDevExt->HcFdoDeviceObject,
                                 IdErrInfo);

    if (NT_ERROR(ntStatus)) {
        // current API implies a release on error
        USBPORT_DereferenceDeviceHandle(rhDevExt->HcFdoDeviceObject,
                                        DeviceHandle,
                                        BusContext,
                                        DEVH_CREATE_DEV_TAG, //creD
                                        FALSE);
    }

    USBPORT_RequestRuntimePMTransitionToIdle(rhDevExt->HcFdoDeviceObject);

    return ntStatus;
}


BOOLEAN
USBPORTBUSIF_HubIsRoot(
    PVOID BusContext,
    PVOID DeviceObject
    )
/*++

Routine Description:

Service exported for use by the hub driver

This routine returns true if the PhysicalDeviceObject passed in is for the
root hub on the controller.

IRQL = ANY

Arguments:

Return Value:

    NT status code.

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_OBJECT pdo = DeviceObject;
    PDEVICE_EXTENSION rhDevExt;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    if (pdoDeviceObject == pdo) {
        return TRUE;
    }

    return FALSE;
}


NTSTATUS
USBPORTBUSIF_WaitAsyncPowerUp(
    PVOID BusContext
    )
/*++
Routine Description:

Service exported for use by the hub driver

This routine is used to wait for USB2 synchronization on resume

IRQL = ANY

Arguments:

Return Value:

    NT status code.

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt, devExt;
    NTSTATUS nts;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    GET_DEVICE_EXT(devExt, rhDevExt->HcFdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    DbgTrace((UsbpDebugTrace, "'Wait RootHubReadyEv HcFdo %p\n", rhDevExt->HcFdoDeviceObject));

    nts = KeWaitForSingleObject(
                &devExt->Fdo.RootHubReadyEv,
                Suspended,
                KernelMode,
                FALSE,
                NULL);

    return nts;
}


NTSTATUS
USBPORTBUSIF_GetUsbDescriptors(
    PVOID BusContext,
    PUSB_DEVICE_HANDLE DeviceHandle,
    PUCHAR DeviceDescriptorBuffer,
    PULONG DeviceDescriptorBufferLength,
    PUCHAR ConfigDescriptorBuffer,
    PULONG ConfigDescriptorBufferLength
    )
/*++

Routine Description:

Service exported for use by the hub driver

Called by the hub driver to retrieve the device and configuration descriptors
for a device.  This service may return cached copies of these descriptors
decreasing the amount of traffic to the device.

IRQL = PASSIVE_LEVEL

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - Handle for the device you want information about. This is the
handle returned by CreateUsbDevice or returned in IOCTL api
IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE.

DeviceDescriptorBuffer - caller allocated buffer to return the device
descriptor in.
DeviceDescriptorBufferLength - pointer, input is the size of the
DeviceDescriptorBuffer in bytes. Output is the number of bytes copied to the
buffer.

ConfigDescriptorBuffer - caller allocated buffer to return the configuration
descriptor in.
ConfigDescriptorBufferLength - pointer, input is the size of the
ConfigDescriptorBuffer in bytes. Output is the number of bytes copied to the
buffer.

Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus;
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt;
    PUSBD_DEVICE_HANDLE deviceHandle = DeviceHandle;

    ASSERT_PASSIVE();

    // assume success
    ntStatus = STATUS_SUCCESS;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    // Use the cached device descriptor instead of bothering the device with
    // another request for it.  You would be surprised by how many devices
    // get confused if you ask for descriptors too often back to back.
    //
    if (DeviceDescriptorBuffer && *DeviceDescriptorBufferLength) {

        USBPORT_ASSERT(*DeviceDescriptorBufferLength ==
                       sizeof(USB_DEVICE_DESCRIPTOR));

        USBPORT_ASSERT(deviceHandle->DeviceDescriptor.bLength >=
                       sizeof(USB_DEVICE_DESCRIPTOR));

        if (*DeviceDescriptorBufferLength > sizeof(USB_DEVICE_DESCRIPTOR)) {
            *DeviceDescriptorBufferLength = sizeof(USB_DEVICE_DESCRIPTOR);
        }

        RtlCopyMemory(DeviceDescriptorBuffer,
                      &deviceHandle->DeviceDescriptor,
                      *DeviceDescriptorBufferLength);
    }

    if (NT_SUCCESS(ntStatus) && ConfigDescriptorBuffer && *ConfigDescriptorBufferLength) {
        //
        // Ensure that the controller is powered up before we touch the hardware
        //
        USBPORT_RequestRuntimePMTransitionToActive(rhDevExt->HcFdoDeviceObject);

        ntStatus =
            USBPORT_GetUsbDescriptor(DeviceHandle,
                                     rhDevExt->HcFdoDeviceObject,
                                     USB_CONFIGURATION_DESCRIPTOR_TYPE,
                                     ConfigDescriptorBuffer,
                                     ConfigDescriptorBufferLength);

        USBPORT_RequestRuntimePMTransitionToIdle(rhDevExt->HcFdoDeviceObject);
    }

    return ntStatus;
}


NTSTATUS
USBPORTBUSIF_RemoveUsbDevice(
    PVOID BusContext,
    PUSB_DEVICE_HANDLE DeviceHandle,
    ULONG Flags
    )
/*++

Routine Description:

Service exported for use by the hub driver to remove an instance of a USB
device from the bus.  This function deletes a device handle created with
CreateUsbDevice.

Called by the hub driver to delete

IRQL = PASSIVE_LEVEL

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - Handle for the device you want delete.  This is the handle
returned by CreateUsbDevice or returned in IOCTL api IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE.

Return Value:

    NT status code.

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt;
    PUSBD_DEVICE_HANDLE deviceHandle = DeviceHandle;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    //
    // Ensure that the controller is powered up before we touch the hardware
    //
    USBPORT_RequestRuntimePMTransitionToActive(rhDevExt->HcFdoDeviceObject);

    USBPORT_DereferenceDeviceHandle(rhDevExt->HcFdoDeviceObject,
                                    deviceHandle,
                                    BusContext,
                                    DEVH_CREATE_DEV_TAG, //creD
                                    FALSE);

    USBPORT_NeoRemoveDevice(deviceHandle, rhDevExt->HcFdoDeviceObject);

    USBPORT_RequestRuntimePMTransitionToIdle(rhDevExt->HcFdoDeviceObject);

    return STATUS_SUCCESS;
}


NTSTATUS
USBPORTBUSIF_RestoreUsbDevice(
    PVOID BusContext,
    PUSB_DEVICE_HANDLE OldDeviceHandle,
    PUSB_DEVICE_HANDLE NewDeviceHandle
    )
/*++

Routine Description:

    This function clones the configuration from the 'OldDeviceHandle'
    to the 'NewDevicehandle' iff the device has the same VID/PID .
    On completion the 'old' device handle is feed

Arguments:

Return Value:

    NT status code.

--*/
{
    KIRQL oldIrql;
    NTSTATUS ntStatus;
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt = NULL, devExt = NULL;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    GET_DEVICE_EXT(devExt, rhDevExt->HcFdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    //
    // Ensure that the controller is powered up before we touch the hardware
    //
    USBPORT_RequestRuntimePMTransitionToActive(rhDevExt->HcFdoDeviceObject);

    ntStatus = USBPORT_CloneDevice(rhDevExt->HcFdoDeviceObject,
                                   OldDeviceHandle,
                                   NewDeviceHandle);

    if(NT_SUCCESS(ntStatus)) {
        PBUS_CONTEXT busCon = (PBUS_CONTEXT)BusContext;
        KeAcquireSpinLock(&devExt->Fdo.BusContextSpin, &oldIrql);
        if(busCon->DeviceHandle == OldDeviceHandle) {
            busCon->DeviceHandle = NewDeviceHandle;
            //busCon->Tt = ((PUSBD_DEVICE_HANDLE)NewDeviceHandle)->Tt;
            // retain same ref counts
        }
        KeReleaseSpinLock(&devExt->Fdo.BusContextSpin, oldIrql);

        // on success free the old handle
        USBPORT_DereferenceDeviceHandle(rhDevExt->HcFdoDeviceObject,
                                        OldDeviceHandle,
                                        BusContext,
                                        DEVH_CREATE_DEV_TAG, //creD
                                        FALSE);

    } else {
        //
        // Interface change for Win7:
        //
        // On failure USBHUB expects NewDeviceHandle to remain valid.
        // USBHUB should subsequently call USBPORTBUSIF_RemoveUsbDevice()
        // to release all resources allocated for NewDeviceHandle on
        // failure.
        //
        // Do not delete NewDeviceHandle now, but record that the
        // Ev_RestoreDevice_Fail event has occured for NewDeviceHandle.
        //
        KeAcquireSpinLock(&devExt->Fdo.DevHandleListSpin, &oldIrql);

        USBPORT_SetDeviceHandleState(rhDevExt->HcFdoDeviceObject,
                                     NewDeviceHandle,
                                     ((PUSBD_DEVICE_HANDLE)NewDeviceHandle)->DhState,
                                     Ev_RestoreDevice_Fail);

        KeReleaseSpinLock(&devExt->Fdo.DevHandleListSpin, oldIrql);
    }

    USBPORT_RequestRuntimePMTransitionToIdle(rhDevExt->HcFdoDeviceObject);
    return ntStatus;
}


NTSTATUS
USBPORTBUSIF_GetDevicePerformanceInfo(
    PVOID BusContext,
    PUSB_DEVICE_HANDLE DeviceHandle,
    PVOID DeviceInformationBuffer,
    ULONG DeviceInformationBufferLength,
    PULONG LengthOfDataCopied
    )
/*++

Routine Description:

Service exported for use by the hub driver.  This api returns various
information about a USB devices attached to the system.

    IRQL = PASSIVE_LEVEL

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - Handle for the device you want information about. This is the
handle returned by CreateUsbDevice or returned in IOCTL api
IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE.

DeviceInformationBuffer - buffer for returning device information.

DeviceInformationBufferLength - length of DeviceInformationBuffer in bytes

LengthOfDataReturned - length of data copied into DeviceInformationBuffer.

Return Value:

    NT status code.

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_OBJECT fdoDeviceObject;
    PDEVICE_EXTENSION rhDevExt;
    PDEVICE_EXTENSION fdoDevExt;
    PUSB_DEVICE_PERFORMANCE_INFO_0 level_0 = DeviceInformationBuffer;
    PUSB_LEVEL_INFORMATION levelInfo = DeviceInformationBuffer;
    PUSBD_DEVICE_HANDLE deviceHandle = DeviceHandle;
    PUSBD_PERFORMANCE_DATA performanceData;
    PLIST_ENTRY pipeListEntry;
    ULONG highSpeedTransfersIn32ms[6] = {256, 128, 64, 32, 16, 8};
    ULONG fullSpeedTransfersIn32ms[6] = {32, 16, 8, 4, 2, 1};
    ULONG bw = 0, n = 0;
    BOOLEAN ref_success;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;
    GET_DEVICE_EXT(fdoDevExt, fdoDeviceObject);

    *LengthOfDataCopied = 0;

    if (DeviceInformationBufferLength < sizeof(*levelInfo)) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    if (levelInfo->InformationLevel > 0) {
        // only support level 0
        return STATUS_NOT_SUPPORTED;
    }

    if(DeviceInformationBufferLength <
       sizeof(USB_DEVICE_PERFORMANCE_INFO_0)){
        return STATUS_BUFFER_TOO_SMALL;
    }

    //
    // This will lock out config changes across the call.  Note that as
    // currently implemented the LOCK_DEVICE_CONFIGS() macro will not
    // access the deviceHandle so it does not matter whether or not the
    // deviceHandle if valid at this point.
    //
    LOCK_DEVICE_CONFIGS(deviceHandle, fdoDeviceObject);

    //
    // Validate that the deviceHandle is valid and take a reference on
    // it to make sure it is not deleted while it is being accessed by
    // this routine.
    //
    ref_success = USBPORT_NeoValidDeviceHandle(fdoDeviceObject,
                                               deviceHandle,
                                               Devh_Valid,
                                               BusContext,  // ReferenceObj
                                               NULL,        // unused parameter
                                               DEVH_BUSIF_GDPI_TAG,
                                               FALSE);      // PendingFlag

    if (ref_success == FALSE) {
        // this is most likely a bug in the hub
        // driver
        TEST_TRAP();

        UNLOCK_DEVICE_CONFIGS(deviceHandle, fdoDeviceObject);
        // chances are that the device handle is bad becuse the
        // device is gone.
        return STATUS_DEVICE_NOT_CONNECTED;
    }

    RtlZeroMemory(level_0, sizeof(USB_DEVICE_PERFORMANCE_INFO_0));

    //
    // enough room, fill in the buffer
    //

    level_0->InformationLevel = 0;
    level_0->ActualLength = sizeof(USB_DEVICE_PERFORMANCE_INFO_0);

    performanceData = &deviceHandle->PerformanceData;

    level_0->BulkBytes = performanceData->StatBulkDataBytes;
    level_0->BulkUrbCount = performanceData->StatNumberBulkUrbs;
    level_0->IsoBytes = performanceData->StatIsoDataBytes;
    level_0->IsoUrbCount = performanceData->StatNumberIsoUrbs;
    level_0->InterruptBytes = performanceData->StatInterruptDataBytes;
    level_0->InterruptUrbCount = performanceData->StatNumberInterruptUrbs;
    level_0->ControlDataBytes = performanceData->StatControlDataBytes;
    level_0->ControlUrbCount = performanceData->StatNumberControlUrbs;
    level_0->DroppedIsoPackets = performanceData->DroppedIsoPackets;
    level_0->TotalIsoLatency = performanceData->TotalIsoLatency;
    level_0->TransferErrors = performanceData->TransferErrors;

    //
    // The FDO keeps the total bw in bits/ms
    //
    level_0->Total32secBandwidth = fdoDevExt->Fdo.TotalBusBandwidth * 32;

// NOTE: This code code has been removed due to the fact that it does not
// return correct TT bandwidth information for the PDO, as the bus context passed
// in is for the hub FDO rather than the PDO.  Additionally this code can cause a
// bugcheck in cases where the hub has been rebalanced, as the bus context will
// contain a state device handle.
/*
    if(USBPORT_IS_USB20(fdoDevExt)){
        PTRANSACTION_TRANSLATOR translator;
        NTSTATUS nts;

        nts = USBPORT_TtFromBusContext(fdoDeviceObject,
                                 BusContext,
                                 &translator);

        if (NT_ERROR(nts)) {
            level_0->TotalTtBandwidth = 0;
        } else if (translator) {
            ASSERT_TT(translator);
            level_0->TotalTtBandwidth = translator->TotalBusBandwidth * 32;

            USBPORT_BusContextReleaseTt(fdoDeviceObject,
                                        BusContext,
                                        translator);
        }
    }
*/

    //
    // Add up bandwidth consumed
    //

    pipeListEntry = &deviceHandle->PipeHandleList;

    if (!IsListEmpty(pipeListEntry)) {
        pipeListEntry = deviceHandle->PipeHandleList.Flink;
    }

    while (pipeListEntry != &deviceHandle->PipeHandleList) {

        PUSBD_PIPE_HANDLE_I pipeHandle;
        PHCD_ENDPOINT endpoint;

        pipeHandle = (PUSBD_PIPE_HANDLE_I) CONTAINING_RECORD(
                pipeListEntry,
                struct _USBD_PIPE_HANDLE_I,
                ListEntry);

        ASSERT_PIPE_HANDLE(pipeHandle);

        if(pipeHandle->EpHandle == USB_ZERO_BW_ENDP_HANDLE ||
           pipeHandle->EpHandle == NULL){
            pipeListEntry = pipeHandle->ListEntry.Flink;
            continue;
        }

        // endpoint is pinned by pipe handle
        endpoint = USBPORT_EndpointFromHandle(pipeHandle->EpHandle);

        if (endpoint != NULL &&
            endpoint != USB_ZERO_BW_ENDP_HANDLE) {

            if(endpoint->Parameters.DeviceSpeed == HighSpeed){
                //
                // High speed endpoint.  We get our data direct from the usb2lib context
                //

                if(endpoint->Parameters.TransferType == Isochronous ||
                   endpoint->Parameters.TransferType == Interrupt){


                    //
                    // First we will convert bytes/microframe to bits/microframe.
                    //
                    bw = MPx_QueryEpBandwidthData(fdoDevExt, endpoint);
                    bw = bw * 8;

                    //
                    // Determine the period of the endpoint
                    //

                    USBPORT_GET_BIT_SET(endpoint->Parameters.Period, n);
                    USBPORT_ASSERT(n<6);

                    //
                    // To keep a common interface with the full/low speed bw numbers, we will report
                    // our bandwidth in bits/32ms(32 frames).
                    // An interrupt or iso transaction can occur every 1,2,4,8,16 or 32 microframes.
                    // Since there are 8 microframes/frame we need to convert using the following
                    // formula:
                    //
                    // Period   Transfers/32 frames(256 microframes)    n
                    //
                    // 1        256                                     0
                    // 2        128                                     1
                    // 4        64                                      2
                    // 8        32                                      3
                    // 16       16                                      4
                    // 32       8                                       5
                    //
                    if(n < 6){
                        bw = bw * highSpeedTransfersIn32ms[n];
                    }
                } else {
                    //
                    // Control or bulk endpoint
                    //
                    bw = 0;
                }
            } else{
                //
                // Full/Low speed endpoint.  All numbers are reported in bits/32 frames.
                //

                if(endpoint->Parameters.TransferType == Isochronous){
                    bw = endpoint->Parameters.Bandwidth * 32;
                }

                else if(endpoint->Parameters.TransferType == Interrupt){

                    USBPORT_GET_BIT_SET(endpoint->Parameters.Period, n);
                    USBPORT_ASSERT(n<6);

                    //
                    // To avoid rounding errors, we report all bandwidth in bits/32 frames(32ms).
                    // An interrupt or iso transaction can occur every 1,2,4,8,16 or 32 microframes.
                    // We need to convert using the following
                    // formula:
                    //
                    // Period   Transfers/32 frames     n
                    //
                    // 1        32                      0
                    // 2        16                      1
                    // 4        8                       2
                    // 8        4                       3
                    // 16       2                       4
                    // 32       1                       5
                    //
                    if(n<6){
                        bw = endpoint->Parameters.Bandwidth  * fullSpeedTransfersIn32ms[n];
                    }
                }
                else{
                    //
                    // Control or bulk endpoint
                    //
                    bw = 0;
                }
            }

            //
            // Place the bw into the right entry
            //
            switch(endpoint->Parameters.TransferType){
            case Interrupt:
                USBPORT_ASSERT(n<6);
                if(n<6){
                    level_0->AllocedInterrupt[n] += bw;
                }
                break;
            case Isochronous:
                level_0->AllocedIso += bw;
                break;
            default:
                break;
            }
        }
        pipeListEntry = pipeHandle->ListEntry.Flink;
    }


    *LengthOfDataCopied = level_0->ActualLength;

    // dump the level data returned
    DbgTrace((UsbpDebugTrace, "  USBD level 0 Device Perf Info:\n"));
    DbgTrace((UsbpDebugTrace, "  InformationLevel %d\n",
        level_0->InformationLevel));
    DbgTrace((UsbpDebugTrace, "  ActualLength %d\n",
        level_0->ActualLength));

    USBPORT_DereferenceDeviceHandle(fdoDeviceObject,
                                    deviceHandle,
                                    BusContext,  // ReferenceObj
                                    DEVH_BUSIF_GDPI_TAG,
                                    FALSE);      // PendingFlag

    UNLOCK_DEVICE_CONFIGS(deviceHandle, fdoDeviceObject);

    return STATUS_SUCCESS;
}


NTSTATUS
USBPORTBUSIF_BusQueryDeviceInformation(
    PVOID BusContext,
    PUSB_DEVICE_HANDLE DeviceHandle,
    PVOID DeviceInformationBuffer,
    ULONG DeviceInformationBufferLength,
    PULONG LengthOfDataCopied
    )
/*++

Routine Description:

Service exported for use by the hub driver.  This api returns various
information about a USB devices attached to the system.

    IRQL = PASSIVE_LEVEL

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - Handle for the device you want information about. This is the
handle returned by CreateUsbDevice or returned in IOCTL api
IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE.

DeviceInformationBuffer - buffer for returning device information.  The type
of data returned is defined by the value passed in the InformationLevel field
of the buffer passed in.
Levels Supported:
_ USB_DEVICE_INFORMATION_0, level = 0

DeviceInformationBufferLength - length of DeviceInformationBuffer in bytes

LengthOfDataReturned - length of data copied into DeviceInformationBuffer.

* currently only level 0 is defined
Level 0 information:
typedef struct _USB_DEVICE_INFORMATION_0 {
    ULONG InformationLevel;
    ULONG ActualLength;

    ULONG PortNumber;
    USB_DEVICE_DESCRIPTOR DeviceDescriptor;
    UCHAR DD_pad[2];
    UCHAR CurrentConfigurationValue;
    UCHAR ReservedMBZ;
    USHORT DeviceAddress;
    ULONG HubAddress;
    USB_DEVICE_SPEED DeviceSpeed;
    USB_DEVICE_TYPE DeviceType;
    ULONG NumberOfOpenPipes;
    USB_PIPE_INFORMATION_0 PipeList[1];
}

typedef struct _USB_PIPE_INFORMATION_0 {
    USB_ENDPOINT_DESCRIPTOR EndpointDescriptor;
    UCHAR ED_Pad[1];
    ULONG ScheduleOffset;
}

-DeviceSpeed
The current operating speed of the device - high, low or full.
-DeviceType
Indicates USB specification version supported by the device, USB1.1 or
USB 2.0.  This field corresponds to bcdUSB field in the device descriptor.


Return Value:

    NT status code.

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_OBJECT fdoDeviceObject;
    PDEVICE_EXTENSION rhDevExt;
    ULONG need;
    PUSBD_CONFIG_HANDLE cfgHandle;
    ULONG i, j;
    PUSB_DEVICE_INFORMATION_0 level_0 = DeviceInformationBuffer;
    PUSB_LEVEL_INFORMATION levelInfo = DeviceInformationBuffer;
    ULONG numberOfPipes = 0;
    PUSBD_DEVICE_HANDLE deviceHandle = DeviceHandle;
    BOOLEAN ref_success;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;

    *LengthOfDataCopied = 0;

    if (DeviceInformationBufferLength < sizeof(*levelInfo)) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    if (levelInfo->InformationLevel > 0) {
        // only support level 0
        return STATUS_NOT_SUPPORTED;
    }

    //
    // This will lock out config changes across the call.  Note that as
    // currently implemented the LOCK_DEVICE_CONFIGS() macro will not
    // access the deviceHandle so it does not matter whether or not the
    // deviceHandle if valid at this point.
    //
    LOCK_DEVICE_CONFIGS(deviceHandle, fdoDeviceObject);

    //
    // Validate that the deviceHandle is valid and take a reference on
    // it to make sure it is not deleted while it is being accessed by
    // this routine.
    //
    ref_success = USBPORT_NeoValidDeviceHandle(fdoDeviceObject,
                                               deviceHandle,
                                               Devh_Valid,
                                               BusContext,  // ReferenceObj
                                               NULL,        // unused parameter
                                               DEVH_BUSIF_BQDI_TAG,
                                               FALSE);      // PendingFlag

    if (ref_success == FALSE) {
        // this is most likely a bug in the hub
        // driver
        TEST_TRAP();

        UNLOCK_DEVICE_CONFIGS(deviceHandle, fdoDeviceObject);
        // chances are that the device handle is bad becuse the
        // device is gone.
        return STATUS_DEVICE_NOT_CONNECTED;
    }

    // figure out how much room we need
    cfgHandle = deviceHandle->ConfigurationHandle;
    if (cfgHandle != NULL) {

        PLIST_ENTRY listEntry;
        PUSBD_INTERFACE_HANDLE_I iHandle;

         // walk the list
        GET_HEAD_LIST(cfgHandle->InterfaceHandleList, listEntry);

        while (listEntry &&
               listEntry != &cfgHandle->InterfaceHandleList) {

            // extract the handle from this entry
            iHandle = (PUSBD_INTERFACE_HANDLE_I) CONTAINING_RECORD(
                        listEntry,
                        struct _USBD_INTERFACE_HANDLE_I,
                        InterfaceLink);

            ASSERT_INTERFACE(iHandle);
            numberOfPipes += iHandle->InterfaceDescriptor.bNumEndpoints;

            listEntry = iHandle->InterfaceLink.Flink;
        }

    }

    need = (numberOfPipes-1) * sizeof(USB_PIPE_INFORMATION_0) +
            sizeof(USB_DEVICE_INFORMATION_0);


    if (DeviceInformationBufferLength < need) {
        // report how much space if possible
        levelInfo->ActualLength = need;
        *LengthOfDataCopied = sizeof(*levelInfo);

        USBPORT_DereferenceDeviceHandle(fdoDeviceObject,
                                        deviceHandle,
                                        BusContext,  // ReferenceObj
                                        DEVH_BUSIF_BQDI_TAG,
                                        FALSE);      // PendingFlag

        UNLOCK_DEVICE_CONFIGS(deviceHandle, fdoDeviceObject);

        return STATUS_BUFFER_TOO_SMALL;
    }

    RtlZeroMemory(level_0, need);

    //
    // enough room, fill in the buffer
    //

    level_0->InformationLevel = 0;
    level_0->ActualLength = need;
    level_0->DeviceAddress = deviceHandle->DeviceAddress;
    level_0->DeviceDescriptor = deviceHandle->DeviceDescriptor;
    level_0->DeviceSpeed = deviceHandle->DeviceSpeed;
    RtlCopyMemory(&level_0->DeviceDescriptor, &deviceHandle->DeviceDescriptor,
        sizeof(level_0->DeviceDescriptor));

    // assume USB 1.1
    level_0->DeviceType = Usb11Device;

    if (deviceHandle->DeviceDescriptor.bcdUSB >= 0x0200) {
        level_0->DeviceType = Usb20Device;
    }

    level_0->NumberOfOpenPipes = numberOfPipes;
    // default to 'unconfigured'
    level_0->CurrentConfigurationValue = 0;
    // get the pipe information
    if (cfgHandle) {

        PLIST_ENTRY listEntry;
        PUSBD_INTERFACE_HANDLE_I iHandle;

        level_0->CurrentConfigurationValue =
            cfgHandle->ConfigurationDescriptor->bConfigurationValue;

         // walk the list
        GET_HEAD_LIST(cfgHandle->InterfaceHandleList, listEntry);

        j = 0;
        while (listEntry &&
               listEntry != &cfgHandle->InterfaceHandleList) {

            // extract the handle from this entry
            iHandle = (PUSBD_INTERFACE_HANDLE_I) CONTAINING_RECORD(
                        listEntry,
                        struct _USBD_INTERFACE_HANDLE_I,
                        InterfaceLink);

            ASSERT_INTERFACE(iHandle);
            for (i=0; i<iHandle->InterfaceDescriptor.bNumEndpoints; i++) {
                __analysis_assume(DeviceInformationBufferLength > j * sizeof(USB_PIPE_INFORMATION_0) + sizeof(USB_DEVICE_INFORMATION_0));
                if (TEST_FLAG(iHandle->PipeHandle[i].PipeStateFlags, USBPORT_PIPE_ZERO_BW) ||
                    iHandle->PipeHandle[i].EpHandle == USB_ZERO_BW_ENDP_HANDLE) {
                    level_0->PipeList[j].ScheduleOffset = 1;
                } else {
                    PHCD_ENDPOINT endpoint;

                    endpoint = USBPORT_EndpointFromHandle(iHandle->PipeHandle[i].EpHandle);

                    level_0->PipeList[j].ScheduleOffset =
                       endpoint->Parameters.ScheduleOffset;
                }

                RtlCopyMemory(&level_0->PipeList[j].EndpointDescriptor,
                              &iHandle->PipeHandle[i].EndpointDescriptor,
                              sizeof(USB_ENDPOINT_DESCRIPTOR));
                j++;
            }
            listEntry = iHandle->InterfaceLink.Flink;
        }
    }

    *LengthOfDataCopied = need;

    // dump the level data returned
    DbgTrace((UsbpDebugTrace, "  USBD level 0 Device Information:\n"));
    DbgTrace((UsbpDebugTrace, "  InformationLevel %d\n",
        level_0->InformationLevel));
    DbgTrace((UsbpDebugTrace, "  ActualLength %d\n",
        level_0->ActualLength));
    DbgTrace((UsbpDebugTrace, "  DeviceSpeed %d\n",
        level_0->DeviceSpeed));
    DbgTrace((UsbpDebugTrace, "  DeviceType %d\n",
        level_0->DeviceType));
    DbgTrace((UsbpDebugTrace, "  PortNumber %d\n",
        level_0->PortNumber));
    DbgTrace((UsbpDebugTrace, "  CurrentConfigurationValue %d\n",
        level_0->CurrentConfigurationValue));
    DbgTrace((UsbpDebugTrace, "  DeviceAddress %d\n",
        level_0->DeviceAddress));
    DbgTrace((UsbpDebugTrace, "  NumberOfOpenPipes %d\n",
        level_0->NumberOfOpenPipes));

    for (i=0; i<level_0->NumberOfOpenPipes; i++) {
        DbgTrace((UsbpDebugTrace, "  ScheduleOffset[%d] %d\n", i,
            level_0->PipeList[i].ScheduleOffset));
        DbgTrace((UsbpDebugTrace, "  MaxPacket %d\n",
            level_0->PipeList[i].EndpointDescriptor.wMaxPacketSize));
        DbgTrace((UsbpDebugTrace, "  Interval %d\n",
            level_0->PipeList[i].EndpointDescriptor.bInterval));
    }

    USBPORT_DereferenceDeviceHandle(fdoDeviceObject,
                                    deviceHandle,
                                    BusContext,  // ReferenceObj
                                    DEVH_BUSIF_BQDI_TAG,
                                    FALSE);      // PendingFlag

    UNLOCK_DEVICE_CONFIGS(deviceHandle, fdoDeviceObject);

    return STATUS_SUCCESS;
}


PVOID
USBPORTBUSIF_GetDeviceBusContext(
    IN PVOID HubBusContext,
    IN PVOID DeviceHandle
    )
/*++

Routine Description:

Service exported for use by the hub driver.  This api returns various information about a USB devices attached
to the system.

This API returns the device relative buscontext for the USBD bus interface given a device handle.  The hub driver
uses this parameter in calls to the usbd bus interface.

If a bus context exists for the device then it is refed used. otherwise one is created and refed. The hub
driver must call UsbdBusInterfaceDereference in order to release the context.

IRQL = PASSIVE_LEVEL

Arguments:

HubBusContext -

DeviceHandle - Handle for the device you want information about. This is the handle returned by CreateUsbDevice
or returned in IOCTL api IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE.

Arguments:

Return Value:

    the device relative bus context

--*/
{
    PBUS_CONTEXT deviceBusContext;

    ASSERT_PASSIVE();

    USBPORT_GetBusContext(&deviceBusContext,
                          USBPORT_PdoFromBusContext(HubBusContext),
                          DeviceHandle);

    if (deviceBusContext) {
         ASSERT_BUS_CONTEXT(deviceBusContext);

         // ref the device context here
         USBPORT_BusContextEvent(deviceBusContext, EvBc_Usbd_Ref);
    }

    return deviceBusContext;
}


BOOLEAN
USBPORTBUSIF_UsbdIsDeviceHighSpeed(
    __in PVOID BusContext
    )
/*++

Routine Description:

    return the speed a given device is operating at

Arguments:

Return Value:

    TRUE if the device is operating at hgh speed.

--*/
{
    PBUS_CONTEXT busCon = (PBUS_CONTEXT)BusContext;
    ASSERT_BUS_CONTEXT(busCon);
#pragma prefast(suppress:__WARNING_REDUNDANT_POINTER_TEST __WARNING_REDUNDANT_POINTER_TEST_FAR_EVIDENCE, "busCon cannot be NULL; see ASSERT_BUS_CONTEXT")
    if ((NULL != busCon) &&
        (NULL != busCon->DeviceHandle) &&
        (UsbHighSpeed == busCon->DeviceHandle->DeviceSpeed)) {
        return TRUE;
    }
    else {
        return FALSE;
    }
}


NTSTATUS
USBPORTBUSIF_UsbdBusQueryBusInformation(
    __in_opt PVOID BusContext,
    __in ULONG Level,
    PVOID BusInformationBuffer,
    __out_opt PULONG BusInformationBufferLength,
    __out_opt PULONG BusInformationActulaLength
    )
/*++

Routine Description:

Arguments:

    BusContext - context structure returned in the businterface query.  This
                structure contins a pointer to the controller, tt and device.




Return Value:

    NT status code.

--*/
{
    NTSTATUS ntStatus = STATUS_NOT_SUPPORTED;
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_OBJECT fdoDeviceObject;
    PDEVICE_EXTENSION devExt, rhDevExt;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;

    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    switch (Level) {
    case 0:
        {
        PUSB_BUS_INFORMATION_LEVEL_0 level_0;

        level_0 =  BusInformationBuffer;
        LOGENTRY(fdoDeviceObject, LOG_MISC, 'lev1', 0, level_0, 0);

        if (BusInformationActulaLength != NULL) {
            *BusInformationActulaLength = sizeof(*level_0);
        }

        if (*BusInformationBufferLength >= sizeof(*level_0)) {
            *BusInformationBufferLength = sizeof(*level_0);

            level_0->TotalBandwidth =
                USBPORT_ComputeTotalBandwidth(fdoDeviceObject,
                                              BusContext);
            level_0->ConsumedBandwidth =
                USBPORT_ComputeAllocatedBandwidth(fdoDeviceObject,
                                                  BusContext);

            ntStatus = STATUS_SUCCESS;
        } else {
            ntStatus = STATUS_BUFFER_TOO_SMALL;
        }
        }
        break;

    case 1:
        {
        PUSB_BUS_INFORMATION_LEVEL_1 level_1;
        ULONG need;

        level_1 =  BusInformationBuffer;
        LOGENTRY(fdoDeviceObject, LOG_MISC, 'lev1', 0, level_1, 0);

        need = sizeof(*level_1) + devExt->SymbolicLinkName.Length;

        if (BusInformationActulaLength != NULL) {
            *BusInformationActulaLength = need;
        }

        if (*BusInformationBufferLength >= need) {
            *BusInformationBufferLength = need;

            level_1->TotalBandwidth =
                USBPORT_ComputeTotalBandwidth(fdoDeviceObject,
                                              BusContext);
            level_1->ConsumedBandwidth =
                USBPORT_ComputeAllocatedBandwidth(fdoDeviceObject,
                                                  BusContext);
            level_1->ControllerNameLength =
                devExt->SymbolicLinkName.Length;

            RtlCopyMemory(level_1->ControllerNameUnicodeString,
                          devExt->SymbolicLinkName.Buffer,
                          level_1->ControllerNameLength);

            ntStatus = STATUS_SUCCESS;
        } else {
            ntStatus = STATUS_BUFFER_TOO_SMALL;
        }
        }
    }

    return ntStatus;
}


NTSTATUS
USBPORT_GetBusInterfaceUSBDI(
    PDEVICE_OBJECT FdoDeviceObject,
    PDEVICE_OBJECT PdoDeviceObject,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PIRP Irp
    )
/*++

Routine Description:

Arguments:

Return Value:

    NT status code.

--*/
{
    PIO_STACK_LOCATION irpStack;
    NTSTATUS ntStatus;
    USHORT requestedSize, requestedVersion;
    PBUS_CONTEXT busContext = NULL;
    PDEVICE_EXTENSION rhDevExt = NULL;
    BOOLEAN ref_success;

    GET_DEVICE_EXT(rhDevExt, PdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    if (NULL == DeviceHandle) {

        //
        // A NULL device handle is a special case for the Root Hub
        // device handle.
        //
        DeviceHandle = rhDevExt->Pdo.RootHubDeviceHandle;
        ref_success = FALSE;

    } else {

        //
        // Validate that the deviceHandle is valid and take a reference on
        // it to make sure it is not deleted while it is being accessed by
        // this routine.
        //
        ref_success = USBPORT_NeoValidDeviceHandle(FdoDeviceObject,
                                                   DeviceHandle,
                                                   Devh_Valid,
                                                   Irp,         // ReferenceObj
                                                   NULL,        // unused parameter
                                                   DEVH_GBIU_TAG,
                                                   FALSE);      // PendingFlag

        if (ref_success == FALSE) {
            ntStatus = STATUS_INVALID_PARAMETER;
            return ntStatus;
        }
    }

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'gbi1', FdoDeviceObject, DeviceHandle, Irp);

    ASSERT_DEVICE_HANDLE(DeviceHandle);

    irpStack = IoGetCurrentIrpStackLocation(Irp);

    requestedSize = irpStack->Parameters.QueryInterface.Size;
    requestedVersion = irpStack->Parameters.QueryInterface.Version;

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'gbi2', FdoDeviceObject, requestedSize, requestedVersion);
    ntStatus = USBPORT_GetBusContext(&busContext, PdoDeviceObject, DeviceHandle);


    if (!NT_SUCCESS(ntStatus)) {

        if (ref_success != FALSE) {

            USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                            DeviceHandle,
                                            Irp,        // ReferenceObj
                                            DEVH_GBIU_TAG,
                                            FALSE);     // PendingFlag
        }

        return ntStatus;
    }

    LOGENTRY(FdoDeviceObject, LOG_MISC, 'gbi3', FdoDeviceObject, busContext, 0);
    ASSERT_BUS_CONTEXT(busContext);

    // assume success
    ntStatus = STATUS_SUCCESS;

    if (requestedVersion >= USB_BUSIF_USBDI_VERSION_0) {

        PUSB_BUS_INTERFACE_USBDI_V0 busInterface0;

        busInterface0 = (PUSB_BUS_INTERFACE_USBDI_V0)
            irpStack->Parameters.QueryInterface.Interface;

        busInterface0->BusContext =
            busContext;
        busInterface0->InterfaceReference =
            USBPORTBUSIF_UsbdBusInterfaceReference;
        busInterface0->InterfaceDereference =
            USBPORTBUSIF_UsbdBusInterfaceDereference;
        busInterface0->GetUSBDIVersion =
            USBPORTBUSIF_UsbdBusGetUSBDIVersion;
        busInterface0->QueryBusTime =
            USBPORTBUSIF_UsbdBusQueryBusTime;
        busInterface0->SubmitIsoOutUrb =
            USBPORTBUSIF_UsbdBusSubmitIsoOutUrb;
        busInterface0->QueryBusInformation =
            USBPORTBUSIF_UsbdBusQueryBusInformation;

        busInterface0->Size = sizeof(USB_BUS_INTERFACE_USBDI_V0);
        busInterface0->Version = USB_BUSIF_USBDI_VERSION_0;
    }


    if (requestedVersion >= USB_BUSIF_USBDI_VERSION_1) {

        PUSB_BUS_INTERFACE_USBDI_V1 busInterface1;

        busInterface1 = (PUSB_BUS_INTERFACE_USBDI_V1)
            irpStack->Parameters.QueryInterface.Interface;

        // add version 1 extensions
        busInterface1->IsDeviceHighSpeed =
            USBPORTBUSIF_UsbdIsDeviceHighSpeed;

        busInterface1->Size = sizeof(USB_BUS_INTERFACE_USBDI_V1);
        busInterface1->Version = USB_BUSIF_USBDI_VERSION_1;
    }

     if (requestedVersion >= USB_BUSIF_USBDI_VERSION_2) {

        PUSB_BUS_INTERFACE_USBDI_V2 busInterface2;

        busInterface2 = (PUSB_BUS_INTERFACE_USBDI_V2)
            irpStack->Parameters.QueryInterface.Interface;

        // add version 2 extensions
        busInterface2->EnumLogEntry =
            USBPORTBUSIF_UsbdBusEnumLogEntry;

        busInterface2->Size = sizeof(USB_BUS_INTERFACE_USBDI_V2);
        busInterface2->Version = USB_BUSIF_USBDI_VERSION_2;
    }


//#ifdef VISTA_RTM

    if (requestedVersion >= USB_BUSIF_USBDI_VERSION_3) {

        PUSB_BUS_INTERFACE_USBDI_V3 busInterface3;

        busInterface3 = (PUSB_BUS_INTERFACE_USBDI_V3)
            irpStack->Parameters.QueryInterface.Interface;


        // add version 3 extensions
        busInterface3->QueryBusTimeEx =
            USBPORTBUSIF_UsbdBusQueryBusTimeEx;

        busInterface3->QueryControllerType =
            USBPORTBUSIF_UsbdQueryControllerType;

        busInterface3->Size = sizeof(USB_BUS_INTERFACE_USBDI_V3);
        busInterface3->Version = USB_BUSIF_USBDI_VERSION_3;
    }

//#endif //VISTA_RTM

    if (NT_SUCCESS(ntStatus)) {
        USBPORTBUSIF_UsbdBusInterfaceReference(busContext);
    }

    if (ref_success != FALSE) {

        USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                        DeviceHandle,
                                        Irp,        // ReferenceObj
                                        DEVH_GBIU_TAG,
                                        FALSE);     // PendingFlag
    }

    return ntStatus;
}


VOID
USBPORTBUSIF_UsbdBusGetUSBDIVersion(
    __in_opt PVOID BusContext,
    __out_opt PUSBD_VERSION_INFORMATION VersionInformation,
    __out_opt PULONG HcdCapabilities
    )
/*++

Routine Description:

    returns USBDI and USB version information.

Arguments:

    BusContext - bus context for a USB device.

    VersionInformation - buffer to be filled in with the appropraite version information

    HcdCapabilities - pointer to ulong that will be set withe the capailities flags for
            the host controller where the device resides.


Return Value:

    NONE.

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_OBJECT fdoDeviceObject;
    PDEVICE_EXTENSION devExt, rhDevExt;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;

    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    if (HcdCapabilities != NULL) {
        *HcdCapabilities = 0;
    }

    if (VersionInformation != NULL) {
        VersionInformation->USBDI_Version = USBDI_VERSION;

        if (USBPORT_IS_USB20(devExt)) {
            VersionInformation->Supported_USB_Version = 0x0200;
        } else {
            VersionInformation->Supported_USB_Version = 0x0110;
        }
    }

//    if (deviceExtensionUsbd->HcdSubmitIsoUrb != NULL) {
//        *HcdCapabilities = USB_HCD_CAPS_SUPPORTS_RT_THREADS;
//    }
}


NTSTATUS
USBPORTBUSIF_UsbdBusQueryBusTime(
    __in_opt PVOID BusContext,
    __out_opt PULONG CurrentFrame
    )
/*++

Routine Description:

    Returns the value of the current USB frame counter or 'clock'.

Arguments:

    BusContext - bus context for a USB device.

    CurrentFrame - pointer to receive the current frame number.

Return Value:

    NT status code.

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_OBJECT fdoDeviceObject;
    PDEVICE_EXTENSION devExt, rhDevExt;
    NTSTATUS nts;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;

    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    if (CurrentFrame == NULL) {
        nts = STATUS_INVALID_PARAMETER;
    } else {
        *CurrentFrame = MPf_Get32BitFrameNumber(devExt);

        LOGENTRY(fdoDeviceObject, LOG_MISC, 'biCF', 0, CurrentFrame, *CurrentFrame);
        nts = STATUS_SUCCESS;
    }

    return nts;
}

//#ifdef VISTA_RTM

NTSTATUS
USBPORTBUSIF_UsbdBusQueryBusTimeEx(
    __in_opt PVOID BusContext,
    __out_opt PULONG CurrentMicroFrame
    )
/*++

Routine Description:

    Returns the value of the current USB frame counter or 'clock'. This routine returns the microframe count in the
    lowest three bits.  If called on a full speed bus the API returns STATUS_NOT_SUPPORTED.

Arguments:

    BusContext - bus context for a USB device.

    CurrentMicroFrame - pointer to receive the current frame number.

Return Value:

    NT status code.

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_OBJECT fdoDeviceObject;
    PDEVICE_EXTENSION devExt, rhDevExt;
    NTSTATUS nts;
    ULONG current_hs_frame;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;

    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    do {
        if (CurrentMicroFrame == NULL) {
            nts = STATUS_INVALID_PARAMETER;
            break;
        }

        if (!USBPORT_IS_USB20(devExt)) {
            nts = STATUS_NOT_SUPPORTED;
            break;
        }

        current_hs_frame = MPf_Get32BitMicroFrameNumber(devExt);

        *CurrentMicroFrame = current_hs_frame;
        LOGENTRY(fdoDeviceObject, LOG_MISC, 'biCF', 0, CurrentMicroFrame, current_hs_frame);

        nts = STATUS_SUCCESS;

    } WHILE (0);

    return nts;
}


NTSTATUS
USBPORTBUSIF_UsbdQueryControllerType(
    __in_opt PVOID BusContext,
    __out_opt PULONG HcdiOptionFlags,
    __out_opt PUSHORT PciVendorId,
    __out_opt PUSHORT PciDeviceId,
    __out_opt PUCHAR PciClass,
    __out_opt PUCHAR PciSubClass,
    __out_opt PUCHAR PciRevisionId,
    __out_opt PUCHAR PciProgIf
    )
/*++

Routine Description:

    Returns information about the controller this device is connected to.

Arguments:

    BusContext - bus context for a USB device.

    HcdiOptionFlags - miniport option flags set when controller registers with usbport.

    PciVendorId - pointer to receive the pci info for controller.

    PciDeviceId -

    PciClass - typically PCI_CLASS_SERIAL_BUS_CTLR

    PciSubClass - typically PCI_SUBCLASS_SB_USB

    PciRevisionId -

    PciProgIf - 0x00 = UHCI, 0x10 = OHCI, 0x20 EHCI.


Return Value:

    NT status code.

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_OBJECT fdoDeviceObject;
    PDEVICE_EXTENSION devExt, rhDevExt;
    NTSTATUS nts;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;

    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    if (HcdiOptionFlags) {
        *HcdiOptionFlags = devExt->Fdo.HcOptionFlags;
    }

    if (PciVendorId) {
        *PciVendorId = devExt->Fdo.PciVendorId;
    }

    if (PciDeviceId) {
        *PciDeviceId = devExt->Fdo.PciDeviceId;
    }

    if (PciClass) {
        *PciClass = devExt->Fdo.PciClass;
    }

    if (PciSubClass) {
        *PciSubClass = devExt->Fdo.PciSubClass;
    }

    if (PciRevisionId) {
        *PciRevisionId = devExt->Fdo.PciRevisionId;
    }

    if (PciProgIf) {
        *PciProgIf = devExt->Fdo.PciProgIf;
    }

    nts = STATUS_SUCCESS;

    return nts;
}

//#endif //VISTA_RTM

NTSTATUS
USBPORTBUSIF_UsbdBusEnumLogEntry(
    __in_opt PVOID BusContext,
    __in ULONG DriverTag,
    __in ULONG EnumTag,
    __in ULONG P1,
    __in ULONG P2
    )
/*++

Routine Description:

    Used to be used to record enumeration failure information. This is now recorded in USBHUB.SYS.

Arguments:

    n/a

Return Value:

    NT status code.

--*/
{
   // this is currently a nop

    return STATUS_SUCCESS;
}


NTSTATUS
USBPORTBUSIF_UsbdBusSubmitIsoOutUrb(
    __in_opt PVOID BusContext,
    __in_opt PURB Urb
    )
/*++

Routine Description:

Arguments:

Return Value:

    NT status code.

--*/
{
    TEST_TRAP();

    return STATUS_NOT_SUPPORTED;
}


NTSTATUS
USBPORTBUSIF_GetControllerInformation(
    PVOID BusContext,
    PVOID ControllerInformationBuffer,
    ULONG ControllerInformationBufferLength,
    PULONG LengthOfDataCopied
    )
/*++

Routine Description:

Arguments:

Return Value:

    NT status code.

--*/
{
    ULONG len;
    PUSB_CONTROLLER_INFORMATION_0 level_0 = ControllerInformationBuffer;
    PUSB_LEVEL_INFORMATION levelInfo = ControllerInformationBuffer;
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_OBJECT fdoDeviceObject;
    PDEVICE_EXTENSION devExt, rhDevExt;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;

    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    *LengthOfDataCopied = 0;

    if (ControllerInformationBufferLength < sizeof(*levelInfo)) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    *LengthOfDataCopied = sizeof(*levelInfo);

    switch (levelInfo->InformationLevel) {
    // level 0
    case 0:
        len = sizeof(*level_0);
        level_0->ActualLength = len;

        if (ControllerInformationBufferLength >= len) {
            *LengthOfDataCopied = len;

            switch(devExt->Fdo.SSP_Support) {
            case SSPsupport_RegEnabled:
                level_0->SelectiveSuspendEnabled = TRUE;
                break;
            case SSPsupport_RegDisabled:
            case SSPsupport_GlobalDisabled:
            case SSPsupport_HcDisabled:
                level_0->SelectiveSuspendEnabled = FALSE;
                break;
            default:
                USBPORT_ASSERTMSG( "Unexpected InformationLevel" , FALSE);
                break;
            }
        }

        *LengthOfDataCopied = sizeof(*level_0);
        if (ControllerInformationBufferLength < *LengthOfDataCopied) {
            return STATUS_BUFFER_OVERFLOW;
        } else {
            return STATUS_SUCCESS;
        }

    default:
        return STATUS_NOT_SUPPORTED;
    }

    // Unreachable section
}


NTSTATUS
USBPORTBUSIF_ControllerSelectiveSuspend(
    PVOID BusContext,
    BOOLEAN Enable
    )
/*++

Routine Description:

Arguments:

Return Value:

    NT status code.

--*/
{
    // Hub selective suspend makes this function obsolete
    return STATUS_NOT_SUPPORTED;
}


NTSTATUS
USBPORTBUSIF_GetExtendedHubInformation(
    PVOID BusContext,
    PDEVICE_OBJECT HubPhysicalDeviceObject,
    PVOID HubInformationBuffer,
    ULONG HubInformationBufferLength,
    PULONG LengthOfDataCopied
    )
/*++

Routine Description:

In order to simplify the implementation of usbport.sys this API is no longer
supported or used in version 6 and beyond.  The usbhub.sys driver is
responsible for retrieving this information internally.

Arguments:

Return Value:

    NT status code.

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_OBJECT fdoDeviceObject;
    PDEVICE_EXTENSION devExt, rhDevExt;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;

    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    return STATUS_NOT_SUPPORTED;

}


NTSTATUS
USBPORT_IoctlGetHubName(
    PDEVICE_OBJECT PdoDeviceObject,
    PDEVICE_OBJECT FdoDeviceObject,
    PIRP Irp
    )
/*++

Routine Description:

    The IOCTL version of USBPORTBUSIF_GetRootHubSymbolicName

    This API returns the symbolic link name for the PDO if the PDO is for a usb
    hub. Otherwise a null string is returned.

This API returns the UNICODE symbolic name for the PDO if the PDO is for a
usbhub, otherwise a NULL string is returned.  The symbolic name can be used
to retrieve additional information about the hub through user mode ioctl apis
and WMI calls.

Parameters:
ioStackLocation->Parameters.DeviceIoControl.OutputBufferLength
Length of buffer passed bytes.

Irp->AssociatedIrp.SystemBuffer
A pointer to a structure (USB_HUB_NAME) to receive the symbolic name.

 USB_BUS_NOTIFICATION.
ActualLength - The structure size in bytes necessary to hold the NULL
   terminated symbolic link name.  This includes the entire structure, not
   just the name.

  HubName - The UNICODE NULL terminated symbolic link name of the external
hub attached to the port.  If there is no external hub attached to the port a
single NULL is returned.

typedef struct _USB_HUB_NAME {
   ULONG ActualLength;
   WCHAR HubName[1];
} USB_HUB_NAME, *PUSB_HUB_NAME;

IRQL: Any

Return Value:

    nt status code

--*/
{
    NTSTATUS ntStatus;
    PIO_STACK_LOCATION ioStack;
    PUSB_HUB_NAME userBuffer;
    ULONG userBufferLength;
    UNICODE_STRING hubNameUnicodeString;
    ULONG length;
    ULONG returnLength = 0;
    PUSB_IOREQUEST_CONTEXT usbIoRequest;

    ioStack = IoGetCurrentIrpStackLocation(Irp);

    userBufferLength = ioStack->Parameters.DeviceIoControl.OutputBufferLength;
    userBuffer = (PUSB_HUB_NAME) Irp->AssociatedIrp.SystemBuffer;

    if (userBufferLength < sizeof(USB_HUB_NAME)) {
        ntStatus = STATUS_BUFFER_TOO_SMALL;
        return ntStatus;
    }

    ntStatus = USBPORT_GetSymbolicName(FdoDeviceObject,
                                       PdoDeviceObject,
                                       &hubNameUnicodeString);

    if (NT_SUCCESS(ntStatus)) {
        length = hubNameUnicodeString.Length;

        if (userBufferLength >= length +
            sizeof(USB_HUB_NAME)) {
            RtlCopyMemory(&userBuffer->HubName[0],
                          hubNameUnicodeString.Buffer,
                          length);

            returnLength = length + sizeof(USB_HUB_NAME);
            userBuffer->ActualLength = (ULONG)Irp->IoStatus.Information;
            ntStatus = STATUS_SUCCESS;

        } else {

            // Output buffer is too small to hold the entire
            // string.  Return just the length needed to hold
            // the entire string.

            userBuffer->ActualLength =
                length + sizeof(USB_HUB_NAME);

            userBuffer->HubName[0] = (WCHAR)0;

            returnLength =  sizeof(USB_HUB_NAME);
            ntStatus = STATUS_SUCCESS;

        }

        RtlFreeUnicodeString(&hubNameUnicodeString);

    }

    usbIoRequest = USBPORT_Core_GetIoRequestFromObject(PdoDeviceObject, Irp);

    USBPORT_Core_IoCompleteIrp(PdoDeviceObject, Irp, NULL, ntStatus, returnLength);

    USBPORT_Core_FreeIoRequest(usbIoRequest);

    return ntStatus;

}


NTSTATUS
USBPORTBUSIF_GetRootHubSymbolicName(
    PVOID BusContext,
    PVOID HubSymNameBuffer,
    ULONG HubSymNameBufferLength,
    PULONG HubSymNameActualLength
    )
/*++

Routine Description:

    This API provides the same functionality as IOCTL_GET_HUB_NAME, returning
    the symbolic name for the root hub PDO.

    The Longhorn hub driver will always retrieve the hub name via the IOCTL to
    minimize alternate code paths in the hub.  Previous versions of the hub
    called this API directly when loaded for the root hub.

Arguments:

Return Value:

    NT status code.

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_OBJECT fdoDeviceObject, rhPdo;
    PDEVICE_EXTENSION devExt, rhDevExt;
    NTSTATUS ntStatus;
    UNICODE_STRING hubNameUnicodeString;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;
    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    DbgTrace((UsbpDebugTrace, "  HubSymNameBuffer %p \n", HubSymNameBuffer));
    DbgTrace((UsbpDebugTrace, "  HubSymNameBufferLength x%x \n", HubSymNameBufferLength));
    DbgTrace((UsbpDebugTrace, "  HubSymNameActualLength x%x \n", HubSymNameBufferLength));

    do {

        rhPdo = USBPORT_RefRootHubPdo(fdoDeviceObject, REF_RhPDO_GetRootHubSymbolicName);
        if (rhPdo == NULL) {
            ntStatus = STATUS_INVALID_PARAMETER;
            break;
        }

        ntStatus = USBPORT_GetSymbolicName(fdoDeviceObject,
                                           rhPdo,
                                           &hubNameUnicodeString);

        // copy what we can
        if (HubSymNameBufferLength >= hubNameUnicodeString.Length) {
            RtlCopyMemory(HubSymNameBuffer,
                          hubNameUnicodeString.Buffer,
                          hubNameUnicodeString.Length);
        } else if (HubSymNameBufferLength >= sizeof(UNICODE_NULL)) {
            // too small return a NULL
                RtlZeroMemory(HubSymNameBuffer,
                          sizeof(UNICODE_NULL));
        }
        *HubSymNameActualLength = hubNameUnicodeString.Length;

        DbgTrace((UsbpDebugTrace, " hubNameUnicodeString.Buffer  %p l %d\n",
            hubNameUnicodeString.Buffer,
            hubNameUnicodeString.Length));

        DbgTrace((UsbpDebugTrace, "  HubSymNameActualLength x%x \n", *HubSymNameActualLength));

        RtlFreeUnicodeString(&hubNameUnicodeString);

        USBPORT_DerefRootHubPdo(fdoDeviceObject, rhPdo, REF_RhPDO_GetRootHubSymbolicName);
        // note we always return status success, in order to be backward
        // compaible with the original IOCTL
    } WHILE (0);

    if (HubSymNameBufferLength < *HubSymNameActualLength) {
        ntStatus = STATUS_BUFFER_OVERFLOW;
    }

    return ntStatus;
}


VOID
USBPORTBUSIF_FlushTransfers(
    PVOID BusContext,
    PVOID DeviceHandle
    )
/*++

Routine Description:

*    Flushes any outstanding tranfers for a device handle plus the bad request
*    list.
*    if no device handle is given just all tranfers on the bad request list
*    are flushed.
*
*    The purpose of this function is to complete any tranfers that may be pening
*    by client drivers that are about to unload.

Service exported for use by the hub driver

This routine flushes any outstanding transfers associated with a bus and
device handle.  The service is used in conjunction with IRP_MN_REMOVE_DEVICE
for a device PDO to ensure that all outstanding transfers have been completed
when a device is removed.

IRQL = ANY

Arguments:

BusContext - Handle to the bus we need to create the device on. This is the
value returned in the call to QUERY_INTERFACE and is opaque to the caller.

DeviceHandle - Opaque structure returned from CreateUsbDevice.


Return Value:



--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_OBJECT fdoDeviceObject;
    PDEVICE_EXTENSION devExt, rhDevExt;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;
    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    return;
}


VOID
USBPORTBUSIF_SetDeviceHandleData(
    PVOID BusContext,
    PVOID DeviceHandle,
    PDEVICE_OBJECT UsbDevicePdo
    )
/*++

Routine Description:

    Assocaites a particular PDO with a device handle for use in post mortem debugging situations.

    This routine must be called at passive level.

    The PDO is referenced during this call, however, it is not referenced after so the pointer
    should not be accessed.  The device object can be used later to detect the particular device
    driver involved in an OCA crash.

Arguments:

    BusContext - bus context for the hub bus interface.

    DeviceHandle - device handle to link this PDO and driver name to

    UsbDevicePdo - PDO enumerated by USBHUB driver for this device

Return Value:

    none.

--*/
{
    PBUS_CONTEXT bc = BusContext;
    PDEVICE_EXTENSION devExt;
    PUSBD_DEVICE_HANDLE dh = DeviceHandle;

    GET_DEVICE_EXT(devExt, bc->HcFdo);
    ASSERT_FDOEXT(devExt);

    DbgTrace((UsbpDebugTrace, "  SetDeviceHandleData (PDO) %p dh (%p)\n", UsbDevicePdo, dh));

    if (UsbDevicePdo != NULL &&
        dh != NULL) {

        PDEVICE_OBJECT fdo;

        // record the PDO
        dh->DevicePdo_Unreferenced = UsbDevicePdo;

        // get driver name from device object.

        // walk up one location for the FDO
        // note: this may be verifier but we need to know this a well
        fdo = UsbDevicePdo->AttachedDevice;
        DbgTrace((UsbpDebugTrace, "  SetDeviceHandleData (FDO) %p \n", fdo));

        // there may be no FDO if the client driver never attached

        if (fdo != NULL &&
            fdo->DriverObject != NULL ) {

            PDRIVER_OBJECT driverObject;
            ULONG len, i;
            PWCHAR pwch;

            driverObject = fdo->DriverObject;

            pwch = driverObject->DriverName.Buffer;

            // Attempt to extract the driver name and store it.
            // driver name length is in bytes, len is char count
            len = (driverObject->DriverName.Length/sizeof(WCHAR)) > USB_DRIVER_NAME_LEN ?
                USB_DRIVER_NAME_LEN :
                driverObject->DriverName.Length/sizeof(WCHAR);

            // grab the driver name
            for (i=0; i<len && pwch; i++) {
                dh->DriverName[i] = *pwch;
                pwch++;
            }
        }
    }
}


ULONG
USBPORTBUSIF_SetDevH_IdleReadyState(
    PVOID BusContext,
    PVOID DeviceHandle,
    ULONG NewIdleReadyState
    )
/*++

Routine Description:

    Sets the idle_ready state for a device handle.  When all handles indicate ready
    we do a global suspend.

Arguments:

    DeviceHandle - refrenced device handle.

    IdleReadyState - nonzero indicates idle ready -- zero indicates clear_idle

Return Value:

ULONG SetDeviceHandleIdleState(
    IN PVOID BusContext,
    IN PUSB_DEVICE_HANDLE DeviceHandle,
    IN ULONG IdleReadyState
    );

--*/
{
    PDEVICE_OBJECT pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_OBJECT fdoDeviceObject;
    PDEVICE_EXTENSION rhDevExt;

    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;

    return USBPORT_SetDevH_IdleReadyState(fdoDeviceObject, DeviceHandle, NewIdleReadyState);
}


ULONG
USBPORT_SetDevH_IdleReadyState(
    PDEVICE_OBJECT HcFdo,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    ULONG NewIdleReadyState
    )
/*++

Routine Description:

    Sets a new idle ready state and schedules a bus resume if necessary.

Arguments:

    DeviceHandle - referenced device handle.

    IdleReadyState - nonzero indicates idle ready -- zero indicates clear_idle

Return Value:

    The previous value of the idle_ready flag for this device

--*/
{
    ULONG oldIdleReadyState;
    PDEVICE_EXTENSION devExt;

    GET_DEVICE_EXT(devExt, HcFdo);
    ASSERT_FDOEXT(devExt);
    ASSERT_DEVICE_HANDLE(DeviceHandle);

    oldIdleReadyState = InterlockedExchange(&DeviceHandle->IdleReadyState,
                                            NewIdleReadyState);

    // Write a USBPORT_ETW_EVENT_DEVICE_SUSPEND_CHANGE event.
    //
    {
        PCEVENT_DESCRIPTOR  eventDescriptor;
        ULONG               SuspendPreviousState;
        ULONG               SuspendNewState;

        if (oldIdleReadyState) {
            SuspendPreviousState = 1;
        } else {
            SuspendPreviousState = 0;
        }

        if (NewIdleReadyState) {
            SuspendNewState = 1;
            eventDescriptor = &USBPORT_ETW_EVENT_DEVICE_IDLE_STATE_SET;
        } else {
            SuspendNewState = 0;
            eventDescriptor = &USBPORT_ETW_EVENT_DEVICE_IDLE_STATE_CLEAR;
        }

        USBPORT_EtwWriteHcDeviceData2(HcFdo,
                                      DeviceHandle,
                                      eventDescriptor,
                                      NULL,
                                      &SuspendPreviousState,
                                      sizeof(ULONG),
                                      &SuspendNewState,
                                      sizeof(ULONG));
    }

    return oldIdleReadyState;
}


VOID
USBPORT_BusContextEvent(
    PBUS_CONTEXT Bc,
    USBPORT_BUS_CONTEXT_EVENT Event
    )
/*++

Routine Description:

    This routine references/dereferences the bus context structure we use of QUERY_BUS_INTERFACE calls.

    Since we cannot fail we do the state transition method.

Arguments:

    Bc -Bus context to add the ref


Return Value:

    none

--*/
{

    PDEVICE_EXTENSION devExt;
    KIRQL irql;
    LONG r;

    GET_DEVICE_EXT(devExt, Bc->HcFdo);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->Fdo.BusContextSpin, &irql);
    LOGENTRY(Bc->HcFdo, LOG_MISC, 'bsEV', 0,  Event, Bc);
    ASSERT_BUS_CONTEXT(Bc);

    DbgTrace((UsbpDebugTrace, "'USBPORT_BusContextEvent %p\n", Bc));

    switch(Event) {
    case EvBc_Create:
        Bc->BcState = Bc_Valid;
        InsertTailList(&devExt->Fdo.BusContextHead,
                       &Bc->NextEntry);

        break;

    case EvBc_Usbd_Ref:
    case EvBc_Usbh_Ref:
    case EvBc_UsbMinid_Ref:
    case EvBc_UsbHubSS_Ref:
        USBPORT_ASSERT(Bc->BcState == Bc_Valid);
        InterlockedIncrement(&Bc->InterfaceRefCount);

        if (Event == EvBc_Usbd_Ref) {
            InterlockedIncrement(&Bc->UsbdRefCount);
        } else if (Event == EvBc_Usbh_Ref) {
            InterlockedIncrement(&Bc->UsbhRefCount);
        } else if (Event == EvBc_UsbMinid_Ref) {
            InterlockedIncrement(&Bc->UsbMinidRefCount);
        } else if (Event == EvBc_UsbHubSS_Ref) {
            InterlockedIncrement(&Bc->UsbHubSSRefCount);
        }

#if DBG
        USBPORT_ASSERT((Bc->UsbhRefCount+Bc->UsbdRefCount+Bc->UsbMinidRefCount+Bc->UsbHubSSRefCount) == Bc->InterfaceRefCount);
#endif
        break;

    case EvBc_Usbd_Deref:
    case EvBc_Usbh_Deref:
    case EvBc_UsbMinid_Deref:
    case EvBc_UsbHubSS_Deref:
        USBPORT_ASSERT(Bc->BcState == Bc_Valid ||
                       Bc->BcState == Bc_Removed);
        r = InterlockedDecrement(&Bc->InterfaceRefCount);

        if (Event == EvBc_Usbd_Deref) {
            InterlockedDecrement(&Bc->UsbdRefCount);
        } else if (Event == EvBc_Usbh_Deref) {
            InterlockedDecrement(&Bc->UsbhRefCount);
        } else if (Event == EvBc_UsbMinid_Deref) {
            InterlockedDecrement(&Bc->UsbMinidRefCount);
        } else if (Event == EvBc_UsbHubSS_Deref) {
            InterlockedDecrement(&Bc->UsbHubSSRefCount);
        }


#if DBG
        USBPORT_ASSERT((Bc->UsbhRefCount+Bc->UsbdRefCount+Bc->UsbMinidRefCount+Bc->UsbHubSSRefCount) == Bc->InterfaceRefCount);
#endif
        if (r <= 0) {
            //last external ref releases the handle

#if DBG
            // if r is less than zero then the last ref was dropped after the remove
            USBPORT_ASSERT(r == 0);
            USBPORT_ASSERT(Bc->UsbhRefCount == 0);
            USBPORT_ASSERT(Bc->UsbdRefCount == 0);
            USBPORT_ASSERT(Bc->UsbMinidRefCount == 0);
            USBPORT_ASSERT(Bc->UsbHubSSRefCount == 0);
#endif
            Bc->BcState = Bc_Released;
            if (Bc->DeviceHandle) {
                USBPORT_DereferenceDeviceHandle(Bc->HcFdo,
                                                Bc->DeviceHandle,
                                                Bc,
                                                DEVH_BUS_CONTEXT_TAG,
                                                FALSE);
                Bc->DeviceHandle = NULL;
            }
        }
        break;

    case EvBc_Remove:
        // this is handle directly by the port remove path
        USBPORT_ASSERTMSG("Unexpected Event EvBc_Remove", FALSE);
        break;

    }

    KeReleaseSpinLock(&devExt->Fdo.BusContextSpin, irql);

}


VOID
USBPORT_ClearBusContextList(
    PDEVICE_OBJECT FdoDeviceObject
    )
/*++

Routine Description:

    This rountine is called to release all buscontext structures.  The bus context
    is a function drivers pointer into the port drivers FDO so we cann unload if
    these are still referenced.

    This routine is called during FDO remove.

Arguments:

Return Value:

--*/
{
    KIRQL oldIrql;
    PLIST_ENTRY busEntry = NULL;
    PBUS_CONTEXT busContext = NULL;
    PDEVICE_EXTENSION devExt = NULL;

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    KeAcquireSpinLock(&devExt->Fdo.BusContextSpin, &oldIrql);
    while(FALSE == IsListEmpty(&devExt->Fdo.BusContextHead)) {
        LONG r;

        busEntry = RemoveHeadList(&devExt->Fdo.BusContextHead);
        busContext = CONTAINING_RECORD(busEntry,
                                       BUS_CONTEXT,
                                       NextEntry);

        // it might be valid if were never ref'ed and releases
        ASSERT_BUS_CONTEXT(busContext);
        USBPORT_ASSERT(busContext->BcState == Bc_Valid ||
                       busContext->BcState == Bc_Released);


        busContext->BcState = Bc_Removed;
        busContext->RhPdo = NULL;
        busContext->HcFdo = NULL;
        r = InterlockedDecrement(&busContext->InterfaceRefCount);

        // if this assert fails it means that someone still has a pointer that
        // we allocated.
        if (r!=-1) {
            // references remains after FDO/PDO have been removed.  These may be due to bugs
            // in the function driver.

            // the code below frees them anyway for backward compatibility.
            USBPORT_KdPrint((1, "Detected leaked bus context %p\n", busContext));
            //TEST_TRAP();
            // release the device handle ref
            busContext->BcState = Bc_Released;
            if (busContext->DeviceHandle) {
                USBPORT_DereferenceDeviceHandle(FdoDeviceObject,
                                                busContext->DeviceHandle,
                                                busContext,
                                                DEVH_BUS_CONTEXT_TAG,
                                                FALSE);
                busContext->DeviceHandle = NULL;
            }

            UsbFreePool(busContext);
        } else {
            UsbFreePool(busContext);
        }
    }
    KeReleaseSpinLock(&devExt->Fdo.BusContextSpin, oldIrql);

    return;
}


VOID
USBPORT_BusifLocationReference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine increments outstanding reference count for this interface

Arguments:

    Context - Root hub PDO extension pointer.

Return Value:

    None

--*/
{
    // Increment something here...
}


VOID
USBPORT_BusifLocationDereference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine decrements outstanding reference count for this interface

Arguments:

    Context - Root hub PDO extension pointer.

Return Value:

    None

--*/
{
    // Decrement something here...
}


NTSTATUS
USBPORT_BusifLocationGetLocationStrings(
    __inout PVOID Context,
    __out PWCHAR *LocationStrings
    )
/*++

Routine Description:

    This routine allocates, fills in, and returns a Multi-Sz string
    containing the bus-relative location identifier string for the
    given device.

    The location string for a USB root hub PDO is "USBROOT(0)"

    This interface is permitted to return a Multi-Sz containing
    multiple strings describing the same device, but in this
    first implementation, only the single strings listed above
    will be returned from the interface.  The string must still
    be in the format of a Multi-Sz, however, meaning a double-NULL
    terminator is required.

Arguments:

    Context - Root hub PDO extension pointer.

Return Value:

    NTSTATUS code.

--*/
{
    PWCHAR  stringBuffer;

    *LocationStrings = NULL;

    UsbAllocPool_Z(&stringBuffer,
                   PagedPool,
                   LOCATION_INTERFACE_DEVICE_STRING_COUNT*sizeof(WCHAR));

    if (!stringBuffer) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    // The location string for a USB root hub PDO is "USBROOT(0)"
    //
    RtlCopyMemory(stringBuffer,
                  L"USBROOT(0)",
                  sizeof(L"USBROOT(0)"));

    *LocationStrings = stringBuffer;

    return STATUS_SUCCESS;
}


NTSTATUS
USBPORT_BusifGetLocationInterface(
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
    PDEVICE_EXTENSION       rhDevExt;
    PIO_STACK_LOCATION      irpStack;
    USHORT                  requestedSize;
    PPNP_LOCATION_INTERFACE locationInterface = NULL;
    NTSTATUS                ntStatus;

    GET_DEVICE_EXT(rhDevExt, Pdo);
    ASSERT_PDOEXT(rhDevExt);

    irpStack = IoGetCurrentIrpStackLocation(Irp);
    requestedSize = irpStack->Parameters.QueryInterface.Size;

    if (requestedSize >= sizeof(PNP_LOCATION_INTERFACE)) {

        locationInterface = (PPNP_LOCATION_INTERFACE)
            irpStack->Parameters.QueryInterface.Interface;

        locationInterface->Size = sizeof(PNP_LOCATION_INTERFACE);
        locationInterface->Version = LOCATION_INTERFACE_VERSION;
        locationInterface->Context = rhDevExt;
        locationInterface->InterfaceReference = USBPORT_BusifLocationReference;
        locationInterface->InterfaceDereference = USBPORT_BusifLocationDereference;
        locationInterface->GetLocationString = USBPORT_BusifLocationGetLocationStrings;

        ntStatus = STATUS_SUCCESS;

    } else {

        ntStatus = STATUS_UNSUCCESSFUL;
    }

    if (NT_SUCCESS(ntStatus)) {
        locationInterface->InterfaceReference(locationInterface->Context);
    }

    Irp->IoStatus.Status = ntStatus;
    Irp->IoStatus.Information = 0;

    return ntStatus;
}


NTSTATUS
USBPORTBUSIF_GetContainerIdForPort(
    PVOID BusContext,
    USHORT PortNumber,
    LPGUID ContainerId
    )
/*++

Routine Description:

    Queries the host controller miniport driver for a Container ID
    for a device attached to the specified port.

Arguments:

    BusContext - hub bus interface context

    PortNumber - Port number the device is attached to

    ContainerId - Pointer to the GUID that will contain the Container ID on return.

Irql: PASSIVE

Return Value:

    nt status, STATUS_SUCCESS if ContainerId contains a valid Conainter ID.

--*/
{
    PDEVICE_OBJECT rhDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt, devExt;

    GET_DEVICE_EXT(rhDevExt, rhDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    GET_DEVICE_EXT(devExt, rhDevExt->HcFdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    return (MPf_GetContainerIdForPort(devExt, PortNumber, ContainerId));

}


VOID
USBPORTBUSIF_SetContainerIdForPort(
    PVOID BusContext,
    USHORT PortNumber,
    LPGUID ContainerId
    )
/*++

Routine Description:

    Informs the host controller miniport driver of the value of the Container ID
    for a device attached to the specified port.

Arguments:

    BusContext - hub bus interface context

    PortNumber - Port number the device is attached to

    ContainerId - Pointer to the GUID that contains the Container ID.

Irql: PASSIVE

Return Value:

    none

--*/
{
    PDEVICE_OBJECT rhDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt, devExt;

    GET_DEVICE_EXT(rhDevExt, rhDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    GET_DEVICE_EXT(devExt, rhDevExt->HcFdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    MPf_SetContainerIdForPort(devExt, PortNumber, ContainerId);
}


NTSTATUS
USBPORTBUSIF_AbortAllDevicePipes(
    PVOID               BusContext,
    PUSB_DEVICE_HANDLE  DeviceHandle
    )
/*++

Routine Description:

Service exported for use by the hub driver

This routine is used to synchronously abort any outstanding transfer
requests on all pipe handles of the device.

IRQL = PASSIVE

Arguments:

    DeviceHandle - a referenced device handle.

Return Value:

    NT status code.

--*/
{
    PDEVICE_OBJECT      pdoDeviceObject;
    PDEVICE_EXTENSION   rhDevExt;
    PDEVICE_OBJECT      fdoDeviceObject;
    PDEVICE_EXTENSION   devExt;
    PUSBD_DEVICE_HANDLE devH;
    NTSTATUS            ntStatus;

    pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;
    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    //
    // Ensure that the controller is powered up before we touch the hardware
    //
    USBPORT_RequestRuntimePMTransitionToActive(rhDevExt->HcFdoDeviceObject);

    devH = (PUSBD_DEVICE_HANDLE)DeviceHandle;

    ntStatus = STATUS_SUCCESS;

    LOCK_DEVICE_CONFIGS(devH, fdoDeviceObject);

    if (!USBPORT_iValidDeviceHandle(fdoDeviceObject, devH)) {

        ntStatus = STATUS_INVALID_PARAMETER;

    } else {

        PLIST_ENTRY listEntry;

        for (listEntry =  devH->PipeHandleList.Flink;
             listEntry != &devH->PipeHandleList;
             listEntry =  listEntry->Flink) {

            PUSBD_PIPE_HANDLE_I pipeHandle;

            pipeHandle = CONTAINING_RECORD(listEntry,
                                           USBD_PIPE_HANDLE_I,
                                           ListEntry);

            ASSERT_PIPE_HANDLE(pipeHandle);

            if (pipeHandle->EpHandle != USB_ZERO_BW_ENDP_HANDLE) {

                PHCD_ENDPOINT   endpoint;

                endpoint = USBPORT_EndpointFromHandle(pipeHandle->EpHandle);

                // If there are any outstanding transfers on the
                // endpoint then abort them now.
                //
                if (USBPORT_Core_GetEndpointTransferCount(fdoDeviceObject, endpoint)) {
                    USBPORT_Core_SyncAbortEndpoint(fdoDeviceObject, endpoint);
                }
            }
        }
    }

    UNLOCK_DEVICE_CONFIGS(devH, fdoDeviceObject);
    USBPORT_RequestRuntimePMTransitionToIdle(rhDevExt->HcFdoDeviceObject);

    return ntStatus;
}


USHORT
USBPORTBUSIF_GetDebugPortNumber(
    PVOID BusContext
    )
/*++

Routine Description:

    Queries the host controller miniport driver for the debug port number

Arguments:

    BusContext - hub bus interface context

Irql: PASSIVE

Return Value:

    Debug port number if supported, 0 otherwise.

--*/
{
    PDEVICE_OBJECT rhDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    PDEVICE_EXTENSION rhDevExt, devExt;

    GET_DEVICE_EXT(rhDevExt, rhDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    GET_DEVICE_EXT(devExt, rhDevExt->HcFdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    return (MPf_GetDebugPortNumber(devExt));

}


VOID
USBPORTBUSIF_SetMsOs20Support(
    PVOID                   BusContext,
    PUSB_DEVICE_HANDLE      DeviceHandle,
    PUSB_DEVICE_DESCRIPTOR  DeviceDescriptor,
    UCHAR                   MsOs20VendorCode
    )
/*++

Routine Description:

    Notification of MS OS 2.0 support for a device

Arguments:

    BusContext - hub bus interface context.

    DeviceHandle - a referenced device handle.

    DeviceDescriptor - Device descriptor to set.

Irql: PASSIVE

Return Value:

    None

--*/
{
    PUSBD_DEVICE_HANDLE deviceHandle;

    deviceHandle = DeviceHandle;
    ASSERT_DEVICE_HANDLE(deviceHandle);

    // Store the original device descriptor
    RtlCopyMemory(&deviceHandle->OriginalDeviceDescriptor,
                  &deviceHandle->DeviceDescriptor,
                  sizeof(USB_DEVICE_DESCRIPTOR));

    if (DeviceDescriptor != NULL) {
        // copy the alternate device descriptor
        RtlCopyMemory(&deviceHandle->DeviceDescriptor, DeviceDescriptor, sizeof(USB_DEVICE_DESCRIPTOR));
    }

    SET_FLAG(deviceHandle->DeviceFlags, USBPORT_DEVICEFLAG_SUPPORTS_MSOS20_DESCRIPTOR);

    deviceHandle->MsOs20VendorCode = MsOs20VendorCode;
}


VOID
USBPORTBUSIF_SetClearRootPortResumeTime(
    PVOID                   BusContext,
    PUSB_DEVICE_HANDLE      DeviceHandle,
    BOOLEAN                 SetTime,
    UCHAR                   MinResumeTime
    )
/*++

Routine Description:

    Sets or clears the resume time for a root port

Arguments:

    BusContext - hub bus interface context.

    DeviceHandle - a referenced device handle.

    SetTime - Indicates whether to set or clear the resume time

    MinResumeTime - If SetTime is TRUE, indicates the resume time to set for the root port

Irql: PASSIVE

Return Value:

    None

--*/
{
    PUSBD_DEVICE_HANDLE deviceHandle;
    USHORT rootPortNumber;
    PDEVICE_OBJECT pdoDeviceObject;
    PDEVICE_OBJECT fdoDeviceObject;
    PDEVICE_EXTENSION devExt;
    PDEVICE_EXTENSION rhDevExt;

    pdoDeviceObject = USBPORT_PdoFromBusContext(BusContext);
    GET_DEVICE_EXT(rhDevExt, pdoDeviceObject);
    ASSERT_PDOEXT(rhDevExt);

    fdoDeviceObject = rhDevExt->HcFdoDeviceObject;

    GET_DEVICE_EXT(devExt, fdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    deviceHandle = DeviceHandle;
    ASSERT_DEVICE_HANDLE(deviceHandle);

    rootPortNumber = LOWORD(deviceHandle->EtwUsbDevice.PortPath.PortPath[0]);

    MPf_SetClearPortResumeTime(devExt,
                               rootPortNumber,
                               SetTime,
                               MinResumeTime);
}

VOID
USBPORTBUSIF_SetDeviceErrataFlag(
    PVOID BusContext,
    PUSB_DEVICE_HANDLE DeviceHandle,
    ULONG DeviceErrataFlag
    )
/*++

Routine Description:

    Informs USBPORT about devices that need work around anomalous behavior

Arguments:

    BusContext - hub bus interface context

    DeviceHandle - Device handle for the hub

    DeviceErrataFlag - Flag indicating workaround needed for the device.

Irql: ANY

Return Value:

    none

--*/
{
    PUSBD_DEVICE_HANDLE devH = (PUSBD_DEVICE_HANDLE)DeviceHandle;

    if (TEST_FLAG(DeviceErrataFlag, ERRATA_FLAG_RESET_TT_ON_CANCEL)) {
        SET_FLAG(devH->DeviceFlags, USBPORT_DEVICEFLAG_HSHUB_RESET_TT_ON_CANCEL);
    }

    if (TEST_FLAG(DeviceErrataFlag, ERRATA_FLAG_NO_CLEAR_TT_BUFFER_ON_CANCEL)) {
        SET_FLAG(devH->DeviceFlags, USBPORT_DEVICEFLAG_HSHUB_NO_CLEAR_TT_BUFFER_ON_CANCEL);
    }
}
