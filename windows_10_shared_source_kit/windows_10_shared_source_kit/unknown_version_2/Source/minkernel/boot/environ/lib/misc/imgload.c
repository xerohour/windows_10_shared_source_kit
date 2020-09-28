/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    imagload.c

Abstract:

    This module implements the code to load an image into memory.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "image.h"
#include <secureboot.h>
#include <sipolicy.h>
#include <blsipolicy.h>
#include <mincrypstring.h>
#include "systemevents.h"
#include "cimin.h"
#if !defined(MISC_MIN)
#include "bltracelogging.h"
#include <telemetry\MicrosoftTelemetry.h>
#endif
// ---------------------------------------------------------------- Definitions

#define IMGLOAD_CATALOG_EXT   L".cat"

#if EFI

#define IMGLOAD_FIRMWARE_EXTENSION ".EFI"

#else

#define IMGLOAD_FIRMWARE_EXTENSION ".EXE"

#endif

//
// Default crypto algorithms for image verification.
//

#if defined(IMG_MINIMUM_HASH_SHA256)
#define IMGLOAD_MINIMUM_HASH_ALGORITHM              CALG_SHA_256
#else
#define IMGLOAD_MINIMUM_HASH_ALGORITHM              CALG_SHA1
#endif

#define IMGLOAD_STRONG_CRYPTO_MINIMUM_HASH_ALGORITHM CALG_SHA_256
#define IMGLOAD_ELAM_HASH_ALGORITHM                  CALG_SHA_256
#define IMGLOAD_HALEXTENSION_HASH_ALGORITHM          CALG_SHA_256
#define IMGLOAD_WINDOWS_COMPONENT_HASH_ALGORITHM     CALG_SHA_256
#define IMGLOAD_FIRMWARE_UPDATE_HASH_ALGORITHM       CALG_SHA_256
#define IMGLOAD_COREEXTENSION_HASH_ALGORITHM         CALG_SHA_256

#define IMGLOAD_ALL_MICROSOFT_PRODUCTION_ROOTS \
            (MINCRYPT_POLICY_PRSROOT | MINCRYPT_POLICY_PRSROOT_CURRENT)

#define IMGLOAD_ALL_MICROSOFT_TEST_ROOTS \
            (MINCRYPT_POLICY_TESTROOT | MINCRYPT_POLICY_TESTROOT_CURRENT)

#define IMGLOAD_ALL_MICROSOFT_FLIGHT_ROOTS MINCRYPT_POLICY_FLIGHTROOT

#define IMGLOAD_ALL_MICROSOFT_ROOTS \
            (IMGLOAD_ALL_MICROSOFT_PRODUCTION_ROOTS | \
             IMGLOAD_ALL_MICROSOFT_TEST_ROOTS       | \
             IMGLOAD_ALL_MICROSOFT_FLIGHT_ROOTS)

//
// Default signing policies.
//

#define IMGLOAD_SIGNING_POLICY_DEFAULT 0

//
// Minimum time of trust for the revocation list.
// Tuesday, May 21, 2013 12:15:21 PM GMT
//

#define IMGLOAD_BOOT_STL_MINIMUM_TIME_OF_TRUST_HIGH 30299735
#define IMGLOAD_BOOT_STL_MINIMUM_TIME_OF_TRUST_LOW  2307533440

//
// For TH, we communicated externally that we do not use the timestamp time
// for WHQL requirement check, which is contrary to our functional spec.
// The team agreed to conform to the external communication, fully realizing
// this will actually relax the security.
//
// The team will revisit this issue in future, tracked by Bug 3655516.
//

#if (0) // Uncomment to enable.
#define IMGLOAD_WHQL_ENABLE_TIMESTAMP_CHECK        1
#endif

// -------------------------------------------------------------------- Globals

extern ULONG BlImgAcceptedRootKeys;
extern ULONG BlImgAcceptedRootKeysMask;
ULONG BlImgAcceptedTrustedBootRootKeys;
LOGICAL BlImgCodeIntegrityInitialized;
ULONG BlImgCodeIntegrityInitializationAttempts;
BOOLEAN BlImgTestSigningEnabled;
BOOLEAN BlImgFlightSigningEnabled;
static PWSTR BlImgCatalogHint;
static BOOLEAN BlImgRevokePrereleaseSignersOnceSet;
IMG_SIGNING_SCENARIO BlImgScenarios[ImgSigningScenarioMax];
IMG_SIGNER_TYPE BlImgSignerTypeMax = ImgSignerTypeProduction;
PCIMG_SIGNER BlImgSigners;
ULONG BlImgSignersCount;
ALG_ID BlImgMinimumHashAlgorithm = IMGLOAD_MINIMUM_HASH_ALGORITHM;
ULONG BlSigningPolicy = IMGLOAD_SIGNING_POLICY_DEFAULT;

FILE_ID BlImgCatalogDirectoryId = INVALID_FILE_ID;
DEVICE_ID BlImgCatalogDeviceId = INVALID_DEVICE_ID;
PWSTR BlImgCatalogDirectoryPath;
LOGICAL BlImgCatalogEnumerationFinished;
ULONG BlImgCatalogEnumerationPass;
ULONG BlImgCatalogPolicy = CATALOG_FLAGS_NONE;
PUNICODE_STRING BlImgCatalogPrecedenceList;
ULONG BlImgCatalogPrecedenceListEntries;
PWCHAR BlImgLastFoundCatalogFileName;
ULONG BlImgLastFoundCatalogFileNameSize;
BOOLEAN BlImgSystemCatalogLoaded;
PWSTR BlImgSystemCatalogName;
LIST_ENTRY BlImgLoadedCatalogs;
#if !defined(MISC_MIN)
BOOLEAN BlImgSystemUpgradeInProgress = FALSE;
BOOLEAN BlImgWinPE = FALSE;
ULONG BlImgWhqlSettings = 0;
BOOLEAN BlImgSysDevWhqlAuditModeEnabled = FALSE;
BOOLEAN BlImgSysDevWhqlSignedEnabled = FALSE;
BOOLEAN BlImgSysDevEvWhqlSignedEnabled = FALSE;
static ULONG BlImgNumEventsFired = 0;
#endif

static const UNICODE_STRING BlpImgScenarioPolicyKeys[ImgSigningScenarioMax] =
{
    RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_KEY_SIGNING_SCENARIO_BOOT_APPS),
    RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_KEY_SIGNING_SCENARIO_WINDOWS),
    RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_KEY_SIGNING_SCENARIO_HAL_EXTENSIONS),
    RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_KEY_SIGNING_SCENARIO_ELAM_DRIVERS),
    RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_KEY_SIGNING_SCENARIO_DRIVERS),
    RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_KEY_SIGNING_SCENARIO_BOOT_REVOCATION_LIST),
    RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_KEY_SIGNING_SCENARIO_FIRMWARE_UPDATE),
    RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_KEY_SIGNING_SCENARIO_CORE_EXTENSIONS),
    RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_KEY_SIGNING_SCENARIO_PLATFORM_MANIFEST_REQUIRED),
    RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_KEY_SIGNING_SCENARIO_RUNTIME_REVOCATION_LIST),
    RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_KEY_SIGNING_SCENARIO_PLATFORM_MANIFEST),
    RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_KEY_SIGNING_SCENARIO_FLASHING),
};

//
// Order must be production, test and testsigning.
//

static const UNICODE_STRING BlpImgPolicySignerTypes[] =
{
    RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_VALUE_PROD_SIGNERS),
    RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_VALUE_TEST_SIGNERS),
    RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_VALUE_TESTSIGN_SIGNERS),
};

static const UNICODE_STRING BlpImgPolicyHashAlgorithmString = \
                RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_VALUE_HASH_ALGORITHM);

static const UNICODE_STRING BlpImgPolicySignersKey = \
                RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_KEY_SIGNING_SIGNERS);

static const UNICODE_STRING BlpImgPolicySignersCount = \
                RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_VALUE_SIGNERS_COUNT);

static const UNICODE_STRING BlpImgPolicySignerKnownRoot = \
                RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_VALUE_KNOWNROOT);

static const UNICODE_STRING BlpImgPolicySignerToBeSignedHash = \
                RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_VALUE_TOBESIGNEDHASH);

static const UNICODE_STRING BlpImgPolicySignerEkus = \
                RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_VALUE_EKUS);

static const UNICODE_STRING BlpSecureBootPlatformKey = \
                RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_PLATFORM_KEY);

static const UNICODE_STRING BlpSecureBootDisableLifetimeSigningValue = \
                RTL_CONSTANT_STRING(SECUREBOOT_POLICIES_DISABLE_LIFETIME_SIGNING);
    

//
// Minimum time of trust for the revocation list.
//

static const LARGE_INTEGER BlpImgBootStlMinimumTimeOfTrust =
{
    IMGLOAD_BOOT_STL_MINIMUM_TIME_OF_TRUST_LOW,
    IMGLOAD_BOOT_STL_MINIMUM_TIME_OF_TRUST_HIGH
};

#if !defined(MISC_MIN)
//
// Default SysDev WHQL enforcement date/time. Can be override by
// HKLM:System\CurrentControlSet\CI\WhqlEnforcementDateTime to be earlier.
//

static LARGE_INTEGER BlImgSysDevWhqlEnforcementDateTime =
{
    CI_WHQL_ENFORCEMENT_DATE_TIME_LOW,
    CI_WHQL_ENFORCEMENT_DATE_TIME_HIGH
};
#endif

//
// Initialize telemetry declarations for CI.
//

#if !defined(MISC_MIN)
// {DDD9464F-84F5-4536-9F80-03E9D3254E5B}
TRACELOGGING_DEFINE_PROVIDER(
    g_CiTraceLoggingProvider,
    CI_TRACE_LOGGING_PROVIDER_NAME,
    (0xddd9464f, 0x84f5, 0x4536, 0x9f, 0x80, 0x3, 0xe9, 0xd3, 0x25, 0x4e, 0x5b),
    TraceLoggingOptionMicrosoftTelemetry());

TRACELOGGING_DEFINE_PROVIDER(
    g_CiAuditTraceLoggingProvider,
    CI_AUDIT_TRACE_LOGGING_PROVIDER_NAME,
    (0x2e1eb30a, 0xc39f, 0x453f, 0xb2, 0x5f, 0x74, 0xe1, 0x48, 0x62, 0xf9, 0x46),
    TraceLoggingOptionMicrosoftTelemetry());
#endif

// --------------------------------------------------------- Image Loading APIs

NTSTATUS
BlImgGetNtHeader (
    __in_bcount(ImageSize) PVOID ImageBase,
    __in ULONG ImageSize,
    __deref_out PIMAGE_NT_HEADERS *NtHeaders
    )

/*++

Routine Description:

    This routine is a boot environment wrapper for RtlImageNtHeaderEx. This
    ensures that RtlImageNtHeaderEx is not called with any flags.

Arguments:

    ImageBase - Supplies a pointer to the base of the image whose headers are to
        be returned.

    ImageSize - Supplies the size (in bytes) of the image in memory.

    NtHeaders - Supplies a pointer to receive the address of the image's NT
        headers.

Return Value:

    NT status code.

--*/

{

    //
    // Do not pass any flags as the first argument to RtlImageNtHeaderEx. This
    // will activate the range check on the image.
    //

    return RtlImageNtHeaderEx(0, ImageBase, ImageSize, NtHeaders);
}

NTSTATUS
BlImgGetPEImageSize (
    __in DEVICE_ID DeviceId,
    __in PWSTR ImagePath,
    __out PULONG ImageSize
    )

/*++

Routine Description:

    This routine finds the size of memory required to load the PE image file.

Arguments:

    DeviceId - Supplies the device containing the PE binary.

    ImagePath - Supplies the path to the PE image.

    ImageSize - Supplies a pointer to a variable that receives the size of
        memory required to load the PE image file.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if ImagePath or ImageSize are NULL.

    STATUS_UNSUCCESSFUL if a filesystem could not be mounted on block i/o
        device.

    STATUS_INVALID_IMAGE_FORMAT if the image machine type was unknown.

    STATUS_NO_SUCH_FILE if the requested file does not exist on the specified
        block i/o device.

    STATUS_FILE_INVALID if the specified file is not a PE image.

--*/

{

    ULONG AlignedImageSize;
    IMG_FILE_HANDLE FileHandle;
    PIMAGE_NT_HEADERS NtHeader;
    UCHAR Sector[1024];
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceImg);
    ImgpInitializeFileHandle(&FileHandle);

    //
    // Parameter checking.  Ensure that the caller provided a file to load,
    // an address to return the loaded image's base.
    //

    if ((ImagePath == NULL) || (ImageSize == NULL)) {
        Status = STATUS_INVALID_PARAMETER;
        goto GetPEImageSizeEnd;
    }

    //
    // Attempt to open the requested file.
    //

    Status = ImgpOpenFile(DeviceId, ImagePath, OPEN_READ, &FileHandle);
    if (!NT_SUCCESS(Status)) {
        goto GetPEImageSizeEnd;
    }

    //
    // Read the PE image headers into a local buffer to obtain the size of the
    // image.
    //

    Status = ImgpReadAtFileOffset(&FileHandle,
                                  sizeof(Sector),
                                  0,
                                  Sector,
                                  NULL,
                                  FILE_FLAGS_NONE);

    if (!NT_SUCCESS(Status)) {
        Status = STATUS_FILE_INVALID;
        goto GetPEImageSizeEnd;
    }

    //
    // Attempt to find the NT Headers for the file.  If the resultant is NULL,
    // then the image is not a valid PE image.
    //

    Status = BlImgGetNtHeader(Sector, sizeof(Sector), &NtHeader);
    if (!NT_SUCCESS(Status)) {
        goto GetPEImageSizeEnd;
    }

    *ImageSize = BlImgGetSizeOfImage(NtHeader);
     AlignedImageSize = ALIGN_RANGE_UP(*ImageSize, PAGE_SIZE);
     if (AlignedImageSize < *ImageSize) {
        Status = STATUS_INTEGER_OVERFLOW;
        goto GetPEImageSizeEnd;
     }

    *ImageSize = AlignedImageSize;

GetPEImageSizeEnd:
    ImgpCloseFile(&FileHandle);
    BlpInterfaceExit(InterfaceImg);
    return Status;
}

NTSTATUS
BlImgLoadPEImageEx (
    __in DEVICE_ID DeviceId,
    __in MEMORY_TYPE MemoryType,
    __in PWSTR LoadFile,
    __deref_inout_bcount_opt(*ImageSize) PVOID *ImageBase,
    __out_opt PULONG ImageSize,
    __out_bcount_opt(MAX_HASH_LEN) PUCHAR ImageHash,
    __out_opt PULONG ImageHashLength,
    __out_opt PULONG ImageHashAlgorithm,
    __in ULONG PreferredAttributes,
    __in ULONG PreferredAlignment,
    __in ULONG Flags,
    __out PULONG LoadInformation,
    __out_opt PBOOT_TRUSTED_BOOT_INFORMATION BootTrustedBootInformation
    )

/*++

Routine Description:

    This routine loads a PE image into memory, expanding the file and when
    appropriate, performs relocations.

Arguments:

    DeviceId - Supplies the device containing the PE binary.

    MemoryType - Supplies the type of memory to to be assigned to the allocated
        memory descriptor.

    LoadFile - Supplies the path to the PE binary to load.

    ImageBase - Supplies a pointer to a variable that contains the preferred
        image base.  A supplied value of NULL indicates no preference.  On
        output, the variable contains the address of the loaded image.

    ImageSize - Supplies a pointer to a variable that receives the size of
        the loaded image.

    ImageHash - Supplies an optional pointer to a variable that receives the
        the cryptographic hash of the image.  The hash is computed prior to
        image relocation.

    ImageHashLength - Supplies an optional pointer to a varaible that receives
        the length of the image hash.

    ImageHashAlgorithm - Supplies an optional pointer to a variable that
        receives the ID of the algorithm used to hash the image.

    PreferredAttributes - Supplies optional memory attributes for the image's
        memory allocation.

    PreferredAlignment - Supplies an optional alignement preference for the
        image's memory allocation.

    Flags - Supplies a bitmask describing API settings.  Possible values
        include:

        IMAGE_FLAGS_MAP_ONE_TO_ONE : Maps the loaded image with a virtual
            address equal to it's virtual address.

        IMAGE_FLAGS_VALIDATE_MACHINE_TYPE : Ensures that the PE image to
            load has the same machine type as the executing Boot Application.

        IMAGE_FLAGS_MEMORY_PRE_ALLOCATED : Indicates that memory has already
            been pre-allocated for the image and that *ImageBase is the
            pointer to the pre-allocated memory address and *ImageSize is
            its size.

        IMAGE_FLAGS_ALLOCATE_IN_LARGE_PAGE : Requests that an allocation for
            the specified image be made in a large page.  A large page
            allocation will reserve an entire large page exclusively for
            allocations requesting large page allocation.

        IMAGE_FLAGS_VALIDATE_IMAGE_HASH : Indicates that the PE image needs
            to be validated by its cryptographic hash.

        IMAGE_FLAGS_VALID_HASH_REQUIRED : Indicates that the PE image load
            must fail if the image's cryptographic hash is found to be
            invalid.  This flag is ignored if the validate image hash flag is
            not set.

        IMAGE_FLAGS_FIRMWARE_UPDATE : Indicates that the PE image load is a
            firmware update capsule, and must fail if the image's cryptographic
            hash is found to be invalid.  This flag is ignored if the validate
            image hash flag is not set.

        IMAGE_FLAGS_MS_SIGNATURE_REQUIRED_CATALOG : Indicates that a MS
            signature is required, but may be found in a catalog file.

        IMAGE_FLAGS_VALIDATE_FORCE_INTEGRITY : Indicates that the PE image
            needs to be validated if it contains the force integrity bit set
            in the dll characteristics field of the PE optional header.

        IMAGE_FLAGS_NO_RELOCATIONS : Specifies that this API should not perform
            relocations on the image.  It is not considered a failure if the
            image has a relocation section.

        IMAGE_FLAGS_EARLY_LAUNCH : Indicates that the PE image load must fail if
            the image does not have the correct signing policy for an early
            launch binary.

        IMAGE_FLAGS_HAL_EXTENSION : Indicates that the PE image load must fail
            if the image does not have the correct signing policy for a HAL
            extension.

        IMAGE_FLAGS_CORE_EXTENSION : Indicates that the PE image load must fail
            if the image does not have the correct signing policy for a Core
            Extensions (which are 3rd party supplied drivers).

    LoadInformation - Supplies an optional pointer to a variable that receives
        informational flags describing various aspects of the load.  The
        following flags may be set during image load.

        LOAD_INFORMATION_IMAGE_HASH_INVALID : Indicates that a cryptographic
            image hash was computed and found to be invalid.

        LOAD_INFORMATION_EMBEDDED_SIGNED : Indicates that the image is verified
            using embedded signature.

        LOAD_INFORMATION_CATALOG_SIGNED : Indicates that the image is verified
            using a signature from a catalog.

        LOAD_INFORMATION_POLICY_VIOLATION : Indicates that the image violated
            system integrity policy.

        LOAD_INFORMATION_POLICY_VIOLATION_AUDIT : Indicates that the image violated
            system integrity policy but that and audit policy is in place 
            which allows the image.

        LOAD_INFORMATION_IMAGE_BOOT_SCENARIO_AWARE : Indicates that the image
            is aware of any boot scenario specific system integrity operations
            that must be performed to preserve the integrity of those scenarios.

        LOAD_INFORMATION_IMAGE_BOOT_SCENARIO_AGNOSTIC : Indicates that the image will not
            implement any boot application scenario system integrity operations but
            is not required to because it is known not to load any other boot
            application or exit the boot environment.

    BootTrustedBootInformation - Supplies a pointer to a structure that receives
        advanced cryptographic information for the image.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if LoadFile or ImageBase are NULL.

    STATUS_NO_MEMORY if there is insufficient memory to perform the request.

    STATUS_UNSUCCESSFUL if a filesystem could not be mounted on block i/o
        device.

    STATUS_UNSUCCESSFUL if unable to apply relocation.

    STATUS_INVALID_IMAGE_FORMAT if relocations are invalid.

    STATUS_INVALID_IMAGE_FORMAT if the image machine type was unknown.

    STATUS_INVALID_IMAGE_WIN_32 if a 32 bit image was loaded by a
        64 bit application.

    STATUS_INVALID_IMAGE_WIN_64 if a 64 bit image was loaded by a
        32 bit application.

    STATUS_NO_SUCH_FILE if the requested file does not exist on the specified
        block i/o device.

    STATUS_FILE_INVALID if the specified file is not a PE image.

    STATUS_BUFFER_TOO_SMALL if a pre-allocated memory buffer was insufficient
        in size for the requested image.

--*/

{

    IMG_FILE_HANDLE FileHandle;
    ULONG LocalLoadInformation;
    NTSTATUS Status;

    LocalLoadInformation = 0;

    BlpInterfaceEnter(InterfaceImg);
    ImgpInitializeFileHandle(&FileHandle);

    //
    // Parameter checking.  Ensure that the caller provided a file to load,
    // an address to return the loaded image's base.  If the caller specified
    // that a buffer was pre-allocated, ensure that it exists.
    //

    if ((ImageBase == NULL) || (LoadFile == NULL) ||
        (CHECK_FLAG(Flags, IMAGE_FLAGS_MEMORY_PRE_ALLOCATED) &&
         ((*ImageBase == NULL) || (ImageSize == NULL)))) {

        Status = STATUS_INVALID_PARAMETER;
        goto LoadPEImageExEnd;
    }

    //
    // Attempt to load the requested file as a PE image.
    //

    Status = ImgpOpenFile(DeviceId, LoadFile, OPEN_READ, &FileHandle);
    if (!NT_SUCCESS(Status)) {
        goto LoadPEImageExEnd;
    }

    Status = ImgpLoadPEImage(&FileHandle,
                             MemoryType,
                             ImageBase,
                             ImageSize,
                             ImageHash,
                             ImageHashLength,
                             ImageHashAlgorithm,
                             PreferredAttributes,
                             PreferredAlignment,
                             Flags,
                             &LocalLoadInformation,
                             BootTrustedBootInformation);

    if (!NT_SUCCESS(Status)) {
        goto LoadPEImageExEnd;
    }

LoadPEImageExEnd:

    if (ARGUMENT_PRESENT(LoadInformation) != FALSE) {
        *LoadInformation = LocalLoadInformation;
    }

    ImgpCloseFile(&FileHandle);
    BlpInterfaceExit(InterfaceImg);
    return Status;
}

NTSTATUS
BlImgLoadPEImageFromSourceBuffer (
    __in_bcount(SourceBufferSize) PVOID SourceBuffer,
    __in ULONG64 SourceBufferSize,
    __in MEMORY_TYPE MemoryType,
    __in_opt PWSTR LoadFile,
    __deref_inout_bcount_opt(*ImageSize) PVOID *ImageBase,
    __out_opt PULONG ImageSize,
    __out_bcount_opt(MAX_HASH_LEN) PUCHAR ImageHash,
    __out_opt PULONG ImageHashLength,
    __out_opt PULONG ImageHashAlgorithm,
    __in ULONG PreferredAttributes,
    __in ULONG PreferredAlignment,
    __in ULONG Flags,
    __out_opt PULONG LoadInformation,
    __out_opt PBOOT_TRUSTED_BOOT_INFORMATION BootTrustedBootInformation
    )

/*++

Routine Description:

    This routine loads a PE image into memory, expanding the file and when
    appropriate, performs relocations.

Arguments:

    SourceBuffer - Supplies a pointer to a flat file image loaded into memory.

    SourceBufferSize - Supplies the size of the source buffer.

    MemoryType - Supplies the type of memory to to be assigned to the allocated
        memory descriptor.

    LoadFile - Supplies an optional pointer to the name of the source file.

    ImageBase - Supplies a pointer to a variable that contains the preferred
        image base.  A supplied value of NULL indicates no preference.  On
        output, the variable contains the address of the loaded image.

    ImageSize - Supplies a pointer to a variable that receives the size of
        the loaded image.

    ImageHash - Supplies an optional pointer to a variable that receives the
        the cryptographic hash of the image.  The hash is computed prior to
        image relocation.

    ImageHashLength - Supplies an optional pointer to a varaible that receives
        the length of the image hash.

    ImageHashAlgorithm - Supplies an optional pointer to a variable that
        receives the ID of the algorithm used to hash the image.

    PreferredAttributes - Supplies optional memory attributes for the image's
        memory allocation.

    PreferredAlignment - Supplies an optional alignement preference for the
        image's memory allocation.

    Flags - Supplies a bitmask describing API settings.  Possible values
        include:

        IMAGE_FLAGS_MAP_ONE_TO_ONE : Maps the loaded image with a virtual
            address equal to it's virtual address.

        IMAGE_FLAGS_VALIDATE_MACHINE_TYPE : Ensures that the PE image to
            load has the same machine type as the executing Boot Application.

        IMAGE_FLAGS_MEMORY_PRE_ALLOCATED : Indicates that memory has already
            been pre-allocated for the image and that *ImageBase is the
            pointer to the pre-allocated memory address and *ImageSize is
            its size.

        IMAGE_FLAGS_ALLOCATE_IN_LARGE_PAGE : Requests that an allocation for
            the specified image be made in a large page.  A large page
            allocation will reserve an entire large page exclusively for
            allocations requesting large page allocation.

        IMAGE_FLAGS_VALIDATE_IMAGE_HASH : Indicates that the PE image needs
            to be validated by its cryptographic hash.

        IMAGE_FLAGS_VALID_HASH_REQUIRED : Indicates that the PE image load
            must fail if the image's cryptographic hash is found to be
            invalid.  This flag is ignored if the validate image hash flag is
            not set.

        IMAGE_FLAGS_VALIDATE_FORCE_INTEGRITY : Indicates that the PE image
            needs to be validated if it contains the force integrity bit set
            in the dll characteristics field of the PE optional header.

        IMAGE_FLAGS_NO_RELOCATIONS : Specifies that this API should not perform
            relocations on the image.  It is not considered a failure if the
            image has a relocation section.

        IMAGE_FLAGS_EARLY_LAUNCH : Indicates that the PE image load must fail if
            the image does not have the correct signing policy for an early
            launch binary.

        IMAGE_FLAGS_HAL_EXTENSION : Indicates that the PE image load must fail
            if the image does not have the correct signing policy for a HAL
            extension.

        IMAGE_FLAGS_CORE_EXTENSION : Indicates that the PE image load must fail
            if the image does not have the correct signing policy for a Core
            Extensions (which are 3rd party supplied drivers).

    LoadInformation - Supplies an optional pointer to a variable that receives
        informational flags describing various aspects of the load.  The
        following flags may be set during image load.

        LOAD_INFORMATION_IMAGE_HASH_INVALID : Indicates that a cryptographic
            image hash was computed and found to be invalid.

        LOAD_INFORMATION_EMBEDDED_SIGNED : Indicates that the image is verified
            using embedded signature.

        LOAD_INFORMATION_CATALOG_SIGNED : Indicates that the image is verified
            using a signature from a catalog.

       LOAD_INFORMATION_POLICY_VIOLATION : Indicates that the image violated
            system integrity policy.

        LOAD_INFORMATION_POLICY_VIOLATION_AUDIT : Indicates that the image violated
            system integrity policy but that and audit policy is in place 
            which allows the image.

       LOAD_INFORMATION_IMAGE_BOOT_SCENARIO_AWARE : Indicates that the image
            is aware of any boot scenario specific system integrity operations
            that must be performed to preserve the integrity of those scenarios.

       LOAD_INFORMATION_IMAGE_BOOT_SCENARIO_AGNOSTIC : Indicates that the image will not
            implement any boot application scenario system integrity operations but
            is not required to because it is known not to load any other boot
            application or exit the boot environment.

    BootTrustedBootInformation - Supplies a pointer to a structure that
        optionally receives advanced cryptographic information for the image.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if ImageBase are NULL.

    STATUS_NO_MEMORY if there is insufficient memory to perform the request.

    STATUS_UNSUCCESSFUL if unable to apply relocation.

    STATUS_INVALID_IMAGE_FORMAT if relocations are invalid.

    STATUS_INVALID_IMAGE_FORMAT if the image machine type was unknown.

    STATUS_INVALID_IMAGE_WIN_32 if a 32 bit image was loaded by a
        64 bit application.

    STATUS_INVALID_IMAGE_WIN_64 if a 64 bit image was loaded by a
        32 bit application.

    STATUS_FILE_INVALID if the specified file is not a PE image.

    STATUS_BUFFER_TOO_SMALL if a pre-allocated memory buffer was insufficient
        in size for the requested image.

--*/

{

    IMG_FILE_HANDLE FileHandle;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceImg);
    ImgpInitializeFileHandleForSourceBuffer(&FileHandle,
                                            SourceBuffer,
                                            SourceBufferSize,
                                            LoadFile);

    //
    // Ensure all caller provided parameters are valid.  Minimally, the caller
    // must provide a valid source buffer and a valid variable to receive the
    // allocated image buffer.
    //

    if ((ARGUMENT_PRESENT(SourceBuffer) == FALSE) ||
        (SourceBufferSize == 0) ||
        (ARGUMENT_PRESENT(ImageBase) == FALSE) ||
        ((CHECK_FLAG(Flags, IMAGE_FLAGS_MEMORY_PRE_ALLOCATED) != FALSE) &&
         ((*ImageBase == NULL) || (ImageSize == NULL)))) {

        Status = STATUS_INVALID_PARAMETER;
        goto LoadPEImageFromSourceBufferEnd;
    }

    //
    // Attempt to load the requested file as a PE image.
    //

    Status = ImgpLoadPEImage(&FileHandle,
                             MemoryType,
                             ImageBase,
                             ImageSize,
                             ImageHash,
                             ImageHashLength,
                             ImageHashAlgorithm,
                             PreferredAttributes,
                             PreferredAlignment,
                             Flags,
                             LoadInformation,
                             BootTrustedBootInformation);

    if (!NT_SUCCESS(Status)) {
        goto LoadPEImageFromSourceBufferEnd;
    }

LoadPEImageFromSourceBufferEnd:
    ImgpCloseFile(&FileHandle);
    BlpInterfaceExit(InterfaceImg);
    return Status;
}

NTSTATUS
BlImgLoadImageWithProgress2 (
    __in DEVICE_ID DeviceId,
    __in MEMORY_TYPE MemoryType,
    __in PWSTR LoadFile,
    __inout PVOID *ImageBase,
    __inout PULONG ImageSize,
    __in ULONG PreferredAttributes,
    __in ULONG PreferredAlignment,
    __in ULONG Flags,
    __in BOOLEAN ShowProgress,
    __deref_opt_out_bcount_opt(*ImageHashSize) PUCHAR* ImageHash,
    __out_opt PULONG ImageHashSize
    )

/*++

Routine Description:

    Loads a file into memory, optionally calculating the file hash. When
    specified, progress is reported during the I/O operations.

Arguments:

    DeviceId - Supplies the device containing the file to load.

    MemoryType - Supplies the type of memory to to be assigned to the allocated
        memory descriptor.

    LoadFile - Supplies the path to the file to load.

    ImageBase - Supplies a pointer to a variable that contains the preferred
        image base.  A supplied value of NULL indicates no preference.  On
        output, the variable contains the address of the loaded image.

    ImageSize - Supplies a pointer to a variable that receives the size of
        the loaded image.

    PreferredAttributes - Supplies optional memory attributes for the image's
        memory allocation.

    PreferredAlignment - Supplies an optional alignement preference for the
        image's memory allocation.

    Flags - Supplies a bitmask describing API settings.  Possible values
        include:

        IMAGE_FLAGS_MAP_ONE_TO_ONE : Maps the loaded image with a virtual
            address equal to it's virtual address.

        IMAGE_FLAGS_MEMORY_PRE_ALLOCATED : Indicates that memory has already
            been pre-allocated for the image and that *ImageBase is the
            pointer to the pre-allocated memory address and *ImageSize is
            its size.

        IMAGE_FLAGS_ALLOCATE_IN_LARGE_PAGE : Requests that an allocation for
            the specified image be made in a large page.  A large page
            allocation will reserve an entire large page exclusively for
            allocations requesting large page allocation.

        IMAGE_FLAGS_VALIDATE_IMAGE_HASH : Indicates that the image needs to
            be validated by its cryptographic hash.

        IMAGE_FLAGS_FIRMWARE_UPDATE : Indicates that the firmware update image
            load must fail if the image's cryptographic hash is found to be
            invalid.

        IMAGE_FLAGS_MS_SIGNATURE_REQUIRED_CATALOG : Indicates that a MS
            signature is required, but may be found in a catalog file.

        IMAGE_FLAGS_CALCULATE_IMAGE_HASH : Requests that the SHA-256 hash of
            the file be calculated.

    ShowProgress - Supplies a boolean indicating whether progress should be
        reported during file load.  If true, BlUtlUpdateProgress is called to
        report progress.

    ImageHash - Supplies a pointer to a variable which will receive the image
        hash. Only calculated and returned when
        IMAGE_FLAGS_CALCULATE_IMAGE_HASH is supplied in Flags. This routine
        allocates this buffer and the caller is responsible for freeing the
        memory.

    ImageHashSize - Supplies a pointer to a variable which receives the size of
        the allocated ImageHash.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if LoadFile or ImageBase are NULL.

    STATUS_NO_MEMORY if there is insufficient memory to perform the request.

    STATUS_UNSUCCESSFUL if a filesystem could not be mounted on block i/o
        device.

    STATUS_UNSUCCESSFUL if unable to apply relocation.

    STATUS_NO_SUCH_FILE if the requested file does not exist on the specified
        block i/o device.

    STATUS_BUFFER_TOO_SMALL if a pre-allocated memory buffer was insufficient
        in size for the requested image.

--*/

{

    PVOID AllocatedImageBase;
    PUCHAR BufferPtr;
    IMG_FILE_HANDLE FileHandle;
    ULONG FileSize;
    UCHAR HashBuffer[MAX_HASH_LEN];
    BOOLEAN HashCalculation;
    MINCRYPL_HASH_CTXT HashContext;
    BOOLEAN HashValidation;
    BOOLEAN LocalShowProgress;
    ULONG PercentRemaining;
    PHYSICAL_ADDRESS PhysicalAddress;
    ULONG ProgressChunkSize;
    BOOLEAN ProgressDisplayComplete;
    ULONG ReadLength;
    ULONGLONG ReadOffset;
    ULONGLONG RemainingLength;
    IMG_SIGNING_SCENARIO_TYPE Scenario;
    ULONG SizeOfImage;
    NTSTATUS Status;
    ULONG ValidationFlags;
    PVOID BaseImageAddr;
    ULONG BaseImageSize;

    BlpInterfaceEnter(InterfaceImg);
    AllocatedImageBase = NULL;
    Scenario = ImgSigningScenarioWindows;
    SizeOfImage = 0;
    HashCalculation = FALSE;
    HashValidation = FALSE;
    ValidationFlags = 0;
    ImgpInitializeFileHandle(&FileHandle);

    //
    // Parameter checking.  Ensure that the caller provied a file to load,
    // an address to return the loaded image's base.  If the caller specified
    // that a buffer was pre-allocated, ensure that it exists.  If the caller
    // desires to calculate the hash of the file, ensure that the caller
    // provided pointers to receive the resulting hash.  Ensure that the caller
    // did not supply conflicting hash flags (calculate and validate).
    //

    if ((ImageBase == NULL) || (LoadFile == NULL) || (ImageSize == NULL) ||
        (CHECK_FLAG(Flags, IMAGE_FLAGS_MEMORY_PRE_ALLOCATED) &&
         (*ImageBase == NULL)) ||
        (CHECK_FLAG(Flags, IMAGE_FLAGS_CALCULATE_IMAGE_HASH) &&
         (ImageHash == NULL || ImageHashSize == NULL)) ||
        (CHECK_FLAG(Flags, IMAGE_FLAGS_CALCULATE_IMAGE_HASH) &&
         CHECK_FLAG(Flags, IMAGE_FLAGS_HASH_VALIDATION_FLAGS))) {

        Status = STATUS_INVALID_PARAMETER;
        goto LoadImageWithProgress2End;
    }

    if (ImageHash != NULL) {
        *ImageHash = NULL;
    }

    if (ImageHashSize != NULL) {
        *ImageHashSize = 0;
    }

    //
    // Attempt to open the requested file.
    //

    Status = ImgpOpenFile(DeviceId, LoadFile, OPEN_READ, &FileHandle);
    if (!NT_SUCCESS(Status)) {
        goto LoadImageWithProgress2End;
    }

    Status = ImgpGetFileSize(&FileHandle, &FileSize);
    if (!NT_SUCCESS(Status)) {
        goto LoadImageWithProgress2End;
    }

    SizeOfImage = FileSize;
    AllocatedImageBase = *ImageBase;
    BaseImageAddr = *ImageBase;
    BaseImageSize = FileSize;

    //
    // Allocate a memory buffer for the in-memory copy of the image.  If the
    // caller pre-allocated a buffer, ensure that it is sufficiently large.
    //

    if (CHECK_FLAG(Flags, IMAGE_FLAGS_MEMORY_PRE_ALLOCATED)) {
        __analysis_assume(ImageSize != NULL);
        if (*ImageSize < SizeOfImage) {
            *ImageSize = (ULONG)SizeOfImage;
            Status = STATUS_BUFFER_TOO_SMALL;
        }

    } else {
        Status = BlImgAllocateImageBuffer(&AllocatedImageBase,
                                          SizeOfImage,
                                          MemoryType,
                                          PreferredAttributes,
                                          PreferredAlignment,
                                          Flags);
    }

    if (!NT_SUCCESS(Status)) {
        goto LoadImageWithProgress2End;
    }

    //
    // Read the entire file into memory a chunk at a time, reporting progress
    // with each I/O operation, starting with zero.
    //

    RemainingLength = FileSize;
    BufferPtr = AllocatedImageBase;
    ReadOffset = 0;
    ProgressDisplayComplete = FALSE;
    LocalShowProgress = ShowProgress;
    if (LocalShowProgress != FALSE) {
        BlUtlUpdateProgress(0, &ProgressDisplayComplete);
        if (ProgressDisplayComplete != FALSE) {
            LocalShowProgress = FALSE;
        }
    }

    //
    // On 64-bit systems, due to firmware errors returned when requesting large
    // reads from DVDs and some HDDs, always read the file one chunk at a
    // time. All other architectures determine the read size based on whether or
    // not progress will be displayed.
    //

#if defined(AMD64) && !defined(NTKERNEL)

    ProgressChunkSize = DEFAULT_PROGRESS_CHUNK_SIZE;
    if (ProgressChunkSize > FileSize) {
        ProgressChunkSize = FileSize;
    }

#else

    if (LocalShowProgress != FALSE) {
        ProgressChunkSize = DEFAULT_PROGRESS_CHUNK_SIZE;
        if (ProgressChunkSize > FileSize) {
            ProgressChunkSize = FileSize;
        }

    } else {
        ProgressChunkSize = FileSize;
    }

#endif

    //
    // Determine if the image hash should be computed, and if so which
    // validation flags to use.
    //

    if ((CHECK_FLAG(Flags, IMAGE_FLAGS_VALIDATE_IMAGE_HASH) != FALSE) ||
        (CHECK_FLAG(Flags, IMAGE_FLAGS_CALCULATE_IMAGE_HASH) != FALSE)) {

        HashCalculation = TRUE;

        Status = ImgpVerifyMinimalCodeIntegrityInitialization();
        if (!NT_SUCCESS(Status)) {
            goto LoadImageWithProgress2End;
        }

        if (CHECK_FLAG(Flags, IMAGE_FLAGS_VALIDATE_IMAGE_HASH) != FALSE) {

            HashValidation = TRUE;

            Scenario = ImgpGetScenarioFromImageFlags(Flags);
            HashContext.HashAlgId = ImgpGetHashAlgorithmForScenario(Scenario);

            if (CHECK_FLAG(Flags,
                           IMAGE_FLAGS_NO_REVOCATION_LIST_CHECKS) != FALSE) {

                SET_FLAGS(ValidationFlags, IMGP_NO_REVOCATION_LIST_CHECKS);
            }

            if (CHECK_FLAG(Flags,
                           IMAGE_FLAGS_MS_SIGNATURE_REQUIRED_CATALOG) != FALSE) {

                SET_FLAGS(ValidationFlags, IMGP_MS_SIGNATURE_REQUIRED);
            }

        } else {

            //
            // Hash validation is done outside of this routine, and is
            // currently restricted to SHA-256.
            //

            HashContext.HashAlgId = CALG_SHA_256;
        }

        MinCrypL_InitHash(&HashContext, NULL);

    }

Retry:
    while (RemainingLength) {
        if (RemainingLength > ProgressChunkSize) {
            ReadLength = ProgressChunkSize;

        } else {
            ReadLength = (ULONG)RemainingLength;
        }

        Status = ImgpReadAtFileOffset(&FileHandle,
                                      ReadLength,
                                      ReadOffset,
                                      BufferPtr,
                                      NULL,
                                      FILE_FLAGS_NONE);

        if (!NT_SUCCESS(Status)) {

            //
            // If reading the entire file in one shot failed, retry with
            // chunking the file to work around firmware bugs.
            //

            if (ProgressChunkSize > DEFAULT_PROGRESS_CHUNK_SIZE) {
                ProgressChunkSize = DEFAULT_PROGRESS_CHUNK_SIZE;
                goto Retry;
            }

            goto LoadImageWithProgress2End;
        }

        if (HashCalculation != FALSE) {
            MinCrypL_UpdateHash(&HashContext, BufferPtr, ReadLength);
        }

        BufferPtr += ReadLength;
        RemainingLength -= ReadLength;
        ReadOffset += ReadLength;

        //
        // Update the progress.
        //

        if (LocalShowProgress != FALSE) {
            PercentRemaining = (ULONG)(((RemainingLength*100) / FileSize));
            BlUtlUpdateProgress(100 - PercentRemaining,
                                &ProgressDisplayComplete);

            if (ProgressDisplayComplete != FALSE) {
                LocalShowProgress = FALSE;
            }
        }
    }

    //
    // Finalize the image hash.
    //

    if (HashCalculation != FALSE) {
        MinCrypL_FinalizeHash(&HashContext, HashBuffer);

        if (HashValidation != FALSE) {

            //
            // N.B. Presently the only files that fall into this category are
            //      the firmware update binary blobs, and these require a MS
            //      signature.
            //

            Status = ImgpValidateImageHash(LoadFile,
                                           BaseImageAddr,
                                           BaseImageSize,
                                           NULL,
                                           HashContext.HashAlgId,
                                           HashBuffer,
                                           ValidationFlags,
                                           Scenario,
                                           NULL,
                                           NULL,
                                           NULL);

            if (!NT_SUCCESS(Status)) {
                Status = ImgpFilterValidationFailure(LoadFile,
                                                     Status,
                                                     Flags);
            }

            if (!NT_SUCCESS(Status)) {
                if (CHECK_FLAG(Flags, IMAGE_FLAGS_VALID_HASH_REQUIRED) != FALSE) {
                    goto LoadImageWithProgress2End;
                }

                Status = STATUS_SUCCESS;
            }

        } else {

            *ImageHash = (PUCHAR)BlMmAllocateHeap(SYMCRYPT_SHA256_RESULT_SIZE);
            if (*ImageHash == NULL) {
                Status = STATUS_NO_MEMORY;
                goto LoadImageWithProgress2End;
            }

            *ImageHashSize = SYMCRYPT_SHA256_RESULT_SIZE;
            RtlCopyMemory(*ImageHash, HashBuffer, *ImageHashSize);
        }
    }

    //
    // Set return values.
    //

    *ImageBase = AllocatedImageBase;
    if (ImageSize) {
        *ImageSize = SizeOfImage;
    }

LoadImageWithProgress2End:

    //
    // Cleanup.
    //

    ImgpCloseFile(&FileHandle);
    if (!NT_SUCCESS(Status) && (AllocatedImageBase != NULL)) {
        if (!CHECK_FLAG(Flags, IMAGE_FLAGS_MEMORY_PRE_ALLOCATED)) {
            if (CHECK_FLAG(Flags, IMAGE_FLAGS_MAP_ONE_TO_ONE)) {
                BlMmUnmapVirtualAddress(AllocatedImageBase, SizeOfImage);
                PhysicalAddress.QuadPart = (UINTN)AllocatedImageBase;
                BlMmFreePhysicalPages(PhysicalAddress);

            } else {
                BlMmFreePages(AllocatedImageBase);
            }
        }
    }

    //
    // If progress was originally set, even if the progress display is already
    // complete, set it to 100% so the next image load with progress knows to
    // restart to 0. Do not read the local value here, it may have changed.
    //
    // N.B. Instead, this could be changed in BlUtlUpdateProgress to assume
    //      that 0 progress means a restart, but it was not written that way
    //      and WDS would need to be regressed.
    //

    if (ShowProgress != FALSE) {
        BlUtlUpdateProgress(100, &ProgressDisplayComplete);
    }

    BlpInterfaceExit(InterfaceImg);
    return Status;
}

NTSTATUS
BlImgUnLoadImage (
    __in PVOID Image,
    __in ULONG ImageSize,
    __in ULONG Flags
    )

/*++

Routine Description:

    Unloads an image that was previously loaded with BlImgLoadImage.

Arguments:

    Image - Supplies a pointer to the image that should be unloaded.

    ImageSize - Supplies the size of the image.

    Flags - Supplies the IMAGE_FLAGS that the image was loaded with.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Image is NULL or ImageSize is 0.

--*/

{

    if ((Image == NULL) || (ImageSize == 0)) {
        return STATUS_INVALID_PARAMETER;
    }

    return BlImgUnallocateImageBuffer(Image, ImageSize, Flags);
}

PIMAGE_SECTION_HEADER
BlImgFindSection (
    __in_bcount(ImageSize) PVOID ImageBase,
    __in ULONG ImageSize,
    __in PSTR SectionName
    )

/*++

Routine Description:

    This routine searches the given image headers for a section matching the
    given name.

Arguments:

    ImageBase - Supplies the base address of the image of interest.

    ImageSize - Supplies the size (in bytes) of the image of interest.

    SectionName - Supplies the name of the section to search for.

Return Value:

    This function returns a pointer to the first section header that matches
    the given section name or NULL if no such section could be found.

--*/

{

    PIMAGE_NT_HEADERS NtHeader;
    ULONG NumberOfSections;
    PIMAGE_SECTION_HEADER SectionHeader;
    PIMAGE_SECTION_HEADER SectionToReturn;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceImg);
    SectionToReturn = NULL;
    Status = BlImgGetNtHeader(ImageBase, ImageSize, &NtHeader);
    if (NT_SUCCESS(Status)) {

        //
        // Search for the requestion section header in the PE header.
        //

        NumberOfSections = NtHeader->FileHeader.NumberOfSections;
        SectionHeader = IMAGE_FIRST_SECTION(NtHeader);
        while (NumberOfSections != 0) {
            if (_stricmp((PCHAR)SectionHeader->Name, SectionName) == 0) {
                SectionToReturn = SectionHeader;
                break;
            }

            SectionHeader += 1;
            NumberOfSections -= 1;
        }
    }

    BlpInterfaceExit(InterfaceImg);
    return SectionToReturn;
}

NTSTATUS
BlImgGetModuleName (
    __in_bcount(ImageSize) PVOID ImageBase,
    __in ULONG ImageSize,
    __out PSTRING ModuleName
    )

/*++

Routine Description:

    This routine extracts the module name from a loaded image.

Arguments:

    ImageBase - Supplies the address of the loaded image in memory.

    ImageSize - Supplies the size of the image loaded in memory.

    ModuleName - Supplies a pointer to a variable that receives the counted
        string.  This structure contains a pointer to a buffer that is allocated
        by this routine.  The caller is responsible for freeing this buffer.

Return Value:

    STATUS_SUCCESS if the operation completes successfully.

    STATUS_FILE_INVALID if the specified address does not point to a loaded
        image.

    STATUS_NOT_FOUND if the image does not contain module name information.

    Another error code if returned from a subroutine.

--*/

{

    PIMAGE_DATA_DIRECTORY DataDirectory;
    PIMAGE_DATA_DIRECTORY DirectoryEntry;
    PIMAGE_EXPORT_DIRECTORY ExportDirectory;
    ULONG ExportOffset;
    PSTR Extension;
    UINTN ExtensionSize;
    ULONG Index;
    UINTN Length;
    UINTN MaximumStringLength;
    PCHAR Name;
    ULONG NameOffset;
    PIMAGE_NT_HEADERS NtHeader;
    ULONG SectionCount;
    PIMAGE_SECTION_HEADER SectionHeader;
    NTSTATUS Status;

    ModuleName->Buffer = NULL;

    //
    // Extract information about the export directory from the image
    // header.  This directory contains the module name.
    //

    Status = BlImgGetNtHeader(ImageBase, ImageSize, &NtHeader);
    if (!NT_SUCCESS(Status)) {
       goto GetModuleNameEnd;
    }

    DataDirectory = NtHeader->OptionalHeader.DataDirectory;
    DirectoryEntry = &DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
    ExportOffset = DirectoryEntry->VirtualAddress;

    //
    // Walk the section headers validating that the export directory falls in
    // a valid section.
    //

    SectionCount = NtHeader->FileHeader.NumberOfSections;
    SectionHeader = IMAGE_FIRST_SECTION(NtHeader);
    for (Index = 0; Index < SectionCount; Index += 1) {
       if ((ExportOffset >= SectionHeader[Index].VirtualAddress) &&
           (ExportOffset < (SectionHeader[Index].VirtualAddress +
                           SectionHeader[Index].SizeOfRawData))) {

           break;
       }
    }

    if (Index == SectionCount) {
       Status = STATUS_NOT_FOUND;
       goto GetModuleNameEnd;
    }

    //
    // Get the module name out of the export directory.
    //

    ExportDirectory = Add2Ptr(ImageBase, ExportOffset);
    NameOffset = ExportDirectory->Name;
    if (NameOffset == 0) {
       Status = STATUS_NOT_FOUND;
       goto GetModuleNameEnd;
    }

    Name = Add2Ptr(ImageBase, NameOffset);

    //
    // Allocate a buffer for the name and copy it over.  The name is
    // NULL-terminated in the image.
    //

    MaximumStringLength = SectionHeader[Index].SizeOfRawData -
                         PtrOffset(SectionHeader[Index].VirtualAddress,
                                   NameOffset);

    Length = strnlen(Name, MaximumStringLength);
    ModuleName->Buffer = BlMmAllocateHeap(Length + 1);
    if (ModuleName->Buffer == NULL) {
       Status = STATUS_NO_MEMORY;
       goto GetModuleNameEnd;
    }

    RtlCopyMemory(ModuleName->Buffer, Name, Length);
    ModuleName->Buffer[Length] = CHAR_NULL;
    ModuleName->MaximumLength = (USHORT)(Length + 1);
    ModuleName->Length = (USHORT)(Length + 1);
    Status = STATUS_SUCCESS;

    //
    // The extension is not correct in all boot environment applications.  Fix
    // it up here, based upon the firmware environment.
    //

    Extension = strrchr(ModuleName->Buffer, '.');
    ExtensionSize = (ModuleName->Buffer + Length) - Extension;
    if ((Extension != NULL) &&
        (ExtensionSize == sizeof(IMGLOAD_FIRMWARE_EXTENSION) - 1)) {

        strcpy_s(Extension,
                 sizeof(IMGLOAD_FIRMWARE_EXTENSION),
                 IMGLOAD_FIRMWARE_EXTENSION);
    }

GetModuleNameEnd:
    if (!NT_SUCCESS(Status)) {
       if (ModuleName->Buffer != NULL) {
           BlMmFreeHeap(ModuleName->Buffer);
       }
    }

    return Status;

}

NTSTATUS
BlImgAllocateImageBuffer (
    __deref_inout_bcount(ImageSize) PVOID *ImageBuffer,
    __in ULONGLONG ImageSize,
    __in MEMORY_TYPE MemoryType,
    __in ULONG PreferredAttributes,
    __in ULONG PreferredAlignment,
    __in ULONG Flags
    )

/*++

Routine Description:

    Allocates an image buffer based on the caller's specifications.

Arguments:

    ImageBuffer - On input, contains a pointer to the preferred allocation
        buffer.  This is only valid if the preferred attributes specify that a
        fixed address is to be used.  On output, the pointer contains the
        address of the allocated buffer.  If any error is encountered,
        *ImageBuffer is NULL on return.

    ImageSize - The size of the image for which the buffer is being allocated.

    MemoryType - Supplies the type of memory to to be assigned to the
        allocated memory descriptor.

    PreferredAttributes - If present, supplies the preferred memory attributes
        of the allocated memory for the image.

    PreferredAlignment - If present, supplies the preferred image alignment.

    Flags - Flags for loaded image.  Includes:

        IMAGE_FLAGS_MAP_ONE_TO_ONE : Maps the loaded image with a virtual
            address equal to it's virtual address.

        IMAGE_FLAGS_ALLOCATE_IN_LARGE_PAGE : Requests that an allocation for
            the specified image be made in a large page.  A large page
            allocation will reserve an entire large page exclusively for
            allocations requesting large page allocation.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if there is insufficient memory to perform the request.

--*/

{

    PVOID AllocatedBase;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceImg);

    ASSERT(ImageBuffer != NULL);

    //
    // Only return with a non-NULL value in successful scenarios.
    //

    AllocatedBase = *ImageBuffer;
    *ImageBuffer = NULL;

    //
    // Allocations that use the Page Allocator should round up size to
    // a page boundary.
    //

    ImageSize = ALIGN_RANGE_UP(ImageSize, PAGE_SIZE);

    //
    // If the caller requested the image to be loaded into a large
    // page allocation, mark the memory attribute.  There is an IMAGE_FLAG
    // to specify this because some of the macros (BlImgLoadPEImage) do
    // not let you specify memory attributes, but do let you set
    // image flags.
    //

    if (CHECK_FLAG(Flags, IMAGE_FLAGS_ALLOCATE_IN_LARGE_PAGE)) {
        SET_FLAGS(PreferredAttributes, MEMORY_ATTRIBUTE_ALLOCATION_LARGE_PAGE);
    }

    //
    // Allocate a sufficient buffer for the image.  After setting the memory
    // attributes for a large page allocatin, the only special case is a
    // one to one mapping.  In this case, make a physical page allocation
    // and explicitly map it one to one.  In the default case, simply
    // allocate a buffer and let the memory manager map the buffer.
    //

    if (CHECK_FLAG(Flags, IMAGE_FLAGS_MAP_ONE_TO_ONE)) {
        PhysicalAddress.QuadPart = (ULONG_PTR)AllocatedBase;
        Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                           ImageSize >> PAGE_SHIFT,
                                           MemoryType,
                                           PreferredAttributes,
                                           PreferredAlignment);

        if (!NT_SUCCESS(Status)) {

            //
            // If the allocation failed, try again without any memory
            // region preferences.  (Allocation preferences are still
            // required).
            //

            CLEAR_OTHER_FLAGS(PreferredAttributes,
                              MEMORY_ATTRIBUTE_ALLOCATION_MASK);

            PhysicalAddress.QuadPart = 0;
            Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                               ImageSize >> PAGE_SHIFT,
                                               MemoryType,
                                               PreferredAttributes,
                                               0);

            if (!NT_SUCCESS(Status)) {
                goto AllocateImageBufferEnd;
            }
        }

        //
        // Create the one to one virtual to physical mapping.
        //

        __analysis_assume(PhysicalAddress.QuadPart != 0);
        AllocatedBase = (PVOID)(ULONG_PTR)PhysicalAddress.QuadPart;
        Status = BlMmMapPhysicalAddress(
                     &AllocatedBase,
                     PhysicalAddress,
                     ImageSize,
                     MEMORY_ATTRIBUTE_ALLOCATION_FIXED_ADDRESS);

        if (!NT_SUCCESS(Status)) {
            BlMmFreePhysicalPages(PhysicalAddress);
            goto AllocateImageBufferEnd;
        }

    } else {
        Status = BlMmAllocatePages(&AllocatedBase,
                                   ImageSize >> PAGE_SHIFT,
                                   MemoryType,
                                   PreferredAttributes,
                                   PreferredAlignment);

        if (!NT_SUCCESS(Status)) {

            //
            // If the allocation failed, try again without any memory
            // region preferences.  (Allocation preferences are still
            // required.).
            //

            CLEAR_OTHER_FLAGS(PreferredAttributes,
                              MEMORY_ATTRIBUTE_ALLOCATION_MASK);

            AllocatedBase = NULL;
            Status = BlMmAllocatePages(&AllocatedBase,
                                       ImageSize >> PAGE_SHIFT,
                                       MemoryType,
                                       PreferredAttributes,
                                       0);

            if (!NT_SUCCESS(Status)) {
                goto AllocateImageBufferEnd;
            }
        }
    }

    Status = STATUS_SUCCESS;
    *ImageBuffer = AllocatedBase;

AllocateImageBufferEnd:
    BlpInterfaceExit(InterfaceImg);
    return Status;
}


NTSTATUS
BlImgUnallocateImageBuffer (
    __in PVOID ImageBuffer,
    __in ULONG ImageSize,
    __in ULONG Flags
    )

/*++

Routine Description:

    Unallocates a buffer that was previously allocated with
    BlImageAllocateImageBuffer.

Arguments:

    ImageBuffer - Supplies a pointer to an image buffer that was previously
        unloaded.

    ImageSize - Supplies the size of the image for which the buffer was
        allocated.

    Flags - Supplies the IMAGE_FLAGS that the image buffer was allocated with.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if ImageBuffer is NULL or ImageSize is 0.

--*/

{

    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;

    if ((ImageBuffer == NULL) || (ImageSize == 0)) {
        return STATUS_INVALID_PARAMETER;
    }

    ImageSize = ALIGN_RANGE_UP(ImageSize, PAGE_SIZE);
    if (CHECK_FLAG(Flags, IMAGE_FLAGS_MAP_ONE_TO_ONE)) {
        if (BlMmTranslateVirtualAddress(ImageBuffer, &PhysicalAddress)) {
            Status = BlMmUnmapVirtualAddress(ImageBuffer, ImageSize);
            if (NT_SUCCESS(Status)) {
                Status = BlMmFreePhysicalPages(PhysicalAddress);
            }

            return Status;
        }

        return STATUS_INVALID_PARAMETER;
    }

    return BlMmFreePages(ImageBuffer);
}

VOID
BlImgQueryCodeIntegrityBootOptions (
    _In_      PBOOT_APPLICATION_ENTRY ApplicationEntry,
    _Out_opt_ PBOOLEAN IntegrityChecksDisabled,
    _Out_opt_ PBOOLEAN TestSigningEnabled,
    _Out_opt_ PBOOLEAN FlightSigningEnabled
    )

/*++

Routine Description:

    This function extracts any settings related to code integrity from the
    given boot option list.

Arguments:

    ApplicationEntry - Supplies the boot application entry structure
        associated with the boot option list to scan.

    IntegrityChecksDisabled - Supplies a pointer to a boolean that receives
        an indication of whether integrity checks have been disabled through
        a boot option.

    TestSigningEnabled - Supplies a pointer to a boolean that receives an
        indication of whether acceptance of test signed binaries has been
        enabled through a boot option.

    FlightSigningEnabled - Supplies a pointer to a boolean that receives an
        indication of whether acceptance of flight signed binaries has been
        enabled through a boot option.

Return Value:

    None.

--*/

{

    PBOOT_ENTRY_OPTION OptionList;
    BOOLEAN OptionValue;
    NTSTATUS Status;

    //
    // Query the disable integrity checks setting.
    //

    OptionList = ApplicationEntry->Options;

    if (ARGUMENT_PRESENT(IntegrityChecksDisabled))
    {
        Status = BlGetBootOptionBoolean(OptionList,
                                        BCDE_LIBRARY_TYPE_DISABLE_INTEGRITY_CHECKS,
                                        &OptionValue);

        if (!NT_SUCCESS(Status)) {
            OptionValue = FALSE;
        }

        *IntegrityChecksDisabled = OptionValue;
    }

    //
    // Query the test signing acceptance setting.
    //

    if (ARGUMENT_PRESENT(TestSigningEnabled))
    {
        Status = BlGetBootOptionBoolean(
                    OptionList,
                    BCDE_LIBRARY_TYPE_ALLOW_PRERELEASE_SIGNATURES,
                    &OptionValue);

        if (!NT_SUCCESS(Status)) {
            OptionValue = FALSE;
        }

        *TestSigningEnabled = OptionValue;
    }

    //
    // Query the flight signing acceptance setting.
    //

    if (ARGUMENT_PRESENT(FlightSigningEnabled))
    {
        //
        // The Secure Boot policy takes precedence over BCD.
        //

        OptionValue = BlSecureBootArePolicyOptionsSet(
                        SECUREBOOT_POLICY_ENABLE_FLIGHT_SIGNING);

        if (OptionValue == FALSE)
        {
            //
            // Work around for Phone to not allow BCD to enable flightsigning.
            //

            if (BlSecureBootArePolicyOptionsSet(
                SECUREBOOT_POLICY_ALLOW_SEQUERYSIGNINGPOLICY_EXTENSION) == FALSE)
            {
                Status = BlGetBootOptionBoolean(
                            OptionList,
                            BCDE_LIBRARY_TYPE_ALLOW_FLIGHT_SIGNATURES,
                            &OptionValue);

                if (!NT_SUCCESS(Status)) {
                    OptionValue = FALSE;
                }
            }
        }

        *FlightSigningEnabled = OptionValue;
    }

    return;
}

NTSTATUS
BlImgRegisterCodeIntegrityCatalogs (
    __in DEVICE_ID DeviceId,
    __in PWSTR CatalogDirectoryPath,
    __in PWSTR SystemCatalogName,
    __in PWSTR RevocationListPath,
    __in_ecount_opt(CatalogPrecedenceListEntries) PWSTR *CatalogPrecedenceList,
    __in_opt ULONG CatalogPrecedenceListEntries,
    __in ULONG CatalogPolicyFlags
    )

/*++

Routine Description:

    Registers a catalog directory and system catalog to use when validating
    image hashes in the boot library's image loader.

Arguments:

    DeviceId - Supplies a device id of the device that the catalog files exists
        on.

    CatalogDirectoryPath - Supplies a path to catalog directory.

    SystemCatalogName - Supplies a name of boot images catalog file.

    RevocationListPath - Supplies a path to the revocation file to load.

    TestSigningEnabled - Supplies the value of /TESTSIGNING boot option.

    CatalogPrecedenceList - Supplies an optional array of catalog prefixes that
        are given precedence when loading catalog files.

        e.g., If "foo" and "bar" are supplied in the precedence list, all
              catalogs with filenames starting with "foo" or "bar" will be
              loaded before all other catalog files (excluding the system
              catalog).

    CatalogPrecedenceListEntries - Supplies the number of entries in the
        catalog precedence list.

    CatalogPolicyFlags - Supplies policy for code integrity catalog usage.
        Flags include:

        CATALOG_FLAGS_SYSTEM_CATALOG_ONLY - Limits code integrity to loading
            the system catalog only.  The catalog directory will not be
            enumerated.

Return Value:

    STATUS_SUCCESS if function succeeded.

    Error status code if returned from a sub-routine.

--*/

{

    ULONG AllocationSize;
    BOOLEAN BoolStatus;
    ULONG Index;
    ULONG Length;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceImg);
    Status = STATUS_SUCCESS;

    Status = ImgpLoadRevocationList(DeviceId, RevocationListPath);
    if (!NT_SUCCESS(Status)) {
        goto RegisterCodeIntegrityCatalogsEnd;
    }

    //
    // Initialize global code integrity variables to reflect the specified
    // catalogs.  Invalidate any values that may have been previously assigned.
    //

    Length = (ULONG)wcslen(CatalogDirectoryPath);
    if (BlImgCatalogDirectoryPath != NULL) {
        BlMmFreeHeap(BlImgCatalogDirectoryPath);
    }

    BlImgCatalogDirectoryPath = BlMmAllocateHeap((Length + 1) * sizeof(WCHAR));
    if (BlImgCatalogDirectoryPath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto RegisterCodeIntegrityCatalogsEnd;
    }

    wcscpy_s(BlImgCatalogDirectoryPath, Length + 1, CatalogDirectoryPath);
    BlImgCatalogDeviceId = DeviceId;
    BlImgCatalogEnumerationFinished = FALSE;
    BlImgCatalogEnumerationPass = 0;
    BlImgSystemCatalogLoaded = FALSE;
    BlImgSystemCatalogName = SystemCatalogName;
    if (BlImgLastFoundCatalogFileName != NULL) {
        BlMmFreeHeap(BlImgLastFoundCatalogFileName);
    }

    BlImgLastFoundCatalogFileName = NULL;
    BlImgLastFoundCatalogFileNameSize = 0;

    if (ARGUMENT_PRESENT(CatalogPrecedenceList) != FALSE) {
        if (CatalogPrecedenceListEntries == 0) {
            Status = STATUS_INVALID_PARAMETER;
            goto RegisterCodeIntegrityCatalogsEnd;
        }

        AllocationSize = CatalogPrecedenceListEntries * sizeof(UNICODE_STRING);
        BlImgCatalogPrecedenceList = BlMmAllocateHeap(AllocationSize);
        if (BlImgCatalogPrecedenceList == NULL) {
            Status = STATUS_NO_MEMORY;
            goto RegisterCodeIntegrityCatalogsEnd;
        }

        for (Index = 0; Index < CatalogPrecedenceListEntries; Index += 1) {
            BoolStatus = RtlCreateUnicodeString(
                             &BlImgCatalogPrecedenceList[Index],
                             CatalogPrecedenceList[Index]);

            if (BoolStatus == FALSE) {
                Status = STATUS_NO_MEMORY;
                goto RegisterCodeIntegrityCatalogsEnd;
            }
        }

        BlImgCatalogPrecedenceListEntries = CatalogPrecedenceListEntries;

    } else {
        BlImgCatalogPrecedenceList = NULL;
        BlImgCatalogPrecedenceListEntries = 0;
    }

    BlImgCatalogPolicy = CatalogPolicyFlags;

RegisterCodeIntegrityCatalogsEnd:
    BlpInterfaceExit(InterfaceImg);
    return Status;
}

NTSTATUS
BlImgLoadRevocationList (
    __in DEVICE_ID RevocationListDevice,
    __in PWSTR RevocationListPath
    )

/*++

Routine Description:

    This routine loads a revocation list into memory and registers it with
    the MinCrypL library.

Arguments:

    RevocationListDevice - Supplies the id of the device that the revocation
         list resides upon.

    RevocationListPath   - Supplies a path to the revocation file to load.

Return Value:

    STATUS_SUCCESS if function succeeded.

    STATUS_NO_MEMORY if memory allocation failed.

    STATUS_INVALID_IMAGE_HASH if revocation list is invalid.

    Another error status code if returned from a sub-routine.

--*/

{

    return ImgpLoadRevocationList(RevocationListDevice, RevocationListPath);
}

NTSTATUS
ImgpParseRevocationList (
    __in BOOLEAN IsBootRevocationList,
    __in_bcount(RevocationListSize) PVOID RevocationList,
    __in ULONG RevocationListSize,
    __deref_bcount_opt(*ParsedRevocationListSize) PVOID* ParsedRevocationList,
    __out PULONG ParsedRevocationListSize,
    __out_opt PLARGE_INTEGER ThisUpdateTime
    )

/*++

Routine Description:

    This routine validates a revocation list's signature and then parses it,
    returning a pointer to the parsed list within the signed list.

Arguments:

    IsBootRevocationList - Supplies FALSE if the revocation list is expected to
         be an OS revocation list, and TRUE otherwise.

    RevocationList - Supplies a pointer to the revocation list to be parsed.

    RevocationListSize - Supplies the size in bytes of the revocation list.

    ParsedRevocationList - Receives a pointer to the raw revocation list. This
         is either RevocationListBase if the list is not signed, or a pointer
         to within RevocationListBase if the list is signed.

    ParsedRevocationListSize - Receives the size in bytes of the parsed
         revocation list.

    ThisUpdateTime - Receives the time that the revocation list was created.

Return Value:

    STATUS_SUCCESS if the revocation list was successfully parsed.

    STATUS_INVALID_IMAGE_HASH if signature verification of the revocation list
         failed.

    Other error status codes for other failure cases.

--*/

{
    MINCRYPT_POLICY_INFO PolicyInfo;
    NTSTATUS Status;

    Status = ImgpVerifyMinimalCodeIntegrityInitialization();
    if (!NT_SUCCESS(Status)) {
        goto ParseRevocationListEnd;
    }

    if (ImgpIsSigningPolicySet() != FALSE) {
        Status = MinCrypL_ParseRevocationList(RevocationList,
                                              RevocationListSize,
                                              NULL,
                                              ParsedRevocationList,
                                              ParsedRevocationListSize,
                                              &PolicyInfo,
                                              ThisUpdateTime);

        if (!NT_SUCCESS(Status)) {
            goto ParseRevocationListEnd;
        }

        Status = BlSecurebootValidateSigningPolicy(
                    IsBootRevocationList != FALSE ?
                        ImgSigningScenarioBootRevocationList :
                        ImgSigningScenarioOsRevocationList,
                    PolicyInfo.pChainInfo,
                    TRUE,
                    NULL);

        if (!NT_SUCCESS(Status)) {
            goto ParseRevocationListEnd;
        }
    }
    else {
        Status = MinCrypL_ParseRevocationList(RevocationList,
                                              RevocationListSize,
                                              NULL,
                                              ParsedRevocationList,
                                              ParsedRevocationListSize,
                                              &PolicyInfo,
                                              ThisUpdateTime);

        if (!NT_SUCCESS(Status)) {
            goto ParseRevocationListEnd;
        }

        //
        // Verify that the revocation list is production signed, unless this is
        // a test-signed build in which case Windows test-signed is acceptable.
        //

        if ((PolicyInfo.ulPolicyBits & (MINCRYPT_POLICY_PRSROOT_CURRENT
#if !defined(BUILD_PRS_SIGNED)
                                        | MINCRYPT_POLICY_TESTROOT_CURRENT
#endif
            )) == 0) {

            Status = STATUS_INVALID_IMAGE_HASH;
            goto ParseRevocationListEnd;
        }

        if (MincryptIsEKUInPolicy(&PolicyInfo,
                                  szOID_MINCRYPT_REVOKED_LIST_SIGNER) == FALSE)
        {
            if ((PolicyInfo.ulPolicyBits & MINCRYPT_POLICY_TESTROOT_CURRENT) == 0 ||
                MincryptIsEKUInPolicy(&PolicyInfo, szOID_NT5_CRYPTO) == FALSE)
            {
                Status = STATUS_INVALID_IMAGE_HASH;
                goto ParseRevocationListEnd;
            }
        }
    }

ParseRevocationListEnd:
    return Status;
}

NTSTATUS
BlImgParseRevocationList (
    __in_bcount(RevocationListSize) PVOID RevocationList,
    __in ULONG RevocationListSize,
    __deref_bcount_opt(*ParsedRevocationListSize) PVOID* ParsedRevocationList,
    __out PULONG ParsedRevocationListSize,
    __out_opt PLARGE_INTEGER ThisUpdateTime
    )

/*++

Routine Description:

    This routine validates a revocation list's signature and then parses it,
    returning a pointer to the parsed list within the signed list.

Arguments:

    RevocationList - Supplies a pointer to the revocation list to be parsed.

    RevocationListSize - Supplies the size in bytes of the revocation list.

    ParsedRevocationList - Receives a pointer to the raw revocation list. This
         is either RevocationListBase if the list is not signed, or a pointer
         to within RevocationListBase if the list is signed.

    ParsedRevocationListSize - Receives the size in bytes of the parsed
         revocation list.

    ThisUpdateTime - Receives the time that the revocation list was created.

Return Value:

    STATUS_SUCCESS if the revocation list was successfully parsed.

    STATUS_INVALID_IMAGE_HASH if signature verification of the revocation list
         failed.

    Other error status codes for other failure cases.

--*/

{
    LARGE_INTEGER BootStlCreationTime;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceImg);

    Status = ImgpParseRevocationList(TRUE,
                                     RevocationList,
                                     RevocationListSize,
                                     ParsedRevocationList,
                                     ParsedRevocationListSize,
                                     &BootStlCreationTime);

    if (!NT_SUCCESS(Status)) {
        goto ParseRevocationListEnd;
    }

    if (BootStlCreationTime.QuadPart < BlpImgBootStlMinimumTimeOfTrust.QuadPart) {
        Status = STATUS_SECUREBOOT_ROLLBACK_DETECTED;
        goto ParseRevocationListEnd;
    }

    if (ARGUMENT_PRESENT(ThisUpdateTime)) {
        ThisUpdateTime->QuadPart = BootStlCreationTime.QuadPart;
    }

ParseRevocationListEnd:
    BlpInterfaceExit(InterfaceImg);
    return Status;
}

NTSTATUS
BlImgParseOsRevocationList (
    _In_reads_bytes_(RevocationListSize) PVOID RevocationList,
    _In_ ULONG RevocationListSize,
    _Outptr_result_buffer_(*ParsedRevocationListSize) PVOID* ParsedRevocationList,
    _Out_ PULONG ParsedRevocationListSize,
    _Out_ PLARGE_INTEGER ThisUpdateTime
    )

/*++

Routine Description:

    This routine validates a revocation list's signature and then parses it,
    returning a pointer to the parsed list within the signed list.

Arguments:

    RevocationList - Supplies a pointer to the revocation list to be parsed.

    RevocationListSize - Supplies the size in bytes of the revocation list.

    ParsedRevocationList - Receives a pointer to the raw revocation list. This
         is either RevocationListBase if the list is not signed, or a pointer
         to within RevocationListBase if the list is signed.

    ParsedRevocationListSize - Receives the size in bytes of the parsed
         revocation list.

    ThisUpdateTime - Receives the time that the revocation list was created.

Return Value:

    STATUS_SUCCESS if the revocation list was successfully parsed.

    STATUS_INVALID_IMAGE_HASH if signature verification of the revocation list
         failed.

    Other error status codes for other failure cases.

--*/

{
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceImg);

    Status = ImgpParseRevocationList(FALSE,
                                     RevocationList,
                                     RevocationListSize,
                                     ParsedRevocationList,
                                     ParsedRevocationListSize,
                                     ThisUpdateTime);

    BlpInterfaceExit(InterfaceImg);
    return Status;
}

VOID
BlImgSetCatalogHint (
    __in PWSTR Catalog
    )

/*++

Routine Description:

    This routine configures a catalog hint that can be used to optimize
    signature validation in cases where the catalog name is known.

Arguments:

    Catalog - Supplies the name of the catalog file.

Return Value:

    None.

--*/

{

    ULONG Size;

    if (Catalog == NULL) {
        return;
    }

    Size = ((ULONG)wcslen(Catalog) + 1) * sizeof(WCHAR);
    BlImgCatalogHint = BlMmAllocateHeap(Size);
    if (BlImgCatalogHint != NULL) {
        RtlCopyMemory(BlImgCatalogHint, Catalog, Size);

    }

    return;
}

NTSTATUS
BlImgSetParsedRevocationList(
    __in_bcount(RevocationListSize) PVOID RevocationList,
    __in ULONG RevocationListSize,
    __in PLARGE_INTEGER CreationTime
    )

/*++

Routine Description:

    This routine sets a previously validated and parsed revocation list and
    appends it to the measured boot log (when enabled).

Arguments:

    RevocationList - Supplies a pointer to the parsed revocation list.

    RevocationListSize - Supplies the size in bytes of the revocation list.

    CreationTime - Supplies the creation time of the revocation list.

Return Value:

    STATUS_SUCCESS if the revocation list was successfully set.

    Other error status codes for other failure cases.

--*/

{
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceImg);

    Status = MinCrypL_SetParsedRevocationList(RevocationList,
                                              RevocationListSize,
                                              CreationTime);

    if (NT_SUCCESS(Status)) {
        BlSiMeasureBootRevocationList(RevocationList,
                                      RevocationListSize,
                                      CreationTime);
    }

    BlpInterfaceExit(InterfaceImg);
    return Status;
}

NTSTATUS
BlImgSetParsedRevocationListWithoutMeasurement(
    __in_bcount(RevocationListSize) PVOID RevocationList,
    __in ULONG RevocationListSize,
    __in PLARGE_INTEGER CreationTime
    )

/*++

Routine Description:

    This routine sets a previously validated and parsed revocation list.

    The revocation list will not be measured into the Measured Boot log.

Arguments:

    RevocationList - Supplies a pointer to the parsed revocation list.

    RevocationListSize - Supplies the size in bytes of the revocation list.

    CreationTime - Supplies the creation time of the revocation list.

Return Value:

    STATUS_SUCCESS if the revocation list was successfully set.

    Other error status codes for other failure cases.

--*/

{
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceImg);

    Status = MinCrypL_SetParsedRevocationList(RevocationList,
                                              RevocationListSize,
                                              CreationTime);

    BlpInterfaceExit(InterfaceImg);
    return Status;
}

NTSTATUS
BlImgVerifySignedPeImageFileContents (
    __in PWSTR ApplicationPath,
    __in PVOID ImageBase,
    __in ULONG ImageSize,
    __in ULONG ImageFlags,
    __in BOOLEAN NotifyEvent
    )

/*++

Routine Description:

    Verifies the given boot application file contents against the system
    catalogs or an embedded certificate.

Arguments:

    ApplicationPath - Supplies a path to the boot environment application
        associated with the given image buffer.

    ImageBase - Supplies the base address of the image buffer holding the
        file contents.

    ImageSize - Supplies the size in bytes of the image buffer.

    ImageFlags - Supplies configuration flags indicating the verification
        policy applied to this application.

    NotifyEvent - Supplies a flag indicating if the hash validation needs
        to be logged.

Return Value:

    STATUS_SUCCESS if function succeeded.

    Error status code if returned from a sub-routine.

--*/

{

    BOOT_TRUSTED_BOOT_INFORMATION BootTrustedBootInformation;
    ULONG BytesHashed;
    PCHAR CertAddress;
    PIMAGE_DATA_DIRECTORY CertDirectory;
    ULONG Flags;
    ALG_ID HashAlgorithm;
    UCHAR HashBuffer[MAX_HASH_LEN];
    PCRYPTO_HASH_CONTEXT HashContext;
    PUCHAR HashPointer;
    ULONG HashSize;
    ALG_ID MinimumHashAlgorithm;
    PIMAGE_NT_HEADERS NtHeader;
    MINCRYPT_SIGNATURE_DESCRIPTOR Signature;
    NTSTATUS Status;
    NOTIFICATION_EVENT_CONTEXT EventContext;
    BOOLEAN ValidHashFound;

    BlpInterfaceEnter(InterfaceImg);

    //
    // Hash the file, skipping checksum, cert directory, and cert itself.
    //

    CertAddress = NULL;
    ValidHashFound = FALSE;
    BytesHashed = 0;
    HashAlgorithm = 0;
    HashContext = NULL;

    RtlZeroMemory(&BootTrustedBootInformation,
                  sizeof(BootTrustedBootInformation));

    RtlZeroMemory(&EventContext,
                  sizeof(EventContext));

    Status = BlImgGetNtHeader(ImageBase, ImageSize, &NtHeader);
    if (!NT_SUCCESS(Status)) {
        goto VerifyBootApplicationEnd;
    }

    //
    // Boot applications must be embed signed.
    //

    Status = BlImgGetValidatedCertificateLocation(NtHeader,
                                                  ImageBase,
                                                  ImageSize,
                                                  &CertDirectory,
                                                  &CertAddress);
    if (!NT_SUCCESS(Status)) {
        goto VerifyBootApplicationEnd;
    }

    if (CertAddress == NULL) {
        Status = STATUS_INVALID_IMAGE_HASH;
        goto VerifyBootApplicationEnd;
    }

    //
    // Determine the hash algorithm to use.
    //

    Status = ImgpVerifyMinimalCodeIntegrityInitialization();
    if (!NT_SUCCESS(Status)) {
        goto VerifyBootApplicationEnd;
    }

    //
    // Although we are verifying a boot application, it is in the context of
    // a boot application for the current version of Windows only.
    //

    MinimumHashAlgorithm = ImgpGetHashAlgorithmForScenario(
                                ImgSigningScenarioWindows);

    Status = MinCryptGetHashAlgorithmFromWinCertificate(
                    CertAddress,
                    CertDirectory->Size,
                    MinimumHashAlgorithm,
                    MinimumHashAlgorithm,
                    &HashAlgorithm,
                    &Signature,
                    NULL);

    if (!NT_SUCCESS(Status)) {
        goto VerifyBootApplicationEnd;
    }

    Status = BlCryptoHashInitialize(HashAlgorithm, &HashContext, &HashSize);
    if (!NT_SUCCESS(Status)) {
        goto VerifyBootApplicationEnd;
    }

    HashPointer = (PUCHAR)BlImgGetChecksumAddress(NtHeader);

    Status = BlCryptoHashUpdate(HashContext,
                                ImageBase,
                                (ULONG)PtrOffset(ImageBase, HashPointer));
    if (!NT_SUCCESS(Status)) {
        goto VerifyBootApplicationEnd;
    }

    HashPointer = Add2Ptr(HashPointer, sizeof(ULONG));
    Status = BlCryptoHashUpdate(HashContext,
                                HashPointer,
                                (ULONG)PtrOffset(HashPointer, CertDirectory));
    if (!NT_SUCCESS(Status)) {
        goto VerifyBootApplicationEnd;
    }

    HashPointer = Add2Ptr(CertDirectory, sizeof(IMAGE_DATA_DIRECTORY));
    BytesHashed = (ULONG)PtrOffset(ImageBase, HashPointer);
    if ((CertDirectory->VirtualAddress == 0) || (CertDirectory->Size == 0)) {
        Status = STATUS_INVALID_IMAGE_HASH;
        goto VerifyBootApplicationEnd;
    }

    if (CertDirectory->VirtualAddress + CertDirectory->Size > ImageSize) {
        Status = STATUS_FILE_INVALID;
        goto VerifyBootApplicationEnd;
    }

    Status = BlCryptoHashUpdate(HashContext,
                                HashPointer,
                                CertDirectory->VirtualAddress - BytesHashed);
    if (!NT_SUCCESS(Status)) {
        goto VerifyBootApplicationEnd;
    }

    //
    // Finalize and validate the image hash.  If the hash is invalid, call the
    // filter routine to allow the failure to be reported to the boot debugger
    // and suppressed if appropriate.  Note that boot applications are
    // required to be self signed and are not subject to checks against any
    // certificate revocation list.
    //

    Status = BlCryptoHashFinalize(HashContext, HashBuffer);
    if (!NT_SUCCESS(Status)) {
        goto VerifyBootApplicationEnd;
    }

    Flags = IMGP_MS_BOOT_APPLICATION_VALIDATION_FLAGS;

    if (CHECK_FLAG(ImageFlags, IMAGE_FLAGS_NO_SIGNATURE_EXPIRATION_CHECKS) != FALSE) {
        SET_FLAGS(Flags, IMGP_NO_SIGNATURE_EXPIRATION_CHECKS);
    }

#ifdef IMG_NO_CATALOG_SUPPORT

    Flags |= IMGP_NO_CATALOG_CHECKS;

#endif

    Status = ImgpValidateImageHash(
                 ApplicationPath,
                 ImageBase,
                 ImageSize,
                 &Signature,
                 HashAlgorithm,
                 HashBuffer,
                 Flags,
                 ImgSigningScenarioWindows,
                 NULL,
                 &BootTrustedBootInformation,
                 NULL);

    if (NT_SUCCESS(Status)) {
        ValidHashFound = TRUE;
    }

    //
    // If the caller specified the notify event flag, send a notification with
    // the validated image information or an event stating a non-validated
    // image was loaded.
    //

    if (NotifyEvent != FALSE) {
        if (ValidHashFound != FALSE) {
            EventContext.u.Hash.FilePath = ApplicationPath;
            BlMmTranslateVirtualAddress(ImageBase,
                                        &EventContext.u.Hash.ImageBase);

            EventContext.u.Hash.ImageSize = ImageSize;
            EventContext.u.Hash.AlgId = HashAlgorithm;
            EventContext.u.Hash.ImageHashSize = HashSize;
            EventContext.u.Hash.ImageHash = HashBuffer;
            EventContext.u.Hash.AuthorityValid = ValidHashFound;
            EventContext.u.Hash.TrustedBootInfo = &BootTrustedBootInformation;

            BlEnNotifyEvent(EVENT_APPLICATION_HASHED, &EventContext);

        } else {
            BlEnNotifyEvent(EVENT_NVPE_IMAGE_LOADED, NULL);
        }
    }

VerifyBootApplicationEnd:
    if (ValidHashFound != FALSE) {
        ImgpDestroyTrustedBootInformation(&BootTrustedBootInformation);
    }

    if (!NT_SUCCESS(Status)) {
        Status = ImgpFilterValidationFailure(ApplicationPath,
                                             Status,
                                             ImageFlags);
    }

    if (HashContext != NULL) {
        BlCryptoHashDestroy(HashContext);
    }

    BlpInterfaceExit(InterfaceImg);
    return Status;
}
NTSTATUS
BlImgSha1KnownAnswerTest (
    __in DEVICE_ID DeviceId,
    __in PWSTR FilePath
    )

/*++

Routine Description:

    Executes a set of known answer test for SHA1 hashing functions. The test
    vectors are read from the file, hashed, and the result is printed into the
    debugger.

Arguments:

    DeviceId - Supplies the id of a device containing the file.

    FilePath - Supplies a path to a file with set of test vectors.

Return Value:

    STATUS_SUCCESS if function succeeded.

    STATUS_NO_MEMORY if memory could not be allocated to process the test.

    STATUS_INVALID_PARAMETER if error is found in a file with test vectors.

    Error status code if returned from a sub-routine.

--*/

{

    ULONGLONG ByteOffset;
    FILE_ID FileId;
    FILE_INFORMATION FileInfo;
    UCHAR Hash[MAX_HASH_LEN];
    ULONG HashSize;
    ULONG Index;
    PUCHAR Message;
    CRYPT_DER_BLOB MessageBlob;
    ULONG MessageSize;
    NTSTATUS Status;
    KAT_HASH_VECTOR Vector;

    BlpInterfaceEnter(InterfaceImg);
    ByteOffset = 0;
    FileId = INVALID_FILE_ID;
    Message = NULL;
    MessageSize = 0;
    Status = ImgpVerifyMinimalCodeIntegrityInitialization();
    if (!NT_SUCCESS(Status)) {
        goto Sha1KnownAnswerTestEnd;
    }

    Status = BlFileOpen(DeviceId, FilePath, OPEN_READ, &FileId);
    if (!NT_SUCCESS(Status)) {
        Status = STATUS_SUCCESS;
        goto Sha1KnownAnswerTestEnd;
    }

    Status = BlFileGetInformation(FileId, &FileInfo);
    if (!NT_SUCCESS(Status)) {
        goto Sha1KnownAnswerTestEnd;
    }

    //
    // Read known answer test vectors one by one and hash them.
    //

    BlStatusPrint(L"[SHA Known Answer Test]\n\n");
    while (ByteOffset < FileInfo.FileSize) {
        Status = BlFileReadAtOffset(FileId,
                                    sizeof(KAT_HASH_VECTOR),
                                    ByteOffset,
                                    &Vector,
                                    NULL);

        if (!NT_SUCCESS(Status)) {
            goto Sha1KnownAnswerTestEnd;
        }

        if (Vector.MessageSize > MessageSize) {
            MessageSize = Vector.MessageSize;
            if (Message != NULL) {
                BlMmFreeHeap(Message);
            }

            Message = (PUCHAR)BlMmAllocateHeap(MessageSize);
            if (Message == NULL) {
                Status = STATUS_NO_MEMORY;
                goto Sha1KnownAnswerTestEnd;
            }
        }

        if (Vector.MessageSize > 0) {
            Status = BlFileReadAtOffset(FileId,
                                        Vector.MessageSize,
                                        Vector.MessageOffset,
                                        Message,
                                        NULL);

            if (!NT_SUCCESS(Status)) {
                goto Sha1KnownAnswerTestEnd;
            }
        }

        BlStatusPrint(L"Len = %d\n", Vector.MessageSize * 8);
        BlStatusPrint(L"Msg = ");
        if (Vector.MessageSize != 0) {
            for (Index = 0; Index < Vector.MessageSize; Index += 1) {
                BlStatusPrint(L"%2.2x", Message[Index]);
            }

        } else {
            BlStatusPrint(L"00");
        }

        BlStatusPrint(L"\n");
        MessageBlob.cbData = Vector.MessageSize;
        MessageBlob.pbData = Message;

        //
        // Calculate hash of the message and print the result.
        //

        Status = MinCrypL_HashMemory(Vector.AlgId,
                                     1,
                                     &MessageBlob,
                                     Hash,
                                     &HashSize);

        if (!NT_SUCCESS(Status)) {
            goto Sha1KnownAnswerTestEnd;
        }

        BlStatusPrint(L"MD = ");
        for (Index = 0; Index < HashSize; Index += 1) {
            BlStatusPrint(L"%2.2x", Hash[Index]);
        }

        BlStatusPrint(L"\n\n");
        if (ByteOffset >= Vector.NextVectorOffset) {
            Status = STATUS_INVALID_PARAMETER;
            goto Sha1KnownAnswerTestEnd;
        }

        ByteOffset = Vector.NextVectorOffset;
    }

Sha1KnownAnswerTestEnd:
    if (Message != NULL) {
        BlMmFreeHeap(Message);
    }

    if (FileId != INVALID_FILE_ID) {
        BlFileClose(FileId);
    }

    BlpInterfaceExit(InterfaceImg);
    return Status;
}

NTSTATUS
BlImgSha1MonteCarloTest (
    __in DEVICE_ID DeviceId,
    __in PWSTR FilePath
    )

/*++

Routine Description:

    Executes a monte carlo test for SHA1 hashing functions. The test
    seed is read from the file, monte carlo function is evaluated multiple
    times and intermidiate checkpoints are printed into the debugger.

Arguments:

    DeviceId - Supplies the id of a device containing the file.

    FilePath - Supplies a path to a file with set of test vectors.

Return Value:

    STATUS_SUCCESS if function succeeded.

    STATUS_NO_MEMORY if memory could not be allocated to process the test.

    STATUS_INVALID_PARAMETER if error is found in a file with test vectors.

    Error status code if returned from a sub-routine.

--*/

{

    ULONGLONG ByteOffset;
    ULONG Count;
    FILE_ID FileId;
    FILE_INFORMATION FileInfo;
    ULONG HashSize;
    KAT_HASH_SEED Header;
    ULONG Index;
    ULONG Iteration;
    UCHAR Message[3][MAX_HASH_LEN];
    CRYPT_DER_BLOB MessageBlob[3];
    UCHAR Seed[MAX_HASH_LEN];
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceImg);
    ByteOffset = 0;
    FileId = INVALID_FILE_ID;

    Status = ImgpVerifyMinimalCodeIntegrityInitialization();
    if (!NT_SUCCESS(Status)) {
        goto Sha1MonteCarloTestEnd;
    }

    Status = BlFileOpen(DeviceId, FilePath, OPEN_READ, &FileId);
    if (!NT_SUCCESS(Status)) {
        Status = STATUS_SUCCESS;
        goto Sha1MonteCarloTestEnd;
    }

    Status = BlFileGetInformation(FileId, &FileInfo);
    if (!NT_SUCCESS(Status)) {
        goto Sha1MonteCarloTestEnd;
    }

    //
    // Read test seeds one by one and perform monte carlo test.
    //

    BlStatusPrint(L"[SHA Monte Carlo Test]\n\n");
    while (ByteOffset < FileInfo.FileSize) {
        Status = BlFileReadAtOffset(FileId,
                                    sizeof(KAT_HASH_SEED),
                                    ByteOffset,
                                    &Header,
                                    NULL);

        if (!NT_SUCCESS(Status)) {
            goto Sha1MonteCarloTestEnd;
        }

        if (Header.SeedSize > MAX_HASH_LEN) {
            Status = STATUS_INVALID_PARAMETER;
            goto Sha1MonteCarloTestEnd;
        }

        Status = BlFileReadAtOffset(FileId,
                                    Header.SeedSize,
                                    Header.SeedOffset,
                                    Seed,
                                    NULL);

        if (!NT_SUCCESS(Status)) {
            goto Sha1MonteCarloTestEnd;
        }

        BlStatusPrint(L"Seed = ");
        for (Index = 0; Index < Header.SeedSize; Index += 1) {
            BlStatusPrint(L"%2.2x", Seed[Index]);
        }

        BlStatusPrint(L"\n\n");
        for (Count = 0; Count < 100; Count += 1) {

            //
            // MD0 = MD1 = MD2 = Seed;
            //

            for (Index = 0; Index < 3; Index += 1) {
                RtlCopyMemory(Message[Index], Seed, Header.SeedSize);
            }

            for (Iteration = 0; Iteration < 1000; Iteration += 1) {

                //
                // Mi = MDi-3 || MDi-2 || MDi-1;
                //

                MessageBlob[0].cbData = Header.SeedSize;
                MessageBlob[0].pbData = Message[Iteration % 3];
                MessageBlob[1].cbData = Header.SeedSize;
                MessageBlob[1].pbData = Message[(Iteration + 1) % 3];
                MessageBlob[2].cbData = Header.SeedSize;
                MessageBlob[2].pbData = Message[(Iteration + 2) % 3];

                //
                // MDi = SHA(Mi);
                //

                Status = MinCrypL_HashMemory(Header.AlgId,
                                             3,
                                             MessageBlob,
                                             Seed,
                                             &HashSize);

                if (!NT_SUCCESS(Status)) {
                    goto Sha1MonteCarloTestEnd;
                }

                RtlCopyMemory(Message[Iteration%3], Seed, Header.SeedSize);
            }

            BlStatusPrint(L"COUNT = %d\n", Count);
            BlStatusPrint(L"MD = ");
            for (Index = 0; Index < Header.SeedSize; Index += 1) {
                BlStatusPrint(L"%2.2x", Seed[Index]);
            }

            BlStatusPrint(L"\n\n");
        }

        if (ByteOffset >= Header.NextSeedOffset) {
            Status = STATUS_INVALID_PARAMETER;
            goto Sha1MonteCarloTestEnd;
        }

        ByteOffset = Header.NextSeedOffset;
    }

Sha1MonteCarloTestEnd:
    if (FileId != INVALID_FILE_ID) {
        BlFileClose(FileId);
    }

    BlpInterfaceExit(InterfaceImg);
    return Status;
}

NTSTATUS
BlImgRsaKnownAnswerTest (
    __in DEVICE_ID DeviceId,
    __in PWSTR FilePath
    )

/*++

Routine Description:

    Executes a set of known answer test for RSA signature verification
    function. The set of test messages and signatures is read from the
    file, the signature for each message is verified and the result is
    printed into the debugger.

Arguments:

    DeviceId - Supplies the id of a device containing the file.

    FilePath - Supplies a path to a file with set of test vectors.

Return Value:

    STATUS_SUCCESS if function succeeded.

    STATUS_NO_MEMORY if memory could not be allocated to process the test.

    STATUS_INVALID_PARAMETER if error is found in a file with test vectors.

    Error status code if returned from a sub-routine.

--*/

{

    ULONGLONG ByteOffset;
    PUCHAR Exponent;
    ULONG ExponentSize;
    ULONG ExponentValue;
    FILE_ID FileId;
    FILE_INFORMATION FileInfo;
    ULONG Index;
    PUCHAR Message;
    ULONG MessageSize;
    PUCHAR Modulus;
    ULONG ModulusSize;
    ULONG PrevModulusSize;
    PUCHAR Signature;
    ULONG SignatureSize;
    NTSTATUS Status;
    KAT_SIGNATURE_VECTOR Vector;
    ULONG VectorCount;

    BlpInterfaceEnter(InterfaceImg);
    ByteOffset = 0;
    Exponent = NULL;
    ExponentSize = 0;
    FileId = INVALID_FILE_ID;
    Message = NULL;
    MessageSize = 0;
    Modulus = NULL;
    ModulusSize = 0;
    PrevModulusSize = 0;
    Signature = NULL;
    SignatureSize = 0;
    VectorCount = 0;

    Status = ImgpVerifyMinimalCodeIntegrityInitialization();
    if (!NT_SUCCESS(Status)) {
        goto RsaKnownAnswerTestEnd;
    }

    Status = BlFileOpen(DeviceId, FilePath, OPEN_READ, &FileId);
    if (!NT_SUCCESS(Status)) {
        Status = STATUS_SUCCESS;
        goto RsaKnownAnswerTestEnd;
    }

    Status = BlFileGetInformation(FileId, &FileInfo);
    if (!NT_SUCCESS(Status)) {
        goto RsaKnownAnswerTestEnd;
    }

    //
    // Read known answer test vectors one by one and verify their signature.
    //

    while (ByteOffset < FileInfo.FileSize) {
        Status = BlFileReadAtOffset(FileId,
                                    sizeof(KAT_SIGNATURE_VECTOR),
                                    ByteOffset,
                                    &Vector,
                                    NULL);

        if (!NT_SUCCESS(Status)) {
            goto RsaKnownAnswerTestEnd;
        }

        //
        // Read the modulus of the public key.
        //

        if (Vector.ModulusSize != PrevModulusSize) {
            BlStatusPrint(L"[mod = %d]\n\n", Vector.ModulusSize*8);
        }

        if (Vector.ModulusSize > ModulusSize) {
            ModulusSize = Vector.ModulusSize;
            if (Modulus != NULL) {
                BlMmFreeHeap(Modulus);
            }

            Modulus = (PUCHAR)BlMmAllocateHeap(Vector.ModulusSize);
            if (Modulus == NULL) {
                Status = STATUS_NO_MEMORY;
                goto RsaKnownAnswerTestEnd;
            }
        }

        Status = BlFileReadAtOffset(FileId,
                                    Vector.ModulusSize,
                                    Vector.ModulusOffset,
                                    Modulus,
                                    NULL);

        if (!NT_SUCCESS(Status)) {
            goto RsaKnownAnswerTestEnd;
        }

        //
        // Print every 4th modulus.
        //

        if ((VectorCount % 4) == 0) {
            BlStatusPrint(L"n = ");
            for (Index = 0; Index < Vector.ModulusSize; Index += 1) {
                BlStatusPrint(L"%2.2x", Modulus[Index]);
            }

            BlStatusPrint(L"\n\n");
        }

        if (Vector.AlgId == CALG_SHA1) {
            BlStatusPrint(L"SHAAlg = SHA1\n");

        } else if (Vector.AlgId == CALG_SHA_256) {
            BlStatusPrint(L"SHAAlg = SHA256\n");

        } else if (Vector.AlgId == CALG_SHA_384) {
            BlStatusPrint(L"SHAAlg = SHA384\n");

        } else if (Vector.AlgId == CALG_SHA_512) {
            BlStatusPrint(L"SHAAlg = SHA512\n");

        } else {
            BlStatusPrint(L"SHAAlg = Unknown\n");
        }

        //
        // Read the exponent of the public key.
        //

        if (Vector.ExponentSize > ExponentSize) {
            ExponentSize = Vector.ExponentSize;
            if (Exponent != NULL) {
                BlMmFreeHeap(Exponent);
            }

            Exponent = (PUCHAR)BlMmAllocateHeap(ExponentSize);
            if (Exponent == NULL) {
                Status = STATUS_NO_MEMORY;
                goto RsaKnownAnswerTestEnd;
            }
        }

        Status = BlFileReadAtOffset(FileId,
                                    Vector.ExponentSize,
                                    Vector.ExponentOffset,
                                    Exponent,
                                    NULL);

        if (!NT_SUCCESS(Status)) {
            goto RsaKnownAnswerTestEnd;
        }

        BlStatusPrint(L"e = ");
        for (Index = 0; Index < Vector.ExponentSize; Index += 1) {
            BlStatusPrint(L"%2.2x", Exponent[Index]);
        }

        BlStatusPrint(L"\n");

        //
        // The exponent value must be smaller then MAX_ULONG.
        //

        ExponentValue = 0;
        for (Index = 0; Index < sizeof(ULONG); Index += 1) {
            if (Vector.ExponentSize > Index) {
                ExponentValue |=
                    Exponent[Vector.ExponentSize - Index - 1] << (8 * Index);
            }
        }

        if (Vector.ExponentSize > sizeof(ULONG)) {
            for (Index = 0;
                 Index < (Vector.ExponentSize - sizeof(ULONG));
                 Index += 1) {
                if (Exponent[Index] != 0) {
                    BlStatusPrint(L"Exponent is too big\n");
                    Status = STATUS_INVALID_PARAMETER;
                    goto RsaKnownAnswerTestEnd;
                }
            }
        }

        //
        // Read the message.
        //

        if (Vector.MessageSize > MessageSize) {
            MessageSize = Vector.MessageSize;
            if (Message != NULL) {
                BlMmFreeHeap(Message);
            }

            Message = (PUCHAR)BlMmAllocateHeap(MessageSize);
            if (Message == NULL) {
                Status = STATUS_NO_MEMORY;
                goto RsaKnownAnswerTestEnd;
            }
        }

        Status = BlFileReadAtOffset(FileId,
                                    Vector.MessageSize,
                                    Vector.MessageOffset,
                                    Message,
                                    NULL);

        if (!NT_SUCCESS(Status)) {
            goto RsaKnownAnswerTestEnd;
        }

        BlStatusPrint(L"Msg = ");
        for (Index = 0; Index < Vector.MessageSize; Index += 1) {
            BlStatusPrint(L"%2.2x", Message[Index]);
        }

        BlStatusPrint(L"\n");

        //
        // Read the signature to be verified.
        //

        if (Vector.SignatureSize > SignatureSize) {
            SignatureSize = Vector.SignatureSize;
            if (Signature != NULL) {
                BlMmFreeHeap(Signature);
            }

            Signature = (PUCHAR)BlMmAllocateHeap(SignatureSize);
            if (Signature == NULL) {
                Status = STATUS_NO_MEMORY;
                goto RsaKnownAnswerTestEnd;
            }
        }

        Status = BlFileReadAtOffset(FileId,
                                    Vector.SignatureSize,
                                    Vector.SignatureOffset,
                                    Signature,
                                    NULL);

        if (!NT_SUCCESS(Status)) {
            goto RsaKnownAnswerTestEnd;
        }

        BlStatusPrint(L"S = ");
        for (Index = 0; Index < Vector.SignatureSize; Index += 1) {
            BlStatusPrint(L"%2.2x", Signature[Index]);
        }

        BlStatusPrint(L"\n");
        if (MincrypL_TestPKCS1SignVerify(Vector.AlgId,
                                         Message,
                                         Vector.MessageSize,
                                         ExponentValue,
                                         Modulus,
                                         Vector.ModulusSize,
                                         Signature,
                                         Vector.SignatureSize)) {
            BlStatusPrint(L"Result = P\n\n");

        } else {
            BlStatusPrint(L"Result = F\n\n");
        }

        if (ByteOffset >= Vector.NextVectorOffset) {
            Status = STATUS_INVALID_PARAMETER;
            goto RsaKnownAnswerTestEnd;
        }

        PrevModulusSize = Vector.ModulusSize;
        VectorCount += 1;
        ByteOffset = Vector.NextVectorOffset;
    }

RsaKnownAnswerTestEnd:
    if (Exponent != NULL) {
        BlMmFreeHeap(Exponent);
    }

    if (FileId != INVALID_FILE_ID) {
        BlFileClose(FileId);
    }

    if (Message != NULL) {
        BlMmFreeHeap(Message);
    }

    if (Modulus != NULL) {
        BlMmFreeHeap(Modulus);
    }

    if (Signature != NULL) {
        BlMmFreeHeap(Signature);
    }

    BlpInterfaceExit(InterfaceImg);
    return Status;
}

NTSTATUS
ImgpClassifyPEBootAppImage (
    __in_bcount(ImageSize) PVOID ImageBase,
    __in ULONG ImageSize,
    __inout PULONG LoadInformation
)

/*++

Routine Description:

    This routine will call into the boot SI Policy library to classify
    the image specified.  Based upon the information returned this 
    routine will add load flags indicating the ability of the boot
    application to be trusted to finalize the boot environment state
    with respect to TPM measurements.

    Note: The flags in question need to be evaluated in conjunction with
          signing and policy flags.  The version resource can only be
          trusted if the PE image is correctly signed and the signing 
          conforms to policy.

Arguments:

    ImageBase - A pointer to the PE image base, loaded into memory and
        having resource information appropriately relocated.

    ImageSize - The size of the loaded PE image.

    LoadInformation - Supplies a pointer to a variable that receives
        informational flags describing various aspects of the load.  The
        following flags may be added by this function:

        LOAD_INFORMATION_IMAGE_BOOT_SCENARIO_AWARE : Indicates that the image
             is aware of any boot scenario specific system integrity operations
             that must be performed to preserve the integrity of those scenarios.

        LOAD_INFORMATION_IMAGE_BOOT_SCENARIO_AGNOSTIC : Indicates that the image will not
             implement any boot application scenario system integrity operations but
             is not required to because it is known not to load any other boot
             application or exit the boot environment.

Return Value:

    NT status code.

--*/

{

    ULONG SiImageFlags;
    NTSTATUS Status;

    Status = BlSIPolicyClassifyBootApplicationImage(ImageBase,
                                                    ImageSize,
                                                    &SiImageFlags);

    if (!NT_SUCCESS(Status)) {
        goto ClassifyPEBootAppImageEnd;
    }

    if (CHECK_FLAG(SiImageFlags, SI_IMG_SCENARIO_AWARE) != FALSE) {
        *LoadInformation |= LOAD_INFORMATION_IMAGE_BOOT_SCENARIO_AWARE;
    }

    if (CHECK_FLAG(SiImageFlags, SI_IMG_SCENARIO_AGNOSTIC) != FALSE) {
        *LoadInformation |= LOAD_INFORMATION_IMAGE_BOOT_SCENARIO_AGNOSTIC;
    }

ClassifyPEBootAppImageEnd:

    return Status;
}

NTSTATUS
ImgpLoadPEImage (
    __in PIMG_FILE_HANDLE FileHandle,
    __in MEMORY_TYPE MemoryType,
    __deref_inout_bcount_opt(*ImageSize) PVOID *ImageBase,
    __out_opt PULONG ImageSize,
    __out_bcount_opt(MAX_HASH_LEN) PUCHAR ImageHash,
    __out_opt PULONG ImageHashLength,
    __out_opt PULONG ImageHashAlgorithm,
    __in ULONG PreferredAttributes,
    __in ULONG PreferredAlignment,
    __in ULONG Flags,
    __out PULONG LoadInformation,
    __out_opt PBOOT_TRUSTED_BOOT_INFORMATION BootTrustedBootInformation
    )

/*++

Routine Description:

    This routine loads a PE image into memory, expanding the file and when
    appropriate, performs relocations.

Arguments:

    FileHandle - Supplies a pointer to an image file handle.

    MemoryType - Supplies the type of memory to to be assigned to the allocated
        memory descriptor.

    ImageBase - Supplies a pointer to a variable that contains the preferred
        image base.  A supplied value of NULL indicates no preference.  On
        output, the variable contains the address of the loaded image.

    ImageSize - Supplies a pointer to a variable that receives the size of
        the loaded image.

    ImageHash - Supplies an optional pointer to a variable that receives the
        the cryptographic hash of the image.  The hash is computed prior to
        image relocation.

    ImageHashLength - Supplies an optional pointer to a varaible that receives
        the length of the image hash.

    ImageHashAlgorithm - Supplies an optional pointer to a variable that
        receives the ID of the algorithm used to hash the image.

    PreferredAttributes - Supplies optional memory attributes for the image's
        memory allocation.

    PreferredAlignment - Supplies an optional alignement preference for the
        image's memory allocation.

    Flags - Supplies a bitmask describing API settings.  Possible values
        include:

        IMAGE_FLAGS_MAP_ONE_TO_ONE : Maps the loaded image with a virtual
            address equal to it's virtual address.

        IMAGE_FLAGS_VALIDATE_MACHINE_TYPE : Ensures that the PE image to
            load has the same machine type as the executing Boot Application.

        IMAGE_FLAGS_MEMORY_PRE_ALLOCATED : Indicates that memory has already
            been pre-allocated for the image and that *ImageBase is the
            pointer to the pre-allocated memory address and *ImageSize is
            its size.

        IMAGE_FLAGS_ALLOCATE_IN_LARGE_PAGE : Requests that an allocation for
            the specified image be made in a large page.  A large page
            allocation will reserve an entire large page exclusively for
            allocations requesting large page allocation.

        IMAGE_FLAGS_VALIDATE_IMAGE_HASH : Indicates that the PE image needs
            to be validated by its cryptographic hash.

        IMAGE_FLAGS_VALID_HASH_REQUIRED : Indicates that the PE image load
            must fail if the image's cryptographic hash is found to be
            invalid.  This flag is ignored if the validate image hash flag is
            not set.

        IMAGE_FLAGS_FIRMWARE_UPDATE : Indicates that the firmware update image
            load must fail if the image's cryptographic hash is found to be
            invalid.  This flag is ignored if the validate image hash flag is
            not set.

        IMAGE_FLAGS_MS_SIGNATURE_REQUIRED_CATALOG : Indicates that a MS
            signature is required, but may be found in a catalog file.

        IMAGE_FLAGS_VALIDATE_FORCE_INTEGRITY : Indicates that the PE image
            needs to be validated if it contains the force integrity bit set
            in the dll characteristics field of the PE optional header.

        IMAGE_FLAGS_NO_RELOCATIONS : Specifies that this API should not perform
            relocations on the image.  It is not considered a failure if the
            image has a relocation section.

        IMAGE_FLAGS_VALIDATE_TRUSTED_ROOT : Indicates that the PE image load
            must fail if the image does not have the trusted boot root
            certificate.

        IMAGE_FLAGS_NO_SIGNATURE_EXPIRATION_CHECKS : Indicates that the signature
            expiration check will be skipped.
            Note that this flag is NOT supported for catalog signed image.

    LoadInformation - Supplies a pointer to a variable that receives
        informational flags describing various aspects of the load.  The
        following flags may be set during image load.

        LOAD_INFORMATION_IMAGE_HASH_INVALID : Indicates that a cryptographic
            image hash was computed and found to be invalid.

        LOAD_INFORMATION_EMBEDDED_SIGNED : Indicates that the image is verified
            using embedded signature.

        LOAD_INFORMATION_CATALOG_SIGNED : Indicates that the image is verified
            using a signature from a catalog.

        LOAD_INFORMATION_POLICY_VIOLATION : Indicates that the image violated
            system integrity policy.

        LOAD_INFORMATION_POLICY_VIOLATION_AUDIT : Indicates that the image violated
            system integrity policy but that and audit policy is in place 
            which allows the image.

        LOAD_INFORMATION_IMAGE_BOOT_SCENARIO_AWARE : Indicates that the image
             is aware of any boot scenario specific system integrity operations
             that must be performed to preserve the integrity of those scenarios.

        LOAD_INFORMATION_IMAGE_BOOT_SCENARIO_AGNOSTIC : Indicates that the image will not
             implement any boot application scenario system integrity operations but
             is not required to because it is known not to load any other boot
             application or exit the boot environment.

    BootTrustedBootInformation - Supplies a pointer to a structure that receives
        advanced cryptographic information for the image.

Return Value:

    NT status code.

--*/

{

    ULONGLONG AlignedImageSize;
    PVOID AllocatedExtraDataBase;
    PVOID AllocatedImageBase;
    PVOID BaseImageAddr;
    ULONG BaseImageSize;
    ULONG BootLibraryFlags;
    BOOT_TRUSTED_BOOT_INFORMATION BootTrustedBootInformationInternal;
    ULONG BytesHashed;
    PVOID CertAddress;
    PIMAGE_DATA_DIRECTORY CertDirectory;
    ULONG CheckSum;
    ULONG CheckSumFlags;
    BOOLEAN ComputeHash;
    ULONG Count;
    USHORT DllCharacteristics;
    ULONG EndOffset;
    NOTIFICATION_EVENT_CONTEXT EventContext;
    ULONG ExtraSize;
    PVOID FileBuffer;
    ULONG FileSize;
    ALG_ID HashAlgorithm;
    UCHAR HashBuffer[MAX_HASH_LEN];
    PCRYPTO_HASH_CONTEXT HashContext;
    PUCHAR HashPointer;
    ULONG HashSize;
    ULONG HeaderSize;
    PIMG_FILE_HANDLE InMemoryFileHandle;
    IMG_FILE_HANDLE InMemoryFileHandleBuffer;
    BOOLEAN ImageAlreadyLoaded;
    BOOLEAN ImageRelocated;
    ULONG Index;
    ULONG LastReadOffset;
    ULONG Length;
    UINTN LengthToZero;
    ALG_ID MinimumHashAlgorithm;
    PIMAGE_NT_HEADERS NtHeader;
    ULONG NumberOfSections;
    ULONG OriginalCheckSum;
    USHORT PartialSum;
    PHYSICAL_ADDRESS PhysicalAddress;
    IMG_SIGNING_SCENARIO_TYPE Scenario;
    PIMAGE_SECTION_HEADER SectionHeader;
    UCHAR Sector[1024];
    ULONG SecurityVersionNumber;
    MINCRYPT_SIGNATURE_DESCRIPTOR Signature;
    ULONGLONG SizeOfImage;
    ULONG SizeOfRawData;
    ULONG SizeToCheckSum;
    NTSTATUS Status;
    USHORT Subsystem;
    PVOID UninitializedImageMemory;
    ULONG ValidationFlags;
    BOOLEAN ValidHashFound;
    PVOID VirtualAddress;
    ULONG VirtualSize;
#if !defined(MISC_MIN)
    BOOLEAN Retry;
    IMG_RETRY_REASON RetriesCompleted;
    IMG_RETRY_REASON RetryReason;
#endif

    BlpInterfaceEnter(InterfaceImg);
    AllocatedImageBase = NULL;
    AllocatedExtraDataBase = NULL;
    BytesHashed = 0;
    CertAddress = NULL;
    CertDirectory = NULL;
    CheckSumFlags = CHECKSUM_WORD_ARRAY | CHECKSUM_ONES_COMPLEMENT;
    ComputeHash = FALSE;
    ExtraSize = 0;
    FileBuffer = NULL;
    FileSize = 0;
    HashAlgorithm = CALG_SHA1;
    HashContext = NULL;
    HashSize = 0;
    ImageAlreadyLoaded = FALSE;
    ImageRelocated = FALSE;
    InMemoryFileHandle = NULL;
    LastReadOffset = 0;
    Scenario = ImgSigningScenarioWindows;
    SecurityVersionNumber = 0;
    Signature.Signature = NULL;
    Signature.SignatureSize = 0;
    SizeOfImage = 0;
    ValidationFlags = 0;
    ValidHashFound = FALSE;
    RtlZeroMemory(&BootTrustedBootInformationInternal,
                  sizeof(BootTrustedBootInformationInternal));
    RtlZeroMemory(&EventContext,
                  sizeof(EventContext));

#if !defined(MISC_MIN)
    RtlZeroMemory(&RetriesCompleted, sizeof(RetriesCompleted));
#endif

    *LoadInformation = 0;

    //
    // Set the default hash algorithm.
    //

    Status = ImgpVerifyMinimalCodeIntegrityInitialization();
    if (!NT_SUCCESS(Status)) {
        goto LoadPEImageEnd;
    }

    MinimumHashAlgorithm = BlImgMinimumHashAlgorithm;

    //
    // Get the size of the file for this image.
    //
    // N.B. The check sum and hash code require the file length to read
    //      any non-section data that resides after the last image section.
    //

    Status = ImgpGetFileSize(FileHandle, &FileSize);
    if (!NT_SUCCESS(Status)) {
        Status = STATUS_FILE_INVALID;
        goto LoadPEImageEnd;
    }

    //
    // In order to increase the performance of the sector by sector expansion,
    // read the entire file into a buffer. This large read is more performant
    // than many individual reads.
    //

    Status = BlImgAllocateImageBuffer(&FileBuffer,
                                      FileSize,
                                      MEMORY_TYPE_MISC_DATA,
                                      0,
                                      0,
                                      0);

    if (!NT_SUCCESS(Status)) {
        goto LoadPEImageEnd;
    }

    Status = ImgpReadAtFileOffset(FileHandle,
                                  FileSize,
                                  0,
                                  FileBuffer,
                                  NULL,
                                  FILE_FLAGS_NONE);

    if (!NT_SUCCESS(Status)) {
        goto LoadPEImageEnd;
    }

    //
    // Initialize further reads for this file to go directly to the allocated
    // buffer, skipping the file system subsystem and file cache.
    //

    InMemoryFileHandle = &InMemoryFileHandleBuffer;
    ImgpInitializeFileHandleForSourceBuffer(InMemoryFileHandle,
                                            FileBuffer,
                                            FileSize,
                                            FileHandle->FilePath);

    //
    // Attempt to find the NT Headers for the file. If this call is not
    // successful, then the image is not a valid PE image.
    //

    Status = BlImgGetNtHeader(FileBuffer, FileSize, &NtHeader);
    if (!NT_SUCCESS(Status)) {
        goto LoadPEImageEnd;
    }

    //
    // If requested by caller, ensure the machine type of the image being
    // loaded.
    //

    if (CHECK_FLAG(Flags, IMAGE_FLAGS_VALIDATE_MACHINE_TYPE) &&
        (BOOT_APPLICATION_MACHINE_TYPE != NtHeader->FileHeader.Machine)) {

        //
        // N.B. STATUS_IMAGE_MACHINE_TYPE_MISMATCH is a warning and a failure
        //      status code is required if the image's machine type does not
        //      match executing machine type.
        //

        switch (NtHeader->FileHeader.Machine) {
        case IMAGE_FILE_MACHINE_I386:
        case IMAGE_FILE_MACHINE_ARMNT:
            Status = STATUS_INVALID_IMAGE_WIN_32;
            break;

        case IMAGE_FILE_MACHINE_AMD64:
        case IMAGE_FILE_MACHINE_IA64:
            Status = STATUS_INVALID_IMAGE_WIN_64;
            break;

        default:
            Status = STATUS_INVALID_IMAGE_FORMAT;
            break;
        }

        goto LoadPEImageEnd;
    }

    //
    // Check the subsystem value for a boot application.
    //

    if (CHECK_FLAG(Flags, (IMAGE_FLAGS_BOOT_APPLICATION |
                           IMAGE_FLAGS_BOOT_SUBSYSTEM)) != FALSE) {

        Subsystem = BlImgGetSubsystem(NtHeader);
        if (Subsystem != IMAGE_SUBSYSTEM_WINDOWS_BOOT_APPLICATION) {
            Status = STATUS_INVALID_IMAGE_FORMAT;
            goto LoadPEImageEnd;
        }
    }

    //
    // Honor load verification policy for the dll force integrity
    // characteristic.
    //

    DllCharacteristics = BlImgGetDllCharacteristics(NtHeader);
    if ((CHECK_FLAG(Flags, IMAGE_FLAGS_VALIDATE_FORCE_INTEGRITY) != FALSE) &&
        (CHECK_FLAG(DllCharacteristics,
                    IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY) != FALSE)) {

        SET_FLAGS(Flags, IMAGE_FLAGS_VALIDATE_IMAGE_HASH);
        SET_FLAGS(Flags, IMAGE_FLAGS_VALID_HASH_REQUIRED);
    }

    //
    // Determine if the image hash should be computed.
    //

    if ((CHECK_FLAG(Flags, IMAGE_FLAGS_VALIDATE_IMAGE_HASH) != FALSE) ||
        (ARGUMENT_PRESENT(ImageHash) != FALSE)) {

        ComputeHash = TRUE;

        Scenario = ImgpGetScenarioFromImageFlags(Flags);
        MinimumHashAlgorithm = ImgpGetHashAlgorithmForScenario(Scenario);

        if (CHECK_FLAG(Flags, IMAGE_FLAGS_BOOT_APPLICATION) != FALSE) {
            ValidationFlags = IMGP_MS_BOOT_APPLICATION_VALIDATION_FLAGS;
        }

        if (CHECK_FLAG(Flags, IMAGE_FLAGS_BOOT_MUI) != FALSE) {
            ValidationFlags = IMGP_MS_BOOT_MUI_VALIDATION_FLAGS;
        }

        if (CHECK_FLAG(Flags, IMAGE_FLAGS_MS_SIGNATURE_REQUIRED) != FALSE) {
            SET_FLAGS(ValidationFlags, IMGP_MS_SIGNATURE_REQUIRED);
            SET_FLAGS(ValidationFlags, IMGP_NO_CATALOG_CHECKS);
        }

        if (CHECK_FLAG(Flags, IMAGE_FLAGS_WINDOWS_COMPONENT) != FALSE) {
            SET_FLAGS(ValidationFlags, IMGP_WINDOWS_ROOT_REQUIRED);

            //
            // Legacy boot applications will be signed with an older cert
            // chain, so only enforce the latest root for other Windows
            // components or if the older roots are disabled
            // (i.e. only the 2010 root or later is enabled).
            //

            if (CHECK_FLAG(Flags, IMAGE_FLAGS_BOOT_APPLICATION) == FALSE ||
                CHECK_FLAG(BlImgAcceptedRootKeys,
                           MINCRYPT_POLICY_PRSROOT |
                           MINCRYPT_POLICY_PRSROOT_CURRENT) ==
                           MINCRYPT_POLICY_PRSROOT_CURRENT ||
                CHECK_FLAG(BlImgAcceptedRootKeys,
                           MINCRYPT_POLICY_TESTROOT |
                           MINCRYPT_POLICY_TESTROOT_CURRENT) ==
                           MINCRYPT_POLICY_TESTROOT_CURRENT) {

                SET_FLAGS(ValidationFlags, IMGP_LATEST_MS_ROOT_REQUIRED);
            }
        }

        if (CHECK_FLAG(Flags, IMAGE_FLAGS_NO_REVOCATION_LIST_CHECKS)
            != FALSE) {

            SET_FLAGS(ValidationFlags, IMGP_NO_REVOCATION_LIST_CHECKS);
        }

        if (CHECK_FLAG(Flags, IMAGE_FLAGS_EARLY_LAUNCH) != FALSE) {
            ValidationFlags = IMGP_EARLY_LAUNCH_VALIDATION_FLAGS;
        }

        if (CHECK_FLAG(Flags, IMAGE_FLAGS_HAL_EXTENSION) != FALSE) {
            ValidationFlags = IMGP_HAL_EXTENSION_VALIDATION_FLAGS;
        }

        //
        // For Core Extensions, set the appropriate image validation flags.
        //
        // N.B. Core Extensions that should be validated against the HAL
        //      signing certificate, the image type will be marked as HAL
        //      extension (and thus should get HAL validation flags set).
        //

        if (CHECK_FLAG(Flags, IMAGE_FLAGS_CORE_EXTENSION) != FALSE) {
            ValidationFlags = IMGP_CORE_EXTENSION_VALIDATION_FLAGS;
        }

#ifdef IMG_NO_CATALOG_SUPPORT

            ValidationFlags |= IMGP_NO_CATALOG_CHECKS;

#endif

        //
        // Firmware updates require a MS signature and use catalog files,
        // even on platforms that do not support boot file validation via
        // catalog files.
        //

        if (CHECK_FLAG(Flags,
                       IMAGE_FLAGS_MS_SIGNATURE_REQUIRED_CATALOG) != FALSE) {

            CLEAR_FLAGS(ValidationFlags, IMGP_NO_CATALOG_CHECKS);
            SET_FLAGS(ValidationFlags, IMGP_MS_SIGNATURE_REQUIRED);
        }

        //
        // BootMgr will set this flag to skip signature expiration check
        // on itself.
        //

        if (CHECK_FLAG(Flags, IMAGE_FLAGS_NO_SIGNATURE_EXPIRATION_CHECKS) != FALSE) {
            SET_FLAGS(ValidationFlags, IMGP_NO_SIGNATURE_EXPIRATION_CHECKS);
        }
    }

    //
    // N.B. The preferred base address is currently ignored.  At some point,
    //      it may be beneficial to load an image at its preferred address.
    //      This would have to be done using some sort of IMAGE_FLAGS since
    //      there are many different scenarios.  The kernel's memory manager
    //      will unmap usermode address space and therefore it would not
    //      make sense for the loader to allow an image to be loaded in this
    //      address range.  However, executables have relocation information
    //      stripped and require a specific address in usermode address
    //      space.  In this scenario, a caller would definitely want to allow
    //      an image to be loaded in this address range.
    //
    //      Another complication that will need to be resolved is the current
    //      assumption made by the kernel's memory manager.  The assumption
    //      is that the loader will have a one to one mapping from kernel mode
    //      address space to physical address space.  Since the Boot Environment
    //      Library does not have this restriction, loading an image at a
    //      preferred virtual address will most likely result in a non-one to
    //      one mapping.
    //

    //
    // Allocate a memory buffer for the in-memory copy of the image.  If the
    // caller pre-allocated a buffer, ensure that it is sufficiently large.
    //

    AllocatedImageBase = *ImageBase;
    SizeOfImage = BlImgGetSizeOfImage(NtHeader);
    AlignedImageSize = ALIGN_RANGE_UP(SizeOfImage, PAGE_SIZE);
    if (AlignedImageSize < SizeOfImage) {
        Status = STATUS_INTEGER_OVERFLOW;
        goto LoadPEImageEnd;
    }

    SizeOfImage = AlignedImageSize;
    if (SizeOfImage > ULONG_MAX) {
        Status = STATUS_INVALID_IMAGE_FORMAT;
        goto LoadPEImageEnd;
    }

    if (CHECK_FLAG(Flags, IMAGE_FLAGS_MEMORY_PRE_ALLOCATED)) {
        if (*ImageSize < SizeOfImage) {
            *ImageSize = (ULONG)SizeOfImage;
            Status = STATUS_BUFFER_TOO_SMALL;
        }

    } else {
        Status = BlImgAllocateImageBuffer(&AllocatedImageBase,
                                          SizeOfImage,
                                          MemoryType,
                                          PreferredAttributes,
                                          PreferredAlignment,
                                          Flags);
    }

    if (!NT_SUCCESS(Status)) {
        goto LoadPEImageEnd;
    }

    BaseImageAddr = AllocatedImageBase;
    BaseImageSize = (ULONG)SizeOfImage;

    //
    // Relocate the NT headers to the allocated image buffer.
    //

    HeaderSize = BlImgGetSizeOfHeaders(NtHeader);
    if (SizeOfImage < HeaderSize) {
        Status = STATUS_INVALID_IMAGE_FORMAT;
        goto LoadPEImageEnd;
    }

    Status = ImgpReadAtFileOffset(InMemoryFileHandle,
                                  HeaderSize,
                                  0,
                                  AllocatedImageBase,
                                  NULL,
                                  FILE_FLAGS_NONE);

    if (!NT_SUCCESS(Status)) {
        goto LoadPEImageEnd;
    }

    Status = BlImgGetNtHeader(AllocatedImageBase, HeaderSize, &NtHeader);
    if (!NT_SUCCESS(Status)) {
        goto LoadPEImageEnd;
    }

    ASSERT(NtHeader != NULL);

    NumberOfSections = NtHeader->FileHeader.NumberOfSections;

    //
    // Compute a partial check sum on the image headers with the
    // CheckSum field set to 0. File check sums must treat the
    // CheckSum field as 0.
    //

    OriginalCheckSum = NtHeader->OptionalHeader.CheckSum;
    NtHeader->OptionalHeader.CheckSum = 0;
    PartialSum = (USHORT)BlUtlCheckSum(0,
                                       AllocatedImageBase,
                                       NtHeader->OptionalHeader.SizeOfHeaders,
                                       CheckSumFlags);

    NtHeader->OptionalHeader.CheckSum = OriginalCheckSum;

    //
    // Hash the header, excluding the checksum and security directory fields.
    // SHA-1 is the default for catalog signed files with no embedded
    // certificate, otherwise read the cert into memory.
    //

    if (ComputeHash != FALSE) {
        Status = BlImgGetValidatedCertificateLocation(NtHeader,
                                                      AllocatedImageBase,
                                                      FileSize,
                                                      &CertDirectory,
                                                      NULL);
        if (!NT_SUCCESS(Status)) {
            goto LoadPEImageEnd;
        }

        if (CertDirectory->VirtualAddress == 0 ||
            CertDirectory->Size == 0) {
            HashAlgorithm = MinimumHashAlgorithm;

        } else {
            Status = BlImgAllocateImageBuffer(&CertAddress,
                                              CertDirectory->Size,
                                              MEMORY_TYPE_MISC_DATA,
                                              0,
                                              0,
                                              0);

            if (!NT_SUCCESS(Status)) {
                goto LoadPEImageEnd;
            }

            Status = ImgpReadAtFileOffset(InMemoryFileHandle,
                                          CertDirectory->Size,
                                          CertDirectory->VirtualAddress,
                                          CertAddress,
                                          NULL,
                                          FILE_FLAGS_NONE);

            if (!NT_SUCCESS(Status)) {
                goto LoadPEImageEnd;
            }

            //
            // Parse the certificate for files with embedded signatures.
            //

            Status = MinCryptGetHashAlgorithmFromWinCertificate(
                            CertAddress,
                            CertDirectory->Size,
                            MinimumHashAlgorithm,
                            MinimumHashAlgorithm,
                            &HashAlgorithm,
                            &Signature,
                            NULL);

            if (!NT_SUCCESS(Status)) {

                //
                // The embedded signature does not use at least the minimum
                // algorithm, so fall back to using the catalogs.
                //

                HashAlgorithm = MinimumHashAlgorithm;
            }
        }
    }

#if !defined(MISC_MIN)
LoadPEImageDoHashCalculation:
#endif

    if (ComputeHash != FALSE) {
        Status = BlCryptoHashInitialize(HashAlgorithm,
                                        &HashContext,
                                        &HashSize);
        if (!NT_SUCCESS(Status)) {
            goto LoadPEImageEnd;
        }

        HashPointer = (PUCHAR)BlImgGetChecksumAddress(NtHeader);
        Status = BlCryptoHashUpdate(
                        HashContext,
                        AllocatedImageBase,
                        (ULONG)PtrOffset(AllocatedImageBase, HashPointer));

        if (!NT_SUCCESS(Status)) {
            goto LoadPEImageEnd;
        }

        HashPointer = Add2Ptr(HashPointer, sizeof(ULONG));
        Status = BlCryptoHashUpdate(
                        HashContext,
                        HashPointer,
                        (ULONG)PtrOffset(HashPointer, CertDirectory));

        if (!NT_SUCCESS(Status)) {
            goto LoadPEImageEnd;
        }

        HashPointer = Add2Ptr(CertDirectory, sizeof(IMAGE_DATA_DIRECTORY));
        Status = BlCryptoHashUpdate(
                        HashContext,
                        HashPointer,
                        (ULONG)PtrOffset(HashPointer,
                                         Add2Ptr(AllocatedImageBase,
                                                 BlImgGetSizeOfHeaders(
                                                    NtHeader))));
        if (!NT_SUCCESS(Status)) {
            goto LoadPEImageEnd;
        }

        BytesHashed = BlImgGetSizeOfHeaders(NtHeader);
    }

    //
    // Load each image section into memory.
    //

    SectionHeader = IMAGE_FIRST_SECTION(NtHeader);
    UninitializedImageMemory = Add2Ptr(AllocatedImageBase, HeaderSize);
    for (Index = 0; Index < NumberOfSections; Index += 1, SectionHeader += 1) {

        //
        // Compute the virtual address of the section.
        //

        VirtualAddress = Add2Ptr(AllocatedImageBase,
                                 SectionHeader->VirtualAddress);

        if ((SizeOfImage < SectionHeader->VirtualAddress) ||
            (VirtualAddress < AllocatedImageBase)) {

            Status = STATUS_INVALID_IMAGE_FORMAT;
            goto LoadPEImageEnd;
        }

        if (ImageAlreadyLoaded == FALSE) {
            //
            // Zero out any unused memory space in the previous section.
            //

            if (UninitializedImageMemory < VirtualAddress) {
                LengthToZero = PtrOffset(UninitializedImageMemory, VirtualAddress);
                RtlZeroMemory(UninitializedImageMemory, LengthToZero);
            }
        }

        //
        // Determine the size of the section.
        //
        // N.B. The value of SectionHeader->SizeOfRawData can be non-zero even
        //      the pointer to the raw data is zero.  Additionally, this value
        //      needs to be capped at the virtual size.
        //

        VirtualSize = SectionHeader->Misc.VirtualSize;
        SizeOfRawData = SectionHeader->SizeOfRawData;
        SizeToCheckSum = ALIGN_RANGE_UP(SizeOfRawData, 2);
        if (SizeToCheckSum < SizeOfRawData) {
            Status = STATUS_INTEGER_OVERFLOW;
            goto LoadPEImageEnd;
        }

        if (VirtualSize == 0) {
            VirtualSize = SizeOfRawData;
        }

        if (SectionHeader->PointerToRawData == 0) {
            SizeOfRawData = 0;

        } else if (VirtualSize < SizeOfRawData) {
            SizeOfRawData = VirtualSize;
        }

        //
        // Validate that the section does not overrun the total image size.
        //

        Status = RtlULongAdd(SectionHeader->VirtualAddress,
                             VirtualSize,
                             &EndOffset);

        if (!NT_SUCCESS(Status) || (SizeOfImage < EndOffset)) {
            Status = STATUS_INVALID_IMAGE_FORMAT;
            goto LoadPEImageEnd;
        }

        Status = RtlULongAdd(SectionHeader->VirtualAddress,
                             SizeToCheckSum,
                             &EndOffset);

        if (!NT_SUCCESS(Status) || (SizeOfImage < EndOffset)) {
            Status = STATUS_INVALID_IMAGE_FORMAT;
            goto LoadPEImageEnd;
        }

        //
        // Load the section's raw data into memory.
        //
        // N.B. Always load check sum length bytes, irregardless of whether the
        //      raw data size will be trimmed, to ensure the check sum and
        //      image hash are properly computed.
        //

        if (SizeOfRawData != 0) {
            if (ImageAlreadyLoaded == FALSE) {
                Status = ImgpReadAtFileOffset(InMemoryFileHandle,
                                              SizeToCheckSum,
                                              SectionHeader->PointerToRawData,
                                              VirtualAddress,
                                              NULL,
                                              FILE_FLAGS_NONE);

                if (!NT_SUCCESS(Status)) {
                    goto LoadPEImageEnd;
                }

                LastReadOffset = SectionHeader->PointerToRawData + SizeToCheckSum;

                //
                // Compute the check sum on the section.
                //

                PartialSum = (USHORT)BlUtlCheckSum(PartialSum,
                                                   VirtualAddress,
                                                   SizeToCheckSum,
                                                   CheckSumFlags);
            }

            //
            // Compute the hash on the section.
            //

            if (ComputeHash != FALSE) {
                Status = BlCryptoHashUpdate(HashContext,
                                            VirtualAddress,
                                            SectionHeader->SizeOfRawData);

                if (!NT_SUCCESS(Status)) {
                    goto LoadPEImageEnd;
                }

                BytesHashed += SectionHeader->SizeOfRawData;
            }
        }

        if (ImageAlreadyLoaded == FALSE) {
            //
            // Zero any memory in the section after the raw data.
            //

            if (SizeOfRawData < VirtualSize) {
                LengthToZero = VirtualSize - SizeOfRawData;
                RtlZeroMemory(Add2Ptr(VirtualAddress, SizeOfRawData), LengthToZero);
            }

            UninitializedImageMemory = Add2Ptr(VirtualAddress, VirtualSize);
        }
    }

    if (ImageAlreadyLoaded == FALSE) {
        //
        // Zero out any unused memory space in the last section of the image.
        //

        VirtualAddress = Add2Ptr(AllocatedImageBase, SizeOfImage);
        if (UninitializedImageMemory < VirtualAddress) {
            LengthToZero = PtrOffset(UninitializedImageMemory, VirtualAddress);
            RtlZeroMemory(UninitializedImageMemory, LengthToZero);
        }
    }

    //
    // Only do the check sum if the image loaded properly.
    //

    if (NT_SUCCESS(Status) && ImageAlreadyLoaded == FALSE) {
        Count = FileSize;
        Count -= LastReadOffset;

        //
        // Read in the rest of the image and check sum it.
        //

        while (Count != 0) {
            if (Count < (sizeof(Sector))) {
                Length = Count;

            } else {
                Length = sizeof(Sector);
            }

            Status = ImgpReadAtFileOffset(InMemoryFileHandle,
                                          Length,
                                          LastReadOffset,
                                          Sector,
                                          &Length,
                                          FILE_FLAGS_NONE);

            if (!NT_SUCCESS(Status) || (Length == 0)) {
                Status = STATUS_FILE_INVALID;
                goto LoadPEImageEnd;
            }

            Count -= Length;
            LastReadOffset += Length;
            PartialSum = (USHORT)BlUtlCheckSum(PartialSum,
                                               Sector,
                                               Length,
                                               CheckSumFlags);
        }

        //
        // Add the file size. The resultant value should match the
        // original image check sum. If it does not, handle corner
        // case for down-level OS image files:
        //
        // There exists a corner case if the image file was built
        // on a pre-Vista OS. The old checksums in sdktools, rtl,
        // and boot included the header checksum in the
        // computation and then attempted to "subtract" it out of
        // the partial checksum before adding the file size. This
        // "subtraction" did not properly roll back the partial
        // checksum if adding the header checksum caused an
        // overflow (i.e. it rolled back to 0x0000 and not to
        // 0xffff).
        //
        // For reference, the old "subtraction" code that ran
        // after the PartialSum was obtained and before the FileSize
        // was added:
        //
        // AdjustSum = (PUSHORT)(&NtHeader->OptionalHeader.CheckSum);
        // PartialSum -= (PartialSum < AdjustSum[0]);
        // PartialSum = PartialSum - AdjustSum[0];
        // PartialSum -= (PartialSum < AdjustSum[1]);
        // PartialSum = PartialSum - AdjustSum[1];
        //

        CheckSum = (ULONG)PartialSum + FileSize;
        if ((CheckSum != OriginalCheckSum) && (PartialSum == 0xffff)) {
            CheckSum = FileSize;
        }

        //
        // N.B. When called for a firmware updated, the image load is performed
        //      to perform an integrity check on the image. In that case, an
        //      invalid checksum is ignored.
        //

        if ((CheckSum != OriginalCheckSum) &&
            (CHECK_FLAG(Flags, IMAGE_FLAGS_FIRMWARE_UPDATE) == FALSE)) {

            Status = STATUS_IMAGE_CHECKSUM_MISMATCH;
            goto LoadPEImageEnd;
        }
    }

    //
    // Load any additional non-section data to factor into the hash.
    //

    if (ComputeHash != FALSE){
        ExtraSize = FileSize - BytesHashed;
        if (CertDirectory->Size > ExtraSize) {
            Status = STATUS_FILE_INVALID;
            goto LoadPEImageEnd;
        }

        //
        // Don't include the certificate.  It was read earlier to
        // get the hash algorithm.
        //

        ExtraSize -= CertDirectory->Size;
        if (ExtraSize != 0) {
            if (ImageAlreadyLoaded == FALSE) {
                Status = BlImgAllocateImageBuffer(&AllocatedExtraDataBase,
                                                  ExtraSize,
                                                  MEMORY_TYPE_MISC_DATA,
                                                  0,
                                                  0,
                                                  0);

                if (!NT_SUCCESS(Status)) {
                    goto LoadPEImageEnd;
                }

                Status = ImgpReadAtFileOffset(InMemoryFileHandle,
                                              ExtraSize,
                                              BytesHashed,
                                              AllocatedExtraDataBase,
                                              NULL,
                                              FILE_FLAGS_NONE);

                if (!NT_SUCCESS(Status)) {
                    goto LoadPEImageEnd;
                }
            }

            Status = BlCryptoHashUpdate(HashContext,
                                        AllocatedExtraDataBase,
                                        ExtraSize);

            if (!NT_SUCCESS(Status)) {
                goto LoadPEImageEnd;
            }
        }

        ImageAlreadyLoaded = TRUE;

        Status = BlCryptoHashFinalize(HashContext, HashBuffer);
        if (!NT_SUCCESS(Status)) {
            goto LoadPEImageEnd;
        }

        if (ARGUMENT_PRESENT(ImageHash) != FALSE) {
            RtlCopyMemory(ImageHash, HashBuffer, sizeof(HashBuffer));
        }

        if (ARGUMENT_PRESENT(ImageHashLength) != FALSE) {
            *ImageHashLength = HashSize;
        }

        if (ARGUMENT_PRESENT(ImageHashAlgorithm) != FALSE) {
            *ImageHashAlgorithm = HashAlgorithm;
        }

        //
        // Validate the integrity of the image.  Boot applications are
        // required to be self signed and are not subject to checks against
        // any certificate revocation list.
        //

        if (CHECK_FLAG(Flags, IMAGE_FLAGS_VALIDATE_IMAGE_HASH) != FALSE) {
            Status = ImgpValidateImageHash(
                         FileHandle->FilePath,
                         BaseImageAddr,
                         BaseImageSize,
                         &Signature,
                         HashAlgorithm,
                         HashBuffer,
                         ValidationFlags,
                         Scenario,
#if !defined(MISC_MIN)
                         &RetryReason,
#else
                         NULL,
#endif
                         (BootTrustedBootInformation != NULL) ?
                             BootTrustedBootInformation :
                             &BootTrustedBootInformationInternal,
                         LoadInformation);

#if !defined(MISC_MIN)
            if (Status == STATUS_RETRY)
            {
                Status = STATUS_INVALID_IMAGE_HASH;

                Retry = ImgpProcessHashRetryReason(&RetryReason,
                                                   &RetriesCompleted,
                                                   MinimumHashAlgorithm,
                                                   &HashAlgorithm,
                                                   &Signature);

                if (Retry != FALSE)
                {
                    goto LoadPEImageDoHashCalculation;
                }
            }
#endif

            //
            // If load information is being collected, indicate that the hash
            // for this image is not valid.
            //

            if (!NT_SUCCESS(Status)) {

                SET_FLAGS(*LoadInformation, LOAD_INFORMATION_IMAGE_HASH_INVALID);

                //
                // Fail the image load attempt if it is not suppressed by the
                // validation failure filter and the load flags indicate that
                // this image is required to carry a valid hash.  Otherwise, set
                // a success status and fall through to complete the load.
                //

                Status = ImgpFilterValidationFailure(FileHandle->FilePath,
                                                     Status,
                                                     Flags);

                if (!NT_SUCCESS(Status)) {
                    if (CHECK_FLAG(Flags, IMAGE_FLAGS_VALID_HASH_REQUIRED)) {
                        goto LoadPEImageEnd;
                    }
                }

                Status = STATUS_SUCCESS;

            } else {

                //
                // If the hash was validated, record it, so we can accurately
                // report to the System Integrity library
                //

                ValidHashFound = TRUE;
            }
        }
    }

    //
    // Verify the file name matches in the version resource when required.
    //

    if (CHECK_FLAG(Flags, IMAGE_FLAGS_VERIFY_FILENAME) != FALSE) {

        if (BlResourceVerifyOriginalFilenameFromImage(
                    BaseImageAddr,
                    BaseImageSize,
                    FileHandle->FilePath) == FALSE) {

            Status = STATUS_SECUREBOOT_FILE_REPLACED;
            goto LoadPEImageEnd;
        }
    }

    //
    // Apply relocations.
    //

    if ((CHECK_FLAG(Flags, IMAGE_FLAGS_NO_RELOCATIONS) == FALSE) &&
        (AllocatedImageBase != BlImgGetImageBase(NtHeader))) {

        Status = LdrRelocateImage(AllocatedImageBase,
                                  "Boot Environment Library",
                                  STATUS_SUCCESS,
                                  STATUS_UNSUCCESSFUL,
                                  STATUS_INVALID_IMAGE_FORMAT);

        if (!NT_SUCCESS(Status)) {
            goto LoadPEImageEnd;
        }
        ImageRelocated = TRUE;
    }

    BootLibraryFlags = BlGetCurrentLibraryParameters().Flags;

    if (CHECK_FLAG(BootLibraryFlags, LF_SVN_CHAIN_CHECK_CHILDREN)) {

        Status = BlSIPolicyGetSvnFromImage(BaseImageAddr,
                                           BaseImageSize,
                                           &SecurityVersionNumber);

        if (!NT_SUCCESS(Status)) {
            SecurityVersionNumber = 0;
            Status = STATUS_SUCCESS;
        }
    }

    if (SecurityVersionNumber != 0) {
        (VOID)BlSIPolicyPerformSvnCheck(SecurityVersionNumber);
    }

    //
    // Notify the load of a non-validated image to all registered components.
    //

    if (ValidHashFound == FALSE) {
        BlEnNotifyEvent(EVENT_NVPE_IMAGE_LOADED, NULL);
    }

    EventContext.u.Hash.FilePath = FileHandle->FilePath;
    BlMmTranslateVirtualAddress(AllocatedImageBase,
                                &EventContext.u.Hash.ImageBase);

    EventContext.u.Hash.ImageSize = SizeOfImage;
    EventContext.u.Hash.AlgId = HashAlgorithm;
    EventContext.u.Hash.ImageHashSize = HashSize;
    EventContext.u.Hash.ImageHash = HashBuffer;
    EventContext.u.Hash.AuthorityValid = ValidHashFound;
    EventContext.u.Hash.TrustedBootInfo = (BootTrustedBootInformation != NULL) ?
                                              BootTrustedBootInformation :
                                              &BootTrustedBootInformationInternal;

    EventContext.u.Hash.SecurityVersionNumber = SecurityVersionNumber;

    BlEnNotifyEvent(EVENT_APPLICATION_HASHED, &EventContext);

    //
    // Get the boot image classification flags so that system
    // integrity operations can be performed later, as needed.
    // N.B. Don't propagate Status, failure will leave flags
    //      in the most conservative state.
    //

    (VOID)ImgpClassifyPEBootAppImage(BaseImageAddr,
                                     BaseImageSize,
                                     LoadInformation);

    //
    // Set return values.
    //

    *ImageBase = AllocatedImageBase;
    if (ARGUMENT_PRESENT(ImageSize) != FALSE) {
        *ImageSize = (ULONG)SizeOfImage;
    }

LoadPEImageEnd:
    if((ValidHashFound != FALSE) && (BootTrustedBootInformation == NULL)) {
        ImgpDestroyTrustedBootInformation(&BootTrustedBootInformationInternal);
    }

    if (HashContext != NULL) {
        BlCryptoHashDestroy(HashContext);
    }

    if (AllocatedExtraDataBase != NULL) {
        BlMmFreePages(AllocatedExtraDataBase);
    }

    if ((CertAddress != NULL) && (CertDirectory != NULL)) {
        BlImgUnallocateImageBuffer(CertAddress, CertDirectory->Size, 0);
    }

    if ((FileBuffer != NULL) && (FileSize != 0)) {
        BlImgUnallocateImageBuffer(FileBuffer, FileSize, 0);
    }

    if (InMemoryFileHandle != NULL) {
        ImgpCloseFile(InMemoryFileHandle);
    }

    if (!NT_SUCCESS(Status) && (AllocatedImageBase != NULL)) {
        if (!CHECK_FLAG(Flags, IMAGE_FLAGS_MEMORY_PRE_ALLOCATED)) {
            if (CHECK_FLAG(Flags, IMAGE_FLAGS_MAP_ONE_TO_ONE)) {
                BlMmUnmapVirtualAddress(AllocatedImageBase, SizeOfImage);
                PhysicalAddress.QuadPart = (UINTN)AllocatedImageBase;
                BlMmFreePhysicalPages(PhysicalAddress);

            } else {
                BlMmFreePages(AllocatedImageBase);
            }
        }
    }

    BlpInterfaceExit(InterfaceImg);
    return Status;
}

BOOLEAN
ImgpQuerySystemTime(
    _Out_ PLARGE_INTEGER Time
    )

/*++

Routine Description:

    This function returns the time in the same format as KeQuerySystemTime.

Arguments:

    Time - Supplies a pointer that receives the current time.

Return Value:

    FALSE if the time could not be retrieved successfully.

--*/

{
    NTSTATUS Status;
    TIME_FIELDS TimeFields;
    BOOLEAN UtcTime;

    Status = BlGetTime(&TimeFields, &UtcTime);
    if (!NT_SUCCESS(Status)) {
        return FALSE;
    }

    if (RtlTimeFieldsToTime(&TimeFields, Time) == FALSE) {
        return FALSE;
    }

    //
    // In order to compare the RTC to other timestamps,
    // the RTC time must be in UTC format. If it is in
    // local time, then adjust for the worst case UTC
    // time which is 26 hours ahead.
    //

    if (UtcTime == FALSE) {
        Time->QuadPart += 936000000000ULL;
    }

    return TRUE;
}

NTSTATUS
ImgpVerifyMinimalCodeIntegrityInitialization (
    VOID
    )

/*++

Routine Description:

    This function carries out the minimal set of actions required before
    image code integrity operations can take place.  If these actions have
    already been attempted, then this function returns the result of the
    previous initialization attempt.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS is returned if code integrity initialization completes
        successfully.

    STATUS_INVALID_IMAGE_HASH is returned if code integrity initialization
        fails, implying that future code integrity operations should not be
        attempted.

--*/

{

    PWSTR BcdOptions;
    BOOLEAN TestRootTrusted;
    NTSTATUS Status;

    //
    // This function is not called as part of library initialization.
    // Instead, it is called on demand only in cases where code integrity
    // operations are actually attempted.  Return immediately if code
    // integrity initialization has already taken place.  If an attempt has
    // already been made, the initialization status indicates whether it
    // succeeded or failed.
    //

    if (BlImgCodeIntegrityInitializationAttempts != 0) {
        if (BlImgCodeIntegrityInitialized != FALSE) {
            return STATUS_SUCCESS;

        } else {
            return STATUS_INVALID_IMAGE_HASH;
        }
    }

    BlImgCodeIntegrityInitializationAttempts += 1;

#if !defined(MISC_MIN)
    TraceLoggingRegister(g_CiTraceLoggingProvider);
    TraceLoggingRegister(g_CiAuditTraceLoggingProvider);

#endif
    //
    // This is the initial code integrity initialization attempt.  Verify that
    // crypto operations used in the mincrypt library pass the known answer
    // test.
    //

    MincrypL_SetCallbackForGetTime(ImgpQuerySystemTime);

    if (MincrypL_SelfTest() == FALSE) {
        return STATUS_INVALID_IMAGE_HASH;
    }

    //
    // Disable flight root trust for test build if test hook is set.
    //
    // For pre-OS, test hook is set through BCD LoadOptions setting.
    //
    // i.e. BcdEdit /set LoadOptions "TestDisableFlight"
    //

    Status = BlGetApplicationOptionString(BCDE_LIBRARY_TYPE_LOAD_OPTIONS_STRING,
                                          &BcdOptions);

    if (NT_SUCCESS(Status)) {
        NT_ASSERT(BcdOptions != NULL);

        _wcsupr(BcdOptions);

        if (wcsstr(BcdOptions, L"TESTDISABLEFLIGHT") != NULL) {
            SET_FLAGS(BlSigningPolicy, IMAGE_SIGNING_POLICY_DO_NOT_TRUST_FLIGHT_ROOT);
        }

        BlMmFreeHeap(BcdOptions);
    }

#if defined(BUILD_PRODUCTION_SIGNED) || defined(BUILD_FLIGHT_SIGNED)
    BlImgRevokePrereleaseSignersOnce(FALSE);
#endif

    //
    // Initialize the set of accepted root keys based upon whether test signed
    // binaries should be loaded or judged as invalid.  This determination is
    // made based upon the boot configuration elements applied to this
    // application.
    //

    BlImgQueryCodeIntegrityBootOptions(BlGetApplicationEntry(),
                                       NULL,
                                       &BlImgTestSigningEnabled,
                                       &BlImgFlightSigningEnabled);

    if (BlImgTestSigningEnabled == FALSE) {

        //
        // If restricted signing is on, only allow Microsoft root.
        //

        BlImgAcceptedRootKeys = BlImgGetDefaultTrustedRoots();

#if !defined(BUILD_PRS_SIGNED)
        //
        // For test build disable flight root trust if test hook is set.
        //

        if (CHECK_FLAG(BlSigningPolicy,
                       IMAGE_SIGNING_POLICY_DO_NOT_TRUST_FLIGHT_ROOT)) {
            CLEAR_FLAGS(BlImgAcceptedRootKeys, MINCRYPT_POLICY_FLIGHTROOT);
        }
#endif

        BlImgAcceptedRootKeys &= BlImgAcceptedRootKeysMask;

        BlImgAcceptedTrustedBootRootKeys =
            BlImgAcceptedRootKeys & (MINCRYPT_POLICY_PRSROOT_CURRENT |
                                     MINCRYPT_POLICY_TESTROOT_CURRENT |
                                     MINCRYPT_POLICY_FLIGHTROOT);
    } else {

        //
        // Notify that test signing has been enabled to all registered
        // components.
        //

        BlEnNotifyEvent(EVENT_TEST_SIGNING_ENABLED, NULL);
        BlImgAcceptedRootKeys = MINCRYPT_POLICY_PRSTRUSTEDROOT |
                                MINCRYPT_POLICY_TESTROOT |
                                MINCRYPT_POLICY_PICTESTROOT |
                                MINCRYPT_POLICY_DMDTESTROOT |
                                MINCRYPT_POLICY_SELFSIGNED |
                                MINCRYPT_POLICY_INCOMPLETE_CHAIN;

        //
        // Trust flight root if not disabled by test hook.
        //

        if (!CHECK_FLAG(BlSigningPolicy,
                        IMAGE_SIGNING_POLICY_DO_NOT_TRUST_FLIGHT_ROOT))
        {
            BlImgAcceptedRootKeys |= MINCRYPT_POLICY_FLIGHTROOT;
        }

        BlImgAcceptedTrustedBootRootKeys = BlImgAcceptedRootKeys |
                                           MINCRYPT_POLICY_TRUSTEDBOOTROOT;
    }

    //
    // If flight signing is on, allow flight signed binaries.
    //

    if (BlImgFlightSigningEnabled)
    {
        BlEnNotifyEvent(EVENT_FLIGHT_SIGNING_ENABLED, NULL);
        BlImgAcceptedRootKeys |= MINCRYPT_POLICY_FLIGHTROOT;
        BlImgAcceptedTrustedBootRootKeys |= MINCRYPT_POLICY_FLIGHTROOT;
    }

    //
    // Process ConfigCI policy disable flight root trust option.
    //
    // Note: The ConfigCI policy overrides all other flight trust
    //       settings, so should be the last option to evaluate.
    //

    ImgpProcessFlightRootTrustPolicy();

    //
    // Tell MinCrypt about flight signing option.
    //

    if (CHECK_FLAG(BlImgAcceptedRootKeys, MINCRYPT_POLICY_FLIGHTROOT))
    {
        MincrypL_SetFlightSigningOption(TRUE);
    }

    //
    // Build up the signing policy.
    //

    TestRootTrusted = CHECK_FLAG(BlImgAcceptedRootKeys, MINCRYPT_POLICY_TESTROOT) ? TRUE : FALSE;
    Status = BlImgApplySigningPolicy(TestRootTrusted, BlImgTestSigningEnabled);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Tell MinCrypt if we should ignore Flight Signing EKU requirement.
    //

    if (BlSecureBootArePolicyOptionsSet(
            SECUREBOOT_POLICY_IGNORE_FLIGHT_SIGNING_EKU) != FALSE)
    {
        MincrypL_IgnoreFlightSigningEKU(TRUE);
    }

    //
    // Tell MinCrypt if we should ignore the Lifetime Signing EKU
    //

    if (BlSecureBootIsRegistryDwordAsBooleanSetInActivePolicy(
            &BlpSecureBootPlatformKey,
            &BlpSecureBootDisableLifetimeSigningValue))
    {
        MincrypL_IgnoreLifetimeSigningEKU(TRUE);
    }

    //
    // Minimal code integrity initialization has completed successfully.
    //

    BlImgCodeIntegrityInitialized = TRUE;

    return STATUS_SUCCESS;
}

VOID
ImgpDestroy(
    VOID
    )

/*++

Routine Description:

    Removes allocations in the certificate catalogs from the
    mincrypt libraty.

Arguments:

    None.

Return Value:

    None.

--*/

{
    PLIST_ENTRY Entry;
    PBOOT_CATALOG_ENTRY CatalogEntry;

    if (NULL != BlImgLoadedCatalogs.Flink) {
        while (!IsListEmpty(&BlImgLoadedCatalogs)) {
            Entry = BlImgLoadedCatalogs.Flink;
            CatalogEntry = (PBOOT_CATALOG_ENTRY)(Entry + 1);
            if (CatalogEntry->PolicyInfo != NULL){
                BlMmFreeHeap(CatalogEntry->PolicyInfo);
                CatalogEntry->PolicyInfo = NULL;
            }
            MinCrypL_RemoveCatalog(CatalogEntry);
            RemoveEntryList(Entry);
            BlMmFreeHeap(Entry);
        }
    }

    if (BlImgCatalogDirectoryPath != NULL) {
        BlMmFreeHeap(BlImgCatalogDirectoryPath);
        BlImgCatalogDirectoryPath = NULL;
    }

    BlImgSystemCatalogLoaded = FALSE;
    if (BlImgSystemCatalogName != NULL) {
        BlMmFreeHeap(BlImgSystemCatalogName);
        BlImgSystemCatalogName = NULL;
    }

    if (INVALID_FILE_ID != BlImgCatalogDirectoryId) {
        BlFileClose(BlImgCatalogDirectoryId);
        BlImgCatalogDirectoryId = INVALID_FILE_ID;
    }

    BlImgCatalogPrecedenceListEntries = 0;

    BlImgCodeIntegrityInitialized = FALSE;
    BlImgCodeIntegrityInitializationAttempts = 0;
    BlImgCatalogPolicy = CATALOG_FLAGS_NONE;

    MinCrypL_Cleanup();

#if !defined(MISC_MIN)
    TraceLoggingUnregister(g_CiTraceLoggingProvider);
    TraceLoggingUnregister(g_CiAuditTraceLoggingProvider);
#endif

    return;
}

NTSTATUS
ImgpLoadCatalog (
    _In_ PWSTR CatalogName,
    _In_ IMG_SIGNING_SCENARIO_TYPE Scenario,
    _Outptr_ struct _BOOT_CATALOG_ENTRY **CatalogEntry,
    _Out_opt_ PULONG LoadInformation
    )

/*++

Routine Description:

    Loads a system catalog into memory and registers the catalog with the
    mincrypt library.

Arguments:

    CatalogName - Supplies a name of the catalog file to load.

    Scenario - Supplies the type of signing scenario currently being used.

    CatalogEntry - Supplies a pointer to a variable that receives address of
        allocated per catalog structure.

   LoadInformation - Supplies an optional pointer to a variable that receives
        informational flags describing various aspects of the load.  The
        following flags may be set during image load.

        LOAD_INFORMATION_WHQL_EXEMPTED_FOR_BOOT_DRIVER : Indicates that the 
            image failed the WHQL requirement, but was allowed to load for
            interest of not bricking the machine.

        LOAD_INFORMATION_WHQL_EXEMPTED_BY_AUDIT_MODE : Indicates that the 
            image failed the WHQL requirement, but was allowed to load because
            SI Audit Mode is enabled.
Return Value:

    STATUS_SUCCESS if function succeeded.

    STATUS_NO_MEMORY if memory allocation failed.

    STATUS_INVALID_IMAGE_HASH if catalog is invalid.

    Another error status code if returned from a sub-routine.

--*/

{

    BOOLEAN CatalogAddedToMincrypt;
    PWCHAR CatalogFilePath;
    PVOID CatalogImageBase;
    ULONG CatalogRootLength;
    ULONG CatalogSize;
    FILE_ID FileId;
    FILE_INFORMATION Info;
    ULONG MatchingSigner;
    MINCRYPT_POLICY_INFO PolicyInfo;
    ULONG RequiredBufferSize;
    NTSTATUS Status;
    PLIST_ENTRY CatalogEntryHeader;

    CatalogAddedToMincrypt = FALSE;
    *CatalogEntry = NULL;
    CatalogImageBase = NULL;
    CatalogEntryHeader = NULL;
    FileId = INVALID_FILE_ID;
    Status = STATUS_SUCCESS;

    //
    // TODO: Memory allocated for loaded catalogs should be freed once the
    //       cleanup registration infrastructure is in place.
    //

    //
    // Verify that minimal code integrity initialization has completed
    // successfully.
    //

    Status = ImgpVerifyMinimalCodeIntegrityInitialization();
    if (!NT_SUCCESS(Status)) {
        goto LoadCatalogEnd;
    }

    //
    // Construct a full file path to the specified catalog.
    //

    CatalogRootLength = (ULONG)wcslen(BlImgCatalogDirectoryPath) + 1;
    RequiredBufferSize =
        (CatalogRootLength + (ULONG)wcslen(CatalogName)) * sizeof(WCHAR);

    CatalogFilePath = BlMmAllocateHeap(RequiredBufferSize);
    if (CatalogFilePath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto LoadCatalogEnd;
    }

    wcscpy_s(CatalogFilePath,
             RequiredBufferSize / sizeof(WCHAR),
             BlImgCatalogDirectoryPath);

    wcscat_s(CatalogFilePath, RequiredBufferSize / sizeof(WCHAR), CatalogName);

    //
    // Attempt to open the specified catalog.
    //

    Status = BlFileOpen(BlImgCatalogDeviceId,
                        CatalogFilePath,
                        OPEN_READ | OPEN_CATALOG,
                        &FileId);

    BlMmFreeHeap(CatalogFilePath);
    if (!NT_SUCCESS(Status)) {
        goto LoadCatalogEnd;
    }

    Status = BlFileGetInformation(FileId, &Info);
    if (!NT_SUCCESS(Status)) {
        goto LoadCatalogEnd;
    }

    ASSERT(Info.FileSize < 0x100000000);

    //
    // Catalog file of 0 size is an invalid catalog.
    //

    if (Info.FileSize == 0) {
        Status = STATUS_INVALID_IMAGE_HASH;
        goto LoadCatalogEnd;
    }

    //
    // Allocate a memory buffer for the in-memory copy of the image.
    //

    CatalogSize = (ULONG)Info.FileSize;
    Status = BlImgAllocateImageBuffer(&CatalogImageBase,
                                      CatalogSize,
                                      MEMORY_TYPE_MISC_DATA,
                                      MEMORY_ATTRIBUTE_NONE,
                                      0,
                                      IMAGE_FLAGS_NONE);

    if (!NT_SUCCESS(Status)) {
        goto LoadCatalogEnd;
    }

    //
    // Read the entire file in one chunk.
    //

    Status = BlFileReadAtOffset(FileId, CatalogSize, 0, CatalogImageBase, NULL);
    if (!NT_SUCCESS(Status)) {
        goto LoadCatalogEnd;
    }

    //
    // Add catalog to the list of loaded catalogs.
    //

    CatalogEntryHeader =
        BlMmAllocateHeap(sizeof(LIST_ENTRY) + sizeof(BOOT_CATALOG_ENTRY));
    if (CatalogEntryHeader == NULL) {
        Status = STATUS_NO_MEMORY;
        goto LoadCatalogEnd;
    }

    *CatalogEntry = (PBOOT_CATALOG_ENTRY)(CatalogEntryHeader + 1);
    Status = MinCrypL_AddCatalog2(CatalogImageBase,
                                  CatalogSize,
                                  SIPolicyAreOemIdRulesActive(),
                                  *CatalogEntry,
                                  &PolicyInfo);

    if (!NT_SUCCESS(Status)) {
        goto LoadCatalogEnd;
    }

    CatalogAddedToMincrypt = TRUE;

    //
    //  Copy PolicyInfo.
    //

    Status = ImgpCopyPolicyInfo(&PolicyInfo, &(*CatalogEntry)->PolicyInfo);
    if (!NT_SUCCESS(Status)) {
        goto LoadCatalogEnd;
    }

    if (ImgpIsSigningPolicySet() == FALSE) {

        if (CHECK_FLAG(PolicyInfo.ulPolicyBits, BlImgAcceptedRootKeys) != FALSE) {
            Status = ImgpValidateDefaultSigningPolicy(ImgSigningScenarioDriver,
                                                      &PolicyInfo,
                                                      &((*CatalogEntry)->SigningTime),
                                                      CatalogName,
                                                      LoadInformation);

            if (NT_SUCCESS(Status)) {
                (*CatalogEntry)->UserContext = PolicyInfo.ulPolicyBits;
                goto LoadCatalogEnd;
            }
        }
    }
    else {
        //
        // This catalog may not be suitable for this scenario but we don't want
        // to fail the load. So we check against all scenarios.
        //

        Status = ImgpValidateSigningPolicyAgainstAllScenarios(
                        Scenario,
                        PolicyInfo.pChainInfo,
                        FALSE,
                        &MatchingSigner);

        if (NT_SUCCESS(Status)) {

            //
            // Validate this catalog against the Package Manifest 
            //

            Status = ImgpValidatePlatformManifestForCatalog(
                        MatchingSigner,
                        &((*CatalogEntry)->PackageName));

            if (!NT_SUCCESS(Status)) {
                goto LoadCatalogEnd;
            }

            (*CatalogEntry)->UserContext = MatchingSigner;
            if (NULL == BlImgLoadedCatalogs.Flink) {
                InitializeListHead(&BlImgLoadedCatalogs);
            }
            InsertTailList(&BlImgLoadedCatalogs, CatalogEntryHeader);
            goto LoadCatalogEnd;
        }
    }

    Status = STATUS_INVALID_IMAGE_HASH;

LoadCatalogEnd:
    if (FileId != INVALID_FILE_ID) {
        BlFileClose(FileId);
    }

    //
    // If this routine failed to fully load and map the catalog, free the
    // allocations.
    //

    if (!NT_SUCCESS(Status)) {

        if (CatalogImageBase != NULL) {
            BlMmFreePages(CatalogImageBase);
        }

        if (*CatalogEntry != NULL)
        {
            if ((*CatalogEntry)->PolicyInfo != NULL) {
                BlMmFreeHeap((*CatalogEntry)->PolicyInfo);
                (*CatalogEntry)->PolicyInfo = NULL;
            }

            if (CatalogAddedToMincrypt != FALSE)
            {
                MinCrypL_RemoveCatalog(*CatalogEntry);
            }
            *CatalogEntry = NULL;
        }

        if (CatalogEntryHeader != NULL) {
            BlMmFreeHeap(CatalogEntryHeader);
            CatalogEntryHeader = NULL;
        }
    }

    return Status;
}

NTSTATUS
ImgpLoadNextCatalog (
    _In_ IMG_SIGNING_SCENARIO_TYPE Scenario,
    _Outptr_ struct _BOOT_CATALOG_ENTRY **CatalogEntry,
    _Out_opt_ PULONG LoadInformation
    )

/*++

Routine Description:

    Loads the next boot catalog enumerated in the system catalog directory.

Arguments:

    Scenario - Supplies the type of signing scenario currently being used.

    CatalogEntry - Supplies a pointer to a variable that receives address of
        allocated per catalog structure.

   LoadInformation - Supplies an optional pointer to a variable that receives
        informational flags describing various aspects of the load.  The
        following flags may be set during image load.

        LOAD_INFORMATION_WHQL_EXEMPTED_FOR_BOOT_DRIVER : Indicates that the 
            image failed the WHQL requirement, but was allowed to load for
            interest of not bricking the machine.

        LOAD_INFORMATION_WHQL_EXEMPTED_BY_AUDIT_MODE : Indicates that the 
            image failed the WHQL requirement, but was allowed to load because
            SI Audit Mode is enabled.
            
Return Value:

    STATUS_SUCCESS if function succeeded.

    STATUS_NOT_FOUND if no more files are found in catalog directory.

    Another error status code if returned from a sub-routine.

--*/

{

    BOOLEAN FoundMatch;
    ULONG Index;
    ULONG Length;
    PUNICODE_STRING PrecedentString;
    NTSTATUS Status;

    *CatalogEntry = NULL;
    Status = STATUS_SUCCESS;

    //
    // Return immediately if the entire catalog directory has already been
    // enumerated.
    //

    if (BlImgCatalogEnumerationFinished != FALSE) {
        Status = STATUS_NOT_FOUND;
        goto LoadNextCatalogEnd;
    }

    //
    // Check that catalog directory is opened.
    //

    if (BlImgCatalogDirectoryId == INVALID_FILE_ID) {
        Status = STATUS_NOT_FOUND;
        goto LoadNextCatalogEnd;
    }

    //
    // Enumerate the files in the catalog directory in search for the next
    // catalog file to load.  Skip over non-catalog files, the system catalog
    // and catalogs based on the global precedence list.
    //

    while (TRUE) {
        Status = BlFileGetNextFile(BlImgCatalogDirectoryId,
                                   &BlImgLastFoundCatalogFileName,
                                   &BlImgLastFoundCatalogFileNameSize,
                                   TRUE);

        //
        // When the enumeration is complete, perform a second pass on the
        // directory if the current pass performed filtering based on the
        // global catalog precedence list.
        //

        if (!NT_SUCCESS(Status)) {
            if ((BlImgCatalogPrecedenceList != NULL) &&
                (BlImgCatalogEnumerationPass == 0)) {

                BlImgCatalogEnumerationPass += 1;
                if (BlImgLastFoundCatalogFileName != NULL) {
                    BlImgLastFoundCatalogFileName[0] = '\0';
                }

                continue;
            }

            if (BlImgLastFoundCatalogFileName != NULL) {
                BlMmFreeHeap(BlImgLastFoundCatalogFileName);
                BlImgLastFoundCatalogFileName = NULL;
                BlImgLastFoundCatalogFileNameSize = 0;
            }

            if (BlImgCatalogDirectoryPath != NULL) {
                BlMmFreeHeap(BlImgCatalogDirectoryPath);
                BlImgCatalogDirectoryPath = NULL;
            }

            BlImgCatalogEnumerationFinished = TRUE;
            goto LoadNextCatalogEnd;
        }

        //
        // Check if the file which was found ends with ".cat" extension.
        //

        Length = (ULONG)wcslen(BlImgLastFoundCatalogFileName);
        if (Length <= (RTL_NUMBER_OF(IMGLOAD_CATALOG_EXT) - 1)) {
            continue;
        }

        Length -= (RTL_NUMBER_OF(IMGLOAD_CATALOG_EXT) - 1);
        if (_wcsicmp(BlImgLastFoundCatalogFileName + Length,
                     IMGLOAD_CATALOG_EXT) != 0) {

            continue;
        }

        //
        // Check if the main boot catalog file was found, if yes then skip it
        // and find different catalog file.
        //

        if (_wcsicmp(BlImgLastFoundCatalogFileName,
                     BlImgSystemCatalogName) == 0) {

            continue;
        }

        //
        // Determine if the current filename matches a string in the precedence
        // list.  During pass 0, skip all catalogs that don't match a string
        // in the precedence list.  During pass 1, skip the catalogs loaded
        // during pass 0.
        //

        if (BlImgCatalogPrecedenceList != NULL) {
            FoundMatch = FALSE;
            for (Index = 0;
                 Index < BlImgCatalogPrecedenceListEntries;
                 Index += 1) {

                PrecedentString = &BlImgCatalogPrecedenceList[Index];
                if (_wcsnicmp(BlImgLastFoundCatalogFileName,
                              PrecedentString->Buffer,
                              PrecedentString->Length / sizeof(WCHAR)) == 0) {

                    FoundMatch = TRUE;
                    break;
                }
            }

            if ((BlImgCatalogEnumerationPass == 0) && (FoundMatch == FALSE)) {
                continue;
            }

            if ((BlImgCatalogEnumerationPass == 1) && (FoundMatch != FALSE)) {
                continue;
            }
        }

        Status = ImgpLoadCatalog(BlImgLastFoundCatalogFileName,
                                 Scenario,
                                 CatalogEntry,
                                 LoadInformation);

        if (!NT_SUCCESS(Status)) {
            continue;
        }

        break;
    }

LoadNextCatalogEnd:
    return Status;
}

NTSTATUS
ImgpLoadRevocationList (
    __in DEVICE_ID RevocationListDevice,
    __in PWSTR RevocationListPath
    )

/*++

Routine Description:

    Loads a revocation list into memory and registers it with the mincrypt
    library.

Arguments:

    RevocationListDevice - Supplies the id of the device that the revocation
         list resides upon.

    RevocationListPath   - Supplies a path to the revocation file to load.

Return Value:

    STATUS_SUCCESS if function succeeded.

    STATUS_NO_MEMORY if memory allocation failed.

    STATUS_INVALID_IMAGE_HASH if revocation list is invalid.

    Another error status code if returned from a sub-routine.

--*/

{

    FILE_ID FileId;
    FILE_INFORMATION Info;
    PVOID ParsedRevocationList;
    ULONG ParsedRevocationListSize;
    LARGE_INTEGER ParsedRevocationTime;
    PVOID RevocationBase;
    ULONG RevocationSize;
    NTSTATUS Status;
    PVOID VarRevocationBase;
    ULONG VarRevocationSize;
    LARGE_INTEGER VarRevocationTime;

    FileId = INVALID_FILE_ID;
    RevocationBase = NULL;
    Status = STATUS_SUCCESS;

    //
    // Verify that minimal code integrity initialization has completed
    // successfully.
    //

    Status = ImgpVerifyMinimalCodeIntegrityInitialization();
    if (!NT_SUCCESS(Status)) {
        goto LoadRevocationListEnd;
    }

    //
    // If Secure Boot is active, try to load the revocation list from the
    // firmware copy. If it doesn't succeed, don't fail just try and use the
    // on-disk revocation list instead.
    //

    if (BlSecureBootIsPolicyActive() != FALSE) {
        Status = BlSecureBootGetRevocationListVariable(&VarRevocationBase,
                                                        &VarRevocationSize,
                                                        &VarRevocationTime);
        if (NT_SUCCESS(Status)) {
            Status = BlImgSetParsedRevocationList(VarRevocationBase,
                                                    VarRevocationSize,
                                                    &VarRevocationTime);
            if (NT_SUCCESS(Status)) {
                goto LoadRevocationListEnd;
            }

            BlMmFreeHeap(VarRevocationBase);
            VarRevocationBase = NULL;
        }
    }

    //
    // Attempt to open a revocation file.
    //

    Status = BlFileOpen(RevocationListDevice,
                        RevocationListPath,
                        OPEN_READ,
                        &FileId);

    if (!NT_SUCCESS(Status)) {
        Status = STATUS_SUCCESS;
        goto LoadRevocationListEnd;
    }

    Status = BlFileGetInformation(FileId, &Info);
    if (!NT_SUCCESS(Status)) {
        goto LoadRevocationListEnd;
    }

    ASSERT(Info.FileSize < 0x100000000);

    //
    // Revocation list of 0 size is an invalid revocation list.
    //

    if (Info.FileSize == 0) {
        Status = STATUS_INVALID_IMAGE_HASH;
        goto LoadRevocationListEnd;
    }

    //
    // Allocate a memory buffer for the in-memory copy of the image.
    //

    RevocationSize = (ULONG)Info.FileSize;
    Status = BlImgAllocateImageBuffer(&RevocationBase,
                                      RevocationSize,
                                      MEMORY_TYPE_MISC_DATA,
                                      MEMORY_ATTRIBUTE_NONE,
                                      0,
                                      IMAGE_FLAGS_NONE);

    if (!NT_SUCCESS(Status)) {
        goto LoadRevocationListEnd;
    }

    //
    // Read the entire file in one chunk.
    //

    Status = BlFileReadAtOffset(FileId,
                                RevocationSize,
                                0,
                                RevocationBase,
                                NULL);

    if (!NT_SUCCESS(Status)) {
        goto LoadRevocationListEnd;
    }

    Status = BlImgParseRevocationList(RevocationBase,
                                      RevocationSize,
                                      &ParsedRevocationList,
                                      &ParsedRevocationListSize,
                                      &ParsedRevocationTime);

    if (!NT_SUCCESS(Status)) {
        goto LoadRevocationListEnd;
    }

    Status = BlImgSetParsedRevocationList(ParsedRevocationList,
                                          ParsedRevocationListSize,
                                          &ParsedRevocationTime);

    if (!NT_SUCCESS(Status)) {
        goto LoadRevocationListEnd;
    }

LoadRevocationListEnd:
    if (FileId != INVALID_FILE_ID) {
        BlFileClose(FileId);
    }

    if (!NT_SUCCESS(Status)) {
        if (RevocationBase != NULL) {
            BlMmFreePages(RevocationBase);
        }
    }

    return Status;
}

SI_SIGNING_SCENARIO_TYPE 
ImgpConvertCiScenarioToSiScenario(
    _In_ IMG_SIGNING_SCENARIO_TYPE Scenario
    )
/*++

Routine Description:

    Converts  IMG_SIGNING_SCENARIO_TYPE to SI_SIGNING_SCENARIO_TYPE.

Arguments:

    Scenario - IMG_SIGNING_SCENARIO_TYPE type.

Return Value:

    SI_SIGNING_SCENARIO_TYPE type.
--*/
{
    switch (Scenario)
    {
        case ImgSigningScenarioWindows:
            __fallthrough;
        case ImgSigningScenarioHalExtension:
            __fallthrough;
        case ImgSigningScenarioElamDriver:
            __fallthrough;
        case ImgSigningScenarioDriver:
            __fallthrough;
        case ImgSigningScenarioCoreExtension:
            __fallthrough;
        case ImgSigningScenarioBootApplication:

            return SiSigningScenarioKernelMode;

        case ImgSigningScenarioBootRevocationList:
            __fallthrough;
        case ImgSigningScenarioFirmwareUpdate:
            return SiSigningScenarioNone;

        default:
                NT_ASSERT(FALSE);
                break;

    }
    return SiSigningScenarioNone;
}

NTSTATUS
BlImgpProcessBootAuditOnFailure(
    _In_ SYSTEM_INTEGRITY_POLICY_HANDLE PolicyHandle,
    _In_ NTSTATUS StatusCode
    )
/*++

Routine Description:

    Process "Boot Audit On Failure" option if any.

Arguments:

    PolicyHandle - Supplies the handle to the SI policy.

    StatusCode - Supplies the original status code.

Return Value:

    STATUS_SUCCESS if function succeeded.

    Error status code otherwise.

--*/
{
    ULONG HvciOptions;
    BOOLEAN IsSignedPolicyRequired = FALSE;
    ULONG PolicyOptions;
    SYSTEM_INTEGRITY_POLICY_TYPE PolicyType = SiPolicyTypeInvalid;
    ULONGLONG PolicyVersion = 0;
    PUCHAR RawData;
    ULONG RawDataSize;
    PWSTR RelativeFileNamePath = NULL;
    NTSTATUS Status;

    Status = SIPolicyGetOptions(PolicyHandle, &PolicyOptions, &HvciOptions);
    if (!NT_SUCCESS(Status)) {
        goto ProcessBootAuditOnFailureEnd;
    }
    if (CHECK_FLAG(PolicyOptions, SYSTEM_INTEGRITY_POLICY_ENABLE_BOOT_AUDIT_ON_FAILURE) == FALSE)
    {
        //
        //  No "Boot Audit on Failure" option to process.
        //

        Status = StatusCode;
        goto ProcessBootAuditOnFailureEnd;
    }

    Status = SIPolicyGetPolicyType(PolicyHandle, &PolicyType);
    if (!NT_SUCCESS(Status))
    {
        goto ProcessBootAuditOnFailureEnd;
    }

    if (PolicyType == SiPolicyTypeRevoke)
    {
        //
        //  Revoke policy should not have "Boot Audit on Failure" option.
        //

        NT_ASSERT(FALSE);
        goto ProcessBootAuditOnFailureEnd;
    }

    Status = BlSIPolicyIsSignedPolicyRequired(PolicyType,
                                              &IsSignedPolicyRequired,
                                              &PolicyVersion);
    if (!NT_SUCCESS(Status)) {
        goto ProcessBootAuditOnFailureEnd;
    }

    if (CHECK_FLAG(PolicyOptions, SYSTEM_INTEGRITY_POLICY_ENABLE_BOOT_AUDIT_ON_FAILURE) != FALSE) {
        
        //
        //  Audit mode already on.
        //

        Status = STATUS_SUCCESS;
        goto ProcessBootAuditOnFailureEnd;

    } else if (IsSignedPolicyRequired == FALSE) {

        //
        //  When audit mode is not on, but unsigned policy is allowed and 
        //  "Boot Audit On Failure" is enabled, then turn on audit mode,
        //   measure the changed policy.
        //

        Status = SIPolicySetOptions(PolicyHandle,
                    PolicyOptions | SYSTEM_INTEGRITY_POLICY_ENABLE_AUDIT_MODE,
                    HvciOptions);
        if (!NT_SUCCESS(Status)) {
            goto ProcessBootAuditOnFailureEnd;
        }

        Status = SIPolicyGetPolicyInfoFromType(PolicyType,
                                               NULL,
                                               NULL,
                                               NULL,
                                               &RelativeFileNamePath,
                                               NULL);
        if (!NT_SUCCESS(Status)) {
            goto ProcessBootAuditOnFailureEnd;
        }

        Status = SIPolicyGetRawData(PolicyHandle,
                                    &RawData,
                                    &RawDataSize);
        if (!NT_SUCCESS(Status)) {
            goto ProcessBootAuditOnFailureEnd;
        }

        Status = BlSIPolicyRepackageActivePolicyForKernel();
        if (!NT_SUCCESS(Status))
        {
            goto ProcessBootAuditOnFailureEnd;
        }
            
        //
        //  Measure modified policy.
        //

        BlSiMeasurePolicyBlob(PolicyVersion,
                              RelativeFileNamePath,
                              RawData,
                              RawDataSize);

        goto ProcessBootAuditOnFailureEnd;
    }

    //
    //  For other cases, return original status code.
    //

    Status = StatusCode;

ProcessBootAuditOnFailureEnd:

    return Status;
}

NTSTATUS
BlImgpApplySIPolicy(
    _In_ IMG_SIGNING_SCENARIO_TYPE ImgScenario,
    _In_reads_bytes_(BaseImageSize) PVOID BaseImageAddr,
    _In_ ULONG BaseImageSize,
    _In_z_ PCWSTR LoadFileName,
    _In_ ALG_ID HashAlgId,
    _In_reads_bytes_(MAX_HASH_LEN) PCUCHAR HashBuffer,
    _In_ PMINCRYPT_CHAIN_INFO ChainInfo,
    _Inout_opt_ PULONG LoadInformation
    )
/*++

Routine Description:

    Apply System Integrity Policy to an image

Arguments:

    ImgScenario - Supplies the type of signing scenario to validate to.

    BaseImageAddr - Base address for the loaded image

    BaseImageSize - Loaded image size

    LoadFileName - File name for the loaded image

    HashAlgId - File hash algorithm ID

    HashBuffer - File hash

    ChainInfo - Chain info about the signing certificate

    LoadInformation - Supplies an optional pointer to a variable that receives
        informational flags describing various aspects of the load.  The
        following flags may be set during image load. However, only
        LOAD_INFORMATION_POLICY_VIOLATION and LOAD_INFORMATION_POLICY_VIOLATION_AUDIT
        will be set in this function.

        LOAD_INFORMATION_EMBEDDED_SIGNED : Indicates that the image is verified
            using embedded signature.

        LOAD_INFORMATION_CATALOG_SIGNED : Indicates that the image is verified
            using a signature from a catalog.

        LOAD_INFORMATION_POLICY_VIOLATION : Indicates that the image violated
            system integrity policy.

        LOAD_INFORMATION_POLICY_VIOLATION_AUDIT : Indicates that the image violated
            system integrity policy but that and audit policy is in place 
            which allows the image.

Return Value:

    STATUS_SUCCESS if function succeeded.

    STATUS_NO_MEMORY if memory allocation failed.

    STATUS_INVALID_IMAGE_HASH if revocation list is invalid.

    Another error status code if returned from a sub-routine.

--*/
{
    NTSTATUS Status = STATUS_SUCCESS;
    ULONGLONG VersionInfo = 0;
    PWSTR LastBackSlash = NULL;
    BOOLEAN SiPolicyAuditMode = FALSE;
    NTSTATUS Status2 = STATUS_SUCCESS;
    UCHAR HashLength = 0;
    UNICODE_STRING FileName = { 0 };
    PVOID Resource;
    ULONG ResourceSize;
    SYSTEM_INTEGRITY_POLICY_TYPE PolicyType = SiPolicyTypeInvalid;
    ULONG i = 0;
    SI_CHAIN_INFOW SiChainInfo = { 0 };
    SI_SIGNING_SCENARIO_TYPE Scenario;

    Scenario = ImgpConvertCiScenarioToSiScenario(ImgScenario);

    if ((Scenario == SiSigningScenarioNone) ||
        (SI_POLICY_IS_CODE_POLICY_ACTIVE() == FALSE)) {
        goto Cleanup;
    }

    //
    // get version and filename info
    //

    Status = STATUS_NOT_FOUND;

    Resource = BlResourceFindVersionFromImage(BaseImageAddr,
                                              BaseImageSize,
                                              &ResourceSize);

    if (Resource != NULL && ResourceSize > 0)
    {
        Status = SIPolicyGetOriginalFilenameAndVersionFromVersionResource(
                    Resource,
                    ResourceSize,
                    &FileName,
                    &VersionInfo);
    }

    if (Status == STATUS_NOT_FOUND) {

        if (LoadFileName != NULL) {

            //
            //  for file that does not have version info in resource
            //

            LastBackSlash = wcsrchr(LoadFileName, L'\\');
            if (LastBackSlash != NULL) {

                LastBackSlash++;

                if (*LastBackSlash) {
                    RtlInitUnicodeString(&FileName, LastBackSlash);
                }
            }
        }
        Status = STATUS_SUCCESS;
    }

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    switch (HashAlgId)
    {

    case CALG_SHA1:
        HashLength = RTL_SHA1_HASH_LEN;
        break;

    case CALG_SHA_256:
        HashLength = RTL_SHA256_HASH_LEN;
        break;

    case CALG_SHA_384:
        HashLength = RTL_SHA384_HASH_LEN;
        break;

    case CALG_SHA_512:
        HashLength = RTL_SHA512_HASH_LEN;
        break;

    //
    // TFS-2382892: Support MD5 in ConfigCI.
    //

    case CALG_MD5:
        HashLength = RTL_MD5_HASH_LEN;
        break;

    default:

        //
        // CALG_MD5 is the miniumum hash algorithm, other
        // banned ones will not be supported.
        //

        Status = STATUS_INVALID_IMAGE_HASH;
        goto Cleanup;

    }
    Status = SIPolicyConvertChainInfo(ChainInfo, &SiChainInfo);
    if (!NT_SUCCESS(Status))
    {
        goto Cleanup;
    }
    for (i = 0; i < g_NumberOfSiPolicies; ++ i)  {
        Status = SIPolicyGetPolicyType(g_SiPolicyHandles[i], &PolicyType);
        if (!NT_SUCCESS(Status)) {
            goto Cleanup;
        }
        if (SI_POLICY_TYPE_IS_CODE_EXECUTION_POLICY(PolicyType) == FALSE) {
            continue;
        }
        Status = SIPolicyValidateImage(g_SiPolicyHandles[i],
            BlImgSignerTypeMax,
            Scenario,
            &SiChainInfo,
            HashLength,
            (PUCHAR)HashBuffer,
            FALSE,
            &FileName,
            VersionInfo);

        if (!NT_SUCCESS(Status)) {
            if (ARGUMENT_PRESENT(LoadInformation)) {
                *LoadInformation |= LOAD_INFORMATION_POLICY_VIOLATION;
            }

            BlSIPolicyLogFailureEvent(BL_OSL_ERROR_SI_POLICY, Status, &FileName);

            Status2 = SIPolicyAuditModeEnabled(g_SiPolicyHandles[i], &SiPolicyAuditMode);

            if (!NT_SUCCESS(Status2)) {

                Status = Status2;
                BlSIPolicyLogFailureEvent(BL_OSL_ERROR_SI_POLICY, Status2, &FileName);
                goto Cleanup;
            }

            if (SiPolicyAuditMode != FALSE) {

                Status = STATUS_SUCCESS;
                if (ARGUMENT_PRESENT(LoadInformation)) {
                    *LoadInformation |= LOAD_INFORMATION_POLICY_VIOLATION_AUDIT;
                }
            } else {
                Status = BlImgpProcessBootAuditOnFailure(g_SiPolicyHandles[i], Status);
            }

            if (!NT_SUCCESS(Status)) {
                goto Cleanup;
            }
        }
        else if (ARGUMENT_PRESENT(LoadInformation)) {

            //
            // When a primary signature fails, but a secondary signature or catalog
            // succeeds, the violation flag must be removed so that CI does not log
            // the image as having failed.
            //

            CLEAR_FLAGS(*LoadInformation, LOAD_INFORMATION_POLICY_VIOLATION);
            CLEAR_FLAGS(*LoadInformation, LOAD_INFORMATION_POLICY_VIOLATION_AUDIT);
        }
    }

Cleanup:
    SIPolicyFreeSIChainInfo(&SiChainInfo);
    return Status;
}

NTSTATUS
ImgpValidateImageHash (
    __in PWSTR LoadFile,
     _In_reads_bytes_ (BaseImageSize) PVOID BaseImageAddr,
     _In_ ULONG BaseImageSize,
    __inout_opt PMINCRYPT_SIGNATURE_DESCRIPTOR Signature,
    __in ALG_ID HashAlgId,
    __in_bcount(MAX_HASH_LEN) UCHAR HashBuffer[MAX_HASH_LEN],
    __in ULONG ValidationFlags,
    __in IMG_SIGNING_SCENARIO_TYPE Scenario,
    __out_opt PIMG_RETRY_REASON RetryReason,
    __out_opt PBOOT_TRUSTED_BOOT_INFORMATION BootTrustedBootInformation,
    __inout_opt PULONG LoadInformation
    )

/*++

Routine Description:

    Validates the full-binary hash of a PE image.

Arguments:

    LoadFile - Supplies the path of the image being loaded.

    BaseImageAddr - Supplies a pointer to the image being loaded.

    BaseImageSize - Supplies the size in bytes of the image being loaded.

    Signature - Supplies the embedded signature to be used for image
         verification.

    HashAlgId - Identifier of the algorithm used to create the hash.

    HashBuffer - Supplies a hash of the binary.

    ValidationFlags - Supplies flags controlling the characteristics of the
        validation operation.  Valid flags include the following.

        IMGP_NO_CATALOG_CHECKS : Indicates that the image is self signed and
            should not be checked against any certificate catalogs.

        IMGP_NO_REVOCATION_LIST_CHECKS : Indicates that the image is not
            subject to certificate revocation and thus does not need to be
            checked against the revocation list.

        IMGP_MS_SIGNATURE_REQUIRED : Indicates that the image requires a
            Microsoft certificate signature.

        IMGP_TRUSTEDBOOT_ROOT_REQUIRED : Indicates that the image requires a
            Trusted Boot signature.

        IMGP_HALEXTENSION_ROOT_REQUIRED : Indicates that the image requires a
            HAL extension signature.

        IMGP_WINDOWS_ROOT_REQUIRED : Indicates that the image requires a
            Windows signature.

        IMGP_LATEST_MS_ROOT_REQUIRED : Indicates that the image must chain to
            the latest Microsoft root, not previous versions of the OS.

        IMGP_NO_SIGNATURE_EXPIRATION_CHECKS : Indicates that the signature
            expiration check will be skipped.
            Note that this flag is NOT supported for catalog signed image.
    
    Scenario - Supplies the type of signing scenario to validate to.

    RetryReason - Supplies an optional pointer to a structure that receives a
         reason for why a retry is needed.

    BootTrustedBootInfo - Supplies a pointer to a structure that receives
        advanced cryptographic information for the image.

   LoadInformation - Supplies an optional pointer to a variable that receives
        informational flags describing various aspects of the load.  The
        following flags may be set during image load.

        LOAD_INFORMATION_EMBEDDED_SIGNED : Indicates that the image is verified
            using embedded signature.

        LOAD_INFORMATION_CATALOG_SIGNED : Indicates that the image is verified
            using a signature from a catalog.

        LOAD_INFORMATION_POLICY_VIOLATION : Indicates that the image violated
            system integrity policy.
            
        LOAD_INFORMATION_WHQL_EXEMPTED_BY_AUDIT_MODE : Indicates that the 
            image failed the WHQL requirement, but was allowed to load because
            SI Audit Mode is enabled.
            
        LOAD_INFORMATION_WHQL_EXEMPTED_FOR_BOOT_DRIVER : Indicates that the 
            image failed the WHQL requirement, but was allowed to load for
            interest of not bricking the machine.

Return Value:

    STATUS_SUCCESS if function succeeded.

    STATUS_NO_MEMORY if memory allocation failed.

    STATUS_INVALID_IMAGE_HASH if the binary hash was not found.

    STATUS_RETRY if the operation should be retried with a different hash.

    Another error status code if returned from a sub-routine.

--*/

{

    ULONG AcceptedRootKeys;
    PBOOT_CATALOG_ENTRY CatalogEntry;
    BOOLEAN CatalogSigned;
    BOOLEAN CheckCatalogs;
    PSTR CheckForEku;
    BOOLEAN CheckRevocationList;
    BOOLEAN DisableExpirationCheck;
    BOOLEAN EmbeddedSigned;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[1];
    LARGE_INTEGER FileHashNotAfter;
    ULONG Length;
    NTSTATUS LoadStatus;
    BOOLEAN MaybeWeak;
    MINCRYPT_TRUSTEDBOOT_INFO MincryptTrustedBootInformation;
    MINCRYPT_POLICY_INFO PolicyInfo;
    MINCRYPT_POLICY_REQS PolicyRequirements;
    PLARGE_INTEGER SigningTime;
    NTSTATUS Status;
    ULONG VerificationSigner;
#if !defined(MISC_MIN)
    ALG_ID FoundHashAlgorithm;
    LARGE_INTEGER LocalSigningTime;
    NTSTATUS QueryStatus;
#endif

    EmbeddedSigned = FALSE;
    CatalogSigned = FALSE;
    FileHashNotAfter.QuadPart = 0;
    MaybeWeak = FALSE;
    SigningTime = NULL;

    if (ARGUMENT_PRESENT(RetryReason)) {
        RtlZeroMemory(RetryReason, sizeof(*RetryReason));
    }

    //
    // Verify that minimal code integrity initialization has completed
    // successfully.
    //

    Status = ImgpVerifyMinimalCodeIntegrityInitialization();
    if (!NT_SUCCESS(Status)) {
        goto ValidateImageHashEnd;
    }

    //
    // Decode the validation flags that control properties of the signature
    // check.
    //

    AcceptedRootKeys = BlImgAcceptedRootKeys;
    if (CHECK_FLAG(ValidationFlags, IMGP_MS_SIGNATURE_REQUIRED)) {
        CLEAR_FLAGS(AcceptedRootKeys,
                    MINCRYPT_POLICY_PICROOT | MINCRYPT_POLICY_DMDROOT);
    }

    CheckCatalogs = TRUE;
    if (CHECK_FLAG(ValidationFlags, IMGP_NO_CATALOG_CHECKS)) {
        CheckCatalogs = FALSE;
    }

    CheckRevocationList = TRUE;
    if (CHECK_FLAG(ValidationFlags, IMGP_NO_REVOCATION_LIST_CHECKS)) {
        CheckRevocationList = FALSE;
    }

    CheckForEku = NULL;
    if (CHECK_FLAG(ValidationFlags, IMGP_TRUSTEDBOOT_ROOT_REQUIRED) != FALSE) {
        CheckForEku = szOID_KP_KERNEL_MODE_TRUSTED_BOOT_SIGNING;
        AcceptedRootKeys = BlImgAcceptedTrustedBootRootKeys;
    }

    if (CHECK_FLAG(ValidationFlags, IMGP_HALEXTENSION_ROOT_REQUIRED) != FALSE) {

        //
        // HAL extensions use the same roots as for Trusted Boot, but with a
        // different EKU in the signing certificate.
        //

        CheckForEku = szOID_KP_KERNEL_MODE_HAL_EXTENSION_SIGNING;
        AcceptedRootKeys = BlImgAcceptedTrustedBootRootKeys;
    }

    if (CHECK_FLAG(ValidationFlags, IMGP_WINDOWS_ROOT_REQUIRED) != FALSE) {
        CheckForEku = szOID_NT5_CRYPTO;
        CLEAR_FLAGS(AcceptedRootKeys, ~IMGLOAD_ALL_MICROSOFT_ROOTS);
    }

    if (CHECK_FLAG(ValidationFlags, IMGP_LATEST_MS_ROOT_REQUIRED) != FALSE) {
        CLEAR_FLAGS(AcceptedRootKeys, MINCRYPT_POLICY_PRSROOT);
        SET_FLAGS(AcceptedRootKeys, MINCRYPT_POLICY_PRSROOT_CURRENT);

        //
        // If test root is trusted, only trust the current test root.
        //

        if (CHECK_FLAG(AcceptedRootKeys, MINCRYPT_POLICY_TESTROOT) != FALSE) {
            CLEAR_FLAGS(AcceptedRootKeys, MINCRYPT_POLICY_TESTROOT);
            SET_FLAGS(AcceptedRootKeys, MINCRYPT_POLICY_TESTROOT_CURRENT);
        }
    }

    //
    // Determine if signature expiration check be disabled?
    //

    DisableExpirationCheck = FALSE;
    if (CHECK_FLAG(ValidationFlags, IMGP_NO_SIGNATURE_EXPIRATION_CHECKS) != FALSE) {
        DisableExpirationCheck = TRUE;
    }

#if !defined(MISC_MIN)
    //
    // Check if the file hash may be weak and so we need the signing time.
    //

    MaybeWeak = MincryptIsFileHashPossiblyWeak(HashAlgId, &FileHashNotAfter);
    if (MaybeWeak != FALSE) {
        SigningTime = &LocalSigningTime;
    }
#endif

    //
    // Check the embedded signature.  Return successfully if a signature exists
    // and roots to one of the accepted keys.
    //

    if ((ARGUMENT_PRESENT(Signature) != FALSE) &&
        (Signature->Signature != 0) &&
        (Signature->SignatureSize != 0)) {

        PolicyRequirements.cbSize = sizeof(PolicyRequirements);
        PolicyRequirements.ulPolicyBits = MINCRYPT_POLICY_REQ_TRANSITIVE_EKU;
        PolicyRequirements.cEKUs = 1;
        PolicyRequirements.rgwszEKUs[0] = szOID_MINCRYPT_PKIX_KP_CODE_SIGNING;

        //
        // If a custom policy is in use only the Code Signing EKU must be
        // checked for.
        //

        if (CheckForEku != NULL && ImgpIsSigningPolicySet() == FALSE) {
            PolicyRequirements.cEKUs++;
            PolicyRequirements.rgwszEKUs[1] = CheckForEku;
        }

        //
        // Skip signature expiration if requested.
        //

        if (DisableExpirationCheck != FALSE) {
            PolicyRequirements.ulPolicyBits |= MINCRYPT_POLICY_REQ_DISABLE_EXPIRATION_CHECK;
        }

#if !defined(MISC_MIN)
        //
        // If caller does not request for signing time and this is a driver,
        // we then explicitly ask for it here if WHQL policy is not enabled,
        // so that we can use later to determine if a driver should be grand
        // fathered if not WHQL signed.
        //

        if (Scenario == ImgSigningScenarioDriver &&
            ImgpIsWhqlPolicyEnabled() == FALSE &&
            SigningTime == NULL)
        {
            SigningTime = &LocalSigningTime;
        }

        //
        // If policy has OEM ID signer rules then the authenticated attributes
        // must be queried to obtain the SPC OPUS Info program name.
        //

        if (((Scenario != ImgSigningScenarioWindows) &&
             (Scenario != ImgSigningScenarioBootApplication)) &&
            (SIPolicyAreOemIdRulesActive() != FALSE)) {
            
            SET_FLAGS(PolicyRequirements.ulPolicyBits,
                      MINCRYPT_POLICY_REQ_RETURN_AUTH_ATTRS);
        }
#endif

        for (;;)
        {
            Status = MinCrypL_CheckSignedFile(
                        Signature,
                        HashBuffer,
                        CheckRevocationList,
                        &PolicyRequirements,
                        &MincryptTrustedBootInformation,
                        &PolicyInfo,
                        SigningTime);

            //
            // If the file hash or certificates are revoked, do not continue.
            //

            if ((Status == STATUS_IMAGE_CERT_REVOKED) ||
                (Status == STATUS_IMAGE_CERT_EXPIRED)) {
                break;
            }

            if (NT_SUCCESS(Status)) {

                if (ImgpIsSigningPolicySet() != FALSE) {
                    Status = BlSecurebootValidateSigningPolicy(Scenario,
                                                               PolicyInfo.pChainInfo,
                                                               FALSE,
                                                               &VerificationSigner);

                    if (NT_SUCCESS(Status)) {

                        //
                        // Check to see if the platform manifest authorizes this binary,
                        // if necessary.
                        //

                        Status = ImgpValidatePlatformManifestForEmbeddedSignedFile(
                                        VerificationSigner, 
                                        &PolicyInfo);
                    }
                }
                else if (CHECK_FLAG(PolicyInfo.ulPolicyBits, AcceptedRootKeys) == FALSE) {
                    Status = STATUS_INVALID_IMAGE_HASH;
                }
                else {
                    Status = ImgpValidateDefaultSigningPolicy(Scenario,
                                                              &PolicyInfo,
                                                              SigningTime,
                                                              LoadFile,
                                                              LoadInformation);
                }

#if !defined(MISC_MIN)
                if (NT_SUCCESS(Status) &&
                    (MaybeWeak != FALSE) &&
                    (ImgpIsWeakSigningTime(SigningTime,
                                           &FileHashNotAfter) != FALSE)) {

                    Status = STATUS_INVALID_IMAGE_HASH;
                }

                if (NT_SUCCESS(Status) && 
                    (CHECK_FLAG(ValidationFlags, IMGP_NO_CONFIG_CI_CHECKS) == 0)) {
                    Status = BlImgpApplySIPolicy(
                                Scenario,
                                BaseImageAddr,
                                BaseImageSize,
                                LoadFile,
                                HashAlgId,
                                HashBuffer,
                                PolicyInfo.pChainInfo,
                                LoadInformation);
                }
#endif
                if (NT_SUCCESS(Status)) {
                    EmbeddedSigned = TRUE;
                    Status = ImgpInitializeTrustedBootInformation(
                                BootTrustedBootInformation,
                                &MincryptTrustedBootInformation);

                    goto ValidateImageHashEnd;
                }
            }

#if !defined(MISC_MIN)
            //
            // Find the next signature for the same hash algorithm. If it's for
            // the same algorithm, loop back to try and verify it.
            //

            QueryStatus = MinCryptGetNextSignature(Signature,
                                                   HashAlgId,
                                                   HashAlgId,
                                                   &FoundHashAlgorithm);

            if (NT_SUCCESS(QueryStatus)) {
                if (FoundHashAlgorithm == HashAlgId) {
                    continue;
                }

                if (ARGUMENT_PRESENT(RetryReason)) {
                    RetryReason->EmbeddedSignatureHashAlgorithm = TRUE;
                    RetryReason->HashAlgorithm = FoundHashAlgorithm;
                    Status = STATUS_RETRY;
                    goto ValidateImageHashEnd;
                }
            }
#endif

            //
            // No more embedded signatures to try.
            //

            break;
        }
    }

    //
    // If we are this point then the Image is not self-signed. It is good to
    // log this case as not having a self-signed signature causes significant
    // performance delays in boot.
    //

    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)LoadFile,
                        (ULONG)((wcslen(LoadFile) + 1) * sizeof(WCHAR)));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_IMAGE_HASH_CHECK,
                  NULL,
                  1,
                  EventDataDescriptor);

    //
    // Preserve expiration and revocation errors.
    //

    if (CheckCatalogs == FALSE) {
        if (Status != STATUS_IMAGE_CERT_EXPIRED &&
            Status != STATUS_IMAGE_CERT_REVOKED) {

            Status = STATUS_INVALID_IMAGE_HASH;
        }

        goto ValidateImageHashEnd;
    }

    //
    // NOTE: We currently do not support disabling signature expiration check
    //       for catalog signed. This is OK, since it is only used by BootMgr,
    //       which is embedded signed.
    //

    if (CI_IS_CATALOG_HASH_ALGORITHM(HashAlgId) == FALSE) {

        //
        // If the current algorithm is not supported by the catalogs, try to
        // retry with one that is.
        //

        if (ARGUMENT_PRESENT(RetryReason)) {
            RetryReason->HashAlgorithmNotSupportedByCatalogs = TRUE;
            RetryReason->HashAlgorithm = CI_DEFAULT_CATALOG_HASH_ALGORITHM;
            Status = STATUS_RETRY;
        }
        else {
            Status = STATUS_INVALID_IMAGE_HASH;
        }

        goto ValidateImageHashEnd;
    }

    //
    // If the signature can be found in a catalog, check the already loaded
    // system catalogs for the image's signature.  Return successfully if the
    // signature exists and is rooted to one of the accepted keys.  If not,
    // iterate through the remaining system catalogs in search of the
    // signature.
    //

    Status = ImgpLoadAndValidateImageHashWithCatalog(
                LoadFile,
                BaseImageAddr,
                BaseImageSize,
                NULL,
                HashAlgId,
                HashBuffer,
                ValidationFlags,
                CheckRevocationList,
                Scenario,
                AcceptedRootKeys,
                MaybeWeak,
                &FileHashNotAfter,
                RetryReason,
                LoadInformation,
                BootTrustedBootInformation);

    if (NT_SUCCESS(Status) || (Status == STATUS_RETRY)) {
        CatalogSigned = TRUE;
        goto ValidateImageHashEnd;
    }

    //
    // Open the catalog directory and the main system catalog for the first
    // image that is not embedded signed.
    //

    if (BlImgCatalogDirectoryId == INVALID_FILE_ID) {

        //
        // Remove the trailing '\' and attempt to open the catalog
        // directory.
        //

        if (BlImgCatalogDirectoryPath == NULL) {
            Status = STATUS_INVALID_IMAGE_HASH;
            goto ValidateImageHashEnd;
        }

        Length = (ULONG)wcslen(BlImgCatalogDirectoryPath);
        BlImgCatalogDirectoryPath[Length - 1] = L'\0';
        LoadStatus = BlFileOpen(BlImgCatalogDeviceId,
                                BlImgCatalogDirectoryPath,
                                OPEN_DIRECTORY | OPEN_READ,
                                &BlImgCatalogDirectoryId);

        BlImgCatalogDirectoryPath[Length - 1] = L'\\';
        if (!NT_SUCCESS(LoadStatus)) {
            goto ValidateImageHashEnd;
        }
    }
        
    //
    // If a catalog hint has been set, try using it first.
    //
        
    if (BlImgCatalogHint != NULL) {
        LoadStatus = ImgpLoadCatalog(BlImgCatalogHint,
                                     Scenario,
                                     &CatalogEntry,
                                     LoadInformation);
            
        if (NT_SUCCESS(LoadStatus)) {
            Status = ImgpLoadAndValidateImageHashWithCatalog(
                        LoadFile,
                        BaseImageAddr,
                        BaseImageSize,
                        CatalogEntry,
                        HashAlgId,
                        HashBuffer,
                        ValidationFlags,
                        CheckRevocationList,
                        Scenario,
                        AcceptedRootKeys,
                        MaybeWeak,
                        &FileHashNotAfter,
                        RetryReason,
                        LoadInformation,
                        BootTrustedBootInformation);

            if (NT_SUCCESS(Status) || (Status == STATUS_RETRY)) {
                CatalogSigned = TRUE;
                goto ValidateImageHashEnd;
            }
        }
    }

    //
    // Load the main system catalog and check for the hash value.
    // Return successfully if the signature exists in the main system
    // catalog and it roots to an accepted key.
    //
        
    if (BlImgSystemCatalogLoaded == FALSE) {
        LoadStatus = ImgpLoadCatalog(BlImgSystemCatalogName,
                                     Scenario,
                                     &CatalogEntry,
                                     LoadInformation);
            
        if (NT_SUCCESS(LoadStatus)) {
            BlImgSystemCatalogLoaded = TRUE;

            Status = ImgpLoadAndValidateImageHashWithCatalog(
                        LoadFile,
                        BaseImageAddr,
                        BaseImageSize,
                        CatalogEntry,
                        HashAlgId,
                        HashBuffer,
                        ValidationFlags,
                        CheckRevocationList,
                        Scenario,
                        AcceptedRootKeys,
                        MaybeWeak,
                        &FileHashNotAfter,
                        RetryReason,
                        LoadInformation,
                        BootTrustedBootInformation);

            if (NT_SUCCESS(Status) || (Status == STATUS_RETRY)) {
                CatalogSigned = TRUE;
                goto ValidateImageHashEnd;
            }
        }
    }

    //
    // If global policy allows, iterate through the remaining catalog files
    // in search of a signature for the loaded image.
    //

    if (CHECK_FLAG(BlImgCatalogPolicy,
                    CATALOG_FLAGS_SYSTEM_CATALOG_ONLY) == FALSE) {

        while (TRUE) {
            LoadStatus = ImgpLoadNextCatalog(Scenario,
                                             &CatalogEntry,
                                             LoadInformation);
            if (!NT_SUCCESS(LoadStatus)) {
                break;
            }

            //
            // Check for the signature only in the new catalog.
            //

            Status = ImgpLoadAndValidateImageHashWithCatalog(
                        LoadFile,
                        BaseImageAddr,
                        BaseImageSize,
                        CatalogEntry,
                        HashAlgId,
                        HashBuffer,
                        ValidationFlags,
                        CheckRevocationList,
                        Scenario,
                        AcceptedRootKeys,
                        MaybeWeak,
                        &FileHashNotAfter,
                        RetryReason,
                        LoadInformation,
                        BootTrustedBootInformation);

            if (NT_SUCCESS(Status) || (Status == STATUS_RETRY)) {
                CatalogSigned = TRUE;
                goto ValidateImageHashEnd;
            }
        }
    }

    if (ARGUMENT_PRESENT(RetryReason) &&
        (HashAlgId != CI_DEFAULT_CATALOG_HASH_ALGORITHM)) {

        RetryReason->HashAlgorithmNotFoundInCatalogs = TRUE;
        RetryReason->HashAlgorithm = CI_DEFAULT_CATALOG_HASH_ALGORITHM;
        Status = STATUS_RETRY;
    }
    else {

#if !defined(MISC_MIN)
        if (Scenario == ImgSigningScenarioDriver) {

            ImgpLogDriverLoadAudit(NULL, FALSE, NULL,LoadFile);
        }
#endif

        Status = STATUS_INVALID_IMAGE_HASH;
    }

ValidateImageHashEnd:

    //
    // Only use a catalog hint once.
    //

    if ((BlImgCatalogHint != NULL) && (Status != STATUS_RETRY)) {
        BlMmFreeHeap(BlImgCatalogHint);
        BlImgCatalogHint = NULL;
    }

    //
    // Indicate if the image was catalog or embedded signed.
    //

    if (ARGUMENT_PRESENT(LoadInformation) != FALSE) {
        if (CatalogSigned != FALSE) {
            *LoadInformation |= LOAD_INFORMATION_CATALOG_SIGNED;

#if !defined(MISC_MIN)
            TraceLoggingWrite(g_CiTraceLoggingProvider,
                "CodeIntegritySignCheckTriggeredCatalogFallback",
                TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES),
                TraceLoggingHexInt32(Status, "NTStatus"),
                TraceLoggingWideString(LoadFile, "FileName"),
                TraceLoggingUInt32(HashAlgId, "HashAlgId"),
                TraceLoggingHexUInt8Array(HashBuffer, (UINT16)MAX_HASH_LEN, "FileHash"),
                TraceLoggingUInt32(BlSigningPolicy, "SigningPolicyCheckFlags")
                );
#endif
        } else if (EmbeddedSigned != FALSE) {
            *LoadInformation |= LOAD_INFORMATION_EMBEDDED_SIGNED;
        }
    }

    //
    // Logging the signature verification failure here
    //

#if !defined(MISC_MIN)
    if (!NT_SUCCESS(Status))
    {
        ImgpLogSignatureVerificationFailures(LoadFile,
            Status,
            &PolicyInfo,
            HashAlgId,
            HashBuffer);
    }
#endif

    return Status;
}

NTSTATUS
ImgpFilterValidationFailure (
    __in PWSTR LoadFile,
    __in NTSTATUS FailureStatus,
    __in ULONG ImageFlags
    )

/*++

Routine Description:

    This function enforces failure handling policy by optionally reporting or
    suppressing image hash validation failures.

Arguments:

    LoadFile - Supplies the name of the file that was found to be invalid.

    FailureStatus - Supplies the result status of the failed image validation
        operation.

    ImageFlags - Supplies configuration flags indicating the verification
        policy applied to this file.

Return Value:

    STATUS_SUCCESS is returned if the validation failure should be suppressed.

    The given original failure status is returned if the validation failure
        should be reported without suppression.

--*/

{

    ULONG ErrorIgnored;
    EVENT_DATA_DESCRIPTOR EventDataDescriptor[4];
    NTSTATUS Status;

    ErrorIgnored = 0;

    //
    // If the debugger is enabled and a valid hash is required for this image,
    // report the failure but allow the image to be loaded.
    //

    Status = FailureStatus;
    if (((FailureStatus == STATUS_INVALID_IMAGE_HASH) ||
         (FailureStatus == STATUS_IMAGE_CERT_EXPIRED)) &&
        CHECK_FLAG(ImageFlags, IMAGE_FLAGS_VALID_HASH_REQUIRED) &&
        (BlBdDebuggerEnabled() != FALSE)) {

        if (FailureStatus == STATUS_INVALID_IMAGE_HASH) {
            BlStatusPrint(L"*** Windows is unable to verify the signature of\n"
                          L"    the file %s.  It will be allowed to load\n"
                          L"    because the boot debugger is enabled.\n",
                          LoadFile);

        } else {

            NT_ASSERT(FailureStatus == STATUS_IMAGE_CERT_EXPIRED);

            BlStatusPrint(L"*** Certificate of the file %s has expired.\n"
                          L"    It will be allowed to load because the\n"
                          L"    boot debugger is enabled.\n",
                          LoadFile);
        }

        Status = STATUS_SUCCESS;
        ErrorIgnored = 1;
    }

    EventDataDescCreate(&EventDataDescriptor[0],
                        (PVOID)LoadFile,
                        (ULONG)((wcslen(LoadFile) + 1) * sizeof(WCHAR)));

    EventDataDescCreate(&EventDataDescriptor[1],
                        (PVOID)&ImageFlags,
                        sizeof(ULONG));

    EventDataDescCreate(&EventDataDescriptor[2],
                        (PVOID)&FailureStatus,
                        sizeof(NTSTATUS));

    EventDataDescCreate(&EventDataDescriptor[3],
                        (PVOID)&ErrorIgnored,
                        sizeof(ULONG));

    BlLogEtwWrite(&BOOTENV_ETW_PROVIDER,
                  &BOOT_IMAGE_INTEGRITY_CHECK,
                  NULL,
                  4,
                  EventDataDescriptor);

    return Status;
}

NTSTATUS
ImgpCloseFile (
    __in PIMG_FILE_HANDLE FileHandle
    )

/*++

Routine Description:

    This routine closes an image file.

Arguments:

    FileHandle - Supplies a pointer to an image file handle.

Return Value:

    NT status code.

--*/

{

    NTSTATUS Status;

    Status = STATUS_SUCCESS;
    if (FileHandle->Valid == 1) {
        if (FileHandle->InMemoryImage == 0) {
            Status = BlFileClose(FileHandle->FileSystem.FileId);

        } else {

            ASSERT(FileHandle->InMemory.FileBase != NULL);

            if (FileHandle->FreeOnClose == 1) {
                Status = BlMmFreePages(FileHandle->InMemory.FileBase);
            }
        }
    }

    return Status;
}

NTSTATUS
ImgpGetFileSize (
    __in PIMG_FILE_HANDLE FileHandle,
    __out PULONG FileSize
    )

/*++

Routine Description:

    This routine returns the size of a file.

Arguments:

    FileHandle - Supplies a pointer to an image file handle.

    FileSize - Supplies a pointer to a variable that receives the size of the
        file.

Return Value:

    NT status code.

--*/

{

    FILE_INFORMATION FileInformation;
    NTSTATUS Status;

    ASSERT(ARGUMENT_PRESENT(FileHandle) != FALSE);
    ASSERT(FileHandle->Valid == 1);
    ASSERT(ARGUMENT_PRESENT(FileSize) != FALSE);

    //
    // If the file is on the filesystem, return the filesize specified by the
    // filesystem.  Otherwise, return the size of the in memory buffer.
    //

    if (FileHandle->InMemoryImage == 0) {
        Status = BlFileGetInformation(FileHandle->FileSystem.FileId,
                                      &FileInformation);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        //
        // This module assumes a file is less than 4GB.  Return unsuccessfully
        // if the file is larger.
        //

        if (FileInformation.FileSize != (ULONG)FileInformation.FileSize) {
            return STATUS_NOT_SUPPORTED;
        }

        *FileSize = (ULONG)FileInformation.FileSize;
        Status = STATUS_SUCCESS;

    } else {

        ASSERT(FileHandle->InMemory.FileBase != NULL);

        *FileSize = FileHandle->InMemory.FileSize;
        Status = STATUS_SUCCESS;
    }

    return Status;
}

VOID
ImgpInitializeFileHandle (
    __out PIMG_FILE_HANDLE FileHandle
    )

/*++

Routine Description:

    This routine initializes an image file handle.

Arguments:

    FileHandle - Supplies a pointer to an image file handle.

Return Value:

    None.

--*/

{

    ASSERT(ARGUMENT_PRESENT(FileHandle) != FALSE);

    RtlZeroMemory(FileHandle, sizeof(IMG_FILE_HANDLE));
    return;
}

VOID
ImgpInitializeFileHandleForSourceBuffer (
    __out PIMG_FILE_HANDLE FileHandle,
    __in_bcount(SourceBufferSize) PVOID SourceBuffer,
    __in ULONG64 SourceBufferSize,
    __in_opt PWSTR FilePath
    )

/*++

Routine Description:

    This routine initializes an image file handle for an in-memory flat file.

Arguments:

    FileHandle - Supplies a pointer to an image file handle.

    SourceBuffer - Supplies a pointer to a flat file image loaded into memory.

    SourceBufferSize - Supplies the size of the source buffer.

    FilePath - Supplies an optional pointer to the name of the source file.

Return Value:

    None.

--*/

{

    ASSERT(ARGUMENT_PRESENT(FileHandle) != FALSE);

    RtlZeroMemory(FileHandle, sizeof(IMG_FILE_HANDLE));
    FileHandle->InMemoryImage = 1;
    FileHandle->InMemory.FileBase = SourceBuffer;
    FileHandle->InMemory.FileSize = (ULONG)SourceBufferSize;
    FileHandle->FilePath = FilePath;
    FileHandle->Valid = 1;
    return;
}

NTSTATUS
ImgpOpenFile (
    __in DEVICE_ID DeviceId,
    __in PWSTR FilePath,
    __in OPEN_FLAGS OpenFlags,
    __inout PIMG_FILE_HANDLE FileHandle
    )

/*++

Routine Description:

    This routine opens the specified file.  The file could be loaded from the
    debugger or from they filesystem.

Arguments:

    DeviceId - Supplies the device handle for the device containing the file
        to open.

    FilePath - Supplies the path of the file to open.

    OpenFlags - Supplies a bitmask containing flags to use when opening the
        file.

    FileHandle - Supplies a pointer to an image file handle.

Return Value:

    NT status code.

--*/

{

    FILE_ID FileId;
    PVOID RemoteDebuggerFileBase;
    ULONG64 RemoteDebuggerFileSize;
    NTSTATUS Status;

    ASSERT(ARGUMENT_PRESENT(FileHandle) != FALSE);

    //
    // Attempt to load the file over the debugger.  If this fails, either
    // due to a connection issue or because the file does not exist, load the
    // file off the specified device.
    //
    // Additionally fail the remote read if the file is larger than 4GB.  This
    // module has a 4GB limitation on all files.
    //

    Status = BlBdPullRemoteFile(FilePath,
                                &RemoteDebuggerFileBase,
                                &RemoteDebuggerFileSize);

    if (NT_SUCCESS(Status) &&
        (RemoteDebuggerFileSize == (ULONG)RemoteDebuggerFileSize)) {

        FileHandle->InMemoryImage = 1;
        FileHandle->InMemory.FileBase = RemoteDebuggerFileBase;
        FileHandle->InMemory.FileSize = (ULONG)RemoteDebuggerFileSize;
        FileHandle->FreeOnClose = 1;

    } else {
        Status = BlFileOpen(DeviceId, FilePath, OpenFlags, &FileId);
        if (!NT_SUCCESS(Status)) {
            goto OpenFileEnd;
        }

        FileHandle->InMemoryImage = 0;
        FileHandle->FileSystem.FileId = FileId;
    }

    FileHandle->FilePath = FilePath;
    FileHandle->Valid = 1;

OpenFileEnd:
    return Status;
}

NTSTATUS
ImgpReadAtFileOffset (
    __in PIMG_FILE_HANDLE FileHandle,
    __in ULONG Size,
    __in ULONGLONG ByteOffset,
    __out_bcount(Size) PVOID Buffer,
    __out_opt PULONG BytesRead,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine reads from a file at the specified offset.  The file can be
    in memory or on a filesystem.

Arguments:

    FileHandle - Supplies a pointer to an image file handle.

    Size - Supplies the number of bytes to read.

    ByteOffset - Supplies the file offset to start the read operation.

    Buffer - Supplies a buffer to receive the contents of the read operation.

    BytesRead - Supplies an optional pointer to a variable that receives the
        number of bytes read.

    Flags - Supplies the flags indicating how the file should be read.

Return Value:

    NT status code.

--*/

{

    NTSTATUS Status;

    ASSERT(ARGUMENT_PRESENT(FileHandle) != FALSE);
    ASSERT(FileHandle->Valid == 1);
    ASSERT(ARGUMENT_PRESENT(Buffer) != FALSE);

    //
    // If the caller specified a valid file ID, read the file from the file
    // system.  Otherwise, read the file from the in memory image.
    //

    if (FileHandle->InMemoryImage == 0) {
        Status = BlFileReadAtOffsetEx(FileHandle->FileSystem.FileId,
                                      Size,
                                      ByteOffset,
                                      Buffer,
                                      BytesRead,
                                      Flags);

    } else {

        ASSERT(FileHandle->InMemory.FileBase != NULL);

        if ((ByteOffset + Size) > FileHandle->InMemory.FileSize) {
            Status = STATUS_INVALID_PARAMETER;

        } else {
            RtlCopyMemory(Buffer,
                          Add2Ptr(FileHandle->InMemory.FileBase, ByteOffset),
                          Size);

            Status = STATUS_SUCCESS;
            if (ARGUMENT_PRESENT(BytesRead) != FALSE) {
                *BytesRead = Size;
            }
        }
    }

    return Status;
}

NTSTATUS
ImgpInitializeTrustedBootInformation (
    __out_opt PBOOT_TRUSTED_BOOT_INFORMATION BootTrustedBootInformation,
    __in PMINCRYPT_TRUSTEDBOOT_INFO MincryptTrustedBootInformation
    )

/*++

Routine Description:

    This routine initializes a boot environment trusted boot information
    structure by copying the contents from a mincrypt trusted boot information
    structure, allocating the necessary buffers.

Arguments:

    BootTrustedBootInformation - Supplies a pointer to a structure that
        optionally receives the trusted boot information.

    MincryptTrustedBootInformation - Supplies a pointer to a structure that
        contains the trusted boot information returned from the mincrypt
        library.

Return Value:

    NTSTATUS Code.

--*/

{

    PVOID IssuerModulus;
    ULONG IssuerModulusLength;
    ULONG NameLength;
    PVOID PublisherCertificate;
    ULONG PublisherCertificateLength;
    PVOID PublisherCertificateSerial;
    ULONG PublisherCertificateSerialLength;
    NTSTATUS Status;
    PVOID Thumbprint;
    ULONG ThumbprintLength;

    ASSERT(MincryptTrustedBootInformation != NULL);

    if (ARGUMENT_PRESENT(BootTrustedBootInformation) == FALSE) {
        return STATUS_SUCCESS;
    }

    RtlZeroMemory(BootTrustedBootInformation, sizeof(BootTrustedBootInformation));

    //
    // Copy the publisher thumbprint information if it exists.
    //

    ThumbprintLength =
        MincryptTrustedBootInformation->PublisherThumbprintLength;

    if (ThumbprintLength != 0) {
        Thumbprint = BlMmAllocateHeap(ThumbprintLength);
        if (Thumbprint == NULL) {
            Status = STATUS_NO_MEMORY;
            goto InitializeTrustedBootInformationEnd;
        }

        RtlCopyMemory(Thumbprint,
                      MincryptTrustedBootInformation->PublisherThumbprint,
                      ThumbprintLength);

        BootTrustedBootInformation->CertificateThumbprint = Thumbprint;
        BootTrustedBootInformation->CertificateThumbprintLength =
            ThumbprintLength;

        BootTrustedBootInformation->ThumbprintHashAlgorithm =
            MincryptTrustedBootInformation->PublisherThumbprintAlgorithm;
    }

    //
    // Copy the Issuer Common Name if it exists.
    //

    NameLength = MincryptTrustedBootInformation->IssuerCommonName.Length;
    if (NameLength != 0) {

        Status = MincryptStringToUnicodeString(
                    &BootTrustedBootInformation->CertificateIssuer,
                    &MincryptTrustedBootInformation->IssuerCommonName,
                    TRUE);

        if (!NT_SUCCESS(Status)) {
            goto InitializeTrustedBootInformationEnd;
        }
    }

    //
    // Copy the Publisher Common Name if it exists.
    //

    NameLength = MincryptTrustedBootInformation->PublisherCommonName.Length;
    if (NameLength != 0) {

        Status = MincryptStringToUnicodeString(
                    &BootTrustedBootInformation->CertificatePublisher,
                    &MincryptTrustedBootInformation->PublisherCommonName,
                    TRUE);

        if (!NT_SUCCESS(Status)) {
            goto InitializeTrustedBootInformationEnd;
        }
    }

    //
    // Copy the Issuer modulus
    //

    IssuerModulusLength = MincryptTrustedBootInformation->IssuerModulus.cbData;
    if (IssuerModulusLength != 0) {
        IssuerModulus = BlMmAllocateHeap(IssuerModulusLength);
        if (IssuerModulus == NULL) {
            Status = STATUS_NO_MEMORY;
            goto InitializeTrustedBootInformationEnd;
        }

        RtlCopyMemory(IssuerModulus,
                      MincryptTrustedBootInformation->IssuerModulus.pbData,
                      IssuerModulusLength);

        BootTrustedBootInformation->IssuerModulus = IssuerModulus;
        BootTrustedBootInformation->IssuerModulusLength = IssuerModulusLength;
    }

    //
    // Copy the Publisher Certificate Serial
    //

    PublisherCertificateSerialLength = MincryptTrustedBootInformation->PublisherCertificateSerial.cbData;
    if (PublisherCertificateSerialLength != 0) {
        PublisherCertificateSerial = BlMmAllocateHeap(PublisherCertificateSerialLength);
        if (PublisherCertificateSerial == NULL) {
            Status = STATUS_NO_MEMORY;
            goto InitializeTrustedBootInformationEnd;
        }

        RtlCopyMemory(PublisherCertificateSerial,
                      MincryptTrustedBootInformation->PublisherCertificateSerial.pbData,
                      PublisherCertificateSerialLength);

        BootTrustedBootInformation->PublisherCertificateSerial = PublisherCertificateSerial;
        BootTrustedBootInformation->PublisherCertificateSerialLength = PublisherCertificateSerialLength;
    }

    //
    // Copy the Publisher Certificate
    //

    PublisherCertificateLength = MincryptTrustedBootInformation->PublisherCertificate.cbData;
    if (PublisherCertificateLength != 0) {
        PublisherCertificate = BlMmAllocateHeap(PublisherCertificateLength);
        if (PublisherCertificate == NULL) {
            Status = STATUS_NO_MEMORY;
            goto InitializeTrustedBootInformationEnd;
        }

        RtlCopyMemory(PublisherCertificate,
                      MincryptTrustedBootInformation->PublisherCertificate.pbData,
                      PublisherCertificateLength);

        BootTrustedBootInformation->PublisherCertificate = PublisherCertificate;
        BootTrustedBootInformation->PublisherCertificateLength = PublisherCertificateLength;
    }

    Status = STATUS_SUCCESS;

InitializeTrustedBootInformationEnd:
    if (!NT_SUCCESS(Status)) {
        ImgpDestroyTrustedBootInformation(BootTrustedBootInformation);
    }

    return Status;
}

NTSTATUS
ImgpDestroyTrustedBootInformation (
    __in_opt PBOOT_TRUSTED_BOOT_INFORMATION BootTrustedBootInformation
    )

/*++

Routine Description:

    This routine destroy a boot environment trusted boot information structure.
    It frees any buffers allocated by the trusted boot initialization routine.

Arguments:

    BootTrustedBootInformation - Supplies a pointer the trusted boot information
        that is to be destroyed.

Return Value:

    NTSTATUS Code.

--*/

{

    if (ARGUMENT_PRESENT(BootTrustedBootInformation) == FALSE) {
        goto DestroyTrustedBootInformationEnd;
    }

    if (BootTrustedBootInformation->CertificateThumbprint != NULL) {
        BlMmFreeHeap(BootTrustedBootInformation->CertificateThumbprint);
    }

    RtlFreeUnicodeString(&BootTrustedBootInformation->CertificateIssuer);
    RtlFreeUnicodeString(&BootTrustedBootInformation->CertificatePublisher);

    if (BootTrustedBootInformation->IssuerModulus != NULL) {
        BlMmFreeHeap(BootTrustedBootInformation->IssuerModulus);
    }

    if (BootTrustedBootInformation->PublisherCertificateSerial != NULL) {
        BlMmFreeHeap(BootTrustedBootInformation->PublisherCertificateSerial);
    }

    if (BootTrustedBootInformation->PublisherCertificate != NULL) {
        BlMmFreeHeap(BootTrustedBootInformation->PublisherCertificate);
    }

DestroyTrustedBootInformationEnd:
    return STATUS_SUCCESS;
}

VOID
BlImgSetRestrictedSigning (
    __in BOOLEAN TrustTestRoot
    )

/*++

Routine Description:

    This routine sets restricted signing mode, forcing all images to be signed
    to the Microsoft 2010 root unless test-signing mode is on.

Arguments:

    TrustTestRoot - Supplies FALSE if the Microsoft test root should not be
         supported.

Return Value:

    None.

--*/

{
    BOOLEAN FlightSigning;

    BlImgAcceptedRootKeysMask = MINCRYPT_POLICY_PRSROOT_CURRENT;

#ifdef BUILD_FLIGHT_SIGNED
    BlImgAcceptedRootKeysMask |= MINCRYPT_POLICY_FLIGHTROOT;
#endif

    if (TrustTestRoot != FALSE) {
        BlImgAcceptedRootKeysMask |= MINCRYPT_POLICY_TESTROOT_CURRENT;
    }

    BlImgQueryCodeIntegrityBootOptions(BlGetApplicationEntry(),
                                       NULL,
                                       NULL,
                                       &FlightSigning);

    if (FlightSigning != FALSE) {
        BlImgAcceptedRootKeysMask |= MINCRYPT_POLICY_FLIGHTROOT;
    }

    //
    // If Code Integrity has already been initialized, and test signing is not
    // on, adjust to the restricted roots.
    //

    if (BlImgCodeIntegrityInitialized != FALSE &&
        BlImgTestSigningEnabled == FALSE) {

        BlImgAcceptedRootKeys = BlImgAcceptedRootKeysMask;

        if (CHECK_FLAG(BlImgAcceptedRootKeysMask, MINCRYPT_POLICY_FLIGHTROOT)) {
            MincrypL_SetFlightSigningOption(TRUE);
        }
    }
}

NTSTATUS
BlImgGetValidatedCertificateLocation (
    __in PIMAGE_NT_HEADERS NtHeader,
    __in_bcount(ImageSize) PVOID ImageBase,
    __in ULONG ImageSize,
    __deref_out PIMAGE_DATA_DIRECTORY *CertDirectory,
    __deref_opt_out_xcount_opt(CertDirectory->Size) PCHAR *CertAddress
    )

/*++

Routine Description:

    This routine returns the location of an Authenticode Attribute Certificate
    Table within a data mapped PE image. If the image does not contain a
    certificate, a null address is returned.

Arguments:

    NtHeader - Supplies a pointer to the pre-computed NtHeader within the
         mapped image.

    ImageBase - Supplies a pointer to the start of the mapped image.

    ImageSize - Supplies the length in bytes of the mapped image.

    CertDirectory - Supplies a pointer that receives the address of the
        NT header Certificate Directory within the mapped image.

    CertAddress - Supplies a pointer that receives the address of the
         certificate table within the mapped image.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_IMAGE_FORMAT if the buffer does not contain the whole NT
         header.

    STATUS_INVALID_IMAGE_HASH if the certificate table is not valid.

--*/

{

    ULONG_PTR CertEnd;
    ULONG_PTR ImageEnd;
    ULONG_PTR LocalCertAddress;
    PIMAGE_DATA_DIRECTORY LocalCertDirectory;
    ULONG_PTR LocalCertDirectoryEnd;
    NTSTATUS Status;

    //
    // Validate the image size.
    //

    Status = RtlULongPtrAdd((ULONG_PTR)ImageBase, ImageSize, &ImageEnd);
    if (!NT_SUCCESS(Status)) {
        goto GetValidatedCertificateLocationEnd;
    }

    //
    // Get the certificate location and validate it.
    //

    LocalCertDirectory = BlImgGetCertDirectory(NtHeader);
    Status = RtlULongPtrAdd((ULONG_PTR)LocalCertDirectory,
                            sizeof(IMAGE_DATA_DIRECTORY),
                            &LocalCertDirectoryEnd);

    if (!NT_SUCCESS(Status)) {
        goto GetValidatedCertificateLocationEnd;
    }

    if (LocalCertDirectoryEnd >= ImageEnd) {
        Status = STATUS_INVALID_IMAGE_FORMAT;
        goto GetValidatedCertificateLocationEnd;
    }

    if (LocalCertDirectory->VirtualAddress == 0 ||
        LocalCertDirectory->Size == 0) {

        if (ARGUMENT_PRESENT(CertAddress) != FALSE) {
            *CertAddress = 0;
        }

        *CertDirectory = LocalCertDirectory;
        goto GetValidatedCertificateLocationEnd;
    }

    Status = RtlULongPtrAdd((ULONG_PTR)ImageBase,
                            LocalCertDirectory->VirtualAddress,
                            &LocalCertAddress);

    if (!NT_SUCCESS(Status)) {
        goto GetValidatedCertificateLocationEnd;
    }

    Status = RtlULongPtrAdd(LocalCertAddress,
                            LocalCertDirectory->Size,
                            &CertEnd);

    if (!NT_SUCCESS(Status)) {
        goto GetValidatedCertificateLocationEnd;
    }

    if (CertEnd != ImageEnd) {
        Status = STATUS_INVALID_IMAGE_HASH;
        goto GetValidatedCertificateLocationEnd;
    }

    if (ARGUMENT_PRESENT(CertAddress) != FALSE) {
        *CertAddress = (PCHAR)LocalCertAddress;
    }

    *CertDirectory = LocalCertDirectory;

GetValidatedCertificateLocationEnd:
    return Status;
}

VOID
BlImgRevokePrereleaseSignersOnce (
    __in BOOLEAN AllowPrereleaseSigners
    )

/*++

Routine Description:

    This routine revokes the prerelease CAs on production systems. These CAs
    are not revoked if testsigning is enabled or a preproduction Secure Boot
    policy is active.

    Only the first call to this routine within a boot application takes effect.
    Subsequent calls return immediately.

Arguments:

    AllowPrereleaseSigners - Supplies FALSE if the prerelease CAs should be
         revoked and TRUE if they should not be revoked (and the caller simply
         wants to mark the first call).

Return Value:

    None.

--*/

{

    BOOLEAN TestSigningEnabled;
    BOOLEAN FlightSigningEnabled;
    BOOLEAN BuildFlightSigned = FALSE;

    if (BlImgRevokePrereleaseSignersOnceSet != FALSE) {
        return;
    }

    BlImgRevokePrereleaseSignersOnceSet = TRUE;

    if (AllowPrereleaseSigners != FALSE) {
        return;
    }

    //
    // Do not revoke the prerelease CA if testsigning is enabled or the policy
    // explicitly allows prerelease or the debug policy is present.
    //

    BlImgQueryCodeIntegrityBootOptions(BlGetApplicationEntry(),
                                       NULL,
                                       &TestSigningEnabled,
                                       &FlightSigningEnabled);

    if (TestSigningEnabled != FALSE ||
        BlSecureBootIsDebugPolicyActive() != FALSE ||
        BlSecureBootArePolicyOptionsSet(
            SECUREBOOT_POLICY_ALLOW_PRERELEASE_SIGNERS) != FALSE) {
        return;
    }

    //
    // Do not revoke the prerelease CA if secure-boot is not enabled, and
    // flight signing is allowed (flighting is on, or flight signed build).
    //

#ifdef BUILD_FLIGHT_SIGNED
    BuildFlightSigned = TRUE;
#endif

    if ((FlightSigningEnabled || BuildFlightSigned) &&
        BlSecureBootIsPolicyActive() == FALSE)
    {
        return;
    }

    MinCryptRevokePrereleaseWindowsPCA();
}

BOOLEAN
ImgpIsSigningPolicySet (
    VOID
    )

/*++

Routine Description:

    This routine determines if a signing policy has already been set.

Arguments:

    None.

Return Value:

    FALSE if no signing policy has been set.

--*/

{
    return BlImgSigners != NULL;
}

NTSTATUS
BlImgApplySigningPolicy (
    _In_ BOOLEAN TestsignedSupported,
    _In_ BOOLEAN TestsigningEnabled
    )

/*++

Routine Description:

    This routine checks if a Secure Boot policy is active and if so apply any
    Code Integrity signing configuration contained with it. If no Secure Boot
    policy is active, or the policy contains no signing configuration, the
    default policy will be used.

Arguments:

    TestsignedSupported - Supplies not FALSE if Microsoft test-signed binaries
         should be supported.

    TestsigningEnabled - Supplies not FALSE if policy to support the BCD
         testsigning should be read and applied.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_SECUREBOOT_INVALID_POLICY if a Secure Boot policy has invalid
         configuration.

--*/

{
    ULONG EkuCount;
    PCUCHAR EkuBuffer;
    ULONG EkuBufferSize;
    unsigned ForEachScenario;
    unsigned ForEachSigner;
    IMG_SIGNER_TYPE ForEachSignerType;
    ULONG HashAlgorithm;
    ULONG KnownRoot;
    UNICODE_STRING SignerKey;
    WCHAR SignerKeyBuffer[ARRAYSIZE(SECUREBOOT_POLICIES_KEY_SIGNING_SIGNERS L"\\####")];
    PIMG_SIGNER Signers;
    ULONG SignersCount;
    PCULONG SignersList;
    ULONG SignersListCount;
    int StringLength;
    NTSTATUS Status;
    ULONG TotalSigners;

    Signers = NULL;

    if (ImgpIsSigningPolicySet() != FALSE) {
        Status = STATUS_SUCCESS;
        goto BuildSigningPolicyEnd;
    }

    //
    // If no signers are specified in the policy use the default.
    //

    Status = BlSecureBootGetRegistryDWordInActivePolicy(
                SECUREBOOT_POLICIES_ROOT_KEY,
                &BlpImgPolicySignersKey,
                &BlpImgPolicySignersCount,
                &SignersCount);

    if (!NT_SUCCESS(Status) || SignersCount == 0) {
        ImgpSetDefaultSigningPolicy();
        Status = STATUS_SUCCESS;
        goto BuildSigningPolicyEnd;
    }

    //
    // Read in all the signers.
    //

    Signers = (PIMG_SIGNER)BlMmAllocateHeap(
                            SignersCount * sizeof(IMG_SIGNER));

    if (Signers == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BuildSigningPolicyEnd;
    }

    for (ForEachSigner = 0; ForEachSigner < SignersCount; ForEachSigner++) {
        StringLength = swprintf_s(
                            SignerKeyBuffer,
                            RTL_NUMBER_OF(SignerKeyBuffer),
                            SECUREBOOT_POLICIES_KEY_SIGNING_SIGNERS L"\\%u",
                            ForEachSigner);

        if (StringLength <= 0) {
            Status = STATUS_SECUREBOOT_INVALID_POLICY;
            goto BuildSigningPolicyEnd;
        }

        RtlInitUnicodeString(&SignerKey, SignerKeyBuffer);

        //
        // Either a known root or a TBS hash much be specified.
        //

        Status = BlSecureBootGetRegistryDWordInActivePolicy(
                    SECUREBOOT_POLICIES_ROOT_KEY,
                    &SignerKey,
                    &BlpImgPolicySignerKnownRoot,
                    &KnownRoot);

        if (NT_SUCCESS(Status)) {
            Signers[ForEachSigner].KnownRoot = KnownRoot;
            Signers[ForEachSigner].ToBeSignedHashLength = 0;
        }
        else {
            Signers[ForEachSigner].KnownRoot = MincryptKnownRootNone;

            Status = BlSecureBootGetRegistryBinaryInActivePolicy(
                        SECUREBOOT_POLICIES_ROOT_KEY,
                        &SignerKey,
                        &BlpImgPolicySignerToBeSignedHash,
                        &Signers[ForEachSigner].ToBeSignedHashLength,
                        &Signers[ForEachSigner].ToBeSignedHash);

            if (!NT_SUCCESS(Status)) {
                Status = STATUS_SECUREBOOT_INVALID_POLICY;
                goto BuildSigningPolicyEnd;
            }

            Status = BlSecureBootGetRegistryDWordInActivePolicy(
                        SECUREBOOT_POLICIES_ROOT_KEY,
                        &SignerKey,
                        &BlpImgPolicyHashAlgorithmString,
                        &HashAlgorithm);

            if (!NT_SUCCESS(Status)) {
                HashAlgorithm = SECUREBOOT_DEFAULT_TBS_HASH_ALGORITHM;
            }

            Signers[ForEachSigner].ToBeSignedHashAlgorithm = HashAlgorithm;
        }

        Status = BlSecureBootGetRegistryBinaryInActivePolicy(
                    SECUREBOOT_POLICIES_ROOT_KEY,
                    &SignerKey,
                    &BlpImgPolicySignerEkus,
                    &EkuBufferSize,
                    &EkuBuffer);

        EkuCount = 0;
        if (NT_SUCCESS(Status)) {
            EkuCount = ImgpValidatePolicyEkuBuffer(EkuBuffer, EkuBufferSize);

            if (EkuCount == 0) {
                Status = STATUS_SECUREBOOT_INVALID_POLICY;
                goto BuildSigningPolicyEnd;
            }

            //
            // Skip the first byte which is the EKU count, which we just
            // validated.
            //

            Signers[ForEachSigner].EkuBufferSize = EkuBufferSize - 1;
            Signers[ForEachSigner].EkuBuffer = EkuBuffer + 1;
        }

        Signers[ForEachSigner].EkuCount = EkuCount;
    }

    //
    // Decided what signer types we need to read from the scenarios.
    //

    if (TestsigningEnabled != FALSE) {
        BlImgSignerTypeMax = ImgSignerTypeTestSigning;
    }
    else if (TestsignedSupported != FALSE) {
        BlImgSignerTypeMax = ImgSignerTypeTest;
    }
    else {
        BlImgSignerTypeMax = ImgSignerTypeProduction;
    }

    //
    // For each scenario, read in the policy.
    //

    for (ForEachScenario = 0;
         ForEachScenario < ImgSigningScenarioMax;
         ForEachScenario++) {

        TotalSigners = 0;

        for (ForEachSignerType = ImgSignerTypeProduction;
             ForEachSignerType <= BlImgSignerTypeMax;
             ForEachSignerType++) {

            Status = BlSecureBootGetRegistryDWordChoiceInActivePolicy(
                        SECUREBOOT_POLICIES_ROOT_KEY,
                        &BlpImgScenarioPolicyKeys[ForEachScenario],
                        &BlpImgPolicySignerTypes[ForEachSignerType],
                        &SignersListCount,
                        &SignersList);

            if (NT_SUCCESS(Status)) {
                BlImgScenarios[ForEachScenario].Signers[ForEachSignerType] = SignersList;
                BlImgScenarios[ForEachScenario].SignersCount[ForEachSignerType] = SignersListCount;
                TotalSigners += SignersListCount;
            }
            else {
                BlImgScenarios[ForEachScenario].SignersCount[ForEachSignerType] = 0;
            }
        }

        //
        // Skip this scenario if no signers are specified.
        //

        if (TotalSigners == 0) {
            continue;
        }

        //
        // Get the minimum hash algorithm for this scenario.
        //

        Status = BlSecureBootGetRegistryDWordInActivePolicy(
                    SECUREBOOT_POLICIES_ROOT_KEY,
                    &BlpImgScenarioPolicyKeys[ForEachScenario],
                    &BlpImgPolicyHashAlgorithmString,
                    &HashAlgorithm);

        if (!NT_SUCCESS(Status)) {
            HashAlgorithm = SECUREBOOT_DEFAULT_MINIMUM_HASH_ALGORITHM;
        }

        BlImgScenarios[ForEachScenario].MinimumHashAlgorithm = HashAlgorithm;
    }

    Status = STATUS_SUCCESS;

    BlImgSignersCount = SignersCount;
    BlImgSigners = Signers;
    Signers = NULL;

BuildSigningPolicyEnd:
    if (Signers != NULL) {
        BlMmFreeHeap(Signers);
    }

    return Status;
}

ALG_ID
ImgpGetHashAlgorithmForScenario (
    _In_ IMG_SIGNING_SCENARIO_TYPE Scenario
    )

/*++

Routine Description:

    This routine returns the minimum hash algorithm required for a scenario.

Arguments:

    Scenario - Supplies the ID of the scenario to query.

Return Value:

    CAPI ALG_ID of the minimum hash algorithm for the scenario.

--*/

{
    ALG_ID Hash;
    BOOLEAN SecureBoot;
    NTSTATUS Status;

    if (ImgpIsSigningPolicySet() != FALSE) {
        return BlImgScenarios[Scenario].MinimumHashAlgorithm;
    }

    switch (Scenario) {
    case ImgSigningScenarioBootApplication:
        //
        // Legacy boot applications will be signed with an older cert chain, so
        // only enforce a higher algorithm if Secure Boot is enabled.
        //

        Status = BlSecureBootIsEnabled(&SecureBoot);
        if (NT_SUCCESS(Status) && SecureBoot != FALSE) {
            Hash = IMGLOAD_WINDOWS_COMPONENT_HASH_ALGORITHM;
        }
        else {
            Hash = BlImgMinimumHashAlgorithm;
        }
        break;

    case ImgSigningScenarioWindows:
        Hash = IMGLOAD_WINDOWS_COMPONENT_HASH_ALGORITHM;
        break;

    case ImgSigningScenarioHalExtension:
        Hash = IMGLOAD_HALEXTENSION_HASH_ALGORITHM;
        break;

    case ImgSigningScenarioElamDriver:
        Hash = IMGLOAD_ELAM_HASH_ALGORITHM;
        break;

    case ImgSigningScenarioBootRevocationList:
        Hash = IMGLOAD_WINDOWS_COMPONENT_HASH_ALGORITHM;
        break;

    case ImgSigningScenarioFirmwareUpdate:
        if (BlImgTestSigningEnabled == FALSE) {
            Hash = IMGLOAD_FIRMWARE_UPDATE_HASH_ALGORITHM;

        } else {
            Hash = BlImgMinimumHashAlgorithm;
        }
        break;

    case ImgSigningScenarioCoreExtension:
        Hash = IMGLOAD_COREEXTENSION_HASH_ALGORITHM;
        break;

    case ImgSigningScenarioDriver:
        __fallthrough;
    default:
        Hash = BlImgMinimumHashAlgorithm;
        break;
    }

    return Hash;
}

IMG_SIGNING_SCENARIO_TYPE
ImgpGetScenarioFromImageFlags (
    _In_ ULONG ImageFlags
    )

/*++

Routine Description:

    This routine decodes the IMAGE_FLAGS_* into a signing scenario type.

    N.B. For Core Extensions that are validated against HAL Extension
         certificate, this routine will return imaging scenario as HAL
         extension.

Arguments:

    ImageFlags - Supplies the IMAGE_FLAGS_* to decode the scenario from.

Return Value:

    The ID of the scenario.

--*/

{
    IMG_SIGNING_SCENARIO_TYPE Scenario;

    if (CHECK_FLAG(ImageFlags, IMAGE_FLAGS_BOOT_MUI) != FALSE) {
        Scenario = ImgSigningScenarioWindows;
    }
    else if (CHECK_FLAG(ImageFlags, IMAGE_FLAGS_BOOT_APPLICATION) != FALSE) {
        Scenario = ImgSigningScenarioBootApplication;
    }
    else if (CHECK_FLAG(ImageFlags, IMAGE_FLAGS_WINDOWS_COMPONENT) != FALSE) {
        Scenario = ImgSigningScenarioWindows;
    }
    else if (CHECK_FLAG(ImageFlags, IMAGE_FLAGS_EARLY_LAUNCH) != FALSE) {
        Scenario = ImgSigningScenarioElamDriver;
    }
    else if (CHECK_FLAG(ImageFlags, IMAGE_FLAGS_HAL_EXTENSION) != FALSE) {
        Scenario = ImgSigningScenarioHalExtension;
    }
    else if (CHECK_FLAG(ImageFlags, IMAGE_FLAGS_FIRMWARE_UPDATE) != FALSE) {
        Scenario = ImgSigningScenarioFirmwareUpdate;
    }
    else if (CHECK_FLAG(ImageFlags, IMAGE_FLAGS_CORE_EXTENSION) != FALSE) {
        Scenario = ImgSigningScenarioCoreExtension;
    }
    else {
        Scenario = ImgSigningScenarioDriver;
    }

    return Scenario;
}

NTSTATUS
BlSecurebootValidateSigningPolicy (
    _In_ IMG_SIGNING_SCENARIO_TYPE Scenario,
    _In_ PCMINCRYPT_CHAIN_INFO SigningInfo,
    _In_ BOOLEAN SkipCodeSigningEkuCheck,
    _Out_opt_ PULONG MatchingSigner
    )

/*++

Routine Description:

    This routine validates a certificate chain against the policy for a
    specific scenario.

Arguments:

    Scenario - Supplies the type of signing scenario to validate for.

    SignInfo - Supplies a pointer to the certificate chain information that is
        to being validated.

    SkipCodeSigningEkuCheck - Supplies FALSE if the chain should include the
        Code Signing EKU.

    Matching Signer - Supplies a pointer that optionally receives the index of
        the matching signer within the array of signers.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_IMAGE_HASH if the chain does not validate for the scenario.

    STATUS_SECUREBOOT_INVALID_POLICY if a Secure Boot policy has invalid
         configuration.

--*/

{
    ULONG Signer;
    IMG_SIGNER_TYPE SignerType;
    ULONG SelectedSigner;
    NTSTATUS Status;

    //
    // Verify that minimal code integrity initialization has completed
    // successfully.
    //

    Status = ImgpVerifyMinimalCodeIntegrityInitialization();
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Search for a matching signer in order of production, test and
    // testsigning as appropriate.
    //

    for (SignerType = ImgSignerTypeProduction;
         SignerType <= BlImgSignerTypeMax;
         SignerType++) {

        for (Signer = 0;
             Signer < BlImgScenarios[Scenario].SignersCount[SignerType];
             Signer++) {

            SelectedSigner = BlImgScenarios[Scenario].Signers[SignerType][Signer];
            if (SelectedSigner >= BlImgSignersCount) {
                return STATUS_SECUREBOOT_INVALID_POLICY;
            }

            if (ImgpValidateChainAgainstSigner(
                    SigningInfo,
                    &BlImgSigners[SelectedSigner],
                    SkipCodeSigningEkuCheck) != FALSE) {

                if (ARGUMENT_PRESENT(MatchingSigner) != FALSE) {
                    *MatchingSigner = SelectedSigner;
                }

                return STATUS_SUCCESS;
            }
        }
    }

    return STATUS_INVALID_IMAGE_HASH;
}

BOOLEAN
ImgpValidateChainAgainstSigner (
    _In_ PCMINCRYPT_CHAIN_INFO SigningInfo,
    _In_ PCIMG_SIGNER Signer,
    _In_ BOOLEAN SkipCodeSigningEkuCheck
    )

/*++

Routine Description:

    This routine validates a certificate chain against a specific signer.

Arguments:

    SignInfo - Supplies a pointer to the certificate chain information that is
        to being validated.

    Signer - Supplies a pointer to the signer to validate the chain against.

    SkipCodeSigningEkuCheck - Supplies FALSE if the chain should include the
        Code Signing EKU.

Return Value:

    FALSE if the chain does not validate against the signer.

--*/

{
    PCUCHAR Eku;
    ULONG EkuBufferLength;
    ULONG EkuCount;
    ULONG EkuSize;
    ULONG i;

    //
    // If the signer specified a root it must match.
    //

    if (Signer->KnownRoot != MincryptKnownRootNone &&
        Signer->KnownRoot != SigningInfo->KnownRoot) {
        return FALSE;
    }

    //
    // If the signer specifies EKUs they all must be in the chain.
    //

    if (Signer->EkuCount) {

        //
        // +1 as the Code Signing EKU is not defined in the policy but must
        // be present for all scenarios except those like revocation lists.
        //

        EkuCount = Signer->EkuCount;
        if (SkipCodeSigningEkuCheck == FALSE) {
            EkuCount++;
        }

        if (EkuCount > SigningInfo->cEKUs) {
            return FALSE;
        }

        Eku = Signer->EkuBuffer;
        EkuBufferLength = Signer->EkuBufferSize;

        while (EkuBufferLength > 0) {

            //
            // First byte is the length followed by the OID encoded EKU.
            //

            EkuSize = *Eku++;
            EkuBufferLength--;
            if (EkuSize > EkuBufferLength) {
                return FALSE;
            }

            for (i = 0; i < SigningInfo->cEKUs; i++) {

                //
                // +2 to skip over the 0x06 OID tag and length from Mincrypt.
                //

                if (SigningInfo->rgEKUs[i].cbData == (EkuSize + 2) &&
                    SigningInfo->rgEKUs[i].pbData[1] == EkuSize &&
                    RtlEqualMemory(SigningInfo->rgEKUs[i].pbData + 2,
                                   Eku,
                                   EkuSize) != FALSE) {
                    break;
                }
            }

            if (i >= SigningInfo->cEKUs) {
                return FALSE;
            }

            Eku += EkuSize;
            EkuBufferLength -= EkuSize;
        }
    }

    //
    // If the signer specifies a TBS hash, ensure there's a match.
    // Note that we start at the end of the TBS list as it is more likely that
    // a CA will be used to match than an end entity cert.
    //

    if (Signer->ToBeSignedHashLength) {
        for (i = SigningInfo->cSignerInfos; i > 0; i--) {

            if (SigningInfo->rgSignerInfos[i - 1].ToBeSignedHashLength ==
                    Signer->ToBeSignedHashLength &&
                SigningInfo->rgSignerInfos[i - 1].ToBeSignedHashAlgorithm ==
                    Signer->ToBeSignedHashAlgorithm &&
                RtlEqualMemory(SigningInfo->rgSignerInfos[i - 1].ToBeSignedHash,
                               Signer->ToBeSignedHash,
                               Signer->ToBeSignedHashLength) != FALSE) {
                return TRUE;
            }
        }

        return FALSE;
    }

    //
    // Everything matched.
    //

    return TRUE;
}

BOOLEAN
ImgpDoesScenarioTrustSigner (
    _In_ IMG_SIGNING_SCENARIO_TYPE Scenario,
    _In_ ULONG SignerToTrust
    )

/*++

Routine Description:

    This routine determines if a scenario trusts a specific signer.

Arguments:

    Scenario - Supplies the type of signing scenario.

    SignerToTrust - Supplies the signer ID to check against the scenario.

Return Value:

    FALSE if the scenario does not trust the signer.

--*/

{
    ULONG i;
    IMG_SIGNER_TYPE SignerType;

    //
    // Search for a matching signer in order of production, test and
    // testsigning as appropriate.
    //

    for (SignerType = ImgSignerTypeProduction;
         SignerType <= BlImgSignerTypeMax;
         SignerType++) {

        for (i = 0;
             i < BlImgScenarios[Scenario].SignersCount[SignerType];
             i++) {

            if (SignerToTrust == BlImgScenarios[Scenario].Signers[SignerType][i]) {
                return TRUE;
            }
        }
    }

    return FALSE;
}

NTSTATUS
ImgpValidateSigningPolicyAgainstAllScenarios (
    _In_ IMG_SIGNING_SCENARIO_TYPE PreferredScenario,
    _In_ PCMINCRYPT_CHAIN_INFO SigningInfo,
    _In_ BOOLEAN SkipCodeSigningEkuCheck,
    _Out_opt_ PULONG MatchingSigner
    )

/*++

Routine Description:

    This routine validates a certificate chain against the policy of all
    scenarios to determine if any scenario trusts the chain. The preferred
    scenario is tested first and if that does not succeed all remaining
    scenarios are tested.

Arguments:

    PreferredScenario - Supplies the preferred signing scenario to validate.

    SignInfo - Supplies a pointer to the certificate chain information that is
        to being validated.

    SkipCodeSigningEkuCheck - Supplies FALSE if the chain should include the
        Code Signing EKU.

    Matching Signer - Supplies a pointer that optionally receives the index of
        the matching signer within the array of signers.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_IMAGE_HASH if the chain does not validate for any scenario.

    STATUS_SECUREBOOT_INVALID_POLICY if a Secure Boot policy has invalid
         configuration.

--*/

{
    IMG_SIGNING_SCENARIO_TYPE Scenario;
    NTSTATUS Status;

    //
    // Check the preferred scenario and then the others if it doesn't validate.
    //

    Status = BlSecurebootValidateSigningPolicy(PreferredScenario,
                                               SigningInfo,
                                               SkipCodeSigningEkuCheck,
                                               MatchingSigner);

    if (NT_SUCCESS(Status)) {
        return Status;
    }

    for (Scenario = 0; Scenario < ImgSigningScenarioMax; Scenario++) {
        if (Scenario == PreferredScenario) {
            continue;
        }

        Status = BlSecurebootValidateSigningPolicy(Scenario,
                                                   SigningInfo,
                                                   SkipCodeSigningEkuCheck,
                                                   MatchingSigner);

        if (NT_SUCCESS(Status)) {
            break;
        }
    }

    return Status;
}

NTSTATUS
ImgpValidateImageHashWithCatalog (
    _In_opt_ PBOOT_CATALOG_ENTRY CatalogEntry,
    _In_ ALG_ID HashAlgId,
    _In_reads_bytes_(MAX_HASH_LEN) PCUCHAR HashBuffer,
    _In_ LOGICAL CheckRevocationList,
    _In_ IMG_SIGNING_SCENARIO_TYPE Scenario,
    _In_ ULONG AcceptedRootKeys,
    _Out_ PMINCRYPT_TRUSTEDBOOT_INFO TrustedBootInformation,
    _Out_opt_ PLARGE_INTEGER SigningTime,
    _Outptr_opt_ PMINCRYPT_POLICY_INFO* PolicyInfo
    )

/*++

Routine Description:

    This routine validates an image hash against the system (or specific)
    catalogs, and check that the catalog validates for the specified scenario.

Arguments:

    CatalogEntry - Supplies a pointer to the optional catalog entry to validate
        against. If the entry is not specified, all currently loaded catalogs
        will be checked.

    HashAlgId - Supplies the hash algorithm type of the image hash.

    HashBuffer - Supplies a pointer to the image hash.

    CheckRevocationList - Supplies FALSE if the image hash is not to be checked
        against the revocation list.

    Scenario - Supplies the type of signing scenario to validate for.

    AcceptedRootKeys - Supplies the list of root flags to validate against
         when classic signing enforcement is being used.

    TrustedBootInformation - Supplies a pointer that receives the Trusted Boot
         information about the validated image and catalog.

    SigningTime - Supplies an optional pointer to a variable that receives the
         time an image was signed.

    PolicyInfo - Supplies the pointer to existing PMINCRYPT_POLICY_INFO data structure.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_IMAGE_HASH if the image does not validate for the scenario.

--*/

{
    NTSTATUS Status;
    ULONG UserContext;

    Status = MinCrypL_CheckImageHash2(HashAlgId,
                                      HashBuffer,
                                      CheckRevocationList,
                                      CatalogEntry,
                                      TrustedBootInformation,
                                      &UserContext,
                                      SigningTime,
                                      PolicyInfo);

    if (!NT_SUCCESS(Status)) {
        goto ValidateImageHashWithCatalogEnd;
    }

    if (ImgpIsSigningPolicySet() == FALSE) {

        if (CHECK_FLAG(UserContext, AcceptedRootKeys) == FALSE) {
            Status = STATUS_INVALID_IMAGE_HASH;
        }
    }
    else if (ImgpDoesScenarioTrustSigner(Scenario, UserContext) == FALSE) {
        Status = STATUS_INVALID_IMAGE_HASH;
    }

ValidateImageHashWithCatalogEnd:
    return Status;
}

_Success_(return != 0)
ULONG
ImgpValidatePolicyEkuBuffer (
    _In_reads_bytes_(EkuBufferSize) PCUCHAR EkuBuffer,
    _In_ ULONG EkuBufferSize
    )

/*++

Routine Description:

    This routine validates the EKU definition for a policy signer.

Arguments:

    EkuBuffer - Supplies a pointer to a policy EKU definitions.

    EkuBufferSize - Supplies the size in bytes of the EKU buffer.

Return Value:

    The count of validated EKUs when successful.

    Zero if the EKU buffer is not valid.

--*/

{
    ULONG Ekus;
    ULONG EkuSize;
    ULONG EkusValidated;

    if (EkuBufferSize == 0) {
        return 0;
    }

    //
    // The first byte of the buffer is the EKU count.
    //

    EkuBufferSize--;
    Ekus = *EkuBuffer++;
    if (Ekus == 0) {
        return 0;
    }

    EkusValidated = 0;
    while (EkuBufferSize > 0) {
        EkuSize = *EkuBuffer++;
        EkuBufferSize--;

        if (EkuSize == 0 || EkuSize > IMGP_EKU_MAX_LEN ||
            EkuSize > EkuBufferSize) {
                return 0;
        }

        EkusValidated++;
        EkuBufferSize -= EkuSize;
        EkuBuffer += EkuSize;
    }

    if (EkusValidated != Ekus) {
        return 0;
    }

    return EkusValidated;
}

ULONG
BlImgGetSigningPolicy (
    VOID
    )

/*++

Routine Description:

    This routine gets the current signing policy.

Arguments:

    None.

Return Value:

    The current signing policy.

--*/

{
    return BlSigningPolicy;
}

VOID
BlImgSetSigningPolicy (
    _In_ ULONG SigningPolicy
    )

/*++

Routine Description:

    This routine supplies the signing policy that is to be enforced, as
    described by a set of IMAGE_SIGNING_POLICY_* flags.

Arguments:

    SigningPolicy - Supplies the signing policy that is required to be enforced.

Return Value:

    None.

--*/

{
    BlSigningPolicy = SigningPolicy;
}

VOID
ImgpSetDefaultSigningPolicy (
    VOID
    )

/*++

Routine Description:

    This routine sets the default signing policy for Code Integrity.

Arguments:

    None.

Return Value:

    None.

--*/

{
    ULONG SigningPolicy;

    SigningPolicy = BlImgGetSigningPolicy();

    BlImgSetSigningPolicy(SigningPolicy);
}

NTSTATUS
ImgpValidateDefaultSigningPolicy (
    _In_ IMG_SIGNING_SCENARIO_TYPE Scenario,
    _In_ PMINCRYPT_POLICY_INFO SigningInfo,
    _In_opt_ PLARGE_INTEGER SigningTime,
    _In_opt_ PCWSTR FileName,
    _Inout_opt_ PULONG LoadInformation
    )
/*++

Routine Description:

    This routine validates that a certificate chain is valid for a scenario.

Arguments:

    Scenario - Supplies the type of signing scenario to validate for.

    SigningInfo - Supplies a pointer to the certificate chain information that is
        being validated.

    SigningTime - Supplies an optional pointer to the signing time of the image.

    FileName - Supplies an optional pointer to Image FileName under verification.

   LoadInformation - Supplies an optional pointer to a variable that receives
        informational flags describing various aspects of the load. The
        following flags may be set.

        LOAD_INFORMATION_WHQL_EXEMPTED_FOR_BOOT_DRIVER : Indicates that the 
            image failed the WHQL requirement, but was allowed to load for
            interest of not bricking the machine.

        LOAD_INFORMATION_WHQL_EXEMPTED_BY_AUDIT_MODE : Indicates that the 
            image failed the WHQL requirement, but was allowed to load because
            SI Audit Mode is enabled.
            
Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_IMAGE_HASH if the signing information does not validate for
         the scenario.

--*/
{
    NTSTATUS Status;

#if !defined(MISC_MIN)
    BOOLEAN Exempted = FALSE;
    ULONG LoadInformationValue = 0;
    BOOLEAN WHQLCheckRequired = FALSE;
#else
    UNREFERENCED_PARAMETER(SigningInfo);
    UNREFERENCED_PARAMETER(SigningTime);
    UNREFERENCED_PARAMETER(FileName);
    UNREFERENCED_PARAMETER(LoadInformation);
#endif

    switch (Scenario) {

    case ImgSigningScenarioDriver:

        //
        // BootMgr does not load drivers.
        //

#if !defined(MISC_MIN)

        //
        // If WHQL is enforced for drivers also allow Windows signed drivers,
        // and kits as appropriate.
        //

        if (ARGUMENT_PRESENT(LoadInformation) == FALSE) {
            LoadInformation = &LoadInformationValue;
        }

        WHQLCheckRequired = ImgpIsWhqlSignatureRequired(SigningInfo,
                                                        SigningTime,
                                                        LoadInformation);

        if (WHQLCheckRequired != FALSE) {

            //
            // Check WHQL requirement.
            //

            Status = ImgpWhqlCheckHelper(SigningInfo);
            if (NT_SUCCESS(Status)) {
                //
                // If WHQL is met, clear exemption flags, if any, so that it
                // will not be logged.
                //

                CLEAR_FLAGS(*LoadInformation,
                            (LOAD_INFORMATION_WHQL_EXEMPTED_BY_AUDIT_MODE |
                             LOAD_INFORMATION_WHQL_EXEMPTED_FOR_BOOT_DRIVER));
            }
            else {
                //
                // Failed WHQL check, determine if exempted?
                //

                if (CHECK_FLAG(*LoadInformation,
                               (LOAD_INFORMATION_WHQL_EXEMPTED_BY_AUDIT_MODE |
                                LOAD_INFORMATION_WHQL_EXEMPTED_FOR_BOOT_DRIVER))) {
                    Exempted = TRUE;
                }
            }

            goto ValidateDefaultSigningPolicyEnd;
        }
#endif
        break;

    default:
        break;
    }

    Status = STATUS_SUCCESS;

#if !defined(MISC_MIN)
ValidateDefaultSigningPolicyEnd:

    if (Scenario == ImgSigningScenarioDriver) {

        //
        // This is called for any driver load scenario for revocation information telemetry
        //

        ImgpLogDriverLoadAudit(SigningInfo, TRUE, SigningTime, FileName);

        //
        // If we load the binary because WHQL is not enrofced, log if
        // binary will be blocked had WHQL been enabled.
        //

        if (NT_SUCCESS(Status)) {
            if (WHQLCheckRequired == FALSE &&
                !NT_SUCCESS(ImgpWhqlCheckHelper(SigningInfo))) {
                ImgpLogWHQLFailures(SigningInfo, SigningTime, FileName, NULL);
            }
        }
        else if (WHQLCheckRequired != FALSE) {

            //
            // Log an event for failed WHQL check.
            //

            ImgpLogWHQLFailures(SigningInfo, SigningTime, FileName, LoadInformation);

            //
            // If exempted, allow load.
            //

            if (Exempted != FALSE) {
                Status = STATUS_SUCCESS;
            }
        }
    }
#endif
    
    return Status;
}

#if !defined(MISC_MIN)
NTSTATUS ImgpEKUsCheckHelper (
    _In_ PMINCRYPT_POLICY_INFO SigningInfo,
    _In_reads_(EkuListCount) PSTR EkuAndList[],
    _In_reads_(EkuListCount) PSTR EkuOrList[],
    _In_ ULONG EkuListCount
    )
/*++

Routine Description:

    This is a helper method verify the policy EKUs using MinCrypt.

Arguments:

    SigningInfo - Supplies a pointer to the certificate chain information that is
        being validated.

    EkuAndList - Pointer to string array of EKUs to be used in AND rule.

    EkuOrList - Pointer to string array of EKUs to be used with EkuAndList 
        with OR condition.

    EkuListCount - Count of number of Policy EKUs to be validated using MinCrypt.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_IMAGE_HASH if the signing information does not validate for
        the scenario.

--*/
{
    ULONG i;
    NTSTATUS Status;
    
    Status = STATUS_SUCCESS;

    if (EkuListCount > 0) {
        for (i = 0; i < EkuListCount; i++) {
            if (MincryptIsEKUInPolicy(SigningInfo, EkuOrList[i]) != FALSE) {
                if (EkuAndList[i] == NULL ||
                    MincryptIsEKUInPolicy(SigningInfo, EkuAndList[i]) != FALSE) {

                    goto EKUsCheckHelperEnd;
                }
            }
        }

        Status = STATUS_INVALID_IMAGE_HASH;
    }

EKUsCheckHelperEnd:
    return Status;
}

NTSTATUS ImgpWhqlCheckHelper (
    _In_ PMINCRYPT_POLICY_INFO SigningInfo
    )
/*++

Routine Description:

    This is a helper method to handle necessary WHQL EKU checks and root specifically
    for ImgSigningScenarioDriver scenario.

Arguments:

    SigningInfo - Supplies a pointer to the certificate chain information that is
        being validated.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_IMAGE_HASH if the signing information does not validate for
        the scenario.

--*/
{
    ULONG AcceptedRoots;
    PSTR EkuOrList[3] = { 0 };
    PSTR EkuAndList[3] = { 0 };
    ULONG EkuListCount;
    NTSTATUS Status;

    Status = STATUS_INVALID_IMAGE_HASH;

    //
    // Determine roots trust.
    //

    AcceptedRoots = BlImgAcceptedRootKeys & IMGLOAD_ALL_MICROSOFT_ROOTS;

    if (!CHECK_FLAG(SigningInfo->ulPolicyBits, AcceptedRoots)) {
        goto WhqlCheckHelperEnd;
    }

    //
    // Check EKUs.
    //

    EkuOrList[0] = szOID_NT5_CRYPTO;
    EkuOrList[1] = szOID_WHQL_CRYPTO;

    if (BlImgSysDevEvWhqlSignedEnabled != FALSE) {
        EkuAndList[1] = szOID_EV_WHQL_CRYPTO;
    }

    EkuListCount = 2;

    if (CHECK_FLAG(BlSigningPolicy, IMAGE_SIGNING_POLICY_ALLOW_KITS_DRIVERS)) {
        EkuOrList[EkuListCount] = szOID_WINDOWS_KITS_SIGNING;
        EkuListCount++;
    }

    Status = ImgpEKUsCheckHelper(SigningInfo, EkuAndList, EkuOrList, EkuListCount);

WhqlCheckHelperEnd:
    return Status;
}


VOID
ImgpLogWHQLFailures(
    _In_ PMINCRYPT_POLICY_INFO SigningInfo,
    _In_opt_ PLARGE_INTEGER SigningTime,
    _In_opt_ PCWSTR FileName,
    _In_opt_ PULONG LoadInformation
)
/*++

Routine Description:

    This function logs a telemetry event on details about image which would
    fail on WHQL enforcement.

Arguments:

    SigningInfo - Supplies a pointer to the certificate chain information that is
        being validated.

    SigningTime - Supplies a pointer to the signing time of the image.
        Optional as the calling function has this as an optional param.

    FileName - Supplies a pointer to Image FileName under verification.
        Optional as the calling function has this as an optional param.

    LoadInformation - Supplies an optional pointer to flags used for exemption
        during WHQL enforcement.

        Value can be zero or one of these
            LOAD_INFORMATION_WHQL_EXEMPTED_BY_AUDIT_MODE
            LOAD_INFORMATION_WHQL_EXEMPTED_FOR_BOOT_DRIVER

Return Value:

    None.

--*/
{
    ULONG cbData;
    BOOLEAN Critical;
    PUCHAR pbData;
    LARGE_INTEGER SystemTime = { 0 };
    LARGE_INTEGER ZeroTime = { 0 };

    if (SigningInfo != NULL &&
        SigningInfo->pChainInfo != NULL &&
        SigningInfo->pChainInfo->cSignerInfos > 0)
    {
        //
        // Non-critical event by default.
        //

        Critical = FALSE;

        if (LoadInformation != NULL) {
            //
            // It will be a critical event if no exempt flag is set, or boot
            // driver exempt flag is set.
            //

            if (((*LoadInformation & 
                    LOAD_INFORMATION_WHQL_EXEMPTED_FOR_BOOT_DRIVER) != 0) ||
                ((*LoadInformation & 
                    LOAD_INFORMATION_WHQL_EXEMPTED_BY_AUDIT_MODE) == 0)) {
                Critical = TRUE;
            }
        }

        cbData = SigningInfo->pChainInfo->rgSignerInfos[0].EncodedCertificate.cbData;
        pbData = SigningInfo->pChainInfo->rgSignerInfos[0].EncodedCertificate.pbData;

        ImgpQuerySystemTime(&SystemTime);

        if (SigningTime == NULL)
        {
            SigningTime = &ZeroTime;
        }

        if (Critical != FALSE)
        {
            TraceLoggingWrite(g_CiTraceLoggingProvider,
                "CodeIntegrityWHQLDriverFailure",
                TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES),
                TraceLoggingWideString((FileName ? FileName : L""), "FileName"),
                TraceLoggingString((FileName ? "Embedded" : "Catalog"), "VerificationFileType"),
                TraceLoggingHexUInt32(SigningInfo->VerificationStatus, "VerificationStatus"),
                TraceLoggingHexUInt32(SigningInfo->ulPolicyBits, "RootPolicyBits"),
                TraceLoggingHexUInt32(*LoadInformation, "LoadInformation"),
                TraceLoggingFileTime(*((PFILETIME)SigningTime), "SigningTime"),
                TraceLoggingFileTime(*((PFILETIME)(&SystemTime)), "CurrentSystemTime"),
                TraceLoggingUInt8Array(pbData, (UINT16)cbData, "EncodedCertBlob")
                );
        }
        else
        {
            TraceLoggingWrite(g_CiTraceLoggingProvider,
                "CodeIntegrityWHQLDriverAuditFailure",
                TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES),
                TraceLoggingWideString((FileName ? FileName : L""), "FileName"),
                TraceLoggingString((FileName ? "Embedded" : "Catalog"), "VerificationFileType"),
                TraceLoggingHexUInt32(SigningInfo->VerificationStatus, "VerificationStatus"),
                TraceLoggingHexUInt32(SigningInfo->ulPolicyBits, "RootPolicyBits"),
                TraceLoggingFileTime(*((PFILETIME)SigningTime), "SigningTime"),
                TraceLoggingFileTime(*((PFILETIME)(&SystemTime)), "CurrentSystemTime"),
                TraceLoggingUInt8Array(pbData, (UINT16)cbData, "EncodedCertBlob")
                );
        }
    }
}

BOOLEAN 
ImgpPseudoRandomLog(
    _In_opt_ PLARGE_INTEGER SystemTime,
    _In_opt_ PCWSTR FileName
    )
/*++

Routine Description:

    This is a helper function to determine if logging should be done or not, 
    based on even distribution of pseudo-random value created using OS time and FileName. 
    For system clock skews system time lower than Jan, 01 2016 00:00 GMT are excluded.

Arguments:

    SystemTime - Supplies a pointer to the current system time.
        Optional as the calling function has this as an optional param.

    FileName - Supplies a pointer to Image FileName under verification.
        Optional as the calling function has this as an optional param.

Return Value:

    FALSE (zero) by default
    TRUE (non zero) only if current system year is post 2016 and last 4 bits of hash are zero.

--*/
{
    ULONG FileLen;
    ALG_ID HashAlgorithm;
    UCHAR HashBuffer[MAX_HASH_LEN];
    PCRYPTO_HASH_CONTEXT HashContext;
    ULONG HashSize;
    UCHAR LastByte;
    BOOLEAN RetVal;
    NTSTATUS Status;

    FileLen = 0;
    HashAlgorithm = CALG_SHA_256;
    HashContext = NULL;
    HashSize = 0;
    RetVal = FALSE;
    Status = STATUS_SUCCESS;

    //
    // Log only if current system time is post Jan 01 2016, 00:00 GMT
    //

    if (SystemTime == NULL || FileName == NULL || SystemTime->QuadPart < IMGP_MIN_SYSTEM_TIME_FOR_AUDIT_EVENT) {
        goto ImgpPseudoRandomLogEnd;
    }

    //
    // Do not send telemetry events for Catalog files
    //

    FileLen = wcslen(FileName);
    if (FileLen > 4 && _wcsicmp(FileName + FileLen - 4, L".cat") == 0) {
        goto ImgpPseudoRandomLogEnd;
    }

    Status = BlCryptoHashInitialize(HashAlgorithm, &HashContext, &HashSize);
    if (!NT_SUCCESS(Status) || HashSize == 0) {
        goto ImgpPseudoRandomLogEnd;
    }

    Status = BlCryptoHashUpdate(HashContext,
                                (PCUCHAR) FileName,
                                (ULONG)((FileLen + 1) * sizeof(WCHAR)));

    if (!NT_SUCCESS(Status)) {
        goto ImgpPseudoRandomLogEnd;
    }

    Status = BlCryptoHashUpdate(HashContext,
                                (PCUCHAR) SystemTime,
                                (ULONG)(sizeof(LARGE_INTEGER)));

    if (!NT_SUCCESS(Status)) {
        goto ImgpPseudoRandomLogEnd;
    }

    Status = BlCryptoHashFinalize(HashContext, HashBuffer);
    if (!NT_SUCCESS(Status)) {
        goto ImgpPseudoRandomLogEnd;
    }

    LastByte = HashBuffer[HashSize - 1];
    if ((LastByte & 0x0F) == 0) {
        RetVal = TRUE;
    }

ImgpPseudoRandomLogEnd:
    if (HashContext != NULL) {
        BlCryptoHashDestroy(HashContext);
    }
    return RetVal;
}

VOID
ImgpLogDriverLoadAudit(
    _In_opt_ PMINCRYPT_POLICY_INFO SigningInfo,
    _In_ BOOLEAN IsEmbeddedSigned,
    _In_opt_ PLARGE_INTEGER SigningTime,
    _In_opt_ PCWSTR FileName
    )
/*++

Routine Description:

    This function logs a telemetry event on details about any successful driver load
    validated through WinLoad.

Arguments:

    SigningInfo - Supplies a pointer to the certificate chain information that is
        being validated, this is Optional in case of unsigned drivers.

    IsEmbeddedSigned - Supplies boolean indicator if its embedded signed or not
        TRUE(non-zero) indicates Embedded signed.

    SigningTime - Supplies a pointer to the signing time of the image.
        Optional as the calling function has this as an optional param.

    FileName - Supplies a pointer to Image FileName under verification.
        Optional as the calling function has this as an optional param.



Return Value:

    None.

    --*/
{
    ULONG cbData;
    PUCHAR pbData;
    LARGE_INTEGER SystemTime = { 0 };
    LARGE_INTEGER ZeroTime = { 0 };
    ULONG AcceptedRoots;
    ALG_ID HashAlgID;

    ImgpQuerySystemTime(&SystemTime);

    if (SigningInfo != NULL &&
        SigningInfo->pChainInfo != NULL &&
        SigningInfo->pChainInfo->cSignerInfos > 0)
    {
        if (BlImgNumEventsFired < IMGP_MAX_AUDIT_EVENTS_PER_BOOT &&
            ImgpPseudoRandomLog(&SystemTime, FileName) != FALSE)
        {

            cbData = SigningInfo->pChainInfo->rgSignerInfos[0].EncodedCertificate.cbData;
            pbData = SigningInfo->pChainInfo->rgSignerInfos[0].EncodedCertificate.pbData;
            HashAlgID = SigningInfo->pChainInfo->rgSignerInfos[0].ToBeSignedHashAlgorithm;
            BlImgNumEventsFired++;

            if (SigningTime == NULL)
            {
                SigningTime = &ZeroTime;
            }

            AcceptedRoots = BlImgAcceptedRootKeys & IMGLOAD_ALL_MICROSOFT_ROOTS;

            if (!CHECK_FLAG(SigningInfo->ulPolicyBits, AcceptedRoots)) 
            {
                TraceLoggingWrite(g_CiAuditTraceLoggingProvider,
                    "ThirdPartySignedWinLoadDriverLoadSuccessAudit",
                    TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES),
                    TraceLoggingWideString((FileName ? FileName : L""), "FileName"),
                    TraceLoggingString((IsEmbeddedSigned ? "Embedded" : "Catalog"), "VerificationFileType"),
                    TraceLoggingHexUInt32(SigningInfo->VerificationStatus, "VerificationStatus"),
                    TraceLoggingHexUInt32(SigningInfo->ulPolicyBits, "RootPolicyBits"),
                    TraceLoggingFileTime(*((PFILETIME)SigningTime), "SigningTime"),
                    TraceLoggingFileTime(*((PFILETIME)(&SystemTime)), "CurrentSystemTime"),
                    TraceLoggingUInt32(HashAlgID, "TBSHashAlgID"),
                    TraceLoggingUInt8Array(SigningInfo->pChainInfo->rgSignerInfos[0].ToBeSignedHash,
                        (UINT16)SigningInfo->pChainInfo->rgSignerInfos[0].ToBeSignedHashLength, "TBSHash"),
                    TraceLoggingUInt8Array(pbData, (UINT16)cbData, "EncodedCertBlob")
                    );
            }
            else
            {
                TraceLoggingWrite(g_CiAuditTraceLoggingProvider,
                    "MSSignedWinLoadDriverLoadSuccessAudit",
                    TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES),
                    TraceLoggingWideString((FileName ? FileName : L""), "FileName"),
                    TraceLoggingString((IsEmbeddedSigned ? "Embedded" : "Catalog"), "VerificationFileType"),
                    TraceLoggingHexUInt32(SigningInfo->VerificationStatus, "VerificationStatus"),
                    TraceLoggingHexUInt32(SigningInfo->ulPolicyBits, "RootPolicyBits"),
                    TraceLoggingFileTime(*((PFILETIME)SigningTime), "SigningTime"),
                    TraceLoggingFileTime(*((PFILETIME)(&SystemTime)), "CurrentSystemTime"),
                    TraceLoggingUInt32(HashAlgID, "TBSHashAlgID"),
                    TraceLoggingUInt8Array(SigningInfo->pChainInfo->rgSignerInfos[0].ToBeSignedHash,
                        (UINT16)SigningInfo->pChainInfo->rgSignerInfos[0].ToBeSignedHashLength, "TBSHash"),
                    TraceLoggingCharArray(SigningInfo->pChainInfo->rgSignerInfos[0].IssuerCommonName.Buffer,
                        (UINT16) SigningInfo->pChainInfo->rgSignerInfos[0].IssuerCommonName.Length, "IssuerCommonName"),
                    TraceLoggingCharArray(SigningInfo->pChainInfo->rgSignerInfos[0].PublisherCommonName.Buffer,
                        (UINT16)SigningInfo->pChainInfo->rgSignerInfos[0].PublisherCommonName.Length, "PublisherCommonName")
                    );
            }
        }
    }
    else
    {
        TraceLoggingWrite(g_CiAuditTraceLoggingProvider,
            "UnsignedWinLoadDriverLoadSuccessAudit",
            TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES),
            TraceLoggingWideString((FileName ? FileName : L""), "FileName"),
            TraceLoggingString((IsEmbeddedSigned ? "Embedded" : "Catalog"), "VerificationFileType")
            );
    }
}

VOID
ImgpLogSignatureVerificationFailures(
    _In_ PWSTR LoadFile,
    _In_ NTSTATUS StatusCode,
    _In_ PMINCRYPT_POLICY_INFO PolicyInfo,
    _In_ ALG_ID HashAlgId,
    _In_reads_bytes_(MAX_HASH_LEN) UCHAR HashBuffer[MAX_HASH_LEN]
    )

/*++

Routine Description:

    This function logs a telemetry event when an image signature validation fails.

Arguments:

    FileName - Supplies a pointer to the name of the image causing the event.

    StatusCode - Supplies the error code returned by Mincrypt.

    pPolicyInfo - Supplies the mincrypt policy info containing signing cert life time,
        flags and other fields

    HashAlgId - Supplies the Hash Algorithm, used for verification.

    HashBuffer - Buffer containing the hash, used for verification.

Return Value:

    None.

--*/

{
    LARGE_INTEGER SystemTime = { 0 };

    TraceLoggingWrite(g_CiTraceLoggingProvider,
        "CodeIntegrityImageSignatureVerificationFailed",
        TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES),
        TraceLoggingWideString(LoadFile, "FileName"),
        TraceLoggingHexInt32(StatusCode, "StatusCode"),
        TraceLoggingUInt32(HashAlgId, "HashAlgId"),
        TraceLoggingHexUInt8Array(HashBuffer, (UINT16)MAX_HASH_LEN, "FileHash")
        );

    if (StatusCode == STATUS_IMAGE_CERT_EXPIRED)
    {
        //
        // Get system time
        //

        ImgpQuerySystemTime(&SystemTime);

        TraceLoggingWrite(
            g_CiTraceLoggingProvider,
            "CodeIntegrityBinarySignatureExpired",
            TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES),
            TraceLoggingWideString(LoadFile, "FileName"),
            TraceLoggingFileTime(*((PFILETIME)(&PolicyInfo->NotValidBefore)), "ValidFrom"),
            TraceLoggingFileTime(*((PFILETIME)(&PolicyInfo->NotValidAfter)), "ValidTo"),
            TraceLoggingFileTime(*((PFILETIME)(&SystemTime)), "CurrentSystemTime"));
    }
}
#endif

NTSTATUS
BlImgTrustFlightSigning(
    VOID
    )

/*++

Routine Description:

    This routine adds trust for flight signing to the trusted roots.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.

--*/

{
    NTSTATUS Status;

    Status = ImgpVerifyMinimalCodeIntegrityInitialization();
    if (!NT_SUCCESS(Status)) {
        goto ImgTrustFlightSigningEnd;
    }

    BlImgAcceptedRootKeys |= MINCRYPT_POLICY_FLIGHTROOT;
    BlImgAcceptedRootKeysMask |= MINCRYPT_POLICY_FLIGHTROOT;

    MincrypL_SetFlightSigningOption(TRUE);

ImgTrustFlightSigningEnd:
    return Status;
}

#if !defined(MISC_MIN)

NTSTATUS
ImgpSetSysDevWhqlPolicy (
    _In_ SYSTEM_INTEGRITY_POLICY_TYPE PolicyType
    )
/*++

Routine Description:

    This routine sets the SysDev WHQL signing policy per setting of the
    ConfigCI policy type.

Arguments:

    PolicyType - Supplies the ConfigCI policy type.

Return Value:

    STATUS_SUCCESS when successful.

--*/
{
    BOOLEAN AuditEnabled;
    SYSTEM_INTEGRITY_POLICY_HANDLE PolicyHandle;
    NTSTATUS Status;

    if (SIPolicyIsPolicyActive(PolicyType, &PolicyHandle) != FALSE) {
        Status = SIPolicyAuditModeEnabled(PolicyHandle, &AuditEnabled);
        if (!NT_SUCCESS(Status)) {
            goto ImgpSetSysDevWhqlPolicyEnd;
        }

        if (AuditEnabled) {
            BlImgSysDevWhqlAuditModeEnabled = TRUE;
        }

        if (SIPolicySysDevWhqlEnabled(PolicyHandle) != FALSE) {
            BlImgSysDevWhqlSignedEnabled = TRUE;
        }

        if (SIPolicySysDevEvWhqlEnabled(PolicyHandle) != FALSE) {
            BlImgSysDevEvWhqlSignedEnabled = TRUE;
        }
    }

    Status = STATUS_SUCCESS;

ImgpSetSysDevWhqlPolicyEnd:

    return Status;
}

NTSTATUS
BlImgSetSysDevWhqlPolicy (
    _In_ BOOLEAN IsSystemUpgradeInProgress,
    _In_ BOOLEAN IsWinPE,
    _In_ ULONG Settings,
    _In_opt_ PLARGE_INTEGER EnforceDateTime
    )
/*++

Routine Description:

    This routine sets the SysDev WHQL signing policy.

Arguments:

    IsSystemUpgradeInProgress - Supplies the boolean to indicate if the system
        is in the prcoess of upgrading.
        
    IsWinPE - Supplies the boolean to indicate if the system is WinPE.
    
    Settings - Supplies the settings.

    EnforceDateTime - Supplies the optional date/time to enforce SysDev
        WHQL signing policy.

Return Value:

    STATUS_SUCCESS when successful.

--*/
{
    NTSTATUS Status;

    //
    // System upgrade in progress.
    //

    BlImgSystemUpgradeInProgress = IsSystemUpgradeInProgress;

    //
    // Win PE.
    //

    BlImgWinPE = IsWinPE;

    //
    // Settings.
    //

    BlImgWhqlSettings = Settings;

    //
    // Can only overwrite the default if earlier.
    //

    if (EnforceDateTime != NULL &&
        EnforceDateTime->QuadPart < BlImgSysDevWhqlEnforcementDateTime.QuadPart) {
        BlImgSysDevWhqlEnforcementDateTime = *EnforceDateTime;
    }

    //
    // Set according to Config CI policies.
    //

    Status = ImgpSetSysDevWhqlPolicy(SiPolicyTypeCi);
    if (!NT_SUCCESS(Status)) {
        goto BlImgSetSysDevWhqlPolicyEnd;
    }

    Status = ImgpSetSysDevWhqlPolicy(SiPolicyTypeSku);
    if (!NT_SUCCESS(Status)) {
        goto BlImgSetSysDevWhqlPolicyEnd;
    }

BlImgSetSysDevWhqlPolicyEnd:

    return Status;
}

BOOLEAN
BlImgIsWinPE (
    VOID
    )
/*++

Routine Description:

    This routine checks if the system is WInPE.

Arguments:

    None.
    
Return Value:

    TRUE if WinPE, else FALSE.

--*/
{
    return BlImgWinPE;
}

BOOLEAN
BlImgIsUpgradedPlatform (
    VOID
    )
/*++

Routine Description:

    This routine checks if the system is upgraded from pre Windows 10.

Arguments:

    None.
    
Return Value:

    TRUE if upgraded from pre Windows 10, else FALSE.

--*/
{
    return CHECK_FLAG(BlImgWhqlSettings,
                      CI_WHQL_REG_SETTINGS_UPGRADED_SYSTEM) != FALSE;
}

BOOLEAN
BlImgIsWhqlDeveloperTestModeEnabled (
    VOID
    )
/*++

Routine Description:

    This routine checks if the WHQL reg policy has the WhqlDeveloperTestMode
    value set to 1.

Arguments:

    None.
    
Return Value:

    TRUE if set, else FALSE.

--*/
{
    return CHECK_FLAG(BlImgWhqlSettings,
                      CI_WHQL_REG_DEVELOPER_TEST_MODE_ENABLED) != FALSE;
}

BOOLEAN
BlImgIsWhqlDisabledBySetting (
    VOID
    )
/*++

Routine Description:

    This routine checks if the WHQL reg policy has the DISABLE setting set.

Arguments:

    None.
    
Return Value:

    TRUE if set, else FALSE.

--*/
{
    return CHECK_FLAG(BlImgWhqlSettings,
                      CI_WHQL_REG_SETTINGS_DISABLE_ENFORCEMENT) != FALSE;
}

BOOLEAN
BlImgIsWhqlEnabledBySetting (
    VOID
    )
/*++

Routine Description:

    This routine checks if the WHQL reg policy has the ENABLE setting set.

Arguments:

    None.
    
Return Value:

    TRUE if set, else FALSE.

--*/
{
    return CHECK_FLAG(BlImgWhqlSettings,
                      CI_WHQL_REG_SETTINGS_ENABLE_ENFORCEMENT) != FALSE;
}

BOOLEAN
ImgpIsWhqlPolicyEnabled (
    VOID
    )
/*++

Routine Description:

    This routine checks if WHQL signature policy is enabled.

Arguments:

    None.
    
Return Value:

    TRUE if WHQL signature policy is enabled, else FALSE.

--*/
{
    //
    // WHQL policy enabled in SBCP or Config CI?
    //

    if (BlImgSysDevWhqlSignedEnabled != FALSE ||
        BlImgSysDevEvWhqlSignedEnabled != FALSE) {
        return TRUE;
    }

    return FALSE;
}

BOOLEAN
ImgpIsWhqlSignatureRequired (
    _In_ PMINCRYPT_POLICY_INFO SigningInfo,
    _In_opt_ PLARGE_INTEGER SigningTime,
    _Inout_ PULONG LoadInformation
    )
/*++

Routine Description:

    This routine checks if WHQL signature is required.

Arguments:

    SigningInfo - Supplies a pointer to the certificate chain information that is
        being validated.

    SigningTime - Supplies an optional pointer to the signing time of the image.

   LoadInformation - Supplies a pointer to a variable that receives
        informational flags describing if the load can be exmpted from WHQL
        requirements for various reason.

        LOAD_INFORMATION_WHQL_EXEMPTED_FOR_BOOT_DRIVER : Indicates that the 
            image failed the WHQL requirement, but was allowed to load for
            interest of not bricking the machine.

        LOAD_INFORMATION_WHQL_EXEMPTED_BY_AUDIT_MODE : Indicates that the 
            image failed the WHQL requirement, but was allowed to load because
            SI Audit Mode is enabled.

Return Value:

    TRUE if WHQL signature is required, else FALSE.

--*/
{
    BOOLEAN SecureBootEnabled;
    NTSTATUS Status;

    //
    // If /Testsigning is on, always do not enforce WHQL.
    //

    if (BlImgTestSigningEnabled != FALSE) {
        return FALSE;
    }

    //
    // If SBCP policy is in place, enforce WHQL requirement.
    //

    if (CHECK_FLAG(BlSigningPolicy,
                   IMAGE_SIGNING_POLICY_ENFORCE_WHQL_SIGNED_DRIVERS)) {
        return TRUE;
    }

    //
    // If SI policy in place, enforce WHQL, but if Audit Mode is
    // enabled, indicate as such.
    //

    if (ImgpIsWhqlPolicyEnabled() != FALSE) {
        if (BlImgSysDevWhqlAuditModeEnabled != FALSE) {
            *LoadInformation |= LOAD_INFORMATION_WHQL_EXEMPTED_BY_AUDIT_MODE;
        }

        return TRUE;
    }

    //
    // If we are in the process of upgrading (setup.exe is running),
    // do not enforce WHQL requirement.
    //

    if (BlImgSystemUpgradeInProgress != FALSE) {
        return FALSE;
    }

    //
    // If WinPE, do not enforce WHQL requirement.
    //

    if (BlImgIsWinPE() != FALSE) {
        return FALSE;
    }

    //
    // If SecureBoot is off, we also do no enforce WHQL requirement.
    //

    Status = BlSecureBootIsEnabled(&SecureBootEnabled);
    if (!NT_SUCCESS(Status) || SecureBootEnabled == FALSE) {
        return FALSE;
    }

    //
    // To ease OEM driver development, OEM can disable WHQL enforcement.
    // This takes precedent over SETTINGS values.
    //

    if (BlImgIsWhqlDeveloperTestModeEnabled() != FALSE) {
        return FALSE;
    }

    //
    // Also for compatibility reason to not bricking machine upgraded
    // from pre Windows 10, skip the WHQL enforcement.
    //
    // This is to address the scenario where a pre Windows 10 (RS1) machine
    // with driver signed with timestamp after the enforcement date. In pre
    // Windows 10, the driver would be loaded just fine, but will fail to load
    // in Windows 10 if WHQL is enforced.
    //
    // In other words, we only enforce WHQL policy if machine is clean
    // installed with Windows 10 and above with no policy in place.
    //
    // Additionally, reg settings can enable/disable WHQL enforcement, which
    // IoT system will utilize.
    //

    if ((BlImgIsUpgradedPlatform() != FALSE ||
         BlImgIsWhqlDisabledBySetting() != FALSE) &&
        BlImgIsWhqlEnabledBySetting() == FALSE) {
        return FALSE;
    }

    //
    // /Testsigning is off, no policy in place (SBCP or Config CI), not WinPE,
    // Secure Boot is off, and not upgraded from pre Windows 10, so the default
    // is to require WHQL, except if signed before the enforcement date/time.
    //

#ifdef IMGLOAD_WHQL_ENABLE_TIMESTAMP_CHECK
        //
        // DSIE: If not enabled, we will always use the NotBefore issuance
        //       time to match our external communication.
        //
        //       We should revisit this in future, tracked by Bug 3655516.
        //

    if (SigningTime == NULL || SigningTime->QuadPart == 0)
#endif
    {
        //
        // Not timestamped, so take the issuance date of the signing cert.
        //
        // Note: For compatibility reason, we realize this is a hole, as we
        //       don't check certiticate expiration in CI, and thus someone
        //       can sign with an expired cert issued before the enforcement
        //       date forever.
        //
        //       In future, we should eventually phase out all drivers that
        //       are not timestamped, and this hole should then be closed.
        //

        SigningTime = &SigningInfo->NotValidBefore;
    }

    //
    //  Require WHQL except signed before enforecement date.
    //

    if (SigningTime->QuadPart >= BlImgSysDevWhqlEnforcementDateTime.QuadPart) {
        //
        // In interest of not bricking machine, indicate this can be exempted.
        //

        *LoadInformation |= LOAD_INFORMATION_WHQL_EXEMPTED_FOR_BOOT_DRIVER;

        return TRUE;
    }

    //
    // Signed before the enforcement date, so do not enfore WHQL.
    //

    return FALSE;
}
#endif

NTSTATUS
ImgpCopyPolicyInfo(
    _In_ PMINCRYPT_POLICY_INFO SrcInfo,
    _Outptr_ PMINCRYPT_POLICY_INFO* DstInfo
    )
/*++

Routine Description:

    This routine copies MINCRYPT_POLICY_INFO data structure from SrcInfo to DstInfo.
    Note that this function will NOT copy pChainInfo->rgPublicKey, and 
    pChainInfo->cSignerInfo[i].EncodedCertificate, so the caller should never use
    any of the related info from the returned pointer DstInfo.

Arguments:

    SrcInfo - Supplies a pointer to the source MINCRYPT_POLICY_INFO data structure.

    DstInfo - Supplies a pointer to receive the pointer that points to copied
        MINCRYPT_POLICY_INFO data structure. The memory allocated for DstInfo should
        be freed by calling BlMmFreeHeap.

Return Value:

    STATUS_SUCCESS when successful.

--*/

{
    
    ULONG i = 0;
    PMINCRYPT_POLICY_INFO LocalInfo = NULL;
    ULONG PolicyInfoSize;
    NTSTATUS Status = STATUS_SUCCESS;
    PUCHAR Work;

    //
    //  Find the total size for DstInfo.
    //

    PolicyInfoSize = sizeof(MINCRYPT_POLICY_INFO);
    if (SrcInfo->pChainInfo != NULL) {
        PolicyInfoSize += sizeof(MINCRYPT_CHAIN_INFO);

        if (SrcInfo->pChainInfo->cEKUs != 0) {
            PolicyInfoSize += sizeof(CRYPT_OBJID_BLOB)*SrcInfo->pChainInfo->cEKUs;

            for (i = 0; i < SrcInfo->pChainInfo->cEKUs; ++ i) {
                PolicyInfoSize += SrcInfo->pChainInfo->rgEKUs[i].cbData;
            }
        }

        if (SrcInfo->pChainInfo->cSignerInfos != 0) {
            PolicyInfoSize += sizeof(MINCRYPT_SIGNER_INFO) * SrcInfo->pChainInfo->cSignerInfos;
            for (i = 0; i < SrcInfo->pChainInfo->cSignerInfos; ++ i) {
                PolicyInfoSize += SrcInfo->pChainInfo->rgSignerInfos[i].PublisherCommonName.Length;
                PolicyInfoSize += SrcInfo->pChainInfo->rgSignerInfos[i].IssuerCommonName.Length;
            }
        }

        PolicyInfoSize += SrcInfo->pChainInfo->AuthenticatedAttributes.cbData;
    }

    LocalInfo = (PMINCRYPT_POLICY_INFO)BlMmAllocateHeap(PolicyInfoSize);
    if (LocalInfo == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto ImgpCopyPolicyInfoEnd;
    }

    RtlZeroMemory(LocalInfo, PolicyInfoSize);
    Work = (PUCHAR)LocalInfo;

    //
    //  Copy the data.
    //

    RtlCopyMemory(Work, SrcInfo, sizeof(MINCRYPT_POLICY_INFO));
    Work += sizeof(MINCRYPT_POLICY_INFO);

    if (SrcInfo->pChainInfo != NULL)
    {
        LocalInfo->pChainInfo = (PMINCRYPT_CHAIN_INFO)Work;
        RtlCopyMemory(LocalInfo->pChainInfo, 
                      SrcInfo->pChainInfo, 
                      sizeof(MINCRYPT_CHAIN_INFO));

        //
        //  rgPublicKeys will not be copied.
        //

        LocalInfo->pChainInfo->cPublicKeys = 0;
        LocalInfo->pChainInfo->rgPublicKeys = NULL;

        Work += sizeof(MINCRYPT_CHAIN_INFO);

        if (SrcInfo->pChainInfo->cEKUs != 0)
        {
            LocalInfo->pChainInfo->rgEKUs = (PCRYPT_OBJID_BLOB)Work;
            RtlCopyMemory(LocalInfo->pChainInfo->rgEKUs,
                          SrcInfo->pChainInfo->rgEKUs,
                          sizeof(CRYPT_OBJID_BLOB)*SrcInfo->pChainInfo->cEKUs);

            Work += sizeof(CRYPT_OBJID_BLOB)*SrcInfo->pChainInfo->cEKUs;

            for (i = 0; i < SrcInfo->pChainInfo->cEKUs; ++i)
            {
                LocalInfo->pChainInfo->rgEKUs[i].pbData = Work;
                RtlCopyMemory(LocalInfo->pChainInfo->rgEKUs[i].pbData,
                              SrcInfo->pChainInfo->rgEKUs[i].pbData,
                              SrcInfo->pChainInfo->rgEKUs[i].cbData);
                if (SrcInfo->pChainInfo->rgEKUs[i].cbData == 0)
                {
                    LocalInfo->pChainInfo->rgEKUs[i].pbData = NULL;
                }

                Work += SrcInfo->pChainInfo->rgEKUs[i].cbData;
            }
        }

        if (SrcInfo->pChainInfo->cSignerInfos != 0)
        {
            LocalInfo->pChainInfo->rgSignerInfos = (PMINCRYPT_SIGNER_INFO)Work;
            RtlCopyMemory(LocalInfo->pChainInfo->rgSignerInfos,
                          SrcInfo->pChainInfo->rgSignerInfos, 
                          sizeof(MINCRYPT_SIGNER_INFO) * SrcInfo->pChainInfo->cSignerInfos);

            Work += sizeof(MINCRYPT_SIGNER_INFO) * SrcInfo->pChainInfo->cSignerInfos;

            for (i = 0; i < SrcInfo->pChainInfo->cSignerInfos; ++ i)
            {
                LocalInfo->pChainInfo->rgSignerInfos[i].PublisherCommonName.Buffer = (PCHAR)Work;
                RtlCopyMemory(LocalInfo->pChainInfo->rgSignerInfos[i].PublisherCommonName.Buffer,
                              SrcInfo->pChainInfo->rgSignerInfos[i].PublisherCommonName.Buffer,
                              SrcInfo->pChainInfo->rgSignerInfos[i].PublisherCommonName.Length);

                Work += SrcInfo->pChainInfo->rgSignerInfos[i].PublisherCommonName.Length;

                LocalInfo->pChainInfo->rgSignerInfos[i].IssuerCommonName.Buffer = (PCHAR)Work;
                RtlCopyMemory(LocalInfo->pChainInfo->rgSignerInfos[i].IssuerCommonName.Buffer,
                              SrcInfo->pChainInfo->rgSignerInfos[i].IssuerCommonName.Buffer,
                              SrcInfo->pChainInfo->rgSignerInfos[i].IssuerCommonName.Length);

                Work += SrcInfo->pChainInfo->rgSignerInfos[i].IssuerCommonName.Length;

                //
                // LocalInfo->pChainInfo->rgSignerInfos[i].EncodedCertificate will not be 
                // copied.
                //

                LocalInfo->pChainInfo->rgSignerInfos[i].EncodedCertificate.pbData = NULL;
                LocalInfo->pChainInfo->rgSignerInfos[i].EncodedCertificate.cbData = 0;
            }
        }

        if (SrcInfo->pChainInfo->AuthenticatedAttributes.cbData != 0)
        {
            LocalInfo->pChainInfo->AuthenticatedAttributes.pbData = Work;
            RtlCopyMemory(LocalInfo->pChainInfo->AuthenticatedAttributes.pbData,
                          SrcInfo->pChainInfo->AuthenticatedAttributes.pbData,
                          SrcInfo->pChainInfo->AuthenticatedAttributes.cbData);

            Work += SrcInfo->pChainInfo->AuthenticatedAttributes.cbData;
        }
    }

    *DstInfo = LocalInfo;
    LocalInfo = NULL;

ImgpCopyPolicyInfoEnd:

    if (LocalInfo != NULL) {
        BlMmFreeHeap(LocalInfo);
        LocalInfo = NULL;
    }
    return Status;

}

BOOLEAN
ImgpPolicyFlightRootTrustDisabled (
    VOID
    )
/*++

Routine Description:

    This routine checks if flight root trust is disabled by Config CI policy.

Arguments:

    None.

Return Value:

    TRUE if flight root trust is disabled by Config CI policy, else FALSE.

--*/
{
    BOOLEAN CiAuditEnabled;
    SYSTEM_INTEGRITY_POLICY_HANDLE CiPolicyHandle;
    BOOLEAN FlightDisabled;
    BOOLEAN SkuAuditEnabled;
    SYSTEM_INTEGRITY_POLICY_HANDLE SkuPolicyHandle;

    CiAuditEnabled = FALSE;
    CiPolicyHandle = NULL;
    FlightDisabled = FALSE;
    SkuAuditEnabled = FALSE;
    SkuPolicyHandle = NULL;

    //
    // Get policies from Config CI.
    //

    if ((SIPolicyIsPolicyActive(SiPolicyTypeCi, &CiPolicyHandle) != FALSE &&
         NT_SUCCESS(SIPolicyAuditModeEnabled(CiPolicyHandle, &CiAuditEnabled)) &&
         CiAuditEnabled == FALSE &&
         SIPolicyFlightRootTrustDisabled(CiPolicyHandle) != FALSE) ||
        (SIPolicyIsPolicyActive(SiPolicyTypeSku, &SkuPolicyHandle) != FALSE &&
         NT_SUCCESS(SIPolicyAuditModeEnabled(SkuPolicyHandle, &SkuAuditEnabled)) &&
         SkuAuditEnabled == FALSE &&
         SIPolicyFlightRootTrustDisabled(SkuPolicyHandle) != FALSE)) {
        FlightDisabled = TRUE;
    }

    return FlightDisabled;
}

VOID
ImgpProcessFlightRootTrustPolicy (
    VOID
    )
/*++

Routine Description:

    This routine processes Config CI policy for the flight root trust option.
    If the option is set and audit mode is not on, flight root trust will be
    disabled.

Arguments:

    None.

Return Value:

    None.

--*/
{
    //
    // If flight root is currently trusted, disable flight root trust
    // option is turned on, and audit mode is not turned on, then
    // remove flight root trust.
    //

    if (CHECK_FLAG(BlImgAcceptedRootKeys, MINCRYPT_POLICY_FLIGHTROOT) &&
        ImgpPolicyFlightRootTrustDisabled() != FALSE) {

        //
        // Remove flight root trust.
        //

        CLEAR_FLAGS(BlImgAcceptedRootKeys, MINCRYPT_POLICY_FLIGHTROOT);
        CLEAR_FLAGS(BlImgAcceptedTrustedBootRootKeys, MINCRYPT_POLICY_FLIGHTROOT);

        //
        // Tell MinCrypt about flight signing option.
        //

        MincrypL_SetFlightSigningOption(FALSE);
    }
}

NTSTATUS
ImgpLoadAndValidateImageHashWithCatalog(
    _In_ PWSTR LoadFile,
    _In_reads_bytes_(BaseImageSize) PVOID BaseImageAddr,
    _In_ ULONG BaseImageSize,
    _In_opt_ PBOOT_CATALOG_ENTRY CatalogEntry,
    _In_ ALG_ID HashAlgId,
    _In_reads_bytes_(MAX_HASH_LEN) PCUCHAR HashBuffer,
    _In_ ULONG ValidationFlags,
    _In_ LOGICAL CheckRevocationList,
    _In_ IMG_SIGNING_SCENARIO_TYPE Scenario,
    _In_ ULONG AcceptedRootKeys,
    _In_ BOOLEAN MaybeWeakHash,
    _In_ PLARGE_INTEGER FileHashNotAfter,
    _Inout_opt_ PIMG_RETRY_REASON RetryReason,
    _Inout_opt_ PULONG LoadInformation,
    _Out_opt_ PBOOT_TRUSTED_BOOT_INFORMATION TrustedBootInformation
    )

/*++

Routine Description:

    This routine validates an image hash against the system (or specific)
    catalogs, and check that the catalog validates for the specified scenario.

Arguments:

    LoadFile - Supplies the path of the image being loaded.

    BaseImageAddr - Supplies a pointer to the image being loaded.

    BaseImageSize - Supplies the size in bytes of the image being loaded.

    CatalogEntry - Supplies a pointer to the optional catalog entry to validate
         against. If the entry is not specified, all currently loaded catalogs
         will be checked.

    HashAlgId - Supplies the hash algorithm type of the image hash.

    HashBuffer - Supplies a pointer to the image hash.

    ValidationFlags - Supplies flags controlling the characteristics of the
        validation operation.

    CheckRevocationList - Supplies FALSE if the image hash is not to be checked
         against the revocation list.

    Scenario - Supplies the type of signing scenario to validate for.

    AcceptedRootKeys - Supplies the list of root flags to validate against
         when classic signing enforcement is being used.

    MaybeWeakHash - Supplies TRUE if the hash algorithm may be weak.

    FileHashNotAfter - Supplies a pointer to a time that a weak hash must be
         signed before to be allowed.

    LoadInformation - Supplies an optional pointer to a variable that receives
         informational flags describing various aspects of the load.  The
         following flags may be set during image load.

         LOAD_INFORMATION_POLICY_VIOLATION : Indicates that the image violated
         system integrity policy.

    RetryReason - Supplies an optional pointer to a structure that receives a
         reason for why a retry is needed.

    TrustedBootInformation - Supplies a pointer that receives the Trusted Boot
         information about the validated image and catalog.

Return Value:

    STATUS_SUCCESS if function succeeded.

    STATUS_RETRY if the operation should be retried with a different hash.

    Another error status code if returned from a sub-routine.

--*/

{
    MINCRYPT_TRUSTEDBOOT_INFO MincryptTrustedBootInformation;
    PMINCRYPT_POLICY_INFO PolicyInfo;
    LARGE_INTEGER SigningTime;
    NTSTATUS Status;

#if defined(MISC_MIN)
    UNREFERENCED_PARAMETER(BaseImageAddr);
    UNREFERENCED_PARAMETER(BaseImageSize);
    UNREFERENCED_PARAMETER(FileHashNotAfter);
    UNREFERENCED_PARAMETER(LoadFile);
    UNREFERENCED_PARAMETER(LoadInformation);
    UNREFERENCED_PARAMETER(RetryReason);
    UNREFERENCED_PARAMETER(ValidationFlags);
#endif

    Status = ImgpValidateImageHashWithCatalog(
                CatalogEntry,
                HashAlgId,
                HashBuffer,
                CheckRevocationList,
                Scenario,
                AcceptedRootKeys,
                &MincryptTrustedBootInformation,
                MaybeWeakHash != FALSE ? &SigningTime : NULL,
                &PolicyInfo);

#if !defined(MISC_MIN)
    if (NT_SUCCESS(Status) &&
        (MaybeWeakHash != FALSE) &&
        (ImgpIsWeakSigningTime(&SigningTime,
                               FileHashNotAfter) != FALSE)) {

        if (ARGUMENT_PRESENT(RetryReason)) {
            RetryReason->WeakFileHashBySigningTime = TRUE;
            RetryReason->HashAlgorithm = CI_WEAK_FILE_HASH_RETRY_HASH_ALGORITHM;
            Status = STATUS_RETRY;
        }
        else {
            Status = STATUS_INVALID_IMAGE_HASH;
        }
    }

    if (NT_SUCCESS(Status) &&
        (CHECK_FLAG(ValidationFlags, IMGP_NO_CONFIG_CI_CHECKS) == 0)) {
        Status = BlImgpApplySIPolicy(
                    Scenario,
                    BaseImageAddr,
                    BaseImageSize,
                    LoadFile,
                    HashAlgId,
                    HashBuffer,
                    PolicyInfo->pChainInfo,
                    LoadInformation);
    }

    if (NT_SUCCESS(Status) && Scenario == ImgSigningScenarioDriver)
    {
        //
        // This is called for any driver load scenario for revocation information telemetry
        // verified using catalog
        //

        ImgpLogDriverLoadAudit(PolicyInfo, FALSE, &SigningTime, LoadFile);
    }
#endif

    if (NT_SUCCESS(Status)) {
        Status = ImgpInitializeTrustedBootInformation(
                    TrustedBootInformation,
                    &MincryptTrustedBootInformation);
    }

    return Status;
}

_Success_(return != FALSE)
BOOLEAN
ImgpProcessHashRetryReason(
    _In_ PCIMG_RETRY_REASON RetryReason,
    _Inout_ PIMG_RETRY_REASON CurrentState,
    _In_ ALG_ID MinimumHashAlgorithm,
    _Inout_ ALG_ID *HashAlgorithm,
    _Inout_ PMINCRYPT_SIGNATURE_DESCRIPTOR EmbeddedSignature
    )

/*++

Routine Description:

    This routine processes a hash validation retry reason and determines if the
    retry should be made.

Arguments:

    RetryReason - Supplies a pointer to the reason for the retry request.

    CurrentState - Supplies a pointer to the retries already attempted.

    MinimumHashAlgorithm - Supplies the minimum hash algorithm for the scenario
         that is being retried.

    HashAlgorithm - Supplies a pointer to the current hash algorithm and
         receives the hash algorithm for the retry when a retry is needed.

    EmbeddedSignature - Supplies a pointer to the embedded signature context.
         The context is cleared when a retry should use catalogs instead.

Return Value:

    TRUE if the hash should be retried, FALSE otherwise.

--*/

{
    ALG_ID NewAlgorithm;
    IMG_RETRY_REASON Reason;
    BOOLEAN Retry = FALSE;

    //
    // Mask out any retry reasons that have already been attempted and then add
    // the new reason into the already attempted state.
    //

    Reason = *RetryReason;
    Reason.RetryState &= ~CurrentState->RetryState;
    CurrentState->RetryState |= Reason.RetryState;

    if (Reason.EmbeddedSignatureHashAlgorithm != FALSE) {

        //
        // This retry can be reattempted as needed, so clear it out of the
        // already attempted state.
        //

        CurrentState->EmbeddedSignatureHashAlgorithm = FALSE;

        //
        // For embedded signatures only a higher algorithm should be returned.
        //

        NT_ASSERT(Reason.HashAlgorithm > *HashAlgorithm);

        *HashAlgorithm = Reason.HashAlgorithm;
        Retry = TRUE;
    }
    else if (Reason.HashAlgorithmNotFoundInCatalogs != FALSE ||
             Reason.HashAlgorithmNotSupportedByCatalogs != FALSE) {

        //
        // ImgpValidateImageHash will return the lowest algorithm supported by
        // catalogs and so we may need to raise the algorithm for the scenario.
        // If the algorithm is not supported by the catalogs or ends up being
        // the same as already tried, just fail.
        //

        NewAlgorithm = max(Reason.HashAlgorithm, MinimumHashAlgorithm);
        if (CI_IS_CATALOG_HASH_ALGORITHM(NewAlgorithm) != FALSE &&
            *HashAlgorithm != NewAlgorithm)
        {
            *HashAlgorithm = NewAlgorithm;
            RtlZeroMemory(EmbeddedSignature, sizeof(*EmbeddedSignature));
            Retry = TRUE;
        }
    }
    else if (Reason.WeakFileHashBySigningTime != FALSE)
    {
        //
        // For weak algorithms only a higher algorithm should be returned.
        // We do not clear out the signature since the image may be dual signed
        // and so we can still use the secondary signature to verify the string
        // hash.
        //

        NT_ASSERT(Reason.HashAlgorithm > *HashAlgorithm);

        *HashAlgorithm = Reason.HashAlgorithm;
        Retry = TRUE;
    }

    return Retry;
}

BOOLEAN
ImgpIsWeakSigningTime(
    _In_ PLARGE_INTEGER SigningTime,
    _In_ PLARGE_INTEGER NotAfter
    )

/*++

Routine Description:

    This routine checks if a signing time does not allow a weak hash to be
    allowed.

Arguments:

    SigningTime - Supplies a pointer to a signing time.

    NotAfter - Supplies a pointer to the time after which a hash must be
         considered weak.

Return Value:

    TRUE if the signing time does not allow the weak hash.

--*/

{
    LARGE_INTEGER LocalSigningTime;

    LocalSigningTime.QuadPart = SigningTime->QuadPart;

    //
    // Use the current system time if there is no signing time. However if that
    // fails just use a small non-zero time so that a FileHashNotAfter of zero
    // can act as a kill switch.
    //

    if (LocalSigningTime.QuadPart == 0) {
        if (ImgpQuerySystemTime(&LocalSigningTime) == FALSE) {
            LocalSigningTime.QuadPart = 1;
        }
    }

    if (LocalSigningTime.QuadPart > NotAfter->QuadPart) {
        return TRUE;
    }

    return FALSE;
}

BOOLEAN
ImgpRequirePlatformManifestForSigner(
    _In_ ULONG ValidationSinger
    )
/*++

    Routine Description:

        This routine checks whether a Platform Manifest check is requierd for
        this signer.

    Arguments:

        ValidationSigner - Which signer ID validated this file.

    Return Value:

        BOOLEAN - TRUE if PlatformManifest check is required.

--*/

{

    if (BlSecureBootIsPlatformManifestActive())
    {
        //
        // Check to see if this signer is present in the Platform Manifest Required Scenario.
        // This is determed by checking if this signer is present in the pseudo-signing scenario
        // called Platform Manifest Required. 
        //

        if (ImgpDoesScenarioTrustSigner(ImgSigningScenarioPlatformManifestRequired, ValidationSinger))
        {
            //
            // Platform Manifest check is required for this signer
            //

            return TRUE;
        }
    }

    return FALSE;
}

NTSTATUS
ImgpValidatePlatformManifestForEmbeddedSignedFile(
    _In_ ULONG ValidationSinger,
    _In_ PCMINCRYPT_POLICY_INFO MincryptPolicyInfo
    )
/*++

    Routine Description:

        This routine first checks to see if a platform manifest entry is required
        for this embedded signature, and if it is, verifies that it is present.

    Arguments:

        ValidationSigner - Which signer ID validated this file.

        MincryptPolicyInfo - The mincrypt policy information gathered
            by Mincrypt during validation of the signature for this file.  It 
            includes the Binary ID of the file, if one was present in the 
            signature.  It is that Binary ID which will be checked against the 
            platform manifest.

    Return Value:

        NTSTATUS

--*/

{
    NTSTATUS Status = STATUS_SUCCESS;

    if (ImgpRequirePlatformManifestForSigner(ValidationSinger)) 
    {

        Status = BlSecureBootIsHashAuthorizedInPlatformManifest(
                    (PBYTE)&MincryptPolicyInfo->pChainInfo->PlatformManifestID,
                    sizeof(MincryptPolicyInfo->pChainInfo->PlatformManifestID)
                    );

        if (!NT_SUCCESS(Status)) 
        {
            if (Status == STATUS_NOT_FOUND) 
            {
                Status = STATUS_PLATFORM_MANIFEST_FILE_NOT_AUTHORIZED;
            }

            goto ValidatePlatformManifestForEmbeddedSignedFileEnd;
        }
    }

ValidatePlatformManifestForEmbeddedSignedFileEnd:

    return Status;
}

NTSTATUS
ImgpValidatePlatformManifestForCatalog(
    _In_ ULONG ValidationSinger,
    _In_ PCUNICODE_STRING PackageName
    )
/*++

    Routine Description:

        This routine first checks to see if a platform manifest entry is required
        for this Catalog file.

    Arguments:

        ValidationSigner - Which signer ID validated this file.

        PackageName - The name of the package which was extracted from the 
            catalog.  This name will be checked against the Platform Manifest
            if necessary.

    Return Value:

        NTSTATUS

--*/

{
    NTSTATUS Status = STATUS_SUCCESS;

    if (ImgpRequirePlatformManifestForSigner(ValidationSinger)) 
    {
        Status = BlSecureBootIsStringAuthorizedInPlatformManifest(PackageName);

        if (!NT_SUCCESS(Status)) 
        {
            if (Status == STATUS_NOT_FOUND) 
            {
                Status = STATUS_PLATFORM_MANIFEST_FILE_NOT_AUTHORIZED;
            }

            goto ValidatePlatformManifestForCatalog;
        }
    }

ValidatePlatformManifestForCatalog:

    return Status;
}

#if !defined(MISC_MIN)

VOID
BlImgGetWhqlEnforcementDateTime (
    _Out_ PLARGE_INTEGER EnforcementDateTime
    )
/*++

Routine Description:

    This routine returns the date and time for WHQL driver enforcement.

Arguments:

    EnforcementDateTime - Supplies a pointer that receives the WHQL driver
         enforcement date and time.

Return Value:

    None.

--*/
{
    *EnforcementDateTime = BlImgSysDevWhqlEnforcementDateTime;
}

BOOLEAN
BlImgIsUpgradeInProgress (
    VOID
    )
/*++

Routine Description:

    This routine checks if the system is in the process of upgrading.

Arguments:

    None.

Return Value:

    TRUE if set, else FALSE.

--*/
{
    return BlImgSystemUpgradeInProgress;
}

#endif // !defined(MISC_MIN)

