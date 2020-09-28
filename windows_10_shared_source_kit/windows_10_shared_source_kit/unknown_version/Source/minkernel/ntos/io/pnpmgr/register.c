/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    register.c

Abstract:

    Plug and Play Manager routines dealing with device manipulation/registration.

Author:

    Lonny McMichael (lonnym) 02/14/95

--*/

#include "pnpmgrp.h"
#pragma hdrstop

typedef struct {
    BOOLEAN Add;
} PROCESS_DRIVER_CONTEXT, *PPROCESS_DRIVER_CONTEXT;

typedef NTSTATUS (*PDEVICE_SERVICE_ITERATOR_ROUTINE)(
    __in PUNICODE_STRING DeviceInstancePath,
    __in PUNICODE_STRING ServiceName,
    __in PVOID Context
    );

typedef struct {
    PUNICODE_STRING DeviceInstancePath;
    PDEVICE_SERVICE_ITERATOR_ROUTINE Iterator;
    PVOID Context;
} DEVICE_SERVICE_ITERATOR_CONTEXT, *PDEVICE_SERVICE_ITERATOR_CONTEXT;

typedef struct {
    CM_REG_PROP Property;
    BOOLEAN IsClassProperty;
} DEVICE_INSTANCE_DRIVER_PROPERTY, *PDEVICE_INSTANCE_DRIVER_PROPERTY;

//
// Prototype utility functions internal to this file.
//

NTSTATUS
PiFindDevInstMatch(
    __in HANDLE ServiceEnumHandle,
    __in PUNICODE_STRING DeviceInstanceName,
    __out PULONG InstanceCount,
    __out PUNICODE_STRING MatchingValueName,
    __out PULONG MatchingInstance
    );

NTSTATUS PiProcessDriverInstance(
    __in PUNICODE_STRING DeviceInstancePath,
    __in PUNICODE_STRING ServiceName,
    __in PPROCESS_DRIVER_CONTEXT Context
    );

NTSTATUS
PpForEachDeviceInstanceDriver(
    PUNICODE_STRING DeviceInstancePath,
    PDEVICE_SERVICE_ITERATOR_ROUTINE IteratorRoutine,
    PVOID Context
    );

NTSTATUS
PiForEachDriverQueryRoutine(
    __in ULONG ValueType,
    __in_bcount(ValueLength) PVOID ValueData,
    __in ULONG ValueLength,
    __in PDEVICE_SERVICE_ITERATOR_CONTEXT InternalContext
    );

VOID
PiRearrangeDeviceInstances(
    __in HANDLE ServiceEnumHandle,
    __in ULONG InstanceCount,
    __in ULONG InstanceDeleted
    );

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, PpDeviceRegistration)
#pragma alloc_text(PAGE, PiDeviceRegistration)
#pragma alloc_text(PAGE, PiProcessDriverInstance)
#pragma alloc_text(PAGE, PiFindDevInstMatch)
#pragma alloc_text(PAGE, PpForEachDeviceInstanceDriver)
#pragma alloc_text(PAGE, PiForEachDriverQueryRoutine)
#pragma alloc_text(PAGE, PiRearrangeDeviceInstances)
#endif // ALLOC_PRAGMA

NTSTATUS
PpDeviceRegistration(
    __in PUNICODE_STRING DeviceInstancePath,
    __in BOOLEAN Add,
    __in_opt PUNICODE_STRING ServiceKeyName,
    __in BOOLEAN LockHeld
    )

/*++

Routine Description:

    If Add is set to TRUE, this Plug and Play Manager API creates (if necessary)
    and populates the volatile Enum subkey of a device's service list entry, based
    on the device instance path specified.  If Add is set to FALSE, the specified
    device instance will be removed from the volatile Enum subkey of a device's
    service list entry.

    For example, if there is a device in the Enum tree as follows:

    HKLM\System\Enum\PCI
        \foo
            \0000
                Service = REG_SZ bar
            \0001
                Service = REG_SZ other

    The result of the call, PpDeviceRegistration("PCI\foo\0000", Add = TRUE), would be:

    HKLM\CurrentControlSet\Services
        \bar
            \Enum
                Count = REG_DWORD 1
                0 = REG_SZ PCI\foo\0000

Arguments:

    DeviceInstancePath - Supplies the path in the registry (relative to
                         HKLM\CCS\System\Enum) of the device to be registered/deregistered.
                         This path must point to an instance subkey.

    Add - Supplies a BOOLEAN value to indicate the operation is for addition or removal.

    ServiceKeyName - Optionally, supplies the address of a unicode string to
                     receive the name of the registry key for this device
                     instance's service (if one exists).  The caller must
                     release the space once done with it.

Return Value:

    NTSTATUS code indicating whether or not the function was successful

--*/

{

    NTSTATUS Status;

    PAGED_CODE();

    //
    // Acquire PnP device-specific registry resource for exclusive (read/write) access.
    //
    if (!LockHeld) {

        PiLockPnpRegistry(TRUE);
    }

    Status = PiDeviceRegistration(DeviceInstancePath,
                                  Add,
                                  ServiceKeyName);

    if (!LockHeld) {

        PiUnlockPnpRegistry();
    }

    return Status;
}


NTSTATUS
PiDeviceRegistration(
    __in PUNICODE_STRING DeviceInstancePath,
    __in BOOLEAN Add,
    __in_opt PUNICODE_STRING ServiceKeyName
    )

/*++

Routine Description:

    If Add is set to TRUE, this Plug and Play Manager API creates (if necessary)
    and populates the volatile Enum subkey of a device's service list entry, based
    on the device instance path specified.  If Add is set to FALSE, the specified
    device instance will be removed from the volatile Enum subkey of a device's
    service list entry.

    For example, if there is a device in the Enum tree as follows:

    HKLM\System\Enum\PCI
        \foo
            \0000
                Service = REG_SZ bar
            \0001
                Service = REG_SZ other

    The result of the call, PpDeviceRegistration("PCI\foo\0000", Add = TRUE), would be:

    HKLM\CurrentControlSet\Services
        \bar
            \Enum
                Count = REG_DWORD 1
                0 = REG_SZ PCI\foo\0000

Arguments:

    DeviceInstancePath - Supplies the path in the registry (relative to
                         HKLM\CCS\System\Enum) of the device to be registered/deregistered.
                         This path must point to an instance subkey.

    Add - Supplies a BOOLEAN value to indicate the operation is for addition or removal.

    ServiceKeyName - Optionally, supplies the address of a unicode string to
                     receive the name of the registry key for this device
                     instance's service (if one exists).  The caller must
                     release the space once done with it.

Return Value:

    NTSTATUS code indicating whether or not the function was successful

--*/

{

    PWSTR InstancePath;
    ULONG PropertyType = 0;
    ULONG PropertySize;
    UNICODE_STRING ServiceName;
    PWSTR ServiceString = NULL;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // Assume successful completion.
    //
    Status = STATUS_SUCCESS;

    if (ServiceKeyName) {
        PiWstrToUnicodeString(ServiceKeyName, NULL);
    }

    //
    // 'Normalize' the DeviceInstancePath by stripping off a trailing
    // backslash (if present)
    //

    if (DeviceInstancePath->Length <= sizeof(WCHAR)) {
        Status = STATUS_INVALID_PARAMETER;
        goto PrepareForReturn1;
    }

    if (DeviceInstancePath->Buffer[CB_TO_CWC(DeviceInstancePath->Length) - 1] ==
                                                            OBJ_NAME_PATH_SEPARATOR) {
        DeviceInstancePath->Length -= sizeof(WCHAR);
    }

    //
    // Read service property of the specified device.
    //

    Status = PnpUnicodeStringToWstr(&InstancePath, NULL, DeviceInstancePath);

    if (!NT_SUCCESS(Status)) {
        goto PrepareForReturn1;
    }

    PropertySize = MAX_SERVICE_NAME_LEN * sizeof(WCHAR);

    ServiceString = ExAllocatePool(PagedPool, PropertySize);

    if (!ServiceString) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto PrepareForReturn1;
    }

    Status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                 InstancePath,
                                 NULL,
                                 CM_REG_PROP_SERVICE,
                                 &PropertyType,
                                 (PUCHAR)ServiceString,
                                 &PropertySize,
                                 0);

    PNP_ASSERT(Status != STATUS_BUFFER_TOO_SMALL);
    PNP_ASSERT(Status != STATUS_BUFFER_OVERFLOW);

    PnpUnicodeStringToWstrFree(InstancePath, DeviceInstancePath);

    if (NT_SUCCESS(Status)) {
        Status = STATUS_OBJECT_NAME_NOT_FOUND;
        if (PropertyType == REG_SZ) {
            if (PropertySize > sizeof(UNICODE_NULL)) {
                Status = RtlInitUnicodeStringEx(&ServiceName, ServiceString);

                PNP_ASSERT(NT_SUCCESS(Status));

                Status = STATUS_SUCCESS;
                if (ServiceKeyName) {

                    //
                    // If need to return ServiceKeyName, make a copy now.
                    //

                    Status = PnpConcatenateUnicodeStrings(ServiceKeyName,
                                                          &ServiceName,
                                                          NULL);
                }
            }
        }

    } else if (Status == STATUS_NOT_FOUND) {
        //
        // The device instance key may have no Service value entry if the device
        // is raw capable.
        //
        Status = STATUS_SUCCESS;
    }

    if (NT_SUCCESS(Status)) {

        PROCESS_DRIVER_CONTEXT context;
        context.Add = Add;

        Status = PpForEachDeviceInstanceDriver(
                    DeviceInstancePath,
                    (PDEVICE_SERVICE_ITERATOR_ROUTINE) PiProcessDriverInstance,
                    &context);

        if (!NT_SUCCESS(Status) && Add) {

            context.Add = FALSE;
            PpForEachDeviceInstanceDriver(DeviceInstancePath,
                                          PiProcessDriverInstance,
                                          &context);
        }
    }

PrepareForReturn1:

    if (!NT_SUCCESS(Status)) {
        if (ServiceKeyName) {
            if (ServiceKeyName->Length != 0) {
                ExFreePool(ServiceKeyName->Buffer);
                ServiceKeyName->Buffer = NULL;
                ServiceKeyName->Length = ServiceKeyName->MaximumLength = 0;
            }
        }
    }

    if (ServiceString) {
        ExFreePool(ServiceString);
    }

    return Status;
}

NTSTATUS
PiProcessDriverInstance(
    __in PUNICODE_STRING DeviceInstancePath,
    __in PUNICODE_STRING ServiceName,
    __in PPROCESS_DRIVER_CONTEXT Context
    )
{
    ULONG Count;
    ULONG instance;
    PKEY_VALUE_FULL_INFORMATION KeyValueInformation = NULL;
    UNICODE_STRING MatchingDeviceInstance;
    HANDLE ServiceEnumHandle;
    NTSTATUS Status = STATUS_OBJECT_NAME_NOT_FOUND;
    UNICODE_STRING TempUnicodeString;
    CHAR UnicodeBuffer[20];
    BOOLEAN UpdateCount = FALSE;

    PAGED_CODE();

    PNP_ASSERT(Context != NULL);

    //
    // Next, open the service entry, and volatile Enum subkey
    // under HKLM\System\CurrentControlSet\Services (creating it if it
    // doesn't exist)
    //

    Status = PipOpenServiceEnumKeys(ServiceName,
                                    KEY_ALL_ACCESS,
                                    NULL,
                                    &ServiceEnumHandle,
                                    TRUE
                                   );
    if(!NT_SUCCESS(Status)) {
        goto PrepareForReturn2;
    }

    //
    // Now, search through the service's existing list of device instances, to see
    // if this instance has previously been registered.
    //

    Status = PiFindDevInstMatch(ServiceEnumHandle,
                                DeviceInstancePath,
                                &Count,
                                &MatchingDeviceInstance,
                                &instance
                                );

    if (!NT_SUCCESS(Status)) {
        goto PrepareForReturn3;
    }

    if (!MatchingDeviceInstance.Buffer) {

        //
        // If we didn't find a match and caller wants to register the device, then we add
        // this instance to the service's Enum list.
        //

        if (Context->Add) {
            PWSTR instancePathBuffer;
            ULONG instancePathLength;
            PWSTR freeBuffer = NULL;

            //
            // Create the value entry and update NextInstance= for the madeup key
            //

            instancePathBuffer = DeviceInstancePath->Buffer;
            instancePathLength = DeviceInstancePath->Length;

            if (instancePathBuffer[instancePathLength / sizeof(WCHAR) - 1] !=
                UNICODE_NULL) {
                freeBuffer = (PWSTR)ExAllocatePool(PagedPool, instancePathLength + sizeof(WCHAR));
                if (freeBuffer) {
                    RtlCopyMemory(freeBuffer,
                                  instancePathBuffer,
                                  instancePathLength
                                  );
                    freeBuffer[instancePathLength / sizeof(WCHAR)] = UNICODE_NULL;
                    instancePathBuffer = freeBuffer;
                    instancePathLength += sizeof(WCHAR);
                }
            }
            PiUlongToUnicodeString(&TempUnicodeString, UnicodeBuffer, 20, Count);
            Status = ZwSetValueKey(
                        ServiceEnumHandle,
                        &TempUnicodeString,
                        TITLE_INDEX_VALUE,
                        REG_SZ,
                        instancePathBuffer,
                        instancePathLength
                        );
            if (freeBuffer) {
                ExFreePool(freeBuffer);
            }
            Count++;
            UpdateCount = TRUE;
        }
    } else {

        //
        // If we did find a match and caller wants to deregister the device, then we remove
        // this instance from the service's Enum list.
        //
        PNP_ASSERT(instance != (ULONG)-1);

        if (Context->Add == FALSE) {

            ZwDeleteValueKey(ServiceEnumHandle, &MatchingDeviceInstance);
            Count--;
            UpdateCount = TRUE;

            //
            // Finally, if Count is not zero we need to physically reorganize the
            // instances under the ServiceKey\Enum key to make them contiguous. We
            // optimize by simply moving the last value into the empty slot. This behavior
            // is different for .Net Server release from previous releases but we hope
            // that no one depends on the ordering of values in this list. This list in a
            // way really represents the order in which devices (using this service) were
            // enumerated.
            //
            if (Count != 0) {

                PiRearrangeDeviceInstances(
                    ServiceEnumHandle,
                    Count,
                    instance
                    );
            }
        }
    }
    if (UpdateCount) {
        PiWstrToUnicodeString(&TempUnicodeString, REGSTR_VALUE_COUNT);
        ZwSetValueKey(
                ServiceEnumHandle,
                &TempUnicodeString,
                TITLE_INDEX_VALUE,
                REG_DWORD,
                &Count,
                sizeof(Count)
                );
        PiWstrToUnicodeString(&TempUnicodeString, REGSTR_VALUE_NEXT_INSTANCE);
        ZwSetValueKey(
                ServiceEnumHandle,
                &TempUnicodeString,
                TITLE_INDEX_VALUE,
                REG_DWORD,
                &Count,
                sizeof(Count)
                );
    }

    //
    // Need to release the matching device value name
    //

    if (MatchingDeviceInstance.Buffer) {
        RtlFreeUnicodeString(&MatchingDeviceInstance);
    }
    Status = STATUS_SUCCESS;

PrepareForReturn3:

    ZwClose(ServiceEnumHandle);

PrepareForReturn2:

    if (KeyValueInformation) {
        ExFreePool(KeyValueInformation);
    }

    return Status;
}


NTSTATUS
PiFindDevInstMatch(
    __in HANDLE ServiceEnumHandle,
    __in PUNICODE_STRING DeviceInstanceName,
    __out PULONG Count,
    __out PUNICODE_STRING MatchingValueName,
    __out PULONG MatchingInstance
    )

/*++

Routine Description:

    This routine searches through the specified Service\Enum values entries
    for a device instance matching the one specified by KeyInformation.
    If a matching is found, the MatchingValueName is returned and caller must
    free the unicode string when done with it.

Arguments:

    ServiceEnumHandle - Supplies a handle to service enum key.

    DeviceInstanceName - Supplies a pointer to a unicode string specifying the
                         name of the device instance key to search for.

    InstanceCount - Supplies a pointer to a ULONG variable to receive the device
                    instance count under the service enum key.

    MatchingNameFound - Supplies a pointer to a UNICODE_STRING to receive the value
                        name of the matched device instance.

Return Value:

    A NTSTATUS code.  if a matching is found, the MatchingValueName is the unicode
    string of the value name.  Otherwise its length and Buffer will be set to empty.

--*/

{
    NTSTATUS status;
    ULONG i, instanceCount, length = 256, junk;
    UNICODE_STRING valueName, unicodeValue;
    PWCHAR unicodeBuffer;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation = NULL;

    PAGED_CODE();

    //
    // Find out how many instances are referenced in the service's Enum key.
    //

    MatchingValueName->Length = 0;
    MatchingValueName->Buffer = NULL;
    *Count = instanceCount = 0;
    *MatchingInstance = (ULONG)-1;

    status = IopGetRegistryValue(ServiceEnumHandle,
                                 REGSTR_VALUE_COUNT,
                                 0,
                                 &keyValueInformation
                                );
    if (NT_SUCCESS(status)) {

        if((keyValueInformation->Type == REG_DWORD) &&
           (keyValueInformation->DataLength >= sizeof(ULONG))) {

            instanceCount = *(PULONG)KEY_VALUE_DATA(keyValueInformation);
            *Count = instanceCount;
        }
        ExFreePool(keyValueInformation);

    } else if(status != STATUS_OBJECT_NAME_NOT_FOUND) {
        return status;
    } else {

        //
        // If 'Count' value entry not found, consider this to mean there are simply
        // no device instance controlled by this service.  Thus we don't have a match.
        //

        return STATUS_SUCCESS;
    }

    keyValueInformation = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePool(
                                    PagedPool, length);
    if (!keyValueInformation) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Allocate heap to store value name
    //

    unicodeBuffer = (PWSTR)ExAllocatePool(PagedPool, 10 * sizeof(WCHAR));
    if (!unicodeBuffer) {
        ExFreePool(keyValueInformation);
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    //
    // Next scan thru each value key to find a match
    //

    for (i = 0; i < instanceCount ; i++) {
       PiUlongToUnicodeString(&valueName, unicodeBuffer, 20, i);
       status = ZwQueryValueKey (
                        ServiceEnumHandle,
                        &valueName,
                        KeyValueFullInformation,
                        keyValueInformation,
                        length,
                        &junk
                        );
        if (!NT_SUCCESS(status)) {
            if (status == STATUS_BUFFER_OVERFLOW || status == STATUS_BUFFER_TOO_SMALL) {
                ExFreePool(keyValueInformation);
                length = junk;
                keyValueInformation = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePool(
                                        PagedPool, length);
                if (!keyValueInformation) {
                    ExFreePool(unicodeBuffer);
                    return STATUS_INSUFFICIENT_RESOURCES;
                }
                i--;
            }
            continue;
        }

        if (keyValueInformation->Type == REG_SZ) {
            if (keyValueInformation->DataLength > sizeof(UNICODE_NULL)) {
                IopRegistryDataToUnicodeString(&unicodeValue,
                                               (PWSTR)KEY_VALUE_DATA(keyValueInformation),
                                               keyValueInformation->DataLength
                                               );
            } else {
                continue;
            }
        } else {
            continue;
        }

        if (RtlEqualUnicodeString(&unicodeValue,
                                  DeviceInstanceName,
                                  TRUE)) {
            //
            // We found a match.
            //
            *MatchingValueName= valueName;
            *MatchingInstance = i;
            break;
        }
    }
    if (keyValueInformation) {
        ExFreePool(keyValueInformation);
    }
    if (MatchingValueName->Length == 0) {

        //
        // If we did not find a match, we need to release the buffer.  Otherwise
        // it is caller's responsibility to release the buffer.
        //

        ExFreePool(unicodeBuffer);
    }
    return STATUS_SUCCESS;
}

NTSTATUS
PpForEachDeviceInstanceDriver(
    PUNICODE_STRING DeviceInstancePath,
    PDEVICE_SERVICE_ITERATOR_ROUTINE IteratorRoutine,
    PVOID Context
    )
/*++

Routine Description:

    This routine will call the iterator routine once for each driver listed
    for this particular device instance.  It will walk through any class
    filter drivers and device filter drivers, as well as the service, in the
    order they will be added to the PDO.  If the iterator routine returns
    a failure status at any point the iteration will be terminated.

Arguments:

    DeviceInstancePath - the registry path (relative to CCS\Enum)

    IteratorRoutine - the routine to be called for each service.  This routine
                      will be passed:

                       * The device instance path
                       * The type of driver that this is (filter, service, etc.)
                       * the Context value passed in
                       * The name of the service

    Context - an arbitrary context passed into the iterator routine

Return Value:

    STATUS_SUCCCESS if everything was run across properly

    status if an error occurred opening critical keys or if the iterator
    routine returns an error.

--*/

{
    WCHAR classGuid[GUID_STRING_LEN];
    HANDLE classKey = NULL;
    static const DEVICE_INSTANCE_DRIVER_PROPERTY deviceInstanceDriverProperty[] =
        {{CM_REG_PROP_LOWERFILTERS, TRUE},
         {CM_REG_PROP_LOWERFILTERS, FALSE},
         {CM_REG_PROP_SERVICE, FALSE},
         {CM_REG_PROP_UPPERFILTERS, FALSE},
         {CM_REG_PROP_UPPERFILTERS, TRUE}};
    ULONG index;
    HANDLE instanceKey = NULL;
    PWSTR instancePath = NULL;
    DEVICE_SERVICE_ITERATOR_CONTEXT internalContext;
    PUCHAR propertyBuffer = NULL;
    ULONG propertySize;
    ULONG propertyType;
    ULONG requiredSize;
    NTSTATUS status;

    PAGED_CODE();

    //
    // Open the instance key for this devnode
    //

    status = PnpUnicodeStringToWstr(&instancePath, NULL, DeviceInstancePath);

    if (!NT_SUCCESS(status)) {
        return status;
    }

    status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                 instancePath,
                                 CM_KEY_TYPE_DEVICE_INSTANCE,
                                 0,
                                 KEY_READ,
                                 FALSE,
                                 &instanceKey,
                                 NULL);

    if (!NT_SUCCESS(status)) {
        goto PrepareForReturn;
    }

    //
    // Get the class guid for this device.  If it exists, open up a handle
    // to that class.
    //

    propertySize = GUID_STRING_LEN * sizeof(WCHAR);
    status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                 instancePath,
                                 instanceKey,
                                 CM_REG_PROP_CLASSGUID,
                                 &propertyType,
                                 (PUCHAR)classGuid,
                                 &propertySize,
                                 0);

    PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);
    PNP_ASSERT(status != STATUS_BUFFER_OVERFLOW);

    if (NT_SUCCESS(status)) {

        if ((propertyType == REG_SZ) &&
            (propertySize != 0)) {

            status = _CmOpenInstallerClassRegKey(PiPnpRtlCtx,
                                                 classGuid,
                                                 CM_KEY_TYPE_DEVICE_INSTALLER_CLASS,
                                                 0,
                                                 KEY_READ,
                                                 FALSE,
                                                 &classKey,
                                                 NULL);
        }
    }

    //
    // For each type of filter driver we want to query for the list and
    // call into our callback routine.  We should do this in order from
    // bottom to top.
    //

    internalContext.Context = Context;
    internalContext.DeviceInstancePath = DeviceInstancePath;
    internalContext.Iterator = IteratorRoutine;

    //
    // Get all the information we have to out of the instance key and
    // the device node.
    //

    propertySize = SERVICE_NAME_LEN_GUESS * sizeof(WCHAR);

    propertyBuffer = ExAllocatePool(PagedPool, propertySize);

    if (propertyBuffer == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto PrepareForReturn;
    }

    for (index = 0; index < RTL_NUMBER_OF(deviceInstanceDriverProperty); index++) {

        //
        // The device might not be installed so it might not have a class to get
        // properties from.  If there is no class key and this is a class property,
        // skip retrieving this property.
        //
        if ((deviceInstanceDriverProperty[index].IsClassProperty) && (classKey == NULL)) {
            continue;
        }

        requiredSize = propertySize;

        if (deviceInstanceDriverProperty[index].IsClassProperty) {

            status = _CmGetInstallerClassRegProp(PiPnpRtlCtx,
                                                 classGuid,
                                                 classKey,
                                                 deviceInstanceDriverProperty[index].Property,
                                                 &propertyType,
                                                 propertyBuffer,
                                                 &requiredSize,
                                                 0);
        } else {

            status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                         instancePath,
                                         instanceKey,
                                         deviceInstanceDriverProperty[index].Property,
                                         &propertyType,
                                         propertyBuffer,
                                         &requiredSize,
                                         0);
        }

        if (status == STATUS_BUFFER_TOO_SMALL) {
            ExFreePool(propertyBuffer);

            propertySize = requiredSize;

            propertyBuffer = ExAllocatePool(PagedPool, propertySize);

            if (propertyBuffer == NULL) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                goto PrepareForReturn;
            }

            if (deviceInstanceDriverProperty[index].IsClassProperty) {

                status = _CmGetInstallerClassRegProp(PiPnpRtlCtx,
                                                     classGuid,
                                                     classKey,
                                                     deviceInstanceDriverProperty[index].Property,
                                                     &propertyType,
                                                     propertyBuffer,
                                                     &requiredSize,
                                                     0);
            } else {

                status = _CmGetDeviceRegProp(PiPnpRtlCtx,
                                             instancePath,
                                             instanceKey,
                                             deviceInstanceDriverProperty[index].Property,
                                             &propertyType,
                                             propertyBuffer,
                                             &requiredSize,
                                             0);
            }

            PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);
        }

        //
        // The property not existing is not an error.  Just move on to retrieving
        // the next property.
        //

        if (status == STATUS_NOT_FOUND) {
            status = STATUS_SUCCESS;
            continue;
        }

        if (!NT_SUCCESS(status)) {
            goto PrepareForReturn;
        }

        //
        // Process the property.
        //

        status = PiForEachDriverQueryRoutine(propertyType,
                                             propertyBuffer,
                                             requiredSize,
                                             &internalContext);

        if (!NT_SUCCESS(status)) {
            goto PrepareForReturn;
        }
    }

PrepareForReturn:

    if (instancePath != NULL) {
        PnpUnicodeStringToWstrFree(instancePath, DeviceInstancePath);
    }

    if (propertyBuffer != NULL) {
        ExFreePool(propertyBuffer);
    }

    if (classKey != NULL) {
        ZwClose(classKey);
    }

    if (instanceKey != NULL) {
        ZwClose(instanceKey);
    }

    return status;
}

NTSTATUS
PiForEachDriverQueryRoutine(
    __in ULONG ValueType,
    __in_bcount(ValueSize) PVOID ValueData,
    __in ULONG ValueSize,
    __in PDEVICE_SERVICE_ITERATOR_CONTEXT InternalContext
    )
{
    PWSTR currentService;
    size_t currentServiceLength;
    UNICODE_STRING ServiceName;
    NTSTATUS status;

    status = STATUS_SUCCESS;

    if ((ValueType != REG_SZ) && (ValueType != REG_MULTI_SZ)) {
        PNP_ASSERT(status == STATUS_SUCCESS);
        goto Cleanup;
    }

    //
    // Make sure the string is a reasonable length.
    //

    if (ValueSize <= sizeof(WCHAR)) {
        PNP_ASSERT(status == STATUS_SUCCESS);
        goto Cleanup;
    }

    if (ValueType == REG_SZ) {

        RtlInitUnicodeString(&ServiceName, ValueData);

        status = InternalContext->Iterator(InternalContext->DeviceInstancePath,
                                           &ServiceName,
                                           InternalContext->Context);

        if (!NT_SUCCESS(status)) {
            goto Cleanup;
        }

    } else {

        PNP_ASSERT(ValueType == REG_MULTI_SZ);

        for (currentService = (PWSTR)ValueData;
             *currentService;
             currentService += currentServiceLength) {

            currentServiceLength = wcslen(currentService) + 1;

            status = RtlULongSub(ValueSize, (ULONG)(currentServiceLength * sizeof(WCHAR)), &ValueSize);

            if (!NT_SUCCESS(status)) {

                //
                // The length of the current service string goes past the end of
                // the property buffer.  Don't call the callback with this value.
                // This should never happen so assert if it does.
                //
                // Note that this isn't treated as an error.  This extra data in the
                // buffer is just ignored.
                //

                PNP_ASSERT(FALSE);
                status = STATUS_SUCCESS;
                goto Cleanup;
            }

            RtlInitUnicodeString(&ServiceName, currentService);

            status = InternalContext->Iterator(InternalContext->DeviceInstancePath,
                                               &ServiceName,
                                               InternalContext->Context);

            if (!NT_SUCCESS(status)) {
                goto Cleanup;
            }

            if (ValueSize < sizeof(WCHAR)) {

                //
                // Make sure there are enough bytes left in the property buffer
                // to at least be a single unicode character for the next iteration
                // of the loop. This should never happen so assert if it gets here.
                //
                // Note that this isn't treated as an error.  The extra byte in the
                // buffer is just ignored.
                //

                PNP_ASSERT(FALSE);
                goto Cleanup;
            }
        }
    }

  Cleanup:

    return status;
}

VOID
PiRearrangeDeviceInstances(
    __in HANDLE ServiceEnumHandle,
    __in ULONG InstanceCount,
    __in ULONG InstanceDeleted
    )
{
    NTSTATUS Status;
    PKEY_VALUE_FULL_INFORMATION KeyValueInformation;
    CHAR UnicodeBuffer[20];
    UNICODE_STRING TempUnicodeString;
    ULONG maxCount;
    KEY_FULL_INFORMATION keyInfo;
    ULONG tmp;

    PAGED_CODE();

    KeyValueInformation = NULL;
    maxCount = 0x200;
    Status = ZwQueryKey(
                ServiceEnumHandle,
                KeyFullInformation,
                &keyInfo,
                sizeof(keyInfo),
                &tmp
                );
    if (NT_SUCCESS(Status) && keyInfo.Values) {

        maxCount = keyInfo.Values;
    }

    //
    // Read the last value.
    //
    PiUlongToUnicodeString(&TempUnicodeString, UnicodeBuffer, 20, InstanceCount);
    Status = IopGetRegistryValue(ServiceEnumHandle,
                                 TempUnicodeString.Buffer,
                                 0,
                                 &KeyValueInformation
                                 );
    if (NT_SUCCESS(Status)) {

        //
        // Delete the last value.
        //
        ZwDeleteValueKey(ServiceEnumHandle, &TempUnicodeString);

        //
        // Set the new value with the instance we just deleted above..
        //
        PiUlongToUnicodeString(&TempUnicodeString, UnicodeBuffer, 20, InstanceDeleted);
        ZwSetValueKey (ServiceEnumHandle,
                       &TempUnicodeString,
                       TITLE_INDEX_VALUE,
                       REG_SZ,
                       (PVOID)KEY_VALUE_DATA(KeyValueInformation),
                       KeyValueInformation->DataLength
                       );

        ExFreePool(KeyValueInformation);
        KeyValueInformation = NULL;
    }

    //
    // Cleanup.
    //
    if (KeyValueInformation) {

        ExFreePool(KeyValueInformation);
    }
}
