/*++

Copyright (c) Microsoft Corporation

Module Name:

    fwconfig.c

Abstract:

    This module will build the loader's configuration tree for an EFI
    boot application.

Environment:

    Boot Environment.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"

//
// -------------------------------------------------- Local Function Prorotypes
//

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
OslFwGetVideoBiosData(
    __deref_out_ecount(*RomBlockCount) PCM_ROM_BLOCK *RomBlockArray,
    __out PULONG RomBlockCount
    )

/*++

Routine Description:

    This routine will retreive the video bios data.  It checks for EBDA range,
    fills that in, and passes back a CM_ROM_BLOCK array.

Arguments:

    RomBlockArray - receives a CM_ROM_BLOCK array.
    RomBlockCount - number of elements in CM_ROM_BLOCK array.

Return Value:

    NTSTATUS indicating outcome.

--*/
{
    //
    // An EFI system has no mechanism to retreive legacy bios ROM information.
    //

    *RomBlockCount = 0;
    *RomBlockArray = 0;

    return STATUS_SUCCESS;

}

USHORT
OslFwGetKeyboardFlags(
    VOID
    )

/*++

Routine Description:

    This routine will retreive the keyboard LED state.  On EFI systems there
    is no interface for retreiving the keyboard LED state.

Arguments:

    None.

Return Value:

    USHORT indicating the LED state.

--*/

{
    return 0;
}

NTSTATUS
OslFwGetFloppyControllerData(
     __deref_out_bcount(*Size) PCM_PARTIAL_RESOURCE_LIST *ResourceData,
     __out PULONG Size
     )

/*++

Routine Description:

    In an ACPI system, a floppy device should be enumerated via the ACPI.
    The _FDE method (present under the floppy controller) is used to enumerate
    floppies and _FDI may be used to retreive the floppy specific data.

    As of this coding, there are a number of ACPI machines which do not
    implement _FDE.  Therefore this routine is provided to retreive the same
    information that _FDE would provide via platform interfaces.  The system
    uses the floppy information from ACPI in preference over this information,
    so it is anticipated that via WHQL requirements and eventual removal of
    support for floppies from platforms that this code can eventually be
    removed.

Arguments:

    ResourceData - receives the PCM_PARTIAL_RESOURCE_LIST describing the floppy
            drive resource parameters.
    Size - receives the size of the allocated ResourceData.

Return Value:

    NTSTATUS code indicating outcome.

--*/

{

    UNREFERENCED_PARAMETER(ResourceData);
    UNREFERENCED_PARAMETER(Size);

    //
    // An EFI platform does not provide a mechanism for retreiving floppy
    // information.  The ACPI mechanism is the only mechanism available.
    //


    return STATUS_SUCCESS;

}



NTSTATUS
OslFwGetFloppyConfigurationData(
    __in DEVICE_ID DeviceId,
    __deref_out_bcount(*Size) PCM_FLOPPY_DEVICE_DATA *FloppyData,
    __out PULONG Size
    )

/*++

Routine Description:

    In an ACPI system, a floppy device should be enumerated via the ACPI.
    The _FDE method is used to enumerate floppies and _FDI and _CRS may be
    used to retreive the hardware register locations.

    As of this coding, there are a number of ACPI machines which do not
    implement _FDE.  Therefore this routine is provided to retreive the same
    information that _FDE would provide via platform interfaces.  The system
    uses the floppy information from ACPI in preference over this information,
    so it is anticipated that via WHQL requirements and eventual removal of
    support for floppies from platforms that this code can eventually be
    removed.

Arguments:

    DeviceId - block device id.
    ResourceData - receives the PCM_PARTIAL_RESOURCE_LIST describing the floppy
            drive resource parameters.
    Size - receives the size of the allocated ResourceData.

Return Value:

    NTSTATUS code indicating outcome.

--*/

{

    UNREFERENCED_PARAMETER(DeviceId);
    UNREFERENCED_PARAMETER(FloppyData);

    if (Size != NULL) {
        *Size = 0;
    }

    //
    // An EFI platform does not provide a mechanism for retreiving floppy
    // information.  The ACPI mechanism is the only mechanism available.
    //

    return(STATUS_SUCCESS);

}

NTSTATUS
OslFwInitializeLoaderBlock (
    __out PFIRMWARE_INFORMATION_LOADER_BLOCK FirmwareInformation
    )

/*++

Routine Description:

    This routine allocates and initializes the EFI runtime services block,
    and creates the firmware resource list.

Arguments:

    FirmwareInformation - Supplies a pointer to the buffer that needs to be
        filled with firmware-specific information.

Return Value:

    NTSTATUS code.

--*/

{

    PEFI_FIRMWARE_INFORMATION EfiInformation;
    PEFI_SYSTEM_RESOURCE_TABLE EfiSystemResourceTable;
    ULONG EfiSystemResourceTableSize;
    PEFI_SYSTEM_RESOURCE_ENTRY FirmwareResource;
    ULONG FirmwareResourceCount;
    PFIRMWARE_RESOURCE_LIST_ENTRY FirmwareResourceListEntry;
    ULONG Index;
    PLIST_ENTRY NextEntry;
    PHYSICAL_ADDRESS PhysicalAddress;
    ULONG Size;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;
    EfiSystemResourceTable = NULL;
    EfiSystemResourceTableSize = sizeof(EFI_SYSTEM_RESOURCE_TABLE);
    EfiInformation = &FirmwareInformation->u.EfiInformation;
    Size = sizeof(VIRTUAL_EFI_RUNTIME_SERVICES);
    EfiInformation->VirtualEfiRuntimeServices = BlMmAllocateHeap(Size);
    if (EfiInformation->VirtualEfiRuntimeServices == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OslFwInitializeLoaderBlockExit;
    }

    RtlZeroMemory(EfiInformation->VirtualEfiRuntimeServices, Size);

    //
    // Initialize the firmware resource list.
    //

    InitializeListHead(&EfiInformation->FirmwareResourceList);
    if (!NT_SUCCESS(BlFwGetSystemTable(FW_TABLE_EFI_SYSTEM_RESOURCES,
                                       &PhysicalAddress))) {

        goto OslFwInitializeLoaderBlockExit;
    }

    //
    // The firmware resource table exists.  Map its physical address.
    //

    if (!NT_SUCCESS(BlMmMapPhysicalAddress(&EfiSystemResourceTable,
                                           PhysicalAddress,
                                           EfiSystemResourceTableSize,
                                           MEMORY_ATTRIBUTE_NONE))) {

        goto OslFwInitializeLoaderBlockExit;
    }

    //
    // Insist on the correct firmware resource version, and at least one
    // table entry.
    //

    if ((EfiSystemResourceTable->FirmwareResourceVersion !=
         FIRMWARE_RESOURCE_ENTRY_VERSION) ||
        (EfiSystemResourceTable->FirmwareResourceCount == 0) ||
        (EfiSystemResourceTable->FirmwareResourceCount >
         EfiSystemResourceTable->FirmwareResourceMaximum)) {

        BlStatusPrint(L"Invalid ESRT table format!\n");
        goto OslFwInitializeLoaderBlockExit;
    }

    //
    // Re-map the entire table if it contains more than one entry.
    //

    FirmwareResourceCount = EfiSystemResourceTable->FirmwareResourceCount;
    if (FirmwareResourceCount > 1) {
        BlMmUnmapVirtualAddress(EfiSystemResourceTable,
                                EfiSystemResourceTableSize);

        EfiSystemResourceTable = NULL;
        EfiSystemResourceTableSize += ((FirmwareResourceCount - 1) *
                                       sizeof(EFI_SYSTEM_RESOURCE_ENTRY));

        if (!NT_SUCCESS(BlMmMapPhysicalAddress(&EfiSystemResourceTable,
                                               PhysicalAddress,
                                               EfiSystemResourceTableSize,
                                               MEMORY_ATTRIBUTE_NONE))) {

            goto OslFwInitializeLoaderBlockExit;
        }
    }

    //
    // Create a list item for each entry and add it to the firmware resource
    // list.
    //

    for (Index = 0; Index < FirmwareResourceCount; Index += 1) {
        FirmwareResourceListEntry =
            BlMmAllocateHeap(sizeof(FIRMWARE_RESOURCE_LIST_ENTRY));

        //
        // Delete all entries on failure.
        //

        if (FirmwareResourceListEntry == NULL) {
            while (IsListEmpty(&EfiInformation->FirmwareResourceList) ==
                   FALSE) {

                NextEntry =
                    RemoveTailList(&EfiInformation->FirmwareResourceList);

                FirmwareResourceListEntry = CONTAINING_RECORD(
                                                NextEntry,
                                                FIRMWARE_RESOURCE_LIST_ENTRY,
                                                Entry);

                BlMmFreeHeap(FirmwareResourceListEntry);
            }

            break;
        }

        FirmwareResource = &FirmwareResourceListEntry->FirmwareResource;
        RtlCopyMemory(FirmwareResource,
                      &EfiSystemResourceTable->FirmwareResource[Index],
                      sizeof(EFI_SYSTEM_RESOURCE_ENTRY));

        InsertTailList(&EfiInformation->FirmwareResourceList,
                       &FirmwareResourceListEntry->Entry);
    }

OslFwInitializeLoaderBlockExit:
    if (EfiSystemResourceTable != NULL) {
        BlMmUnmapVirtualAddress(EfiSystemResourceTable,
                                EfiSystemResourceTableSize);
    }

    ASSERT(NT_SUCCESS(Status) ||
           (EfiInformation->VirtualEfiRuntimeServices == NULL));

    return Status;
}

NTSTATUS
OslFwDestroyLoaderBlock (
    __in PFIRMWARE_INFORMATION_LOADER_BLOCK FirmwareInformation
    )

/*++

Routine Description:

    This routine destroys all EFI-specific information within the firmware
    information block.

Arguments:

    FirmwareInformation - Supplies a pointer to the buffer that needs to be
        filled with firmware-specific information.

Return Value:

    NTSTATUS code.

--*/

{

    PEFI_FIRMWARE_INFORMATION EfiInformation;
    PFIRMWARE_RESOURCE_LIST_ENTRY FirmwareResourceListEntry;
    PLIST_ENTRY NextEntry;

    EfiInformation = &FirmwareInformation->u.EfiInformation;
    BlMmFreeHeap(EfiInformation->VirtualEfiRuntimeServices);
    EfiInformation->VirtualEfiRuntimeServices = NULL;
    while (IsListEmpty(&EfiInformation->FirmwareResourceList) == FALSE) {
        NextEntry = RemoveTailList(&EfiInformation->FirmwareResourceList);
        FirmwareResourceListEntry = CONTAINING_RECORD(
                                        NextEntry,
                                        FIRMWARE_RESOURCE_LIST_ENTRY,
                                        Entry);

        BlMmFreeHeap(FirmwareResourceListEntry);
    }

    if (EfiInformation->EfiMemoryMap != NULL) {
        BlMmFreeHeap(EfiInformation->EfiMemoryMap);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
OslFwGetBootModeInfo (
    __out PEFI_OS_BOOT_MODE BootMode,
    __out PUNICODE_STRING BootProfile
    )

/*++

Routine Description:

Arguments:

    BootMode - Supplies a pointer to a variable that receives the currently
        selected boot mode.

    BootProfile - Supplies a pointer to receive the boot mode profile name.

Return Value:

    NTSTATUS code.

--*/

{

    EFI_HANDLE EfiHandle;
    EFI_HANDLE *HandleArray;
    EFI_OS_BOOT_MODE LocalBootMode;
    UINTN NumberDevices;
    PWSTR ProfileName;
    UINT32 ProfileNameElements;
    EFI_BOOT_MODE_MGMT_PROTOCOL *ProtocolPointer;
    ULONG ReturnedBytes;
    NTSTATUS Status;

    EfiHandle = NULL;
    HandleArray = NULL;
    NumberDevices = 0;
    ProfileName = NULL;
    ProtocolPointer = NULL;
    Status = STATUS_SUCCESS;

    //
    // Determine if any boot mode management interfaces exist.  If more than one
    // handle is returned, use the first handle of the list.
    //

    Status = EfiLocateHandleBuffer(ByProtocol,
                                   &EfiBootModeMgmtProtocol,
                                   NULL,
                                   &NumberDevices,
                                   &HandleArray);

    if (!NT_SUCCESS(Status)) {
        goto FwGetBootModeInfoEnd;

    }

    if (NumberDevices == 0) {
        Status = STATUS_NOT_SUPPORTED;
        goto FwGetBootModeInfoEnd;
    }

    EfiHandle = HandleArray[0];

    //
    // Now that there is a handle to the EFI interface, query for the protocol.
    //

    Status = EfiOpenProtocol(EfiHandle,
                             &EfiBootModeMgmtProtocol,
                             &ProtocolPointer);

    if (!NT_SUCCESS(Status)) {
        goto FwGetBootModeInfoEnd;
    }

    if (ProtocolPointer->Revision < EFI_BOOT_MODE_MGMT_PROTOCOL_REVISION) {
        Status = STATUS_NOT_SUPPORTED;
        goto FwGetBootModeInfoEnd;
    }

    ProfileNameElements = MANUFACTURING_PROFILE_MAX_NAME_SIZE / sizeof(WCHAR);
    ProfileName = BlMmAllocateHeap(MANUFACTURING_PROFILE_MAX_NAME_SIZE);

    if (ProfileName == NULL) {
        Status = STATUS_NO_MEMORY;
        goto FwGetBootModeInfoEnd;
    }

    Status = EfiBootModeMgmtGetBootModeInfo(ProtocolPointer,
                                            &LocalBootMode,
                                            &ProfileNameElements,
                                            ProfileName);

    if (!NT_SUCCESS(Status)) {
        goto FwGetBootModeInfoEnd;
    }

    //
    // Make sure the profile name is valid and null terminated.
    //

    ReturnedBytes = ProfileNameElements * sizeof(WCHAR);
    if (ReturnedBytes > MANUFACTURING_PROFILE_MAX_NAME_SIZE)  {
        Status = STATUS_UNSUCCESSFUL;
        goto FwGetBootModeInfoEnd;
    }

    if ((ReturnedBytes == MANUFACTURING_PROFILE_MAX_NAME_SIZE) &&
        (ProfileName[ProfileNameElements-1] != UNICODE_NULL)) {

        Status = STATUS_UNSUCCESSFUL;
        goto FwGetBootModeInfoEnd;

    } else {
        ProfileName[ProfileNameElements] = UNICODE_NULL;
    }

    //
    // Return the output data.
    //

    *BootMode = LocalBootMode;
    BootProfile->Length = (USHORT)ReturnedBytes;
    BootProfile->MaximumLength = MANUFACTURING_PROFILE_MAX_NAME_SIZE;
    BootProfile->Buffer = ProfileName;
    ProfileName = NULL;

  FwGetBootModeInfoEnd:

    if (ProtocolPointer != NULL) {
        EfiCloseProtocol(EfiHandle,
                         &EfiBootModeMgmtProtocol,
                         ProtocolPointer);
    }

    if (HandleArray != NULL) {
        BlMmFreeHeap(HandleArray);
    }

    if (ProfileName != NULL) {
        BlMmFreeHeap(ProfileName);
    }

    return Status;
}


