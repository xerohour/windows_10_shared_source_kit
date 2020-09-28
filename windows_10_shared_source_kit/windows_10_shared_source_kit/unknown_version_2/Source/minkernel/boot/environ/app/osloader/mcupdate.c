/*++

Copyright (c) Microsoft Corporation

Module Name:

    mcupdate.c

Abstract:

    Routines required to load microcode update on boot processor.

Environment:

    Boot Environment.

--*/

// ------------------------------------------------------------------- Includes

#include "osloader.h"
#include "mcupdate.h"

#pragma warning( disable : 4055 )

// -------------------------------------------------------------------- Globals

PMCUPDATEMICROCODEFUNC McpUpdateMicrocodeFunc = NULL;

// ----------------------------------------------------------------- Prototypes

typedef
NTSTATUS
(*PMCUPDATEENTRYPOINT)(
    __out PMCUPDATE_FUNCTIONS ExportedFunctions,
    __in PMCUPDATE_DLL_DEPENDENCIES ImportedFunctions
    );

NTSTATUS
McpUpdateInitialize (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

PKLDR_DATA_TABLE_ENTRY
McpUpdateFindDataTableEntry (
    __in PLIST_ENTRY DataTableEntryList
    );

PVOID
McpMemoryLockStub (
    __in PVOID UnusedParam
    );

VOID
McpMemoryLockByHandleStub (
    __in PVOID UnusedParam
    );

NTSTATUS
McpMemoryUnlockStub (
    __in PVOID UnusedParam
    );

NTSTATUS
McpUpdateMicrocode (
    VOID
    );

ULONG
McpReadPciConfig (
    __in ULONG BusNumber,
    __in ULONG SlotNumber,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    );

// ------------------------------------------------------------------ Functions

NTSTATUS
OslLoadMicrocode (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine will initialize the internal structures required to perform
    microcode update and then triggers application of microcode update.

Arguments:

    LoaderBlock - A pointer to the loader parameter block.

Return Value:

    NT status code.

--*/

{
    NTSTATUS Status;

    Status = McpUpdateInitialize(LoaderBlock);
    if (!NT_SUCCESS(Status)) {
        goto LoadMicrocodeExit;
    }

    //
    // Update the processor microcode now.
    //

    McpUpdateMicrocode();

LoadMicrocodeExit:
    
    return Status;
}


NTSTATUS
McpUpdateInitialize (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This function initializes the microcode update architecture. It should be 
    called before any attempt is made to update microcode from a loaded 
    microcode update DLL.

Arguments:

    LoaderBlock - A pointer to the loader parameter block.
    
Return Value:

    NT status code.

--*/

{
    PKLDR_DATA_TABLE_ENTRY DataTableEntry;
    PMCUPDATEENTRYPOINT EntryPoint;
    MCUPDATE_FUNCTIONS Functions;
    PLIST_ENTRY LoadedModuleList;
    NTSTATUS Status;
    MCUPDATE_DLL_DEPENDENCIES UpdateDllImportedFunctions;

    Status = STATUS_SUCCESS;
    DataTableEntry = NULL;

    McpUpdateMicrocodeFunc = NULL;

    // 
    // Initialize the function dependencies for the update dll.
    //

    UpdateDllImportedFunctions.ReadPciConfig = McpReadPciConfig;
    UpdateDllImportedFunctions.LockPagableCodeSection = McpMemoryLockStub;
    UpdateDllImportedFunctions.LockPagableDataSection = McpMemoryLockStub;
    UpdateDllImportedFunctions.LockPagableSectionByHandle = 
        McpMemoryLockByHandleStub;
    UpdateDllImportedFunctions.UnlockPagableImageSection = 
        (PMCUPDATE_UNLOCK_PAGABLE_IMAGE_SECTION) McpMemoryUnlockStub;
    
    //
    // Determine whether a microcode update module has been loaded.  If one has 
    // been loaded, then determine the entry point and call it.  This will yield
    // pointers to the functions required to perform microcode update.  These
    // pointers must be stored for use when microcode update is required.
    //

    if (LoaderBlock == NULL) {
        ASSERT(FALSE);
        goto InitializeEnd;
    }

    LoadedModuleList = &LoaderBlock->LoadOrderListHead;
    DataTableEntry = McpUpdateFindDataTableEntry(LoadedModuleList);

    //
    // If microcode update module was not loaded nothing needs to be done.
    // 

    if (DataTableEntry == NULL) {
        goto InitializeEnd;
    }

    EntryPoint = (PMCUPDATEENTRYPOINT)DataTableEntry->EntryPoint;

    Status = (NTSTATUS)EntryPoint(&Functions, &UpdateDllImportedFunctions);

    if (!NT_SUCCESS(Status)) {
        goto InitializeEnd;
    }

    McpUpdateMicrocodeFunc = Functions.MicrocodeFunc;
    
InitializeEnd:

    return Status;
}


PKLDR_DATA_TABLE_ENTRY
McpUpdateFindDataTableEntry (
    __in PLIST_ENTRY DataTableEntryList
    )

/*++

Routine Description:

    This routine traverses the provided data table entry list in search of the
    entry for the microcode update library.

Arguments:

    DataTableEntryList - Linked list of LDR_DATA_TABLE_ENTRYs.

Return Value:

    A pointer to the data table entry for the specified image.

    NULL if there does not exist an entry for the image.

--*/

{
    UNICODE_STRING CompareName = RTL_CONSTANT_STRING (L"mcupdate.dll");
    PUNICODE_STRING EntryName;
    PKLDR_DATA_TABLE_ENTRY Entry;
    PLIST_ENTRY Index;

    //
    // Walk the specified loaded image list until the requested image is found,
    // or the end of the loaded image list is reached.
    //

    for (Index = DataTableEntryList->Flink;
         Index != DataTableEntryList;
         Index = Index->Flink) {

        Entry = CONTAINING_RECORD(Index,
                                  KLDR_DATA_TABLE_ENTRY,
                                  InLoadOrderLinks);

        EntryName = &Entry->BaseDllName;

        if (EntryName->Length != CompareName.Length) {
            continue;
        }

        if (_wcsicmp(EntryName->Buffer, CompareName.Buffer) == 0) {
            return Entry;
        }
    }

    return NULL;
}


PVOID
McpMemoryLockStub (
    __in PVOID UnusedParam
    )
{
    UNREFERENCED_PARAMETER(UnusedParam);

    return NULL;
}


VOID
McpMemoryLockByHandleStub (
    __in PVOID UnusedParam
    )
{
    UNREFERENCED_PARAMETER(UnusedParam);
}

NTSTATUS
McpMemoryUnlockStub (
    __in PVOID UnusedParam
    )
{
    UNREFERENCED_PARAMETER(UnusedParam);

    return STATUS_SUCCESS;
}


NTSTATUS
McpUpdateMicrocode (
    VOID
    )

/*++

Routine Description:

    This routine performs the microcode update.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{
    NTSTATUS Status;

    Status = STATUS_SUCCESS;

    if (McpUpdateMicrocodeFunc != NULL) {
        Status = McpUpdateMicrocodeFunc(MC_UPDATE_RESIDENT, 0);
    }

    return Status;
}


ULONG
McpReadPciConfig (
    __in ULONG BusNumber,
    __in ULONG SlotNumber,
    __out_bcount(Length) PVOID Buffer,
    __in ULONG Offset,
    __in ULONG Length
    )

/*++

Routine Description:

    This routine is provided so that the microcode update module can access PCI
    configuration space to obtain information to assist in its decision making
    flow. The microcode update module is only permitted to access configuration
    space on bus 0, and then only the first 256 bytes of each device.

Arguments:

    BusNumber - Supplies the bus number to be accessed. This is currently
        to bus 0.

    Slot - Supplies the slot number (device/function number) of the device to 
        access.

    Buffer - Supplies the space to store the data.

    Offset - Supplies the offset into configuration space to access.

    Length - Supplies a count in bytes of the maximum amount to return.

Return Value:

    Lenght of the PCI config space read.into the given Buffer.

-*/

{
    NTSTATUS Status;
    PCI_CONFIG_ADDRESS DeviceAddress;
    PCI_SLOT_NUMBER Slot;
    
    Status = STATUS_SUCCESS;
    
    //
    // The microcode update module is only permitted to access configuration
    // space on bus 0, and then only the first 256 bytes of each device.
    //

    if (BusNumber > 0) {
        return 0;
    }

    if (Length + Offset > 256 ) {
        if (Offset > 256) {
            return 0;
        }

        Length = 256 - Offset;
    }

    Slot.u.AsULONG = SlotNumber;

    DeviceAddress.Segment = 0;
    DeviceAddress.Bus = (UCHAR)BusNumber;
    DeviceAddress.Device = (UCHAR)Slot.u.bits.DeviceNumber;
    DeviceAddress.Function = (UCHAR)Slot.u.bits.FunctionNumber;

    Status = BlPltReadPciConfig(&DeviceAddress,
                                Buffer,
                                Offset,
                                Length);

    if (!NT_SUCCESS(Status)) {
        Length = 0;
    }

    return Length;
}


