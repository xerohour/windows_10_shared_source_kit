/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    blkalloc.c

Abstract:

    This module implements the block allocator routines for allocating,
    maintaining, and releasing memory pages in blocks.

--*/

#include <bootlibp.h>

/*++

Block Memory Allocator Description:

    Block memory allocator provides support for pre-allocating chunks of
    memory for use by a boot application. The actual memory allocation
    is achieved through the page-allocator and thus it can be viewed as
    layered on top of the page-allocator. Pre-allocating blocks of memory is
    beneficial in situations where a boot application or library
    component makes several allocations for a fixed size of memory. The main
    advantage of the block memory allocator is that new allocations do not
    cause new memory descriptors to be created if the memory already exists.
    In contrast, each allocation from the page allocator generates a new
    descriptor unless it is a coalesced memory allocation. Limiting the
    number of descriptors is required in situations where only a fixed number
    of descriptors can be handled by the boot library. One such scenario occurs
    when a new boot application maps the calling boot application's allocated
    memory list. If the caller's memory descriptor list is large (possible
    if a large WIM file is loaded while booting from the CD), the new boot
    application may run out of global descriptors during library memory
    initialization and cause boot failure. Another scenario occurs during
    boot application parameter passing. Only a fixed amount of pages are
    available for parameter passing. If calling boot application's descriptor
    list is large, the memory allocated for parameters may be overrun. Another
    benefit of the block allocator is improved performance. With memory
    pre-allocated, majority of the requests are satisfied from the memory
    pool causing fewer number of firmware memory allocation calls.

    The current implementation supports allocating arbitrary number of blocks
    (of fixed size). The application can configure how much memory is
    pre-allocated by the block allocator and size of each block. It also
    supports releasing previously allocated memory blocks. A boot application
    can release all the allocated memory at once by calling the destory
    routine.

--*/

// -------------------------------------------------------------------- Defines

#define BLOCK_ALLOCATOR_ERROR_NOT_FOUND     ((ULONG) -1)
#define BLOCK_ALLOCATOR_INITIAL_TABLE_SIZE  8

// ------------------------------------------------------------ Data Structures

typedef struct _BLOCK_ALLOCATOR_DESCRIPTOR {
    LIST_ENTRY NextBlock;
    ULONG TotalBlocks;
    ULONG FreeBlocks;
    ULONG HintIndex;
    RTL_BITMAP AllocationMap;
    BLOCK_MEMORY_ADDRESS BlockAddress;
} BLOCK_ALLOCATOR_DESCRIPTOR, *PBLOCK_ALLOCATOR_DESCRIPTOR;

/*++

Block Memory Allocator Descriptor:

    This structure describes a pool of pre-allocated memory.

Fields:

    NextBlock - This field contains the pointer to the next block memory
        descriptor in the chain of descriptors.

    TotalBlocks - This fields contains the total number of pre-allocated
        memory for this descriptor.

    FreeBlocks - This field contains the number of free memory blocks within
        the descriptor. If this value is equal 0, then all the
        memory blocks are consumed. If this value is equal to TotalBlocks, then
        all of the memory blocks are free.

    HintIndex -  This field contains the hint where the next chunk of
        free memory blocks might be available.

    AllocationMap - This fields contains a bitmap representing whether each
        of the memory blocks is available or free. A bit value of 1 indicates
        that the corresponding block is in use. A bit value of 0 indicates
        that the corresponding block is free.

    AllocatedPages - This field contains the physical address for the
        allocated memory block. This field is only used when physical pages
        are allocated for the block.

    AllocatedMemory - This field contains a pointer to the allocated memory
        block. This field is only used when virtual pages are allocated
        for the block.

--*/

typedef struct _BLOCK_ALLOCATOR_INFORMATION {
    BLOCK_ALLOCATOR_DESCRIPTOR Root;
    BLOCK_ALLOCATOR_MEMORY_TYPE MemoryType;
    MEMORY_TYPE PageType;
    ULONG Attributes;
    ULONG Alignment;
    ULONG InitialBlocks;
    ULONG DefaultBlocks;
    ULONG BlockUnitSize;
    BLOCK_ALLOCATOR_HANDLE Handle;
    ULONG ReferenceCount;
} BLOCK_ALLOCATOR_INFORMATION, *PBLOCK_ALLOCATOR_INFORMATION;


/*++

Block Memory Allocator Information:

    This structure contains all the parameters/information required
    for pre-allocating a block of memory.

Fields:

    Root - This field points represents the head of the block allocator
        descriptor list.

    MemoryType - This field contains the memory type for the block allocation.
        This value can be either PhysicalPages or VirtualPages. This is used
        in deciding whether physical or virtual pages need to be
        allocated for the memory block.

    PageType - This field contains the type for each page within the allocated
        memory block. This value is not used by block allocator and is
        passed directly to page allocator routines as MemoryType parameter.

    Attributes - This field contains the attributes for pages to be used
        when allocating memory pool. This value is not used by block
        allocator and is passed directly page allocator routines as
        Attributes parameter.

    Alignment - Supplies the alignment required for the allocated memory pool.
        This value is not used by the block allocator and is passed
        directly to the page allocator for the  Alignment parameter.

    InitialBlocks - This field contains the total number of memory blocks to be
        pre-allocated on the first allocation only. Supplying this value
        can be beneficial in cases where the boot application makes a large
        amount of initial allocation and small amount of fixed allocations
        later on. This field is not mandatory and can be zero.

    DefaultBlocks - This field contains the minimum number of memory
        blocks to be allocated each time the block allocator extends the memory
        pool. Supplying this value can be beneficial in cases where the boot
        application makes requests for constant number of blocks. This field
        is not mandatory and can be zero.

    BlockUnitSize - This field contains the size (in bytes) for each individual
        block. This is a mandatory field and must be an integral multiple
        of the page size.

    Handle - This field contains a unique value that identifies a particular
        block allocator instance.

--*/

// ----------------------------------------------------------------- Globals

BOOLEAN MmBlockAllocatorInitialized = FALSE;
PBLOCK_ALLOCATOR_INFORMATION *MmBlockAllocatorTable;
ULONG MmBlockAllocatorTableEntries;

// ----------------------------------------------------------------- Prototypes

NTSTATUS
MmBapAllocatePagesHelper (
    __out PBLOCK_ALLOCATOR_DESCRIPTOR BlockDescriptor,
    __in PBLOCK_ALLOCATOR_INFORMATION BlockInformation,
    __in ULONG NumberOfBlocks
    );

NTSTATUS
MmBapExtendBlockAllocator (
    __out PBLOCK_ALLOCATOR_DESCRIPTOR *ReturnDescriptor,
    __in PBLOCK_ALLOCATOR_INFORMATION BlockInformation,
    __in ULONG RequiredBlocks
    );

NTSTATUS
MmBapFreeBlockAllocatorDescriptor (
    __in PBLOCK_ALLOCATOR_INFORMATION BlockInformation,
    __in PBLOCK_ALLOCATOR_DESCRIPTOR BlockDescriptor
    );

// ---------------------------------------------------- Handle Table Prototypes

BOOLEAN
MmBapCompareBlockAllocatorTableEntry (
    __in PVOID Entry,
    __in PVOID MatchParameter,
    __in PVOID Arg2,
    __in PVOID Arg3,
    __in PVOID Arg4
    );

PBLOCK_ALLOCATOR_INFORMATION
MmBapFindBlockInformation (
    __in BLOCK_ALLOCATOR_HANDLE Handle
    );

NTSTATUS
MmBapFreeBlockAllocatorTableEntry (
    __in PBLOCK_ALLOCATOR_INFORMATION BlockInformation,
    __in ULONG Index
    );

NTSTATUS
MmBapPurgeBlockAlloctorTableEntry (
    __in PBLOCK_ALLOCATOR_INFORMATION BlockInformation
    );

// --------------------------------------------------------- Exported Functions

NTSTATUS
BlpMmAllocateMemoryBlocks (
    __in BLOCK_ALLOCATOR_HANDLE BlockHandle,
    __in ULONG RequiredBlocks,
    __out PBLOCK_MEMORY_ADDRESS BlockAddress
    )

/*++

Routine Description:

    This routine allocates the specified amount of blocks from a pre-allocated
    memory pool.

    It first attempts to satisfy the request from a pre-allocated memory pool.
    If sufficient pool memory does not exist, this routine will attempt
    to grow the pool to satisfy the request.

Arguments:

    BlockHandle - Supplies a handle to the block allocator instance. This handle
        should be the return value from block allocator creation routine.

    RequiredBlocks - Supplies the number of blocks required to be allocated.
        This value is independent of the initial and the default number of
        blocks.

    BlockAddress - Supplies a pointer to a variable that receives the starting
        physical address for the allocated memory on success.

Return Value:

    STATUS_SUCCESS if the specified number of blocks were allocated.

    STATUS_INVALID_PARAMETER if the input parameters are not valid.

    STATUS_UNSUCCESSFUL if the memory cannot be allocated.

    Appropriate NTSTATUS code otherwise.

--*/

{

    PBLOCK_ALLOCATOR_DESCRIPTOR BlockDescriptor;
    PBLOCK_ALLOCATOR_INFORMATION BlockInformation;
    PLIST_ENTRY BlockMemoryHead;
    PLIST_ENTRY NextEntry;
    ULONG NextFreeBlockIndex;
    ULONGLONG Offset;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceBlkalloc);

    //
    // Bail out if the block allocator has not been initialized.
    //

    if (MmBlockAllocatorInitialized == FALSE) {
        Status = STATUS_UNSUCCESSFUL;
        goto AllocateBlockMemoryPagesExit;
    }

    //
    // Validate input parameters.
    //

    if ((BlockAddress == NULL) ||
        (BlockHandle == BLOCK_ALLOCATOR_INVALID_HANDLE) ||
        (RequiredBlocks == 0)) {

        Status = STATUS_INVALID_PARAMETER;
        goto AllocateBlockMemoryPagesExit;
    }

    //
    // Find the block information structure that corresponds to the
    // supplied handle.
    //

    BlockInformation = MmBapFindBlockInformation(BlockHandle);
    if (BlockInformation == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto AllocateBlockMemoryPagesExit;
    }

    //
    // No further allocations are allowed after the block allocator instance
    // has been deleted.
    //
    // N.B. This exposes a bug in the caller. ASSERT to catch the culprit
    //      red-handed.
    //

    if (BlockInformation->ReferenceCount == 0) {

        ASSERT(FALSE);

        Status = STATUS_INVALID_PARAMETER;
        goto AllocateBlockMemoryPagesExit;
    }

    //
    // Walk through the block memory descriptor list to find a descriptor
    // that contains the specified number of consecutive free blocks.
    //
    // In the current implementation first-fit allocation policy is used, i.e.,
    // the first available set of memory blocks sufficient enough to satisfy
    // the request is chosen.
    //
    // TODO: Investigate whether best-fit, worst-fit or other allocation
    //       policies work out better.
    //

    BlockDescriptor = NULL;
    NextFreeBlockIndex = 0;
    BlockMemoryHead = &BlockInformation->Root.NextBlock;
    NextEntry = BlockMemoryHead->Flink;
    while (NextEntry != BlockMemoryHead) {
        BlockDescriptor = CONTAINING_RECORD(NextEntry,
                                            BLOCK_ALLOCATOR_DESCRIPTOR,
                                            NextBlock);

        //
        // N.B. The memory pool may be fragmented and hence the request may
        //      not be satisfied even if FreeBlocks >= RequiredBlocks as
        //      contiguous free blocks are needed.
        //

        if (BlockDescriptor->FreeBlocks >= RequiredBlocks) {
            NextFreeBlockIndex = RtlFindClearBits(
                                    &BlockDescriptor->AllocationMap,
                                    RequiredBlocks,
                                    BlockDescriptor->HintIndex);

            if (NextFreeBlockIndex != BLOCK_ALLOCATOR_ERROR_NOT_FOUND) {
                break;
            }
        }

        NextEntry = NextEntry->Flink;       // Advance to the next descriptor.
    }

    //
    // If none of the descriptors had sufficient number of free blocks to
    // satisfy the request, then grow the memory pool.
    //

    if (NextEntry == BlockMemoryHead) {
        BlockDescriptor = NULL;
        Status = MmBapExtendBlockAllocator(&BlockDescriptor,
                                         BlockInformation,
                                         RequiredBlocks);

        if (!NT_SUCCESS(Status)) {
            goto AllocateBlockMemoryPagesExit;
        }

        //
        // New descriptors should have atleast the required number of blocks
        // allocated within it.
        //

        ASSERT(BlockDescriptor->TotalBlocks >= RequiredBlocks);
        NextFreeBlockIndex = 0;
    }

    ASSERT((BlockDescriptor != NULL) &&
           (NextFreeBlockIndex < BlockDescriptor->TotalBlocks));

    //
    // Allocate the required number of blocks starting at the next free
    // block location in the descriptor.
    //

    RtlSetBits(&BlockDescriptor->AllocationMap,
               NextFreeBlockIndex,
               RequiredBlocks);

    BlockDescriptor->HintIndex = NextFreeBlockIndex + RequiredBlocks;
    BlockDescriptor->FreeBlocks -= RequiredBlocks;

    //
    // Ensure that the allocation does not cause an overflow.
    //

    ASSERT(BlockDescriptor->TotalBlocks >= BlockDescriptor->FreeBlocks);

    Offset = (ULONG64)NextFreeBlockIndex *
             (ULONG64)BlockInformation->BlockUnitSize;

    //
    // Return the address for memory block.
    //

    if (BlockInformation->MemoryType == PhysicalPages) {
        BlockAddress->PhysicalAddress = BlockDescriptor->BlockAddress.PhysicalAddress;
        BlockAddress->PhysicalAddress.QuadPart += Offset;

    } else {
        BlockAddress->VirtualAddress =
            Add2Ptr(BlockDescriptor->BlockAddress.VirtualAddress, Offset);
    }

    Status = STATUS_SUCCESS;

AllocateBlockMemoryPagesExit:

    BlpInterfaceExit(InterfaceBlkalloc);
    return Status;
}

BLOCK_ALLOCATOR_HANDLE
BlpMmCreateBlockAllocator (
    __in BLOCK_ALLOCATOR_MEMORY_TYPE MemoryType,
    __in MEMORY_TYPE PageType,
    __in ULONG Attributes,
    __in ULONG Alignment,
    __in_opt ULONG InitialBlocks,
    __in_opt ULONG DefaultBlocks,
    __in ULONG BlockUnitSize
    )

/*++

Routine Description:

    This routine initializes and configures a new block allocator instance.

Arguments:

    MemoryType - Supplies the type of memory pages (physical or virtual) to
        be allocated.

    PageType  - Supplies the type to be passed to page allocator routines
        when allocating memory blocks.

    Attributes - Supplies the attributes required for the allocated memory
        blocks.

    Alignment - Supplies the alignment required for the allocated memory
        blocks.

    InitialBlocks - Supplies the total number of memory blocks to be
        pre-allocated. The physical pages are actually allocated when the
        boot application makes its first block memory allocation. This value
        can be zero if the boot application does not desire an initial amount
        of memory to be pre-allocated.

    DefaultBlocks - Supplies the minimum number of memory blocks to be allocated
        when previously allocated block of memory is exhausted. This value
        can be zero if a suitable default size is not known for the boot
        application.

    BlockUnitSize - Supplies the size (in bytes) for each individual block.

Return Value:

    BLOCK_ALLOCATOR_HANDLE - A handle value that identifies the created
        block allocator instance. This value must be specified by the boot
        application while making a memory request to the block memory allocator.
        The handle value is BLOCK_ALLOCATOR_INVALID_HANDLE on failure.

        N.B. A zero value for handle is valid.

--*/

{

    PBLOCK_ALLOCATOR_INFORMATION BlockInformation;
    BLOCK_ALLOCATOR_HANDLE Handle;
    ULONG Size;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceBlkalloc);

    //
    // Bail out if the block allocator has not been initialized.
    //

    Handle = BLOCK_ALLOCATOR_INVALID_HANDLE;
    BlockInformation = NULL;
    if (MmBlockAllocatorInitialized == FALSE) {
        goto CreateBlockAllocatorExit;
    }

    //
    // The size for each memory block must be a non-zero value and an
    // integral of PAGE_SIZE.
    //

    if ((BlockUnitSize == 0) && ((BlockUnitSize % PAGE_SIZE) != 0)) {
        goto CreateBlockAllocatorExit;
    }

    //
    // Setup the informational to be used while pre-allocating blocks of
    // memory.
    //
    // N.B. The initial pool of memory is lazily allocated on the first block
    //      memory allocation call.
    //

    Size = sizeof(BLOCK_ALLOCATOR_INFORMATION);
    BlockInformation = BlMmAllocateHeap(Size);
    if (BlockInformation == NULL) {
        goto CreateBlockAllocatorExit;
    }

    RtlZeroMemory(BlockInformation, Size);
    BlockInformation->MemoryType = MemoryType;
    BlockInformation->PageType = PageType;
    BlockInformation->Attributes = Attributes;
    BlockInformation->Alignment = Alignment;
    BlockInformation->InitialBlocks = InitialBlocks;
    BlockInformation->DefaultBlocks = DefaultBlocks;
    BlockInformation->BlockUnitSize = BlockUnitSize;
    InitializeListHead(&BlockInformation->Root.NextBlock);

    //
    // Insert the block information structure into the block allocator handle
    // table.
    //

    Status = BlTblSetEntry(&MmBlockAllocatorTable,
                           &MmBlockAllocatorTableEntries,
                           BlockInformation,
                           &Handle,
                           MmBapPurgeBlockAlloctorTableEntry);

    if (!NT_SUCCESS(Status)) {
        Handle = BLOCK_ALLOCATOR_INVALID_HANDLE;
        goto CreateBlockAllocatorExit;
    }

    //
    // The handle should be valid on successful insertion into handle table.
    //

    ASSERT(Handle != BLOCK_ALLOCATOR_INVALID_HANDLE);

    //
    // Mark the block allocator instance as in use.
    //
    // N.B. This count will be decremented when the caller tears down the
    //      block allocator instance.
    //

    BlockInformation->ReferenceCount = 1;

    //
    // Establish binding between the handle and the corresponding block
    // allocator instance.
    //

    BlockInformation->Handle = Handle;

CreateBlockAllocatorExit:

    //
    // Free up the block information structure on failure.
    //

    if ((Handle == BLOCK_ALLOCATOR_INVALID_HANDLE) &&
        (BlockInformation != NULL)) {

        BlMmFreeHeap(BlockInformation);
    }

    BlpInterfaceExit(InterfaceBlkalloc);
    return Handle;
}

NTSTATUS
BlpMmDeleteBlockAllocator (
    __in BLOCK_ALLOCATOR_HANDLE BlockHandle
    )

/*++

Routine Description:

    This routine destroys a block allocator instance by freeing all the
    allocated memory blocks. On return, the specified handle is invalid and
    should not be used for making any further block allocations.

Arguments:

    BlockHandle - Supplies a handle to the block allocator instance. This handle
        should be the return value from block allocator creation routine.

Return Value:

    STATUS_SUCCESS if the destruction was successful.

    STATUS_UNSUCCESSFUL if the memory could not be freed.

    STATUS_INVALID_PARAMETER if the speicified handle is not valid.

    Appropriate NTSTATUS code otherwise.

--*/

{

    PBLOCK_ALLOCATOR_DESCRIPTOR BlockDescriptor;
    PBLOCK_ALLOCATOR_INFORMATION BlockInformation;
    PLIST_ENTRY BlockMemoryHead;
    PLIST_ENTRY NextEntry;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceBlkalloc);

    //
    // Bail out if the block allocator has not been initialized.
    //

    if (MmBlockAllocatorInitialized == FALSE) {
        ReturnStatus = STATUS_UNSUCCESSFUL;
        goto DeleteBlockAllocatorExit;
    }

    //
    // Find the block information structure that corresponds to the
    // supplied handle.
    //

    BlockInformation = MmBapFindBlockInformation(BlockHandle);
    if (BlockInformation == NULL) {
        ReturnStatus = STATUS_INVALID_PARAMETER;
        goto DeleteBlockAllocatorExit;
    }

    //
    // Ensure that a particular block allocator instance is deleted only once.
    //
    // N.B. Deleting a block allocator instance multiple times exposes a bug
    //      in the caller. ASSERT to catch the caller red-handed.
    //

    if (BlockInformation->ReferenceCount == 0) {

        ASSERT(FALSE);

        ReturnStatus = STATUS_INVALID_PARAMETER;
        goto DeleteBlockAllocatorExit;
    }

    //
    // Walk through the block allocator descriptor list and free each
    // descriptor. This will also cause the memory pool associated with
    // each descriptor to be also freed.
    //

    ReturnStatus = STATUS_SUCCESS;
    BlockMemoryHead = &BlockInformation->Root.NextBlock;
    while (IsListEmpty(BlockMemoryHead) == FALSE) {
        NextEntry = BlockMemoryHead->Flink;
        BlockDescriptor = CONTAINING_RECORD(NextEntry,
                                            BLOCK_ALLOCATOR_DESCRIPTOR,
                                            NextBlock);

        Status = MmBapFreeBlockAllocatorDescriptor(BlockInformation,
                                                   BlockDescriptor);

        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }
    }

    //
    // Decrement the reference count to account for the initial reference
    // (taken when creating the allocator instance). This marks the block
    // allocator instance as free and sets it up for a purge operation when
    // the block allocator handle table becomes full.
    //

    BlockInformation->ReferenceCount -= 1;

    ASSERT(BlockInformation->ReferenceCount == 0);

DeleteBlockAllocatorExit:

    BlpInterfaceExit(InterfaceBlkalloc);
    return ReturnStatus;
}

NTSTATUS
BlpMmFreeMemoryBlocks (
    __in BLOCK_ALLOCATOR_HANDLE BlockHandle,
    __in BLOCK_MEMORY_ADDRESS BlockAddress,
    __in ULONG BlocksToFree
    )

/*++

Routine Description:

    This routine releases the specified number of memory blocks starting at
    the given address.

    It searches for the given memory block in the current list of memory
    descriptors associated with the desired block allocator instance.
    If such a descriptor is found, then it marks the specified memory blocks
    as free. It will additionally shrink the memory pool if freeing the blocks
    causes it to be no longer in use.

Arguments:

    BlockHandle - Supplies a handle to the block allocator instance. This handle
        should be the return value from block allocator creation routine.

    BlockAddress - Supplies the starting address for the memory range to be
        freed. This should be the return value from BlpMmFreeBlockMemoryPages.

    BlocksToFree - Supplies the number of blocks to be freed.

Return Value:

    STATUS_SUCCESS if the memory could be successfully freed.

    STATUS_INVALID_PARAMETER if the input parameters are not valid.

    Appropriate NTSTATUS code otherwise.

--*/

{

    PBLOCK_ALLOCATOR_DESCRIPTOR BlockDescriptor;
    ULONGLONG BlockEndAddress;
    PBLOCK_ALLOCATOR_INFORMATION BlockInformation;
    PLIST_ENTRY BlockMemoryHead;
    ULONGLONG BlockStartAddress;
    ULONGLONG EndAddress;
    PLIST_ENTRY NextEntry;
    ULONGLONG Offset;
    ULONGLONG StartAddress;
    ULONG StartingBlock;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceBlkalloc);

    //
    // Bail out if the block allocator has not been initialized.
    //

    if (MmBlockAllocatorInitialized == FALSE) {
        Status = STATUS_UNSUCCESSFUL;
        goto FreeBlockMemoryPagesExit;
    }

    //
    // Validate input parameters.
    //

    if ((BlockHandle == BLOCK_ALLOCATOR_INVALID_HANDLE) ||
        (BlocksToFree == 0)) {

        Status = STATUS_INVALID_PARAMETER;
        goto FreeBlockMemoryPagesExit;
    }

    //
    // Find the block information structure that corresponds to the
    // supplied handle. Ensure that the supplied memory type matches the
    // initialized one.
    //

    BlockInformation = MmBapFindBlockInformation(BlockHandle);
    if (BlockInformation == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto FreeBlockMemoryPagesExit;
    }

    //
    // Compute the start and end address for the address range to be freed.
    //

    if (BlockInformation->MemoryType == PhysicalPages) {
        StartAddress = (ULONGLONG)BlockAddress.PhysicalAddress.QuadPart;
    } else {
        StartAddress = (ULONGLONG)(ULONG_PTR)BlockAddress.VirtualAddress;
    }

    EndAddress = StartAddress + (BlocksToFree * BlockInformation->BlockUnitSize);
    if (EndAddress < StartAddress) {
        Status = STATUS_INVALID_PARAMETER;
        goto FreeBlockMemoryPagesExit;
    }

    //
    // Walk through the block memory descriptor list to find a descriptor
    // that contains the address range to be freed.
    //

    BlockDescriptor = NULL;
    BlockStartAddress = 0;
    BlockEndAddress = 0;
    BlockMemoryHead = &BlockInformation->Root.NextBlock;
    for (NextEntry = BlockMemoryHead->Flink;
         NextEntry != BlockMemoryHead;
         NextEntry = NextEntry->Flink) {

        BlockDescriptor = CONTAINING_RECORD(NextEntry,
                                            BLOCK_ALLOCATOR_DESCRIPTOR,
                                            NextBlock);

        if (BlockInformation->MemoryType == PhysicalPages) {
            BlockStartAddress =
                (ULONGLONG)BlockDescriptor->BlockAddress.PhysicalAddress.QuadPart;

        } else {
            BlockStartAddress =
                (ULONGLONG)(ULONG_PTR)BlockDescriptor->BlockAddress.VirtualAddress;
        }

        BlockEndAddress = BlockStartAddress + (BlockDescriptor->TotalBlocks *
                                BlockInformation->BlockUnitSize);

        //
        // BlockDescriptor contains the address to be freed if the input
        // start address lies within the descriptor's memory range.
        //

        if ((StartAddress >= BlockStartAddress) &&
            (StartAddress < BlockEndAddress)) {

            break;
        }
    }

    if (NextEntry == BlockMemoryHead) {
        Status = STATUS_NOT_FOUND;
        goto FreeBlockMemoryPagesExit;
    }

    //
    // Ensure that the caller is not trying to release memory beyond the
    // end of the descriptor range.
    //

    if (EndAddress > BlockEndAddress) {
        Status = STATUS_INVALID_PARAMETER;
        goto FreeBlockMemoryPagesExit;
    }

    //
    // The address to be freed must always be aligned on block unit size.
    //

    Offset = StartAddress - BlockStartAddress;
    if ((Offset % sizeof(BlockInformation->BlockUnitSize)) != 0) {
        Status = STATUS_INVALID_PARAMETER;
        goto FreeBlockMemoryPagesExit;
    }

    //
    // Free the specified number of blocks in BlockDescriptor.
    //

    StartingBlock = (ULONG)(Offset / BlockInformation->BlockUnitSize);
    RtlClearBits(&BlockDescriptor->AllocationMap,
                 StartingBlock,
                 BlocksToFree);

    BlockDescriptor->FreeBlocks += BlocksToFree;

    //
    // Update the hint on first free within the descriptor.
    //

    if (BlockDescriptor->HintIndex >= BlockDescriptor->TotalBlocks) {
        BlockDescriptor->HintIndex = StartingBlock;
    }

    //
    // Free the descriptor when all blocks within it are free. This will
    // cause the associated memory block to be also freed and the pool to be
    // shrunk.
    //

    if (BlockDescriptor->FreeBlocks == BlockDescriptor->TotalBlocks) {
        MmBapFreeBlockAllocatorDescriptor(BlockInformation, BlockDescriptor);
    }

    Status = STATUS_SUCCESS;

FreeBlockMemoryPagesExit:

    BlpInterfaceExit(InterfaceBlkalloc);
    return Status;
}

NTSTATUS
BlpMmSetBlockAllocatorDefaultBlocks (
    __in BLOCK_ALLOCATOR_HANDLE BlockHandle,
    __in_opt ULONG DefaultBlocks
    )

/*++

Routine Description:

    This routine sets a new default number of blocks to be allocated when
    growing the memory pool for the block allocator instance. The new default
    value will be used for future allocations and it doesnt affect previously
    allocated memory blocks.

Arguments:

    BlockHandle - Supplies a handle to the block allocator instance. This handle
        should be the return value from block allocator creation routine.

    DefaultBlocks - Supplies the new default number of blocks to allocate.
        This is an optional value and can be zero. A value of zero implies
        that only the requested number of blocks will be allocated on
        each allocation request.

Return Value:

    STATUS_SUCCESS if the new default value was successfully set.

    STATUS_INVALID_PARAMETER if the input handle value is not valid.

    STATUS_UNSUCCESSFUL if the default value could not be set.

--*/

{

    PBLOCK_ALLOCATOR_INFORMATION BlockInformation;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceBlkalloc);

    //
    // Bail out if the block allocator has not been initialized.
    //

    if (MmBlockAllocatorInitialized == FALSE) {
        Status = STATUS_UNSUCCESSFUL;
        goto SetBlockAllocatorDefaultBlocksExit;
    }

    //
    // Validate input parameters.
    //

    if (BlockHandle == BLOCK_ALLOCATOR_INVALID_HANDLE) {
        Status = STATUS_INVALID_PARAMETER;
        goto SetBlockAllocatorDefaultBlocksExit;
    }

    //
    // Find the block information structure that corresponds to the
    // supplied handle.
    //

    BlockInformation = MmBapFindBlockInformation(BlockHandle);
    if (BlockInformation == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto SetBlockAllocatorDefaultBlocksExit;
    }

    //
    // Set the new default block size.
    //

    BlockInformation->DefaultBlocks = DefaultBlocks;
    Status = STATUS_SUCCESS;

SetBlockAllocatorDefaultBlocksExit:

    BlpInterfaceExit(InterfaceBlkalloc);
    return Status;
}

// ------------------------------------------------------- MM Internal Functions

NTSTATUS
MmBaDestroy (
    VOID
    )

/*++

Routine Description:

    This routine destroys the global state for the block allocator. This
    involves destroying the block allocator handle table and freeing all
    active block allocator instances.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS if all the active block allocator table entries could be
        successfully cleaned up.

    Appropriate NTSTATUS code otherwise.

--*/

{

    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceBlkalloc);

    //
    // Successfully bail out if the block allocator has not been initialized.
    //

    if (MmBlockAllocatorInitialized == FALSE) {
        ReturnStatus = STATUS_SUCCESS;
        goto BaDestroyExit;
    }

    //
    // Delete all entries still remaining in block allocator table.
    //

    ReturnStatus = STATUS_SUCCESS;
    Status = BlTblMap(MmBlockAllocatorTable,
                      MmBlockAllocatorTableEntries,
                      MmBapFreeBlockAllocatorTableEntry,
                      FALSE);

    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    //
    // Free the memory allocated for the handle table.
    //

    Status = BlMmFreeHeap(MmBlockAllocatorTable);

    //
    // Give precedence to the failure to cleanup table entries.
    //

    if (NT_SUCCESS(ReturnStatus)) {
        ReturnStatus = Status;
    }

    MmBlockAllocatorInitialized = FALSE;

BaDestroyExit:

    BlpInterfaceExit(InterfaceBlkalloc);
    return ReturnStatus;
}

NTSTATUS
MmBaInitialize (
    VOID
    )

/*++

Routine Description:

    This routine initializes global block allocator state. This involves
    allocating buffer for the block allocator handle table and initializing it.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS if memory for handle table could be allocated.

    STATUS_NO_MEMORY otherwise.

--*/

{

    ULONG Size;
    NTSTATUS Status;

    //
    // Allocate the global handle table to store information about all the
    // instantiated memory pools.
    //

    MmBlockAllocatorTableEntries = BLOCK_ALLOCATOR_INITIAL_TABLE_SIZE;
    Size = MmBlockAllocatorTableEntries * sizeof(BLOCK_ALLOCATOR_INFORMATION);
    MmBlockAllocatorTable = BlMmAllocateHeap(Size);
    if (MmBlockAllocatorTable == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BaInitializeExit;
    }

    RtlZeroMemory(MmBlockAllocatorTable, Size);
    MmBlockAllocatorInitialized = TRUE;
    Status = STATUS_SUCCESS;

BaInitializeExit:

    if (!NT_SUCCESS(Status)) {

        if (MmBlockAllocatorTable != NULL) {
            BlMmFreeHeap(MmBlockAllocatorTable);
            MmBlockAllocatorTable = NULL;
        }

        MmBlockAllocatorInitialized = FALSE;
    }

    return Status;
}

// ------------------------------------------------------------ Local Functions

NTSTATUS
MmBapAllocatePagesHelper (
    __out PBLOCK_ALLOCATOR_DESCRIPTOR BlockDescriptor,
    __in PBLOCK_ALLOCATOR_INFORMATION BlockInformation,
    __in ULONG NumberOfBlocks
    )

/*++

Routine Description:

    This is a helper routine that allocates the specified number of memory
    pages.

Arguments:

    BlockDescriptor - Supplies a pointer to a structure that receives
        the starting address for the allocated pages.

    BlockInformation - Supplies a pointer to  a structure that contains
        the attributes to be used while allocating the pages.

    NumberOfBlocks - Supplies the number of blocks that are to be allocated.

Return Value:

    STATUS_SUCCESS if the specified amount of pages could be allocated.

    Appropriate NTSTATUS code otherwise

--*/

{

    PULONG AllocationBitMapBuffer;
    ULONG BitMapBufferSize;
    ULONG BufferSizeInBits;
    ULONGLONG BufferSize;
    ULONGLONG NumberOfPages;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    PVOID VirtualAddress;

    //
    // Allocate the buffer to hold allocation bitmap. The bitmap buffer size
    // must have atleast NumberOfBlocks bits and be an integral of
    // sizeof(ULONG).
    //

    BufferSizeInBits = ALIGN_RANGE_UP(NumberOfBlocks, sizeof(ULONG) * 8);
    if (BufferSizeInBits < NumberOfBlocks) {
        Status = STATUS_INTEGER_OVERFLOW;
        goto MmpAllocatePagesHelperExit;
    }

    BitMapBufferSize = BufferSizeInBits / 8;
    AllocationBitMapBuffer = BlMmAllocateHeap(BitMapBufferSize);
    if (AllocationBitMapBuffer == NULL) {
        Status = STATUS_NO_MEMORY;
        goto MmpAllocatePagesHelperExit;
    }

    //
    // Convert from number of blocks into pages.
    //

    BufferSize = NumberOfBlocks * BlockInformation->BlockUnitSize;
    BufferSize = ALIGN_RANGE_UP(BufferSize, PAGE_SIZE);
    NumberOfPages = BufferSize >> PAGE_SHIFT;

    //
    // Allocate the required number of memory pages.
    //

    if (BlockInformation->MemoryType == PhysicalPages) {
        PhysicalAddress.QuadPart = 0;
        Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                           NumberOfPages,
                                           BlockInformation->PageType,
                                           BlockInformation->Attributes,
                                           BlockInformation->Alignment);

        BlockDescriptor->BlockAddress.PhysicalAddress = PhysicalAddress;

    } else {
        VirtualAddress = NULL;
        Status = BlMmAllocatePages(&VirtualAddress,
                                   NumberOfPages,
                                   BlockInformation->PageType,
                                   BlockInformation->Attributes,
                                   BlockInformation->Alignment);

        BlockDescriptor->BlockAddress.VirtualAddress = VirtualAddress;
    }

    //
    // Initialize the bitmap if the physical pages were allocated. Free the
    // bitmap buffer otherwise.
    //

    if (NT_SUCCESS(Status)) {
        RtlZeroMemory(AllocationBitMapBuffer,
                      BitMapBufferSize);

        RtlInitializeBitMap(&BlockDescriptor->AllocationMap,
                            AllocationBitMapBuffer,
                            BufferSizeInBits);

        //
        // Set all the bits that go beyond the end of allocated pages (i.e,
        // the bitmap padding), so that they are never found free.
        //

        RtlSetBits(&BlockDescriptor->AllocationMap,
                   NumberOfBlocks,
                   BufferSizeInBits - NumberOfBlocks);

    } else {
        BlMmFreeHeap(AllocationBitMapBuffer);
    }

MmpAllocatePagesHelperExit:
    return Status;
}

NTSTATUS
MmBapExtendBlockAllocator (
    __out PBLOCK_ALLOCATOR_DESCRIPTOR *ReturnDescriptor,
    __in PBLOCK_ALLOCATOR_INFORMATION BlockInformation,
    __in ULONG RequiredBlocks
    )

/*++

Routine Description:

    This routine grows the memory pool by atleast the specified number of
    blocks. It also creates a descriptor to describe the newly allocated
    memory and adds it to the descriptor list for the block allocator.

Arguments:

    ReturnDescriptor - Supplies a pointer to a variable that receives
        the block memory descriptor allocated by this routine. The
        descriptor describes the memory block allocated as part of extension.

    BlockInformation - Supplies a pointer to attributes to be used while
        allocating the memory pages.

    RequiredBlocks - Supplies the number of blocks that are to be allocated.

Return Value:

    STATUS_SUCCESS if all the physical pages could be allocated.

    Appropriate NTSTATUS code otherwise

--*/

{

    PBLOCK_ALLOCATOR_DESCRIPTOR BlockDescriptor;
    PLIST_ENTRY BlockMemoryHead;
    BOOLEAN FirstBlock;
    ULONG NumberOfBlocks;
    ULONG Size;
    NTSTATUS Status;

    //
    // Allocate memory for the descriptor.
    //

    NumberOfBlocks = 0;
    BlockMemoryHead = NULL;
    Size = sizeof(BLOCK_ALLOCATOR_DESCRIPTOR);
    BlockDescriptor = BlMmAllocateHeap(Size);
    if (BlockDescriptor == NULL) {
        Status = STATUS_NO_MEMORY;
        goto ExtendBlockAllocatorExit;
    }

    RtlZeroMemory(BlockDescriptor, Size);
    BlockMemoryHead = &BlockInformation->Root.NextBlock;
    FirstBlock = (BOOLEAN)IsListEmpty(BlockMemoryHead);

    //
    // Extend the memory pool by allocating a chunk of memory as follows:
    // 1a. For the first block, allocate the initial number of blocks.
    // 1b. If that fails, allocate the default number of blocks (unless the
    //     default is greater than the initial number of blocks).
    // 3. For all other blocks, allocate the default number of blocks.
    // 4. If above allocations fail, allocate the required number of blocks.
    //
    // Couple of exceptions apply:
    //
    // 1. If initial number of blocks is less than required blocks, then
    //    directly attempt to allocate default number of blocks.
    // 2. If the default number of blocks is less than the required number of
    //    blocks, then directly attempt to allocate the required number of
    //    blocks.
    //

    if ((FirstBlock != FALSE) &&
        (BlockInformation->InitialBlocks > RequiredBlocks)) {

        NumberOfBlocks = BlockInformation->InitialBlocks;

    } else if (BlockInformation->DefaultBlocks > RequiredBlocks) {
        NumberOfBlocks = BlockInformation->DefaultBlocks;

    } else {
        NumberOfBlocks = RequiredBlocks;
    }

    Status = MmBapAllocatePagesHelper(BlockDescriptor,
                                      BlockInformation,
                                      NumberOfBlocks);

    //
    // Skip allocating other block sizes on success.
    //

    if (NT_SUCCESS(Status)) {
        goto ExtendBlockAllocatorExit;
    }

    //
    // Attempt to allocate default number of blocks unless an attempt to
    // allocate fewer number of initial blocks failed or the default number of
    // blocks was already attempted.
    //

    if ((NumberOfBlocks > BlockInformation->DefaultBlocks) &&
        (BlockInformation->DefaultBlocks > RequiredBlocks)) {

        NumberOfBlocks = BlockInformation->DefaultBlocks;
        Status = MmBapAllocatePagesHelper(BlockDescriptor,
                                          BlockInformation,
                                          NumberOfBlocks);

        //
        // Skip allocating other block sizes on success.
        //

        if (NT_SUCCESS(Status)) {
            goto ExtendBlockAllocatorExit;
        }
    }

    //
    // Attempt to allocate required number of blocks unless already attempted.
    //

    if (NumberOfBlocks > RequiredBlocks) {
        NumberOfBlocks = RequiredBlocks;
        Status = MmBapAllocatePagesHelper(BlockDescriptor,
                                          BlockInformation,
                                          NumberOfBlocks);
    }

ExtendBlockAllocatorExit:

    //
    // On successful allocation, insert the descriptor to the head of the list.
    // Inserting new blocks at the head of the list improves the chances of
    // finding free blocks for new allocations in a single descriptor lookup
    // (as other descriptors are expected to be near capacity).
    //

    if (NT_SUCCESS(Status)) {
        BlockDescriptor->TotalBlocks = NumberOfBlocks;
        BlockDescriptor->FreeBlocks = BlockDescriptor->TotalBlocks;
        InsertHeadList(BlockMemoryHead,
                       &BlockDescriptor->NextBlock);

        //
        // Increment the reference count for the block allocator.
        //

        BlockInformation->ReferenceCount += 1;
        *ReturnDescriptor = BlockDescriptor;

    } else {

        //
        // Free up the memory allocated for descriptor on failure.
        //

        if (BlockDescriptor != NULL) {
            BlMmFreeHeap(BlockDescriptor);
        }
    }

    return Status;
}

NTSTATUS
MmBapFreeBlockAllocatorDescriptor (
    __in PBLOCK_ALLOCATOR_INFORMATION BlockInformation,
    __in PBLOCK_ALLOCATOR_DESCRIPTOR BlockDescriptor
    )

/*++

Routine Description:

    This routine frees a block allocator descriptor and also unlinks it from
    the descriptor list for block allocator. It also frees the memory
    pages described by the descriptor and the bitmap buffer used for keeping
    track of in use allocations.

Arguments:

    BlockInformation - Supplies a pointer to the block allocator instance
        that the specified descriptor belongs to.

    BlockDescriptor - Supplies a pointer to the descriptor to be freed.

Return Value:

    STATUS_SUCCESS if all the memory could be successfully freed.

    Appropriate NTSTATUS code otherwise.

--*/

{

    ULONGLONG BufferSize;
    ULONGLONG NumberOfPages;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;

#if DBG

    BOOLEAN AllBitsClear;

    //
    // Ensure that the bitmap is consistent for descriptors that are fully free.
    //
    // N.B. A partially-occupied descriptor might be freed if the block memory
    //      allocator is being destroyed.
    //

    if (BlockDescriptor->FreeBlocks == BlockDescriptor->TotalBlocks) {
        AllBitsClear = RtlAreBitsClear(&BlockDescriptor->AllocationMap,
                                       0,
                                       BlockDescriptor->TotalBlocks);

        ASSERT(AllBitsClear == TRUE);
    }

#endif

    //
    // Free the physical pages and the bitmap buffer.
    //

    ReturnStatus = STATUS_SUCCESS;
    if (BlockInformation->MemoryType == PhysicalPages) {

        //
        // Convert from number of blocks into pages.
        //

        BufferSize = BlockDescriptor->TotalBlocks;
        BufferSize *= BlockInformation->BlockUnitSize;
        BufferSize = ALIGN_RANGE_UP(BufferSize, PAGE_SIZE);
        NumberOfPages = BufferSize >> PAGE_SHIFT;
        Status = BlpMmFreePhysicalPagesEx(
                     BlockDescriptor->BlockAddress.PhysicalAddress,
                     NumberOfPages);

    } else {
        Status = BlMmFreePages(BlockDescriptor->BlockAddress.VirtualAddress);
    }

    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    Status = BlMmFreeHeap(BlockDescriptor->AllocationMap.Buffer);
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    //
    // Unlink the block descriptor from the descriptor list. Decrement the
    // reference count to balance the increment done on insertion.
    //

    RemoveEntryList(&BlockDescriptor->NextBlock);
    BlockInformation->ReferenceCount -= 1;

    //
    // Free the memory allocated for the descriptor.
    //

    Status = BlMmFreeHeap(BlockDescriptor);
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    return ReturnStatus;
}

// ----------------------------------------------- Local Handle Table Functions

BOOLEAN
MmBapCompareBlockAllocatorTableEntry (
    __in PVOID Entry,
    __in PVOID MatchParameter,
    __in PVOID Arg2,
    __in PVOID Arg3,
    __in PVOID Arg4
    )

/*++

Routine Description:


    Compares the given table entry to see if it corresponds to the specified
    handle.

Arguments:

    Entry - Supplies the entry in the table to compare.

    MatchParameter - Supplies the handle value to compare with.

    Arg2 - Unused.

    Arg3 - Unused.

    Arg4 - Unused.

Return Value:

    TRUE if the entry corresponds to the given handle information.

    FALSE otherwise.

--*/

{
    PBLOCK_ALLOCATOR_INFORMATION BlockInformation;
    BLOCK_ALLOCATOR_HANDLE Handle;

    UNREFERENCED_PARAMETER(Arg2);
    UNREFERENCED_PARAMETER(Arg3);
    UNREFERENCED_PARAMETER(Arg4);

    BlockInformation = (PBLOCK_ALLOCATOR_INFORMATION)Entry;
    Handle = (BLOCK_ALLOCATOR_HANDLE)(ULONG_PTR)MatchParameter;
    if (BlockInformation->Handle == Handle) {
        return TRUE;

    } else {
        return FALSE;
    }
}

PBLOCK_ALLOCATOR_INFORMATION
MmBapFindBlockInformation (
    __in BLOCK_ALLOCATOR_HANDLE Handle
    )

/*++

Routine Description:

    This routine walks through the block allocator handle table to find the
    the block allocator instance associated with the given handle.

Arguments:

    BlockHandle - Supplies a handle to the block allocator instance.

Return Value:

    PBLOCK_ALLOCATOR_INFORMATION - A non-NULL value if the associated
        block allocator instance could be found for the handle. NULL otherwise.

--*/

{

    PBLOCK_ALLOCATOR_INFORMATION BlockInformation;
    ULONG EntryId;

    BlockInformation = BlTblFindEntry(MmBlockAllocatorTable,
                                      MmBlockAllocatorTableEntries,
                                      &EntryId,
                                      MmBapCompareBlockAllocatorTableEntry,
                                      (PVOID)(ULONG_PTR)Handle,
                                      NULL,
                                      NULL,
                                      NULL);

    return BlockInformation;
}

NTSTATUS
MmBapFreeBlockAllocatorTableEntry (
    __in PBLOCK_ALLOCATOR_INFORMATION BlockInformation,
    __in ULONG Index
    )

/*++

Routine Description:

    This routine frees a block allocator instance and marks the corresponding
    handle table entry as free.

Arguments:

    BlockInformation - Supplies a pointer to the block allocator instance
        entry inside the handle table to be freed.

    Index - Supplies the index value inside the handle table for the given
        entry.

Return Value:

    PBLOCK_ALLOCATOR_INFORMATION - A non-NULL value if the associated
        block allocator instance could be found for the handle. NULL otherwise.

--*/

{

    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    //
    // The table entry may still have descriptors if this routine is invoked
    // during library destruction. If descriptors are present, free them up
    // first.
    //

    ReturnStatus = STATUS_SUCCESS;
    if (BlockInformation->ReferenceCount > 1) {
        Status = BlpMmDeleteBlockAllocator(BlockInformation->Handle);
        if (!NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }
    }

    //
    // Free the buffer allocated for block allocator information and mark
    // the handle table entry as unused.
    //

    Status = BlMmFreeHeap(BlockInformation);
    if (!NT_SUCCESS(Status)) {
        ReturnStatus = Status;
    }

    MmBlockAllocatorTable[Index] = NULL;
    return ReturnStatus;
}

NTSTATUS
MmBapPurgeBlockAlloctorTableEntry (
    __in PBLOCK_ALLOCATOR_INFORMATION BlockInformation
    )

/*++

Routine Description:

    This routine purges the specified block allocator instance if it is
    not in use. The entries are purged lazily when the handle table becomes
    full and space for new entry needs to be created.

Arguments:

    BlockInformation - Supplies a pointer to the block allocator instance
        entry inside the handle table to be freed.

Return Value:

    STATUS_SUCCESS if the entry is free and could be successfully purged.

    STATUS_UNSUCCESSFUL if the entry is not free.

    Appropriate NTSTATUS code otherwise.

--*/

{

    NTSTATUS Status;

    //
    // The block allocator instance can be purged if there is no reference
    // to it.
    //

    if (BlockInformation->ReferenceCount != 0) {
        return STATUS_UNSUCCESSFUL;
    }

    Status = MmBapFreeBlockAllocatorTableEntry(BlockInformation,
                                               BlockInformation->Handle);

    return Status;
}


