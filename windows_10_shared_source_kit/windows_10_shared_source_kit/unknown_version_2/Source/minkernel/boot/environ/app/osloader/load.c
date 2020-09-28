/*++

Copyright (c) Microsoft Corporation

Module Name:

    load.c

Abstract:

    This module provides the code for loading boot drivers into memory.

Environment:

    Boot Environment.

--*/

// ------------------------------------------------------------------- Includes

#include "osloader.h"
#include "systemevents.h"
#include <mincrypl.h>
#include <bootlibp.h>
#include <sipolicy.h>
#include <cimin.h>

#define FEATURE_STAGING_LEGACY_MODE
#include <featurestaging-aow.h>
#include <featurestaging-aowmobile.h>

// ---------------------------------------------------------------- Definitions

#define SYSTEM_CATALOG_NAME  L"windows-legacy-whql.cat"
#define REVOCATION_FILE_PATH L"Boot\\EFI\\boot.stl"
#define SHA1_TEST_FILE       L"System32\\winsha1.bin"
#define SHA1MONTE_TEST_FILE  L"System32\\winsha1monte.bin"
#define RSA_TEST_FILE        L"System32\\winrsa.bin"

//
// Define valid driver types for driver load.
//
// N.B. 1. Core Extensions that need to be validated against the HAL signing
//         certificate will also have the OSL_IMAGE_FLAGS_HAL_EXTENSION set.
//
//      2. OSL_IMAGE_FLAGS_NONE is included for readability.
//

#define OSL_LOAD_FLAGS_VALID_DRIVER \
    (OSL_IMAGE_FLAGS_EARLY_LAUNCH | \
     OSL_IMAGE_FLAGS_CORE_DRIVER | \
     OSL_IMAGE_FLAGS_HAL_EXTENSION | \
     OSL_IMAGE_FLAGS_CORE_EXTENSION | \
     OSL_IMAGE_FLAGS_NONE)

typedef struct _WEAK_CRYPTO_VALUE_NAME {
    PCWSTR Value;
    UCHAR Type;
} WEAK_CRYPTO_VALUE_NAME, *PWEAK_CRYPTO_VALUE_NAME;

typedef const WEAK_CRYPTO_VALUE_NAME *PCWEAK_CRYPTO_VALUE_NAME;

#define WEAK_CRYPTO_VALUE_DWORD(a)   {(a), REG_DWORD}
#define WEAK_CRYPTO_VALUE_BINARY(a)  {(a), REG_BINARY}
#define WEAK_CRYPTO_VALUE_MULTISZ(a) {(a), REG_MULTI_SZ}

typedef struct _WEAK_CRYPTO_SETTING {
    UCHAR Set;
    UCHAR Type;
    union
    {
        struct
        {
            ULONG HashCount;

            _Field_size_(HashCount)
            UCHAR (*Hashes)[RTL_SHA256_HASH_LEN];
        };

        ULONG Options;
        LARGE_INTEGER LargeInteger;
    };
} WEAK_CRYPTO_SETTING, *PWEAK_CRYPTO_SETTING;

typedef struct _WEAK_CRYPTO_SETTINGS {
    BOOLEAN Enabled;
    WEAK_CRYPTO_SETTING Values[CI_WEAK_CRYPTO_NUM_REG_ENTRIES];
} WEAK_CRYPTO_SETTINGS, *PWEAK_CRYPTO_SETTINGS;

typedef struct _CORE_EXTENSION_SUBGROUP_INFORMATION {

    //
    // Head of the list of drivers that belong into this sub-group.
    //

    LIST_ENTRY ListHead;

    //
    // Load order groups that belong to this sub-group. The list should
    // terminated by a NULL string.
    //

    const PCWSTR *LoadOrderGroups;

    //
    // The flags to be applied to validate drivers belonging to this sub-group.
    //

    ULONG TrustValidationFlags;
} CORE_EXTENSION_SUBGROUP_INFORMATION, *PCORE_EXTENSION_SUBGROUP_INFORMATION;

//
// Define which pico drivers are allowed to load.
//

#if defined(_AMD64_)

#define OSL_LXSS_ALLOWED
#define OSL_PALCORE_ALLOWED

#endif

#if (defined(_X86_) || defined(_ARM_)) && (FEATURE_STAGING_IS_FEATURE_ENABLED(Feature_AOW_Desktop) || FEATURE_STAGING_IS_FEATURE_ENABLED(Feature_AOW_Mobile))

#define OSL_ADSS_ALLOWED

#endif

// -------------------------------------------------------------------- Globals

typedef enum _INTEGRITY_CHECK_POLICY {
    IntegrityCheckUninitialized,
    IntegrityCheckDisabled,
    IntegrityCheckEnabled,
    IntegrityCheckAllImagesEnabled
} INTEGRITY_CHECK_POLICY, *PINTEGRITY_CHECK_POLICY;

INTEGRITY_CHECK_POLICY LoadIntegrityCheckPolicy = IntegrityCheckUninitialized;
BOOLEAN LoadIntegrityTestSigningEnabled = FALSE;

const PWCHAR OslMicrosoftBootImages[] =
    { L"bootvid.dll", L"ci.dll", L"clfs.sys", L"fvevol.sys", HAL_DEFAULT_NAME,
      KD_FRIENDLY_NAME, L"ksecdd.sys", KERNEL_DEFAULT_NAME, L"pshed.dll",
      UPDATE_DEFAULT_NAME, HARDWARE_POLICY_NAME, KD_EXTENSION_FRIENDLY_NAME,
      L"lxcore.sys", NULL };

//
// Define the list of Core Drivers and TPM Core driver list.
//
// N.B. VerifierExt.sys is Driver Verifier extension and it needs to
//      be loaded before any other driver.
//

const PCWSTR OslCoreDriverServices[] =
    { L"VERIFIEREXT", L"WDF01000", L"ACPIEX", L"CNG",

#if defined(OSL_ADSS_ALLOWED)

      L"ADSS",

#endif

#if defined(OSL_LXSS_ALLOWED)

      L"LXSS",

#endif

#if defined(OSL_PALCORE_ALLOWED)

    L"PALCORE",

#endif

    NULL };

const PCWSTR OslTpmCoreDriverServices[] =
    { L"ACPISIM", L"ACPI", NULL };

const PCWSTR OslTpmCoreDriverServicesMeasured[] =
    {  L"ACPISIM", L"ACPI", L"MSISADRV", L"PCI",
      #if !defined(BUILD_PRODUCTION_SIGNED) && !defined(BUILD_FLIGHT_SIGNED)
        L"FTPMSIM",
      #endif
      L"TPM", NULL };

const PWCHAR OslElamDriverAllowedImports[] = { L"tbs.sys", NULL };

const UNICODE_STRING OslpDriverVerifierExtensionString =
        RTL_CONSTANT_STRING(L"VERIFIEREXT");

//
// Define the set of load order groups that will be considered as core
// extension drivers.
//

const PCWSTR OslCorePlatformExtensionLoadOrderGroups[] =
    { L"Core Platform Extensions", NULL };


const PCWSTR OslCoreSecurityExtensionLoadOrderGroups[] =
    { L"Core Security Extensions", NULL };

CORE_EXTENSION_SUBGROUP_INFORMATION OslCoreExtensionSubGroups[2];

//
// Weak crypto support.
// WEAK_CRYPTO_VALUE_NAME arrays must be in order of WEAK_CRYPTO_REG_ENTRY.
//

MINCRYPT_WEAK_CRYPTO_POLICIES OslWeakCryptoPolicies = {0};
MINCRYPT_WEAK_CRYPTO_POLICY LoadWeakCryptoPolicy[CI_WEAK_CRYPTO_MAX_ALG] = {0};
NTSTATUS OslWeakCryptoStatus = STATUS_SUCCESS;

static const WEAK_CRYPTO_VALUE_NAME
LoadWeakCryptoMD5Values[CI_WEAK_CRYPTO_NUM_REG_ENTRIES] =
{
    WEAK_CRYPTO_VALUE_DWORD(CI_WEAK_CRYPTO_MD5_FLAGS_SUBKEY),
    WEAK_CRYPTO_VALUE_BINARY(CI_WEAK_CRYPTO_MD5_CERT_AFTER_TIME_SUBKEY),

    //
    // MD5 file hashes are not supported:
    // CI_WEAK_CRYPTO_MD5_FILE_AFTER_TIME_SUBKEY
    //

    { {0}, REG_BINARY },
    WEAK_CRYPTO_VALUE_BINARY(CI_WEAK_CRYPTO_MD5_TIMESTAMP_AFTER_TIME_SUBKEY),
    WEAK_CRYPTO_VALUE_MULTISZ(CI_WEAK_CRYPTO_MD5_ALLOW_LIST_SUBKEY),
};

static const WEAK_CRYPTO_VALUE_NAME
LoadWeakCryptoSHA1Values[CI_WEAK_CRYPTO_NUM_REG_ENTRIES] =
{
    WEAK_CRYPTO_VALUE_DWORD(CI_WEAK_CRYPTO_SHA1_FLAGS_SUBKEY),
    WEAK_CRYPTO_VALUE_BINARY(CI_WEAK_CRYPTO_SHA1_CERT_AFTER_TIME_SUBKEY),
    WEAK_CRYPTO_VALUE_BINARY(CI_WEAK_CRYPTO_SHA1_FILE_AFTER_TIME_SUBKEY),
    WEAK_CRYPTO_VALUE_BINARY(CI_WEAK_CRYPTO_SHA1_TIMESTAMP_AFTER_TIME_SUBKEY),
    WEAK_CRYPTO_VALUE_MULTISZ(CI_WEAK_CRYPTO_SHA1_ALLOW_LIST_SUBKEY),
};

static const ALG_ID LoadWeakCryptoAlgorithms[CI_WEAK_CRYPTO_MAX_ALG] =
{
    CALG_MD5,
    CALG_SHA1
};

//
// Must be in order of LoadWeakCryptoAlgorithms.
//

static const PCWEAK_CRYPTO_VALUE_NAME
LoadWeakCryptoValues[CI_WEAK_CRYPTO_MAX_ALG] =
{
    LoadWeakCryptoMD5Values,
    LoadWeakCryptoSHA1Values
};

//
// Have the system integrity policies already been processed?
//

BOOLEAN LoadProcessedSystemIntegrityPolicy = FALSE;

// ----------------------------------------------------------------- Prototypes

NTSTATUS
LoadImageEx (
    __in PUNICODE_STRING ImageDirectoryPath,
    __in PWSTR ImageName,
    __in_opt PUNICODE_STRING ServiceFilePath,
    __in DEVICE_ID DeviceId,
    __in MEMORY_TYPE MemoryType,
    __in PLIST_ENTRY LoadedImageList,
    __deref_out PKLDR_DATA_TABLE_ENTRY *DataTableEntry,
    __in ULONG KldrFlags,
    __in ULONG BldrFlags,
    __in ULONG ImageFlags,
    __out_opt PULONG LoadInformation,
    __in ULONG OslLoadImageFlags
    );

NTSTATUS
LoadImports (
    __in DEVICE_ID DeviceId,
    __in MEMORY_TYPE MemoryType,
    __in PKLDR_DATA_TABLE_ENTRY DataTableEntry,
    __in PLIST_ENTRY LoadedImageList,
    __in ULONG ImageFlags,
    __in ULONG OslLoadImageFlags
    );

NTSTATUS
GetFullPath (
    __in PWSTR FilePath,
    __deref_out PWSTR *FullPath
    );

ULONG
GetImageValidationFlags (
    __in PWSTR LoadFile,
    __in ULONG OslLoadImageFlags
    );

VOID
KnownAnswerTest (
    __in DEVICE_ID OsDeviceId
    );

BOOLEAN
OslpAllowedEarlyLaunchImport (
    __in PCWSTR FileName
    );

PWSTR
OslpConvertFromSymbolicPath (
    __in PWSTR PathToConvert
    );

PWSTR
OslpConvertToSymbolicPath (
    __in PWSTR PathToConvert
    );

VOID
OslpFilterCoreExtensionsOnGroups (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in_xcount(null_terminated_at_compile_time) const PCWSTR *LoadOrderGroups,
    __inout PLIST_ENTRY FilteredDriverList
    );

VOID
OslpFilterCoreDrivers (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
OslpFilterDriverList (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
OslpFilterDriverListOnGroup (
    __in PUNICODE_STRING Group,
    __inout PLIST_ENTRY BootDriverList,
    __inout PLIST_ENTRY FilteredDriverList
    );

VOID
OslpFilterDriverListOnServices (
    __in_xcount(null_terminated_at_compile_time) const PCWSTR *ServiceNames,
    __inout PLIST_ENTRY BootDriverList,
    __inout PLIST_ENTRY FilteredDriverList
    );

VOID
ReportCodeIntegrityFailure (
    __in NTSTATUS FailureStatus
    );

VOID
StripExtensionImportRelocations (
    __in PKLDR_DATA_TABLE_ENTRY ImageEntry,
    __in PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor
    );

VOID
OslpSetSigningPolicy (
    _In_ ULONG SystemHiveHandle
    );

NTSTATUS
OslpSetWeakCryptoPolicy (
    _In_ ULONG SystemHiveHandle
    );

NTSTATUS
OslpLoadWeakCryptoPolicy(
    _In_ ULONG SystemHiveHandle,
    _Inout_updates_(CI_WEAK_CRYPTO_MAX_ALG)
        WEAK_CRYPTO_SETTINGS Default[CI_WEAK_CRYPTO_MAX_ALG],
    _Inout_updates_(CI_WEAK_CRYPTO_MAX_ALG)
        WEAK_CRYPTO_SETTINGS Config[CI_WEAK_CRYPTO_MAX_ALG]
    );

NTSTATUS
OslpLoadWeakCryptoValues (
    _In_ ULONG SystemHiveHandle,
    _In_ PCM_KEY_NODE SettingsCell,
    _In_reads_(CI_WEAK_CRYPTO_NUM_REG_ENTRIES)
        const WEAK_CRYPTO_VALUE_NAME Values[CI_WEAK_CRYPTO_NUM_REG_ENTRIES],
    _Inout_updates_(CI_WEAK_CRYPTO_NUM_REG_ENTRIES)
        WEAK_CRYPTO_SETTING Settings[CI_WEAK_CRYPTO_NUM_REG_ENTRIES]
    );

VOID
OslpFreeWeakCryptoValues(
    _Inout_updates_(CI_WEAK_CRYPTO_NUM_REG_ENTRIES)
        WEAK_CRYPTO_SETTING Settings[CI_WEAK_CRYPTO_NUM_REG_ENTRIES]
    );

NTSTATUS
OslpRegMultiSzToThumbprintArray(
    _In_reads_(MultiSzSize) PVOID MultiSz,
    _In_ ULONG MultiSzSize,
    _Outptr_result_buffer_(*ThumbprintCount) UCHAR (**Thumbprints)[RTL_SHA256_HASH_LEN],
    _Out_ PULONG ThumbprintCount
    );

UCHAR
OslpHexDecodeCharW(
    _In_ WCHAR ch
    );

NTSTATUS
OslpHexDecodeSha256Thumbprint(
    _In_reads_(RTL_SHA256_HASH_LEN * 2) PCWCHAR EncodedData,
    _Out_writes_(RTL_SHA256_HASH_LEN) PUCHAR DecodedData
    );

NTSTATUS
OslpWeakCryptoConcatAllowLists(
    _In_reads_(ListCount1) UCHAR (*List1)[RTL_SHA256_HASH_LEN],
    _In_ ULONG ListCount1,
    _In_reads_(ListCount2) UCHAR (*List2)[RTL_SHA256_HASH_LEN],
    _In_ ULONG ListCount2,
    _Outptr_result_buffer_(*MergedCount) UCHAR (**MergedList)[RTL_SHA256_HASH_LEN],
    _Out_ PULONG MergedCount
    );

int
__cdecl
OslpSortCompareRoutineSHA256(
    _In_reads_bytes_(RTL_SHA256_HASH_LEN) const void *Element1,
    _In_reads_bytes_(RTL_SHA256_HASH_LEN) const void *Element2
    );

VOID
OslpFreeWeakCryptoPolicies(
    VOID
    );

VOID
OslpProcessLoadConfigDirectoryForModule (
    _In_ PKLDR_DATA_TABLE_ENTRY DataTableEntry
    );

NTSTATUS
OslpLoadWhqlRegSettings (
    _In_ ULONG SystemHiveHandle,
    _Out_ PULONG Settings
    );

NTSTATUS
OslpLoadSysDevWhqlEnforcementDateTime (
    _In_ ULONG SystemHiveHandle,
    _Out_ PLARGE_INTEGER EnforceDateTime
    );

VOID
OslpSetSysDevWhqlPolicy (
    _In_ ULONG SystemHiveHandle
    );

// ------------------------------------------------------------------ Functions

VOID
OslDestroyDriverList (
    __in PLIST_ENTRY DriverList
    )

/*++

Routine Description:

    This routine destroys a driver list.

Arguments:

    DriverList - Supplies a pointer to the head of the driver list.

Return Value:

    None.

--*/

{

    PBLDR_DATA_TABLE_ENTRY BootLoaderEntry;
    PLIST_ENTRY CurrentEntry;
    PBOOT_DRIVER_LIST_ENTRY DriverListEntry;
    PBOOT_DRIVER_NODE DriverNode;
    PLIST_ENTRY NextEntry;

    CurrentEntry = DriverList->Flink;
    while (CurrentEntry != DriverList) {
        DriverNode = CONTAINING_RECORD(CurrentEntry,
                                       BOOT_DRIVER_NODE,
                                       ListEntry.Link);

        //
        // Release the driver node allocations (DriverDependencies and Group are
        // pointers into the hive, and don't need to be freed).
        //

        RtlFreeUnicodeString(&DriverNode->Name);

        //
        // Release the driver list entry allocations.
        //

        DriverListEntry = &DriverNode->ListEntry;
        RtlFreeUnicodeString(&DriverListEntry->FilePath);
        RtlFreeUnicodeString(&DriverListEntry->RegistryPath);

        //
        // Release the boot loader table entry allocations.
        //

        BootLoaderEntry = DriverListEntry->LdrEntry;
        if (BootLoaderEntry != NULL) {
            BlImgUnLoadImage(BootLoaderEntry->KldrEntry.DllBase,
                             BootLoaderEntry->KldrEntry.SizeOfImage,
                             BootLoaderEntry->KldrEntry.Flags);

            BlLdrFreeDataTableEntry(&BootLoaderEntry->KldrEntry);
        }

        //
        // Move to the next entry, freeing the current driver node.
        //

        NextEntry = CurrentEntry->Flink;
        RemoveEntryList(CurrentEntry);
        BlMmFreeHeap(DriverNode);
        CurrentEntry = NextEntry;
    }

    NT_ASSERT(IsListEmpty(DriverList) != FALSE);

    return;
}

VOID
OslFilterCoreExtensions (
    __in ULONG Phase,
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine filters the core extensions out of the boot driver list and
    adds them to the appropriate list in loader block.

    N.B. The Core Extension group is special as it consists of multiple
         sub-groups where each sub-group has a different trust-level and
         requires validating drivers against a different signing certificate.
         To allow this to be done easily, the drivers are first moved to
         temporary lists until they are validated (Phase 0). Once validated,
         they are all merged together in the appropriate loader block list
         (Phase 1).

Arguments:

    Phase - Supplies the phase for the filtering process.

    LoaderBlock - Supplies a pointer to the loader parameter block.

Return Value:

    None.

--*/

{

    ULONG Index;
    PLIST_ENTRY SubGroupList;

    NT_ASSERT(Phase < 2);

    //
    // Filter core extension drivers, moving them to their own list.
    //
    // Phase 0: Filter entries from the boot driver list into separate
    //     per-sub-group lists.
    //
    // Phase 1: Merge per-sub-group lists into the Core Extension driver list
    //     in the loader block.
    //

    for (Index = 0; Index < ARRAYSIZE(OslCoreExtensionSubGroups); Index += 1) {
        if (Phase == 0) {
            OslpFilterCoreExtensionsOnGroups(
                LoaderBlock,
                OslCoreExtensionSubGroups[Index].LoadOrderGroups,
                &OslCoreExtensionSubGroups[Index].ListHead);

        } else {
            SubGroupList = &OslCoreExtensionSubGroups[Index].ListHead;

            //
            // Append the sub-group list to the Core Extension list.
            //
            // N.B. This will also insert the head of the append list onto the
            //      Core Extension list.
            //

            AppendTailList(&LoaderBlock->CoreExtensionsDriverListHead,
                           SubGroupList);

            //
            // Remove the head of the sub-group list as that shouldn't be
            // on the Core Extension list.
            //

            RemoveEntryList(SubGroupList);
        }
    }

    return;
}

NTSTATUS
OslInitializeCodeIntegrity (
    __in DEVICE_ID OsDeviceId,
    __in ULONG SystemHiveHandle
    )

/*++

Routine Description:

    Initializes Code Integrity global variables, verifies the results of known
    answer tests for crypto operations used in code integrity, and processes
    test vectors specified by FIPS test lab.  Note that the boot manager
    performs integrity checks on this image before transferring control into
    it, implying that the self integrity check does not need to be repeated
    here.

Arguments:

    OsDeviceId - Supplies the id of a device containing the OS.

    SystemHiveHandle - Supplies the handle to the system hive.

Return Value:

    STATUS_SUCCESS when successful.

    Another error status code if returned from a sub-routine.

--*/

{

    ULONG CatalogPolicy;
    PWCHAR CatalogRoot;
    BOOLEAN IntegrityChecksDisabled;
    INTEGRITY_CHECK_POLICY Policy;
    PWSTR PrecedenceList[1];
    PWCHAR RevocationList;
    NTSTATUS Status;
    BOOLEAN TestSigningEnabled;

    CatalogRoot = NULL;
    RevocationList = NULL;

    //
    // Integrity checks are run whenever possible to provide boot driver
    // validity information to the kernel.  By default, integrity checks are
    // also used to block load of any boot drivers with invalid image hashes.
    // Integrity checks can be disabled by a user. And a user can additionally
    // enable test signing, to allow for test root certificates. Query the boot
    // library to get the current code integrity policy.
    //
    // By default, x64 and ARM will validate the integrity of all loaded
    // images. x86 will validate the integrity of all loaded images if Secure
    // Boot is enabled. Other architectures just validate the integrity of
    // Microsoft images.
    //

    BlImgQueryCodeIntegrityBootOptions(BlGetApplicationEntry(),
                                       &IntegrityChecksDisabled,
                                       &TestSigningEnabled,
                                       NULL);

    if (IntegrityChecksDisabled != FALSE) {
        Policy = IntegrityCheckDisabled;

    } else {
#pragma prefast ( suppress: __WARNING_CONST_CONST_COMP __WARNING_LOGICALOROFCONSTANTS ZEROLOGICALANDLOSINGSIDEEFFECTS, "logical-or of constants is by design and BlSecureBootIsPolicyActive does not cause any side effects if left side condition is false")
        if (BOOT_APPLICATION_MACHINE_TYPE == IMAGE_FILE_MACHINE_AMD64 ||
            BOOT_APPLICATION_MACHINE_TYPE == IMAGE_FILE_MACHINE_ARMNT ||
            (BOOT_APPLICATION_MACHINE_TYPE == IMAGE_FILE_MACHINE_I386 &&
             BlSecureBootIsPolicyActive() != FALSE)) {
            Policy = IntegrityCheckAllImagesEnabled;

        } else {
            Policy = IntegrityCheckEnabled;
        }
    }

    //
    // Set the required signing policy.
    //

    OslpSetSigningPolicy(SystemHiveHandle);

    //
    //  Process System Integrity Policy
    //

    Status = OslpProcessSIPolicy(IntegrityChecksDisabled);
    if (!NT_SUCCESS(Status)) {
        goto OslInitializeCodeIntegrityEnd;
    }

    //
    // Set any weak crypto policy.
    // Store the result so that CI can log an event if there is a parse error.
    //

    OslWeakCryptoStatus = OslpSetWeakCryptoPolicy(SystemHiveHandle);


    //
    // Load SysDev WHQL signing policy.
    //

    OslpSetSysDevWhqlPolicy(SystemHiveHandle);

    //
    // If debugger is enabled, load and process all test vectors specified
    // by FIPS test lab.
    //

#if DBG

    if (BlBdDebuggerEnabled() != FALSE) {
        KnownAnswerTest(OsDeviceId);
    }

#endif

    //
    // Register code integrity catalog usage and policy with the boot library.
    // Optimize a catalog search to search OEM catalogs before all other
    // searches.  On x86, disable the use of catalogs to always optimize for
    // performance.
    //

    Status = GetFullPath(CATALOG_ROOT, &CatalogRoot);
    if (!NT_SUCCESS(Status)) {
        goto OslInitializeCodeIntegrityEnd;
    }

    Status = GetFullPath(REVOCATION_FILE_PATH, &RevocationList);
    if (!NT_SUCCESS(Status)) {
        goto OslInitializeCodeIntegrityEnd;
    }

#pragma prefast ( suppress: __WARNING_CONST_CONST_COMP ZEROLOGICALANDLOSINGSIDEEFFECTS, "BOOT_APPLICATION_MACHINE_TYPE intentionally const and BlSecureBootIsPolicyActive does not cause any side effects if left side condition is false" )
    if (BOOT_APPLICATION_MACHINE_TYPE == IMAGE_FILE_MACHINE_I386 &&
        BlSecureBootIsPolicyActive() == FALSE) {
        CatalogPolicy = CATALOG_FLAGS_SYSTEM_CATALOG_ONLY;

    } else {
        CatalogPolicy = CATALOG_FLAGS_NONE;
    }

    PrecedenceList[0] = L"OEM";
    Status = BlImgRegisterCodeIntegrityCatalogs(OsDeviceId,
                                                CatalogRoot,
                                                SYSTEM_CATALOG_NAME,
                                                RevocationList,
                                                PrecedenceList,
                                                1,
                                                CatalogPolicy);

    if (!NT_SUCCESS(Status)) {
        goto OslInitializeCodeIntegrityEnd;
    }

    //
    // Code integrity initialization is complete.  Update global state to
    // reflect the selected validation policy.
    //

    LoadIntegrityCheckPolicy = Policy;
    LoadIntegrityTestSigningEnabled = TestSigningEnabled;

    //
    // Returning an error status blocks further initialization of the OS
    // loader.  This is only appropriate if integrity enforcement is enabled
    // and code integrity initialization or validation of the boot application
    // has failed.  If integrity enforcement is not enabled, replace any error
    // status with a success status before exiting.
    //
    // N.B. The global integrity check policy is left in the uninitialized
    //      state if fundamental code integrity initialization fails.  This
    //      blocks future code integrity operations even if the absence of
    //      integrity enforcement forces a success status return code that
    //      allows OS loader execution to proceed.
    //

OslInitializeCodeIntegrityEnd:
    if (Policy < IntegrityCheckEnabled) {
        Status = STATUS_SUCCESS;
    }

    if (CatalogRoot != NULL) {
        BlMmFreeHeap(CatalogRoot);
    }

    if (RevocationList != NULL) {
        BlMmFreeHeap(RevocationList);
    }

    if (!NT_SUCCESS(Status)) {
        ReportCodeIntegrityFailure(Status);
    }

    return Status;
}

NTSTATUS
OslLoadCoreExtensions (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in PUNICODE_STRING DefaultPath,
    __in DEVICE_ID DefaultDevice,
    __in BOOLEAN LastKnownGoodBoot
    )

/*++

Routine Description:

    This routine will load all the Core Extension binaries and their imports
    into memory.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

    DefaultPath - Supplies the default file path to use when a driver node only
        specifies a relative image path.

    DefaultDevice - Supplies the default device to use when a driver node only
        specifies a relative image path.

    LastKnownGoodBoot - Supplies whether the last known good boot set should
        be booted into (TRUE) or not (FALSE).

Return Value:

    NTSTATUS code.

--*/

{

    ULONG Index;
    NTSTATUS Status;
    PLIST_ENTRY SubGroupList;
    ULONG ValidationFlags;

    //
    // Walk the Core Extension sub-groups and load all drivers within each
    // sub-group (including their dependencies). The drivers are to be loaded
    // in the order of the sub-groups.
    //

    Status = STATUS_SUCCESS;
    for (Index = 0; Index < ARRAYSIZE(OslCoreExtensionSubGroups); Index += 1) {
        SubGroupList = &OslCoreExtensionSubGroups[Index].ListHead;
        ValidationFlags = OslCoreExtensionSubGroups[Index].TrustValidationFlags;
        Status = OslLoadDrivers(SubGroupList,
                                &LoaderBlock->LoadOrderListHead,
                                DefaultPath,
                                DefaultDevice,
                                ValidationFlags,
                                LastKnownGoodBoot);

        if (!NT_SUCCESS(Status)) {
            goto LoadCoreExtensionsEnd;
        }
    }

LoadCoreExtensionsEnd:
    return Status;
}

NTSTATUS
OslLoadDrivers (
    __in PLIST_ENTRY DriverList,
    __in PLIST_ENTRY LoadedDriverList,
    __in PUNICODE_STRING DefaultPath,
    __in DEVICE_ID DefaultDevice,
    __in ULONG Flags,
    __in BOOLEAN LastKnownGoodBoot
    )

/*++

Routine Description:

    This routine will load all the boot drivers (and their dependencies)
    specified by the provided linked list into memory.
Arguments:

    DriverList - Linked list (of BOOT_DRIVER_NODE's), specifying the drivers
        to load into memory.

    LoadedDriverList - Linked list (of KLDR_DATA_TABLE_ENTRY's), specifying the
        drivers already loaded into memory.

    DefaultPath - The default file path to use when a driver node only
        specifies a relative image path.

    DefaultDevice - The default device to use when a driver node only
        specifies a relative image path.

    Flags - Supplies a bitmask containing additional load options. Possible
        values include:

        OSL_IMAGE_FLAGS_NONE: Specifies that no additional logic is required.

        OSL_IMAGE_FLAGS_EARLY_LAUNCH : Specifies that the image is an anti-
            malware driver and is subject to a higher bar of integrity policy.

        OSL_IMAGE_FLAGS_CORE_DRIVER : Specifies that the image is an image to
            be loaded before the anti-malware driver and is subject to a higher
            bar of integrity policy.

        OSL_IMAGE_FLAGS_ELAM_IMPORT : Specifes that the image is an image to be
            initialized before the anti-malware driver because the anti-malware
            driver imports from it.

        OSL_IMAGE_FLAGS_WINDOWS : Specifies that the image is subject to
            Windows integrity policy.

        OSL_IMAGE_FLAGS_CORE_EXTENSION : Specifies that the image is to
             be loaded before the TpmCore drivers and is subject to a higher
            bar of integrity policy.

        This routine assumes the caller sets only one bit among the valid bits
        (e.g. caller doesn't pass in (OSL_IMAGE_FLAGS_EARLY_LAUNCH |
              OSL_IMAGE_FLAGS_CORE_DRIVER)).

    LastKnownGoodBoot - If TRUE, indicates that we should boot to the last
        known good boot set.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if DefaultDevice is invalid, DefaultPath is NULL
        or DriverList is NULL.
    STATUS_NO_MEMORY if there is insufficient memory to perform the request.

--*/

{

    PBOOT_DRIVER_LIST_ENTRY DriverEntry;
    PBOOT_DRIVER_NODE DriverNode;
    PWCHAR DriverPath;
    LARGE_INTEGER EndTime;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[1];
    LARGE_INTEGER Frequency;
    ULONGLONG FrequencyInMilliseconds;
    BOOLEAN IgnoreMissingBootStartDrivers;
    ULONGLONG Integer;
    BCDE_DRIVER_LOAD_FAILURE_POLICY LoadFailurePolicy;
    ULONG LoadInformation;
    PLIST_ENTRY NextEntry;
    ULONG OslImageFlags;
    ULONG Pass;
    NTSTATUS ReturnStatus;
    LARGE_INTEGER StartTime;
    NTSTATUS Status;
    BOOLEAN StringAllocated;
    UNICODE_STRING SubDirectory;
    PUNICODE_STRING SubDirectoryPath;
    ULONGLONG TimeTaken;

#if DBG

    ULONGLONG SafemodeType;

#endif

    if ((DriverList == NULL) || (DefaultPath == NULL)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // This routine only supports loading Core drivers, Core Extensions,
    // Early Launch and normal drivers (i.e. no flags).
    //

    if ((Flags & ~OSL_LOAD_FLAGS_VALID_DRIVER) != 0) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Determine the policy to use when confronted with a driver load failure.
    // The default behavior is to treat most boot driver load failures as
    // fatal.  The rationale is that anything marked as a boot start driver
    // is critical to booting the operating system.  In general, if a driver
    // is not critical to the booting of the operating system, it should not be
    // a boot start driver.
    //

    Status = BlGetApplicationOptionInteger(
                 BCDE_OSLOADER_TYPE_DRIVER_LOAD_FAILURE_POLICY,
                 &Integer);

    if (NT_SUCCESS(Status)) {
        LoadFailurePolicy = Integer;

    } else {
        LoadFailurePolicy = DriverLoadFailurePolicyFatal;
    }

    //
    // There are a few third party applications that install boot start
    // drivers and later forget to unregister them during the uninstall.
    // These must be considered rogue drivers and shouldn't prevent users from
    // booting successfully.  The down side of this ignore is that such
    // drivers may never get fixed.  In this case preserve the strictness on
    // checked builds and fail according to the policy above if a boot start
    // driver is missing.
    //
    // N.B. Always attempt safe boot on checked builds, even if boot start
    //      drivers are missing.
    //

#if DBG

    Status = BlGetApplicationOptionInteger(BCDE_OSLOADER_TYPE_SAFEBOOT,
                                           &SafemodeType);

    if (NT_SUCCESS(Status)) {
        IgnoreMissingBootStartDrivers = TRUE;

    } else {
        IgnoreMissingBootStartDrivers = FALSE;
    }

#else

    IgnoreMissingBootStartDrivers = TRUE;

#endif

    ReturnStatus = STATUS_SUCCESS;
    Status = STATUS_SUCCESS;

    StartTime = BlTimeQueryPerformanceCounter(NULL);

    //
    // The file path of driver entries (specified from the service key in the
    // system hive) can have one of three formats.
    //
    // - Relative file path from the OS path.
    //   (ie. "System32\Drivers\driver.sys").
    //
    // - Full file path using SystemRoot notation.
    //   (ie. "\SystemRoot\System32\Drivers\driver.sys").
    //
    // - Full device and file path using arc syntax.
    //   (ie. \ArcDeviceName\dir\subdir\filename").
    //
    // In the case of the first two, they both rely on the OS Device and
    // OS Filepath.  The last format ignores these.
    //

    //
    // Traverse the driver list, loading each driver (and it's imports) into
    // memory.
    //

    RtlInitUnicodeString(&SubDirectory, NULL);
    NextEntry = DriverList->Flink;
    while (NextEntry != DriverList) {
        DriverNode = CONTAINING_RECORD(NextEntry,
                                       BOOT_DRIVER_NODE,
                                       ListEntry.Link);

        DriverEntry = &DriverNode->ListEntry;

        //
        // Construct the full path for the driver.
        //
        // If the driver has one of the first two formats described above
        // (relative driver path or system root notation), obtain a pointer
        // to the start of the driver path, starting after the OS path.
        //

        DriverPath = DriverEntry->FilePath.Buffer;
        if (memcmp(DriverPath, L"\\SystemRoot\\",
                   sizeof(L"\\SystemRoot\\") - sizeof(WCHAR)) == 0) {

            DriverPath = Add2Ptr(DriverPath,
                                 sizeof(L"\\SystemRoot\\") - sizeof(WCHAR));
        }

        //
        // Fork code based on the two driver path formats.  The first case
        // is a relative path starting at the OS path.  The second is a full
        // arc device and file path.
        //

        DriverEntry->LoadInformation = 0;
        if (DriverPath[0] != L'\\') {

            //
            // If we're doing a last known good boot, we make up to 3
            // attempts to load the file from the different directories
            // that the boot file may be located in.  If we successfully
            // load the file, then we're done.
            //
            // N.B. This order is explicit.
            //

            for (Pass = 0; Pass < 3; Pass += 1) {
                LoadInformation = 0;
                StringAllocated = FALSE;
                if (LastKnownGoodBoot == FALSE) {

                    //
                    // Only perform the default handling if LKG is off.
                    //

                    Pass = 2;
                    SubDirectoryPath = DefaultPath;

                } else {
                    if ((Pass == 0) || (Pass == 1)) {

                        //
                        // Build up the last known good string.  Use the
                        // handy append API since it will allocate the correct
                        // amount of space.
                        //

                        RtlInitUnicodeString(&SubDirectory, NULL);
                        if (BlAppendUnicodeToString(
                                            &SubDirectory,
                                            DefaultPath->Buffer) == FALSE) {

                            Status = STATUS_INSUFFICIENT_RESOURCES;
                            goto NextPass;
                        }

                        StringAllocated = TRUE;
                        if (Pass == 0) {
                            if (BlAppendUnicodeToString(
                                            &SubDirectory,
                                            L"LastGood.Tmp\\") == FALSE) {

                                Status = STATUS_INSUFFICIENT_RESOURCES;
                                goto NextPass;
                            }

                        } else {
                            if (BlAppendUnicodeToString(
                                            &SubDirectory,
                                            L"LastGood\\") == FALSE) {

                                Status = STATUS_INSUFFICIENT_RESOURCES;
                                goto NextPass;
                            }
                        }

                        SubDirectoryPath = &SubDirectory;

                    } else {
                        SubDirectoryPath = DefaultPath;
                    }
                }

                //
                // Attempt to load the driver and all of its imports.
                //

                OslImageFlags = Flags | OSL_IMAGE_FLAGS_LOAD_IMPORTS;
                Status = LoadImageEx(SubDirectoryPath,
                                     DriverPath,
                                     &DriverEntry->FilePath,
                                     DefaultDevice,
                                     OSL_MEMORY_TYPE_BOOT_DRIVER,
                                     LoadedDriverList,
                                     (PKLDR_DATA_TABLE_ENTRY *)
                                     &DriverEntry->LdrEntry,
                                     0,
                                     0,
                                     IMAGE_FLAGS_NONE,
                                     &LoadInformation,
                                     OslImageFlags);

                //
                // Make sure to free any buffer if allocated in the prior pass.
                //

NextPass:
                if (StringAllocated != FALSE) {
                    if (SubDirectory.Buffer != NULL) {
                        BlMmFreeHeap(SubDirectory.Buffer);
                    }
                }

                //
                // Merge any load information collected during this load
                // attempt into the boot driver list.
                //

                DriverEntry->LoadInformation |= LoadInformation;

                //
                // Proceed to the next driver if the image has been loaded
                // successfully.
                //

                if (NT_SUCCESS(Status)) {
                    break;
                }
            }

            //
            // Record the load status in the boot driver list.  The kernel
            // will ignore all entries marked with a failure status.  Boot
            // driver list entries marked with a failure status are left in
            // the list to improve the kernel's ability to diagnose and
            // characterize boot driver load failures.
            //
            // Treat the failure as fatal if specified by either the driver
            // load policy or the driver's error control.
            //

            DriverEntry->LoadStatus = Status;
            if (!NT_SUCCESS(Status)) {
                DriverEntry->LdrEntry = NULL;
                if ((DriverNode->ErrorControl == CriticalError) ||
                    ((LoadFailurePolicy == DriverLoadFailurePolicyFatal) &&
                     ((Status != STATUS_NO_SUCH_FILE) ||
                      (IgnoreMissingBootStartDrivers == FALSE)))) {

                    OslFatalError(OSL_ERROR_CRITICAL_DRIVER,
                                  (ULONG_PTR)DefaultPath->Buffer,
                                  (ULONG_PTR)DriverPath,
                                  Status);

                    ReturnStatus = Status;
                    break;
                }

                BlStatusPrint(L"\nIgnoring failure to load driver %s%s\n"
                              L"error control %d, status %x.\n",
                              DefaultPath->Buffer,
                              DriverPath,
                              DriverNode->ErrorControl,
                              Status);
            }

        } else {

            //
            // todo: Need to do the ARC syntax boot driver entries.
            //

            ASSERT(FALSE);  // Assert to see if there are any instances of this.

            //
            // Apart from the ARC case, control can reach this point if the
            // driver path read from the registry is malformed such that a
            // leading backslash remains even after any substitutions
            // performed above.  On free builds (and on continuation from the
            // preceding assertion), mark this boot driver list entry to
            // reflect its malformed driver path.
            //

            DriverEntry->LdrEntry = NULL;
            DriverEntry->LoadStatus = STATUS_INVALID_PLUGPLAY_DEVICE_PATH;
            DriverEntry->LoadInformation =
                LOAD_INFORMATION_MALFORMED_DRIVER_PATH;
        }

        //
        // Advance to the next boot driver.
        //

        NextEntry = NextEntry->Flink;
    }

    EndTime = BlTimeQueryPerformanceCounter(&Frequency);
    FrequencyInMilliseconds = Frequency.QuadPart / 1000;

    TimeTaken = (EndTime.QuadPart - StartTime.QuadPart) /
                (FrequencyInMilliseconds);

    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)&TimeTaken,
                        sizeof(ULONGLONG));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_LOAD_DRIVERS_TIME,
                  NULL,
                  1,
                  EventDataDescriptor);

    return ReturnStatus;
}

NTSTATUS
OslLoadImage(
    _In_ DEVICE_ID DeviceId,
    _In_ MEMORY_TYPE MemoryType,
    _In_ PWSTR ImagePath,
    _In_opt_ PWSTR FriendlyName,
    _In_opt_ PUNICODE_STRING ServiceFilePath,
    _In_opt_ PVOID PreAllocatedImageBase,
    _In_ ULONG PreAllocatedImageSize,
    _In_ PLIST_ENTRY LoadedImageList,
    _Outptr_opt_ PKLDR_DATA_TABLE_ENTRY *DataTableEntry,
    _In_ ULONG KldrFlags,
    _In_ ULONG BldrFlags,
    _In_ ULONG ImageFlags,
    _In_ ULONG OslLoadImageFlags,
    _Out_opt_ PULONG LoadInformation
    )

/*++

Routine Description:

    This routine loads a PE image into memory.

Arguments:

    DeviceId - Supplies a handle to the device containing the image to load.

    MemoryType - Supplies the type of memory to mark the image allocation.

    ImagePath - Supplies the file path of the image to load.

    FriendlyName - Supplies an alternate name for image.  If specified, the
        data table entry's BaseDllName equals FriendlyName.  This is needed by
        the kernel since it can have any one of a few actual names.

    ServiceFilePath - Supplies an optional pointer to the file path for a
        boot driver, as described in its service registry entry.

    PreAllocatedImageBase - Supplies an optional pointer to the pre-allocated
        image buffer. This is only used when IMAGE_FLAGS_MEMORY_PRE_ALLOCATED
        is set in ImageFlags.

    PreAllocatedImageSize - Specifies the size of the pre-allocated image
        buffer. This is only used when IMAGE_FLAGS_MEMORY_PRE_ALLOCATED
        is set in ImageFlags.

    LoadedDriverList - Supplies a list of images currently loaded by the OS
        loader.

    DataTableEntry - Supplies a pointer to a variable that receives an
        allocated data table entry describing the loaded module.  The caller
        is responsible for freeing this memory.

    KldrFlags - Supplies the kernel loader flags to use in the allocated data
        table entry.

    BldrFlags - Supplies the boot loader flags to use in the allocated data
        table entry.

    ImageFlags - Supplies image flags indicating any preferences for the image
        load.  Possible values include:

        IMAGE_FLAGS_MEMORY_PRE_ALLOCATED : Indicates that memory has already
            been pre-allocated for the image and that PreAllocatedImageBase
            is the pointer to the pre-allocated memory address and
            PreAllocatedImageSize is its size.

        IMAGE_FLAGS_ALLOCATE_IN_LARGE_PAGE : Specifies that an image should
            be allocated in an exclusive system component large page.

    OslLoadImageFlags - Supplies image flags specifying functionality of this
        routine.  Possible values include:

        OSL_IMAGE_FLAGS_LOAD_IMPORTS : Specifies that the image's imports
            should be loaded.  LoadedImageList is ordered by dependency.  If
            driver A contains imports from DLL B, then B is inserted before A
            in the list.

        OSL_IMAGE_FLAGS_EARLY_LAUNCH  : Specifies that the image is an anti-
            malware driver and is subject to a higher bar of integrity policy.

        OSL_IMAGE_FLAGS_CORE_DRIVER : Specifies that the image is an image to
            be loaded before the anti-malware driver and is subject to a higher
            bar of integrity policy.

        OSL_IMAGE_FLAGS_APISET : Specifies that the image was originally
            referred to as an API set (or API set extension), thus it is an
            OS internal component and is subject to a higher bar of
            integrity policy (e.g., must be MS-signed).

        OSL_IMAGE_FLAGS_ELAM_IMPORT : Specifes that the image is an image to be
            initialized before the anti-malware driver because the anti-malware
            driver imports from it.

        OSL_IMAGE_FLAGS_HAL_EXTENSION : Specifies that the image is a HAL
            extension and is subject to a higher bar of integrity policy.

        OSL_IMAGE_FLAGS_IUM : Specifies that the image runs in the Isolated
            User Mode environment.

        OSL_IMAGE_FLAGS_WINDOWS : Specifies that the image is subject to
            Windows integrity policy.

        OSL_IMAGE_FLAGS_CORE_EXTENSION : Specifies that the image is to
            be loaded before the TpmCore drivers and is subject to a higher
            bar of integrity policy.
            N.B. By default, Core Extension images are only required to be
                 WHQL-signed. If an image requires a higher integrity policy,
                 then an appropriate flag that maps to the required integrity
                 level also needs to be specified. For instance, if an image
                 has the same bar as HAL extensions, then
                 OSL_IMAGE_FLAGS_HAL_EXTENSION also needs to be specified.

    LoadInformation - Supplies an optional pointer to a variable that receives
            informational flags describing various aspects of the load.  The
            following flags may be directly set by this routine.

        LOAD_INFORMATION_IMPORT_LOAD_FAILED : Indicates that the PE image
            could not be loaded because an error occurred loading one of its
            imports.

        OSL_IMAGE_FLAGS_POST_LOAD_IMPORTS : Specifies that the image's imports
            should be loaded.  The specified image is expected to have already
            been loaded.

        LOAD_INFORMATION_EMBEDDED_SIGNED : Indicates that the image is verified
            using embedded signature.

        LOAD_INFORMATION_CATALOG_SIGNED : Indicates taht the image is verified
            using a signature from a catalog.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if LoadFile or ImageBase are NULL.

    STATUS_NO_MEMORY if there is insufficient memory to perform the request.

    STATUS_UNSUCCESSFUL if a filesystem could not be mounted on block i/o
        device.

    STATUS_UNSUCCESSFUL if unable to apply relocation.

    STATUS_INVALID_IMAGE_FORMAT if relocations are invalid.

    STATUS_NO_SUCH_FILE if the requested file does not exist on the specified
        block i/o device.

    STATUS_FILE_INVALID if the specified file is not a PE image.

--*/

{

    PBLDR_DATA_TABLE_ENTRY BootImageEntry;
    PWCHAR FileName;
    PVOID ImageBase;
    PUCHAR ImageHash;
    ULONG ImageHashAlgorithm;
    UCHAR ImageHashBuffer[MAX_HASH_LEN];
    ULONG ImageHashLength;
    ULONG ImageSize;
    PKLDR_DATA_TABLE_ENTRY LoadedImageEntry;
    ULONG LocalLoadInformation;
    ULONG PreferredAttributes;
    PVOID RemoteFileBase;
    ULONG64 RemoteFileSize;
    NTSTATUS Status;
    PWCHAR SymbolicName;
    BOOT_TRUSTED_BOOT_INFORMATION TrustedBootInformation;

    ASSERT(ARGUMENT_PRESENT(ImagePath) != FALSE);

    SymbolicName = NULL;

    //
    // If IMAGE_FLAGS_MEMORY_PRE_ALLOCATED flag is set, make sure
    // PreAllocatedImageBase and PreAllocatedImageSize are valid.
    //

    if (CHECK_FLAG(ImageFlags, IMAGE_FLAGS_MEMORY_PRE_ALLOCATED)) {
        if ((PreAllocatedImageBase == NULL) || (PreAllocatedImageSize == 0)) {
            return STATUS_INVALID_PARAMETER;
        }
    }

    ImageFlags |= IMAGE_FLAGS_VALIDATE_MACHINE_TYPE;

    //
    // Check to see if the image has already been loaded.  If it has not
    // already been loaded, load it.  If the image has already been loaded,
    // only when specified, load its imports.  This routine allows for the
    // delayed import loading for such modules as the kernel and hal.
    //

    if (FriendlyName != NULL) {
        FileName = FriendlyName;

    } else {
        FileName = wcsrchr(ImagePath, L'\\') + 1;

        ASSERT(FileName != NULL);

    }

    //
    // The empty string is not a valid filename.
    //

    if (FileName[0] == UNICODE_NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Determine if the image has already been loaded. If it has been loaded,
    // process its post-load imports, if necessary. If it has not been loaded
    // and it is a dependent DLL of the ELAM driver, it must be on the list of
    // allowed drivers. If it is, then mark it as an ELAM import for validation
    // and continue. Additionally, do not allow ELAM imports to load new
    // imports; they must come from already loaded images. Otherwise in the
    // non-ELAM case, if it has not been loaded, load the image and process its
    // imports.
    //

    LocalLoadInformation = 0;
    LoadedImageEntry = BlLdrFindDataTableEntry(LoadedImageList, FileName, TRUE);
    if (LoadedImageEntry == NULL) {
        PreferredAttributes = MEMORY_ATTRIBUTE_ALLOCATION_KSEG;
        if (((OslLoadImageFlags & OSL_IMAGE_FLAGS_EARLY_LAUNCH) != 0) &&
            ((KldrFlags & LDRP_DRIVER_DEPENDENT_DLL) != 0)) {

            if (OslpAllowedEarlyLaunchImport(FileName) == FALSE) {
                return STATUS_UNSUCCESSFUL;

            } else {
                CLEAR_FLAGS(OslLoadImageFlags, OSL_IMAGE_FLAGS_EARLY_LAUNCH);
                CLEAR_FLAGS(ImageFlags, IMAGE_FLAGS_EARLY_LAUNCH);
                SET_FLAGS(OslLoadImageFlags, OSL_IMAGE_FLAGS_ELAM_IMPORT);

                //
                // Mark ELAM import drivers as core group dependencies so they
                // will get initialized before the ELAM driver.
                //

                SET_FLAGS(BldrFlags, BLDR_FLAGS_CORE_DRIVER_DEPENDENT_DLL);
            }

        } else if (((OslLoadImageFlags & OSL_IMAGE_FLAGS_ELAM_IMPORT) != 0) &&
                   ((KldrFlags & LDRP_DRIVER_DEPENDENT_DLL) != 0)) {

            return STATUS_UNSUCCESSFUL;
        }

        //
        // Mark Core Extension import drivers appropriately so that they get
        // initialized prior to the drivers dependent on them.
        //

        if (((OslLoadImageFlags & OSL_IMAGE_FLAGS_CORE_EXTENSION) != 0) &&
            ((KldrFlags & LDRP_DRIVER_DEPENDENT_DLL) != 0)) {

            SET_FLAGS(BldrFlags, BLDR_FLAGS_CORE_EXTENSION_DEPENDENT_DLL);
        }

        //
        // Add any image validation flags applied to this image by the current
        // image validation policy.
        //
        // Whether an image should be validated is always determined at this
        // point, so any lingering validation policy flags supplied by the
        // caller must be cleared.
        //

        if (CHECK_FLAG(OslLoadImageFlags,
                       OSL_IMAGE_FLAGS_WINDOWS) != FALSE) {

            if (LoadIntegrityCheckPolicy == IntegrityCheckDisabled) {
                SET_FLAGS(ImageFlags, IMAGE_FLAGS_VALIDATE_IMAGE_HASH);

            } else {
                SET_FLAGS(ImageFlags, IMAGE_FLAGS_HASH_VALIDATION_FLAGS);
                SET_FLAGS(ImageFlags, IMAGE_FLAGS_WINDOWS_COMPONENT);
            }

        } else if (CHECK_FLAG(OslLoadImageFlags,
                              OSL_IMAGE_FLAGS_EARLY_LAUNCH) != FALSE) {

            if (LoadIntegrityCheckPolicy == IntegrityCheckDisabled) {
                SET_FLAGS(ImageFlags, IMAGE_FLAGS_VALIDATE_IMAGE_HASH);

            } else {
                SET_FLAGS(ImageFlags, IMAGE_FLAGS_HASH_VALIDATION_FLAGS);
                SET_FLAGS(ImageFlags, IMAGE_FLAGS_EARLY_LAUNCH);
            }

        } else if (CHECK_FLAG(OslLoadImageFlags,
                              OSL_IMAGE_FLAGS_CORE_DRIVER) != FALSE) {

            if (LoadIntegrityCheckPolicy == IntegrityCheckDisabled) {
                SET_FLAGS(ImageFlags, IMAGE_FLAGS_VALIDATE_IMAGE_HASH);

            } else {
                SET_FLAGS(ImageFlags, IMAGE_FLAGS_HASH_VALIDATION_FLAGS);

                //
                // If test-signing is disabled, CORE drivers must be Windows'
                // components. This enables KIT signed drivers to be used
                // during Logo testing but not in production.
                //

                if (LoadIntegrityTestSigningEnabled == FALSE) {
                    SET_FLAGS(ImageFlags, IMAGE_FLAGS_WINDOWS_COMPONENT);
                }
            }

        } else if (CHECK_FLAG(OslLoadImageFlags,
                              OSL_IMAGE_FLAGS_ELAM_IMPORT) != FALSE) {

                if (LoadIntegrityCheckPolicy == IntegrityCheckDisabled) {
                    SET_FLAGS(ImageFlags, IMAGE_FLAGS_VALIDATE_IMAGE_HASH);

                } else {
                    SET_FLAGS(ImageFlags, IMAGE_FLAGS_HASH_VALIDATION_FLAGS);
                    SET_FLAGS(ImageFlags, IMAGE_FLAGS_WINDOWS_COMPONENT);
                }

        //
        // For HAL Extensions and Core Extensions that should be validated
        // against the HAL signing certificate, set the appropriate HAL
        // extension validation flags.
        //

        } else if (CHECK_FLAG(OslLoadImageFlags,
                              OSL_IMAGE_FLAGS_HAL_EXTENSION) != FALSE) {

            if (LoadIntegrityCheckPolicy == IntegrityCheckDisabled) {
                SET_FLAGS(ImageFlags, IMAGE_FLAGS_VALIDATE_IMAGE_HASH);

            } else {
                SET_FLAGS(ImageFlags, IMAGE_FLAGS_HASH_VALIDATION_FLAGS);
                SET_FLAGS(ImageFlags, IMAGE_FLAGS_HAL_EXTENSION);
            }

        } else if (CHECK_FLAG(OslLoadImageFlags,
                              OSL_IMAGE_FLAGS_IUM) != FALSE) {

            SET_FLAGS(ImageFlags, IMAGE_FLAGS_HASH_VALIDATION_FLAGS);
            SET_FLAGS(ImageFlags, IMAGE_FLAGS_WINDOWS_COMPONENT);
            SET_FLAGS(OslLoadImageFlags, OSL_IMAGE_FLAGS_LOAD_IMPORTS);

        //
        // For Core Extensions, set the default image validation flags.
        //
        // N.B. Core Extensions that should be validated against the HAL
        //      signing certificate, the image flags should have HAL extension
        //      specified and thus should get HAL validation flags set
        //      above.
        //

        } else if (CHECK_FLAG(OslLoadImageFlags,
                              OSL_IMAGE_FLAGS_CORE_EXTENSION) != FALSE) {

            NT_ASSERT(CHECK_FLAG(OslLoadImageFlags,
                                 OSL_IMAGE_FLAGS_HAL_EXTENSION) == FALSE);

            if (LoadIntegrityCheckPolicy == IntegrityCheckDisabled) {
                SET_FLAGS(ImageFlags, IMAGE_FLAGS_VALIDATE_IMAGE_HASH);

            } else {
                SET_FLAGS(ImageFlags, IMAGE_FLAGS_HASH_VALIDATION_FLAGS);
                SET_FLAGS(ImageFlags, IMAGE_FLAGS_CORE_EXTENSION);
            }

        } else {
            CLEAR_FLAGS(ImageFlags, IMAGE_FLAGS_HASH_VALIDATION_FLAGS);
            SET_FLAGS(ImageFlags,
                      GetImageValidationFlags(FileName, OslLoadImageFlags));
        }

        //
        // For Core drivers, Core Extensions and ELAM imports, verify that the
        // file name matches that in the file's version resource.
        //

        if ((CHECK_FLAG(OslLoadImageFlags,
                        OSL_IMAGE_FLAGS_CORE_DRIVER) != FALSE) ||
            (CHECK_FLAG(OslLoadImageFlags,
                        OSL_IMAGE_FLAGS_CORE_EXTENSION) != FALSE) ||
            (CHECK_FLAG(OslLoadImageFlags,
                        OSL_IMAGE_FLAGS_ELAM_IMPORT) != FALSE)) {

            SET_FLAGS(ImageFlags, IMAGE_FLAGS_VERIFY_FILENAME);
        }

        //
        // If the caller specifies that the image buffer is pre-allocated,
        // set the pre-allocate buffer address and size.
        //

        if (CHECK_FLAG(ImageFlags, IMAGE_FLAGS_MEMORY_PRE_ALLOCATED)) {
            ImageBase = PreAllocatedImageBase;
            ImageSize = PreAllocatedImageSize;

        } else {
            ImageBase = NULL;
            ImageSize = 0;
        }

        //
        // Load the specified image into memory.  First check to see if the
        // file should be loaded over the debugger connection.  If not, load
        // the file from the specified device and file location.
        //

        Status = STATUS_UNSUCCESSFUL;
        ImageHash = ImageHashBuffer;
        ImageHashAlgorithm = 0;
        ImageHashLength = 0;
        RtlZeroMemory(&TrustedBootInformation,
                      sizeof(BOOT_TRUSTED_BOOT_INFORMATION));

        if (ARGUMENT_PRESENT(ServiceFilePath) != FALSE) {
            Status = BlBdPullRemoteFile(ServiceFilePath->Buffer,
                                        &RemoteFileBase,
                                        &RemoteFileSize);

            if (NT_SUCCESS(Status) != FALSE) {
                Status = BlImgLoadPEImageFromSourceBuffer(
                             RemoteFileBase,
                             RemoteFileSize,
                             MemoryType,
                             ImagePath,
                             &ImageBase,
                             &ImageSize,
                             ImageHash,
                             &ImageHashLength,
                             &ImageHashAlgorithm,
                             PreferredAttributes,
                             0,
                             ImageFlags,
                             &LocalLoadInformation,
                             &TrustedBootInformation);

                BlMmFreePages(RemoteFileBase);
            }
        }

        if (!NT_SUCCESS(Status)) {
            Status = BlImgLoadPEImageEx(DeviceId,
                                        MemoryType,
                                        ImagePath,
                                        &ImageBase,
                                        &ImageSize,
                                        ImageHash,
                                        &ImageHashLength,
                                        &ImageHashAlgorithm,
                                        PreferredAttributes,
                                        0,
                                        ImageFlags,
                                        &LocalLoadInformation,
                                        &TrustedBootInformation);

            if (!NT_SUCCESS(Status)) {
                goto OslLoadImageEnd;
            }
        }

        //
        // Create a string to represent the full (symbolic) DLL name.
        //

        SymbolicName = OslpConvertToSymbolicPath(ImagePath);

        //
        // Allocate and insert a data table entry for the loaded image.
        //

        Status = BlLdrAllocateDataTableEntry(ImagePath,
                                             SymbolicName,
                                             FriendlyName,
                                             ImageBase,
                                             ImageSize,
                                             ImageHash,
                                             ImageHashLength,
                                             ImageHashAlgorithm,
                                             KldrFlags,
                                             BldrFlags,
                                             LOADER_DTE_FLAGS_PE_ENTRY,
                                             &LocalLoadInformation,
                                             &TrustedBootInformation,
                                             &LoadedImageEntry);

        if (!NT_SUCCESS(Status)) {
            goto OslLoadImageEnd;
        }

        //
        // Process load configuration for the module.
        //

        OslpProcessLoadConfigDirectoryForModule(LoadedImageEntry);

        //
        // Insert the newly allocated data table entry into the loaded list.
        // N.B. Even when loading imports, we need to insert the entry before
        //      attempting to load any imports.  Otherwise, we could fall
        //      into a recursive loop when an import attempts to import this
        //      DLL/driver.  If this happns, we need to have an entry
        //      present in the loop to avoid this image from being re-loaded.
        //

        InsertTailList(LoadedImageList, &LoadedImageEntry->InLoadOrderLinks);

        //
        // When specified by the caller, load all modules listed in the import
        // descriptor table.
        //

        if (CHECK_FLAG(OslLoadImageFlags, OSL_IMAGE_FLAGS_LOAD_IMPORTS)) {
            Status = LoadImports(DeviceId,
                                 MemoryType,
                                 LoadedImageEntry,
                                 LoadedImageList,
                                 ImageFlags,
                                 OslLoadImageFlags);

            if (!NT_SUCCESS(Status)) {
                RemoveEntryList(&LoadedImageEntry->InLoadOrderLinks);
                BlLdrFreeDataTableEntry(LoadedImageEntry);
                SET_FLAGS(LocalLoadInformation,
                          LOAD_INFORMATION_IMPORT_LOAD_FAILED);

                goto OslLoadImageEnd;
            }

            //
            // If this is an imported DLL, insert the data table entry
            // after all imports.
            //

            if (CHECK_FLAG(KldrFlags, LDRP_DRIVER_DEPENDENT_DLL)) {
                RemoveEntryList(&LoadedImageEntry->InLoadOrderLinks);
                InsertTailList(LoadedImageList,
                               &LoadedImageEntry->InLoadOrderLinks);
            }
        }

        //
        // Mark the image as processed.
        //

        LoadedImageEntry->Flags |= LDRP_ENTRY_PROCESSED;

    } else {
        if (CHECK_FLAG(OslLoadImageFlags, OSL_IMAGE_FLAGS_POST_LOAD_IMPORTS)) {
            CLEAR_FLAGS(OslLoadImageFlags, OSL_IMAGE_FLAGS_POST_LOAD_IMPORTS);
            Status = LoadImports(DeviceId,
                                 MemoryType,
                                 LoadedImageEntry,
                                 LoadedImageList,
                                 ImageFlags,
                                 OslLoadImageFlags);

            if (!NT_SUCCESS(Status)) {
                goto OslLoadImageEnd;
            }
        }

        //
        // A boot driver does not get the dependent dll flag set.  If this
        // routine was not explicitly called with the flag set, then clear the
        // flag if previously set.  When the routine is called without the flag
        // set, it is being called on a boot driver.
        //

        if (((KldrFlags & LoadedImageEntry->Flags) &
              LDRP_DRIVER_DEPENDENT_DLL) == 0) {

            CLEAR_FLAGS(KldrFlags, LDRP_DRIVER_DEPENDENT_DLL);
            CLEAR_FLAGS(LoadedImageEntry->Flags, LDRP_DRIVER_DEPENDENT_DLL);
        }

        BootImageEntry = (PBLDR_DATA_TABLE_ENTRY)LoadedImageEntry;

#ifdef DBG

        //
        // Check to see if this binary is being initialized as a boot driver
        // but was already marked as a core driver dependent dll. Such binaries
        // need to be in the core driver group. If they are already in the core
        // group, then they are in the wrong order. Break into the debugger.
        //
        // N.B. KldrFlags needs to be checked here as well. If a non core
        //      driver imports the same DLL as a core driver, the
        //      BLDR_FLAGS_CORE_DRIVER_DEPENDENT_DLL needs to remain so it is
        //      initialized early.
        //

        if (((KldrFlags & LDRP_DRIVER_DEPENDENT_DLL) == 0) &&
            ((BldrFlags & BLDR_FLAGS_CORE_DRIVER_DEPENDENT_DLL) == 0) &&
            ((BootImageEntry->Flags &
              BLDR_FLAGS_CORE_DRIVER_DEPENDENT_DLL) != 0)) {

            DbgBreakPoint();
        }

        //
        // Check to see if this binary is being initialized as a Core driver
        // dependent dll. If so, then it must not have already been marked as
        // a Core Extension dependent DLL. Otherwise it implies some Core
        // drivers were processed after Core Extensions.
        //

        if (((KldrFlags & LDRP_DRIVER_DEPENDENT_DLL) != 0) &&
            ((BldrFlags & BLDR_FLAGS_CORE_DRIVER_DEPENDENT_DLL) != 0) &&
            ((BootImageEntry->Flags &
              BLDR_FLAGS_CORE_EXTENSION_DEPENDENT_DLL) != 0)) {

            DbgBreakPoint();
        }

#endif

        //
        // Check to see if this binary is being initialized as a core driver
        // dependent dll, but was already initialized as a core boot driver. If
        // so, clear the core driver dependent dll flags. This is necessary for
        // the case where one core driver has a dependency on another that has
        // loaded before it.
        //

        if (((BldrFlags & BLDR_FLAGS_CORE_DRIVER_DEPENDENT_DLL) != 0) &&
            ((BootImageEntry->Flags &
              BLDR_FLAGS_CORE_DRIVER_DEPENDENT_DLL) == 0)) {

            CLEAR_FLAGS(BldrFlags, BLDR_FLAGS_CORE_DRIVER_DEPENDENT_DLL);
        }

        //
        // Check to see if this binary is being initialized as a Core Extension
        // dependent dll but was already initialized as a Core driver dependent
        // DLL (which includes ELAM imports). If so, do not set the Core
        // Extension dependent DLL flags.
        //

        if (((BldrFlags & BLDR_FLAGS_CORE_EXTENSION_DEPENDENT_DLL) != 0) &&
            ((BootImageEntry->Flags &
                BLDR_FLAGS_CORE_DRIVER_DEPENDENT_DLL) != 0)) {

            CLEAR_FLAGS(BldrFlags, BLDR_FLAGS_CORE_EXTENSION_DEPENDENT_DLL);
        }

        //
        // A binary can be marked as either Core driver dependent dll or Core
        // Extension dependent dll but not both.
        //

        NT_ASSERT(((BldrFlags & BLDR_FLAGS_CORE_EXTENSION_DEPENDENT_DLL) == 0)
                      ||
                  ((BldrFlags & BLDR_FLAGS_CORE_DRIVER_DEPENDENT_DLL) == 0));

        //
        // Update the reference count for the binary and if the image is a
        // driver that was previously imported, clear the DLL flag.
        //

        BootImageEntry->Flags |= BldrFlags;
        LoadedImageEntry->Flags |= KldrFlags;
        LoadedImageEntry->LoadCount += 1;
    }

    //
    // Return the pointer to the loaded data table entry.
    //

    if (DataTableEntry != NULL) {
        *DataTableEntry = LoadedImageEntry;
    }

    Status = STATUS_SUCCESS;

OslLoadImageEnd:
    if (SymbolicName != NULL) {
        BlMmFreeHeap(SymbolicName);
    }

    if (ARGUMENT_PRESENT(LoadInformation)) {
        *LoadInformation = LocalLoadInformation;
    }

    return Status;
}

NTSTATUS
OslGetBootDrivers (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in ULONG SystemHiveHandle,
    __in_opt PWSTR BootFileSystem,
    __in_opt PULONG Flags
    )

/*++

Routine Description:

    This routine initializes each boot driver list in the loader block with a
    list of drivers to load.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

    SystemHiveHandle - Supplies the handle to the system hive.

    BootFileSystem - Supplies an optional pointer to the base name of the
        file system on the OS device.

    Flags - Supplies optional driver flags used to promote drivers to boot
        start based on the current boot environment.

Return Value:

    NT Status code.

--*/

{

    PBAPD_ITEM Data;
    BAPD_ITEM_DESCRIPTOR DataDescriptor;
    BOOLEAN DataDescriptorRead;
    GUID FwUpdateResultsGuid = FWUPDATE_RESULTS_GUID;
    PFWUPDATE_RESULTS_HEADER ResultsHeader;
    NTSTATUS Status;

    DataDescriptorRead = FALSE;
    ResultsHeader = NULL;

    //
    // Get firmware update results, if present, to use when resolving dependencies.
    //

    Status = BlpPdQueryData(&DataDescriptor, NULL, &FwUpdateResultsGuid);

    if (NT_SUCCESS(Status)) {
        DataDescriptorRead = TRUE;

        Data = (PBAPD_ITEM)(UINTN)DataDescriptor.u.PhysAddr.QuadPart;

        if (Data->DataSize >= sizeof(FWUPDATE_RESULTS_HEADER)) {
            ResultsHeader = (PFWUPDATE_RESULTS_HEADER)Add2Ptr(Data, Data->DataOffset);

            if (Data->DataSize < (sizeof(FWUPDATE_RESULTS_HEADER) +
                                  ((ResultsHeader->Results - 1) * sizeof(FIRMWARE_UPDATE_INFORMATION)))) {
                ResultsHeader = NULL;
            }
        }
    }

    //
    // If firmware update results were not found, or there was an error
    // retrieving them, that is ok.
    //

    Status = STATUS_SUCCESS;

    //
    // Read the registry, obtaining the list of all boot start drivers.
    //

    Status = OslHiveFindDrivers(
                 SystemHiveHandle,
                 BootLoad,
                 BootFileSystem,
                 Flags,
                 &LoaderBlock->Extension->HardwareConfigurationId,
                 &LoaderBlock->Extension->ManufacturingProfile,
                 ResultsHeader,
                 &LoaderBlock->BootDriverListHead);

    if (!NT_SUCCESS(Status)) {
        goto OslGetBootDriversEnd;
    }

    //
    // Walk the boot driver list and filter out core drivers, (3rd party) core
    // extensions, and ELAM drivers into their respective lists.
    //

    OslpFilterDriverList(LoaderBlock);

OslGetBootDriversEnd:

    if (DataDescriptorRead != FALSE) {
        BlpPdReleaseData(&DataDescriptor);
    }

    return Status;
}

VOID
OslInitializeCoreExtensionSubGroups (
    VOID
    )

/*++

Routine Description:

    This routine will initialize the Core Extension sub-group information.

Arguments:

    None.

Return Value:

    None.

--*/

{

    ULONG Index;

    for (Index = 0; Index < ARRAYSIZE(OslCoreExtensionSubGroups); Index += 1) {
        InitializeListHead(&OslCoreExtensionSubGroups[Index].ListHead);
    }

    //
    // Initialize the Core Extension sub-group information.
    //
    // N.B. The drivers will be initialized by the OS in the order they appear
    //      in this list.
    //

    //
    // The first sub-group to be initialized is Core Platform Extensions.
    //
    // The Core Extension image flag needs to be set to ensure that the imports
    // are marked appropriately.
    //

    OslCoreExtensionSubGroups[0].LoadOrderGroups =
        OslCorePlatformExtensionLoadOrderGroups;

    OslCoreExtensionSubGroups[0].TrustValidationFlags =
        OSL_IMAGE_FLAGS_CORE_EXTENSION | OSL_IMAGE_FLAGS_NONE;

    //
    // The second sub-group to be initialized is Core Security Extensions.
    //
    // Core Security Extensions are required to be signed with the HAL
    // certificate and thus the HAL flag needs to be set. The Core Extension
    // image flag needs to be set to ensure that the imports are marked
    // appropriately.
    //

    OslCoreExtensionSubGroups[1].LoadOrderGroups =
        OslCoreSecurityExtensionLoadOrderGroups;

    OslCoreExtensionSubGroups[1].TrustValidationFlags =
        OSL_IMAGE_FLAGS_CORE_EXTENSION | OSL_IMAGE_FLAGS_HAL_EXTENSION;

    return;
}

// ------------------------------------------------------------ Local Functions

NTSTATUS
InitializeDriversPath (
    __inout PUNICODE_STRING DriversPath
    )

/*++

Routine Description:

    This routine initializes a unicode string with the system32\drivers
    directory according to the current system root.

    N.B. If the string is already initialized, this routine does nothing.

Arguments:

    DriversPath - Supplies the string to be initialized with the drivers path.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if there is insufficient memory to perform the request.

--*/

{
    if (DriversPath->Buffer != NULL) {
        return STATUS_SUCCESS;
    }

    DriversPath->MaximumLength = (USHORT)(wcslen(OslSystemRoot) * sizeof(WCHAR)) +
        sizeof(L"System32\\drivers\\");

    DriversPath->Buffer = BlMmAllocateHeap(DriversPath->MaximumLength);
    if (DriversPath->Buffer == NULL) {
        return STATUS_NO_MEMORY;
    }

    swprintf_s(DriversPath->Buffer,
               DriversPath->MaximumLength / sizeof(WCHAR),
               L"%sSystem32\\drivers\\",
               OslSystemRoot);

    DriversPath->Length = DriversPath->MaximumLength - sizeof(WCHAR);

    return STATUS_SUCCESS;
}

NTSTATUS
LoadImports (
    __in DEVICE_ID DeviceId,
    __in MEMORY_TYPE MemoryType,
    __in PKLDR_DATA_TABLE_ENTRY DataTableEntry,
    __in PLIST_ENTRY LoadedImageList,
    __in ULONG ImageFlags,
    __in ULONG OslLoadImageFlags
    )

/*++

Routine Description:

    Loads each module specified in the import table of the provided image.

Arguments:

    DeviceId - The device on which the the loaded image is located.

    MemoryType - The type of memory to mark allocations made for the imported
        modules.

    DataTableEntry - The data table entry for the image whose imports should
        be loaded.

    LoadedImageList - The list of images already loaded.

    ImageFlags - Special image flags indicating any preferences for the
        image load.  Possible values include:

        IMAGE_FLAGS_ALLOCATE_IN_LARGE_PAGE : Specifies that an image should
            be allocated in an exclusive system component large page.

    OslLoadImageFlags - Supplies image flags specifying functionality of this
        routine.  Possible values include:

        OSL_IMAGE_FLAGS_LOAD_IMPORTS : Specifies that the image's imports
            should be loaded.  LoadedImageList is ordered by dependency.  If
            driver A contains imports from DLL B, then B is inserted before A
            in the list.

        OSL_IMAGE_FLAGS_EARLY_LAUNCH  : Specifies that the image is an anti-
            malware driver and is subject to a higher bar of integrity policy.

        OSL_IMAGE_FLAGS_CORE_DRIVER : Specifies that the image is an image to
            be loaded before the anti-malware driver and is subject to a higher
            bar of integrity policy.

        OSL_IMAGE_FLAGS_ELAM_IMPORT : Specifes that the image is an image to be
            initialized before the anti-malware driver because the anti-malware
            driver imports from it.

        OSL_IMAGE_FLAGS_IUM : Specifies that the image runs in the Isolated
            User Mode environment.

        OSL_IMAGE_FLAGS_WINDOWS : Specifies that the image is subject to
            Windows integrity policy.

        OSL_IMAGE_FLAGS_CORE_EXTENSION : Specifies that the image is to
            be loaded before the TPM Core drivers and is subject to a higher
            bar of integrity policy.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if there is insufficient memory to perform the request.
    STATUS_UNSUCCESSFUL if unable to apply relocations to an imported DLL.
    STATUS_INVALID_IMAGE_FORMAT if relocations are invalid.
    STATUS_NO_SUCH_FILE if the requested file does not exist on the specified
        block i/o device.
    STATUS_FILE_INVALID if the specified file is not a PE image.

--*/

{

    ULONG BldrFlags;
    UNICODE_STRING DirectoryPath;
    PWCHAR FullPath;
    UNICODE_STRING DriversPath;
    UNICODE_STRING ImageName;
    PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor;
    PCHAR ImportName;
    ULONG ImportTableSize;
    PKLDR_DATA_TABLE_ENTRY ImportedImageDataTableEntry;
    NTSTATUS Status;
    PWCHAR TempString;
    BOOLEAN ApiSetResolved;
    UNICODE_STRING ApiSetHost;
    PVOID ApiSetSchema;
    PUNICODE_STRING LoadPath;
    const UNICODE_STRING ExtensionPrefix = RTL_CONSTANT_STRING(L"ext-");
    const UNICODE_STRING SecureKernel = RTL_CONSTANT_STRING(SECUREKERNEL_NAME);

    FullPath = NULL;
    Status = STATUS_SUCCESS;
    RtlInitUnicodeString(&DirectoryPath, NULL);
    RtlInitUnicodeString(&DriversPath, NULL);
    RtlInitUnicodeString(&ImageName, NULL);

    //
    // Locate the import table in the loaded image.  If NULL, then the binary
    // does not have any dependencies.
    //

    ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)
        RtlImageDirectoryEntryToData(DataTableEntry->DllBase,
                                     TRUE,
                                     IMAGE_DIRECTORY_ENTRY_IMPORT,
                                     &ImportTableSize);

    if (ImportDescriptor == NULL) {
        goto LoadImportsEnd;
    }

    //
    // Create the directory path from the full (symbolic) path name in the
    // data table entry.
    //

    FullPath = OslpConvertFromSymbolicPath(DataTableEntry->FullDllName.Buffer);
    if (FullPath != NULL) {
        TempString = FullPath;

    } else {
        TempString = DataTableEntry->FullDllName.Buffer;
    }

    if (RtlCreateUnicodeString(&DirectoryPath, TempString) == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto LoadImportsEnd;
    }

    TempString  = wcsrchr(DirectoryPath.Buffer, L'\\') + 1;
    *TempString = L'\0';
    DirectoryPath.Length = (USHORT)PtrOffset(DirectoryPath.Buffer, TempString);

    //
    // Recursively load imports.
    //

    OslLoadImageFlags |= OSL_IMAGE_FLAGS_LOAD_IMPORTS;

    //
    // If this is an import for a core driver or an ELAM driver import, mark it
    // as a core driver dependent DLL.
    //

    BldrFlags = 0;
    if ((CHECK_FLAG(OslLoadImageFlags, OSL_IMAGE_FLAGS_CORE_DRIVER) != FALSE) ||
        (CHECK_FLAG(OslLoadImageFlags, OSL_IMAGE_FLAGS_ELAM_IMPORT) != FALSE)) {

        BldrFlags = BLDR_FLAGS_CORE_DRIVER_DEPENDENT_DLL;
    }

    //
    // If this is an import for a Core Extension, then mark it as
    // Core Extension dependent DLL.
    //

    if (CHECK_FLAG(OslLoadImageFlags,
            OSL_IMAGE_FLAGS_CORE_EXTENSION) != FALSE) {

        BldrFlags = BLDR_FLAGS_CORE_EXTENSION_DEPENDENT_DLL;
    }

    //
    // Schema used to load imports will be different
    // for the normal and secure sides.
    //

    if (CHECK_FLAG(OslLoadImageFlags, OSL_IMAGE_FLAGS_IUM)) {
        ApiSetSchema = OslApiSetSecureSchema;
    } else {
        ApiSetSchema = OslApiSetSchema;
    }

    //
    // Walk the import table, loading each specified DLL.
    //

    while ((ImportDescriptor->Name != 0) &&
           (ImportDescriptor->OriginalFirstThunk != 0) &&
           (ImportTableSize != 0)) {

        //
        // Create the full image path to the referenced DLL.
        //

        ImportName = Add2Ptr(DataTableEntry->DllBase, ImportDescriptor->Name);
        Status = BlCopyStringToUnicodeString(&ImageName, ImportName);
        if (!NT_SUCCESS(Status)) {
            break;
        }

        //
        // Redirect NTOS and HAL imports to the SecureKernel when loading an
        // IUM image.
        //

        if ((CHECK_FLAG(OslLoadImageFlags, OSL_IMAGE_FLAGS_IUM) != FALSE) &&
            ((_wcsicmp(ImageName.Buffer, KERNEL_DEFAULT_NAME) == 0) ||
             (_wcsicmp(ImageName.Buffer, HAL_DEFAULT_NAME) == 0))) {

            Status = BlCopyUnicodeStringToUnicodeString(&ImageName,
                                                        &SecureKernel);

            if (!NT_SUCCESS(Status)) {
                break;
            }
        }

        //
        // If importing an API set, replace the import name
        // and directory.
        //

        LoadPath = &DirectoryPath;
        OslLoadImageFlags &= ~OSL_IMAGE_FLAGS_APISET;

        if (NULL != ApiSetSchema) {
            Status = ApiSetResolveToHost(ApiSetSchema,
                                         &ImageName,
                                         &DataTableEntry->BaseDllName,
                                         &ApiSetResolved,
                                         &ApiSetHost);

            if (!NT_SUCCESS(Status)) {
                break;
            }

            if (ApiSetResolved) {
                if (ApiSetHost.Length == 0) {
                    ImportDescriptor += 1;
                    ImportTableSize -= sizeof(IMAGE_IMPORT_DESCRIPTOR);
                    continue;
                }

                if (RtlPrefixUnicodeString(&ExtensionPrefix,
                                           &ImageName,
                                           TRUE) != FALSE) {

                    StripExtensionImportRelocations(DataTableEntry,
                                                    ImportDescriptor);
                }

                Status = BlCopyUnicodeStringToUnicodeString(&ImageName,
                                                            &ApiSetHost);

                if (!NT_SUCCESS(Status)) {
                    break;
                }

                Status = InitializeDriversPath(&DriversPath);
                if (!NT_SUCCESS(Status)) {
                    break;
                }

                LoadPath = &DriversPath;
                OslLoadImageFlags |= OSL_IMAGE_FLAGS_APISET;
            }
        }

        //
        // For all non-self referencing imports, load the binary containing the
        // imports and bind their references in the current image.
        //

        if (RtlEqualUnicodeString(&DataTableEntry->BaseDllName,
                                  &ImageName,
                                  TRUE) == FALSE) {

            Status = LoadImageEx(LoadPath,
                                 ImageName.Buffer,
                                 NULL,
                                 DeviceId,
                                 MemoryType,
                                 LoadedImageList,
                                 &ImportedImageDataTableEntry,
                                 LDRP_DRIVER_DEPENDENT_DLL,
                                 BldrFlags,
                                 ImageFlags,
                                 NULL,
                                 OslLoadImageFlags);

            if ((Status == STATUS_NO_SUCH_FILE) && (LoadPath != &DriversPath)) {

                //
                // When binary is not found beside its importing image then
                // attempt to load it from under the drivers directory.
                //

                Status = InitializeDriversPath(&DriversPath);
                if (!NT_SUCCESS(Status)) {
                    break;
                }

                Status = LoadImageEx(&DriversPath,
                                     ImageName.Buffer,
                                     NULL,
                                     DeviceId,
                                     MemoryType,
                                     LoadedImageList,
                                     &ImportedImageDataTableEntry,
                                     LDRP_DRIVER_DEPENDENT_DLL,
                                     BldrFlags,
                                     ImageFlags,
                                     NULL,
                                     OslLoadImageFlags);
            }

            if (!NT_SUCCESS(Status)) {
                OslFatalError(OSL_ERROR_LOAD_IMPORT,
                              (ULONG_PTR)ImageName.Buffer,
                              (ULONG_PTR)DataTableEntry->FullDllName.Buffer,
                              Status);

                break;
            }

            Status = BlLdrBindImportReferences(DataTableEntry,
                                               ImportedImageDataTableEntry,
                                               ImportDescriptor,
                                               LoadedImageList,
                                               FALSE);

            if (!NT_SUCCESS(Status)) {
                break;
            }
        }

        ImportDescriptor += 1;    // Advance pointer.
        ImportTableSize -= sizeof(IMAGE_IMPORT_DESCRIPTOR);
    }

LoadImportsEnd:
    if (FullPath != NULL) {
        BlMmFreeHeap(FullPath);
    }

    if (ImageName.Buffer != NULL) {
        BlMmFreeHeap(ImageName.Buffer);
    }

    if (DirectoryPath.Buffer != NULL) {
        BlMmFreeHeap(DirectoryPath.Buffer);
    }

    if (DriversPath.Buffer != NULL) {
        BlMmFreeHeap(DriversPath.Buffer);
    }

    return Status;
}

NTSTATUS
LoadImageEx (
    __in PUNICODE_STRING ImageDirectoryPath,
    __in PWSTR ImageName,
    __in_opt PUNICODE_STRING ServiceFilePath,
    __in DEVICE_ID DeviceId,
    __in MEMORY_TYPE MemoryType,
    __in PLIST_ENTRY LoadedImageList,
    __deref_out PKLDR_DATA_TABLE_ENTRY *DataTableEntry,
    __in ULONG KldrFlags,
    __in ULONG BldrFlags,
    __in ULONG ImageFlags,
    __out_opt PULONG LoadInformation,
    __in ULONG OslLoadImageFlags
    )

/*++

Routine Description:

    This routine loads a PE image into memory.

Arguments:

    ImageDirectoryPath - Supplies the directory path for the image to load.
        The memory allocated for the unicode string may change.  The caller is
        responsible for freeing this memory.

    ImageName - Supplies the name of the image to load.

    ServiceFilePath - Supplies an optional pointer to the file path for a
        boot driver, as described in its service registry entry.

    DeviceId - Supplies a handle to the device containing the image to load.

    MemoryType - Supplies the type of memory to mark the image allocation.

    LoadedImageList - Supplies a list of images currently loaded by the OS
        loader.

    DataTableEntry - Supplies a pointer to a variable that receives an allocated
        data table entry describing the loaded module.  The caller is
        responsible for freeing this memory.

    KldrFlags - Supplies the kernel loader flags to use in the allocated data
        table entry.

    BldrFlags - Supplies the boot loader flags to use in the allocated data
        table entry.

    ImageFlags - Supplies boot library image loading flags.  Flags include :

        IMAGE_FLAGS_ALLOCATE_IN_LARGE_PAGE : Specifies that an image should
            be allocated in an exclusive system component large page.

    LoadInformation - Supplies an optional pointer to a variable that receives
        informational flags describing various aspects of the load.

    OslLoadImageFlags - Supplies image flags specifying functionality of this
        routine.  Possible values include :

        OSL_IMAGE_FLAGS_LOAD_IMPORTS : Specifies that the image's imports
            should be loaded.  LoadedImageList is ordered by dependency.  If
            driver A contains imports from DLL B, then B is inserted before A
            in the list.

        OSL_IMAGE_FLAGS_EARLY_LAUNCH : Specifies that the image is an anti-
            malware driver and is subject to a higher bar of integrity policy.

        OSL_IMAGE_FLAGS_CORE_DRIVER : Specifies that the image is an image to
            be loaded before the anti-malware driver and is subject to a higher
            bar of integrity policy.

        OSL_IMAGE_FLAGS_APISET : Specifies that the image was originally
            referred to as an API set (or API set extension), thus it is an
            OS internal component and is subject to a higher bar of
            integrity policy (e.g., must be MS-signed).

        OSL_IMAGE_FLAGS_ELAM_IMPORT : Specifes that the image is an image to be
            initialized before the anti-malware driver because the anti-malware
            driver imports from it.

        OSL_IMAGE_FLAGS_WINDOWS : Specifies that the image is subject to
            Windows integrity policy.

        OSL_IMAGE_FLAGS_IUM : Specifies that the image runs in the Isolated
            User Mode environment.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if there is insufficient memory to perform the request.

    STATUS_UNSUCCESSFUL if unable to apply relocations to an imported DLL.

    STATUS_INVALID_IMAGE_FORMAT if relocations are invalid.

    STATUS_NO_SUCH_FILE if the requested file does not exist on the specified
        block i/o device.

    STATUS_FILE_INVALID if the specified file is not a PE image.

--*/

{

    BOOLEAN bRet;
    UNICODE_STRING FileName;
    NTSTATUS Status;

    RtlInitUnicodeString(&FileName, NULL);

    //
    // Build a full path for the specified DLL, consisting of the directory
    // path plus the imported file name.  Take care to calculate the correct
    // size up front so that we only have to call the allocator one time.
    //

    FileName.MaximumLength =
        ImageDirectoryPath->Length +
            (USHORT)(wcslen(ImageName) + 1) * sizeof(WCHAR);

    FileName.Buffer = BlMmAllocateHeap(FileName.MaximumLength);
    if (FileName.Buffer == NULL) {
        Status = STATUS_NO_MEMORY;
        goto LoadImageExEnd;
    }

    RtlZeroMemory(FileName.Buffer, FileName.MaximumLength);
    bRet = BlAppendUnicodeToString(&FileName, ImageDirectoryPath->Buffer);
    if (bRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto LoadImageExEnd;
    }

    bRet = BlAppendUnicodeToString(&FileName, ImageName);
    if (bRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto LoadImageExEnd;
    }

    //
    // Load the specified image and all of its imports.
    //

    Status = OslLoadImage(DeviceId,
                          MemoryType,
                          FileName.Buffer,
                          NULL,
                          ServiceFilePath,
                          NULL,
                          0,
                          LoadedImageList,
                          DataTableEntry,
                          KldrFlags,
                          BldrFlags,
                          ImageFlags,
                          OslLoadImageFlags,
                          LoadInformation);

LoadImageExEnd:
    if (FileName.Buffer != NULL) {
        BlMmFreeHeap(FileName.Buffer);
    }

    return Status;
}

NTSTATUS
GetFullPath (
    __in PWSTR FilePath,
    __deref_out PWSTR *FullPath
    )

/*++

Routine Description:

    Returns the full path to a file or directory.

Arguments:

    FilePath - Supplies a path starting from system root.

    FullPath - Supplies a pointer to a variable that receives the full path.

Return Value:

    STATUS_SUCCESS if function succeeded.

    STATUS_NO_MEMORY if memory allocation failed.

    Another error status code if returned from a sub-routine.

--*/

{

    ULONG Length;
    PWCHAR OsFilePath;
    ULONG OsFilePathSize;
    ULONG RequiredBufferSize;
    NTSTATUS Status;
    PWCHAR SystemRoot;

    OsFilePath = NULL;
    Status = STATUS_SUCCESS;

    //
    // Extract the OS path, appending a trailing backslash if one does not
    // exist.
    //

    Status = BlGetApplicationOptionString(BCDE_OSLOADER_TYPE_SYSTEM_ROOT,
                                          &SystemRoot);

    if (!NT_SUCCESS(Status)) {
        goto GetFullPathEnd;
    }

    Length = (ULONG)wcslen(SystemRoot);
    if (SystemRoot[Length - 1] != L'\\') {
        OsFilePath = BlMmAllocateHeap((Length + 2) * sizeof(WCHAR));
        if (OsFilePath == NULL) {
            OsFilePath = SystemRoot;
            Status = STATUS_NO_MEMORY;
            goto GetFullPathEnd;
        }

        wcscpy_s(OsFilePath, Length + 2, SystemRoot);
        wcscat_s(OsFilePath, Length + 2, L"\\");
        Length += 1;
        BlMmFreeHeap(SystemRoot);

    } else {
        OsFilePath = SystemRoot;
    }

    OsFilePathSize = Length * sizeof(WCHAR);
    RequiredBufferSize = OsFilePathSize + (ULONG)(wcslen(FilePath) + 1) * sizeof(WCHAR);
    *FullPath = BlMmAllocateHeap(RequiredBufferSize);
    if (*FullPath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto GetFullPathEnd;
    }

    wcscpy_s(*FullPath, RequiredBufferSize / sizeof(WCHAR), OsFilePath);
    wcscat_s(*FullPath, RequiredBufferSize / sizeof(WCHAR), FilePath);

GetFullPathEnd:
    if (OsFilePath != NULL) {
        BlMmFreeHeap(OsFilePath);
    }

    return Status;
}

ULONG
GetImageValidationFlags (
    __in PWSTR FileName,
    __in ULONG OslLoadImageFlags
    )

/*++

Routine Description:

    This routine will check if a given file should be hashed and validated
    against the system catalogs or a self-signed certificate.

Arguments:

    FileName - Supplies the name of the binary to check.

    OslLoadImageFlags - Supplies the load flags.

Return Value:

    A mask of image flags describing the validation actions that should be
    applied to the given binary.  These flags must constitute a subset of the
    IMAGE_FLAGS_HASH_VALIDATION_FLAGS mask.

--*/

{

    const PCWSTR *CurrentName;
    ULONG Flags;

    //
    // The hash of every image is validated whenever the code integrity
    // subsystem is initialized.
    //

    if (LoadIntegrityCheckPolicy == IntegrityCheckUninitialized) {
        return IMAGE_FLAGS_NONE;
    }

    Flags = IMAGE_FLAGS_VALIDATE_IMAGE_HASH;

    //
    // Set the valid hash required bit if hash validity is required by global
    // policy.  If the image is a Microsoft image, validate the integrity and
    // ensure the signature is a Microsoft certificate.
    //
    // If the header contains the force integrity bit set, validate the the
    // integrity of the signature.
    //

    if (LoadIntegrityCheckPolicy >= IntegrityCheckEnabled) {
        if (LoadIntegrityCheckPolicy == IntegrityCheckAllImagesEnabled) {
            SET_FLAGS(Flags, IMAGE_FLAGS_VALID_HASH_REQUIRED);
        }

        if (CHECK_FLAG(OslLoadImageFlags, OSL_IMAGE_FLAGS_APISET) != FALSE) {
            SET_FLAGS(Flags, IMAGE_FLAGS_VALID_HASH_REQUIRED |
                             IMAGE_FLAGS_WINDOWS_SIGNATURE_REQUIRED);

        } else if (CHECK_FLAG(OslLoadImageFlags,
                              OSL_IMAGE_FLAGS_APISET_SCHEMA) != FALSE) {

                SET_FLAGS(Flags, IMAGE_FLAGS_VALID_HASH_REQUIRED |
                                 IMAGE_FLAGS_WINDOWS_SIGNATURE_REQUIRED);

        } else {
            for (CurrentName = OslMicrosoftBootImages;
                 *CurrentName != NULL;
                 CurrentName += 1) {

                if (_wcsicmp(FileName, *CurrentName) == 0) {
                    SET_FLAGS(Flags, IMAGE_FLAGS_VALID_HASH_REQUIRED |
                                     IMAGE_FLAGS_WINDOWS_SIGNATURE_REQUIRED);

                    break;
                }
            }
        }

        SET_FLAGS(Flags, IMAGE_FLAGS_VALIDATE_FORCE_INTEGRITY);
    }

#if !defined(BUILD_PRODUCTION_SIGNED) && !defined(BUILD_FLIGHT_SIGNED)

    //
    // If test signing is enabled, then turn off all image validation for the
    // debug transport binaries.  This is to enable 3rd parties to build and
    // use custom debug transports for bringing up Windows on their own unique
    // hardware platforms.  (SOC self enablement.)
    //

    if ((LoadIntegrityTestSigningEnabled != FALSE) &&
        ((_wcsicmp(FileName, KD_FRIENDLY_NAME) == 0) ||
        (_wcsicmp(FileName, KD_EXTENSION_FRIENDLY_NAME) == 0))) {

        Flags = IMAGE_FLAGS_NONE;
    }


#endif

    return Flags;
}

VOID
KnownAnswerTest (
    __in DEVICE_ID OsDeviceId
    )

/*++

Routine Description:

    Executes a set of known answer test for RSA signature verification and SHA1
    hashing functions.

Arguments:

    OsDeviceId - Supplies the id of a device containing the OS.

Return Value:

    None.

--*/

{

    PWCHAR FilePath;
    NTSTATUS Status;

    FilePath = NULL;

    //
    // Open file \SystemRoot\System32\winsha1.bin and process sha1 test
    // vectors.
    //

    Status = GetFullPath(SHA1_TEST_FILE, &FilePath);
    if (!NT_SUCCESS(Status)) {
        goto KnownAnswerTestEnd;
    }

    Status = BlImgSha1KnownAnswerTest(OsDeviceId, FilePath);
    if (!NT_SUCCESS(Status)) {
        BlStatusPrint(L"Failed to process SHA1 test vectors. Status = %x\n",
                      Status);
    }

    BlMmFreeHeap(FilePath);
    FilePath = NULL;

    //
    // Open file \SystemRoot\System32\winsha1monte.bin and process Sha1 Monte
    // Carlo test vectors.
    //

    Status = GetFullPath(SHA1MONTE_TEST_FILE, &FilePath);
    if (!NT_SUCCESS(Status)) {
        goto KnownAnswerTestEnd;
    }

    Status = BlImgSha1MonteCarloTest(OsDeviceId, FilePath);
    if (!NT_SUCCESS(Status)) {
        BlStatusPrint(L"Failed to process SHA1 Monte Carlo test vectors."
                      L"Status = %x\n",
                      Status);
    }

    //
    // Open file \SystemRoot\System32\winrsa.bin and process rsa test vectors.
    //

    Status = GetFullPath(RSA_TEST_FILE, &FilePath);
    if (!NT_SUCCESS(Status)) {
        goto KnownAnswerTestEnd;
    }

    Status = BlImgRsaKnownAnswerTest(OsDeviceId, FilePath);
    if (!NT_SUCCESS(Status)) {
        BlStatusPrint(L"Failed to process RSA test vectors. Status = %x\n",
                      Status);
    }

KnownAnswerTestEnd:
    if (FilePath != NULL) {
        BlMmFreeHeap(FilePath);
    }

    return;
}

BOOLEAN
OslpAllowedEarlyLaunchImport (
    __in PCWSTR FileName
    )

/*++

Routine Description:

    This routine determines if the supplied file is an allowed import of an
    Early-Launch Anti-Malware driver.

Arguments:

    FileName - Supplise the name of the file to be checked against the list of
        allowed files.

Return Value:

    TRUE if the supplied file is on the allowed list. FALSE otherwise.

--*/

{

    const PCWSTR *CurrentName;

    for (CurrentName = OslElamDriverAllowedImports;
         *CurrentName != NULL;
         CurrentName += 1) {

        if (_wcsicmp(FileName, *CurrentName) == 0) {
            return TRUE;
        }
    }

    return FALSE;
}

PWSTR
OslpConvertFromSymbolicPath (
    __in PWSTR PathToConvert
    )

/*++

Routine Description:

    This routine converts a symbolic path (\SystemRoot) to an OS-relative
    one (typically \Windows).

Arguments:

    PathToConvert - Supplies the path to convert.

Return Value:

    Returns a heap-allocated string representing the explicit OS-relative
        path, or NULL.

--*/

{

    PWCHAR ExplicitPath;
    SIZE_T ExplicitPathSize;
    PWCHAR TempString;

    TempString = wcsstr(PathToConvert, L"\\SystemRoot\\");
    if (TempString == NULL) {
        return NULL;
    }

    TempString += wcslen(L"\\SystemRoot\\");
    ExplicitPathSize = wcslen(OslSystemRoot) + wcslen(TempString) + 1;
    ExplicitPath = BlMmAllocateHeap(ExplicitPathSize * sizeof(WCHAR));
    if (ExplicitPath == NULL) {
        return NULL;
    }

    swprintf_s(ExplicitPath,
               ExplicitPathSize,
               L"%s%s",
               OslSystemRoot,
               TempString);

    return ExplicitPath;
}

PWSTR
OslpConvertToSymbolicPath (
    __in PWSTR PathToConvert
    )

/*++

Routine Description:

    This routine converts an OS-relative path (usually \Windows) to a
    symbolic one (\SystemRoot).

Arguments:

    PathToConvert - Supplies the path to convert.

Return Value:

    Returns a heap-allocated string representing the symbolic path, or NULL.

--*/

{

    PWCHAR SymbolicPath;
    SIZE_T SymbolicPathSize;
    PWCHAR TempString;

    TempString = wcsstr(PathToConvert, OslSystemRoot);
    if (TempString == NULL) {
        return NULL;
    }

    TempString += wcslen(OslSystemRoot);
    SymbolicPathSize = wcslen(L"\\SystemRoot\\") + wcslen(TempString) + 1;
    SymbolicPath = BlMmAllocateHeap(SymbolicPathSize * sizeof(WCHAR));
    if (SymbolicPath == NULL) {
        return NULL;
    }

    swprintf_s(SymbolicPath,
               SymbolicPathSize,
               L"\\SystemRoot\\%s",
               TempString);

    return SymbolicPath;
}

VOID
OslpFilterDriverList (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine filters out core drivers, (3rd party) core extensions, and
    ELAM drivers into their respective lists.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

Return Value:

    None.

--*/

{

    UNICODE_STRING FilterGroup;

    //
    // Filter the boot driver list, moving ELAM drivers into their own driver
    // list.
    //

    RtlInitUnicodeString(&FilterGroup, L"Early-Launch");
    OslpFilterDriverListOnGroup(&FilterGroup,
                                &LoaderBlock->BootDriverListHead,
                                &LoaderBlock->EarlyLaunchListHead);

    //
    // Filter the boot driver list, moving core drivers into their own driver
    // list.
    //

    OslpFilterCoreDrivers(LoaderBlock);

    //
    // Perform phase 0 of core extensions filtering.
    //

    OslFilterCoreExtensions(0, LoaderBlock);

    return;
}

VOID
OslpFilterCoreDrivers (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine filters the core drivers out of the boot driver list.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

Return Value:

    None.

--*/

{

    const PCWSTR *ServiceList;
    NTSTATUS Status;

    //
    // Search the boot driver list for each of the core driver services. The
    // core driver list should be sorted by the order of the core driver
    // services names.
    //

    //
    // Filter the Core drivers out of the boot driver list and move them into
    // the Core driver list.
    //

    OslpFilterDriverListOnServices(OslCoreDriverServices,
                                   &LoaderBlock->BootDriverListHead,
                                   &LoaderBlock->CoreDriverListHead);

    //
    // Use the list of drivers for measured boot only if there is a TPM
    // device available.
    //

    Status = BlTpmStatus();
    if (NT_SUCCESS(Status)) {
        ServiceList = OslTpmCoreDriverServicesMeasured;
    } else {
        ServiceList = OslTpmCoreDriverServices;
    }

    //
    // Filter the TPM Core out of the boot driver list and move them into the
    // TPM Core driver list.
    //

    OslpFilterDriverListOnServices(ServiceList,
                                   &LoaderBlock->BootDriverListHead,
                                   &LoaderBlock->TpmCoreDriverListHead);

    return;
}

VOID
OslpFilterCoreExtensionsOnGroups (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in_xcount(null_terminated_at_compile_time) const PCWSTR *LoadOrderGroups,
    __inout PLIST_ENTRY FilteredDriverList
    )

/*++

Routine Description:

    This routine filters the core extensions out of the boot driver list and
    adds them to the appropriate list in loader block.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

Return Value:

    None.

--*/

{

    UNICODE_STRING FilterGroup;
    const PCWSTR *NameListEntry;

    //
    // Each group is statically defined at compile time and shouldn't be NULL.
    //

    __analysis_assume(LoadOrderGroups != NULL);

    //
    // Walk the boot driver list and filter drivers in the core extension
    // groups into a separate list.
    //

    NameListEntry = LoadOrderGroups;
    while (*NameListEntry != NULL) {
        RtlInitUnicodeStringEx(&FilterGroup, *NameListEntry);
        OslpFilterDriverListOnGroup(
            &FilterGroup,
            &LoaderBlock->BootDriverListHead,
            FilteredDriverList);

        NameListEntry += 1;
    }

    return;
}

VOID
OslpFilterDriverListOnGroup (
    __in PUNICODE_STRING Group,
    __inout PLIST_ENTRY BootDriverList,
    __inout PLIST_ENTRY FilteredDriverList
    )

/*++

Routine Description:

    This routine filters a group from the boot driver list, removing matches
    and inserting them in the specified linked list.

Arguments:

    Group - Supplies the name of the group to filter.

    BootDriverList - Supplies the list of boot start drivers.

    FilteredDriverList - Supplies a linked list that receives each of the
        drivers filtered.

Return Value:

    None.

--*/

{

    PBOOT_DRIVER_NODE BootDriver;
    PLIST_ENTRY NextEntry;

    ASSERT(Group != NULL);
    ASSERT(BootDriverList != NULL);
    ASSERT(FilteredDriverList != NULL);

    NextEntry = BootDriverList->Flink;
    while (NextEntry != BootDriverList) {
        BootDriver = CONTAINING_RECORD(NextEntry,
                                       BOOT_DRIVER_NODE,
                                       ListEntry.Link);

        NextEntry = NextEntry->Flink;
        if (RtlEqualUnicodeString(&BootDriver->Group, Group, TRUE) != FALSE) {
            RemoveEntryList(&BootDriver->ListEntry.Link);
            InsertTailList(FilteredDriverList, &BootDriver->ListEntry.Link);
        }
    }

    return;
}

VOID
OslpFilterDriverListOnServices (
    __in_xcount(null_terminated_at_compile_time) const PCWSTR *ServiceNames,
    __inout PLIST_ENTRY BootDriverList,
    __inout PLIST_ENTRY FilteredDriverList
    )

/*++

Routine Description:

    This routine filters drivers with given service names from the specified
    boot driver list and adds them to the filtered driver list.

Arguments:

    ServiceNames - Supplies the driver service names to be filtered.

    BootDriverList - Supplies the list of boot start drivers.

    FilteredDriverList - Supplies a linked list that receives each of the
        drivers filtered.

Return Value:

    None.

--*/

{

    PBOOT_DRIVER_NODE BootDriver;
    UNICODE_STRING CurrentName = {0};
    const PCWSTR *NameListEntry;
    PLIST_ENTRY NextEntry;

    NameListEntry = ServiceNames;
    while (*NameListEntry != NULL) {
        RtlInitUnicodeStringEx(&CurrentName, *NameListEntry);
        NextEntry = BootDriverList->Flink;
        while (NextEntry != BootDriverList) {
            BootDriver = CONTAINING_RECORD(NextEntry,
                                           BOOT_DRIVER_NODE,
                                           ListEntry.Link);

            NextEntry = NextEntry->Flink;
            if (RtlEqualUnicodeString(&BootDriver->Name,
                                      &CurrentName,
                                      TRUE) != FALSE) {

                RemoveEntryList(&BootDriver->ListEntry.Link);
                InsertTailList(FilteredDriverList, &BootDriver->ListEntry.Link);
                break;
            }
        }

        NameListEntry += 1;
    }

    return;
}

VOID
ReportCodeIntegrityFailure (
    __in NTSTATUS FailureStatus
    )

/*++

Routine Description:

    Reports the failure of Code Integrity self verification tests.

Arguments:

    FailureStatus - Supplies the failure status of self test.

Return Value:

    None.

--*/

{

    PWCHAR ApplicationPath;
    NTSTATUS Status;

    ApplicationPath = NULL;

    //
    // Get the path of boot application.
    //

    Status = BlGetApplicationOptionString(BCDE_LIBRARY_TYPE_APPLICATION_PATH,
                                          &ApplicationPath);

    if (!NT_SUCCESS(Status)) {
        goto ReportCodeIntegrityFailureEnd;
    }

    OslFatalError(OSL_ERROR_CODE_INTEGRITY_INITIALIZATION,
                  (ULONG_PTR)ApplicationPath,
                  FailureStatus,
                  0);

ReportCodeIntegrityFailureEnd:
    if (ApplicationPath != NULL) {
        BlMmFreeHeap(ApplicationPath);
    }

    return;
}

VOID
StripExtensionImportRelocations (
    __in PKLDR_DATA_TABLE_ENTRY ImageEntry,
    __in PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor
    )

/*++

Routine Description:

    This routine strips any potential relocation entries for addresses in the
    import table of an extension. This is done because the kernel may relocate
    images, and if it applies the relocations to snapped thunks, it will
    corrupt them - the relocations are required only when bound to the stubs.

Arguments:

    ImageEntry - Supplies the data table entry for the image to strip the import
        relocations.

    ImportDescriptor - Supplies the import descriptor whose imports will have
        relocations stripped.

Return Value:

    None.

--*/

{
    PUSHORT Entry;
    PUSHORT EntryEnd;
    ULONG Offset;
    ULONG OffsetEnd;
    ULONG OffsetStart;
    PIMAGE_BASE_RELOCATION Relocation;
    ULONG RelocationSize;
    PIMAGE_THUNK_DATA ThunkAddressEntry;

    //
    // Find the relocation table. We're done if the image doesn't have one.
    //

    Relocation = RtlImageDirectoryEntryToData(ImageEntry->DllBase,
                                              TRUE,
                                              IMAGE_DIRECTORY_ENTRY_BASERELOC,
                                              &RelocationSize);

    if ((Relocation == NULL) || (RelocationSize == 0)) {
        return;
    }

    //
    // Calculate the RVA range for which we want to strip relocations. We do
    // this by traversing the thunks until we reach the null terminator.
    //

    ThunkAddressEntry = Add2Ptr(ImageEntry->DllBase,
                                ImportDescriptor->FirstThunk);

    while (ThunkAddressEntry->u1.Function != 0) {
        ThunkAddressEntry += 1;
    }

    OffsetStart = ImportDescriptor->FirstThunk;

    OffsetEnd =
        (ULONG)((ULONG_PTR)ThunkAddressEntry - (ULONG_PTR)ImageEntry->DllBase);

    //
    // Iterate through the relocations and strip the ones within the range.
    //

    while (RelocationSize > 0) {

        if ((Relocation->VirtualAddress < OffsetEnd) &&
            (Relocation->VirtualAddress + 4096 > OffsetStart)) {

            Entry = (PUSHORT)(Relocation + 1);
            EntryEnd = Add2Ptr(Entry,
                               Relocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION));

            while (Entry < EntryEnd) {
                Offset = Relocation->VirtualAddress + (*Entry & 0xfff);
                if ((Offset >= OffsetStart) && (Offset < OffsetEnd)) {
                    *Entry = (*Entry & 0xfff) | (IMAGE_REL_BASED_ABSOLUTE << 12);
                }

                Entry += 1;
            }
        }

        RelocationSize -= Relocation->SizeOfBlock;
        Relocation = Add2Ptr(Relocation, Relocation->SizeOfBlock);
    }
}

VOID
OslpSetSigningPolicy (
    _In_ ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine sets the signing policy required for image verification.

Arguments:

    SystemHiveHandle - Supplies the handle to the system hive.

Return Value:

    None.

--*/

{
    BOOLEAN IsDebugDevice;
    BOOLEAN SecureBootEnabled;
    ULONG SigningPolicy;
    NTSTATUS Status;
    ULONG TestFlags;

    //
    // Get the current signing policy.
    //

    SigningPolicy = BlImgGetSigningPolicy();

    //
    // Read the test hooks.
    //

    Status = OslHiveReadWriteControlDword(SystemHiveHandle,
                                          FALSE,
                                          L"CI",
                                          L"TestFlags",
                                          &TestFlags);

    if (!NT_SUCCESS(Status)) {
        TestFlags = 0;
    }

    //
    // Support kits signers on debug devices so that labs may install kits.
    //

    Status = BlSecureBootIsEnabled(&SecureBootEnabled);
    if (!NT_SUCCESS(Status)) {
        SecureBootEnabled = FALSE;
    }

    IsDebugDevice = SecureBootEnabled == FALSE || BlSecureBootIsDebugPolicyActive();
    if (IsDebugDevice) {
        SET_FLAGS(SigningPolicy, IMAGE_SIGNING_POLICY_ALLOW_KITS_DRIVERS);
    }

    //
    // If Authenticode signed third-party drivers are disabled, allow only WHQL
    // signed third party drivers.
    //

    if (BlSecureBootArePolicyOptionsSet(
            SECUREBOOT_POLICY_DISABLE_THIRD_PARTY_DRIVER_SIGNING) != FALSE) {

        BlImgSetRestrictedSigning(TRUE);
        SET_FLAGS(SigningPolicy, IMAGE_SIGNING_POLICY_ENFORCE_WHQL_SIGNED_DRIVERS);
    }

    if (BlSecureBootArePolicyOptionsSet(
            SECUREBOOT_POLICY_ALLOW_KITS_SIGNERS) != FALSE) {

        SET_FLAGS(SigningPolicy, IMAGE_SIGNING_POLICY_ALLOW_KITS_DRIVERS);
    }

    //
    // It is safe to always support the test hook for disabling kits signers.
    //

    if (CHECK_FLAG(TestFlags, CI_TEST_FLAG_DO_NOT_TRUST_KITS_SIGNERS) != FALSE) {
        CLEAR_FLAGS(SigningPolicy, IMAGE_SIGNING_POLICY_ALLOW_KITS_DRIVERS);
    }

    //
    // Inform the boot library of the required policy.
    //

    BlImgSetSigningPolicy(SigningPolicy);
}

NTSTATUS
OslpProcessSIPolicy(
    _In_ BOOLEAN IntegrityChecksDisabled
    )
/*++

Routine Description:

    This routine processes system integrity policy

Arguments:

    IntegrityChecksDisabled - indicates if code integrity check is disabled.

Return Value:

    Standard NTSTATUS code

--*/
{
    NTSTATUS                        Status = STATUS_SUCCESS;

    //
    //  If integrity check is disabled, disable SI policy enforcement for this boot only
    //  by erasing current policy blob and do not try to read any si policy file.
    //

    if (IntegrityChecksDisabled != FALSE)
    {
        Status = SIPolicyClearAllActivePolicy();
        if (!NT_SUCCESS(Status)) {
            goto ProcessSIPolicyEnd;
        }

        Status = BlSIPolicyPackageActivePolicyForKernel(NULL, 0);
        goto ProcessSIPolicyEnd;
    }

    //
    // Have the policies already been processed?
    //

    if (LoadProcessedSystemIntegrityPolicy != FALSE) {
        goto ProcessSIPolicyEnd;
    }

    //
    //  Load,verify and activate SI policy
    //

    Status = BlSIPolicyCheckPolicyOnDevice(OslLoadDevice, TRUE);

    if (!NT_SUCCESS(Status)) {
        goto ProcessSIPolicyEnd;
    }

    //
    // Persist the active policies for kernel.
    //

    Status = BlSIPolicyRepackageActivePolicyForKernel();
    if (!NT_SUCCESS(Status)) {
        goto ProcessSIPolicyEnd;
    }

    LoadProcessedSystemIntegrityPolicy = TRUE;

ProcessSIPolicyEnd:

    if (!NT_SUCCESS(Status) && BlBdDebuggerEnabled() != FALSE) {
        BlStatusPrint(L"Error - OslpProcessSIPolicy failed. Status = 0x%08x\n", Status);
    }
    return Status;
}

VOID
OslpProcessLoadConfigDirectoryForModule (
    _In_ PKLDR_DATA_TABLE_ENTRY DataTableEntry
    )

/*++

Routine Description:

    This routine examines a loaded image to determine whether it contains a
    load config directory.   If so, the load config directory is processed as
    appropriate - for example, /GS cookies are initialized (if present).

Arguments:

    DataTableEntry - Supplies a pointer to the data table entry for the module
        that is to be processed.

Return Value:

    None.

--*/

{

    ULONG_PTR ExtensionSeed;

    ExtensionSeed = OslGenRandom();

    //
    // Set up the /GS cookie for the module.
    //

    LdrInitSecurityCookie(DataTableEntry->DllBase,
                          DataTableEntry->SizeOfImage,
                          NULL,
                          ExtensionSeed,
                          NULL);

    return;
}

NTSTATUS
OslpSetWeakCryptoPolicy (
    _In_ ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine sets the weak crypto policy configured for the system.

Arguments:

    SystemHiveHandle - Supplies the handle to the system hive.

Return Value:

    STATUS_SUCCESS if successful.

--*/

{
    WEAK_CRYPTO_SETTINGS Config[CI_WEAK_CRYPTO_MAX_ALG] = {0};
    WEAK_CRYPTO_SETTINGS Default[CI_WEAK_CRYPTO_MAX_ALG] = {0};
    ULONG i;
    ULONG NumberOfPolicies;
    NTSTATUS Status;
    PMINCRYPT_WEAK_CRYPTO_POLICY WeakCryptoPolicies;

    //
    // Load the weak crypto policy from the registry.
    //

    Status = OslpLoadWeakCryptoPolicy(SystemHiveHandle, Default, Config);
    if (!NT_SUCCESS(Status)) {
        goto SetWeakCryptoPolicyEnd;
    }

    //
    // Count the number of policies.
    //

    NumberOfPolicies = 0;

    for (i = 0; i < CI_WEAK_CRYPTO_MAX_ALG; i++) {
        if (Default[i].Enabled != FALSE || Config[i].Enabled != FALSE) {

            NumberOfPolicies++;
        }
    }

    if (NumberOfPolicies == 0) {
        //
        // No weak crypto policy, we are done!
        //

        goto SetWeakCryptoPolicyEnd;
    }

    //
    // Initialize policy structures (Config values override Default).
    //

    WeakCryptoPolicies = LoadWeakCryptoPolicy;

    for (i = 0; i < CI_WEAK_CRYPTO_MAX_ALG; i++) {

        if (Default[i].Enabled == FALSE && Config[i].Enabled == FALSE) {
            continue;
        }

        //
        // Hash algorithm.
        //

        WeakCryptoPolicies->HashAlgorithm = LoadWeakCryptoAlgorithms[i];

        //
        // Flags.
        //

        if (Config[i].Enabled != FALSE)
        {
            WeakCryptoPolicies->Flags = Config[i].Values[CI_WEAK_CRYPTO_FLAGS_REG_ENTRY].Options;
        }
        else
        {
            WeakCryptoPolicies->Flags = Default[i].Values[CI_WEAK_CRYPTO_FLAGS_REG_ENTRY].Options;
        }

        //
        // Cert hash's after time.
        //

        if (Config[i].Enabled != FALSE &&
            Config[i].Values[CI_WEAK_CRYPTO_CERT_AFTER_TIME_REG_ENTRY].Set != FALSE)
        {
            WeakCryptoPolicies->CertHashNotAfter = Config[i].Values[CI_WEAK_CRYPTO_CERT_AFTER_TIME_REG_ENTRY].LargeInteger;
        }
        else if (Default[i].Enabled != FALSE &&
                 Default[i].Values[CI_WEAK_CRYPTO_CERT_AFTER_TIME_REG_ENTRY].Set != FALSE)
        {
            WeakCryptoPolicies->CertHashNotAfter = Default[i].Values[CI_WEAK_CRYPTO_CERT_AFTER_TIME_REG_ENTRY].LargeInteger;
        }

        //
        // File hash's after time.
        //

        if (Config[i].Enabled != FALSE &&
            Config[i].Values[CI_WEAK_CRYPTO_FILE_AFTER_TIME_REG_ENTRY].Set != FALSE)
        {
            WeakCryptoPolicies->FileHashNotAfter = Config[i].Values[CI_WEAK_CRYPTO_FILE_AFTER_TIME_REG_ENTRY].LargeInteger;
        }
        else if (Default[i].Enabled != FALSE &&
                 Default[i].Values[CI_WEAK_CRYPTO_FILE_AFTER_TIME_REG_ENTRY].Set != FALSE)
        {
            WeakCryptoPolicies->FileHashNotAfter = Default[i].Values[CI_WEAK_CRYPTO_FILE_AFTER_TIME_REG_ENTRY].LargeInteger;
        }

        //
        // Timestamp hash's after time.
        //

        if (Config[i].Enabled != FALSE &&
            Config[i].Values[CI_WEAK_CRYPTO_TIMESTAMP_AFTER_TIME_REG_ENTRY].Set != FALSE)
        {
            WeakCryptoPolicies->TimestampHashNotAfter = Config[i].Values[CI_WEAK_CRYPTO_TIMESTAMP_AFTER_TIME_REG_ENTRY].LargeInteger;
        }
        else if (Default[i].Enabled != FALSE &&
                 Default[i].Values[CI_WEAK_CRYPTO_TIMESTAMP_AFTER_TIME_REG_ENTRY].Set != FALSE)
        {
            WeakCryptoPolicies->TimestampHashNotAfter = Default[i].Values[CI_WEAK_CRYPTO_TIMESTAMP_AFTER_TIME_REG_ENTRY].LargeInteger;
        }

        //
        // Allow list.
        //

        if (Config[i].Enabled != FALSE &&
            Config[i].Values[CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY].Set != FALSE)
        {
            if (Default[i].Enabled != FALSE &&
                 Default[i].Values[CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY].Set != FALSE)
            {
                //
                // Union both lists.
                //

                Status = OslpWeakCryptoConcatAllowLists(
                             Config[i].Values[CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY].Hashes,
                             Config[i].Values[CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY].HashCount,
                             Default[i].Values[CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY].Hashes,
                             Default[i].Values[CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY].HashCount,
                             &WeakCryptoPolicies->AllowList,
                             &WeakCryptoPolicies->AllowListCount);

                if (!NT_SUCCESS(Status))
                {
                    goto SetWeakCryptoPolicyEnd;
                }
            }
            else
            {
                //
                // Don't copy over, just use the parameter buffer.
                //

                WeakCryptoPolicies->AllowListCount =
                        Config[i].Values[CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY].HashCount;
                WeakCryptoPolicies->AllowList =
                        Config[i].Values[CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY].Hashes;

                Config[i].Values[CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY].HashCount = 0;
                Config[i].Values[CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY].Hashes = NULL;
            }
        }
        else if (Default[i].Enabled != FALSE &&
                 Default[i].Values[CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY].Set != FALSE)
        {
            //
            // Don't copy over, just use the parameter buffer.
            //

            WeakCryptoPolicies->AllowListCount =
                    Default[i].Values[CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY].HashCount;
            WeakCryptoPolicies->AllowList =
                    Default[i].Values[CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY].Hashes;

            Default[i].Values[CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY].HashCount = 0;
            Default[i].Values[CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY].Hashes = NULL;
        }

        //
        // Sort the list so that search will be more efficient.
        //

        if (WeakCryptoPolicies->AllowListCount > 0)
        {
            qsort(WeakCryptoPolicies->AllowList,
                  WeakCryptoPolicies->AllowListCount,
                  RTL_SHA256_HASH_LEN,
                  OslpSortCompareRoutineSHA256);
        }

        WeakCryptoPolicies++;
    }

    //
    // Set the policy.
    //

    OslWeakCryptoPolicies.Count = NumberOfPolicies;
    OslWeakCryptoPolicies.Policies = LoadWeakCryptoPolicy;

    MincryptSetWeakCryptoPolicy(&OslWeakCryptoPolicies);

SetWeakCryptoPolicyEnd:
    if (!NT_SUCCESS(Status)) {
        OslpFreeWeakCryptoPolicies();
    }

    for (i = 0; i < RTL_NUMBER_OF(Config); i++) {
        OslpFreeWeakCryptoValues(Config[i].Values);
        OslpFreeWeakCryptoValues(Default[i].Values);
    }

    return Status;
}

NTSTATUS
OslpLoadWeakCryptoPolicy(
    _In_ ULONG SystemHiveHandle,
    _Inout_updates_(CI_WEAK_CRYPTO_MAX_ALG)
        WEAK_CRYPTO_SETTINGS Default[CI_WEAK_CRYPTO_MAX_ALG],
    _Inout_updates_(CI_WEAK_CRYPTO_MAX_ALG)
        WEAK_CRYPTO_SETTINGS Config[CI_WEAK_CRYPTO_MAX_ALG]
    )

/*++

Routine Description:

    This routine load the weak crypto policy from the registry.

Arguments:

    SystemHiveHandle - Supplies the handle to the system hive.

    Default - Supplies a pointer that receives the default weak crypto policy.

    Config - Supplies a pointer that receives the user's weak crypto policy.

Return Value:

    STATUS_SUCCESS if successful.

--*/

{

    PCM_KEY_NODE CiCell;
    PWEAK_CRYPTO_SETTINGS CurrentSettings;
    ULONG ForEachAlgorithm;
    ULONG ForEachSettings;
    PCM_KEY_NODE SettingsCell;
    NTSTATUS Status;

    //
    // Open CI's Config key. The CI key must exist but the Config key may not.
    // Note that the "Default" key is under the Config key.
    //

    Status = OslGetCodeIntegrityCell(SystemHiveHandle, &CiCell);
    if (!NT_SUCCESS(Status)) {
        goto LoadWeakCryptoPolicyEnd;
    }

    Status = OslGetSubkey(SystemHiveHandle,
                          CiCell,
                          CI_WEAK_CRYPTO_POLICY_CONFIG_KEY_NAME,
                          &SettingsCell);

    if (!NT_SUCCESS(Status)) {
        Status = STATUS_SUCCESS;
        goto LoadWeakCryptoPolicyEnd;
    }

    //
    // First load any user config settings and then any defaults.
    //

    CurrentSettings = Config;

    for (ForEachSettings = 0;
         ForEachSettings < 2;
         ForEachSettings++)
    {
        for (ForEachAlgorithm = 0;
             ForEachAlgorithm < CI_WEAK_CRYPTO_MAX_ALG;
             ForEachAlgorithm++)
        {
            Status = OslpLoadWeakCryptoValues(
                        SystemHiveHandle,
                        SettingsCell,
                        LoadWeakCryptoValues[ForEachAlgorithm],
                        CurrentSettings[ForEachAlgorithm].Values);

            if (!NT_SUCCESS(Status)) {
                goto LoadWeakCryptoPolicyEnd;
            }

            //
            // Determine if the policy is enabled.
            //

            if (CurrentSettings[ForEachAlgorithm].Values[CI_WEAK_CRYPTO_FLAGS_REG_ENTRY].Set != FALSE &&
                (CurrentSettings[ForEachAlgorithm].Values[CI_WEAK_CRYPTO_FLAGS_REG_ENTRY].Options &
                MINCRYPT_CERT_CHAIN_ENABLE_WEAK_SETTINGS_FLAG)) {

                CurrentSettings[ForEachAlgorithm].Enabled = TRUE;
            }
        }

        if (ForEachSettings == 0) {
            Status = OslGetSubkey(SystemHiveHandle,
                                  SettingsCell,
                                  CI_WEAK_CRYPTO_POLICY_DEFAULT_CONFIG_KEY_NAME,
                                  &SettingsCell);

            if (!NT_SUCCESS(Status)) {
                Status = STATUS_SUCCESS;
                break;
            }

            CurrentSettings = Default;
        }
    }

LoadWeakCryptoPolicyEnd:
    return Status;
}

NTSTATUS
OslpLoadWeakCryptoValues (
    _In_ ULONG SystemHiveHandle,
    _In_ PCM_KEY_NODE SettingsCell,
    _In_reads_(CI_WEAK_CRYPTO_NUM_REG_ENTRIES)
        const WEAK_CRYPTO_VALUE_NAME Values[CI_WEAK_CRYPTO_NUM_REG_ENTRIES],
    _Inout_updates_(CI_WEAK_CRYPTO_NUM_REG_ENTRIES)
        WEAK_CRYPTO_SETTING Settings[CI_WEAK_CRYPTO_NUM_REG_ENTRIES]
    )

/*++

Routine Description:

    This routine loads a set of weak crypto policy settings.

Arguments:

    SystemHiveHandle - Supplies the handle to the system hive.

    SettingsCell - Supplies a pointer to the hive cell that contains the values
         to be read.

    Values - Supplies a pointer to the list of values to be read.

    Settings - Supplies a pointer to a list that receives the setting for a
         value.

Return Value:

    STATUS_SUCCESS if successful.

--*/

{

    PBYTE Buffer;
    ULONG i;
    NTSTATUS Status;
    ULONG Type;
    ULONG ValueLength;

    Buffer = NULL;

    for (i = 0; i < CI_WEAK_CRYPTO_NUM_REG_ENTRIES; i++) {

        if (Values[i].Value == NULL) {
            //
            // We must always reads the flags, but other values are not
            // required.
            //

            NT_ASSERT(i != CI_WEAK_CRYPTO_FLAGS_REG_ENTRY);
            continue;
        }

        switch (Values[i].Type) {
        case REG_DWORD:
            Status = OslGetDWordValue(SystemHiveHandle,
                                      SettingsCell,
                                      Values[i].Value,
                                      &Settings[i].Options);

            break;

        case REG_BINARY:
            ValueLength = sizeof(Settings[i].LargeInteger);
            Status = OslGetValue(SystemHiveHandle,
                                 SettingsCell,
                                 Values[i].Value,
                                 &Type,
                                 &ValueLength,
                                 (PBYTE)&Settings[i].LargeInteger);

            if (NT_SUCCESS(Status)) {
                if (Type != REG_BINARY ||
                    ValueLength != sizeof(Settings[i].LargeInteger)) {

                    Status = STATUS_OBJECT_TYPE_MISMATCH;
                }
            }

            break;

        case REG_MULTI_SZ:
            Status = OslGetCopyOfValue(SystemHiveHandle,
                                       SettingsCell,
                                       Values[i].Value,
                                       &Type,
                                       &Buffer,
                                       &ValueLength);

            if (NT_SUCCESS(Status)) {
                if (Type != REG_MULTI_SZ ||
                    i != CI_WEAK_CRYPTO_ALLOW_LIST_REG_ENTRY) {

                    //
                    // We only have one REG_MULTI_SZ so this must be the allow
                    // list.
                    //

                    Status = STATUS_OBJECT_TYPE_MISMATCH;
                }
                else {
                    Status = OslpRegMultiSzToThumbprintArray(
                                Buffer,
                                ValueLength,
                                &Settings[i].Hashes,
                                &Settings[i].HashCount);
                }

                BlMmFreeHeap(Buffer);
                Buffer = NULL;
            }

            break;

        default:
            Status = STATUS_NOT_SUPPORTED;
            break;
        }

        if (!NT_SUCCESS(Status)) {
            if (Status == STATUS_NOT_FOUND) {

                //
                // If we could not find the flags, no need to read the rest.
                //

                if (i == CI_WEAK_CRYPTO_FLAGS_REG_ENTRY) {
                    break;
                }

                continue;
            }

            goto LoadWeakCryptoValuesEnd;
        }

        Settings[i].Type = Values[i].Type;
        Settings[i].Set = TRUE;
    }

    Status = STATUS_SUCCESS;

LoadWeakCryptoValuesEnd:
    return Status;
}

VOID
OslpFreeWeakCryptoValues(
    _Inout_updates_(CI_WEAK_CRYPTO_NUM_REG_ENTRIES)
        WEAK_CRYPTO_SETTING Settings[CI_WEAK_CRYPTO_NUM_REG_ENTRIES]
    )

/*++

Routine Description:

    This routine releases resources allocated for weak crypto policy loaded
    from registry.

Arguments:

    Settings - Supplies a pointer to registry parameters.

Return Value:

    None.

--*/
{
    ULONG i;

    for (i = 0; i < CI_WEAK_CRYPTO_NUM_REG_ENTRIES; i++) {

        if (Settings[i].Type == REG_MULTI_SZ && Settings[i].Hashes != NULL) {
            BlMmFreeHeap(Settings[i].Hashes);
        }

        RtlZeroMemory(&Settings[i], sizeof(Settings[i]));
    }
}

NTSTATUS
OslpRegMultiSzToThumbprintArray(
    _In_reads_(MultiSzSize) PVOID MultiSz,
    _In_ ULONG MultiSzSize,
    _Outptr_result_buffer_(*ThumbprintCount) UCHAR (**Thumbprints)[RTL_SHA256_HASH_LEN],
    _Out_ PULONG ThumbprintCount
    )

/*++

Routine Description:

    This routine takes a list of hex strings of SHA256 thumbprints, and convert
    it to an array of equivalent binary values.

Arguments:

    MultiSz - Supplies a pointer to the reg key value of type REG_MULTI_SZ,
         which contains hex encoded strings of SHA256 thumbprints.

    MultiSzSize - Supplies the length in bytes of the MultiSz buffer.

    Thumbprints - Receives a pointer to an array of SHA256 thumbprints, each
         initialized with a copy of one of the strings in the list.

    ThumbprintCount - Receives the number of elements written to the thumbprint
         array.

Return Value:

    STATUS_SUCCESS if successful.

    Other status codes if unsuccessful.

--*/
{

    PWCHAR BufferEnd;
    ULONG EncodedLength = 0;
    ULONG i;
    PCWSTR p;
    ULONG RemainingLength = 0;
    NTSTATUS Status;
    ULONG StringCount = 0;
    UCHAR (*ThumbprintArray)[RTL_SHA256_HASH_LEN] = NULL;

    //
    // Init out params.
    //

    *Thumbprints = NULL;
    *ThumbprintCount = 0;

    //
    // Since it is unicode string, so it cannot be odd length.
    //

    NT_ASSERT(MultiSzSize <= MINCRYPT_WEAK_CRYPTO_POLICY_MAX_BYTES);

    if ((MultiSzSize & 1) != 0) {
       Status = STATUS_INVALID_PARAMETER_1;
       goto RegMultiSzToThumbprintArrayEnd;
    }

    //
    // Make a preliminary pass through the buffer to count the number of
    // strings.
    //

    p = (PCWSTR) MultiSz;
    BufferEnd = (PWCHAR) Add2Ptr(p, MultiSzSize);
    RemainingLength = MultiSzSize / sizeof(WCHAR);

    while (p < BufferEnd && *p) {
        //
        // Make sure string length is as expected.
        //

        EncodedLength = (ULONG) wcsnlen(p, RemainingLength);

        //
        // Two chars for hex per binary byte.
        //

        if (EncodedLength != (RTL_SHA256_HASH_LEN * 2)) {
            Status = STATUS_BAD_DATA;
            goto RegMultiSzToThumbprintArrayEnd;
        }

        StringCount++;
        p += EncodedLength + 1;
        RemainingLength -= EncodedLength + 1;
    }

    //
    // Last char should be the double null.
    //

    if (RemainingLength != 1) {
        //
        // Treated as error if we didn't process the entire buffer.
        //

        Status = STATUS_BAD_DATA;
        goto RegMultiSzToThumbprintArrayEnd;
    }

    //
    // We are done if no string found.
    //

    if (StringCount == 0) {
        Status = STATUS_SUCCESS;
        goto RegMultiSzToThumbprintArrayEnd;
    }

    //
    // Allocate memory for array.
    //

    ThumbprintArray = BlMmAllocateHeap(StringCount * RTL_SHA256_HASH_LEN);
    if (ThumbprintArray == NULL) {
        Status = STATUS_NO_MEMORY;
        goto RegMultiSzToThumbprintArrayEnd;
    }

    //
    // Second pass to convert hex to binary into array.
    //
    // At this point, we are sure all strings are in correct length.
    //

    p = (PCWSTR)MultiSz;

    for (i = 0; i < StringCount; i++) {
        NT_ASSERT(p < BufferEnd);

        Status = OslpHexDecodeSha256Thumbprint(p, ThumbprintArray[i]);
        if (!NT_SUCCESS(Status)) {
            goto RegMultiSzToThumbprintArrayEnd;
        }

        p += EncodedLength + 1;
    }

    *Thumbprints = ThumbprintArray;
    *ThumbprintCount = StringCount;

    Status = STATUS_SUCCESS;

RegMultiSzToThumbprintArrayEnd:
    if (!NT_SUCCESS(Status) && ThumbprintArray != NULL) {
        BlMmFreeHeap(ThumbprintArray);
    }

    return Status;
}

UCHAR
OslpHexDecodeCharW(
    _In_ WCHAR ch
    )

/*++

Routine Description:

    This routine decodes a hex character.

Arguments:

    ch - Supplies the hex character to be decoded.

Return Value:

    Decoded hex value.

--*/
{

    if ((ch >= L'0') && (ch <= L'9')) {
        return (UCHAR)(ch - L'0');
    }

    ch |= 0x20;

    if ((ch >= L'a') && (ch <= L'f')) {
        return (UCHAR)(ch - (L'a' - 10));
    }

    return (UCHAR) -1;
}

NTSTATUS
OslpHexDecodeSha256Thumbprint(
    _In_reads_(RTL_SHA256_HASH_LEN * 2) PCWCHAR EncodedData,
    _Out_writes_(RTL_SHA256_HASH_LEN) PUCHAR DecodedData
    )

/*++

Routine Description:

    This routine hex decodes a SHA256 thumprint.

Arguments:

    EncodedData - Supplies the hex encoded SHA256 thumbprint.

    DecodedData - Supplies a pointer to receive the decoded thumbprint value.

Return Value:

    STATUS_SUCCESS if successful.

    Other status codes if unsuccessful.

--*/
{

    UCHAR hi, lo;
    ULONG i;
    NTSTATUS Status;

    for (i = 0; i < RTL_SHA256_HASH_LEN; i++) {
        hi = OslpHexDecodeCharW(*EncodedData++);
        lo = OslpHexDecodeCharW(*EncodedData++);

        if ((hi | lo) > 0x0F) {
            Status = STATUS_BAD_DATA;
            goto HexDecodeSha256ThumbprintEnd;
        }

        DecodedData[i] = (UCHAR)((hi << 4) | lo);
    }

    Status = STATUS_SUCCESS;

HexDecodeSha256ThumbprintEnd:
   return Status;
}

NTSTATUS
OslpWeakCryptoConcatAllowLists(
    _In_reads_(ListCount1) UCHAR (*List1)[RTL_SHA256_HASH_LEN],
    _In_ ULONG ListCount1,
    _In_reads_(ListCount2) UCHAR (*List2)[RTL_SHA256_HASH_LEN],
    _In_ ULONG ListCount2,
    _Outptr_result_buffer_(*MergedCount) UCHAR (**MergedList)[RTL_SHA256_HASH_LEN],
    _Out_ PULONG MergedCount
    )

/*++

Routine Description:

    This routine concatenates two SHA256 thumbprint allow lists into one.

Arguments:

    List1 - Supplies a pointer to the first list.

    List2 - Supplies a pointer to the second list.

    MergedList - Supplies a pointer to receive a pointer to the merged list.

    MergedCount - Supplies a pointer to receive the merged count.

Return Value:

    STATUS_SUCCESS if successful.

    Other status codes if unsuccessful.

--*/
{

    ULONG Count;
    UCHAR (*List)[RTL_SHA256_HASH_LEN];
    ULONG Size;
    NTSTATUS Status;

    Status = RtlULongAdd(ListCount1, ListCount2, &Count);
    if (!NT_SUCCESS(Status)) {
        goto WeakCryptoConcatAllowListsEnd;
    }

    Status = RtlULongMult(Count, RTL_SHA256_HASH_LEN, &Size);
    if (!NT_SUCCESS(Status)) {
        goto WeakCryptoConcatAllowListsEnd;
    }

    List = BlMmAllocateHeap(Size);
    if (List == NULL) {
        Status = STATUS_NO_MEMORY;
        goto WeakCryptoConcatAllowListsEnd;
    }

    RtlCopyMemory(List, List1, ListCount1 * RTL_SHA256_HASH_LEN);
    RtlCopyMemory(&List[ListCount1], List2, ListCount2 * RTL_SHA256_HASH_LEN);

    *MergedList = List;
    *MergedCount = Count;

    Status = STATUS_SUCCESS;

WeakCryptoConcatAllowListsEnd:
    return Status;
}

int
__cdecl
OslpSortCompareRoutineSHA256(
    _In_reads_bytes_(RTL_SHA256_HASH_LEN) const void *Element1,
    _In_reads_bytes_(RTL_SHA256_HASH_LEN) const void *Element2
    )

/*++

Routine Description:

    This routine compares two SHA256 hashes, returning the difference.

Arguments:

    Element1 - Supplies a pointer to the first hash to compare.

    Element1 - Supplies a pointer to the second hash to compare.

Return Value:

    Difference between the hashes.

--*/

{
    return memcmp(Element1, Element2, RTL_SHA256_HASH_LEN);
}

VOID
OslpFreeWeakCryptoPolicies(
    VOID
    )

/*++

Routine Description:

    This routine frees all weak crypto policies.

Arguments:

    None.

Return Value:

    None.

--*/

{

    ULONG i;

    for (i = 0; i < RTL_NUMBER_OF(LoadWeakCryptoPolicy); i++) {
        if (LoadWeakCryptoPolicy[i].AllowList != NULL) {
            BlMmFreeHeap(LoadWeakCryptoPolicy[i].AllowList);
        }
    }

    RtlZeroMemory(&LoadWeakCryptoPolicy, sizeof(LoadWeakCryptoPolicy));
    RtlZeroMemory(&OslWeakCryptoPolicies, sizeof(OslWeakCryptoPolicies));
}

ULONG
OslGetWeakCryptoPolicySerializedSize (
    VOID
    )

/*++

Routine Description:

    This routine calculates the size of the serialized weak crypto policies.

Arguments:

    None.

Return Value:

    Size in bytes of the serialized policies.

--*/

{

    ULONG i;
    ULONG Size;

    Size = 0;

    //
    // The serialized format is:
    // For each Policy in PolicyCount
    // MINCRYPT_WEAK_CRYPTO_POLICY Policy;
    // UCHAR AllowList[Policy.AllowListCount][RTL_SHA256_HASH_LEN];
    //

    for (i = 0; i < OslWeakCryptoPolicies.Count; i++) {
        Size += sizeof(MINCRYPT_WEAK_CRYPTO_POLICY);
        Size += OslWeakCryptoPolicies.Policies[i].AllowListCount *
                    RTL_SHA256_HASH_LEN;
    }

    return Size;
}

NTSTATUS
OslSerializeWeakCryptoPolicy (
    _Out_writes_bytes_to_(*BufferSize, *BufferSize) PVOID Buffer,
    _Inout_ PULONG BufferSize
    )

/*++

Routine Description:

    This routine serializes the weak crypto policies.

Arguments:

    Buffer - Supplies a pointer to a buffer that receives the serialized weak
         crypto policies.

    BufferSize - Supplies a pointer that on input contains the size in bytes of
         the serialization buffer and on return contains the size in bytes of
         the policy written.

Return Value:

    STATUS_SUCCESS if successful.

    STATUS_INVALID_BUFFER_SIZE if the buffer is not large enough.

--*/

{
    ULONG Available;
    ULONG i;
    ULONG ListSize;
    NTSTATUS Status;

    Available = *BufferSize;

    NT_ASSERT(Available == OslGetWeakCryptoPolicySerializedSize());

    for (i = 0; i < OslWeakCryptoPolicies.Count; i++) {

        if (Available < sizeof(MINCRYPT_WEAK_CRYPTO_POLICY)) {
            Status = STATUS_INVALID_BUFFER_SIZE;
            goto SerializeWeakCryptoPolicyEnd;
        }

        RtlCopyMemory(Buffer,
                      &OslWeakCryptoPolicies.Policies[i],
                      sizeof(MINCRYPT_WEAK_CRYPTO_POLICY));

        Buffer = Add2Ptr(Buffer, sizeof(MINCRYPT_WEAK_CRYPTO_POLICY));
        Available -= sizeof(MINCRYPT_WEAK_CRYPTO_POLICY);

        if (OslWeakCryptoPolicies.Policies[i].AllowListCount > 0) {

            ListSize = OslWeakCryptoPolicies.Policies[i].AllowListCount *
                            RTL_SHA256_HASH_LEN;

            if (Available < ListSize) {
                Status = STATUS_INVALID_BUFFER_SIZE;
                goto SerializeWeakCryptoPolicyEnd;
            }

            RtlCopyMemory(Buffer,
                          OslWeakCryptoPolicies.Policies[i].AllowList,
                          ListSize);

            Buffer = Add2Ptr(Buffer, ListSize);
            Available -= ListSize;
        }
    }

    *BufferSize -= Available;
    Status = STATUS_SUCCESS;

SerializeWeakCryptoPolicyEnd:
    return Status;
}

NTSTATUS
OslpLoadWhqlRegSettings (
    _In_ ULONG SystemHiveHandle,
    _Out_ PULONG Settings
    )

/*++

Routine Description:

    This routine loads WHQL settings from registry.

Arguments:

    SystemHiveHandle - Supplies the handle to the system hive.

    Settings - Supplies a pointer that receives the settings.

Return Value:

    STATUS_SUCCESS if successful.

--*/

{
    PCM_KEY_NODE CiCell;
    DWORD LocalDeveloperTestMode;
    DWORD LocalSettings;
    DWORD LocalUpgradedSystem;
    PCM_KEY_NODE PolicyCell;
    NTSTATUS Status;

    //
    // Open CI key.
    //

    Status = OslGetCodeIntegrityCell(SystemHiveHandle, &CiCell);
    if (!NT_SUCCESS(Status)) {
        goto LoadRegWhqlSettingsEnd;
    }

    //
    // Open CI\Policy sub-key.
    //

    Status = OslGetSubkey(SystemHiveHandle,
                          CiCell,
                          CI_WHQL_POLICY_REG_SUBKEY_NAME,
                          &PolicyCell);
    if (!NT_SUCCESS(Status)) {
        goto LoadRegWhqlSettingsEnd;
    }

    //
    // Read UpgradedSystem value.
    //

    Status = OslGetDWordValue(SystemHiveHandle,
                              PolicyCell,
                              CI_WHQL_UPGRADED_SYSTEM_REG_VALUE_NAME,
                              &LocalUpgradedSystem);
    if (!NT_SUCCESS(Status)) {
        LocalUpgradedSystem = 0;
    }
    else {
        //
        // Value is immaterial. Transform to settings flag.
        //

        LocalUpgradedSystem = CI_WHQL_REG_SETTINGS_UPGRADED_SYSTEM;
    }

    //
    // Read WhqlDeveloperTestMode value.
    //

    Status = OslGetDWordValue(SystemHiveHandle,
                              PolicyCell,
                              CI_WHQL_DEVELOPER_TEST_MODE_REG_VALUE_NAME,
                              &LocalDeveloperTestMode);
    if (!NT_SUCCESS(Status) || LocalDeveloperTestMode != 1) {
        LocalDeveloperTestMode = 0;
    }
    else {
        //
        // Transform to settings flag.
        //

        LocalDeveloperTestMode = CI_WHQL_REG_DEVELOPER_TEST_MODE_ENABLED;
    }

    //
    // Read WhqlSettings value.
    //

    Status = OslGetDWordValue(SystemHiveHandle,
                              PolicyCell,
                              CI_WHQL_SETTINGS_REG_VALUE_NAME,
                              &LocalSettings);
    if (!NT_SUCCESS(Status)) {
        LocalSettings = 0;
        Status = STATUS_SUCCESS;
    }

    *Settings = LocalUpgradedSystem | LocalDeveloperTestMode | LocalSettings;

LoadRegWhqlSettingsEnd:
    return Status;
}

NTSTATUS
OslpLoadSysDevWhqlEnforcementDateTime (
    _In_ ULONG SystemHiveHandle,
    _Out_ PLARGE_INTEGER EnforceDateTime
    )

/*++

Routine Description:

    This routine loads the SysDev WHQL signing enforcement date/time from the
    registry.

Arguments:

    SystemHiveHandle - Supplies the handle to the system hive.

    EnforceDateTime - Supplies a pointer that receives the SysDev WHQL signing
        enforcement date/time set by the admin.

Return Value:

    STATUS_SUCCESS if successful.

--*/

{
    PCM_KEY_NODE CiCell;
    LARGE_INTEGER LocalEnforceDateTime;
    PCM_KEY_NODE PolicyCell;
    NTSTATUS Status;
    ULONG Type;
    ULONG ValueLength;

    //
    // Open CI key.
    //

    Status = OslGetCodeIntegrityCell(SystemHiveHandle, &CiCell);
    if (!NT_SUCCESS(Status)) {
        goto LoadSysDevWhqlEnforcementDateTimeEnd;
    }

    Status = OslGetSubkey(SystemHiveHandle,
                          CiCell,
                          CI_WHQL_POLICY_REG_SUBKEY_NAME,
                          &PolicyCell);
    if (!NT_SUCCESS(Status)) {
        goto LoadSysDevWhqlEnforcementDateTimeEnd;
    }

    ValueLength = sizeof(LARGE_INTEGER);
    Status = OslGetValue(SystemHiveHandle,
                         PolicyCell,
                         CI_WHQL_ENFORCEMENT_DATETIME_REG_VALUE_NAME,
                         &Type,
                         &ValueLength,
                         (PBYTE) &LocalEnforceDateTime);
    if (!NT_SUCCESS(Status)) {
        goto LoadSysDevWhqlEnforcementDateTimeEnd;
    }

    if (Type != REG_BINARY || ValueLength != sizeof(LARGE_INTEGER)) {
        Status = STATUS_OBJECT_TYPE_MISMATCH;
        goto LoadSysDevWhqlEnforcementDateTimeEnd;
    }

    *EnforceDateTime = LocalEnforceDateTime;

LoadSysDevWhqlEnforcementDateTimeEnd:
    return Status;
}

VOID
OslpSetSysDevWhqlPolicy (
    _In_ ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine sets the SysDev WHQL signing policy.

Arguments:

    SystemHiveHandle - Supplies the handle to the system hive.

Return Value:

    None.

--*/

{
    PLARGE_INTEGER EnforceDateTime;
    LARGE_INTEGER EnforceDateTimeValue;
    BOOLEAN IsSystemUpgradeInProgress;
    BOOLEAN IsWinPE;
    ULONG Settings;
    NTSTATUS Status;

    IsSystemUpgradeInProgress = OslIsSystemUpgradeInProgress(SystemHiveHandle);

    Status = BlGetApplicationOptionBoolean(BCDE_OSLOADER_TYPE_WINPE, &IsWinPE);
    if (!NT_SUCCESS(Status)) {
        IsWinPE = FALSE;
    }

    Status = OslpLoadWhqlRegSettings(SystemHiveHandle, &Settings);
    if (!NT_SUCCESS(Status)) {
        Settings = 0;
    }

    EnforceDateTime = NULL;

    if (CHECK_FLAG(Settings, CI_WHQL_REG_SETTINGS_UPGRADED_SYSTEM) == FALSE) {
        Status = OslpLoadSysDevWhqlEnforcementDateTime(SystemHiveHandle,
                                                       &EnforceDateTimeValue);
        if (NT_SUCCESS(Status)) {
            EnforceDateTime = &EnforceDateTimeValue;
        }
    }

    (VOID)BlImgSetSysDevWhqlPolicy(IsSystemUpgradeInProgress,
                                   IsWinPE,
                                   Settings,
                                   EnforceDateTime);

    return;
}

NTSTATUS
OslpOpenRevocationList(
    _In_ DEVICE_ID OsDeviceId,
    _Out_ PFILE_ID RevocationListFileId,
    _Out_ PULONG RevocationListSize
    )

/*++

Routine Description:

    This routine opens the Code Integrity revocation list and queries its size.

Arguments:

    OsDeviceId - Supplies a handle to the device containing the OS.

    RevocationListFileId - Supplies a pointer that receives a file handle to
         the revocation list.

    RevocationListSize - Supplies a pointer that receives the size in bytes of
         the revocation list.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_FILE_INVALID if the file is of zero length or too large.

    Another error status code if returned from a sub-routine.

--*/

{
    FILE_ID FileId;
    FILE_INFORMATION FileInfo;
    UNICODE_STRING RevocationListPath;
    ULONG Size;
    NTSTATUS Status;
    BOOLEAN Succeeded;

    FileId = INVALID_FILE_ID;
    RtlInitUnicodeString(&RevocationListPath, NULL);

    //
    // Create the path to the revocation list and query its size.
    //

    Succeeded = BlAppendUnicodeToString(&RevocationListPath, OslSystemRoot);
    if (Succeeded == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OpenRevocationListEnd;
    }

    Succeeded = BlAppendUnicodeToString(
                    &RevocationListPath,
                    CI_REVOCATION_LIST_PATH CI_REVOCATION_LIST_NAME);

    if (Succeeded == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OpenRevocationListEnd;
    }

    Status = BlFileOpen(OsDeviceId,
                        RevocationListPath.Buffer,
                        OPEN_READ,
                        &FileId);

    if (!NT_SUCCESS(Status)) {
        goto OpenRevocationListEnd;
    }

    Status = BlFileGetInformation(FileId, &FileInfo);
    if (!NT_SUCCESS(Status)) {
        goto OpenRevocationListEnd;
    }

    Status = RtlULongLongToULong(FileInfo.FileSize, &Size);
    if (!NT_SUCCESS(Status) || (Size == 0)) {
        Status = STATUS_FILE_INVALID;
        goto OpenRevocationListEnd;
    }

    *RevocationListSize = Size;
    *RevocationListFileId = FileId;
    FileId = INVALID_FILE_ID;

OpenRevocationListEnd:

    if (FileId != INVALID_FILE_ID) {
        BlFileClose(FileId);
    }

    if (RevocationListPath.Buffer != NULL) {
        BlMmFreeHeap(RevocationListPath.Buffer);
    }

    return Status;
}

NTSTATUS
OslpLoadAndAttestRevocationList(
    _In_ FILE_ID RevocationListFileId,
    _Out_writes_bytes_(RevocationListSize) PVOID RevocationList,
    _In_ ULONG RevocationListSize
    )

/*++

Routine Description:

    This routine loads and attests the Code Integrity revocation list.

Arguments:

    RevocationListFileId - Supplies a handle to the revocation list.

    RevocationList - Supplies a pointer to a buffer to load the revocation list.

    RevocationListSize - Supplies the size in bytes of the revocation list.

Return Value:

    STATUS_SUCCESS when successful.

    Another error status code if returned from a sub-routine.

--*/

{
    PVOID ParsedRevocationList;
    LARGE_INTEGER ParsedRevocationListCreationTime;
    ULONG ParsedRevocationListSize;
    NTSTATUS Status;

    Status = BlFileReadAtOffset(RevocationListFileId,
                                RevocationListSize,
                                0,
                                RevocationList,
                                NULL);

    if (!NT_SUCCESS(Status)) {
        goto LoadAndAttestRevocationListEnd;
    }

    //
    // Attest the revocation list.
    //

    Status = BlImgParseOsRevocationList(RevocationList,
                                        RevocationListSize,
                                        &ParsedRevocationList,
                                        &ParsedRevocationListSize,
                                        &ParsedRevocationListCreationTime);

    if (!NT_SUCCESS(Status)) {
        goto LoadAndAttestRevocationListEnd;
    }

    BlSiMeasureOsRevocationList(ParsedRevocationList,
                                ParsedRevocationListSize,
                                &ParsedRevocationListCreationTime);

LoadAndAttestRevocationListEnd:
    return Status;
}

ULONG
OslpBuildCodeIntegrityOptions(
    VOID
    )
/*++

Routine Description:

    This routine determines the set of Code Integrity option flags that
    represent the configuration of Code Integrity for this boot.

Arguments:

    None.

Return Value:

    A set of Code Integrity option flags.

--*/

{
    ULONG CodeIntegrityOptions;
    BOOLEAN FlightSigningEnabled;
    BOOLEAN IntegrityChecksDisabled;
    BOOLEAN TestSigningEnabled;

    CodeIntegrityOptions = 0;

    //
    // Query the Code Integrity options from BCD.
    //

    BlImgQueryCodeIntegrityBootOptions(BlGetApplicationEntry(),
                                       &IntegrityChecksDisabled,
                                       &TestSigningEnabled,
                                       &FlightSigningEnabled);


    if (IntegrityChecksDisabled == FALSE) {
        CodeIntegrityOptions |= CI_OPTION_FAIL_UNSIGNED_DRIVERS |
                                CI_OPTION_FAIL_UNSIGNED_CRYPTO_IMAGES;
    }

    if (TestSigningEnabled != FALSE) {
        CodeIntegrityOptions |= CI_OPTION_ALLOW_TEST_SIGNING;
    }

    if (FlightSigningEnabled != FALSE) {
        CodeIntegrityOptions |= CI_OPTION_ALLOW_FLIGHT_SIGNING;
    }

    //
    // WHQL enforcement settings.
    //

    if (BlImgIsUpgradedPlatform() != FALSE) {
        CodeIntegrityOptions |= CI_OPTION_SYSTEM_IS_UPGRADED_PLATFORM;
    }

    if (BlImgIsWhqlDeveloperTestModeEnabled() != FALSE) {
        CodeIntegrityOptions |= CI_OPTION_WHQL_DEVELOPER_TEST_MODE_ENABLED;
    }

    if (BlImgIsWhqlDisabledBySetting() != FALSE) {
        CodeIntegrityOptions |= CI_OPTION_WHQL_DISABLED_BY_SETTING;
    }

    if (BlImgIsWhqlEnabledBySetting() != FALSE) {
        CodeIntegrityOptions |= CI_OPTION_WHQL_ENABLED_BY_SETTING;
    }

    return CodeIntegrityOptions;
}

NTSTATUS
OslBuildCodeIntegrityLoaderBlock (
    _In_ DEVICE_ID OsDeviceId,
    _Inout_ PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine loads (and attests as necessary) the data required for Code
    Integrity in the runtime OS. This data is used by both NT and Secure
    Kernel.

Arguments:

    OsDeviceId - Supplies a handle to the device containing the OS.

    LoaderBlock - Supplies a pointer to the NT loader block that receives the
         Code Integrity configuration to pass up to the OS.

Return Value:

    STATUS_SUCCESS when successful.

    Another error status code if returned from a sub-routine.

--*/

{

    PVOID Buffer;
    PLOADER_PARAMETER_CI_EXTENSION Data;
    FILE_ID FileId;
    ULONG Pages;
    PVOID Policy;
    BAPD_ITEM_DESCRIPTOR PolicyDescriptor;
    ULONG PolicySize;
    ULONG RevocationListSize;
    ULONG SecureBootSize;
    ULONG Size;
    NTSTATUS Status;
    ULONG WeakCryptoSize;

    Data = NULL;
    FileId = INVALID_FILE_ID;
    Policy = NULL;
    RevocationListSize = 0;
    Size = FIELD_OFFSET(LOADER_PARAMETER_CI_EXTENSION, SerializedData);

    //
    // Query the size of the revocation list.
    //

    Status = OslpOpenRevocationList(OsDeviceId, &FileId, &RevocationListSize);
    if (!NT_SUCCESS(Status)) {
        goto BuildCodeIntegrityLoaderBlockEnd;
    }

    Status = RtlULongAdd(Size, RevocationListSize, &Size);
    if (!NT_SUCCESS(Status)) {
        goto BuildCodeIntegrityLoaderBlockEnd;
    }

    //
    // Acquire the CI policy if it exists.
    //

    Status = BlSIPolicyAcquireCurrentPolicy(&PolicyDescriptor,
                                            &Policy,
                                            &PolicySize);

    if (!NT_SUCCESS(Status)) {
        if (Status != STATUS_NOT_FOUND) {
            goto BuildCodeIntegrityLoaderBlockEnd;
        }

        Policy = NULL;
        PolicySize = 0;
        Status = STATUS_SUCCESS;
    }
    else
    {
        Status = RtlULongAdd(Size, PolicySize, &Size);
        if (!NT_SUCCESS(Status)) {
            goto BuildCodeIntegrityLoaderBlockEnd;
        }
    }

    //
    // Query the Weak Crypto Policy size.
    //

    WeakCryptoSize = OslGetWeakCryptoPolicySerializedSize();
    Status = RtlULongAdd(Size, WeakCryptoSize, &Size);
    if (!NT_SUCCESS(Status)) {
        goto BuildCodeIntegrityLoaderBlockEnd;
    }

    //
    // Query the Secure Boot policy size.
    //

    Status = BlSecureBootGetSizeOfKernelPolicyPackage(&SecureBootSize);
    if (!NT_SUCCESS(Status)) {
        goto BuildCodeIntegrityLoaderBlockEnd;
    }

    Status = RtlULongAdd(Size, SecureBootSize, &Size);
    if (!NT_SUCCESS(Status)) {
        goto BuildCodeIntegrityLoaderBlockEnd;
    }

    //
    // Allocate and fill in the code integrity data block.
    //

    Pages = BYTES_TO_PAGES(Size);
    Status = BlMmAllocatePages(&Data,
                               Pages,
                               OSL_MEMORY_TYPE_MISC_MODULE,
                               MEMORY_ATTRIBUTE_ALLOCATION_KSEG,
                               0);

    if (!NT_SUCCESS(Status)) {
        goto BuildCodeIntegrityLoaderBlockEnd;
    }

    RtlZeroMemory(Data, FIELD_OFFSET(LOADER_PARAMETER_CI_EXTENSION, SerializedData));
    Buffer = &Data->SerializedData[0];

    //
    // Load and attest the revocation list.
    //

    Status = OslpLoadAndAttestRevocationList(FileId,
                                             Buffer,
                                             RevocationListSize);

    if (!NT_SUCCESS(Status)) {
        goto BuildCodeIntegrityLoaderBlockEnd;
    }

    Buffer = Add2Ptr(Buffer, RevocationListSize);

    //
    // Copy the policies.
    //

    if (PolicySize > 0) {
        RtlCopyMemory(Buffer, Policy, PolicySize);
        Buffer = Add2Ptr(Buffer, PolicySize);
    }

    if (WeakCryptoSize > 0) {
        Status = OslSerializeWeakCryptoPolicy(Buffer, &WeakCryptoSize);
        if (!NT_SUCCESS(Status)) {
            goto BuildCodeIntegrityLoaderBlockEnd;
        }

        Buffer = Add2Ptr(Buffer, WeakCryptoSize);
    }

    if (SecureBootSize > 0) {
        Status = BlSecureBootGetKernelPolicyPackage(Buffer, SecureBootSize);
        if (!NT_SUCCESS(Status)) {
            goto BuildCodeIntegrityLoaderBlockEnd;
        }

        Buffer = Add2Ptr(Buffer, SecureBootSize);
    }

    NT_ASSERT(((ULONG_PTR)Buffer - (ULONG_PTR)Data) == Size);

    //
    // Fill in the options, offsets and sizes.
    //

    Data->CodeIntegrityOptions = OslpBuildCodeIntegrityOptions();
    Data->IsWinPE = BlImgIsWinPE();
    Data->UpgradeInProgress = BlImgIsUpgradeInProgress();

    BlImgGetWhqlEnforcementDateTime(&Data->WhqlEnforcementDate);

    Data->RevocationListOffset = 0;
    Data->RevocationListSize = RevocationListSize;

    Data->CodeIntegrityPolicyOffset = Data->RevocationListOffset + Data->RevocationListSize;
    Data->CodeIntegrityPolicySize = PolicySize;

    Data->WeakCryptoPolicyLoadStatus = OslWeakCryptoStatus;
    Data->WeakCryptoPolicyOffset = Data->CodeIntegrityPolicyOffset + Data->CodeIntegrityPolicySize;
    Data->WeakCryptoPolicySize = WeakCryptoSize;

    Data->SecureBootPolicyOffset = Data->WeakCryptoPolicyOffset + Data->WeakCryptoPolicySize;
    Data->SecureBootPolicySize = SecureBootSize;

    LoaderBlock->Extension->CodeIntegrityDataSize = Size;
    LoaderBlock->Extension->CodeIntegrityData = Data;
    Data = NULL;

BuildCodeIntegrityLoaderBlockEnd:
    if (Data != NULL) {
        BlMmFreePages(Data);
    }

    if (Policy != NULL) {
        BlSIPolicyReleaseCurrentPolicy(&PolicyDescriptor);
    }

    if (FileId != INVALID_FILE_ID) {
        BlFileClose(FileId);
    }

    return Status;

}

SE_SIGNING_LEVEL
OslpDetermineSeILSigningPolicy(
    _In_ ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine determines the signing level that should be enforced by NT for
    the SeILSigningPolicy. This function provides the equivalent functionality
    implemented by nt!SeCodeIntegrityInitializePolicy.

Arguments:

    SystemHiveHandle - Supplies the handle to the system hive.

Return Value:

    The signing level that should be enforced for SeILSigningPolicy.

--*/

{
    PCM_KEY_NODE CiCell;
    SYSTEM_INTEGRITY_POLICY_HANDLE PolicyHandle;
    BOOLEAN SecureBoot;
    SE_SIGNING_LEVEL SigningLevel;
    NTSTATUS Status;
    DWORD UmciDisabled;

    SigningLevel = SE_SIGNING_LEVEL_PLATFORM_DEFAULT;

    //
    // Should UMCI be enabled by the Secure Boot Policy?
    //

#if SE_IS_PLATFORM_LOCKED_DOWN_BY_DEFAULT() == FALSE
    if (BlSecureBootArePolicyOptionsSet(SECUREBOOT_POLICY_ENABLE_UMCI) != FALSE) {
        SigningLevel = SE_SIGNING_LEVEL_RUNTIME_UMCI_DEFAULT;
    }
#endif

    //
    // Allow the signing level to be lowered if either secure boot is disabled
    // or the policy allows UMCI debugging options.
    //

    if (SigningLevel != SE_SIGNING_LEVEL_UNCHECKED) {

        Status = BlSecureBootIsEnabled(&SecureBoot);
        if ((!NT_SUCCESS(Status) || (SecureBoot == FALSE)) ||
            (BlSecureBootArePolicyOptionsSet(
                SECUREBOOT_POLICY_ALLOW_UMCI_DEBUG_OPTIONS) != FALSE)) {

            //
            // Check if the disablement reg value is set.
            //

            Status = OslGetCodeIntegrityCell(SystemHiveHandle, &CiCell);
            if (NT_SUCCESS(Status)) {
                Status = OslGetDWordValue(SystemHiveHandle,
                                          CiCell,
                                          CI_OPTION_UMCI_DISABLED_VALUE,
                                          &UmciDisabled);

                if (NT_SUCCESS(Status) && (UmciDisabled == 1)) {
                    SigningLevel = SE_SIGNING_LEVEL_UNCHECKED;
                }
            }
        }
    }

    //
    // Check if a CI policy has enabled UMCI.
    //

    if (SigningLevel == SE_SIGNING_LEVEL_UNCHECKED) {

        if (((SIPolicyIsPolicyActive(SiPolicyTypeSku, &PolicyHandle) != FALSE) &&
             (SIPolicyUmciEnabled(PolicyHandle) != FALSE)) ||
            ((SIPolicyIsPolicyActive(SiPolicyTypeCi, &PolicyHandle) != FALSE) &&
             (SIPolicyUmciEnabled(PolicyHandle) != FALSE))) {

            SigningLevel = SE_SIGNING_LEVEL_ENTERPRISE;
        }
    }

    return SigningLevel;
}

NTSTATUS
OslSetSeILSigningPolicy(
    _In_ PLOADER_PARAMETER_BLOCK LoaderBlock,
    _In_ ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine initializes the value of nt!SetSeILSigningPolicy. Must be done
    in the boot loader so the read-only section is written prior to PatchGuard
    attempting to gather signatures of memory pages.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block. Required
        to locate nt in memory.

    SystemHiveHandle - Supplies the handle to the system hive.

Return Value:

    STATUS_SUCCESS when successful.

--*/

{
    PKLDR_DATA_TABLE_ENTRY KernelEntry;
    PUCHAR KernelBase;
    PSE_SIGNING_LEVEL *SeILSigningPolicyPtrPtr;

    //
    // Kernel is always the first entry in the load order list.
    //

    KernelEntry = CONTAINING_RECORD(LoaderBlock->LoadOrderListHead.Flink,
                                    KLDR_DATA_TABLE_ENTRY,
                                    InLoadOrderLinks);

    if (KernelEntry == NULL) {
        return STATUS_NO_MEMORY;
    }

    KernelBase = (PUCHAR)KernelEntry->DllBase;

    SeILSigningPolicyPtrPtr = RtlFindExportedRoutineByName(KernelBase, "SeILSigningPolicyPtr");

    //
    // Although this pointer target is supposed to be read-only, in this phase
    // of startup memory protections have not yet been applied. Direct write.
    //
    // This is a ptr to a ptr. First step is to set the target value.
    //

    **SeILSigningPolicyPtrPtr = OslpDetermineSeILSigningPolicy(SystemHiveHandle);

    //
    // Next, clear the pointer in nt to make life harder on attackers.
    //

    *SeILSigningPolicyPtrPtr = NULL;

    return STATUS_SUCCESS;
}

