/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    eficfg.c

Abstract:

    This module implements PCI configuration space access routines using the
    EFI PCI Root Bus Protocol.

Environment:

    Boot Environment.

--*/

// ------------------------------------------------------------------- Includes

#include "platform.h"

// ---------------------------------------------------------------------- Types

#define TAG_END 0x78
#define TAG_QWORD_ADDRESS 0x8a

#define LARGE_RESOURCE_TAG 0x80
#define SMALL_TAG_MASK 0xf8

#define ADDRESS_MEMORY_TYPE 0x0
#define ADDRESS_IO_TYPE 0x1
#define ADDRESS_BUS_NUMBER_TYPE 0x2

#include <pshpack1.h>

typedef struct _QWORD_ADDRESS_DESCRIPTOR {
    UCHAR Tag;                    // 10001010B, Large item name= 10
    USHORT Length;                // Length of the descriptor = 23 (min)
    UCHAR RFlag;                  // Resource Flags
    UCHAR GFlag;                  // General Flags
    UCHAR TFlag;                  // Type specific flags
    ULONGLONG Granularity;        // Memory Decode Bits
    ULONGLONG MinimumAddress;     // Minium Address in range
    ULONGLONG MaximumAddress;     // Maximum Address in range
    ULONGLONG TranslationAddress; // How to translate the address
    ULONGLONG AddressLength;      // Number of bytes requested
} QWORD_ADDRESS_DESCRIPTOR, *PQWORD_ADDRESS_DESCRIPTOR;

#include <poppack.h>

typedef struct _PROTOCOL_CONTEXT {
    EFI_HANDLE Handle;
    PVOID Interface;
} PROTOCOL_CONTEXT, *PPROTOCOL_CONTEXT;

// ----------------------------------------------------------------- Prototypes

NTSTATUS
PltpAccessConfig (
    __in ULONG64 Context,
    __in PPCI_CONFIG_ADDRESS DeviceAddress,
    _When_(ReadAccess != FALSE, __out_bcount(Length))
    _When_(ReadAccess == FALSE, __in_bcount(Length))
         PUCHAR Buffer,
    __in ULONG Offset,
    __in ULONG Length,
    __in BOOLEAN ReadAccess
    );

NTSTATUS
PltpCreateConfigEntry (
    __in EFI_PCI_ROOT_BRIDGE_IO_INTERFACE *Interface,
    __in EFI_HANDLE Handle,
    __deref_out PPCI_CONFIG_ENTRY *ConfigEntryOut
    );

VOID
PltpDestroyConfigTableEntry (
    __in PPCI_CONFIG_ENTRY Entry
    );

NTSTATUS
PltpDestroyTableMapRoutine (
    __in PPCI_CONFIG_ENTRY Entry,
    __in ULONG Index
    );

NTSTATUS
PltpGetRootBusResources (
    __in EFI_PCI_ROOT_BRIDGE_IO_INTERFACE *Interface,
    __out PVOID *ResourcesOut
    );

// ------------------------------------------------------------------- Routines

VOID
PltDestroyPciConfiguration (
    VOID
    )

/*++

Routine Description:

    This routine destroys PCI configuration space access using the EFI
    PCI Root Bus Protocol.

Arguments:

    None.

Return Value:

    None.

--*/

{

    if (PltPciConfigTable != NULL) {
        BlTblMap(PltPciConfigTable,
                 PltPciConfigTableEntries,
                 PltpDestroyTableMapRoutine,
                 FALSE);

        BlMmFreeHeap(PltPciConfigTable);
        PltPciConfigTable = NULL;
    }

    return;
}

NTSTATUS
PltInitializePciConfiguration (
    VOID
    )

/*++

Routine Description:

    This routine initializes PCI configuration space access using the EFI
    PCI Root Bus Protocol.

Arguments:

    None.

Return Value:

    NT Status code.

--*/

{

    PPCI_CONFIG_ENTRY ConfigEntry;
    ULONG EntryId;
    EFI_HANDLE *HandleBuffer;
    UINTN HandleCount;
    UINTN Index;
    PVOID Interface;
    NTSTATUS Status;

    HandleBuffer = NULL;

    //
    // Initialize the config access table.
    //

    PltPciConfigTableEntries = INITIAL_PCI_CONFIG_TABLE_SIZE;
    PltPciConfigTable = BlMmAllocateHeap(PltPciConfigTableEntries *
                                         sizeof(PPCI_CONFIG_ENTRY));

    if (PltPciConfigTable == NULL) {
        Status = STATUS_NO_MEMORY;
        goto InitializePciConfigurationEnd;
    }

    RtlZeroMemory(PltPciConfigTable,
                  PltPciConfigTableEntries * sizeof(PPCI_CONFIG_ENTRY));

    //
    // Retrieve the list of handles that support the PCI root bridge protocol.
    //

    HandleCount = 0;
    HandleBuffer = NULL;
    Status = EfiLocateHandleBuffer(ByProtocol,
                                   &EfiPciRootBusProtocol,
                                   NULL,
                                   &HandleCount,
                                   &HandleBuffer);

    if (!NT_SUCCESS(Status)) {
        goto InitializePciConfigurationEnd;
    }

    //
    // Open the root bus protocols in turn and create a config access table
    // entry referring to each.  Each protocol can be used to access the config
    // space of devices in the bus range decoded by the root bus that exposes
    // the protocol.
    //

    for (Index = 0; Index < HandleCount; Index += 1) {
        Status = EfiOpenProtocol(HandleBuffer[Index],
                                 &EfiPciRootBusProtocol,
                                 &Interface);

        if (!NT_SUCCESS(Status)) {
            continue;
        }

        Status = PltpCreateConfigEntry(Interface,
                                       HandleBuffer[Index],
                                       &ConfigEntry);

        if (NT_SUCCESS(Status)) {
            Status = BlTblSetEntry(&PltPciConfigTable,
                                   &PltPciConfigTableEntries,
                                   ConfigEntry,
                                   &EntryId,
                                   TblDoNotPurgeEntry);

            if (!NT_SUCCESS(Status)) {
                PltpDestroyConfigTableEntry(ConfigEntry);
            }
        }

        if (!NT_SUCCESS(Status)) {
            EfiCloseProtocol(HandleBuffer[Index],
                             &EfiPciRootBusProtocol,
                             Interface);
        }
    }

    Status = STATUS_SUCCESS;

InitializePciConfigurationEnd:
    if (!NT_SUCCESS(Status)) {
        PltDestroyPciConfiguration();
    }

    if (HandleBuffer != NULL) {
        BlMmFreeHeap(HandleBuffer);
    }

    return STATUS_SUCCESS;
}

// --------------------------------------------------------- Internal Functions

NTSTATUS
PltpAccessConfig (
    __in ULONG64 Context,
    __in PPCI_CONFIG_ADDRESS DeviceAddress,
    _When_(ReadAccess != FALSE, __out_bcount(Length))
    _When_(ReadAccess == FALSE, __in_bcount(Length))
         PUCHAR Buffer,
    __in ULONG Offset,
    __in ULONG Length,
    __in BOOLEAN ReadAccess
    )

/*++

Routine Description:

    This routine performs a single PCI configuration space access using the
    EFI PCI Root Bus I/O Protocol.

Arguments:

    Context - Unused.

    DeviceAddress - Supplies the PCI bus, device and function number of the
        device to be accessed.

    Buffer - Supplies a pointer to the buffer which receives the configuration
        data.

    Offset - Supplies the offset into the device's configuration space that is
        to be accessed.

    Length - Supplies the number of bytes to be read.

    ReadAccess - Supplies a boolean indicating whether the access is a read or
        a write.

Return Value:

    STATUS_SUCCESS if the access succeeds.

    STATUS_INVALID_PARAMETER if the access is not a naturally-aligned access
        of 1, 2 or 4 bytes.

    An error status code if returned from the EFI interface routine.

--*/

{

    UINT64 Address;
    ULONG ExtendedRegister;
    EFI_PCI_ROOT_BRIDGE_IO_INTERFACE *Interface;
    PPROTOCOL_CONTEXT ProtocolContext;
    UCHAR Register;
    EFI_PCI_IO_PROTOCOL_WIDTH Width;

    ProtocolContext = (PPROTOCOL_CONTEXT)(ULONG_PTR)Context;
    Interface = ProtocolContext->Interface;

    //
    // Configuration space must be accessed in naturally aligned lengths of 1,
    // 2 or 4 bytes.
    //

    if ((Length != 1) && (Length != 2) && (Length != 4)) {
        return STATUS_INVALID_PARAMETER;
    }

    if ((Offset % Length) != 0) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Construct the address field from the device address and register offset.
    //

    Register = (UCHAR)(Offset & 0xFF);
    ExtendedRegister = Offset >> 8;
    Address = Register;
    Address |= (DeviceAddress->Function << 8);
    Address |= (DeviceAddress->Device << 16);
    Address |= (DeviceAddress->Bus << 24);
    Address |= (((ULONGLONG)ExtendedRegister) << 32);

    //
    // Choose the correct enumeration value for the width.
    //

    if (Length == 1) {
        Width = EfiPciIoWidthUint8;

    } else if (Length == 2) {
        Width = EfiPciIoWidthUint16;

    } else {
        Width = EfiPciIoWidthUint32;
    }

    //
    // Call the firmware to access the specified address.
    //

    return EfiPciAccessConfigSpace(ProtocolContext->Interface,
                                   Width,
                                   Address,
                                   1,
                                   Buffer,
                                   ReadAccess);
}

NTSTATUS
PltpCreateConfigEntry (
    __in EFI_PCI_ROOT_BRIDGE_IO_INTERFACE *Interface,
    __in EFI_HANDLE Handle,
    __deref_out PPCI_CONFIG_ENTRY *ConfigEntryOut
    )

/*++

Routine Description:

    This routine creates a config access table entry referring to the given
    root bus protocol interface.

Arguments:

    Interface - Supplies the interface for which the config table entry is to
        be created.

    Handle - Supplies the handle to the supplied interface.

    ConfigEntry - Supplies a pointer to a variable that receives a
        heap-allocated config access table entry.  The caller is responsible for
        destroying this entry.

Return Value:

    STATUS_SUCCESS if the table entry is created successfully.

    STATUS_NO_MEMORY if there is a memroy allocation failure.

    STATUS_INVALID_MEMBER if the given root bus protocol does not contain a
        valid resource description.

--*/

{

    PPCI_CONFIG_ENTRY ConfigEntry;
    PQWORD_ADDRESS_DESCRIPTOR Descriptor;
    ULONG Length;
    PPROTOCOL_CONTEXT ProtocolContext;
    BOOLEAN RangeFound;
    PVOID Resources;
    NTSTATUS Status;
    UCHAR Tag;

    ProtocolContext = NULL;

    //
    // Allocate a buffer for the config access entry.
    //

    ConfigEntry = BlMmAllocateHeap(sizeof(PCI_CONFIG_ENTRY));
    if (ConfigEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto CreateConfigEntryEnd;
    }

    RtlZeroMemory(ConfigEntry, sizeof(PCI_CONFIG_ENTRY));

    //
    // Fill in the routine used to access config space through this interface,
    // and include the interface pointer and handle as context.
    //

    ConfigEntry->AccessRoutine = PltpAccessConfig;
    ProtocolContext = BlMmAllocateHeap(sizeof(PROTOCOL_CONTEXT));
    if (ProtocolContext == NULL) {
        Status = STATUS_NO_MEMORY;
        goto CreateConfigEntryEnd;
    }

    ProtocolContext->Handle = Handle;
    ProtocolContext->Interface = Interface;
    ConfigEntry->AccessContext = (ULONG64)(ULONG_PTR)ProtocolContext;

    //
    // Get the set of bus numbers supported by this root bus protocol by
    // querying the resource configuration from the interface.  The segment
    // number is available from the interface directly.
    //

    ConfigEntry->SegmentNumber = Interface->SegmentNumber;
    Status = PltpGetRootBusResources(Interface, &Resources);
    if (!NT_SUCCESS(Status)) {
        goto CreateConfigEntryEnd;
    }

    //
    // The resources are a packed array of ACPI resource descriptors.  The only
    // allowable descriptors are QWORD address descriptors or an end descriptor,
    // which terminates the list.  The list is checked for validity when it is
    // retrieved, so validity checking is not required here.
    //

    RtlInitializeBitMap(&ConfigEntry->BusNumberMap,
                        ConfigEntry->BusNumberMapBuffer,
                        0x100);

    Tag = *(PUCHAR)Resources;
    if ((Tag & LARGE_RESOURCE_TAG) == 0) {
        Tag &= SMALL_TAG_MASK;
    }

    Length = 0;
    RangeFound = FALSE;
    while (Tag != TAG_END) {
        Descriptor = (PQWORD_ADDRESS_DESCRIPTOR)Add2Ptr(Resources, Length);
        if ((Descriptor->RFlag == ADDRESS_BUS_NUMBER_TYPE) &&
            (Descriptor->MinimumAddress <= 0xFF) &&
            (Descriptor->MaximumAddress <= 0xFF) &&
            ((Descriptor->MinimumAddress + Descriptor->AddressLength - 1) ==
             Descriptor->MaximumAddress)) {

            RtlSetBits(&ConfigEntry->BusNumberMap,
                       (ULONG)Descriptor->MinimumAddress,
                       (USHORT)Descriptor->AddressLength);

            RangeFound = TRUE;
        }

        //
        // Make sure to handle QWORD_ADDRESS_DESCRIPTORS whose Length field is
        // correct per the spec - which excludes the Tag and Length fields, as
        // well as lengths that this code previously assumed were the actual
        // QWORD_ADDRESS_DESCRIPTOR size.  Which is not per the UEFI spec but
        // was what this code expected from Feb 2006 - Jan 2015.  Now either
        // length should work fine.
        //

        Length += Descriptor->Length;
        if (Descriptor->Length == (sizeof(QWORD_ADDRESS_DESCRIPTOR) -
            FIELD_OFFSET(QWORD_ADDRESS_DESCRIPTOR, RFlag))) {

            Length += FIELD_OFFSET(QWORD_ADDRESS_DESCRIPTOR, RFlag);
        }

        Tag = *(PUCHAR)Add2Ptr(Resources, Length);
        if ((Tag & LARGE_RESOURCE_TAG) == 0) {
            Tag &= SMALL_TAG_MASK;
        }
    }

    if (RangeFound != FALSE) {
        Status = STATUS_SUCCESS;

    } else {
        Status = STATUS_INVALID_MEMBER;
    }

CreateConfigEntryEnd:
    if (NT_SUCCESS(Status)) {
        *ConfigEntryOut = ConfigEntry;

    } else {
        BlMmFreeHeap(ConfigEntry);
    }

    return Status;
}

VOID
PltpDestroyConfigTableEntry (
    __in PPCI_CONFIG_ENTRY Entry
    )

/*++

Routine Description:

    This routine destroys a config access table entry on systems using the
    EFI PCI Root Bus Protocol config access method.

Arguments:

    Entry - Supplies the entry to destroy.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PPROTOCOL_CONTEXT ProtocolContext;

    //
    // Close the protocol used to access config space through this entry.
    //

    ProtocolContext = (PPROTOCOL_CONTEXT)(ULONG_PTR)Entry->AccessContext;
    EfiCloseProtocol(ProtocolContext->Handle,
                     &EfiPciRootBusProtocol,
                     ProtocolContext->Interface);

    //
    // Free the protocol context and the entry.
    //

    BlMmFreeHeap(ProtocolContext);
    BlMmFreeHeap(Entry);
    return;
}

NTSTATUS
PltpDestroyTableMapRoutine (
    __in PPCI_CONFIG_ENTRY Entry,
    __in ULONG Index
    )

/*++

Routine Description:

    This routine destroys a config access table entry on systems using the
    EFI PCI Root Bus Protocol config access method.

Arguments:

    Entry - Supplies the entry to destroy.

    Index - Supplies the index in the config access table of the given entry.

Return Value:

    STATUS_SUCCESS.

--*/

{

    PltpDestroyConfigTableEntry(Entry);
    PltPciConfigTable[Index] = NULL;
    return STATUS_SUCCESS;
}

NTSTATUS
PltpGetRootBusResources (
    __in EFI_PCI_ROOT_BRIDGE_IO_INTERFACE *Interface,
    __out PVOID *ResourcesOut
    )

/*++

Routine Description:

    This routine retrieves the PCI root bus resources from the configuration
    exposed by the EFI PCI Root Bridge Protocol.

Arguments:

    Interface - Supplies an interface to the EFI PCI Root Bridge Protocol to
        query for resources.

    ResourcesOut - Supplies a pointer to a variable that receives a pointer to
        the root bus resource configuration.  This configuration buffer is BIOS
        internal memory and must be treated as read-only and not freed by the
        caller.

Return Value:

    STATUS_SUCCESS if the resource configuration was retrieved properly.

    STATUS_INVALID_LABEL if the interface returned a buffer containing invalid
        data.

    Another error status code if returned from the EFI interface call.

--*/

{

    EXECUTION_CONTEXT_TYPE ContextType;
    PQWORD_ADDRESS_DESCRIPTOR Descriptor;
    ULONG Length;
    PVOID Resources;
    PHYSICAL_ADDRESS ResourcesPa;
    NTSTATUS Status;
    UCHAR Tag;

    Resources = NULL;
    ResourcesPa.QuadPart = 0;
    Length = 0;

    Status = EfiPciGetConfiguration(Interface, &ResourcesPa);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // In order to determine the length of the resource array, switch into
    // physical mode to access the returned physical address directly.  This
    // length will be the number of bytes that are to be mapped in order to
    // allow the buffer to be accessed with translation enabled.
    //

    //
    // If the application is running within the application execution context,
    // switch it to firmware execution context for making firmware the call.
    //

    ContextType = BlpArchQueryCurrentContextType();
    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ExecutionContextFirmware);
    }

    Resources = (PVOID)(UINTN)ResourcesPa.QuadPart;

    //
    // The resources are a packed array of ACPI resource descriptors.  The only
    // allowable descriptors are QWORD address descriptors or an end descriptor,
    // which terminates the list.  Run the list whether or not execution began
    // in virtual mode as a validity check of the buffer returned from the
    // firmware.
    //

    Tag = *(PUCHAR)Resources;
    if ((Tag & LARGE_RESOURCE_TAG) == 0) {
        Tag &= SMALL_TAG_MASK;
    }

    while (Tag != TAG_END) {
        if (Tag != TAG_QWORD_ADDRESS) {
            Status = STATUS_INVALID_LABEL;
            goto GetRootBusResourcesExit;
        }

        Descriptor = (PQWORD_ADDRESS_DESCRIPTOR)Add2Ptr(Resources, Length);
        if (Descriptor->Length == 0) {
            Status = STATUS_INVALID_LABEL;
            goto GetRootBusResourcesExit;
        }

        //
        // Make sure to handle QWORD_ADDRESS_DESCRIPTORS whose Length field is
        // correct per the spec - which excludes the Tag and Length fields, as
        // well as lengths that this code previously assumed were the actual
        // QWORD_ADDRESS_DESCRIPTOR size.  Which is not per the UEFI spec but
        // was what this code expected from Feb 2006 - Jan 2015.  Now either
        // length should work fine.
        //

        Length += Descriptor->Length;
        if (Descriptor->Length == (sizeof(QWORD_ADDRESS_DESCRIPTOR) -
            FIELD_OFFSET(QWORD_ADDRESS_DESCRIPTOR, RFlag))) {

            Length += FIELD_OFFSET(QWORD_ADDRESS_DESCRIPTOR, RFlag);
        }

        Tag = *(PUCHAR)Add2Ptr(Resources, Length);
        if ((Tag & LARGE_RESOURCE_TAG) == 0) {
            Tag &= SMALL_TAG_MASK;
        }
    }

    Length += 1;
    if (Length != 1) {
        Status = STATUS_SUCCESS;

    } else {
        Status = STATUS_INVALID_LABEL;
    }

GetRootBusResourcesExit:

    //
    // Restore original execution context.
    //

    if (ContextType != ExecutionContextFirmware) {
        BlpArchSwitchContext(ContextType);

        //
        // Now that translation has been re-enabled, map the resource array
        // so that it can be accessed in virtual mode.
        //

        if (NT_SUCCESS(Status) && (Length != 0)) {
            Status = BlMmMapPhysicalAddress(&Resources,
                                            ResourcesPa,
                                            Length,
                                            MEMORY_ATTRIBUTE_NONE);
        }
    }

    if (NT_SUCCESS(Status)) {
        *ResourcesOut = Resources;
    }

    return Status;
}

