/*++

Copyright (c) Microsoft Corporation

Module Name:

    registry.c

Abstract:

    The OS Loader will load a system hive into memory for the purpose of
    both collecting data (boot driver list, NLS data, profile information, etc)
    as well as the hive's need during kernel initialization and therefore it's
    requirement in the loader block.

    This module implements the interface for interacting with the system hive.

Environment:

    Boot Environment.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"
#include "systemevents.h"

//
// ------------------------------------------------- Data Structure Definitions
//

//
// Each loaded hive will be stored in a table using the Generic Boot
// Environment Table APIs.  Each entry in the table is a LOADED_HIVE which
// contains the address and size of the hive as well as an internal CMHIVE
// data structure.
//

typedef struct _LOADED_HIVE {
    PVOID ImageBase;
    ULONG ImageSize;
    CMHIVE Hive;
    HCELL_INDEX ControlSet;
    PWCHAR FileName;
    ULONG Flags;
    PBOOT_ENVIRONMENT_DEVICE Device;
} LOADED_HIVE, *PLOADED_HIVE;

typedef struct _HIVE_RECOVERY_READ_CONTEXT {
    PUCHAR LogBase;
    ULONG LogSize;
} HIVE_RECOVERY_READ_CONTEXT, *PHIVE_RECOVERY_READ_CONTEXT;

#define HIVE_MAX_LOG_COUNT 2

typedef struct _HIVE_LOG_INFORMATION {
    PVOID LogImages[HIVE_MAX_LOG_COUNT];
    ULONG LogImageSizes[HIVE_MAX_LOG_COUNT];
    ULONG LogIndices[HIVE_MAX_LOG_COUNT];
    ULONG MaximumHiveSize;
    ULONG FirstLogSequence;
    BOOLEAN UseLegacyRecovery[HIVE_MAX_LOG_COUNT];
} HIVE_LOG_INFORMATION, *PHIVE_LOG_INFORMATION;

#define LOADED_HIVE_FLAGS_NONE  0
#define LOADED_HIVE_FLAGS_DIRTY 1

//
// -------------------------------------------------------------------- Globals
//

BOOLEAN HiveHashLibraryInitialized = FALSE;
ULONG HiveAllocatedTableEntries;
ULONG HiveTableEntries;
PLOADED_HIVE *HiveTable = NULL;

#define INITIAL_HIVE_TABLE_SIZE (4)
#define CM_MAX_NAME             (64)
#define MAX_HARDWARE_CONFIGURATION_NUMBER_LENGTH (64)

const UNICODE_STRING DriverLoadPolicySubKeyPath =
       RTL_CONSTANT_STRING(L"System\\CurrentControlSet\\Policies\\EarlyLaunch");

const UNICODE_STRING DriverLoadPolicySubKeyPathFallback =
        RTL_CONSTANT_STRING(L"System\\CurrentControlSet\\Control\\EarlyLaunch");

const UNICODE_STRING DriverLoadPolicyValue =
        RTL_CONSTANT_STRING(L"DriverLoadPolicy");

extern const PCWSTR OslControlString;
const PCWSTR OslDeviceLockSettingString = L"DeviceLockSettings";
const PCWSTR OslEnabledString = L"Enabled";

const PCWSTR OslSetupString = L"Setup";
const PCWSTR OslUpgradeString = L"Upgrade";

//
// ----------------------------------------------------------------- Prototypes
//

PVOID
HiveAllocate (
    __in ULONG Length,
    __in BOOLEAN UseForIo,
    __in ULONG Tag
    );

VOID
HiveFree (
    __in PVOID MemoryBlock,
    __in ULONG GlobalQuotaSize
    );

NTSTATUS
HiveGetLogInformation (
    __in DEVICE_ID DeviceId,
    __in PWSTR LogPath,
    __in PLARGE_INTEGER TimeStamp,
    __in ULONG MinimumLogSequence,
    __deref_out_bcount(*LogImageSize) PVOID *LogImage,
    __out PULONG LogImageSize,
    __out PULONG MaximumHiveSize,
    __out PULONG FirstLogSequence,
    __out PBOOLEAN UseLegacyRecovery
    );

NTSTATUS
HiveGetHiveLogInformation (
    __in DEVICE_ID DeviceId,
    __in_ecount(HIVE_MAX_LOG_COUNT) PWSTR *LogPaths,
    __in PLARGE_INTEGER TimeStamp,
    __in ULONG MinimumLogSequence,
    __in BOOLEAN PrimaryBaseBlockValid,
    __out PHIVE_LOG_INFORMATION LogInformation
    );

NTSTATUS
HiveInitializeAndValidate (
    __out PLOADED_HIVE LoadedHive
    );

NTSTATUS
HiveLoadHiveFromLocation (
    __in DEVICE_ID DeviceId,
    __in __nullterminated PWCHAR FilePath,
    __deref_out_bcount(*ImageSize) PVOID *ImageBase,
    __out PULONG ImageSize,
    __out PLOADER_HIVE_RECOVERY_INFO RecoveryInfo,
    __out_opt PULONG Handle
    );

NTSTATUS
HiveRecover (
    __inout PHHIVE Hive,
    __inout_bcount(HiveSize) PVOID HiveBase,
    __in ULONG HiveSize,
    __in_bcount(LogSize) PVOID LogBase,
    __in ULONG LogSize,
    __inout PULONG Sequence,
    __in BOOLEAN UseLegacyRecovery,
    __out PULONG LogNextOffset
    );

NTSTATUS
HiveDisableDriverVerifier (
    __in ULONG Handle
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
OslLoadAndInitializeHive (
    __in DEVICE_ID DeviceId,
    __in PWSTR FilePath,
    __in BOOLEAN SystemHive,
    __in_opt PWSTR LogPath1,
    __in_opt PWSTR LogPath2,
    __deref_out_bcount(*ImageSize) PVOID *ImageBase,
    __out PULONG ImageSize,
    __out PLOADER_HIVE_RECOVERY_INFO RecoveryInfo,
    __out_opt PULONG Handle
    )

/*++

Routine Description:

    Attempts to load and an initialize a hive into memory.

Arguments:

    DeviceId - Handle to the device containing the specified hive.

    FilePath - The file path for the hive.

    SystemHive - Supplies whether or not SYSTEM hive is being loaded

    LogPath - Supplies a file path to a hive log.  If an inconsistency is found
        during initialization of the hive or during explicit consistency
        checks, this log file will be used in attempt to fixup the
        inconsistencies.

    ImageBase - On input, contains a pointer to the preferred address for the
        image's allocation.  *ImageBase == NULL indicates there is no
        preference.  On output, contains a pointer to the base address of the
        loaded image.

    ImageSize - On output, contains the size of the loaded image.

    RecoveryInfo - Supplies a pointer to a buffer that upon success receives
                   information about the recovery performed on the hive.

    Handle - On output, contains a handle to be used for all subsequent
        references to the loaded hive (when specified, i.e. Handle != NULL).

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if any parameter is NULL.
    STATUS_NO_MEMORY if memory can not be allocated for an internal data
        structure.
    STATUS_NOT_FOUND if the specified hive does not exist.

--*/

{

    PHBIN Bin;
    PWCHAR ControlSetName;
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    LARGE_INTEGER EndTime;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[2];
    ULONG FilePathLength;
    LARGE_INTEGER Frequency;
    ULONGLONG FrequencyInMilliseconds;
    BOOLEAN ImageBufferAllocated;
    ULONG Index;
    BOOLEAN KsrRecovered;
    PLOADED_HIVE LoadedHive;
    ULONG LogCurrentOffset;
    HIVE_LOG_INFORMATION LogInformation;
    ULONG LogMinimumSequence;
    PWSTR LogPaths[HIVE_MAX_LOG_COUNT];
    ULONG MaximumHiveSize;
    ULONG MinimumLogSequence;
    PVOID NewImage;
    PHBASE_BLOCK PrimaryBaseBlock;
    BOOLEAN PrimaryBaseBlockValid;
    BOOLEAN Recovered;
    ULONG Sequence;
    LARGE_INTEGER StartTime;
    NTSTATUS Status;
    LARGE_INTEGER TimeStamp;
    ULONGLONG TimeTaken;

    ImageBufferAllocated = FALSE;
    KsrRecovered = FALSE;
    LoadedHive = NULL;
    RtlZeroMemory(&LogInformation, sizeof(HIVE_LOG_INFORMATION));

    LogMinimumSequence = 0;
    LogCurrentOffset = 0;
    Recovered = FALSE;
    StartTime = BlTimeQueryPerformanceCounter(NULL);

    if (HiveHashLibraryInitialized == FALSE) {
        Status = HvInitializeHashLibrary();
        if (!NT_SUCCESS(Status)) {
            goto OslLoadAndInitializeHiveEnd;
        }

        HiveHashLibraryInitialized = TRUE;
    }

    //
    // Simple parameter checking.
    //

    if ((FilePath == NULL)  || (ImageBase == NULL) || (ImageSize == NULL)) {
        Status = STATUS_INVALID_PARAMETER;
        goto OslLoadAndInitializeHiveEnd;
    }

    //
    // Attempt to read the hive into memory.
    //

    Status = BlImgLoadImageEx(DeviceId,
                              OSL_MEMORY_TYPE_REGISTRY_DATA,
                              FilePath,
                              ImageBase,
                              ImageSize,
                              MEMORY_ATTRIBUTE_ALLOCATION_KSEG,
                              0,
                              IMAGE_FLAGS_NONE);

#if defined(NTKERNEL)

    //
    // When preparing for the SoftReboot, the attempt to load a
    // hive will fail with sharing violation error in case
    // if target OS is the same as the current one.
    // This error is recoverable, but this entire issue applies only to the
    // SoftReboot scenario, so the code is forked on #define to make it clear.
    //

    if (STATUS_SHARING_VIOLATION == Status) {
        Status = OslLoadHiveFromHost(DeviceId,
                                     FilePath,
                                     ImageBase,
                                     ImageSize);

        //
        // Keep the original status code in case if error returned
        // by the routine is not sufficiently meaningful.
        //

        if (STATUS_UNSUCCESSFUL == Status) {
            Status = STATUS_SHARING_VIOLATION;
        }

        //
        // This code loaded the hive from the currently
        // running system and it may change before target OS starts
        // using it. To prevent the mismatch between the currently loaded
        // data and files on the disk, mark the hive as been recovered
        // at boot. This way the target OS will force its flush to the storage
        // maintaining data consistency at the cost of discarding
        // intermediate changes.
        //

        KsrRecovered = TRUE;
        Sequence = 0;
    }

#endif

    if (!NT_SUCCESS(Status)) {
        goto OslLoadAndInitializeHiveEnd;
    }

    ImageBufferAllocated = TRUE;

    //
    // The hive was successfully read into memory.  Allocate a tracking
    // structure to be used to initialize the hive.
    //

    LoadedHive = BlMmAllocateHeap(sizeof(LOADED_HIVE));
    if (LoadedHive == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto OslLoadAndInitializeHiveEnd;
    }

    LoadedHive->ImageBase = *ImageBase;
    LoadedHive->ImageSize = *ImageSize;
    LoadedHive->ControlSet = HCELL_NIL;

    //
    // Save the hive file name and boot device so that changes to the loaded
    // image can be flushed back to non-volatile storage, if necessary.
    //

    FilePathLength = (ULONG)wcslen(FilePath);
    LoadedHive->FileName = BlMmAllocateHeap((FilePathLength + 1) *
                                            sizeof(WCHAR));

    if (LoadedHive->FileName != NULL) {
        LoadedHive->FileName[FilePathLength] = L'\0';
        RtlCopyMemory(LoadedHive->FileName,
                      FilePath,
                      FilePathLength * sizeof(WCHAR));
    }

    Status = BlGetDeviceIdentifier(DeviceId, &DeviceIdentifier);
    if (NT_SUCCESS(Status)) {
        LoadedHive->Device = BlMmAllocateHeap(DeviceIdentifier->Size);
        if (LoadedHive->Device != NULL) {
            RtlCopyMemory(LoadedHive->Device,
                          DeviceIdentifier,
                          DeviceIdentifier->Size);
        }
    }

    //
    // If the primary hive image is too small to contain a base block,
    // then there is no way to repair the hive, so exit.
    //

    PrimaryBaseBlock = (PHBASE_BLOCK)LoadedHive->ImageBase;
    if ((LoadedHive->ImageSize < HBLOCK_SIZE) ||
        (PrimaryBaseBlock->Length + HBLOCK_SIZE) < HBLOCK_SIZE) {

        Status = STATUS_REGISTRY_CORRUPT;
        goto OslLoadAndInitializeHiveEnd;
    }

    //
    // Locate the hive's timestamp so that it can be correlated with the
    // timestamp in the hive's log file. If the hive's header has a valid
    // checksum, use the timestamp in the header. Otherwise, use the
    // timestamp in the hive's first bin.
    //

    if (HvIsInPlaceBaseBlockValid(PrimaryBaseBlock) != FALSE) {
        PrimaryBaseBlockValid = TRUE;
        MaximumHiveSize = PrimaryBaseBlock->Length;
        TimeStamp.QuadPart = PrimaryBaseBlock->TimeStamp.QuadPart;

    } else {
        PrimaryBaseBlockValid = FALSE;
        MaximumHiveSize = 0;

        //
        // If the header's checksum is invalid, get the timestamp from the first
        // bin instead. If that bin does not exist in the image, fail the load.
        //

        if (HvpHeaderCheckSum(PrimaryBaseBlock) != PrimaryBaseBlock->CheckSum) {
            if (LoadedHive->ImageSize < (HBLOCK_SIZE + sizeof(HBIN))) {
                Status = STATUS_REGISTRY_CORRUPT;
                goto OslLoadAndInitializeHiveEnd;
            }

            Bin = (PHBIN)Add2Ptr(LoadedHive->ImageBase, HBLOCK_SIZE);
            if (Bin->Signature != HBIN_SIGNATURE) {
                Status = STATUS_REGISTRY_CORRUPT;
                goto OslLoadAndInitializeHiveEnd;
            }

            TimeStamp.QuadPart = Bin->TimeStamp.QuadPart;

        } else {
            TimeStamp.QuadPart = PrimaryBaseBlock->TimeStamp.QuadPart;
        }
    }

    //
    // If the hive has an invalid primary base block, or has a valid primary
    // base block with mismatched sequence numbers, try to get information
    // about the hive's log files. Otherwise, verify that the hive can be
    // loaded without recovery.
    //

    if ((PrimaryBaseBlockValid == FALSE) ||
        (PrimaryBaseBlock->Sequence1 != PrimaryBaseBlock->Sequence2)) {

        if (PrimaryBaseBlockValid == FALSE) {
            MinimumLogSequence = 0;

        } else {
            MinimumLogSequence = PrimaryBaseBlock->Sequence2;
        }

        LogPaths[0] = LogPath1;
        LogPaths[1] = LogPath2;
        Status = HiveGetHiveLogInformation(DeviceId,
                                           LogPaths,
                                           &TimeStamp,
                                           MinimumLogSequence,
                                           PrimaryBaseBlockValid,
                                           &LogInformation);

        if (LogInformation.MaximumHiveSize > MaximumHiveSize) {
            MaximumHiveSize = LogInformation.MaximumHiveSize;
        }

    } else {

        //
        // The hive image must be large enough to map the length specified
        // by the hive's base block. If the image is too small, the hive is
        // truncated, and because log file recovery is not in use, the
        // truncation cannot be repaired by adding data back from the log
        // files. If this occurs, report that the hive is corrupt and exit
        // so that a recovery environment will eventually be invoked.
        //

        if ((PrimaryBaseBlock->Length + HBLOCK_SIZE) > LoadedHive->ImageSize) {
            Status = STATUS_REGISTRY_CORRUPT;

        } else {
            Status = STATUS_SUCCESS;
        }

        ASSERT(LogInformation.LogImages[0] == NULL);
        ASSERT(LogInformation.LogImages[1] == NULL);

        LoadedHive->Hive.Hive.BaseBlock = PrimaryBaseBlock;
    }

    if (!NT_SUCCESS(Status)) {
        goto OslLoadAndInitializeHiveEnd;
    }

    //
    // If the primary base block is invalid and there are no logs to recover
    // from, fail the load.
    //

    if (((PrimaryBaseBlockValid == FALSE) ||
         (PrimaryBaseBlock->Sequence1 != PrimaryBaseBlock->Sequence2)) &&
        (LogInformation.LogImages[0] == NULL) &&
        (LogInformation.LogImages[1] == NULL)) {

        Status = STATUS_REGISTRY_CORRUPT;
        goto OslLoadAndInitializeHiveEnd;
    }

    //
    // If the existing hive image buffer is too small to hold the contents of
    // the recovered hive, attempt to allocate a new range of pages that is
    // large enough to hold the entire buffer.
    //

    ASSERT(MaximumHiveSize != 0);
    ASSERT((MaximumHiveSize % HBLOCK_SIZE) == 0);
    ASSERT((MaximumHiveSize + HBLOCK_SIZE) > MaximumHiveSize);

    if (*ImageSize < (MaximumHiveSize + HBLOCK_SIZE)) {
        Status =
            BlMmAllocatePages(&NewImage,
                              (MaximumHiveSize + HBLOCK_SIZE) >> PAGE_SHIFT,
                              OSL_MEMORY_TYPE_REGISTRY_DATA,
                              MEMORY_ATTRIBUTE_ALLOCATION_KSEG,
                              0);

        if (!NT_SUCCESS(Status)) {
            goto OslLoadAndInitializeHiveEnd;
        }

        //
        // Copy the old image into the new buffer, then free the old buffer.
        //

        RtlCopyMemory(NewImage, *ImageBase, *ImageSize);
        BlMmFreePages(*ImageBase);
        *ImageBase = NewImage;
        NewImage = NULL;
        *ImageSize = MaximumHiveSize + HBLOCK_SIZE;
        LoadedHive->ImageBase = *ImageBase;
        LoadedHive->ImageSize = *ImageSize;
    }

    //
    // Apply any changes from the log files.
    //

    Sequence = LogInformation.FirstLogSequence;
    for (Index = 0; Index < HIVE_MAX_LOG_COUNT; Index += 1) {
        if (LogInformation.LogImages[Index] == NULL) {
            break;
        }

        LogMinimumSequence = Sequence;

        Status = HiveRecover(&LoadedHive->Hive.Hive,
                             *ImageBase,
                             *ImageSize,
                             LogInformation.LogImages[Index],
                             LogInformation.LogImageSizes[Index],
                             &Sequence,
                             LogInformation.UseLegacyRecovery[Index],
                             &LogCurrentOffset);

        if (!NT_SUCCESS(Status)) {
            goto OslLoadAndInitializeHiveEnd;
        }

        Recovered = TRUE;
    }

    //
    // If recovery occurred, the kernel will need to write the recovered
    // hive image back to the disk when it receives the hive. Indicate
    // that this needs to occur by setting the recovery flag in the base
    // block and setting up the recovery information structure.
    //
    // N.B. The flag will only propagate correctly if it is set in the
    //      base block located at the start of the hive image that will
    //      be handed off to the kernel.
    //

    if ((Recovered != FALSE) || (KsrRecovered != FALSE)) {

        NT_ASSERT(LoadedHive->Hive.Hive.BaseBlock == *ImageBase);

        LoadedHive->Hive.Hive.BaseBlock->Sequence2 =
            LoadedHive->Hive.Hive.BaseBlock->Sequence1 =
            Sequence;

        LoadedHive->Hive.Hive.BaseBlock->BootRecover = 1;
        LoadedHive->Hive.Hive.BaseBlock->CheckSum =
            HvpHeaderCheckSum(LoadedHive->Hive.Hive.BaseBlock);

        //
        // Setup the recovery info.
        //
        // If recovery was indicated due to a soft reboot then tell the kernel
        // to write its next log entry at the start of log 1 (since the log
        // state can be changing any information provided here may be wrong so
        // just keep the Threshold behaviour).
        //

        RtlZeroMemory(RecoveryInfo, sizeof(*RecoveryInfo));

        if (KsrRecovered != FALSE) {

            RecoveryInfo->Recovered = TRUE;
            RecoveryInfo->LegacyRecovery = FALSE;
            RecoveryInfo->SoftRebootConflict = TRUE;

            RecoveryInfo->MostRecentLog = HFILE_TYPE_LOG1;
            RecoveryInfo->LogNextSequence = 0;
            RecoveryInfo->LogMinimumSequence = 0;
            RecoveryInfo->LogCurrentOffset = 0;

        } else {

            RecoveryInfo->Recovered = TRUE;
            RecoveryInfo->SoftRebootConflict = FALSE;

            if (LogInformation.LogImages[1] != NULL) {

                RecoveryInfo->MostRecentLog = LogInformation.LogIndices[1];
                RecoveryInfo->LegacyRecovery =
                    LogInformation.UseLegacyRecovery[1];

            } else {

                NT_ASSERT(LogInformation.LogImages[0] != NULL);

                RecoveryInfo->MostRecentLog = LogInformation.LogIndices[0];
                RecoveryInfo->LegacyRecovery =
                    LogInformation.UseLegacyRecovery[0];
            }

            if (RecoveryInfo->MostRecentLog == 0) {
                RecoveryInfo->MostRecentLog = HFILE_TYPE_LOG1;
            } else {
                RecoveryInfo->MostRecentLog = HFILE_TYPE_LOG2;
            }

            RecoveryInfo->LogNextSequence = Sequence;
            RecoveryInfo->LogMinimumSequence = LogInformation.FirstLogSequence;
            RecoveryInfo->LogCurrentOffset = LogCurrentOffset;
        }
    }

    //
    // All of the changes in the log files have been applied, so attempt to
    // initialize and validate the hive.
    //

    Status = HiveInitializeAndValidate(LoadedHive);
    if (!NT_SUCCESS(Status)) {
        goto OslLoadAndInitializeHiveEnd;
    }

    //
    // Allocate the global table if it is not currently allocated.
    //

    if (Handle != NULL) {
        if (HiveTableEntries == 0) {
            HiveTableEntries = INITIAL_HIVE_TABLE_SIZE;
            HiveTable =
                BlMmAllocateHeap(HiveTableEntries * sizeof(PLOADED_HIVE));

            if (HiveTable == NULL) {
                Status = STATUS_NO_MEMORY;
                goto OslLoadAndInitializeHiveEnd;
            }

            RtlZeroMemory(HiveTable, HiveTableEntries * sizeof(PLOADED_HIVE));
            HiveAllocatedTableEntries = 0;
        }

        //
        // Insert the entry into the global Hive Table.
        //

        Status = BlTblSetEntry(&HiveTable,
                               &HiveTableEntries,
                               LoadedHive,
                               Handle,
                               TblDoNotPurgeEntry);

        if (!NT_SUCCESS(Status)) {
            goto OslLoadAndInitializeHiveEndAfterAllocation;
        }
    }

    //
    // Initially use the Default control set. Reinitialization of the system
    // hive will result in the correct control set being set based on the load
    // options. If this fails, it is not fatal as the LastKnownGood control
    // set can be chosen via the boot menu.
    //

    if (SystemHive != FALSE) {
        ControlSetName = L"Default";
        Status = OslSetControlSet(*Handle, ControlSetName);
        if (!NT_SUCCESS(Status)) {

            ASSERT(HiveTable[*Handle]->ControlSet == HCELL_NIL);

        }
    }

    Status = STATUS_SUCCESS;

OslLoadAndInitializeHiveEndAfterAllocation:

    //
    // Routine cleanup.
    //

    if (!NT_SUCCESS(Status)) {

        //
        // If we just allocated the hive table, free the memory now to avoid
        // getting out of sync.
        //

        if ((Handle != NULL) && (HiveAllocatedTableEntries == 0)) {
            BlMmFreeHeap(HiveTable);
        }
    }

OslLoadAndInitializeHiveEnd:

    if (!NT_SUCCESS(Status)) {
        if (LoadedHive != NULL) {
            BlMmFreeHeap(LoadedHive);
        }

        if (ImageBufferAllocated) {
            BlMmFreePages(*ImageBase);
        }

    } else {

        //
        // Keep track of entries in the loaded hive table. (if used)
        //

        if (Handle != NULL) {
            HiveAllocatedTableEntries += 1;
        }
    }

    for (Index = 0; Index < HIVE_MAX_LOG_COUNT; Index += 1) {
        if (LogInformation.LogImages[Index] != NULL) {
            BlMmFreePages(LogInformation.LogImages[Index]);
        }
    }

    if (NT_SUCCESS(Status)) {
        EndTime = BlTimeQueryPerformanceCounter(&Frequency);
        FrequencyInMilliseconds = Frequency.QuadPart / 1000;

        TimeTaken = (EndTime.QuadPart - StartTime.QuadPart) /
            (FrequencyInMilliseconds);

        EventDataDescCreate(&EventDataDescriptor[0],
                            FilePath,
                            (ULONG)(wcslen(FilePath) + 1) * sizeof(WCHAR));

        EventDataDescCreate(&EventDataDescriptor[1],
                            (PVOID)&TimeTaken,
                            sizeof(ULONGLONG));

        BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                      &BOOT_LOAD_HIVE_TIME,
                      NULL,
                      2,
                      EventDataDescriptor);
    }

    return Status;
}

NTSTATUS
OslReinitializeSystemHive (
    __in ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine reinitializes the system registry hive after the advanced
    options have been processed. This supports options that require enabling
    regsitry settings or setting a different control set as the current control
    set.

Arguments:

    SystemHiveHandle - Supplies a handle to the system registry hive.

Return Value:

    NTSTATUS Code.

--*/

{

    PWCHAR ControlSetName;
    NTSTATUS ControlSetStatus;
    BOOLEAN DisableAutoReboot;
    BOOLEAN LastKnownGoodBoot;
    NTSTATUS Status;
    ULONG ValueData = 0;

    //
    // If the default control set was loaded, mark control set status as
    // success. Otherwise it should be STATUS_NOT_FOUND as this is the error
    // reported back from the registry when a current control set is missing.
    //

    if (HiveTable[SystemHiveHandle]->ControlSet != HCELL_NIL) {
        ControlSetStatus = STATUS_SUCCESS;

    } else {
        ControlSetStatus = STATUS_NOT_FOUND;
    }

    //
    // Determine if the user requested LKG through the options menu.
    //

    Status = BlGetApplicationOptionBoolean(
        BCDE_OSLOADER_TYPE_LAST_GOOD_SETTINGS,
        &LastKnownGoodBoot);

    //
    // Ignore the LKG option if it has been specifically disabled
    // by the following registry key:
    //
    // HKLM\System\CurrentControlSet\Control\PnP\DisableLKG
    //
    // N.B. This assumes that the Default control set is already set to be the
    //      CurrentControlSet. This is a safe assumption, because failing to
    //      load the Default control set is fatal.
    //

    if (NT_SUCCESS(Status) && (LastKnownGoodBoot != FALSE)) {
        if (NT_SUCCESS(ControlSetStatus)) {
            Status = OslHiveReadWriteControlDword(SystemHiveHandle,
                                                  FALSE,
                                                  L"PnP",
                                                  L"DisableLKG",
                                                  &ValueData);

        } else {
            ValueData = 0;
        }

        if (!NT_SUCCESS(Status) || (ValueData != 1)) {
            ControlSetName = L"LastKnownGood";
            ControlSetStatus = OslSetControlSet(SystemHiveHandle,
                                                ControlSetName);
        }
    }

    //
    // After processing the LKG load option, if there is still no control set
    // for the system hive, then this is fatal.
    //

    if (!NT_SUCCESS(ControlSetStatus)) {
        Status = ControlSetStatus;
        goto OslReinitializeSystemHiveEnd;
    }

    //
    // Disable auto-reboot by zeroing the following key value in the
    // in-memory copy of the hive:
    //
    // HKLM\System\CurrentControlSet\Control\CrashControl\AutoReboot
    //

    Status = BlGetApplicationOptionBoolean(
        BCDE_OSLOADER_TYPE_DISABLE_CRASH_AUTOREBOOT,
        &DisableAutoReboot);

    if (NT_SUCCESS(Status) && (DisableAutoReboot != FALSE)) {
        ValueData = 0;
        OslHiveReadWriteControlDword(SystemHiveHandle,
                                     TRUE,
                                     L"CrashControl",
                                     L"AutoReboot",
                                     &ValueData);
    }

    Status = STATUS_SUCCESS;

OslReinitializeSystemHiveEnd:
    return Status;
}

NTSTATUS
OslUnloadHive (
    __in ULONG Handle
    )

/*++

Routine Description:

    Unloads a previously loaded hive.  The hive must have been loaded by
    OslLoadAndInitializeHive.

Arguments:

    Handle - The hive reference returned by OslLoadAndInitializeHive.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if Handle is invalid.
    STATUS_MEMORY_NOT_ALLOCATED if there as an error freeing the memory
        allocated for the hive.

--*/

{

    NTSTATUS Status;

    if ((HiveTableEntries <= Handle) || (HiveTable[Handle] == NULL)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Free all allocations made by BlLoadApplication.
    //

    Status = BlMmFreePages(HiveTable[Handle]->ImageBase);
    if (!NT_SUCCESS(Status)) {
        Status = STATUS_MEMORY_NOT_ALLOCATED;
    }

    if (HiveTable[Handle]->FileName != NULL) {
        BlMmFreeHeap(HiveTable[Handle]->FileName);
    }

    if (HiveTable[Handle]->Device != NULL) {
        BlMmFreeHeap(HiveTable[Handle]->Device);
    }

    BlMmFreeHeap(HiveTable[Handle]);

    //
    // Clear the entry in the loaded hive table.
    //

    HiveTable[Handle] = NULL;

    //
    // Update the global count of allocated table entries.  If there are no
    // entries left in the table, free up the table.
    //

    HiveAllocatedTableEntries -= 1;
    if (HiveAllocatedTableEntries == 0) {
        HiveTableEntries = 0;
        BlMmFreeHeap(HiveTable);
        HiveTable = NULL;
    }

    return Status;
}

NTSTATUS
OslSetControlSet (
    __in ULONG Handle,
    __in __nullterminated PWCHAR ControlName
    )

/*++

Routine Description:

    Sets the control set for a given hive.

Arguments:

    Handle - The hive reference returned by OslLoadAndInitializeHive.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if Handle is invalid.
    STATUS_MEMORY_NOT_ALLOCATED if there as an error freeing the memory
        allocated for the hive.

--*/

{

    BOOLEAN AutoSelect;
    HCELL_INDEX ControlSetIndex;
    PLOADED_HIVE LoadedHive;
    UNICODE_STRING UnicodeString;

    //
    // Parameter checking.
    //

    if ((HiveTableEntries <= Handle) || (HiveTable[Handle] == NULL) ||
        (ControlName == NULL)) {

        return STATUS_INVALID_PARAMETER;
    }

    //
    // Get the hive index to the specified control set.
    //

    LoadedHive = HiveTable[Handle];
    RtlInitUnicodeString(&UnicodeString, ControlName);
    ControlSetIndex = CmpFindControlSet(&LoadedHive->Hive.Hive,
                                        LoadedHive->Hive.Hive.BaseBlock->RootCell,
                                        &UnicodeString,
                                        &AutoSelect);   // Ignored.

    if (ControlSetIndex == HCELL_NIL) {
        return STATUS_NOT_FOUND;
    }

    LoadedHive->ControlSet = ControlSetIndex;
    return STATUS_SUCCESS;
}

NTSTATUS
OslHiveFindDrivers (
    __in ULONG Handle,
    __in SERVICE_LOAD_TYPE LoadType,
    __in_opt PWSTR BootFileSystem,
    __in_opt PULONG Flags,
    __in_opt LPGUID HardwareConfigurationId,
    __in_opt PCUNICODE_STRING ManufacturingProfileName,
    __in_opt PFWUPDATE_RESULTS_HEADER FirmwareUpdateResults,
    __in PLIST_ENTRY DriverListHead
    )

/*++

Routine Description:

    Wrapper for CmpFindDrivers.  Traverses a control set in a hive,
    creating a list of drivers with the specified load type.  The list is
    unordered.

Arguments:

    Handle - The hive reference returned by OslLoadAndInitializeHive.

    LoadType - Supplies the type of drivers to be loaded (BootLoad,
            SystemLoad, AutoLoad, etc)

    BootFileSystem - If present, supplies the base name of the boot
        filesystem, which is explicitly added to the driver list.

    Flags - Supplies optional driver flags to match against.  If the flags
        argument is present, any driver with matching flags will be promoted
        to a boot driver.

    HardwareConfigurationId - Supplies an optional hardware configuration id.

    FirmwareUpdateResults - Supplies optional list of firmware update results
        gathered during this boot.

    DriverListHead - Supplies a pointer to the head of the (empty) list
            of boot drivers to load.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if Handle is invalid or if DriverListHead is NULL.
    STATUS_UNSUCCESSFUL if CmpFindDrivers failed.

--*/

{

    BOOLEAN BoolRet;
    LARGE_INTEGER EndTime;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[1];
    LARGE_INTEGER Frequency;
    ULONGLONG FrequencyInMilliseconds;
    PUNICODE_STRING HardwareConfigurationNumber;
    PWCHAR ManufacturingProfile;
    LONG Number;
    WCHAR NumberString[MAX_HARDWARE_CONFIGURATION_NUMBER_LENGTH];
    BOOLEAN ResetConfig;
    ULONGLONG SafemodeType;
    LARGE_INTEGER StartTime;
    NTSTATUS Status;
    ULONGLONG TimeTaken;
    UNICODE_STRING UnicodeString;

    HardwareConfigurationNumber = NULL;
    ManufacturingProfile = NULL;
    StartTime = BlTimeQueryPerformanceCounter(NULL);

    //
    // Parameter checking.
    //

    if ((HiveTableEntries <= Handle) || (HiveTable[Handle] == NULL) ||
        (DriverListHead == NULL)) {

        Status = STATUS_INVALID_PARAMETER;
        goto HiveFindDriversEnd;
    }

    if ((ManufacturingProfileName != NULL) &&
        (ManufacturingProfileName->Buffer != NULL)) {

        ManufacturingProfile = ManufacturingProfileName->Buffer;
    } else {
        ManufacturingProfile = NULL;
    }

    //
    // Check to see if safe mode boot has been selected.  If the system is
    // booting in safe mode, a services overlay should not be used.
    //

    Status = BlGetApplicationOptionInteger(BCDE_OSLOADER_TYPE_SAFEBOOT,
                                           &SafemodeType);

    //
    // Get the services overlay from the specified hardware configuration
    //

    if ((HardwareConfigurationId != NULL) && (!NT_SUCCESS(Status))) {
        //
        // Check to see if this hardware configuration should be reset.  If so,
        // don't use a services overlay.
        //
        Status =
            CmpCheckHardwareConfigurationResetConfig(&HiveTable[Handle]->Hive.Hive,
                                                     HardwareConfigurationId,
                                                     &ResetConfig);

        if (!NT_SUCCESS(Status)) {
            ResetConfig = FALSE;
        }

        if (!ResetConfig) {
            Status =
                CmpFindHardwareConfigurationNumber(&HiveTable[Handle]->Hive.Hive,
                                                   HardwareConfigurationId,
                                                   &Number);

            if (NT_SUCCESS(Status)) {

                //
                // Make sure that the '%d' format specifer is kept in sync with
                // what the kernel uses when setting that value.
                //

                if (swprintf_s(NumberString,
                               MAX_HARDWARE_CONFIGURATION_NUMBER_LENGTH,
                               L"%d",
                               Number) == -1) {

                    Status = STATUS_UNSUCCESSFUL;
                    goto HiveFindDriversEnd;
                }

                RtlInitUnicodeString(&UnicodeString, NumberString);

                HardwareConfigurationNumber = &UnicodeString;
            }
        }
    }

    Status = STATUS_SUCCESS;

    BoolRet = CmpFindDrivers(&HiveTable[Handle]->Hive.Hive,
                             HiveTable[Handle]->ControlSet,
                             LoadType,
                             BootFileSystem,
                             DriverListHead,
                             HardwareConfigurationNumber,
                             ManufacturingProfile,
                             Flags);

    if (BoolRet == FALSE) {
        Status = STATUS_UNSUCCESSFUL;
        goto HiveFindDriversEnd;
    }

    BoolRet = CmpFindPendingDrivers(&HiveTable[Handle]->Hive.Hive,
                                    HiveTable[Handle]->ControlSet,
                                    LoadType,
                                    DriverListHead,
                                    HardwareConfigurationNumber,
                                    ManufacturingProfile,
                                    OslIsFirmwareDependencySatisfied,
                                    FirmwareUpdateResults,
                                    Flags);

    if (BoolRet == FALSE) {
        Status = STATUS_UNSUCCESSFUL;
        goto HiveFindDriversEnd;
    }

    BoolRet = CmpAddDependentDrivers(&HiveTable[Handle]->Hive.Hive,
                                     HiveTable[Handle]->ControlSet,
                                     DriverListHead);

    if (BoolRet == FALSE) {
        Status = STATUS_UNSUCCESSFUL;
        goto HiveFindDriversEnd;
    }

    BoolRet = CmpSortDriverList(&HiveTable[Handle]->Hive.Hive,
                                HiveTable[Handle]->ControlSet,
                                DriverListHead);

    if (BoolRet == FALSE) {
        Status = STATUS_UNSUCCESSFUL;
        goto HiveFindDriversEnd;
    }

  HiveFindDriversEnd:

    EndTime = BlTimeQueryPerformanceCounter(&Frequency);
    FrequencyInMilliseconds = Frequency.QuadPart / 1000;

    TimeTaken = (EndTime.QuadPart - StartTime.QuadPart) /
                (FrequencyInMilliseconds);

    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)&TimeTaken,
                        sizeof(ULONGLONG));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_RETRIEVE_DRIVERS_LIST_TIME,
                  NULL,
                  1,
                  EventDataDescriptor);

    return Status;
}

NTSTATUS
OslHiveFindNumaData (
    __in ULONG Handle,
    __deref_out PVOID *LocalityInfo,
    __deref_out PVOID *GroupAssignment
    )

/*++

Routine Description:

    Wrapper for CmpFindNumaData.  Traverses a particular control set and
    gets the NUMA node locality information and group assignment data.

Arguments:

    Handle - The hive reference returned by OslLoadAndInitializeHive.

    LocalityInfo - Returns the pointer to the NUMA node locality information.

    GroupAssignment - Returns the pointer to the NUMA node group assignment
            data.

Return Value:

    STATUS_INVALID_PARAMETER if any parameter is invalid.
    STATUS_SUCCESS otherwise, regardless if valid NUMA information is found.

--*/

{

    ULONG AssignmentLength;
    BOOLEAN DataValid;
    ULONG ExpectedLength;
    ULONG InfoLength;
    ULONG NodeCount;

    if ((HiveTableEntries <= Handle) ||
        (HiveTable[Handle] == NULL) ||
        (LocalityInfo == NULL) ||
        (GroupAssignment == NULL)) {

        return STATUS_INVALID_PARAMETER;
    }

    CmpFindNumaData(&HiveTable[Handle]->Hive.Hive,
                    HiveTable[Handle]->ControlSet,
                    &InfoLength,
                    LocalityInfo,
                    &AssignmentLength,
                    GroupAssignment);

    //
    // Check if the returned locality information has correct length.
    // The format of the NUMA locality information is:
    //
    //    Node count (N)                        - 1 ULONG
    //    Table index to proximity ID mapping   - N ULONGs
    //    NxN matrix of distance data           - N * N USHORTs
    //

    DataValid = FALSE;
    if ((*LocalityInfo != NULL) && (InfoLength >= sizeof(ULONG))) {
        NodeCount = *((PULONG)(*LocalityInfo));
        ExpectedLength = sizeof(ULONG) +
                         (NodeCount * sizeof(ULONG)) +
                         (NodeCount * NodeCount * sizeof(USHORT));

        if (InfoLength == ExpectedLength) {
            DataValid = TRUE;
        }
    }

    if (DataValid == FALSE) {
        *LocalityInfo = NULL;
    }

    //
    // Check if the returned group assignment data has correct length.
    // The format of the NUMA group assignment data is:
    //
    //    Node count (N)                              - 1 ULONG
    //    N pairs of proximity id and group number    - 2 * N ULONGs
    //

    DataValid = FALSE;
    if ((*GroupAssignment != NULL) && (AssignmentLength >= sizeof(ULONG))) {
        NodeCount = *((PULONG)(*GroupAssignment));
        ExpectedLength = sizeof(ULONG) * (1 + (2 * NodeCount));
        if (AssignmentLength == ExpectedLength) {
            DataValid = TRUE;
        }
    }

    if (DataValid == FALSE) {
        *GroupAssignment = NULL;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
OslHiveFindNlsData (
    __in ULONG Handle,
    __out PUNICODE_STRING AnsiFilename,
    __out PUNICODE_STRING OemFilename,
    __out PUNICODE_STRING CaseTableFilename
    )

/*++

Routine Description:

    Wrapper for CmpFindNlsData.  Traverses a particular control set and
    determines the filenames for the NLS data files that need to be loaded.

Arguments:

    Handle - The hive reference returned by OslLoadAndInitializeHive.

    AnsiFileName - Returns the name of the Ansi codepage file (c_1252.nls)

    OemFileName -  Returns the name of the OEM codepage file  (c_437.nls)

    CaseTableFileName - Returns the name of the Unicode upper/lowercase
            table for the language (l_intl.nls)

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if Handle is invalid or if any filename string
        is NULL.
    STATUS_UNSUCCESSFUL if CmpFindNLSData failed.

--*/

{

    BOOLEAN BoolRet;
    NTSTATUS Status;

    if ((HiveTableEntries <= Handle) || (HiveTable[Handle] == NULL) ||
        (AnsiFilename == NULL) || (OemFilename == NULL) ||
        (CaseTableFilename == NULL)) {

        return STATUS_INVALID_PARAMETER;
    }

    BoolRet = CmpFindNLSData(&HiveTable[Handle]->Hive.Hive,
                             HiveTable[Handle]->ControlSet,
                             AnsiFilename,
                             OemFilename,
                             CaseTableFilename);

    if (BoolRet == FALSE) {
        Status = STATUS_NOT_FOUND;

    } else {
        Status = STATUS_SUCCESS;
    }

    return Status;
}

NTSTATUS
OslHiveFindSkuType (
    __in ULONG Handle,
    __out PUNICODE_STRING SkuType
    )

/*++

Routine Description:

    Wrapper for CmpFindSkuType.  Traverses a particular control set and
    determines the Sku type name (e.g., "WinNT" or "ServerNT")

Arguments:

    Handle - The hive reference returned by OslLoadAndInitializeHive.

    SkuType - Supplies a pointer that receives the name of the SKU
        (e.g., "WinNT" or "ServerNT")

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if Handle is invalid or if any filename string
        is NULL.
    STATUS_UNSUCCESSFUL if CmpFindSkuType failed.

--*/

{

    BOOLEAN BoolRet;
    NTSTATUS Status;

    if ((HiveTableEntries <= Handle) || (HiveTable[Handle] == NULL) ||
        (SkuType == NULL)) {

        return STATUS_INVALID_PARAMETER;
    }

    BoolRet = CmpFindSkuType(&HiveTable[Handle]->Hive.Hive,
                             HiveTable[Handle]->ControlSet,
                             SkuType);

    if (BoolRet == FALSE) {
        Status = STATUS_NOT_FOUND;

    } else {
        Status = STATUS_SUCCESS;
    }

    return Status;
}

VOID
OslHiveQueryBugcheckOptions (
    __in ULONG SystemHiveHandle,
    __out PBOOLEAN BugcheckDisabled,
    __out PBOOLEAN EmoticonDisabled,
    __out PBOOLEAN DisplayParameters
    )

/*++

Routine Description:

    Wrapper for CmpQueryBugcheckOptions. Traverses a particular control set to
    query bugcheck related key values. The key is:

    HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\CrashControl\

    Values to query: DisplayDisabled, DisableEmoticon, DisplayParameters

Arguments:

    SystemHiveHandle - Supplies a handle to the system hive.

    BugcheckDisabled - Receives value of the "DisplayDisabled" key value if
        present.

    EmoticonDisabled - Receives value of the "DisableEmoticon" key value if
        present.

    DisplayParameters - Receives value of the "DisplayParameters" key value if
        present.

Return Value:

    None.

--*/

{

    HCELL_INDEX ControlSetIndex;

    ControlSetIndex = HiveTable[SystemHiveHandle]->ControlSet;
    if (ControlSetIndex == HCELL_NIL) {
        *BugcheckDisabled = FALSE;
        *EmoticonDisabled = FALSE;
        *DisplayParameters = FALSE;
        return;
    }

    CmpQueryBugcheckOptions(&HiveTable[SystemHiveHandle]->Hive.Hive,
                            ControlSetIndex,
                            BugcheckDisabled,
                            EmoticonDisabled,
                            DisplayParameters);

    return;
}


NTSTATUS
OslHiveGetDriverLoadPolicy (
    __in ULONG Handle,
    __out PULONG DriverLoadPolicy
    )

/*++

Routine Description:

    This routine reads the driver load policy from the registry and returns the
    value to the caller. The policy is stored in the following registry key:

    HKLM\System\CurrentControlSet\Control\EarlyLaunch\DriverLoadPolicy

Arguments:

    Handle - Supplies a handle to the system registry hive.

    DriverLoadPolicy - Supplies a pointer to a variable that receives the driver
        load policy state.

Return Value:

    NTSTATUS Code.

--*/

{

    HCELL_INDEX ControlSet;
    PHHIVE Hive;
    NTSTATUS Status;

    Hive = &HiveTable[Handle]->Hive.Hive;
    ControlSet = HiveTable[Handle]->ControlSet;
    Status = CmpGetDriverLoadPolicy(Hive, ControlSet, DriverLoadPolicy);

    Status = BlSecureBootFilterRegistryDWordInActivePolicy(
                    Status,
                    &DriverLoadPolicySubKeyPath,
                    &DriverLoadPolicyValue,
                    DriverLoadPolicy);

    if (!NT_SUCCESS(Status)) {
        Status = BlSecureBootFilterRegistryDWordInActivePolicy(
                        Status,
                        &DriverLoadPolicySubKeyPathFallback,
                        &DriverLoadPolicyValue,
                        DriverLoadPolicy);
    }

    return Status;
}

NTSTATUS
OslHiveGetSidSharingPolicy (
    __in ULONG SystemHiveHandle,
    __out PULONG SidSharingPolicy
    )

/*++

Routine Description:

    Wrapper for CmpGetSidSharingPolicy. Reads the SYSTEM hive and returns SID
    sharing policy state, which is stored at the following registry key:

    HKLM\System\CurrentControlSet\Control\SRM\SidSharingPolicy

Arguments:

    SystemHiveHandle - Supplies a handle to the system hive.

    SidSharingPolicy - Supplies a pointer to a variable that receives the SID
        sharing policy state.

Return Value:

    NTSTATUS Code.

--*/

{

    HCELL_INDEX ControlSetIndex;

    ControlSetIndex = HiveTable[SystemHiveHandle]->ControlSet;
    if (ControlSetIndex == HCELL_NIL) {
        return STATUS_UNSUCCESSFUL;
    }

    return CmpGetSidSharingPolicy(&HiveTable[SystemHiveHandle]->Hive.Hive,
                                  ControlSetIndex,
                                  SidSharingPolicy);
}

BOOLEAN
OslHiveIsLkgEnabled (
    __in ULONG SystemHiveHandle
    )

/*++

Routine Description:

    Wrapper for CmpIsLkgEnabled. Traverses a particular control set to
    determine if last known good is enabled by checking the value of:

    HKLM\SYSTEM\CurrentControlSet\Control\SessionManager\Configuration Manager
        \LastKnownGood\Enabled

Arguments:

    SystemHiveHandle - Supplies a handle to the system hive.

Return Value:

    TRUE if the key was present.

    FALSE otherwise.

--*/

{

    HCELL_INDEX ControlSetIndex;

    ControlSetIndex = HiveTable[SystemHiveHandle]->ControlSet;
    if (ControlSetIndex == HCELL_NIL) {
        return FALSE;
    }

    return CmpIsLkgEnabled(&HiveTable[SystemHiveHandle]->Hive.Hive,
                           ControlSetIndex);
}

BOOLEAN
OslGetExtendedStateFlags (
    __in ULONG SystemHiveHandle,
    __out PULONG Flags
    )

/*++

Routine Description:

    Wrapper for CmpGetXsaveFeatureFlags. Traverses a particular
    control set to retrieve the bitmask of XSAVE sub-features that
    are disabled via Flags

Arguments:

    SystemHiveHandle - Supplies a handle to the system hive.

    Flags - Supplies a pointer to the variable that retrieves the value of
        HKLM\System\CurrentControlSet\Control\Session Manager\Kernel
        \Extended State\Flags

Return Value:

    TRUE if the key was present.

    FALSE otherwise.

--*/

{

    HCELL_INDEX ControlSetIndex;

    ControlSetIndex = HiveTable[SystemHiveHandle]->ControlSet;
    if (ControlSetIndex == HCELL_NIL) {
        return FALSE;
    }

    return CmpGetXsaveFeatureFlags(&HiveTable[SystemHiveHandle]->Hive.Hive,
        ControlSetIndex,
        Flags);

}

BOOLEAN
OslGetExtendedStateAllowedFeatures (
    __in ULONG SystemHiveHandle,
    __out PULONG64 AllowedFeatures
    )

/*++

Routine Description:

    Wrapper for CmpGetExtendedStateAllowedFeatures. Traverses
    a particular control set and retrieves the bitmask of Extended State
    User Mode features enabled by the OS if supported by the CPU

Arguments:

    SystemHiveHandle - Supplies a handle to the system hive.

    AllowedFeatures - Supplies a pointer to the variable that retrieves
        the value of HKLM\System\CurrentControlSet\Control\Session Manager
        \Kernel\Extended State\AllowedFeatures

Return Value:

    TRUE if the key was present.

    FALSE otherwise.

--*/

{

    HCELL_INDEX ControlSetIndex;

    ControlSetIndex = HiveTable[SystemHiveHandle]->ControlSet;
    if (ControlSetIndex == HCELL_NIL) {
        return FALSE;
    }

    return CmpGetExtendedStateAllowedFeatures(&HiveTable[SystemHiveHandle]->Hive.Hive,
        ControlSetIndex,
        AllowedFeatures);
}

BOOLEAN
OslGetDriverVerifierLevelFlags (
    __in ULONG SystemHiveHandle,
    __out PULONG Flags
    )

/*++

Routine Description:

    This routine is a wrapper for CmpGetDriverVerifierLevelFlags. It returns
    the value of the Driver Verifier level flags by retrieving the value of:

    HKLM\System\CurrentControlSet\Control\Session Manager\Memory Management
        \VerifyDriverLevel.

Arguments:

    SystemHiveHandle - Supplies a handle to the system hive.

Return Value:

    TRUE if the key was present.

    FALSE otherwise.

--*/

{

    HCELL_INDEX ControlSetIndex;

    ControlSetIndex = HiveTable[SystemHiveHandle]->ControlSet;
    if (ControlSetIndex == HCELL_NIL) {
        return FALSE;
    }

    return CmpGetDriverVerifierLevelFlags(&HiveTable[SystemHiveHandle]->Hive.Hive,
                                          ControlSetIndex,
                                          Flags);
}

BOOLEAN
OslGetDriverVerifierOptionFlags (
    __in ULONG SystemHiveHandle,
    __out PULONG Flags
    )

/*++

Routine Description:

    This routine is a wrapper for CmpGetDriverVerifierOptionFlags. It returns
    the value of the Driver Verfier option flags by retreiving the value of:

    HKLM\System\CurrentControlSet\Control\Session Manager\Memory Management
        \VerifierOptions.

Arguments:

    SystemHiveHandle - Supplies a handle to the system hive.

Return Value:

    TRUE if the key was present.

    FALSE otherwise.

--*/

{

    HCELL_INDEX ControlSetIndex;

    ControlSetIndex = HiveTable[SystemHiveHandle]->ControlSet;
    if (ControlSetIndex == HCELL_NIL) {
        return FALSE;
    }

    return CmpGetDriverVerifierOptionFlags(&HiveTable[SystemHiveHandle]->Hive.Hive,
                                           ControlSetIndex,
                                           Flags);
}

NTSTATUS
OslDisableDriverVerifier (
    __in ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine is a wrapper for HiveDisableDriverVerifier. It disables the
    Driver Verifier by clearing the following registry values:

    HKLM\System\CurrentControlSet\Control\Session Manager\Memory Management\
        VerifyDriverLevel
        VerifyDrivers

Arguments:

    SystemHiveHandle - Supplies a handle to the system hive.

Return Value:

    NT status code.

--*/

{

    return HiveDisableDriverVerifier(SystemHiveHandle);
}

BOOLEAN
OslGetBootDriverFlagsFromRegistry (
    __in ULONG Handle,
    __in_opt LPGUID HardwareConfigurationId,
    __out PULONG BootDriverFlags
    )

/*++

Routine Description:

    This routine reads and returns the value for the boot driver flags key
    in the registry.

Arguments:

    Handle - Supplies a handle to the system hive.

    HardwareConfigurationId - Optionally supplies a GUID identifier for the
        hardware configuration to retrieve the flags from.

    BootDriverFlags - Supplies a pointer to a variable that receives the value
        of the boot driver flags.

Return Value:

    TRUE if the key value could be successfully retrieved.

    FALSE otherwise.

--*/

{

    HCELL_INDEX ControlSet;
    PHHIVE Hive;
    ULONGLONG SafemodeType;
    NTSTATUS Status;

    Hive = &HiveTable[Handle]->Hive.Hive;

    //
    // Try to retrieve a BootDriverFlags value that has been updated by sysprep
    // to only contain the bits known to be needed to boot this
    // system.  This updated value is stored with a hardware configuration, so
    // it can only be retrieved if a hardware configuration is provided.
    //

    if (HardwareConfigurationId != NULL) {

        //
        // Check to see if safe mode boot has been selected.  If the system is
        // booting in safe mode, the default boot driver flags value should be
        // used. If the system is not booting in safe mode, an attempt to get
        // BCDE_OSLOADER_TYPE_SAFEBOOT will return an error.
        //

        Status = BlGetApplicationOptionInteger(BCDE_OSLOADER_TYPE_SAFEBOOT,
                                               &SafemodeType);

        if (!NT_SUCCESS(Status)) {
            if (CmpGetHardwareConfigurationBootDriverFlags(Hive,
                                                           HardwareConfigurationId,
                                                           BootDriverFlags) == TRUE) {

                return TRUE;
            }
        }
    }

    //
    // If there was no updated BootDriverFlags value or there was an error
    // retrieving it, fall back to the default BootDriverFlags value.
    //
    // The default BootDriverFlags will also be used if the system is booting
    // in safe mode.
    //

    ControlSet = HiveTable[Handle]->ControlSet;
    return CmpGetDefaultBootDriverFlags(Hive, ControlSet, BootDriverFlags);
}

BOOLEAN
OslGetErrataFileNameFromRegistry (
    __in ULONG Handle,
    __out PUNICODE_STRING Name
    )

/*++

Routine Description:

    This routine reads and returns the name of INF file containing the
    errata manager database.

Arguments:

    Handle - The hive reference returned by OslLoadAndInitializeHive.

    Name - Supplies a pointer to a variable that receives the name of the errata
        manager database.

Return Value:

    TRUE if the name of the errata manager database could be retreived.

    FALSE otherwise.

--*/

{

    HCELL_INDEX ControlSet;
    PHHIVE Hive;

    Hive = &HiveTable[Handle]->Hive.Hive;
    ControlSet = HiveTable[Handle]->ControlSet;
    return CmpGetErrataFileNameFromRegistry(Hive, ControlSet, Name);
}

NTSTATUS
OslAttachHiveToLoaderBlock (
    __in DEVICE_ID DeviceId,
    _When_((Flags & LHB_SYSTEM_HIVE) != 0, __in)
    _When_((Flags & LHB_SYSTEM_HIVE) == 0, __in_opt)
        PUNICODE_STRING OsFilePath,
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in ULONG  Flags,
    __in __nullterminated PWCHAR FilePath,
    __in __nullterminated PWCHAR RegistryParent,
    __in __nullterminated PWCHAR RegistryName,
    __out_opt PULONG Handle
    )

/*++

Routine Description:

    This routine loads and verifies a registry hive. When successful, a new
    loader hive block is created and inserted in the loader parameter block.
    When a hive is attached to the loader block, it's gonna be mounted and
    used by kernel.

Arguments:

    OsDeviceId - Supplies handle to a device.

    OsFilePath - Supplies a pointer to the file path of the OS's system root
        directory.

    LoaderBlock - Supplies loader block to which this hive will be attached.

    Flags - Supplies flags for the attached hive. The caller must specify one
        (and only one) from the following flags:

            LHB_SYSTEM_HIVE - Hive is located in the
                \SystemRoot\System32\Config\ directory

            LHB_BOOT_PARTITION - Hive is located on the BOOT partition.

            LHB_SYSTEM_PARTITION - Hive is located in the SYSTEM partition.

    FilePath - Supplies file path of the hive file in the above specified
        location.

    RegistryParent - Supplies the reg keynode to which this hive will be
        attached. Currently the valid choices are MACHINE or USER, but it's
        possible that other key nodes will be introduced

    RegistryName - Supplies the key node that'll represent the loaded hive in
        kernel. (e.g. L"BCD000"). It's expected that the caller won't call this
        function twice with the same value in RegName parameter.

    Handle - Supplies a pointer to the handle for the input hive. If the caller
        needs to use the hive to be attached on the loader side this handle
        should be present. Otherwise NULL indicates that only the attach to the
        loader block is requested and no other operation is going to be
        performed on the hive

 Return Value:

    NT Status code.

--*/

{

    ULONG FilePathLength;
    PLOADER_HIVE_BLOCK HiveBlock;
    PWCHAR HivePath;
    PWCHAR HivePathBuffer;
    UINTN HivePathSize;
    ULONG Length;
    ULONG RegistryParentLength;
    ULONG RegistryNameLength;
    NTSTATUS Status;
    PWCHAR TempPath;

    HiveBlock = NULL;
    HivePathBuffer = NULL;

    if ((Flags == 0) || ((Flags & LHB_VALID_FLAGS) != Flags)) {
        Status = STATUS_INVALID_PARAMETER;
        goto AttachHiveToLoaderBlockEnd;
    }

    if ((FilePath == NULL) || (RegistryParent == NULL)) {
        Status = STATUS_INVALID_PARAMETER;
        goto AttachHiveToLoaderBlockEnd;
    }

    //
    // If the hive is a system hive, build the path to the system hive
    // directory with the file name appended. Otherwise, the supplied file path
    // is the full path to the hive.
    //

    if (CHECK_FLAG(Flags, LHB_SYSTEM_HIVE) != FALSE) {
        HivePathSize = OsFilePath->Length + sizeof(SYSTEM_HIVE_PATH);
        HivePathSize += (wcslen(FilePath) * sizeof(WCHAR));
        HivePathBuffer = BlMmAllocateHeap(HivePathSize);
        if (HivePathBuffer == NULL) {
            Status = STATUS_NO_MEMORY;
            goto AttachHiveToLoaderBlockEnd;
        }

        swprintf_s(HivePathBuffer,
                   HivePathSize / sizeof(WCHAR),
                   L"%s%s%s",
                   OsFilePath->Buffer,
                   SYSTEM_HIVE_PATH,
                   FilePath);

        HivePath = HivePathBuffer;

    } else {
        HivePath = FilePath;
    }

    FilePathLength = (ULONG)wcslen(FilePath) + 1;
    RegistryParentLength = (ULONG)wcslen(RegistryParent) + 1;
    RegistryNameLength = (ULONG)wcslen(RegistryName) + 1;

    //
    // The complete path length is restricted to CM_MAX_NAME. It's pretty
    // much an arbitrarily made choice based on what CM currently expects.
    // If the CM_MAX_NAME value turns out to be insufficient appropriate part
    // of registry code must be changed.
    //

    if (FilePathLength >= CM_MAX_NAME) {
        Status = STATUS_INVALID_PARAMETER;
        goto AttachHiveToLoaderBlockEnd;
    }

    //
    // Complete registry path length is restricted to CM_MAX_NAME.
    //

    Length = RegistryNameLength + RegistryParentLength;
    Length += sizeof(L"\\REGISTRY") / sizeof(WCHAR);
    if (Length >= CM_MAX_NAME) {
        Status = STATUS_INVALID_PARAMETER;
        goto AttachHiveToLoaderBlockEnd;
    }

    Length = FilePathLength + RegistryParentLength + RegistryNameLength;
    Length *= (ULONG)sizeof(WCHAR);
    Length += (ULONG)sizeof(LOADER_HIVE_BLOCK);
    HiveBlock = BlMmAllocateHeap(Length);
    if (HiveBlock == NULL) {
        Status = STATUS_NO_MEMORY;
        goto AttachHiveToLoaderBlockEnd;
    }

    RtlZeroMemory(HiveBlock, Length);
    Status = HiveLoadHiveFromLocation(DeviceId,
                                      HivePath,
                                      &HiveBlock->RegistryBase,
                                      &HiveBlock->RegistryLength,
                                      &HiveBlock->RecoveryInfo,
                                      Handle);

    if (!NT_SUCCESS(Status)) {
        goto AttachHiveToLoaderBlockEnd;
    }

    HiveBlock->Flags = Flags;

    //
    // Copy the rest of the parameters to the hive block.
    //

    TempPath = (PWCHAR)(HiveBlock + 1);
    HiveBlock->FilePath = &TempPath[0];
    wcscpy_s(&TempPath[0], FilePathLength, &FilePath[0]);
    TempPath += FilePathLength;
    HiveBlock->RegistryParent = &TempPath[0];
    wcscpy_s(&TempPath[0], RegistryParentLength, &RegistryParent[0]);
    TempPath += RegistryParentLength;
    HiveBlock->RegistryName = &TempPath[0];
    wcscpy_s(&TempPath[0], RegistryNameLength, &RegistryName[0]);
    InsertHeadList(&LoaderBlock->Extension->AttachedHives, &HiveBlock->Entry);

AttachHiveToLoaderBlockEnd:
    if (!NT_SUCCESS(Status)) {
        if (HiveBlock != NULL) {
            BlMmFreeHeap(HiveBlock);
        }
    }

    if (HivePathBuffer != NULL) {
        BlMmFreeHeap(HivePathBuffer);
    }

    return Status;
}

NTSTATUS
OslHiveReadWriteDwordValues(
    _In_ ULONG Handle,
    _In_ BOOLEAN Write,
    _In_ PCWSTR KeyName,
    _In_ PCWSTR SubKeyName,
    _When_(Write, _In_) _When_(!Write, _Out_) PULONG ValueData
    )

    /*++

Routine Description:

    This routine checks for a REG_DWORD value set under the provided key
    with a value that is of type REG_DWORD and not 0 when reading.
    When writing, it will overwrite any REG_DWORD value with the provided value.

Arguments:

    Handle - Supplies a handle to the system hive.

    Write - Supplies a boolean that is TRUE for writing a value, and FALSE
    when reading a value.

    KeyName - Supplies the name of a control key.

    SubKeyName - Supplies the name of the Subkey uner KeyName.

    ValueData - Supplies a buffer returning the information read (1 or 0)

Return Value:

    NT status code.

    --*/

{
    HCELL_INDEX Control;
    HCELL_INDEX ControlSet;
    HCELL_INDEX ControlKey;
    HCELL_INDEX ScenarioKey;
    PULONG Data = NULL;
    ULONG DataSize;
    PHHIVE Hive;
    PCM_KEY_NODE HiveCell;
    NTSTATUS Status;
    UNICODE_STRING UnicodeString;
    PCM_KEY_VALUE Value;

    // Initialize with 0 when reading meaning that no configuration was found.
    if (Write == FALSE) {
        *ValueData = 0;
    }

    if ((HiveTableEntries <= Handle) ||
        (HiveTable[Handle] == NULL) ||
        (KeyName == NULL) ||
        (SubKeyName == NULL) ||
        (ValueData == NULL)) {

        Status = STATUS_INVALID_PARAMETER;
        goto ReadWriteDwordValuesEnd;
    }

    Status = STATUS_OBJECT_NAME_NOT_FOUND;
    Hive = &HiveTable[Handle]->Hive.Hive;
    ControlSet = HiveTable[Handle]->ControlSet;
    HiveCell = (PCM_KEY_NODE)HvGetCell(Hive, ControlSet, NULL);
    if (HiveCell == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto ReadWriteDwordValuesEnd;
    }

    RtlInitUnicodeString(&UnicodeString, L"Control");
    Control = CmpFindSubKeyByName(Hive, HiveCell, &UnicodeString);
    if (Control == HCELL_NIL) {
        goto ReadWriteDwordValuesEnd;
    }

    HiveCell = (PCM_KEY_NODE)HvGetCell(Hive, Control, NULL);
    if (HiveCell == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto ReadWriteDwordValuesEnd;
    }

    RtlInitUnicodeString(&UnicodeString, KeyName);
    ControlKey = CmpFindSubKeyByName(Hive, HiveCell, &UnicodeString);
    if (ControlKey == HCELL_NIL) {
        goto ReadWriteDwordValuesEnd;
    }

    HiveCell = (PCM_KEY_NODE)HvGetCell(Hive, ControlKey, NULL);
    if (HiveCell == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto ReadWriteDwordValuesEnd;
    }

    RtlInitUnicodeString(&UnicodeString, SubKeyName);
    ScenarioKey = CmpFindSubKeyByName(Hive, HiveCell, &UnicodeString);
    if (ControlKey == HCELL_NIL) {
        goto ReadWriteDwordValuesEnd;
    }

    HiveCell = (PCM_KEY_NODE)HvGetCell(Hive, ScenarioKey, NULL);
    if (HiveCell == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto ReadWriteDwordValuesEnd;
    }

    //
    // Enumerate all values under that key.
    // Check only REG_DWORD values.
    //

    if (HiveCell->ValueList.Count == 0) {
        goto ReadWriteDwordValuesEnd;
    }

    HCELL_INDEX ValueList = HiveCell->ValueList.List;
    PCELL_DATA List = (PCELL_DATA)HvGetCell(Hive, ValueList, NULL);

    for (ULONG Current = 0; Current < HiveCell->ValueList.Count; Current++) {
        Value = (PCM_KEY_VALUE)HvGetCell(Hive, List->u.KeyList[Current], NULL);
        if (Value == NULL) {
            Status = STATUS_REGISTRY_CORRUPT;
            goto ReadWriteDwordValuesEnd;
        }

        if (Value->Type == REG_DWORD) {
            DataSize = CmpGetHKeyValueRealSize(Value->DataLength);

            if (CmpIsHKeyValueSmall(Value->DataLength)) {
                if (DataSize == sizeof(ULONG)) {
                    Data = (PULONG)((struct _CELL_DATA *)&Value->Data);
                    if (Data == NULL) {
                        continue;
                    }

                    if (Write == FALSE) {
                        if (*Data != 0) {
                            // Found at least 1 REG_DWORD set to something different than 0.
                            // Returning first value found.
                            *ValueData = *Data;
                            Status = STATUS_SUCCESS;
                            goto ReadWriteDwordValuesEnd;
                        }
                    }
                    else {
                        // Write - Set all found values to the provided value.
                        *Data = *ValueData;
                    }
                }
            }
        }
    }

    Status = STATUS_SUCCESS;

ReadWriteDwordValuesEnd:

    return Status;
}

NTSTATUS
OslHiveReadWriteControlDword (
    __in ULONG Handle,
    __in BOOLEAN Write,
    __in PWSTR KeyName,
    __in PWSTR ValueName,
    _When_(Write, _In_) _When_(!Write, _Out_) PULONG ValueData
    )

/*++

Routine Description:

    This routine reads or writes the DWORD value of the supplied control key.

Arguments:

    Handle - Supplies a handle to the system hive.

    Write - Supplies a boolean that is TRUE for writing a value, and FALSE
        when reading a value.

    KeyName - Supplies the name of a control key.

    ValueName - Supplies the name of the value to read or write.

    ValueData - Supplies a buffer containing the information to write, or
        returning the information read.

Return Value:

    NT status code.

--*/

{

    HCELL_INDEX Control;
    HCELL_INDEX ControlKey;
    HCELL_INDEX ControlSet;
    PULONG Data;
    ULONG DataSize;
    PHHIVE Hive;
    PCM_KEY_NODE HiveCell;
    NTSTATUS Status;
    UNICODE_STRING UnicodeString;
    PCM_KEY_VALUE Value;
    HCELL_INDEX ValueCell;

    if ((HiveTableEntries <= Handle) ||
        (HiveTable[Handle] == NULL) ||
        (KeyName == NULL) ||
        (ValueName == NULL) ||
        (ValueData == NULL)) {

        Status = STATUS_INVALID_PARAMETER;
        goto ReadWriteControlDwordEnd;
    }

    Status = STATUS_OBJECT_NAME_NOT_FOUND;
    Hive = &HiveTable[Handle]->Hive.Hive;
    ControlSet = HiveTable[Handle]->ControlSet;
    HiveCell = (PCM_KEY_NODE)HvGetCell(Hive, ControlSet, NULL);
    if (HiveCell == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto ReadWriteControlDwordEnd;
    }

    RtlInitUnicodeString(&UnicodeString, L"Control");
    Control = CmpFindSubKeyByName(Hive, HiveCell, &UnicodeString);
    if (Control == HCELL_NIL) {
        goto ReadWriteControlDwordEnd;
    }

    HiveCell = (PCM_KEY_NODE)HvGetCell(Hive, Control, NULL);
    if (HiveCell == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto ReadWriteControlDwordEnd;
    }

    RtlInitUnicodeString(&UnicodeString, KeyName);
    ControlKey = CmpFindSubKeyByName(Hive, HiveCell, &UnicodeString);
    if (ControlKey == HCELL_NIL) {
        goto ReadWriteControlDwordEnd;
    }

    HiveCell = (PCM_KEY_NODE)HvGetCell(Hive, ControlKey, NULL);
    if (HiveCell == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto ReadWriteControlDwordEnd;
    }

    RtlInitUnicodeString(&UnicodeString, ValueName);
    ValueCell = CmpFindValueByName(Hive, HiveCell, &UnicodeString);
    if (ValueCell == HCELL_NIL) {
        goto ReadWriteControlDwordEnd;
    }

    Value = (PCM_KEY_VALUE)HvGetCell(Hive, ValueCell, NULL);
    if (Value == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto ReadWriteControlDwordEnd;
    }

    if (Value->Type == REG_DWORD) {
        DataSize = CmpGetHKeyValueRealSize(Value->DataLength);
        if (CmpIsHKeyValueSmall(Value->DataLength)) {
            Data = (PULONG)((struct _CELL_DATA *)&Value->Data);

        } else {
            Data = (PULONG)HvGetCell(Hive, Value->Data, NULL);
            if (Data == NULL) {
                Status = STATUS_REGISTRY_CORRUPT;
                goto ReadWriteControlDwordEnd;
            }
        }

        if (Write == FALSE) {
            *ValueData = *Data;

        } else {
            *Data = *ValueData;
        }

        Status = STATUS_SUCCESS;
    }

ReadWriteControlDwordEnd:
    return Status;
}

NTSTATUS
OslHiveGetApiSetSchemaExtensions (
    __in ULONG Handle,
    __in PLIST_ENTRY ListHead
    )

/*++

Routine Description:

    This routine is a wrapper for CmpGetApiSetSchemaExtensions. The routine
    obtains the list of API set schema extensions to be loaded.

Arguments:

    Handle - Supplies a handle to the system hive.

    ListHead - Supplies a pointer to the list head.

Return Value:

    STATUS_SUCCESS.

--*/

{

    CmpGetApiSetSchemaExtensions(&HiveTable[Handle]->Hive.Hive,
                                 HiveTable[Handle]->ControlSet,
                                 ListHead);

    return STATUS_SUCCESS;
}

//
// ------------------------------------------------------------ Local Functions
//

PVOID
HiveAllocate (
    __in ULONG Length,
    __in BOOLEAN UseForIo,
    __in ULONG Tag
    )

/*++

Routine Description:

    Wrapper for hive allocation calls.

Arguments:

    Length - Supplies the size of block required in bytes.

    UseForIo - Supplies whether or not the memory is to be used for I/O
               (this is currently ignored).

    Tag - (currently ignored).

Return Value:

    Address of the allocated block of memory.
    NULL if unsuccessful.

--*/

{

    UNREFERENCED_PARAMETER(UseForIo);
    UNREFERENCED_PARAMETER(Tag);

    return BlMmAllocateHeap(Length);
}

VOID
HiveFree (
    __in PVOID MemoryBlock,
    __in ULONG GlobalQuotaSize
    )

/*++

Routine Description:

    Wrapper for hive de-allocation calls.

Arguments:

    MemoryBlock - Memory buffer to free (must be allocated from HiveAllocated).

    GlobalQuotaSize - currently ignored.

Return Value:

    None.

--*/

{

    UNREFERENCED_PARAMETER(GlobalQuotaSize);

    BlMmFreeHeap(MemoryBlock);
}

NTSTATUS
HiveDisableDriverVerifier (
    __in ULONG Handle
    )

/*++

Routine Description:

    This routine disables the usage of the driver verifier for the current
    boot by zeroing the VerifyMode key in the system hive.

    N.B. Setting this value only has affect on the current boot.

Arguments:

    Handle - Supplies the handle to the system hive.

Return Value:

    None.

--*/

{

    HCELL_INDEX Control;
    HCELL_INDEX ControlSet;
    PULONG Data;
    ULONG DataSize;
    PWCHAR DataString;
    UNICODE_STRING KeyName;
    PCM_KEY_NODE HiveCell;
    HCELL_INDEX MemoryManager;
    PHHIVE Hive;
    HCELL_INDEX SessionManager;
    NTSTATUS Status;
    PCM_KEY_VALUE Value;
    HCELL_INDEX ValueCell;

    if ((HiveTableEntries <= Handle) || (HiveTable[Handle] == NULL)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Zero the data value for
    // \Control\Session Manager\Memory Management\VerifyDriverlLevel and
    // set the data value for
    // \Control\Session Manager\Memory Management\VerifyDrivers to NULL.  It
    // is not considered a fatal error if either of the keys does not exist.
    //

    Status = STATUS_SUCCESS;
    Hive = &HiveTable[Handle]->Hive.Hive;
    ControlSet = HiveTable[Handle]->ControlSet;
    RtlInitUnicodeString(&KeyName, L"Control");
    HiveCell = (PCM_KEY_NODE)HvGetCell(Hive, ControlSet, NULL);
    if (HiveCell == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto DisableDriverVerifierEnd;
    }

    Control = CmpFindSubKeyByName(Hive, HiveCell, &KeyName);
    if (Control == HCELL_NIL) {
        goto DisableDriverVerifierEnd;
    }

    RtlInitUnicodeString(&KeyName, L"Session Manager");
    HiveCell = (PCM_KEY_NODE)HvGetCell(Hive, Control, NULL);
    if (HiveCell == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto DisableDriverVerifierEnd;
    }

    SessionManager = CmpFindSubKeyByName(Hive, HiveCell, &KeyName);
    if (SessionManager == HCELL_NIL) {
        goto DisableDriverVerifierEnd;
    }

    RtlInitUnicodeString(&KeyName, L"Memory Management");
    HiveCell = (PCM_KEY_NODE)HvGetCell(Hive, SessionManager, NULL);
    if (HiveCell == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto DisableDriverVerifierEnd;
    }

    MemoryManager = CmpFindSubKeyByName(Hive, HiveCell, &KeyName);
    if (MemoryManager == HCELL_NIL) {
        goto DisableDriverVerifierEnd;
    }

    RtlInitUnicodeString(&KeyName, L"VerifyDriverLevel");
    HiveCell = (PCM_KEY_NODE)HvGetCell(Hive, MemoryManager, NULL);
    if (HiveCell == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto DisableDriverVerifierEnd;
    }

    ValueCell = CmpFindValueByName(Hive, HiveCell, &KeyName);
    if (ValueCell != HCELL_NIL) {
        Value = (PCM_KEY_VALUE)HvGetCell(Hive, ValueCell, NULL);
        if (Value == NULL) {
            Status = STATUS_REGISTRY_CORRUPT;
            goto DisableDriverVerifierEnd;
        }

        if (Value->Type == REG_DWORD) {
            DataSize = CmpGetHKeyValueRealSize(Value->DataLength);
            if (CmpIsHKeyValueSmall(Value->DataLength)) {
                Data = (PULONG)((struct _CELL_DATA *)&Value->Data);

            } else {
                Data = (PULONG)HvGetCell(Hive, Value->Data, NULL);
                if (Data == NULL) {
                    Status = STATUS_REGISTRY_CORRUPT;
                    goto DisableDriverVerifierEnd;
                }
            }

            *Data = 0;
        }
    }

    RtlInitUnicodeString(&KeyName, L"VerifyDrivers");
    ValueCell = CmpFindValueByName(Hive, HiveCell, &KeyName);
    if (ValueCell != HCELL_NIL) {
        Value = (PCM_KEY_VALUE)HvGetCell(Hive, ValueCell, NULL);
        if (Value == NULL) {
            Status = STATUS_REGISTRY_CORRUPT;
            goto DisableDriverVerifierEnd;
        }

        if (Value->Type == REG_SZ) {
            DataSize = CmpGetHKeyValueRealSize(Value->DataLength);
            if (CmpIsHKeyValueSmall(Value->DataLength)) {
                DataString = (PWCHAR)((struct _CELL_DATA *)&Value->Data);

            } else {
                DataString = (PWCHAR)HvGetCell(Hive, Value->Data, NULL);
                if (DataString == NULL) {
                    Status = STATUS_REGISTRY_CORRUPT;
                    goto DisableDriverVerifierEnd;
                }
            }

            if (Value->DataLength >= sizeof(WCHAR)) {
                *DataString = L'\0';
                Value->DataLength = sizeof(WCHAR);
            }
        }
    }

DisableDriverVerifierEnd:
    return Status;
}

NTSTATUS
HiveGetLogInformation (
    __in DEVICE_ID DeviceId,
    __in PWSTR LogPath,
    __in PLARGE_INTEGER TimeStamp,
    __in ULONG MinimumLogSequence,
    __deref_out_bcount(*LogImageSize) PVOID *LogImage,
    __out PULONG LogImageSize,
    __out PULONG MaximumHiveSize,
    __out PULONG FirstLogSequence,
    __out PBOOLEAN UseLegacyRecovery
    )

/*++

Routine Description:

    This routine gets information about a log file.

Arguments:

    DeviceId - Supplies an identifier for the device that contains the file.

    LogPath - Supplies the path to the file.

    TimeStamp - Supplies a timestamp that the log must match in order to be
        valid.

    MinimumLogSequence - Supplies a minimum desired sequence value for the log
        file. The log's initial entry's sequence number must be at least this
        value in order for the log to be valid.

    LogImage - Supplies a buffer that receives a pointer to the log file image.

    LogImageSize - Supplies a buffer that receives the log image's size.

    MaximumHiveSize - Supplies a buffer that receives the maximum hive size
        stored in the log's entries.

    FirstLogSequence - Supplies a buffer that receives the sequence number
        found in the log's header.

    UseLegacyRecovery - Supplies a buffer that receives a Boolean value that
        indicates whether legacy recovery should be used.

Return Value:

    STATUS_SUCCESS if the log file was successfully processed.

    STATUS_REGISTRY_CORRUPT if the log file is determined to be inapplicable.
    Note that this does not necessarily mean that the associated hive is
    unloadable--it only implies that the current log cannot be used for
    recovery.

    Other NT status codes if callees of this routine fail.

--*/

{

    PHBASE_BLOCK BaseBlock;
    BOOLEAN DownlevelLogFormatInUse;
    ULONG LocalFirstLogSequence;
    PVOID LocalLogImage;
    ULONG LocalLogImageSize;
    ULONG LocalMaximumHiveSize;
    NTSTATUS Status;

    LocalLogImage = NULL;
    DownlevelLogFormatInUse = FALSE;

    //
    // Load the log image. The image has to contain at least a log header for
    // it to have any chance of being intelligible, so if no such header is
    // present, exit.
    //

    Status = BlImgLoadImage(DeviceId,
                            OSL_MEMORY_TYPE_REGISTRY_DATA,
                            LogPath,
                            &LocalLogImage,
                            &LocalLogImageSize,
                            IMAGE_FLAGS_NONE);

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    if (LocalLogImageSize < (ULONG)HLOG_HEADER_SIZE) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto Exit;
    }

    //
    // The log header must have a valid clustering factor and length, its
    // sequence numbers must match, and it must have a valid checksum. If one
    // or more of these properties does not hold, the log is inapplicable.
    //

    BaseBlock = (PHBASE_BLOCK)LocalLogImage;
    if ((BaseBlock->Cluster == 0) ||
        (BaseBlock->Length == 0) ||
        (BaseBlock->Sequence1 != BaseBlock->Sequence2) ||
        (HvpHeaderCheckSum(BaseBlock) != BaseBlock->CheckSum) ||
        (BaseBlock->TimeStamp.QuadPart != TimeStamp->QuadPart)) {

        Status = STATUS_REGISTRY_CORRUPT;
        goto Exit;
    }

    ASSERT(BaseBlock->Cluster == 1);

    if (BaseBlock->Type == HBASE_BLOCK_TYPE_INCREMENTAL_LOG) {

        //
        // If the base block indicates that the log has no valid entries, exit.
        //

        if (HvIsLogSequenceNumberAtLeastMinimum(BaseBlock->Sequence1,
                                                MinimumLogSequence) == FALSE) {

            Status = STATUS_REGISTRY_CORRUPT;
            goto Exit;
        }

        LocalFirstLogSequence = BaseBlock->Sequence1;

        //
        // Walk the entries in the log and find the one with the largest hive
        // size.
        //

        LocalMaximumHiveSize = HvGetLargestHiveSizeInLog(LocalLogImage,
                                                         LocalLogImageSize);

        //
        // If the hive has no entries with a valid hive size, then there is
        // nothing to apply in the log.
        //

        if (LocalMaximumHiveSize == 0) {
            Status = STATUS_REGISTRY_CORRUPT;
            goto Exit;
        }

    } else if (BaseBlock->Type == HBASE_BLOCK_TYPE_LEGACY_LOG) {
        LocalFirstLogSequence = BaseBlock->Sequence1;
        LocalMaximumHiveSize = BaseBlock->Length;
        DownlevelLogFormatInUse = TRUE;

    } else {
        Status = STATUS_REGISTRY_CORRUPT;
        goto Exit;
    }

    *LogImage = LocalLogImage;
    LocalLogImage = NULL;
    *LogImageSize = LocalLogImageSize;
    *MaximumHiveSize = LocalMaximumHiveSize;
    *FirstLogSequence = LocalFirstLogSequence;
    *UseLegacyRecovery = DownlevelLogFormatInUse;

Exit:
    if (LocalLogImage != NULL) {
        BlMmFreePages(LocalLogImage);
    }

    return Status;
}

NTSTATUS
HiveGetHiveLogInformation (
    __in DEVICE_ID DeviceId,
    __in_ecount(HIVE_MAX_LOG_COUNT) PWSTR *LogPaths,
    __in PLARGE_INTEGER TimeStamp,
    __in ULONG MinimumLogSequence,
    __in BOOLEAN PrimaryBaseBlockValid,
    __out PHIVE_LOG_INFORMATION LogInformation
    )

/*++

Routine Description:

    This routine retrieves information about a hive's log files.

Arguments:

    DeviceId - Supplies a device identifier handle for the device containing
        the hive's logs.

    LogPaths - Supplies an array of strings containing paths to the log files
        to attempt to load.

        This array is permitted to have a NULL element. Such an element
        serves as a sentinel; as soon as it is observed, this routine stops
        considering new log files.

    TimeStamp - Supplies a timestamp that the hive must match in order to be
        considered valid.

    MinimumLogSequence - Supplies a minimum desired sequence value for each log
        file. A log's initial entry's sequence number must be at least this
        value in order for the log to be valid.

    PrimaryBaseBlockValid - Supplies a Boolean value indicating whether the hive
        is being recovered because its primary base block had mismatched
        sequence numbers (TRUE) or because the header is totally invalid
        (FALSE).

    LogInformation - Supplies a log information block that will be populated
        by this routine.

Return Value:

    NT status code.

    This routine always initializes the log image pointers in the log
    information block. If more than one applicable log is found, the image
    pointers are stored in the array in the order they are to be applied.

--*/

{

    BOOLEAN DownlevelLogFormatInUse[HIVE_MAX_LOG_COUNT];
    PVOID ImageToFree;
    ULONG Index;
    ULONG LatestIndex;
    PVOID LocalLogImages[HIVE_MAX_LOG_COUNT];
    ULONG LocalLogImageSizes[HIVE_MAX_LOG_COUNT];
    ULONG LocalMaximumHiveSizes[HIVE_MAX_LOG_COUNT];
    ULONG LocalSequences[HIVE_MAX_LOG_COUNT];
    ULONG LocalLogIndices[HIVE_MAX_LOG_COUNT];
    NTSTATUS Status;
    ULONG ValidLogIndex;

    RtlZeroMemory(LocalLogImages, sizeof(LocalLogImages));

    //
    // If the caller indicated that the primary base block was not valid,
    // consider all possible log files.
    //

    if (PrimaryBaseBlockValid == FALSE) {
        MinimumLogSequence = 0;
    }

    DownlevelLogFormatInUse[0] = FALSE;
    DownlevelLogFormatInUse[1] = FALSE;

    ValidLogIndex = 0;
    for (Index = 0; Index < HIVE_MAX_LOG_COUNT; Index += 1) {
        if (LogPaths[Index] == NULL) {
            break;
        }

        //
        // Fill the next slot in the local buffers with information about
        // the current log file.
        //

        Status = HiveGetLogInformation(DeviceId,
                                       LogPaths[Index],
                                       TimeStamp,
                                       MinimumLogSequence,
                                       &LocalLogImages[ValidLogIndex],
                                       &LocalLogImageSizes[ValidLogIndex],
                                       &LocalMaximumHiveSizes[ValidLogIndex],
                                       &LocalSequences[ValidLogIndex],
                                       &DownlevelLogFormatInUse[ValidLogIndex]);

        LocalLogIndices[ValidLogIndex] = Index;

        //
        // If the current log was invalid, move to the next one.
        //

        if (!NT_SUCCESS(Status)) {

            NT_ASSERT(LocalLogImages[ValidLogIndex] == NULL);

            continue;
        }

        NT_ASSERT(LocalLogImages[ValidLogIndex] != NULL);
        NT_ASSERT(LocalLogImageSizes[ValidLogIndex] != 0);
        NT_ASSERT(LocalMaximumHiveSizes[ValidLogIndex] != 0);

        ValidLogIndex += 1;
    }

    //
    // If the caller indicated that the primary base block was invalid, and
    // there are multiple recoverable log files, only recover from the log
    // file that contains newer entries.
    //

    if ((ValidLogIndex == HIVE_MAX_LOG_COUNT) &&
        (PrimaryBaseBlockValid == FALSE)) {

        LatestIndex = HvpDetermineLatestLogFile(LocalSequences);

        //
        // The log image that is not chosen for recovery will not be returned
        // to the caller, so this routine must free it. Capture a pointer to
        // the image to make it safe to freely manipulate the local log
        // information arrays.
        //

        if (LatestIndex == 0) {
            ImageToFree = LocalLogImages[1];

        } else {
            ImageToFree = LocalLogImages[0];
        }

        NT_ASSERT(ImageToFree != NULL);

        //
        // Modify the local log information arrays so that they contain the
        // state they would have contained if the chosen log file had been
        // the only valid file, then change the number of valid log files
        // to 1 so that only this log will be returned to the caller.
        //

        LocalLogImages[0] = LocalLogImages[LatestIndex];
        LocalLogImageSizes[0] = LocalLogImageSizes[LatestIndex];
        LocalMaximumHiveSizes[0] = LocalMaximumHiveSizes[LatestIndex];
        LocalSequences[0] = LocalSequences[LatestIndex];
        LocalLogIndices[0] = LatestIndex;
        DownlevelLogFormatInUse[0] = DownlevelLogFormatInUse[LatestIndex];
        ValidLogIndex = 1;

        //
        // The unused log image will not be referenced again, so free it.
        //

        BlMmFreePages(ImageToFree);
    }

    //
    // Copy the log information out to the callee's buffer. Note that this
    // routine returns success even if no valid logs were found. The caller
    // must check the returned log image pointers to detect this case.
    //

    if (ValidLogIndex == 0) {

        LogInformation->LogImages[0] = LogInformation->LogImages[1] = NULL;

    } else if (ValidLogIndex == 1) {
        LogInformation->LogImages[0] = LocalLogImages[0];
        LogInformation->LogImageSizes[0] = LocalLogImageSizes[0];
        LogInformation->LogIndices[0] = LocalLogIndices[0];
        LogInformation->MaximumHiveSize = LocalMaximumHiveSizes[0];
        LogInformation->FirstLogSequence = LocalSequences[0];
        LogInformation->UseLegacyRecovery[0] = DownlevelLogFormatInUse[0];

        LogInformation->LogImages[1] = NULL;

    } else {

        NT_ASSERT(ValidLogIndex == HIVE_MAX_LOG_COUNT);
        NT_ASSERT(((DownlevelLogFormatInUse[0] != FALSE) &&
                   (DownlevelLogFormatInUse[1] != FALSE)) == FALSE);

        //
        // The log images need to be applied in order by sequence number.
        //

        if (LocalSequences[0] <= LocalSequences[1]) {
            LogInformation->LogImages[0] = LocalLogImages[0];
            LogInformation->LogImageSizes[0] = LocalLogImageSizes[0];
            LogInformation->LogIndices[0] = LocalLogIndices[0];
            LogInformation->UseLegacyRecovery[0] = DownlevelLogFormatInUse[0];

            LogInformation->LogImages[1] = LocalLogImages[1];
            LogInformation->LogImageSizes[1] = LocalLogImageSizes[1];
            LogInformation->LogIndices[1] = LocalLogIndices[1];
            LogInformation->UseLegacyRecovery[1] = DownlevelLogFormatInUse[1];

            LogInformation->FirstLogSequence = LocalSequences[0];

        } else {
            LogInformation->LogImages[0] = LocalLogImages[1];
            LogInformation->LogImageSizes[0] = LocalLogImageSizes[1];
            LogInformation->LogIndices[0] = LocalLogIndices[1];
            LogInformation->UseLegacyRecovery[0] = DownlevelLogFormatInUse[1];

            LogInformation->LogImages[1] = LocalLogImages[0];
            LogInformation->LogImageSizes[1] = LocalLogImageSizes[0];
            LogInformation->LogIndices[1] = LocalLogIndices[0];
            LogInformation->UseLegacyRecovery[1] = DownlevelLogFormatInUse[0];

            LogInformation->FirstLogSequence = LocalSequences[1];
        }

        //
        // The log with the entry with the largest hive size always
        // determines the maximum hive size, regardless of the order
        // in which the files are applied.
        //

        if (LocalMaximumHiveSizes[0] > LocalMaximumHiveSizes[1]) {
            LogInformation->MaximumHiveSize = LocalMaximumHiveSizes[0];

        } else {
            LogInformation->MaximumHiveSize = LocalMaximumHiveSizes[1];
        }
    }

    RtlZeroMemory(LocalLogImages, sizeof(LocalLogImages));
    return STATUS_SUCCESS;
}

NTSTATUS
HiveInitializeAndValidate (
    __out PLOADED_HIVE LoadedHive
    )

/*++

Routine Description:

    Attempts to initialize a loaded hive.  When successful, a consistency
    check is performed on the hive, zeroing volatile entries from the
    SubKeyCounts and SubKeyLists array in a key.

Arguments:

    LoadedHive - Supplies a pointer to a loaded hive structure.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_REGISTRY_CORRUPT if HvInitializeHive fails due to a hive
        inconsistency.
    STATUS_NO_MEMORY if memory can not be allocated during hive initialization.
    STATUS_UNSUCCESSFUL if a consistency check failed on the hive.

--*/

{

    PHBASE_BLOCK BaseBlock;
    PCMHIVE Hive;
    ULONG HiveCheckStatus;
    HIVE_LOAD_FAILURE HiveLoadFailure;
    HIVE_ROUTINES HiveRoutines;
    NTSTATUS Status;

    BaseBlock = LoadedHive->ImageBase;
    Hive = &(LoadedHive->Hive);

    ASSERT(LoadedHive->ImageSize >= HBLOCK_SIZE);
    ASSERT((BaseBlock->Length + HBLOCK_SIZE) >= HBLOCK_SIZE);

    //
    // Initialize the hive in memory.
    //

    RtlZeroMemory(Hive, sizeof(CMHIVE));
    RtlZeroMemory(&HiveLoadFailure, sizeof(HIVE_LOAD_FAILURE));
    RtlZeroMemory(&HiveRoutines, sizeof(HIVE_ROUTINES));
    HiveRoutines.AllocateRoutine = HiveAllocate;
    HiveRoutines.FreeRoutine = HiveFree;
    Hive->Hive.HiveLoadFailure = &HiveLoadFailure;
    HiveLoadFailure.Hive = &Hive->Hive;
    Status = HvInitializeHive(&Hive->Hive,
                              HINIT_MEMORY_INPLACE,
                              0,
                              HLOG_TYPE_NONE,
                              LoadedHive->ImageBase,
                              &HiveRoutines,
                              1,                    // Cluster size = 512 bytes.
                              NULL,
                              NULL,
                              NULL,
                              NULL,
                              NULL);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Perform a consistency check on the system hive.  Additionally, this call
    // will zero out the volatile entries from the SubKeyCounts and SubKeyLists
    // array in a key.  If this is not done, then CM initialization will assert
    // on each key.
    //

    HiveCheckStatus = CmCheckRegistry(Hive,
                                      (CM_CHECK_REGISTRY_LOADER_CLEAN |
                                       CM_CHECK_REGISTRY_HIVE_CHECK));

    Hive->Hive.HiveLoadFailure = NULL;
    if (HiveCheckStatus != 0) {
        return STATUS_UNSUCCESSFUL;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
HiveRecoverReadRoutine (
    __in PHHIVE Hive,
    __inout PVOID Context,
    __in ULONG FileOffset,
    __in ULONG DataSize,
    __deref_out_bcount(DataSize) PUCHAR *Data
    )

/*++

Routine Description:

    This routine "reads" data from a log file image and returns a pointer to
    it.

Arguments:

    Hive - Supplies the hive of interest.

    Context - Supplies a context block describing the available data.

    FileOffset - Supplies the offset to read.

    DataSize - Supplies the size of the data to read.

    Data - Supplies a buffer that receives a pointer to the desired data.

Return Value:

    NT status code.

--*/

{

    PHIVE_RECOVERY_READ_CONTEXT ReadContext;

    UNREFERENCED_PARAMETER(Hive);

    ReadContext = (PHIVE_RECOVERY_READ_CONTEXT)Context;
    if ((FileOffset + DataSize) < FileOffset) {
        return STATUS_INVALID_PARAMETER;
    }

    if ((FileOffset + DataSize) > ReadContext->LogSize) {
        return STATUS_END_OF_FILE;
    }

    *Data = (ReadContext->LogBase + FileOffset);
    return STATUS_SUCCESS;
}

NTSTATUS
HiveRecover (
    __inout PHHIVE Hive,
    __inout_bcount(HiveSize) PVOID HiveBase,
    __in ULONG HiveSize,
    __in_bcount(LogSize) PVOID LogBase,
    __in ULONG LogSize,
    __inout PULONG Sequence,
    __in BOOLEAN UseLegacyRecovery,
    __out PULONG LogCurrentOffset
    )

/*++

Routine Description:

    This routine recovers information in a log file by copying it into a hive's
    primary image.

    The caller is presumed to have validated the log file's header before
    calling this routine.

Arguments:

    Hive - Supplies a hive structure to initialize.

    HiveBase - Supplies a pointer to the base of a registry hive.

    HiveSize - Supplies the size of the registry hive, in bytes. This size must
        be at least large enough to hold a hive header.

    LogBase - Supplies a pointer to the base of a hive log.

    LogSize - Supplies the size of the registry hive log, in bytes. This size
        must be at least large enough to hold a log header.

    Sequence - Supplies a buffer that, on input, holds the sequence value
        expected to be at the start of the target log file and that, on output,
        receives the first sequence number that did not have a corresponding
        entry in the target log file.

    UseLegacyRecovery - Supplies a Boolean value that indicates whether to use
        legacy log file recovery when recovering this hive.

    LogCurrentOffset - Supplies a pointer to a buffer that upon success receives
                       the offset at which the next log entry should be written.

Return Value:

    NT status code.

--*/

{

    PHBASE_BLOCK BaseBlockHive;
    PHBASE_BLOCK BaseBlockLog;
    HIVE_RECOVERY_READ_CONTEXT Context;
    ULONG ClusterSize;
    ULONG EntriesRecovered;
    ULONG Length;
    NTSTATUS Status;

#if DBG
    PHBIN Bin;
#endif

    ASSERT(HiveHashLibraryInitialized != FALSE);

    BaseBlockHive = (PHBASE_BLOCK)HiveBase;
    BaseBlockLog = (PHBASE_BLOCK)LogBase;

    //
    // Assert that the parameters are valid and that the log file's base block
    // is valid.
    //

    __analysis_assume(HiveSize >= sizeof(HBASE_BLOCK));
    ASSERT(HiveSize >= sizeof(HBASE_BLOCK));
    ASSERT(LogSize >= (ULONG)HLOG_HEADER_SIZE);
    ASSERT(BaseBlockLog->Cluster == 1);
    ASSERT(BaseBlockLog->Length != 0);
    ASSERT(BaseBlockLog->Sequence1 == BaseBlockLog->Sequence2);
    ASSERT(HvpHeaderCheckSum(BaseBlockLog) == BaseBlockLog->CheckSum);
    ASSERT(BaseBlockLog->Cluster == 1);

    Status = RtlULongMult(BaseBlockLog->Cluster, HSECTOR_SIZE, &ClusterSize);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    if (ClusterSize > HiveSize) {
        return STATUS_REGISTRY_CORRUPT;
    }

    //
    // Match the timestamp values from the file headers.  If the hive header
    // is invalid, check the timestamp value from the first bin and copy the
    // log header into the hive image.
    //


    if (HvpHeaderCheckSum(BaseBlockHive) != BaseBlockHive->CheckSum) {

#if DBG

        Bin = (PHBIN)Add2Ptr(HiveBase, HBLOCK_SIZE);

        ASSERT(Bin->Signature == HBIN_SIGNATURE);
        ASSERT(Bin->TimeStamp.QuadPart == BaseBlockLog->TimeStamp.QuadPart);

#endif

        RtlCopyMemory(BaseBlockHive, BaseBlockLog, ClusterSize);
        BaseBlockHive->Type = HFILE_TYPE_PRIMARY;

    } else {

        ASSERT(BaseBlockHive->TimeStamp.QuadPart ==
               BaseBlockLog->TimeStamp.QuadPart);
    }

    //
    // Validate the hive length.
    //

    Length = BaseBlockHive->Length;
    if (((Length % HSECTOR_SIZE) != 0) ||
        (HiveSize < (Length + sizeof(HBASE_BLOCK)))) {

        return STATUS_REGISTRY_CORRUPT;
    }

    //
    // The hive is recoverable, so attempt to recover it. To do so, temporarily
    // set the base block pointer in the hive so that the recovery library will
    // be able to find the hive image. Also, temporarily set the allocate and
    // free routines so that the recovery process can allocate memory if
    // required.
    //

    Hive->BaseBlock = BaseBlockHive;
    Hive->Allocate = HiveAllocate;
    Hive->Free = HiveFree;
    Context.LogBase = LogBase;
    Context.LogSize = LogSize;

    //
    // N.B. The maximum hive size must exclude the size of the hive's base
    //      block.
    //

    if (UseLegacyRecovery == FALSE) {
        Status = HvApplyLogFile(Hive,
                                *Sequence,
                                HiveSize - HBLOCK_SIZE,
                                HiveRecoverReadRoutine,
                                &Context,
                                NULL,
                                Sequence,
                                &EntriesRecovered,
                                LogCurrentOffset);

    } else {
        Status = HvApplyLegacyLogFile(Hive,
                                      HiveRecoverReadRoutine,
                                      &Context,
                                      NULL);

        if (NT_SUCCESS(Status)) {
            *Sequence += 1;
            *LogCurrentOffset = 0;
        }
    }

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
HiveLoadHiveFromLocation (
    __in DEVICE_ID DeviceId,
    __in __nullterminated PWCHAR FilePath,
    __deref_out_bcount(*ImageSize) PVOID *ImageBase,
    __out PULONG ImageSize,
    __out PLOADER_HIVE_RECOVERY_INFO RecoveryInfo,
    __out_opt PULONG Handle
    )

/*++

Routine Description:

    This routine loads and verifies a registry hive.

Arguments:

    OsDeviceId - Supplies handle to a device.

    FilePath - Supplies the hive path relative to the root of the specified
        device.

    ImageBase - Supplies a pointer to a variable that receives an allocated
        pointer to the base of the hardware database. The caller is responsible
        for freeing this memory.

    ImageSize - Supplies a pointer to a variable that receives the size of the
        loaded database.

    RecoveryInfo - Supplies a pointer to a buffer that upon success receives
                   information about the recovery performed on the hive.

    Handle - Supplies a pointer to a variable that receives a handle to the
        loaded database.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY when an allocation fails.

    STATUS_NOT_FOUND if the specified hive does not exist.

--*/

{

    PWCHAR HiveLogPath1;
    UINTN HiveLogPath1Size;
    PWCHAR HiveLogPath2;
    UINTN HiveLogPath2Size;
    PWCHAR HivePath;
    UINTN HivePathSize;
    NTSTATUS Status;

    //
    // Allocate a buffer for the path to hive. Account for extra L'\\' and NULL
    // at the end.
    //

    HivePathSize = (UINTN)(wcslen(FilePath) + 2) * sizeof(WCHAR);
    HiveLogPath1Size = HivePathSize;
    HiveLogPath1Size += sizeof(HIVE_LOG1_EXTENSION);
    HiveLogPath2Size = HiveLogPath1Size;
    HivePath = BlMmAllocateHeap(HivePathSize);
    HiveLogPath2 = BlMmAllocateHeap(HiveLogPath2Size);
    HiveLogPath1 = BlMmAllocateHeap(HiveLogPath1Size);
    if ((HivePath == NULL) || (HiveLogPath1 == NULL) ||
        (HiveLogPath2 == NULL)) {

        Status = STATUS_NO_MEMORY;
        goto LoadHiveFromLocationEnd;
    }

    swprintf_s(HivePath,
               HivePathSize / sizeof(WCHAR),
               L"\\%s",
               FilePath);

    swprintf_s(HiveLogPath1,
               HiveLogPath1Size / sizeof(WCHAR),
               L"\\%s%s",
               FilePath,
               HIVE_LOG1_EXTENSION);

    swprintf_s(HiveLogPath2,
               HiveLogPath2Size / sizeof(WCHAR),
               L"\\%s%s",
               FilePath,
               HIVE_LOG2_EXTENSION);

    //
    // Attempt to load an initialize the hive.
    //

    Status = OslLoadAndInitializeHive(DeviceId,
                                      HivePath,
                                      FALSE,
                                      HiveLogPath1,
                                      HiveLogPath2,
                                      ImageBase,
                                      ImageSize,
                                      RecoveryInfo,
                                      Handle);

LoadHiveFromLocationEnd:
    if (HivePath != NULL) {
        BlMmFreeHeap(HivePath);
    }

    if (HiveLogPath2 != NULL) {
        BlMmFreeHeap(HiveLogPath2);
    }

    if (HiveLogPath1 != NULL) {
        BlMmFreeHeap(HiveLogPath1);
    }

    return Status;
}

NTSTATUS
OslGetSubkey (
    __in ULONG Handle,
    __in_opt PCM_KEY_NODE HiveCellParent,
    __in_opt PCWSTR SubkeyName,
    __out PCM_KEY_NODE *HiveCell
    )

/*++

Routine Description:

    This helper routine returns a subkey cell associated with a subkey in the
    registry.

Arguments:

    Handle - Supplies the handle for all subsequent references to
        the system hive.

    HiveCellParent - Supplies a pointer to the parent cell of the registry
        value name being queried. If this parameter is NULL, the function
        ignores SubkeyName and returns a subkey cell associated with the
        HKLM\CurrentControlSet registry subkey.

    SubkeyName - Supplies a pointer to the subkey name being queried.

    HiveCell - Supplies a pointer into which the cell is returned on success.

Return Value:

    STATUS_SUCCESS if the operation succeeds, otherwise an error status.

--*/

{

    HCELL_INDEX CellIndex;
    PHHIVE Hive;
    PCM_KEY_NODE HiveCellTemp;
    NTSTATUS Status;
    UNICODE_STRING UnicodeString;

    Hive = &HiveTable[Handle]->Hive.Hive;

    if (HiveCellParent == NULL) {
        CellIndex = HiveTable[Handle]->ControlSet;

    } else {
        RtlInitUnicodeString(&UnicodeString, SubkeyName);
        CellIndex = CmpFindSubKeyByName(Hive, HiveCellParent, &UnicodeString);
    }

    if (CellIndex == HCELL_NIL) {
        Status = STATUS_UNSUCCESSFUL;
        goto OslGetSubkeyEnd;
    }

    HiveCellTemp = (PCM_KEY_NODE)HvGetCell(Hive, CellIndex, NULL);
    if (HiveCellTemp == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto OslGetSubkeyEnd;
    }

    if (HiveCellTemp->Signature != CM_KEY_NODE_SIGNATURE) {
        Status = STATUS_UNSUCCESSFUL;
        goto OslGetSubkeyEnd;
    }

    *HiveCell = HiveCellTemp;
    Status = STATUS_SUCCESS;

OslGetSubkeyEnd:
    return Status;
}

NTSTATUS
OslGetSubkeyByNumber (
    __in ULONG Handle,
    __in_opt PCM_KEY_NODE HiveCellParent,
    __in ULONG SubkeyNumber,
    __out_opt PCM_KEY_NODE *HiveCell,
    __deref_out_z_opt PWSTR *SubkeyName
    )

/*++

Routine Description:

    This helper routine returns a subkey cell associated with a subkey in the
    registry.

Arguments:

    Handle - Supplies the handle for all subsequent references to
        the system hive.

    HiveCellParent - Supplies a pointer to the parent cell of the registry
        value name being queried. If this parameter is NULL, the function
        ignores SubkeyNumber and returns a subkey cell associated with the
        HKLM\CurrentControlSet registry subkey.

    SubkeyNumber - Supplies the number of the subkey name being queried.

    HiveCell - Supplies a pointer into which the cell is returned on success.

    SubkeyName - Pointer to the name of a subkey. This should be
         freed with a call to BlMmFreeHeap().

Return Value:

    STATUS_SUCCESS if the operation succeeds, otherwise an error status.

--*/

{

    HCELL_INDEX CellIndex;
    PHHIVE Hive;
    PCM_KEY_NODE HiveCellTemp;
    NTSTATUS Status;

    if(SubkeyName != NULL)
    {
        *SubkeyName = NULL;
    }
    if(HiveCell != NULL)
    {
        *HiveCell = NULL;
    }

    Hive = &HiveTable[Handle]->Hive.Hive;

    if (HiveCellParent == NULL) {
        CellIndex = HiveTable[Handle]->ControlSet;

    } else {
        if((Status = CmpFindSubKeyByNumber(Hive,
                                           HiveCellParent,
                                           SubkeyNumber,
                                           &CellIndex)) != STATUS_SUCCESS)
        {
            goto OslGetSubkeyByNumberEnd;
        }
    }

    if (CellIndex == HCELL_NIL) {
        Status = STATUS_UNSUCCESSFUL;
        goto OslGetSubkeyByNumberEnd;
    }

    HiveCellTemp = (PCM_KEY_NODE)HvGetCell(Hive, CellIndex, NULL);
    if (HiveCellTemp == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto OslGetSubkeyByNumberEnd;
    }

    if (HiveCellTemp->Signature != CM_KEY_NODE_SIGNATURE) {
        Status = STATUS_UNSUCCESSFUL;
        goto OslGetSubkeyByNumberEnd;
    }

    if(SubkeyName != NULL)
    {
        ULONG cchName = 0;

        if (HiveCellTemp->Flags & KEY_COMP_NAME)
        {
            cchName = CmpCompressedNameSize(HiveCellTemp->NameLength) / sizeof(WCHAR);
            *SubkeyName = (PWSTR)BlMmAllocateHeap((cchName + 1) * sizeof(WCHAR));
            if (*SubkeyName == NULL)
            {
                Status = STATUS_INSUFFICIENT_RESOURCES;
                goto OslGetSubkeyByNumberEnd;
            }
            CmpCopyCompressedName(*SubkeyName,
                                  cchName * sizeof(WCHAR),
                                  HiveCellTemp->Name,
                                  HiveCellTemp->NameLength);
        }
        else
        {
            cchName = HiveCellTemp->NameLength / sizeof(WCHAR);
            *SubkeyName = (PWSTR)BlMmAllocateHeap((cchName + 1) * sizeof(WCHAR));
            if (*SubkeyName == NULL)
            {
                Status = STATUS_INSUFFICIENT_RESOURCES;
                goto OslGetSubkeyByNumberEnd;
            }
            RtlCopyMemory(*SubkeyName, HiveCellTemp->Name, cchName * sizeof(WCHAR));
        }

        (*SubkeyName)[cchName] = UNICODE_NULL;
    }

    if(HiveCell != NULL)
    {
        *HiveCell = HiveCellTemp;
    }
    Status = STATUS_SUCCESS;

OslGetSubkeyByNumberEnd:
    return Status;
}

NTSTATUS
OslGetStringValue (
    __in ULONG Handle,
    __in PCM_KEY_NODE HiveCellParent,
    __in PCWSTR ValueName,
    __in ULONG NameLength,
    __out_ecount(NameLength) PWCHAR Name
    )

/*++

Routine Description:

    This helper routine returns the string contents of a name-value pair in the
    registry.

Arguments:

    Handle - Supplies the handle for all subsequent references to
        the system hive.

    HiveCellParent - Supplies a pointer to the parent cell of the registry
        value name being queried.

    ValueName - Supplies a pointer to the value name being queried.

    ModuleNameLength - Supplies the length in characters of the Name
        parameter.

    ModuleName - Supplies a pointer to a string into which the string value
        is written on success.

Return Value:

    STATUS_SUCCESS if the operation succeeds, otherwise an error status.

--*/

{

    HCELL_INDEX CellIndex;
    ULONG DataSize;
    PWCHAR DataString;
    PHHIVE Hive;
    PCM_KEY_VALUE HiveValue;
    NTSTATUS Status;
    UNICODE_STRING UnicodeString;
    errno_t WcsCpyReturn;

    Hive = &HiveTable[Handle]->Hive.Hive;
    RtlInitUnicodeString(&UnicodeString, ValueName);
    CellIndex = CmpFindValueByName(Hive, HiveCellParent, &UnicodeString);
    if (CellIndex == HCELL_NIL) {
        Status = STATUS_UNSUCCESSFUL;
        goto OslGetStringValueEnd;
    }

    HiveValue = (PCM_KEY_VALUE)HvGetCell(Hive, CellIndex, NULL);
    if (HiveValue == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto OslGetStringValueEnd;
    }

    if (HiveValue->Signature != CM_KEY_VALUE_SIGNATURE) {
        Status = STATUS_UNSUCCESSFUL;
        goto OslGetStringValueEnd;
    }

    if (HiveValue->Type != REG_SZ) {
        Status = STATUS_UNSUCCESSFUL;
        goto OslGetStringValueEnd;
    }

    DataSize = CmpGetHKeyValueRealSize(HiveValue->DataLength);
    if (CmpIsHKeyValueSmall(HiveValue->DataLength)) {
        DataString = (PWCHAR)((struct _CELL_DATA *)&HiveValue->Data);

    } else {
        DataString = (PWCHAR)HvGetCell(Hive, HiveValue->Data, NULL);
        if (DataString == NULL) {
            Status = STATUS_REGISTRY_CORRUPT;
            goto OslGetStringValueEnd;
        }
    }

    WcsCpyReturn = wcscpy_s(Name, NameLength, DataString);
    if (WcsCpyReturn != 0) {
        Status = STATUS_UNSUCCESSFUL;
        goto OslGetStringValueEnd;
    }

    Status = STATUS_SUCCESS;

OslGetStringValueEnd:
    return Status;
}

NTSTATUS
OslGetValue (
    __in ULONG Handle,
    __in PCM_KEY_NODE HiveCellParent,
    __in PCWSTR ValueName,
    __out PULONG Type,
    __inout PULONG DataLength,
    __out_ecount(*DataLength) PBYTE Data
    )

/*++

Routine Description:

    This helper routine returns the contents of a name-value pair in the
    registry.

Arguments:

    Handle - Supplies the handle for all subsequent references to
        the system hive.

    HiveCellParent - Supplies a pointer to the parent cell of the registry
        value name being queried.

    ValueName - Supplies a pointer to the value name being queried.

    Type - Supplies the pointer to the ULONG that receives the type of the value

    DataLength - Supplies a pointer to a ULONG that contains the length of the
        Data buffer on entry, and the # bytes written to the buffer upon return.

    Data - Supplies a pointer to a buffer that is written to on success.

Return Value:

    STATUS_SUCCESS if the operation succeeds, otherwise an error status.

--*/

{

    HCELL_INDEX CellIndex;
    ULONG DataSize;
    PHHIVE Hive;
    PCM_KEY_VALUE HiveValue;
    NTSTATUS Status;
    UNICODE_STRING UnicodeString;
    PBYTE ValueData;

    Hive = &HiveTable[Handle]->Hive.Hive;
    RtlInitUnicodeString(&UnicodeString, ValueName);
    CellIndex = CmpFindValueByName(Hive, HiveCellParent, &UnicodeString);
    if (CellIndex == HCELL_NIL) {
        Status = STATUS_NOT_FOUND;
        goto OslGetValueEnd;
    }

    HiveValue = (PCM_KEY_VALUE)HvGetCell(Hive, CellIndex, NULL);
    if (HiveValue == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto OslGetValueEnd;
    }

    if (HiveValue->Signature != CM_KEY_VALUE_SIGNATURE) {
        Status = STATUS_NOT_FOUND;
        goto OslGetValueEnd;
    }

    DataSize = CmpGetHKeyValueRealSize(HiveValue->DataLength);
    if (CmpIsHKeyValueSmall(HiveValue->DataLength)) {
        ValueData = (PBYTE)((struct _CELL_DATA *)&HiveValue->Data);

    } else {
        ValueData = (PBYTE)HvGetCell(Hive, HiveValue->Data, NULL);
        if (ValueData == NULL) {
            Status = STATUS_REGISTRY_CORRUPT;
            goto OslGetValueEnd;
        }
    }

    if( DataSize > *DataLength ) {
        Status = STATUS_UNSUCCESSFUL;
        goto OslGetValueEnd;
    }

    memcpy(Data, ValueData, DataSize);
    *DataLength = DataSize;
    *Type = HiveValue->Type;
    Status = STATUS_SUCCESS;

OslGetValueEnd:
    return Status;
}


PCM_KEY_NODE
OslGetRootCell (
    __in ULONG Handle
    )

/*++

Routine Description:

    This helper routine returns the root cell of the hive selected by the
    handle.

Arguments:

    Handle - Supplies the handle for the system hive.

Return Value:

    Root cell of the hive, or NULL on eror

--*/

{
    HCELL_INDEX CellIndex;
    PHHIVE Hive;
    PCM_KEY_NODE Result;

    Hive = &HiveTable[Handle]->Hive.Hive;
    CellIndex = Hive->BaseBlock->RootCell;
    Result = (PCM_KEY_NODE)HvGetCell(Hive, CellIndex, NULL);

    return Result;
}

NTSTATUS
OslGetCopyOfValue (
    __in ULONG Handle,
    __in PCM_KEY_NODE HiveCellParent,
    __in PCWSTR ValueName,
    __out PULONG Type,
    __deref_out_bcount(*ValueSize) PBYTE* Value,
    __out PULONG ValueSize
    )

/*++

Routine Description:

    This helper routine returns the contents of a name-value pair in the
    registry.

Arguments:

    Handle - Supplies the handle for all subsequent references to
        the system hive.

    HiveCellParent - Supplies a pointer to the parent cell of the registry
        value name being queried.

    ValueName - Supplies a pointer to the value name being queried.

    Type - Supplies the pointer to the ULONG that receives the type of the
         value.

    Value - Receives a pointer to the buffer being returned. This should be
         freed with a call to BlMmFreeHeap().

    ValueSize - Receives the length in bytes of the buffer returned to Value.

Return Value:

    STATUS_SUCCESS if the operation succeeds, otherwise an error status.

--*/

{

    HCELL_INDEX CellIndex;
    ULONG DataSize;
    PBYTE DataValue;
    PHHIVE Hive;
    PCM_KEY_VALUE HiveValue;
    NTSTATUS Status;
    UNICODE_STRING UnicodeString;

    Hive = &HiveTable[Handle]->Hive.Hive;
    RtlInitUnicodeString(&UnicodeString, ValueName);
    CellIndex = CmpFindValueByName(Hive, HiveCellParent, &UnicodeString);
    if (CellIndex == HCELL_NIL) {
        Status = STATUS_NOT_FOUND;
        goto OslGetCopyOfValueEnd;
    }

    HiveValue = (PCM_KEY_VALUE)HvGetCell(Hive, CellIndex, NULL);
    if (HiveValue == NULL) {
        Status = STATUS_REGISTRY_CORRUPT;
        goto OslGetCopyOfValueEnd;
    }

    if (HiveValue->Signature != CM_KEY_VALUE_SIGNATURE) {
        Status = STATUS_UNSUCCESSFUL;
        goto OslGetCopyOfValueEnd;
    }

    DataSize = CmpGetHKeyValueRealSize(HiveValue->DataLength);
    if (CmpIsHKeyValueSmall(HiveValue->DataLength)) {
        DataValue = (PBYTE)((struct _CELL_DATA *)&HiveValue->Data);

    } else {
        DataSize = HiveValue->DataLength;
        DataValue = (PBYTE)HvGetCell(Hive, HiveValue->Data, NULL);
        if (DataValue == NULL) {
            Status = STATUS_REGISTRY_CORRUPT;
            goto OslGetCopyOfValueEnd;
        }
    }

    *Value = (PBYTE)BlMmAllocateHeap(DataSize);
    if (*Value == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto OslGetCopyOfValueEnd;
    }

    RtlCopyMemory(*Value, DataValue, DataSize);
    *ValueSize = DataSize;
    *Type = HiveValue->Type;

    Status = STATUS_SUCCESS;

OslGetCopyOfValueEnd:
    return Status;
}

NTSTATUS
OslGetBinaryValue(
    __in ULONG Handle,
    __in PCM_KEY_NODE HiveCellParent,
    __in PCWSTR ValueName,
    __deref_out_bcount(*ValueSize) PBYTE* Value,
    __out PULONG ValueSize
    )

/*++

Routine Description:

    This helper routine returns the binary contents of a name-value pair in the
    registry.

Arguments:

    Handle - Supplies the handle for all subsequent references to
         the system hive.

    HiveCellParent - Supplies a pointer to the parent cell of the registry
         value name being queried.

    ValueName - Supplies a pointer to the value name being queried.

    Value - Receives a pointer to the buffer being returned. This should be
         freed with a call to BlMmFreeHeap().

    ValueSize - Receives the length in bytes of the buffer returned to Value.

Return Value:

    STATUS_SUCCESS if the operation succeeds, otherwise an error status.

--*/

{
    PBYTE Buffer;
    ULONG BufferSize;
    NTSTATUS Status;
    ULONG Type;

    Buffer = NULL;

    Status = OslGetCopyOfValue(Handle,
                               HiveCellParent,
                               ValueName,
                               &Type,
                               &Buffer,
                               &BufferSize);

    if (!NT_SUCCESS(Status)) {
        goto OslGetBinaryValueEnd;
    }

    if (Type != REG_BINARY) {
        Status = STATUS_UNSUCCESSFUL;
        goto OslGetBinaryValueEnd;
    }

    *ValueSize = BufferSize;
    *Value = Buffer;
    Buffer = NULL;

OslGetBinaryValueEnd:
    if (Buffer != NULL) {
        BlMmFreeHeap(Buffer);
    }

    return Status;
}

BOOLEAN
OslAreDriverDependenciesSatisfied (
    __in ULONG Handle,
    __in PCM_KEY_NODE HiveCellParent,
    __in PWSTR SubkeyName,
    __in_opt PCMP_DRIVER_DEPENDENCY_CALLBACK_ROUTINE DependencyCallback,
    __in_opt PVOID DependencyCallbackContext
    )

/*++

Routine Description:

    Wrapper for CmpAreDriverDependenciesSatisfied.

Arguments:

    Handle - Supplies the handle for all subsequent references to
        the system hive.

    HiveCellParent - Supplies a pointer to the parent cell of the registry key
        to check if depedencies are satisfied.

    SubkeyName - Supplies the name of the subkey that contains driver
        dependencies.

    DependencyCallback - Optionally supplies a callback to evaluate if the
        dependency is met before consulting the registry.

    DependencyCallbackContext - Optionally supplies a context to be provided to
        the dependency callback function.

Return Value:

    TRUE is the driver dependencies are satisfied.

    FALSE otherwise.

--*/

{
    HCELL_INDEX CellIndex;
    HCELL_INDEX ControlSetCellIndex;
    PHHIVE Hive;
    UNICODE_STRING UnicodeString;

    Hive = &HiveTable[Handle]->Hive.Hive;

    ControlSetCellIndex = HiveTable[Handle]->ControlSet;

    if (ControlSetCellIndex == HCELL_NIL) {
        return FALSE;
    }

    RtlInitUnicodeString(&UnicodeString, SubkeyName);
    CellIndex = CmpFindSubKeyByName(Hive, HiveCellParent, &UnicodeString);

    if (CellIndex == HCELL_NIL) {
        return FALSE;
    }

    return CmpAreDriverDependenciesSatisfied(Hive,
                                             ControlSetCellIndex,
                                             CellIndex,
                                             DependencyCallback,
                                             DependencyCallbackContext);
}

NTSTATUS
OslIsFirmwareDependencySatisfied (
    __in PWSTR Dependency,
    __in_opt PVOID Context,
    __out PBOOLEAN DependencySatisfied
    )

/*++

Routine Description:

    This routine evaluates whether the specified dependency is satisfied by
    examining the status of the current firmware.

Arguments:

    Dependency - Supplies the dependency to evaluate.

    Context - Optionally supplies a list of firmware update results

    DependencySatisfied - Returns whether or not the dependency is satisfied.

Return Value:

    STATUS_SUCCESS if the dependency was evaluated.

    STATUS_MORE_PROCESSING_REQUIRED if the dependency is not in the firmware
    resource list.

    NTSTATUS error code otherwise.

--*/

{
    GUID DependencyGuid;
    ULONG Index;
    PFWUPDATE_RESULTS_HEADER ResultsHeader;
    NTSTATUS Status;
    NTSTATUS TempStatus;
    UNICODE_STRING UnicodeString;
    PFIRMWARE_UPDATE_INFORMATION UpdateInformation;

    ASSERT(Dependency != NULL);
    ASSERT(DependencySatisfied != NULL);

    *DependencySatisfied = FALSE;

    Status = STATUS_MORE_PROCESSING_REQUIRED;

    if (Context == NULL) {
        goto OslIsFirmwareDependencySatisfiedEnd;
    }

    ResultsHeader = (PFWUPDATE_RESULTS_HEADER)Context;

    if (ResultsHeader->Results == 0) {
        goto OslIsFirmwareDependencySatisfiedEnd;
    }

    TempStatus = RtlInitUnicodeStringEx(&UnicodeString,
                                        Dependency);

    if (!NT_SUCCESS(TempStatus)) {
        Status = TempStatus;
        goto OslIsFirmwareDependencySatisfiedEnd;
    }

    TempStatus = RtlGUIDFromString(&UnicodeString,
                                   &DependencyGuid);

    if (!NT_SUCCESS(TempStatus)) {
        Status = TempStatus;
        goto OslIsFirmwareDependencySatisfiedEnd;
    }

    for (Index = 0; Index < ResultsHeader->Results; Index++) {
        UpdateInformation = &ResultsHeader->FirmwareUpdateInformation[Index];

        if (RtlCompareMemory(&DependencyGuid,
                             &(UpdateInformation->FirmwareClass),
                             sizeof(GUID)) == sizeof(GUID)) {

            Status = STATUS_SUCCESS;

            if (UpdateInformation->LastAttemptStatus == STATUS_SUCCESS) {

                //
                // The firmware is in a good state.  The dependency is satisfied.
                //

                *DependencySatisfied = TRUE;
            } else {

                //
                // The firmware is in an error state.  The dependency is not satsified.
                //

                *DependencySatisfied = FALSE;
            }

            break;
        }
    }

  OslIsFirmwareDependencySatisfiedEnd:

    return Status;
}

NTSTATUS
OslReadManufacturingModeProfileSettings (
    __in ULONG Handle,
    __out PBOOLEAN ManufacturingModeEnabled,
    __out PWSTR *ManufacturingProfile
    )

/*++

Routine Description:

    This routine checks whether a manufacturing profile exists.

Arguments:

    Handle - Supplies a handle to the system hive.

    ManufacturingModeEnabled - Returns whether or not the manufacturing mode
        settings indicate manufacturing mode should be enabled.

    ManufacturingProfile - Returns the specified manufacturing profile.

Return Value:

    STATUS_SUCCESS if the manufacturing mode settings were successfully
    examined.

    STATUS_NO_MEMORY when an allocation fails.

    STATUS_NOT_FOUND if the settings cannot be read.

--*/

{

    ULONG DataSize;
    DWORD Enabled;
    PCM_KEY_NODE ManufacturingModeNode;
    NTSTATUS Status;
    PWCHAR TempProfile;
    ULONG Type;

    ASSERT(ManufacturingModeEnabled != NULL);
    ASSERT(ManufacturingProfile != NULL);

    Enabled = 0;
    Status = STATUS_SUCCESS;
    TempProfile = NULL;

    //
    // Get the key node for the ManufacturingMode key
    //

    if (CmpLoadManufacturingModeNode(&HiveTable[Handle]->Hive.Hive,
                                     HiveTable[Handle]->ControlSet,
                                     &ManufacturingModeNode,
                                     NULL) == FALSE) {

        Status = STATUS_NOT_FOUND;
        goto ReadManufacturingModeProfileSettingsEnd;
    }

    //
    // Check if the registry settings indicate manufacturing mode should be
    // enabled.
    //

    DataSize = sizeof(Enabled);
    Status = OslGetValue(Handle,
                         ManufacturingModeNode,
                         REGSTR_VALUE_MANUFACTURING_MODE_ENABLED,
                         &Type,
                         &DataSize,
                         (PBYTE)&Enabled);

    if (Status == STATUS_NOT_FOUND) {

        //
        // If the enabled setting doesn't exist, it is not an error.  It means
        // manufacturing mode is not enabled.
        //

        Status = STATUS_SUCCESS;
        *ManufacturingModeEnabled = FALSE;
        *ManufacturingProfile = NULL;
        goto ReadManufacturingModeProfileSettingsEnd;

    } else if (!NT_SUCCESS(Status)) {
        goto ReadManufacturingModeProfileSettingsEnd;

    } else if ((Type != REG_DWORD) ||
               (DataSize != sizeof(Enabled))) {

        Status = STATUS_UNSUCCESSFUL;
        goto ReadManufacturingModeProfileSettingsEnd;
    }

    if (Enabled == 0) {
        *ManufacturingModeEnabled = FALSE;
        *ManufacturingProfile = NULL;
        goto ReadManufacturingModeProfileSettingsEnd;
    }

    *ManufacturingModeEnabled = TRUE;

    //
    // Manufacturing mode is enabled.  Retrieve the default profile.
    //

    DataSize = MANUFACTURING_PROFILE_MAX_NAME_SIZE;
    TempProfile = BlMmAllocateHeap(DataSize);
    if (TempProfile == NULL) {
        Status = STATUS_NO_MEMORY;
        goto ReadManufacturingModeProfileSettingsEnd;
    }

    Status = OslGetValue(Handle,
                         ManufacturingModeNode,
                         REGSTR_VALUE_MANUFACTURING_DEFAULT_PROFILE,
                         &Type,
                         &DataSize,
                         (PBYTE)TempProfile);

    if (!NT_SUCCESS(Status)) {
        goto ReadManufacturingModeProfileSettingsEnd;

    } else if (Type != REG_SZ) {
        Status = STATUS_UNSUCCESSFUL;
        goto ReadManufacturingModeProfileSettingsEnd;
    }

    *ManufacturingProfile = TempProfile;
    TempProfile = NULL;

  ReadManufacturingModeProfileSettingsEnd:

    if (TempProfile != NULL) {
        BlMmFreeHeap(TempProfile);
    }

    return Status;
}

BOOLEAN
OslDoesManufacturingProfileExist (
    __in ULONG Handle,
    __in PWSTR ProfileName
    )

/*++

Routine Description:

    This routine checks whether a manufacturing profile exists.

Arguments:

    Handle - Supplies a handle to the system hive.

    ProfileName - Supplies the name of the manufacturing profile to check.

Return Value:

    TRUE if the profile exists, FALSE otherwise.

--*/

{

    PCM_KEY_NODE ProfileNode;

    return CmpLoadManufacturingProfileNode(&HiveTable[Handle]->Hive.Hive,
                                           HiveTable[Handle]->ControlSet,
                                           ProfileName,
                                           &ProfileNode,
                                           NULL);
}

BOOLEAN
OslIsDeviceScreenLockEnabled (
    __in ULONG Handle
    )

/*++

Routine Description:

    This routine checks whether the device has screen lock enabled and
    protecting the device.

Arguments:

    Handle - Supplies a handle to the system hive.

Return Value:

    TRUE if the screen is locked, FALSE otherwise.

--*/

{

    BOOLEAN BoolRet;
    PCM_KEY_NODE ControlCell;
    PCM_KEY_NODE ControlSetCell;
    DWORD EnabledSetting;
    PCM_KEY_NODE LockSettingCell;
    NTSTATUS Status;

    BoolRet = FALSE;

    Status = OslGetSubkey(Handle, NULL, NULL, &ControlSetCell);
    if (!NT_SUCCESS(Status)) {
        goto IsDeviceScreenLockedEnd;
    }

    Status = OslGetSubkey(Handle,
                          ControlSetCell,
                          OslControlString,
                          &ControlCell);

    if (!NT_SUCCESS(Status)) {
        goto IsDeviceScreenLockedEnd;
    }

    Status = OslGetSubkey(Handle,
                          ControlCell,
                          OslDeviceLockSettingString,
                          &LockSettingCell);

    if (!NT_SUCCESS(Status)) {
        goto IsDeviceScreenLockedEnd;
    }

    Status = OslGetDWordValue(Handle,
                              LockSettingCell,
                              OslEnabledString,
                              &EnabledSetting);

    if (!NT_SUCCESS(Status)) {
        goto IsDeviceScreenLockedEnd;
    }

    if (EnabledSetting != 0) {
        BoolRet = TRUE;
    }

  IsDeviceScreenLockedEnd:

    return BoolRet;
}

#if defined(EFI)

NTSTATUS
OslHiveFlush (
    __in ULONG Handle
    )

/*++

Routine Description:

    This routine flushes a loaded hive to its backing storage.

Arguments:

    Handle - The hive reference returned by OslLoadAndInitializeHive.

Return Value:

    NT status code.

--*/

{

    DEVICE_ID DeviceId;
    FILE_ID FileId;
    ULONG Size;
    NTSTATUS Status;

    DeviceId = INVALID_DEVICE_ID;
    FileId = INVALID_FILE_ID;
    if ((HiveTableEntries <= Handle) ||
        (HiveTable[Handle] == NULL) ||
        (HiveTable[Handle]->Device == NULL) ||
        (HiveTable[Handle]->FileName == NULL)) {

        Status = STATUS_INVALID_PARAMETER;
        goto HiveFlushEnd;
    }

    //
    // Flush any changes back to disk.
    //

    if (CHECK_FLAG(HiveTable[Handle]->Flags, LOADED_HIVE_FLAGS_DIRTY) ==
        FALSE) {

        Status = STATUS_SUCCESS;
        goto HiveFlushEnd;
    }

    //
    // The hive contains its source file path.  Open this file for write
    // access.
    //

    Status = BlDeviceOpen(HiveTable[Handle]->Device,
                          OPEN_READ_WRITE,
                          &DeviceId);

    if (!NT_SUCCESS(Status)) {
        goto HiveFlushEnd;
    }

    Status = BlFileOpen(DeviceId,
                        HiveTable[Handle]->FileName,
                        OPEN_READ_WRITE,
                        &FileId);

    if (!NT_SUCCESS(Status)) {
        goto HiveFlushEnd;
    }

    //
    // Get the size to flush.  The length of meaningful data in the hive may
    // be less than the file size.  Flush the header and the bin data.
    //

    Size = HiveTable[Handle]->Hive.Hive.BaseBlock->Length + HBLOCK_SIZE;
    if (Size > HiveTable[Handle]->ImageSize) {
        Size = HiveTable[Handle]->ImageSize;
    }

    //
    // Flush the entire file.
    //

    Status = BlFileWriteAtOffset(FileId,
                                 Size,
                                 0,
                                 HiveTable[Handle]->ImageBase,
                                 NULL);

    if (NT_SUCCESS(Status)) {
        CLEAR_FLAGS(HiveTable[Handle]->Flags, LOADED_HIVE_FLAGS_DIRTY);
    }

HiveFlushEnd:
    if (FileId != INVALID_FILE_ID) {
        BlFileClose(FileId);
    }

    if (DeviceId != INVALID_DEVICE_ID) {

        //
        // Make sure device writes went through.
        //

        (void)BlDeviceFlush(DeviceId);

        BlDeviceClose(DeviceId);
    }

    return Status;
}

PFIRMWARE_UPDATE_SETTINGS
OslHiveGetUpdateSettings (
    __in PEFI_SYSTEM_RESOURCE_ENTRY FirmwareResource,
    __in ULONG Handle
    )

/*++

Routine Description:

    This routine loads the firmware update settings for a given firmware
    resource based on its firmware_class GUID:

    HKLM\SYSTEM\CurrentControlSet\Control\FirmwareResources\{<firmware_class>}

    N.B. When update settings are successfully allocated and returned, this
         routine auto-increments the firmware update 'Phase' to minimize the
         risk of a failure that would cause the loader to repeatedly attempt
         the same update.  Consequenlty, the calling firmware update support
         doesn't explicitly need to update the registry 'Phase' value as part
         of its process.  This works because updates are processed in batches
         during one OS boot-cycle, therefore the 'Phase' values are always in
         sync for all active updates.

Arguments:

    FirmwareResource - Supplies the firmware resource whose update settings
        will be loaded.

    Handle - Supplies a handle to the system hive.

Return Value:

   Returns an allocated firmware update settings structure.  The caller is
   responsible for freeing this allocation.

--*/

{

    HCELL_INDEX Control;
    HCELL_INDEX ControlKey;
    HCELL_INDEX ControlKey2;
    HCELL_INDEX ControlSet;
    PULONG Data;
    ULONG DataSize;
    PFIRMWARE_UPDATE_SETTINGS FirmwareUpdateSettings;
    UNICODE_STRING GuidString;
    PHHIVE Hive;
    PCM_KEY_NODE HiveCell;
    NTSTATUS LastAttemptStatus;
    ULONG LastAttemptVersion;
    ULONG Phase;
    ULONG Policy;
    ULONG Size;
    NTSTATUS Status;
    ULONG Type;
    UNICODE_STRING UnicodeString;
    PCM_KEY_VALUE Value;
    HCELL_INDEX ValueCell;
    ULONG Version;

    FirmwareUpdateSettings = NULL;
    if ((HiveTableEntries <= Handle) ||
        (HiveTable[Handle] == NULL) ||
        (FirmwareResource == NULL)) {

        goto HiveGetUpdateSettingsEnd;
    }

    //
    // Look for firmware update settings corresponsing to FirmwareResource->
    // FirmwareClass GUID in the FirmwareResource control section.
    //

    Hive = &HiveTable[Handle]->Hive.Hive;
    ControlSet = HiveTable[Handle]->ControlSet;
    HiveCell = (PCM_KEY_NODE)HvGetCell(Hive, ControlSet, NULL);
    if (HiveCell == NULL) {
        goto HiveGetUpdateSettingsEnd;
    }

    RtlInitUnicodeString(&UnicodeString, L"Control");
    Control = CmpFindSubKeyByName(Hive, HiveCell, &UnicodeString);
    if (Control == HCELL_NIL) {
        goto HiveGetUpdateSettingsEnd;
    }

    HiveCell = (PCM_KEY_NODE)HvGetCell(Hive, Control, NULL);
    if (HiveCell == NULL) {
        goto HiveGetUpdateSettingsEnd;
    }

    RtlInitUnicodeString(&UnicodeString, L"FirmwareResources");
    ControlKey = CmpFindSubKeyByName(Hive, HiveCell, &UnicodeString);
    if (ControlKey == HCELL_NIL) {
        goto HiveGetUpdateSettingsEnd;
    }

    HiveCell = (PCM_KEY_NODE)HvGetCell(Hive, ControlKey, NULL);
    if (HiveCell == NULL) {
        goto HiveGetUpdateSettingsEnd;
    }

    Status = RtlStringFromGUID((GUID *)&FirmwareResource->FirmwareClass,
                               &GuidString);

    if (!NT_SUCCESS(Status)) {
        goto HiveGetUpdateSettingsEnd;
    }

    RtlInitUnicodeString(&UnicodeString, GuidString.Buffer);
    ControlKey2 = CmpFindSubKeyByName(Hive, HiveCell, &UnicodeString);
    RtlFreeUnicodeString(&GuidString);
    if (ControlKey2 == HCELL_NIL) {
        goto HiveGetUpdateSettingsEnd;
    }

    HiveCell = (PCM_KEY_NODE)HvGetCell(Hive, ControlKey2, NULL);
    if (HiveCell == NULL) {
        goto HiveGetUpdateSettingsEnd;
    }

    RtlInitUnicodeString(&UnicodeString, L"Phase");
    ValueCell = CmpFindValueByName(Hive, HiveCell, &UnicodeString);
    if (ValueCell == HCELL_NIL) {
        goto HiveGetUpdateSettingsEnd;
    }

    Value = (PCM_KEY_VALUE)HvGetCell(Hive, ValueCell, NULL);
    if ((Value == NULL) || (Value->Type != REG_DWORD)) {
        goto HiveGetUpdateSettingsEnd;
    }

    DataSize = CmpGetHKeyValueRealSize(Value->DataLength);
    if (CmpIsHKeyValueSmall(Value->DataLength)) {
        Data = (PULONG)((struct _CELL_DATA *)&Value->Data);

    } else {
        Data = (PULONG)HvGetCell(Hive, Value->Data, NULL);
        if (Data == NULL) {
            goto HiveGetUpdateSettingsEnd;
        }
    }

    //
    // Update settings exist, read in their values and allocate a buffer to
    // return them.
    //

    DataSize = sizeof(ULONG);
    Status = OslGetValue(Handle,
                         HiveCell,
                         L"Version",
                         &Type,
                         &DataSize,
                         (PBYTE)&Version);

    if (!NT_SUCCESS(Status) || (DataSize != sizeof(ULONG))) {
        goto HiveGetUpdateSettingsEnd;
    }

    Status = OslGetValue(Handle,
                         HiveCell,
                         L"Policy",
                         &Type,
                         &DataSize,
                         (PBYTE)&Policy);

    if (!NT_SUCCESS(Status) || (DataSize != sizeof(ULONG))) {
        Policy = FIRMWARE_UPDATE_POLICY_DEFAULT;
    }

    //
    // If the registry update 'Phase' value is not 0 or 1 (update/report), then
    // ignore these update settings.
    //

    Phase = *Data;
    if (Phase >= FIRMWARE_UPDATE_PHASE_IGNORE) {

        //
        // Verify that the last reported results are in sync with the firmware
        // resource in case the resource was updated externally and the results
        // are now stale.
        //

        if ((Phase != FIRMWARE_UPDATE_PHASE_IGNORE) ||
            (Policy != FIRMWARE_UPDATE_POLICY_DEFAULT) ||
            (Version > FirmwareResource->FirmwareVersion)) {
            goto HiveGetUpdateSettingsEnd;
        }

        DataSize = sizeof(ULONG);
        Status = OslGetValue(Handle,
                             HiveCell,
                             L"LastAttemptVersion",
                             &Type,
                             &DataSize,
                             (PBYTE)&LastAttemptVersion);

        if (!NT_SUCCESS(Status) ||
            (DataSize != sizeof(ULONG)) ||
            (LastAttemptVersion != Version)) {
            goto HiveGetUpdateSettingsEnd;
        }

        DataSize = sizeof(NTSTATUS);
        Status = OslGetValue(Handle,
                             HiveCell,
                             L"LastAttemptStatus",
                             &Type,
                             &DataSize,
                             (PBYTE)&LastAttemptStatus);

        if (!NT_SUCCESS(Status) ||
            (DataSize != sizeof(NTSTATUS)) ||
            (LastAttemptStatus == STATUS_SUCCESS)) {
            goto HiveGetUpdateSettingsEnd;
        }

        //
        // Reset phase in order to push firmware update through the reporting
        // phase again.
        //

        Phase = FIRMWARE_UPDATE_PHASE_UPDATE;
    }

    //
    // As an optimization, increment firmware update 'Phase' value immediately
    // to minimize the risk of a failure that would cause the loader to
    // repeatedly attempt the same update.
    //
    // N.B. Since the update process applies all firmware updates in one boot
    //      cycle, the phase values will always be in sync for active updates.
    //

    *Data = Phase + 1;

    //
    // Mark the hive as dirty so that writes will be updated back to
    // non-volatile storage when the hive is flushed during the update phase.
    //

    SET_FLAGS(HiveTable[Handle]->Flags, LOADED_HIVE_FLAGS_DIRTY);

    //
    // The firmware update structure already includes 1 character
    // (ANYSIZE_ARRAY).
    //

    Size = sizeof(FIRMWARE_UPDATE_SETTINGS) + ((255 + 256) * sizeof(WCHAR));
    FirmwareUpdateSettings = BlMmAllocateHeap(Size);
    if (FirmwareUpdateSettings == NULL) {
        goto HiveGetUpdateSettingsEnd;
    }

    RtlZeroMemory(FirmwareUpdateSettings, Size);
    DataSize = 256 * sizeof(WCHAR);
    Status = OslGetStringValue(Handle,
                               HiveCell,
                               L"Filename",
                               DataSize,
                               &FirmwareUpdateSettings->Filename[0]);

    if (!NT_SUCCESS(Status)) {
        BlMmFreeHeap(FirmwareUpdateSettings);
        FirmwareUpdateSettings = NULL;
        goto HiveGetUpdateSettingsEnd;
    }

    //
    // Save the catalog name to optimize firmware update signature validation.
    //

    FirmwareUpdateSettings->Catalog =
        Add2Ptr(&FirmwareUpdateSettings->Filename[0], DataSize);

    Status = OslGetStringValue(Handle,
                               HiveCell,
                               L"Catalog",
                               DataSize,
                               FirmwareUpdateSettings->Catalog);

    if (!NT_SUCCESS(Status)) {
        FirmwareUpdateSettings->Catalog = NULL;
    }

    FirmwareUpdateSettings->Phase = Phase;
    FirmwareUpdateSettings->Policy = Policy;
    FirmwareUpdateSettings->Version = Version;
    FirmwareUpdateSettings->Resource = FirmwareResource;

HiveGetUpdateSettingsEnd:
    return FirmwareUpdateSettings;
}

#endif

BOOLEAN
OslIsSystemUpgradeInProgress (
    _In_ ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine checks whether the system is in the process of upgrading.

Arguments:

    SystemHiveHandle - Supplies the handle for all subsequent references to the
         system hive.

Return Value:

    TRUE if the system is in the process of upgrading, FALSE otherwise.

--*/

{
    BOOLEAN RetVal;
    PCM_KEY_NODE SetupCell;
    NTSTATUS Status;
    PCM_KEY_NODE SystemCell;
    DWORD SystemUpgradeInProgress;

    RetVal = FALSE;

    SystemCell = OslGetRootCell(SystemHiveHandle);
    if (SystemCell == NULL) {
        goto IsSystemUpgradeInProgressEnd;
    }

    Status = OslGetSubkey(SystemHiveHandle,
                          SystemCell,
                          OslSetupString,
                          &SetupCell);
    if (!NT_SUCCESS(Status)) {
        goto IsSystemUpgradeInProgressEnd;
    }

    Status = OslGetDWordValue(SystemHiveHandle,
                              SetupCell,
                              OslUpgradeString,
                              &SystemUpgradeInProgress);
    if (!NT_SUCCESS(Status)) {
        goto IsSystemUpgradeInProgressEnd;
    }

    if (SystemUpgradeInProgress == 1) {
        RetVal = TRUE;
    }

IsSystemUpgradeInProgressEnd:

    return RetVal;
}

NTSTATUS
OslGetCodeIntegrityCell (
    _In_ ULONG SystemHiveHandle,
    _Outptr_ PCM_KEY_NODE *HiveCell
    )

/*++

Routine Description:

    This routine returns the subkey cell for the Code Integrity key in the
    registry.

Arguments:

    SystemHiveHandle - Supplies the handle for all subsequent references to the
         system hive.

    HiveCell - Supplies a pointer that receives the cell for the CI key.

Return Value:

    STATUS_SUCCESS if successful.

    STATUS_NOT_FOUND if the key could not be found.

--*/

{

    PCM_KEY_NODE CiCell;
    PCM_KEY_NODE ControlCell;
    PCM_KEY_NODE ControlSetCell;
    NTSTATUS Status;

    Status = OslGetSubkey(SystemHiveHandle, NULL, NULL, &ControlSetCell);
    if (!NT_SUCCESS(Status)) {
        goto GetCodeIntegrityKeyEnd;
    }

    Status = OslGetSubkey(SystemHiveHandle,
                          ControlSetCell,
                          OslControlString,
                          &ControlCell);

    if (!NT_SUCCESS(Status)) {
        goto GetCodeIntegrityKeyEnd;
    }

    Status = OslGetSubkey(SystemHiveHandle, ControlCell, L"CI", &CiCell);
    if (!NT_SUCCESS(Status)) {
        goto GetCodeIntegrityKeyEnd;
    }

    *HiveCell = CiCell;

GetCodeIntegrityKeyEnd:
    return Status;
}

NTSTATUS
OslGetDWordValue (
    _In_ ULONG Handle,
    _In_ PCM_KEY_NODE HiveCellParent,
    _In_ PCWSTR ValueName,
    _Out_ PDWORD Dword
    )

/*++

Routine Description:

    This routine returns the contents of a DWORD name-value pair in the
    registry.

Arguments:

    Handle - Supplies the handle for all subsequent references to
        the system hive.

    HiveCellParent - Supplies a pointer to the parent cell of the registry
        value name being queried.

    ValueName - Supplies a pointer to the value name being queried.

    Dword - Supplies a pointer to a DWORD that receives the DWORD value.

Return Value:

    STATUS_SUCCESS if successful.

    STATUS_NOT_FOUND if the value could not be found.

    STATUS_OBJECT_TYPE_MISMATCH if the value is not REG_DWORD.

--*/

{
    NTSTATUS Status;
    ULONG Type;
    DWORD Value;
    ULONG ValueLength;

    ValueLength = sizeof(Value);
    Status = OslGetValue(Handle,
                         HiveCellParent,
                         ValueName,
                         &Type,
                         &ValueLength,
                         (PBYTE)&Value);

    if (!NT_SUCCESS(Status)) {
        goto GetDWordValueEnd;
    }

    if (Type != REG_DWORD || ValueLength != sizeof(Value)) {
        Status = STATUS_OBJECT_TYPE_MISMATCH;
        goto GetDWordValueEnd;
    }

    *Dword = Value;

GetDWordValueEnd:
    return Status;
}

