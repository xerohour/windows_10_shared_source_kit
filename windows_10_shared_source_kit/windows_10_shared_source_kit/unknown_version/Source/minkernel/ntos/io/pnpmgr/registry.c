/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    registry.c

Abstract:

    This module contains registry functions for the PNP subsystem.

Author:

    Santosh Jodh (santoshj) August 12, 2004

--*/

#include "pnpmgrp.h"
#pragma hdrstop

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, IopCreateRegistryKeyEx)
#pragma alloc_text(PAGE, IopOpenRegistryKeyEx)
#pragma alloc_text(PAGE, PipOpenServiceEnumKeys)
#pragma alloc_text(PAGE, IopOpenCurrentHwProfileDeviceInstanceKey)
#pragma alloc_text(PAGE, PipApplyFunctionToServiceInstances)
#pragma alloc_text(PAGE, PipServiceInstanceToDeviceInstance)
#pragma alloc_text(PAGE, PpSystemHiveLimitCallback)
#pragma alloc_text(PAGE, PnpSetRegistryResourceList)
#pragma alloc_text(PAGE, PnpSetRegistryRequirementsList)
#pragma alloc_text(PAGE, PnpSetRegistryDword)
#endif // ALLOC_PRAGMA

_Must_inspect_result_
__drv_maxIRQL(APC_LEVEL)
NTSTATUS
IopOpenRegistryKeyEx(
    __out PHANDLE Handle,
    __in_opt HANDLE BaseHandle,
    __in PUNICODE_STRING KeyName,
    __in ACCESS_MASK DesiredAccess
    )

/*++

Routine Description:

    Opens a registry key using the name passed in based at the BaseHandle node.
    This name may specify a key that is actually a registry path.

Arguments:

    Handle - Pointer to the handle which will contain the registry key that
        was opened.

    BaseHandle - Optional handle to the base path from which the key must be
        opened. If this parameter is specified, then KeyName must be a relative
        path.

    KeyName - Name of the Key that must be opened/created (possibly a registry path)

    DesiredAccess - Specifies the desired access that the caller needs to
        the key.

Return Value:

   The function value is the final status of the operation.

--*/

{
    OBJECT_ATTRIBUTES objectAttributes;

    PAGED_CODE();

    *Handle = NULL;

    InitializeObjectAttributes( &objectAttributes,
                                KeyName,
                                OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                BaseHandle,
                                (PSECURITY_DESCRIPTOR) NULL
                                );
    //
    // Simply attempt to open the path, as specified.
    //
    return ZwOpenKey( Handle, DesiredAccess, &objectAttributes );
}

__drv_maxIRQL(APC_LEVEL)
NTSTATUS
IopCreateRegistryKeyEx(
    __out PHANDLE Handle,
    __in_opt HANDLE BaseHandle,
    __in PUNICODE_STRING KeyName,
    __in ACCESS_MASK DesiredAccess,
    __in ULONG CreateOptions,
    __out_opt PULONG Disposition
    )

/*++

Routine Description:

    Opens or creates a registry key using the name
    passed in based at the BaseHandle node. This name may specify a key
    that is actually a registry path, in which case each intermediate subkey
    will be created (if Create is TRUE).

    NOTE: Creating a registry path (i.e., more than one of the keys in the path
    do not presently exist) requires that a BaseHandle be specified.

Arguments:

    Handle - Pointer to the handle which will contain the registry key that
        was opened.

    BaseHandle - Optional handle to the base path from which the key must be opened.
        If KeyName specifies a registry path that must be created, then this parameter
        must be specified, and KeyName must be a relative path.

    KeyName - Name of the Key that must be opened/created (possibly a registry path)

    DesiredAccess - Specifies the desired access that the caller needs to
        the key.

    CreateOptions - Options passed to ZwCreateKey.

    Disposition - If Create is TRUE, this optional pointer receives a ULONG indicating
        whether the key was newly created:

            REG_CREATED_NEW_KEY - A new Registry Key was created
            REG_OPENED_EXISTING_KEY - An existing Registry Key was opened

Return Value:

   The function value is the final status of the operation.

--*/

{
    OBJECT_ATTRIBUTES objectAttributes;
    ULONG disposition, baseHandleIndex = 0, keyHandleIndex = 1, closeBaseHandle;
    HANDLE handles[2];
    BOOLEAN continueParsing;
    PWCHAR pathEndPtr, pathCurPtr, pathBeginPtr;
    ULONG pathComponentLength;
    UNICODE_STRING unicodeString;
    NTSTATUS status;

    PAGED_CODE();

    *Handle= NULL;

    InitializeObjectAttributes( &objectAttributes,
                                KeyName,
                                OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                BaseHandle,
                                (PSECURITY_DESCRIPTOR) NULL
                                );
    //
    // Attempt to create the path as specified. We have to try it this
    // way first, because it allows us to create a key without a BaseHandle
    // (if only the last component of the registry path is not present).
    //
    status = ZwCreateKey(&(handles[keyHandleIndex]),
                         DesiredAccess,
                         &objectAttributes,
                         0,
                         (PUNICODE_STRING) NULL,
                         CreateOptions,
                         &disposition
                         );

    if (status == STATUS_OBJECT_NAME_NOT_FOUND && ARGUMENT_PRESENT(BaseHandle)) {
        //
        // If we get to here, then there must be more than one element of the
        // registry path that does not currently exist.  We will now parse the
        // specified path, extracting each component and doing a ZwCreateKey on it.
        //
        handles[baseHandleIndex] = NULL;
        handles[keyHandleIndex] = BaseHandle;
        closeBaseHandle = 0;
        continueParsing = TRUE;
        pathBeginPtr = KeyName->Buffer;
        pathEndPtr = (PWCHAR)((PCHAR)pathBeginPtr + KeyName->Length);
        status = STATUS_SUCCESS;

        while(continueParsing) {
            //
            // There's more to do, so close the previous base handle (if necessary),
            // and replace it with the current key handle.
            //
            if(closeBaseHandle > 1) {
                ZwClose(handles[baseHandleIndex]);
            }
            baseHandleIndex = keyHandleIndex;
            keyHandleIndex = (keyHandleIndex + 1) & 1;  // toggle between 0 and 1.
            handles[keyHandleIndex] = NULL;

            //
            // Extract next component out of the specified registry path.
            //
            for (pathCurPtr = pathBeginPtr;
                ((pathCurPtr < pathEndPtr) && (*pathCurPtr != OBJ_NAME_PATH_SEPARATOR));
                pathCurPtr++);

            pathComponentLength = (ULONG)((PCHAR)pathCurPtr - (PCHAR)pathBeginPtr);
            if (pathComponentLength != 0) {
                //
                // Then we have a non-empty path component (key name).  Attempt
                // to create this key.
                //
                unicodeString.Buffer = pathBeginPtr;
                unicodeString.Length = unicodeString.MaximumLength = (USHORT)pathComponentLength;

                InitializeObjectAttributes(&objectAttributes,
                                           &unicodeString,
                                           OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                           handles[baseHandleIndex],
                                           (PSECURITY_DESCRIPTOR) NULL
                                          );
                status = ZwCreateKey(&(handles[keyHandleIndex]),
                                     DesiredAccess,
                                     &objectAttributes,
                                     0,
                                     (PUNICODE_STRING) NULL,
                                     CreateOptions,
                                     &disposition
                                    );
                if(NT_SUCCESS(status)) {
                    //
                    // Increment the closeBaseHandle value, which basically tells us whether
                    // the BaseHandle passed in has been 'shifted out' of our way, so that
                    // we should start closing our base handles when we're finished with them.
                    //
                    closeBaseHandle++;
                } else {
                    continueParsing = FALSE;
                    continue;
                }
            } else {
                //
                // Either a path separator ('\') was included at the beginning of
                // the path, or we hit 2 consecutive separators.
                //
                status = STATUS_INVALID_PARAMETER;
                continueParsing = FALSE;
                continue;
            }

            if((pathCurPtr == pathEndPtr) ||
               ((pathBeginPtr = pathCurPtr + 1) == pathEndPtr)) {
                //
                // Then we've reached the end of the path
                //
                continueParsing = FALSE;
            }
        }

        if(closeBaseHandle > 1) {
            ZwClose(handles[baseHandleIndex]);
        }
    }

    if(NT_SUCCESS(status)) {
        *Handle = handles[keyHandleIndex];

        if(ARGUMENT_PRESENT(Disposition)) {
            *Disposition = disposition;
        }
    }

    return status;
}

NTSTATUS
PipOpenServiceEnumKeys (
    __in PUNICODE_STRING ServiceKeyName,
    __in ACCESS_MASK DesiredAccess,
    __out_opt PHANDLE ServiceHandle,
    __out_opt PHANDLE ServiceEnumHandle,
    __in BOOLEAN CreateEnum
    )

/*++

Routine Description:

    This routine opens the HKEY_LOCAL_MACHINE\CurrentControlSet\Services\
    ServiceKeyName and its Enum subkey and returns handles for both key.
    It is caller's responsibility to close the returned handles.

Arguments:

    ServiceKeyName - Supplies a pointer to the name of the subkey in the
        system service list (HKEY_LOCAL_MACHINE\CurrentControlSet\Services)
        that caused the driver to load. This is the RegistryPath parameter
        to the DriverEntry routine.

    DesiredAccess - Specifies the desired access to the keys.

    ServiceHandle - Supplies a variable to receive a handle to ServiceKeyName.
        A NULL ServiceHandle indicates caller does not want need the handle to
        the ServiceKeyName.

    ServiceEnumHandle - Supplies a variable to receive a handle to ServiceKeyName\Enum.
        A NULL ServiceEnumHandle indicates caller does not need the handle to
        the ServiceKeyName\Enum.

    CreateEnum - Supplies a BOOLEAN variable to indicate should the Enum subkey be
        created if not present.

Return Value:

    status

--*/

{
    HANDLE serviceHandle, enumHandle;
    UNICODE_STRING enumName;
    NTSTATUS status;
    HANDLE serviceRootHandle;

    status = _PnpCtxGetCachedContextBaseKey(PiPnpRtlCtx,
                                            PNPCTX_BASE_KEY_TYPE_SERVICES,
                                            &serviceRootHandle);

    if (!NT_SUCCESS( status )) {

        return status;
    }

    //
    // Open the registry ServiceKeyName key.
    //

    status = IopOpenRegistryKeyEx(&serviceHandle,
                                  serviceRootHandle,
                                  ServiceKeyName,
                                  DesiredAccess);

    if (!NT_SUCCESS( status )) {

        //
        // There is no registry key for the ServiceKeyName information.
        //

        return status;
    }

    if (ARGUMENT_PRESENT(ServiceEnumHandle) || CreateEnum) {

        //
        // Open registry ServiceKeyName\Enum branch if caller wants
        // the handle or wants to create it.
        //

        PiWstrToUnicodeString(&enumName, REGSTR_KEY_ENUM);

        if (CreateEnum) {
            status = IopCreateRegistryKeyEx( &enumHandle,
                                             serviceHandle,
                                             &enumName,
                                             DesiredAccess,
                                             REG_OPTION_VOLATILE,
                                             NULL
                                             );
        } else {
            status = IopOpenRegistryKeyEx( &enumHandle,
                                           serviceHandle,
                                           &enumName,
                                           DesiredAccess
                                           );

        }

        if (!NT_SUCCESS( status )) {

            //
            // There is no registry key for the ServiceKeyName\Enum information.
            //

            ZwClose(serviceHandle);
            return status;
        }
        if (ARGUMENT_PRESENT(ServiceEnumHandle)) {
            *ServiceEnumHandle = enumHandle;
        } else {
            ZwClose(enumHandle);
        }
    }

    //
    // if caller wants to have the ServiceKey handle, we return it.  Otherwise
    // we close it.
    //

    if (ARGUMENT_PRESENT(ServiceHandle)) {
        *ServiceHandle = serviceHandle;
    } else {
        ZwClose(serviceHandle);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
IopOpenCurrentHwProfileDeviceInstanceKey(
    __out PHANDLE Handle,
    __in  PUNICODE_STRING ServiceKeyName,
    __in  ULONG Instance,
    __in  ACCESS_MASK DesiredAccess,
    __in  BOOLEAN Create
    )

/*++

Routine Description:

    This routine sets the csconfig flags for the specified device
    which is specified by the instance number under ServiceKeyName\Enum.

Arguments:

    ServiceKeyName - Supplies a pointer to the name of the subkey in the
        system service list (HKEY_LOCAL_MACHINE\CurrentControlSet\Services)
        that caused the driver to load. This is the RegistryPath parameter
        to the DriverEntry routine.

    Instance - Supplies the instance value under ServiceKeyName\Enum key

    DesiredAccess - Specifies the desired access that the caller needs to
        the key.

    Create - Determines if the key is to be created if it does not exist.

Return Value:

    status

--*/

{
    NTSTATUS status;
    UNICODE_STRING tempUnicodeString;

    status = PipServiceInstanceToDeviceInstance(NULL,
                                                ServiceKeyName,
                                                Instance,
                                                &tempUnicodeString,
                                                NULL,
                                                0
                                                );
    if (NT_SUCCESS(status)) {

        status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                     tempUnicodeString.Buffer,
                                     CM_KEY_TYPE_DEVICE_INSTANCE|CM_KEY_TYPE_SCOPE_CONFIG,
                                     0,
                                     DesiredAccess,
                                     Create,
                                     Handle,
                                     NULL
                                     );

        RtlFreeUnicodeString(&tempUnicodeString);
    }
    return status;
}

NTSTATUS
PipApplyFunctionToServiceInstances(
    __in_opt  HANDLE ServiceKeyHandle,
    __in_opt  PUNICODE_STRING ServiceKeyName,
    __in        ACCESS_MASK DesiredAccess,
    __in        BOOLEAN IgnoreNonCriticalErrors,
    __in        PIOP_SUBKEY_CALLBACK_ROUTINE DevInstCallbackRoutine,
    __inout     PVOID Context,
    __out_opt PULONG ServiceInstanceOrdinal
    )

/*++

Routine Description:

    This routine enumerates all device instances referenced by the instance
    ordinal entries under a service's volatile Enum key, and calls
    the specified callback routine for each instance's corresponding subkey
    under HKLM\System\Enum.

Arguments:

    ServiceKeyHandle - Optional handle to the service entry. If this parameter
        is not specified, then the service key name must be given in
        ServiceKeyName (if both parameters are specified, then ServiceKeyHandle
        is used, and ServiceKeyName is ignored).

    ServiceKeyName - Optional name of the service entry key (under
        HKLM\CurrentControlSet\Services). If this parameter is not specified,
        then ServiceKeyHandle must contain a handle to the desired service key.

    DesiredAccess - Specifies the desired access that the callback routine
        needs to the enumerated device instance keys.  If no desired access is
        specified (i.e., DesiredAccess is zero), then no handle will be opened
        for the device instance keys, and the callback will be passed a NULL for
        its DeviceInstanceHandle parameter.

    IgnoreNonCriticalErrors - Specifies whether this function should
        immediately terminate on all errors, or only on critical ones.
        An example of a non-critical error is when an enumerated device instance
        key cannot be opened for the desired access.

    DevInstCallbackRoutine - Supplies a pointer to a function that will
        be called for each device instance key referenced by a service instance
        entry under the service's volatile Enum subkey. The prototype of the
        function is as follows:

            typedef BOOLEAN (*PIOP_SUBKEY_CALLBACK_ROUTINE) (
                __in     HANDLE DeviceInstanceHandle,
                __in     PUNICODE_STRING DeviceInstancePath,
                __inout PVOID Context
                );

        where DeviceInstanceHandle is the handle to an enumerated device instance
        key, DeviceInstancePath is the registry path (relative to
        HKLM\System\Enum) to this device instance, and Context is a pointer to
        user-defined data.

        This function should return TRUE to continue enumeration, or
        FALSE to terminate it.

    Context - Supplies a pointer to user-defined data that will be passed
        in to the callback routine at each device instance key invocation.

    ServiceInstanceOrdinal - Optionally, receives the service instance ordinal (1 based)
        that terminated the enumeration, or the total number of instances enumerated
        if the enumeration completed without being aborted.

Return Value:

    NT status code indicating whether the device instance keys were successfully
    enumerated.  Note that this does not provide information on the success or
    failure of the callback routine--if desired, this information should be
    stored in the Context structure.

--*/

{
    NTSTATUS Status;
    HANDLE ServiceEnumHandle, DeviceInstanceHandle;
    UNICODE_STRING TempUnicodeString;
    ULONG ServiceInstanceCount, i, junk;
    PKEY_VALUE_FULL_INFORMATION KeyValueInformation;
    LOGICAL ContinueEnumeration;
    PWSTR DeviceInstance;

    //
    // First, open up the volatile Enum subkey under the specified service entry.
    //

    if (ARGUMENT_PRESENT(ServiceKeyHandle)) {
        PiWstrToUnicodeString(&TempUnicodeString, REGSTR_KEY_ENUM);
        Status = IopOpenRegistryKeyEx( &ServiceEnumHandle,
                                       ServiceKeyHandle,
                                       &TempUnicodeString,
                                       KEY_READ
                                       );
    } else {
        Status = PipOpenServiceEnumKeys(ServiceKeyName,
                                        KEY_READ,
                                        NULL,
                                        &ServiceEnumHandle,
                                        FALSE
                                       );
    }
    if (!NT_SUCCESS(Status)) {
        return Status;
    }

    //
    // Find out how many instances are referenced in the service's Enum key.
    //

    ServiceInstanceCount = 0;   // assume none.

    Status = IopGetRegistryValue(ServiceEnumHandle,
                                 REGSTR_VALUE_COUNT,
                                 0,
                                 &KeyValueInformation
                                );
    if (NT_SUCCESS(Status)) {

        if ((KeyValueInformation->Type == REG_DWORD) &&
           (KeyValueInformation->DataLength >= sizeof(ULONG))) {

            ServiceInstanceCount = *(PULONG)KEY_VALUE_DATA(KeyValueInformation);

        }
        ExFreePool(KeyValueInformation);

    } else if (Status != STATUS_OBJECT_NAME_NOT_FOUND) {
        goto PrepareForReturn;
    } else {
        //
        // If 'Count' value entry not found, consider this to mean there are simply
        // no device instance controlled by this service.
        //
        Status = STATUS_SUCCESS;
    }

    //
    // Now, enumerate each service instance, and call the specified callback function
    // for the corresponding device instance.
    //

    if (ServiceInstanceCount) {

        //
        // Set DeviceInstanceHandle to NULL (assume we won't be opening up the
        // device instance keys).
        //

        DeviceInstanceHandle = NULL;

        KeyValueInformation = (PKEY_VALUE_FULL_INFORMATION)ExAllocatePool(
                                                              PagedPool,
                                                              PNP_SCRATCH_BUFFER_SIZE);
        if (!KeyValueInformation) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto PrepareForReturn;
        }

        for (i = 0; ; i++) {

            Status = ZwEnumerateValueKey(
                            ServiceEnumHandle,
                            i,
                            KeyValueFullInformation,
                            KeyValueInformation,
                            PNP_SCRATCH_BUFFER_SIZE,
                            &junk
                            );

            if (!NT_SUCCESS (Status)) {
                if (Status == STATUS_NO_MORE_ENTRIES) {
                    Status = STATUS_SUCCESS;
                    break;
                } else if (IgnoreNonCriticalErrors) {
                    continue;
                } else {
                    break;
                }
            }

            if (KeyValueInformation->Type != REG_SZ) {
                continue;
            }

            ContinueEnumeration = TRUE;
            TempUnicodeString.Length = 0;
            IopRegistryDataToUnicodeString(&TempUnicodeString,
                                           (PWSTR)KEY_VALUE_DATA(KeyValueInformation),
                                           KeyValueInformation->DataLength
                                           );
            if (TempUnicodeString.Length) {

                //
                // We have retrieved a (non-empty) string for this service instance.
                // If the user specified a non-zero value for the DesiredAccess
                // parameter, we will attempt to open up the corresponding device
                // instance key under HKLM\System\Enum.
                //
                if (DesiredAccess) {
                    Status = PnpUnicodeStringToWstr(&DeviceInstance, NULL, &TempUnicodeString);

                    if (NT_SUCCESS(Status)) {
                        Status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                                     DeviceInstance,
                                                     CM_KEY_TYPE_DEVICE_INSTANCE,
                                                     0,
                                                     DesiredAccess,
                                                     FALSE,
                                                     &DeviceInstanceHandle,
                                                     NULL);

                        PnpUnicodeStringToWstrFree(DeviceInstance, &TempUnicodeString);
                    }
                }

                if (NT_SUCCESS(Status)) {
                    //
                    // Invoke the specified callback routine for this device instance.
                    //
                    ContinueEnumeration = DevInstCallbackRoutine(DeviceInstanceHandle,
                                                                 &TempUnicodeString,
                                                                 Context
                                                                );
                    if (DesiredAccess) {
                        ZwClose(DeviceInstanceHandle);
                    }
                } else if (IgnoreNonCriticalErrors) {
                    continue;
                } else {
                    break;
                }
            } else {
                continue;
            }
            if (!ContinueEnumeration) {
                break;
            }
        }

        if (ARGUMENT_PRESENT(ServiceInstanceOrdinal)) {
            *ServiceInstanceOrdinal = i;
        }

        ExFreePool(KeyValueInformation);
    }


PrepareForReturn:

    ZwClose(ServiceEnumHandle);

    return Status;
}

VOID
PpSystemHiveLimitCallback(
    PSYSTEM_HIVE_LIMITS HiveLimits,
    ULONG Level
    )
{
    PAGED_CODE();

    if (Level >= HiveLimits->High) {

        PnpSystemHiveTooLarge = TRUE;
    } else {

        PNP_ASSERT(Level <= HiveLimits->Low);

        PnpSystemHiveTooLarge = FALSE;

        PpResetProblemDevices(IopRootDeviceNode, CM_PROB_REGISTRY_TOO_LARGE);

        PnpRequestDeviceAction(IopRootDeviceNode->PhysicalDeviceObject,
                               RestartEnumeration,
                               FALSE,
                               0,
                               NULL,
                               NULL);

    }
}

NTSTATUS
PipServiceInstanceToDeviceInstance (
    __in_opt  HANDLE ServiceKeyHandle ,
    __in_opt  PUNICODE_STRING ServiceKeyName,
    __in        ULONG ServiceInstanceOrdinal,
    __out_opt PUNICODE_STRING DeviceInstanceRegistryPath,
    __out_opt PHANDLE DeviceInstanceHandle,
    __in        ACCESS_MASK DesiredAccess
    )

/*++

Routine Description:

    This routine reads the service node enum entry to find the desired device instance
    under the System\Enum tree.  It then optionally returns the registry path of the
    specified device instance (relative to HKLM\System\Enum) and an open handle
    to that registry key.

    It is the caller's responsibility to close the handle returned if
    DeviceInstanceHandle is supplied, and also to free the (PagedPool) memory
    allocated for the unicode string buffer of DeviceInstanceRegistryPath, if
    supplied.

Parameters:

    ServiceKeyHandle - Optionally, supplies a handle to the driver service node in the
        registry that controls this device instance.  If this argument is not specified,
        then ServiceKeyName is used to specify the service entry.

    ServiceKeyName - Optionally supplies the name of the service entry that controls
        the device instance. This must be specified if ServiceKeyHandle isn't given.

    ServiceInstanceOrdinal - Supplies the instance value under the service entry's
        volatile Enum subkey that references the desired device instance.

    DeviceInstanceRegistryPath - Optionally, supplies a pointer to a unicode string
        that will be initialized with the registry path (relative to HKLM\System\Enum)
        to the device instance key.

    DeviceInstanceHandle - Optionally, supplies a pointer to a variable that will
        receive a handle to the opened device instance registry key.

    DesiredAccess - If DeviceInstanceHandle is specified (i.e., the device instance
        key is to be opened), then this variable specifies the access that is needed
        to this key.

Return Value:

    NT status code indicating whether the function was successful.

--*/

{
    PWSTR instancePath;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;
    HANDLE handle;
    NTSTATUS status;
    WCHAR unicodeBuffer[20];
    UNICODE_STRING unicodeKeyName;

    instancePath = NULL;

    //
    // Open registry ServiceKeyName\Enum branch
    //
    if(ARGUMENT_PRESENT(ServiceKeyHandle)) {

        PiWstrToUnicodeString(&unicodeKeyName, REGSTR_KEY_ENUM);
        status = IopOpenRegistryKeyEx( &handle,
                                       ServiceKeyHandle,
                                       &unicodeKeyName,
                                       KEY_READ
                                       );
    } else {

        status = PipOpenServiceEnumKeys(ServiceKeyName,
                                        KEY_READ,
                                        NULL,
                                        &handle,
                                        FALSE
                                       );
    }

    if (!NT_SUCCESS( status )) {

        //
        // There is no registry key for the ServiceKeyName\Enum information.
        //

        return status;
    }

    //
    // Read a path to System\Enum hardware tree branch specified by the service
    // instance ordinal
    //

    RtlStringCbPrintfW(unicodeBuffer,
                       sizeof(unicodeBuffer),
                       REGSTR_VALUE_STANDARD_ULONG_FORMAT,
                       ServiceInstanceOrdinal);

    status = IopGetRegistryValue ( handle,
                                   unicodeBuffer,
                                   SERVICE_INSTANCE_SIZE,
                                   &keyValueInformation
                                   );

    ZwClose(handle);
    if (!NT_SUCCESS( status )) {
        return status;
    } else {
        if(keyValueInformation->Type == REG_SZ) {
            IopRegistryDataToUnicodeString(&unicodeKeyName,
                                           (PWSTR)KEY_VALUE_DATA(keyValueInformation),
                                           keyValueInformation->DataLength
                                          );
            if(!unicodeKeyName.Length) {
                status = STATUS_OBJECT_PATH_NOT_FOUND;
            }
        } else {
            status = STATUS_INVALID_PLUGPLAY_DEVICE_PATH;
        }

        if(!NT_SUCCESS(status)) {
            goto PrepareForReturn;
        }
    }

    //
    // If the DeviceInstanceHandle argument was specified, open the device instance
    // key under HKLM\System\CurrentControlSet\Enum
    //

    if (ARGUMENT_PRESENT(DeviceInstanceHandle)) {
        status = PnpUnicodeStringToWstr(&instancePath, NULL, &unicodeKeyName);

        if (!NT_SUCCESS(status)) {
            goto PrepareForReturn;
        }

        status = _CmOpenDeviceRegKey(PiPnpRtlCtx,
                                     instancePath,
                                     CM_KEY_TYPE_DEVICE_INSTANCE,
                                     0,
                                     DesiredAccess,
                                     FALSE,
                                     DeviceInstanceHandle,
                                     NULL);

        PnpUnicodeStringToWstrFree(instancePath,
                                   &unicodeKeyName);

        if (!NT_SUCCESS( status )) {
            goto PrepareForReturn;
        }
    }

    //
    // If the DeviceInstanceRegistryPath argument was specified, then store a
    // copy of the device instance path in the supplied unicode string variable.
    //

    if (ARGUMENT_PRESENT(DeviceInstanceRegistryPath)) {
        status = PnpConcatenateUnicodeStrings(DeviceInstanceRegistryPath,
                                              &unicodeKeyName,
                                              NULL);
        if (!NT_SUCCESS(status)) {
            if(ARGUMENT_PRESENT(DeviceInstanceHandle)) {
                ZwClose(*DeviceInstanceHandle);
            }
        }
    }

PrepareForReturn:

    ExFreePool(keyValueInformation);
    return status;
}

NTSTATUS
PnpSetRegistryResourceList (
    __in HANDLE Key,
    __in PWSTR Name,
    __in_opt PCM_RESOURCE_LIST ResourceList
    )

/*++

Routine Description:

    This routine will set the resource list under the specified key.
    If no resource list is passed, it will delete any existing resource list
    from under the key.

Arguments:

    Key - Supplies the handle to the registry key.

    Name - Supplies a pointer to the value name.

    ResourceList - Supplies a pointer to the resource list.

Return Value:

    NTSTATUS.

--*/

{
    ULONG size;
    UNICODE_STRING unicodeName;

    PAGED_CODE();

    PNP_ASSERT(Key != NULL);
    PNP_ASSERT(Name != NULL);

    RtlInitUnicodeString(&unicodeName, Name);
    if (ResourceList == NULL) {
        return ZwDeleteValueKey(Key, &unicodeName);
    }

    //
    // Determine size of resources.
    //

    size = PnpDetermineResourceListSize(ResourceList);
    PNP_ASSERT(size != 0);

    //
    // Set resources under the specified key.
    //

    return ZwSetValueKey(Key,
                         &unicodeName,
                         TITLE_INDEX_VALUE,
                         REG_RESOURCE_LIST,
                         ResourceList,
                         size);
}

NTSTATUS
PnpSetRegistryRequirementsList (
    __in HANDLE Key,
    __in PWSTR Name,
    __in_opt PIO_RESOURCE_REQUIREMENTS_LIST ResourceRequirements
    )

/*++

Routine Description:

    This routine will set the resource requirements under the specified key.
    If no requirement is passed, it will delete any existing requirement
    from under the key.

Arguments:

    Key - Supplies the handle to the registry key.

    Name - Supplies a pointer to the value name.

    ResourceRequirements - Supplies a pointer to the resource requirements
        list.

Return Value:

    NTSTATUS.

--*/

{
    UNICODE_STRING unicodeName;

    PAGED_CODE();

    PNP_ASSERT(Key != NULL);
    PNP_ASSERT(Name != NULL);
    PNP_ASSERT(ResourceRequirements == NULL || ResourceRequirements->ListSize != 0);

    RtlInitUnicodeString(&unicodeName, Name);
    if (ResourceRequirements == NULL) {
        return ZwDeleteValueKey(Key, &unicodeName);
    }

    return ZwSetValueKey(Key,
                         &unicodeName,
                         TITLE_INDEX_VALUE,
                         REG_RESOURCE_REQUIREMENTS_LIST,
                         ResourceRequirements,
                         ResourceRequirements->ListSize);
}

NTSTATUS
PnpSetRegistryDword (
    __in HANDLE Key,
    __in PWSTR Name,
    __in ULONG Value
    )

/*++

Routine Description:

    This routine will set the integer value under the specified key.

Arguments:

    Key - Supplies the handle to the registry key.

    Name - Supplies a pointer to the value name.

    Value - Supplies the integer value to be set.

Return Value:

    NTSTATUS.

--*/

{
    UNICODE_STRING unicodeName;

    PAGED_CODE();

    PNP_ASSERT(Key != NULL);
    PNP_ASSERT(Name != NULL);

    RtlInitUnicodeString(&unicodeName, Name);
    return ZwSetValueKey(Key,
                         &unicodeName,
                         TITLE_INDEX_VALUE,
                         REG_DWORD,
                         (PBYTE)&Value,
                         sizeof(Value));
}
