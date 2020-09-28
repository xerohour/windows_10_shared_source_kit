/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    utils.c

Abstract:

    This module contains the plug-and-play subroutines for the
    I/O system.


Author:

    Shie-Lin Tzong (shielint) 3-Jan-1995

Environment:

    Kernel mode

--*/

#include "pnpmgrp.h"
#pragma hdrstop
#include <devpkey.h>
#include <initguid.h>
#include <devpkeyp.h>

#ifdef POOL_TAGGING
#undef ExAllocatePool
#define ExAllocatePool(a,b) ExAllocatePoolWithTag(a,b,'uspP')
#endif

//
// Regular data segment
//

#ifdef ALLOC_DATA_PRAGMA
#pragma  data_seg()
#endif

#define DRIVER_DEPENDENCY_WILDCARD TEXT("*")

//
// Table to map InstancePath to DO.
//

RTL_GENERIC_TABLE PnpDeviceReferenceTable;

//
// Lock to synchronize access to the table.
//

KGUARDED_MUTEX PnpDeviceReferenceTableLock;

//
// Table of BusType GUIDs
//

GUID *PnpBusTypeGuidArray;

//
// Number of entries in the BusTypeGuid table.
//

ULONG PnpBusTypeGuidCount;

//
// Maximum number of entries in the BusTypeGuid table.
//

ULONG PnpBusTypeGuidCountMax;

//
// Lock used to synchronize access to the BusTypeGuid table.
//

KGUARDED_MUTEX PnpBusTypeGuidLock;

SYSTEM_HIVE_LIMITS PnpSystemHiveLimits = {0};
BOOLEAN PnpSystemHiveTooLarge = FALSE;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, PnpInitializeDeviceReferenceTable)
#pragma alloc_text(INIT, PnpRegMultiSzToUnicodeStrings)
#pragma alloc_text(INIT, PnpBusTypeGuidInitialize)
//#pragma alloc_text(NONPAGED, PnpIsChainDereferenced)
//#pragma alloc_text(NONPAGED, PnpFindMountableDevice)
//#pragma alloc_text(NONPAGED, PnpUpdateExtensionFlags)
//#pragma alloc_text(NONPAGED, PnpIsAnyDeviceInUse)
//#pragma alloc_text(NONPAGED, PnpIsSafeToExamineUserModeTeb)
#pragma alloc_text(PAGE, PnpCleanupDeviceRegistryValues)
#pragma alloc_text(PAGE, PnpCmResourcesToIoResources)
#pragma alloc_text(PAGE, IopAllocateUnicodeString)
#pragma alloc_text(PAGE, PnpConcatenateUnicodeStrings)
#pragma alloc_text(PAGE, PnpDetermineResourceListSize)
#pragma alloc_text(PAGE, PnpGenerateDeviceIdsHash)
#pragma alloc_text(PAGE, PnpSaveDeviceCapabilities)
#pragma alloc_text(PAGE, PnpQueryAndSaveDeviceNodeCapabilities)
#pragma alloc_text(PAGE, PnpDeviceObjectFromDeviceInstance)
#pragma alloc_text(PAGE, PnpDeviceObjectToDeviceInstance)
#pragma alloc_text(PAGE, PnpDisableDevice)
#pragma alloc_text(PAGE, PnpDriverLoadingFailed)
#pragma alloc_text(PAGE, PnpFilterResourceRequirementsList)
#pragma alloc_text(PAGE, PnpGetDeviceInstanceCsConfigFlags)
#pragma alloc_text(PAGE, PnpGetDeviceResourcesFromRegistry)
#pragma alloc_text(PAGE, PnpGetServiceInstanceCsConfigFlags)
#pragma alloc_text(PAGE, PnpInitializeSessionId)
#pragma alloc_text(PAGE, PnpIsAnyDeviceInstanceEnabled)
#pragma alloc_text(PAGE, PnpIsDeviceInstanceEnabled)
#pragma alloc_text(PAGE, PnpIsDuplicateDevice)
#pragma alloc_text(PAGE, PnpIsLegacyDriver)
#pragma alloc_text(PAGE, PnpMergeFilteredResourceRequirementsList)
#pragma alloc_text(PAGE, PnpPrepareDriverLoading)
#pragma alloc_text(PAGE, PnpReadDeviceConfiguration)
#pragma alloc_text(PAGE, PnpRestartDeviceNode)
#pragma alloc_text(PAGE, PnpMapDeviceObjectToDeviceInstance)
#pragma alloc_text(PAGE, PnpRegSzToString)
#pragma alloc_text(PAGE, PnpCompareInstancePath)
#pragma alloc_text(PAGE, PnpAllocateGenericTableEntry)
#pragma alloc_text(PAGE, PnpFreeGenericTableEntry)
#pragma alloc_text(PAGE, PnpLogEvent)
#pragma alloc_text(PAGE, PnpFixupID)
#pragma alloc_text(PAGE, PnpQueryID)
#pragma alloc_text(PAGE, PnpQueryDeviceID)
#pragma alloc_text(PAGE, PnpQueryBusInformation)
#pragma alloc_text(PAGE, PnpBusTypeGuidGetIndex)
#pragma alloc_text(PAGE, PnpBusTypeGuidGet)
#pragma alloc_text(PAGE, PnpDriverObjectDereferenceComplete)
#pragma alloc_text(PAGE, PnpDriverStarted)
#pragma alloc_text(PAGE, IopDeviceObjectFromSymbolicName)
#pragma alloc_text(PAGE, IopGetSessionIdFromSymbolicName)
#pragma alloc_text(PAGE, IopGetSessionIdFromPDO)
#pragma alloc_text(PAGE, PnpValidateMultiSz)
#pragma alloc_text(PAGE, PnpAllocatePWSTR)
#pragma alloc_text(PAGE, PnpConcatPWSTR)
#pragma alloc_text(PAGE, PnpGetMultiSzLength)
#pragma alloc_text(PAGE, PnpCompareMultiSz)
#pragma alloc_text(PAGE, PnpAllocateMultiSZ)
#pragma alloc_text(PAGE, PnpCopyDevProperty)
#pragma alloc_text(PAGE, PnpFreeDevProperty)
#pragma alloc_text(PAGE, PnpCopyDevPropertyArray)
#pragma alloc_text(PAGE, PnpFreeDevPropertyArray)
#pragma alloc_text(PAGE, PnpUnicodeStringToWstr)
#pragma alloc_text(PAGE, PnpUnicodeStringToWstrFree)
#pragma alloc_text(PAGE, PnpIsValidGuidString)
#pragma alloc_text(PAGE, PnpIsNullGuid)
#pragma alloc_text(PAGE, PnpIsNullGuidString)
#pragma alloc_text(PAGE, PnpStringFromGuid)
#pragma alloc_text(PAGE, PnpGetObjectProperty)
#pragma alloc_text(PAGE, PnpGetCallerSessionId)
#pragma alloc_text(PAGE, PnpCheckDeviceIdsChanged)
#pragma alloc_text(PAGE, PnpConvertDevpropcompkeyArrayToString)
#pragma alloc_text(PAGE, IopReplaceSeperatorWithPound)
#pragma alloc_text(PAGE, PnpQueryInterface)
#pragma alloc_text(PAGE, PnpGetDeviceLocationStrings)
#pragma alloc_text(PAGE, PnpOpenFirstMatchingSubKey)
#pragma alloc_text(PAGE, PnpCheckDriverDependencies)
#pragma alloc_text(PAGE, PnpGetStableSystemBootTime)
#pragma alloc_text(PAGE, PnpLogDeviceConflictingResource)
#pragma alloc_text(PAGE, PnpLogDeviceRebalanceResult)

#endif

NTSTATUS
IopAllocateUnicodeString(
    __inout PUNICODE_STRING String,
    __in USHORT Length
    )

/*++

Routine Description:

    This routine allocates a buffer for a unicode string of a given length
    and initialises the UNICODE_STRING structure appropriately. When the
    string is no longer required it can be freed using RtlFreeUnicodeString.
    The buffer also be directly deleted by ExFreePool and so can be handed
    back to a caller.

Parameters:

    String - Supplies a pointer to an uninitialised unicode string which will
        be manipulated by the function.

    Length - The number of BYTES long that the string will be.

Return Value:

    Either STATUS_INSUFFICIENT_RESOURCES indicating paged pool is exhausted or
    STATUS_SUCCESS.

Remarks:

    The buffer allocated will be one character (2 bytes) more than length
    specified. This is to allow for easy null termination of the strings - eg
    for registry storage.

--*/

{
    NTSTATUS status;

    PAGED_CODE();

    String->Length = 0;

    status = RtlUShortAdd(Length,
                          sizeof(UNICODE_NULL),
                          &(String->MaximumLength));

    if (!NT_SUCCESS(status)) {
        goto Exit;
    }

    String->Buffer = ExAllocatePool(PagedPool, String->MaximumLength);
    if (String->Buffer == NULL) {

        String->MaximumLength = 0;
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

  Exit:

    return status;
}

NTSTATUS
PnpConcatenateUnicodeStrings (
    __out PUNICODE_STRING Destination,
    __in  PUNICODE_STRING String1,
    __in_opt PUNICODE_STRING String2
    )

/*++

Routine Description:

    This routine returns a buffer containing the concatenation of the
    two specified strings.  Since String2 is optional, this function may
    also be used to make a copy of a unicode string.  Paged pool space
    is allocated for the destination string.  Caller must release the
    space once done with it.

Parameters:

    Destination - Supplies a variable to receive the concatenated
        UNICODE_STRING.

    String1 - Supplies a pointer to the frist UNICODE_STRING.

    String2 - Supplies an optional pointer to the second UNICODE_STRING.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    NTSTATUS status;
    USHORT length;

    PAGED_CODE();

    length = String1->Length;
    if (ARGUMENT_PRESENT(String2)) {
        status = RtlUShortAdd(length, String2->Length, &length);
        if (!NT_SUCCESS(status)) {
            goto ConcatenateUnicodeStringsEnd;
        }
    }

    status = IopAllocateUnicodeString(Destination, length);
    if (!NT_SUCCESS(status)) {
        goto ConcatenateUnicodeStringsEnd;
    }

    RtlCopyUnicodeString(Destination, String1);
    if (ARGUMENT_PRESENT(String2)) {
        RtlAppendUnicodeStringToString(Destination, String2);
    }

    status = STATUS_SUCCESS;

ConcatenateUnicodeStringsEnd:
    return status;
}

NTSTATUS
PnpPrepareDriverLoading (
    __in PUNICODE_STRING KeyName,
    __in HANDLE KeyHandle,
    __in PVOID ImageBase,
    __in LOGICAL IsFilter,
    __inout PULONG PnpFlags
    )

/*++

Routine Description:

    This routine first checks if the driver is loadable.  If its a
    PnP driver, it will always be loaded (we trust it to do the right
    things.)  If it is a legacy driver, we need to check if its device
    has been disabled.  Once we decide to load the driver, the Enum
    subkey of the service node will be checked for duplicates, if any.

Parameters:

    KeyName - Supplies a pointer to the driver's service key unicode string

    KeyHandle - Supplies a handle to the driver service node in the registry
        that describes the driver to be loaded.

    ImageBase - driver load address in memory.

    IsFilter - Specifies whether this is a filter driver or not.

    PnpFlags - value of the PnP flags key under the service name.

Return Value:

    The function value is the final status of the load operation.

--*/

{
    NTSTATUS status;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;

#if defined(XBOX_SYSTEMOS)

    UNREFERENCED_PARAMETER(KeyName);
    UNREFERENCED_PARAMETER(ImageBase);
    UNREFERENCED_PARAMETER(IsFilter);

#else

    PIMAGE_NT_HEADERS header;
    GUID blockedDriverGuid;

#endif

    PAGED_CODE();

    *PnpFlags = 0;

#if defined(XBOX_SYSTEMOS)

    status = STATUS_SUCCESS;

#else

    header = RtlImageNtHeader(ImageBase);

    if (header == NULL) {
        return STATUS_UNSUCCESSFUL;
    }

    RtlZeroMemory(&blockedDriverGuid, sizeof(GUID));

    status = PpCheckInDriverDatabase(
            KeyName,
            KeyHandle,
            ImageBase,
            header->OptionalHeader.SizeOfImage,
            IsFilter,
            &blockedDriverGuid);

    if (status == STATUS_DRIVER_BLOCKED ||
        status == STATUS_DRIVER_BLOCKED_CRITICAL) {
        //
        // Notify the user-mode Plug and Play manager that a driver was just
        // blocked.
        //
        PnpSetBlockedDriverEvent(&blockedDriverGuid);
    }

#endif

    if (NT_SUCCESS(IopGetRegistryValue(KeyHandle,
                                       REGSTR_VALUE_PNP_FLAGS,
                                       0,
                                       &keyValueInformation))) {

        if (keyValueInformation->Type == REG_DWORD &&
            keyValueInformation->DataLength == sizeof(ULONG)) {

            *PnpFlags = *(PULONG)KEY_VALUE_DATA(keyValueInformation);
        }
        ExFreePool(keyValueInformation);
    }

    return status;
}

NTSTATUS
PnpGetDeviceInstanceCsConfigFlags(
    __in PUNICODE_STRING DeviceInstance,
    __in ULONG ProfileId,
    __out PULONG CsConfigFlags
    )

/*++

Routine Description:

    This routine retrieves the csconfig flags for the specified device.

Arguments:

    DeviceInstance - Supplies a pointer to the devnode's instance path.

    CsConfigFlags - Supplies a variable to receive the device's CsConfigFlags.

Return Value:

    status

--*/

{
    HANDLE handle;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;
    NTSTATUS status;
    PWSTR tempDeviceInstance;

    PAGED_CODE();

    PNP_ASSERT(DeviceInstance != NULL);
    PNP_ASSERT(CsConfigFlags != NULL);

    *CsConfigFlags = 0;

    status = PnpUnicodeStringToWstr(&tempDeviceInstance, NULL, DeviceInstance);

    if (!NT_SUCCESS(status)) {

        return status;
    }

    //
    // Open the key for the specified device under the current hardware profile
    // key.
    //

    status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                 tempDeviceInstance,
                                 CM_KEY_TYPE_DEVICE_INSTANCE | CM_KEY_TYPE_SCOPE_CONFIG,
                                 ProfileId,
                                 KEY_READ,
                                 FALSE,
                                 &handle,
                                 NULL);

    PnpUnicodeStringToWstrFree(tempDeviceInstance, DeviceInstance);

    if (!NT_SUCCESS(status)) {

        return status;
    }

    //
    // Get the hardware profile specific config flags.
    //

    status = IopGetRegistryValue( handle,
                                  REGSTR_VALUE_CSCONFIG_FLAGS,
                                  0,
                                  &keyValueInformation
                                  );

    ZwClose(handle);

    if (NT_SUCCESS(status)) {
        if ((keyValueInformation->Type == REG_DWORD) &&
            (keyValueInformation->DataLength >= sizeof(ULONG))) {

            *CsConfigFlags = *(PULONG)KEY_VALUE_DATA(keyValueInformation);
        }
        ExFreePool(keyValueInformation);
    }

    return status;
}

NTSTATUS
PnpGetServiceInstanceCsConfigFlags(
    __in PUNICODE_STRING ServiceKeyName,
    __in ULONG Instance,
    __out PULONG CsConfigFlags
    )

/*++

Routine Description:

    This routine retrieves the csconfig flags for the specified device
    which is specified by the instance number under ServiceKeyName\Enum.

Arguments:

    ServiceKeyName - Supplies a pointer to the name of the subkey in the
        system service list (HKEY_LOCAL_MACHINE\CurrentControlSet\Services)
        that caused the driver to load.

    Instance - Supplies the instance value under ServiceKeyName\Enum key

    CsConfigFlags - Supplies a variable to receive the device's CsConfigFlags

Return Value:

    status

--*/

{
    NTSTATUS status;
    HANDLE handle;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;

    PAGED_CODE();

    *CsConfigFlags = 0;

    status = IopOpenCurrentHwProfileDeviceInstanceKey(&handle,
                                                      ServiceKeyName,
                                                      Instance,
                                                      KEY_READ,
                                                      FALSE
                                                     );
    if (NT_SUCCESS(status)) {
        status = IopGetRegistryValue(handle,
                                     REGSTR_VALUE_CSCONFIG_FLAGS,
                                     0,
                                     &keyValueInformation
                                    );
        if (NT_SUCCESS(status)) {
            if ((keyValueInformation->Type == REG_DWORD) &&
                (keyValueInformation->DataLength >= sizeof(ULONG))) {
                *CsConfigFlags = *(PULONG)KEY_VALUE_DATA(keyValueInformation);
            }
            ExFreePool(keyValueInformation);
        }
        ZwClose(handle);
    }
    return status;
}

NTSTATUS
PnpRegMultiSzToUnicodeStrings(
    __in  PKEY_VALUE_FULL_INFORMATION KeyValueInformation,
    __deref_out_ecount(*UnicodeStringCount) PUNICODE_STRING *UnicodeStringList,
    __out PULONG UnicodeStringCount
    )

/*++

Routine Description:

    This routine takes a KEY_VALUE_FULL_INFORMATION structure containing
    a REG_MULTI_SZ value, and allocates an array of UNICODE_STRINGs,
    initializing each one to a copy of one of the strings in the value entry.
    All the resulting UNICODE_STRINGs will be NULL terminated
    (MaximumLength = Length + sizeof(UNICODE_NULL)).

    It is the responsibility of the caller to free the buffers for each
    unicode string, as well as the buffer containing the UNICODE_STRING
    array. This may be done by calling PnpFreeUnicodeStringList.

Arguments:

    KeyValueInformation - Supplies the buffer containing the REG_MULTI_SZ
        value entry data.

    UnicodeStringList - Receives a pointer to an array of UNICODE_STRINGs, each
        initialized with a copy of one of the strings in the REG_MULTI_SZ.

    UnicodeStringCount - Receives the number of strings in the
        UnicodeStringList.

Returns:

    NT status code indicating whether the function was successful.

    NOTE: This function is only available during INIT time!

--*/

{
    PWCHAR p, BufferEnd, StringStart;
    ULONG StringCount, i, StringLength;

    //
    // First, make sure this is really a REG_MULTI_SZ value.
    //
    if(KeyValueInformation->Type != REG_MULTI_SZ) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Make a preliminary pass through the buffer to count the number of strings
    // There will always be at least one string returned (possibly empty).
    //
    // FUTURE: Make this robust against odd length buffers.
    //
    StringCount = 0;
    p = (PWCHAR)KEY_VALUE_DATA(KeyValueInformation);
    BufferEnd = (PWCHAR)((PUCHAR)p + KeyValueInformation->DataLength);
    while(p != BufferEnd) {
        if(!*p) {
            StringCount++;
            if(((p + 1) == BufferEnd) || !*(p + 1)) {
                break;
            }
        }
        p++;
    }
    if(p == BufferEnd) {
        StringCount++;
    }

    *UnicodeStringList = ExAllocatePool(PagedPool, sizeof(UNICODE_STRING) * StringCount);
    if(!(*UnicodeStringList)) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Now, make a second pass through the buffer making copies of each string.
    //
    i = 0;
    StringStart = p = (PWCHAR)KEY_VALUE_DATA(KeyValueInformation);
    while(p != BufferEnd) {
        if(!*p) {
            StringLength = (ULONG)((PUCHAR)p - (PUCHAR)StringStart) + sizeof(UNICODE_NULL);
            (*UnicodeStringList)[i].Buffer = ExAllocatePool(PagedPool, StringLength);

            if(!((*UnicodeStringList)[i].Buffer)) {
                PnpFreeUnicodeStringList(*UnicodeStringList, i);
                return STATUS_INSUFFICIENT_RESOURCES;
            }

            RtlCopyMemory((*UnicodeStringList)[i].Buffer, StringStart, StringLength);

            (*UnicodeStringList)[i].Length =
                ((*UnicodeStringList)[i].MaximumLength = (USHORT)StringLength)
                - sizeof(UNICODE_NULL);

            i++;

            if(((p + 1) == BufferEnd) || !*(p + 1)) {
                break;
            } else {
                StringStart = p + 1;
            }
        }
        p++;
    }
    if(p == BufferEnd) {
        StringLength = (ULONG)((PUCHAR)p - (PUCHAR)StringStart);
        (*UnicodeStringList)[i].Buffer = ExAllocatePool(PagedPool,
                                                        StringLength + sizeof(UNICODE_NULL)
                                                       );
        if(!((*UnicodeStringList)[i].Buffer)) {
            PnpFreeUnicodeStringList(*UnicodeStringList, i);
            return STATUS_INSUFFICIENT_RESOURCES;
        }
        if(StringLength) {
            RtlCopyMemory((*UnicodeStringList)[i].Buffer, StringStart, StringLength);
        }
        (*UnicodeStringList)[i].Buffer[CB_TO_CWC(StringLength)] = UNICODE_NULL;

        (*UnicodeStringList)[i].MaximumLength =
                ((*UnicodeStringList)[i].Length = (USHORT)StringLength)
                + sizeof(UNICODE_NULL);
    }

    *UnicodeStringCount = StringCount;

    return STATUS_SUCCESS;
}

LOGICAL
PnpIsDuplicateDevice (
    __in PCM_RESOURCE_LIST Configuration1,
    __in PCM_RESOURCE_LIST Configuration2,
    __in_opt PHAL_BUS_INFORMATION BusInfo1 ,
    __in_opt PHAL_BUS_INFORMATION BusInfo2
    )

/*++

Routine Description:

    This routine compares two set of configurations and bus information to
    determine if the resources indicate the same device.  If BusInfo1 and
    BusInfo2 both are absent, it means caller wants to compare the raw
    resources.

Arguments:

    Configuration1 - Supplies a pointer to the first set of resource.

    Configuration2 - Supplies a pointer to the second set of resource.

    BusInfo1 - Supplies a pointer to the first set of bus information.

    BusInfo2 - Supplies a pointer to the second set of bus information.

Return Value:

    returns TRUE if the two set of resources indicate the same device;
    otherwise a value of FALSE is returned.

--*/

{
    PCM_PARTIAL_RESOURCE_LIST list1, list2;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR descriptor1, descriptor2;

    ULONG i, j;
    ULONG pass = 0;

    //
    // The BusInfo for both resources must be both present or not present.
    //

    if ((ARGUMENT_PRESENT(BusInfo1) && !ARGUMENT_PRESENT(BusInfo2)) ||
        (!ARGUMENT_PRESENT(BusInfo1) && ARGUMENT_PRESENT(BusInfo2))) {

        //
        // Unable to determine.
        //

        return FALSE;
    }

    //
    // Next check resources used by the two devices.
    // Currently, we *only* check the Io ports.
    //

    if (Configuration1->Count == 0 || Configuration2->Count == 0) {

        //
        // If any one of the configuration data is empty, we assume
        // the devices are not duplicates.
        //

        return FALSE;
    }

RedoScan:

    list1 = &(Configuration1->List[0].PartialResourceList);
    list2 = &(Configuration2->List[0].PartialResourceList);

    for(i = 0, descriptor1 = list1->PartialDescriptors;
        i < list1->Count;
        i++, descriptor1++) {

        //
        // If this is an i/o port or a memory range then look for a match
        // in the other list.
        //

        if((descriptor1->Type == CmResourceTypePort) ||
           (descriptor1->Type == CmResourceTypeMemory) ||
           (descriptor1->Type == CmResourceTypeMemoryLarge)) {

            for(j = 0, descriptor2 = list2->PartialDescriptors;
                j < list2->Count;
                j++, descriptor2++) {

                //
                // If the types match then check to see if both addresses
                // match as well.  If bus info was provided then go ahead
                // and translate the ranges first.
                //

                if(descriptor1->Type == descriptor2->Type) {

                    PHYSICAL_ADDRESS range1;
                    PHYSICAL_ADDRESS range1Translated;
                    PHYSICAL_ADDRESS range2;
                    PHYSICAL_ADDRESS range2Translated;
                    ULONG range1IoSpace;
                    ULONG range2IoSpace;

                    range1 = descriptor1->u.Generic.Start;
                    range2 = descriptor2->u.Generic.Start;
                    range1IoSpace = (descriptor1->Type == CmResourceTypePort) ?
                            ADDRESS_SPACE_PORT : ADDRESS_SPACE_MEMORY;
                    range2IoSpace = (descriptor2->Type == CmResourceTypePort) ?
                            ADDRESS_SPACE_PORT : ADDRESS_SPACE_MEMORY;
                    if((range1.QuadPart == 0) ||
                       (BusInfo1 == NULL) ||
                       (HalTranslateBusAddress(
                            BusInfo1->BusType,
                            BusInfo1->BusNumber,
                            range1,
                            &range1IoSpace,
                            &range1Translated) == FALSE)) {

                        range1Translated = range1;
                    }

                    if((range2.QuadPart == 0) ||
                       (BusInfo2 == NULL) ||
                       (HalTranslateBusAddress(
                            BusInfo2->BusType,
                            BusInfo2->BusNumber,
                            range2,
                            &range2IoSpace,
                            &range2Translated) == FALSE)) {

                        range2Translated = range2;
                    }

                    //
                    // If the ranges are in the same space and start at the
                    // same location then break out and go on to the next
                    // range
                    //

                    if((range1Translated.QuadPart == range2Translated.QuadPart) &&
                       (range1IoSpace == range2IoSpace)) {

                        break;
                    }
                }
            }

            //
            // If we made it all the way through the resource list without
            // finding a match then these are not duplicates.
            //

            if(j == list2->Count) {
                return FALSE;
            }
        }
    }

    //
    // If every resource in list 1 exists in list 2 then we also need to make
    // sure that every resource in list 2 exists in list 1.
    //

    if(pass == 0) {

        PVOID tmp ;

        tmp = Configuration2;
        Configuration2 = Configuration1;
        Configuration1 = tmp;

        tmp = BusInfo2;
        BusInfo2 = BusInfo1;
        BusInfo1 = tmp;

        pass = 1;

        goto RedoScan;
    }

    return TRUE;
}

VOID
PnpFreeUnicodeStringList(
    __in_ecount(StringCount) PUNICODE_STRING UnicodeStringList,
    __in ULONG StringCount
    )

/*++

Routine Description:

    This routine frees the buffer for each UNICODE_STRING in the specified list
    (there are StringCount of them), and then frees the memory used for the
    string list itself.

Arguments:

    UnicodeStringList - Supplies a pointer to an array of UNICODE_STRINGs.

    StringCount - Supplies the number of strings in the UnicodeStringList array.

Returns:

    None.

    NOTE: This function is only available during INIT time!

--*/

{
    ULONG i;

    if (UnicodeStringList) {

        for (i = 0; i < StringCount; i++) {

            if (UnicodeStringList[i].Buffer) {

                ExFreePool(UnicodeStringList[i].Buffer);
            }
        }
        ExFreePool(UnicodeStringList);
    }
}

NTSTATUS
PnpDriverLoadingFailed (
    __in_opt HANDLE ServiceHandle ,
    __in_opt PUNICODE_STRING ServiceName
    )

/*++

Routine Description:

    This routine is invoked when driver failed to start.  All the device
    instances controlled by this driver/service are marked as failing to
    start.

Arguments:

    ServiceKeyHandle - Optionally, supplies a handle to the driver service node in the
        registry that controls this device instance.  If this argument is not specified,
        then ServiceKeyName is used to specify the service entry.

    ServiceKeyName - Optionally supplies the name of the service entry that controls
        the device instance. This must be specified if ServiceKeyHandle isn't given.

Returns:

    None.

--*/

{
    LOGICAL closeHandle;
    ULONG count;
    LOGICAL deletePdo;
    ULONG deviceFlags;
    UNICODE_STRING deviceInstanceName;
    ULONG i;
    ULONG j;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;
    ULONG newCount;
    HANDLE serviceEnumHandle;
    NTSTATUS status;
    WCHAR unicodeBuffer[20];
    UNICODE_STRING unicodeValueName;

    PAGED_CODE();

    closeHandle = FALSE;

    //
    // Open registry ServiceKeyName\Enum branch.
    //

    if (!ARGUMENT_PRESENT(ServiceHandle)) {
        status = PipOpenServiceEnumKeys(ServiceName,
                                        KEY_READ,
                                        &ServiceHandle,
                                        &serviceEnumHandle,
                                        FALSE
                                        );
        closeHandle = TRUE;
    } else {
        PiWstrToUnicodeString(&unicodeValueName, REGSTR_KEY_ENUM);
        status = IopOpenRegistryKeyEx(&serviceEnumHandle,
                                      ServiceHandle,
                                      &unicodeValueName,
                                      KEY_READ
                                      );
    }
    if (!NT_SUCCESS( status )) {

        //
        // No Service Enum key? no device instance.  Return FALSE.
        //

        return status;
    }

    //
    // Set "INITSTARTFAILED" flags so we won't load it again.
    //

    PiWstrToUnicodeString(&unicodeValueName, L"INITSTARTFAILED");
    deviceFlags = 1;
    ZwSetValueKey(serviceEnumHandle,
                  &unicodeValueName,
                  TITLE_INDEX_VALUE,
                  REG_DWORD,
                  &deviceFlags,
                  sizeof(deviceFlags)
                  );

    //
    // Find out how many device instances listed in the ServiceName's Enum key.
    //

    status = IopGetRegistryValue(serviceEnumHandle,
                                 REGSTR_VALUE_COUNT,
                                 0,
                                 &keyValueInformation
                                 );
    count = 0;
    if (NT_SUCCESS(status)) {
        if ((keyValueInformation->Type == REG_DWORD) &&
            (keyValueInformation->DataLength >= sizeof(ULONG))) {

            count = *(PULONG)KEY_VALUE_DATA(keyValueInformation);
        }
        ExFreePool(keyValueInformation);
    }
    if (count == 0) {
        ZwClose(serviceEnumHandle);
        if (closeHandle) {
            ZwClose(ServiceHandle);
        }
        return status;
    }

    //
    // Walk through each registered device instance to mark its Problem and
    // StatusFlags as fail to start.
    //

    newCount = count;
    for (i = 0; i < count; i++) {
        deletePdo = FALSE;
        status = PipServiceInstanceToDeviceInstance(ServiceHandle,
                                                    ServiceName,
                                                    i,
                                                    &deviceInstanceName,
                                                    NULL,
                                                    0
                                                    );

        if (NT_SUCCESS(status)) {

            PDEVICE_NODE deviceNode;
            PDEVICE_OBJECT deviceObject;

            //
            // If the device instance is a detected device reported during driver's
            // DriverEntry we need to clean it up.
            //

            deviceObject = PnpDeviceObjectFromDeviceInstance(&deviceInstanceName);
            if (deviceObject != NULL) {
                deviceNode = (PDEVICE_NODE)deviceObject->DeviceObjectExtension->DeviceNode;

                //
                // There are cases where a filter may have been loaded and
                // unloaded for a device and then removed from its instance key.
                // However, the service enum key for that driver would still
                // have a stale value for that device.  This is often the case
                // with pnpdtest.
                //
                // If ever that filter was loaded again and failed, this routine
                // would attempt to release resources for EVERY device in that
                // driver's service enum key, regardless if the driver was
                // on that device's stack or not.  As such, ensure that
                // resources are not released for started devices.
                //

                if ((deviceNode != NULL) &&
                    (PipIsDevNodeDNStarted(deviceNode) == FALSE)) {

                    IopReleaseDeviceResources(deviceNode, TRUE);
                }
            }

            //
            // We cannot lock the registry before releasing resources as we
            // callout to the drivers who may try to call back into PNP
            // to read the registry.
            //

            PiLockPnpRegistry(FALSE);
            if (deviceObject != NULL) {
                deviceNode = (PDEVICE_NODE)deviceObject->DeviceObjectExtension->DeviceNode;
                if (deviceNode != NULL) {
                    if ((deviceNode->Flags & DNF_MADEUP) &&
                        ((deviceNode->State == DeviceNodeStarted) ||
                        (deviceNode->State == DeviceNodeStartPostWork))) {

                        //
                        // Notify the power manager to abandon this device.
                        //

                        PoFxAbandonDevice(deviceNode);

                        //
                        // Now mark this one deleted.
                        //

                        PipSetDevNodeState(deviceNode, DeviceNodeRemoved, NULL);
                        PipSetDevNodeProblem(deviceNode, CM_PROB_DEVICE_NOT_THERE, STATUS_SUCCESS);
                        deletePdo = TRUE;
                    }
                }

                ObDereferenceObject(deviceObject);
            }

            ExFreePool(deviceInstanceName.Buffer);

            PiUnlockPnpRegistry();
        }
    }

    //
    // If some instance value entry is deleted, we need to update the count of
    // instance value entries and rearrange the instance value entries under
    // service enum key.
    //

    if (newCount != count) {

        PiLockPnpRegistry(FALSE);

        if (newCount != 0) {
            j = 0;
            i = 0;
            while (i < count) {
                PiUlongToUnicodeString(&unicodeValueName, unicodeBuffer, 20, i);
                status = IopGetRegistryValue(serviceEnumHandle,
                                             unicodeValueName.Buffer,
                                             0,
                                             &keyValueInformation
                                             );
                if (NT_SUCCESS(status)) {
                    if (i != j) {

                        //
                        // Need to change the instance i to instance j.
                        //

                        ZwDeleteValueKey(serviceEnumHandle, &unicodeValueName);

                        PiUlongToUnicodeString(&unicodeValueName, unicodeBuffer, 20, j);
                        ZwSetValueKey (serviceEnumHandle,
                                       &unicodeValueName,
                                       TITLE_INDEX_VALUE,
                                       REG_SZ,
                                       (PVOID)KEY_VALUE_DATA(keyValueInformation),
                                       keyValueInformation->DataLength
                                       );
                    }
                    ExFreePool(keyValueInformation);
                    j++;
                }
                i++;
            }
        }

        //
        // Don't forget to update the "Count=" and "NextInstance=" value entries.
        //

        PiWstrToUnicodeString( &unicodeValueName, REGSTR_VALUE_COUNT);

        ZwSetValueKey(serviceEnumHandle,
                      &unicodeValueName,
                      TITLE_INDEX_VALUE,
                      REG_DWORD,
                      &newCount,
                      sizeof (newCount)
                      );
        PiWstrToUnicodeString( &unicodeValueName, REGSTR_VALUE_NEXT_INSTANCE);

        ZwSetValueKey(serviceEnumHandle,
                      &unicodeValueName,
                      TITLE_INDEX_VALUE,
                      REG_DWORD,
                      &newCount,
                      sizeof (newCount)
                      );

        PiUnlockPnpRegistry();
    }

    ZwClose(serviceEnumHandle);
    if (closeHandle) {
        ZwClose(ServiceHandle);
    }

    return STATUS_SUCCESS;
}

VOID
PnpDisableDevice(
    __in PDEVICE_NODE   DeviceNode,
    __in ULONG          Problem
    )

/*++

Routine Description:

    This routine tries to ask a bus driver stopping decoding resources

Arguments:

    DeviceNode - Specifies the device to be disabled.

    Handle - specifies the device instance handle.

Returns:

    None.

--*/

{

    NTSTATUS status;

    PAGED_CODE();

    status = STATUS_SUCCESS;
    switch(DeviceNode->State) {
    case DeviceNodeQueryStopped:
    case DeviceNodeStopped:
    case DeviceNodeRestartCompletion:
    case DeviceNodeEnumeratePending:
    case DeviceNodeEnumerateCompletion:

        //
        // Not expecting these states.
        //

        PNP_ASSERT(0);

        //
        // fall through
        //

    case DeviceNodeDriversAdded:
    case DeviceNodeResourcesAssigned:
    case DeviceNodeStartPending:
    case DeviceNodeStartCompletion:
    case DeviceNodeStartPostWork:
    case DeviceNodeStarted:

        //
        // If the device has boot config, we will query-remove and remove the
        // device to free the boot config if possible.
        //

        status = PiIrpQueryRemoveDevice(DeviceNode->PhysicalDeviceObject,
                                        NULL);

        if (!NT_SUCCESS(status)) {
            IopRemoveDevice(DeviceNode->PhysicalDeviceObject,
                            IRP_MN_CANCEL_REMOVE_DEVICE);
        }

        //
        // fall through
        //

    case DeviceNodeUninitialized:
    case DeviceNodeInitialized:

        if (NT_SUCCESS(status)) {
            status = IopRemoveDevice(DeviceNode->PhysicalDeviceObject,
                                     IRP_MN_REMOVE_DEVICE);

            PNP_ASSERT(NT_SUCCESS(status));

            IopReleaseDeviceResources(DeviceNode, TRUE);
        }

        break;

    default:

        //
        // Not expecting other states.
        //

        PNP_ASSERT(0);

        break;
    }

    if (PipDoesDevNodeHaveProblem(DeviceNode)) {
        PipClearDevNodeProblem(DeviceNode);
    }

    PipSetDevNodeProblem(DeviceNode, Problem, STATUS_SUCCESS);
}

LOGICAL
PnpIsAnyDeviceInstanceEnabled(
    __in PUNICODE_STRING ServiceKeyName,
    __in_opt HANDLE ServiceHandle
    )

/*++

Routine Description:

    This routine checks if any of the devices instances is turned on for the
    specified service. This routine is used for Pnp Driver only and is temporary
    function to support SUR.

Arguments:

    ServiceKeyName - Specifies the service key unicode name

    ServiceHandle - Optionally supplies a handle to the service key to be
        checked.

    LegacyIncluded - TRUE, a legacy device instance key is counted as a device
        instance.
                     FALSE, a legacy device instance key is not counted.

Returns:

    A BOOLEAN value.

--*/

{
    LOGICAL closeHandle;
    ULONG count;
    LOGICAL enabled;
    HANDLE handle;
    ULONG i;
    LOGICAL instanceEnabled;
    UNICODE_STRING instancePath;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;
    HANDLE serviceEnumHandle;
    NTSTATUS status;
    UNICODE_STRING unicodeName;

    PAGED_CODE();

    //
    // Initialize for proper cleanup.
    //

    closeHandle = FALSE;

    //
    // Initialize for all instances disabled.
    //

    enabled = FALSE;

    //
    // Open registry ServiceKeyName\Enum branch.
    //

    if (!ARGUMENT_PRESENT(ServiceHandle)) {

        status = PipOpenServiceEnumKeys(ServiceKeyName,
                                        KEY_READ,
                                        &ServiceHandle,
                                        &serviceEnumHandle,
                                        FALSE
                                        );
        if (NT_SUCCESS(status)) {

            closeHandle = TRUE;
        }

    } else {

        PiWstrToUnicodeString(&unicodeName, REGSTR_KEY_ENUM);
        status = IopOpenRegistryKeyEx(&serviceEnumHandle,
                                      ServiceHandle,
                                      &unicodeName,
                                      KEY_READ
                                      );
    }
    if (!NT_SUCCESS(status)) {

        //
        // No Service Enum key? no device instance.  Return FALSE.
        //

        goto exit;
    }

    //
    // Find out how many device instances listed in the ServiceName's Enum key.
    //

    count = 0;
    status = IopGetRegistryValue(serviceEnumHandle,
                                 REGSTR_VALUE_COUNT,
                                 0,
                                 &keyValueInformation
                                 );
    if (NT_SUCCESS(status)) {

        if ((keyValueInformation->Type == REG_DWORD) &&
            (keyValueInformation->DataLength >= sizeof(ULONG))) {

            count = *(PULONG)KEY_VALUE_DATA(keyValueInformation);
        }

        ExFreePool(keyValueInformation);
    }
    ZwClose(serviceEnumHandle);

    if (count == 0) {

        goto exit;
    }

    //
    // Walk through each registered device instance to check if it is enabled.
    //

    for (i = 0; i < count; i++) {

        //
        // Get device instance handle.  If it fails, we will skip this device
        // instance.
        //

        status = PipServiceInstanceToDeviceInstance(ServiceHandle,
                                                    NULL,
                                                    i,
                                                    &instancePath,
                                                    &handle,
                                                    KEY_ALL_ACCESS
                                                    );
        if (!NT_SUCCESS(status)) {
            continue;
        }

        instanceEnabled = PnpIsDeviceInstanceEnabled(handle,
                                                     &instancePath,
                                                     FALSE);

        ExFreePool(instancePath.Buffer);

        ZwClose(handle);

        if (instanceEnabled) {

            //
            // Mark that the driver has at least one device instance to work
            // with.
            //

            enabled = TRUE;
            break;
        }
    }

exit:

    if (closeHandle) {

        ZwClose(ServiceHandle);
    }

    return enabled;
}

LOGICAL
PnpIsDeviceInstanceEnabled(
    __in_opt HANDLE DeviceInstanceHandle,
    __in PUNICODE_STRING DeviceInstance,
    __in LOGICAL Disable
    )

/*++

Routine Description:

    This routine checks if the specified devices instances is enabled.

Arguments:

    DeviceInstanceHandle - Optionally supplies a handle to the device instance
        key to be checked.

    DeviceInstance - Specifies the device instance key unicode name.  Caller
        must at least specified DeviceInstanceHandle or DeviceInstance.

    Disable - If this flag is set, and the device should be disabled
        but is currently disabled, then the device is disabled.

Returns:

    A BOOLEAN value.

--*/

{
    LOGICAL closeHandle;
    ULONG deviceFlags;
    PWSTR deviceInstancePath;
    PDEVICE_NODE deviceNode;
    PDEVICE_OBJECT deviceObject;
    LOGICAL enabled;
    ULONG propertySize;
    ULONG propertyType;
    NTSTATUS status;

    PAGED_CODE();

    //
    // Initialize for proper cleanup.
    //
    deviceObject = NULL;
    closeHandle = FALSE;
    deviceInstancePath = NULL;

    //
    // Assume device is enabled.
    //
    enabled = TRUE;

    //
    // First check if the device node is already disabled.
    //
    deviceObject = PnpDeviceObjectFromDeviceInstance(DeviceInstance);
    deviceNode = PP_DO_TO_DN(deviceObject);
    if (deviceNode) {

        if (    PipIsDevNodeProblem(deviceNode, CM_PROB_DISABLED) ||
                PipIsDevNodeProblem(deviceNode, CM_PROB_HARDWARE_DISABLED)) {

            enabled = FALSE;
            goto exit;
        }
    }

    status = PnpUnicodeStringToWstr(&deviceInstancePath, NULL, DeviceInstance);

    if (!NT_SUCCESS(status)) {
        enabled = FALSE;
        goto exit;
    }

    //
    // Open the device instance key if not specified.
    //
    if (!ARGUMENT_PRESENT(DeviceInstanceHandle)) {

        status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                     deviceInstancePath,
                                     CM_KEY_TYPE_DEVICE_INSTANCE,
                                     0,
                                     KEY_READ,
                                     FALSE,
                                     &DeviceInstanceHandle,
                                     NULL);

        //
        // If we cannot open the device instance key
        //
        if (!NT_SUCCESS(status)) {

            enabled = FALSE;
            goto exit;
        }

        //
        // Remember to close the key since we opened it.
        //
        closeHandle = TRUE;
    }

    //
    // First check if the device has been disabled by the global CONFIGFLAG.
    //
    deviceFlags = 0;
    propertySize = sizeof(deviceFlags);
    status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                 deviceInstancePath,
                                 DeviceInstanceHandle,
                                 CM_REG_PROP_CONFIGFLAGS,
                                 &propertyType,
                                 (PUCHAR)&deviceFlags,
                                 &propertySize,
                                 0);
    if (NT_SUCCESS(status)) {

        if ( (propertyType != REG_DWORD) ||
             (propertySize != sizeof(deviceFlags)) ) {

            deviceFlags = 0;
        }
    }

    if (deviceFlags & CONFIGFLAG_DISABLED) {

        deviceFlags = CSCONFIGFLAG_DISABLED;
    } else {

        //
        // Get the configflags for this device in the current profile.
        //
        PnpGetDeviceInstanceCsConfigFlags(DeviceInstance, 0, &deviceFlags);
    }

    //
    // Determine if the device should be disabled based on flags.
    //
    if (    (deviceFlags & CSCONFIGFLAG_DISABLED) ||
            (deviceFlags & CSCONFIGFLAG_DO_NOT_CREATE) ||
            (deviceFlags & CSCONFIGFLAG_DO_NOT_START)) {

        enabled = FALSE;
    }

    if (enabled == FALSE) {

        //
        // Device is disabled.
        //
        if (Disable) {

            //
            // Disable if we have an initialized devicenode.
            //
            if (deviceNode && deviceNode->State != DeviceNodeUninitialized) {

                PnpDisableDevice(deviceNode, CM_PROB_DISABLED);
            }
        }
    }

exit:

    //
    // Cleanup.
    //
    if (deviceObject) {

        ObDereferenceObject(deviceObject);
    }
    if (closeHandle) {

        ZwClose(DeviceInstanceHandle);
    }
    if (deviceInstancePath) {

        PnpUnicodeStringToWstrFree(deviceInstancePath, DeviceInstance);
    }

    return enabled;
}

ULONG
PnpDetermineResourceListSize (
    __in_opt PCM_RESOURCE_LIST ResourceList
    )

/*++

Routine Description:

    This routine determines size of the passed in ResourceList
    structure.

Arguments:

    Configuration1 - Supplies a pointer to the resource list.

Return Value:

    size of the resource list structure.

--*/

{
#pragma prefast (suppress: __WARNING_OVERFLOW_OR_UNDERFLOW_IN_ALLOCATOR_SIZE_FUNC_RETVAL, "lists built by trusted sources");
    ULONG totalSize, listSize, descriptorSize, i, j;
    PCM_FULL_RESOURCE_DESCRIPTOR fullResourceDesc;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR partialDescriptor;

    if (!ResourceList) {
        totalSize = 0;

    } else {
        totalSize = FIELD_OFFSET(CM_RESOURCE_LIST, List);
        fullResourceDesc = &ResourceList->List[0];
        for (i = 0; i < ResourceList->Count; i++) {
            listSize = FIELD_OFFSET(CM_FULL_RESOURCE_DESCRIPTOR,
                                    PartialResourceList) +
                       FIELD_OFFSET(CM_PARTIAL_RESOURCE_LIST,
                                    PartialDescriptors);

            partialDescriptor = &fullResourceDesc->PartialResourceList.PartialDescriptors[0];
            for (j = 0; j < fullResourceDesc->PartialResourceList.Count; j++) {
                descriptorSize = sizeof(CM_PARTIAL_RESOURCE_DESCRIPTOR);
                if (partialDescriptor->Type == CmResourceTypeDeviceSpecific) {
                    PNP_ASSERT((descriptorSize +
                               partialDescriptor->u.DeviceSpecificData.DataSize) >
                               descriptorSize);

                    descriptorSize += partialDescriptor->u.DeviceSpecificData.DataSize;
                }

                PNP_ASSERT((listSize + descriptorSize) > listSize);
                listSize += descriptorSize;
                partialDescriptor = (PCM_PARTIAL_RESOURCE_DESCRIPTOR)
                                        ((PUCHAR)partialDescriptor + descriptorSize);
            }

            PNP_ASSERT((totalSize + listSize) > totalSize);
            totalSize += listSize;
            fullResourceDesc = (PCM_FULL_RESOURCE_DESCRIPTOR)
                                      ((PUCHAR)fullResourceDesc + listSize);
        }
    }

    return totalSize;
}

VOID
PnpInitializeDeviceReferenceTable (
    VOID
    )

/*++

Routine Description:

    This routine initializes data structures associated with the device
    reference table.

Arguments:

    None.

Return Value:

    None.

--*/

{
    KeInitializeGuardedMutex(&PnpDeviceReferenceTableLock);
    RtlInitializeGenericTable(  &PnpDeviceReferenceTable,
                                PnpCompareInstancePath,
                                PnpAllocateGenericTableEntry,
                                PnpFreeGenericTableEntry,
                                NULL);
}

RTL_GENERIC_COMPARE_RESULTS
NTAPI
PnpCompareInstancePath(
    __in  PRTL_GENERIC_TABLE          Table,
    __in  PVOID                       FirstStruct,
    __in  PVOID                       SecondStruct
    )

/*++

Routine Description:

    This routine is the callback for the generic table routines.

Arguments:

    Table       - Table for which this is invoked.

    FirstStruct - An element in the table to compare.

    SecondStruct - Another element in the table to compare.

Return Value:

    RTL_GENERIC_COMPARE_RESULTS.

--*/

{
    PUNICODE_STRING lhs = ((PPNP_DEVICE_REFERENCE)FirstStruct)->DeviceInstance;
    PUNICODE_STRING rhs = ((PPNP_DEVICE_REFERENCE)SecondStruct)->DeviceInstance;
    LONG            result;

    PAGED_CODE();

    UNREFERENCED_PARAMETER (Table);

    result = RtlCompareUnicodeString(lhs, rhs, TRUE);
    if (result < 0) {

        return GenericLessThan;
    } else if (result > 0) {

        return GenericGreaterThan;
    }
    return GenericEqual;
}

PVOID
NTAPI
PnpAllocateGenericTableEntry(
    __in  PRTL_GENERIC_TABLE          Table,
    __in  CLONG                       ByteSize
    )

/*++

Routine Description:

    This routine is the callback for allocation for entries in the generic table.

Arguments:

    Table       - Table for which this is invoked.

    ByteSize    - Amount of memory to allocate.

Return Value:

    Pointer to allocated memory if successful, else NULL.

--*/

{
    PAGED_CODE();

    UNREFERENCED_PARAMETER (Table);

    return ExAllocatePool(PagedPool, ByteSize);
}

VOID
NTAPI
PnpFreeGenericTableEntry(
    __in  PRTL_GENERIC_TABLE          Table,
    __in  PVOID                       Buffer
    )

/*++

Routine Description:

    This routine is the callback for releasing memory for entries in the generic
    table.

Arguments:

    Table       - Table for which this is invoked.

    Buffer      - Buffer to free.

Return Value:

    None.

--*/

{
    PAGED_CODE();

    UNREFERENCED_PARAMETER (Table);

    ExFreePool(Buffer);
}

NTSTATUS
PnpMapDeviceObjectToDeviceInstance(
    __in PDEVICE_OBJECT   DeviceObject,
    __in PUNICODE_STRING  DeviceInstance
    )
/*++

Routine Description:

    This routine adds a reference for the specified device to the
    PnpDeviceReferenceTable lookup table.

    Note, caller must own the PnpRegistryDeviceResource before calling the
    function.

Arguments:

    DeviceObject - supplies a pointer to a physical device object.

    DeviceInstance - supplies a UNICODE_STRING to specify the device instance path.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    NTSTATUS    status;
    PNP_DEVICE_REFERENCE deviceReference;

#if DBG

    PDEVICE_OBJECT oldDeviceObject;

#endif

    PAGED_CODE();

#if DBG

    oldDeviceObject = PnpDeviceObjectFromDeviceInstance(DeviceInstance);
    PNP_ASSERT(!oldDeviceObject);
    if (oldDeviceObject) {
        ObDereferenceObject(oldDeviceObject);
    }

#endif

    deviceReference.DeviceObject    = DeviceObject;
    deviceReference.DeviceInstance  = DeviceInstance;
    KeAcquireGuardedMutex(&PnpDeviceReferenceTableLock);
    if (RtlInsertElementGenericTable(&PnpDeviceReferenceTable,
                                     (PVOID)&deviceReference,
                                     (CLONG)sizeof(PNP_DEVICE_REFERENCE),
                                     NULL)) {
        status = STATUS_SUCCESS;
    } else {

        status = STATUS_UNSUCCESSFUL;
    }
    KeReleaseGuardedMutex(&PnpDeviceReferenceTableLock);

    return status;
}

PDEVICE_OBJECT
PnpDeviceObjectFromDeviceInstance(
    __in PUNICODE_STRING  DeviceInstance
    )

/*++

Routine Description:

    This routine receives a DeviceInstance path (or DeviceInstance handle) and
    returns a reference to a bus device object for the DeviceInstance.

    Note, caller must owner the PnRegistryDeviceResource before calling the function,

Arguments:

    DeviceInstance - supplies a UNICODE_STRING to specify the device instance path.

Returns:

    A reference to the desired bus device object.

--*/

{
    PNP_DEVICE_REFERENCE    key;
    PPNP_DEVICE_REFERENCE   deviceReference;
    PDEVICE_OBJECT      deviceObject;
    PDEVICE_NODE        deviceNode;

    PAGED_CODE();
    //
    // Look-up the DO in our table.
    //
    deviceObject        = NULL;
    key.DeviceObject    = NULL;
    key.DeviceInstance  = DeviceInstance;
    KeAcquireGuardedMutex(&PnpDeviceReferenceTableLock);

    deviceReference = RtlLookupElementGenericTable(&PnpDeviceReferenceTable, (PVOID)&key);
    if (deviceReference) {

        deviceObject = deviceReference->DeviceObject;
        PNP_ASSERT(deviceObject);
        if (deviceObject) {

            PNP_ASSERT(deviceObject->Type == IO_TYPE_DEVICE);
            if (deviceObject->Type != IO_TYPE_DEVICE) {

                deviceObject = NULL;
            } else {

                deviceNode = (PDEVICE_NODE)deviceObject->DeviceObjectExtension->DeviceNode;
                PNP_ASSERT(deviceNode && (deviceNode->PhysicalDeviceObject == deviceObject));
                if (!deviceNode || deviceNode->PhysicalDeviceObject != deviceObject) {

                    deviceObject = NULL;
                }
            }
        }
    }
    //
    // Take a reference if we found the device object.
    //
    if (deviceObject) {

        ObReferenceObject(deviceObject);
    }

    KeReleaseGuardedMutex(&PnpDeviceReferenceTableLock);

    return deviceObject;
}

PDEVICE_OBJECT
IopDeviceObjectFromSymbolicName(
    __in PUNICODE_STRING  SymbolicLinkName
    )

/*++

Routine Description:

    This routine receives a symbolic link name of the interface device and
    returns a reference to a bus device object for the symbolic link.

Arguments:

    SymbolicLinkName - The kernel mode symbolic link name of the interface device

Returns:

    A reference to the desired bus device object.

Author:
    SriramSa (July 2005)

--*/

{

    PWSTR deviceInstance = NULL;
    PDEVICE_OBJECT physicalDeviceObject = NULL;
    ULONG propertySize;
    DEVPROPTYPE propertyType;
    NTSTATUS status;
    PWSTR symbolicLink;
    UNICODE_STRING tempString;

    PAGED_CODE();

    //
    // Make sure we have a SymbolicLinkName.
    //

    if ((!ARGUMENT_PRESENT(SymbolicLinkName)) ||
        (SymbolicLinkName->Buffer == NULL)    ||
        (SymbolicLinkName->Length == 0)) {
        status = STATUS_INVALID_PARAMETER;
        goto exit;
    }

    status = PnpUnicodeStringToWstr(&symbolicLink, NULL, SymbolicLinkName);

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Find out the name of the device instance that 'owns' this interface.
    //
    propertySize = MAX_DEVICE_ID_LEN * sizeof(WCHAR);

    deviceInstance = ExAllocatePoolWithTag(PagedPool,
                                           propertySize,
                                           PNP_POOLTAG_GENERIC);

    if (!deviceInstance) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto exit;
    }

    status = _PnpGetObjectProperty(PiPnpRtlCtx,
                                   symbolicLink,
                                   PNP_OBJECT_TYPE_DEVICE_INTERFACE,
                                   NULL,
                                   NULL,
                                   &DEVPKEY_Device_InstanceId,
                                   &propertyType,
                                   (PUCHAR)deviceInstance,
                                   propertySize,
                                   &propertySize,
                                   0);

    PnpUnicodeStringToWstrFree(symbolicLink, SymbolicLinkName);

    if (!NT_SUCCESS(status)) {
        goto exit;
    }

    //
    // Find the PDO corresponding to this device instance name.
    //
    if (propertyType == DEVPROP_TYPE_STRING) {

        status = RtlInitUnicodeStringEx(&tempString, deviceInstance);

        if (NT_SUCCESS(status)) {
            physicalDeviceObject = PnpDeviceObjectFromDeviceInstance(&tempString);
        }
    }

exit:

    if (deviceInstance) {
        ExFreePoolWithTag(deviceInstance, PNP_POOLTAG_GENERIC);
    }

    return physicalDeviceObject;
}

ULONG
IopGetSessionIdFromSymbolicName(
    __in PUNICODE_STRING  SymbolicLinkName
    )

/*++

Routine Description:

    This routine receives a symbolic link name of the interface device and
    returns the Session id of the device. If its a Global Device and is not
    a per-session device, Session 0 (meaning Globally visible) is returned.

Arguments:

    SymbolicLinkName - The kernel mode symbolic link name of the interface device

Returns:

    Session id of the device.

Author:
    SriramSa (July 2005)

--*/

{

    PDEVICE_OBJECT pdo;
    ULONG DeviceSessionId = INVALID_SESSION;

    PAGED_CODE();

    pdo = IopDeviceObjectFromSymbolicName(SymbolicLinkName);

    if (IS_PDO(pdo)) {
        DeviceSessionId = IopGetSessionIdFromPDO(pdo);
        //
        // We dont need PDO anymore, dereference it
        //
        ObDereferenceObject(pdo);
    }

    IopDbgPrint((IOP_TRACE_LEVEL, "IopGetSessionIdFromSymbolicName DeviceSessionId = %d !! \n", DeviceSessionId));

    return DeviceSessionId;
}

ULONG
IopGetSessionIdFromPDO(
    __in PDEVICE_OBJECT pdo
    )

/*++

Routine Description:

    This routine receives a PDO and returns the Session id of the device.
    If its a Global Device and is not a a per-session device, Session 0
    (meaning Globally visible) is returned.

Arguments:

    pdo - Physical Device Object of the device

Returns:

    Session id of the device.

Author:
    SriramSa (July 2005)

--*/

{

    NTSTATUS status;
    ULONG DeviceSessionId = INVALID_SESSION, requiredSize, type;

    PAGED_CODE();

    if (IS_PDO(pdo)) {

        status = IoGetDevicePropertyData(pdo,
                                         &DEVPKEY_Device_SessionId,
                                         0,
                                         0,
                                         sizeof(DWORD),
                                         &DeviceSessionId,
                                         &requiredSize,
                                         &type);

        if (!NT_SUCCESS(status)) {
            //
            // Could not get session id property - device is global
            //
            status = STATUS_SUCCESS;
            DeviceSessionId = INVALID_SESSION;
        }
    }
    IopDbgPrint((IOP_TRACE_LEVEL, "IopGetSessionIdFromPDO - DeviceSessionId = %d !! \n", DeviceSessionId));

    return DeviceSessionId;
}

NTSTATUS
PnpDeviceObjectToDeviceInstance (
    __in PDEVICE_OBJECT DeviceObject,
    __out PHANDLE DeviceInstanceHandle,
    __in  ACCESS_MASK DesiredAccess
    )

/*++

Routine Description:

    This routine receives a DeviceObject pointer and returns a handle to the device
    instance path under registry System\ENUM key.

    Note, caller must owner the PnpRegistryDeviceResource before calling the function,

Arguments:

    DeviceObject - supplies a pointer to a physical device object.

    DeviceInstanceHandle - Supplies a variable to receive the handle to the registry
             device instance key.

    DesiredAccess - specifies the access that is needed to this key.

Returns:

    NTSTATUS code to indicate success or failure.

--*/

{
    NTSTATUS status;
    PDEVICE_NODE deviceNode;

    PAGED_CODE();

    deviceNode = (PDEVICE_NODE) DeviceObject->DeviceObjectExtension->DeviceNode;
    if (deviceNode != NULL && PnpIsDeviceInstancePathEmpty(deviceNode) == FALSE) {
        status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                     PnpGetDeviceInstancePath(deviceNode)->Buffer,
                                     CM_KEY_TYPE_DEVICE_INSTANCE,
                                     0,
                                     DesiredAccess,
                                     FALSE,
                                     DeviceInstanceHandle,
                                     NULL);

    } else {
        status = STATUS_INVALID_DEVICE_REQUEST;
    }

    return status;
}

NTSTATUS
PnpCleanupDeviceRegistryValues (
    __in PUNICODE_STRING InstancePath
    )

/*++

Routine Description:

    This routine cleans up a device instance key when the device is no
    longer present/enumerated.  If the device is registered to a Service
    the Service's enum key will also been cleaned up.

    Note the caller must lock the RegistryDeviceResource

Arguments:

    InstancePath - supplies a pointer to the name of the device instance key.

Return Value:

    status

--*/

{
    PNP_DEVICE_REFERENCE    key;
    NTSTATUS            status;

#if DBG

    PDEVICE_OBJECT      deviceObject;

#endif

    PAGED_CODE();

    //
    // Delete the mapping between this instance path and corresponding DO.
    //
    key.DeviceObject         = NULL;
    key.DeviceInstance       = InstancePath;

    KeAcquireGuardedMutex(&PnpDeviceReferenceTableLock);
    RtlDeleteElementGenericTable(&PnpDeviceReferenceTable, (PVOID)&key);
    KeReleaseGuardedMutex(&PnpDeviceReferenceTableLock);

#if DBG

    deviceObject = PnpDeviceObjectFromDeviceInstance(InstancePath);
    PNP_ASSERT(!deviceObject);
    if (deviceObject) {
        ObDereferenceObject(deviceObject);
    }

#endif

    //
    // Deregister the device from its controlling service's service enum key
    //

    status = PpDeviceRegistration( InstancePath, FALSE, NULL, TRUE);

    return status;
}

NTSTATUS
PnpGetDeviceResourcesFromRegistry (
    __in PDEVICE_OBJECT DeviceObject,
    __in ULONG ResourceType,
    __in ULONG Preference,
    __deref_out_bcount(*Length) PVOID *Resource,
    __out PULONG Length
    )

/*++

Routine Description:

    This routine determines the resources decoded by the device specified.
    If the device object is a madeup device, we will try to read the resources
    from registry.  Otherwise, we need to traverse the internal assigned resource
    list to compose the resource list.

Arguments:

    DeviceObject - supplies a pointer to a device object whose registry
        values are to be cleaned up.

    ResourceType - 0 for CM_RESOURCE_LIST and 1 for IO_RESOURCE_REQUIREMENTS_LIS

    Flags - specify the preference.

    Resource - Specified a variable to receive the required resources.

    Length - Specified a variable to receive the length of the resource structure.

Return Value:

    status

--*/

{
    HANDLE handle;
    NTSTATUS status = STATUS_SUCCESS;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;
    PWCHAR valueName = NULL;

    PAGED_CODE();

    *Resource = NULL;
    *Length = 0;

    //
    // Open the LogConfig key of the device instance.
    //

    if (ResourceType == QUERY_RESOURCE_LIST) {

        //
        // Caller is asking for CM_RESOURCE_LIST
        //

        if (Preference & REGISTRY_ALLOC_CONFIG) {

            //
            // Try alloc config first
            //

            status = _CmOpenDeviceRegKey( PiPnpRtlCtx,
                                          PnpGetDeviceInstancePath(PP_DO_TO_DN(DeviceObject))->Buffer,
                                          CM_KEY_TYPE_DEVICE_CONTROL,
                                          0,
                                          KEY_READ,
                                          FALSE,
                                          &handle,
                                          NULL
                                          );
            if (NT_SUCCESS(status)) {
                status = PnpReadDeviceConfiguration (handle, REGISTRY_ALLOC_CONFIG, (PCM_RESOURCE_LIST *)Resource, Length);
                ZwClose(handle);
                if (NT_SUCCESS(status)) {
                    return status;
                }
            }
        }

        handle = NULL;
        if (Preference & REGISTRY_FORCED_CONFIG) {

            status = _CmOpenDeviceRegKey( PiPnpRtlCtx,
                                          PnpGetDeviceInstancePath(PP_DO_TO_DN(DeviceObject))->Buffer,
                                          CM_KEY_TYPE_DEVICE_LOGCONF,
                                          0,
                                          KEY_READ,
                                          FALSE,
                                          &handle,
                                          NULL
                                          );
            if (NT_SUCCESS(status)) {
                status = PnpReadDeviceConfiguration (handle, REGISTRY_FORCED_CONFIG, (PCM_RESOURCE_LIST *)Resource, Length);
                if (NT_SUCCESS(status)) {
                    ZwClose(handle);
                    return status;
                }
            } else {
                return status;
            }
        }
        if (Preference & REGISTRY_BOOT_CONFIG) {

            //
            // Try alloc config first
            //

            if (handle == NULL) {
                status = _CmOpenDeviceRegKey( PiPnpRtlCtx,
                                              PnpGetDeviceInstancePath(PP_DO_TO_DN(DeviceObject))->Buffer,
                                              CM_KEY_TYPE_DEVICE_LOGCONF,
                                              0,
                                              KEY_READ,
                                              FALSE,
                                              &handle,
                                              NULL
                                              );
                if (!NT_SUCCESS(status)) {
                    return status;
                }
            }
            status = PnpReadDeviceConfiguration( handle,
                                                 REGISTRY_BOOT_CONFIG,
                                                 (PCM_RESOURCE_LIST *)Resource,
                                                 Length);
        }
        if (handle) {
            ZwClose(handle);
        }
    } else {

        status = _CmOpenDeviceRegKey( PiPnpRtlCtx,
                                      PnpGetDeviceInstancePath(PP_DO_TO_DN(DeviceObject))->Buffer,
                                      CM_KEY_TYPE_DEVICE_LOGCONF,
                                      0,
                                      KEY_READ,
                                      FALSE,
                                      &handle,
                                      NULL
                                      );
        if (NT_SUCCESS(status)) {

            if (Preference & REGISTRY_OVERRIDE_CONFIGVECTOR) {
                valueName = REGSTR_VALUE_OVERRIDE_CONFIG_VECTOR;
            } else if (Preference & REGISTRY_BASIC_CONFIGVECTOR) {
                valueName = REGSTR_VALUE_BASIC_CONFIG_VECTOR;
            }
            if (valueName) {

                //
                // Try to read device's configuration vector
                //

                status = IopGetRegistryValue (handle,
                                              valueName,
                                              0,
                                              &keyValueInformation);
                if (NT_SUCCESS(status)) {

                    //
                    // Try to read what caller wants.
                    //

                    if ((keyValueInformation->Type == REG_RESOURCE_REQUIREMENTS_LIST) &&
                        (keyValueInformation->DataLength != 0)) {

                        *Resource = ExAllocatePool(PagedPool,
                                                   keyValueInformation->DataLength);
                        if (*Resource) {
                            PIO_RESOURCE_REQUIREMENTS_LIST ioResource;

                            *Length = keyValueInformation->DataLength;
                            RtlCopyMemory(*Resource,
                                          KEY_VALUE_DATA(keyValueInformation),
                                          keyValueInformation->DataLength);

                            //
                            // Process the io resource requirements list to change undefined
                            // interface type to our default type.
                            //

                            ioResource = *Resource;
                            if (ioResource->InterfaceType == InterfaceTypeUndefined) {
                                ioResource->BusNumber = 0;
                                ioResource->InterfaceType = PnpDefaultInterfaceType;
                            }
                        } else {
                            status = STATUS_INVALID_PARAMETER_2;
                        }
                    }
                    ExFreePool(keyValueInformation);
                }
            }
            ZwClose(handle);
        }
    }
    return status;
}

NTSTATUS
PnpReadDeviceConfiguration (
    __in HANDLE Handle,
    __in ULONG Flags,
    __deref_out_bcount(*Length) PCM_RESOURCE_LIST *CmResource,
    __out PULONG Length
    )

/*++

Routine Description:

    This routine read the specified ALLOC config or ForcedConfig or Boot config.

Arguments:

    Hanle - supplies a handle to the registry key to read resources.

Return Value:

    status

--*/

{
    NTSTATUS status;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;
    PWCHAR valueName;
    PCM_RESOURCE_LIST resourceList;
    PCM_FULL_RESOURCE_DESCRIPTOR cmFullDesc;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR cmPartDesc;
    ULONG j, k, size;

    PAGED_CODE();

    *CmResource = NULL;
    *Length = 0;

    switch (Flags) {

        case REGISTRY_ALLOC_CONFIG:
            valueName = REGSTR_VALUE_ALLOC_CONFIG;
            break;

        case REGISTRY_FORCED_CONFIG:
            valueName = REGSTR_VALUE_FORCED_CONFIG;
            break;

        case REGISTRY_BOOT_CONFIG:
            valueName = REGSTR_VALUE_BOOT_CONFIG;
            break;

        default:
            return STATUS_INVALID_PARAMETER_2;
    }

    //
    // Read the registry value of the desired value name
    //
    status = IopGetRegistryValue(Handle,
                                 valueName,
                                 0,
                                 &keyValueInformation
                                 );
    if (NT_SUCCESS(status)) {

        if (    keyValueInformation->Type == REG_RESOURCE_LIST &&
                keyValueInformation->DataLength != 0) {

            *CmResource = ExAllocatePool(PagedPool,
                                         keyValueInformation->DataLength
                                         );
            if (*CmResource) {

                *Length = keyValueInformation->DataLength;
                RtlCopyMemory(*CmResource,
                              KEY_VALUE_DATA(keyValueInformation),
                              keyValueInformation->DataLength
                              );
                //
                // Process the resource list read from Registry to change undefined
                // interface type to our default interface type.
                //
                resourceList = *CmResource;
                cmFullDesc = &resourceList->List[0];
                for (j = 0; j < resourceList->Count; j++) {

                    if (cmFullDesc->InterfaceType == InterfaceTypeUndefined) {

                        cmFullDesc->BusNumber = 0;
                        cmFullDesc->InterfaceType = PnpDefaultInterfaceType;
                    }

                    cmPartDesc = &cmFullDesc->PartialResourceList.PartialDescriptors[0];
                    for (k = 0; k < cmFullDesc->PartialResourceList.Count; k++) {

                        size = 0;
                        switch (cmPartDesc->Type) {
                        case CmResourceTypeDeviceSpecific:
                             size = cmPartDesc->u.DeviceSpecificData.DataSize;
                             break;
                        }
                        cmPartDesc++;
                        cmPartDesc = (PCM_PARTIAL_RESOURCE_DESCRIPTOR) ((PUCHAR)cmPartDesc + size);
                    }
                    cmFullDesc = (PCM_FULL_RESOURCE_DESCRIPTOR)cmPartDesc;
                }
            } else {

                status = STATUS_INSUFFICIENT_RESOURCES;
            }

        } else if (keyValueInformation->Type != REG_RESOURCE_LIST) {

            status = STATUS_UNSUCCESSFUL;
        }

        ExFreePool(keyValueInformation);
    }

    return status;
}

PIO_RESOURCE_REQUIREMENTS_LIST
PnpCmResourcesToIoResources(
    __in ULONG SlotNumber,
    __in PCM_RESOURCE_LIST CmResourceList,
    __in ULONG Priority
    )

/*++

Routine Description:

    This routines converts the input CmResourceList to IO_RESOURCE_REQUIREMENTS_LIST.

Arguments:

    SlotNumber - supplies the SlotNumber the resources refer to.

    CmResourceList - the cm resource list to convert.

    Priority - specifies the priority of the logconfig

Return Value:

    returns a IO_RESOURCE_REQUIREMENTS_LISTST if succeeds.  Otherwise a NULL value is
    returned.

--*/
{
    PIO_RESOURCE_REQUIREMENTS_LIST ioResReqList;
    ULONG count = 0, size, i, j;
    PCM_FULL_RESOURCE_DESCRIPTOR cmFullDesc;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR cmPartDesc;
    PIO_RESOURCE_DESCRIPTOR ioDesc;

    PAGED_CODE();

    //
    // First determine number of descriptors required.
    //
    cmFullDesc = &CmResourceList->List[0];
    for (i = 0; i < CmResourceList->Count; i++) {
        count += cmFullDesc->PartialResourceList.Count;
        cmPartDesc = &cmFullDesc->PartialResourceList.PartialDescriptors[0];
        for (j = 0; j < cmFullDesc->PartialResourceList.Count; j++) {
            size = 0;
            switch (cmPartDesc->Type) {
            case CmResourceTypeDeviceSpecific:
                 size = cmPartDesc->u.DeviceSpecificData.DataSize;
                 count--;
                 break;
            }
            cmPartDesc++;
            cmPartDesc = (PCM_PARTIAL_RESOURCE_DESCRIPTOR) ((PUCHAR)cmPartDesc + size);
        }
        cmFullDesc = (PCM_FULL_RESOURCE_DESCRIPTOR)cmPartDesc;
    }

    if (count == 0) {
        return NULL;
    }

    //
    // Count the extra descriptors for InterfaceType and BusNumber information.
    //

    count += CmResourceList->Count - 1;

    //
    // Allocate heap space for IO RESOURCE REQUIREMENTS LIST
    //

    count++;           // add one for CmResourceTypeConfigData
    ioResReqList = (PIO_RESOURCE_REQUIREMENTS_LIST)ExAllocatePool(
                       PagedPool,
                       sizeof(IO_RESOURCE_REQUIREMENTS_LIST) +
                           count * sizeof(IO_RESOURCE_DESCRIPTOR)
                       );
    if (!ioResReqList) {
        return NULL;
    }

    //
    // Parse the cm resource descriptor and build its corresponding IO resource descriptor
    //

    ioResReqList->InterfaceType = CmResourceList->List[0].InterfaceType;
    ioResReqList->BusNumber = CmResourceList->List[0].BusNumber;
    ioResReqList->SlotNumber = SlotNumber;
    ioResReqList->Reserved[0] = 0;
    ioResReqList->Reserved[1] = 0;
    ioResReqList->Reserved[2] = 0;
    ioResReqList->AlternativeLists = 1;
    ioResReqList->List[0].Version = 1;
    ioResReqList->List[0].Revision = 1;
    ioResReqList->List[0].Count = count;

    //
    // Generate a CmResourceTypeConfigData descriptor
    //

    ioDesc = &ioResReqList->List[0].Descriptors[0];
    ioDesc->Option = IO_RESOURCE_PREFERRED;
    ioDesc->Type = CmResourceTypeConfigData;
    ioDesc->ShareDisposition = CmResourceShareShared;
    ioDesc->Flags = 0;
    ioDesc->Spare1 = 0;
    ioDesc->Spare2 = 0;
    ioDesc->u.ConfigData.Priority = Priority;
    ioDesc++;

    cmFullDesc = &CmResourceList->List[0];
    for (i = 0; i < CmResourceList->Count; i++) {
        if (i != 0) {

            //
            // Set up descriptor to remember the InterfaceType and BusNumber.
            //

            ioDesc->Option = IO_RESOURCE_PREFERRED;
            ioDesc->Type = CmResourceTypeReserved;
            ioDesc->ShareDisposition = CmResourceShareUndetermined;
            ioDesc->Flags = 0;
            ioDesc->Spare1 = 0;
            ioDesc->Spare2 = 0;
            if (cmFullDesc->InterfaceType == InterfaceTypeUndefined) {
                ioDesc->u.DevicePrivate.Data[0] = PnpDefaultInterfaceType;
            } else {
                ioDesc->u.DevicePrivate.Data[0] = cmFullDesc->InterfaceType;
            }
            ioDesc->u.DevicePrivate.Data[1] = cmFullDesc->BusNumber;
            ioDesc->u.DevicePrivate.Data[2] = 0;
            ioDesc++;
        }
        cmPartDesc = &cmFullDesc->PartialResourceList.PartialDescriptors[0];
        for (j = 0; j < cmFullDesc->PartialResourceList.Count; j++) {
            ioDesc->Option = IO_RESOURCE_PREFERRED;
            ioDesc->Type = cmPartDesc->Type;
            ioDesc->ShareDisposition = cmPartDesc->ShareDisposition;
            ioDesc->Flags = cmPartDesc->Flags;
            ioDesc->Spare1 = 0;
            ioDesc->Spare2 = 0;

            size = 0;
            switch (cmPartDesc->Type) {
            case CmResourceTypeInterrupt:
                if ((cmPartDesc->Flags & CM_RESOURCE_INTERRUPT_MESSAGE) == 0) {
#if defined(_X86_)
                    ioDesc->u.Interrupt.MinimumVector =
                        ioDesc->u.Interrupt.MaximumVector =
                            cmPartDesc->u.Interrupt.Level;
#else
                    ioDesc->u.Interrupt.MinimumVector =
                        ioDesc->u.Interrupt.MaximumVector =
                            cmPartDesc->u.Interrupt.Vector;
#endif
                } else {
                    ioDesc->u.Interrupt.MaximumVector =
                        CM_RESOURCE_INTERRUPT_MESSAGE_TOKEN;

                    ioDesc->u.Interrupt.MinimumVector =
                        CM_RESOURCE_INTERRUPT_MESSAGE_TOKEN -
                            cmPartDesc->u.MessageInterrupt.Raw.MessageCount + 1;

                    //
                    // Grab the policy info from resource descriptor and place
                    // it in the requirement descriptor. The policy info stems
                    // from the processor affinity set and remaining elements
                    // can be derived from it and so assign default values to
                    // them.
                    //

                    ioDesc->u.Interrupt.AffinityPolicy =
                        IrqPolicySpecifiedProcessors;

                    ioDesc->u.Interrupt.PriorityPolicy =
                        IrqPriorityUndefined;

                    ioDesc->u.Interrupt.Group =
                        cmPartDesc->u.MessageInterrupt.Raw.Group;

                    ioDesc->u.Interrupt.TargetedProcessors =
                        cmPartDesc->u.MessageInterrupt.Raw.Affinity;
                }

                ioDesc++;
                break;
            case CmResourceTypePort:
            case CmResourceTypeMemory:
            case CmResourceTypeMemoryLarge: {
                 ULONGLONG Length;
                 ULONGLONG Start;
                 Length = RtlCmDecodeMemIoResource(cmPartDesc, &Start);
                 ioDesc->u.Generic.Length = cmPartDesc->u.Generic.Length;
                 ioDesc->u.Generic.Alignment = 1;
                 ioDesc->u.Generic.MinimumAddress.QuadPart = Start;
                 ioDesc->u.Generic.MaximumAddress.QuadPart = Start + Length - 1;
                 ioDesc++;
                 break;
            }
            case CmResourceTypeDma:
                 if ((cmPartDesc->Flags & CM_RESOURCE_DMA_V3) != 0) {

                    //
                    // To make the arbiter think that Request lines are unique,
                    // the channel and request line are flipped in DMAv3.
                    // This means that the IO_DESCRIPTOR has a request line,
                    // which alligns to the Channel of a CM_RESOURCE.
                    //

                    ioDesc->u.DmaV3.RequestLine =
                                                cmPartDesc->u.DmaV3.RequestLine;
                    ioDesc->u.DmaV3.Reserved = cmPartDesc->u.DmaV3.RequestLine;
                    ioDesc->u.DmaV3.Channel = cmPartDesc->u.DmaV3.Channel;
                    ioDesc->u.DmaV3.TransferWidth =
                                              cmPartDesc->u.DmaV3.TransferWidth;

                 } else {
                     ioDesc->u.Dma.MinimumChannel = cmPartDesc->u.Dma.Channel;
                     ioDesc->u.Dma.MaximumChannel = cmPartDesc->u.Dma.Channel;
                 }

                 ioDesc++;
                 break;
            case CmResourceTypeDeviceSpecific:
                 size = cmPartDesc->u.DeviceSpecificData.DataSize;
                 break;
            case CmResourceTypeBusNumber:
                 ioDesc->u.BusNumber.MinBusNumber = cmPartDesc->u.BusNumber.Start;
                 ioDesc->u.BusNumber.MaxBusNumber = cmPartDesc->u.BusNumber.Start +
                                                    cmPartDesc->u.BusNumber.Length - 1;
                 ioDesc->u.BusNumber.Length = cmPartDesc->u.BusNumber.Length;
                 ioDesc++;
                 break;
            default:
                 ioDesc->u.DevicePrivate.Data[0] = cmPartDesc->u.DevicePrivate.Data[0];
                 ioDesc->u.DevicePrivate.Data[1] = cmPartDesc->u.DevicePrivate.Data[1];
                 ioDesc->u.DevicePrivate.Data[2] = cmPartDesc->u.DevicePrivate.Data[2];
                 ioDesc++;
                 break;
            }
            cmPartDesc++;
            cmPartDesc = (PCM_PARTIAL_RESOURCE_DESCRIPTOR) ((PUCHAR)cmPartDesc + size);
        }
        cmFullDesc = (PCM_FULL_RESOURCE_DESCRIPTOR)cmPartDesc;
    }
    ioResReqList->ListSize = (ULONG)((ULONG_PTR)ioDesc - (ULONG_PTR)ioResReqList);
    return ioResReqList;
}

NTSTATUS
PnpFilterResourceRequirementsList(
    __in PIO_RESOURCE_REQUIREMENTS_LIST IoList,
    __in PCM_RESOURCE_LIST CmList,
    __inout PIO_RESOURCE_REQUIREMENTS_LIST *FilteredList,
    __out PLOGICAL ExactMatch
    )

/*++

Routine Description:

    This routines adjusts the input IoList based on input BootConfig.


Arguments:

    IoList - supplies the pointer to an IoResourceRequirementsList

    CmList - supplies the pointer to a BootConfig.

    FilteredList - Supplies a variable to receive the filtered resource
             requirements list.

Return Value:

    A NTSTATUS code to indicate the result of the function.

--*/
{
    PIO_RESOURCE_REQUIREMENTS_LIST ioList, newList;
    PIO_RESOURCE_LIST ioResourceList, newIoResourceList, selectedResourceList = NULL;
    PIO_RESOURCE_DESCRIPTOR ioResourceDescriptor, ioResourceDescriptorEnd;
    PIO_RESOURCE_DESCRIPTOR newIoResourceDescriptor, configDataDescriptor;
    LONG ioResourceDescriptorCount = 0;
    USHORT version;
    PCM_FULL_RESOURCE_DESCRIPTOR cmFullDesc;
    PCM_PARTIAL_RESOURCE_DESCRIPTOR cmDescriptor;
    ULONG cmDescriptorCount = 0;
    ULONG size, i, j, oldCount, phase, nullTypeCount;
    LONG k, alternativeLists;
    LOGICAL exactMatch;

    PAGED_CODE();

    *FilteredList = NULL;
    *ExactMatch = FALSE;

    //
    // Make sure there is some resource requirements to be filtered.
    // If no, we will convert CmList/BootConfig to an IoResourceRequirementsList
    //

    if (IoList == NULL || IoList->AlternativeLists == 0) {
        if (CmList && CmList->Count != 0) {
            *FilteredList = PnpCmResourcesToIoResources(0,
                                                        CmList,
                                                        LCPRI_BOOTCONFIG);
        }

        return STATUS_SUCCESS;
    }

    //
    // Make a copy of the Io Resource Requirements List
    //

    ioList = (PIO_RESOURCE_REQUIREMENTS_LIST) ExAllocatePool(PagedPool, IoList->ListSize);
    if (ioList == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    RtlCopyMemory(ioList, IoList, IoList->ListSize);

    //
    // If there is no BootConfig, simply return the copy of the input Io list.
    //

    if (CmList == NULL || CmList->Count == 0) {
        *FilteredList = ioList;
        return STATUS_SUCCESS;
    }

    //
    // First determine minimum number of descriptors required.
    //

    cmFullDesc = &CmList->List[0];
    for (i = 0; i < CmList->Count; i++) {
        cmDescriptorCount += cmFullDesc->PartialResourceList.Count;
        cmDescriptor = &cmFullDesc->PartialResourceList.PartialDescriptors[0];
        for (j = 0; j < cmFullDesc->PartialResourceList.Count; j++) {
            size = 0;
            switch (cmDescriptor->Type) {
            case CmResourceTypeConfigData:
            case CmResourceTypeDevicePrivate:
                 cmDescriptorCount--;
                 break;
            case CmResourceTypeDeviceSpecific:
                 size = cmDescriptor->u.DeviceSpecificData.DataSize;
                 cmDescriptorCount--;
                 break;
            default:

                 //
                 // Invalid cmresource list.  Ignore it and use io resources
                 //

                 if (cmDescriptor->Type == CmResourceTypeNull ||
                     cmDescriptor->Type >= CmResourceTypeMaximum) {
                     cmDescriptorCount--;
                 }
            }
            cmDescriptor++;
            cmDescriptor = (PCM_PARTIAL_RESOURCE_DESCRIPTOR) ((PUCHAR)cmDescriptor + size);
        }
        cmFullDesc = (PCM_FULL_RESOURCE_DESCRIPTOR)cmDescriptor;
    }

    if (cmDescriptorCount == 0) {
        *FilteredList = ioList;
        return STATUS_SUCCESS;
    }

    //
    // cmDescriptorCount is the number of BootConfig Descriptors needs.
    //
    // For each IO list Alternative ...
    //
    nullTypeCount = 0;
    ioResourceList = ioList->List;
    k = ioList->AlternativeLists;
    while (--k >= 0) {
        ioResourceDescriptor = ioResourceList->Descriptors;
        ioResourceDescriptorEnd = ioResourceDescriptor + ioResourceList->Count;
        while (ioResourceDescriptor < ioResourceDescriptorEnd) {

            //
            // Count the original CmResourceTypeNull descriptors so they can be removed
            // while building the filtered list.
            //
            if (ioResourceDescriptor->Type == CmResourceTypeNull) {

                nullTypeCount++;
            }

            ioResourceDescriptor->Spare1 = 0;
            ioResourceDescriptor++;
        }
        ioResourceList = (PIO_RESOURCE_LIST) ioResourceDescriptorEnd;
    }

    ioResourceList = ioList->List;
    k = alternativeLists = ioList->AlternativeLists;
    while (--k >= 0) {
        version = ioResourceList->Version;
        if (version == 0xffff) {  // Convert bogus version to valid number
            version = 1;
        }

        //
        // We use Version field to store number of BootConfig found.
        // Count field to store new number of descriptor in the alternative list.
        //

        ioResourceList->Version = 0;
        oldCount = ioResourceList->Count;

        ioResourceDescriptor = ioResourceList->Descriptors;
        ioResourceDescriptorEnd = ioResourceDescriptor + ioResourceList->Count;

        if (ioResourceDescriptor == ioResourceDescriptorEnd) {

            //
            // An alternative list with zero descriptor count
            //

            ioResourceList->Version = 0xffff;  // Mark it as invalid
            ioList->AlternativeLists--;
            continue;
        }

        exactMatch = TRUE;

        //
        // For each Cm Resource descriptor ... except DevicePrivate and
        // DeviceSpecific...
        //

        cmFullDesc = &CmList->List[0];
        for (i = 0; i < CmList->Count; i++) {
            cmDescriptor = &cmFullDesc->PartialResourceList.PartialDescriptors[0];
            for (j = 0; j < cmFullDesc->PartialResourceList.Count; j++) {
                size = 0;
                switch (cmDescriptor->Type) {
                case CmResourceTypeDevicePrivate:
                     break;
                case CmResourceTypeDeviceSpecific:
                     size = cmDescriptor->u.DeviceSpecificData.DataSize;
                     break;
                default:
                    if (cmDescriptor->Type == CmResourceTypeNull ||
                        cmDescriptor->Type >= CmResourceTypeMaximum) {
                        break;
                    }

                    //
                    // Check CmDescriptor against current Io Alternative list
                    //

                    for (phase = 0; phase < 2; phase++) {
                        ioResourceDescriptor = ioResourceList->Descriptors;
                        while (ioResourceDescriptor < ioResourceDescriptorEnd) {
                            if ((ioResourceDescriptor->Type == cmDescriptor->Type) &&
                                (ioResourceDescriptor->Spare1 == 0)) {
                                ULONGLONG min1, max1, min2, max2;
                                ULONGLONG len1 = 1, len2 = 1, align1, align2;
                                UCHAR share1, share2;

                                share2 = ioResourceDescriptor->ShareDisposition;
                                share1 = cmDescriptor->ShareDisposition;
                                if ((share1 == CmResourceShareUndetermined) ||
                                    (share1 > CmResourceShareShared)) {
                                    share1 = share2;
                                }
                                if ((share2 == CmResourceShareUndetermined) ||
                                    (share2 > CmResourceShareShared)) {
                                    share2 = share1;
                                }
                                align1 = align2 = 1;

                                switch (cmDescriptor->Type) {
                                case CmResourceTypePort:
                                case CmResourceTypeMemory:
                                case CmResourceTypeMemoryLarge:
                                    len1 = RtlCmDecodeMemIoResource(cmDescriptor, &min1);
                                    max1 = min1 + len1 - 1;
                                    len2 = RtlIoDecodeMemIoResource(ioResourceDescriptor,
                                                                     &align2,
                                                                     &min2,
                                                                     &max2);
                                    break;
                                case CmResourceTypeInterrupt:
                                    max1 = min1 = cmDescriptor->u.Interrupt.Vector;
                                    min2 = ioResourceDescriptor->u.Interrupt.MinimumVector;
                                    max2 = ioResourceDescriptor->u.Interrupt.MaximumVector;
                                    break;
                                case CmResourceTypeDma:
                                    min1 = max1 =cmDescriptor->u.Dma.Channel;
                                    min2 = ioResourceDescriptor->u.Dma.MinimumChannel;
                                    max2 = ioResourceDescriptor->u.Dma.MaximumChannel;
                                    break;
                                case CmResourceTypeBusNumber:
                                    min1 = cmDescriptor->u.BusNumber.Start;
                                    max1 = cmDescriptor->u.BusNumber.Start + cmDescriptor->u.BusNumber.Length - 1;
                                    len1 = cmDescriptor->u.BusNumber.Length;
                                    min2 = ioResourceDescriptor->u.BusNumber.MinBusNumber;
                                    max2 = ioResourceDescriptor->u.BusNumber.MaxBusNumber;
                                    len2 = ioResourceDescriptor->u.BusNumber.Length;
                                    break;
                                default:
                                    PNP_ASSERT(0);
                                    max1 = max2 = min1 = min2 = 0;
                                    break;
                                }
                                if (phase == 0) {
                                    if (share1 == share2 && min2 == min1 && max2 >= max1 && len2 >= len1) {

                                        //
                                        // For phase 0 match, we want near exact match...
                                        //

                                        if (max2 != max1) {
                                            exactMatch = FALSE;
                                        }
                                        ioResourceList->Version++;
                                        ioResourceDescriptor->Spare1 = 0x80;
                                        if (ioResourceDescriptor->Option & IO_RESOURCE_ALTERNATIVE) {
                                            PIO_RESOURCE_DESCRIPTOR ioDesc;

                                            ioDesc = ioResourceDescriptor;
                                            ioDesc--;
                                            while (ioDesc >= ioResourceList->Descriptors) {
                                                ioDesc->Type = CmResourceTypeNull;
                                                ioResourceList->Count--;
                                                if (ioDesc->Option == IO_RESOURCE_ALTERNATIVE) {
                                                    ioDesc--;
                                                } else {
                                                    break;
                                                }
                                            }
                                        }
                                        ioResourceDescriptor->Option = IO_RESOURCE_PREFERRED;
                                        ioResourceDescriptor->Flags = cmDescriptor->Flags;
                                        if (ioResourceDescriptor->Type == CmResourceTypePort ||
                                            ioResourceDescriptor->Type == CmResourceTypeMemory ||
                                            ioResourceDescriptor->Type == CmResourceTypeMemoryLarge) {
                                            ioResourceDescriptor->u.Port.MinimumAddress.QuadPart = min1;
                                            ioResourceDescriptor->u.Port.MaximumAddress.QuadPart = min1 + len2 - 1;
                                            ioResourceDescriptor->u.Port.Alignment = 1;
                                        } else if (ioResourceDescriptor->Type == CmResourceTypeBusNumber) {
                                            ioResourceDescriptor->u.BusNumber.MinBusNumber = (ULONG)min1;
                                            ioResourceDescriptor->u.BusNumber.MaxBusNumber = (ULONG)(min1 + len2 - 1);
                                        }
                                        ioResourceDescriptor++;
                                        while (ioResourceDescriptor < ioResourceDescriptorEnd) {
                                            if (ioResourceDescriptor->Option & IO_RESOURCE_ALTERNATIVE) {
                                                ioResourceDescriptor->Type = CmResourceTypeNull;
                                                ioResourceDescriptor++;
                                                ioResourceList->Count--;
                                            } else {
                                                break;
                                            }
                                        }
                                        phase = 1;   // skip phase 1
                                        break;
                                    } else {
                                        ioResourceDescriptor++;
                                    }
                                } else {
                                    exactMatch = FALSE;
                                    if (share1 == share2 && min2 <= min1 && max2 >= max1 && len2 >= len1 &&
                                        (min1 & (align2 - 1)) == 0) {

                                        //
                                        // Io range covers Cm range ... Change the Io range to what is specified
                                        // in BootConfig.
                                        //
                                        //

                                        switch (cmDescriptor->Type) {
                                        case CmResourceTypePort:
                                        case CmResourceTypeMemory:
                                        case CmResourceTypeMemoryLarge:
                                            ioResourceDescriptor->u.Port.MinimumAddress.QuadPart = min1;
                                            ioResourceDescriptor->u.Port.MaximumAddress.QuadPart = min1 + len2 - 1;
                                            break;
                                        case CmResourceTypeInterrupt:
                                        case CmResourceTypeDma:
                                            ioResourceDescriptor->u.Interrupt.MinimumVector = (ULONG)min1;
                                            ioResourceDescriptor->u.Interrupt.MaximumVector = (ULONG)max1;
                                            break;
                                        case CmResourceTypeBusNumber:
                                            ioResourceDescriptor->u.BusNumber.MinBusNumber = (ULONG)min1;
                                            ioResourceDescriptor->u.BusNumber.MaxBusNumber = (ULONG)(min1 + len2 - 1);
                                            break;
                                        }
                                        ioResourceList->Version++;
                                        ioResourceDescriptor->Spare1 = 0x80;
                                        ioResourceDescriptor->Flags = cmDescriptor->Flags;
                                        if (ioResourceDescriptor->Option & IO_RESOURCE_ALTERNATIVE) {
                                            PIO_RESOURCE_DESCRIPTOR ioDesc;

                                            ioDesc = ioResourceDescriptor;
                                            ioDesc--;
                                            while (ioDesc >= ioResourceList->Descriptors) {
                                                ioDesc->Type = CmResourceTypeNull;
                                                ioResourceList->Count--;
                                                if (ioDesc->Option == IO_RESOURCE_ALTERNATIVE) {
                                                    ioDesc--;
                                                } else {
                                                    break;
                                                }
                                            }
                                        }
                                        ioResourceDescriptor->Option = IO_RESOURCE_PREFERRED;
                                        ioResourceDescriptor++;
                                        while (ioResourceDescriptor < ioResourceDescriptorEnd) {
                                            if (ioResourceDescriptor->Option & IO_RESOURCE_ALTERNATIVE) {
                                                ioResourceDescriptor->Type = CmResourceTypeNull;
                                                ioResourceList->Count--;
                                                ioResourceDescriptor++;
                                            } else {
                                                break;
                                            }
                                        }
                                        break;
                                    } else {
                                        ioResourceDescriptor++;
                                    }
                                }
                            } else {
                                ioResourceDescriptor++;
                            }
                        } // Don't add any instruction after this ...
                    } // phase
                } // switch

                //
                // Move to next Cm Descriptor
                //
                cmDescriptor++;
                cmDescriptor = (PCM_PARTIAL_RESOURCE_DESCRIPTOR) ((PUCHAR)cmDescriptor + size);
            }

            //
            // Move to next Cm List
            //
            cmFullDesc = (PCM_FULL_RESOURCE_DESCRIPTOR)cmDescriptor;
        }

        if (ioResourceList->Version != (USHORT)cmDescriptorCount) {

            //
            // If the current alternative list does not cover all the boot config
            // descriptors, make it as invalid.
            //
            ioResourceList->Version = 0xffff;
            ioList->AlternativeLists--;

        } else {

            if ((ioResourceList->Count == cmDescriptorCount) ||
                (ioResourceList->Count == (cmDescriptorCount + 1) &&
                 ioResourceList->Descriptors[0].Type == CmResourceTypeConfigData)) {

                if (selectedResourceList) {

                    ioResourceList->Version = 0xffff;
                    ioList->AlternativeLists--;

                } else {

                    selectedResourceList = ioResourceList;
                    ioResourceDescriptorCount += ioResourceList->Count;
                    ioResourceList->Version = version;
                    if (exactMatch) {

                        *ExactMatch = TRUE;
                    }
                }

            } else {

                ioResourceDescriptorCount += ioResourceList->Count;
                ioResourceList->Version = version;
            }
        }

        ioResourceList->Count = oldCount;

        //
        // Move to next Io alternative list.
        //
        ioResourceList = (PIO_RESOURCE_LIST) ioResourceDescriptorEnd;
    }

    //
    // If there is not any valid alternative, convert CmList to Io list.
    //

    if (ioList->AlternativeLists == 0) {
         *FilteredList = PnpCmResourcesToIoResources(0,
                                                     CmList,
                                                     LCPRI_BOOTCONFIG);
        ExFreePool(ioList);
        return STATUS_SUCCESS;
    }

    //
    // we have finished filtering the resource requirements list.  Now allocate memory
    // and rebuild a new list.
    // We adjust the size to account for the CmResourceTypeNull type descriptors in the
    // original requirements as they dont get copied over into the filtered list.
    // We use the descriptor built into each IO_RESOURCE_LIST for storing the configdata
    // indicating boot priority.
    //
    size = sizeof(IO_RESOURCE_REQUIREMENTS_LIST) +
               sizeof(IO_RESOURCE_LIST) * (ioList->AlternativeLists - 1) +
               sizeof(IO_RESOURCE_DESCRIPTOR) * (ioResourceDescriptorCount - nullTypeCount);
    newList = (PIO_RESOURCE_REQUIREMENTS_LIST) ExAllocatePool(PagedPool, size);
    if (newList == NULL) {
        ExFreePool(ioList);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Walk through the io resource requirements list and pick up any valid descriptor.
    //

    newList->ListSize = size;
    newList->InterfaceType = CmList->List->InterfaceType;
    newList->BusNumber = CmList->List->BusNumber;
    newList->SlotNumber = ioList->SlotNumber;
    if (ioList->AlternativeLists > 1) {
        *ExactMatch = FALSE;
    }
    newList->AlternativeLists = ioList->AlternativeLists;
    ioResourceList = ioList->List;
    newIoResourceList = newList->List;
    while (--alternativeLists >= 0) {
        ioResourceDescriptor = ioResourceList->Descriptors;
        ioResourceDescriptorEnd = ioResourceDescriptor + ioResourceList->Count;
        if (ioResourceList->Version == 0xffff) {
            ioResourceList = (PIO_RESOURCE_LIST)ioResourceDescriptorEnd;
            continue;
        }
        newIoResourceList->Version = ioResourceList->Version;
        newIoResourceList->Revision = ioResourceList->Revision;

        newIoResourceDescriptor = newIoResourceList->Descriptors;
        if (ioResourceDescriptor->Type != CmResourceTypeConfigData) {
            newIoResourceDescriptor->Option = IO_RESOURCE_PREFERRED;
            newIoResourceDescriptor->Type = CmResourceTypeConfigData;
            newIoResourceDescriptor->ShareDisposition = CmResourceShareShared;
            newIoResourceDescriptor->Flags = 0;
            newIoResourceDescriptor->Spare1 = 0;
            newIoResourceDescriptor->Spare2 = 0;
            newIoResourceDescriptor->u.ConfigData.Priority = LCPRI_BOOTCONFIG;
            configDataDescriptor = newIoResourceDescriptor;
            newIoResourceDescriptor++;
        } else {
            newList->ListSize -= sizeof(IO_RESOURCE_DESCRIPTOR);
            configDataDescriptor = newIoResourceDescriptor;
        }

        while (ioResourceDescriptor < ioResourceDescriptorEnd) {
            if (ioResourceDescriptor->Type != CmResourceTypeNull) {
                *newIoResourceDescriptor = *ioResourceDescriptor;
                newIoResourceDescriptor++;
            }
            ioResourceDescriptor++;
        }
        newIoResourceList->Count = (ULONG)(newIoResourceDescriptor - newIoResourceList->Descriptors);

        configDataDescriptor->u.ConfigData.Priority =  LCPRI_BOOTCONFIG;

        //
        // Move to next Io alternative list.
        //
        newIoResourceList = (PIO_RESOURCE_LIST) newIoResourceDescriptor;
        ioResourceList = (PIO_RESOURCE_LIST) ioResourceDescriptorEnd;
    }

    PNP_ASSERT((PUCHAR)newIoResourceList == ((PUCHAR)newList + newList->ListSize));

    *FilteredList = newList;
    ExFreePool(ioList);
    return STATUS_SUCCESS;
}

NTSTATUS
PnpMergeFilteredResourceRequirementsList (
    __in PIO_RESOURCE_REQUIREMENTS_LIST IoList1,
    __in PIO_RESOURCE_REQUIREMENTS_LIST IoList2,
    __inout PIO_RESOURCE_REQUIREMENTS_LIST *MergedList
    )

/*++

Routine Description:

    This routines merges two IoLists into one.


Arguments:

    IoList1 - supplies the pointer to the first IoResourceRequirementsList

    IoList2 - supplies the pointer to the second IoResourceRequirementsList

    MergedList - Supplies a variable to receive the merged resource
             requirements list.

Return Value:

    A NTSTATUS code to indicate the result of the function.

--*/
{
    NTSTATUS status = STATUS_SUCCESS;
    PIO_RESOURCE_REQUIREMENTS_LIST ioList, newList;
    ULONG size;
    PUCHAR p;

    PAGED_CODE();

    *MergedList = NULL;

    //
    // First handle the easy cases that both IO Lists are empty or any one of
    // them is empty.
    //

    if ((IoList1 == NULL || IoList1->AlternativeLists == 0) &&
        (IoList2 == NULL || IoList2->AlternativeLists == 0)) {
        return status;
    }
    ioList = NULL;
    if (IoList1 == NULL || IoList1->AlternativeLists == 0) {
        ioList = IoList2;
    } else if (IoList2 == NULL || IoList2->AlternativeLists == 0) {
        ioList = IoList1;
    }
    if (ioList) {
        newList = (PIO_RESOURCE_REQUIREMENTS_LIST) ExAllocatePool(PagedPool, ioList->ListSize);
        if (newList == NULL) {
            return STATUS_INSUFFICIENT_RESOURCES;
        }
        RtlCopyMemory(newList, ioList, ioList->ListSize);
        *MergedList = newList;
        return status;
    }

    //
    // Do real work...
    //

    size = IoList1->ListSize + IoList2->ListSize - FIELD_OFFSET(IO_RESOURCE_REQUIREMENTS_LIST, List);
    newList = (PIO_RESOURCE_REQUIREMENTS_LIST) ExAllocatePool(
                          PagedPool,
                          size
                          );
    if (newList == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    p = (PUCHAR)newList;
    RtlCopyMemory(p, IoList1, IoList1->ListSize);
    p += IoList1->ListSize;
    __analysis_assume(size > IoList1->ListSize);
    PNP_ASSERT(size > IoList1->ListSize);
    RtlCopyMemory(p,
                  &IoList2->List[0],
                  size - IoList1->ListSize
                  );
    newList->ListSize = size;
    newList->AlternativeLists += IoList2->AlternativeLists;
    *MergedList = newList;
    return status;

}

LOGICAL
PnpIsLegacyDriver (
    __in PDRIVER_OBJECT DriverObject
    )

/*++

Routine Description:

    This routine checks if the driver object specifies a legacy driver.

Arguments:

    DriverObject - supplies a pointer to the driver object to be checked.

Return Value:

    LOGICAL

--*/

{

    PAGED_CODE();

    PNP_ASSERT(DriverObject != NULL);
    PNP_ASSERT(DriverObject->DriverExtension != NULL);

    //
    // If AddDevice entry is not empty it is a wdm driver
    //

    if (DriverObject->DriverExtension->AddDevice) {
        return FALSE;
    }

    //
    // Else if LEGACY flag is set in the driver object, it's a legacy driver.
    //

    if (DriverObject->Flags & DRVO_LEGACY_DRIVER) {
        return TRUE;
    }

    return FALSE;
}

NTSTATUS
PnpQueryAndSaveDeviceNodeCapabilities (
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    Called after start to refresh Capability flags

Arguments:

    DeviceObject - supplies a pointer to a device object whose registry
        values are to be updated.

Return Value:

    status

--*/

{
    NTSTATUS status;
    DEVICE_CAPABILITIES capabilities;

    PAGED_CODE();

    PNP_ASSERT(DeviceNode != NULL);

    //
    // Open the device instance key
    //

    status = PpIrpQueryCapabilities(DeviceNode->PhysicalDeviceObject, &capabilities);
    if (!NT_SUCCESS(status)) {

        return status;
    }

    return PnpSaveDeviceCapabilities(DeviceNode, &capabilities, FALSE);
}

NTSTATUS
PnpSaveDeviceCapabilities (
    __in PDEVICE_NODE DeviceNode,
    __in PDEVICE_CAPABILITIES Capabilities,
    __in BOOLEAN NewDevice
    )

/*++

Routine Description:

    This routine updates device capabilities, must be called after a valid
    device instance key has been created Called directly from
    IopProcessNewDeviceNode, and indirecly via
    PnpQueryAndSaveDeviceNodeCapabilities after device is started.

Arguments:

    DeviceObject - Supplies a pointer to a device object whose registry values
        are to be updated.

    Capabilities - Supplies the new capabilities for the device

    NewDevice - Supplies whether the device is a newly created device or not.

Return Value:

    status

--*/

{
    HANDLE handle;
    ULONG setPropertyFlags;
    NTSTATUS status;
    ULONG value;

    PAGED_CODE();

    PNP_ASSERT(DeviceNode != NULL);
    PNP_ASSERT(Capabilities != NULL);

    setPropertyFlags = (NewDevice) ? PIPNPRTL_FLAG_WRITE_THROUGH : 0;

    //
    // Open the device instance key
    //

    status = PnpDeviceObjectToDeviceInstance(DeviceNode->PhysicalDeviceObject,
                                             &handle,
                                             KEY_ALL_ACCESS);

    if (NT_SUCCESS(status)) {

        if (DeviceNode->Flags & DNF_HAS_BOOT_CONFIG) {

            Capabilities->SurpriseRemovalOK = 0;
        }

        //
        // Pnp_assert the bit fields are completely contained in a ULONG. This
        // is a public structure, so it shouldn't ever change, but paranoia is a
        // good thing...
        //

        PNP_ASSERT((FIELD_OFFSET(DEVICE_CAPABILITIES, Address) -
                FIELD_OFFSET(DEVICE_CAPABILITIES, Version) -
                FIELD_SIZE  (DEVICE_CAPABILITIES, Version)) == sizeof(ULONG));

        DeviceNode->CapabilityFlags =
            *((PULONG) (((PUCHAR) Capabilities) +
            FIELD_OFFSET(DEVICE_CAPABILITIES, Version) +
            FIELD_SIZE(DEVICE_CAPABILITIES, Version)));

        value =    (Capabilities->LockSupported)          |
                   (Capabilities->EjectSupported    << 1) |
                   (Capabilities->WarmEjectSupported<< 1) |
                   (Capabilities->Removable         << 2) |
                   (Capabilities->DockDevice        << 3) |
                   (Capabilities->UniqueID          << 4) |
                   (Capabilities->SilentInstall     << 5) |
                   (Capabilities->RawDeviceOK       << 6) |
                   (Capabilities->SurpriseRemovalOK << 7) |
                   (Capabilities->HardwareDisabled  << 8) |
                   (Capabilities->NonDynamic        << 9);

        _CmSetDeviceRegProp(PiPnpRtlCtx,
                            PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                            handle,
                            CM_REG_PROP_CAPABILITIES,
                            REG_DWORD,
                            (PUCHAR)&value,
                            sizeof(value),
                            setPropertyFlags);

        value = Capabilities->UINumber;
        if (value != (ULONG)-1) {

            _CmSetDeviceRegProp(PiPnpRtlCtx,
                                PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                handle,
                                CM_REG_PROP_UI_NUMBER,
                                REG_DWORD,
                                (PUCHAR)&value,
                                sizeof(value),
                                setPropertyFlags);

        } else {

            //
            // Delete the property
            //

            _CmSetDeviceRegProp(PiPnpRtlCtx,
                                PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                handle,
                                CM_REG_PROP_UI_NUMBER,
                                REG_DWORD,
                                NULL,
                                0,
                                setPropertyFlags);
        }

        ZwClose(handle);
    }

    return status;
}


NTSTATUS
PnpGenerateDeviceIdsHash(
    __in_opt PCZZWSTR HardwareIds,
    __in_opt PCZZWSTR CompatibleIds,
    __out PULONG DeviceIdsHash
    )
/*++

Description:

    This routine generates a device IDs hash value that is the sum of the
    case-insensitive hash values of each string in the multi-sz HardwareIds
    and CompatibleIds lists reported for the device.  The hash is computed
    by calling RtlHashUnicodeString (CaseInSensitive = TRUE,
    HASH_STRING_ALGORITHM_DEFAULT) on every non-enmpty string in the list
    and adding all values together, allowing for the possibility of integer
    overflow.

Arguments:

    HardwareIds - Optional multi-sz list of hardware IDs for device node.

    CompatibleIds - Optional multi-sz list of compatible IDs for device node.

    DeviceIdsHash  - Returns generated hash value.

Return Value:

    Returns the success status code if the hash was generated.

--*/

{
    NTSTATUS status = STATUS_SUCCESS;
    INT i;
    PCZZWSTR deviceIds[2];
    PWSTR deviceId;
    UNICODE_STRING unicodeString;
    ULONG hashValue, deviceIdsHash;

    *DeviceIdsHash = 0;

    deviceIds[0] = HardwareIds;
    deviceIds[1] = CompatibleIds;
    deviceIdsHash = 0;

    for (i = 0; i < RTL_NUMBER_OF(deviceIds); i++) {

        for (deviceId = (PWSTR)deviceIds[i];
             deviceId && (*deviceId != UNICODE_NULL);
             deviceId += wcslen(deviceId) + 1) {

            status = RtlInitUnicodeStringEx(&unicodeString, deviceId);

            if (!NT_SUCCESS(status)) {
                break;
            }

            status = RtlHashUnicodeString(&unicodeString,
                                          TRUE,
                                          HASH_STRING_ALGORITHM_DEFAULT,
                                          &hashValue);
            if (!NT_SUCCESS(status)) {
                break;
            }

            deviceIdsHash += hashValue;
        }

        if (!NT_SUCCESS(status)) {
            break;
        }
    }

    if (!NT_SUCCESS(status)) {
        goto Exit;
    }

    *DeviceIdsHash = deviceIdsHash;

Exit:

    return status;
}


NTSTATUS
PnpRestartDeviceNode(
    __in PDEVICE_NODE DeviceNode
    )
{
    PAGED_CODE();

    PpDevNodeLockTree(PPL_TREEOP_BLOCK_READS_FROM_ALLOW);

    PNP_ASSERT(DeviceNode->State == DeviceNodeRemoved ||
           DeviceNode->State == DeviceNodeInitialized );

    PNP_ASSERT(!PipDoesDevNodeHaveProblem(DeviceNode));

    PNP_ASSERT(DeviceNode->Flags & DNF_ENUMERATED);

    if (!(DeviceNode->Flags & DNF_ENUMERATED)) {

        PpDevNodeUnlockTree(PPL_TREEOP_BLOCK_READS_FROM_ALLOW);
        return STATUS_UNSUCCESSFUL;
    }

    PipClearDevNodeUserFlags(DeviceNode, DNUF_NEED_RESTART | DNUF_WILL_BE_REMOVED);
    PipClearDevNodeFlags(DeviceNode, DNF_DRIVER_BLOCKED | DNF_HARDWARE_VERIFICATION);

    PNP_ASSERT((DeviceNode->PowerFlags & DNPF_PREPARED) == 0);

    DeviceNode->PowerFlags = 0;
    DeviceNode->StateFlags &= ~DNSF_POFX_FLAGS;

#pragma prefast(push)
#pragma prefast(disable:__WARNING_RACE_CONDITION, "No concurrent updates of this data are possible while the device restarts.")

    DeviceNode->FxActivationCount = 0;
    DeviceNode->FxSleepCount = 0;

#pragma prefast(pop)

#if DBG

    DeviceNode->FailureStatus = 0;
    if (DeviceNode->PreviousResourceList) {
        ExFreePool(DeviceNode->PreviousResourceList);
        DeviceNode->PreviousResourceList = NULL;
    }

    if (DeviceNode->PreviousResourceRequirements) {
        ExFreePool(DeviceNode->PreviousResourceRequirements);
        DeviceNode->PreviousResourceRequirements = NULL;
    }

#endif

    //
    //     Prepare to set the device state back to DeviceNodeUninitialized. To
    // do this we free any existing devnode strings so we can recreate them
    // during enumeration.
    //
    // ADRIAO N.B. 8/19/2000 -
    //     We don't restore the state to DeviceNodeInitialized to maintain Win2K
    // behavior. We have no idea if anyone actually depends on this. In theory
    // this would let a bus driver get away with changing a child's IDs after a
    // remove.
    //

    if (DeviceNode->State != DeviceNodeUninitialized) {
        PipClearDevNodeFlags(DeviceNode, DNF_NO_RESOURCE_REQUIRED |
                                         DNF_RESOURCE_REQUIREMENTS_CHANGED |
                                         DNF_NON_STOPPED_REBALANCE |
                                         DNF_CHILD_WITH_INVALID_ID |
                                         DRIVER_FLAGS_MASK);

        if (DeviceNode->ServiceName.Length != 0) {
            ExFreePool(DeviceNode->ServiceName.Buffer);
            PiWstrToUnicodeString(&DeviceNode->ServiceName, NULL);
        }

        IopAcquireResourceListLock(&PiResourceListLock);
        if (DeviceNode->ResourceRequirements != NULL) {
            ExFreePool(DeviceNode->ResourceRequirements);
            DeviceNode->ResourceRequirements = NULL;
            PipClearDevNodeFlags(DeviceNode, DNF_RESOURCE_REQUIREMENTS_NEED_FILTERED);
        }
        IopReleaseResourceListLock(&PiResourceListLock);
    }

    PNP_ASSERT(DeviceNode->ServiceName.Length == 0 &&
           DeviceNode->ServiceName.MaximumLength == 0 &&
           DeviceNode->ServiceName.Buffer == NULL);

    PNP_ASSERT(!(DeviceNode->Flags &
           ~(DNF_MADEUP | DNF_ENUMERATED | DNF_HAS_BOOT_CONFIG | DNF_IDS_QUERIED |
             DNF_BOOT_CONFIG_RESERVED | DNF_NO_RESOURCE_REQUIRED |
             DNF_UNINSTALLED)));

    DeviceNode->NumaNodeIndex = PNP_NUMA_NODE_UNKNOWN;

    PipSetDevNodeState(DeviceNode, DeviceNodeUninitialized, NULL);

    PpDevNodeUnlockTree(PPL_TREEOP_BLOCK_READS_FROM_ALLOW);

    return STATUS_SUCCESS;
}

LOGICAL
PnpRegSzToString(
    __in_bcount(RegSzLength)  PWCHAR RegSzData,
    __in  ULONG  RegSzLength,
    __out_opt PULONG StringLength  ,
    __deref_opt_out_bcount(*StringLength) PWSTR  *CopiedString
    )

/*++

Routine Description:

    This routine takes as input a REG_SZ data buffer (as returned in the DataOffset area
    of the buffer in a KEY_VALUE_FULL_INFORMATION structure), as well as the length
    of the buffer, in bytes (as specified by the DataLength field in the above mentioned
    struct).  It optionally returns the length of the contained string (in bytes), not
    including the terminating NULL, as well as an optional copy of the string itself
    (properly NULL-terminated).

    It is the responsibility of the caller to free the (PagedPool) buffer allocated
    for the string copy.

Arguments:

    RegSzData - Supplies a pointer to the REG_SZ data buffer.

    RegSzLength - Supplies the length of the RegSzData buffer, in bytes.

    StringLength - Optionally supplies a pointer to a variable that will receive
        the length, in bytes, of the string (excluding terminating NULL).

    CopiedString - Optionally supplies a pointer to a wide character pointer
        that will recieve a (properly NULL-terminated) copy of the specified
        string. If this paramater is NULL, no copy will be made.

Return Value:

    If success, returns TRUE

    If failure (not able to allocate memory for string copy), returns FALSE

--*/

{
    PWCHAR curPos, endOfRegSzData;
    ULONG actualStringLength;

    //
    // Since we're converting a byte count to a wide-character count (and the
    // compiler is converting it back when adding it to a PWCHAR), we are
    // ensuring that endOfRegSzData is not on an odd-byte boundary, even if
    // the RegSzLength passed in was odd.  This takes care of the case where
    // the REG_SZ buffer retrieved from the registry is bogus (e.g., you have
    // a 5-byte buffer, the 1st unicode character of which is a UNICODE_NULL).
    //
    endOfRegSzData = (curPos = RegSzData) + CB_TO_CWC(RegSzLength);

    while ((curPos < endOfRegSzData) && *curPos) {

        curPos++;
    }

    actualStringLength = (ULONG)((PUCHAR)curPos - (PUCHAR)RegSzData);

    if (ARGUMENT_PRESENT(StringLength)) {

        *StringLength = (ULONG)((PUCHAR)curPos - (PUCHAR)RegSzData);
    }

    if (ARGUMENT_PRESENT(CopiedString)) {

        //
        // Allocate memory for the string (+ terminating NULL)
        //
        *CopiedString = (PWSTR)ExAllocatePool(PagedPool,
                                              actualStringLength +
                                                sizeof(UNICODE_NULL));
        if (*CopiedString == NULL) {

            return FALSE;
        }

        //
        // Copy the string and NULL-terminate it.
        //
        if (actualStringLength) {

            RtlCopyMemory(*CopiedString, RegSzData, actualStringLength);
        }

        *(PWCHAR)((PUCHAR)(*CopiedString) + actualStringLength) = UNICODE_NULL;
    }

    return TRUE;
}

ULONG
IopDebugPrint (
    __in   ULONG    Level,
    __in_z PCHAR    Format,
    ...
    )
{
    va_list ap;

    va_start(ap, Format);

    vDbgPrintExWithPrefix("NTOSPNP:", DPFLTR_NTOSPNP_ID, Level, Format, ap);

    va_end(ap);

    return Level;
}

VOID
PnpLogEvent(
    __in PUNICODE_STRING InsertionString1,
    __in_opt PUNICODE_STRING InsertionString2,
    __in NTSTATUS Status,
    __in_bcount_opt(DumpDataSize) PVOID DumpData,
    __in_opt ULONG DumpDataSize
    )
/*++

Routine Description:

    This routine logs the driver block event.

Arguments:

    InsertionString1 - First insertion string for event log entry.

    InsertionString2 - Second insertion string for event log entry.

    Status - Status code to be logged.

    DumpData - Data to be logged with the event.

    DumpDataSize - Size of the data to be logged in bytes.

Return Value:

    None.

--*/
{
    SIZE_T size, stringLength1, stringLength2, stringOffset;
    PIO_ERROR_LOG_PACKET errorLogEntry;
    PUCHAR stringPtr;

    PAGED_CODE();

    stringLength1 = stringLength2 = 0;

    if (InsertionString1) {

        stringLength1 = InsertionString1->Length + sizeof(UNICODE_NULL);
    }

    if (InsertionString2) {

        stringLength2 = InsertionString2->Length + sizeof(UNICODE_NULL);
    }

    //
    // Calculate the size of the the error packet
    //
    size = FIELD_OFFSET(IO_ERROR_LOG_PACKET, DumpData) + DumpDataSize;

    //
    // Determine the string offset and size, adjusting for alignment.
    //
    stringOffset = ALIGN_UP_ULONG(size, 2);

    size = stringOffset + stringLength1 + stringLength2;

    if (size <= ERROR_LOG_MAXIMUM_SIZE) {

        //
        // Allocate an error log packet. Note that Io takes care of initializing
        // the header and zeroing all fields (such as NumberOfStrings).
        //
        errorLogEntry = IoAllocateGenericErrorLogEntry((UCHAR)size);

        if (errorLogEntry) {

            errorLogEntry->ErrorCode = Status;
            errorLogEntry->FinalStatus = Status;
            errorLogEntry->DumpDataSize = (USHORT)DumpDataSize;
            errorLogEntry->StringOffset = (USHORT)stringOffset;
            stringPtr = ((PUCHAR)errorLogEntry) + stringOffset;

            if (DumpDataSize) {

                RtlCopyMemory(&errorLogEntry->DumpData[0], DumpData, DumpDataSize);
            }

            if (InsertionString1) {

                errorLogEntry->NumberOfStrings = 1;
                RtlCopyMemory(stringPtr, InsertionString1->Buffer, InsertionString1->Length);
                stringPtr += InsertionString1->Length;
                *(PWCHAR)stringPtr = UNICODE_NULL;
                stringPtr += sizeof(UNICODE_NULL);
            }

            if (InsertionString2) {

                errorLogEntry->NumberOfStrings += 1;
                RtlCopyMemory(stringPtr, InsertionString2->Buffer, InsertionString2->Length);
                stringPtr += InsertionString2->Length;
                *(PWCHAR)stringPtr = UNICODE_NULL;
            }

            IoWriteErrorLogEntry(errorLogEntry);
        }
    } else {

        PNP_ASSERT(size <= ERROR_LOG_MAXIMUM_SIZE);
    }
}

ULONG
PnpFixupID(
    __in __nullnullterminated PWSTR ID,
    __in ULONG MaxIDLength,
    __in LOGICAL Multi,
    __in ULONG AllowedSeparators,
    __in_opt PUNICODE_STRING LogString
    )

/*++

Routine Description:

    This routine parses the device instance string and replaces any invalid
    characters (not allowed in a "device instance") with an underscore
    character.

    Invalid characters are:
        c <= 0x20 (' ')
        c >  0x7F
        c == 0x2C (',')

Arguments:

    ID - ID to be fixed up.

    MaxIDLength - Maximum allowed size of ID.

    Multi - Specifies if the ID is MULTI_SZ or not.

    AllowedSeparators - Number of separators allowed in the ID.

Return Value:

    ID length in number of characters.

--*/

{
    PWCHAR p, pMax, lastNull;
    ULONG separators;
    UNICODE_STRING reason;

    PAGED_CODE();

    //
    //
    separators = 0;
    lastNull = NULL;
    for(p = ID, pMax = p + MaxIDLength; p < pMax; p++) {

        if(*p == UNICODE_NULL) {

            if(Multi == FALSE || (lastNull && p == lastNull + 1)) {

                break;
            }
            pMax += MaxIDLength;
            lastNull = p;
            continue;
        }
        if (*p == L' ') {

            *p = L'_';
        } else if ((*p < L' ')  || (*p > (WCHAR)0x7F) || (*p == L',')) {

            IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                         "PnpFixupID: ID at %p has invalid character %02X\n",
                         ID,
                         *p));

            if(LogString) {

                PiWstrToUnicodeString(&reason, L"invalid character");
                PnpLogEvent(LogString, &reason, STATUS_PNP_INVALID_ID, p, sizeof(WCHAR));
            }

            return 0;
        } else if ((*p == OBJ_NAME_PATH_SEPARATOR && ++separators > AllowedSeparators)) {

            IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                         "PnpFixupID: ID at %p has too many (%d) separators\n",
                         ID,
                         separators));
            if(LogString) {

                PiWstrToUnicodeString(&reason, L"too many separators");
                PnpLogEvent(LogString,
                           &reason,
                           STATUS_PNP_INVALID_ID,
                           &separators,
                           sizeof(ULONG)
                           );
            }
            return 0;
        }
    }
    if( p >= pMax ||
        (AllowedSeparators != (ULONG)-1 &&
         separators != AllowedSeparators)) {

        IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                     "PnpFixupID: ID at %p not terminated, or too long or has invalid number (%d) of separators\n",
                     ID,
                     separators));
        if(LogString) {

            PiWstrToUnicodeString(&reason,
                                  L"not terminated, too long or invalid number of separators"
                                  );
            PnpLogEvent(LogString, &reason, STATUS_PNP_INVALID_ID, NULL, 0);
        }
        return 0;
    }

    return (ULONG)(ULONG_PTR)(p - ID) + 1;
}

NTSTATUS
PnpQueryID(
    __in PDEVICE_NODE DeviceNode,
    __in BUS_QUERY_ID_TYPE IDType,
    __deref_out_bcount(*IDLength) PWCHAR *ID,
    __out PULONG IDLength
    )

/*++

Routine Description:

    This routine queries the specified ID and fixes it up. If this
    routine fails, ID will be set to NULL.

Arguments:

    DeviceNode - The devnode whose IDs need to be queried.

    IDType - Type of ID to be queried.

    ID - Receives the ID returned by the driver if any. The caller
    is expected to free the storage for ID on success.

    IDLength - Receives the length of the ID (including terminating NULL) in
        bytes.

Return Value:

    NTSTATUS.

--*/

{
    NTSTATUS status;
    UNICODE_STRING reason;

    PAGED_CODE();

    PNP_ASSERT(IDType == BusQueryDeviceID || IDType == BusQueryInstanceID ||
          IDType == BusQueryHardwareIDs || IDType == BusQueryCompatibleIDs);

    *IDLength = 0;
    status = PnpIrpQueryID(DeviceNode->PhysicalDeviceObject, IDType, ID);
    if(NT_SUCCESS(status)) {

        switch(IDType) {

            case BusQueryDeviceID:

                *IDLength = PnpFixupID(*ID,
                                      MAX_DEVICE_ID_LEN,
                                      FALSE,
                                      1,
                                      &DeviceNode->Parent->ServiceName);
                break;

            case BusQueryInstanceID:

                *IDLength = PnpFixupID(*ID,
                                      MAX_DEVICE_ID_LEN,
                                      FALSE,
                                      0,
                                      &DeviceNode->Parent->ServiceName);
                break;

            case BusQueryHardwareIDs:
            case BusQueryCompatibleIDs:

                *IDLength = PnpFixupID(*ID,
                                      REGSTR_VAL_MAX_HCID_LEN,
                                      TRUE,
                                      (ULONG)-1,
                                      &DeviceNode->Parent->ServiceName);
                break;

            default:

                *IDLength = 0;
                break;
        }
        (*IDLength) *= sizeof(WCHAR);
        if(*IDLength == 0) {

            status = STATUS_PNP_INVALID_ID;
        }
    }

    if(!NT_SUCCESS(status)) {

        if (status == STATUS_PNP_INVALID_ID ||
            IDType == BusQueryDeviceID ||
            (IDType == BusQueryInstanceID && status == STATUS_INSUFFICIENT_RESOURCES)) {

            PipSetDevNodeProblem(DeviceNode, CM_PROB_INVALID_DATA, status);
            if ((DeviceNode->Parent->Flags & DNF_CHILD_WITH_INVALID_ID) == 0) {

                PipSetDevNodeFlags(DeviceNode->Parent, DNF_CHILD_WITH_INVALID_ID);
                PnpSetInvalidIDEvent(PnpGetDeviceInstancePath(DeviceNode->Parent));
            }
        }
        if (status == STATUS_PNP_INVALID_ID) {

            IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                         "PnpQueryID: Bogus ID returned by %wZ\n",
                         &DeviceNode->Parent->ServiceName));
            PNP_ASSERTMSG("Bogus ID returned by bus driver, see preceeding debug output for details ",
                      status != STATUS_PNP_INVALID_ID);

        } else if (IDType == BusQueryDeviceID &&
                   status != STATUS_INSUFFICIENT_RESOURCES) {
            //
            // DeviceID is not optional.
            //
            PiWstrToUnicodeString(&reason, L"failed IRP_MN_QUERY_ID-BusQueryDeviceID");
            PnpLogEvent(&DeviceNode->Parent->ServiceName,
                       &reason,
                       status,
                       NULL,
                       0);

            IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                         "PnpIrpQueryID: Failed by %wZ, status = %x\n",
                         &DeviceNode->Parent->ServiceName, status));
            PNP_ASSERTMSG("IRP_MN_QUERY_ID failed, see preceeding debug output for details ",
                      IDType != BusQueryDeviceID);
        }

        if(*ID) {

            ExFreePool(*ID);
            *ID = NULL;
            *IDLength = 0;
        }
    }

    return status;
}

NTSTATUS
PnpQueryDeviceID(
    __in PDEVICE_NODE DeviceNode,
    __deref_out PWSTR *BusID,
    __deref_out PWSTR *DeviceID
    )

/*++

Routine Description:

    This routine queries the Device ID and fixes it up. It also parses the
    DeviceID and returns the pointers to BusID and DeviceID parts. If this
    routine fails, BusID and DeviceID will be set to NULL.

Arguments:

    DeviceNode - The devnode whose DeviceID needs to be queried.

    BusID - Recieves the pointer to the bus part of DeviceID.

    DeviceID - Recieves the pointer to the device part of DeviceID.

Return Value:

    NTSTATUS.

--*/

{
    NTSTATUS status;
    PWCHAR id, separator;
    ULONG idLength;

    PAGED_CODE();

    *BusID = NULL;
    *DeviceID= NULL;

    status = PnpQueryID(DeviceNode, BusQueryDeviceID, &id, &idLength);
    if(NT_SUCCESS(status)) {

        PNP_ASSERT(id && idLength);

        *BusID = id;
        separator = wcschr(id, OBJ_NAME_PATH_SEPARATOR);

        PNP_ASSERT(separator);

        *separator = UNICODE_NULL;
        *DeviceID = separator + 1;

    } else {

        PNP_ASSERT(id == NULL && idLength == 0);
    }

    return status;
}

NTSTATUS
PnpQueryBusInformation(
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This routine queries the bus information.

Arguments:

    DeviceNode - The devnode whose BusInormation needs to be queried.

Return Value:

    NTSTATUS.

--*/

{
    NTSTATUS status;
    PPNP_BUS_INFORMATION busInfo;

    PAGED_CODE();

    status = PpIrpQueryBusInformation(
                DeviceNode->PhysicalDeviceObject,
                &busInfo
                );
    if(NT_SUCCESS(status)) {

        PNP_ASSERT(busInfo);

        DeviceNode->ChildBusTypeIndex = PnpBusTypeGuidGetIndex(
                                            &busInfo->BusTypeGuid
                                            );
        DeviceNode->ChildInterfaceType = busInfo->LegacyBusType;
        DeviceNode->ChildBusNumber = busInfo->BusNumber;

        ExFreePool(busInfo);

    } else {

        PNP_ASSERT(busInfo == NULL);

        DeviceNode->ChildBusTypeIndex = 0xffff;
        DeviceNode->ChildInterfaceType = InterfaceTypeUndefined;
        DeviceNode->ChildBusNumber = 0xfffffff0;
    }

    return status;
}

NTSTATUS
PnpBusTypeGuidInitialize(
    VOID
    )

/*++

Routine Description:

    This routine opens the specified subkey.

Arguments:

    None

Return Value:

    STATUS_SUCCESS.

--*/

{
    PAGED_CODE();

    PnpBusTypeGuidCountMax = 16;
    PnpBusTypeGuidArray = ExAllocatePool(PagedPool,
                                        sizeof(GUID) * PnpBusTypeGuidCountMax);
    if (PnpBusTypeGuidArray == NULL) {

        PnpBusTypeGuidCountMax = 0;
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    PnpBusTypeGuidCount = 0;

    KeInitializeGuardedMutex(&PnpBusTypeGuidLock);

    return STATUS_SUCCESS;
}

USHORT
PnpBusTypeGuidGetIndex(
    __in LPGUID BusTypeGuid
    )

/*++

Routine Description:

    This routine looks up the BusTypeGuid and returns its index into the table.

Arguments:

    BusTypeGuid - GUID to lookup.

Return Value:

    Index into the table iff successful, else 0xFFFF.

--*/

{
    LPGUID p;
    ULONG i;

    PAGED_CODE();

    KeAcquireGuardedMutex(&PnpBusTypeGuidLock);
    //
    // First look it up.
    //
    for (i = 0; i < PnpBusTypeGuidCount; i++) {

        if (IopCompareGuid(BusTypeGuid, &PnpBusTypeGuidArray[i])) {

            break;
        }
    }
    //
    // If the GUID is not in the table, add it.
    //
    if (i == PnpBusTypeGuidCount) {
        //
        // Grow the table if needed.
        //
        if (i == PnpBusTypeGuidCountMax) {
            //
            // We grow the table one entry at a time. This should not be a
            // problem since this should not happen often.
            //
            p  = ExAllocatePool(PagedPool, (i + 1) * sizeof(GUID));
            if (p) {
                //
                // Copy the old table.
                //
                RtlCopyMemory(p,
                              PnpBusTypeGuidArray,
                              PnpBusTypeGuidCount * sizeof(GUID)
                              );
                //
                // Update global data.
                //
                PnpBusTypeGuidCountMax++;
                if (PnpBusTypeGuidArray) {

                    ExFreePool(PnpBusTypeGuidArray);
                }
                PnpBusTypeGuidArray = p;

            } else {
                //
                // Return invalid index on failure.
                //
                i = (ULONG)-1;
            }
        }
        //
        // Copy the new entry on success.
        //
        if (i != (ULONG)-1) {
            //
            // Copy the new entry.
            //
            RtlCopyMemory(&PnpBusTypeGuidArray[PnpBusTypeGuidCount],
                          BusTypeGuid,
                          sizeof(GUID)
                          );
            //
            // Update global data.
            //
            PnpBusTypeGuidCount++;
        }
    }

    KeReleaseGuardedMutex(&PnpBusTypeGuidLock);

    return (USHORT)i;
}

NTSTATUS
PnpBusTypeGuidGet(
    __in USHORT Index,
    __inout LPGUID BusTypeGuid
    )

/*++

Routine Description:

    This routine return the BusTypeGuid in the table at the specified index.

Arguments:

    Index - BusTypeGuid index.

    BusTypeGuid - Recieves the GUID.

Return Value:

    NTSTATUS.

--*/

{
    NTSTATUS status;

    PAGED_CODE();

    KeAcquireGuardedMutex(&PnpBusTypeGuidLock);

    if (Index < PnpBusTypeGuidCount) {

        RtlCopyMemory(BusTypeGuid, &PnpBusTypeGuidArray[Index], sizeof(GUID));
        status = STATUS_SUCCESS;
    } else {

        status = STATUS_OBJECT_NAME_NOT_FOUND;
    }

    KeReleaseGuardedMutex(&PnpBusTypeGuidLock);

    return status;
}

NTSTATUS
PnpDriverObjectDereferenceComplete(
    __in PDRIVER_OBJECT DriverObject
    )
{
    PAGED_CODE();

    UNREFERENCED_PARAMETER(DriverObject);

    return PnpRequestDeviceAction(IopRootDeviceNode->PhysicalDeviceObject,
                                  ClearProblem,
                                  FALSE,
                                  (ULONG_PTR)CM_PROB_DRIVER_FAILED_PRIOR_UNLOAD,
                                  NULL,
                                  NULL);
}

LOGICAL
PnpIsChainDereferenced (
    __in_ecount(Count) PDEVICE_OBJECT *DeviceObjects,
    __in ULONG Count,
    __in LOGICAL Query,
    __in LOGICAL CleanStack,
    __out_opt PDEVICE_OBJECT *VetoingDevice
    )

/*++

Routine Description:

    Called by PnP when processing a Surprise Removal or a Query Remove.

    In the case of Surprise Removal this function will set DOE_REMOVE_PENDING
    in the device extension flags of the each PDO and all its attached devices.
    If there are no more references to the device, PnpChainDereferenceComplete
    is called to notify PnP that this PDO is ready for removal.

    If there are references then as each remaining PDO and its attachment
    chain's ReferenceCount drops to zero IopCheckUnloadOrDelete will call
    PnpChainDereferenceComplete (supplied by PnP).

    In the case of Query Remove this function set DOE_REMOVE_PENDING on the
    PDO and all its attached devices to prevent further opens.  It also checks
    to see if the ReferenceCount for all the PDOs and their attached devices is
    zero.  If so it leaves the DOE_REMOVE_PENDING set and returns FALSE.  If
    not, it resets the DOE_REMOVE_PENDING on all the PDOs and their attached
    devices and returns TRUE.

Arguments:

    PhysicalDeviceObjects   List of PDEVICE_OBJECTs for all of the PDOs to be
                            checked.

    DeviceObjectCount       Count of PDEVICE_OBJECTs in PhysicalDeviceObjects.

    Query                   TRUE if this is for a Query Remove.

    VetoingDevice           Only used for Query Remove, Set to first PDO with a
                            ReferenceCount not equal to zero.  This is used to
                            provide feedback to the user as to why the query
                            may have failed.


Return Value:
    If Query is set then the return value is TRUE if there are outstanding
    opens on any of the PDOs or the attached devices, otherwise FALSE is
    returned.

    If Query is NOT set then the return value is always TRUE.

--*/

{
    PDEVICE_OBJECT deviceObject;
    LOGICAL inUse;
    LONG i;
    KIRQL irql;

    if (VetoingDevice != NULL) {
        *VetoingDevice = NULL;
    }

    deviceObject = NULL;
    inUse = FALSE;
    irql = KeAcquireQueuedSpinLock(LockQueueIoDatabaseLock);
    for (i = 0; i < (LONG)Count; i++) {
        deviceObject = NULL;
        inUse = PnpIsAnyDeviceInUse(&DeviceObjects[i], 1, &deviceObject);
        PnpUpdateExtensionFlags(&DeviceObjects[i],
                                1,
                                TRUE,
                                TRUE,   // Set
                                DOE_REMOVE_PENDING);

        //
        // If this is a surprise remove/fail and the device is not in use,
        // attempt to remove it.
        //

        if (Query == FALSE && inUse == FALSE) {

            //
            // Tag the stack with no open handles.
            //

            KeReleaseQueuedSpinLock(LockQueueIoDatabaseLock, irql);
            PnpChainDereferenceComplete(DeviceObjects[i], CleanStack);
            irql = KeAcquireQueuedSpinLock(LockQueueIoDatabaseLock);

        //
        // Otherwise if this is a query-remove and there is still a device in
        // use, then fail and cancel.
        //

        } else if (Query != FALSE && inUse != FALSE) {

            break;
        }
    }

    if (Query != FALSE) {
        if (inUse != FALSE) {
            if (VetoingDevice != NULL) {
                *VetoingDevice = deviceObject;
            }

            //
            // There is some open handle thats preventing query remove.
            // Undo the flag that prevents create.
            //

            for (; i >= 0; i--) {
                PnpUpdateExtensionFlags(&DeviceObjects[i],
                                        1,
                                        TRUE,
                                        FALSE,  // Clear
                                        DOE_REMOVE_PENDING);
            }
        }
    }

    KeReleaseQueuedSpinLock(LockQueueIoDatabaseLock, irql);
    if (Query == FALSE) {
        return TRUE;

    } else {

        return inUse;
    }
}

NTSTATUS
PnpDriverStarted (
    __in PDRIVER_OBJECT DriverObject,
    __in HANDLE KeyHandle,
    __in PUNICODE_STRING ServiceName
    )

/*++

Routine Description:

    This function gets called after a driver successfully loads and initializes.

Arguments:

    DriverObject - Supplies a pointer to the new initialized driver object.

    KeyHandle - Supplies the service key handle.

    ServiceName - Supplies a pointer to the service name.

Return Value:

    NTSTATUS.

--*/

{
    NTSTATUS status;

    PAGED_CODE();

    PNP_ASSERT(DriverObject);
    PNP_ASSERT(KeyHandle);
    PNP_ASSERT(ServiceName);

    status = STATUS_SUCCESS;
    if (DriverObject->DeviceObject == NULL &&
        ServiceName->Buffer &&
        !PnpIsAnyDeviceInstanceEnabled(ServiceName, NULL) &&
        !(DriverObject->Flags & (DRVO_REINIT_REGISTERED |
                                 DRVO_ROOT_DEVICE_REPORTED))) {
        PnpDriverLoadingFailed(KeyHandle, NULL);
        status = STATUS_PLUGPLAY_NO_DEVICE;
    }

    return status;
}

PDEVICE_OBJECT
PnpFindMountableDevice (
    __in PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This function will find the first mountable device in the stack.

Arguments:

    DeviceObject - Supplies a pointer to the PDO (bottom of the stack).

Return Value:

    A pointer to the first mountable device object in the stack
    walking up from the PDO. NULL otherwise.

--*/

{
    PDEVICE_OBJECT mountableDevice;
    KIRQL irql;

    PNP_ASSERT(DeviceObject != NULL);

    mountableDevice = DeviceObject;
    irql = KeAcquireQueuedSpinLock(LockQueueIoDatabaseLock);
    while (mountableDevice != NULL) {

        //
        // Look for a named device with a VPB.
        //

        if ((mountableDevice->Flags & DO_DEVICE_HAS_NAME) &&
            (mountableDevice->Vpb != NULL)) {

            //
            // Found a mountable device in the stack.
            //

            break;
        }

        mountableDevice = mountableDevice->AttachedDevice;
    }

    KeReleaseQueuedSpinLock(LockQueueIoDatabaseLock, irql);
    return mountableDevice;
}

VOID
PnpLockMountableDevice (
    __in PDEVICE_OBJECT DeviceObject
    )

/*++

Routine Description:

    This function will lock all mountable devices in the device stack.

Arguments:

    DeviceObject - Supplies a pointer to the PDO.

Return Value:

    None.

--*/

{
    PDEVICE_OBJECT attachedDevice;
    KIRQL irql;

    attachedDevice = DeviceObject;
    while (attachedDevice != NULL) {

        //
        // Look for a device with a VPB.
        //

        if (attachedDevice->Vpb != NULL) {

            //
            // Grab the device lock.  This will ensure that there are no mount
            // or verify operations in progress.
            //

            KeWaitForSingleObject(&attachedDevice->DeviceLock,
                                  Executive,
                                  KernelMode,
                                  FALSE,
                                  NULL);
        }

        irql = KeAcquireQueuedSpinLock(LockQueueIoDatabaseLock);
        attachedDevice = attachedDevice->AttachedDevice;
        KeReleaseQueuedSpinLock(LockQueueIoDatabaseLock, irql);
    }

    return;
}

VOID
PnpUnlockMountableDevice (
    __in PDEVICE_OBJECT DeviceObject
    )
/*++

Routine Description:

    This function will unlock all previously locked mountable devices
    in the device stack.

Arguments:

    DeviceObject - Supplies a pointer to the PDO.

Return Value:

    None.

--*/

{
    PDEVICE_OBJECT attachedDevice;
    KIRQL irql;

    attachedDevice = DeviceObject;
    while (attachedDevice != NULL) {

        //
        // Look for a device with a VPB.
        //

        if (attachedDevice->Vpb != NULL) {

            //
            // Release the device lock.  Mount or verify operations can
            // now make progress.
            //

            KeSetEvent(&attachedDevice->DeviceLock, IO_NO_INCREMENT, FALSE);
        }

        irql = KeAcquireQueuedSpinLock(LockQueueIoDatabaseLock);
        attachedDevice = attachedDevice->AttachedDevice;
        KeReleaseQueuedSpinLock(LockQueueIoDatabaseLock, irql);
    }

    return;
}

PDEVICE_OBJECT
PnpMarkDeviceForRemove (
    __in PDEVICE_OBJECT DeviceObject,
    __in LOGICAL Set,
    __out PPNP_LOCK_MOUNTABLE_DEVICE_CONTEXT Context
    )

/*++

Routine Description:

    This function will set or clear the VPB_REMOVE_PENDING flag
    on every mountable device in the stack.

Arguments:

    DeviceObject - Supplies a pointer to the PDO.

    Set - Specifies whether to set or clear the flag.

    Context - Supplies a pointer to the context to be used between Set
        and Clear.

Return Value:

    Pointer to the mounted device in the stack, otherwise pointer
    to the PDO.

--*/

{
    PDEVICE_OBJECT attachedDevice;
    KIRQL irql;
    PVPB vpb;


    if (Set != FALSE) {

        //
        // Initialize the context.
        //

        RtlZeroMemory(Context, sizeof(PNP_LOCK_MOUNTABLE_DEVICE_CONTEXT));
        Context->MountedDevice = DeviceObject;
    }

    //
    // Walk up the device stack looking for mountable devices.
    //

    attachedDevice = DeviceObject;
    while (attachedDevice != NULL) {
        if (attachedDevice->Vpb != NULL) {

            //
            // Found a mountable device.
            //

            IoAcquireVpbSpinLock(&irql);

            //
            // Set\clear the VPB_REMOVE_PENDING.
            //

            vpb = attachedDevice->Vpb;
            PNP_ASSERT(vpb != NULL);
            if (Set) {
                vpb->Flags |= VPB_REMOVE_PENDING;

                //
                // If the file system is already mounted,
                // remember the mounted device and corresponding
                // FS device.
                //

                if(vpb->Flags & VPB_MOUNTED) {
                    Context->MountedDevice = attachedDevice;
                    Context->FsDevice = vpb->DeviceObject;

                    //
                    // Increment the reference count on the FS
                    // device object to ensure it remains valid
                    // if the FS tears down the volume during
                    // the remove.
                    //

                    ObReferenceObject(Context->FsDevice);
                    IopIncrementDeviceObjectHandleCount(Context->FsDevice);

                }

            } else {
                vpb->Flags &= ~VPB_REMOVE_PENDING;
            }

            IoReleaseVpbSpinLock(irql);
        }

        //
        // Stop at the first device with a mounted
        // filesystem.
        //

        if (Set != FALSE && Context->FsDevice != NULL) {
            break;
        }

        irql = KeAcquireQueuedSpinLock(LockQueueIoDatabaseLock);
        attachedDevice = attachedDevice->AttachedDevice;
        KeReleaseQueuedSpinLock(LockQueueIoDatabaseLock, irql);
    }

    //
    // Return the FS device if there is one.
    //

    if (Context->FsDevice != NULL) {
        return Context->FsDevice;
    }

    return Context->MountedDevice;
}

VOID
PnpUpdateExtensionFlags (
    __in_ecount(Count) PDEVICE_OBJECT *DeviceObjects,
    __in ULONG Count,
    __in LOGICAL LockHeld,
    __in LOGICAL Set,
    __in ULONG Flags
    )

/*++

Routine Description:

    This function sets\clears the specified extension flags.

Arguments:

    DeviceObjects - Supplies a pointer to the array of devices to be updated.

    Count - Supplies the number of devices to be updated.

    LockHeld - TRUE if caller has already acquired LockQueueIoDatabaseLock.

    Set - TRUE if the specified flags should be set, otherwise specified flags
        are cleared.

    Flags - Supplies the flags to set\clear in the extension.

Return Value:

    None.

--*/

{
    PDEVICE_OBJECT attachedDevice;
    ULONG i;
    KIRQL irql;

    PNP_ASSERT(DeviceObjects != NULL);
    PNP_ASSERT(Count != 0);

    irql = 0;
    if (LockHeld == FALSE) {
        irql = KeAcquireQueuedSpinLock(LockQueueIoDatabaseLock);
    }

    for (i = 0; i < Count; i++) {
        for (attachedDevice = DeviceObjects[i];
             attachedDevice != NULL;
             attachedDevice = attachedDevice->AttachedDevice) {

            //
            // Update the flags as specified.
            //

            if (Set != FALSE) {
                attachedDevice->DeviceObjectExtension->ExtensionFlags |= Flags;

            } else {
                attachedDevice->DeviceObjectExtension->ExtensionFlags &= ~Flags;
            }
        }
    }

    if (LockHeld == FALSE) {
        KeReleaseQueuedSpinLock(LockQueueIoDatabaseLock, irql);
    }

    return;
}

LOGICAL
PnpIsAnyDeviceInUse (
    __in_ecount(Count) PDEVICE_OBJECT *DeviceObjects,
    __in ULONG Count,
    __out_opt PDEVICE_OBJECT *VetoingDevice
    )

/*++

Routine Description:

    This function checks if any of the devices have one or more open
    handles against them.
    Caller should have acquired LockQueueIoDatabaseLock prior to calling
    this routine.

Arguments:

    DeviceObjects - Supplies a pointer to the array of devices to be checked.

    Count - Supplies the number of devices to be checked.

    VetoingDevice - Supplies a pointer that recieves the first device with
        open handle.

Return Value:

    TRUE if any device has open handles against it, otherwise FALSE.

--*/

{
    ULONG i;
    ULONG referenced;
    PDEVICE_OBJECT device;

    PNP_ASSERT(DeviceObjects != NULL);
    PNP_ASSERT(Count != 0);

    device = NULL;
    referenced = 0;
    for (i = 0; (i < Count) && (referenced == 0); i++) {

        //
        // Check attachment chain for each device.
        //

        device = DeviceObjects[i];
        while (device != NULL) {
            referenced |= device->ReferenceCount;
            if (referenced != 0) {
                break;
            }

            device = device->AttachedDevice;
        }
    }

    //
    // Return the first device with open handle.
    //

    PNP_ASSERT((referenced == 0) || device != NULL);
    if (VetoingDevice != NULL) {
        *VetoingDevice = device;
    }

    return (referenced != 0)? TRUE : FALSE;
}

__drv_maxIRQL(DISPATCH_LEVEL)
VOID
PnpBugcheckPowerTimeout (
    __in ULONG Timeout
    )

/*++

Routine Description:

    This routine bugchecks the system 0x9F/4 when a power transition has hung
    for too long waiting for the PNP lock. A special triage data structure is
    prepared to aid in creating more actionalbe minidumps for OCA analysis.

Arguments:

    Timeout - Supplies the power transition watchdog timeout.

Return Value:

    None (never returns)

--*/

{

    PETHREAD Thread;
    TRIAGE_9F_PNP Triage;

    //
    // Capture the Pnp thread that is likely holding on to the lock which has
    // prevented the power transition from proceeding.
    //

    Thread = PnpDeviceEventThread;
    if (Thread == NULL) {
        Thread = PnpDeviceActionThread;
    }

    RtlZeroMemory(&Triage, sizeof(Triage));
    Triage.Signature = TRIAGE_9F_PNP_SIGNATURE;
    Triage.Revision = TRIAGE_9F_PNP_REVISION;
    Triage.CompletionQueue =
        (PTRIAGE_PNP_DEVICE_COMPLETION_QUEUE)&PnpDeviceCompletionQueue;

    Triage.DelayedWorkQueue =
        (PTRIAGE_EX_WORK_QUEUE)ExWorkerQueue;

    //
    // Bugcheck 0x9F/4 with the timeout, thread and triage structure.
    //

    KeBugCheckEx(DRIVER_POWER_STATE_FAILURE,
                 4,
                 Timeout,
                 (ULONG_PTR)Thread,
                 (ULONG_PTR)&Triage);

    return;
}

_Must_inspect_result_
NTSTATUS
PnpValidateMultiSz(
    __in_ecount_opt(MaxLength) PCZZWSTR InString,
    size_t MaxLength)

/*++

Routine Description:

    Validate that a MultiSz string is well formed.

Arguments:

    InString - MultiSz string to validate.

    MaxLength - Maximum length of the string in characters.

Return Value:

    NTSTATUS indicating success for failure.

--*/

{
    NTSTATUS status = STATUS_INVALID_PARAMETER;
    size_t multiSzLength = 0;
    size_t stringLength;

    PAGED_CODE();

    if (!InString) {
        goto Exit;
    }

    do {

        status = RtlStringCchLengthW(InString + multiSzLength,
                                     MaxLength - multiSzLength,
                                     &stringLength);

        if (!NT_SUCCESS(status)){
            goto Exit;
        }

        status = RtlSizeTAdd(multiSzLength,
                             stringLength,
                             &multiSzLength);

        if (!NT_SUCCESS(status)){
            goto Exit;
        }

        status = RtlSizeTAdd(multiSzLength,
                             1,
                             &multiSzLength);

        if (!NT_SUCCESS(status)){
            goto Exit;
        }

    } while (stringLength != 0);

Exit:

    return status;
}

_Must_inspect_result_
NTSTATUS
PnpAllocatePWSTR(
    __in_ecount_opt(MaxLength) PCWSTR InString,
    size_t MaxLength,   // Length of the input string
    ULONG Tag,
    __deref_out PWSTR* OutString
    )

/*++

Routine Description:

    Copy a string into a newly allocaged buffer.

Arguments:

    InString - String to copy.

    MaxLength - Maximum length of the string in characters.

    Tag - Pooltag to use for the allocation.

    OutString - Pointer to the new buffer.

Return Value:

    NTSTATUS indicating success for failure.

--*/

{
    NTSTATUS status = STATUS_SUCCESS;
    size_t stringLength;
    size_t bufferSize;

    PAGED_CODE();

    *OutString = NULL;

    if (!InString) {
        goto Exit;
    }

    status = RtlStringCchLengthW(InString,
                                 MaxLength,
                                 &stringLength);

    if (!NT_SUCCESS(status) || (stringLength == 0)) {
        goto Exit;
    }

    status = RtlSizeTMult(stringLength + 1,
                          sizeof(WCHAR),
                          &bufferSize);

    if (!NT_SUCCESS(status)) {
        goto Exit;
    }

    *OutString = (PWSTR) ExAllocatePoolWithTag(PagedPool,
                                               bufferSize,
                                               Tag);

    if (*OutString == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    status = RtlStringCbCopyW(*OutString,
                              bufferSize,
                              InString);

    if (!NT_SUCCESS(status)) {
        goto Exit;
    }

Exit:

    if (!NT_SUCCESS(status) && *OutString) {

        ExFreePoolWithTag(*OutString, Tag);
        *OutString = NULL;
    }

    return status;
}

NTSTATUS
PnpConcatPWSTR(
    size_t MaxLength,   // Max length of the concatinated string
    ULONG Tag,
    __deref_out PWSTR* OutString,
    size_t InputStringCount,
    ...)

/*++

Routine Description:

    Concatinate a number of string together into a new buffer.

Arguments:

    MaxLength - Maximum length of the combined strings in characters.

    Tag - Pooltag to use for the allocation.

    OutString - Pointer to the new buffer.

    InputStringCount Number of input strings.

Return Value:

    NTSTATUS indicating success for failure.

--*/

{
    NTSTATUS status = STATUS_SUCCESS;
    va_list ArgumentPtr;

    size_t totalLength = 1;
    PWSTR thisString = NULL;
    size_t thisStringLength;
    size_t bufferSize = 0;
    size_t i;

    PAGED_CODE();

    *OutString = NULL;

    va_start(ArgumentPtr, InputStringCount);

    for (i = 0; i < InputStringCount; ++i) {

        thisString = va_arg(ArgumentPtr, PWSTR);

        if (thisString) {

            status = RtlStringCchLengthW(thisString,
                                         MaxLength,
                                         &thisStringLength);

            if (!NT_SUCCESS(status)) {
                goto Exit;
            }

            totalLength += thisStringLength;
        }
    }

    va_end(ArgumentPtr);

    if (totalLength > MaxLength) {

        status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    status = RtlSizeTMult(totalLength,
                          sizeof(WCHAR),
                          &bufferSize);

    if (!NT_SUCCESS(status)) {
        goto Exit;
    }

    *OutString = (PWSTR) ExAllocatePoolWithTag(PagedPool,
                                               bufferSize,
                                               Tag);

    if (*OutString == NULL) {

        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    **OutString = L'\0';

    va_start(ArgumentPtr, InputStringCount);

    for (i = 0; i < InputStringCount; ++i) {

        thisString = va_arg(ArgumentPtr, PWSTR);

        if (thisString) {

            status = RtlStringCbCatW(
                *OutString,
                bufferSize,
                thisString);

            if (!NT_SUCCESS(status)) {
                goto Exit;
            }
        }
    }

    va_end(ArgumentPtr);

Exit:

    if (!NT_SUCCESS(status) && *OutString) {

        ExFreePoolWithTag(*OutString, Tag);
        *OutString = NULL;
    }

    return status;
}

_Must_inspect_result_
NTSTATUS
PnpGetMultiSzLength(
    __in_ecount(MaxLength) PCZZWSTR MultiSz,
    size_t MaxLength,
    size_t *MultiSzLength
    )

/*++

Routine Description:

    Get the length of a MultiSz string in characters, inluding the terminating NULL.

Arguments:

    MultiSz - Input MultiSz

    MaxLength - Maximum length of the MultiSz in characters.

    Length of the MultiSz in characters, inluding the terminating NULL.

Return Value:

    NTSTATUS indicating success for failure.

--*/

{
    NTSTATUS status = STATUS_SUCCESS;
    size_t stringLength;

    PAGED_CODE();

    *MultiSzLength = 0;

    do
    {
        status = RtlStringCchLengthW(MultiSz + *MultiSzLength,
                                     MaxLength - *MultiSzLength,
                                     &stringLength);

        if (!NT_SUCCESS(status)) {
            goto Exit;
        }

        status = RtlSizeTAdd(*MultiSzLength,
                             stringLength,
                             MultiSzLength);

        if (!NT_SUCCESS(status)) {
            goto Exit;
        }

        status = RtlSizeTAdd(*MultiSzLength,
                             1,
                             MultiSzLength);

        if (!NT_SUCCESS(status)){
            goto Exit;
        }

    } while (stringLength != 0);

Exit:

    return status;
}

BOOLEAN
PnpCompareMultiSz(
    __in PCZZWSTR MultiSz1,
    __in PCZZWSTR MultiSz2,
    __in BOOLEAN CaseInsensitive
    )

/*++

Routine Description:

    Compare two MultiSz strings.

Arguments:

    MultiSz1 - Strings to compare.

    MultiSz2 - Strings to compare.

Return Value:

    TRUE is the strings are the same otherwise FALSE.

--*/

{
    NTSTATUS status;
    size_t len1, len2;
    BOOLEAN ret = FALSE;

    PAGED_CODE();

    status = PnpGetMultiSzLength(MultiSz1,
                                 NTSTRSAFE_MAX_CCH,
                                 &len1);

    if (!NT_SUCCESS(status)){
        goto Exit;
    }

    status = PnpGetMultiSzLength(MultiSz2,
                                 NTSTRSAFE_MAX_CCH,
                                 &len2);

    if (!NT_SUCCESS(status)){
        goto Exit;
    }

    if (len1 != len2) {
        goto Exit;
    }

    ret = (RtlCompareUnicodeStrings(MultiSz1,
                                    len1,
                                    MultiSz2,
                                    len2,
                                    CaseInsensitive) == 0);

Exit:

    return ret;
}

_Must_inspect_result_
NTSTATUS
PnpAllocateMultiSZ(
    __in_ecount_opt(MaxLength) PCZZWSTR InString,
    size_t MaxLength,   // size of the input buffer
    ULONG Tag,
    __deref_out PZZWSTR* OutString
    )

/*++

Routine Description:

    Copy a MultiSz into a newly allocaged buffer.

Arguments:

    InString - MultiSz to copy.

    MaxLength - Maximum length of the MultiSz in characters.

    Tag - Pooltag to use for the allocation.

    OutString - Pointer to the new buffer.

Return Value:

    NTSTATUS indicating success for failure.

--*/

{
    NTSTATUS status = STATUS_SUCCESS;
    size_t multiSzLength;
    size_t bufferSize;

    PAGED_CODE();

    *OutString = NULL;

    if (!InString) {
        goto Exit;
    }

    status = PnpGetMultiSzLength(InString,
                                 MaxLength,
                                 &multiSzLength);

    if (!NT_SUCCESS(status)){
        goto Exit;
    }

    //
    // Only allocate non-empty MultiSz
    //

    if (multiSzLength <= 2) {
        goto Exit;
    }

    status = RtlSizeTMult(multiSzLength,
                          sizeof(WCHAR),
                          &bufferSize);

    if (!NT_SUCCESS(status)){
        goto Exit;
    }

    *OutString = (PWSTR) ExAllocatePoolWithTag(PagedPool,
                                               bufferSize,
                                               Tag);

    if (*OutString == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    RtlCopyMemory(
        *OutString,
        InString,
        bufferSize);

    // Make sure the string is double null terminated.

    (*OutString)[multiSzLength-1] = '\0';
    (*OutString)[multiSzLength-2] = '\0';

Exit:

    return status;
}

_Must_inspect_result_
NTSTATUS
PnpCopyDevProperty(
    __in const DEVPROPERTY *InProperty,
    ULONG Tag,
    __in PDEVPROPERTY OutProperty
    )

/*++

Routine Description:

    Perform deep copy of a DEVPROPERTY structure.

Arguments:

    InProperty - DEVPROPERTY to copy.

    Tag - Pooltag to use for the allocation.

    OutProperty - DEVPROPERTY structure being copied to.

Return Value:

    NTSTATUS indicating success for failure.

--*/

{
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();

    *OutProperty = *InProperty;
    OutProperty->CompKey.LocaleName = NULL;
    OutProperty->Buffer = NULL;

    if (InProperty->CompKey.LocaleName) {

        status = PnpAllocatePWSTR(InProperty->CompKey.LocaleName,
                                  NTSTRSAFE_MAX_CCH,
                                  Tag,
                                  (PWSTR*)&OutProperty->CompKey.LocaleName);
    }

    if (!NT_SUCCESS(status)) {
        goto Exit;
    }

    if (OutProperty->BufferSize > 0)
    {
        OutProperty->Buffer = ExAllocatePoolWithTag(PagedPool,
                                                    OutProperty->BufferSize,
                                                    Tag);

        if (!OutProperty->Buffer)
        {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit;
        }
    }

    RtlCopyMemory(OutProperty->Buffer,
                  InProperty->Buffer,
                  OutProperty->BufferSize);

Exit:

    if (!NT_SUCCESS(status)) {

        PnpFreeDevProperty(OutProperty, Tag);

        RtlZeroMemory(OutProperty, sizeof(DEVPROPERTY));
    }

    return status;
}

VOID
PnpFreeDevProperty(
    __in PDEVPROPERTY Property,
    ULONG Tag
    )

/*++

Routine Description:

    Free a DEVPROPERTY.

Arguments:

    Property - DEVPROPERTY structures to free.

    Tag - Pooltag to use for the allocation.

Return Value:

    None

--*/

{
    if (Property->CompKey.LocaleName) {

        ExFreePoolWithTag((PWSTR) Property->CompKey.LocaleName, Tag);
    }

    if (Property->Buffer) {

        ExFreePoolWithTag(Property->Buffer, Tag);
    }
}

_Must_inspect_result_
NTSTATUS
PnpCopyDevPropertyArray(
    ULONG InPropertyCount,
    __in_ecount_opt(InPropertyCount) const DEVPROPERTY *InProperties,
    ULONG Tag,
    __out PULONG OutPropertyCount,
    __deref_out_ecount(*OutPropertyCount) PDEVPROPERTY* OutProperties
    )

/*++

Routine Description:

    Perform deep copy of a array of DEVPROPERTY structures into an
    allocated buffer.

Arguments:

    InPropertyCount - Number of elements in the array;

    InProperties - Array of DEVPROPERTY structures to copy.

    Tag - Pooltag to use for the allocation.

    OutPropertyCount - Number of elements in the out array.
        Always the same as the number of input properties on success.

    OutProperties - Newly allocated array of DEVPROPERTY structures.

Return Value:

    NTSTATUS indicating success for failure.

--*/

{
    NTSTATUS status = STATUS_SUCCESS;
    size_t bufferSize;
    ULONG i;

    PAGED_CODE();

    *OutPropertyCount = 0;
    *OutProperties = NULL;

    if (!InProperties || (InPropertyCount == 0)) {
        goto Exit;
    }

    status = RtlSizeTMult(InPropertyCount,
                          sizeof(DEVPROPERTY),
                          &bufferSize);

    if (!NT_SUCCESS(status)) {

        goto Exit;
    }

    *OutProperties = ExAllocatePoolWithTag(PagedPool,
                                           bufferSize,
                                           Tag);

    if (*OutProperties == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    RtlZeroMemory(*OutProperties, bufferSize);

    for (i = 0; i < InPropertyCount; ++i) {

        status = PnpCopyDevProperty(InProperties + i, Tag, *OutProperties + i);

        if (!NT_SUCCESS(status))  {

            goto Exit;
        }

        ++(*OutPropertyCount);
    }

Exit:

    if (!NT_SUCCESS(status) && *OutProperties) {
        // Cleanup on failure

        for (i = 0; i < *OutPropertyCount; ++i) {

            ExFreePoolWithTag((*OutProperties + i)->Buffer, Tag);
        }

        ExFreePoolWithTag(*OutProperties, Tag);

        *OutPropertyCount = 0;
        *OutProperties = NULL;
    }

    return status;
}


VOID
PnpFreeDevPropertyArray(
    ULONG PropertyCount,
    __in_ecount_opt(PropertyCount) PDEVPROPERTY Properties,
    ULONG Tag
    )

/*++

Routine Description:

    Free an DEVPROPERTY array allocated with PnPCopyDevPropertyArray.

Arguments:

    PropertyCount - Number of elements in the array;

    Properties - Array of DEVPROPERTY structures to free.

    Tag - Pooltag to use for the allocation.


Return Value:

    None

--*/

{
    ULONG i;

    PAGED_CODE();

    for (i = 0; i < PropertyCount; ++i) {

        PnpFreeDevProperty(Properties + i, Tag);
    }

    ExFreePoolWithTag(Properties, Tag);
}

NTSTATUS
PnpUnicodeStringToWstr(
    __deref_out PWSTR* OutString,
    __out_opt PULONG OutStringMaximumSize,
    __in const  PUNICODE_STRING InUnicodeString
    )

/*++

Routine Description:

    Provide a null-terminated PCWSTR from a PUNICODE_STRING.

Arguments:

    OutString - The required PCWSTR obtained from the given UNICODE_STRING.

    OutStringMaximumSize - The maximum size (in bytes) of the OutString output.  Callers should not touch
        memory more than this many bytes past the value returned in OutString.  NOTE: This does not represent
        the length of the string contained in OutString.

    InUnicodeString - Address of the UNICODE_STRING structure containing the desired string.

Return Value:

    NTSTATUS indicating if we can successfully provide the output string

--*/

{
    PWSTR bufferalloc;
    ULONG buffersize;
    NTSTATUS status = STATUS_SUCCESS;

    PAGED_CODE();

    if (OutString == NULL || InUnicodeString == NULL) {
        status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    if ((InUnicodeString->MaximumLength < sizeof(WCHAR)) ||
        (InUnicodeString->Buffer == NULL)) {

        PNP_ASSERT(InUnicodeString->Length == 0);

        *OutString = NULL;
        if (OutStringMaximumSize != NULL) {
            *OutStringMaximumSize = 0;
        }
        goto Exit;
    }

    if (InUnicodeString->Length > InUnicodeString->MaximumLength) {
        status = STATUS_INVALID_PARAMETER;
        goto Exit;
    }

    if ((InUnicodeString->Length == 0) ||
        (InUnicodeString->Buffer[0] == UNICODE_NULL)) {

        //
        // Passed an empty string. If the buffer provides an empty string then use it
        // otherwise allocate an empty string
        //

        if (InUnicodeString->Buffer[0] == UNICODE_NULL) {

            *OutString = InUnicodeString->Buffer;

            if (OutStringMaximumSize != NULL) {
                *OutStringMaximumSize = InUnicodeString->MaximumLength;
            }

        } else {

            buffersize = sizeof(WCHAR);
            bufferalloc = ExAllocatePool(PagedPool, buffersize);

            if (bufferalloc == NULL) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                goto Exit;
            }

            bufferalloc[0] = UNICODE_NULL;

            *OutString = bufferalloc;

            if (OutStringMaximumSize != NULL) {
                *OutStringMaximumSize = buffersize;
            }
        }

        goto Exit;

    }

    if ((InUnicodeString->Length == InUnicodeString->MaximumLength) &&
        (InUnicodeString->Buffer[InUnicodeString->Length/sizeof(WCHAR) - 1] == UNICODE_NULL)) {

        //
        // The Length equals the maximum length and includes the NULL.
        //

        *OutString = InUnicodeString->Buffer;

        if (OutStringMaximumSize != NULL) {
            *OutStringMaximumSize = InUnicodeString->MaximumLength;
        }

    } else if ((InUnicodeString->Length <= (InUnicodeString->MaximumLength - sizeof(WCHAR))) &&
               ((InUnicodeString->Buffer[InUnicodeString->Length/sizeof(WCHAR) - 1] == UNICODE_NULL) ||
                (InUnicodeString->Buffer[InUnicodeString->Length/sizeof(WCHAR)]     == UNICODE_NULL))) {

        //
        // There is space for the NULL and it exists at Length characters or
        // immediately after.
        //

        *OutString = InUnicodeString->Buffer;

        if (OutStringMaximumSize != NULL) {
            *OutStringMaximumSize = InUnicodeString->MaximumLength;
        }
    } else {

        //
        // The string is not null terminated or the buffer doesn't support it.
        // Allocate a null terminated string
        //

        buffersize = ((ULONG)InUnicodeString->Length) + sizeof(WCHAR);
        bufferalloc = ExAllocatePool(PagedPool, buffersize);

        if (bufferalloc == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit;
        }

        RtlCopyMemory(bufferalloc,
                      InUnicodeString->Buffer,
                      InUnicodeString->Length);

        bufferalloc[InUnicodeString->Length/sizeof(WCHAR)] = UNICODE_NULL;

        *OutString = bufferalloc;

        if (OutStringMaximumSize != NULL) {
            *OutStringMaximumSize = buffersize;
        }
    }

Exit:

    return status;
}


VOID
PnpUnicodeStringToWstrFree(
    __in_opt PWSTR InFreeString,
    __in_opt const PUNICODE_STRING InUnicodeString
    )

/*++

Routine Description:

    Free a null-terminated PCWSTR that may have been allocated using PiUnicodeStringToWstr.

Arguments:

    InFreeString - The PCWSTR that may need to be freed.

    InUnicodeString - Address of the UNICODE_STRING structure that it was derived from.

Return Value:

    None

--*/

{

    PAGED_CODE();

    if (InFreeString == NULL || InUnicodeString == NULL) {
        goto Exit;
    }

    if (InUnicodeString->MaximumLength == 0 || InUnicodeString->Buffer == NULL)
    {
        goto Exit;
    }

    if (InUnicodeString->Buffer == InFreeString)
    {
        goto Exit;
    }

    ExFreePool(InFreeString);

Exit:

    return;
}



//
// GUID utility routines.
//

_Must_inspect_result_
BOOLEAN
PnpIsValidGuidString(
    __in PCWSTR pGuidString
    )
{
    NTSTATUS Status = STATUS_SUCCESS;
    UNICODE_STRING UnicodeString;
    ULONG GuidStringLen;
    GUID Guid;

    PAGED_CODE();

    Status = RtlInitUnicodeStringEx(&UnicodeString, pGuidString);

    if (!NT_SUCCESS(Status)) {
        return FALSE;
    }

    GuidStringLen = UnicodeString.MaximumLength / sizeof(WCHAR);

    if (GuidStringLen != GUID_STRING_LEN) {
        return FALSE;
    }

    Status = RtlGUIDFromString(&UnicodeString, &Guid);

    if (!NT_SUCCESS(Status)) {
        return FALSE;
    }

    return TRUE;
}



_Must_inspect_result_
BOOLEAN
PnpIsNullGuid(
    __in LPCGUID Guid
    )
{
    PAGED_CODE();

    return (BOOLEAN)(RtlCompareMemory(&NullGuid, Guid, sizeof(GUID)) == sizeof(GUID));
}



_Must_inspect_result_
BOOLEAN
PnpIsNullGuidString(
    __in PCWSTR GuidString
    )
{
    PAGED_CODE();

    return (wcscmp(L"{00000000-0000-0000-0000-000000000000}", GuidString) == 0);
}



NTSTATUS
PnpStringFromGuid(
    __in LPCGUID Guid,
    __out_ecount(GuidStringLength) PWSTR GuidString,
    __in ULONG GuidStringLength
    )
{
    PAGED_CODE();

    return RtlStringCchPrintfExW(GuidString,
                                 GuidStringLength,
                                 NULL, NULL,
                                 STRSAFE_NULL_ON_FAILURE,
                                 L"{%08lx-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
                                 Guid->Data1,
                                 Guid->Data2,
                                 Guid->Data3,
                                 Guid->Data4[0],
                                 Guid->Data4[1],
                                 Guid->Data4[2],
                                 Guid->Data4[3],
                                 Guid->Data4[4],
                                 Guid->Data4[5],
                                 Guid->Data4[6],
                                 Guid->Data4[7]);
}



_Must_inspect_result_
NTSTATUS
PnpGetCallerSessionId(
    __out PULONG SessionId
    )

/*++

Routine Description:

    This routine retrieves the session id of the caller.

Parameters:

    SessionId � Returns the session id.

Return Value:

    NTSTATUS indicating success or failure.

--*/

{
    NTSTATUS status = STATUS_SUCCESS;
    SECURITY_SUBJECT_CONTEXT securityContext;

    PAGED_CODE();

    NT_ASSERT(SessionId != NULL);
    NT_ASSERT(!IS_SYSTEM_THREAD(PsGetCurrentThread()));

    SeCaptureSubjectContext(&securityContext);

    status = SeQuerySessionIdToken(SeQuerySubjectContextToken(&securityContext),
                                   SessionId);

    SeReleaseSubjectContext(&securityContext);

    return status;
}

NTSTATUS
PnpCheckDeviceIdsChanged(
    __in PDEVICE_NODE DeviceNode,
    __in_opt HANDLE DeviceInstanceKey,
    __in_bcount_opt(IdListSize) PWSTR IdList,
    __in ULONG IdListSize,
    __in BOOLEAN CheckHardwareIds,
    __out PBOOLEAN IdsChanged
    )
{
    PWSTR newID;
    PWSTR oldId;
    CM_REG_PROP property;
    ULONG propertySize;
    PWSTR propertyString;
    ULONG propertyType;
    NTSTATUS status;
    PWSTR string1;
    ULONG string1Length;
    PWSTR string2;
    ULONG string2Length;
    HANDLE tempDeviceInstanceKey;
    PWSTR tempIdList;

    PNP_ASSERT(DeviceNode != NULL);
    PNP_ASSERT((IdList != NULL) || (IdListSize == 0));
    PNP_ASSERT(IdsChanged != NULL);

    property = (CheckHardwareIds) ?
        CM_REG_PROP_HARDWAREID : CM_REG_PROP_COMPATIBLEIDS;
    propertyString = (CheckHardwareIds) ? L"Hardware IDs" : L"Compatible IDs";
    status = STATUS_SUCCESS;
    tempDeviceInstanceKey = NULL;
    tempIdList = NULL;

    //
    // Initialize output parameters.
    //

    *IdsChanged = FALSE;

    //
    // Open up a handle to this device's instance key if one wasn't provided.
    //
    if (DeviceInstanceKey == NULL) {
        status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                     CM_KEY_TYPE_DEVICE_INSTANCE,
                                     0,
                                     KEY_ALL_ACCESS,
                                     FALSE,
                                     &tempDeviceInstanceKey,
                                     NULL);

        if (!NT_SUCCESS(status)) {
            goto Exit;
        }

        DeviceInstanceKey = tempDeviceInstanceKey;
    }

    //
    // Start with an intial guess of how large the IDs might need to be.
    //

    propertySize = INITIAL_HARDWARE_ID_LIST_SIZE;

    tempIdList = ExAllocatePool(PagedPool, propertySize);

    if (tempIdList == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto Exit;
    }

    //
    // Retrieve the device's current ids.
    //

    status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                 PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                 DeviceInstanceKey,
                                 property,
                                 &propertyType,
                                 (PUCHAR)tempIdList,
                                 &propertySize,
                                 0);

    if (status == STATUS_BUFFER_TOO_SMALL) {

        //
        // Allocate a buffer that is large enough to get the property.
        //

        PNP_ASSERT(tempIdList != NULL);
        ExFreePool(tempIdList);

        tempIdList = ExAllocatePool(PagedPool, propertySize);

        if (tempIdList == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto Exit;
        }

        //
        // Try to retrieve the property again.
        //

        status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                     PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                     DeviceInstanceKey,
                                     property,
                                     &propertyType,
                                     (PUCHAR)tempIdList,
                                     &propertySize,
                                     0);

        PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);
    }

    if (NT_SUCCESS(status)) {

        if (propertyType == REG_MULTI_SZ) {

            PNP_ASSERT(*IdsChanged == FALSE);

            if (IdList == NULL) {

                //
                // There currently is no id list, but the device has one stored
                // for it (if we got here then the ids were successfully
                // retrieved).  Return that the ids have changed.
                //

                *IdsChanged = TRUE;

            } else if (IdListSize != propertySize) {

                //
                // If the size of the current list of ids does not match the size
                // of what was previously saved for this device, return that the
                // ids have changed.
                //

                *IdsChanged = TRUE;

            } else {

                for (string1 = IdList, string2 = tempIdList;
                     *string1 && *string2;
                     string1 += string1Length, string2 += string2Length) {

                    string1Length = (ULONG) wcslen(string1) + 1;
                    string2Length = (ULONG) wcslen(string2) + 1;

                    if ((string1Length != string2Length) ||
                        (RtlCompareUnicodeStrings(string1,
                                                  string1Length,
                                                  string2,
                                                  string2Length,
                                                  TRUE) != 0)) {

                        //
                        // The old and new list are not identical.
                        //

                        *IdsChanged = TRUE;
                        break;
                    }
                }

                if ((*IdsChanged == FALSE) && (*string1 || *string2)) {

                    //
                    // The old and new list are not identical.
                    //

                    *IdsChanged = TRUE;
                }
            }

            //
            // If the old and new list are not identical, print info to the
            // debugger and exit.
            //

            if (*IdsChanged) {

                IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                             "PnpCheckDeviceIdsChanged: %ws have changed for %wZ\n",
                             propertyString,
                             PnpGetDeviceInstancePath(DeviceNode)));

                IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                             "Old %ws are:\n",
                             propertyString));

                for (oldId = tempIdList; (((oldId - tempIdList) * sizeof(WCHAR)) < propertySize) && *oldId; oldId += wcslen(oldId) + 1) {
                    IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                                 "%ws\n",
                                 oldId));
                }

                IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                             "New %ws are:\n",
                             propertyString));

                if (IdList != NULL) {
                    for (newID = IdList; *newID; newID += wcslen(newID) + 1) {
                        IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                                     "%ws\n",
                                     newID));
                    }
                } else {
                    IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL, "(NULL)\n"));
                }

                goto Exit;
            }

        } else {
            IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                         "PnpCheckDeviceIdsChanged: %ws property was incorrect type for %wZ\n",
                         propertyString,
                         PnpGetDeviceInstancePath(DeviceNode)));

            status = STATUS_UNSUCCESSFUL;
            goto Exit;
        }
    } else if (status == STATUS_NOT_FOUND) {

        status = STATUS_SUCCESS;

        if (IdList == NULL) {

            //
            // There is currently no id list and none was saved for this device.
            // The ids have not changed.
            //

            PNP_ASSERT(*IdsChanged == FALSE);

            goto Exit;
        } else {

            //
            // There is currently a id list but none was saved for this device.
            // The ids have changed.
            //

            *IdsChanged = TRUE;

            //
            // Print an error to the debugger.
            //
            IopDbgPrint((IOP_ENUMERATION_ERROR_LEVEL,
                         "PnpCheckDeviceIdsChanged: %ws have changed for %wZ\n",
                         propertyString,
                         PnpGetDeviceInstancePath(DeviceNode)));

            IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                         "Old %ws are:\n(NULL)\n",
                         propertyString));

            IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                         "New %ws are:\n",
                         propertyString));

            for (newID = IdList; *newID; newID += wcslen(newID) + 1) {
                IopDbgPrint((IOP_ENUMERATION_INFO_LEVEL,
                             "%ws\n",
                             newID));
            }

            goto Exit;
        }
    } else {

        //
        // There was some error retrieving the ids.
        //

        goto Exit;
    }

  Exit:

    if (tempIdList != NULL) {
        ExFreePool(tempIdList);
    }

    if (tempDeviceInstanceKey != NULL) {
        ZwClose(tempDeviceInstanceKey);
    }

    return status;
}

VOID
PnpInitializeSessionId(
    __in PDEVICE_NODE DeviceNode,
    __in HANDLE InstanceKey,
    __in BOOLEAN NewDevice
    )
/*++

Routine Description:

    This routine intializes the session ID for a device node.

    This involves checking the parent node for a session property and
    propagating that if present; otherwise any existing session property is
    cleared.

    This means that any time a device node is started it is guaranteed to have
    either the same session ID as the parent *or* no session ID.

    Drivers and applications are free to change this property at any point after
    this routine has been called but any changes will not be propagated to
    existing children until they are restarted.

Arguments:

    DeviceNode - Pointer to device node on which to initialize session ID.

    InstanceKey - Open handle to instance key for device.

    NewDevice - Indicates whether this device was just created or not.

--*/
{
    ULONG deviceSessionId = INVALID_SESSION;
    ULONG oldSessionId = INVALID_SESSION;
    ULONG propertySize = sizeof(oldSessionId);

    PAGED_CODE();

    if (DeviceNode->Parent &&
        DeviceNode->Parent->PhysicalDeviceObject->DeviceObjectExtension->ExtensionFlags & DOE_SESSION_DEVICE) {

            deviceSessionId = IopGetSessionIdFromPDO(DeviceNode->Parent->PhysicalDeviceObject);
    }

    PNP_ASSERT(PnpGetDeviceInstancePath(DeviceNode)->Buffer != NULL);

    //
    // When the device is newly created, there is no existing session ID that
    // needs to be cleared if the device will have no session ID.
    //

    if ((NewDevice != FALSE) && (deviceSessionId == INVALID_SESSION)) {
        return;
    }

    //
    // Set a new session ID.  If the session ID is invalid, clear any existing
    // session ID property.  If the device is new, it will have no existing
    // session ID property.  Use the write through flag to turn off get before
    // set behavior of the datamodel, since the get in this case will be wasted.
    //

    (VOID) _PnpSetObjectProperty(PiPnpRtlCtx,
                                 PnpGetDeviceInstancePath(DeviceNode)->Buffer,
                                 PNP_OBJECT_TYPE_DEVICE,
                                 InstanceKey,
                                 NULL,
                                 &DEVPKEY_Device_SessionId,
                                 (deviceSessionId == INVALID_SESSION) ? DEVPROP_TYPE_EMPTY : DEVPROP_TYPE_UINT32,
                                 (deviceSessionId == INVALID_SESSION) ? NULL : (PUCHAR)&deviceSessionId,
                                 (deviceSessionId == INVALID_SESSION) ? 0 : propertySize,
                                 (NewDevice != FALSE) ? PIPNPRTL_FLAG_WRITE_THROUGH : 0);
}

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
    )

/*++

Routine Description:

    Get an  object property and allocate the buffer needed.
    The caller must free PropertyBuffer ith ExFreePoolWithTag().

Arguments:

    Tag - Pooltag to use for the allocation.

    InitialBufferSize - Initial buffer size to use when attempting
      to fetch the property.

    ObjectName - Name of the object.

    ObjectType - Pnp object type of the object.

    ObjectBaseKey - Optional handle to the object's base key.

    PropertyLocaleName - Property locale.

    PropertyKey - Key for the property to retrieve.

    PropertyType - Returns the type of the retrieved property.

    PropertyBuffer - Returns a pointer to the allocated buffer
        that contains the property data.

    PropertySize - Optional parameter to receive the size of the property.

    Flags - Flags to pass to _PnpGetObjectProperty.

Return Value:

    NTSTATUS indicating success for failure.

--*/

{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG bufferSize = 0, propertySize = InitialBufferSize;

    PAGED_CODE();

    *PropertyBuffer = NULL;

    for ( ; ; ) {

        if (propertySize > bufferSize) {

            bufferSize = propertySize;

            if (*PropertyBuffer) {

                ExFreePoolWithTag(*PropertyBuffer, Tag);
            }

            *PropertyBuffer = ExAllocatePoolWithTag(PagedPool, bufferSize, Tag);
            if (!*PropertyBuffer) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                break;
            }
        }

        propertySize = 0;

        status = _PnpGetObjectProperty(PiPnpRtlCtx,
                                       ObjectName,
                                       ObjectType,
                                       ObjectBaseKey,
                                       PropertyLocaleName,
                                       PropertyKey,
                                       PropertyType,
                                       (PUCHAR)*PropertyBuffer,
                                       bufferSize,
                                       &propertySize,
                                       Flags);

        if (status != STATUS_BUFFER_TOO_SMALL) {
            break;
        }
    }

    if (NT_SUCCESS(status)) {

        if (PropertySize) {

            *PropertySize = propertySize;
        }

        if (propertySize == 0) {

            ExFreePoolWithTag(*PropertyBuffer, Tag);
            *PropertyBuffer = NULL;
        }
    } else {

        if (*PropertyBuffer) {

            ExFreePoolWithTag(*PropertyBuffer, Tag);
            *PropertyBuffer = NULL;
        }
    }

    return status;
}

NTSTATUS
PnpConvertDevpropcompkeyArrayToString(
    __in_ecount(CompKeyArrayLength) const DEVPROPCOMPKEY * CompKeyArray,
    __in ULONG CompKeyArrayLength,
    __out_bcount_opt(CompKeyArrayStringSize) PWSTR CompKeyArrayString,
    __in ULONG CompKeyArrayStringSize,
    __out_opt PULONG RequiredSize
    )
{
    NTSTATUS status = STATUS_SUCCESS;
    ULONG tempSize;
    ULONG index;
    ULONG devpropcompkeySize;
    ULONG originalCompKeyArrayStringSize;

    PAGED_CODE();

    NT_ASSERT(CompKeyArray != NULL);
    NT_ASSERT(CompKeyArrayLength > 0);
    NT_ASSERT((CompKeyArrayString != NULL) || (CompKeyArrayStringSize == 0));

    originalCompKeyArrayStringSize = CompKeyArrayStringSize;

    //
    // Initialize tempSize to already include the null terminator.
    //
    tempSize = sizeof(UNICODE_NULL);

    if (CompKeyArrayStringSize >= sizeof(UNICODE_NULL)) {
        *CompKeyArrayString = UNICODE_NULL;
    }

    for (index = 0; index < CompKeyArrayLength; index++) {
        status = ConvertDevpropcompkeyToString((const PDEVPROPCOMPKEY)(&CompKeyArray[index]),
                                               CompKeyArrayString,
                                               CompKeyArrayStringSize,
                                               &devpropcompkeySize);

        if ((!NT_SUCCESS(status)) && (status != STATUS_BUFFER_TOO_SMALL)) {
            goto exit;
        }

        //
        // tempSize already accounts for the single null terminator needed so
        // subtract off the one in the required size returned for this individual
        // DEVPROPCOMPKEY.
        //
        status = RtlULongAdd(tempSize,
                             (devpropcompkeySize - sizeof(UNICODE_NULL)),
                             &tempSize);

        if (!NT_SUCCESS(status)) {
            goto exit;
        }

        if (CompKeyArrayStringSize >= (devpropcompkeySize - sizeof(UNICODE_NULL))) {
            CompKeyArrayStringSize -= (devpropcompkeySize - sizeof(UNICODE_NULL));
            CompKeyArrayString += (devpropcompkeySize - sizeof(UNICODE_NULL)) / sizeof(WCHAR);
        }
    }

    status = STATUS_SUCCESS;

    if (ARGUMENT_PRESENT(RequiredSize)) {
        *RequiredSize = tempSize;
    }

    if (tempSize > originalCompKeyArrayStringSize) {
        status = STATUS_BUFFER_TOO_SMALL;
        goto exit;
    }

  exit:

    return status;
}

extern
KINLINE
VOID
PipUpdatePostStartCharacteristics(
    __in PDEVICE_OBJECT PhysicalDeviceObject
    )
/*++

Routine Description:

    This routine ensures any characteristics that may be added to a device
    object during start get propagated throughout the stack.

Arguments:

    PhysicalDeviceObject - Device object for devnode that was started.

--*/
{
    PDEVICE_OBJECT currentDevice;
    ULONG characteristics = 0;

    currentDevice = PhysicalDeviceObject;

    while (currentDevice != NULL) {
        characteristics |= currentDevice->Characteristics & PNP_POST_START_CHARACTERISTICS;
        currentDevice = currentDevice->AttachedDevice;
    }

    if (characteristics == 0) {
        return;
    }

    currentDevice = PhysicalDeviceObject;

    while (currentDevice != NULL) {
        currentDevice->Characteristics |= characteristics;
        currentDevice = currentDevice->AttachedDevice;
    }
}


NTSTATUS
IopReplaceSeperatorWithPound(
    __inout PUNICODE_STRING OutString,
    __in PUNICODE_STRING    InString
    )

/*++

Routine Description:

    This routine will copy a string from InString to OutString replacing any
    occurence of '\' or '/' with '#' as it goes.

Parameters:

    OutString - Supplies a pointer to a string which has already been
        initialised to have a buffer large enough to accomodate the string. The
        contents of this string will be over written

    InString - Supplies a pointer to the string to be munged

Return Value:

    Status code that indicates whether or not the function was successful.

Remarks:

    In place munging can be performed - ie. the In and Out strings can be the
    same.

--*/

{
    PWSTR pInPosition, pOutPosition;
    USHORT count;

    PAGED_CODE();

    PNP_ASSERT(InString);
    PNP_ASSERT(OutString);

    //
    // Ensure we have enough space in the output string
    //
    if (InString->Length > OutString->MaximumLength) {

        return STATUS_BUFFER_TOO_SMALL;
    }

    pInPosition = InString->Buffer;
    pOutPosition = OutString->Buffer;
    count = CB_TO_CWC(InString->Length);

    //
    // Traverse the in string copying and replacing all occurences of '\' or '/'
    // with '#'
    //
    while (count--) {

        if ( (*pInPosition == L'\\') ||
             (*pInPosition == L'/')) {

            *pOutPosition = L'#';
        } else {

            *pOutPosition = *pInPosition;
        }
        pInPosition++;
        pOutPosition++;
    }

    OutString->Length = InString->Length;

    return STATUS_SUCCESS;
}

NTSTATUS
PnpQueryInterface(
    __in  PDEVICE_OBJECT  DeviceObject,
    __in  CONST GUID *    InterfaceGuid,
    __in  USHORT          InterfaceVersion,
    __in  USHORT          InterfaceSize,
    __in_opt  PVOID       InterfaceSpecificData,
    __out_bcount(InterfaceSize) PINTERFACE      Interface
    )

/*++

Routine Description:

    Queries the specified device for the requested interface.

Arguments:

    DeviceObject -
        Specifies a device object in the stack to query.
        The query-interface irp will be sent to the top of the stack.

    InterfaceGuid -
        The GUID of the interface requested.

    InterfaceVersion -
        The version of the interface requested.

    InterfaceSize -
        The size of the interface requested.

    InterfaceSpecificData -
        The data provided when the interface is being requested.

    Interface -
        The place in which to return the interface.

Return Value:

    Returns STATUS_SUCCESS if the interface was retrieved, else an error.

--*/

{
    NTSTATUS            Status;
    KEVENT              Event;
    PDEVICE_OBJECT      deviceObject;
    IO_STATUS_BLOCK     IoStatusBlock;
    PIRP                Irp;
    PIO_STACK_LOCATION  IrpStackNext;

    PAGED_CODE();

    if (InterfaceSize < sizeof(INTERFACE)) {
        return STATUS_INVALID_PARAMETER;
    }

    RtlZeroMemory(Interface, InterfaceSize);
    Interface->Size = InterfaceSize;
    Interface->Version = InterfaceVersion;

    //
    // There is no file object associated with this Irp, so the event may be located
    // on the stack as a non-object manager object.
    //

    KeInitializeEvent(&Event, NotificationEvent, FALSE);


    //
    // Get a pointer to the topmost device object in the stack of devices.
    //
    deviceObject = IoGetAttachedDeviceReference(DeviceObject);

    Irp = IoBuildSynchronousFsdRequest(
        IRP_MJ_PNP,
        deviceObject,
        NULL,
        0,
        NULL,
        &Event,
        &IoStatusBlock);

    if (Irp) {
        Irp->RequestorMode = KernelMode;
        Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
        IrpStackNext = IoGetNextIrpStackLocation(Irp);

        //
        // Create an interface query out of the Irp.
        //
        IrpStackNext->MinorFunction = IRP_MN_QUERY_INTERFACE;
        IrpStackNext->Parameters.QueryInterface.InterfaceType = (GUID*)InterfaceGuid;
        IrpStackNext->Parameters.QueryInterface.Size = InterfaceSize;
        IrpStackNext->Parameters.QueryInterface.Version = InterfaceVersion;
        IrpStackNext->Parameters.QueryInterface.Interface = (PINTERFACE)Interface;
        IrpStackNext->Parameters.QueryInterface.InterfaceSpecificData = InterfaceSpecificData;

        Status = IoCallDriver(deviceObject, Irp);

        if (Status == STATUS_PENDING) {
            //
            // This waits using KernelMode, so that the stack, and therefore the
            // event on that stack, is not paged out.
            //
            KeWaitForSingleObject(&Event, Executive, KernelMode, FALSE, NULL);
            Status = IoStatusBlock.Status;
        }

    } else {
        Status = STATUS_INSUFFICIENT_RESOURCES;
    }

    ObDereferenceObject(deviceObject);

    return Status;

} // PpQueryInterface

NTSTATUS
PnpGetDeviceLocationStrings(
    __in        PDEVICE_NODE    DeviceNode,
    __deref_out_ecount(*DeviceLocationStringsLength) PZZWSTR *DeviceLocationStrings,
    __out       PULONG          DeviceLocationStringsLength
    )

/*++

Routine Description:

    This routine retrieves the device location string for a device node.

Arguments:

    DeviceNode -

        Specifies the device whose location strings are to be retrieved.

    DeviceLocationStrings -

        Returns a multi-sz string of the device location path strings, composed
        from the set of location strings returned from each device in the
        anscestry of the specified device.

    DeviceLocationStringsLength -

        Character count of the multi-sz string, including the NULL terminators.

Return Value:

    NTSTATUS code.

--*/

{
    NTSTATUS Status = STATUS_SUCCESS;
    PDEVICE_NODE deviceNode;
    ULONG  QueriedLocationStringsArraySize;
    PZZWSTR *QueriedLocationStrings = NULL;
    PULONG QueriedLocationStringsCount = NULL;
    PNP_LOCATION_INTERFACE LocationInterface;
    PZZWSTR TempMultiSz;
    ULONG TempMultiSzLength;
    BOOLEAN DeviceHasRootString = FALSE;
    PWCHAR p, pdlp, pdls;
    ULONG LongestStringLengthAtLevel;
    ULONG FinalStringLevel, i;
    BOOLEAN LocationStringIsBase, LocationStringIsRoot;
    BOOLEAN LocationPathIncludesBase, LocationPathIncludesBaseOnly, LocationPathIncludesBaseOnlyAtStart;
    ULONG DeviceHasBaseToLevel = ULONG_MAX, LocationPathSelectedBaseToLevel = ULONG_MAX;
    ULONG DeviceLocationPathMultiSzStringCount;
    ULONG DeviceLocationPathMultiSzLength;
    PWCHAR DeviceLocationPathMultiSz = NULL;
    ULONG CombinationsRemaining, CombinationEnumIndex;
    ULONG MultiSzIndex, MultiSzLookupIndex, StringLength;

    PAGED_CODE();

    //
    // Validate parameters.
    //
    if ((!ARGUMENT_PRESENT(DeviceNode)) ||
        (!ARGUMENT_PRESENT(DeviceLocationStrings)) ||
        (!ARGUMENT_PRESENT(DeviceLocationStringsLength))) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Initialize out parameters.
    //
    *DeviceLocationStrings = NULL;
    *DeviceLocationStringsLength = 0;

    //
    // We should NEVER have to query for the location of the root devnode.
    //
    if (DeviceNode == IopRootDeviceNode) {
        return STATUS_NO_SUCH_DEVICE;
    }

    //
    // Count the number of devnodes in the ancestry of the device to find out
    // the max number of location string sets we may have to query for.
    //
    QueriedLocationStringsArraySize = 0;

    for (deviceNode = DeviceNode;
         deviceNode != IopRootDeviceNode;
         deviceNode = deviceNode->Parent) {
        QueriedLocationStringsArraySize++;
    }

    PNP_ASSERT(QueriedLocationStringsArraySize > 0);

    //
    // Allocate and initialize an array of string buffer pointers for all
    // devices in the ancestry, and a corresponding array for the number of
    // strings retrieved for each device.
    //
    QueriedLocationStrings = (PZZWSTR*)ExAllocatePool(PagedPool,
        QueriedLocationStringsArraySize*sizeof(PZZWSTR));

    if (QueriedLocationStrings == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Clean0;
    }

    RtlZeroMemory(QueriedLocationStrings,
                  QueriedLocationStringsArraySize*sizeof(PZZWSTR));

    QueriedLocationStringsCount = (ULONG*)ExAllocatePool(PagedPool,
        QueriedLocationStringsArraySize*sizeof(ULONG));

    if (QueriedLocationStringsCount == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Clean0;
    }

    RtlZeroMemory(QueriedLocationStringsCount,
                  QueriedLocationStringsArraySize*sizeof(ULONG));

    //
    // Starting at the target device, walk up the devnode tree, retrieving the
    // set of location strings for all ancestors up to (but not including) the
    // root devnode.  We'll stop when we've reached the top of the tree, or when
    // some intermediate device has explicitly declared that the translation is
    // complete.
    //
    i = 0;

    //
    // Along the way, we count the total number of string combinations that can
    // be formed by taking a single string from the multi-sz list at each level.
    // This is simply the product of the number of string elements in the
    // multi-sz list at each level.
    //
    DeviceLocationPathMultiSzStringCount = 1;

    //
    // Also along the way, calculate the length (in chars) of the longest device
    // location path that can be generated from all combinations.  This is just
    // the sum of the longest string at each level (LongestStringLengthAtLevel
    // below), plus the necessary path component separator strings and NULL
    // terminating character.
    //
    //
    //            WARNING!! EXCESSIVELY VERBOSE COMMENT AHEAD!!
    //
    // NOTE: 27-Nov-2001 : Jim Cavalaris (jamesca)
    //
    //   We use this length to calculate the length of the buffer required to
    //   hold the entire multi-sz list of device location paths ASSUMING ALL
    //   GENERATED STRINGS ARE EQUALLY AS LONG.  This is an UPPER-BOUND, so we
    //   may end up allocating more memory than we actually need.
    //
    //   This should be ok, since in the ideal (and assumed to be most-common)
    //   case, only one location string will ever be returned per device - in
    //   which case this calculation will be exactly the size required.
    //
    //   In the event that multiple strings are returned per device, we should
    //   expect these strings to all be relatively short, and equal (or similar)
    //   in length.  In that case, this calculation will be exactly the size
    //   required (or similar).
    //
    //   We also currently do not expect to have to query many devices in the
    //   ancestry to complete the translation, so we shouldn't event expect too
    //   many combinations.
    //
    //   These are our assumptions, so consider yourself warned!  If any of
    //   these change such that we would need to allocate an excessive amount of
    //   memory, you will want to either run through the same algorithm the
    //   device location path generation code runs through just to calculate the
    //   exact size, or find some way to enumerate the device location path
    //   combinations incrementally.
    //
    DeviceLocationPathMultiSzLength = 0;

    for (deviceNode = DeviceNode;
         deviceNode != IopRootDeviceNode;
         deviceNode = deviceNode->Parent) {

        //
        // Query the device for the location interface.
        //
        Status = PnpQueryInterface(deviceNode->PhysicalDeviceObject,
                                  &GUID_PNP_LOCATION_INTERFACE,
                                  PNP_LOCATION_INTERFACE_VERSION,
                                  sizeof(PNP_LOCATION_INTERFACE),
                                  NULL,
                                  (PINTERFACE)&LocationInterface);

        if (!NT_SUCCESS(Status)) {
            //
            // If the location interface was not available for some device
            // before translation is complete, the entire operation is
            // unsuccessful.
            //
            goto Clean0;
        }

        //
        // If the location interface is supported, the required interface
        // routines must be supplied.
        //
        PNP_ASSERT(LocationInterface.InterfaceReference != NULL);
        PNP_ASSERT(LocationInterface.InterfaceDereference != NULL);
        PNP_ASSERT(LocationInterface.GetLocationString != NULL);

        if (LocationInterface.GetLocationString != NULL) {

            //
            // Initialize the location string.
            //
            TempMultiSz = NULL;

            //
            // Get the set of location strings for this device.
            //
            Status = LocationInterface.GetLocationString(
                LocationInterface.Context,
                &TempMultiSz);

            if (NT_SUCCESS(Status)) {
                //
                // If successful, the caller must have supplied us with a
                // buffer.
                //
                PNP_ASSERT(TempMultiSz != NULL);

                //
                // If not, the call was not really successful.
                //
                if (TempMultiSz == NULL) {
                    Status = STATUS_NOT_SUPPORTED;
                }
            }

            if (NT_SUCCESS(Status)) {
                //
                // If a multi-sz list of device location strings was returned,
                // inspect it, and keep note of a few things.  Specifically, the
                // number of strings in the multi-sz list, the length of the
                // multi-sz list, and the length of the longest string in the
                // list.
                //
                QueriedLocationStringsCount[i] = 0;
                TempMultiSzLength = 0;
                LongestStringLengthAtLevel = 0;

                for (p = TempMultiSz; *p != UNICODE_NULL; p += wcslen(p)+1) {
                    //
                    // Count the number of strings at this level (in this
                    // multi-sz list).
                    //
                    QueriedLocationStringsCount[i]++;

                    //
                    // Determine the length (in chars) of the multi-sz list so
                    // we can allocate our own buffer, and copy it.
                    //
                    TempMultiSzLength += (ULONG)(wcslen(p) + 1);

                    //
                    // Also determine the length of the longest string of all
                    // strings in this multi-sz list so we can estimate the
                    // length required for all device location path
                    // combinations.
                    //
                    StringLength = (ULONG)wcslen(p);
                    if (StringLength > LongestStringLengthAtLevel) {
                        LongestStringLengthAtLevel = StringLength;
                    }
                }

                PNP_ASSERT(QueriedLocationStringsCount[i] > 0);
                PNP_ASSERT(TempMultiSzLength > 0);
                PNP_ASSERT(LongestStringLengthAtLevel > 0);

                //
                // Include the length of the double NULL-terminating character.
                //
                TempMultiSzLength += 1;

                //
                // After analyzing the device location strings at each level,
                // update the number of device path combinations possible by
                // simply multiplying the combinations possible so far by the
                // number of strings retrieved for this level (in this multi-sz list).
                //
                DeviceLocationPathMultiSzStringCount *= QueriedLocationStringsCount[i];

                //
                // Also, update the length of the longest device location path
                // possible by adding the length of the longest string available
                // at this level.
                //
                DeviceLocationPathMultiSzLength += LongestStringLengthAtLevel;

                //
                // Make our own copy of the caller supplied multi-sz list of
                // device location strings.
                //
                QueriedLocationStrings[i] = (PWSTR)ExAllocatePool(PagedPool,
                    TempMultiSzLength*sizeof(WCHAR));

                if (QueriedLocationStrings[i] != NULL) {
                    //
                    // Note on array element ordering - since we start at the
                    // target devnode and walk up the chain of parents, we don't
                    // yet know just how high up the translation will go.  We
                    // add children towards the front of the array, so if
                    // translation is complete before every ancestor is queried,
                    // we'll just end up with some empty entries at the end.
                    //
                    RtlCopyMemory(QueriedLocationStrings[i],
                                  TempMultiSz,
                                  TempMultiSzLength*sizeof(WCHAR));


                    //
                    // Remember whether the device has any root or base
                    // strings.
                    //
                    for (p = QueriedLocationStrings[i]; *p; p += wcslen(p)+1) {

                        LocationStringIsBase = FALSE;
                        LocationStringIsRoot = FALSE;

                        for ( ; ; ) {

                            if (*p == _DEVICE_LOCATION_PATH_BASE_PREFIX_CHAR) {
                                LocationStringIsBase = TRUE;
                            } else if (*p == _DEVICE_LOCATION_PATH_ROOT_PREFIX_CHAR) {
                                LocationStringIsRoot = TRUE;
                            } else {
                                break;
                            }

                            p++;
                        }

                        //
                        // If the device has a root string, we can't skip
                        // any base strings since they may be required to
                        // "bridge" the path to get to them. Using a root
                        // string will restart building of the location path
                        // from that point, and still produce a valid path.
                        //
                        if (LocationStringIsRoot) {
                            DeviceHasRootString = TRUE;
                        }

                        //
                        // Remember the lowest level device in the tree that
                        // has a base level.  Since we start at the lowest
                        // device and work up the tree, only set the level
                        // if we have not already found a base string.
                        //
                        if ((LocationStringIsBase) && (DeviceHasBaseToLevel == ULONG_MAX)) {
                            DeviceHasBaseToLevel = i;
                        }
                    }

                    i++;

                } else {
                    //
                    // Unable to allocate a buffer for our own list of pointers.
                    //
                    Status = STATUS_INSUFFICIENT_RESOURCES;
                }

                //
                // Free the callee-allocated buffer.
                //
                ExFreePool(TempMultiSz);
                TempMultiSz = NULL;

            } else {
                //
                // If unsuccessful, make sure no location string was returned by
                // the interface routine.
                //
                PNP_ASSERT(TempMultiSz == NULL);

                //
                // If the driver failed the call, but still allocated memory for
                // us anyways, we'll clean up after it.
                //
                if (TempMultiSz != NULL) {
                    ExFreePool(TempMultiSz);
                    TempMultiSz = NULL;
                }
            }

        } else {
            //
            // If a GetLocationString location interface routine was not
            // supplied with the interface for some device before translation is
            // complete, the entire operation is unsuccessful.
            //
            // Fall through to dereference the interface below, then exit.
            //
            Status = STATUS_UNSUCCESSFUL;
        }

        //
        // Dereference the Location Interface.
        //
        if (LocationInterface.InterfaceDereference != NULL) {
            LocationInterface.InterfaceDereference(LocationInterface.Context);
        }

        if (!NT_SUCCESS(Status)) {
            //
            // If unsuccessful while requesting location information for some
            // device before translation was complete, the entire operation is
            // unsuccessful.
            //
            goto Clean0;

        } else if ((Status == STATUS_TRANSLATION_COMPLETE) ||
                   (i == QueriedLocationStringsArraySize)) {
            //
            // If successful, and the last device queried specifically indicated
            // the end of the translation - OR - this is the last device in the
            // ancestry, and therefore translation is explicitly complete, note
            // translation is complete.
            //
            Status = STATUS_TRANSLATION_COMPLETE;

            //
            // Account for the length of the NULL-terminating character in our
            // longest-length single string component estimate.
            //
            DeviceLocationPathMultiSzLength += 1;

            //
            // Stop walking up the device tree.
            //
            break;
        }

        //
        // Success so far, but we still need to query more devices for
        // location strings.
        //
        PNP_ASSERT(i < QueriedLocationStringsArraySize);

        //
        // Account for the length of a location path separator after every
        // path component but the last.
        //
        DeviceLocationPathMultiSzLength += IopConstStringLength(_DEVICE_LOCATION_PATH_SEPARATOR_STRING);
    }

    //
    // The location information of every device in the ancestry has been queried
    // successfully.
    //
    PNP_ASSERT(Status == STATUS_TRANSLATION_COMPLETE);

    if (NT_SUCCESS(Status)) {
        Status = STATUS_SUCCESS;
    } else {
        goto Clean0;
    }

    //
    // Make sure we queried at least one device.
    //
    PNP_ASSERT(i > 0);

    //
    // If DeviceLocationPathMultiSzLength is 0, NT_SUCCESS(Status) should be
    // FALSE and the execution shouldn't reach here. Make this assumption here
    // to suppress perfix warnings.
    //
    __analysis_assume(DeviceLocationPathMultiSzLength > 0);

    //
    // Allocate a buffer large enough to assume that all device location path
    // string combinations are as long as the longest device location path
    // string formed.  Also account for the double NULL-terminating character.
    //
    DeviceLocationPathMultiSzLength *= DeviceLocationPathMultiSzStringCount;
    DeviceLocationPathMultiSzLength += 1;

    DeviceLocationPathMultiSz = (PWCHAR)ExAllocatePool(PagedPool,
        DeviceLocationPathMultiSzLength*sizeof(WCHAR));

    if (DeviceLocationPathMultiSz == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto Clean0;
    }

    RtlZeroMemory(DeviceLocationPathMultiSz,
                  DeviceLocationPathMultiSzLength*sizeof(WCHAR));

    //
    // We should now have an array of multi-sz strings returned by the location
    // string interface routine for a set of devices in the ancestry of the
    // specified device.  From these multi-sz strings, we now need to build all
    // possible device paths.
    //

    //
    // First, determine where the first string in the device path is stored.
    // Since we stored these in the array in order, starting with the child
    // device, the last non-NULL string placed in the array (i - 1) is the most
    // significant location string.
    //
    FinalStringLevel = i - 1;
    PNP_ASSERT(QueriedLocationStrings[FinalStringLevel] != NULL);
    PNP_ASSERT(QueriedLocationStringsCount[FinalStringLevel] > 0);

    //
    // Build all string combinations by enumerating the total number of possible
    // combinations, and picking the appropriate string element from each
    // multi-sz list on each iteration.
    //
    pdlp = DeviceLocationPathMultiSz;

    for (CombinationEnumIndex = 0;
         CombinationEnumIndex < DeviceLocationPathMultiSzStringCount;
         CombinationEnumIndex++) {

        //
        // The queried location strings array starts with the most specific
        // device (the target device, at level 0) and ends with the highest
        // ancestor in the tree that reported a location path (at
        // FinalStringLevel).  We want to build the paths in tree order, so
        // start with the multi-sz list at the FinalStringLevel, and work
        // down to level 0.
        //
        i = FinalStringLevel;

        //
        // When starting at the first level, the number of combinations remaining is
        // simply the total number of combinations that can be formed from all
        // levels.  The number of combination remaining will be adjusted after
        // selecting a string from each subsequent level, by discounting the
        // combinations that the level contributed.
        //
        CombinationsRemaining = DeviceLocationPathMultiSzStringCount;

        //
        // Track where to copy the next string within the location path
        // separate from the start of the location path that being built for
        // this combination. Since we may skip some location strings (and
        // they may be at the end of all location strings for that level),
        // we may have to abort an entire combination that has already been
        // partially built.  Until we complete a combination, we should
        // start the next combination at the same location where we were
        // building the previous.
        //
        pdls = pdlp;

        //
        // For each location path being built, track how any base
        // strings have been used within it.
        //
        LocationPathIncludesBase = FALSE;
        LocationPathIncludesBaseOnly = FALSE;
        LocationPathIncludesBaseOnlyAtStart  = FALSE;

        for ( ; ; ) {

            PNP_ASSERT(CombinationsRemaining != 0);

            //
            // Calculate the index of the string in the multi-sz list at this
            // level that is needed by this enumeration.
            //
            // NOTE: 27-Nov-2001 : Jim Cavalaris (jamesca)
            //
            // The element to pick at each level to generate this
            // enumeration's device location path is calculated based on:
            //
            // - the enumeration element we require,
            // - the number of combinations remaining to be generated,
            // - the number of elements to choose from at this level.
            //
            // This will will build all possible combinations of device
            // location paths, enumerating elements from the least
            // significant device (the target device) to the most
            // significant device (translation complete), considering the
            // the order of the strings at a particular level have been
            // placed in the multi-sz list in order of decreasing relevance
            // (i.e. most relevant location string for a device first).
            //
            // - CombinationsRemaining is the number of complete elements
            //   that must be built from the selections available from all
            //   levels above the current level. At the start of every path,
            //   this is set to the count of all total combinations, as if
            //   no strings have been selected at any level.
            //
            // - (CombinationsRemaining / QueriedLocationStringsCount[i])
            //   describes the number of iterations through each element at
            //   the current level that are required before selecting the next
            //   element.
            //
            // - dividing that number into the index of the current
            //   enumeration gives the absolute index of the element in the
            //   expanded version of the selections at that level.
            //
            // - mod by the number of elements actually at this level to
            //   indicate which one to select.
            //

            MultiSzLookupIndex =
                (CombinationEnumIndex /
                 (CombinationsRemaining / QueriedLocationStringsCount[i])) %
                QueriedLocationStringsCount[i];

            //
            // (you may just want to trust me on this one.)
            //

            //
            // On the first enumeration, we should be selecting the first
            // element from every level, so that we end up with the most
            // specific location path string.
            //
            if (CombinationEnumIndex == 0) {
                NT_ASSERT(MultiSzLookupIndex == 0);
            }

            //
            // Find the calculated string.
            //
            MultiSzIndex = 0;
            for (p = QueriedLocationStrings[i]; MultiSzIndex < MultiSzLookupIndex; p += wcslen(p)+1) {
                MultiSzIndex++;
                PNP_ASSERT(*p != UNICODE_NULL);
                PNP_ASSERT(MultiSzIndex < QueriedLocationStringsCount[i]);
            }

            //
            // Check for location string isolation and/or translation terminator
            // prefix characters.
            //
            LocationStringIsBase = FALSE;
            LocationStringIsRoot = FALSE;

            for ( ; ; ) {

                if (*p == _DEVICE_LOCATION_PATH_BASE_PREFIX_CHAR) {
                    LocationStringIsBase = TRUE;
                } else if (*p == _DEVICE_LOCATION_PATH_ROOT_PREFIX_CHAR) {
                    LocationStringIsRoot = TRUE;
                } else {
                    break;
                }

                p++;
            }

            //
            // At the start of each location path combination, initialize
            // our base string usage tracking to match the first string
            // in the path.
            //
            if (i == FinalStringLevel) {
                LocationPathIncludesBase = LocationStringIsBase;
                LocationPathIncludesBaseOnly = LocationStringIsBase;
                LocationPathIncludesBaseOnlyAtStart = LocationStringIsBase;
                LocationPathSelectedBaseToLevel = LocationStringIsBase ? i : ULONG_MAX;
            }

            if (LocationStringIsBase) {
                //
                // Skip any combinations that would select a base string
                // unless:
                //
                // (1) The device has a root string that can possibly be
                //     used in some combination -- in which case we may need
                //     this string to "bridge" the path to the new root
                //     string.
                //
                // (2) The base string is used to build a location path that
                //     includes base strings ONLY at the START of the path.
                //     Once we branch a location path away from base
                //     strings, we should not use any more.
                //
                if ((!DeviceHasRootString) && (!LocationPathIncludesBaseOnlyAtStart)) {
                    break;
                }

                //
                // Remember that this location path includes a base
                // string. Once the location path is complete, if some base
                // string still exists in the path AFTER a non-base string,
                // the path does not include base strings ONLY at the START,
                // and we should abort that path.
                //
                LocationPathIncludesBase = TRUE;
                LocationPathSelectedBaseToLevel = i;

                //
                // If we create a location path that adds a base string to a
                // path that already includes non-base strings, we know the
                // path does not begin with only base strings.  We are only
                // building it because of the possibility that we may need
                // it to bridge the gap up to some later root string, but we
                // will want to throw it away if that doesn't happen.
                //
                if (!LocationPathIncludesBaseOnly) {
                    LocationPathIncludesBaseOnlyAtStart = FALSE;
                }

            } else {
                //
                // This combination will NOT contain only base strings.
                //
                LocationPathIncludesBaseOnly = FALSE;
            }

            if (LocationStringIsRoot) {
                //
                // Reset location path to the begining of the last combination
                // when its translation was completed.
                //
                pdls = pdlp;

                //
                // We just restarted the location path, so reset our base
                // string tracking to match whatever this root string is.
                //
                LocationPathIncludesBase = LocationStringIsBase;
                LocationPathIncludesBaseOnly = LocationStringIsBase;
                LocationPathIncludesBaseOnlyAtStart = LocationStringIsBase;
                LocationPathSelectedBaseToLevel = LocationStringIsBase ? i : ULONG_MAX;
            }

            //
            // Append the string to the buffer.
            //
            RtlCopyMemory(pdls, p, wcslen(p)*sizeof(WCHAR));
            pdls += wcslen(p);

            if (i == 0) {
                //
                // This is the last level.  If the path we just built
                // includes base strings, but does not includes all base
                // strings the device has available, abort this path.
                //
                if ((LocationPathIncludesBase) &&
                    ((!LocationPathIncludesBaseOnlyAtStart) ||
                     (LocationPathSelectedBaseToLevel > DeviceHasBaseToLevel))) {
                    break;
                }

                //
                // NULL terminate this device location path combination
                // string just formed.
                //
                *pdls = UNICODE_NULL;
                pdlp = pdls + 1;
                break;
            }

            //
            // If there are still more levels to process, append the device
            // location path separator string.
            //
            RtlCopyMemory(pdls,
                          _DEVICE_LOCATION_PATH_SEPARATOR_STRING,
                          IopConstStringSize(_DEVICE_LOCATION_PATH_SEPARATOR_STRING));
            pdls += IopConstStringLength(_DEVICE_LOCATION_PATH_SEPARATOR_STRING);

            //
            // Adjust the total remaining number of string combinations that are
            // possible to form from the string lists at the remaining levels.
            //
            CombinationsRemaining /= QueriedLocationStringsCount[i];

            //
            // Process the next level down.
            //
            i--;
        }
    }

    //
    // Double-NULL terminate the entire device location path multi-sz list.
    //
    *pdlp = UNICODE_NULL;
    ++pdlp;

    //
    // The multi-sz list of device location paths for this device has been built
    // successfully.
    //

    *DeviceLocationStrings = DeviceLocationPathMultiSz;
    *DeviceLocationStringsLength = (ULONG) (pdlp - DeviceLocationPathMultiSz);

  Clean0:

    //
    // Free any memory we may have allocated along the way.
    //
    if (QueriedLocationStrings != NULL) {
        PNP_ASSERT(QueriedLocationStringsArraySize > 0);
        for (i = 0; i < QueriedLocationStringsArraySize; i++) {
            if (QueriedLocationStrings[i] != NULL) {
                ExFreePool(QueriedLocationStrings[i]);
            }
        }
        ExFreePool(QueriedLocationStrings);
    }

    if (QueriedLocationStringsCount != NULL) {
        PNP_ASSERT(QueriedLocationStringsArraySize > 0);
        ExFreePool(QueriedLocationStringsCount);
    }

    //
    // If unsuccesful, make sure we don't return a buffer to the caller.
    //
    if (!NT_SUCCESS(Status)) {

        PNP_ASSERT(*DeviceLocationStrings == NULL);

        PNP_ASSERT(DeviceLocationPathMultiSz == NULL);
        if (DeviceLocationPathMultiSz != NULL) {
            ExFreePool(DeviceLocationPathMultiSz);
        }

    } else {
        PNP_ASSERT(*DeviceLocationStrings != NULL);
    }

    return Status;

} // PnpGetDeviceLocationStrings


NTSTATUS
PnpOpenFirstMatchingSubKey(
    __in __nullnullterminated PWCHAR MultiSzKeyNames,
    __in      HANDLE          RootHandle,
    __in      ACCESS_MASK     DesiredAccess,
    __in_opt  PPNP_MATCHING_SUBKEY_CALLBACK MatchingSubkeyCallback,
    __out     PHANDLE         MatchingKeyHandle,
    __out_opt PWSTR           *MatchingKeyName
    )

/*++

Routine Description:

    This routine retrieves the first subkey of the supplied root that matched a
    string in the multi-sz list.

Arguments:

    MultiSzKeyNames -

        Supplies a multi-sz list of possible matching subkey names.

    RootHandle -

        Specifies a handle to the root key that should be searched for a
        matching subkey.

    DesiredAccess -

        Specifies the desired access the matching subkey should be opened with,
        if found.

    MatchingSubkeyCallback -

        Optionally, specifies a callback routine to be called with matching
        subkeys to perform additional verification of potential subkey matches.
        If the callback routine returns FALSE for a potential match, the subkey
        is then considered NOT to be a match, and the search will continue.

    MatchingKeyHandle -

        Specifies the address of a variable to retrieve the open handle to the
        first matching subkey.

    MatchingKeyName -

        Optionally, specifies the address of a variable to retrieve the name
        of the first matching subkey.

Return Value:

    NTSTATUS code.

--*/

{
    NTSTATUS        Status;
    PWSTR           p;
    UNICODE_STRING  UnicodeString;

    PAGED_CODE();

    //
    // Validate parameters.
    //
    if ((!ARGUMENT_PRESENT(MultiSzKeyNames)) ||
        (RootHandle == NULL) ||
        (!ARGUMENT_PRESENT(MatchingKeyHandle))) {
        return STATUS_INVALID_PARAMETER;
    }

    //
    // Initialize output parameter.
    //
    *MatchingKeyHandle = NULL;

    if (MatchingKeyName != NULL) {
        *MatchingKeyName = NULL;
    }

    //
    // Start with no match found yet, in case the multi-sz is empty.
    //
    Status = STATUS_OBJECT_NAME_NOT_FOUND;

    //
    // Check each string in the multi-sz list.
    //
    for (p = MultiSzKeyNames; *p != UNICODE_NULL; p += wcslen(p)+1) {

        //
        // Attempt to open a corresponding subkey of the root.
        //
        RtlInitUnicodeString(&UnicodeString, p);

        Status = IopOpenRegistryKeyEx(MatchingKeyHandle,
                                      RootHandle,
                                      &UnicodeString,
                                      DesiredAccess);

        if (NT_SUCCESS(Status)) {

            PNP_ASSERT(*MatchingKeyHandle != NULL);

            //
            // We have a conditional match - check the MatchingSubkeyCallback
            // for verification, if we have one.
            //

            if ((ARGUMENT_PRESENT(MatchingSubkeyCallback)) &&
                (!(MatchingSubkeyCallback(*MatchingKeyHandle)))) {

                //
                // Not a match.
                //

                Status = STATUS_OBJECT_NAME_NOT_FOUND;

                //
                // Close the key and continue.
                //

                ZwClose(*MatchingKeyHandle);
                *MatchingKeyHandle = NULL;

                continue;
            }

            //
            // Match!
            //
            if (MatchingKeyName != NULL) {
                *MatchingKeyName = p;
            }

            break;
        }

        PNP_ASSERT(*MatchingKeyHandle == NULL);
        *MatchingKeyHandle = NULL;
    }

    if (NT_SUCCESS(Status)) {
        PNP_ASSERT(*MatchingKeyHandle != NULL);
    } else {
        PNP_ASSERT(*MatchingKeyHandle == NULL);
    }

    return Status;

} // PnpOpenFirstMatchingSubKey

NTSTATUS
PnpCheckDriverDependencies(
    __in PCZZWSTR Dependencies,
    __out PBOOLEAN DependenciesSatisfied,
    __out_opt PBOOLEAN GenericBootDependency
    )

/*++

Routine Description:

    This routine checks if all the specified dependencies are satisfied.

Arguments:

    Dependencies - Supplies a multi-sz list of dependencies to check

    DependenciesSatisfied - Returns a boolean value indicating if all the
        specified dependencies were satisfied or not.

    GenericBootDependency - Optionally returns if the list of dependencies
        contained a generic boot dependency.


Return Value:

    STATUS_SUCCESS if the dependencies were successfully checked.  An NTSTATUS
    error code otherwise.

--*/
{
    PCWSTR Dependency = NULL;
    BOOLEAN FirmwareAutomaticallySatisfied = FALSE;
    HANDLE hCurrentControlSet = NULL;
    HANDLE hFirmwareResources = NULL;
    HANDLE hResource = NULL;
    DWORD LastAttemptStatus = 0;
    DWORD Phase = 0;
    ULONG RegType;
    ULONG Size;
    NTSTATUS Status;

    PAGED_CODE();

    PNP_ASSERT(Dependencies != NULL);
    PNP_ASSERT(DependenciesSatisfied != NULL);

    *DependenciesSatisfied = FALSE;

    if (GenericBootDependency != NULL) {
        *GenericBootDependency = FALSE;
    }

    Status = _PnpCtxGetCachedContextBaseKey(PiPnpRtlCtx,
                                            PNPCTX_BASE_KEY_TYPE_CONTROLSET,
                                            &hCurrentControlSet);

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    for (Dependency = Dependencies;
         *Dependency != UNICODE_NULL;
         Dependency += wcslen(Dependency) + 1) {

        if (_wcsicmp(Dependency, DRIVER_DEPENDENCY_WILDCARD) == 0) {

            //
            // This dependency is a wildcard.  The dependency is automatically
            // met upon each system boot.
            //

            if (GenericBootDependency != NULL) {
                *GenericBootDependency = TRUE;
            }

            continue;
        } else if (FirmwareAutomaticallySatisfied) {
            continue;
        }

        //
        // There is a real dependency.  Open its firmware resources state to see
        // if the dependency is met.
        //

        if (hFirmwareResources == NULL) {
            Status = _PnpCtxRegOpenKey(PiPnpRtlCtx,
                                       hCurrentControlSet,
                                       REGSTR_PATH_CONTROL_FIRMWARE_RESOURCES,
                                       0,
                                       KEY_READ,
                                       &hFirmwareResources);

            if ((Status == STATUS_OBJECT_NAME_NOT_FOUND) ||
                (Status == STATUS_KEY_DELETED)) {

                //
                // The firmware resources key does not exist, so there are no
                // pending firmware resources. All non-wildcard dependencies are
                // satisfied.
                //

                Status = STATUS_SUCCESS;
                FirmwareAutomaticallySatisfied = TRUE;
                continue;

            } else if (!NT_SUCCESS(Status)) {
                break;
            }
        }

        //
        // Close the resource handle from a previous iteration of the loop, if
        // there is one, so handles don't get leaked.
        //

        if (hResource != NULL) {
            _PnpCtxRegCloseKey(PiPnpRtlCtx, hResource);
            hResource = NULL;
        }

        Status = _PnpCtxRegOpenKey(PiPnpRtlCtx,
                                   hFirmwareResources,
                                   Dependency,
                                   0,
                                   KEY_QUERY_VALUE,
                                   &hResource);

        if ((Status == STATUS_OBJECT_NAME_NOT_FOUND) ||
            (Status == STATUS_KEY_DELETED)) {

            //
            // The key for the particular firmware dependency does not exist.
            // There is no pending update to this firmware.  The dependency is
            // satisfied.
            //

            Status = STATUS_SUCCESS;
            continue;

        } else if (!NT_SUCCESS(Status)) {
            break;
        }

        Size = sizeof(Phase);
        Status = _PnpCtxRegQueryValue(PiPnpRtlCtx,
                                      hResource,
                                      REGSTR_VALUE_PHASE,
                                      &RegType,
                                      &Phase,
                                      &Size);

        if (Status == STATUS_OBJECT_NAME_NOT_FOUND) {

            //
            // The phase doesn't exist.  The firmware dependency is not
            // satisfied.
            //

            Status = STATUS_SUCCESS;
            break;

        } else if (Status == STATUS_KEY_DELETED) {

            //
            // The firmware resource has been deleted.  If there is no firmware
            // resource, the dependency is considered satisfied.
            //

            Status = STATUS_SUCCESS;
            continue;

        } else if (!NT_SUCCESS(Status)) {
            break;
        }

        if (Phase != 2) {

            //
            // The resource is not in the reporting phase.  The dependency is
            // not satisfied.
            //

            break;
        }

        Size = sizeof(LastAttemptStatus);
        Status = _PnpCtxRegQueryValue(PiPnpRtlCtx,
                                      hResource,
                                      REGSTR_VALUE_LAST_ATTEMPT_STATUS,
                                      &RegType,
                                      &LastAttemptStatus,
                                      &Size);

        if (Status == STATUS_OBJECT_NAME_NOT_FOUND) {

            //
            // The last attempt status doesn't exist.  The firmware dependency
            // is not satisfied.
            //

            Status = STATUS_SUCCESS;
            break;

        } else if (Status == STATUS_KEY_DELETED) {

            //
            // The firmware resource has been deleted.  If there is no firmware
            // resource, the dependency is considered satisfied.
            //

            Status = STATUS_SUCCESS;
            continue;

        } else if (!NT_SUCCESS(Status)) {
            break;
        }

        if (LastAttemptStatus != 0) {

            //
            // The resource is not in the reporting phase.  The dependency is
            // not satisfied.
            //

            break;
        }
    }

    if (!NT_SUCCESS(Status)) {
        goto Exit;
    }

    if (*Dependency == UNICODE_NULL) {

        //
        // All dependencies are satisfied.
        //

        *DependenciesSatisfied = TRUE;
    } else {
        PNP_ASSERT(*DependenciesSatisfied == FALSE);
    }

  Exit:

    //
    // DO NOT close hCurrentControlSet.  It is a cached handle from pnprtl
    // and MUST NOT be closed
    //

    if (hFirmwareResources != NULL) {
        _PnpCtxRegCloseKey(PiPnpRtlCtx, hFirmwareResources);
    }

    if (hResource != NULL) {
        _PnpCtxRegCloseKey(PiPnpRtlCtx, hResource);
    }

    return Status;
}

NTSTATUS
PnpGetStableSystemBootTime(
    __out PLARGE_INTEGER SystemBootTime
    )
/*++

Routine Description:

    This routine tries to get a stable system boot time.  The boot time reported
    by the kernel may change if the system clock is adjusted, since the kernel
    tries to maintain the boot time as a consistent amount of time prior to what
    the current time is.  The kernel keeps track of how much it has biased the
    boot time so the original time could theoretically be calculated.  However,
    it is possible for the bias to be reset as part of updating the boot time.

    To mitigate the possibility of the bias being reset, this routine first
    tries to read the boot time stored in the current hardware configuration.
    This is captured early in boot and will not change until the next time this
    hardware configuration is booted.  If that value cannot be retrieved, this
    falls back to calculating the boot time by adjusting the reported boot time
    by the reported bias.

Arguments:

    SystemBootTime - Returns a stable system boot time in UTC.

Return Value:

    STATUS_SUCCESS if the system boot time was successfully retrieved.  An
    NTSTATUS error code otherwise.

--*/
{
    NTSTATUS                     Status = STATUS_SUCCESS;
    SYSTEM_TIMEOFDAY_INFORMATION SystemTimeOfDayInfo;

    PAGED_CODE();

    ASSERT(SystemBootTime != NULL);

    //
    // First, attempt to retrieve the value stored in the current hardware
    // configuration at boot.
    //
    Status = PipHardwareConfigGetLastUseTime(NULL, SystemBootTime);

    //
    // If retrieving it from the registry failed, fall back to calculating
    // it based off the reported time and the bias.
    //
    if (!NT_SUCCESS(Status)) {
        Status = ZwQuerySystemInformation(SystemTimeOfDayInformation,
                                          &SystemTimeOfDayInfo,
                                          sizeof(SystemTimeOfDayInfo),
                                          NULL);

        if (NT_SUCCESS(Status)) {
            //
            // Adjust the boot time by the bias, before returning it as the
            // boot time.
            //
            SystemTimeOfDayInfo.BootTime.QuadPart -= SystemTimeOfDayInfo.BootTimeBias;

            RtlCopyMemory(SystemBootTime,
                          &SystemTimeOfDayInfo.BootTime,
                          sizeof(LARGE_INTEGER));
        }
    }

    return Status;
}

BOOLEAN
PnpIsSafeToExamineUserModeTeb(
    VOID
    )
{
    PETHREAD currentThread;

    //
    // If this thread is attached to a different process,
    // always return a guard page exception because the current
    // thread's TEB values have no meaning in the attached
    // process' virtual address space.
    //

    if (KeIsAttachedProcess()) {
        return FALSE;
    }

    currentThread = PsGetCurrentThread();

    //
    // Check if it is safe to look at the TEB.  That means the previous mode is
    // user-mode, special APCs are not disabled, we are not currently servicing
    // more than a single fault (this code can cause a single fault trying to
    // read the TEB but we don't want to recurse more than that) and no DPCs are
    // executing.
    //

    return ((KeGetPreviousModeByThread(&currentThread->Tcb) == UserMode) &&
            (!KeAreAllApcsDisabled()) &&
            (currentThread->ActiveFaultCount <= 1));
}

VOID
PnpLogDeviceConflictingResource(
    _In_ PPI_RESOURCE_ARBITER_ENTRY  ArbiterEntry,
    _In_ PVOID ArbiterInstance
    )

/*++

Routine Description:

    This routine logs the conflicting resource if the device fails resource
    arbitration.

Arguments:

    ArbiterEntry - Contains the required information for logging.

    ArbiterInstance - Supplies a pointer to the arbiter instance.

Return Value:

    VOID.

--*/

{

    PDEVICE_NODE FailedDeviceNode;
    PPNP_REBALANCE_TRACE_CONTEXT RebalanceContext;
    PARBITER_LIST_ENTRY RequestEntry;

    PAGED_CODE();

    RequestEntry = CONTAINING_RECORD(ArbiterEntry->ResourceList.Flink,
                                     ARBITER_LIST_ENTRY,
                                     ListEntry);

    if (RequestEntry != NULL) {
        FailedDeviceNode = PP_DO_TO_DN(RequestEntry->PhysicalDeviceObject);
        if (FailedDeviceNode != NULL) {
            if (FailedDeviceNode->RebalanceContext == NULL) {
                FailedDeviceNode->RebalanceContext = ExAllocatePoolWithTag(
                                                    PagedPool,
                                                    sizeof(PNP_REBALANCE_TRACE_CONTEXT),
                                                    PNP_POOLTAG_REBALANCE_TRACE_CONTEXT);

                if (FailedDeviceNode->RebalanceContext != NULL) {
                    RtlZeroMemory(FailedDeviceNode->RebalanceContext,
                                  sizeof(PNP_REBALANCE_TRACE_CONTEXT));
                }

            }

            if (FailedDeviceNode->RebalanceContext != NULL) {
                RebalanceContext = FailedDeviceNode->RebalanceContext;
                if (RebalanceContext->ConflictContext.ResourceRequests == NULL) {
                    RebalanceContext->ConflictContext.ResourceType = ArbiterEntry->ResourceType;
                    RebalanceContext->ConflictContext.AlternativeCount = RequestEntry->AlternativeCount;
                    RebalanceContext->ConflictContext.ResourceRequests = RequestEntry->Alternatives;
                    RebalanceContext->ConflictContext.ArbiterInstance = ArbiterInstance;
                }
            }
        }
    }

    return;
}

VOID
PnpLogDeviceRebalanceResult(
    _In_ PDEVICE_NODE  DeviceNode,
    _In_ PPNP_REBALANCE_TRACE_CONTEXT Context
    )

/*++

Routine Description:

    This routine logs the rebalance trace result for better analyzing rebalance
    failures.

Arguments:

    DeviceNode - Supplies a pointer to the device node that triggers rebalance.

    Context - Supplies a pointer to the rebalance loggings.

Return Value:

    VOID.

--*/

{

    PPNP_REBALANCE_TRACE_CONTEXT RebalanceContext;

    PAGED_CODE();

    if (DeviceNode == NULL || Context == NULL) {
        return;
    }

    if (DeviceNode->RebalanceContext == NULL) {
        return;
    }

    RebalanceContext = DeviceNode->RebalanceContext;
    RtlCopyMemory(RebalanceContext,
                  Context,
                  (FIELD_OFFSET(PNP_REBALANCE_TRACE_CONTEXT, ConflictContext)));

    return;
}
