/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    tph.c

Abstract:

    This module contains functions associated with configuring the PCI
    Express-specific feature TPH (TLP Processing Hints).









--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include <initguid.h>
#include "ntpoapi.h"

// --------------------------------------------------------------------- Defines

#define MAX_ST_SIZE_TPH 64
#define MAX_ST_SIZE_MSIX 2048

// ----------------------------------------------------------------------- Types

typedef union _PCI_MSIX_VECTOR_CONTROL {
    struct {
        USHORT Mask:1;
        USHORT Reserved:15;
        USHORT SteeringTag;
    };

    ULONG AsULONG;
} PCI_MSIX_VECTOR_CONTROL, *PPCI_MSIX_VECTOR_CONTROL;

// ------------------------------------------------------------------ Prototypes

VOID
ExpressAllocTphStEntries(
    __inout PEXPRESS_PORT ExpressPort,
    __in ULONG StTableSize
);

NTSTATUS
ExpressSaveTphStEntries(
    __inout PEXPRESS_PORT ExpressPort,
    __in ULONG StTableSize
);

NTSTATUS
ExpressRestoreTphStEntries(
    __inout PEXPRESS_PORT ExpressPort,
    __in ULONG StTableSize
);

NTSTATUS
ExpressSaveTphMsiXStEntries(
    __inout PEXPRESS_PORT ExpressPort,
    __in ULONG StTableSize
);

NTSTATUS
ExpressRestoreMsiXTphStEntries(
    __inout PEXPRESS_PORT ExpressPort,
    __in ULONG StTableSize
);

// ------------------------------------------------------------------ Globals

volatile LONG PciDebugTphSaveDone;
volatile LONG PciDebugTphSaveNotDone;

// ------------------------------------------------------------------- Functions

VOID
ExpressAllocTphStEntries(
    __inout PEXPRESS_PORT ExpressPort,
    __in ULONG StTableSize
)
{

    //
    // If not already allocated, create a place to store the table.
    //

    if (ExpressPort->TphSteeringTags==NULL) {
        ExpressPort->TphSteeringTags = PciAllocatePool(
            NonPagedPoolNx,
            StTableSize * sizeof(PCI_EXPRESS_TPH_ST_TABLE_ENTRY));
    }

    return;
}

NTSTATUS
ExpressSaveTphStEntries(
    __inout PEXPRESS_PORT ExpressPort,
    __in ULONG StTableSize
)

/*++

Routine Description:

    This routine saves/caches the TPH Steering Tag table entries from the end
    of the TPH capability structure.

Arguments:

    ExpressPort - Supplies the Express device whose TPH Steering Tags will be
        saved.

    StTableSize - Supplies the number of entries in the Steering Tag table.

Return Value:

    STATUS_SUCCESS

--*/

{

    NT_ASSERT(StTableSize <= MAX_ST_SIZE_TPH);

    if (StTableSize > MAX_ST_SIZE_TPH) {
        StTableSize = MAX_ST_SIZE_TPH;
    }

    ExpressAllocTphStEntries(ExpressPort, StTableSize);

    if (ExpressPort->TphSteeringTags == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    PciReadDeviceConfig(
        ExpressPort->Device,
        ExpressPort->TphSteeringTags,
        ExpressPort->TphRequesterCapability + sizeof(PCI_EXPRESS_TPH_REQUESTER_CAPABILITY),
        StTableSize * sizeof(PCI_EXPRESS_TPH_ST_TABLE_ENTRY));

    return STATUS_SUCCESS;

}

NTSTATUS
ExpressRestoreTphStEntries(
    __inout PEXPRESS_PORT ExpressPort,
    __in ULONG StTableSize
)

/*++

Routine Description:

    This routine restores the TPH Steering Tag table entries which are
    embedded in at the end of the TPH Capability structure.

Arguments:

    ExpressPort - Supplies the Express device whose TPH Steering Tags will be
        restored.

    StTableSize - Supplies the number of entries in the Steering Tag table.

Return Value:

    STATUS_SUCCESS

--*/

{

    NT_ASSERT(StTableSize <= MAX_ST_SIZE_TPH);

    if (StTableSize > MAX_ST_SIZE_TPH) {
        StTableSize = MAX_ST_SIZE_TPH;
    }

    if (ExpressPort->TphSteeringTags == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    PciWriteDeviceConfig(
        ExpressPort->Device,
        ExpressPort->TphSteeringTags,
        ExpressPort->TphRequesterCapability + sizeof(PCI_EXPRESS_TPH_REQUESTER_CAPABILITY),
        StTableSize * sizeof(PCI_EXPRESS_TPH_ST_TABLE_ENTRY));

    return STATUS_SUCCESS;
}

NTSTATUS
ExpressSaveTphMsiXStEntries(
    __inout PEXPRESS_PORT ExpressPort,
    __in ULONG StTableSize
)

/*++

Routine Description:

    This routine saves/caches the TPH Steering Tag table entries which are
    embedded in the MSI-X table.

Arguments:

    ExpressPort - Supplies the Express device whose TPH Steering Tags will be
        saved.

    StTableSize - Supplies the number of entries in the Steering Tag table.

Return Value:

    STATUS_SUCCESS

--*/

{
    PPCI_DEVICE Device;
    PPCI_MSIX_TABLE_ENTRY Table;
    ULONG TableEntry;
    PCI_MSIX_VECTOR_CONTROL VectorControl;

    NT_ASSERT(StTableSize <= MAX_ST_SIZE_MSIX);

    if (StTableSize > MAX_ST_SIZE_MSIX) {
        StTableSize = MAX_ST_SIZE_MSIX;
    }

    ExpressAllocTphStEntries(ExpressPort, StTableSize);

    if (ExpressPort->TphSteeringTags == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    Device = ExpressPort->Device;
    Table = Device->InterruptResource.MsiX.Table;

    NT_ASSERT(Table != NULL);
#pragma prefast(suppress: __WARNING_REDUNDANT_POINTER_TEST)
    if (Table == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // The PCI Conventional Spec says:
    //  "For all accesses to MSI-X Table and MSI-X PBA fields, software must
    //   use aligned full DWORD or aligned full QWORD transactions; otherwise,
    //   the result is undefined."
    //
    // Therefore, access the steering tags using 32-bit macros.
    //

    for (TableEntry = 0; TableEntry < StTableSize; TableEntry++) {
        READ_REGISTER_BUFFER_ULONG(
            (PULONG)&Table[TableEntry].VectorControl,
            &VectorControl.AsULONG,
            1);

        ExpressPort->TphSteeringTags[TableEntry].AsUSHORT =
            VectorControl.SteeringTag;
    }

    return STATUS_SUCCESS;

}

NTSTATUS
ExpressRestoreTphMsiXStEntries(
    __inout PEXPRESS_PORT ExpressPort,
    __in ULONG StTableSize
)

/*++

Routine Description:

    This routine restores the TPH Steering Tag table entries which are
    embedded in the MSI-X table.

Arguments:

    ExpressPort - Supplies the Express device whose TPH Steering Tags will be
        restored.

    StTableSize - Supplies the number of entries in the Steering Tag table.

Return Value:

    STATUS_SUCCESS

--*/

{
    PPCI_DEVICE Device;
    PPCI_MSIX_TABLE_ENTRY Table;
    ULONG TableEntry;
    PCI_MSIX_VECTOR_CONTROL VectorControl;

    NT_ASSERT(StTableSize <= MAX_ST_SIZE_MSIX);

    if (StTableSize > MAX_ST_SIZE_MSIX) {
        StTableSize = MAX_ST_SIZE_MSIX;
    }

    if (ExpressPort->TphSteeringTags == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    Device = ExpressPort->Device;
    Table = Device->InterruptResource.MsiX.Table;

    NT_ASSERT(Table != NULL);
#pragma prefast(suppress: __WARNING_REDUNDANT_POINTER_TEST)
    if (Table == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // The PCI Conventional Spec says:
    //  "For all accesses to MSI-X Table and MSI-X PBA fields, software must
    //   use aligned full DWORD or aligned full QWORD transactions; otherwise,
    //   the result is undefined."
    //
    // Therefore, access the steering tags using 32-bit macros.
    //

    for (TableEntry = 0; TableEntry < StTableSize; TableEntry++) {
        READ_REGISTER_BUFFER_ULONG(
            (PULONG)&Table[TableEntry].VectorControl,
            &VectorControl.AsULONG,
            1);

        VectorControl.SteeringTag =
            ExpressPort->TphSteeringTags[TableEntry].AsUSHORT;

        WRITE_REGISTER_BUFFER_ULONG(
            (PULONG)&Table[TableEntry].VectorControl,
            &VectorControl.AsULONG,
            1);
    }

    return STATUS_SUCCESS;

}

NTSTATUS
ExpressSaveTphRequesterCapability (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine processes the PCI Express device's TPH Requester capability.

Arguments:

    ExpressPort - Supplies the Express device whose TPH Requester capability
        will be processed.

Return Value:

    STATUS_SUCCESS

--*/

{

    ULONG ActualStTableSize;
    PCI_DEVICE_ID_DATA IdData;
    NTSTATUS Status;
    PPCI_EXPRESS_TPH_REQUESTER_CAPABILITY Tph;

    PCI_ASSERT(ExpressPort->TphRequesterCapability != 0);

    //
    // Save the information in the Tph Requester Capability so it can
    // be restored on future power up.
    //

    Tph = &ExpressPort->TphRequesterCapabilityBlock;
    PciReadDeviceConfig(ExpressPort->Device,
                        Tph,
                        ExpressPort->TphRequesterCapability,
                        sizeof(PCI_EXPRESS_TPH_REQUESTER_CAPABILITY));

    //
    // Determine where the steering tags are stored and save them
    // so they can be restored on future power up.
    // The StTableSize value is in N-1 form. 0 means there is 1 entry.
    // Add one to get the actual table size.
    //

    ActualStTableSize = Tph->RequesterCapability.StTableSize + 1;
    switch (Tph->RequesterCapability.StTableLocation) {
    case PCI_EXPRESS_TPH_ST_LOCATION_TPH_CAPABILITY:
        Status = ExpressSaveTphStEntries(ExpressPort, ActualStTableSize);
        break;

    case PCI_EXPRESS_TPH_ST_LOCATION_MSIX_TABLE:
        Status = ExpressSaveTphMsiXStEntries(ExpressPort, ActualStTableSize);
        break;

    case PCI_EXPRESS_TPH_ST_LOCATION_NONE:
        Status = STATUS_SUCCESS;
        break;

    case PCI_EXPRESS_TPH_ST_LOCATION_RESERVED:
    default:
        PCI_ASSERT(FALSE);
        Status = STATUS_INVALID_PARAMETER;
        break;
    }

    if (NT_SUCCESS(Status) &&
        NT_SUCCESS(PciReadDeviceIdData(ExpressPort->Device, &IdData)) &&
        PciIsSameDevice(ExpressPort->Device, &IdData)) {

        ExpressPort->TphRequesterCapabilitySaved = TRUE;
        InterlockedIncrement(&PciDebugTphSaveDone);

    } else {
        ExpressPort->TphRequesterCapabilitySaved = FALSE;
        InterlockedIncrement(&PciDebugTphSaveNotDone);
        TraceLoggingWrite(PciTraceLoggingProvider,
                          "PciDebugTphSave",
                          TraceLoggingInt32(PciDebugTphSaveNotDone, "NotDone"),
                          TraceLoggingInt32(PciDebugTphSaveDone, "Done"),
                          TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES));
    }

    return Status;
}

NTSTATUS
ExpressRestoreTphRequesterCapability (
    __inout PEXPRESS_PORT ExpressPort
    )

/*++

Routine Description:

    This routine restores the PCI Express device's TPH Requester capability.

Arguments:

    ExpressPort - Supplies the Express device whose TPH capability will
        be restored.

Return Value:

    STATUS_SUCCESS

--*/

{
    PPCI_EXPRESS_TPH_REQUESTER_CAPABILITY Tph;
    ULONG ActualStTableSize;

    if (ExpressPort->TphRequesterCapabilitySaved == FALSE) {
        return STATUS_SUCCESS;
    }

    PCI_ASSERT(ExpressPort->TphRequesterCapability != 0);

    //
    // Restore the RequesterControl register. This is the only writeable
    // register (other than the steering tags) in the TPH Capability block.
    //

    Tph = &ExpressPort->TphRequesterCapabilityBlock;
    PciWriteTphRequesterRegister(
        ExpressPort,
        RequesterControl,
        &Tph->RequesterControl.AsULONG);

    //
    // Restore the Steering Tag (ST) table entries
    // The StTableSize field is encoded as N-1. The actual table size is
    // determined by adding 1 to the value in the capability.
    //

    ActualStTableSize = Tph->RequesterCapability.StTableSize + 1;
    switch (Tph->RequesterCapability.StTableLocation) {
    case PCI_EXPRESS_TPH_ST_LOCATION_TPH_CAPABILITY:
        ExpressRestoreTphStEntries(ExpressPort, ActualStTableSize);
        break;

    case PCI_EXPRESS_TPH_ST_LOCATION_MSIX_TABLE:
        ExpressRestoreTphMsiXStEntries(ExpressPort, ActualStTableSize);
        break;

    case PCI_EXPRESS_TPH_ST_LOCATION_NONE:
        break;

    case PCI_EXPRESS_TPH_ST_LOCATION_RESERVED:
    default:
        PCI_ASSERT(FALSE);
        break;
    }

    return STATUS_SUCCESS;
}
