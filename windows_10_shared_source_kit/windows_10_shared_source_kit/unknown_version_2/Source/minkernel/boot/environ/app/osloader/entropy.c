/*++

Copyright (c) Microsoft Corporation

Module Name:

    entropy.c

Abstract:

    This module contains routines for the entropy/rng processing in the
    os loader.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"
#include "kernelprng.h"

//
// ---------------------------------------------------------------- Definitions
//

#define ENTROPY_TPM_BYTES_TO_GATHER (40)

#define RNG_KEY_NAME                    L"RNG"
#define SEED_VALUE_NAME                 L"Seed"
#define EXTERNAL_ENTROPY_VALUE_NAME     L"ExternalEntropy"
#define EXTERNAL_ENTROPY_MAX_LENGTH     256

typedef VOID (*ENTROPY_SOURCE_GATHER_FUNCTION) (
    ULONG SystemHiveHandle,
    PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    );

typedef struct _SEED_FILE_LAYOUT
{
    BYTE    magic[8];       // "SeedFile"
    ULONG   estimatedEntropyInMilliBits;
    BYTE    data[SYMCRYPT_SHA512_RESULT_SIZE];
} SEED_FILE_LAYOUT, *PSEED_FILE_LAYOUT;

#if defined(EFI)

#define BYTES_FROM_EFI_RNG  (32)

#endif

static const BYTE OslpEntropySeedFileMagic[8] = {'S', 'e', 'e', 'd', 'F', 'i', 'l', 'e'};

LOGICAL OslRngInitialized;
LOGICAL SymCryptRngInitialized;

//
// -------------------------------------------------------------------- Globals
//

SYMCRYPT_RNG_AES_FIPS140_2_STATE SymCryptRng;

//
// ----------------------------------------------------------------- Prototypes
//

VOID
OslpGatherAcpiOem0Entropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    );

VOID
OslpGatherExternalEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    );

VOID
OslpGatherSeedFileEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    );

VOID
OslpGatherTimeEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    );

VOID
OslpGatherTpmEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    );

VOID
OslpGatherRdrandEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    );

VOID
OslpGatherUefiEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    );

VOID
OslpGatherCngEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    );


//
// ----------------------------------------------------------- More definitions
//

const ENTROPY_SOURCE_GATHER_FUNCTION OslpEntropySourceGatherFunctions[] = {
    OslpGatherSeedFileEntropy,
    OslpGatherExternalEntropy,
    OslpGatherTpmEntropy,
    OslpGatherTimeEntropy,
    OslpGatherAcpiOem0Entropy,

#if defined (_X86_) | defined(_AMD64_)

    OslpGatherRdrandEntropy,

#endif

#if defined (EFI)

    OslpGatherUefiEntropy,

#endif

#if defined (NTKERNEL)

    OslpGatherCngEntropy,

#endif

};

#define N_SOURCE_GATHER_FUNCTIONS (\
        sizeof(OslpEntropySourceGatherFunctions) / \
        sizeof(OslpEntropySourceGatherFunctions[0]) \
    )

//
// ------------------------------------------------------------------ Functions
//

VOID
OslConfigureRNG (
    _In_ PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine initializes the OS Loader's internal RNG using the entropy
    data stashed in the loader block.

Arguments:

    LoaderBlock - Supplies a pointer to the loader block.

Return Value:

    None.

--*/

{

    PBOOT_ENTROPY_LDR_RESULT BootResult;

    //
    // The first few RNG bytes intended for NTOSKRNL are reserved for use with setting
    // up boot loaded module /GS cookies.
    // XXX: These are no longer used by boot, but continue to remove them for
    // XXX: compatibility with NTOSKRNL (see ExpRngInitSystem).
    //

    BootResult = &LoaderBlock->Extension->BootEntropyResult;
    RtlSecureZeroMemory(&BootResult->RngBytesForNtoskrnl[0],
                        BOOT_BL_NTOSKRNL_RNG_BYTES_USED);

    NT_ASSERT(SymCryptRngInitialized != FALSE);
    NT_ASSERT(OslRngInitialized == FALSE);

    OslRngInitialized = TRUE;
    return;
}

ULONG_PTR
OslGenRandom (
    VOID
    )

/*++

Routine Description:

    Generates a random, pointer-sized value.

Arguments:

    None.

Return Value:

    A random, pointer-sized value is returned.

--*/

{

    ULONG_PTR Value;

    NT_ASSERT(SymCryptRngInitialized != FALSE);
    NT_ASSERT(OslRngInitialized != FALSE);

    SymCryptRngAesFips140_2Generate(&SymCryptRng, (PBYTE)&Value, sizeof(Value));
    return Value;
}

VOID
OslTerminateRNG (
    VOID
    )

/*++

Routine Description:

    This routine clears the state of the RNG after all usage of the RNG has
    completed.

Arguments:

    None.

Return Value:

    None.

--*/

{

    OslRngInitialized = FALSE;
    SymCryptRngInitialized = FALSE;
    SymCryptRngAesFips140_2Uninstantiate(&SymCryptRng);
    return;
}

VOID
OslpGatherSeedFileEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    )

/*++

Routine Description:

    This routine gets the random data from the seed file.

Arguments:

    SystemHiveHandle - Supplies a Handle to the system hive

    EntropyResult - Supplies a pointer to a buffer that receives
        the results of the entropy operation.

Return Value:

    None

--*/

{

    ULONG DataLength;
    SEED_FILE_LAYOUT Layout;
    BOOT_ENTROPY_SOURCE_RESULT_CODE ResultCode;
    PCM_KEY_NODE RngKeyCell;
    PCM_KEY_NODE RootCell;
    NTSTATUS Status;
    ULONG Type;

    C_ASSERT(BOOT_ENTROPY_SOURCE_DATA_SIZE == sizeof(Layout.data));

    EntropyResult->SourceId = BootEntropySourceSeedfile;
    EntropyResult->Policy = 0;

    //
    // The seed file is NOT controlled by the Initial Machine Config system.
    // IMC should use the External entropy key to inject entropy into a 
    // new machine, or just rely on the OEM0 ACPI table.
    //

    RootCell = OslGetRootCell(SystemHiveHandle);
    Status = OslGetSubkey(SystemHiveHandle,
                            RootCell,
                            RNG_KEY_NAME,
                            &RngKeyCell);

    if (!NT_SUCCESS(Status)) {
        ResultCode = BootEntropySourceNotPresent;
        goto cleanup;
    }

    DataLength = sizeof(Layout);
    Status = OslGetValue(SystemHiveHandle,
                            RngKeyCell,
                            SEED_VALUE_NAME,
                            &Type,
                            &DataLength,
                            (PBYTE)&Layout);

    if (!NT_SUCCESS(Status)) {
        ResultCode = BootEntropySourceNotPresent;
        goto cleanup;
    }

    if (DataLength != sizeof(Layout)) {
        Status = STATUS_INFO_LENGTH_MISMATCH;
        ResultCode = BootEntropySourceError;
        goto cleanup;
    }

    RtlCopyMemory(EntropyResult->EntropyData,
                  Layout.data,
                  BOOT_ENTROPY_SOURCE_DATA_SIZE);

    EntropyResult->EntropyLength = BOOT_ENTROPY_SOURCE_DATA_SIZE;

    //
    // If the OS is not properly shut down, the magic string is different from "SeedFile".
    // We pass this on to the OS through the status field.
    //

    if( memcmp( Layout.magic, OslpEntropySeedFileMagic, sizeof( Layout.magic ) ) != 0 ) {
        Status = STATUS_DEVICE_POWER_FAILURE;
    } else {
        Status = STATUS_SUCCESS;
    }

    ResultCode = BootEntropySourceSuccess;

cleanup:
    EntropyResult->ResultCode = ResultCode;
    EntropyResult->ResultStatus = Status;

    RtlSecureZeroMemory(&Layout, sizeof(Layout));
}


VOID
OslpGatherExternalEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    )

/*++

Routine Description:

    This routine gets the random data from the external entropy registry key.

Arguments:

    SystemHiveHandle - Supplies a Handle to the system hive

    EntropyResult - Supplies a pointer to a buffer that receives
        the results of the entropy operation.

Return Value:

    None

--*/

{

    BYTE Data[EXTERNAL_ENTROPY_MAX_LENGTH];
    ULONG DataLength;
    BOOT_ENTROPY_SOURCE_RESULT_CODE ResultCode;
    PCM_KEY_NODE RngKeyCell;
    PCM_KEY_NODE RootCell;
    NTSTATUS Status;
    ULONG Type;

    C_ASSERT(BOOT_ENTROPY_SOURCE_DATA_SIZE >= SYMCRYPT_SHA512_RESULT_SIZE);

    EntropyResult->SourceId = BootEntropySourceExternal;
    EntropyResult->Policy = 0;

    //
    // Check to see if there is an Initial Machine Config value for this.
    //

    Status = OslGetImcSystemEntropyKeyValue(EXTERNAL_ENTROPY_VALUE_NAME,
                                           Data,
                                           sizeof(Data),
                                           &DataLength);

    if (!NT_SUCCESS(Status)) {

        RootCell = OslGetRootCell(SystemHiveHandle);
        Status = OslGetSubkey(SystemHiveHandle,
                              RootCell,
                              RNG_KEY_NAME,
                              &RngKeyCell);
        if (!NT_SUCCESS(Status)) {
            ResultCode = BootEntropySourceNotPresent;
            goto cleanup;
        }

        DataLength = sizeof(Data);
        Status = OslGetValue(SystemHiveHandle,
                             RngKeyCell,
                             EXTERNAL_ENTROPY_VALUE_NAME,
                             &Type,
                             &DataLength,
                             Data);
    }

    if (!NT_SUCCESS(Status)) {
        ResultCode = BootEntropySourceNotPresent;
        goto cleanup;
    }

    SymCryptSha512(Data, DataLength, EntropyResult->EntropyData);
    EntropyResult->EntropyLength = SYMCRYPT_SHA512_RESULT_SIZE;
    Status = STATUS_SUCCESS;
    ResultCode = BootEntropySourceSuccess;

cleanup:
    EntropyResult->ResultCode = ResultCode;
    EntropyResult->ResultStatus = Status;
}


VOID
OslpGatherTpmEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    )

/*++

Routine Description:

    This routine gets random data from the TPM.  The behavior of the routine
    is controlled by the BCD TPM entropy boot option.

Arguments:

    SystemHiveHandle - Supplies a Handle to the system hive

    EntropyResult - Supplies a pointer to a buffer that receives
        the results of the TPM entropy operation.

Return Value:

    None

--*/

{

    ULONGLONG Policy;
    BOOT_ENTROPY_SOURCE_RESULT_CODE ResultCode;
    NTSTATUS Status;

    C_ASSERT(ENTROPY_TPM_BYTES_TO_GATHER <=
             sizeof(EntropyResult->EntropyData));

    UNREFERENCED_PARAMETER(SystemHiveHandle);

    EntropyResult->SourceId = BootEntropySourceTpm;

    //
    // Obtain the policy from BCD.  If the policy is unknown or non-existent,
    // use the default policy.
    //

    Status = BlGetApplicationOptionInteger(
                 BCDE_OSLOADER_TYPE_TPM_BOOT_ENTROPY_POLICY,
                 &Policy);

    if ((!NT_SUCCESS(Status)) ||
        ((Policy != TpmBootEntropyPolicyForceEnable) &&
         (Policy != TpmBootEntropyPolicyForceDisable))) {

        Policy = TpmBootEntropyPolicyDefault;
    }

    EntropyResult->Policy = Policy;

    //
    // Return if the policy is disabled.
    //

    if (Policy == TpmBootEntropyPolicyForceDisable) {
        ResultCode = BootEntropySourceDisabledByPolicy;
        Status = STATUS_SUCCESS;
        goto cleanup;
    }

    //
    // Return if the TPM does not exist.
    //

    Status = BlTpmStatus();
    if (!NT_SUCCESS(Status)) {
        ResultCode = BootEntropySourceNotPresent;
        goto cleanup;
    }

    //
    // Obtain the entropy from the TPM.  Save the time the operation took for
    // logging purposes.
    //

    Status = BlTpmGetRandom(ENTROPY_TPM_BYTES_TO_GATHER,
                            &EntropyResult->EntropyData[0]);

    if (!NT_SUCCESS(Status)) {
        ResultCode = BootEntropySourceError;
        goto cleanup;
    }

    EntropyResult->EntropyLength = ENTROPY_TPM_BYTES_TO_GATHER;
    ResultCode = BootEntropySourceSuccess;
    Status = STATUS_SUCCESS;


cleanup:
    EntropyResult->ResultCode = ResultCode;
    EntropyResult->ResultStatus = Status;
}


VOID
OslpGatherTimeEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    )

/*++

Routine Description:

    This routine gathers timing information from the boot library.
    This is not a good entropy source, but it provides a fallback
    for those situations where none of the other sources has any entropy.
    (e.g. Identical machines from OEMs without TPM or RDRAND.)
    This gives us at least a fighting chance to be different on different
    machines.

Arguments:

    SystemHiveHandle - Supplies a Handle to the system hive

    EntropyResult - Supplies a pointer to a buffer that receives
        the results of the entropy operation.

Return Value:

    None

--*/

{

    struct {
        ULONGLONG TimeStampCounter;
        ULONGLONG ArchPerfCounter;
        LARGE_INTEGER TimeQueryPerformanceCounter;
        LARGE_INTEGER TimeQueryPerformanceCounterFrequency;
        TIME_FIELDS TimeFields;
        ULONG RelativeTime;
    } TimeValues;

    C_ASSERT(sizeof(TimeValues) <= BOOT_ENTROPY_SOURCE_DATA_SIZE);

    UNREFERENCED_PARAMETER(SystemHiveHandle);

    //
    // Gather all the inforamtion we can find through the APIs.
    // Much of this is redundant, but some functions return different data on
    // different architectures, so just gather everything and hash it all.
    //

    TimeValues.TimeStampCounter = ReadTimeStampCounter();
    TimeValues.ArchPerfCounter = BlArchGetPerformanceCounter();
    TimeValues.TimeQueryPerformanceCounter =
        BlTimeQueryPerformanceCounter(
            &TimeValues.TimeQueryPerformanceCounterFrequency);

    BlGetTime(&TimeValues.TimeFields, NULL);
    TimeValues.RelativeTime = BlTimeGetRelativeTime();
    EntropyResult->SourceId = BootEntropySourceTime;
    EntropyResult->ResultCode = BootEntropySourceSuccess;
    EntropyResult->EntropyLength = sizeof(TimeValues);

    //
    // Copy the structure directly.
    // In general this is a bad thing to do as some of the fields
    // might not have been initialized and the padding space
    // in the structure is uninitialized.
    // However, for entropy gathering it doesn't matter that there
    // are undefined bytes in the structure.
    //

    RtlCopyMemory(EntropyResult->EntropyData, &TimeValues, sizeof(TimeValues));
}


VOID
OslpGatherAcpiOem0Entropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    )

/*++

Routine Description:

    This routine gathers random data from the ACPI tables.
    Hyper-V adds 64 bytes of high-quality random to the "OEM0" table.
    As the boot library doesn't allow for enumeration of other ACPI tables
    we do that in CNG when it loads.

Arguments:

    SystemHiveHandle - Supplies a Handle to the system hive

    EntropyResult - Supplies a pointer to a buffer that receives
        the results of the entropy operation.

Return Value:

    None

--*/

{

    ULONG Bytes;
    BOOT_ENTROPY_SOURCE_RESULT_CODE ResultCode;
    NTSTATUS Status;
    POEM0_TABLE Table;

    C_ASSERT(BOOT_ENTROPY_SOURCE_DATA_SIZE >= SYMCRYPT_SHA512_RESULT_SIZE);

    UNREFERENCED_PARAMETER(SystemHiveHandle);

    Status = STATUS_SUCCESS;
    ResultCode = BootEntropySourceSuccess;
    EntropyResult->SourceId = BootEntropySourceAcpiOem0;
    Status = BlUtlGetAcpiTable(&Table, OEM0_SIGNATURE);
    if(!NT_SUCCESS(Status)) {
        ResultCode = BootEntropySourceNotPresent;
        goto Cleanup;
    }

    //
    // Even if the table is not the right size; we don't report an error and
    // pass the data that we get. We use all bytes from entropy sources, even 
    // those that report an error.
    // MSFT 232457: In previous versions we reported an error which our tests 
    // reported as a compatibility error.
    //

    if(Table->Header.Length > sizeof(DESCRIPTION_HEADER)) {
        Bytes = min(Table->Header.Length - sizeof(DESCRIPTION_HEADER),
                    BOOT_ENTROPY_SOURCE_DATA_SIZE);

        EntropyResult->EntropyLength = Bytes;
        RtlCopyMemory(EntropyResult->EntropyData, &Table->Data[0], Bytes);
    }

Cleanup:
    EntropyResult->ResultCode = ResultCode;
    EntropyResult->ResultStatus = Status;
}

#if defined (_X86_) | defined(_AMD64_)

VOID
OslpGatherRdrandEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    )

/*++

Routine Description:

    This routine gathers random data from the RDRAND instruction.

Arguments:

    SystemHiveHandle - Supplies a Handle to the system hive

    EntropyResult - Supplies a pointer to a buffer that receives
        the results of the entropy operation.

Return Value:

    None

--*/

{

    PBYTE Buf;
    BOOT_ENTROPY_SOURCE_RESULT_CODE ResultCode;
    NTSTATUS Status;

    C_ASSERT(2 * SYMCRYPT_SHA512_RESULT_SIZE <= 3 * SYMCRYPT_RDRAND_RESEED_SIZE);
    UNREFERENCED_PARAMETER(SystemHiveHandle);

    Buf = NULL;
    Status = STATUS_SUCCESS;
    ResultCode = BootEntropySourceSuccess;
    EntropyResult->SourceId = BootEntropySourceRdrand;
    if(SymCryptRdrandStatus() != SYMCRYPT_NO_ERROR && SymCryptRdseedStatus() != SYMCRYPT_NO_ERROR) {
        ResultCode = BootEntropySourceNotPresent;
        goto Cleanup;
    }

    Buf = BlMmAllocateHeap(3 * SYMCRYPT_RDRAND_RESEED_SIZE);
    if(Buf == NULL) {
        ResultCode = BootEntropySourceError;
        Status = STATUS_NO_MEMORY;
        goto Cleanup;
    }
    EntropyResult->EntropyLength = SYMCRYPT_SHA512_RESULT_SIZE;

    if(SymCryptRdseedStatus() == SYMCRYPT_NO_ERROR) {

        //
        // We get 128 bytes from Rdseed, and then hash it down to 64 bytes.
        // This protects against small biasses that are left from the entropy source.
        // The elementary source always has biases, and the HW bias removal is limited.
        //

        SymCryptRdseedGet(Buf, 2 * SYMCRYPT_SHA512_RESULT_SIZE);
        SymCryptSha512(Buf, 2 * SYMCRYPT_SHA512_RESULT_SIZE, EntropyResult->EntropyData);
    } else {
        SymCryptRdrandGet(Buf, 3 * SYMCRYPT_RDRAND_RESEED_SIZE, EntropyResult->EntropyData);
    }


Cleanup:
    if (Buf != NULL) {
        BlMmFreeHeap(Buf);
        Buf = NULL;
    }
    EntropyResult->ResultCode = ResultCode;
    EntropyResult->ResultStatus = Status;
}

#endif // _X86_ or _AMD64_

#if defined(EFI)

VOID
OslpGatherUefiEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    )

/*++

Routine Description:

    This routine gathers entropy from the UEFI entropy protocol

Arguments:

    SystemHiveHandle - Supplies a Handle to the system hive

    EntropyResult - Supplies a pointer to a buffer that receives
        the results of the entropy operation.

Return Value:

    None

--*/

{

    EFI_HANDLE EfiHandle;
    EFI_HANDLE *HandleArray;
    UINTN NumberDevices;
    EFI_RNG_PROTOCOL *ProtocolPointer;
    BOOT_ENTROPY_SOURCE_RESULT_CODE ResultCode;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(SystemHiveHandle);

    HandleArray = NULL;
    ProtocolPointer = NULL;
    EfiHandle = NULL;
    Status = STATUS_SUCCESS;
    ResultCode = BootEntropySourceSuccess;
    EntropyResult->SourceId = BootEntropySourceUefi;
    NumberDevices = 0;

    //
    // Determine if any RNG interfaces exist. If more than one handle is
    // returned, use the first handle of the list.
    //

    Status = EfiLocateHandleBuffer(ByProtocol,
                                   &EfiRngProtocol,
                                   NULL,
                                   &NumberDevices,
                                   &HandleArray);

    if (!NT_SUCCESS(Status) || NumberDevices == 0) {
        ResultCode = BootEntropySourceNotPresent;
        goto Cleanup;
    }

    EfiHandle = HandleArray[0];

    //
    // We now have a handle to the EFI interface. Query for the
    // protocol.
    //

    Status = EfiOpenProtocol(EfiHandle,
                             &EfiRngProtocol,
                             &ProtocolPointer);

    if (!NT_SUCCESS(Status)) {
        ResultCode = BootEntropySourceNotPresent;
        goto Cleanup;
    }

    Status = EfiRngGetRng(ProtocolPointer,
                          BYTES_FROM_EFI_RNG,
                          EntropyResult->EntropyData);

    if (!NT_SUCCESS(Status)) {
        ResultCode = BootEntropySourceError;
        goto Cleanup;
    }

    EntropyResult->EntropyLength = BYTES_FROM_EFI_RNG;

Cleanup:

    EntropyResult->ResultCode = ResultCode;
    EntropyResult->ResultStatus = Status;

    if (ProtocolPointer != NULL) {
        EfiCloseProtocol(EfiHandle,
                         &EfiRngProtocol,
                         ProtocolPointer);

        ProtocolPointer = NULL;
        EfiHandle = NULL;
    }

    if (HandleArray != NULL) {
        BlMmFreeHeap(HandleArray);
        HandleArray = NULL;
    }
}

#endif // EFI

#if defined (NTKERNEL)
VOID
OslpGatherCngEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_SOURCE_LDR_RESULT EntropyResult
    )

/*++

Routine Description:

    This routine gathers entropy from CNG during soft reboot

Arguments:

    SystemHiveHandle - Supplies a Handle to the system hive

    EntropyResult - Supplies a pointer to a buffer that receives
        the results of the entropy operation.

Return Value:

    None

--*/

{
    BOOT_ENTROPY_SOURCE_RESULT_CODE ResultCode;
    NTSTATUS Status;

    UNREFERENCED_PARAMETER(SystemHiveHandle);

    Status = STATUS_SUCCESS;
    ResultCode = BootEntropySourceSuccess;
    EntropyResult->SourceId = BootEntropySourceCng;

    EntropyResult->EntropyLength = SYMCRYPT_SHA512_RESULT_SIZE;
    SystemPrng(EntropyResult->EntropyData, SYMCRYPT_SHA512_RESULT_SIZE);

//Cleanup:
    EntropyResult->ResultCode = ResultCode;
    EntropyResult->ResultStatus = Status;
}

#endif


VOID
OslpGatherEntropySources (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_LDR_RESULT EntropyResult
    )

/*++

Routine Description:

    This routine gathers entropy from all the available sources.

Arguments:

    SystemHiveHandle - Supplies a Handle to the system hive

    EntropyResult - Supplies a pointer to a buffer that receives
        the results of the entropy operation.

Return Value:

    None

--*/

{

    ULONG Cnt;
    PBOOT_ENTROPY_SOURCE_LDR_RESULT SourceResult;
    ULONGLONG StartTime;
    ULONGLONG StopTime;

    RtlZeroMemory(EntropyResult, sizeof(*EntropyResult));
    EntropyResult->maxEntropySources = BootMaxEntropySources;

    //
    // Call each of the entorpy gathering functions in turn.
    //

    for (Cnt = 0; Cnt < N_SOURCE_GATHER_FUNCTIONS; Cnt++) {

        C_ASSERT(N_SOURCE_GATHER_FUNCTIONS <= BootMaxEntropySources);

        SourceResult = &EntropyResult->EntropySourceResult[Cnt];
        StartTime = BlArchGetPerformanceCounter();
        (*OslpEntropySourceGatherFunctions[Cnt])(SystemHiveHandle,
                                                 SourceResult);

        StopTime = BlArchGetPerformanceCounter();
        SourceResult->Time = StopTime - StartTime;
    }
}


NTSTATUS
OslGatherEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_LDR_RESULT EntropyResult,
    __out_bcount_opt(BOOT_SEED_BYTES_FOR_VSM) PBYTE EntropyForVsm,
    __out_bcount_opt(BOOT_SEED_BYTES_FOR_IDK) PBYTE EntropyForIdk,
    __out_bcount_opt(VSM_LKEY_LENGTH + BL_TPM_SEALED_BLOB_KEY_LENGTH_BYTES) PBYTE VsmLKeyCandidate
    )
/*++

Routine Description:

    This routine performs all the entropy gathering tasks in Boot

Arguments:

    SystemHiveHandle - Supplies a Handle to the system hive

    EntropyResult - Supplies a pointer to the structure that receives
        the results of the entropy operation.

    EntropyForVsm - Supplies a pointer to a buffer to receive entropy for VSM.

    EntropyForIdk - Supplies a pointer to a buffer to receive entropy for IDK generation.

    VsmLKeyCandidate - Supplies a pointer to a buffer to receive random bytes
                       which can then be used as VSM Local enryption key.

Return Value:

    STATUS_SUCCESS or error value.

--*/

{

    ULONG Cnt;
    PBOOT_ENTROPY_SOURCE_LDR_RESULT Result;
    SYMCRYPT_ERROR ScError;
    BYTE Seed[SYMCRYPT_SHA512_RESULT_SIZE];
    SYMCRYPT_SHA512_STATE Sha512;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;

    //
    // Gather the information from all the sources.
    //

    OslpGatherEntropySources( SystemHiveHandle, EntropyResult );

    //
    // Algorithm self test
    //
    SymCryptHmacSha512Selftest();
    SymCryptRngAesInstantiateSelftest();
    SymCryptRngAesReseedSelftest();
    SymCryptRngAesGenerateSelftest();

    //
    // Hash the entropy contributions together using SHA-512.
    //

    SymCryptSha512Init(&Sha512);
    for (Cnt = 0; Cnt < BootMaxEntropySources; Cnt++) {
        Result = &EntropyResult->EntropySourceResult[Cnt];
        SymCryptSha512Append(&Sha512,
                             &Result->EntropyData[0],
                             Result->EntropyLength);
    }

    SymCryptSha512Result(&Sha512, Seed);

    //
    // Seed an SP800-90 AES-CTR DRBG in FIPS mode.
    //

    ScError = SymCryptRngAesFips140_2Instantiate(
                    &SymCryptRng,
                    Seed,
                    SYMCRYPT_RNG_AES_MIN_INSTANTIATE_SIZE);

    if (ScError != SYMCRYPT_NO_ERROR) {
        Status = STATUS_UNSUCCESSFUL;
        goto cleanup;
    }

    //
    // Generate random data for CNG and for Ntoskrnl.
    //

    SymCryptRngAesFips140_2Generate(&SymCryptRng,
                                    &EntropyResult->SeedBytesForCng[0],
                                    sizeof(EntropyResult->SeedBytesForCng));

    SymCryptRngAesFips140_2Generate(&SymCryptRng,
                                    &EntropyResult->RngBytesForNtoskrnl[0],
                                    sizeof(EntropyResult->RngBytesForNtoskrnl));

    //
    // Generate random data for VSM and IDK generation.
    //
    
    if (EntropyForVsm != NULL) {
        SymCryptRngAesFips140_2Generate(&SymCryptRng,
                                        EntropyForVsm,
                                        BOOT_SEED_BYTES_FOR_VSM);
    }

    if (EntropyForIdk != NULL) {
        SymCryptRngAesFips140_2Generate(&SymCryptRng,
                                        EntropyForIdk,
                                        BOOT_SEED_BYTES_FOR_IDK);
    }

    if (VsmLKeyCandidate != NULL) {
        SymCryptRngAesFips140_2Generate(&SymCryptRng,
                                        VsmLKeyCandidate,
                                        VSM_LKEY_LENGTH + BL_TPM_SEALED_BLOB_KEY_LENGTH_BYTES);
    }

    SymCryptRngInitialized = TRUE;

cleanup:
    RtlSecureZeroMemory(Seed, sizeof(Seed));
    return Status;
}

