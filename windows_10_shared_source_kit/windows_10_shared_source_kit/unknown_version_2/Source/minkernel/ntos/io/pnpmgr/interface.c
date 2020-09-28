/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    interface.c

Abstract:

    This module contains APIs and routines for handling device interfaces.

Author:


Environment:

    Kernel mode

Revision History:


--*/

#include "pnpmgrp.h"
#pragma hdrstop

//
// Definitions for IoGetDeviceInterfaces
//

#define INITIAL_SYMLINK_BUFFER_SIZE      1024
#define INITIAL_SYMLINK_LIST_BUFFER_SIZE 8192
#define INITIAL_REFERENCE_STRING_BUFFER_SIZE 256

typedef struct _INTERFACE_FILTER_CONTEXT {
    PWSTR DefaultInterface;
    SECURITY_SUBJECT_CONTEXT SecuritySubjectContext;
} INTERFACE_FILTER_CONTEXT, *PINTERFACE_FILTER_CONTEXT;

//
// Definitions for IoRegisterDeviceInterface
//

#define GLOBAL_SYMLINK_STRING_PREFIX       TEXT("\\GLOBAL??\\")

//
// Prototypes
//

NTSTATUS
IopBuildGlobalSymbolicLinkString(
    __in  PUNICODE_STRING SymbolicLinkName,
    __out PUNICODE_STRING GlobalString
    );

NTSTATUS
IopReplaceSeperatorWithPound(
    __inout PUNICODE_STRING OutString,
    __in PUNICODE_STRING    InString
    );

__callback
BOOLEAN
IopDeviceInterfaceFilterCallback(
    __in HPNPCTX        hPnpCtx,
    __in PCWSTR         ObjectName,
    __in CM_OBJECT_TYPE ObjectType,
    __in_opt PVOID      Context
    );

NTSTATUS
IopProcessSetInterfaceState(
    __in PUNICODE_STRING SymbolicLinkName,
    __in BOOLEAN Enable,
    __in BOOLEAN DeferNotStarted
    );

BOOLEAN
PipCheckForDenyExecute(
    __in PCWSTR InterfaceClass
    );

NTSTATUS
PiDeferSetInterfaceState(
    __in PDEVICE_NODE DeviceNode,
    __in PUNICODE_STRING SymbolicLinkName
    );

NTSTATUS
PiRemoveDeferredSetInterfaceState(
    __in PDEVICE_NODE DeviceNode,
    __in PUNICODE_STRING SymbolicLinkName
    );

BOOLEAN
PipCanEnableInterfaces (
    __in PDEVICE_NODE DeviceNode
    );

#ifdef ALLOC_PRAGMA

#pragma alloc_text(PAGE, IoGetDeviceInterfaceAlias)
#pragma alloc_text(PAGE, IoGetDeviceInterfaces)
#pragma alloc_text(PAGE, IoOpenDeviceInterfaceRegistryKey)
#pragma alloc_text(PAGE, IoRegisterDeviceInterface)
#pragma alloc_text(PAGE, IoSetDeviceInterfaceState)
#pragma alloc_text(PAGE, IopBuildGlobalSymbolicLinkString)
#pragma alloc_text(PAGE, IopGetDeviceInterfaces)
#pragma alloc_text(PAGE, IopDeviceInterfaceFilterCallback)
#pragma alloc_text(PAGE, IopProcessSetInterfaceState)
#pragma alloc_text(PAGE, IopRegisterDeviceInterface)
#pragma alloc_text(PAGE, PipCheckForDenyExecute)
#pragma alloc_text(PAGE, PnpDisableDeviceInterfaces)
#pragma alloc_text(PAGE, PnpDeleteDeviceInterfaces)
#pragma alloc_text(PAGE, IopDoDeferredSetInterfaceState)
#pragma alloc_text(PAGE, PiDeferSetInterfaceState)
#pragma alloc_text(PAGE, PiRemoveDeferredSetInterfaceState)
#pragma alloc_text(PAGE, PipCanEnableInterfaces)

#endif // ALLOC_PRAGMA

NTSTATUS
IopGetDeviceInterfaces(
    __in CONST GUID *InterfaceClassGuid,
    __in_opt PUNICODE_STRING DevicePath,
    __in ULONG Flags,
    __in BOOLEAN UserModeFormat,
    __deref_out_bcount(*SymbolicLinkListSize) PWSTR *SymbolicLinkList,
    __out_opt PULONG SymbolicLinkListSize
    )

/*++

Routine Description:

    This API allows a WDM driver to get a list of paths that represent all
    devices registered for the specified interface class.

Parameters:

    InterfaceClassGuid - Supplies a pointer to a GUID representing the interface
        class for whom a list of members is to be retrieved.

    DevicePath - Optionally, supplies a pointer to a unicode string containing
        the enumeration path for a device for whom interfaces of the specified
        class are to be retrieved.  If this parameter is not supplied, then
        all interface devices (regardless of what physical device exposes them)
        will be returned.

    Flags - Supplies flags that modify the behavior of list retrieval.
        The following flags are presently defined:

        DEVICE_INTERFACE_INCLUDE_NONACTIVE -- If this flag is specified, then
            all interface devices, whether currently active or not, will be
            returned (potentially filtered based on the PhysicalDeviceObject,
            if specified).

    UserModeFormat - If TRUE the multi-sz returned will have user mode prefixes
        (\\?\) otherwise they will have kernel mode prefixes (\??\).

    SymbolicLinkList - Supplies the address of a character pointer, that on
        success will contain a multi-sz list of symbolic link
        names that provide the requested functionality.  The caller is
        responsible for freeing the memory via ExFreePool.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    PWSTR currentInterface;
    PWSTR defaultInterface = NULL;
    ULONG defaultInterfaceSize;
    BOOLEAN defaultPresent = FALSE;
    PWSTR deviceInstance = NULL;
    DEVPROP_BOOLEAN enabled;
    INTERFACE_FILTER_CONTEXT filterContext;
    WCHAR guidString[GUID_STRING_LEN];
    HANDLE hClass = NULL;
    HANDLE hKey= NULL;
    PWSTR interfaceDevice = NULL;
    PWSTR interfaceList = NULL;
    ULONG interfaceListLength = 0;
    ULONG propertySize;
    DEVPROPTYPE propertyType;
    PWSTR remainingList;
    size_t remainingListLength;
    ULONG retryCount;
    NTSTATUS status;
    ULONG tempInterfaceListLength = 0;
    UNICODE_STRING tempUnicodeString;

    PAGED_CODE();

    //
    // Initialize out parameters.
    //

    *SymbolicLinkList = NULL;

    //
    // Convert the GUID into a string.
    //

    status = PnpStringFromGuid(InterfaceClassGuid, guidString, RTL_NUMBER_OF(guidString));
    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    //
    // Enter critical section and acquire a lock on the registry.  Both these
    // mechanisms are required to prevent deadlock in the case where an APC
    // routine calls this routine after the registry resource has been claimed
    // in this case it would wait blocking this thread so the registry would
    // never be released -> deadlock.  Critical sectioning the registry
    // manipulation portion solves this problem
    //

    PiLockPnpRegistry(TRUE);

    //
    // Open function class GUID key.
    //

    status = _CmOpenInterfaceClassRegKey(PiPnpRtlCtx,
                                         guidString,
                                         CM_KEY_TYPE_DEVICE_INTERFACE_CLASS,
                                         0,
                                         KEY_ALL_ACCESS,
                                         FALSE,
                                         &hClass,
                                         NULL);

    if (status == STATUS_OBJECT_NAME_NOT_FOUND || status == STATUS_OBJECT_PATH_NOT_FOUND) {

        //
        // The path does not exist - return a single null character buffer.
        //

        interfaceListLength = 1;
        interfaceList = ExAllocatePool(PagedPool, interfaceListLength * sizeof(WCHAR));

        if (interfaceList == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto clean1;
        }

        interfaceList[0] = UNICODE_NULL;
        status = STATUS_SUCCESS;
        goto clean1;

    } else if (!NT_SUCCESS(status)) {
        goto clean1;
    }

    //
    // Get the default value if it exists.
    //

    status = PnpGetObjectProperty(PNP_POOLTAG_GENERIC,
                                  INITIAL_SYMLINK_BUFFER_SIZE,
                                  guidString,
                                  PNP_OBJECT_TYPE_DEVICE_INTERFACE_CLASS,
                                  hClass,
                                  NULL,
                                  &DEVPKEY_DeviceInterfaceClass_DefaultInterface,
                                  &propertyType,
                                  &defaultInterface,
                                  &defaultInterfaceSize,
                                  0);

    if ((NT_SUCCESS(status)) &&
        (propertyType == DEVPROP_TYPE_STRING)) {

        defaultPresent = TRUE;

        //
        // Open the device interface instance key for the default name.
        //

        status = _CmOpenDeviceInterfaceRegKey(PiPnpRtlCtx,
                                              defaultInterface,
                                              CM_KEY_TYPE_DEVICE_INTERFACE_INSTANCE,
                                              0,
                                              KEY_READ,
                                              FALSE,
                                              &hKey,
                                              NULL);

        if (!NT_SUCCESS(status)) {

            //
            // Ignore the invalid default entry.
            //

            defaultPresent = FALSE;

        } else {

            //
            // If a device filter is specified, make sure the default interface
            // belongs to this device.
            //

            if (DevicePath) {
                PNP_ASSERT(defaultPresent);

                propertySize = MAX_DEVICE_ID_LEN * sizeof(WCHAR);

                interfaceDevice = ExAllocatePool(PagedPool, propertySize);

                if (!interfaceDevice) {
                    status = STATUS_INSUFFICIENT_RESOURCES;
                    goto clean1;
                }

                status = _PnpGetObjectProperty(PiPnpRtlCtx,
                                               defaultInterface,
                                               PNP_OBJECT_TYPE_DEVICE_INTERFACE,
                                               hKey,
                                               NULL,
                                               &DEVPKEY_Device_InstanceId,
                                               &propertyType,
                                               (PUCHAR)interfaceDevice,
                                               propertySize,
                                               &propertySize,
                                               0);

                if ((NT_SUCCESS(status)) &&
                    (propertyType == DEVPROP_TYPE_STRING)) {

                    status = RtlInitUnicodeStringEx(&tempUnicodeString, interfaceDevice);

                    if ((!NT_SUCCESS(status)) ||
                        (FALSE == RtlEqualUnicodeString(DevicePath,
                                                        &tempUnicodeString,
                                                        TRUE))) {
                        defaultPresent = FALSE;
                    }
                } else {

                    //
                    // The device this interface belongs to couldn't be retrieved.
                    // Assume that means this interface doesn't belong to the
                    // specified device.
                    //

                    defaultPresent = FALSE;
                }
            }

            //
            // If we are just supposed to return live interfaces, then make sure
            // this default interface is enabled.
            //

            if ((defaultPresent) &&
                (!(Flags & DEVICE_INTERFACE_INCLUDE_NONACTIVE))) {

                defaultPresent = FALSE;

                propertySize = sizeof(DEVPROP_BOOLEAN);
                status = _PnpGetObjectProperty(PiPnpRtlCtx,
                                               defaultInterface,
                                               PNP_OBJECT_TYPE_DEVICE_INTERFACE,
                                               hKey,
                                               NULL,
                                               &DEVPKEY_DeviceInterface_Enabled,
                                               &propertyType,
                                               (PUCHAR)&enabled,
                                               propertySize,
                                               &propertySize,
                                               0);

                if ((NT_SUCCESS(status)) &&
                    (propertyType == DEVPROP_TYPE_BOOLEAN) &&
                    (propertySize == sizeof(DEVPROP_BOOLEAN))) {

                    defaultPresent = (enabled == DEVPROP_TRUE) ? TRUE : FALSE;
                }
            }

            if (defaultPresent) {
                SECURITY_SUBJECT_CONTEXT securitySubjectContext;
                BOOLEAN valid = FALSE;

                //
                // Perform filtering to see if the interface that is the default
                // interface should be returned to this caller.
                //

                SeCaptureSubjectContext(&securitySubjectContext);

                status = PiPnpRtlApplyMandatoryFilters(PiPnpRtlCtx,
                                                       defaultInterface,
                                                       PNP_OBJECT_TYPE_DEVICE_INTERFACE,
                                                       hKey,
                                                       &securitySubjectContext,
                                                       &valid);

                SeReleaseSubjectContext(&securitySubjectContext);

                if (!NT_SUCCESS(status) || !valid) {
                    defaultPresent = FALSE;
                }
            }

            ZwClose(hKey);
        }

        if (!defaultPresent) {

            //
            // The default device interface shouldn't be present in the list.
            // Free up the resources for it.
            //

            PNP_ASSERT(defaultInterface != NULL);

            ExFreePool(defaultInterface);
            defaultInterface = NULL;
            defaultInterfaceSize = 0;
        }
    } else if ((status == STATUS_OBJECT_NAME_NOT_FOUND) ||
               (status == STATUS_OBJECT_PATH_NOT_FOUND) ||
               (status == STATUS_NOT_FOUND)) {

        //
        // Do nothing - there is no default.
        //

    } else {

        //
        // An unexpected error occured - clean up.
        //

        if (NT_SUCCESS(status)) {
            status = STATUS_UNSUCCESSFUL;
        }

        goto clean1;
    }

    if (DevicePath != NULL) {
        status = PnpUnicodeStringToWstr(&deviceInstance, NULL, DevicePath);

        if (!NT_SUCCESS(status)) {
            goto clean1;
        }
    }

    //
    // Set up the filter context so the default interface can be filtered out if
    // neccessary.
    //

    RtlZeroMemory(&filterContext, sizeof(filterContext));

    if (defaultPresent) {

        //
        // Only add the default interface to the context if the default
        // interface is present and is being added to the beginning of
        // the interface list.
        //

        filterContext.DefaultInterface = defaultInterface;
    }

    SeCaptureSubjectContext(&(filterContext.SecuritySubjectContext));

    PNP_ASSERT(interfaceList == NULL);
    interfaceListLength = INITIAL_SYMLINK_LIST_BUFFER_SIZE / sizeof(WCHAR);

    //
    // If the default interface is being added to the front of the list, make
    // sure the initial guess of the buffer size is at least large enough to
    // hold the default interface plus an extra character.
    //

    if (defaultPresent) {

        if (interfaceListLength <= (defaultInterfaceSize / sizeof(WCHAR))) {
            interfaceListLength = (defaultInterfaceSize / sizeof(WCHAR)) + 1;
        }
    }

    status = STATUS_BUFFER_TOO_SMALL;
    retryCount = 0;

    //
    // The list of device interfaces could change between calls so handle
    // getting back STATUS_BUFFER_TOO_SMALL multiple times.
    //

    while ((status == STATUS_BUFFER_TOO_SMALL) && (retryCount < MAX_GET_LIST_RETRIES)) {

        if (interfaceList != NULL) {
            ExFreePool(interfaceList);
        }

        interfaceList = ExAllocatePool(PagedPool, interfaceListLength * sizeof(WCHAR));

        if (interfaceList == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        //
        // Save away the amount of characters that can actually be stored in the
        // list so it can be used later.
        //

        tempInterfaceListLength = interfaceListLength;

        if (!defaultPresent) {

            //
            // There is no default interface or it isn't being added to the front
            // of the list.  The whole buffer that was just allocated can be used
            // to retrieve the list.
            //

            remainingList = interfaceList;
            remainingListLength = interfaceListLength;
        } else {

            //
            // The default interface needs to go at the beginning of the list.
            // The list should be long enough to hold at least the default
            // interface so this shouldn't return an error. Fail if it does
            // return an error.
            //

            status = RtlStringCchCopyExW(interfaceList,
                                         interfaceListLength,
                                         defaultInterface,
                                         &remainingList,
                                         &remainingListLength,
                                         STRSAFE_NULL_ON_FAILURE);

            if (!NT_SUCCESS(status)) {
                break;
            }

            //
            // The remaining data points to the null terminator of the default interface
            // so move it past that.
            //

            PNP_ASSERT(remainingListLength > 0);

            if (remainingListLength <= 0) {

                //
                // The list should be large enough to at least hold the default
                // interface plus a character, so this case shouldn't happen.
                // Return an error.
                //

                status = STATUS_UNSUCCESSFUL;
                break;
            }

            remainingListLength--;
            remainingList++;
        }

        //
        // Retrieve the list.  This will only use the unused portion of the buffer
        // if a default interface was added to the beginning.
        //

        status = _CmGetMatchingFilteredDeviceInterfaceList(PiPnpRtlCtx,
                                                           guidString,
                                                           deviceInstance,
                                                           (Flags & DEVICE_INTERFACE_INCLUDE_NONACTIVE) ?
                                                           0 : CM_DEVICE_INTERFACE_LIST_FILTER_PRESENT,
                                                           &IopDeviceInterfaceFilterCallback,
                                                           &filterContext,
                                                           remainingList,
                                                           (ULONG)remainingListLength,
                                                           &interfaceListLength,
                                                           0);

        //
        // If a default interface was already added to the beginning of the list,
        // it would have been filtered out and the required size would be missing
        // it.  To compensate, add on to the required size of the buffer the room
        // needed for the default interface.  If no default interface was added
        // to the beginning already, remainingList and interfaceList will be equal
        // so the required length will not change.
        //

        interfaceListLength += (ULONG)(remainingList - interfaceList);

        //
        // If the default interface is being included at the beginning of the list,
        // this should always be at least the length of the default interface plus
        // a character.
        //
        if (defaultPresent) {
            PNP_ASSERT(interfaceListLength > (defaultInterfaceSize / sizeof(WCHAR)));
        }

        retryCount++;
    }

    SeReleaseSubjectContext(&(filterContext.SecuritySubjectContext));

    if (!NT_SUCCESS(status)) {
        goto clean1;
    }

    //
    // If there are no matching interfaces, return an empty multi-sz list.
    //
    if (NT_SUCCESS(status) && (interfaceListLength == 0)) {
        interfaceListLength++;

        //
        // If the current interface list buffer doesn't have enough room for a
        // null terminator, allocate a buffer that does.
        //
        if (tempInterfaceListLength < 1) {
            ExFreePool(interfaceList);

            interfaceList = ExAllocatePool(PagedPool, interfaceListLength * sizeof(WCHAR));

            if (interfaceList == NULL) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                goto clean1;
            }
        }

        interfaceList[0] = UNICODE_NULL;
    }

    if (!NT_SUCCESS(status)) {
        goto clean1;
    }

    //
    // If we are returning kernel mode strings then patch the prefix.
    //

    if (!UserModeFormat) {

        for (currentInterface = interfaceList;
             *currentInterface != UNICODE_NULL;
             currentInterface += (wcslen(currentInterface) + 1)) {

            status = _CmSetDeviceInterfacePathFormat(PiPnpRtlCtx,
                                                     currentInterface,
                                                     TRUE);

            if (!NT_SUCCESS(status)) {
                goto clean1;
            }
        }
    }

clean1:

    PiUnlockPnpRegistry();

clean0:

    if (NT_SUCCESS(status)) {

        *SymbolicLinkList = interfaceList;

        if (ARGUMENT_PRESENT(SymbolicLinkListSize)) {
            *SymbolicLinkListSize = interfaceListLength * sizeof(WCHAR);
        }

        //
        // This buffer is being returned to the caller.  Set this pointer to null
        // now so it doesn't get freed below.
        //

        interfaceList = NULL;

    } else {

        *SymbolicLinkList = NULL;

        if (ARGUMENT_PRESENT(SymbolicLinkListSize)) {
            *SymbolicLinkListSize = 0;
        }
    }

    if (hClass != NULL) {
        ZwClose(hClass);
    }

    PnpUnicodeStringToWstrFree(deviceInstance, DevicePath);

    if (defaultInterface != NULL) {
        ExFreePool(defaultInterface);
    }

    if (interfaceList != NULL) {
        ExFreePool(interfaceList);
    }

    if (interfaceDevice != NULL) {
        ExFreePool(interfaceDevice);
    }

    return status;
}

__callback
BOOLEAN
IopDeviceInterfaceFilterCallback(
    __in HPNPCTX        hPnpCtx,
    __in PCWSTR         ObjectName,
    __in CM_OBJECT_TYPE ObjectType,
    __in_opt PVOID      Context
    )
{
    UNICODE_STRING currentInterface;
    UNICODE_STRING defaultInterface;
    PINTERFACE_FILTER_CONTEXT interfaceFilterContext;
    BOOLEAN match = FALSE;
    NTSTATUS status;

    PAGED_CODE();

    PNP_ASSERT(ObjectName != NULL);

    //
    // This function only knows how to filter for device interfaces.
    // If any other object type is specified, return an error.
    //

    if (ObjectType != CM_OBJECT_TYPE_DEVICE_INTERFACE) {
        PNP_ASSERT(match == FALSE);
        goto Clean0;
    }

    //
    // Context is required by this routine.
    //

    interfaceFilterContext = (PINTERFACE_FILTER_CONTEXT)Context;

    if (interfaceFilterContext == NULL) {
        PNP_ASSERT(match == FALSE);
        goto Clean0;
    }

    //
    // If there is a default interface specified and this interface matches
    // it, don't return it in the result set.  It will already be accounted
    // for by the caller.
    //

    if (interfaceFilterContext->DefaultInterface != NULL) {
        status = RtlInitUnicodeStringEx(&defaultInterface,
                                        interfaceFilterContext->DefaultInterface);

        if (NT_SUCCESS(status)) {
            status = RtlInitUnicodeStringEx(&currentInterface, ObjectName);
        }

        if (!NT_SUCCESS(status)) {
            PNP_ASSERT(match == FALSE);
            goto Clean0;
        }

        if (TRUE == RtlEqualUnicodeString(&defaultInterface,
                                          &currentInterface,
                                          TRUE)) {
            match = FALSE;
            goto Clean0;
        }
    }

    //
    // Now apply the mandatory device interface filters
    //

    status = PiPnpRtlApplyMandatoryFilters(hPnpCtx,
                                           ObjectName,
                                           PNP_OBJECT_TYPE_DEVICE_INTERFACE,
                                           NULL,
                                           &(interfaceFilterContext->SecuritySubjectContext),
                                           &match);

    if (!NT_SUCCESS(status)) {
        match = FALSE;
        goto Clean0;
    }

  Clean0:

    return match;
}

_IRQL_requires_max_(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
IoGetDeviceInterfaces(
    _In_ CONST GUID *InterfaceClassGuid,
    _In_opt_ PDEVICE_OBJECT PhysicalDeviceObject,
    _In_ ULONG Flags,
    _Outptr_result_nullonfailure_
    _At_(*SymbolicLinkList,
        _When_(return==0, __drv_allocatesMem(Mem)))
    PZZWSTR *SymbolicLinkList
    )

/*++

Routine Description:

    This API allows a WDM driver to get a list of paths that represent all
    device interfaces registered for the specified interface class.

Parameters:

    InterfaceClassGuid - Supplies a pointer to a GUID representing the interface
        class for whom a list of members is to be retrieved

    PhysicalDeviceObject - Optionally, supplies a pointer to the PDO for whom
        interfaces of the specified class are to be re-trieved.  If this
        parameteris not supplied, then all interface devices (regardless of what
        physical device exposes them) will be returned.

    Flags - Supplies flags that modify the behavior of list retrieval.
        The following flags are presently defined:

        DEVICE_INTERFACE_INCLUDE_NONACTIVE -- If this flag is specified, then
            all device interfaces, whether currently active or not, will be
            returned (potentially filtered based on the PhysicalDeviceObject, if
            specified).

    SymbolicLinkList - Supplies the address of a character pointer, that on
        success will contain a multi-sz list of \DosDevices\ symbolic link
        names that provide the requested functionality.  The caller is
        responsible for freeing the memory via ExFreePool

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    NTSTATUS status;
    PUNICODE_STRING pDeviceName = NULL;

    PAGED_CODE();

    //
    // Check we have a PDO and if so extract the instance path from it
    //
    if (ARGUMENT_PRESENT(PhysicalDeviceObject)) {

        ASSERT_PDO(PhysicalDeviceObject);

        pDeviceName = PnpGetDeviceInstancePath(PP_DO_TO_DN(PhysicalDeviceObject));
    }

    status = IopGetDeviceInterfaces(
                InterfaceClassGuid,
                pDeviceName,
                Flags,
                FALSE,
                SymbolicLinkList,
                NULL);

    return status;
}


__drv_maxIRQL(PASSIVE_LEVEL) __drv_valueIs(>=0;<0)
_Must_inspect_result_
NTSTATUS
IoSetDeviceInterfaceState(
    __in PUNICODE_STRING SymbolicLinkName,
    __in BOOLEAN Enable
    )

/*++

Routine Description:

    This DDI allows a device class to activate and deactivate an association
    previously registered using IoRegisterDeviceInterface

Parameters:

    SymbolicLinkName - Supplies a pointer to the symbolic link name which was
        returned by IoRegisterDeviceInterface when the interface was registered,
        or as returned by IoGetDeviceInterfaces.

    Enable - If TRUE (non-zero), the interface will be enabled.  If FALSE, it
        will be disabled.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    NTSTATUS status;

    PAGED_CODE();

    //
    // Enter critical section and acquire a lock on the registry.  Both these
    // mechanisms are required to prevent deadlock in the case where an APC
    // routine calls this routine after the registry resource has been claimed
    // in this case it would wait blocking this thread so the registry would
    // never be released -> deadlock.  Critical sectioning the registry
    // manipulation portion solves this problem
    //
    PiLockPnpRegistry(TRUE);

    status = IopProcessSetInterfaceState(SymbolicLinkName, Enable, TRUE);

    PiUnlockPnpRegistry();

    if (!NT_SUCCESS(status)) {

        //
        // If we failed to disable an interface (most likely because the
        // interface keys have already been deleted) report success.
        //
        if (!Enable) {

            status = STATUS_SUCCESS;
        }
    }

    return status;
}


__drv_maxIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
IoOpenDeviceInterfaceRegistryKey(
    __in PUNICODE_STRING SymbolicLinkName,
    __in ACCESS_MASK DesiredAccess,
    __out PHANDLE DeviceInterfaceKey
    )

/*++

Routine Description:

    This routine will open the registry key where the data associated with a
    specific device interface can be stored.

Parameters:

    SymbolicLinkName - Supplies a pointer to the symbolic link name which was
        returned by IoRegisterDeviceInterface when the device class was
        registered.

    DesiredAccess - Supplies the access privileges to the key the caller wants.

    DeviceInterfaceKey - Supplies a pointer to a handle which on success will
        contain the handle to the requested registry key.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    NTSTATUS status = STATUS_SUCCESS;
    PWSTR tempString = NULL;

    PAGED_CODE();

    //
    // Make sure we have a SymbolicLinkName.
    //

    if ((!ARGUMENT_PRESENT(SymbolicLinkName)) ||
        (SymbolicLinkName->Buffer == NULL)    ||
        (SymbolicLinkName->Length == 0)) {

        status = STATUS_INVALID_PARAMETER;
        goto clean0;
    }

    //
    // Make copy of the symbolic link name since the input buffer might
    // not be null-terminated
    //

    status = PnpUnicodeStringToWstr(&tempString, NULL, SymbolicLinkName);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    PiLockPnpRegistry(TRUE);

    status = _CmOpenDeviceInterfaceRegKey(PiPnpRtlCtx,
                                          tempString,
                                          CM_KEY_TYPE_DEVICE_INTERFACE_PARAMETERS,
                                          0,
                                          DesiredAccess,
                                          TRUE,
                                          DeviceInterfaceKey,
                                          NULL);

    PiUnlockPnpRegistry();

clean0:

    PnpUnicodeStringToWstrFree(tempString, SymbolicLinkName);

    return status;
}

__drv_maxIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
IoRegisterDeviceInterface(
    __in PDEVICE_OBJECT PhysicalDeviceObject,
    __in CONST GUID *InterfaceClassGuid,
    __in_opt PUNICODE_STRING ReferenceString,
    __out __drv_when(return==0,
                     __drv_at(SymbolicLinkName->Buffer, __drv_allocatesMem(Mem)))
    PUNICODE_STRING SymbolicLinkName
    )

/*++

Routine Description:

    This device driver interface allows a WDM driver to register a particular
    interface of its underlying hardware (ie PDO) as a member of a function
    class.

Parameters:

    PhysicalDeviceObject - Supplies a pointer to the PDO for the P&P device
        instance associated with the functionality being registered

    InterfaceClassGuid - Supplies a pointer to the GUID representring the
        functionality to be registered

    ReferenceString - Optionally, supplies an additional context string which is
        appended to the enumeration path of the device

    SymbolicLinkName - Supplies a pointer to a string which on success will
        contain the kernel mode path of the symbolic link used to open this
        device.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    PDEVICE_NODE deviceNode;
    PUNICODE_STRING deviceInstanceId;
    ULONG length;
    ULONG pdoFlags;
    NTSTATUS status = STATUS_SUCCESS;
    PWSTR symbolicLink = NULL;
    PWSTR refString = NULL;

    PAGED_CODE();

    RtlZeroMemory(SymbolicLinkName, sizeof(UNICODE_STRING));

    //
    // Until PartMgr/Disk stop registering non PDOs allow the system to boot.
    //
    // ASSERT_PDO(PhysicalDeviceObject);
    //
    if (!IS_PDO(PhysicalDeviceObject)) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        goto clean0;
    }

    //
    // Ensure we have a PDO - only PDO's have a device node attached
    //

    deviceNode = PP_DO_TO_DN(PhysicalDeviceObject);

    if ((deviceNode == NULL) || (PnpIsDeviceInstancePathEmpty(deviceNode))) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        goto clean0;
    }

    //
    // Get the instance path string
    //

    deviceInstanceId = PnpGetDeviceInstancePath(deviceNode);

    //
    // Make sure that the PDO is named.
    //
    length = 0;
    status = ObQueryNameString(PhysicalDeviceObject,
                               NULL,
                               0,
                               &length);
    if (length <= sizeof(OBJECT_NAME_INFORMATION)) {
        PNP_ASSERTMSG("Attempt to register an interface on an unnamed PDO!!!",
                      length > sizeof(OBJECT_NAME_INFORMATION));
        status = STATUS_INVALID_DEVICE_REQUEST;
        goto clean0;
    }

    //
    // Only try to convert the reference string from PUNICODE_STRING into PWSTR
    // if it is present and it is of non-zero length.  An empty string is not
    // considered a valid refstring, but we ignore it here so it doesn't generate
    // an error.  This is to remain compatible with previous behavior of this API.
    //

    if ((ReferenceString != NULL) && (ReferenceString->Length >= sizeof(WCHAR))) {
        status = PnpUnicodeStringToWstr(&refString, NULL, ReferenceString);

        if (!NT_SUCCESS(status)) {
            goto clean0;
        }

        //
        // Check that the reference string does not contain a path seperator.
        // This same check is performed by PnPRtl, but that check returns a
        // different status value.  For compatibility we want to use the
        // status value of STATUS_INVALID_DEVICE_REQUEST for this case.
        //

        if (wcspbrk(refString, L"\\/") != NULL) {
            status = STATUS_INVALID_DEVICE_REQUEST;
            IopDbgPrint((   IOP_ERROR_LEVEL,
                            "IoRegisterDeviceInterface: Invalid RefString!! failed with status = %8.8X\n",
                            status));
            goto clean0;
        }
    }

    pdoFlags = 0;

    status = IopRegisterDeviceInterface(deviceInstanceId->Buffer,
                                        InterfaceClassGuid,
                                        refString,
                                        FALSE,           // kernel-mode format
                                        &symbolicLink,
                                        &pdoFlags);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    PhysicalDeviceObject->Flags |= pdoFlags;

    status = RtlInitUnicodeStringEx(SymbolicLinkName, symbolicLink);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    symbolicLink = NULL;

clean0:

    if (symbolicLink) {
        ExFreePool(symbolicLink);
    }

    PnpUnicodeStringToWstrFree(refString, ReferenceString);

    return status;
}

BOOLEAN
PipCheckForDenyExecute(
    __in PCWSTR InterfaceClass
    )
/*++

Routine Description:

    This routine checks for a group policy setting that disallows execution
    for a specific device class.  If set, a flag gets set on the PDO which
    tells the memory manager to disallow execution on the device.

Parameters:

    InterfaceClass - The GUID string representing the class which should be checked.

Return Value:

    TRUE if deny execute policy is set for this class, FALSE otherwise.

--*/
{
    ULONG denyValue;
    PKEY_VALUE_FULL_INFORMATION keyValueInformation;
    HANDLE regKeyHandle = NULL;
    PWSTR regKeyPath = NULL;
    BOOLEAN retVal = FALSE;
    NTSTATUS status = STATUS_SUCCESS;
    UNICODE_STRING unicodeString;

    PAGED_CODE();

    //
    // Check if this is for disk devices. The volume manager will
    // handle this case.
    //
    if (_wcsicmp(REG_DISK_KEY, InterfaceClass) == 0) {
        goto clean0;
    }

    status = PnpConcatPWSTR(REG_MAX_KEY_NAME_LENGTH,
                            PNP_POOLTAG_GENERIC,
                            &regKeyPath,
                            3,
                            REG_DENY_EXECUTE,
                            L"\\",
                            InterfaceClass);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    RtlInitUnicodeString(&unicodeString, regKeyPath);

    status = IopOpenRegistryKeyEx(&regKeyHandle,
                                  NULL,
                                  &unicodeString,
                                  KEY_READ);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    status = IopGetRegistryValue(regKeyHandle,
                                 L"Deny_Execute",
                                 0,
                                 &keyValueInformation);

    if (!NT_SUCCESS(status) || !keyValueInformation){
        goto clean0;
    }

    if ((keyValueInformation->Type == REG_DWORD) &&
        (keyValueInformation->DataLength == sizeof(ULONG))){

        denyValue = *(PULONG)(KEY_VALUE_DATA(keyValueInformation));

        if (denyValue != 0) {
            retVal = TRUE;
        }
    }

clean0:

    if (regKeyHandle) {
        ZwClose(regKeyHandle);
    }

    if (regKeyPath) {
        ExFreePoolWithTag(regKeyPath, PNP_POOLTAG_GENERIC);
    }

    return retVal;
}


NTSTATUS
IopRegisterDeviceInterface(
    __in PCWSTR DeviceInstanceName,
    __in CONST GUID *InterfaceClassGuid,
    __in_opt PCWSTR ReferenceString,
    __in BOOLEAN UserModeFormat,
    __deref_out PWSTR *SymbolicLinkName,
    __out_opt PULONG PdoFlags
    )

/*++

Routine Description:

    This is the worker routine for IoRegisterDeviceInterface.  It is also
    called by the user-mode ConfigMgr (via an NtPlugPlayControl), which is why it
    must take a device instance name instead of a PDO (since the device instance
    may not currently be 'live'), and also why it must optionally return the user-
    mode form of the interface device name (i.e., "\\?\" instead of "\??\").

Parameters:

    DeviceInstanceName - Supplies the name of the device instance for which a
        device interface is being registered.

    InterfaceClassGuid - Supplies a pointer to the GUID representring the class
        of the device interface being registered.

    ReferenceString - Optionally, supplies an additional context string which is
        appended to the enumeration path of the device

    UserModeFormat - If non-zero, then the symbolic link name returned for the
        interface device is in user-mode form (i.e., "\\?\").  If zero (FALSE),
        it is in kernel-mode form (i.e., "\??\").

    SymbolicLinkName - Supplies a pointer to a string which on success will contain
        either the kernel-mode or user-mode path of the symbolic link used to open
        this device.

    PdoFlags - Supplies a pointer to a variable that will hold any flags that need
        to be applied to a PDO associated with this device interface.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    GUID containerId;
    WCHAR containerIdStr[GUID_STRING_LEN];
    PPI_DM_OBJECT containerObject = NULL;
    HANDLE deviceInterfaceKey = NULL;
    PPI_DM_OBJECT deviceObject = NULL;
    BOOLEAN hasRegistryBacking = FALSE;
    HPNPOPCTX hPnpOpCtx = NULL;
    BOOLEAN interfaceCreated = FALSE;
    PPI_DM_OBJECT interfaceObject = NULL;
    PPI_DM_OBJECT interfaceClassObject = NULL;
    WCHAR interfaceClassStr[GUID_STRING_LEN];
    ULONG propType;
    ULONG propSize;
    NTSTATUS status;
    ULONG symbolicLinkLength;

    PAGED_CODE();

    *SymbolicLinkName = NULL;

    //
    // Convert the class guid into string form
    //

    status = PnpStringFromGuid(InterfaceClassGuid,
                               interfaceClassStr,
                               RTL_NUMBER_OF(interfaceClassStr));

    if (!NT_SUCCESS(status)){
        goto clean0;
    }

    symbolicLinkLength = INITIAL_SYMLINK_BUFFER_SIZE / sizeof(WCHAR);

    *SymbolicLinkName = ExAllocatePool(PagedPool, symbolicLinkLength * sizeof(WCHAR));

    if (*SymbolicLinkName == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto clean0;
    }

    //
    // Always retrieve the interface name in the Win32 path format since that
    // format needs to be used when writing the ETW event indicating an
    // interface has been registered.
    //

    status = _CmGetDeviceInterfaceName(PiPnpRtlCtx,
                                       interfaceClassStr,
                                       DeviceInstanceName,
                                       ReferenceString,
                                       FALSE,
                                       *SymbolicLinkName,
                                       symbolicLinkLength,
                                       &symbolicLinkLength);

    if (status == STATUS_BUFFER_TOO_SMALL) {
        ExFreePool(*SymbolicLinkName);

        *SymbolicLinkName = ExAllocatePool(PagedPool,
                                           symbolicLinkLength * sizeof(WCHAR));

        if (*SymbolicLinkName == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto clean0;
        }

        status = _CmGetDeviceInterfaceName(PiPnpRtlCtx,
                                           interfaceClassStr,
                                           DeviceInstanceName,
                                           ReferenceString,
                                           FALSE,
                                           *SymbolicLinkName,
                                           symbolicLinkLength,
                                           &symbolicLinkLength);

        PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);
    }

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    //
    // Check for existing group policy key that says that this class is not
    // allowed to execute files.
    //

    if (ARGUMENT_PRESENT(PdoFlags)) {
        if (PipCheckForDenyExecute(interfaceClassStr)) {
            *PdoFlags = DO_DISALLOW_EXECUTE;
        } else {
            *PdoFlags = 0;
        }
    }

    //
    // Take the registry lock to make sure that the interface is not deleted
    // after we checked that it has registry backing.
    //

    PiLockPnpRegistry(TRUE);

    //
    // Check if the interface already exists in the cache.
    // If it does and has registry backing then there is no need
    // to do any registry operations.
    //

    status = PiDmGetObject(PNP_OBJECT_TYPE_DEVICE_INTERFACE,
                           *SymbolicLinkName,
                           &interfaceObject);

    if (NT_SUCCESS(status)) {

        PiDmObjectAcquireSharedLock(interfaceObject);

        hasRegistryBacking = (interfaceObject->Flags.HasRegistryBacking != FALSE);

        PiDmObjectReleaseLock(interfaceObject);
    }

    //
    // Get a cache reference to all the constraint objects now before attempting
    // to perform any registry operations.  Note that in the
    // IoReportDetectedDevice path its possible to create an interface for a
    // device that does not yet have registry backing.  We can't assume that the
    // objects exists in the cache already.
    //

    status = PiDmAddCacheReferenceForObject(PNP_OBJECT_TYPE_DEVICE_INTERFACE_CLASS,
                                            interfaceClassStr,
                                            &interfaceClassObject);

    if (!NT_SUCCESS(status)) {
        goto clean1;
    }

    status = PiDmAddCacheReferenceForObject(PNP_OBJECT_TYPE_DEVICE,
                                            DeviceInstanceName,
                                            &deviceObject);

    if (!NT_SUCCESS(status)) {
        goto clean1;
    }

    //
    // Get the container Id for the device because we'll need it later.
    //

    status = _PnpGetObjectProperty(PiPnpRtlCtx,
                                   DeviceInstanceName,
                                   PNP_OBJECT_TYPE_DEVICE,
                                   NULL,
                                   NULL,
                                   &DEVPKEY_Device_ContainerId,
                                   &propType,
                                   (PUCHAR) &containerId,
                                   sizeof(containerId),
                                   &propSize,
                                   0);

    if (status == STATUS_NOT_FOUND) {
        RtlZeroMemory(&containerId, sizeof(GUID));
        status = STATUS_SUCCESS;

    } else if (!NT_SUCCESS(status)) {
        goto clean1;

    } else {
        NT_ASSERT(propType == DEVPROP_TYPE_GUID);
        NT_ASSERT(propSize == sizeof(GUID));
    }

    if (!PnpIsNullGuid(&containerId)) {

        status = PnpStringFromGuid(&containerId,
                                   containerIdStr,
                                   RTL_NUMBER_OF(containerIdStr));

        NT_ASSERT(NT_SUCCESS(status));

        if (!NT_SUCCESS(status)) {
            goto clean1;
        }

        status = PiDmAddCacheReferenceForObject(PNP_OBJECT_TYPE_DEVICE_CONTAINER,
                                                containerIdStr,
                                                &containerObject);

        if (!NT_SUCCESS(status)) {
            goto clean1;
        }
    }

    if (!hasRegistryBacking) {

        //
        // Begin an opereration.
        //

        status = PiPnpRtlBeginOperation(&hPnpOpCtx);
        if ( !NT_SUCCESS(status) ){
            goto clean1;
        }

        //
        // Create the device interface.
        //

        status = _CmCreateDeviceInterface(PiPnpRtlCtx,
                                          *SymbolicLinkName,
                                          KEY_SET_VALUE,
                                          &deviceInterfaceKey,
                                          &interfaceCreated,
                                          0);

        if (!NT_SUCCESS(status)){
            goto clean1;
        }
    }

    //
    // If we don't already have the interface object, get it now after it's been
    // created.
    //

    if (!interfaceObject) {
        status = PiDmGetObject(PNP_OBJECT_TYPE_DEVICE_INTERFACE,
                               *SymbolicLinkName,
                               &interfaceObject);

        NT_ASSERT(NT_SUCCESS(status));

        if (!NT_SUCCESS(status)) {
            goto clean1;
        }
    }

    //
    // Add the interface to the interface class constraint list.
    //

    PiDmListAddObject(PiDmListInterfacesInInterfaceClass,
                      interfaceClassObject,
                      interfaceObject,
                      NULL);

    //
    // Set the device instance id on the interface. Do this step even if the
    // interface already exists because this step is needed to fix up cases
    // where the interface exists but the device instance id set previously
    // failed and after that the interface could not be deleted.
    //

    status = _PnpSetObjectProperty(PiPnpRtlCtx,
                                   *SymbolicLinkName,
                                   PNP_OBJECT_TYPE_DEVICE_INTERFACE,
                                   deviceInterfaceKey,
                                   NULL,
                                   &DEVPKEY_Device_InstanceId,
                                   DEVPROP_TYPE_STRING,
                                   (PUCHAR) DeviceInstanceName,
                                   (ULONG)((wcslen(DeviceInstanceName) + 1) * sizeof(WCHAR)),
                                   (interfaceCreated != FALSE) ?
                                       PIPNPRTL_FLAG_WRITE_THROUGH : 0);

    if (NT_SUCCESS(status)) {

        //
        // After the device instance Id has been set, update the
        // device and container constraint lists.
        //

        PiDmListAddObject(PiDmListInterfacesInDevice,
                          deviceObject,
                          interfaceObject,
                          NULL);

        if (containerObject) {

            PiDmListAddObject(PiDmListInterfacesInContainer,
                              containerObject,
                              interfaceObject,
                              NULL);
        }

    } else {

        if (interfaceCreated) {

            //
            // The interface failed to have it's device instance id property set
            // If this call is the one that creates the interface, attempt to
            // delete the interface because a interface without a device
            // instance is not usable.
            //
            // Don't check the return code of the delete since there is nothing
            // that can be done if it fails.
            //

            _CmDeleteDeviceInterface(PiPnpRtlCtx,
                                     *SymbolicLinkName,
                                     0);

            interfaceCreated = FALSE;
        }

        goto clean1;
    }

clean1:

    PiUnlockPnpRegistry();

clean0:

    if (!NT_SUCCESS(status)) {
        if (*SymbolicLinkName) {
            ExFreePool(*SymbolicLinkName);
            *SymbolicLinkName = NULL;
        }
    } else if (!UserModeFormat) {
        //
        // Since the symbolic link name is currently in the Win32 path format but
        // the caller requested the NT path format, convert the format.
        //
        status = _CmSetDeviceInterfacePathFormat(PiPnpRtlCtx,
                                                 *SymbolicLinkName,
                                                 TRUE);
    }

    if (deviceInterfaceKey) {
        _PnpCtxRegCloseKey(PiPnpRtlCtx, deviceInterfaceKey);
    }

    if (hPnpOpCtx) {
        PiPnpRtlEndOperation(hPnpOpCtx);
    }

    if (interfaceObject) {
        PiDmObjectRelease(interfaceObject);
    }

    if (interfaceClassObject) {
        PiDmRemoveCacheReferenceForObject(interfaceClassObject->Type,
                                          interfaceClassObject->Name,
                                          NULL);
        PiDmObjectRelease(interfaceClassObject);
    }

    if (deviceObject) {
        PiDmRemoveCacheReferenceForObject(deviceObject->Type,
                                          deviceObject->Name,
                                          NULL);
        PiDmObjectRelease(deviceObject);
    }

    if (containerObject) {
        PiDmRemoveCacheReferenceForObject(containerObject->Type,
                                          containerObject->Name,
                                          NULL);
        PiDmObjectRelease(containerObject);
    }

    return status;
}


NTSTATUS
PnpDisableDeviceInterfaces(
    __in PUNICODE_STRING DeviceInstancePath
    )
/*++

Routine Description:

    This routine disables all enabled device interfaces for a given device
    instance.  This is typically done after a device has been removed, in case
    the driver did not disable the interfaces for that device, as it should
    have.

    Note that this routine acquires a lock on the registry.

Parameters:

    DeviceInterfacePath - Supplies a pointer to a unicode string which contains
                          the DeviceInterface name of the device for which
                          interfaces to are to be disabled.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/
{
    PWSTR deviceInstance = NULL;
    PWSTR interfaces = NULL;
    ULONG interfacesLength;
    ULONG retryCount;
    NTSTATUS status = STATUS_SUCCESS;
    PWCHAR symLink;
    UNICODE_STRING tempString;

    PAGED_CODE();

    //
    // Enter critical section and acquire a lock on the registry.  Both these
    // mechanisms are required to prevent deadlock in the case where an APC
    // routine calls this routine after the registry resource has been claimed
    // in this case it would wait blocking this thread so the registry would
    // never be released -> deadlock.  Critical sectioning the registry
    // manipulation portion solves this problem.
    //

    PiLockPnpRegistry(TRUE);

    status = PnpUnicodeStringToWstr(&deviceInstance, NULL, DeviceInstancePath);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    interfacesLength = INITIAL_SYMLINK_LIST_BUFFER_SIZE / sizeof(WCHAR);

    status = STATUS_BUFFER_TOO_SMALL;
    retryCount = 0;

    //
    // The list of device interfaces could change between calls so handle
    // getting back STATUS_BUFFER_TOO_SMALL multiple times.
    //

    while ((status == STATUS_BUFFER_TOO_SMALL) && (retryCount < MAX_GET_LIST_RETRIES)) {

        if (interfaces != NULL) {
            ExFreePool(interfaces);
        }

        interfaces = ExAllocatePool(PagedPool, interfacesLength * sizeof(WCHAR));

        if (interfaces == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        status = _CmGetMatchingFilteredDeviceInterfaceList(PiPnpRtlCtx,
                                                           NULL,
                                                           deviceInstance,
                                                           CM_DEVICE_INTERFACE_LIST_FILTER_PRESENT,
                                                           NULL,
                                                           NULL,
                                                           interfaces,
                                                           interfacesLength,
                                                           &interfacesLength,
                                                           0);

        retryCount++;
    }

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    //
    // Iterate through all enabled instances of a device interface
    // exposed by the given device instance.
    //
    symLink = interfaces;
    while (*symLink != UNICODE_NULL) {

        status = RtlInitUnicodeStringEx(&tempString, symLink);

        if (NT_SUCCESS(status)) {
            IopDbgPrint((IOP_WARNING_LEVEL,
                         "PnpDisableDeviceInterfaces: auto-disabling interface %Z for device instance %Z\n",
                         tempString,
                         DeviceInstancePath));

            //
            // Disable this device interface.
            //
            IoSetDeviceInterfaceState(&tempString, FALSE);

        } else {
            IopDbgPrint((IOP_ERROR_LEVEL,
                         "PnpDisableDeviceInterfaces: failed to auto-disable interface for device instance %Z.  failed with status = %8.8X\n",
                         DeviceInstancePath,
                         status));
        }

        symLink += ((tempString.Length + sizeof(UNICODE_NULL)) / sizeof(WCHAR));
    }

  clean0:

    PiUnlockPnpRegistry();

    if (interfaces != NULL) {
        ExFreePool(interfaces);
    }

    PnpUnicodeStringToWstrFree(deviceInstance, DeviceInstancePath);

    return status;
}


NTSTATUS
PnpDeleteDeviceInterfaces(
    __in PUNICODE_STRING DeviceInstancePath
    )
/*++

Routine Description:

    This routine deletes all device interfaces for a given device instance.

    Note that this routine acquires a lock on the registry.

Parameters:

    DeviceInstancePath - Supplies a pointer to a unicode string which contains
                         the DeviceInstancePath of the device for which
                         interfaces to are to be deleted.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/
{
    PWSTR deviceInstance = NULL;
    PWSTR interfaces = NULL;
    ULONG interfacesLength;
    ULONG retryCount;
    NTSTATUS status = STATUS_SUCCESS;
    PWCHAR symLink;
    UNICODE_STRING tempString;

    PAGED_CODE();

    PiLockPnpRegistry(TRUE);

    status = PnpUnicodeStringToWstr(&deviceInstance, NULL, DeviceInstancePath);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    interfacesLength = INITIAL_SYMLINK_LIST_BUFFER_SIZE / sizeof(WCHAR);

    status = STATUS_BUFFER_TOO_SMALL;
    retryCount = 0;

    //
    // The list of device interfaces could change between calls so handle
    // getting back STATUS_BUFFER_TOO_SMALL multiple times.
    //

    while ((status == STATUS_BUFFER_TOO_SMALL) && (retryCount < MAX_GET_LIST_RETRIES)) {

        if (interfaces != NULL) {
            ExFreePool(interfaces);
        }

        interfaces = ExAllocatePool(PagedPool, interfacesLength * sizeof(WCHAR));

        if (interfaces == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            break;
        }

        status = _CmGetMatchingFilteredDeviceInterfaceList(PiPnpRtlCtx,
                                                           NULL,
                                                           deviceInstance,
                                                           CM_DEVICE_INTERFACE_LIST_FILTER_NONE,
                                                           NULL,
                                                           NULL,
                                                           interfaces,
                                                           interfacesLength,
                                                           &interfacesLength,
                                                           0);

        retryCount++;
    }

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    //
    // Iterate through all device interfaces exposed by device instance.
    //
    symLink = interfaces;
    while (*symLink != UNICODE_NULL) {

        status = RtlInitUnicodeStringEx(&tempString, symLink);

        if (NT_SUCCESS(status)) {
            IopDbgPrint((IOP_WARNING_LEVEL,
                         "PnpDeleteDeviceInterfaces: deleting interface %Z for device instance %Z\n",
                         tempString,
                         DeviceInstancePath));

            //
            // Delete this device interface.
            //
            _CmDeleteDeviceInterface(PiPnpRtlCtx, tempString.Buffer, 0);
        }

        symLink += ((tempString.Length + sizeof(UNICODE_NULL)) / sizeof(WCHAR));
    }

  clean0:

    PiUnlockPnpRegistry();

    if (interfaces != NULL) {
        ExFreePool(interfaces);
    }

    PnpUnicodeStringToWstrFree(deviceInstance, DeviceInstancePath);

    return status;
}


__drv_maxIRQL(PASSIVE_LEVEL)
_Must_inspect_result_
NTSTATUS
IoGetDeviceInterfaceAlias(
    __in PUNICODE_STRING SymbolicLinkName,
    __in CONST GUID *AliasInterfaceClassGuid,
    __out
    __drv_when(return==0,
               __drv_at(AliasSymbolicLinkName->Buffer, __drv_allocatesMem(Mem)))
    PUNICODE_STRING AliasSymbolicLinkName
    )

/*++

Routine Description:

    This API returns a symbolic link name (i.e., device interface) of a
    particular interface class that 'aliases' the specified device interface.
    Two device interfaces are considered aliases of each other if the
    following two criteria are met:

        1.  Both interfaces are exposed by the same PDO (devnode).
        2.  Both interfaces share the same RefString.

Parameters:

    SymbolicLinkName - Supplies the name of the device interface whose alias is
        to be retrieved.

    AliasInterfaceClassGuid - Supplies a pointer to the GUID representing the
        interface class for which an alias is to be retrieved.

    AliasSymbolicLinkName - Supplies a pointer to a string which, upon success,
        will contain the name of the device interface in the specified class that
        aliases the SymbolicLinkName interface.  (This symbolic link name will be
        returned in either kernel-mode or user-mode form, depeding upon the form
        of the SymbolicLinkName path).

        It is the caller's responsibility to free the buffer allocated for this
        string via ExFreePool().

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    BOOLEAN accessAllowed = FALSE;
    SECURITY_SUBJECT_CONTEXT securitySubjectContext;
    PWSTR alias = NULL;
    ULONG aliasLength;
    PWSTR deviceInstance = NULL;
    WCHAR guidString[GUID_STRING_LEN];
    HANDLE hKey;
    BOOLEAN kernelModeFormat;
    ULONG propertySize;
    DEVPROPTYPE propertyType;
    PWSTR referenceString = NULL;
    ULONG referenceStringLength;
    ULONG requiredSize;
    NTSTATUS status;
    PWSTR symbolicLink = NULL;

    PAGED_CODE();

    //
    // Make sure we have a SymbolicLinkName to parse.
    //

    if ((!ARGUMENT_PRESENT(SymbolicLinkName)) ||
        (SymbolicLinkName->Buffer == NULL)    ||
        (SymbolicLinkName->Length == 0)) {
        status = STATUS_INVALID_PARAMETER;
        goto clean0;
    }

    status = PnpUnicodeStringToWstr(&symbolicLink, NULL, SymbolicLinkName);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    //
    // Check that the input symbolic link is valid.
    //

    status = _CmValidateDeviceInterfaceName(PiPnpRtlCtx, symbolicLink);

    if (!NT_SUCCESS(status)) {
        status = STATUS_INVALID_PARAMETER;
        goto clean0;
    }

    //
    // Check if the caller has access to the interface they want an alias of.
    //

    SeCaptureSubjectContext(&securitySubjectContext);

    status = PiPnpRtlApplyMandatoryFilters(PiPnpRtlCtx,
                                           symbolicLink,
                                           PNP_OBJECT_TYPE_DEVICE_INTERFACE,
                                           NULL,
                                           &securitySubjectContext,
                                           &accessAllowed);

    SeReleaseSubjectContext(&securitySubjectContext);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    if (!accessAllowed) {

        //
        // If not, then the caller does not have access to find an alias for this
        // device.
        //

        IopDbgPrint((IOP_TRACE_LEVEL,
                     "IoGetDeviceInterfaceAlias : device gets filtered !!!! \n\n"));

        status = STATUS_ACCESS_DENIED;
        goto clean0;
    }

    //
    // Convert the class guid into string form.
    //

    status = PnpStringFromGuid(AliasInterfaceClassGuid, guidString, RTL_NUMBER_OF(guidString));
    if ( !NT_SUCCESS(status) ){
        goto clean0;
    }

    //
    // Enter critical section and acquire a lock on the registry.  Both these
    // mechanisms are required to prevent deadlock in the case where an APC
    // routine calls this routine after the registry resource has been claimed
    // in this case it would wait blocking this thread so the registry would
    // never be released -> deadlock.  Critical sectioning the registry manipulation
    // portion solves this problem.
    //

    PiLockPnpRegistry(TRUE);

    //
    // Get the name of the device instance that 'owns' this interface.
    //

    propertySize = MAX_DEVICE_ID_LEN * sizeof(WCHAR);

    deviceInstance = ExAllocatePool(PagedPool, propertySize);

    if (!deviceInstance) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto clean1;
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
                                   &requiredSize,
                                   0);

    //
    // If this device instance was stored against an interface, it should be
    // valid so retrieving this property shouldn't return buffer too small.
    //

    PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);

    if (!NT_SUCCESS(status)) {
        goto clean1;
    }

    if (propertyType != DEVPROP_TYPE_STRING) {
        status = STATUS_INVALID_PARAMETER_1;
        goto clean1;

    }

    //
    // Now parse out the refstring, so that we can construct the name of the
    // interface device's alias.  (NOTE: we have not yet verified that the alias
    // actually exists, we're only constructing what its name would be, if it did
    // exist.)
    //

    referenceStringLength = INITIAL_REFERENCE_STRING_BUFFER_SIZE / sizeof(WCHAR);
    referenceString = ExAllocatePool(PagedPool, referenceStringLength * sizeof(WCHAR));

    if (referenceString == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto clean1;
    }

    status = _CmGetDeviceInterfaceReferenceString(PiPnpRtlCtx,
                                                  symbolicLink,
                                                  referenceString,
                                                  referenceStringLength,
                                                  &referenceStringLength);

    if (status == STATUS_BUFFER_TOO_SMALL) {
        ExFreePool(referenceString);

        referenceString = ExAllocatePool(PagedPool, referenceStringLength * sizeof(WCHAR));

        if (referenceString == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto clean1;
        }

        status = _CmGetDeviceInterfaceReferenceString(PiPnpRtlCtx,
                                                      symbolicLink,
                                                      referenceString,
                                                      referenceStringLength,
                                                      &referenceStringLength);

        PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);
    }

    if (status == STATUS_OBJECT_NAME_NOT_FOUND) {

        //
        // This symbolic link does not have a refstring. This is not an error.
        //

        status = STATUS_SUCCESS;
        ExFreePool(referenceString);
        referenceString = NULL;
    }

    if (!NT_SUCCESS(status)) {
        goto clean1;
    }

    //
    // Did the caller supply us with a user-mode or kernel-mode format path?
    //

    status = _CmGetDeviceInterfacePathFormat(PiPnpRtlCtx,
                                             symbolicLink,
                                             &kernelModeFormat);

    if (!NT_SUCCESS(status)) {
        goto clean1;
    }

    aliasLength = INITIAL_SYMLINK_BUFFER_SIZE / sizeof(WCHAR);

    alias = ExAllocatePool(PagedPool, aliasLength * sizeof(WCHAR));

    if (alias == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto clean1;
    }

    status = _CmGetDeviceInterfaceName(PiPnpRtlCtx,
                                       guidString,
                                       deviceInstance,
                                       referenceString,
                                       kernelModeFormat,
                                       alias,
                                       aliasLength,
                                       &aliasLength);

    if (status == STATUS_BUFFER_TOO_SMALL) {
        ExFreePool(alias);

        alias = ExAllocatePool(PagedPool, aliasLength * sizeof(WCHAR));

        if (alias == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto clean1;
        }

        status = _CmGetDeviceInterfaceName(PiPnpRtlCtx,
                                           guidString,
                                           deviceInstance,
                                           referenceString,
                                           kernelModeFormat,
                                           alias,
                                           aliasLength,
                                           &aliasLength);

        PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);
    }

    if (!NT_SUCCESS(status)) {
        goto clean1;
    }

    status = RtlInitUnicodeStringEx(AliasSymbolicLinkName, alias);

    if (!NT_SUCCESS(status)) {
        goto clean1;
    }

    //
    // OK, we now have the symbolic link name of the alias, but we don't yet
    // know whether it actually exists.  Check this by attempting to open the
    // associated registry key.
    //

    status = _CmOpenDeviceInterfaceRegKey(PiPnpRtlCtx,
                                          alias,
                                          CM_KEY_TYPE_DEVICE_INTERFACE_INSTANCE,
                                          0,
                                          KEY_READ,
                                          FALSE,
                                          &hKey,
                                          NULL);

    if (NT_SUCCESS(status)) {

        //
        // Alias exists--close the key handle.
        //

        ZwClose(hKey);

    } else {

        RtlFreeUnicodeString(AliasSymbolicLinkName);
    }

clean1:

    PiUnlockPnpRegistry();

clean0:

    if (referenceString != NULL) {
        ExFreePool(referenceString);
    }

    if (deviceInstance != NULL) {
        ExFreePool(deviceInstance);
    }

    PnpUnicodeStringToWstrFree(symbolicLink, SymbolicLinkName);

    return status;
}


NTSTATUS
IopBuildGlobalSymbolicLinkString(
    __in  PUNICODE_STRING SymbolicLink,
    __out PUNICODE_STRING GlobalString
    )
/*++

Routine Description:

    This routine will construct the global symbolic link name for the given
    kernel-mode or user-mode relative symbolic link.

Parameters:

    SymbolicLink - Supplies a pointer to a symbolic link string. Both the
        kernel-mode and user-mode prefixed strings are valid.

    GlobalString - Supplies a pointer to an uninitialised string which on
        success will contain the string that represents the symbolic link
        withing the global namespace.  It is of the format
        \GLOBAL??\<MungedDeviceString>\<GuidString>\<Reference>. When no longer
        required it should be freed using RtlFreeUnicodeString.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    USHORT globalStringSize;
    NTSTATUS status;
    PWSTR symbolicLinkName = NULL;
    ULONG symbolicLinkNameLength;
    PWSTR symbolicLinkString = NULL;

    PAGED_CODE();

    status = PnpUnicodeStringToWstr(&symbolicLinkString, NULL, SymbolicLink);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    //
    // Get the symbolic link name for the symbolic link.  This is the symbolic
    // link with the prefix and reference string stripped off.
    //

    symbolicLinkNameLength = INITIAL_SYMLINK_BUFFER_SIZE / sizeof(WCHAR);
    symbolicLinkName = ExAllocatePool(PagedPool, symbolicLinkNameLength * sizeof(WCHAR));

    if (symbolicLinkName == NULL) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto clean0;
    }

    status = _CmGetDeviceInterfaceSymbolicLinkName(PiPnpRtlCtx,
                                                   symbolicLinkString,
                                                   symbolicLinkName,
                                                   symbolicLinkNameLength,
                                                   &symbolicLinkNameLength);

    if (status == STATUS_BUFFER_TOO_SMALL) {

        //
        // The buffer was too small to hold the symbolic link name.  Reallocate
        // and try retrieving the name again.
        //

        ExFreePool(symbolicLinkName);

        symbolicLinkName = ExAllocatePool(PagedPool, symbolicLinkNameLength * sizeof(WCHAR));

        if (symbolicLinkName == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            goto clean0;
        }

        status = _CmGetDeviceInterfaceSymbolicLinkName(PiPnpRtlCtx,
                                                       symbolicLinkString,
                                                       symbolicLinkName,
                                                       symbolicLinkNameLength,
                                                       &symbolicLinkNameLength);

        PNP_ASSERT(status != STATUS_BUFFER_TOO_SMALL);
    }

    if (!NT_SUCCESS(status)) {
        if (status == STATUS_OBJECT_NAME_INVALID) {

            //
            // This is to maintain compatibility with what this function used
            // to return if the string was not formatted properly.
            //

            status = STATUS_INVALID_PARAMETER;
        }

        goto clean0;
    }

    //
    // Compute the size of the global symbolic link string.
    //

    status = RtlULongToUShort(((symbolicLinkNameLength * sizeof(WCHAR)) +
                                IopConstStringSize(GLOBAL_SYMLINK_STRING_PREFIX)),
                              &globalStringSize);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    //
    // Allocate space for the strings.
    //

    status = IopAllocateUnicodeString(GlobalString, globalStringSize);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    //
    // Copy the \GLOBAL?? symbolic link name prefix to the string.
    //

    status = RtlAppendUnicodeToString(GlobalString, GLOBAL_SYMLINK_STRING_PREFIX);

    PNP_ASSERT(NT_SUCCESS(status));

    if (!NT_SUCCESS(status)) {
        RtlFreeUnicodeString(GlobalString);
        goto clean0;
    }

    //
    // Append the part of the SymbolicLinkName that follows the prefix.
    //

    status = RtlAppendUnicodeToString(GlobalString, symbolicLinkName);

    PNP_ASSERT(NT_SUCCESS(status));

    if (!NT_SUCCESS(status)) {
        RtlFreeUnicodeString(GlobalString);
        goto clean0;
    }

    //
    // Make sure that the sizes match up.  GlobalString->Length does not
    // account for a null terminator while globalStringSize does.
    //

    PNP_ASSERT(GlobalString->Length == (globalStringSize - sizeof(UNICODE_NULL)));

clean0:

    PnpUnicodeStringToWstrFree(symbolicLinkString, SymbolicLink);

    if (symbolicLinkName != NULL) {
        ExFreePool(symbolicLinkName);
    }

    return status;
}


NTSTATUS
IopProcessSetInterfaceState(
    __in PUNICODE_STRING SymbolicLinkName,
    __in BOOLEAN Enable,
    __in BOOLEAN DeferNotStarted
    )
/*++

Routine Description:

    This DDI allows a device class to activate and deactivate an interface
    previously registered using IoRegisterDeviceInterface.

Parameters:

    SymbolicLinkName - Supplies a pointer to the symbolic link name which was
        returned by IoRegisterDeviceInterface when the interface was registered,
        or as returned by IoGetDeviceInterfaces.

    Enable - If TRUE (non-zero), the interface will be enabled.  If FALSE, it
        will be disabled.

    DeferNotStarted - If TRUE then enables will be queued if the PDO isn't
        started.  It is FALSE when we've started the PDO and are processing the
        queued enables.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/

{
    PDEVICE_NODE deviceNode;
    PWSTR deviceInstanceId = NULL;
    PWCHAR deviceNameBuffer = NULL;
    ULONG deviceNameBufferLength;
    UNICODE_STRING deviceNameString;
    UNICODE_STRING globalSymbolicLinkName = {0};
    GUID interfaceClassGuid;
    PPI_DM_OBJECT object = NULL;
    OBJECT_ATTRIBUTES objectAttributes;
    PDEVICE_OBJECT physicalDeviceObject = NULL;
    DEVPROPTYPE propertyType;
    ULONG propertySize;
    NTSTATUS status = STATUS_SUCCESS;
    PWCHAR symbolicLinkBuffer = NULL;
    BOOLEAN raiseNotifications = FALSE;
    BOOLEAN removeCacheReference = FALSE;
    UNICODE_STRING tempString;

    PAGED_CODE();

    //
    // Make sure we have a SymbolicLinkName.
    //

    if ((!ARGUMENT_PRESENT(SymbolicLinkName)) ||
        (SymbolicLinkName->Buffer == NULL)    ||
        (SymbolicLinkName->Length == 0)) {
        status = STATUS_INVALID_PARAMETER;
        goto clean0;
    }

    status = PnpUnicodeStringToWstr(&symbolicLinkBuffer, NULL, SymbolicLinkName);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    //
    // Check that the supplied symbolic link can be parsed to extract the device
    // class guid - note that this is also a way of verifying that the
    // SymbolicLinkName string is valid.
    //

    status = _CmGetDeviceInterfaceClassGuid(PiPnpRtlCtx,
                                            symbolicLinkBuffer,
                                            &interfaceClassGuid);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    //
    // Symbolic links created for device interfaces should be visible to all
    // users, in all sessions, so we need to contruct an absolute name for
    // symbolic link in the global DosDevices namespace '\GLOBAL??'.  This
    // ensures that a global symbolic link will always be created or deleted by
    // IoSetDeviceInterfaceState, no matter what context it is called in.
    //

    status = IopBuildGlobalSymbolicLinkString(SymbolicLinkName,
                                              &globalSymbolicLinkName);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    deviceInstanceId = ExAllocatePool(PagedPool, MAX_DEVICE_ID_LEN * sizeof(WCHAR));

    if (!deviceInstanceId) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto clean0;
    }

    status = _PnpGetObjectProperty(PiPnpRtlCtx,
                                   symbolicLinkBuffer,
                                   PNP_OBJECT_TYPE_DEVICE_INTERFACE,
                                   NULL,
                                   NULL,
                                   &DEVPKEY_Device_InstanceId,
                                   &propertyType,
                                   (PUCHAR)deviceInstanceId,
                                   MAX_DEVICE_ID_LEN * sizeof(WCHAR),
                                   &propertySize,
                                   0);

    PNP_ASSERT((status != STATUS_BUFFER_TOO_SMALL) &&
               (status != STATUS_NOT_FOUND));

    if ((status == STATUS_NOT_FOUND) ||
        (NT_SUCCESS(status) && (propertyType != DEVPROP_TYPE_STRING))) {

        //
        // This error was returned by the previous implementation.
        // Keep it for compatibility.
        //

        status = STATUS_INVALID_DEVICE_REQUEST;
        goto clean0;
    }

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    //
    // Find the PDO corresponding to this device instance name.
    //

    status = RtlInitUnicodeStringEx(&tempString, deviceInstanceId);

    if (!NT_SUCCESS(status)) {
        goto clean0;
    }

    physicalDeviceObject = PnpDeviceObjectFromDeviceInstance(&tempString);

    if (!physicalDeviceObject) {

        if (Enable) {

            status = STATUS_INVALID_DEVICE_REQUEST;

        } else {

            //
            // In the case of Disable we want to continue even if there was an error
            // finding the PDO.  Prior to adding support for deferring the
            // IoSetDeviceInterfaceState calls, we never looked up the PDO for
            // disables.  This will make sure that we continue to behave the same as
            // we used to in the case where we can't find the PDO.
            //

            PNP_ASSERT(status == STATUS_SUCCESS);
        }

        goto clean0;
    }

    deviceNode = PP_DO_TO_DN(physicalDeviceObject);

    //
    // If trying to enable an interface, verify that the device is in a valid
    // state in which to enable interfaces.
    //

    if ((Enable) && (PipCanEnableInterfaces(deviceNode) == FALSE)) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        goto clean0;
    }

    //
    // DeferNotStarted is set TRUE if we are being called from
    // IoSetDeviceInterfaceState.  It will be set FALSE if we are
    // processing previously queued operations as we are starting the
    // device.
    //

    if (DeferNotStarted &&
        (physicalDeviceObject->DeviceObjectExtension->ExtensionFlags & DOE_START_PENDING)) {

        //
        // The device hasn't been started yet.  We need to queue
        // any enables and remove items from the queue on a disable.
        //

        if (Enable) {

            status = PiDeferSetInterfaceState(deviceNode, SymbolicLinkName);

        } else {

            PiRemoveDeferredSetInterfaceState(deviceNode, SymbolicLinkName);
        }

        goto clean0;
    }

    //
    // Enable or Disable the interface.
    //

    if (Enable) {

        //
        // Retrieve the PDO's device object name.
        //

        deviceNameBufferLength = 256 * sizeof(WCHAR);

        for ( ; ; ) {

            if (deviceNameBuffer) {
                ExFreePool(deviceNameBuffer);
            }

            deviceNameBuffer = ExAllocatePool(PagedPool, deviceNameBufferLength);

            if (!deviceNameBuffer) {
                status = STATUS_INSUFFICIENT_RESOURCES;
                break;
            }

            status = IoGetDeviceProperty(physicalDeviceObject,
                                         DevicePropertyPhysicalDeviceObjectName,
                                         deviceNameBufferLength,
                                         deviceNameBuffer,
                                         &deviceNameBufferLength);

            if (status != STATUS_BUFFER_TOO_SMALL) {
                break;
            }
        }

        if (!NT_SUCCESS(status) || deviceNameBufferLength == 0) {
            goto clean0;
        }

        RtlInitUnicodeString(&deviceNameString, deviceNameBuffer);

        //
        // Add a cache reference to the interface that is being enabled.
        //

        status = PiDmAddCacheReferenceForObject(PNP_OBJECT_TYPE_DEVICE_INTERFACE,
                                                symbolicLinkBuffer,
                                                &object);

        if (!NT_SUCCESS(status)) {
            goto clean0;
        }

        PiDmObjectAcquireExclusiveLock(object);

        //
        // Check that the interface exists in the registry.
        //

        if (object->Flags.HasRegistryBacking == FALSE) {

            removeCacheReference = TRUE;
            status = STATUS_OBJECT_NAME_NOT_FOUND;
            goto clean1;
        }

        //
        // Check if the interface is already enabled.
        //

        if (object->TypeExt.Interface.SymbolicLinkHandle) {

            removeCacheReference = TRUE;

            //
            // This is an informational message, not an error.
            // Returning this status maintains compatibility
            // with the previous implementation.
            //

            status = STATUS_OBJECT_NAME_EXISTS;
            goto clean1;
        }

        //
        // Open the symbolic link and create it if
        // it doesn't already exist.
        //

        InitializeObjectAttributes(&objectAttributes,
                                   &globalSymbolicLinkName,
                                   OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                                   NULL,
                                   SePublicDefaultUnrestrictedSd );

        status = ZwOpenSymbolicLinkObject(&object->TypeExt.Interface.SymbolicLinkHandle,
                                          SYMBOLIC_LINK_ALL_ACCESS,
                                          &objectAttributes);

        if (status == STATUS_OBJECT_NAME_NOT_FOUND) {

            status = ZwCreateSymbolicLinkObject(&object->TypeExt.Interface.SymbolicLinkHandle,
                                                SYMBOLIC_LINK_ALL_ACCESS,
                                                &objectAttributes,
                                                &deviceNameString);
        }

        if (!NT_SUCCESS(status)) {
            goto clean1;
        }

        raiseNotifications = TRUE;

    } else {  // Disable

        status = PiDmGetObject(PNP_OBJECT_TYPE_DEVICE_INTERFACE,
                               symbolicLinkBuffer,
                               &object);

        if (!NT_SUCCESS(status)) {
            goto clean0;
        }

        PiDmObjectAcquireExclusiveLock(object);

        //
        // Check if the interface is enabled, if it is not fail.
        // STATUS_OBJECT_NAME_NOT_FOUND is the value returned in the
        // previous implementation.  Keep it for compatibility.
        //

        if (!object->TypeExt.Interface.SymbolicLinkHandle) {

            status = STATUS_OBJECT_NAME_NOT_FOUND;
            goto clean1;
        }

        //
        // Disable the interface by closing the handle to the
        // symbolic link and removing the cache reference.  Once the
        // last handle to the symbolic link is released, the object
        // manager will destory the symbolic link.
        //

        ZwClose(object->TypeExt.Interface.SymbolicLinkHandle);
        object->TypeExt.Interface.SymbolicLinkHandle = NULL;

        removeCacheReference = TRUE;

        raiseNotifications = TRUE;
    }

clean1:

    PiDmObjectReleaseLock(object);

clean0:

    //
    // Raise device interface notifications.
    //

    if (raiseNotifications) {

        if (Enable) {

            PnpSetDeviceClassChange(&GUID_DEVICE_INTERFACE_ARRIVAL,
                                    &interfaceClassGuid,
                                    SymbolicLinkName);
        } else {

            PnpSetDeviceClassChange((LPGUID)&GUID_DEVICE_INTERFACE_REMOVAL,
                                    &interfaceClassGuid,
                                    SymbolicLinkName);
        }

        _PnpRaiseNtPlugPlayInterfacePropertyChangeEvent(PiPnpRtlCtx,
                                                        symbolicLinkBuffer,
                                                        NT_PLUGPLAY_MAPPED_INTERFACE_PROPERTY_ENABLED);
    }

    if (removeCacheReference) {
        PiDmRemoveCacheReferenceForObject(PNP_OBJECT_TYPE_DEVICE_INTERFACE,
                                          symbolicLinkBuffer,
                                          NULL);
    }

    RtlFreeUnicodeString(&globalSymbolicLinkName);

    PnpUnicodeStringToWstrFree(symbolicLinkBuffer, SymbolicLinkName);

    if (physicalDeviceObject) {
        ObDereferenceObject(physicalDeviceObject);
    }

    if (deviceNameBuffer) {
        ExFreePool(deviceNameBuffer);
    }

    if (deviceInstanceId) {
        ExFreePool(deviceInstanceId);
    }

    if (object) {
        PiDmObjectRelease(object);
    }

    return status;
}

NTSTATUS
PiDeferSetInterfaceState(
    __in PDEVICE_NODE DeviceNode,
    __in PUNICODE_STRING SymbolicLinkName
    )
{
    NTSTATUS status;
    PPENDING_SET_INTERFACE_STATE pendingSetState = NULL;

    PAGED_CODE();

    PNP_ASSERT(PiIsPnpRegistryLocked(TRUE));

    pendingSetState = ExAllocatePool(PagedPool,
                                     sizeof(PENDING_SET_INTERFACE_STATE));

    if (!pendingSetState) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto clean0;
    }

    status = IopAllocateUnicodeString(&pendingSetState->LinkName,
                                      SymbolicLinkName->Length);

    if (!NT_SUCCESS(status)) {
        status = STATUS_INSUFFICIENT_RESOURCES;
        goto clean0;
    }

    RtlCopyUnicodeString(&pendingSetState->LinkName, SymbolicLinkName);

    InsertTailList(&DeviceNode->PendedSetInterfaceState,
                   &pendingSetState->List);

clean0:

    if (!NT_SUCCESS(status) && pendingSetState) {
        ExFreePool(pendingSetState);
    }

    return status;
}

NTSTATUS
PiRemoveDeferredSetInterfaceState(
    __in PDEVICE_NODE DeviceNode,
    __in PUNICODE_STRING SymbolicLinkName
    )
{
    NTSTATUS status = STATUS_UNSUCCESSFUL;
    PPENDING_SET_INTERFACE_STATE pendingSetState;
    PLIST_ENTRY entry;

    PAGED_CODE();

    PNP_ASSERT(PiIsPnpRegistryLocked(TRUE));

    //
    // Find the deferred entry and remove it.
    //

    for (   entry = DeviceNode->PendedSetInterfaceState.Flink;
            entry != &DeviceNode->PendedSetInterfaceState;
            entry = entry->Flink)  {

        pendingSetState = CONTAINING_RECORD(entry,
                                            PENDING_SET_INTERFACE_STATE,
                                            List);

        if (RtlEqualUnicodeString(&pendingSetState->LinkName,
                                  SymbolicLinkName,
                                  TRUE)) {

            //
            // Remove the entry and free associated storage.
            //

            RemoveEntryList(&pendingSetState->List);

            ExFreePool(pendingSetState->LinkName.Buffer);
            ExFreePool(pendingSetState);

            status = STATUS_SUCCESS;
            break;
        }
    }

    return status;
}

NTSTATUS
IopDoDeferredSetInterfaceState(
    __in PDEVICE_NODE DeviceNode
    )
/*++

Routine Description:

    Process the queued IoSetDeviceInterfaceState calls.

Parameters:

    DeviceNode - Device node which has just been started.

Return Value:

    Status code that indicates whether or not the function was successful.

--*/
{
    PPENDING_SET_INTERFACE_STATE entry;

    PAGED_CODE();

    PiLockPnpRegistry(TRUE);

    PpMarkDeviceStackExtensionFlag(DeviceNode->PhysicalDeviceObject, DOE_START_PENDING, FALSE);

    while (!IsListEmpty(&DeviceNode->PendedSetInterfaceState)) {

        entry = (PPENDING_SET_INTERFACE_STATE)RemoveHeadList(
                    &DeviceNode->PendedSetInterfaceState);

        IopProcessSetInterfaceState(&entry->LinkName, TRUE, FALSE);

        ExFreePool(entry->LinkName.Buffer);
        ExFreePool(entry);
    }

    PiUnlockPnpRegistry();

    return STATUS_SUCCESS;
}

BOOLEAN
PipCanEnableInterfaces (
    __in PDEVICE_NODE DeviceNode
    )

/*++

Routine Description:

    This routine determines if a device node is started or being enumerated and
    thus an attempt to enable interfaces is valid.

Arguments:

    DeviceNode - The device node whose state is being checked.

Return Value:

    TRUE if the device can attempt to enable interfaces; FALSE otherwise.

--*/

{

    BOOLEAN CanEnable;

    PAGED_CODE();

    PNP_ASSERT(DeviceNode != NULL);

    switch (DeviceNode->State) {
    case DeviceNodeUninitialized:
        CanEnable = FALSE;
        break;

    case DeviceNodeInitialized:
    case DeviceNodeDriversAdded:
    case DeviceNodeResourcesAssigned:
    case DeviceNodeStartPending:
    case DeviceNodeStartCompletion:
    case DeviceNodeStartPostWork:
    case DeviceNodeStarted:
    case DeviceNodeQueryStopped:
    case DeviceNodeStopped:
    case DeviceNodeRestartCompletion:
    case DeviceNodeEnumeratePending:
    case DeviceNodeEnumerateCompletion:

    //
    // Allow drivers to enable the interface in the context of a cancel remove
    // IRP.
    //

    case DeviceNodeQueryRemoved:
        CanEnable = TRUE;
        break;

    case DeviceNodeAwaitingQueuedDeletion:
    case DeviceNodeAwaitingQueuedRemoval:
    case DeviceNodeRemovePendingCloses:
    case DeviceNodeRemoved:
    case DeviceNodeDeletePendingCloses:
    case DeviceNodeDeleted:
        CanEnable = FALSE;
        break;

    default:

        PNP_ASSERT(FALSE);

        CanEnable = FALSE;
        break;
    }

    return CanEnable;
}
