/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    imgapp.c

Abstract:

    This module implements the image APIs for loading and executing Boot
    Environment Applications.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "image.h"

// -------------------------------------------------------------------- Globals

ULONG IapAllocatedTableEntries = 0;
ULONG IapTableEntries = 0;
PLOADED_IMAGE *IapImageTable = NULL;

#define INITIAL_IMAGE_TABLE_SIZE (2)

//
// Bitmask of which boot apps must have their file name verified against their
// version resource.
//

const ULONG IapVerifyFileNameOfTheseApps = BOOT_ENTRY_HV_LOADER;

// ----------------------------------------------------- Image Application APIs

NTSTATUS
BlImgLoadBootApplication (
    __in_opt PBOOT_APPLICATION_ENTRY ApplicationEntry,
    __in_opt PBOOT_ENVIRONMENT_DEVICE OverrideDevice,
    __in_opt PWSTR OverridePath,
    __out PULONG ImageHandle
    )

/*++

Routine Description:

    Attempts to load a boot environment application into memory, registering
    the loaded application with image loader.

Arguments:

    ApplicationEntry - Supplies the identifier for the application and its
        options, including the device and path of the application binary. If
        this parameter is NULL, the current boot application's entry is used,
        i.e. the new boot application inherits the current application's
        boot options.

    OverrideDevice - Supplies the device to load the boot application. If this
        parameter is not NULL, it overrides the application device setting from
        ApplicationEntry.

    OverridePath - Supplies the path to load the boot application. If this
        parameter is not NULL, it overrides the application path setting from
        ApplicationEntry.

    ImageHandle - On output, contains the handle to use for all subsequent
        references to the loaded image.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if ImageHandle is NULL.

    STATUS_INVALID_PARAMETER if a BootEntry is not specified or if Device and
        ApplicationFilePath are not specified.

    STATUS_NO_MEMORY if memory can not be allocated for an internal data
        structure.

--*/

{

    BOOLEAN AppendFrequency;
    PBOOT_ENVIRONMENT_DEVICE ApplicationDevice;
    DEVICE_ID ApplicationDeviceId;
    PWSTR ApplicationPath;
    ULONG EntrySize;
    NOTIFICATION_EVENT_CONTEXT EventContext;
    LARGE_INTEGER Frequency;
    PBOOT_ENVIRONMENT_DEVICE FveDevice;
    UCHAR HashBuffer[MAX_HASH_LEN];
    PVOID ImageBase;
    PLOADED_IMAGE ImageEntry;
    PBOOT_APPLICATION_TRANSITION_ENTRY ImageApplicationEntry;
    PUCHAR ImageHash;
    ULONG ImageFlags;
    UNICODE_STRING ImageName;
    ULONG ImageSize;
    ULONG LoadInformation;
    ULONG OptionSize;
    NTSTATUS Status;
    PUCHAR TrustedHash;

    BlpInterfaceEnter(InterfaceImg);

    //
    // Initialize local data.
    //

    ApplicationDeviceId = INVALID_DEVICE_ID;
    ApplicationDevice = NULL;
    ApplicationPath = NULL;
    FveDevice = NULL;
    ImageEntry = NULL;
    ImageApplicationEntry = NULL;
    ImageBase = NULL;
    ImageFlags = 0;
    ImageSize = 0;
    ImageHash = HashBuffer;
    LoadInformation = 0;
    TrustedHash = NULL;

    //
    // If ApplicationEntry is NULL, use the current boot application's
    // application entry.
    //

    if (ARGUMENT_PRESENT(ApplicationEntry) == FALSE) {
        ApplicationEntry = BlGetApplicationEntry();
    }

    //
    // As an optimization, pass the boot library performance counter frequency
    // to any newly loaded application in its options such that it does not
    // need to be recalibrated. Only do this if the application can accept this
    // in-memory BCD option.
    //

    AppendFrequency = BlInMemorySettingAllowed(
                          ApplicationEntry,
                          BCDE_LIBRARY_TYPE_PERFORMANCE_FREQUENCY);

    if (AppendFrequency != FALSE) {
        BlTimeQueryPerformanceCounter(&Frequency);
        (VOID)BlAppendBootOptionInteger(ApplicationEntry,
                                        BCDE_LIBRARY_TYPE_PERFORMANCE_FREQUENCY,
                                        Frequency.QuadPart);
    }

    //
    // Events to allow any pre-processing to occur.  In this case to unlock the
    // FVE device to allow boot to occur.
    //

    Status = BlFveSecureBootUnlockBootDevice(ApplicationEntry,
                                             &FveDevice,
                                             &TrustedHash);

    if (!NT_SUCCESS(Status)) {
        goto BlImgLoadBootApplicationEnd;
    }

    //
    // The options list contains the application device and path. Extract these
    // from the list. If an override device or path is supplied, use the
    // override one instead.
    //

    if (ARGUMENT_PRESENT(OverrideDevice) == FALSE) {
        Status = BlGetBootOptionDevice(ApplicationEntry->Options,
                                       BCDE_LIBRARY_TYPE_APPLICATION_DEVICE,
                                       &ApplicationDevice,
                                       NULL);

        if (!NT_SUCCESS(Status)) {
            goto BlImgLoadBootApplicationEnd;
        }

    } else {
        ApplicationDevice = OverrideDevice;
        __analysis_assume(ApplicationDevice->Size == OverrideDevice->Size);
    }

    if (ARGUMENT_PRESENT(OverridePath) == FALSE) {
        Status = BlGetBootOptionString(ApplicationEntry->Options,
                                       BCDE_LIBRARY_TYPE_APPLICATION_PATH,
                                       &ApplicationPath);

        if (!NT_SUCCESS(Status)) {
            goto BlImgLoadBootApplicationEnd;
        }

    } else {
        ApplicationPath = OverridePath;
    }

    //
    // Open the device for the boot application.
    //

    Status = BlDeviceOpen(ApplicationDevice,
                          OPEN_READ,
                          &ApplicationDeviceId);

    if (!NT_SUCCESS(Status)) {
        goto BlImgLoadBootApplicationEnd;
    }

    //
    // Finalize SecureBoot policy now that the OSVolume is available
    //

    Status = BlSecureBootFinalizeSecureBootAfterOSVolumeAvailable(ApplicationDeviceId);
    if (!NT_SUCCESS(Status)) {
        goto BlImgLoadBootApplicationEnd;
    }

    //
    //
    // Determine the image code integrity flags, if any, that should be used
    // when loading this boot application into memory.
    //

    Status = ImgpComputeCodeIntegrityFlags(ApplicationEntry, &ImageFlags);
    if (!NT_SUCCESS(Status)) {
        goto BlImgLoadBootApplicationEnd;
    }

    //
    // Verify the file name matches in the version resource when required.
    //

    if (CHECK_FLAG(ApplicationEntry->Attributes,
                   IapVerifyFileNameOfTheseApps) != FALSE) {

        SET_FLAGS(ImageFlags, IMAGE_FLAGS_VERIFY_FILENAME);
    }

    //
    // Notify boot application launch is starting to all registered components.
    //

    RtlZeroMemory(&EventContext, sizeof(NOTIFICATION_EVENT_CONTEXT));
    EventContext.u.LaunchStart.ApplicationEntry = ApplicationEntry;
    BlEnNotifyEvent(EVENT_APPLICATION_LAUNCH_STARTING, &EventContext);


    //
    // Load the boot application into memory.
    //

    Status = ImgFwLoadBootApplication(ApplicationDeviceId,
                                      ApplicationPath,
                                      ApplicationEntry->Attributes,
                                      &ImageBase,
                                      &ImageSize,
                                      ImageHash,
                                      ImageFlags,
                                      &LoadInformation);

    if (!NT_SUCCESS(Status)) {
        goto BlImgLoadBootApplicationEnd;
    }

    //
    // If debugging the transition out of this application, load the symbols
    // for the next application now.
    //

    if (BlBdDebugTransitionsEnabled() != FALSE) {
        ApplicationEntry->Attributes |= BOOT_ENTRY_DEBUG_TRANSITION;
        RtlInitUnicodeString(&ImageName, ApplicationPath);
        BlBdLoadImageSymbols(&ImageName, ImageBase);
    }

    //
    // Check for new SecureBoot policy on the BitLocker OS volume for the app.
    // This needs to be done before the call to
    // BlFveSecureBootCheckpointBootApp, since that will clear secrets from the
    // device state which are used to detect whether BitLocker is bound to
    // SecureBoot. If the device contains a newer Secure Boot policy, it will
    // be made active.
    //

    Status = BlSecureBootCheckPolicyOnFveDevice(FveDevice);
    if (!NT_SUCCESS(Status)) {
        goto BlImgLoadBootApplicationEnd;
    }

    //
    // The specified boot application was successfully loaded into memory.
    // If the application is on a volume encrypted by FVE, ensure that
    // the application can be accessed, and that there is a key-ring in memory
    // and referenced in the options that will be passed to the application.
    // If the system is using a TPM for security measures, access control to
    // unlocked volumes occurs at this point to ensure that the loaded
    // application can be executed without compromising the security of all of
    // the code-bound volumes.
    // The ApplicationEntry->Options list may be modified at this point, so
    // this function call must occur before the options are copied for this
    // image.  Also validate hash of the application.
    //

    Status = BlFveSecureBootCheckpointBootApp(ApplicationEntry,
                                              FveDevice,
                                              ImageHash,
                                              TrustedHash);

    if (!NT_SUCCESS(Status)) {
        goto BlImgLoadBootApplicationEnd;
    }

#if defined(_ARM_) || defined(_ARM64_)

    //
    // Before building the image entry, append a dummy BCD option to the
    // application parameters for the self map base. This needs to be done here
    // because memory cannot be allocated for the child after the page tables
    // are created, which is when the real dynamic self map base is determined.
    //

    BlRemoveBootOption(ApplicationEntry->Options,
                       BCDE_LIBRARY_TYPE_SELF_MAP_PDE_BASE);

    BlAppendBootOptionInteger(ApplicationEntry,
                              BCDE_LIBRARY_TYPE_SELF_MAP_PDE_BASE,
#if defined(_ARM_)
                              (ULONGLONG)PDE_BASE
#else
                              (ULONGLONG)PXE_BASE
#endif
                              );

#endif

    //
    // Create an entry for the application in the loaded image table.  The
    // index in the table will be used for all subsequent image requests
    // (unload, start).
    //

    OptionSize = BlGetBootOptionListSize(ApplicationEntry->Options);
    EntrySize = FIELD_OFFSET(BOOT_APPLICATION_TRANSITION_ENTRY, InlineOptions) +
                OptionSize;

    ImageApplicationEntry = BlMmAllocateHeap(EntrySize);
    if (ImageApplicationEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BlImgLoadBootApplicationEnd;
    }

    RtlZeroMemory(ImageApplicationEntry, EntrySize);

    //
    // Copy the application entry to the transition entry, which has its options
    // inline in the entry structure, rather than via an indirect pointer.  It
    // also contains a signature for validation by the application being
    // launched.
    //

    strcpy_s(ImageApplicationEntry->Signature,
             sizeof(ImageApplicationEntry->Signature),
             BOOT_ENTRY_SIGNATURE);

    ImageApplicationEntry->Attributes = ApplicationEntry->Attributes;
    RtlCopyMemory(&ImageApplicationEntry->Identifier,
                  &ApplicationEntry->Identifier,
                  sizeof(GUID));

    RtlCopyMemory(&ImageApplicationEntry->InlineOptions,
                  ApplicationEntry->Options,
                  OptionSize);

    //
    // Allocate memory for the image entry.
    //

    ImageEntry = BlMmAllocateHeap(sizeof(LOADED_IMAGE));
    if (ImageEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BlImgLoadBootApplicationEnd;
    }

    ImageEntry->ImageBase = ImageBase;
    ImageEntry->ImageSize = ImageSize;
    ImageEntry->ApplicationEntry = ImageApplicationEntry;
    ImageEntry->LoadInformation = LoadInformation;

    //
    // Allocate the global loaded image table if it is not currently
    // allocated.
    //

    if (IapTableEntries == 0) {
        IapAllocatedTableEntries = 0;
        IapTableEntries = INITIAL_IMAGE_TABLE_SIZE;
        IapImageTable = BlMmAllocateHeap(IapTableEntries *
                                         sizeof(PLOADED_IMAGE));

        if (IapImageTable == NULL) {
            Status = STATUS_NO_MEMORY;
            goto BlImgLoadBootApplicationEnd;
        }

        RtlZeroMemory(IapImageTable, IapTableEntries * sizeof(PLOADED_IMAGE));
    }

    //
    // Insert the entry into the Image Table.
    //

    Status = BlTblSetEntry(&IapImageTable,
                           &IapTableEntries,
                           ImageEntry,
                           ImageHandle,
                           TblDoNotPurgeEntry);

    if (!NT_SUCCESS(Status)) {
        goto BlImgLoadBootApplicationEnd;
    }

    //
    // Image has successfully been loaded.
    //

    ;

BlImgLoadBootApplicationEnd:
    if (ApplicationDeviceId != INVALID_DEVICE_ID) {
        BlDeviceClose(ApplicationDeviceId);
    }

    //
    // Only free ApplicationDevice and ApplicationPath if the override ones are
    // NULL.
    //

    if ((ApplicationDevice != NULL) && (OverrideDevice == NULL)) {
        BlMmFreeHeap(ApplicationDevice);
    }

    if ((ApplicationPath != NULL) && (OverridePath == NULL)) {
        BlMmFreeHeap(ApplicationPath);
    }

    if (FveDevice != NULL) {
        BlMmFreeHeap(FveDevice);
    }

    if (TrustedHash != NULL) {
        BlMmFreeHeap(TrustedHash);
    }

    //
    // Function cleanup.
    //

    if (!NT_SUCCESS(Status)) {

        //
        // If a failure occurred after the image was loaded, use the firmware
        // to unload the application.
        //

        if (ImageBase != NULL) {
            ImgFwUnloadBootApplication(ImageBase, ImageSize);
        }

        //
        // Free any allocations for internal data structures.
        //

        if (ImageApplicationEntry != NULL) {
            BlMmFreeHeap(ImageApplicationEntry);
        }

        if (ImageEntry != NULL) {
            BlMmFreeHeap(ImageEntry);
        }

        //
        // If the Image Table was allocated and we were unsuccessful in
        // creating an entry, then free the table's allocation to avoid
        // getting out of sync.
        //

        if ((IapAllocatedTableEntries == 0) && (IapImageTable != NULL)) {
            BlMmFreeHeap(IapImageTable);
            IapTableEntries = 0;
            IapImageTable = NULL;
        }

    } else {

        //
        // Successful in attempt to load the image.
        //

        IapAllocatedTableEntries += 1;
    }

    BlpInterfaceExit(InterfaceImg);
    return Status;
}

NTSTATUS
BlImgUnloadBootApplication (
    __in ULONG ImageHandle
    )

/*++

Routine Description:

    Unloads a previously loaded image.  If the image was loaded into memory
    by BlImgLoadBootApplication, the memory allocated for the image is freed.
    The ImageHandle becomes invalid.

Arguments:

    ImageHandle - The image reference returned by BlImgLoadBootApplication.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if ImageHandle is invalid.

    STATUS_MEMORY_NOT_ALLOCATED if there as an error freeing the memory
        allocated for the image.

--*/

{

    PLOADED_IMAGE ImageEntry;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceImg);

    if ((IapTableEntries <= ImageHandle) ||
        (IapImageTable[ImageHandle] == NULL)) {

        Status = STATUS_INVALID_PARAMETER;

    } else {

        //
        // Unload the application via the underlying firmware.
        //

        ImageEntry = IapImageTable[ImageHandle];
        Status = ImgFwUnloadBootApplication(ImageEntry->ImageBase,
                                            ImageEntry->ImageSize);

        //
        // Return the status of ImgFwUnloadBootApplication to the caller.  But
        // first, free all memory allocated for the loaded image.
        //

        //
        // Free all memory allocated by BlImgLoadBootApplication.
        //

        BlMmFreeHeap(ImageEntry->ApplicationEntry);
        BlMmFreeHeap(ImageEntry);

        //
        // Clear the entry in the Image Table.
        //

        IapImageTable[ImageHandle] = NULL;

        //
        // Update the global count of allocated table entries.  If there are no
        // entries left in the table, free up the table.
        //

        IapAllocatedTableEntries -= 1;
        if (IapAllocatedTableEntries == 0) {
            BlMmFreeHeap(IapImageTable);
            IapImageTable = NULL;
            IapTableEntries = 0;
        }
    }

    BlpInterfaceExit(InterfaceImg);
    return Status;
}

NTSTATUS
BlImgStartBootApplication (
    __in ULONG ImageHandle,
    __in ULONG StartFlags,
    __out_opt PBOOT_APPLICATION_RETURN_ARGUMENTS ReturnArgs
    )

/*++

Routine Description:

    Transfers execution to the loaded image's entry point.

Arguments:

    ImageHandle - The image reference returned by BlImgLoadBootApplication.

    StartFlags - Supplies start flags. Possible values include:

        IMAGE_FLAGS_ISOLATED_EXECUTION_CONTEXT - Specifies that the child
            application should run completely in its own execution context.

    ReturnArgs - Optional buffer for the application's return arguments.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if ImageHandle is invalid.

    STATUS_CONFLICTING_ADDRESSES if memory map conflicts cannot be resolved.

    STATUS_INSUFFICIENT_RESOURCES if an internal resource allocation failed.

    STATUS_NO_MEMORY if an internal allocation failed.

--*/

{

    PLOADED_IMAGE ImageEntry;
    BOOT_APPLICATION_RETURN_ARGUMENTS BaReturnArgBuffer;
    PBOOT_APPLICATION_RETURN_ARGUMENTS BaReturnArgs;
    NOTIFICATION_EVENT_CONTEXT AppLaunchContext;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceImg);

    //
    // If the caller didn't supply a return arguments buffer, then we'll
    // supply our own.
    //

    BaReturnArgs = ReturnArgs;
    if (BaReturnArgs == NULL) {
        BaReturnArgs = &BaReturnArgBuffer;
        BaReturnArgBuffer.Version = BOOT_APPLICATION_RETURN_ARGUMENTS_VERSION;
        BaReturnArgBuffer.ReturnStatus = STATUS_SUCCESS;
        BaReturnArgBuffer.Flags = 0;
        RtlZeroMemory(&BaReturnArgBuffer.PersistentData,
                      sizeof(BaReturnArgBuffer.PersistentData));
    }

    //
    // Ensure that ImageHandle is valid.
    //

    if ((IapTableEntries <= ImageHandle) ||
        (IapImageTable[ImageHandle] == NULL)) {

        Status = STATUS_INVALID_PARAMETER;

    } else {

        ImageEntry = IapImageTable[ImageHandle];

        //
        // Raise an event to notify that the application is launching
        // and provide information about the application.
        //

        RtlZeroMemory(&AppLaunchContext, sizeof(NOTIFICATION_EVENT_CONTEXT));

        if ((CHECK_FLAG(ImageEntry->LoadInformation, 
                        LOAD_INFORMATION_EMBEDDED_SIGNED) != FALSE) ||
            (CHECK_FLAG(ImageEntry->LoadInformation,
                        LOAD_INFORMATION_CATALOG_SIGNED) != FALSE)) {

            AppLaunchContext.u.AppLaunching.ImageSigned = TRUE;
        }


        if (CHECK_FLAG(ImageEntry->LoadInformation, 
                       LOAD_INFORMATION_POLICY_VIOLATION) != FALSE) {

            AppLaunchContext.u.AppLaunching.ImageViolatedPolicy = TRUE;

            if (CHECK_FLAG(ImageEntry->LoadInformation, 
                           LOAD_INFORMATION_POLICY_VIOLATION_AUDIT) != FALSE) {

                AppLaunchContext.u.AppLaunching.ImagePolicyViolationAllowedByAuditMode = TRUE;
            }
        }

        if (CHECK_FLAG(ImageEntry->LoadInformation,
                       LOAD_INFORMATION_IMAGE_BOOT_SCENARIO_AWARE) != FALSE) {

            AppLaunchContext.u.AppLaunching.SealingScenarioAware = TRUE;
        }

        if (CHECK_FLAG(ImageEntry->LoadInformation,
                       LOAD_INFORMATION_IMAGE_BOOT_SCENARIO_AGNOSTIC) != FALSE) {

            AppLaunchContext.u.AppLaunching.SealingScenarioAgnostic = TRUE;
        }

        //
        // Notify the application launch event to all registered components.
        //

        BlEnNotifyEvent(EVENT_APPLICATION_LAUNCHING, &AppLaunchContext);

        //
        // Before starting a new application, stop the logging to persist the
        // data.
        //

        BlLogDestroy();

        //
        // Purge all file caches before the next application to prevent data
        // leak.
        //

        BlFilePurgeAllCaches();

        //
        // Purge all device caches to maximize memory resources available to
        // the next application.
        //

        BlDevicePurgeAllCaches();

        //
        // Make sure instruction and data caches are in sync.
        //

        BlArchSweepIcacheRange(ImageEntry->ImageBase, ImageEntry->ImageSize);

        //
        // Transfer execution to the boot application.  This steps to do so
        // will vary based on image type and firmware.
        //

        Status = ImgFwStartBootApplication(ImageEntry->ApplicationEntry,
                                           ImageEntry->ImageBase,
                                           ImageEntry->ImageSize,
                                           StartFlags,
                                           BaReturnArgs);

        //
        // Restore any application persistent data.
        //

        BlpPdParseReturnArguments(BaReturnArgs);

        //
        // Reload BitLocker cached keys.
        //

        BlpFveReloadKeys();

        //
        // Notify the application return event to all registered components.
        //

        BlEnNotifyEvent(EVENT_APPLICATION_RETURN, NULL);

        //
        // Restore any changes made to the video mode.
        //

        BlpDisplayReinitialize();

        //
        // Restart the logging. This is not critical for the functioning of the
        // boot manager, hence ignore the status.
        //

        BlpLogInitialize();
   }

    BlpInterfaceExit(InterfaceImg);
    return Status;
}

// --------------------------------------------------------- Internal Functions

NTSTATUS
ImgpCopyApplicationBootDevice (
    __out PBOOT_ENVIRONMENT_DEVICE Target,
    __in PBOOT_ENVIRONMENT_DEVICE Source
    )

/*++

Routine Description:

    This function populates the target device identifier so that it is both
    functionally equivalent to the source device identifier and suitable for
    use in an application transition structure.

Arguments:

    Target - Supplies the device identifier to populate.  The number of bytes
        written to this buffer is at most the size of the source buffer.

    Source - Supplies the boot device to copy into the target device
        identifier.

Return Value:

    STATUS_SUCCESS is returned if the target device identifier is populated
        successfully.

    STATUS_DEVICE_DOES_NOT_EXIST is returned if an attempt to open the source
        device fails.

--*/

{

    PARTITION_IDENTIFIER DeprecatedPartition;
    DEVICE_ID DeviceId;
    DEVICE_INFORMATION Information;
    ULONG_PTR LeadingSize;
    ULONG_PTR ParentSize;
    PPARTITION_IDENTIFIER_EX Partition;
    ULONG_PTR PartitionDataSize;
    PARTITION_FORMAT PartitionFormat;
    PDEVICE_PARTITION_INFORMATION PartitionInformation;
    NTSTATUS Status;

    //
    // Generally the source device can be directly copied to the target.
    // Partition devices must be converted to the deprecated format since
    // this is the only partition device format supported by downlevel
    // applications.  All of this code can be removed when the old partition
    // device format is fully deprecated and applications supporting only the
    // old format are no longer supported.
    //

    if (Source->DeviceType != DEVICE_PARTITION_EX) {
        RtlCopyMemory(Target, Source, Source->Size);

    } else {

        //
        // N.B. Zeroing the deprecated partition identifier is not
        //      functionally required.  The identifier is zeroed to clear
        //      slack space at the end of unions where only the first few
        //      bytes are relevant for some partition types.  This is
        //      consistent with the behavior of other code that generates
        //      device identifiers.
        //

        PartitionDataSize = FIELD_OFFSET(PARTITION_IDENTIFIER,
                                         ParentIdentifier);

        RtlZeroMemory(&DeprecatedPartition, PartitionDataSize);
        Partition = &Source->u.PartitionEx;
        PartitionFormat = Partition->ParentIdentifier.u.HardDisk.PartitionType;
        if (PartitionFormat == MBR) {

            //
            // The deprecated partition format used partition numbers rather
            // than partition offsets to identify MBR partitions.  Open the
            // given boot device and extract the partition number from its
            // device information structure.
            //
            // N.B. It is expected that the boot device can always be opened
            //      successfully.  If the open fails, the source identifier
            //      could possibly be copied to the target identifier and
            //      allow the boot application to start if it supports the new
            //      partition format.  This is not done because failure to
            //      open the boot device is indicative of a serious problem
            //      that should be signaled here rather than having it
            //      manifest in less obvious ways (e.g., a downlevel
            //      application is started and fails because it does not
            //      support the new partition format).
            //

            Status = BlDeviceOpen(Source, OPEN_READ, &DeviceId);
            if (!NT_SUCCESS(Status)) {
                return STATUS_DEVICE_DOES_NOT_EXIST;
            }

            Status = BlDeviceGetInformation(DeviceId, &Information);
            BlDeviceClose(DeviceId);

            ASSERT(Status == STATUS_SUCCESS);

            PartitionInformation =
                &Information.u.BlockIo.DeviceInformation.Partition;

            DeprecatedPartition.Mbr.PartitionNumber =
                PartitionInformation->Mbr.Number;

        } else if (PartitionFormat == GPT) {
            RtlCopyMemory(&DeprecatedPartition.Gpt.PartitionSignature,
                          &Partition->Gpt.PartitionSignature,
                          sizeof(GUID));

        } else {
            DeprecatedPartition.ElTorito.BootEntry =
                Partition->ElTorito.BootEntry;
        }

        //
        // Compute the size of the target partition device as the sum of the
        // sizes of the leading fields of the boot environment device
        // structure, the partition identifier fields computed above, and the
        // parent block I/O identifier from the source device.
        //
        // N.B. This relies on the partition identification fields populated
        //      above being the only ones that precede the parent identifier
        //      in the partition identifier structure.  Uninitialized fields
        //      would be copied were this not the case.
        //

        ParentSize = Source->Size -
                     FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE,
                                  u.PartitionEx.ParentIdentifier);

        LeadingSize = FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u.Partition);
        RtlZeroMemory(Target, LeadingSize);
        Target->DeviceType = DEVICE_PARTITION;
        Target->Size = (ULONG)(LeadingSize + PartitionDataSize + ParentSize);
        Target->Flags = Source->Flags;

        //
        // Copy the deprecated partition identification information computed
        // above and complete the device identifier by appending the parent
        // block I/O identifier from the source device.
        //

        RtlCopyMemory(&Target->u.Partition,
                      &DeprecatedPartition,
                      PartitionDataSize);

        RtlCopyMemory(&Target->u.Partition.ParentIdentifier,
                      &Source->u.PartitionEx.ParentIdentifier,
                      ParentSize);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
ImgpInitializeBootApplicationParameters (
    __inout PGENERIC_BUFFER EntryParameters,
    __in PBOOT_APPLICATION_TRANSITION_ENTRY ApplicationEntry,
    __in_bcount(ImageSize) PVOID ImageBase,
    __in ULONG ImageSize,
    __in BOOLEAN AllocateBuffer
    )

/*++

Routine Description:

    Initialize the input parameters for a boot application.

    N.B. All contents of the application parameter structure are assumed to be
         unaligned by all consumers.  As a result, this routine does not need
         to insert any explicit padding into the parameter structure.

Arguments:

    EntryParameters - Pointer to a generic buffer structure.  On input,
        BufferSize refers to the size of the buffer.  If not-zero, then
        Buffer is assumed to be valid.  On output, Buffer points to the
        boot application parameter structure initialized by this routine.
        DataSize refers to the size of the structure and BufferSize refers
        to the size of the buffer.  If AllocateBuffer does not equal TRUE,
        neither of these values will change.

    ApplicationEntry - Supplies information about the boot application,
        including an identifier for it and its options.

    ImageBase - Supplies a pointer that is the base address of the boot
        application.

    ImageSize - Supplies the size (in bytes) of the boot application.

    AllocateBuffer - Boolean value indicating whether a new buffer should
        be allocated if the one referenced by EntryParameters is insufficient
        for the entire parameter structure.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the image is not a PE/COFF image.

    STATUS_NO_MEMORY if the memory map could not by copied to the input
        parameter structure.

    STATUS_BUFFER_TOO_SMALL if the buffer pointed to by EntryParameters is
        too small to copy the resultant parameter structure.

--*/

{

    ULONG AlignedBufferOffset;
    LIST_ENTRY AllocatedMDL;
    ULONG AllocationFlags;
    ULONG ApplicationEntrySize;
    ULONG ArraySlotSize;
    PALLOCATED_MEMORY BaAllocatedMemory;
    PBOOT_APPLICATION_TRANSITION_ENTRY BaApplicationEntry;
    PBOOT_ENVIRONMENT_DEVICE BaBootDevice;
    PFIRMWARE_PARAMETERS BaFirmwareParameters;
    PBOOT_APPLICATION_PARAMETERS BaParameters;
    PBOOT_APPLICATION_RETURN_ARGUMENTS BaReturnArgs;
    PBOOT_ENVIRONMENT_DEVICE BootDevice;
    BOOT_ENTRY UNALIGNED *ConvertedEntry;
    GENERIC_BUFFER DescriptorBuffer;
    PIMAGE_NT_HEADERS NtHeader;
    ULONG RequiredSize;
    ULONG Size;
    NTSTATUS Status;

    ConvertedEntry = NULL;

    //
    // Get a pointer to the start of the NT headers.
    //

    Status = BlImgGetNtHeader(ImageBase, ImageSize, &NtHeader);
    if (!NT_SUCCESS(Status)) {
        goto InitializeBootApplicationParametersEnd;
    }

    //
    // Determine the size required for the input parameters.
    //

    AllocationFlags = (MEMORYMAP_TYPE_ALLOCATED |
                       MEMORYMAP_TYPE_MAPPED_UNALLOCATED);

    RequiredSize = sizeof(BOOT_APPLICATION_PARAMETERS);
    RequiredSize += sizeof(ALLOCATED_MEMORY);
    DescriptorBuffer.BufferSize = 0;
    Status = BlMmGetMemoryMap(&AllocatedMDL,
                              &DescriptorBuffer,
                              AllocationFlags,
                              MDL_FLAGS_NONE);

    //
    // The above call will determine the required buffer size for the MDL.
    // Allow Status to equal STATUS_SUCCESS, since a zero buffer size will
    // return successfully.
    //

    if ((Status != STATUS_BUFFER_TOO_SMALL) && (Status != STATUS_SUCCESS)) {
        goto InitializeBootApplicationParametersEnd;
    }

    //
    // The descriptor buffer will be filled by BlMmGetMemoryMap, which expects
    // the base address of the given descriptor array to be appropriately
    // aligned.  If necessary, allocate extra space here to allow the
    // descriptor buffer to start on the appropriate boundary.
    //

    RequiredSize = ALIGN_RANGE_UP(RequiredSize,
                                  TYPE_ALIGNMENT(MEMORY_DESCRIPTOR));

    RequiredSize += DescriptorBuffer.BufferSize;

    //
    // The allocated memory, boot entry and boot device structures are
    // variable in length.  Determine how much memory is required for each
    // before adding the fixed size of the trailing firmware parameter and
    // return argument structures.
    //

    ApplicationEntrySize =
        FIELD_OFFSET(BOOT_APPLICATION_TRANSITION_ENTRY, InlineOptions) +
        BlGetBootOptionListSize(&ApplicationEntry->InlineOptions);

    BL_ASSERT_ASSUME(RequiredSize < (RequiredSize + ApplicationEntrySize));

    RequiredSize += ApplicationEntrySize;
    BootDevice = BlGetBootDevice();

    BL_ASSERT_ASSUME(RequiredSize < (RequiredSize + BootDevice->Size));

    RequiredSize += BootDevice->Size;
    RequiredSize += sizeof(FIRMWARE_PARAMETERS);
    RequiredSize += sizeof(BOOT_APPLICATION_RETURN_ARGUMENTS);

    //
    // Ensure that the specified buffer is large enough for the input
    // parameter structure.
    //

    if (EntryParameters->BufferSize < RequiredSize) {

        //
        // If the caller did not request a new buffer to be allocated, then
        // return with status, STATUS_BUFFER_TOO_SMALL.
        //

        if (AllocateBuffer == FALSE) {
            EntryParameters->BufferSize = RequiredSize;
            Status = STATUS_BUFFER_TOO_SMALL;
            goto InitializeBootApplicationParametersEnd;
        }

        //
        // Allocate a buffer large enough for the entire data structure.
        //

        if (EntryParameters->BufferSize != 0) {
            BlMmFreeHeap(EntryParameters->Buffer);
        }

        EntryParameters->BufferSize = RequiredSize;
        EntryParameters->Buffer = BlMmAllocateHeap(RequiredSize);
        if (EntryParameters->Buffer == NULL) {
            Status = STATUS_NO_MEMORY;
            goto InitializeBootApplicationParametersEnd;
        }
    }

    //
    // Build the input parameter structure.
    //

    BaParameters = EntryParameters->Buffer;
    RtlZeroMemory(BaParameters, RequiredSize);

    //
    // Header.
    //

    BaParameters->Header.Signature = BOOT_APPLICATION_PARAMETERS_SIGNATURE;
    BaParameters->Header.Version = BOOT_APPLICATION_PARAMETERS_VERSION;
    BaParameters->Header.SizeOfParameters = RequiredSize;

    //
    // Simple data (not a complex structure referenced from parameters).
    //

    BaParameters->MachineType = BOOT_APPLICATION_MACHINE_TYPE;
    BaParameters->TranslationType = BlMmQueryTranslationType();
    BaParameters->ImageBase = (ULONGLONG)(UINTN)ImageBase;
    BaParameters->ImageSize = BlImgGetSizeOfImage(NtHeader);

    Size = sizeof(BOOT_APPLICATION_PARAMETERS);

    //
    // Allocated MDL.  This code needs to carefully align the base of the
    // descriptor array that is passed to BlMmGetMemoryMap.
    //
    // Note that the aligned buffer offset needs to be computed relative to
    // the base of the application parameter structure itself since the
    // allocated memory structure is not necessarily aligned.
    //

    BaParameters->AllocatedMemoryOffset = Size;
    BaAllocatedMemory = Add2Ptr(BaParameters, Size);
    BaAllocatedMemory->Version = ALLOCATED_MEMORY_VERSION;
    AlignedBufferOffset = ALIGN_RANGE_UP(Size + sizeof(ALLOCATED_MEMORY),
                                         TYPE_ALIGNMENT(MEMORY_DESCRIPTOR));

    BaAllocatedMemory->MdlOffset = AlignedBufferOffset - Size;
    DescriptorBuffer.Buffer = Add2Ptr(BaAllocatedMemory,
                                      BaAllocatedMemory->MdlOffset);

    Status = BlMmGetMemoryMap(&AllocatedMDL,
                              &DescriptorBuffer,
                              AllocationFlags,
                              MDL_FLAGS_NONE);

    if (!NT_SUCCESS(Status)) {
        goto InitializeBootApplicationParametersEnd;
    }

    //
    // N.B. The C standard guarantees that the sizeof operator will correctly
    //      return the number of bytes separating adjacent array entries, even
    //      in the case where trailing padding is added to the structure.
    //

    ArraySlotSize = sizeof(MEMORY_DESCRIPTOR);
    BaAllocatedMemory->SizeOfDescriptor = ArraySlotSize;
    BaAllocatedMemory->NumberOfDescriptors =
        (DescriptorBuffer.DataSize / ArraySlotSize);

    BaAllocatedMemory->FixedDataOffset =
        FIELD_OFFSET(MEMORY_DESCRIPTOR, FixedLengthData);

    Size += BaAllocatedMemory->MdlOffset;
    Size += DescriptorBuffer.BufferSize;

    //
    // Application Entry.
    //

    BaParameters->ApplicationEntryOffset = Size;
    BaApplicationEntry = Add2Ptr(BaParameters, Size);

    BL_ASSERT_ASSUME((Size + ApplicationEntrySize) <= RequiredSize);

    RtlCopyMemory(BaApplicationEntry,
                  ApplicationEntry,
                  ApplicationEntrySize);

    Size += ApplicationEntrySize;

    //
    // Copy the boot device from the global structure.
    //

    BaParameters->BootDeviceOffset = Size;
    BaBootDevice = Add2Ptr(BaParameters, Size);

    BL_ASSERT_ASSUME((Size + BootDevice->Size) <= RequiredSize);

    Status = ImgpCopyApplicationBootDevice(BaBootDevice, BootDevice);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Size += BaBootDevice->Size;

    //
    // Firmware Parameters.
    //

    BaParameters->FirmwareDataOffset = Size;
    BaFirmwareParameters = Add2Ptr(BaParameters, Size);

    BL_ASSERT_ASSUME((Size + sizeof(FIRMWARE_PARAMETERS)) <= RequiredSize);

    Status = BlFwGetParameters(BaFirmwareParameters);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Size += sizeof(FIRMWARE_PARAMETERS);

    //
    // Return Arguments.
    //

    BaParameters->ReturnArgumentOffset = Size;

    BL_ASSERT_ASSUME((Size + sizeof(BOOT_APPLICATION_RETURN_ARGUMENTS) <=
                      RequiredSize));

    BaReturnArgs = Add2Ptr(BaParameters, Size);
    BaReturnArgs->Version = BOOT_APPLICATION_RETURN_ARGUMENTS_VERSION;
    BaReturnArgs->PersistentData.PageCount = 0;
    Size += sizeof(BOOT_APPLICATION_RETURN_ARGUMENTS);

    //
    // Done.
    //

    EntryParameters->DataSize = Size;

    ASSERT(Size <= RequiredSize);

    Status = STATUS_SUCCESS;

InitializeBootApplicationParametersEnd:
    if (ConvertedEntry != NULL) {
        BlMmFreeHeap(ConvertedEntry);
    }

    return Status;
}

NTSTATUS
ImgpComputeCodeIntegrityFlags (
    __in PBOOT_APPLICATION_ENTRY ApplicationEntry,
    __out PULONG ImageFlags
    )

/*++

Routine Description:

    This function determines the code integrity validation flags that should
    be used when loading the boot application described by the given
    application entry.

Arguments:

    ApplicationEntry - Supplies the application entry describing the boot
        application being loaded.

    ImageFlags - Supplies a pointer to a variable that receives a bitmask
        containing the code integrity validation flags that should be applied
        when loading this application.

Return Value:

    STATUS_SUCCESS is returned if the application load should proceed using
        the returned load flags.

    An error code is returned if a fundamental code integrity problem occurs
        and application load should be blocked immediately.

--*/

{

    BOOLEAN IntegrityChecksDisabled;

    //
    // Query the code integrity configuration elements from the option list
    // associated with the given application entry.  Note that the query
    // routine internally detects entries describing an OS loader WinPE boot
    // attempt and forces integrity checks off in these cases.
    //

    BlImgQueryCodeIntegrityBootOptions(ApplicationEntry,
                                       &IntegrityChecksDisabled,
                                       NULL,
                                       NULL);

    //
    // If integrity checks are explicitly disabled, then it is likely that
    // this boot entry describes a test application of some sort.  Validating
    // the hash is an option since it would allow integrity check failures to
    // be logged or possibly reported to a boot debugger.  Reporting failures
    // to the boot debugger is not desirable for test applications and no
    // persistent logging of boot application integrity check failures is
    // done at this time.  Therefore, no hash validation is performed when
    // loading boot applications that disable integrity checks in their option
    // list.
    //

    if (IntegrityChecksDisabled != FALSE) {
        *ImageFlags = 0;
        return STATUS_SUCCESS;
    }

    //
    // Integrity checks are not explicitly disabled in the application boot
    // options, so set image flags that will block image load unless the image
    // is correctly signed and carries a valid image hash.
    //

    *ImageFlags = IMAGE_FLAGS_VALIDATE_IMAGE_HASH |
                  IMAGE_FLAGS_VALID_HASH_REQUIRED |
                  IMAGE_FLAGS_WINDOWS_SIGNATURE_REQUIRED;

    return STATUS_SUCCESS;
}

