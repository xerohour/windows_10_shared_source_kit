/*++

Copyright (c) Microsoft Corporation

Module Name:

    config.c

Abstract:

    This module will build the loader's configuration tree for an EFI
    boot application.

Environment:

    Boot Environment.

Revision History:

--*/

// ------------------------------------------------------------------- Includes

#include "osloader.h"
#include <ntacpi.h>

// ---------------------------------------------------------------- Definitions

#define MACHINE_IDENTIFIER_X86  "AT/AT COMPATIBLE"
#define MACHINE_IDENTIFIER_ARM  "ARM processor family"
#define MACHINE_IDENTIFIER_UNKNOWN "Unknown Machine Type"

// ----------------------------------------------------------------- Prototypes

NTSTATUS
OslpInitializeControllerNode (
    __in CONFIGURATION_CLASS Class,
    __in CONFIGURATION_TYPE Type,
    __in ULONG AffinityMask,
    __in_bcount_opt(IdentifierLength) PCHAR IdentifierString,
    __in size_t IdentifierLength,
    __deref_out PCONFIGURATION_COMPONENT_DATA *ControllerNode
    );

NTSTATUS
OslpCleanupControllerNode (
    __inout PCONFIGURATION_COMPONENT_DATA ControllerNode
    );

VOID
OslpCollectInt13DriveParameters (
    __in ULONG DiskCount,
    __out_ecount(DiskCount) PCM_INT13_DRIVE_PARAMETER ParameterBuffer,
    __in_ecount(DiskCount) PDEVICE_ID DiskIdList
    );

NTSTATUS
OslpBuildDiskControllerTree (
    __deref_out PCONFIGURATION_COMPONENT_DATA *DiskRootEntry,
    __in PGENERIC_BUFFER DiskIdBuffer
    );

NTSTATUS
OslpBuildAcpiBiosNode (
    __deref_out PCONFIGURATION_COMPONENT_DATA *AcpiEntry
    );

NTSTATUS
OslpBuildKeyboardControllerNode (
    __deref_out PCONFIGURATION_COMPONENT_DATA *KeyboardEntry
    );

NTSTATUS
OslpCreateFloppyPeripheralEntry (
    __in DEVICE_ID DeviceId,
    __deref_out PCONFIGURATION_COMPONENT_DATA *FloppyPeripheralEntry
    );

NTSTATUS
OslpCreateDiskPeripheralEntry (
    __in DEVICE_ID DeviceId,
    __in ULONG EnumNumber,
    __deref_out PCONFIGURATION_COMPONENT_DATA *DiskPeripheralEntry
    );

NTSTATUS
OslpBuildSystemNode (
    __deref_out PCONFIGURATION_COMPONENT_DATA *SystemEntry,
    __in PGENERIC_BUFFER DiskIdBuffer
    );

// ------------------------------------------------------------------ Functions

NTSTATUS
OslLegacyHardwareDetection (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine will build a configuration tree for the loader block.

    It is the goal of this routine to build a minimal configuration tree.
    The structure of that tree is as follows:

        - System Class

            - ISA Adapter

                - Disk Information

            - ACPI (RSDT Address).

Arguments:

    LoaderBlock - A pointer to the argument structure that will eventually
        be passed into the kernel.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if an allocation fails.
    STATUS_INVALID_PARAMETER if the firmware configuration tree is malformed.

--*/

{

    PCONFIGURATION_COMPONENT_DATA ConfigurationRoot;
    PCONFIGURATION_COMPONENT_DATA CurrentEntry;
    GENERIC_BUFFER DiskIdBuffer;
    BOOT_ENVIRONMENT_DEVICE EnumerateDeviceId;
    PCONFIGURATION_COMPONENT_DATA ParentEntry;
    PCONFIGURATION_COMPONENT_DATA SiblingEntry;
    NTSTATUS Status;

    ConfigurationRoot = NULL;

    //
    // Enumerate the hard disks present in the machine.  The list of hard
    // disks is used at multiple points during configuration tree construction
    // and thus must be computed before configuration tree construction
    // begins.
    //

    RtlZeroMemory(&EnumerateDeviceId, sizeof(BOOT_ENVIRONMENT_DEVICE));
    EnumerateDeviceId.DeviceType = DEVICE_BLOCK_IO;
    EnumerateDeviceId.Size = sizeof(BOOT_ENVIRONMENT_DEVICE);
    EnumerateDeviceId.u.BlockIo.BlockIoType = BLOCK_IO_HARD_DISK;
    DiskIdBuffer.Buffer = NULL;
    DiskIdBuffer.BufferSize = 0;
    Status = BlDeviceEnumerateDeviceClass(&EnumerateDeviceId,
                                          &DiskIdBuffer,
                                          TRUE);

    if (!NT_SUCCESS(Status)) {
        goto OslLegacyHardwareDetectionEnd;
    }

    //
    // The root of the configuration tree is a system class node.
    //

    Status = OslpBuildSystemNode(&ConfigurationRoot, &DiskIdBuffer);
    if (!NT_SUCCESS(Status)) {
        goto OslLegacyHardwareDetectionEnd;
    }

    ParentEntry = ConfigurationRoot;

    //
    // Create an ISA adapter node.
    //

    Status = OslpInitializeControllerNode(
                                AdapterClass,
                                MultiFunctionAdapter,
                                0xffffffff,
                                "ISA",
                                sizeof("ISA"),
                                &CurrentEntry);

    if (!NT_SUCCESS(Status)) {
        goto OslLegacyHardwareDetectionEnd;
    }

    //
    // This node should be inserted into the tree as a child of the
    // root.
    //

    CurrentEntry->Parent = ParentEntry;
    ParentEntry->Child   = CurrentEntry;
    ParentEntry = CurrentEntry;  // ParentEntry = ISA Adapter.

    //
    // Collect Disk Geometry information.
    //

    Status = OslpBuildDiskControllerTree(&CurrentEntry, &DiskIdBuffer);
    if (!NT_SUCCESS(Status)) {
        goto OslLegacyHardwareDetectionEnd;
    }

    //
    // Insert the disk information as a child to the ISA Adapter.
    //

    CurrentEntry->Parent = ParentEntry;
    ParentEntry->Child = CurrentEntry;

    //
    // Collect keyboard controller information.
    //

    SiblingEntry = CurrentEntry;
    Status = OslpBuildKeyboardControllerNode(&CurrentEntry);
    if (!NT_SUCCESS(Status)) {
        goto OslLegacyHardwareDetectionEnd;
    }

    //
    // Insert keyboard node as a child to the ISA Adapter and as a
    // sibling of the disk node.
    //

    CurrentEntry->Parent = SiblingEntry->Parent;
    SiblingEntry->Sibling = CurrentEntry;

    //
    // Create ACPI Bios node
    //

    Status = OslpBuildAcpiBiosNode(&CurrentEntry);
    if (NT_SUCCESS(Status)) {

        //
        // Adjust pointers to be relative to the System Class Node to insert
        // the ACPI Bios node.
        //

        SiblingEntry = ParentEntry;
        ParentEntry = ParentEntry->Parent;

        //
        // Insert ACPI node as a child of the system class and a sibling to
        // the ISA Adapter Node.
        //

        CurrentEntry->Parent = ParentEntry;
        SiblingEntry->Sibling = CurrentEntry;

    } else {
        goto OslLegacyHardwareDetectionEnd;
    }

    //
    // We were successful in creating a configuration tree.  Pass this to
    // the kernel via the loader block.
    //

    LoaderBlock->ConfigurationRoot = ConfigurationRoot;

OslLegacyHardwareDetectionEnd:
    if (DiskIdBuffer.BufferSize != 0) {
        BlMmFreeHeap(DiskIdBuffer.Buffer);
    }

    if (!NT_SUCCESS(Status)) {

        //
        // In the event of an error, the configuration tree should be
        // freed.
        //

        OslpCleanupControllerNode(ConfigurationRoot);
    }

    return Status;
}

NTSTATUS
OslpBuildSystemNode (
    __deref_out PCONFIGURATION_COMPONENT_DATA *SystemEntry,
    __in PGENERIC_BUFFER DiskIdBuffer
    )

/*++

Routine Description:

    This routine will build configuration tree entries for the root
    system node.  It handles node initialization and is also responsible
    for populating the relevant ConfigurationData for the system node.

Arguments:

    SystemEntry - On output, contains a pointer to the root of the
        configuration tree.

    DiskIdBuffer - Supplies a generic buffer listing the device IDs for all
        hard disks enumerated by the boot environment.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if an allocation fails.
    STATUS_INVALID_PARAMETER if the firmware configuration tree is malformed.

--*/

{

    PCONFIGURATION_COMPONENT Component;
    ULONG ConfigurationDataLength;
    PCONFIGURATION_COMPONENT_DATA ConfigurationRoot;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR Descriptor;
    ULONG DiskCount;
    ULONG DiskInformationLength;
    PCHAR MachineType;
    PCM_PARTIAL_RESOURCE_LIST ResourceList;
    NTSTATUS Status;
    PVOID Target;
    PCM_ROM_BLOCK VideoRomBlock;
    ULONG VideoRomBlockCount;
    BOOLEAN VideoRomBlockPresent;

    VideoRomBlock = NULL;

    //
    // Root of configuration tree is a system class node.
    //

    ConfigurationRoot = NULL;
#pragma prefast(suppress: 6285, "logical-or of constants is by design")
    if ((BOOT_APPLICATION_MACHINE_TYPE == IMAGE_FILE_MACHINE_I386) ||
        (BOOT_APPLICATION_MACHINE_TYPE == IMAGE_FILE_MACHINE_AMD64)) {

        MachineType = MACHINE_IDENTIFIER_X86;

#pragma prefast(suppress: 6285, "logical-or of constants is by design")
    } else if ((BOOT_APPLICATION_MACHINE_TYPE == IMAGE_FILE_MACHINE_ARMNT) ||
               (BOOT_APPLICATION_MACHINE_TYPE == IMAGE_FILE_MACHINE_ARM64)) {
        MachineType = MACHINE_IDENTIFIER_ARM;

    } else {

        ASSERT(FALSE);

        MachineType = MACHINE_IDENTIFIER_UNKNOWN;
    }

    Status = OslpInitializeControllerNode(
                        SystemClass,
                        MaximumType,
                        0,
                        MachineType,
                        strlen(MachineType)+1,
                        &ConfigurationRoot);

    if (!NT_SUCCESS(Status)) {
        goto BuildSystemNodeEnd;
    }

    Component = &ConfigurationRoot->ComponentEntry;

    //
    // The ConfigurationData for the root entry is a :
    //
    //         CM_PARTIAL_RESOURCE_LIST header
    //         CM_PARTIAL_RESOURCE_DESCRIPTOR for disk parameter table.
    //         CM_PARTIAL_RESOURCE_DESCRIPTOR for Video Rom blocks.
    //
    // The CM_PARTIAL_RESOURCE_LIST header contains a single partial resource
    // descriptor structure, so extra space is needed for only one of the two
    // resource descriptors.
    //

    //
    // Code in disk.sys consumes disk parameters encoded into the
    // configuration data element attached to the system node.  In addition,
    // the firmware specific implementation must be queried to find out if
    // video rom blocks are present, as these are needed to allow VGA and 16
    // bit VDM bios emulation to function properly.  After this information
    // has been collected, the appropriately sized packed structure can be
    // constructed and linked to the system node.
    //

    ConfigurationDataLength = sizeof(CM_PARTIAL_RESOURCE_LIST) +
                              sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR);

    DiskCount = DiskIdBuffer->DataSize / sizeof(DEVICE_ID);
    DiskInformationLength = DiskCount * sizeof(CM_INT13_DRIVE_PARAMETER);
    ConfigurationDataLength += DiskInformationLength;
    VideoRomBlockPresent = FALSE;
    Status = OslFwGetVideoBiosData(&VideoRomBlock, &VideoRomBlockCount);
    if (NT_SUCCESS(Status)) {

        //
        // Video BIOS data is present on this system.  Roll the size of the
        // video BIOS rom block information into the total size of the system
        // node configuration data element.
        //

        ConfigurationDataLength += sizeof(CM_ROM_BLOCK) * VideoRomBlockCount;
        VideoRomBlockPresent = TRUE;
    }

    //
    // Allocate space for the descriptor and fill it in.
    //

    ResourceList = BlMmAllocateHeap(ConfigurationDataLength);
    if (ResourceList == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BuildSystemNodeEnd;
    }

    Component->ConfigurationDataLength = ConfigurationDataLength;
    ConfigurationRoot->ConfigurationData = ResourceList;
    RtlZeroMemory(ResourceList, ConfigurationDataLength);
    ResourceList->Count = 2;
    Descriptor = &ResourceList->PartialDescriptors[0];
    Descriptor->Type = CmResourceTypeDeviceSpecific;
    Descriptor->u.DeviceSpecificData.DataSize = DiskInformationLength;

    //
    // Copy the disk information, which may be zero bytes in length, into the
    // resource list immediately following the initial descriptor.
    //

    Target = (PVOID)(Descriptor + 1);
    if (DiskInformationLength != 0) {
        OslpCollectInt13DriveParameters(DiskCount,
                                        Target,
                                        DiskIdBuffer->Buffer);
    }

    //
    // If video ROM block information has been retrieved from the BIOS, make
    // sure to copy it in after the second resource descriptor.
    //
    // N.B. The location of the second resource descriptor is variable
    //      depending upon the amount of preceding disk information.  All
    //      consumers of this data in the kernel account for this.
    //

    Descriptor = Add2Ptr(Target, DiskInformationLength);
    Descriptor->Type = CmResourceTypeDeviceSpecific;
    Descriptor->u.DeviceSpecificData.DataSize = 0;
    if ((VideoRomBlockPresent != FALSE) && (VideoRomBlockCount > 0)) {
        Descriptor->u.DeviceSpecificData.DataSize = VideoRomBlockCount *
                                                    sizeof(CM_ROM_BLOCK);

        Target = (PVOID)(Descriptor + 1);
        RtlCopyMemory(Target,
                      VideoRomBlock,
                      VideoRomBlockCount * sizeof(CM_ROM_BLOCK));
    }

    *SystemEntry = ConfigurationRoot;

BuildSystemNodeEnd:
    if (VideoRomBlock != NULL) {
        BlMmFreeHeap(VideoRomBlock);
    }

    if (!NT_SUCCESS(Status)) {
        OslpCleanupControllerNode(ConfigurationRoot);
    }

    return Status;
}

NTSTATUS
OslpBuildDiskControllerTree (
    __deref_out PCONFIGURATION_COMPONENT_DATA *DiskRootEntry,
    __in PGENERIC_BUFFER DiskIdBuffer
    )

/*++

Routine Description:

    This routine will build configuration tree entries for all block i/o
    devices.

Arguments:

    DiskRootEntry - On output, contains a pointer to the root of the
        disk controller configuration tree.

    DiskIdBuffer - Supplies a generic buffer listing the device IDs for all
        hard disks enumerated by the boot environment.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if an allocation fails.
    STATUS_INVALID_PARAMETER if the firmware configuration tree is malformed.

--*/

{

    PCONFIGURATION_COMPONENT Component;
    PCONFIGURATION_COMPONENT_DATA CurrentEntry;
    GENERIC_BUFFER DeviceIdBuffer;
    BOOT_ENVIRONMENT_DEVICE EnumerateDeviceId;
    ULONG Index;
    ULONG NumDevices;
    PCONFIGURATION_COMPONENT_DATA RootControllerEntry;
    PCONFIGURATION_COMPONENT_DATA SiblingEntry;
    NTSTATUS Status;

    DeviceIdBuffer.Buffer = NULL;
    DeviceIdBuffer.BufferSize = 0;
    SiblingEntry = NULL;

    //
    // Allocate and initialize the root of the disk controller configuration
    // tree.
    //
    // The tree is one level deep, containing a list of all floppy devices
    // followed immediately by a list of all HDDs.
    //

    Status = OslpInitializeControllerNode(
                    ControllerClass,
                    DiskController,
                    0xffffffff,
                    NULL,
                    0,
                    &RootControllerEntry);

    if (!NT_SUCCESS(Status)) {
        goto BuildDiskControllerTreeEnd;
    }

    Component = &RootControllerEntry->ComponentEntry;

    Status = OslFwGetFloppyControllerData(
        (PCM_PARTIAL_RESOURCE_LIST *) &RootControllerEntry->ConfigurationData,
        &Component->ConfigurationDataLength);

    if (!NT_SUCCESS(Status)) {
        goto BuildDiskControllerFloppyEnd;
    }

    //
    // Enumerate all floppy devices, creating a configuration tree component
    // for each.
    //

    RtlZeroMemory(&EnumerateDeviceId, sizeof(BOOT_ENVIRONMENT_DEVICE));
    EnumerateDeviceId.DeviceType = DEVICE_BLOCK_IO;
    EnumerateDeviceId.Size = sizeof(BOOT_ENVIRONMENT_DEVICE);
    EnumerateDeviceId.u.BlockIo.BlockIoType = BLOCK_IO_REMOVABLE_DISK;
    Status = BlDeviceEnumerateDeviceClass(&EnumerateDeviceId,
                                          &DeviceIdBuffer,
                                          TRUE);

    if (!NT_SUCCESS(Status)) {
        goto BuildDiskControllerFloppyEnd;
    }

    //
    // Try to create a peripheral floppy class entry for each removable
    // disk.
    //

    NumDevices = DeviceIdBuffer.DataSize / sizeof(DEVICE_ID);
    for (Index = 0; Index < NumDevices; Index += 1) {
        Status = OslpCreateFloppyPeripheralEntry(
                             ((PDEVICE_ID)DeviceIdBuffer.Buffer)[Index],
                             &CurrentEntry);

        if (!NT_SUCCESS(Status)) {
            continue;
        }

        //
        // Insert the floppy entry into the configuration tree.  All
        // peripheral device class nodes are inserted as children to the
        // disk controller.
        //

        if (SiblingEntry != NULL) {
            SiblingEntry->Sibling = CurrentEntry;

        } else {
            RootControllerEntry->Child = CurrentEntry;
        }

        CurrentEntry->Parent = RootControllerEntry;
        SiblingEntry = CurrentEntry;
    }

    //
    // Iterate through the list of hard drives supplied by the caller,
    // creating a configuration tree component for each.
    //

BuildDiskControllerFloppyEnd:
    NumDevices = DiskIdBuffer->DataSize / sizeof(DEVICE_ID);
    for (Index = 0; Index < NumDevices; Index += 1) {
        Status = OslpCreateDiskPeripheralEntry(
                       ((PDEVICE_ID)DiskIdBuffer->Buffer)[Index],
                       Index,
                       &CurrentEntry);

        if (!NT_SUCCESS(Status)) {
            continue;
        }

        //
        // Insert the disk entry into the configuration tree.  All peripheral
        // device class nodes are inserted as children to the disk controller.
        //

        if (SiblingEntry != NULL) {
            SiblingEntry->Sibling = CurrentEntry;

        } else {
            RootControllerEntry->Child = CurrentEntry;
        }

        CurrentEntry->Parent = RootControllerEntry;
        SiblingEntry = CurrentEntry;
    }

    Status = STATUS_SUCCESS;
    *DiskRootEntry = RootControllerEntry;

BuildDiskControllerTreeEnd:

    //
    // Free memory allocated by this routine.
    //

    if (DeviceIdBuffer.BufferSize != 0) {
        BlMmFreeHeap(DeviceIdBuffer.Buffer);
    }

    if (!NT_SUCCESS(Status)) {

        //
        // In the event of an error, the configuration tree should be freed.
        //

        OslpCleanupControllerNode(RootControllerEntry);
    }

    return Status;
}

NTSTATUS
OslpBuildAcpiBiosNode (
    __deref_out PCONFIGURATION_COMPONENT_DATA *AcpiClassNode
    )

/*++

Routine Description:

    This routine will build configuration a configuration tree entry
    for an ACPI bios.  This entry includes a pointer to the root ACPI table
    and a description of ACPI memory.

Arguments:

    AcpiClassNode - On output, contains a pointer to a configuration tree
        entry for the ACPI bios.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if an allocation fails.

--*/

{

    PCONFIGURATION_COMPONENT_DATA AcpiEntry;
    PACPI_BIOS_MULTI_NODE AcpiMultiNode;
    PCONFIGURATION_COMPONENT Component;
    PACPI_E820_ENTRY MemoryMapArray;
    ULONG MemoryMapArraySize;
    ULONG MemoryMapCount;
    PCM_PARTIAL_RESOURCE_LIST ResourceList;
    PHYSICAL_ADDRESS RsdtPa;
    NTSTATUS Status;

    MemoryMapArray = NULL;
    ResourceList = NULL;
    Status = OslpInitializeControllerNode(
                                AdapterClass,
                                MultiFunctionAdapter,
                                0xffffffff,
                                "ACPI BIOS",
                                sizeof("ACPI BIOS"),
                                &AcpiEntry);

    if (!NT_SUCCESS(Status)) {
        goto BuildAcpiBiosNodeEnd;
    }

    //
    // An ACPI Class Node contains an ACPI_BIOS_MULTI_NODE resource.  This
    // structure describes the location of the RSDT as well as the E820
    // table.  The E820 table is used on legacy bios systems to get the
    // firmware memory map.  The ACPI driver uses this to make sure
    // the PCI driver does not attempt to assign a non-reserved address
    // range for a PCI devices config or memory space.
    //
    // The loader already passes a memory map to the kernel.  If any specific
    // information is required, it should be derived from that firmware
    // independent memory map.
    //
    // TODO: The ACPI component does not properly use the OS memory map.  Until
    //       such time, the OS loader will pass the duplicated memory map via
    //       the legacy interface.
    //

    Status = BlFwGetAcpiMemoryMap(&MemoryMapArray, &MemoryMapCount);
    if (!NT_SUCCESS(Status) || (MemoryMapCount == 0)) {
        goto BuildAcpiBiosNodeEnd;
    }

    MemoryMapArraySize = sizeof(ACPI_E820_ENTRY) * MemoryMapCount;
    Component = &AcpiEntry->ComponentEntry;
    Component->ConfigurationDataLength =
        (sizeof(CM_PARTIAL_RESOURCE_LIST) +
         FIELD_OFFSET(ACPI_BIOS_MULTI_NODE, E820Entry) +
         MemoryMapArraySize);

    ResourceList = BlMmAllocateHeap(Component->ConfigurationDataLength);
    if (ResourceList == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BuildAcpiBiosNodeEnd;
    }

    AcpiEntry->ConfigurationData = ResourceList;
    RtlZeroMemory(ResourceList, Component->ConfigurationDataLength);
    ResourceList->Count = 1;
    ResourceList->PartialDescriptors[0].Type = CmResourceTypeDeviceSpecific;
    ResourceList->PartialDescriptors[0].u.DeviceSpecificData.DataSize =
        FIELD_OFFSET(ACPI_BIOS_MULTI_NODE, E820Entry) + MemoryMapArraySize;

    AcpiMultiNode = Add2Ptr(ResourceList,sizeof(CM_PARTIAL_RESOURCE_LIST));

    //
    // Determine the address of the RSDT (Root System Description Table).
    // The firmware may not have populated the necessary tables on ARM platform.
    // Hence do not treat a failure as fatal. The tables are expected to be
    // supplied via an override mechanism.
    //

    Status = BlFwGetSystemTable(FW_TABLE_RSDT, &RsdtPa);
    if (!NT_SUCCESS(Status)) {
        RsdtPa.QuadPart = RSDT_ADDRESS_NOT_PRESENT;
        Status = STATUS_SUCCESS;
    }

    AcpiMultiNode->RsdtAddress.QuadPart = RsdtPa.QuadPart;

    //
    // Copy the memory map into the descriptor.
    //

    AcpiMultiNode->Count = MemoryMapCount;
    RtlCopyMemory(&AcpiMultiNode->E820Entry[0],
                  MemoryMapArray,
                  MemoryMapArraySize);

    *AcpiClassNode = AcpiEntry;

BuildAcpiBiosNodeEnd:

    //
    // In the error case, free any memory allocated by this routine.
    //
    // N.B. The resource list is freed as part of cleaning up the ACPI node.
    //

    if (!NT_SUCCESS(Status)) {
        OslpCleanupControllerNode(AcpiEntry);
    }

    if (MemoryMapArray != NULL) {
        BlMmFreeHeap(MemoryMapArray);
    }

    return Status;
}

NTSTATUS
OslpBuildKeyboardControllerNode (
    __deref_out PCONFIGURATION_COMPONENT_DATA *KeyboardControllerNode
    )

/*++

Routine Description:

    This routine will build a configuration tree entry for a keyboard
    controller and peripheral. This entry includes the keyboard LED state.

    All other information about the controller is derived via normal PNP
    mechanisms.

Arguments:

    KeyboardControllerNode - On output, contains a pointer to a configuration tree
        entry for the keyboard controller.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if an allocation fails.

--*/

{

    PCONFIGURATION_COMPONENT Component = NULL;
    PCONFIGURATION_COMPONENT_DATA KeyboardControllerEntry;
    PCONFIGURATION_COMPONENT_DATA KeyboardPeripheralEntry;
    PCM_KEYBOARD_DEVICE_DATA KeyboardData;
    PCM_PARTIAL_RESOURCE_LIST ResourceList;
    NTSTATUS Status;

    KeyboardControllerEntry = NULL;
    KeyboardPeripheralEntry = NULL;
    ResourceList = NULL;

    //
    // We must create a keyboard controller entry with a
    // keyboard peripheral entry as it's child.  First create the
    // parent controller entry, which is unnamed.
    //

    Status = OslpInitializeControllerNode(
                                ControllerClass,
                                KeyboardController,
                                0xffffffff,
                                NULL,
                                0,
                                &KeyboardControllerEntry);

    if (!NT_SUCCESS(Status)) {
        goto BuildKeyboardControllerNodeEnd;
    }

    Component = &KeyboardControllerEntry->ComponentEntry;
    Component->Flags.ConsoleIn = 1;
    Component->Flags.Input = 1;

    //
    // Now create the peripheral entry.
    //

    Status = OslpInitializeControllerNode(
                                PeripheralClass,
                                KeyboardPeripheral,
                                0xffffffff,
                                "UNKNOWN_KEYBOARD",
                                sizeof("UNKNOWN_KEYBOARD"),
                                &KeyboardPeripheralEntry);

    if (!NT_SUCCESS(Status)) {
        goto BuildKeyboardControllerNodeEnd;
    }

    //
    // Link the two entries together.
    //

    KeyboardPeripheralEntry->Parent = KeyboardControllerEntry;
    KeyboardControllerEntry->Child = KeyboardPeripheralEntry;

    //
    // Allocate and link in the space for the keyboard LED state for the
    // peripheral.
    //

    Component = &KeyboardPeripheralEntry->ComponentEntry;
    Component->Flags.ConsoleIn = 1;
    Component->Flags.Input = 1;
    Component->ConfigurationDataLength = sizeof(CM_PARTIAL_RESOURCE_LIST) +
                                         sizeof(CM_KEYBOARD_DEVICE_DATA);

    ResourceList = BlMmAllocateHeap(Component->ConfigurationDataLength);
    if (ResourceList == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BuildKeyboardControllerNodeEnd;
    }

    KeyboardPeripheralEntry->ConfigurationData = ResourceList;
    RtlZeroMemory(ResourceList, Component->ConfigurationDataLength);
    ResourceList->Count = 1;
    ResourceList->PartialDescriptors[0].Type = CmResourceTypeDeviceSpecific;
    ResourceList->PartialDescriptors[0].u.DeviceSpecificData.DataSize =
               sizeof(CM_KEYBOARD_DEVICE_DATA);

    KeyboardData = (PCM_KEYBOARD_DEVICE_DATA)Add2Ptr(
                                    ResourceList,
                                    sizeof(CM_PARTIAL_RESOURCE_LIST));
    KeyboardData->KeyboardFlags = OslFwGetKeyboardFlags();
    KeyboardData->Type = 255;
    KeyboardData->Subtype = 255;
    *KeyboardControllerNode = KeyboardControllerEntry;

BuildKeyboardControllerNodeEnd:

    //
    // In the error case, free any memory allocated by this routine.
    //

    if (!NT_SUCCESS(Status)) {
        OslpCleanupControllerNode(KeyboardControllerEntry);
    }

    return Status;
}

NTSTATUS
OslpCreateFloppyPeripheralEntry (
    __in DEVICE_ID DeviceId,
    __deref_out PCONFIGURATION_COMPONENT_DATA *FloppyPeripheralEntry
    )

/*++

Routine Description:

    This routine will build configuration tree entry for a floppy
    peripheral device.

Arguments:

    DeviceId - ID returned from BlDeviceEnumerateDeviceClass for a particular
        floppy device.

    FloppyPeripheralEntry - On output, contains a pointer to an allocated and
        initialized configuration tree node for the floppy device described
        by DeviceId.


Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY when an allocation fails.

--*/

{

    PCONFIGURATION_COMPONENT Component;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    PCM_FLOPPY_DEVICE_DATA FloppyDeviceData = NULL;
    ULONG FloppyDeviceDataSize;
    PCONFIGURATION_COMPONENT_DATA FloppyEntry;
    PCM_FLOPPY_DEVICE_DATA LocalFloppyData;
    PCM_PARTIAL_RESOURCE_LIST ResourceList;
    NTSTATUS Status;

    Status = BlGetDeviceIdentifier(DeviceId, &DeviceIdentifier);

    ASSERT(Status == STATUS_SUCCESS);
    ASSERT(DeviceIdentifier->DeviceType == DEVICE_BLOCK_IO);
    ASSERT(DeviceIdentifier->u.BlockIo.BlockIoType == BLOCK_IO_REMOVABLE_DISK);

    //
    // Initialize floppy entry.
    //

    Status = OslpInitializeControllerNode(
                            PeripheralClass,
                            FloppyDiskPeripheral,
                            0xffffffff,
                            "FLOPPY1",
                            sizeof("FLOPPY1"),
                            &FloppyEntry);

    if (!NT_SUCCESS(Status)) {
        goto CreateFloppyPeripheralEntryEnd;
    }

    Component = &FloppyEntry->ComponentEntry;
    Component->Key = DeviceIdentifier->u.BlockIo.u.RemovableDisk.DriveNumber;

    ASSERT(Component->Key < 10);

    Component->Identifier[6] = Component->Identifier[6] + (CHAR)Component->Key;

    //
    // Retrieve any firmware specific hardware configuration data about the
    // device.
    //
    Status = OslFwGetFloppyConfigurationData(
                    DeviceId,
                    &FloppyDeviceData,
                    &FloppyDeviceDataSize);

    if (!NT_SUCCESS(Status)) {
        goto CreateFloppyPeripheralEntryEnd;
    }

    if (FloppyDeviceDataSize) {
        Component->ConfigurationDataLength =
                    sizeof(CM_PARTIAL_RESOURCE_LIST) + FloppyDeviceDataSize;

        ResourceList = BlMmAllocateHeap( Component->ConfigurationDataLength );
        if (!ResourceList) {
            Status = STATUS_NO_MEMORY;
            goto CreateFloppyPeripheralEntryEnd;
        }

        FloppyEntry->ConfigurationData = ResourceList;
        RtlZeroMemory(ResourceList, Component->ConfigurationDataLength);
        ResourceList->Count = 1;
        ResourceList->PartialDescriptors[0].Type =
                                CmResourceTypeDeviceSpecific;
        ResourceList->PartialDescriptors[0].u.DeviceSpecificData.DataSize =
                                FloppyDeviceDataSize;

        LocalFloppyData = (PCM_FLOPPY_DEVICE_DATA)Add2Ptr(
                                        ResourceList,
                                        sizeof(CM_PARTIAL_RESOURCE_LIST));
        RtlCopyMemory(
                LocalFloppyData,
                FloppyDeviceData,
                FloppyDeviceDataSize);

    }

    *FloppyPeripheralEntry = FloppyEntry;

CreateFloppyPeripheralEntryEnd:

    //
    // In the error case, free any memory allocated by this routine.
    //

    if (!NT_SUCCESS(Status)) {
        OslpCleanupControllerNode(FloppyEntry);
    }

    return Status;
}

NTSTATUS
OslpCreateDiskPeripheralEntry (
    __in DEVICE_ID DeviceId,
    __in ULONG EnumNumber,
    __deref_out PCONFIGURATION_COMPONENT_DATA *DiskPeripheralEntry
    )

/*++

Routine Description:

    This routine will build configuration tree entry for a hard disk drive
    peripheral device.

Arguments:

    DeviceId - ID returned from BlDeviceEnumerateDeviceClass for a particular
        disk device.

    EnumNumber - A value indicating the order in which the firmware enumerated
        the disk device.

    DiskPeripheralEntry - On output, contains a pointer to an allocated and
        initialized configuration tree node for the HDD described by DeviceId
        and EnumNumber.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY when an allocation fails.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        a read operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs while
        performing a read operation.

--*/

{

    UCHAR Sector[512];
    CHAR AppendChar;
    LONG Checksum;
    PCONFIGURATION_COMPONENT Component;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    DEVICE_INFORMATION DeviceInformation;
    PCONFIGURATION_COMPONENT_DATA DiskEntry;
    PCM_DISK_GEOMETRY_DEVICE_DATA DiskGeometry;
    ULONG i;
    PCM_PARTIAL_RESOURCE_LIST ResourceList = NULL;
    ULONG Signature;
    NTSTATUS Status;

    Status = BlGetDeviceIdentifier(DeviceId, &DeviceIdentifier);

    ASSERT(Status == STATUS_SUCCESS);
    ASSERT(DeviceIdentifier->DeviceType == DEVICE_BLOCK_IO);
    ASSERT(DeviceIdentifier->u.BlockIo.BlockIoType == BLOCK_IO_HARD_DISK);

    RtlZeroMemory(&DeviceInformation, sizeof(DEVICE_INFORMATION));
    Status = BlDeviceGetInformation(DeviceId, &DeviceInformation);

    ASSERT(Status == STATUS_SUCCESS);
    ASSERT(DeviceInformation.DeviceType == DEVICE_BLOCK_IO);
    ASSERT(DeviceInformation.u.BlockIo.Type == BLOCK_IO_HARD_DISK);

    //
    // Initialize the disk entry.
    //

    Status = OslpInitializeControllerNode(
                        PeripheralClass,
                        DiskPeripheral,
                        0xffffffff,
                        NULL,
                        0,
                        &DiskEntry);

    if (!NT_SUCCESS(Status)) {
        goto CreateDiskPeripheralEntryEnd;
    }

    Component = &DiskEntry->ComponentEntry;
    Component->Flags.Input  = 1;
    Component->Flags.Output = 1;
    Component->Key   = EnumNumber;

    //
    // Create an identifier string for the disk device.  NTDETECT used to
    // do this as follows:
    //
    // "8-digit-checksum" + "-" + "8-digit-signature" + "-" +
    // "Valid MBR identifier"
    //
    // For example: 00ff6396-6549071f-A.
    //
    // No special format was defined for a GPT disk.  The identifier is
    // based on the legacy MBR.  Raw disk are appended with an 'X' (not an
    // 'A').
    //

    Component->IdentifierLength = 20;
    Component->Identifier = BlMmAllocateHeap(20);  // 8 + 1 + 8 + 1 + 1 + 1.

    if (Component->Identifier == NULL) {
        Status = STATUS_NO_MEMORY;
        goto CreateDiskPeripheralEntryEnd;
    }

    //
    // Read the MBR into memory.
    //

    Status = BlDeviceReadAtOffset(DeviceId,
                                  0,
                                  sizeof(Sector),
                                  Sector,
                                  NULL);

    if (!NT_SUCCESS(Status)) {
        goto CreateDiskPeripheralEntryEnd;
    }

    //
    // Compute the checksum.
    //

    for (Checksum = 0, i = 0; i < 128; i += 1) {
        Checksum += ((PULONG)Sector)[i];
    }

    Checksum = -Checksum;

    //
    // The unique signature in a MBR is at offset 0x1b8.
    //

    Signature = ((PULONG)Sector)[0x1B8 / 4];

    //
    // A MBR is deemed as valid (even if this is a GPT disk) if the last
    // two bytes are '0xaa55'.
    //

    if (((PUSHORT)Sector)[(0x200 / sizeof(USHORT)) - 1] == 0xAA55) {
        AppendChar = 'A';

    } else {
        AppendChar = 'X';
    }

    //
    // Initialize the identifier.
    //

    sprintf_s(Component->Identifier,
              Component->IdentifierLength,
              "%08x-%08x-%c",
              Checksum,
              Signature,
              AppendChar);

    //
    // Each disk entry will contain a partial resource descriptor list which
    // will include disk geometry information.
    //

    Component->ConfigurationDataLength = sizeof(CM_PARTIAL_RESOURCE_LIST) +
                                         sizeof(CM_DISK_GEOMETRY_DEVICE_DATA);

    ResourceList = BlMmAllocateHeap(Component->ConfigurationDataLength);
    if (ResourceList == NULL) {
        Status = STATUS_NO_MEMORY;
        goto CreateDiskPeripheralEntryEnd;
    }

    DiskEntry->ConfigurationData = ResourceList;
    RtlZeroMemory(ResourceList, Component->ConfigurationDataLength);
    ResourceList->Count = 1;
    ResourceList->PartialDescriptors[0].Type = CmResourceTypeDeviceSpecific;
    ResourceList->PartialDescriptors[0].u.DeviceSpecificData.DataSize =
        sizeof(CM_DISK_GEOMETRY_DEVICE_DATA);

    DiskGeometry = (PCM_DISK_GEOMETRY_DEVICE_DATA)Add2Ptr(
                                        ResourceList,
                                        sizeof(CM_PARTIAL_RESOURCE_LIST));

    //
    // Fill in the CM_DISK_GEOMETRY_DEVICE_DATA.
    //
    // N.B. Although all fields of this structure are populated, the CHS
    //      information is not valid on an EFI system.  This information
    //      is not necessary on an EFI system.  EFI systems use LBA
    //      exclusively, so only the block size is ever referenced.
    //

    DiskGeometry->BytesPerSector = DeviceInformation.u.BlockIo.BlockSize;
    DiskGeometry->NumberOfCylinders =
        DeviceInformation.u.BlockIo.DeviceInformation.Disk.Cylinders;

    DiskGeometry->SectorsPerTrack =
        DeviceInformation.u.BlockIo.DeviceInformation.Disk.SectorsPerTrack;

    DiskGeometry->NumberOfHeads =
        DeviceInformation.u.BlockIo.DeviceInformation.Disk.Heads;

    *DiskPeripheralEntry = DiskEntry;

CreateDiskPeripheralEntryEnd:

    //
    // In the error case, free any memory allocated by this routine.
    //

    if (!NT_SUCCESS(Status)) {
        OslpCleanupControllerNode(DiskEntry);
    }

    return Status;
}

VOID
OslpCollectInt13DriveParameters (
    __in ULONG DiskCount,
    __out_ecount(DiskCount) PCM_INT13_DRIVE_PARAMETER ParameterBuffer,
    __in_ecount(DiskCount) PDEVICE_ID DiskIdList
    )

/*++

Routine Description:

    This function iterates through the given list of hard disks and collects
    their respect int 13h geometry information into the given buffer.

Arguments:

    DiskCount - Supplies the number of disks to process.

    ParameterBuffer - Supplies a buffer that receives the int 13h geometry for
        each disk listed in the given device ID list.

    DiskIdList - Supplies a list containing the device IDs for all hard disks
        enumerated by the boot environment.

Return Value:

    None.

--*/

{

    DEVICE_ID DeviceId;
    DEVICE_INFORMATION DeviceInformation;
    PDEVICE_DISK_INFORMATION DiskInformation;
    ULONG Index;
    PCM_INT13_DRIVE_PARAMETER Parameters;
    NTSTATUS Status;

    //
    // Iterate through the supplied list of device IDs.
    //

    DiskInformation = &DeviceInformation.u.BlockIo.DeviceInformation.Disk;
    Parameters = &ParameterBuffer[0];
    for (Index = 0; Index < DiskCount; Index += 1) {

        //
        // Query the device information for this disk.
        //

        RtlZeroMemory(&DeviceInformation, sizeof(DEVICE_INFORMATION));
        DeviceId = DiskIdList[Index];
        Status = BlDeviceGetInformation(DeviceId, &DeviceInformation);

        ASSERT(Status == STATUS_SUCCESS);
        ASSERT(DeviceInformation.DeviceType == DEVICE_BLOCK_IO);
        ASSERT(DeviceInformation.u.BlockIo.Type == BLOCK_IO_HARD_DISK);

        //
        // Populate the current slot in the parameter buffer with information
        // on the geometry of this disk.  Consumers of this geometry in the OS
        // expect it to be supplied in the raw form returned by the BIOS.  The
        // boot library adds one to both the cylinder and head values, so
        // these values must be decremented in order to exactly match the
        // geometry supplied by the BIOS.
        //
        // N.B. The device information fields consumed here are only
        //      initialized on machines that use PC/AT style drive geometry.
        //      On other platforms random data is loaded into the parameter
        //      structure.  This is acceptable since the OS doesn't consume
        //      these parameters on such platforms.
        //

        Parameters->DriveSelect = DiskInformation->DriveNumber;
        Parameters->MaxCylinders = DiskInformation->Cylinders - 1;
        Parameters->SectorsPerTrack = (USHORT)DiskInformation->SectorsPerTrack;
        Parameters->MaxHeads = (USHORT)(DiskInformation->Heads - 1);
        Parameters->NumberDrives = (USHORT)DiskCount;
        Parameters += 1;
    }

    return;
}

NTSTATUS
OslpDeleteControllerNode (
    __inout PCONFIGURATION_COMPONENT_DATA Node
    )

/*++

Routine Description:

    Utility routine to deallocate a config tree node.

Arguments:

    Node - pointer to the node to deallocate.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if an allocation fails.

--*/

{

    PCONFIGURATION_COMPONENT Component;

    Component = &Node->ComponentEntry;
    if (Node != NULL) {
        if (Component->Identifier != NULL) {
            BlMmFreeHeap(Component->Identifier);
        }

        if (Node->ConfigurationData != NULL) {
            BlMmFreeHeap(Node->ConfigurationData);
        }

        BlMmFreeHeap(Node);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
OslpCleanupControllerNode (
    __inout PCONFIGURATION_COMPONENT_DATA ConfigurationTreeEntry
    )

/*++

Routine Description:

    Utility routine to deallocate a configuration tree.  It walks the
    entire configuration tree and deallocates all nodes.

Arguments:

    ConfigurationTreeEntry - pointer to root of tree to delete.

Return Value:

    STATUS_SUCCESS when successful.

--*/

{

    PCONFIGURATION_COMPONENT_DATA CurrentEntry;
    PCONFIGURATION_COMPONENT_DATA ParentEntry;
    PCONFIGURATION_COMPONENT_DATA SiblingEntry;

    CurrentEntry = ConfigurationTreeEntry;
    while (CurrentEntry != NULL) {

        //
        // Keep traversing down if you see a child.
        //

        if (CurrentEntry->Child) {
            ParentEntry = CurrentEntry;
            CurrentEntry = CurrentEntry->Child;
            ParentEntry->Child = NULL;

        } else if (CurrentEntry->Sibling) {

            //
            // There are no children for this node.  It can be deleted,
            // but the sibling must be saved off first so it can be
            // traversed.
            //

            SiblingEntry = CurrentEntry->Sibling;

            ASSERT( SiblingEntry->Parent == CurrentEntry->Parent);

            OslpDeleteControllerNode(CurrentEntry);
            CurrentEntry = SiblingEntry;

        } else {

            //
            // This entry has neither children nor siblings.
            // It can be deleted, but we must save off the parent
            // pointer.
            //

            ParentEntry = CurrentEntry->Parent;

            ASSERT((ParentEntry == NULL) || (ParentEntry->Child == NULL));

            OslpDeleteControllerNode(CurrentEntry);
            CurrentEntry = ParentEntry;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
OslpInitializeControllerNode (
    __in CONFIGURATION_CLASS Class,
    __in CONFIGURATION_TYPE Type,
    __in ULONG AffinityMask,
    __in_bcount_opt(IdentifierLength) PCHAR IdentifierString,
    __in size_t IdentifierLength,
    __deref_out PCONFIGURATION_COMPONENT_DATA *ControllerNode
    )

/*++

Routine Description:

    Utility routine to allocate and initialize a configuration tree entry.

Arguments:

    Class - specifies the class for the controller node member data.
    Type - specifies the type for the controller node member data.
    AffinityMask - specifies the Affinity for the controller node member data.
    IdentifierString - if specified, specifies the identifier member data.
    IdentifierLength - specifies the length in bytes of IdentifierString.
    ControllerNode - On output, contains a pointer to a newly allocated
        configuration tree entry.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if an allocation fails.

--*/

{

    PCONFIGURATION_COMPONENT Component;
    PCONFIGURATION_COMPONENT_DATA ConfigurationTreeEntry;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;
    *ControllerNode = NULL;
    ConfigurationTreeEntry = BlMmAllocateHeap(sizeof(CONFIGURATION_COMPONENT_DATA));
    if (ConfigurationTreeEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto InitializeControllerNodeEnd;
    }

    //
    //  Initialize the configuration node.
    //

    RtlZeroMemory(ConfigurationTreeEntry, sizeof(CONFIGURATION_COMPONENT_DATA));
    Component = &ConfigurationTreeEntry->ComponentEntry;
    Component->Class = Class;
    Component->Type = Type;
    Component->AffinityMask = AffinityMask;
    Component->IdentifierLength = (ULONG)IdentifierLength;
    if (Component->IdentifierLength) {
        Component->Identifier = BlMmAllocateHeap(IdentifierLength);
        if (Component->Identifier == NULL) {
            Status = STATUS_NO_MEMORY;
            goto InitializeControllerNodeEnd;
        }

        strcpy_s(Component->Identifier, IdentifierLength, IdentifierString);
    }

    *ControllerNode = ConfigurationTreeEntry;

InitializeControllerNodeEnd:

    if (!NT_SUCCESS(Status)) {
        OslpCleanupControllerNode(ConfigurationTreeEntry);
    }

    return Status;
}

