/*++

Copyright (c) Microsoft Corporation

Module Name:

    osextens.c

Abstract:

    Handles loading OS Extensions and optional ACPI data tables.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"
#include <bootlibp.h>

//
// ---------------------------------------------------------------- Definitions
//

#define MAX_RESOURCE_ID_LENGTH \
    (_countof(L"VEN_vvvv&DEV_dddd&SUBVEN_ssss&SUBDEV_yyyy&REV_rrrr") + 1)

//
// ------------------------------------------------- Data Structure Definitions
//

//
// RESOURCE_ID_ORDER controls the order in which Resource ID's are enumerated
// by OslpGetResourceIdString. Changing/adding Resource ID formats requires
// modifying this set of enums and adding the appropriate case to that function.
//

typedef enum _RESOURCE_ID_ORDER {
    ResourceIdOrderInit = 0,
    ResourceIdOrderVenDevSubvenSubdevRev,
    ResourceIdOrderVenDevSubvenSubdev,
    ResourceIdOrderVenDevRev,
    ResourceIdOrderVenDev
} RESOURCE_ID_ORDER, *PRESOURCE_ID_ORDER;

//
// ------------------------------------------------- String Constant Definitions
//

const PCWSTR OslControlString = L"Control";
const PCWSTR OslHalExtensionString = L"HalExtension";
const PCWSTR OslOsExtensionDatabaseString = L"OsExtensionDatabase";
const PCWSTR OslPendingDriverOperationsString = L"PendingDriverOperations";

//
// ------------------------------------------------- Local Function Prototoypes
//

NTSTATUS
OslpLoadOsExtensionForResource (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __in ULONG SystemHiveHandle,
    __in PBYTE CsrtStart,
    __in CSRT_RESOURCE_GROUP_HEADER *ResourceGroup
    );

NTSTATUS
OslpGetKdExtensionName (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in BCDE_DEBUGGER_TYPE DebuggerType,
    __in ULONG ExtensionPathLength,
    __out_ecount(ExtensionPathLength) PWCHAR ExtensionPath
    );

NTSTATUS
OslpGetResourceIdString (
    __in CSRT_RESOURCE_GROUP_HEADER *ResourceGroup,
    __in ULONG ResourceIdLength,
    __out_ecount(ResourceIdLength) PWCHAR ResourceId,
    __inout PRESOURCE_ID_ORDER ResourceIdOrder
    );

NTSTATUS
OslpGetOsExtensionModulePath (
    __in ULONG Handle,
    __in __nullterminated PWCHAR ResourceId,
    __in ULONG ModulePathLength,
    __out_ecount(ModulePathLength) PWCHAR ModulePath,
    __in_opt PFWUPDATE_RESULTS_HEADER FirmwareUpdateResults
    );

NTSTATUS
OslpLoadHalExtensionFromModulePath (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __in __nullterminated PWCHAR ModulePath,
    __in ULONG OffsetIntoCsrt
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
OslGetAcpiTable (
    __deref_out PVOID *TableAddress,
    __in ULONG TableSignature,
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine returns a pointer to the first ACPI table matching the
    specified TableSignature. It will first search the optional ACPI table data
    block for the requested ACPI table. If the table is not found there, it will
    fall back to searching the ACPI Firmware table.

Arguments:

    TableAddress - Supplies a pointer that receives the address of the requested
        ACPI table upon success.

    TableSignature - Supplies the 4 byte unique signature for the requested
        table.

    LoaderBlock - Supplies a pointer to the loader parameter block.

Returns:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if TableAddress or LoaderBlock are NULL.
    STATUS_NOT_FOUND if the requested table could not be found.
    STATUS_NO_MEMORY if an internal allocation fails.

--*/

{

    PDESCRIPTION_HEADER Header;
    NTSTATUS Status;
    PBYTE TableCur;
    PBYTE TableEnd;

    if ((TableAddress == NULL) || (LoaderBlock == NULL)) {
        Status = STATUS_INVALID_PARAMETER;
        goto OslGetAcpiTableEnd;
    }

    switch (TableSignature) {
    case RSDT_SIGNATURE:
    case XSDT_SIGNATURE:

        //
        // Don't search for signatures that should not be overridden.
        //

        break;

    default:

        //
        // Search the optional ACPI table for the requested signature.
        //

        TableCur = LoaderBlock->Extension->AcpiTable;
        TableEnd = TableCur + LoaderBlock->Extension->AcpiTableSize;
        while (TableCur < TableEnd) {
            Header = (PDESCRIPTION_HEADER)TableCur;
            if (Header->Length < sizeof(DESCRIPTION_HEADER)) {
                break;
            }

            if (Header->Signature == TableSignature) {
                *TableAddress = Header;
                Status = STATUS_SUCCESS;
                goto OslGetAcpiTableEnd;
            }

            TableCur += Header->Length;
        }

        break;
    }

    //
    // There was no override for this table.
    // Fall back to the firmware ACPI table.
    //

    Status = BlUtlGetAcpiTable(TableAddress, TableSignature);

OslGetAcpiTableEnd:
    return Status;
}

NTSTATUS
OslMergeAcpiTableData (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in PVOID AcpiTableData,
    __in ULONG AcpiTableDataSize
    )

/*++

Routine Description:

    This routine merges an additional ACPI table into the LoaderBlock entry
    for the optional ACPI table. It handles the case where the LoaderBlock
    entry is NULL. It also updates the Bootloader's ACPI table handling code
    so that optional tables can be accessed within the bootloader using
    BlUtlGetAcpiTable.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

    AcpiTable - Supplies a pointer to the ACPI table to merge.

    AcpiTableSize - Supplies the size of the ACPI table to merge.

Return Value:

    STATUS_SUCCESS if the operation succeeds, otherwise an error status.

--*/

{

    NTSTATUS Status;

    if (LoaderBlock->Extension->AcpiTable==NULL) {

        //
        // Special case: if the LoaderBlock entry is empty, just copy over the
        // new table.
        //

        LoaderBlock->Extension->AcpiTable = AcpiTableData;
        LoaderBlock->Extension->AcpiTableSize = AcpiTableDataSize;
        Status = STATUS_SUCCESS;

    } else {
        Status = OslMergeAcpiTables(
                    &LoaderBlock->Extension->AcpiTable,
                    &LoaderBlock->Extension->AcpiTableSize,
                    AcpiTableData,
                    AcpiTableDataSize);
    }

    return Status;
}

NTSTATUS
OslpValidateAcpiTableData (
    __in PVOID AcpiTableData,
    __in ULONG AcpiTableDataSize
    )

/*++

Routine Description:

    This routine validates ACPI Table Data. It ensures that each table's
    checksum is correct and that the sum of all tables matches the size of
    the data block.

Arguments:

    AcpiTable - Supplies a pointer to the ACPI table to validate.

    AcpiTableSize - Supplies the size of the ACPI table to validate.

Return Value:

    STATUS_SUCCESS if the operation succeeds, otherwise an error status.

--*/

{

    UCHAR Checksum;
    PDESCRIPTION_HEADER Header;
    NTSTATUS Status;
    PBYTE TableCur;
    PBYTE TableEnd;

    TableCur = AcpiTableData;
    TableEnd = TableCur + AcpiTableDataSize;
    while (TableCur < TableEnd) {

        //
        // Ensure there is enough data in the table for the header.
        //

        if ((TableEnd - TableCur) < sizeof(DESCRIPTION_HEADER)) {
            Status = STATUS_UNSUCCESSFUL;
            goto OslpValidateAcpiTableDataEnd;
        }

        //
        // Validate the checksum of each table.
        //

        Header = (PDESCRIPTION_HEADER)TableCur;

        //
        // Ensure that the size of the table is at least as big
        // as a description header.
        //

        if (Header->Length < sizeof(DESCRIPTION_HEADER)) {
            Status = STATUS_UNSUCCESSFUL;
            goto OslpValidateAcpiTableDataEnd;
        }

        //
        // Ensure that the embedded table doesn't fall off the
        // end of the buffer.
        //

        if (PtrOffset(TableCur, TableEnd) < Header->Length) {
            Status = STATUS_UNSUCCESSFUL;
            goto OslpValidateAcpiTableDataEnd;
        }

        Checksum = (UCHAR)BlUtlCheckSum(0,
                                        Header,
                                        Header->Length,
                                        CHECKSUM_BYTE_ARRAY);

        if (Checksum != 0) {
            Status = STATUS_UNSUCCESSFUL;
            goto OslpValidateAcpiTableDataEnd;
        }
        TableCur += Header->Length;
    }

    //
    // After iterating through the table the current pointer should
    // exactly match the end pointer.
    //

    if (TableCur != TableEnd) {
        Status = STATUS_UNSUCCESSFUL;
        goto OslpValidateAcpiTableDataEnd;
    }

    Status = STATUS_SUCCESS;

OslpValidateAcpiTableDataEnd:
    return Status;
}

NTSTATUS
OslLoadOptionalAcpiTableData (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId
    )

/*++

Routine Description:

    This routine will load the optional acpitabl.dat file into memory for
    further processing by the OS.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

    OsFilePath - Supplies the file path to the Operating System directory.

    OsDeviceId - Supplies the handle for the device containing the Operating
        System.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if there is insufficient memory to perform a request.
    STATUS_INVALID_IMAGE_FORMAT if relocations can not be applied to a PE
        image being loaded.
    STATUS_NO_SUCH_FILE if a file required to load does not exist.
    STATUS_UNSUCCESSFUL if file system can not be mounted on OsDeviceId.

--*/

{

    PVOID AcpiTableData;
    ULONG AcpiTableDataSize;
    BOOLEAN BoolRet;
    USHORT OsFilePathOffset;
    NTSTATUS Status;
    BOOLEAN TestSigningEnabled;

    //
    // Only load acpitabl.dat if test-signing is enabled.
    //

    BlImgQueryCodeIntegrityBootOptions(BlGetApplicationEntry(),
                                       NULL,
                                       &TestSigningEnabled,
                                       NULL);

    if (TestSigningEnabled == FALSE) {
        return STATUS_SUCCESS;
    }

    OsFilePathOffset = OsFilePath->Length;
    AcpiTableData = NULL;
    AcpiTableDataSize = 0;
    BoolRet = BlAppendUnicodeToString(OsFilePath, ACPI_TABLE_NAME);
    if (BoolRet == FALSE) {
        Status = STATUS_NO_MEMORY;
        goto OslLoadAcpiTableEnd;
    }

    Status = BlImgLoadImageEx(OsDeviceId,
                              OSL_MEMORY_TYPE_ACPI_TABLE,
                              OsFilePath->Buffer,
                              &AcpiTableData,
                              &AcpiTableDataSize,
                              MEMORY_ATTRIBUTE_ALLOCATION_KSEG,
                              0,
                              IMAGE_FLAGS_NONE);

    if (!NT_SUCCESS(Status)) {
        if (Status == STATUS_NO_SUCH_FILE) {

            //
            // This is the only "expected" error while loading ACPI table
            // overload file. All other errors are fatal.
            //

            Status = STATUS_SUCCESS;
        }

        goto OslLoadAcpiTableEnd;
    }

    //
    // Validate the table data
    //

    Status = OslpValidateAcpiTableData(AcpiTableData, AcpiTableDataSize);
    if (!NT_SUCCESS(Status)) {
        goto OslLoadAcpiTableEnd;
    }

    //
    // Merge the optional Acpi table into the LoaderBlock
    //

    OslMergeAcpiTableData(LoaderBlock,  AcpiTableData, AcpiTableDataSize);

OslLoadAcpiTableEnd:
    OsFilePath->Length = OsFilePathOffset;
    OsFilePath->Buffer[OsFilePathOffset / sizeof(WCHAR)] = L'\0';
    return Status;
}

NTSTATUS
OslLoadOptionalAcpiWpbt (
    __out PVOID *WpbtTable,
    __out PULONG WpbtLength
    )

/*++

Routine Description:

    This routine will scan low memory (A0000 to 0) for WPBT table on PCAT
    machines.

Arguments:

    WpbtTable - Supplies a pointer which receives the ACPI table.

    WpbtLength - Supplies a pointer to a variable which receives the lenght of
                 the table.

Return Value:

    STATUS_SUCCESS - WPBT table found

    STATUS_NOT_FOUND - WPBT table not found in low memory on PCAT machine.

--*/

{

    PACPI_WPBT_TABLE PWpbt = NULL;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    ULONG WpbtTableLength;

    *WpbtTable = NULL;
    *WpbtLength = 0;

    //
    // Search memory for the legacy table.
    //

    Status = BlFwGetSystemTable(FW_TABLE_WPBT, &PhysicalAddress);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // First we need to map the header to obtain the table's length, then
    // remap the entire table.
    //

    Status = BlMmMapPhysicalAddress(&PWpbt,
                                    PhysicalAddress,
                                    sizeof(ACPI_WPBT_TABLE),
                                    MEMORY_ATTRIBUTE_NONE);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Remap the entire table.
    //

    WpbtTableLength = PWpbt->Header.Length;
    BlMmUnmapVirtualAddress(PWpbt, sizeof(ACPI_WPBT_TABLE));
    Status = BlMmMapPhysicalAddress(&PWpbt,
                                    PhysicalAddress,
                                    WpbtTableLength,
                                    MEMORY_ATTRIBUTE_NONE);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    *WpbtTable = PWpbt;
    *WpbtLength = WpbtTableLength;
    return Status;
}

NTSTATUS
OslpOpenOsExtensionDatabase (
    __in ULONG Handle,
    __out PCM_KEY_NODE *OsExtensionDatabaseNode
    )

/*++

Routine Description:

    This routine returns the Key Node corresponding to the OsExtensionDatabase
    key.

Arguments:

    Handle - Supplies the handle for all subsequent references to
        the system hive.

    OsExtensionDatabaseNode - Supplies Gets the key node for OsExtensionDatabase
        key on success.

Return Value:

    STATUS_SUCCESS if the operation succeeds, otherwise an error status.

--*/

{

    static PCM_KEY_NODE CachedNode = NULL;
    PCM_KEY_NODE HiveCell;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;

    //
    // Cache the subkey:
    // HKLM\SYSTEM\CurrentControlSet\Control\OsExtensionDatabase.
    //

    if (CachedNode == NULL) {
        Status = OslGetSubkey(Handle, NULL, NULL, &HiveCell);
        if (!NT_SUCCESS(Status)) {
            goto OpenOsExtensionDatabaseEnd;
        }

        Status = OslGetSubkey(Handle, HiveCell, OslControlString, &HiveCell);
        if (!NT_SUCCESS(Status)) {
            goto OpenOsExtensionDatabaseEnd;
        }

        Status = OslGetSubkey(Handle,
                              HiveCell,
                              OslOsExtensionDatabaseString,
                             &CachedNode);

        if (!NT_SUCCESS(Status)) {
            goto OpenOsExtensionDatabaseEnd;
        }
    }

OpenOsExtensionDatabaseEnd:
    *OsExtensionDatabaseNode = CachedNode;
    return Status;
}

NTSTATUS
OslpOpenPendingOsExtensionDatabase (
    __in ULONG Handle,
    __out PCM_KEY_NODE *PendingOsExtensionDatabaseNode
    )

/*++

Routine Description:

    This routine returns the Key Node corresponding to the pending
    OsExtensionDatabase key.

Arguments:

    Handle - Supplies the handle for all subsequent references to
        the system hive.

    PendingOsExtensionDatabaseNode - Returns the key node for the pending
        OsExtensionDatabase key on success.

Return Value:

    STATUS_SUCCESS if the operation succeeds, otherwise an error status.

--*/

{

    static PCM_KEY_NODE CachedNode = NULL;
    PCM_KEY_NODE HiveCell;
    NTSTATUS Status;

    Status = STATUS_SUCCESS;

    //
    // Cache the subkey:
    // HKLM\SYSTEM\CurrentControlSet\Control\PendingDriverOperations\OsExtensionDatabase.
    //

    if (CachedNode == NULL) {
        Status = OslGetSubkey(Handle, NULL, NULL, &HiveCell);
        if (!NT_SUCCESS(Status)) {
            goto OpenPendingOsExtensionDatabaseEnd;
        }

        Status = OslGetSubkey(Handle, HiveCell, OslControlString, &HiveCell);
        if (!NT_SUCCESS(Status)) {
            goto OpenPendingOsExtensionDatabaseEnd;
        }

        Status = OslGetSubkey(Handle, HiveCell, OslPendingDriverOperationsString, &HiveCell);
        if (!NT_SUCCESS(Status)) {
            goto OpenPendingOsExtensionDatabaseEnd;
        }

        Status = OslGetSubkey(Handle,
                              HiveCell,
                              OslOsExtensionDatabaseString,
                             &CachedNode);

        if (!NT_SUCCESS(Status)) {
            goto OpenPendingOsExtensionDatabaseEnd;
        }
    }

OpenPendingOsExtensionDatabaseEnd:
    *PendingOsExtensionDatabaseNode = CachedNode;
    return Status;
}

NTSTATUS
OslLoadOsExtensions (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __in ULONG SystemHiveHandle
    )

/*++

Routine Description:

    This routine will load OS Extensions based on CSRT data and mapping
    information in the OSExtensionDatabase in the registry.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

    OsFilePath - Supplies the file path to the Operating System directory.

    OsDeviceId - Supplies the handle for the device containing the Operating
        System.

    SystemHiveHandle - Supplies the handle for all subsequent references to
        the system hive.

Return Value:

    STATUS_SUCCESS if the operation succeeds, otherwise an error status.

--*/

{

    PDESCRIPTION_HEADER Csrt;
    PBYTE CsrtCurrent;
    PBYTE CsrtEnd;
    PBYTE CsrtStart;
    CSRT_RESOURCE_GROUP_HEADER *ResourceGroup;
    NTSTATUS Status;

    InitializeListHead(&LoaderBlock->Extension->HalExtensionModuleList);

    //
    // Get a pointer to the Core System Resource Table (CSRT).
    //

    Status = OslGetAcpiTable(&Csrt, CSRT_SIGNATURE, LoaderBlock);
    if (!NT_SUCCESS(Status)) {

        //
        // Failure to find a CSRT is not a fatal error; just return.
        //

        Status = STATUS_SUCCESS;
        goto OslLoadOsExtensionsEnd;
    }

    ASSERT(Csrt != NULL);

    //
    // Iterate through each Resource Group in the CSRT.
    //

    CsrtStart = (PBYTE)Csrt;
    CsrtEnd = CsrtStart + Csrt->Length;
    CsrtCurrent = CsrtStart + sizeof(DESCRIPTION_HEADER);
    while (CsrtCurrent < CsrtEnd) {
        if ((CsrtEnd - CsrtCurrent) < sizeof(CSRT_RESOURCE_GROUP_HEADER)) {
            Status = STATUS_UNSUCCESSFUL;
            break;
        }

        ResourceGroup = (CSRT_RESOURCE_GROUP_HEADER *)CsrtCurrent;
        if (ResourceGroup->Length < sizeof(CSRT_RESOURCE_GROUP_HEADER)) {
            Status = STATUS_UNSUCCESSFUL;
            break;
        }

        Status = OslpLoadOsExtensionForResource(LoaderBlock,
                                                OsFilePath,
                                                OsDeviceId,
                                                SystemHiveHandle,
                                                CsrtStart,
                                                ResourceGroup);

        CsrtCurrent = CsrtCurrent + ResourceGroup->Length;
    }

OslLoadOsExtensionsEnd:
    return Status;
}

NTSTATUS
OslpGetResourceIdString (
    __in CSRT_RESOURCE_GROUP_HEADER *ResourceGroup,
    __in ULONG ResourceIdLength,
    __out_ecount(ResourceIdLength) PWCHAR ResourceId,
    __inout PRESOURCE_ID_ORDER ResourceIdOrder
    )

/*++

Routine Description:

    This routine iterates through the potential Resource ID strings associated
    with a Resource Group.

Arguments:

    ResourceGroup - Supplies a pointer to the Resource Group Header.

    ResourceIdLength - Supplies the length in characters of the ResourceId
        parameter. This name should be at least MAX_RESOURCE_ID_LENGTH
        characters long.

    ResourceId - Supplies a pointer to a string into which the Resource
        Identifier is stored.

    ResourceIdOrder - Supplies a pointer to a RESOURCE_ID_ORDER enum value used
        to retain state between calls to this function. To reset the iteration,
        this value should be initialized to ResourceIdOrderInit prior to
        the first call to this function.

Return Value:

    STATUS_SUCCESS if the operation succeeds.
    STATUS_NO_MORE_ENTRIES if there are no more strings to be enumerated.

--*/

{

    NTSTATUS Status;

    for (Status = STATUS_MORE_ENTRIES; Status == STATUS_MORE_ENTRIES;) {
        *ResourceIdOrder = *ResourceIdOrder + 1;
        switch (*ResourceIdOrder) {
        case ResourceIdOrderVenDevSubvenSubdevRev:

            //
            // VEN_vvvv&DEV_dddd&SUBVEN_ssss&SUBDEV_yyyy&REV_rrrr
            //

            if ((ResourceGroup->VendorId != 0) &&
                (ResourceGroup->SubvendorId != 0)) {
                Status = STATUS_SUCCESS;
                swprintf_s(ResourceId,
                           ResourceIdLength,
                           L"VEN_%.4hs&DEV_%04x&SUBVEN_%.4hs&SUBDEV_%04x&REV_%04x",
                           (LPCSTR)&ResourceGroup->VendorId,
                           ResourceGroup->DeviceId,
                           (LPCSTR)&ResourceGroup->SubvendorId,
                           ResourceGroup->SubdeviceId,
                           ResourceGroup->Revision);
            }

            break;

        case ResourceIdOrderVenDevSubvenSubdev:

            //
            // VEN_vvvv&DEV_dddd&SUBVEN_ssss&SUBDEV_yyyy
            //

            if ((ResourceGroup->VendorId != 0) &&
                (ResourceGroup->SubvendorId != 0)) {
                Status = STATUS_SUCCESS;
                swprintf_s(ResourceId,
                           ResourceIdLength,
                           L"VEN_%.4hs&DEV_%04x&SUBVEN_%.4hs&SUBDEV_%04x",
                           (LPCSTR)&ResourceGroup->VendorId,
                           ResourceGroup->DeviceId,
                           (LPCSTR)&ResourceGroup->SubvendorId,
                           ResourceGroup->SubdeviceId);
            }

            break;

        case ResourceIdOrderVenDevRev:

            //
            // VEN_vvvv&DEV_dddd&REV_rrrr
            //

            if (ResourceGroup->VendorId != 0) {
                Status = STATUS_SUCCESS;
                swprintf_s(ResourceId,
                           ResourceIdLength,
                           L"VEN_%.4hs&DEV_%04x&REV_%04x",
                           (LPCSTR)&ResourceGroup->VendorId,
                           ResourceGroup->DeviceId,
                           ResourceGroup->Revision);
            }

            break;

        case ResourceIdOrderVenDev:

            //
            // VEN_vvvv&DEV_dddd
            //

            if (ResourceGroup->VendorId != 0) {
                Status = STATUS_SUCCESS;
                swprintf_s(ResourceId,
                           ResourceIdLength,
                           L"VEN_%.4hs&DEV_%04x",
                           (LPCSTR)&ResourceGroup->VendorId,
                           ResourceGroup->DeviceId);
            }

            break;

        default:
            Status = STATUS_NO_MORE_ENTRIES;
            break;
        }
    }

    return Status;
}

NTSTATUS
OslpLoadOsExtensionForResource (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __in ULONG SystemHiveHandle,
    __in PBYTE CsrtStart,
    __in CSRT_RESOURCE_GROUP_HEADER *ResourceGroup
    )

/*++

Routine Description:

    This routine will attempt to load an OS Extension for a specific
    Resource Group using information in the OSExtensionDatabase in the registry.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

    OsFilePath - Supplies the file path to the system32 subdirectory of the
        Operating System directory.

    OsDeviceId - Supplies the handle for the device containing the Operating
        System.

    SystemHiveHandle - Supplies the handle for all subsequent references to
        the system hive.

    CsrtStart - Supplies a pointer to the start of the CSRT. This parameter is
        required to determine the offset of the ResourceGroup, which is passed
        passed to the HAL.

    ResourceGroup - Supplies a pointer to the start of the Resource Group.

Return Value:

    STATUS_SUCCESS if the operation succeeds, otherwise an error status.

--*/

{

    PBAPD_ITEM Data;
    BAPD_ITEM_DESCRIPTOR DataDescriptor;
    BOOLEAN DataDescriptorRead;
    GUID FwUpdateResultsGuid = FWUPDATE_RESULTS_GUID;
    WCHAR ModulePath[MAX_PATH];
    ULONG OffsetIntoCsrt;
    WCHAR ResourceId[MAX_RESOURCE_ID_LENGTH];
    RESOURCE_ID_ORDER ResourceIdOrder;
    PFWUPDATE_RESULTS_HEADER ResultsHeader;
    NTSTATUS Status;

    DataDescriptorRead = FALSE;
    ResultsHeader = NULL;

    //
    // Get firmware update results, if present, to use when resolving dependencies.
    //

    Status = BlpPdQueryData(&DataDescriptor, NULL, &FwUpdateResultsGuid);

    if (NT_SUCCESS(Status)) {
        DataDescriptorRead = TRUE;

        Data = (PBAPD_ITEM)(UINTN)DataDescriptor.u.PhysAddr.QuadPart;

        if (Data->DataSize >= sizeof(FWUPDATE_RESULTS_HEADER)) {
            ResultsHeader = (PFWUPDATE_RESULTS_HEADER)Add2Ptr(Data, Data->DataOffset);

            if (Data->DataSize < (sizeof(FWUPDATE_RESULTS_HEADER) +
                                  ((ResultsHeader->Results - 1) * sizeof(FIRMWARE_UPDATE_INFORMATION)))) {
                ResultsHeader = NULL;
            }
        }
    }

    //
    // If firmware update results were not found, or there was an error
    // retrieving them, that is ok.
    //

    Status = STATUS_SUCCESS;

    //
    // Enumerate over the various ResourceId formats that can be generated
    // from this ResourceGroup's header information, and try to find a registry
    // entry for one of them to map it to a HAL Extension Module.
    //

    ResourceIdOrder = ResourceIdOrderInit;
    for (;;) {

        //
        // Get the next ResourceId string associated with this
        // Resource Group.
        //

        Status = OslpGetResourceIdString(ResourceGroup,
                                         _countof(ResourceId),
                                         ResourceId,
                                         &ResourceIdOrder);

        if (Status == STATUS_NO_MORE_ENTRIES) {

            //
            // There are no more Resource ID strings to be searched on. This
            // means that we've exhausted searching the registry for a matching
            // Resource ID for this Resource Group. Failure to find a mapping
            // from a Resource Group to a HAL Extension Module is not an error.
            //

            Status = STATUS_SUCCESS;
            break;

        } else if (!NT_SUCCESS(Status)) {

            //
            // Any other error is unexpected and should return an error.
            //

            ASSERT (FALSE);

            goto OslpLoadOsExtensionForResourceEnd;
        }

        //
        // Try to map the Resource ID string to a Module Path via the
        // OsExtensionDatabase subtree in the registry
        //

        Status = OslpGetOsExtensionModulePath(SystemHiveHandle,
                                              ResourceId,
                                              _countof(ModulePath),
                                              ModulePath,
                                              ResultsHeader);

        if (!NT_SUCCESS(Status)) {

            //
            // There was no mapping for this ResourceId. This is not an error.
            // Try again with the next ResourceId string.
            //

            continue;
        }

        //
        // Calculate the Offset into the CSRT of this Resource Group. This
        // value will be passed to the HAL to enable it to match each
        // HAL extension instance with its associated Resource Group.
        //

        OffsetIntoCsrt = (ULONG)(((PBYTE)ResourceGroup) - CsrtStart);
        Status = OslpLoadHalExtensionFromModulePath(LoaderBlock,
                                                    OsFilePath,
                                                    OsDeviceId,
                                                    ModulePath,
                                                    OffsetIntoCsrt);

        if (!NT_SUCCESS(Status)) {
            goto OslpLoadOsExtensionForResourceEnd;
        }

        break;
    }

OslpLoadOsExtensionForResourceEnd:

    if (DataDescriptorRead != FALSE) {
        BlpPdReleaseData(&DataDescriptor);
    }

    return Status;
}

NTSTATUS
OslpGetOsExtensionModulePath (
    __in ULONG Handle,
    __in __nullterminated PWCHAR ResourceId,
    __in ULONG ModulePathLength,
    __out_ecount(ModulePathLength) PWCHAR ModulePath,
    __in_opt PFWUPDATE_RESULTS_HEADER FirmwareUpdateResults
    )

/*++

Routine Description:

    This routine uses the OSExtensionDatabase registry subtree to map a
    ResourceId to an OS Extension Module path.

Arguments:

    Handle - Supplies the handle for all subsequent references to
        the system hive.

    ResourceId - Supplies a pointer to a string containing a Resource Identifier.


    ModulePathLength - Supplies the length in characters of the ModulePath
        parameter.

    ModulePath - Supplies a pointer to a string into which the module path is
        written on success.

    FirmwareUpdateResults - Supplies optional list of firmware update results
        gathered during this boot.

Return Value:

    STATUS_SUCCESS if the operation succeeds, otherwise an error status.

--*/

{

    PCM_KEY_NODE Node = NULL;
    PCM_KEY_NODE HiveCell;
    NTSTATUS Status;

    //
    // Open a pending OS extension update, if dependencies are met.
    //

    if ((!NT_SUCCESS(OslpOpenPendingOsExtensionDatabase(Handle, &Node))) ||
        (OslAreDriverDependenciesSatisfied(Handle,
                                           Node,
                                           ResourceId,
                                           OslIsFirmwareDependencySatisfied,
                                           FirmwareUpdateResults) == FALSE)) {
        Node = NULL;
    }

    //
    // Fall back to the primary OS Extension database if no pending update was
    // found.
    //

    if (Node == NULL) {
        Status = OslpOpenOsExtensionDatabase(Handle, &Node);
        if (!NT_SUCCESS(Status)) {
            goto OslpGetOsExtensionModulePathEnd;
        }
    }

    Status = OslGetSubkey(Handle, Node, ResourceId, &HiveCell);
    if (!NT_SUCCESS(Status)) {
        goto OslpGetOsExtensionModulePathEnd;
    }

    //
    // Try to query the HalExtension string value. That value will be the
    // module path of the HAL Extension.
    //

    Status = OslGetStringValue(Handle,
                               HiveCell,
                               OslHalExtensionString,
                               ModulePathLength,
                               ModulePath
                               );

OslpGetOsExtensionModulePathEnd:
    return Status;
}

NTSTATUS
OslpLoadHalExtensionFromModulePath (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __in __nullterminated PWCHAR ModulePath,
    __in ULONG OffsetIntoCsrt
    )

/*++

Routine Description:

    This routine determines if a HAL Extension Module has already been
    loaded, and if not, loads the module and creates an entry in the module
    list. The routine then adds an entry to the instance list providing the
    offset into the csrt at which the Resource Group Header for this
    instance of the module is located.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

    OsFilePath - Supplies the file path to the system32 subdirectory of the
        Operating System directory.

    OsDeviceId - Supplies the handle for the device containing the Operating
        System.

    ModulePath - Supplies a pointer to a string into which the module name
        is written on success.

    OffsetIntoCsrt - Supplies the byte offset from the start of the
        Core System Resource Table (CSRT) at which the Resource Group
        associated with this instance of the OS Extension is located. This
        value is passed on to the HAL so it can provide a pointer to the
        Resource Group to the OS Extension during initialization.

Return Value:

    STATUS_SUCCESS if the operation succeeds, otherwise an error status.

--*/

{

    BOOLEAN BoolRet;
    PHAL_EXTENSION_INSTANCE_ENTRY InstanceEntry;
    PLIST_ENTRY ListEntry;
    PHAL_EXTENSION_MODULE_ENTRY ModuleEntry;
    PHAL_EXTENSION_MODULE_ENTRY ModuleEntrySearch;
    PKLDR_DATA_TABLE_ENTRY ModuleInfo;
    PWSTR ModuleName;
    CHAR ModuleNameAnsi[MAX_MODULE_NAME_LENGTH];
    size_t ModuleNameLen;
    CHAR ModulePathAnsi[MAX_PATH];
    size_t ModulePathBaseLen;
    size_t ModulePathLen;
    USHORT OsFilePathOffset;
    NTSTATUS Status;

    //
    // See if this extension is already on the Hal Extension Module list.
    //

    ModuleName = wcsrchr(ModulePath, L'\\');

    if (ModuleName == NULL) {

        //
        // The module path is just a file name so it is also the module name.
        //

        ModuleName = ModulePath;
    } else {

        //
        // The module name is the file name at the end of the path.  Increment
        // past the path separator.
        //

        ModuleName++;
    }

    BlCopyWcharStringToString(ModuleNameAnsi, ModuleName);
    BlCopyWcharStringToString(ModulePathAnsi, ModulePath);
    ModuleEntry = NULL;
    for (ListEntry = LoaderBlock->Extension->HalExtensionModuleList.Flink;
         ListEntry != &(LoaderBlock->Extension->HalExtensionModuleList);
         ListEntry = ListEntry->Flink) {

        ModuleEntrySearch = CONTAINING_RECORD(ListEntry,
                                              HAL_EXTENSION_MODULE_ENTRY,
                                              ListEntry);

        if (_stricmp(ModuleEntrySearch->ModuleName, ModuleNameAnsi) == 0) {
            ModuleEntry = ModuleEntrySearch;
            break;
        }
    }

    if (ModuleEntry == NULL) {

        //
        // If the extension wasn't already on the Hal Extension Module list, add
        // it to that list and then try to load it.
        //
        // The ModuleEntry structure is still placed on the list even if the
        // load attempt fails. Code in the HAL can examine the ModuleLoadStatus
        // value to determine the source of the failure.
        //

        ModuleNameLen = strlen(ModuleNameAnsi) + 1;
        ModulePathLen = strlen(ModulePathAnsi) + 1;
        ModuleEntry = BlMmAllocateHeap(sizeof(*ModuleEntry) + ModuleNameLen + ModulePathLen);
        if (ModuleEntry == NULL) {
            Status = STATUS_NO_MEMORY;
            goto OslLoadHalExtensionFromModulePathEnd;
        }

        ModuleEntry->ModuleName = (PCHAR)(ModuleEntry+1);
        strcpy_s(ModuleEntry->ModuleName, ModuleNameLen, ModuleNameAnsi);
        ModuleEntry->ModulePath = ModuleEntry->ModuleName + ModuleNameLen;
        strcpy_s(ModuleEntry->ModulePath, ModulePathLen, ModulePathAnsi);
        ModuleEntry->HalExtensionInfo = NULL;
        ModuleEntry->ModuleLoadStatus = STATUS_UNSUCCESSFUL;
        InitializeListHead(&ModuleEntry->HalExtensionInstanceList);
        InsertTailList(&LoaderBlock->Extension->HalExtensionModuleList,
                       &ModuleEntry->ListEntry);

        //
        // All module files must live under \SystemRoot\system32.  They can be
        // specified via a fully qualified path, or as a path relative to
        // \SystemRoot\system32.
        //
        if (ModulePath[0] == L'\\') {
            ModulePathBaseLen = wcslen(L"\\SystemRoot\\system32\\") + 1;

            if ((ModulePathLen > ModulePathBaseLen) &&
                (_wcsnicmp(ModulePath,
                           L"\\SystemRoot\\system32\\",
                           ModulePathBaseLen - 1) == 0)) {
                ModulePath += ModulePathBaseLen - 1;
            } else {
                Status = STATUS_OBJECT_PATH_INVALID;
                goto OslLoadHalExtensionFromModulePathEnd;
            }
        }

        OsFilePathOffset = OsFilePath->Length;
        BoolRet = BlAppendUnicodeToString(OsFilePath, ModulePath);
        if (BoolRet == FALSE) {
            Status = STATUS_NO_MEMORY;
            goto OslLoadHalExtensionFromModulePathEnd;
        }

        Status = OslLoadImage(OsDeviceId,
                              OSL_MEMORY_TYPE_SYSTEM_CODE,
                              OsFilePath->Buffer,
                              ModuleName,
                              NULL,
                              NULL,
                              0,
                              &LoaderBlock->LoadOrderListHead,
                              &ModuleInfo,
                              0,
                              0,
                              IMAGE_FLAGS_NONE,
                              OSL_IMAGE_FLAGS_LOAD_IMPORTS |
                              OSL_IMAGE_FLAGS_HAL_EXTENSION,
                              NULL);

        OsFilePath->Length = OsFilePathOffset;
        OsFilePath->Buffer[OsFilePathOffset / sizeof(WCHAR)] = L'\0';
        ModuleEntry->ModuleLoadStatus = Status;
        if (NT_SUCCESS(Status)) {

            //
            // Mark this module so sysload won't try to relocate it.
            //

            ModuleInfo->Flags |= LDRP_DONT_RELOCATE;
            ModuleEntry->HalExtensionInfo = ModuleInfo;
        }
    } else {

        //
        // Verify that there are not two extensions listed with the same module
        // name but different paths.
        //

        ASSERT(_stricmp(ModuleEntry->ModulePath, ModulePathAnsi) == 0);
    }

    //
    // Create an entry on the Hal Extension Instance List.
    //

    InstanceEntry = BlMmAllocateHeap(sizeof(*InstanceEntry));
    if (InstanceEntry == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OslLoadHalExtensionFromModulePathEnd;
    }

    InstanceEntry->OffsetIntoCsrt = OffsetIntoCsrt;
    InsertTailList(&ModuleEntry->HalExtensionInstanceList,
                   &InstanceEntry->ListEntry);

    Status = STATUS_SUCCESS;

OslLoadHalExtensionFromModulePathEnd:
    return Status;
}


