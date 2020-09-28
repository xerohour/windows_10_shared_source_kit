/*++

Copyright (c) Microsoft Corporation

Module Name:

    fwupdate.c

Abstract:

    This module implements UEFI update capsule firmware update.

Environment:

    Boot.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"
#include "systemevents.h"

//
// ---------------------------------------------------------------- Definitions
//

#define XOR(_a, _b) ((((_a) != FALSE) && ((_b) == FALSE)) || \
            (((_a) == FALSE) && ((_b) != FALSE)))

typedef struct _FIRMWARE_UPDATE_DESCRIPTOR {
    LIST_ENTRY Entry;
    PFIRMWARE_UPDATE_SETTINGS FirmwareUpdateSettings;
    EFI_CAPSULE_BLOCK_DESCRIPTOR *CapsuleDescriptor;
    ULONG CapsuleAllocationSize;
    PHYSICAL_ADDRESS CapsuleDescriptorAddress;
    EFI_CAPSULE_HEADER *UpdateCapsule;
    PHYSICAL_ADDRESS UpdateCapsuleAddress;
} FIRMWARE_UPDATE_DESCRIPTOR, *PFIRMWARE_UPDATE_DESCRIPTOR;

#define OSL_FIRMWARE_UPDATE_VALIDATION_FLAGS \
    (IMAGE_FLAGS_VALIDATE_IMAGE_HASH | \
     IMAGE_FLAGS_FIRMWARE_UPDATE | \
     IMAGE_FLAGS_MS_SIGNATURE_REQUIRED_CATALOG | \
     IMAGE_FLAGS_VALID_HASH_REQUIRED | \
     IMAGE_FLAGS_NO_REVOCATION_LIST_CHECKS)

//
// EFI Firmware update display capsule.
//

typedef struct {
  UINT8     Version;
  UINT8     Checksum;
  UINT8     ImageType;
  UINT8     Reserved;
  UINT32    Mode;
  UINT32    ImageOffsetX;
  UINT32    ImageOffsetY;
} EFI_FIRMWARE_UPDATE_DISPLAY_CAPSULE_PAYLOAD;

#define EFI_FIRMWARE_UPDATE_DISPLAY_CAPSULE \
    { 0x3b8c8162, 0x188c, 0x46a4, 0xae, 0xc9, 0xbe, 0x43, 0xf1, 0xd6, 0x56, 0x97 }

EFI_GUID EfiFirmwareUpdateDisplayGuid = EFI_FIRMWARE_UPDATE_DISPLAY_CAPSULE;

//
// -------------------------------------------------- Local Function Prototypes
//

NTSTATUS
OslpEfiApplyUpdates (
    __in ULONG Count,
    __in PLIST_ENTRY UpdateSettings,
    __in ULONG SystemHiveHandle,
    __in PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId
    );

PFIRMWARE_UPDATE_DESCRIPTOR
OslpEfiCreateDisplayCapsule (
    VOID
    );

VOID
OslpEfiFreeUpdate (
    __in PFIRMWARE_UPDATE_DESCRIPTOR UpdateDescriptor
    );

NTSTATUS
OslpEfiLoadUpdate (
    __in PFIRMWARE_UPDATE_SETTINGS FirmwareUpdateSettings,
    __in PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __out PFIRMWARE_UPDATE_DESCRIPTOR *FirmwareUpdateDescriptor
    );

VOID
OslpEfiLogStatus (
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in_opt PUNICODE_STRING FilePath,
    __in NTSTATUS Status
    );

NTSTATUS
OslpEfiReportUpdates (
    __in ULONG Count,
    __in PLIST_ENTRY UpdateSettings
    );

VOID
OslpEfiSetUpdateStatus (
    __in GUID *FirmwareClass,
    __in NTSTATUS LastAttemptStatus,
    __in UINT32 LastAttemptVersion,
    __out PFIRMWARE_UPDATE_INFORMATION FirmwareUpdateInformation
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
OslFwProcessUpdates (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __in ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This is the main entrypoint for processing UEFI update capsule firmware
    updates.  It has two primary purposes, loading updates and calling UEFI
    update capsule, and providing status back up to the OS of previous update
    attempts.

    N.B. When calling UEFI update capsule to perform firmware updates, control
         does not return to this function.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

    OsFilePath - Supplies the file path to the Operating System directory.

    OsDeviceId - Supplies a handle to the device containing the Operating
        System.

    SystemHiveHandle - Supplies a handle for the system hive.

Return Value:

    NT status code.

--*/

{

    PEFI_FIRMWARE_INFORMATION EfiInformation;
    PEFI_SYSTEM_RESOURCE_ENTRY FirmwareResource;
    PFIRMWARE_UPDATE_SETTINGS FailedFirmwareUpdateSettings;
    LIST_ENTRY FailedUpdateList;
    PLIST_ENTRY FirmwareResourceList;
    PFIRMWARE_RESOURCE_LIST_ENTRY FirmwareResourceListEntry;
    PFIRMWARE_UPDATE_SETTINGS FirmwareUpdateSettings;
    PLIST_ENTRY NextEntry;
    PLIST_ENTRY NextFailedEntry;
    BOOLEAN Repeat;
    ULONG Report;
    NTSTATUS Status;
    ULONG Update;
    LIST_ENTRY UpdateList;

    //
    // Sanitize input parameters.
    //

    InitializeListHead(&UpdateList);
    InitializeListHead(&FailedUpdateList);
    if ((ARGUMENT_PRESENT(LoaderBlock) == FALSE) ||
        (ARGUMENT_PRESENT(OsFilePath) == FALSE) ||
        (OsDeviceId == INVALID_DEVICE_ID)) {

        Status = STATUS_INVALID_PARAMETER;
        goto ProcessUpdatesEnd;
    }

    //
    // If there are no firmware resources then return success.
    //

    Status = STATUS_SUCCESS;
    EfiInformation = &LoaderBlock->FirmwareInformation.u.EfiInformation;
    FirmwareResourceList = &EfiInformation->FirmwareResourceList;
    if (IsListEmpty(FirmwareResourceList) != FALSE) {
        goto ProcessUpdatesEnd;
    }

    //
    // Walk the firmware resource list, and create a list of firmware update
    // settings.
    //

    Repeat = FALSE;

NextUpdatePhase:
    Update = 0;
    Report = 0;
    NextEntry = FirmwareResourceList->Flink;
    while (NextEntry != FirmwareResourceList) {
        FirmwareResourceListEntry = CONTAINING_RECORD(
                                        NextEntry,
                                        FIRMWARE_RESOURCE_LIST_ENTRY,
                                        Entry);

        FirmwareResource = &FirmwareResourceListEntry->FirmwareResource;
        FirmwareUpdateSettings = OslHiveGetUpdateSettings(FirmwareResource,
                                                          SystemHiveHandle);

        if (FirmwareUpdateSettings != NULL) {
            InsertTailList(&UpdateList, &FirmwareUpdateSettings->Entry);

            //
            // Each time an update setting is fetched, its phase value is
            // automatically incremented.  Only settings whose phase was
            // previously update can be re-queried, in which the next phase
            // must be report.
            //

            if (Repeat != FALSE) {

                ASSERT(FirmwareUpdateSettings->Phase ==
                       FIRMWARE_UPDATE_PHASE_REPORT);

                //
                // Locate firmware resource in the failed update list from the
                // last update phase and promote its load failure status to
                // this entry for reporting purposes.
                //

                NextFailedEntry = FailedUpdateList.Flink;
                while (NextFailedEntry != &FailedUpdateList) {
                    FailedFirmwareUpdateSettings = CONTAINING_RECORD(
                                                        NextFailedEntry,
                                                        FIRMWARE_UPDATE_SETTINGS,
                                                        Entry);

                    if (FailedFirmwareUpdateSettings->Resource == FirmwareResource) {

                        FirmwareUpdateSettings->LoadStatus = 
                                FailedFirmwareUpdateSettings->LoadStatus;

                        break;
                    }

                    NextFailedEntry = NextFailedEntry->Flink;
                }

            }

            if (FirmwareUpdateSettings->Phase ==
                FIRMWARE_UPDATE_PHASE_UPDATE) {

                Update += 1;

            } else if (FirmwareUpdateSettings->Phase ==
                       FIRMWARE_UPDATE_PHASE_REPORT) {

                Report += 1;
            }
        }

        NextEntry = NextEntry->Flink;
    }

    //
    // No updates found, return.
    //

    if ((Update + Report) == 0) {

        ASSERT(IsListEmpty(&UpdateList) != FALSE);
        ASSERT(IsListEmpty(&FailedUpdateList) != FALSE);

        goto ProcessUpdatesEnd;
    }

    //
    // Process updates.
    //
    // N.B. Since all updates are applied in batch during one boot-cycle,
    //      the 'Phase' should always be in sync., i.e., either update or
    //      report, but never both.  Otherwise, if report and update are
    //      false, then this indicates an error since this routine exited
    //      earlier in the case where there aare no updates to process.
    //

    ASSERT(XOR((Update > 0), (Report > 0)) != FALSE);

    if (Update > 0) {

        ASSERT(Repeat == FALSE);

        Status = OslpEfiApplyUpdates(Update,
                                     &UpdateList,
                                     SystemHiveHandle,
                                     OsFilePath,
                                     OsDeviceId);

        //
        // If an error occurred processing updates, free the updates settings
        // and fetch them again.  This will force the next update phase
        // (reporting) without requiring an additional reboot.
        //

        if (!NT_SUCCESS(Status)) {
            
            //
            // Update settings list contains load failure information that
            // should be preserved for reporting purposes.
            //

            while (IsListEmpty(&UpdateList) == FALSE) {
                NextEntry = RemoveTailList(&UpdateList);
                FirmwareUpdateSettings = CONTAINING_RECORD(
                                            NextEntry,
                                            FIRMWARE_UPDATE_SETTINGS,
                                            Entry);

                InsertTailList(&FailedUpdateList,
                               &FirmwareUpdateSettings->Entry);
            }

            Repeat = TRUE;
            goto NextUpdatePhase;
        }
    }

    //
    // Report firmware update results.
    //

    if (Report > 0) {
        Status = OslpEfiReportUpdates(Report, &UpdateList);
    }

    //
    // Free any update settings.
    //

    while (IsListEmpty(&UpdateList) == FALSE) {
        NextEntry = RemoveTailList(&UpdateList);
        FirmwareUpdateSettings = CONTAINING_RECORD(NextEntry,
                                                   FIRMWARE_UPDATE_SETTINGS,
                                                   Entry);

        BlMmFreeHeap(FirmwareUpdateSettings);
    }

    while (IsListEmpty(&FailedUpdateList) == FALSE) {
        NextFailedEntry = RemoveTailList(&FailedUpdateList);
        FailedFirmwareUpdateSettings = CONTAINING_RECORD(
                                            NextFailedEntry,
                                            FIRMWARE_UPDATE_SETTINGS,
                                            Entry);

        BlMmFreeHeap(FailedFirmwareUpdateSettings);
    }

ProcessUpdatesEnd:
    return Status;
}

//
// ------------------------------------------------------------ Local Functions
//

NTSTATUS
OslpEfiApplyUpdates (
    __in ULONG Count,
    __in PLIST_ENTRY UpdateSettings,
    __in ULONG SystemHiveHandle,
    __in PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId
    )

/*++

Routine Description:

    This routine persists firmware update results so that they are available
    in the kernel.

Arguments:

    Count - Supplies the number of firmware updates to perform.

    UpdateSettings - Supplies the head of the update settings list.

    SystemHiveHandle - Supplies a handle for the system hive.

    OsFilePath - Supplies the file path to the Operating System directory.

    OsDeviceId - Supplies a handle to the device containing the Operating
        System.

Return Value:

    NT status code.

--*/

{

    BOOLEAN BootUxDisabled;
    PBOOT_APPLICATION_ENTRY BootEntry;
    EFI_CAPSULE_HEADER **CapsuleArray;
    PFIRMWARE_UPDATE_SETTINGS FirmwareUpdateSettings;
    EFI_CAPSULE_BLOCK_DESCRIPTOR *LastDescriptor;
    PLIST_ENTRY NextEntry;
    EFI_PHYSICAL_ADDRESS ScatterGatherList;
    NTSTATUS Status;
    EFI_CAPSULE_BLOCK_DESCRIPTOR *ThisDescriptor;
    ULONG UpdateCount;
    PFIRMWARE_UPDATE_DESCRIPTOR UpdateDescriptor;
    LIST_ENTRY UpdateList;
    BOOLEAN WinPE;

    CapsuleArray = NULL;
    InitializeListHead(&UpdateList);
    if ((Count == 0) || (OsFilePath == NULL) || (UpdateSettings == NULL)) {
        Status = STATUS_INVALID_PARAMETER;
        goto ApplyUpdatesEnd;
    }

    if (IsListEmpty(UpdateSettings) != FALSE) {
        Status = STATUS_SUCCESS;
        goto ApplyUpdatesEnd;
    }

    //
    // Flush the system hive to ensure that modifications are reflected back
    // to non-volatile storage.  If changes to the update phase value cannot
    // be successfully flushed to non-volatile sorage, then updates cannot
    // proceed, otherwise the system is at risk of a failure during the update
    // process resulting in an infinte update cycle.
    //
    // N.B. This is not necessary in the reporting phase, since there is
    //      presumably no harm in retrying the report phase in cases where the
    //      OS fails to load.  And if the OS loads successfully, then the OS is
    //      responsible to ensure that modifications to the currently loaded
    //      system hive are properly flushed back to non-volatile storage.
    //

    //
    // Don't flush the system hive on WinPE. For WinPE, the behavior is to
    // always attempt to update firmware (since the incremented update 'Phase'
    // value written to the registry is never flushed to backing sotre.)
    //

    BootEntry = BlGetApplicationEntry();

    ASSERT(BootEntry != NULL);

    Status = BlGetBootOptionBoolean(BootEntry->Options,
                                    BCDE_OSLOADER_TYPE_WINPE,
                                    &WinPE);

    if (!NT_SUCCESS(Status)) {
        WinPE = FALSE;
    }

    if (WinPE == FALSE) {
        Status = OslHiveFlush(SystemHiveHandle);

    } else {
        Status = STATUS_SUCCESS;
    }

    if (!NT_SUCCESS(Status)) {
        goto ApplyUpdatesEnd;
    }

    //
    // Walk through the updates, and attempt to load each one.
    //

    UpdateCount = 0;
    NextEntry = UpdateSettings->Flink;
    while (NextEntry != UpdateSettings) {
        FirmwareUpdateSettings = CONTAINING_RECORD(NextEntry,
                                                   FIRMWARE_UPDATE_SETTINGS,
                                                   Entry);

        //
        // The default policy is to only upgrade when the update version is
        // greater than the current version.
        //

        if ((FirmwareUpdateSettings->Policy == FIRMWARE_UPDATE_POLICY_DEFAULT)
            && (FirmwareUpdateSettings->Version <=
                FirmwareUpdateSettings->Resource->FirmwareVersion)) {

            NextEntry = NextEntry->Flink;
            continue;
        }

        Status = OslpEfiLoadUpdate(FirmwareUpdateSettings,
                                   OsFilePath,
                                   OsDeviceId,
                                   &UpdateDescriptor);

        if (!NT_SUCCESS(Status)) {
            FirmwareUpdateSettings->LoadStatus = Status;
            NextEntry = NextEntry->Flink;
            continue;
        }

        ASSERT(UpdateDescriptor != NULL);

        UpdateCount += 1;
        InsertTailList(&UpdateList, &UpdateDescriptor->Entry);
        NextEntry = NextEntry->Flink;
    }

    //
    // No updates could be loaded, fail.
    //

    if (UpdateCount == 0) {
        Status = STATUS_UNSUCCESSFUL;
        goto ApplyUpdatesEnd;
    }

    ASSERT(IsListEmpty(&UpdateList) == FALSE);

    //
    // Don't create display capsule if boot UX is disabled.
    //

    Status = BlGetApplicationOptionBoolean(BCDE_LIBRARY_TYPE_BOOTUX_DISABLE,
                                           &BootUxDisabled);

    if (!NT_SUCCESS(Status)) {
        BootUxDisabled = FALSE;
    }

    if (BootUxDisabled == FALSE) {
        UpdateDescriptor = OslpEfiCreateDisplayCapsule();
        if (UpdateDescriptor != NULL) {
            InsertTailList(&UpdateList, &UpdateDescriptor->Entry);

            //
            // Reserve one more update capsule for display capsule.
            //

            UpdateCount += 1;
        }
    }

    //
    // Allocate the capsule array.
    //

    CapsuleArray = BlMmAllocateHeap(UpdateCount *
                                    sizeof(EFI_CAPSULE_HEADER *));

    if (CapsuleArray == NULL) {
        Status = STATUS_NO_MEMORY;
        goto ApplyUpdatesEnd;
    }

    //
    // Traverse the update list, and initialize and link the scatter-gather
    // tables.  Each capsule allocation contains two capsule block descriptors.
    // Since the capsule allocation header, and data are contiguous, this is
    // guaranteed to suffice as the first descriptor fully describes the
    // entire capsule, header and all, and the second entry will be either a
    // link or an end node, depending on whether there are other capsules to
    // link with.
    //

    UpdateCount = 0;
    LastDescriptor = NULL;
    NextEntry = UpdateList.Flink;
    ScatterGatherList = 0;
    while (NextEntry != &UpdateList) {
        UpdateDescriptor = CONTAINING_RECORD(NextEntry,
                                             FIRMWARE_UPDATE_DESCRIPTOR,
                                             Entry);

        if (UpdateCount == 0) {
            ScatterGatherList =
                UpdateDescriptor->CapsuleDescriptorAddress.QuadPart;
        }

        //
        // Link the continuation pointer of the last capsule block to this
        // one.
        //

        if (LastDescriptor != NULL) {
            LastDescriptor->Union.ContinuationPointer =
                UpdateDescriptor->CapsuleDescriptorAddress.QuadPart;
        }

        //
        // Fill in the scatter-gather capsule block descriptors.
        //

        ThisDescriptor = UpdateDescriptor->CapsuleDescriptor;
        ThisDescriptor->Length =
            UpdateDescriptor->UpdateCapsule->CapsuleImageSize;

        ThisDescriptor->Union.DataBlock =
            UpdateDescriptor->UpdateCapsuleAddress.QuadPart;

        ThisDescriptor = Add2Ptr(ThisDescriptor,
                                 sizeof(EFI_CAPSULE_BLOCK_DESCRIPTOR));

        RtlZeroMemory(ThisDescriptor, sizeof(EFI_CAPSULE_BLOCK_DESCRIPTOR));
        CapsuleArray[UpdateCount] = UpdateDescriptor->UpdateCapsule;
        LastDescriptor = ThisDescriptor;
        NextEntry = NextEntry->Flink;
        UpdateCount += 1;
    }

    ASSERT(UpdateCount > 0);

    //
    // Set the boot status data to make sure the system tries to boot twice
    // after the firmware update before failing over to recovery.
    //
    // N.B. BootId is decremented, to avoid reporting a failed boot when the
    //      machine boots into the OS after firmware update.
    //

    OslSetBootStatusData(TRUE, TRUE, TRUE, OslBootCountUpdateDecrement, 0);

    //
    // Just before calling to update capsule which is expected to reset device
    // we will make sure there are no BitLocker secrets left in memory.
    //

    BlFveEraseAllSecrets();

    //
    // Call update capsule to apply the firmware updates.
    //
    // N.B. Should not return.
    //

    Status = EfiUpdateCapsule(CapsuleArray,
                              UpdateCount,
                              ScatterGatherList);

    ASSERT(FALSE);

    OslpEfiLogStatus(&BOOT_FW_UPDATE_UPDATE_CAPSULE_FAILURE, NULL, Status);

ApplyUpdatesEnd:
    while (IsListEmpty(&UpdateList) == FALSE) {
        NextEntry = RemoveTailList(&UpdateList);
        UpdateDescriptor = CONTAINING_RECORD(NextEntry,
                                             FIRMWARE_UPDATE_DESCRIPTOR,
                                             Entry);

        OslpEfiFreeUpdate(UpdateDescriptor);
    }

    if (CapsuleArray != NULL) {
        BlMmFreeHeap(CapsuleArray);
    }

    return Status;
}

NTSTATUS
OslpEfiConvertFwUpdateStatus (
    __in UINT32 FwUpdateStatus
    )

/*++

Routine Description:

    This routine maps an ESRT firmware update attempt status to the most
    appropriate NT status.

Arguments:

    FwUpdateStatus - Supplies the firmware update status to convert to NT
        status.

Return Value:

    NT status code.

--*/

{


    NTSTATUS Status;

    switch (FwUpdateStatus) {
        case FIRMWARE_UPDATE_STATUS_SUCCESS:
            Status = STATUS_SUCCESS;
            break;

        case FIRMWARE_UPDATE_STATUS_INSUFFICIENT_RESOURCES:
            Status = STATUS_INSUFFICIENT_RESOURCES;
            break;

        case FIRMWARE_UPDATE_STATUS_INCORRECT_VERSION:
            Status = STATUS_REVISION_MISMATCH;
            break;

        case FIRMWARE_UPDATE_STATUS_INVALID_IMAGE_FORMAT:
            Status = STATUS_INVALID_IMAGE_FORMAT;
            break;

        case FIRMWARE_UPDATE_STATUS_AUTHENTICATION_ERROR:
            Status = STATUS_ACCESS_DENIED;
            break;

        case FIRMWARE_UPDATE_STATUS_REQUIRES_AC:
            Status = STATUS_POWER_STATE_INVALID;
            break;

        case FIRMWARE_UPDATE_STATUS_INSUFFICIENT_BATTERY:
            Status = STATUS_INSUFFICIENT_POWER;
            break;

        case FIRMWARE_UPDATE_STATUS_UNSUCCESSFUL:
        default:
            Status = STATUS_UNSUCCESSFUL;
            break;
    }

    return Status;
}

PFIRMWARE_UPDATE_DESCRIPTOR
OslpEfiCreateDisplayCapsule (
    VOID
    )

/*++

Routine Description:

    This routine captures display frame buffer and creates the display capsule.

Arguments:

    None.

Return Value:

    Returns a heap allocated update pack for which the caller is responsible
    to free.

--*/

{

    PVOID CapsuleAllocationBase;
    ULONG CapsuleAllocationSize;
    ULONG CapsuleDescriptorSize;
    ULONG CapsuleHeaderSize;
    UCHAR Checksum;
    PBITMAP_IMAGE DisplayBitmap;
    PBITMAP_IMAGE_PROLOG DisplayBitmapProlog;
    EFI_FIRMWARE_UPDATE_DISPLAY_CAPSULE_PAYLOAD *DisplayCapsule;
    ULONG DisplayCapsuleSize;
    PBITMAP_IMAGE ImageBitmap;
    ULONG ImageHeight;
    ULONG ImageOffsetX;
    ULONG ImageOffsetY;
    ULONG ImageWidth;
    PWCHAR Message;
    ULONG Mode;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    PFIRMWARE_UPDATE_DESCRIPTOR UpdateDescriptor;

    CapsuleAllocationBase = NULL;
    CapsuleAllocationSize = 0; // Satisfy compiler.
    PhysicalAddress.QuadPart = 0; // Satisfy compiler.
    UpdateDescriptor = NULL;
    ImageBitmap = NULL;

    //
    // Display the firmware update message and capture the portion of message
    // as part of the update capsule to allow firmware continue displaying
    // the update message during progress.
    //

    Message = BlResourceFindMessage(OSL_DISPLAY_STRING_FIRMWARE_UPDATE);

    ASSERT(Message != NULL);

    Status = BgDisplayString(Message);

    //
    // Don't bother to capture the screen if BgDisplayString failed.
    //

    if (!NT_SUCCESS(Status)) {
        goto CreateDisplayCapsuleEnd;
    }

    Status = BgGetTextRegionContext(&ImageOffsetX,
                                    &ImageOffsetY,
                                    &ImageWidth,
                                    &ImageHeight);

    if (!NT_SUCCESS(Status)) {
        goto CreateDisplayCapsuleEnd;
    }

    ImageBitmap = BgGetDisplayImage(ImageOffsetX,
                                    ImageOffsetY,
                                    ImageWidth,
                                    ImageHeight);

    if (ImageBitmap == NULL) {
        goto CreateDisplayCapsuleEnd;
    }

    //
    // Allocate a buffer for the block descriptors, capsule header and data.
    //
    // N.B. Ensure the block descriptors (2), capsule header, and its data all
    //      begin on seperate page boundaries.
    //

    DisplayCapsuleSize = sizeof(EFI_FIRMWARE_UPDATE_DISPLAY_CAPSULE_PAYLOAD);
    DisplayCapsuleSize += sizeof(BITMAP_IMAGE_PROLOG);
    DisplayCapsuleSize += ImageBitmap->HeaderSize;
    DisplayCapsuleSize += ImageBitmap->DataSize;
    CapsuleDescriptorSize =
        ALIGN_RANGE_UP((2 * (sizeof(EFI_CAPSULE_BLOCK_DESCRIPTOR))),
                       PAGE_SIZE);

    CapsuleHeaderSize = sizeof(EFI_CAPSULE_HEADER);
    CapsuleAllocationSize =
        ALIGN_RANGE_UP(CapsuleHeaderSize + DisplayCapsuleSize, PAGE_SIZE);

    Status = RtlULongAdd(CapsuleAllocationSize,
                         CapsuleDescriptorSize,
                         &CapsuleAllocationSize);

    if (!NT_SUCCESS(Status)) {
        goto CreateDisplayCapsuleEnd;
    }

    Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                       (CapsuleAllocationSize / PAGE_SIZE),
                                       OSL_MEMORY_TYPE_FIRMWARE_UPDATE,
                                       MEMORY_ATTRIBUTE_NONE,
                                       0);

    if (!NT_SUCCESS(Status)) {
        goto CreateDisplayCapsuleEnd;
    }

    Status = BlMmMapPhysicalAddress(&CapsuleAllocationBase,
                                    PhysicalAddress,
                                    CapsuleAllocationSize,
                                    MEMORY_ATTRIBUTE_NONE);

    if (!NT_SUCCESS(Status)) {
        BlMmFreePhysicalPages(PhysicalAddress);
        goto CreateDisplayCapsuleEnd;
    }

    //
    // Compose the display capsule in the pre-allocated, page-aligned buffer.
    //

    DisplayCapsule = Add2Ptr(CapsuleAllocationBase,
                            (CapsuleDescriptorSize + CapsuleHeaderSize));

    //
    // Build bitmap prolog.
    //

    DisplayBitmapProlog =
        Add2Ptr(DisplayCapsule,
                sizeof(EFI_FIRMWARE_UPDATE_DISPLAY_CAPSULE_PAYLOAD));

    DisplayBitmapProlog->Identifier = 'MB';
    DisplayBitmapProlog->FileSize = sizeof(BITMAP_IMAGE_PROLOG);
    DisplayBitmapProlog->FileSize += ImageBitmap->HeaderSize;
    DisplayBitmapProlog->FileSize += ImageBitmap->DataSize;
    DisplayBitmapProlog->Reserved = 0;
    DisplayBitmapProlog->DataOffset = sizeof(BITMAP_IMAGE_PROLOG);
    DisplayBitmapProlog->DataOffset += ImageBitmap->HeaderSize;

    //
    // Build bitmap header and body.
    //

    DisplayBitmap = Add2Ptr(DisplayBitmapProlog, sizeof(BITMAP_IMAGE_PROLOG));
    RtlCopyMemory(DisplayBitmap,
                  ImageBitmap,
                  ImageBitmap->HeaderSize + ImageBitmap->DataSize);

    //
    // Build display capsule header.
    //

    DisplayCapsule->Version = 1;
    DisplayCapsule->ImageType = BgrtImageTypeBitmap;
    Status = BlDisplayGetMode(&Mode);
    if (!NT_SUCCESS(Status)) {
        goto CreateDisplayCapsuleEnd;
    }

    DisplayCapsule->Mode = Mode;
    DisplayCapsule->Reserved = 0;
    DisplayCapsule->ImageOffsetX = ImageOffsetX;
    DisplayCapsule->ImageOffsetY = ImageOffsetY;

    //
    // Allocate and initialize a firmware update descriptor and fill in the
    // capsule header.
    //

    UpdateDescriptor = BlMmAllocateHeap(sizeof(FIRMWARE_UPDATE_DESCRIPTOR));
    if (UpdateDescriptor == NULL) {
        goto CreateDisplayCapsuleEnd;
    }

    UpdateDescriptor->FirmwareUpdateSettings = NULL;
    UpdateDescriptor->CapsuleDescriptor = CapsuleAllocationBase;
    UpdateDescriptor->CapsuleAllocationSize = CapsuleAllocationSize;
    UpdateDescriptor->CapsuleDescriptorAddress = PhysicalAddress;
    UpdateDescriptor->UpdateCapsule = Add2Ptr(CapsuleAllocationBase,
                                              CapsuleDescriptorSize);

    UpdateDescriptor->UpdateCapsuleAddress.QuadPart = PhysicalAddress.QuadPart;
    UpdateDescriptor->UpdateCapsuleAddress.QuadPart += CapsuleDescriptorSize;
    RtlZeroMemory(UpdateDescriptor->CapsuleDescriptor, CapsuleDescriptorSize);
    RtlZeroMemory(UpdateDescriptor->UpdateCapsule, CapsuleHeaderSize);
    RtlCopyMemory(&UpdateDescriptor->UpdateCapsule->CapsuleGuid,
                  &EfiFirmwareUpdateDisplayGuid,
                  sizeof(EFI_GUID));

    UpdateDescriptor->UpdateCapsule->HeaderSize = CapsuleHeaderSize;
    UpdateDescriptor->UpdateCapsule->CapsuleImageSize = CapsuleHeaderSize;
    UpdateDescriptor->UpdateCapsule->CapsuleImageSize += DisplayCapsuleSize;

    //
    // Firmware update display capsule sets persist across reset flag.
    //

    UpdateDescriptor->UpdateCapsule->Flags = CAPSULE_FLAGS_PERSIST_ACROSS_RESET;

    //
    // Zero out the checksum.
    //

    DisplayCapsule->Checksum = 0;
    Checksum =
        (UCHAR)BlUtlCheckSum(0,
                             UpdateDescriptor->UpdateCapsule,
                             UpdateDescriptor->UpdateCapsule->CapsuleImageSize,
                             CHECKSUM_BYTE_ARRAY);

    DisplayCapsule->Checksum = -Checksum;

CreateDisplayCapsuleEnd:
    if (ImageBitmap != NULL) {
        BgFreeDisplayImage(ImageBitmap);
    }

    if (UpdateDescriptor == NULL) {
        if (CapsuleAllocationBase != NULL) {
            BlMmUnmapVirtualAddress(CapsuleAllocationBase,
                                    CapsuleAllocationSize);

            BlMmFreePhysicalPages(PhysicalAddress);
        }
    }

    return UpdateDescriptor;
}

VOID
OslpEfiFreeUpdate (
    __in PFIRMWARE_UPDATE_DESCRIPTOR UpdateDescriptor
    )

/*++

Routine Description:

    Frees a firmware update descriptor and its associated resources.

Arguments:

    UpdateDescriptor - Supplies the update descriptor to free.

Return Value:

    None.

--*/

{

    if (UpdateDescriptor != NULL) {
        if (UpdateDescriptor->CapsuleDescriptor != NULL) {
            BlMmUnmapVirtualAddress(UpdateDescriptor->CapsuleDescriptor,
                                    UpdateDescriptor->CapsuleAllocationSize);

            BlMmFreePhysicalPages(UpdateDescriptor->CapsuleDescriptorAddress);
        }

        BlMmFreeHeap(UpdateDescriptor);
    }

    return;
}

VOID
OslpEfiLogStatus (
    __in PCEVENT_DESCRIPTOR EventDescriptor,
    __in_opt PUNICODE_STRING FilePath,
    __in NTSTATUS Status
    )

/*++

Routine Description:

    This routine generates ETW event log with user supplied argument.

Arguments:

    EventDescriptor - Supplies the event descriptor to log.

    FilePath - Supplies the NULL terminated file path to log.

    Status - Supplies the NT Status code to log.

Return Value:

    None.

--*/

{

    ULONG EventCount;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[2];

    EventCount = 0;
    if (ARGUMENT_PRESENT(FilePath) != FALSE) {

        //
        // EventDataDescCreate needs the data length including the
        // NULL terminating character.
        //

        EventDataDescCreate(&EventDataDescriptor[EventCount],
                            (PVOID)(FilePath->Buffer),
                            FilePath->Length + sizeof(WCHAR));

        EventCount += 1;
    }

    EventDataDescCreate(&EventDataDescriptor[EventCount],
                        (PVOID)&Status,
                        sizeof(NTSTATUS));

    EventCount += 1;
    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  EventDescriptor,
                  NULL,
                  EventCount,
                  EventDataDescriptor);

    return;
}

NTSTATUS
OslpEfiLoadUpdate (
    __in PFIRMWARE_UPDATE_SETTINGS FirmwareUpdateSettings,
    __in PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __out PFIRMWARE_UPDATE_DESCRIPTOR *FirmwareUpdateDescriptor
    )

/*++

Routine Description:

    This routine loads a firmware update and creates the capsule header.

Arguments:

    FirmwareUpdateSettings - Supplies the update filename and firmware
        resource.

    OsFilePath - Supplies the file path to the Operating System directory.

    OsDeviceId - Supplies a handle to the device containing the Operating
        System.

    FirmwareUpdateDescriptor - Supplies a buffer that returns the firmware
        update descriptor as a heap allocated update pack for which the caller
        is responsible to free. 

Return Value:

    NT status code.

--*/

{

    PVOID CapsuleAllocationBase;
    ULONG CapsuleAllocationSize;
    ULONG CapsuleDataSize;
    ULONG CapsuleDescriptorSize;
    ULONG CapsuleHeaderSize;
    FILE_ID FileId;
    FILE_INFORMATION FileInformation;
    PVOID ImageBase;
    ULONG ImageFlags;
    ULONG ImageSize;
    UNICODE_STRING Path;
    BOOLEAN PeImage;
    ULONG PeImageSize;
    PHYSICAL_ADDRESS PhysicalAddress;
    BOOLEAN Result;
    NTSTATUS Status;
    PVOID TempImageBase;
    ULONG TempImageSize;
    PFIRMWARE_UPDATE_DESCRIPTOR UpdateDescriptor;

    *FirmwareUpdateDescriptor = NULL;

    //
    // Construct the full path to the firmware update file.
    //

    CapsuleAllocationBase = NULL;
    CapsuleAllocationSize = 0; // Satisfy compiler.
    PhysicalAddress.QuadPart = 0; // Satisfy compiler.
    UpdateDescriptor = NULL;
    RtlInitUnicodeString(&Path, NULL);
    Status = BlCopyUnicodeStringToUnicodeString(&Path, OsFilePath);
    if (!NT_SUCCESS(Status)) {
        goto LoadUpdateEnd;
    }

    Result = BlAppendUnicodeToString(&Path, L"Firmware\\");
    if (Result == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto LoadUpdateEnd;
    }

    Result = BlAppendUnicodeToString(&Path, FirmwareUpdateSettings->Filename);
    if (Result == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto LoadUpdateEnd;
    }

    //
    // Open the firmware update file and obtain its size.
    //

    Status = BlFileOpen(OsDeviceId, Path.Buffer, OPEN_READ, &FileId);
    if (!NT_SUCCESS(Status)) {
        goto LoadUpdateEnd;
    }

    Status = BlFileGetInformation(FileId, &FileInformation);
    BlFileClose(FileId);
    if (!NT_SUCCESS(Status)) {
        goto LoadUpdateEnd;
    }

    //
    // Allocate a buffer for the block descriptors, capsule header and data.
    //
    // N.B. Ensure the block descriptors (2), capsule header, and its data all
    //      begin on seperate page boundaries.
    //

    CapsuleDescriptorSize =
        ALIGN_RANGE_UP((2 * (sizeof(EFI_CAPSULE_BLOCK_DESCRIPTOR))),
                       PAGE_SIZE);

    CapsuleHeaderSize = ALIGN_RANGE_UP(sizeof(EFI_CAPSULE_HEADER), PAGE_SIZE);
    CapsuleDataSize = ALIGN_RANGE_UP(FileInformation.FileSize, PAGE_SIZE);
    if (CapsuleDataSize < FileInformation.FileSize) {
        Status = STATUS_INTEGER_OVERFLOW;
        goto LoadUpdateEnd;
    }

    Status = RtlULongAdd(CapsuleHeaderSize,
                         CapsuleDataSize,
                         &CapsuleAllocationSize);

    if (NT_SUCCESS(Status)) {
        Status = RtlULongAdd(CapsuleAllocationSize,
                             CapsuleDescriptorSize,
                             &CapsuleAllocationSize);
    }

    if (!NT_SUCCESS(Status)) {
        goto LoadUpdateEnd;
    }

    Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                       (CapsuleAllocationSize / PAGE_SIZE),
                                       OSL_MEMORY_TYPE_FIRMWARE_UPDATE,
                                       MEMORY_ATTRIBUTE_NONE,
                                       0);

    if (!NT_SUCCESS(Status)) {
        goto LoadUpdateEnd;
    }

    Status = BlMmMapPhysicalAddress(&CapsuleAllocationBase,
                                    PhysicalAddress,
                                    CapsuleAllocationSize,
                                    MEMORY_ATTRIBUTE_NONE);

    if (!NT_SUCCESS(Status)) {
        BlMmFreePhysicalPages(PhysicalAddress);
        goto LoadUpdateEnd;
    }

    //
    // Determine if the update payload image is a PE/COFF image (i.e. EFI
    // application). If the update payload is a PE/COFF image, the catalog for
    // the firmware update driver package contains the signature hash of the
    // PE image which excludes the security directory and various header
    // fields.
    //
    // The capsule containing the firmware update payload always contains the
    // flat-file. The integrity check for a flat file can occur when loading
    // the file into memory. For a PE/COFF image, the integrity check is
    // performed as a second (PE/COFF) image load.
    //

    ImageFlags = IMAGE_FLAGS_MEMORY_PRE_ALLOCATED;
    PeImage = FALSE;
    PeImageSize = 0;
    Status = BlImgGetPEImageSize(OsDeviceId, Path.Buffer, &PeImageSize);
    if (NT_SUCCESS(Status)) {
        PeImage = TRUE;

        ASSERT(PeImageSize != 0);

    } else {
        ImageFlags |= OSL_FIRMWARE_UPDATE_VALIDATION_FLAGS;
    }

    //
    // Load the firmware update in the pre-allocated, page-aligned buffer.
    //

    ImageBase = Add2Ptr(CapsuleAllocationBase,
                        (CapsuleDescriptorSize + CapsuleHeaderSize));

    ImageSize = (ULONG)FileInformation.FileSize;
    BlImgSetCatalogHint(FirmwareUpdateSettings->Catalog);
    Status = BlImgLoadImageEx(OsDeviceId,
                              OSL_MEMORY_TYPE_FIRMWARE_UPDATE,
                              Path.Buffer,
                              &ImageBase,
                              &ImageSize,
                              MEMORY_ATTRIBUTE_NONE,
                              0,
                              ImageFlags);

    if (!NT_SUCCESS(Status)) {
        if (Status == STATUS_INVALID_IMAGE_HASH) {
            OslpEfiLogStatus(&BOOT_FW_UPDATE_CODE_INTEGRITY_FAILURE,
                             &Path,
                             Status);

        } else {
            OslpEfiLogStatus(&BOOT_FW_UPDATE_IMAGE_LOAD_FAILURE,
                             &Path,
                             Status);
        }

        goto LoadUpdateEnd;
    }

    //
    // If the firmware update payload is a PE/COFF image, load it as a PE/COFF
    // image to perform the integrity check.
    //

    if (PeImage != FALSE) {
        TempImageBase = NULL;
        TempImageSize = 0;
        ImageFlags =
            IMAGE_FLAGS_NO_RELOCATIONS | OSL_FIRMWARE_UPDATE_VALIDATION_FLAGS;

        Status =
            BlImgLoadPEImageFromSourceBuffer(ImageBase,
                                             ImageSize,
                                             OSL_MEMORY_TYPE_FIRMWARE_UPDATE,
                                             Path.Buffer,
                                             &TempImageBase,
                                             &TempImageSize,
                                             NULL,
                                             NULL,
                                             NULL,
                                             MEMORY_ATTRIBUTE_NONE,
                                             0,
                                             ImageFlags,
                                             NULL,
                                             NULL);

        if (!NT_SUCCESS(Status)) {
            if (Status == STATUS_INVALID_IMAGE_HASH) {
                OslpEfiLogStatus(&BOOT_FW_UPDATE_CODE_INTEGRITY_FAILURE,
                                 &Path,
                                 Status);

            } else {
                OslpEfiLogStatus(&BOOT_FW_UPDATE_PE_IMAGE_LOAD_FAILURE,
                                 &Path,
                                 Status);
            }

            goto LoadUpdateEnd;
        }

        if (TempImageBase != NULL) {
            BlImgUnallocateImageBuffer(TempImageBase,
                                       TempImageSize,
                                       ImageFlags);
        }
    }

    //
    // Allocate and initialize a firmware update descriptor and fill in the
    // capsule header.
    //

    UpdateDescriptor = BlMmAllocateHeap(sizeof(FIRMWARE_UPDATE_DESCRIPTOR));
    if (UpdateDescriptor == NULL) {
        Status = STATUS_NO_MEMORY;
        goto LoadUpdateEnd;
    }

    UpdateDescriptor->FirmwareUpdateSettings = FirmwareUpdateSettings;
    UpdateDescriptor->CapsuleDescriptor = CapsuleAllocationBase;
    UpdateDescriptor->CapsuleAllocationSize = CapsuleAllocationSize;
    UpdateDescriptor->CapsuleDescriptorAddress = PhysicalAddress;
    UpdateDescriptor->UpdateCapsule = Add2Ptr(CapsuleAllocationBase,
                                              CapsuleDescriptorSize);

    UpdateDescriptor->UpdateCapsuleAddress.QuadPart =
        PhysicalAddress.QuadPart + CapsuleDescriptorSize;

    RtlZeroMemory(UpdateDescriptor->CapsuleDescriptor, CapsuleDescriptorSize);
    RtlZeroMemory(UpdateDescriptor->UpdateCapsule, CapsuleHeaderSize);
    RtlCopyMemory(&UpdateDescriptor->UpdateCapsule->CapsuleGuid,
                  &FirmwareUpdateSettings->Resource->FirmwareClass,
                  sizeof(EFI_GUID));

    UpdateDescriptor->UpdateCapsule->HeaderSize = CapsuleHeaderSize;
    UpdateDescriptor->UpdateCapsule->CapsuleImageSize = CapsuleHeaderSize +
        (ULONG)FileInformation.FileSize;

    //
    // All capsule updates set persist across reset and initiate reset flags.
    //

    UpdateDescriptor->UpdateCapsule->Flags =
        CAPSULE_FLAGS_PERSIST_ACROSS_RESET | CAPSULE_FLAGS_INITIATE_RESET;

    if (FirmwareUpdateSettings->Resource->FirmwareType ==
        FIRMWARE_RESOURCE_TYPE_DEVICE_FIRMWARE) {

        UpdateDescriptor->UpdateCapsule->Flags |=
            FirmwareUpdateSettings->Resource->CapsuleFlags &
            FIRMWARE_CAPSULE_FLAGS_DEVICE_MASK;

    } else {
        UpdateDescriptor->UpdateCapsule->Flags |=
            FirmwareUpdateSettings->Resource->CapsuleFlags &
            FIRMWARE_CAPSULE_FLAGS_MASK;
    }

    *FirmwareUpdateDescriptor = UpdateDescriptor;

LoadUpdateEnd:
    if (Path.Buffer != NULL) {
        BlMmFreeHeap(Path.Buffer);
    }

    if (UpdateDescriptor == NULL) {
        if (CapsuleAllocationBase != NULL) {
            BlMmUnmapVirtualAddress(CapsuleAllocationBase,
                                    CapsuleAllocationSize);

            BlMmFreePhysicalPages(PhysicalAddress);
        }
    }

    return Status;
}

NTSTATUS
OslpEfiReportUpdates (
    __in ULONG Count,
    __in PLIST_ENTRY UpdateSettings
    )

/*++

Routine Description:

    This routine persists firmware update results so that they are available
    in the kernel.

Arguments:

    Count - Supplies the number of firmware updates to report.

    UpdateSettings - Supplies the head of the update settings list.

Return Value:

    NT status code.

--*/

{

    GUID *FirmwareClass;
    PFIRMWARE_UPDATE_SETTINGS FirmwareUpdateSettings;
    GUID FwUpdateResultsGUID = FWUPDATE_RESULTS_GUID;
    PLIST_ENTRY NextEntry;
    GENERIC_BUFFER ResultsBuffer;
    PEFI_SYSTEM_RESOURCE_ENTRY Resource;
    PFWUPDATE_RESULTS_HEADER ResultsHeader;
    ULONG Size;
    NTSTATUS Status;
    PFIRMWARE_UPDATE_INFORMATION FirmwareUpdateInformation;
    ULONG UpdateCount;
    NTSTATUS UpdateStatus;

    if ((Count == 0) ||
        (UpdateSettings == NULL) ||
        (IsListEmpty(UpdateSettings) != FALSE)) {

        Status = STATUS_INVALID_PARAMETER;
        goto ReportUpdatesEnd;
    }

    if (IsListEmpty(UpdateSettings) != FALSE) {
        Status = STATUS_SUCCESS;
        goto ReportUpdatesEnd;
    }

    UpdateCount = 0;
    NextEntry = UpdateSettings->Flink;
    while (NextEntry != UpdateSettings) {
        FirmwareUpdateSettings = CONTAINING_RECORD(NextEntry,
                                                   FIRMWARE_UPDATE_SETTINGS,
                                                   Entry);

        UpdateCount += 1;
        NextEntry = NextEntry->Flink;
    }

    if (UpdateCount == 0) {
        Status = STATUS_SUCCESS;
        goto ReportUpdatesEnd;
    }

    Size = sizeof(FWUPDATE_RESULTS_HEADER);
    Size += (UpdateCount - 1) * sizeof(FIRMWARE_UPDATE_INFORMATION);
    ResultsHeader = BlMmAllocateHeap(Size);
    if (ResultsHeader == NULL) {
        Status = STATUS_NO_MEMORY;
        goto ReportUpdatesEnd;
    }

    //
    // Copy the results into the allocated buffer.
    //

    ResultsHeader->Results = UpdateCount;
    NextEntry = UpdateSettings->Flink;
    FirmwareUpdateInformation = &ResultsHeader->FirmwareUpdateInformation[0];
    while (NextEntry != UpdateSettings) {
        FirmwareUpdateSettings = CONTAINING_RECORD(NextEntry,
                                                   FIRMWARE_UPDATE_SETTINGS,
                                                   Entry);

        //
        // Ensure valid results.
        //

        FirmwareClass =
            (GUID *)&FirmwareUpdateSettings->Resource->FirmwareClass;

        Resource = FirmwareUpdateSettings->Resource;
        if (Resource->FirmwareUpdateStatus.LastAttemptVersion !=
            FirmwareUpdateSettings->Version) {

            //
            // These settings don't apply to this update version, assume they
            // are stale.
            //

            if ((Resource->FirmwareVersion ==
                 FirmwareUpdateSettings->Version) ||
                ((FirmwareUpdateSettings->Policy ==
                  FIRMWARE_UPDATE_POLICY_DEFAULT) &&
                 (Resource->FirmwareVersion >
                  FirmwareUpdateSettings->Version))) {

                UpdateStatus = STATUS_SUCCESS;

            } else {

                //
                // The firmware may not have gotten far enough to determine
                // the firmware update target version, but may have recorded a
                // valid failure status. In the case of the latter, prefer this
                // status, otherwise set a generic failure code.
                //

                if (Resource->FirmwareUpdateStatus.LastAttemptStatus !=
                    FIRMWARE_UPDATE_STATUS_SUCCESS) {

                    UpdateStatus = OslpEfiConvertFwUpdateStatus(
                        Resource->FirmwareUpdateStatus.LastAttemptStatus);

                } else if (!NT_SUCCESS(FirmwareUpdateSettings->LoadStatus)) {
                    UpdateStatus = FirmwareUpdateSettings->LoadStatus;

                } else {
                    UpdateStatus = STATUS_UNKNOWN_REVISION;
                }
            }

            OslpEfiSetUpdateStatus(FirmwareClass,
                                   UpdateStatus,
                                   FirmwareUpdateSettings->Version,
                                   FirmwareUpdateInformation);

        } else if (((Resource->FirmwareVersion ==
                     FirmwareUpdateSettings->Version) ||
                    ((FirmwareUpdateSettings->Policy ==
                      FIRMWARE_UPDATE_POLICY_DEFAULT) &&
                     (Resource->FirmwareVersion >
                      FirmwareUpdateSettings->Version))) &&
                   (Resource->FirmwareUpdateStatus.LastAttemptStatus !=
                    FIRMWARE_UPDATE_STATUS_SUCCESS)) {

            //
            // The resource update status indicates failure whereas the
            // resources version indicates the update was successful.
            // Assume the latter is true.
            //

            OslpEfiSetUpdateStatus(FirmwareClass,
                                   STATUS_SUCCESS,
                                   FirmwareUpdateSettings->Version,
                                   FirmwareUpdateInformation);

        } else if ((Resource->FirmwareVersion !=
                    FirmwareUpdateSettings->Version) &&
                   (Resource->FirmwareUpdateStatus.LastAttemptStatus ==
                    FIRMWARE_UPDATE_STATUS_SUCCESS)) {

            //
            // The resource version doesn't match the update, but its
            // last attempt status indicates success.  Assume the update
            // failed unless it was not even attempted.
            //

            if ((FirmwareUpdateSettings->Policy ==
                 FIRMWARE_UPDATE_POLICY_DEFAULT) &&
                (Resource->FirmwareVersion >
                 FirmwareUpdateSettings->Version)) {
                UpdateStatus = STATUS_SUCCESS;

            } else if (!NT_SUCCESS(FirmwareUpdateSettings->LoadStatus)) {
                UpdateStatus = FirmwareUpdateSettings->LoadStatus;

            } else {
                UpdateStatus = STATUS_NOT_COMMITTED;
            }
                        
            OslpEfiSetUpdateStatus(FirmwareClass,
                                   UpdateStatus,
                                   FirmwareUpdateSettings->Version,
                                   FirmwareUpdateInformation);

        } else {

            //
            // The resource's update status is valid, use it.
            //

            UpdateStatus = OslpEfiConvertFwUpdateStatus(
                Resource->FirmwareUpdateStatus.LastAttemptStatus);

            OslpEfiSetUpdateStatus(
                FirmwareClass,
                UpdateStatus,
                Resource->FirmwareUpdateStatus.LastAttemptVersion,
                FirmwareUpdateInformation);
        }

        NextEntry = NextEntry->Flink;
        FirmwareUpdateInformation =
            Add2Ptr(FirmwareUpdateInformation,
                    sizeof(FIRMWARE_UPDATE_INFORMATION));
    }

    //
    // Persist the results.
    //

    BlInitGenericBuffer(&ResultsBuffer, ResultsHeader, Size);
    Status = BlPdSaveData(NULL, &FwUpdateResultsGUID, &ResultsBuffer);
    BlMmFreeHeap(ResultsHeader);

ReportUpdatesEnd:
    return Status;
}

VOID
OslpEfiSetUpdateStatus (
    __in GUID *FirmwareClass,
    __in NTSTATUS LastAttemptStatus,
    __in UINT32 LastAttemptVersion,
    __out PFIRMWARE_UPDATE_INFORMATION FirmwareUpdateInformation
    )

/*++

Routine Description:

    This routine creates a firmware update status using the supplied values
    and today's date.

Arguments:

    FirmwareClass - Supplies the ID of a firmware update resource.

    LastAttemptStatus - Supplies the last attempt status to set.

    LastAttemptVersion - Supplies the last attempt version to set.

    FirmwareUpdateInformation - Supplies a buffer that returns the firmware
        update information.

Return Value:

    None.

--*/

{

    if (FirmwareClass != NULL) {
        RtlCopyMemory(&FirmwareUpdateInformation->FirmwareClass,
                      FirmwareClass,
                      sizeof(GUID));
    }

    //
    // Convert the firmware update status to NT status. Kernel boot application
    // persistent data support will record this value in the registry as the
    // final result of the firmware update operation.
    //

    FirmwareUpdateInformation->LastAttemptVersion = (ULONG)LastAttemptVersion;
    FirmwareUpdateInformation->LastAttemptStatus = LastAttemptStatus;

    return;
}

