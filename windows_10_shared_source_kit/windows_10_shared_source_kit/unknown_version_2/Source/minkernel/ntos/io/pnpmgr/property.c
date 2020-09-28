/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    property.c

Abstract:

    This module contains custom property routines.

Author:

    Santosh Jodh (santoshj) February 04, 2005

--*/

#include "pnpmgrp.h"
#include <halpnpp.h>
#include <telemetry\MicrosoftTelemetry.h>

#pragma hdrstop

#ifdef ALLOC_PRAGMA

#pragma alloc_text(PAGE, PnpSetDevicePropertyData)
#pragma alloc_text(PAGE, PnpGetDevicePropertyData)
#pragma alloc_text(PAGE, PnpSetDeviceInterfacePropertyData)
#pragma alloc_text(PAGE, PnpGetDeviceInterfacePropertyData)
#pragma alloc_text(PAGE, PnpSetInterruptInformation)
#pragma alloc_text(PAGE, PnpGetInterruptInformation)
#pragma alloc_text(PAGE, PnpCompareInterruptInformation)
#pragma alloc_text(PAGE, PnpFreeInterruptInformation)

#endif // ALLOC_PRAGMA

ERESOURCE PnpDevicePropertyLock;

NTSTATUS
PnpSetDevicePropertyData (
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

    This routine will set the specified device property data.

Parameters

    Pdo - Pointer to the physical device object in the stack.

    PropertyKey - Pointer to the property key identifier.

    Lcid - Optional language specifier. If not specified, the LOCALE_NEUTRAL
        language is assumed.

    Flags -
        PLUGPLAY_PROPERTY_PERSISTENT - Property should be persisted across boots.

    Type - Specifies property data type.

    Size - Size of the property data in bytes.

    Data - Pointer to the property data.

Return Value

    NTSTATUS.

--*/
{
    NTSTATUS status;
    PDEVICE_NODE deviceNode;
    PUNICODE_STRING pDeviceName = NULL;
    WCHAR Buffer[LOCALE_NAME_MAX_LENGTH] = { 0 };
    UNICODE_STRING localeName = { 0 };

    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    status = STATUS_SUCCESS;
    deviceNode = PP_DO_TO_DN(Pdo);

    if ((deviceNode == NULL) || PnpIsDeviceInstancePathEmpty(deviceNode)) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        goto clean0;
    }

    pDeviceName = PnpGetDeviceInstancePath(deviceNode);

    if (Lcid != 0) {
        localeName.Buffer = Buffer;
        localeName.Length = 0;
        localeName.MaximumLength = sizeof(Buffer);
        if (!RtlLCIDToCultureName(Lcid, &localeName)) {
            status = STATUS_UNSUCCESSFUL;
            goto clean0;
        }
    } else {
        localeName.Buffer = NULL;
    }

    PnpLockDeviceProperty(TRUE);

    status = PiPnpRtlSetObjectProperty(PiPnpRtlCtx,
                                       (PCWSTR)pDeviceName->Buffer,
                                       PNP_OBJECT_TYPE_DEVICE,
                                       NULL,
                                       localeName.Buffer,
                                       PropertyKey,
                                       Type,
                                       Data,
                                       Size,
                                       0);

    if (NT_SUCCESS(status) &&
        IsEqualDevPropKey(*PropertyKey, INTERRUPT_CONNECTION_DATA_PKEY)) {

        //
        // The interrupt connection information is being moved from a device 
        // property to the device object extension.  To reduce compat risks, 
        // the information is stored in both locations until such time 
        // as telemetry proves there is no legacy regression.
        //

        status = PnpSetInterruptInformation(Pdo, Data, Size);
    }

    PnpUnlockDeviceProperty();

    //
    // Maintain compatibility with previous implementations.
    //
    if (status == STATUS_NOT_FOUND) {
        status = STATUS_OBJECT_NAME_NOT_FOUND;
    }

    //
    // Send a property change event to user-mode if the devnode is not in the
    // uninitialized or initialized state. This corresponds to a state where
    // GUID_DEVICE_ENUMERATED has already been queued for this device.
    //
    if (deviceNode->State >= DeviceNodeDriversAdded) {
        PnpSetDeviceInstancePropertyChangeEvent(deviceNode);
    }

  clean0:
    return status;
}


NTSTATUS
PnpGetDevicePropertyData (
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

    This routine will get the specified device property data.

Parameters

    Pdo - Pointer to the physical device object in the stack.

    PropertyKey - Pointer to the property key identifier.

    Lcid - Optional language specifier. If not specified, the LOCALE_NEUTRAL
        language is assumed.

    Flags - Reserved.

    Size - Size of the callers buffer in bytes.

    Data - Pointer that recieves the property data.

    RequiredSize - Pointer that recieves the required buffer size to get all of the
        property data.

    Type - Pointer that recieves the property data type.

Return Value

    NTSTATUS.

--*/
{
    NTSTATUS status;
    PDEVICE_NODE deviceNode;
    PUNICODE_STRING pDeviceName = NULL;
    WCHAR Buffer[LOCALE_NAME_MAX_LENGTH] = { 0 };
    UNICODE_STRING localeName = { 0 };

    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    deviceNode = PP_DO_TO_DN(Pdo);

    if ((deviceNode == NULL) || PnpIsDeviceInstancePathEmpty(deviceNode)) {
        status = STATUS_INVALID_DEVICE_REQUEST;
        goto clean0;
    }

    pDeviceName = PnpGetDeviceInstancePath(deviceNode);

    if (Lcid != 0) {
        localeName.Buffer = Buffer;
        localeName.Length = 0;
        localeName.MaximumLength = sizeof(Buffer);
        if (!RtlLCIDToCultureName(Lcid, &localeName)) {
            status = STATUS_UNSUCCESSFUL;
            goto clean0;
        }
    } else {
        localeName.Buffer = NULL;
    }

    PnpLockDeviceProperty(FALSE);

    status = _PnpGetObjectProperty(PiPnpRtlCtx,
                                   (PCWSTR)pDeviceName->Buffer,
                                   PNP_OBJECT_TYPE_DEVICE,
                                   NULL,
                                   localeName.Buffer,
                                   PropertyKey,
                                   Type,
                                   Data,
                                   Size,
                                   RequiredSize,
                                   0);

    if (IsEqualDevPropKey(*PropertyKey, INTERRUPT_CONNECTION_DATA_PKEY)) {
        switch (status) {
        case STATUS_NOT_FOUND:
        case STATUS_OBJECT_NAME_NOT_FOUND:

            //
            // The information was not found in the registry.  Check if 
            // it can be found in the device object extension.  This is the
            // sysprep scenario described in threshold bug #1307146.
            //

            status = PnpGetInterruptInformation(Pdo, Data, Size, RequiredSize);
            break;

        case STATUS_SUCCESS:

            //
            // The information was found in the registry.  Compare it against
            // the data in the device object extension.
            //

            PnpCompareInterruptInformation(Pdo, Data, *RequiredSize);
            break;
        }
    }

    PnpUnlockDeviceProperty();

    //
    // Maintain compatibility with previous implementations.
    //
    if (status == STATUS_NOT_FOUND) {
        status = STATUS_OBJECT_NAME_NOT_FOUND;
    }

  clean0:
    return status;
}


NTSTATUS
PnpSetDeviceInterfacePropertyData (
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

    This routine will set the specified device interface property data.

Parameters

    SymbolicLinkName - Supplies a pointer to the symbolic link name which was
        returned by IoRegisterDeviceInterface when the interface was registered,
        or as returned by IoGetDeviceInterfaces.

    PropertyKey - Pointer to the property key identifier.

    Lcid - Optional language specifier. If not specified, the LOCALE_NEUTRAL
        language is assumed.

    Flags -
        PLUGPLAY_PROPERTY_PERSISTENT - Property should be persisted across boots.

    Type - Specifies property data type.

    Size - Size of the property data in bytes.

    Data - Pointer to the property data.

Return Value

    NTSTATUS.

--*/
{
    NTSTATUS status;
    WCHAR Buffer[LOCALE_NAME_MAX_LENGTH] = { 0 };
    UNICODE_STRING localeName = { 0 };
    PWCHAR SymbolicLinkNameBuffer = NULL;

    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    if ((!ARGUMENT_PRESENT(SymbolicLinkName)) ||
        (SymbolicLinkName->Buffer == NULL)    ||
        (SymbolicLinkName->Length == 0)) {
        status = STATUS_INVALID_PARAMETER;
        goto clean0;
    }

    if (Lcid != 0) {
        localeName.Buffer = Buffer;
        localeName.Length = 0;
        localeName.MaximumLength = sizeof(Buffer);
        if (!RtlLCIDToCultureName(Lcid, &localeName)) {
            status = STATUS_UNSUCCESSFUL;
            goto clean0;
        }
    } else {
        localeName.Buffer = NULL;
    }

    PnpLockDeviceProperty(TRUE);

    status = PnpUnicodeStringToWstr(&SymbolicLinkNameBuffer, NULL, SymbolicLinkName);

    if (NT_SUCCESS(status)) {

        status = PiPnpRtlSetObjectProperty(PiPnpRtlCtx,
                                           SymbolicLinkNameBuffer,
                                           PNP_OBJECT_TYPE_DEVICE_INTERFACE,
                                           NULL,
                                           localeName.Buffer,
                                           PropertyKey,
                                           Type,
                                           Data,
                                           Size,
                                           0);
    }


    PnpUnicodeStringToWstrFree(SymbolicLinkNameBuffer, SymbolicLinkName);

    PnpUnlockDeviceProperty();

    //
    // Keep the newer device interface property APIs consistent with the device
    // property APIs
    //
    if (status == STATUS_NOT_FOUND) {
        status = STATUS_OBJECT_NAME_NOT_FOUND;
    }

  clean0:
    return status;
}


NTSTATUS
PnpGetDeviceInterfacePropertyData (
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

    This routine will get the specified device interface property data.

Parameters

    SymbolicLinkName - Supplies a pointer to the symbolic link name which was
        returned by IoRegisterDeviceInterface when the interface was registered,
        or as returned by IoGetDeviceInterfaces.

    PropertyKey - Pointer to the property key identifier.

    Lcid - Optional language specifier. If not specified, the LOCALE_NEUTRAL
        language is assumed.

    Flags - Reserved.

    Size - Size of the callers buffer in bytes.

    Data - Pointer that recieves the property data.

    RequiredSize - Pointer that recieves the required buffer size to get all of the
        property data.

    Type - Pointer that recieves the property data type.

Return Value

    NTSTATUS.

--*/
{

    NTSTATUS status;
    WCHAR Buffer[LOCALE_NAME_MAX_LENGTH] = { 0 };
    UNICODE_STRING localeName = { 0 };
    PWCHAR SymbolicLinkNameBuffer = NULL;

    UNREFERENCED_PARAMETER(Flags);

    PAGED_CODE();

    if ((!ARGUMENT_PRESENT(SymbolicLinkName)) ||
        (SymbolicLinkName->Buffer == NULL)    ||
        (SymbolicLinkName->Length == 0)) {
        status = STATUS_INVALID_PARAMETER;
        goto clean0;
    }

    if (Lcid != 0) {
        localeName.Buffer = Buffer;
        localeName.Length = 0;
        localeName.MaximumLength = sizeof(Buffer);
        if (!RtlLCIDToCultureName(Lcid, &localeName)) {
            status = STATUS_UNSUCCESSFUL;
            goto clean0;
        }
    } else {
        localeName.Buffer = NULL;
    }

    PnpLockDeviceProperty(FALSE);

    status = PnpUnicodeStringToWstr(&SymbolicLinkNameBuffer, NULL, SymbolicLinkName);

    if (NT_SUCCESS(status)) {

        status = _PnpGetObjectProperty(PiPnpRtlCtx,
                                       SymbolicLinkNameBuffer,
                                       PNP_OBJECT_TYPE_DEVICE_INTERFACE,
                                       NULL,
                                       localeName.Buffer,
                                       PropertyKey,
                                       Type,
                                       Data,
                                       Size,
                                       RequiredSize,
                                       0);
    }

    PnpUnicodeStringToWstrFree(SymbolicLinkNameBuffer, SymbolicLinkName);

    PnpUnlockDeviceProperty();

    //
    // Keep the newer device interface property APIs consistent with the device
    // property APIs
    //
    if (status == STATUS_NOT_FOUND) {
        status = STATUS_OBJECT_NAME_NOT_FOUND;
    }

  clean0:
    return status;
}

NTSTATUS
PnpSetInterruptInformation (
    __in PDEVICE_OBJECT Pdo,
    __in PVOID Buffer,
    __in ULONG Size
    )

/*++

Description

    This routine will set the interrupt information in the device object 
    extension.

Parameters

    Pdo - Pointer to the physical device object in the stack.

    Buffer - Pointer to the interrupt data.

    Size - Size of the property data in bytes.

Return Value

    NTSTATUS.

--*/

{

    PPNP_INTERRUPT_CONTEXT newInfo;
    PPNP_INTERRUPT_CONTEXT prevInfo;
    ULONG totalSize;

    PAGED_CODE();

    totalSize = RTL_SIZEOF_THROUGH_FIELD(PNP_INTERRUPT_CONTEXT, Size) + Size;
    newInfo = (PPNP_INTERRUPT_CONTEXT) ExAllocatePoolWithTag(PagedPool,
                                                             totalSize,
                                                             PNP_POOLTAG_GENERIC);

    if (newInfo == NULL) {
        return STATUS_INSUFFICIENT_RESOURCES;
    }

    newInfo->Size = Size;
    RtlCopyMemory(&newInfo->Data[0],
                  Buffer,
                  Size);

    //
    // Update the interrupt connection information 
    // in the device object extension.  This swap is 
    // protected by the DeviceProperty lock
    //

    prevInfo = Pdo->DeviceObjectExtension->InterruptContext;
    Pdo->DeviceObjectExtension->InterruptContext = newInfo;

    //
    // Free the previous interrupt connection information if it existed.
    //

    if (prevInfo != NULL) {
        ExFreePoolWithTag(prevInfo, PNP_POOLTAG_GENERIC);
    }

    return STATUS_SUCCESS;
}

NTSTATUS
PnpGetInterruptInformation (
    __in PDEVICE_OBJECT Pdo,
    __in PVOID Buffer,
    __in ULONG Size,
    __out PULONG RequiredSize
    )

/*++

Description

    This routine will get the interrupt information in the device object extension.

Parameters

    Pdo - Pointer to the physical device object in the stack.

    Buffer - Pointer to the interrupt data.

    Size - Size of the property data in bytes.
    
    RequiredSize - The required size of the property data in bytes.

Return Value

    NTSTATUS.

--*/

{

    PPNP_INTERRUPT_CONTEXT info;

    PAGED_CODE();

    info = Pdo->DeviceObjectExtension->InterruptContext;
    if (info == NULL) {
        return STATUS_OBJECT_NAME_NOT_FOUND;
    }

    if (RequiredSize != NULL) {
        *RequiredSize = info->Size;
    }

    if (Size < info->Size) {
        return STATUS_BUFFER_TOO_SMALL;
    }

    RtlCopyMemory(Buffer,
                  &info->Data[0],
                  info->Size);

    return STATUS_SUCCESS;
}

VOID
PnpCompareInterruptInformation (
    __in PDEVICE_OBJECT Pdo,
    __in PVOID Buffer,
    __in ULONG Size
    )

/*++

Description

    This routine will compare the interrupt information contained in the device object extension 
    and registry.

Parameters

    Pdo - Pointer to the physical device object in the stack.

    Buffer - Pointer to the interrupt data in registry

    Size - Size of the property data in bytes.

Return Value

    VOID

--*/

{

    PDEVICE_NODE deviceNode;
    PPNP_INTERRUPT_CONTEXT info;
    SIZE_T length;

    PAGED_CODE();

    info = Pdo->DeviceObjectExtension->InterruptContext;
    deviceNode = PP_DO_TO_DN(Pdo);
    if (info == NULL) {

        //
        // The interrupt connection information was found
        // in the registry, but not in the device object 
        // extension.
        //

        TraceLoggingWrite(
            PnpTraceLoggingHandle,
            "PnpCompareInterruptInformation",
            TraceLoggingString("Not Found", "Reason"),
            TraceLoggingUnicodeString(PnpGetDeviceInstancePath(deviceNode),
                                      "DeviceInstanceId"),
            TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES));

        return;
    } 

    if (info->Size != Size) {

        //
        // The interrupt connection information was found
        // in both places, but the sizes did not match.
        //

        TraceLoggingWrite(
            PnpTraceLoggingHandle,
            "PnpCompareInterruptInformation",
            TraceLoggingString("Size Mismatch", "Reason"),
            TraceLoggingUInt32(info->Size, "Size1"),
            TraceLoggingUInt32(Size, "Size2"),
            TraceLoggingUnicodeString(PnpGetDeviceInstancePath(deviceNode),
                                      "DeviceInstanceId"),
            TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES));

        return;
    }

    length = RtlCompareMemory(&info->Data[0],
                              Buffer,
                              Size);

    if (length != Size) {

        //
        // The interrupt connection information did not 
        // match between the registry and device object
        // extension.
        //

        TraceLoggingWrite(
            PnpTraceLoggingHandle,
            "PnpCompareInterruptInformation",
            TraceLoggingString("Compare Failed", "Reason"),
            TraceLoggingUInt32((UINT)length, "Size1"),
            TraceLoggingUInt32(Size, "Size2"),
            TraceLoggingUnicodeString(PnpGetDeviceInstancePath(deviceNode),
                                      "DeviceInstanceId"),
            TraceLoggingKeyword(MICROSOFT_KEYWORD_MEASURES));

        return;
    }
}

VOID
PnpFreeInterruptInformation (
    __in PDEVICE_OBJECT Pdo
    )

/*++

Description

    This routine will free the interrupt information contained in the device object extension.

Parameters

    Pdo - Pointer to the physical device object in the stack.

Return Value

    VOID

--*/

{

    PAGED_CODE();

    if (Pdo->DeviceObjectExtension->InterruptContext != NULL) {
        ExFreePoolWithTag(Pdo->DeviceObjectExtension->InterruptContext, 
                          PNP_POOLTAG_GENERIC);

        Pdo->DeviceObjectExtension->InterruptContext = NULL;
    }
}