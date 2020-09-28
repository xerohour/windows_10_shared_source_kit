/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    loader.c

Abstract:

    This module implements the code to load an image into memory.

Environment:

    Boot

--*/

// ------------------------------------------------------------------- Includes

#include "image.h"

// -------------------------------------------------------------------- Globals

LIST_ENTRY LdrModuleList;
PKLDR_DATA_TABLE_ENTRY LdrModuleDataTableEntry;

// ---------------------------------------------------------------- Definitions

#define BLDR_IMAGE_FLAGS_NONE                 0x00000000
#define BLDR_IMAGE_FLAGS_LOAD_IMPORTS         0x00000001
#define BLDR_IMAGE_FLAGS_POST_LOAD_IMPORTS    0x00000002

// ----------------------------------------------------------------- Prototypes

NTSTATUS
BindReference (
    __in PKLDR_DATA_TABLE_ENTRY DllEntry,
    __in PKLDR_DATA_TABLE_ENTRY ImportEntry,
    __in PIMAGE_THUNK_DATA ThunkNameEntry,
    __in PIMAGE_THUNK_DATA ThunkAddressEntry,
    __in_bcount(ExportTableSize) PIMAGE_EXPORT_DIRECTORY ExportDirectory,
    __in ULONG ExportTableSize,
    __in PLIST_ENTRY LoadedImageList,
    __in BOOLEAN BindForwarder,
    __in BOOLEAN BindToHeaderImageBase
    );

BOOLEAN
CompareDllName (
    __in PUNICODE_STRING DllName1,
    __in PUNICODE_STRING DllName2
    );

NTSTATUS
LdrLoadImports (
    __in DEVICE_ID DeviceId,
    __in MEMORY_TYPE MemoryType,
    __in PKLDR_DATA_TABLE_ENTRY DataTableEntry,
    __in PLIST_ENTRY LoadedImageList,
    __in UINT32 ImageFlags,
    __in UINT32 LoadImageFlags
    );

// --------------------------------------------------------- Image Loading APIs

NTSTATUS
BlLdrAllocateDataTableEntry (
    __in PWSTR ImagePath,
    __in_opt PWSTR SymbolicName,
    __in_opt PWSTR FriendlyName,
    __in PVOID ImageBase,
    __in ULONG ImageSize,
    __in_bcount(MAX_HASH_LEN) PUCHAR ImageHash,
    __in ULONG ImageHashLength,
    __in ULONG ImageHashAlgorithm,
    __in ULONG KldrFlags,
    __in ULONG BldrFlags,
    __in ULONG FunctionFlags,
    __in_opt PULONG LoadInformation,
    __in_opt PBOOT_TRUSTED_BOOT_INFORMATION TrustedBootInformation,
    __deref_out PKLDR_DATA_TABLE_ENTRY *DataTableEntry
    )

/*++

Routine Description:

    This routine will allocate and initialize a BLDR data table entry for
    an image module.  Memory will be allocated for the entry as well as the
    unicode strings contained in the allocated structure.

    The image module can be either a PE image or other.

Arguments:

    ImagePath - Full pathname to a loaded image.

    SymbolicName - Alternative pathname for the image. If specified, the data
        table entry field, FullDllName, will be initialized to equal
        SymbolicName.

    FriendlyName - Alternate name for image.  If specified, the data table
        entry field, BaseDllName, will be initialized to equal FriendlyName.
        This is needed for certain system images (namely the kernel and hal)
        which have different versions depending on platform characteristics.

    ImageBase - Base address of the loaded image.

    ImageSize - The size of the image.

    ImageHash - Supplies a pointer to a buffer containing the image hash.

    ImageHashLength - Supplies the length of the image hash.

    ImageHashAlgorithm - Supplies the ID of the algorithm used to hash the
        image.

    KldrFlags - Supplies the kernel loader flags to use in the allocated data
        table entry.

    BldrFlags - Supplies the boot loader flags to use in the allocated data
        table entry.

    FunctionFlags - OSM_DTE_FLAGS for the function.  Possible values include:

        LOADER_DTE_FLAGS_NONPE_ENTRY : Indicates that the imaged described by
            the input parameters is not a PE image.

    LoadInformation - Supplies an optional pointer to a variable that provides
        informational flags describing various aspects of the image load.

    TrustedBootInformation - Supplies a pointer to advanced cryptographic
        information for the image.

    DataTableEntry - On output, contains a pointer to the kernel data table
        entry structure within the allocated boot data table entry.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if there is insufficient memory to perform the request.

--*/

{

    ULONG CheckSum;
    USHORT DllCharacteristics;
    PVOID EntryPoint;
    PVOID ImageHashBuffer;
    PKLDR_DATA_TABLE_ENTRY KldrImageEntry;
    PBLDR_DATA_TABLE_ENTRY LoadedImageEntry;
    PIMAGE_NT_HEADERS NtHeaders;
    ULONG SizeOfImage;
    NTSTATUS Status;
    PWCHAR TempString;

    *DataTableEntry = NULL;
    KldrImageEntry = NULL;

    //
    // If the specified image is a PE image, the data table entry can be
    // initialized with data from the NT/PE image header.  For non-PE images,
    // the entry point is assumed to be at offset zero and the size must
    // be specified by the caller.
    //

    if (CHECK_FLAG(FunctionFlags, LOADER_DTE_FLAGS_NONPE_ENTRY)) {

        ASSERT(ImageSize != 0);
        ASSERT(ImageSize < _4GB);

        SizeOfImage = (ULONG)ImageSize;
        EntryPoint = ImageBase;
        CheckSum = 0;

    } else {
        Status = BlImgGetNtHeader(ImageBase, ImageSize, &NtHeaders);

        ASSERT(NT_SUCCESS(Status));
        ASSERT(NtHeaders != NULL);

        SizeOfImage = BlImgGetSizeOfImage(NtHeaders);

        ASSERT(ImageSize == ALIGN_RANGE_UP(SizeOfImage, PAGE_SIZE));

        EntryPoint = Add2Ptr(ImageBase, BlImgGetEntryPoint(NtHeaders));
        CheckSum = BlImgGetChecksum(NtHeaders);
        DllCharacteristics = BlImgGetDllCharacteristics(NtHeaders);
        if (CHECK_FLAG(DllCharacteristics,
            IMAGE_DLLCHARACTERISTICS_FORCE_INTEGRITY) != FALSE) {

                KldrFlags |= LDRP_IMAGE_INTEGRITY_FORCED;
        }
    }

    //
    // Allocate and initialize a data table entry.
    //

    LoadedImageEntry = BlMmAllocateHeap(sizeof(BLDR_DATA_TABLE_ENTRY));
    if (LoadedImageEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BlLdrAllocateDataTableEntryEnd;
    }

    RtlZeroMemory(LoadedImageEntry, sizeof(BLDR_DATA_TABLE_ENTRY));
    KldrImageEntry = &LoadedImageEntry->KldrEntry;
    KldrImageEntry->DllBase = ImageBase;
    KldrImageEntry->SizeOfImage = SizeOfImage;
    KldrImageEntry->EntryPoint = EntryPoint;
    KldrImageEntry->CheckSum = CheckSum;
    KldrImageEntry->Flags = KldrFlags;
    KldrImageEntry->LoadCount = 1;

    //
    // Copy the trusted boot information.
    //

    if (TrustedBootInformation != NULL) {
        RtlCopyMemory(&LoadedImageEntry->CertificateIssuer,
                      &TrustedBootInformation->CertificateIssuer,
                      sizeof(UNICODE_STRING));

        RtlCopyMemory(&LoadedImageEntry->CertificatePublisher,
                      &TrustedBootInformation->CertificatePublisher,
                      sizeof(UNICODE_STRING));

        LoadedImageEntry->CertificateThumbprint =
            TrustedBootInformation->CertificateThumbprint;

        LoadedImageEntry->CertificateThumbprintLength =
            TrustedBootInformation->CertificateThumbprintLength;

        LoadedImageEntry->ThumbprintHashAlgorithm =
            TrustedBootInformation->ThumbprintHashAlgorithm;
    }

    //
    // Copy the image hash.
    //

    if (ImageHashLength > 0) {
        ImageHashBuffer = BlMmAllocateHeap(ImageHashLength);
        if (ImageHashBuffer == NULL) {
            Status = STATUS_NO_MEMORY;
            goto BlLdrAllocateDataTableEntryEnd;
        }

        RtlCopyMemory(ImageHashBuffer,
                      ImageHash,
                      ImageHashLength);

        LoadedImageEntry->ImageHash = ImageHashBuffer;
        LoadedImageEntry->ImageHashAlgorithm = ImageHashAlgorithm;
        LoadedImageEntry->ImageHashLength = ImageHashLength;
    }

    //
    // Set the load information if the argument is present and indicate if the
    // image is catalog or embedded signed in the loader table entry.
    //

    if (ARGUMENT_PRESENT(LoadInformation) != FALSE) {
        LoadedImageEntry->LoadInformation = *LoadInformation;
        if ((*LoadInformation & LOAD_INFORMATION_CATALOG_SIGNED) != 0) {
            KldrImageEntry->u1.SignatureType = SeImageSignatureCatalogNotCached;
        }

        if ((*LoadInformation & LOAD_INFORMATION_EMBEDDED_SIGNED) != 0) {
            KldrImageEntry->u1.SignatureType = SeImageSignatureEmbedded;
        }
    }

    LoadedImageEntry->Flags = BldrFlags;

    //
    // Allocate a buffer for the base DLL name (just the image name).
    //
    // If FriendlyName is not present, get a pointer to the last occurrence
    // of '\' in the full path.  This represents the file name.
    //

    if (FriendlyName == NULL) {
        FriendlyName = wcsrchr(ImagePath, L'\\');
        if (FriendlyName != NULL) {
            FriendlyName += 1;   // Advance past '\'.

        } else {
            FriendlyName = ImagePath;
        }
    }

    if (!RtlCreateUnicodeString(&KldrImageEntry->BaseDllName, FriendlyName)) {
        Status = STATUS_NO_MEMORY;
        goto BlLdrAllocateDataTableEntryEnd;
    }

    if (SymbolicName != NULL) {
        TempString = SymbolicName;

    } else {
        TempString = ImagePath;
    }

    if (!RtlCreateUnicodeString(&KldrImageEntry->FullDllName, TempString)) {
        Status = STATUS_NO_MEMORY;
        goto BlLdrAllocateDataTableEntryEnd;
    }

    Status = STATUS_SUCCESS;
    *DataTableEntry = KldrImageEntry;

BlLdrAllocateDataTableEntryEnd:
    if (!NT_SUCCESS(Status)) {
        if (KldrImageEntry != NULL) {
            BlLdrFreeDataTableEntry(KldrImageEntry);
        }
    }

    return Status;
}

PKLDR_DATA_TABLE_ENTRY
BlLdrFindDataTableEntry (
    __in PLIST_ENTRY DataTableEntryList,
    __in PCWSTR FileName,
    __in BOOLEAN ForwardSearch
    )

/*++

Routine Description:

    Traverses the provided data table entry list in search of the entry for
    the requested image.

Arguments:

    DataTableEntryList - Linked list of KLDR_DATA_TABLE_ENTRYs.

    FileName - Image name.

    ForwardSearch - Boolean value indicating if the linear search should be
        done using the list entry's forward links.

Return Value:

    A pointer to the data table entry for the specified image.

    NULL if there does not exist an entry for the image.

--*/

{

    PKLDR_DATA_TABLE_ENTRY DataTableEntry;
    UNICODE_STRING ComparePath;
    PLIST_ENTRY NextEntry;

    RtlInitUnicodeString(&ComparePath, FileName);

    //
    // Walk the specified loaded image list in the prescribed direction until
    // the requested entry is found.
    //

    if (ForwardSearch != FALSE) {
        NextEntry = DataTableEntryList->Flink;

    } else {
        NextEntry = DataTableEntryList->Blink;
    }

    while (NextEntry != DataTableEntryList) {
        DataTableEntry = CONTAINING_RECORD(NextEntry,
                                           KLDR_DATA_TABLE_ENTRY,
                                           InLoadOrderLinks);

        if (CompareDllName(&DataTableEntry->BaseDllName, &ComparePath)) {
            return DataTableEntry;
        }

        if (ForwardSearch != FALSE) {
            NextEntry = NextEntry->Flink;

        } else {
            NextEntry = NextEntry->Blink;
        }
    }

    return NULL;
}

VOID
BlLdrFreeDataTableEntry (
    __in PKLDR_DATA_TABLE_ENTRY DataTableEntry
    )

/*++

Routine Description:

    This routine will free all memory allocated for a BDLR_DATA_TABLE_ENTRY.

Arguments:

    DataTableEntry - Supplies a pointer to a data table entry for a loaded
        kernel image.

Return Value:

    None.

--*/

{

    PBLDR_DATA_TABLE_ENTRY BootDataTableEntry;

    NT_ASSERT(DataTableEntry != NULL);

    //
    // Free KLDR elements.
    //

    if ((DataTableEntry->BaseDllName.Buffer != NULL) &&
        (DataTableEntry->BaseDllName.MaximumLength != 0)) {

        BlMmFreeHeap(DataTableEntry->BaseDllName.Buffer);
    }

    if ((DataTableEntry->FullDllName.Buffer != NULL) &&
        (DataTableEntry->FullDllName.MaximumLength != 0)) {

        BlMmFreeHeap(DataTableEntry->FullDllName.Buffer);
    }

    //
    // Free BLDR elements.
    //

    BootDataTableEntry = CONTAINING_RECORD(DataTableEntry,
                                           BLDR_DATA_TABLE_ENTRY,
                                           KldrEntry);

    if ((BootDataTableEntry->CertificateThumbprint != NULL) &&
        (BootDataTableEntry->CertificateThumbprintLength != 0)) {

        BlMmFreeHeap(BootDataTableEntry->CertificateThumbprint);
    }

    if ((BootDataTableEntry->ImageHash != NULL) &&
        (BootDataTableEntry->ImageHashLength != 0)) {

        BlMmFreeHeap(BootDataTableEntry->ImageHash);
    }

    RtlFreeUnicodeString(&BootDataTableEntry->CertificateIssuer);
    RtlFreeUnicodeString(&BootDataTableEntry->CertificatePublisher);
    BlMmFreeHeap(BootDataTableEntry);
    return;
}

NTSTATUS
BlLdrBindImportReferences (
    _In_ PKLDR_DATA_TABLE_ENTRY DllEntry,
    _In_ PKLDR_DATA_TABLE_ENTRY ImportEntry,
    _In_ PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor,
    _In_ PLIST_ENTRY LoadedImageList,
    _In_ BOOLEAN BindToHeaderImageBase
)

/*++

Routine Description:

    This routine will bind all imported references from ImportEntry in
    DllEntry.

Arguments:

    DllEntry - Data table entry for the image to fixup.

    ImportEntry - Data table entry for the image containing the exports.

    ImportDescriptor - Import table entry containing all the imported
        references from ImportEntry.

    LoadedImageList - A list of currently loaded images.

    BindToHeaderImageBase - Supplies a BOOLEAN indicating if the reference
        should bind relative to the image header's image base rather than its
        current va.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_FILE if an import does not exist.

--*/

{

    PIMAGE_EXPORT_DIRECTORY ExportDirectory;
    ULONG ExportTableSize;
    PIMAGE_IMPORT_BY_NAME NameImport;
    NTSTATUS Status;
    PIMAGE_THUNK_DATA ThunkAddressEntry;
    PIMAGE_THUNK_DATA ThunkNameEntry;

    //
    // Find the export table in the image we are importing from.
    //

    ExportDirectory = (PIMAGE_EXPORT_DIRECTORY)
        RtlImageDirectoryEntryToData(ImportEntry->DllBase,
                                     TRUE,
                                     IMAGE_DIRECTORY_ENTRY_EXPORT,
                                     &ExportTableSize);

    if (ExportDirectory == NULL) {
        return STATUS_FILE_INVALID;
    }

    //
    // Get a pointer to the thunk tables for the imported references.
    //

    ThunkNameEntry = Add2Ptr(DllEntry->DllBase,
                             ImportDescriptor->OriginalFirstThunk);

    ThunkAddressEntry = Add2Ptr(DllEntry->DllBase,
                                ImportDescriptor->FirstThunk);

    //
    // Iterate through each reference, binding the reference to the actual
    // loaded address.
    //

    while (ThunkNameEntry->u1.AddressOfData != 0) {   // Import by name.
        Status = BindReference(DllEntry,
                               ImportEntry,
                               ThunkNameEntry,
                               ThunkAddressEntry,
                               ExportDirectory,
                               ExportTableSize,
                               LoadedImageList,
                               FALSE,
                               BindToHeaderImageBase);

        if (!NT_SUCCESS(Status)) {
            NameImport = Add2Ptr(DllEntry->DllBase,
                                 ThunkNameEntry->u1.AddressOfData);

            BlStatusError(BL_ERROR_IMPORT,
                          (ULONG_PTR)&NameImport->Name[0],
                          (ULONG_PTR)DllEntry->BaseDllName.Buffer,
                          Status,
                          0);

            return Status;
        }

        ThunkNameEntry += 1;
        ThunkAddressEntry += 1;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
BindReference (
    __in PKLDR_DATA_TABLE_ENTRY DllEntry,
    __in PKLDR_DATA_TABLE_ENTRY ImportEntry,
    __in PIMAGE_THUNK_DATA ThunkNameEntry,
    __in PIMAGE_THUNK_DATA ThunkAddressEntry,
    __in_bcount(ExportTableSize) PIMAGE_EXPORT_DIRECTORY ExportDirectory,
    __in ULONG ExportTableSize,
    __in PLIST_ENTRY LoadedImageList,
    __in BOOLEAN BindForwarder,
    __in BOOLEAN BindToHeaderImageBase
    )

/*++

Routine Description:

    This routine will bind an import table reference with the address in
    the loaded imported, filling out the thunk data.

Arguments:

    DllEntry - Data table entry for the image to fixup.

    ImportEntry - Data table entry for the image containing the exports.

    ThunkNameEntry - Supplies a pointer to a thunk table name entry.

    ThunkAddressEntry - Supplies a pointer to a thunk table address entry.

    ExportDirectory - Supplies a pointer to the export directory of the
        DLL loaded at ImportBase

    ExportTableSize - The size of the above export directory.

    LoadedImageList - List of loaded imaged.

    BindForwarder - Supplies a BOOLEAN indicating if the reference to bind is
        a forwarder from another image.  If it is, the import type is by name.
        In this case, ThunkNameEntry and ThunkAddressEntry are allocated and
        their data is not within the image.

    BindToHeaderImageBase - Supplies a BOOLEAN indicating if the reference
        should bind relative to the image header's image base rather than its
        current va.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_FILE_INVALID if an import does not exist.

--*/

{

    PIMAGE_IMPORT_BY_NAME AddressOfData;
    PVOID DllBase;
    PCHAR ForwarderString;
    PULONG FunctionTable;
    LONG High;
    PVOID ImportBase;
    UNICODE_STRING ImportFile;
    LONG Low;
    LONG Middle;
    PIMAGE_IMPORT_BY_NAME NameImport;
    PULONG NameTable;
    ULONG Ordinal;
    PUSHORT OrdinalTable;
    LONG Ret;
    ULONG Size;
    NTSTATUS Status;
    PKLDR_DATA_TABLE_ENTRY TargetEntry;
    PIMAGE_EXPORT_DIRECTORY TargetExportDirectory;
    ULONG TargetExportDirectorySize;
    PCHAR Temp;
    IMAGE_THUNK_DATA ThunkData;
    PIMAGE_NT_HEADERS ImportHeader;

    ImportBase = ImportEntry->DllBase;
    DllBase = DllEntry->DllBase;

    //
    // The export directory contains a table of export function names, and
    // associated with this table is an ordinal table which maps an entry
    // from the name table to the function table.  The function table has
    // the routine offset in the target DLL.
    //
    // Imports can be made by name or by ordinal with the former being the
    // most common.  When specified by name, the ordinal must be determined
    // by finding a match in the export name table described above.
    //

    if (IMAGE_SNAP_BY_ORDINAL(ThunkNameEntry->u1.Ordinal) &&
        (BindForwarder == FALSE)) {

            Ordinal = (ULONG)(IMAGE_ORDINAL(ThunkNameEntry->u1.Ordinal) -
                      ExportDirectory->Base);

    } else {
        NameImport = Add2Ptr(DllBase, ThunkNameEntry->u1.AddressOfData);
        NameTable = Add2Ptr(ImportBase, ExportDirectory->AddressOfNames);
        OrdinalTable =
            Add2Ptr(ImportBase, ExportDirectory->AddressOfNameOrdinals);

        //
        // The entries in the name table are in alphabetical order.  Perform a
        // binary search, looking for the referenced routine.
        // (But try the hint first).
        //

        Middle = NameImport->Hint;
        if ((ExportDirectory->NumberOfNames <= NameImport->Hint) ||
            (strcmp(&NameImport->Name[0],
            Add2Ptr(ImportBase, NameTable[Middle])))) {

                //
                // Either the Hint was invalid, or the hint was unsuccessful.
                // Perform a binary search, looking for the referenced function.
                //

                Low = 0;
                High = ExportDirectory->NumberOfNames - 1;
                while (Low <= High) {
                    Middle = (Low + High) / 2;
                    Ret = strcmp(&NameImport->Name[0],
                                 Add2Ptr(ImportBase, NameTable[Middle]));

                    //
                    // Adjust Middle based on return value of string comparison.
                    //

                    if (Ret < 0) {
                        High = Middle - 1;

                    } else if (Ret > 0) {
                        Low = Middle + 1;

                    } else {
                        break;
                    }
                }

                if (High < Low) {
                    return STATUS_FILE_INVALID;
                }
        }

        Ordinal = OrdinalTable[Middle];
    }

    //
    // If the ordinal found is valid, bind the reference.
    //

    if (ExportDirectory->NumberOfFunctions <= Ordinal) {
        return STATUS_FILE_INVALID;
    }

    FunctionTable = Add2Ptr(ImportBase, ExportDirectory->AddressOfFunctions);
    ThunkAddressEntry->u1.Function =
        (ULONG_PTR)Add2Ptr(ImportBase, FunctionTable[Ordinal]);

    //
    // Check to see if this actually references another image.  If the
    // function pointer is within the export directory, this is import
    // is actually from another binary.
    //

    ForwarderString = (PCHAR)ThunkAddressEntry->u1.ForwarderString;
    if (((UINTN)ExportDirectory < (UINTN)ForwarderString) &&
        ((UINTN)ForwarderString < (UINTN)Add2Ptr(ExportDirectory,
                                                 ExportTableSize))) {

            Temp = strchr(ForwarderString, '.');

            if (Temp == NULL) {
                return STATUS_FILE_INVALID;   // Malformed forwarding string.
            }

            *Temp = '\0';
            RtlInitUnicodeString(&ImportFile, NULL);
            Status = BlCopyStringToUnicodeString(&ImportFile, ForwarderString);
            *Temp = '.';
            if (!NT_SUCCESS(Status)) {
                return STATUS_NO_MEMORY;
            }

            //
            // Attempt to find the DLL referenced in the forwarder string.
            //

            TargetEntry = BlLdrFindDataTableEntry(LoadedImageList,
                                                  ImportFile.Buffer,
                                                  TRUE);

            BlMmFreeHeap(ImportFile.Buffer);
            if (TargetEntry == NULL) {

                //
                // TODO:  Need to load the DLL.  But we need to add an extension
                //        to ImportFile.
                //

                return STATUS_NOT_IMPLEMENTED;
            }

            //
            // Get a pointer to the export directory of the referenced image.
            //

            TargetExportDirectory =
                (PIMAGE_EXPORT_DIRECTORY)RtlImageDirectoryEntryToData(
                    TargetEntry->DllBase,
                    TRUE,
                    IMAGE_DIRECTORY_ENTRY_EXPORT,
                    &TargetExportDirectorySize);

            if (TargetExportDirectory == NULL) {
                return STATUS_FILE_INVALID;
            }

            Temp += 1;        // Temp = Import name.

            //
            // Create an IMAGE_THUNK_DATA structure which will specify that
            // the forwarded import should be binded against the target DLL.
            //
            // This is equivalent to an entry in an import table.  We are just
            // allocated a blob of memory and creating one manually.
            //

            Size = (ULONG)(FIELD_OFFSET(IMAGE_IMPORT_BY_NAME, Name) +
                   strlen(Temp) + 1);

            AddressOfData = BlMmAllocateHeap(Size);
            if (AddressOfData == NULL) {
                return STATUS_NO_MEMORY;
            }

            //
            // Create the Name entry.
            //

            AddressOfData->Hint = 0;
            RtlCopyMemory(&AddressOfData->Name[0], Temp, strlen(Temp) + 1);

            //
            // BindReference will add DllBase to AddressOfData.  Make sure
            // this results in a pointer to our Name entry.
            //

            ThunkData.u1.AddressOfData =
                (ULONG_PTR)AddressOfData - (ULONG_PTR)DllBase;

            //
            // Bind the reference.
            //

            Status = BindReference(DllEntry,
                                   TargetEntry,
                                   &ThunkData,
                                   &ThunkData,
                                   TargetExportDirectory,
                                   Size,
                                   LoadedImageList,
                                   TRUE,
                                   BindToHeaderImageBase);

            BlMmFreeHeap(AddressOfData);  // Free before error check.
            if (!NT_SUCCESS(Status)) {
                return Status;
            }

            ThunkAddressEntry->u1 = ThunkData.u1;

    } else if (BindToHeaderImageBase == TRUE) {

        //
        // When the loader doesn't operate in the same VA space as the image
        // being loaded, such as HvLoader and the hypervisor, the export
        // functions must be added to the import table using the eventual base
        // of the image, asspecified in the image header.
        //

        Status = BlImgGetNtHeader(ImportBase,
                                  ImportEntry->SizeOfImage,
                                  &ImportHeader);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        ThunkAddressEntry->u1.Function =
            (ULONG_PTR)Add2Ptr(BlImgGetImageBase(ImportHeader),
                               FunctionTable[Ordinal]);
    }

    return STATUS_SUCCESS;
}

BOOLEAN
CompareDllName (
    __in PUNICODE_STRING DllName1,
    __in PUNICODE_STRING DllName2
    )

/*++

Routine Description:

    This routine will determine if two strings represent the same filename.

    This routine only compares the filename (not the file extension), and
    is the reason why a string comparision can not be used.

Arguments:

    DllName1 - First filename in compare.

    DllName2 - Second filename in compare.

Return Value:

    TRUE if the two strings represent the same DLL.

    FALSE otherwise.

--*/

{
    PWCHAR String1;
    PWCHAR String2;
    PWCHAR Limit;
    ULONG Length1;
    ULONG Length2;

    ASSERT(DllName1->Buffer != NULL);
    ASSERT(DllName2->Buffer != NULL);
    ASSERT(DllName1->Length != 0);
    ASSERT(DllName2->Length != 0);

    //
    // Determine the length of each DLL name.  Only compare the names up
    // until the file extension.  Forward strings will not contain the
    // extension and this routine needs to handle this case as well.
    //

    Length1 = DllName1->Length;
    String1 = wcschr(DllName1->Buffer, L'.');
    if (String1 != NULL) {
        Length1 = (ULONG)PtrOffset(DllName1->Buffer, String1);
    }

    Length2 = DllName2->Length;
    String2 = wcschr(DllName2->Buffer, L'.');
    if (String2 != NULL) {
        Length2 = (ULONG)PtrOffset(DllName2->Buffer, String2);
    }

    //
    // Determine if the DLL names match.
    //

    if (Length1 != Length2) {
        return FALSE;
    }

    String1 = DllName1->Buffer;
    String2 = DllName2->Buffer;
    Limit = Add2Ptr(String1, Length1);
    while (String1 < Limit) {
        if (ToUpper((CHAR)*String1) != ToUpper((CHAR)*String2)) {
            return FALSE;
        }

        String1 += 1;
        String2 += 1;
    }

    return TRUE;
}

NTSTATUS
BlLdrLoadImage (
    __in DEVICE_ID DeviceId,
    __in MEMORY_TYPE MemoryType,
    __in PWSTR ImagePath,
    __in_opt PWSTR FriendlyName,
    __in_opt PUNICODE_STRING ServiceFilePath,
    __in_opt PVOID PreAllocatedImageBase,
    __in UINT32 PreAllocatedImageSize,
    __in PLIST_ENTRY LoadedImageList,
    __deref_opt_out PKLDR_DATA_TABLE_ENTRY *DataTableEntry,
    __in UINT32 KldrFlags,
    __in UINT32 BldrFlags,
    __in UINT32 ImageFlags,
    __in UINT32 LoadImageFlags,
    __out_opt PUINT32 LoadInformation
    )

/*++

Routine Description:

    This routine loads a PE image into memory.

Arguments:

    DeviceId - Supplies a handle to the device containing the image to load.

    MemoryType - Supplies the type of memory to mark the image allocation.

    ImagePath - Supplies the file path of the image to load.

    FriendlyName - Supplies an alternate name for image.  If specified, the
        data table entry's BaseDllName equals FriendlyName.  This is needed by
        the kernel since it can have any one of a few actual names.

    ServiceFilePath - Supplies an optional pointer to the file path for a
        boot driver, as described in its service registry entry.

    PreAllocatedImageBase - Supplies an optional pointer to the pre-allocated
        image buffer. This is only used when IMAGE_FLAGS_MEMORY_PRE_ALLOCATED
        is set in ImageFlags.

    PreAllocatedImageSize - Specifies the size of the pre-allocated image
        buffer. This is only used when IMAGE_FLAGS_MEMORY_PRE_ALLOCATED
        is set in ImageFlags.

    LoadedDriverList - Supplies a list of images currently loaded by the OS
        loader.

    DataTableEntry - Supplies a pointer to a variable that receives an
        allocated data table entry describing the loaded module.  The caller
        is responsible for freeing this memory.

    KldrFlags - Supplies the kernel loader flags to use in the allocated data
        table entry.

    BldrFlags - Supplies the boot loader flags to use in the allocated data
        table entry.

    ImageFlags - Supplies image flags indicating any preferences for the image
        load.  Possible values include:

        IMAGE_FLAGS_MEMORY_PRE_ALLOCATED : Indicates that memory has already
            been pre-allocated for the image and that PreAllocatedImageBase
            is the pointer to the pre-allocated memory address and
            PreAllocatedImageSize is its size.

        IMAGE_FLAGS_ALLOCATE_IN_LARGE_PAGE : Specifies that an image should
            be allocated in an exclusive system component large page.

    LoadImageFlags - Supplies image flags specifying functionality of this
        routine.  Possible values include:

        BLDR_IMAGE_FLAGS_LOAD_IMPORTS : Specifies that the image's imports
            should be loaded.  LoadedImageList is ordered by dependency.  If
            driver A contains imports from DLL B, then B is inserted before A
            in the list.

        BLDR_IMAGE_FLAGS_POST_LOAD_IMPORTS : Specifies that the image's
            imports should be loaded.  The specified image is expected to have
            already been loaded.

    LoadInformation - Supplies an optional pointer to a variable that receives
            informational flags describing various aspects of the load.  The
            following flags may be directly set by this routine.

        LOAD_INFORMATION_IMPORT_LOAD_FAILED : Indicates that the PE image
            could not be loaded because an error occurred loading one of its
            imports.

        LOAD_INFORMATION_EMBEDDED_SIGNED : Indicates that the image is verified
            using embedded signature.

        LOAD_INFORMATION_CATALOG_SIGNED : Indicates taht the image is verified
            using a signature from a catalog.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if LoadFile or ImageBase are NULL.

    STATUS_NO_MEMORY if there is insufficient memory to perform the request.

    STATUS_UNSUCCESSFUL if a filesystem could not be mounted on block i/o
        device.

    STATUS_UNSUCCESSFUL if unable to apply relocation.

    STATUS_INVALID_IMAGE_FORMAT if relocations are invalid.

    STATUS_NO_SUCH_FILE if the requested file does not exist on the specified
        block i/o device.

    STATUS_FILE_INVALID if the specified file is not a PE image.

--*/

{

    PBLDR_DATA_TABLE_ENTRY BootImageEntry;
    PWCHAR FileName;
    PVOID ImageBase;
    PUCHAR ImageHash;
    ULONG ImageHashAlgorithm;
    UCHAR ImageHashBuffer[MAX_HASH_LEN];
    ULONG ImageHashLength;
    ULONG ImageSize;
    PKLDR_DATA_TABLE_ENTRY LoadedImageEntry;
    ULONG LocalLoadInformation;
    PVOID RemoteFileBase;
    UINT64 RemoteFileSize;
    NTSTATUS Status;
    BOOT_TRUSTED_BOOT_INFORMATION TrustedBootInformation;

    ASSERT(ARGUMENT_PRESENT(ImagePath) != FALSE);

    //
    // If IMAGE_FLAGS_MEMORY_PRE_ALLOCATED flag is set, make sure
    // PreAllocatedImageBase and PreAllocatedImageSize are valid.
    //

    if (CHECK_FLAG(ImageFlags, IMAGE_FLAGS_MEMORY_PRE_ALLOCATED)) {
        if ((PreAllocatedImageBase == NULL) || (PreAllocatedImageSize == 0)) {
            return STATUS_INVALID_PARAMETER;
        }
    }

    //
    // Check to see if the Image has already been loaded.  If it has not
    // already been loaded, load it. If the Image has already been loaded,
    // only when specified, load its imports.
    //

    if (FriendlyName != NULL) {
        FileName = FriendlyName;

    } else {
        FileName = wcsrchr(ImagePath, L'\\');
        if (FileName != NULL) {
            FileName += 1;

        } else {
            FileName = ImagePath;
        }
    }

    //
    // The empty string is not a valid FileName.
    //

    if (FileName[0] == UNICODE_NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Determine if the Image has already been loaded. If it has been loaded,
    // process its post-load imports, if necessary. If it has not been loaded,
    // load the Image and process its imports.
    //

    LocalLoadInformation = 0;
    LoadedImageEntry = BlLdrFindDataTableEntry(LoadedImageList,
                                               FileName,
                                               TRUE);

    if (LoadedImageEntry == NULL) {

        //
        // If the caller specifies that the Image buffer is pre-allocated,
        // set the pre-allocate buffer address and size.
        //

        if (CHECK_FLAG(ImageFlags, IMAGE_FLAGS_MEMORY_PRE_ALLOCATED)) {
            ImageBase = PreAllocatedImageBase;
            ImageSize = PreAllocatedImageSize;

        } else {
            ImageBase = NULL;
            ImageSize = 0;
        }

        //
        // Load the specified Image into memory.  First check to see if the
        // file should be loaded over the debugger connection.  If not, load
        // the file from the specified device and file location.
        //

        Status = STATUS_UNSUCCESSFUL;
        ImageHash = ImageHashBuffer;
        ImageHashAlgorithm = 0;
        ImageHashLength = 0;
        RtlZeroMemory(&TrustedBootInformation,
                      sizeof(BOOT_TRUSTED_BOOT_INFORMATION));

        if (ARGUMENT_PRESENT(ServiceFilePath) != FALSE) {
            Status = BlBdPullRemoteFile(ServiceFilePath->Buffer,
                                        &RemoteFileBase,
                                        &RemoteFileSize);

            if (NT_SUCCESS(Status) != FALSE) {
                Status = BlImgLoadPEImageFromSourceBuffer(
                             RemoteFileBase,
                             RemoteFileSize,
                             MemoryType,
                             ImagePath,
                             &ImageBase,
                             &ImageSize,
                             ImageHash,
                             &ImageHashLength,
                             &ImageHashAlgorithm,
                             MEMORY_ATTRIBUTE_NONE,
                             0,
                             ImageFlags,
                             &LocalLoadInformation,
                             &TrustedBootInformation);

                BlMmFreePages(RemoteFileBase);
            }
        }

        if (!NT_SUCCESS(Status)) {
            Status = BlImgLoadPEImageEx(DeviceId,
                                        MemoryType,
                                        ImagePath,
                                        &ImageBase,
                                        &ImageSize,
                                        ImageHash,
                                        &ImageHashLength,
                                        &ImageHashAlgorithm,
                                        MEMORY_ATTRIBUTE_NONE,
                                        0,
                                        ImageFlags,
                                        &LocalLoadInformation,
                                        &TrustedBootInformation);

            if (!NT_SUCCESS(Status)) {
                goto Cleanup;
            }
        }

        //
        // Initialize security cookie for the image. 
        //
        // N.B. Currenty boot environment does not use GS protection, and cookie
        //      initialization is only needed when sharing OS binaries with the
        //      boot environment in order to satisfy the cookie initialization
        //      self check.
        //

#if !defined(MISC_MIN)

        LdrInitSecurityCookie(ImageBase,
                              ImageSize,
                              NULL,
                              (ULONG_PTR)BlArchGetPerformanceCounter(),
                              NULL);

#endif

        //
        // Allocate and insert a data table entry for the loaded Image.
        //

        Status = BlLdrAllocateDataTableEntry(ImagePath,
                                             NULL,
                                             FriendlyName,
                                             ImageBase,
                                             ImageSize,
                                             ImageHash,
                                             ImageHashLength,
                                             ImageHashAlgorithm,
                                             KldrFlags,
                                             BldrFlags,
                                             LOADER_DTE_FLAGS_PE_ENTRY,
                                             &LocalLoadInformation,
                                             &TrustedBootInformation,
                                             &LoadedImageEntry);

        if (!NT_SUCCESS(Status)) {
            goto Cleanup;
        }

        //
        // Insert the newly allocated data table entry into the loaded list.
        // N.B. Even when loading imports, we need to insert the entry before
        //      attempting to load any imports.  Otherwise, we could fall
        //      into a recursive loop when an import attempts to import this
        //      DLL/driver.  If this happns, we need to have an entry
        //      present in the loop to avoid this Image from being re-loaded.
        //

        InsertTailList(LoadedImageList, &LoadedImageEntry->InLoadOrderLinks);

        //
        // When specified by the caller, load all modules listed in the import
        // descriptor table.
        //

        if (CHECK_FLAG(LoadImageFlags, BLDR_IMAGE_FLAGS_LOAD_IMPORTS)) {
            Status = LdrLoadImports(DeviceId,
                                    MemoryType,
                                    LoadedImageEntry,
                                    LoadedImageList,
                                    ImageFlags,
                                    LoadImageFlags);

            if (!NT_SUCCESS(Status)) {
                RemoveEntryList(&LoadedImageEntry->InLoadOrderLinks);
                BlLdrFreeDataTableEntry(LoadedImageEntry);
                SET_FLAGS(LocalLoadInformation,
                          LOAD_INFORMATION_IMPORT_LOAD_FAILED);

                goto Cleanup;
            }

            //
            // If this is an imported DLL, insert the data table entry
            // after all imports.
            //

            if (CHECK_FLAG(KldrFlags, LDRP_DRIVER_DEPENDENT_DLL)) {
                RemoveEntryList(&LoadedImageEntry->InLoadOrderLinks);
                InsertTailList(LoadedImageList,
                               &LoadedImageEntry->InLoadOrderLinks);
            }
        }

        //
        // Mark the Image as processed.
        //

        LoadedImageEntry->Flags |= LDRP_ENTRY_PROCESSED;

    } else {

        if (CHECK_FLAG(LoadImageFlags, BLDR_IMAGE_FLAGS_POST_LOAD_IMPORTS)) {
            CLEAR_FLAGS(LoadImageFlags, BLDR_IMAGE_FLAGS_POST_LOAD_IMPORTS);
            Status = LdrLoadImports(DeviceId,
                                    MemoryType,
                                    LoadedImageEntry,
                                    LoadedImageList,
                                    ImageFlags,
                                    LoadImageFlags);

            if (!NT_SUCCESS(Status)) {
                goto Cleanup;
            }
        }

        //
        // A boot driver does not get the dependent dll flag set.  If this
        // routine was not explicitly called with the flag set, then clear the
        // flag if previously set.  When the routine is called without the flag
        // set, it is being called on a boot driver.
        //

        if (((KldrFlags & LoadedImageEntry->Flags) & LDRP_DRIVER_DEPENDENT_DLL) == 0) {
            CLEAR_FLAGS(KldrFlags, LDRP_DRIVER_DEPENDENT_DLL);
            CLEAR_FLAGS(LoadedImageEntry->Flags, LDRP_DRIVER_DEPENDENT_DLL);
        }

        //
        // Update the reference count for the binary and if the Image is a
        // driver that was previously imported, clear the DLL flag.
        //

        BootImageEntry = (PBLDR_DATA_TABLE_ENTRY)LoadedImageEntry;
        BootImageEntry->Flags |= BldrFlags;
        LoadedImageEntry->Flags |= KldrFlags;
        LoadedImageEntry->LoadCount += 1;
    }

    //
    // Ensure that the I and D caches are consistent prior to
    // executing code from the loaded module.
    //

    BlArchSweepIcacheRange(LoadedImageEntry->DllBase,
                           LoadedImageEntry->SizeOfImage);

    //
    // Return the pointer to the loaded data table entry.
    //

    if (DataTableEntry != NULL) {
        *DataTableEntry = LoadedImageEntry;
    }

    Status = STATUS_SUCCESS;

Cleanup:
    if (ARGUMENT_PRESENT(LoadInformation)) {
        *LoadInformation = LocalLoadInformation;
    }

    return Status;
}

NTSTATUS
BlLdrLoadImageEx (
    __in PUNICODE_STRING ImageDirectoryPath,
    __in PWSTR ImageName,
    __in_opt PUNICODE_STRING ServiceFilePath,
    __in DEVICE_ID DeviceId,
    __in MEMORY_TYPE MemoryType,
    __in PLIST_ENTRY LoadedImageList,
    __deref_out PKLDR_DATA_TABLE_ENTRY *DataTableEntry,
    __in UINT32 KldrFlags,
    __in UINT32 BldrFlags,
    __in UINT32 ImageFlags,
    __out_opt PUINT32 LoadInformation,
    __in UINT32 LoadImageFlags
    )

/*++

Routine Description:

    This routine loads a PE image into memory.

Arguments:

    ImageDirectoryPath - Supplies the directory path for the image to load.
        The memory allocated for the unicode string may change.  The caller is
        responsible for freeing this memory.

    ImageName - Supplies the name of the image to load.

    ServiceFilePath - Supplies an optional pointer to the file path for a
        boot driver, as described in its service registry entry.

    DeviceId - Supplies a handle to the device containing the image to load.

    MemoryType - Supplies the type of memory to mark the image allocation.

    LoadedImageList - Supplies a list of images currently loaded by the OS
        loader.

    DataTableEntry - Supplies a pointer to a variable that receives an
        allocated data table entry describing the loaded module.  The caller is
        responsible for freeing this memory.

    KldrFlags - Supplies the kernel loader flags to use in the allocated data
        table entry.

    BldrFlags - Supplies the boot loader flags to use in the allocated data
        table entry.

    ImageFlags - Supplies boot library image loading flags.  Flags include :

        IMAGE_FLAGS_ALLOCATE_IN_LARGE_PAGE : Specifies that an image should
            be allocated in an exclusive system component large page.

    LoadInformation - Supplies an optional pointer to a variable that receives
        informational flags describing various aspects of the load.

    LoadImageFlags - Supplies image flags specifying functionality of this
        routine.  Possible values include :

        BLDR_IMAGE_FLAGS_LOAD_IMPORTS : Specifies that the image's imports
            should be loaded.  LoadedImageList is ordered by dependency.  If
            driver A contains imports from DLL B, then B is inserted before A
            in the list.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if there is insufficient memory to perform the request.

    STATUS_UNSUCCESSFUL if unable to apply relocations to an imported DLL.

    STATUS_INVALID_IMAGE_FORMAT if relocations are invalid.

    STATUS_NO_SUCH_FILE if the requested file does not exist on the specified
        block i/o device.

    STATUS_FILE_INVALID if the specified file is not a PE image.

--*/

{

    UNICODE_STRING ImagePath;
    NTSTATUS Status;

    RtlInitUnicodeString(&ImagePath, NULL);

    //
    // Build a full path for the specified DLL, consisting of the directory
    // path plus the imported file name.  Take care to calculate the correct
    // size up front so that we only have to call the allocator one time.
    //

    ImagePath.MaximumLength =
        ImageDirectoryPath->Length +
            (USHORT)(wcslen(ImageName) + 1) * sizeof(WCHAR);

    ImagePath.Buffer = BlMmAllocateHeap(ImagePath.MaximumLength);
    if (ImagePath.Buffer == NULL) {
        Status = STATUS_NO_MEMORY;
        goto Exit;
    }

    if (BlAppendUnicodeToString(&ImagePath,
                                ImageDirectoryPath->Buffer) == FALSE) {

        Status = STATUS_NO_MEMORY;
        goto Exit;
    }


    if (BlAppendUnicodeToString(&ImagePath, ImageName) == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto Exit;
    }

    //
    // Load the specified image and all of its imports.
    //

    Status = BlLdrLoadImage(DeviceId,
                            MemoryType,
                            ImagePath.Buffer,
                            NULL,
                            ServiceFilePath,
                            NULL,
                            0,
                            LoadedImageList,
                            DataTableEntry,
                            KldrFlags,
                            BldrFlags,
                            ImageFlags,
                            LoadImageFlags,
                            LoadInformation);

Exit:
    RtlFreeUnicodeString(&ImagePath);
    return Status;
}

VOID
BlLdrUnloadImage (
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

    RemoveEntryList(&DataTableEntry->InLoadOrderLinks);
    BlImgUnLoadImage(DataTableEntry->DllBase,
                     DataTableEntry->SizeOfImage,
                     DataTableEntry->Flags);

    BlLdrFreeDataTableEntry(DataTableEntry);
    return;
}
    
NTSTATUS
LdrLoadImports (
    __in DEVICE_ID DeviceId,
    __in MEMORY_TYPE MemoryType,
    __in PKLDR_DATA_TABLE_ENTRY DataTableEntry,
    __in PLIST_ENTRY LoadedImageList,
    __in UINT32 ImageFlags,
    __in UINT32 LoadImageFlags
    )

/*++

Routine Description:

    Loads each module specified in the import table of the provided image.

Arguments:

    DeviceId - The device on which the the loaded image is located.

    MemoryType - The type of memory to mark allocations made for the imported
        modules.

    DataTableEntry - The data table entry for the image whose imports should
        be loaded.

    LoadedImageList - The list of images already loaded.

    ImageFlags - Special image flags indicating any preferences for the
        image load.

    LoadImageFlags - Supplies image flags specifying functionality of this
        routine.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if there is insufficient memory to perform the request.
    STATUS_UNSUCCESSFUL if unable to apply relocations to an imported DLL.
    STATUS_INVALID_IMAGE_FORMAT if relocations are invalid.
    STATUS_NO_SUCH_FILE if the requested file does not exist on the specified
        block i/o device.
    STATUS_FILE_INVALID if the specified file is not a PE image.

--*/

{

    UNICODE_STRING DirectoryPath;
    PWCHAR FullPath;
    UNICODE_STRING ImageName;
    PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor;
    PCHAR ImportName;
    ULONG ImportTableSize;
    PKLDR_DATA_TABLE_ENTRY ImportedImageDataTableEntry;
    NTSTATUS Status;
    PWCHAR TempString;

    FullPath = NULL;
    Status = STATUS_SUCCESS;
    RtlInitUnicodeString(&DirectoryPath, NULL);
    RtlInitUnicodeString(&ImageName, NULL);

    //
    // Locate the Import table in the loaded image.  If NULL, then the binary
    // does not have any dependencies.
    //

    ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)
        RtlImageDirectoryEntryToData(DataTableEntry->DllBase,
                                     TRUE,
                                     IMAGE_DIRECTORY_ENTRY_IMPORT,
                                     &ImportTableSize);

    if (ImportDescriptor == NULL) {
        goto Cleanup;
    }

    //
    // Create the directory path from the Full (symbolic) path name in the
    // data table entry.
    //

    TempString = DataTableEntry->FullDllName.Buffer;
    if (RtlCreateUnicodeString(&DirectoryPath, TempString) == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto Cleanup;
    }

    TempString  = wcsrchr(DirectoryPath.Buffer, L'\\') + 1;
    *TempString = L'\0';
    DirectoryPath.Length = (USHORT)PtrOffset(DirectoryPath.Buffer, TempString);

    //
    // Recursively load Imports.
    //

    LoadImageFlags |= BLDR_IMAGE_FLAGS_LOAD_IMPORTS;

    //
    // Walk the Import table, loading each specified DLL.
    //

    while ((ImportDescriptor->Name != 0) &&
           (ImportDescriptor->OriginalFirstThunk != 0) &&
           (ImportTableSize != 0)) {

        //
        // Create the Full image path to the referenced DLL.
        //

        ImportName = Add2Ptr(DataTableEntry->DllBase, ImportDescriptor->Name);
        Status = BlCopyStringToUnicodeString(&ImageName, ImportName);
        if (!NT_SUCCESS(Status)) {
            break;
        }

        //
        // For all non-self referencing Imports, load the binary containing the
        // Imports and bind their references in the current image.
        //

        if (RtlEqualUnicodeString(&DataTableEntry->BaseDllName,
                                  &ImageName,
                                  TRUE) == FALSE) {

            Status = BlLdrLoadImageEx(&DirectoryPath,
                                      ImageName.Buffer,
                                      NULL,
                                      DeviceId,
                                      MemoryType,
                                      LoadedImageList,
                                      &ImportedImageDataTableEntry,
                                      LDRP_DRIVER_DEPENDENT_DLL,
                                      0,
                                      ImageFlags,
                                      NULL,
                                      LoadImageFlags);

            if (!NT_SUCCESS(Status)) {
                goto Cleanup;
            }

            Status = BlLdrBindImportReferences(DataTableEntry,
                                               ImportedImageDataTableEntry,
                                               ImportDescriptor,
                                               LoadedImageList,
                                               TRUE);

            if (!NT_SUCCESS(Status)) {
                break;
            }
        }

        ImportDescriptor += 1;
        ImportTableSize -= sizeof(IMAGE_IMPORT_DESCRIPTOR);
    }

Cleanup:
    if (FullPath != NULL) {
        BlMmFreeHeap(FullPath);
    }

    RtlFreeUnicodeString(&ImageName);
    RtlFreeUnicodeString(&DirectoryPath);
    return Status;
}

NTSTATUS
BlLdrLoadDll (
    __in DEVICE_ID DeviceId,
    __in PWSTR ImagePath,
    __in ULONG ImageFlags,
    __out_opt PVOID *DllBase
    )

/*++

Routine Description:

    Loads a module for use by boot application and resolves its imports.

Arguments:

    DeviceId - The device on which the the loaded image is located.

    ImagePath - Path to the image to load.

    ImageFlags - Special image flags indicating any preferences for the
        image load.

    DllBase - Receives base address of the loaded image.

Return Value:

    NTSTATUS.

--*/

{

    PKLDR_DATA_TABLE_ENTRY DataTableEntry;
    UNICODE_STRING ImageName;
    NTSTATUS Status;

    //
    // Load image and resolve imports.
    //

    ImageFlags |= IMAGE_FLAGS_VALIDATE_MACHINE_TYPE;
    Status = BlLdrLoadImage(DeviceId,
                            MEMORY_TYPE_BOOT_APPLICATION,
                            ImagePath,
                            NULL,
                            NULL,
                            NULL,
                            0,
                            &LdrModuleList,
                            &DataTableEntry,
                            0,
                            0,
                            ImageFlags,
                            BLDR_IMAGE_FLAGS_LOAD_IMPORTS,
                            NULL);

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    //
    // Send image load notification to the debugger.
    //

    RtlInitUnicodeString(&ImageName, ImagePath);
    BlBdLoadImageSymbols(&ImageName, DataTableEntry->DllBase);

    if (ARGUMENT_PRESENT(DllBase)) {
        *DllBase = DataTableEntry->DllBase;
    }

Exit:
    return Status;
}

NTSTATUS
BlpLdrModuleNameFromImage (
    __in_bcount(ImageSize) PVOID ImageBase,
    __in ULONG ImageSize,
    __out PSTRING ModuleName
    )

/*++

Routine Description:

    This routine extracts the module name from a loaded image.

Arguments:

    ImageBase - Supplies the address of the loaded image in memory.

    ImageSize - Supplies the size (in bytes) of the image in memory.

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

    PSTR Extension;
    NTSTATUS Status;

    Status = BlImgGetModuleName(ImageBase, ImageSize, ModuleName);
    if (!NT_SUCCESS(Status)) {
        goto ModuleNameFromImageEnd;
    }

    //
    // Strip the extension out of the name by searching backwards for a '.'.
    //

    Extension = strrchr(ModuleName->Buffer, '.');
    if (Extension != NULL) {
        *Extension = '\0';
        ModuleName->Length = (USHORT)PtrOffset(ModuleName->Buffer, Extension);
    }

ModuleNameFromImageEnd:
    if (!NT_SUCCESS(Status)) {
        if (ModuleName->Buffer != NULL) {
            BlMmFreeHeap(ModuleName->Buffer);
        }
    }

    return Status;
}

NTSTATUS
BlpLdrPopulateDataTableEntry (
    __out PKLDR_DATA_TABLE_ENTRY *TableEntry
    )

/*++

Routine Description:

    This routine populates a data table entry with data about the currently
    executing application.

Arguments:

    TableEntry - Supplies the data table entry to populate.

Return Value:

    STATUS_SUCCESS if the data table entry is populated successfully.

    STATUS_NO_MEMORY if there is a memory allocation failure.

--*/

{

    PWCHAR AppPath;
    PVOID ImageBase;
    ULONG ImageSize;
    UINTN Length;
    STRING ModuleName;
    NTSTATUS Status;
    BOOLEAN StringAllocated;

    AppPath = NULL;

    //
    // Get base address and size for the loaded image.
    //

    NT_VERIFY(
        NT_SUCCESS(
            BlGetApplicationBaseAndSize(&ImageBase, &ImageSize)));

    //
    // Construct the full path to the application and the base file name.  Get
    // this from the application path if it exists.  Otherwise, crack the image
    // and extract it.  This does not provide the full path, only the file name.
    //

    Status = BlGetApplicationOptionString(BCDE_LIBRARY_TYPE_APPLICATION_PATH,
                                          &AppPath);

    if (!NT_SUCCESS(Status)) {

        //
        // The application path option does not exist.  Get the module name from
        // the image.  If this does not exist, just use a default name.
        //

        ModuleName.Buffer = NULL;
        Status = BlpLdrModuleNameFromImage(ImageBase, ImageSize, &ModuleName);
        if (NT_SUCCESS(Status)) {
            StringAllocated = TRUE;

        } else {
            ModuleName.Buffer = "Boot Application";
            ModuleName.Length = (USHORT)strlen("Boot Application");
            ModuleName.MaximumLength = ModuleName.Length + 1;
            StringAllocated = FALSE;
        }

        //
        // The image module name is a single-byte character string.  Convert it
        // to a unicode string.
        //

        Length = (ModuleName.Length + 1) * sizeof(WCHAR);
        AppPath = BlMmAllocateHeap(Length);
        if (AppPath != NULL) {
            BlCopyStringToWcharString(AppPath, ModuleName.Buffer);
        }

        if ((ModuleName.Buffer != NULL) && (StringAllocated != FALSE)) {
            BlMmFreeHeap(ModuleName.Buffer);
        }

        if (AppPath == NULL) {
            Status = STATUS_NO_MEMORY;
            goto PopulateDataTableEntryEnd;
        }
    }

    Status = BlLdrAllocateDataTableEntry(AppPath,
                                         NULL,
                                         NULL,
                                         ImageBase,
                                         ImageSize,
                                         NULL,
                                         0,
                                         0,
                                         0,
                                         0,
                                         0,
                                         NULL,
                                         NULL,
                                         TableEntry);

PopulateDataTableEntryEnd:
    if (AppPath != NULL) {
        BlMmFreeHeap(AppPath);
    }

    return Status;
}

NTSTATUS
BlLdrInitialize (
    VOID
    )

/*++

Routine Description:

    This routine populates a data table entry with data about the currently
    executing application.

Arguments:

    TableEntry - Supplies the data table entry to populate.

Return Value:

    STATUS_SUCCESS if the data table entry is populated successfully.

    STATUS_NO_MEMORY if there is a memory allocation failure.

--*/

{

    NTSTATUS Status;

    InitializeListHead(&LdrModuleList);

    //
    // Create entry for the current application.
    //

    Status = BlpLdrPopulateDataTableEntry(&LdrModuleDataTableEntry);
    if (!NT_SUCCESS(Status)) {
        goto LdrInitializeEnd;
    }

    InsertTailList(&LdrModuleList, &LdrModuleDataTableEntry->InLoadOrderLinks);

LdrInitializeEnd:
    return Status;
}

VOID
BlLdrDestroy (
    VOID
    )

/*++

Routine Description:

    This routine populates a data table entry with data about the currently
    executing application.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PLIST_ENTRY Entry;
    PKLDR_DATA_TABLE_ENTRY DataTableEntry;

    while (!IsListEmpty(&LdrModuleList)) {
        Entry = RemoveHeadList(&LdrModuleList);
        DataTableEntry = CONTAINING_RECORD(Entry, KLDR_DATA_TABLE_ENTRY, InLoadOrderLinks);
        BlLdrFreeDataTableEntry(DataTableEntry);
    }

    return;
}


