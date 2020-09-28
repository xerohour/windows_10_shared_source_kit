/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    utils.h

Abstract:

    This module implements the utility functions for the PNP subsystem.

Author:

    Santosh Jodh (santoshj) 08-10-2004

Environment:

    KernelMode only.

Revision History:

--*/

#pragma once

//
// Data structure for each entry in the device reference table.
//

typedef struct _PNP_DEVICE_REFERENCE {

    //
    // PDO.
    //

    PDEVICE_OBJECT  DeviceObject;

    //
    // Pointer to instance path for the PDO devnode.
    //

    PUNICODE_STRING DeviceInstance;

} PNP_DEVICE_REFERENCE, *PPNP_DEVICE_REFERENCE;

typedef struct _PNP_LOCK_MOUNTABLE_DEVICE_CONTEXT{

    PDEVICE_OBJECT MountedDevice;
    PDEVICE_OBJECT FsDevice;

} PNP_LOCK_MOUNTABLE_DEVICE_CONTEXT, *PPNP_LOCK_MOUNTABLE_DEVICE_CONTEXT;

//
//  Current PNP_LOCATION_INTERFACE version.
//
#define PNP_LOCATION_INTERFACE_VERSION  0x1

//
// Optional device location path separator string
//
// i.e. TEXT("#")   --> PCIROOT(0)#PCI(1100)
//      TEXT("")    --> PCIROOT(0)PCI(1100)
//      TEXT("FOO") --> PCIROOT(0)FOOPCI(1100)
//
// NOTE: The resulting path will be used as a single registry key, so NEVER use
// the NT path separator string RtlNtPathSeperatorString (L"\\"), or any string
// that contains a path separator character (verify with RtlIsNtPathSeparator).
//
#define _DEVICE_LOCATION_PATH_SEPARATOR_STRING    TEXT("#")

//
// Optional device location path root terminator prefix character
//
// Each location string will be checked to see if it is pre-fixed by this
// character.  If so, then the string will be considered a "root" string.
//
// When a root location string is encountered while building a location
// path, it will be considered the start of the location path, replacing any
// parts of the path that were generated using location strings from devices
// higher in the device tree.
//
// A root location string is handled as if STATUS_TRANSLATION_COMPLETE were
// returned for only some location strings within the list returned by a
// single call to GetLocationStrings.  It allows the "translation complete"
// designation to be placed on some locations strings within the list, while
// still allowing other non-prefixed strings to continue normal translation
// up the tree.
//
// Example: For a root string prefix char of:
//
//   TEXT('@')
//
// And returned location strings of:
//
//   [0]: "ABC(0)"
//   [1]: "ABC(1)", "@XYZ(1)"
//   [2]: "ABC(2)", "XYZ(2)"
//
// The following location path strings WILL be generated:
//
//   "ABC(0)#ABC(1)#ABC(2)"
//   "ABC(0)#ABC(1)#XYZ(2)"
//   "XYZ(1)#ABC(2)"
//   "XYZ(1)#XYZ(2)"
//
// The following location path strings will NOT be generated:
//
//   "ABC(0)#XYZ(1)#ABC(2)" <-- @XYZ(1) string is not used as root
//   "ABC(0)#XYZ(1)#XYZ(2)" <-- @XYZ(1) string is not used as root
//
#define _DEVICE_LOCATION_PATH_ROOT_PREFIX_CHAR TEXT('@')

//
// Optional device location base string prefix decoration
//
// Each location string will be checked to see if it is pre-fixed by this
// character.  If so, then the string will be considered a "base" string.
//
// A base location string is expected to be reported by some underlying
// component that has consistent knowledge of the device's location,
// very close to the hardware reported representation of the device, and
// independent of specific the bus drivers that may be used to enumerate the
// device along the way.
//
// The canonical example of a component that could report a base location
// string is the operating system's ACPI driver, using information from
// Device elements of the system's hierarchical ACPI DSDT AML table.
// For example: \"_SB"\"PCI0"\"EUSB"\"HUBN".
//
// Note that there is expected to be at most one base location string
// reported at each level. If multiple base location strings are reported at
// the same level, there is no attempt to determine the relationship between
// them.  All base strings are currently considered to be interchangeable.
//
// Base location strings will only be used to generate a location path that
// includes ONLY base strings, up until the last level where a base string
// was reported.
//
// Note that because of this, it is expected that once some device has a
// base location string reported for it, every ancestor of that device will
// also have a base location string. There should be no "gaps" or "early
// drop offs" in base location strings up the device tree, otherwise the
// location path that uses ONLY base strings to that last level will be
// considered "unreachable" and never be generated.
//
// Example: For a base string prefix char of:
//
//   TEXT('!')
//
// And returned location strings of:
//
//   [0]: "!ABC(0)", "XYZ(0)"
//   [1]: "!ABC(1)", "XYZ(1)"
//   [2]:            "XYZ(2)"
//
// The following location path strings WILL be generated:
//
//   "ABC(0)#ABC(1)#XYZ(2)"
//   "XYZ(0)#XYZ(1)#XYZ(2)"
//
// The following location path strings will NOT be generated:
//
//   "ABC(0)#XYZ(1)#XYZ(2)" <-- ABC(1) base string not used for base-only start
//   "XYZ(0)#ABC(1)#XYZ(2)" <-- ABC(0) base string not used for base-only start
//
#define _DEVICE_LOCATION_PATH_BASE_PREFIX_CHAR TEXT('!')


//
// Prototype verification callback routine for PnpOpenFirstMatchingSubKey
//

typedef
BOOLEAN
(*PPNP_MATCHING_SUBKEY_CALLBACK)(
    __in  HANDLE          MatchingKeyHandle
    );

LOGICAL
PnpRegSzToString(
    __in_bcount(RegSzLength)  PWCHAR RegSzData,
    __in  ULONG  RegSzLength,
    __out_opt PULONG StringLength  ,
    __deref_opt_out_bcount(*StringLength) PWSTR  *CopiedString
    );

PVOID
NTAPI
PnpAllocateGenericTableEntry (
    __in PRTL_GENERIC_TABLE Table,
    __in CLONG ByteSize
    );

VOID
NTAPI
PnpFreeGenericTableEntry (
    __in PRTL_GENERIC_TABLE Table,
    __in PVOID Buffer
    );

VOID
PnpInitializeDeviceReferenceTable (
    VOID
    );

VOID
PnpLogEvent(
    __in PUNICODE_STRING InsertionString1,
    __in_opt PUNICODE_STRING InsertionString2,
    __in NTSTATUS Status,
    __in_bcount_opt(DumpDataSize) PVOID DumpData,
    __in_opt ULONG DumpDataSize
    );

NTSTATUS
PnpQueryID(
    __in PDEVICE_NODE DeviceNode,
    __in BUS_QUERY_ID_TYPE IDType,
    __deref_out_bcount(*IDLength) PWCHAR *ID,
    __out PULONG IDLength
    );

NTSTATUS
PnpQueryDeviceID(
    __in PDEVICE_NODE DeviceNode,
    __deref_out PWSTR *BusID,
    __deref_out PWSTR *DeviceID
    );

NTSTATUS
PnpQueryBusInformation(
    __in PDEVICE_NODE DeviceNode
    );

NTSTATUS
PnpGenerateDeviceIdsHash(
    __in_opt PCZZWSTR HardwareIds,
    __in_opt PCZZWSTR CompatibleIds,
    __out PULONG DeviceIdsHash
    );

NTSTATUS
PnpSaveDeviceCapabilities (
    __in PDEVICE_NODE DeviceNode,
    __in PDEVICE_CAPABILITIES Capabilities,
    __in BOOLEAN NewDevice
    );

NTSTATUS
PnpBusTypeGuidInitialize (
    VOID
    );

USHORT
PnpBusTypeGuidGetIndex(
    __in LPGUID BusTypeGuid
    );

NTSTATUS
PnpBusTypeGuidGet(
    __in USHORT Index,
    __inout LPGUID BusTypeGuid
    );

LOGICAL
PnpIsDuplicateDevice (
    __in PCM_RESOURCE_LIST Configuration1,
    __in PCM_RESOURCE_LIST Configuration2,
    __in_opt PHAL_BUS_INFORMATION BusInfo1 ,
    __in_opt PHAL_BUS_INFORMATION BusInfo2
    );

NTSTATUS
IopAllocateUnicodeString(
    __inout PUNICODE_STRING String,
    __in USHORT Length
    );

NTSTATUS
PnpConcatenateUnicodeStrings (
    __out PUNICODE_STRING Destination,
    __in  PUNICODE_STRING String1,
    __in_opt PUNICODE_STRING String2
    );

NTSTATUS
PnpGetDeviceInstanceCsConfigFlags(
    __in PUNICODE_STRING DeviceInstance,
    __in ULONG ProfileId,
    __out PULONG CsConfigFlags
    );

NTSTATUS
PnpGetServiceInstanceCsConfigFlags(
    __in PUNICODE_STRING ServiceKeyName,
    __in ULONG Instance,
    __out PULONG CsConfigFlags
    );

NTSTATUS
PnpRegMultiSzToUnicodeStrings (
    __in PKEY_VALUE_FULL_INFORMATION KeyValueInformation,
    __deref_out_ecount(*UnicodeStringCount) PUNICODE_STRING *UnicodeStringList,
    __out PULONG UnicodeStringCount
    );

VOID
PnpFreeUnicodeStringList (
    __in_ecount(StringCount) PUNICODE_STRING UnicodeStringList,
    __in ULONG StringCount
    );

ULONG
PnpDetermineResourceListSize (
    __in_opt PCM_RESOURCE_LIST ResourceList
    );

PDEVICE_OBJECT
PnpDeviceObjectFromDeviceInstance(
    __in PUNICODE_STRING  DeviceInstance
    );

PDEVICE_OBJECT
IopDeviceObjectFromSymbolicName(
    __in PUNICODE_STRING SymbolicLinkName
    );

ULONG
IopGetSessionIdFromSymbolicName(
    __in PUNICODE_STRING  SymbolicLinkName
    );

NTSTATUS
PnpMapDeviceObjectToDeviceInstance(
    __in PDEVICE_OBJECT   DeviceObject,
    __in PUNICODE_STRING  DeviceInstance
    );

NTSTATUS
PnpDeviceObjectToDeviceInstance(
    __in PDEVICE_OBJECT DeviceObject,
    __out PHANDLE DeviceInstanceHandle,
    __in  ACCESS_MASK DesiredAccess
    );

LOGICAL
PnpIsDeviceInstanceEnabled(
    __in_opt HANDLE DeviceInstanceHandle,
    __in PUNICODE_STRING DeviceInstance,
    __in LOGICAL Disable
    );

NTSTATUS
PnpCleanupDeviceRegistryValues (
    __in PUNICODE_STRING InstancePath
    );

NTSTATUS
PnpGetDeviceResourcesFromRegistry (
    __in PDEVICE_OBJECT DeviceObject,
    __in ULONG ResourceType,
    __in ULONG Preference,
    __deref_out_bcount(*Length) PVOID *Resource,
    __out PULONG Length
    );

NTSTATUS
PnpReadDeviceConfiguration(
    __in HANDLE Handle,
    __in ULONG Flags,
    __deref_out_bcount(*Length) PCM_RESOURCE_LIST *CmResource,
    __out PULONG Length
    );

NTSTATUS
PnpFilterResourceRequirementsList (
    __in PIO_RESOURCE_REQUIREMENTS_LIST IoList,
    __in PCM_RESOURCE_LIST CmList,
    __inout PIO_RESOURCE_REQUIREMENTS_LIST *FilteredList,
    __out PLOGICAL ExactMatch
    );

NTSTATUS
PnpMergeFilteredResourceRequirementsList (
    __in PIO_RESOURCE_REQUIREMENTS_LIST IoList1,
    __in PIO_RESOURCE_REQUIREMENTS_LIST IoList2,
    __inout PIO_RESOURCE_REQUIREMENTS_LIST *MergedList
    );

PIO_RESOURCE_REQUIREMENTS_LIST
PnpCmResourcesToIoResources (
    __in ULONG SlotNumber,
    __in PCM_RESOURCE_LIST CmResourceList,
    __in ULONG Priority
    );

NTSTATUS
PnpRestartDeviceNode(
    __in PDEVICE_NODE DeviceNode
    );

LOGICAL
PnpIsChainDereferenced (
    __in_ecount(Count) PDEVICE_OBJECT *DeviceObjects,
    __in ULONG Count,
    __in LOGICAL Query,
    __in LOGICAL CleanStack,
    __out_opt PDEVICE_OBJECT *VetoingDevice
    );

LOGICAL
PnpIsAnyDeviceInstanceEnabled(
    __in PUNICODE_STRING ServiceKeyName,
    __in_opt HANDLE ServiceHandle
    );

NTSTATUS
PnpQueryAndSaveDeviceNodeCapabilities (
    __in PDEVICE_NODE DeviceNode
    );

VOID
PnpDisableDevice(
    __in PDEVICE_NODE   DeviceNode,
    __in ULONG          Problem
    );

PDEVICE_OBJECT
PnpFindMountableDevice (
    __in PDEVICE_OBJECT DeviceObject
    );

RTL_GENERIC_COMPARE_RESULTS
NTAPI
PnpCompareInstancePath (
    __in PRTL_GENERIC_TABLE Table,
    __in PVOID FirstStruct,
    __in PVOID SecondStruct
    );

ULONG
PnpFixupID(
    __in __nullnullterminated PWSTR ID,
    __in ULONG MaxIDLength,
    __in LOGICAL Multi,
    __in ULONG AllowedSeparators,
    __in_opt PUNICODE_STRING LogString
    );

PDEVICE_OBJECT
PnpFindMountableDevice (
    __in PDEVICE_OBJECT DeviceObject
    );

VOID
PnpLockMountableDevice (
    __in PDEVICE_OBJECT DeviceObject
    );

VOID
PnpUnlockMountableDevice (
    __in PDEVICE_OBJECT DeviceObject
    );

PDEVICE_OBJECT
PnpMarkDeviceForRemove (
    __in PDEVICE_OBJECT DeviceObject,
    __in LOGICAL Set,
    __out PPNP_LOCK_MOUNTABLE_DEVICE_CONTEXT Context
    );

#define PnpMarkDeviceStackStartPending(d, s) \
    PnpUpdateExtensionFlags(&(d), 1, FALSE, s, DOE_START_PENDING)

#define PnpQueryInstanceID(dn, id, l) PnpQueryID(dn, BusQueryInstanceID, id, l)

#define PnpQueryHardwareIDs(dn, id, l) PnpQueryID(dn, BusQueryHardwareIDs, id, l)

#define PnpQueryCompatibleIDs(dn, id, l) PnpQueryID(dn, BusQueryCompatibleIDs, id, l)

#define PnpQuerySerialNumber(dn, id)    \
    PnpIrpQueryID((dn)->PhysicalDeviceObject, BusQueryDeviceSerialNumber, id)

#define PnpQueryContainerID(dn, id)     \
    PnpIrpQueryID((dn)->PhysicalDeviceObject, BusQueryContainerID, id)

#define PnpQueryDeviceDescription(dn, desc)         \
    PnpQueryDeviceText((dn)->PhysicalDeviceObject, DeviceTextDescription, PsDefaultSystemLocaleId, desc)

#define PnpQueryDeviceLocationInformation(dn, loc)  \
    PnpQueryDeviceText((dn)->PhysicalDeviceObject, DeviceTextLocationInformation, PsDefaultSystemLocaleId, loc)

extern SYSTEM_HIVE_LIMITS PnpSystemHiveLimits;
extern BOOLEAN PnpSystemHiveTooLarge;

_Must_inspect_result_
NTSTATUS
PnpValidateMultiSz(
    __in_ecount_opt(MaxLength) PCZZWSTR InString,
    size_t MaxLength
    );

_Must_inspect_result_
NTSTATUS
PnpAllocatePWSTR(
    __in_ecount_opt(MaxLength) PCWSTR InString,
    size_t MaxLength,   // Length of the input string
    ULONG Tag,
    __deref_out PWSTR* OutString
    );

NTSTATUS
PnpConcatPWSTR(
    size_t MaxLength,   // Max length of the concatinated string
    ULONG Tag,
    __deref_out PWSTR* OutString,
    size_t InputStringCount,
    ...
    );

_Must_inspect_result_
NTSTATUS
PnpGetMultiSzLength(
    __in_ecount(MaxLength) PCZZWSTR MultiSz,
    size_t MaxLength,
    size_t *MultiSzLength
    );

BOOLEAN
PnpCompareMultiSz(
    __in PCZZWSTR MultiSz1,
    __in PCZZWSTR MultiSz2,
    __in BOOLEAN CaseInsensitive
    );

_Must_inspect_result_
NTSTATUS
PnpAllocateMultiSZ(
    __in_ecount_opt(MaxLength) PCZZWSTR InString,
    size_t MaxLength,   // Length of the input string
    ULONG Tag,
    __deref_out PZZWSTR* OutString
    );

_Must_inspect_result_
NTSTATUS
PnpCopyDevProperty(
    __in const DEVPROPERTY *InProperty,
    ULONG Tag,
    __in PDEVPROPERTY OutProperty
    );

VOID
PnpFreeDevProperty(
    __in PDEVPROPERTY Property,
    ULONG Tag
    );

_Must_inspect_result_
NTSTATUS
PnpCopyDevPropertyArray(
    ULONG InPropretyCount,
    __in_ecount_opt(InPropretyCount) const DEVPROPERTY *InProperties,
    ULONG Tag,
    __out PULONG OutPropertyCount,
    __deref_out_ecount(*OutPropertyCount) PDEVPROPERTY* OutProperties
    );

VOID
PnpFreeDevPropertyArray(
    ULONG PropertyCount,
    __in_ecount_opt(PropertyCount) PDEVPROPERTY Properties,
    ULONG Tag
    );

NTSTATUS
PnpUnicodeStringToWstr(
    __deref_out PWSTR* OutString,
    __out_opt PULONG OutStringMaximumSize,
    __in const  PUNICODE_STRING InUnicodeString
    );

VOID
PnpUnicodeStringToWstrFree(
    __in_opt PWSTR InFreeString,
    __in_opt const PUNICODE_STRING InUnicodeString
    );

_Must_inspect_result_
BOOLEAN
PnpIsValidGuidString(
    __in PCWSTR pGuidString
    );

_Must_inspect_result_
BOOLEAN
PnpIsNullGuid(
    __in LPCGUID Guid
    );

_Must_inspect_result_
BOOLEAN
PnpIsNullGuidString(
    __in PCWSTR GuidString
    );

NTSTATUS
PnpStringFromGuid(
    __in LPCGUID Guid,
    __out_ecount(GuidStringLength) PWSTR GuidString,
    __in ULONG GuidStringLength
    );

_Must_inspect_result_
NTSTATUS
PnpGetCallerSessionId(
    __out PULONG SessionId
    );

NTSTATUS
PnpCheckDeviceIdsChanged(
    __in PDEVICE_NODE DeviceNode,
    __in_opt HANDLE DeviceInstanceKey,
    __in_bcount_opt(IdListSize) PWSTR IdList,
    __in ULONG IdListSize,
    __in BOOLEAN CheckHardwareIds,
    __out PBOOLEAN IdsChanged
    );

VOID
PnpInitializeSessionId(
    __in PDEVICE_NODE DeviceNode,
    __in HANDLE InstanceKey,
    __in BOOLEAN NewDevice
    );

_Must_inspect_result_
NTSTATUS
PnpGetObjectProperty(
    __in ULONG Tag,
    __in ULONG InitialBufferSize,
    __in PCWSTR ObjectName,
    __in PNP_OBJECT_TYPE ObjectType,
    __in_opt HANDLE ObjectBaseKey,
    __in_opt PCWSTR PropertyLocaleName,
    __in const DEVPROPKEY *PropertyKey,
    __out DEVPROPTYPE *PropertyType,
    __deref_out_bcount_opt(*PropertySize) PVOID* PropertyBuffer,
    __out_opt PULONG PropertySize,
    __in ULONG Flags
    );

NTSTATUS
PnpConvertDevpropcompkeyArrayToString(
    __in_ecount(CompKeyArrayLength) const DEVPROPCOMPKEY * CompKeyArray,
    __in ULONG CompKeyArrayLength,
    __out_bcount_opt(CompKeyArrayStringSize) PWSTR CompKeyArrayString,
    __in ULONG CompKeyArrayStringSize,
    __out_opt PULONG RequiredSize
    );

KINLINE
VOID
PipUpdatePostStartCharacteristics(
    __in PDEVICE_OBJECT PhysicalDeviceObject
    );

NTSTATUS
IopReplaceSeperatorWithPound(
    __inout PUNICODE_STRING OutString,
    __in PUNICODE_STRING    InString
    );

NTSTATUS
PnpQueryInterface(
    __in  PDEVICE_OBJECT  DeviceObject,
    __in  CONST GUID *    InterfaceGuid,
    __in  USHORT          InterfaceVersion,
    __in  USHORT          InterfaceSize,
    __in_opt  PVOID       InterfaceSpecificData,
    __out_bcount(InterfaceSize) PINTERFACE      Interface
    );

NTSTATUS
PnpGetDeviceLocationStrings(
    __in        PDEVICE_NODE    DeviceNode,
    __deref_out_ecount(*DeviceLocationStringsLength) PZZWSTR *DeviceLocationStrings,
    __out       PULONG          DeviceLocationStringsLength
    );

NTSTATUS
PnpOpenFirstMatchingSubKey(
    __in __nullnullterminated PWCHAR MultiSzList,
    __in      HANDLE          RootHandle,
    __in      ACCESS_MASK     DesiredAccess,
    __in_opt  PPNP_MATCHING_SUBKEY_CALLBACK  MatchingSubkeyCallback,
    __out     PHANDLE         MatchingKeyHandle,
    __out_opt PWSTR           *MatchingKeyName
    );

NTSTATUS
PnpCheckDriverDependencies(
    __in PCZZWSTR Dependencies,
    __out PBOOLEAN DependenciesSatisfied,
    __out_opt PBOOLEAN GenericBootDependency
    );

NTSTATUS
PnpGetStableSystemBootTime(
    __out PLARGE_INTEGER SystemBootTime
    );

VOID
PnpLogDeviceConflictingResource(
    _In_ PPI_RESOURCE_ARBITER_ENTRY  ArbiterEntry,
    _In_ PVOID ArbiterInstance
    );

VOID
PnpLogDeviceRebalanceResult(
    _In_ PDEVICE_NODE  DeviceNode,
    _In_ PPNP_REBALANCE_TRACE_CONTEXT Context
    );
