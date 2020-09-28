/*++

Copyright (c) Microsoft Corporation

Module Name:

    osloader.h

Abstract:

    Header file for the Windows OS Loader.

Environment:

    Boot

--*/

#pragma once

//
// ------------------------------------------------------------------- Includes
//

#include <bootlib.h>
#include "baerror.h"
#include "msg.h"
#include "bootstatus.h"
#include <ntacpi.h>
#include <ntverp.h>
#include <ntiodump.h>
#include <hdlsblk.h>
#include <resminfo.h>
#include <wmdinfo.h>
#include <sqminfo.h>
#include "inbv.h"
#include "bgfx.h"
#include <sha.h>
#include <ntstrsafe.h>
#include <symcrypt.h>
#include <esrt.h>
#include <secureboot.h>
#include <apisetp.h>
#include <mfgmode.h>
#include "bltracelogging.h"
#include <blblobsealing.h>
#include "hypercall.h"

#undef ROUND_UP            // hive.h redefines ROUND_UP.
#include "cmp.h"

// VSM IDK headers.
#include <bcrypt.h>
#include <vsmidk.h>
#include <BlVsmKeys.h>
#include <blsvn.h>
#include <vsml.h>

//
// ---------------------------------------------------------------- Definitions
//

#define KERNEL_DEFAULT_NAME L"ntoskrnl.exe"
#define KERNEL_PAE_NAME     KERNEL_DEFAULT_NAME
#define HAL_DEFAULT_NAME    L"hal.dll"
#define KD_DEFAULT_NAME     L"kd.dll"
#define KD_SERIAL_NAME      L"kdcom.dll"
#define KD_1394_NAME        L"kd1394.dll"
#define KD_USB_NAME         L"kdusb.dll"
#define KD_NET_NAME         L"kdnet.dll"
#define KD_LOCAL_NAME       L"kd.dll"
#define KD_FRIENDLY_NAME    KD_SERIAL_NAME
#define KD_EXTENSION_FRIENDLY_NAME KD_EXTENSION_DEFAULT_NAME
#define KD_HV_COM_NAME      L"kdhvcom.dll"
#define KD_HV_1394_NAME     L"kdhv1394.dll"
#define UPDATE_DEFAULT_NAME L"mcupdate.dll"
#define UPDATE_NAME_PREFIX  L"mcupdate_"
#define DRVMAIN_NAME        L"drvmain.sdb"
#define ACPI_TABLE_NAME     L"acpitabl.dat"
#define SYSTEM_PATH         L"system32"
#define DRVMAIN_PATH        L"AppPatch"
#define INF_PATH            L"inf"
#define SYSTEM_HIVE_PATH    L"system32\\config\\"
#define SYSTEM_HIVE_NAME    L"system"
#define SYSTEM_HIVE_LOGNAME1 L"system.log1"
#define SYSTEM_HIVE_LOGNAME2 L"system.log2"
#define FPSWA_NAME          L"Drivers\\fpswa.efi"
#define HIVE_LOG1_EXTENSION L".LOG1"
#define HIVE_LOG2_EXTENSION L".LOG2"
#define HARDWARE_POLICY_NAME L"hwpolicy.sys"
#define ELAM_HIVE_NAME      L"elam"
#define SECUREKERNEL_NAME L"securekernel.exe"
#define APISET_SCHEMA_NAME  API_SET_STRING_U (API_SET_SCHEMA_NAME) PS_SYSTEM_DLL_SUFFIX
#define CI_REVOCATION_LIST_PATH SYSTEM_PATH L"\\CodeIntegrity\\"
#define CI_REVOCATION_LIST_NAME L"driver.stl"


typedef struct _ADVANCED_BOOT_OPTION {
    ULONG AdvancedOption;
    PWCHAR AdvancedOptionString;
} ADVANCED_BOOT_OPTION, *PADVANCED_BOOT_OPTION;

#define OSL_ERROR_CRITICAL_DRIVER               0x00000001
#define OSL_ERROR_LOAD_IMPORT                   0x00000002
#define OSL_ERROR_IMPORT                        0x00000003
#define OSL_ERROR_NO_ACPI_DETECTED              0x00000004
#define OSL_ERROR_REGISTRY                      0x00000005
#define OSL_ERROR_KERNEL                        0x00000006
#define OSL_ERROR_HAL                           0x00000007
#define OSL_ERROR_KD_DLL                        0x00000008
#define OSL_ERROR_NLS_DATA                      0x00000009
#define OSL_ERROR_INVALID_RSDP                  0x0000000A
#define OSL_ERROR_CODE_INTEGRITY_INITIALIZATION 0x0000000B
#define OSL_ERROR_UPDATE_DLL                    0x0000000C
#define OSL_ERROR_MEMORY_MAP_CONFLICT           0x0000000D
#define OSL_ERROR_OS_EXTENSION                  0x0000000E
#define OSL_ERROR_BAD_BOOT                      0x0000000F
#define OSL_ERROR_BAD_SHUTDOWN                  0x00000010
#define OSL_ERROR_MANUFACTURING_MODE            0x00000011
#define OSL_ERROR_MANUFACTURING_MODE_SECURITY_LOCK_SCREEN      0x00000012
#define OSL_ERROR_MANUFACTURING_MODE_SECURITY_BITLOCKER        0x00000013
#define OSL_ERROR_MANUFACTURING_MODE_SECURITY_RESET_PROTECTION 0x00000014

//
// The numbers in this table must match exactly to the options listed
// in the osloader.xsl "osboot-advops" section.
//

#define SAFEMODE                  0
#define SAFEMODE_NET              1
#define SAFEMODE_CMD              2
#define RECOVERY_SEQUENCE         3
#define BOOT_LOGGING              4
#define BASICDISPLAY_MODE         5
#define LAST_KNOWN_GOOD           6
#define DS_REPAIR_MODE            7
#define DEBUG_MODE                8
#define DISABLE_CRASH_AUTOREBOOT  9
#define DISABLE_CODE_INTEGRITY   10
#define BOOT_NORMAL              11
#define NOGUIBOOT                12
#define DISABLE_ELAM_DRIVERS     13
#define MAX_ADVANCED_BOOT_OPTION 14

//
// Define memory types used by the OS Loader.
//

#define OSL_MEMORY_TYPE(_x) (MEMORY_APPLICATION_TYPE_DEFINABLE_START + (_x))

#define OSL_MEMORY_TYPE_KERNEL_DATA_STRUCTURE \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x01))

#define OSL_MEMORY_TYPE_SYSTEM_CODE \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x02))

#define OSL_MEMORY_TYPE_BOOT_DRIVER \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x03))

#define OSL_MEMORY_TYPE_MISC_MODULE \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x04))

#define OSL_MEMORY_TYPE_NLS_DATA \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x05))

#define OSL_MEMORY_TYPE_REGISTRY_DATA \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x06))

#define OSL_MEMORY_TYPE_KERNEL_INIT_DATA \
    GET_MEMORY_TYPE(MEMORY_CLASS_APPLICATION, OSL_MEMORY_TYPE(0x07))

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

