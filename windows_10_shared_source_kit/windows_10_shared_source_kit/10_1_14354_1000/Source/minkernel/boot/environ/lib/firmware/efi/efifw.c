/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    efifw.c

Abstract:

    This module implements the Boot Environment Library's Firmware APIs for
    a Boot Application executing on an EFI firmware.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include <bootlibp.h>

//
// ---------------------------------------------------------------- Definitions
//

//
// TODO: The following GUIDs should be defined in shared kernel/boot headers
//       like the rest of the EFI defined GUIDs.
//

#define ACPI_TABLE_10_GUID  \
  { 0xeb9d2d30, 0x2d88, 0x11d3, 0x9a, 0x16, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d }

#define FPSWA_GUID          \
  { 0xc41b6531, 0x97b9, 0x11d3, 0x9a, 0x29, 0x0, 0x90, 0x27, 0x3f, 0xc1, 0x4d }

//
// -------------------------------------------------------------------- Globals
//

FIRMWARE_PARAMETERS UNALIGNED EfiFirmwareData;
PFIRMWARE_PARAMETERS EfiFirmwareParameters;
EFI_SYSTEM_TABLE *EfiST;
EFI_BOOT_SERVICES *EfiBS;
EFI_RUNTIME_SERVICES *EfiRT;
SIMPLE_TEXT_OUTPUT_INTERFACE *EfiConOut;
SIMPLE_INPUT_INTERFACE *EfiConIn;
EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL *EfiConInEx;

// TODO: Remove these and their externs
EFI_UGA_DRAW_PROTOCOL *EfiUgaDrawOut;
EFI_HANDLE EfiUgaDrawHandle;

EFI_HANDLE EfiImageHandle;

//
// Define a global to track whether all the devices in the system are
// enumerated or not.
//

BOOLEAN EfiAllDevicesEnumerated;

//
// The physical address of the RSDT will be cached so that it only needs to be
// determined once.
//

PHYSICAL_ADDRESS EfiRsdt;

//
// Efi Protocol GUIDs.
//

const EFI_GUID EfiBlockIoProtocol             = BLOCK_IO_PROTOCOL;
const EFI_GUID EfiDebugPortProtocol           = DEBUG_PORT_PROTOCOL;
const EFI_GUID EfiDevicePathProtocol          = DEVICE_PATH_PROTOCOL;
const EFI_GUID EfiDevicePathToTextProtocol    = DEVICE_PATH_TO_TEXT_PROTOCOL;
const EFI_GUID EfiLoadedImageProtocol         = LOADED_IMAGE_PROTOCOL;
const EFI_GUID EfiFileSystemProtocol          = SIMPLE_FILE_SYSTEM_PROTOCOL;
const EFI_GUID EfiGlobalVariable              = EFI_GLOBAL_VARIABLE;
const EFI_GUID EfiFpswaGuid                   = FPSWA_GUID;
const EFI_GUID EfiPxeBaseCodeProtocol         = EFI_PXE_BASE_CODE_PROTOCOL;
const EFI_GUID EfiSimpleNetworkProtocol       = EFI_SIMPLE_NETWORK_PROTOCOL;
const EFI_GUID EfiUgaDrawProtocol             = EFI_UGA_DRAW_PROTOCOL_GUID;
const EFI_GUID EfiGraphicsOutputProtocol      = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
const EFI_GUID EfiPciRootBusProtocol          = EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL;
const EFI_GUID EfiTcgProtocol                 = EFI_TCG_PROTOCOL_GUID;
const EFI_GUID EfiTreeProtocol                = EFI_TREE_PROTOCOL_GUID;
const EFI_GUID EfiDhcp4ServiceBindingProtocol = EFI_DHCP4_SERVICE_BINDING_PROTOCOL_GUID;
const EFI_GUID EfiDhcp4Protocol               = EFI_DHCP4_PROTOCOL_GUID;
const EFI_GUID EfiStorageSecCmdProtocol       = EFI_STORAGE_SECURITY_COMMAND_PROTOCOL_GUID;
const EFI_GUID EfiRngProtocol                 = EFI_RNG_PROTOCOL_GUID;
const EFI_GUID EfiSimpleTextInputExProtocol   = EFI_SIMPLE_TEXT_INPUT_EX_PROTOCOL_GUID;
const EFI_GUID EfiDhcp6ServiceBindingProtocol = EFI_DHCP6_SERVICE_BINDING_PROTOCOL_GUID;
const EFI_GUID EfiDhcp6Protocol               = EFI_DHCP6_PROTOCOL_GUID;
const EFI_GUID EfiHashProtocol                = EFI_HASH_PROTOCOL_GUID;
const EFI_GUID EfiEdidActiveProtocol          = EFI_EDID_ACTIVE_PROTOCOL_GUID;
const EFI_GUID EfiBootModeMgmtProtocol        = EFI_BOOT_MODE_MGMT_PROTOCOL_GUID;

//
// EFI Table GUIDs.
//

const EFI_GUID EfiRootAcpiTableGuid   = ACPI_TABLE_GUID;
const EFI_GUID EfiRootAcpiTable10Guid = ACPI_TABLE_10_GUID;
const EFI_GUID EfiSMBiosTableGuid     = SMBIOS_TABLE_GUID;
const EFI_GUID EfiSMBios3TableGuid    = SMBIOS3_TABLE_GUID;
const EFI_GUID EfiSalSystemTableGuid  = SAL_SYSTEM_TABLE_GUID;
const EFI_GUID EfiWindowsDumpTableGuid = WINDOWS_DUMP_TABLE_GUID;
const EFI_GUID EfiSystemResourceTableGuid = EFI_SYSTEM_RESOURCE_TABLE_GUID;
const EFI_GUID EfiOfflineCrashdumpTableGuid = EFI_OFFLINE_CRASHDUMP_TABLE_GUID;
const EFI_GUID EfiPropertiesTableGuid = EFI_PROPERTIES_TABLE_GUID;
const EFI_GUID EfiMemoryAttributesTableGuid = EFI_MEMORY_ATTRIBUTES_TABLE_GUID;

//
// EFI Variable GUIDs.
//

const EFI_GUID EfiMorBitVariable = MEMORY_ONLY_RESET_CONTROL_GUID;
const EFI_GUID EfiMorLockVariable = MOR_LOCK_GUID;
const EFI_GUID EfiImageSecurityDatabaseVariable = EFI_IMAGE_SECURITY_DATABASE_GUID;

//
// EFI Signature Types.
//

const EFI_GUID EfiCertX509Guid    = EFI_CERT_X509_GUID;
const EFI_GUID EfiCertRsa2048Guid = EFI_CERT_RSA2048_GUID;
const EFI_GUID EfiCertSha256Guid  = EFI_CERT_SHA256_GUID;
const EFI_GUID EfiCertSha384Guid  = EFI_CERT_SHA384_GUID;
const EFI_GUID EfiCertSha512Guid  = EFI_CERT_SHA512_GUID;

//
// EFI Hash Algorithms.
//

const EFI_GUID EfiHashSha1NoPadGuid   = EFI_HASH_ALGORITHM_SHA1_NOPAD_GUID;
const EFI_GUID EfiHashSha256NoPadGuid = EFI_HASH_ALGORITHM_SHA256_NOPAD_GUID;

//
// ----------------------------------------------------------------- Prototypes
//

NTSTATUS
EfipGetRsdt (
    __out PPHYSICAL_ADDRESS Rsdt
    );

NTSTATUS
EfipGetSystemTable (
    __in const EFI_GUID *TableGuid,
    __out PPHYSICAL_ADDRESS TableAddress
    );

NTSTATUS
BlockIoEfiCompareDevice (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in EFI_HANDLE Handle
    );

VOID
BlpFwConvertEfiDescriptorToAcpiDescriptor (
    __in EFI_MEMORY_DESCRIPTOR *EfiDescriptor,
    __out PACPI_E820_ENTRY AcpiDescriptor
    );


//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
BlpFwDestroy (
    VOID
    )

/*++

Routine Description:

    This routine destroys the boot environment library for a given firmware
    type.

Arguments:

    None.

Return Value:

    NT Status Code.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFw);

    //
    // Close the extended simple text input protocol.
    //

    if (EfiConInEx != NULL) {
        Status = EfiCloseProtocol(EfiST->ConsoleInHandle,
                                  &EfiSimpleTextInputExProtocol,
                                  EfiConInEx);

        if (!NT_SUCCESS(Status)) {
            goto FwDestroyEnd;
        }
    }

    Status = STATUS_SUCCESS;

FwDestroyEnd:
    BlpInterfaceExit(InterfaceFw);
    return Status;
}

NTSTATUS
BlpFwInitialize (
    __in ULONG Phase,
    __in PFIRMWARE_PARAMETERS Parameters
    )

/*++

Routine Description:

    This routine performs all EFI specific initialization required for
    the Boot Environment Library.

    Phase 0 : Initialize global pointers for EFI boot services, runtime
        services and console.

    Phase 1 : Disable the system watchdog timer.

    N.B. MM services are required for making firmware calls. Since Phase 1
         occurs after MM has been initialized, firmware calls should only be
         made during Phase 1.

Arguments:

    Phase - Supplies the current phase value for boot library initialization.

    Parameters - Supplies the EFI parameters for the boot application.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER if Parameters is NULL or if the version of the
        parameter block is 0.

--*/

{

    EFI_KEY_TOGGLE_STATE KeyToggleState;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFw);

    if ((Parameters == NULL) || (Parameters->Version == 0)) {
        Status = STATUS_INVALID_PARAMETER;
        goto InitializeEnd;
    }

    //
    // Initialize pointers for boot services, runtime services etc. during
    // Phase 0. Disable system watchdog timer during Phase 1.
    //

    if (Phase == 0) {
        EfiFirmwareParameters = &EfiFirmwareData;

        //
        // Try to read firmware parameters to the boot application and
        // convert them into the currently recognized format, if possible.
        //

        Status = EfiTranslateParameters(EfiFirmwareParameters, Parameters);
        if (!NT_SUCCESS(Status)) {
            goto InitializeEnd;
        }

        EfiImageHandle = Parameters->ImageHandle;
        EfiST = Parameters->SystemTable;
        EfiBS = EfiST->BootServices;
        EfiRT = EfiST->RuntimeServices;
        EfiConOut = EfiST->ConOut;
        EfiConIn  = EfiST->ConIn;
        EfiConInEx = NULL;

    } else {

        ASSERT(Phase == 1);

        //
        // Try to get a handle to the extended simple text input protocol. If
        // this fails, then only the non-extended console is supported. If it
        // succeeds, request to listen to partial key strokes.
        //
        // N.B. EfiOpenProtocol can only be called once the MM library has been
        //      initialized and ARCH phase 0 hase completed.
        //

        Status = EfiOpenProtocol(EfiST->ConsoleInHandle,
                                 &EfiSimpleTextInputExProtocol,
                                 &EfiConInEx);

        if (NT_SUCCESS(Status)) {
            KeyToggleState = EFI_KEY_STATE_EXPOSED | EFI_TOGGLE_STATE_VALID;
            EfiConInExSetState(EfiConInEx, &KeyToggleState);
        }

        //
        // TODO: Implement fully featured watchdog support instead of just
        //    disabling the timer here.
        //

        EfiSetWatchdogTimer(0, 0, 0, NULL);
    }

    Status = STATUS_SUCCESS;

InitializeEnd:
    BlpInterfaceExit(InterfaceFw);
    return Status;
}

NTSTATUS
BlFwGetAcpiMemoryMap (
    __deref_out_ecount_opt(*MemoryMapCount) PACPI_E820_ENTRY *MemoryMapArray,
    __out PULONG MemoryMapCount
    )

/*++

Routine Description:

    This routine will retreive the system memory map and return it to the caller
    in the format compatible with the ACPI driver.

Arguments:

    MemoryMapArray - Supplies a pointer to a variable that receives the address
        of an allocated buffer containing the platform memory map.  The caller
        is responsible for freeing the memory allocation.

    MemoryMapCount - Supplies a pointer to a value to be filled in with the
        number of elements in MemoryMapArray.

Return Value:

    NTSTATUS indicating outcome.

--*/

{

    ULONG AllocationCount;
    ULONG AllocationSize;
    PACPI_E820_ENTRY CurrentAcpiEntry;
    EFI_MEMORY_DESCRIPTOR *CurrentEfiEntry;
    UINTN DescriptorSize;
    UINT32 DescriptorVersion;
    EFI_MEMORY_DESCRIPTOR *EfiMemoryMap;
    EFI_MEMORY_DESCRIPTOR *EfiMemoryMapEnd;
    UINTN EfiMemoryMapSize;
    UINTN Key;
    NTSTATUS Status;

    EfiMemoryMap = NULL;
    EfiMemoryMapSize = 0;
    *MemoryMapArray = NULL;
    *MemoryMapCount = 0;

    //
    // Make two calls to retreive the memory map.  The first call should fail
    // with the size required to retreive the memory map.
    //

    Status = EfiGetMemoryMap(&EfiMemoryMapSize,
                             EfiMemoryMap,
                             &Key,
                             &DescriptorSize,
                             &DescriptorVersion);

    ASSERT(!NT_SUCCESS(Status));
    if (!NT_SUCCESS(Status)) {
        if (Status != STATUS_BUFFER_TOO_SMALL) {
            goto GetAcpiMemoryMapExit;
        }

        EfiMemoryMap = BlMmAllocateHeap(EfiMemoryMapSize);
        if (EfiMemoryMap == NULL) {
            Status = STATUS_NO_MEMORY;
            goto GetAcpiMemoryMapExit;
        }

        Status = EfiGetMemoryMap(&EfiMemoryMapSize,
                                 EfiMemoryMap,
                                 &Key,
                                 &DescriptorSize,
                                 &DescriptorVersion);

        if (!NT_SUCCESS(Status)) {
            goto GetAcpiMemoryMapExit;
        }
    }

    //
    // Validate that the arguments returned make sense.
    //

    if (((EfiMemoryMapSize % DescriptorSize) != 0) ||
        (DescriptorSize < sizeof(EFI_MEMORY_DESCRIPTOR))) {

        Status = STATUS_UNSUCCESSFUL;
        goto GetAcpiMemoryMapExit;
    }

    AllocationCount = (ULONG)(EfiMemoryMapSize / DescriptorSize);
    AllocationSize = AllocationCount * sizeof(ACPI_E820_ENTRY);
    *MemoryMapCount = AllocationCount;
    *MemoryMapArray = BlMmAllocateHeap(AllocationSize);

    if (*MemoryMapArray == NULL) {
        Status = STATUS_NO_MEMORY;
        goto GetAcpiMemoryMapExit;
    }

    //
    // Create the ACPI memory list by converting each EFI memory descriptor
    // into an ACPI E820 entry.
    //

    CurrentAcpiEntry = *MemoryMapArray;
    EfiMemoryMapEnd = Add2Ptr(EfiMemoryMap,EfiMemoryMapSize);
    for (CurrentEfiEntry = EfiMemoryMap;
         CurrentEfiEntry < EfiMemoryMapEnd;
         CurrentEfiEntry = Add2Ptr(CurrentEfiEntry, DescriptorSize)) {

        BlpFwConvertEfiDescriptorToAcpiDescriptor(CurrentEfiEntry,
                                                  CurrentAcpiEntry);

        if (!NT_SUCCESS(Status)) {
            goto GetAcpiMemoryMapExit;
        }

        CurrentAcpiEntry += 1;
    }

    Status = STATUS_SUCCESS;

GetAcpiMemoryMapExit:
    if (EfiMemoryMap != NULL) {
        BlMmFreeHeap(EfiMemoryMap);
    }

    if (!NT_SUCCESS(Status)) {
        if (*MemoryMapArray != NULL)  {
            BlMmFreeHeap(*MemoryMapArray);
        }
    }

    return Status;
}

NTSTATUS
BlFwGetParameters (
    __out PFIRMWARE_PARAMETERS Parameters
    )

/*++

Routine Description:

    Initializes the firmware parameter structure based on the global firmware
    pointers received during initialization.

Arguments:

    Parameters - Supplies a pointer to a data structure to initialize with
        firmware data.

Returns:

    STATUS_SUCCESS when successful.

    STATUS_INVALID_PARAMETER when Parameters is NULL.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFw);

    if (Parameters == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto GetParametersEnd;
    }

    RtlCopyMemory(Parameters,
                  EfiFirmwareParameters,
                  sizeof(FIRMWARE_PARAMETERS));

    Status = STATUS_SUCCESS;

GetParametersEnd:
    BlpInterfaceExit(InterfaceFw);
    return Status;
}

NTSTATUS
BlFwGetSystemTable (
    __in FIRMWARE_TABLE_ID TableId,
    __out PPHYSICAL_ADDRESS TableAddress
    )

/*++

Routine Description:

    This routine attempts to find the specified system table.  When found,
    the physical address of the table is returned.

Arguments:

    TableId - Supplies the identifier of the system table to find.

    TableAddress - Supplies a location in which the physical address of the
        specified table is returned.

Returns:

    STATUS_SUCCESS when successful.
    STATUS_INVALID_PARAMETER if TableId is invalid.
    STATUS_NOT_FOUND when the table is not found.

--*/

{

    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFw);

    switch (TableId) {
    case FW_TABLE_RSDT:

        //
        // The Root System Description Table (RSDT) or Extended System
        // Description Table (XSDT).
        //

        Status = EfipGetRsdt(TableAddress);
        break;

    case FW_TABLE_SMBIOS:

        //
        // Like the RSDP, the root SMBIOS table is referenced in the
        // EFI System Table's list of table.  Attempt to identify this
        // root table.
        //

        Status = EfipGetSystemTable(&EfiSMBiosTableGuid, TableAddress);
        break;

    case FW_TABLE_SMBIOS3:

        //
        // Like the SMBIOS, the root SMBIOS3 table is referenced in the
        // EFI System Table's list of table.  Attempt to identify this
        // root table.
        //

        Status = EfipGetSystemTable(&EfiSMBios3TableGuid, TableAddress);
        break;

    case FW_TABLE_SAL:

        //
        // The EFI System Table should contain a reference to a table
        // describing the System Abstraction Layer.
        //

        Status = EfipGetSystemTable(&EfiSalSystemTableGuid, TableAddress);
        break;

    case FW_TABLE_WINDOWS_DUMP:
        Status = EfipGetSystemTable(&EfiWindowsDumpTableGuid, TableAddress);
        break;

    case FW_TABLE_EFI_SYSTEM_RESOURCES:
        Status = EfipGetSystemTable(&EfiSystemResourceTableGuid, TableAddress);
        break;

    case FW_TABLE_OFFLINE_CRASHDUMP:
        Status = EfipGetSystemTable(&EfiOfflineCrashdumpTableGuid, TableAddress);
        break;

    case FW_TABLE_PROPERTIES:
        Status = EfipGetSystemTable(&EfiPropertiesTableGuid, TableAddress);
        break;

    case FW_TABLE_MEMORY_ATTRIBUTES:
        Status = EfipGetSystemTable(&EfiMemoryAttributesTableGuid, TableAddress);
        break;

    default:
        Status = STATUS_INVALID_PARAMETER;
        break;
    }

    BlpInterfaceExit(InterfaceFw);
    return Status;
}

VOID
BlFwReboot (
    VOID
    )

/*++

Routine Description:

    This routine contains firmware specific code for rebooting an EFI
    system.

Arguments:

    None.

Return Value:

    This function will not return.

--*/

{

    BlpInterfaceEnter(InterfaceFw);

    //
    // On systems with a TPM 2.0 device, the TPM is informed that the power cycle
    // is intentional.
    //

    BlTpmSendShutdownCommand();

    //
    // Make sure there are no BitLocker secrets left in memory.
    //

    BlFveEraseAllSecrets();

    //
    // Disable any outstanding connection to the debugger.
    //

    BlBdStop();
    EfiResetSystem(EfiResetCold, 0, 0, NULL);
    BlpInterfaceExit(InterfaceFw);
    return;
}

VOID
BlFwShutdown (
    VOID
    )

/*++

Routine Description:

    This routine contains firmware specific code for turning off an EFI
    system.

Arguments:

    None.

Return Value:

    This function will not return.

--*/

{

    BlpInterfaceEnter(InterfaceFw);

    //
    // On systems with a TPM 2.0 device, the TPM is informed that the power cycle
    // is intentional.
    //

    BlTpmSendShutdownCommand();

    //
    // Make sure there are no BitLocker secrets left in memory.
    //

    BlFveEraseAllSecrets();

    //
    // Disable any outstanding connection to the debugger.
    //

    BlBdStop();
    EfiResetSystem(EfiResetShutdown, 0, 0, NULL);
    BlpInterfaceExit(InterfaceFw);
    return;
}

NTSTATUS
BlFwEnumerateAllDevices (
    VOID
    )

/*++

Routine Description:

    This routine enumerates all devices in the system.

    N.B. This routine could potentially take a long time to complete on large
         server systems. Hence precaution should be taken to call this only when
         necessary.

Arguments:

    None.

Return Value:

    NT status code.

--*/

{

    UINTN HandleCount;
    EFI_HANDLE *Handles;
    UINTN Index;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    //
    // If all the devices have been already enumerated, then bail out.
    //

    Handles = NULL;
    if (EfiAllDevicesEnumerated != FALSE) {
        ReturnStatus = STATUS_SUCCESS;
        goto EnumerateAllDevicesEnd;
    }

    EfiAllDevicesEnumerated = TRUE;

    //
    // Retrieve a list of all the open handles in the system.
    //

    HandleCount = 0;
    ReturnStatus = EfiLocateHandleBuffer(AllHandles,
                                         NULL,
                                         NULL,
                                         &HandleCount,
                                         &Handles);

    if (!NT_SUCCESS(ReturnStatus)) {
        goto EnumerateAllDevicesEnd;
    }

    //
    // Walk through the list of open handles and invoke connect controller
    // (recursively) on each of them.
    //
    // It is possible for some handles to not support ConnectController
    // interface (as they may not be controller handles) and return failure.
    // Hence return success if atleast one handle returned success (implying
    // some new devices were potentially discovered).
    //

    ReturnStatus = STATUS_UNSUCCESSFUL;
    for (Index = 0; Index < HandleCount; Index += 1) {
        Status = EfiConnectController(Handles[Index], NULL, NULL, TRUE);
        if (NT_SUCCESS(Status)) {
            ReturnStatus = Status;
        }
    }

EnumerateAllDevicesEnd:
    if (Handles != NULL) {
        BlMmFreeHeap(Handles);
    }

    return ReturnStatus;
}

NTSTATUS
BlFwEnumerateDevice (
    __in PBOOT_ENVIRONMENT_DEVICE Device
    )

/*++

Routine Description:

    This routine enumerates the specified device.

Arguments:

    Device - Supplies the device to enumerate.

Return Value:

    NT status code.

--*/

{

    UINTN BlockCount;
    EFI_HANDLE *BlockHandles;
    UINTN DevicePathCount;
    EFI_HANDLE *DevicePathHandles;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    BlockHandles = NULL;
    DevicePathHandles = NULL;

    //
    // Validate parameters.  Currently only block IO devices are supported.
    //

    if ((Device->DeviceType != DEVICE_BLOCK_IO) &&
        (Device->DeviceType != DEVICE_PARTITION) &&
        (Device->DeviceType != DEVICE_PARTITION_EX)) {

        ReturnStatus = STATUS_NOT_SUPPORTED;
        goto ConnectDeviceEnd;
    }

    //
    // Iterate through all devices that support the device path protocol, and
    // attempt to connect them.
    //

    DevicePathCount = 0;
    ReturnStatus = EfiLocateHandleBuffer(ByProtocol,
                                         &EfiDevicePathProtocol,
                                         NULL,
                                         &DevicePathCount,
                                         &DevicePathHandles);

    if (!NT_SUCCESS(ReturnStatus)) {
        goto ConnectDeviceEnd;
    }

    ASSERT(DevicePathHandles != NULL);

    ReturnStatus = STATUS_NOT_FOUND;

    while (DevicePathCount > 0) {
        DevicePathCount -= 1;
        Status = EfiConnectController(DevicePathHandles[DevicePathCount],
                                      NULL,
                                      NULL,
                                      TRUE);

        if (!NT_SUCCESS(Status)) {
            continue;
        }

        //
        // Obtain the list of block IO devices, and compare each one against
        // the target device.
        //

        BlockCount = 0;
        ReturnStatus = EfiLocateHandleBuffer(ByProtocol,
                                             &EfiBlockIoProtocol,
                                             NULL,
                                             &BlockCount,
                                             &BlockHandles);

        if (!NT_SUCCESS(ReturnStatus)) {
            goto ConnectDeviceEnd;
        }

        while (BlockCount > 0) {
            BlockCount -= 1;
            Status = BlockIoEfiCompareDevice(Device, BlockHandles[BlockCount]);
            if (NT_SUCCESS(Status)) {
                ReturnStatus = Status;
                goto ConnectDeviceEnd;
            }
        }

        BlMmFreeHeap(BlockHandles);
        BlockHandles = NULL;
    }

ConnectDeviceEnd:
    if (DevicePathHandles != NULL) {
        BlMmFreeHeap(DevicePathHandles);
    }

    if (BlockHandles != NULL) {
        BlMmFreeHeap(BlockHandles);
    }

    return ReturnStatus;
}

NTSTATUS
BlpFwCheckTimer (
    __in PFIRMWARE_TIMER Timer
    )

/*++

Routine Description:

    This routine checks the status of a timer. If the timer has been expired,
    the routine will return successfully.

Arguments:

    Timer - Supplies a pointer to the firmware timer structure.

Return Value:

    STATUS_SUCCESS if the timer has expired.

    STATUS_NOT_FOUND if the timer has not yet expired.

    Other error code if an error occured.

--*/

{

    NTSTATUS Status;

    if (Timer == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    Status = EfiCheckEvent(Timer->EfiTimer);
    return Status;
}

NTSTATUS
BlpFwCloseTimer (
    __in PFIRMWARE_TIMER Timer
    )

/*++

Routine Description:

    This routine closes a handle for a timer event.

Arguments:

    Timer - Supplies a pointer to the firmware timer structure.

Return Value:

    NT status code.

--*/

{

    NTSTATUS Status;

    if ((Timer == NULL) || (Timer->EfiTimer == NULL)) {
        return STATUS_INVALID_PARAMETER;
    }

    Status = EfiCloseEvent(Timer->EfiTimer);
    if (!NT_SUCCESS(Status)) {
        goto FwCloseTimerEnd;
    }

    Timer->EfiTimer = NULL;

FwCloseTimerEnd:
    return Status;
}

NTSTATUS
BlpFwCreateTimer (
    __in ULONG TriggerTime,
    __out PFIRMWARE_TIMER Timer
    )

/*++

Routine Description:

    This routine creates a timer.

Arguments:

    TriggerTime - Supplies the time, in seconds, for triggering the timer event.

    Timer - Supplies a pointer to a firmware timer structure to get handle to
        the timer.

Return Value:

    NT status code.

--*/

{

    return BlpFwCreateTimerEx(TriggerTime,
                              TIME_FLAGS_DEFAULT,
                              Timer);
}

NTSTATUS
BlpFwCreateTimerEx (
    __in ULONG TriggerTime,
    __in ULONG Flags,
    __out PFIRMWARE_TIMER Timer
    )

/*++

Routine Description:

    This routine creates a timer.

Arguments:

    TriggerTime - Supplies the time for triggering the timer event. The unit
        of this trigger time is specified through Flags. By default it is
        specified in seconds.

    Flags - Supplies the type of timer created, including:

        TIME_FLAGS_DEFAULT: Indicates that the trigger time is specified in
            default units, viz. seconds.

        TIME_FLAGS_MILLISECONDS: Indicates that the trigger time is specified
            in milliseconds.

    Timer - Supplies a pointer to a firmware timer structure to get handle to
        the timer.

Return Value:

    NT status code.

--*/

{

    EFI_EVENT EfiTimer;
    NTSTATUS Status;
    ULONGLONG TriggerTimeIn100ns;

    EfiTimer = NULL;
    if (Timer == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto CreateTimerExEnd;
    }

    Status = EfiCreateEvent(EVT_TIMER, TPL_CALLBACK, NULL, NULL, &EfiTimer);
    if (!NT_SUCCESS(Status)) {
        goto CreateTimerExEnd;
    }

    if (CHECK_FLAG(Flags, TIME_FLAGS_MILLISECONDS)) {
        TriggerTimeIn100ns = (ULONGLONG)TriggerTime * 1000 * 10;

    } else {
        TriggerTimeIn100ns = (ULONGLONG)TriggerTime * 1000 * 1000 * 10;
    }

    Status = EfiSetTimer(EfiTimer, TimerRelative, TriggerTimeIn100ns);
    if (!NT_SUCCESS(Status)) {
        goto CreateTimerExEnd;
    }

    Timer->EfiTimer = EfiTimer;

CreateTimerExEnd:
    if (!NT_SUCCESS(Status) && (EfiTimer != NULL)) {
        EfiCloseEvent(EfiTimer);
    }

    return Status;
}

// ------------------------------------------------------------ Local Functions

NTSTATUS
EfipGetRsdt (
    __out PPHYSICAL_ADDRESS Rsdt
    )

/*++

Routine Description:

    This routine will attempt to obtain the address of the XSDT.  If the XSDT
    cannot be found or the RSDP is invalid, then the routine will attempt to
    obtain the address of the RSDT.  If successful, the physical address will
    be returned to the caller.

    It is the caller's responsibility to map the table into virtual memory and
    determine whether it is the XSDT or RSDT.

Arguments:

    Rsdt - Supplies a location in which the physical address of the XSDT or RSDT
        will be returned.

Return Value:

    STATUS_SUCCESSFUL when successful.

    STATUS_NOT_FOUND when the table could not be located.

--*/

{

    ULONG MapLength;
    ULONG OldMapLength;
    PRSDP Rsdp;
    PHYSICAL_ADDRESS RsdpAddress;
    NTSTATUS Status;

    Rsdp = NULL;
    MapLength = 0;
    Status = STATUS_SUCCESS;

    //
    // The root ACPI table (RSDP) is referenced in the EFI System Table's
    // list of table.  Attempt to identify the RSDP.  There is currenly
    // two possible table identifiers, one for version 2.0 (preferred)
    // and one of 1.0.
    //

    if (EfiRsdt.QuadPart == 0) {
        Status = EfipGetSystemTable(&EfiRootAcpiTableGuid, &RsdpAddress);
        if (!NT_SUCCESS(Status)) {
            Status = EfipGetSystemTable(&EfiRootAcpiTable10Guid, &RsdpAddress);
        }

        if (!NT_SUCCESS(Status)) {
            goto GetRsdtEnd;
        }

        MapLength = sizeof(RSDP);
        Status = BlMmMapPhysicalAddress(&Rsdp,
                                        RsdpAddress,
                                        MapLength,
                                        MEMORY_ATTRIBUTE_NONE);

        if (!NT_SUCCESS(Status)) {
            goto GetRsdtEnd;
        }

        if (Rsdp->Revision > 0) {
            if (Rsdp->Length > MapLength) {
                OldMapLength = MapLength;
                MapLength = Rsdp->Length;
                BlMmUnmapVirtualAddress(Rsdp, OldMapLength);
                Status = BlMmMapPhysicalAddress(&Rsdp,
                                                RsdpAddress,
                                                MapLength,
                                                MEMORY_ATTRIBUTE_NONE);

                if (!NT_SUCCESS(Status)) {
                    Rsdp = NULL;
                    goto GetRsdtEnd;
                }
            }

            EfiRsdt.QuadPart = Rsdp->XsdtAddress.QuadPart;

        } else {
            EfiRsdt.QuadPart = (ULONGLONG)(ULONG_PTR)Rsdp->RsdtAddress;
        }
    }

    Rsdt->QuadPart = EfiRsdt.QuadPart;

GetRsdtEnd:
    if (Rsdp != NULL) {
        BlMmUnmapVirtualAddress(Rsdp, MapLength);
    }

    return Status;
}

NTSTATUS
EfipGetSystemTable (
    __in const EFI_GUID *TableGuid,
    __out PPHYSICAL_ADDRESS TableAddress
    )

/*++

Routine Description:

    The EFI System Table contains an array of Vendor specified configuration
    tables.  This routine will walk the list of tables in search of the
    specified table.  When found, the physical address of the table is
    returned.

Arguments:

    TableGuid - Supplies the identifier of the system table to find.

    TableAddress - Supplies a location in which the physical address of the
        specified table is returned.

Returns:

    STATUS_SUCCESS when successful.
    STATUS_NOT_FOUND when the table is not found.

--*/

{

    ULONG Index;
    NTSTATUS Status;

    BlpInterfaceEnter(InterfaceFw);

    //
    // Walk the system tables linked in the EFI System Table.  If the
    // specified table is found, return successfully.
    //

    Status = STATUS_NOT_FOUND;
    for (Index = 0; Index < EfiST->NumberOfTableEntries; Index += 1) {
        if (RtlEqualMemory(&EfiST->ConfigurationTable[Index].VendorGuid,
                           TableGuid,
                           sizeof(EFI_GUID))) {

            TableAddress->QuadPart =
                (ULONG_PTR)EfiST->ConfigurationTable[Index].VendorTable;

            Status = STATUS_SUCCESS;
            break;
        }
    }

    BlpInterfaceExit(InterfaceFw);
    return Status;
}

VOID
BlpFwConvertEfiDescriptorToAcpiDescriptor (
    __in EFI_MEMORY_DESCRIPTOR *EfiDescriptor,
    __out PACPI_E820_ENTRY AcpiDescriptor
    )

/*++

Routine Description:

    This routine converts and EFI memory descriptor to the format expected by
    the OS ACPI driver.

Arguments:

    EfiDescriptor - Supplies a pointer to a buffer containing the EFI
        descriptor

    AcpiDescriptor - Supplies a pointer to a buffer containing the ACPI
        descriptor

Return Value:

    None.

--*/

{

    ULONGLONG EfiEndPage;
    ULONGLONG EfiStartPage;

    EfiStartPage = EfiDescriptor->PhysicalStart >> EFI_PAGE_SHIFT;
    EfiEndPage = ROUND_UP(EfiStartPage + EfiDescriptor->NumberOfPages,
                          GET_EFIPAGE(1));

    AcpiDescriptor->Base.QuadPart = GET_OSPAGE(EfiStartPage) << PAGE_SHIFT;
    AcpiDescriptor->Length.QuadPart =
        (GET_OSPAGE(EfiEndPage) - GET_OSPAGE(EfiStartPage)) << PAGE_SHIFT;

    //
    // Map all EFI memory types to ACPI descriptor types.  Since EFI descriptors
    // are more descriptive than ACPI descriptors, many of the EFI descriptors
    // are mapped to ACPI reserved descriptors.
    //

    switch (EfiDescriptor->Type) {

    //
    // Handle all types which map to ACPI Reserved.
    //

    case EfiPalCode:
    case EfiMemoryMappedIOPortSpace:
    case EfiMemoryMappedIO:
    case EfiUnusableMemory:
    case EfiRuntimeServicesCode:
    case EfiRuntimeServicesData:
    case EfiReservedMemoryType:
        AcpiDescriptor->Type = AcpiAddressRangeReserved;
        break;

    //
    // Handle all types which map to ACPI Memory.
    //

    case EfiConventionalMemory:
    case EfiLoaderCode:
    case EfiLoaderData:
    case EfiBootServicesCode:
    case EfiBootServicesData:
        AcpiDescriptor->Type = AcpiAddressRangeMemory;
        break;

    case EfiACPIReclaimMemory:
        AcpiDescriptor->Type = AcpiAddressRangeACPI;
        break;

    case EfiACPIMemoryNVS:
        AcpiDescriptor->Type = AcpiAddressRangeNVS;
        break;

    default:
        AcpiDescriptor->Type = AcpiAddressRangeReserved;
        break;
    }
}




