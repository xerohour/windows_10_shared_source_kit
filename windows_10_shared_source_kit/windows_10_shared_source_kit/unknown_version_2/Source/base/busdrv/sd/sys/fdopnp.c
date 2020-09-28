/*++

Copyright (c) 1997-2000 Microsoft Corporation

Module Name:

    fdopnp.c

Abstract:

    This module contains the code that handles PNP irps for sd bus driver
    targeted towards the FDO's (for the sd controller object)

Author:

    Neil Sandlin (neilsa) 01-Jan-2002
    Yue Zuo (yuezuo) 01-Feb-2008

Environment:

    Kernel mode

Revision History :

--*/

#include "pch.h"
#include "fdopnp.tmh"

//
// Internal References
//


NTSTATUS
SdbusGetPciControllerType(
    __in PDEVICE_OBJECT Pdo,
    __inout PDEVICE_OBJECT Fdo
    );

NTSTATUS
SdbusGetAcpiControllerType(
    __in PDEVICE_OBJECT Pdo,
    __inout PDEVICE_OBJECT Fdo
    );

NTSTATUS
SdbusFdoStartDevice(
    __inout PDEVICE_OBJECT Fdo,
    __in PIRP Irp
    );

IO_COMPLETION_ROUTINE SdbusFdoStartDeviceComplete;
/*
NTSTATUS
SdbusFdoStartDeviceComplete(
    __in PDEVICE_OBJECT Fdo,
    __in PIRP Irp,
    __in PVOID Context
    );
*/

IO_WORKITEM_ROUTINE_EX SdbusFdoStartDeviceWorker;
/*
VOID
SdbusFdoStartDeviceWorker(
    __in PDEVICE_OBJECT Fdo,
    __in PIRP Irp,
    __in PIO_WORKITEM IoWorkItem
    )
*/

VOID
SdbusFdoStopDevice(
    __inout PDEVICE_OBJECT Fdo
    );

VOID
SdbusFdoFreeQueuedPackets(
    __inout PDEVICE_OBJECT Fdo
    );

VOID
SdbusFdoFreeDeviceResources(
    __inout PDEVICE_OBJECT Fdo
    );

VOID
SdbusFdoRemoveDevice(
    __in PDEVICE_OBJECT Fdo
    );

NTSTATUS
SdbusFdoDeviceCapabilities(
    __inout PDEVICE_OBJECT Fdo,
    __inout PIRP Irp
    );

NTSTATUS
SdbusFdoQueryBusInformation(
    __inout PDEVICE_OBJECT Pdo,
    __inout PFDO_EXTENSION FdoExtension    
    );

NTSTATUS
SdbusDeviceRelations(
    __inout PDEVICE_OBJECT Fdo,
    __inout PIRP Irp,
    __in DEVICE_RELATION_TYPE RelationType,
    __deref_out PDEVICE_RELATIONS *DeviceRelations
    );

NTSTATUS
SdbusFdoDeviceUsageNotification(
    __in PDEVICE_OBJECT Fdo,
    __in PIRP Irp
    );

NTSTATUS
SdbusInitializeAdma(
    __inout PSD_SOCKET Socket,
    __in PDEVICE_OBJECT DeviceObject,
    __in ULONG BusNumber
    );

NTSTATUS
SdbusInitializeDbDma(
    __inout PSD_SOCKET Socket,
    __in PDEVICE_OBJECT DeviceObject,
    __in ULONG BusNumber
    );

NTSTATUS
SdbusInitializeSystemDma(
    __inout PSD_SOCKET Socket,
    __in PDEVICE_OBJECT DeviceObject,
    __in ULONG BusNumber
    );

NTSTATUS
SdbusInitializeTransfer(
    __inout PSD_SOCKET Socket,
    __in PDEVICE_OBJECT DeviceObject,
    __in ULONG BusNumber
    );

NTSTATUS
SdbusEnableRuntimePowerManagement(
    __in PFDO_EXTENSION FdoExtension,
    __in BOOLEAN        AllSocketsNonRemovable
    );

NTSTATUS
SdbusAcpiQueryRemovability(
    __in PFDO_EXTENSION FdoExtension
    );

NTSTATUS
SdbusQueryD3ColdInterface(
    __in    PFDO_EXTENSION              FdoExtension,
    __out   PD3COLD_SUPPORT_INTERFACE   D3ColdInterface
    );

NTSTATUS
SdbusGetDeepestWakeableDstate(
    __in    PFDO_EXTENSION              FdoExtension
    );

extern UNICODE_STRING  DriverRegistryPath;

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, SdbusFdoPnpDispatch)
    #pragma alloc_text(PAGE, SdbusFdoStartDevice)
    #pragma alloc_text(PAGE, SdbusFdoStartDeviceWorker)
    #pragma alloc_text(PAGE, SdbusFdoRemoveDevice)
    #pragma alloc_text(PAGE, SdbusFdoDeviceCapabilities)
    #pragma alloc_text(PAGE, SdbusFdoQueryBusInformation)    
    #pragma alloc_text(PAGE, SdbusAddDevice)
    #pragma alloc_text(PAGE, SdbusGetPciControllerType)
    #pragma alloc_text(PAGE, SdbusGetAcpiControllerType)    
    #pragma alloc_text(PAGE, SdbusDeviceRelations)
    #pragma alloc_text(PAGE, SdbusQueryD3ColdInterface)
    #pragma alloc_text(PAGE, SdbusGetDeepestWakeableDstate)
    #pragma alloc_text(PAGE, SdbusFdoDeviceUsageNotification)
#endif



NTSTATUS
SdbusFdoPnpDispatch(
    PDEVICE_OBJECT DeviceObject,
    PIRP Irp
    )

/*++

Routine Description:

    PNP/Power IRPs dispatch routine for the sd bus controller

Arguments:

    DeviceObject - Pointer to the device object.
 
    Irp - Pointer to the IRP

Return Value:

    Status

--*/

{
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PFDO_EXTENSION fdoExtension = DeviceObject->DeviceExtension;
    NTSTATUS status;

    PAGED_CODE();

    NT_ASSERT (fdoExtension->LowerDevice != NULL);

    status = IoAcquireRemoveLock(&fdoExtension->RemoveLock, Irp);
    if (!NT_SUCCESS(status)) {
        Irp->IoStatus.Status = status;
        PoStartNextPowerIrp (Irp);
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        return status;
    }

    if (irpStack->MinorFunction > IRP_MN_PNP_MAXIMUM_FUNCTION) {
        DebugPrint((SDBUS_DEBUG_PNP,
                    "fdo %p irp %p - Unknown PNP irp %d",
                    DeviceObject, 
                    Irp, 
                    irpStack->MinorFunction));
    } else {
        DebugPrint((SDBUS_DEBUG_PNP,
                    "fdo %p irp %p --> %s",
                    DeviceObject, 
                    Irp, 
                    PNP_IRP_STRING(irpStack->MinorFunction)));
    }

    SdbusAddFxDbgLog(fdoExtension,
                     'PnpF',
                     irpStack->MinorFunction,
                     fdoExtension->Flags,
                     fdoExtension->PnpState);

    switch (irpStack->MinorFunction) {

    case IRP_MN_START_DEVICE:

        //
        // If the controller receives a start device IRP after it's already been
        // started just pass the request down.
        //

        if (fdoExtension->PnpState == SDBUS_DEVICE_STARTED) {
            status = Irp->IoStatus.Status;
            break;
        }

        //
        // Send this down to the PDO first
        //

        if (SdUseSyncStartDevice) {

            //
            //  We allow sync start device because it's impossible
            //  to read the debug tracing if we allow async StartDevice.
            //
            
            IoCopyCurrentIrpStackLocationToNext(Irp);
            status = IoSynchronousCallDriver(fdoExtension->LowerDevice, Irp);
            if (!NT_SUCCESS(status)) {

                DebugPrint((SDBUS_DEBUG_FAIL,
                            "fdo %p lower device can't be started 0x%x!",
                            DeviceObject, 
                            status));
            } else {

                status = SdbusFdoStartDevice(DeviceObject, Irp);
            }

            Irp->IoStatus.Status = status;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            IoReleaseRemoveLock(&fdoExtension->RemoveLock, Irp);
            return status;            
        }

        IoMarkIrpPending(Irp);
        IoCopyCurrentIrpStackLocationToNext(Irp);
        IoSetCompletionRoutine(Irp,
                               SdbusFdoStartDeviceComplete,
                               NULL,
                               TRUE,
                               TRUE,
                               TRUE);
    
        IoCallDriver(fdoExtension->LowerDevice, Irp);

        return STATUS_PENDING;

    case IRP_MN_QUERY_STOP_DEVICE:
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_CANCEL_STOP_DEVICE:
        if (fdoExtension->PnpState == SDBUS_DEVICE_STOP_PENDING) {
            RESTORE_PREVIOUS_PNP_STATE(fdoExtension);
        }
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_STOP_DEVICE:
        SdbusFdoStopDevice(DeviceObject);
        status = STATUS_SUCCESS;
        SET_NEW_PNP_STATE(fdoExtension, SDBUS_DEVICE_STOPPED);
        break;


    case IRP_MN_QUERY_REMOVE_DEVICE:
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_CANCEL_REMOVE_DEVICE:
        if (fdoExtension->PnpState == SDBUS_DEVICE_REMOVE_PENDING) {
            RESTORE_PREVIOUS_PNP_STATE(fdoExtension);
        }
        status = STATUS_SUCCESS;
        break;

    case IRP_MN_SURPRISE_REMOVAL:

        SdbusFdoStopDevice(DeviceObject);
        SdbusFdoFreeQueuedPackets(DeviceObject);

        status = STATUS_SUCCESS;
        SET_NEW_PNP_STATE(fdoExtension, SDBUS_DEVICE_SURPRISE_REMOVE_PENDING);
        break;

    case IRP_MN_REMOVE_DEVICE:

        DebugPrint((SDBUS_DEBUG_PNP,
                    "fdo %p Start waiting for remove lock",
                    DeviceObject));

        //
        // We don't grap FDO's remove lock for any requests, it is safe to do
        // that here before draining the queue.
        // 

        IoReleaseRemoveLockAndWait(&fdoExtension->RemoveLock, Irp);

        DebugPrint((SDBUS_DEBUG_PNP,
                    "fdo %p Got the remove lock",
                    DeviceObject));

        if (fdoExtension->PnpState == SDBUS_DEVICE_STARTED) {

            //
            // Stop the fdo first.
            //

            SdbusFdoStopDevice(DeviceObject);
        }

        SdbusFdoFreeQueuedPackets(DeviceObject);

        Irp->IoStatus.Status = STATUS_SUCCESS;
        IoSkipCurrentIrpStackLocation(Irp);
        status = IoCallDriver(fdoExtension->LowerDevice, Irp);

        SdbusFdoRemoveDevice(DeviceObject);
        return status;


    case IRP_MN_QUERY_DEVICE_RELATIONS:
        
        //
        // Return the list of devices on the bus
        //
        
        status = SdbusDeviceRelations(
                        DeviceObject,
                        Irp,
                        irpStack->Parameters.QueryDeviceRelations.Type,
                        (PDEVICE_RELATIONS *) &Irp->IoStatus.Information);

        if (!NT_SUCCESS(status) && (status != STATUS_NOT_SUPPORTED)) {
            Irp->IoStatus.Status = status;
            IoCompleteRequest(Irp, IO_NO_INCREMENT);
            IoReleaseRemoveLock(&fdoExtension->RemoveLock, Irp);
            return status;
        }
        break;


    case IRP_MN_QUERY_CAPABILITIES:
        status = SdbusFdoDeviceCapabilities(DeviceObject, Irp);
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        IoReleaseRemoveLock(&fdoExtension->RemoveLock, Irp);
        return status;

    case IRP_MN_DEVICE_USAGE_NOTIFICATION:
        status = SdbusFdoDeviceUsageNotification(DeviceObject, Irp);
        Irp->IoStatus.Status = status;
        IoCompleteRequest(Irp, IO_NO_INCREMENT);
        IoReleaseRemoveLock(&fdoExtension->RemoveLock, Irp);
        return status;

    default:
        DebugPrint((SDBUS_DEBUG_PNP,
                    "fdo %p irp %p - Skipping unsupported irp: minor func=%d",
                    DeviceObject, 
                    Irp,
                    irpStack->MinorFunction));

        status = Irp->IoStatus.Status;
        break;
    }

    //
    // Set the IRP status only if we set it to something other than
    // STATUS_NOT_SUPPORTED.
    //

    if (status != STATUS_NOT_SUPPORTED) {
        Irp->IoStatus.Status = status ;
    }

    DebugPrint((SDBUS_DEBUG_PNP,
                "fdo %p Forward irp %p",
                DeviceObject, 
                Irp));

    IoSkipCurrentIrpStackLocation (Irp);
    status = IoCallDriver(fdoExtension->LowerDevice, Irp);

    IoReleaseRemoveLock(&fdoExtension->RemoveLock, Irp);

    return status;
}



NTSTATUS
SdbusAddDevice(
    __in PDRIVER_OBJECT DriverObject,
    __in PDEVICE_OBJECT Pdo
    )

/*++

Routine Description:

    This routine creates functional device objects for each SD controller in
    the system and attaches them to the physical device objects for the
    controllers


Arguments:

    DriverObject - a pointer to the object for this driver
 
    PhysicalDeviceObject - a pointer to the physical object we need to attach
    to

Return Value:

    Status from device creation and initialization

--*/

{
    LONG deviceNumber = 0;
    PDEVICE_OBJECT fdo = NULL;
    PDEVICE_OBJECT lowerDevice = NULL;
    PFDO_EXTENSION fdoExtension;
    UNICODE_STRING deviceNameString = {0};
    WCHAR deviceNameBuffer[100];
    NTSTATUS status;

    PAGED_CODE();

    DebugPrint((SDBUS_DEBUG_PNP,
                "AddDevice Entered with pdo %p",
                Pdo));

    if (Pdo == NULL) {

        //
        // Have we been asked to do detection on our own?
        // if so just return no more devices
        //

        DebugPrint((SDBUS_DEBUG_FAIL,
                    "SdbusAddDevice - asked to do detection"));
        return STATUS_NO_MORE_ENTRIES;
    }

    //
    // Attempt to create the device with a unique name
    //

    do {

        status = RtlStringCbPrintfW(deviceNameBuffer,
                                    sizeof(deviceNameBuffer),
                                    L"%s%d",
                                    L"\\Device\\SdBus-HC",
                                    deviceNumber);
        if (!NT_SUCCESS(status)) {
            return status;
        }

        RtlInitUnicodeString(&deviceNameString, deviceNameBuffer);
        
        //
        // create and initialize the new functional device object
        //

        status = IoCreateDevice(DriverObject,
                                sizeof(FDO_EXTENSION),
                                &deviceNameString,
                                FILE_DEVICE_CONTROLLER,
                                0L,
                                FALSE,
                                &fdo);

        if (NT_SUCCESS(status)) {
            break;
        }

        deviceNumber++;

    } while((status == STATUS_OBJECT_NAME_EXISTS) ||
            (status == STATUS_OBJECT_NAME_COLLISION));

    if (!NT_SUCCESS(status)) {

        DebugPrint((SDBUS_DEBUG_FAIL,
                    "SdbusAddDevice - error creating Fdo [%#08lx]", 
                    status));
        return status;
    }

    fdoExtension = (PFDO_EXTENSION)fdo->DeviceExtension;

    RtlZeroMemory(fdoExtension, sizeof(FDO_EXTENSION));

    //
    // Set up the device extension.
    //

    fdoExtension->DeviceNumber     = deviceNumber;
    fdoExtension->Signature        = SDBUS_FDO_EXTENSION_SIGNATURE;
    fdoExtension->DeviceObject     = fdo;
    fdoExtension->RegistryPath     = &DriverRegistryPath;
    fdoExtension->DriverObject     = DriverObject;
    fdoExtension->Flags            = SDBUS_FDO_OFFLINE;
    fdoExtension->WaitWakeState    = WAKESTATE_DISARMED;
    fdoExtension->CardDetectMethod = SDBUS_CARD_DETECT_METHOD_UNSPECIFIED;

#if defined (TARGETING_Win2K)
    SdbusDbgInitTracing(fdo, fdoExtension->RegistryPath);
    SetDeviceFlag(fdoExtension, SDBUS_FDO_WPP_INITIALIZED);
#endif

    SdbusInitializeFxDbgLog(fdoExtension);

    IoInitializeRemoveLock(&fdoExtension->RemoveLock, 
                           SDBUS_TAG_LOCK, 
                           1,
                           100);

    //
    // Layer our FDO on top of the PDO
    //

    lowerDevice = IoAttachDeviceToDeviceStack(fdo, Pdo);

    //
    // No status. Do the best we can.
    //

    if (lowerDevice == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    fdoExtension->LowerDevice = lowerDevice;
    fdoExtension->Pdo = Pdo;

    //
    // Determine the bus type
    //

    status = SdbusFdoQueryBusInformation(Pdo, fdoExtension);
    if (!NT_SUCCESS(status)) {
        DebugPrint((SDBUS_DEBUG_FAIL,
                    "fdo %p IRP_MN_QUERY_BUS_INFORMATION - error(%08x) from SdbusFdoQueryBusInformation!",
                    fdo, status));
        goto Cleanup;        
    }

    //
    // Depending on what bus enumerator we're on top of, gather some information.
    //

    if (IS_PCI_DEVICE(fdoExtension)) {
        
        //
        // Get the pci interface for reading/writing to config header space
        //

        status = SdbusGetInterface(Pdo,
                                   &GUID_BUS_INTERFACE_STANDARD,
                                   sizeof(BUS_INTERFACE_STANDARD),
                                   fdoExtension);
        if (!NT_SUCCESS(status)) {
            DebugPrint((SDBUS_DEBUG_FAIL,
                        "fdo %p AddDevice - error(%08x) from GetInterface!",
                        fdo, status));
            goto Cleanup;
        }

        //
        // Get our controller type and set the function block so that SDPORT_xxx
        // functions will work.
        //

        status = SdbusGetPciControllerType(Pdo, fdo);
        if (!NT_SUCCESS(status)) {
            DebugPrint((SDBUS_DEBUG_FAIL,
                        "fdo %p AddDevice - error(%08x) from GetControllerType!",
                        fdo, 
                        status));
            goto Cleanup;
        }        
        
    } else if (IS_ACPI_DEVICE(fdoExtension)) {

        //
        // Get our controller type and set the function block so that SDPORT_xxx
        // functions will work.
        //
        
        status = SdbusGetAcpiControllerType(Pdo, fdo);
        if (!NT_SUCCESS(status)) {
            DebugPrint((SDBUS_DEBUG_FAIL,
                        "fdo %p AddDevice - error(%08x) from GetControllerType!",
                        fdo, 
                        status));
            goto Cleanup;
        }
        
    } else {
    
        //
        // If it isn't PCI, and it isn't ACPI, then we're in trouble, but assume the 
        // default just in case. Maybe it's HAL enumerated, which is not really
        // supported any more.
        //

        NT_ASSERT(FALSE && L"HAL enumerated devices not supported.");

        fdoExtension->FunctionBlock = &SdhcSupportFns;
        
    }
    
    //
    // Link this fdo to the list of fdo's managed by the driver
    //

    DebugPrint((SDBUS_DEBUG_PNP,
                "FDO %p now linked to fdolist by AddDevice", 
                fdo));

    fdoExtension->NextFdo = FdoList;
    FdoList = fdo;
    status = ExInitializeLookasideListEx(&fdoExtension->WorkPacketLookasideList,
                                            NULL,
                                            NULL,
                                            NonPagedPoolNx,
                                            0,
                                            sizeof(SD_WORK_PACKET),
                                            SDBUS_TAG_WORK_PACKET,
                                            0);

    NT_ASSERT(NT_SUCCESS(status));

    INITIALIZE_PNP_STATE(fdoExtension);

    fdo->Flags &= ~DO_DEVICE_INITIALIZING;

    IoRegisterShutdownNotification(fdo);

Cleanup:

    if (!NT_SUCCESS(status)) {

        SET_NEW_PNP_STATE(fdoExtension, SDBUS_DEVICE_DELETED);

        //
        // Delete the bus driver interface for manipulating config space.
        //

        SdbusDeleteInterface(fdoExtension);

        //
        // Cannot support a controller without knowing its type etc.
        //

        if (fdoExtension->LowerDevice) {
            IoDetachDevice(fdoExtension->LowerDevice);
        }

        IoDeleteDevice(fdo);

    }
    return status;
}

NTSTATUS
SdbusGetPciControllerType(
    __in PDEVICE_OBJECT Pdo,
    __inout PDEVICE_OBJECT Fdo
    )

/*++

Routine Description: 
 
    Look at the PCI hardware ID to see if it is already a device we know
    about. If so, set the appropriate controller type in the fdoExtension.

Arguments: 
 
    Pdo - Supplies the physical Device object for the Sdbus controller owned
          by the PCI driver
 
    Fdo - Supplies functional Device object for the sd controller owned by
          this driver, whose extension will store the relevant controller
          information upon exit from this routine.

Return Value: 
 
    STATUS_SUCCESS - Things are fine and information obtained
 
    STATUS_NOT_SUPPORTED - This is actually a healthy status for this routine:
                           all it means is that this PDO is not on a PCI bus,
                           so no information needs to be obtained anyways. Any
                           other status Failure. Caller probably needs to back
                           out & not support this controller
--*/

{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PCI_COMMON_CONFIG pciConfig = {0};
    NTSTATUS status;

    PAGED_CODE();

    //
    // read our config space
    //

    status = GetPciConfigSpace(fdoExtension,
                               0,
                               (PUCHAR)&pciConfig,
                               sizeof(pciConfig));

    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // remember the device ID for hacks
    //

    fdoExtension->PciControllerVendorID     = pciConfig.VendorID;
    fdoExtension->PciControllerDeviceID     = pciConfig.DeviceID;
    fdoExtension->PciControllerRevisionID   = pciConfig.RevisionID;
    fdoExtension->PciControllerSubsysID     = pciConfig.u.type0.SubSystemID << 16 |
                                              pciConfig.u.type0.SubVendorID;

    //
    // Set up flags to indicate compatibility behavior
    //

    fdoExtension->CompatType = SDHC_COMPAT_TYPE_STANDARD;


    //
    // Now fill in the port function table.
    //
    
    //
    // Look to see if our standard class code is present
    //

    if ((pciConfig.BaseClass == 8) && (pciConfig.SubClass == 5)) {
        
        fdoExtension->FunctionBlock = &SdhcSupportFns;
        
    } else {

        //
        // Didn't find our class, try to just base it on the vendor id
        //

        switch(pciConfig.VendorID) {

        case PCI_TOSHIBA_VENDORID:

            // 
            // In Win7, we don't support toshiba controller anymore.
            // 
            
            status = STATUS_NOT_SUPPORTED;
            break;

        case PCI_TED_VENDORID:

            fdoExtension->FunctionBlock = &SdhcSupportFns;
            status = STATUS_SUCCESS;
            break;

        default:
            status = STATUS_UNSUCCESSFUL;
        }
        
    }

    if (fdoExtension->CompatType == SDHC_COMPAT_TYPE_STANDARD) {
        status = SdbusGetControllerRegistrySettings(Pdo, Fdo);
    }
    return status;
}


NTSTATUS
SdbusGetAcpiControllerType(
    __in PDEVICE_OBJECT Pdo,
    __inout PDEVICE_OBJECT Fdo
    )

/*++

Routine Description: 
 
    Set the appropriate controller type in the fdoExtension.

Arguments: 
 
    Pdo - Supplies the physical Device object for the Sdbus controller owned
          by the ACPI driver
 
    Fdo - Supplies functional Device object for the sd controller owned by
          this driver, whose extension will store the relevant controller
          information upon exit from this routine.

Return Value: 
 
    STATUS_SUCCESS - Things are fine and information obtained
 
--*/

{
    NTSTATUS            Status = STATUS_SUCCESS;

    PAGED_CODE();

    Status = SdbusGetControllerRegistrySettings(Pdo, Fdo);

    return Status;
}

NTSTATUS
SdbusFdoQueryBusInformation(
    __inout PDEVICE_OBJECT Pdo,
    __inout PFDO_EXTENSION FdoExtension    
    )
{
    NTSTATUS            Status = STATUS_SUCCESS;
    ULONG               BytesReturned = 0;

    RtlZeroMemory(&FdoExtension->BusInformation, sizeof(PNP_BUS_INFORMATION));

    Status = IoGetDeviceProperty(Pdo, DevicePropertyBusTypeGuid, sizeof(GUID), &FdoExtension->BusInformation.BusTypeGuid, &BytesReturned);
    if (!NT_SUCCESS(Status)) {
        goto exit;
    }

    Status = IoGetDeviceProperty(Pdo, DevicePropertyLegacyBusType, sizeof(INTERFACE_TYPE), &FdoExtension->BusInformation.LegacyBusType, &BytesReturned);
    if (!NT_SUCCESS(Status)) {
        goto exit;
    }

    Status = IoGetDeviceProperty(Pdo, DevicePropertyBusNumber, sizeof(ULONG), &FdoExtension->BusInformation.BusNumber, &BytesReturned);
    if (!NT_SUCCESS(Status)) {
        goto exit;
    }

exit:
    
    return Status;
}

NTSTATUS
SdbusFdoDeviceCapabilities(
    __inout PDEVICE_OBJECT Fdo,
    __inout PIRP Irp
    )

/*++

Routine Description
    Records the device capabilities of this sd controller,
    so  1. they can be used in the power management for the controller
    and 2. they can be used for determining the capabilities of the
           child pc-card PDO's of this sd controller.

Arguments

    Fdo - Pointer to functional device object of the sd controller
          
    Irp - Pointer to the i/o request packet

Return Value

    STATUS_SUCCESS - Capabilities returned
 
    STATUS_INSUFFICIENT_RESOURCES - Could not allocate memory to cache the
                                    capabilities

--*/

{
    PFDO_EXTENSION fdoExtension;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    PDEVICE_CAPABILITIES capabilities;
    NTSTATUS status;

    PAGED_CODE();

    capabilities = irpStack->Parameters.DeviceCapabilities.Capabilities;
    fdoExtension = Fdo->DeviceExtension;

    //
    // Send this down the irpStack to obtain the capabilities
    //

    status = SdbusIoCallDriverSynchronous(fdoExtension->LowerDevice, Irp);


    if (NT_SUCCESS(status)) {

        DebugPrint((SDBUS_DEBUG_PNP,
                    "fdo %p irp %p Received the device capabilities from bus driver",
                    Fdo,
                    Irp));

        //
        // Cache the device capabilities in the device extension
        // for this sd controller.
        //

        RtlCopyMemory(&fdoExtension->DeviceCapabilities,
                      capabilities,
                      sizeof(DEVICE_CAPABILITIES));

    } else {

        DebugPrint((SDBUS_DEBUG_PNP,
                    "fdo %p irp %p Failed to query the device capabilities",
                    Fdo,
                    Irp));

        RtlZeroMemory(&fdoExtension->DeviceCapabilities, 
                      sizeof(DEVICE_CAPABILITIES));

    }

    return status;
}



NTSTATUS
SdbusInitializeTransfer(
    __inout PSD_SOCKET Socket,
    __in PDEVICE_OBJECT DeviceObject,
    __in ULONG BusNumber
    )
{
    ULONG reservedSize = Socket->Settings.ReservedAddressSize + PAGE_SIZE;
    NTSTATUS status = STATUS_SUCCESS;

    Socket->ReservedAddress = MmAllocateMappingAddress(reservedSize,
                                                       SDBUS_TAG_MAPPING);
    if (!Socket->ReservedAddress) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    Socket->ReservedMdl = IoAllocateMdl(NULL,
                                        reservedSize,
                                        FALSE,
                                        FALSE,
                                        NULL);
    if (!Socket->ReservedMdl) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Determine which transfer method will be used for the current Socket
    //

    Socket->TransferMethod = SDBUS_TRANSFER_METHOD_UNSPECIFIED;

    if (Socket->Settings.TransferMethod == SDBUS_TRANSFER_METHOD_UNSPECIFIED) {
        if (Socket->Capabilities.SupportAdma2) {
            Socket->TransferMethod = SDBUS_TRANSFER_METHOD_ADMA2;
        } else if (Socket->Capabilities.SupportDma) {
            Socket->TransferMethod = SDBUS_TRANSFER_METHOD_DB_DMA;            
        } else {
            Socket->TransferMethod = SDBUS_TRANSFER_METHOD_PIO;
        }
    } else if (Socket->Settings.TransferMethod == SDBUS_TRANSFER_METHOD_ADMA2) {
        if (Socket->Capabilities.SupportAdma2) {
            Socket->TransferMethod = SDBUS_TRANSFER_METHOD_ADMA2;    
        } else {
            Socket->TransferMethod = SDBUS_TRANSFER_METHOD_PIO;    
        }
    } else if (Socket->Settings.TransferMethod == SDBUS_TRANSFER_METHOD_DB_DMA) {
        if (Socket->Capabilities.SupportDma) {
            Socket->TransferMethod = SDBUS_TRANSFER_METHOD_DB_DMA;    
        } else {
            Socket->TransferMethod = SDBUS_TRANSFER_METHOD_PIO;    
        }
    } else {
        Socket->TransferMethod = SDBUS_TRANSFER_METHOD_PIO;    
    }

    NT_ASSERT(Socket->TransferMethod != SDBUS_TRANSFER_METHOD_UNSPECIFIED);


    if (Socket->TransferMethod == SDBUS_TRANSFER_METHOD_ADMA2) {
        status = SdbusInitializeAdma(Socket,
                                     DeviceObject,
                                     BusNumber);

    } else if (Socket->TransferMethod == SDBUS_TRANSFER_METHOD_DB_DMA) {
        status = SdbusInitializeDbDma(Socket,
                                      DeviceObject,
                                      BusNumber);
        
    } else if (Socket->TransferMethod == SDBUS_TRANSFER_METHOD_SYSTEM_DMA) {
        status = SdbusInitializeSystemDma(Socket,
                                          DeviceObject,
                                          BusNumber);
    }

    if (!NT_SUCCESS(status)) {
        Socket->TransferMethod = SDBUS_TRANSFER_METHOD_PIO;
    }

    if (Socket->TransferMethod == SDBUS_TRANSFER_METHOD_PIO) {
        Socket->DmaPhysicalAddr.QuadPart = 0;
        Socket->DmaLength = 0;
        Socket->DmaVirtualAddr = NULL;
    }

    status = SDPORT_InitSocketDma(Socket);
    return status;
}

#define HAL_MAXIMUM_DMA_MAP_REGISTERS 64

NTSTATUS
SdbusInitializeAdma(
    __inout PSD_SOCKET Socket,
    __in PDEVICE_OBJECT DeviceObject,
    __in ULONG BusNumber
    )
{
    NTSTATUS            Status = STATUS_SUCCESS;    
    DEVICE_DESCRIPTION  deviceDescription;

    PDMA_ADAPTER        DmaAdapter = NULL;

    PVOID               DmaVirtualAddr = NULL;
    PHYSICAL_ADDRESS    DmaPhysicalAddr = {0,0};

    ULONG               DmaScatterGatherListSize = 0; 
    ULONG               MapRegisterCount = 0;
    ULONG               DmaScatterGatherListMultiplier = 1;

    PVOID               DmaScatterGatherBuffer = NULL;

    //
    // Allocate DMA adapter
    //

    RtlZeroMemory(&deviceDescription, sizeof(deviceDescription));

    deviceDescription.Version = DEVICE_DESCRIPTION_VERSION3;
    deviceDescription.Master = TRUE;

    //
    // We need scatter gather list for ADMA
    // 

    deviceDescription.ScatterGather = TRUE;
    deviceDescription.IgnoreCount = FALSE;

    //
    // Making a decision for the mini-port
    // 

    deviceDescription.BusNumber = BusNumber;
    deviceDescription.InterfaceType = (IS_PCI_DEVICE(Socket->FdoExtension)) ? PCIBus : ACPIBus;
    deviceDescription.MaximumLength = Socket->Settings.MaxTransferLength;

    deviceDescription.Dma32BitAddresses = TRUE;                                 // deprecated in V3
    deviceDescription.Dma64BitAddresses = Socket->Capabilities.Support64Bus;    // deprecated in V3
    deviceDescription.DmaAddressWidth = (Socket->Capabilities.Support64Bus) ? 64 : 32;

    DmaAdapter = IoGetDmaAdapter(DeviceObject,
                                 &deviceDescription,
                                 &Socket->DmaMaxMapRegs);
    if (!DmaAdapter) {
        DebugPrint((SDBUS_DEBUG_FAIL,
                    "Error: StartDevice - IoGetDmaAdapter returned NULL for ADMA"));

        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    //
    // Allocate the DMA adapter with a MaximumLength based on the number of
    // map registers allocated on the first call to IoGetDmaAdapter.  This will help
    // limit the number of low memory situations on calls to GetScatterGatherList
    // as there are issues in the low-memory condition code.
    //
    // N.B. It might be useful to instead implement a BuildScatterGatherList approach
    // for ADMA2 so that we could ensure that we never enter low-memory mode as
    // the memory for the scatter-gather list has been preallocated.
    //

    DmaAdapter->DmaOperations->PutDmaAdapter(DmaAdapter);
    DmaAdapter = NULL;

    //
    // Optimal maximum transfer length for eMMC devices for sequential 
    // throughput is 1MB.
    //
    
    Socket->Settings.MaxTransferLength = 4096 * 256;
    deviceDescription.MaximumLength = Socket->Settings.MaxTransferLength;

    DmaAdapter = IoGetDmaAdapter(DeviceObject,
                                 &deviceDescription,
                                 &Socket->DmaMaxMapRegs);

    if (!DmaAdapter) {
        
        DebugPrint((SDBUS_DEBUG_FAIL, "Error: StartDevice - IoGetDmaAdapter returned NULL for ADMA"));       
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }
    
    Socket->DmaMaxMapRegs = 
                MIN(Socket->DmaMaxMapRegs, 
                    BYTES_TO_PAGES(Socket->Settings.MaxTransferLength) + 3);

    //
    // DMA length is the size of descriptor table--the table needs as many
    // entries as there are possible pages in the I/O.
    // 

    Socket->DmaLength = 
        (BYTES_TO_PAGES(Socket->Settings.MaxTransferLength) + 3) * 
        Socket->Capabilities.DescriptorSize;

    DmaVirtualAddr = DmaAdapter->DmaOperations->AllocateCommonBuffer(
                                                DmaAdapter,
                                                Socket->DmaLength,
                                                &DmaPhysicalAddr,
                                                FALSE);
    
    if (!DmaVirtualAddr) {

        DebugPrint((SDBUS_DEBUG_FAIL,
                    "Error: StartDevice - failure to allocate ADMA2 descriptor buffer!"));
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    DebugPrint((SDBUS_DEBUG_INFO,
                "StartDevice: allocated ADMA common buffer: bufsize=0x%x, map regs=%d, max length=%d",
                Socket->DmaLength,
                Socket->DmaMaxMapRegs,
                Socket->Settings.MaxTransferLength));

    //
    // Allocate our scatter gather list buffer. We need to preallocate our worst case largest
    // possible SG list, so we never overrun it or fall back to low memory mode.
    //

    DmaAdapter->DmaOperations->CalculateScatterGatherList(
                                                    DmaAdapter,
                                                    NULL,
                                                    0,
                                                    Socket->DmaMaxMapRegs * PAGE_SIZE,
                                                    &DmaScatterGatherListSize,
                                                    &MapRegisterCount);

    //
    // N.B. In order to get a transfer size of 1MB, we have to effectively
    // ignore the scatter gather list size returned by
    // CalculateScatterGatherList because its calculation is gated by the number
    // of map registers that the HAL has allocated to us through
    // IoGetDmaAdapter.
    //

    NT_ASSERT(MapRegisterCount >= HAL_MAXIMUM_DMA_MAP_REGISTERS);

    DmaScatterGatherListMultiplier = 
        MAX(Socket->Settings.MaxTransferLength / (Socket->DmaMaxMapRegs << PAGE_SHIFT), 1);

    DmaScatterGatherListSize = DmaScatterGatherListSize * DmaScatterGatherListMultiplier + 3 * sizeof(SCATTER_GATHER_ELEMENT);

    DmaScatterGatherBuffer = ExAllocatePoolWithTag( NonPagedPoolNx, DmaScatterGatherListSize, SDBUS_TAG_DMA_SG_LIST );
    if (!DmaScatterGatherBuffer) {

        DebugPrint((SDBUS_DEBUG_FAIL,
                    "Error: StartDevice - failure to allocate scatter gather buffer!"));
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    //
    // No failures below this point, transfer ownership into the socket structure.
    //

    Socket->DmaAdapter = DmaAdapter;
    DmaAdapter = NULL;

    Socket->DmaVirtualAddr = DmaVirtualAddr;
    Socket->DmaPhysicalAddr = DmaPhysicalAddr;
    DmaVirtualAddr = NULL;
    DmaPhysicalAddr.LowPart = 0;
    DmaPhysicalAddr.HighPart = 0;

    Socket->DmaScatterGatherBuffer = DmaScatterGatherBuffer;
    Socket->DmaScatterGatherListSize = DmaScatterGatherListSize;
    DmaScatterGatherBuffer = NULL;
    
exit:

    if (DmaScatterGatherBuffer) {
        ExFreePoolWithTag(DmaScatterGatherBuffer, SDBUS_TAG_DMA_SG_LIST);
        DmaScatterGatherBuffer = NULL;
    }

    if (DmaVirtualAddr) {
        DmaAdapter->DmaOperations->FreeCommonBuffer(
                                                    DmaAdapter,
                                                    Socket->DmaLength,
                                                    DmaPhysicalAddr,
                                                    DmaVirtualAddr,
                                                    TRUE);
        DmaPhysicalAddr.LowPart = 0;
        DmaPhysicalAddr.HighPart = 0;
        DmaVirtualAddr = NULL;
    }

    if (DmaAdapter) {        
        DmaAdapter->DmaOperations->PutDmaAdapter(DmaAdapter);
        DmaAdapter = NULL;
    }

    return Status;
}


NTSTATUS
SdbusInitializeDbDma(
    __inout PSD_SOCKET Socket,
    __in PDEVICE_OBJECT DeviceObject,
    __in ULONG BusNumber
    )
{
    DEVICE_DESCRIPTION deviceDescription;

    Socket->DmaLength = Socket->Settings.DmaBufferSize;
    Socket->DmaMaxMapRegs = 0;

    NT_ASSERT(Socket->DmaLength != 0);

    //
    // Allocate DMA adapter
    //

    RtlZeroMemory(&deviceDescription, sizeof(deviceDescription));

    deviceDescription.Version = DEVICE_DESCRIPTION_VERSION3;
    deviceDescription.Master = TRUE;
    deviceDescription.ScatterGather = FALSE;
    deviceDescription.IgnoreCount = FALSE;
    deviceDescription.BusNumber = BusNumber;
    deviceDescription.InterfaceType = (IS_PCI_DEVICE(Socket->FdoExtension)) ? PCIBus : ACPIBus;
    deviceDescription.MaximumLength = Socket->DmaLength;
    deviceDescription.Dma32BitAddresses = TRUE;     // deprecated in V3
    deviceDescription.Dma64BitAddresses = FALSE;    // deprecated in V3
    deviceDescription.DmaAddressWidth = 32;

    Socket->DmaAdapter = IoGetDmaAdapter(DeviceObject,
                                         &deviceDescription,
                                         &Socket->DmaMaxMapRegs);

    if (!Socket->DmaAdapter) {
        DebugPrint((SDBUS_DEBUG_FAIL,
                    "Error: StartDevice - IoGetDmaAdapter returned NULL for DBDMA"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    DebugPrint((SDBUS_DEBUG_INFO,
                "StartDevice: allocated DMA adapter: bufsize=%x",
                Socket->DmaLength));

    //
    // Allocate DMA buffer
    //

    Socket->DmaVirtualAddr = 
        Socket->DmaAdapter->DmaOperations->AllocateCommonBuffer(
                                                Socket->DmaAdapter,
                                                Socket->DmaLength,
                                                &Socket->DmaPhysicalAddr,
                                                FALSE);
    if (!Socket->DmaVirtualAddr) {

        Socket->DmaAdapter = NULL;

        DebugPrint((SDBUS_DEBUG_FAIL,
                    "Error: StartDevice - failure to allocate common buffer!"));
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    return STATUS_SUCCESS;
}


NTSTATUS
SdbusInitializeSystemDma(
    __inout PSD_SOCKET Socket,
    __in PDEVICE_OBJECT DeviceObject,
    __in ULONG BusNumber
    )
    
{
    DEVICE_DESCRIPTION DeviceDescription;
    PFDO_EXTENSION FdoExtension;
    ULONG ScatterGatherListMultiplier;
    NTSTATUS Status;

    Socket->DmaLength = Socket->Settings.DmaBufferSize;
    Socket->DmaMaxMapRegs = 0;
    Socket->DmaMaxMapRegsRx = 0;
    Socket->DmaMaxMapRegsTx = 0;

    NT_ASSERT(Socket->DmaLength != 0);

    KeInitializeSpinLock(&Socket->DmaCancelLock);
    Socket->Settings.MaxTransferLength = 4096 * 256;
    RtlZeroMemory(&DeviceDescription, sizeof(DeviceDescription));
    DeviceDescription.Version = DEVICE_DESCRIPTION_VERSION3;
    DeviceDescription.Master = FALSE;
    DeviceDescription.ScatterGather = FALSE;
    DeviceDescription.IgnoreCount = FALSE;
    DeviceDescription.BusNumber = BusNumber;
    DeviceDescription.InterfaceType = IS_PCI_DEVICE(Socket->FdoExtension) ? PCIBus : ACPIBus;
    DeviceDescription.MaximumLength = Socket->Settings.MaxTransferLength;
    DeviceDescription.DmaWidth = Width32Bits;
    DeviceDescription.Dma32BitAddresses = TRUE;     // deprecated in V3
    DeviceDescription.Dma64BitAddresses = FALSE;    // deprecated in V3
    DeviceDescription.DmaAddressWidth = 32;
    DeviceDescription.DeviceAddress = Socket->HostRegisterPhysicalBase;

    FdoExtension = Socket->FdoExtension;


    //
    // V3 DMA APIs require a transfer context.
    //

    Socket->DmaTransferContext = 
        ExAllocatePoolWithTag(NonPagedPoolNx,
                              DMA_TRANSFER_CONTEXT_SIZE_V1,
                              SDBUS_TAG_DMA);

    if (Socket->DmaTransferContext == NULL) {
        DebugPrint((SDBUS_DEBUG_FAIL, 
                    "Error: StartDevice - IoGetDmaAdapter returned NULL for System DMA"));
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    Socket->SystemDmaEnabled = TRUE;
    return Status;

Cleanup:
    
    if (Socket->DmaAdapter != NULL) {
        Socket->DmaAdapter->DmaOperations->PutDmaAdapter(Socket->DmaAdapter);
        Socket->DmaAdapter = NULL;
    }
    
    if (Socket->DmaAdapterRx != NULL) {
        Socket->DmaAdapterRx->DmaOperations->PutDmaAdapter(Socket->DmaAdapterRx);
        Socket->DmaAdapterRx = NULL;
    }
    
    if (Socket->DmaAdapterTx != NULL) {
        Socket->DmaAdapterTx->DmaOperations->PutDmaAdapter(Socket->DmaAdapterTx);
        Socket->DmaAdapterTx = NULL;
    }
    
    if (Socket->DmaTransferContext != NULL) {
        ExFreePoolWithTag(Socket->DmaTransferContext, SDBUS_TAG_DMA);
        Socket->DmaTransferContext = NULL;
    }
    
    return Status;
}

NTSTATUS
SdbusFdoStartDevice(
    __inout PDEVICE_OBJECT Fdo,
    __in PIRP Irp
    )

/*++

Routine Description:

    This routine will start the sd controller with the supplied resources.  
    The IRP is sent down to the pdo first, so PCI or whoever sits underneath 
    gets a chance to program the controller to decode the resources.

Arguments:

    Fdo - Functional device object of the sd controller
 
    Irp - Pointer to the i/o request packet
 
Return value:

    Status

--*/

{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PIO_STACK_LOCATION irpStack = IoGetCurrentIrpStackLocation(Irp);
    USHORT socketNumber;
    PCM_RESOURCE_LIST ResourceList = 
            irpStack->Parameters.StartDevice.AllocatedResources;
    PCM_RESOURCE_LIST translatedResourceList = 
            irpStack->Parameters.StartDevice.AllocatedResourcesTranslated;
    PCM_FULL_RESOURCE_DESCRIPTOR fullResourceDesc;
    PCM_PARTIAL_RESOURCE_LIST resourceList;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR resourceDescriptor;
    IO_CONNECT_INTERRUPT_PARAMETERS connectInterruptParameters;
    IO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS interruptActiveStateParameters;
    ULONG resourceCount = 0;
    ULONG translatedInterruptResourceCount = 0;
    ULONG rawInterruptResourceCount = 0;
    ULONG i;
    BOOLEAN gpioInterrupt = FALSE;
    BOOLEAN sharedInterrupt;
    KINTERRUPT_MODE interruptMode;
    NTSTATUS status;
    ULONG channelNumber = 0;
    PHANDLE handle;
    IO_STATUS_BLOCK ioStatus;
    OBJECT_ATTRIBUTES objectAttributes;
    DECLARE_UNICODE_STRING_SIZE(string, RESOURCE_HUB_PATH_SIZE);
    ULONG data = 0;
    BOOLEAN allSocketsNonRemovable = TRUE;

    PAGED_CODE();

    //
    // Give the hardware some time to settle after returning from the pdo
    //

    SdbusWait(256);

    //
    // Parse AllocatedResources & get IoPort/AttributeMemoryBase/IRQ info.
    //

    if ((ResourceList == NULL) || (ResourceList->Count <=0) ) {
        status = STATUS_UNSUCCESSFUL;
        DebugPrint((SDBUS_DEBUG_FAIL,
                    "fdo %p StartDevice - resourceList error!", 
                    Fdo));
        goto Cleanup;
    }

    fullResourceDesc = &translatedResourceList->List[0];
    resourceList = &fullResourceDesc->PartialResourceList;

    //
    // StartController will initialize NumberOfSockets
    //

    status = SDPORT_StartController(fdoExtension, 
                                    translatedResourceList);
    if (!NT_SUCCESS(status)) {
        DebugPrint((SDBUS_DEBUG_FAIL,
                    "fdo %p StartDevice - error(%08x) from StartController!",
                    Fdo, status));
        goto Cleanup;
    }

    //
    // If this is the first time through StartDevice, then allocate a
    // socket structure for each socket
    //

    if (fdoExtension->Socket[0] == NULL) {

        for (socketNumber = 0; 
             socketNumber < fdoExtension->NumberOfSockets; 
             socketNumber ++) {

            PSD_SOCKET socket;
            ULONG Index;

            socket = ExAllocatePoolWithTag(NonPagedPoolNx,
                                           sizeof(SD_SOCKET),
                                           SDBUS_TAG_SOCKET);
            if (!socket) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                goto Cleanup;
            }

            RtlZeroMemory(socket, sizeof(SD_SOCKET));

            fdoExtension->Socket[socketNumber] = socket;
            socket->Signature = SDBUS_SOCKET_SIGNATURE;
            socket->Index = socketNumber;
            socket->FdoExtension = fdoExtension;

            SdbusInitializeDbgLog(socket);
            KeInitializeSpinLock(&socket->SpinLock);
            KeInitializeEvent(&socket->CardInterruptCompleteEvent, NotificationEvent, TRUE);

            //
            // We need two reserved work packets and contexts to handle 
            // activate socket requests.
            //

            InitializeListHead(&socket->ReservedWorkPacketList);
            InitializeListHead(&socket->ReservedActivateContextList);
            for (Index = 0; Index < SDBUS_RESERVED_WORK_PACKET_COUNT; Index += 1) {
                socket->ReservedWorkPackets[Index].ReservedPacket = TRUE;            
                InsertTailList(
                    &socket->ReservedWorkPacketList,
                    &socket->ReservedWorkPackets[Index].ReservedWorkPacketList);

                socket->ReservedActivateContexts[Index].ReservedContext = TRUE;
                InsertTailList(&socket->ReservedActivateContextList,
                               &socket->ReservedActivateContexts[Index].ReservedActivateContextList);
            }
            
            status = SdbusWorkerInitialize(&socket->WorkEngine);
            socket->WorkEngine.Socket = socket;
            if (!NT_SUCCESS(status)) {
                DebugPrint((SDBUS_DEBUG_FAIL,
                            "fdo %p StartDevice - error(%08x) from InitializeWorkEngine!",
                            Fdo,
                            status));
                goto Cleanup;
            }
        }
    }

    //
    // Query the ACPI namespace for SDHC children and update the NonremovableDevice
    // property for each of the available sockets under the given controller.
    //

    status = SdbusAcpiQueryRemovability(fdoExtension);

    if (!NT_SUCCESS(status)) {

        //
        // We could not determine removability, and have fallen back to safe bet setting.
        // Not a big enough issue to consider failure and to go to Cleanup, so just log it.
        //

        DebugPrint((SDBUS_DEBUG_FAIL,
                    "fdo %p StartDevice - error(%08x) from AcpiQueryRemovability!",
                    Fdo, 
                    status));
                    
        status = STATUS_SUCCESS;

    }

    //
    // Now run a StartSocket on each socket
    //

    for (socketNumber = 0;
         socketNumber < fdoExtension->NumberOfSockets; 
         socketNumber ++) {

        PSD_SOCKET socket = fdoExtension->Socket[socketNumber];

        //
        // Get various control settings
        //

        status = SdbusGetSocketRegistrySettings(fdoExtension->Pdo,
                                                    socket);
        if (!NT_SUCCESS(status)) {
            DebugPrint((SDBUS_DEBUG_FAIL,
                        "fdo %p StartDevice - error(%08x) from GetControllerRegistrySettings!",
                        Fdo,
                        status));
            goto Cleanup;
        }

        //
        //  Check if sdbus is disabled on this controller
        //

        if (socket->Settings.Disabled) {
            status = STATUS_NOT_SUPPORTED;
            goto Cleanup;
        }

        status = SDPORT_StartSocket(socket, translatedResourceList);

        if (!NT_SUCCESS(status)) {
            DebugPrint((SDBUS_DEBUG_FAIL,
                        "fdo %p StartDevice - error(%08x) from StartSocket!",
                        Fdo, 
                        status));
            goto Cleanup;
        }
    }

    allSocketsNonRemovable = TRUE;

    for (socketNumber = 0;
         socketNumber < fdoExtension->NumberOfSockets; 
         socketNumber ++) {

        PSD_SOCKET socket = fdoExtension->Socket[socketNumber];

        if (socket->Settings.NonremovableDevice == FALSE) {

            allSocketsNonRemovable = FALSE;
            break;
        }
    }

    //
    // See if an IRQ is assigned. The first interrupt is the standard interrupt.
    // A second optional interrupt resource may be provided, representing a
    // GPIO card detect interrupt line on SoC systems.
    //

    for (i = 0, resourceDescriptor = resourceList->PartialDescriptors, resourceCount = 0;
         i < resourceList->Count && resourceCount < 2;
         i++, resourceDescriptor++) {

        if (resourceDescriptor->Type == CmResourceTypeInterrupt) {

            DebugPrint((SDBUS_DEBUG_INFO,
                        "SdbusGetAssignedResources: Interrupt resource assigned"));

            if (resourceCount < 1) {

                //
                // This is our standard interrupt resource.
                //

                fdoExtension->TranslatedInterrupt = *resourceDescriptor;
                
            } else {

                //
                // This is an optional GPIO resource for card detect on SoC
                // systems.
                //

                fdoExtension->GpioTranslatedInterrupt = *resourceDescriptor;

                //
                // We should have no more interrupt resources. Ignore any
                // further ones in the loop condition.
                //
            }
            
            resourceCount++;
        }
    }

    translatedInterruptResourceCount = resourceCount;

    //
    // Get the raw interrupt resources.
    //

    fullResourceDesc = &ResourceList->List[0];
    resourceList = &fullResourceDesc->PartialResourceList;
    resourceDescriptor = resourceList->PartialDescriptors;

    for (i = 0, resourceDescriptor = resourceList->PartialDescriptors, resourceCount = 0;
         i < resourceList->Count && resourceCount < 2;
         i++, resourceDescriptor++) {

        if (resourceDescriptor->Type == CmResourceTypeInterrupt) {

            if (resourceCount < 1) {

                fdoExtension->Interrupt = *resourceDescriptor;
            } else {
            
                fdoExtension->GpioInterrupt = *resourceDescriptor;
            }

            resourceCount++;
        }
    }

    rawInterruptResourceCount = resourceCount;

    //
    // We should have at least one interrupt resource, otherwise we are screwed.
    // If we have a second interrupt resource, that's the GPIO interrupt.
    //

    NT_ASSERT((translatedInterruptResourceCount > 0) &&
              (rawInterruptResourceCount > 0) &&
              "No interrupt resource");

    if ((translatedInterruptResourceCount == 2) && (rawInterruptResourceCount == 2)) {

        gpioInterrupt = TRUE;
    }

    //
    // Check to see if we have any GPIO PIN resources. The first one should be
    // the card detect resource and the second should be the write protect
    // resource.
    //

    fullResourceDesc = &translatedResourceList->List[0];
    resourceList = &fullResourceDesc->PartialResourceList;
    resourceDescriptor = resourceList->PartialDescriptors;

    for (i = 0, resourceCount = 0; i < resourceList->Count; i++, resourceDescriptor++) {

        if (resourceDescriptor->Type == CmResourceTypeConnection) {

            //
            // Check the expected connection type.
            //

            if ((resourceDescriptor->u.Connection.Class == 
                 CM_RESOURCE_CONNECTION_CLASS_GPIO) &&
                (resourceDescriptor->u.Connection.Type == 
                 CM_RESOURCE_CONNECTION_TYPE_GPIO_IO)) {

                //
                // The first resource is card detect, second is write protect.
                //

                if (resourceCount < 1) {    
                    handle = &fdoExtension->GpioCardDetectHandle;
                } else {
                    handle = &fdoExtension->GpioWriteProtectHandle;
                }

                status = RESOURCE_HUB_CREATE_PATH_FROM_ID(
                            &string,
                            resourceDescriptor->u.Connection.IdLowPart,
                            resourceDescriptor->u.Connection.IdHighPart);

                if (!NT_SUCCESS(status)) {
                    NT_ASSERT(FALSE && "Unable to open handle to GPIO");
                    goto Cleanup;
                }

                InitializeObjectAttributes(&objectAttributes,
                                           &string,
                                           OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                           NULL,
                                           NULL);

                status = ZwOpenFile(handle,
                                    GENERIC_READ | SYNCHRONIZE,
                                    &objectAttributes,
                                    &ioStatus,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                                    FILE_NON_DIRECTORY_FILE | FILE_SYNCHRONOUS_IO_NONALERT);

                if (!NT_SUCCESS(status)) {
                    NT_ASSERT(FALSE && "Unable to open handle to GPIO");
                    goto Cleanup;
                }

                //
                // We only expect 2 GPIO I/O resources.
                //
                
                resourceCount += 1;
                if (resourceCount == 2) {
                    break;
                }
            }
        }
    }

    //
    // Check for ACPI wake support for card detect in D3Cold.
    //

    status = SdbusGetDeepestWakeableDstate(fdoExtension);

    //
    // Our preference for supporting card detect upon D3Cold is as follows:
    //
    // 1. if the device is non-removable, then we don't need card detect;
    //    TODO: note that 1. and 2. are currently swapped since not all firmwares support _RMV,
    //    and for Cardhu for example we will assume the external SD host controller as non-removable,
    //    thus turning off card detect. Once every firmware supports _RMV, 1. and 2. should be put in right order.
    // 2. if both GPIO interrupt and PIN resources are present, then the card detect PIN is not
    //    wired to the host register and a card change won't trigger a normal interrupt when the
    //    host controller is in D0, and note that wait-wake is only armed when the host controller
    //    is in D3. Thus we must use GPIO in this case;
    // 3. else, if the device is wake capable, then we prefer to use the wait-wake mechanism;
    // 4. else, if GPIO interrupt resource is present, then we should still use GPIO;
    // 5. otherwise, no wake capable, no GPIO interrupt resource. We can't support card detect.
    //

    if ((gpioInterrupt != FALSE) && (fdoExtension->GpioCardDetectHandle != NULL)) {

        fdoExtension->CardDetectMethod = SDBUS_CARD_DETECT_METHOD_GPIO;

    } else if (allSocketsNonRemovable != FALSE) {

        fdoExtension->CardDetectMethod = SDBUS_CARD_DETECT_METHOD_NOT_SUPPORTED;

    } else if (fdoExtension->DeepestWakeableDstate > DeviceWakeDepthD0) {

        fdoExtension->CardDetectMethod = SDBUS_CARD_DETECT_METHOD_WAIT_WAKE;
        fdoExtension->WaitWakeArmWorkItem = IoAllocateWorkItem(fdoExtension->DeviceObject);

        if (fdoExtension->WaitWakeArmWorkItem == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

    } else if (gpioInterrupt != FALSE) {

        fdoExtension->CardDetectMethod = SDBUS_CARD_DETECT_METHOD_GPIO;

    } else {

        fdoExtension->CardDetectMethod = SDBUS_CARD_DETECT_METHOD_NOT_SUPPORTED;
    }

    //
    // Allocate a work item to check eMMC write cache state on hibernate since the 
    // device doesn't go to D3 if we know we're handling an S4 IRP.
    //

    fdoExtension->CacheCheckWorkItem = IoAllocateWorkItem(fdoExtension->DeviceObject);
    if (fdoExtension->CacheCheckWorkItem == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

//@@BEGIN_DDKSPLIT
    //
    // TI OMAP35xx SDHC isn't a DMA master controller.  DMA transfers are only
    // supported throuh system DMA requests, so we have to see if a DMA channel
    // is assigned.
    //

    if (fdoExtension->CompatType == SDHC_COMPAT_TYPE_OMAP3 ||
        fdoExtension->CompatType == SDHC_COMPAT_TYPE_OMAP4) {
        
        for (i = 0, resourceDescriptor = resourceList->PartialDescriptors;
             i < resourceList->Count;
             i++, resourceDescriptor++) {

            if (resourceDescriptor->Type == CmResourceTypeDma) {

                //
                // A DMA channel is available.
                //  
                if (channelNumber == 0) {
                    fdoExtension->TranslatedDmaRx = *resourceDescriptor;
                } else {
                    fdoExtension->TranslatedDmaTx = *resourceDescriptor;
                }
                channelNumber++;
            }
        }

        //
        // Get the raw DMA channel resources to enable DMA.
        //
        
        channelNumber = 0;
        fullResourceDesc = &ResourceList->List[0];
        resourceList = &fullResourceDesc->PartialResourceList;
        resourceDescriptor = resourceList->PartialDescriptors;

        for (i = 0; i < resourceList->Count; i++, resourceDescriptor++) {
            if (resourceDescriptor->Type == CmResourceTypeDma) {
                if (channelNumber == 0) {
                    fdoExtension->DmaRx = *resourceDescriptor;
                } else {
                    fdoExtension->DmaTx = *resourceDescriptor;
                }
                channelNumber++;
            }
        }
    } else if (fdoExtension->CompatType == SDHC_COMPAT_TYPE_QC8660) {
        channelNumber = 0;
        fullResourceDesc = &ResourceList->List[0];
        resourceList = &fullResourceDesc->PartialResourceList;
        resourceDescriptor = resourceList->PartialDescriptors;

        for (i = 0; i < resourceList->Count; i++, resourceDescriptor++) {
            if (resourceDescriptor->Type == CmResourceTypeDma) {
                fdoExtension->DmaResource = *resourceDescriptor;
            }
        }
    } else if (fdoExtension->CompatType == SDHC_COMPAT_TYPE_QC8960) {
    
        channelNumber = 0;
        fullResourceDesc = &ResourceList->List[0];
        resourceList = &fullResourceDesc->PartialResourceList;
        resourceDescriptor = resourceList->PartialDescriptors;

        for (i = 0; i < resourceList->Count; i++, resourceDescriptor++) {
            if (resourceDescriptor->Type == CmResourceTypeDma) {
                if (channelNumber == 0) {
                    fdoExtension->DmaTx = *resourceDescriptor;
                } else {
                    fdoExtension->DmaRx = *resourceDescriptor;
                }
                channelNumber++;
            }
        }
    }
//@@END_DDKSPLIT

    fdoExtension->SystemPowerState = PowerSystemWorking;
    fdoExtension->DevicePowerState = PowerDeviceD0;

    //
    // Initialize our DpcForIsr
    //

    IoInitializeDpcRequest(Fdo, SdbusInterruptDpc);

    if (fdoExtension->Interrupt.u.Interrupt.Level == 0) {
        status = STATUS_UNSUCCESSFUL;
        DebugPrint((SDBUS_DEBUG_FAIL,
                    "fdo %p StartDevice - Interrupt.Level == 0!", 
                    Fdo));
        goto Cleanup;
    }

    //
    // Hook up the controller interrupt for detecting pc-card plug ins/outs
    //

    fdoExtension->CardInterruptForward = TRUE;

    interruptMode =
        ((fdoExtension->Interrupt.Flags & CM_RESOURCE_INTERRUPT_LATCHED) == 
            CM_RESOURCE_INTERRUPT_LATCHED) ? 
        Latched : LevelSensitive;

    //
    // Because we need to attach both a DIRQL and passive level handler, we will always
    // request the interrupt as shared. This is ok.
    //

    sharedInterrupt = TRUE;

#if 0

    sharedInterrupt = 
        (fdoExtension->Interrupt.ShareDisposition == CmResourceShareShared)?
        TRUE : FALSE;

#endif

    //
    // Initialize our shared interrupt spinlock.
    //

    KeInitializeSpinLock( &fdoExtension->SdbusInterruptSpinLock );
    fdoExtension->SdbusInterruptSpinlockIrql = (KIRQL)fdoExtension->TranslatedInterrupt.u.Interrupt.Level;

    //
    // Connect up our DIRQL level interrupt handler
    //

    RtlZeroMemory( &connectInterruptParameters, sizeof(IO_CONNECT_INTERRUPT_PARAMETERS));

    connectInterruptParameters.Version = CONNECT_FULLY_SPECIFIED;
    connectInterruptParameters.FullySpecified.PhysicalDeviceObject = fdoExtension->Pdo;
    connectInterruptParameters.FullySpecified.InterruptObject = &(fdoExtension->SdbusInterruptObject);
    connectInterruptParameters.FullySpecified.ServiceRoutine = (PKSERVICE_ROUTINE) SdbusDirqlInterrupt;
    connectInterruptParameters.FullySpecified.ServiceContext = (PVOID) Fdo;
    connectInterruptParameters.FullySpecified.SpinLock = &fdoExtension->SdbusInterruptSpinLock;
    connectInterruptParameters.FullySpecified.SynchronizeIrql = (KIRQL) fdoExtension->SdbusInterruptSpinlockIrql;
    connectInterruptParameters.FullySpecified.FloatingSave = FALSE;
    connectInterruptParameters.FullySpecified.ShareVector = sharedInterrupt;
    connectInterruptParameters.FullySpecified.Vector = fdoExtension->TranslatedInterrupt.u.Interrupt.Vector;
    connectInterruptParameters.FullySpecified.Irql = (KIRQL) fdoExtension->TranslatedInterrupt.u.Interrupt.Level;
    connectInterruptParameters.FullySpecified.InterruptMode = interruptMode;
    connectInterruptParameters.FullySpecified.ProcessorEnableMask = (KAFFINITY) fdoExtension->TranslatedInterrupt.u.Interrupt.Affinity;

    status = IoConnectInterruptEx( &connectInterruptParameters);

    if (!NT_SUCCESS(status)) {
        NT_ASSERT( 0 && "fdo %p Unable to connect interrupt" );
        DebugPrint((SDBUS_DEBUG_FAIL,
                    "fdo %p Unable to connect interrupt", 
                    Fdo));
        goto Cleanup;
    }

    //
    // Connect up our passive level interrupt handler.
    //
    // Note that in order to ensure synchronization against the DIRQL ISR above, we have a shared spinlock 
    // in fdoExtension->SdbusInterruptSpinLock, at a shared DIRQL fdoExtension->SdbusInterruptSpinlockIrql.
    //
    // This cannot be set here, as IoConnectInterruptEx will reject a spinlock for a PASSIVE_LEVEL handler.
    // But we acquire it manually in the passive level ISR itself.
    //
    
    RtlZeroMemory( &connectInterruptParameters, sizeof(IO_CONNECT_INTERRUPT_PARAMETERS));

    connectInterruptParameters.Version = CONNECT_FULLY_SPECIFIED;
    connectInterruptParameters.FullySpecified.PhysicalDeviceObject = fdoExtension->Pdo;
    connectInterruptParameters.FullySpecified.InterruptObject = &(fdoExtension->SdbusPassiveInterruptObject);
    connectInterruptParameters.FullySpecified.ServiceRoutine = (PKSERVICE_ROUTINE) SdbusPassiveInterrupt;
    connectInterruptParameters.FullySpecified.ServiceContext = (PVOID) Fdo;
    connectInterruptParameters.FullySpecified.SpinLock = NULL;
    connectInterruptParameters.FullySpecified.SynchronizeIrql = (KIRQL) PASSIVE_LEVEL;
    connectInterruptParameters.FullySpecified.FloatingSave = FALSE;
    connectInterruptParameters.FullySpecified.ShareVector = sharedInterrupt;
    connectInterruptParameters.FullySpecified.Vector = fdoExtension->TranslatedInterrupt.u.Interrupt.Vector;
    connectInterruptParameters.FullySpecified.Irql = (KIRQL) PASSIVE_LEVEL;
    connectInterruptParameters.FullySpecified.InterruptMode = interruptMode;
    connectInterruptParameters.FullySpecified.ProcessorEnableMask = (KAFFINITY) fdoExtension->TranslatedInterrupt.u.Interrupt.Affinity;

    status = IoConnectInterruptEx( &connectInterruptParameters);

    if (!NT_SUCCESS(status)) {
        NT_ASSERT( 0 && "fdo %p Unable to connect passive level interrupt" );
        DebugPrint((SDBUS_DEBUG_FAIL,
                    "fdo %p Unable to connect passive level  interrupt", 
                    Fdo));
        goto Cleanup;
    }

    //
    // The PASSIVE_LEVEL ISR is not immediately active. 
    //
    // For SDIO, we will wait for the functional driver to register before we enable it.
    // For SD, we enable it if the card detection method is via WAIT WAKE.
    //

    fdoExtension->SdbusPassiveInterruptActive = 
        fdoExtension->CardDetectMethod == SDBUS_CARD_DETECT_METHOD_WAIT_WAKE;

    interruptActiveStateParameters.Version = CONNECT_FULLY_SPECIFIED;
    interruptActiveStateParameters.ConnectionContext.InterruptObject = fdoExtension->SdbusPassiveInterruptObject;
    
    if (!fdoExtension->SdbusPassiveInterruptActive) {
        IoReportInterruptInactive(&interruptActiveStateParameters);
    }
    
    KeInitializeDpc(&fdoExtension->CardDetectDpc, (PKDEFERRED_ROUTINE) SdbusCardDetectDpc, Fdo);

    if (fdoExtension->CardDetectMethod == SDBUS_CARD_DETECT_METHOD_GPIO) {

        //
        // Hook up the optional GPIO card detect interrupt if it exists.
        //

        if (fdoExtension->GpioInterrupt.u.Interrupt.Level == 0) {
            status = STATUS_UNSUCCESSFUL;
            DebugPrint((SDBUS_DEBUG_FAIL,
                        "fdo %p StartDevice - GpioInterrupt.Level == 0!", 
                        Fdo));
            goto Cleanup;
        }

        interruptMode =
            ((fdoExtension->GpioInterrupt.Flags & CM_RESOURCE_INTERRUPT_LATCHED) == 
                CM_RESOURCE_INTERRUPT_LATCHED) ? 
            Latched : LevelSensitive;

        sharedInterrupt = 
            (fdoExtension->GpioInterrupt.ShareDisposition == CmResourceShareShared)?
            TRUE : FALSE;

        RtlZeroMemory( &connectInterruptParameters, sizeof(IO_CONNECT_INTERRUPT_PARAMETERS));

        connectInterruptParameters.Version = CONNECT_FULLY_SPECIFIED;
        connectInterruptParameters.FullySpecified.PhysicalDeviceObject = fdoExtension->Pdo;
        connectInterruptParameters.FullySpecified.InterruptObject = &(fdoExtension->SdbusGpioInterruptObject);
        connectInterruptParameters.FullySpecified.ServiceRoutine = (PKSERVICE_ROUTINE) SdbusGpioInterrupt;
        connectInterruptParameters.FullySpecified.ServiceContext = (PVOID) Fdo;
        connectInterruptParameters.FullySpecified.SpinLock = NULL;
        connectInterruptParameters.FullySpecified.SynchronizeIrql = 
            (KIRQL) fdoExtension->GpioTranslatedInterrupt.u.Interrupt.Level;
        connectInterruptParameters.FullySpecified.FloatingSave = FALSE;
        connectInterruptParameters.FullySpecified.ShareVector = sharedInterrupt;
        connectInterruptParameters.FullySpecified.Vector = 
            fdoExtension->GpioTranslatedInterrupt.u.Interrupt.Vector;
        connectInterruptParameters.FullySpecified.Irql = 
            (KIRQL) fdoExtension->GpioTranslatedInterrupt.u.Interrupt.Level;
        connectInterruptParameters.FullySpecified.InterruptMode = interruptMode;
        connectInterruptParameters.FullySpecified.ProcessorEnableMask = 
            (KAFFINITY) fdoExtension->GpioTranslatedInterrupt.u.Interrupt.Affinity;

        status = IoConnectInterruptEx( &connectInterruptParameters);

        if (!NT_SUCCESS(status)) {
            NT_ASSERT( 0 && "fdo %p Unable to connect GPIO interrupt" );
            DebugPrint((SDBUS_DEBUG_FAIL,
                        "fdo %p Unable to connect GPIO interrupt", 
                        Fdo));
            goto Cleanup;
        }

        if ((fdoExtension->GpioCardDetectHandle == NULL) &&
            (fdoExtension->SdbusGpioInterruptObject != NULL)) {

            //
            // Disconnect GPIO interrupt, in case GPIO interrupt line is correctly hooked up
            // and card detect pin is correctly wired to the host register, since when we are
            // powered up, normal interrupt will fire and we can read host register for card
            // detect. If we don't disconnect, some platforms will double fire both normal and
            // GPIO interrupts.
            //

            interruptActiveStateParameters.Version = CONNECT_FULLY_SPECIFIED;
            interruptActiveStateParameters.ConnectionContext.InterruptObject = fdoExtension->SdbusGpioInterruptObject;

#if defined(CAPTURE_ACTIVE_IDLE)
            InterlockedIncrement(&fdoExtension->Dbg.ReportGPIOInterruptInactiveCount);
#endif            

            IoReportInterruptInactive(&interruptActiveStateParameters);
        }
    }

    //
    // do vendor-specific init of controller
    //

    SDPORT_InitController(fdoExtension);

    //
    // Now the controller registers should be accessible
    //

    fdoExtension->Flags &= ~SDBUS_FDO_OFFLINE;

    //
    // Register for runtime power management, unless we disabled it in the registry.
    //
    if (fdoExtension->FxHandle == NULL && !SdDisableRuntimePower) {
        status = SdbusEnableRuntimePowerManagement(fdoExtension, allSocketsNonRemovable);
    }

    //
    // Do resets on the sockets to update the the socket's capabilities
    //

    for (socketNumber = 0; 
         socketNumber < fdoExtension->NumberOfSockets; 
         socketNumber ++) {

        PSD_SOCKET socket = fdoExtension->Socket[socketNumber];
        SD_SOCKET_RESET_WORKER_PARAMETER parameter = { SDBUS_RESET_TYPE_ALL };

        status = SdbusExecuteWorkSynchronousWithParameter(
                                            SDWP_SOCKET_RESET,
                                            socket,
                                            NULL,
                                            NULL,
                                            (UCHAR *) &parameter,
                                            (ULONG) sizeof(parameter));

        if (!NT_SUCCESS(status)) {
            DebugPrint((SDBUS_DEBUG_FAIL,
                        "fdo %p StartDevice - error(%08x) from SDWP_SOCKET_RESET!",
                        Fdo, 
                        status));
            goto Cleanup;
        }

        socket->IoWorkItem = IoAllocateWorkItem(Fdo);

        if (socket->IoWorkItem == NULL) {

            status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        SDPORT_InitSocket(socket);

        socket->SystemWorkItem = IoAllocateWorkItem(Fdo);
        if (socket->SystemWorkItem == NULL) {

            status = STATUS_INSUFFICIENT_RESOURCES;
            goto Cleanup;
        }

        if (BitTest((PLONG)&socket->Capabilities.DsmFunctionsSupported, DSM_FUNCTION_SET_18_VOLTAGE_SIGNALING) ||
            BitTest((PLONG)&socket->Capabilities.DsmFunctionsSupported, DSM_FUNCTION_SET_33_VOLTAGE_SIGNALING)) {

            socket->DsmWorkItem = IoAllocateWorkItem(Fdo);
            if (socket->DsmWorkItem == NULL) {

                status = STATUS_INSUFFICIENT_RESOURCES;
                goto Cleanup;
            }

            socket->DsmContext = ExAllocatePool(NonPagedPoolNx, sizeof(SD_DSM_CONTEXT));
            if (socket->DsmContext == NULL) {

                status = STATUS_INSUFFICIENT_RESOURCES;
                goto Cleanup;
            }
        }
    }

    //
    // We needed to wait until after doing a socket reset to decide if DMA
    // is supported or not. Now that we have checked the socket
    // capabilities, then allocate an adapter as necessary.
    //

    for (socketNumber = 0; 
         socketNumber < fdoExtension->NumberOfSockets; 
         socketNumber ++) {

        PSD_SOCKET socket = fdoExtension->Socket[socketNumber];

        status = SdbusInitializeTransfer(socket,
                                         fdoExtension->Pdo,
                                         ResourceList->List[0].BusNumber);
        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }
    }


    //
    // Check to see if we need the poll timer
    //

    if ((fdoExtension->UsePollTimer == POLL_TIMER_ENABLED) ||
        (fdoExtension->UsePollTimer == POLL_TIMER_DIAGNOSTIC)) {

        LARGE_INTEGER dueTime;

        DebugPrint((SDBUS_DEBUG_INFO,
                    "SdbusFdoStartDevice: starting poll timer"));

        SetDeviceFlag(fdoExtension, SDBUS_FDO_POLL_TIMER_ACTIVE);

        KeInitializeDpc(&fdoExtension->PollTimerDpc, SdbusPollTimerDpc, Fdo);
        KeInitializeTimer(&fdoExtension->PollTimer);

        //
        // Set first fire to twice the peroidic interval - just
        //

        dueTime.QuadPart = - (LONG)PollTimerInterval * 1000 * 10 * 2;

        KeSetTimerEx(&(fdoExtension->PollTimer),
                     dueTime,
                     PollTimerInterval,
                     &fdoExtension->PollTimerDpc);
    }

    for (socketNumber = 0; 
         socketNumber < fdoExtension->NumberOfSockets; 
         socketNumber ++) {

        PSD_SOCKET socket = fdoExtension->Socket[socketNumber];
        
        KeInitializeEvent(&socket->SocketInitialized,
                          NotificationEvent,
                          FALSE);

        //
        // Check to see if there's a card in the socket.
        //

        if (fdoExtension->GpioCardDetectHandle != NULL) {
        
            data = 0;

            status = ZwDeviceIoControlFile(fdoExtension->GpioCardDetectHandle,
                                           NULL,
                                           NULL,
                                           NULL,
                                           &ioStatus,
                                           IOCTL_GPIO_READ_PINS,
                                           NULL,
                                           0,
                                           (PVOID)&data,
                                           sizeof(data));
                                           
            if (!NT_SUCCESS(status)) {

                NT_ASSERT(FALSE && "Unable to send IOCTL to GPIO driver to query card insertion");
                goto Cleanup;
            }

            //
            // Invert the value since card detect is active low, meaning
            // when inserted the pin is low and value is 0.
            //

            socket->CardInserted = (BOOLEAN) data;
            socket->CardInserted = !socket->CardInserted;

        } else {

            //
            // Otherwise, the card detect PIN is correctly wired to the host controller,
            // and thus we should be able to read from host registers.
            //

            socket->CardInserted = SDPORT_DetectCardInSocket(socket);
        }

        //
        // Check what the write protect state of the card is.
        //

        if (socket->CardInserted != FALSE) {
            if (fdoExtension->GpioWriteProtectHandle != NULL) {            
                data = 0;
                status = ZwDeviceIoControlFile(fdoExtension->GpioWriteProtectHandle,
                                               NULL,
                                               NULL,
                                               NULL,
                                               &ioStatus,
                                               IOCTL_GPIO_READ_PINS,
                                               NULL,
                                               0,
                                               (PVOID)&data,
                                               sizeof(data));
                                               
                if (!NT_SUCCESS(status)) {

                    NT_ASSERT(FALSE && "Unable to send IOCTL to GPIO driver to query write protect state");
                    
                    goto Cleanup;
                }

                socket->CardData.CardIsWriteProtected = (BOOLEAN) data;
                
            } else {
                socket->CardData.CardIsWriteProtected = 
                    SDPORT_IsWriteProtected(socket);
                
            }
        }

        if (socket->CardInserted || 
            (fdoExtension->GpioCardDetectHandle == NULL)) {

            //
            // Activate socket will power up and ready the card so we
            // can test to see if the card has changed while the controller
            // was stopped.
            //

            SdbusActivateSocket(socket, 
                                SA_REQUEST_VERIFY_CARD_ON_POWER_UP, 
                                NULL, 
                                NULL, 
                                NULL);
        
            KeWaitForSingleObject(&socket->SocketInitialized,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);

            KeClearEvent(&socket->SocketInitialized);
        }
    }

    //
    //  Get the Host Controller Symbolic Link so that we can register
    //

    status = IoRegisterDeviceInterface(fdoExtension->Pdo,
                                       &GUID_SD_HOST_CONTROLLER,
                                       NULL,
                                       &fdoExtension->SymbolicLink);
    if ( !NT_SUCCESS(status) ) {
        goto Cleanup;
    }

    //
    //  Enable the Host Controller Interface
    //

    status = IoSetDeviceInterfaceState(&fdoExtension->SymbolicLink, TRUE);

    if ( !NT_SUCCESS(status) ) {
        goto Cleanup;
    }

#ifdef SQM_SUPPORT
    SdbusSqmInsertControllerData(Fdo);
#endif

Cleanup:

    if (!NT_SUCCESS(status)) {

        //
        // Failure
        //

        if (IsDeviceFlagSet(fdoExtension, SDBUS_FDO_POLL_TIMER_ACTIVE)) {
            //
            // cancel the card interrupt poller
            //
            KeCancelTimer(&fdoExtension->PollTimer);
            ResetDeviceFlag(fdoExtension, SDBUS_FDO_POLL_TIMER_ACTIVE);
        }

        SdbusFdoFreeDeviceResources(Fdo);
    } else {
        SET_NEW_PNP_STATE(fdoExtension, SDBUS_DEVICE_STARTED);
        fdoExtension->SdbusGpioInterruptActive = TRUE;
    }
  
    return status;
}

NTSTATUS
SdbusFdoStartDeviceComplete(
    __in PDEVICE_OBJECT Fdo,
    __in PIRP Irp,
    __in PVOID Context
    )
{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PIO_WORKITEM workItem = NULL;
    NTSTATUS status = Irp->IoStatus.Status;

    UNREFERENCED_PARAMETER(Context);

    //
    // Ensure that the lower driver successfully handled the IRP.
    //

    if (!NT_SUCCESS(status)) {
        goto Failure;
    }

    workItem = IoAllocateWorkItem(Fdo);

    if (workItem == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Failure;
    }

    IoQueueWorkItemEx(workItem,
                      SdbusFdoStartDeviceWorker,
                      DelayedWorkQueue,
                      Irp);

    return STATUS_MORE_PROCESSING_REQUIRED;
    
Failure:
    
    DebugPrint((SDBUS_DEBUG_FAIL,
               "fdo %p lower device can't be started 0x%x!",
               Fdo, 
               status));
    Irp->IoStatus.Status = status;
    IoReleaseRemoveLock(&fdoExtension->RemoveLock, Irp); 

    return STATUS_CONTINUE_COMPLETION;

}

VOID
SdbusFdoStartDeviceWorker(
    __in PDEVICE_OBJECT Fdo,
    __in PIRP Irp,
    __in PIO_WORKITEM IoWorkItem
    )
/*++

Routine Description:

    Start device routine

Arguments:

    IoObject - pointer to the FDO of this device

    Irp - the start device IRP

    IoWorkItem - the work item associated with this callback

Return Value:

    status

--*/
{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    NTSTATUS status;

    PAGED_CODE();

    status = SdbusFdoStartDevice(Fdo, Irp);

    Irp->IoStatus.Status = status;
    
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    IoReleaseRemoveLock(&fdoExtension->RemoveLock, Irp);
    IoFreeWorkItem(IoWorkItem);
}

VOID
SdbusFdoStopDevice(
    __inout PDEVICE_OBJECT Fdo
    )

/*++

Routine Description:

    IRP_MN_STOP_DEVICE handler for the given sd controller. If Irp is present,
    it'll send it down first to the PDO. Unhooks the interrupt/cancels poll
    timer etc.

Arguments:

    Fdo - Pointer to functional device object for the sd controller

Return value:

    VOID - no reason to fail stoping the device

--*/

{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    ULONG socketNumber;
    PSD_SOCKET socket;    

    if (IsDeviceFlagSet(fdoExtension, SDBUS_FDO_POLL_TIMER_ACTIVE)) {

        //
        // cancel the card interrupt poller
        //

        KeCancelTimer(&fdoExtension->PollTimer);
        ResetDeviceFlag(fdoExtension, SDBUS_FDO_POLL_TIMER_ACTIVE);
    }

    if (fdoExtension->FxPStateIntervalStartTime != 0) {
        KeCancelTimer(&fdoExtension->FxPStateTimer);
    }

    SdbusFdoDisarmWake(fdoExtension);

    //
    // Stop workpackets on the controller and disable interrupts
    //

    for (socketNumber = 0; 
         socketNumber < fdoExtension->NumberOfSockets; 
         socketNumber++) {

        NTSTATUS status;

        socket = fdoExtension->Socket[socketNumber];

        KeCancelTimer(&SD_DEFAULT_WORK_ENGINE(socket)->RetuningTimer);

        status = SdbusExecuteWorkSynchronous(SDWP_STOP_SOCKET,
                                             socket,
                                             NULL,
                                             NULL);

        if (!NT_SUCCESS(status)) {

            NT_ASSERT(NT_SUCCESS(status));
    
            DebugPrint((SDBUS_DEBUG_WARNING,
                        "fdo %p StopDevice - error(%08x) from SDWP_STOP_RESET",
                        Fdo, 
                        status));
        }

        SdbusDisableEvents(socket, SDBUS_EVENT_CARD_INTERRUPT);
        SdbusDisableEvents(socket, SDBUS_DEVICE_CONTROLLER_EVENTS);
    }

    //
    // The bus driver below us will make us go offline
    //

    fdoExtension->Flags |= SDBUS_FDO_OFFLINE;

    //
    //  Disable the Host Controller interface
    //

    IoSetDeviceInterfaceState(&fdoExtension->SymbolicLink, FALSE);

    SdbusFdoFreeDeviceResources(Fdo);
}



VOID
SdbusFdoFreeQueuedPackets(
    __inout PDEVICE_OBJECT Fdo
    )
{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PSD_SOCKET socket;
    ULONG socketNumber;

    for (socketNumber = 0; 
         socketNumber < fdoExtension->NumberOfSockets; 
         socketNumber++) {

        socket = fdoExtension->Socket[socketNumber];

        //
        // If we failed to allocate a socket structure in StartDevice 
        // (due to low memory conditions), there's nothing to free.
        //
        
        if (!socket) {
            continue;
        }

        NT_ASSERT(socket->WorkEngine.State == WORKER_IDLE);
        SdbusWorkerFreeQueuedPackets(&socket->WorkEngine,
                                     STATUS_DEVICE_REMOVED);
    }
}



VOID
SdbusFdoFreeDeviceResources(
    __inout PDEVICE_OBJECT Fdo
    )

/*++

Routine Description:

    This routine undoes the work that start_device does to acquire I/O
    resources for the device. It is called by stop_device, or from within
    start_device if there is an error

Arguments:

    Fdo - Pointer to functional device object for the controller to be removed

Return value:

   NTSTATUS

--*/

{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PSD_SOCKET socket;
    ULONG socketNumber;

    if (fdoExtension->SdbusInterruptObject) {

        //
        // unhook the DIRQL interrupt
        //

        IoDisconnectInterrupt(fdoExtension->SdbusInterruptObject);
        fdoExtension->SdbusInterruptObject = NULL;
    }

    if (fdoExtension->SdbusPassiveInterruptObject) {

        //
        // unhook the passive level interrupt
        //

        IoDisconnectInterrupt(fdoExtension->SdbusPassiveInterruptObject);
        fdoExtension->SdbusPassiveInterruptObject = NULL;
    }

    fdoExtension->SdbusGpioInterruptActive = FALSE;
    if (fdoExtension->SdbusGpioInterruptObject) {
        IoDisconnectInterrupt(fdoExtension->SdbusGpioInterruptObject);
        fdoExtension->SdbusGpioInterruptObject = NULL;
    }

    if (fdoExtension->GpioCardDetectHandle) {
        ZwClose(fdoExtension->GpioCardDetectHandle);
        fdoExtension->GpioCardDetectHandle = NULL;
    }

    if (fdoExtension->GpioWriteProtectHandle) {
        ZwClose(fdoExtension->GpioWriteProtectHandle);
        fdoExtension->GpioWriteProtectHandle = NULL;
    }

    if (fdoExtension->FxPStates != NULL) {
        ExFreePoolWithTag(fdoExtension->FxPStates, SDBUS_TAG_RUNTIME_POWER);
        fdoExtension->FxPStates = NULL;
    }

    if (fdoExtension->WaitWakeArmWorkItem != NULL) {
        IoFreeWorkItem(fdoExtension->WaitWakeArmWorkItem);
    }

    if (fdoExtension->CacheCheckWorkItem != NULL) {
        IoFreeWorkItem(fdoExtension->CacheCheckWorkItem);
    }

    //
    // Unmap any i/o space or memory we might have mapped
    //

    for (socketNumber = 0; 
         socketNumber < fdoExtension->NumberOfSockets; 
         socketNumber++) {

        socket = fdoExtension->Socket[socketNumber];

        //
        // If we failed to allocate a socket structure in StartDevice (due to low memory
        // conditions), there's nothing to free.
        //
        
        if (!socket) {
            continue;
        }

        if (socket->DmaScatterGatherBuffer) {
            ExFreePoolWithTag(socket->DmaScatterGatherBuffer,SDBUS_TAG_DMA_SG_LIST);
            socket->DmaScatterGatherBuffer = NULL;
        }

        if (socket->DmaScatterGatherBufferRx) {
            ExFreePoolWithTag(socket->DmaScatterGatherBufferRx,SDBUS_TAG_DMA_SG_LIST);
            socket->DmaScatterGatherBufferRx = NULL;
        }

        if (socket->DmaScatterGatherBufferTx) {
            ExFreePoolWithTag(socket->DmaScatterGatherBufferTx,SDBUS_TAG_DMA_SG_LIST);
            socket->DmaScatterGatherBufferTx = NULL;
        }

        if (socket->DmaVirtualAddr) {

            //
            // DmaVirtualAddress is also used for system DMA as well, so we have
            // to make sure that we only free the buffer if we're not using
            // system DMA.
            //

            if (socket->DmaAdapter && 
                ((socket->TransferMethod == SDBUS_TRANSFER_METHOD_ADMA2) ||
                 (socket->TransferMethod == SDBUS_TRANSFER_METHOD_DB_DMA))) {
                 
                socket->DmaAdapter->DmaOperations->FreeCommonBuffer(
                                                    socket->DmaAdapter,
                                                    socket->DmaLength,
                                                    socket->DmaPhysicalAddr,
                                                    socket->DmaVirtualAddr,
                                                    TRUE);
            }
            socket->DmaVirtualAddr = NULL;
        }

        if (socket->DmaAdapter) {
            socket->DmaAdapter->DmaOperations->PutDmaAdapter(socket->DmaAdapter);
            socket->DmaAdapter = NULL;
        }

        if (socket->DmaAdapterRx) {
            socket->DmaAdapterRx->DmaOperations->PutDmaAdapter(socket->DmaAdapterRx);
            socket->DmaAdapterRx = NULL;
        }

        if (socket->DmaAdapterTx) {
            socket->DmaAdapterTx->DmaOperations->PutDmaAdapter(socket->DmaAdapterTx);
            socket->DmaAdapterTx = NULL;
        }

        if (socket->DmaTransferContext) {
            ExFreePoolWithTag(socket->DmaTransferContext, SDBUS_TAG_DMA);
            socket->DmaTransferContext = NULL;
        }

        if (socket->ReservedAddress) {
            MmFreeMappingAddress(socket->ReservedAddress,
                                 SDBUS_TAG_MAPPING);
            socket->ReservedAddress = NULL;
        }

        if (socket->ReservedMdl) {
            IoFreeMdl(socket->ReservedMdl);
            socket->ReservedMdl = NULL;
        }

        if (socket->HostRegisterMemoryMapBase != NULL) {
            MmUnmapIoSpace(socket->HostRegisterMemoryMapBase,
                           socket->HostRegisterMemoryMapSize);

            socket->HostRegisterMemoryMapBase = NULL;
            socket->HostRegisterMemoryMapSize = 0;
            
            socket->HostRegisterBase = NULL;
            socket->HostRegisterSize = 0;
        }

        if (socket->IoWorkItem) {
            IoFreeWorkItem(socket->IoWorkItem);
            socket->IoWorkItem = NULL;
        }

        if (socket->SystemWorkItem) {
            IoFreeWorkItem(socket->SystemWorkItem);
            socket->SystemWorkItem = NULL;
        }

        if (socket->DsmWorkItem) {
            IoFreeWorkItem(socket->DsmWorkItem);
            socket->DsmWorkItem = NULL;
        }

        if (socket->DsmContext) {
            ExFreePoolWithTag(socket->DsmContext, SDBUS_TAG_ACPI_INFORMATION);
            socket->DsmContext = NULL;
        }
    }
}



VOID
SdbusFdoRemoveDevice(
    __in PDEVICE_OBJECT Fdo
    )

/*++

Routine Description:

    Handles IRP_MN_REMOVE for the sd controller. Stops the adapter if it isn't
    already, sends the IRP to the PDO first & cleans up the Fdo for this
    controller and detaches & deletes the device object.

Arguments:

    Fdo - Pointer to functional device object for the controller to be removed

Return value:

   NTSTATUS

--*/

{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    PDEVICE_OBJECT pdo, nextPdo, fdo, prevFdo;
    PPDO_EXTENSION pdoExtension;
    ULONG socketNumber;
    PSD_SOCKET socket;

    PAGED_CODE();

#ifdef SQM_SUPPORT
    SdbusSqmInsertUsageData(Fdo);
#endif

    KeFlushQueuedDpcs();

    //
    // Unregister with runtime power management.
    //
    
    if (fdoExtension->FxPStates != NULL) {
        ExFreePoolWithTag(fdoExtension->FxPStates, SDBUS_TAG_RUNTIME_POWER);
        fdoExtension->FxPStates = NULL;
    }

    if (fdoExtension->FxHandle != NULL) {
        PoFxUnregisterDevice(fdoExtension->FxHandle);
        fdoExtension->FxHandle = NULL;
    }

    //
    // If the PdoList in the fdoExtension is non-empty it means: that the PDOs
    // in the list were not physically removed, but a soft REMOVE was issued,
    // hence they are still hanging on and now this controller itself is
    // being REMOVED. Hence we dispose of those PDOs now
    // 

    for (socketNumber = 0; 
         socketNumber < fdoExtension->NumberOfSockets; 
         socketNumber++) {

        socket = fdoExtension->Socket[socketNumber];

        if (socket == NULL) {
            continue;
        }

        for (pdo = socket->PdoList; pdo != NULL ; pdo = nextPdo) {

            DebugPrint((SDBUS_DEBUG_INFO,
                        "RemoveDevice: pdo %p child of fdo %p was not removed before fdo",
                        pdo, 
                        Fdo));

            pdoExtension = pdo->DeviceExtension;

            NT_ASSERT (!IsDeviceFlagSet(pdoExtension, SDBUS_PDO_PHYSICALLY_REMOVED));

            if (pdoExtension->InterfaceContextPendingDeletion) {
                ExFreePoolWithTag(pdoExtension->InterfaceContextPendingDeletion,
                                  SDBUS_TAG_INTERFACE_CONTEXT);
                pdoExtension->InterfaceContextPendingDeletion = NULL;
            }

            nextPdo = pdoExtension->NextPdo;
            if (pdoExtension->PnpState != SDBUS_DEVICE_DELETED) {
                SET_NEW_PNP_STATE(pdoExtension, SDBUS_DEVICE_DELETED);
                IoDeleteDevice(pdo);
            } else {
                NT_ASSERT(FALSE);
            }
        }

        NT_ASSERT(IsListEmpty(&socket->WorkEngine.IoIrpQueue));
        NT_ASSERT(IsListEmpty(&socket->WorkEngine.SystemWorkPacketQueue));
        NT_ASSERT(socket->WorkEngine.State == WORKER_IDLE);

        ExFreePoolWithTag(socket, SDBUS_TAG_SOCKET);
        fdoExtension->Socket[socketNumber] = NULL;
    }

    ExDeleteLookasideListEx(&fdoExtension->WorkPacketLookasideList);

    SET_NEW_PNP_STATE(fdoExtension, SDBUS_DEVICE_DELETED);

    //
    // Remove this from the fdo list..
    //

    prevFdo = NULL;
    for (fdo = FdoList; 
         fdo != NULL; 
         prevFdo = fdo, fdo = fdoExtension->NextFdo) {

        fdoExtension = fdo->DeviceExtension;
        if (fdo == Fdo) {
            if (prevFdo) {

                //
                // Delink this fdo
                //

                ((PFDO_EXTENSION)prevFdo->DeviceExtension)->NextFdo = 
                    fdoExtension->NextFdo;
            } else {
                FdoList = fdoExtension->NextFdo;
            }
            break;
        }
    }

    DebugPrint((SDBUS_DEBUG_PNP,
                "fdo %p Remove detach & delete", 
                Fdo));

#if defined (TARGETING_Win2K)
    fdoExtension = fdo->DeviceExtension;
    ResetDeviceFlag(fdoExtension, SDBUS_FDO_WPP_INITIALIZED);

    SdbusDbgCleanupTracing(Fdo);
#endif

    //
    // Delete the bus driver interface for manipulating config space.
    //

    SdbusDeleteInterface((PFDO_EXTENSION)Fdo->DeviceExtension);

    IoDetachDevice(((PFDO_EXTENSION)Fdo->DeviceExtension)->LowerDevice);
    IoDeleteDevice(Fdo);
}



NTSTATUS
SdbusDeviceRelations(
    __inout PDEVICE_OBJECT Fdo,
    __inout PIRP Irp,
    __in DEVICE_RELATION_TYPE RelationType,
    __deref_out PDEVICE_RELATIONS *DeviceRelations
    )

/*++

Routine Description:

    This routine will force enumeration of the sd controller represented by
    Fdo, allocate a device relations structure and fill in the count and
    object array with referenced object pointers to the valid PDOs which are
    created during enumeration

Arguments:

    Fdo - a pointer to the functional device object being enumerated
 
    Irp - pointer to the Irp
 
    RelationType - Type of relationship to be retrieved
 
    DeviceRelations - Structure to store the device relations
 
Return value:

   NTSTATUS

--*/

{

    PDEVICE_OBJECT currentPdo;
    PPDO_EXTENSION currentPdoExtension;
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    ULONG newRelationsSize, oldRelationsSize = 0;
    ULONG newRelationsCount;
    PDEVICE_RELATIONS deviceRelations = NULL, oldDeviceRelations;
    ULONG count;
    NTSTATUS status;
    ULONG socketNumber;
    PSD_SOCKET socket;

    PAGED_CODE();

    switch(RelationType) {

    case BusRelations:

        //
        // This idle call corresponds to the active reference taken in SdbusSetSocketCardChanged where
        // we have invalidated the device relations.
        //

        if (InterlockedCompareExchange(&fdoExtension->BusRelationsRefCount, 0, 1) == 1) {

            SdbusFxComponentIdle(fdoExtension, 0);
        }

        oldDeviceRelations = (PDEVICE_RELATIONS) Irp->IoStatus.Information;
        NT_ASSERT(oldDeviceRelations == NULL);

        status = SdbusEnumerateDevices(Fdo, Irp);

        if (!NT_SUCCESS(status)) {
            break;
        }

        if (fdoExtension->LivePdoCount == 0) {

            //
            // No PDO's to report, we can return early. If no device_relations
            // structure has yet been allocated, however, we need to allocate
            // one & set the count to zero. This will ensure that regardless
            // of whether we pass this IRP down or not, the IO subsystem won't
            // fail.
            //

            if (oldDeviceRelations == NULL) {
                *DeviceRelations = ExAllocatePoolWithTag(
                                                PagedPool, 
                                                sizeof(DEVICE_RELATIONS),
                                                SDBUS_TAG_DEVICE_RELATION);

                if (*DeviceRelations == NULL) {
                   return STATUS_INSUFFICIENT_RESOURCES;
                }
                (*DeviceRelations)->Count = 0;
                (*DeviceRelations)->Objects[0] = NULL;
            }
            status = STATUS_SUCCESS;
            break;
        }

        if (!(oldDeviceRelations) || (oldDeviceRelations->Count == 0)) {

            newRelationsSize = sizeof(DEVICE_RELATIONS) +
                    (fdoExtension->LivePdoCount - 1) * sizeof(PDEVICE_OBJECT);
            newRelationsCount = fdoExtension->LivePdoCount;

        } else {

            oldRelationsSize = sizeof(DEVICE_RELATIONS) +
                    (oldDeviceRelations->Count-1) * sizeof(PDEVICE_OBJECT);
            newRelationsSize = oldRelationsSize + 
                    fdoExtension->LivePdoCount * sizeof(PDEVICE_OBJECT);
            newRelationsCount = fdoExtension->LivePdoCount + 
                                oldDeviceRelations->Count;
        }

        deviceRelations = ExAllocatePoolWithTag(PagedPool, 
                                                newRelationsSize,
                                                SDBUS_TAG_DEVICE_RELATION);

        if (deviceRelations == NULL) {

            DebugPrint((SDBUS_DEBUG_FAIL,
                        "SdbusDeviceRelations: unable to allocate %d bytes for device relations",
                        newRelationsSize));
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        if (oldDeviceRelations) {
            if ((oldDeviceRelations)->Count > 0) {
                RtlCopyMemory(deviceRelations, 
                              oldDeviceRelations, 
                              oldRelationsSize);
            }
            count = oldDeviceRelations->Count; // May be zero
            ExFreePoolWithTag(oldDeviceRelations,
                              SDBUS_TAG_DEVICE_RELATION);

        } else {
            count = 0;
        }


        for (socketNumber = 0; 
             socketNumber < fdoExtension->NumberOfSockets; 
             socketNumber++) {

            socket = fdoExtension->Socket[socketNumber];

            //
            // Copy the object pointers into the structure
            //

            for (currentPdo = socket->PdoList; 
                 currentPdo != NULL; 
                 currentPdo = currentPdoExtension->NextPdo) {

                currentPdoExtension = currentPdo->DeviceExtension;

                if (!IsDeviceFlagSet(currentPdoExtension, 
                                     SDBUS_PDO_PHYSICALLY_REMOVED) && 
                    (count < newRelationsCount)) {

                    //
                    // Devices have to be referenced by the bus driver
                    // before returning them to PNP
                    //
                    deviceRelations->Objects[count++] = currentPdo;

                    status = ObReferenceObjectByPointer(currentPdo,
                                                        0, 
                                                        NULL, 
                                                        KernelMode);

                    if (!NT_SUCCESS(status)) {
                        DebugPrint((SDBUS_DEBUG_FAIL,
                                    "SdbusDeviceRelations: status %#08lx while referencing object %p",
                                    status, 
                                    currentPdo));
                        NT_ASSERT(NT_SUCCESS(status));
                    }
                }
            }
        }

        deviceRelations->Count = count;
        *DeviceRelations = deviceRelations;
        status = STATUS_SUCCESS;
        break;


    case RemovalRelations:

        //
        // Need reenumeration only if bus relations are required. We need to
        // save the pointer to the old device relations before we call
        // SdbusReenumerateDevices, as it might trample on it
        //

        oldDeviceRelations = (PDEVICE_RELATIONS) Irp->IoStatus.Information;

        NT_ASSERT(oldDeviceRelations == NULL);

        //
        // No PDO's to report, we can return early. If no device_relations
        // structure has yet been allocated, however, we need to allocate
        // one & set the count to zero. This will ensure that regardless of
        // whether we pass this IRP down or not, the IO subsystem won't
        // fail.
        //

        if (oldDeviceRelations == NULL) {
            *DeviceRelations = ExAllocatePoolWithTag(PagedPool, 
                                                     sizeof(DEVICE_RELATIONS),
                                                     SDBUS_TAG_DEVICE_RELATION);
            if (*DeviceRelations == NULL) {
               return STATUS_INSUFFICIENT_RESOURCES;
            }
            (*DeviceRelations)->Count = 0;
            (*DeviceRelations)->Objects[0] = NULL;
        }
        status = STATUS_SUCCESS;
        break;

    default:

        DebugPrint((SDBUS_DEBUG_INFO,
                   "SdbusDeviceRelations: RelationType %d, not handled",
                   (USHORT) RelationType));
        status = STATUS_NOT_SUPPORTED;
    }

    return status;
}

NTSTATUS
SdbusFdoDeviceUsageNotification(
    __in PDEVICE_OBJECT Fdo,
    __in PIRP Irp
    )
    
{

    PFDO_EXTENSION FdoExtension;
    PIO_STACK_LOCATION IrpSp;
    NTSTATUS Status;

    PAGED_CODE();

    FdoExtension = Fdo->DeviceExtension;
    IoCopyCurrentIrpStackLocationToNext(Irp);
    Status = IoSynchronousCallDriver(FdoExtension->LowerDevice, Irp);
    if (NT_SUCCESS(Status)) {
        IrpSp = IoGetCurrentIrpStackLocation(Irp);

        //
        // If the device is on any path (paging/hiber/dump/boot), clear
        // the usage notification.
        //

        if (IrpSp->Parameters.UsageNotification.InPath != FALSE) {
            FdoExtension->DeviceObject->Flags &= ~DO_POWER_PAGABLE;   

        } else {

            //
            // Pick up the usage of the parent device object.
            //

            if (FdoExtension->LowerDevice->Flags & DO_POWER_PAGABLE) {
                FdoExtension->DeviceObject->Flags |= DO_POWER_PAGABLE;
               
            } else {
                FdoExtension->DeviceObject->Flags &= ~DO_POWER_PAGABLE;   
            }
        }
    }

    return Status;
}

NTSTATUS
SdbusEnableRuntimePowerManagement(
    __in PFDO_EXTENSION FdoExtension,
    __in BOOLEAN        AllSocketsNonRemovable
    )

/*++

Routine Description

    Registers with the runtime power management framework.

Arguments

    FdoExtension - pointer to the device extension of the host controller's FDO

    AllSocketsNonRemovable - if all sockets in the controller is non-removable

Return Value

    STATUS_SUCCESS - Successfully registered with the framework
 
    STATUS_UNSUCCESSFUL - Could not register with the framework

--*/

{

    BOOLEAN ForceRuntimePowerManagement = FALSE;
    PO_FX_DEVICE_V2 fxDevice = {0};
    ULONG Index;
    PO_FX_COMPONENT_IDLE_STATE HostStates[SDBUS_FX_STATE_COUNT];
    SDBUS_PSTATE_REGISTRATION PStateRegistration = {0};
    NTSTATUS status = STATUS_SUCCESS;
    ULONG TimeIncrement;

    //
    // The device must be started and in D0 to register for runtime power 
    // management.
    //
    
    if (FdoExtension->DevicePowerState != PowerDeviceD0) {
        return STATUS_UNSUCCESSFUL;
    }

    SdbusCheckCrashdumpSetting();
    
    //
    // Initialize the host state definitions.
    //

    HostStates[SDBUS_FX_ON].TransitionLatency = 0;
    HostStates[SDBUS_FX_ON].ResidencyRequirement = 0; 
    HostStates[SDBUS_FX_ON].NominalPower = PO_FX_UNKNOWN_POWER;

    HostStates[SDBUS_FX_CLOCK_GATED].TransitionLatency = 10;
    HostStates[SDBUS_FX_CLOCK_GATED].ResidencyRequirement = 10;
    HostStates[SDBUS_FX_CLOCK_GATED].NominalPower = 1000;
    
    //
    // In addition to each socket, the host controller itself counts as a 
    // component.
    //

    fxDevice.Version                           = PO_FX_VERSION_V2;
    fxDevice.ComponentCount                    = 1;
    fxDevice.ComponentActiveConditionCallback  = SdbusFxComponentActiveCallback;
    fxDevice.ComponentIdleConditionCallback    = SdbusFxComponentIdleCallback;
    fxDevice.ComponentIdleStateCallback        = SdbusFxComponentIdleStateCallback;
    fxDevice.DevicePowerRequiredCallback       = SdbusFxDevicePowerRequiredCallback;
    fxDevice.DevicePowerNotRequiredCallback    = SdbusFxDevicePowerNotRequiredCallback;
    fxDevice.PowerControlCallback              = SdbusFxPowerControlCallback;
    fxDevice.DeviceContext                     = FdoExtension;

    //
    // Initialize component 0 (host controller).
    //

    //
    // Comment out "F0 on Dx" until V1 PEPs are no longer available.
    // fxDevice.Components[0].Flags = PO_FX_COMPONENT_FLAG_F0_ON_DX;
    //

    fxDevice.Components[0].IdleStateCount = SDBUS_FX_STATE_COUNT;
    
    fxDevice.Components[0].IdleStates = HostStates;
    fxDevice.Components[0].DeepestWakeableIdleState = SDBUS_FX_ON;

    FdoExtension->Active = TRUE;

#if defined(CAPTURE_ACTIVE_IDLE)    
    FdoExtension->Dbg.FxDebugActiveState = TRUE;
#endif

    KeInitializeSpinLock(&FdoExtension->FxIrpQueueLock);
    InitializeListHead(&FdoExtension->FxIrpQueue);

    //
    // Supress buffer overflow warning until PoFxRegisterDevice is fixed to
    // accept correct device type. Since PoFxRegisterDevice will figure out
    // device type in runtime, there is no buffer overflow.
    //

#pragma prefast(suppress: __WARNING_POTENTIAL_BUFFER_OVERFLOW_HIGH_PRIORITY, "PoFxRegisterDevice will figure out right type")
    status = PoFxRegisterDevice(FdoExtension->Pdo,
                                (PPO_FX_DEVICE)&fxDevice,
                                &FdoExtension->FxHandle);

    NT_ASSERT(NT_SUCCESS(status));

    //
    // Set the latency and residency that the driver can tolerate. The framework
    // will compare the value set here to the idle state definitions described
    // above.
    //

    PoFxSetComponentLatency(FdoExtension->FxHandle, 0, 10);
    PoFxSetComponentResidency(FdoExtension->FxHandle, 0, 10);

    //
    // If the registry key hack to force us to stay in F0 is set,
    // then tell PoFx never to put us into any F-state lower than F0.
    //
    
    if (SdForceF0State) {
        SdbusFxSetComponentWake(FdoExtension->FxHandle, 0, TRUE);
    }

    //
    // Ask the PEP if it has any P-states for this controller device.
    // The registration structure is an output buffer as we expect 
    // any PEP that supports this request to fill in the buffer with
    // the P-states that it supports.
    //

    status = PoFxPowerControl(FdoExtension->FxHandle,
                              (LPCGUID) &GUID_SDBUS_PSTATE_REGISTER,
                              NULL,
                              0,
                              &PStateRegistration,
                              sizeof(PStateRegistration),
                              NULL);

    NT_ASSERT(FdoExtension->FxPStateCount == 0);

    NT_ASSERT(FdoExtension->FxPStates == NULL);

    if (NT_SUCCESS(status)) {
        FdoExtension->FxPStateCount = PStateRegistration.PerfStateCount;

        NT_ASSERT(FdoExtension->FxPStateCount > 0);

        FdoExtension->FxPStateSamplingInterval = 
            PStateRegistration.SamplingInterval;

        NT_ASSERT(FdoExtension->FxPStateSamplingInterval <= MAXLONGLONG);

        //
        // If the interval provided is less than the timer tick,
        // we want to round up to at least one timer tick.
        //

        TimeIncrement = KeQueryTimeIncrement();
        if (FdoExtension->FxPStateSamplingInterval < TimeIncrement) {
            FdoExtension->FxPStateSamplingInterval = TimeIncrement;
        }

        FdoExtension->FxPStates = 
            ExAllocatePoolWithTag(NonPagedPoolNx,
                                  sizeof(SDBUS_PSTATE) * FdoExtension->FxPStateCount,
                                  SDBUS_TAG_RUNTIME_POWER);

        if (FdoExtension->FxPStates == NULL) {
            FdoExtension->FxPStateCount = 0;
            FdoExtension->FxPStateSamplingInterval = 0;
            status = STATUS_INSUFFICIENT_RESOURCES;

        } else {
            RtlCopyMemory(FdoExtension->FxPStates, 
                          PStateRegistration.PerfStates,
                          sizeof(SDBUS_PSTATE) * FdoExtension->FxPStateCount);
        }

        for (Index = 0; Index < FdoExtension->NumberOfSockets; Index += 1) {
            if (FdoExtension->Socket[Index]->Capabilities.BaseClockFrequency <
                FdoExtension->FxPStates[0].Frequency) {

                FdoExtension->Socket[Index]->Capabilities.BaseClockFrequency =
                    FdoExtension->FxPStates[0].Frequency;
            }
        }

    } else {

        //
        // PEP doesn't support this call, just keep going.
        //

        status = STATUS_SUCCESS;
    }

    //
    // For platforms that support eMMC sleep, let the platform know whether 
    // the feature is expected to be enabled in sdbus so it knows to
    // remove Vcc on Dx transitions.
    //

    if ((FdoExtension->CompatType == SDHC_COMPAT_TYPE_QC8974) &&
        (FdoExtension->HostRevision == SDHC_HOST_REVISION_QC8994)) {

        status = PoFxPowerControl(FdoExtension->FxHandle,
                                  (LPCGUID) &GUID_SDBUS_REPORT_SLEEP_FEATURE,
                                  &SdEnableEmmcSleep,
                                  sizeof(SdEnableEmmcSleep),
                                  NULL,
                                  0,
                                  NULL);

        //
        // Ignore failures if the platform doesn't support this method.
        //

        if (!NT_SUCCESS(status)) {
            status = STATUS_SUCCESS;
        }
    }

    if (NT_SUCCESS(status)) {
        NT_ASSERT(FdoExtension->FxHandle != NULL);

        //
        // Store the number of P-states each controller has globally in order
        // to synchronize the tuning procedures.
        //

        if ((FdoExtension->CompatType == SDHC_COMPAT_TYPE_TEGRA2) &&
            (FdoExtension->HostRevision == SDHC_HOST_REVISION_T40)) {

            ULONG ControllerIndex = 
                (FdoExtension->Socket[0]->HostRegisterPhysicalBase.LowPart - SDHC_SDMMC1_T30_BASE_ADDRESS) / 0x200;

            NvSdmmcPStateCount[ControllerIndex] = FdoExtension->FxPStateCount;
        }

        //
        // We should enable RTPM as long as the device is non-removable, or it is removable
        // but wake-capable.
        //

#if defined(_ARM_) || defined(_ARM64_)
        
        ForceRuntimePowerManagement = TRUE;

#endif

        if ((AllSocketsNonRemovable != FALSE) ||
            (FdoExtension->DeepestWakeableDstate > DeviceWakeDepthD0) ||
            (ForceRuntimePowerManagement != FALSE)) {

            if (FdoExtension->DeepestWakeableDstate == DeviceWakeDepthD3cold) {

                //
                // Enable D3Cold for the device.
                //

                FdoExtension->D3ColdInterface.SetD3ColdSupport(FdoExtension->D3ColdInterface.Context, TRUE);
            }

            PoFxStartDevicePowerManagement(FdoExtension->FxHandle);
        }
        
    } else {

        if (FdoExtension->FxPStates != NULL) {
            ExFreePoolWithTag(FdoExtension->FxPStates, SDBUS_TAG_RUNTIME_POWER);
        }

        if (FdoExtension->FxHandle != NULL) {
            PoFxUnregisterDevice(FdoExtension->FxHandle);
            FdoExtension->FxHandle = NULL;
        }
    }

    return status;
}


#define ACPI_METHOD_ADR   (ULONG)('RDA_') // _ADR
#define ACPI_METHOD_RMV   (ULONG)('VMR_') // _RMV


NTSTATUS
SdbusAcpiQueryRemovability(
    __in PFDO_EXTENSION FdoExtension
    )
    
/*++

Routine Description

    Queries the ACPI namespace for devices under the host controller (identified by the
    FDO's device extension in the function arguments), it evaluates the _ADR and _RMV control
    methods of each of these devices to determine and update the NonremovableDevice
    property for each SD_SOCKET part of the given host controller.

    This method does not assume the child PDOs are created.

Arguments

    FdoExtension - Pointer to the device extension of the host controller's FDO

Return Value

    STATUS_SUCCESS - Successfully queried the ACPI namespace

    STATUS_ACPI_DATA_INVALID - Could not query the ACPI namespace or invalid data returned.

    STATUS_INSUFFICIENT_RESOURCES - Could not allocate enough memory to complete the request

--*/

{
    NTSTATUS status = STATUS_SUCCESS;
    ACPI_ENUM_CHILDREN_OUTPUT_BUFFER dummyBuffer = {0};
    PACPI_ENUM_CHILDREN_OUTPUT_BUFFER outputEnumBuffer = NULL;
    PACPI_ENUM_CHILD acpiEnumChild = NULL;
    ANSI_STRING targetDeviceName;
    ULONG bufferSize;    
    ULONG socketNumber;
    ULONG count;
    BOOLEAN defaultSetting = FALSE;
    BOOLEAN safebetSetting = FALSE;

    struct {  
        ACPI_EVAL_OUTPUT_BUFFER OutputBuffer;  
        ULONG HighPart;  
    } outputEvalBuffer = {'\0'};

    //
    // If not ACPI or PCI enumerated, there is no known way to query for removability,
    // and thus we shouldn't change the default settings.
    //

    if ((IS_ACPI_DEVICE(FdoExtension) == FALSE) &&
        (IS_PCI_DEVICE(FdoExtension) == FALSE)) {

        goto Cleanup;
    }

    //
    // The new ACPI guideline says: if an SD socket in an ACPI enumerated host controller
    // is non-removable, then the host controller node must have a child node representing
    // the socket in ACPI namespace, and the child node must define a _RMV method that
    // evaluates to 0. In all other cases, the socket is considered removable.
    //
    // Legacy ACPI enumerated devices don't conform to the above guideline, and thus we should
    // keep the default settings (non-removable if ARM) instead of assuming removable.
    //

    if (IS_ACPI_DEVICE(FdoExtension) &&
        (FdoExtension->Socket[0]->HackFlags & SDBUS_HACK_NO_ACPI_RMV)) {

        goto Cleanup;
    }

    //
    // If ACPI enumerated:
    //      default to removable, but in case of incorrect firmware / internal error, fall back to non-removable.
    //
    // If PCI enumerated:
    //      default to removable, and in case of any error, always assume removable.
    //

    if (IS_ACPI_DEVICE(FdoExtension)) {

        defaultSetting = FALSE;
        safebetSetting = TRUE;

    } else {

        defaultSetting = FALSE;
        safebetSetting = FALSE;
    }

    for (count = 0; count < FdoExtension->NumberOfSockets; count++) {
        FdoExtension->Socket[count]->Settings.NonremovableDevice = defaultSetting;
    }

    //
    // Get all the ACPI namespace paths for all device identification objects
    // under the SD Host Controller namespace. First attempt is only to determine
    // the size of the output buffer.
    //
    
    status = SdbusEnumAcpiChildren(FdoExtension->LowerDevice,
                                   &dummyBuffer,
                                   sizeof(ACPI_ENUM_CHILDREN_OUTPUT_BUFFER));

    if (status != STATUS_BUFFER_OVERFLOW) {

        //
        // Unexpected return value from the ACPI driver.
        //

        status = STATUS_ACPI_INVALID_DATA;
        goto Cleanup;
    }


    //
    // The NumberOfChildren indicates the required output buffer size.
    // Allocate sufficient memory for the output and enumerate.
    //     
    
    if (dummyBuffer.NumberOfChildren < sizeof(ACPI_ENUM_CHILDREN_OUTPUT_BUFFER)) {
        
        status = STATUS_ACPI_INVALID_DATA;
        goto Cleanup;
    }

    bufferSize = dummyBuffer.NumberOfChildren;
    outputEnumBuffer = (PACPI_ENUM_CHILDREN_OUTPUT_BUFFER)
        ExAllocatePoolWithTag(NonPagedPoolNx, bufferSize, SDBUS_TAG_ACPI_INFORMATION);
  
    if (outputEnumBuffer == NULL) {

        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    RtlZeroMemory(outputEnumBuffer, bufferSize);

    status = SdbusEnumAcpiChildren(FdoExtension->LowerDevice,
                                   outputEnumBuffer,
                                   bufferSize);

    NT_ASSERT(NT_SUCCESS(status));

    if (outputEnumBuffer->NumberOfChildren == 0) {
        
        status =  STATUS_ACPI_INVALID_DATA;
        goto Cleanup;
    }

    acpiEnumChild = &(outputEnumBuffer->Children[0]);

    for (count = 1; count < outputEnumBuffer->NumberOfChildren; count++) {

        //
        // Skip the first child since ACPI returns the SDHC device itself as the first 'child'.
        //

        acpiEnumChild = ACPI_ENUM_CHILD_NEXT(acpiEnumChild);

        //
        // If this child does not have children, it means that it is a method and not a device,
        // because methods do not have children, and devices have at least the _ADR method.
        //

        if (!(acpiEnumChild->Flags & ACPI_OBJECT_HAS_CHILDREN)) {
            continue;
        }

        //
        // For each child device, get the _ADR to determine the number of the socket into which
        // the device is plugged in.
        //

        RtlInitAnsiString(&targetDeviceName, (PCSZ)&acpiEnumChild->Name[0]);

        status = SdbusEvalAcpiMethod(FdoExtension->LowerDevice,
                                     &targetDeviceName,
                                     ACPI_METHOD_ADR,
                                     NULL,
                                     0,
                                     &outputEvalBuffer,
                                     sizeof(outputEvalBuffer));

        //
        // SD devices should always have and _ADR. If _ADR is not present or returned invalid data,
        // abort further queries and fall back to safe bet.
        //

        NT_ASSERT(NT_SUCCESS(status));

        if (outputEvalBuffer.OutputBuffer.Argument->Type != ACPI_METHOD_ARGUMENT_INTEGER) {

            status = STATUS_ACPI_INVALID_DATA;
            goto Cleanup;
        }

        //
        // The socket number is given by the high word of the _ADR.
        //

        socketNumber = outputEvalBuffer.OutputBuffer.Argument->Argument >> (sizeof(ULONG) * 4);
        NT_ASSERT(socketNumber < FdoExtension->NumberOfSockets);

        //
        // Evaluate the _RMV method to determine removability. If _RMV is not present or returns
        // invalid data, just fall back to safe bet and move on to parse other device nodes.
        //

        RtlInitAnsiString(&targetDeviceName, (PCSZ)&acpiEnumChild->Name[0]);
                
        status = SdbusEvalAcpiMethod(FdoExtension->LowerDevice,
                                     &targetDeviceName,
                                     ACPI_METHOD_RMV,
                                     NULL,
                                     0,
                                     &outputEvalBuffer,
                                     sizeof(outputEvalBuffer));
        
        if (NT_SUCCESS(status)) {

            if (outputEvalBuffer.OutputBuffer.Argument->Type != ACPI_METHOD_ARGUMENT_INTEGER) {

                FdoExtension->Socket[socketNumber]->Settings.NonremovableDevice = safebetSetting;
                continue;
            }

            if (outputEvalBuffer.OutputBuffer.Argument->Argument == 0) {

                FdoExtension->Socket[socketNumber]->Settings.NonremovableDevice = TRUE;
            }         
        } else {
        
            FdoExtension->Socket[socketNumber]->Settings.NonremovableDevice = safebetSetting;
        }
    }


Cleanup:

    if (outputEnumBuffer != NULL) {
        ExFreePoolWithTag(outputEnumBuffer, SDBUS_TAG_ACPI_INFORMATION);
    }

    if (!NT_SUCCESS(status)) {

        //
        // A big enough error occured that we need to set back to safe bet setting.
        //

        for (count = 0; count < FdoExtension->NumberOfSockets; count++) {
            FdoExtension->Socket[count]->Settings.NonremovableDevice = safebetSetting;
        }
    }
    
    return status;
}


NTSTATUS
SdbusQueryD3ColdInterface(
    __in    PFDO_EXTENSION              FdoExtension,
    __out   PD3COLD_SUPPORT_INTERFACE   D3ColdInterface
    )

/*++

Routine Description:

    Queries ACPI for the D3Cold support interface.

Arguments:

    FdoExtension - pointer to the device extension of the host controller's FDO
    D3ColdInterface - output buffer receiving the interface

Return Value:

    NTSTATUS

--*/

{
    PIRP                irp = NULL;
    NTSTATUS            status = STATUS_SUCCESS;
    PDEVICE_OBJECT      targetDevice = NULL;
    PIO_STACK_LOCATION  irpStack = NULL;

    PAGED_CODE();

    RtlZeroMemory(D3ColdInterface, sizeof (D3COLD_SUPPORT_INTERFACE));

    //
    // Query D3COLD support interface synchronously
    //

    targetDevice = IoGetAttachedDeviceReference(FdoExtension->DeviceObject);

    irp = IoAllocateIrp(targetDevice->StackSize, FALSE);
    
    if (irp == NULL)
    {
        status = STATUS_INSUFFICIENT_RESOURCES;

        goto Cleanup;
    }

    irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
    irp->IoStatus.Information = 0;

    irpStack = IoGetNextIrpStackLocation(irp);
    irpStack->MajorFunction = IRP_MJ_PNP;
    irpStack->MinorFunction = IRP_MN_QUERY_INTERFACE;
    irpStack->Parameters.QueryInterface.InterfaceType = (LPGUID) &GUID_D3COLD_SUPPORT_INTERFACE;
    irpStack->Parameters.QueryInterface.Size = sizeof (D3COLD_SUPPORT_INTERFACE);
    irpStack->Parameters.QueryInterface.Version = D3COLD_SUPPORT_INTERFACE_VERSION;
    irpStack->Parameters.QueryInterface.Interface = (PINTERFACE) D3ColdInterface;
    irpStack->Parameters.QueryInterface.InterfaceSpecificData = NULL;

    status = IoSynchronousCallDriver(targetDevice, irp);  

    IoFreeIrp(irp);

    if (!NT_SUCCESS(status))
    {
        goto Cleanup;
    }

    NT_ASSERT(D3ColdInterface->SetD3ColdSupport != NULL);
    NT_ASSERT(D3ColdInterface->GetIdleWakeInfo != NULL);
    NT_ASSERT(D3ColdInterface->GetD3ColdCapability != NULL);

Cleanup:

    ObDereferenceObject(targetDevice);

    return status;
}


NTSTATUS
SdbusGetDeepestWakeableDstate(
    __in    PFDO_EXTENSION              FdoExtension
    )
{
    NTSTATUS                            status = STATUS_SUCCESS;
    PD3COLD_SUPPORT_INTERFACE           d3ColdInterface = NULL;

    PAGED_CODE();

    status = SdbusQueryD3ColdInterface(FdoExtension, &FdoExtension->D3ColdInterface);

    if (!NT_SUCCESS(status))
    {
        DebugPrint((SDBUS_DEBUG_WARNING,
                    "fdo %p query D3Cold interface - error(%08x)",
                    FdoExtension->DeviceObject, 
                    status));

        goto Cleanup;
    }

    d3ColdInterface = &FdoExtension->D3ColdInterface;

    status = d3ColdInterface->GetIdleWakeInfo(d3ColdInterface->Context,
                                              PowerSystemWorking,
                                              &FdoExtension->DeepestWakeableDstate);

    if (NT_SUCCESS(status)) {

        //
        // Since SDHCs only have D0 and D3 defined, the returned deepest wake-able D-state
        // should be a variant of D3, otherwise sdbus will not know what to do for D1 or D2
        // since they are not defined anywhere.
        //

        NT_ASSERT((FdoExtension->DeepestWakeableDstate == DeviceWakeDepthD3hot) ||
                  (FdoExtension->DeepestWakeableDstate == DeviceWakeDepthD3cold) ||
                  (FdoExtension->DeepestWakeableDstate == DeviceWakeDepthNotWakeable));

        FdoExtension->PowerNotRequiredTargetState = 
            MapWakeDepthToDstate(FdoExtension->DeepestWakeableDstate);

    } else {

        FdoExtension->DeepestWakeableDstate = DeviceWakeDepthNotWakeable;
        FdoExtension->PowerNotRequiredTargetState = PowerDeviceUnspecified;
    }

Cleanup:

    return status;
}


