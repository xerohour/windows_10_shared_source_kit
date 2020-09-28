/*++

Copyright (c) Microsoft Corporation

Module Name:

    schema.c

Abstract:

    Implementation of the Windows composible API set schema.

--*/

#include "osloader.h"

NTSTATUS
OslpLoadApiSetSchemaImage (
    _In_ DEVICE_ID OsDeviceId,
    _In_ PWSTR ImagePath,
    _Outptr_result_bytebuffer_(*SchemaDataSize) PVOID *SchemaData,
    _Out_ ULONG *SchemaDataSize,
    _Inout_ PLIST_ENTRY LoadedImageList,
    _Outptr_ PKLDR_DATA_TABLE_ENTRY *DataTableEntry
    );

VOID
OslpUnloadApiSetSchemaImage (
    _Inout_ PKLDR_DATA_TABLE_ENTRY DataTableEntry
    );

PVOID OslApiSetSecureSchema;
PVOID OslApiSetSchema;
ULONG OslApiSetSchemaSize;

NTSTATUS
OslLoadApiSetSchema (
    _In_ PLOADER_PARAMETER_BLOCK LoaderBlock,
    _In_ ULONG SystemHiveHandle,
    _Inout_ PUNICODE_STRING OsFilePath,
    _In_ DEVICE_ID OsDeviceId
    )

/*++

Routine Description:

    This routine loads the API set schema.

Arguments:

    LoaderBlock - A pointer to the loader parameter block.

    OsFilePath - The file path to the Operating System directory.

    OsDeviceId - Handle to the device containing the Operating System.

Return Value:

    NTSTATUS code.

--*/

{
    BOOLEAN BoolRet;
    PKLDR_DATA_TABLE_ENTRY DataTableEntry;
    PAPI_SET_SCHEMA_EXTENSION_NODE Entry;
    PLIST_ENTRY Extensions;
    USHORT OsFilePathLength;
    PLIST_ENTRY Node;
    NTSTATUS Status;
    PVOID Schema;
    PVOID SchemaData;
    ULONG SchemaDataSize;
    ULONG SchemaSize;
    BOOLEAN Sealed;

    DataTableEntry = NULL;
    Schema = NULL;

    //
    // Construct the path to the base apiset schema file and load it.
    //

    OsFilePathLength = OsFilePath->Length;
    BoolRet = BlAppendUnicodeToString(OsFilePath, APISET_SCHEMA_NAME);
    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OslLoadApiSetSchemaEnd;
    }

    Status = OslpLoadApiSetSchemaImage(OsDeviceId,
                                       OsFilePath->Buffer,
                                       &SchemaData,
                                       &SchemaDataSize,
                                       &LoaderBlock->LoadOrderListHead,
                                       &DataTableEntry);

    if (!NT_SUCCESS(Status)) {
        goto OslLoadApiSetSchemaEnd;
    }

    OsFilePath->Buffer[OsFilePathLength / sizeof(WCHAR)] = UNICODE_NULL;
    OsFilePath->Length = OsFilePathLength;

    //
    // Allocate a buffer and copy the base schema into it, and then unload the
    // image.
    //

    SchemaSize = SchemaDataSize;
    Schema = BlMmAllocateHeap(SchemaSize);
    if (Schema == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OslLoadApiSetSchemaEnd;
    }

    RtlCopyMemory(Schema, SchemaData, SchemaSize);
    OslpUnloadApiSetSchemaImage(DataTableEntry);

    //
    // Save the schema data to global variables for use by the OS loader, and
    // to the loader parameter block for later use by the kernel.
    //

    OslApiSetSchema = Schema;
    OslApiSetSchemaSize = SchemaSize;
    LoaderBlock->Extension->ApiSetSchema = Schema;
    LoaderBlock->Extension->ApiSetSchemaSize = SchemaSize;

    //
    // If the schema is sealed, then ignore any registered API set schema
    // extensions. Composition will not be attempted.
    //

    Sealed = ApiSetIsSchemaSealed(Schema);
    if (Sealed != FALSE) {
        goto OslLoadApiSetSchemaEnd;
    }

    //
    // Obtain the list of schema extensions that must be loaded for composition
    // and then add the extensions into the base schema.
    //

    Extensions = &LoaderBlock->Extension->ApiSetSchemaExtensions;
    Status = OslHiveGetApiSetSchemaExtensions(SystemHiveHandle, Extensions);
    if (!NT_SUCCESS(Status)) {
        goto OslLoadApiSetSchemaEnd;
    }

    for (Node = Extensions->Flink; Node != Extensions; Node = Node->Flink) {
        Entry = CONTAINING_RECORD(Node, API_SET_SCHEMA_EXTENSION_NODE, Links);

        //
        // Load the next API set schema extension image. Note that if this
        // fails, we should still attempt to boot. The entry in the list shows
        // the failure so that the kernel can report it later.
        //

        BoolRet = BlAppendUnicodeToString(OsFilePath, Entry->FileName.Buffer);
        if (BoolRet == FALSE) {
            Status = STATUS_NO_MEMORY;
            goto OslLoadApiSetSchemaEnd;
        }

        Status = OslpLoadApiSetSchemaImage(OsDeviceId,
                                           OsFilePath->Buffer,
                                           &SchemaData,
                                           &SchemaDataSize,
                                           &LoaderBlock->LoadOrderListHead,
                                           &DataTableEntry);

        OsFilePath->Buffer[OsFilePathLength / sizeof(WCHAR)] = UNICODE_NULL;
        OsFilePath->Length = OsFilePathLength;
        if (!NT_SUCCESS(Status)) {
            Entry->Status = Status;
            continue;
        }

        //
        // Process the API set schema extension data. There is no need to unpack
        // the data into a buffer, as it will not be modified and will no longer
        // be needed once it has been added into the schema. If this fails, then
        // boot will continue so that the error can be reported by the OS. Note
        // that in the case of failure, the module remains loaded so that it can
        // be examined.
        //

        Status =
            ApiSetComposeSchema((PAPI_SET_NAMESPACE *)&OslApiSetSchema,
                                &OslApiSetSchemaSize,
                                SchemaData,
                                SchemaDataSize);

        if (!NT_SUCCESS(Status)) {
            Entry->Status = Status;
            continue;
        }

        OslpUnloadApiSetSchemaImage(DataTableEntry);
    }

    LoaderBlock->Extension->ApiSetSchema = OslApiSetSchema;
    LoaderBlock->Extension->ApiSetSchemaSize = OslApiSetSchemaSize;
    Status = STATUS_SUCCESS;

OslLoadApiSetSchemaEnd:
    return Status;
}

NTSTATUS
OslpLoadApiSetSchemaImage (
    _In_ DEVICE_ID OsDeviceId,
    _In_ PWSTR ImagePath,
    _Outptr_result_bytebuffer_(*SchemaDataSize) PVOID *SchemaData,
    _Out_ ULONG *SchemaDataSize,
    _Inout_ PLIST_ENTRY LoadedImageList,
    _Outptr_ PKLDR_DATA_TABLE_ENTRY *DataTableEntry
    )

/*++

Routine Description:

    This routine will load the specified API set schema data file, returning the
    location and size of the schema data.

    N.B. The caller should take care to only access this data while the data file
         remains loaded.

Arguments:

    OsDeviceId - Supplies the device ID for the Windows boot partition.

    ImagePath - Supplies a pointer to the name of the API set schema file.

    LoaderBlock - Supplies a pointer to the loader parameter block.

    SchemaData - Supplies a location in which a pointer to the schema data is
        returned.

    SchemaDataSize - Supllies a location in which the size of the schema data is
        returned.

    LoadedImageList - Supplies a pointer to the load image list head. The loaded
        schema image will be added to the list.

Return Value:

    NTSTATUS code.

--*/

{

    PKLDR_DATA_TABLE_ENTRY LoadedModule;
    PIMAGE_SECTION_HEADER Section;
    NTSTATUS Status;

    Status = OslLoadImage(OsDeviceId,
                          OSL_MEMORY_TYPE_SYSTEM_CODE,
                          ImagePath,
                          NULL,
                          NULL,
                          NULL,
                          0,
                          LoadedImageList,
                          &LoadedModule,
                          0,
                          0,
                          IMAGE_FLAGS_NONE,
                          OSL_IMAGE_FLAGS_APISET_SCHEMA,
                          NULL);

    if (!NT_SUCCESS(Status)) {
        goto OslpLoadApiSetSchemaImageEnd;
    }

    //
    // Find the section that contains the schema data.
    //

    Section = BlImgFindSection(LoadedModule->DllBase,
                               LoadedModule->SizeOfImage,
                               API_SET_SECTION_NAME);

    if (Section == NULL) {
        OslpUnloadApiSetSchemaImage(LoadedModule);
        Status = STATUS_INVALID_IMAGE_FORMAT;
        goto OslpLoadApiSetSchemaImageEnd;
    }

    //
    // Return the location and size of the schema data. The caller must ony
    // access this data while the image is still loaded.
    //

    *SchemaData = Add2Ptr(LoadedModule->DllBase, Section->VirtualAddress);
    *SchemaDataSize = Section->Misc.VirtualSize;
    *DataTableEntry = LoadedModule;

OslpLoadApiSetSchemaImageEnd:
    return Status;
}

VOID
OslpUnloadApiSetSchemaImage (
    _Inout_ PKLDR_DATA_TABLE_ENTRY DataTableEntry
    )

/*++

Routine Description:

    This routine will unload the specified image.

Arguments:

    DataTableEntry - Supplies a pointer to the loaded module data table entry
        returned when the module was loaded.

Return Value:

    None.

--*/

{

    BlLdrUnloadImage(DataTableEntry);
    return;
}

