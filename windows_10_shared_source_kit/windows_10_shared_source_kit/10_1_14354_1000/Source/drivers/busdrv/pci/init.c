/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    init.c

Abstract:

    This module contains the initialization code for PCI.SYS.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include <ntacpi.h>
#include <init.tmh>
#if defined(_AMD64_)
#include <hvgdk_mini.h>
#endif

// --------------------------------------------------------------------- Defines

#define CurrentControlSetKey    L"\\Registry\\Machine\\System\\CurrentControlSet"

// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes

DRIVER_INITIALIZE DriverEntry;

DRIVER_UNLOAD PciDriverUnload;

NTSTATUS
PciGetDebugPorts(
    __out PPCI_DEBUG_PORT *DebugPorts,
    __out PULONG DebugPortsCount
    );

PVOID
PciGetAcpiTable(
    __in ULONG Signature
    );

NTSTATUS
PciAcpiFindRsdt(
    __out PACPI_BIOS_MULTI_NODE *AcpiMulti
    );

NTSTATUS
PciGetMultifunctionSubKey(
    __in PWSTR IdentifierString,
    __out PHANDLE KeyHandle
    );

NTSTATUS
PciGetSystemWideHackFlags (
    VOID
    );

NTSTATUS
PciGetSystemWideHackFlagsFromEM (
    VOID
    );

NTSTATUS
PciGetSystemWideHackFlagsFromRegistry (
    VOID
    );

VOID
PciQueryInterruptSteeringInformation (
    VOID
    );

VOID
PciInitializeNumaInformation (
    VOID
    );

CALLBACK_FUNCTION PciPowerStateCallback;

// --------------------------------------------------------------------- Pragmas

#pragma prefast(disable: __WARNING_UNUSED_SCALAR_ASSIGNMENT, "asserts use these assignments")

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(INIT, DriverEntry)
    #pragma alloc_text(INIT, PciGetDebugPorts)
    #pragma alloc_text(INIT, PciAcpiFindRsdt)
    #pragma alloc_text(INIT, PciGetAcpiTable)
    #pragma alloc_text(INIT, PciGetMultifunctionSubKey)
    #pragma alloc_text(INIT, PciGetSystemWideHackFlags)
    #pragma alloc_text(INIT, PciGetSystemWideHackFlagsFromEM)
    #pragma alloc_text(INIT, PciGetSystemWideHackFlagsFromRegistry)
    #pragma alloc_text(INIT, PciQueryInterruptSteeringInformation)
    #pragma alloc_text(PAGE, PciDriverUnload)
    #pragma alloc_text(PAGE, PciGetNumaInformation)
    #pragma alloc_text(INIT, PciInitializeNumaInformation)
    #pragma alloc_text(PAGE, PciFreeNumaInformation)
    #pragma alloc_text(PAGE, PciPowerStateCallback)
#endif

// --------------------------------------------------------------------- Globals

PDRIVER_OBJECT PciDriverObject;
UNICODE_STRING PciRegistryPath = {0};

PDESCRIPTION_HEADER            PciWatchdogTable;
PPCI_DEBUG_PORT                PciDebugPorts;
ULONG                          PciDebugPortsCount;

BOOLEAN PciLockDeviceResources;
BOOLEAN PciSystemMsiEnabled;
ULONG PciSystemWideHackFlags = PCI_SYS_HACK_NO_SRIOV; // starting point
ULONG PciWHEARecordPreallocationCount = 1UL;
ULONG PciEnableNativeModeATA;
ULONG PciEnableLegacyPcieRuntimePme;
BOOLEAN PciInterruptSteeringEnabled;

KEVENT PciGlobalLock;
KEVENT PciBusLock;
PCALLBACK_OBJECT PowerCallbackObj;
PVOID PciPowerCallbackRegistrationHandle;
BOOLEAN PciSystemPowerStateTransitionActive;

extern PPCI_HACK_TABLE_ENTRY PciHackTable;
extern PULONG InitSafeBootMode;

// ------------------------------------------------------------------- Functions

//
// Disable Prefast warnings for annotation inconsistency.  These warnings
// should be disabled until the definition of DRIVER_INITIALIZE can be
// updated to match this annotation.
//

_Function_class_(DRIVER_INITIALIZE)
_IRQL_requires_same_
NTSTATUS
DriverEntry (
    _In_ struct _DRIVER_OBJECT *DriverObject,
    _In_ PUNICODE_STRING RegistryPath
    )
/*++

Routine Description:

    Entrypoint needed to initialize the PCI bus enumerator.  This fills in
    IRP dispatch routines and initializes global variables.

Arguments:

    DriverObject - Pointer to the driver object created by the system.

    RegistryPath - Pointer to the unicode registry service path.

Return Value:

    NT status.

--*/
{

    OBJECT_ATTRIBUTES attributes;
    UNICODE_STRING callbackName;
    HANDLE ccsHandle;
    BOOLEAN ExclusionListsInitialized;
    PPCI_SEGMENT segment;
    HANDLE serviceHandle;
    NTSTATUS status;
    UNICODE_STRING unicodeLockString;
    UNICODE_STRING unicodeValueString;
    PULONG valueDword;
    ULONG valueLength;
    PWCHAR valueString;
    HAL_INTERRUPT_TARGET_INFORMATION targetInformation;

    PAGED_CODE();

    WPP_INIT_TRACING(DriverObject, RegistryPath);

    ccsHandle = NULL;
    PciHackTable = NULL;
    PciWatchdogTable = NULL;
    serviceHandle = NULL;
    ExclusionListsInitialized = FALSE;

    //
    // Fill in the driver object.
    //
    // The driver does not queue IRPs for the control device. The default
    // handler for IRP_MJ_CLEANUP/CLOSE returns success. There is nothing to do
    // in these two handlers, we just use WMI default handlers.
    //

    PciDriverObject = DriverObject;
    DriverObject->MajorFunction[IRP_MJ_CREATE]         = PciDispatchCreate;
    DriverObject->MajorFunction[IRP_MJ_PNP]            = PciDispatchPnpPower;
    DriverObject->MajorFunction[IRP_MJ_POWER]          = PciDispatchPnpPower;
    DriverObject->MajorFunction[IRP_MJ_SYSTEM_CONTROL] = PciDispatchSystemControl;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = PciDispatchDeviceControl;
    DriverObject->DriverUnload                         = PciDriverUnload;
    DriverObject->DriverExtension->AddDevice           = PciAddDevice;

    //
    // Initialize locks.
    //

    KeInitializeEvent(&PciGlobalLock, SynchronizationEvent, TRUE);
    KeInitializeEvent(&PciBusLock, SynchronizationEvent, TRUE);
    KeInitializeEvent(&PciConfigDoubleBufferLock, SynchronizationEvent, TRUE);

    //
    // Initialize the event that we'll use to synchronize access to the legacy
    // hardware description map in the registry.
    //

    KeInitializeEvent(&PciLegacyDescriptionLock, SynchronizationEvent, TRUE);

    //
    // Make a copy of the registry path.
    //

    PciRegistryPath.Buffer = PciAllocatePool(PagedPool,
                                             RegistryPath->MaximumLength);

    if (PciRegistryPath.Buffer == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    PciRegistryPath.MaximumLength = RegistryPath->MaximumLength;
    PciRegistryPath.Length = 0;
    RtlCopyUnicodeString(&PciRegistryPath, RegistryPath);

    //
    // Initialize the PCI early restore list.
    //

    InitializeListHead(&PciEarlyRestoreList);

    //
    // Initialize the PCI late restore feature
    //

    PciLateRestoreInitialize();

    //
    // According to the PCI Firmware Specification, there is at least one PCI
    // Segment Group - 0 in a system.
    // Initialize segment 0. All PCI root buses by default belong to segment 0.
    //

    segment = PciAllocatePool(NonPagedPoolNx, sizeof(PCI_SEGMENT));
    if (segment == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    RtlZeroMemory(segment, sizeof(PCI_SEGMENT));
    RtlInitializeBitMap(&segment->BusNumberMap, segment->BusNumberMapBuffer, 0x100);
    RtlClearAllBits(&segment->BusNumberMap);

    //
    // Add segment 0 to the global linked list of segments.
    //

    PciSegmentList = segment;

    //
    // Open our service key, which is the base key for both
    // the hack table and the information about debug ports.
    //

    InitializeObjectAttributes(&attributes,
                               RegistryPath,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               NULL,      // Root Directory
                               NULL       // Security Descriptor
                               );

    status = ZwOpenKey(&serviceHandle, KEY_READ, &attributes);
    if (!NT_SUCCESS(status) || serviceHandle == 0) {
        goto exit;
    }

    status = PciBuildHackTable(serviceHandle, &PciHackTable);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    status = PciRegisterWithEM(DriverObject);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // When there are no PCI debugging devices, PCI should continue to
    // initialize.
    //

    status = PciGetDebugPorts(&PciDebugPorts, &PciDebugPortsCount);
    if (!NT_SUCCESS(status)) {
        PciDebugPortsCount = 0;
    }

    //
    // Need access to the CurrentControlSet for various initialization chores.
    //

    if (!PciOpenKey(CurrentControlSetKey,
                    NULL, // Parent handle
                    KEY_READ,
                    &ccsHandle,
                    &status)) {
        goto exit;
    }

    //
    // Get OSLOADOPTIONS and see if PCILOCK was specified.
    // (Unless the driver is build to force PCILOCK).
    // (Note: Can't check for leading '/', it was stripped
    // before getting put in the registry).
    //
    // If PCILOCK isn't in the OS load options, there still
    // might be a key in CCS\Control\BiosInfo\PCI that indicates
    // that PciLock semantics should be used.
    //

    PciLockDeviceResources = FALSE;
    if (NT_SUCCESS(PciGetRegistryValue(L"SystemStartOptions",
                                       L"Control",
                                       ccsHandle,
                                       REG_SZ,
                                       &valueString,
                                       &valueLength
                                       ))) {

        PciConstStringToUnicodeString(&unicodeLockString, L"PCILOCK");

        //
        // We assume that the string coming from the registry is NUL terminated
        // and if this isn't the case, the MaximumLength in the counted string
        // prevents us from over running our buffer.  If the string is larger
        // than MAX_USHORT bytes then we truncate it.
        //

        unicodeValueString.Buffer = valueString;
        unicodeValueString.Length = (USHORT)(valueLength - sizeof(UNICODE_NULL));
        unicodeValueString.MaximumLength = (USHORT)valueLength;
        if (PciUnicodeStringStrStr(&unicodeValueString, &unicodeLockString, TRUE)) {
            PciLockDeviceResources = TRUE;
        }

        PciFreePool(valueString);
        valueString = NULL;
    }

    if (!PciLockDeviceResources) {

        if (NT_SUCCESS(PciGetRegistryValue(L"PCILock",
                                           L"Control\\BiosInfo\\PCI",
                                           ccsHandle,
                                           REG_DWORD,
                                           &valueDword,
                                           &valueLength))) {

            if ((valueLength == sizeof(ULONG)) && (*valueDword == 1)) {

                PciLockDeviceResources = TRUE;
            }

            PciFreePool(valueDword);
            valueDword = NULL;
        }
    }

    //
    // Get the system wide hack flags.
    //

    status = PciGetSystemWideHackFlags();
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    PciQueryInterruptSteeringInformation();

    //
    // See if Native mode ATA should be enabled.
    //

    PciEnableNativeModeATA = 0;
    if (NT_SUCCESS(PciGetRegistryValue(L"EnableNativeModeATA",
                                       L"Control\\PnP\\PCI",
                                       ccsHandle,
                                       REG_DWORD,
                                       &valueDword,
                                       &valueLength))) {

        if ((valueDword != NULL) && (valueLength == sizeof(ULONG))) {

            PciEnableNativeModeATA = *valueDword;
            PciFreePool(valueDword);
        }

        valueDword = NULL;
    }

    //
    // Determine whether runtime PME on legacy-mode PCIe devices is supported.
    //

    PciEnableLegacyPcieRuntimePme = 0;
#if DBG
    if (NT_SUCCESS(PciGetRegistryValue(L"EnableRuntimePCIePME",
                                       L"Control\\PnP\\PCI",
                                       ccsHandle,
                                       REG_DWORD,
                                       &valueDword,
                                       &valueLength))) {

        if ((valueDword != NULL) && (valueLength == sizeof(ULONG))) {

            PciEnableLegacyPcieRuntimePme = *valueDword;
            PciFreePool(valueDword);
        }

        valueDword = NULL;
    }
#endif

    //
    // Get the MSI support status by querying the HAL. The HAL will
    // fill in global information about interrupt delivery characteristics.
    //
    // N.B. We currently use this to expose MSI support via custom properties,
    //      and also to handle MSI only PCIe root/switch port devices that may
    //      exist on a system without MSI enabled (or intentionally disabled).
    //

    PciSystemMsiEnabled = FALSE;
    RtlZeroMemory(&targetInformation,
                  sizeof(HAL_INTERRUPT_TARGET_INFORMATION));

    status = HalGetInterruptTargetInformation(InterruptTargetTypeGlobal,
                                              0,
                                              &targetInformation);

    if ((NT_SUCCESS(status)) &&
        (targetInformation.Flags & INTERRUPT_TARGET_APIC_MSI_SUPPORTED) != 0) {

        PciSystemMsiEnabled = TRUE;
    }

    //
    // Get NUMA information for a system. This information is saved in the
    // global variables for later processing.
    //

    PciInitializeNumaInformation();
    status = PciGetNumaInformation();
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Get the WD ACPI table.  This contains the bus/device/function
    // of any watchdog timer device, so that we can know to suppress
    // enumeration of the device.
    //

    PciWatchdogTable = (PDESCRIPTION_HEADER)PciGetAcpiTable(WDAT_SIGNATURE);
    if (PciWatchdogTable == NULL) {
        PciWatchdogTable = (PDESCRIPTION_HEADER)PciGetAcpiTable(WDTT_SIGNATURE);
    }

    //
    // Build the I/O arbiter exclusion lists.
    //

    status = PciBuildDefaultIoExclusionLists();
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    ExclusionListsInitialized = TRUE;

    //
    // Override the functions that used to be in the HAL but are now in the
    // PCI driver
    //

    PciHookHal();

    //
    // Enable the hardware verifier code if appropriate.
    //

    PciVerifierInit(DriverObject);

    //
    // Configure the WHEA subsystem for the PCI Express error type.
    //

    PciWheaInit();

    //
    // Initialize variables for PCI express wake control.
    //

    ExpressDevicesEnabledForWake = 0;
    KeInitializeSpinLock(&ExpressWakeControlLock);

    //
    // Initialize ETW tracing.
    //

    PciDiagInitialize();

    //
    // Initialize TraceLogging support.
    //

    PciTraceLogInitialize();

    //
    // Register for the power state callback.  If this fails, either in callback
    // opening or registration, move on.  The only consequence will be that we
    // fail to mark certian IRP_MN_WAIT_WAKE instances as responsible for
    // waking the system from S1 through S4, and failing here would be worse
    // than that.  Note that this code may leave a reference on the callback
    // object until the driver unloads, even if it fails to register.  This is
    // fine, as that object's count will never drop to zero anyhow.  Avoiding
    // leaving this reference would make the code larger.
    //

    RtlInitUnicodeString(&callbackName, L"\\Callback\\PowerState");
    InitializeObjectAttributes(&attributes,
                               &callbackName,
                               OBJ_CASE_INSENSITIVE | OBJ_PERMANENT,
                               NULL,
                               NULL);
    status = ExCreateCallback(&PowerCallbackObj,
                              &attributes,
                              FALSE,
                              TRUE);
    if (!NT_SUCCESS(status)) {
        PowerCallbackObj = NULL;
    } else {
        PciPowerCallbackRegistrationHandle = ExRegisterCallback(PowerCallbackObj,
                                                                PciPowerStateCallback,
                                                                NULL);
    }

    PciInitConsoleLock();
    status = STATUS_SUCCESS;

exit:
    if (ccsHandle) {
        ZwClose(ccsHandle);
    }

    if (serviceHandle) {
        ZwClose(serviceHandle);
    }

    if (!NT_SUCCESS(status)) {
        if (PciRegistryPath.Buffer != NULL) {
            PciFreePool(PciRegistryPath.Buffer);
            PciRegistryPath.Buffer = NULL;
        }

        if (PciSegmentList != NULL) {
            PciFreePool(PciSegmentList);
            PciSegmentList = NULL;
        }

        if (PciWatchdogTable) {
            PciFreePool(PciWatchdogTable);
        }

        if (PciDebugPorts) {
            PciFreePool(PciDebugPorts);
        }

        if (PciHackTable) {
            PciFreePool(PciHackTable);
        }

        //
        // unhook errata manager registration.  If PCI has not registered with
        // errata manager, the call is a NOP.
        //

        PciDeregisterWithEM();
        if (ExclusionListsInitialized != FALSE) {
            RtlFreeRangeList(&PciIsaBitExclusionList);
            RtlFreeRangeList(&PciVgaAndIsaBitExclusionList);
        }
    }

    return status;
}

VOID
PciDriverUnload(
    __in PDRIVER_OBJECT DriverObject
    )
/*++

Routine Description:

    Entrypoint used to unload the PCI driver.   Does nothing, the
    PCI driver is never unloaded.

Arguments:

    DriverObject - Pointer to the driver object created by the system.

Return Value:

    None.

--*/
{
    PPCI_SEGMENT nextSegment;

    PAGED_CODE();

    PciDeinitConsoleLock();
    if (PciPowerCallbackRegistrationHandle != NULL) {
        ExUnregisterCallback(PciPowerCallbackRegistrationHandle);
    }

    if (PowerCallbackObj != NULL) {
        ObDereferenceObject(PowerCallbackObj);
    }

    //
    // Disable the hardware verifier code if appropriate.
    //

    PciVerifierUnload();

    //
    // Free the segment list
    //

    while (PciSegmentList) {
        nextSegment = PciSegmentList->Next;
        PciFreePool(PciSegmentList);
        PciSegmentList = nextSegment;
    }

    //
    // Unallocate exclusion lists.
    //

    RtlFreeRangeList(&PciIsaBitExclusionList);
    RtlFreeRangeList(&PciVgaAndIsaBitExclusionList);

    //
    // Free pool allocations made during driver entry.
    //

    RtlFreeUnicodeString(&PciRegistryPath);
    if (PciWatchdogTable) {
        PciFreePool(PciWatchdogTable);
    }

    if (PciDebugPorts) {
        PciFreePool(PciDebugPorts);
    }

    if (PciHackTable) {
        PciFreePool(PciHackTable);
    }

    PciDeregisterWithEM();

    PciFreeNumaInformation();

    //
    // Attempt to remove our hooks from the HAL.
    //

    PciUnhookHal();

    //
    // Give the arbiter library a chance to cleanup after itself.
    //

    ArbLibraryDeinitialize();

    //
    // Unregister with ETW.
    //

    PciDiagUninitialize();

    //
    // Uninitialize TraceLogging support.
    //

    PciTraceLogUninitialize();

    WPP_CLEANUP(DriverObject);
}

NTSTATUS
PciGetDebugPorts(
    __out PPCI_DEBUG_PORT *DebugPorts,
    __out PULONG DebugPortsCount
    )
/*++

Routine Description:

    Looks in the PCI service key for debug port info and puts in into
    the PciDebugPorts global table.  This routine also adds the debugging
    devices to the early restore list.

Arguments:

    ServiceHandle - handle to the PCI service key passed into DriverEntry

    DebugPorts - Head of an array of PCI_DEBUG_PORT structures to populate
        with info about the debug ports.

    DebugPortsCount - The number of debug ports populated into DebugPorts.

Return Value:

    NT Status code.

--*/
{

    ULONG BufferLength;
    ULONG Count;
    PDEBUGGING_DEVICE_IN_USE_INFORMATION DebugInformation;
    PDEBUGGING_DEVICE_IN_USE Device;
    ULONG DeviceOffset;
    ULONG Index;
    NTSTATUS Status;
    PPCI_DEBUG_PORT Table;

    C_ASSERT(MAX_DEBUGGING_DEVICES_SUPPORTED <= 999);

    PAGED_CODE();

    *DebugPorts = NULL;
    *DebugPortsCount = 0;
    Device = NULL;
    DebugInformation = NULL;
    Table = NULL;

    //
    // Retrieve the size of the debugging device from the HAL.
    //

    Status = HalQuerySystemInformation(HalQueryDebuggerInformation,
                                       0,
                                       DebugInformation,
                                       &BufferLength);

    //
    // Correctly allocate the size needed to retrieve the debugging information.
    //

    if (Status != STATUS_BUFFER_OVERFLOW) {
        goto PciGetDebugPortsEnd;
    }

    DebugInformation = PciAllocatePool(PagedPool, BufferLength);
    if (DebugInformation == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto PciGetDebugPortsEnd;
    }

    Status = HalQuerySystemInformation(HalQueryDebuggerInformation,
                                       BufferLength,
                                       DebugInformation,
                                       &BufferLength);

    if (NT_SUCCESS(Status) == FALSE) {
        goto PciGetDebugPortsEnd;
    }

    Table = PciAllocatePool(NonPagedPoolNx,
                            sizeof(PCI_DEBUG_PORT) *
                            DebugInformation->DeviceCount);

    if (Table == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto PciGetDebugPortsEnd;
    }

    Count = 0;
    DeviceOffset = 0;
    Device = DebugInformation->Device;
    for (Index = 0; Index < DebugInformation->DeviceCount; Index += 1) {
        Device = (PDEBUGGING_DEVICE_IN_USE)(((PUCHAR)Device) + DeviceOffset);
        DeviceOffset = Device->StructureLength;

        //
        // Skip any non PCI debugging devices.
        //

        if (Device->NameSpace != KdNameSpacePCI) {
            continue;
        }

        Table[Count].Segment = Device->PciDevice.Segment;
        Table[Count].Bus = Device->PciDevice.Bus;
        Table[Count].Slot.u.AsULONG = Device->PciDevice.Slot;
        NT_VERIFY(NT_SUCCESS(PciEarlyRestoreAddListItem(
            Table[Count].Bus,
            &(Table[Count].Slot),
            PCI_EARLY_RESTORE_DEBUG))
            );

        Count += 1;
    }

    *DebugPorts = Table;
    *DebugPortsCount = Count;

    //
    // Set table to not be deallocated after exposing it to the caller.
    //

    Table = NULL;

PciGetDebugPortsEnd:

    if (DebugInformation != NULL) {
        PciFreePool(DebugInformation);
    }

    if (Table != NULL) {
        PciFreePool(Table);
    }

    return Status;
}

PVOID
PciGetAcpiTable(
    __in ULONG Signature
    )
/*++

Routine Description:

    This routine will retrieve the specified ACPI table.

    N.B. This function relies on the fact that the ACPI driver has already
         registered as the provider for 'ACPI' tables.

Arguments:

    Signature - Supplies the signature of the required table.

Return Value:

    A pointer to a copy of the table, or NULL it the table cannot be found.

    The caller is responsible for freeing the memory returned by this function.

--*/
{

    NTSTATUS Status;
    PVOID Table;
    PSYSTEM_FIRMWARE_TABLE_INFORMATION TableInformation;
    ULONG TableInformationLength;
    ULONG TableLength;

    PAGED_CODE();

    Table = NULL;
    TableInformation = NULL;

    //
    // Allocate enough space to accommodate a SYSTEM_FIRMWARE_TABLE_INFORMATION
    // structure.
    //

    TableInformationLength = FIELD_OFFSET(SYSTEM_FIRMWARE_TABLE_INFORMATION,
                                          TableBuffer);

    TableInformation = PciAllocatePool(PagedPool, TableInformationLength);
    if (TableInformation == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto GetAcpiTableEnd;
    }

    RtlZeroMemory(TableInformation, TableInformationLength);
    TableInformation->ProviderSignature = 'ACPI';
    TableInformation->Action = SystemFirmwareTable_Get;
    TableInformation->TableID = Signature;
    TableInformation->TableBufferLength = 0;

    //
    // If the requested table exists, then this function should fail with
    // STATUS_BUFFER_TOO_SMALL, and the required size of the table information
    // buffer should be returned in TableInformationLengthRequired.  All other
    // failure status codes are fatal.
    //

    Status = ZwQuerySystemInformation(SystemFirmwareTableInformation,
                                      TableInformation,
                                      TableInformationLength,
                                      &TableInformationLength);

    //
    // It would be unexpected for this call to have succeeded, and so at this
    // point a successful status should be converted into a failure.
    //

    PCI_ASSERT(!NT_SUCCESS(Status));

    if (NT_SUCCESS(Status)) {
        Status = STATUS_UNSUCCESSFUL;
    }

    if (Status != STATUS_BUFFER_TOO_SMALL) {
        goto GetAcpiTableEnd;
    }

    //
    // Reallocate the TableInformation buffer allowing for the accommodation of
    // the entire requested firmware table.  Calculate the required size using
    // the size of the firmware table as a basis.
    //

    PciFreePool(TableInformation);
    TableInformation = PciAllocatePool(PagedPool, TableInformationLength);
    if (TableInformation == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto GetAcpiTableEnd;
    }

    TableLength = TableInformationLength -
        FIELD_OFFSET(SYSTEM_FIRMWARE_TABLE_INFORMATION, TableBuffer);

    //
    // Query the table a second time.  This time, it should succeed, returning
    // the entire table.
    //

    RtlZeroMemory(TableInformation, TableInformationLength);
    TableInformation->ProviderSignature = 'ACPI';
    TableInformation->Action = SystemFirmwareTable_Get;
    TableInformation->TableID = Signature;
    TableInformation->TableBufferLength = TableLength;
    Status = ZwQuerySystemInformation(SystemFirmwareTableInformation,
                                      TableInformation,
                                      TableInformationLength,
                                      &TableInformationLength);

    if (!NT_SUCCESS(Status)) {
        goto GetAcpiTableEnd;
    }

    //
    // Allocate space for the firmware table only.  This buffer will be used
    // to return the table to the caller.  This must be allocated from nonpaged
    // pool since the table data is accessed from nonpaged code.
    //

    Table = PciAllocatePool(NonPagedPoolNx,
                            TableInformation->TableBufferLength);

    if (Table == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto GetAcpiTableEnd;
    }

    //
    // Copy the firmware table into the newly allocated buffer, ready to be
    // returned to the caller.
    //

    RtlCopyMemory(Table,
                  &TableInformation->TableBuffer,
                  TableInformation->TableBufferLength);

GetAcpiTableEnd:
    if (TableInformation != NULL) {
        PciFreePool(TableInformation);
    }

    return Table;
}

NTSTATUS
PciAcpiFindRsdt(
    __out PACPI_BIOS_MULTI_NODE *AcpiMulti
    )
/*++

Routine Description:

    This routine retrieves the ACPI RSDT table.

Arguments:

    AcpiMulti - On success, will point to a buffer containing
        the RSDT.

Return Value:

    NT Status code.

--*/
{
    PCM_PARTIAL_RESOURCE_LIST partialResList;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor;
    PACPI_BIOS_MULTI_NODE multiNode;
    ULONG length, multiNodeSize;
    HANDLE subKeyHandle;
    NTSTATUS status;

    PAGED_CODE();

    *AcpiMulti = NULL;

    subKeyHandle = NULL;
    partialResList = NULL;

    status = PciGetMultifunctionSubKey(L"ACPI BIOS", &subKeyHandle);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    status = PciGetRegistryValue(L"Configuration Data",
                                 NULL,
                                 subKeyHandle,
                                 REG_FULL_RESOURCE_DESCRIPTOR,
                                 &partialResList,
                                 &length
                                 );
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    descriptor = &partialResList->PartialDescriptors[0];

    multiNode = (PACPI_BIOS_MULTI_NODE)((PCHAR)descriptor +
                sizeof(CM_PARTIAL_RESOURCE_LIST));

    multiNodeSize = sizeof(ACPI_BIOS_MULTI_NODE)
                    + ((ULONG)(multiNode->Count - 1) * sizeof(ACPI_E820_ENTRY));

    *AcpiMulti = (PACPI_BIOS_MULTI_NODE)PciAllocatePool(NonPagedPoolNx,
                                                        multiNodeSize);

    if (*AcpiMulti == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    RtlCopyMemory(*AcpiMulti, multiNode, multiNodeSize);

exit:

    if (subKeyHandle) {
        ZwClose(subKeyHandle);
    }

    if (partialResList) {
        PciFreePool(partialResList);
    }

    return status;
}

NTSTATUS
PciGetMultifunctionSubKey(
    __in PWSTR IdentifierString,
    __out PHANDLE KeyHandle
    )
/*++

Routine Description:

    Searches HKLM\Hardware\Description\System\MultiFunctionAdapter for
    a subkey with an "Identifier" value equal to the given Identifier.

Arguments:

    IdentifierString - The identifier to match to a subkey of the
        MultifunctionAdapter key.

    KeyHandle - On success, points to a handle opened to the subkey
        of the MultifunctionAdapter key that has the given Identifier.

Return Value:

    NTSTATUS - failure indicates inability to get irq routing table
    information from the registry.

--*/
{
    PKEY_FULL_INFORMATION multifunctionInfo;
    PKEY_BASIC_INFORMATION subKeyBasicInfo;
    ULONG length, subKeyBasicInfoLength;
    HANDLE multifunctionHandle;
    HANDLE subKeyHandle;
    ULONG dummy;
    NTSTATUS status;
    PUCHAR valueString;
    ULONG valueStringLength;
    ULONG i;
    BOOLEAN result;

    PAGED_CODE();

    multifunctionHandle = NULL;
    multifunctionInfo = NULL;
    subKeyBasicInfo = NULL;
    valueString = NULL;

    if (!PciOpenKey(L"\\Registry\\Machine\\HARDWARE\\DESCRIPTION\\System\\MultiFunctionAdapter",
                    NULL,
                    KEY_READ,
                    &multifunctionHandle,
                    &status
                    )) {
        goto cleanup;
    }

    //
    // Get the KeyFullInformation of the multifunction key.  This includes
    // the maximum length of subkeys.
    //
    multifunctionInfo = NULL;
    status = ZwQueryKey(multifunctionHandle,
                        KeyFullInformation,
                        NULL,
                        0,
                        &length
                        );
    if ((status != STATUS_BUFFER_TOO_SMALL) || (multifunctionHandle == 0)) {
        status = STATUS_INVALID_PARAMETER_1;
        goto cleanup;
    }
    multifunctionInfo = PciAllocatePool(PagedPool | POOL_COLD_ALLOCATION, length);
    if (multifunctionInfo == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }
    status = ZwQueryKey(multifunctionHandle,
                        KeyFullInformation,
                        multifunctionInfo,
                        length,
                        &length
                        );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    //
    // allocate space for basic information about subkeys,
    // including space for the subkey with the maximum length
    // and the terminating null that will be added later.
    //
    subKeyBasicInfoLength = multifunctionInfo->MaxNameLen
                            + sizeof(KEY_BASIC_INFORMATION)
                            + sizeof(WCHAR);

    subKeyBasicInfo = PciAllocatePool(PagedPool | POOL_COLD_ALLOCATION,
                                      subKeyBasicInfoLength
                                      );
    if (subKeyBasicInfo == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }

    //
    // Enumerate subkeys of multifunction key looking for keys with an
    // Identifier value of "PCI BIOS".  If we find one, look for the
    // irq routing table in the tree below.
    //
    i = 0;
    do {
        status = ZwEnumerateKey(multifunctionHandle,
                                i,
                                KeyBasicInformation,
                                subKeyBasicInfo,
                                subKeyBasicInfoLength,
                                &dummy
                                );
        if (!NT_SUCCESS(status)) {

            //
            // If not NT_SUCCESS, only alowable value is
            // STATUS_NO_MORE_ENTRIES,... otherwise, someone
            // is playing with the keys as we enumerate
            //
            PCI_ASSERT(status == STATUS_NO_MORE_ENTRIES);
            break;
        }

        //
        // Found a key, now we need to open it and check the
        // 'Identifier' value to see if it is 'PCI BIOS'
        //
        subKeyBasicInfo->Name[subKeyBasicInfo->NameLength / sizeof(WCHAR)] = UNICODE_NULL;
        result = PciOpenKey(subKeyBasicInfo->Name,
                            multifunctionHandle,
                            KEY_READ,
                            &subKeyHandle,
                            &status
                            );
        if (result) {

            //
            // Checking 'Identifier' value to see if it contains 'PCI BIOS'
            //
            status = PciGetRegistryValue(L"Identifier",
                                         NULL,
                                         subKeyHandle,
                                         REG_SZ,
                                         &valueString,
                                         &valueStringLength
                                         );
            if (NT_SUCCESS(status) &&
                RtlEqualMemory(IdentifierString, valueString, valueStringLength)) {

                PciFreePool(valueString);
                *KeyHandle = subKeyHandle;
                status = STATUS_SUCCESS;
                goto cleanup;
            }

            if (valueString != NULL) {
                PciFreePool(valueString);
            }

            ZwClose(subKeyHandle);
        }

        i++;

    } while (status != STATUS_NO_MORE_ENTRIES);

cleanup:

    if (subKeyBasicInfo != NULL) {
        PciFreePool(subKeyBasicInfo);
    }

    if (multifunctionInfo != NULL) {
        PciFreePool(multifunctionInfo);
    }

    if (multifunctionHandle != NULL) {
        ZwClose(multifunctionHandle);
    }

    return status;
}

NTSTATUS
PciGetSystemWideHackFlags (
    VOID
    )

/*++

Routine Description:

    This routine is invoked to fetch the system wide hack flags for PCI. These
    hack flags are obtained by querying the errata manager or the registry.

    Some of these hack flags are set for specific reasons, say when executing
    in safe mode.

Arguments:

    None

Return Value:

    Status code returned from other routines invoked in this function.

--*/

{

#if defined(_AMD64_)
    HV_CPUID_RESULT Result = {0};
#endif
    NTSTATUS Status;

    //
    // Now query the errata manager and the registry. We still support querying
    // the registry because the errata.inf is not open for public consumption
    // and this is the only way that our customers can apply the PCI hacks that
    // are published in KB articles before they get a chance to report these
    // failures to us.
    //

    Status = PciGetSystemWideHackFlagsFromEM();
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Status = PciGetSystemWideHackFlagsFromRegistry();
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Special condition hack - Disable ASPM if we are running in a safe mode.
    //

    if (*InitSafeBootMode != 0) {
        PciSystemWideHackFlags |= PCI_SYS_HACK_DISABLE_EXPRESS_ASPM;
    }

#if defined(_AMD64_)
    //
    // Check to see if this is a Hyper-V parent partition and if the underlying
    // machine supports SR-IOV.
    //

    __cpuid(&Result, HvCpuIdFunctionVersionAndFeatures);

    if (Result.VersionAndFeatures.HypervisorPresent == FALSE) {
        goto exit;
    }

    __cpuid(&Result, HvCpuIdFunctionHvInterface);

    if (Result.HvInterface.Interface != HvMicrosoftHypervisorInterface) {
        goto exit;
    }

    __cpuid(&Result, HvCpuIdFunctionHvVendorAndMaxFunction);

    if (Result.HvVendorAndMaxFunction.MaxFunction <
        HvCpuIdFunctionMsHvHardwareFeatures) {
        goto exit;
    }

    //
    // The measure for whether this partition can host SR-IOV is that an
    // IOMMU is in use and interrupt remapping should occur.
    //

    __cpuid(&Result, HvCpuIdFunctionMsHvHardwareFeatures);

    if ((Result.MsHvHardwareFeatures.DmaRemappingInUse != 0) &&
        (Result.MsHvHardwareFeatures.InterruptRemappingInUse != 0)) {
        PciSystemWideHackFlags &= ~PCI_SYS_HACK_NO_SRIOV;
    }

    exit:
#endif

    return Status;
}

NTSTATUS
PciGetSystemWideHackFlagsFromEM (
    VOID
    )

/*++

Routine Description:

    This routine is invoked to fetch the system wide hack flags for PCI by
    querying the errata manager.

Arguments:

    None

Return Value:

    STATUS_SUCCESS on success.

    Otherwise a status code returned from one of the functions invoked in this
    routine.

--*/

{

    EM_RULE_STATE EmState;
    NTSTATUS Status;

    EmState = STATE_UNKNOWN;

    //
    // IgnorePciRootBusNumberRange.
    //

    Status = EmClientQueryRuleState(
                &GUID_EM_RULE_IGNORE_PCI_ROOTBUS_NUMBER_RANGE,
                &EmState);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    if (EmState == STATE_TRUE) {
        PciSystemWideHackFlags |= PCI_SYS_HACK_IGNORE_ROOTBUS_RANGE;
    }

    //
    // IgnoreOverlappingPciRootBusNumberRanges.
    //

    Status = EmClientQueryRuleState(
                &GUID_EM_RULE_IGNORE_OVERLAPPING_PCI_ROOTBUS_NUMBER_RANGES,
                &EmState);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    if (EmState == STATE_TRUE) {
        PciSystemWideHackFlags |=
            PCI_SYS_HACK_IGNORE_OVERLAPPING_ROOTBUS_RANGES;

    }

    //
    // IgnoreCBMemLimits.
    //

    Status = EmClientQueryRuleState(&GUID_EM_RULE_IGNORE_CB_MEM_LIMITS,
                                    &EmState);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    if (EmState == STATE_TRUE) {
        PciSystemWideHackFlags |= PCI_SYS_HACK_IGNORE_CB_MEMORY_WINDOW_LIMITS;
    }

    //
    // AvoidAssertOnBadIdDataForHostBridge.
    //

    Status = EmClientQueryRuleState(
                &GUID_EM_RULE_AVOID_ASSERT_ON_BAD_ID_DATA_FOR_HOST_BRIDGE,
                &EmState);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    if (EmState == STATE_TRUE) {
        PciSystemWideHackFlags |=
            PCI_SYS_HACK_AVOID_ASSERT_ON_BAD_IDDATA_FOR_HOST_BRIDGE;

    }

    //
    // IgnoreIsaVgaBitConflict.
    //

    Status = EmClientQueryRuleState(&GUID_EM_RULE_IGNORE_ISA_VGA_BIT_CONFLICT,
                                    &EmState);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    if (EmState == STATE_TRUE) {
        PciSystemWideHackFlags |= PCI_SYS_HACK_IGNORE_ISA_VGA_CONFLICT;
    }

    //
    // DisablePciExpressASPM.
    //

    Status = EmClientQueryRuleState(&GUID_EM_RULE_DISABLE_PCI_EXPRESS_ASPM,
                                    &EmState);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    if (EmState == STATE_TRUE) {
        PciSystemWideHackFlags |= PCI_SYS_HACK_DISABLE_EXPRESS_ASPM;
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PciGetSystemWideHackFlagsFromRegistry (
    VOID
    )

/*++

Routine Description:

    This routine is invoked to fetch the system wide hack flags for PCI by
    querying the registry.

Arguments:

    None

Return Value:

    STATUS_SUCCESSS on success.

    Otherwise a status code returned by one of the functions invoked in this
    routine.

--*/

{

    HANDLE CcsHandle;
    BOOLEAN Result;
    NTSTATUS Status;
    PULONG ValueDword;
    ULONG ValueLength;

    CcsHandle = NULL;

    //
    // Need access to the CurrentControlSet for various initialization chores.
    //

    Result = PciOpenKey(CurrentControlSetKey,
                        NULL,
                        KEY_READ,
                        &CcsHandle,
                        &Status);

    if (Result == FALSE) {
        return STATUS_UNSUCCESSFUL;
    }

    //
    // Now open the HackFlags subkey. Failure to open this key is not fatal.
    //

    Status = PciGetRegistryValue(L"HackFlags",
                                 L"Control\\PnP\\PCI",
                                 CcsHandle,
                                 REG_DWORD,
                                 &ValueDword,
                                 &ValueLength);

    if (NT_SUCCESS(Status)) {
        if (ValueLength == sizeof(ULONG)) {
            PciSystemWideHackFlags |= *ValueDword;
        }

        PciFreePool(ValueDword);
        ValueDword = NULL;
    }

    //
    // Check for a WHEA error record preallocation count override.
    // Failure to open this key is not fatal.
    // Minimum is 1.
    // Cap at 128 because this will cause non-paged pool allocation.
    //

    Status = PciGetRegistryValue(L"WHEARecordCount",
                                 L"Control\\PnP\\PCI",
                                 CcsHandle,
                                 REG_DWORD,
                                 &ValueDword,
                                 &ValueLength);

    if (NT_SUCCESS(Status)) {
        if (ValueLength == sizeof(ULONG)) {
            if (*ValueDword > 128) {
                *ValueDword = 128;
            } else if (*ValueDword == 0) {
                *ValueDword = 1;
            }
            
            PciWHEARecordPreallocationCount = *ValueDword;
        }

        PciFreePool(ValueDword);
        ValueDword = NULL;
    }

    if (CcsHandle) {
        ZwClose(CcsHandle);
    }

    return STATUS_SUCCESS;
}

VOID
PciQueryInterruptSteeringInformation (
    VOID
    )

{

    ULONG InputBufferSize;
    ULONG OutputSize;
    SYSTEM_INTERRUPT_STEERING_INFORMATION_INPUT QueryBuffer;
    NTSTATUS Status;
    SYSTEM_INTERRUPT_STEERING_INFORMATION_OUTPUT SteeringInformation;

    PAGED_CODE();

    PCI_ASSERT(PciInterruptSteeringEnabled == FALSE);

    InputBufferSize = sizeof(QueryBuffer);
    OutputSize = sizeof(SteeringInformation);
    RtlZeroMemory(&QueryBuffer, InputBufferSize);
    QueryBuffer.ControllerInterrupt = TRUE;
    QueryBuffer.Gsiv = SYSTEM_INTERRUPT_STEERING_INVALID_GSIV;
    RtlZeroMemory(&SteeringInformation, OutputSize);

    //
    // Call into the kernel to determine if interrupt steering is globally
    // enabled.
    //

    Status = ZwQuerySystemInformationEx(SystemInterruptSteeringInformation,
                                        &QueryBuffer,
                                        InputBufferSize,
                                        &SteeringInformation,
                                        OutputSize,
                                        NULL);

    //
    // Not every SKU implements the query, so assume that interrupt steering
    // is disabled if the function returns a failure.
    //

    if (NT_SUCCESS(Status) && (SteeringInformation.Enabled != FALSE)) {
        PciInterruptSteeringEnabled = TRUE;
    }

    return;
}

NTSTATUS
PciGetNumaInformation (
    VOID
    )

/*++

Routine Description:

    This routine is invoked to obtain NUMA information for a system. The key
    information saved in global variables is the number of NUMA nodes, active
    processor mask for each node and index of the first processor in each node
    for round robin assignment of MSI-X messages.

Arguments:

    None.

Return Value:

    NT status value.

--*/

{

    ULONG AllocationSize;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX Current;
    ULONG GroupZeroIndex;
    ULONG HigherGroupIndex;
    ULONG Index;
    ULONG InformationLength;
    ULONG NodeIndex;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX ProcessorInformation;
    LOGICAL_PROCESSOR_RELATIONSHIP RelationshipType;
    NTSTATUS Status;

    PAGED_CODE();

    PciProcessorCount = KeQueryActiveProcessorCountEx(ALL_PROCESSOR_GROUPS);
    InformationLength = 0;
    ProcessorInformation = NULL;
    RelationshipType = RelationNumaNode;
    Status = NtQuerySystemInformationEx(
                    SystemLogicalProcessorAndGroupInformation,
                    &RelationshipType,
                    sizeof(RelationshipType),
                    NULL,
                    0,
                    &InformationLength);

    if (Status != STATUS_INFO_LENGTH_MISMATCH) {
        Status = STATUS_UNSUCCESSFUL;
        goto Exit;
    }

    ProcessorInformation = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)
                           PciAllocatePool(PagedPool, InformationLength);

    if (ProcessorInformation == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    Status = NtQuerySystemInformationEx(
                    SystemLogicalProcessorAndGroupInformation,
                    &RelationshipType,
                    sizeof(RelationshipType),
                    ProcessorInformation,
                    InformationLength,
                    &InformationLength);

    if (!NT_SUCCESS(Status)) {
        goto Cleanup;
    }

    //
    // Process NUMA information in two passes. In the first pass count the
    // number of nodes and allocate memory for the relevant information.
    //

    Current = ProcessorInformation;
    while (InformationLength >=
           FIELD_OFFSET(SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, Processor)) {

        if (InformationLength < Current->Size) {
            break;
        }

        PCI_ASSERT(Current->Relationship == RelationNumaNode);

        PciNumaNodeCount += 1;
        if (Current->NumaNode.GroupMask.Group == 0) {
            PciGroupZeroNumaNodeCount += 1;
        }

        InformationLength -= Current->Size;
        Current = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)
                  (((PCHAR)Current) + Current->Size);
    }

    PCI_ASSERT(PciNumaNodeCount > 0);
    PCI_ASSERT(PciGroupZeroNumaNodeCount > 0);

    //
    // Allocate memory to save processor mask for each node.
    //

    AllocationSize = sizeof(GROUP_AFFINITY) * PciNumaNodeCount;
    PciNumaNodesProcessorMask =
        (PGROUP_AFFINITY)PciAllocatePool(PagedPool, AllocationSize);

    if (PciNumaNodesProcessorMask == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    RtlZeroMemory(PciNumaNodesProcessorMask, AllocationSize);

    //
    // Allocate memory to save current processor for each node.
    //

    AllocationSize = sizeof(UCHAR) * PciNumaNodeCount;
    PciRoundRobinCurrentProcessor =
        (PUCHAR)PciAllocatePool(PagedPool, AllocationSize);

    if (PciRoundRobinCurrentProcessor == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Cleanup;
    }

    RtlZeroMemory(PciRoundRobinCurrentProcessor, AllocationSize);

    //
    // In the second pass, save the information on active processor mask for
    // each node as well as an index of the first processor in every node for
    // round robin assignment. All nodes in group 0 are saved before any nodes
    // in higher groups. This is so that they can be easily accessed for
    // drivers that are not group-aware.
    //

    Current = ProcessorInformation;
    GroupZeroIndex = 0;
    HigherGroupIndex = PciGroupZeroNumaNodeCount;
    for (Index = 0; Index < PciNumaNodeCount; Index += 1) {
        if (Current->NumaNode.GroupMask.Group == 0) {
            NodeIndex = GroupZeroIndex;
            GroupZeroIndex += 1;

        } else {
            NodeIndex = HigherGroupIndex;
            HigherGroupIndex += 1;
        }

        PciNumaNodesProcessorMask[NodeIndex] = Current->NumaNode.GroupMask;
        PciRoundRobinCurrentProcessor[NodeIndex] =
            RtlFindLeastSignificantBit(Current->NumaNode.GroupMask.Mask);

        Current = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)
                  (((PCHAR)Current) + Current->Size);
    }

    PCI_ASSERT(GroupZeroIndex == PciGroupZeroNumaNodeCount);
    PCI_ASSERT(HigherGroupIndex == PciNumaNodeCount);

    Status = STATUS_SUCCESS;
    goto Exit;

Cleanup:
    PciFreeNumaInformation();

Exit:
    if (ProcessorInformation != NULL) {
        PciFreePool(ProcessorInformation);
    }

    return Status;
}

VOID
PciInitializeNumaInformation (
    VOID
    )

/*++

Routine Description:

    This routine is invoked to initialize global variables that store NUMA
    information.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PciNumaNodeCount = 0;
    PciGroupZeroNumaNodeCount = 0;
    PciProcessorCount = 0;
    PciNumaNodesProcessorMask = NULL;
    PciRoundRobinCurrentProcessor = NULL;
    KeInitializeEvent(&PciNumaInformationLock, SynchronizationEvent, TRUE);
    return;
}

VOID
PciFreeNumaInformation (
    VOID
    )

/*++

Routine Description:

    This routine is invoked to free NUMA information saved in global variables.

Arguments:

    None.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    PciNumaNodeCount = 0;
    PciGroupZeroNumaNodeCount = 0;
    PciProcessorCount = 0;
    if (PciNumaNodesProcessorMask != NULL) {
        PciFreePool(PciNumaNodesProcessorMask);
        PciNumaNodesProcessorMask = NULL;
    }

    if (PciRoundRobinCurrentProcessor != NULL) {
        PciFreePool(PciRoundRobinCurrentProcessor);
        PciRoundRobinCurrentProcessor = NULL;
    }

    return;
}

_IRQL_requires_same_
_Function_class_(CALLBACK_FUNCTION)
VOID
PciPowerStateCallback (
    _In_opt_ PVOID CallbackContext,
    _In_opt_ PVOID Argument1,
    _In_opt_ PVOID Argument2
    )

/*++

Routine Description:

    This routine is invoked at the very beginning and the very end of system
    power state transitions.  It's job is to set a global variable that allows
    the power management code to know when a S-state transition is active.

Arguments:

    CallbackContext - Context supplied at registration time (unused.)

    Argument1 - Indicates which reason this callback is being invoked.  (See
        MSDN for more details.)

    Argument2 - Parameter associated with the reason given in Argument 1.

Return Value:

    None.

--*/

{
    PAGED_CODE();

    switch ((ULONG_PTR)Argument1) {
    case PO_CB_SYSTEM_STATE_LOCK:
        if ((ULONG_PTR)Argument2 == 0) {
            PciSystemPowerStateTransitionActive = TRUE;
        } else {
            PciSystemPowerStateTransitionActive = FALSE;
        }
        break;
    }

    return;
    UNREFERENCED_PARAMETER(CallbackContext);
}
