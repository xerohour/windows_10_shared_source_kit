/*++

Copyright (c) 2006  Microsoft Corporation

Module Name:

    sqm.c

Abstract:

    This module contains the code to collect and upload SQM data

Author:

    Yue Zuo (yuezuo) 07-Nov-2006

--*/

#include "pch.h"
#include "storsqm.h"
#include "sqm.tmh"

#ifdef SQM_SUPPORT

static const DWORD DATAID_SDBUS_SOCKET_CAPABILITIES =            2832;
static const DWORD DATAID_SDBUS_NUM_OF_UNRECOGNIZED_CARDS =      2833;
static const DWORD DATAID_SDBUS_NUM_OF_SOCKETS =                 2834;
static const DWORD DATAID_SDBUS_NUM_OF_RECOGNIZED_CARDS =        2835;
static const DWORD DATAID_DEPDIAG_BOOT_FIRMWARE_TYPE =           2837;
static const DWORD DATAID_SDBUS_CARD_INFORMATION =               2838; // (Stream)
static const DWORD DATAID_SDBUS_FAILED_ACTIVATION =              2840; // (Stream)
static const DWORD DATAID_SDBUS_CONTROLLER_VENDOR_ID =           2839;
static const DWORD DATAID_SDBUS_CONTROLLER_DEVICE_ID =           2841;
static const DWORD DATAID_SDBUS_CONTROLLER_REVISION_ID =         6431;
static const DWORD DATAID_SDBUS_CONTROLLER_SUBSYS_ID =           6614;
static const DWORD DATAID_SDBUS_CONTROLLER_TYPE =                2836;
static const DWORD DATAID_SDBUS_USAGE_INFO_ID =                  6063;


NTSTATUS
SdbusSqmStart(
    VOID
    )

/*++

Routine Description:

    This routine initializes the data strutures needed for SQM event logging.

Arguments:

Return value:

    Status

--*/

{
    return StorSqmInitialize();
}



VOID
SdbusSqmEnd(
    VOID
    )

/*++

Routine Description:

Arguments:

Return value:

--*/

{
    StorSqmCleanup();
}



VOID
SdbusSqmInsertControllerData(
    IN  PDEVICE_OBJECT Fdo
    )

/*++

Routine Description: 
 
    This routine collects the per-contoller data ponints.

Arguments: 
 
    Fdo - the FDO device object of the controller

Return value:

--*/

{
    PFDO_EXTENSION  fdoExtension = Fdo->DeviceExtension;

    //
    // Set the vendor ID and device ID of the contoller
    //

    StorSqmSetDWORD(NULL,
                    DATAID_SDBUS_CONTROLLER_VENDOR_ID,
                    fdoExtension->PciControllerVendorID);

    StorSqmSetDWORD(NULL,
                    DATAID_SDBUS_CONTROLLER_DEVICE_ID,
                    fdoExtension->PciControllerDeviceID);

    StorSqmSetDWORD(NULL,
                    DATAID_SDBUS_CONTROLLER_SUBSYS_ID,
                    fdoExtension->PciControllerSubsysID);

    StorSqmSetDWORD(NULL,
                    DATAID_SDBUS_CONTROLLER_REVISION_ID,
                    fdoExtension->PciControllerRevisionID);

    //
    // Set the socket information
    //

    StorSqmSetDWORD(NULL,
                    DATAID_SDBUS_NUM_OF_SOCKETS,
                    fdoExtension->NumberOfSockets);

    if (fdoExtension->FunctionBlock == &SdhcSupportFns) {

        StorSqmSetDWORD(NULL,
                        DATAID_SDBUS_CONTROLLER_TYPE,
                        1);

        if ((fdoExtension->NumberOfSockets > 0) &&
            (fdoExtension->Socket[0] != NULL)) {

            //
            // read SDHC_CAPABILITIES for standard controller
            // 

            ULONG capacities;
            capacities = SdbusReadRegisterUlong(fdoExtension->Socket[0],
                                                0x40);

            StorSqmSetDWORD(NULL,
                            DATAID_SDBUS_SOCKET_CAPABILITIES,
                            capacities);
        }

    } else {

        StorSqmSetDWORD(NULL,
                        DATAID_SDBUS_CONTROLLER_TYPE,
                        0);
    }
}


VOID
SdbusSqmInsertCardData(
    IN PSD_CARD_DATA CardData,
    IN PSD_SOCKET Socket
    )

/*++

Routine Description: 
 
    This routine collects the per-card data ponints.

Arguments:

Return value:

--*/

{
    SQM_STREAM_ENTRY sqmStreamEntry[9] = {0};
    ULONG sqmData;

    //
    // sqmStreamEntry[0] = VendorId
    //

    if (CardData->NumIoFunctions > 0) {
        sqmData = CardData->Sdio.FuncCfg[0].MfgId;
    } else {
        sqmData = CardData->Mem.ManufacturerId;
    }

    StorSqmCreateDWORDStreamEntry(&sqmStreamEntry[0], sqmData);


    //
    // sqmStreamEntry[1] = DeviceId
    //

    if (CardData->NumIoFunctions > 0) {
        sqmData = CardData->Sdio.FuncCfg[0].MfgInfo;
    } else {
        sqmData = CardData->Mem.OemId;
    }

    StorSqmCreateDWORDStreamEntry(&sqmStreamEntry[1], sqmData);


    //
    // sqmStreamEntry[2] = RevisionId
    //

    if (CardData->NumIoFunctions > 0) {
        sqmData = 0;
    } else {
        sqmData = CardData->Mem.Revision;
    }

    StorSqmCreateDWORDStreamEntry(&sqmStreamEntry[2], sqmData);


    //
    // sqmStreamEntry[3] = SpecRev
    //

    if (CardData->NumIoFunctions > 0) {
        sqmData = Socket->CardData.Sdio.SpecVersion;
    } else {
        sqmData = Socket->CardData.Mem.SpecVersion;
    }

    StorSqmCreateDWORDStreamEntry(&sqmStreamEntry[3], sqmData);


    //
    // sqmStreamEntry[4] = CardType
    //

    sqmData = (CardData->MemFunction << 8) + CardData->NumIoFunctions;

    StorSqmCreateDWORDStreamEntry(&sqmStreamEntry[4], sqmData);


    //
    // sqmStreamEntry[5] = MemCapacity
    //

    if (CardData->NumIoFunctions > 0) {
        sqmData = 0;
    } else {
        sqmData = (ULONG) CardData->Mem.DeviceCapacity / (1024 * 1024);
    }

    StorSqmCreateDWORDStreamEntry(&sqmStreamEntry[5], sqmData);


    //
    // sqmStreamEntry[6] = MaxClockSpeed
    //

    sqmData = ((Socket->CardData.MaxHighFrequency / 1000) << 16) +
              (Socket->CardData.MaxNormalFrequency / 1000);

    StorSqmCreateDWORDStreamEntry(&sqmStreamEntry[6], sqmData);


    //
    // sqmStreamEntry[7] = SdioCapacities
    //

    if (CardData->NumIoFunctions > 0) {
        sqmData = CardData->Sdio.CardCapabilities;
    } else {
        sqmData = 0;
    }

    if (Socket->CardData.SupportHighCapacity) {
        sqmData |= 1 << 8;
    }

    if (Socket->CardData.SupportWideBus) {
        sqmData |= 1 << 9;
    }

    if (Socket->CardData.SupportHighSpeed) {
        sqmData |= 1 << 10;
    }

    StorSqmCreateDWORDStreamEntry(&sqmStreamEntry[7], sqmData);


    //
    // sqmStreamEntry[8] = SdioFuncIf
    //

    sqmData = 0;
    if (CardData->NumIoFunctions > 0) {

        int     i;
        for (i = 4; i >= 1; i --) {
            sqmData = (sqmData << 8) + CardData->Sdio.FuncCfg[i].IoDeviceInterface;
        }
    }

    StorSqmCreateDWORDStreamEntry(&sqmStreamEntry[8], sqmData);

    StorSqmAddToStream(NULL,
                       DATAID_SDBUS_CARD_INFORMATION,
                       ARRAYSIZE(sqmStreamEntry),
                       sqmStreamEntry);
}


VOID
SdbusSqmInsertActivationFailureData(
    IN SOCKET_ACTIVATION_TYPE ActivationType,
    IN ULONG ActivationPhase,
    IN NTSTATUS ActivationStatus,
    IN ULONG FunctionPhase
    )

/*++

Routine Description:

Arguments:

Return value:

--*/

{
    SQM_STREAM_ENTRY sqmStreamEntry[4] = {0};

    StorSqmCreateDWORDStreamEntry(&sqmStreamEntry[0], ActivationType);
    StorSqmCreateDWORDStreamEntry(&sqmStreamEntry[1], ActivationPhase);
    StorSqmCreateDWORDStreamEntry(&sqmStreamEntry[2], ActivationStatus);
    StorSqmCreateDWORDStreamEntry(&sqmStreamEntry[3], FunctionPhase);

    StorSqmAddToStream(NULL,
                       DATAID_SDBUS_FAILED_ACTIVATION,
                       ARRAYSIZE(sqmStreamEntry),
                       sqmStreamEntry);
}


VOID
SdbusSqmInsertUsageData(
    IN  PDEVICE_OBJECT Fdo
    )

/*++

Routine Description:

Arguments:

Return value:

--*/

{
    PFDO_EXTENSION fdoExtension = Fdo->DeviceExtension;

    StorSqmIncrementDWORD(NULL,
                          DATAID_SDBUS_NUM_OF_RECOGNIZED_CARDS,
                          fdoExtension->SqmData.SdbusNumOfRecognizedCards);

    StorSqmIncrementDWORD(NULL,
                          DATAID_SDBUS_NUM_OF_UNRECOGNIZED_CARDS,
                          fdoExtension->SqmData.SdbusNumOfUnrecognizedCards);

    StorSqmIncrementDWORD(NULL,
                          DATAID_SDBUS_USAGE_INFO_ID,
                          fdoExtension->SqmData.SdbusUsageInfo);

}


#endif