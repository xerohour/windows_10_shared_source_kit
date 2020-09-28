/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    memdesc.c

Abstract:

    This module contains routines that operate on a memory descriptor or
    a memory descriptor list.  These functions serve as helper functions to
    other modules in the MM library, in particular the Page Allocator.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "mm.h"

// -------------------------------------------------------------------- Defines

//
// Define the total number of descriptors available to the boot application.
//

#define MAX_GLOBAL_DESCRIPTOR_COUNT (MmGlobalMemoryDescriptorCount)

// -------------------------------------------------------------------- Globals

//
// Maintain an integer to keep track of the number of nested MM API calls
// that may allocate or use global memory descriptors.  This is used to
// ensure the proper maintenance of the global descriptor array.
//

ULONG MmDescriptorCallTreeCount = 0;

//
// Define the dynamic descriptor buffer. The memory for dynamic descriptors
// is allocated during initialization if a boot application requests it.
//

PMEMORY_DESCRIPTOR MmDynamicMemoryDescriptors;

//
// Define the descriptor buffer to be used for memory operations. This points
// to either the static or dynamic memory descriptor buffer.
//

PMEMORY_DESCRIPTOR MmGlobalMemoryDescriptors;

//
// A global scratch buffer is used for memory descriptors generated when
// manipulating any memory descriptor list.  The global descriptors are
// required to ensure an operation will succeed.  Otherwise, the request may
// cause a nested request by the Heap Allocator.
//

MEMORY_DESCRIPTOR MmStaticMemoryDescriptors[MAX_STATIC_DESCRIPTOR_COUNT];

//
// Define the total number of dynamic descriptors available to the boot
// application.
//

ULONG MmDynamicMemoryDescriptorCount;

//
// Define the total number of descriptors available to the boot application.
//

ULONG MmGlobalMemoryDescriptorCount;

//
// Define the total number of descriptors in use by the application.
//

ULONG MmGlobalMemoryDescriptorsUsed;

//
// Platform memory type precedence ordering.
//
// When the firmware places regions carrying different memory types within the
// same page of memory, precedence rules are needed in order to select the
// memory type that will be applied to the whole page in the boot
// environment's internal memory descriptor lists.
//
// Precedence rules must be selected such that the most restrictive memory
// types override the least restrictive.  For example, if half of a page is
// firmware reserved while the other half is reclaimable after OS handoff,
// the reserved memory type must override the reclaimable one to avoid
// marking the entire page as usable memory in the future.
//
// Types carrying higher precedence are listed earlier in the array.  Types
// not listed in this array are assumed to have precedence higher than that of
// the first entry.
//

MEMORY_TYPE MmPlatformMemoryTypePrecedence[] = {
    MEMORY_TYPE_PLATFORM_RESERVED,
    MEMORY_TYPE_BAD,
    MEMORY_TYPE_MMIO_SPACE,
    MEMORY_TYPE_MMIO_PORT_SPACE,
    MEMORY_TYPE_PAL_CODE,
    MEMORY_TYPE_RUNTIME_SERVICE_CODE,
    MEMORY_TYPE_RUNTIME_SERVICE_DATA,
    MEMORY_TYPE_ACPI,
    MEMORY_TYPE_ACPI_TABLES,
    MEMORY_TYPE_BOOT_SERVICE_CODE,
    MEMORY_TYPE_FREE,
    MEMORY_TYPE_FREE_ZEROED,
};

// ---------------------------------------------------------------- Definitions

#define DO_COALESCE_ATTRIBUTES_MATCH(_Attributes1, _Attributes2) \
    (((_Attributes1) & MEMORY_ATTRIBUTE_COALESCE_MATCH_MASK) == \
     ((_Attributes2) & MEMORY_ATTRIBUTE_COALESCE_MATCH_MASK))

// ----------------------------------------------------------------- Prototypes

VOID
MmMdpSaveCurrentListPointer (
    __inout PMEMORY_DESCRIPTOR_LIST Mdl,
    __in PLIST_ENTRY ListEntry
    );


VOID
MmMdpSwitchToDynamicDescriptors (
    __in ULONG NumberOfDescriptors
    );

NTSTATUS
MmMdpSwitchToStaticDescriptors (
    VOID
    );

BOOLEAN
MmMdpTruncateDescriptor (
    __in PMEMORY_DESCRIPTOR_LIST Mdl,
    __in PMEMORY_DESCRIPTOR MemoryDescriptor,
    __in ULONG InsertionFlags
    );

BOOLEAN
MmMdpCoalesceDescriptor (
    __in PMEMORY_DESCRIPTOR_LIST Mdl,
    __in PMEMORY_DESCRIPTOR MemoryDescriptor,
    __in ULONG InsertionFlags
    );

BOOLEAN
MmMdpHasPrecedence (
    __in MEMORY_TYPE Type1,
    __in MEMORY_TYPE Type2
    );

ULONG
MmMdpLookupTypePrecedenceIndex (
    __in MEMORY_TYPE Type
    );

// ------------------------------------------------------------------ Functions

NTSTATUS
MmMdDestroy (
    VOID
    )

/*++

Routine Description:

    This routine performs destruction of dynamic memory descriptor buffer and
    switches the application to use static descriptors.

Arguments:

    None.

Return Value:

    NTSTATUS code.

--*/

{

    NTSTATUS Status;

    Status = MmMdpSwitchToStaticDescriptors();
    return Status;
}

VOID
MmMdFreeList (
    __in PMEMORY_DESCRIPTOR_LIST Mdl
    )

/*++

Routine Description:

    This routine frees a MDL.

Arguments:

    Mdl - Supplies a pointer to the MDL to free.

Return Value:

    None.

--*/

{

    PMEMORY_DESCRIPTOR Descriptor;
    PLIST_ENTRY NextEntry;

    NextEntry = Mdl->Head->Flink;
    while (NextEntry != Mdl->Head) {
        Descriptor = CONTAINING_RECORD(NextEntry,
                                       MEMORY_DESCRIPTOR,
                                       ListEntry);

        NextEntry = NextEntry->Flink;

        //
        // Remove the descriptor from the list and free it.
        //

        MmMdRemoveDescriptorFromList(Mdl, Descriptor);
        MmMdFreeDescriptor(Descriptor);
    }

    return;
}

VOID
MmMdInitialize (
    __in ULONG Phase,
    __in PLIBRARY_PARAMETERS Parameters
    )

/*++

Routine Description:

    This routine performs initialization of memory descriptors.

    Phase 0: This routine initializes the static memory descriptor buffer and
        initializes global descriptors to use static descriptors. Only static
        memory descriptors are available for an application by default.

    Phase 1: This routine allocates dynamic descriptor buffer and switches
        the boot application to use it if requested by the boot application.

Arguments:

    Phase - Supplies the value for memory intialization phase.

    Parameters - The application-supplied parameters for configuring the
        boot library. This value contains the number of dynamic descriptors
        required by the application (if any).

Return Value:

    None.

--*/

{

    ULONG NumberOfDynamicDescriptors;

    if (Phase == 0) {
        MmGlobalMemoryDescriptors = &MmStaticMemoryDescriptors[0];
        MmGlobalMemoryDescriptorCount = MAX_STATIC_DESCRIPTOR_COUNT;
        RtlZeroMemory(MmGlobalMemoryDescriptors,
                      sizeof(MEMORY_DESCRIPTOR) * MAX_GLOBAL_DESCRIPTOR_COUNT);

        MmGlobalMemoryDescriptorsUsed = 0;

    } else {

        ASSERT(Phase == 1);

        //
        // Convert global descriptors to heap allocations, now that heap is
        // available.
        //

        if (MmGlobalMemoryDescriptorsUsed != 0) {

            ASSERT(MmDescriptorCallTreeCount == 1);

            MmMdFreeGlobalDescriptors();

            ASSERT((MmDescriptorCallTreeCount == 1) &&
                   (MmGlobalMemoryDescriptorsUsed == 0));

        }

        //
        // Create a dynamic descriptor buffer and switch the application to
        // use dynamic descriptors by default. Dynamic descriptors are only
        // required if the static descriptors may not be sufficient.
        //

        NumberOfDynamicDescriptors = Parameters->NumberOfDynamicDescriptors;
        if (NumberOfDynamicDescriptors > MAX_STATIC_DESCRIPTOR_COUNT) {
            MmMdpSwitchToDynamicDescriptors(NumberOfDynamicDescriptors);
        }
    }

    return;
}

VOID
MmMdInitializeList (
    __out PMEMORY_DESCRIPTOR_LIST Mdl,
    __in MEMORY_DESCRIPTOR_LIST_TYPE Type,
    __in_opt PLIST_ENTRY ListHead
    )

/*++

Routine Description:

    This routine initializes an MDL either with an empty list of
    descriptors or the provided list.

Arguments:

    Mdl - Supplies a pointer to the MDL to initialize.

    Type - Supplies the type of MDL being initialized.

    ListHead - Optionally supplies a pointer to the list of descriptors
        to initialize the MDL with.  If this is not provided the MDL
        will be initialized with an empty list using storage inside
        the MDL for the list head.

Return Value:

    None.

--*/

{

    if (ARGUMENT_PRESENT(ListHead)) {
        Mdl->Head = ListHead;

    } else {
        InitializeListHead(&Mdl->ListEntry);
        Mdl->Head = &Mdl->ListEntry;
    }

    Mdl->Type = Type;
    Mdl->Current = NULL;
    return;
}

PMEMORY_DESCRIPTOR
MmMdInitDescriptor (
    __in ULONGLONG BasePage,
    __in ULONGLONG MappedBasePage,
    __in ULONGLONG PageCount,
    __in ULONG Attributes,
    __in MEMORY_TYPE MemoryType
    )

/*++

Routine Description:

    Initializes the next available global descriptor with the prescribed
    values.  The global memory descriptor array is used when creating a
    memory descriptor list and when modifying the lists to ensure a
    operation can succeeds before attempting to allocate heap for a
    descriptor.

Arguments:

    BasePage - Supplies the base page for the memory region.

    MappedBasePage - Supplies an optional mapped base page for the region, if
        the memory region describes physical memory that is mapped.

    PageCount - Supplies the size in pages of the memory region.

    Attributes - Supplies the memory attributes of the memory region.

    MemoryType - Supplies type of memory of the region.

Return Value:

    Pointer to the static descriptor that was initialized with the above
    values.

--*/

{

    ULONG Id;

    //
    // Make sure there are still static descriptors available.  If not, then
    // it may be indicative of a recursive loop.  There are definitely enough
    // static descriptors to get through initialization.  So make this
    // failure case an assert as there is something internally wrong when
    // this occurs.
    //

    if (MmGlobalMemoryDescriptorsUsed >= MAX_GLOBAL_DESCRIPTOR_COUNT) {
        return NULL;
    }

    Id = MmGlobalMemoryDescriptorsUsed;

    //
    // Set the next global descriptor with the parameters to this routine.
    //

    MmGlobalMemoryDescriptors[Id].BasePage = BasePage;
    MmGlobalMemoryDescriptors[Id].MappedBasePage = MappedBasePage;
    MmGlobalMemoryDescriptors[Id].PageCount = PageCount;
    MmGlobalMemoryDescriptors[Id].Attributes = Attributes;
    MmGlobalMemoryDescriptors[Id].MemoryType = MemoryType;
    InitializeListHead(&MmGlobalMemoryDescriptors[Id].ListEntry);
    MmGlobalMemoryDescriptorsUsed += 1;
    return &MmGlobalMemoryDescriptors[Id];
}

PMEMORY_DESCRIPTOR
MmMdInitByteGranularDescriptor (
    __in ULONGLONG Base,
    __in ULONGLONG MappedBase,
    __in ULONGLONG Size,
    __in ULONG Attributes,
    __in MEMORY_TYPE MemoryType
    )

/*++

Routine Description:

    Allocates the next available global descriptor and initializes it as a byte
    granular descriptor carrying the prescribed values.

Arguments:

    Base - Supplies the physical base address for the memory region.

    MappedBase - Supplies an optional virtual address reflecting the virtual
        mapping of the physical base address.

    Size - Supplies the size in bytes of the memory region.

    Attributes - Supplies the memory attributes used during allocation or
        mapping of the memory region.

    MemoryType - Supplies type of memory of the region.

Return Value:

    Pointer to the static descriptor that was initialized with the above
    values.

--*/

{

    ULONG Id;

    //
    // Make sure there are still static descriptors available.  If not, then
    // it may be indicative of a recursive loop.  There are definitely enough
    // static descriptors to get through initialization.  So make this
    // failure case an assert as there is something internally wrong when
    // this occurs.
    //

    if (MmGlobalMemoryDescriptorsUsed >= MAX_GLOBAL_DESCRIPTOR_COUNT) {
        return NULL;
    }

    Id = MmGlobalMemoryDescriptorsUsed;

    //
    // Set the next global descriptor with the parameters to this routine.
    //

    MmGlobalMemoryDescriptors[Id].ByteGranular.Base = Base;
    MmGlobalMemoryDescriptors[Id].ByteGranular.MappedBase = MappedBase;
    MmGlobalMemoryDescriptors[Id].ByteGranular.Size = Size;
    MmGlobalMemoryDescriptors[Id].ByteGranular.Attributes = Attributes;
    MmGlobalMemoryDescriptors[Id].ByteGranular.MemoryType = MemoryType;
    InitializeListHead(&MmGlobalMemoryDescriptors[Id].ListEntry);
    MmGlobalMemoryDescriptorsUsed += 1;
    return &MmGlobalMemoryDescriptors[Id];
}

NTSTATUS
MmMdFreeDescriptor (
    __in PMEMORY_DESCRIPTOR Descriptor
    )

/*++

Routine Description:

    This routine will check if the descriptor to be freed is a static, dynamic
    or a heap-based descriptor. The descriptor is freed only if it is
    determined to be heap-based descriptor.

Arguments:

    Descriptor - Supplies a pointer to a memory descriptor to be freed.

Return Value:

    None.

--*/

{

    PMEMORY_DESCRIPTOR EndRange;
    BOOLEAN InRange;
    PMEMORY_DESCRIPTOR StartRange;
    NTSTATUS Status;

    //
    // Check whether the descriptor falls within the static or dynamic buffer
    // range.
    //

    InRange = FALSE;
    if (MmDynamicMemoryDescriptors != NULL) {
        StartRange = MmDynamicMemoryDescriptors;
        EndRange = StartRange + MmDynamicMemoryDescriptorCount;
        if ((Descriptor >= StartRange) && (Descriptor < EndRange)) {
            InRange = TRUE;
        }
    }

    if (InRange == FALSE) {
        StartRange = MmStaticMemoryDescriptors;
        EndRange = StartRange + MAX_STATIC_DESCRIPTOR_COUNT;
        if ((Descriptor >= StartRange) && (Descriptor < EndRange)) {
            InRange = TRUE;
        }
    }

    //
    // If the descriptor falls outside the range of static and dynamic
    // descriptor buffers, then it must be a heap-descriptor and can be
    // freed. For static or dynamic descriptors, the descriptor contents
    // must be zero-ed out to ensure that the descriptor is not converted to
    // a heap descriptor when walked inside of MmMdFreeGlobalMemoryDescriptors.
    //

    if (InRange == FALSE) {
        Status = BlMmFreeHeap(Descriptor);

    } else {
        RtlZeroMemory(Descriptor, sizeof(MEMORY_DESCRIPTOR));
        Status = STATUS_SUCCESS;
    }

    return Status;
}

VOID
MmMdFreeGlobalDescriptors (
    VOID
    )

/*++

Routine Description:

    Allocates heap for each memory descriptor that is currently being used
    in the global scratch memory descriptor list.  Additionally, each
    descriptor is zeroed and MmGlobalMemoryDescriptorsUsed is reset to zero.

Arguments:

    None.

Return Value:

    None.

    MmGlobalMemoryDescriptors will be zeroed and
    MmGlboalMemoryDescriptorsUsed will be zero.

--*/

{


    PMEMORY_DESCRIPTOR AllocatedMemoryDescriptor;
    ULONG Id;
    PLIST_ENTRY ListBlink;
    PLIST_ENTRY ListFlink;

    ASSERT(MmDescriptorCallTreeCount != 0);

    if (MmDescriptorCallTreeCount == 1) {

        //
        // Allocate a buffer from the heap for each descriptor in the global
        // array.
        //
        // N.B. The global memory descriptor array must be processed in order
        //      since any memory allocation could result in a heap allocator
        //      extend which in turn could generate a page allocator extend.
        //      A page allocator extend would result in a change to the memory
        //      map and therefore require changes to the memory descriptor
        //      lists, resulting in the use of new global memory descriptors.
        //      Servicing the array in order is required to ensure the freeing
        //      of all global descriptors allocated as the result of a nested
        //      page allocator extend.
        //

        for (Id = 0; Id < MmGlobalMemoryDescriptorsUsed; Id += 1) {

            //
            // Only allocate memory for a valid memory descriptor.  There is
            // no point to allocate memory, just to orphan it.
            //

            if (MmGlobalMemoryDescriptors[Id].PageCount != 0) {
                AllocatedMemoryDescriptor =
                               BlMmAllocateHeap(sizeof(MEMORY_DESCRIPTOR));

                //
                // BlMmAllocateHeap failed.  We are unable to copy the global
                // descriptors to the heap.  Return now, and do not reset
                // MmGlobalMemoryDescriptorsUsed.
                //

                if (AllocatedMemoryDescriptor == NULL) {
                    return;
                }

                ListBlink = MmGlobalMemoryDescriptors[Id].ListEntry.Blink;
                ListFlink = MmGlobalMemoryDescriptors[Id].ListEntry.Flink;

                //
                // Copy memory descriptor to the allocated buffer.
                //

                RtlCopyMemory(AllocatedMemoryDescriptor,
                              &MmGlobalMemoryDescriptors[Id],
                              sizeof(MEMORY_DESCRIPTOR));

                //
                // Update flink/blink;
                //

                ListBlink->Flink = &AllocatedMemoryDescriptor->ListEntry;
                ListFlink->Blink = &AllocatedMemoryDescriptor->ListEntry;

                //
                // Clear the global descriptor.
                //

                RtlZeroMemory(&MmGlobalMemoryDescriptors[Id],
                              sizeof(MEMORY_DESCRIPTOR));
            }
        }

        //
        // N.B. MmGlobalMemoryDescriptorsUsed needs to be updated at the end.
        //      Otherwise, it could not be used to reference the next entry,
        //      and BlMmAllocateHeap could cause a nested call into the Page
        //      Allocator.
        //

        MmGlobalMemoryDescriptorsUsed = 0;
    }
}

NTSTATUS
MmMdAddDescriptorToList (
    __in PMEMORY_DESCRIPTOR_LIST MemoryDescriptorList,
    __in PMEMORY_DESCRIPTOR MemoryDescriptor,
    __in ULONG Flags
    )

/*++

Routine Description:

    Inserts the specified into the prescribed memory descriptor list.

Arguments:

    MemoryDescriptorList - The memory descriptor list in which the specified
        memory descriptor should be inserted.

    MemoryDescriptor - The memory descriptor to be inserted.

    Flags - Insertion flags, including:

        MDL_FLAGS_COALESCE - Coalesce address range with adjacent descriptors.
            (In order to coalesce, the adjacent descriptor must be the same
            for both virtual and physical range).

        MDL_FLAGS_TRUNCATE - Truncate with adjacent descriptor (according to
            memory type precedence) if descriptors overlap in address space.

        MDL_FLAGS_OVERRIDE_COALESCE - Ensures that adjacent memory descriptors
            are not coalesced under any condition.

        MDL_FLAGS_OVERRIDE_TRUNCATE - Ensures that memory descriptors are not
            truncated under any condition.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if MemoryDescriptorList or MemoryDescriptor are
        NULL.

--*/

{

    PMEMORY_DESCRIPTOR CurrentDescriptor;
    PMEMORY_DESCRIPTOR Descriptor;
    PLIST_ENTRY ListBlink;
    PLIST_ENTRY NextEntry;

    //
    // Parameter checking.
    //

    if ((MemoryDescriptorList == NULL) || (MemoryDescriptor == NULL)) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // If the descriptor explicitly contains the coalesce attribute, respect
    // the request unless the caller specified an override.
    //

    if (CHECK_FLAG(Flags, MDL_FLAGS_OVERRIDE_COALESCE)) {
        CLEAR_FLAGS(Flags, MDL_FLAGS_COALESCE);

    } else if (CHECK_FLAG(MemoryDescriptor->Attributes,
                          MEMORY_ATTRIBUTE_DESCRIPTION_COALESCE)) {

        SET_FLAGS(Flags, MDL_FLAGS_COALESCE);
    }

    if (CHECK_FLAG(Flags, MDL_FLAGS_OVERRIDE_TRUNCATE)) {
        CLEAR_FLAGS(Flags, MDL_FLAGS_TRUNCATE);
    }

    if (CHECK_FLAG(MemoryDescriptor->Attributes,
                   MEMORY_ATTRIBUTE_DESCRIPTION_CACHE)) {

        SET_FLAGS(Flags, MDL_FLAGS_CACHE_CURRENT_DESCRIPTOR);
    }

    //
    // Insert descriptor based on the base address.
    //

    if (MemoryDescriptorList->Current != NULL) {

        ASSERT(IsListEmpty(MemoryDescriptorList->Head) == FALSE);

        CurrentDescriptor = CONTAINING_RECORD(MemoryDescriptorList->Current,
                                              MEMORY_DESCRIPTOR,
                                              ListEntry);

        if (MemoryDescriptor->BasePage < CurrentDescriptor->BasePage) {
            NextEntry = MemoryDescriptorList->Head->Flink;

        } else {
            NextEntry = MemoryDescriptorList->Current;
        }

    } else {
        NextEntry = MemoryDescriptorList->Head->Flink;
    }


    while (NextEntry != MemoryDescriptorList->Head) {
        Descriptor = CONTAINING_RECORD(NextEntry,
                                       MEMORY_DESCRIPTOR,
                                       ListEntry);

        //
        // Insert the descriptor based on the physical address, with
        // the lowest addresses at the front of the list.
        //
        // If the descriptor to be inserted has the same base page as one
        // already in the MDL, insert based on memory type precedence
        // with higher priority before those with lower priority.
        //

        if ((MemoryDescriptor->BasePage < Descriptor->BasePage) ||
            ((MemoryDescriptor->BasePage == Descriptor->BasePage) &&
             (MmMdpHasPrecedence(MemoryDescriptor->MemoryType,
                                 Descriptor->MemoryType)))) {

            ListBlink = NextEntry->Blink;
            ListBlink->Flink = &MemoryDescriptor->ListEntry;
            NextEntry->Blink = &MemoryDescriptor->ListEntry;
            MemoryDescriptor->ListEntry.Flink = NextEntry;
            MemoryDescriptor->ListEntry.Blink = ListBlink;
            if (CHECK_FLAG(Flags, MDL_FLAGS_CACHE_CURRENT_DESCRIPTOR)) {
                MmMdpSaveCurrentListPointer(MemoryDescriptorList,
                                            ListBlink);
            }

            //
            // If the newly inserted descriptor overlaps with either adjacent
            // descriptor, truncate based on precedence.  If this routine
            // returns true, the memory descriptor has been deleted during
            // truncation.
            //

            if ((CHECK_FLAG(Flags, MDL_FLAGS_TRUNCATE)) &&
                (MmMdpTruncateDescriptor(MemoryDescriptorList,
                                         MemoryDescriptor,
                                         Flags) != FALSE)) {

                return STATUS_SUCCESS;
            }

            //
            // If requested by the caller, coalesce descriptors based on memory
            // type.  If this routine returns true, the memory descriptor has
            // been deleted and there is no need to continue.
            //

            if (CHECK_FLAG(Flags, MDL_FLAGS_COALESCE) &&
                (MmMdpCoalesceDescriptor(MemoryDescriptorList,
                                         MemoryDescriptor,
                                         Flags) != FALSE)) {

                return STATUS_SUCCESS;
            }


            //
            // Track the fact that this descriptor was the last one accessed.
            //

            if (CHECK_FLAG(Flags, MDL_FLAGS_CACHE_CURRENT_DESCRIPTOR)) {
                MmMdpSaveCurrentListPointer(MemoryDescriptorList,
                                            &MemoryDescriptor->ListEntry);
            }

            //
            // Successful in inserting descriptor.  Return.
            //

            return STATUS_SUCCESS;
        }

        NextEntry = NextEntry->Flink;  // Advance pointer.
    }

    //
    // Insert the descriptor at the end of the list.  This also covers the
    // case when the list is empty.
    //

    InsertTailList(MemoryDescriptorList->Head, &MemoryDescriptor->ListEntry);

    //
    // Attempt to truncate and coalesce memory range with adjacent memory
    // regions.  If either of these operations return true, the memory
    // descriptor was deleted during the operation.
    //

    if ((CHECK_FLAG(Flags, MDL_FLAGS_TRUNCATE)) &&
        (MmMdpTruncateDescriptor(MemoryDescriptorList,
                                 MemoryDescriptor,
                                 Flags) != FALSE)) {

        return STATUS_SUCCESS;
    }

    if (CHECK_FLAG(Flags, MDL_FLAGS_COALESCE) &&
        (MmMdpCoalesceDescriptor(MemoryDescriptorList,
                                 MemoryDescriptor,
                                 Flags) != FALSE)) {

        return STATUS_SUCCESS;
    }

    if (CHECK_FLAG(Flags, MDL_FLAGS_CACHE_CURRENT_DESCRIPTOR)) {
        MmMdpSaveCurrentListPointer(MemoryDescriptorList,
                                    &MemoryDescriptor->ListEntry);
    }

    return STATUS_SUCCESS;
}

VOID
MmMdRemoveDescriptorFromList (
    __in PMEMORY_DESCRIPTOR_LIST Mdl,
    __in PMEMORY_DESCRIPTOR MemoryDescriptor
    )

/*++

Routine Description:

    This routine removes a memory descriptor from an MDL.

Arguments:

    Mdl - Supplies the MDL to remove the descriptor from.

    MemoryDescriptor - Supplies the memory descriptor to remove from the MDL.

Return Value:

    None.

--*/

{

    PLIST_ENTRY PreviousEntry;

    PreviousEntry = MemoryDescriptor->ListEntry.Blink;
    RemoveEntryList(&MemoryDescriptor->ListEntry);
    if (Mdl->Current == &MemoryDescriptor->ListEntry) {
        Mdl->Current = NULL;
        MmMdpSaveCurrentListPointer(Mdl, PreviousEntry);
    }
}

PMEMORY_DESCRIPTOR
MmMdFindDescriptor (
    __in ULONGLONG Page,
    __in ULONG ListTypes,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine will attempt to find a memory descriptor containing the
    specified page in the global memory descriptor lists which are described
    by the provided ListTypes bitmask.   This routine only finds physical pages
    in physical address MDLs.  The virtual address MDL kept internally by the
    address translation engine is not searched by this routine.

Arguments:

    Page - Supplies the page number to search for.

    ListTypes - Supplies a bitmask which describes the memory descriptor lists
        to search.  Values include:

        MEMORYMAP_TYPE_MAPPED_ALLOCATED
        MEMORYMAP_TYPE_MAPPED_UNALLOCATED
        MEMORYMAP_TYPE_UNMAPPED_ALLOCATED
        MEMORYMAP_TYPE_UNMAPPED_UNALLOCATED
        MEMORYMAP_TYPE_RESERVED_ALLOCATED
        MEMORYMAP_TYPE_BAD
        MEMORYMAP_TYPE_TRUNCATED
        MEMORYMAP_TYPE_PERSISTENT

        Pre-defined definitions for multiple descriptor lists:

        MEMORYMAP_TYPE_ALLOCATED
        MEMORYMAP_TYPE_UNALLOCATED
        MEMORYMAP_TYPE_ALL

    Flags - A memory descriptor flag describing whether the specified page
        is physical or virtual.  Possible values include:

        MDL_FLAGS_VIRTUAL_ADDRESS
        MDL_FLAGS_PHYSICAL_ADDRESS (Default).

Return Value:

    When successful, this routine will return a pointer to the memory
    descriptor containing the specified page.  Otherwise, the pointer returned
    will be NULL.

--*/

{

    PMEMORY_DESCRIPTOR Descriptor;

#if !defined(MM_MIN)

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_MAPPED_ALLOCATED)) {
        Descriptor = MmMdFindDescriptorFromMdl(&MmMdlMappedAllocated,
                                               Page,
                                               Flags);

        if (Descriptor != NULL) {
            return Descriptor;
        }
    }

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_MAPPED_UNALLOCATED)) {
        Descriptor = MmMdFindDescriptorFromMdl(&MmMdlMappedUnallocated,
                                               Page,
                                               Flags);

        if (Descriptor != NULL) {
            return Descriptor;
        }
    }

#endif

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_UNMAPPED_ALLOCATED)) {
        Descriptor = MmMdFindDescriptorFromMdl(&MmMdlUnmappedAllocated,
                                               Page,
                                               Flags);

        if (Descriptor != NULL) {
            return Descriptor;
        }
    }

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_UNMAPPED_UNALLOCATED)) {
        Descriptor = MmMdFindDescriptorFromMdl(&MmMdlUnmappedUnallocated,
                                               Page,
                                               Flags);

        if (Descriptor != NULL) {
            return Descriptor;
        }
    }

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_RESERVED_ALLOCATED)) {
        Descriptor = MmMdFindDescriptorFromMdl(&MmMdlReservedAllocated,
                                               Page,
                                               Flags);

        if (Descriptor != NULL) {
            return Descriptor;
        }
    }

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_BAD)) {
        Descriptor = MmMdFindDescriptorFromMdl(&MmMdlBadMemory, Page, Flags);
        if (Descriptor != NULL) {
            return Descriptor;
        }
    }

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_TRUNCATED)) {
        Descriptor = MmMdFindDescriptorFromMdl(&MmMdlTruncatedMemory,
                                               Page,
                                               Flags);

        if (Descriptor != NULL) {
            return Descriptor;
        }
    }

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_PERSISTENT)) {
        Descriptor = MmMdFindDescriptorFromMdl(&MmMdlPersistentMemory,
                                               Page,
                                               Flags);

        if (Descriptor != NULL) {
            return Descriptor;
        }
    }

    return NULL;
}

PMEMORY_DESCRIPTOR
MmMdFindDescriptorFromMdl (
    __in PMEMORY_DESCRIPTOR_LIST Mdl,
    __in ULONGLONG Page,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routines attempts to find a memory descriptor containing the specified
    page in the specified memory descriptor list.  If MDL_FLAGS_VIRTUAL_ADDRESS
    is set in Flags, then the page number is interpreted as a virtual page
    number.

Arguments:

    Mdl - Supplies the memory descriptor list to search.

    Page - Supplies the page number to search for.

    Flags - Supplies a memory descriptor flag describing whether the specified
        page is physical or virtual.  Possible values include:

        MDL_FLAGS_VIRTUAL_ADDRESS
        MDL_FLAGS_PHYSICAL_ADDRESS (Default).

Return Value:

    When successful, this routine will return a pointer to the memory
    descriptor containing the specified page.  Otherwise, the pointer returned
    will be NULL.

--*/

{

    PMEMORY_DESCRIPTOR CurrentDescriptor;
    ULONGLONG ListBasePage;
    PMEMORY_DESCRIPTOR ListDescriptor;
    BOOLEAN MappedPage;
    PLIST_ENTRY NextEntry;

    //
    // Physical MDLs describe virtual addresses in the mapped page of memory
    // descriptors.  Virtual MDLs describe virtual addresses in the base page,
    // and do not describe physical addresses at all.
    //

    MappedPage = FALSE;
    if (CHECK_FLAG(Flags, MDL_FLAGS_VIRTUAL_ADDRESS)) {
        if (Mdl->Type == MdlTypePhysical) {
            MappedPage = TRUE;
        }

    } else {

        //
        // A request for physical memory in a virtual MDL cannot be satisfied.
        //

        if (Mdl->Type == MdlTypeVirtual) {
            return NULL;
        }
    }

    //
    // Walk the memory descriptor list in search of a memory region containing
    // the specified page.   Start from the currently cached descriptor if
    // the cache is populated.  Since the descriptors are ordered in the list
    // by base page, only do this for base page searches.
    //

    if ((MappedPage == FALSE) && (Mdl->Current != NULL)) {

        ASSERT(IsListEmpty(Mdl->Head) == FALSE);

        CurrentDescriptor = CONTAINING_RECORD(Mdl->Current,
                                              MEMORY_DESCRIPTOR,
                                              ListEntry);

        if (Page < CurrentDescriptor->BasePage) {
            NextEntry = Mdl->Head->Flink;

        } else {
            NextEntry = Mdl->Current;
        }

    } else {
        NextEntry = Mdl->Head->Flink;
    }

    while (NextEntry != Mdl->Head) {
        ListDescriptor = CONTAINING_RECORD(NextEntry,
                                           MEMORY_DESCRIPTOR,
                                           ListEntry);

        //
        // If the current descriptor contains the specified page, return
        // with a pointer to the memory descriptor.
        //

        if (MappedPage != FALSE) {
            ListBasePage = ListDescriptor->MappedBasePage;

        } else {
            ListBasePage = ListDescriptor->BasePage;
        }

        //
        // Be careful when looking for a mapped region.  Skip over any region
        // that is not currently mapped.
        //

        if (((MappedPage == FALSE) || (ListBasePage != 0)) &&
            (ListBasePage <= Page) &&
            (Page < (ListBasePage + ListDescriptor->PageCount))) {

            return ListDescriptor;
        }

        NextEntry = NextEntry->Flink;  // Advance the pointer.
    }

    return NULL;
}

PMEMORY_DESCRIPTOR
MmMdFindOverlappingDescriptor (
    _In_ ULONGLONG BasePage,
    _In_ ULONGLONG EndPage,
    _In_ ULONG ListTypes,
    _Out_ PMEMORY_DESCRIPTOR_LIST *Mdl
    )

/*++

Routine Description:

    This routine will attempt to find the first memory descriptor overlapping
    the specified physical page range in the global memory descriptor lists
    which are described by the provided ListTypes bitmask.  If more than one
    descriptor overlaps, this routine finds the descriptor containing the
    lowest physical page number.  This routine only finds physical pages in
    physical address MDLs.  The virtual address MDL kept internally by the
    address translation engine is not searched by this routine.

Arguments:

    BasePage - Supplies the first page number of the range to search for.

    EndPage - Supplies the last page number (exclusive) of the range to search
        for.

    ListTypes - Supplies a bitmask which describes the memory descriptor lists
        to search.  Values include:

        MEMORYMAP_TYPE_MAPPED_ALLOCATED
        MEMORYMAP_TYPE_MAPPED_UNALLOCATED
        MEMORYMAP_TYPE_UNMAPPED_ALLOCATED
        MEMORYMAP_TYPE_UNMAPPED_UNALLOCATED
        MEMORYMAP_TYPE_RESERVED_ALLOCATED
        MEMORYMAP_TYPE_BAD
        MEMORYMAP_TYPE_TRUNCATED
        MEMORYMAP_TYPE_PERSISTENT

        Pre-defined definitions for multiple descriptor lists:

        MEMORYMAP_TYPE_ALLOCATED
        MEMORYMAP_TYPE_UNALLOCATED
        MEMORYMAP_TYPE_ALL

    Mdl - When successful, returns a pointer to the memory descriptor list that
        contains the found memory descriptor.

Return Value:

    When successful, this routine will return a pointer to the first memory
    descriptor containing the specified page range.  Otherwise, the pointer
    returned will be NULL.

--*/

{

    PMEMORY_DESCRIPTOR Descriptor;
    PMEMORY_DESCRIPTOR FirstDescriptor;

    *Mdl = NULL;
    FirstDescriptor = NULL;

#if !defined(MM_MIN)

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_MAPPED_ALLOCATED)) {
        Descriptor = MmMdFindOverlappingDescriptorFromMdl(
                         &MmMdlMappedAllocated,
                         BasePage,
                         EndPage);

        if ((Descriptor != NULL) &&
            ((FirstDescriptor == NULL) ||
             (Descriptor->BasePage < FirstDescriptor->BasePage))) {

            FirstDescriptor = Descriptor;
            *Mdl = &MmMdlMappedAllocated;
        }
    }

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_MAPPED_UNALLOCATED)) {
        Descriptor = MmMdFindOverlappingDescriptorFromMdl(
                         &MmMdlMappedUnallocated,
                         BasePage,
                         EndPage);

        if ((Descriptor != NULL) &&
            ((FirstDescriptor == NULL) ||
             (Descriptor->BasePage < FirstDescriptor->BasePage))) {

            FirstDescriptor = Descriptor;
            *Mdl = &MmMdlMappedUnallocated;
        }
    }

#endif

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_UNMAPPED_ALLOCATED)) {
        Descriptor = MmMdFindOverlappingDescriptorFromMdl(
                         &MmMdlUnmappedAllocated,
                         BasePage,
                         EndPage);

        if ((Descriptor != NULL) &&
            ((FirstDescriptor == NULL) ||
             (Descriptor->BasePage < FirstDescriptor->BasePage))) {

            FirstDescriptor = Descriptor;
            *Mdl = &MmMdlUnmappedAllocated;
        }
    }

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_UNMAPPED_UNALLOCATED)) {
        Descriptor = MmMdFindOverlappingDescriptorFromMdl(
                         &MmMdlUnmappedUnallocated,
                         BasePage,
                         EndPage);

        if ((Descriptor != NULL) &&
            ((FirstDescriptor == NULL) ||
             (Descriptor->BasePage < FirstDescriptor->BasePage))) {

            FirstDescriptor = Descriptor;
            *Mdl = &MmMdlUnmappedUnallocated;
        }
    }

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_RESERVED_ALLOCATED)) {
        Descriptor = MmMdFindOverlappingDescriptorFromMdl(
                         &MmMdlReservedAllocated,
                         BasePage,
                         EndPage);

        if ((Descriptor != NULL) &&
            ((FirstDescriptor == NULL) ||
             (Descriptor->BasePage < FirstDescriptor->BasePage))) {

            FirstDescriptor = Descriptor;
            *Mdl = &MmMdlReservedAllocated;
        }
    }

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_BAD)) {
        Descriptor = MmMdFindOverlappingDescriptorFromMdl(
                         &MmMdlBadMemory,
                         BasePage,
                         EndPage);

        if ((Descriptor != NULL) &&
            ((FirstDescriptor == NULL) ||
             (Descriptor->BasePage < FirstDescriptor->BasePage))) {

            FirstDescriptor = Descriptor;
            *Mdl = &MmMdlBadMemory;
        }
    }

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_TRUNCATED)) {
        Descriptor = MmMdFindOverlappingDescriptorFromMdl(
                         &MmMdlTruncatedMemory,
                         BasePage,
                         EndPage);

        if ((Descriptor != NULL) &&
            ((FirstDescriptor == NULL) ||
             (Descriptor->BasePage < FirstDescriptor->BasePage))) {

            FirstDescriptor = Descriptor;
            *Mdl = &MmMdlTruncatedMemory;
        }
    }

    if (CHECK_FLAG(ListTypes, MEMORYMAP_TYPE_PERSISTENT)) {
        Descriptor = MmMdFindOverlappingDescriptorFromMdl(
                         &MmMdlPersistentMemory,
                         BasePage,
                         EndPage);

        if ((Descriptor != NULL) &&
            ((FirstDescriptor == NULL) ||
             (Descriptor->BasePage < FirstDescriptor->BasePage))) {

            FirstDescriptor = Descriptor;
            *Mdl = &MmMdlPersistentMemory;
        }
    }

    return FirstDescriptor;
}

PMEMORY_DESCRIPTOR
MmMdFindOverlappingDescriptorFromMdl (
    _In_ PMEMORY_DESCRIPTOR_LIST Mdl,
    _In_ ULONGLONG BasePage,
    _In_ ULONGLONG EndPage
    )

/*++

Routine Description:

    This routine attempts to find the first memory descriptor overlapping the
    specified physical page range in the specified memory descriptor list.  If
    more than descriptor overlaps, this routine finds the descriptor containing
    the lowest physical page number.

Arguments:

    Mdl - Supplies the memory descriptor list to search.

    BasePage - Supplies the first page number of the range to search for.

    EndPage - Supplies the last page number (exclusive) of the range to search
        for.

Return Value:

    When successful, this routine will return a pointer to the first memory
    descriptor containing the specified page range.  Otherwise, the pointer
    returned will be NULL.

--*/

{

    PMEMORY_DESCRIPTOR CurrentDescriptor;
    PMEMORY_DESCRIPTOR ListDescriptor;
    ULONGLONG ListEndPage;
    PLIST_ENTRY NextEntry;

    //
    // Walk the memory descriptor list in search of a memory region overlapping
    // the specified page range.  Start from the current cached descriptor if
    // the cache is populated and it lies before the range.
    //

    if (Mdl->Current != NULL) {

        ASSERT(IsListEmpty(Mdl->Head) == FALSE);

        CurrentDescriptor = CONTAINING_RECORD(Mdl->Current,
                                              MEMORY_DESCRIPTOR,
                                              ListEntry);

        if (CurrentDescriptor->BasePage <= BasePage) {
            NextEntry = Mdl->Current;

        } else {
            NextEntry = Mdl->Head->Flink;
        }

    } else {
        NextEntry = Mdl->Head->Flink;
    }

    while (NextEntry != Mdl->Head) {
        ListDescriptor = CONTAINING_RECORD(NextEntry,
                                           MEMORY_DESCRIPTOR,
                                           ListEntry);

        //
        // If the current descriptor overlaps the specified page range, return
        // with a pointer to the memory descriptor.
        //

        ListEndPage = ListDescriptor->BasePage + ListDescriptor->PageCount;
        if ((ListDescriptor->BasePage < EndPage) && (BasePage < ListEndPage)) {
            return ListDescriptor;
        }

        //
        // If the current descriptor lies after the range, the search is
        // unsuccessful, since the descriptors are ordered in the list by base
        // page.
        //

        if (EndPage <= ListDescriptor->BasePage) {
            break;
        }

        NextEntry = NextEntry->Flink;
    }

    return NULL;
}

NTSTATUS
MmMdTruncateDescriptors (
    __inout PMEMORY_DESCRIPTOR_LIST Mdl,
    __inout PMEMORY_DESCRIPTOR_LIST MdlDestination,
    __in ULONGLONG Page
    )

/*++

Routine Description:

    Traverses the specified memory descriptor list, removing all references
    to memory above the specified page number.  Descriptors may be
    removed entirely from the MDL or their page count can be modified
    to account for the removed memory pages.

Arguments:

    Mdl - Supplies the memory descriptor list to truncate.

    MdlDestination - Specifies the list that keeps track of truncated regions.

    Page - Supplies the maximum page number allowed in the MDL.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if an allocation fails.

--*/

{

    PMEMORY_DESCRIPTOR MemoryDescriptor;
    UNPACKED_MEMORY_TYPE MemoryType;
    PLIST_ENTRY NextEntry;
    NTSTATUS ReturnStatus;
    PMEMORY_DESCRIPTOR TruncatedDescriptor;
    ULONGLONG TruncatedPageCount;

    //
    // Walk the memory descriptor list, removing all descriptors whose
    // base page is higher than PhysicalPage.
    //

    ReturnStatus = STATUS_SUCCESS;
    NextEntry = Mdl->Head->Flink;
    while (NextEntry != Mdl->Head) {
        MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                             MEMORY_DESCRIPTOR,
                                             ListEntry);

        //
        // Advance pointer before operating on the memory descriptor.
        // (This may result in the descriptor being removed from the MDL).
        //

        NextEntry = NextEntry->Flink;

        //
        // Remove the descriptor from the MDL if the physical base page
        // is greater than the specified value and the type may be removed.
        // Certain platform type memory ranges may not be removed as they are
        // critical to the operation of the system.
        //

        MemoryType.MemoryType = MemoryDescriptor->MemoryType;
        if ((MemoryType.Class == MEMORY_CLASS_PLATFORM) &&
            (MemoryType.SubType != MEMORY_PLATFORM_TYPE_AVAILABLE)) {

            continue;
        }

        //
        // Crop all descriptors which straddle the specified physical page
        // line.
        //

        if (Page <= MemoryDescriptor->BasePage) {
            MmMdRemoveDescriptorFromList(Mdl, MemoryDescriptor);
            MmMdAddDescriptorToList(MdlDestination,
                                    MemoryDescriptor,
                                    MDL_FLAGS_COALESCE);

        } else if (Page < (MemoryDescriptor->BasePage +
                           MemoryDescriptor->PageCount)) {

            TruncatedPageCount = ((MemoryDescriptor->BasePage +
                                   MemoryDescriptor->PageCount) - Page);

            TruncatedDescriptor =
                MmMdInitDescriptor(Page,
                                   0,
                                   TruncatedPageCount,
                                   MemoryDescriptor->Attributes,
                                   MemoryDescriptor->MemoryType);

            if (TruncatedDescriptor == NULL) {
                ReturnStatus = STATUS_NO_MEMORY;
                break;
            }

            MemoryDescriptor->PageCount = (Page - MemoryDescriptor->BasePage);
            MmMdAddDescriptorToList(MdlDestination,
                                    TruncatedDescriptor,
                                    MDL_FLAGS_COALESCE);
        }
    }

    return ReturnStatus;
}

NTSTATUS
MmMdCopyList (
    __inout PMEMORY_DESCRIPTOR_LIST Destination,
    __in PMEMORY_DESCRIPTOR_LIST Source,
    __in_opt PMEMORY_DESCRIPTOR DescriptorBuffer,
    __inout PULONG DescriptorsUsed,
    __in_opt ULONG DescriptorCount,
    __in ULONG Flags
    )

/*++

Routine Description:

    Copies the source descriptor list to the destination descriptor list.
    For each descriptor in the source list, a memory descriptor is
    allocated for the destination list.

    In order to ensure that the source memory descriptor list does not change
    while copying, it is imperative that the descriptors for the destination
    MDL come from a pre-allocated array of descriptors.  The caller can
    specify the list (DescriptorBuffer) or this routine will use the global
    memory descriptor array.

Arguments:

    Destination - Supplies a pointer to the destination memory descriptor list.

    Source - Supplies a pointer to the source memory descriptor list.

    DescriptorBuffer - Supplies a pre-allocated buffer of memory descriptors to
        use when copying the MDL.

    DescriptorsUsed - Supplies the number of descriptors currently allocated
        from the descriptor buffer.

    DescriptorCount - Supplies the total number of descriptors that can fit into
        the descriptor buffer.

    Flags - Supplies insertion flags, including:

        MDL_FLAGS_COALESCE - Coalesce address range with adjacent descriptors.
            (In order to coalesce, the adjacent descriptor must be the same
            for both virtual and physical range).

            N.B. MDL_FLAGS_COALESCE is ignored when a descriptor buffer is
                 specified.  This operation could result in a memory descriptor
                 being freed.  Since a descriptor buffer is allocated as a
                 single allocation, it is illegal to free an internal pointer
                 to this array.

        N.B. MDL_FLAGS_TRUNCATE is not supported in any condition.  In the case
             where a descriptor buffer is provided, the flag is not supported
             for the same reason as MDL_FLAGS_COALESCE; the operation could
             result in a memory descriptor being freed.  It is not supported
             in the scenario where the descriptor buffer is not specified since
             a truncation operation could result in the allocation of a global
             memory descriptor, but the global memory descriptors are used as
             the descriptor buffer in this scenario.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if DstMdl or SrcMdl are
        NULL.

    STATUS_NO_MEMORY if there is not enough room in the descriptor buffer.

--*/

{

    PMEMORY_DESCRIPTOR MemoryDescriptor;
    PLIST_ENTRY NextEntry;
    NTSTATUS Status;

    //
    // Simple parameter checking.  Ensure that both a source and destination
    // list are specified.
    //

    if ((ARGUMENT_PRESENT(Destination) == FALSE) ||
        (ARGUMENT_PRESENT(Source) == FALSE)) {

        return STATUS_INVALID_PARAMETER;
    }

    Status = STATUS_SUCCESS;

    //
    // A memory descriptor list can not be modified during the copy operation.
    // As a result, the caller can specify an allocated buffer to use for the
    // memory descriptors or the global descriptor list can be used.  Since
    // the descriptor buffer is a single allocation, freeing an entry in the
    // array is illegal.  Override coalescing and trucation operations in this
    // scenario.  Additionally, truncation is not allowed in the case where the
    // global descriptor list is utilized, since this operation could result
    // in the allocation of a descriptor from this list.
    //

    if (DescriptorBuffer == NULL) {
        MmEnterDescriptorLibraryApi();
        DescriptorBuffer = MmGlobalMemoryDescriptors;
        DescriptorCount = MAX_GLOBAL_DESCRIPTOR_COUNT;
        DescriptorsUsed = &MmGlobalMemoryDescriptorsUsed;

    } else {
        SET_FLAGS(Flags, MDL_FLAGS_OVERRIDE_COALESCE);
    }

    SET_FLAGS(Flags, MDL_FLAGS_OVERRIDE_TRUNCATE);

    //
    // Walk the source descriptor list, creating a memory descriptor for
    // each entry and then inserting it into the destination list.
    //

    NextEntry = Source->Head->Flink;
    while ((NextEntry != Source->Head) && NT_SUCCESS(Status)) {
        MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                             MEMORY_DESCRIPTOR,
                                             ListEntry);

        //
        // Create a memory descriptor based on the contents of the current
        // entry.
        //

        if (DescriptorCount <= *DescriptorsUsed) {
            Status = STATUS_NO_MEMORY;
            break;
        }

        RtlCopyMemory(&DescriptorBuffer[*DescriptorsUsed],
                      MemoryDescriptor,
                      sizeof(MEMORY_DESCRIPTOR));

        //
        // Insert the new descriptor into the destination list.
        //

        Status = MmMdAddDescriptorToList(Destination,
                                         &DescriptorBuffer[*DescriptorsUsed],
                                         Flags);

        *DescriptorsUsed += 1;
        NextEntry = NextEntry->Flink;
    }

    //
    // Allocate a memory descriptor for each temporary descriptor.
    //

    if (DescriptorBuffer == MmGlobalMemoryDescriptors) {
        MmLeaveDescriptorLibraryApi();
    }

    return Status;
}

NTSTATUS
MmMdMigrateMdlToGlobalDescriptors (
    __in PMEMORY_DESCRIPTOR_LIST DescriptorList
    )

/*++

Routine Description:

    This routine will copy a memory descriptor list to the global descriptor
    array.  Additionally, the original memory descriptor list will be deleted
    and all memory will be freed.

    When this routine returns, either successfully or unsuccessfully, there
    are no heap-allocated memory descriptors in the memory descriptor list.

Arguments:

    DescriptorList - Supplies the list of memory descriptors to migrate to the
        global descriptor array.

Return Value:

    STATUS_SUCCESS if successful.

    STATUS_NO_MEMORY if there are no more entries in the global descriptor
        array.

--*/

{

    PMEMORY_DESCRIPTOR Descriptor;
    PMEMORY_DESCRIPTOR GlobalDescriptor;
    MEMORY_DESCRIPTOR_LIST_TYPE MdlType;
    PLIST_ENTRY NextEntry;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;
    MdlType = DescriptorList->Type;
    NextEntry = DescriptorList->Head->Flink;
    while (NextEntry != DescriptorList->Head) {
        Descriptor = CONTAINING_RECORD(NextEntry,
                                       MEMORY_DESCRIPTOR,
                                       ListEntry);

        //
        // Copy the memory descriptor to the global array, using the copy
        // routine appropriate for the granularity disposition of this MDL.
        //

        if (MdlType == MdlTypeByteGranular) {
            GlobalDescriptor = MmMdInitByteGranularDescriptor(
                                   Descriptor->ByteGranular.Base,
                                   Descriptor->ByteGranular.MappedBase,
                                   Descriptor->ByteGranular.Size,
                                   Descriptor->ByteGranular.Attributes,
                                   Descriptor->ByteGranular.MemoryType);

        } else {
            GlobalDescriptor = MmMdInitDescriptor(Descriptor->BasePage,
                                                  Descriptor->MappedBasePage,
                                                  Descriptor->PageCount,
                                                  Descriptor->Attributes,
                                                  Descriptor->MemoryType);
        }

        //
        // Insert the memory descriptor into the list by usurping the flink
        // and blink pointers from the source descriptor.  If a global
        // descriptor could not be allocated, remove the descriptor from the
        // memory descriptor list.
        //

        NextEntry = Descriptor->ListEntry.Flink;   // Advance pointer.
        if (GlobalDescriptor != NULL) {
            GlobalDescriptor->ListEntry.Flink = Descriptor->ListEntry.Flink;
            GlobalDescriptor->ListEntry.Blink = Descriptor->ListEntry.Blink;
            (Descriptor->ListEntry.Flink)->Blink = &GlobalDescriptor->ListEntry;
            (Descriptor->ListEntry.Blink)->Flink = &GlobalDescriptor->ListEntry;

        } else {
            Status = STATUS_NO_MEMORY;
            RemoveEntryList(&Descriptor->ListEntry);
        }

        MmMdFreeDescriptor(Descriptor);
    }

    //
    // Invalidate the current pointer.  When valid, it would have contained
    // a pointer to one of the descriptors just freed.
    //

    DescriptorList->Current = NULL;
    return Status;
}

NTSTATUS
MmMdRemoveRegionFromMdl (
    __in PMEMORY_DESCRIPTOR_LIST Mdl,
    __in ULONGLONG BasePage,
    __in ULONGLONG PageCount,
    __in ULONG Flags
    )

/*++

Routine Description:

    Removes all descriptors or portions of a descriptor which reference the
    specified memory region.

    N.B. This routine is different that MmPaAllocateRegionFromList, where
         the destination MDL is NULL.  The later routine will fail if the
         specified region is not completely contained within one memory
         descriptor.  This routine is the preferred and generic method for
         removing a region from a list.

Arguments:

    Mdl - Memory descriptor list from which all references to the specified
        memory range should be removed.

    BasePage - The starting page of the memory range to remove.  Whether this
        is a virtual page or a physical page is specified by Flags.

    PageCount - Size of the region to remove.

    Flags - Indicates if the specified memory region is a virtual range or a
        physical range.

        MDL_FLAGS_VIRTUAL_ADDRESS - Memory range is virtual.

        MDL_FLAGS_PHYSICAL_ADDRESS - Memory range is physical.  (default).


Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the flags indicate a search for a physical
        address and a virtual address MDL is specified.

    STATUS_NO_MEMORY if an allocation fails.

--*/

{

    NTSTATUS Status;

    Status = MmMdRemoveRegionFromMdlEx(Mdl,
                                       BasePage,
                                       PageCount,
                                       Flags,
                                       NULL);

    return Status;
}

NTSTATUS
MmMdRemoveRegionFromMdlEx (
    __in PMEMORY_DESCRIPTOR_LIST Mdl,
    __in ULONGLONG BasePage,
    __in ULONGLONG PageCount,
    __in ULONG Flags,
    __out_opt PMEMORY_DESCRIPTOR_LIST RemovalList
    )

/*++

Routine Description:

    Removes all descriptors or portions of a descriptor which reference the
    specified memory region.

    This routine may allocate descriptors from the global descriptor list.
    This routine will not allocate any descriptors from the heap.

    If the RemovalList argument is not present, this routine may free
    descriptors.

    If the RemovalList argument is present, this routine will not free any
    descriptors except for those that it has allocated from the global list,
    and it will do so only upon failure.

Arguments:

    Mdl - Memory descriptor list from which all references to the specified
        memory range should be removed.

    BasePage - The starting page of the memory range to remove.  Whether this
        is a virtual page or a physical page is specified by Flags.

    PageCount - Size of the region to remove.

    Flags - Indicates if the specified memory region is a virtual range or a
        physical range.

        MDL_FLAGS_VIRTUAL_ADDRESS - Memory range is virtual.

        MDL_FLAGS_PHYSICAL_ADDRESS - Memory range is physical.  (default).

    RemovalList - Supplies a pointer a memory descriptor list which receives
        a list of the regions that were actually removed by this routine. On
        success, the caller is responsible for freeing the descriptors in the
        list.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if the flags indicate a search for a physical
        address and a virtual address MDL is specified.

    STATUS_NO_MEMORY if an allocation fails.

--*/

{

    ULONGLONG DescriptorBasePage;
    ULONGLONG DescriptorEndPage;
    BOOLEAN MappedPage;
    PMEMORY_DESCRIPTOR MemoryDescriptor;
    PLIST_ENTRY NextEntry;
    ULONGLONG Offset;
    PMEMORY_DESCRIPTOR RemovalDescriptor;
    MEMORY_DESCRIPTOR RemovedRegion;
    BOOLEAN RemovalTracking;
    PMEMORY_DESCRIPTOR SplitDescriptor;
    MEMORY_DESCRIPTOR TempDescriptor;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;
    RemovalDescriptor = NULL;
    SplitDescriptor = NULL;

    //
    // Determine whether regions removed should be tracked or not.
    //

    RemovalTracking = FALSE;
    if (ARGUMENT_PRESENT(RemovalList) != FALSE) {
        MmMdInitializeList(RemovalList, Mdl->Type, NULL);
        RemovalTracking = TRUE;
    }

    //
    // Determine the field in the memory descriptor to examine.  Virtual
    // addresses are described in the mapped page field of descriptors in
    // physical MDLs, and in the base page field of virtual MDLs.  Physical
    // addresses are not tracked at all in virtual MDLs.
    //

    MappedPage = FALSE;
    if (CHECK_FLAG(Flags, MDL_FLAGS_VIRTUAL_ADDRESS)) {
        if (Mdl->Type == MdlTypePhysical) {
            MappedPage = TRUE;
        }

    } else {
        if (Mdl->Type == MdlTypeVirtual) {
            Status = STATUS_INVALID_PARAMETER;
            goto RemoveRegionFromMdl;
        }
    }

    //
    //
    // Walk the descriptor list, removing any region of a descriptor that
    // is described by the specified region.
    //
    // N.B. The MDL is walked until completion to assure that the specified
    //      region is removed from all memory descriptors.
    //

    NextEntry = Mdl->Head->Flink;
    while (NextEntry != Mdl->Head) {
        MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                             MEMORY_DESCRIPTOR,
                                             ListEntry);

        //
        // Search on the base page or the mapped page based on the caller's
        // request for memory type and the MDL type.
        //

        if (MappedPage != FALSE) {
            DescriptorBasePage = MemoryDescriptor->MappedBasePage;

        } else {
            DescriptorBasePage = MemoryDescriptor->BasePage;
        }

        DescriptorEndPage = DescriptorBasePage + MemoryDescriptor->PageCount;
        Offset = 0;

        //
        // Assume the entire region described by the descriptor will be
        // removed.
        //

        RemovedRegion = *MemoryDescriptor;

        //
        // Remove any part of the descriptor that falls in the specified
        // region.
        //
        // The three option include the region straddling the start of the
        // descriptor, the end of the descriptor or falling completely within
        // the descriptor.
        //

        if ((BasePage <= DescriptorBasePage) &&
            (DescriptorBasePage < (BasePage + PageCount))) {

            //
            // The region straddles the start of the descriptor.
            //

            Offset = min(BasePage + PageCount, DescriptorEndPage) -
                     DescriptorBasePage;

            //
            // Adjust the memory descriptor in the list to reflect the
            // change.
            //

            MemoryDescriptor->BasePage += Offset;
            MemoryDescriptor->PageCount -= Offset;
            if (MemoryDescriptor->MappedBasePage != 0) {
                MemoryDescriptor->MappedBasePage += Offset;
            }

            //
            // The region removed starts at the base described by the memory
            // descriptor and spans Offset pages.
            //

            RemovedRegion.PageCount = Offset;

            //
            // If the specified region completely overlaps with the current
            // descriptor, remove the descriptor from the list.
            //

            if (MemoryDescriptor->PageCount == 0) {
                NextEntry = NextEntry->Flink;     // Advance pointer first.
                MmMdRemoveDescriptorFromList(Mdl, MemoryDescriptor);

                //
                // If removals are being tracked, recycle the removed
                // memory's descriptor for use in the removal list.
                //
                // Else, free the removed memory's descriptor.
                //

                if (RemovalTracking != FALSE) {
                    RemovalDescriptor = MemoryDescriptor;

                } else {
                    MmMdFreeDescriptor(MemoryDescriptor);
                }

                goto AddRemovedRegion;
            }

        } else if ((BasePage < DescriptorEndPage) &&
                   (DescriptorEndPage <= (BasePage + PageCount))) {

            //
            // The region straddles the end of the descriptor.
            //
            // N.B. We do not need to check to see if the specified region
            //      completely overlaps the memory descriptor.  In that case,
            //      the previous check would have removed the entire
            //      descriptor.
            //

            Offset = DescriptorEndPage - BasePage;

            //
            // Adjust the memory descriptor in the list to reflect the
            // change.
            //

            MemoryDescriptor->PageCount -= Offset;

            ASSERT(MemoryDescriptor->PageCount != 0);

            //
            // The region removed starts at the input base page and spans
            // Offset pages.
            //

            RemovedRegion.BasePage += MemoryDescriptor->PageCount;
            RemovedRegion.PageCount = Offset;
            if (RemovedRegion.MappedBasePage != 0) {
                RemovedRegion.MappedBasePage += MemoryDescriptor->PageCount;
            }

        } else if ((DescriptorBasePage < BasePage) &&
                   ((BasePage + PageCount) < DescriptorEndPage)) {

            //
            // If the specified region is completely contained within the
            // memory descriptor, we need to split the descriptor to describe
            // the entire region minus the caller specified region.
            //
            // Use the existing descriptor to describe the second region.
            // This ensures we will not duplicate work when we perform checks
            // on the next memory descriptor in the list.
            //

            TempDescriptor.BasePage = MemoryDescriptor->BasePage;
            TempDescriptor.MappedBasePage = MemoryDescriptor->MappedBasePage;
            TempDescriptor.PageCount = BasePage - DescriptorBasePage;
            Offset = TempDescriptor.PageCount + PageCount;
            MemoryDescriptor->BasePage += Offset;
            MemoryDescriptor->PageCount -= Offset;
            if (MemoryDescriptor->MappedBasePage != 0) {
                MemoryDescriptor->MappedBasePage += Offset;
            }

            //
            // Create the descriptor for the region before the caller's
            // specified region.
            //

            SplitDescriptor = MmMdInitDescriptor(TempDescriptor.BasePage,
                                                 TempDescriptor.MappedBasePage,
                                                 TempDescriptor.PageCount,
                                                 MemoryDescriptor->Attributes,
                                                 MemoryDescriptor->MemoryType);

            if (SplitDescriptor == NULL) {
                Status = STATUS_NO_MEMORY;
                goto RemoveRegionFromMdl;
            }

            //
            // Add the new descriptor back to the MDL.
            //

            Status = MmMdAddDescriptorToList(Mdl, SplitDescriptor, Flags);
            if (!NT_SUCCESS(Status)) {
                goto RemoveRegionFromMdl;
            }

            SplitDescriptor = NULL;

            //
            // The region removed is the specified input region.
            //

            RemovedRegion.BasePage += TempDescriptor.PageCount;
            RemovedRegion.PageCount = PageCount;
            if (RemovedRegion.MappedBasePage != 0) {
                RemovedRegion.MappedBasePage += TempDescriptor.PageCount;
            }

        } else {

            //
            // The given region does not overlap with the current descriptor.
            //

            ASSERT(((BasePage + PageCount) <= DescriptorBasePage) ||
                   (DescriptorEndPage <= BasePage));

            RemovedRegion.PageCount = 0;
        }

        NextEntry = NextEntry->Flink;  // Advance pointer.

        //
        // If any part of the descriptor was removed and removals need to be
        // tracked, create a descriptor for the removed region and add it
        // to the list.
        //

AddRemovedRegion:
        if ((RemovedRegion.PageCount > 0) && (RemovalTracking != FALSE)) {

            //
            // If a removal descriptor has already been allocated
            // (recycled from a removed region's descriptor), initialize
            // that descriptor.
            //
            // Else, allocate a descriptor from the global list and
            // initialize it.
            //

            if (RemovalDescriptor != NULL) {
                *RemovalDescriptor = RemovedRegion;

            } else {
                RemovalDescriptor = MmMdInitDescriptor(
                                        RemovedRegion.BasePage,
                                        RemovedRegion.MappedBasePage,
                                        RemovedRegion.PageCount,
                                        RemovedRegion.Attributes,
                                        RemovedRegion.MemoryType);

                if (RemovalDescriptor == NULL) {
                    Status = STATUS_NO_MEMORY;
                    goto RemoveRegionFromMdl;
                }
            }

            //
            // Add the descriptor described the region removed to the removal
            // list.
            //

            Status = MmMdAddDescriptorToList(RemovalList,
                                             RemovalDescriptor,
                                             MDL_FLAGS_NONE);

            if (!NT_SUCCESS(Status)) {
                goto RemoveRegionFromMdl;
            }

            RemovalDescriptor = NULL;
        }
    }

RemoveRegionFromMdl:
    if (!NT_SUCCESS(Status)) {
       if (RemovalList != NULL) {
           MmMdFreeList(RemovalList);
           MmMdInitializeList(RemovalList, Mdl->Type, NULL);
       }

       if (RemovalDescriptor != NULL) {
           MmMdFreeDescriptor(RemovalDescriptor);
       }

       if (SplitDescriptor != NULL) {
           MmMdFreeDescriptor(SplitDescriptor);
       }
    }

    return Status;
}

BOOLEAN
MmMdFindSatisfyingRegion (
    __in PMEMORY_DESCRIPTOR MemoryRegion,
    __out PMEMORY_DESCRIPTOR AllocateRegion,
    __in ULONGLONG Pages,
    __in PADDRESS_RANGE BaseRange,
    __in PADDRESS_RANGE MappedRange,
    __in BOOLEAN PreferHighAddresses,
    __in MEMORY_TYPE RequiredMemoryType,
    __in ULONG Attributes,
    __in ULONG Alignment
    )

/*++

Routine Description:

    Determines if there is a region in MemoryRegion that satisfies the size,
    attribute and alignment requirements requested by caller.  If true, then
    AllocateRegion is filled in with the region that satisfies the requirements.

Arguments:

    MemoryRegion - Supplies a memory descriptor to check for a region containing
        the desired requirements.

    AllocateRegion - Supplies a pointer to a variable that receives the region
        within the supplied memory descriptor that satisfies the caller's
        request.

    Pages - Supplies the number of pages required.

    BaseRange - Supplies the range of acceptable physical addresses.

    MappedRange - Supplies the range of acceptable virtual addresses.

    PreferHighAddresses - Supplies a boolean indicating whether the highest
        matching address range is to be chosen.  By default the lowest matching
        range is chosen.

    RequiredMemoryType - Supplies the memory type that is required of the
        input memory region.

    Attributes - Supplies required attributes.

    Alignment - Supplies required alignment, in pages.

Return Value:

    TRUE if MemoryRegion has a region of the requested number of pages that
    is aligned to the caller's desire and has the requested attributes.
    AllocateRegion will be filled out with a region in MemoryRegion
    that satisfies these requirements.

    FALSE otherwise.

--*/

{

    ULONGLONG AlignDownOffset;
    ULONGLONG AlignUpOffset;
    ULONG AttrSub1;
    ULONG AttrSub2;
    ULONGLONG BasePage;
    ADDRESS_RANGE Intersection;
    ULONGLONG MappedBasePage;

    ASSERT(Alignment != 0);
    ASSERT(Pages > 0);

    //
    // Get the intersection between the memory region and the acceptable base
    // address range.  If this intersection does not contain space for an
    // aligned allocation, fail.
    //

    Intersection.Minimum = max(BaseRange->Minimum,
                               MemoryRegion->BasePage);

    Intersection.Maximum = min(BaseRange->Maximum,
                               MemoryRegion->BasePage
                               + MemoryRegion->PageCount
                               - 1);

    if (Intersection.Minimum > Intersection.Maximum) {
        return FALSE;
    }

    AlignUpOffset = ROUND_UP(Intersection.Minimum, Alignment) - Intersection.Minimum;
    Intersection.Minimum += AlignUpOffset;
    if (((Intersection.Minimum + Pages - 1) < Intersection.Minimum) ||
        ((Intersection.Minimum + Pages - 1) > Intersection.Maximum)) {

        return FALSE;
    }

    //
    // Use the aligned bottom or top of the intersection as the base of the
    // allocation, based on caller preference.
    //

    if (PreferHighAddresses != FALSE) {
        BasePage = Intersection.Maximum - (Pages - 1);
        AlignDownOffset = BasePage - ROUND_DOWN(BasePage, Alignment);
        BasePage -= AlignDownOffset;

        ASSERT(BasePage >= Intersection.Minimum);

    } else {
        BasePage = Intersection.Minimum;
        AlignDownOffset = 0;
    }

    MappedBasePage = 0;

    //
    // If the caller specified a virtual base page, then make sure the
    // region described by MemoryRegion is virtually mapped.
    //

    if ((UNUSED_ADDRESS_RANGE(MappedRange) == FALSE) &&
        (MemoryRegion->MappedBasePage == 0)) {

        return FALSE;
    }

    //
    // Make sure that the mapped address of the descriptor meets the caller's
    // requirements.
    //

    if (MemoryRegion->MappedBasePage != 0) {

        //
        // Get the intersection between the memory region's virtual address and
        // the acceptable base address range.  If this intersection does not
        // contain space for an aligned allocation, fail.
        //

        Intersection.Minimum = max(MappedRange->Minimum,
                                   MemoryRegion->MappedBasePage);

        Intersection.Maximum = min(MappedRange->Maximum,
                                   MemoryRegion->MappedBasePage
                                   + MemoryRegion->PageCount
                                   - 1);

        if (Intersection.Minimum > Intersection.Maximum) {
            return FALSE;
        }

        Intersection.Minimum += AlignUpOffset;
        if (((Intersection.Minimum + Pages - 1) < Intersection.Minimum) ||
            ((Intersection.Minimum + Pages - 1) > Intersection.Maximum)) {

            return FALSE;
        }

        if (PreferHighAddresses != FALSE) {
            MappedBasePage = Intersection.Maximum - (Pages - 1);
            MappedBasePage -= AlignDownOffset;

            ASSERT(MappedBasePage >= Intersection.Minimum);

        } else {
            MappedBasePage = Intersection.Minimum;
        }
    }

    //
    // Check to make sure the region's attributes are adequate.  There are four
    // types of allocations; cachability, protection, allocation and
    // descriptive.  The first two (cachability and protection) describe
    // support.  For a region to be sufficient for a request, a memory region
    // needs to support *at least* the specified attributes.  The allocation
    // attributes require an *exact* match to assure the desired memory region
    // is being allocated for the caller.  The descriptive attributes are
    // descriptive only and do not need to match.
    //

    AttrSub1 = MemoryRegion->Attributes & MEMORY_ATTRIBUTE_CACHE_MASK;
    AttrSub2 = Attributes & MEMORY_ATTRIBUTE_CACHE_MASK;

    //
    // Caller requires a memory region to support cache attributes.
    //

    if (CHECK_FLAG(AttrSub1, AttrSub2) != AttrSub2) {
        return FALSE;
    }

    AttrSub1 = MemoryRegion->Attributes & MEMORY_ATTRIBUTE_PROTECT_MASK;
    AttrSub2 = Attributes & MEMORY_ATTRIBUTE_PROTECT_MASK;

    //
    // Caller requires a memory region to support protection attributes.
    //

    if (CHECK_FLAG(AttrSub1, AttrSub2) != AttrSub2) {
        return FALSE;
    }

    AttrSub1 = Attributes & MEMORY_ATTRIBUTE_ALLOCATION_ALLOCATE_MASK;
    AttrSub2 =
        MemoryRegion->Attributes & MEMORY_ATTRIBUTE_ALLOCATION_ALLOCATE_MASK;

    //
    // Caller requires a memory region to have some of the same allocation
    // attributes.  Allocation attributes not covered by the match mask indicate
    // address policy, which has already been encoded into the address ranges.
    //

    if (AttrSub1 != AttrSub2) {
        return FALSE;
    }

    //
    // Finally, check that the memory type is that which the caller requested.
    //

    if (MemoryRegion->MemoryType != RequiredMemoryType) {
        return FALSE;
    }

    //
    // Record the allocated region.
    //

    AllocateRegion->BasePage = BasePage;
    AllocateRegion->MappedBasePage = MappedBasePage;
    AllocateRegion->PageCount = Pages;
    AllocateRegion->Attributes = MemoryRegion->Attributes;
    AllocateRegion->MemoryType = MemoryRegion->MemoryType;
    return TRUE;
}

ULONG
MmMdCountList (
    __in PMEMORY_DESCRIPTOR_LIST Mdl
    )

/*++

Routine Description:

    Counts the number of descriptors in a memory descriptor list.

Arguments:

    Mdl - Supplies the memory descriptor list to count.

Return Value:

    The number of descriptors in the MDL.

--*/

{

    ULONG Count;
    PLIST_ENTRY NextEntry;

    Count = 0;
    NextEntry = Mdl->Head->Flink;
    while (NextEntry != Mdl->Head) {
        Count += 1;
        NextEntry = NextEntry->Flink;
    }

    return Count;
}

VOID
MmMdPrintList (
    __in PMEMORY_DESCRIPTOR_LIST DescriptorList
    )

/*++

Routine Description:

    Prints to the console the contents of each memory descriptor in the
    specified memory descriptor list..

Arguments:

    DescriptorList - Supplies the list to print.

Return Value:

    None.

--*/

{

    ULONGLONG EndPage;
    ULONGLONG MappedEndPage;
    PMEMORY_DESCRIPTOR MemoryDescriptor;
    PLIST_ENTRY NextEntry;

    NextEntry = DescriptorList->Head->Flink;
    while (NextEntry != DescriptorList->Head) {
        MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                             MEMORY_DESCRIPTOR,
                                             ListEntry);

        EndPage = MemoryDescriptor->BasePage + MemoryDescriptor->PageCount;
        if (MemoryDescriptor->MappedBasePage != 0) {
            MappedEndPage = MemoryDescriptor->MappedBasePage +
                            MemoryDescriptor->PageCount;

        } else {
            MappedEndPage = 0;
        }


        //
        // Print [PA BasePage:PA EndPage] [VA BasePage:VA EndPage]:MemoryType:Attributes
        //

        ConsolePrint(L"[%08x%08x:%08x%08x] [%08x%08x:%08x%08x]:%x:%x\r\n",
                     (ULONG)(MemoryDescriptor->BasePage >> 32),
                     (ULONG)MemoryDescriptor->BasePage,
                     (ULONG)(EndPage >> 32),
                     (ULONG)EndPage,
                     (ULONG)(MemoryDescriptor->MappedBasePage >> 32),
                     (ULONG)MemoryDescriptor->MappedBasePage,
                     (ULONG)(MappedEndPage >> 32),
                     (ULONG)MappedEndPage,
                     (ULONG)MemoryDescriptor->MemoryType,
                     (ULONG)MemoryDescriptor->Attributes);

        NextEntry = NextEntry->Flink;
    }
}

NTSTATUS
MmMdValidateList (
    __in PMEMORY_DESCRIPTOR_LIST DescriptorList
    )

/*++

Routine Description:

    Validates a memory descriptor list, asserting that the list does not
    contain any overlapping memory descriptors.

Arguments:

    DescriptorList - Supplies the list to validate.

Return Value:

    STATUS_UNSUCCESSFUL if there are any overlapping descriptors.

    STATUS_SUCCESS when no errors are found.

--*/

{

    ULONGLONG EndPage;
    PMEMORY_DESCRIPTOR MemoryDescriptor;
    PLIST_ENTRY NextEntry;

    //
    // Ensure there are no overlapping descriptor.  This can be done with a
    // single pass through the descriptor list, asserting that the previous
    // descriptor's memory range does not overlap with the current descriptor.
    //
    // N.B. This routine does not check for overlaps in mapped addresses in
    //      physical MDLs, since these addresses can be arbitrary and are not
    //      sorted.
    //

    EndPage = 0;
    NextEntry = DescriptorList->Head->Flink;
    while (NextEntry != DescriptorList->Head) {
        MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                             MEMORY_DESCRIPTOR,
                                             ListEntry);

        ASSERT(EndPage <= MemoryDescriptor->BasePage);

        if (EndPage > MemoryDescriptor->BasePage){
            return STATUS_UNSUCCESSFUL;
        }

        EndPage = MemoryDescriptor->BasePage + MemoryDescriptor->PageCount;
        NextEntry = NextEntry->Flink;
    }

    return STATUS_SUCCESS;
}

// --------------------------------------------------------- Internal Functions

VOID
MmMdpSaveCurrentListPointer (
    __inout PMEMORY_DESCRIPTOR_LIST Mdl,
    __in PLIST_ENTRY ListEntry
    )

/*++

Routine Description:

    This routine caches the given memory descriptor as the currently
    accessed element of an MDL.  Subsequent MDL searches may then start
    from this descriptor to reduce access time.

Arguments:

    Mdl - Supplies a pointer to the MDL to cache the pointer in.

    ListEntry - Supplies a pointer to the list entry in the MDL corresponding
        to the memory descriptor to be cached.

Return Value:

    None.

--*/

{

    PMEMORY_DESCRIPTOR EndAddress;

    //
    // Do not cache global memory descriptors.  These are reclaimed and freed
    // outside of normal MDL processing, and it is therefore possible to get
    // stale cache entries if these are cached.
    //

    EndAddress = &MmGlobalMemoryDescriptors[MAX_GLOBAL_DESCRIPTOR_COUNT];
    if (((ULONG_PTR)ListEntry >= (ULONG_PTR)MmGlobalMemoryDescriptors) &&
        ((ULONG_PTR)ListEntry < (ULONG_PTR)EndAddress)) {

        return;
    }

    //
    // Do not cache a list head.  This does not correspond to a memory
    // descriptor.
    //

    if (ListEntry != Mdl->Head) {
        Mdl->Current = ListEntry;
    }

    return;
}

VOID
MmMdpSwitchToDynamicDescriptors (
    __in ULONG NumberOfDescriptors
    )

/*++

Routine Description:

    This routine allocates a heap for dynamic descriptors and switches
    the global state to allocate new descriptors out of the dynamic descriptor
    buffer. If the buffer allocation fails, the application continues to
    use the static descriptors.

Arguments:

    NumberOfDescriptors - Supplies the total number of dynamic descriptors
        required.

Return Value:

    None.

--*/

{

    ULONG Size;
    NTSTATUS Status;

    Status = RtlULongMult(NumberOfDescriptors,
                          sizeof(MEMORY_DESCRIPTOR),
                          &Size);

    if (!NT_SUCCESS(Status)) {
        goto SwitchToDynamicDescriptorsEnd;
    }

    MmDynamicMemoryDescriptors = BlMmAllocateHeap(Size);

    //
    // Switch to dynamic descriptors only if the buffer could be allocated.
    //

    if (MmDynamicMemoryDescriptors == NULL) {
        goto SwitchToDynamicDescriptorsEnd;
    }

    //
    // All static descriptors generated during the heap allocation of dynamic
    // descriptor buffer should be first migrated to heap based descriptors.
    //
    // N.B. MmDescriptorCallTreeCount might have changed during the heap
    //      allocation above. But it should be 1 at this point to correspond
    //      with the increment done inside BlMmInitialize.
    //

    if (MmGlobalMemoryDescriptorsUsed != 0) {

        ASSERT(MmDescriptorCallTreeCount == 1);

        MmMdFreeGlobalDescriptors();

        //
        // Ensure that all the descriptors were correctly converted.
        //

        ASSERT((MmDescriptorCallTreeCount == 1) &&
               (MmGlobalMemoryDescriptorsUsed == 0));
    }

    //
    // Switch the application to use the dynamic descriptors.
    //

    MmGlobalMemoryDescriptors = MmDynamicMemoryDescriptors;
    MmDynamicMemoryDescriptorCount = NumberOfDescriptors;
    MmGlobalMemoryDescriptorCount = NumberOfDescriptors;
    RtlZeroMemory(MmGlobalMemoryDescriptors,
                  sizeof(MEMORY_DESCRIPTOR) * MAX_GLOBAL_DESCRIPTOR_COUNT);

    MmGlobalMemoryDescriptorsUsed = 0;

SwitchToDynamicDescriptorsEnd:
    return;
}

NTSTATUS
MmMdpSwitchToStaticDescriptors (
    VOID
    )

/*++

Routine Description:

    This routine switches the application to use static memory descriptors.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS if the application is using only static descriptors.

    Appropriate NTSTATUS code otherwise.

--*/

{

    NTSTATUS Status;

    //
    // Switching to static descriptors is only required if the application is
    // currently using dynamic descriptors.
    //

    if (MmGlobalMemoryDescriptors != MmDynamicMemoryDescriptors) {
        Status = STATUS_SUCCESS;
        goto SwitchToStaticDescriptorsEnd;
    }

    //
    // Memory operations that occur before the attempt to switch to
    // static descriptors might generate new descriptors. All such
    // descriptors will be resident in the dynamic descriptor buffer.
    // Convert such descriptors to heap-based descriptors in preparation
    // to free the dynamic descriptor buffer.
    //

    if (MmGlobalMemoryDescriptorsUsed != 0) {

        ASSERT(MmDescriptorCallTreeCount == 1);

        MmMdFreeGlobalDescriptors();

        //
        // Ensure that all the descriptors were correctly converted.
        //

        ASSERT((MmDescriptorCallTreeCount == 1) &&
               (MmGlobalMemoryDescriptorsUsed == 0));
    }

    //
    // Switch to static descriptors before freeing the dynamic descriptor
    // buffer. This is done to prevent new descriptors from being allocated
    // from the dynamic descriptor buffer that is being freed!
    //

    MmGlobalMemoryDescriptors = MmStaticMemoryDescriptors;
    MmGlobalMemoryDescriptorCount = MAX_STATIC_DESCRIPTOR_COUNT;
    RtlZeroMemory(MmGlobalMemoryDescriptors,
                  sizeof(MEMORY_DESCRIPTOR) * MAX_GLOBAL_DESCRIPTOR_COUNT);

    MmGlobalMemoryDescriptorsUsed = 0;

    //
    // Free the dynamic descriptor buffer.
    //

    Status = BlMmFreeHeap(MmDynamicMemoryDescriptors);

    ASSERT(Status == STATUS_SUCCESS);

    MmDynamicMemoryDescriptors = NULL;
    MmDynamicMemoryDescriptorCount = 0;

SwitchToStaticDescriptorsEnd:
    return Status;
}

BOOLEAN
MmMdpTruncateDescriptor (
    __in PMEMORY_DESCRIPTOR_LIST Mdl,
    __in PMEMORY_DESCRIPTOR MemoryDescriptor,
    __in ULONG InsertionFlags
    )

/*++

Routine Description:

    Truncates any overlapping regions between the specified memory
    descriptor and its adjacent descriptors.  This process is beneficial
    for platforms whose firmware used a different memory page size than
    the Boot Environment Library.  If the granularity of the firmware page
    size is smaller than the OS's, then there could be memory
    described by the firmware that is only partially in a memory region
    described by the OS.  If a firmware stub lets each partial page look
    like a full page, this routine can properly decide which memory type
    should take precedence.

    N.B. If this routine has to split a descriptor, it will use one of the
         descriptors from the global array.  It will be the caller's
         responsibility to free the descriptor, since this routine can be
         called during initialization before the heap allocator has been
         initialized.

Arguments:

    Mdl - Supplies the memory descriptor list containing the given memory
        descriptor.

    MemoryDescriptor - Supplies the memory descriptor to search the MDL for
        overlaps.

    InsertionFlags - Supplies flags to use when inserting split descriptors
        as part of the truncation.

Return Value:

    TRUE if MemoryDescriptor was deleted from list.

--*/

{

    PMEMORY_DESCRIPTOR BlinkDescriptor;
    ULONGLONG BlinkDescriptorEnd;
    ULONGLONG Difference;
    PMEMORY_DESCRIPTOR FlinkDescriptor;
    ULONGLONG FlinkDescriptorEnd;
    PLIST_ENTRY ListBlink;
    PLIST_ENTRY ListFlink;
    ULONGLONG MappedBasePage;
    ULONGLONG MemoryDescriptorEnd;
    PMEMORY_DESCRIPTOR SplitDescriptor;

    //
    // Get the adjacent memory descriptors.
    //

    ListBlink = MemoryDescriptor->ListEntry.Blink;
    ListFlink = MemoryDescriptor->ListEntry.Flink;
    BlinkDescriptor = CONTAINING_RECORD(ListBlink,
                                        MEMORY_DESCRIPTOR,
                                        ListEntry);

    FlinkDescriptor = CONTAINING_RECORD(ListFlink,
                                        MEMORY_DESCRIPTOR,
                                        ListEntry);

    MemoryDescriptorEnd = MemoryDescriptor->BasePage +
                          MemoryDescriptor->PageCount;

    BlinkDescriptorEnd = BlinkDescriptor->BasePage + BlinkDescriptor->PageCount;
    FlinkDescriptorEnd = FlinkDescriptor->BasePage + FlinkDescriptor->PageCount;

    //
    // Check for overlapping regions with previous descriptor.
    //

    if ((ListBlink != Mdl->Head) &&
        (MemoryDescriptor->BasePage < BlinkDescriptorEnd)) {

        //
        // If two descriptors have the same memory type, give precedence to
        // the descriptor with the lowest base page (first arguement to
        // MmMdpHasPrecedence).
        //

        if (MmMdpHasPrecedence(MemoryDescriptor->MemoryType,
                               BlinkDescriptor->MemoryType) != FALSE) {

            //
            // The current memory descriptor has precedence.  Therefore,
            // the overlapping region in the previous descriptor needs to
            // be removed from the previous descriptor.
            //
            // N.B. This operation could potentially split the previous
            //      descriptor into two regions.  The MDL is ordered by
            //      base page.  However, this does not say anything about
            //      the end page.
            //
            // Adjust the end page of the overlapping descriptor.
            //

            BlinkDescriptor->PageCount =
                MemoryDescriptor->BasePage - BlinkDescriptor->BasePage;

            //
            // Split the previous descriptor if necessary.
            //

            if (MemoryDescriptorEnd < BlinkDescriptorEnd) {
                if (BlinkDescriptor->MappedBasePage != 0) {
                    MappedBasePage =
                        BlinkDescriptor->MappedBasePage +
                        MemoryDescriptorEnd -
                        BlinkDescriptor->BasePage;

                } else {
                    MappedBasePage = 0;
                }

                SplitDescriptor =
                    MmMdInitDescriptor(MemoryDescriptorEnd,
                                       MappedBasePage,
                                       BlinkDescriptorEnd - MemoryDescriptorEnd,
                                       BlinkDescriptor->Attributes,
                                       BlinkDescriptor->MemoryType);

                if (SplitDescriptor != NULL) {
                    MmMdAddDescriptorToList(Mdl,
                                            SplitDescriptor,
                                            InsertionFlags);
                }
            }

            //
            // Remove the previous descriptor from the list if the current
            // descriptor overlaps the entire region.
            //

            if (BlinkDescriptor->PageCount == 0) {
                MmMdRemoveDescriptorFromList(Mdl, BlinkDescriptor);
                MmMdFreeDescriptor(BlinkDescriptor);
            }

        } else {

            //
            // The previous descriptor has precedence.  The overlapping
            // region in the current memory descriptor needs to be removed.
            //

            if (MemoryDescriptorEnd <= BlinkDescriptorEnd) {

                //
                // If the previous descriptor completely overlaps with the
                // current descriptor, remove it from the MDL.
                //

                MmMdRemoveDescriptorFromList(Mdl, MemoryDescriptor);
                MmMdFreeDescriptor(MemoryDescriptor);

                //
                // There is no reason to fall through to see if the current
                // descriptor can be truncated with the next descriptor.
                // Return now.
                //

                return TRUE;
            }

            //
            // Adjust start page for current descriptor.
            //

            Difference = BlinkDescriptorEnd - MemoryDescriptor->BasePage;
            MemoryDescriptor->BasePage += Difference;
            MemoryDescriptor->PageCount -= Difference;
            if (MemoryDescriptor->MappedBasePage != 0) {
                MemoryDescriptor->MappedBasePage += Difference;
            }
        }
    }

    //
    // Check for overlapping region with next descriptor.
    //

    if ((ListFlink != Mdl->Head) &&
        (FlinkDescriptor->BasePage < MemoryDescriptorEnd)) {

        //
        // If two descriptors have the same memory type, give precedence to
        // the descriptor with the lowest base page (first argument to
        // MmMdpHasPrecedence).
        //

        if (MmMdpHasPrecedence(FlinkDescriptor->MemoryType,
                               MemoryDescriptor->MemoryType) != FALSE) {

            //
            // The next memory descriptor has precedence.  Therefore, the
            // overlapping region in the current memory descriptor needs
            // to be removed.
            //
            // N.B. This operation could potentially split the previous
            //      descriptor into two regions.  The MDL is ordered by
            //      base page.  However, this does not say anything about
            //      the end page.
            //
            // Adjust the end page of the current descriptor.
            //

            MemoryDescriptor->PageCount =
                FlinkDescriptor->BasePage - MemoryDescriptor->BasePage;

            //
            // Split the current descriptor if necessary.
            //

            if (FlinkDescriptorEnd < MemoryDescriptorEnd) {
                if (MemoryDescriptor->MappedBasePage != 0) {
                    MappedBasePage =
                        MemoryDescriptor->MappedBasePage +
                        FlinkDescriptorEnd -
                        MemoryDescriptor->BasePage;

                } else {
                    MappedBasePage = 0;
                }

                SplitDescriptor =
                    MmMdInitDescriptor(FlinkDescriptorEnd,
                                       MappedBasePage,
                                       MemoryDescriptorEnd - FlinkDescriptorEnd,
                                       MemoryDescriptor->Attributes,
                                       MemoryDescriptor->MemoryType);

                if (SplitDescriptor != NULL) {
                    MmMdAddDescriptorToList(Mdl, SplitDescriptor, InsertionFlags);
                }
            }

            //
            // Remove the current descriptor from the list if the next
            // descriptor completely overlaps with the current descriptor.
            //

            if (MemoryDescriptor->PageCount == 0) {
                MmMdRemoveDescriptorFromList(Mdl, MemoryDescriptor);
                MmMdFreeDescriptor(MemoryDescriptor);

                //
                // The current descriptor has been deleted.  Return this
                // status to the caller.
                //

                return TRUE;
            }

        } else {

            //
            // The current memory descriptor has precedence.  Remove the
            // overlapping region in the next descriptor.
            //

            if (FlinkDescriptorEnd <= MemoryDescriptorEnd) {

                //
                // If the next descriptor completely overlaps with the current
                // descriptor, remove it from the MDL.
                //

                MmMdRemoveDescriptorFromList(Mdl, FlinkDescriptor);
                MmMdFreeDescriptor(FlinkDescriptor);

            } else {

                //
                // Adjust the start page for the next descriptor.
                //

                Difference = MemoryDescriptorEnd - FlinkDescriptor->BasePage;
                FlinkDescriptor->BasePage += Difference;
                FlinkDescriptor->PageCount -= Difference;
                if (FlinkDescriptor->MappedBasePage != 0) {
                    FlinkDescriptor->MappedBasePage += Difference;
                }
            }
        }
    }

    return FALSE;
}

BOOLEAN
MmMdpCoalesceDescriptor (
    __in PMEMORY_DESCRIPTOR_LIST Mdl,
    __in PMEMORY_DESCRIPTOR MemoryDescriptor,
    __in ULONG InsertionFlags
    )

/*++

Routine Description:

    Attempts to coalesce the specified memory descriptor with its adjacent
    descriptors.  Coalescing will occur if and only if the memory descriptor
    shares a memory boundary with an adjacent descriptor and the two also
    share the same memory type.

Arguments:

    Mdl - Supplies a memory descriptor list containing the given descriptor.

    MemoryDescriptor - Supplies the memory descriptor  to attempt to coalesce
        with adjacent descriptors.

    InsertionFlags - Insertion flags.  Not currently used.

Return Value:

    TRUE if MemoryDescriptor was deleted from list as a result of being
    coalesced with an adjacent descriptor.
    FALSE otherwise.

--*/

{

    PMEMORY_DESCRIPTOR BlinkDescriptor;
    BOOLEAN CoalescedDescriptor;
    PMEMORY_DESCRIPTOR FlinkDescriptor;
    PLIST_ENTRY ListBlink;
    PLIST_ENTRY ListFlink;

    UNREFERENCED_PARAMETER(InsertionFlags);

    CoalescedDescriptor = FALSE;

    //
    // Get adjacent descriptors.
    //

    ListBlink = MemoryDescriptor->ListEntry.Blink;
    ListFlink = MemoryDescriptor->ListEntry.Flink;
    BlinkDescriptor = CONTAINING_RECORD(ListBlink,
                                        MEMORY_DESCRIPTOR,
                                        ListEntry);

    FlinkDescriptor = CONTAINING_RECORD(ListFlink,
                                        MEMORY_DESCRIPTOR,
                                        ListEntry);

    //
    // Attempt to coalesce with previous descriptor.
    //

    if ((ListBlink != Mdl->Head) &&

        //
        // In order to coalesce, the newly inserted descriptor and the
        // previous descriptor must have the same memory type.
        //

        (BlinkDescriptor->MemoryType == MemoryDescriptor->MemoryType) &&

        //
        // Memory ranges with different attributes must remain partitioned
        // and can not be coalesced.
        //

        (DO_COALESCE_ATTRIBUTES_MATCH(BlinkDescriptor->Attributes,
                                      MemoryDescriptor->Attributes) != FALSE) &&

        //
        // And descriptors must describe adjacent memory regions (both
        // physically and virtually).
        //

        ((BlinkDescriptor->BasePage + BlinkDescriptor->PageCount) ==
         MemoryDescriptor->BasePage) &&

        //
        // Only compare for mapped address adjacency if both regions are mapped.
        //

        (((MemoryDescriptor->MappedBasePage == 0) &&
          (BlinkDescriptor->MappedBasePage == 0))

         ||

         ((MemoryDescriptor->MappedBasePage != 0) &&
          (BlinkDescriptor->MappedBasePage != 0) &&
          (BlinkDescriptor->MappedBasePage + BlinkDescriptor->PageCount ==
           MemoryDescriptor->MappedBasePage)))) {

        //
        // The specified memory descriptor can be coalesced with the
        // previous descriptor.
        //

        BlinkDescriptor->PageCount += MemoryDescriptor->PageCount;
        MmMdRemoveDescriptorFromList(Mdl, MemoryDescriptor);
        MmMdFreeDescriptor(MemoryDescriptor);

        //
        // Fall through to next case, using the newly formed BlinkDescriptor.
        //

        CoalescedDescriptor = TRUE;
        MemoryDescriptor = BlinkDescriptor;
    }

    //
    // Attempt to coalesce with next descriptor.
    //

    if ((ListFlink != Mdl->Head) &&

        //
        // In order to coalesce, the newly inserted descriptor and the
        // next descriptor must have the same memory type.
        //

        (FlinkDescriptor->MemoryType == MemoryDescriptor->MemoryType) &&

        //
        // Memory ranges with different attributes must remain partitioned
        // and can not be coalesced.
        //

        (DO_COALESCE_ATTRIBUTES_MATCH(FlinkDescriptor->Attributes,
                                      MemoryDescriptor->Attributes) != FALSE) &&

        //
        // And descriptors must describe adjacent memory regions (both
        // physically and virtually).
        //

        ((MemoryDescriptor->BasePage + MemoryDescriptor->PageCount) ==
         FlinkDescriptor->BasePage) &&

        //
        // Only compare for virtual adjacency if both regions are virtually
        // mapped.
        //

        (((MemoryDescriptor->MappedBasePage == 0) &&
          (FlinkDescriptor->MappedBasePage == 0))

         ||

         ((MemoryDescriptor->MappedBasePage != 0) &&
          (FlinkDescriptor->MappedBasePage != 0) &&
          ((MemoryDescriptor->MappedBasePage + MemoryDescriptor->PageCount) ==
           FlinkDescriptor->MappedBasePage)))) {

        //
        // The specified memory descriptor can be coalesced with the
        // next descriptor.
        //

        FlinkDescriptor->BasePage = MemoryDescriptor->BasePage;
        FlinkDescriptor->MappedBasePage  = MemoryDescriptor->MappedBasePage;
        FlinkDescriptor->PageCount += MemoryDescriptor->PageCount;
        MmMdRemoveDescriptorFromList(Mdl, MemoryDescriptor);
        MmMdFreeDescriptor(MemoryDescriptor);
        CoalescedDescriptor = TRUE;
    }

    return CoalescedDescriptor;
}

BOOLEAN
MmMdpHasPrecedence (
    __in MEMORY_TYPE Type1,
    __in MEMORY_TYPE Type2
    )

/*++

Routine Description:

    Determines if a memory region with Type1 memory has precedence over a
    memory region with Type2 memory.  Precedence rules are as follows:

    - Free memory has the lowest precedence.
    - In use, platform memory has highest precedence.
    - Library memory has precendence over application memory.

Arguments:

    Type1 - First memory type to compare.

    Type2 - Second memory type to compare.

Return Value:

    TRUE if Memory type 1 has precedence over Memory type 2.
    Tie goes to Type1.

--*/

{

    ULONG PrecedenceIndex1;
    ULONG PrecedenceIndex2;
    UNPACKED_MEMORY_TYPE UnpackedType1;
    UNPACKED_MEMORY_TYPE UnpackedType2;

    //
    // Take a shortcut in case if one of types is 'free' or 'zeroed'
    // as this types have the lowest precedence.
    //

    if (Type2 == MEMORY_TYPE_FREE_ZEROED) {
        return TRUE;

    } else if (Type1 == MEMORY_TYPE_FREE_ZEROED) {
        return FALSE;
    }

    if (Type2 == MEMORY_TYPE_FREE) {
        return TRUE;

    } else if (Type1 == MEMORY_TYPE_FREE) {
        return FALSE;
    }

    //
    // If either memory type is an unknown class, assume it has precedence.
    //

    UnpackedType1.MemoryType = Type1;
    UnpackedType2.MemoryType = Type2;
    if ((UnpackedType1.Class != MEMORY_CLASS_PLATFORM) &&
        (UnpackedType1.Class != MEMORY_CLASS_LIBRARY) &&
        (UnpackedType1.Class != MEMORY_CLASS_APPLICATION)) {

        return TRUE;
    }

    if ((UnpackedType2.Class != MEMORY_CLASS_PLATFORM) &&
        (UnpackedType2.Class != MEMORY_CLASS_LIBRARY) &&
        (UnpackedType2.Class != MEMORY_CLASS_APPLICATION)) {

        return FALSE;
    }

    //
    // Both memory types are of known memory class.  Compare each and determine
    // which has precendence.  If both memory types have the same memory class,
    // precendence goes to Type1.
    //

    if (UnpackedType1.Class == MEMORY_CLASS_PLATFORM) {
        if (UnpackedType2.Class != MEMORY_CLASS_PLATFORM) {
            return TRUE;

        } else {

            //
            // Both memory types are of platform class.  Finer grained
            // comparison needs to be done based on the specific types in
            // question.  Types with lower precedence indices carry
            // higher precedence.  Ties go to Type1.
            //

            PrecedenceIndex1 = MmMdpLookupTypePrecedenceIndex(Type1);
            PrecedenceIndex2 = MmMdpLookupTypePrecedenceIndex(Type2);
            if (PrecedenceIndex1 == MM_MAXIMAL_PRECEDENCE) {
                return TRUE;

            } else if (PrecedenceIndex2 == MM_MAXIMAL_PRECEDENCE) {
                return FALSE;

            } else {
                if (PrecedenceIndex1 <= PrecedenceIndex2) {
                    return TRUE;

                } else {
                    return FALSE;
                }
            }
        }

    } else if (UnpackedType2.Class == MEMORY_CLASS_PLATFORM) {
        return FALSE;
    }

    if (UnpackedType1.Class == MEMORY_CLASS_LIBRARY) {
        return TRUE;

    } else if (UnpackedType2.Class == MEMORY_CLASS_LIBRARY) {
        return FALSE;
    }

    //
    // Both are of class, MEMORY_CLASS_APPLICATION.  Precedence goes to Type1.
    //

    return TRUE;
}

ULONG
MmMdpLookupTypePrecedenceIndex (
    __in MEMORY_TYPE Type
    )

/*++

Routine Description:

    This routine determines the index of the given memory type within the
    platform memory type precedence list.

Arguments:

    Type - Supplies the type to look up in the precedence list.

Return Value:

    The index of the given memory type within the platform type precedence
    list or MM_MAXIMAL_PRECEDENCE if the given type is not listed and should
    thus be given maximum precedence.

--*/

{

    MEMORY_TYPE CurrentType;
    ULONG Index;

    for (Index = 0;
         Index < RTL_NUMBER_OF(MmPlatformMemoryTypePrecedence);
         Index += 1) {

        CurrentType = MmPlatformMemoryTypePrecedence[Index];
        if (CurrentType == Type) {
            return Index;
        }
    }

    return MM_MAXIMAL_PRECEDENCE;
}

