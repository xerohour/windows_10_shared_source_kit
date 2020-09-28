/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    table.c

Abstract: 
    
    Misc. implementation of a generic data table.  The table is an array
    of functional or data pointers.  The routines implemented in this module
    are for the processing and manipulation of a generic table.

Environment:

    boot

--*/

// ------------------------------------------------------------------- Includes

#include <bootlibp.h>

// ------------------------------------------------------------------ Functions

NTSTATUS
BlTblSetEntry (
    __deref_in_ecount(*TableEntries) PVOID **Table,
    __inout PULONG TableEntries,
    __in PVOID Entry,
    __out PULONG Id,
    __in PTABLE_PURGE_ENTRY PurgeRoutine
    )

/*++

Routine Description:

    Attempts to insert an entry into a device table.
    
    Given a table of pointers, BlTblSetEntry will search for a free
    entry in the data table.  If one is not found, it will traverse the 
    table a second time, this time with a callback for each table 
    entry.  This routine is defined to pick an entry to close and delete.

    The device table is actually an array of pointers to the functional or data
    entries.  The array is also allocated from the heap, so it can grow in size
    if needed.
    
    BlTblSetEntry will simply do a linear search of the table and use the
    first id that is available.  If unsuccessful, this routine will do a 
    second search using a the callback routine, PurgeRoutine, on each 
    entry.  This routine is defined to look at the device and decide if it
    should force the close and purge of the device.  If the routine succeeds, 
    it is assumed to have successfully closed the device and it's  entry
    can be deleted.
    
Arguments:

    Table - Pointer to device table.

    TableEntries - Max size of Table.  The number of entries, the table was
                   allocated for.

    Entry - The new device's entry.
    
    Id - Pointer to address to return the index of the new entry.

    PurgeRoutine - Callback routine used to decide if an entry should be 
                   purged.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if Table or TableEntires are NULL.
    STATUS_NO_MEMORY if the table needs to grow and allocation failed.
    
--*/

{

    PVOID *DeviceTable;
    ULONG Index;
    ULONG MaxEntries;
    PVOID *NewDeviceTable;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceTbl);

    //
    // Simple parameter checking.
    //

    if (Table == NULL || *Table == NULL || TableEntries == NULL ||
        PurgeRoutine == NULL) {

        Status = STATUS_INVALID_PARAMETER;
        goto BlTblSetEntryEnd;
    }

    //
    // Find unused id.
    //

    DeviceTable = *Table;
    MaxEntries = *TableEntries;    
    for (Index = 0; 
         (Index < MaxEntries) && (DeviceTable[Index] != NULL); 
         Index += 1) {

        //
        // Nothing.
        //

    }

    if (Index == MaxEntries) {

        //
        // If we did not find an open entry, find the first available 
        // entry that has not yet been purged.
        //

        Status = STATUS_UNSUCCESSFUL;
        for (Index = 0; Index < MaxEntries; Index += 1) {
            Status = PurgeRoutine(DeviceTable[Index]);

            //
            // If entry was successfully purged, then break out of loop.  This
            // is done here instead of in the for loop's conditional to ensure 
            // i is set to the index that will be used for the insertion.
            //

            if (NT_SUCCESS(Status)) {
                break;
            }
        }

        //
        // If we did not find an open entry, grow the table.
        //

        if (!NT_SUCCESS(Status)) {

            //
            // Double the size of the table.
            //

            NewDeviceTable = BlMmAllocateHeap(MaxEntries * 2 * sizeof(PVOID));
            if (NewDeviceTable == NULL) {
                Status = STATUS_NO_MEMORY;
                goto BlTblSetEntryEnd;
            }

            //
            // Copy old device table.  Zero out the rest of the new table.
            //

            RtlZeroMemory(NewDeviceTable + MaxEntries, 
                          MaxEntries * sizeof(PVOID));

            RtlCopyMemory(NewDeviceTable, 
                          DeviceTable, 
                          MaxEntries * sizeof(PVOID));

            BlMmFreeHeap(DeviceTable);
            DeviceTable = NewDeviceTable;
            *TableEntries = MaxEntries * 2;
            *Table = DeviceTable;
        }
    }

    //
    // Set the pointer in the table to the new entry.
    //

    Status = STATUS_SUCCESS;
    DeviceTable[Index] = Entry;
    *Id = Index;

BlTblSetEntryEnd:

    BlpInterfaceExit(InterfaceTbl);
    return Status;
}

PVOID
BlTblFindEntry (
    __deref_in_ecount(TableEntries) PVOID *Table,
    __in ULONG TableEntries,
    __out PULONG Id,
    __in PTABLE_COMPARE_ENTRY CompareRoutine,
    __in_opt PVOID Arg1,
    __in_opt PVOID Arg2,
    __in_opt PVOID Arg3,
    __in_opt PVOID Arg4
    )

/*++

Routine Description:

    This routine will walk a device table looking for an entry that 
    successfully returns true to :
    CompareRoutine(Entry, Arg1, Arg2, Arg3).

Arguments:

    Table - Device table
    
    TableEntries - The max device entries in the device table.
    
    Id - Pointer to address to store index of the first entry that 
         successfully returns from the compare routine.
         
    CompareRoutine - PTABLE_COMPARE_ENTRY routine.
    
    Arg1 - Caller defined argument #1
    
    Arg2 - Caller defined argument #2 

    Arg3 - Caller defined argument #3
    
    Arg4 - Caller defined argument #4
    
Return Value:

    The first device that successfully returns from the CompareRoutine when 
    passed a device entry with the 3 caller defined arguments.  If no entry's
    return successfully, NULL is returned.  
    
--*/

{

    ULONG Index;
    PVOID ReturnValue;

    BlpInterfaceEnter(InterfaceTbl);
    ReturnValue = NULL;

    //
    // Parameter checking.
    //

    if ((Table != NULL) && (Id != NULL)) {
    
        //
        // Iterate through the device table looking at each entry to see if it 
        // satisfies the CompareRoutine, given the 3 caller defined arguments.
        //

        for (Index = 0; Index < TableEntries; Index += 1) {
            if ((Table[Index] != NULL) && 
                (CompareRoutine(Table[Index], Arg1, Arg2, Arg3, Arg4))) {

                //
                // Set the return values.
                //

                *Id = Index;
                ReturnValue = Table[Index];
                break;
            }
        }
    }

    BlpInterfaceExit(InterfaceTbl);
    return ReturnValue;
}

NTSTATUS
BlTblMap (
    __deref_in_ecount(TableEntries) PVOID *Table,
    __in ULONG TableEntries,
    __in PTABLE_MAP_ENTRY MapRoutine,
    __in BOOLEAN ErrorFatal
    )

/*++

Routine Description:

    Calls MapRoutine with every non-null entry in the device table.
    If ErrorFatal is TRUE, then DeviceTableMap will return immediately when
    an error occurs.  Otherwise, the last error will be returned after the 
    MapRoutine has been called with every table entry.

Arguments:

    Table - Device table
    
    TableEntries - The max device entries in the device table.

    MapRoutine - Routine to be called with every entry of the device table.
   
    ErrorFatal - TRUE if BlTblMap should return immediately if an 
                 error occurs.
    
Return Value:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if Table is NULL.
    Last error status received from the MapRoutine on a particular entry.
    
--*/

{

    NTSTATUS ReturnStatus;
    NTSTATUS Status;
    ULONG Index;

    BlpInterfaceEnter(InterfaceTbl);

    //
    // Parameter checking.
    //

    if (Table == NULL) {
        ReturnStatus = STATUS_INVALID_PARAMETER;
    
    } else {
    
        ReturnStatus = STATUS_SUCCESS;

        //
        // Iterate through the device table, calling the MapRoutine with every
        // non-null entry.
        //

        for (Index = 0; Index < TableEntries; Index += 1) {
            if (Table[Index] != NULL) {            
                Status = MapRoutine(Table[Index], Index);
                if (!NT_SUCCESS(Status)) {
                    ReturnStatus = Status;

                    //
                    // Exit loop if this error is to be considered fatal.
                    //

                    if (ErrorFatal != FALSE) {
                        break;
                    }
                }
            }
        }
    }

    BlpInterfaceExit(InterfaceTbl);
    return ReturnStatus;
}

NTSTATUS
TblDoNotPurgeEntry (
    __in PVOID Entry
    )

/*++

Routine Description:

    A PurgeRoutine that can be used by any caller to BlTblSetEntry.  When 
    used, this routine will not purge any existing table entries.

Arguments:

    Entry - Entry in table.
    
Return Value:

    STATUS_UNSUCCESSFUL always.
    
--*/

{
    
    UNREFERENCED_PARAMETER( Entry);

    return STATUS_UNSUCCESSFUL;
}

