RY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x07))

#define OSL_MEMORY_TYPE_KERNEL_PAGE_TABLE \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x08))

#define OSL_MEMORY_TYPE_DPC_STACK \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x09))

#define OSL_MEMORY_TYPE_KERNEL_STACK \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x0A))

#define OSL_MEMORY_TYPE_RECLAIMABLE_ALLOCATION \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x0B))

#define OSL_MEMORY_TYPE_ACPI_TABLE \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x0C))

#define OSL_MEMORY_TYPE_HYPERVISOR_ALLOCATION \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x0D))

#define OSL_MEMORY_TYPE_HYPERVISOR_LOADER \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x0E))

#define OSL_MEMORY_TYPE_FIRMWARE_UPDATE \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x0F))

#define OSL_MEMORY_TYPE_PERSISTENT_PAGES \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x10))

#define OSL_MEMORY_TYPE_KD_EXECUTABLE_DATA \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x11))

#define OSL_MEMORY_TYPE_VSM_PAGES \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x12))

#define OSL_MEMORY_TYPE_MEMORY_PARTITION_PAGES \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x13))

//
// Define kernel/HAL address randomization.
//
// OSL_IMAGE_RANDOMNESS - specifies the number of possible starting
//      addresses for kernel/HAL.
//
//      NB: When loading OS in kernel mode, the loader will not try to
//          randomize kernel+hal starting offset, since mapped address
//          of the buffer allocated for these images has already been
//          randomized by the memory manager of the running operating system.
//
// OSL_IMAGE_KERNEL_ALIGNMENT - specifies the alignment of kernel
//      (not HAL) image in unit of pages. Must be non-zero.
//

#if defined(NTKERNEL)
#define OSL_IMAGE_RANDOMNESS                0
#else
#define OSL_IMAGE_RANDOMNESS                32
#endif

// ARM64_64KPAGE_WORKAROUND: hack for 64KB alignment
#if defined(_ARM64_)
#define OSL_IMAGE_KERNEL_ALIGNMENT ALIGN_RANGE_UP(1, _64KB / PAGE_SIZE)
#else
#define OSL_IMAGE_KERNEL_ALIGNMENT          1
#endif

//
// Define the structure used to track loaded firmware drivers.
//

typedef struct _FIRMWARE_LOADED_DRIVER_ENTRY {
    LIST_ENTRY ListEntry;
    HANDLE DriverHandle;
} FIRMWARE_LOADED_DRIVER_ENTRY, *PFIRMWARE_LOADED_DRIVER_ENTRY;

//
// Initial value to set the boot attempt count to on a reset.
//

#define DEFAULT_BOOT_ATTEMPT_COUNT 0

//
// Firmware update settings per ESRT firmware_class GUID:
//
//   HKLM\SYSTEM\CurrentControlSet\Control\FirmwareResources\{<firmware_class>}
//

typedef struct _FIRMWARE_UPDATE_SETTINGS {
    LIST_ENTRY Entry;
    ULONG Version;
    ULONG Phase;
    ULONG Policy;
    NTSTATUS LoadStatus;
    PEFI_SYSTEM_RESOURCE_ENTRY Resource;
    PWSTR Catalog;
    WCHAR Filename[ANYSIZE_ARRAY];
} FIRMWARE_UPDATE_SETTINGS, *PFIRMWARE_UPDATE_SETTINGS;

#define FIRMWARE_UPDATE_PHASE_UPDATE 0
#define FIRMWARE_UPDATE_PHASE_REPORT 1
#define FIRMWARE_UPDATE_PHASE_IGNORE 2

//
// The default policy is to update the firmware when the update version is
// greater than the current version.
//

#define FIRMWARE_UPDATE_POLICY_DEFAULT 0

//
// The capsule flags mask is used to isolate any capsule-specific flags to be
// passed into update capsule.  (Windows is responsible for configuring the
// update capsule flags defined in the UEFI specification.)
//
// N.B. For device firmware updates, allow UEFI to control whether the update
//      gets populated in the system table.
//

#define FIRMWARE_CAPSULE_FLAGS_MASK 0x000000000000FFFF
#define FIRMWARE_CAPSULE_FLAGS_DEVICE_MASK \
    (CAPSULE_FLAGS_POPULATE_SYSTEM_TABLE | FIRMWARE_CAPSULE_FLAGS_MASK)

//
// Os Loader specific standard display flags.
//

#define OSL_BGFX_FLAGS_BODY_TEXT_SMALL 0x1
#define OSL_BGFX_FLAGS_BODY_TEXT_LARGE 0x2

//
// Default font directory for winload.
//

#define OSLOADER_DEFAULT_FONT_DIRECTORY L"\\Windows\\Boot\\Fonts";

//
// Used to load OS extensions.
//

#define MAX_MODULE_NAME_LENGTH 128

//
// Determine whether this platform supports the use of SGX
//

#if defined(_X86_) || defined(_AMD64_)
#define OSL_SGX_SUPPORT 1
#endif

//
// -------------------------------------------------------------------- Globals
//

extern ULONG OslBootId;
extern PBOOT_ENVIRONMENT_DEVICE OslLoadDevice;
extern PWCHAR OslSystemRoot;
extern PCHAR OslLoadOptions;
extern PRESUME_CONTEXT ResumeContext;
extern PVOID OslApiSetSecureSchema;
extern PVOID OslApiSetSchema;
extern ULONG OslApiSetSchemaSize;
extern PDEBUG_DEVICE_DESCRIPTOR BdDebugDevice;
extern PDEBUG_DEVICE_DESCRIPTOR KdDebugDevice;
extern GUID OslApplicationIdentifier;
extern BOOLEAN OslManufacturingBitlockerEnabled;
extern LIST_ENTRY OslFreeMemoryDesctiptorsList;
extern ULONG OslFreeMemoryDesctiptorsListSize;
TRACELOGGING_DECLARE_PROVIDER(OslBootProvider);

//
// ----------------------------------------------------------------- Prototypes
//


NTSTATUS
OslArchDetectKernelHal (
    __inout PUNICODE_STRING KernelName,
    __inout PUNICODE_STRING HalName
    );

NTSTATUS
OslArchDetectUpdateLibrary (
    __inout PUNICODE_STRING UpdateLibrary
    );

NTSTATUS
OslArchKernelSetup (
    __in ULONG Phase,
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslArchLoadFirmwareDrivers (
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __out PLIST_ENTRY LoadedFirmwareDriverList
    );

VOID
OslArchDebugPatchKernelInterrupts (
    VOID
    );

VOID
OslArchTransferToKernel (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in PVOID KernelEntry
    );

NTSTATUS
OslAttachHiveToLoaderBlock (
    __in DEVICE_ID DeviceId,
    _When_((Flags & LHB_SYSTEM_HIVE) != 0, __in)
    _When_((Flags & LHB_SYSTEM_HIVE) == 0, __in_opt)
        PUNICODE_STRING OsFilePath,
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in ULONG  Flags,
    __in __nullterminated PWCHAR FilePath,
    __in __nullterminated PWCHAR RegistryParent,
    __in __nullterminated PWCHAR RegistryName,
    __out_opt PULONG Handle
    );

NTSTATUS
OslBuildKernelMemoryMap (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in ULONG ReserveDescriptorsCount,
    __out PLIST_ENTRY FreeDescriptorsList,
    __out PULONG FreeDescriptorsListSize,
    __out PVOID* AllocatedDescriptorsBuffer,
    __out PLIST_ENTRY MemoryMap,
    __out GENERIC_BUFFER* MemoryMapDescriptors
    );

NTSTATUS
OslRebuildKernelMemoryMap (
    __in ULONG ReserveDescriptors
    );

VOID
OslConfigureRNG (
    _In_ PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
OslDestroyDriverList (
    __in PLIST_ENTRY DriverList
    );

NTSTATUS
OslDestroyResumeContext (
    VOID
    );

ULONG
OslDetermineKernelStackSize (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslDisplayAdvancedOptionsProcess (
    __in ULONG SystemHiveHandle,
    __in BOOLEAN LastBootSucceeded,
    __in BOOLEAN LastBootShutdown,
    __in BOOLEAN LastBootCheckpoint,
    __in UCHAR BootAttemptCount,
    __out PULONG ReturnFlags
    );

VOID
OslDisplayDestroy (
    __in_opt PVOID DisplayContext
    );

NTSTATUS
OslDisplayInitialize (
    __in ULONG SystemHiveHandle
    );

VOID
OslDisplayMessage (
    VOID
    );

NTSTATUS
OslEnumerateDisks (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslBlStatusErrorHandler(
    __in ULONG ErrorCode,
    __in ULONG_PTR ErrorParameter1,
    __in ULONG_PTR ErrorParameter2,
    __in ULONG_PTR ErrorParameter3,
    __in ULONG_PTR ErrorParameter4
    );

VOID
OslFatalErrorEx (
    __in ULONG ErrorCode,
    __in ULONG_PTR ErrorParameter1,
    __in ULONG_PTR ErrorParameter2,
    __in ULONG_PTR ErrorParameter3,
    __in_opt PSTR  SourceFile,
    __in_opt ULONG SourceLine
    );

#if DBG

#define OslFatalError(ErrorCode, Parameter1, Parameter2, Parameter3) \
        OslFatalErrorEx((ErrorCode), (Parameter1), (Parameter2), (Parameter3), __FILE__, __LINE__);

#else

#define OslFatalError(ErrorCode, Parameter1, Parameter2, Parameter3) \
        OslFatalErrorEx((ErrorCode), (Parameter1), (Parameter2), (Parameter3), NULL, 0);

#endif

NTSTATUS
OslBuildMemoryCachingRequirementsList (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
OslFilterCoreExtensions (
    __in ULONG Phase,
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslFwBuildMemoryCachingRequirementsList (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslFwBuildRuntimeMemoryMap (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

VOID
OslFwCheckDeploySecureBoot (
    __in ULONG SystemHiveHandle
    );

NTSTATUS
OslFwDestroyLoaderBlock (
    __in PFIRMWARE_INFORMATION_LOADER_BLOCK FirmwareInformation
    );

NTSTATUS
OslFwGetBootModeInfo (
    __out PEFI_OS_BOOT_MODE BootMode,
    __out PUNICODE_STRING ProfileName
    );

NTSTATUS
OslFwGetFloppyConfigurationData (
    __in DEVICE_ID DeviceId,
    __deref_out_bcount(*Size) PCM_FLOPPY_DEVICE_DATA *FloppyData,
    __out PULONG Size
    );

NTSTATUS
OslFwGetFloppyControllerData (
     __deref_out_bcount(*Size) PCM_PARTIAL_RESOURCE_LIST *ResourceData,
     __out PULONG Size
     );

USHORT
OslFwGetKeyboardFlags (
    VOID
    );

NTSTATUS
OslFwGetVideoBiosData (
    __deref_out_ecount(*RomBlockCount) PCM_ROM_BLOCK *RomBlockArray,
    __out PULONG RomBlockCount
    );

NTSTATUS
OslFwInitializeLoaderBlock (
    __out PFIRMWARE_INFORMATION_LOADER_BLOCK FirmwareInformation
    );

NTSTATUS
OslFwKernelSetup (
    __in ULONG Phase,
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslFwLoadDriver(
    __in DEVICE_ID DeviceId,
    __in PWSTR ImagePath,
    __in BOOLEAN StartImage,
    __out PHANDLE DriverHandle
    );

NTSTATUS
OslFwLoadFirmwareDrivers(
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __out PLIST_ENTRY LoadedFirmwareDriverList
    );

NTSTATUS
OslFwProcessUpdates (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId,
    __in ULONG SystemHiveHandle
    );

NTSTATUS
OslFwProtectSecureBootVariables (
    __in ULONG SystemHiveHandle
    );

NTSTATUS
OslFwUnloadDriver(
    __in HANDLE DriverHandle
    );

BOOLEAN
OslGetBootDriverFlagsFromRegistry (
    __in     ULONG  Handle,
    __in_opt LPGUID HardwareConfigurationId,
    __out    PULONG BootDriverFlags
    );

NTSTATUS
OslGetBootDrivers (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in ULONG SystemHiveHandle,
    __in_opt PWSTR BootFileSystem,
    __in_opt PULONG Flags
    );

NTSTATUS
OslGetBootStatusData (
    __out PBOOLEAN LastBootSucceeded,
    __out PBOOLEAN LastBootShutdown,
    __out PBOOLEAN LastBootCheckpoint,
    __out PUCHAR BootAttemptCount,
    __out PULONG LastBootId,
    __out PBOOLEAN UnfilteredBootSucceeded,
    __out PBOOLEAN UnfilteredBootShutdown
    );

BOOLEAN
OslGetDriverVerifierLevelFlags (
    __in ULONG SystemHiveHandle,
    __out PULONG Flags
    );

BOOLEAN
OslGetDriverVerifierOptionFlags (
    __in ULONG SystemHiveHandle,
    __out PULONG Flags
    );

NTSTATUS
OslDisableDriverVerifier (
    __in ULONG SystemHiveHandle
    );

BOOLEAN
OslGetErrataFileNameFromRegistry (
    __in ULONG Handle,
    __out PUNICODE_STRING Name
    );

NTSTATUS
OslGetLastPowerTransition (
    __out PBSD_POWER_TRANSITION PowerTransition
    );

ULONG_PTR
OslGenRandom (
    VOID
    );

NTSTATUS
OslHiveFindDrivers (
    __in ULONG Handle,
    __in SERVICE_LOAD_TYPE LoadType,
    __in_opt PWSTR BootFileSystem,
    __in_opt PULONG Flags,
    __in_opt LPGUID HardwareConfigurationId,
    __in_opt PCUNICODE_STRING ManufacturingProfileName,
    __in_opt PFWUPDATE_RESULTS_HEADER FirmwareUpdateResults,
    __in PLIST_ENTRY DriverListHead
    );

NTSTATUS
OslHiveFindNlsData (
    __in ULONG Handle,
    __out PUNICODE_STRING AnsiFilename,
    __out PUNICODE_STRING OemFilename,
    __out PUNICODE_STRING CaseTableFilename
    );

NTSTATUS
OslHiveFindNumaData (
    __in ULONG Handle,
    __deref_out PVOID *LocalityInfo,
    __deref_out PVOID *GroupAssignment
    );

BOOLEAN
OslIsRunningInSecureKernel (
    _Out_opt_ PBOOLEAN SecureKernelRunning
    );

NTSTATUS
OslIsFirmwareDependencySatisfied (
    __in PWSTR Dependency,
    __in_opt PVOID Context,
    __out PBOOLEAN DependencySatisfied
    );

NTSTATUS
OslReadManufacturingModeProfileSettings (
    __in ULONG Handle,
    __out PBOOLEAN ManufacturingModeEnabled,
    __out PWSTR *ManufacturingProfile
    );

BOOLEAN
OslDoesManufacturingProfileExist (
    __in ULONG Handle,
    __in PWSTR ProfileName
    );

BOOLEAN
OslIsDeviceScreenLockEnabled (
    __in ULONG Handle
    );

NTSTATUS
OslHiveFlush (
    __in ULONG Handle
    );

VOID
OslHiveQueryBugcheckOptions (
    __in ULONG SystemHiveHandle,
    __out PBOOLEAN BugcheckDisabled,
    __out PBOOLEAN EmoticonDisabled,
    __out PBOOLEAN DisplayParameters
    );

BOOLEAN
OslHiveIsLkgEnabled (
    __in ULONG SystemHiveHandle
    );

NTSTATUS
OslHiveFindSkuType (
    __in ULONG Handle,
    __out PUNICODE_STRING SKUType
    );

VOID
OslReadXsaveRegistryValues(
    __in ULONG SystemHiveHandle,
    __out PLOADER_PARAMETER_BLOCK LoaderBlock
    );

BOOLEAN
OslGetExtendedStateFlags(
    __in ULONG SystemHiveHandle,
    __out PULONG Flags
    );

BOOLEAN
OslGetExtendedStateAllowedFeatures(
    __in ULONG SystemHiveHandle,
    __out PULONG64 AllowedFeatures
    );

NTSTATUS
OslHiveGetDriverLoadPolicy (
    __in ULONG Handle,
    __out PULONG DriverLoadPolicy
    );

NTSTATUS
OslHiveGetSidSharingPolicy (
    __in ULONG SystemHiveHandle,
    __out PULONG SidSharingPolicy
    );

PFIRMWARE_UPDATE_SETTINGS
OslHiveGetUpdateSettings (
    __in PEFI_SYSTEM_RESOURCE_ENTRY FirmwareResource,
    __in ULONG Handle
    );

NTSTATUS
OslHiveReadWriteDwordValues(
    _In_ ULONG Handle,
    _In_ BOOLEAN Write,
    _In_ PCWSTR KeyName,
    _In_ PCWSTR SubKeyName,
    _When_(Write, _In_) _When_(!Write, _Out_) PULONG ValueData
    );

NTSTATUS
OslHiveReadWriteControlDword (
    __in ULONG Handle,
    __in BOOLEAN Write,
    __in PWSTR KeyName,
    __in PWSTR ValueName,
    _When_(Write, _In_) _When_(!Write, _Out_) PULONG ValueData
    );

NTSTATUS
OslHiveGetApiSetSchemaExtensions (
    __in ULONG Handle,
    __in PLIST_ENTRY ListHead
    );

NTSTATUS
OslInitializeCodeIntegrity (
    __in DEVICE_ID OsDeviceId,
    __in ULONG SystemHiveHandle
    );

NTSTATUS
OslBuildCodeIntegrityLoaderBlock (
    _In_ DEVICE_ID OsDeviceId,
    _Inout_ PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslSetSeILSigningPolicy(
    _In_ PLOADER_PARAMETER_BLOCK LoaderBlock,
    _In_ ULONG SystemHiveHandle
    );

NTSTATUS
OslKdInitialize(
    _In_ PLOADER_PARAMETER_BLOCK LoaderBlock,
    _At_((PWSTR)(SystemFilePath->Buffer), _Inout_z_) PUNICODE_STRING SystemFilePath,
    _In_ DEVICE_ID OsDeviceId,
    _Outptr_opt_ PKLDR_DATA_TABLE_ENTRY *KdTransportEntry,
    _Outptr_opt_result_maybenull_ PKLDR_DATA_TABLE_ENTRY *KdExtensionEntry
    );

VOID
OslInitializeCoreExtensionSubGroups (
    VOID
    );

NTSTATUS
OslInitializeLoaderBlock (
    __in DEVICE_ID OsDeviceId,
    __in ULONG SystemHiveHandle,
    __deref_out PLOADER_PARAMETER_BLOCK *LoaderBlockAddress
    );

NTSTATUS
OslInitializeResumeContext (
    VOID
    );

NTSTATUS
OslInitializeBitlockerStatus (
    VOID
    );

NTSTATUS
OslInitializeResumePages (
    __in DEVICE_ID OsDeviceId,
    __out PULONG ResumePages
    );

NTSTATUS
OslIscsiBootMakeAcpiTable (
    __out PVOID *IscsiAcpiTable,
    __out PULONG IscsiAcpiTableLength
    );

NTSTATUS
OslLegacyHardwareDetection (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslLoadAndInitializeHive (
    __in DEVICE_ID DeviceId,
    __in PWSTR FilePath,
    __in BOOLEAN SystemHive,
    __in_opt PWSTR LogPath1,
    __in_opt PWSTR LogPath2,
    __deref_out_bcount(*ImageSize) PVOID *ImageBase,
    __out PULONG ImageSize,
    __out PLOADER_HIVE_RECOVERY_INFO RecoveryInfo,
    __out_opt PULONG Handle
    );

NTSTATUS
OslLoadCoreExtensions (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in PUNICODE_STRING DefaultPath,
    __in DEVICE_ID DefaultDevice,
    __in BOOLEAN LastKnownGoodBoot
    );

#if defined(NTKERNEL)

NTSTATUS
OslLoadHiveFromHost (
    __in DEVICE_ID DeviceId,
    __in PWSTR FilePath,
    __deref_out_bcount(*ImageSize) PVOID *ImageBase,
    __out PULONG ImageSize
    );

BOOLEAN
OslIsHiveInUse (
    VOID
    );

#endif

NTSTATUS
OslReinitializeSystemHive (
    __in ULONG SystemHiveHandle
    );

NTSTATUS
OslLoadDrivers (
    __in PLIST_ENTRY DriverList,
    __in PLIST_ENTRY LoadedDriverList,
    __in PUNICODE_STRING DefaultPath,
    __in DEVICE_ID DefaultDevice,
    __in ULONG Flags,
    __in BOOLEAN LastKnownGoodBoot
    );

NTSTATUS
OslMeasureEarlyLaunchHive (
    __in ULONG ElamHiveHandle
    );

NTSTATUS
OslMergeAcpiTableData (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in PVOID AcpiTableData,
    __in ULONG AcpiTableDataSize
    );

NTSTATUS
OslLoadOptionalAcpiTableData (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId
    );

NTSTATUS
OslLoadOptionalAcpiWpbt (
    __out PVOID *WpbtTable,
    __out PULONG WpbtLength
    );

NTSTATUS
OslGetAcpiTable (
    __deref_out PVOID *TableAddress,
    __in ULONG TableSignature,
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslGetSubkey (
    __in ULONG Handle,
    __in_opt PCM_KEY_NODE HiveCellParent,
    __in_opt PCWSTR SubkeyName,
    __out PCM_KEY_NODE *HiveCell
    );

NTSTATUS
OslGetSubkeyByNumber (
    __in ULONG Handle,
    __in_opt PCM_KEY_NODE HiveCellParent,
    __in ULONG SubkeyNumber,
    __out_opt PCM_KEY_NODE *HiveCell,
    __deref_out_z_opt PWSTR *SubkeyName
    );

NTSTATUS
OslGetStringValue (
    __in ULONG Handle,
    __in PCM_KEY_NODE HiveCellParent,
    __in PCWSTR ValueName,
    __in ULONG NameLength,
    __out_ecount(NameLength) PWCHAR Name
    );

NTSTATUS
OslGetValue (
    __in ULONG Handle,
    __in PCM_KEY_NODE HiveCellParent,
    __in PCWSTR ValueName,
    __out PULONG Type,
    __inout PULONG DataLength,
    __out_ecount(*DataLength) PBYTE Data
    );

PCM_KEY_NODE
OslGetRootCell (
    __in ULONG Handle
    );

NTSTATUS
OslGetCopyOfValue (
    __in ULONG Handle,
    __in PCM_KEY_NODE HiveCellParent,
    __in PCWSTR ValueName,
    __out PULONG Type,
    __deref_out_bcount(*ValueSize) PBYTE* Value,
    __out PULONG ValueSize
    );

NTSTATUS
OslGetBinaryValue (
    __in ULONG Handle,
    __in PCM_KEY_NODE HiveCellParent,
    __in PCWSTR ValueName,
    __deref_out_bcount(*ValueSize) PBYTE* Value,
    __out PULONG ValueSize
    );

BOOLEAN
OslIsSystemUpgradeInProgress (
    _In_ ULONG SystemHiveHandle
    );

NTSTATUS
OslGetCodeIntegrityCell (
    _In_ ULONG SystemHiveHandle,
    _Outptr_ PCM_KEY_NODE *HiveCell
    );

NTSTATUS
OslGetDWordValue (
    _In_ ULONG Handle,
    _In_ PCM_KEY_NODE HiveCellParent,
    _In_ PCWSTR ValueName,
    _Out_ PDWORD Dword
    );

BOOLEAN
OslAreDriverDependenciesSatisfied (
    __in ULONG Handle,
    __in PCM_KEY_NODE HiveCellParent,
    __in PWSTR SubkeyName,
    __in_opt PCMP_DRIVER_DEPENDENCY_CALLBACK_ROUTINE DependencyCallback,
    __in_opt PVOID DependencyCallbackContext
    );

NTSTATUS
OslLoadOsExtensions (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID DeviceId,
    __in ULONG SystemHiveHandle
    );

NTSTATUS
OslLoadImage (
    _In_ DEVICE_ID DeviceId,
    _In_ MEMORY_TYPE MemoryType,
    _In_ PWSTR ImagePath,
    _In_opt_ PWSTR FriendlyName,
    _In_opt_ PUNICODE_STRING ServiceFilePath,
    _In_opt_ PVOID PreAllocatedImageBase,
    _In_ ULONG PreAllocatedImageSize,
    _In_ PLIST_ENTRY LoadedImageList,
    _Outptr_opt_ PKLDR_DATA_TABLE_ENTRY *DataTableEntry,
    _In_ ULONG KldrFlags,
    _In_ ULONG BldrFlags,
    _In_ ULONG ImageFlags,
    _In_ ULONG OslLoadImageFlags,
    _Out_opt_ PULONG LoadInformation
    );

#define OSL_IMAGE_FLAGS_NONE                    0x00000000
#define OSL_IMAGE_FLAGS_LOAD_IMPORTS            0x00000001
#define OSL_IMAGE_FLAGS_POST_LOAD_IMPORTS       0x00000002
#define OSL_IMAGE_FLAGS_EARLY_LAUNCH            0x00000004
#define OSL_IMAGE_FLAGS_CORE_DRIVER             0x00000008
#define OSL_IMAGE_FLAGS_APISET                  0x00000010
#define OSL_IMAGE_FLAGS_ELAM_IMPORT             0x00000020
#define OSL_IMAGE_FLAGS_HAL_EXTENSION           0x00000040
#define OSL_IMAGE_FLAGS_APISET_SCHEMA           0x00000080
#define OSL_IMAGE_FLAGS_WINDOWS                 0x00000100
#define OSL_IMAGE_FLAGS_IUM                     0x00000200
#define OSL_IMAGE_FLAGS_CORE_EXTENSION          0x00000400

#define OslLoadSystemImage(_d, _m, _i, _f, _b, _s, _l, _k) \
        OslLoadImage(_d, _m, _i, _f, NULL, _b, _s, _l, _k, 0, 0, \
                     IMAGE_FLAGS_MEMORY_PRE_ALLOCATED, \
                     OSL_IMAGE_FLAGS_WINDOWS, \
                     NULL)

#define OslLoadSystemImageImports(_d, _m, _i, _f, _l, _k) \
        OslLoadImage(_d, _m, _i, _f, NULL, NULL, 0, _l, _k, 0, 0, \
                     IMAGE_FLAGS_NONE, \
                     OSL_IMAGE_FLAGS_POST_LOAD_IMPORTS | \
                        OSL_IMAGE_FLAGS_WINDOWS, \
                     NULL)

VOID
OslAbortBoot (
    __in NTSTATUS Status
    );

NTSTATUS
OslExecuteTransition (
    VOID
    );

NTSTATUS
OslMain (
    __in PBOOT_APPLICATION_PARAMETERS Parameters
    );

NTSTATUS
OslMergeAcpiTables (
    __inout PVOID *AcpiMergeTable,
    __inout ULONG *AcpiMergeTableSize,
    __in PVOID AcpiTable,
    __in ULONG AcpiTableSize
    );

NTSTATUS
OslpMain (
    __out PULONG ReturnFlags
    );

VOID
OslPersistBootDebugger (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslPrepareTarget (
    __out PULONG ReturnFlags,
    __out PBOOLEAN ExecuteTransition
    );

NTSTATUS
OslProcessApplicationPersistentData (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

NTSTATUS
OslpProcessSIPolicy(
    _In_ BOOLEAN IntegrityChecksDisabled
    );

VOID
OslReportKernelLaunch (
    __in PBOOT_ENTRY_OPTION OsLoaderOptions,
    __in ULONG SystemHiveHandle
    );

VOID
OslCloseSipaReporting ();

BOOLEAN
OslSwitchTpmPcrBanks (
    __in PBOOT_ENVIRONMENT_DEVICE BootDevice
    );

typedef enum _OSL_BOOT_COUNT_UPDATE_TYPE
{
    OslBootCountUpdateNone,
    OslBootCountUpdateIncrement,
    OslBootCountUpdateDecrement
} OSL_BOOT_COUNT_UPDATE_TYPE, *POSL_BOOT_COUNT_UPDATE_TYPE;

NTSTATUS
OslSetBootStatusData (
    __in BOOLEAN LastBootSucceeded,
    __in BOOLEAN LastBootShutdown,
    __in BOOLEAN LastBootCheckpoint,
    __in OSL_BOOT_COUNT_UPDATE_TYPE RequestedBootCountUpdate,
    __in UCHAR BootAttemptCount
    );

NTSTATUS
OslSetControlSet (
    __in ULONG Handle,
    __in __nullterminated PWCHAR ControlName
    );

VOID
OslSetSystemTimeParameters (
    __inout PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in PBSD_POWER_TRANSITION PowerTransition
    );

VOID
OslTerminateRNG (
    VOID
    );

NTSTATUS
OslUpdateBootOptions (
    __in PCWSTR BootOptions
    );

NTSTATUS
OslCheckForInvalidBootOptions (
    __in PCWSTR BootOptions,
    __in BOOLEAN CheckForSecureBootOptions
    );

NTSTATUS
OslFilterLoadOptions (
    __in PCWSTR LoadOptions,
    __deref_opt_out PWSTR *FilteredLoadOptions
    );

NTSTATUS
OslUnloadHive (
    __in ULONG Handle
    );

NTSTATUS
OslLoadMicrocode (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock
    );

BOOLEAN
OslArchHvlInitialized (
    VOID
    );

BOOLEAN
OslArchHvlBelow1MbPageAllocated (
    VOID
    );

ULONGLONG
OslArchHvlGetBelow1MbPage (
    VOID
    );

NTSTATUS
OslArchHypervisorSetup (
    __in ULONG Phase,
    __in_opt PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in ULONG SystemHiveHandle
    );

VOID
OslArchHypervisorCleanup (
    VOID
    );

BOOLEAN
OslArchHypervisorPresent (
    VOID
    );

BOOLEAN
OslArchIsPartitionCpuManager (
    VOID
    );

NTSTATUS
OslArchHypercallSetup (
    __in ULONG Phase
    );

PVOID
OslArchGetHypercallCodePage (
    VOID
    );

BOOLEAN
OslGetHypervisorDebuggerOverrides (
    __in BCDE_DEBUGGER_TYPE RootDebuggerType,
    __out PWSTR *DllPath
    );

VOID
OslArchRescindHvVsm (
    VOID
    );

NTSTATUS
OslGetImcSystemEntropyKeyValue(
    __in PCWSTR ValueName,
    __out_ecount(BufferLength) PBYTE Buffer,
    __in ULONG BufferLength,
    __out PULONG DataLength
    );

NTSTATUS
OslGatherEntropy (
    __in ULONG SystemHiveHandle,
    __out PBOOT_ENTROPY_LDR_RESULT EntropyResult,
    __out_bcount_opt(BOOT_SEED_BYTES_FOR_VSM) PBYTE EntropyForVsm,
    __out_bcount_opt(BOOT_SEED_BYTES_FOR_IDK) PBYTE EntropyForIdk,
    __out_bcount_opt(VSM_LKEY_LENGTH + BL_TPM_SEALED_BLOB_KEY_LENGTH_BYTES) PBYTE VsmLKeyCandidate
    );

VOID
OslCheckFipsMode (
    __in  ULONG    HiveHandle,
    __in  BOOLEAN  bImcHiveHandle,
    __out PBOOLEAN pbFipsModeEnabled
    );

NTSTATUS
OslFwGetEfiVersion (
    __out PUNICODE_STRING Version
    );

NTSTATUS
OslFwMapPhysicalAddressInFirmwareVaSpace (
    __in PHYSICAL_ADDRESS PhysicalStartAddress,
    __in ULONGLONG Size,
    __in ULONG Attributes,
    __out PVOID *VirtualStartAddress
    );

BOOLEAN
OslIsUnicodeStringNullTerminated (
    __in PUNICODE_STRING String
    );

NTSTATUS
OslLoadApiSetSchema (
    __in PLOADER_PARAMETER_BLOCK LoaderBlock,
    __in ULONG SystemHiveHandle,
    __inout PUNICODE_STRING OsFilePath,
    __in DEVICE_ID OsDeviceId
    );

PFN_NUMBER
OslVsmGetReservedPage (
    VOID
    );

NTSTATUS
OslVsmInitializeHypercallSupport (
   VOID
   );

NTSTATUS
OslZeroPhysicalPages (
    __in PHYSICAL_ADDRESS PhysicalAddress,
    __in ULONG PageCount
    );

#if defined(NT_IUM)

NTSTATUS
OslVsmProvisionIdk (
    _In_ PBOOT_ENVIRONMENT_DEVICE LoadDevice,
    _In_z_ PWCHAR SystemRoot,
    _Inout_updates_bytes_(BOOT_SEED_BYTES_FOR_IDK) PBYTE IdkEntropySeed,
    _In_reads_bytes_opt_(EncryptionKeySize) PBYTE IdkEncryptionKey,
    _In_ ULONG EncryptionKeySize,
    _Outptr_ PVSM_IDK_PLAINTEXT* VsmIdk,
    _Outptr_ PVSM_IDK_PLAINTEXT* VsmIdkSigning
    );

NTSTATUS
OslVsmProvisionLKey (
    _Inout_updates_bytes_(VSM_LKEY_LENGTH) PBYTE VsmLKeyCandidate,
    _Inout_updates_bytes_(BL_TPM_SEALED_BLOB_KEY_LENGTH_BYTES) PBYTE VsmLKeyEncryptionKey,
    _Out_ PVSM_LKEY_ARRAY* VsmLKeyArray
    );

#endif

NTSTATUS
OslVsmEfiSetup (
    _In_reads_bytes_(TableSize) EFI_MEMORY_DESCRIPTOR *DescriptorTable,
    _In_ ULONG_PTR TableSize,
    _In_ UINTN DescriptorSize,
    _In_reads_bytes_opt_(EfiAttributesTableSize) PEFI_MEMORY_ATTRIBUTES_TABLE EfiAttributesTable,
    _In_ SIZE_T EfiAttributesTableSize
    );

NTSTATUS
OslVsmSetup (
    _In_ ULONG Phase,
    _In_ DEVICE_ID OsDeviceId,
    _In_ PLOADER_PARAMETER_BLOCK LoaderBlock
    );

BOOLEAN
OslVsmIsInitialized (
    VOID
    );

NTSTATUS
OslSetVsmPolicy (
    _In_ ULONG SystemHiveHandle
    );

ULONG
OslGetWeakCryptoPolicySerializedSize (
    VOID
    );

NTSTATUS
OslSerializeWeakCryptoPolicy (
    _Out_writes_bytes_to_(*BufferSize, *BufferSize) PVOID Buffer,
    _Inout_ PULONG BufferSize
    );

//
// ----------------------------------------  Boot Manager BGFX Display Routines
//

NTSTATUS
OslBgDisplayAddBodyRegion (
    __in_z PWCHAR String,
    __in ULONG Flags,
    __out_opt HANDLE *Handle
    );

NTSTATUS
OslBgDisplayAddFooterRegion (
    __in_z PWCHAR String
    );

NTSTATUS
OslBgDisplayClearScreen (
    VOID
    );

NTSTATUS
OslBgDisplayDestroy (
    VOID
    );

NTSTATUS
OslBgDisplayInitialize (
    VOID
    );

NTSTATUS
OslBgDisplayInitializeInput (
    VOID
    );

VOID
OslBgDisplayRender (
    VOID
    );

NTSTATUS
OslBgDisplaySetHeader (
    __in_z PWCHAR String
    );

NTSTATUS
OslBgDisplaySetTitle (
    __in_z PWCHAR String
    );

VOID
OslBgDisplayUpdateBodyRegion (
    __in HANDLE Handle,
    __in_z PWCHAR String
    );

NTSTATUS
OslBgDisplayUpdateInput (
    __in_z PWCHAR String,
    __in ULONG Flags,
    __in ULONG CursorIndex
    );

NTSTATUS
OslBgDisplayUpdateTitle (
    __in_z PWCHAR String
    );

NTSTATUS
OslCopyOfflineCrashdumpTable (
    __out POFFLINE_CRASHDUMP_CONFIGURATION_TABLE ConfigTable
    );

#if defined(OSL_SGX_SUPPORT)

NTSTATUS
OslEnumerateEnclavePageRegions (
    VOID
    );

#endif

#ifndef __CQUERY_H__
#define __CQUERY_H__

#include "Framework.h"

class CQuery : public CObject
{
    LPDIRECT3DQUERY9 m_pQuery9;

	CDevice *m_pDevice;

    HRESULT (CQuery::*pGetDevice)(CDevice** ppDevice);
    virtual HRESULT GetDevice9(CDevice** ppDevice);
    virtual HRESULT GetDevice8(CDevice** ppDevice);
    virtual HRESULT GetDevice7(CDevice** ppDevice);
    virtual HRESULT GetDevice6(CDevice** ppDevice);

    QUERYTYPE (CQuery::*pGetType)();
    virtual QUERYTYPE GetType9();
    virtual QUERYTYPE GetType8();
    virtual QUERYTYPE GetType7();
    virtual QUERYTYPE GetType6();

    DWORD (CQuery::*pGetDataSize)();
    virtual DWORD GetDataSize9();
    virtual DWORD GetDataSize8();
    virtual DWORD GetDataSize7();
    virtual DWORD GetDataSize6();

    HRESULT (CQuery::*pIssue)(DWORD dwIssueFlags);
    virtual HRESULT Issue9(DWORD dwIssueFlags);
    virtual HRESULT Issue8(DWORD dwIssueFlags);
    virtual HRESULT Issue7(DWORD dwIssueFlags);
    virtual HRESULT Issue6(DWORD dwIssueFlags);

    HRESULT (CQuery::*pGetData)(void* pData,DWORD dwSize,DWORD dwGetDataFlags);
    virtual HRESULT GetData9(void* pData,DWORD dwSize,DWORD dwGetDataFlags);
    virtual HRESULT GetData8(void* pData,DWORD dwSize,DWORD dwGetDataFlags);
    virtual HRESULT GetData7(void* pData,DWORD dwSize,DWORD dwGetDataFlags);
    virtual HRESULT GetData6(void* pData,DWORD dwSize,DWORD dwGetDataFlags);

    void (CQuery::*pReleaseD3D)();
    void ReleaseD3D() { (this->*pReleaseD3D)();}
    
	void ReleaseD3D9();
	void ReleaseD3D8();
    void ReleaseD3D7();
    void ReleaseD3D6();

	void ReleaseAll();
    
protected:
	virtual void *GetInternalPointer(void);

    bool Init9(LPVOID pQuery);
    bool Init8(LPVOID pQuery);
    bool Init7(LPVOID pQuery);
    bool Init6(LPVOID pQuery);

public:

    CQuery();
    ~CQuery();

    virtual ULONG   WINAPI AddRef(void);
    virtual ULONG   WINAPI Release(void);

    virtual bool Init(LPVOID pQuery, CDevice *pDevice);

    virtual HRESULT GetDevice(CDevice** ppDevice)
                        { return (this->*pGetDevice)(ppDevice); }
    virtual QUERYTYPE GetType()
                        { return (this->*pGetType)(); }
    virtual DWORD GetDataSize()
                        { return (this->*pGetDataSize)(); }
    virtual HRESULT Issue(DWORD dwIssueFlags)
                        { return (this->*pIssue)(dwIssueFlags); }
    virtual HRESULT GetData(void* pData,DWORD dwSize,DWORD dwGetDataFlags)
                        { return (this->*pGetData)(pData, dwSize, dwGetDataFlags); }
};

// -------------------------------------------------------------------------------------


//
// CnQuery - used to tell framework to handle lists of swapchains into wrapper calls
//
class CnQuery : public CObject
{
private:
	CQuery** m_pQueries;
	UINT     m_uDevices;
public:
	~CnQuery();
    virtual bool Init(UINT uDevices);
	virtual bool GetQuery(CQuery **ppQuery, UINT n);
	virtual bool SetQuery(CQuery *pQuery, UINT n);

	//virtual bool GetDevice(CnDevice* ppDevice)
	virtual QUERYTYPE GetType();
    virtual DWORD GetDataSize();
    virtual bool Issue(DWORD dwIssueFlags);
	virtual HRESULT GetData(void* pData,DWORD dwSize,DWORD dwGetDataFlags);
};

#endif

// ============================================================================
//
//  pshader.cpp
//  Written by robald.
//
// ============================================================================
#include <stdio.h>
#include <io.h>
#include "PShader.h"
#include "CTextures.h"
#include "CEffect.h"
#include "types.h"

CD3DWindowFramework App;
#ifndef FAST_TEST
CPShader1_x			PixelShader1_x_Test;
CPShader1_x			PixelShader1_x_Test_tex("tex");
CPShader1_x			PixelShader1_x_Test_Cube("cube");
CPShader1_x			PixelShader1_x_Test_Volume("vol");
CPShader2_0			PixelShader2_0_Test;
CPShader2_0			PixelShader2_0_Test_tex("tex");
CPShader2_0			PixelShader2_0_Test_cube("cube");
CPShader2_0			PixelShader2_0_Test_vol("vol");
CLandscape2_0       Landscape2_0;
CStarburst2_0       Starburst2_0;
CPShader2_x			PixelShader2_x_Test;
CPShader2_x			PixelShader2_x_Test_tex("tex");
CPShader2_x			PixelShader2_x_Test_cube("cube");
CPShader2_x			PixelShader2_x_Test_vol("vol");
#endif
CPShader3_0			PixelShader3_0_Test;
CPShader3_0			PixelShader3_0_Test_tex("tex");
CPShader3_0			PixelShader3_0_Test_cube("cube");
CPShader3_0			PixelShader3_0_Test_vol("vol");

#define MAX_STRIPES 8
#define TEXSIZE     32
#define RANGE       127


#ifdef FAST_TEST
#define SHADER_NAME_PREFIX "fast_"
static const FMT g_cTestFormat = FMT_A8R8G8B8;
#else
static const FMT g_cTestFormat = FMT_ALL;
#define SHADER_NAME_PREFIX ""
#endif

// --------------------------------------------------------------------------------------

CPShader2_0::CPShader2_0(LPTSTR str) : CPShaderX()
{
	m_Options.D3DOptions.fMinDXVersion = 9.f; // !!! 8 maybe?

	// Setup macro for pixel shader version:
	
	static D3DXMACRO macros_2_0[] = {{"USING_PS_2_0", ""}, {NULL,NULL}};
	
	m_pMacros = macros_2_0;

	if(!_tcsicmp(str, "tex"))
	{		
		m_szTestName="PShader2.0_Textures";
		m_szCommandKey="ps20_tex";
		m_szPSPrefix = "ps2_0tex";
		m_Options.ModeOptions.dwTexExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN;// | PF_YUV;
		m_Options.ModeOptions.TextureFmt = g_cTestFormat;
	}
	else if(!_tcsicmp(str, "cube"))
	{
        m_szTestName="PShader2.0_Cubes";
		m_szCommandKey="ps20_cube";
		m_szPSPrefix = "ps2_0cube";
		m_Options.ModeOptions.dwCubeExclude |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN;// | PF_YUV;
		m_Options.ModeOptions.CubeFmt = g_cTestFormat;
	}
	else if(!_tcsicmp(str, "vol"))
	{
		m_szTestName="PShader2.0_Volumes";
		m_szCommandKey="ps20_vol";
		m_szPSPrefix = "ps2_0vol";
		m_Options.ModeOptions.dwVolExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN;// | PF_YUV;
		m_Options.ModeOptions.VolumeFmt = g_cTestFormat;
	}
	else
	{
		m_szTestName="PShader2.0";
		m_szCommandKey="ps20";
		m_szPSPrefix = "ps2_0_";
	}
}

// ----------------------------------------------------------------------------

CPShader2_x::CPShader2_x(LPTSTR str) : CPShaderX()
{
	m_Options.D3DOptions.fMinDXVersion = 9.f; // !!! 8 maybe?

	// Setup macro for pixel shader version:
	static D3DXMACRO macros_2_x[] = {{"USING_PS_2_X", ""}, {NULL,NULL}};
	m_pMacros = macros_2_x;

	if(!_tcsicmp(str, "tex"))
	{		
		m_szTestName="PShader2x_Textures";
		m_szCommandKey="ps2x_tex";
		m_szPSPrefix = "ps2_xtex";
		m_Options.ModeOptions.dwTexExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN | PF_YUV;
		m_Options.ModeOptions.TextureFmt = g_cTestFormat;
	}
	else if(!_tcsicmp(str, "cube"))
	{
        m_szTestName="PShader2x_Cubes";
		m_szCommandKey="ps2x_cube";
		m_szPSPrefix = "ps2_xcube";
		m_Options.ModeOptions.dwCubeExclude |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN | PF_YUV;
		m_Options.ModeOptions.CubeFmt = g_cTestFormat;
	}
	else if(!_tcsicmp(str, "vol"))
	{
		m_szTestName="PShader2x_Volumes";
		m_szCommandKey="ps2x_vol";
		m_szPSPrefix = "ps2_xvol";
		m_Options.ModeOptions.dwVolExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN | PF_YUV;
		m_Options.ModeOptions.VolumeFmt = g_cTestFormat;
	}
	else
	{
		m_szTestName="PShader2x";
		m_szCommandKey="ps2x";
		m_szPSPrefix = "ps2_x_";
	}
}

// ----------------------------------------------------------------------------

CPShader3_0::CPShader3_0(LPTSTR str) : CPShaderX()
{
	m_Options.D3DOptions.fMinDXVersion = 9.f;

	// Setup macro for pixel shader version:
	static D3DXMACRO macros_3_0[] = {{"USING_PS_3_0", ""}, {NULL,NULL}};
	m_pMacros = macros_3_0;

	if(!_tcsicmp(str, "tex"))
	{		
		m_szTestName="PShader3.0_Textures";
		m_szCommandKey="ps30_tex";
		m_szPSPrefix = SHADER_NAME_PREFIX "ps3_0tex";
		m_Options.ModeOptions.dwTexExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN | PF_YUV;
		m_Options.ModeOptions.TextureFmt = g_cTestFormat;
	}
	else if(!_tcsicmp(str, "cube"))
	{
		m_szTestName="PShader3.0_Cubes";
		m_szCommandKey="ps30_cube";
		m_szPSPrefix = SHADER_NAME_PREFIX "ps3_0cube";
		m_Options.ModeOptions.dwCubeExclude |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN | PF_YUV;
		m_Options.ModeOptions.CubeFmt = g_cTestFormat;
	}
	else if(!_tcsicmp(str, "vol"))
	{
		m_szTestName="PShader3.0_Volumes";
		m_szCommandKey="ps30_vol";
		m_szPSPrefix = SHADER_NAME_PREFIX "ps3_0vol";
		m_Options.ModeOptions.dwVolExclude  |= PF_BUMPDUDV | PF_BUMPLUMINANCE | PF_METN | PF_YUV;
		m_Options.ModeOptions.VolumeFmt = g_cTestFormat;
	}
	else
	{
		m_szTestName="PShader3.0";
		m_szCommandKey="ps30";
		m_szPSPrefix = SHADER_NAME_PREFIX "ps3_0_";
	}

}
    
// ----------------------------------------------------------------------------

CPShaderX::CPShaderX() {
    SetTestType(TESTTYPE_CONF);

	m_Options.D3DOptions.dwPSGPRegChange = PSGPREGCHANGE_DISABLE;
	m_Options.D3DOptions.dwRefPSGPRegChange = PSGPREGCHANGE_DISABLE;

	m_nTex2D = m_nTexCube = m_nTexVol = 0;
    // Setup the colors for the Stripes array
    m_Stripes[0] = RGBA_MAKE(255,255,255,255);  // White
    m_Stripes[1] = RGBA_MAKE(255,0,0,255);	    // Red
    m_Stripes[2] = RGBA_MAKE(0,255,0,255);	    // Green
    m_Stripes[3] = RGBA_MAKE(0,0,255,255);	    // Blue
    m_Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
    m_Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
    m_Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
    m_Stripes[7] = RGBA_MAKE(128,128,128,255);  // Grey

    m_nEffectDesc = 0;
    m_nEffectDescAlloc = 0;
    m_pEffectDesc = NULL;
    m_nCurrentEffect = 0;
	m_dwFXCreateFlags = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;

    // Parse the cmdline
    m_bLoadFromFile = KeySet(_T("FromFile"));
    m_bLoadFile = ReadString(_T("Load"), m_sFile, ARRAYSIZE(m_sFile));

    m_Include.SetLoadFromFile(m_bLoadFromFile);
	
    memset(m_pTex2D, 0, sizeof(CnTexture*) * m_nMaxTex2D);
    memset(m_pTexCube, 0, sizeof(CnCubeTexture*) * m_nMaxTexCube);
    memset(m_pTexVol, 0, sizeof(CnVolTexture*) * m_nMaxTexVol);

    m_szPSPrefix = "X"; // should not match any shader filenames

	m_pMacros = NULL;
}

// ----------------------------------------------------------------------------

bool CPShaderX::Setup() 
{
	m_fPrevTolPerPixel = m_pFramework->m_ImageCmp.GetTolPerPixel();
	m_fPrevTolPerPixelChannel = m_pFramework->m_ImageCmp.GetTolPerPixelChannel();
	
	float 		fAspect = 1.33f;
    float		fFOV = pi/4.0f;

    // Set the viewport data
    m_vLoc = D3DXVECTOR3(0.0f, 0.0f, -2.4f);
    m_vAt  = D3DXVECTOR3(0.0f, 0.0f, 10.0f);
    m_vUp  = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
    
	D3DXMatrixPerspectiveFovLH(&m_mPerc, fFOV, fAspect, 1.0f, 1000.0f);
	D3DXMatrixOrthoLH(&m_mOrtho, 2.f, 2.f, -3.0f, 3.0f);
    D3DXMatrixLookAtLH(&m_mView,  &m_vLoc, &m_vAt, &m_vUp);
    D3DXMatrixIdentity(&m_mWorld);
    D3DXMatrixMultiply(&m_mTotal[0], &m_mWorld, &m_mView);
    D3DXMatrixMultiply(&m_mTotal[0], &m_mTotal[0], &m_mOrtho);
	D3DXMatrixMultiply(&m_mTotal[1], &m_mWorld, &m_mView);
    D3DXMatrixMultiply(&m_mTotal[1], &m_mTotal[1], &m_mPerc);

    // Set the FixedFunction Pipeline	
    if(!SetTransform(D3DTS_PROJECTION, &m_mOrtho) ||
       !SetTransform(D3DTS_WORLD,      &m_mWorld) ||
       !SetTransform(D3DTS_VIEW,       &m_mView )  )
        return false;

    // Create the geometry
    CShapes	Quad, Sphere, TinyQuad;
    Quad.NewShape(CS_MESH, 3, 3);
    Quad.Scale(2.7f,2.f,2.f);
    Sphere.NewShape(CS_SPHERE, 20, 20);
    Sphere.Scale(2.4f,1.8f,1.8f);
    TinyQuad.NewShape(CS_MESH, 3, 3);
    TinyQuad.Scale(.27f,.2f,.2f);

	VERTEXELEMENT decl[] = 
	{
            {0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
            {0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL,   0},
            {0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
            DECL_END()
	};
    if(!CreateVertexDeclaration(decl, &(m_QuadInfo.pDecl    )) ||
       !CreateVertexDeclaration(decl, &(m_SphereInfo.pDecl  )) ||
       !CreateVertexDeclaration(decl, &(m_TinyQuadInfo.pDecl))  )
    {
        WriteToLog("CPShaderX::Setup - CreateVertexDeclaration failed.\n");
        return false;
    }
    m_QuadInfo.pVertices = new D3DVERTEX[Quad.m_nVertices];
    m_QuadInfo.pIndices = new WORD[Quad.m_nIndices];
    m_QuadInfo.nVertices = Quad.m_nVertices;
    m_QuadInfo.nIndices = Quad.m_nIndices;
    memcpy( m_QuadInfo.pVertices, Quad.m_pVertices, Quad.m_nVertices*sizeof(D3DVERTEX) );
    memcpy( m_QuadInfo.pIndices, Quad.m_pIndices, Quad.m_nIndices*sizeof(WORD) );
    
	m_SphereInfo.pVertices = new D3DVERTEX[Sphere.m_nVertices];
    m_SphereInfo.pIndices = new WORD[Sphere.m_nIndices];
    m_SphereInfo.nVertices = Sphere.m_nVertices;
    m_SphereInfo.nIndices = Sphere.m_nIndices;
    memcpy( m_SphereInfo.pVertices, Sphere.m_pVertices, Sphere.m_nVertices*sizeof(D3DVERTEX) );
    memcpy( m_SphereInfo.pIndices, Sphere.m_pIndices, Sphere.m_nIndices*sizeof(WORD) );
    
	m_TinyQuadInfo.pVertices = new D3DVERTEX[TinyQuad.m_nVertices];
    m_TinyQuadInfo.pIndices = new WORD[TinyQuad.m_nIndices];
    m_TinyQuadInfo.nVertices = TinyQuad.m_nVertices;
    m_TinyQuadInfo.nIndices = TinyQuad.m_nIndices;
    memcpy( m_TinyQuadInfo.pVertices, TinyQuad.m_pVertices, TinyQuad.m_nVertices*sizeof(D3DVERTEX) );
    memcpy( m_TinyQuadInfo.pIndices, TinyQuad.m_pIndices, TinyQuad.m_nIndices*sizeof(WORD) );

	if ( !CreatePrecisionBuffers() )
    {
        WriteToLog("CPShaderX::Setup - CreatePrecisionBuffers failed.\n");
        return false;
    }

    if ( !CreateTextures() )
    {
        WriteToLog("CPShaderX::Setup - CreateTextures failed.\n");
        return false;
    }
    if ( !CreateCubeTextures() )
    {
        WriteToLog("CPShaderX::Setup - CreateCubeTextures failed.\n");
        return false;
    }
    if ( !CreateVolTextures() )
    {
        WriteToLog("CPShaderX::Setup - CreateVolTextures failed.\n");
        return false;
    }

	/*float v0[4] = {-1.96875, -0.96875, -1.6875,  1.8125}; 
	float v1[4] = {-1.78125,  0.09375, -0.46875, 1.34375}; 
	float v2[4] = {-1.625,   -1.46875,  0.625,  -1.125};

	float lrp[4];
	for(int i = 0; i < 4; i++)
		lrp[i] = v0[i]*(v1[i]-v2[i]) + v2[i];*/

	//GenerateTechniques();
	/*	float a[] = {512, 513, 0.5f};
	int n = 3;
	
	for (int i =0; i < n; i++)
	{
		float x = a[i];
		float y = log(x)/log(2.f);
		float z = floor(abs(y));
        z = z- 15;
		float e = exp((float)z);
		e =e;
	}
*/
	return true;
}

// ----------------------------------------------------------------------------

BOOL CPShaderX::GetEffectDesc(LPTSTR lpszName)
{
    CEffect *pEffect = NULL;
	HRESULT hr = S_OK;
    LPEFFECTDESC pEffectDesc = NULL;
    const UINT nGrow = 16;
    LPD3DXBUFFER pErrors = NULL;
    D3DXEFFECT_DESC FXDesc;

    // Grow the list as needed
    if(m_nEffectDesc + 1 >= m_nEffectDescAlloc)
    {
        pEffectDesc = new EFFECTDESC[m_nEffectDescAlloc + nGrow];
        
        if(NULL == pEffectDesc)
        {
            DPF(1, _T("CPShaderX::GetEffectDesc() - Out of memory.\n"));
            return FALSE;
        }
        
        m_nEffectDescAlloc += nGrow;
        
        memset(pEffectDesc, 0, sizeof(EFFECTDESC) * m_nEffectDescAlloc);

        if(NULL != m_pEffectDesc)
        {
            //memcpy(pEffectDesc, m_pEffectDesc, m_nEffectDesc * sizeof(EFFECTDESC));

			for (UINT i = 0; i < m_nEffectDesc; i++)
			{
				pEffectDesc[ i ].sName = m_pEffectDesc[ i ].sName;
				pEffectDesc[ i ].nTechniques = m_pEffectDesc[ i ].nTechniques;
				
				if ( m_pEffectDesc[ i ].pIndices )
				{
					pEffectDesc[ i ].pIndices = new int[ m_pEffectDesc[ i ].nTechniques ];
					memcpy(pEffectDesc[ i ].pIndices, m_pEffectDesc[ i ].pIndices, m_pEffectDesc[ i ].nTechniques * sizeof(int));
					delete [] m_pEffectDesc[ i ].pIndices;
				}
				else 
					pEffectDesc[ i ].pIndices = NULL;

			}
		}

		SAFEDELETEA(m_pEffectDesc);
        m_pEffectDesc = pEffectDesc;
    }
    
    // The test absolutely crawls when loading all the shader files. Provide a nice update to tell the user what's going on.
    
    TCHAR szStatus[MAX_PATH];
    
    wsprintf(szStatus,"Load: %s",lpszName);
    
    ::SendMessage(m_pFramework->m_pStatusBar->m_hWindow,WM_SETTEXT,0,(LPARAM) szStatus);
    
    // Provide test interactivity
    
    m_pFramework->ProcessMessages();

    // Load the effect and count the techniques
    
    if(m_bLoadFromFile)
        hr = m_pSrcDevice->CreateEffectFromFile(lpszName, m_pMacros, &m_Include, m_dwFXCreateFlags, NULL, &pEffect, &pErrors);
    else
        hr = m_pSrcDevice->CreateEffectFromResource(NULL, lpszName, m_pMacros, &m_Include, m_dwFXCreateFlags, NULL, &pEffect, &pErrors);
        
    if(FAILED(hr))
    {
        DPF(1, _T("CPShaderX::GetEffectDesc() - Failed to load effect(%s).\n"), lpszName);
        if(pErrors)
        {
            DPF(1, _T("CPShaderX::GetEffectDesc() - Compile error: %s.\n"), pErrors->GetBufferPointer());
        }
        
        goto Exit;
    }
    
    pEffect->GetDesc(&FXDesc);

    m_pEffectDesc[m_nEffectDesc].nTechniques = FXDesc.Techniques;

	int *pTempIndices = new int[ FXDesc.Techniques ];
	//m_pEffectDesc[m_nEffectDesc].pIndices    = new int[ FXDesc.Techniques ];

	for (UINT i = 0; i < FXDesc.Techniques; i++)
		pTempIndices[ i ] = i;

	DEVICEDESC *pCaps = m_pSrcDevice->GetCaps();

	//we want to skip all the techniques which depend on caps which are not supported
	if ( !KeySet("show_skips") )
	{
		for (UINT i = 0; i < FXDesc.Techniques; i++)
		{
			if ( NULL != strstr(lpszName, "PS2_X") || NULL != strstr(lpszName, "PS1_X") )
			{
				if (FAILED(pEffect->ValidateTechnique( pEffect->GetTechnique(i) )))
				{
					pTempIndices[ i ] = -1;
					m_pEffectDesc[m_nEffectDesc].nTechniques--;
					continue;
				}
			}

			//ValidateTechnique() doesn't validate sampler states
			if ( !(pCaps->dwRasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS) )
			{
				D3DXHANDLE hAnnot = NULL;
				LPCSTR     sName = NULL;
				
				hAnnot = pEffect->GetAnnotationByName(pEffect->GetTechnique( i ), "Name");
				pEffect->GetString( hAnnot, &sName );

				if ( NULL != strstr(sName, "MipMapLODBias") )
				{
					pTempIndices[ i ] = -1;
					m_pEffectDesc[m_nEffectDesc].nTechniques--;
				}
			}
		}
		m_pFramework->m_DXGLog.SkipTest( FXDesc.Techniques - m_pEffectDesc[m_nEffectDesc].nTechniques );
	}

	if ( m_pEffectDesc[m_nEffectDesc].nTechniques != 0 )
		m_pEffectDesc[ m_nEffectDesc ].pIndices = new int[ m_pEffectDesc[m_nEffectDesc].nTechniques ];
	else 
		m_pEffectDesc[ m_nEffectDesc ].pIndices = NULL;

	UINT j = 0;
	for (UINT i = 0; i < FXDesc.Techniques; i++)
	{
		if ( pTempIndices[ i ] != -1 )
		{
			ASSERT(j < m_pEffectDesc[m_nEffectDesc].nTechniques);
			m_pEffectDesc[ m_nEffectDesc ].pIndices[ j++ ] = i;
		}
	}
	delete [] pTempIndices;

    COPYSTRING(m_pEffectDesc[ m_nEffectDesc ].sName, lpszName);
    m_nEffectDesc++;

  Exit:
    RELEASE(pEffect);
    RELEASE(pErrors);

    return FAILED(hr) ? FALSE : TRUE;
}

// ----------------------------------------------------------------------------

BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam)
{
    CPShaderX *pTest = (CPShaderX*)lParam;
    
    if(_tcsicmp(lpszName, _T("include.fx")))
    {
        // if the glove don't fit, you must acquit
        
        if (0 == _tcsnicmp(pTest->m_szPSPrefix, lpszName, _tcslen(pTest->m_szPSPrefix)))
        {
            return pTest->GetEffectDesc(lpszName);
        }
    }

    return TRUE;
}

// ----------------------------------------------------------------------------

bool CPShaderX::CapsCheck(void) {
    HRESULT hr = S_OK;
    CAPS    D3DRawCapsSrc;
    DWORD   dwMaxPSVersion = 0;

    // Check if caps from 2 GetDeviceCaps() functions are in agreement
    // Source device
    
    hr = m_pD3D->GetDeviceCaps(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), &D3DRawCapsSrc);
    
    if (FAILED(hr))
    {
        WriteToLog("PShader: GetDeviceCaps() call failed\n");
        return false;
    }
    else
    {
        if (D3DRawCapsSrc.PixelShaderVersion != m_pSrcDevice->GetCaps()->dwPixelShaderVersion)
        {
            WriteToLog("PShader: Caps are not consistent on source device\n");
            return false;
        }
    }
	
    dwMaxPSVersion = m_pSrcDevice->GetCaps()->dwPixelShaderVersion;
    
    WriteToLog( _T("Pixel shader version:  %8X\n"), dwMaxPSVersion);
    WriteToLog( _T("Pixel shader version:  %d.%d\n"), D3DSHADER_VERSION_MAJOR(dwMaxPSVersion), D3DSHADER_VERSION_MINOR(dwMaxPSVersion));
                
    if ( FLT_MAX == m_pSrcDevice->GetCaps()->dvMaxPixelShaderValue )
        WriteToLog( _T("Pixel shader max value: FLT_MAX\n"));
    else
        WriteToLog( _T("Pixel shader max value: %f\n"), m_pSrcDevice->GetCaps()->dvMaxPixelShaderValue);

    if (0xFFFF0000 != (dwMaxPSVersion & 0xFFFF0000))
    {
        WriteToLog( _T("ERROR: High word of version (%4X) should be FFFF.\n"), (dwMaxPSVersion >> 16));
    }
    //define some texture related variables (used for texldp)
    
    m_MaxTexRepeat[0] = (float)m_pSrcDevice->GetCaps()->dwMaxTextureRepeat;
    
    if ( m_MaxTexRepeat[0] == 0 )
        m_MaxTexRepeat[0] = 1000.f;
        
	m_MaxTexRepeat[1] = -m_MaxTexRepeat[0];
	m_MaxTexRepeat[2] = 1.f/m_MaxTexRepeat[0];
	m_MaxTexRepeat[3] = 1.f/(4.f*m_pSrcDevice->GetCaps()->dwMaxTextureWidth);

    return true;
}

// ----------------------------------------------------------------------------

UINT CPShaderX::TestInitialize(void)
{
    HRESULT		hr = S_OK;
    UINT		nTests = 0;

	m_bCheckPrecision = false;
	m_uExponentPrecision = 0;
	m_uMantissaPrecision = 0;

	//!!! We don't need this, right?
#if 0
    // Do Version / Cap checking
    if( m_dwVersion < 0x900 )
    {
        WriteToLog(_T("This test requires DX9.\n"));
        SetTestRange(1,1);
        return D3DTESTINIT_SKIPALL;
    }
#endif

    // Check if the cmdline overload is set to load a specific file
    if(m_bLoadFile)
    {
        // Load the file requested from the cmdline
        m_bLoadFromFile = true;
        m_Include.SetLoadFromFile(m_bLoadFromFile);
        if(!GetEffectDesc(m_sFile))
        {
            WriteToLog(_T("Failed to load %s.\n"), m_sFile);
            return D3DTESTINIT_ABORT;
        }
    }
    else
    {
        // Enumerate all the FX files in the resources
        if(!EnumResourceNames(NULL, RT_RCDATA, (ENUMRESNAMEPROCA)EnumResNameProc, (LONG_PTR)this))
        {
            WriteToLog(_T("Failed to enumerate the effects.\n"));
            return D3DTESTINIT_ABORT;
        }
    }

    // Calc the test range
    for(UINT i=0; i<m_nEffectDesc; i++)
        nTests += m_pEffectDesc[i].nTechniques;

    // Set the test range we've calculated
    SetTestRange(1,nTests);

	if ( (m_pCommonTextureFormats && m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_A8) ||
		 (m_pCommonCubeFormats    && m_pCommonCubeFormats   [m_pCurrentMode->nCubeFormat   ].d3dfFormat == FMT_A8) ||
		 (m_pCommonVolumeFormats  && m_pCommonVolumeFormats [m_pCurrentMode->nVolumeFormat ].d3dfFormat == FMT_A8)  )
	{
		SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		SetRenderState(D3DRS_DESTBLEND, D3DBLEND_SRCALPHA);

		SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
	}

	return D3DTESTINIT_RUN;
}
// --------------------------------------------------------------------------------------

bool CPShaderX::ClearFrame(void)
{
	if ( (m_pCommonTextureFormats && m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_A8) ||
		 (m_pCommonCubeFormats    && m_pCommonCubeFormats   [m_pCurrentMode->nCubeFormat   ].d3dfFormat == FMT_A8) ||
		 (m_pCommonVolumeFormats  && m_pCommonVolumeFormats [m_pCurrentMode->nVolumeFormat ].d3dfFormat == FMT_A8)  )
		return Clear(0, NULL, FMWK_DEFAULT, 0xFFffffff, 1.0f, 0);
	else
		return Clear(0, NULL, FMWK_DEFAULT, 0xFF000050, 1.0f, 0);
}

// ----------------------------------------------------------------------------

bool CPShaderX::ExecuteTest(UINT uTestNumber)
{
    HRESULT         hr = S_OK;
    bool			result = true;
	TCHAR           tcsDesc[512];
    LPD3DXBUFFER    pErrors = NULL;
    D3DXHANDLE      hAnnot = NULL;
    UINT            uPasses = 0;
    LPCSTR          sEffectTestCase = NULL, sShape = NULL;
    DECL_INFO      *pShape0_Info = NULL, *pShape1_Info = NULL;
	int             NumIndices = 0;
	int             offset = 0;
	UINT            Stride;
    UINT            nEffect = 0;
    UINT            nTechnique = uTestNumber-1;
	CAPS            caps;
	
	m_pSrcDevice->GetDeviceCaps(&caps); 

    // Decode the test number
    for(UINT i = 0; i < m_nEffectDesc; i++)
    {
        if(nTechnique >= m_pEffectDesc[i].nTechniques)
            nTechnique -= m_pEffectDesc[i].nTechniques;
        else
        {
            nEffect = i;
			nTechnique = m_pEffectDesc[ i ].pIndices[ nTechnique ];
            break;
        }
    }

    // Load the next effect if needed
    if(m_pCurrentEffect == NULL || nEffect != m_nCurrentEffect)
    {
        // Release existing state
        RELEASE(m_pCurrentEffect);

        m_nCurrentEffect = nEffect;

        // Load the effect
		if(m_bLoadFromFile)
			result = CreateEffectFromFile((TCHAR*)m_pEffectDesc[m_nCurrentEffect].sName, m_pMacros, &m_Include, m_dwFXCreateFlags, NULL, &m_pCurrentEffect, &pErrors);
		else
			result = CreateEffectFromResource(NULL, (TCHAR*)m_pEffectDesc[m_nCurrentEffect].sName, m_pMacros, &m_Include, m_dwFXCreateFlags, NULL, &m_pCurrentEffect, &pErrors);
        
        if(!result)
        {
            _stprintf(tcsDesc, _T("Effect %s, Technique %d"), m_pEffectDesc[m_nCurrentEffect].sName, nTechnique);
            BeginTestCase(tcsDesc, uTestNumber);
			if(pErrors)
			{
				DPF(1, _T("ExecuteTest: failed to create effect - Compile error: %s.\n"), pErrors->GetBufferPointer());
				RELEASE (pErrors);
			}
            Fail();
            return false;
        }
    }

	if (!m_pCurrentEffect->SetTechniqueByIndex(nTechnique))
	{ 
		BeginTestCase("Could not extract test name from effect!", uTestNumber);
		WriteToLog(_T("ExecuteTest: SetTechniqueByIndex() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
		Fail(); 
		return false;
	}
	
    hAnnot = m_pCurrentEffect->GetAnnotationByName(m_pCurrentEffect->GetCurrentTechnique(), "Name");
    m_pCurrentEffect->GetString(hAnnot, &sEffectTestCase);

    hAnnot = m_pCurrentEffect->GetAnnotationByName(m_pCurrentEffect->GetCurrentTechnique(), "Shape");
    m_pCurrentEffect->GetString(hAnnot, &sShape);

	//for some cases we increase pixel tolerance
	hAnnot = m_pCurrentEffect->GetAnnotationByName(m_pCurrentEffect->GetCurrentTechnique(), "TolPerPixel");

	m_fPrevTolPerPixel = m_pFramework->m_ImageCmp.GetTolPerPixel();

	if (hAnnot != NULL)
	{
        float  fTolPerPixel = 0.f;

        m_pCurrentEffect->GetFloat(hAnnot, &fTolPerPixel);
		m_pFramework->m_ImageCmp.SetTolPerPixel(fTolPerPixel);
	}

	hAnnot = m_pCurrentEffect->GetAnnotationByName(m_pCurrentEffect->GetCurrentTechnique(), "TolPerPixelPerChannel");

    m_fPrevTolPerPixelChannel = m_pFramework->m_ImageCmp.GetTolPerPixelChannel();

	if (hAnnot != NULL)
	{
        float  fTolPerPixelChannel = 0.f;

        m_pCurrentEffect->GetFloat(hAnnot, &fTolPerPixelChannel);
		m_pFramework->m_ImageCmp.SetTolPerPixelChannel(fTolPerPixelChannel);
	}

	//we want to generate pixel shader ID for each shader. We'll keep two separate constants:
	//c10.x - Test Number
	//c10.y - sum of characters in the string
	LPCSTR tempStr = sEffectTestCase;
	UINT sum = 0;
	float TechniqueID[4] = {(float)uTestNumber, 0.f, 0.f, 0.f}; 
	while (NULL != tempStr && tempStr[0] != '\0')
	{
		sum += tempStr[0];
		tempStr++;
	}
	TechniqueID[1] = (float)sum;
	
	UINT uConst = 10;
	
#pragma warning( disable : 38001 )
	if (lstrcmpi(m_pMacros->Name,"USING_PS_1_X") == 0)
	    uConst = 7;
#pragma warning( default : 38001 )
	    
	SetPixelShaderConstant(uConst, TechniqueID, 1); 

    // Begin the test case
	TCHAR testName[200];
	_stprintf(testName, "(%d) ", sum);
	
	if ( NULL != sEffectTestCase )
		strncat(testName, sEffectTestCase, 200 - strlen(testName));

    BeginTestCase(testName, uTestNumber);

	//if case uses LODBias then need to check if LODBiasing is supported by current adapter
	//unless show_skips command line is set we have already checked for this cap in GetEffectDesc()
	if ( KeySet("show_skips") && NULL != sEffectTestCase )
	{
		if ( !(m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS) )
		{
			if ( NULL != strstr(sEffectTestCase, "MipMapLODBias") )
			{
				WriteToLog( _T("ExecuteTest: D3DPRASTERCAPS_MIPMAPLODBIAS is not set.\n") ); 
				Skip();
				return true;
			}
		}
	}

	if (!m_pCurrentEffect->ValidateTechniqueByIndex(nTechnique))
	{
        WriteToLog(_T("ExecuteTest: ValidateTechniqueByIndex() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr); 
		Skip();
		return true;
    }

	// Set the params
    if (!m_pCurrentEffect->SetValue("mWorld",        &m_mWorld,       sizeof(D3DXMATRIX))  ||
	    !m_pCurrentEffect->SetValue("fvPosLimits",   &m_vPosLimits,   4*sizeof(float))     ||
        !m_pCurrentEffect->SetValue("fMaxTexRepeat", &m_MaxTexRepeat, 4*sizeof(float))     ||
        !m_pCurrentEffect->SetValue("fTexSizes",     &m_TexSizes,     4*sizeof(float))     ||
	    !m_pCurrentEffect->SetValue("fCubeSizes",    &m_CubeSizes,    4*sizeof(float))     ||
	    !m_pCurrentEffect->SetValue("fVolSizes",     &m_VolSizes,     4*sizeof(float))      )
	{
		WriteToLog(_T("ExecuteTest: SetValue() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr); 
		Fail(); 
		return false;
    }

	//choose m_mTotal matrix to use based on what shape is used. 
	//If shpere is used then we use percpective projection, otherwise we use orthogonal projection
	if(!_tcsicmp(sShape, _T("sphere")) || KeySet("perc"))
    	m_pCurrentEffect->SetValue("mFinal", &m_mTotal[1], sizeof(D3DXMATRIX));
	else
		m_pCurrentEffect->SetValue("mFinal", &m_mTotal[0], sizeof(D3DXMATRIX));

    for(UINT i=0; i<m_nTex2D; i++)
    {
        _stprintf(tcsDesc, _T("Tex2D_%d"), i);
        if (!m_pCurrentEffect->SetTexture(tcsDesc, m_pTex2D[i]))
		{
			WriteToLog(_T("ExecuteTest: SetTexture() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr); 
			Fail(); 
			return false;
		}
    }
    for(UINT i=0; i<m_nTexCube; i++)
    {
        _stprintf(tcsDesc, _T("TexCube_%d"), i);
        if (!m_pCurrentEffect->SetCubeTexture(tcsDesc, m_pTexCube[i]))
		{
			WriteToLog(_T("ExecuteTest: SetCubeTexture() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr); 
			Fail(); 
			return false;
		}
    }
    for(UINT i=0; i<m_nTexVol; i++)
    {
        _stprintf(tcsDesc, _T("TexVol_%d"), i);
        if (!m_pCurrentEffect->SetVolTexture(tcsDesc, m_pTexVol[i]))
		{
			WriteToLog(_T("ExecuteTest: SetVolTexture() failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr); 
			Fail(); 
			return false;
		}
    }
	
    // Render the scene
    if(!m_pCurrentEffect->Begin(&uPasses, 0))
    {
        WriteToLog(_T("ExecuteTest: m_pCurrentEffect->Begin() failed! hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
		Fail(); 
		return false;
    }

    BeginScene();

    // Render all passes
    
    for(UINT i=0; i<uPasses; i++) 
	{
        if (!m_pCurrentEffect->BeginPass(i)) 
		{
            WriteToLog(_T("Pass(%d) failed. hr=%s(%x)\n"), i, m_pD3D->HResultToString(GetLastError()), hr); 
			Fail(); 
			break;
        }

		bool bUseIndices = true;

		// choose a shape to use
		
		if(!_tcsicmp(sShape, _T("Precision")))
        {
            pShape0_Info = &m_PrecisionInfo;
            Stride = sizeof(TVERTEX);
			bUseIndices = false;
			m_bCheckPrecision = true;
        }
        else if(!_tcsicmp(sShape, _T("quad")))
        {
            pShape0_Info = &m_QuadInfo;
            Stride = sizeof(D3DVERTEX);
        }
        else if(!_tcsicmp(sShape, _T("sphere")))
        {
            pShape0_Info = &m_SphereInfo;
            Stride = sizeof(D3DVERTEX);
        }
        else if(!_tcsicmp(sShape, _T("tinyquad")))
        {
            pShape0_Info = &m_TinyQuadInfo;
            Stride = sizeof(D3DVERTEX);
        }
		else if(!_tcsnicmp(sShape, _T("textl"), _tcslen("textl")))
        {
			LPSTR temp = NULL;
			temp = _tcsninc(sShape, _tcslen("textl"));
			int l = atoi(temp);

			if (l >= m_pTex2D[0]->GetLevelCount())
			{
                EndScene();
				m_pCurrentEffect->End();
				SkipTestRange(uTestNumber, uTestNumber);
				return true;
			}

			offset = m_TexTLInfo.nIndices*l;
			m_TexTLInfo.pIndices = m_TexTLInfo.pIndices + offset;
			pShape0_Info = &m_TexTLInfo;
			Stride = sizeof(TLVERTEX);
        }
		else if(!_tcsnicmp(sShape, _T("cubetl"), _tcslen("cubetl")))
        {
			LPSTR temp = NULL;
			temp = _tcsninc(sShape, _tcslen("cubetl"));
			int l = atoi(temp);

			if (l >= m_pTexCube[0]->GetLevelCount())
			{
                EndScene();
				m_pCurrentEffect->End();
				SkipTestRange(uTestNumber, uTestNumber);
				return true;
			}

			offset = m_CubeTLInfo.nIndices*l;
			m_CubeTLInfo.pIndices = m_CubeTLInfo.pIndices + offset;
			pShape0_Info = &m_CubeTLInfo;
			Stride = sizeof(TLVOLVERTEX);
        }
		else if(!_tcsnicmp(sShape, _T("voltlz"), _tcslen("voltlz")))
        {
			LPSTR temp = NULL;
			temp = _tcsninc(sShape, _tcslen("voltlz"));
			int l = atoi(temp);

			if (l >= m_pTexVol[0]->GetLevelCount())
			{
                EndScene();
				m_pCurrentEffect->End();
				SkipTestRange(uTestNumber, uTestNumber);
				return true;
			}

			NumIndices = m_VolTLInfo.nIndices;
			offset = m_VolTLInfo.nIndices*l;
			m_VolTLInfo.nIndices = NumIndices-6*2;
			m_VolTLInfo.pIndices = m_VolTLInfo.pIndices + offset;
			pShape0_Info = &m_VolTLInfo;
			Stride = sizeof(TLVOLVERTEX);
        }
		else if(!_tcsnicmp(sShape, _T("voltlx"), _tcslen("voltlx")))
        {
			LPSTR temp = NULL;
			temp = _tcsninc(sShape, _tcslen("voltlx"));
			int l = atoi(temp);

			if (l >= m_pTexVol[0]->GetLevelCount())
			{
                EndScene();
				m_pCurrentEffect->End();
				SkipTestRange(uTestNumber, uTestNumber);
				return true;
			}

			NumIndices = m_VolTLInfo.nIndices;
			offset = m_VolTLInfo.nIndices*(l+1)-12;
			m_VolTLInfo.nIndices = 6;
			m_VolTLInfo.pIndices = m_VolTLInfo.pIndices + offset;
			pShape0_Info = &m_VolTLInfo;
			Stride = sizeof(TLVOLVERTEX);
        }
		else if(!_tcsnicmp(sShape, _T("voltly"), _tcslen("voltly")))
        {
			LPSTR temp = NULL;
			temp = _tcsninc(sShape, _tcslen("voltly"));
			int l = atoi(temp);

			if (l >= m_pTexVol[0]->GetLevelCount())
			{
                EndScene();
				m_pCurrentEffect->End();
				SkipTestRange(uTestNumber, uTestNumber);
				return true;
			}

			NumIndices = m_VolTLInfo.nIndices;
			offset = m_VolTLInfo.nIndices*(l+1)-6;
			m_VolTLInfo.nIndices = 6;
			m_VolTLInfo.pIndices = m_VolTLInfo.pIndices + offset;
			pShape0_Info = &m_VolTLInfo;
			Stride = sizeof(TLVOLVERTEX);
        }
       	//these are only used with texldp which was removed for cube maps
		/*else if ( !_tcsicmp(sShape, _T("Faces01")) )
        {
			NumIndices = m_CubeTLInfo.nIndices;
			m_CubeTLInfo.nIndices = 12;
			pShape0_Info = &m_CubeTLInfo;
			Stride = sizeof(TLVOLVERTEX);
        }
		else if ( !_tcsicmp(sShape, _T("Faces23")) )
        {
			NumIndices = m_CubeTLInfo.nIndices;
			m_CubeTLInfo.nIndices = 12;
			offset = 12;
			m_CubeTLInfo.pIndices = m_CubeTLInfo.pIndices + offset;
			pShape0_Info = &m_CubeTLInfo;
			Stride = sizeof(TLVOLVERTEX);
        }
		else if ( !_tcsicmp(sShape, _T("Faces45")) )
        {
			NumIndices = m_CubeTLInfo.nIndices;
			m_CubeTLInfo.nIndices = 12;
			offset = 24;
			m_CubeTLInfo.pIndices = m_CubeTLInfo.pIndices + offset;
			pShape0_Info = &m_CubeTLInfo;
			Stride = sizeof(TLVOLVERTEX);
        }*/
						
        // Draw the fist shape
        
        if ( pShape0_Info )
        {
            if ( !SetVertexDeclaration(pShape0_Info->pDecl) ) 
            {
                WriteToLog(_T("SetVertexDeclaration failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
                Fail();
				break;
            }
            
            if (bUseIndices)
			{
				if( !DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, pShape0_Info->nVertices, pShape0_Info->nIndices / 3,
											pShape0_Info->pIndices, FMT_INDEX16, pShape0_Info->pVertices, Stride) )
				{
					WriteToLog(_T("RenderPrimitive failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
					Fail();
					break;
				}
			}
			else
			{
				if( !DrawPrimitiveUP(D3DPT_POINTLIST, pShape0_Info->nVertices, pShape0_Info->pVertices, Stride) )
				{
					WriteToLog(_T("RenderPrimitive failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
					Fail();
					break;
				}
			}
        }
		
        // Draw the second shape
        
        if( pShape1_Info )
        {
            if ( !SetVertexDeclaration(pShape1_Info->pDecl) ) 
            {
                WriteToLog(_T("SetVertexDeclaration failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
                Fail();
				break;
            }			
            if (bUseIndices)
			{
				if ( !DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, pShape1_Info->nVertices, pShape1_Info->nIndices / 3,
											pShape1_Info->pIndices, FMT_INDEX16, pShape1_Info->pVertices, Stride) )
				{
					WriteToLog(_T("RenderPrimitive failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
					Fail();
					break;
				}
			}
			else
			{
				if( !DrawPrimitiveUP(D3DPT_POINTLIST, pShape0_Info->nVertices, pShape0_Info->pVertices, Stride) )
				{
					WriteToLog(_T("RenderPrimitive failed. hr=%s(%x)\n"), m_pD3D->HResultToString(GetLastError()), hr);
					Fail();
					break;
				}
			}
        }

		if(!_tcsnicmp(sShape, _T("textl"), _tcslen("textl")))
        {
			m_TexTLInfo.pIndices -= offset;
        }
		if(!_tcsnicmp(sShape, _T("cubetl"), _tcslen("cubetl")))
		{
			m_CubeTLInfo.pIndices -= offset;
        }
		if(!_tcsnicmp(sShape, _T("voltl"), _tcslen("voltl")))
        {
			m_VolTLInfo.pIndices -= offset;
			m_VolTLInfo.nIndices = NumIndices;
        }
		/*if(!_tcsnicmp(sShape, _T("faces"), _tcslen("faces")))
		{
			m_CubeTLInfo.pIndices -= offset;
			m_CubeTLInfo.nIndices = NumIndices;
        }*/

		if (!m_pCurrentEffect->EndPass()) 
		{
            WriteToLog(_T("EndPass() failed. hr=%s(%x)\n"), i, m_pD3D->HResultToString(GetLastError()), hr); 
			Fail(); 
			break;
        }
    }
    
    EndScene();
    m_pCurrentEffect->End();
	
    return true;
}

// ----------------------------------------------------------------------------

void CPShaderX::Cleanup(void)
{
	UINT i;
	for(i=0; i<m_nTex2D; i++)
	{
		RELEASE(m_pTex2D[i]);
	}
	m_nTex2D = 0;

	for(i=0; i<m_nTexCube; i++)
	{
		RELEASE(m_pTexCube[i]);
	}
	m_nTexCube = 0;

	for(UINT i=0; i<m_nTexVol; i++)
	{
		RELEASE(m_pTexVol[i]);
	}
	m_nTexVol = 0;

	delete [] m_PrecisionInfo.pVertices;
	m_PrecisionInfo.pVertices = NULL;
	delete [] m_PrecisionInfo.pIndices;
	m_PrecisionInfo.pIndices = NULL;
	RELEASE(m_PrecisionInfo.pDecl);

	delete [] m_QuadInfo.pVertices;
	m_QuadInfo.pVertices = NULL;
	delete [] m_QuadInfo.pIndices;
	m_QuadInfo.pIndices = NULL;
	RELEASE(m_QuadInfo.pDecl);

	delete [] m_SphereInfo.pVertices;
	m_SphereInfo.pVertices = NULL;
	delete [] m_SphereInfo.pIndices;
	m_SphereInfo.pIndices = NULL;
	RELEASE(m_SphereInfo.pDecl);

	delete [] m_TinyQuadInfo.pVertices;
	m_TinyQuadInfo.pVertices = NULL;
	delete [] m_TinyQuadInfo.pIndices;
	m_TinyQuadInfo.pIndices = NULL;
	RELEASE(m_TinyQuadInfo.pDecl);

	delete [] m_TexTLInfo.pVertices;
	m_TexTLInfo.pVertices = NULL;
	delete [] m_TexTLInfo.pIndices;
	m_TexTLInfo.pIndices = NULL;
	RELEASE(m_TexTLInfo.pDecl);

	delete [] m_CubeTLInfo.pVertices;
	m_CubeTLInfo.pVertices = NULL;
	delete [] m_CubeTLInfo.pIndices;
	m_CubeTLInfo.pIndices = NULL;
	RELEASE(m_CubeTLInfo.pDecl);

	delete [] m_VolTLInfo.pVertices;
	m_VolTLInfo.pVertices = NULL;
	delete [] m_VolTLInfo.pIndices;
	m_VolTLInfo.pIndices = NULL;
	RELEASE(m_VolTLInfo.pDecl);
}

// ----------------------------------------------------------------------------

bool CPShaderX::TestTerminate(void)
{
	RELEASE(m_pCurrentEffect);

	for(UINT i = 0; i < m_nEffectDesc; i++)
	{
		if ( m_pEffectDesc[ i ].pIndices )
			delete [] m_pEffectDesc[ i ].pIndices;
        SAFEDELETEA( m_pEffectDesc[ i ].sName );
	}
	
	SAFEDELETEA(m_pEffectDesc);
	m_nEffectDesc = 0;
	m_nEffectDescAlloc = 0;

	return CD3DTest::TestTerminate();
}

// ----------------------------------------------------------------------------

bool CPShaderX::ProcessFrame(void)
{
	CSurface *pSurf1 = NULL;
	HRESULT   hr;
	bool      result = true;

    //we run standard ProcessFrame() for all cases but those which try to determine pixel shader precision

	if (false == m_bCheckPrecision)
	{
		result = CD3DTest::ProcessFrame();
		
		//for some cases we changed pixel tolerance

		if (m_fPrevTolPerPixel != m_pFramework->m_ImageCmp.GetTolPerPixel())
			m_pFramework->m_ImageCmp.SetTolPerPixel(m_fPrevTolPerPixel);

		if (m_fPrevTolPerPixelChannel != m_pFramework->m_ImageCmp.GetTolPerPixelChannel())
			m_pFramework->m_ImageCmp.SetTolPerPixelChannel(m_fPrevTolPerPixelChannel);

		return result;
	}

	hr = m_pSrcDevice->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pSurf1);

	if(FAILED(hr))
	{
		WriteToLog("ProcessFrame: GetBackBufferCached failed with result hr=%s(%x).\n",	m_pD3D->HResultToString(hr), hr);
		return false;
	}

    int         nPitch1;
    LOCKED_RECT LockRect1;
    void	    *pBuf1;
	DWORD RedMask1   = pSurf1->m_Mask.dwRedMask;
	DWORD GreenMask1 = pSurf1->m_Mask.dwGreenMask;
	DWORD BlueMask1  = pSurf1->m_Mask.dwBlueMask;
	DWORD amount = 0;

    // Get the Surface data pointer for our subrect
    hr = pSurf1->LockRect( &LockRect1, NULL, NULL );
    if (FAILED(hr))
	{
		WriteToLog("CheckPrimitiveColor: LockRect failed with HResult = %s(%x).\n", m_pD3D->HResultToString(hr), hr);
		RELEASE(pSurf1);
        return false;
	}
	pBuf1 = LockRect1.pBits;
    nPitch1 = LockRect1.iPitch;

	DWORD Width  = m_Options.D3DOptions.nBackBufferWidth;
	DWORD Height = m_Options.D3DOptions.nBackBufferHeight;
	DWORD color = RGBA_MAKE(0xff, 0xff, 0, 0xff);
	void *pColor1;
	DWORD c1;

	m_uExponentPrecision = 0;
	m_uMantissaPrecision = 0;

	for (UINT y = 0; y < Height; y++)		// Height loop
	{
		pColor1 = (char*)pBuf1 + (nPitch1 * y);

		UINT x;
		for (x = 0; x < Width; x++)
		{
			c1 = pSurf1->ReadColor(&pColor1);
				
			//if already get exponent then don't need to compare any more
			if (m_uExponentPrecision == 0)
			if ( (RGBA_GETRED(c1) & RedMask1) != (RGBA_GETRED(color) & RedMask1) )
				m_uExponentPrecision = y*Width + x;

			//if already get mantissa then don't need to compare any more
			if (m_uMantissaPrecision == 0)
			if ( (RGBA_GETGREEN(c1) & GreenMask1) != (RGBA_GETGREEN(color) & GreenMask1) )
				m_uMantissaPrecision = y*Width + x;

			if (y*Width + x >= RANGE)
				break;
		}

		if (y*Width + x >= RANGE)
			break;
	}

	hr = pSurf1->UnlockRect();
    if (FAILED(hr))
    {
	    WriteToLog("CheckPrimitiveColor: UnlockRect failed with HResult = %s(%x).\n", m_pD3D->HResultToString(hr), hr);
		RELEASE(pSurf1);
        return false;
    }
	RELEASE(pSurf1);

	UINT i = 1;

	while (m_uExponentPrecision > 0)
	{
		m_uExponentPrecision >>= 1;
		i++;
	}
	m_uExponentPrecision = i;

	WriteToLog("exponent bits = %d\n", m_uExponentPrecision);
	WriteToLog("mantissa bits = %d\n", m_uMantissaPrecision);

	if (m_uExponentPrecision >= 7 && m_uMantissaPrecision >= 16)
		Pass();
	else
		Fail();

	m_bCheckPrecision = false;
	
	return result;
}

// ----------------------------------------------------------------------------

bool CPShaderX::CreatePrecisionBuffers()
{
	DWORD   Pool  = POOL_MANAGED;
	DWORD   Usage = FMWK_DEFAULT;
	
	TVERTEX  *pVB   = NULL;
	PWORD      pIB   = NULL;
	bool       bResult = true;
	
	//create and fill out vertex buffer
	//normals are going to be uses to store variable based on which we'll find out shader precision
	//n.x and n.y are used to figure out shader's exponent precision
	//n.z and n.w are used to figure out shader's mantissa precision
	
	pVB = new TVERTEX[RANGE];
        
    if (NULL == pVB)
		return false;

	float x, y;
	float nx = 1.f, ny = 2.f;
	float epsilon = 0.5f;
	float nz = 1.f, nw = nz + epsilon;
     
    for (UINT i = 0; i < RANGE; i++)
	{
		x = (float)(i%m_pCurrentMode->nBackBufferWidth);
		y = (float)(i/m_pCurrentMode->nBackBufferWidth);
					
		pVB[i] = TVERTEX(D3DXVECTOR3( x, y, 0.f), D3DXVECTOR4(nx, ny, nz, nw));

		nx *= 2.f;
		ny *= 2.f;

		epsilon /= 2.f;
		nw = 1.f + epsilon;
	}

	//create and fill out index buffer used
	pIB = new WORD[3 * RANGE];

    if (NULL == pIB)
		return false;
    	
	for (UINT i = 0; i < RANGE; i++)
	{
		pIB[3*i + 0] = (WORD)i;
		pIB[3*i + 1] = (WORD)i;
		pIB[3*i + 2] = (WORD)i;
	}
    	
    m_PrecisionInfo.nVertices = RANGE;
    m_PrecisionInfo.nIndices = 3*
		RANGE;
    m_PrecisionInfo.pVertices = pVB;
    m_PrecisionInfo.pIndices = pIB;

	VERTEXELEMENT declT[] = 
	{
		{0,  0, (DECLTYPE)D3DDECLTYPE_FLOAT4, DECLMETHOD_DEFAULT, DECLUSAGE_POSITIONT, 0},
        {0, 16, (DECLTYPE)D3DDECLTYPE_FLOAT4, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  0},
		//{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD,  1},
        DECL_END()
	};
    	
    if(!CreateVertexDeclaration(declT, &(m_PrecisionInfo.pDecl)))
    {
		WriteToLog("CreatePrecisionBuffers: CreateVertexDeclaration() failed.\n");
        return false;
    }

	return true;
}

// ----------------------------------------------------------------------------

bool CPShaderX::CreateTextures()
{
    if (m_uCommonTextureFormats == 0)
        return true;    // No textures;

	FORMAT *pTexFormat = &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
	DWORD   Pool  = POOL_MANAGED;
	DWORD   Usage = FMWK_DEFAULT;
	
	CImage   **ppImages = NULL;
	TLVERTEX  *pTexVB   = NULL;
	PWORD      pTexIB   = NULL;
	UINT       uWidth   = TEXSIZE;
	UINT       uHeight  = TEXSIZE;
	UINT       uLevels  = 6;
	UINT	   uStripes = 8;
	CAPS       caps;
	bool       bResult = true;
	
	m_pSrcDevice->GetDeviceCaps(&caps); 

	if (KeySet("nomiptex") || !(m_pSrcDevice->GetCaps()->dwTextureCaps & D3DPTEXTURECAPS_MIPMAP))
		uLevels = 1;

	if ( pTexFormat->d3dfFormat == FMT_A1 )
		uLevels = 1;

	if (m_pD3D->GetRuntimeVersion() >= 9.029f && FAILED(m_pD3D->CheckDeviceFormat(m_pSrcDevice->GetAdapterID(), m_pSrcDevice->GetDevType(), m_pCurrentMode->pDisplay->Format, USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, *pTexFormat)))
	{
		uLevels = 1;

		for(int i = 0; i < 16; i++)
		{
			SetSamplerState(i, SAMP_ADDRESSU, D3DTADDRESS_CLAMP);
			SetSamplerState(i, SAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		}
	}

	m_TexSizes[0] = (float) uWidth;  //Width  of the primitive
    m_TexSizes[1] = (float) uHeight; //height of the primitive
    m_TexSizes[2] = (float) uWidth;  //Width  of the texture
    m_TexSizes[3] = (float) uHeight; //height of the texture

    //create and fill out vertex buffer used
	
	pTexVB = new TLVERTEX[4*uLevels];
        
    if (NULL == pTexVB)
		return false;
        
    for (UINT level = 0; level < uLevels; level++)
	{
		float x1 = (float)(m_pCurrentMode->nBackBufferWidth/2 - uWidth/(1<<(level+1)));
		float y1 = (float)(m_pCurrentMode->nBackBufferHeight/2 - uHeight/(1<<(level+1)));
		float x2 = (float)(x1 + uWidth/(1<<level));
		float y2 = (float)(y1 + uHeight/(1<<level));
    		
		pTexVB[0+4*level] = TLVERTEX(D3DXVECTOR3( x1, y1, 0.0f), 0xffffffff, D3DXVECTOR4(0.00f, 0.00f, 0.f, 0.f), D3DXVECTOR4(0.99f, 0.99f, 0.f, 1.f), D3DXVECTOR4(0.00f, 0.00f, 0.f, 5.f));
		pTexVB[1+4*level] = TLVERTEX(D3DXVECTOR3( x2, y1, 0.0f), 0xffffffff, D3DXVECTOR4(0.99f, 0.00f, 0.f, 0.f), D3DXVECTOR4(0.00f, 0.99f, 0.f, 1.f), D3DXVECTOR4(0.00f, 0.99f, 0.f, 5.f));
		pTexVB[2+4*level] = TLVERTEX(D3DXVECTOR3( x1, y2, 0.0f), 0xffffffff, D3DXVECTOR4(0.00f, 0.99f, 0.f, 0.f), D3DXVECTOR4(0.99f, 0.00f, 0.f, 1.f), D3DXVECTOR4(0.99f, 0.00f, 0.f, 5.f));
		pTexVB[3+4*level] = TLVERTEX(D3DXVECTOR3( x2, y2, 0.0f), 0xffffffff, D3DXVECTOR4(0.99f, 0.99f, 0.f, 0.f), D3DXVECTOR4(0.00f, 0.00f, 0.f, 1.f), D3DXVECTOR4(0.99f, 0.99f, 0.f, 5.f));
	}

	for(UINT i = 0; i < 4*uLevels; i++)
	{
		pTexVB[i].x -= 0.5f;
		pTexVB[i].y -= 0.5f;
	}

	m_vPosLimits[0]