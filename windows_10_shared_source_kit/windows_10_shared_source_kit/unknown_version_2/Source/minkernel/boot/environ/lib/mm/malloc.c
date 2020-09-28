/*++

Copyright (c) Microsoft Corporation

Module Name:

    malloc.c

Abstract:

    Implementation of the Boot Environment Library's Heap Allocator (malloc /
    free).

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "malloc.h"
#include "mm.h"

//
// -------------------------------------------------------------------- Globals
//

LIST_ENTRY MmHeapBoundaries;       // Linked list of heaps.
UINTN *MmFreeList = NULL;          // Global freelist.
ULONG HapMinimumHeapSize;

//
// Global variable indicating the initialization status of the heap allocator.
//

MODULE_INITIALIZATION_STATUS HapInitializationStatus = ModuleUninitialized;

//
// Attributes to use for page allocations of memory to be used by the heap.
//

ULONG HapAllocationAttributes;

//
// Function pointers to allow the RTL to operate in the boot environment.
//

const PRTL_ALLOCATE_STRING_ROUTINE RtlAllocateStringRoutine = (PRTL_ALLOCATE_STRING_ROUTINE)BlMmAllocateHeap;
const PRTL_FREE_STRING_ROUTINE RtlFreeStringRoutine = BlMmFreeHeap;

//
// --------------------------------------------------------------------- Locals
//

static volatile ULONG HeapExtendFailure = 0;

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
MmHaInitialize (
    __in ULONG MinimumHeapSize,
    __in ULONG AllocationAttributes
    )

/*++

Routine Description:

    Initializes the required data structures for the Heap Allocator.

    The initial heap is allocated from the Page Allocator.  The size of the
    allocation is a very rough estimate of what may be required by a
    Boot Environment Application.  The Heap Allocator supports multiple heaps,
    and therefore can grow if the initial allocation is insufficient.

Arguments:

    MinimumHeapSize - Supplies the minimum size, in bytes, that a heap can be.
        This is provided by the application.

    AllocationAttributes - Supplies the attributes to be used when performing
        page allocations for use by the heap.  This is provided by the
        application.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY when unable to initialize heap.

--*/

{

    NTSTATUS Status;

    //
    // Store the minimum heap size and allocation attributes.
    //

    HapMinimumHeapSize = ALIGN_RANGE_UP(MinimumHeapSize, PAGE_SIZE);
    HapAllocationAttributes =
        (AllocationAttributes & SUPPORTED_HEAP_ATTRIBUTES);

    //
    // Initialize MmHeapBoundaries, the global variable used to keep track
    // of each heap maintained by the Heap Allocator.
    //

    MmFreeList = NULL;
    InitializeListHead(&MmHeapBoundaries);

    //
    // Allocate memory from the page allocator for the heap.
    //

    Status = MmHapHeapAllocatorExtend(sizeof(ULONG_PTR));
    if (!NT_SUCCESS(Status)) {
        goto MmHaInitializeEnd;
    }
    //
    // Set a global flag indicating the heap has been initialized.  This flag
    // is required to ensure no allocation operations are performed prior to
    // initialization.
    //

    HapInitializationStatus = ModuleInitialized;

#if DBG_MM

    MmHaPrint();

#endif

    Status = STATUS_SUCCESS;

MmHaInitializeEnd:
    return Status;
}

NTSTATUS
MmHaDestroy (
    VOID
    )

/*++

Routine Description:

    Set the global initialization status to uninitialized, ensuring that no
    additional allocations are made via the heap allocator.

    N.B. There is still memory allocated from the heap that needs to be used
         and referenced.  Most notably are the memory descriptors in each of
         the page allocator's MDLs.  On a platform with a real firmware
         (such as EFI), memory can not be accesses after being freed.  A
         firmware can specifically attempt to catch this behavior.

         As a result, this routine does not free memory allocated for the heap.
         Heap memory is allocated via the page allocator and is freed to the
         firmware during the destruction of the page allocator.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS always.

--*/

{

    //
    // Modify the global state variable to reflect the destruction of the heap.
    // Additionally, increment the call tree count for the global descriptor
    // allocator.  By incrementing this value, no heap allocations will be
    // made for global descriptors.
    //

    HapInitializationStatus = ModuleUninitialized;
    MmEnterDescriptorLibraryApi();
    return STATUS_SUCCESS;
}

PVOID
BlMmAllocateHeap (
    __in UINTN Size
    )

/*++

Routine Description:

    Allocates a Size byte buffer using the Heap Allocator.

    N.B. Returned memory buffers are not zeroed.

Arguments:

    Size - Size (in bytes) of the buffer to allocate.

Return Value:

    A pointer to the allocated memory buffer (sizeof(UINTN) aligned).
    NULL if request fails.

--*/

{

    PVOID CallerBuffer;
    PMALLOC_HEADER NewPtr;
    BOOLEAN RetryHeapAllocation;
    UINTN RequiredSize;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMm);
    CallerBuffer = NULL;

    if (HapInitializationStatus != ModuleInitialized) {
        goto AllocateHeapEnd;
    }

#if DBG_MM

    ConsolePrint(L"BlMmAllocateHeap: Size = 0x%x\r\n", Size);

#endif

    //
    // Determine the required size for the internal buffer which includes both
    // the internal header and the user buffer.  All internal buffers must be
    // properly aligned.  Round up the requested size, ensuring no overflow.
    //

    RequiredSize = ALIGN_RANGE_UP((sizeof(MALLOC_HEADER) + Size), MALLOC_ALIGNMENT);
    if (RequiredSize <= Size) {
        goto AllocateHeapEnd;
    }

    RequiredSize = max(MALLOC_MINIMUM_BUFFER_SIZE, RequiredSize);
    do {

        //
        // Attempt to find a sufficiently sized memory buffer using a buffer
        // in the free-lists or from the heap fringe.
        //

        NewPtr = MmHapFindBuffer(RequiredSize);

        //
        // When a sufficiently sized buffer is found, mark the buffer as
        // allocated and return the buffer to the caller.  If a buffer could not
        // be allocated from the current heap or freelists, attempt to grow the
        // heap.
        //

        if (NewPtr != NULL) {
            SET_ALLOCATED(NewPtr); // Note: Clears fringe bit, if set.

#if DBG_MM

            ConsolePrint(L"BlMmAllocateHeap: Ptr = %08x %08x\r\n",
                         (ULONG)((UINTN)GET_RETURN_BUFFER(NewPtr) >> 32),
                         (ULONG)((UINTN)GET_RETURN_BUFFER(NewPtr)));

#endif

            CallerBuffer = GET_RETURN_BUFFER(NewPtr);
            RetryHeapAllocation = FALSE;

        } else {
            Status = MmHapHeapAllocatorExtend(RequiredSize);
            if (NT_SUCCESS(Status)) {
                RetryHeapAllocation = TRUE;

            } else {
                RetryHeapAllocation = FALSE;
            }
        }

    } while (RetryHeapAllocation != FALSE);

AllocateHeapEnd:
    BlpInterfaceExit(InterfaceMm);
    return CallerBuffer;
}

NTSTATUS
BlMmFreeHeap (
    __in PVOID Ptr
    )

/*++

Routine Description:

    Frees an allocation made by BlMmAllocateHeap.

Arguments:

    Ptr - Buffer to free.

Return Value:

    STATUS_SUCCESS if successful.
    STATUS_INVALID_PARAMETER if Ptr is an invalid pointer (not a pointer
        returned by BlMmAllocateHeap).
    STATUS_UNSUCCESSFUL if heap has yet to be initialized.

--*/

{

    PMALLOC_FREE_HEADER InternalBuffer;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceMm);

    //
    // Quick exit if heap allocator is not yet initialized.
    //

    if (HapInitializationStatus != ModuleInitialized) {
        Status = STATUS_UNSUCCESSFUL;

    } else {

#if DBG_MM

        ConsolePrint(L"BlMmFreeHeap: 0x%08x %08x\r\n",
                     (ULONG)((UINTN)Ptr >> 32),
                     (ULONG)(UINTN)Ptr);

#endif

        //
        // Get a pointer to the internal buffer (header) for the caller's
        // buffer.
        //

        InternalBuffer = GET_INTERNAL_BUFFER(Ptr);

        //
        // Do some parameter checking to have some consistent behavior when
        // given bad arguments.
        //
        //   - Ignore all Free requests for a buffer that was not allocated by
        //     this module.
        //
        //   - Ignore any Free requests for a buffer that is already free
        //     (do not allow a buffer to be double freed).
        //
        //
        // TODO: Put both returns back into a single if block.  They are
        //       split for the time being to add the asserts attempting to
        //       catch callers who free a NULL pointer or attempt to double
        //       free a buffer.
        //

        if (MmHapBufferFoundInHeapBoundaries(InternalBuffer) == FALSE) {

            //
            // Only assert that the pointer is not NULL.  There are legitimate
            // cases where a caller will pass a non-heap allocated address
            // to BlMmFreeHeap.  One such example is the Page Allocator which
            // may pass an entry in the global memory descriptor array when
            // attempting to free a descriptor during coalescing.
            //

            ASSERT(Ptr != NULL);

            Status = STATUS_INVALID_PARAMETER;

        } else if (IS_FREE(InternalBuffer)) {

            ASSERT(!IS_FREE(InternalBuffer));

            Status = STATUS_INVALID_PARAMETER;

        } else {

            //
            // Add the buffer to a freelist.
            //

            InternalBuffer = MmHapAddToFreeList(InternalBuffer, FALSE);

            ASSERT(InternalBuffer != NULL);

            Status = STATUS_SUCCESS;
        }
    }

    BlpInterfaceExit(InterfaceMm);
    return Status;
}

// --------------------------------------------- Consistency Checking Functions

VOID
MmHapReportHeapCorruption (
    __in PMALLOC_HEADER Buffer
    )

/*++

Routine Description:

    This function is called to report detected heap corruption.

Arguments:

    Buffer - Supplies the heap buffer that was being examined when the
        corruption was detected.

Return Value:

    None.

--*/

{

    BlStatusPrint(L"Heap corruption in the links surrounding %p!\n", Buffer);
    BlStatusError(BL_ERROR_HEAP_CORRUPTION, (ULONG_PTR)Buffer, 0, 0, 0);
    return;
}

PMALLOC_FREE_HEADER
MmHapCheckFreeListLinks (
    __in PMALLOC_FREE_HEADER Buffer
    )

/*++

Routine Description:

    This function verifies that the free list links associated with the
    supplied header are well formed.

Arguments:

    Buffer - Supplies the header to check.

Return Value:

    The supplied buffer pointer is returned if the free list links are well
    formed.  NULL is returned if heap corruption is detected.

--*/

{

    PMALLOC_FREE_HEADER NextBufferInFreeList;
    PMALLOC_FREE_HEADER PrevBufferInFreeList;

    //
    // Determine whether the free list links associated with this buffer are
    // well formed.
    //

    NextBufferInFreeList = GET_NEXT_FREELIST(Buffer);
    PrevBufferInFreeList = GET_PREV_FREELIST(Buffer);
    if (((NextBufferInFreeList != NULL) &&
         (GET_PREV_FREELIST(NextBufferInFreeList) != Buffer)) ||
        ((PrevBufferInFreeList != NULL) &&
         (GET_NEXT_FREELIST(PrevBufferInFreeList) != Buffer))) {

        MmHapReportHeapCorruption((PMALLOC_HEADER)Buffer);
        return NULL;
    }

    return Buffer;
}

PMALLOC_HEADER
MmHapCheckBufferLinks (
    __in PMALLOC_HEADER Buffer
    )

/*++

Routine Description:

    This function verifies that the buffer list links associated with the
    supplied header are well formed.

Arguments:

    Buffer - Supplies the header to check.

Return Value:

    The supplied buffer pointer is returned if the buffer list links are well
    formed.  NULL is returned if heap corruption is detected.

--*/

{

    PMALLOC_HEADER NextBuffer;
    PMALLOC_HEADER PrevBuffer;

    //
    // Determine whether the buffer list links associated with this buffer are
    // well formed.
    //

    NextBuffer = GET_NEXT_BUFFER(Buffer);
    PrevBuffer = GET_PREV_BUFFER(Buffer);
    if (((NextBuffer != NULL) &&
         (GET_PREV_BUFFER(NextBuffer) != Buffer)) ||
        ((PrevBuffer != NULL) &&
         (GET_NEXT_BUFFER(PrevBuffer) != Buffer))) {

        MmHapReportHeapCorruption(Buffer);
        return NULL;
    }

    return Buffer;
}

// ------------------------------------------------------------ Debug Functions

#if DBG_MM

VOID
MmHaPrint (
    VOID
    )

/*++

Routine Description:

    Prints the freelists and global Heap Variables.  This routine is implemented
    for debug purposes.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PMALLOC_FREE_HEADER Header;
    PHEAP_BOUNDARY HeapBoundary;
    ULONG Index;
    PLIST_ENTRY NextEntry;

    if (HapInitializationStatus == ModuleInitialized) {

        //
        // Go through each heap, printing off the regions that are and are not
        // allocated.
        //

        Index = 1;
        NextEntry = MmHeapBoundaries.Flink;
        while (NextEntry != &MmHeapBoundaries) {
            HeapBoundary = CONTAINING_RECORD(NextEntry,
                                             HEAP_BOUNDARY,
                                             ListEntry);

            ConsolePrint(L"Heap %x\r\n", Index);
            ConsolePrint(L"\tAllocated:   [%08x %08x - %08x %08x]\r\n",
                         (ULONG)((ULONGLONG)HeapBoundary->HeapLow >> 32),
                         (ULONG)HeapBoundary->HeapLow,
                         (ULONG)((ULONGLONG)HeapBoundary->HeapFringe >> 32),
                         (ULONG)HeapBoundary->HeapFringe);

            ConsolePrint(L"\tUnAllocated: [%08x %08x - %08x %08x]\r\n",
                         (ULONG)((ULONGLONG)HeapBoundary->HeapFringe >> 32),
                         (ULONG)HeapBoundary->HeapFringe,
                         (ULONG)((ULONGLONG)HeapBoundary->HeapHigh >> 32),
                         (ULONG)HeapBoundary->HeapHigh);

            NextEntry = NextEntry->Flink;
            Index += 1;
        }

        //
        // Walk down the freelist buckets, printing off the array of buffers
        // in each freelist bucket.
        //

        ConsolePrint(L"FreeList: \r\n");
        for (Index = 0; Index < NUMBER_FREELIST_BINS; Index++) {
            ConsolePrint(L"BIN: 0x%x [%08x %08x]\r\n",
                         Index,
                         (ULONG)((ULONGLONG)GET_FREELIST(Index) >> 32),
                         (ULONG)(ULONGLONG)GET_FREELIST(Index));

            //
            // Walk the freelist bucket.  Print out each buffer in the list.
            //

            Header = GET_FREELIST_HEAD(Index);
            while (Header != NULL) {
                ConsolePrint(L"\t[%08x %08x]:%x:%x:%x\r\n",
                             (ULONG)((ULONGLONG)Header >> 32),
                             (ULONG)(UINTN)Header,
                             (ULONG)GET_BUFFER_SIZE(Header),
                             IS_FREE(Header) ? 1 : 0,
                             IS_FRINGE(Header) ? 1 : 0);

                Header = GET_NEXT_FREELIST(Header);
            }
        }
    }
}

#endif DBG_MM

NTSTATUS
MmHaValidate (
    VOID
    )

/*++

Routine Description:

    Attempts to validate internal Heap Allocator data structures by walking
    each heap inspecting each allocated buffer, ensuring that each adjacent
    buffer references the current buffer.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS if successful.
    STATUS_UNSUCCESSFUL if there are any inconsistent data structures (in the
        free list or in the heap itself).

--*/

{

    PMALLOC_FREE_HEADER FreeListHeader;
    PMALLOC_HEADER Header;
    PHEAP_BOUNDARY HeapBoundary;
    ULONG Index;
    PLIST_ENTRY NextEntry;
    PMALLOC_FREE_HEADER NextFreeListHeader;
    PMALLOC_HEADER NextHeader;
    PMALLOC_FREE_HEADER PreviousFreeListHeader;
    PMALLOC_HEADER PreviousHeader;

    if (HapInitializationStatus != ModuleInitialized) {
        return STATUS_SUCCESS;
    }

    //
    // Validate each heap.
    //

    NextEntry = MmHeapBoundaries.Flink;
    while (NextEntry != &MmHeapBoundaries) {
        HeapBoundary = CONTAINING_RECORD(NextEntry, HEAP_BOUNDARY, ListEntry);

        //
        // Walk the allocated pool forwards.
        //

        Header = (PMALLOC_HEADER) HeapBoundary->HeapLow;
        while (!IS_FRINGE(Header)) {
            NextHeader = GET_NEXT_BUFFER(Header);
            PreviousHeader = GET_PREV_BUFFER(Header);

            ASSERT((NextHeader == NULL) ||
                   (GET_PREV_BUFFER(NextHeader) == Header));

            ASSERT((PreviousHeader == NULL) ||
                   (GET_NEXT_BUFFER(PreviousHeader) == Header));

            //
            // Make sure the next header points back to this header.
            //

            if ((NextHeader != NULL) &&
                (GET_PREV_BUFFER(NextHeader) != Header)) {

#if DBG_MM

                ConsolePrint(L"Inconsistent Header/Previous(Next) [%08x:%008x]\r\n",
                             (ULONG)(ULONG_PTR) Header,
                             (ULONG)(ULONG_PTR) GET_PREV_BUFFER(NextHeader));

#endif

                MmHapReportHeapCorruption(Header);
                return STATUS_UNSUCCESSFUL;
            }

            //
            // Make sure the prev header points to this header.
            //

            if ((PreviousHeader != NULL) &&
                (GET_NEXT_BUFFER(PreviousHeader) != Header)) {

#if DBG_MM

                ConsolePrint(L"Inconsistent Header/Next(Previous) [%08x:%008x]\r\n",
                             (ULONG)(ULONG_PTR) Header,
                             (ULONG)(ULONG_PTR) GET_NEXT_BUFFER(PreviousHeader));

#endif

                MmHapReportHeapCorruption(Header);
                return STATUS_UNSUCCESSFUL;
            }

            //
            // Ensure that adjacent entries are ascending in address.
            //

            ASSERT((NextHeader == NULL) ||
                   ((ULONG_PTR)Header < (ULONG_PTR)NextHeader));

            ASSERT((ULONG_PTR)PreviousHeader < (ULONG_PTR)Header);

            if ((NextHeader != NULL) &&
                ((ULONG_PTR)Header >= (ULONG_PTR)NextHeader)) {

#if DBG_MM

                ConsolePrint(L"Non-ascending heap buffer (NextHeader) [%08x:%008x]\r\n",
                             (ULONG)(ULONG_PTR)Header,
                             (ULONG)(ULONG_PTR)NextHeader);

#endif

                MmHapReportHeapCorruption(Header);
                return STATUS_UNSUCCESSFUL;
            }

            if ((ULONG_PTR)PreviousHeader >= (ULONG_PTR)Header) {

#if DBG_MM

                ConsolePrint(L"Non-ascending heap buffer (PreviousHeader) [%08x:%008x]\r\n",
                             (ULONG)(ULONG_PTR)PreviousHeader,
                             (ULONG)(ULONG_PTR)Header);

#endif

                MmHapReportHeapCorruption(Header);
                return STATUS_UNSUCCESSFUL;
            }

            Header = NextHeader;
        }

        NextEntry = NextEntry->Flink;
    }

    //
    // Validate the freelists.
    //

    for (Index = 0; Index < NUMBER_FREELIST_BINS; Index += 1) {
        FreeListHeader = GET_FREELIST_HEAD(Index);

        //
        // Walk the freelist.
        //

        while (FreeListHeader != NULL) {
            NextFreeListHeader = GET_NEXT_FREELIST(FreeListHeader);
            PreviousFreeListHeader = GET_PREV_FREELIST(FreeListHeader);

            ASSERT((NextFreeListHeader == NULL) ||
                   (GET_PREV_FREELIST(NextFreeListHeader) == FreeListHeader));

            ASSERT((PreviousFreeListHeader == NULL) ||
                   (GET_NEXT_FREELIST(PreviousFreeListHeader) == FreeListHeader));

            //
            // Make sure the next header points back to this header.
            //

            if ((NextFreeListHeader != NULL) &&
                (GET_PREV_FREELIST(NextFreeListHeader) != FreeListHeader)) {

                MmHapReportHeapCorruption((PMALLOC_HEADER)FreeListHeader);
                return STATUS_UNSUCCESSFUL;
            }

            //
            // Make sure the prev header points to this header
            //

            if ((PreviousFreeListHeader != NULL) &&
                (GET_NEXT_FREELIST(PreviousFreeListHeader) != FreeListHeader)) {

                MmHapReportHeapCorruption((PMALLOC_HEADER)FreeListHeader);
                return STATUS_UNSUCCESSFUL;
            }

            FreeListHeader = NextFreeListHeader;
        }
    }

    return STATUS_SUCCESS;
}

// --------------------------------------------------------- Internal Functions

PMALLOC_HEADER
MmHapFindBuffer (
    __in UINTN Size
    )

/*++

Routine Description:

    Given the current view of the heap, find a buffer of sufficient size
    for the specified allocation request.

Arguments:

    Size - The internal buffer size required for the allocation.

Return Value:

    A pointer to an internal buffer of sufficient size for the requested
    allocation.

    NULL if a sufficient buffer is not found.

--*/

{

    ULONG_PTR EndOfAllocation;
    PHEAP_BOUNDARY HeapBoundary;
    PMALLOC_HEADER NewPtr;
    NTSTATUS Status;

    //
    // Attempt to reuse an allocation by searching the free-lists for a free
    // buffer that can service the caller's request.
    //

    NewPtr = MmHapFindBufferInFreeList(Size);

    //
    // If a buffer was not found in the freelists, we must allocate one from
    // the heap.
    //

    if (NewPtr == NULL) {
        if (!IsListEmpty(&MmHeapBoundaries)) {

            //
            // If the new buffer can be carved from the heap, simply adjust
            // HeapFringe to illustrate the allocation from the Heap.
            //
            // N.B. The head of MmHeapBoundaries is the most recently allocated
            // heap.  Only one heap can be active at a time.  When a new heap
            // is allocated, the remainder of the existing heap is added to the
            // freelists.
            //

            HeapBoundary = CONTAINING_RECORD(MmHeapBoundaries.Flink,
                                             HEAP_BOUNDARY,
                                             ListEntry);

            //
            // Keep room for one header at the end of the heap which is used
            // to signify the end of the heap.
            //

            Status = RtlULongPtrAdd(HeapBoundary->HeapFringe,
                                    Size,
                                    &EndOfAllocation);

            if (NT_SUCCESS(Status) &&
                (EndOfAllocation <=
                 (HeapBoundary->HeapHigh - sizeof(MALLOC_HEADER)))) {

                //
                // Carve the new buffer from the heap.
                //

                NewPtr = (PMALLOC_HEADER)HeapBoundary->HeapFringe;
                HeapBoundary->HeapFringe += Size;

                //
                // Set next/prev pointers.
                //

                SET_NEXT_BUFFER(NewPtr, HeapBoundary->HeapFringe);
                SET_PREV_BUFFER(HeapBoundary->HeapFringe, NewPtr);
                SET_FREE(HeapBoundary->HeapFringe);
                SET_FRINGE(HeapBoundary->HeapFringe);
            }
        }
    }

    return NewPtr;
}

ULONG
MmHapLog (
    __in UINTN Num
    )

/*++

Routine Description:

    Determines the Log Base 2 of Num.

Arguments:

    Num - number to find the logarithm for.

Return Value:

    Log base 2 of Num.
    -1 is returned if 0 is Num.

--*/

{

    ULONG x;

    //
    // Right shift Num until the resultant is zero.
    //

    x = 0;
    while (Num >> x) {
        x += 1;
    }

    return (x - 1);
}

ULONG
MmHapGetBucketId (
    __in UINTN Size
    )

/*++

Routine Description:

    Given an internal buffer size, this routine will return the id for the
    freelist bucket that should contain buffers of this size.

Arguments:

    Size - Size of an internal buffer.

Return Value:

    Bucket ID.

--*/

{

    return ((Size < PAGE_SIZE) ? (MmHapLog(Size) - MIN_BUFFER_SIZE_SHIFT) :
                                 (NUMBER_FREELIST_BINS - 1));
}

PMALLOC_FREE_HEADER
MmHapAddToFreeList (
    __in PMALLOC_FREE_HEADER Ptr,
    __in BOOLEAN SuppressZero
    )

/*++

Routine Description:

    Adds a buffer to the freelist in the appropriate bucket.

    The status field will be changed in the header to indicate the buffer
    is free.

    Before inserting into the freelist, coalescing will be attempted with
    adjacent buffers.

Arguments:

    Ptr - Pointer to the buffer to add to the freelists.

    SuppressZero - Supplies a boolean indicating whether to suppress
        zeroing of the buffer for cases where it is not required because the
        memory was not previously used, or was already zeroed.

Return Value:

    Pointer to buffer that was inserted into the freelist.  This is not
    necessarily the same buffer as Ptr, since coalescing may have
    occurred.  NULL is returned if heap corruption is detected.

--*/

{

    PMALLOC_HEADER Buffer;
    PMALLOC_FREE_HEADER CurrentHeadPtr;
    ULONG Index;
    LIBRARY_PARAMETERS LibraryParameters;
    PMALLOC_HEADER NextBuffer;
    PMALLOC_HEADER PrevBuffer;

    //
    // Do a basic check on the header, ensuring the adjacent buffers point
    // back to this buffer.
    //

    NextBuffer = GET_NEXT_BUFFER(Ptr);
    PrevBuffer = GET_PREV_BUFFER(Ptr);
    Buffer = (PMALLOC_HEADER)Ptr;
    if ((GET_PREV_BUFFER(NextBuffer) != Buffer) ||
        ((PrevBuffer != NULL) && (GET_NEXT_BUFFER(PrevBuffer) != Buffer))) {

#if DBG_MM

        ConsolePrint(L"MmHapAddToFreeList: Header Prev/Next mismatch: %08x %08x\r\n",
                     (ULONG)((UINTN)Ptr >> 32),
                     (ULONG)(UINTN)Ptr);

        ConsolePrint(L"Prev: %08x Next: %08x\r\n",
                     (ULONG)(UINTN)PrevBuffer,
                     (ULONG)(UINTN)NextBuffer);

        ConsolePrint(L"NextPrev: %08x PrevNext: %08x\r\n",
                     (ULONG)(UINTN)GET_PREV_BUFFER(NextBuffer),
                     (ULONG)(UINTN)GET_NEXT_BUFFER(PrevBuffer));

#endif

        MmHapReportHeapCorruption(Buffer);
        return NULL;
    }

    //
    // When specified, zero the free memory buffer.
    //

    LibraryParameters = BlGetCurrentLibraryParameters();
    if ((CHECK_FLAG(LibraryParameters.Flags, LF_ZERO_SENSITIVE_DATA) != FALSE)
        && (SuppressZero == FALSE)) {

        RtlSecureZeroMemory(GET_RETURN_BUFFER(Ptr),
                            GET_RETURN_BUFFER_SIZE(Ptr));
    }

    //
    // Mark the buffer as free.
    //

    SET_FREE(Ptr);

    //
    // Attempt to coalesce with adjacent buffers.
    //

    Ptr = MmHapCoalesceFreeBuffer(Ptr);
    if (Ptr != NULL) {

        //
        // Add the buffer to the head of the appropriate freelist bucket.
        //

        Index = MmHapGetBucketId(GET_BUFFER_SIZE(Ptr));
        CurrentHeadPtr = GET_FREELIST_HEAD(Index);

        //
        // Update the FreeListPrev for the current head.
        //

        if (CurrentHeadPtr != NULL) {
            SET_PREV_FREELIST(CurrentHeadPtr, Ptr);
        }

        //
        // Update FreeListNext/Prev for the Buffer we are adding to the bucket
        //

        SET_PREV_FREELIST(Ptr, 0);
        SET_NEXT_FREELIST(Ptr, CurrentHeadPtr);

        //
        // Lastly, update the pointer in the freelist to point to the head
        //

        SET_FREELIST_HEAD(Index, Ptr);
    }

    return Ptr;
}

PMALLOC_HEADER
MmHapFindBufferInFreeList (
    __in UINTN Size
    )

/*++

Routine Description:

    Searches the freelists for a buffer of that is at least the requested size.

    If found, the buffer will be removed from the freelist and returned.

Arguments:

    Size - Requested size (in bytes) for a buffer.

Return Value:

    Pointer to buffer of at least Size bytes if found in the freelist NULL if
    buffer can not be found.

--*/

{


    UINTN BufferSize;
    ULONG Index;
    PMALLOC_FREE_HEADER NewPtr;
    PMALLOC_HEADER SplitPtr;
    PMALLOC_HEADER ValidatedBuffer;

    ASSERT(MALLOC_MINIMUM_BUFFER_SIZE <= Size);

    //
    // There is one freelist bucket for every power of 2.  Start the buffer
    // search at the bin which would hold a buffer of the requested size.
    // Every bucket after this contains buffers of greater size and, therefore,
    // can still be used.
    //

    for (NewPtr = NULL, Index = MmHapGetBucketId(Size);
         (Index < NUMBER_FREELIST_BINS) && (NewPtr == NULL);
         Index += 1) {

        //
        // Try to find buffer in this bucket that will fit our needs.
        //

        NewPtr = GET_FREELIST_HEAD(Index);
        while ((NewPtr != NULL) && (GET_BUFFER_SIZE(NewPtr) < Size)) {
            NewPtr = GET_NEXT_FREELIST(NewPtr);
        }
    }

    if (NewPtr != NULL) {

        //
        // Found a sufficient buffer in the freelists.  Attempt to remove the
        // buffer from the list.
        //

        NewPtr = MmHapRemoveBufferFromFreeList(NewPtr);
        if (NewPtr != NULL) {

            //
            // If the buffer that we found is larger than needed, see if it can
            // be split up.
            //

            ValidatedBuffer = MmHapCheckBufferLinks((PMALLOC_HEADER)NewPtr);
            if (ValidatedBuffer == NULL) {
                NewPtr = NULL;

            } else {
                BufferSize = GET_BUFFER_SIZE(NewPtr);
                if (MALLOC_MINIMUM_BUFFER_SIZE <= (BufferSize - Size)) {

                    //
                    // Split the buffer into two.
                    //

                    SplitPtr = Add2Ptr(NewPtr, Size);

                    //
                    // Set Prev/Next buffer pointers.
                    //

                    SET_NEXT_BUFFER(SplitPtr, GET_NEXT_BUFFER(NewPtr));
                    SET_PREV_BUFFER(GET_NEXT_BUFFER(NewPtr), SplitPtr);
                    SET_PREV_BUFFER(SplitPtr, NewPtr);
                    SET_NEXT_BUFFER(NewPtr, SplitPtr);

                    //
                    // Now add the second part of the split buffer to the
                    // freelist and suppress zeroing of this buffer as it
                    // from heap that is already free.
                    //

                    MmHapAddToFreeList((PMALLOC_FREE_HEADER)SplitPtr, TRUE);
                }
            }
        }
    }

    return (PMALLOC_HEADER)NewPtr;
}

PMALLOC_FREE_HEADER
MmHapCoalesceFreeBuffer (
    __in PMALLOC_FREE_HEADER Buffer
    )

/*++

Routine Description:

    Attempts to coalesce free memory buffer with adjacent buffers.  If either
    adjacent buffer is also free, it is removed from its respective freelist
    and coalesces with the specified buffer.

Arguments:

    Buffer - Free memory buffer.

Return Value:

    Pointer to the coalesced buffer.  NULL is returned if heap corruption is
    detected.

--*/

{

    PMALLOC_FREE_HEADER NextBuffer;
    PMALLOC_FREE_HEADER PrevBuffer;
    PMALLOC_HEADER ValidatedBuffer;

    ValidatedBuffer = MmHapCheckBufferLinks((PMALLOC_HEADER)Buffer);
    if (ValidatedBuffer == NULL) {
        return NULL;
    }

    //
    // Attempt to coalesce with the next buffer.
    //

    NextBuffer = (PMALLOC_FREE_HEADER)GET_NEXT_BUFFER(Buffer);
    if (!IS_FRINGE(NextBuffer) && IS_FREE(NextBuffer)) {

        //
        // Since we are coalescing Buffer and NextBuffer, remove Next
        // Buffer from the freelist.
        //

        NextBuffer = MmHapRemoveBufferFromFreeList(NextBuffer);
        if (NextBuffer == NULL) {
            return NULL;
        }

        //
        // Set the new pointers.
        //

        SET_PREV_BUFFER(GET_NEXT_BUFFER(NextBuffer), Buffer);
        SET_NEXT_BUFFER(Buffer, GET_NEXT_BUFFER(NextBuffer));

        //
        // Rewrite the status bit for this buffer.  It is part of the next
        // buffer field.  Since that was just written to, we have to
        // rewrite the status bit.
        //

        SET_FREE(Buffer);
    }

    //
    // Attempt to coalesce with the previous buffer.
    //

    PrevBuffer = (PMALLOC_FREE_HEADER)GET_PREV_BUFFER(Buffer);
    if ((PrevBuffer != NULL) && IS_FREE(PrevBuffer)) {

        //
        // Since we are coalescing Buffer and PrevBuffer, remove Prev
        // Buffer from the freelist.
        //

        PrevBuffer = MmHapRemoveBufferFromFreeList(PrevBuffer);
        if (PrevBuffer == NULL) {
            return NULL;
        }

        //
        // Set the new pointers.
        //

        SET_PREV_BUFFER(GET_NEXT_BUFFER(Buffer), PrevBuffer);
        SET_NEXT_BUFFER(PrevBuffer, GET_NEXT_BUFFER(Buffer));

        //
        // Rewrite the status bit for the previous buffer.  It is part of
        // the next buffer field.  Since that was just written to, we have
        // to rewrite the status bit.
        //

        SET_FREE(PrevBuffer);

        //
        // Set the current buffer's pointer to the previous buffer since
        // they have been coalesce.  This way, the caller will get the
        // start of the coalesced buffer as it's return value.
        //

        Buffer = PrevBuffer;
    }

    return Buffer;
}

PMALLOC_FREE_HEADER
MmHapRemoveBufferFromFreeList (
    __in PMALLOC_FREE_HEADER Buffer
    )

/*++

Routine Description:

    Removes the specified buffer from its respective freelist.

    N.B. The specified buffer MUST currently be in a free list.

Arguments:

    Buffer - Freed memory buffer.

Return Value:

    Pointer to the Buffer that was removed.  NULL is returned if heap
    corruption is detected.

--*/

{

    PMALLOC_FREE_HEADER NextBufferInFreeList;
    PMALLOC_FREE_HEADER PrevBufferInFreeList;

    ASSERT(IS_FREE(Buffer));

    NextBufferInFreeList = GET_NEXT_FREELIST(Buffer);
    PrevBufferInFreeList = GET_PREV_FREELIST(Buffer);

    //
    // Adjust the previous / next pointers of the adjacent entries in the
    // freelist.  If the buffer is the first entry in a free list, update
    // the value in the freelist head.
    //

    Buffer = MmHapCheckFreeListLinks(Buffer);
    if (Buffer == NULL) {
        return NULL;
    }

    if (NextBufferInFreeList != NULL) {
        SET_PREV_FREELIST(NextBufferInFreeList, PrevBufferInFreeList);
    }

    if (PrevBufferInFreeList != NULL) {
        SET_NEXT_FREELIST(PrevBufferInFreeList, NextBufferInFreeList);

    } else {
        SET_FREELIST_HEAD(MmHapGetBucketId(GET_BUFFER_SIZE(Buffer)),
                          NextBufferInFreeList);
    }

    return Buffer;
}

BOOLEAN
MmHapBufferFoundInHeapBoundaries (
    __in PMALLOC_FREE_HEADER Address
    )

/*++

Routine Description:

    Determines if the specified address falls within the boundaries of one
    of the heaps managed by the Heap Allocator.

Arguments:

    Address - Address to check.

Return Value:

    TRUE if the specified address lies within one of the heaps managed by
        the Heap Allocator.
    FALSE otherwise.

--*/

{

    PHEAP_BOUNDARY HeapBoundary;
    PLIST_ENTRY NextEntry;

    //
    // Walk the list of heap's.
    //

    NextEntry = MmHeapBoundaries.Flink;
    while (NextEntry != &MmHeapBoundaries) {
        HeapBoundary = CONTAINING_RECORD(NextEntry, HEAP_BOUNDARY, ListEntry);
        if ((HeapBoundary->HeapLow <= (UINTN)Address) &&
            ((UINTN)Address < HeapBoundary->HeapFringe)) {

            return TRUE;
        }

        NextEntry = NextEntry->Flink;   // Advance pointer.
    }

    return FALSE;
}

NTSTATUS
MmHapHeapAllocatorExtend (
    __in UINTN AllocationSize
    )

/*++

Routine Description:

    Allocates and initializes a heap for the Heap Allocator.  If no heaps
    have previously been allocated, this routine will additionally initialize
    the freelists using part of the heap.

Arguments:

    AllocationSize - The size of the allocation which generated the extend.
        The new heap will be large enough to service the requested allocation.

Return Value:

    STATUS_SUCCESS if successful.
    STATUS_NO_MEMORY if allocation fails.

--*/

{

    PVOID Heap;
    ULONG_PTR HeapAllocationSize;
    PHEAP_BOUNDARY HeapBoundary;
    PMALLOC_FREE_HEADER NewHighBuffer;
    UINTN Scratch;
    NTSTATUS Status;

    //
    // Ensure that the new heap will be sufficiently large for the allocation
    // request generating the extend call.  Since internal data structures are
    // carved out of the heap, make sure the allocation is a couple of pages
    // larger than the caller's request.
    //

    Status = RtlULongPtrAdd(AllocationSize, 2 * PAGE_SIZE, &HeapAllocationSize);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    HeapAllocationSize = max(HapMinimumHeapSize, HeapAllocationSize);
    HeapAllocationSize = ALIGN_RANGE_UP(HeapAllocationSize, PAGE_SIZE);
    if (HeapAllocationSize == 0) {
        return STATUS_INTEGER_OVERFLOW;
    }

    //
    // Since both the Page Allocator and Heap Allocator ultimately manage
    // the same resource, allocations and initialization of either can be
    // problematic.  The Page Allocator requires heap to maintain memory
    // descriptor lists which describe system memory.  An allocation request
    // can cause a memory region to split, which would require the Page
    // Allocator to require a new descriptor to accurately describe memory.
    // By reserving a page of memory, the Heap Allocator can service the
    // Page Allocator's request and transitively, its request for a new heap.
    //
    // The last page of each heap is reserved for precisely this case.  Adjust
    // the fields of the heap to expose this memory to the heap allocator.
    //

    if (!IsListEmpty(&MmHeapBoundaries)) {
        HeapBoundary = CONTAINING_RECORD(MmHeapBoundaries.Flink,
                                         HEAP_BOUNDARY,
                                         ListEntry);

        //
        // Check if the heap boundary was adjusted previously due to a failed
        // extend attempt. If so, then don't re-adjust it.
        //

        if ((HeapBoundary->HeapHigh + PAGE_SIZE) <= HeapBoundary->HeapLimit) {
            HeapBoundary->HeapHigh += PAGE_SIZE;
            NewHighBuffer = GET_INTERNAL_BUFFER(HeapBoundary->HeapHigh);
            SET_NEXT_BUFFER(HeapBoundary->HeapFringe, NewHighBuffer);
            SET_PREV_BUFFER(NewHighBuffer, HeapBoundary->HeapFringe);
            SET_FREE(NewHighBuffer);
            SET_FRINGE(NewHighBuffer);

            //
            // Add the remaining fringe to the freelists.  That way, the most
            // recently allocated heap is the only one that ever has an active
            // fringe. Suppress zeroing of the heap fringe when adding it to
            // the free list as it has never contained any sensitive user or
            // application data.
            //

            CLEAR_FRINGE(HeapBoundary->HeapFringe);
            MmHapAddToFreeList((PMALLOC_FREE_HEADER)HeapBoundary->HeapFringe,
                               TRUE);

            HeapBoundary->HeapFringe = (UINTN)NewHighBuffer;
        }
    }

    //
    // Allocate pages for the new heap.
    //

    Heap = NULL;
    Status = BlMmAllocatePages(&Heap,
                               HeapAllocationSize >> PAGE_SHIFT,
                               MEMORY_TYPE_HEAP,
                               HapAllocationAttributes,
                               0); // No Alignment requirement

    if (!NT_SUCCESS(Status)) {
        HeapExtendFailure = 1; // For !analyze debugger support.
        return Status;
    }

    //
    // Initialize the newly allocated heap.  If this is the first heap
    // allocated, the freelist will be carved from the top of the heap.
    //
    // As described above, one page is reserve for heap allocations by the
    // Page Allocator during subsequent calls to this routine to grow the
    // Heap Allocator.
    //
    // Lastly, the memory required for the HEAP_BOUNDARY data structure is
    // carved from the start of the newly allocated heap.
    //

    //
    // Initialize HEAP_BOUNDARY structure to describe the heap.
    //

    Scratch = (UINTN)Heap;
    HeapBoundary = GET_RETURN_BUFFER(Scratch);
    HeapBoundary->HeapLow = Scratch;
    HeapBoundary->HeapHigh = Scratch + HeapAllocationSize;

    //
    // The bottom of the heap will start just after this structure.
    //

    HeapBoundary->HeapFringe = (UINTN)HeapBoundary + sizeof(HEAP_BOUNDARY);

    //
    // Initialize the internal buffer header for the buffer used by this
    // structure.
    //

    SET_PREV_BUFFER(Scratch, 0);
    SET_NEXT_BUFFER(Scratch, HeapBoundary->HeapFringe);
    SET_ALLOCATED(Scratch);

    //
    // Set up the header for the fringe.
    //

    SET_PREV_BUFFER(HeapBoundary->HeapFringe, Scratch);
    SET_NEXT_BUFFER(HeapBoundary->HeapFringe, HeapBoundary->HeapFringe);
    SET_FREE(HeapBoundary->HeapFringe);
    SET_FRINGE(HeapBoundary->HeapFringe);

    //
    // If this is the first heap allocation, initialize the free lists.
    //

    if (IsListEmpty(&MmHeapBoundaries)) {
        MmFreeList = (UINTN *)HeapBoundary->HeapHigh - NUMBER_FREELIST_BINS;
        HeapBoundary->HeapHigh = (UINTN)MmFreeList;

        memset(MmFreeList, 0, NUMBER_FREELIST_BINS * sizeof(MmFreeList));
    }

    //
    // Save a page to be used when growing the heap.
    //

    HeapBoundary->HeapLimit = HeapBoundary->HeapHigh;
    HeapBoundary->HeapHigh -= PAGE_SIZE;

    //
    // Add the newly allocated heap to the global list.  The newest heap
    // should always be in the front since it is the only heap with an
    // active fringe.
    //

    InsertHeadList(&MmHeapBoundaries, &HeapBoundary->ListEntry);
    return STATUS_SUCCESS;
}

