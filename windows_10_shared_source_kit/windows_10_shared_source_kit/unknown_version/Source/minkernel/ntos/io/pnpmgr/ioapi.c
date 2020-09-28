/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    ioapi.c

Abstract:

    This module contains the plug-and-play IO system APIs.

Author:

    Shie-Lin Tzong (shielint) 3-Jan-1995
    Andrew Thornton (andrewth) 5-Sept-1996
    Paula Tomlinson (paulat) 1-May-1997

Environment:

    Kernel mode

--*/

#include "pnpmgrp.h"
#pragma hdrstop
#include <halpnpp.h>
#include <stddef.h>

#ifdef POOL_TAGGING
#undef ExAllocatePool
#define ExAllocatePool(a,b) ExAllocatePoolWithTag(a,b,'oipP')
#endif


//
// Define device state work item.
//

typedef struct _DEVICE_WORK_ITEM {
    WORK_QUEUE_ITEM WorkItem;
    PDEVICE_OBJECT DeviceObject;
    PVOID Context;
} DEVICE_WORK_ITEM, *PDEVICE_WORK_ITEM;

NTSTATUS
IopQueueDeviceWorkItem(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in VOID (*WorkerRoutine)(PVOID),
    __in_opt PVOID Context
    );

BOOLEAN
IopIsReportedAlready(
    __in PUNICODE_STRING InstanceName,
    __in HANDLE Handle,
    __in PUNICODE_STRING ServiceName,
    __in PCM_RESOURCE_LIST ResourceList,
    __out PLOGICAL MatchingKey
    );

//
// Definitions for PpCreateLegacyDeviceIds
//

#define LEGACY_COMPATIBLE_ID_BASE           TEXT("DETECTED")

//
// Define the maximum number of arguments for an indirect registry string.
//

#define MAX_FORMAT_INSERTS 20

//
// Macro to mask off prefast type coding on device property type.
//

#define DEVICE_PROPERTY(DeviceProperty) (DeviceProperty & 0xFFF)

NTSTATUS
PpCreateLegacyDeviceIds(
    __in PDEVICE_OBJECT DeviceObject,
    __in PUNICODE_STRING DriverName,
    __in PCM_RESOURCE_LIST Resources
    );

//
// An IO_GET_LEGACY_VETO_LIST_CONTEXT structure.
//

typedef struct {
    PWSTR *                     VetoList;
    ULONG                       VetoListLength;
    PPNP_VETO_TYPE              VetoType;
    NTSTATUS *                  Status;
} IO_GET_LEGACY_VETO_LIST_CONTEXT, *PIO_GET_LEGACY_VETO_LIST_CONTEXT;

typedef struct _PNP_DEVICE_EJECT {
    PIO_DEVICE_EJECT_CALLBACK   Callback;
    PVOID                       Context;
    PDRIVER_OBJECT              DriverObject;
    NTSTATUS                    Status;
    WORK_QUEUE_ITEM             WorkQueueItem;
    WCHAR                       InstancePath[MAX_DEVICE_ID_LEN];
    PNP_VETO_TYPE               VetoType;
    WCHAR                       VetoName[MAX_VETO_NAME_LENGTH];
} PNP_DEVICE_EJECT, *PPNP_DEVICE_EJECT;

//
// Define the I/O system's structure for a connected interrupt.  This defines
// the state of a single request to connect an ISR to multiple processors,
// since each processor has its own interrupt object. It also defines the
// spinlock (if caller does not supply one) and event object to be used for
// synchronization.
//
// Note in case where the interrupt is not chained, and the interrupt needs to
// be serviced at passive IRQL, both the spinlock and the event object are used.
//

typedef struct _IO_INTERRUPT_STRUCTURE {
    KSPIN_LOCK SpinLock;
    KAFFINITY TargetProcessors;
    INTERRUPT_CONNECTION_DATA ConnectionData;
    KINTERRUPT UserInterruptObject;
    BOOLEAN PassiveInterrupt;
    KEVENT PassiveEvent;
    KDPC Dpc;
    PKINTERRUPT InterruptObject[ANYSIZE_ARRAY];
} IO_INTERRUPT_STRUCTURE, *PIO_INTERRUPT_STRUCTURE;

//
// Interrupt line information table definition. This is internally defined only
// for backwards compatibility with the line-based connection parameters.
//

typedef struct _IO_INTERRUPT_LINE_INFO {

    //
    // A token interrupt object that is returned to the user and identifies
    // this structure. This object will be the first interrupt object in the
    // first line allocated to this device.
    //

    KINTERRUPT UserInterruptObject;

    //
    // Unified IRQL - indicates the IRQL that will be used when calling an
    // interrupt service routine associated with any of the interrupt lines
    // in this table. This is the highest IRQL used by all the interrupt lines
    // or a passed-in caller IRQL if it's higher.
    //

    KIRQL UnifiedIrql;

    //
    // Interrupt count - indicates the number of entries contained in this
    // interrupt information table.
    //

    ULONG InterruptCount;

    //
    // Interrupt info - lies at the start of a variable size array of
    // information table entries, with the size of the array dictated by the
    // interrupt count associated with this table.  Each entry describes a
    // different interrupt line that has been allocated to this device.
    //

    PIO_INTERRUPT_STRUCTURE InterruptInfo[ANYSIZE_ARRAY];

} IO_INTERRUPT_LINE_INFO, *PIO_INTERRUPT_LINE_INFO;

BOOLEAN
IopAppendLegacyVeto(
    __in PIO_GET_LEGACY_VETO_LIST_CONTEXT Context,
    __in PUNICODE_STRING VetoName
    );

NTSTATUS
IopConnectLineBasedInterrupt(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __out PKINTERRUPT *InterruptObject,
    __in PKSERVICE_ROUTINE ServiceRoutine,
    __in PVOID ServiceContext,
    __in_opt PKSPIN_LOCK SpinLock,
    __in_opt KIRQL SynchronizeIrql,
    __in BOOLEAN FloatingSave
    );

NTSTATUS
IopConnectMessageBasedInterrupt(
    __in ULONG Version,
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __out PIO_INTERRUPT_MESSAGE_INFO *MessageTableOut,
    __in PKMESSAGE_SERVICE_ROUTINE ServiceRoutine,
    __in PVOID ServiceContext,
    __in_opt PKSPIN_LOCK SpinLock,
    __in_opt KIRQL SynchronizeIrql,
    __in BOOLEAN FloatingSave
    );

NTSTATUS
IopConnectInterruptFullySpecified (
    __out PKINTERRUPT *InterruptObject,
    __in PKSERVICE_ROUTINE ServiceRoutine,
    __in_opt PVOID ServiceContext,
    __in_opt PKSPIN_LOCK SpinLock,
    __in ULONG Vector,
    __in KIRQL Irql,
    __in KIRQL SynchronizeIrql,
    __in KINTERRUPT_MODE InterruptMode,
    __in BOOLEAN ShareVector,
    __in PGROUP_AFFINITY TargetProcessors,
    __in BOOLEAN FloatingSave
    );

NTSTATUS
IopConnectInterrupt (
    __out PIO_INTERRUPT_STRUCTURE *InterruptStructureOut,
    __in_opt PKSERVICE_ROUTINE ServiceRoutine,
    __in_opt PKMESSAGE_SERVICE_ROUTINE MessageServiceRoutine,
    __in_opt PVOID ServiceContext,
    __in ULONG MessageIndex,
    __in_opt PKSPIN_LOCK SpinLock,
    __in KIRQL SynchronizeIrql,
    __in BOOLEAN ShareVector,
    __in BOOLEAN FloatingSave,
    __in PINTERRUPT_CONNECTION_DATA ConnectionData
    );

NTSTATUS
IopGetInterruptConnectionData(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __deref_out PINTERRUPT_CONNECTION_DATA *ConnectionDataOut
    );

BOOLEAN
IopGetLegacyVetoListDevice(
    __in PDEVICE_NODE DeviceNode,
    __in PIO_GET_LEGACY_VETO_LIST_CONTEXT Context
    );

BOOLEAN
IopGetLegacyVetoListDeviceNode(
    __in PDEVICE_NODE DeviceNode,
    __in PIO_GET_LEGACY_VETO_LIST_CONTEXT Context
    );

VOID
IopGetLegacyVetoListDrivers(
    __in PIO_GET_LEGACY_VETO_LIST_CONTEXT Context
    );

VOID
PnpRequestDeviceEjectExWorker (
    __in PPNP_DEVICE_EJECT EjectData
    );

NTSTATUS
IopQueryInterfaceRecurseUp (
    _In_ PDEVICE_OBJECT Device,
    _In_ CONST GUID *InterfaceType,
    _In_ USHORT Size,
    _In_ USHORT Version,
    _In_ PVOID InterfaceSpecificData,
    _Out_ PINTERFACE Interface
    );

#ifdef ALLOC_PRAGMA

#pragma alloc_text(PAGE, IoAssignResources)
#pragma alloc_text(PAGE, IoConnectInterrupt)
#pragma alloc_text(PAGE, IoConnectInterruptEx)
#pragma alloc_text(PAGE, IopConnectInterrupt)
#pragma alloc_text(PAGE, IopConnectLineBasedInterrupt)
#pragma alloc_text(PAGE, IopConnectMessageBasedInterrupt)
#pragma alloc_text(PAGE, IopConnectInterruptFullySpecified)
#pragma alloc_text(PAGE, IoDisconnectInterrupt)
#pragma alloc_text(PAGE, IoDisconnectInterruptEx)
#pragma alloc_text(PAGE, IopGetInterruptConnectionData)
#pragma alloc_text(PAGE, IoForwardAndCatchIrp)
#pragma alloc_text(PAGE, IoGetDeviceInterfacePropertyData)
#pragma alloc_text(PAGE, IoGetDeviceNumaNode)
#pragma alloc_text(PAGE, IoGetDeviceProperty)
#pragma alloc_text(PAGE, IoGetDevicePropertyData)
#pragma alloc_text(PAGE, IoGetDmaAdapter)
#pragma alloc_text(PAGE, IoGetLegacyVetoList)
#pragma alloc_text(PAGE, IoIsWdmVersionAvailable)
#pragma alloc_text(PAGE, IoOpenDeviceRegistryKey)
#pragma alloc_text(PAGE, IoReplacePartitionUnit)
#pragma alloc_text(PAGE, IoReportDetectedDevice)
#pragma alloc_text(PAGE, IoReportRootDevice)
#pragma alloc_text(PAGE, IoSetDeviceInterfacePropertyData)
#pragma alloc_text(PAGE, IoSetDevicePropertyData)
#pragma alloc_text(PAGE, IoSynchronousInvalidateDeviceRelations)
#pragma alloc_text(PAGE, PpCreateLegacyDeviceIds)
#pragma alloc_text(PAGE, IopAppendLegacyVeto)
#pragma alloc_text(PAGE, IopGetLegacyVetoListDevice)
#pragma alloc_text(PAGE, IopGetLegacyVetoListDeviceNode)
#pragma alloc_text(PAGE, IopGetLegacyVetoListDrivers)
#pragma alloc_text(PAGE, IopIsReportedAlready)
#pragma alloc_text(PAGE, PnpRequestDeviceEjectExWorker)
#pragma alloc_text(PAGE, IopResourceRequirementsChanged)
#pragma alloc_text(PAGE, PiGetDeviceRegistryProperty)
#pragma alloc_text(PAGE, PiGetDeviceRegProperty)
#pragma alloc_text(PAGE, PnpFindAlternateStringData)
#pragma alloc_text(PAGE, IoQueryInterface)
#pragma alloc_text(PAGE, IopQueryInterfaceRecurseUp)
#pragma alloc_text(PAGE, IoSteerInterrupt)

#endif // ALLOC_PRAGMA

__drv_maxIRQL(PASSIVE_LEVEL)
__drv_when((DeviceProperty & __string_type),
    __drv_at(PropertyBuffer,
        __post __nullterminated)
    )
__drv_when((DeviceProperty & __multiString_type),
    __drv_at(PropertyBuffer,
        __post __nullnullterminated)
    )
NTSTATUS
IoGetDeviceProperty(
    __in PDEVICE_OBJECT DeviceObject,
    __in DEVICE_REGISTRY_PROPERTY DeviceProperty,
    __in ULONG BufferLength,
    __out_bcount_opt(BufferLength) PVOID PropertyBuffer,
    __out _Notref_ __deref_out_range(<=, BufferLength) PULONG ResultLength
    )

/*++

Routine Description:

    This routine lets drivers query the registry properties associated with the
    specified device.

Parameters:

    DeviceObject - Supplies the device object whoes registry property is to be
                   returned.  This device object should be the one created by
                   a bus driver.

    DeviceProperty - Specifies what device property to get.

    BufferLength - Specifies the length, in byte, of the PropertyBuffer.

    PropertyBuffer - Supplies a pointer to a buffer to receive property data.

    ResultLength - Supplies a pointer to a variable to receive the size of the
                   property data returned.

ReturnValue:

    Status code that indicates whether or not the function was successful.  If
    PropertyBuffer is not big enough to hold requested data, STATUS_BUFFER_TOO_SMALL
    will be returned and ResultLength will be set to the number of bytes actually
    required.

--*/

{
    GUID busTypeGuid;
    DEVICE_CAPABILITIES capabilities;
    ULONG configFlags;
    DEVICE_INSTALL_STATE deviceInstallState = InstallStateFailedInstall;
    PDEVICE_NODE deviceNode;
    POBJECT_NAME_INFORMATION deviceObjectName;
    PWSTR keyName = NULL;
    ULONG length;
    ULONG propertyType;
    CM_REG_PROP regProperty;
    PIO_RESOURCE_REQUIREMENTS_LIST requirements;
    ULONG resourceLength;
    NTSTATUS status;
    ULONG translatedLength;
    PWSTR valueName;
    ULONG valueType;

    PAGED_CODE();

    PNP_ASSERT(ResultLength != NULL);
    PNP_ASSERT((BufferLength == 0) || (PropertyBuffer != NULL));

    //
    // Mask the prefetch type coding.
    //

    DeviceProperty = DEVICE_PROPERTY(DeviceProperty);

    //
    // Initialize out parameters.
    //

    *ResultLength = 0;

    if (!IS_PDO(DeviceObject)) {

        if ((DeviceObject != NULL) &&
            (DeviceProperty != DEVICE_PROPERTY(DevicePropertyInstallState)) &&
            ((DeviceProperty != DEVICE_PROPERTY(DevicePropertyEnumeratorName)) ||
             (NULL == DeviceObject->DeviceObjectExtension->DeviceNode))) {

            //
            // We'll use the verifier to fail anyone who passes in something
            // that is not a PDO *except* for the DevicePropertyInstallState.
            // This is because our check for if something is a PDO really means
            // is this a PDO that PNP knows about.  For the most part these are
            // the same, but the DevicePropertyInstallState will get called by
            // classpnp, for device objects that *it* thinks it reported as
            // PDOs, but PartMgr actually swallowed.  This is a gross exception
            // to make, so PartMgr really should be fixed.
            //
            // The arbiters attempt to retrieve the Enumerator Name property
            // in determining whether "driver shared" resource allocations may
            // be accommodated.  The PDO used may be of the "legacy resource
            // devnode" placeholder variety.  The IS_PDO() macro explicitly
            // disallows these devnodes, so we must special-case this as well,
            // in order to avoid a verifier failure.  Note that our behavior
            // here is correct--we want the get-property call to fail for these
            // legacy resource devnodes.
            //

            PpvUtilFailDriver(
                PPVERROR_DDI_REQUIRES_PDO,
                (PVOID) _ReturnAddress(),
                DeviceObject,
                NULL
                );
        }

        status = STATUS_INVALID_DEVICE_REQUEST;
        goto exit;
    }

    deviceNode = (PDEVICE_NODE) DeviceObject->DeviceObjectExtension->DeviceNode;

    //
    // Map Device Property to registry value name and value type.
    //

    switch(DeviceProperty) {

    case DEVICE_PROPERTY(DevicePropertyPhysicalDeviceObjectName):

        //
        // Create a buffer for the Obj manager.
        //

        length = BufferLength + sizeof (OBJECT_NAME_INFORMATION);
        deviceObjectName = (POBJECT_NAME_INFORMATION)ExAllocatePool(PagedPool,
                                                                    length);
        if (NULL == deviceObjectName) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto exit;
        }

        status = ObQueryNameString (DeviceObject,
                                    deviceObjectName,
                                    length,
                                    ResultLength);
        if (STATUS_INFO_LENGTH_MISMATCH == status) {

            status = STATUS_BUFFER_TOO_SMALL;
        }
        if (NT_SUCCESS (status)) {

            if (deviceObjectName->Name.Length == 0)  {

                //
                // PDO has no NAME, probably it's been deleted.
                //

                *ResultLength = 0;
            } else {

                *ResultLength = deviceObjectName->Name.Length + sizeof(UNICODE_NULL);
                if (*ResultLength > BufferLength) {

                    status = STATUS_BUFFER_TOO_SMALL;
                } else {

                    RtlCopyMemory(PropertyBuffer,
                                  deviceObjectName->Name.Buffer,
                                  deviceObjectName->Name.Length);

                    //
                    // NULL terminate.
                    //

                    *(PWCHAR)(((PUCHAR)PropertyBuffer) + deviceObjectName->Name.Length) = L'\0';
                }
            }
        } else {

            *ResultLength -= sizeof(OBJECT_NAME_INFORMATION);
        }

        ExFreePool (deviceObjectName);
        goto exit;

    case DEVICE_PROPERTY(DevicePropertyBusTypeGuid):

        status = PnpBusTypeGuidGet(deviceNode->ChildBusTypeIndex, &busTypeGuid);
        if (NT_SUCCESS(status)) {

            *ResultLength = sizeof(GUID);
            if (*ResultLength <= BufferLength) {

                RtlCopyMemory(PropertyBuffer,
                              &busTypeGuid,
                              sizeof(GUID));
            } else {

                status = STATUS_BUFFER_TOO_SMALL;
            }
        }

        goto exit;

    case DEVICE_PROPERTY(DevicePropertyLegacyBusType):

        if (deviceNode->ChildInterfaceType != InterfaceTypeUndefined) {

            *ResultLength = sizeof(INTERFACE_TYPE);
            if (*ResultLength <= BufferLength) {

                *(PINTERFACE_TYPE)PropertyBuffer = deviceNode->ChildInterfaceType;
                status = STATUS_SUCCESS;
            } else {

                status = STATUS_BUFFER_TOO_SMALL;
            }
        } else {

            status = STATUS_OBJECT_NAME_NOT_FOUND;
        }

        goto exit;

    case DEVICE_PROPERTY(DevicePropertyBusNumber):

        //
        // Retrieve the property from the parent's devnode field.
        //

        if ((deviceNode->ChildBusNumber & 0x80000000) != 0x80000000) {
            *ResultLength = sizeof(ULONG);
            if (*ResultLength <= BufferLength) {
                *(PULONG)PropertyBuffer = deviceNode->ChildBusNumber;
                status = STATUS_SUCCESS;

            } else {
                status = STATUS_BUFFER_TOO_SMALL;
            }

        } else {
            status = STATUS_OBJECT_NAME_NOT_FOUND;
        }

        goto exit;

    case DEVICE_PROPERTY(DevicePropertyEnumeratorName):

        regProperty = CM_REG_PROP_ENUMERATOR_NAME;
        valueType = REG_SZ;
        break;

    case DEVICE_PROPERTY(DevicePropertyAddress):

        status = PpIrpQueryCapabilities(DeviceObject, &capabilities);
        if (NT_SUCCESS(status) && (capabilities.Address != 0xFFFFFFFF)) {
            *ResultLength = sizeof(ULONG);
            if (*ResultLength <= BufferLength) {
                *(PULONG)PropertyBuffer = capabilities.Address;
                status = STATUS_SUCCESS;

            } else {
                status = STATUS_BUFFER_TOO_SMALL;
            }

        } else {
            status = STATUS_OBJECT_NAME_NOT_FOUND;
        }

        goto exit;

    case DEVICE_PROPERTY(DevicePropertyRemovalPolicy):

        *ResultLength = sizeof(ULONG);
        if (*ResultLength <= BufferLength) {
            PpHotSwapGetDevnodeRemovalPolicy(deviceNode,
                                             TRUE, // Include Registry Override
                                             (PDEVICE_REMOVAL_POLICY) PropertyBuffer);
            status = STATUS_SUCCESS;

        } else {
            status = STATUS_BUFFER_TOO_SMALL;
        }

        goto exit;

    case DEVICE_PROPERTY(DevicePropertyUINumber):

        regProperty = CM_REG_PROP_UI_NUMBER;
        valueType = REG_DWORD;
        break;

    case DEVICE_PROPERTY(DevicePropertyLocationInformation):

        regProperty = CM_REG_PROP_LOCATION_INFORMATION;
        valueType = REG_SZ;
        break;

    case DEVICE_PROPERTY(DevicePropertyDeviceDescription):

        regProperty = CM_REG_PROP_DEVICEDESC;
        valueType = REG_SZ;
        break;

    case DEVICE_PROPERTY(DevicePropertyHardwareID):

        regProperty = CM_REG_PROP_HARDWAREID;
        valueType = REG_MULTI_SZ;
        break;

    case DEVICE_PROPERTY(DevicePropertyCompatibleIDs):

        regProperty = CM_REG_PROP_COMPATIBLEIDS;
        valueType = REG_MULTI_SZ;
        break;

    case DEVICE_PROPERTY(DevicePropertyContainerID):

        regProperty = CM_REG_PROP_BASE_CONTAINERID;
        valueType = REG_SZ;
        break;

    case DEVICE_PROPERTY(DevicePropertyBootConfiguration):

        keyName   = REGSTR_KEY_LOGCONF;
        valueName = REGSTR_VAL_BOOTCONFIG;
        valueType = REG_RESOURCE_LIST;

        //
        // Get the registry value.
        //

        *ResultLength = BufferLength;
        status = PiGetDeviceRegistryProperty(DeviceObject,
                                             valueType,
                                             valueName,
                                             keyName,
                                             PropertyBuffer,
                                             ResultLength);

        goto exit;

    case DEVICE_PROPERTY(DevicePropertyBootConfigurationTranslated):

        if (deviceNode->BootResourcesTranslated) {
            translatedLength = PnpDetermineResourceListSize(deviceNode->BootResourcesTranslated);
            *ResultLength = translatedLength;
            if (*ResultLength <= BufferLength) {

                RtlCopyMemory(PropertyBuffer,
                              deviceNode->BootResourcesTranslated,
                              translatedLength);

                status = STATUS_SUCCESS;

            } else {
                status = STATUS_BUFFER_TOO_SMALL;
            }

        } else {
            *ResultLength = 0;
            status = STATUS_SUCCESS;
        }

        goto exit;

    case DEVICE_PROPERTY(DevicePropertyClassName):

        regProperty = CM_REG_PROP_CLASS;
        valueType = REG_SZ;
        break;

    case DEVICE_PROPERTY(DevicePropertyClassGuid):

        regProperty = CM_REG_PROP_CLASSGUID;
        valueType = REG_SZ;
        break;

    case DEVICE_PROPERTY(DevicePropertyDriverKeyName):

        regProperty = CM_REG_PROP_DRIVER;
        valueType = REG_SZ;
        break;

    case DEVICE_PROPERTY(DevicePropertyManufacturer):

        regProperty = CM_REG_PROP_MFG;
        valueType = REG_SZ;
        break;

    case DEVICE_PROPERTY(DevicePropertyFriendlyName):

        regProperty = CM_REG_PROP_FRIENDLYNAME;
        valueType = REG_SZ;
        break;

    case DEVICE_PROPERTY(DevicePropertyInstallState):

        if (deviceNode == IopRootDeviceNode) {

            //
            // The root devnode is always installed, by definition.  We
            // specifically set it's InstallState here because the
            // CONFIGFLAG_REINSTALL flag will wunfortunately still exist on the
            // root devnode reg key on a running system (we should fix that
            // later).
            //

            deviceInstallState = InstallStateInstalled;
            status = STATUS_SUCCESS;

        } else {

            //
            // Make sure this device has an instance path.
            //

            if (PnpIsDeviceInstancePathEmpty(deviceNode)) {
                status = STATUS_INVALID_DEVICE_REQUEST;
            } else {

                //
                // For all other devnodes, walk up the devnode tree, retrieving the
                // install state of all ancestors up to (but not including) the root
                // devnode.  We'll stop when we've reached the top of the tree, or
                // when some intermediate device has an "uninstalled" install state.
                //

                do {

                    //
                    // Get the ConfigFlags registry value.
                    //

                    length = sizeof(configFlags);
                    status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                                 PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                                 NULL,
                                                 CM_REG_PROP_CONFIGFLAGS,
                                                 &propertyType,
                                                 (PUCHAR)&configFlags,
                                                 &length,
                                                 0);
                    if ((NT_SUCCESS(status)) && (propertyType == REG_DWORD) && (length == sizeof(configFlags))) {

                        //
                        // The install state is just a subset of the device's ConfigFlags
                        //

                        if (configFlags & CONFIGFLAG_REINSTALL) {
                            deviceInstallState = InstallStateNeedsReinstall;

                        } else if (configFlags & CONFIGFLAG_FAILEDINSTALL) {
                            deviceInstallState = InstallStateFailedInstall;

                        } else if (configFlags & CONFIGFLAG_FINISH_INSTALL) {
                            deviceInstallState = InstallStateFinishInstall;

                        } else {
                            deviceInstallState = InstallStateInstalled;
                        }

                    } else {
                        deviceInstallState = InstallStateFailedInstall;
                        break;
                    }

                    deviceNode = deviceNode->Parent;

                } while ((deviceInstallState == InstallStateInstalled) &&
                         (deviceNode != IopRootDeviceNode));
            }
        }

        if (NT_SUCCESS(status)) {
            *ResultLength = sizeof(ULONG);
            if (*ResultLength <= BufferLength) {
                *(PDEVICE_INSTALL_STATE)PropertyBuffer = deviceInstallState;

            } else {
                status = STATUS_BUFFER_TOO_SMALL;
            }
        } else if ((status == STATUS_NOT_FOUND) ||
                   (status == STATUS_NO_SUCH_DEVICE)) {

            //
            // This is to maintain compatibility with the expected return codes
            // of this method.
            //
            // 2011-05-06 - jknichel -
            // STATUS_NOT_FOUND: IoGetDeviceProperty returns
            //   STATUS_OBJECT_NAME_NOT_FOUND when it is used to attempt to
            //   retrieve a property that does not exist.  However, pnprtl returns
            //   STATUS_NOT_FOUND.  The error code should be mapped for
            //   compatibility reasons.
            // STATUS_NO_SUCH_DEVICE: Prior to Windows 8, when a device was
            //   uninstalled, if the device was still present, its registry state
            //   would be made volatile so it could still be queried.  Starting
            //   with windows 8, the state is removed.  The above call to get the
            //   config flags could fail with STATUS_NO_SUCH_DEVICE if this device
            //   or one of its ancestors has been uninstalled and had its registry
            //   state removed.  For compatibility reasons, this should be treated
            //   as if the config flags for that device just couldn't be found
            //   which would have returned STATUS_OBJECT_NAME_NOT_FOUND.
            //

            status = STATUS_OBJECT_NAME_NOT_FOUND;
        }

        goto exit;

    case DEVICE_PROPERTY(DevicePropertyResourceRequirements):

        IopAcquireResourceListLock(&PiResourceListLock);
        requirements = deviceNode->ResourceRequirements;
        if (requirements) {
            *ResultLength = requirements->ListSize;
            if (*ResultLength <= BufferLength) {
                RtlCopyMemory(PropertyBuffer, requirements, requirements->ListSize);
                status = STATUS_SUCCESS;

            } else {
                status = STATUS_BUFFER_TOO_SMALL;
            }

        } else {
            *ResultLength = 0;
            status = STATUS_SUCCESS;
        }

        IopReleaseResourceListLock(&PiResourceListLock);
        goto exit;

    case DEVICE_PROPERTY(DevicePropertyAllocatedResources):

        IopAcquireResourceListLock(&PiResourceListLock);
        if (deviceNode->ResourceList && deviceNode->ResourceListTranslated) {
            resourceLength = PnpDetermineResourceListSize(deviceNode->ResourceList);
            translatedLength = PnpDetermineResourceListSize(deviceNode->ResourceListTranslated);
            *ResultLength = resourceLength + translatedLength;
            if (*ResultLength <= BufferLength) {
                RtlCopyMemory(PropertyBuffer,
                              deviceNode->ResourceList,
                              resourceLength);
                RtlCopyMemory((PUCHAR)PropertyBuffer + resourceLength,
                              deviceNode->ResourceListTranslated,
                              translatedLength);
                status = STATUS_SUCCESS;

            } else {
                status = STATUS_BUFFER_TOO_SMALL;
            }

        } else {
            PNP_ASSERT((deviceNode->ResourceList == NULL) &&
                   (deviceNode->ResourceListTranslated == NULL));
            *ResultLength = 0;
            status = STATUS_SUCCESS;
        }

        IopReleaseResourceListLock(&PiResourceListLock);
        goto exit;

    default:
        PNP_ASSERT(0);
        status = STATUS_INVALID_PARAMETER_2;
        goto exit;
    }

    //
    // Make sure this device has an instance path.
    //

    if (PnpIsDeviceInstancePathEmpty(deviceNode)) {
        status = STATUS_INVALID_DEVICE_REQUEST;
    } else {

        //
        // Get the registry value.
        //

        *ResultLength = BufferLength;
        status = PiGetDeviceRegProperty(PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                        NULL,
                                        valueType,
                                        regProperty,
                                        PropertyBuffer,
                                        ResultLength);

        if (status == STATUS_NOT_FOUND) {

            //
            // Remap the error code if the property doesn't exist.  This is to maintain
            // compatibility with the expected return values of this function.
            //

            status = STATUS_OBJECT_NAME_NOT_FOUND;
        }
    }

  exit:

    return status;
}

LOGICAL
PnpFindAlternateStringData(
    __in_bcount(StringLength) PWCHAR String,
    __in ULONG StringLength,
    __deref_out_bcount(*SubStringLength) PWCHAR *SubStringPtr,
    __out PULONG SubStringLength
    )

/*++

Routine Description:

    This routine parses an indirect string resource identifier to find the
    optional (non-localized) alternate sub-string data.

    Currently, indirect string resource identifiers are supported for DLL
    resources and INF [Strings] section tokens:

      @[path\]<dllname>,-<strId>[;comment]

      @[path\]<InfName>,<%Token%>[;comment]

    If the specified string appears to be in one of the above formats, and
    contains alternate-text, the location and length of the string will be
    received by StringStringPtr and SubStringLength.

Parameters:

    String - Supplies a pointer to the registry string data that is to be
                   searched for an alternate-text substring.

    StringLength - Specifies the length, in byte, of the String to be searched.

    SubStringPtr - Receives a pointer to the position within the String where
                   the (optional) alternate text (comment) begins (after the
                   separating ';' character).

    SubStringLength - Supplies a pointer to a variable to receive the size, in
                   bytes, of the alternate text sub-string.

ReturnValue:

    A LOGICAL value indicating whether alternate string data was found within
    the specified string.

Remarks:

    Note that the parsing algorithm implemented here is not as complete as the
    one implemented by the user-mode plug and play manager (see
    base\pnp\umpnpmgr\mui.c), but it should be sufficient for locating the
    alternate text for kernel-mode callers.

    This parsing algorith should be kept up-to-date with the set of indirect
    string resource identifiers currently supported by the user-mode plug and
    play manager (see base\pnp\umpnpmgr\mui.c and DEVPROP_TYPE_STRING_INDIRECT)

--*/

{
    PWCHAR endOfString;
    LOGICAL hasComma;
    PWCHAR p;

    PAGED_CODE();

    //
    // Validate parameters.
    //

    PNP_ASSERT(String != NULL);
    PNP_ASSERT(SubStringPtr != NULL);
    PNP_ASSERT(SubStringLength != NULL);

    //
    // Check if the string data *appears* to be an indirect resource string
    // (both INF and DLL formats begin with '@' and contain additional data
    // after that).  If it's not one of those, return to the caller with the
    // existing string pointer and length intact.
    //

    if ((StringLength <= sizeof(WCHAR)) || (*((PWCHAR)String) != L'@')) {
        return FALSE;
    }

    //
    // If the string *appears* to be an indirect resource string, start
    // immediately after the '@' and advance through the data looking for
    // (optional) non-localized comment text.  If present, the comment
    // appears immediately following the first ';' character.
    //
    // Check only as far as one character worth of data less than the total
    // length of the existing data because we need to return a pointer to
    // the string data starting *after* it (i.e. the ';' cannot be in the
    // last WCHAR worth of data because there would be no remaining data to
    // return).
    //

    hasComma = FALSE;
    endOfString = ((PWCHAR)String) + CB_TO_CWC(StringLength);
    p = (PWCHAR)(String) + 1;
    while (((p+1) < endOfString) && (*p != UNICODE_NULL)) {
        if (*p == L',') {
            if ((*(p+1) != L'%') && (*(p+1) != L'-') && (*(p+1) != L'#')) {

                //
                // If we find a comma ',' the character following it must be
                // a '-' (resource DLL indirection), '#' (resource message entry
                // redirection) or '%' (INF string token redirection).
                //

                break;

            } else {

                //
                // Keep track of the fact that there is found a ',' comma
                // separator in the string.  Since all formats of resource
                // string indirection that currently exist (INF,DLL) must
                // contain a comma, this will serve as extra validation that
                // the string is indeed an indirected resource string if we
                // eventually do find a ';' comment character.
                //

                hasComma = TRUE;
            }

        } else if (*p == L';') {
            if (hasComma == FALSE) {

                //
                // Found the ';' comment character before a ',' comma
                // separator.  Not a valid indirect resource string.
                //

                break;

            } else {

                //
                // Found the ';' comment character in a string where we've
                // already found a ',' comma separator.  This appears to be
                // a valid indirect resource string.  Note that it's OK if
                // the comment is the next-to-last WCHAR.  In that case, the
                // alternate text is a single character (which should be an
                // empty string, otherwise the string is not NULL-
                // terminated).
                //

                *SubStringPtr = (PWCHAR)(p+1);
                *SubStringLength = StringLength - (ULONG)((PUCHAR)(*SubStringPtr) - (PUCHAR)(String));

                ASSERT(*SubStringLength >= sizeof(WCHAR));
                ASSERT((*SubStringLength > sizeof(WCHAR))||(*(p+1) == L'\0'));

                return TRUE;
            }
        }

        p++;
    }

    return FALSE;
}

NTSTATUS
PiGetDeviceRegistryProperty(
    __in      PDEVICE_OBJECT   DeviceObject,
    __in      ULONG            ValueType,
    __in      PWSTR            ValueName,
    __in_opt  PWSTR            KeyName,
    __out_bcount_opt(*BufferLength)  PVOID  Buffer,
    __inout   PULONG           BufferLength
    )
{
    NTSTATUS status;
    HANDLE handle, subKeyHandle;
    UNICODE_STRING unicodeKey;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation = NULL;
    PCHAR valueData;
    PWCHAR subString;
    ULONG valueLength, subStringLength;
    ULONG argCount;
    PWCHAR args;
    ULONG_PTR argumentArray[MAX_FORMAT_INSERTS];
    BOOLEAN conversionFailed;
    PWCHAR nextArg;
    ULONG last;

    PAGED_CODE();

    args = NULL;
    subString = NULL;
    conversionFailed = FALSE;

    //
    // Enter critical section and acquire a lock on the registry.  Both these
    // mechanisms are required to prevent deadlock in the case where an APC
    // routine calls this routine after the registry resource has been claimed
    // in this case it would wait blocking this thread so the registry would
    // never be released -> deadlock.  Critical sectioning the registry manipulatio
    // portion solves this problem
    //

    PiLockPnpRegistry(TRUE);

    //
    // Based on the PDO specified by caller, find the handle of its device
    // instance registry key.
    //

    status = PnpDeviceObjectToDeviceInstance(DeviceObject, &handle, KEY_READ);
    if (NT_SUCCESS(status)) {

        //
        // If the data is stored in a subkey then open this key and close the old one
        //

        if (KeyName) {
            RtlInitUnicodeString(&unicodeKey, KeyName);
            status = IopOpenRegistryKeyEx(&subKeyHandle,
                                          handle,
                                          &unicodeKey,
                                          KEY_READ);
            if(NT_SUCCESS(status)){
                ZwClose(handle);
                handle = subKeyHandle;
            }
        }

        if (NT_SUCCESS(status)) {

            //
            // Read the registry value of the desired value name
            //

            status = IopGetRegistryValue(handle,
                                         ValueName,
                                         REGISTRY_PROPERTY_SIZE,
                                         &keyValueInformation);
        }

        //
        // We have finished using the registry so clean up and release our resources
        //

        ZwClose(handle);
    }

    PiUnlockPnpRegistry();

    //
    // If we have been sucessful in finding the info hand it back to the caller
    //

    if (NT_SUCCESS(status)) {

        valueData = KEY_VALUE_DATA(keyValueInformation);
        valueLength = keyValueInformation->DataLength;

        if (keyValueInformation->Type == REG_SZ) {

            //
            // For REG_SZ registry properties, check the string specifies an
            // alternate (non-localized) sub-string.  If no alternate string
            // exists, or the string cannot be parsed, return the full data from
            // the registry.
            //

            if (PnpFindAlternateStringData((PWCHAR)valueData,
                                           valueLength,
                                           &subString,
                                           &subStringLength)) {

                valueData = (PCHAR)subString;
                valueLength = subStringLength;

                //
                // An alternate string can be a format string to be modified by
                // a set of string arguments.  If arguments are present, they
                // will be in the following format: ;(Arg1,Arg2,...)
                //

                args = wcsstr(subString, L";(");
                last = (subStringLength / sizeof(WCHAR)) - 2;
                if ((args != NULL) && (subString[last] == L')')) {

                    //
                    // Separate the arguments from each other and from the
                    // format string with NULLs.  Replace the final closing
                    // parenthesis with a NULL since it is not part of the
                    // final argument.  Store the address of each argument in
                    // an argument array.
                    //

                    args[0] = UNICODE_NULL;
                    subString[last] = UNICODE_NULL;
                    args += 2;

                    RtlZeroMemory(argumentArray, sizeof(argumentArray));
                    argumentArray[0] = (ULONG_PTR)args;
                    argCount = 1;
                    nextArg = wcschr(args, L',');
                    while (nextArg != NULL) {
                        *nextArg = UNICODE_NULL;
                        nextArg += 1;
                        if (argCount >= (MAX_FORMAT_INSERTS - 1)) {
                            conversionFailed = TRUE;
                            break;
                        }

                        argumentArray[argCount] = (ULONG_PTR)nextArg;
                        argCount += 1;
                        nextArg = wcschr(nextArg, L',');
                    }
                }
            }
        }

        //
        // Check that the buffer we have been given is big enough and that the
        // value returned is of the correct registry type.
        //

        if (*BufferLength >= valueLength) {

            if (keyValueInformation->Type == ValueType) {
                if (args != NULL) {

                    //
                    // Format the value, along with the arguments list, into the
                    // caller's buffer.
                    //

                    if (conversionFailed != FALSE) {
                        status = STATUS_TOO_MANY_NAMES;

                    } else {
                        status = RtlFormatMessage(subString,
                                                  0,
                                                  FALSE,
                                                  FALSE,
                                                  TRUE,
                                                  (va_list *)argumentArray,
                                                  Buffer,
                                                  *BufferLength,
                                                  BufferLength);
                    }

                } else {
                    RtlCopyMemory(Buffer,
                                  valueData,
                                  valueLength);
                }



            } else {
               status = STATUS_INVALID_PARAMETER_2;
            }

        } else {
            status = STATUS_BUFFER_TOO_SMALL;
        }

        *BufferLength = valueLength;
        ExFreePool(keyValueInformation);
    }

    return status;
}

NTSTATUS
PiGetDeviceRegProperty(
    __in      PCWSTR           DeviceInstanceId,
    __in_opt  HANDLE           BaseKeyHandle,
    __in      ULONG            ExpectedType,
    __in      CM_REG_PROP      RegProperty,
    __out_bcount_opt(*BufferSize)  PVOID  Buffer,
    __inout   PULONG           BufferSize
    )
{
    ULONG argCount;
    PWCHAR args;
    ULONG_PTR argumentArray[MAX_FORMAT_INSERTS];
    BOOLEAN conversionFailed;
    ULONG last;
    PWCHAR nextArg;
    ULONG propertyType = 0;
    ULONG propertySize;
    NTSTATUS status;
    PWCHAR subString;
    ULONG subStringSize;
    PWCHAR tempBuffer;
    PWCHAR tempSubString;
    ULONG tempSubStringSize;

    PAGED_CODE();

    PNP_ASSERT(DeviceInstanceId != NULL);
    PNP_ASSERT(BufferSize != NULL);
    PNP_ASSERT((Buffer != NULL) || (*BufferSize == 0));

    args = NULL;
    conversionFailed = FALSE;
    subString = NULL;
    tempBuffer = NULL;

    //
    // Try retrieving the requested property using the caller's buffer.
    //

    propertySize = *BufferSize;
    status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                 DeviceInstanceId,
                                 BaseKeyHandle,
                                 RegProperty,
                                 &propertyType,
                                 (PUCHAR)Buffer,
                                 &propertySize,
                                 0);

    //
    // If the expected type is REG_SZ, it needs special additional handling.
    //

    if (ExpectedType != REG_SZ) {
        if (NT_SUCCESS(status) && (propertyType != ExpectedType)) {
            status = STATUS_INVALID_PARAMETER_2;
        }

        if (NT_SUCCESS(status) || (status == STATUS_BUFFER_TOO_SMALL)) {
            *BufferSize = propertySize;
        }

        //
        // If the type isn't REG_SZ, processing is done so jump to the end.
        //

        goto exit;
    }

    //
    // If there is any other error than that the buffer was too small, return
    // that error to the caller.
    //

    if (!NT_SUCCESS(status) && (status != STATUS_BUFFER_TOO_SMALL)) {
        goto exit;
    }

    //
    // If this property is expected to be a string, but isn't, return an error
    // to the caller.
    //

    if (NT_SUCCESS(status) && (propertyType != REG_SZ)) {
        status = STATUS_INVALID_PARAMETER_2;
        goto exit;
    }

    //
    // Allocate a buffer that is large enough to hold the property based off the
    // required size returned in the first attempt.
    //

    tempBuffer = (PWCHAR)ExAllocatePool(PagedPool, propertySize);

    if (tempBuffer == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    if (status == STATUS_BUFFER_TOO_SMALL) {
        //
        // The property needs to be retrieved in order to determine if it is an
        // indirect string or not since that influences the required size to hold
        // the property.
        //

        status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                     DeviceInstanceId,
                                     BaseKeyHandle,
                                     RegProperty,
                                     &propertyType,
                                     (PUCHAR)tempBuffer,
                                     &propertySize,
                                     0);

        if (!NT_SUCCESS(status)) {
            PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);
            if (status == STATUS_BUFFER_TOO_SMALL) {
                *BufferSize = propertySize;
            }
            goto exit;
        }

        //
        // Now that the property has been successfully retrieved, check the returned
        // property type again.
        //

        if (propertyType != REG_SZ) {
            status = STATUS_INVALID_PARAMETER_2;
            goto exit;
        }

    } else {
        //
        // The caller's buffer was big enough to fully hold the property so the
        // property needs to be copied over into the allocated buffer so it
        // can be manipulated before being copied back into the caller's buffer
        //

        PNP_ASSERT(Buffer != NULL);
        RtlCopyMemory(tempBuffer, Buffer, propertySize);
    }

    //
    // Assert that the string property is null terminated.
    //

    PNP_ASSERT(tempBuffer[(propertySize / sizeof(WCHAR)) - 1] == UNICODE_NULL);

    //
    // If we have been sucessful in finding the info hand it back to the caller
    //

    //
    // For REG_SZ registry properties, check the string specifies an alternate
    // (non-localized) sub-string.  If no alternate string exists, or the string
    // cannot be parsed, return the full data from the registry.
    //

    subString = tempBuffer;
    subStringSize = propertySize;

    //
    // This assignment is to satisfy the compiler which thinks it might be possible
    // to use tempSubString in RtlFormatMessage below without it being initialized.
    // The compiler can't detect that if args is not null then tempSubString must
    // be initialized.
    //
    tempSubString = subString;
    tempSubStringSize = subStringSize;

    if (PnpFindAlternateStringData((PWCHAR)tempBuffer,
                                   propertySize,
                                   &subString,
                                   &subStringSize)) {

        tempSubString = subString;
        tempSubStringSize = subStringSize;

        //
        // An alternate string can be a format string to be modified by
        // a set of string arguments.  If arguments are present, they
        // will be in the following format: ;(Arg1,Arg2,...)
        //

        args = wcsstr(tempSubString, L";(");
        last = (tempSubStringSize / sizeof(WCHAR)) - 2;
        if ((args != NULL) && (tempSubString[last] == L')')) {

            //
            // Separate the arguments from each other and from the
            // format string with NULLs.  Replace the final closing
            // parenthesis with a NULL since it is not part of the
            // final argument.  Store the address of each argument in
            // an argument array.
            //

            args[0] = UNICODE_NULL;
            tempSubString[last] = UNICODE_NULL;
            args += 2;

            RtlZeroMemory(argumentArray, sizeof(argumentArray));
            argumentArray[0] = (ULONG_PTR)args;
            argCount = 1;
            nextArg = wcschr(args, L',');
            while (nextArg != NULL) {
                *nextArg = UNICODE_NULL;
                nextArg += 1;
                if (argCount >= (MAX_FORMAT_INSERTS - 1)) {
                    conversionFailed = TRUE;
                    break;
                }

                argumentArray[argCount] = (ULONG_PTR)nextArg;
                argCount += 1;
                nextArg = wcschr(nextArg, L',');
            }
        }
    }

    //
    // Check that the buffer we have been given is big enough.
    //

    if (*BufferSize >= subStringSize) {
        PNP_ASSERT(Buffer != NULL);

        if (args != NULL) {

            //
            // Format the value, along with the arguments list, into the
            // caller's buffer.
            //

            if (conversionFailed != FALSE) {
                status = STATUS_TOO_MANY_NAMES;

            } else {
                status = RtlFormatMessage(tempSubString,
                                          0,
                                          FALSE,
                                          FALSE,
                                          TRUE,
                                          (va_list *)argumentArray,
                                          Buffer,
                                          *BufferSize,
                                          BufferSize);
            }

        } else {
            RtlCopyMemory(Buffer,
                          subString,
                          subStringSize);
        }

    } else {
        status = STATUS_BUFFER_TOO_SMALL;
    }


    *BufferSize = subStringSize;

  exit:

    if (tempBuffer != NULL) {
        ExFreePool(tempBuffer);
    }

    return status;
}


__drv_maxIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
IoOpenDeviceRegistryKey(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in ULONG DevInstKeyType,
    __in ACCESS_MASK DesiredAccess,
    __out PHANDLE DevInstRegKey
    )

/*++

Routine Description:

    This routine returns a handle to an opened registry key that the driver
    may use to store/retrieve configuration information specific to a particular
    device instance.

    The driver must call ZwClose to close the handle returned from this api
    when access is no longer required.

Parameters:

    DeviceObject   - Supples the device object of the physical device instance to
                     open a registry storage key for.  Normally it is a device object
                     created by the hal bus extender.

    DevInstKeyType - Supplies flags specifying which storage key associated with
                     the device instance is to be opened.  May be a combination of
                     the following value:

                     PLUGPLAY_REGKEY_DEVICE - Open a key for storing device specific
                         (driver-independent) information relating to the device instance.
                         The flag may not be specified with PLUGPLAY_REGKEY_DRIVER.

                     PLUGPLAY_REGKEY_DRIVER - Open a key for storing driver-specific
                         information relating to the device instance,  This flag may
                         not be specified with PLUGPLAY_REGKEY_DEVICE.

                     PLUGPLAY_REGKEY_CURRENT_HWPROFILE - If this flag is specified,
                         then a key in the current hardware profile branch will be
                         opened for the specified storage type.  This allows the driver
                         to access configuration information that is hardware profile
                         specific.

    DesiredAccess - Specifies the access mask for the key to be opened.

    DevInstRegKey - Supplies the address of a variable that receives a handle to the
                    opened key for the specified registry storage location.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    NTSTATUS status;
    CM_KEY_TYPE keyTypeFlags;

    if (!IS_PDO(PhysicalDeviceObject)) {
        return STATUS_INVALID_PARAMETER;
    }

    keyTypeFlags = 0;

    if (DevInstKeyType & PLUGPLAY_REGKEY_DEVICE) {
        keyTypeFlags |= CM_KEY_TYPE_DEVICE_HARDWARE;
    } else if (DevInstKeyType & PLUGPLAY_REGKEY_DRIVER) {
        keyTypeFlags |= CM_KEY_TYPE_DEVICE_SOFTWARE;
    } else {
        return STATUS_INVALID_PARAMETER;
    }

    if (DevInstKeyType & PLUGPLAY_REGKEY_CURRENT_HWPROFILE) {
        keyTypeFlags |= CM_KEY_TYPE_SCOPE_CONFIG;
    }

    PiLockPnpRegistry(TRUE);

    status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                 PnpGetDeviceInstancePath(PP_DO_TO_DN(PhysicalDeviceObject))->Buffer,
                                 keyTypeFlags,
                                 0,
                                 DesiredAccess,
                                 TRUE,
                                 DevInstRegKey,
                                 NULL);

    PiUnlockPnpRegistry();

    return status;
}


NTSTATUS
IoSynchronousInvalidateDeviceRelations(
    __in  PDEVICE_OBJECT          DeviceObject,
    __in  DEVICE_RELATION_TYPE    Type
    )

/*++

Routine Description:

    This API notifies the system that changes have occurred in the device
    relations of the specified type for the supplied DeviceObject.   All
    cached information concerning the relationships must be invalidated,
    and if needed re-obtained via IRP_MN_QUERY_DEVICE_RELATIONS.

    This routine performs device enumeration synchronously.
    Note, A driver can NOT call this IO api while processing pnp irps AND
    A driver can NOT call this api from any system thread except the system
    threads created by the driver itself.

Parameters:

    DeviceObject - the PDEVICE_OBJECT for which the specified relation type
                   information has been invalidated.  This pointer is valid
                   for the duration of the call.

    Type - specifies the type of the relation being invalidated.

ReturnValue:

    Status code that indicates whether or not the function was successful.

--*/

{
    PDEVICE_NODE deviceNode;
    NTSTATUS status = STATUS_SUCCESS;
    KEVENT completionEvent;

    PAGED_CODE();

    ASSERT_PDO(DeviceObject);

    switch (Type) {
    case BusRelations:

        if (PnPInitialized) {

            deviceNode = (PDEVICE_NODE)DeviceObject->DeviceObjectExtension->DeviceNode;

            if (deviceNode->State == DeviceNodeStarted) {

                KeInitializeEvent( &completionEvent, NotificationEvent, FALSE );

                status = PnpRequestDeviceAction(DeviceObject,
                                                ReenumerateDeviceTree,
                                                FALSE,
                                                0,
                                                &completionEvent,
                                                NULL);

                if (NT_SUCCESS(status)) {

                    status = KeWaitForSingleObject( &completionEvent,
                                                    Executive,
                                                    KernelMode,
                                                    FALSE,
                                                    NULL);
                }
            } else {
                status = STATUS_UNSUCCESSFUL;
            }
        } else {
            status = STATUS_UNSUCCESSFUL;
        }
        break;

    case EjectionRelations:
    case PowerRelations:

        //
        // For Ejection relation change, we will ignore it.  We don't keep track
        // the Ejection relation.  We will query the Ejection relation only when
        // we are requested to eject a device.
        //
        // Power relations are not implemented.
        //

        status = STATUS_NOT_SUPPORTED;
        break;
    }
    return status;
}


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
IoInvalidateDeviceRelations(
    __in  PDEVICE_OBJECT          DeviceObject,
    __in  DEVICE_RELATION_TYPE    Type
    )

/*++

Routine Description:

    This API notifies the system that changes have occurred in the device
    relations of the specified type for the supplied DeviceObject.   All
    cached information concerning the relationships must be invalidated,
    and if needed re-obtained via IRP_MN_QUERY_DEVICE_RELATIONS.

Parameters:

    DeviceObject - the PDEVICE_OBJECT for which the specified relation type
                   information has been invalidated.  This pointer is valid
                   for the duration of the call.

    Type - specifies the type of the relation being invalidated.

ReturnValue:

    none.

--*/

{

    PDEVICE_NODE deviceNode;

    ASSERT_PDO(DeviceObject);

    switch (Type) {
    case BusRelations:
    case SingleBusRelations:

        //
        // If the call was made before PnP completes device enumeration
        // we can safely ignore it.  PnP manager will do it without
        // driver's request.
        //

        deviceNode = (PDEVICE_NODE) DeviceObject->DeviceObjectExtension->DeviceNode;

        if (deviceNode) {
            PnpRequestDeviceAction(DeviceObject,
                                   Type == BusRelations ?
                                       ReenumerateDeviceTree : ReenumerateDeviceOnly,
                                   FALSE,
                                   0,
                                   NULL,
                                   NULL);
        }

        break;

    case TransportRelations:

        //
        // Let the world know that this device has invalidated its transport relations.
        // Anyone that needs to know the current transport relations can query for it
        // themselves.
        //

        PnpRequestDeviceAction(DeviceObject,
                               NotifyTransportRelationsChange,
                               FALSE,
                               0,
                               NULL,
                               NULL);

        break;

    case PowerRelations:

        //
        // Queue a request to query this device's power relations.
        //

        PnpRequestDeviceAction(DeviceObject,
                               QueryPowerRelations,
                               FALSE,
                               0,
                               NULL,
                               NULL);

        break;

    case EjectionRelations:

        //
        // Let the world know that this device has invalidated its ejection relations.
        // Anyone that needs to know the current ejection relations can query for it
        // themselves.
        //

        PnpRequestDeviceAction(DeviceObject,
                               NotifyEjectionRelationsChange,
                               FALSE,
                               0,
                               NULL,
                               NULL);

        break;
    }
}

__drv_maxIRQL(DISPATCH_LEVEL)
_Must_inspect_result_
NTSTATUS
IoRequestDeviceEjectEx (
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in_opt PIO_DEVICE_EJECT_CALLBACK Callback,
    __in_opt PVOID Context,
    __in_opt PDRIVER_OBJECT DriverObject
    )
/*++

Routine Description:

    This routine requests an asynchronous device eject. The caller may pass in
    a callback (and context) that will be called when the eject operation is
    finished with the final status.

Arguments:

    PhysicalDeviceObject - pointer to the pdo of the device to eject

    Callback - pointer to the caller's callback if they want to find out the
        ultimate status of the eject operation

    Context - context value passed to the callback

    DriverObject - caller's driver object for referencing

Return Value:

    Standard NTSTATUS codes. If this function succeeds, it does *not* mean that
    the eject has succeeded -- rather, it means that the eject was successfully
    queued up and that the caller's callback will be called with the final
    status.

    If this function fails, the caller's callback is not called.

--*/
{
    PDEVICE_NODE deviceNode;
    PPNP_DEVICE_EJECT ejectData;
    UNICODE_STRING instancePath;
    UNICODE_STRING unicodeVeto;

    PNP_ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);
    PNP_ASSERT(PhysicalDeviceObject != NULL);
    ASSERT_PDO(PhysicalDeviceObject);

    //
    // Allocate and capture eject parameters.
    //

    ejectData = ExAllocatePoolWithTag(NonPagedPoolNx,
                                      sizeof(PNP_DEVICE_EJECT),
                                      PNP_POOLTAG_EJECT_DATA);
    if (ejectData == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Initialize the ejection data.
    //

    PNP_ASSERT(ejectData != NULL);
    deviceNode = PP_DO_TO_DN(PhysicalDeviceObject);
    ejectData->Callback = Callback;
    ejectData->Context = Context;
    PNP_ASSERT(PnpGetDeviceInstancePath(deviceNode)->Length < RTL_FIELD_SIZE(PNP_DEVICE_EJECT,
                                                                InstancePath));
    RtlCopyMemory(ejectData->InstancePath,
                  PnpGetDeviceInstancePath(deviceNode)->Buffer,
                  PnpGetDeviceInstancePath(deviceNode)->Length);
    ejectData->InstancePath[PnpGetDeviceInstancePath(deviceNode)->Length / sizeof(WCHAR)] = UNICODE_NULL;
    instancePath.Buffer = ejectData->InstancePath;
    instancePath.Length = PnpGetDeviceInstancePath(deviceNode)->Length;
    instancePath.MaximumLength = instancePath.Length;
    if (DriverObject != NULL) {
        ObReferenceObject(DriverObject);
    }

    ejectData->DriverObject = DriverObject;
    ejectData->VetoType = PNP_VetoTypeUnknown;
    ejectData->VetoName[0] = UNICODE_NULL;
    unicodeVeto.Buffer = &ejectData->VetoName[0];
    unicodeVeto.MaximumLength = RTL_FIELD_SIZE(PNP_DEVICE_EJECT, VetoName);
    unicodeVeto.Length = 0;

    //
    // Queue device ejection to a work item.
    //

    ExInitializeWorkItem(&ejectData->WorkQueueItem,
                         PnpRequestDeviceEjectExWorker,
                         ejectData);
    ExQueueWorkItem(&ejectData->WorkQueueItem,
                    DelayedWorkQueue);
    return STATUS_SUCCESS;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
IoRequestDeviceEject (
    __in PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This API notifies that the device eject button has been pressed. This API must
    be called at IRQL <= DISPATCH_LEVEL.

    This API informs PnP that a device eject has been requested, the device will
    not necessarily be ejected as a result of this API.  The device will only be
    ejected if the drivers associated with it agree to stop and the device is
    successfully powered down.  Note that eject in this context refers to device
    eject, not to media (floppies, cds, tapes) eject.  For example, eject of a
    cd-rom disk drive, not ejection of a cd-rom disk.

Arguments:

    DeviceObject - the PDEVICE_OBJECT for the device whose eject button has
                   been pressed.  This pointer is valid for the duration of
                   the call, if the API wants to keep a copy of it, it
                   should obtain its own reference to the object
                   (ObReferenceObject).

ReturnValue:

    None.

--*/

{
    IoRequestDeviceEjectEx(DeviceObject, NULL, NULL, NULL);
}

VOID
PnpRequestDeviceEjectExWorker (
    __in PPNP_DEVICE_EJECT EjectData
    )

/*++

Routine Description:

    This routine waits for a device ejection to complete, so that a
    client's callback can be notified of the final status.

Arguments:

    Context - A structure containing the event to wait on and the callback to run.

Return Value:

    None.

--*/

{
    UNICODE_STRING deviceInstance;
    ULONG vetoNameLength;

    PAGED_CODE();

    PNP_ASSERT(EjectData != NULL);
    RtlInitUnicodeString(&deviceInstance, EjectData->InstancePath);
    vetoNameLength = RTL_FIELD_SIZE(PNP_DEVICE_EJECT, VetoName);

    //
    // Queue the ejection request.
    //

    EjectData->Status = PnpQueueQueryAndRemoveEvent(&deviceInstance,
                                                    &EjectData->VetoType,
                                                    EjectData->VetoName,
                                                    &vetoNameLength,
                                                    PNP_QUERY_AND_REMOVE_EJECT_DEVICE,
                                                    TRUE);

    //
    // Call any callback with the final status of the ejection
    // and cleanup.
    //

    if (EjectData->Callback != NULL) {
        EjectData->Callback(EjectData->Status,
                            EjectData->Context);
    }

    if (EjectData->DriverObject != NULL) {
        ObDereferenceObject(EjectData->DriverObject);
    }

    ExFreePoolWithTag(EjectData, PNP_POOLTAG_EJECT_DATA);
    return;
}

__drv_maxIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
IoReportDetectedDevice(
    __in PDRIVER_OBJECT DriverObject,
    __in INTERFACE_TYPE LegacyBusType,
    __in ULONG BusNumber,
    __in ULONG SlotNumber,
    __in_opt PCM_RESOURCE_LIST ResourceList,
    __in_opt PIO_RESOURCE_REQUIREMENTS_LIST ResourceRequirements,
    __in BOOLEAN ResourceAssigned,
    __deref_inout_opt PDEVICE_OBJECT *DeviceObject
    )

/*++

Routine Description:

    PnP device drivers call this API to report any device detected.  This routine
    creates a Physical Device object, reference the Physical Device object and
    returns back to the callers.  Once the detected device is reported, the Pnp manager
    considers the device has been fully controlled by the reporting drivers.  Thus it
    will not invoke AddDevice entry and send StartDevice irp to the driver.

    The driver needs to report the resources it used to detect this device such that
    pnp manager can perform duplicates detection on this device.

    The caller must dereference the DeviceObject once it no longer needs it.

Parameters:

    DriverObject - Supplies the driver object of the driver who detected
                   this device.

    ResourceList - Supplies a pointer to the resource list which the driver used
                   to detect the device.

    ResourceRequirements - supplies a pointer to the resource requirements list
                   for the detected device.  This is optional.

    ResourceAssigned - if TRUE, the driver already called IoReportResourceUsage or
                   IoAssignResource to get the ownership of the resources.  Otherwise,
                   the PnP manager will call IoReportResourceUsage to allocate the
                   resources for the driver.

    DeviceObject - if NULL, this routine will create a PDO and return it thru this variable.
                   Otherwise, a PDO is already created and this routine will simply use the supplied
                   PDO.

Return Value:

    Status code that indicates whether or not the function was successful.


--*/
{
    WCHAR buffer[MAX_DEVICE_ID_LEN];
    PCM_RESOURCE_LIST cmResource;
    BOOLEAN conflict;
    HANDLE controlKey;
    BOOLEAN created;
    PDEVICE_NODE deviceNode;
    DEVPROP_BOOLEAN devpropBoolean;
    ULONG disposition;
    UNICODE_STRING driverName = {0};
    ULONG i;
    UNICODE_STRING instance;
    HANDLE instanceKey;
    UNICODE_STRING instancePath;
    LOGICAL isDuplicate;
    ULONG length;
    ULONG listSize;
    HANDLE logConfKey;
    BOOLEAN newPdoCreated;
    PWCHAR p;
    PDEVICE_OBJECT pdo;
    PUNICODE_STRING serviceName;
    PWSTR serviceString;
    ULONG serviceStringMaximumSize;
    size_t serviceStringSize;
    NTSTATUS status;
    ULONG ulongProperty;
    UNICODE_STRING unicodeName;
    HPNPOPCTX hPnpOpCtx;

    PAGED_CODE();

    //
    // Validate parameters.
    //

    PNP_ASSERT(DriverObject != NULL);
    PNP_ASSERT(DeviceObject != NULL);

    //
    // Initialize locals for proper cleanup on exit.
    //

    controlKey = NULL;
    created = FALSE;
    deviceNode = NULL;
    disposition = 0;
    isDuplicate = FALSE;
    instanceKey = NULL;
    instancePath.Buffer = buffer;
    instancePath.MaximumLength = sizeof(buffer);
    instancePath.Length = 0;
    logConfKey = NULL;
    newPdoCreated = FALSE;
    pdo = NULL;
    serviceName = &DriverObject->DriverExtension->ServiceKeyName;
    status = STATUS_SUCCESS;
    hPnpOpCtx = NULL;

    if (*DeviceObject != NULL) {

        //
        // Validate that there is a devnode for this device object.
        //

        deviceNode = PP_DO_TO_DN(*DeviceObject);
        if (deviceNode == NULL) {
            PNP_ASSERT(deviceNode != NULL);
            return STATUS_INVALID_PARAMETER_8;
        }

    } else {

        //
        // Driver created via IoCreateDriver(). Service name is in the
        // "\Driver\Service" format. Extract just the "Service" part.
        //

        if (DriverObject->Flags & DRVO_BUILTIN_DRIVER) {
            p = serviceName->Buffer + (serviceName->Length / sizeof(WCHAR)) - 1;
            for (;;) {
                if (p == serviceName->Buffer) {
                    PNP_ASSERT(0);
                    return STATUS_INVALID_PARAMETER_1;
                }

                if (*p == OBJ_NAME_PATH_SEPARATOR) {
                    p++;
                    break;
                }

                p--;
            }

            driverName.Buffer = p;
            driverName.Length = serviceName->Length -
                                    (USHORT)((p - serviceName->Buffer) * sizeof(WCHAR));
            driverName.MaximumLength = driverName.Length;
        }

        //
        // Build device id for this detected device. Its format is
        // "ROOT\Service".
        //

        status = RtlAppendUnicodeToString(&instancePath, TEXT("ROOT\\"));
        if (!NT_SUCCESS(status)) {
            PNP_ASSERT(NT_SUCCESS(status));
            return status;
        }

        PNP_ASSERT(serviceName != NULL);
        status = RtlAppendUnicodeStringToString(&instancePath,
                                                (DriverObject->Flags & DRVO_BUILTIN_DRIVER) ?
                                                    &driverName : serviceName);

        if (!NT_SUCCESS(status)) {
            PNP_ASSERT(NT_SUCCESS(status));
            return status;
        }
    }

    status = PiPnpRtlBeginOperation(&hPnpOpCtx);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }


    //
    // Lock the device tree so nothing is added\removed from it.
    // There should only be a single exit path from this point onwards
    // using a "goto exit;"
    //

    PpDevNodeLockTree(PPL_TREEOP_ALLOW_READS);

    //
    // Check for a duplicate before creating a new one.
    //

    if (deviceNode == NULL) {
        if (!(DriverObject->Flags & DRVO_BUILTIN_DRIVER)) {
            status = IopDuplicateDetection(LegacyBusType,
                                           BusNumber,
                                           SlotNumber,
                                           &deviceNode);
            if (NT_SUCCESS(status) && deviceNode != NULL) {
                isDuplicate = TRUE;
            }
        }
    }

    //
    // Lock the registry since new keys are going to be created.
    //

    PiLockPnpRegistry(TRUE);
    if (deviceNode == NULL) {

        //
        // Determine the first available instance for this detected device.
        //

        status = RtlAppendUnicodeToString(&instancePath, TEXT("\\"));
        if (!NT_SUCCESS(status)) {
            PNP_ASSERT(NT_SUCCESS(status));
            goto exit;
        }

        length = instancePath.Length;
        for (i = 0; ; i++) {
            //
            // Reset the length of the instance path back to just "ROOT\<service>\".
            //
            instancePath.Length = (USHORT)length;

            //
            // InstancePath.Buffer points to buffer, so this adds the current
            // instance on to the end of instancePath.
            //
            PiUlongToInstanceKeyUnicodeString(&instance,
                                              buffer +
                                                (instancePath.Length / sizeof(WCHAR)),
                                              sizeof(buffer) - instancePath.Length,
                                              i);

            //
            // Update the length of the instance path since it is now a full
            // device instance path.
            //
            instancePath.Length = instancePath.Length + instance.Length;

            status = _CmCreateDevice(PiPnpRtlCtx,
                                     instancePath.Buffer,
                                     KEY_ALL_ACCESS,
                                     &instanceKey,
                                     &created,
                                     0);

            if (!NT_SUCCESS(status) || (created)) {
                break;
            }

            isDuplicate = FALSE;
            if (IopIsReportedAlready(&instancePath,
                                     instanceKey,
                                     serviceName,
                                     ResourceList,
                                     &isDuplicate)) {

                PNP_ASSERT(isDuplicate != FALSE);

                pdo = PnpDeviceObjectFromDeviceInstance(&instancePath);
                if (pdo == NULL) {
                    status = STATUS_UNSUCCESSFUL;
                }

                deviceNode = PP_DO_TO_DN(pdo);
                break;
            }

            ZwClose(instanceKey);
        }

        if (!NT_SUCCESS(status)) {
            goto exit;
        }

        PNP_ASSERT((created) || (isDuplicate != FALSE));
        PNP_ASSERT(instanceKey != NULL);
    }

    if (deviceNode == NULL) {

        //
        // Create the detected device.
        //

        PNP_ASSERT(deviceNode == NULL);
        PNP_ASSERT(pdo == NULL);
        status = IoCreateDevice(PnpDriverObject,
                                0,
                                NULL,
                                FILE_DEVICE_CONTROLLER,
                                FILE_AUTOGENERATED_DEVICE_NAME,
                                FALSE,
                                &pdo);

        if (!NT_SUCCESS(status)) {
            goto exit;
        }

        newPdoCreated = TRUE;
        pdo->Flags |= DO_BUS_ENUMERATED_DEVICE;   // Mark this is a PDO
        status = PipAllocateDeviceNode(pdo, &deviceNode);
        if ((status == STATUS_SYSTEM_HIVE_TOO_LARGE) || (deviceNode == NULL)) {
            IoDeleteDevice(pdo);
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto exit;
        }

        //
        // Initialize the devnode.
        //

        if (!(DriverObject->Flags & DRVO_BUILTIN_DRIVER)) {
            deviceNode->ServiceName.Buffer = ExAllocatePoolWithTag(
                                                PagedPool,
                                                serviceName->Length,
                                                PNP_POOLTAG_SERVICE_NAME);

            if (deviceNode->ServiceName.Buffer == NULL) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                PipSetDevNodeProblem(deviceNode, CM_PROB_OUT_OF_MEMORY, status);
                goto exit;
            }

            deviceNode->ServiceName.MaximumLength = serviceName->Length;
            status = RtlUpcaseUnicodeString(&deviceNode->ServiceName,
                                            serviceName,
                                            FALSE);

            if (!NT_SUCCESS(status)) {
                PipSetDevNodeProblem(deviceNode, CM_PROB_OUT_OF_MEMORY, status);
                goto exit;
            }
        }

        status = PnpAllocateDeviceInstancePath(deviceNode,
                                               instancePath.Length + sizeof(UNICODE_NULL));

        if (!NT_SUCCESS(status)) {
            PipSetDevNodeProblem(deviceNode, CM_PROB_OUT_OF_MEMORY, status);
            goto exit;
        }

        status = RtlUpcaseUnicodeString(PnpGetDeviceInstancePath(deviceNode),
                                        &instancePath,
                                        FALSE);

        if (!NT_SUCCESS(status)) {
            PipSetDevNodeProblem(deviceNode, CM_PROB_OUT_OF_MEMORY, status);
            goto exit;
        }

        PnpGetDeviceInstancePath(deviceNode)->Buffer[PnpGetDeviceInstancePath(deviceNode)->Length/sizeof(WCHAR)] = UNICODE_NULL;

        ulongProperty = CONFIGFLAG_REINSTALL;
        status = _CmSetDeviceRegProp(PiPnpRtlCtx,
                                     PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                     instanceKey,
                                     CM_REG_PROP_CONFIGFLAGS,
                                     REG_DWORD,
                                     (PUCHAR)&ulongProperty,
                                     sizeof(ulongProperty),
                                     0);

        if (!NT_SUCCESS(status)) {
            PipSetDevNodeProblem(deviceNode, CM_PROB_REGISTRY, status);
            goto exit;
        }

        devpropBoolean = DEVPROP_TRUE;
        status = _PnpSetObjectProperty(PiPnpRtlCtx,
                                       PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                       PNP_OBJECT_TYPE_DEVICE,
                                       instanceKey,
                                       NULL,
                                       &DEVPKEY_Device_Reported,
                                       DEVPROP_TYPE_BOOLEAN,
                                       (PUCHAR)&devpropBoolean,
                                       sizeof(devpropBoolean),
                                       0);

        if (!NT_SUCCESS(status)) {
            PipSetDevNodeProblem(deviceNode, CM_PROB_REGISTRY, status);
            goto exit;
        }

        status = PnpUnicodeStringToWstr(&serviceString, &serviceStringMaximumSize, serviceName);

        if (!NT_SUCCESS(status)) {
            PipSetDevNodeProblem(deviceNode, CM_PROB_REGISTRY, status);
            goto exit;
        }

        status = RtlStringCbLengthW(serviceString,
                                    serviceStringMaximumSize,
                                    &serviceStringSize);

        if (!NT_SUCCESS(status)) {
            PnpUnicodeStringToWstrFree(serviceString, serviceName);
            PipSetDevNodeProblem(deviceNode, CM_PROB_REGISTRY, status);
            goto exit;
        }

        //
        // Add in the null terminator
        //
        serviceStringSize += sizeof(UNICODE_NULL);
        PNP_ASSERT((ULONG)serviceStringSize <= serviceStringMaximumSize);

        status = _CmSetDeviceRegProp(PiPnpRtlCtx,
                                     PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                     instanceKey,
                                     CM_REG_PROP_SERVICE,
                                     REG_SZ,
                                     (PUCHAR)serviceString,
                                     (ULONG)serviceStringSize,
                                     0);

        PnpUnicodeStringToWstrFree(serviceString, serviceName);

        if (!NT_SUCCESS(status)) {
            PipSetDevNodeProblem(deviceNode, CM_PROB_REGISTRY, status);
            goto exit;
        }

        controlKey = NULL;
        status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                     PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                     CM_KEY_TYPE_DEVICE_CONTROL,
                                     0,
                                     KEY_ALL_ACCESS,
                                     TRUE,
                                     &controlKey,
                                     NULL);

        if (!NT_SUCCESS(status)) {
            PipSetDevNodeProblem(deviceNode, CM_PROB_REGISTRY, status);
            goto exit;
        }

        status = PnpSetRegistryDword(controlKey,
                                     REGSTR_VALUE_DEVICE_REPORTED,
                                     1);

        if (!NT_SUCCESS(status)) {
            PipSetDevNodeProblem(deviceNode, CM_PROB_REGISTRY, status);
            goto exit;
        }

        if (!(DriverObject->Flags & DRVO_BUILTIN_DRIVER)) {
            PpDeviceRegistration(&instancePath,
                                 TRUE,
                                 &deviceNode->ServiceName,
                                 TRUE);
        }

        status = PpCreateLegacyDeviceIds(pdo,
                                         (DriverObject->Flags & DRVO_BUILTIN_DRIVER) ?
                                            &driverName : serviceName,
                                         ResourceList);

        if (!NT_SUCCESS(status)) {
            PipSetDevNodeProblem(deviceNode, CM_PROB_REGISTRY, status);
            goto exit;
        }

        PipSetDevNodeFlags(deviceNode, DNF_MADEUP | DNF_ENUMERATED);
        deviceNode->NumaNodeIndex = IopRootDeviceNode->NumaNodeIndex;
        PipSetDevNodeState(deviceNode, DeviceNodeInitialized, NULL);

        PpDevNodeInsertIntoTree(IopRootDeviceNode, deviceNode);

        //
        // Add an entry into the table to set up a mapping between the DO
        // and the instance path.
        //

        status = PnpMapDeviceObjectToDeviceInstance(pdo,
                                                    PnpGetDeviceInstancePath(deviceNode));
        if (!NT_SUCCESS(status)) {
            PipSetDevNodeProblem(deviceNode, CM_PROB_OUT_OF_MEMORY, status);
            goto exit;
        }

        //
        // Add a reference to the DeviceObject for ourself
        //

        ObReferenceObject(pdo);

    } else {
        if (isDuplicate != FALSE) {
            PNP_ASSERT(deviceNode != NULL);
            if (PipAreDriversLoaded(deviceNode) ||
                (PipDoesDevNodeHaveProblem(deviceNode) &&
                    (deviceNode->Problem != CM_PROB_NOT_CONFIGURED) &&
                    (deviceNode->Problem != CM_PROB_REINSTALL) &&
                    (deviceNode->Problem != CM_PROB_FAILED_INSTALL))) {
                ObDereferenceObject(deviceNode->PhysicalDeviceObject);
                status = STATUS_NO_SUCH_DEVICE;
                goto exit;
            }

            if (pdo == NULL) {
                PipClearDevNodeProblem(deviceNode);
            }
        }

        if (instanceKey == NULL) {
            status = PnpDeviceObjectToDeviceInstance(deviceNode->PhysicalDeviceObject,
                                                     &instanceKey,
                                                     KEY_ALL_ACCESS);

            if (!NT_SUCCESS(status)) {
                goto exit;
            }
        }
    }

    PNP_ASSERT(instanceKey != NULL);
    PNP_ASSERT(deviceNode != NULL);

    //
    // Only create the LogConf key if a resource list or resource requirements
    // list will be set.
    //

    if ((ResourceList != NULL) || (ResourceRequirements != NULL)) {
        status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                     PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                     CM_KEY_TYPE_DEVICE_LOGCONF,
                                     0,
                                     KEY_ALL_ACCESS,
                                     TRUE,
                                     &logConfKey,
                                     NULL);

        if (!NT_SUCCESS(status)) {
            PipSetDevNodeProblem(deviceNode, CM_PROB_REGISTRY, status);
            goto exit;
        }

        if (ResourceList != NULL) {
            status = PnpSetRegistryResourceList(logConfKey,
                                                REGSTR_VAL_BOOTCONFIG,
                                                ResourceList);

            if (!NT_SUCCESS(status)) {
                PipSetDevNodeProblem(deviceNode, CM_PROB_REGISTRY, status);
                goto exit;
            }
        }

        if (ResourceRequirements != NULL) {
            status = PnpSetRegistryRequirementsList(logConfKey,
                                                    REGSTR_VALUE_BASIC_CONFIG_VECTOR,
                                                    ResourceRequirements);

            if (!NT_SUCCESS(status)) {
                PipSetDevNodeProblem(deviceNode, CM_PROB_REGISTRY, status);
                goto exit;
            }
        }
    }

    //
    // Allocate resources for the detected device.
    //

    if (ResourceAssigned) {

        //
        // Resources already acquired for this device.
        //

        PipSetDevNodeFlags(deviceNode, DNF_NO_RESOURCE_REQUIRED);
        PnpSetRegistryDword(instanceKey,
                            REGSTR_VALUE_NO_RESOURCE_AT_INIT,
                            1);

        IopWriteAllocatedResourcesToRegistry(deviceNode,
                                             ResourceList,
                                             PnpDetermineResourceListSize(ResourceList));

    } else {
        if ((ResourceList != NULL) &&
            (ResourceList->Count != 0) &&
            (ResourceList->List[0].PartialResourceList.Count != 0)) {
            listSize = PnpDetermineResourceListSize(ResourceList);
            cmResource = ExAllocatePoolWithTag(PagedPool,
                                               listSize,
                                               PNP_POOLTAG_GENERIC);

            if (cmResource == NULL) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                PipSetDevNodeProblem(deviceNode, CM_PROB_OUT_OF_MEMORY, STATUS_INSUFFICIENT_RESOURCES);
                goto exit;
            }

            RtlCopyMemory(cmResource, ResourceList, listSize);

            //
            // Allocate resources for this detected device.
            //

            pdo = deviceNode->PhysicalDeviceObject;
            PiWstrToUnicodeString(&unicodeName, PNPMGR_STR_PNP_MANAGER);
            conflict = FALSE;
            status = IoReportResourceUsageInternal(ArbiterRequestLegacyReported,
                                                   &unicodeName,
                                                   pdo->DriverObject,
                                                   NULL,
                                                   0,
                                                   pdo,
                                                   cmResource,
                                                   listSize,
                                                   FALSE,
                                                   &conflict);

            ExFreePoolWithTag(cmResource, PNP_POOLTAG_GENERIC);
            if (!NT_SUCCESS(status) || (conflict != FALSE)) {
                if (PipIsDevNodeDNStarted(deviceNode) == FALSE) {
                    PipSetDevNodeProblem(deviceNode, CM_PROB_NORMAL_CONFLICT, status);
                }
                status = STATUS_CONFLICTING_ADDRESSES;
            }

        } else {
            PNP_ASSERT(ResourceRequirements == NULL);
            PipSetDevNodeFlags(deviceNode, DNF_NO_RESOURCE_REQUIRED);
        }
    }

exit:

    if (NT_SUCCESS(status)) {

        //
        // Assume detected device as started and reenumerate its children.
        // Caller is expected to attach to the PDO returned by this API.
        // There is no way for PNP to synchronize with this attach. So if
        // the caller wants to report children, it needs to call
        // IoInvalidateDeviceRelations() on this PDO after it attaches.
        //

        PNP_ASSERT(deviceNode != NULL);
        PNP_ASSERT(deviceNode->Parent != NULL);
        deviceNode->NumaNodeIndex = deviceNode->Parent->NumaNodeIndex;
        IopDoDeferredSetInterfaceState(deviceNode);
        PipSetDevNodeState(deviceNode, DeviceNodeStartPostWork, NULL);

        //
        // If the PDO was created by this function (as it is in nearly every
        // case), then wait until the FDO is attached before sending queries to
        // the device (such as query capabilities or device state). If an
        // existing PDO was used, then the FDO is already attached, so
        // don't block waiting for something to attach.
        //

        if (newPdoCreated != FALSE) {
            PipSetDevNodeFlags(deviceNode, DNF_WAITING_FOR_FDO);
        }

        *DeviceObject = deviceNode->PhysicalDeviceObject;

    } else {
        if (created) {

            //
            // Cleanup this devnode and remove it from the device tree.
            //

            PNP_ASSERT(deviceNode->PhysicalDeviceObject != *DeviceObject);

            if (logConfKey != NULL) {
                ZwDeleteKey(logConfKey);
            }

            if (controlKey != NULL) {
                ZwDeleteKey(controlKey);
            }

            if (instanceKey != NULL) {
                ZwDeleteKey(instanceKey);
            }

            PnpCleanupDeviceRegistryValues(PnpGetDeviceInstancePath(deviceNode));
            PpDevNodeRemoveFromTree(deviceNode);
            IoDeleteDevice(deviceNode->PhysicalDeviceObject);
            ObDereferenceObject(deviceNode->PhysicalDeviceObject);
        }
    }

    //
    // Unlock and cleanup.
    //

    PiUnlockPnpRegistry();
    PpDevNodeUnlockTree(PPL_TREEOP_ALLOW_READS);
    if (logConfKey != NULL) {
        ZwClose(logConfKey);
    }

    if (controlKey != NULL) {
        ZwClose(controlKey);
    }

    if (instanceKey != NULL) {
        ZwClose(instanceKey);
    }

    if (hPnpOpCtx) {
        PiPnpRtlEndOperation(hPnpOpCtx);
    }

    return status;
}

__drv_maxIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
IoReportRootDevice(
    __in PDRIVER_OBJECT DriverObject
    )

/*++

Routine Description:

    PnP device drivers call this API to create a root-enumerated device without
    going through device installation.  The service provided in the driver
    object is loaded for the device.  Only one root enumerated device per
    service can be loaded in this manner.  If any devices for this service
    already exist, this routine will succeed but not create a new device.

Parameters:

    DriverObject - Supplies the driver object of the driver who detected
        this device.

Return Value:

    Status code that indicates whether or not the function was successful.


--*/
{
    WCHAR buffer[MAX_DEVICE_ID_LEN];
    WCHAR bufferHwId[MAX_DEVICE_ID_LEN+1];
    BOOLEAN created;
    DEVPROP_BOOLEAN devpropBoolean;
    UNICODE_STRING hardwareIds;
    UNICODE_STRING instance;
    HANDLE instanceKey;
    UNICODE_STRING instancePath;
    ULONG length;
    PUNICODE_STRING serviceName;
    NTSTATUS status;
    ULONG value;
    HPNPOPCTX hPnpOpCtx;

    PAGED_CODE();

    PNP_ASSERT(DriverObject != NULL);

    //
    // Initialize locals for proper cleanup on exit.
    //

    created = FALSE;
    instanceKey = NULL;
    instancePath.Buffer = buffer;
    instancePath.MaximumLength = sizeof(buffer);
    instancePath.Length = 0;
    hardwareIds.Buffer = bufferHwId;
    hardwareIds.MaximumLength = sizeof(bufferHwId);
    hardwareIds.Length = 0;
    serviceName = &DriverObject->DriverExtension->ServiceKeyName;
    hPnpOpCtx = NULL;

    //
    // Build device id for this detected device. Its format is
    // "ROOT\Service".  Use the dveice id as the HardwareIds.
    //

    status = RtlAppendUnicodeToString(&instancePath, TEXT("ROOT\\"));
    if (!NT_SUCCESS(status)) {
        PNP_ASSERT(NT_SUCCESS(status));
        return status;
    }

    PNP_ASSERT(serviceName != NULL);
    status = RtlAppendUnicodeStringToString(&instancePath, serviceName);
    if (!NT_SUCCESS(status)) {
        PNP_ASSERT(NT_SUCCESS(status));
        return status;
    }

    if (instancePath.Length > (sizeof(bufferHwId) - (2*sizeof(UNICODE_NULL)))) {
        return STATUS_OBJECT_NAME_INVALID;
    }

    RtlCopyUnicodeString(&hardwareIds, &instancePath);

    status = PiPnpRtlBeginOperation(&hPnpOpCtx);
    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Lock the device tree so nothing is added\removed from it.
    // There should only be a single exit path from this point onwards
    // using a "goto exit;"
    //

    PpDevNodeLockTree(PPL_TREEOP_ALLOW_READS);

    //
    // Lock the registry since new keys are going to be created.
    //

    PiLockPnpRegistry(TRUE);

    //
    // Open instance 0 for this service.
    //

    PNP_ASSERT(instancePath.Length > 0);

    status = RtlAppendUnicodeToString(&instancePath, TEXT("\\"));
    if (!NT_SUCCESS(status)) {
        PNP_ASSERT(NT_SUCCESS(status));
        goto exit;
    }

    length = instancePath.Length;
    PiUlongToInstanceKeyUnicodeString(&instance,
                                      buffer + (instancePath.Length / sizeof(WCHAR)),
                                      sizeof(buffer) - instancePath.Length,
                                      0);

    instancePath.Length = instancePath.Length + instance.Length;

    //
    // Create/open the instance key, if it already exists, there's nothing more to do.
    //

    status = _CmCreateDevice(PiPnpRtlCtx,
                             instancePath.Buffer,
                             KEY_ALL_ACCESS,
                             &instanceKey,
                             &created,
                             0);

    if ((!NT_SUCCESS(status)) || (!created)) {
        goto exit;
    }

    PNP_ASSERT(instanceKey != NULL);

    //
    // HardwareId = ROOT\<serviceName>
    //

    length = hardwareIds.Length + (2 * sizeof(UNICODE_NULL));
    NT_ASSERT((length <= sizeof(bufferHwId)) && (length > (2*sizeof(WCHAR))));
    hardwareIds.Buffer[(length/sizeof(WCHAR))-1] = UNICODE_NULL;
    hardwareIds.Buffer[(length/sizeof(WCHAR))-2] = UNICODE_NULL;
    status = _CmSetDeviceRegProp(PiPnpRtlCtx,
                                 instancePath.Buffer,
                                 instanceKey,
                                 CM_REG_PROP_HARDWAREID,
                                 REG_MULTI_SZ,
                                 (PUCHAR)hardwareIds.Buffer,
                                 length,
                                 0);

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // ConfigFlags = CONFIGFLAG_REINSTALL
    //

    value = CONFIGFLAG_REINSTALL;
    status = _CmSetDeviceRegProp(PiPnpRtlCtx,
                                 instancePath.Buffer,
                                 instanceKey,
                                 CM_REG_PROP_CONFIGFLAGS,
                                 REG_DWORD,
                                 (PUCHAR)&value,
                                 sizeof(value),
                                 0);

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Reported = 1
    //

    devpropBoolean = DEVPROP_TRUE;
    status = _PnpSetObjectProperty(PiPnpRtlCtx,
                                   instancePath.Buffer,
                                   PNP_OBJECT_TYPE_DEVICE,
                                   instanceKey,
                                   NULL,
                                   &DEVPKEY_Device_Reported,
                                   DEVPROP_TYPE_BOOLEAN,
                                   (PUCHAR)&devpropBoolean,
                                   sizeof(devpropBoolean),
                                   0);

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Service = <serviceName>
    //

    status = _CmSetDeviceRegProp(PiPnpRtlCtx,
                                 instancePath.Buffer,
                                 instanceKey,
                                 CM_REG_PROP_SERVICE,
                                 REG_SZ,
                                 (PUCHAR)serviceName->Buffer,
                                 (serviceName->Length + sizeof(UNICODE_NULL)),
                                 0);

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Mark the driver object as having successfully reported a root
    // device so it stays loaded if this is the only device for
    // the driver.
    //
    DriverObject->Flags |= DRVO_ROOT_DEVICE_REPORTED;

    //
    // Re-enumerate the root devnode to pick up this new device.
    //

    PnpRequestDeviceAction(IopRootDeviceNode->PhysicalDeviceObject,
                           ReenumerateDeviceOnly,
                           FALSE,
                           0,
                           NULL,
                           NULL);
exit:

    if ((!NT_SUCCESS(status)) && (created)) {
        _CmDeleteDevice(PiPnpRtlCtx, instancePath.Buffer, 0);
        PnpCleanupDeviceRegistryValues(&instancePath);
    }

    //
    // Unlock and cleanup.
    //

    PiUnlockPnpRegistry();
    PpDevNodeUnlockTree(PPL_TREEOP_ALLOW_READS);

    if (instanceKey != NULL) {
        ZwClose(instanceKey);
    }

    if (hPnpOpCtx) {
        PiPnpRtlEndOperation(hPnpOpCtx);
    }

    return status;
}

BOOLEAN
IopIsReportedAlready(
    __in PUNICODE_STRING InstanceName,
    __in HANDLE Handle,
    __in PUNICODE_STRING ServiceName,
    __in PCM_RESOURCE_LIST ResourceList,
    __out PLOGICAL MatchingKey
    )

/*++

Routine Description:

    This routine determines if the reported device instance is already reported
    or not.

Parameters:

    Handle - Supplies a handle to the reported device instance key.

    ServiceName - supplies a pointer to the unicode service key name.

    ResourceList - supplies a pointer to the reported Resource list.

    MatchingKey - supplies a pointer to a variable to receive whether the
        ServiceName and ResourceList properties for this key match those
        reported.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    PKEY_VALUE_FULL_INFORMATION keyValueInfo1 = NULL, keyValueInfo2 = NULL;
    NTSTATUS status;
    UNICODE_STRING unicodeName;
    HANDLE logConfHandle, controlHandle = NULL;
    BOOLEAN returnValue = FALSE;
    PCM_RESOURCE_LIST cmResource = NULL;
    ULONG tmpValue;
    ULONG propertyType = 0;
    ULONG propertySize;
    WCHAR serviceName[MAX_SERVICE_NAME_LEN];

    PAGED_CODE();

    //
    // Assume no match unless we determine otherwise.
    //
    *MatchingKey = FALSE;

    //
    // Check if "Service" value matches what the caller passed in.
    //

    propertySize = MAX_SERVICE_NAME_LEN * sizeof(WCHAR);
    status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                 InstanceName->Buffer,
                                 Handle,
                                 CM_REG_PROP_SERVICE,
                                 &propertyType,
                                 (PUCHAR)serviceName,
                                 &propertySize,
                                 0);

    PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);
    PNP_ASSERT(status != STATUS_BUFFER_OVERFLOW);

    if (NT_SUCCESS(status)) {
        if ((propertyType == REG_SZ) &&
            (propertySize != 0)) {
            status = RtlInitUnicodeStringEx(&unicodeName, serviceName);

            if ((NT_SUCCESS(status)) &&
                (RtlEqualUnicodeString(ServiceName, &unicodeName, TRUE))) {

                //
                // Next check if resources are the same
                //

                status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                             InstanceName->Buffer,
                                             CM_KEY_TYPE_DEVICE_LOGCONF,
                                             0,
                                             KEY_READ,
                                             FALSE,
                                             &logConfHandle,
                                             NULL);

                if (NT_SUCCESS(status)) {
                    status = IopGetRegistryValue(logConfHandle,
                                                 REGSTR_VAL_BOOTCONFIG,
                                                 0,
                                                 &keyValueInfo2);
                    ZwClose(logConfHandle);
                    if (NT_SUCCESS(status)) {
                        if ((keyValueInfo2->Type == REG_RESOURCE_LIST) &&
                            (keyValueInfo2->DataLength != 0)) {
                            cmResource = (PCM_RESOURCE_LIST)KEY_VALUE_DATA(keyValueInfo2);
                            if (ResourceList && cmResource &&
                                PnpIsDuplicateDevice(ResourceList, cmResource, NULL, NULL)) {
                                *MatchingKey = TRUE;
                            }
                        }
                    }
                }
                if (!ResourceList && !cmResource) {
                    *MatchingKey = TRUE;
                }
            }
        }
    }

    //
    // If this registry key is for a device reported during the same boot
    // this is not a duplicate.
    //

    status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                 InstanceName->Buffer,
                                 CM_KEY_TYPE_DEVICE_CONTROL,
                                 0,
                                 KEY_ALL_ACCESS,
                                 FALSE,
                                 &controlHandle,
                                 NULL
                                 );
    if (NT_SUCCESS(status)) {
        status = IopGetRegistryValue(controlHandle,
                                     REGSTR_VALUE_DEVICE_REPORTED,
                                     0,
                                     &keyValueInfo1);
    }

    if (NT_SUCCESS(status)) {

        returnValue = FALSE;

    } else if (*MatchingKey != FALSE) {

        status = STATUS_SUCCESS;

        //
        // Mark this key has been used.
        //

        if (controlHandle == NULL) {
            status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                         InstanceName->Buffer,
                                         CM_KEY_TYPE_DEVICE_CONTROL,
                                         0,
                                         KEY_ALL_ACCESS,
                                         TRUE,
                                         &controlHandle,
                                         NULL
                                         );
        }

        if (NT_SUCCESS(status)) {
            PNP_ASSERT(controlHandle != NULL);

            PiWstrToUnicodeString(&unicodeName, REGSTR_VALUE_DEVICE_REPORTED);
            tmpValue = 1;
            status = ZwSetValueKey(controlHandle,
                                   &unicodeName,
                                   TITLE_INDEX_VALUE,
                                   REG_DWORD,
                                   &tmpValue,
                                   sizeof(ULONG)
                                   );
        }

        if (NT_SUCCESS(status)) {
            returnValue = TRUE;
        } else {
            returnValue = FALSE;
        }
    }

    if (controlHandle) {
        ZwClose(controlHandle);
    }

    if (keyValueInfo1) {
        ExFreePool(keyValueInfo1);
    }

    if (keyValueInfo2) {
        ExFreePool(keyValueInfo2);
    }

    return returnValue;
}


__drv_maxIRQL(DISPATCH_LEVEL)
VOID
IoInvalidateDeviceState(
    __in PDEVICE_OBJECT PhysicalDeviceObject
    )

/*++

Routine Description:

    This API will cause the PnP manager to send the specified PDO an IRP_MN_QUERY_PNP_DEVICE_STATE
    IRP.

Parameters:

    PhysicalDeviceObject - Provides a pointer to the PDO who's state is to be invalidated.

Return Value:

    none.

--*/
{
    PDEVICE_NODE deviceNode;

    ASSERT_PDO(PhysicalDeviceObject);

    //
    // If the call was made before PnP completes device enumeration
    // we can safely ignore it.  PnP manager will do it without
    // driver's request.  If the device was already removed or surprised
    // removed then ignore it as well since this is only valid for started
    // devices.
    //

    deviceNode = (PDEVICE_NODE)PhysicalDeviceObject->DeviceObjectExtension->DeviceNode;

    if (deviceNode->State != DeviceNodeStarted) {
        return;
    }

    PnpRequestDeviceAction(PhysicalDeviceObject,
                           RequeryDeviceState,
                           FALSE,
                           0,
                           NULL,
                           NULL);
}


NTSTATUS
IopQueueDeviceWorkItem(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in VOID (*WorkerRoutine)(PVOID),
    __in_opt PVOID Context
    )

/*++

Routine Description:

    This API will cause the PnP manager to send the specified PDO an
    IRP_MN_QUERY_PNP_DEVICE_STATE IRP.

Parameters:

    PhysicalDeviceObject - Provides a pointer to the PDO who's state is to be
    invalidated.

Return Value:

    none.

--*/

{
    PDEVICE_WORK_ITEM deviceWorkItem;

    //
    // Since this routine can be called at DPC level we need to queue
    // a work item and process it when the irql drops.
    //

    deviceWorkItem = ExAllocatePool(NonPagedPoolNx, sizeof(DEVICE_WORK_ITEM));
    if (deviceWorkItem == NULL) {

        //
        // Failed to allocate memory for work item.  Nothing we can do ...
        //

        return STATUS_INSUFFICIENT_RESOURCES;
    }

    ObReferenceObject(PhysicalDeviceObject);
    deviceWorkItem->DeviceObject = PhysicalDeviceObject;
    deviceWorkItem->Context = Context;

    ExInitializeWorkItem( &deviceWorkItem->WorkItem,
                          WorkerRoutine,
                          deviceWorkItem);

    //
    // Queue a work item to do the enumeration
    //

    ExQueueWorkItem( &deviceWorkItem->WorkItem, DelayedWorkQueue );

    return STATUS_SUCCESS;
}


//
// Private routines
//
VOID
IopResourceRequirementsChanged(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in BOOLEAN StopRequired
    )

/*++

Routine Description:

    This routine handles request of device resource requirements list change.

Parameters:

    PhysicalDeviceObject - Provides a pointer to the PDO who's state is to be invalidated.

    StopRequired - Supplies a BOOLEAN value to indicate if the resources reallocation needs
                   to be done after device stopped.

Return Value:

    none.

--*/

{

    PAGED_CODE();

    PnpRequestDeviceAction(PhysicalDeviceObject,
                           ResourceRequirementsChanged,
                           FALSE,
                           StopRequired,
                           NULL,
                           NULL);
}


__drv_maxIRQL(PASSIVE_LEVEL)
__drv_preferredFunction("RtlIsNtDdiVersionAvailable","Preferred")
BOOLEAN
IoIsWdmVersionAvailable(
    __drv_when(MajorVersion!=1&&MajorVersion!=6,
    __in __drv_reportError("MajorVersion must be 1 or 6")) UCHAR MajorVersion,
    __in __drv_when(MinorVersion!=0 && MinorVersion!=5 && MinorVersion!=16
                    && MinorVersion!=32 && MinorVersion!=48,
    __drv_reportError("MinorVersion must be 0, 0x5, 0x10, 0x20, or 0x30"))
    UCHAR MinorVersion
    )

/*++

Routine Description:

    This routine reports whether WDM functionality is available that
    is greater than or equal to the specified major and minor version.

Parameters:

    MajorVersion - Supplies the WDM major version that is required.

    MinorVersion - Supplies the WDM minor version that is required.

Return Value:

    If WDM support is available at _at least_ the requested level, the
    return value is TRUE, otherwise it is FALSE.

--*/

{
    return ((MajorVersion < WDM_MAJORVERSION) ||
            ((MajorVersion == WDM_MAJORVERSION) && (MinorVersion <= WDM_MINORVERSION)));
}


_Must_inspect_result_
__drv_maxIRQL(PASSIVE_LEVEL)
__drv_valueIs(!=0;==0)
NTKERNELAPI
PDMA_ADAPTER
IoGetDmaAdapter(
    __in_opt PDEVICE_OBJECT PhysicalDeviceObject,
    __in PDEVICE_DESCRIPTION DeviceDescription,
    __out __drv_when(return!=0, __drv_IoGetDmaAdapter __deref _Must_inspect_result_)
    PULONG NumberOfMapRegisters
    )
/*++

Routine Description:

    This function returns the appropriate DMA adapter object for the device
    defined in the device description structure.  This code is a wrapper
    which queries the bus interface standard and then calls the returned
    get DMA adapter function.   If an adapter object was not retrieved then
    a legecy function is attempted.

Arguments:

    PhysicalDeviceObject - Optionally, supplies the PDO for the device
        requesting the DMA adapter.  If not supplied, this routine performs the
        function of the non-PnP HalGetDmaAdapter routine.

    DeviceDescriptor - Supplies a description of the deivce.

    NumberOfMapRegisters - Returns the maximum number of map registers which
        may be allocated by the device driver.

Return Value:

    A pointer to the requested adapter object or NULL if an adapter could not
    be created.

--*/

{

    NTSTATUS status;
    BUS_INTERFACE_STANDARD busInterface;
    PDMA_ADAPTER dmaAdapter = NULL;
    PDEVICE_DESCRIPTION deviceDescriptionToUse;
    DEVICE_DESCRIPTION privateDeviceDescription;
    ULONG DeviceDescriptionLength;
    ULONG resultLength;

    PAGED_CODE();

    if (PhysicalDeviceObject != NULL) {

        ASSERT_PDO(PhysicalDeviceObject);

        //
        // Bus drivers may call IoGetDmaAdapter recursively. In order to allow
        // the HAL to link a request back to the real device object, the
        // HAL is provided with a token now that it will map to the
        // device object associated with the first time it sees that token.
        //

        HalDmaLinkDeviceObjectByToken((ULONG_PTR)KeGetCurrentThread(),
                                      PhysicalDeviceObject);

        //
        // First off, determine whether or not the caller has requested that we
        // automatically fill in the proper InterfaceType value into the
        // DEVICE_DESCRIPTION structure used in retrieving the DMA adapter
        // object.  If so, then retrieve that interface type value into our
        // own copy of the DEVICE_DESCRIPTION buffer.
        //

        if ((DeviceDescription->InterfaceType == InterfaceTypeUndefined) ||
            (DeviceDescription->InterfaceType == PNPBus)) {

            //
            // Make a copy of the caller-supplied device description, so
            // we can modify it to fill in the correct interface type.
            //

            if (DeviceDescription->Version < DEVICE_DESCRIPTION_VERSION3) {
                DeviceDescriptionLength = RTL_SIZEOF_THROUGH_FIELD(
                                              DEVICE_DESCRIPTION,
                                              DmaPort);

            } else if (DeviceDescription->Version ==
                       DEVICE_DESCRIPTION_VERSION3) {

                DeviceDescriptionLength = RTL_SIZEOF_THROUGH_FIELD(
                                              DEVICE_DESCRIPTION,
                                              DeviceAddress);
            } else {

                PNP_ASSERT(FALSE);

                DeviceDescriptionLength = 0;
            }
            RtlCopyMemory(&privateDeviceDescription,
                          DeviceDescription,
                          DeviceDescriptionLength
                         );

            status = IoGetDeviceProperty(PhysicalDeviceObject,
                                         DevicePropertyLegacyBusType,
                                         sizeof(privateDeviceDescription.InterfaceType),
                                         (PVOID)&(privateDeviceDescription.InterfaceType),
                                         &resultLength
                                        );

            if (!NT_SUCCESS(status)) {

                PNP_ASSERT(status == STATUS_OBJECT_NAME_NOT_FOUND);

                //
                // Since the enumerator didn't tell us what interface type to
                // use for this PDO, we'll fall back to our default.  This is
                // ISA for machines where the legacy bus is ISA or EISA, and it
                // is MCA for machines whose legacy bus is MicroChannel.
                //
                privateDeviceDescription.InterfaceType = PnpDefaultInterfaceType;
            }

            //
            // Use our private device description buffer from now on.
            //
            deviceDescriptionToUse = &privateDeviceDescription;

        } else {
            //
            // Use the caller-supplied device description.
            //
            deviceDescriptionToUse = DeviceDescription;
        }

        status = PnpQueryInterface(PhysicalDeviceObject,
                                   &GUID_BUS_INTERFACE_STANDARD,
                                   1,
                                   sizeof(busInterface),
                                   NULL,
                                   (PINTERFACE) &busInterface);

        if (NT_SUCCESS( status)) {

            if (busInterface.GetDmaAdapter != NULL) {


                dmaAdapter = busInterface.GetDmaAdapter( busInterface.Context,
                                                         deviceDescriptionToUse,
                                                         NumberOfMapRegisters );

            }

            //
            // Dereference the interface
            //

            busInterface.InterfaceDereference( busInterface.Context );
        }

    } else {
        //
        // The caller didn't specify the PDO, so we'll just use the device
        // description exactly as they specified it (i.e., we can't attempt to
        // make our own determination of what interface type to use).
        //
        deviceDescriptionToUse = DeviceDescription;
    }

    //
    // If there is no DMA adapter, try the legacy mode code.
    //

#if !defined(NO_LEGACY_DRIVERS)

    if (dmaAdapter == NULL) {

        dmaAdapter = HalGetDmaAdapter( PhysicalDeviceObject,
                                       deviceDescriptionToUse,
                                       NumberOfMapRegisters );

    }

#endif // NO_LEGACY_DRIVERS

    if (PhysicalDeviceObject != NULL) {

        //
        // In case the HAL was never called, clean up the link between the
        // current thread and the PDO.
        //

        HalDmaLinkDeviceObjectByToken((ULONG_PTR)KeGetCurrentThread(), NULL);
    }

    return dmaAdapter;
}


NTSTATUS
PpCreateLegacyDeviceIds(
    __in PDEVICE_OBJECT DeviceObject,
    __in PUNICODE_STRING DriverName,
    __in PCM_RESOURCE_LIST Resources
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    PDEVICE_NODE deviceNode = NULL;
    PWCHAR compatibleIds = NULL, end = NULL;
    size_t cchRemaining = 0;
    ULONG length = 0;
    INTERFACE_TYPE interface;
    static const WCHAR* interfaceNames[] ={L"",
                              L"Internal",
                              L"Isa",
                              L"Eisa",
                              L"MicroChannel",
                              L"TurboChannel",
                              L"PCIBus",
                              L"VMEBus",
                              L"NuBus",
                              L"PCMCIABus",
                              L"CBus",
                              L"MPIBus",
                              L"MPSABus",
                              L"ProcessorInternal",
                              L"InternalPowerBus",
                              L"PNPISABus",
                              L"PNPBus",
                              L"Other",
                              L"Root",
                              L"Unknown"};

    PAGED_CODE();

    deviceNode = PP_DO_TO_DN(DeviceObject);

    if ((deviceNode == NULL) || PnpIsDeviceInstancePathEmpty(deviceNode)) {
        status = STATUS_INVALID_PARAMETER;
        goto exit;
    }

    if (Resources != NULL) {

        interface = Resources->List[0].InterfaceType;

        if ((interface > MaximumInterfaceType) ||
            (interface < InterfaceTypeUndefined)) {
            interface = MaximumInterfaceType;
        }
    } else {
        interface = Internal;
    }

    interface++;

    //
    // The compatible ID generated will be
    // DETECTED<InterfaceName>\<Driver Name>
    //

    length = (ULONG)(CSTRLEN(LEGACY_COMPATIBLE_ID_BASE) * sizeof(WCHAR));
    length += (ULONG)(wcslen(interfaceNames[interface]) * sizeof(WCHAR));
    length += sizeof(L'\\');
    length += DriverName->Length;
    length += sizeof(UNICODE_NULL);

    length += (ULONG)(CSTRLEN(LEGACY_COMPATIBLE_ID_BASE) * sizeof(WCHAR));
    length += sizeof(L'\\');
    length += DriverName->Length;
    length += sizeof(UNICODE_NULL) * 2;

    compatibleIds = ExAllocatePool(PagedPool, length);

    if (compatibleIds == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    RtlZeroMemory(compatibleIds, length);

    status = RtlStringCchPrintfExW(compatibleIds,
                                   length / sizeof(WCHAR),
                                   &end,
                                   &cchRemaining,
                                   0,
                                   L"%ws%ws\\%wZ",
                                   LEGACY_COMPATIBLE_ID_BASE,
                                   interfaceNames[interface],
                                   DriverName);

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // The variable "end" points to the null terminator of the first string, so
    // move past that to where the next ID should start.
    //

    end++;
    cchRemaining--;

    status = RtlStringCchPrintfW(end,
                                 cchRemaining,
                                 L"%ws\\%wZ",
                                 LEGACY_COMPATIBLE_ID_BASE,
                                 DriverName);

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    status = _CmSetDeviceRegProp(PiPnpRtlCtx,
                                 PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                 NULL,
                                 CM_REG_PROP_COMPATIBLEIDS,
                                 REG_MULTI_SZ,
                                 (PUCHAR)compatibleIds,
                                 length,
                                 0);

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

  exit:

    if (compatibleIds != NULL) {
        ExFreePool(compatibleIds);
    }

    return status;
}


BOOLEAN
IopAppendLegacyVeto(
    __in PIO_GET_LEGACY_VETO_LIST_CONTEXT Context,
    __in PUNICODE_STRING VetoName
    )
/*++

Routine Description:

    This routine appends a veto (driver name or device instance path) to the
    veto list.

Parameters:

    Context - An IO_GET_LEGACY_VETO_LIST_CONTEXT pointer.

    VetoName - The name of the driver/device to append to the veto list.

ReturnValue:

    A BOOLEAN which indicates whether the append operation was successful.

--*/
{
    ULONG Length;
    PWSTR Buffer;

    //
    // Compute the length of the (new) veto list.  This is the length of
    // the old veto list + the size of the new veto + the size of the
    // terminating '\0'.
    //

    Length = Context->VetoListLength + VetoName->Length + sizeof (WCHAR);

    //
    // Allocate the new veto list.
    //

    Buffer = ExAllocatePool(
                 NonPagedPoolNx,
                 Length
             );

    //
    // If we succeeded in allocating the new veto list, copy the old
    // veto list to the new list, append the new veto, and finally,
    // append a terminating '\0'.  Otherwise, update the status to
    // indicate an error; IopGetLegacyVetoList will free the veto list
    // before it returns.
    //

    if (Buffer != NULL) {

        if (*Context->VetoList != NULL) {

            RtlCopyMemory(
                Buffer,
                *Context->VetoList,
                Context->VetoListLength
            );

            ExFreePool(*Context->VetoList);

        }

        RtlCopyMemory(
            &Buffer[Context->VetoListLength / sizeof (WCHAR)],
            VetoName->Buffer,
            VetoName->Length
        );

        Buffer[Length / sizeof (WCHAR) - 1] = L'\0';

        *Context->VetoList = Buffer;
        Context->VetoListLength = Length;

        return TRUE;

    } else {

        *Context->Status = STATUS_INSUFFICIENT_RESOURCES;

        return FALSE;

    }
}


BOOLEAN
IopGetLegacyVetoListDevice(
    __in PDEVICE_NODE DeviceNode,
    __in PIO_GET_LEGACY_VETO_LIST_CONTEXT Context
    )
/*++

Routine Description:

    This routine determines whether the specified device node should be added to
    the veto list, and if so, calls IopAppendLegacyVeto to add it.

Parameters:

    DeviceNode - The device node to be added.

    Context - An IO_GET_LEGACY_VETO_LIST_CONTEXT pointer.

ReturnValue:

    A BOOLEAN value which indicates whether the device node enumeration
    process should be terminated or not.

--*/
{
    PDEVICE_CAPABILITIES DeviceCapabilities;

    //
    // A device node should be added added to the veto list, if it has the
    // NonDynamic capability.
    //

    DeviceCapabilities = IopDeviceNodeFlagsToCapabilities(DeviceNode);

    if (DeviceCapabilities->NonDynamic) {

        //
        // Update the veto type.  If an error occurrs while adding the device
        // node to the veto list, or the caller did not provide a veto list
        // pointer, terminate the enumeration process now.
        //

        *Context->VetoType = PNP_VetoLegacyDevice;

        if (Context->VetoList != NULL) {

            if (!IopAppendLegacyVeto(Context, PnpGetDeviceInstancePath(DeviceNode))) {
                return FALSE;
            }

        } else {

            return FALSE;

        }

    }

    return TRUE;
}


BOOLEAN
IopGetLegacyVetoListDeviceNode(
    __in PDEVICE_NODE DeviceNode,
    __in PIO_GET_LEGACY_VETO_LIST_CONTEXT Context
    )
/*++

Routine Description:

    This routine recusively walks the device tree, invoking
    IopGetLegacyVetoListDevice to add device nodes to the veto list
    (as appropriate).

Parameters:

    DeviceNode - The device node.

    Context - An IO_GET_LEGACY_VETO_LIST_CONTEXT pointer.


ReturnValue:

    A BOOLEAN value which indicates whether the device tree enumeration
    process should be terminated or not.

--*/
{
    PDEVICE_NODE Child;

    //
    // Determine whether the device node should be added to the veto
    // list and add it.  If an operation is unsuccessful or we determine
    // the veto type but the caller doesn't need the veto list, then we
    // terminate our search now.
    //

    if (!IopGetLegacyVetoListDevice(DeviceNode, Context)) {
        return FALSE;
    }

    //
    // Call ourselves recursively to enumerate our children.  If while
    // enumerating our children we determine we can terminate the search
    // prematurely, do so.
    //

    for (Child = DeviceNode->Child;
         Child != NULL;
         Child = Child->Sibling) {

        if (!IopGetLegacyVetoListDeviceNode(Child, Context)) {
            return FALSE;
        }

    }

    return TRUE;
}


VOID
IopGetLegacyVetoListDrivers(
    __in PIO_GET_LEGACY_VETO_LIST_CONTEXT Context
    )
{
    PDRIVER_OBJECT driverObject;
    OBJECT_ATTRIBUTES attributes;
    UNICODE_STRING driverString;
    UNICODE_STRING directoryString;
    POBJECT_DIRECTORY_INFORMATION dirInfo;
    HANDLE directoryHandle;
    ULONG dirInfoLength, neededLength, dirContext = 0;
    NTSTATUS status;
    BOOLEAN restartScan;
    USHORT objectPathLength;

    dirInfo = NULL;
    dirInfoLength = 0;
    directoryHandle = NULL;
    restartScan = TRUE;

    RtlZeroMemory(&driverString, sizeof(driverString));

    //
    // Get handle to \\Driver directory
    //

    PiWstrToUnicodeString(&directoryString, L"\\Driver");

    InitializeObjectAttributes(&attributes,
                               &directoryString,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               NULL,
                               NULL
                               );

    status = ZwOpenDirectoryObject(&directoryHandle,
                                   DIRECTORY_QUERY,
                                   &attributes
                                   );
    if (!NT_SUCCESS(status)) {
        *Context->Status = status;
        goto exit;
    }

    //
    // Initialize the directory info buffer to a reasonable length that will
    // prevent frequent re-sizing of the buffer as the directory is enumerated.
    //

    dirInfoLength = sizeof(OBJECT_DIRECTORY_INFORMATION) +
        (SERVICE_NAME_LEN_GUESS * sizeof(WCHAR));
    dirInfo = ExAllocatePool(PagedPool, dirInfoLength);
    if (dirInfo == NULL) {
        *Context->Status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    //
    // Initialize the driver string to a reasonable length that will prevent
    // frequent re-sizing of the string as object paths are created.
    //

    driverString.MaximumLength = sizeof(L"\\Driver\\") +
        (SERVICE_NAME_LEN_GUESS * sizeof(WCHAR));
    driverString.Length = 0;
    driverString.Buffer = ExAllocatePool(PagedPool,
                                         driverString.MaximumLength);
    if (driverString.Buffer == NULL) {
        *Context->Status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }
    driverString.Buffer[0] = UNICODE_NULL;

    for (;;) {

        //
        // Get info on next object in directory.  If the buffer is too
        // small, reallocate it and try again.  Otherwise, any failure
        // including STATUS_NO_MORE_ENTRIES breaks us out.
        //

        status = ZwQueryDirectoryObject(directoryHandle,
                                        dirInfo,
                                        dirInfoLength,
                                        TRUE,           // force one at a time
                                        restartScan,
                                        &dirContext,
                                        &neededLength);
        if (status == STATUS_BUFFER_TOO_SMALL) {
            dirInfoLength = neededLength;
            if (dirInfo != NULL) {
                ExFreePool(dirInfo);
            }
            dirInfo = ExAllocatePool(PagedPool, dirInfoLength);
            if (dirInfo == NULL) {
                *Context->Status = STATUS_INSUFFICIENT_RESOURCES;
                break;
            }
            status = ZwQueryDirectoryObject(directoryHandle,
                                            dirInfo,
                                            dirInfoLength,
                                            TRUE,       // force one at a time
                                            restartScan,
                                            &dirContext,
                                            &neededLength);
        }
        restartScan = FALSE;

        if (!NT_SUCCESS(status)) {
            break;
        }

        //
        // Have name of object.  Create object path and use
        // ObReferenceObjectByName() to get DriverObject.  This may fail
        // non-fatally if DriverObject has gone away in the interim.
        //

        objectPathLength = sizeof(L"\\Driver\\") + dirInfo->Name.Length;

        if (objectPathLength > driverString.MaximumLength) {
            ASSERT(driverString.Buffer != NULL);
            ExFreePool(driverString.Buffer);
            driverString.MaximumLength = objectPathLength;
            driverString.Buffer = ExAllocatePool(PagedPool,
                                                 driverString.MaximumLength);
            if (driverString.Buffer == NULL) {
                *Context->Status = STATUS_INSUFFICIENT_RESOURCES;
                break;
            }
        }

        driverString.Length = objectPathLength - sizeof(WCHAR);
        RtlStringCbPrintfW(driverString.Buffer,
                           driverString.MaximumLength,
                           L"\\Driver\\%ws",
                           dirInfo->Name.Buffer);

        status = ObReferenceObjectByName(&driverString,
                                         OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                         NULL,                 // access state
                                         0,                    // access mask
                                         IoDriverObjectType,
                                         KernelMode,
                                         NULL,                 // parse context
                                         &driverObject);

        if (NT_SUCCESS(status)) {
            PNP_ASSERT(driverObject->Type == IO_TYPE_DRIVER);
            if (driverObject->Flags & DRVO_LEGACY_RESOURCES) {
                //
                // Update the veto type.  If the caller provided a
                // veto list pointer, add the driver to the veto list.
                // If an error occurs while adding the driver to the
                // veto list, or the caller did not provide a veto
                // list pointer, terminate the driver enumeration now.
                //
                // NOTE: Driver may be loaded but not running,
                // distinction is not made here.


                *Context->VetoType = PNP_VetoLegacyDriver;

                if (Context->VetoList != NULL) {
                    IopAppendLegacyVeto(Context, &dirInfo->Name);
                }
            }
            ObDereferenceObject(driverObject);

            //
            // Early out if we have a veto and the caller didn't want a list or
            // we hit some error already
            //
            if (((*Context->VetoType == PNP_VetoLegacyDriver) &&
                (Context->VetoList == NULL)) ||
                !NT_SUCCESS(*Context->Status)) {
                break;
            }
        }
    }

  exit:

    if (driverString.Buffer != NULL) {
        ExFreePool(driverString.Buffer);
    }

    if (dirInfo != NULL) {
        ExFreePool(dirInfo);
    }

    if (directoryHandle != NULL) {
        ZwClose(directoryHandle);
    }
}


NTSTATUS
IoGetLegacyVetoList(
    __out_opt PWSTR *VetoList,
    __out PPNP_VETO_TYPE VetoType
    )
/*++

Routine Description:

    This routine is used by PNP and PO to determine whether legacy drivers and
    devices are installed in the system.  This routine is conceptually a
    QUERY_REMOVE_DEVICE and QUERY_POWER-like interface for legacy drivers
    and devices.

Parameters:

    VetoList - A pointer to a PWSTR. (Optional)  If specified,
        IoGetLegacyVetoList will allocate a veto list, and return a
        pointer to the veto list in VetoList.

    VetoType - A pointer to a PNP_VETO_TYPE.  If no legacy drivers
        or devices are found in the system, VetoType is assigned
        PNP_VetoTypeUnknown.  If one or more legacy drivers are installed,
        VetoType is assigned PNP_VetoLegacyDriver.  If one or more
        legacy devices are installed, VetoType is assigned
        PNP_VetoLegacyDevice.  VetoType is assigned independent of
        whether a VetoList is created.

ReturnValue:

    An NTSTATUS value indicating whether the IoGetLegacyVetoList() operation
    was successful.

--*/
{
    NTSTATUS Status;
    IO_GET_LEGACY_VETO_LIST_CONTEXT Context;
    UNICODE_STRING UnicodeString;

    PAGED_CODE();

    //
    // Initialize the veto list.
    //

    if (VetoList != NULL) {
        *VetoList = NULL;
    }

    //
    // Initialize the veto type.
    //

    PNP_ASSERT(VetoType != NULL);

    *VetoType = PNP_VetoTypeUnknown;

    //
    // Initialize the status.
    //

    Status = STATUS_SUCCESS;

    if (PnPInitialized == FALSE) {

        //
        // Can't touch anything, but nothing is really started either.
        //
        return Status;
    }

    //
    // Initialize our local context.
    //

    Context.VetoList = VetoList;
    Context.VetoListLength = 0;
    Context.VetoType = VetoType;
    Context.Status = &Status;

    //
    // Enumerate all driver objects.  This process can: (1) modify
    // the veto list, (2) modify the veto type and/or (3) modify the
    // status.
    //

    IopGetLegacyVetoListDrivers(&Context);

    //
    // If the driver enumeration process was successful and no legacy
    // drivers were detected, enumerate all device nodes.  The same
    // context values as above may be modified during device enumeration.
    //

    if (NT_SUCCESS(Status)) {

        if (*VetoType == PNP_VetoTypeUnknown) {

            PpDevNodeLockTree(PPL_SIMPLE_READ);

            IopGetLegacyVetoListDeviceNode(
                IopRootDeviceNode,
                &Context
            );

            PpDevNodeUnlockTree(PPL_SIMPLE_READ);
        }

    }

    //
    // If the previous operation(s) was/were successful, and the caller
    // provided a veto list pointer and we have constructed a veto
    // list, terminate the veto list with an empty string, i.e. MULTI_SZ.
    //

    if (NT_SUCCESS(Status)) {

        if (*VetoType != PNP_VetoTypeUnknown) {

            if (VetoList != NULL) {

                PiWstrToUnicodeString(
                    &UnicodeString,
                    L""
                );

                IopAppendLegacyVeto(
                    &Context,
                    &UnicodeString
                );

            }

        }

    }

    //
    // If a previous operation was unsuccessful, free any veto list we may have
    // allocated along the way.
    //

    if (!NT_SUCCESS(Status)) {

        if (VetoList != NULL && *VetoList != NULL) {
            ExFreePool(*VetoList);
            *VetoList = NULL;
        }

    }

    return Status;
}


NTSTATUS
PnpCompletionRoutine(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp,
    __in PKEVENT Event
    )
/*++

Routine Description:

    This function is used to stop further processing on an Irp which has been
    passed to IoForwardAndCatchIrp. It signals a event which has been passed
    in the context parameter to indicate that the Irp processing is complete.
    It then returns STATUS_MORE_PROCESSING_REQUIRED in order to stop processing
    on this Irp.

Arguments:

    DeviceObject -
        Contains the device which set up this completion routine.

    Irp -
        Contains the Irp which is being stopped.

    Event -
        Contains the event which is used to signal that this Irp has been
        completed.

Return Value:

    Returns STATUS_MORE_PROCESSING_REQUIRED in order to stop processing on the
    Irp.

--*/
{
    UNREFERENCED_PARAMETER( DeviceObject );
    UNREFERENCED_PARAMETER( Irp );

    //
    // This will allow the ForwardAndCatchIrp call to continue on its way.
    //
    KeSetEvent(Event, IO_NO_INCREMENT, FALSE);
    //
    // This will ensure that nothing else touches the Irp, since the original
    // caller has now continued, and the Irp may not exist anymore.
    //
    return STATUS_MORE_PROCESSING_REQUIRED;
}


NTKERNELAPI
NTSTATUS
IoSynchronousCallDriver(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    )
/*++

Routine Description:

    This function is used with devices which may be stacked, and may not use
    file objects to communicate.

    Forwards an IRP to the specified driver after initializing the next
    stack location, and regains control of the Irp on completion from that
    driver.

Arguments:

    DeviceObject -
        Contains the device to forward the Irp to.

    Irp -
        Contains the Irp which is being forwarded to the specified driver.

Return Value:

    Returns TRUE if the IRP was forwarded, else FALSE if no stack space
    was available.

--*/
{
    KEVENT Event;
    NTSTATUS Status;

    PNP_ASSERT(KeGetCurrentIrql() < DISPATCH_LEVEL);

    //
    // Set up a completion routine so that the Irp is not actually
    // completed. Thus the caller can get control of the Irp back after
    // this next driver is done with it.
    //
    KeInitializeEvent(&Event, NotificationEvent, FALSE);
    IoSetCompletionRoutine(Irp, PnpCompletionRoutine, &Event, TRUE, TRUE, TRUE);
    Status = IoCallDriver(DeviceObject, Irp);
    if (Status == STATUS_PENDING) {

        //
        // Wait for completion which will occur when the CompletionRoutine
        // signals this event. Wait in KernelMode so that the current stack
        // is not paged out, since there is an event object on this stack.
        //

        KeWaitForSingleObject(&Event,
                              Suspended,
                              KernelMode,
                              FALSE,
                              NULL);

        Status = Irp->IoStatus.Status;
    }

    return Status;
}


NTKERNELAPI
BOOLEAN
IoForwardAndCatchIrp(
    __in PDEVICE_OBJECT DeviceObject,
    __in PIRP Irp
    )
/*++

Routine Description:

    This function is used with devices which may be stacked, and may not use
    file objects to communicate.

    Forwards an IRP to the specified driver after initializing the next
    stack location, and regains control of the Irp on completion from that
    driver.

Arguments:

    DeviceObject -
        Contains the device to forward the Irp to.

    Irp -
        Contains the Irp which is being forwarded to the specified driver.

Return Value:

    Returns TRUE if the IRP was forwarded, else FALSE if no stack space
    was available.

--*/
{
    PAGED_CODE();
    //
    // Ensure that there is another stack location before copying parameters.
    //
    PNP_ASSERT(Irp->CurrentLocation > 1);
    if (Irp->CurrentLocation == 1) {
        return FALSE;
    }
    IoCopyCurrentIrpStackLocationToNext(Irp);

    IoSynchronousCallDriver(DeviceObject, Irp);

    return TRUE;
}

NTSTATUS
IoGetDeviceInstanceName (
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __out PUNICODE_STRING InstanceName
    )
{
    NTSTATUS status;
    PDEVICE_NODE deviceNode;

    ASSERT_PDO(PhysicalDeviceObject);

    deviceNode = PhysicalDeviceObject->DeviceObjectExtension->DeviceNode;
    status = PnpConcatenateUnicodeStrings(InstanceName,
                                          PnpGetDeviceInstancePath(deviceNode),
                                          NULL);
    return status;
}

VOID
IoControlPnpDeviceActionQueue(
    __in BOOLEAN Lock
    )
{
    if (Lock) {

        PnpLockDeviceActionQueue();
    } else {

        PnpUnlockDeviceActionQueue();
    }
}

__drv_preferredFunction("(see documentation)", "Obsolete")
NTSTATUS
IoAssignResources (
    __in      PUNICODE_STRING                 RegistryPath,
    __in_opt PUNICODE_STRING                 DriverClassName,
    __in      PDRIVER_OBJECT                  DriverObject,
    __in_opt PDEVICE_OBJECT                  DeviceObject,
    __in_opt PIO_RESOURCE_REQUIREMENTS_LIST  RequestedResources,
    __inout  PCM_RESOURCE_LIST               *pAllocatedResources
    )
/*++

Routine Description:

    This routine takes an input request of RequestedResources, and returned
    allocated resources in pAllocatedResources.   The allocated resources are
    automatically recorded in the registry under the ResourceMap for the
    DriverClassName/DriverObject/DeviceObject requestor.

Arguments:

    RegistryPath
        For a simple driver, this would be the value passed to the drivers
        initialization function.  For drivers call IoAssignResources with
        multiple DeviceObjects are responsible for passing in a unique
        RegistryPath for each object.

        The registry path is checked for:
            RegitryPath:
                AssignedSystemResources.

        AssignSystemResources is of type REG_RESOURCE_REQUIREMENTS_LIST

        If present, IoAssignResources will attempt to use these settings to
        satisify the requested resources.  If the listed settings do
        not conform to the resource requirements, then IoAssignResources
        will fail.

        Note: IoAssignResources may store other internal binary information
        in the supplied RegisteryPath.

    DriverObject:
        The driver object of the caller.

    DeviceObject:
        If non-null, then requested resoruce list refers to this device.
        If null, the requested resource list refers to the driver.

    DriverClassName
        Used to partition allocated resources into different device classes.

    RequestedResources
        A list of resources to allocate.

        Allocated resources may be appended or freed by re-invoking
        IoAssignResources with the same RegistryPath, DriverObject and
        DeviceObject.  (editing requirements on a resource list by using
        sucessive calls is not preferred driver behaviour).

    AllocatedResources
        Returns the allocated resources for the requested resource list.

        Note that the driver is responsible for passing in a pointer to
        an uninitialized pointer.  IoAssignResources will initialize the
        pointer to point to the allocated CM_RESOURCE_LIST.  The driver
        is responisble for returning the memory back to pool when it is
        done with them structure.

Return Value:

    The status returned is the final completion status of the operation.

--*/
{
    PDEVICE_NODE    deviceNode;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(RegistryPath);
    UNREFERENCED_PARAMETER(DriverClassName);

    if (DeviceObject) {

        deviceNode = (PDEVICE_NODE)DeviceObject->DeviceObjectExtension->DeviceNode;
        if (    deviceNode &&
                !(deviceNode->Flags & DNF_LEGACY_RESOURCE_DEVICENODE)) {

            PNP_SAVE_DRIVEROBJECT_TO_TRIAGE_DUMP(DriverObject);
            PNP_SAVE_DEVICEOBJECT_TO_TRIAGE_DUMP(DeviceObject);
            KeBugCheckEx(
                PNP_DETECTED_FATAL_ERROR,
                PNP_ERR_INVALID_PDO,
                (ULONG_PTR)DeviceObject,
                (ULONG_PTR)DriverObject,
                0);
        }
    }
    if (RequestedResources) {

        if (    RequestedResources->AlternativeLists == 0 ||
                RequestedResources->List[0].Count == 0) {

            RequestedResources = NULL;
        }
    }
    if (pAllocatedResources) {

        *pAllocatedResources = NULL;
    }
    return IopLegacyResourceAllocation (
            ArbiterRequestLegacyAssigned,
            DriverObject,
            DeviceObject,
            RequestedResources,
            pAllocatedResources);
}

NTSTATUS
IoConnectInterruptEx (
    __inout PIO_CONNECT_INTERRUPT_PARAMETERS Parameters
    )

/*++

Routine Description:

    This function builds interrupt objects and connects an interrupt service
    routine for the interrupts named in the given connection parameter
    structure.  This routine supercedes IoConnectInterrupt.

Arguments:

    Parameters - Supplies the parameters associated with this interrupt
        connection attempt.  See the definition of this structure for details
        on the meaning of the enclosed fields.

Return Value:

    If at least one interrupt is successfully connected then this routine
    returns STATUS_SUCCESS, returning the type of connection performed and any
    associated connection context through the given parameter structure.  If
    an interrupt cannot be connected then this routine returns an appropriate
    failure status, returning the maximum connection type supported by this
    routine if an unsupported connection type is requested by the caller.

Environment:

    PASSIVE_LEVEL.  This routine can only connect interrupts for devices that
    have been assigned resources and have received their IRP_MN_START_DEVICE
    IRP.  This routine is often called during the driver's processing of an
    IRP_MN_START_DEVICE or IRP_MN_SET_POWER request, although it can be called
    at other times.  No PnP or I/O manager locks are held with the exception
    of those that may be held while sending these IRPs (e.g., the resource
    manager lock can be held when sending IRP_MN_START_DEVICE IRPs).

--*/

{

    PINTERRUPT_CONNECTION_DATA ConnectionData;
    PIO_CONNECT_INTERRUPT_FULLY_SPECIFIED_PARAMETERS FullySpecified;
    INTERRUPT_CONNECTION_DATA FullySpecifiedData;
    USHORT Group;
    ULONG Index;
    PINTERRUPT_VECTOR_DATA InterruptData;
    PIO_INTERRUPT_STRUCTURE InterruptStructure;
    PIO_CONNECT_INTERRUPT_LINE_BASED_PARAMETERS LineBased;
    PIO_CONNECT_INTERRUPT_MESSAGE_BASED_PARAMETERS MessageBased;
    BOOLEAN PassiveInterrupt;
    NTSTATUS Status;
    GROUP_AFFINITY TargetProcessors;

    PAGED_CODE();

    //
    // N.B. Since this function is called infrequently, we can afford an extra
    // IRQL check.
    //

    if (KeGetCurrentIrql() != PASSIVE_LEVEL) {
        KeBugCheckEx(DRIVER_VIOLATION,
                     DRIVER_IRQL_VIOLATION,
                     KeGetCurrentIrql(),
                     PASSIVE_LEVEL,
                     0);
    }

    //
    // Attempt to connect the requested interrupt or interrupts in the manner
    // appropriate for the given connection type.
    //

    switch (Parameters->Version) {

        //
        // Fully specified connection attempt case.  In this case we are
        // explicitly given all of the information needed to call
        // IoConnectInterrupt.  Ensure that the caller has provided a device
        // object as required by the specification of this API, that an
        // interrupt service routine has been provided, and that the
        // given synchronization IRQL is valid before handing off to the
        // legacy routine.
        //

    case CONNECT_FULLY_SPECIFIED:
    case CONNECT_FULLY_SPECIFIED_GROUP:
        FullySpecified = &Parameters->FullySpecified;
        if (ARGUMENT_PRESENT(FullySpecified->PhysicalDeviceObject) == FALSE) {
            return STATUS_INVALID_PARAMETER;
        }

        if (ARGUMENT_PRESENT(FullySpecified->ServiceRoutine) == FALSE) {
            return STATUS_INVALID_PARAMETER;
        }

        if (FullySpecified->SynchronizeIrql < FullySpecified->Irql) {
            return STATUS_INVALID_PARAMETER;
        }

        //
        // Determine whether the request is for connecting a passive-level
        // interrupt.
        //

        if ((FullySpecified->SynchronizeIrql == PASSIVE_LEVEL) &&
            (FullySpecified->Irql == PASSIVE_LEVEL)) {

            PassiveInterrupt = TRUE;

        } else {
            PassiveInterrupt = FALSE;
        }

        //
        // A spinlock cannot be supplied when requesting passive-level
        // interrupt connection.
        //

        if ((PassiveInterrupt != FALSE) && (FullySpecified->SpinLock != NULL)) {
            return STATUS_INVALID_PARAMETER;
        }

        //
        // Get group number.
        //

        if (Parameters->Version == CONNECT_FULLY_SPECIFIED_GROUP) {
            Group = FullySpecified->Group;

        } else {
            Group = 0;
        }

        //
        // The fully specified connection parameters do not actually provide
        // all the information that is necessary to connect the interrupt.
        // The remaining information is stored as a device property of the PDO.
        // Retrieve this information, and as there is overlap between this
        // and the parameters, validate that the two copies are the same.
        //
        // If this information is not available, fall back to using the legacy
        // connection mechanism.  This can happen if the driver connecting the
        // interrupt is not on the same device stack as the driver that received
        // the interrupt resource.  This is discouraged behavior, but it exists
        // in the system today.  Removing support for IoConnectInterrupt is
        // desirable, but this case will need to be handled before that can
        // occur.
        //
        // The above special case is not supported if the caller wishes to
        // connect the interrupt for passive-level servicing.
        //

        Status = IopGetInterruptConnectionData(
                    FullySpecified->PhysicalDeviceObject,
                    &ConnectionData);

        if (!NT_SUCCESS(Status) && (PassiveInterrupt != FALSE)) {
            return STATUS_NOT_SUPPORTED;
        }

        if (NT_SUCCESS(Status)) {

            //
            // A fully specified connection request may be made to connect
            // a single request out of a multi-message device allocation.
            // Search for the vector that corresponds to the parameters passed
            // by the caller.
            //

            RtlZeroMemory(&FullySpecifiedData, sizeof(FullySpecifiedData));
            FullySpecifiedData.Count = 0;
            for (Index = 0; Index < ConnectionData->Count; Index++) {
                InterruptData = &ConnectionData->Vectors[Index];
                if ((InterruptData->Vector == FullySpecified->Vector) &&
                    ((InterruptData->Irql == FullySpecified->Irql) ||
                         (FullySpecified->Irql == PASSIVE_LEVEL)) &&
                    (InterruptData->Mode == FullySpecified->InterruptMode) &&
                    (InterruptData->TargetProcessors.Group == Group) &&
                    (InterruptData->TargetProcessors.Mask ==
                     FullySpecified->ProcessorEnableMask)) {

                    FullySpecifiedData.Count = 1;
                    RtlCopyMemory(&FullySpecifiedData.Vectors[0],
                                  &ConnectionData->Vectors[Index],
                                  sizeof(INTERRUPT_VECTOR_DATA));

                    //
                    // If the interrupt was arbitrated as "exclusive" then the
                    // arbiters will have guaranteed that there is only one
                    // device assigned to it.  If it is marked as "wake" then
                    // there may be two drivers that need to connect it,
                    // however. So convert any wake interrupt that is line-
                    // based to shared at this point, so that two drivers can
                    // have outstanding ISRs.
                    //

                    if ((FullySpecifiedData.Vectors[0].Type ==
                            InterruptTypeControllerInput) &&
                        (FullySpecifiedData.Vectors[0].ControllerInput.WakeInterrupt ==
                            1)) {

                        FullySpecified->ShareVector = TRUE;
                    }

                    break;
                }
            }

            if (FullySpecifiedData.Count == 0) {
                ExFreePool(ConnectionData);
                return STATUS_NOT_SUPPORTED;
            }

            Status = IopConnectInterrupt(&InterruptStructure,
                                         FullySpecified->ServiceRoutine,
                                         NULL,
                                         FullySpecified->ServiceContext,
                                         0,
                                         FullySpecified->SpinLock,
                                         FullySpecified->SynchronizeIrql,
                                         FullySpecified->ShareVector,
                                         FullySpecified->FloatingSave,
                                         &FullySpecifiedData);

            if (NT_SUCCESS(Status)) {
                *FullySpecified->InterruptObject =
                    &InterruptStructure->UserInterruptObject;
            }

            ExFreePool(ConnectionData);

        } else {
            RtlZeroMemory(&TargetProcessors, sizeof(GROUP_AFFINITY));
            TargetProcessors.Group = Group;
            TargetProcessors.Mask = FullySpecified->ProcessorEnableMask;
            Status = IopConnectInterruptFullySpecified(
                         FullySpecified->InterruptObject,
                         FullySpecified->ServiceRoutine,
                         FullySpecified->ServiceContext,
                         FullySpecified->SpinLock,
                         FullySpecified->Vector,
                         FullySpecified->Irql,
                         FullySpecified->SynchronizeIrql,
                         FullySpecified->InterruptMode,
                         FullySpecified->ShareVector,
                         &TargetProcessors,
                         FullySpecified->FloatingSave);
        }

        break;

        //
        // Line based connection attempt case.  Hand off to the line based
        // interrupt connection routine.
        //

    case CONNECT_LINE_BASED:
        LineBased = &Parameters->LineBased;
        Status = IopConnectLineBasedInterrupt(LineBased->PhysicalDeviceObject,
                                              LineBased->InterruptObject,
                                              LineBased->ServiceRoutine,
                                              LineBased->ServiceContext,
                                              LineBased->SpinLock,
                                              LineBased->SynchronizeIrql,
                                              LineBased->FloatingSave);

        break;

        //
        // Message based connection attempt case.  Attempt to connect to
        // the interrupt as a message-signaled interrupt.  If this fails and
        // a fallback service routine was provided then attempt to connect the
        // line based interrupt associated with this device by handing off to
        // the line based interrupt connection routine.
        //
        // Set the version field to indicate the connection type that was used.
        // If both connections fail, set the version to indicate the maximum
        // version supported by this routine, which is message-based connection.
        //

    case CONNECT_MESSAGE_BASED:
    case CONNECT_MESSAGE_BASED_PASSIVE:
        MessageBased = &Parameters->MessageBased;
        Status = IopConnectMessageBasedInterrupt(
                     Parameters->Version,
                     MessageBased->PhysicalDeviceObject,
                     MessageBased->ConnectionContext.InterruptMessageTable,
                     MessageBased->MessageServiceRoutine,
                     MessageBased->ServiceContext,
                     MessageBased->SpinLock,
                     MessageBased->SynchronizeIrql,
                     MessageBased->FloatingSave);

        if (NT_SUCCESS(Status)) {
            return Status;
        }

        if (ARGUMENT_PRESENT(Parameters->MessageBased.FallBackServiceRoutine)) {
            MessageBased = &Parameters->MessageBased;
            Status = IopConnectLineBasedInterrupt(
                         MessageBased->PhysicalDeviceObject,
                         MessageBased->ConnectionContext.InterruptObject,
                         MessageBased->FallBackServiceRoutine,
                         MessageBased->ServiceContext,
                         MessageBased->SpinLock,
                         MessageBased->SynchronizeIrql,
                         MessageBased->FloatingSave);

            //
            // Provide an indication to the caller that we attempted line
            // based interrupt connection in response to this request.
            //

            Parameters->Version = CONNECT_LINE_BASED;

        } else {
            return STATUS_NOT_SUPPORTED;
        }

        break;

        //
        // Unrecognized connection attempt case.  Pass the maximum connection
        // type supported by this routine back to the caller and return a
        // failure status.
        //

    default:
        Parameters->Version = CONNECT_MESSAGE_BASED;
        return STATUS_NOT_SUPPORTED;
    }

    return Status;
}

__drv_maxIRQL(PASSIVE_LEVEL)
NTSTATUS
IoConnectInterrupt (
    __out PKINTERRUPT *InterruptObject,
    __in PKSERVICE_ROUTINE ServiceRoutine,
    __in_opt PVOID ServiceContext,
    __in_opt PKSPIN_LOCK SpinLock,
    __in ULONG Vector,
    __in KIRQL Irql,
    __in KIRQL SynchronizeIrql,
    __in KINTERRUPT_MODE InterruptMode,
    __in BOOLEAN ShareVector,
    __in KAFFINITY ProcessorEnableMask,
    __in BOOLEAN FloatingSave
    )

/*++

Routine Description:

    This routine allocates, initializes, and connects interrupt objects for
    all of the processors specified in the processor enable mask.   This routine
    is legacy and should be deprecated.  IoConnectInterruptEx supercedes its
    functionality.

    N.B. This API is obsolete and is kept for backward compatibility only. The
         ProcessorEnableMask is a processor mask for group 0.

Arguments:

    InterruptObject - Address of a variable to receive a pointer to the first
        interrupt object allocated and initialized.

    ServiceRoutine - Address of the interrupt service routine (ISR) that should
        be executed when the interrupt occurs.

    ServiceContext - Supplies a pointer to the context information required
        by the ISR.

    SpinLock - Supplies a pointer to a spin lock to be used when synchronizing
        with the ISR.

    Vector - Supplies the vector upon which the interrupt occurs.

    Irql - Supplies the IRQL upon which the interrupt occurs.

    SynchronizeIrql - The request priority that the interrupt should be
        synchronized with.

    InterruptMode - Specifies the interrupt mode of the device.

    ShareVector - Supplies a boolean value that specifies whether the
        vector can be shared with other interrupt objects or not.  If FALSE
        then the vector may not be shared, if TRUE it may be.
        Latched.

    ProcessorEnableMask - Specifies a bit-vector for each processor on which
        the interrupt is to be connected.  A value of one in the bit position
        cooresponding to the processor number indicates that the interrupt
        should be allowed on that processor.  At least one bit must be set.

    FloatingSave - A BOOLEAN that specifies whether or not the machine's
        floating point state should be saved before invoking the ISR.

Return Value:

    The function value is the final function status.  The three status values
    that this routine can itself return are:

        STATUS_SUCCESS - Everything worked successfully.
        STATUS_INVALID_PARAMETER - No processors were specified.
        STATUS_INSUFFICIENT_RESOURCES - There was not enough nonpaged pool.

--*/

{

    GROUP_AFFINITY TargetProcessors;

    PAGED_CODE();

    //
    // N.B. Since this function is called infrequently, we can afford an extra
    // IRQL check.
    //

    if (KeGetCurrentIrql() != PASSIVE_LEVEL) {
        KeBugCheckEx(DRIVER_VIOLATION,
                     DRIVER_IRQL_VIOLATION,
                     KeGetCurrentIrql(),
                     PASSIVE_LEVEL,
                     0);
    }

    RtlZeroMemory(&TargetProcessors, sizeof(GROUP_AFFINITY));
    TargetProcessors.Mask = ProcessorEnableMask;
    return IopConnectInterruptFullySpecified(InterruptObject,
                                             ServiceRoutine,
                                             ServiceContext,
                                             SpinLock,
                                             Vector,
                                             Irql,
                                             SynchronizeIrql,
                                             InterruptMode,
                                             ShareVector,
                                             &TargetProcessors,
                                             FloatingSave);
}

NTSTATUS
IopConnectInterruptFullySpecified (
    __out PKINTERRUPT *InterruptObject,
    __in PKSERVICE_ROUTINE ServiceRoutine,
    __in_opt PVOID ServiceContext,
    __in_opt PKSPIN_LOCK SpinLock,
    __in ULONG Vector,
    __in KIRQL Irql,
    __in KIRQL SynchronizeIrql,
    __in KINTERRUPT_MODE InterruptMode,
    __in BOOLEAN ShareVector,
    __in PGROUP_AFFINITY TargetProcessors,
    __in BOOLEAN FloatingSave
    )

/*++

Routine Description:

    This routine allocates, initializes, and connects interrupt objects for
    all of the processors specified in the processor enable mask.   This routine
    is legacy and should be deprecated.  IoConnectInterruptEx supercedes its
    functionality.

Arguments:

    InterruptObject - Address of a variable to receive a pointer to the first
        interrupt object allocated and initialized.

    ServiceRoutine - Address of the interrupt service routine (ISR) that should
        be executed when the interrupt occurs.

    ServiceContext - Supplies a pointer to the context information required
        by the ISR.

    SpinLock - Supplies a pointer to a spin lock to be used when synchronizing
        with the ISR.

    Vector - Supplies the vector upon which the interrupt occurs.

    Irql - Supplies the IRQL upon which the interrupt occurs.

    SynchronizeIrql - The request priority that the interrupt should be
        synchronized with.

    InterruptMode - Specifies the interrupt mode of the device.

    ShareVector - Supplies a boolean value that specifies whether the
        vector can be shared with other interrupt objects or not.  If FALSE
        then the vector may not be shared, if TRUE it may be.
        Latched.

    TargetProcessors - Specifies the group affinity of each processor on which
        the interrupt is to be connected.

    FloatingSave - A BOOLEAN that specifies whether or not the machine's
        floating point state should be saved before invoking the ISR.

Return Value:

    The function value is the final function status.  The four status values
    that this routine can itself return are:

        STATUS_SUCCESS - Everything worked successfully.
        STATUS_INVALID_PARAMETER - No processors were specified.
        STATUS_INSUFFICIENT_RESOURCES - There was not enough nonpaged pool.
        STATUS_NOT_SUPPORTED - The interrupt has a polarity of
            InterruptActiveBothTriggerLow or InterruptActiveBothTriggerHigh,
            and is targeted to the primary interrupt controller.
            IoConnectInterruptEx must be used instead.

--*/

{

    INTERRUPT_CONNECTION_DATA ConnectionData;
    ULONG Gsiv;
    PINTERRUPT_VECTOR_DATA InterruptData;
    PIO_INTERRUPT_STRUCTURE InterruptStructure;
    KINTERRUPT_POLARITY Polarity;
    NTSTATUS Status;

    //
    // Verify that a service routine has been provided, that the request is
    // to target one or more processors, and that the IRQL and synchronization
    // IRQL are valid with respect to each other.
    //

    if (ARGUMENT_PRESENT(ServiceRoutine) == FALSE) {
        return STATUS_INVALID_PARAMETER;
    }

    NT_ASSERT(ARGUMENT_PRESENT(TargetProcessors) != FALSE);

    if (KeVerifyGroupAffinity(TargetProcessors, FALSE) == FALSE) {
        return STATUS_INVALID_PARAMETER;
    }

    if (ARGUMENT_PRESENT(SynchronizeIrql) == FALSE) {
        SynchronizeIrql = Irql;

    } else if (SynchronizeIrql < Irql) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // IopConnectInterruptFullySpecified can only be called to connect a
    // line-based interrupt.
    //

    ConnectionData.Count = 1;
    InterruptData = &ConnectionData.Vectors[0];

    //
    // A line-based interrupt has only one source and one target vector, so
    // the connection data only has one resource to describe.  Fill in this
    // structure from the caller's parameters, filling in the necessary gaps
    // from the HAL.
    //

    InterruptData->Type = InterruptTypeControllerInput;
    InterruptData->Vector = Vector;
    InterruptData->Irql = Irql;
    InterruptData->Mode = InterruptMode;
    InterruptData->TargetProcessors = *TargetProcessors;
    Status = HalGetVectorInput(Vector,
                               &InterruptData->TargetProcessors,
                               &Gsiv,
                               &Polarity,
                               &InterruptData->IntRemapInfo);

    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // ActiveBoth primary interrupts are new to Windows Blue. Prevent their
    // usage with the legacy IoConnectInterrupt. This includes the case where
    // IoConnectInterruptEx is used to connect a primary interrupt resource
    // from a different device stack.
    //

    if ((HalIsInterruptTypeSecondary(InterruptTypeControllerInput, Gsiv)
         == FALSE)) {

        if ((Polarity == InterruptActiveBothTriggerLow) ||
            (Polarity == InterruptActiveBothTriggerHigh)) {

            return STATUS_NOT_SUPPORTED;
        }
    }

    InterruptData->Polarity = Polarity;
    InterruptData->ControllerInput.Gsiv = Gsiv;

    //
    // If the interrupt was arbitrated as "exclusive" then the arbiters will
    // have guaranteed that there is only one device assigned to it.  If it is
    // marked as "wake" then there may be two drivers that need to connect it,
    // however. So convert any wake interrupt that is line-based to shared at
    // this point, so that two drivers can have outstanding ISRs.
    //

    if (InterruptData->ControllerInput.WakeInterrupt == 1) {
        ShareVector = TRUE;
    }

    //
    // Call the common connection routine.
    //

    *InterruptObject = NULL;
    Status = IopConnectInterrupt(&InterruptStructure,
                                 ServiceRoutine,
                                 NULL,
                                 ServiceContext,
                                 0,
                                 SpinLock,
                                 SynchronizeIrql,
                                 ShareVector,
                                 FloatingSave,
                                 &ConnectionData);

    //
    // Return a pointer to the first processor-specific interrupt object in
    // the structure.  This is opaque to the caller.
    //

    if (NT_SUCCESS(Status)) {
        *InterruptObject = &InterruptStructure->UserInterruptObject;
    }

    return Status;
}

NTSTATUS
IopConnectLineBasedInterrupt (
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __out PKINTERRUPT *InterruptObject,
    __in PKSERVICE_ROUTINE ServiceRoutine,
    __in PVOID ServiceContext,
    __in_opt PKSPIN_LOCK SpinLock,
    __in_opt KIRQL SynchronizeIrql,
    __in BOOLEAN FloatingSave
    )

/*++

Routine Description:

    This function attempts to build interrupt objects and connect an interrupt
    service routine for the line based interrupt associated with the given
    physical device object.

Arguments:

    PhysicalDeviceObject - Supplies the physical device object associated with
        the line based interrupt being connected.  In order to correctly
        determine the interrupt to connect, this is generally required to be
        the physical device object associated with the device that generates
        the interrupt of interest.

    InterruptObject - Supplies a pointer to the location that will be used to
        return a pointer to the interrupt object allocated in association with
        the interrupt being connected.

    ServiceRoutine - Supplies the address of the interrupt service routine
        (ISR) that should be executed when the interrupt occurs.

    ServiceContext - Supplies an opaque pointer to the driver context
        information that should be passed to the ISR.

    SpinLock - Supplies an optional pointer to a spin lock that will be
        acquired before every call to the ISR.  After providing a spin lock,
        the driver can synchronize with the ISR by acquiring the spin lock at
        the synchronization IRQL associated with the interrupt.  If this
        parameter is not provided, then an internal spin lock will be acquired
        before each call to the ISR.  The driver can use
        KeSynchronizeExecution to acquire this internal spin lock at the
        appropriate IRQL and thus synchronize with the ISR.

    SynchronizeIrql - Supplies an optional IRQL at which the interrupt spin
        lock should be acquired and at which the ISR should be executed.  If a
        nonzero value is provided for this parameter, it must be greater than
        or equal to the IRQL associated with the interrupt.  This parameter is
        most often used in conjunction with a caller provided spin lock to
        serialize ISR execution across multiple interrupts, however it can
        also be used without a spin lock to block this ISR from directly
        preempting or being directly preempted by some other ISR.  If this
        parameter is omitted then the IRQL of the interrupt being connected is
        used as the sychronization IRQL, both in the case where the caller
        provides a spin lock and in the case where the spin lock is omitted.

    FloatingSave - Supplies an indication of whether or not the machine's
        floating point state should be saved before invoking the ISR.

Return Value:

    If the given device's line based interrupt is successfully connected then
    this routine returns STATUS_SUCCESS.  Otherwise an appropriate failure
    status is returned.

Environment:

    Same as IoConnectInterruptEx.

--*/

{

    PINTERRUPT_CONNECTION_DATA ConnectionData;
    ULONG Index;
    PIO_INTERRUPT_LINE_INFO InterruptTable;
    ULONG InterruptTableSize;
    PIO_INTERRUPT_STRUCTURE *InterruptStructure;
    INTERRUPT_CONNECTION_DATA LineBasedData;
    BOOLEAN ShareVector;
    NTSTATUS Status;
    KIRQL UnifiedIrql;
    ULONG VectorCount;
    ULONG VectorIndex;
    PINTERRUPT_VECTOR_DATA VectorData;

    PAGED_CODE();

    PNP_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    ConnectionData = NULL;
    *InterruptObject = NULL;
    InterruptTable = NULL;
    VectorIndex = 0;

    //
    // Verify that both a physical device object (PDO) and an interrupt
    // service routine have been provided before proceeding.
    //

    if (ARGUMENT_PRESENT(PhysicalDeviceObject) == FALSE) {
        return STATUS_INVALID_PARAMETER;
    }

    if (IS_PDO(PhysicalDeviceObject) == FALSE) {
        return STATUS_INVALID_PARAMETER;
    }

    if (ARGUMENT_PRESENT(ServiceRoutine) == FALSE) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Retrieve the data describing the interrupt resources assigned to
    // this device.
    //

    Status = IopGetInterruptConnectionData(PhysicalDeviceObject,
                                           &ConnectionData);

    if ((!NT_SUCCESS(Status)) || (ConnectionData == NULL)) {
        return Status;
    }

    //
    // Find out how many line based vectors there are and get the highest IRQL.
    //

    VectorCount = 0;
    UnifiedIrql = PASSIVE_LEVEL;
    for (Index = 0; Index < ConnectionData->Count; Index++) {
        VectorData = &ConnectionData->Vectors[Index];
        if (VectorData->Type == InterruptTypeControllerInput) {
            VectorCount += 1;
            if (UnifiedIrql < VectorData->Irql) {
                UnifiedIrql = VectorData->Irql;
            }
        }
    }

    if (VectorCount == 0) {
        Status = STATUS_INVALID_PARAMETER;
        goto IopConnectLineBasedInterruptExit;
    }

    //
    // Validate that the synchronization IRQL passed in by the caller is at
    // least the highest IRQL associated with the vector(s) being connected to.
    // If the caller did not provide a synchronization IRQL, use the vector(s)
    // highest IRQL.
    //

    if (ARGUMENT_PRESENT(SynchronizeIrql) != FALSE) {
        if (SynchronizeIrql < UnifiedIrql) {
            Status = STATUS_INVALID_PARAMETER;
            goto IopConnectLineBasedInterruptExit;
        }

        UnifiedIrql = SynchronizeIrql;
    }

    //
    // The share vector parameter to IopConnectInterrupt indicates whether it
    // is allowable for multiple interrupt objects to be connected to this
    // interrupt on a single processor.  Sharing will never cause a functional
    // problem with line based interrupts and allows a single device stack to
    // connect multiple service routines to its single line based interrupt if
    // desired.  As a result, all line based interrupts will be connected with
    // vector sharing enabled regardless of the share disposition encoded into
    // the interrupt resource descriptor.
    //

    ShareVector = TRUE;


    //
    // Allocate enough memory to store all the vector data.
    //

    InterruptTableSize = sizeof(IO_INTERRUPT_LINE_INFO);
    InterruptTableSize += (sizeof(PIO_INTERRUPT_STRUCTURE) * (VectorCount - 1));
    InterruptTable = ExAllocatePoolWithTag(NonPagedPoolNx,
                                           InterruptTableSize,
                                           'nioI');

    if (InterruptTable == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto IopConnectLineBasedInterruptExit;
    }

    RtlZeroMemory(InterruptTable, InterruptTableSize);

    //
    // Initialize the interrupt table.
    //

    InterruptTable->UnifiedIrql = UnifiedIrql;
    InterruptTable->InterruptCount = VectorCount;
    RtlZeroMemory(&LineBasedData, sizeof(INTERRUPT_CONNECTION_DATA));
    LineBasedData.Count = 1;
    for (Index = 0; Index < ConnectionData->Count; Index++) {
        VectorData = &ConnectionData->Vectors[Index];
        if (VectorData->Type == InterruptTypeControllerInput) {

            //
            // Connect the interrupt, creating an interrupt structure to track
            // the connection.  If this succeeds, return a pointer to the first
            // processor specific interrupt object within the structure.  This
            // is opaque to the caller. The connection must happen one line at
            // a time since IopConnectInterrupt doesn't support multiple
            // connections.
            //

            RtlCopyMemory(&LineBasedData.Vectors[0],
                          VectorData,
                          sizeof(INTERRUPT_VECTOR_DATA));

            InterruptStructure = &InterruptTable->InterruptInfo[VectorIndex];
            Status = IopConnectInterrupt(InterruptStructure,
                                         ServiceRoutine,
                                         NULL,
                                         ServiceContext,
                                         0,
                                         SpinLock,
                                         UnifiedIrql,
                                         ShareVector,
                                         FloatingSave,
                                         &LineBasedData);
            if (!NT_SUCCESS(Status)) {
                goto IopConnectLineBasedInterruptExit;
            }

            VectorIndex += 1;
            if (VectorIndex >= VectorCount) {
                break;
            }
        }
    }

    //
    // Set and return the user's interrupt object.
    //

    //
    // Suppress 26000: Buffer Overflow.
    // Prefast mistakenly believes that IO_INTERRUPT_STRUCTURE pointed by
    // InterruptInfo[0] is part of InterruptTable buffer. In reality is
    // allocated by IopConnectInterrupt and only the pointer is stored.
    //

#pragma prefast(suppress:__WARNING_BUFFER_OVERFLOW)
    InterruptTable->UserInterruptObject =
        InterruptTable->InterruptInfo[0]->UserInterruptObject;

    *InterruptObject = &InterruptTable->UserInterruptObject;

IopConnectLineBasedInterruptExit:
    ExFreePool(ConnectionData);
    if (!NT_SUCCESS(Status)) {
        if (InterruptTable != NULL) {
            for (Index = 0; Index < VectorIndex; Index +=1) {
                IoDisconnectInterrupt(
                  &(InterruptTable->InterruptInfo[Index]->UserInterruptObject));
            }

            ExFreePool(InterruptTable);
        }
    }

    return Status;
}

NTSTATUS
IopConnectMessageBasedInterrupt (
    __in ULONG Version,
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __out PIO_INTERRUPT_MESSAGE_INFO *MessageTableOut,
    __in PKMESSAGE_SERVICE_ROUTINE ServiceRoutine,
    __in PVOID ServiceContext,
    __in_opt PKSPIN_LOCK SpinLock,
    __in_opt KIRQL SynchronizeIrql,
    __in BOOLEAN FloatingSave
    )

/*++

Routine Description:

    This function attempts to build interrupt objects and connect an interrupt
    service routine for the message based interrupt associated with the given
    physical device object.

Arguments:

    Version - Supplies the version of message connection, either CONNECT_MESSAGE_BASED
              or CONNECT_MESSAGE_BASED_PASSIVE.

    PhysicalDeviceObject - Supplies the physical device object associated with
        the line based interrupt being connected.  In order to correctly
        determine the interrupt to connect, this is generally required to be
        the physical device object associated with the device that generates
        the interrupt of interest.

    MessageTableOut - Supplies a pointer to the location that will be used to
        return a pointer to the interrupt message table allocated in association
        with the interrupt being connected.

    ServiceRoutine - Supplies the address of the interrupt message service
        routine (IMSR) that should be executed when the interrupt occurs.

    ServiceContext - Supplies an opaque pointer to the driver context
        information that should be passed to the ISR.

    SpinLock - Supplies an optional pointer to a spin lock that will be
        acquired before every call to the ISR.  After providing a spin lock,
        the driver can synchronize with the ISR by acquiring the spin lock at
        the synchronization IRQL associated with the interrupt.  If this
        parameter is not provided, then an internal spin lock will be acquired
        before each call to the ISR.  The driver can use
        KeSynchronizeExecution to acquire this internal spin lock at the
        appropriate IRQL and thus synchronize with the ISR.

    SynchronizeIrql - Supplies an optional IRQL at which the interrupt spin
        lock should be acquired and at which the ISR should be executed.  If a
        nonzero value is provided for this parameter, it must be greater than
        or equal to the IRQL associated with the interrupt.  This parameter is
        most often used in conjunction with a caller provided spin lock to
        serialize ISR execution across multiple interrupts, however it can
        also be used without a spin lock to block this ISR from directly
        preempting or being directly preempted by some other ISR.  If this
        parameter is omitted then the IRQL of the interrupt being connected is
        used as the sychronization IRQL, both in the case where the caller
        provides a spin lock and in the case where the spin lock is omitted.

    FloatingSave - Supplies an indication of whether or not the machine's
        floating point state should be saved before invoking the ISR.

Return Value:

    If the given device's message based interrupt is successfully connected then
    this routine returns STATUS_SUCCESS.  Otherwise an appropriate failure
    status is returned.

Environment:

    Same as IoConnectInterruptEx.

--*/

{

    PINTERRUPT_CONNECTION_DATA ConnectionData;
    ULONG Index;
    PIO_INTERRUPT_STRUCTURE InterruptStructure;
    ULONG MessageCount;
    PINTERRUPT_VECTOR_DATA MessageData;
    INTERRUPT_CONNECTION_DATA MessageConnectionData;
    PIO_INTERRUPT_MESSAGE_INFO_ENTRY OutputEntry;
    PIO_INTERRUPT_MESSAGE_INFO OutputTable;
    BOOLEAN PassiveInterrupt;
    HAL_MESSAGE_TARGET_REQUEST Request;
    BOOLEAN ShareVector;
    ULONG Size;
    NTSTATUS Status;
    KIRQL UnifiedIrql;

    PAGED_CODE();

    PNP_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    MessageCount = 0;
    *MessageTableOut = NULL;
    ConnectionData = NULL;
    OutputTable = NULL;
    PassiveInterrupt = FALSE;
    UnifiedIrql = 0;
    RtlZeroMemory(&MessageConnectionData, sizeof(INTERRUPT_CONNECTION_DATA));

    //
    // Verify that both a physical device object (PDO) and an interrupt
    // service routine have been provided before proceeding.
    //

    if (ARGUMENT_PRESENT(PhysicalDeviceObject) == FALSE) {
        Status = STATUS_INVALID_PARAMETER;
        goto IopConnectMessageBasedInterruptExit;
    }

    if (IS_PDO(PhysicalDeviceObject) == FALSE) {
        Status = STATUS_INVALID_PARAMETER;
        goto IopConnectMessageBasedInterruptExit;
    }

    if (ARGUMENT_PRESENT(ServiceRoutine) == FALSE) {
        Status = STATUS_INVALID_PARAMETER;
        goto IopConnectMessageBasedInterruptExit;
    }

    //
    // Retrieve the data describing the interrupt resources assigned to
    // this device.  This may include many interrupt message descriptors, but
    // also some non-message descriptors.
    //

    Status = IopGetInterruptConnectionData(PhysicalDeviceObject,
                                           &ConnectionData);

    if (!NT_SUCCESS(Status)) {
        goto IopConnectMessageBasedInterruptExit;
    }

    //
    // Parse the connection data for the number of message descriptors as well
    // as any appropriate synchronization IRQL.
    //

    for (Index = 0; Index < ConnectionData->Count; Index++) {
        MessageData = &ConnectionData->Vectors[Index];
        if ((MessageData->Type != InterruptTypeXapicMessage) &&
            (MessageData->Type != InterruptTypeHypertransport) &&
            (MessageData->Type != InterruptTypeMessageRequest)) {

            continue;
        }

        MessageCount++;

        if (Version == CONNECT_MESSAGE_BASED_PASSIVE) {
            PassiveInterrupt = TRUE;

        } else if (Version == CONNECT_MESSAGE_BASED) {

            //
            // If appropriate, generate the unified IRQL that will be used as the
            // priority level for all message requests.  Multiple message requests
            // are not required to have a unified IRQL.  A caller expresses a
            // request for a unified IRQL by providing either a spin lock or a
            // synchronization IRQL.
            //

            if (ARGUMENT_PRESENT(SynchronizeIrql)) {

                //
                // A synchronization IRQL is provided.  Use it as the unified IRQL,
                // after verifying that it is compatible with the IRQLs at which
                // each message will be signaled.
                //

                if (SynchronizeIrql < MessageData->Irql) {
                    Status = STATUS_INVALID_PARAMETER;
                    goto IopConnectMessageBasedInterruptExit;
                }

                UnifiedIrql = SynchronizeIrql;

            } else if (ARGUMENT_PRESENT(SpinLock)) {

                //
                // A spin lock is provided.  Infer the unified IRQL from the individual
                // IRQLs of the messages, using the highest IRQL of the set.
                //

                if (ConnectionData->Vectors[Index].Irql > UnifiedIrql) {
                    UnifiedIrql = ConnectionData->Vectors[Index].Irql;
                }

            } else {
                UnifiedIrql = 0;
            }
        }
    }

    //
    // If there are no messages described, message based connection cannot
    // proceed.
    //

    if (MessageCount == 0) {
        Status = STATUS_INVALID_PARAMETER;
        goto IopConnectMessageBasedInterruptExit;
    }

    //
    // The results of the connection request will be returned to the caller in
    // the form of an interrupt message table, with one entry per message.
    // Allocate this table.
    //

    Size = FIELD_OFFSET(IO_INTERRUPT_MESSAGE_INFO, MessageInfo[MessageCount]);
    OutputTable = ExAllocatePoolWithTag(NonPagedPoolNx, Size, 'nioI');
    if (OutputTable == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto IopConnectMessageBasedInterruptExit;
    }

    RtlZeroMemory(OutputTable, Size);
    OutputTable->UnifiedIrql = UnifiedIrql;

    //
    // The share vector parameter to IopConnectInterrupt indicates whether it
    // is allowable for multiple interrupt objects to be connected to this
    // interrupt on a single processor.  Sharing allows a single device stack to
    // connect multiple service routines to its single interrupt if desired.
    // As a result, all message based interrupts will be connected with
    // vector sharing enabled regardless of the share disposition in the device
    // interrupt resources.
    //

    ShareVector = TRUE;

    //
    // Connect the interrupt one message at a time.
    //

    for (Index = 0; Index < ConnectionData->Count; Index++) {
        MessageData = &ConnectionData->Vectors[Index];
        if ((MessageData->Type != InterruptTypeXapicMessage) &&
            (MessageData->Type != InterruptTypeHypertransport) &&
            (MessageData->Type != InterruptTypeMessageRequest)) {

            continue;
        }

        //
        // Determine the synchronization IRQL for this message.
        //

        if (PassiveInterrupt == FALSE) {

            //
            // If there is no unified IRQL, the synchronization IRQL is simply the
            // IRQL associated with the message's vector.
            //

            if (UnifiedIrql != 0) {
                SynchronizeIrql = UnifiedIrql;

            } else {
                SynchronizeIrql = MessageData->Irql;
            }

        } else {

            //
            // Passive level interrupts must set SynchronizeIrql to 0.
            //

            SynchronizeIrql = 0;
        }

        //
        // Construct a connection data structure representing only this
        // message, since IopConnectInterrupt can connect only a single vector.
        //

        MessageConnectionData.Count = 1;
        RtlCopyMemory(&MessageConnectionData.Vectors[0],
                      MessageData,
                      sizeof(INTERRUPT_VECTOR_DATA));

        Status = IopConnectInterrupt(&InterruptStructure,
                                     NULL,
                                     ServiceRoutine,
                                     ServiceContext,
                                     OutputTable->MessageCount,
                                     SpinLock,
                                     SynchronizeIrql,
                                     ShareVector,
                                     FloatingSave,
                                     &MessageConnectionData);

        if (!NT_SUCCESS(Status)) {
            goto IopConnectMessageBasedInterruptExit;
        }

        //
        // On success fill in the output table entry associated with this
        // message.
        //

        OutputEntry = &OutputTable->MessageInfo[OutputTable->MessageCount];
        if (MessageData->Type == InterruptTypeMessageRequest) {
            Request.Type = InterruptTargetTypeApic;
            Request.Apic.TargetProcessors = MessageData->TargetProcessors;
            Request.Apic.Vector = MessageData->Vector;
            Request.Apic.IntRemapInfo = MessageData->IntRemapInfo;
            Request.Apic.DestinationMode =
                MessageData->MessageRequest.DestinationMode;

            Status = HalGetMessageRoutingInfo(&Request, &MessageConnectionData);
            if (!NT_SUCCESS(Status)) {
                goto IopConnectMessageBasedInterruptExit;
            }

            MessageData = &MessageConnectionData.Vectors[0];
        }

        OutputEntry->MessageAddress.QuadPart =
            MessageData->GenericMessage.Address.QuadPart;

        OutputEntry->MessageData = MessageData->GenericMessage.DataPayload;
        OutputEntry->TargetProcessorSet = MessageData->TargetProcessors.Mask;
        OutputEntry->InterruptObject = &InterruptStructure->UserInterruptObject;
        OutputEntry->Vector = MessageData->Vector;
        OutputEntry->Irql = MessageData->Irql;
        OutputEntry->Mode = MessageData->Mode;
        OutputEntry->Polarity = MessageData->Polarity;

        //
        // Increment the number of messages processed.
        //

        OutputTable->MessageCount++;
    }

    NT_ASSERT(OutputTable->MessageCount == MessageCount);

    *MessageTableOut = OutputTable;
    Status = STATUS_SUCCESS;

IopConnectMessageBasedInterruptExit:

    if (ConnectionData != NULL) {
        ExFreePool(ConnectionData);
    }

    if (!NT_SUCCESS(Status)) {
        if (OutputTable != NULL) {

            PNP_ASSERT(OutputTable->MessageCount <= MessageCount);
            __analysis_assume(OutputTable->MessageCount <= MessageCount);

            for (Index = 0; Index < OutputTable->MessageCount; Index++) {
                OutputEntry = &OutputTable->MessageInfo[Index];
                IoDisconnectInterrupt(OutputEntry->InterruptObject);
            }

            ExFreePool(OutputTable);
        }
    }

    return Status;
}

NTSTATUS
IopConnectInterrupt (
    __out PIO_INTERRUPT_STRUCTURE *InterruptStructureOut,
    __in_opt PKSERVICE_ROUTINE ServiceRoutine,
    __in_opt PKMESSAGE_SERVICE_ROUTINE MessageServiceRoutine,
    __in_opt PVOID ServiceContext,
    __in ULONG MessageIndex,
    __in_opt PKSPIN_LOCK SpinLock,
    __in KIRQL SynchronizeIrql,
    __in BOOLEAN ShareVector,
    __in BOOLEAN FloatingSave,
    __in PINTERRUPT_CONNECTION_DATA ConnectionData
    )

/*++

Routine Description:

    This routine allocates, initializes, and connects interrupt objects for
    all of the processors specified in the processor enable mask.

Arguments:

    InterruptStructureOut - Address of a variable to receive a pointer to the
        interrupt structure allocated and initialized for this request.

    ServiceRoutine - Address of the interrupt service routine (ISR) that should
        be executed when the interrupt occurs.

    MessageServiceRoutine - Supplies the address of the interrupt message
        service routine that should be executed when the interrupt occurs.

    ServiceContext - Supplies a pointer to the context information required
        by the ISR.

    SpinLock - Supplies a pointer to a spin lock to be used when synchronizing
        with the ISR.

    Vector - Supplies the vector upon which the interrupt occurs.

    Irql - Supplies the IRQL upon which the interrupt occurs.

    SynchronizeIrql - The request priority that the interrupt should be
        synchronized with.

    InterruptMode - Specifies the interrupt mode of the device.

    ShareVector - Supplies a boolean value that specifies whether the
        vector can be shared with other interrupt objects or not.  If FALSE
        then the vector may not be shared, if TRUE it may be.
        Latched.

    FloatingSave - A BOOLEAN that specifies whether or not the machine's
        floating point state should be saved before invoking the ISR.

    ConnectionData - Supplies a pointer to the interrupt connection data.

Return Value:

    The function value is the final function status.  The three status values
    that this routine can itself return are:

        STATUS_SUCCESS - Everything worked successfully.
        STATUS_INVALID_PARAMETER - No processors were specified.
        STATUS_INSUFFICIENT_RESOURCES - There was not enough nonpaged pool.

--*/

{

    ACTIVE_CONNECT_BLOCK ConnectBlock;
    BOOLEAN EmulateActiveBothForPrimaryInterrupt;
    UCHAR GroupIndex;
    PIO_INTERRUPT_STRUCTURE InterruptStructure;
    PKINTERRUPT InterruptObject;
    KIRQL Irql;
    KINTERRUPT_MODE Mode;
    CCHAR ObjectCount;
    CCHAR ObjectNumber;
    PKSPIN_LOCK ObjectSpinLock;
    PKEVENT PassiveEvent;
    BOOLEAN PassiveInterrupt;
    PKPRCB Prcb;
    CCHAR ProcessorCount;
    ULONG ProcIndex;
    PROCESSOR_NUMBER ProcNumber;
    NTSTATUS Status;
    ULONG StructureSize;
    GROUP_AFFINITY TargetProcessors;
    ULONG Vector;

    PAGED_CODE();

    EmulateActiveBothForPrimaryInterrupt = FALSE;
    *InterruptStructureOut = NULL;
    Status = STATUS_SUCCESS;
    InterruptStructure = NULL;
    PassiveEvent = NULL;
    ProcessorCount = 0;

    //
    // Initialize an active connect/disconnect block.
    //

    IopInitializeActiveConnectBlock(ConnectionData, &ConnectBlock);

    //
    // The connection data describes the properties of the hardware connection
    // of this interrupt.  Pull the parameters needed for this routine out of
    // the structure.  Since this routine only connects a single interrupt
    // request to a single vector, this structure should only have a single
    // item in it.
    //

    PNP_ASSERT(ConnectionData->Count == 1);

    Vector = ConnectionData->Vectors[0].Vector;
    Mode = ConnectionData->Vectors[0].Mode;
    TargetProcessors = ConnectionData->Vectors[0].TargetProcessors;
    TargetProcessors.Mask &= KeActiveProcessors.Bitmap[TargetProcessors.Group];
    Irql = ConnectionData->Vectors[0].Irql;

    //
    // ActiveBoth primary interrupts are specified as edge-triggered in ACPI
    // firmware but are emulated using levels.
    //

    if ((ConnectionData->Vectors[0].Type == InterruptTypeControllerInput) &&
        (HalIsInterruptTypeSecondary(
            InterruptTypeControllerInput,
            ConnectionData->Vectors[0].ControllerInput.Gsiv) == FALSE)) {

        if ((ConnectionData->Vectors[0].Polarity ==
             InterruptActiveBothTriggerLow) ||
            (ConnectionData->Vectors[0].Polarity ==
             InterruptActiveBothTriggerHigh)) {

            EmulateActiveBothForPrimaryInterrupt = TRUE;

            NT_ASSERT(Mode == Latched);

            Mode = LevelSensitive;
        }
    }

    //
    // Determine whether this is a passive-level interrupt or not.
    //

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(SynchronizeIrql != PASSIVE_LEVEL);

    PassiveInterrupt = FALSE;

#else

    if (SynchronizeIrql == PASSIVE_LEVEL) {
        PassiveInterrupt = TRUE;

    } else {
        PassiveInterrupt = FALSE;
    }

#endif

    if (PassiveInterrupt == FALSE) {
        PNP_ASSERT(Irql <= SynchronizeIrql);
    }

    //
    // Validate the specified target processors this interrupt is connected to.
    //

    PNP_ASSERT(KeVerifyGroupAffinity(&TargetProcessors, FALSE) != FALSE);
    if (KeVerifyGroupAffinity(&TargetProcessors, FALSE) == FALSE) {
        Status = STATUS_INVALID_PARAMETER;
        goto IopConnectInterruptExit;
    }

    ProcessorCount = (CCHAR)RtlNumberOfSetBitsUlongPtr(TargetProcessors.Mask);

    PNP_ASSERT(ProcessorCount != 0);

    if (ProcessorCount == 0) {
        Status = STATUS_INVALID_PARAMETER;
        goto IopConnectInterruptExit;
    }

    //
    // Allocate a structure to hold information about this connection request.
    // This includes one interrupt object per processor and the stored
    // connection information.
    //

    StructureSize = sizeof(IO_INTERRUPT_STRUCTURE) +
                    ((ProcessorCount - 1) * sizeof(PKINTERRUPT));

    InterruptStructure = ExAllocatePoolWithTag(NonPagedPoolNx,
                                               StructureSize,
                                               'nioI');

    if (InterruptStructure == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto IopConnectInterruptExit;
    }

    RtlZeroMemory(InterruptStructure, StructureSize);
    KeInitializeDpc(&InterruptStructure->Dpc, NULL, NULL);

    //
    // Copy the connection information provided by the caller into this
    // interrupt structure:
    //     1. Update the mode in case ActiveBoth is being emulated using
    //        levels.
    //     2. Update the target processors in case the caller provided a bogus
    //        target processor set.
    //

    RtlCopyMemory(&InterruptStructure->ConnectionData,
                  ConnectionData,
                  sizeof(INTERRUPT_CONNECTION_DATA));

    InterruptStructure->ConnectionData.Vectors[0].Mode = Mode;
    InterruptStructure->ConnectionData.Vectors[0].TargetProcessors =
        TargetProcessors;

    //
    // If the caller specified a spin lock to be used for the interrupt object,
    // use it.  Otherwise, provide one by using the one in the structure that
    // was just allocated.
    //

    if (ARGUMENT_PRESENT(SpinLock)) {
        ObjectSpinLock = SpinLock;

    } else {
        ObjectSpinLock = &InterruptStructure->SpinLock;
    }

    //
    // For interrupts to be serviced at passive-level, use the event object
    // provided by the interrupt structure just allocated.
    //

    InterruptStructure->PassiveInterrupt = PassiveInterrupt;

#if !defined(XBOX_SYSTEMOS)

    if (PassiveInterrupt != FALSE) {
        PassiveEvent = &InterruptStructure->PassiveEvent;
    }

#endif

    //
    // Initialize and connect the interrupt objects to the appropriate
    // processors.
    //

    InterruptStructure->TargetProcessors = TargetProcessors.Mask;

    //
    // For each entry in the processor enable mask that is set, allocate and
    // initialize an interrupt object.
    //

    ProcNumber.Group = TargetProcessors.Group;
    ProcNumber.Reserved = 0;
    for (ObjectCount = 0, GroupIndex = 0;
         ObjectCount < ProcessorCount;
         GroupIndex += 1) {

        if (TargetProcessors.Mask & AFFINITY_MASK(GroupIndex)) {

            //
            // Allocate an interrupt object for this processor, ensuring that
            // the current thread is running on the target processor when the
            // allocation occurs.  This may allow the memory manager to perform
            // the allocation from processor-local memory on a NUMA machine.
            // Since the interrupt object contains the interrupt dispatch code
            // that is run when the interrupt occurs, this may result in a
            // performance benefit.
            //

            ProcNumber.Number = GroupIndex;
            ProcIndex = KeGetProcessorIndexFromNumber(&ProcNumber);
            Prcb = KiProcessorBlock[ProcIndex];
            InterruptObject = KeAllocateInterrupt(Prcb);
            if (InterruptObject == NULL) {
                Status = STATUS_INSUFFICIENT_RESOURCES;
                goto IopConnectInterruptExit;
            }

            RtlZeroMemory(InterruptObject, sizeof(KINTERRUPT));
            KeInitializeInterruptEx(InterruptObject,
                                    ServiceRoutine,
                                    MessageServiceRoutine,
                                    ServiceContext,
                                    MessageIndex,
                                    ObjectSpinLock,
                                    PassiveEvent,
                                    Vector,
                                    Irql,
                                    SynchronizeIrql,
                                    Mode,
                                    ShareVector,
                                    ProcIndex,
                                    FloatingSave,
                                    EmulateActiveBothForPrimaryInterrupt,
                                    &InterruptStructure->ConnectionData);

            InterruptStructure->InterruptObject[ObjectCount] = InterruptObject;
            ObjectCount++;
        }
    }

    //
    // Allocate a passive interrupt block for servicing the interrupt.
    //
    // N.B. This object must be created before the interrupt is enabled at the
    //      controller to guarantee that it is available when the interrupt
    //      fires. In cases where the passive interrupt block is already
    //      created for the supplied interrupt, a new one will not be created
    //      again.
    //

#if !defined(XBOX_SYSTEMOS)

    if (PassiveInterrupt != FALSE) {
        Status = IopAllocatePassiveInterruptBlock(
                    &InterruptStructure->ConnectionData,
                    &TargetProcessors);

        if (!NT_SUCCESS(Status)) {
            goto IopConnectInterruptExit;
        }
    }

#endif

    //
    // Acquire the connect lock to serialize connect/disconnect operations (on
    // a given GSIV). Note acquiring the connect lock will cause the critical
    // region to be entered.
    //

    IopAcquireActiveConnectLock(&ConnectBlock);

    //
    // Connect the array of interrupt objects to the specified vector.  This
    // also unmasks the interrupt in the platform.
    //

    Status = KeConnectInterrupt(InterruptStructure->InterruptObject,
                                ProcessorCount,
                                &InterruptStructure->ConnectionData);

    //
    // Release the connect lock. This will exit the critical region.
    //

    IopReleaseActiveConnectLock(&ConnectBlock);

    if (!NT_SUCCESS(Status)) {

#if !defined(XBOX_SYSTEMOS)

        if (PassiveInterrupt != FALSE) {
            IopDestroyPassiveInterruptBlock(Vector);
        }

#endif

        goto IopConnectInterruptExit;
    }

    //
    // Copy the data from the first per-processor interrupt object into the
    // user interrupt object.  The user interrupt object is the only part of
    // the interrupt structure that will be passed back to the caller.  The
    // user object is opaque to the caller, but copying this data provides
    // consistency with the implementation on previous versions of the system,
    // in case a caller has reverse engineered the fields in the object.
    //

    RtlCopyMemory(&InterruptStructure->UserInterruptObject,
                  InterruptStructure->InterruptObject[0],
                  sizeof(KINTERRUPT));

    *InterruptStructureOut = InterruptStructure;
    Status = STATUS_SUCCESS;

IopConnectInterruptExit:
    if (!NT_SUCCESS(Status)) {
        if (InterruptStructure != NULL) {
            for (ObjectNumber = 0;
                 ObjectNumber < ProcessorCount;
                 ObjectNumber += 1) {

                InterruptObject = InterruptStructure->InterruptObject[ObjectNumber];
                if (InterruptObject != NULL) {
                    KeFreeInterrupt(InterruptObject);
                }
            }

            ExFreePoolWithTag(InterruptStructure, 'nioI');
        }
    }

    IopDestroyActiveConnectBlock(&ConnectBlock);
    return Status;
}

VOID
IoDisconnectInterruptEx (
    __in PIO_DISCONNECT_INTERRUPT_PARAMETERS Parameters
    )

/*++

Routine Description:

    This function frees the interrupt objects and disconnects the interrupt
    service routine associated with an interrupt or set of interrupts that
    were connected through an earlier call to IoConnectInterruptEx.  This
    routine supersedes IoDisconnectInterrupt.

Arguments:

    Parameters - Supplies the parameters associated with this interrupt
        disconnection attempt.  See the definition of this structure for
        details on the meaning of the enclosed fields.

Return Value:

    None.

Environment:

    PASSIVE_LEVEL.

--*/

{

    PIO_INTERRUPT_MESSAGE_INFO_ENTRY Entry;
    ULONG Index;
    PIO_INTERRUPT_LINE_INFO InterruptTable;
    PIO_INTERRUPT_MESSAGE_INFO MessageTable;

    PAGED_CODE();

    PNP_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    switch (Parameters->Version) {

        //
        // Fully specified or line based interrupt disconnection case.
        // Disconnect the interrupt by passing a pointer to the associated
        // interrupt object to IoDisconnectInterrupt.
        //

    case CONNECT_FULLY_SPECIFIED:
    case CONNECT_FULLY_SPECIFIED_GROUP:
        IoDisconnectInterrupt(Parameters->ConnectionContext.InterruptObject);
        break;

        //
        // Line based case. There may be multiple interrupt objects associated
        // with this request all stored in an interrupt table. Disconnect each
        // interrupt object and free the interrupt table.
        //

    case CONNECT_LINE_BASED:
        InterruptTable = CONTAINING_RECORD(
                                Parameters->ConnectionContext.InterruptObject,
                                IO_INTERRUPT_LINE_INFO,
                                UserInterruptObject);

        for (Index = 0; Index < InterruptTable->InterruptCount; Index +=1) {
                IoDisconnectInterrupt(
                  &(InterruptTable->InterruptInfo[Index]->UserInterruptObject));
        }

        ExFreePool(InterruptTable);
        break;

        //
        // Message based case.  There may be multiple interrupt objects
        // associated with this request, all stored in a message table.
        // Disconnect each interrupt object and free the message table.
        //

    case CONNECT_MESSAGE_BASED:
    case CONNECT_MESSAGE_BASED_PASSIVE:
        MessageTable = Parameters->ConnectionContext.InterruptMessageTable;
        for (Index = 0; Index < MessageTable->MessageCount; Index++) {
            Entry = &MessageTable->MessageInfo[Index];
            IoDisconnectInterrupt(Entry->InterruptObject);
        }

        ExFreePool(MessageTable);
        break;

        //
        // Unrecognized interrupt disconnection case.  IoConnectInterruptEx
        // will never successfully carry out a connection attempt of this type.
        // Furthermore, the given connection type is required to match the one
        // returned by a corresponding successful interrupt connection attempt.
        // As a result, we'll only reach this case in the presence of driver
        // bugs.
        //

    default:
        KeBugCheckEx(PNP_DETECTED_FATAL_ERROR,
                     PNP_ERR_BAD_INTERRUPT_DISCONNECT_ATTEMPT,
                     (ULONG_PTR)Parameters->Version,
                     0,
                     0);

        break;
    }

    return;
}

__drv_maxIRQL(PASSIVE_LEVEL)
VOID
IoDisconnectInterrupt (
    __in PKINTERRUPT InterruptObject
    )

/*++

Routine Description:

    This routine disconnects an interrupt object initialized and connected by
    a call to IopConnectInterrupt.  The interrupt object passed to this routine
    is an opaque structure that is part of an internal interrupt structure
    containing the state of the call to IopConnectInterrupt.  This routine
    disconnects each of the per-processor interrupts and frees all the memory
    associated with the interrupt structure.  This routine does not simply
    disconnect the single processor vector described by the single interrupt
    object passed to this routine.  Note that no interrupt objects directly
    connected using the kernel services may be input to this routine.

Arguments:

    InterruptObject - Supplies a pointer to an opaque structure that is part
        of an internal interrupt structure that tracks the interrupt connection.
        Although this has the same type as the per-processor interrupt objects
        that are allocated as part of interrupt connection, it is not one of
        these objects.  It serves only to fit the calling convention of
        IoConnectInterrupt and IoDisconnectInterrupt, and to allow this routine
        to determine the address of the interrupt structure.

Return Value:

    None.

--*/

{

    ACTIVE_CONNECT_BLOCK ConnectBlock;

#if !defined(XBOX_SYSTEMOS)

    PKINTERRUPT Interrupt;

#endif

    PIO_INTERRUPT_STRUCTURE InterruptStructure;

#if !defined(XBOX_SYSTEMOS)

    BOOLEAN LockOwner;

#endif

    CCHAR ObjectCount;
    CCHAR ProcessorCount;

#if !defined(XBOX_SYSTEMOS)

    PKTHREAD ServiceThread;
    NTSTATUS Status;
    ULONG Vector;

#endif

    PAGED_CODE();

    PNP_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    //
    // Obtain a pointer to the builtin interrupt object in the I/O interrupt
    // structure.
    //

    InterruptStructure = CONTAINING_RECORD(InterruptObject,
                                           IO_INTERRUPT_STRUCTURE,
                                           UserInterruptObject);

    //
    // Remove any queued work items.
    //

    KeRemoveQueueDpc(&InterruptStructure->Dpc);

    //
    // Initialize an active connect/disconnect block.
    //

    IopInitializeActiveConnectBlock(&InterruptStructure->ConnectionData,
                                    &ConnectBlock);

    //
    // Call Ke to disconnect the array of per-processor interrupt objects.
    // There is one object for each set bit in the target processor mask.
    // This call also disconnects the interrupt from any platform interrupt
    // controllers, if appropriate.
    //

    ProcessorCount = (CCHAR)RtlNumberOfSetBitsUlongPtr(InterruptStructure->TargetProcessors);
    PNP_ASSERT(ProcessorCount != 0);

    //
    // Acquire the connect lock to serialize connect/disconnect operations (on
    // a given GSIV). Note acquiring the connect lock will cause the critical
    // region to be entered.
    //

    IopAcquireActiveConnectLock(&ConnectBlock);

    KeDisconnectInterrupt(&InterruptStructure->InterruptObject[0],
                          ProcessorCount,
                          &InterruptStructure->ConnectionData);

    //
    // Release the connect lock. This will exit the critical region.
    //

    IopReleaseActiveConnectLock(&ConnectBlock);

    //
    // Release the dispatcher worker lock and delete the passive interrupt block
    // if one was allocated. Note if the interrupt is still connected and there
    // are passive-level interrupts still on the chain, the passive interrupt
    // block will not get deleted. Only the current reference will be dropped
    // in such case.
    //

#if defined(XBOX_SYSTEMOS)

    NT_ASSERT(InterruptStructure->PassiveInterrupt == FALSE);

#else

    if (InterruptStructure->PassiveInterrupt != FALSE) {

        PNP_ASSERT(InterruptStructure->ConnectionData.Count >= 1);

        Vector = InterruptStructure->ConnectionData.Vectors[0].Vector;
        IopDestroyPassiveInterruptBlock(Vector);
    }

#endif

    //
    // Protect this thread from being suspended while passive event
    // lock is held.
    //

    KeEnterCriticalRegion();

    //
    // For interrupts connected at passive-level, additionally wait for the
    // passive-level to become available. This is done to ensure any
    // outstanding interrupt service routine callbacks finish before interrupt
    // is disconnected.
    //
    // N.B. In some scenarios (e.g. ACPI Wait/Wake IRP handling) the interrupt
    //      disconnect may originate in the context of the ISR callback. The
    //      lock should not be acquired in that case as it is already acquired
    //      by the current thread.
    //

#if !defined(XBOX_SYSTEMOS)

    if (InterruptStructure->PassiveInterrupt != FALSE) {
        LockOwner = FALSE;
        ServiceThread = KeGetCurrentThread();
        for (ObjectCount = 0; ObjectCount < ProcessorCount; ObjectCount += 1) {
            Interrupt = InterruptStructure->InterruptObject[ObjectCount];
            if (Interrupt->ServiceThread == ServiceThread) {
                LockOwner = TRUE;
            }
        }

        if (LockOwner == FALSE) {
            Status = KeWaitForSingleObject(&InterruptStructure->PassiveEvent,
                                           Executive,
                                           KernelMode,
                                           FALSE,
                                           NULL);

            NT_ASSERT(NT_SUCCESS(Status) != FALSE);
        }
    }

#endif

    //
    // Exit the critical region.
    //

    KeLeaveCriticalRegion();

    //
    // De-allocate the memory associated with the interrupt structure.
    //

    for (ObjectCount = 0; ObjectCount < ProcessorCount; ObjectCount += 1) {
        KeFreeInterrupt(InterruptStructure->InterruptObject[ObjectCount]);
    }

    IopDestroyActiveConnectBlock(&ConnectBlock);
    ExFreePool(InterruptStructure);
    return;
}

VOID
IopMaskInterrupt (
    __in PKINTERRUPT InterruptObject
    )

/*++

Routine Description:

    This routine masks an interrupt object initialized and connected by
    a call to IopConnectInterrupt.  The interrupt object passed to this routine
    is an opaque structure that is part of an internal interrupt structure
    containing the state of the call to IopConnectInterrupt. Note that no
    interrupt objects directly connected using the kernel services may be input
    to this routine.

Arguments:

    InterruptObject - Supplies a pointer to an opaque structure that is part
        of an internal interrupt structure that tracks the interrupt connection.

Return Value:

    None.

--*/

{

    PIO_INTERRUPT_STRUCTURE InterruptStructure;
    CCHAR ProcessorCount;
    NTSTATUS Status;

    PNP_ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Obtain a pointer to the builtin interrupt object in the I/O interrupt
    // structure.
    //

    InterruptStructure = CONTAINING_RECORD(InterruptObject,
                                           IO_INTERRUPT_STRUCTURE,
                                           UserInterruptObject);

    //
    // Call Ke to mask the array of per-processor interrupt objects.
    // There is one object for each set bit in the target processor mask.
    // This call also mask the interrupt from any platform interrupt
    // controllers, if appropriate.
    //

    ProcessorCount = (CCHAR)RtlNumberOfSetBitsUlongPtr(
                                InterruptStructure->TargetProcessors);

    PNP_ASSERT(ProcessorCount != 0);

    Status = KeMaskInterrupt(InterruptStructure->InterruptObject,
                             ProcessorCount,
                             &InterruptStructure->ConnectionData,
                             &InterruptStructure->Dpc);

    PNP_ASSERT(Status == STATUS_SUCCESS ||
               Status == STATUS_ALREADY_DISCONNECTED);

    return;
}

VOID
IopUnmaskInterrupt (
    __in PKINTERRUPT InterruptObject
    )

/*++

Routine Description:

    This routine unmasks an interrupt object initialized and connected by
    a call to IopConnectInterrupt and then subsequently masked by
    IopMaskInterrupt.  The interrupt object passed to this routine
    is an opaque structure that is part of an internal interrupt structure
    containing the state of the call to IopConnectInterrupt. Note that no
    interrupt objects directly connected using the kernel services may be input
    to this routine.

Arguments:

    InterruptObject - Supplies a pointer to an opaque structure that is part
        of an internal interrupt structure that tracks the interrupt connection.

Return Value:

    None.

--*/

{

    PIO_INTERRUPT_STRUCTURE InterruptStructure;
    CCHAR ProcessorCount;
    NTSTATUS Status;

    PNP_ASSERT(KeGetCurrentIrql() <= DISPATCH_LEVEL);

    //
    // Obtain a pointer to the builtin interrupt object in the I/O interrupt
    // structure.
    //

    InterruptStructure = CONTAINING_RECORD(InterruptObject,
                                           IO_INTERRUPT_STRUCTURE,
                                           UserInterruptObject);

    //
    // Call Ke to unmask the array of per-processor interrupt objects.
    // There is one object for each set bit in the target processor mask.
    // This call also unmasks the interrupt from any platform interrupt
    // controllers, if appropriate.
    //

    ProcessorCount = (CCHAR)RtlNumberOfSetBitsUlongPtr(
                                InterruptStructure->TargetProcessors);

    PNP_ASSERT(ProcessorCount != 0);

    Status = KeUnmaskInterrupt(InterruptStructure->InterruptObject,
                               ProcessorCount,
                               &InterruptStructure->ConnectionData);

    PNP_ASSERT(Status == STATUS_SUCCESS ||
               Status == STATUS_INTERRUPT_VECTOR_ALREADY_CONNECTED);

    return;
}

VOID
IoReportInterruptActive (
    _In_ PIO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS Parameters
    )

/*++

Routine Description:

    This function informs the system that the interrupt service routine
    associated with an interrupt or set of interrupts that were connected
    through an earlier call to IoConnectInterruptEx is active and expecting
    interrupt requests on the associated lines. This is typically called after
    having reported the lines as inactive via IoReportInterruptInactive.

Arguments:

    Parameters - Supplies the parameters associated with this interrupt
        reporting attempt.  See the definition of this structure for
        details on the meaning of the enclosed fields.

Return Value:

    None.

Environment:

    Kenrel mode.

--*/

{

    PIO_INTERRUPT_MESSAGE_INFO_ENTRY Entry;
    ULONG Index;
    PIO_INTERRUPT_LINE_INFO InterruptTable;
    PIO_INTERRUPT_MESSAGE_INFO MessageTable;

    switch (Parameters->Version) {

        //
        // Fully specified or line based interrupt case. Unmask the interrupt
        // by passing a pointer to the associated interrupt object to
        // IopUnmaskInterrupt.
        //

    case CONNECT_FULLY_SPECIFIED:
    case CONNECT_FULLY_SPECIFIED_GROUP:
        IopUnmaskInterrupt(Parameters->ConnectionContext.InterruptObject);
        break;

        //
        // Line based case. There may be multiple interrupt objects associated
        // with this request all stored in an interrupt table. Unmask each
        // interrupt object.
        //

    case CONNECT_LINE_BASED:
        InterruptTable = CONTAINING_RECORD(
                                Parameters->ConnectionContext.InterruptObject,
                                IO_INTERRUPT_LINE_INFO,
                                UserInterruptObject);

        for (Index = 0; Index < InterruptTable->InterruptCount; Index +=1) {
            IopUnmaskInterrupt(
                &(InterruptTable->InterruptInfo[Index]->UserInterruptObject));
        }

        break;

        //
        // Message based case.  There may be multiple interrupt objects
        // associated with this request, all stored in a message table.
        // Unmask each interrupt object.
        //

    case CONNECT_MESSAGE_BASED:
        MessageTable = Parameters->ConnectionContext.InterruptMessageTable;
        for (Index = 0; Index < MessageTable->MessageCount; Index++) {
            Entry = &MessageTable->MessageInfo[Index];
            IopUnmaskInterrupt(Entry->InterruptObject);
        }

        break;

        //
        // Unrecognized interrupt case.  IoConnectInterruptEx will never
        // successfully carry out a connection attempt of this type.
        // Furthermore, the given connection type is required to match the one
        // returned by a corresponding successful interrupt connection attempt.
        // As a result, we'll only reach this case in the presence of driver
        // bugs.
        //

    default:
        KeBugCheckEx(PNP_DETECTED_FATAL_ERROR,
                     PNP_ERR_BAD_INTERRUPT_REPORTING_ATTEMPT,
                     (ULONG_PTR)Parameters->Version,
                     0,
                     0);

        break;
    }

    return;
}

VOID
IoReportInterruptInactive (
    _In_ PIO_REPORT_INTERRUPT_ACTIVE_STATE_PARAMETERS Parameters
    )

/*++

Routine Description:

    This function informs the system that the the interrupt service routine
    associated with an interrupt or set of interrupts that were connected
    through an earlier call to IoConnectInterruptEx is no longer active and
    is not expecting interrupt requests on the associated lines. From the
    inactive state the association can be re-activated or disonnected.

Arguments:

    Parameters - Supplies the parameters associated with this interrupt
        reporting attempt.  See the definition of this structure for
        details on the meaning of the enclosed fields.

Return Value:

    None.

Environment:

    Kenrel mode.

--*/

{

    PIO_INTERRUPT_MESSAGE_INFO_ENTRY Entry;
    ULONG Index;
    PIO_INTERRUPT_LINE_INFO InterruptTable;
    PIO_INTERRUPT_MESSAGE_INFO MessageTable;

    switch (Parameters->Version) {

        //
        // Fully specified or line based interrupt case. Mask the interrupt by
        // passing a pointer to the associated interrupt object to
        // IopMaskInterrupt.
        //

    case CONNECT_FULLY_SPECIFIED:
    case CONNECT_FULLY_SPECIFIED_GROUP:
        IopMaskInterrupt(Parameters->ConnectionContext.InterruptObject);
        break;

        //
        // Line based case. There may be multiple interrupt objects associated
        // with this request all stored in an interrupt table. Mask each
        // interrupt object.
        //

    case CONNECT_LINE_BASED:
        InterruptTable = CONTAINING_RECORD(
                                Parameters->ConnectionContext.InterruptObject,
                                IO_INTERRUPT_LINE_INFO,
                                UserInterruptObject);

        for (Index = 0; Index < InterruptTable->InterruptCount; Index +=1) {
            IopMaskInterrupt(
                &InterruptTable->InterruptInfo[Index]->UserInterruptObject);
        }

        break;

        //
        // Message based case.  There may be multiple interrupt objects
        // associated with this request, all stored in a message table.
        // Mask each interrupt object.
        //

    case CONNECT_MESSAGE_BASED:
        MessageTable = Parameters->ConnectionContext.InterruptMessageTable;
        for (Index = 0; Index < MessageTable->MessageCount; Index++) {
            Entry = &MessageTable->MessageInfo[Index];
            IopMaskInterrupt(Entry->InterruptObject);
        }

        break;

        //
        // Unrecognized interrupt case.  IoConnectInterruptEx will never
        // successfully carry out a connection attempt of this type.
        // Furthermore, the given connection type is required to match the one
        // returned by a corresponding successful interrupt connection attempt.
        // As a result, we'll only reach this case in the presence of driver
        // bugs.
        //

    default:
        KeBugCheckEx(PNP_DETECTED_FATAL_ERROR,
                     PNP_ERR_BAD_INTERRUPT_REPORTING_ATTEMPT,
                     (ULONG_PTR)Parameters->Version,
                     0,
                     0);

        break;
    }

    return;
}

NTSTATUS
IoGetAffinityInterrupt (
    __in PKINTERRUPT InterruptObject,
    __out PGROUP_AFFINITY GroupAffinity
    )

/*++

Routine Description:

    This routine retrieves group affinity of an opaque interrupt object.

Arguments:

    InterruptObject - Supplies a pointer to an opaque internal interrupt
        structure.

    GroupAffinity - Supplies a pointer to a structure that receives the
        group affinity of the given interrupt object.

Return Value:

    STATUS_SUCCESS - Everything worked successfully.
    STATUS_INVALID_PARAMETER - Invalid interrupt object is specified.

--*/

{

    PIO_INTERRUPT_STRUCTURE InterruptStructure;
    PROCESSOR_NUMBER ProcNum;
    NTSTATUS Status;

    //
    // Obtain a pointer to the builtin interrupt object in the I/O interrupt
    // structure.
    //

    InterruptStructure = CONTAINING_RECORD(InterruptObject,
                                           IO_INTERRUPT_STRUCTURE,
                                           UserInterruptObject);

    Status = KeGetProcessorNumberFromIndex(InterruptObject->Number, &ProcNum);
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    RtlZeroMemory(GroupAffinity, sizeof(*GroupAffinity));
    GroupAffinity->Group = ProcNum.Group;
    GroupAffinity->Mask = InterruptStructure->TargetProcessors;

    return STATUS_SUCCESS;
}

NTSTATUS
IopGetInterruptConnectionData (
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __deref_out PINTERRUPT_CONNECTION_DATA *ConnectionDataOut
    )

/*++

Routine Description:

    This routine retrieves the extended interrupt connection data associated
    with the given device.  This information is stored as a device property.

Arguments:

    PhysicalDeviceObject - Supplies the device object for which the connection
        data should be retrieved.

    ConnectionDataOut - Supplies a pointer to a variable that receives a pointer
        to the interrupt connection data for the device.

Return Value:

    STATUS_SUCCESS if the operation succeeds.
    STATUS_DATA_ERROR if the connection data could be retrieved, but is
        internally inconsistent.
    Another error status if returned from the property retrieval routine.

--*/

{

    PINTERRUPT_CONNECTION_DATA ConnectionData;
    ULONG ExpectedSize;
    ULONG RequiredSize;
    NTSTATUS Status;
    ULONG Type;

    PAGED_CODE();

    *ConnectionDataOut = NULL;
    ConnectionData = NULL;

    //
    // The extended interrupt connection data for a device is stored as a
    // device property.  This is an arbitrarily sized structure, so first
    // retrieve the necessary size of the data buffer.
    //

    Status = IoGetDevicePropertyData(PhysicalDeviceObject,
                                     &INTERRUPT_CONNECTION_DATA_PKEY,
                                     0, // Language ID
                                     0, // Flags
                                     0, // Size
                                     NULL, // Data
                                     &RequiredSize,
                                     &Type);

    if ((Status != STATUS_BUFFER_TOO_SMALL) ||
        (RequiredSize < sizeof(INTERRUPT_CONNECTION_DATA))) {
        return Status;
    }

    ConnectionData = ExAllocatePool(NonPagedPoolNx, RequiredSize);
    if (ConnectionData == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Call the property routine a second time to retrieve the data.
    //

    Status = IoGetDevicePropertyData(PhysicalDeviceObject,
                                     &INTERRUPT_CONNECTION_DATA_PKEY,
                                     0, // Language ID
                                     0, // Flags
                                     RequiredSize, // Size
                                     ConnectionData, // Data
                                     &RequiredSize,
                                     &Type);

    if (!NT_SUCCESS(Status)) {
        goto IopGetInterruptConnectionDataExit;
    }

    //
    // Validate the data.  The structure should have one vector description
    // for each message in the structure count.  If the buffer is smaller than
    // this, fail so that callers do not run off the end of the buffer.
    //

    ExpectedSize = sizeof(INTERRUPT_CONNECTION_DATA) +
                   ((ConnectionData->Count - 1) * sizeof(INTERRUPT_VECTOR_DATA));

    if (RequiredSize < ExpectedSize) {
        Status = STATUS_DATA_ERROR;
        goto IopGetInterruptConnectionDataExit;
    }

    *ConnectionDataOut = ConnectionData;

IopGetInterruptConnectionDataExit:

    if (!NT_SUCCESS(Status)) {
        if (ConnectionData != NULL) {
            ExFreePool(ConnectionData);
        }
    }

    return Status;
}

__drv_maxIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
IoSetDevicePropertyData (
    __in PDEVICE_OBJECT     Pdo,
    __in CONST DEVPROPKEY   *PropertyKey,
    __in LCID               Lcid,
    __in ULONG              Flags,
    __in DEVPROPTYPE        Type,
    __in ULONG              Size,
    __in_bcount_opt(Size)
         PVOID              Data
    )

/*++

Description

    This routine sets a specified custom device property on the device.

Parameters

    Pdo - Pointer to the physical device object.

    PropertyKey - Pointer to the property key identifying the property.

    Lcid - Optional language identifier. If not specified, property is set for the default
        language.

    Flags -
        PLUGPLAY_PROPERTY_PERSISTENT - Property should be persisted across boots.

    Type - Custom property type.

    Size - Custom property data size.

    Data - Pointer to the custom property data. If NULL, the specified property is
        deleted.

Return Value

    NTSTATUS.

Headers

    Declared in wdm.h and ntddk.h. Include wdm.h or ntddk.h.

Comments

    Callers of IoSetDevicePropertyData must be running at IRQL = PASSIVE_LEVEL.

See Also

    IoGetDevicePropertyData

--*/

{
    PAGED_CODE();

    ASSERT_PDO(Pdo);

    if (!ARGUMENT_PRESENT(Data)) {
        Type = DEVPROP_TYPE_EMPTY;
        Size = 0;
    }

    return PnpSetDevicePropertyData(Pdo,
                                    PropertyKey,
                                    Lcid,
                                    Flags,
                                    Type,
                                    Size,
                                    Data);
}

__drv_maxIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
IoGetDevicePropertyData (
    __in PDEVICE_OBJECT     Pdo,
    __in CONST DEVPROPKEY   *PropertyKey,
    __in LCID               Lcid,
    __reserved ULONG        Flags,
    __in ULONG              Size,
    __out_bcount_part(Size, *RequiredSize)
          PVOID             Data,
    __out PULONG            RequiredSize,
    __out PDEVPROPTYPE      Type
    )

/*++

Description

    This routine gets the specified custom device property previously set on the device.

Parameters

    Pdo - Pointer to the physical device object.

    PropertyKey - Pointer to the property key identifying the property.

    Lcid - Optional language identifier. If not specified, data for the default language
        is returned.

    Flags - Reserved.

    Size - Size of the Data in bytes.

    Data - Pointer to recieve the custom property data.

    RequiredSize - Pointer that recieves the required size for getting the data.

    Type - Pointer that recieves the custom property data type.

Return Value

    NTSTATUS.

Headers

    Declared in wdm.h and ntddk.h. Include wdm.h or ntddk.h.

Comments

    Callers of IoGetDevicePropertyData must be running at IRQL = PASSIVE_LEVEL.

See Also

    IoSetDevicePropertyData

--*/

{

    PAGED_CODE();

    ASSERT_PDO(Pdo);

    return PnpGetDevicePropertyData(Pdo,
                                    PropertyKey,
                                    Lcid,
                                    Flags,
                                    Size,
                                    Data,
                                    RequiredSize,
                                    Type);
}

__drv_maxIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
IoSetDeviceInterfacePropertyData (
    __in PUNICODE_STRING    SymbolicLinkName,
    __in CONST DEVPROPKEY   *PropertyKey,
    __in LCID               Lcid,
    __in ULONG              Flags,
    __in DEVPROPTYPE        Type,
    __in ULONG              Size,
    __in_bcount_opt(Size)
         PVOID              Data
    )

/*++

Description

    This routine sets the specified property on the device interface.

Parameters

    SymbolicLinkName - Supplies a pointer to the symbolic link name which was
        returned by IoRegisterDeviceInterface when the interface was registered,
        or as returned by IoGetDeviceInterfaces.

    PropertyKey - Pointer to the property key identifying the property.

    Lcid - Optional language identifier. If not specified, property is set for the default
        language.

    Flags -
        PLUGPLAY_PROPERTY_PERSISTENT - Property should be persisted across boots.

    Type - Custom property type.

    Size - Custom property data size.

    Data - Pointer to the custom property data. If NULL, the specified property is
        deleted.

Return Value

    NTSTATUS.

Headers

    Declared in wdm.h and ntddk.h. Include wdm.h or ntddk.h.

Comments

    Callers of IoSetDeviceInterfacePropertyData must be running at IRQL = PASSIVE_LEVEL.

See Also

    IoGetDeviceInterfacePropertyData

--*/

{
    PAGED_CODE();

    if (!ARGUMENT_PRESENT(Data)) {
        Type = DEVPROP_TYPE_EMPTY;
        Size = 0;
    }

    return PnpSetDeviceInterfacePropertyData(SymbolicLinkName,
                                             PropertyKey,
                                             Lcid,
                                             Flags,
                                             Type,
                                             Size,
                                             Data);
}

__drv_maxIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
IoGetDeviceInterfacePropertyData (
    __in PUNICODE_STRING    SymbolicLinkName,
    __in CONST DEVPROPKEY   *PropertyKey,
    __in LCID               Lcid,
    __reserved ULONG        Flags,
    __in ULONG              Size,
    __out_bcount_part(Size, *RequiredSize)
          PVOID             Data,
    __out PULONG            RequiredSize,
    __out PDEVPROPTYPE      Type
    )

/*++

Description

    This routine gets the specified property on the device interface.

Parameters

    SymbolicLinkName - Supplies a pointer to the symbolic link name which was
        returned by IoRegisterDeviceInterface when the interface was registered,
        or as returned by IoGetDeviceInterfaces.

    PropertyKey - Pointer to the property key identifying the property.

    Lcid - Optional language identifier. If not specified, data for the default language
        is returned.

    Flags - Reserved.

    Size - Size of the Data in bytes.

    Data - Pointer to recieve the custom property data.

    RequiredSize - Pointer that recieves the required size for getting the data.

    Type - Pointer that recieves the custom property data type.

Return Value

    NTSTATUS.

Headers

    Declared in wdm.h and ntddk.h. Include wdm.h or ntddk.h.

Comments

    Callers of IoGetDeviceInterfacePropertyData must be running at IRQL = PASSIVE_LEVEL.

See Also

    IoSetDevicePropertyData

--*/

{

    PAGED_CODE();

    return PnpGetDeviceInterfacePropertyData(SymbolicLinkName,
                                             PropertyKey,
                                             Lcid,
                                             Flags,
                                             Size,
                                             Data,
                                             RequiredSize,
                                             Type);
}

__drv_maxIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
IoReplacePartitionUnit (
    __in PDEVICE_OBJECT TargetPdo,
    __in PDEVICE_OBJECT SparePdo,
    __in ULONG Flags
    )

/*++

Routine Description:

    This routine is used by drivers to replace the target partition unit with
    the spare partition unit.

Arguments:

    TargetPdo - Supplies a pointer to the target partition unit PDO.

    SparePdo - Supplies a pointer to the spare partition unit PDO.

    Flags - Always 0.

Return Value:

    NTSTAUS of the actual replace operation.

--*/

{
    PETHREAD CurrentThread;
    PNP_REPLACE_ARGUMENTS Arguments;

    PAGED_CODE();

    RtlZeroMemory(&Arguments, sizeof(PNP_REPLACE_ARGUMENTS));
    Arguments.TargetPdo = TargetPdo;
    Arguments.SparePdo = SparePdo;
    Arguments.Flags = Flags;
    Arguments.Status = STATUS_UNSUCCESSFUL;

    //
    // If this call is made in the context of the system process, perform the
    // replace directly.  Otherwise, queue it to a work item and wait for
    // completion.
    //

    KeInitializeEvent(&Arguments.CompletionEvent,
                      NotificationEvent,
                      FALSE);

    CurrentThread = PsGetCurrentThread();
    if (PsGetCurrentProcessByThread(CurrentThread) == PsInitialSystemProcess) {
        PnpReplacePartitionUnit(&Arguments);

    } else {
        ExInitializeWorkItem(&Arguments.WorkQueueItem,
                             PnpReplacePartitionUnit,
                             &Arguments);

        ExQueueWorkItem(&Arguments.WorkQueueItem, DelayedWorkQueue);
        KeWaitForSingleObject(&Arguments.CompletionEvent,
                              Executive,
                              KernelMode,
                              FALSE,
                              (PLARGE_INTEGER)NULL);
    }

    return Arguments.Status;
}

__drv_maxIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
IoGetDeviceNumaNode (
    __in PDEVICE_OBJECT Pdo,
    __out PUSHORT NodeNumber
    )

/*++

Routine Description:

    This routine returns the node number of the specified device, if known.

Arguments:

    Pdo - Supplies the physical device object to query.

    NodeNumber - Supplies a pointer to a variable that receives the device node
        number.

Return Value:

    STATUS_SUCCESS if the node number is returned successfully.

    STATUS_NOT_FOUND if the node number of this device is unknown.

    STATUS_INVALID_PARAMETER if the supplied device is invalid.

--*/

{

    PDEVICE_NODE DeviceNode;

    PAGED_CODE();

    if (IS_PDO(Pdo) == FALSE) {
        return STATUS_INVALID_PARAMETER;
    }

    DeviceNode = (PDEVICE_NODE)Pdo->DeviceObjectExtension->DeviceNode;
    if (DeviceNode == NULL) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Return the device's NUMA node.  If the device does not have a specified
    // node, return failure unless the machine only has a single node.
    //

    if ((DeviceNode->NumaNodeIndex == PNP_NUMA_NODE_UNKNOWN) ||
        (DeviceNode->NumaNodeIndex == PNP_NUMA_NODE_NONE)) {

        if (KeQueryHighestNodeNumber() == 0) {
            *NodeNumber = 0;
            return STATUS_SUCCESS;

        } else {
            return STATUS_NOT_FOUND;
        }

    } else {
        *NodeNumber = (USHORT)DeviceNode->NumaNodeIndex;
        return STATUS_SUCCESS;
    }
}

_Use_decl_annotations_
NTSTATUS
IoQueryInterface (
    PDEVICE_OBJECT Device,
    QUERY_INTERFACE_FLAGS Flags,
    CONST GUID *InterfaceType,
    USHORT Size,
    USHORT Version,
    PVOID InterfaceSpecificData,
    PINTERFACE Interface
)

/*++

Routine Description:

    This routine sends IRP_MN_QUERY_INTERFACE to device stacks and returns
    the result.

    By default, it is simply a convenience wrapper for sending the IRP to the
    provided device stack.

    When Flags.TraverseUp is set, the IRP is sent to a device stack, and if it
    does not implement it, the parent device in the device tree will be tried,
    until a device stack implements it or the tree root is reached.

Arguments:

    Device - Supplies a pointer to the device stack that will have the IRP
        sent. If Flags.TraverseUp is set, this indicates the first stack; its
        ancestors will be queried as well in ascending order, if necessary.

    Flags - Supplies a set of flags to control the behavior.

    InterfaceType - Supplies a pointer to the interface GUID that is being
        queried.

    Size - Supplies the size of the Interface buffer, in bytes.

    Version - Supplies the version being requested. An implementer will return
        an interface with the version that is closest to the requested version
        without exceeding the requested version.

    InterfaceSpecificData - Supplies extra data that will be sent to device
        stacks when the interface is being requested.

    Interface - Supplies a pointer to receive the interface implementation,
        valid only when the call is successful.

Return Value:

    NT status code.

--*/

{

    NTSTATUS Status;

    PAGED_CODE();

    if (Flags.TraverseUp != 0) {
        Status = IopQueryInterfaceRecurseUp(Device,
                                            InterfaceType,
                                            Size,
                                            Version,
                                            InterfaceSpecificData,
                                            Interface);

    } else {
        Status = PnpQueryInterface(Device,
                                   InterfaceType,
                                   Version,
                                   Size,
                                   InterfaceSpecificData,
                                   Interface);

    }

    return Status;
}

_Use_decl_annotations_
NTSTATUS
IopQueryInterfaceRecurseUp (
    PDEVICE_OBJECT Device,
    CONST GUID *InterfaceType,
    USHORT Size,
    USHORT Version,
    PVOID InterfaceSpecificData,
    PINTERFACE Interface
)

/*++

Routine Description:

    This routine sends IRP_MN_QUERY_INTERFACE to a device stack, and if it
    does not implement it, the parent device in the device tree will be tried,
    until a device stack implements it or the tree root is reached.

Arguments:

    Device - Supplies a pointer to the first stack to query; its ancestors will
        be queried as well, if required.

    InterfaceType - Supplies a pointer to the interface GUID that is being
        queried.

    Size - Supplies the size of the Interface buffer, in bytes.

    Version - Supplies the version being requested.

    InterfaceSpecificData - Supplies extra data that will be sent to device
        stacks when the interface is being requested.

    Interface - Supplies a pointer to receive the interface implementation,
        valid only when the call is successful.

Return Value:

    NT status code.

--*/

{

    PDEVICE_NODE DeviceNode;
    PDEVICE_OBJECT PhysicalDeviceObject;
    NTSTATUS Status;

    PAGED_CODE();

    PpDevNodeLockTree(PPL_SIMPLE_READ);
    PhysicalDeviceObject = IoGetDeviceAttachmentBaseRef(Device);
    DeviceNode = PP_DO_TO_DN(PhysicalDeviceObject);

    NT_ASSERT(DeviceNode != IopRootDeviceNode);
    
    while (TRUE) {

        //
        // Reached the tree root; skip the IRP and exit out.
        //

        if (DeviceNode == IopRootDeviceNode) {
            Status = STATUS_NOT_SUPPORTED;
            break;
        }
        
        //
        // The subtree is not attached to the device tree, meaning that it was
        // deleted.
        //

        if (DeviceNode == NULL ||
            DeviceNode->State == DeviceNodeDeleted) {

            Status = STATUS_DEVICE_REMOVED;
            break;
        }

        //
        // Send the query interface IRP.
        //

        Status = PnpQueryInterface(DeviceNode->PhysicalDeviceObject,
                                   InterfaceType,
                                   Version,
                                   Size,
                                   InterfaceSpecificData,
                                   Interface);

        if (Status == STATUS_NOT_SUPPORTED) {

            //
            // Current stack does not implement the interface.
            // Try the parent device.
            //

            DeviceNode = DeviceNode->Parent;

        } else {

            //
            // Current stack implements the interface; return with the result.
            //

            break;
        }
    }

    ObDereferenceObject(PhysicalDeviceObject);
    PpDevNodeUnlockTree(PPL_SIMPLE_READ);
    return Status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
IoSteerInterrupt (
    _In_ PIO_STEER_INTERRUPT_CONNECTION Connection,
    _In_ PIO_STEER_INTERRUPT_TARGET Target
    )

/*++

Routine Description:

    This routine steers an interrupt to the provided target.

    If the line is shared by multiple devices (i.e., multiple interrupts for
    given GSIV), all of their destinations will be modified as well.

Arguments:

    Connection - Supplies a pointer to the interrupt connection information.

    Target - Supplies a pointer to the interrupt destination.

Return Value:

    NT status code.

--*/

{

#if defined(XBOX_SYSTEMOS)

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Connection);
    UNREFERENCED_PARAMETER(Target);

    return STATUS_NOT_SUPPORTED;

#else

    PIO_STEER_INTERRUPT_CONNECTION_CONTEXT ConnectionContext;
    PIO_INTERRUPT_STRUCTURE InterruptStructure;
    PKINTERRUPT KernelInterruptObject;
    PIO_INTERRUPT_LINE_INFO LineInfo;
    PIO_INTERRUPT_MESSAGE_INFO MessageInfo;
    NTSTATUS Status;
    PKINTERRUPT UserInterruptObject;

    PAGED_CODE();

    Status = STATUS_UNSUCCESSFUL;
    if (KeGetCurrentIrql() != PASSIVE_LEVEL) {
#pragma prefast(suppress: 28159)
        KeBugCheckEx(DRIVER_VIOLATION,
                     DRIVER_IRQL_VIOLATION,
                     KeGetCurrentIrql(),
                     PASSIVE_LEVEL,
                     0);
    }

    if ((ARGUMENT_PRESENT(Connection) == FALSE) ||
        (ARGUMENT_PRESENT(Target) == FALSE)) {

        RtlFailFast(FAST_FAIL_INVALID_ARG);
    }

    if (Connection->Type != InterruptConnectionSingle) {
        Status = STATUS_INVALID_PARAMETER;
        goto AssignCpuSetToInterruptsEnd;
    }

    if (Target->Type >= InterruptTargetMax) {
        Status = STATUS_INVALID_PARAMETER;
        goto AssignCpuSetToInterruptsEnd;
    }

    if ((Target->Type == InterruptTargetCpuSetMask) &&
        (Target->CpuSetMask.Mask == 0)) {

        Status = STATUS_INVALID_PARAMETER;
        goto AssignCpuSetToInterruptsEnd;
    }

    //
    // Fetch the real interrupt object from the connection data.
    //

    ConnectionContext = &Connection->Interrupt;

#pragma prefast(suppress: 28159)
    ASSERT_PDO(ConnectionContext->PhysicalDeviceObject);

    switch (ConnectionContext->Version) {
    case CONNECT_LINE_BASED:
        LineInfo = CONTAINING_RECORD(
                       ConnectionContext->InterruptObject,
                       IO_INTERRUPT_LINE_INFO,
                       UserInterruptObject);

        UserInterruptObject = &LineInfo->InterruptInfo[0]->UserInterruptObject;
        break;

    case CONNECT_MESSAGE_BASED:
        MessageInfo = ConnectionContext->InterruptMessageTable;
        UserInterruptObject = MessageInfo->MessageInfo[0].InterruptObject;
        break;

    case CONNECT_FULLY_SPECIFIED:
    case CONNECT_FULLY_SPECIFIED_GROUP:
        UserInterruptObject = ConnectionContext->InterruptObject;
        break;

    default:
        Status = STATUS_NOT_SUPPORTED;
        goto AssignCpuSetToInterruptsEnd;
    }

    InterruptStructure = CONTAINING_RECORD(UserInterruptObject,
                                           IO_INTERRUPT_STRUCTURE,
                                           UserInterruptObject);

    KernelInterruptObject = InterruptStructure->InterruptObject[0];

    //
    // Sanity check
    //

    if (KernelInterruptObject->Vector != UserInterruptObject->Vector) {
        RtlFailFast(FAST_FAIL_INVALID_ARG);
    }

    //
    // Call into interrupt steering to set the new target.
    //

    if (Target->Type == InterruptTargetCpuSetMask) {
        Status = KeIntSteerAssignCpuSet(KernelInterruptObject,
                                        Target->CpuSetMask.Group,
                                        Target->CpuSetMask.Mask);

    } else if (Target->Type == InterruptTargetDefault) {
        Status = KeIntSteerAssignCpuSet(KernelInterruptObject, 0, 0);
    }

AssignCpuSetToInterruptsEnd:
    return Status;

#endif

}

