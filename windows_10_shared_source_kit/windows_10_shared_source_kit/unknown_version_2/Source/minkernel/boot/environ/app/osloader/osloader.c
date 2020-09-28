/*++

Copyright (c) Microsoft Corporation

Module Name:

    osloader.c

Abstract:

    Implementation of the Windows OS Loader.

Environment:

    Boot Environment.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"
#include "bootevents.h"
#include "blfirmw.h"
#include "vsml.h"
#include "hvilib.h"
#include <telemetry\MicrosoftTelemetry.h>
#include "vmbusfilesystem.h"

//
// ---------------------------------------------------------------- Definitions
//

//
// Define OS/2 executable resource information structure.
//

#define FONT_DIRECTORY   0x8007
#define FONT_RESOURCE    0x8008

typedef struct _RESOURCE_TYPE_INFORMATION {
    USHORT Ident;
    USHORT Number;
    LONG Proc;
} RESOURCE_TYPE_INFORMATION, *PRESOURCE_TYPE_INFORMATION;

//
// Define OS/2 executable resource name information structure.
//

typedef struct _RESOURCE_NAME_INFORMATION {
    USHORT Offset;
    USHORT Length;
    USHORT Flags;
    USHORT Ident;
    USHORT Handle;
    USHORT Usage;
} RESOURCE_NAME_INFORMATION, *PRESOURCE_NAME_INFORMATION;

//
// Define the boot status data log file name, as well as the offset from the
// beginning of the log file to the start of the actual boot status data log
// section.
//

#define BSD_LOG_DATA_FILENAME   L"bootstat.dat"
#define BSD_LOG_DATA_OFFSET     2048

//
// Mount root for the EV persistent store.
//

#define EVSTORE_MOUNT_ROOT      L"EVSTORE"

//
// Mount root for the Early Launch Anti-Malware store.
//

#define ELAM_MOUNT_ROOT         L"ELAM"

//
// The Hypervisor Loader's Persistent Data Application GUID:
//
//    {335498ea-8a6f-421f-af3c-572e9d9decb9}
//

#define HVL_APPLICATION_GUID {0x335498ea, 0x8a6f, 0x421f, \
    { 0xaf, 0x3c, 0x57, 0x2e, 0x9d, 0x9d, 0xec, 0xb9}}

//
// Bitlocker's Status Persistent Data Application GUID:
//
//    {bd750217-2a5e-40c6-abdf-183cc7760f2b}
//

#define FVE_BITLOCKER_STATUS_GUID {0xbd750217, 0x2a5e, 0x40c6, \
    {0xab, 0xdf, 0x18, 0x3c, 0xc7, 0x76, 0xf, 0x2b}}

//
// The API set extension that is used to determine whether this is the
// MobileCore SKU.
//

const UNICODE_STRING OslMobileCoreBootExtension =
    CONST_UNICODE_STRING(L"ext-ms-win-mobilecore-boot-l1-1-0");

//
// Identifier for the VMBUS device.
//

const VMBUS_IDENTIFIER OslVmbusIdentifier = {
    GUID_VMBFS_INTERFACE_TYPE_DEFINE,
    GUID_VMBFS_IMC_INSTANCE_DEFINE
};

//
// Machine configuration hive path on the synthetic IMC device.
//

#define IMC_SYNTHETIC_PATH          L"\\imc.hiv"

//
// Define the key under which the Initial Machine Configuration hive is loaded.
//

#define IMC_MOUNT_ROOT               L"InitialMachineConfig"
#define IMC_RNG_KEY_NAME             L"RNG"
#define IMC_SYSTEM_KEY_NAME          L"SYSTEM"
#define IMC_SEQUENCE_VALUE_NAME      L"SEQUENCE"
#define IMC_SYSTEM_CONTROL_KEY_NAME  L"CONTROL"
#define IMC_IMC_KEY_NAME             L"INITIALMACHINECONFIG"

//
// Define the SID sharing policy value which corresponds to the Disabled case.
//

#define SID_SHARING_DISABLED 0x1

//
// Define the number of pages in a megabyte for the performance buffer.
//

#define PAGES_PER_MB (1024 * 1024) / PAGE_SIZE

//
// Initial number of reserve memory descriptors to allocate when building kernel
// memory map. Those descriptors are used to allow updates of the memory map
// when doing memory preservation without the need to rebuild memory map.
//

#if !defined(NTKERNEL)

#define INITIAL_RESERVE_DESCRIPTORS_COUNT (OSL_HC_BOOT_ZEROED_MEMORY_MAX_RANGES * 2)

#undef BlTraceBuildKernelMemoryMapStart
#define BlTraceBuildKernelMemoryMapStart(...)

#undef BlTraceBuildKernelMemoryMapStop
#define BlTraceBuildKernelMemoryMapStop(...)

#else

#define INITIAL_RESERVE_DESCRIPTORS_COUNT 512

#endif

//
// -------------------------------------------------------------------- Globals
//

#pragma bss_seg(push)
#pragma bss_seg()

PLOADER_PARAMETER_BLOCK OslLoaderBlock;
PVOID OslEntryPoint;
ULONG OslSystemHiveHandle;

#pragma bss_seg(pop)

UCHAR OslBootAttemptCount;
UCHAR OslBootAttemptMaximum;
ULONG OslBootId;
BOOLEAN OslCurrentBootCheckpoint;
BOOLEAN OslCurrentBootShutdown;
BOOLEAN OslCurrentBootSucceeded;
OSL_BOOT_COUNT_UPDATE_TYPE OslBootCountUpdateRequestForAbort;
ULONG OslElamHiveHandle;
LIST_ENTRY OslFinalMemoryMap;
GENERIC_BUFFER OslFinalMemoryMapDescriptorsBuffer;
LIST_ENTRY OslFreeMemoryDesctiptorsList;
ULONG OslFreeMemoryDesctiptorsListSize;
ULONG OslImcHiveHandle;
BOOLEAN OslImcProcessingValid;
PBOOT_ENVIRONMENT_DEVICE OslLoadDevice;
LIST_ENTRY OslLoadedFirmwareDriverList;
PCHAR OslLoadOptions;
ULONG OslMachineHiveHandle;
PVOID OslMemoryDescriptorBuffer;

BOOLEAN OslResetBootStatus;
PWCHAR OslSystemRoot;
GUID OslApplicationIdentifier;
BOOLEAN OslManufacturingBitlockerEnabled;

#if defined(NT_IUM)

//
// Indicates whether VSM IDK needs to be provisioned.
//

BOOLEAN OslNeedToProvisionOslVsmIdk;

//
// A pointer to VSM IDK.
//

PVSM_IDK_PLAINTEXT  OslVsmIdk;

//
// A pointer to VSM IDK_S (VSM signing key).
//

PVSM_IDK_PLAINTEXT  OslVsmIdkSigning;

//
// Indicates whether VSM Local key needs to be provisioned.
//

BOOLEAN OslNeedToProvisionVsmLKey;

//
// A pointer to an array of VSM Local encryption Keys.
//

PVSM_LKEY_ARRAY OslVsmLKeyArray;

//
// Entropy bytes for VSM.
//

BYTE OslVsmEntropy[BOOT_SEED_BYTES_FOR_VSM];

//
// Flag for FIPS mode enabled
//

BOOLEAN OslFipsMode;

//
// Entropy bytes for IDK generation.
//

BYTE OslIdkEntropy[BOOT_SEED_BYTES_FOR_IDK];

//
// These are random bytes which will be used as VSM Local key
// when the previously cached key instance cannot be obtained.
// The first VSM_LKEY_LENGTH bytes are used as the VSM Local key,
// the next BL_TPM_SEALED_BLOB_KEY_LENGTH_BYTES bytes are used as an encryption
// key for the VSM local key when sealing the latter to TPM.
//

BYTE OslVsmLKeyCandidate[VSM_LKEY_LENGTH + BL_TPM_SEALED_BLOB_KEY_LENGTH_BYTES];

#endif

//
// Define the registration entry and associated metadata for the boot telemetry
// provider.
//

// {23b76a75-ce4f-56ef-f903-c3a2d6ae3f6b}
TRACELOGGING_DEFINE_PROVIDER(
    OslBootProvider,
    BOOT_PROVIDER_NAME,
    (0x23b76a75, 0xce4f, 0x56ef, 0xf9, 0x03, 0xc3, 0xa2, 0xd6, 0xae, 0x3f, 0x6b),
    TraceLoggingOptionMicrosoftTelemetry());

//
// ----------------------------------------------------------------- Prototypes
//

BOOLEAN
OslpAdvancedOptionsRequested (
    VOID
    );

BOOLEAN
OslpIsMobileCoreBoot (
    __in PVOID ApiSetSchema
    );

NTSTATUS
OslpLoadEarlyLaunchHive (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in DEVICE_ID OsDeviceId,
    __in PUNICODE_STRING OsFilePath
    );

NTSTATUS
OslpLoadSystemHive (
    __in DEVICE_ID OsDeviceId,
    __in PUNICODE_STRING OsFilePath,
    __deref_out_bcount(*RegistryLength) PVOID *RegistryBase,
    __out PULONG RegistryLength,
    __out PLOADER_HIVE_RECOVERY_INFO RecoveryInfo,
    __out PULONG SystemHiveHandle
    );

NTSTATUS
OslpReinitializeSystemHive (
    __in ULONG SystemHiveHandle,
    __in PUNICODE_STRING OsFilePath
    );

NTSTATUS
OslpLoadAllModules (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __in ULONG SystemHiveHandle
    );

NTSTATUS
OslpLoadMiscModules (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __in ULONG SystemHiveHandle
    );

NTSTATUS
OslpUnloadFirmwareDrivers(
    __inout PLIST_ENTRY LoadedFirmwareDriverList
    );

NTSTATUS
OslpDetermineKernelHal (
    __out PUNICODE_STRING KernelName,
    __out PUNICODE_STRING HalName
    );

NTSTATUS
OslpLoadNlsData (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID DeviceId,
    __in ULONG HiveId
    );

NTSTATUS
OslpRemoveInternalApplicationOptions (
    VOID
    );

NTSTATUS
OslpValidateImageType (
    __in_bcount(ImageSize) PVOID ImageBase,
    __in ULONG ImageSize
    );

NTSTATUS
OslpLogSaveInformation (
    VOID
    );

VOID
OslpInitializeBootStatusDataLog (
    VOID
    );

NTSTATUS
OslpCheckForcedFailure (
    VOID
    );

NTSTATUS
OslpProcessEVStore (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in PBOOT_ENVIRONMENT_DEVICE BootDevice
    );

NTSTATUS
OslpGetBootDriverFlags (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in ULONG SystemHiveHandle,
    __in DEVICE_ID OsDeviceId,
    __out PULONG Flags
    );

NTSTATUS
OslpProcessInitialMachineConfiguration (
    __in PUNICODE_STRING OsFilePath,
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
OslpSanitizeLoadOptionsString (
    __in PWSTR LoadOptionsString,
    __in PWSTR SanitizeString
    );

VOID
OslpSanitizeStringOptions (
    __in PBOOT_ENTRY_OPTION OptionList
    );

VOID
OslpSetSystemBootFlags (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __out PBCDE_BOOT_MENU_POLICY FinalBootMenuPolicy
    );

NTSTATUS
OslpAllocatePerformanceBuffer (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
OslpLogLoaderTransition (
    _In_ PSTR BootType,
    _In_ ULONGLONG UserInputTime,
    _In_ BOOLEAN LastBootSucceeded,
    _In_ BOOLEAN LastShutdownSucceeded,
    _In_ UCHAR BootAttemptCount,
    _In_ BCDE_BOOT_MENU_POLICY MenuPolicy
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
OslpMain (
    __out PULONG ReturnFlags
    )

/*++

Routine Description:

    Architecture and Firmware independent entry point for the OS Loader.

Arguments:

    ReturnFlags - Supplies a pointer to a variable that receives the flags to
        return to the calling application in the boot application return
        arguments.  Valid flags are RETURN_ARGUMENT_xxx.

Return Value:

    Error code if fatal failure is experienced.

--*/

{

    BOOLEAN ExecuteTransition;
    BOOLEAN ForceNx;
    NTSTATUS Status;

    ForceNx = BlMmEnableNx();

    Status = OslPrepareTarget(ReturnFlags, &ExecuteTransition);
    if (NT_SUCCESS(Status) && (ExecuteTransition != FALSE)) {
        Status = OslExecuteTransition();
    }

    BlMmDisableNx(ForceNx);
    return Status;
}

NTSTATUS
OslPrepareTarget (
    __out PULONG ReturnFlags,
    __out PBOOLEAN ExecuteTransition
    )

/*++

Routine Description:

    Architecture and Firmware independent entry point for the OS Loader.

Arguments:

    ReturnFlags - Supplies a pointer to a variable that receives the flags to
        return to the calling application in the boot application return
        arguments.  Valid flags are RETURN_ARGUMENT_xxx.

    ExecuteTransition - Supplies a boolean indicating if transition to kernel
        should be performed.

Return Value:

    Error code if fatal failure is experienced.

--*/

{

    ULONGLONG AdvancedMenuEndTime;
    ULONGLONG AdvancedMenuStartTime;
    PBOOT_ENVIRONMENT_DEVICE AllocatedOsDevice;
    ULONGLONG AppEndTime;
    ULONGLONG AppStartTime;
    BOOLEAN BoolRet;
    PBOOT_ENVIRONMENT_DEVICE BootDevice;
    PBOOT_ENTRY_OPTION BootOptions;
    ULONG BootType;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[1];
    PGUID Identifier;
    BOOLEAN LastBootCheckpoint;
    ULONG LastBootId;
    BOOLEAN LastBootShutdown;
    BOOLEAN LastBootSucceeded;
    PBSD_POWER_TRANSITION LastPowerTransition;
    UINTN Length;
    PLOADER_PARAMETER_BLOCK LoaderBlock;
    BCDE_BOOT_MENU_POLICY MenuPolicy;
    BOOLEAN OemValid;
    DEVICE_ID OsDeviceId;
    UNICODE_STRING OsFilePath;
    BSD_POWER_TRANSITION PowerTransitionBuffer;
    PVOID RegistryBase;
    ULONG RegistryLength;
    LOADER_HIVE_RECOVERY_INFO RegistryRecoveryInfo;
    ULONG SidSharingPolicy;
    NTSTATUS Status;
    PWCHAR SystemRoot;
    BOOLEAN UnfilteredBootShutdown;
    BOOLEAN UnfilteredBootSucceeded;
    BOOLEAN VideoBiosSupported;

#if defined(NT_IUM)

    PBYTE   VsmIdkEncryptionKeyRef;
    ULONG   VsmIdkEncryptionKeySize;
    VSM_POLICY VsmPolicy;

    VsmPolicy.AllOptions = 0;

#endif

    *ReturnFlags = 0;

    //
    // Initialize loader globals.
    //

    OslSystemHiveHandle = OslElamHiveHandle = OslMachineHiveHandle = OslImcHiveHandle = (ULONG)-1;
    OslImcProcessingValid = FALSE;
    OslLoadDevice = NULL;
    OslLoadOptions = NULL;
    OslSystemRoot = NULL;
    OslLoaderBlock = NULL;
    InitializeListHead(&OslFreeMemoryDesctiptorsList);
    OslFreeMemoryDesctiptorsListSize = 0;
    OslMemoryDescriptorBuffer = NULL;
    InitializeListHead(&OslFinalMemoryMap);
    RtlZeroMemory(&OslFinalMemoryMapDescriptorsBuffer, sizeof(GENERIC_BUFFER));
    OslResetBootStatus = FALSE;
    InitializeListHead(&OslLoadedFirmwareDriverList);
    OslInitializeCoreExtensionSubGroups();

    //
    // Initialize routine locals.
    //

    AllocatedOsDevice = NULL;
    BootDevice = NULL;
    RegistryBase = NULL;
    RtlZeroMemory(&RegistryRecoveryInfo, sizeof(RegistryRecoveryInfo));

    //
    // Record the time that the boot application started executing.
    //

    AppStartTime = BlArchGetPerformanceCounter();

    //
    // Report the current application type to Secure Boot.
    //

    BlSecureBootSetApplicationType(
        MAKE_BCD_APPLICATION_OBJECT(
            BCD_IMAGE_TYPE_BOOT_APPLICATION,
            BCD_APPLICATION_TYPE_WINDOWS_BOOT_LOADER));

    //
    // Register boot library status error handler.
    //

    BlStatusRegisterErrorHandler(&OslBlStatusErrorHandler);


    //
    // Save the identifier used to retrieve settings for this application from
    // the configuration data store.
    //

    Identifier = BlGetApplicationIdentifier();
    if (Identifier != NULL) {
        RtlCopyMemory(&OslApplicationIdentifier, Identifier, sizeof(GUID));
    }

    //
    // Register the KernelBoot TraceLogging provider. Failure here simply makes
    // TraceLoggingWrite calls no-ops.
    //

    TraceLoggingRegister(OslBootProvider);

    //
    // Remove any application options which may only be set internally.
    //

    Status = OslpRemoveInternalApplicationOptions();
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Extract the OS device for global use by this application.  If boot
    // device syntax is used, resolve the device.
    //

    Status = BlGetApplicationOptionDevice(BCDE_OSLOADER_TYPE_OS_DEVICE,
                                          &AllocatedOsDevice,
                                          NULL);

    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    if (AllocatedOsDevice->DeviceType == DEVICE_BOOT) {
        OslLoadDevice = BlGetBootDevice();

    } else {
        OslLoadDevice = AllocatedOsDevice;
    }

    //
    // Extract the OS path, appending a trailing backslash if one does not
    // exist.
    //

    Status = BlGetApplicationOptionString(BCDE_OSLOADER_TYPE_SYSTEM_ROOT,
                                          &SystemRoot);

    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    Length = wcslen(SystemRoot);
    if (SystemRoot[Length - 1] != L'\\') {
        OslSystemRoot = BlMmAllocateHeap((Length + 2) * sizeof(WCHAR));
        if (OslSystemRoot == NULL) {
            Status = STATUS_NO_MEMORY;
            goto PrepareTargetEnd;
        }

        wcscpy_s(OslSystemRoot, Length + 2, SystemRoot);
        wcscat_s(OslSystemRoot, Length + 2, L"\\");
        BlMmFreeHeap(SystemRoot);

    } else {
        OslSystemRoot = SystemRoot;
    }

    //
    // Initialize the boot status data logging engine, and check whether the
    // user has requested a forced failure.
    //

    OslpInitializeBootStatusDataLog();
    Status = OslpCheckForcedFailure();

    //
    // If a forced failure was called for, exit now.
    //

    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Check if the system supports a legacy video BIOS and if not, append the
    // appropriate boot switch. If the function fails then disable VGA anyway.
    //

    Status = BlArchIsVideoBiosSupported(&VideoBiosSupported);
    if ((!NT_SUCCESS(Status)) || (VideoBiosSupported == FALSE)) {
        Status = BlAppendApplicationOptionBoolean(
                     BCDE_OSLOADER_TYPE_DISABLE_VGA_MODE,
                     TRUE);

        if (!NT_SUCCESS(Status)) {
            goto PrepareTargetEnd;
        }
    }

    //
    // Capture boot/shutdown status from the last OS boot.
    //

    Status = OslGetBootStatusData(&LastBootSucceeded,
                                  &LastBootShutdown,
                                  &LastBootCheckpoint,
                                  &OslBootAttemptCount,
                                  &LastBootId,
                                  &UnfilteredBootSucceeded,
                                  &UnfilteredBootShutdown);

    if (!NT_SUCCESS(Status)) {
        LastBootId = 0;
        LastBootSucceeded = TRUE;
        LastBootShutdown = TRUE;
        LastBootCheckpoint = TRUE;
        OslBootAttemptCount = DEFAULT_BOOT_ATTEMPT_COUNT;
        UnfilteredBootSucceeded = TRUE;
        UnfilteredBootShutdown = TRUE;
    }

    OslBootId = LastBootId + 1;
    OslResetBootStatus = TRUE;
    OslBootCountUpdateRequestForAbort = OslBootCountUpdateNone;

    //
    // Capture the power transition data from the last boot before the updated
    // boot status is written avoiding extra reads from disk.
    //

    Status = OslGetLastPowerTransition(&PowerTransitionBuffer);
    if (!NT_SUCCESS(Status)) {
        LastPowerTransition = NULL;

    } else {
        LastPowerTransition = &PowerTransitionBuffer;
    }

    //
    // Initialize the maximum boot attempts allowed before failure based on the
    // boot device type.
    //

    if (BlDeviceIsVirtualPartitionDevice(OslLoadDevice, NULL) != FALSE) {
        OslBootAttemptMaximum = BOOT_ATTEMPT_COUNT_MAXIMUM_VHD;

    } else {
        OslBootAttemptMaximum = BOOT_ATTEMPT_COUNT_MAXIMUM;
    }

    //
    // Only process the boot status data if advanced options were not requested.
    //

#if !defined(NTKERNEL)

    if (OslpAdvancedOptionsRequested() == FALSE) {

        //
        // If the last boot failed and the maximum boot attempts have been made
        // or exceeded, then automatically enter recovery.
        //

        if ((LastBootSucceeded == FALSE) &&
            (OslBootAttemptCount >= OslBootAttemptMaximum)) {

            //
            // Always tell the boot manager to attempt recovery. If there is no
            // recovery environment, it will display an error message. Set the
            // boot attempt count to one less than the maximum, giving the OS
            // only one chance to boot successfully after entering recovery or
            // displaying an error message.
            //

            OslBootAttemptCount = OslBootAttemptMaximum - 1;
            OslCurrentBootCheckpoint = LastBootCheckpoint;
            OslCurrentBootSucceeded = LastBootSucceeded;
            OslCurrentBootShutdown = LastBootShutdown;
            SET_FLAGS(*ReturnFlags,
                      RETURN_ARGUMENT_LAUNCH_RECOVERY_FOR_BAD_BOOT);

            OslFatalError(OSL_ERROR_BAD_BOOT, 0, 0, 0);
            Status = STATUS_UNSUCCESSFUL;
            goto PrepareTargetEnd;
        }

        //
        // If the boot checkpoint was not reached and the maximum boot attempts
        // have been made or exceeded, then automatically enter recovery.
        //

        if ((LastBootCheckpoint == FALSE) &&
            (OslBootAttemptCount >= OslBootAttemptMaximum)) {

            ASSERT(LastBootSucceeded != FALSE);

            OslBootAttemptCount = OslBootAttemptMaximum - 1;
            OslCurrentBootCheckpoint = LastBootCheckpoint;
            OslCurrentBootShutdown = LastBootShutdown;
            OslCurrentBootSucceeded = LastBootSucceeded;
            SET_FLAGS(*ReturnFlags,
                      RETURN_ARGUMENT_LAUNCH_RECOVERY_FOR_BAD_SHUTDOWN);

            OslFatalError(OSL_ERROR_BAD_SHUTDOWN, 0, 0, 0);
            Status = STATUS_UNSUCCESSFUL;
            goto PrepareTargetEnd;
        }

        //
        // If there was a bad shutdown and the attempt count had been reset,
        // automatically enter recovery.
        //

        if ((LastBootShutdown == FALSE) &&
            (OslBootAttemptCount == 0)) {

            ASSERT(LastBootSucceeded != FALSE);
            ASSERT(LastBootCheckpoint != FALSE);

            //
            // Mark the last shutdown as successful. This will allow another
            // boot attempt if the user decides to try again in the case where
            // there is no recovery environment, or in the case where the
            // recovery environment does not clear the bad shutdown flag.
            //

            OslCurrentBootShutdown = TRUE;
            OslCurrentBootCheckpoint = LastBootCheckpoint;
            OslCurrentBootSucceeded = LastBootSucceeded;
            SET_FLAGS(*ReturnFlags,
                      RETURN_ARGUMENT_LAUNCH_RECOVERY_FOR_BAD_SHUTDOWN);

            OslFatalError(OSL_ERROR_BAD_SHUTDOWN, 0, 0, 0);
            Status = STATUS_UNSUCCESSFUL;
            goto PrepareTargetEnd;
        }
    }

#endif !defined(NTKERNEL)

    //
    // Before beginning the OS boot attempt, set the boot status to "failed"
    // and increment the attempt count to indicate, on disk, that another boot
    // is being attempted.
    //

    OslBootAttemptCount += 1;
    OslCurrentBootCheckpoint = FALSE;
    OslCurrentBootSucceeded = FALSE;
    OslCurrentBootShutdown = FALSE;
    OslSetBootStatusData(OslCurrentBootSucceeded,
                         OslCurrentBootShutdown,
                         OslCurrentBootCheckpoint,
                         OslBootCountUpdateIncrement,
                         OslBootAttemptCount);

    //
    // Open the device containing the Operating System.
    //

    Status = BlDeviceOpen(OslLoadDevice,
                          OPEN_READ_WRITE,
                          &OsDeviceId);

    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Allocate a new buffer for the OS file path string as subroutines rely on
    // being able to free and re-allocate this buffer.
    //

    RtlInitUnicodeString(&OsFilePath, NULL);
    BoolRet = BlAppendUnicodeToString(&OsFilePath, OslSystemRoot);
    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto PrepareTargetEnd;
    }

    /* TODO raigner-2015-03-27: disabled until Intel SDP are updated with
                                correct firmware that support this feature

    //
    // After the boot device has been opened, call into the SI library
    // to check if a switch of TPM PCR banks can be performed. The boot
    // device has to be open to check if Bitlocker already sealed to
    // PCR banks.
    //
    BoolRet = OslSwitchTpmPcrBanks(OslLoadDevice);
    if (BoolRet == TRUE) {
        //
        // A switch of the PCR banks has been initiated.
        // Mark the boot as successful and decrement the boot
        // attempt.
        //
        OslCurrentBootCheckpoint = LastBootCheckpoint;
        OslCurrentBootSucceeded = LastBootSucceeded;
        OslCurrentBootShutdown = LastBootShutdown;
        OslBootAttemptCount -= 1;
        OslSetBootStatusData(OslCurrentBootSucceeded,
                             OslCurrentBootShutdown,
                             OslCurrentBootCheckpoint,
                             OslBootAttemptCount);

#if !defined(WIN_APP)
        BlFwReboot();
#else // WIN_APP
        Status = STATUS_CANCELLED;
        goto PrepareTargetEnd;
#endif // WIN_APP
    }
    */

    //
    // Load the system hive into memory.
    //

    Status = OslpLoadSystemHive(OsDeviceId,
                                &OsFilePath,
                                &RegistryBase,
                                &RegistryLength,
                                &RegistryRecoveryInfo,
                                &OslSystemHiveHandle);

    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Convert the load options into an OS load options string.  Do this early
    // in the loader's execution so that this string can be displayed and
    // modified by the user if necessary. This must be called after the secure
    // boot policy has been initialized.
    //

    Status = AhCreateLoadOptionsString(BlGetApplicationEntry()->Options,
        &OslLoadOptions);

    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Initialize the display.
    //

    Status = OslDisplayInitialize(OslSystemHiveHandle);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    //  Process System Integrity Policy
    //  This is necessary before displaying advanced options because the
    //  SI policy can prevent display of the advanced options.
    //

    Status = OslpProcessSIPolicy(FALSE);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Handle any advanced options processing. The total OS loader time should
    // be adjusted to compensate for the time taken while waiting for user input
    // at the auto advanced menu. Note that this is not 100% accurate since the
    // auto advanced menu might not show up at all.
    //

    AdvancedMenuStartTime = BlArchGetPerformanceCounter();
    Status = OslDisplayAdvancedOptionsProcess(OslSystemHiveHandle,
                                              LastBootSucceeded,
                                              LastBootShutdown,
                                              LastBootCheckpoint,
                                              OslBootAttemptCount,
                                              ReturnFlags);

    //
    // If the user has requested that recovery be run for this application,
    // return control to the boot manager, which will launch the recovery
    // sequence. For this case, and the case where the user canceled the boot
    // from an options menu, revert the boot status information on disk.
    //

    if ((NT_SUCCESS(Status) && (*ReturnFlags != FALSE)) ||
        (Status == STATUS_CANCELLED)) {

        OslBootCountUpdateRequestForAbort = OslBootCountUpdateDecrement;
        OslCurrentBootCheckpoint = LastBootCheckpoint;
        OslCurrentBootSucceeded = LastBootSucceeded;
        OslCurrentBootShutdown = LastBootShutdown;
        OslBootAttemptCount -= 1;
        goto PrepareTargetEnd;

    } else if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    AdvancedMenuEndTime = BlArchGetPerformanceCounter();
    AppStartTime += AdvancedMenuEndTime - AdvancedMenuStartTime;

#if defined(NT_IUM)

    //
    // The VSM policy must be set before the initialization of any features
    // that may be required for Virtualization Based Security (such as the
    // hypervisor).
    //

    Status = OslSetVsmPolicy(OslSystemHiveHandle);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    BlVsmGetSystemPolicy(&VsmPolicy);

#endif

#if defined(OSL_SGX_SUPPORT)

    //
    // If any SGX enclave memory regions are configured on this system, add
    // them to the memory map. They will be passed to other boot apps (namely,
    // the hypervisor loader).
    //

    Status = OslEnumerateEnclavePageRegions();
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

#endif

    //
    // If the hypervisor launch type is set to auto in the boot operation,
    // run hypervisor loader to allocate the resources needed for hypervisor
    // launch.
    //
    // N.B. Run hypervisor loader before loading kernel binaries so that the
    //      memory allocated in OsLoader will not be mapped in HvLoader
    //      address space.
    //
    //      Currently, only ADM64 architecture supports hypervisor launch. For
    //      other architectures, OslArchHypervisorSetup returns immediately.
    //
    //      If there is a failure while allocating resources for hypervisor,
    //      OsLoader will continue to load NT and hypervisor will not be
    //      launched. Therefore, the return code from the following function
    //      is not checked.
    //

    Status = OslArchHypervisorSetup(0, NULL, OslSystemHiveHandle);

    //
    // Return an error on failure if a hypervisor is present and we are
    // the CPU manager and we failed setting up the hypervisor for re-launch.
    //

    if (!NT_SUCCESS(Status) &&
        OslArchHypervisorPresent() &&
        OslArchIsPartitionCpuManager()) {

        goto PrepareTargetEnd;
    }

    //
    // Forget the Hypervisor setup status.
    //

    Status = STATUS_SUCCESS;

#if defined(NT_IUM)

    //
    // Check SVN and whether we need to provision keys for VSM/IUM.
    //

    if ((OslArchHvlInitialized() != FALSE) ||
        ((OslArchHypervisorPresent() != FALSE) &&
         (HviIsHypervisorMicrosoftCompatible() != FALSE))) {

        if ((VsmPolicy.Options.Enabled != 0) ||
            (VsmPolicy.Options.VsmRequired != 0) ) {

            Status = BlSvnGetChainStatus();

            if (NT_SUCCESS(Status)) {

                //
                // Provision VSM keys for both the root and guest partitions.
                //

                OslNeedToProvisionOslVsmIdk = TRUE;
                OslNeedToProvisionVsmLKey = TRUE;
            }

            //
            // Do not fail boot.
            //

            Status = STATUS_SUCCESS;
        }
    }

#endif

    //
    // Perform the first phase of initialization required for hypercall setup.
    //

    Status = OslArchHypercallSetup(0);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Display the background image at this point. The graphics library will
    // fail gracefully if graphics mode is not enabled.
    //
    // N.B. Don't redraw the OEM logo if it's already valid.
    //

    OemValid = BlDisplayValidOemBitmap(TRUE);
    if (OemValid == FALSE) {
        BgDisplayBackgroundImage();
    }

    OslDisplayMessage();

    //
    // Initialize the resume context that is required for hibernation.
    //

    Status = OslInitializeResumeContext();
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Initialize the bitlocker status that is required for crash dump capsule.
    // Ignore the return value because the data being passed in the loader block
    // is not required and default assumes BitLocker is on.
    //

    OslInitializeBitlockerStatus();

    //
    // Allocate a buffer for the loader block.
    //

    Status = OslInitializeLoaderBlock(OsDeviceId,
                                      OslSystemHiveHandle,
                                      &OslLoaderBlock);

    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    LoaderBlock = OslLoaderBlock;
    LoaderBlock->RegistryBase = RegistryBase;
    LoaderBlock->RegistryLength = RegistryLength;
    LoaderBlock->Extension->LastBootSucceeded = UnfilteredBootSucceeded;
    LoaderBlock->Extension->LastBootShutdown = UnfilteredBootShutdown;
    LoaderBlock->Extension->BootId = OslBootId;

    RtlCopyMemory(&LoaderBlock->Extension->SystemHiveRecoveryInfo,
                  &RegistryRecoveryInfo,
                  sizeof(LOADER_HIVE_RECOVERY_INFO));

    //
    // Load the Initial Machine Configuration hive if one is
    // specified by the configuration data and attach it
    // to the loader block.
    //
    // All failures are handled internally by propagating an
    // error through the loader block extension to the kernel.
    //

    Status = OslpProcessInitialMachineConfiguration(&OsFilePath,
                                                    LoaderBlock);

    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Save SID sharing disabled flag from the hive.
    //

    Status = OslHiveGetSidSharingPolicy(OslSystemHiveHandle, &SidSharingPolicy);
    if (!NT_SUCCESS(Status) || SidSharingPolicy != SID_SHARING_DISABLED) {
        LoaderBlock->Extension->SidSharingDisabled = FALSE;

    } else {
        LoaderBlock->Extension->SidSharingDisabled = TRUE;
    }

    //
    // Obtain entropy for the system.
    // This is used by the OS to seed the
    // system random number generator.
    // We do this late in the load process to give the TPM maximum time
    // to run self-tests and/or run a physical entropy source.
    //
    // N.B. This call is here so that it can be made after the
    //      InitMachineConfig load which must be after loader block
    //      initialization.
    //

    Status = OslGatherEntropy(OslSystemHiveHandle,
                              &LoaderBlock->Extension->BootEntropyResult,

#if defined(NT_IUM)

                              OslVsmEntropy,
                              OslIdkEntropy,
                              OslVsmLKeyCandidate);

#else

                              NULL,
                              NULL,
                              NULL);

#endif

    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Check the FIPS Enabled key for VSM
    //
#if defined(NT_IUM)

    OslFipsMode = FALSE;

    // For Initial Machine Configuration
    if ((OslImcProcessingValid == TRUE) &&
       (OslImcHiveHandle != (ULONG)-1)) {
        OslCheckFipsMode(OslImcHiveHandle,
                         TRUE,
                         &OslFipsMode);
    }

    // For the system hive
    if (OslFipsMode == FALSE)
    {
        OslCheckFipsMode(OslSystemHiveHandle,
                         FALSE,
                         &OslFipsMode);
    }
#endif

    //
    // Set the flag indicating whether the TPM was initialized correctly or not.
    //

    Status = BlTpmStatus();
    if (NT_SUCCESS(Status)) {
        LoaderBlock->Extension->TpmInitialized = TRUE;
    }

    //
    // Enumerate all firmware enumerable disks.  Create a list of each
    // in the loader block.
    //

    Status = OslEnumerateDisks(LoaderBlock);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Reinitialize the system hive, accounting for any options selected
    // during the advanced options processing.
    //

    Status = OslpReinitializeSystemHive(OslSystemHiveHandle, &OsFilePath);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Initialize Code Integrity structures.
    //

    Status = OslInitializeCodeIntegrity(OsDeviceId, OslSystemHiveHandle);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    Status = OslBuildCodeIntegrityLoaderBlock(OsDeviceId, LoaderBlock);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Process any Secure Boot variables.
    //

    Status = OslFwProtectSecureBootVariables(OslSystemHiveHandle);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Deploy UEFI 2.5+ if needed (User Mode to Deployed Mode)
    //

    OslFwCheckDeploySecureBoot(OslSystemHiveHandle);

    //
    // Process any pending firmware updates.
    //

    Status = OslFwProcessUpdates(LoaderBlock,
                                 &OsFilePath,
                                 OsDeviceId,
                                 OslSystemHiveHandle);

    //
    // Load everything into memory that is required for the kernel.
    // (kernel, hal, boot drivers, etc).
    //

    Status = OslpLoadAllModules(LoaderBlock,
                                &OsFilePath,
                                OsDeviceId,
                                OslSystemHiveHandle);

    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Initialize the boot graphics context.
    //
    // N.B. A NULL return value implies no boot graphics library support in
    //      the kernel and is not considered fatal.
    //
    // The context initialization is delayed until this point to allow the
    // BGRT bitmap to be cleared in some cases, to strictly enforce that
    // specialized Windows Phone behavior cannot be enabled on core Windows.
    //

    LoaderBlock->Extension->BgContext = BgGetContext();

    //
    // Load microcode update on BSP.
    //

    OslLoadMicrocode(LoaderBlock);

    //
    // Load any required firmware drivers that are required for the OS.
    //

    Status = OslFwLoadFirmwareDrivers(&OsFilePath,
                                      OsDeviceId,
                                      &OslLoadedFirmwareDriverList);

    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Initialize the system time parameters that will be used by the kernel.
    //

    OslSetSystemTimeParameters(LoaderBlock, LastPowerTransition);

    //
    // Allocate and map the performance buffer if required.
    //

    OslpAllocatePerformanceBuffer(LoaderBlock);

    //
    // Query Hardware Security Test Interface and transfer to OS.
    //

    BlHSTICallProviders();

    //
    // If hypervisor setup succeeded, attempt to launch VSM. This is loaded
    // only if the VSM launch type is set to auto in the BCD.
    //

#if defined(NT_IUM)

    if (OslNeedToProvisionVsmLKey) {

        //
        // The OslVsmLKeyArray heap allocation will be freed during VSM phase 1 init.
        //

        Status = OslVsmProvisionLKey(OslVsmLKeyCandidate,
                                     OslVsmLKeyCandidate + VSM_LKEY_LENGTH,
                                     &OslVsmLKeyArray);

        if (!NT_SUCCESS(Status)) {
            Status = BlVsmCheckSystemPolicy(VsmPolicyPhaseVsmMasterEncryptionKeyProvisioningComplete,
                                            Status);

            if (!NT_SUCCESS(Status)) {
                goto PrepareTargetEnd;
            }
        }
    }

    if ((OslArchHvlInitialized() != FALSE) ||
        (HviIsHypervisorMicrosoftCompatible() != FALSE)) {

        Status = OslVsmSetup(0, OsDeviceId, LoaderBlock);
    }

    //
    // Deny disabling VSM on soft reboot.
    //

    if (OslIsRunningInSecureKernel(NULL) && !OslVsmIsInitialized()) {
        Status = STATUS_ACCESS_DENIED;
        goto PrepareTargetEnd;
    }

    //
    // Check if failure to launch VSM should block boot. Update hypervisor
    // loader block as well, if applicable.
    //

    if ((OslVsmIsInitialized() == FALSE) || !NT_SUCCESS(Status)) {

        if (OslArchHvlInitialized() != FALSE) {
            OslArchRescindHvVsm();
        }

        Status = BlVsmCheckSystemPolicy(VsmPolicyPhaseVsmPhase0Complete,
                                        Status);

        if (!NT_SUCCESS(Status)) {
            goto PrepareTargetEnd;
        }
    }

#endif

    //
    // Set the value of nt!SeILSigningPolicy.
    //

    Status = OslSetSeILSigningPolicy(LoaderBlock, OslSystemHiveHandle);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // We need to finalize and commit all SI data before we lose access to
    // firmware services.
    //
    // NOTE (klitwack-20071009): It is possible that kernel launch could still
    // fail after this point. However, it shouldn't be exploitable, because
    // the result will be a return all the way back to bootmgr, which will
    // log any subsequent boot app launch.
    //
    // PENTEST REVIEW REQUIRED.
    //

    OslReportKernelLaunch(BlGetApplicationEntry()->Options, OslSystemHiveHandle);

#if defined(NT_IUM)

    //
    // Unseal existing or create a new VSM Identity Key.
    //

    if (OslNeedToProvisionOslVsmIdk) {

        //
        // The OslVsmIdk heap allocation will be freed when the IDK is copied over to
        // the VSM loader block during VSM phase 1 init.
        //

        VsmIdkEncryptionKeyRef = NULL;
        VsmIdkEncryptionKeySize = 0;

        if ((OslVsmLKeyArray != NULL) && (OslVsmLKeyArray->ElementCount > 0)) {
            Status = BlVsmKeysGetCurrentLKeyRefFromArray(OslVsmLKeyArray,
                                                         &VsmIdkEncryptionKeyRef,
                                                         &VsmIdkEncryptionKeySize);
        }

        if (NT_SUCCESS(Status)) {
            Status = OslVsmProvisionIdk(OslLoadDevice,
                                        OslSystemRoot,
                                        OslIdkEntropy,
                                        VsmIdkEncryptionKeyRef,
                                        VsmIdkEncryptionKeySize,
                                        &OslVsmIdk,
                                        &OslVsmIdkSigning);
        }

        if (!NT_SUCCESS(Status)) {
            Status = BlVsmCheckSystemPolicy(VsmPolicyPhaseVsmMasterEncryptionKeyProvisioningComplete,
                                            Status);

            if (!NT_SUCCESS(Status)) {
                goto PrepareTargetEnd;
            }
        }
    }

#endif

    //
    // Unconditionally cap the boot environment to prevent unsealing
    // now that IDK processing is complete.  This is the last SIPA event.
    //

    BlSiRecordEnvironmentCapEvent(SI_PCR_11_WIN10_PREBOOT_SEALING_LOCK_OUT);

    //
    // Close SI library. No more measured boot events.
    //

    OslCloseSipaReporting();

    //
    // At this point, any files required by the Operating System have been
    // loaded.  Additionally the loader block has been initialized with all
    // platform independent data, minus the current memory map.  After
    // an OS copy of the memory map has been written to the loader block,
    // no more memory services can be performed.  To ensure this, no
    // library services can be utilized.  Therefore, all platform dependent
    // steps required to either populate the loader block or prepare for
    // kernel transition must be broken up into steps that require memory
    // services and those that do not.
    //

    //
    // Perform any architecture specific steps required to transfer execution
    // to the kernel.  This routine may use MM (library) services.
    //

#if defined(_X86_) || defined (_AMD64_)

    //
    // Retrieve values of XSAVE related registry keys to be used later in the
    // OS loader. Also, pass these values to kernel via LoaderBlock->Extension
    //

    OslReadXsaveRegistryValues(OslSystemHiveHandle, LoaderBlock);

#endif // (_X86_) || (_AMD64_)

    Status = OslArchKernelSetup(0, LoaderBlock);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // RNG services are no longer needed at this point.
    //

    OslTerminateRNG();

    //
    // Perform any firmware related steps required to kernel transition.
    // This routine may use MM (library) services.
    //

    Status = OslFwKernelSetup(0, LoaderBlock);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Process evstore option if present.
    //

    BootDevice = BlGetBootDevice();
    Status = OslpProcessEVStore(LoaderBlock, BootDevice);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Log the total device IO as an ETW event. This must be done before
    // the logs are destroyed and persistent data is processed.
    //

    BlLogEtwTotalIo();

    //
    // Log this as cold boot.
    //

    BootType = (ULONG)EtwBootTypeCold;
    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)&BootType,
                        sizeof(ULONG));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_TYPE,
                  NULL,
                  1,
                  EventDataDescriptor);

    //
    // Determine the system boot flags based on options passed from the boot
    // manager and key presses. Pass the information to the OS in the loader
    // block.
    //

    OslpSetSystemBootFlags(LoaderBlock, &MenuPolicy);

    //
    // Make a copy of the boot options, which will be passed to the kernel for
    // auditing.
    //

    Status = BlCopyBootOptions(BlGetApplicationEntry()->Options, &BootOptions);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    LoaderBlock->Extension->BootOptions = BootOptions;

    //
    // Log the transition occurrence.
    //

    OslpLogLoaderTransition("Cold",
                            AdvancedMenuEndTime - AdvancedMenuStartTime,
                            UnfilteredBootSucceeded,
                            UnfilteredBootShutdown,
                            OslBootAttemptCount,
                            MenuPolicy);

    //
    // End the Boot Logging and create a header and persist it as well. Ignore
    // the status.
    //

    OslpLogSaveInformation();

    //
    // Make sure FVE will finalize the set of keys it's going to pass
    // to the OS that is being loaded.
    //

    Status = BlFveSecureBootRestrictToOne(OsDeviceId);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // If OSLoader was started using network boot then shutdown UNDI stack
    // before kernel is launched.
    //

    if (BootDevice->DeviceType == DEVICE_UDP) {
        Status = BlNetUndiClose();
        if (!NT_SUCCESS(Status)) {
            goto PrepareTargetEnd;
        }
    }

    //
    // Destroy any memory allocated by the boot graphics library.
    //

    OslDisplayDestroy(NULL);

    //
    // Process any application data that must be made available during OS
    // init.
    //
    // N.B. Failures cannot be ignored since the physical memory consumed by
    //      persistent data remains allocated through OS init and the memory
    //      manager requires all such memory to be mapped into the kernel
    //      address space.  In failure cases, the physical memory will
    //      generally not be mapped, meaning that memory manager
    //      initialization can fail.
    //

    Status = OslProcessApplicationPersistentData(LoaderBlock);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Log the entry that indicates that control is about to be transferred to
    // the kernel. This must be done now, since access to firmware services is
    // about to be lost.
    //

    BlBsdLogEntry(BSD_EVENT_LEVEL_INFORMATION,
                  BSD_OSLOADER_EVENT_LAUNCH_OS,
                  NULL,
                  0);

    //
    // Wipe the local, remote and firmware keyboard input buffers before
    // handing off to the kernel. This is to prevent exposure of this data to
    // other users.
    //

    BlDisplayWipeInputBuffer(NULL);

    //
    // Build a map of the platform's memory caching requirements so that the OS
    // can use correct cacheability flags when setting up memory mappings.
    //

    Status = OslFwBuildMemoryCachingRequirementsList(LoaderBlock);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Build any firmware runtime memory mappings required at OS runtime.
    //

    Status = OslFwBuildRuntimeMemoryMap(LoaderBlock);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

#if defined(NT_IUM)

    //
    // With firmware memory maps ready, call the VSM last time
    // while it is allowed to make allocations.
    // If fails, the failure reason will be memory allocation
    // errors, not policy blocks. Treat errors from this
    // step as non-continuable.
    //

    Status = OslVsmSetup(1, OsDeviceId, LoaderBlock);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

#endif

    //
    // Build the memory map for the kernel.
    //
    // N.B. No MM Allocation services can be used after this routine has
    //      called.  Doing so would result in a change to the memory map,
    //      invalidating the memory map created for the kernel.  This
    //      restriction exists for heap allocation services as well, since
    //      these could result in the heap growing, which will leverage the
    //      page allocator.
    //

    BlTraceBuildKernelMemoryMapStart(NULL);
    Status = OslBuildKernelMemoryMap(LoaderBlock,
                                     INITIAL_RESERVE_DESCRIPTORS_COUNT,
                                     &OslFreeMemoryDesctiptorsList,
                                     &OslFreeMemoryDesctiptorsListSize,
                                     &OslMemoryDescriptorBuffer,
                                     &OslFinalMemoryMap,
                                     &OslFinalMemoryMapDescriptorsBuffer);

    BlTraceBuildKernelMemoryMapStop(NULL, Status);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Perform any architecture specific steps required to transfer execution
    // to the kernel.  This routine may NOT use MM (library) services.
    //

    Status = OslArchKernelSetup(1, LoaderBlock);
    if (!NT_SUCCESS(Status)) {
        goto PrepareTargetEnd;
    }

    //
    // Prior to transferring to the kernel, ensure TPM services have been
    // shutdown.  Ignore any error.  If the TPM services are not shutdown
    // properly, the OS may still boot and do so securely.
    //

    BlTpmShutdown();

    //
    // If debugging has been enabled on kernel initialization, prepare the
    // kernel for the boot debugger to remain active.
    //

    if (BlBdDebugTransitionsEnabled() != FALSE) {
        OslPersistBootDebugger(LoaderBlock);
    }

    //
    // Firmware based debug transport (UEFI Debug port) depends on boot services.
    // So need to stop the boot debugger before EfiExitBootServices.
    //

    BlBdStopFirmwareTransport();

    //
    // Record the time that the boot application finished executing, and store
    // the begin and end times into the loader block.
    //

    AppEndTime = BlArchGetPerformanceCounter();
    LoaderBlock->Extension->LoaderPerformanceData->StartTime = AppStartTime;
    LoaderBlock->Extension->LoaderPerformanceData->EndTime = AppEndTime;
    *ExecuteTransition = TRUE;
    return STATUS_SUCCESS;

PrepareTargetEnd:

#if defined(NTKERNEL)

    BlTracePrepareTargetFailure(NULL, Status, 0);

#endif

    OslTerminateRNG();
    OslAbortBoot(Status);
    *ExecuteTransition = FALSE;
    return Status;
}

NTSTATUS
OslRebuildKernelMemoryMap (
    __in ULONG ReserveDescriptors
    )

/*++

Routine Description:

    This routine rebuilds the kernel memory map.

Arguments:

    ReserveDescriptors - Number of descriptors to reserve in a free
        descriptors list.

Return Value:

    NTSTATUS - Return status of the operation.

--*/

{

    NTSTATUS Status;

    //
    // Free original allocations and initialize list heads.
    //

    if (OslMemoryDescriptorBuffer != NULL) {
        BlMmFreePages(OslMemoryDescriptorBuffer);
        OslMemoryDescriptorBuffer = NULL;
    }

    if (OslFinalMemoryMapDescriptorsBuffer.Buffer != NULL) {
        BlMmFreeHeap(OslFinalMemoryMapDescriptorsBuffer.Buffer);
        OslFinalMemoryMapDescriptorsBuffer.Buffer = NULL;
    }

    InitializeListHead(&OslFreeMemoryDesctiptorsList);
    InitializeListHead(&OslLoaderBlock->MemoryDescriptorListHead);
    InitializeListHead(&OslFinalMemoryMap);

    //
    // Build a new memory map providing a desired free descriptor
    // list size.
    //

    Status = OslBuildKernelMemoryMap(OslLoaderBlock,
                                     ReserveDescriptors,
                                     &OslFreeMemoryDesctiptorsList,
                                     &OslFreeMemoryDesctiptorsListSize,
                                     &OslMemoryDescriptorBuffer,
                                     &OslFinalMemoryMap,
                                     &OslFinalMemoryMapDescriptorsBuffer);

    return Status;
}

#pragma code_seg(push)
#pragma code_seg()

NTSTATUS
OslExecuteTransition (
    VOID
    )

/*++

Routine Description:

    This routine completes final steps of the OS loader preparation and jumps
    to the target kernel entry point.

    If succeeds, this routine never returns.

Arguments:

    None.

Return Value:

    NTSTATUS - Return status of the operation.

--*/

{

    PLOADER_PARAMETER_BLOCK LoaderBlock;
    NTSTATUS Status;

    LoaderBlock = OslLoaderBlock;

    //
    // Perform any firmware related steps required to transfer execution to the
    // kernel.  After this call, firmware services are no longer available.
    // This routine may NOT use MM (library) services.
    //

    Status = OslFwKernelSetup(1, LoaderBlock);
    if (!NT_SUCCESS(Status)) {
        goto ExecuteTransitionEnd;
    }

    //
    // Perform any architecture specific steps required to transfer execution
    // to the kernel.  This routine may NOT use MM (library) services or
    // firmware services.
    //

    Status = OslArchKernelSetup(2, LoaderBlock);
    if (!NT_SUCCESS(Status)) {
        goto ExecuteTransitionEnd;
    }

    //
    // Launch hypervisor after there is no more firmware call.
    //

    OslArchHypervisorSetup(1,
                           LoaderBlock,
                           OslSystemHiveHandle);

    //
    // Perform the final phase of initialization required for hypercall setup.
    //

    Status = OslArchHypercallSetup(1);
    if (!NT_SUCCESS(Status)) {
        goto ExecuteTransitionEnd;
    }

    //
    // Launch Virtual Secure Mode, which cannot be initialized until the
    // hypervisor is running.
    //

#if defined(NT_IUM)

    //
    // Check if the IOMMU state satisfies the VSM launch policy.
    //

    Status = BlVsmCheckSystemPolicy(VsmPolicyPhaseLaunchVsmPhase1, Status);
    if (!NT_SUCCESS(Status)) {
        goto ExecuteTransitionEnd;
    }

    Status = OslVsmSetup(2, INVALID_DEVICE_ID, LoaderBlock);
    if (!NT_SUCCESS(Status)) {
        Status = BlVsmCheckSystemPolicy(VsmPolicyPhaseVsmPhase1Complete, Status);
        if (!NT_SUCCESS(Status)) {
            goto ExecuteTransitionEnd;
        }
    }

    Status = STATUS_SUCCESS;

#endif

#if !defined(NTKERNEL)

    //
    // Stop the boot debugger, in preparation for transfer to the kernel.
    //

    BlBdStop();

#endif

    //
    // Transfer execution to the kernel.
    //

    OslArchTransferToKernel(LoaderBlock, OslEntryPoint);

    //
    // N.B. Firmware services are unavailable upon successful completion of
    //      OslFwKernelSetup(1, _x).  All continuation paths require firmware
    //      services.
    //

    while (1) {
        NOTHING;
    }

ExecuteTransitionEnd:
    return Status;
}

#pragma code_seg(pop)

VOID
OslAbortBoot (
    __in NTSTATUS Status
    )

/*++

Routine Description:

    This routine undoes actions and frees allocations made when preparing the
    OS load target.

Arguments:

    NTSTATUS - Abort status code.

Return Value:

    None.

--*/

{

    GUID HvlApplicationGuid = HVL_APPLICATION_GUID;

    if (OslResetBootStatus != FALSE) {
        if ((!NT_SUCCESS(Status)) && (Status != STATUS_CANCELLED)) {
            if (OslBootAttemptCount >= OslBootAttemptMaximum) {
                OslBootAttemptCount = OslBootAttemptMaximum - 1;
            }
        }

        OslSetBootStatusData(OslCurrentBootSucceeded,
                             OslCurrentBootShutdown,
                             OslCurrentBootCheckpoint,
                             OslBootCountUpdateRequestForAbort,
                             OslBootAttemptCount);
    }

    OslDestroyResumeContext();

    //
    // Deallocate HvLoader resources
    //

    OslArchHypervisorCleanup();

    //
    // Do not persist the persistent allocations from HvLoader to the parent
    // application.
    //

    (VOID)BlPdDestroyPersistentAllocations(&HvlApplicationGuid);

    //
    // Unload any firmware drivers that were loaded directly by the OS loader.
    //

    OslpUnloadFirmwareDrivers(&OslLoadedFirmwareDriverList);

    ASSERT(IsListEmpty(&OslLoadedFirmwareDriverList));

    BlBsdCloseLog();
    if (BlGetBootDevice() != OslLoadDevice) {
        BlMmFreeHeap(OslLoadDevice);
    }

    if (OslSystemRoot != NULL) {
        BlMmFreeHeap(OslSystemRoot);
    }

    if (OslLoadOptions != NULL) {
        BlMmFreeHeap(OslLoadOptions);
    }

    if (OslLoaderBlock != NULL) {
        if (NULL != OslLoaderBlock->Extension->BgContext) {
            OslDisplayDestroy(OslLoaderBlock->Extension->BgContext);
        }

        BlMmFreeHeap(OslLoaderBlock);
    }

    if ((ULONG)-1 != OslElamHiveHandle) {
        OslUnloadHive(OslElamHiveHandle);
    }

    if ((ULONG)-1 != OslMachineHiveHandle) {
        OslUnloadHive(OslMachineHiveHandle);
    }

    if ((ULONG)-1 != OslSystemHiveHandle) {
        OslUnloadHive(OslSystemHiveHandle);
    }

    if ((ULONG)-1 != OslImcHiveHandle) {
        OslUnloadHive(OslImcHiveHandle);
    }

    TraceLoggingUnregister(OslBootProvider);

    return;
}

//
// ----------------------------------------------------------- Private Routines
//

BOOLEAN
OslpAdvancedOptionsRequested (
    VOID
    )

/*++

Routine Description:

    This routine returns a boolean value indicating whether or not the user
    requested an advanced boot menu option.

Arguments:

    None.

Return Value:

    TRUE if an advanced option was requested. FALSE otherwise.

--*/

{

    BOOLEAN AdvancedOptions;
    BOOLEAN BootOptions;
    NTSTATUS Status;

    //
    // Check if the user has requested to edit the boot options.
    //

    BootOptions = FALSE;
    Status = BlGetApplicationOptionBoolean(
                 BCDE_LIBRARY_TYPE_DISPLAY_OPTIONS_EDIT,
                 &BootOptions);

    if (NT_SUCCESS(Status) && (BootOptions != FALSE)) {
        return TRUE;
    }

    //
    // Determine whether the user has requested advanced options.
    //

    AdvancedOptions = FALSE;
    Status = BlGetApplicationOptionBoolean(
                 BCDE_LIBRARY_TYPE_DISPLAY_ADVANCED_OPTIONS,
                 &AdvancedOptions);

    //
    // Display the advanced OS options menu.
    //

    if (NT_SUCCESS(Status) && (AdvancedOptions != FALSE)) {
        return TRUE;
    }

    return FALSE;
}

NTSTATUS
OslpLoadEarlyLaunchHive (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in DEVICE_ID OsDeviceId,
    __in PUNICODE_STRING OsFilePath
    )

/*++

Routine Description:

    This routine loads the Early Launch Anti-Malware registry hive into memory
    and attaches it to the loader block.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block where the
        early launch hive information will be stored.

    OsDeviceId - Supplies the device ID for the OS partition.

    OsFilePath - Supplies a pointer to the file path of the OS's system root
        directory.

Return Value:

    NT status code.

--*/

{

    NTSTATUS Status;

    //
    // Load the ELAM hive if it is present and attach it to the loader
    // block. Ignore failures to load the hive.
    //

    Status = OslAttachHiveToLoaderBlock(OsDeviceId,
                                        OsFilePath,
                                        LoaderBlock,
                                        LHB_SYSTEM_HIVE | LHB_ELAM_HIVE,
                                        ELAM_HIVE_NAME,
                                        L"MACHINE",
                                        ELAM_MOUNT_ROOT,
                                        &OslElamHiveHandle);

    if (!NT_SUCCESS(Status)) {
        Status = STATUS_SUCCESS;
        goto LoadEarlyLaunchHiveEnd;
    }

    //
    // Upon a successful load of the ELAM hive, measure it. This is
    // best effort and should not stop the system from booting.
    //

    OslMeasureEarlyLaunchHive(OslElamHiveHandle);

LoadEarlyLaunchHiveEnd:
    return Status;
}

NTSTATUS
OslpLoadSystemHive (
    __in DEVICE_ID OsDeviceId,
    __in PUNICODE_STRING OsFilePath,
    __deref_out_bcount(*RegistryLength) PVOID *RegistryBase,
    __out PULONG RegistryLength,
    __out PLOADER_HIVE_RECOVERY_INFO RecoveryInfo,
    __out PULONG SystemHiveHandle
    )

/*++

Routine Description:

    Loads and initializes the system hive.  The system hive is required for
    kernel initialization and is passed as part of the loader block.
    Additionally, the loader will receive policy information such as the
    boot driver list from the system hive.

    This routine will read the system hive into memory and save it's location
    and size, which will be stored in the loader block by the caller.

Arguments:

    OsDeviceId - Handle for the device containing the Operating System.

    OsFilePath - File path to the OS directory.

    RegistryBase - Supplies a pointer to the preferred address for the image's
        allocation. *ImageBase == NULL indicates there is no preference.  On
        output, contains a pointer to the base address of the loaded image.

    RegistryLength - Supplies a pointer to receive the size of the loaded image.

    RecoveryInfo - Supplies a pointer to a buffer that upon success receives
                   information about the recovery performed on the hive.

    SystemHiveHandle - Handle for all subsequent references to the system
        hive.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY when an allocation fails.
    STATUS_NOT_FOUND if the specified hive does not exist.

--*/

{

    PBOOT_ENVIRONMENT_DEVICE AllocatedDevice;
    DEVICE_ID DeviceId;
    DEVICE_ID DeviceIdToClose;
    PWCHAR HiveLogPath1;
    PWCHAR HiveLogPath2;
    PWCHAR HivePath;
    UINTN HiveLogPath1Size;
    UINTN HiveLogPath2Size;
    UINTN HivePathSize;
    NTSTATUS Status;

    AllocatedDevice = NULL;
    DeviceId = INVALID_DEVICE_ID;
    DeviceIdToClose = INVALID_DEVICE_ID;
    HiveLogPath1 = NULL;
    HiveLogPath2 = NULL;
    HivePath = NULL;

    Status = BlGetApplicationOptionDevice(BCDE_OSLOADER_TYPE_SYSTEM_DATA_DEVICE,
                                          &AllocatedDevice,
                                          NULL);

    if (NT_SUCCESS(Status)) {

        //
        // The BCD store specifies an override for the device containing the
        // SYSTEM hive. Open the device and attempt to load the hive.
        //

        Status = BlDeviceOpen(AllocatedDevice,
                              OPEN_READ_WRITE,
                              &DeviceIdToClose);

        if (!NT_SUCCESS(Status)) {
            OslFatalError(OSL_ERROR_REGISTRY,
                          (ULONG)HivePath,
                          Status,
                          2);
        }

        DeviceId = DeviceIdToClose;

    } else if (Status == STATUS_NOT_FOUND) {
        DeviceId = OsDeviceId;

    } else {
        OslFatalError(OSL_ERROR_REGISTRY,
                      (ULONG)HivePath,
                      Status,
                      1);
    }

    //
    // Allocate a buffer for the path to the system hive.
    //

    HiveLogPath2Size = HiveLogPath1Size = HivePathSize =
        OsFilePath->Length + sizeof(SYSTEM_HIVE_PATH);

    HiveLogPath2Size += sizeof(SYSTEM_HIVE_LOGNAME2);
    HiveLogPath1Size += sizeof(SYSTEM_HIVE_LOGNAME1);
    HivePathSize += sizeof(SYSTEM_HIVE_NAME);
    HiveLogPath2 = BlMmAllocateHeap(HiveLogPath2Size);
    HiveLogPath1 = BlMmAllocateHeap(HiveLogPath1Size);
    HivePath = BlMmAllocateHeap(HivePathSize);
    if ((HivePath == NULL) || (HiveLogPath1 == NULL) ||
        (HiveLogPath2 == NULL)) {

        Status = STATUS_NO_MEMORY;
        goto LoadSystemHiveEnd;
    }

    swprintf_s(HivePath,
               HivePathSize / sizeof(WCHAR),
               L"%s%s%s",
               OsFilePath->Buffer,
               SYSTEM_HIVE_PATH,
               SYSTEM_HIVE_NAME);

    swprintf_s(HiveLogPath1,
               HiveLogPath1Size / sizeof(WCHAR),
               L"%s%s%s",
               OsFilePath->Buffer,
               SYSTEM_HIVE_PATH,
               SYSTEM_HIVE_LOGNAME1);

    swprintf_s(HiveLogPath2,
               HiveLogPath2Size / sizeof(WCHAR),
               L"%s%s%s",
               OsFilePath->Buffer,
               SYSTEM_HIVE_PATH,
               SYSTEM_HIVE_LOGNAME2);

    //
    // Attempt to load an initialize the system hive.
    //

    Status = OslLoadAndInitializeHive(DeviceId,
                                      HivePath,
                                      TRUE,
                                      HiveLogPath1,
                                      HiveLogPath2,
                                      RegistryBase,
                                      RegistryLength,
                                      RecoveryInfo,
                                      SystemHiveHandle);

    if (!NT_SUCCESS(Status)) {
        OslFatalError(OSL_ERROR_REGISTRY,
                      (ULONG_PTR)HivePath,
                      Status,
                      0);
    }

LoadSystemHiveEnd:
    if (HiveLogPath2 != NULL) {
        BlMmFreeHeap(HiveLogPath2);
    }

    if (HiveLogPath1 != NULL) {
        BlMmFreeHeap(HiveLogPath1);
    }

    if (HivePath != NULL) {
        BlMmFreeHeap(HivePath);
    }

    if (DeviceIdToClose != INVALID_DEVICE_ID) {
        BlDeviceClose(DeviceIdToClose);
    }

    if (AllocatedDevice != NULL) {
        BlMmFreeHeap(AllocatedDevice);
    }

    return Status;
}

NTSTATUS
OslpReinitializeSystemHive (
    __in ULONG SystemHiveHandle,
    __in PUNICODE_STRING OsFilePath
    )

/*++

Routine Description:

    This routine reinitializes the system registry hive after the advanced
    options have been processed. This supports options that require enabling
    registry settings or setting a different control set as the current control
    set.

Arguments:

    SystemHiveHandle - Supplies a handle to the system registry hive.

    OsFilePath - Supplies a pointer to the file path of the OS directory.

Return Value:

    NTSTATUS Code.

--*/

{

    PWCHAR HivePath;
    UINTN HivePathSize;
    NTSTATUS Status;

    HivePath = NULL;
    Status = OslReinitializeSystemHive(SystemHiveHandle);
    if (!NT_SUCCESS(Status)) {
        HivePathSize = OsFilePath->Length + sizeof(SYSTEM_HIVE_PATH);
        HivePathSize += sizeof(SYSTEM_HIVE_NAME);
        HivePath = BlMmAllocateHeap(HivePathSize);
        if (HivePath == NULL) {
            Status = STATUS_NO_MEMORY;
            goto ReinitializeSystemHiveEnd;
        }

        swprintf_s(HivePath,
                   HivePathSize / sizeof(WCHAR),
                   L"%s%s%s",
                   OsFilePath->Buffer,
                   SYSTEM_HIVE_PATH,
                   SYSTEM_HIVE_NAME);

        OslFatalError(OSL_ERROR_REGISTRY,
                      (ULONG_PTR)HivePath,
                      Status,
                      0);
    }

ReinitializeSystemHiveEnd:
    if (HivePath != NULL) {
        BlMmFreeHeap(HivePath);
    }

    return Status;
}


BOOLEAN
OslpIsMobileCoreBoot (
    __in PVOID ApiSetSchema
    )

/*++

Routine Description:

    This routine returns true if this is the MobileCore SKU.
    This function is used as a secure way to enable/disable specialized
    Windows Phone behavior.

Arguments:

    ApiSetSchema - Supplies a pointer to the API set schema

Return Value:

    True if this is the MobileCore SKU, false otherwise.

--*/

{

    UNICODE_STRING ApiSetHost;
    BOOLEAN ApiSetResolved;
    BOOLEAN MobileCoreApiSetPresent;
    NTSTATUS Status;

    MobileCoreApiSetPresent = FALSE;

    //
    // Try to resolve the API set and verify the result.
    //

    Status = ApiSetResolveToHost(ApiSetSchema,
                                 &OslMobileCoreBootExtension,
                                 NULL,
                                 &ApiSetResolved,
                                 &ApiSetHost);

    if (!NT_SUCCESS(Status)) {
        goto IsMobileCoreBootEnd;
    }

    if (ApiSetResolved == FALSE) {
        goto IsMobileCoreBootEnd;
    }

    //
    // If all the checks passed then the MobileCore API set is present and
    // this is MobileCore (and Windows Phone).
    //

    MobileCoreApiSetPresent = TRUE;

IsMobileCoreBootEnd:
    return MobileCoreApiSetPresent;
}

NTSTATUS
OslpLoadAllModules (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __in ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine will load all images that are required for kernel
    initialization into memory.  This includes the kernel itself, the hal
    and the boot drivers.

Arguments:

    LoaderBlock - A pointer to the loader parameter block.

    OsFilePath - The file path to the Operating System directory.

    OsDeviceId - Handle to the device containing the Operating System.

    SystemHiveHandle - Handle for the system hive.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if there is insufficient memory to perform a request.
    STATUS_INVALID_IMAGE_FORMAT if relocations can not be applied to a PE
        image being loaded.
    STATUS_NO_SUCH_FILE if a file required to load does not exist.
    STATUS_UNSUCCESSFUL if file system can not be mounted on OsDeviceId.

--*/

{

    PVOID AcpiTable;
    ULONG AcpiTableSize;
    BOOLEAN BoolRet;
    ULONG BootDriverFlags;
    PVOID BootParameters;
    BOOLEAN DisableElamDrivers;
    ULONG ExtraSize;
    FILE_ID FileId;
    FILE_INFORMATION FileInformation;
    PULONG Flags;
    PKLDR_DATA_TABLE_ENTRY HalDataTableEntry;
    PVOID HalImageBase;
    UNICODE_STRING HalName;
    ULONG HalSize;
    BOOLEAN IsMobileCore;
    PKLDR_DATA_TABLE_ENTRY KdDataTableEntry;
    PKLDR_DATA_TABLE_ENTRY KdExtensionDataTableEntry;
    PKLDR_DATA_TABLE_ENTRY KernelDataTableEntry;
    PVOID KernelImageBase;
    UNICODE_STRING KernelName;
    ULONG KernelSize;
    BOOLEAN LastKnownGoodBoot;
    USHORT OsFilePathOffset;
    ULONG StartingPage;
    NTSTATUS Status;
    ULONG SystemImageAttributes;
    PVOID SystemImageBase;
    ULONGLONG SystemImageSize;
    USHORT SystemPathOffset;
    PKLDR_DATA_TABLE_ENTRY UpdateDataTableEntry;
    UNICODE_STRING UpdateName;
    BOOLEAN UseNoUpdate;
    PVOID WpbtTable;

    BootDriverFlags = 0;
    Flags = NULL;
    OsFilePathOffset = OsFilePath->Length;
    SystemImageBase = NULL;
    UseNoUpdate = FALSE;
    KdExtensionDataTableEntry = NULL;

    OslConfigureRNG(LoaderBlock);

    //
    // Determine the kernel and hal to load.
    //

    RtlInitUnicodeString(&KernelName, NULL);
    RtlInitUnicodeString(&HalName, NULL);
    RtlInitUnicodeString(&UpdateName, NULL);
    Status = OslpDetermineKernelHal(&KernelName, &HalName);
    if (!NT_SUCCESS(Status)) {
        goto OslpLoadAllModulesEnd;
    }

    //
    // If HvLoader was successful microcode updates have been taken care off by
    // HvLoader for BSP and will be applied by hypervisor for APs. In this case
    // skip load of microcode update library.
    //

    if (OslArchHvlInitialized()) {
        UseNoUpdate = TRUE;

    }
    else {
        //
        // Determine the microcode update library to load.  If this call fails
        // with STATUS_NOT_SUPPORTED, then this isn't fatal.  It just means that
        // no update library should be loaded since the CPU does not support
        // microcode update.  If no library is loaded for x86 or x64, boot will
        // fail later when it is unable to resolve the HAL imports.
        //

        Status = OslArchDetectUpdateLibrary(&UpdateName);
        if (Status == STATUS_NOT_SUPPORTED) {
            Status = STATUS_SUCCESS;
            UseNoUpdate = TRUE;
        }

        if (!NT_SUCCESS(Status)) {
            goto OslpLoadAllModulesEnd;
        }
    }

    //
    // All files are located in the subdirectory system32.
    //

    BoolRet = BlAppendUnicodeToString(OsFilePath, L"system32\\");
    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OslpLoadAllModulesEnd;
    }

    SystemPathOffset = OsFilePath->Length;

    //
    // Load kernel and HAL with the following requirements:
    //   1. Both are loaded in a large page, or if their combined size
    //      exceeds a large page, multiple consecutive large pages.
    //   2. Starting address of the first image is randomly selected,
    //      but the kernel image base address must be aligned according
    //      to OSL_IMAGE_KERNEL_ALIGNMENT.
    //   3. One is loaded immediately after the other, with the order
    //      randomly selected.
    //

    //
    // Get the image size of kernel.
    //

    BoolRet = BlAppendUnicodeToString(OsFilePath, KernelName.Buffer);
    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OslpLoadAllModulesEnd;
    }

    Status = BlImgGetPEImageSize(OsDeviceId,
                                 OsFilePath->Buffer,
                                 &KernelSize);

    if (!NT_SUCCESS(Status)) {
        OslFatalError(OSL_ERROR_KERNEL,
                      (ULONG_PTR)OsFilePath->Buffer,
                      Status,
                      0);

        goto OslpLoadAllModulesEnd;
    }

    OsFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = SystemPathOffset;

    //
    // Get the image size of HAL.
    //

    BoolRet = BlAppendUnicodeToString(OsFilePath, HalName.Buffer);
    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OslpLoadAllModulesEnd;
    }

    Status = BlImgGetPEImageSize(OsDeviceId,
                                 OsFilePath->Buffer,
                                 &HalSize);

    if (!NT_SUCCESS(Status)) {
        OslFatalError(OSL_ERROR_HAL,
                      (ULONG_PTR)OsFilePath->Buffer,
                      Status,
                      0);

        goto OslpLoadAllModulesEnd;
    }

    OsFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = SystemPathOffset;

    //
    // Allocate an image buffer large enough for both kernel and HAL
    // plus some extra space with large page attribute. The extra space
    // is needed so that the location of kernel/HAL image can be randomly
    // chosen within the buffer.
    //

    ExtraSize = (OSL_IMAGE_RANDOMNESS + 1) * OSL_IMAGE_KERNEL_ALIGNMENT;
    ExtraSize <<= PAGE_SHIFT;

    SystemImageAttributes = MEMORY_ATTRIBUTE_ALLOCATION_KSEG |
                            MEMORY_ATTRIBUTE_ALLOCATION_LARGE_PAGE;

    SystemImageSize = (ULONGLONG)KernelSize + HalSize + ExtraSize;
    Status = BlImgAllocateImageBuffer(&SystemImageBase,
                                      SystemImageSize,
                                      OSL_MEMORY_TYPE_SYSTEM_CODE,
                                      SystemImageAttributes,
                                      1,
                                      IMAGE_FLAGS_NONE);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadAllModulesEnd;
    }

    //
    // Randomly determine the starting address of kernel/HAL image within
    // the image buffer just allocated. The starting address of the kernel
    // must be aligned according to OSL_IMAGE_KERNEL_ALIGNMENT. Also randomly
    // determine the order in which kernel and HAL are loaded. Use performance
    // counter as a random number.
    //
    // N.B. The last 4 bits of performance counter on some chips may
    //      remain unchanged at all times.
    //

#if (OSL_IMAGE_RANDOMNESS > 0)

    StartingPage = ((ULONG)(OslGenRandom())) %
                   (OSL_IMAGE_RANDOMNESS * OSL_IMAGE_KERNEL_ALIGNMENT);

#else

    StartingPage = 0;

#endif

    if (((OslGenRandom()) % 2) == 0) {
        StartingPage = ROUND_UP(StartingPage, OSL_IMAGE_KERNEL_ALIGNMENT);
        KernelImageBase = Add2Ptr(SystemImageBase,
                                  (((ULONG_PTR) StartingPage) << PAGE_SHIFT));

        HalImageBase = Add2Ptr(KernelImageBase, KernelSize);

    } else {
        StartingPage += HalSize >> PAGE_SHIFT;
        StartingPage = ROUND_UP(StartingPage, OSL_IMAGE_KERNEL_ALIGNMENT);
        KernelImageBase = Add2Ptr(SystemImageBase,
                                  (((ULONG_PTR) StartingPage) << PAGE_SHIFT));

        HalImageBase = Add2Ptr(KernelImageBase, -((LONG)HalSize));
    }

    //
    // Load the kernel.
    //

    BoolRet = BlAppendUnicodeToString(OsFilePath, KernelName.Buffer);
    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OslpLoadAllModulesEnd;
    }

    Status = OslLoadSystemImage(OsDeviceId,
                                OSL_MEMORY_TYPE_SYSTEM_CODE,
                                OsFilePath->Buffer,
                                KERNEL_DEFAULT_NAME,
                                KernelImageBase,
                                KernelSize,
                                &LoaderBlock->LoadOrderListHead,
                                &KernelDataTableEntry);

    if (!NT_SUCCESS(Status)) {
        OslFatalError(OSL_ERROR_KERNEL,
                      (ULONG_PTR)OsFilePath->Buffer,
                      Status,
                      0);

        goto OslpLoadAllModulesEnd;
    }

    Status = OslpValidateImageType(KernelDataTableEntry->DllBase,
                                   KernelDataTableEntry->SizeOfImage);

    if (!NT_SUCCESS(Status)) {
        OslFatalError(OSL_ERROR_KERNEL,
                      (ULONG_PTR)OsFilePath->Buffer,
                      Status,
                      0);

        goto OslpLoadAllModulesEnd;
    }

    //
    // Record kernel entry point for future use.
    //

    OslEntryPoint = KernelDataTableEntry->EntryPoint;

    //
    // Load the hal.
    //

    OsFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = SystemPathOffset;
    BoolRet = BlAppendUnicodeToString(OsFilePath, HalName.Buffer);
    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OslpLoadAllModulesEnd;
    }

    Status = OslLoadSystemImage(OsDeviceId,
                                OSL_MEMORY_TYPE_SYSTEM_CODE,
                                OsFilePath->Buffer,
                                HAL_DEFAULT_NAME,
                                HalImageBase,
                                HalSize,
                                &LoaderBlock->LoadOrderListHead,
                                &HalDataTableEntry);

    if (!NT_SUCCESS(Status)) {
        OslFatalError(OSL_ERROR_HAL,
                      (ULONG_PTR)OsFilePath->Buffer,
                      Status,
                      0);

        goto OslpLoadAllModulesEnd;
    }

    OsFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = SystemPathOffset;

    //
    // Load the ApiSet schema dll. The schema is used to map "virtual binaries"
    // to real ones.
    //
    // N.B. This must happen before any imports are resolved because any
    //      binaries, including the kernel and hal can be linked against api
    //      sets.
    //

    Status = OslLoadApiSetSchema(LoaderBlock,
                                 SystemHiveHandle,
                                 OsFilePath,
                                 OsDeviceId);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadAllModulesEnd;
    }

    //
    // Configure the background update settings based on the result of a
    // secure MobileCore SKU check.
    //

    IsMobileCore = OslpIsMobileCoreBoot(OslApiSetSchema);
    BgConfigureBackgroundUpdate(IsMobileCore);

    //
    // Load optional acpitabl.dat file.
    //

    OsFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = SystemPathOffset;
    Status = OslLoadOptionalAcpiTableData(LoaderBlock,
                                          OsFilePath,
                                          OsDeviceId);

    //
    // Initialize the debugging device for the kernel and load the transports.
    // This must be done here because the kernel links to the debug transport
    // so it must be loaded before imports are resolved incorrectly.
    //

    Status = OslKdInitialize(LoaderBlock,
                             OsFilePath,
                             OsDeviceId,
                             &KdDataTableEntry,
                             &KdExtensionDataTableEntry);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadAllModulesEnd;
    }

    //
    // Load the microcode update library.  If the previous call to
    // OslArchDetectUpdateLibrary returned STATUS_NOT_SUPPORTED then no
    // manufacturer specific library should be loaded.  The default one will
    // be loaded when the HAL imports are resolved.
    //
    // All microcode update DLLs are given the friendly name of MCUPDATE.DLL
    //  to resolve all MCUPDATE imports. Therefore, this step must take place
    // before loading the imports for the kernel and hal.
    //

    if (UseNoUpdate == FALSE) {
        BoolRet = BlAppendUnicodeToString(OsFilePath, UpdateName.Buffer);
        if (BoolRet == FALSE) {
            Status = STATUS_NO_MEMORY;
            goto OslpLoadAllModulesEnd;
        }

        Status = OslLoadImage(OsDeviceId,
                              OSL_MEMORY_TYPE_SYSTEM_CODE,
                              OsFilePath->Buffer,
                              UPDATE_DEFAULT_NAME,
                              NULL,
                              NULL,
                              0,
                              &LoaderBlock->LoadOrderListHead,
                              &UpdateDataTableEntry,
                              0,
                              0,
                              IMAGE_FLAGS_NONE,
                              OSL_IMAGE_FLAGS_LOAD_IMPORTS,
                              NULL);

        //
        // If the manufacturer specific microcode update library is not present,
        // then the generic update.dll microcode update library will be loaded
        // when the HAL imports are resolved.  If the attempt to load the update
        // library failed for any other reason, then this is fatal.
        //

        if (Status == STATUS_NO_SUCH_FILE) {
            Status = STATUS_SUCCESS;
        }

        if (!NT_SUCCESS(Status)) {
            OslFatalError(OSL_ERROR_UPDATE_DLL,
                          (ULONG_PTR)OsFilePath->Buffer,
                          Status,
                          0);

            goto OslpLoadAllModulesEnd;
        }
    }

    //
    // Optional WPBT (Windows Platform Binary Table)
    // Search for the table in RSDT/XSDT first. If not found search
    // well known locations.
    //

    Status = BlUtlGetAcpiTable(&WpbtTable, WPBT_SIGNATURE);
    if (!NT_SUCCESS(Status)) {
        Status = OslLoadOptionalAcpiWpbt(&AcpiTable, &AcpiTableSize);
        if (NT_SUCCESS(Status)) {
            OslMergeAcpiTableData(LoaderBlock, AcpiTable, AcpiTableSize);
        }
    }

    OsFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = SystemPathOffset;

    //
    // Load OS Extensions.
    //

    Status = OslLoadOsExtensions(LoaderBlock,
                                 OsFilePath,
                                 OsDeviceId,
                                 SystemHiveHandle);

    OsFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = SystemPathOffset;

    //
    // Load all the system code imports (bootvid, etc).
    //

    BoolRet = BlAppendUnicodeToString(OsFilePath, KernelName.Buffer);
    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OslpLoadAllModulesEnd;
    }

    Status = OslLoadSystemImageImports(OsDeviceId,
                                       OSL_MEMORY_TYPE_SYSTEM_CODE,
                                       OsFilePath->Buffer,
                                       KERNEL_DEFAULT_NAME,
                                       &LoaderBlock->LoadOrderListHead,
                                       &KernelDataTableEntry);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadAllModulesEnd;
    }

    OsFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = SystemPathOffset;

    BoolRet = BlAppendUnicodeToString(OsFilePath, HalName.Buffer);
    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OslpLoadAllModulesEnd;
    }

    Status = OslLoadSystemImageImports(OsDeviceId,
                                       OSL_MEMORY_TYPE_SYSTEM_CODE,
                                       OsFilePath->Buffer,
                                       HAL_DEFAULT_NAME,
                                       &LoaderBlock->LoadOrderListHead,
                                       &HalDataTableEntry);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadAllModulesEnd;
    }

    KernelDataTableEntry->Flags |= LDRP_DONT_RELOCATE;
    HalDataTableEntry->Flags |= LDRP_DONT_RELOCATE;
    KdDataTableEntry->Flags |= LDRP_DONT_RELOCATE;


    //
    // Ensure that the kernel is the first entry in the load order list, the hal
    // is second and kd is the third entry.  Depending on the order that the
    // kernel imports the hal and it's other imports, the location of the hal
    // can vary.
    //

    if (KdExtensionDataTableEntry != NULL) {
     KdExtensionDataTableEntry->Flags |= LDRP_DONT_RELOCATE;
     RemoveEntryList(&KdExtensionDataTableEntry->InLoadOrderLinks);
     InsertHeadList(&LoaderBlock->LoadOrderListHead,
                    &KdExtensionDataTableEntry->InLoadOrderLinks);
    }

    RemoveEntryList(&KdDataTableEntry->InLoadOrderLinks);
    InsertHeadList(&LoaderBlock->LoadOrderListHead,
                &KdDataTableEntry->InLoadOrderLinks);

    RemoveEntryList(&HalDataTableEntry->InLoadOrderLinks);
    InsertHeadList(&LoaderBlock->LoadOrderListHead,
                   &HalDataTableEntry->InLoadOrderLinks);

    RemoveEntryList(&KernelDataTableEntry->InLoadOrderLinks);
    InsertHeadList(&LoaderBlock->LoadOrderListHead,
                   &KernelDataTableEntry->InLoadOrderLinks);

    Status = BlGetApplicationOptionBoolean(
                 BCDE_OSLOADER_TYPE_LAST_GOOD_SETTINGS,
                 &LastKnownGoodBoot);

    if (!NT_SUCCESS(Status)) {
        LastKnownGoodBoot = FALSE;
    }

    //
    // Determine the file system of the boot partition.  It is imperative that
    // the file system driver is loaded for the Operating System device.
    //

    Status = BlFileOpen(OsDeviceId, OsFilePath->Buffer, OPEN_READ, &FileId);
    if (!NT_SUCCESS(Status)) {
        goto OslpLoadAllModulesEnd;
    }

    Status = BlFileGetInformation(FileId, &FileInformation);
    if (!NT_SUCCESS(Status)) {
        goto OslpLoadAllModulesEnd;
    }

    //
    // If a file system is not specified for the OS device, assume NTFS.
    //
    // TODO: The WIM filesystem code does not currently set
    //       FileInformation.FileSystemName.  It seems like the cleaner
    //       solution would be for the WIM code to specify the WIM driver's
    //       name.  If the boot device is BLOCK_IO_FILE, we can look at the
    //       underlying device and load its driver as well.
    //

    if (FileInformation.FileSystemName == NULL) {
        FileInformation.FileSystemName = L"NTFS";
    }

    //
    // iSCSI boot is determined by the presence of the iSCSI ACPI table or
    // iSCSI boot table.  If the platform only populated an iSCSI boot table,
    // create an ACPI table so the OS only has to query the one table.
    // This needs to be done prior to determining the boot driver flags so
    // that all required drivers get boot loaded.
    //

    Status = BlUtlGetAcpiTable(&BootParameters, ISCSI_BOOT_SIGNATURE);
    if (!NT_SUCCESS(Status)) {
        Status = OslIscsiBootMakeAcpiTable(
                     &AcpiTable,
                     &AcpiTableSize);

        //
        // Merge the iSCSI Acpi table into the LoaderBlock.
        //

        if (NT_SUCCESS(Status)) {
            OslMergeAcpiTableData(LoaderBlock, AcpiTable, AcpiTableSize);
        }
    }

    //
    // Ensure that all OS drivers needed to boot from the boot device are
    // loaded regardless of the start type.
    //

    Status = OslpGetBootDriverFlags(LoaderBlock,
                                    SystemHiveHandle,
                                    OsDeviceId,
                                    &BootDriverFlags);

    if (BootDriverFlags != 0) {
        Flags = &BootDriverFlags;
    }

    //
    // Load all boot drivers and their dependencies into memory.
    //

    Status = OslGetBootDrivers(LoaderBlock,
                               SystemHiveHandle,
                               FileInformation.FileSystemName,
                               Flags);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadAllModulesEnd;
    }

    //
    // Load various Core driver groups and ELAM drivers. The drivers are broken
    // into the following groups:
    //
    //  1. Core Drivers: This group consists of drivers that ELAM drivers and
    //         3rd party Core Extensions depend upon (e.g. WDF, CNG.sys). All
    //         drivers in this group should be MS-supplied and thus MS-signed.
    //
    //  2. ELAM drivers. This group consists of 3rd party ELAM drivers. These
    //         drivers need to be signed with ELAM certificate.
    //
    //  3 Core Extensions: This group consists of 3rd party drivers (viz.
    //         Platform Extensions and Tree drivers) that TPM Core drivers
    //         depend upon. These drivers need to be signed with Core Extension
    //         certificate.
    //
    //  4 TPM Core: This group consists of TPM driver and bus drivers (e.g.
    //         ACPI, PCI) that are necessary to enumerate TPM. All drivers in
    //         this group should be MS-supplied and thus MS-signed.
    //

    OsFilePath->Buffer[OsFilePathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = OsFilePathOffset;
    Status = OslLoadDrivers(&LoaderBlock->CoreDriverListHead,
                            &LoaderBlock->LoadOrderListHead,
                            OsFilePath,
                            OsDeviceId,
                            OSL_IMAGE_FLAGS_CORE_DRIVER,
                            LastKnownGoodBoot);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadAllModulesEnd;
    }

    Status = OslLoadDrivers(&LoaderBlock->TpmCoreDriverListHead,
                            &LoaderBlock->LoadOrderListHead,
                            OsFilePath,
                            OsDeviceId,
                            OSL_IMAGE_FLAGS_CORE_DRIVER,
                            LastKnownGoodBoot);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadAllModulesEnd;
    }

    //
    // Do not load the ELAM driver if the local user disabled it.
    //

    Status = BlGetApplicationOptionBoolean(
                 BCDE_OSLOADER_TYPE_DISABLE_ELAM_DRIVERS,
                 &DisableElamDrivers);

    if ((!NT_SUCCESS(Status) || (DisableElamDrivers == FALSE)) &&
        (IsListEmpty(&LoaderBlock->EarlyLaunchListHead) == FALSE)) {

        Status = OslLoadDrivers(&LoaderBlock->EarlyLaunchListHead,
                                &LoaderBlock->LoadOrderListHead,
                                OsFilePath,
                                OsDeviceId,
                                OSL_IMAGE_FLAGS_EARLY_LAUNCH,
                                LastKnownGoodBoot);

        if (!NT_SUCCESS(Status)) {
            goto OslpLoadAllModulesEnd;
        }

        //
        // Load the Early Launch Anti-Malware registry hive.
        //

        Status = OslpLoadEarlyLaunchHive(LoaderBlock, OsDeviceId, OsFilePath);
        if (!NT_SUCCESS(Status)) {
            goto OslpLoadAllModulesEnd;
        }

    } else {
        OslDestroyDriverList(&LoaderBlock->EarlyLaunchListHead);
    }

    //
    // Load the (3rd party) Core Extensions. These are loaded after all Core
    // drivers and ELAM drivers to let all the Core driver and ELAM imports
    // be loaded and resolved first.
    //

    Status = OslLoadCoreExtensions(LoaderBlock,
                                   OsFilePath,
                                   OsDeviceId,
                                   LastKnownGoodBoot);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadAllModulesEnd;
    }

    //
    // Perform final phase (Phase 1) of Core Extensions filtering. This step
    // needs to be done after all the Core Extensions have been loaded.
    //

    OslFilterCoreExtensions(1, LoaderBlock);

    //
    // Load the remaining boot drivers.
    //

    Status = OslLoadDrivers(&LoaderBlock->BootDriverListHead,
                            &LoaderBlock->LoadOrderListHead,
                            OsFilePath,
                            OsDeviceId,
                            OSL_IMAGE_FLAGS_NONE,
                            LastKnownGoodBoot);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadAllModulesEnd;
    }

    //
    // Load NLS data files.
    //

    Status = OslpLoadNlsData(LoaderBlock,
                             OsFilePath,
                             OsDeviceId,
                             SystemHiveHandle);

    if (!NT_SUCCESS(Status)) {
        OslFatalError(OSL_ERROR_NLS_DATA, Status, 0, 0);
        goto OslpLoadAllModulesEnd;
    }

    //
    // Load all miscellaneous modules (like defective driver database etc).
    //

    OsFilePath->Buffer[OsFilePathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = OsFilePathOffset;
    Status = OslpLoadMiscModules(LoaderBlock,
                                 OsFilePath,
                                 OsDeviceId,
                                 SystemHiveHandle);

    //
    // Load NUMA data.
    //

    OslHiveFindNumaData(SystemHiveHandle,
                        &LoaderBlock->Extension->NumaLocalityInfo,
                        &LoaderBlock->Extension->NumaGroupAssignment);

    //
    // Done loading all modules.
    //

OslpLoadAllModulesEnd:
    OsFilePath->Buffer[OsFilePathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = OsFilePathOffset;
    if (KernelName.MaximumLength != 0) {
        BlMmFreeHeap(KernelName.Buffer);
    }

    if (HalName.MaximumLength != 0) {
        BlMmFreeHeap(HalName.Buffer);
    }

    if (UpdateName.MaximumLength != 0) {
        BlMmFreeHeap(UpdateName.Buffer);
    }

    if (!NT_SUCCESS(Status) && (SystemImageBase != NULL)) {
        BlMmFreePages(SystemImageBase);
    }

    return Status;
}

NTSTATUS
OslpLoadMiscModules (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __in ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine will load all miscellaneous files into memory for further
    processing by the OS. These include files such as the one containing
    machine descriptions for applying firmware specific workarounds
    (biosinfo.inf) etc.

Arguments:

    LoaderBlock - A pointer to the loader parameter block.

    OsFilePath - The file path to the Operating System directory.

    OsDeviceId - Handle to the device containing the Operating System.

    SystemHiveHandle - Handle for the system hive.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if there is insufficient memory to perform a request.
    STATUS_INVALID_IMAGE_FORMAT if relocations can not be applied to a PE
        image being loaded.
    STATUS_NO_SUCH_FILE if a file required to load does not exist.
    STATUS_UNSUCCESSFUL if file system can not be mounted on OsDeviceId.

--*/

{
    BOOLEAN BoolRet;
    UNICODE_STRING ErrataInfName;
    USHORT OsFilePathOffset;
    NTSTATUS Status;

    OsFilePathOffset = OsFilePath->Length;

    //
    // Load the defective driver database.
    //

    OsFilePath->Buffer[OsFilePathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = OsFilePathOffset;
    BoolRet = BlAppendUnicodeToString(OsFilePath,
                                      DRVMAIN_PATH L"\\" DRVMAIN_NAME);

    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OslpLoadMiscModulesEnd;
    }

    Status = BlImgLoadImageEx(OsDeviceId,
                              OSL_MEMORY_TYPE_MISC_MODULE,
                              OsFilePath->Buffer,
                              &LoaderBlock->Extension->DrvDBImage,
                              &LoaderBlock->Extension->DrvDBSize,
                              MEMORY_ATTRIBUTE_ALLOCATION_KSEG,
                              0,
                              IMAGE_FLAGS_NONE);

    if (Status == STATUS_NO_SUCH_FILE) {

        //
        // This is the only "expected" error while loading defective driver
        // database. All other errors are fatal.
        //

        Status = STATUS_SUCCESS;
    }

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadMiscModulesEnd;
    }

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadMiscModulesEnd;
    }

    //
    // Load the errata manger database file.
    //

    RtlInitUnicodeString(&ErrataInfName, NULL);
    BoolRet = OslGetErrataFileNameFromRegistry(SystemHiveHandle,
                                               &ErrataInfName);

    if (BoolRet != FALSE) {
        OsFilePath->Buffer[OsFilePathOffset / sizeof(WCHAR)] = L'\0';
        OsFilePath->Length = OsFilePathOffset;
        BoolRet = BlAppendUnicodeToString(OsFilePath, INF_PATH L"\\");
        if (BoolRet == FALSE) {
            Status = STATUS_NO_MEMORY;
            goto OslpLoadMiscModulesEnd;
        }

        BoolRet = BlAppendUnicodeToString(OsFilePath, ErrataInfName.Buffer);
        if (BoolRet == FALSE) {
            Status = STATUS_NO_MEMORY;
            goto OslpLoadMiscModulesEnd;
        }

        Status = BlImgLoadImageEx(OsDeviceId,
                                  OSL_MEMORY_TYPE_MISC_MODULE,
                                  OsFilePath->Buffer,
                                  &LoaderBlock->Extension->EmInfFileImage,
                                  &LoaderBlock->Extension->EmInfFileSize,
                                  MEMORY_ATTRIBUTE_ALLOCATION_KSEG,
                                  0,
                                  IMAGE_FLAGS_NONE);

        if (Status == STATUS_NO_SUCH_FILE) {

            //
            // This is the only "expected" error.
            //

            Status = STATUS_SUCCESS;
        }

        if (!NT_SUCCESS(Status)) {
            goto OslpLoadMiscModulesEnd;
        }
    }

OslpLoadMiscModulesEnd:

    OsFilePath->Length = OsFilePathOffset;
    return Status;
}

NTSTATUS
OslpUnloadFirmwareDrivers(
    __inout PLIST_ENTRY LoadedFirmwareDriverList
    )

/*++

Routine Description:

    This routine will unload all listed firmware drivers.

Arguments:

    LoadedFirmwareDriverList - Supplies a list of loaded firmware drivers.

Return Value:

    STATUS_SUCCESS if successful.

--*/

{

    PFIRMWARE_LOADED_DRIVER_ENTRY LoadedFirmwareDriverEntry;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    ReturnStatus = STATUS_SUCCESS;

    while (IsListEmpty(LoadedFirmwareDriverList) == FALSE) {
        LoadedFirmwareDriverEntry =
            CONTAINING_RECORD(LoadedFirmwareDriverList->Flink,
                              FIRMWARE_LOADED_DRIVER_ENTRY,
                              ListEntry);

        RemoveEntryList(&LoadedFirmwareDriverEntry->ListEntry);
        Status = OslFwUnloadDriver(LoadedFirmwareDriverEntry->DriverHandle);
        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }

        BlMmFreeHeap(LoadedFirmwareDriverEntry);
    }

    return ReturnStatus;
}

NTSTATUS
OslpDetermineKernelHal (
    __out PUNICODE_STRING KernelName,
    __out PUNICODE_STRING HalName
    )

/*++

Routine Description:

    Determines the kernel and hal to load.

    This routine will first call into an architecture specific detection
    routine to determine the appropriate HAL and KERNEL to load.

    If the loader contains the option, /KERNEL, this routine will return a
    string initialized to the specific kernel name.

    If the loader contains the option, /HAL, this routine will return a
    string initialized to the specified hal name.

    In all cases, memory will be allocated to hold the string.  The caller
    is responsible for freeing the memory.

Arguments:

    KernelName - The name of the kernel to load.

    HalName - The name of the hal to load.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY when a memory allocation fails.

--*/

{

    BOOLEAN BoolRet;
    PWCHAR OverridePath;
    NTSTATUS Status;

    //
    // Call architecture-specific routines to determine which kernel and HAL
    // to load.
    //

    Status = OslArchDetectKernelHal(KernelName, HalName);
    if (NT_SUCCESS(Status) == FALSE) {
        return Status;
    }


    //
    // If a kernel override was specified, use the user's kernel over the
    // default kernel.
    //

    Status = BlGetApplicationOptionString(BCDE_OSLOADER_TYPE_KERNEL_PATH,
                                          &OverridePath);

    if (NT_SUCCESS(Status)) {
        RtlFreeUnicodeString(KernelName);
        RtlInitUnicodeString(KernelName, NULL);
        BoolRet = BlAppendUnicodeToString(KernelName, OverridePath);
        BlMmFreeHeap(OverridePath);
        if (BoolRet == FALSE) {
            return STATUS_NO_MEMORY;
        }
    }

    //
    // If /HAL was specified, use the user's hal over the default hal.
    //

    Status = BlGetApplicationOptionString(BCDE_OSLOADER_TYPE_HAL_PATH,
                                          &OverridePath);

    if (NT_SUCCESS(Status)) {
        RtlFreeUnicodeString(HalName);
        RtlInitUnicodeString(HalName, NULL);
        BoolRet = BlAppendUnicodeToString(HalName, OverridePath);
        BlMmFreeHeap(OverridePath);
        if (BoolRet == FALSE) {
            return STATUS_NO_MEMORY;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
OslpProcessEVStore (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in PBOOT_ENVIRONMENT_DEVICE BootDevice
    )

/*++

Routine Description:

    This routine determines whether a hive needs to be loaded for EV.

    If the loader contains the option, /EVSTORE, this routine will load
    the hive the specified hive and attach it to the loader block. The kernel
    will then mount the hive at the EVSTORE root during phase1 init.

Arguments:

    LoaderBlock - Supplies the loader parameter block.

    BootDevice - Supplies a device on which the EV hive resides.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY when a memory allocation fails.

    STATUS_NOT_FOUND if the specified hive does not exist.

    STATUS_UNSUCCESSFUL if a consistency check failed on the hive.

    STATUS_REGISTRY_CORRUPT if HvInitializeHive fails due to hive corruption.

--*/

{

    DEVICE_ID DeviceId;
    NTSTATUS Status;
    PWCHAR StorePath;

    //
    // If the EV store boot option is present, load the hive and attach it to
    // the loader block.
    //

    Status = BlGetApplicationOptionString(BCDE_OSLOADER_TYPE_EVSTORE,
                                          &StorePath);

    if (NT_SUCCESS(Status)) {
        Status = BlDeviceOpen(BootDevice, OPEN_READ, &DeviceId);
        if (!NT_SUCCESS(Status)) {
            goto ProcessEVStoreEnd;
        }

        Status = OslAttachHiveToLoaderBlock(DeviceId,
                                            NULL,
                                            LoaderBlock,
                                            LHB_SYSTEM_PARTITION,
                                            StorePath,
                                            L"MACHINE",
                                            EVSTORE_MOUNT_ROOT,
                                            &OslMachineHiveHandle);

ProcessEVStoreEnd:
        if (!NT_SUCCESS(Status)) {
            OslFatalError(OSL_ERROR_REGISTRY,
                          (ULONG_PTR)StorePath,
                          Status,
                          0);
        }

        BlMmFreeHeap(StorePath);
        return Status;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
OslpLoadNlsData (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID DeviceId,
    __in ULONG HiveId
    )

/*++

Routine Description:

    This routine will load the ansi code page, oem code page, and unicode case
    table into memory.

Arguments:

    LoaderBlock - Pointer to the loader parameter block.

    OsFilePath - The path of the Operating System.

    DeviceId - Handle for the device containing the operating system.

    HiveId - Handle for the system hive.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if a memory allocation fails.
    STATUS_NO_SUCH_FILE if one of the font files does not exist.
    STATUS_INVALID_IMAGE_FORMAT if the hal font file has an invalid format.

--*/

{

    ULONG AlignedImageSize;
    UNICODE_STRING AnsiFilename;
    ULONG AnsiFileSize;
    BOOLEAN BoolRet;
    PVOID Buffer;
    UNICODE_STRING CaseTableFilename;
    ULONG CaseTableFileSize;
    FILE_ID FileId;
    FILE_INFORMATION FileInfo;
    ULONG ImageSize;
    PNLS_DATA_BLOCK NlsData;
    UNICODE_STRING OemFilename;
    ULONG OemFileSize;
    BOOLEAN OemSameAsAnsi;
    USHORT OsFilePathOffset;
    NTSTATUS Status;
    USHORT SystemPathOffset;

    OemSameAsAnsi = FALSE;
    OsFilePathOffset = OsFilePath->Length;

    //
    // Allocate Nls Data buffer.
    //

    NlsData = BlMmAllocateHeap(sizeof(NLS_DATA_BLOCK));
    if (NlsData == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OslpLoadNlsDataEnd;
    }

    RtlZeroMemory(NlsData, sizeof(NLS_DATA_BLOCK));
    LoaderBlock->NlsData = NlsData;

    //
    // The Ansi code page, oem code page and the unicode table are all in
    // the sytem32 subdirectory.
    //

    BoolRet = BlAppendUnicodeToString(OsFilePath, L"system32\\");
    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OslpLoadNlsDataEnd;
    }

    SystemPathOffset = OsFilePath->Length;

    //
    // Determine the font files required for the Operating System.
    //

    Status = OslHiveFindNlsData(HiveId,
                                &AnsiFilename,
                                &OemFilename,
                                &CaseTableFilename);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadNlsDataEnd;
    }

    //
    // In the Japanese version of the Operating System, the ANSI codepage
    // and the OEM codepage are the same.  In this case, only load the file
    // once.  Currently, kernel init code will map NLS tables using a single
    // system cache view.  This limits the tables to 256KB in size.  Only
    // loading one table will save space and avoid later problems.
    //

    if ((AnsiFilename.Length == OemFilename.Length) &&
        (_wcsnicmp(AnsiFilename.Buffer,
                   OemFilename.Buffer,
                   AnsiFilename.Length) == 0)) {

        OemSameAsAnsi = TRUE;
    }

    //
    // N.B. The OS assumes that the ANSI NLS table, the OEM NLS table and the
    //      case table are loaded into memory described with the NLS memory
    //      type. No other allocations can use this memory type. Additionally,
    //      the OS assumes that these tables are in a single contiguous
    //      allocation.
    //

    //
    // Determine the size of the Ansi code page.
    //

    BoolRet = BlAppendUnicodeToString(OsFilePath, AnsiFilename.Buffer);
    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OslpLoadNlsDataEnd;
    }

    Status = BlFileOpen(DeviceId, OsFilePath->Buffer, OPEN_READ, &FileId);
    if (!NT_SUCCESS(Status)) {
        goto OslpLoadNlsDataEnd;
    }

    Status = BlFileGetInformation(FileId, &FileInfo);
    BlFileClose(FileId);
    if (!NT_SUCCESS(Status)) {
        goto OslpLoadNlsDataEnd;
    }

    AnsiFileSize = (ULONG)ALIGN_RANGE_UP(FileInfo.FileSize, sizeof(UINTN));
    if (AnsiFileSize < FileInfo.FileSize) {
        Status = STATUS_INTEGER_OVERFLOW;
        goto OslpLoadNlsDataEnd;
    }

    OsFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = SystemPathOffset;

    //
    // Determine the size of the Oem code page.
    //

    if (OemSameAsAnsi == FALSE) {
        BoolRet = BlAppendUnicodeToString(OsFilePath, OemFilename.Buffer);
        if (BoolRet == FALSE) {
            Status = STATUS_NO_MEMORY;
            goto OslpLoadNlsDataEnd;
        }

        Status = BlFileOpen(DeviceId, OsFilePath->Buffer, OPEN_READ, &FileId);
        if (!NT_SUCCESS(Status)) {
            goto OslpLoadNlsDataEnd;
        }

        Status = BlFileGetInformation(FileId, &FileInfo);
        BlFileClose(FileId);
        if (!NT_SUCCESS(Status)) {
            goto OslpLoadNlsDataEnd;
        }

        OemFileSize = (ULONG)ALIGN_RANGE_UP(FileInfo.FileSize, sizeof(UINTN));
        if (OemFileSize < FileInfo.FileSize) {
            Status = STATUS_INTEGER_OVERFLOW;
            goto OslpLoadNlsDataEnd;
        }

        OsFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
        OsFilePath->Length = SystemPathOffset;

    } else {
        OemFileSize = 0;
    }

    //
    // Determine the size of the Case Table.
    //

    BoolRet = BlAppendUnicodeToString(OsFilePath, CaseTableFilename.Buffer);
    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OslpLoadNlsDataEnd;
    }

    Status = BlFileOpen(DeviceId, OsFilePath->Buffer, OPEN_READ, &FileId);
    if (!NT_SUCCESS(Status)) {
        goto OslpLoadNlsDataEnd;
    }

    Status = BlFileGetInformation(FileId, &FileInfo);
    BlFileClose(FileId);
    if (!NT_SUCCESS(Status)) {
        goto OslpLoadNlsDataEnd;
    }

    CaseTableFileSize = (ULONG)ALIGN_RANGE_UP(FileInfo.FileSize, sizeof(UINTN));
    if (CaseTableFileSize < FileInfo.FileSize) {
        Status = STATUS_INTEGER_OVERFLOW;
        goto OslpLoadNlsDataEnd;
    }

    OsFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = SystemPathOffset;

    //
    // Allocate a memory buffer for all three font tables.
    //

    Buffer = NULL;

    //
    // Safely add the file sizes. If any add overflows, quit.
    //

    ImageSize = 0;
    Status = RtlULongAdd(AnsiFileSize,
                         OemFileSize,
                         &ImageSize);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadNlsDataEnd;
    }

    Status = RtlULongAdd(ImageSize,
                         CaseTableFileSize,
                         &ImageSize);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadNlsDataEnd;
    }

    AlignedImageSize = ALIGN_RANGE_UP(ImageSize, PAGE_SIZE);
    if (AlignedImageSize < ImageSize) {
        Status = STATUS_INTEGER_OVERFLOW;
        goto OslpLoadNlsDataEnd;
    }

    ImageSize = AlignedImageSize;
    Status = BlMmAllocatePages(&Buffer,
                               ImageSize >> PAGE_SHIFT,
                               OSL_MEMORY_TYPE_NLS_DATA,
                               MEMORY_ATTRIBUTE_ALLOCATION_KSEG,
                               0);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadNlsDataEnd;
    }

    NlsData->AnsiCodePageData = Buffer;
    NlsData->UnicodeCaseTableData = Add2Ptr(Buffer, AnsiFileSize + OemFileSize);
    if (OemSameAsAnsi == FALSE) {
        NlsData->OemCodePageData = Add2Ptr(Buffer, AnsiFileSize);

    } else {
        NlsData->OemCodePageData = NlsData->AnsiCodePageData;
    }

    //
    // Load the Ansi code page into memory.
    //

    BoolRet = BlAppendUnicodeToString(OsFilePath, AnsiFilename.Buffer);
    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OslpLoadNlsDataEnd;
    }

    Status = BlImgLoadImage(DeviceId,
                            OSL_MEMORY_TYPE_NLS_DATA,
                            OsFilePath->Buffer,
                            &NlsData->AnsiCodePageData,
                            &AnsiFileSize,
                            IMAGE_FLAGS_MEMORY_PRE_ALLOCATED);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadNlsDataEnd;
    }

    OsFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = SystemPathOffset;

    //
    // Load the Oem code page into memory.
    //

    if (OemSameAsAnsi == FALSE) {
        BoolRet = BlAppendUnicodeToString(OsFilePath, OemFilename.Buffer);
        if (BoolRet == FALSE) {
            Status = STATUS_NO_MEMORY;
            goto OslpLoadNlsDataEnd;
        }

        Status = BlImgLoadImage(DeviceId,
                                OSL_MEMORY_TYPE_NLS_DATA,
                                OsFilePath->Buffer,
                                &NlsData->OemCodePageData,
                                &OemFileSize,
                                IMAGE_FLAGS_MEMORY_PRE_ALLOCATED);

        if (!NT_SUCCESS(Status)) {
            goto OslpLoadNlsDataEnd;
        }

        OsFilePath->Buffer[SystemPathOffset / sizeof(WCHAR)] = L'\0';
        OsFilePath->Length = SystemPathOffset;
    }

    //
    // Load the unicode table into memory.
    //

    BoolRet = BlAppendUnicodeToString(OsFilePath, CaseTableFilename.Buffer);
    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OslpLoadNlsDataEnd;
    }

    Status = BlImgLoadImage(DeviceId,
                            OSL_MEMORY_TYPE_NLS_DATA,
                            OsFilePath->Buffer,
                            &NlsData->UnicodeCaseTableData,
                            &CaseTableFileSize,
                            IMAGE_FLAGS_MEMORY_PRE_ALLOCATED);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadNlsDataEnd;
    }

    //
    // Cleanup.
    //

OslpLoadNlsDataEnd:
    OsFilePath->Buffer[OsFilePathOffset / sizeof(WCHAR)] = L'\0';
    OsFilePath->Length = OsFilePathOffset;
    if (!NT_SUCCESS(Status)) {
        if (NlsData != NULL) {
            if (NlsData->AnsiCodePageData != NULL) {
                BlMmFreePages(NlsData->AnsiCodePageData);
            }

            BlMmFreeHeap(NlsData);
            LoaderBlock->NlsData = NULL;
        }
    }

    return Status;
}

NTSTATUS
OslpValidateImageType (
    __in_bcount(ImageSize) PVOID ImageBase,
    __in ULONG ImageSize
    )

/*++

Routine Description:

    This routine validates that the specified image conforms to the operating
    environment. Specifically, it ensures that the current operating type
    and the image type characteristics match.

Arguments:

    ImageBase - Supplies a pointer to a buffer containing a loaded image.
    ImageSize - Supplies the size of the ImageBase buffer.

Return Value:

    NT Status code indicating outcome.

--*/

{

    USHORT ImageCharacteristics;
    PIMAGE_NT_HEADERS NtHeader;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;

    //
    // Only perform image validation on x86 images.
    //

#pragma prefast ( suppress: __WARNING_CONST_CONST_COMP, "Intentionally const" )
    if (BOOT_APPLICATION_MACHINE_TYPE != IMAGE_FILE_MACHINE_I386) {
        goto ValidateImageTypeExit;
    }

    //
    // Determine if the image is a PAE aware image.
    //

    if (ImageSize < sizeof(IMAGE_DOS_HEADER)) {
        Status = STATUS_INVALID_IMAGE_FORMAT;
        goto ValidateImageTypeExit;
    }

    Status = BlImgGetNtHeader(ImageBase, ImageSize, &NtHeader);
    if (!NT_SUCCESS(Status)) {
        goto ValidateImageTypeExit;
    }

    ImageCharacteristics = NtHeader->FileHeader.Characteristics;
    if ((ImageCharacteristics & IMAGE_FILE_LARGE_ADDRESS_AWARE) == 0) {
        Status = STATUS_INVALID_IMAGE_FORMAT;
        goto ValidateImageTypeExit;
    }

ValidateImageTypeExit:
    return Status;
}

NTSTATUS
OslpLogSaveInformation (
    )

/*++

Routine Description:

    This routine makes a call to destroy the logging framework, which would end
    up persisting any current logs, before destroying the logging. It also
    creates a header for the log which is also persisted.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS is logging was ended successfully.

--*/

{

    BL_LOG_HEADER LogHeader;
    GUID LogHeaderGuid = BL_LOG_HEADER_GUID;
    GENERIC_BUFFER PersistBuffer;
    NTSTATUS Status;

    //
    // End Logging. This would persist any current logs.
    //

    BlLogDestroy();

    //
    // Set the entry header information
    //

    LogHeader.Header.Timestamp = BlTimeGetRelativeTime();
    LogHeader.Header.LogType = BlLogTypeHeader;
    LogHeader.Header.DataSize = sizeof(LogHeader);

    //
    // Set the information pertaining to the logging that was done. For right
    // now, set the log size to be the size of the header, which should be
    // updated once the logs are merged in the kernel.
    //

    LogHeader.ScenarioType = BL_LOG_SCENARIO_BOOT;
    LogHeader.Version = BL_LOG_VERSION;
    LogHeader.Boot.Spare = 0;
    LogHeader.TraceSize = sizeof(LogHeader);

    //
    // Persist the log header.
    //

    BlInitGenericBuffer(&PersistBuffer, &LogHeader, sizeof(LogHeader));
    Status = BlPdSaveData(NULL, &LogHeaderGuid, &PersistBuffer);
    if (!NT_SUCCESS(Status)) {
        goto cleanup;
    }

    Status = STATUS_SUCCESS;

cleanup:

    return Status;
}

VOID
OslpInitializeBootStatusDataLog (
    VOID
    )

/*++

Routine Description:

    This routine will initialize boot status data (BSD) logging.  No result
    is returned, since no action can be taken if this fails.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PBOOT_ENVIRONMENT_DEVICE LogDevice;
    PBOOT_ENVIRONMENT_DEVICE LogDeviceOverride;
    ULONG InitializationFlags;
    PWSTR LogPath;
    ULONG LogPathLength;
    BOOLEAN PreserveData;
    NTSTATUS Status;

    LogDeviceOverride = NULL;
    LogPath = NULL;

    //
    // Obtain any override values for the logging device.
    //

    Status = BlGetApplicationOptionDevice(BCDE_LIBRARY_TYPE_BSD_LOG_DEVICE,
                                          &LogDeviceOverride,
                                          NULL);

    //
    // If is an override present, use that, otherwise use the same device as the
    // application device.
    //

    if (!NT_SUCCESS(Status)) {
        LogDevice = OslLoadDevice;
    } else {
        LogDevice = LogDeviceOverride;
    }

    //
    // Obtain any override values for the logging path.
    //

    Status = BlGetApplicationOptionString(BCDE_LIBRARY_TYPE_BSD_LOG_PATH,
                                          &LogPath);

    //
    // If is an override present, use that, otherwise use the same path as the
    // SystemRoot + bootstat.dat.
    //

    if (!NT_SUCCESS(Status)) {
        LogPathLength = (ULONG)
            (wcslen(OslSystemRoot) + wcslen(BSD_LOG_DATA_FILENAME));

        LogPath = BlMmAllocateHeap((LogPathLength + 1) * sizeof(WCHAR));
        if (LogPath == NULL) {
            Status = STATUS_NO_MEMORY;
            goto OslpInitializeBootStatusDataLogEnd;
        }

        //
        // OslSystemRoot already contains a trailing backslash.
        //

        wcscpy_s(LogPath, LogPathLength + 1, OslSystemRoot);
        wcscat_s(LogPath, LogPathLength + 1, BSD_LOG_DATA_FILENAME);
    }

    //
    // Determine whether the flag is set to preserve log data at initialization.
    //

    Status = BlGetApplicationOptionBoolean(BCDE_LIBRARY_TYPE_BSD_PRESERVE_LOG,
                                           &PreserveData);

    //
    // If is an override present, use that, otherwise use the same path as the
    // boot manager binary.
    //

    if (NT_SUCCESS(Status) && (PreserveData != FALSE)) {
        InitializationFlags = BSD_INIT_FLAGS_PRESERVE_LOG;
    } else {
        InitializationFlags = 0;
    }

    //
    // Initialize the logging engine.  For the boot manager, the offset to the
    // start of the log data section of the file is zero.
    //

    BlBsdInitializeLog(BCD_APPLICATION_TYPE_WINDOWS_BOOT_LOADER,
                       LogDevice,
                       LogPath,
                       BSD_LOG_DATA_OFFSET,
                       InitializationFlags);

OslpInitializeBootStatusDataLogEnd:
    if (LogDeviceOverride != NULL) {
        BlMmFreeHeap(LogDeviceOverride);
    }

    if (LogPath != NULL) {
        BlMmFreeHeap(LogPath);
    }
}


NTSTATUS
OslpCheckForcedFailure (
    VOID
    )

/*++

Routine Description:

    This routine provides a means for users to force a failure in the OSLOADER.
    This is for test purposes only.  Note that the failure information is hard-
    coded, and so may have no bearing on the actual OS installation?

Arguments:

    None.

Return Value:

    A NTSTATUS value indicating the status code of the failure that was forced.

--*/

{

    ULONGLONG Force;
    NTSTATUS Status;

    Status = BlGetApplicationOptionInteger(BCDE_OSLOADER_TYPE_FORCE_FAILURE,
                                           &Force);

    //
    // If the forced failure option doesn't exist, or is not valid, then this
    // function must succeed.
    //

    if (!NT_SUCCESS(Status) || (Force >= ForceFailureCount)) {
        return STATUS_SUCCESS;
    }

    Status = STATUS_SUCCESS;
    switch (Force) {
    case ForceFailureLoad:
        Status = STATUS_NO_SUCH_FILE;
        OslFatalError(OSL_ERROR_KERNEL,
                      (ULONG_PTR)L"\\windows\\system32\\ntkrnlmp.exe",
                      (ULONG_PTR)STATUS_NO_SUCH_FILE,
                      0);

        break;

    case ForceFailureHive:
        Status = STATUS_NO_SUCH_FILE;
        OslFatalError(OSL_ERROR_REGISTRY,
                      (ULONG_PTR)L"\\windows\\system32\\config\\system",
                      (ULONG_PTR)STATUS_NO_SUCH_FILE,
                      0);

        break;

    case ForceFailureAcpi:
        Status = STATUS_ACPI_FATAL;
        OslFatalError(OSL_ERROR_NO_ACPI_DETECTED, 0, 0, (ULONG_PTR)Status);
        break;

    case ForceFailureGeneral:

        //
        // The general failure is logged when an unknown OS loader error code is
        // passed to OslFatalError.
        //

        Status = STATUS_UNSUCCESSFUL;
        OslFatalError(MAXULONG, 0, 0, 0);
        break;
    };

    return Status;
}

VOID
OslpSanitizeLoadOptionsString (
    __in PWSTR LoadOptionsString,
    __in PWSTR SanitizeString
    )

/*++

Routine Description:

    This routine removes the string to sanitize from the load options string.

Arguments:

    LoadOptionsString - Supplies a string of load options.

    SanitizeString - Supplies a malicious string to remove from the load
        options.

Return Value:

    None.

--*/

{

    UINTN Index;
    UINTN Length;
    PWCHAR SubString;

    SubString = LoadOptionsString;
    do {
        SubString = wcsstr(SubString, SanitizeString);
        if (SubString != NULL) {

            //
            // The string may be present either with or without a leading
            // slash, and the slash should be removed if present.
            //

            Length = wcslen(SanitizeString);
            if (PtrOffset(LoadOptionsString, SubString) != 0) {
                if (SubString[-1] == L'/') {
                    SubString -= 1;
                    Length += 1;
                }
            }

            //
            // Replace the load options substring with spaces, deleting it.
            //

            for (Index = 0; Index < Length; Index += 1) {
                SubString[Index] = L' ';
            }

            SubString += Length;
        }

    } while (SubString != NULL);

    return;
}

NTSTATUS
OlspCheckImcApplicability(
    VOID
    )

/*++

Routine Description:

    This routine determines whether IMC processing can occur because the
    IMC hive has been loaded and the Sequence numbers are present and
    mismatched.  It will return STATUS_SUCCESS in this case and
    STATUS_UNSUCCESSFUL for all other cases.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS - IMC processing is possible and applicable.

    STATUS_UNSUCCESSFUL - IMC processing is either impossible or
        not applicable.

--*/

{
    ULONG DataLength;
    ULONG ImcHiveSequence;
    PCM_KEY_NODE RootCell;
    NTSTATUS Status;
    PCM_KEY_NODE SystemControlKeyCell;
    PCM_KEY_NODE SystemControlSetKeyCell;
    ULONG SystemHiveSequence;
    PCM_KEY_NODE SystemImcControlKeyCell;
    ULONG Type;

    Status = STATUS_UNSUCCESSFUL;

    if (OslImcHiveHandle == (ULONG)-1) {
        goto OlspCheckImcApplicabilityEnd;
    }

    //
    // Get the Sequence value under the root of IMC.
    //

    DataLength = sizeof(ULONG);
    RootCell = OslGetRootCell(OslImcHiveHandle);
    Status = OslGetValue(OslImcHiveHandle,
                         RootCell,
                         IMC_SEQUENCE_VALUE_NAME,
                         &Type,
                         &DataLength,
                         (PBYTE)(&ImcHiveSequence));

    if ((!NT_SUCCESS(Status)) ||
        (Type != REG_DWORD) ||
        (DataLength != sizeof(ULONG))) {

        Status = STATUS_UNSUCCESSFUL;
        goto OlspCheckImcApplicabilityEnd;
    }

    //
    // Get the CurrentControlSet key from the system hive.
    // Passing NULL for HiveCellParent causes the CCS
    // to be returned if the hive is a System hive.
    //

    Status = OslGetSubkey(OslSystemHiveHandle,
                          NULL,
                          NULL,
                          &SystemControlSetKeyCell);

    if (!NT_SUCCESS(Status)) {
        goto OlspCheckImcApplicabilityEnd;
    }

    //
    // Get the InitialMachineConfig Control key, this may
    // not be present which is OK.
    //

    Status = OslGetSubkey(OslSystemHiveHandle,
                          SystemControlSetKeyCell,
                          IMC_SYSTEM_CONTROL_KEY_NAME,
                          &SystemControlKeyCell);

    if (!NT_SUCCESS(Status)) {
        goto OlspCheckImcApplicabilityEnd;
    }

    Status = OslGetSubkey(OslSystemHiveHandle,
                          SystemControlKeyCell,
                          IMC_IMC_KEY_NAME,
                          &SystemImcControlKeyCell);

    if (!NT_SUCCESS(Status)) {
        goto OlspCheckImcApplicabilityEnd;
    }

    //
    // Find the Sequence number that applies to the System
    // Hive's control value.
    //

    DataLength = sizeof(ULONG);
    Status = OslGetValue(OslSystemHiveHandle,
                         SystemImcControlKeyCell,
                         IMC_SEQUENCE_VALUE_NAME,
                         &Type,
                         &DataLength,
                         (PBYTE)(&SystemHiveSequence));

    if ((!NT_SUCCESS(Status)) ||
        (Type != REG_DWORD) ||
        (DataLength != sizeof(ULONG))) {

        Status = STATUS_UNSUCCESSFUL;
        goto OlspCheckImcApplicabilityEnd;
    }

    //
    // If the values match then do not perform IMC processing,
    // we are at the correct level.
    //

    if (SystemHiveSequence == ImcHiveSequence) {
        Status = STATUS_UNSUCCESSFUL;
    }

OlspCheckImcApplicabilityEnd:

    if (!NT_SUCCESS(Status)) {
        Status = STATUS_UNSUCCESSFUL;
    }

    return Status;
}


NTSTATUS
OslGetImcSystemEntropyKeyValue(
    __in PCWSTR ValueName,
    __out_ecount(BufferLength) PBYTE Buffer,
    __in ULONG BufferLength,
    __out PULONG DataLength
    )

/*++

Routine Description:

    This routine will locate an RNG entropy seed key under the IMC
    have if IMC processing is being performed.

Arguments:

    ValueName - The name of the entropy value which is assumed to be
        under the System\RNG key.

    Buffer - A caller-allocated buffer that will contain the entropy
        value on output, if it exists.

    BufferLength - The length of the buffer supplied to receive the
        entropy value if it exists.

    DataLength - A pointer to a ULONG which recieves the length of
        the entropy value read.  Valid if the function returns
        STATUS_SUCCESS.

Return Value:

    STATUS_SUCCESS - An RNG key cell was located and returned.

    STATUS_UNSUCCESSFUL - There is not RNG key cell in the IMC hive
        or the IMC hive does not exist.
--*/

{
    PCM_KEY_NODE ImcSystemKeyCell;
    PCM_KEY_NODE RngKeyCell;
    PCM_KEY_NODE RootCell;
    NTSTATUS Status;
    ULONG Type;

    Status = STATUS_UNSUCCESSFUL;
    *DataLength = BufferLength;

    //
    // If IMC never loaded a hive or the sequence numbers
    // didn't call for processing then bail.
    //

    if ((OslImcProcessingValid == FALSE) ||
       (OslImcHiveHandle == (ULONG)-1)) {
        goto OslGetImcSystemEntropyKeyValueEnd;
    }

    RootCell = OslGetRootCell(OslImcHiveHandle);
    Status = OslGetSubkey(OslImcHiveHandle,
                          RootCell,
                          IMC_SYSTEM_KEY_NAME,
                          &ImcSystemKeyCell);

    if (!NT_SUCCESS(Status)) {
        goto OslGetImcSystemEntropyKeyValueEnd;
    }

    Status = OslGetSubkey(OslImcHiveHandle,
                          ImcSystemKeyCell,
                          IMC_RNG_KEY_NAME,
                          &RngKeyCell);

    if (!NT_SUCCESS(Status)) {
        goto OslGetImcSystemEntropyKeyValueEnd;
    }

    Status = OslGetValue(OslImcHiveHandle,
                         RngKeyCell,
                         (PWSTR)ValueName,
                         &Type,
                         DataLength,
                         Buffer);

    if ((!NT_SUCCESS(Status)) || (*DataLength != BufferLength)) {
        Status = STATUS_UNSUCCESSFUL;
        goto OslGetImcSystemEntropyKeyValueEnd;
    }


OslGetImcSystemEntropyKeyValueEnd:

    if (!NT_SUCCESS(Status)) {
        Status = STATUS_UNSUCCESSFUL;
    }

    return Status;
}

NTSTATUS
OslpProcessInitialMachineConfiguration (
    __in PUNICODE_STRING OsFilePath,
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine will attempt to retreive boot options that describe an
    Initial Machine Configuration hive.  If valid options are located
    then the IMC hive is loaded and attached to the loader block.

    If an error occurs in processing IMC then the error is also passed
    to the kernel through the loader block.  The kernel will triage
    errors and likely bugcheck.

Arguments:

    OsFilePath - The system root path for the OS being booted.

    LoaderBlock - The loader block to which the IMC hive will be
        attached.

Return Value:

    NTSTATUS - Return status of the operation.

--*/

{

    ULONG CodePoint;
    PBOOT_ENVIRONMENT_DEVICE ImcDevice;
    DEVICE_ID ImcDeviceId;
    PBOOT_ENVIRONMENT_DEVICE ImcDeviceUse;
    BOOT_ENVIRONMENT_DEVICE SyntheticImcDevice;
    PWCHAR ImcHivePath;
    NTSTATUS Status;
    BOOLEAN UsingSyntheticImcDevice;

    CodePoint = 0;
    ImcDevice = NULL;
    ImcDeviceId = INVALID_DEVICE_ID;
    ImcDeviceUse = NULL;
    UsingSyntheticImcDevice = FALSE;

    //
    // It is expected and normal not to have a hive name, it simply means
    // no IMC processing should be done so exit successfully.
    //

    Status = BlGetApplicationOptionString(BCDE_OSLOADER_TYPE_IMC_HIVE_NAME, &ImcHivePath);
    if (!NT_SUCCESS(Status)) {
        ImcHivePath = NULL;
    }

    Status = STATUS_SUCCESS;
    if (ImcHivePath != NULL)  {

        //
        // Determine the root device of the IMC path.  This may
        // be either bootdevice or osdevice.  In physical boot
        // cases it doesn't matter and the default is osdevice.
        //

        Status = BlGetApplicationOptionDevice(BCDE_OSLOADER_TYPE_IMC_DEVICE,
                                              &ImcDevice,
                                              NULL);

        //
        // STATUS_NOT_FOUND is returned when the boot option is not found.
        // In that case it is not an error because the hive name is present,
        // default to the boot device.
        //

        if (Status == STATUS_NOT_FOUND) {
            ImcDeviceUse = BlGetBootDevice();
        } else {
            ImcDeviceUse = ImcDevice;
        }

        if (ImcDeviceUse == NULL) {
            Status = STATUS_NO_SUCH_DEVICE;
            CodePoint = 1;
            goto OslProcessInitialMachineConfigurationEnd;
        }

    } else if (HviIsHypervisorMicrosoftCompatible() &&
             !HviIsCpuManagementPartition()) {

        //
        // Try to find the Hyper-V IMC device.
        //

        RtlZeroMemory(&SyntheticImcDevice, sizeof(BOOT_ENVIRONMENT_DEVICE));
        SyntheticImcDevice.u.Vmbus = OslVmbusIdentifier;
        SyntheticImcDevice.DeviceType = DEVICE_VMBUS;
        SyntheticImcDevice.Size =
            FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u.Vmbus) +
            sizeof(VMBUS_IDENTIFIER),

        ImcDeviceUse = &SyntheticImcDevice;
        ImcHivePath = IMC_SYNTHETIC_PATH;
        UsingSyntheticImcDevice = TRUE;

    } else {
        goto OslProcessInitialMachineConfigurationEnd;
    }

    Status = BlDeviceOpen(ImcDeviceUse,
                          OPEN_READ,
                          &ImcDeviceId);

    if (!NT_SUCCESS(Status)) {
        CodePoint = 2;
        goto OslProcessInitialMachineConfigurationEnd;
    }

    //
    // Load the hive and attach it to the loader block.  Note
    // that we use LHB_MOUNT_VOLATILE so the file name need
    // only be accessible in the boot environment via
    // the boot environment device.
    //

    CodePoint = 3;
    Status = OslAttachHiveToLoaderBlock(ImcDeviceId,
                                        OsFilePath,
                                        LoaderBlock,
                                        LHB_MOUNT_VOLATILE,
                                        ImcHivePath,
                                        L"MACHINE",
                                        IMC_MOUNT_ROOT,
                                        &OslImcHiveHandle);

    //
    // Close device, ignore errors which should never occur
    // since the device opened here.
    //

    if (ImcDeviceId != INVALID_DEVICE_ID) {
        BlDeviceClose(ImcDeviceId);
    }

    if (NT_SUCCESS(OlspCheckImcApplicability())) {
        OslImcProcessingValid = TRUE;
    }

OslProcessInitialMachineConfigurationEnd:

    if (!UsingSyntheticImcDevice) {
        if (ImcHivePath != NULL) {
            BlMmFreeHeap(ImcHivePath);
        }

        if (ImcDeviceUse != NULL &&
            ImcDeviceUse != BlGetBootDevice()) {

            BlMmFreeHeap(ImcDeviceUse);
        }
    }

    if (!NT_SUCCESS(Status)) {
        if (UsingSyntheticImcDevice &&
            (Status == STATUS_NO_SUCH_DEVICE ||
            Status == STATUS_NO_SUCH_FILE ||
            Status == STATUS_NOT_IMPLEMENTED ||
            Status == STATUS_NOT_SUPPORTED)) {

            return STATUS_SUCCESS;
        }

        OslFatalError(OSL_ERROR_REGISTRY,
                      (ULONG_PTR)ImcHivePath,
                      Status,
                      0);

        //
        // In the case of an error at this point push the error
        // into the loader extension block so it can be handled
        // in the kernel.
        //
        // N.B. This will not be done in case of the soft reboot.
        //      A failure to load the IMC hive should fail the 'prepare' step
        //      rather than cause an unexpected bugcheck of the target OS.
        //

#if !defined(NTKERNEL)

        LoaderBlock->Extension->BugcheckParameters.BugcheckCode = CONFIG_INITIALIZATION_FAILED;
        LoaderBlock->Extension->BugcheckParameters.BugcheckParameter1 = INIT_SYSTEM_LOADER;
        LoaderBlock->Extension->BugcheckParameters.BugcheckParameter2 = CodePoint;
        LoaderBlock->Extension->BugcheckParameters.BugcheckParameter3 = Status;
        LoaderBlock->Extension->BugcheckParameters.BugcheckParameter4 = 0;
        Status = STATUS_SUCCESS;

#endif

    }

    return Status;
}

VOID
OslpSanitizeStringOptions (
    __in PBOOT_ENTRY_OPTION OptionList
    )

/*++

Routine Description:

    This routine sanitizes all the strings in the given option list. It will
    mark options that have malicious substrings as invalid. These substrings
    correspond to application options that are only allowed to be set by a
    physically present user.

Arguments:

    OptionList - Supplies a pointer to the option list to be sanitized.

Return Value:

    None.

--*/

{

    PBOOT_ENTRY_OPTION AssociatedOptions;
    PBOOT_ENTRY_OPTION CurrentOption;
    ULONG NextOffset;
    BOOLEAN SecureBootPolicy;
    NTSTATUS Status;
    PBCDE_STRING StringOption;

    //
    // Is there a Secure Boot policy active?
    //

    SecureBootPolicy = BlSecureBootIsPolicyActive();

    NextOffset = 0;
    do {
        CurrentOption = Add2Ptr(OptionList, NextOffset);

        //
        // Sanitize associated options for all element types.
        //

        if (CurrentOption->AssociatedOptionsOffset != 0) {
            NextOffset += CurrentOption->AssociatedOptionsOffset;
            AssociatedOptions = Add2Ptr(OptionList, NextOffset);
            OslpSanitizeStringOptions(AssociatedOptions);
        }

        //
        // Only sanitize valid string options.
        //

        if ((GET_BCDE_DATA_FORMAT(CurrentOption->Type) == BCDE_FORMAT_STRING) &&
            (CurrentOption->Invalid == FALSE)) {

            StringOption = (PBCDE_STRING)Add2Ptr(CurrentOption,
                                                 CurrentOption->DataOffset);

            //
            // If a string contains "DISABLE_INTEGRITY_CHECKS",
            // "DISABLEELAMDRIVERS", or "NOINTEGRITYCHECKS", mark the option as
            // invalid.
            // Addition values are checked for if there is a Secure Boot
            // policy in use.
            //

            Status = OslCheckForInvalidBootOptions(StringOption->String,
                                                   SecureBootPolicy);

            if (!NT_SUCCESS(Status)) {
                CurrentOption->Invalid = TRUE;
            }
        }

        NextOffset = CurrentOption->NextOptionOffset;
    } while (NextOffset != 0);

    return;
}

VOID
OslpSetSystemBootFlags (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __out PBCDE_BOOT_MENU_POLICY FinalBootMenuPolicy
    )

/*++

Routine Description:

    This routine sets the boot flags based on keyboard input and BCD settings.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

Return Value:

    None.

--*/

{

    ULONGLONG BootMenuPolicy;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[1];

#ifdef DISABLE_INTERNAL_POLICIES

    //
    // Default to no flags.
    //

    LoaderBlock->Extension->BootFlags = SYSTEM_BOOT_FLAGS_NONE;

    //
    // Use the legacy boot menu.
    //

    BootMenuPolicy = MenuPolicyLegacy;

#else
    BOOLEAN OsSelectionMenu;
    NTSTATUS Status;

    //
    // Default to no flags.
    //

    LoaderBlock->Extension->BootFlags = SYSTEM_BOOT_FLAGS_NONE;

    //
    // Test the boot menu policy for this OS. If no policy is set, or it has
    // legacy policy, then don't communicate boot flags to the OS.
    //

    Status = BlGetApplicationOptionInteger(BCDE_OSLOADER_TYPE_BOOT_MENU_POLICY,
                                           &BootMenuPolicy);

    if (!NT_SUCCESS(Status)) {
        BootMenuPolicy = MenuPolicyLegacy;
    }

    //
    // Exit now if the menu policy is legacy.
    //

    if (BootMenuPolicy == MenuPolicyLegacy) {
        goto SetSystemBootFlagsEnd;
    }

    //
    // During any Safe Mode boot, do not pass up the standard policy flags.
    //

    if (BlApplicationOptionExists(BCDE_OSLOADER_TYPE_SAFEBOOT) != FALSE) {
        goto SetSystemBootFlagsEnd;
    }

    //
    // Check for a multi-OS menu request from the boot manager, sending it on
    // to the OS.
    //

    Status = BlGetApplicationOptionBoolean(BCDE_LIBRARY_TYPE_MULTI_BOOT_SYSTEM,
                                           &OsSelectionMenu);

    if (NT_SUCCESS(Status) && (OsSelectionMenu != FALSE)) {
        LoaderBlock->Extension->BootFlags |=
            SYSTEM_BOOT_FLAGS_MENU_OS_SELECTION;
    }

SetSystemBootFlagsEnd:

#endif

    //
    // Log the boot menu policy to ETW for SQM to pick-up.
    //

    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)&BootMenuPolicy,
                        sizeof(ULONGLONG));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_MENU_POLICY,
                  NULL,
                  1,
                  EventDataDescriptor);

    *FinalBootMenuPolicy = (BCDE_BOOT_MENU_POLICY)BootMenuPolicy;

    return;

}

NTSTATUS
OslpRemoveInternalApplicationOptions (
    VOID
    )

/*++

Routine Description:

    This routine removes application options that are only allowed to be set
    internally during the execution of this application.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS if the removal succeeds.

    An error status code if the munged application options could not be set
    properly.

--*/

{

    PBOOT_APPLICATION_ENTRY ApplicationEntry;
    PBOOT_ENTRY_OPTION ApplicationOptions;
    PWSTR LoadString;
    NTSTATUS Status;

    LoadString = NULL;

    //
    // Remove the disable integrity check and the disable ELAM drivers
    // options.  This option may only be set by a physically present user
    // selecting it through the advanced options menu.
    //

    BlRemoveApplicationOption(BCDE_LIBRARY_TYPE_DISABLE_INTEGRITY_CHECKS);
    BlRemoveApplicationOption(BCDE_OSLOADER_TYPE_DISABLE_ELAM_DRIVERS);

    //
    // The integrity checks and disable elam options are consumed by the kernel
    // as well as this application. If the corresponding kernel option string
    // is present in the load options string option, remove it as well.
    //

    Status = BlGetApplicationOptionString(BCDE_LIBRARY_TYPE_LOAD_OPTIONS_STRING,
                                          &LoadString);

    if (NT_SUCCESS(Status)) {
        _wcsupr(LoadString);
        OslpSanitizeLoadOptionsString(LoadString, L"DISABLE_INTEGRITY_CHECKS");
        OslpSanitizeLoadOptionsString(LoadString, L"NOINTEGRITYCHECKS");
        OslpSanitizeLoadOptionsString(LoadString, L"DISABLEELAMDRIVERS");

        //
        // The modification of the string has been done on a copy of the option.
        // Remove the original and replace with the copy.
        //

        BlRemoveApplicationOption(BCDE_LIBRARY_TYPE_LOAD_OPTIONS_STRING);

        Status = BlAppendApplicationOptionString(
                        BCDE_LIBRARY_TYPE_LOAD_OPTIONS_STRING,
                        LoadString);

        //
        // Free the copy.  Another copy was made by the library while appending
        // the string to the application option list.
        //

        BlMmFreeHeap(LoadString);

    } else {
        Status = STATUS_SUCCESS;
    }

    //
    // Sanitize each of the BCD string options. This is necessary because a
    // malicious user could have set a file path or debugger setting to be
    // "/DISABLE_INTEGRITY_CHECKS", "/NOINTEGRITYCHECKS" or
    // "/DISABLEELAMDRIVERS".
    //

    ApplicationEntry = BlGetApplicationEntry();
    ApplicationOptions = ApplicationEntry->Options;
    OslpSanitizeStringOptions(ApplicationOptions);
    return Status;
}

NTSTATUS
OslpGetBootDriverFlags (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in ULONG SystemHiveHandle,
    __in DEVICE_ID OsDeviceId,
    __out PULONG Flags
    )

/*++

Routine Description:

    This routine will determine the boot driver flags to be applied while
    loading the boot drivers.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

    SystemHiveHandle - Supplies a handle for the system hive.

    OsDeviceId - Supplies a handle to the device containing the Operating System.

    Name - Supplies a pointer to a variable that receives the flags.

Return Value:

    NT status code.

--*/

{

    ULONG BootDriverFlags;
    PVOID BootParameters;
    DEVICE_INFORMATION DeviceInformation;
    BOOLEAN DisableDriverVerifier;
    ULONG RegistryFlags;
    BOOLEAN Result;
    NTSTATUS Status;
    PCHAR Options = NULL;

    BootDriverFlags = 0;

    //
    // Check whether we are booting into WinPE
    //

    if (LoaderBlock->LoadOptions != NULL) {

        Options = _strupr(LoaderBlock->LoadOptions);

        if (strstr(Options, "MININT") != NULL) {

            //
            // Set CM_SERVICE_WINPE_BOOT_LOAD for WinPE
            //

            BootDriverFlags |= CM_SERVICE_WINPE_BOOT_LOAD;
        }
    }

    //
    // If the OS is booting from iSCSI disk, then ensure all the drivers needed
    // for iSCSI boot are loaded. iSCSI boot is determined by the presence of
    // the iSCSI ACPI table. If the platform did not create an ACPI table, the
    // OS loader will have previously built one from the iSCSI boot table and
    // attached it to the loader block.
    //
    // N.B. OslGetAcpiTable searches both the loader block and the
    //      ACPI firmware tables.
    //

    Status = OslGetAcpiTable(&BootParameters,
                             ISCSI_BOOT_SIGNATURE,
                             LoaderBlock);

    if (NT_SUCCESS(Status)) {
        BootDriverFlags |= CM_SERVICE_NETWORK_BOOT_LOAD;

    } else {
        BootParameters = NULL;
    }

    //
    // If the OS is booting from a VHD, then load all the required drivers
    // regardless of its start type.
    //

    Status = BlDeviceGetInformation(OsDeviceId, &DeviceInformation);
    if (!NT_SUCCESS(Status)) {
        goto GetBootDriverFlagsEnd;
    }

    if((DeviceInformation.DeviceType == DEVICE_BLOCK_IO) &&
       (DeviceInformation.u.BlockIo.Type == BLOCK_IO_PARTITION)) {

        if (CHECK_FLAG(DeviceInformation.u.BlockIo.DeviceFlags,
                       BLOCK_IO_DEVICE_FLAGS_VIRTUAL_DEVICE) != FALSE) {

            BootDriverFlags |= CM_SERVICE_VIRTUAL_DISK_BOOT_LOAD;
        }
    }

    //
    // IntegrityServices are always enabled if TPM device present.
    //

    Status = BlTpmStatus();
    if (NT_SUCCESS(Status))
    {
        BootDriverFlags |= CM_SERVICE_MEASURED_BOOT_LOAD;
    }
    else
    {
        // reset status to success to return correct value
        Status = STATUS_SUCCESS;
    }

    //
    // Check if the last boot failed and if an administrator wants to disable
    // the driver verifier after failed boot. The driver verifier key may not
    // be present. Hence a failure is not treated as fatal. The same applies
    // to the return value from OslDisableDriverVerifier.
    //

    DisableDriverVerifier = FALSE;

    if (LoaderBlock->Extension->LastBootSucceeded == FALSE) {
        RegistryFlags = 0;
        Result = OslGetDriverVerifierOptionFlags(SystemHiveHandle,
                                                 &RegistryFlags);

        if ((Result != FALSE) &&
            ((RegistryFlags & ~VERIFIER_OPTION_VALID_FLAGS) == 0) &&
            (RegistryFlags & VERIFIER_OPTION_DISABLE_AFTER_FAILED_BOOT)) {

            OslDisableDriverVerifier(SystemHiveHandle);
            DisableDriverVerifier = TRUE;
        }
    }

    //
    // Check if the driver verifier extension is enabled in the system registry
    // and if the OS is not booting into the safe mode. The driver verifier key
    // may not be present. Hence a failure is not treated as fatal.
    //

    if (DisableDriverVerifier == FALSE) {
        RegistryFlags = 0;
        Result = OslGetDriverVerifierLevelFlags(SystemHiveHandle,
                                                &RegistryFlags);

        if ((Result != FALSE) &&
            (RegistryFlags & DRIVER_VERIFIER_EXTENSION_FLAGS) &&
            (BlApplicationOptionExists(BCDE_OSLOADER_TYPE_SAFEBOOT) == FALSE)) {

            BootDriverFlags |= CM_SERVICE_VERIFIER_BOOT_LOAD;
        }
    }

    //
    // Apply boot driver flags specified in the registry. The key may not
    // be present if additional flags are not required. Hence a failure is not
    // treated as fatal.
    //

    RegistryFlags = 0;
    Result = OslGetBootDriverFlagsFromRegistry(SystemHiveHandle,
                                               &LoaderBlock->Extension->HardwareConfigurationId,
                                               &RegistryFlags);

    if (Result != FALSE) {
        RegistryFlags &= CM_SERVICE_VALID_PROMOTION_MASK;
        BootDriverFlags |= RegistryFlags;
    }

GetBootDriverFlagsEnd:
    *Flags = BootDriverFlags;
    return Status;
}

NTSTATUS
OslpAllocatePerformanceBuffer (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine will attempt to allocate a buffer for performance tracking
    that is physically and virtually contiguous and mapped in a VA range that
    can be accessible by both the root OS and the hypervisor.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

Return Value:

    NTSTATUS - Return status of the operation.

--*/

{

    ULONGLONG BufferSize;
    ULONGLONG Pages;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    PVOID VirtualAddress;

    Pages = 0;
    PhysicalAddress.QuadPart = 0;
    VirtualAddress = NULL;

    //
    // Get the performance buffer size in MB.
    //

    Status = BlGetApplicationOptionInteger(
                BCDE_OSLOADER_TYPE_PERFORMANCE_DATA_MEMORY,
                &BufferSize);

    if (!NT_SUCCESS(Status) || (BufferSize == 0)) {
        goto AllocatePerformanceBufferEnd;
    }

    //
    // Allocate contigous memory with the required size. Buffer size is
    // expressed in MB and each MB is 256 pages. Align up to use an entire PDE
    // in case this memory has to be mapped in user-mode.
    //

    Pages = BufferSize * PAGES_PER_MB;
    Pages = ALIGN_UP_BY(Pages, PTE_PER_PAGE);
    Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                       Pages,
                                       OSL_MEMORY_TYPE_KERNEL_DATA_STRUCTURE,
                                       MEMORY_ATTRIBUTE_NONE,
                                       PTE_PER_PAGE);

    if (!NT_SUCCESS(Status)) {
        goto AllocatePerformanceBufferEnd;
    }

    Status = OslFwMapPhysicalAddressInFirmwareVaSpace(
                    PhysicalAddress,
                    Pages * PAGE_SIZE,
                    MEMORY_ATTRIBUTE_NONE,
                    &VirtualAddress);

    if (!NT_SUCCESS(Status)) {
        goto AllocatePerformanceBufferEnd;
    }

    //
    // Store the address for the kernel to pick up.
    //

    LoaderBlock->Extension->BbtBuffer = VirtualAddress;

AllocatePerformanceBufferEnd:
    if (!NT_SUCCESS(Status)) {
        if (VirtualAddress != NULL) {
            BlMmUnmapVirtualAddress(VirtualAddress, Pages * PAGE_SIZE);
        }

        if (PhysicalAddress.QuadPart != 0) {
            BlMmFreePhysicalPages(PhysicalAddress);
        }
    }

    return Status;
}

VOID
OslpLogLoaderTransition (
    _In_ PSTR BootType,
    _In_ ULONGLONG UserInputTime,
    _In_ BOOLEAN LastBootSucceeded,
    _In_ BOOLEAN LastShutdownSucceeded,
    _In_ UCHAR BootAttemptCount,
    _In_ BCDE_BOOT_MENU_POLICY MenuPolicy
    )

/*++

Routine Description:

    This routine logs the loader transition event for telemetry.

Arguments:

    BootType - Supplies a pointer to a string describing the type of boot
        (such as "Cold", "Hiber", "Resume")

    UserInputTime - Supplies the amount of time the loader application spent
        waiting for user input.

    LastBootSucceeded - Supplies a flag indicating whether the last boot
        was successful.

    LastShutdownSucceeded - Supplies a flag indicating whether the last
        shutdown was successful.

    BootAttemptCount - Supplies the number of consecutive times the boot
        manager has attempted to boot into this operating system.

    MenuPolicy - Supplies a menu policy enum indicating which type of advanced
        options menu that should be shown to the user.

Return Value:

    None.

--*/

{

    DEVICE_IO_INFORMATION DeviceIoInformation;
    NTSTATUS Status;

    Status = BlDeviceGetIoInformation(&DeviceIoInformation);
    if (!NT_SUCCESS(Status)) {
        RtlZeroMemory(&DeviceIoInformation, sizeof(DEVICE_IO_INFORMATION));
    }

    TraceLoggingWrite(OslBootProvider,
                      "OsLaunch",
                      TraceLoggingLevel(TRACE_LEVEL_INFORMATION),
                      TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES),
                      TraceLoggingBootPartB(OslApplicationIdentifier, OslBootId),
                      TraceLoggingStruct(2, "IO"),
                          TraceLoggingUInt64(DeviceIoInformation.TotalBytesRead, "BytesRead"),
                          TraceLoggingUInt64(DeviceIoInformation.TotalBytesWritten, "BytesWritten"),
                      TraceLoggingString(BootType, "BootType"),
                      TraceLoggingUInt64(UserInputTime, "UserInputTime"),
                      TraceLoggingBool(LastShutdownSucceeded, "LastShutdownSucceeded"),
                      TraceLoggingBool(LastBootSucceeded, "LastBootSucceeded"),
                      TraceLoggingUInt8(BootAttemptCount, "BootAttemptCount"),
                      TraceLoggingString((MenuPolicy == MenuPolicyLegacy ? "Legacy" : "Standard"), "MenuPolicy"));

    return;
}

NTSTATUS
OslInitializeBitlockerStatus (
    VOID
    )
{

    NTSTATUS Status;
    ULONG FveBitmap;
    ULONG FveStaticFlags;
    GENERIC_BUFFER PersistentBuffer;
    GUID PersistGuid = FVE_BITLOCKER_STATUS_GUID;

    BlInitGenericBuffer(&PersistentBuffer, NULL, 0);
    OslManufacturingBitlockerEnabled = TRUE;

    //
    // Get bitlocker status first.
    //

    Status = BlFveQueryStaticDeviceStatus(OslLoadDevice,
                                          &FveStaticFlags,
                                          &FveBitmap);

    if (NT_SUCCESS(Status)) {

        //
        // Save away the manufacturing mode Bitlocker information so the flags
        // do not need to be queried again later.
        //

        OslManufacturingBitlockerEnabled =
            (BlFveBitlockerEnabled(FveStaticFlags) != FALSE);

        //
        // Allocate persistent buffer
        //

        PersistentBuffer.DataSize = sizeof(FveStaticFlags);
        Status = BlPdAllocateData(NULL,
                                  &PersistGuid,
                                  MEMORY_ATTRIBUTE_ALLOCATION_KSEG,
                                  &PersistentBuffer);

        if (!NT_SUCCESS(Status)) {
            goto InitializeBitlockerStatusEnd;
        }

        //
        // Set the value of persistent buffer. It is 1 when bitlocker is on
        // and not suspended, otherwise it is 0.
        //

        if ((BlFveBitlockerEnabled(FveStaticFlags) == TRUE) &&
            (BlFveBitlockerSuspended(FveStaticFlags) == FALSE)) {
            *((ULONG*)PersistentBuffer.Buffer) = 1;

        } else {
            *((ULONG*)PersistentBuffer.Buffer) = 0;
        }
    }

InitializeBitlockerStatusEnd:
    if (!NT_SUCCESS(Status) &&
        (PersistentBuffer.Buffer != NULL)) {

        NT_VERIFY(
            NT_SUCCESS(
                BlPdFreeData(NULL, &PersistGuid, &PersistentBuffer)));
    }

    return Status;
}

