/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    utility.c

Abstract:

    This module implements misc. utility functions for the Boot Environment
    Library.

Environment:

    Boot.

--*/

// ------------------------------------------------------------------- Includes

#include <bootlibp.h>

// -------------------------------------------------------------------- Globals

PRSDT UtlRsdt = NULL;
PXSDT UtlXsdt = NULL;

PVOID UtlMcContext = NULL;
PMULTICAST_DISPLAY_WAIT_MESSAGE_ROUTINE UtlMcDisplayMessageRoutine = NULL;
PMULTICAST_UPDATE_WAIT_MESSAGE_ROUTINE UtlMcUpdateMessageRoutine = NULL;
PWSTR UtlProgressInfo = NULL;
BOOLEAN UtlProgressNeedsInfoUpdate = FALSE;
PUPDATE_PROGRESS_ROUTINE UtlProgressRoutine = NULL;
PUPDATE_PROGRESS_INFO_ROUTINE UtlProgressInfoRoutine = NULL;
PVOID UtlProgressContext = NULL;
ULONG UtlProgressGranularity = 0;
ULONG UtlCurrentPercentComplete = 0;
ULONG UtlNextUpdatePercentage = 0;
PVOID UtlAcpiOverrideBuffer;
ULONG UtlAcpiOverrideBufferSize;

// ------------------------------------------------------------------ Functions

NTSTATUS
BlUtlAcpiReset (
    VOID
    )

/*++

Routine Description:

    This routine implements the ACPI reset method for memory and IO.

    N.B. PCI config space method is not currently supported.

Arguments:

    None.

Return Value:

    NT status code on error, otherwise the system is reset.

--*/

{

    PFADT Fadt;
    PVOID ResetAddress;
    NTSTATUS Status;

    //
    // Get the FADT.
    //

    Status = BlUtlGetAcpiTable(&Fadt, FADT_SIGNATURE);
    if (!NT_SUCCESS(Status)) {
        goto AcpiResetEnd;
    }

    ASSERT(Fadt != NULL);

    //
    // Use the ACPI specified RESET_REG capability if specified.
    //

    Status = STATUS_NOT_SUPPORTED;
    if ((Fadt->Header.Revision > 1) && (Fadt->flags & RESET_CAP)) {
        switch (Fadt->reset_reg.AddressSpaceID) {
            case 0: // Memory
                Status = BlMmMapPhysicalAddress(&ResetAddress,
                                                Fadt->reset_reg.Address,
                                                1,
                                                MEMORY_ATTRIBUTE_NONE);

                if (!NT_SUCCESS(Status)) {
                    goto AcpiResetEnd;
                }

                WRITE_REGISTER_UCHAR(ResetAddress, Fadt->reset_val);
                BlMmUnmapVirtualAddress(ResetAddress, 1);
                Status = STATUS_UNSUCCESSFUL;
                break;

            case 1: // I/O
#if !defined(_ARM_) && !defined(_ARM64_)
                BL_WRITE_PORT_UCHAR((PUCHAR)(ULONG_PTR)
                                    Fadt->reset_reg.Address.LowPart,
                                    Fadt->reset_val);
#endif

                Status = STATUS_UNSUCCESSFUL;
                break;

            case 2: // PCI Config
                Status = STATUS_NOT_IMPLEMENTED;
                BlStatusPrint(L"WARNING: ACPI reset via PCI config space is "
                              L"NOT supported (email ntcrload alias)!\n");

                break;

            default:
                Status = STATUS_INVALID_PARAMETER;
                break;
        }

        //
        // Stall the processor for 1000 microseconds to give the machine a
        // chance to reboot.
        //

        BlTimeStallExecution(1000);
    }

    BlMmUnmapVirtualAddress(Fadt, Fadt->Header.Length);

AcpiResetEnd:
    return Status;
}

NTSTATUS
BlUtlInitialize (
    VOID
    )

/*++

Routine Description:

    This routine initializes the utility library, setting up global
    variables for first use.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS.

--*/

{

    BlpInterfaceEnter(InterfaceUtl);
    UtlRsdt = NULL;
    UtlXsdt = NULL;
    UtlMcContext = NULL;
    UtlMcDisplayMessageRoutine = NULL;
    UtlMcUpdateMessageRoutine = NULL;
    UtlProgressRoutine = NULL;
    UtlProgressContext = NULL;
    UtlProgressInfoRoutine = NULL;
    UtlProgressGranularity = 0;
    UtlCurrentPercentComplete = 0;
    UtlNextUpdatePercentage = 0;
    UtlProgressNeedsInfoUpdate = FALSE;
    UtlProgressInfo = NULL;
    BlpInterfaceExit(InterfaceUtl);
    return STATUS_SUCCESS;
}

NTSTATUS
BlUtlSetAcpiTableOverrides (
    __in_bcount(TableOverrideSize) PVOID TableOverrides,
    __in ULONG TableOverrideSize
    )

/*++

Routine Description:

    This routine will add a group of tables that will be returned instead of any
    default tables supplied from the firmware.

Arguments:

    TableOverrides - Supplies one or more tables to be used as overrides.

    TableOverrideSize - Supplies the size in bytes of the override buffer.

Returns:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if overrides were already specified.

--*/

{

    if (UtlAcpiOverrideBuffer != NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    UtlAcpiOverrideBuffer = TableOverrides;
    UtlAcpiOverrideBufferSize = TableOverrideSize;

    return STATUS_SUCCESS;
}

NTSTATUS
BlUtlGetAcpiTableOverrides (
    __deref_out_bcount(*TableOverrideSize) PVOID* TableOverrides,
    __out PULONG TableOverrideSize
    )

/*++

Routine Description:

    This routine will add a group of tables that will be returned instead of any
    default tables supplied from the firmware.

Arguments:

    TableOverrides - Receives a pointer to one or more tables to be used as overrides.

    TableOverrideSize - Receives the size in bytes of the override buffer.

Returns:

    STATUS_SUCCESS when successful.

--*/

{

    *TableOverrides = UtlAcpiOverrideBuffer;
    *TableOverrideSize = UtlAcpiOverrideBufferSize;
    return STATUS_SUCCESS;
}

NTSTATUS
BlUtlGetAcpiTable (
    __deref_out PVOID *TableAddress,
    __in ULONG TableSignature
    )

/*++

Routine Description:

    This routine will find and virtually map the requested ACPI table.

Arguments:

    TableAddress - On successful return, contains a pointer to the virtually
        mapped ACPI table.

    TableSignature - The four byte unique signature for the requested table.

Returns:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if TableAddress is NULL.
    STATUS_NOT_FOUND if the requested table could not be found.
    STATUS_NO_MEMORY if an internal allocation fails.

--*/

{

    PDESCRIPTION_HEADER Header;
    ULONG Index;
    ULONG Length;
    PDESCRIPTION_HEADER OverrideEnd;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    ULONG TableCount;

    BlpInterfaceEnter(InterfaceUtl);
    Header = NULL;

    //
    // Sanitize parameters.
    //

    if (TableAddress == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto BlUtlGetAcpiTableEnd;
    }

    //
    // Check to see if the table is not supported.
    //

    if (IS_UNSUPPORTED_ACPI_TABLE(TableSignature) != FALSE) {
        Status = STATUS_NOT_SUPPORTED;
        goto BlUtlGetAcpiTableEnd;
    }

    //
    // Use overriden tables, if any.
    //

    if (UtlAcpiOverrideBuffer != NULL) {
        Header = (PDESCRIPTION_HEADER)UtlAcpiOverrideBuffer;
        OverrideEnd =
            (PDESCRIPTION_HEADER)((ULONG_PTR)Header + UtlAcpiOverrideBufferSize);

        while (Header < OverrideEnd) {
            if (Header->Length < sizeof(DESCRIPTION_HEADER)) {
                break;
            }

            if (Header->Signature == TableSignature) {
                *TableAddress = Header;
                Status = STATUS_SUCCESS;
                goto BlUtlGetAcpiTableEnd;
            }

            Header = (PDESCRIPTION_HEADER)((ULONG_PTR)Header + Header->Length);
        }
    }

    //
    // Obtain and map the RSDT (Root System Descriptor Table).  The resulting
    // table could be the RSDT or the XSDT (Extended System Descriptor Table).
    // The correct global pointer should be set.  Only try to find the RSDT/XSDT
    // if it has not been previously found.
    //

    if ((UtlRsdt == NULL) && (UtlXsdt == NULL)) {
        Status = BlFwGetSystemTable(FW_TABLE_RSDT, &PhysicalAddress);
        if (!NT_SUCCESS(Status)) {
            goto BlUtlGetAcpiTableEnd;
        }

        Status = BlMmMapPhysicalAddress(&Header,
                                        PhysicalAddress,
                                        sizeof(DESCRIPTION_HEADER),
                                        MEMORY_ATTRIBUTE_NONE);

        if (!NT_SUCCESS(Status)) {
            goto BlUtlGetAcpiTableEnd;
        }

        Length = Header->Length;
        BlMmUnmapVirtualAddress(Header, sizeof(DESCRIPTION_HEADER));
        Status = BlMmMapPhysicalAddress(&Header,
                                        PhysicalAddress,
                                        Length,
                                        MEMORY_ATTRIBUTE_NONE);

        if (!NT_SUCCESS(Status)) {
            Header = NULL;
            goto BlUtlGetAcpiTableEnd;
        }

        //
        // If this is the XSDT, ensure that the correct global pointer is set.
        //

        if (Header->Signature == XSDT_SIGNATURE) {
            UtlXsdt = (PXSDT)Header;

        } else {
            UtlRsdt = (PRSDT)Header;
        }
    }

    ASSERT(((UtlXsdt != NULL) && (UtlRsdt == NULL)) ||
           ((UtlXsdt == NULL) && (UtlRsdt != NULL)));

    //
    // If the caller requested either the RSDT or XSDT, return the global
    // pointer.
    //

    if (TableSignature == RSDT_SIGNATURE) {
        if (UtlRsdt == NULL) {
            Status = STATUS_NOT_FOUND;

        } else {
            *TableAddress = UtlRsdt;
            Status = STATUS_SUCCESS;
        }

        goto BlUtlGetAcpiTableEnd;

    } else if (TableSignature == XSDT_SIGNATURE) {
        if (UtlXsdt == NULL) {
            Status = STATUS_NOT_FOUND;

        } else {
            *TableAddress = UtlXsdt;
            Status = STATUS_SUCCESS;
        }

        goto BlUtlGetAcpiTableEnd;
    }

    //
    // Search through the RSDT/XSDT to find the requested table.  If the XSDT
    // was found, use that, otherwise use the RSDT.  For each table, map the
    // header and check the signature.  If the table is the one being sought
    // then map the rest of the table and return it to the caller.
    //

    if (UtlXsdt != NULL) {
        TableCount = NumTableEntriesFromXSDTPointer(UtlXsdt);

    } else {
        TableCount = NumTableEntriesFromRSDTPointer(UtlRsdt);
    }

    for (Index = 0; Index < TableCount; Index += 1) {
        if (UtlXsdt != NULL) {
            PhysicalAddress.QuadPart = UtlXsdt->Tables[Index].QuadPart;

        } else {
            PhysicalAddress.QuadPart = UtlRsdt->Tables[Index];
        }

        Status = BlMmMapPhysicalAddress(&Header,
                                        PhysicalAddress,
                                        sizeof(DESCRIPTION_HEADER),
                                        MEMORY_ATTRIBUTE_NONE);

        if (!NT_SUCCESS(Status)) {
            goto BlUtlGetAcpiTableEnd;
        }

        if (Header->Signature == TableSignature) {
            Length = Header->Length;
            BlMmUnmapVirtualAddress(Header, sizeof(DESCRIPTION_HEADER));
            Status = BlMmMapPhysicalAddress(TableAddress,
                                            PhysicalAddress,
                                            Length,
                                            MEMORY_ATTRIBUTE_NONE);

            goto BlUtlGetAcpiTableEnd;

        } else {
            BlMmUnmapVirtualAddress(Header, sizeof(DESCRIPTION_HEADER));
        }
    }

    Status = STATUS_NOT_FOUND;

BlUtlGetAcpiTableEnd:
    BlpInterfaceExit(InterfaceUtl);
    return Status;
}


NTSTATUS
BlUtlPopulateAcpiTableCache (
    __deref_out PVOID *CacheAddress,
    __out PULONG CacheSize,
    __in_ecount(Count) const ULONG* TableSignatures,
    __in ULONG Count
    )

/*++

Routine Description:

    This routine will find the requested ACPI tables and copy them to a
    contiguous buffer pointed to by CacheAddress.  Failure to find one or all
    requested tables is not considered an error.

Arguments:

    CacheAddress - On successful return where CacheSize > 0, contains a pointer
        to a contiguous ACPI table cache.  Callers are expected to free this
        buffer when it is no longer required.

    CacheSize - On successful return, contains the size of the ACPI table cache.

    TableSignatures - An array containing signatures of tables to collect.

    Count - The number of elements in TableSignatures.

Returns:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if any parameter is NULL or 0.
    STATUS_NO_MEMORY if an internal allocation fails.

--*/

{

    PDESCRIPTION_HEADER Header;
    ULONG Index;
    NTSTATUS Status;
    PVOID* TablePointers;
    ULONG TempCacheSize;
    PUCHAR WriteOffset;

    TablePointers = NULL;

    BlpInterfaceEnter(InterfaceUtl);

    //
    // Sanitize parameters.
    //

    if ((CacheAddress == NULL) || (CacheSize == NULL) ||
        (TableSignatures == NULL) || (Count == 0)) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlUtlPopulateAcpiTableCacheEnd;
    }


    //
    // Allocate an array of pointers to keep track of the ACPI tables.
    //

    TablePointers = BlMmAllocateHeap(Count * sizeof(PVOID));
    if (TablePointers == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BlUtlPopulateAcpiTableCacheEnd;
    }

    //
    // Get pointers to the ACPI fixed tables that will later be copied to a
    // contiguous buffer.
    //

    TempCacheSize = 0;
    Header = NULL;
    for (Index = 0; Index < Count; Index++) {
        Status = BlUtlGetAcpiTable(&Header, TableSignatures[Index]);
        if (!NT_SUCCESS(Status)) {

            //
            // None of the tables in the list are strictly required, so
            // continue execution.
            //

            TablePointers[Index] = NULL;
            continue;
        }

        TempCacheSize += Header->Length;
        TablePointers[Index] = Header;
    }

    //
    // If none of the tables were found, return now.
    //

    if (TempCacheSize == 0) {
        *CacheAddress = NULL;
        *CacheSize = 0;
        Status = STATUS_SUCCESS;
        goto BlUtlPopulateAcpiTableCacheEnd;
    }

    //
    // Allocate a contiguous buffer to hold the ACPI tables.
    //

    *CacheAddress = BlMmAllocateHeap(TempCacheSize);
    if (*CacheAddress == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BlUtlPopulateAcpiTableCacheEnd;
    }

    *CacheSize = TempCacheSize;

    //
    // Iterate over the TablePointers and copy the tables to the contiguous
    // buffer.
    //

    WriteOffset = *CacheAddress;
    Header = NULL;
    for (Index = 0; Index < Count; Index++) {
        if (TablePointers[Index] == NULL) {
            continue;
        }

        Header = (PDESCRIPTION_HEADER)TablePointers[Index];
        RtlCopyMemory(WriteOffset, Header, Header->Length);
        WriteOffset += Header->Length;
    }

    Status = STATUS_SUCCESS;

BlUtlPopulateAcpiTableCacheEnd:
    if (TablePointers != NULL) {
        BlMmFreeHeap(TablePointers);
    }

    BlpInterfaceExit(InterfaceUtl);
    return Status;
}


VOID
BlUtlMulticastDisplayMessage (
    VOID
    )

/*++

Routine Description:

    This function displays the wait message (waiting for multicast session to
    start) on user console.

Arguments:

    None.

Return Value:

    None.

--*/

{

    BlpInterfaceEnter(InterfaceUtl);
    if (UtlMcDisplayMessageRoutine != NULL) {
        UtlMcDisplayMessageRoutine(UtlMcContext, TRUE);
    }

    BlpInterfaceExit(InterfaceUtl);
    return;
}

VOID
BlUtlMulticastRemoveMessage (
    VOID
    )

/*++

Routine Description:

    This function removes the wait message (waiting for multicast session to
    start) from user console.

Arguments:

    None.

Return Value:

    None.

--*/

{

    BlpInterfaceEnter(InterfaceUtl);
    if (UtlMcDisplayMessageRoutine != NULL) {
        UtlMcDisplayMessageRoutine(UtlMcContext, FALSE);
    }

    BlpInterfaceExit(InterfaceUtl);
    return;
}

NTSTATUS
BlUtlMulticastUpdateMessage (
    VOID
    )

/*++

Routine Description:

    This function updates the user console to show activity while waiting for
    multicast session to start.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS on success.
    STATUS_CANCELLED if user has cancelled the download.

--*/

{
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceUtl);
    if (UtlMcUpdateMessageRoutine != NULL) {
        Status = UtlMcUpdateMessageRoutine(UtlMcContext);

    } else {
        Status = STATUS_SUCCESS;
    }

    BlpInterfaceExit(InterfaceUtl);
    return Status;
}

NTSTATUS
BlUtlRegisterMulticastRoutine (
    __in PMULTICAST_DISPLAY_WAIT_MESSAGE_ROUTINE DisplayRoutine,
    __in PMULTICAST_UPDATE_WAIT_MESSAGE_ROUTINE UpdateRoutine,
    __in PVOID Parameter
    )

/*++

Routine Description:

    This function registers callbacks which are used to display message to user
    when multicast session has not started and to display a spinning counter to
    indicate that server is still up and running.

Arguments:

    DisplayRoutine - Supplies pointer to function which is used to display and
        remove the message which lets the user know that we are waiting for
        download to start.

    UpdateRoutine - Supplies pointer to function that is used to update the
        display with some activity (a spinning character) to let the user know
        that we are not dead and still waiting for session to start.

    Parameter - Supplies caller-specified parameter which is passed back to
        the callback functions.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_UNSUCCESSFUL if a callback is already registered.
    STATUS_INVALID_PARAMETER if invalid parameters are specified.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceUtl);
    if (UtlMcDisplayMessageRoutine != NULL) {
        Status = STATUS_UNSUCCESSFUL;

    } else if ((DisplayRoutine == NULL) || (UpdateRoutine == NULL)) {
        Status = STATUS_INVALID_PARAMETER;

    } else {
        UtlMcDisplayMessageRoutine = DisplayRoutine;
        UtlMcUpdateMessageRoutine = UpdateRoutine;
        UtlMcContext = Parameter;
        Status = STATUS_SUCCESS;
    }

    BlpInterfaceExit(InterfaceUtl);
    return Status;
}

NTSTATUS
BlUtlRegisterProgressRoutine (
    __in PUPDATE_PROGRESS_ROUTINE ProgressRoutine,
    __in_opt PUPDATE_PROGRESS_INFO_ROUTINE ProgressInfoRoutine,
    __inout_opt PVOID ProgressContext,
    __in ULONG ProgressGranularity
    )

/*++

Routine Description:

    This routine registers a progress update routine with the library.
    Only one routine can be registered at once, so if there is already
    a routine registered the routine fails.

Arguments:

    ProgressRoutine - Supplies a routine to be called on progress updates.

    ProgressInfoRoutine - Supplies a simple routine to display additional
                          context for the progress indicator, such as IP
                          address.

    ProgressContext - Supplies a context to be passed to the progress
        routine during progress updates.

    ProgressGranularity - Supplies the percentage granularity that updates
        should be sent to the progress routine.  Progress updates will only
        be sent when the task completion percentage is a multiple of this
        granularity.

Return Value:

    STATUS_SUCCESS if the routine was registered successfully.
    STATUS_UNSUCCESSFUL if there was already a routine registered.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceUtl);
    if (UtlProgressRoutine != NULL) {
        Status = STATUS_UNSUCCESSFUL;

    } else {
        UtlProgressRoutine = ProgressRoutine;
        UtlProgressContext = ProgressContext;
        UtlProgressGranularity = ProgressGranularity;
        UtlCurrentPercentComplete = 0;
        UtlNextUpdatePercentage = 0;
        if (ProgressInfoRoutine != NULL) {
            UtlProgressInfoRoutine = ProgressInfoRoutine;
        }

        Status = STATUS_SUCCESS;
    }

    BlpInterfaceExit(InterfaceUtl);
    return Status;
}

VOID
BlUtlUnregisterMulticastRoutine (
    __in PMULTICAST_DISPLAY_WAIT_MESSAGE_ROUTINE DisplayRoutine
    )

/*++

Routine Description:

    This routine unregisters a multicast routines with the library.

Arguments:

    DisplayRoutine - Supplies a routine to unregister.  This must match
        the registered routine for the unregister to take place.

Return Value:

    None.

--*/

{

    BlpInterfaceEnter(InterfaceUtl);
    if (DisplayRoutine == UtlMcDisplayMessageRoutine) {
        UtlMcContext = NULL;
        UtlMcDisplayMessageRoutine = NULL;
        UtlMcUpdateMessageRoutine = NULL;
    }

    BlpInterfaceExit(InterfaceUtl);
    return;
}

VOID
BlUtlUnregisterProgressRoutine (
    __in PUPDATE_PROGRESS_ROUTINE ProgressRoutine
    )

/*++

Routine Description:

    This routine unregisters a progress update routine with the library.

Arguments:

    ProgressRoutine - Supplies a routine to unregister.  This must match
        the registered routine for the unregister to take place.

Return Value:

    None.

--*/

{

    BlpInterfaceEnter(InterfaceUtl);
    if (ProgressRoutine == UtlProgressRoutine) {
        UtlProgressRoutine = NULL;
        UtlProgressInfoRoutine = NULL;
        if (UtlProgressInfo != NULL) {
            BlMmFreeHeap(UtlProgressInfo);
        }
    }

    BlpInterfaceExit(InterfaceUtl);
    return;
}

VOID
BlUtlUpdateProgress (
    __in ULONG PercentComplete,
    __out_opt PBOOLEAN ProgressDisplayComplete
    )

/*++

Routine Description:

    This routine updates the progress of a task.  This implementation is
    currently very simple.  It doesn't track different tasks and the progress
    of each, or anything more robust than a simple percentage.  As a result
    it is primarily useful when there is a single expensive routine that needs
    to report progress.

Arguments:

    PercentComplete - Supplies the percentage, from 0 to 100 that the task
        is complete.

    ProgressDisplayComplete - Supplies a boolean that receives a value
        indicating if the progress display is complete.

Return Value:

    None.

--*/

{

    BlpInterfaceEnter(InterfaceUtl);
    if (UtlProgressRoutine != NULL) {
        if (PercentComplete > 100) {
            PercentComplete = 100;
        }

        //
        // If this looks like the beginning of a new task and the previous
        // task is complete, restart the percentage tracking from the beginning.
        // Tasks must be independent and non-overlapping to be supported here.
        //

        if (PercentComplete < UtlCurrentPercentComplete) {
            UtlCurrentPercentComplete = 0;
            UtlNextUpdatePercentage = 0;
        }

        //
        // Update the progress at the granularity specified.
        //

        while (PercentComplete >= UtlNextUpdatePercentage) {
            UtlProgressRoutine(UtlProgressContext,
                               UtlNextUpdatePercentage,
                               ProgressDisplayComplete);

            UtlNextUpdatePercentage += UtlProgressGranularity;
        }

        UtlCurrentPercentComplete = PercentComplete;

        //
        // If there is progress information to update in addition to the
        // percentage, do this now.
        //

        if ((UtlProgressInfoRoutine != NULL) &&
            (UtlProgressNeedsInfoUpdate == TRUE)) {
            UtlProgressInfoRoutine(UtlProgressContext, UtlProgressInfo);
            UtlProgressNeedsInfoUpdate = FALSE;
        }

    } else {
        if (ARGUMENT_PRESENT(ProgressDisplayComplete) != FALSE) {
            *ProgressDisplayComplete = TRUE;
        }
    }

    BlpInterfaceExit(InterfaceUtl);
    return;
}

VOID
BlUtlUpdateProgressInfo (
    __in PWSTR ProgressInfo
    )

/*++

Routine Description:

    This routine updates the progress context of a task.

Arguments:

    ProgressInfo - Supplies a string of additional context for the progress
        indicator, such as IP address.

Return Value:

    None.

--*/

{

    NTSTATUS Status;
    SIZE_T WcsSize;

    BlpInterfaceEnter(InterfaceUtl);
    if (UtlProgressInfoRoutine != NULL) {
        Status = RtlSIZETMult(wcslen(ProgressInfo), sizeof(WCHAR), &WcsSize);
        if (!NT_SUCCESS(Status)) {
            goto UpdateProgressInfoEnd;
        }

        Status = RtlSIZETAdd(WcsSize, sizeof(UNICODE_NULL), &WcsSize);
        if (!NT_SUCCESS(Status)) {
            goto UpdateProgressInfoEnd;
        }

        UtlProgressInfo = BlMmAllocateHeap(WcsSize);
        if (UtlProgressInfo != NULL) {
            wcscpy_s(UtlProgressInfo, (WcsSize / sizeof(WCHAR)), ProgressInfo);
            UtlProgressNeedsInfoUpdate = TRUE;
        }
    }

UpdateProgressInfoEnd:
    BlpInterfaceExit(InterfaceUtl);
    return;
}

NTSTATUS
BlUtlCopySmBiosTable (
    __deref_out PVOID *SmBiosTableCopy
    )

/*++

Routine Description:

    This routine will attempt to find the SMBIOS table.  When successful,
    a buffer is allocated and the table is copied to the newly allocated
    buffer.

Arguments:

    SmBiosTableCopy - On successful output, contains the address of the
        allocated buffer containing the SMBIOS table.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NOT_FOUND if the requested table could not be found.
    STATUS_NO_MEMORY if a memory allocation fails.
    STATUS_INVALID_PARAMETER if the table was corrupt.

--*/

{

    UCHAR Checksum;
    PVOID SmBiosTable;
    PSMBIOS_EPS_HEADER SmBiosEps;
    PSMBIOS3_EPS_HEADER SmBios3Eps;
    NTSTATUS Status;
    ULONG TableSize;

    ASSERT(ARGUMENT_PRESENT(SmBiosTableCopy));

    SmBiosEps = NULL;
    TableSize = 0;

    //
    // Attempt to find the physical address of the SMBIOS table.
    //

    Status = BlUtlMapSmBiosTable(&SmBiosTable, &TableSize);
    if (!NT_SUCCESS(Status)) {
        goto BlpCopySmBiosTableEnd;
    }

    if (IS_SMBIOS3_HEADER(SmBiosTable) == FALSE) {
        TableSize = sizeof(*SmBios3Eps);
    }

    NT_ASSERT(TableSize >= sizeof(*SmBios3Eps));

    SmBios3Eps = BlMmAllocateHeap(TableSize);
    if (SmBios3Eps == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BlpCopySmBiosTableEnd;
    }

    //
    // Convert to SMBios 3 table header.
    //

    if (IS_SMBIOS3_HEADER(SmBiosTable) == FALSE) {
        SmBiosEps = SmBiosTable;
        RtlZeroMemory(SmBios3Eps, sizeof(*SmBios3Eps));
        SmBios3Eps->Signature[0] = '_';
        SmBios3Eps->Signature[1] = 'S';
        SmBios3Eps->Signature[2] = 'M';
        SmBios3Eps->Signature[3] = '3';
        SmBios3Eps->Signature[4] = '_';
        SmBios3Eps->Length = sizeof(*SmBios3Eps);
        SmBios3Eps->MajorVersion = SmBiosEps->MajorVersion;
        SmBios3Eps->MinorVersion = SmBiosEps->MinorVersion;
        SmBios3Eps->EntryPointRevision = 0x01;
        SmBios3Eps->StructureTableMaximumSize = SmBiosEps->StructureTableLength;
        SmBios3Eps->StructureTableAddress = SmBiosEps->StructureTableAddress;
        Checksum = (UCHAR)BlUtlCheckSum(0,
                                        SmBios3Eps,
                                        SmBios3Eps->Length,
                                        CHECKSUM_BYTE_ARRAY);

        SmBios3Eps->Checksum = -Checksum;

    } else {

        //
        // The SMBIOS Table is valid.  Copy the table into a buffer allocated
        // for the loader block.
        //

        RtlCopyMemory(SmBios3Eps, SmBiosTable, TableSize);
    }

    *SmBiosTableCopy = SmBios3Eps;
BlpCopySmBiosTableEnd:

    if (NULL != SmBiosTable) {
        BlMmUnmapVirtualAddress(SmBiosTable, TableSize);
    }

    return Status;
}


