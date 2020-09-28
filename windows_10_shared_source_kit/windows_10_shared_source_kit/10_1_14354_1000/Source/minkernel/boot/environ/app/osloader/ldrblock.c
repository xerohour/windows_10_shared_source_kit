/*++

Copyright (c) Microsoft Corporation

Module Name:

    ldrblock.c

Abstract:

    This module contains routines for populating the OS loader block.

Environment:

    Boot

--*/

//
// ------------------------------------------------------------------- Includes
//

#include "osloader.h"
#include "bltracelogging.h"
#include <bootlibp.h>

//
// ---------------------------------------------------------------- Definitions
//

#define PAGEFILE_SYS L"\\pagefile.sys"

//
// What follows is a list of valid supported baud rates, as well as their
// representation in the SPCR table.
//

typedef struct _BAUD_RATE_VECTOR {
    UCHAR Identifier;
    ULONG BaudRate;
} BAUD_RATE_VECTOR;

const BAUD_RATE_VECTOR OslConsoleBaudRates[] = {
    {3, 9600},
    {4, 19200},
    {6, 57600},
    {7, 115200}
};

#define KERNEL_MAP_ATTEMPTS_TOTAL 4

#define KSEG_PAGE(_page) \
    ((((_page) << PAGE_SHIFT) & KSEG0_BASE) == KSEG0_BASE)

#define CompareDisks(_x, _y) \
    ((((_x)->PartitionType == MBR) && \
      ((_y)->PartitionType == MBR) && \
      ((_x)->Mbr.Signature == (_y)->Mbr.Signature)) || \
     (((_x)->PartitionType == GPT) && \
      ((_y)->PartitionType == GPT) && \
      RtlEqualMemory(&(_x)->Gpt.Signature, \
                     &(_y)->Gpt.Signature, \
                     sizeof(GUID)) != FALSE))

const GUID OSL_INVALID_GUID =
    {0xFFFFFFFF, 0xFFFF, 0xFFFF, {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

const GUID OSL_NULL_GUID =
    {0x00000000, 0x0000, 0x0000, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}};

//
// WFS FP data definitions.
//

#define FP_FILE_NAME   L"config\\FP"

//
// A formatting string used to construct the BIOS version string according to
// the ACPI and constants regarding the size of the BIOS version string.
//

#define ACPI_BIOS_VERSION_SEPARATOR_AND_REVISION_FORMAT L" - %X"
#define ACPI_BIOS_VERSION_SEPARATOR_CHARACTER_COUNT 3
#define MAX_ACPI_BIOS_REVISION_CHARACTER_COUNT 8
#define ACPI_BIOS_VERSION_BUFFER_SIZE \
    (((ACPI_MAX_OEM_ID + \
       ACPI_BIOS_VERSION_SEPARATOR_CHARACTER_COUNT + \
       MAX_ACPI_BIOS_REVISION_CHARACTER_COUNT) * sizeof(WCHAR)) + \
     sizeof(UNICODE_NULL))

//
// ----------------------------------------------------------------- Prototypes
//

NTSTATUS
OslpBuildKernelMemoryMap (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in PLIST_ENTRY LoaderMemoryMap,
    __in ULONG KernelBufferSize,
    __out_bcount(KernelBufferSize) PMEMORY_ALLOCATION_DESCRIPTOR KernelMapBuffer,
    __out PLIST_ENTRY FreeDescriptorsList,
    __out PULONG ReservedDescriptorsCount
    );

ULONG
OslpConvertBaudRate (
    UCHAR Identifier
    );

VOID
OslpConvertMemoryDescriptor (
    __out PMEMORY_ALLOCATION_DESCRIPTOR KernelDescriptor,
    __in PMEMORY_DESCRIPTOR LibraryDescriptor
    );

ULONG
OslpSplitKernelHalMemoryDescriptor (
    __in PMEMORY_DESCRIPTOR LibraryDescriptor,
    __inout_ecount(4) PMEMORY_ALLOCATION_DESCRIPTOR KernelDescriptor,
    __in PKLDR_DATA_TABLE_ENTRY KernelEntry,
    __in PKLDR_DATA_TABLE_ENTRY HalEntry
    );

ULONG
OslpSplitSinglePageMemoryDescriptor (
    __in PMEMORY_DESCRIPTOR LibraryDescriptor,
    __inout_ecount(3) PMEMORY_ALLOCATION_DESCRIPTOR KernelDescriptor,
    __in ULONGLONG SinglePage,
    __in MEMORY_TYPE SinglePageType
    );

NTSTATUS
OslpConvertOptionToSubString (
    __in PBOOT_ENTRY_OPTION OptionList,
    __in PBOOT_ENTRY_OPTION Option,
    __out_opt PCHAR OptionString,
    __in ULONG BufferLength,
    __out PULONG StringLength
    );

NTSTATUS
OslpCopyEmsSettings (
    __in_opt PSMBIOS3_EPS_HEADER SmBiosEps,
    __deref_out PHEADLESS_LOADER_BLOCK *EmsLoaderBlock
    );

NTSTATUS
OslpCreateArcDiskSignature (
    __in DEVICE_ID DeviceId,
    __out PARC_DISK_SIGNATURE *DiskSignatureOut
    );

NTSTATUS
OslpGetSpcrTable (
    __deref_out PSERIAL_PORT_REDIRECTION_TABLE *SpcrTable
    );

NTSTATUS
OslpInitializeNetworkLoaderBlock (
    __deref_out PNETWORK_LOADER_BLOCK *NetworkLoaderBlock
    );

NTSTATUS
OslpInitializeDevicePathInformation (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslpLoadSystemGuid (
    __in_opt PSMBIOS3_EPS_HEADER SmBiosEps,
    __out GUID *Guid
    );

NTSTATUS
OslpFwSaveCapsuleDump (
    __in DEVICE_ID TriageDeviceId
    );

NTSTATUS
OslpLoadCrashDump (
    __in DEVICE_ID TriageDeviceId,
    __deref_out_bcount_opt(sizeof(DUMP_HEADER)) PVOID *DumpHeaderAddress,
    __deref_out_bcount_opt(TRIAGE_DUMP_SIZE) PVOID *TriageDumpAddress
    );

NTSTATUS
OslpPersistData (
    __in PMEMORY_DESCRIPTOR MemoryDescriptor,
    __out PBAPD_LIST_ITEM *PdDescription
    );

NTSTATUS
OslpStampSignature (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in PARC_DISK_SIGNATURE DiskSignature,
    __in PLIST_ENTRY DiskSignaturesListHead
    );

NTSTATUS
OslpCheckOsRamdiskDeviceDescriptor (
    __in PMEMORY_DESCRIPTOR Descriptor
    );

BOOLEAN
OslpGetSmBiosTableString (
    __in PSMBIOS_STRUCT_HEADER TableBase,
    __in ULONG TableLength,
    __in UCHAR StringIndex,
    __out PSTR* BiosString,
    __out PULONG BiosStringLength
    );

NTSTATUS
OslpGetNextSmBiosTable (
    __inout PULONG StructureTableRemainingLength,
    __in PSMBIOS_STRUCT_HEADER CurrentTable,
    __out PULONG CurrentTableLength,
    __out PSMBIOS_STRUCT_HEADER* NextTable
    );

BOOLEAN
OslpAddSmBiosStringToHash (
    __in PSMBIOS_STRUCT_HEADER TableBase,
    __in ULONG TableLength,
    __in UCHAR StringIndex,
    __inout PSYMCRYPT_SHA1_STATE ShaContext
    );

NTSTATUS
OslpHashSmBiosStrings (
    __in_opt PSMBIOS3_EPS_HEADER SmBiosEps,
    __inout PSYMCRYPT_SHA1_STATE ShaContext
    );

NTSTATUS
OslpGenerateHardwareConfigurationId (
    __in_opt PSMBIOS3_EPS_HEADER SmBiosEps,
    __out PGUID HardwareConfigurationId
    );

VOID
OslpGetHardwareConfigurationId (
    __in_opt PSMBIOS3_EPS_HEADER SmBiosEps,
    __out PGUID HardwareConfigurationId
    );

NTSTATUS
OslpGetLastReferenceTime (
    __in PBSD_POWER_TRANSITION PowerTransition,
    __out PTIME_FIELDS TimeFields
    );

VOID
OslpGetWindowsBuildTime (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __out PTIME_FIELDS TimeFields
    );

NTSTATUS
OslpGetAcpiBiosVersion (
    __inout PUNICODE_STRING Version
    );

NTSTATUS
OslpGetSmbiosVersion (
    __inout PUNICODE_STRING Version,
    __in PSMBIOS3_EPS_HEADER SmBiosEps
    );

NTSTATUS
OslpInitializeManufacturingInformation (
    __in ULONG SystemHiveHandle,
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

//
// ------------------------------------------------------------------ Functions
//

NTSTATUS
OslInitializeLoaderBlock (
    __in DEVICE_ID OsDeviceId,
    __in ULONG SystemHiveHandle,
    __deref_out PLOADER_PARAMETER_BLOCK *LoaderBlockAddress
    )

/*++

Routine Description:

    Allocates a loader parameter block and initializes it with any information
    it can from the input boot entry.

Arguments:

    OsDeviceId - The id of device containing the OS.

    SystemHiveHandle - Supplies a handle to the system hive.

    LoaderBlockAddress - On output, contains the pointer of the allocated
        loader block.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY when an allocation fails.

--*/

{

    PVOID AcpiTableData;
    ULONG AcpiTableDataSize;
    BOOLEAN BootLogEnabled;
    BOOLEAN FirmwareInformationIsInitialized;
    DEVICE_INFORMATION DeviceInformation;
    LARGE_INTEGER Frequency;
    ULONG Index;
    UINTN Length;
    PLOADER_PARAMETER_BLOCK LoaderBlock;
    PNETWORK_LOADER_BLOCK NetLoaderBlock;
    PHYSICAL_ADDRESS PhysicalAddress;
    PSMBIOS3_EPS_HEADER SmBiosEps;
    NTSTATUS Status;

    *LoaderBlockAddress = NULL;
    FirmwareInformationIsInitialized = FALSE;

    //
    // Allocate memory for the loader block.
    //

    LoaderBlock = BlMmAllocateHeap(sizeof(LOADER_PARAMETER_BLOCK));
    if (LoaderBlock == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OslInitializeLoaderBlockEnd;
    }

    RtlZeroMemory(LoaderBlock, sizeof(LOADER_PARAMETER_BLOCK));

    //
    // Initialize the loader block.
    //

    LoaderBlock->OsMajorVersion = VER_PRODUCTMAJORVERSION;
    LoaderBlock->OsMinorVersion = VER_PRODUCTMINORVERSION;
    LoaderBlock->Size = sizeof(LOADER_PARAMETER_BLOCK);
    LoaderBlock->OsLoaderSecurityVersion = OSLOADER_SECURITY_VERSION_CURRENT;

    //
    // Set the load options string.  Use an empty string if there are no
    // options.  Otherwise, use the converted load options string, converting
    // it to upper case and replacing slashes with spaces.
    //

    if (OslLoadOptions != NULL) {
        _strupr(OslLoadOptions);
        Length = strlen(OslLoadOptions);
        for (Index = 0; Index < Length; Index += 1) {
            if (OslLoadOptions[Index] == '/') {
                OslLoadOptions[Index] = ' ';
            }
        }

        LoaderBlock->LoadOptions = OslLoadOptions;

    } else {
        LoaderBlock->LoadOptions = BlMmAllocateHeap(sizeof(""));
        if (LoaderBlock->LoadOptions == NULL) {
            Status = STATUS_NO_MEMORY;
            goto OslInitializeLoaderBlockEnd;
        }

        RtlCopyMemory(LoaderBlock->LoadOptions, "", sizeof(""));
    }

    //
    // Initialize the configuration tree (requires LoadOptions to be
    // initialized).
    //

    Status = OslLegacyHardwareDetection(LoaderBlock);
    if (!NT_SUCCESS(Status)) {
        goto OslInitializeLoaderBlockEnd;
    }

    //
    // Build all the device path information for the loader block.
    //

    Status = OslpInitializeDevicePathInformation(LoaderBlock);
    if (!NT_SUCCESS(Status)) {
        goto OslInitializeLoaderBlockEnd;
    }

    //
    // Initialize all linked lists.
    //

    InitializeListHead(&LoaderBlock->LoadOrderListHead);
    InitializeListHead(&LoaderBlock->BootDriverListHead);
    InitializeListHead(&LoaderBlock->EarlyLaunchListHead);
    InitializeListHead(&LoaderBlock->CoreDriverListHead);
    InitializeListHead(&LoaderBlock->CoreExtensionsDriverListHead);
    InitializeListHead(&LoaderBlock->TpmCoreDriverListHead);
    InitializeListHead(&LoaderBlock->MemoryDescriptorListHead);

    //
    // Initialize loader parameter extension.
    //

    LoaderBlock->Extension =
        BlMmAllocateHeap(sizeof(LOADER_PARAMETER_EXTENSION));

    if (LoaderBlock->Extension == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OslInitializeLoaderBlockEnd;
    }

    RtlZeroMemory(LoaderBlock->Extension, sizeof(LOADER_PARAMETER_EXTENSION));
    LoaderBlock->Extension->Size = sizeof(LOADER_PARAMETER_EXTENSION);

    //
    // The OS loader does not support selecting a profile.  (This was done in
    // previous OS loaders by prompting the user to explicitly select a profile
    // or by detecting the docking state via PC/AT bios interrupt service.
    // Not exposing UI is a simplication for a feature that is no longer used
    // by customers, and with the move to make Windows and ACPI-only OS, using
    // bios services is no longer appropriate either.)
    // Therefore the OS loader passes in the HW_PROFILE_STATUS_TRUE_MATCH flag,
    // which indicates that a profile was explicitly selected.
    //

    LoaderBlock->Extension->Profile.Status = HW_PROFILE_STATUS_TRUE_MATCH;

    //
    // An ACPI OS must evaluate ACPI control methods on the docking namespace to
    // evaluate the current docking state.  It is currently not possible for
    // the OS Loader to run the ACPI interpreter to determine the current
    // docking state, so it passes the unknown docking state flag and unknown
    // docking information.  The OS is responsible for determining docking
    // state later on.
    //

    LoaderBlock->Extension->Profile.DockingState = HW_PROFILE_DOCKSTATE_UNKNOWN;
    LoaderBlock->Extension->Profile.Capabilities = 0;
    LoaderBlock->Extension->Profile.DockID = 0;
    LoaderBlock->Extension->Profile.SerialNumber = 0;

    //
    // Set the identifier for this application.
    //

    LoaderBlock->Extension->BootIdentifier = OslApplicationIdentifier;

    //
    // Identify the osloader's release to the kernel.
    //

    LoaderBlock->Extension->MajorRelease = NTDDI_VERSION;
    LoaderBlock->Extension->Reserved = 0;

    InitializeListHead(&LoaderBlock->Extension->FirmwareDescriptorListHead);

    //
    // Set ACPI tables used by the loader to be used by the kernel too.
    //

    Status = BlUtlGetAcpiTableOverrides(&AcpiTableData, &AcpiTableDataSize);
    if (NT_SUCCESS(Status) && (AcpiTableDataSize > 0)) {
        Status = BlMmAllocatePhysicalPages(&PhysicalAddress,
                                           (ALIGN_RANGE_UP(AcpiTableDataSize, PAGE_SIZE) /
                                            PAGE_SIZE),
                                           OSL_MEMORY_TYPE_ACPI_TABLE,
                                           MEMORY_ATTRIBUTE_NONE,
                                           0);

        if (!NT_SUCCESS(Status)) {
            goto OslInitializeLoaderBlockEnd;
        }

        Status = BlMmMapPhysicalAddress(&LoaderBlock->Extension->AcpiTable,
                                        PhysicalAddress,
                                        AcpiTableDataSize,
                                        MEMORY_ATTRIBUTE_NONE);

        if (!NT_SUCCESS(Status)) {
            BlMmFreePhysicalPages(PhysicalAddress);
            goto OslInitializeLoaderBlockEnd;
        }

        RtlCopyMemory(LoaderBlock->Extension->AcpiTable,
                      AcpiTableData,
                      AcpiTableDataSize);

        LoaderBlock->Extension->AcpiTableSize = AcpiTableDataSize;
    }


    //
    // Save dump persisted in firmware back to the page file where
    // the kernel expects to find it.
    //

    OslpFwSaveCapsuleDump(OsDeviceId);

    //
    // Load the crash dump header if one exists. If the dump is a triage dump,
    // load the entire dump as well for triage.
    //

    OslpLoadCrashDump(OsDeviceId,
                      &LoaderBlock->Extension->DumpHeader,
                      &LoaderBlock->Extension->TriageDumpBlock);

    //
    // Copy the Offline Crashdump Configuration Table to the loader block
    // if it exists.
    //

    OslCopyOfflineCrashdumpTable(
        &LoaderBlock->Extension->OfflineCrashdumpConfigurationTable);

    //
    // Copy the SMBIOS Table to the loader block.  Do not consider a failure
    // fatal, as boot can succeed without the SMBIOS table.
    //

    BlUtlCopySmBiosTable(&LoaderBlock->Extension->SMBiosEPSHeader);

    //
    // Initialize the EMS settings in the loader block.  Do not consider a
    // failure fatal, as boot can succeed without the headless settings
    // being present.
    //

    OslpCopyEmsSettings(LoaderBlock->Extension->SMBiosEPSHeader,
                        &LoaderBlock->Extension->HeadlessLoaderBlock);

    //
    // If we are doing a PXE boot, initialize the NetworkLoaderBlock in the
    // loader extension.
    //

    Status = OslpInitializeNetworkLoaderBlock(&NetLoaderBlock);
    if (!NT_SUCCESS(Status)) {
        goto OslInitializeLoaderBlockEnd;
    }

    LoaderBlock->Extension->NetworkLoaderBlock = NetLoaderBlock;

    //
    // Initialize performance data we want to pass into the OS.
    //

    LoaderBlock->Extension->LoaderPerformanceData =
                            BlMmAllocateHeap(sizeof(LOADER_PERFORMANCE_DATA));

    if (!LoaderBlock->Extension->LoaderPerformanceData) {
        Status = STATUS_NO_MEMORY;
        goto OslInitializeLoaderBlockEnd;
    }

    RtlZeroMemory(LoaderBlock->Extension->LoaderPerformanceData,
                  sizeof(LOADER_PERFORMANCE_DATA));

    //
    // Initialize the persistent data list that is passed into the OS.
    //

    InitializeListHead(&LoaderBlock->Extension->BootApplicationPersistentData);

    //
    // Determine the number of scratch pages required to resume from hibernate.
    //

    Status = OslInitializeResumePages(OsDeviceId,
                                      &LoaderBlock->Extension->ResumePages);

    if (!NT_SUCCESS(Status)) {
        goto OslInitializeLoaderBlockEnd;
    }

    //
    // Initialize firmware-specific information that needs to be passed to
    // the OS.
    //

    Status = OslFwInitializeLoaderBlock(&LoaderBlock->FirmwareInformation);
    if (!NT_SUCCESS(Status)) {
        goto OslInitializeLoaderBlockEnd;
    }

    FirmwareInformationIsInitialized = TRUE;

    //
    // Initialize the attached hives.
    //

    InitializeListHead(&LoaderBlock->Extension->AttachedHives);

    //
    // Store the frequency of the boot processor's counter.
    //
    // N.B. This assumes the boot libraries performance counter is implemented
    //      using the TSC/ITC.
    //

    BlTimeQueryPerformanceCounter(&Frequency);
    LoaderBlock->Extension->ProcessorCounterFrequency = Frequency.QuadPart;

    //
    // Initialize the flag indicating whether the platform supports access to
    // IO ports.
    //

    if (BlArchQueryIoPortAccessSupported() != FALSE) {
        LoaderBlock->Extension->IoPortAccessSupported = 1;
    }

    //
    // Compute the hardware configuration Id to be used for PNP roaming.
    //

    OslpGetHardwareConfigurationId(
        LoaderBlock->Extension->SMBiosEPSHeader,
        &LoaderBlock->Extension->HardwareConfigurationId);

    //
    // Store firmware version strings.
    //

    OslpGetAcpiBiosVersion(&LoaderBlock->Extension->AcpiBiosVersion);
    SmBiosEps = LoaderBlock->Extension->SMBiosEPSHeader;
    if (SmBiosEps != NULL) {
        OslpGetSmbiosVersion(&LoaderBlock->Extension->SmbiosVersion, SmBiosEps);
    }

    OslFwGetEfiVersion(&LoaderBlock->Extension->EfiVersion);

    //
    // Initialize the API set schema extension list.
    //

    InitializeListHead(&LoaderBlock->Extension->ApiSetSchemaExtensions);

    //
    // Record if boot logging is enabled.
    //

    Status = BlGetApplicationOptionBoolean(BCDE_OSLOADER_TYPE_LOG_INITIALIZATION,
                                           &BootLogEnabled);

    if (NT_SUCCESS(Status) && (BootLogEnabled != FALSE)) {
        LoaderBlock->Extension->BootLogEnabled = TRUE;
    }

    //
    // Initialize manufacturing information.
    //

    Status = OslpInitializeManufacturingInformation(SystemHiveHandle,
                                                    LoaderBlock);

    if (!NT_SUCCESS(Status)) {
        goto OslInitializeLoaderBlockEnd;
    }

#if defined(_AMD64_)

    //
    // Record the address of any hypercall code page that may be used.
    //

    LoaderBlock->Extension->HypercallCodeVa = OslArchGetHypercallCodePage();

#endif

    //
    // For VMBUS, Ntos will continue with Smb/Wcifs Boot.
    //

    Status = BlDeviceGetInformation(OsDeviceId, &DeviceInformation);
    if (NT_SUCCESS(Status) && (DeviceInformation.DeviceType == DEVICE_VMBUS)) {
        LoaderBlock->Extension->IsSmbboot = TRUE;
    }

    //
    // Return the loader block address to the caller.
    //

    *LoaderBlockAddress = LoaderBlock;

OslInitializeLoaderBlockEnd:

    if (!NT_SUCCESS(Status)) {

        //
        // Clean up our allocations due to failure.
        //

        if (LoaderBlock != NULL) {
            if (LoaderBlock->Extension != NULL) {
                if (LoaderBlock->Extension->LoaderPerformanceData != NULL) {
                    BlMmFreeHeap(LoaderBlock->Extension->LoaderPerformanceData);
                    LoaderBlock->Extension->LoaderPerformanceData = NULL;
                }

                if (LoaderBlock->Extension->NetworkLoaderBlock != NULL) {
                    BlMmFreeHeap(LoaderBlock->Extension->NetworkLoaderBlock);
                    LoaderBlock->Extension->NetworkLoaderBlock = NULL;
                }

                if (LoaderBlock->Extension->DumpHeader != NULL) {
                    if (LoaderBlock->Extension->TriageDumpBlock != NULL) {
                        BlMmFreeHeap(LoaderBlock->Extension->TriageDumpBlock);
                    } else {
                        BlMmFreeHeap(LoaderBlock->Extension->DumpHeader);
                    }
                    LoaderBlock->Extension->DumpHeader = NULL;
                    LoaderBlock->Extension->TriageDumpBlock = NULL;
                }

                if (LoaderBlock->Extension->SMBiosEPSHeader != NULL) {
                    BlMmFreeHeap(LoaderBlock->Extension->SMBiosEPSHeader);
                    LoaderBlock->Extension->SMBiosEPSHeader = NULL;
                }

                if (NULL != LoaderBlock->Extension->AcpiTable) {
                    if (BlMmTranslateVirtualAddress(LoaderBlock->Extension->AcpiTable,
                                                    &PhysicalAddress)) {

                        BlMmUnmapVirtualAddress(LoaderBlock->Extension->AcpiTable,
                                                LoaderBlock->Extension->AcpiTableSize);

                        BlMmFreePhysicalPages(PhysicalAddress);
                    }
                }

                RtlFreeUnicodeString(&LoaderBlock->Extension->AcpiBiosVersion);
                RtlFreeUnicodeString(&LoaderBlock->Extension->SmbiosVersion);
                RtlFreeUnicodeString(&LoaderBlock->Extension->EfiVersion);

                BlMmFreeHeap(LoaderBlock->Extension);
                LoaderBlock->Extension = NULL;
            }

            if (FirmwareInformationIsInitialized != FALSE) {
                OslFwDestroyLoaderBlock(&LoaderBlock->FirmwareInformation);
            }

            if ((LoaderBlock->LoadOptions != NULL) &&
                (LoaderBlock->LoadOptions != OslLoadOptions)) {

                BlMmFreeHeap(LoaderBlock->LoadOptions);
                LoaderBlock->LoadOptions = NULL;
            }

            BlMmFreeHeap(LoaderBlock);
            LoaderBlock = NULL;
        }
    }

    return Status;
}

NTSTATUS
OslpCreateVhdSignature (
    __in PBOOT_ENVIRONMENT_DEVICE VhdDevice,
    __deref_out PVHD_DISK_SIGNATURE *VhdDiskSignature
    )

/*++

Routine Description:

    This routine allocates and initializes the VHD disk signature that will be
    used by the OS to surface the VHD from which it is booting.

Arguments:

    VhdDevice - Supplies the VHD boot device containing the OS.

    VhdDiskSignature - Supplies a pointer to allocate and return the VHD disk
        signature.

Return Value:

    NT status code.

--*/

{

    DEVICE_ID DeviceId;
    DEVICE_INFORMATION DeviceInformation;
    PDEVICE_PARTITION_INFORMATION PartitionInformation;
    NTSTATUS Status;
    PBOOT_ENVIRONMENT_DEVICE VhdFileDevice;
    PBOOT_ENVIRONMENT_DEVICE VhdParentPartitionDevice;
    ULONG VhdParentPartitionNumber;
    PVHD_DISK_SIGNATURE  VhdSignature;

    //
    // Validate parameters.
    //

    if ((VhdDevice == NULL) ||
        (VhdDiskSignature == NULL) ||
        (VhdDevice->DeviceType != DEVICE_PARTITION_EX) ||
        (VhdDevice->u.PartitionEx.ParentIdentifier.BlockIoType !=
         BLOCK_IO_VIRTUAL_HARD_DISK)) {

        Status = STATUS_INVALID_PARAMETER;
        goto CreateVhdSignatureEnd;
    }

    //
    // The OS needs the partition number for the VHD file's parent partition
    // before OS VHD initialization can occur.  Locate the boot device
    // representing the VHD file's parent partition, then query it for the
    // required information.
    //

    ASSERT((VhdDevice->Size - VHD_PARTITION_EX_FILE_OFFSET) >=
           FILE_DEVICE_MINIMUM_SIZE);

    VhdFileDevice = Add2Ptr(VhdDevice, VHD_PARTITION_EX_FILE_OFFSET);

    ASSERT((VhdFileDevice->DeviceType == DEVICE_BLOCK_IO) &&
           (VhdFileDevice->u.BlockIo.BlockIoType == BLOCK_IO_FILE));

    VhdParentPartitionDevice =
        BlGetDevice(&VhdFileDevice->u.BlockIo.u.File.SourcePath);

    if (VhdParentPartitionDevice == NULL) {
        Status = STATUS_VHD_FORMAT_UNKNOWN;
        goto CreateVhdSignatureEnd;
    }

    //
    // Open the VHD file's parent partition device and obtain its number.
    //

    Status = BlDeviceOpen(VhdParentPartitionDevice, OPEN_READ, &DeviceId);
    if (!NT_SUCCESS(Status)) {
        goto CreateVhdSignatureEnd;
    }

    Status = BlDeviceGetInformation(DeviceId, &DeviceInformation);
    BlDeviceClose(DeviceId);
    if (!NT_SUCCESS(Status)) {
        goto CreateVhdSignatureEnd;
    }

    ASSERT((DeviceInformation.DeviceType == DEVICE_BLOCK_IO) &&
           (DeviceInformation.u.BlockIo.Type == BLOCK_IO_PARTITION));

    PartitionInformation =
        &DeviceInformation.u.BlockIo.DeviceInformation.Partition;

    if (DeviceInformation.u.BlockIo.PartitionType == MBR) {
        VhdParentPartitionNumber = PartitionInformation->Mbr.Number;

    } else if (DeviceInformation.u.BlockIo.PartitionType== GPT) {
        VhdParentPartitionNumber = PartitionInformation->Gpt.Number;

    } else {
        Status = STATUS_VHD_FORMAT_UNKNOWN;
        goto CreateVhdSignatureEnd;
    }

    //
    // Allocate and initialize the VHD disk signature return buffer.
    //

    VhdSignature = BlMmAllocateHeap(VhdDevice->Size +
                                    FIELD_OFFSET(VHD_DISK_SIGNATURE,
                                                 BootDevice));

    if (VhdSignature == NULL) {
        Status = STATUS_NO_MEMORY;
        goto CreateVhdSignatureEnd;
    }

    VhdSignature->ParentPartitionNumber = VhdParentPartitionNumber;
    RtlCopyMemory(VhdSignature->BootDevice,
                  VhdDevice,
                  VhdDevice->Size);

    *VhdDiskSignature = VhdSignature;

CreateVhdSignatureEnd:
    return Status;
}

VOID
OslpGetVhdParentDeviceAndFilePath (
    __in PBOOT_ENVIRONMENT_DEVICE VirtualDisk,
    __out PBOOT_ENVIRONMENT_DEVICE *VhdFileParent,
    __out_opt PWSTR *VhdFilePath
    )

/*++

Routine Desciption:

    This routine asserts that the boot VHD disk device is correctly formed,
    and returns its parent device, and relative file path.

Arguments:

    VirtualDisk - Supplies the block IO virtual hard disk device.

    VhdFileParent - Supplies a buffer which returns the VHD parent device.

    VhdFilePath - Supplies an optional buffer which returns the VHD's file
        path.

Return Value:

    None.

--*/

{

    PUNALIGNED_FILE_PATH BootFilePath;
    PBOOT_ENVIRONMENT_DEVICE FileParent;
    PBOOT_ENVIRONMENT_DEVICE VhdFileDevice;

    //
    // Obtain the VHD parent disk device, and the VHD file's relative name.
    //
    // N.B. If the VHD device is mal-formed, then execution should never have
    //      gotten this far.  Assert that this is the case.
    //

    ASSERT(VirtualDisk->Size >= (VHD_BLOCK_IO_DEVICE_FILE_OFFSET +
                                 FILE_DEVICE_MINIMUM_SIZE));

    VhdFileDevice = Add2Ptr(VirtualDisk, VHD_BLOCK_IO_DEVICE_FILE_OFFSET);

    ASSERT((VhdFileDevice->DeviceType == DEVICE_BLOCK_IO) &&
           (VhdFileDevice->u.BlockIo.BlockIoType == BLOCK_IO_FILE));

    BootFilePath = &VhdFileDevice->u.BlockIo.u.File.SourcePath;

    ASSERT(BootFilePath->Type == FILE_PATH_TYPE_BOOT_ENV);

    ASSERT(BootFilePath->Length >=
           (FIELD_OFFSET(UNALIGNED_FILE_PATH, FilePath) +
            FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, Pad)));

    FileParent = (PBOOT_ENVIRONMENT_DEVICE)BootFilePath->FilePath;
    if (ARGUMENT_PRESENT(VhdFilePath) != FALSE) {

        ASSERT(BootFilePath->Length >=
               (FIELD_OFFSET(UNALIGNED_FILE_PATH, FilePath) +
                FileParent->Size +
                sizeof(UNICODE_NULL)));

        *VhdFilePath = Add2Ptr(FileParent, FileParent->Size);
    }

    *VhdFileParent = FileParent;
    return;
}

BOOLEAN
OslpBootVhdResidesOnSystemDisk (
    __in PBOOT_ENVIRONMENT_DEVICE VirtualDisk
    )

/*++

Routine Description:

    This routine determines whether a VHD resides on the system (boot) disk.

Arguments:

    VirtualDisk - Supplies the block IO virtual hard disk device.

Return Value:

    TRUE if the VHD resides on the boot disk, otherwise FALSE.

--*/

{

    PBOOT_ENVIRONMENT_DEVICE BootDevice;
    PHARD_DISK_IDENTIFIER BootDisk;
    PBLOCK_IO_IDENTIFIER ParentIdentifier;
    BOOLEAN ReturnValue;
    PBOOT_ENVIRONMENT_DEVICE VhdFileParent;
    PHARD_DISK_IDENTIFIER VhdParentDisk;

    //
    // If the boot device is not a hard disk partition, then the boot VHD
    // cannot reside on it as boot from VHD is only supported on hard disks
    // devices.
    //

    BootDevice = BlGetBootDevice();
    ReturnValue = FALSE;
    if (BootDevice->DeviceType == DEVICE_PARTITION_EX) {
        ParentIdentifier = &BootDevice->u.PartitionEx.ParentIdentifier;

    } else if (BootDevice->DeviceType == DEVICE_PARTITION) {
        ParentIdentifier = &BootDevice->u.Partition.ParentIdentifier;

    } else {

        ASSERT(FALSE);

        goto BootVhdResidesOnSystemDiskEnd;
    }

    ASSERT(ParentIdentifier->BlockIoType == BLOCK_IO_HARD_DISK);

    BootDisk = &ParentIdentifier->u.HardDisk;

    //
    // Obtain the VHD parent disk device.
    //

    OslpGetVhdParentDeviceAndFilePath(VirtualDisk, &VhdFileParent, NULL);
    if (VhdFileParent->DeviceType == DEVICE_PARTITION_EX) {
        ParentIdentifier = &VhdFileParent->u.PartitionEx.ParentIdentifier;

    } else if (VhdFileParent->DeviceType == DEVICE_PARTITION) {
        ParentIdentifier = &VhdFileParent->u.Partition.ParentIdentifier;

    } else {

        ASSERT(FALSE);

        goto BootVhdResidesOnSystemDiskEnd;
    }

    ASSERT(ParentIdentifier->BlockIoType == BLOCK_IO_HARD_DISK);

    VhdParentDisk = &ParentIdentifier->u.HardDisk;

    //
    // Compare the two disks and return the result.
    //

    if (CompareDisks(BootDisk, VhdParentDisk) != FALSE) {
        ReturnValue = TRUE;
    }

BootVhdResidesOnSystemDiskEnd:
    return ReturnValue;
}

ULONGLONG
OslpBootVhdFileSize (
    __in PBOOT_ENVIRONMENT_DEVICE VirtualDisk
)

/*++

Routine Description:

    This routine determines the size off the file representing a VHD boot disk
    device.

Arguments:

    VirtualDisk - Supplies the block IO virtual hard disk device.

Return Value:

    Returns the size of the VHD file.

--*/

{

    DEVICE_ID DeviceId;
    FILE_ID FileId;
    FILE_INFORMATION FileInformation;
    ULONGLONG ReturnValue;
    NTSTATUS Status;
    PBOOT_ENVIRONMENT_DEVICE VhdFileParent;
    PWCHAR VhdFilePath;

    ReturnValue = 0;

    //
    // Obtain the VHD parent disk device, and the VHD file's relative name.
    //

    OslpGetVhdParentDeviceAndFilePath(VirtualDisk,
                                      &VhdFileParent,
                                      &VhdFilePath);

    //
    // Open the VHD file and obtain its information.
    //

    Status = BlDeviceOpen(VhdFileParent, OPEN_READ, &DeviceId);
    if (NT_SUCCESS(Status)) {
        Status = BlFileOpen(DeviceId, VhdFilePath, OPEN_READ, &FileId);
        if (NT_SUCCESS(Status)) {
            Status = BlFileGetInformation(FileId, &FileInformation);
            if (NT_SUCCESS(Status)) {
                ReturnValue = FileInformation.FileSize;
            }

            BlFileClose(FileId);
        }

        BlDeviceClose(DeviceId);
    }

    return ReturnValue;
}

NTSTATUS
OslpSqmVhdBootData (
    __in PBOOT_ENVIRONMENT_DEVICE VirtualDisk
    )

/*++

Routine Description:

    This routine persists VHD boot data for SQM reporting.

Arguments:

    VirtualDisk - Supplies the block IO virtual hard disk device.

Return Value:

    NT status code.

--*/

{

    GUID ApplicationId;
    DEVICE_ID DeviceId;
    ULONG DiskType;
    DEVICE_INFORMATION Information;
    ULONG PhysicalSize;
    BOOLEAN ResidesOnSystemDisk;
    NTSTATUS Status;
    ULONG VirtualSize;
    PDEVICE_VHD_INFORMATION VhdInformation;

    DiskType = 0;
    PhysicalSize = 0;
    VirtualSize = 0;

    //
    // Query the virtual disk device for its information.
    //

    Status = BlDeviceOpen(VirtualDisk, OPEN_READ, &DeviceId);
    if (!NT_SUCCESS(Status)) {
        goto SqmVhdBootDataEmitEvent;
    }

    Status = BlDeviceGetInformation(DeviceId, &Information);
    BlDeviceClose(DeviceId);
    if (!NT_SUCCESS(Status)) {
        goto SqmVhdBootDataEmitEvent;
    }

    if ((Information.DeviceType != DEVICE_BLOCK_IO) ||
        (Information.u.BlockIo.Type != BLOCK_IO_VIRTUAL_HARD_DISK)) {

        Status = STATUS_INVALID_PARAMETER;
        goto SqmVhdBootDataEmitEvent;
    }

    //
    // Set the VHD information.
    //
    // N.B. Round sizes to nearest GB.
    //

    VhdInformation = &Information.u.BlockIo.DeviceInformation.VirtualDisk;

    DiskType = VhdInformation->Type;

    VirtualSize = (ULONG)((VhdInformation->CurrentSize + _512MB) / _1GB);


SqmVhdBootDataEmitEvent:

    PhysicalSize = (ULONG)((OslpBootVhdFileSize(VirtualDisk) + _512MB) / _1GB);

    ResidesOnSystemDisk = OslpBootVhdResidesOnSystemDisk(VirtualDisk);

    RtlCopyMemory(&ApplicationId, BlGetApplicationIdentifier(), sizeof(GUID));

    //
    // Emit the VHD boot telemetry event even if the VHD could not be queried.
    //

    TraceLoggingWrite(OslBootProvider,
                      "NativeVhdBoot",
                      TraceLoggingLevel(TRACE_LEVEL_INFORMATION),
                      TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES),
                      TraceLoggingBootPartB(OslApplicationIdentifier, OslBootId),
                      TraceLoggingUInt32(DiskType, "DiskType"),
                      TraceLoggingBool(ResidesOnSystemDisk, "ResidesOnSystemDisk"),
                      TraceLoggingUInt32(PhysicalSize, "PhysicalSizeGB"),
                      TraceLoggingUInt32(VirtualSize, "VirtualSizeGB")
                      );

    return Status;
}

NTSTATUS
OslEnumerateDisks (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    Enumerates all firmware enumerable disk devices.  This routine
    will build the arc disk information data structure in the loader block.

Arguments:

    LoaderBlock - Pointer to the loader parameter block.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if a memory allocation fails.
    STATUS_IO_DEVICE_ERROR or
    STATUS_UNEXPECTED_IO_ERROR if there is an error when reading the MBR to
        calculate a checksum.

--*/

{

    PBOOT_ENVIRONMENT_DEVICE BlockIoDevice;
    PBLOCK_IO_IDENTIFIER BlockIoIdentifier;
    ULONG BlockIoOffset;
    PBOOT_APPLICATION_ENTRY BootEntry;
    PBOOT_ENVIRONMENT_DEVICE Device;
    BOOT_ENVIRONMENT_DEVICE DeviceClassIdentifier;
    DEVICE_ID DeviceId;
    ULONG DeviceSize;
    GENERIC_BUFFER DiskHandleArray;
    PARC_DISK_INFORMATION DiskInformation;
    PARC_DISK_SIGNATURE DiskSignature;
    ULONG HandleCount;
    ULONG Index;
    PBOOT_ENVIRONMENT_DEVICE OsDevice;
    PLIST_ENTRY NextEntry;
    NTSTATUS Status;
    PVHD_DISK_SIGNATURE VhdSignature;
    BOOLEAN WinPeOptionValue;

    BlockIoDevice = NULL;
    BootEntry = NULL;
    BlInitGenericBuffer(&DiskHandleArray, NULL, 0);
    OsDevice = OslLoadDevice;
    VhdSignature = NULL;
    WinPeOptionValue = FALSE;

    //
    // Allocate memory for the disk information structure (list entry).
    //

    DiskInformation = BlMmAllocateHeap(sizeof(ARC_DISK_INFORMATION));
    if (DiskInformation == NULL) {
        Status = STATUS_NO_MEMORY;
        goto EnumerateDisksEnd;
    }

    InitializeListHead(&DiskInformation->DiskSignatures);

    //
    // Enumerate all hard disk drives, getting an array of device handles
    // for each.
    //

    RtlZeroMemory(&DeviceClassIdentifier, sizeof(BOOT_ENVIRONMENT_DEVICE));
    DeviceClassIdentifier.Size = sizeof(BOOT_ENVIRONMENT_DEVICE);
    DeviceClassIdentifier.DeviceType = DEVICE_BLOCK_IO;
    DeviceClassIdentifier.u.BlockIo.BlockIoType = BLOCK_IO_HARD_DISK;
    Status = BlDeviceEnumerateDeviceClass(&DeviceClassIdentifier,
                                          &DiskHandleArray,
                                          TRUE);

    if (!NT_SUCCESS(Status)) {
        goto EnumerateDisksEnd;
    }

    //
    // Iterate through the list of device handles.  For each one, build an
    // arc disk signature structure and insert it into the linked list.
    //

    HandleCount = DiskHandleArray.DataSize / sizeof(DEVICE_ID);
    BootEntry = BlGetApplicationEntry();
    ASSERT(BootEntry != NULL);
    Status = BlGetBootOptionBoolean(BootEntry->Options,
                                    BCDE_OSLOADER_TYPE_WINPE,
                                    &WinPeOptionValue);

    if (!NT_SUCCESS(Status) && (Status != STATUS_NOT_FOUND)) {
        goto EnumerateDisksEnd;
    }

    for (Index = 0; Index < HandleCount; Index += 1) {
        DeviceId = ((PDEVICE_ID)DiskHandleArray.Buffer)[Index];
        Status = BlGetDeviceIdentifier(DeviceId, &Device);
        if (!NT_SUCCESS(Status)) {
            goto EnumerateDisksEnd;
        }


        //
        // Create a disk signature structure representing this device.
        //

        Status = OslpCreateArcDiskSignature(DeviceId, &DiskSignature);
        if (!NT_SUCCESS(Status)) {
            continue;
        }

        //
        // If this is a WinPE boot, the user may be attempting to install
        // the OS.  If RAW disks are present in the system, these disks
        // will need to be stamped with a unique signature so the OS
        // can distinguish them accordingly.
        //

        if ((WinPeOptionValue != FALSE) &&
            (DiskSignature->ValidPartitionTable == FALSE)) {

            OslpStampSignature(Device,
                               DiskSignature,
                               &DiskInformation->DiskSignatures);

        }

        //
        // Insert the disk signature into the linked list.
        //

        InsertTailList(&DiskInformation->DiskSignatures,
                       &DiskSignature->ListEntry);
    }

    //
    // Add the OS device if it is a block I/O device but is not a hard
    // disk, a partition on a hard disk or a ramdisk.  In both the hard disk
    // and partition case the OS device has been added by the code above,
    // and the OS does not expect a signature for a ramdisk.
    //
    // These block I/O devices need to be present if they are the OS boot
    // device, but are not added unconditionally because they have not been
    // historically.  Additionally, they may refer to devices without any
    // media in them (like an empty CD-ROM drive) which could confuse OS
    // code attempting to read from them.
    //
    // If the OS device is a file on a block device, add the information for
    // the block device itself, not the file.
    //

    if (OsDevice->DeviceType == DEVICE_BLOCK_IO) {
        BlockIoIdentifier = &OsDevice->u.BlockIo;
        if (BlockIoIdentifier->BlockIoType == BLOCK_IO_FILE) {
            OsDevice = BlGetDevice(&BlockIoIdentifier->u.File.SourcePath);
            if (OsDevice == NULL) {
                Status = STATUS_NO_SUCH_DEVICE;
                goto EnumerateDisksEnd;
            }
        }
    }

    BlockIoOffset = 0;
    BlockIoIdentifier = NULL;
    if (OsDevice->DeviceType == DEVICE_PARTITION) {
        BlockIoIdentifier = &OsDevice->u.Partition.ParentIdentifier;
        BlockIoOffset = FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE,
                                     u.Partition.ParentIdentifier);

    } else if (OsDevice->DeviceType == DEVICE_PARTITION_EX) {
        BlockIoIdentifier = &OsDevice->u.PartitionEx.ParentIdentifier;
        BlockIoOffset = FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE,
                                     u.PartitionEx.ParentIdentifier);

    } else if (OsDevice->DeviceType == DEVICE_BLOCK_IO) {
        BlockIoIdentifier = &OsDevice->u.BlockIo;
        BlockIoOffset = FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u.BlockIo);
    }

    if ((BlockIoIdentifier != NULL) &&
        (BlockIoIdentifier->BlockIoType != BLOCK_IO_HARD_DISK) &&
        (BlockIoIdentifier->BlockIoType != BLOCK_IO_RAMDISK)) {

        //
        // If the OS device is a virtual partition create a block IO virtual
        // hard disk device suitable for ARC disk signature creation and
        // create the VHD disk signature that wil be used by the OS to surface
        // the disk during init.
        //

        if (BlockIoIdentifier->BlockIoType == BLOCK_IO_VIRTUAL_HARD_DISK) {
            DeviceSize = OslLoadDevice->Size - BlockIoOffset +
                FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u.BlockIo);

            BlockIoDevice = BlMmAllocateHeap(DeviceSize);
            if (BlockIoDevice == NULL) {
                Status = STATUS_NO_MEMORY;
                goto EnumerateDisksEnd;
            }

            //
            // Initialize the block IO virtual disk device.
            //

            RtlZeroMemory(BlockIoDevice,
                          FIELD_OFFSET(BOOT_ENVIRONMENT_DEVICE, u.BlockIo));

            RtlCopyMemory(&BlockIoDevice->u.BlockIo,
                          Add2Ptr(OslLoadDevice, BlockIoOffset),
                          OslLoadDevice->Size - BlockIoOffset);

            BlockIoDevice->DeviceType = DEVICE_BLOCK_IO;
            BlockIoDevice->Size = DeviceSize;
            OsDevice = BlockIoDevice;

            //
            // Create the VHD disk signature.
            //

            Status = OslpCreateVhdSignature(OslLoadDevice, &VhdSignature);
            if (!NT_SUCCESS(Status)) {
                goto EnumerateDisksEnd;
            }
        }

        Status = BlDeviceOpen(OsDevice, OPEN_READ, &DeviceId);
        if (!NT_SUCCESS(Status)) {
            goto EnumerateDisksEnd;
        }

        Status = OslpCreateArcDiskSignature(DeviceId, &DiskSignature);
        BlDeviceClose(DeviceId);
        if (!NT_SUCCESS(Status)) {
            goto EnumerateDisksEnd;
        }

        //
        // Attach the VHD boot partition device to its associated ARC disk
        // signature so the IO manager can surface it during init.
        //

        if (VhdSignature != NULL) {

            ASSERT(DiskSignature->VhdSignature == NULL);

            DiskSignature->VhdSignature = VhdSignature; // Allocated above.

            //
            // Now that boot from VHD has been established, use the block IO
            // virtual hard disk device created above to SQM any interesting
            // data.
            //

            Status = OslpSqmVhdBootData(OsDevice);

            ASSERT(NT_SUCCESS(Status));
        }

        InsertTailList(&DiskInformation->DiskSignatures,
                       &DiskSignature->ListEntry);
    }

    //
    // Pass the arc disk information up to the kernel via the loader block.
    //

    Status = STATUS_SUCCESS;
    LoaderBlock->ArcDiskInformation = DiskInformation;

EnumerateDisksEnd:
    if (!NT_SUCCESS(Status)) {
        if (VhdSignature != NULL) {
            BlMmFreeHeap(VhdSignature);
        }

        //
        // Free each disk signature structure.
        //

        if (DiskInformation != NULL) {
            NextEntry = DiskInformation->DiskSignatures.Flink;
            while (NextEntry != &DiskInformation->DiskSignatures) {
                DiskSignature = CONTAINING_RECORD(NextEntry,
                                                  ARC_DISK_SIGNATURE,
                                                  ListEntry);

                NextEntry = NextEntry->Flink; // Advance pointer.
                RemoveEntryList(&DiskSignature->ListEntry);
                BlMmFreeHeap(DiskSignature);
            }

            BlMmFreeHeap(DiskInformation);      // Delete list entry.
        }

        if (DiskHandleArray.BufferSize != 0) {
            BlMmFreeHeap(DiskHandleArray.Buffer);
        }
    }

    if (BlockIoDevice != NULL) {
        BlMmFreeHeap(BlockIoDevice);
    }

    return Status;
}

NTSTATUS
OslProcessApplicationPersistentData (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine will make available to the Operating System any memory
    described by the library as persistent data.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if a memory allocation fails.

--*/

{

    LIST_ENTRY AllocationList;
    PMEMORY_DESCRIPTOR MemoryDescriptor;
    MEMORY_MAP_HANDLE MemoryMapHandle;
    PLIST_ENTRY NextEntry;
    PLIST_ENTRY PdListHead;
    PBAPD_LIST_ITEM PdListItem;
    NTSTATUS ReturnStatus;
    NTSTATUS Status;

    //
    // Get a list of all persistent data allocations.
    //

    Status = BlMmEnumerateAllocations(&AllocationList,
                                      MEMORY_TYPE_PERSISTENT_DATA,
                                      &MemoryMapHandle);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Walk through the allocations looking for persistent data items.  For
    // each memory region marked persistent, create an OS accessible virtual
    // mapping to the allocation.
    //

    ReturnStatus = STATUS_SUCCESS;
    PdListHead = &LoaderBlock->Extension->BootApplicationPersistentData;
    for (NextEntry = AllocationList.Flink;
         NextEntry != &AllocationList;
         NextEntry = NextEntry->Flink) {

        MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                             MEMORY_DESCRIPTOR,
                                             ListEntry);

        //
        // Persist allocations into the operating system.  If an error occurs,
        // continue to process the list, returning the last error.
        //

        NT_ASSERT(MemoryDescriptor->MemoryType == MEMORY_TYPE_PERSISTENT_DATA);

        Status = OslpPersistData(MemoryDescriptor, &PdListItem);
        if (NT_SUCCESS(Status)) {
            InsertTailList(PdListHead, &PdListItem->Entry);

        } else if (Status != STATUS_NOT_FOUND) {

            //
            // N.B. STATUS_NOT_FOUND specifies the data was previously marked as
            //      destroyed and it's invalid, hence, it's simply ignored,
            //      function is terminated on other errors as they might be
            //      fatal.
            //

            ReturnStatus = Status;
            goto OslProcessApplicationPersistentDataEnd;
        }
    }

OslProcessApplicationPersistentDataEnd:
    BlMmReleaseMemoryMap(&MemoryMapHandle);
    return ReturnStatus;
}

NTSTATUS
OslBuildKernelMemoryMap (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in ULONG ReserveDescriptorsCount,
    __out PLIST_ENTRY FreeDescriptorsList,
    __out PULONG FreeDescriptorsListSize,
    __out PVOID* AllocatedDescriptorsBuffer,
    __out PLIST_ENTRY MemoryMap,
    __out GENERIC_BUFFER* MemoryMapDescriptors
    )

/*++

Routine Description:

    This routine will get the loader's current memory map and translate it into
    the memory map that will be passed to the kernel.

    N.B. No MM Allocation services can be used after this routine has called.
         Doing so would result in a change to the memory map, invalidating
         the memory map created for the kernel.  This restriction exists for
         heap allocation services as well, since these could result in the
         heap growing, which will leverage the page allocator.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

    ReserveDescriptorsCount - Number of memory allocation descriptors
        to reserve in order to allow memory map entries to be added
        without reallocating entire buffer.

    FreeDescriptorsList - Supplies a pointer to a variable that receives a
        linked list of memory descriptors that were allocated but not used.

    FreeDescriptorsListSize - Supplies a pointer to a variable that receives a
        length of the free descriptors list.

    AllocatedDescriptorsBuffer - Supplies a pointer to the free descriptor's
        buffer.

    MemoryMap - Supplies a list of memory descriptors used to build the loader
        block map.

    MemoryMapDescriptors - Supplies a description of the buffer for memory
        descriptors used to build the loader map.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if a memory allocation fails.

    Appropriate NTSTATUS code otherwise.

--*/

{

    ULONG AllocatedReservedDescriptors;
    ULONG Attempts;
    ULONG BufferSize;
    PMEMORY_ALLOCATION_DESCRIPTOR DescriptorBuffer;
    ULONG MemoryMapCount;
    ULONG NumberOfPages;
    NTSTATUS Status;

    //
    // Estimate the number of pages required to hold the kernel's memory map
    // descriptors. This value depends on the size of the current memory map,
    // which can be obtained using BlMmGetMemoryMap.
    //

    MemoryMapDescriptors->BufferSize = 0;
    Status = BlMmGetMemoryMap(MemoryMap,
                              MemoryMapDescriptors,
                              MEMORYMAP_TYPE_ALL,
                              0);

    if (Status != STATUS_BUFFER_TOO_SMALL) {
        return Status;
    }

    ASSERT(MemoryMapDescriptors->BufferSize > 0);
    ASSERT((MemoryMapDescriptors->BufferSize % sizeof(MEMORY_DESCRIPTOR)) == 0);

    //
    // The buffer size is in terms of loader's memory descriptor. Compute the
    // corresponding size in terms of kernel's memory descriptor. Also add some
    // slack space (7 descriptors) to acommodate growth in the memory map.
    //
    // N.B. The 7 descriptor slack assumes 1 descriptor for the kernel
    //      memory map buffer and up to 3 descriptors for page table pages,
    //      and up to 3 for splitting the memory descriptor for kernel and HAL
    //      and  the unused portions before and after kernel/HAL in the large
    //      page(s). (Kernal and HAL are loaded in one or more consecutive
    //      large pages described by a single memory descriptor in the loader,
    //      but memory manager in kernel expects that each of them is described
    //      individually by its own memory descriptor.)
    //
    //      This value may be insufficient if additional pages (and descriptors)
    //      are required for page table reference pages (if implemented
    //      on the platform). This will cause failure to create the kernel
    //      memory map. This scenario is handled by increasing the buffer
    //      size and trying for the kernel memory map again.
    //
    // N.B. Since kernel memory map will be rebuilt if previous attempt fails,
    //      it is not necessary to account for the slack space. However, doing
    //      so improves the chances of success in the first attempt itself.
    //

    BufferSize = MemoryMapDescriptors->BufferSize;
    MemoryMapCount = BufferSize / sizeof(MEMORY_DESCRIPTOR);
    MemoryMapCount += ReserveDescriptorsCount;
    MemoryMapCount += 7;
    BufferSize = MemoryMapCount * sizeof(MEMORY_ALLOCATION_DESCRIPTOR);
    BufferSize = ALIGN_RANGE_UP(BufferSize, PAGE_SIZE);
    NumberOfPages =  BufferSize >> PAGE_SHIFT;

    //
    // Attempt to build the kernel memory map. In each attempt:
    // 1) Allocate a buffer for kernel's memory map descriptors;
    // 2) Obtain the loader's current memory map;
    // 3) Convert loader's memory descriptors to kernel's memory descriptors;
    // 4) Cleanup on failure in step #3.
    //
    // The kernel's memory map buffer needs to be allocated before the
    // call to BlMmGetMemoryMap (in step #2) to prevent the memory map from
    // changing due to memory allocations afterwards.
    //

    MemoryMapDescriptors->Buffer = NULL;
    DescriptorBuffer = NULL;
    AllocatedReservedDescriptors = 0;
    for (Attempts = 0; Attempts < KERNEL_MAP_ATTEMPTS_TOTAL; Attempts += 1) {

        //
        // N.B. The kernel's memory map buffer must be marked as a persistent
        //      allocation since the OS will use the contained memory
        //      descriptor list through phase 1 initialization.
        //

        Status = BlMmAllocatePages(&DescriptorBuffer,
                                   NumberOfPages,
                                   OSL_MEMORY_TYPE_KERNEL_INIT_DATA,
                                   MEMORY_ATTRIBUTE_ALLOCATION_KSEG,
                                   0);

        if (!NT_SUCCESS(Status)) {
            goto BuildKernelMemoryMapEnd;
        }

        MemoryMapDescriptors->BufferSize = 0;
        Status = BlMmGetMemoryMap(MemoryMap,
                                  MemoryMapDescriptors,
                                  MEMORYMAP_TYPE_ALL,
                                  MDL_FLAGS_ALLOCATE_DESCRIPTORS);

        if (!NT_SUCCESS(Status)) {
            goto BuildKernelMemoryMapEnd;
        }

        ASSERT((MemoryMapDescriptors->BufferSize % sizeof(MEMORY_DESCRIPTOR)) == 0);

        Status = OslpBuildKernelMemoryMap(LoaderBlock,
                                          MemoryMap,
                                          NumberOfPages << PAGE_SHIFT,
                                          DescriptorBuffer,
                                          FreeDescriptorsList,
                                          &AllocatedReservedDescriptors);

        //
        // In case if allocation succeeded but number of free descriptors
        // was less than requested, retry the allocation with increased
        // buffer size.
        //

        if (NT_SUCCESS(Status) &&
            (AllocatedReservedDescriptors < ReserveDescriptorsCount)) {

            Status = STATUS_BUFFER_TOO_SMALL;
        }

        if (Status != STATUS_BUFFER_TOO_SMALL) {
            break;
        }

        //
        // If the kernel memory map could not be built due to insufficient
        // buffer size, release previously allocated buffers and reinitialize
        // the loader block list in preparation for another attempt.
        //
        // N.B. This can be caused due to additional descriptors that get
        //      created due to internal memory allocations (for page tables
        //      and reference pages) while allocating buffer for the kernel
        //      memory map above.
        //

        BlMmFreePages(DescriptorBuffer);
        DescriptorBuffer = NULL;
        BlMmFreeHeap(MemoryMapDescriptors->Buffer);
        MemoryMapDescriptors->Buffer = NULL;
        InitializeListHead(&LoaderBlock->MemoryDescriptorListHead);

        //
        // Double the number of pages for every attempt.
        //

        NumberOfPages = NumberOfPages * 2;
    }

    if (NT_SUCCESS(Status)) {
        *AllocatedDescriptorsBuffer = DescriptorBuffer;
        *FreeDescriptorsListSize = AllocatedReservedDescriptors;
    }

BuildKernelMemoryMapEnd:
    if (!NT_SUCCESS(Status)) {
        if (DescriptorBuffer != NULL) {
            BlMmFreePages(DescriptorBuffer);
        }

        if (MemoryMapDescriptors->Buffer != NULL) {
            BlMmFreeHeap(MemoryMapDescriptors->Buffer);
            MemoryMapDescriptors->Buffer = NULL;
        }
    }

    return Status;
}

NTSTATUS
OslpInitializeDevicePathInformation (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine will initialize all device path and file path values in
    the loader block.

Arguments:

    LoaderBlock - A pointer to the loader parameter block.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if a memory allocation fails.
    STATUS_INVALID_PARAMETER if the file path translation fails.

--*/

{

    PFILE_PATH FilePath;
    ULONG FilePathSize;
    PCHAR LoaderFilePath;
    UINTN LoaderFilePathSize;
    PCHAR Ptr;
    ANSI_STRING OsFileArcPath;
    PCHAR OsFilePath;
    UINTN OsFilePathSize;
    NTSTATUS Status;
    ULONG StringSize;
    ANSI_STRING SystemPartitionArcPath;
    DEVICE_ID SystemPartitionDeviceId;
    PBOOT_ENVIRONMENT_DEVICE BootDevice;
    PBOOT_ENVIRONMENT_DEVICE VhdDevice;
    PBOOT_ENVIRONMENT_DEVICE OsDevice = OslLoadDevice;

    OsFilePath = NULL;
    LoaderFilePath = NULL;
    RtlInitAnsiString(&SystemPartitionArcPath, NULL);
    RtlInitAnsiString(&OsFileArcPath, NULL);

    //
    // Translate the boot device into an arc device.  In the case of a PXE boot,
    // use the OS device as the boot device, since the network is not a legal
    // boot device from the OS perspective.
    //

    BootDevice = BlGetBootDevice();
    if (BootDevice->DeviceType == DEVICE_UDP) {
        BootDevice = OslLoadDevice;

        //
        // If boot device is a VHD, fall back to the underlying device hosting
        // the VHD.
        //

        if (BlDeviceIsVirtualPartitionDevice(BootDevice, &VhdDevice) != FALSE) {
            NT_VERIFY(
                NT_SUCCESS(
                    BlDeviceGetVhdFileDevice(VhdDevice, &BootDevice)));
        }
    }

    if (BootDevice->DeviceType == DEVICE_VMBUS) {

        //
        // If the boot device is VMBUS, then the real arcdevice (which will
        // not be VMBUS) should have been defined in the BCD Store.
        //

        Status = BlGetApplicationOptionDevice(BCDE_OSLOADER_TYPE_OS_ARC_DEVICE,
                                              &BootDevice,
                                              NULL);

        ASSERT(NT_SUCCESS(Status) && (BootDevice->DeviceType != DEVICE_VMBUS));

        if (!NT_SUCCESS(Status)) {
            return Status;
        }

        OsDevice = BootDevice;
    }

    Status = BlDeviceOpen(BootDevice,
                          OPEN_READ,
                          &SystemPartitionDeviceId);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    Status = AhGetArcDevice(SystemPartitionDeviceId,
                            &SystemPartitionArcPath,
                            TRUE);

    if (!NT_SUCCESS(Status)) {
        goto OslpInitializeDevicePathInformationEnd;
    }

    //
    // Allocate a buffer for the file path to the loader.  The new model
    // (boot manager and loader) does not fit well into this old convention.
    // We should really describe the full loader path (device and file) to the
    // Operating System.  This was what the old loader did.  But the device
    // was only ever used to identify the system partition.  Therefore, we
    // must continue to describe the boot device (system partition).  Actually
    // describing the loader's file path would be confusing, so continue to
    // pass '\' to the kernel (like what was done for the old NTLDR).
    //

    LoaderFilePathSize = sizeof("\\");
    LoaderFilePath = BlMmAllocateHeap(LoaderFilePathSize);
    if (LoaderFilePath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OslpInitializeDevicePathInformationEnd;
    }

    strcpy_s(LoaderFilePath, LoaderFilePathSize, "\\");

    //
    // Get the full arc path to the Operating System.
    //

    FilePathSize = FIELD_OFFSET(FILE_PATH, FilePath);
    FilePathSize += OsDevice->Size;
    StringSize = ((ULONG)wcslen(OslSystemRoot) * sizeof(WCHAR)) +
                 sizeof(UNICODE_NULL);

    FilePathSize += StringSize;
    FilePath = BlMmAllocateHeap(FilePathSize);
    if (FilePath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OslpInitializeDevicePathInformationEnd;
    }

    FilePath->Version = 1;
    FilePath->Type = FILE_PATH_TYPE_BOOT_ENV;
    FilePath->Length = FilePathSize;
    RtlCopyMemory(FilePath->FilePath,
                  OsDevice,
                  OsDevice->Size);

    RtlCopyMemory(Add2Ptr(FilePath->FilePath, OsDevice->Size),
                  OslSystemRoot,
                  StringSize);

    Status = AhGetArcPath(FilePath, &OsFileArcPath, TRUE);
    if (!NT_SUCCESS(Status)) {
        goto OslpInitializeDevicePathInformationEnd;
    }

    //
    // Split the full arc path into a device path and a file path.  A new
    // buffer actually has to be allocated for the OS file path, since this
    // string requires the preceeding '\'.
    //

    Ptr = strchr(OsFileArcPath.Buffer, '\\');

    ASSERT(Ptr != NULL);

    OsFilePathSize = (ULONG)strlen(Ptr) + 1;
    OsFilePath = BlMmAllocateHeap(OsFilePathSize);
    if (OsFilePath == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OslpInitializeDevicePathInformationEnd;
    }

    strcpy_s(OsFilePath, OsFilePathSize, Ptr);
    *Ptr = '\0';               // Null terminate the string in the device path.

    //
    // Initialize device / file paths in the loader block.
    //
    // ArcBootDeviceName - Arc device path for the device containing the OS.
    // ArcHalDeviceName - Arc device path for the boot device.
    // NtBootPathName - File path to the OS directory.
    // NtHalPathName - File path for the OS Loader ('\').
    //

    LoaderBlock->ArcBootDeviceName = OsFileArcPath.Buffer;
    LoaderBlock->ArcHalDeviceName = SystemPartitionArcPath.Buffer;
    LoaderBlock->NtBootPathName = OsFilePath;
    LoaderBlock->NtHalPathName = LoaderFilePath;

OslpInitializeDevicePathInformationEnd:
    if (!NT_SUCCESS(Status)) {
        BlDeviceClose(SystemPartitionDeviceId);

        if (LoaderFilePath != NULL) {
            BlMmFreeHeap(LoaderFilePath);
        }

        if (OsFilePath != NULL) {
            BlMmFreeHeap(OsFilePath);
        }

        if (SystemPartitionArcPath.MaximumLength != 0) {
            BlMmFreeHeap(SystemPartitionArcPath.Buffer);
        }

        if (OsFileArcPath.MaximumLength != 0) {
            BlMmFreeHeap(OsFileArcPath.Buffer);
        }
    }

    return Status;
}

NTSTATUS
OslpCreateArcDiskSignature (
    __in DEVICE_ID DeviceId,
    __out PARC_DISK_SIGNATURE *DiskSignatureOut
    )

/*++

Routine Description:

    This routine creates an arc disk signature structure for the given
    block I/O device.  This structure describes the disk in the loader
    block.

Arguments:

    DeviceId - Supplies a handle to the boot environment device to
        create the arc disk signature for.

    DiskSignatureOut - Supplies a pointer to storage for a pointer to
        an arc disk signature structure.  This will be filled in by
        this routine with a pointer to the structure representing the
        given device.

Return Value:

    STATUS_SUCCESS if the routine completes successfully.
    STATUS_NO_MEMORY if there is a memory allocation failure.
    STATUS_BAD_DEVICE_TYPE if the given device is not a block I/O device.
    Another error status code if returned from the library.

--*/

{

    ANSI_STRING AnsiString;
    PULONG Buffer;
    ULONG BufferSize;
    ULONG BootSectorOffset;
    DEVICE_INFORMATION DeviceInformation;
    PDEVICE_DISK_INFORMATION DiskInformation;
    PARC_DISK_SIGNATURE DiskSignature;
    ULONG i;
    NTSTATUS Status;
    ULONG Sum;

    Buffer = NULL;
    DiskSignature = NULL;

    //
    // Allocate and initialize a disk signature structure for this
    // device.
    //

    DiskSignature = BlMmAllocateHeap(sizeof(ARC_DISK_SIGNATURE));
    if (DiskSignature == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OslpCreateArcDiskSignatureEnd;
    }

    RtlZeroMemory(DiskSignature, sizeof(ARC_DISK_SIGNATURE));

    //
    // Get the device information for this device.
    //

    Status = BlDeviceGetInformation(DeviceId, &DeviceInformation);
    if (!NT_SUCCESS(Status)) {
        goto OslpCreateArcDiskSignatureEnd;
    }

    ASSERT(DeviceInformation.DeviceType == DEVICE_BLOCK_IO);

    //
    // Perform per-device-type processing tasks.
    //

    switch (DeviceInformation.u.BlockIo.Type) {

        //
        // Hard disk case.  Fill in the signature information based on
        // the type of disk.
        //

    case BLOCK_IO_VIRTUAL_HARD_DISK:
    case BLOCK_IO_HARD_DISK:
        DiskInformation = &DeviceInformation.u.BlockIo.DeviceInformation.Disk;
        switch (DeviceInformation.u.BlockIo.PartitionType) {
        case GPT:
            DiskSignature->ValidPartitionTable = TRUE;
            DiskSignature->IsGpt = TRUE;
            RtlCopyMemory(&DiskSignature->GptSignature[0],
                          &DiskInformation->Gpt.Signature,
                          sizeof(DiskSignature->GptSignature));

            break;

        case MBR:
            DiskSignature->ValidPartitionTable = TRUE;
            DiskSignature->Signature = DiskInformation->Mbr.Signature;
            break;
        }

        BootSectorOffset = 0;
        break;

        //
        // CD-ROM case.
        //

    case BLOCK_IO_CDROM:

        //
        // The first sector of a CDROM for signature and checksumming
        // purposes is actually sector 16, which contains the volume
        // descriptors and other information at the head of the ISO filesystem.
        //

        BootSectorOffset = DeviceInformation.u.BlockIo.BlockSize * 16;
        break;

    default:
        Status = STATUS_BAD_DEVICE_TYPE;
        goto OslpCreateArcDiskSignatureEnd;
    }

    //
    // Record whether the device supports Int13 extensions.
    //

    DiskSignature->xInt13 =
        DeviceInformation.u.BlockIo.DeviceInformation.Disk.SupportsExtendedInt13;

    //
    // Read the boot sector into memory and calculate its checksum.
    //

    BufferSize = DeviceInformation.u.BlockIo.BlockSize;
    Buffer = BlMmAllocateHeap(BufferSize);
    if (Buffer == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OslpCreateArcDiskSignatureEnd;
    }

    Status = BlDeviceReadAtOffset(DeviceId,
                                  BootSectorOffset,
                                  BufferSize,
                                  Buffer,
                                  NULL);

    if (!NT_SUCCESS(Status)) {
        goto OslpCreateArcDiskSignatureEnd;
    }

    //
    // Calculate a checksum of the first sector.
    //

    BufferSize /= 4; // Add up Sector 4 bytes at a time.
    for (i = 0, Sum = 0; i < BufferSize; i += 1) {
        Sum += Buffer[i];
    }

    DiskSignature->CheckSum = ~Sum + 1;

    //
    // Get the arc device name for the disk.
    //

    RtlInitAnsiString(&AnsiString, NULL);
    Status = AhGetArcDevice(DeviceId, &AnsiString, TRUE);
    if (!NT_SUCCESS(Status)) {
        BlMmFreeHeap(DiskSignature);
        goto OslpCreateArcDiskSignatureEnd;
    }

    DiskSignature->ArcName = AnsiString.Buffer;

    //
    // Set the populated structure into the caller-provided pointer.
    //

    *DiskSignatureOut = DiskSignature;
    Status = STATUS_SUCCESS;

OslpCreateArcDiskSignatureEnd:

    if (Buffer != NULL) {
        BlMmFreeHeap(Buffer);
    }

    if (!NT_SUCCESS(Status)) {
        if (DiskSignature != NULL) {
            BlMmFreeHeap(DiskSignature);
        }
    }

    return Status;
}

NTSTATUS
OslpBuildKernelMemoryMap (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in PLIST_ENTRY LoaderMemoryMap,
    __in ULONG KernelBufferSize,
    __out_bcount(KernelBufferSize) PMEMORY_ALLOCATION_DESCRIPTOR KernelMapBuffer,
    __out PLIST_ENTRY FreeDescriptorsList,
    __out PULONG ReservedDescriptorsCount
    )

/*++

Routine Description:

    This routine walks through the supplied loader memory descriptor list and
    converts each loader descriptor into corresponding kernel memory descriptor.
    The supplied memory buffer is used for the kernel memory map.

    This routine will update :

        LOADER_PARAMETER_BLOCK.LoaderMemoryList

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

    LoaderMemoryMap - Supplies a pointer to the loader's memory descriptor list.

    KernelBufferSize - Supplies the size (in bytes) for the kernel memory map
        buffer.

    KernelMemoryMap - Supplies a pointer to a memory buffer to use for the
        kernel memory map.

    FreeDescriptorsList - Supplies a pointer to a variable that receives a
        linked list of descriptor slots that are still available in buffer.

    ReservedDescriptorsCount - Receives a number of entries in a
        FreeDescriptorsList.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_BUFFER_TOO_SMALL if the kernel buffer is insufficient to hold all
        the memory descriptors.

--*/

{

    PMEMORY_ALLOCATION_DESCRIPTOR CurrentDescriptor;
    PKLDR_DATA_TABLE_ENTRY HalEntry;
    ULONGLONG HypervisorLowPageAddress;
    PKLDR_DATA_TABLE_ENTRY KernelEntry;
    PMEMORY_DESCRIPTOR MemoryDescriptor;
    PLIST_ENTRY NextEntry;
    PMEMORY_ALLOCATION_DESCRIPTOR PreviousDescriptor;
    ULONG SizeRemaining;
    ULONGLONG Size;
    ULONG SplitCount;
    NTSTATUS Status;
    PVOID VirtualAddress;

    ASSERT((LoaderMemoryMap != NULL) && (KernelMapBuffer != NULL));

    //
    // Find the kernel and HAL's data table entries. Kernel data table entry
    // is always the first one on the LoadOrderListHead list of loader block.
    // HAL data table entry is always the second one on the list.
    //

    KernelEntry = CONTAINING_RECORD(LoaderBlock->LoadOrderListHead.Flink,
                                    KLDR_DATA_TABLE_ENTRY,
                                    InLoadOrderLinks);

    HalEntry = CONTAINING_RECORD(LoaderBlock->LoadOrderListHead.Flink->Flink,
                                 KLDR_DATA_TABLE_ENTRY,
                                 InLoadOrderLinks);

    //
    // Walk through the loader's memory descriptor list and convert each
    // descriptor into corresponding kernel descriptor.
    //

    CurrentDescriptor = KernelMapBuffer;
    NextEntry = LoaderMemoryMap->Flink;
    SizeRemaining = KernelBufferSize;
    while (NextEntry != LoaderMemoryMap) {
        if (SizeRemaining < sizeof(MEMORY_ALLOCATION_DESCRIPTOR)) {
            Status = STATUS_BUFFER_TOO_SMALL;
            goto BuildKernelMemoryMapEnd;
        }

        MemoryDescriptor = CONTAINING_RECORD(NextEntry,
                                             MEMORY_DESCRIPTOR,
                                             ListEntry);

        //
        // Initialize a kernel memory descriptor based on the descriptor
        // returned from the library.
        //

        OslpConvertMemoryDescriptor(CurrentDescriptor, MemoryDescriptor);

        //
        // The kernel descriptor may have been truncated to zero pages. If that
        // happens then just go to the next memory descriptor.
        //

        if (CurrentDescriptor->PageCount == 0) {
            NextEntry = NextEntry->Flink;
            continue;
        }

        //
        // The kernel expects that every page marked as free was never touched
        // by the loader.  This isn't necessarily the case since the page
        // allocator may have mapped a free region expecting its use.  In this
        // case, the loader must unmap the region so as not to confuse MM.
        //

        if ((CurrentDescriptor->MemoryType == LoaderFree) &&
            (MemoryDescriptor->MappedBasePage != 0)) {

            Size = MemoryDescriptor->PageCount << PAGE_SHIFT;
            VirtualAddress = (PVOID)(ULONG_PTR)
                  (MemoryDescriptor->MappedBasePage << PAGE_SHIFT);

            NT_VERIFY(NT_SUCCESS(BlMmUnmapVirtualAddress(VirtualAddress, Size)));
        }

        //
        // If the below 1MB page was not allocated but only reserved by HvLoader
        // then reserve the page in the kernel memory map.
        //
        // The following code path (page initialized but not allocated) can currently
        // not happen on ARM64 because hvloader.efi will abort with an error if it cannot
        // allocate space for the transition address space (since ARM64 does not guarantee
        // memory to be present under 1MB unlike Intel).
        // But since hvloader.efi allocates a set of pages (not just one), this path must
        // be updated to carve the same amount of memory from the descriptors in case
        // hvloader ever supports this scenario.
        //

        if (OslArchHvlInitialized() &&
            OslArchHvlBelow1MbPageAllocated() == FALSE) {

            HypervisorLowPageAddress = OslArchHvlGetBelow1MbPage();

            if (IS_ADDR_CONTAINED_IN_MD_UNMAPPED(MemoryDescriptor,
                                                 HypervisorLowPageAddress)) {

                //
                // Split the descriptor to reserve one page for the hypervisor.
                //

                if (SizeRemaining < (3 * sizeof(MEMORY_ALLOCATION_DESCRIPTOR))) {
                    Status = STATUS_BUFFER_TOO_SMALL;
                    goto BuildKernelMemoryMapEnd;
                }

                SplitCount = OslpSplitSinglePageMemoryDescriptor(
                                 MemoryDescriptor,
                                 CurrentDescriptor,
                                 HypervisorLowPageAddress >> PAGE_SHIFT,
                                 LoaderFirmwarePermanent);

                //
                // Add all split descriptors to the kernel's memory map.
                // Splitting the hypervisor page descriptor should generate a
                // minimum of one descriptor & maximum of three descriptors,
                // depending on whether there is any unused region before or
                // after the hypervisor page.
                //

                if ((SplitCount < 1) || (SplitCount > 3)) {
                    Status = STATUS_UNSUCCESSFUL;
                    goto BuildKernelMemoryMapEnd;
                }

                //
                // Add the split descriptors to the kernel's memory map.
                //

                while (SplitCount > 0) {
                    InsertTailList(&LoaderBlock->MemoryDescriptorListHead,
                                   &CurrentDescriptor->ListEntry);

                    CurrentDescriptor += 1;
                    SizeRemaining -= sizeof(MEMORY_ALLOCATION_DESCRIPTOR);
                    SplitCount -= 1;
                }

                //
                // Continue to next descriptor.
                //

                NextEntry = NextEntry->Flink;
                continue;
            }
        }

#if (OSL_IMAGE_RANDOMNESS > 0)

        //
        // In case if loader implements the ASLR by using a combined buffer
        // for the kernel and hal and extra space for the randomization,
        // that extra space need to be freed.
        // Check if the memory descriptor describes combined kernel and HAL
        // image. If so, this descriptor should be split into up to four
        // descriptors: unused descriptor before kernel/HAL, kernel, HAL,
        // unused descriptor after kernel/HAL. It's possible that there is
        // no unused descriptor before or/and after kernel/HAL. Otherwise,
        // simply add the descriptor to the kernel's memory map.
        //

        if (IS_ADDR_CONTAINED_IN_MD(MemoryDescriptor, KernelEntry->DllBase) &&
            IS_ADDR_CONTAINED_IN_MD(MemoryDescriptor, HalEntry->DllBase)) {

            //
            // Split the memory descriptor.
            //

            if (SizeRemaining < (4 * sizeof(MEMORY_ALLOCATION_DESCRIPTOR))) {
                Status = STATUS_BUFFER_TOO_SMALL;
                goto BuildKernelMemoryMapEnd;
            }

            SplitCount = OslpSplitKernelHalMemoryDescriptor(MemoryDescriptor,
                                                            CurrentDescriptor,
                                                            KernelEntry,
                                                            HalEntry);

            //
            // Add all split descriptors to the kernel's memory map. Splitting
            // the kernel/hal descriptor should generate a minimum of two
            // descriptors & maximum of four descriptors, depending on
            // whether there is any unused region before or after the
            // kernel/HAL regions.
            //

            if ((SplitCount < 2) || (SplitCount > 4)) {
                Status = STATUS_UNSUCCESSFUL;
                goto BuildKernelMemoryMapEnd;
            }

            while (SplitCount > 0) {
                InsertTailList(&LoaderBlock->MemoryDescriptorListHead,
                               &CurrentDescriptor->ListEntry);

                CurrentDescriptor += 1;
                SizeRemaining -= sizeof(MEMORY_ALLOCATION_DESCRIPTOR);
                SplitCount -= 1;
            }

        } else

#endif

        {

            //
            // If the previous memory descriptor has the same type, coalesce the
            // adjacent descriptors to reduce the size of the list.
            //

            if (IsListEmpty(&LoaderBlock->MemoryDescriptorListHead) == FALSE) {

                PreviousDescriptor =
                    CONTAINING_RECORD(LoaderBlock->MemoryDescriptorListHead.Blink,
                                      MEMORY_ALLOCATION_DESCRIPTOR,
                                      ListEntry);

                if (CurrentDescriptor->MemoryType ==
                    PreviousDescriptor->MemoryType) {

                    if (CurrentDescriptor->BasePage ==
                        (PreviousDescriptor->BasePage +
                         PreviousDescriptor->PageCount)) {

                        PreviousDescriptor->PageCount +=
                            CurrentDescriptor->PageCount;

                        NextEntry = NextEntry->Flink;
                        continue;
                    }
                }
            }

            //
            // Add the descriptor to the kernel's memory map and move to the
            // next descriptor.
            //

            InsertTailList(&LoaderBlock->MemoryDescriptorListHead,
                           &CurrentDescriptor->ListEntry);

            CurrentDescriptor += 1;
            SizeRemaining -= sizeof(MEMORY_ALLOCATION_DESCRIPTOR);
        }

        NextEntry = NextEntry->Flink;
    }

    Status = STATUS_SUCCESS;

    //
    // Build the free list by linking together all remaining entries in the
    // descriptor list.
    //

    InitializeListHead(FreeDescriptorsList);
    *ReservedDescriptorsCount = 0;
    while (SizeRemaining >= sizeof(MEMORY_ALLOCATION_DESCRIPTOR)) {
        InsertTailList(FreeDescriptorsList, &CurrentDescriptor->ListEntry);
        *ReservedDescriptorsCount += 1;
        CurrentDescriptor += 1;
        SizeRemaining -= sizeof(MEMORY_ALLOCATION_DESCRIPTOR);
    }

BuildKernelMemoryMapEnd:
    return Status;
}

NTSTATUS
OslpLoadCrashDump (
    __in DEVICE_ID TriageDeviceId,
    __deref_out_bcount_opt(sizeof(DUMP_HEADER)) PVOID *DumpHeaderAddress,
    __deref_out_bcount_opt(TRIAGE_DUMP_SIZE) PVOID *TriageDumpAddress
    )

/*++

Routine Description:

    This routine loads the dump data, if it exists. The dump header is always
    loaded. If the dump is a triage dump, then the rest of the triage dump is
    loaded as well.

Arguments:

    TriageDeviceId - The id of the device where we should check for the triage
        dump in the pagefile.

    DumpHeader - Where the dump header pointer is copied to on success.

    TriageDump - Where the triage dump pointer is copied to on success.

Return Value:

    NTSTATUS

--*/

{

    ULONG ByteCount;
    PDUMP_HEADER DumpHeader;
    PMEMORY_DUMP MemoryDump;
    FILE_ID PageFileId;
    NTSTATUS Status;
    BOOLEAN TriageDump;
    PULONG ValidPtr;

    ASSERT(TriageDeviceId != INVALID_DEVICE_ID);
    ASSERT(TriageDumpAddress != NULL);

    PageFileId = INVALID_FILE_ID;
    DumpHeader = NULL;
    MemoryDump = NULL;
    *TriageDumpAddress = NULL;
    *DumpHeaderAddress = NULL;

    //
    // The pagefile contains the triage dump information.
    //

    Status = BlFileOpen(TriageDeviceId, PAGEFILE_SYS, OPEN_READ, &PageFileId);
    if (!NT_SUCCESS(Status)) {
        goto OslpLoadCrashDumpEnd;
    }

    //
    // Allocate the buffer for the dump header.
    //

    DumpHeader = (PDUMP_HEADER)BlMmAllocateHeap(sizeof(DUMP_HEADER));
    if (DumpHeader == NULL) {

        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto OslpLoadCrashDumpEnd;
    }

    //
    // Read the dump header out of the pagefile.
    //

    Status = BlFileReadAtOffset(PageFileId,
                                sizeof(DUMP_HEADER),
                                0,
                                DumpHeader,
                                &ByteCount);

    if (!NT_SUCCESS(Status) || (ByteCount != sizeof(DUMP_HEADER))) {
        goto OslpLoadCrashDumpEnd;
    }

    //
    // Verify this is a valid dump header.
    //

    if ((DumpHeader->ValidDump != DUMP_VALID_DUMP) ||
        (DumpHeader->Signature != DUMP_SIGNATURE)) {

        Status = STATUS_INVALID_IMAGE_FORMAT;
        goto OslpLoadCrashDumpEnd;
    }

    if (DumpHeader->DumpType == DUMP_TYPE_TRIAGE) {
        TriageDump = TRUE;

    } else {
        TriageDump = FALSE;
    }

    *DumpHeaderAddress = DumpHeader;
    DumpHeader = NULL;

    //
    // Check if a triage dump needs to be loaded as well.
    //

    if (TriageDump == FALSE) {

        Status = STATUS_SUCCESS;
        goto OslpLoadCrashDumpEnd;
    }

    //
    // Allocate the buffer for the triage dump.
    //

    MemoryDump = (PMEMORY_DUMP)BlMmAllocateHeap(TRIAGE_DUMP_SIZE);
    if (MemoryDump == NULL) {

        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto OslpLoadCrashDumpEnd;
    }

    //
    // Read the triage dump out of the pagefile.
    //

    Status = BlFileReadAtOffset(PageFileId,
                                TRIAGE_DUMP_SIZE,
                                0,
                                MemoryDump,
                                &ByteCount);

    if (!NT_SUCCESS(Status) || (ByteCount != TRIAGE_DUMP_SIZE)) {
        goto OslpLoadCrashDumpEnd;
    }

    //
    // Verify that this is a valid triage memory dump file. The dump header has
    // already been validated.
    //

    ValidPtr = Add2Ptr(MemoryDump, MemoryDump->Triage.ValidOffset);
    if ((MemoryDump->Triage.ValidOffset > (TRIAGE_DUMP_SIZE - sizeof(ULONG))) ||
        (*ValidPtr != TRIAGE_DUMP_VALID)) {

        Status = STATUS_INVALID_IMAGE_FORMAT;
        goto OslpLoadCrashDumpEnd;
    }

    Status = STATUS_SUCCESS;
    *TriageDumpAddress = MemoryDump;
    BlMmFreeHeap(*DumpHeaderAddress);
    *DumpHeaderAddress = MemoryDump;
    MemoryDump = NULL;

    //
    // Clean up allocations and resources.
    //

OslpLoadCrashDumpEnd:
    if (MemoryDump != NULL) {
        BlMmFreeHeap(MemoryDump);
        MemoryDump = NULL;
    }

    if (DumpHeader != NULL) {
        BlMmFreeHeap(DumpHeader);
        DumpHeader = NULL;
    }

    if (PageFileId != INVALID_FILE_ID) {
        BlFileClose(PageFileId);
        PageFileId = INVALID_FILE_ID;
    }

    return Status;
}


NTSTATUS
OslpInitializeNetworkLoaderBlock (
    __deref_out PNETWORK_LOADER_BLOCK *NetworkLoaderBlock
    )

/*++

Routine Description:

    This routine will attempt to find PXE boot settings and populate the
    provided network loader block.

Arguments:

    NetworkLoaderBlock - On successful output, contains the address of an
        allocated buffer containing network PXE boot information.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_NO_MEMORY if a memory allocation fails.
    STATUS_UNSUCCESSFUL if the PXE boot information structure is not
        initialized.

--*/

{

    PNETWORK_LOADER_BLOCK NetBlock;
    PPXE_INFO PxeInfo;
    NTSTATUS Status;

    NetBlock = NULL;

    //
    // The network loader block is only allocated and populated on a network
    // boot.  All other boot scenarios can exit early.
    //

    if (BlGetBootDevice()->DeviceType != DEVICE_UDP) {
        *NetworkLoaderBlock = NULL;
        return STATUS_SUCCESS;
    }

    //
    // The network loader block contains the contents of the DHCP ACK packet
    // and the BINL reply packet.  Both packets are contains in the PXE
    // boot information data structure.
    //

    PxeInfo = BlNetGetPxeInfo();
    if (PxeInfo == NULL) {
        return STATUS_UNSUCCESSFUL;
    }

    Status = STATUS_SUCCESS;
    NetBlock = BlMmAllocateHeap(sizeof(NETWORK_LOADER_BLOCK));
    if (NetBlock == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OslpInitializeNetworkLoaderBlockEnd;
    }

    RtlZeroMemory(NetBlock, sizeof(NETWORK_LOADER_BLOCK));

    //
    // The PXE_INFO structure contains pointers to both the DHCP ACK packet
    // and the BINL reply packet.  Both are required by the Operating System
    // and included in the network loader block.  The reply packets are
    // allocated by the Boot Environment Library during initialization.  The
    // packets must be in kernel address space.  If the library ever stops
    // allocating memory for these packets, during the construction of the
    // network loader block, buffers would need to be allocated to ensure
    // their presence in kernel address space.
    //

    NetBlock->DHCPServerACK = PxeInfo->DhcpAckPacketBuf;
    NetBlock->DHCPServerACKLength = PxeInfo->DhcpAckPacketSize;
    NetBlock->BootServerReplyPacket = PxeInfo->BinlReplyPacketBuf;
    NetBlock->BootServerReplyPacketLength = PxeInfo->BinlReplyPacketSize;

    *NetworkLoaderBlock = NetBlock;

OslpInitializeNetworkLoaderBlockEnd:

    if (!NT_SUCCESS(Status)) {
        if (NetBlock != NULL) {
            BlMmFreeHeap(NetBlock);
        }
    }

    return Status;
}

NTSTATUS
OslpCopyEmsSettings (
    __in_opt PSMBIOS3_EPS_HEADER SmBiosEps,
    __deref_out PHEADLESS_LOADER_BLOCK *EmsLoaderBlock
    )

/*++

Routine Description:

    This routine will attempt to find EMS settings, and populate the
    provided headless loader block.

Arguments:

    SmBiosEps - Supplies a reference to a copy of the system management BIOS3
        entry point structure if the system supplies one.

    EmsLoaderBlock - On successful output, contains the address of an allocated
        buffer containing headless redirection settings.

Return Value:

    STATUS_SUCCESS when successful.
    STATUS_VARIABLE_NOT_FOUND if the loader is not redirecting.
    STATUS_NOT_FOUND if the SPCR table is required, but not present.
    STATUS_INVALID_PARAMETER if an EMS setting is invalid.

--*/

{

    ULONG_PTR AddressUlongPtr;
    ULONGLONG BaudRate;
    ULONG Count;
    BOOLEAN EmsEnabled;
    PHEADLESS_LOADER_BLOCK LoaderBlock;
    ULONGLONG PortAddress;
    ULONGLONG PortNumber;
    PSERIAL_PORT_REDIRECTION_TABLE SpcrTable;
    NTSTATUS Status;

    ASSERT(ARGUMENT_PRESENT(EmsLoaderBlock) != FALSE);

    LoaderBlock = NULL;

    //
    // If EMS is not enabled, there are no settings to copy.
    //

    Status = BlGetApplicationOptionBoolean(
                 BCDE_OSLOADER_TYPE_KERNEL_EMS_ENABLED,
                 &EmsEnabled);

    if (!NT_SUCCESS(Status) || (EmsEnabled == FALSE)) {
        Status = STATUS_NOT_FOUND;
        goto OslpCopyEmsSettingsEnd;
    }

    LoaderBlock = BlMmAllocateHeap(sizeof(HEADLESS_LOADER_BLOCK));
    if (LoaderBlock == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OslpCopyEmsSettingsEnd;
    }

    RtlZeroMemory(LoaderBlock, sizeof(HEADLESS_LOADER_BLOCK));

    //
    // Get the EMS port number from the application options.  If this is not
    // supplied, use the BIOS redirection settings.
    //

    Status = BlGetApplicationOptionInteger(BCDE_LIBRARY_TYPE_EMS_PORT_NUMBER,
                                           &PortNumber);

    if (NT_SUCCESS(Status)) {
        LoaderBlock->PortNumber = (ULONG)PortNumber;
        if (PortNumber == 1) {
            PortAddress = 0x3f8;

        } else if (PortNumber == 2) {
            PortAddress = 0x2f8;

        } else if (PortNumber == 3) {
            PortAddress = 0x3e8;

        } else if (PortNumber == 4) {
            PortAddress = 0x2e8;

        } else {
            Status = STATUS_INVALID_PARAMETER;
            goto OslpCopyEmsSettingsEnd;
        }

        LoaderBlock->PortAddress = (PUCHAR)(ULONG_PTR)PortAddress;

        //
        // Check for an EMS baudrate option. This is not required
        // but can override the default baud rate of 9600 bps.
        //

        LoaderBlock->BaudRate = 9600;
        Status = BlGetApplicationOptionInteger(
                     BCDE_LIBRARY_TYPE_EMS_BAUDRATE,
                     &BaudRate);

        if (NT_SUCCESS(Status)) {
            Count = 0;
            while (Count < RTL_NUMBER_OF(OslConsoleBaudRates)) {
                if (BaudRate == OslConsoleBaudRates[Count].BaudRate) {
                    LoaderBlock->BaudRate = OslConsoleBaudRates[Count].BaudRate;
                    break;
                }

                Count++;
            }
        }

        //
        // Fill in the remaining fields.
        //

        LoaderBlock->UsedBiosSettings = TRUE;
        LoaderBlock->DataBits = 8;
        LoaderBlock->StopBits = 1;
        LoaderBlock->Parity = FALSE;
        LoaderBlock->PciDeviceId = (USHORT)0xffff;
        LoaderBlock->PciVendorId = (USHORT)0xffff;
        LoaderBlock->IsMMIODevice = FALSE;
        LoaderBlock->TerminalType = 2;

    } else {

        //
        // No port number override is specified.  Use the BIOS settings.  If
        // an SPCR table does not exist, redirection cannot be enabled.
        //

        Status = OslpGetSpcrTable(&SpcrTable);
        if (!NT_SUCCESS(Status)) {
            goto OslpCopyEmsSettingsEnd;
        }

        if (SpcrTable->BaseAddress.AddressSpaceID == 0) {
            LoaderBlock->IsMMIODevice = TRUE;
        }

        AddressUlongPtr = (ULONG_PTR)SpcrTable->BaseAddress.Address.QuadPart;
        ASSERT((ULONGLONG)AddressUlongPtr ==
               (ULONGLONG)SpcrTable->BaseAddress.Address.QuadPart);

        LoaderBlock->UsedBiosSettings = TRUE;
        LoaderBlock->PortNumber = 3;
        LoaderBlock->PortAddress = (PUCHAR)AddressUlongPtr;
        LoaderBlock->BaudRate = OslpConvertBaudRate(SpcrTable->BaudRate);
        LoaderBlock->Parity = SpcrTable->Parity;
        LoaderBlock->StopBits = SpcrTable->StopBits;
        LoaderBlock->TerminalType = SpcrTable->TerminalType;
        if (SpcrTable->Header.Revision >= 2) {
            LoaderBlock->InterfaceType = SpcrTable->InterfaceType;
            LoaderBlock->RegisterBitWidth = SpcrTable->BaseAddress.BitWidth;
        }

        if (SpcrTable->Header.Length >= sizeof(SERIAL_PORT_REDIRECTION_TABLE)) {

            //
            // This is the new table, and so it contains all of the PCI device
            // information.
            //

            LoaderBlock->PciDeviceId = SpcrTable->PciDeviceId;
            LoaderBlock->PciVendorId = SpcrTable->PciVendorId;
            LoaderBlock->PciBusNumber = SpcrTable->PciBusNumber;
            LoaderBlock->PciBusSegment = SpcrTable->PciSegment;
            LoaderBlock->PciSlotNumber = SpcrTable->PciSlotNumber;
            LoaderBlock->PciFunctionNumber = SpcrTable->PciFunctionNumber;
            LoaderBlock->PciFlags = SpcrTable->PciFlags;

        } else {

            //
            // There's PCI device information in this table.
            //

            LoaderBlock->PciDeviceId = (USHORT)0xffff;
            LoaderBlock->PciVendorId = (USHORT)0xffff;
        }
    }

    //
    // If the system has provided a system management BIOS, then determine the
    // system identifier and place it into the EMS loader block.
    //

    OslpLoadSystemGuid(SmBiosEps, &LoaderBlock->SystemGUID);

OslpCopyEmsSettingsEnd:

    if (NT_SUCCESS(Status)) {
        *EmsLoaderBlock = LoaderBlock;

    } else {
        *EmsLoaderBlock = NULL;
        if (LoaderBlock != NULL) {
            BlMmFreeHeap(LoaderBlock);
        }
    }

    return  Status;
}


NTSTATUS
OslpGetSpcrTable (
    __deref_out PSERIAL_PORT_REDIRECTION_TABLE *SpcrTable
    )

/*++

Routine Description:

    This routine will attempt to recover a pointer to the SPCR table.  It will
    fail if the table is not found, or if the table is not valid.  If the table
    is found, but is not valid, the pointer is returned anyway.  The return
    value of the function should be used to determine success or failure.

Arguments:

    SpcrTable - the pointer to the SPCR table will be returned here.

Return Value:

    NTSTATUS indicating the success or failure of the operation.

--*/

{

    NTSTATUS Status;
    PSERIAL_PORT_REDIRECTION_TABLE Spcr;
    UCHAR Checksum;

    //
    // Try to find the SPCR table, and fail if we can't find it.
    //

    Status = BlUtlGetAcpiTable(&Spcr, SPCR_SIGNATURE);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Calculate the checksum, ensuring that it is zero.
    //

    Checksum = (UCHAR)BlUtlCheckSum(0,
                                    Spcr,
                                    Spcr->Header.Length,
                                    CHECKSUM_BYTE_ARRAY);

    if (Checksum != 0) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Perform some sanity checks on the information contained in the SPCR
    // table.
    //

    if (Spcr->BaseAddress.Address.QuadPart == 0) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // The address space ID needs to be system or memory mapped.
    //

    if ((Spcr->BaseAddress.AddressSpaceID != 0) &&
        (Spcr->BaseAddress.AddressSpaceID != 1)) {

        return STATUS_INVALID_PARAMETER;
    }

    //
    // Return the SPCR table to the caller.
    //

    *SpcrTable = Spcr;

    return STATUS_SUCCESS;
}


ULONG
OslpConvertBaudRate (
    UCHAR Identifier
    )

/*++

Routine Description:

    This routine will convert the baud rate identifier field from the SPCR
    table into the actual baud rate.  Note that if an invalid identifier
    is specified, the returned rate will default to 9600 bps.

Arguments:

    Identifier - Provides the identifier to be converted into a baud rate.


Return Value:

    The converted baud rate in bits per second.

--*/

{

    UCHAR Count;

    Count = 0;
    while (Count < RTL_NUMBER_OF(OslConsoleBaudRates)) {
        if (Identifier == OslConsoleBaudRates[Count].Identifier) {
            return OslConsoleBaudRates[Count].BaudRate;
        }

        Count++;
    }

    return 9600;
}


NTSTATUS
OslpLoadSystemGuid (
    __in_opt PSMBIOS3_EPS_HEADER SmBiosEps,
    __out GUID *Guid
    )

/*++

Routine Description:

    This routine attempts to discover the system GUID, and returns it in the
    buffer provided.

Arguments:

    SmBiosEps - Supplies a reference to a copy of the system management BIOS 3
        entry point structure.

    Guid - the system GUID will be returned in this buffer.

Return Value:

    None

--*/

{

    PUCHAR CurrentAddress;
    ULONG Length;
    NTSTATUS Status;
    PUCHAR StructureTable;
    PHYSICAL_ADDRESS StructureTableAddress;
    PUCHAR StructureTableEnd;
    PSMBIOS_SYSTEM_INFORMATION_STRUCT SystemInformation;
    ULONG ValidLength;

    if (SmBiosEps == NULL) {
            return STATUS_NOT_FOUND;
    }

    StructureTableAddress.QuadPart = SmBiosEps->StructureTableAddress;
    Length = SmBiosEps->StructureTableMaximumSize;
    StructureTable = NULL;
    Status = BlMmMapPhysicalAddress(&StructureTable,
                                    StructureTableAddress,
                                    Length,
                                    MEMORY_ATTRIBUTE_NONE);

    if (!NT_SUCCESS(Status)) {
        goto LoadSystemGuidEnd;
    }

    RtlZeroMemory(Guid, sizeof(GUID));

    //
    // Scan the packed structure table for the system information type. Continue
    // until there is not enough data remaining to contain a system information
    // type structure.
    //

    CurrentAddress = StructureTable;
    StructureTableEnd = StructureTable + Length;
    ValidLength = RTL_SIZEOF_THROUGH_FIELD(
                      SMBIOS_SYSTEM_INFORMATION_STRUCT,
                      Uuid);

    while ((CurrentAddress + ValidLength) <= StructureTableEnd) {
        SystemInformation = (PSMBIOS_SYSTEM_INFORMATION_STRUCT)CurrentAddress;
        if (SystemInformation->Type == SMBIOS_SYSTEM_INFORMATION) {
            if (SystemInformation->Length >= ValidLength) {
                RtlCopyMemory(Guid, &SystemInformation->Uuid[0], sizeof(GUID));
                Status = STATUS_SUCCESS;
                goto LoadSystemGuidEnd;
            }
        }

        //
        // Jump to the end of the formatted portion of the current SMBIOS
        // structure and then search for the double NULL terminating characters
        // indicating the end of the structure.
        //

        CurrentAddress += SystemInformation->Length;
        while ((*((USHORT UNALIGNED*)CurrentAddress) != 0) &&
               ((CurrentAddress + ValidLength) <= StructureTableEnd)) {

            CurrentAddress += 1;
        }

        if ((CurrentAddress + ValidLength) > StructureTableEnd) {
            break;
        }

        CurrentAddress += 2;
    }

LoadSystemGuidEnd:
    if (StructureTable != NULL) {
        BlMmUnmapVirtualAddress(StructureTable, Length);
    }

    return Status;
}

VOID
OslpConvertMemoryDescriptor (
    __out PMEMORY_ALLOCATION_DESCRIPTOR KernelDescriptor,
    __in PMEMORY_DESCRIPTOR LibraryDescriptor
   )

/*++

Routine Description:

    Initializes a kernel memory descriptor based on the values of the specified
    Boot Environment Library memory descriptor.

Arguments:

    KernelDescriptor - Supplies a pointer to a kernel memory descriptor to
        initialize.

    LibraryDescriptor - Supplies a pointer to a Boot Environment Library
        memory descriptor describing a physical address region.

Return Value:

    None.

--*/

{

    ULONGLONG MaxPhysicalAddress;
    ULONGLONG MaxPhysicalPage;
    ULONGLONG SpannedPages;
    NTSTATUS Status;
    UNPACKED_MEMORY_TYPE UnpackedMemoryType;

    KernelDescriptor->BasePage = (ULONG_PTR)LibraryDescriptor->BasePage;
    KernelDescriptor->PageCount = (ULONG_PTR)LibraryDescriptor->PageCount;

    //
    // Map the library's memory type into the memory type used by the kernel.
    //

    UnpackedMemoryType.MemoryType = LibraryDescriptor->MemoryType;
    switch (UnpackedMemoryType.Class) {
    case MEMORY_CLASS_PLATFORM:

        //
        // Fork based on platform sub-types, mapping each to the
        // appropriate kernel type.
        //

        switch (UnpackedMemoryType.SubType) {
        case MEMORY_PLATFORM_TYPE_AVAILABLE:
        case MEMORY_PLATFORM_TYPE_HARDWARE_ZEROED:

            //
            // Remain consistent with the old loader which marks large
            // page slush as firmware temporary memory.
            //

            if (CHECK_FLAG(LibraryDescriptor->Attributes,
                           MEMORY_ATTRIBUTE_ALLOCATION_LARGE_PAGE)) {

                KernelDescriptor->MemoryType = LoaderFirmwareTemporary;

            } else {

                //
                // Mark the low 1MB of memy as firmware temporary.
                //

                if (CHECK_FLAG(LibraryDescriptor->Attributes,
                               MEMORY_ATTRIBUTE_ALLOCATION_REALMODE_ACCESSIBLE)) {

                    KernelDescriptor->MemoryType = LoaderFirmwareTemporary;

                } else if (UnpackedMemoryType.SubType ==
                           MEMORY_PLATFORM_TYPE_HARDWARE_ZEROED) {

                    KernelDescriptor->MemoryType = LoaderZero;

                } else {
                    KernelDescriptor->MemoryType = LoaderFree;
                }
            }

            break;

        case MEMORY_PLATFORM_TYPE_BAD:
            KernelDescriptor->MemoryType = LoaderBad;
            break;

        case MEMORY_PLATFORM_TYPE_RESERVED:
        case MEMORY_PLATFORM_TYPE_MMIO_SPACE:
        case MEMORY_PLATFORM_TYPE_MMIO_PORT_SPACE:
            KernelDescriptor->MemoryType = LoaderFirmwareReserved;
            break;

        case MEMORY_PLATFORM_TYPE_ACPI_TABLES:
        case MEMORY_PLATFORM_TYPE_ACPI:
        case MEMORY_PLATFORM_TYPE_PAL_CODE:
            KernelDescriptor->MemoryType = LoaderFirmwarePermanent;
            break;

        case MEMORY_PLATFORM_TYPE_RUNTIME_SERVICE_CODE:
            KernelDescriptor->MemoryType = LoaderFirmwareCode;
            break;

        case MEMORY_PLATFORM_TYPE_BOOT_SERVICE_CODE:
            KernelDescriptor->MemoryType = LoaderFirmwareTemporary;
            break;

        case MEMORY_PLATFORM_TYPE_RUNTIME_SERVICE_DATA:
            KernelDescriptor->MemoryType = LoaderFirmwareData;
            break;

        case MEMORY_PLATFORM_TYPE_ERROR_LOG:
            KernelDescriptor->MemoryType = LoaderErrorLogMemory;
            break;

        case MEMORY_PLATFORM_TYPE_ENCLAVE:
            KernelDescriptor->MemoryType = LoaderEnclaveMemory;
            break;

        default:

            //
            // If the platform sub-type is unknown, assume the descriptor was
            // initialized by a previously executing boot application.  Mark
            // the memory range as off-limits to the Operating System, making
            // no assumptions of the memory's usage.  The memory could have
            // been used in page tables or other memory which can not be
            // reclaimed and it is safest to be cautious and allow the OS to
            // boot.
            //

            KernelDescriptor->MemoryType = LoaderFirmwarePermanent;
            break;
        }

        break;

    case MEMORY_CLASS_LIBRARY:

        //
        // Fork based on library sub-types, mapping each to the appropriate
        // kernel type.  Library code which is part of the loader's address
        // space include any memory which needs to be present after kernel
        // phase 0 initialization.
        //

        switch (UnpackedMemoryType.SubType) {
        case MEMORY_LIBRARY_TYPE_APPLICATION_DATA_STRUCTURE:
        case MEMORY_LIBRARY_TYPE_DEBUGGER_DATA:

            //
            // When debugging the transition to the kernel, prevent reuse of
            // memory that may be used by the boot debugger transport.  This
            // enables debugging of the kernel debug transport initialization
            // code by the boot debugger.
            //

            KernelDescriptor->MemoryType = LoaderFirmwareTemporary;
            if (BlBdDebugTransitionsEnabled() != FALSE) {
                KernelDescriptor->MemoryType = LoaderFirmwarePermanent;
            }

            break;

        case MEMORY_LIBRARY_TYPE_UNAVAILABLE:
        case MEMORY_LIBRARY_TYPE_MISC_DATA:
        case MEMORY_LIBRARY_TYPE_PAGE_TABLE_DATA:
        case MEMORY_LIBRARY_TYPE_DEVICE_DATA:
        case MEMORY_LIBRARY_TYPE_BLOCK_CACHE_DATA:
        case MEMORY_LIBRARY_TYPE_BCD:
        case MEMORY_LIBRARY_TYPE_BITMAP_DATA:

            //
            // Library memory which does not need to be consumed by the
            // kernel can be marked with a memory type which will be freed
            // during kernel phase 0 initialization.
            //

            KernelDescriptor->MemoryType = LoaderFirmwareTemporary;
            break;

        case MEMORY_LIBRARY_TYPE_HEAP:
        case MEMORY_LIBRARY_TYPE_BGFX:

            //
            // The kernel's memory manager will not free loader heap
            // regions during phase 0 initialization since it contains
            // data, such as the loader block, which is consumed throughout
            // kernel initialization.
            //
            // At the same time, MM will bugcheck if there are any loader
            // allocations that are NOT freed during phase 0 and are
            // additionally NOT mapped in kernel address space.  This means
            // that anything with the memory type LoaderOsloaderHeap must
            // be mapped.  Since the boot manager does not use address
            // translation, its heap is not mapped into kernel address
            // space.  This memory, and any like it, needs to be marked by
            // a memory type that is freed during phase 0 initialization.
            //

            if (LibraryDescriptor->MappedBasePage == 0) {
                KernelDescriptor->MemoryType = LoaderFirmwareTemporary;

            } else {

                ASSERT(KSEG_PAGE(LibraryDescriptor->MappedBasePage) != FALSE);

                KernelDescriptor->MemoryType = LoaderOsloaderHeap;
            }

            break;

        case MEMORY_LIBRARY_TYPE_BOOT_APPLICATION:
            KernelDescriptor->MemoryType = LoaderLoadedProgram;
            break;

        case MEMORY_LIBRARY_TYPE_RAMDISK:

            //
            // The RAM disk driver determines the physical location of a
            // RAM disk by looking for the memory type, LoaderXIPRom.
            //
            // It is possible for other ramdisk images to be loaded into memory
            // by the previous boot application prior to invoking the current
            // boot application. The OS has a limitation in that it can only
            // handle a single ramdisk being present in memory (first
            // LoaderXIPRom descriptor is assumed to be the OS ramdisk device;
            // others are ignored). To prevent the OS from using an incorrect
            // ramdisk device in such scenario, convert non-OS ramdisk devices
            // to free memory regions.
            //

            Status = OslpCheckOsRamdiskDeviceDescriptor(LibraryDescriptor);
            if (Status == STATUS_SUCCESS) {
                KernelDescriptor->MemoryType = LoaderXIPRom;

            } else {
                KernelDescriptor->MemoryType = LoaderFree;
            }

            break;

        case MEMORY_LIBRARY_TYPE_PAGE_TABLE:
            KernelDescriptor->MemoryType = LoaderMemoryData;
            break;

        case MEMORY_LIBRARY_TYPE_SELF_MAP_RESERVED:

            //
            // The self map reserved region can be immediately reclaimed.
            //

            KernelDescriptor->MemoryType = LoaderFree;
            break;

        default:

            //
            // If the library sub-type is unknown, assume the descriptor was
            // initialized by a previously executing boot application.  Mark
            // the memory range as off-limits to the Operating System, making
            // no assumptions of the memory's usage.  The memory could have
            // been used in page tables or other memory which can not be
            // reclaimed and it is safest to be cautious and allow the OS to
            // boot.
            //

            KernelDescriptor->MemoryType = LoaderFirmwarePermanent;
            break;
        }

        break;

    case MEMORY_CLASS_APPLICATION:

        //
        // Fork based on application sub-types, mapping each to the appropriate
        // kernel type.
        //

        switch (UnpackedMemoryType.SubType) {
        case MEMORY_APPLICATION_TYPE_UNAVAILABLE:
        case MEMORY_APPLICATION_TYPE_ERROR_DATA:
        case MEMORY_APPLICATION_TYPE_LOG_DATA:
            KernelDescriptor->MemoryType = LoaderFirmwareTemporary;
            break;

        case MEMORY_APPLICATION_TYPE_PERSISTENT_DATA:

            //
            // Mark Application Persistent Data with a memory type which will
            // be available through all parts of phase 1 kernel
            // initialization, but yet still reclaimable by MM.
            //
            // The one kernel memory type which has this characteristic
            // is LoaderOsloaderHeap.
            //

            ASSERT(KSEG_PAGE(LibraryDescriptor->MappedBasePage) != FALSE);

            KernelDescriptor->MemoryType = LoaderOsloaderHeap;
            break;

        default:
            if (UnpackedMemoryType.SubType >=
                MEMORY_APPLICATION_TYPE_DEFINABLE_START) {

                switch (UnpackedMemoryType.MemoryType) {
                case OSL_MEMORY_TYPE_KERNEL_DATA_STRUCTURE:

                    //
                    // Mark memory allocated for kernel data structure with
                    // a memory type which will not be unmapped or reclaimed.
                    //

                    KernelDescriptor->MemoryType = LoaderMemoryData;
                    break;

                case OSL_MEMORY_TYPE_SYSTEM_CODE:
                    KernelDescriptor->MemoryType = LoaderSystemCode;
                    break;

                case OSL_MEMORY_TYPE_BOOT_DRIVER:
                    KernelDescriptor->MemoryType = LoaderBootDriver;
                    break;

                case OSL_MEMORY_TYPE_MISC_MODULE:

                    //
                    // Mark miscellaneous modules to remain available through
                    // phase 1 kernel initialization, but then be reclaimed by
                    // MM.
                    //

                    KernelDescriptor->MemoryType = LoaderOsloaderHeap;
                    break;

                case OSL_MEMORY_TYPE_KERNEL_PAGE_TABLE:
                    KernelDescriptor->MemoryType = LoaderMemoryData;
                    break;

                case OSL_MEMORY_TYPE_NLS_DATA:
                    KernelDescriptor->MemoryType = LoaderNlsData;
                    break;

                case OSL_MEMORY_TYPE_REGISTRY_DATA:
                    KernelDescriptor->MemoryType = LoaderRegistryData;
                    break;

                case OSL_MEMORY_TYPE_DPC_STACK:
                    KernelDescriptor->MemoryType = LoaderStartupDpcStack;
                    break;

                case OSL_MEMORY_TYPE_KERNEL_STACK:
                    KernelDescriptor->MemoryType = LoaderStartupKernelStack;
                    break;

                case OSL_MEMORY_TYPE_KERNEL_INIT_DATA:

                    //
                    // Mark special loader data with a memory type which will
                    // be available through all parts of phase 1 kernel
                    // initialization, but yet still reclaimable by MM.  The
                    // one kernel memory type which has this characteristic
                    // is LoaderOsloaderHeap.
                    //

                    ASSERT(KSEG_PAGE(LibraryDescriptor->MappedBasePage) !=
                           FALSE);

                    KernelDescriptor->MemoryType = LoaderOsloaderHeap;
                    break;

                case OSL_MEMORY_TYPE_RECLAIMABLE_ALLOCATION:
                    KernelDescriptor->MemoryType = LoaderFirmwareTemporary;
                    break;

                case OSL_MEMORY_TYPE_ACPI_TABLE:

                    //
                    // The optional ACPI table override and the iSCSI ACPI table
                    // cannot be unmapped or reclaimed.
                    //

                    KernelDescriptor->MemoryType = LoaderMemoryData;
                    break;

                case OSL_MEMORY_TYPE_VSM_PAGES:

                    //
                    // Mark pages used by VSM so that they can have their
                    // permissions modified in the SLAT, and can be returned to
                    // NT MM.
                    //

                    KernelDescriptor->MemoryType = LoaderVsmMemory;
                    break;

                case OSL_MEMORY_TYPE_HYPERVISOR_ALLOCATION:

                    //
                    // Mark pages used for hypervisor launch as allocated so
                    // that these pages can be withdrawn from hypervisor and
                    // returned to NT MM.
                    //

                    KernelDescriptor->MemoryType = LoaderHALCachedMemory;
                    break;

                case OSL_MEMORY_TYPE_HYPERVISOR_LOADER:

                    //
                    // Mark pages allocated for hypervisor loader block as
                    // LoaderFirmwareTemporary so that they can be reclaimed
                    // by NT MM after hypervisor launch.
                    //

                    KernelDescriptor->MemoryType = LoaderFirmwareTemporary;
                    break;

                case OSL_MEMORY_TYPE_PERSISTENT_PAGES:

                    //
                    // Mark pages that marked as preserved as
                    // LoaderHALCachedMemory so that these pages can be
                    // returned to NT MM when not needed anymore.
                    //

                    KernelDescriptor->MemoryType = LoaderHALCachedMemory;
                    break;

                case OSL_MEMORY_TYPE_MEMORY_PARTITION_PAGES:

                    //
                    // Mark pages for memory partitions preserved across soft
                    // restart. Those pages will be initially excluded by Mm
                    // initialization, and will be hot-added later.
                    //

                    KernelDescriptor->MemoryType = LoaderFirmwareKsr;
                    break;

                case OSL_MEMORY_TYPE_KD_EXECUTABLE_DATA:

                    //
                    // Mark pages that marked as KD executable data as
                    // LoaderFirmwarePermanent so that these pages will not
                    // be unmapped or reclaimedby NT MM.
                    //

                    KernelDescriptor->MemoryType = LoaderFirmwarePermanent;
                    break;

                default:

                    //
                    // The specified library memory descriptor has an OS Loader
                    // defined memory type which is not properly handled.
                    //

                    ASSERT(FALSE);

                    KernelDescriptor->MemoryType = LoaderFirmwarePermanent;
                    break;
                }

            } else {

                //
                // If the application sub-type is unknown, assume the
                // descriptor was initialized by a previously executing boot
                // application.  Mark the memory range as off-limits to the
                // Operating System, making no assumptions of the memory's
                // usage.  The memory could have been used in page tables or
                // other memory which can not be reclaimed and it is safest to
                // be cautious and allow the OS to boot.
                //

                KernelDescriptor->MemoryType = LoaderFirmwarePermanent;
            }

            break;
        }

        break;

    default:

        //
        // If the memory clas is unknown, assume the descriptor was initialized
        // by a previously executing boot application.  Mark the memory range
        // as off-limits to the Operating System, making no assumptions of the
        // memory's usage.  The memory could have been used in page tables or
        // other memory which can not be reclaimed and it is safest to be
        // cautious and allow the OS to boot.
        //

        KernelDescriptor->MemoryType = LoaderFirmwarePermanent;
        break;
    }

    //
    // Before returning, truncate any firmware temporary memory that is higher
    // than the maximum physical address that should be reported to the kernel.
    // These address where not pruned by the library.
    //

    if (KernelDescriptor->MemoryType == LoaderFirmwareTemporary) {
        Status = BlGetApplicationOptionInteger(
                                 BCDE_LIBRARY_TYPE_TRUNCATE_PHYSICAL_MEMORY,
                                 &MaxPhysicalAddress);

        if (NT_SUCCESS(Status)) {
            MaxPhysicalPage = MaxPhysicalAddress >> PAGE_SHIFT;
            if (KernelDescriptor->BasePage > MaxPhysicalPage) {
                KernelDescriptor->PageCount = 0;

            } else {
                SpannedPages = MaxPhysicalPage - KernelDescriptor->BasePage + 1;
                if (KernelDescriptor->PageCount > SpannedPages) {
                    KernelDescriptor->PageCount = (ULONG_PTR)SpannedPages;
                }
            }
        }
    }

    return;
}

BOOLEAN
OslpNewSignatureNeeded (
    __in PARC_DISK_SIGNATURE DiskSignature,
    __in PLIST_ENTRY DiskSignaturesListHead
    )

/*++

Routine Description:

    This routine checks if the supplied disk signature is unique.

Arguments:

    DiskSignature - Supplies a pointer to the existing disk
        signature structure.

    DiskSignaturesListHead - Supplies a pointer to the list of previously
        processed disk signatures

Return Value:

    Boolean value indicating if supplied disk signature is not unique.

--*/

{

    PARC_DISK_SIGNATURE CurrentDiskSignature;
    PLIST_ENTRY ListEntry;

    if ((DiskSignature->Signature == 0) && (DiskSignature->CheckSum == 0)) {
        return TRUE;
    }

    ListEntry = DiskSignaturesListHead->Flink;
    while (ListEntry != DiskSignaturesListHead) {
        CurrentDiskSignature = CONTAINING_RECORD(ListEntry,
                                                 ARC_DISK_SIGNATURE,
                                                 ListEntry);

        if (CurrentDiskSignature->CheckSum == DiskSignature->CheckSum) {
            return TRUE;
        }

        ListEntry = ListEntry->Flink;
    }

    return FALSE;
}

NTSTATUS
OslpPersistData (
    __in PMEMORY_DESCRIPTOR MemoryDescriptor,
    __out PBAPD_LIST_ITEM *PdDescription
    )

/*++

Routine Description:

    This routine will make available to the Operating System the specified
    memory allocation.

Arguments:

    LoaderBlock - Supplies a memory region to persist.

    PdDescription - Supplies a pointer to a variable that receives the OS
        persistent data list entry item for the memory region.

Return Value:

    STATUS_SUCCESS when successful.

    STATUS_NO_MEMORY if a memory allocation fails.

    STATUS_NOT_FOUND if the data had been previously marked as destroyed.

--*/

{

    ULONGLONG AllocationSize;
    GUID InvalidDataGuid = BAPD_INVALID_DATA_GUID;
    PBAPD_ITEM PdItem;
    PBAPD_LIST_ITEM PdListItem;
    PHYSICAL_ADDRESS PhysicalAddress;
    NTSTATUS Status;
    ULONG_PTR VirtualAddress;

    PdListItem = NULL;
    Status = STATUS_SUCCESS;
    VirtualAddress = 0;

    //
    // Boot library persistent data gets mapped into kernel address space
    // and is marked with a memory descriptor which will keep the mapping
    // around through phase two of initialization.
    //

    AllocationSize = MemoryDescriptor->PageCount << PAGE_SHIFT;
    PhysicalAddress.QuadPart = MemoryDescriptor->BasePage << PAGE_SHIFT;

    __analysis_assume(AllocationSize > 0);
    ASSERT(AllocationSize > 0);
    __analysis_assume(PhysicalAddress.QuadPart > 0);
    ASSERT(PhysicalAddress.QuadPart > 0);

    //
    // Create a virtual mapping from kernel address space if the persistent data
    // is not already mapped into kernel address space.
    //

    if (KSEG_PAGE(MemoryDescriptor->MappedBasePage) == FALSE) {
        Status = BlMmMapPhysicalAddress((PVOID *)(&VirtualAddress),
                                        PhysicalAddress,
                                        AllocationSize,
                                        MEMORY_ATTRIBUTE_ALLOCATION_KSEG);

        if (!NT_SUCCESS(Status)) {
            goto OslpPersistDataEnd;
        }

    } else {
        VirtualAddress =
            (ULONG_PTR)MemoryDescriptor->MappedBasePage << PAGE_SHIFT;
    }

    PdItem = (PBAPD_ITEM)VirtualAddress;
    if ((RtlEqualMemory(&PdItem->DataType, &InvalidDataGuid, sizeof(GUID))) &&
        (RtlEqualMemory(&PdItem->Application, &InvalidDataGuid, sizeof(GUID)))) {

        //
        // Fail for data that has been invalidated. Invalidated data is not to
        // be reported to the Kernel via the loader block, however, the page
        // where the data resides still needs to be mapped.
        //

        Status = STATUS_NOT_FOUND;
        goto OslpPersistDataEnd;
    }

    //
    // Allocate and initialize an entry to describe the OS mapping.
    //

    PdListItem = BlMmAllocateHeap(sizeof(BAPD_LIST_ITEM));
    if (PdListItem == NULL) {
        Status = STATUS_NO_MEMORY;
        goto OslpPersistDataEnd;
    }

    RtlZeroMemory(PdListItem, sizeof(BAPD_LIST_ITEM));
    PdListItem->ItemDesc.u.VirtualAddr = PdItem;
    PdListItem->ItemDesc.Length = PdItem->DataOffset + PdItem->DataSize;
    *PdDescription = PdListItem;

OslpPersistDataEnd:
    if ((!NT_SUCCESS(Status)) && (PdListItem != NULL)) {
        BlMmFreeHeap(PdListItem);
    }

    return Status;
}

ULONG
OslpSplitKernelHalMemoryDescriptor (
    __in PMEMORY_DESCRIPTOR LibraryDescriptor,
    __inout_ecount(4) PMEMORY_ALLOCATION_DESCRIPTOR KernelDescriptor,
    __in PKLDR_DATA_TABLE_ENTRY KernelEntry,
    __in PKLDR_DATA_TABLE_ENTRY HalEntry
    )

/*++

Routine Description:

    This routine splits the kernel memory descriptor that describes the
    combined region of kernel and HAL images into up to four kernel memory
    descriptors: unallocated region before kernel/HAL, kernel, HAL, and
    unallocated region after kernel/HAL. It's possible that there is no
    unallocated region before or/and after kernel/HAL.

Arguments:

    LibraryDescriptor - Supplies a pointer to the Boot Environment Library
        memory descriptor describing combined region of kernel and HAL images.

    KernelDescriptor - Supplies a pointer to an array of four kernel memory
        descriptors. The first descriptor was initialized and was converted
        from the above Boot Environment Library memory descriptor. This
        descriptor will be split into up to four descriptors which will be
        written to the array. (In other words, the first descriptor will be
        overwritten.)

    KernelEntry - Supplies a pointer to the kernel image data table entry.

    HalEntry - Supplies a pointer to the HAL image data table entry.

Return Value:

    Number of split kernel memory descriptors.

--*/

{

    ULONG_PTR BasePage[5];
    ULONG_PTR HalBasePage;
    ULONG Index;
    ULONG_PTR KernelBasePage;
    ULONG SplitCount;

    //
    // This memory descriptor should contain both kernel and HAL images.
    // Two images must be at different base address.
    //

    ASSERT(IS_ADDR_CONTAINED_IN_MD(LibraryDescriptor, KernelEntry->DllBase));
    ASSERT(IS_ADDR_CONTAINED_IN_MD(LibraryDescriptor, HalEntry->DllBase));
    ASSERT(KernelEntry->DllBase != HalEntry->DllBase);

    //
    // Determine the base page for kernel and HAL.
    //

    KernelBasePage = KernelDescriptor->BasePage +
                     PAGE_OFFSET(LibraryDescriptor, KernelEntry->DllBase);

    HalBasePage = KernelDescriptor->BasePage +
                  PAGE_OFFSET(LibraryDescriptor, HalEntry->DllBase);

    //
    // Determine the base page and page count for the 4 descriptors.
    // Use BasePage[4] as the end mark.
    //

    BasePage[0] = KernelDescriptor->BasePage;
    if ((ULONG_PTR)KernelEntry->DllBase < (ULONG_PTR)HalEntry->DllBase) {
        BasePage[1] = KernelBasePage;
        BasePage[2] = HalBasePage;
        BasePage[3] = HalBasePage + (HalEntry->SizeOfImage >> PAGE_SHIFT);
        ASSERT(HalBasePage - KernelBasePage ==
               KernelEntry->SizeOfImage >> PAGE_SHIFT);

    } else {
        BasePage[1] = HalBasePage;
        BasePage[2] = KernelBasePage;
        BasePage[3] = KernelBasePage + (KernelEntry->SizeOfImage >> PAGE_SHIFT);
        ASSERT(KernelBasePage - HalBasePage ==
               HalEntry->SizeOfImage >> PAGE_SHIFT);
    }

    BasePage[4] = KernelDescriptor->BasePage + KernelDescriptor->PageCount;

    //
    // Calculate the descriptor for each region.
    //

    SplitCount = 0;
    for (Index = 0; Index < 4; Index += 1) {

        //
        // Skip the descriptor if its page count is zero.
        //

        if (BasePage[Index + 1] == BasePage[Index]) {
            continue;
        }

        //
        // The second and third descriptors are for kernel/HAL, the
        // first and forth are unallocated.
        //

        if ((Index == 1) || (Index == 2)) {
            KernelDescriptor->MemoryType = LoaderSystemCode;

        } else {
            KernelDescriptor->MemoryType = LoaderFirmwareTemporary;
        }

        KernelDescriptor->BasePage = BasePage[Index];
        KernelDescriptor->PageCount = BasePage[Index + 1] - BasePage[Index];
        KernelDescriptor += 1;
        SplitCount += 1;
    }

    return SplitCount;
}

ULONG
OslpSplitSinglePageMemoryDescriptor (
    __in PMEMORY_DESCRIPTOR LibraryDescriptor,
    __inout_ecount(3) PMEMORY_ALLOCATION_DESCRIPTOR KernelDescriptor,
    __in ULONGLONG SinglePage,
    __in MEMORY_TYPE SinglePageType
    )

/*++

Routine Description:

    This routine splits a kernel memory descriptor that that is free/contains
    boot service code/data to mark one page with a special type.

Arguments:

    LibraryDescriptor - Supplies a pointer to the Boot Environment Library
        memory descriptor that is free or contains boot service code/data.

    KernelDescriptor - Supplies a pointer to an array of two kernel memory
        descriptors. The first descriptor was initialized and was converted
        from the above Boot Environment Library memory descriptor. This
        descriptor will be split into up to two descriptors which will be
        written to the array. (In other words, the first descriptor will be
        overwritten.)

    SinglePage - Supplies the page index of the single split page.

    SinglePageType - Supplies the memory to be assigned to the split page.

Return Value:

    Number of split kernel memory descriptors.

--*/

{
    ULONG_PTR BasePage[4];
    ULONG Index;
    MEMORY_TYPE MemoryType;
    ULONG SplitCount;

    UNREFERENCED_PARAMETER(LibraryDescriptor);

    if (KernelDescriptor->PageCount == 1) {

        //
        // If the descriptor is already only a single page, just set the
        // memory type.
        //

        KernelDescriptor->MemoryType = SinglePageType;
        return 1;
    }

    BasePage[0] = KernelDescriptor->BasePage;
    BasePage[1] = (ULONG_PTR)SinglePage;
    BasePage[2] = (ULONG_PTR)SinglePage + 1;
    BasePage[3] = KernelDescriptor->BasePage + KernelDescriptor->PageCount;

    //
    // Store the memory type of the original descriptor.
    //

    MemoryType = KernelDescriptor->MemoryType;

    //
    // Calculate the descriptor for each region.
    //

    SplitCount = 0;
    for (Index = 0; Index < 3; Index += 1) {

        //
        // Skip the descriptor if its page count is zero.
        //

        if (BasePage[Index + 1] == BasePage[Index]) {
            continue;
        }

        //
        // The second descriptor is the single page descriptor.
        //

        if (Index == 1) {
            KernelDescriptor->MemoryType = SinglePageType;

        } else {
            KernelDescriptor->MemoryType = MemoryType;
        }

        KernelDescriptor->BasePage = BasePage[Index];
        KernelDescriptor->PageCount = BasePage[Index + 1] - BasePage[Index];
        KernelDescriptor += 1;
        SplitCount += 1;
    }

    return SplitCount;
}

NTSTATUS
OslpStampSignature (
    __in PBOOT_ENVIRONMENT_DEVICE Device,
    __in PARC_DISK_SIGNATURE DiskSignature,
    __in PLIST_ENTRY DiskSignaturesListHead
    )

/*++

Routine Description:

    This routine stamps a given drive with a unique signature.
    It traverses the list of disk signatures to ensure that it
    stamps a signature that is not already present in the
    disk list.  Then it writes the new disk signature to the
    disk and recomputes the checksum.  Note that if the original
    signature is unique across all disks, then the signature is
    left unmodified.

Arguments:

    Device - Supplies a pointer to a structure describing the device.

    DiskSignature - Supplies a pointer to the existing disk
        signature structure.

    DiskSignaturesListHead - Supplies a pointer to the list of previously
        processed disk signatures

Return Value:

    NTSTATUS indicating the success or failure of the operation.

--*/

{

    PBLOCK_IO_IDENTIFIER BlockIoIdentifier;
    PULONG Buffer;
    ULONG BufferSize;
    ULONG CurrentByte;
    DEVICE_INFORMATION DeviceInformation;
    ULONG NewSignature;
    BOOLEAN StampDisk;
    NTSTATUS Status;
    ULONG Sum;
    DEVICE_ID WriteableId;

    Buffer = NULL;
    BufferSize = 0;
    WriteableId = INVALID_DEVICE_ID;
    Status = STATUS_SUCCESS;

    //
    // Find out if this is a hard drive otherwise stamping will not work.
    //

    BlockIoIdentifier = &Device->u.BlockIo;
    if (BlockIoIdentifier->BlockIoType != BLOCK_IO_HARD_DISK) {
        Status = STATUS_SUCCESS;
        goto OslpStampSignatureExit;
    }

    //
    // Generate a relatively unique seed to start off with.
    //

    NewSignature = BlTimeGetRelativeTime();
    NewSignature = (NewSignature & 0xFFFF) << 16;
    NewSignature += BlTimeGetRelativeTime();

    //
    // Scan through the list to check if the current checksum is unique.
    //

    StampDisk = FALSE;

Rescan:

    //
    // If a duplicate checksum is found, stamp the disk with a new
    // signature.  If this is the first time through the loop, read the
    // sector off disk.  Otherwise, increment the signature until the
    // checksum is unique.
    //

    if (OslpNewSignatureNeeded(DiskSignature,
                               DiskSignaturesListHead) != FALSE) {

        if (StampDisk == FALSE) {
            StampDisk = TRUE;
            Status = BlDeviceOpen(Device,
                                  OPEN_READ | OPEN_WRITE,
                                  &WriteableId);

            if (!NT_SUCCESS(Status)) {
                goto OslpStampSignatureExit;
            }

            Status = BlDeviceGetInformation(WriteableId, &DeviceInformation);
            if (!NT_SUCCESS(Status)) {
                goto OslpStampSignatureExit;
            }

            //
            // Ensure that the buffer will not be overrun if the signature
            // will be stamped.
            //

            BufferSize = DeviceInformation.u.BlockIo.BlockSize;
            if (BufferSize < ((PARTITION_TABLE_OFFSET * 2) + sizeof(ULONG))) {
                Status = STATUS_INVALID_PARAMETER;
                goto OslpStampSignatureExit;
            }

            //
            // Read the boot sector into memory.
            //

            Buffer = BlMmAllocateHeap(BufferSize);
            if (Buffer == NULL) {
                Status = STATUS_NO_MEMORY;
                goto OslpStampSignatureExit;
            }

            Status = BlDeviceReadAtOffset(WriteableId,
                                          0,
                                          BufferSize,
                                          Buffer,
                                          NULL);

            if (!NT_SUCCESS(Status)) {
                goto OslpStampSignatureExit;
            }

        } else {
                NewSignature += 1;
        }

        ASSERT((Buffer != NULL) && (BufferSize != 0));

        //
        // Apply the new signature back to the boot sector.
        //

        ((ULONG UNALIGNED *)Buffer)[PARTITION_TABLE_OFFSET/2-1] = NewSignature;

        //
        // Recompute the checksum by adding each sector 4 bytes at a time.
        //

        DiskSignature->Signature = NewSignature;
        for (CurrentByte = 0, Sum = 0;
             CurrentByte < (BufferSize / 4);
             CurrentByte += 1) {

            Sum += Buffer[CurrentByte];
        }

        DiskSignature->CheckSum = ~Sum + 1;
        goto Rescan;
    }

    if (StampDisk != FALSE) {

        ASSERT(WriteableId != INVALID_DEVICE_ID);

        Status = BlDeviceWriteAtOffset(WriteableId,
                                       0,
                                       BufferSize,
                                       Buffer,
                                       NULL);
    }

OslpStampSignatureExit:
    if (WriteableId != INVALID_DEVICE_ID) {
        BlDeviceClose(WriteableId);
    }

    if (Buffer != NULL) {
        BlMmFreeHeap(Buffer);
    }

    return Status;
}

NTSTATUS
OslpCheckOsRamdiskDeviceDescriptor (
    __in PMEMORY_DESCRIPTOR Descriptor
    )

/*++

Routine Description:

    This routine checks whether the given descriptor describes ramdisk device
    containing the OS being booted into.

Arguments:

    Descriptor - Supplies the memory descriptor to be checked.

Return Value:

    STATUS_SUCCESS if the descriptor describes the OS ramdisk device.
    STATUS_NO_MATCH if the descriptor does not describe the OS device.
    STATUS_INVALID_DEVICE_REQUEST if the OS is not booting out of a ramdisk.

--*/

{

    PRAM_DISK_IDENTIFIER RamdiskIdentifier;
    LARGE_INTEGER StartAddress;
    NTSTATUS Status;

    //
    // For a successful match, the descriptor should describe the physical
    // address where the OS ramdisk device is loaded.
    //

    Status = STATUS_INVALID_DEVICE_REQUEST;
    if ((OslLoadDevice->DeviceType == DEVICE_BLOCK_IO) &&
        (OslLoadDevice->u.BlockIo.BlockIoType == BLOCK_IO_RAMDISK)) {

        RamdiskIdentifier = &OslLoadDevice->u.BlockIo.u.RamDisk;
        StartAddress.QuadPart = Descriptor->BasePage << PAGE_SHIFT;
        if (RamdiskIdentifier->ImageBase.QuadPart == StartAddress.QuadPart) {
            Status = STATUS_SUCCESS;

        } else {
            Status = STATUS_NO_MATCH;
        }
    }

    return Status;
}

BOOLEAN
OslpGetSmBiosTableString (
    __in PSMBIOS_STRUCT_HEADER TableBase,
    __in ULONG TableLength,
    __in UCHAR StringIndex,
    __out PSTR* BiosString,
    __out PULONG BiosStringLength
    )

/*++

Routine Description:

    This routine iterates through the string table of the given table to locate
    a valid string of non-zero length with the given index.

Arguments:

    TableBase - Supplies a pointer to the base of an SMBIOS table.

    TableLength - Supplies the length of the SMBIOS table including its strings.

    StringIndex - Supplies the index of the string in the string table.

    BiosString - Supplies a pointer that receives a pointer to the string.

    BiosStringLength - Supplies a pointer that receives the length of the string
        excluding the NULL-terminator.

Return Value:

    TRUE - If for the given index a valid string of non-zero length was located.

--*/

{

    PSTR CurrentString;
    ULONG Index;
    ULONG RemainingStringTableLength;
    size_t StringLength;

    //
    // Length of Table + Strings must be more than length of the fixed part of
    // the table.
    //

    if (TableLength <= TableBase->Length) {
        return FALSE;
    }

    //
    // Calculate length of the string table and point to the first string.
    //

    RemainingStringTableLength = TableLength - TableBase->Length;
    CurrentString = (PSTR)TableBase + TableBase->Length;

    //
    // N.B. SMBIOS string indexes are 1-based.
    //

    Index = 1;
    while (NT_SUCCESS(RtlStringCchLengthA(CurrentString,
                                          RemainingStringTableLength,
                                          &StringLength))) {

        if (Index == StringIndex) {
            if (StringLength != 0) {
                *BiosString = CurrentString;
                *BiosStringLength = (ULONG)StringLength;
                return TRUE;
            } else {
                return FALSE;
            }
        }

        StringLength += sizeof(CHAR);
        CurrentString += StringLength;
        RemainingStringTableLength -= (ULONG)StringLength;
        Index += 1;
    }

    return FALSE;
}

NTSTATUS
OslpGetNextSmBiosTable (
    __inout PULONG StructureTableRemainingLength,
    __in PSMBIOS_STRUCT_HEADER CurrentTable,
    __out PULONG CurrentTableLength,
    __out PSMBIOS_STRUCT_HEADER* NextTable
    )

/*++

Routine Description:

    This routine is meant to be called in a loop to iterate through SMBIOS
    tables. SMBIOS tables are present in a serialized memory blob, where each
    type of table starts with a common fixed size header. Header contains the
    length of the particular table, but this length does not account for the
    variable number of strings that follow each table. Traverse the strings to
    find the actual length of the table:

        Table->Length + Sum of the length of all strings of this table.

    End of a table is marked by an extra NULL terminator after the last string.

Arguments:

    StructureTableRemainingLength - Supplies a pointer to the combined length
        of all the tables not seen so far. On output, gets updated with the
        length remaining after accounting for the current table.

    CurrentTable - Supplies a pointer to what is potentially a table, but needs
        to be validated for length.

    CurrentTableLength - Supplies a pointer that receives the length of the
        current table after accounting for all the variable length strings that
        follow the fixed size table structure.

    NextTable - Supplies a pointer that receives a pointer to what is potentially
        a table. This pointer should be passed as CurrentTable in the next call
        to this function.

Return Value:

    TRUE - If CurrentTable points to a valid table.

    FALSE - If there are no more tables, or one of the bounds checks failed.

--*/

{

    PUCHAR CurrentPtr;
    PUCHAR EndPtr;

    if (*StructureTableRemainingLength < sizeof(SMBIOS_STRUCT_HEADER)) {
        return STATUS_UNSUCCESSFUL;
    }

    if (*StructureTableRemainingLength < CurrentTable->Length) {
        return STATUS_UNSUCCESSFUL;
    }

    CurrentPtr = (PUCHAR)CurrentTable + CurrentTable->Length;
    EndPtr = (PUCHAR)CurrentTable + *StructureTableRemainingLength;

    //
    // End of the table is marked by a double NULL-terminated ANSI string.
    // Do not traverse beyond last two bytes of the table in order to be
    // able to dereference a USHORT and compare to 0.
    //

    while (CurrentPtr <= EndPtr - sizeof(USHORT)) {

        //
        // Check for a double NULL-terminator.
        //

        if (*((USHORT UNALIGNED*)CurrentPtr) == 0) {
            CurrentPtr += sizeof(USHORT);
            *CurrentTableLength = (ULONG)(CurrentPtr - (PUCHAR)CurrentTable);
            *StructureTableRemainingLength -= *CurrentTableLength;
            *NextTable = (PSMBIOS_STRUCT_HEADER)CurrentPtr;
            return STATUS_SUCCESS;
        }

        CurrentPtr += 1;
    }

    return STATUS_UNSUCCESSFUL;
}

BOOLEAN
OslpAddSmBiosStringToHash (
    __in PSMBIOS_STRUCT_HEADER TableBase,
    __in ULONG TableLength,
    __in UCHAR StringIndex,
    __inout PSYMCRYPT_SHA1_STATE ShaContext
    )

/*++

Routine Description:

    This routine adds an SMBIOS string to the hash, given an SMBIOS table and
    an index into the string table.

Arguments:

    TableBase - Supplies a pointer to the base of an SMBIOS table.

    TableLength - Supplies the length of the SMBIOS table including its strings.

    StringIndex - Supplies the index of the string in the string table.

    ShaContext - Supplies a pointer to the context used for hashing.

Return Value:

    TRUE - If a valid string of non-zero length was found for the given index
        and added to the hash.

--*/

{

    PSTR BiosString;
    ULONG BiosStringLength;

    if (OslpGetSmBiosTableString(TableBase,
                                 TableLength,
                                 StringIndex,
                                 &BiosString,
                                 &BiosStringLength)) {

        SymCryptSha1Append(ShaContext, (PUCHAR)BiosString, BiosStringLength);
        return TRUE;
    }

    return FALSE;
}

NTSTATUS
OslpHashSmBiosStrings (
    __in_opt PSMBIOS3_EPS_HEADER SmBiosEps,
    __inout PSYMCRYPT_SHA1_STATE ShaContext
    )

/*++

Routine Description:

    This routine iterates through SMBIOS tables and adds certain known strings
    to the hash. The chosen strings are unlikely to change with firmware update
    or hardware changes.

Arguments:

    SmBiosEps - Supplies a pointer to a copy of the system management BIOS3
        entry point structure if the system supplies one.

    ShaContext - Supplies a pointer to the context used for hashing.

Return Value:

    STATUS_SUCCESS - If atleast some data was added to the hash.

--*/

{

    PSMBIOS_STRUCT_HEADER CurrentTable;
    ULONG CurrentTableLength;
    BOOLEAN HashedSomeData;
    PSMBIOS_STRUCT_HEADER NextTable;
    NTSTATUS Status;
    PUCHAR StructureTable;
    PHYSICAL_ADDRESS StructureTableAddress;
    ULONG StructureTableLength;
    ULONG StructureTableRemainingLength;

    Status = STATUS_UNSUCCESSFUL;
    if (SmBiosEps == NULL) {
        return Status;
    }

    StructureTable = NULL;
    StructureTableAddress.QuadPart = SmBiosEps->StructureTableAddress;
    StructureTableLength = SmBiosEps->StructureTableMaximumSize;
    Status = BlMmMapPhysicalAddress(&StructureTable,
                                    StructureTableAddress,
                                    StructureTableLength,
                                    MEMORY_ATTRIBUTE_NONE);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Iterate through each table, and hash the fields and strings that are
    // unlikely to change due to firmware update, hardware change etc.
    //

    HashedSomeData = FALSE;
    CurrentTable = (PSMBIOS_STRUCT_HEADER)StructureTable;
    StructureTableRemainingLength = StructureTableLength;
    while (NT_SUCCESS(OslpGetNextSmBiosTable(&StructureTableRemainingLength,
                                             CurrentTable,
                                             &CurrentTableLength,
                                             &NextTable))) {

        switch (CurrentTable->Type) {

        case SMBIOS_BIOS_INFORMATION_TYPE:
        {
            PSMBIOS_BIOS_INFORMATION_STRUCT BiosTable =
                (PSMBIOS_BIOS_INFORMATION_STRUCT)CurrentTable;

            HashedSomeData |= OslpAddSmBiosStringToHash(CurrentTable,
                                                        CurrentTableLength,
                                                        BiosTable->Vendor,
                                                        ShaContext);

            break;
        }
        case SMBIOS_SYSTEM_INFORMATION:
        {
            PSMBIOS_SYSTEM_INFORMATION_STRUCT SystemTable =
                (PSMBIOS_SYSTEM_INFORMATION_STRUCT)CurrentTable;

            HashedSomeData |= OslpAddSmBiosStringToHash(CurrentTable,
                                                        CurrentTableLength,
                                                        SystemTable->Manufacturer,
                                                        ShaContext);

            HashedSomeData |= OslpAddSmBiosStringToHash(CurrentTable,
                                                        CurrentTableLength,
                                                        SystemTable->ProductName,
                                                        ShaContext);

            HashedSomeData |= OslpAddSmBiosStringToHash(CurrentTable,
                                                        CurrentTableLength,
                                                        SystemTable->SerialNumber,
                                                        ShaContext);

            HashedSomeData |= OslpAddSmBiosStringToHash(CurrentTable,
                                                        CurrentTableLength,
                                                        SystemTable->SKUNumber,
                                                        ShaContext);

            HashedSomeData |= OslpAddSmBiosStringToHash(CurrentTable,
                                                        CurrentTableLength,
                                                        SystemTable->Family,
                                                        ShaContext);

            break;
        }
        case SMBIOS_BASE_BOARD_INFORMATION_TYPE:
        {
            PSMBIOS_BASE_BOARD_INFORMATION_STRUCT BaseBoardTable =
                (PSMBIOS_BASE_BOARD_INFORMATION_STRUCT)CurrentTable;

            HashedSomeData |= OslpAddSmBiosStringToHash(CurrentTable,
                                                        CurrentTableLength,
                                                        BaseBoardTable->Manufacturer,
                                                        ShaContext);

            HashedSomeData |= OslpAddSmBiosStringToHash(CurrentTable,
                                                        CurrentTableLength,
                                                        BaseBoardTable->Product,
                                                        ShaContext);

            HashedSomeData |= OslpAddSmBiosStringToHash(CurrentTable,
                                                        CurrentTableLength,
                                                        BaseBoardTable->SerialNumber,
                                                        ShaContext);

            break;
        }
        case SMBIOS_SYSTEM_CHASIS_INFORMATION_TYPE:
        {
            PSMBIOS_SYSTEM_CHASIS_INFORMATION_STRUCT ChasisTable =
                (PSMBIOS_SYSTEM_CHASIS_INFORMATION_STRUCT)CurrentTable;

            HashedSomeData |= OslpAddSmBiosStringToHash(CurrentTable,
                                                        CurrentTableLength,
                                                        ChasisTable->Manufacturer,
                                                        ShaContext);

            HashedSomeData |= OslpAddSmBiosStringToHash(CurrentTable,
                                                        CurrentTableLength,
                                                        ChasisTable->SerialNumber,
                                                        ShaContext);

            break;
        }
        case SMBIOS_PROCESSOR_INFORMATION_TYPE:
        {
            PSMBIOS_PROCESSOR_INFORMATION_STRUCT ProcessorTable =
                (PSMBIOS_PROCESSOR_INFORMATION_STRUCT)CurrentTable;

            HashedSomeData |= OslpAddSmBiosStringToHash(CurrentTable,
                                                        CurrentTableLength,
                                                        ProcessorTable->SocketDesignation,
                                                        ShaContext);

            HashedSomeData |= OslpAddSmBiosStringToHash(CurrentTable,
                                                        CurrentTableLength,
                                                        ProcessorTable->ProcessorManufacturer,
                                                        ShaContext);

            HashedSomeData |= OslpAddSmBiosStringToHash(CurrentTable,
                                                        CurrentTableLength,
                                                        ProcessorTable->ProcessorVersion,
                                                        ShaContext);

            break;
        }
        case SMBIOS_SYSTEM_POWER_SUPPLY_TYPE:
        case SMBIOS_PORTABLE_BATTERY_TYPE:
        case SMBIOS_MEMORY_DEVICE_TYPE:
        case SMBIOS_SYSTEM_EVENTLOG:
        default:
            break;
        }

        CurrentTable = NextTable;
    }

    if (StructureTable != NULL) {
        BlMmUnmapVirtualAddress(StructureTable, StructureTableLength);
    }

    if (HashedSomeData != FALSE) {
        Status = STATUS_SUCCESS;
    }

    return Status;
}

NTSTATUS
OslpGenerateHardwareConfigurationId (
    __in_opt PSMBIOS3_EPS_HEADER SmBiosEps,
    __out PGUID HardwareConfigurationId
    )

/*++

Routine Description:

    This routine returns a HardwareConfigurationId in the form of a GUID that
    is a pseudo-unique Id generated from hash of various hardware information.

Arguments:

    SmBiosEps - Supplies a pointer to a copy of the system management BIOS3
        entry point structure if the system supplies one.

    HardwareConfigurationId - Supplies a pointer to a buffer that receives the
        calculated hash in the form of a Guid.

Return Value:

    STATUS_SUCCESS - If hash was successfully generated.

    STATUS_UNSUCCESSFUL - If hash was not generated. HardwareConfigurationId
        will be untouched.

--*/

{

    SYMCRYPT_SHA1_STATE ShaContext;
    BYTE ShaHash[SYMCRYPT_SHA1_RESULT_SIZE];
    NTSTATUS Status;

    SymCryptSha1Init(&ShaContext);
    Status = OslpHashSmBiosStrings(SmBiosEps, &ShaContext);
    SymCryptSha1Result(&ShaContext, ShaHash);

    //
    // N.B. Hash value is 20 bytes long. Since we only need 16, copying the
    //      first 16 bytes.
    //

    if (NT_SUCCESS(Status)) {
        RtlCopyMemory(HardwareConfigurationId, ShaHash, sizeof(GUID));
    }

    return Status;
}

VOID
OslpGetHardwareConfigurationId (
    __in_opt PSMBIOS3_EPS_HEADER SmBiosEps,
    __out PGUID HardwareConfigurationId
    )

/*++

Routine Description:

    This routine returns a hardware Id in the form of a GUID that is the SMBIOS
    UUID if present, or a pseudo-unique Id generated from hash of SMBIOS strings.

Arguments:

    SmBiosEps - Supplies a pointer to a copy of the system management BIOS3
        entry point structure if the system supplies one.

    HardwareConfigurationId - Supplies a pointer to a buffer that receives the
        calculated Id.

Return Value:

    None. HardwareConfigurationId will be set to NULL GUID on failure.

--*/

{

    NTSTATUS Status;

    //
    // Prefer SMBIOS UUID. According to the SMBIOS spec, a system UUID of
    // {00000000-0000-0000-0000-000000000000} means that no system UUID is set,
    // {FFFFFFFF-FFFF-FFFF-FFFF-FFFFFFFFFFFF} means that one is not set right
    // now but could be manually set through the BIOS or other means.
    //

    Status = OslpLoadSystemGuid(SmBiosEps, HardwareConfigurationId);
    if (NT_SUCCESS(Status) &&
        (RtlCompareMemory(HardwareConfigurationId, &OSL_NULL_GUID, sizeof(GUID))
         != sizeof(GUID)) &&
        (RtlCompareMemory(HardwareConfigurationId, &OSL_INVALID_GUID, sizeof(GUID))
         != sizeof(GUID))) {

        return;
    }

    Status = OslpGenerateHardwareConfigurationId(SmBiosEps,
                                                 HardwareConfigurationId);

    if (!NT_SUCCESS(Status)) {
        BlStatusPrint(L"Failed to generate HardwareConfigurationId. An Id of "
                      L"NULL GUID will be used. Error code: 0x%X\n",
                      Status);

        RtlCopyMemory(HardwareConfigurationId, &OSL_NULL_GUID, sizeof(GUID));
    }

    return;
}

VOID
OslpGetWindowsBuildTime (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __out PTIME_FIELDS TimeFields
    )

/*++

Routine Description:

    This routine reads Windows Build time by looking at ntos's PE header
    time stamp.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

    TimeFields - Supplies a pointer that receives Windows (NTOS) build time.

Return Value:

    None.

--*/

{

    PKLDR_DATA_TABLE_ENTRY KernelEntry;
    PIMAGE_NT_HEADERS NtHeader;
    LARGE_INTEGER WindowsBuildTime;

    KernelEntry = BlLdrFindDataTableEntry(&LoaderBlock->LoadOrderListHead,
                                          KERNEL_DEFAULT_NAME,
                                          TRUE);

    //
    // There must be a kernel entry.
    //

    ASSERT (KernelEntry != NULL);

    NT_VERIFY(NT_SUCCESS(BlImgGetNtHeader(KernelEntry->DllBase,
                                          KernelEntry->SizeOfImage,
                                          &NtHeader)));

    RtlSecondsSince1970ToTime(NtHeader->FileHeader.TimeDateStamp,
                              &WindowsBuildTime);

    RtlTimeToTimeFields(&WindowsBuildTime, TimeFields);
    return;
}

NTSTATUS
OslpGetLastReferenceTime (
    __in PBSD_POWER_TRANSITION PowerTransition,
    __out PTIME_FIELDS TimeFields
    )

/*++

Routine Description:

    This routine reads LastReferenceTime from boot status log file.

Arguments:

    PowerTransition - Supplies a pointer to the power transition data from the
        previous boot.

    TimeFields - Supplies a pointer that receives last shutdown \
        hibernate \ sleep time.

Return Value:

    None.

--*/

{

    ULONG CheckSum;
    NTSTATUS Status;

    if (PowerTransition == NULL) {
        Status = STATUS_UNSUCCESSFUL;
        goto GetLastReferenceTimeEnd;
    }

    //
    // Make sure the read is not corrupted by calculating the checksum.
    //

    CheckSum = RtlComputeCrc32(0,
                               &PowerTransition->LastReferenceTime,
                               sizeof(PowerTransition->LastReferenceTime));

    if (PowerTransition->LastReferenceTimeCheckSum != CheckSum) {
        Status = STATUS_UNSUCCESSFUL;
        goto GetLastReferenceTimeEnd;
    }

    RtlTimeToTimeFields(&PowerTransition->LastReferenceTime, TimeFields);

    Status = STATUS_SUCCESS;

GetLastReferenceTimeEnd:
    return Status;
}

VOID
OslSetSystemTimeParameters (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in PBSD_POWER_TRANSITION PowerTransition
    )

/*++

Routine Description:

    This routine retrieves the system time from the RTC and stores it in the
    loader extension. If an RTC is not present, it will attempt to fall back
    to the last shutdown time if available, otherwise, it will fall back to
    the build time. The routine will also snap the cycle counter at which
    the system time was read.

Arguments:

    LoaderBlock - Supplies a pointer to the loader parameter block.

    PowerTransition - Supplies a pointer to the power transition data from the
        previous boot.

Return Value:

    None.

--*/

{

    LARGE_INTEGER BuildTime = {0};
    PLOADER_PARAMETER_EXTENSION Extension;
    LARGE_INTEGER HighestRtcUtcTime;
    LARGE_INTEGER ReferenceTime;
    LARGE_INTEGER RtcTime;
    BOOLEAN RtcTimeAvailable;
    NTSTATUS Status;
    TIME_FIELDS TimeFields;
    BOOLEAN TimeValid;
    BOOLEAN UtcTime;

    Extension = LoaderBlock->Extension;
    Extension->TimeStampAtSystemTimeRead = ReadTimeStampCounter();
    HighestRtcUtcTime.QuadPart = 0;
    RtcTimeAvailable = FALSE;

    //
    // First attempt to get the time from the RTC.
    //

    Status = BlGetTime(&TimeFields, &UtcTime);
    TimeValid = RtlTimeFieldsToTime(&TimeFields, &RtcTime);
    if ((NT_SUCCESS(Status)) && (TimeValid != FALSE)) {
        Extension->SystemTime = RtcTime;
        RtcTimeAvailable = TRUE;

        //
        // In order to compare the RTC to other timestamps, the RTC time must
        // be in UTC format. If it is in local time, then adjust for the worst
        // case UTC time which is 26 hours ahead.
        //

        if (UtcTime != FALSE) {
            HighestRtcUtcTime = RtcTime;

        } else {
            HighestRtcUtcTime.QuadPart = RtcTime.QuadPart + 936000000000ULL;
        }
    }

    //
    // The RTC time cannot always be trusted (bad firmware, dead battery, etc.)
    // attempt to get the time from the last shutdown time.
    //

    Status = OslpGetLastReferenceTime(PowerTransition, &TimeFields);
    TimeValid = RtlTimeFieldsToTime(&TimeFields, &ReferenceTime);
    if ((NT_SUCCESS(Status)) && (TimeValid != FALSE)) {

        //
        // There is a valid shutdown time, use it if it is better than the RTC
        // time or if we have no RTC time at all.
        //

        if ((RtcTimeAvailable == FALSE) ||
            (ReferenceTime.QuadPart > HighestRtcUtcTime.QuadPart)) {

            RtcTimeAvailable = FALSE;
            Extension->SystemTime = ReferenceTime;
        }

        goto SetSystemTimeEnd;
    }

    //
    // Fallback again to attempt to get the time from the build. This call
    // cannot fail.
    //

    OslpGetWindowsBuildTime(LoaderBlock, &TimeFields);
    TimeValid = RtlTimeFieldsToTime(&TimeFields, &BuildTime);

    ASSERT(TimeValid != FALSE);

    //
    // At this point, there is no shutdown down time that can be used. Validate
    // the RTC if it was read.
    //

    if ((RtcTimeAvailable == FALSE) ||
        (BuildTime.QuadPart > HighestRtcUtcTime.QuadPart)) {

        RtcTimeAvailable = FALSE;
        Extension->SystemTime = BuildTime;
    }

SetSystemTimeEnd:

    //
    // Set the boot flags and return. If there is no RTC time, then the time
    // must be in UTC format.
    //

    if (RtcTimeAvailable != FALSE) {
        Extension->InternalBootFlags |= INTERNAL_BOOT_FLAGS_RTC_BOOT_TIME;

    } else {
        UtcTime = TRUE;
    }

    if (UtcTime != FALSE) {
        Extension->InternalBootFlags |= INTERNAL_BOOT_FLAGS_UTC_BOOT_TIME;
    }

    return;
}

NTSTATUS
OslpGetAcpiBiosVersion (
    __out PUNICODE_STRING Version
    )

/*++

Routine Description:

    This routine retrieves the BIOS version from the ACPI.

Arguments:

    Version - Supplies the string to which this machine will write
        the BIOS version. If this routine succeeds, it will have
        allocated a buffer for this string and the caller will be responsible
        for freeing that buffer. If this routine fails, it will leave this
        string unchanged.

Return Value:

    NTSTATUS.

--*/

{

    CHAR AnsiCharacter;
    PWCHAR Buffer;
    USHORT CharactersPrintedCount;
    PFADT Fadt;
    USHORT Index;
    USHORT MaximumLength;
    NTSTATUS Status;

    //
    // Allocate a buffer for the version string.
    //

    MaximumLength = ACPI_BIOS_VERSION_BUFFER_SIZE;
    Buffer = BlMmAllocateHeap(MaximumLength);
    if (Buffer == NULL) {
        Status = STATUS_NO_MEMORY;
        goto GetAcpiBiosVersionEnd;
    }

    //
    // Get the ACPI's FADT.
    //

    Status = BlUtlGetAcpiTable(&Fadt, FADT_SIGNATURE);
    if (!NT_SUCCESS(Status)) {
        goto GetAcpiBiosVersionEnd;
    }

    //
    // Copy the OEM ID from the FADT header to the version string's
    // buffer.
    //
    // N.B. The OEMID string in the FADT is not necessarily null-terminated.
    //

    for (Index = 0; Index < ACPI_MAX_OEM_ID; Index += 1) {
        AnsiCharacter = Fadt->Header.OEMID[Index];
        if (AnsiCharacter == ANSI_NULL) {
            break;
        }

        Buffer[Index] = (WCHAR)AnsiCharacter;
    }

    //
    // Print the ID-revision separator and the FADT's OEM revision to the
    // version string's buffer.
    //

    CharactersPrintedCount =
        (USHORT)swprintf_s(&Buffer[Index],
                           (MaximumLength / sizeof(WCHAR)) - Index,
                           ACPI_BIOS_VERSION_SEPARATOR_AND_REVISION_FORMAT,
                           Fadt->Header.OEMRevision);

    if (CharactersPrintedCount == -1) {
        Status = STATUS_UNSUCCESSFUL;
        goto GetAcpiBiosVersionEnd;
    }

    //
    // Write the results of this routine's work to the Version string
    // that the caller passed and set the return status to STATUS_SUCCESS.
    //

    Version->Buffer = Buffer;
    Version->Length = (Index + CharactersPrintedCount) * sizeof(WCHAR);
    Version->MaximumLength = MaximumLength;
    Status = STATUS_SUCCESS;

GetAcpiBiosVersionEnd:
    if ((!NT_SUCCESS(Status)) && (Buffer != NULL)) {
        BlMmFreeHeap(Buffer);
    }

    ASSERT((!NT_SUCCESS(Status)) ||
           (OslIsUnicodeStringNullTerminated(Version) != FALSE));

    return Status;
}

NTSTATUS
OslpGetSmbiosVersion (
    __inout PUNICODE_STRING Version,
    __in PSMBIOS3_EPS_HEADER SmBiosEps
    )

/*++

Routine Description:

    This routine retrieves the BIOS version from the SMBIOS.

Arguments:

    Version - Supplies the string to which this machine will write
        the BIOS version. If this routine succeeds, it will have
        allocated a buffer for this string and the caller will be responsible
        for freeing that buffer. If this routine fails, it will leave this
        string unchanged.

    SmBiosEps - Supplies a pointer to a copy of the system management BIOS3
        entry point structure.

Return Value:

    NTSTATUS.

--*/

{

    PSTR AnsiVersionString;
    ULONG AnsiVersionStringLength;
    PSMBIOS_BIOS_INFORMATION_STRUCT BiosInformation;
    PWCHAR Buffer;
    PSMBIOS_STRUCT_HEADER CurrentTable;
    ULONG CurrentTableLength;
    BOOLEAN FoundVersion;
    USHORT MaximumLength;
    PSMBIOS_STRUCT_HEADER NextTable;
    NTSTATUS Status;
    PUCHAR StructureTable;
    PHYSICAL_ADDRESS StructureTableAddress;
    ULONG StructureTableLength;
    ULONG StructureTableRemainingLength;

    Buffer = NULL;
    StructureTable = NULL;

    //
    // Map the SMBIOS tables into virtual memory.
    //

    StructureTableAddress.QuadPart = SmBiosEps->StructureTableAddress;
    StructureTableLength = SmBiosEps->StructureTableMaximumSize;
    Status = BlMmMapPhysicalAddress(&StructureTable,
                                    StructureTableAddress,
                                    StructureTableLength,
                                    MEMORY_ATTRIBUTE_NONE);

    if (!NT_SUCCESS(Status)) {
        goto GetSmbiosVersionEnd;
    }

    //
    // Search for the BIOS information table.
    //

    CurrentTable = (PSMBIOS_STRUCT_HEADER)StructureTable;
    StructureTableRemainingLength = StructureTableLength;
    for(;;) {

        //
        // Get the current table's length including its strings
        // and get the next table.
        //

        Status = OslpGetNextSmBiosTable(&StructureTableRemainingLength,
                                        CurrentTable,
                                        &CurrentTableLength,
                                        &NextTable);

        if (!NT_SUCCESS(Status)) {
            goto GetSmbiosVersionEnd;
        }

        if (CurrentTable->Type == SMBIOS_BIOS_INFORMATION_TYPE) {

            //
            // The current table is the BIOS information table.
            //

            BiosInformation = (PSMBIOS_BIOS_INFORMATION_STRUCT)CurrentTable;

            //
            // Get the BIOS version string from the table.
            //

            FoundVersion =
                OslpGetSmBiosTableString((PSMBIOS_STRUCT_HEADER)BiosInformation,
                                         CurrentTableLength,
                                         BiosInformation->Version,
                                         &AnsiVersionString,
                                         &AnsiVersionStringLength);

            if (FoundVersion == FALSE) {
                Status = STATUS_UNSUCCESSFUL;
                goto GetSmbiosVersionEnd;
            }

            //
            // N.B. According to DMTF's SMBIOS reference, document number
            //      DSP0134, version 2.7.0,
            //
            //          There is no limit on the length of each individual
            //          text string. However, the length of the entire
            //          structure table (including all strings) must be
            //          reported in the Structure Table Length field of
            //          the SMBIOS Structure Table Entry Point . . . which
            //          is a WORD field limited to 65,535 bytes.
            //

            ASSERT(AnsiVersionStringLength <= MAXUSHORT);

            //
            // Allocate a buffer for the Unicode version of the BIOS
            // version string.
            //

            MaximumLength = (USHORT)AnsiVersionStringLength * sizeof(WCHAR) +
                            sizeof(UNICODE_NULL);

            Buffer = BlMmAllocateHeap(MaximumLength);
            if (Buffer == NULL) {
                Status = STATUS_NO_MEMORY;
                goto GetSmbiosVersionEnd;
            }

            //
            // Convert the version string to Unicode.
            //

            BlCopyStringToWcharString(Buffer, AnsiVersionString);
            Buffer[AnsiVersionStringLength] = UNICODE_NULL;

            //
            // Write the results of this routine's work to the
            // Version string that the caller passed.
            //

            Version->Buffer = Buffer;
            Version->Length = (USHORT)AnsiVersionStringLength * sizeof(WCHAR);
            Version->MaximumLength = MaximumLength;

            ASSERT(Status == STATUS_SUCCESS);

            goto GetSmbiosVersionEnd;
        }

        //
        // Move to the next SMBIOS table.
        //

        CurrentTable = NextTable;
    }

GetSmbiosVersionEnd:
    if (StructureTable != NULL) {
        BlMmUnmapVirtualAddress(StructureTable, StructureTableLength);
    }

    if ((!NT_SUCCESS(Status)) && (Buffer != NULL)) {
        BlMmFreeHeap(Buffer);
    }

    ASSERT((!NT_SUCCESS(Status)) ||
           (OslIsUnicodeStringNullTerminated(Version) != FALSE));

    return Status;
}

NTSTATUS
OslpInitializeManufacturingInformation (
    __in ULONG SystemHiveHandle,
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    )

/*++

Routine Description:

    This routine will initialize all manufacturing information in the loader
    block.

Arguments:

    SystemHiveHandle - Supplies a handle to the system hive.

    LoaderBlock - Supplies a pointer to the loader parameter block.

Return Value:

    NT status code.

--*/

{

    EFI_OS_BOOT_MODE BootMode;
    UNICODE_STRING BootProfile;
    BOOLEAN ManufacturingModeEnabled;
    PWCHAR ManufacturingProfile;
    SIZE_T ManufacturingProfileSize;
    BOOLEAN ResetProtected;
    NTSTATUS Status;

    BootMode = EfiOsBootModeFullOs;
    RtlInitEmptyUnicodeString(&BootProfile, NULL, 0);
    ManufacturingModeEnabled = FALSE;
    ManufacturingProfile = NULL;
    ResetProtected = FALSE;

    RtlInitEmptyUnicodeString(&LoaderBlock->Extension->ManufacturingProfile,
                              NULL,
                              0);

    //
    // First check if the firmware indicates to enable manufacturing mode.
    //

    Status = OslFwGetBootModeInfo(&BootMode,
                                  &BootProfile);

    if ((!NT_SUCCESS(Status)) || (BootMode != EfiOsBootModeManufacturingOs)) {

        //
        // The firmware has no boot mode info or indicates to boot into a mode
        // other than manufacturing mode.  Check what the BCD store indicates
        // for manufacturing mode.
        //

        Status = BlGetApplicationOptionString(
                     BCDE_OSLOADER_TYPE_MANUFACTURING_MODE,
                     &ManufacturingProfile);

        if (!NT_SUCCESS(Status)) {

            //
            // No BCD entry exists.  Check the registry settings.
            //

            if ((!NT_SUCCESS(OslReadManufacturingModeProfileSettings(
                                 SystemHiveHandle,
                                 &ManufacturingModeEnabled,
                                 &ManufacturingProfile))) ||
                (ManufacturingModeEnabled == FALSE)) {

                //
                // No source has indicated that manufacturing mode is enabled.
                // This is not an error.
                //

                Status = STATUS_SUCCESS;
                goto InitializeManfacturingInformationEnd;
            }

            if (ManufacturingProfile == NULL) {

                //
                // If the settings indicate manufacturing mode is enabled, but
                // no default profile is found, return success and do not boot
                // into manufacturing mode.  This type of failure should not be
                // considered a hard failure.
                //

                Status = STATUS_SUCCESS;
                BlStatusPrint(L"Manufacturing mode profile was missing\n");
                goto InitializeManfacturingInformationEnd;
            }
        }
    } else {
        ManufacturingProfile = BootProfile.Buffer;

        //
        // ManufacturingProfile now owns the allocated memory.  Clear
        // BootProfile.Buffer so that memory does not get freed twice.
        //

        BootProfile.Buffer = NULL;
    }

    ASSERT(ManufacturingProfile != NULL);

    ManufacturingProfileSize = (wcslen(ManufacturingProfile) + 1) * sizeof(WCHAR);
    if (ManufacturingProfileSize > MANUFACTURING_PROFILE_MAX_NAME_SIZE) {
        Status = STATUS_NAME_TOO_LONG;
        OslFatalError(OSL_ERROR_MANUFACTURING_MODE,
                      (ULONG_PTR)ManufacturingProfile,
                      Status,
                      0);

        goto InitializeManfacturingInformationEnd;
    }

    if (OslDoesManufacturingProfileExist(SystemHiveHandle,
                                         ManufacturingProfile) == FALSE) {

        //
        // If the manufacturing mode profile does not exist, return success and
        // ignore the specified manufacturing profile and boot normally.  This
        // type of failure should not be considered a hard failure.  There may
        // be other operating systems on the device other than the primary one
        // (e.g. Update OS, recovery OS, etc) that may need to boot and won't have
        // OEM profiles defined.
        //

        Status = STATUS_SUCCESS;
        BlStatusPrint(L"Manufacturing mode profile %s does not exist\n",
                      ManufacturingProfile);

        goto InitializeManfacturingInformationEnd;
    }

    //
    // After the specified profile is validated, make sure there are no security
    // features enabled that should prevent the system from booting in
    // manufacturing mode. Manufacturing mode shouldn't be allowed to circumvent
    // security measures designed to protect user data.
    //

    //
    // Check if a lock screen PIN is enabled.
    //

    if (OslIsDeviceScreenLockEnabled(SystemHiveHandle)) {
        Status = STATUS_CONTENT_BLOCKED;
        OslFatalError(OSL_ERROR_MANUFACTURING_MODE_SECURITY_LOCK_SCREEN,
                      (ULONG_PTR)ManufacturingProfile,
                      Status,
                      0);

        goto InitializeManfacturingInformationEnd;
    }

    //
    // Check if Bitlocker is enabled.
    //

    if (OslManufacturingBitlockerEnabled != FALSE) {

        Status = STATUS_FVE_LOCKED_VOLUME;
        OslFatalError(OSL_ERROR_MANUFACTURING_MODE_SECURITY_BITLOCKER,
                      (ULONG_PTR)ManufacturingProfile,
                      Status,
                      0);

        goto InitializeManfacturingInformationEnd;
    }

    //
    // Check if Anti-theft is enabled.
    //

    Status = BlIsDeviceResetProtected(&ResetProtected);
    if ((NT_SUCCESS(Status)) && (ResetProtected != FALSE)) {
        Status = STATUS_CONTENT_BLOCKED;
        OslFatalError(OSL_ERROR_MANUFACTURING_MODE_SECURITY_RESET_PROTECTION,
                      (ULONG_PTR)ManufacturingProfile,
                      Status,
                      0);

        goto InitializeManfacturingInformationEnd;
    }

    Status = RtlInitUnicodeStringEx(
                 &LoaderBlock->Extension->ManufacturingProfile,
                 ManufacturingProfile);

    if (!NT_SUCCESS(Status)) {
        goto InitializeManfacturingInformationEnd;
    }

    ManufacturingProfile = NULL;

InitializeManfacturingInformationEnd:

    if (ManufacturingProfile != NULL) {
        BlMmFreeHeap(ManufacturingProfile);
    }

    if (BootProfile.Buffer != NULL) {
        BlMmFreeHeap(BootProfile.Buffer);
    }

    return Status;
}



