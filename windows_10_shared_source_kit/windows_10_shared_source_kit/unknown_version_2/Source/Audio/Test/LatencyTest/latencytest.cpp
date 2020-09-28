lEntry;
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
        // geometry supplied by the BIOS