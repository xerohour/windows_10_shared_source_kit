/*++

Copyright (c) Microsoft Corporation

Module Name:

    resmcntx.c

Abstract:

    Implementation of resume context support for passing information
    between the loader and resume boot applications.

Environment:

    Boot Environment.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"

//
// -------------------------------------------------------------------- Globals
//

PRESUME_CONTEXT ResumeContext = NULL;

//
// ---------------------------------------------------------- Private Functions
//

ULONG
OslpGetBootFootPrintSize (
    __in DEVICE_ID OsDeviceId
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
OslDestroyResumeContext (
    VOID
    )

/*++

Routine Description:

    This routine destroys the resume context in cases where the loader fails.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

    GENERIC_BUFFER PersistentData;
    GUID ResumeContextGUID = RESUME_CONTEXT_GUID;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;
    if (ResumeContext != NULL) {
        BlInitGenericBuffer(&PersistentData,
                            ResumeContext,
                            ResumeContext->Size);

        Status = BlPdFreeData(NULL, &ResumeContextGUID, &PersistentData);
        ResumeContext = NULL;
    }

    return Status;
}

NTSTATUS
OslInitializeResumeContext (
    VOID
    )

/*++

Routine Description:

    This routine creates the resume context that gets passed to the resume
    application during resume from hibernate.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

    PMEMORY_DESCRIPTOR Descriptor;
    LIST_ENTRY MemoryMap;
    GENERIC_BUFFER MemoryMapDescriptors;
    PLIST_ENTRY NextEntry;
    GENERIC_BUFFER PersistentData;
    GUID ResumeContextGUID = RESUME_CONTEXT_GUID;
    PMEMORY_DESCRIPTOR ResumeDescriptor;
    NTSTATUS Status;

    BlInitGenericBuffer(&MemoryMapDescriptors, NULL, 0);
    BlInitGenericBuffer(&PersistentData, NULL, 0);

    //
    // Obtain a copy of the firmware memory map. Get the non-coalesced version
    // so all firmware runtime regions get their own descriptor.
    //

    Status = BlMmGetMemoryMap(&MemoryMap,
                              &MemoryMapDescriptors,
                              MEMORYMAP_TYPE_FIRMWARE_NOT_COALESCED,
                              MDL_FLAGS_ALLOCATE_DESCRIPTORS);

    if (!NT_SUCCESS(Status)) {
        goto InitializeResumeContextEnd;
    }

    //
    // Allocate a persistent buffer for the resume context.
    //

    PersistentData.DataSize = FIELD_OFFSET(RESUME_CONTEXT, PhysicalMemoryMap) +
        MemoryMapDescriptors.DataSize;

    Status = BlPdAllocateData(NULL,
                              &ResumeContextGUID,
                              MEMORY_ATTRIBUTE_ALLOCATION_KSEG,
                              &PersistentData);

    if (!NT_SUCCESS(Status)) {
        goto InitializeResumeContextEnd;
    }

    //
    // Initialize the resume context and determine the size of boot code and
    // data regions.
    //

    ResumeContext = PersistentData.Buffer;
    RtlZeroMemory(ResumeContext, PersistentData.DataSize);
    ResumeContext->Revision = RESUME_CONTEXT_VERSION;
    ResumeContext->Size = PersistentData.DataSize;
    ResumeDescriptor = ResumeContext->PhysicalMemoryMap;
    NextEntry = MemoryMap.Flink;
    while (NextEntry != &MemoryMap) {
        Descriptor = CONTAINING_RECORD(NextEntry,
                                       MEMORY_DESCRIPTOR,
                                       ListEntry);

        ResumeDescriptor->BasePage = Descriptor->BasePage;
        ResumeDescriptor->PageCount = Descriptor->PageCount;
        ResumeDescriptor->MemoryType = Descriptor->MemoryType;
        ResumeContext->PhysicalMemoryMapCount += 1;
        ResumeDescriptor += 1;
        NextEntry = NextEntry->Flink;
    }

    ASSERT(ResumeContext->PhysicalMemoryMapCount != 0);

InitializeResumeContextEnd:
    if (!NT_SUCCESS(Status) && (PersistentData.Buffer != NULL)) {
        NT_VERIFY(
            NT_SUCCESS(
                BlPdFreeData(NULL, &ResumeContextGUID, &PersistentData)));
    }

    if (MemoryMapDescriptors.Buffer != NULL) {
        BlMmFreeHeap(MemoryMapDescriptors.Buffer);
    }

    return Status;
}

NTSTATUS
OslInitializeResumePages (
    __in DEVICE_ID OsDeviceId,
    __out PULONG ResumePages
    )

/*++

Routine Description:

    This routine determines the number of resume pages that are required to
    resume from hibernate.  This value will be passed to the power manager
    thru the loader block.

Arguments:

    OsDeviceId - Supplies the device ID for the OS device.

    ResumePages - Supplies an integer buffer which receives the page count.

Return Value:

    NT status code.

--*/

{

    ULONGLONG BootServicesPages;
    PMEMORY_DESCRIPTOR Descriptor;
    ULONG Index;
    ULONG Size;

    if (ResumeContext == NULL) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // For EFI systems the boot services memory regions increase the scratch
    // space requirement as they must also persist until the final stage of
    // the resume process.  Determine the size of the boot services memory
    // regions.
    //

    BootServicesPages = 0;
    Descriptor = ResumeContext->PhysicalMemoryMap;
    for (Index = 0;
         Index < ResumeContext->PhysicalMemoryMapCount;
         Index += 1) {

        if ((Descriptor->MemoryType == MEMORY_TYPE_BOOT_SERVICE_CODE)) {
            BootServicesPages += Descriptor->PageCount;
        }

        Descriptor += 1;
    }

    Size = OslpGetBootFootPrintSize(OsDeviceId);
    *ResumePages = Size >> PAGE_SHIFT;
    *ResumePages += (ULONG)BootServicesPages;
    return STATUS_SUCCESS;
}

//
// ---------------------------------------------------------- Private Functions
//

ULONG
OslpGetBootFootPrintSize (
    __in DEVICE_ID OsDeviceId
    )

/*++

Routine Description:

    This routine determines the boot environment foot print size.

Arguments:

    OsDeviceId - Supplies the device ID for the OS device.

Return Value:

    ULONG - Size of boot environment foot print (in bytes).

--*/

{
    UNREFERENCED_PARAMETER(OsDeviceId);

    return BOOT_ENVIRONMENT_FOOTPRINT_BASE_SIZE;
}

