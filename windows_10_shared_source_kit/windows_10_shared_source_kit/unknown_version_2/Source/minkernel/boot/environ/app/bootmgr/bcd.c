/*++

Copyright (c) Microsoft Corporation

Module Name:

    bcd.c

Abstract:

    This module provides the implementation of the reading and parsing of boot
    configuration data.

--*/

//
// ------------------------------------------------------------------- Includes
//

#include <initguid.h>
#include "bootmgr.h"

//
// -------------------------------------------------------------------- Globals
//

//
// Stores a pointer to the currently selected boot entry.
//

PBOOT_APPLICATION_ENTRY BmpSelectedBootEntry = NULL;

//
// ----------------------------------------------------------------- Prototypes
//

NTSTATUS
BmpPopulateBootEntryList (
    __in HANDLE DataStoreHandle,
    __in_ecount(*EntryCount) GUID *IdentifierArray,
    __in ULONG Attributes,
    __out_ecount(*EntryCount) PBOOT_APPLICATION_ENTRY *BootEntryList,
    __inout PULONG EntryCount
    );

NTSTATUS
BmpIsValidAssociatedOs (
    _In_ PGUID WinReIdentifier,
    _In_ PBOOT_APPLICATION_ENTRY LoaderEntry,
    _Out_ PBOOLEAN IsAssociateOs
    );

NTSTATUS
BmpGetBootDisplayOrder (
    _In_ HANDLE DataStoreHandle,
    _Outptr_result_buffer_(*DisplayOrderCount) PGUID *DisplayOrder,
    _Out_ ULONG *DisplayOrderCount
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
BmOpenDataStore (
    __out PHANDLE DataStoreHandle
    )

/*++

Routine Description:

    This routine opens the boot configuration data store containing the settings
    and boot entries to be processed by this application. When booting from
    network and the BINL reply packet does not contain path to Bcd file, WDS
    server is contacted to get the path to Bcd file.

Arguments:

    DataStoreHandle - Supplies a pointer to a variable that receives a handle
        to the opened data store.

Return Value:

    STATUS_SUCCESS if the data store was opened successfully.

    STATUS_NO_MEMORY if there is a memory allocation failure.

    Another error status code if the BCD API fails to open the store.

--*/

{

    PBOOT_ENVIRONMENT_DEVICE BcdDevice;
    PWCHAR BcdFilePath;
    BOOLEAN BcdFilePathAllocated;
    ULONG BcdFilePathLength;
    ULONG BufferSize;
    PWCHAR FilePath;
    PPXE_INFO PxeInfo;
    NTSTATUS Status;
    UNICODE_STRING UnicodeString;

    BcdDevice = NULL;
    BcdFilePath = NULL;
    BcdFilePathAllocated = FALSE;


    //
    // The data store is in a file on the application device.  Construct a file
    // path to this file, which consists of a boot environment device
    // concatenated with the file name relative to the root of the device.
    //
    // Overrides can be specified for a test version of the boot manager.  This
    // could include a device or a filepath for the BCD data store.  Based on
    // the presence of the overrides, determine the location of the data store.
    //

    Status = BlGetApplicationOptionDevice(BCDE_BOOTMGR_TYPE_BCD_DEVICE,
                                          &BcdDevice,
                                          NULL);

    if (!NT_SUCCESS(Status)) {
        Status = BlGetApplicationOptionDevice(
                     BCDE_LIBRARY_TYPE_APPLICATION_DEVICE,
                     &BcdDevice,
                     NULL);

        if (!NT_SUCCESS(Status)) {
            goto OpenDataStoreEnd;
        }
    }

    //
    // Determine the BCD file path.
    //

    Status = BlGetApplicationOptionString(BCDE_BOOTMGR_TYPE_BCD_FILEPATH,
                                          &BcdFilePath);

    if (NT_SUCCESS(Status)) {
        BcdFilePathAllocated = TRUE;

    } else {

        //
        // For UDP device, the path to Bcd File is passed in the BINL reply
        // packet.
        //

        if (BcdDevice->DeviceType == DEVICE_UDP) {
            PxeInfo = BlNetGetPxeInfo();
            if ((PxeInfo == NULL) || (PxeInfo->PxenvEntry == NULL)) {
                Status = STATUS_NO_SUCH_DEVICE;
                goto OpenDataStoreEnd;
            }

            //
            // If path to Bcd file is in the BINL reply packet then use it.
            //

            if (PxeInfo->BcdFilePath == NULL) {
                Status = PxeGetBcdFilePath();
                if (!NT_SUCCESS(Status) && (Status != STATUS_UNSUCCESSFUL)) {
                    goto OpenDataStoreEnd;
                }

                Status = STATUS_SUCCESS;
            }

            if (PxeInfo->BcdFilePath != NULL) {
                BcdFilePath = PxeInfo->BcdFilePath;
            }

        } else {
            Status = BmFwGetBcdFilePath(&BcdFilePath, &BcdFilePathAllocated);
            if (!NT_SUCCESS(Status)) {
                goto OpenDataStoreEnd;
            }
        }
    }

    ASSERT(BcdFilePath != NULL);

    Status = RtlULongAdd((ULONG)wcslen(BcdFilePath), 1, &BcdFilePathLength);
    if (!NT_SUCCESS(Status)) {
        goto OpenDataStoreEnd;
    }

    Status = RtlULongMult(BcdFilePathLength,
                          sizeof(WCHAR),
                          &BcdFilePathLength);

    if (!NT_SUCCESS(Status)) {
        goto OpenDataStoreEnd;
    }

    Status = RtlULongAdd(BcdDevice->Size, BcdFilePathLength, &BufferSize);
    if (!NT_SUCCESS(Status)) {
        goto OpenDataStoreEnd;
    }

    FilePath = BlMmAllocateHeap(BufferSize);
    if (FilePath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OpenDataStoreEnd;
    }

    RtlCopyMemory(FilePath, BcdDevice, BcdDevice->Size);
    RtlCopyMemory(Add2Ptr(FilePath, BcdDevice->Size),
                  BcdFilePath,
                  BcdFilePathLength);

    //
    // The BCD API accepts a unicode string, since it is built with the same
    // definition for a Windows environment and the boot environment, and file
    // paths are strings in Windows.  Fake up a unicode string to pass to the
    // API.
    //

    UnicodeString.Length = (USHORT)BufferSize;
    UnicodeString.MaximumLength = (USHORT)BufferSize;
    UnicodeString.Buffer = FilePath;

    //
    // Open the data store.
    // In the short term, if the system data store is not present, exclusively
    // use boot.ini.
    //

    Status = BcdOpenStoreFromFile(&UnicodeString, DataStoreHandle);
    BlMmFreeHeap(FilePath);

OpenDataStoreEnd:
    if (BcdDevice != NULL) {
        BlMmFreeHeap(BcdDevice);
    }

    if (!NT_SUCCESS(Status)) {
        BmFatalError(BM_ERROR_CONFIGURATION_DATA,
                     (ULONG_PTR)BcdFilePath,
                     Status,
                     0);
    }

    if ((BcdFilePath != NULL) && (BcdFilePathAllocated != FALSE)) {
        BlMmFreeHeap(BcdFilePath);
    }

    return Status;
}

VOID
BmCloseDataStore (
    __in HANDLE DataStoreHandle
    )

/*++

Routine Description:

    This routine closes a handle to the boot configuration data store.

Arguments:

    DataStoreHandle - Supplies a handle to the data store to close.

Return Value:

    None.

--*/

{

    BcdCloseStore(DataStoreHandle);
    return;
}

ULONGLONG
BmGetBootMenuPolicy (
    __in_opt PBOOT_APPLICATION_ENTRY BootEntry
    )

/*++

Routine Description:

    This routine returns the boot menu policy for the currently selected boot
    application. If EMS is enabled, legacy menu policy is forced.

Arguments:

    BootEntry - Supplies a pointer to an optional boot entry to check the menu
        policy of.

Return Value:

    The boot menu policy.

--*/

{

#ifdef DISABLE_INTERNAL_POLICIES

    UNREFERENCED_PARAMETER(BootEntry);

    return MenuPolicyLegacy;

#else

    PBOOT_APPLICATION_ENTRY LocalBootEntry;
    ULONGLONG BootMenuPolicy;
    BOOLEAN EmsEnabled;
    NTSTATUS Status;

    //
    // If the boot manager has EMS enabled, revert immediately to legacy.
    //

    Status = BlGetApplicationOptionBoolean(BCDE_LIBRARY_TYPE_EMS_ENABLED,
                                           &EmsEnabled);

    if (NT_SUCCESS(Status) && (EmsEnabled != FALSE)) {
        return MenuPolicyLegacy;
    }

    //
    // Get the currently selected boot entry. This is the entry associated with
    // the application that needs Bitlocker input to launch.
    //

    if (ARGUMENT_PRESENT(BootEntry) == FALSE) {
        LocalBootEntry = BmGetSelectedBootEntry();

    } else {
        LocalBootEntry = BootEntry;
    }

    //
    // Return standard menu policy if there is no boot entry.
    //

    if (LocalBootEntry == NULL) {
        return MenuPolicyStandard;
    }

    //
    // Determine if it is an OS loader or Resume loader and the menu policy.
    //

    Status = STATUS_UNSUCCESSFUL;
    BootMenuPolicy = MenuPolicyLegacy;
    if (CHECK_FLAG(LocalBootEntry->Attributes, BOOT_ENTRY_OS_LOADER)) {
        Status = BlGetBootOptionInteger(LocalBootEntry->Options,
                                        BCDE_OSLOADER_TYPE_BOOT_MENU_POLICY,
                                        &BootMenuPolicy);

    } else if (CHECK_FLAG(LocalBootEntry->Attributes,
                          BOOT_ENTRY_OS_RESUME_LOADER)) {

        Status = BlGetBootOptionInteger(
                     LocalBootEntry->Options,
                     BCDE_RESUME_LOADER_TYPE_BOOT_MENU_POLICY,
                     &BootMenuPolicy);
    }

    if (!NT_SUCCESS(Status)) {
        BootMenuPolicy = MenuPolicyLegacy;
    }

    return BootMenuPolicy;

#endif

}

ULONGLONG
BmGetBootErrorUxType (
    __in_opt PBOOT_APPLICATION_ENTRY BootEntry,
    __in BOOLEAN NoKeyboard
    )

/*++

Routine Description:

    This routine returns the boot error ux type for the currently selected
    boot application. If EMS is enabled, legacy boot error ux type is forced.

Arguments:

    BootEntry - Supplies a pointer to an optional boot entry to check the menu
        type of.
   NoKeyboard - Supplies a boolean indicating if no keyboard is present on the
        device.

Return Value:

    The boot error ux type.

--*/

{

#ifdef DISABLE_INTERNAL_POLICIES

    UNREFERENCED_PARAMETER(BootEntry);

    return BgErrorUxLegacy;

#else

    PBOOT_APPLICATION_ENTRY LocalBootEntry;
    ULONGLONG BootErrorUxType;
    ULONGLONG BootMenuPolicy;
    BOOLEAN EmsEnabled;
    NTSTATUS Status;

    //
    // If the boot manager has EMS enabled, revert immediately to legacy.
    //

    Status = BlGetApplicationOptionBoolean(BCDE_LIBRARY_TYPE_EMS_ENABLED,
                                           &EmsEnabled);

    if (NT_SUCCESS(Status) && (EmsEnabled != FALSE)) {
        return BgErrorUxLegacy;
    }

    //
    // Get the currently selected boot entry. This is the entry associated with
    // the application that needs Bitlocker input to launch.
    //

    if (ARGUMENT_PRESENT(BootEntry) == FALSE) {
        LocalBootEntry = BmGetSelectedBootEntry();

    } else {
        LocalBootEntry = BootEntry;
    }

    //
    // Return standard error ux type if there is no boot entry.
    //

    if (LocalBootEntry == NULL) {
        return BgErrorUxStandard;
    }

    // 
    // If the system does not have a keyboard (today that means it is either a
    // Slate or a Phone), check if the boot error ux bcd flag is set to simple,
    // and if set use that value and override the menu policy set in the OS
    // Loader and Resume Loader.
    // N.B. This is enforced so the Phone can use this to override the
    // default behavior. This logic needs to be cleaned up once the ACPI
    // FADT has an unique entry to identify a Phone profile.
    //
    if (NoKeyboard) {
        Status = BlGetBootOptionInteger(LocalBootEntry->Options,
                                        BCDE_LIBRARY_TYPE_BOOT_ERROR_UX,
                                        &BootErrorUxType);
        if (NT_SUCCESS(Status)) {
            if (BootErrorUxType == BgErrorUxSimple) {
                return BgErrorUxSimple;
            }
        }
    }

    //
    // On a system with keyboard or one where the boot error ux bcd is not set,
    // OS loader or Resume loader boot menu policy will take preference over
    // boot error ux for backward compatibility purpose for the respective
    // boot applications.
    //

    Status = STATUS_UNSUCCESSFUL;
    BootMenuPolicy = MenuPolicyLegacy;
    if (CHECK_FLAG(LocalBootEntry->Attributes, BOOT_ENTRY_OS_LOADER)) {
        Status = BlGetBootOptionInteger(LocalBootEntry->Options,
                                        BCDE_OSLOADER_TYPE_BOOT_MENU_POLICY,
                                        &BootMenuPolicy);

    } else if (CHECK_FLAG(LocalBootEntry->Attributes,
                          BOOT_ENTRY_OS_RESUME_LOADER)) {
        Status = BlGetBootOptionInteger(
                     LocalBootEntry->Options,
                     BCDE_RESUME_LOADER_TYPE_BOOT_MENU_POLICY,
                     &BootMenuPolicy);
    } 

    if (NT_SUCCESS(Status)) {
        switch (BootMenuPolicy) {
            case MenuPolicyLegacy:
                BootErrorUxType = BgErrorUxLegacy;
                break;
            case MenuPolicyStandard:
                BootErrorUxType = BgErrorUxStandard;
                break;
            default:
                BootErrorUxType = BgErrorUxLegacy;
                break;
        }

    } else {
        Status = BlGetBootOptionInteger(LocalBootEntry->Options,
                                        BCDE_LIBRARY_TYPE_BOOT_ERROR_UX,
                                        &BootErrorUxType);
        if (!NT_SUCCESS(Status)) {
            BootErrorUxType = BgErrorUxLegacy;
        }

    }

    return BootErrorUxType;

#endif

}

NTSTATUS
BmGetOptionList (
    __in HANDLE DataStoreHandle,
    __in LPCGUID ObjectGuid,
    __out PBOOT_ENTRY_OPTION *OptionListOut
    )

/*++

Routine Description:

    This routine returns the option list for a given object GUID.  This list
    includes the options from objects inherited by the given object, as long
    as the object does not contain an option of the same type itself.  It also
    unpacks any options contained in objects specified as additional
    options for a device option.

Arguments:

    DataStoreHandle - Supplies a handle to the data store to query for options.

    ObjectGuid - Supplies the identifier of the object to query for options.

    OptionListOut - Supplies an optional buffer to populate with the options for
        the specified object.

Return Value:

    STATUS_SUCCESS if the option list is populated successfully.

    STATUS_NO_MEMORY if there is a memory allocation failure.

    Another error status code if there is an error opening the requested object
        or enumerating its elements.

--*/

{

    PBOOT_ENTRY_OPTION AdditionalOptions;
    ULONG BufferSize;
    PBOOT_ENTRY_OPTION CurrentOption;
    PVOID Data;
    PBCD_ELEMENT_DESCRIPTION Description;
    PBCDE_DEVICE DeviceElement;
    PBOOT_ENTRY_OPTION DeviceOptions;
    ULONG ElementCount;
    PBCD_ELEMENT Elements;
    ULONG Flags;
    GUID Identifier;
    ULONG Index;
    HANDLE ObjectHandle;
    PBOOT_ENTRY_OPTION OptionList;
    PBOOT_ENTRY_OPTION PreviousOption;
    ULONG RequiredSize;
    NTSTATUS Status;
    BCDE_DATA_TYPE Type;

    Elements = NULL;
    ObjectHandle = NULL;
    OptionList = NULL;

    //
    // Open the object.
    //

    Status = BcdOpenObject(DataStoreHandle, ObjectGuid, &ObjectHandle);
    if (!NT_SUCCESS(Status)) {
        goto GetOptionListEnd;
    }

    //
    // Retrieve all of the extended elements for this object.
    //

    Flags = BCD_FLAGS_ENUMERATE_INHERITED_OBJECTS;
    Flags |= BCD_FLAGS_ENUMERATE_DEVICE_OPTIONS;
    Flags |= BCD_FLAGS_OBSERVE_PRECEDENCE;
    BufferSize = 0;
    Status = BcdEnumerateAndUnpackElements(DataStoreHandle,
                                           ObjectHandle,
                                           Flags,
                                           NULL,
                                           &BufferSize,
                                           &ElementCount);

    if (Status != STATUS_BUFFER_TOO_SMALL) {
        if (NT_SUCCESS(Status)) {
            Status = STATUS_INVALID_PARAMETER;
        }

        goto GetOptionListEnd;
    }

    Elements = BlMmAllocateHeap(BufferSize);
    if (Elements == NULL) {
        Status = STATUS_NO_MEMORY;
        goto GetOptionListEnd;
    }

    Status = BcdEnumerateAndUnpackElements(DataStoreHandle,
                                           ObjectHandle,
                                           Flags,
                                           Elements,
                                           &BufferSize,
                                           &ElementCount);

    if (!NT_SUCCESS(Status)) {
        goto GetOptionListEnd;
    }

    //
    // Iterate through each element, determining the size of the buffer
    // required to hold a boot option corresponding to the element.
    //

    RequiredSize = 0;
    for (Index = 0; Index < ElementCount; Index += 1) {

        //
        // Most elements require space for a fixed-size header and then the
        // variable-size data.
        //

        MARK_SAFE_ADDITION(RequiredSize, sizeof(BOOT_ENTRY_OPTION));
        RequiredSize += sizeof(BOOT_ENTRY_OPTION);
        MARK_SAFE_ADDITION(RequiredSize, Elements[Index].Description->DataSize);
        RequiredSize += Elements[Index].Description->DataSize;
    }

    OptionList = BlMmAllocateHeap(RequiredSize);
    if (OptionList == NULL) {
        Status = STATUS_NO_MEMORY;
        goto GetOptionListEnd;
    }

    RtlZeroMemory(OptionList, RequiredSize);

    //
    // Walk the element list again, creating a corresponding boot option for
    // each.
    //

    PreviousOption = NULL;
    CurrentOption = OptionList;
    for (Index = 0; Index < ElementCount; Index += 1) {
        Description = Elements[Index].Description;
        Type = Description->Type;
        if (BlBootOptionExists(OptionList, Type) == FALSE) {

            ASSERT((PVOID)CurrentOption < (Add2Ptr(OptionList, RequiredSize)));

            if (PreviousOption != NULL) {
                PreviousOption->NextOptionOffset =
                    (ULONG)PtrOffset(OptionList, CurrentOption);
            }

            //
            // Copy the element description and data consecutively into the
            // boot option, and record the offset to the data.
            //

            CurrentOption->Type = Type;
            CurrentOption->DataSize = Description->DataSize;
            Data = Add2Ptr(CurrentOption, sizeof(BOOT_ENTRY_OPTION));
            RtlCopyMemory(Data, Elements[Index].Data, Description->DataSize);
            CurrentOption->DataOffset = (ULONG)PtrOffset(CurrentOption, Data);

            //
            // If this is a device with associated options, copy those options
            // now.
            //

            if (GET_BCDE_DATA_FORMAT(Type) == BCDE_FORMAT_DEVICE) {
                DeviceElement = Data;
                RtlCopyMemory(&Identifier,
                              &DeviceElement->AdditionalOptions,
                              sizeof(GUID));

                Status = BmGetOptionList(DataStoreHandle,
                                         &Identifier,
                                         &DeviceOptions);

                if (NT_SUCCESS(Status)) {
                    AdditionalOptions = Add2Ptr(Data, Description->DataSize);
                    RtlCopyMemory(AdditionalOptions,
                                  DeviceOptions,
                                  BlGetBootOptionListSize(DeviceOptions));

                    BlMmFreeHeap(DeviceOptions);
                    CurrentOption->AssociatedOptionsOffset =
                        (ULONG)PtrOffset(CurrentOption, AdditionalOptions);
                }
            }

            //
            // Update the pointer to the current location in the caller's
            // buffer, and continue the iteration.
            //

            PreviousOption = CurrentOption;
            CurrentOption = Add2Ptr(CurrentOption,
                                    BlGetBootOptionSize(CurrentOption));
        }
    }

    //
    // Populate the amount of buffer consumed by the options, and return.
    //

    *OptionListOut = OptionList;
    Status = STATUS_SUCCESS;

GetOptionListEnd:
    if (!NT_SUCCESS(Status)) {
        if (OptionList != NULL) {
            BlMmFreeHeap(OptionList);
        }
    }

    if (Elements != NULL) {
        BlMmFreeHeap(Elements);
    }

    if (ObjectHandle != NULL) {
        BcdCloseObject(ObjectHandle);
    }

    return Status;
}

BOOLEAN
BmIsMultiBootSystem (
    __out_opt PULONG BootEntryCount
    )

/*++

Routine Description:

    This routine determines if the current system has multiple options in the
    display order.

Arguments:

    BootEntryCount - Supplies a pointer to a variable that receives the number
        of boot entries on the system. This parameter is optional.

Return Value:

    TRUE if there are multiple boot entries. FALSE otherwise.

--*/

{

    GUID *DisplayOrder;
    ULONG DisplayOrderCount = 0;
    BOOLEAN MultiBootSystem;
    NTSTATUS Status;

    DisplayOrder = NULL;
    MultiBootSystem = FALSE;
    Status = BlGetApplicationOptionGuidList(BCDE_BOOTMGR_TYPE_DISPLAY_ORDER,
                                            &DisplayOrder,
                                            &DisplayOrderCount);

    if (NT_SUCCESS(Status) && (DisplayOrderCount > 1)) {
        MultiBootSystem = TRUE;
    }

    if (ARGUMENT_PRESENT(BootEntryCount) != FALSE) {
        *BootEntryCount = DisplayOrderCount;
    }

    return MultiBootSystem;
}

NTSTATUS
BmPurgeOption (
    __in HANDLE DataStoreHandle,
    __in LPCGUID ObjectGuid,
    __in BCDE_DATA_TYPE DataType
    )

/*++

Routine Description:

    This routine purges an option from the given object's permanent storage in
    the configuration data store.

Arguments:

    DataStoreHandle - Supplies a handle to the data store containing the object
        for which the option is to be purged.

    ObjectGuid - Supplies a pointer to the identifier of the object for which
        the option is to be purged.

    DataType - Supplies the data type of the option to be purged.

Return Value:

    STATUS_SUCCESS if the option was purged.  Success is returned even if the
        delete of the option failed, since the option may not have existed in
        the store.

    An error status code if the given object could not be opened in the data
        store.

--*/

{

    HANDLE ObjectHandle;
    NTSTATUS Status;

    ObjectHandle = NULL;
    Status = BcdOpenObject(DataStoreHandle, ObjectGuid, &ObjectHandle);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    BcdDeleteElement(ObjectHandle, DataType);
    BcdCloseObject(ObjectHandle);
    return STATUS_SUCCESS;
}

NTSTATUS
BmEnumerateBootEntries (
    __in HANDLE DataStoreHandle,
    __deref_out_ecount(*EntryCount) PBOOT_APPLICATION_ENTRY **EntryArrayOut,
    __out PULONG EntryCount
    )

/*++

Routine Description:

    Enumerates the boot entries to be displayed by this application.

Arguments:

    DataStoreHandle - Supplies a handle to the data store to query for boot
        entries.

    EntryArrayOut - Supplies a pointer to a variable that receives an array of
        pointers to each boot entry in the list, ordered by their display order.

    EntryCount - Supplies a pointer to a variable that receives the number of
        entries in the entry array.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if there is a memory allocation failure.

    Another error status code if boot configuration data could not be read
        successfully.

--*/

{

    GUID DefaultObject;    
    GUID *DisplayOrder;
    BOOLEAN DisplayOrderAllocated;
    ULONG DisplayOrderCount;
    PBOOT_APPLICATION_ENTRY *EntryArray;
    ULONG Index;
    ULONG RequiredSize;
    NTSTATUS Status;
    ULONG TotalCount;
    DisplayOrder = NULL;
    DisplayOrderAllocated = TRUE;
    EntryArray = NULL;
    TotalCount = 0;

    //
    // Get the display order.  If no display order is specified, create a single
    // boot option for the default entry.  If the default entry is not
    // specified, there are no boot entries to enumerate.
    //

    Status = BlGetApplicationOptionGuidList(BCDE_BOOTMGR_TYPE_DISPLAY_ORDER,
                                            &DisplayOrder,
                                            &DisplayOrderCount);

    if (!NT_SUCCESS(Status)) {
        Status = BlGetApplicationOptionGuid(BCDE_BOOTMGR_TYPE_DEFAULT_OBJECT,
                                            &DefaultObject);

        if (!NT_SUCCESS(Status)) {
            *EntryArrayOut = NULL;
            *EntryCount = 0;
            Status = STATUS_SUCCESS;
            goto EnumerateBootEntriesEnd;
        }

        DisplayOrder = &DefaultObject;
        DisplayOrderCount = 1;
        DisplayOrderAllocated = FALSE;
    }

    //
    // Allocate a buffer to hold pointers to each boot entry.
    //

    TotalCount = DisplayOrderCount;
    RequiredSize = TotalCount * sizeof(PBOOT_APPLICATION_ENTRY);
    EntryArray = BlMmAllocateHeap(RequiredSize);
    if (EntryArray == NULL) {
        Status = STATUS_NO_MEMORY;
        goto EnumerateBootEntriesEnd;
    }

    RtlZeroMemory(EntryArray, RequiredSize);

    //
    // First populate the BCD entries, then boot.ini entries if they exist.
    //

    if (DisplayOrderCount != 0) {
        Status = BmpPopulateBootEntryList(DataStoreHandle,
                                          DisplayOrder,
                                          BOOT_ENTRY_DISPLAY,
                                          EntryArray,
                                          &DisplayOrderCount);

        if (!NT_SUCCESS(Status)) {
            goto EnumerateBootEntriesEnd;
        }
    }


    *EntryArrayOut = EntryArray;
    *EntryCount = DisplayOrderCount;
    Status = STATUS_SUCCESS;

EnumerateBootEntriesEnd:
    if ((DisplayOrderAllocated != FALSE) && (DisplayOrder != NULL)) {
        BlMmFreeHeap(DisplayOrder);
    }

    if (!NT_SUCCESS(Status)) {
        if (EntryArray != NULL) {
            for (Index = 0; Index < TotalCount; Index += 1) {
                if (EntryArray[Index] != NULL) {
                    BlMmFreeHeap(EntryArray[Index]);
                }
            }

            BlMmFreeHeap(EntryArray);
        }
    }

    return Status;
}

NTSTATUS
BmEnumerateToolEntries (
    __in HANDLE DataStoreHandle,
    __deref_out_ecount(*EntryCount) PBOOT_APPLICATION_ENTRY **EntryArrayOut,
    __out PULONG EntryCount
    )

/*++

Routine Description:

    Enumerates the tool application entries to be displayed by the boot
    manager.

Arguments:

    DataStoreHandle - Supplies a handle to the data store to query for boot
        entries.

    EntryArrayOut - Supplies a pointer to a variable that receives an array of
        pointers to each tool application entry in the list, ordered by their
        display order.

    EntryCount - Supplies a pointer to a variable that receives the number of
        entries in the entry array.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if there is a memory allocation failure.

    Another error status code if boot configuration data could not be read
        successfully.

--*/

{

    GUID *DisplayOrder;
    ULONG DisplayOrderCount;
    PBOOT_APPLICATION_ENTRY *EntryArray;
    ULONG Index;
    ULONG RequiredSize;
    NTSTATUS Status;

    DisplayOrder = NULL;
    EntryArray = NULL;
    DisplayOrderCount = 0;

    //
    // Get the tools display order.
    //

    Status = BlGetApplicationOptionGuidList(
                 BCDE_BOOTMGR_TYPE_TOOLS_DISPLAY_ORDER,
                 &DisplayOrder,
                 &DisplayOrderCount);

    if (!NT_SUCCESS(Status) || (DisplayOrderCount == 0)) {
        *EntryArrayOut = NULL;
        *EntryCount = 0;
        return STATUS_SUCCESS;
    }

    //
    // Allocate a buffer to hold pointers to each tool application entry.
    //

    RequiredSize = DisplayOrderCount * sizeof(PBOOT_APPLICATION_ENTRY);
    EntryArray = BlMmAllocateHeap(RequiredSize);
    if (EntryArray == NULL) {
        Status = STATUS_NO_MEMORY;
        goto EnumerateToolEntriesEnd;
    }

    RtlZeroMemory(EntryArray, RequiredSize);

    //
    // Populate the BCD entries.
    //

    Status = BmpPopulateBootEntryList(DataStoreHandle,
                                      DisplayOrder,
                                      BOOT_ENTRY_TOOL,
                                      EntryArray,
                                      &DisplayOrderCount);

    if (!NT_SUCCESS(Status)) {
        goto EnumerateToolEntriesEnd;
    }

    *EntryArrayOut = EntryArray;
    *EntryCount = DisplayOrderCount;
    Status = STATUS_SUCCESS;

EnumerateToolEntriesEnd:
    if (DisplayOrder != NULL) {
        BlMmFreeHeap(DisplayOrder);
    }

    if (!NT_SUCCESS(Status)) {
        if (EntryArray != NULL) {
            for (Index = 0; Index < DisplayOrderCount; Index += 1) {
                if (EntryArray[Index] != NULL) {
                    BlMmFreeHeap(EntryArray[Index]);
                }
            }

            BlMmFreeHeap(EntryArray);
        }
    }

    return Status;
}

NTSTATUS
BmGetBootSequence (
    __in HANDLE DataStoreHandle,
    __in_ecount(Count) GUID *IdentifierSequence,
    __in ULONG Count,
    __in ULONG Attributes,
    __deref_out_ecount(*SequenceCount) PBOOT_APPLICATION_ENTRY **BootEntrySequence,
    __out PULONG SequenceCount
    )

/*++

Routine Description:

    This routine retrieves a sequence of boot entries described by a boot
    option.  The boot option must be an object list.  Each object in the list
    is extracted to the corresponding boot entry.

Arguments:

    DataStoreHandle - Supplies a handle to the data store containing boot
        entry configuration.

    IdentifierSequence - Supplies an array of GUIDs identifying each entry in
        the sequence.

    Count - Supplies the number of elements in the IdentifierSequence array.

    Attributes - Supplies attributes to be applied to each boot entry.

    BootEntrySequence - Supplies a pointer to a variable that receives an array
        of boot entry pointers.

    SequenceCount - Supplies a pointer to a variable that receives the number
        of boot entries contained in the boot entry sequence.

Return Value:

    STATUS_SUCCESS if the boot sequence is retrieved.

    STATUS_NO_MEMORY if there is a memory allocation failure.

    STATUS_NOT_FOUND if there is no boot sequence.

    Another error status code if there is an error communicating with the
        data store.

--*/

{

    PBOOT_APPLICATION_ENTRY *EntryList;
    NTSTATUS Status;

    EntryList = NULL;

    //
    // Allocate a buffer to hold an array of pointers to boot entries.
    //

    EntryList = BlMmAllocateHeap(Count * sizeof(PBOOT_APPLICATION_ENTRY));
    if (EntryList == NULL) {
        Status = STATUS_NO_MEMORY;
        goto GetBootSequenceEnd;
    }

    //
    // Populate the array with boot entries corresponding to each entry in
    // the boot sequence.
    //

    Status = BmpPopulateBootEntryList(DataStoreHandle,
                                      IdentifierSequence,
                                      Attributes,
                                      EntryList,
                                      &Count);

    if (!NT_SUCCESS(Status)) {
        goto GetBootSequenceEnd;
    }

    *BootEntrySequence = EntryList;
    *SequenceCount = Count;
    Status = STATUS_SUCCESS;

GetBootSequenceEnd:
    if (!NT_SUCCESS(Status)) {
        if (EntryList != NULL) {
            BlMmFreeHeap(EntryList);
        }
    }

    return Status;
}

NTSTATUS
BmpPopulateBootEntryList (
    __in HANDLE DataStoreHandle,
    __in_ecount(*EntryCount) GUID *IdentifierArray,
    __in ULONG Attributes,
    __out_ecount(*EntryCount) PBOOT_APPLICATION_ENTRY *BootEntryList,
    __inout PULONG EntryCount
    )

/*++

Routine Description:

    This routine populates a buffer with an array of pointers to boot entries
    that correspond to the given entry identifiers.

Arguments:

    DataStoreHandle - Supplies a handle to the data store containing boot
        entry configuration.

    IdentifierArray - Supplies an array of boot entry identifiers to be
        converted to complete boot entries.

    Attributes - Supplies attributes to be applied to each boot entry.

    BootEntryList - Supplies a pointer to a variable that receives an array
        of boot entry pointers.  Each entry pointer is independently allocated
        from heap.  The caller is responsible for freeing each entry pointer.

    EntryCount - Supplies a pointer to the number of available entries in the
        boot entry list.  This variable receives the number of entries
        consumed by this routine.

Return Value:

    STATUS_SUCCESS if the boot entry list is populated successfully.

    STATUS_NO_MEMORY if there is a memory allocation failure.

--*/

{

    ULONG ApplicationType;
    PBOOT_APPLICATION_ENTRY BootEntry;
    PWCHAR DefaultLoaderPath;
    BCD_OBJECT_DESCRIPTION Description;
    ULONG EntryIndex;
    NTSTATUS ErrorStatus;
    GUID Identifier;
    ULONG Index;
    ULONG OptionListSize;
    NTSTATUS OptionStatus;
    PBOOT_ENTRY_OPTION Options;
    BOOLEAN PathOptionExists;
    ULONG RequiredSize;
    BOOLEAN SoftReboot;
    NTSTATUS Status;
    BOOLEAN WinPEOptionValue;

    ErrorStatus = STATUS_NOT_FOUND;
    Options = NULL;

    //
    // Fill in each boot entry in order.
    //

    EntryIndex = 0;
    for (Index = 0; Index < *EntryCount; Index += 1) {
        BootEntry = NULL;

        //
        // Get the option list for the boot entry, and make sure it contains
        // the required elements.
        //

        Status = BmGetOptionList(DataStoreHandle,
                                 &IdentifierArray[Index],
                                 &Options);

        if (!NT_SUCCESS(Status)) {
            goto NextOption;
        }

        if (BlBootOptionExists(Options,
                               BCDE_LIBRARY_TYPE_APPLICATION_DEVICE) == FALSE) {

            Status = STATUS_UNSUCCESSFUL;
            goto NextOption;
        }

        if (BlBootOptionExists(Options,
                               BCDE_LIBRARY_TYPE_DESCRIPTION) == FALSE) {

            Status = STATUS_UNSUCCESSFUL;
            goto NextOption;
        }

        //
        // Allocate a buffer for the boot entry.
        //

        OptionListSize = BlGetBootOptionListSize(Options);
        RequiredSize = sizeof(BOOT_APPLICATION_ENTRY) + OptionListSize;
        BootEntry = BlMmAllocateHeap(RequiredSize);
        if (BootEntry == NULL) {
            Status = STATUS_NO_MEMORY;
            goto PopulateBootEntryListEnd;
        }

        BootEntryList[EntryIndex] = BootEntry;

        //
        // Copy the identifier and options, zeroing the rest of the identifier.
        //

        RtlZeroMemory(BootEntry, sizeof(BOOT_APPLICATION_ENTRY));
        Identifier = IdentifierArray[Index];
        RtlCopyMemory(&BootEntry->Identifier, &Identifier, sizeof(GUID));
        BootEntry->Options = Add2Ptr(BootEntry, sizeof(BOOT_APPLICATION_ENTRY));
        RtlCopyMemory(BootEntry->Options,
                      Options,
                      OptionListSize);

        //
        // Setup the boot entry attributes and indicate whether the boot
        // entry is for an OS loader or an OS resume loader.  Also, if the
        // boot entry is for an OS loader, and path element is missing, use
        // the default path.
        //

        BootEntry->Attributes = Attributes;
        Status = BmGetEntryDescription(DataStoreHandle,
                                       &Identifier,
                                       &Description);

        if (!NT_SUCCESS(Status)) {
            goto NextOption;
        }

        PathOptionExists = BlBootOptionExists(
                               Options,
                               BCDE_LIBRARY_TYPE_APPLICATION_PATH);

        if (GET_BCD_OBJECT_TYPE(Description.Type) ==
            BCD_OBJECT_TYPE_APPLICATION) {

            ApplicationType = GET_BCD_OBJECT_APPLICATION_TYPE(Description.Type);
            switch (ApplicationType) {
            case BCD_APPLICATION_TYPE_WINDOWS_BOOT_LOADER:
                SET_FLAGS(BootEntry->Attributes, BOOT_ENTRY_OS_LOADER);

                //
                // The default path depends on whether the entry is for WinPE
                // or regular boot.
                //

                if (PathOptionExists == FALSE) {
                    WinPEOptionValue = FALSE;
                    Status = BlGetBootOptionBoolean(Options,
                                                    BCDE_OSLOADER_TYPE_WINPE,
                                                    &WinPEOptionValue);

                    if ((!NT_SUCCESS(Status)) &&
                        (Status != STATUS_NOT_FOUND)) {

                        goto PopulateBootEntryListEnd;
                    }

                    if (WinPEOptionValue == FALSE) {
                        DefaultLoaderPath = BM_DEFAULT_OS_LOADER_PATH;

                    } else {
                        DefaultLoaderPath = BM_DEFAULT_WINPE_OS_LOADER_PATH;
                    }

                    Status = BlAppendBootOptionString(
                        BootEntry,
                        BCDE_LIBRARY_TYPE_APPLICATION_PATH,
                        DefaultLoaderPath);

                    if (!NT_SUCCESS(Status)) {
                        goto PopulateBootEntryListEnd;
                    }

                    PathOptionExists = TRUE;
                }

                break;

            case BCD_APPLICATION_TYPE_WINDOWS_RESUME_APPLICATION:
                SET_FLAGS(BootEntry->Attributes, BOOT_ENTRY_OS_RESUME_LOADER);
                break;

            case BCD_APPLICATION_TYPE_LEGACY_NTLDR:
                SET_FLAGS(BootEntry->Attributes, BOOT_ENTRY_NTLDR);
                break;

            case BCD_APPLICATION_TYPE_LEGACY_SETUPLDR:
                SET_FLAGS(BootEntry->Attributes, BOOT_ENTRY_SETUPLDR);
                break;

            case BCD_APPLICATION_TYPE_BOOT_SECTOR:
                SET_FLAGS(BootEntry->Attributes, BOOT_ENTRY_BOOT_SECTOR);
                break;

            case BCD_APPLICATION_TYPE_WINDOWS_BOOT_MANAGER:
                SET_FLAGS(BootEntry->Attributes, BOOT_ENTRY_BOOT_MANAGER);
                break;
            }
        }

        if (PathOptionExists == FALSE) {
            Status = STATUS_UNSUCCESSFUL;
            goto NextOption;
        }

        //
        // Set the appropriate attribute if the boot entry is for a realmode
        // code application that is to be loaded through PXE restart.
        //

        if ((GET_BCD_OBJECT_TYPE(Description.Type) ==
             BCD_OBJECT_TYPE_APPLICATION) &&
            (GET_BCD_OBJECT_IMAGE_TYPE(Description.Type) ==
             BCD_IMAGE_TYPE_REALMODE_CODE) &&
            (GET_BCD_OBJECT_APPLICATION_TYPE(Description.Type) ==
             BCD_APPLICATION_TYPE_STARTUP_MODULE)) {

            OptionStatus = BlGetBootOptionBoolean(
                               Options,
                               BCDE_STARTUP_TYPE_PXE_SOFT_REBOOT,
                               &SoftReboot);

            if (NT_SUCCESS(OptionStatus) && (SoftReboot != FALSE)) {
                SET_FLAGS(BootEntry->Attributes, BOOT_ENTRY_RESTART_LOADER);
            }
        }

NextOption:
        if (Options != NULL) {
            BlMmFreeHeap(Options);
            Options = NULL;
        }

        if (NT_SUCCESS(Status)) {
            EntryIndex += 1;

        } else {
            ErrorStatus = Status;
            if (BootEntry != NULL) {
                BlDestroyBootEntry(BootEntry);
                BootEntry = NULL;
                BootEntryList[EntryIndex] = NULL;
            }
        }
    }

    if (EntryIndex > 0) {
        *EntryCount = EntryIndex;
        Status = STATUS_SUCCESS;

    } else {
        Status = ErrorStatus;
    }

PopulateBootEntryListEnd:
    if (Options != NULL) {
        BlMmFreeHeap(Options);
    }

    return Status;
}

NTSTATUS
BmGetEntryDescription (
    __in HANDLE DataStoreHandle,
    __in GUID *Identifier,
    __out PBCD_OBJECT_DESCRIPTION Description
    )

/*++

Routine Description:

    This routine retrieves the description of boot entry with the given
    identifier.

Arguments:

    DataStoreHandle - Supplies a handle to the data store containing the boot
        entries being enumerated.

    Identifier - Supplies a pointer to the identifier of the boot entry for
        which the description is to be retrieved.

    Description - Supplies a pointer to a variable that receives the description
        of the boot entry.

Return Value:

    STATUS_SUCCESS if the description is retrieved successfully.

    An error status code as returned from the BCD API set if there is an error.

--*/

{

    HANDLE ObjectHandle;
    NTSTATUS Status;

    Status = BcdOpenObject(DataStoreHandle, Identifier, &ObjectHandle);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Status = BcdQueryObject(ObjectHandle,
                            BCD_OBJECT_DESCRIPTION_VERSION,
                            Description,
                            NULL);

    BcdCloseObject(ObjectHandle);
    return Status;
}

PBOOT_APPLICATION_ENTRY
BmGetSelectedBootEntry (
    VOID
    )

/*++

Routine Description:

    This routine returns a pointer to the boot application entry for the
    application that was selected by the boot manager's selection logic.

Arguments:

    None.

Return Value:

    A pointer to the currently selected boot entry.

--*/

{

    return BmpSelectedBootEntry;
}

VOID
BmSetSelectedBootEntry (
    __in PBOOT_APPLICATION_ENTRY BootEntry
    )

/*++

Routine Description:

    This routine sets the currently selected boot application entry. The boot
    manager application logic calls this before it begins launching a new
    application.

Arguments:

    BootEntry - Supplies a pointer to the selected boot application entry.

Return Value:

    None.

--*/

{

    BmpSelectedBootEntry = BootEntry;
    return;
}

NTSTATUS
BmFindAssociatedOsEntry (
    _In_ PGUID WinReIdentifier,
    _Outptr_ PBOOT_APPLICATION_ENTRY *AssociatedOsEntry
    )

/*++

Routine Description:

    Attempt to find an Associated OS Volume for the given
    WinRe identifier.

Arguments:

    WinReIdentifier - WinRE identifier to search for.

    AssociatedOsEntry - On success, set to the application entry of the 
        Associated OS. Otherwise NULL.

Return Value:

    STATUS_SUCCESS on success.

    STATUS_NOT_FOUND if no associated OS was found.

    Other error codes for other errors.

--*/

{
    ULONG BootEntryIndex;
    PBOOT_APPLICATION_ENTRY CurrentEntry;
    GUID CurrentWinReEntry;
    HANDLE DataStoreHandle;
    PGUID DisplayOrder;
    ULONG DisplayOrderCount;
    PBOOT_APPLICATION_ENTRY *EntryArray;
    BOOLEAN IsAssociatedOs;
    PBOOT_APPLICATION_ENTRY LocalEntryCopy;
    ULONG RequiredSize;
    ULONG SingleCount;
    NTSTATUS Status;

    *AssociatedOsEntry = NULL;
    DataStoreHandle = NULL;
    DisplayOrder = NULL;
    DisplayOrderCount = 0;
    EntryArray = NULL;
    IsAssociatedOs = FALSE;
    LocalEntryCopy = NULL;
    SingleCount = 1;
    Status = STATUS_SUCCESS;

    Status = BmOpenDataStore(&DataStoreHandle);
    if (!NT_SUCCESS(Status)) {
        goto BmFindAssociatedOsEntryEnd;
    }

    Status = BmpGetBootDisplayOrder(DataStoreHandle,
                                    &DisplayOrder,
                                    &DisplayOrderCount);
    if (!NT_SUCCESS(Status)) {
        goto BmFindAssociatedOsEntryEnd;
    }

    //
    // If there are no entries, exit out.
    //

    if (DisplayOrderCount == 0) {
        Status = STATUS_NOT_FOUND;
        goto BmFindAssociatedOsEntryEnd;
    }

    //
    // Allocate an array of application entries pointers for
    // BmpPopulateBootEntryList to fill.
    //

    RequiredSize = DisplayOrderCount * sizeof(PBOOT_APPLICATION_ENTRY);
    EntryArray = BlMmAllocateHeap(RequiredSize);
    if (EntryArray == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BmFindAssociatedOsEntryEnd;
    }

    RtlZeroMemory(EntryArray, RequiredSize);

    Status = BmpPopulateBootEntryList(DataStoreHandle,
                                      DisplayOrder,
                                      0,
                                      EntryArray,
                                      &DisplayOrderCount);
    if (!NT_SUCCESS(Status)) {
        goto BmFindAssociatedOsEntryEnd;
    }

    //
    // BmpPopulateBootEntryList has loaded the options for our
    // potential OS loaders. Iterate through and see if we have a
    // valid associated OS.
    //

    for (BootEntryIndex = 0;
         BootEntryIndex < DisplayOrderCount;
         BootEntryIndex++) {

        CurrentEntry = EntryArray[BootEntryIndex];

        Status = BmpIsValidAssociatedOs(WinReIdentifier,
                                        CurrentEntry,
                                        &IsAssociatedOs);
        if (!NT_SUCCESS(Status)) {
            goto BmFindAssociatedOsEntryEnd;
        }

        if (IsAssociatedOs != FALSE) {

            //
            // Found the Associated OS. Use BmpPopulateBootEntryList
            // again to get a copy of just that entry.
            // Copy Identifier for alignment.
            //

            RtlCopyMemory(&CurrentWinReEntry,
                          &(CurrentEntry->Identifier),
                          sizeof(GUID));

            Status = BmpPopulateBootEntryList(DataStoreHandle,
                                              &CurrentWinReEntry,
                                              0,
                                              &LocalEntryCopy,
                                              &SingleCount);
            if (!NT_SUCCESS(Status)) {
                goto BmFindAssociatedOsEntryEnd;
            }

            *AssociatedOsEntry = LocalEntryCopy;
            LocalEntryCopy = NULL;

            break;
        }
    }

    if (IsAssociatedOs == FALSE) {
        Status = STATUS_NOT_FOUND;
    }

BmFindAssociatedOsEntryEnd:

    if (DisplayOrder != NULL) {
        BlMmFreeHeap(DisplayOrder);
    }

    if (DataStoreHandle != NULL) {
        BmCloseDataStore(DataStoreHandle);
    }

    if (LocalEntryCopy != NULL) {
        BlMmFreeHeap(LocalEntryCopy);
    }

    if (EntryArray != NULL) {
        for (BootEntryIndex = 0;
             BootEntryIndex < DisplayOrderCount;
             BootEntryIndex++) {

            if (EntryArray[BootEntryIndex] != NULL) {
                BlMmFreeHeap(EntryArray[BootEntryIndex]);
            }

        }

        BlMmFreeHeap(EntryArray);
    }

    return Status;
}

NTSTATUS
BmpIsValidAssociatedOs (
    _In_ PGUID WinReIdentifier,
    _In_ PBOOT_APPLICATION_ENTRY LoaderEntry,
    _Out_ PBOOLEAN IsAssociateOs
    )

/*++

Routine Description:

    Checks if the given LoaderEntry is a valid OS loader and uses
    the given WinReIdentifier in its recovery sequence.

Arguments:

    WinReIdentifier - GUID identifier of WinRE.

    LoaderEntry - Potential OS loader in question.

    IsAssociatedOs - TRUE if LoaderEntry is a valid associated OS.

Return Value:

    STATUS_SUCCESS on success, error code otherwise.

--*/

{
    ULONG Index;
    PGUID RecoverySequence;
    ULONG RecoverySequenceCount;
    NTSTATUS Status;

    *IsAssociateOs = FALSE;
    RecoverySequence = NULL;
    RecoverySequenceCount = 0;
    Status = STATUS_SUCCESS;

    if (CHECK_FLAG(LoaderEntry->Attributes,
                   BOOT_ENTRY_OS_LOADER) == FALSE) {
        goto BmpIsValidAssociatedOsEnd;
    }

    Status = BlGetBootOptionGuidList(LoaderEntry->Options,
                                     BCDE_LIBRARY_TYPE_RECOVERY_SEQUENCE,
                                     &RecoverySequence,
                                     &RecoverySequenceCount);
    if (!NT_SUCCESS(Status)) {

        //
        // STATUS_NOT_FOUND indicates this isn't the Associated OS
        // we are looking for, but is not a failure.
        //

        if (Status == STATUS_NOT_FOUND) {
            Status = STATUS_SUCCESS;
        }

        goto BmpIsValidAssociatedOsEnd;
    }

    for (Index = 0;
         Index < RecoverySequenceCount;
         Index++) {

        if (RtlCompareMemory(WinReIdentifier,
                             &(RecoverySequence[Index]),
                             sizeof(GUID)) == sizeof(GUID)) {
            *IsAssociateOs = TRUE;
            break;
        }
    }

BmpIsValidAssociatedOsEnd:

    if (RecoverySequence != NULL) {
        BlMmFreeHeap(RecoverySequence);
    }

    return Status;
}

NTSTATUS
BmpGetBootDisplayOrder (
    _In_ HANDLE DataStoreHandle,
    _Outptr_result_buffer_(*DisplayOrderCount) PGUID *DisplayOrder,
    _Out_ ULONG *DisplayOrderCount
    )

/*++

Routine Description:

    Get the Boot Display order for BootMgr directly from data store,
    not from already loaded BCD settings.

Arguments:

    DataStoreHandle - Handle to BCD store.

    DisplayOrder - On success, an array of loader GUIDs.

    DisplayOrderCount - On success, the number of entries in DisplayOrder.

Return Value:

    STATUS_SUCCESS on success.

    STATUS_NOT_FOUND if no dispay order or empty display order.

    Other error code otherwise.

--*/

{
    HANDLE BootManager;
    PGUID LocalGuidList;
    ULONG LocalGuidListSize;
    NTSTATUS Status;

    BootManager = NULL;
    LocalGuidList = NULL;
    LocalGuidListSize = 0;
    Status = STATUS_SUCCESS;

    Status = BcdOpenObject(DataStoreHandle,
                           &GUID_WINDOWS_BOOTMGR,
                           &BootManager);
    if (!NT_SUCCESS(Status)) {
        goto BmpGetBootDisplayOrderEnd;
    }

    Status = BcdGetElementData(BootManager,
                               BCDE_BOOTMGR_TYPE_DISPLAY_ORDER,
                               NULL,
                               &LocalGuidListSize);
    if (Status != STATUS_BUFFER_TOO_SMALL) {

        if (Status == STATUS_SUCCESS) {

            //
            // There seems to be no display order for this bootmgr.
            //

            Status = STATUS_NOT_FOUND;
        }
        goto BmpGetBootDisplayOrderEnd;
    }

    if (LocalGuidListSize % sizeof(GUID) != 0) {
        Status = STATUS_INVALID_BUFFER_SIZE;
        goto BmpGetBootDisplayOrderEnd;
    }

    LocalGuidList = (PGUID)BlMmAllocateHeap(LocalGuidListSize);
    if (LocalGuidList == NULL) {
        Status = STATUS_NO_MEMORY;
        goto BmpGetBootDisplayOrderEnd;
    }

    Status = BcdGetElementData(BootManager,
                               BCDE_BOOTMGR_TYPE_DISPLAY_ORDER,
                               LocalGuidList,
                               &LocalGuidListSize);
    if (!NT_SUCCESS(Status)) {
        goto BmpGetBootDisplayOrderEnd;
    }

    *DisplayOrderCount = LocalGuidListSize / sizeof(GUID);
    *DisplayOrder = LocalGuidList;
    LocalGuidList = NULL;

BmpGetBootDisplayOrderEnd:

    if (BootManager != NULL) {
        BcdCloseObject(BootManager);
    }

    if (LocalGuidList != NULL) {
        BlMmFreeHeap(LocalGuidList);
    }

    return Status;
}

