/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    hack.c

Abstract:

    This module contains functions dealing with the PCI driver's
    hack database for buggy devices.









--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"

// --------------------------------------------------------------------- Defines

#define HACKFMT_VENDORDEV         (sizeof(L"VVVVDDDD") - sizeof(UNICODE_NULL))
#define HACKFMT_VENDORDEVREVISION (sizeof(L"VVVVDDDDRR") - sizeof(UNICODE_NULL))
#define HACKFMT_SUBSYSTEM         (sizeof(L"VVVVDDDDSSSSssss") - sizeof(UNICODE_NULL))
#define HACKFMT_SUBSYSTEMREVISION (sizeof(L"VVVVDDDDSSSSssssRR") - sizeof(UNICODE_NULL))
#define HACKFMT_MAX_LENGTH        HACKFMT_SUBSYSTEMREVISION

#define HACKFMT_DEVICE_OFFSET     4
#define HACKFMT_SUBVENDOR_OFFSET  8
#define HACKFMT_SUBSYSTEM_OFFSET 12

#define PATH_ARBITERS   L"\\Registry\\Machine\\System\\CurrentControlSet\\Control\\Arbiters\\ReservedResources"

//
// The number of InputEntries needed to supply to PciDeviceMatchCallback()
// and PciSetHackflagsCallback().
//

#define NUM_INPUT_ENTRY_FOR_PCI_CALLBACK             1

//
// The number of comparison operators that are passed into function
// PciDeviceMatchCallback(), used to match the numberic IDs passed in.
//

#define NUM_OPERATORS_FOR_PCI_DEVICE_MATCH_CALLBACK  1

//
// The number of input numeric fields that are passed into function
// PciDeviceMatchCallback(), as the PCI device IDs to be matched.
// (VendorId, DeviceId, SubVendorId, SubsystemId, RevisionId)
//

#define NUM_IDS_FOR_PCI_DEVICE_MATCH_CALLBACK        5

//
// The number of input numeric fields that are passed into function
// PciSetHackflagsCallback(), as the hackflags that will be set to this device.
//

#define NUM_HACKFLAGS_FOR_PCI_SET_HACKFLAGS_CALLBACK 2

//
// The number of input numeric fields that are passed into function
// PciSetD0DelayCallback().
//

#define NUM_VALUES_FOR_PCI_SET_D0DELAY_CALLBACK 1

//
// The number of input numeric fields that are passed into function
// PciSetMpsSizeCallback().
//

#define NUM_VALUES_FOR_PCI_SET_MPSSIZE_CALLBACK 1

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

VOID
PciSetHackFlagsByClass(
    __inout PPCI_DEVICE Device
    );

BOOLEAN
PciCriticalDevice(
    __in PPCI_DEVICE Device
    );

VOID
PciApplyIdeHacks(
    __inout PPCI_DEVICE Device
    );

VOID
PciApplyDeviceSpecificHacks(
    __inout PPCI_DEVICE Device
    );

VOID
PciApplyBrokenVideoHack(
    __inout PPCI_BUS Bus
    );

BOOLEAN
PciStringToUSHORT(
    __in_ecount(4) PWCHAR String,
    __out PUSHORT Result
    );

BOOLEAN
PciCompareOperator(
    __in LPCSTR Operator,
    __in ULONG Lhs,
    __in ULONG Rhs
    );

EM_CALLBACK_ROUTINE PciDeviceMatchCallback;
EM_CALLBACK_ROUTINE PciSetHackflagsCallback;
EM_CALLBACK_ROUTINE PciSetD0DelayCallback;
EM_CALLBACK_ROUTINE PciSetMpsSizeCallback;
EM_CALLBACK_ROUTINE PciQueryRuleCallback;


// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(INIT, PciBuildHackTable)
    #pragma alloc_text(INIT, PciStringToUSHORT)
    #pragma alloc_text(INIT, PciRegisterWithEM)
    #pragma alloc_text(PAGE, PciGetBusHackFlags)
    #pragma alloc_text(PAGE, PciApplyInitialHacks)
    #pragma alloc_text(PAGE, PciApplyIdeHacks)
    #pragma alloc_text(PAGE, PciApplyDeviceSpecificHacks)
    #pragma alloc_text(PAGE, PciApplyBrokenVideoHack)
    #pragma alloc_text(PAGE, PciDeregisterWithEM)
    #pragma alloc_text(PAGE, PciDeviceMatchCallback)
    #pragma alloc_text(PAGE, PciSetHackflagsCallback)
    #pragma alloc_text(PAGE, PciSetD0DelayCallback)
    #pragma alloc_text(PAGE, PciSetMpsSizeCallback)
    #pragma alloc_text(PAGE, PciQueryRuleCallback)
#endif

// --------------------------------------------------------------------- Globals

extern PULONG InitSafeBootMode;
PPCI_HACK_TABLE_ENTRY PciHackTable;
PVOID PciEmProviderHandle;
BOOLEAN DmaNonworkingListDev = FALSE;
BOOLEAN InterruptNonworkingListDev = FALSE;

EM_ENTRY_REGISTRATION EntryReg[] = {
    {&GUID_EM_PCI_DEVICE_TYPE, NULL, NULL}
};

EM_CALLBACK_REGISTRATION CallbackReg[] = {
    {&GUID_EM_PCI_DEVICE_MATCH_CALLBACK, &PciDeviceMatchCallback, NULL},
    {&GUID_EM_PCI_DEVICE_SET_HACKFLAGS_CALLBACK, &PciSetHackflagsCallback, NULL},
    {&GUID_EM_PCI_DEVICE_SET_D0DELAY_CALLBACK, &PciSetD0DelayCallback, NULL},
    {&GUID_EM_PCI_DEVICE_SET_MPSSIZE_CALLBACK, &PciSetMpsSizeCallback, NULL},
    {&GUID_EM_PCI_DEVICE_QUERY_RULE_CALLBACK, &PciQueryRuleCallback, NULL}
};

// ------------------------------------------------------------------- Functions
NTSTATUS
PciBuildHackTable(
    __in HANDLE ServiceHandle,
    __out PPCI_HACK_TABLE_ENTRY *HackTable
    )
/*++

Routine Description:

    This routine builds the hack database for PCI.  It reads hack
    entries from the registry and populates the database.

Arguments:

    ServiceHandle - Handle to PCI's service key, where the hack
        entries are stored.

    HackTable - On successful exit, will point to a block of memory
        allocated by this routine holding the hack database.

Return Value:

    NT Status code.

--*/
{
    PKEY_VALUE_FULL_INFORMATION valueInfo;
    PPCI_HACK_TABLE_ENTRY table, entry;
    KEY_FULL_INFORMATION keyInfo;
    ULONG hackCount, index;
    HANDLE paramsHandle;
    ULONG valueInfoSize;
    NTSTATUS status;
    USHORT revision;
    ULONGLONG data;
    ULONG dummy;

    PAGED_CODE();

    *HackTable = NULL;

    table = NULL;
    valueInfo = NULL;
    paramsHandle = NULL;

    //
    // The hack table is stored in the Parameters subkey of the service key.
    //

    hackCount = 0;
    if (PciOpenKey(L"Parameters", ServiceHandle, KEY_READ, &paramsHandle, &status)) {

        //
        // Get the key info so we know how many hack values there are.
        //
        status = ZwQueryKey(paramsHandle,
                            KeyFullInformation,
                            &keyInfo,
                            sizeof(KEY_FULL_INFORMATION),
                            &dummy
                            );
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        hackCount = keyInfo.Values;
    }

    //
    // Allocate and initialize the hack table
    //
    table = PciAllocatePool(NonPagedPoolNx, (hackCount + 1) * sizeof(PCI_HACK_TABLE_ENTRY));
    if (!table) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }

    entry = table;
    if (hackCount > 0) {
        //
        // Allocate a valueInfo buffer big enough for the biggest valid
        // format and a ULONGLONG worth of data.
        //
        valueInfoSize = sizeof(KEY_VALUE_FULL_INFORMATION)
                        + HACKFMT_MAX_LENGTH
                        + sizeof(ULONGLONG);

        valueInfo = PciAllocatePool(PagedPool | POOL_COLD_ALLOCATION, valueInfoSize);
        if (!valueInfo) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto cleanup;
        }


        for (index = 0; index < hackCount; index++) {

            status = ZwEnumerateValueKey(paramsHandle,
                                         index,
                                         KeyValueFullInformation,
                                         valueInfo,
                                         valueInfoSize,
                                         &dummy
                                         );
            if (!NT_SUCCESS(status)) {

                if ((status == STATUS_BUFFER_OVERFLOW) ||
                    (status == STATUS_BUFFER_TOO_SMALL)) {

                    //
                    // All out data is of fixed length and the buffer is big enough
                    // so this can't be for us.
                    //
                    continue;

                } else {
                    goto cleanup;
                }
            }

            //
            // Get pointer to the data if its of the right type
            //
            if ((valueInfo->Type == REG_BINARY) &&
                (valueInfo->DataLength == sizeof(ULONGLONG))) {

                data = *(ULONGLONG UNALIGNED *)(((PUCHAR)valueInfo) + valueInfo->DataOffset);

            } else {
                continue;
            }

            //
            // Now see if the name is formatted like we expect it to be:
            // VVVVDDDD
            // VVVVDDDDRR
            // VVVVDDDDSSSSssss
            // VVVVDDDDSSSSssssRR
            //
            if ((valueInfo->NameLength != HACKFMT_VENDORDEV) &&
                (valueInfo->NameLength != HACKFMT_VENDORDEVREVISION) &&
                (valueInfo->NameLength != HACKFMT_SUBSYSTEM) &&
                (valueInfo->NameLength != HACKFMT_SUBSYSTEMREVISION)) {

                TRACE(INIT_TRACE_LEVEL, "Skipping hack entry with invalid length name\n");

                continue;
            }

            RtlZeroMemory(entry, sizeof(PCI_HACK_TABLE_ENTRY));

            //
            // Look for DeviceID and VendorID (VVVVDDDD)
            //
            if (!PciStringToUSHORT(valueInfo->Name, &entry->VendorID)) {
                continue;
            }

            if (!PciStringToUSHORT(valueInfo->Name + HACKFMT_DEVICE_OFFSET,
                                   &entry->DeviceID)) {
                continue;
            }

            //
            // Look for SubsystemVendorID/SubSystemID (SSSSssss)
            //
            if ((valueInfo->NameLength == HACKFMT_SUBSYSTEM) ||
                (valueInfo->NameLength == HACKFMT_SUBSYSTEMREVISION)) {

                if (!PciStringToUSHORT(valueInfo->Name + HACKFMT_SUBVENDOR_OFFSET,
                                       &entry->SubVendorID)) {
                    continue;
                }

                if (!PciStringToUSHORT(valueInfo->Name + HACKFMT_SUBSYSTEM_OFFSET,
                                       &entry->SubSystemID)) {
                    continue;
                }

                entry->Flags |= PCI_HACK_FLAG_SUBSYSTEM;
            }

            //
            // Look for RevisionID (RR)
            //
            if ((valueInfo->NameLength == HACKFMT_VENDORDEVREVISION) ||
                (valueInfo->NameLength == HACKFMT_SUBSYSTEMREVISION)) {

                if (PciStringToUSHORT(valueInfo->Name
                                      + (valueInfo->NameLength/sizeof(WCHAR) - 4),
                                      &revision)) {

                    entry->RevisionID = (UCHAR)revision & 0xFF;
                    entry->Flags |= PCI_HACK_FLAG_REVISION;

                } else {
                    continue;
                }
            }

            PCI_ASSERT(entry->VendorID != 0xFFFF);

            //
            // Fill in the entry
            //
            entry->HackFlags = data;

            TRACE(INIT_TRACE_LEVEL,
                  "Adding Hack entry for Vendor:0x%04x Device:0x%04x ",
                  entry->VendorID, entry->DeviceID
                  );

            if (entry->Flags & PCI_HACK_FLAG_SUBSYSTEM) {

                TRACE_CONTINUE(INIT_TRACE_LEVEL,
                               "SybSys:0x%04x SubVendor:0x%04x ",
                               entry->SubSystemID, entry->SubVendorID
                               );
            }

            if (entry->Flags & PCI_HACK_FLAG_REVISION) {

                TRACE_CONTINUE(INIT_TRACE_LEVEL,
                               "Revision:0x%02x",
                                (ULONG)entry->RevisionID
                                );
            }

            TRACE_CONTINUE(INIT_TRACE_LEVEL,
                           " = 0x%I64x\n",
                           entry->HackFlags
                           );

            entry++;
        }

        PciFreePool(valueInfo);
    }

    if (paramsHandle) {
        ZwClose(paramsHandle);
    }

    //
    // Terminate the table with an invalid VendorID
    //
    PCI_ASSERT(entry < (table + hackCount + 1));
    entry->VendorID = 0xFFFF;

    *HackTable = table;

    return STATUS_SUCCESS;

cleanup:

    PCI_ASSERT(!NT_SUCCESS(status));

    if (paramsHandle) {
        ZwClose(paramsHandle);
    }

    if (valueInfo) {
        PciFreePool(valueInfo);
    }

    if (table) {
        PciFreePool(table);
    }

    return status;
}

NTSTATUS
PciRegisterWithEM (
    __inout PDRIVER_OBJECT DriverObject
    )

/*++

Description:

    This routine will register the PCI driver as a provider
    to the Errata Manager. This will enable the PCI driver to
    later query the EM to evaluate errata rules and hack flags.

Arguments:

    DriverObject - Supplies a pointer to the driver object passed in from
        DriverEntry. This is needed to register with EM.

Return Value:

    NT Status code.

--*/

{

    ULONG CallbackRegCount;
    ULONG EntryRegCount;
    NTSTATUS Status;

    PAGED_CODE();

    EntryRegCount = ARRAYSIZE(EntryReg);
    CallbackRegCount = ARRAYSIZE(CallbackReg);
    Status = EmProviderRegister(DriverObject,
                                EntryReg,
                                EntryRegCount,
                                CallbackReg,
                                CallbackRegCount,
                                &PciEmProviderHandle);

    return Status;
}

VOID
PciDeregisterWithEM (
    VOID
    )

/*++

Description:

    This routine will deregister the PCI driver as a provider
    to the Errata Manager. This will free up the callbacks and rules
    in EM that are associated with the PCI driver.

Arguments:

    None.

--*/

{

    PAGED_CODE();

    //
    // Deregister the PCI driver as a provider for EM.
    //

    if (PciEmProviderHandle != NULL) {
        EmProviderDeregister(PciEmProviderHandle);
    }

    return;
}

NTSTATUS
PciGetHackFlagsFromRegistry (
    __in PPCI_DEVICE Device,
    __out PULONGLONG HackFlags
    )
/*++

Description:

    Look in the registry for any flags for this VendorId/DeviceId.

Arguments:

    Device - Device to retrieve the hack flags for.

    HackFlags - Pointer to a ULONGLONG that will contain the hack flags.

Return Value:

    64 bit flags value or 0 if not found.

--*/
{
    PPCI_HACK_TABLE_ENTRY current;
    ULONG match, bestMatch;
    ULONGLONG flags;

    NON_PAGED_CODE();

    PCI_ASSERT(PciHackTable);

    //
    // We want to do a best-case match:
    // VVVVDDDDSSSSssssRR
    // VVVVDDDDSSSSssss
    // VVVVDDDDRR
    // VVVVDDDD
    //
    // List is currently unsorted, so keep updating current best match.
    //
    flags = 0;
    bestMatch = 0;
    for (current = PciHackTable; current->VendorID != 0xFFFF; current++) {
        match = 0;

        //
        // Must at least match vendor/dev
        //
        if ((current->DeviceID != Device->DeviceID) ||
            (current->VendorID != Device->VendorID)) {
            continue;
        }

        match = 1;

        //
        // If this entry specifies a revision, check that it is consistent.
        //
        if (current->Flags & PCI_HACK_FLAG_REVISION) {
            if (current->RevisionID == Device->RevisionID) {
                match += 2;
            } else {
                continue;
            }
        }

        //
        // If this entry specifies subsystems, check that they are consistent
        //
        if (current->Flags & PCI_HACK_FLAG_SUBSYSTEM) {
            if (current->SubVendorID == Device->SubVendorID &&
                current->SubSystemID == Device->SubSystemID) {
                match += 4;
            } else {
                continue;
            }
        }

        if (match > bestMatch) {
            bestMatch = match;
            flags = current->HackFlags;
        }
    }

    *HackFlags = flags;
    return STATUS_SUCCESS;
}

NTSTATUS
PciGetHackFlagsFromEM (
    __inout PDEVICE_OBJECT DeviceObject
    )

/*++

Description:

    This routine looks up the hack flags for this PCI device from the
    Errata Manager.

Arguments:

    DeviceObject - Supplies the PCI physical device object in order
        to get its hack flags from EM.

Return Value:

    NT STATUS_SUCCESS is returned on success, otherwise the status
    code from the failing EM function is returned.

--*/

{

    EM_RULE_STATE EmState;
    EM_ENTRY_DATA EntryData;
    PEM_ENTRY_DATA InputEntries[NUM_INPUT_ENTRY_FOR_PCI_CALLBACK];
    NTSTATUS Status;

    NON_PAGED_CODE();

    //
    // Set up the PDO as the input entry passed into the Errata
    // Manager query.
    //

    EntryData.Data = DeviceObject;
    EntryData.DataLength = sizeof(void*);
    InputEntries[0] = &EntryData;

    //
    // This is the EM rule that gets all the PCI device-specific
    // hack flags. This supersedes the hack flags that were previously
    // in machine.inx.
    //

    Status = EmClientRuleEvaluate(&GUID_EM_PCI_DEVICE_HACK_RULE,
                                  InputEntries,
                                  NUM_INPUT_ENTRY_FOR_PCI_CALLBACK,
                                  &EmState);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // This is the EM rule that allows an alternate resume-to-D0 delay.
    //

    Status = EmClientRuleEvaluate(&GUID_EM_PCI_D0_DELAY_RULE,
                                  InputEntries,
                                  NUM_INPUT_ENTRY_FOR_PCI_CALLBACK,
                                  &EmState);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // This is the EM rule that allows an alternate MPS size.
    //

    Status = EmClientRuleEvaluate(&GUID_EM_PCI_DEVICE_SET_MPSSIZE_RULE,
                                  InputEntries,
                                  NUM_INPUT_ENTRY_FOR_PCI_CALLBACK,
                                  &EmState);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // This is the EM rule that sets PCI device-specific hack flags for the
    // matching bios machines.
    //

    Status = EmClientRuleEvaluate(&GUID_EM_PCI_DEVICE_HACK_BIOS_MATCH_RULE,
                                  InputEntries,
                                  NUM_INPUT_ENTRY_FOR_PCI_CALLBACK,
                                  &EmState);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // This is the EM rule that sets PCI device-specific hack flags for the
    // matching CPU machines.
    //

    Status = EmClientRuleEvaluate(&GUID_EM_PCI_DEVICE_HACK_CPU_MATCH_RULE,
                                  InputEntries,
                                  NUM_INPUT_ENTRY_FOR_PCI_CALLBACK,
                                  &EmState);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Some PCI express root ports have broken support for ASPM in the
    // hardware, which causes the system to crash when ASPM is enabled.
    // Therefore when these devices are found, ASPM will be disabled on
    // these systems accordingly.
    //

    Status = EmClientRuleEvaluate(
                 &GUID_EM_PCI_DEVICE_DISABLE_PCI_EXPRESS_ASPM_RULE,
                 InputEntries,
                 NUM_INPUT_ENTRY_FOR_PCI_CALLBACK,
                 &EmState);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    if (EmState == STATE_TRUE) {
        PciSystemWideHackFlags |= PCI_SYS_HACK_DISABLE_EXPRESS_ASPM;
    }

    //
    // This is the EM rule for Intel Iov chipsets which require Dma remapping
    // root port alternate error handing.
    //

    if (!DmaNonworkingListDev) {
        Status = EmClientRuleEvaluate(&GUID_EM_PCI_DEVICE_DMA_REMAPPING_ERROR_HANDLING_RULE,
                                      InputEntries,
                                      NUM_INPUT_ENTRY_FOR_PCI_CALLBACK,
                                      &EmState);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        if (EmState == STATE_TRUE) {
            DmaNonworkingListDev = TRUE;
        }
    }

    //
    // This is the EM rule for AMD IOMMU devices that don't remap all interrups.
    //
    if (!InterruptNonworkingListDev) {
        Status = EmClientRuleEvaluate(&GUID_EM_PCI_DEVICE_INTERRUPT_REMAPPING_NOT_ALL_REMAPPED_RULE,
                                      InputEntries,
                                      NUM_INPUT_ENTRY_FOR_PCI_CALLBACK,
                                      &EmState);

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        if (EmState == STATE_TRUE) {
            InterruptNonworkingListDev = TRUE;
        }
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciGetBusHackFlags(
    __in PPCI_BUS Bus,
    __out PULONG HackFlags
    )

/*++

Routine Description:

    This routine retrieves any hack flags for the given bus.
    These are stored in the device's PnP registry key under a value
    called "HackFlags"

Arguments:

    Bus - The bus to get the hack flags for.

    HackFlags - Pointer to a ULONGLONG to contain the hackflags on exit.

Return Value:

    NT Status code.

--*/
{
    HANDLE deviceRegistryHandle;
    PULONG valueDword;
    ULONG valueLength;
    NTSTATUS status;

    PAGED_CODE();

    *HackFlags = 0;

    valueDword = NULL;
    deviceRegistryHandle = NULL;

    //
    // Check if there are any hacks to apply to this bus.
    // These are located under the device registry key in a value called HackFlags
    //
    status = IoOpenDeviceRegistryKey(Bus->PhysicalDeviceObject,
                                     PLUGPLAY_REGKEY_DEVICE,
                                     KEY_ALL_ACCESS,
                                     &deviceRegistryHandle
                                     );
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    status = PciGetRegistryValue(L"HackFlags",
                                 NULL,
                                 deviceRegistryHandle,
                                 REG_DWORD,
                                 &valueDword,
                                 &valueLength
                                 );
    if (!NT_SUCCESS(status)) {

        //
        // Not having the value present is not a failure.
        //
        if (status == STATUS_OBJECT_NAME_NOT_FOUND) {
            status = STATUS_SUCCESS;
        }
        goto exit;
    }

    //
    // If we have valid data in the registry then remember it
    //
    if ((valueDword != NULL) && (valueLength == sizeof(ULONG))) {

        *HackFlags = *valueDword;
    }

    status = STATUS_SUCCESS;

exit:

    if (deviceRegistryHandle) {
        ZwClose(deviceRegistryHandle);
    }

    if (valueDword) {
        PciFreePool(valueDword);
    }

    return status;
}


VOID
PciMungeHackFlags(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine sets hack flags to be used on a device based
    on other hack flags set on the device or other device
    characteristics.

Arguments:

    Device - Device to set hack flags for.

    Config - The initial config header for the device.

Return Value:

    VOID

--*/
{
    NON_PAGED_CODE();

    //
    // First get the device's hack flags in a consistent state.
    // This involves fixing up any misreported class codes, then
    // setting any hack flags that need to be applied to the device's
    // class, and then setting any hack flags that are implied
    // by other hack flags.
    //
    PciSetHackFlagsByClass(Device);

    if (Device->PowerCapability == 0) {

        Device->HackFlags |= PCI_HACK_NO_PM_CAPS;
    }

    if ((Device->Parent->PciBridge != NULL) &&
        (Device->Parent->PciBridge->DisableChildPowerManagement != 0)) {

        Device->HackFlags |= PCI_HACK_NO_PM_CAPS;
    }

    if (Device->SubVendorID == 0xFFFF) {

        Device->HackFlags |= PCI_HACK_NO_SUBSYSTEM;
    }

    //
    // The CRITICAL_DEVICE hack flag only dictates the way that
    // BAR probing/setting is done.  For critical devices, also
    // set the flags that will prevent the device from being
    // disabled or powered down.
    //
    if (Device->HackFlags & PCI_HACK_CRITICAL_DEVICE) {

        Device->HackFlags |= PCI_HACK_NO_DISABLE_FOR_STOP_REMOVE;
        if (Device->OnPostPath == FALSE) {
            Device->HackFlags |= PCI_HACK_DISABLE_POWER_DOWN;
        }
    }

    //
    // The PCI_HACK_PRESERVE_COMMAND is very old and has an uncertain
    // history.  The pre-Longhorn code base had very bizarre behavior
    // when it was set (leaving the decodes enabled when probing the
    // BARs in the interest of preserving the command register, for
    // instance.)  Treat this as the same as the NEVER_DISCONNECT
    // hack.
    //
    if (Device->HackFlags & PCI_HACK_PRESERVE_COMMAND) {

        Device->HackFlags |= PCI_HACK_NEVER_DISCONNECT;
    }

    //
    // If you can't disconnect the device, you can't figure
    // out its requirements, so never give this device any
    // resources.
    //
    if (Device->HackFlags & PCI_HACK_NEVER_DISCONNECT) {

        Device->HackFlags |= PCI_HACK_ENUM_NO_RESOURCE;
    }

    //
    //  If this is a bridge, ignore SSIDs after D3.
    //
    if (Device->HeaderType == PCI_BRIDGE_TYPE) {
        Device->HackFlags |= PCI_HACK_NO_SUBSYSTEM_AFTER_D3;
    }
}

VOID
PciApplyInitialHacks(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine sets elements of the Device extension based
    on the hack flags of the device.

Arguments:

    Device - The device to set hack flags of.

Return Value:

    VOID

--*/
{
    USHORT command;

    PAGED_CODE();

    PciReadConfigRegister(Device, Command, &command);

    //
    // apply any initial behavior that the existence of a
    // hack flag implies.
    //
    if (Device->HackFlags & PCI_HACK_NO_PM_CAPS) {

        Device->PowerCapability = 0;

        //
        // There are no power capabilities on the device.
        // In this case we only support D0 and D3. D3 is defined as decodes
        // off.
        //
        if ((command & (PCI_ENABLE_IO_SPACE |
                        PCI_ENABLE_MEMORY_SPACE |
                        PCI_ENABLE_BUS_MASTER)) != 0) {

            Device->LogicalPowerState = PowerDeviceD0;

        } else {

            Device->LogicalPowerState = PowerDeviceD3;
        }
    }

    if ((Device->HackFlags & PCI_HACK_VIDEO_LEGACY_DECODE) &&
        PCI_ROOT_BUS(Device->Parent) &&
        !Device->Parent->IoArbiterVideoHackApplied) {

        //
        // This hack flag enables a hack in the PCI arbiters to reserve a
        // large number IO ranges for broken S3 and ATI cards.  These
        // legacy cards don't function behind a bridge so we only perform
        // the check on a root bus and only perform it once
        //
        PciApplyBrokenVideoHack(Device->Parent);
    }

    if (Device->HackFlags & PCI_HACK_NO_SUBSYSTEM) {
        Device->SubSystemID = 0;
        Device->SubVendorID = 0;
        Device->SubIDsCapability = 0;
    }

    if (Device->HackFlags & PCI_HACK_FAKE_CLASS_CODE) {
        Device->BaseClass = PCI_CLASS_BASE_SYSTEM_DEV;
        Device->SubClass = PCI_SUBCLASS_SYS_OTHER;
    }

    //
    // Now apply initial behavior required by a particular device
    // or device class.
    //
    if ((Device->BaseClass == PCI_CLASS_MASS_STORAGE_CTLR) &&
        (Device->SubClass == PCI_SUBCLASS_MSC_IDE_CTLR)) {

        PciApplyIdeHacks(Device);
    }

    //
    // If VGA controllers are disabled by the BIOS, respect this
    // and leave the I/O and memory decodes off.
    //
    if (PCI_VGA_CONTROLLER(Device)) {
        Device->PossibleDecodes = command & (PCI_ENABLE_IO_SPACE |
                                             PCI_ENABLE_MEMORY_SPACE);

        Device->PossibleDecodes |= PCI_ENABLE_BUS_MASTER;
        if (Device->PossibleDecodes != PCI_DECODE_ENABLES) {
            TraceLoggingWrite(
                PciTraceLoggingProvider, 
                "PciVgaDisabledByBios",
                TraceLoggingInt32(command, "Command"),
                TraceLoggingInt32(Device->VendorID, "VendorID"),
                TraceLoggingInt32(Device->DeviceID, "DeviceID"),
                TraceLoggingInt32(Device->RevisionID, "RevisionID"),
                TraceLoggingInt32(Device->SubVendorID, "SubVendorID"),
                TraceLoggingInt32(Device->SubSystemID, "SubSystemID"),
                TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES)
            );
        }
    }

    if (Device->HackFlags & PCI_HACK_NO_EXPRESS_CAP) {
        Device->ExpressCapability = 0;
    }

    PciApplyDeviceSpecificHacks(Device);
}

// ----------------------------------------------------------------------------- Utilities

VOID
PciMungeClassCodes(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine sets the class code of a device based on
    information like a hack flag or explicit ven/dev IDs.
    This is to fix up misreporting devices.

Arguments:

    Device - Device to fix up the class codes of.

Return Value:

    VOID

--*/
{
    NON_PAGED_CODE();






























    return;
}

VOID
PciSetHackFlagsByClass(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine sets hack flags to be used on a device based
    on the class of the device.

Arguments:

    Device - Device to set hack flags for.

Return Value:

    VOID

--*/
{
    NON_PAGED_CODE();

    if (Device->BaseClass == PCI_CLASS_BRIDGE_DEV) {

        if ((Device->SubClass == PCI_SUBCLASS_BR_HOST) ||
            (Device->SubClass == PCI_SUBCLASS_BR_ISA) ||
            (Device->SubClass == PCI_SUBCLASS_BR_EISA) ||
            (Device->SubClass == PCI_SUBCLASS_BR_MCA)) {

            //
            // For host bridges, emit the PDO in case there is
            // a (miniport) driver for it, but under no circumstances
            // should we attempt to figure out what resources it
            // consumes (we don't know the format of its configuration
            // space).
            //
            // Microchannel bridges report their resource usage
            // like good citizens.   Unfortunately we really want
            // them to behave like ISA bridges and consume no
            // resources themselves.  Their children are subtractive
            // from the parent bus.   Enumerate the device but not
            // its resources.
            //
            // Mark them as "never disconnect" which will prevent us
            // from trying to modify their command register at all.
            // This will prevent us from being able to see what
            // resources they consume (PCI_HACK_ENUM_NO_RESOURCE will
            // be set on them in PciMungeHackFlags.
            //
            Device->HackFlags |= PCI_HACK_NEVER_DISCONNECT;
        }

        if (Device->SubClass == PCI_SUBCLASS_BR_OTHER) {

            //
            // "Other" bridges have been treated in the past as
            // devices whose decodes shouldn't be turned off for semi-permanent
            // operations like disabling and powering down, but still need
            // to have their decodes disabled for resource determination.
            // We have a hack flag for that.
            //
            Device->HackFlags |= PCI_HACK_DONT_DISABLE;
        }
    }

    //
    // IDE Hacks.
    //
    //










    if ((Device->BaseClass == PCI_CLASS_MASS_STORAGE_CTLR) &&
        (Device->SubClass  == PCI_SUBCLASS_MSC_IDE_CTLR)) {

        Device->HackFlags |= PCI_HACK_DISABLE_POWER_DOWN;
    }

    //
    // The POST device path can't be disabled.
    //
    if (Device->OnPostPath != FALSE) {
        Device->HackFlags |= PCI_HACK_NO_DISABLE_FOR_STOP_REMOVE;
    }

    //
    // Critical device hacks.
    //
    // Devices can be critical if they are explicitly marked as such in
    // the database or if they are of a critical device class.
    // Critical devices will have their resources queried in a safe way
    // to ensure that no other threads can target traffic at the device
    // while its decodes are off.  They should not be permanently disabled
    // except for VGA cards, which are critical but should be allowed
    // to be put into low power.
    //
    if (PciCriticalDevice(Device) &&
        !(Device->HackFlags & PCI_HACK_OVERRIDE_CRITICAL_DEVICE)) {

        Device->HackFlags |= PCI_HACK_CRITICAL_DEVICE;
    }
}

BOOLEAN
PciCriticalDevice(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    Checks to see if a given device is "critical", that is, it
    cannot be turned off at any time during boot (not to probe
    the BARs, etc) without risking a crash.

Arguments:

    Device - The device to check.

Return Value:

    TRUE if the device is critical
    FALSE otherwise

--*/
{
    NON_PAGED_CODE();

    //
    // Interrupt controllers are critical system devices and
    // must be treated very specially.  They cannot be turned
    // off without stopping all traffic in the system, but
    // they can't be left alone either.
    //
    if ((Device->BaseClass == PCI_CLASS_BASE_SYSTEM_DEV) &&
        (Device->SubClass == PCI_SUBCLASS_SYS_INTERRUPT_CTLR)) {

        return TRUE;
    }

    //
    // ISA bridges are critical because they are often a bridge
    // to legacy system devices like timers and interrupt controllers
    // that are not enumerated at all.  Many chipsets don't have
    // any resources in their ISA bridges and don't listen when
    // we try to disable their decodes, but for the ones that
    // operate more-or-less like normal PCI devices, we apply
    // this behavior.
    //
    if ((Device->BaseClass == PCI_CLASS_BRIDGE_DEV) &&
        (Device->SubClass == PCI_SUBCLASS_BR_ISA)) {

        return TRUE;
    }

    //
    // Video cards are critical because the kernel writes to the
    // video card during boot in parallel with PnP enumeration
    // of the video card.  It doesn't know if/when PnP tries
    // to turn off the card for enumeration.
    //
    if (Device->OnPostPath != FALSE) {
        return TRUE;
    }

    return FALSE;
}

DWORD
PciApplyArbiterInitializationHack (
    VOID
    )

/*++

Routine Description:

    This routine applies relavent hacks to the root PCI bus's I/O arbiter from
    errata manager.

Arguments:

    None.

Return Value:

    STATUS_SUCCESS on success.

    Otherwise a status code returned from one of the functions invoked in this
    routine.

--*/

{

    HANDLE ArbiterKey;
    OBJECT_ATTRIBUTES Attributes;
    ULONG Disposition;
    EM_RULE_STATE EmState;
    UNICODE_STRING PathString;
    NTSTATUS Status;

    ArbiterKey = NULL;
    EmState = STATE_UNKNOWN;
    Status = STATUS_SUCCESS;

    //
    // Unreported memory at F8.
    //

    Status = EmClientQueryRuleState(
                 &GUID_EM_PCI_ARBITER_UNREPORTED_MEMORY_AT_F8_RULE,
                 &EmState);

    if (!NT_SUCCESS(Status)) {
        goto ApplyArbiterInitializationHackExit;
    }

    if (EmState == STATE_TRUE) {

        //
        // Write the memory config override shortcut for pci arbiters.
        //

        PciConstStringToUnicodeString(&PathString, PATH_ARBITERS);
        InitializeObjectAttributes(&Attributes,
                                   &PathString,
                                   OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                   NULL,
                                   NULL);

        Status = ZwCreateKey(&ArbiterKey,
                             KEY_READ | KEY_WRITE,
                             &Attributes,
                             0,
                             NULL,
                             REG_OPTION_VOLATILE,
                             &Disposition);

        if (!NT_SUCCESS(Status)) {
            goto ApplyArbiterInitializationHackExit;
        }

        PciConstStringToUnicodeString(&PathString, L"PCI");
        Status = ZwSetValueKey(ArbiterKey,
                               &PathString,
                               0,
                               REG_SZ,
                               L"BrokenMemAtF8",
                               sizeof(L"BrokenMemAtF8"));

        if (!NT_SUCCESS(Status)) {
            goto ApplyArbiterInitializationHackExit;
        }
    }

ApplyArbiterInitializationHackExit:

    if (ArbiterKey != NULL) {
        ZwClose(ArbiterKey);
    }

    return Status;
}

VOID
PciApplyIdeHacks(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine sets hack flags to be used on a device based
    on the class of the device.

Arguments:

    Device - Device to set hack flags for.

Return Value:

    VOID

--*/
{
    PAGED_CODE();
















    //
    // Deal with switching between legacy and native mode.
    //
    // Check with the BIOS to ensure that it can deal with the mode change
    // This is indicated by the *parent* of the device having a method
    // called NIDE which returns a package of integers which are slots
    // in _ADR format that can be switched to native mode.
    //
    // This method used to be called NIDE but we shipped XP with a bug
    // in PCIIDE so that if a spurious interrupt occured with a machine
    // without a subordinate device we would attempt to select the subordinate
    // (we may not have enumerated yet) and attempt to dismiss the interrupt
    // unfortunatley selecting the subordinate pulls the IDE INT pin high and
    // triggers another interrupt (yuck).  We renamed this method so OEMs
    // have a way to enable native mode without risking incompatibility
    // with XP gold
    //
    // Enabling native mode might expose BIOS bugs like interrupt routing problems
    // that could prevent the machine from booting, so don't do this for
    // safe mode, so that the user has some way to boot.
    //
    // Also base the enablement of this feature on a registry key which OEMs must
    // set in their preinstall to indicate they have tested the feature.
    //
    if ((PciEnableNativeModeATA != 0) &&
        (*InitSafeBootMode == 0) &&
        PciSlotPresentInParentMethod(Device, (ULONG)'ATAN') &&
        !(Device->HackFlags & PCI_HACK_BAD_NATIVE_IDE)) {

        PCI_DEVICE_SET_FLAG(Device, NativeIdeSwitchAllowed);
    }

    //
    // Config is updated to reflect the results of the switch
    // if any.  Relied upon below.
    //
    if (PciConfigureIdeController(Device, TRUE)) {

        PCI_DEVICE_SET_FLAG(Device, IdeInNativeMode);
    }
}

VOID
PciApplyDeviceSpecificHacks(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine applies behavior if the given device requires
    some device-specific behavior.

Arguments:

    Device - Device to apply the hacks to.

Return Value:

    VOID

--*/
{
    ULONG zeroLMBA;
    UCHAR configByte;

    PAGED_CODE();

    //
    // Force cardbus controllers into cardbus mode.
    //
    if (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE) {

        zeroLMBA = 0;

        PciWriteConfigRegister(Device, type2.LegacyModeBaseAddress, &zeroLMBA);
    }










































}

VOID
PciApplyBrokenVideoHack(
    __inout PPCI_BUS Bus
    )
/*++

Routine Description:

    This routine sets the root PCI bus's I/O arbiter to
    use a new, special assignment ordering that reserves large
    ranges of I/O to work around certain broken video cards..

Arguments:

    Bus - The bus to apply the hack to.

Return Value:

    VOID

--*/
{
    PARBITER_INSTANCE arbiter;
    NTSTATUS status;

    PAGED_CODE();

    PCI_ASSERT(!Bus->IoArbiterVideoHackApplied);
    PCI_ASSERT(PCI_ROOT_BUS(Bus));

    //
    // we should always have an arbiter for a root bus.
    //
    PCI_ASSERT(Bus->IoArbiter);

    arbiter = Bus->IoArbiter;

    //
    // We are reinitializing the orderings, so free the old ones
    //
    ArbFreeOrderingList(&arbiter->OrderingList);
    ArbFreeOrderingList(&arbiter->ReservedList);

    //
    // Rebuild the ordering list reserving all the places these broken S3 and
    // ATI cards might want to live - this should not fail.
    //
    status = ArbBuildAssignmentOrdering(arbiter,
                                        L"Pci",
                                        L"BrokenVideo",
                                        NULL
                                        );

    PCI_ASSERT(NT_SUCCESS(status));
    UNREFERENCED_PARAMETER(status);

    PCI_BUS_SET_FLAG(Bus, IoArbiterVideoHackApplied);
}

BOOLEAN
PciStringToUSHORT(
    __in_ecount(4) PWCHAR String,
    __out PUSHORT Result
    )
/*++

Description:

    Takes a 4 character hexidecimal sting and converts it into a USHORT.

Arguments:

    String - the string

    Result - the USHORT

Return Value:

    TRUE is success, FASLE otherwise

--*/
{
    ULONG count;
    USHORT number = 0;
    PWCHAR current;

    PAGED_CODE();

    current = String;

    for (count = 0; count < 4; count++) {

        number <<= 4;

        if (*current >= L'0' && *current <= L'9') {
            number |= *current - L'0';
        } else if (*current >= L'A' && *current <= L'F') {
            number |= *current + 10 - L'A';
        } else if (*current >= L'a' && *current <= L'f') {
            number |= *current + 10 - L'a';
        } else {
            *Result = 0;
            return FALSE;
        }

        current++;
    }

    *Result = number;
    return TRUE;
}

BOOLEAN
PciCompareOperator (
    __in LPCSTR Operator,
    __in ULONG Lhs,
    __in ULONG Rhs
    )

/*++

Routine Description:

    This routine tests condition specified by the operator by
    applying it to the specified LHS and RHS arguments.

Arguments:

    Operator - Supplies the operator to be tested.

    Lhs - Supplies the Left Hand Side argument for the Operator.

    Rhs - Supplies Right Hand Side argument for the Operator..

Return Value:

    True iff the condition Lhs Operator Rhs is satisfied.

--*/

{

    BOOLEAN Result;

    //
    // "=" or "==" for EQUAL.
    // "!=" or "=!" or "<>" for NOT EQUAL.
    // "<" for LESS THAN.
    // "<=" or "=<" for LESS THAN or EQUAL.
    // ">" for GREATER THAN.
    // ">=" or "=>" for GREATER THAN or EQUAL.
    //

    Result = FALSE;
    if (strcmp(Operator, "=") == 0 || strcmp(Operator, "==") == 0) {
        Result = (Lhs == Rhs);

    } else if (strcmp(Operator, "!=") == 0 ||
               strcmp(Operator, "<>") == 0 ||
               strcmp(Operator, "=!") == 0) {

        Result = (Lhs != Rhs);

    } else if (strcmp(Operator, "<") == 0) {
        Result = (Lhs < Rhs);

    } else if (strcmp(Operator, "<=") == 0 || strcmp(Operator, "=<") == 0) {
        Result = (Lhs <= Rhs);

    } else if (strcmp(Operator, ">") == 0) {
        Result = (Lhs > Rhs);

    } else if (strcmp(Operator, ">=") == 0 || strcmp(Operator, "=>") == 0) {
        Result = (Lhs >= Rhs);

    } else if (strcmp(Operator, "*") == 0) {
        Result = TRUE;
    }

    return Result;
}

_Function_class_(EM_CALLBACK_ROUTINE)
_IRQL_requires_max_(APC_LEVEL)
_IRQL_requires_same_
EM_RULE_STATE
PciDeviceMatchCallback (
    _In_reads_opt_(NumberOfEntries) EM_ENTRY_DATA **InputEntries,
    _In_ ULONG NumberOfEntries,
    _In_reads_opt_(NumberOfStrings) LPCSTR *InputStrings,
    _In_ ULONG NumberOfStrings,
    _In_reads_opt_(NumberOfNumerics) PULONG InputNumerics,
    _In_ ULONG NumberOfNumerics,
    _In_opt_ PVOID Context
    )

/*++

Description:

    This routine is the callback matching function PCI driver provides
    for the Errata Manager. This callback function will be used to
    evaluate whether the specific PCI device fall into the rule or not.

Arguments:

    InputEntries[0] - Supplies the PCI physical device object the
        hackflags will be set for.

    NumberOfEntries - Supplies the number of input entries that are
        passed in. As seen there should only be 1.

    InputStrings[0] - Supplies the operator used for comparing the
        Revision ID of the device.

    NumberOfStrings - Supplies the number of comparison operators
        that are passed in. As seen there should only be 1.

    InputNumerics[0] - Supplies the Vendor ID of the device.

    InputNumerics[1] - Supplies the Device ID of the device.

    InputNumerics[2] - Supplies the Subsystem Vendor ID of the device.

    InputNumerics[3] - Supplies the Subsystem Device ID of the device.

    InputNumerics[4] - Supplies the Revision ID of the device.

    NumberOfNumerics - Supplies the number of input numerics that are
        passed in. As seen above there should always be 5 of them.

    All the other arguments are currently unused.

Return Value:

    STATE_TRUE is returned if this PCI device matches with inputs.

    STATE_FALSE is returned if otherwise.

--*/

{

    PPCI_DEVICE Device;
    PDEVICE_OBJECT DeviceObject;
    EM_RULE_STATE ReturnValue;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Context);

    ReturnValue = STATE_TRUE;

    //
    // First check if the number of parameters are correct.
    //

    if ((NumberOfEntries != NUM_INPUT_ENTRY_FOR_PCI_CALLBACK) ||
        (NumberOfStrings != NUM_OPERATORS_FOR_PCI_DEVICE_MATCH_CALLBACK) ||
        (NumberOfNumerics != NUM_IDS_FOR_PCI_DEVICE_MATCH_CALLBACK)) {

        ReturnValue = STATE_FALSE;
        goto PciDeviceMatchCallbackEnd;
    }

    //
    // Check if the InputEntry is actually the physical device object.
    //

    if (InputEntries[0]->DataLength != sizeof(PDEVICE_OBJECT)) {
        ReturnValue = STATE_FALSE;
        goto PciDeviceMatchCallbackEnd;
    }

    DeviceObject = InputEntries[0]->Data;
    Device = (PPCI_DEVICE)DeviceObject->DeviceExtension;

    //
    // Try to do a best-case match:
    // VVVVDDDD.SSSSssss.RR
    // VVVVDDDD.SSSSssss
    // VVVVDDDD.RR
    // VVVVDDDD
    //
    // The device entries in the errata database should be sorted
    // in the order of maximum number of fields to the minimum number
    // of fields, so the first match will always be the best match.
    //

    //
    // Must at least match vendor + device IDs.
    //

    if ((InputNumerics[0] != Device->VendorID) ||
        (InputNumerics[1] != Device->DeviceID)) {

        ReturnValue = STATE_FALSE;
        goto PciDeviceMatchCallbackEnd;
    }

    //
    // If this entry specifies subsystem, check that they are consistent.
    //

    if ((InputNumerics[2] > 0) ||
        (InputNumerics[3] > 0)) {

        if ((InputNumerics[2] != Device->SubVendorID) ||
            (InputNumerics[3] != Device->SubSystemID)) {

            ReturnValue = STATE_FALSE;
            goto PciDeviceMatchCallbackEnd;
        }
    }

    //
    // If this entry specifies a revision operator, check that it is
    // consistent.
    //

    if (PciCompareOperator(InputStrings[0],
                           Device->RevisionID,
                           InputNumerics[4]) == FALSE) {

        ReturnValue = STATE_FALSE;
        goto PciDeviceMatchCallbackEnd;
    }

PciDeviceMatchCallbackEnd:
    return ReturnValue;

}

_Function_class_(EM_CALLBACK_ROUTINE)
_IRQL_requires_max_(APC_LEVEL)
_IRQL_requires_same_
EM_RULE_STATE
PciSetHackflagsCallback (
    _In_reads_opt_(NumberOfEntries) EM_ENTRY_DATA **InputEntries,
    _In_ ULONG NumberOfEntries,
    _In_reads_opt_(NumberOfStrings) LPCSTR *InputStrings,
    _In_ ULONG NumberOfStrings,
    _In_reads_opt_(NumberOfNumerics) PULONG InputNumerics,
    _In_ ULONG NumberOfNumerics,
    _In_opt_ PVOID Context
    )

/*++

Description:

    This callback function will take the hack flags (passed in as the
    InputNumerics) and set them into the hack flags field of the PCI
    device (passed in as the InputEntries).

Arguments:

    InputEntries[0] - Supplies the PCI physical device object the
        hackflags will be set for.

    NumberOfEntries - Supplies the number of input entries that are
        passed in. As seen there should only be 1.

    InputNumerics[0] - Supplies the hack flags being set, part 1.

    InputNumerics[1] - Supplies the hack flags being set, part 2.

    NumberOfNumerics - Supplies the number of input numerics that are
        passed in. As seen above there should always be 2 of them.

    All the other arguments are currently unused.

Return Value:

    STATE_FALSE is returned if the number of input entries do not conform
    to what the function is expecting, and STATE_TRUE is returned otherwise.

--*/

{

    PPCI_DEVICE Device;
    PDEVICE_OBJECT DeviceObject;
    ULONGLONG flags;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(InputStrings);
    UNREFERENCED_PARAMETER(NumberOfStrings);

    //
    // First check if the number of parameters are correct.
    //

    if ((NumberOfEntries != NUM_INPUT_ENTRY_FOR_PCI_CALLBACK) ||
        (NumberOfNumerics != NUM_HACKFLAGS_FOR_PCI_SET_HACKFLAGS_CALLBACK)) {

        return STATE_FALSE;
    }

    //
    // Check if the InputEntry is actually the physical device object.
    //

    if (InputEntries[0]->DataLength != sizeof(PDEVICE_OBJECT)) {
        return STATE_FALSE;
    }

    DeviceObject = (PDEVICE_OBJECT)InputEntries[0]->Data;
    Device = (PPCI_DEVICE)DeviceObject->DeviceExtension;

    //
    // OR the input hack flags into the field in this PCI device.
    //

    flags = 0;
    flags = (ULONGLONG)InputNumerics[0] << 32;
    flags |= InputNumerics[1];
    Device->HackFlags |= flags;
    return STATE_TRUE;
}

_Function_class_(EM_CALLBACK_ROUTINE)
_IRQL_requires_max_(APC_LEVEL)
_IRQL_requires_same_
EM_RULE_STATE
PciSetD0DelayCallback (
    _In_reads_opt_(NumberOfEntries) EM_ENTRY_DATA **InputEntries,
    _In_ ULONG NumberOfEntries,
    _In_reads_opt_(NumberOfStrings) LPCSTR *InputStrings,
    _In_ ULONG NumberOfStrings,
    _In_reads_opt_(NumberOfNumerics) PULONG InputNumerics,
    _In_ ULONG NumberOfNumerics,
    _In_opt_ PVOID Context
    )

/*++

Description:

    This callback function will take the D0 delay (passed in as the
    InputNumerics) and set them into the D0 dealy field of the PCI
    device (passed in as the InputEntries).

Arguments:

    InputEntries[0] - Supplies the PCI physical device object the
        hackflags will be set for.

    NumberOfEntries - Supplies the number of input entries that are
        passed in. As seen there should only be 1.

    InputNumerics[0] - Supplies the D0 delay, in milliseconds.

    NumberOfNumerics - Supplies the number of input numerics that are
        passed in. As seen above there should always be 2 of them.

    All the other arguments are currently unused.

Return Value:

    STATE_FALSE is returned if the number of input entries do not conform
    to what the function is expecting, and STATE_TRUE is returned otherwise.

--*/

{

    PPCI_DEVICE Device;
    PDEVICE_OBJECT DeviceObject;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(InputStrings);
    UNREFERENCED_PARAMETER(NumberOfStrings);

    //
    // First check if the number of parameters are correct.
    //

    if ((NumberOfEntries != NUM_INPUT_ENTRY_FOR_PCI_CALLBACK) ||
        (NumberOfNumerics != NUM_VALUES_FOR_PCI_SET_D0DELAY_CALLBACK)) {

        return STATE_FALSE;
    }

    //
    // Check if the InputEntry is actually the physical device object.
    //

    if (InputEntries[0]->DataLength != sizeof(PDEVICE_OBJECT)) {
        return STATE_FALSE;
    }

    DeviceObject = (PDEVICE_OBJECT)InputEntries[0]->Data;
    Device = (PPCI_DEVICE)DeviceObject->DeviceExtension;

    Device->D0DelayInMilliseconds = (USHORT)(0xFFFF & InputNumerics[0]);
    return STATE_TRUE;
}

_Function_class_(EM_CALLBACK_ROUTINE)
_IRQL_requires_max_(APC_LEVEL)
_IRQL_requires_same_
EM_RULE_STATE
PciSetMpsSizeCallback (
    _In_reads_opt_(NumberOfEntries) EM_ENTRY_DATA **InputEntries,
    _In_ ULONG NumberOfEntries,
    _In_reads_opt_(NumberOfStrings) LPCSTR *InputStrings,
    _In_ ULONG NumberOfStrings,
    _In_reads_opt_(NumberOfNumerics) PULONG InputNumerics,
    _In_ ULONG NumberOfNumerics,
    _In_opt_ PVOID Context
    )

/*++

Description:

    This callback function will take the MPS Size (passed in as the
    InputNumerics) and set it into the MpsSizeOverride field of the PCI
    device (passed in as the InputEntries).

Arguments:

    InputEntries[0] - Supplies the PCI physical device object the
        hackflags will be set for.

    NumberOfEntries - Supplies the number of input entries that are
        passed in. As seen there should only be 1.

    InputNumerics[0] - Supplies the MPS Mask.

    NumberOfNumerics - Supplies the number of input numerics that are
        passed in. As seen above there should always be 2 of them.

    All the other arguments are currently unused.

Return Value:

    STATE_FALSE is returned if the number of input entries do not conform
    to what the function is expecting, and STATE_TRUE is returned otherwise.

--*/

{

    PPCI_DEVICE Device;
    PDEVICE_OBJECT DeviceObject;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(InputStrings);
    UNREFERENCED_PARAMETER(NumberOfStrings);

    //
    // First check if the number of parameters are correct.
    //

    if ((NumberOfEntries != NUM_INPUT_ENTRY_FOR_PCI_CALLBACK) ||
        (NumberOfNumerics != NUM_VALUES_FOR_PCI_SET_MPSSIZE_CALLBACK)) {

        return STATE_FALSE;
    }

    //
    // Check if the InputEntry is actually the physical device object.
    //

    if (InputEntries[0]->DataLength != sizeof(PDEVICE_OBJECT)) {
        return STATE_FALSE;
    }

    DeviceObject = (PDEVICE_OBJECT)InputEntries[0]->Data;
    Device = (PPCI_DEVICE)DeviceObject->DeviceExtension;
    Device->MpsSizeOverride = (UCHAR)(0x7 & InputNumerics[0]);
    Device->MpsSizeOverrideSet = 1;
    return STATE_TRUE;
}

_Function_class_(EM_CALLBACK_ROUTINE)
_IRQL_requires_max_(APC_LEVEL)
_IRQL_requires_same_
EM_RULE_STATE
PciQueryRuleCallback (
    _In_reads_opt_(NumberOfEntries) EM_ENTRY_DATA **InputEntries,
    _In_ ULONG NumberOfEntries,
    _In_reads_opt_(NumberOfStrings) LPCSTR *InputStrings,
    _In_ ULONG NumberOfStrings,
    _In_reads_opt_(NumberOfNumerics) PULONG InputNumerics,
    _In_ ULONG NumberOfNumerics,
    _In_opt_ PVOID Context
    )

/*++

Description:

    This callback function compares the DMA and INTERRUPT rule guid with
    EM_ENTRY_DATA.data. It checks the internal cache of iov nonworking devices
    and returns STATE_TRUE or STATE_FALSE accordingly.

Arguments:

    InputEntries[0] - Supplies the IOV DMA or INTERRUPT rule GUID.

    NumberOfEntries - Supplies the number of input entries that are
        passed in. As seen there should only be 1.

    InputStrings - Not used.

    NumberOfStrings - 0.

    InputNumerics - Not used.

    NumberOfNumerics - 0.

    All the other arguments are currently unused.

Return Value:

    EM_RULE_STATE

--*/

{
    LPCGUID RuleId;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Context);
    UNREFERENCED_PARAMETER(InputStrings);
    UNREFERENCED_PARAMETER(InputNumerics);

    //
    // First check if the number of parameters are correct.
    //
    if (NumberOfEntries != NUM_INPUT_ENTRY_FOR_PCI_CALLBACK ||
        NumberOfNumerics != 0 ||
        NumberOfStrings != 0) {

        return STATE_FALSE;
    }

    //
    // Check if the InputEntry is actually the GUID of
    // GUID_EM_PCI_DEVICE_DMA_REMAPPING_ERROR_HANDLING_RULE or
    // GUID_EM_PCI_DEVICE_INTERRUPT_REMAPPING_NOT_ALL_REMAPPED_RULE
    //

    if (InputEntries[0]->DataLength != sizeof(GUID)) {
        return STATE_FALSE;
    }

    RuleId = (LPCGUID)InputEntries[0]->Data;
    if (IsEqualGUID(RuleId, &GUID_EM_PCI_DEVICE_DMA_REMAPPING_ERROR_HANDLING_RULE)) {
        return DmaNonworkingListDev ? STATE_TRUE : STATE_FALSE;
    }

    if (IsEqualGUID(RuleId, &GUID_EM_PCI_DEVICE_INTERRUPT_REMAPPING_NOT_ALL_REMAPPED_RULE)) {
        return InterruptNonworkingListDev ? STATE_TRUE : STATE_FALSE;
    }

    return STATE_FALSE;
}
