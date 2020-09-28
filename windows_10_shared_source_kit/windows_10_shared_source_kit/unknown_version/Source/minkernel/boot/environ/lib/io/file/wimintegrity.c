/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    wimintegrity.c

Abstract:

    Implementation of integrity for the boot library's WIM filter.

Environment:

    boot

--*/

// ------------------------------------------------------------------- Includes

#include <bootlib.h>
#include <symcrypt.h>
#include "wimboot.h"

#ifndef FILE_MIN

// ---------------------------------------------------------------- Definitions

//
// Load upto 64k of hashes at a time, enough to validate 8MB of raw blocks.
// The cache must be page aligned.
//

#define WIM_INTEGRITY_HASH_CACHE_SIZE \
    ((_8MB / WIM_INTEGRITY_BLOCK_LENGTH) * WIM_INTEGRITY_BLOCK_HASH_LENGTH)

C_ASSERT(WIM_INTEGRITY_HASH_CACHE_SIZE ==
         ALIGN_RANGE_DOWN(WIM_INTEGRITY_HASH_CACHE_SIZE, PAGE_SIZE));

#define WIM_INTEGRITY_HASH_CACHE_PAGES \
            (WIM_INTEGRITY_HASH_CACHE_SIZE / PAGE_SIZE)

#define WIM_INTEGRITY_HASH_CACHE_ENTRIES_COUNT \
            (WIM_INTEGRITY_HASH_CACHE_SIZE / WIM_INTEGRITY_BLOCK_HASH_LENGTH)

//
// We divide by block length and then multiply by hash length, so ensure this
// is safe from overflow.
//

C_ASSERT(WIM_INTEGRITY_BLOCK_LENGTH >= WIM_INTEGRITY_BLOCK_HASH_LENGTH);

//
// Block hash computation and result contexts.
//

typedef struct _WIM_INTEGRITY_HASH_BLOCK
{
    UCHAR Results[WIM_INTEGRITY_BLOCK_CHUNKS][SYMCRYPT_SHA256_RESULT_SIZE];

    union
    {
        struct _WIM_INTEGRITY_PARALLEL_HASH
        {
            SYMCRYPT_SHA256_STATE States[WIM_INTEGRITY_BLOCK_CHUNKS];
            SYMCRYPT_PARALLEL_HASH_OPERATION Operations[2*WIM_INTEGRITY_BLOCK_CHUNKS];
            BYTE Scratch[SYMCRYPT_PARALLEL_SHA256_FIXED_SCRATCH + WIM_INTEGRITY_BLOCK_CHUNKS * SYMCRYPT_PARALLEL_HASH_PER_STATE_SCRATCH];
        } Parallel;

        SYMCRYPT_SHA256_STATE Single;
    } Type;

} WIM_INTEGRITY_HASH_BLOCK, *PWIM_INTEGRITY_HASH_BLOCK;

// -------------------------------------------------------------------- Globals

//
// Debug counters.
//

ULONG WimBlocksValidated = 0;
ULONGLONG WimBlocksAlreadyValidated = 0;

//
// Buffer to perform aligned reads into when the caller's buffer cannot be
// used. Since this is allocated as memory type MEMORY_TYPE_DEVICE_DATA,
// it will automatically get scavanged at library exit.
//

PVOID WimIntegrityReadBuffer = NULL;
ULONG WimIntegrityReadBufferSize = 0;

// ------------------------------------------------------------------ Functions

NTSTATUS
WimpIntegrityCreateContext (
    _In_ DEVICE_ID WimDeviceId,
    _Out_ PWIM_INTEGRITY_CONTEXT *Integrity
    )
/*++

Routine Description:

    This routine determines if integrity is ready for a WIM and creates a
    context to support integrity verification if so.

Arguments:

    WimDeviceId - Supplies the device id of the WIM to create integrity for.

    Integrity - Supplies a pointer that receives an integrity context if
         integrity is ready for a WIM, or NULL if not.

Return Value:

    STATUS_SUCCESS if successful.

--*/
{
    PBOOT_ENVIRONMENT_DEVICE DeviceIdentifier;
    DEVICE_INFORMATION DeviceInfo;
    FILE_ID FileId;
    ULONG FveBitmap;
    ULONG FveStaticFlags;
    BOOLEAN IntegrityReady;
    BOOLEAN IntegrityValid;
    ULONG OffsetToHashes;
    DEVICE_ID OverlayedDeviceId;
    NTSTATUS Status;
    ULONGLONG WimFileSize;

    FileId = INVALID_FILE_ID;

    //
    // Open the integrity file for this WIM.
    //

    Status = WimpIntegrityOpenIntegrityFile(WimDeviceId, &FileId, &OverlayedDeviceId);
    if (!NT_SUCCESS(Status)) {

        //
        // Run without integrity if the file does not exist. Fail otherwise.
        //

        if (Status == STATUS_NO_SUCH_FILE) {
            *Integrity = NULL;
            Status = STATUS_SUCCESS;
        }

        goto CreateIntegrityContextEnd;
    }

    //
    // If BitLocker is disabled, run without integrity.
    //

    Status = BlGetDeviceIdentifier(OverlayedDeviceId, &DeviceIdentifier);
    if (!NT_SUCCESS(Status)) {
        goto CreateIntegrityContextEnd;
    }

    Status = BlFveQueryStaticDeviceStatus(DeviceIdentifier,
                                          &FveStaticFlags,
                                          &FveBitmap);

    if (!NT_SUCCESS(Status)) {
        goto CreateIntegrityContextEnd;
    }

    if (BlFveBitlockerEnabled(FveStaticFlags) == FALSE) {
        *Integrity = NULL;
        Status = STATUS_SUCCESS;
        goto CreateIntegrityContextEnd;
    }

    //
    // Get the size of the WIM.
    //

    Status = BlDeviceGetInformation(WimDeviceId, &DeviceInfo);
    if (!NT_SUCCESS(Status)) {
        goto CreateIntegrityContextEnd;
    }

    if (DeviceInfo.DeviceType != DEVICE_BLOCK_IO ||
        DeviceInfo.u.BlockIo.Type != BLOCK_IO_FILE ||
        DeviceInfo.u.BlockIo.BlockSize != 1) {

        Status = STATUS_CORRUPT_SYSTEM_FILE;
        goto CreateIntegrityContextEnd;
    }

    Status = RtlULongLongAdd(DeviceInfo.u.BlockIo.LastBlock, 1, &WimFileSize);
    if (!NT_SUCCESS(Status)) {
        goto CreateIntegrityContextEnd;
    }

    //
    // Check that the integrity file is valid.
    //

    Status = WimpIntegrityValidateIntegrityFile(FileId,
                                                WimDeviceId,
                                                WimFileSize,
                                                &IntegrityValid,
                                                &IntegrityReady,
                                                &OffsetToHashes);

    if (!NT_SUCCESS(Status)) {
        goto CreateIntegrityContextEnd;
    }

    if (IntegrityValid == FALSE) {
        Status = STATUS_CORRUPT_SYSTEM_FILE;
        goto CreateIntegrityContextEnd;
    }

    //
    // Run without integrity if it is not complete.
    //

    if (IntegrityReady == FALSE) {
        *Integrity = NULL;
        Status = STATUS_SUCCESS;
        goto CreateIntegrityContextEnd;
    }

    //
    // Build the integrity context for this WIM.
    //

    Status = WimpIntegrityBuildContext(WimFileSize,
                                       FileId,
                                       OffsetToHashes,
                                       Integrity);

    if (!NT_SUCCESS(Status)) {
        goto CreateIntegrityContextEnd;
    }

    FileId = INVALID_FILE_ID;

CreateIntegrityContextEnd:

    if (FileId != INVALID_FILE_ID) {
        BlFileClose(FileId);
    }

    return Status;
}

VOID
WimpIntegrityDestroyContext (
    _Inout_ PWIM_INTEGRITY_CONTEXT Integrity
    )
/*++

Routine Description:

    This routine frees an integrity context.

Arguments:

    Integrity - Supplies a pointer to the context to be freed.

Return Value:

    None.

--*/
{
    if (Integrity->IntegrityFileId != INVALID_FILE_ID) {
        BlFileClose(Integrity->IntegrityFileId);
    }

    BlMmFreePages(Integrity);
}

_Success_(return != FALSE)
BOOLEAN
WimpIntegrityCalculateBlockIndices(
    _In_ PWIM_INTEGRITY_CONTEXT Integrity,
    _In_ ULONGLONG FileOffset,
    _In_ ULONG ReadSize,
    _Out_ PULONG BlockStart,
    _Out_ PULONG BlockCount,
    _Out_opt_ PULONG BlockEnd
    )
/*++

Routine Description:

    This routine calculates the indices within the validation bitmap for an
    aligned read offset and non-aligned read size. If the read size is not
    aligned it must be the last block of the WIM file.

Arguments:

    Integrity - Supplies a pointer to the integrity context for the WIM.

    FileOffset - Supplies the offset within the WIM file of the starting block.

    ReadSize - Supplies the length of the block data being queried.

    BlockStart - Supplies a pointer that receives the starting index of the
         block.

    BlockCount - Supplies a pointer that receives the number of blocks being
         read.

    BlockEnd - Supplies a pointer that receives BlockStart + BlockCount.

Return Value:

    FALSE if there was an error in calculating the indices.

    TRUE if the indices were calculated without error.

--*/
{
    ULONG Count;
    ULONG End;
    ULONGLONG FileSize;
    ULONG Start;
    NTSTATUS Status;

    NT_ASSERT(FileOffset == ALIGN_RANGE_DOWN(FileOffset, WIM_INTEGRITY_BLOCK_LENGTH));

    Status = RtlULongLongToULong(FileOffset / WIM_INTEGRITY_BLOCK_LENGTH, &Start);
    if (!NT_SUCCESS(Status)) {
        return FALSE;
    }

    Count = ALIGN_RANGE_UP(ReadSize, WIM_INTEGRITY_BLOCK_LENGTH);
    if (Count < ReadSize) {
        return FALSE;
    }

    Count /= WIM_INTEGRITY_BLOCK_LENGTH;

    Status = RtlULongAdd(Start, Count, &End);
    if (!NT_SUCCESS(Status)) {
        return FALSE;
    }

    //
    // Since the calculation of End didn't overflow and End != Start, we don't
    // need to check if Start is beyond the bitmap.
    //

    if (End > Integrity->VerifiedBlockBitmap.SizeOfBitMap) {
        return FALSE;
    }

    //
    // If the buffer size is not size aligned this must be the last block.
    //

    if ((ReadSize & (WIM_INTEGRITY_BLOCK_LENGTH - 1)) != 0) {

        //
        // Verify this is the end of the file, and not just an unexpected short
        // read.
        //

        Status = RtlULongLongAdd(FileOffset, ReadSize, &FileSize);
        if (!NT_SUCCESS(Status) || FileSize != Integrity->WimFileSize) {

            WimpIntegrityReportFailure(
                Integrity,
                Integrity->VerifiedBlockBitmap.SizeOfBitMap - 1);

            return FALSE;
        }
    }

    *BlockStart = Start;
    *BlockCount = Count;

    if (BlockEnd != NULL) {
        *BlockEnd = End;
    }

    return TRUE;
}

VOID
WimpIntegrityReportFailure(
    _In_ PWIM_INTEGRITY_CONTEXT Integrity,
    _In_ ULONG FailingBlock
    )
/*++

Routine Description:

    This routine reports integrity failures.

Arguments:

    Integrity - Supplies a pointer to the integrity context for the WIM.

    FailingBlock - Supplies the index of the WIM block that failed validation.

Return Value:

    None.

--*/
{
    UNREFERENCED_PARAMETER(Integrity);

    if (BlBdDebuggerEnabled() != FALSE) {
        BlStatusPrint(L"*** Integrity validation failed for block %u.\r\n",
                      FailingBlock);

        DbgBreakPoint();
    }
}

BOOLEAN
WimpIntegrityAreBlocksValidated (
    _In_ PWIM_INTEGRITY_CONTEXT Integrity,
    _In_ ULONGLONG FileOffset,
    _In_ ULONG ReadSize
    )
/*++

Routine Description:

    This routine checks if blocks of a WIM file have previously been validated.

Arguments:

    Integrity - Supplies a pointer to the integrity context for the WIM.

    FileOffset - Supplies the offset within the WIM file of the starting block.

    ReadSize - Supplies the length of the block data being queried.

Return Value:

    FALSE if not all the blocks have previously passed verification.

    TRUE if all the blocks have previously passed verification.

--*/
{
    ULONG BlockCount;
    ULONG BlockStart;
    BOOLEAN Valid;

    if (ReadSize == 0) {
        return TRUE;
    }

    Valid = WimpIntegrityCalculateBlockIndices(Integrity,
                                               FileOffset,
                                               ReadSize,
                                               &BlockStart,
                                               &BlockCount,
                                               NULL);

    if (Valid != FALSE) {

        //
        // If all the blocks are verified, success.
        // Test the first bit as an optimization.
        //

        if (RtlTestBit(&Integrity->VerifiedBlockBitmap, BlockStart) != FALSE &&
            RtlAreBitsSet(&Integrity->VerifiedBlockBitmap,
                          BlockStart,
                          BlockCount) != FALSE) {

            WimBlocksAlreadyValidated += BlockCount;
            return TRUE;
        }
    }

    return FALSE;
}

VOID
WimpIntegrityGetAlignedSizes(
    _In_ PWIM_INTEGRITY_CONTEXT Integrity,
    _In_ ULONGLONG ReadOffset,
    _In_ ULONG ReadSize,
    _In_ ULONG Alignment,
    _Out_ PULONGLONG AlignedOffset,
    _Out_ PULONG AlignedReadSize,
    _Out_ PULONG Padding
    )
/*++

Routine Description:

    This routine calculates the aligned file offset and length required for
    integrity checking a region of a WIM.

Arguments:

    Integrity - Supplies a pointer to the integrity context for a WIM.

    ReadOffset - Supplies the offset into the WIM that the read begins.

    ReadSize - Supplies the number of bytes being read.

    Alignment - Supplies the default alignment that is required.

    AlignedOffset - Supplies a pointer that receives the aligned offset that
         the read begins.

    AlignedReadSize - Supplies a pointer that receives the aligned number of
         bytes being read.

    Padding - Supplies a pointer that receives the difference of AlignedOffset
         and ByteOffset.

Return Value:

    None.

--*/
{
    UNREFERENCED_PARAMETER(Integrity);

    //
    // Align up the requested alignment to a multiple of the integrity block length.
    //

    Alignment = ALIGN_RANGE_UP(Alignment, WIM_INTEGRITY_BLOCK_LENGTH);

    //
    // Align down the read offset to the start of a WIM block.
    //

    *AlignedOffset = ALIGN_RANGE_DOWN(ReadOffset, Alignment);

    *Padding = (ULONG)(ReadOffset - *AlignedOffset);

    //
    // Align up the read to a multiple of a WIM block.
    //

    *AlignedReadSize = ALIGN_RANGE_UP(ReadSize + *Padding, Alignment);
}

NTSTATUS
WimpIntegrityValidateBlocks (
    _Inout_opt_ PWIM_INTEGRITY_CONTEXT Integrity,
    _In_ ULONGLONG FileOffset,
    _In_reads_bytes_(BufferSize) PVOID Buffer,
    _In_ ULONG BufferSize
)
/*++

Routine Description:

    This routine verifies that blocks of a WIM file are valid.

Arguments:

    Integrity - Supplies a pointer to the integrity context for the WIM.

    FileOffset - Supplies the offset within the WIM file of the starting block.

    Buffer - Supplies a pointer to the block data of the WIM.

    BufferSize - Supplies the length of the block data to be verified.

Return Value:

    STATUS_SUCCESS if successful.

    STATUS_CORRUPT_SYSTEM_FILE if a block does not pass verification.

--*/
{
    ULONG BlockCount;
    ULONG BlockEnd;
    ULONG BlockSize;
    ULONG BlockStart;
    ULONG BlocksValid;
    WIM_INTEGRITY_HASH_BLOCK Hash;
    PVOID HashInCache;
    ULONG HashLength;
    ULONG i;
    NTSTATUS ReadStatus;
    NTSTATUS Status;

    if (Integrity == NULL || BufferSize == 0) {
        return STATUS_SUCCESS;
    }

    BlocksValid = WimpIntegrityCalculateBlockIndices(Integrity,
                                                     FileOffset,
                                                     BufferSize,
                                                     &BlockStart,
                                                     &BlockCount,
                                                     &BlockEnd);

    if (BlocksValid == FALSE) {
        WimpIntegrityReportFailure(Integrity, 0);
        return STATUS_CORRUPT_SYSTEM_FILE;
    }

    //
    // Verify the blocks.
    //

    Status = STATUS_SUCCESS;

    for (i = BlockStart;
         i < BlockEnd;
         i++, BufferSize -= WIM_INTEGRITY_BLOCK_LENGTH) {

        if (RtlTestBit(&Integrity->VerifiedBlockBitmap, i) != FALSE) {
            Buffer = Add2Ptr(Buffer, WIM_INTEGRITY_BLOCK_LENGTH),
            WimBlocksAlreadyValidated++;
            continue;
        }

        BlockSize = min(WIM_INTEGRITY_BLOCK_LENGTH, BufferSize);
        HashLength = BlockSize / 4;

        SymCryptSha256(Buffer, HashLength, Hash.Results[0]);
        Buffer = Add2Ptr(Buffer, HashLength);

        SymCryptSha256(Buffer, HashLength, Hash.Results[1]);
        Buffer = Add2Ptr(Buffer, HashLength);

        SymCryptSha256(Buffer, HashLength, Hash.Results[2]);
        Buffer = Add2Ptr(Buffer, HashLength);

        SymCryptSha256(Buffer, BlockSize - (HashLength * 3), Hash.Results[3]);
        Buffer = Add2Ptr(Buffer, BlockSize - (HashLength * 3));

        SymCryptSha256Init(&Hash.Type.Single);
        SymCryptSha256Append(&Hash.Type.Single, &Hash.Results[0][0], sizeof(Hash.Results));
        SymCryptSha256Result(&Hash.Type.Single, Hash.Results[0]);

        //
        // Update the cache if the hash for this block isn't present.
        //

        if (i < Integrity->HashCacheBlockStart ||
            i >= Integrity->HashCacheBlockEnd) {

            ReadStatus = WimpIntegrityLoadHashesInCache(Integrity, i);
            if (!NT_SUCCESS(ReadStatus)) {
                Status = ReadStatus;
                WimpIntegrityReportFailure(Integrity, i);
                break;
            }
        }

        //
        // Compare the calculated hash to that from the integrity file.
        //

        HashInCache = Add2Ptr(Integrity->HashCache,
                              (i - Integrity->HashCacheBlockStart) *
                              WIM_INTEGRITY_BLOCK_HASH_LENGTH);

        if (RtlEqualMemory(Hash.Results[0],
                           HashInCache,
                           WIM_INTEGRITY_BLOCK_HASH_LENGTH) != FALSE) {

            RtlSetBits(&Integrity->VerifiedBlockBitmap, i, 1);
            WimBlocksValidated++;
        }
        else {
            //
            // Ultimately return failure, but check if other blocks still
            // succeed. This avoids double hashing good blocks.

            WimpIntegrityReportFailure(Integrity, i);
            Status = STATUS_CORRUPT_SYSTEM_FILE;
        }
    }

    return Status;
}

NTSTATUS
WimpIntegrityLoadHashesInCache (
    _Inout_ PWIM_INTEGRITY_CONTEXT Integrity,
    _In_ ULONG BlockStart
    )
/*++

Routine Description:

    This routine loads hashes from the integrity file into the hash cache of an
    integrity context.

Arguments:

    Integrity - Supplies a pointer to the integrity context for the WIM.

    BlockStart - Supplies the index of the block to start loading hashes for.

Return Value:

    STATUS_SUCCESS if successful.

--*/
{
    ULONG BlockCount;
    ULONG BytesRead;
    ULONGLONG Offset;
    ULONG Size;
    NTSTATUS Status;

    NT_ASSERT(BlockStart < Integrity->VerifiedBlockBitmap.SizeOfBitMap);

    //
    // Only load hashes up to the maximum size of the cache.
    //

    BlockCount = min(WIM_INTEGRITY_HASH_CACHE_ENTRIES_COUNT,
                     Integrity->VerifiedBlockBitmap.SizeOfBitMap - BlockStart);

    Offset = Integrity->OffsetToHashes;
    Offset += BlockStart * WIM_INTEGRITY_BLOCK_HASH_LENGTH;
    Size = BlockCount * WIM_INTEGRITY_BLOCK_HASH_LENGTH;

    Status = BlFileReadAtOffsetEx(Integrity->IntegrityFileId,
                                  Size,
                                  Offset,
                                  Integrity->HashCache,
                                  &BytesRead,
                                  FILE_FLAGS_NONE);

    if (!NT_SUCCESS(Status)) {
        goto IntegrityLoadHashesInCacheEnd;
    }

    //
    // If the correct size wasn't read, empty the cache so that it is not
    // trusted for the next hash verification.
    //

    if (BytesRead != Size) {
        Integrity->HashCacheBlockStart = 0;
        Integrity->HashCacheBlockEnd = 0;
        Status = STATUS_CORRUPT_SYSTEM_FILE;
        goto IntegrityLoadHashesInCacheEnd;
    }

    //
    // Update the cache.
    //

    Integrity->HashCacheBlockStart = BlockStart;
    Integrity->HashCacheBlockEnd = BlockStart + BlockCount;

IntegrityLoadHashesInCacheEnd:
    return Status;
}

NTSTATUS
WimpIntegrityDeviceReadAtOffset(
    _Inout_opt_ PWIM_INTEGRITY_CONTEXT Integrity,
    _In_ DEVICE_ID DeviceId,
    _In_ ULONGLONG ByteOffset,
    _In_ ULONG Size,
    _Out_writes_bytes_(Size) PVOID Buffer,
    _Out_opt_ PULONG BytesRead
)
/*++

Routine Description:

    This routine is a wrapper for BlDeviceReadAtOffset that integrity checks
    the data read from the WIM.

Arguments:

    Integrity - Supplies a pointer to the integrity context for the WIM.

    DeviceId - Supplies the id of the WIM device.

    ByteOffset - Supplies the offset on the device where the read operation
         starts.

    Size - Supplies the number of bytes to be read.

    Buffer - Supplies a pointer that receives data being read.

    BytesRead - Supplies a pointer that receives the actual number of bytes
         read.

Return Value:

    STATUS_SUCCESS if successful.

    Other status codes from BlDeviceReadAtOffset if unsuccessful.

--*/
{
    ULONG ActualRead;
    ULONGLONG AlignedOffset;
    ULONG AlignedSize;
    BOOLEAN IntegrityNeeded;
    ULONG Padding;
    PVOID ReadBuffer;
    ULONG SizeToAllocate;
    NTSTATUS Status;

    AlignedOffset = 0;
    AlignedSize = 0;
    IntegrityNeeded = FALSE;
    ReadBuffer = Buffer;
    Padding = 0;

    if (Integrity != NULL) {

        //
        // Check if the read range being requested is already validated. If yes
        // the requested range can be read as is.
        //

        WimpIntegrityGetAlignedSizes(Integrity,
                                     ByteOffset,
                                     Size,
                                     1,
                                     &AlignedOffset,
                                     &AlignedSize,
                                     &Padding);

        IntegrityNeeded = (FALSE == WimpIntegrityAreBlocksValidated(
                                        Integrity,
                                        AlignedOffset,
                                        AlignedSize));

        if (IntegrityNeeded != FALSE &&
            (AlignedOffset != ByteOffset || AlignedSize != Size)) {

            //
            // Integrity is needed but we cannot use the caller's buffer.
            // See if we can use the lookaside, and if not recreate it.
            //

            if (AlignedSize > WimIntegrityReadBufferSize) {
                if (WimIntegrityReadBuffer != NULL) {
                    BlMmFreePages(WimIntegrityReadBuffer);
                    WimIntegrityReadBuffer = NULL;
                    WimIntegrityReadBufferSize = 0;
                }

                Status = RtlULongAdd(AlignedSize, PAGE_SIZE - 1, &SizeToAllocate);
                if (!NT_SUCCESS(Status)) {
                    goto IntegrityDeviceReadAtOffsetEnd;
                }

                SizeToAllocate = ALIGN_RANGE_DOWN(SizeToAllocate, PAGE_SIZE);

                Status = BlMmAllocatePages(&WimIntegrityReadBuffer,
                                           SizeToAllocate >> PAGE_SHIFT,
                                           MEMORY_TYPE_DEVICE_DATA,
                                           MEMORY_ATTRIBUTE_NONE,
                                           0);

                if (!NT_SUCCESS(Status)) {
                    goto IntegrityDeviceReadAtOffsetEnd;
                }

                WimIntegrityReadBufferSize = SizeToAllocate;
            }

            ReadBuffer = WimIntegrityReadBuffer;
        }
    }

    if (IntegrityNeeded == FALSE) {
        Status = BlDeviceReadAtOffset(DeviceId,
                                      ByteOffset,
                                      Size,
                                      Buffer,
                                      BytesRead);
    }
    else {
        Status = BlDeviceReadAtOffsetEx(DeviceId,
                                        AlignedOffset,
                                        AlignedSize,
                                        ReadBuffer,
                                        &ActualRead,
                                        DEVICE_FLAGS_NO_OFFSET_UPDATE);

        if (!NT_SUCCESS(Status)) {
            goto IntegrityDeviceReadAtOffsetEnd;
        }

        //
        // Validate the blocks loaded.
        //

        Status = WimpIntegrityValidateBlocks(Integrity,
                                             AlignedOffset,
                                             ReadBuffer,
                                             ActualRead);

        if (!NT_SUCCESS(Status)) {
            goto IntegrityDeviceReadAtOffsetEnd;
        }

        //
        // Copy to the caller buffer as required.
        //

        if (ReadBuffer != Buffer) {
            RtlCopyMemory(Buffer, Add2Ptr(ReadBuffer, Padding), Size);
        }

        if (BytesRead != NULL) {
            if ((ActualRead - Padding) >= Size) {
                *BytesRead = Size;
            }
            else {
                *BytesRead = ActualRead - Padding;
            }
        }
    }

IntegrityDeviceReadAtOffsetEnd:
    return Status;
}
NTSTATUS
WimpIntegrityBuildContext (
    _In_ ULONGLONG WimFileSize,
    _In_ FILE_ID IntegrityFileId,
    _In_ ULONG OffsetToHashes,
    _Outptr_ PWIM_INTEGRITY_CONTEXT *Integrity
    )
/*++

Routine Description:

    This routine creates an integrity context for a WIM.

Arguments:

    WimFileSize - Supplies the size in bytes of the WIM.

    IntegrityFileId - Supplies the file id of the integrity file for the WIM.

    OffsetToHashes - Supplies the offset from the start of the integrity file
         that the hashes begin at.

    Integrity - Supplies a pointer that receives a pointer to the new integrity
         context.

Return Value:

    STATUS_SUCCESS if successful.

--*/
{
    ULONG AlignedBitCount;
    ULONG BitCount;
    ULONGLONG BlockCount;
    PWIM_INTEGRITY_CONTEXT Context;
    ULONG HashCachePageOffset;
    ULONG PagesToAllocate;
    NTSTATUS Status;

    Context = NULL;

    //
    // Calculate how many blocks the WIM comprises.
    //

    Status = RtlULongLongAdd(WimFileSize,
                             WIM_INTEGRITY_BLOCK_LENGTH - 1,
                             &BlockCount);

    if (!NT_SUCCESS(Status)) {
        goto IntegrityBuildContextEnd;
    }

    BlockCount /= WIM_INTEGRITY_BLOCK_LENGTH;

    //
    // Calculate the length of a bitmap (which must have ULONG sized buffer).
    //

    Status = RtlULongLongToULong(BlockCount, &BitCount);
    if (!NT_SUCCESS(Status)) {
        goto IntegrityBuildContextEnd;
    }

    Status = RtlULongAdd(BitCount, 31, &AlignedBitCount);
    if (!NT_SUCCESS(Status)) {
        goto IntegrityBuildContextEnd;
    }

    AlignedBitCount = ALIGN_DOWN_BY(AlignedBitCount, 32);

    //
    // Allocate the integrity context via the page allocator as this could be
    // quite large. The allocation includes:
    // PAGE_ALIGNED(Structure + Bitmap Buffer) + PAGE_ALIGNED(Hash cache)
    //

    Status = RtlULongAdd(sizeof(WIM_INTEGRITY_CONTEXT),
                         AlignedBitCount >> 3,
                         &PagesToAllocate);

    if (!NT_SUCCESS(Status)) {
        goto IntegrityBuildContextEnd;
    }

    Status = RtlULongAdd(PagesToAllocate, PAGE_SIZE - 1, &PagesToAllocate);
    if (!NT_SUCCESS(Status)) {
        goto IntegrityBuildContextEnd;
    }

    PagesToAllocate >>= PAGE_SHIFT;
    HashCachePageOffset = PagesToAllocate;

    Status = RtlULongAdd(PagesToAllocate,
                         WIM_INTEGRITY_HASH_CACHE_PAGES,
                         &PagesToAllocate);

    if (!NT_SUCCESS(Status)) {
        goto IntegrityBuildContextEnd;
    }

    Status = BlMmAllocatePages(&Context,
                               PagesToAllocate,
                               MEMORY_TYPE_DEVICE_DATA,
                               MEMORY_ATTRIBUTE_NONE,
                               0);

    if (!NT_SUCCESS(Status)) {
        goto IntegrityBuildContextEnd;
    }

    //
    // Initialize the context.
    //

    RtlZeroMemory(Context, sizeof(WIM_INTEGRITY_CONTEXT));

    RtlInitializeBitMap(&Context->VerifiedBlockBitmap,
                        (PULONG)(Context + 1),
                        BitCount);

    RtlClearAllBits(&Context->VerifiedBlockBitmap);

    Context->WimFileSize = WimFileSize;
    Context->IntegrityFileId = IntegrityFileId;
    Context->OffsetToHashes = OffsetToHashes;
    Context->HashCache = Add2Ptr(Context, HashCachePageOffset * PAGE_SIZE);

    *Integrity = Context;
    Context = NULL;

IntegrityBuildContextEnd:
    if (Context != NULL) {
        WimpIntegrityDestroyContext(Context);
    }

    return Status;
}

NTSTATUS
WimpIntegrityValidateIntegrityFile (
    _In_ FILE_ID IntegrityFileId,
    _In_ DEVICE_ID WimDeviceId,
    _In_ ULONGLONG WimFileSize,
    _Out_ PBOOLEAN IntegrityValid,
    _Out_ PBOOLEAN IntegrityReady,
    _Out_ PULONG OffsetToHashes
    )
/*++

Routine Description:

    This routine determines if an integrity file is valid for a WIM.

Arguments:

    IntegrityFileId - Supplies the file id of the integrity file for the WIM.

    WimDeviceId - Supplies the device id of the WIM.

    WimFileSize - Supplies the size in bytes of the WIM.

    IntegrityValid - Supplies a pointer that receives FALSE if the integrity
         file is not valid, and TRUE if it is valid.

    IntegrityReady - Supplies a pointer that receives FALSE if the integrity
         file is valid but still being generated, and TRUE if it is complete.

    OffsetToHashes - Supplies a pointer that receives the offset from the start
         of the integrity file that the hashes begin at.

Return Value:

    STATUS_SUCCESS if successful.

--*/
{
    ULONG BytesRead;
    FILE_INFORMATION FileInfo;
    WIM_INTEGRITY_FILE_HEADER Header;
    ULONG HashesOffset;
    UCHAR HeaderHash[SYMCRYPT_SHA256_RESULT_SIZE];
    BOOLEAN Ready;
    ULONGLONG Size;
    NTSTATUS Status;
    BOOLEAN Valid;
    WIMHEADER_PACKED WimHeader;

    HashesOffset = 0;
    Ready = FALSE;
    Valid = FALSE;

    //
    // Read the header of the integrity file.
    //

    Status = BlFileReadAtOffsetEx(IntegrityFileId,
                                  sizeof(Header),
                                  0,
                                  &Header,
                                  &BytesRead,
                                  FILE_FLAGS_NONE);

    if (!NT_SUCCESS(Status)) {
        goto IntegrityValidateIntegrityFileEnd;
    }

    //
    // Validate the integrity file header is the right version and has a good
    // header hash.
    //

    if (BytesRead != sizeof(Header) ||
        Header.Signature != WIM_INTEGRITY_FILE_SIGNATURE ||
        Header.Version != WIM_INTEGRITY_FILE_CURRENT_VERSION ||
        Header.OffsetToHashes < sizeof(Header)) {

        goto IntegrityValidateIntegrityFileEnd;
    }

    SymCryptSha256((PCBYTE)&Header,
                   FIELD_OFFSET(WIM_INTEGRITY_FILE_HEADER, HeaderHash),
                   HeaderHash);

    if (RtlEqualMemory(HeaderHash,
                       Header.HeaderHash,
                       SYMCRYPT_SHA256_RESULT_SIZE) == FALSE) {

        goto IntegrityValidateIntegrityFileEnd;
    }

    //
    // Check that this integrity file is for this WIM.
    //

    Status = BlDeviceReadAtOffset(WimDeviceId,
                                  0,
                                  sizeof(WimHeader),
                                  &WimHeader,
                                  &BytesRead);

    if (!NT_SUCCESS(Status)) {
        goto IntegrityValidateIntegrityFileEnd;
    }

    if (BytesRead != sizeof(WimHeader) ||
        Header.WimFileSize != WimFileSize ||
        RtlEqualMemory(&Header.WimHeaderGuid,
                       &WimHeader.gWIMGuid,
                       sizeof(GUID)) == FALSE) {

        goto IntegrityValidateIntegrityFileEnd;
    }

    //
    // If NextOffsetToGenerate is less than the size of the WIM then the hashes
    // have not completed being generated yet.
    //

    if (Header.NextOffsetToGenerate > WimFileSize) {
        goto IntegrityValidateIntegrityFileEnd;
    }

    if (Header.NextOffsetToGenerate < WimFileSize) {
        Valid = TRUE;
        Ready = FALSE;
    }
    else {
        //
        // The hashes are marked as complete. Ensure the integrity file size
        // matches that needed for this WIM.
        //

        Status = RtlULongLongAdd(WimFileSize,
                                 WIM_INTEGRITY_BLOCK_LENGTH - 1,
                                 &Size);

        if (!NT_SUCCESS(Status)) {
            goto IntegrityValidateIntegrityFileEnd;
        }

        Size /= WIM_INTEGRITY_BLOCK_LENGTH;
        Size *= WIM_INTEGRITY_BLOCK_HASH_LENGTH;

        Status = RtlULongLongAdd(Size, Header.OffsetToHashes, &Size);
        if (!NT_SUCCESS(Status)) {
            goto IntegrityValidateIntegrityFileEnd;
        }

        Status = BlFileGetInformation(IntegrityFileId, &FileInfo);
        if (!NT_SUCCESS(Status)) {
            goto IntegrityValidateIntegrityFileEnd;
        }

        if (Size != FileInfo.FileSize) {
            goto IntegrityValidateIntegrityFileEnd;
        }

        Valid = TRUE;
        Ready = TRUE;
        HashesOffset = Header.OffsetToHashes;
    }

IntegrityValidateIntegrityFileEnd:
    if (NT_SUCCESS(Status)) {
        *IntegrityReady = Ready;
        *IntegrityValid = Valid;
        *OffsetToHashes = HashesOffset;
    }

    if (Valid == FALSE) {
        WimpIntegrityReportFailure(NULL, 0);
    }

    return Status;
}

NTSTATUS
WimpIntegrityOpenIntegrityFile (
    _In_ DEVICE_ID WimDeviceId,
    _Out_ PFILE_ID IntegrityFileId,
    _Out_ PDEVICE_ID OverlayedDeviceId
)
/*++

Routine Description:

    This routine opens the integrity file for a WIM.

Arguments:

    WimDeviceId - Supplies the device id of the WIM.

    IntegrityFileId - Supplies a pointer that receives the file id of the
         integrity file for the WIM.

    OverlayedDeviceId - Supplies a pointer that receives the device id of the
         device that the WIM is overlaying.

Return Value:

    STATUS_SUCCESS if successful.

    STATUS_NO_SUCH_FILE if the integrity file does not exist.

    Other status codes if unsuccessful.

--*/
{
    LARGE_INTEGER DataSource;
    DEVICE_ID DeviceId;
    WCHAR FilePath[WIM_INTEGRITY_FILE_PATH_LENGTH_Z];
    int IntStatus;
    NTSTATUS Status;

    //
    // Determine if this WIM is an overlay. If not it could be a WIM stored in
    // a resource DLL, and so we should not fail and just report there is no
    // integrity needed for this WIM.
    //

    DeviceId = BlDeviceGetOverlayInformation(WimDeviceId, &DataSource);
    if (DeviceId == INVALID_DEVICE_ID) {
        Status = STATUS_NO_SUCH_FILE;
        goto IntegrityOpenIntegrityFileEnd;
    }

    IntStatus = swprintf_s(FilePath,
                           ARRAYSIZE(FilePath),
                           L"%s\\%s%016I64X",
                           WIM_INTEGRITY_DIRECTORY_Z,
                           WIM_INTEGRITY_FILENAME_PREFIX_Z,
                           DataSource.QuadPart);

    if (IntStatus < 1) {
        Status = STATUS_INTEGER_OVERFLOW;
        goto IntegrityOpenIntegrityFileEnd;
    }

    //
    // Create the path to the integrity file.
    //

    Status = BlFileOpen(DeviceId,
                        FilePath,
                        OPEN_READ,
                        IntegrityFileId);

    if (!NT_SUCCESS(Status)) {
        goto IntegrityOpenIntegrityFileEnd;
    }

    *OverlayedDeviceId = DeviceId;

IntegrityOpenIntegrityFileEnd:
        return Status;
}

#endif
