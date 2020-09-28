/*++

Copyright (c) 2002 Microsoft Corporation

Module Name:

    dispatch.c

Abstract:

    This module contains the global dispatch related
    routines for the sd controller & it's child devices

Author:

    Neil Sandlin  (neilsa) Jan 1 2002

Environment:

    Kernel mode

Revision History :


--*/

#include "pch.h"
#include "dispatch.tmh"


__drv_dispatchType(IRP_MJ_CREATE)
__drv_dispatchType(IRP_MJ_CLOSE)
__drv_dispatchType(IRP_MJ_DEVICE_CONTROL)
__drv_dispatchType(IRP_MJ_INTERNAL_DEVICE_CONTROL)
__drv_dispatchType(IRP_MJ_SHUTDOWN)
__drv_dispatchType(IRP_MJ_CLEANUP)
__drv_dispatchType(IRP_MJ_POWER)
__drv_dispatchType(IRP_MJ_SYSTEM_CONTROL)
__drv_dispatchType(IRP_MJ_PNP)
DRIVER_DISPATCH SdbusDispatch;
/*
NTSTATUS
SdbusDispatch(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    );
*/

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(INIT, SdbusInitDeviceDispatchTable)
#endif

//
// Dispatch table array for FDOs/PDOs
//

PDRIVER_DISPATCH DeviceObjectDispatch[sizeof(DEVICE_OBJECT_TYPE)][IRP_MJ_MAXIMUM_FUNCTION + 1];


VOID
SdbusInitDeviceDispatchTable(
    __inout PDRIVER_OBJECT DriverObject
    )

/*++

Routine Description: 
 
    Initializes the IRP dispatch tables for Pdo's & Fdo's

Arguments: 
 
    DriverObject

Return value: 
 
    VOID

--*/

{
    PAGED_CODE();

    //
    // Init the controller (FDO) dispatch table
    //

    DeviceObjectDispatch[FDO][IRP_MJ_CREATE]                    = SdbusOpenCloseDispatch;
    DeviceObjectDispatch[FDO][IRP_MJ_CLOSE]                     = SdbusOpenCloseDispatch;
    DeviceObjectDispatch[FDO][IRP_MJ_CLEANUP]                   = SdbusCleanupDispatch;
    DeviceObjectDispatch[FDO][IRP_MJ_DEVICE_CONTROL]            = SdbusFdoDeviceControl;
    DeviceObjectDispatch[FDO][IRP_MJ_INTERNAL_DEVICE_CONTROL]   = SdbusFdoInternalDeviceControl;
    DeviceObjectDispatch[FDO][IRP_MJ_SYSTEM_CONTROL]            = SdbusFdoSystemControl;
    DeviceObjectDispatch[FDO][IRP_MJ_PNP]                       = SdbusFdoPnpDispatch;
    DeviceObjectDispatch[FDO][IRP_MJ_POWER]                     = SdbusFdoPowerDispatch;
    DeviceObjectDispatch[FDO][IRP_MJ_SHUTDOWN]                  = SdbusShutdownDispatch;

    //
    // Init the PDO dispatch table
    //

    DeviceObjectDispatch[PDO][IRP_MJ_DEVICE_CONTROL]          = SdbusPdoDeviceControl;
    DeviceObjectDispatch[PDO][IRP_MJ_INTERNAL_DEVICE_CONTROL] = SdbusPdoInternalDeviceControl;
    DeviceObjectDispatch[PDO][IRP_MJ_SYSTEM_CONTROL]          = SdbusPdoSystemControl;
    DeviceObjectDispatch[PDO][IRP_MJ_PNP]                     = SdbusPdoPnpDispatch;
    DeviceObjectDispatch[PDO][IRP_MJ_POWER]                   = SdbusPdoPowerDispatch;

    //
    // Set the global dispatch table
    // 

#pragma warning(push)
#pragma warning(disable:28175)
    DriverObject->MajorFunction[IRP_MJ_CREATE]                  = SdbusDispatch;
    DriverObject->MajorFunction[IRP_MJ_CLOSE]                   = SdbusDispatch;
    DriverObject->MajorFunction[IRP_MJ_CLEANUP]                 = SdbusDispatch;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL]          = SdbusDispatch;
    DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = SdbusDispatch;
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL]          = SdbusDispatch;
    DriverObject->MajorFunction[IRP_MJ_SHUTDOWN]                = SdbusDispatch;
    DriverObject->MajorFunction[IRP_MJ_PNP]                     = SdbusDispatch;
    DriverObject->MajorFunction[IRP_MJ_POWER]                   = SdbusDispatch;
#pragma warning(pop)

}


NTSTATUS
SdbusDispatch(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )

/*++

Routine Description:

   Dispatch routine for all IRPs handled by this driver. This dispatch would
   then call the appropriate real dispatch routine which corresponds to the
   device object type (physical or functional).

Arguments:

   DeviceObject - Pointer to the device object this dispatch is intended for
 
   Irp - Pointer to the IRP to be handled

Return value: 
 
   Returns the status from the 'real' dispatch routine which handles this IRP

--*/

{
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PPDO_EXTENSION deviceExtension;
    NTSTATUS status;
    DEVICE_OBJECT_TYPE devtype;
    UCHAR MajorFunction = irpStack->MajorFunction;

    //
    // Note that the following code assumes that "Signature" is the first
    // field in both PDO_EXTENSION and FDO_EXTENSION structs.
    //

    deviceExtension = (PPDO_EXTENSION)DeviceObject->DeviceExtension;
    if (deviceExtension->Signature == SDBUS_PDO_EXTENSION_SIGNATURE) {
        devtype = PDO;
    } else {
        devtype = FDO;
    }

    if ((MajorFunction > IRP_MJ_MAXIMUM_FUNCTION) ||
       (DeviceObjectDispatch[devtype][MajorFunction] == NULL)) {

        DebugPrint((SDBUS_DEBUG_INFO,
                    "SDBUS: Dispatch skipping unimplemented Irp MJ function %x", 
                    MajorFunction));

        status = Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

    } else if (((devtype == PDO) && 
                ((PPDO_EXTENSION) DeviceObject->DeviceExtension)->PnpState == SDBUS_DEVICE_DELETED) ||
               ((devtype == FDO) && 
                ((PFDO_EXTENSION) DeviceObject->DeviceExtension)->PnpState == SDBUS_DEVICE_DELETED)) {

        //
        // This do was supposed to have been already deleted
        // so we don't support any IRPs on it
        //

        DebugPrint((SDBUS_DEBUG_INFO,
                    "SDBUS: Dispatch skipping Irp on deleted DO %p MJ function %x", 
                    DeviceObject, 
                    MajorFunction));

        if (MajorFunction == IRP_MJ_POWER) {
            PoStartNextPowerIrp(Irp);
        }
        status = Irp->IoStatus.Status = STATUS_DELETE_PENDING;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);

    } else {

        //
        // Dispatch the irp
        //

        status = ((*DeviceObjectDispatch[devtype][MajorFunction])(DeviceObject, Irp));
    }

    return status;
}


NTSTATUS
SdbusFdoPowerDispatch(
    PDEVICE_OBJECT Fdo,
    PIRP Irp
    )

/*++

Routine Description:

    This routine handles power requests for the PDOs.
 
Arguments:

    Pdo - pointer to the physical device object
 
    Irp - pointer to the io request packet

Return Value:

    NTSTATUS

--*/

{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;
    
    switch (irpStack->MinorFunction) {

    case IRP_MN_SET_POWER:

        if (irpStack->Parameters.Power.Type == SystemPowerState) {

            DebugPrint((SDBUS_DEBUG_POWER,
                        "fdo %p irp %p ====> IRP_MN_SET_POWER - FDO (S%x -> S%x)",
                        Fdo,
                        Irp,
                        fdoExtension->SystemPowerState - 1,
                        irpStack->Parameters.Power.State.SystemState - 1));

            SdbusAddFxDbgLog(fdoExtension,
                           'PowF',
                           irpStack->MinorFunction,
                           irpStack->Parameters.Power.Type,
                           irpStack->Parameters.Power.State.SystemState);
            
        } else {

            DebugPrint((SDBUS_DEBUG_POWER,
                        "fdo %p irp %p ====> IRP_MN_SET_POWER - FDO (D%x -> D%x)",
                        Fdo,
                        Irp,
                        fdoExtension->DevicePowerState - 1,
                        irpStack->Parameters.Power.State.DeviceState - 1));

            SdbusAddFxDbgLog(fdoExtension,
                           'PowF',
                           irpStack->MinorFunction,
                           irpStack->Parameters.Power.Type,
                           irpStack->Parameters.Power.State.DeviceState);
            
        }

        status = SdbusFdoSetPowerState(Fdo, Irp);
        break;


    case IRP_MN_QUERY_POWER:

        DebugPrint((SDBUS_DEBUG_POWER, 
                    "fdo %p irp %p --> IRP_MN_QUERY_POWER", 
                    Fdo, 
                    Irp));

        SdbusAddFxDbgLog(fdoExtension,
                       'PowF',
                       irpStack->MinorFunction,
                       (ULONG_PTR)Fdo,
                       (ULONG_PTR)Irp);
        
#if 0
        if (irpStack->Parameters.Power.Type == SystemPowerState) {
            DebugPrint((SDBUS_DEBUG_POWER,
                        "      (S%x)",
                        irpStack->Parameters.Power.State.SystemState - 1));            
        } else {
            DebugPrint((SDBUS_DEBUG_POWER,
                        "      (D%x)",
                        irpStack->Parameters.Power.State.DeviceState - 1));
        }
#endif

        //
        // Let the pdo handle it
        //

        PoStartNextPowerIrp(Irp);
        IoSkipCurrentIrpStackLocation(Irp);
        status = PoCallDriver(fdoExtension->LowerDevice, Irp);
        break;


    case IRP_MN_WAIT_WAKE:

        DebugPrint((SDBUS_DEBUG_POWER,
                    "fdo %p irp %p --> IRP_MN_WAIT_WAKE", 
                    Fdo, 
                    Irp));

        SdbusAddFxDbgLog(fdoExtension,
                       'PowF',
                       irpStack->MinorFunction,
                       (ULONG_PTR)Fdo,
                       (ULONG_PTR)Irp);

        status = SdbusFdoWaitWake(Fdo, Irp);
        break;


    default:
        DebugPrint((SDBUS_DEBUG_POWER,
                    "FdoPowerDispatch: Unhandled Irp %p received for %p",
                    Irp,
                    Fdo));

        SdbusAddFxDbgLog(fdoExtension,
                       'PowF',
                       irpStack->MinorFunction,
                       (ULONG_PTR)Fdo,
                       (ULONG_PTR)Irp);
        
        PoStartNextPowerIrp(Irp);
        IoSkipCurrentIrpStackLocation(Irp);
        status = PoCallDriver(fdoExtension->LowerDevice, Irp);
        break;
    }

    DebugPrint((SDBUS_DEBUG_POWER,
                "fdo %p irp %p <-- %08x", 
                Fdo, 
                Irp, 
                status));

    return status;
}



NTSTATUS
SdbusPdoPowerDispatch(
    PDEVICE_OBJECT Pdo,
    PIRP Irp
    )

/*++

Routine Description:

    This routine handles power requests for the PDOs.
 
Arguments:

    Pdo - pointer to the physical device object
 
    Irp - pointer to the io request packet

Return Value:

    NTSTATUS

--*/

{
    PPDO_EXTENSION pdoExtension = Pdo->DeviceExtension;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    NTSTATUS status = STATUS_INVALID_DEVICE_REQUEST;

    switch (irpStack->MinorFunction) {

    case IRP_MN_SET_POWER:


        if (irpStack->Parameters.Power.Type == SystemPowerState) {

            DebugPrint((SDBUS_DEBUG_POWER,
                        "pdo %p irp %p ====> IRP_MN_SET_POWER - PDO (S%x -> S%x)",
                        Pdo,
                        Irp,
                        pdoExtension->SystemPowerState - 1,
                        irpStack->Parameters.Power.State.SystemState - 1));

            SdbusAddDbgLog(pdoExtension->Socket,
                           'PowP',
                           irpStack->MinorFunction,
                           irpStack->Parameters.Power.Type,
                           irpStack->Parameters.Power.State.SystemState);
            
        } else {

            PFDO_EXTENSION fdoExtension = pdoExtension->Socket->FdoExtension;
            KLOCK_QUEUE_HANDLE lockHandle = {0};
            BOOLEAN irpQueued = FALSE;

            DebugPrint((SDBUS_DEBUG_POWER,
                        "pdo %p irp %p ====> IRP_MN_SET_POWER - PDO (D%x -> D%x)",
                        Pdo,
                        Irp,
                        pdoExtension->DevicePowerState - 1,
                        irpStack->Parameters.Power.State.DeviceState - 1));

            SdbusAddDbgLog(pdoExtension->Socket,
                           'PowP',
                           irpStack->MinorFunction,
                           irpStack->Parameters.Power.Type,
                           irpStack->Parameters.Power.State.DeviceState);

            NT_ASSERT((ULONG)fdoExtension->PoweredPdoCount <= fdoExtension->LivePdoCount);

            if (IsDeviceFlagSet(pdoExtension, SDBUS_PDO_REPORTED_MISSING)) {
                Irp->IoStatus.Status = STATUS_SUCCESS;
                status = Irp->IoStatus.Status;
                PoStartNextPowerIrp(Irp);
                IoCompleteRequest(Irp, IO_NO_INCREMENT);
                break;
            }

            //
            // We know the IRP is valid and will be processed--go active.
            //

            SdbusFxComponentActive(fdoExtension, 0);

            //
            // Save the PDO and the IRP in the device extension so we
            // can process them in the active callback.
            //

            SdbusAcquireInStackQueuedSpinLock(&fdoExtension->FxIrpQueueLock,
                                              &lockHandle);

            //
            // If the device is already active, then the IRP can be issued
            // now.
            //

            if (!fdoExtension->Active) {
                
                InsertTailList(&fdoExtension->FxIrpQueue,
                               &Irp->Tail.Overlay.ListEntry);

                irpQueued = TRUE;
            }

            if (irpQueued) {
                IoMarkIrpPending(Irp);
                SdbusReleaseInStackQueuedSpinLock(&lockHandle);
                return STATUS_PENDING;
            }

            SdbusReleaseInStackQueuedSpinLock(&lockHandle);
        }

        status = SdbusPdoSetPowerState(Pdo, Irp);
        break;

    case IRP_MN_QUERY_POWER:

        DebugPrint((SDBUS_DEBUG_POWER,
                    "pdo %p irp %p --> IRP_MN_QUERY_POWER", 
                    Pdo, 
                    Irp));

        SdbusAddDbgLog(pdoExtension->Socket,
                       'PowP',
                       irpStack->MinorFunction,
                       (ULONG_PTR)Pdo,
                       (ULONG_PTR)Irp);

#if 0
        if (irpStack->Parameters.Power.Type == SystemPowerState) {
            DebugPrint((SDBUS_DEBUG_POWER,
                        "      (S%x)",
                        irpStack->Parameters.Power.State.SystemState - 1));            
        } else {
            DebugPrint((SDBUS_DEBUG_POWER,
                        "      (D%x)",
                        irpStack->Parameters.Power.State.DeviceState - 1));
        }
#endif

        status = Irp->IoStatus.Status = STATUS_SUCCESS;
        PoStartNextPowerIrp(Irp);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        break;


    case IRP_MN_WAIT_WAKE: {

        BOOLEAN completeIrp;

        DebugPrint((SDBUS_DEBUG_POWER,
                    "pdo %p irp %p --> IRP_MN_WAIT_WAKE", 
                    Pdo, 
                    Irp));

        SdbusAddDbgLog(pdoExtension->Socket,
                       'PowP',
                       irpStack->MinorFunction,
                       (ULONG_PTR)Pdo,
                       (ULONG_PTR)Irp);

        //
        // Should not have a wake pending already
        //

        NT_ASSERT (!(((PPDO_EXTENSION)Pdo->DeviceExtension)->Flags & SDBUS_PDO_WAKE_PENDING));

        status = SdbusPdoWaitWake(Pdo, Irp, &completeIrp);

        if (completeIrp) {
            PoStartNextPowerIrp(Irp);
            Irp->IoStatus.Status = status;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
        }
        break;
    }

    default:

        //
        // Unhandled minor function
        //

        SdbusAddDbgLog(pdoExtension->Socket,
                       'PowP',
                       irpStack->MinorFunction,
                       (ULONG_PTR)Pdo,
                       (ULONG_PTR)Irp);
        
        status = Irp->IoStatus.Status;
        PoStartNextPowerIrp(Irp);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
    }

    DebugPrint((SDBUS_DEBUG_POWER, "pdo %p irp %p <-- %08x", 
                Pdo, 
                Irp, 
                status));
    return status;
}


