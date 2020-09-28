/*++

Copyright (c) 2002 Microsoft Corporation

Module Name:

    enum.c

Abstract:

    This module contains the bus enum code for SDBUS driver

Authors:

    Neil Sandlin (neilsa) 1-Jan-2002

Environment:

    Kernel mode only

Notes:

Revision History:

--*/

#include "pch.h"
#include "enum.tmh"

//
// Internal References
//

NTSTATUS
SdbusCreatePdo(
    __in PDEVICE_OBJECT Fdo,
    __in PSD_SOCKET Socket,
    __deref_out PDEVICE_OBJECT *PdoPtr
    );

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, SdbusEnumerateDevices)
    #pragma alloc_text(PAGE, SdbusCreatePdo)
#endif

NTSTATUS
SdbusEnumerateSocketPdos(
    __in PDEVICE_OBJECT Fdo,
    __in PSD_SOCKET Socket
    )
{
    PPDO_EXTENSION pdoExtension;
    PDEVICE_OBJECT pdo;
    ULONG_PTR information = (ULONG_PTR) NULL;
    PSD_CARD_DATA cardData = NULL;
    NTSTATUS status;
    UCHAR function;

    //
    // Get a copy of the card data
    //

    status = SdbusExecuteWorkSynchronous(SDWP_ENUMERATE_DEVICES,
                                         Socket,
                                         NULL,
                                         &information);
    if (!NT_SUCCESS(status)) {
        DebugPrint((SDBUS_DEBUG_FAIL,
                    "skt %p Error: can't get card data!!! status:%x",
                    Socket,
                    status));
        return status;
    }

    cardData = (PSD_CARD_DATA) information;

    if (cardData == NULL) {
        DebugPrint((SDBUS_DEBUG_ENUM,
                    "skt %p enumerate devices: same card. no need to enumerate",
                    Socket));
        return STATUS_SUCCESS;
    }

    for (function=1; function <= cardData->NumIoFunctions; function++) {

        status = SdbusCreatePdo(Fdo, Socket, &pdo);
        if (NT_SUCCESS(status)) {

            DebugPrint((SDBUS_DEBUG_ENUM,
                        "skt %p created PDO %p",
                        Socket,
                        pdo));

            //
            // initialize the pointers
            //

            pdoExtension = pdo->DeviceExtension;
            pdoExtension->Function = function;
            pdoExtension->FunctionType = SDBUS_FUNCTION_TYPE_SDIO;
            RtlCopyMemory(&pdoExtension->CardData,
                          cardData,
                          sizeof(pdoExtension->CardData));

        } else {
            DebugPrint((SDBUS_DEBUG_WARNING,
                        "skt %p Error creating PDO %08x",
                        Socket,
                        status));
        }
    }

    if (IsTypeMemory(cardData->MemFunction)) {

        status = SdbusCreatePdo(Fdo, Socket, &pdo);

        if (NT_SUCCESS(status)) {


            DebugPrint((SDBUS_DEBUG_ENUM,
                        "skt %p created PDO %p\n",
                        Socket,
                        pdo));

            //
            // initialize the pointers
            //

            pdoExtension = pdo->DeviceExtension;
            pdoExtension->Function = 8;
            pdoExtension->FunctionType = cardData->MemFunction;
            RtlCopyMemory(&pdoExtension->CardData,
                          cardData,
                          sizeof(pdoExtension->CardData));

            pdoExtension->MediaChangeCount++;

        } else {

            DebugPrint((SDBUS_DEBUG_WARNING,
                        "skt %p Error creating PDO %08x",
                        Socket,
                        status));
        }
    }

    ExFreePoolWithTag(cardData, SDBUS_TAG_CARD);

    return status;
}

VOID
SdbusUpdateDevicePdoCount(
    __in PDEVICE_OBJECT Fdo
    )
{

    PFDO_EXTENSION FdoExtension;
    ULONG Index;
    PDEVICE_OBJECT Pdo;
    PPDO_EXTENSION PdoExtension;
    PSD_SOCKET Socket;

    //
    // Update the number of PDOs if the card has changed.
    //

    FdoExtension = Fdo->DeviceExtension;
    FdoExtension->LivePdoCount = 0;
    FdoExtension->PoweredPdoCount = 0;
    FdoExtension->StartedPdoCount = 0;
    for (Index = 0; Index < FdoExtension->NumberOfSockets; Index += 1) {
        Socket = FdoExtension->Socket[Index];
        for (Pdo = Socket->PdoList; Pdo != NULL; Pdo = PdoExtension->NextPdo) {
            PdoExtension = Pdo->DeviceExtension;
            if (IsDeviceFlagSet(PdoExtension, SDBUS_PDO_PHYSICALLY_REMOVED)) {
                SetDeviceFlag(PdoExtension, SDBUS_PDO_REPORTED_MISSING);
                
            } else {
                FdoExtension->LivePdoCount += 1;
                if (PdoExtension->DevicePowerState == PowerDeviceD0) {
                    FdoExtension->PoweredPdoCount += 1;
                }

                if (PdoExtension->PnpState == SDBUS_DEVICE_STARTED) {
                    FdoExtension->StartedPdoCount += 1;
                }
            }
        }
    }

    //
    // If we have unstarted PDOs, set an idle timeout to allow us to initialize
    // them without the controller going in and out of D3. PNP will keep us
    // active between compeletion of the controller start device IRP and the 
    // completion of the QDR.
    //
    
    if (FdoExtension->StartedPdoCount < (LONG)FdoExtension->LivePdoCount) {
        PoFxSetDeviceIdleTimeout(FdoExtension->FxHandle, SdDeviceIdleTimeout);
    }
    
    DebugPrint((SDBUS_DEBUG_ENUM,
                "fdo %p live pdo count = %d\n",
                Fdo,
                FdoExtension->LivePdoCount));
}

NTSTATUS
SdbusEnumerateSocket(
    __in PDEVICE_OBJECT Fdo,
    __in PSD_SOCKET Socket
    )
/*++

Routine Description:

   This enumerates the sd bus which is represented by Fdo (a pointer to the
   device object representing the sd controller. It creates new PDOs for any
   new PC-Cards which have been discovered since the last enumeration in the
   given socket.

Notes:

Arguments:

   Fdo - Supplies the pointer to the functional device object for the SD
         controller which needs to be enumerated

   Socket - The socket from which to enumerate new PDOs

Return value:

   None

--*/
{
    NTSTATUS status;
    
    status = SdbusEnumerateSocketPdos(Fdo, Socket);

    if (NT_SUCCESS(status)) {
        SdbusUpdateDevicePdoCount(Fdo);
    }

    return status;
}


NTSTATUS
SdbusEnumerateDevices(
    __in PDEVICE_OBJECT Fdo,
    __in PIRP Irp
    )

/*++

Routine Description:

   This enumerates the sd bus which is represented by Fdo (a pointer to the
   device object representing the sd controller. It creates new PDOs for any
   new PC-Cards which have been discovered since the last enumeration

Notes:

Arguments:

   Fdo - Supplies the pointer to the functional device object for the SD
         controller which needs to be enumerated

Return value:

   None

--*/

{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;
    NTSTATUS status;
    ULONG socketNumber;
    PSD_SOCKET socket;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Irp);

    for (socketNumber = 0; socketNumber < fdoExtension->NumberOfSockets; socketNumber++) {
        socket = fdoExtension->Socket[socketNumber];
        status = SdbusEnumerateSocketPdos(Fdo, socket);
    }

    SdbusUpdateDevicePdoCount(Fdo);

    if (fdoExtension->LivePdoCount == 0) {
        //
        // ISSUE: active power management not implemented
        // Hint for the controller to check if it should turn itself off
        //
        //        SdbusFdoCheckForIdle(fdoExtension);
    }

    return STATUS_SUCCESS;
}



NTSTATUS
SdbusCreatePdo(
    __in PDEVICE_OBJECT Fdo,
    __in PSD_SOCKET Socket,
    __deref_out PDEVICE_OBJECT *PdoPtr
    )

/*++

Routine Description:

    Creates and initializes a device object - which will be referred to as a
    Physical Device Object or PDO - for the PC-Card in the socket represented
    by Socket, hanging off the SDBUS controller represented by Fdo.

Arguments:

    Fdo - Functional device object representing the SDBUS controller

    Socket - Socket in which the PC-Card for which we're creating a PDO
             resides

    PdoPtr - Pointer to an area of memory where the created PDO is returned

Return value:

    STATUS_SUCCESS - Pdo creation/initialization successful, PdoPtr contains
                     the pointer to the Pdo

    Any other status - creation/initialization unsuccessful

--*/

{
    ULONG pdoNameIndex = 0;
    ULONG characteristics = 0;
    PPDO_EXTENSION pdoExtension = NULL;
    CHAR deviceName[128];
    ANSI_STRING ansiName;
    UNICODE_STRING unicodeName = {0};
    NTSTATUS status;

    PAGED_CODE();

    if (!Socket->Settings.NonremovableDevice && !Socket->Settings.AlwaysReportFixedDevice) {
        characteristics |= FILE_REMOVABLE_MEDIA;
    }

    if (SDPORT_IsWriteProtected(Socket)) {
        characteristics |= FILE_READ_ONLY_DEVICE;
    }

    //
    // Allocate space for the Unicode string:(handles upto 0xFFFF
    // devices for now :)
    //

    status = RtlStringCbPrintfA(deviceName,
                                sizeof(deviceName),
                                "%s-%d", "\\Device\\SdBus",
                                0xFFFF);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    RtlInitAnsiString(&ansiName, deviceName);

    status = RtlAnsiStringToUnicodeString(&unicodeName,
                                          &ansiName,
                                          TRUE);
    if (!NT_SUCCESS(status)) {
        return status;
    }

    //
    // Attempt to create the device with a unique name
    //

    do {
        status = RtlStringCbPrintfA(deviceName,
                                    sizeof(deviceName),
                                    "%s-%d",
                                    "\\Device\\SdBus",
                                    pdoNameIndex++);
        if (!NT_SUCCESS(status)) {
            RtlFreeUnicodeString(&unicodeName);
            return status;
        }

        RtlInitAnsiString(&ansiName, deviceName);
        status = RtlAnsiStringToUnicodeString(&unicodeName,
                                              &ansiName,
                                              FALSE);

        if (!NT_SUCCESS(status)) {
            RtlFreeUnicodeString(&unicodeName);
            return status;
        }

        status = IoCreateDevice(Fdo->DriverObject,
                                sizeof(PDO_EXTENSION),
                                &unicodeName,
                                FILE_DEVICE_UNKNOWN,
                                characteristics,
                                FALSE,
                                PdoPtr);

    } while ((status == STATUS_OBJECT_NAME_EXISTS) ||
             (status == STATUS_OBJECT_NAME_COLLISION));

    RtlFreeUnicodeString(&unicodeName);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    if (Socket->TransferMethod == SDBUS_TRANSFER_METHOD_ADMA2) {

        //
        // ISSUE: Get this from miniport
        //

        (*PdoPtr)->AlignmentRequirement = MAX((*PdoPtr)->AlignmentRequirement,
                                              7);
    } else if (Socket->TransferMethod == SDBUS_TRANSFER_METHOD_SYSTEM_DMA) {
        (*PdoPtr)->AlignmentRequirement = MAX((*PdoPtr)->AlignmentRequirement,
                                              Fdo->AlignmentRequirement);
    }

    //
    // Initialize the device extension for the PDO
    //

    pdoExtension = (*PdoPtr)->DeviceExtension;
    RtlZeroMemory(pdoExtension, sizeof(PDO_EXTENSION));

    pdoExtension->Signature = SDBUS_PDO_EXTENSION_SIGNATURE;
    pdoExtension->DeviceObject = *PdoPtr;

    //
    // Initialize power states
    //

    pdoExtension->SystemPowerState = PowerSystemWorking;
    pdoExtension->DevicePowerState = PowerDeviceD3;

    //
    // Link this PDO into the socket's PDO list
    //

    pdoExtension->NextPdo = Socket->PdoList;
    Socket->PdoList = *PdoPtr;
    pdoExtension->Socket = Socket;

    INITIALIZE_PNP_STATE(pdoExtension);

    (*PdoPtr)->Flags |= DO_POWER_PAGABLE;
    (*PdoPtr)->Flags &= ~DO_DEVICE_INITIALIZING;

    return STATUS_SUCCESS;
}


