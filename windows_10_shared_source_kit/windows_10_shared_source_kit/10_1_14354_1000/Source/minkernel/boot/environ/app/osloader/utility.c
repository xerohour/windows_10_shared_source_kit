/*++

Copyright (c) Microsoft Corporation

Module Name:

    utility.c

Abstract:

    Public utilty routines for the OS Loader.

Environment:

    Boot Environment.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"

NTSTATUS
OslMergeAcpiTables (
    __inout PVOID *AcpiMergeTable,
    __inout ULONG *AcpiMergeTableSize,
    __in PVOID AcpiTable,
    __in ULONG AcpiTableSize
    )

/*++

Routine Description:

    This routine merges two ACPI tables together.  It also attempts to unmap
    and free the original allocations, however any error that occurs while
    attempting to do so will be ignored.

    N.B. The original ACPI tables are assumed to be page allocated.

Arguments:

    AcpiMergeTable - Supplies a pointer to an ACPI table to merge, and
        returns the newly merged table's address.

    AcpiMergeTableSize - Supplies the size of the ACPI table to merge, and
        returns the newly merged table's size (in bytes).

    AcpiTable - Supplies a pointer to a second ACPI table to merge.

    AcpiTableSize - Supplies the size of the second ACPI table to merge.

Return Value:

    STATUS_SUCCESS if the operation succeeds, otherwise an error status.

--*/

{

    PVOID NewAcpiTable;
    ULONG NewAcpiTableSize;
    ULONG PageCount;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    BOOLEAN UnmapVa;

    //
    // Allocate and map a new ACPI table big enough for both tables, then
    // copy each of the tables in, one right after the other.
    //

    NewAcpiTableSize = AcpiTableSize;
    NewAcpiTableSize += *AcpiMergeTableSize;
    PageCount = ALIGN_RANGE_UP(NewAcpiTableSize, PAGE_SIZE) >> PAGE_SHIFT;
    Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                       PageCount,
                                       OSL_MEMORY_TYPE_ACPI_TABLE,
                                       MEMORY_ATTRIBUTE_NONE,
                                       0);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Status = BlMmMapPhysicalAddress(&NewAcpiTable,
                                    PhysicalAddress,
                                    NewAcpiTableSize,
                                    MEMORY_ATTRIBUTE_NONE);

    if (!NT_SUCCESS(Status)) {
        BlMmFreePhysicalPages(PhysicalAddress);
        return Status;
    }

    RtlCopyMemory(NewAcpiTable, *AcpiMergeTable, *AcpiMergeTableSize);
    RtlCopyMemory(Add2Ptr(NewAcpiTable, *AcpiMergeTableSize),
                  AcpiTable,
                  AcpiTableSize);

    //
    // Unmap and free the first ACPI table, and set its return
    // value.
    //

    UnmapVa = BlMmTranslateVirtualAddress(*AcpiMergeTable,
                                          &PhysicalAddress);

    if (UnmapVa != FALSE) {
        BlMmUnmapVirtualAddress(*AcpiMergeTable,
                                *AcpiMergeTableSize);

        BlMmFreePhysicalPages(PhysicalAddress);
    }

    *AcpiMergeTable = NewAcpiTable;
    *AcpiMergeTableSize = NewAcpiTableSize;

    //
    // Unmap and free the second ACPI table.
    //

    UnmapVa = BlMmTranslateVirtualAddress(AcpiTable,
                                          &PhysicalAddress);

    if (UnmapVa != FALSE) {
        BlMmUnmapVirtualAddress(AcpiTable, AcpiTableSize);
        BlMmFreePhysicalPages(PhysicalAddress);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
OslUpdateBootOptions (
    __in PCWSTR BootOptions
    )

/*++

Routine Description:

    This routine updates the boot options for the application.  It will update
    the OslLoadOptions global variable as well as updating the boot entry
    options for the application.  After updating the boot options, it
    reinitializes the library so that the application can use the new flags.

Arguments:

    BootOptions - Supplies a string containing the new boot options.

Return Value:

    NTSTATUS code indicating outcome.

--*/

{

    ULONG Length;
    LIBRARY_PARAMETERS LibraryParameters;
    PWSTR LocalBootOptions;
    PCHAR NewLoadOptions;
    PBOOT_ENTRY_OPTION OptionBuffer;
    ULONG OptionBufferSize;
    PWCHAR OptionString;
    NTSTATUS Status;

    LocalBootOptions = NULL;
    NewLoadOptions = NULL;
    OptionBuffer = NULL;
    OptionString = NULL;

    //
    // Create an option list from the original load option string.
    //

    if (OslLoadOptions != NULL) {
        OptionString = BlMmAllocateHeap((strlen(OslLoadOptions) + 1) *
                                        sizeof(WCHAR));

        if (OptionString == NULL) {
            Status = STATUS_NO_MEMORY;
            goto UpdateBootOptionsEnd;
        }

        BlCopyStringToWcharString(OptionString, OslLoadOptions);
        Status = AhCreateLoadOptionsList(OptionString,
                                         NULL,
                                         0,
                                         NULL,
                                         NULL,
                                         &OptionBufferSize);

        if (Status != STATUS_BUFFER_TOO_SMALL) {
            Status = STATUS_UNSUCCESSFUL;
            goto UpdateBootOptionsEnd;
        }

        OptionBuffer = BlMmAllocateHeap(OptionBufferSize);
        if (OptionBuffer == NULL) {
            Status = STATUS_NO_MEMORY;
            goto UpdateBootOptionsEnd;
        }

        Status = AhCreateLoadOptionsList(OptionString,
                                         OptionBuffer,
                                         OptionBufferSize,
                                         NULL,
                                         NULL,
                                         &Length);

        if (!NT_SUCCESS(Status)) {
            goto UpdateBootOptionsEnd;
        }

        ASSERT(OptionBufferSize == Length);

        //
        // Remove all options corresponding to the original load options
        // string.
        //

        Status = BlRemoveApplicationOptions(OptionBuffer);
        if (!NT_SUCCESS(Status)) {
            goto UpdateBootOptionsEnd;
        }

        BlMmFreeHeap(OptionBuffer);
        OptionBuffer = NULL;
    }

    //
    // Create a new global boot options string.  A copy of the input boot
    // options must be made because the boot options string may be modified by
    // the routine, and the input is a constant string.
    //

    Length = (ULONG)(wcslen(BootOptions) + 1);
    LocalBootOptions = BlMmAllocateHeap(Length * sizeof(WCHAR));
    if (LocalBootOptions == NULL) {
        Status = STATUS_NO_MEMORY;
        goto UpdateBootOptionsEnd;
    }

    NewLoadOptions = BlMmAllocateHeap(Length * sizeof(CHAR));
    if (NewLoadOptions == NULL) {
        Status = STATUS_NO_MEMORY;
        goto UpdateBootOptionsEnd;
    }

    RtlCopyMemory(LocalBootOptions, BootOptions, Length * sizeof(WCHAR));
    BlCopyWcharStringToString(NewLoadOptions, LocalBootOptions);

    //
    // Build up the new boot environment-specific boot entry options.  This
    // process requires two passes so that the correctly sized buffer may be
    // allocated.
    //

    Status = AhCreateLoadOptionsList(LocalBootOptions,
                                     NULL,
                                     0,
                                     NULL,
                                     NULL,
                                     &OptionBufferSize);

    if (Status != STATUS_BUFFER_TOO_SMALL) {
        Status = STATUS_UNSUCCESSFUL;
        goto UpdateBootOptionsEnd;
    }

    OptionBuffer = BlMmAllocateHeap(OptionBufferSize);
    if (OptionBuffer == NULL) {
        Status = STATUS_NO_MEMORY;
        goto UpdateBootOptionsEnd;
    }

    Status = AhCreateLoadOptionsList(LocalBootOptions,
                                     OptionBuffer,
                                     OptionBufferSize,
                                     NULL,
                                     NULL,
                                     &Length);

    if (!NT_SUCCESS(Status)) {
        goto UpdateBootOptionsEnd;
    }

    ASSERT(OptionBufferSize == Length);

    //
    // Update the new options in the loader's boot option list.
    //

    Status = BlUpdateApplicationOptions(OptionBuffer);
    if (!NT_SUCCESS(Status)) {
        goto UpdateBootOptionsEnd;
    }

    //
    // Free the old loader option string, and update the global pointer
    // to its new value.
    //

    if (OslLoadOptions != NULL) {
        BlMmFreeHeap(OslLoadOptions);
    }

    OslLoadOptions = NewLoadOptions;
    NewLoadOptions = NULL;

    //
    // Reinitialize the library now that the application options have been
    // successfully updated.
    //

    LibraryParameters = BlGetCurrentLibraryParameters();
    LibraryParameters.Flags |= LF_REINITIALIZE;
    Status = BlInitializeLibrary(BlGetApplicationParameters(),
                                 &LibraryParameters);

UpdateBootOptionsEnd:
    if (NewLoadOptions != NULL) {
        BlMmFreeHeap(NewLoadOptions);
    }

    if (OptionString != NULL) {
        BlMmFreeHeap(OptionString);
    }

    if (LocalBootOptions != NULL) {
        BlMmFreeHeap(LocalBootOptions);
    }

    if (OptionBuffer != NULL) {
        BlMmFreeHeap(OptionBuffer);
    }

    return Status;
}

NTSTATUS
OslFilterLoadOptions (
    __in PCWSTR LoadOptions,
    __deref_opt_out PWSTR *FilteredLoadOptions
    )

/*++

Routine Description:

    This routine filters a string containing load options against any active
    Secure Boot policy.

Arguments:

    LoadOptions - Supplies a string containing the load options to filter.

    FilteredLoadOptions - Receives a string containing the filtered load
         options.

Return Value:

    NTSTATUS code indicating outcome.

--*/

{

    ULONG Length;
    PBOOT_ENTRY_OPTION OptionBuffer;
    ULONG OptionBufferSize;
    PSTR OptionString;
    NTSTATUS Status;

    OptionBuffer = NULL;
    OptionString = NULL;

    //
    // Create a boot option list from the input string.
    //

    Status = AhCreateLoadOptionsList((PWSTR)LoadOptions,
                                     NULL,
                                     0,
                                     NULL,
                                     NULL,
                                     &OptionBufferSize);

    if (Status != STATUS_BUFFER_TOO_SMALL) {
        Status = STATUS_UNSUCCESSFUL;
        goto FilterBootOptionsEnd;
    }

    OptionBuffer = BlMmAllocateHeap(OptionBufferSize);
    if (OptionBuffer == NULL) {
        Status = STATUS_NO_MEMORY;
        goto FilterBootOptionsEnd;
    }

    Status = AhCreateLoadOptionsList((PWSTR)LoadOptions,
                                     OptionBuffer,
                                     OptionBufferSize,
                                     NULL,
                                     NULL,
                                     &Length);

    if (!NT_SUCCESS(Status)) {
        goto FilterBootOptionsEnd;
    }

    //
    // Remove the LoadOptions string which was also added by
    // AhCreateLoadOptionsList, as that is the input string which we are
    // trying to filter out.
    //

    BlRemoveBootOption(OptionBuffer, BCDE_LIBRARY_TYPE_LOAD_OPTIONS_STRING);

    //
    // Convert the options back to a string. This will be filtered.
    //

    Status = AhCreateLoadOptionsString(OptionBuffer, &OptionString);
    if (!NT_SUCCESS(Status)) {
        goto FilterBootOptionsEnd;
    }

    //
    // If there are no options, the string will be null.
    //

    if (OptionString == NULL) {
        *FilteredLoadOptions = NULL;
        goto FilterBootOptionsEnd;
    }

    //
    // Copy the string back to Unicode.
    //

    Length = (ULONG)strlen(OptionString) + 1;

    *FilteredLoadOptions = (PWSTR)BlMmAllocateHeap(Length * sizeof(WCHAR));
    if (*FilteredLoadOptions == NULL) {
        Status = STATUS_NO_MEMORY;
        goto FilterBootOptionsEnd;
    }

    BlCopyStringToWcharString(*FilteredLoadOptions, OptionString);

FilterBootOptionsEnd:
    if (OptionString != NULL) {
        BlMmFreeHeap(OptionString);
    }

    if (OptionBuffer != NULL) {
        BlMmFreeHeap(OptionBuffer);
    }

    return Status;
}

NTSTATUS
OslCheckForInvalidBootOptions (
    __in PCWSTR BootOptions,
    __in BOOLEAN CheckForSecureBootOptions
    )

/*++

Routine Description:

    This routine checks if a string contains any boot options that are invalid.

Arguments:

    BootOptions - Supplies a string containing the boot options to check.

    CheckForSecureBootOptions - Supplies FALSE if the options protected by
         Secure Boot policy should not be checked for.

Return Value:

    STATUS_SUCCESS if the string contains no invalid boot options.

    STATUS_INVALID_PARAMETER if an invalid option is found.

    STATUS_NO_MEMORY if memory could not be allocated.

--*/

{

    SIZE_T Length;
    NTSTATUS Status;
    PSTR StringCopy;

    //
    // Make an ANSI copy of the string before searching within it.
    //
    // N.B. LoadOptions passed down to the OS in an ANSI string constructed
    //      by casting Unicode characters to Ansi characters. To avoid possible
    //      attacks taking advantage of this down cast the comparison is done
    //      using Ansi version of the string.
    //
    // N.B. String size is capped to USHORT, making overflow below not possible.
    //

    Length = wcslen(BootOptions);
    if (Length > (USHORT_MAX / sizeof(WCHAR) - 1)) {
        return STATUS_INVALID_PARAMETER;
    }

    Length += 1;
    StringCopy = BlMmAllocateHeap(Length * sizeof(CHAR));
    if (StringCopy == NULL) {
        Status = STATUS_NO_MEMORY;
        goto CleanExit;
    }

    BlCopyWcharStringToString(StringCopy, (PWSTR)BootOptions);
 
    _strupr(StringCopy);
    if (strstr(StringCopy, "DISABLE_INTEGRITY_CHECKS") != NULL ||
        strstr(StringCopy, "DISABLEELAMDRIVERS") != NULL ||
        strstr(StringCopy, "NOINTEGRITYCHECKS") != NULL) {

        Status = STATUS_INVALID_PARAMETER;
        goto CleanExit;
    }

    if (CheckForSecureBootOptions == FALSE) {
        Status = STATUS_SUCCESS;
        goto CleanExit;
    }

    if (strstr(StringCopy, "TESTSIGNING") != NULL ||
        strstr(StringCopy, "DEBUG") != NULL ||
        strstr(StringCopy, "EXECUTE") != NULL ||
        strstr(StringCopy, "FLIGHTSIGNING") != NULL ) {

        Status = STATUS_INVALID_PARAMETER;
        goto CleanExit;
    }

    Status = STATUS_SUCCESS;

CleanExit:
    if (StringCopy != NULL) {
        BlMmFreeHeap(StringCopy);
    }

    return Status;
}

BOOLEAN
OslIsUnicodeStringNullTerminated (
    __in PUNICODE_STRING String
    )

/*++

Routine Description:

    This routine tests whether a UNICODE_STRING is null-terminated.
    Calling it on an invalid string may result in an access violation.
    This routine is meant to be used only within assertions.

Arguments:

    String - Provides the string to test.

Return Value:

    TRUE if the string's fields make it a null-terminated UNICODE_STRING.

    FALSE otherwise.

--*/

{

    return ((String->Length < String->MaximumLength) &&
            (String->Buffer[String->Length / sizeof(WCHAR)] == UNICODE_NULL));
}

NTSTATUS
OslZeroPhysicalPages (
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in ULONG PageCount
    )

/*++

Routine Description:

    This routine is uses to zero memory at supplied physical address.

Arguments:

    PhysicalAddress - Supplies starting physical address for memory to zero.

    PageCount - Supplies number of pages to zero.

Return Value:

    NTSTATUS code.

--*/

{

    SIZE_T Size;
    NTSTATUS Status;
    PVOID VirtualAddress;

    //
    // Map the pages, zero and unmap.
    //

    Size = PageCount * PAGE_SIZE;
    Status = BlMmMapPhysicalAddress(&VirtualAddress,
                                    PhysicalAddress,
                                    Size,
                                    MEMORY_ATTRIBUTE_NONE);

    if (!NT_SUCCESS(Status)) {
        goto ZeroPhysicalPagesEnd;
    }

    RtlZeroMemory(VirtualAddress, Size);
    BlMmUnmapVirtualAddress(VirtualAddress, Size);

ZeroPhysicalPagesEnd:
    return Status;
}

