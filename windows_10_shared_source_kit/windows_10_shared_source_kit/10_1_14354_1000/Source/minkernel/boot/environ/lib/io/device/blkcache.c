/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    blkcache.c

Abstract:

    Implements a cache for Block I/O Device reads.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "block.h"

//
// ---------------------------------------------------------------- Definitions
//

//
// Enable checksum verification of the data in the cache on CHK builds.
//

#if DBG

#define BC_CHECKSUM 1

#endif

//
// Define the default and initial number of blocks that the block allocator
// will allocate.
//

#define BLOCK_CACHE_ALLOCATOR_DEFAULT_SIZE (_512KB >> PAGE_SHIFT)
#define BLOCK_CACHE_ALLOCATOR_INITIAL_SIZE (_512KB >> PAGE_SHIFT)

//
// Define the maximum number of entries in the cache entry list.
//

#define BLOCK_CACHE_HASH_TABLE_SIZE 50

// ------------------------------------------------------------ Data Structures

typedef struct _BLOCK_CACHE_KEY {
    ULONGLONG Lbo;
    PDEVICE_ENTRY Device;
} BLOCK_CACHE_KEY, *PBLOCK_CACHE_KEY;

/*++

Block Cache Key Description:

    Describes a key in the block cache hash table.

Fields:

    Lbo - This field contains the logical block offset for this cache entry.

    Device - A pointer to the device entry for the device on which the I/O
        occurred.

--*/

typedef struct _BLOCK_CACHE_DATA {
    PVOID Buffer;
    ULONG BufferSize;

#if defined(BC_CHECKSUM)

    ULONG Checksum;

#endif

} BLOCK_CACHE_DATA, *PBLOCK_CACHE_DATA;

/*++

Block Cache Data Description:

    Describes the data in the block cache hash table.

Fields:

    Buffer - Buffer containing the cached contents of a disk read on the
        specified device at the specified location and size.

    BufferSize - This field contains the size (in bytes) of the buffer.

--*/

// -------------------------------------------------------------------- Globals

BLOCK_ALLOCATOR_HANDLE BcpBlockAllocatorHandle =
    BLOCK_ALLOCATOR_INVALID_HANDLE;

#if DBG

ULONG BcpCacheHits;
ULONG BcpCacheMisses;

#endif

HASH_TABLE_ID BcpHashTableId = INVALID_HASH_TABLE_ID;

// ----------------------------------------------------------------- Prototypes

BOOLEAN
BcpCompareKey (
    __in PHASH_KEY Key1,
    __in PHASH_KEY Key2
    );

NTSTATUS
BcpDestroy (
    VOID
    );

VOID
BcpDestroyEntry (
    __in_opt PBLOCK_CACHE_KEY CacheKey,
    __in_opt PBLOCK_CACHE_DATA CacheData
    );

ULONG
BcpHashFunction (
    __in PHASH_KEY Key,
    __in ULONG HashTableSize
    );

VOID
BcpPurgeCacheEntries (
    VOID
    );

#if defined(BC_CHECKSUM)

BOOLEAN
BcpVerifyChecksum (
    __in PBLOCK_CACHE_KEY CacheKey,
    __in PBLOCK_CACHE_DATA CacheData
    );

#endif

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
BcInitialize (
    VOID
    )

/*++

Routine Description:

    Initializes global data structures required for the block cache.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if unsuccessful in allocating internal memory.

--*/

{

    NTSTATUS Status;

    Status = BlHtCreate(BLOCK_CACHE_HASH_TABLE_SIZE,
                        BcpHashFunction,
                        BcpCompareKey,
                        &BcpHashTableId);

    if (!NT_SUCCESS(Status)) {
        goto InitializeEnd;
    }

#if DBG

    //
    // Maintain counters used for performance analysis.
    //

    BcpCacheHits = 0;
    BcpCacheMisses = 0;

#endif

    //
    // Initialize the block allocator.
    //

    BcpBlockAllocatorHandle = BlpMmCreateBlockAllocator(
                                  VirtualPages,
                                  MEMORY_TYPE_BLOCK_CACHE_DATA,
                                  MEMORY_ATTRIBUTE_NONE,
                                  1,
                                  BLOCK_CACHE_ALLOCATOR_INITIAL_SIZE,
                                  BLOCK_CACHE_ALLOCATOR_DEFAULT_SIZE,
                                  PAGE_SIZE);

    if (BcpBlockAllocatorHandle == BLOCK_ALLOCATOR_INVALID_HANDLE) {
        Status = STATUS_UNSUCCESSFUL;
        goto InitializeEnd;
    }

    //
    // Register a destruction routine for cleaning up the global state of
    // the block cache during library termination.
    //

    Status = BlpIoRegisterDestroyRoutine(BcpDestroy);
    if (!NT_SUCCESS(Status)) {
        goto InitializeEnd;
    }

InitializeEnd:
    if (!NT_SUCCESS(Status)) {
        if (BcpHashTableId != INVALID_HASH_TABLE_ID) {
            BlHtDestroy(BcpHashTableId);
            BcpHashTableId = INVALID_HASH_TABLE_ID;
        }

        if (BcpBlockAllocatorHandle != BLOCK_ALLOCATOR_INVALID_HANDLE) {
            BlpMmDeleteBlockAllocator(BcpBlockAllocatorHandle);
        }
    }

    return Status;
}

NTSTATUS
BcReadFromCache (
    __in PDEVICE_ENTRY DeviceEntry,
    __out_bcount(Length) PVOID Buffer,
    __in ULONGLONG DeviceLbo,
    __in ULONG Length
    )

/*++

Routine Description:

    Attempts to use the block cache to perform the specified device read.

    The caller is responsible for issuing requests within cache boundaries.

Arguments:

    DeviceEntry - A pointer to the device entry for the device on which to
        perform the read operation.

    Buffer - Supples a memory buffer to copy the contents of the cached device
        read.

    DeviceLbo - The logical byte offset on the device for the read operation.

    Length - The length (in bytes) of the read request.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NOT_FOUND if the specified read is not present in the block cache.

--*/

{

    PBLOCK_CACHE_DATA CacheData;
    BLOCK_CACHE_KEY CacheKey;
    PHASH_DATA HashData;
    HASH_KEY HashKey;
    NTSTATUS Status;

    ASSERT(ALIGN_RANGE_DOWN(DeviceLbo, BLOCK_CACHE_READ_SIZE) ==
           ALIGN_RANGE_DOWN(DeviceLbo + Length - 1, BLOCK_CACHE_READ_SIZE));

    //
    // Create a cache key that looks like one that would already be in the hash
    // table. The LBO should be aligned down.
    //

    CacheKey.Lbo = ALIGN_RANGE_DOWN(DeviceLbo, BLOCK_CACHE_READ_SIZE);
    CacheKey.Device = DeviceEntry;
    HashKey.Size = sizeof(BLOCK_CACHE_KEY);
    HashKey.Flags = HASH_KEY_FLAGS_NONE;
    HashKey.Value = &CacheKey;
    Status = BlHtLookup(BcpHashTableId, &HashKey, &HashData);
    if (!NT_SUCCESS(Status)) {
        goto ReadFromCacheEnd;
    }

    CacheData = (PBLOCK_CACHE_DATA)HashData->Value;

#if defined(BC_CHECKSUM)

    NT_VERIFY(BcpVerifyChecksum(&CacheKey, CacheData));

#endif

    //
    // If successful, copy the cached data to the caller's buffer.
    //

    RtlCopyMemory(Buffer,
                  Add2Ptr(CacheData->Buffer, DeviceLbo - CacheKey.Lbo),
                  Length);

ReadFromCacheEnd:

    //
    // Maintain global counters for performance analysis of the block cache.
    //

#if DBG

    if (Status == STATUS_SUCCESS) {
        BcpCacheHits += 1;

    } else {
        BcpCacheMisses += 1;
    }

#endif

    return Status;
}


NTSTATUS
BcCacheRead (
    __in PDEVICE_ENTRY DeviceEntry,
    __in ULONGLONG DeviceLbo,
    __deref_out_bcount(BLOCK_CACHE_READ_SIZE) PVOID *CacheBuffer
    )

/*++

Routine Description:

    Performs the specified device read, caching the results in the block
    cache.

    The caller is responsible for breaking up device reads according to
    cache boundaries.

    N.B. This routine guarantees that the internal fields, LbaOffset and
         BlockOffset, will be the same on return as on entrance.

Arguments:

    DeviceEntry - A pointer to the device entry for the device on which to
        perform the read operation.

    DeviceLbo - The logical byte offset on the device for the read operation.

    Length - The length (in bytes) of the read request.

    CacheBuffer - Supplies an address to store the address of the buffer
        allocated to cache the contents of the device read.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if a requested block is invalid.
    STATUS_IO_DEVICE_ERROR if the device reported an error when performing
        the requested operation.
    STATUS_UNEXPECTED_IO_ERROR if an unexpected I/O error occurs when
        performing the requested operation.

--*/

{

    BLOCK_MEMORY_ADDRESS BlockAddress;
    PBLOCK_IO_DEVICE BlockIoDevice;
    PBLOCK_IO_INFORMATION BlockIoInformation;
    ULONGLONG BlockLbo;
    PBLOCK_CACHE_DATA CacheData;
    PBLOCK_CACHE_KEY CacheKey;
    ULONG EntrySize;
    HASH_KEY HashKey;
    ULONG OriginalBlockOffset;
    ULONGLONG OriginalLbaOffset;
    NTSTATUS Status;

    ASSERT(DeviceLbo == ALIGN_RANGE_DOWN(DeviceLbo, BLOCK_CACHE_READ_SIZE));

    CacheKey = NULL;
    CacheData = NULL;

    //
    // Preserve the value of LbaOffset and BlockOffset to ensure similar
    // calling semantics between BcCacheRead and BcReadFromCache.
    //

    BlockIoDevice = DeviceEntry->DeviceData;
    BlockIoInformation = &BlockIoDevice->Information;
    OriginalBlockOffset = BlockIoInformation->BlockOffset;
    OriginalLbaOffset = BlockIoInformation->LbaOffset;

    //
    // Allocate a cache entry and cache data for the requested device read.
    //

    CacheKey = BlMmAllocateHeap(sizeof(BLOCK_CACHE_KEY));
    if (CacheKey == NULL) {
        Status = STATUS_NO_MEMORY;
        goto CacheReadEnd;
    }

    CacheData = BlMmAllocateHeap(sizeof(BLOCK_CACHE_DATA));
    if (CacheData == NULL) {
        Status = STATUS_NO_MEMORY;
        goto CacheReadEnd;
    }

    EntrySize = ALIGN_RANGE_UP(BLOCK_CACHE_READ_SIZE, PAGE_SIZE);
    CacheData->Buffer = NULL;
    BlockAddress.VirtualAddress = NULL;
    CacheData->BufferSize = EntrySize;
    Status = BlpMmAllocateMemoryBlocks(BcpBlockAllocatorHandle,
                                       EntrySize >> PAGE_SHIFT,
                                       &BlockAddress);

    if (!NT_SUCCESS(Status)) {
        goto CacheReadEnd;
    }

    CacheData->Buffer = BlockAddress.VirtualAddress;

    //
    // Perform the requested physical disk read.
    //

    BlockLbo = ALIGN_RANGE_DOWN(DeviceLbo, BlockIoInformation->BlockSize);
    BlockIoInformation->LbaOffset = BlockLbo / BlockIoInformation->BlockSize;
    BlockIoInformation->BlockOffset = (ULONG)(DeviceLbo - BlockLbo);
    Status = BlockIoRead(DeviceEntry,
                         CacheData->Buffer,
                         BLOCK_CACHE_READ_SIZE,
                         NULL);

    if (!NT_SUCCESS(Status)) {
        goto CacheReadEnd;
    }

    //
    // Compute checksum for the data.
    //

#if defined(BC_CHECKSUM)

    CacheData->Checksum = BlUtlCheckSum(0,
                                        CacheData->Buffer,
                                        CacheData->BufferSize,
                                        CHECKSUM_BYTE_ARRAY);

#endif

    //
    // Insert the cached entry into the block cache.
    //

    CacheKey->Lbo = DeviceLbo;
    CacheKey->Device = DeviceEntry;
    HashKey.Size = sizeof(BLOCK_CACHE_KEY);
    HashKey.Value = CacheKey;
    HashKey.Flags = HASH_KEY_FLAGS_NONE;
    Status = BlHtStore(BcpHashTableId,
                       &HashKey,
                       CacheData,
                       sizeof(BLOCK_CACHE_DATA));

    if (!NT_SUCCESS(Status)) {
        goto CacheReadEnd;
    }

    *CacheBuffer = CacheData->Buffer;

CacheReadEnd:
    if (!NT_SUCCESS(Status)) {
        BcpDestroyEntry(CacheKey, CacheData);
    }

    BlockIoInformation->BlockOffset = OriginalBlockOffset;
    BlockIoInformation->LbaOffset = OriginalLbaOffset;
    return Status;
}

VOID
BcFlushRangeInCache (
    __in PDEVICE_ENTRY DeviceEntry,
    __in ULONGLONG DeviceLbo,
    __in ULONG Length
    )

/*++

Routine Description:

    Removes all block cache entries describing a read within the specified
    parameters.

Arguments:

    DeviceEntry - A pointer to the device entry for the device on which to
        perform the flush operation.

    DeviceLbo - The logical byte offset on the device to start the flush
        operation.

    Length - The length (in bytes) of disk reads to flush.

Return Value:

    None.

--*/

{

    BLOCK_CACHE_KEY CacheKey;
    ULONGLONG CurrentDeviceLbo;
    PBLOCK_CACHE_DATA DataValue;
    ULONGLONG EndDeviceLbo;
    PHASH_DATA HashData;
    HASH_KEY HashKey;
    PBLOCK_CACHE_KEY KeyValue;
    NTSTATUS Status;

    CurrentDeviceLbo = ALIGN_RANGE_DOWN(DeviceLbo, BLOCK_CACHE_READ_SIZE);
    EndDeviceLbo = DeviceLbo + Length;
    while (CurrentDeviceLbo < EndDeviceLbo) {
        CacheKey.Lbo = CurrentDeviceLbo;
        CacheKey.Device = DeviceEntry;
        HashKey.Size = sizeof(BLOCK_CACHE_KEY);
        HashKey.Flags = HASH_KEY_FLAGS_NONE;
        HashKey.Value = &CacheKey;
        Status = BlHtLookup(BcpHashTableId, &HashKey, &HashData);
        if (!NT_SUCCESS(Status)) {
            CurrentDeviceLbo += BLOCK_CACHE_READ_SIZE;
            continue;
        }

        //
        // If an entry was found, delete it.
        //
        // N.B. BlHtLookup does not return a copy of HashData and HasKey like
        //      BlHtGetNext does, so it is necessary to record the internal
        //      pointers before call BlHtDelete. This should be fixed so both
        //      routines operate in the same way.
        //

        KeyValue = HashKey.Value;
        DataValue = HashData->Value;
        BlHtDelete(BcpHashTableId, &HashKey);
        BcpDestroyEntry(KeyValue, DataValue);
        CurrentDeviceLbo += BLOCK_CACHE_READ_SIZE;
    }

    return;
}

NTSTATUS
BcPurge (
    VOID
    )

/*++

Routine Description:

    This routine purges all entries from the block cache, and frees their
    associated memory allocations.

Arguments:

    None.

Return Value:

    Always returns STATUS_SUCCESS.

--*/

{

    BcpPurgeCacheEntries();

    //
    // The purge is normally issued in order to maximize the available memory.
    // Notify the block allocator as to not pre-allocate any memory blocks (and
    // thus maximize available memory) by setting the default blocks to zero.
    //

    BlpMmSetBlockAllocatorDefaultBlocks(BcpBlockAllocatorHandle, 0);
    return STATUS_SUCCESS;
}

BOOLEAN
BcpPurgeCallback (
    __in PHASH_KEY HashKey,
    __in PHASH_DATA HashData,
    __in_opt PVOID CallbackContext
    )

/*++

Routine Description:

    A callback function used to free block cache entries with matching device.

Arguments:

    HashKey - Supplies a pointer to the hash key stored in the entry.

    HashData - Supplies a pointer to hash data stored in the entry.

    CallbackContext - Supplies context value for the callback.

Return Value:

    TRUE if the entry should be removed. FALSE otherwise.

--*/

{

    PBLOCK_CACHE_KEY CacheKey;
    PDEVICE_ENTRY DeviceEntry;

    //
    // Check if the entry has matching device, and free the data if it does.
    //

    DeviceEntry = (PDEVICE_ENTRY)CallbackContext;
    CacheKey = (PBLOCK_CACHE_KEY)HashKey->Value;
    if (CacheKey->Device == DeviceEntry) {
        BcpDestroyEntry((PBLOCK_CACHE_KEY)HashKey->Value, (PBLOCK_CACHE_DATA)HashData->Value);
        return TRUE;
    }

    return FALSE;
}

VOID
BlockIoPurgeCacheByDevice (
    __in PDEVICE_ENTRY DeviceEntry
    )

/*++

Routine Description:

    This routine is used to free any cache entries associated with a device
    going away.

Arguments:

    DeviceEntry - Supplies a pointer to the device entry to match.

Return Value:

    None.

--*/

{

    if (BcpHashTableId != INVALID_HASH_TABLE_ID) {
        BlHtEnumerateAndRemoveEntries(BcpHashTableId,
                                      BcpPurgeCallback,
                                      DeviceEntry);
    }

    return;
}

// ------------------------------------------------------------ Local Functions

BOOLEAN
BcpCompareKey (
    __in PHASH_KEY Key1,
    __in PHASH_KEY Key2
    )

/*++

Routine Description:

    This routine compares two block cache hash table keys.

Arguments:

    Key1 - Supplies a pointer to a hash key.

    Key2 - Supplies a pointer to a second hash key.

Return Value:

    TRUE is the keys are identical. FALSE otherwise.

--*/

{

    PBLOCK_CACHE_KEY BlockKey1;
    PBLOCK_CACHE_KEY BlockKey2;

    ASSERT(Key1 != NULL);
    ASSERT(Key2 != NULL);

    BlockKey1 = (PBLOCK_CACHE_KEY)Key1->Value;
    BlockKey2 = (PBLOCK_CACHE_KEY)Key2->Value;

    //
    // For two keys to be the same, they must be the same size, have the same
    // flags, have the same LBO, and have the same device.
    //

    if ((Key1->Size != Key2->Size) ||
        (Key1->Flags != Key2->Flags) ||
        (BlockKey1->Lbo != BlockKey2->Lbo) ||
        (BlockKey1->Device != BlockKey2->Device)) {

        return FALSE;
    }

    return TRUE;
}

NTSTATUS
BcpDestroy (
    VOID
    )

/*++

Routine Description:

    Destroys all global data structures required for the block cache.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    NTSTATUS Status;

    BcpPurgeCacheEntries();

    //
    // Tear down the block allocator instance that was created.
    //

    Status = BlpMmDeleteBlockAllocator(BcpBlockAllocatorHandle);
    return Status;
}

VOID
BcpDestroyEntry (
    __in_opt PBLOCK_CACHE_KEY CacheKey,
    __in_opt PBLOCK_CACHE_DATA CacheData
    )

/*++

Routine Description:

    This routine frees the resources associated with a block cach entry.

Arguments:

    CacheKey - Supplies a pointer to the entry to be removed from the cache.

Return Value:

    None.

--*/

{

    BLOCK_MEMORY_ADDRESS BlockAddress;
    ULONG NumberOfBlocks;

    if (CacheData != NULL) {
        if (CacheData->Buffer != NULL) {
            BlockAddress.VirtualAddress = CacheData->Buffer;
            NumberOfBlocks = CacheData->BufferSize >> PAGE_SHIFT;
            BlpMmFreeMemoryBlocks(BcpBlockAllocatorHandle,
                                  BlockAddress,
                                  NumberOfBlocks);
        }

        BlMmFreeHeap(CacheData);
    }

    if (CacheKey != NULL) {
        BlMmFreeHeap(CacheKey);
    }

    return;
}

ULONG
BcpHashFunction (
    __in PHASH_KEY Key,
    __in ULONG HashTableSize
    )

/*++

Routine Description:

    This routine maps a block cache key to a bucket by summing the value of
    each byte in the key's LBO and Device pointer and returning the result
    modulo HashTableSize.

Arguments:

    Key - Supplies the key for the object to hash.

    HashTableSize - Supplies the size of the hash table.

Return Value:

    A value between zero and HashTableSize - 1.

--*/

{

    PBLOCK_CACHE_KEY CacheKey;
    ULONG HashIndex;
    ULONG Index;
    ULONG Sum;
    PUCHAR UcharPtr;

    ASSERT(Key != NULL);
    ASSERT(Key->Size != 0);
    ASSERT(Key->Value != NULL);

    CacheKey = (PBLOCK_CACHE_KEY)Key->Value;

    //
    // Sum the bytes of the LBO.
    //

    Sum = 0;
    UcharPtr = (PUCHAR)&CacheKey->Lbo;
    for (Index = 0; Index < sizeof(ULONGLONG); Index += 1) {
        Sum += UcharPtr[Index];
    }

    //
    // Sum the bytes of the Device pointer.
    //

    UcharPtr = (PUCHAR)&CacheKey->Device;
    for (Index = 0; Index < sizeof(PDEVICE_ENTRY); Index += 1) {
        Sum += UcharPtr[Index];
    }

    HashIndex = Sum % HashTableSize;
    return HashIndex;
}

VOID
BcpPurgeCacheEntries (
    VOID
    )

/*++

Routine Description:

    This routine walks through the cache entry list and deletes each cache
    entry.

Arguments:

    None.

Return Value:

    None.

--*/

{

    HASH_DATA HashData;
    HASH_KEY HashKey;
    NTSTATUS Status;

    for (;;) {
        Status = BlHtGetNext(BcpHashTableId, NULL, &HashKey, &HashData);
        if (!NT_SUCCESS(Status)) {
            break;
        }

        BlHtDelete(BcpHashTableId, &HashKey);
        BcpDestroyEntry((PBLOCK_CACHE_KEY)HashKey.Value,
                        (PBLOCK_CACHE_DATA)HashData.Value);
    }

    return;
}

#if defined(BC_CHECKSUM)

BOOLEAN
BcpVerifyChecksum (
    __in PBLOCK_CACHE_KEY CacheKey,
    __in PBLOCK_CACHE_DATA CacheData
    )

/*++

Routine Description:

    This routine validates checksum value for the supplied entry in the block
    level cache.

Arguments:

    CacheKey - Supplies a pointer to the key for the entry.

    CacheData - Supplies a pointer for the entry to validate.

Return Value:

    Boolean value indicating if checksum of the buffer matches the expected
    value.

--*/

{

    ULONG Checksum;

    Checksum = BlUtlCheckSum(0,
                             CacheData->Buffer,
                             CacheData->BufferSize,
                             CHECKSUM_BYTE_ARRAY);

    if (Checksum != CacheData->Checksum) {
        BlStatusPrint(L"BlockCache checksum mismatch: DeviceId %d Lbo %I64d\n",
                      CacheKey->Device->DeviceId,
                      CacheKey->Lbo);

        BlStatusError(BL_ERROR_BLOCK_CACHE,
                      (ULONG_PTR)CacheKey,
                      (ULONG_PTR)CacheData,
                      Checksum,
                      CacheData->Checksum);

        return FALSE;
    }

    return TRUE;
}

#endif

