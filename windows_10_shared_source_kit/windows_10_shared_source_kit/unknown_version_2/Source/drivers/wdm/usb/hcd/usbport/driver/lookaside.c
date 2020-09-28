/*++

Copyright (c) 2013 Microsoft Corporation

Module Name:

    lookaside.c

Abstract:

    Allocation and Free functions for frequent USB allocations

Environment:

    kernel mode only

Notes:

Revision History:

    04-05-13 : created

--*/

#include "common.h"

//
// We will allocate look aside entries of sizes
// 0x200, 0x400, 0x600 ... 0x1000
//
#define MIN_LOOKASIDE_ALLOCATION    (PAGE_SIZE >> 3)
#define NUM_LOOKASIDE_ENTRIES       (PAGE_SIZE / MIN_LOOKASIDE_ALLOCATION)
#define MAX_LOOKASIDE_ALLOCATION    (PAGE_SIZE)

#define USB_LOOKASIDE_TAG           'Lbsu'
#define USB_GENERICTABLE_TAG        'Tbsu'
#define LOOKASIDE_PADDING_SIGN       0xbaadf00d

//
// Time period for lookaside flushes = 2 mins
//
#define LOOKASIDE_FLUSH_PERIOD_IN_MS  (120000)

#if _WIN64
#define MEM_ALIGN __declspec(align(16))
#else
#define MEM_ALIGN
#endif

typedef struct _USB_LOOKASIDE_ENTRY {
    ULONG                           Size;
    LONG                            Count;
    MEM_ALIGN LOOKASIDE_LIST_EX     Lookaside;
    ULONGLONG                       LastFlushTime;
} USB_LOOKASIDE_ENTRY,   *PUSB_LOOKASIDE_ENTRY;

//
// Disabling warning: The use of 0 sized array is OK and intended
// warning C4200: nonstandard extension used: zero-sized array in struct/union
//
#pragma warning(push)
#pragma warning(disable:4200)

_Struct_size_bytes_(LookasideEntry->Size)
typedef struct _USB_ALLOCATION_HEADER {
    PUSB_LOOKASIDE_ENTRY  LookasideEntry;
    ULONG                 Size;
    UCHAR                 Buffer[0];
} USB_ALLOCATION_HEADER,  *PUSB_ALLOCATION_HEADER;

#pragma warning(pop)

PUSB_LOOKASIDE_ENTRY   g_LookasideEntries[NUM_LOOKASIDE_ENTRIES];

#if DBG
//
// Used for debugging
//
VOID UT_LookasideAllocateAndFreeTest();
VOID UsbLookasideCheckSignature(
    __in PVOID Ptr
    );
VOID UsbLookasideFillSignature(
    __in PVOID Ptr,
    __in ULONG AllocationSize,
    __in ULONG LookasideSize
    );
#endif

//
// Private functions
//
FORCEINLINE
PUSB_LOOKASIDE_ENTRY
GetLookasideAllocationEntry(
    __in ULONG NumberOfBytes
    );


NTSTATUS
USBPORT_InitLookasideLists()
/*++

Routine Description:

    Initialize lookaside list entries and table

Arguments:
    None

Return Value:

    NTSTATUS

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG    i = 0;

    RtlZeroMemory(g_LookasideEntries,
                  sizeof(PUSB_LOOKASIDE_ENTRY) * NUM_LOOKASIDE_ENTRIES);

    for (i = 0; i < NUM_LOOKASIDE_ENTRIES; i++) {

        g_LookasideEntries[i] = ExAllocatePoolWithTag(NonPagedPoolNx,
                                                      sizeof(USB_LOOKASIDE_ENTRY),
                                                      USB_LOOKASIDE_TAG);

        if (NULL == g_LookasideEntries[i]) {
            status = STATUS_NO_MEMORY;
            break;
        }

        RtlZeroMemory(g_LookasideEntries[i], sizeof(USB_LOOKASIDE_ENTRY));

        g_LookasideEntries[i]->Size = (i + 1) * MIN_LOOKASIDE_ALLOCATION;
        g_LookasideEntries[i]->Count = 0;
        g_LookasideEntries[i]->LastFlushTime = KeQueryInterruptTime();

        status = ExInitializeLookasideListEx(&(g_LookasideEntries[i]->Lookaside),
                                             NULL,
                                             NULL,
                                             NonPagedPoolNx,
                                             0,
                                             (i + 1) * MIN_LOOKASIDE_ALLOCATION,
                                             USB_LOOKASIDE_TAG,
                                             0);
    }

    if (!NT_SUCCESS(status)) {
        USBPORT_FreeLookasideLists();
    }

#if 0
    //
    // Unit test
    //
    UT_LookasideAllocateAndFreeTest();
#endif

    return status;
}

VOID
USBPORT_FreeLookasideLists()
/*++
Routine Description:

    Frees the lookaside

Arguments:

    None

Return Value:

    None

--*/
{
    ULONG i;

    for (i = 0; i < NUM_LOOKASIDE_ENTRIES; i++) {

        if (NULL == g_LookasideEntries[i]) {
            continue;
        }

        NT_ASSERT(g_LookasideEntries[i]->Count == 0);

        ExDeleteLookasideListEx(&(g_LookasideEntries[i]->Lookaside));

        ExFreePoolWithTag(g_LookasideEntries[i], USB_LOOKASIDE_TAG);

        g_LookasideEntries[i] = NULL;
    }
}


FORCEINLINE
PUSB_LOOKASIDE_ENTRY
GetLookasideAllocationEntry(
    __in ULONG NumberOfBytes
    )
/*++

Routine Description:

    Obtains the lookaside allocation entry for the given size.

Arguments:

    Size of block required

Return Value:

    Lookaside allocation entry

--*/
{

    ULONG index;

    index = NumberOfBytes / MIN_LOOKASIDE_ALLOCATION;

    return (g_LookasideEntries[index]);
}

__drv_allocatesMem(Mem)
_Post_writable_byte_size_(NumberOfBytes)
PVOID
UsbTryAllocPoolFromNonPagedLookaside_Z(
    __in ULONG NumberOfBytes
    )
/*++

Routine Description:

    Tries to allocate from the NonPaged lookaside. If lookaside was not initialized or
    the size cannot be allocated from lookaside, we fallback to direct.
    allocation from the NonPaged pool

Arguments:

    NumberOfBytes - size of allocation

Return Value:

    NTSTATUS

--*/

{
    PVOID                 p;
    PUSB_ALLOCATION_HEADER entry;
    PUSB_LOOKASIDE_ENTRY  lookasideEntry;
    ULONG                 allocationSize;

    p = NULL;
    entry = NULL;
    allocationSize = NumberOfBytes;

    USBPORT_ASSERT(allocationSize);

    if (allocationSize == 0) {
        return NULL;
    }

    //
    // Include bytes for allocation header
    //
    allocationSize += sizeof(USB_ALLOCATION_HEADER);

    //
    // Try lookaside allocation first
    //
    if (NULL != g_LookasideEntries &&
        allocationSize < MAX_LOOKASIDE_ALLOCATION) {

        // Allocate from lookaside
        lookasideEntry = GetLookasideAllocationEntry(allocationSize);
        if (NULL != lookasideEntry) {

            p = ExAllocateFromLookasideListEx(&lookasideEntry->Lookaside);

            if (NULL != p) {

                InterlockedIncrement(&lookasideEntry->Count);

                entry = p;
                entry->LookasideEntry = lookasideEntry;
                entry->Size = allocationSize;
#if DBG
                UsbLookasideFillSignature(p, allocationSize, lookasideEntry->Size);
#endif
            }
        }
    }

    if (NULL == p) {
        //
        // Allocation from lookaside failed or is not supported
        // allocate from the pool and save in our allocation table
        //
        p = ExAllocatePoolWithTag(NonPagedPoolNx, allocationSize, USBPORT_TAG);
        if (NULL != p) {
            entry = p;
            entry->LookasideEntry = NULL;
            entry->Size = allocationSize;
        }
    }

    if (NULL != p) {
        p = entry->Buffer;
        RtlZeroMemory(p, allocationSize - sizeof(USB_ALLOCATION_HEADER));

    }

    return p;
}

#pragma prefast(push)
#pragma prefast(disable:__WARNING_ALIASED_MEMORY_LEAK, "Function will free the allocation just not the pointer passed to it")
#pragma prefast(disable:__WARNING_BUFFER_UNDERFLOW,    "Function will read backward from passed pointer")
VOID
UsbFreeToLookaside(
    __in __drv_freesMem(Mem) PVOID Ptr
    )
/*++
Routine Description:

    This routines frees an allocation

Returns:

    NTSTATUS
--*/
{
    PUSB_ALLOCATION_HEADER entry;
    PUSB_LOOKASIDE_ENTRY   lookasideEntry;

    if (NULL == Ptr) {
        return;
    }

    entry = CONTAINING_RECORD(Ptr, USB_ALLOCATION_HEADER, Buffer);
    lookasideEntry = entry->LookasideEntry;

#if DBG
    UsbLookasideCheckSignature(Ptr);
#endif

    if (NULL == entry->LookasideEntry) {
        ExFreePoolWithTag(entry, USBPORT_TAG);
        return;
    }

    if (InterlockedDecrement(&(lookasideEntry->Count)) < 0) {
        NT_ASSERTMSG("LookAside allocation tracking count is less than zero", FALSE);
    }

    ExFreeToLookasideListEx(&(lookasideEntry->Lookaside), entry);

    //
    // Flush lookaside if sufficient time has elapsed since
    //
    if ((KeQueryInterruptTime() - lookasideEntry->LastFlushTime) > (LOOKASIDE_FLUSH_PERIOD_IN_MS * 1000 * 10LL)) {
        ExFlushLookasideListEx(&(lookasideEntry->Lookaside));
        lookasideEntry->LastFlushTime = KeQueryInterruptTime();
    }

    return;
}
#pragma prefast(pop)

#if DBG

VOID UsbLookasideFillSignature(
    __in PVOID Ptr,
    __in ULONG AllocationSize,
    __in ULONG LookasideSize
    )
/*++

    Routine Description:

    This routines verifies the signature in the padded area of the lookaside
    allocation.

    Arguments:
        Buffer - The buffer to pad
        Allocation Size - The allocation size of Buffer
        LookasideSize - The actual size of buffer

    Returns: None

--*/
{
    PDWORD fillStart;
    PDWORD fillEnd;
    ULONG  startOffset;

    // Align start offset to DWORD boundary
    startOffset = ALIGN_UP(AllocationSize, DWORD);

    // Fill extra allocation with a signature
    fillStart = (PDWORD) ((PBYTE) Ptr + startOffset);
    fillEnd   = (PDWORD) ((PBYTE) Ptr + LookasideSize);

    while (fillStart < fillEnd) {
        *fillStart = LOOKASIDE_PADDING_SIGN;
        fillStart++;
    }
}

VOID UsbLookasideCheckSignature(
    __in PVOID Ptr
    )
/*++

    Routine Description:

    This routines verifies the signature in the padded area of the lookaside
    allocation.

    Arguments: USB Lookaside entry

    Returns: None

--*/
{
    PDWORD                  fillStart;
    PDWORD                  fillEnd;
    ULONG                   startOffset;
    PUSB_ALLOCATION_HEADER  entry = NULL;

    entry = CONTAINING_RECORD(Ptr, USB_ALLOCATION_HEADER, Buffer);

#pragma prefast(suppress:__WARNING_BUFFER_UNDERFLOW,    "Function will read backward from passed pointer")
    if (NULL == entry->LookasideEntry) {
        return;
    }

    // Align start offset to DWORD boundary
#pragma prefast(suppress:__WARNING_BUFFER_UNDERFLOW,    "Function will read backward from passed pointer")
    startOffset = ALIGN_UP(entry->Size, DWORD);
    fillStart = (PDWORD) ((PBYTE) Ptr + startOffset);
    fillEnd   = (PDWORD) ((PBYTE) entry + entry->LookasideEntry->Size);

    // Check padding for signature
    while (fillStart < fillEnd) {
        NT_ASSERT(LOOKASIDE_PADDING_SIGN == *fillStart);
        fillStart++;
    }
}

#endif

#if 0

VOID UT_LookasideAllocateAndFreeTest()
/*++

    Routine Description:

    Runs allocation and free testing on the lookaside allocator

    Arguments: None

    Returns: None

--*/
{

    PVOID *allocationArray;
    PVOID *iter;
    ULONG i;

    iter = allocationArray = UsbTryAllocPoolFromNonPagedLookaside_Z(2*PAGE_SIZE*sizeof(PVOID));

    //
    // Allocate and fill
    //
    for (i = 0; i < 2*PAGE_SIZE; i++)    {
        ULONG j = 0;
        *iter = UsbTryAllocPoolFromNonPagedLookaside_Z(i+1);
        NT_ASSERT(*iter);

        for (j = 0; j < i + 1; j++) {
            *(((PBYTE) *iter) + j) = 0xFF;
        }
        iter++;
    }

    iter = allocationArray;

    //
    // Check pattern and free
    //
    for (i = 0; i< 2*PAGE_SIZE; i++)    {
        ULONG j = 0;

        for (j = 0; j < i + 1; j++) {
            NT_ASSERT(*(((PBYTE) *iter) + j) == 0xFF);
        }

        UsbFreeToLookaside(*iter++);
    }

    UsbFreeToLookaside(allocationArray);
}

#endif


