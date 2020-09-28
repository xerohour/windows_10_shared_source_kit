/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    vmproxy.c

Abstract:

    This module contains functions for enumerating PCI devices as virtual
    machine proxies.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "vmproxy.tmh"

// ------------------------------------------------------------------ Prototypes

NTSTATUS
VmProxyWriteDevicePathsToRegistry(
    _In_ const PCONTROL_DEVICE_EXTENSION Extension
    );

NTSTATUS
VmProxyAddPath(
    _In_ PCONTROL_DEVICE_EXTENSION Extension,
    _In_z_ const WCHAR *DevicePath
    );

NTSTATUS
VmProxyRemovePath(
    _In_ PCONTROL_DEVICE_EXTENSION Extension,
    _In_z_ const WCHAR *DevicePath
    );

NTSTATUS
VmProxyIsSubString(
    _In_ PCZZWSTR MultiSzStr,
    _In_ PCWSTR SubStrIn,
    _Out_ PCWSTR* SubStrLocation
    );

NTSTATUS
VmProxyGetDeviceState(
    _In_z_ const WCHAR *DeviceLocationPath,
    _Out_ PCI_OBJECT_STATE *CurrentDeviceState
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, VmProxyIsProxyDevice)
#pragma alloc_text(PAGE, VmProxyIsSubString)
#pragma alloc_text(PAGE, VmProxyReadDevicePathsFromRegistry)
#pragma alloc_text(PAGE, VmProxyWriteDevicePathsToRegistry)
#pragma alloc_text(PAGE, VmProxyGetDeviceLocationString)
#pragma alloc_text(PAGE, VmProxyAddRemovePath)
#pragma alloc_text(PAGE, VmProxyAddPath)
#pragma alloc_text(PAGE, VmProxyGetDeviceState)
#pragma alloc_text(PAGE, VmProxyRemovePath)
#pragma alloc_text(PAGE, VmProxyClearPath)
#endif

// ------------------------------------------------------------------- Functions

NTSTATUS
VmProxyIsProxyDevice(
    _In_ const PPCI_DEVICE Device,
    _In_opt_ LPCWSTR ProxyDevicePaths,
    _Out_ BOOLEAN *ProxyDevice
    )

/*++

Routine Description:

    This routine detects if the PCI device is VM proxy device by location path
    matching.

Arguments:

    Device - Supplies the device context of PDO.

    ProxyDevicePaths - Supplies the Multi_sz location paths for VM proxy
        devices.  For example: L"PCIROOT(0)#PCI(0000)\0PCIROOT(1)#PCI(1B00)\0".
        If this is NULL, there are no proxy devices in the system.

    ProxyDevice - Supplies a pointer to a variable that receives TRUE if the
        device is a VM proxy device.

Return Value:

    NT status.

--*/

{

    PWSTR LocationPath;
    NTSTATUS Status;
    PCWSTR SubStrLoc;

    PAGED_CODE();

    LocationPath = NULL;
    SubStrLoc = NULL;
    Status = STATUS_SUCCESS;

    PCI_ASSERT(Device != NULL);
    PCI_ASSERT(ProxyDevice != NULL);

    *ProxyDevice = FALSE;

    //
    // If no proxy device paths are supplied, there is no proxy device.
    //

    if (ProxyDevicePaths == NULL) {
        goto cleanup;
    }

    //
    // only a leaf device(PCI_DEVICE_TYPE) can be assigned as a proxy
    //

    if (Device->HeaderType != PCI_DEVICE_TYPE) {
        goto cleanup;
    }

    Status = VmProxyGetDeviceLocationString(Device, &LocationPath);
    if (!NT_SUCCESS(Status)) {
        goto cleanup;
    }

    Status = VmProxyIsSubString(ProxyDevicePaths, LocationPath, &SubStrLoc);
    if (!NT_SUCCESS(Status)) {
        goto cleanup;
    }

    *ProxyDevice = (SubStrLoc != NULL);

cleanup:

    if (LocationPath != NULL) {
        PciFreePool(LocationPath);
        LocationPath = NULL;
    }

    return Status;
}


NTSTATUS
VmProxyCheckTearDown(
    _In_ PPCI_DEVICE Device,
    _In_opt_ LPCWSTR ProxyDevicePaths,
    _Out_ BOOLEAN *TearDownOldCreateNew
    )

/*++

Routine Description:

    This routine detects if the devnode needs to be torn down and create a new
    devnode.

Arguments:

    Device - Supplies the device context of PDO.

    ProxyDevicePaths - Supplies the Multi_sz location paths for VM proxy
        devices.  For example: L"PCIROOT(0)#PCI(0000)\0PCIROOT(1)#PCI(1B00)\0".
        If this is NULL, there are no proxy devices in the system.

    TearDownOldCreateNew - Supplies a pointer to a variable that receives TRUE
        if the devnode needs teardown and build_new.

Return Value:

    NT status.

--*/

{

    BOOLEAN ProxyDevice;
    NTSTATUS Status;

    ProxyDevice = FALSE;
    Status = STATUS_SUCCESS;

    PCI_ASSERT(Device != NULL);
    PCI_ASSERT(TearDownOldCreateNew != NULL);

    *TearDownOldCreateNew = FALSE;

    //
    // only a leaf device(PCI_DEVICE_TYPE) can be assigned as a proxy
    // don't touch non PCI_DEVICE_TYPE device type.
    //

    if (Device->HeaderType != PCI_DEVICE_TYPE) {
        goto cleanup;
    }

    Status = VmProxyIsProxyDevice(Device, ProxyDevicePaths, &ProxyDevice);
    if (!NT_SUCCESS(Status)) {
        goto cleanup;
    }

    //
    // Check if teardown is allowed in the current device state. If not,
    // just return with success.
    //
    // Only an inactive/shutdown device can be torn down. It is a device state
    // after a successful IRP_MN_(QUERY)_REMOVE_DEVICE.
    //











    if (Device->DeviceState != PciNotStarted &&
        ProxyDevice) {
        goto cleanup;
    }

    //
    // Teardown the devnode and build new devnode if the device is claimed
    // as a proxy device and currently reported as PCI\XXXX.
    //
    // Teardown the devnode and build new devnode if the device is not
    // claimed as a proxy device and currently reported as PCIP\XXXX.
    //

    if (((ProxyDevice != FALSE) && (Device->PciOwned != FALSE)) ||
        ((ProxyDevice == FALSE) && (Device->PciOwned == FALSE))) {

        *TearDownOldCreateNew = TRUE;
    }

cleanup:

    return Status;
}

NTSTATUS
VmProxyGetDeviceState(
    _In_z_ const WCHAR *DeviceLocationPath,
    _Out_ PCI_OBJECT_STATE *CurrentDeviceState
)

/*++

Routine Description:

    This routine uses the supplied device location path to traverse
    the PCI tree and search for a device matching that location string.
    When a device is found matching that location path, the device's
    state is returned to CurrentDeviceState, with STATUS_SUCCESS returned.
    If no device matching the location path is found, STATUS_NOT_FOUND
    is returned.

    This method will hold the PciGlobalLock and ChildDeviceLock on buses
    when it is traversing the PCI tree.

Arguments:

    DeviceLocationPath - A null-terminated string containing the 
        Location Path of the device's state to be found.

    CurrentDeviceState - Supplies a pointer to a variable that receives
        the DeviceState of the PDO found.

Return Value:

    NT status.

--*/

{

    NTSTATUS Status;
    PPCI_BUS Bus;
    PPCI_DEVICE CurrentDevice;
    PPCI_SEGMENT Segment;
    WCHAR* CurrentLocationPath;
    BOOLEAN DeviceFound;

    PAGED_CODE();

    Status = STATUS_NOT_FOUND;

    PCI_ASSERT(DeviceLocationPath != NULL);
    PCI_ASSERT(CurrentDeviceState != NULL);

    *CurrentDeviceState = PciNotStarted;
    CurrentLocationPath = NULL;
    DeviceFound = FALSE;

    PciAcquirePassiveLock(&PciGlobalLock);

    for (Segment = PciSegmentList; Segment != NULL; Segment->Next) {
        Bus = Segment->PciRootBusList;

        for (Bus = Segment->PciRootBusList; Bus != NULL; Bus = PciNextBusInSegment(Bus)) {
            PciAcquirePassiveLockInCriticalSection(&Bus->ChildDeviceLock);

            for (CurrentDevice = Bus->ChildDevices;
                CurrentDevice != NULL;
                CurrentDevice = CurrentDevice->Sibling) {

                //
                // Only a leaf device (PCI_DEVICE_TYPE) can be assigned as a proxy,
                // Ignore other device types.
                //

                if (CurrentDevice->HeaderType != PCI_DEVICE_TYPE) {
                    continue;
                }

                Status = VmProxyGetDeviceLocationString(CurrentDevice, &CurrentLocationPath);

                // 
                // Location String construction failed, exit out.
                //

                if (!NT_SUCCESS(Status)) {
                    break;
                }

                // 
                // Check if the current LocationPath matches the one provided, and if
                // yes, read it's state and set flag to exit loops.
                //

                if (wcscmp(DeviceLocationPath, CurrentLocationPath) == 0) {
                    *CurrentDeviceState = CurrentDevice->DeviceState;
                    DeviceFound = TRUE;
                    Status = STATUS_SUCCESS;
                }

                PciFreePool(CurrentLocationPath);
                CurrentLocationPath = NULL;

                if (DeviceFound != FALSE) {
                    break;
                }
            }

            PciReleasePassiveLockInCriticalSection(&Bus->ChildDeviceLock);

            if ((DeviceFound != FALSE) ||
                !NT_SUCCESS(Status)) {
                break;
            }
        }

        if ((DeviceFound != FALSE) ||
            !NT_SUCCESS(Status)) {
            break;
        }
    }

    PciReleasePassiveLock(&PciGlobalLock);

    //
    // If we didn't find the device, set status.
    //

    if (DeviceFound == FALSE &&
        NT_SUCCESS(Status)) {
        Status = STATUS_NOT_FOUND;
    }

    if (CurrentLocationPath != NULL) {
        PciFreePool(CurrentLocationPath);
    }

    return Status;
}


NTSTATUS
VmProxyReadDevicePathsFromRegistry(
    _In_ PCONTROL_DEVICE_EXTENSION Extension
    )

/*++

Routine Description:

    This routine reads multi-sz proxy device location path from the registry.

Parameters:

    Extension - Provides the control device pointer to receive a multi-sz device
        location paths and buffer length

Return Value:

    NTSTATUS

--*/

{

    HANDLE Handle;
    NTSTATUS Status;

    PAGED_CODE();

    Handle = NULL;
    Status = STATUS_SUCCESS;

    PCI_ASSERT(Extension != NULL);
    PCI_ASSERT(Extension->MultiSzProxyDevicePath == NULL);
    PCI_ASSERT(Extension->ByteSize == 0);

    Extension->MultiSzProxyDevicePath = NULL;
    Extension->ByteSize = 0;

    //
    // Open HKLM\System\CurrentControlSet\Control\PnP\Pci key
    //

    if (!PciOpenKey((PWSTR)s_VmProxyRegPath,
                    NULL,
                    KEY_READ,
                    &Handle,
                    &Status)) {

        goto cleanup;
    }

    //
    // read multi_sz value from VmProxy
    //

    Status = PciGetRegistryValue((PWSTR)s_VmProxyValue,
                                 NULL,
                                 Handle,
                                 REG_MULTI_SZ,
                                 (PVOID*)&Extension->MultiSzProxyDevicePath,
                                 &Extension->ByteSize
                                 );
    if (!NT_SUCCESS(Status)) {
        if (Status == STATUS_OBJECT_PATH_NOT_FOUND ||
            Status == STATUS_OBJECT_NAME_NOT_FOUND ||
            Status == STATUS_NOT_FOUND) {

            Extension->MultiSzProxyDevicePath = NULL;
            Extension->ByteSize = 0;

            Status = STATUS_SUCCESS;
        }
        goto cleanup;
    }

cleanup:

    if (Handle != NULL) {
        ZwClose(Handle);
        Handle = NULL;
    }

    return Status;
}

NTSTATUS
VmProxyWriteDevicePathsToRegistry(
    _In_ const PCONTROL_DEVICE_EXTENSION Extension
    )

/*++

Routine Description:

    This routine writes multi-sz proxy device location path to the registry.
    This routine is not thread safe. The caller should hold proper lock in
    multi-threaded environment.

Parameters:

    Extension - Supplies Pci control device entension.

Return Value:

    NTSTATUS

--*/

{
    HANDLE PciKey;
    NTSTATUS Status;
    UNICODE_STRING Value;

    PAGED_CODE();

    Status = STATUS_SUCCESS;
    PciKey = NULL;

    if (Extension->MultiSzProxyDevicePath == NULL) {
        goto cleanup;
    }

    //
    // Open HKLM\System\CurrentControlSet\Control\PnP\Pci
    //

    if (!PciOpenKey((PWSTR)s_VmProxyRegPath,
                    NULL,
                    KEY_READ | KEY_WRITE,
                    &PciKey,
                    &Status)) {

        goto cleanup;
    }

    //
    // Set VmProxy value.
    //

    RtlInitUnicodeString(&Value, s_VmProxyValue);
    Status = ZwSetValueKey(PciKey,
                           &Value,
                           0,
                           REG_MULTI_SZ,
                           Extension->MultiSzProxyDevicePath,
                           Extension->ByteSize);
    if (!NT_SUCCESS(Status)) {
        goto cleanup;
    }

cleanup:

    if (PciKey != NULL) {
        ZwClose(PciKey);
        PciKey = NULL;
    }

    return Status;
}


//
// Disable prefast warning about string not being null-terminated.
// PciLocationInterface_GetLocationStrings does terminate the string
// (even doubly so).
//
#pragma prefast(push)
#pragma prefast(disable: __WARNING_INCORRECT_ANNOTATION_STRING)

NTSTATUS
VmProxyGetDeviceLocationString (
    _In_ const PPCI_DEVICE Device,
    _Outptr_result_z_ PWCHAR *DeviceLocationString
    )

/*++

Routine Description:

    This routine gets the pci device location string for a device.

Arguments:

    Device - Supplies the PCI device whose location string is to be retrieved.

    DeviceLocationString - Supplies a double pointer to receive a single-null
        terminated string of the device location paths, composed from the set of
        location string returned from parent devices.

Return Value:

    NTSTATUS code.

--*/

{

    WCHAR *Buffer;
    ULONG Cch;
    ULONG CchTotal;
    WCHAR *CopyTo;
    PPCI_COMMON_EXTENSION Extension;
    WCHAR *LocationPath;
    PCI_DEVICE PciDevice;
    NTSTATUS Status;

    PAGED_CODE();

    Buffer = NULL;
    Cch = 0;
    CchTotal = 0;
    CopyTo = NULL;
    Extension = NULL;
    LocationPath = NULL;
    RtlZeroMemory(&PciDevice, sizeof(PCI_DEVICE));
    Status = STATUS_SUCCESS;

    PCI_ASSERT(Device != NULL);
    PCI_ASSERT(DeviceLocationString != NULL);

    if (Device == NULL || DeviceLocationString == NULL) {
        Status = STATUS_INVALID_PARAMETER;
        goto cleanup;
    }

    *DeviceLocationString = NULL;

    for (Extension = (PPCI_COMMON_EXTENSION)Device; Extension != NULL;) {
        if (Buffer != NULL) {
            PciFreePool(Buffer);
            Buffer = NULL;
        }

        //
        // PciLocationInterface_GetLocationStrings accepts PCI_DEVICE type
        // and root PCI_BUS. For PCI bridge, fake up PCI_DEVICE to represent
        // a bridge.
        //

        if (PCI_BUS_EXTENSION(Extension) &&
            !PCI_ROOT_BUS((PPCI_BUS)Extension)) {

            PciDevice.Signature = PCI_DEVICE_SIGNATURE;
            PciDevice.Slot.u.AsULONG =
                ((PPCI_BUS)Extension)->PciBridge->Slot.u.AsULONG;
            Status = PciLocationInterface_GetLocationStrings(&PciDevice,
                                                             &Buffer);

        } else {
            Status = PciLocationInterface_GetLocationStrings(Extension,
                                                             &Buffer);

        }
        if (!NT_SUCCESS(Status)) {
            TraceEvents(WppRecorderLogGetDefault(),
                        TRACE_LEVEL_ERROR,
                        DBG_INIT,
                        "PciLocationInterface_GetLocationStrings failed %!STATUS!",
                        Status);
            goto cleanup;
        }

        if (PCI_DEVICE_EXTENSION(Extension)) {
            Extension = (PPCI_COMMON_EXTENSION)((PPCI_DEVICE)Extension)->Parent;

        } else {
            PCI_ASSERT(PCI_BUS_EXTENSION(Extension));
            Extension = (PPCI_COMMON_EXTENSION)((PPCI_BUS)Extension)->ParentBus;
        }

        CchTotal += (ULONG)wcslen(Buffer);

        //
        // Adding space for L'#' in location string
        // (for example: PCIROOT(0)#PCI(1B00)).
        //

        if (Extension != NULL) {
            CchTotal += 1;
        }
    }

    //
    // allocate location string
    //

    LocationPath = (WCHAR*)PciAllocatePool(PagedPool,
                                           (CchTotal + 1)*sizeof(WCHAR));
    if (LocationPath == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }

    RtlZeroMemory(LocationPath, (CchTotal + 1)*sizeof(WCHAR));

    //
    // Fill in the location string.
    // Point to terminating NULL.
    //

    CopyTo = LocationPath + CchTotal;
    for (Extension = (PPCI_COMMON_EXTENSION)Device; Extension != NULL;) {
        if (Buffer != NULL) {
            PciFreePool(Buffer);
            Buffer = NULL;
        }

        //
        // PciLocationInterface_GetLocationStrings accept PCI_DEVICE type
        // or root PCI_BUS. For PCI bridge, fake up PCI_DEVICE to represent a
        // bridge.
        //

        if (PCI_BUS_EXTENSION(Extension) &&
            !PCI_ROOT_BUS((PPCI_BUS)Extension)) {

            PciDevice.Signature = PCI_DEVICE_SIGNATURE;
            PciDevice.Slot.u.AsULONG =
                ((PPCI_BUS)Extension)->PciBridge->Slot.u.AsULONG;
            Status = PciLocationInterface_GetLocationStrings(&PciDevice,
                                                             &Buffer);

        } else {
            Status = PciLocationInterface_GetLocationStrings(Extension,
                                                             &Buffer);

        }
        if (!NT_SUCCESS(Status)) {
            TraceEvents(WppRecorderLogGetDefault(),
                        TRACE_LEVEL_ERROR,
                        DBG_INIT,
                        "PciLocationInterface_GetLocationStrings failed %!STATUS!",
                        Status);
            goto cleanup;
        }

        if (PCI_DEVICE_EXTENSION(Extension)) {
            Extension = (PPCI_COMMON_EXTENSION)((PPCI_DEVICE)Extension)->Parent;

        } else {
            PCI_ASSERT(PCI_BUS_EXTENSION(Extension));
            Extension = (PPCI_COMMON_EXTENSION)((PPCI_BUS)Extension)->ParentBus;
        }

        Cch = (ULONG)wcslen(Buffer);
        CopyTo -= Cch;
        PCI_ASSERT(CopyTo >= LocationPath);
        RtlCopyMemory(CopyTo, Buffer, Cch * sizeof(WCHAR));

        if (Extension != NULL) {
            CopyTo -= 1;
            PCI_ASSERT(CopyTo > LocationPath);
            *CopyTo = L'#';
        }
    }

    *DeviceLocationString = LocationPath;
    LocationPath = NULL;

cleanup:

    if (Buffer != NULL) {
        PciFreePool(Buffer);
        Buffer = NULL;
    }

    if (LocationPath != NULL) {
        PciFreePool(LocationPath);
        LocationPath = NULL;
    }

    return Status;
}

#pragma prefast(pop)

NTSTATUS
VmProxyAddPath(
    _In_ PCONTROL_DEVICE_EXTENSION Extension,
    _In_z_ const WCHAR *DevicePath
    )

/*++

Routine Description:

    This routine adds a proxy device location path.
    This routine is not thread safe. The caller should hold proper lock for
    multi-thread environment.

Arguments:

    Extension - Supplies control device extension.

    DevicePath - Supplies the device location string to add.

Return Value:

    NTSTATUS code.

--*/

{
    ULONG ByteSize;
    WCHAR *Dest;
    WCHAR *DevicePathLocal;
    NTSTATUS Status;
    ULONG StrLenIn;
    PCWSTR SubStrLocation;
    PCI_OBJECT_STATE DeviceState;

    PAGED_CODE();

    ByteSize = 0;
    Dest = NULL;
    DevicePathLocal = NULL;
    Status = STATUS_SUCCESS;
    StrLenIn = 0;
    SubStrLocation = NULL;

    if (Extension == NULL || DevicePath == NULL || wcslen(DevicePath) == 0) {
        Status = STATUS_INVALID_PARAMETER;
        goto cleanup;
    }

    //
    // Check that the device currently is not in the enabled state.
    // 
    Status = VmProxyGetDeviceState(DevicePath, &DeviceState);

    if (!NT_SUCCESS(Status)) {
        goto cleanup;
    }

    if (DeviceState != PciNotStarted) {
        Status = STATUS_INVALID_DEVICE_STATE;
        goto cleanup;
    }

    StrLenIn = (ULONG)wcslen(DevicePath);

    if (Extension->MultiSzProxyDevicePath == NULL) {
        ByteSize = sizeof(WCHAR);
        DevicePathLocal = (WCHAR*)PciAllocatePool(PagedPool, ByteSize);
        if (DevicePathLocal == NULL) {
            Status = STATUS_INSUFFICIENT_RESOURCES;
            goto cleanup;
        }
        RtlZeroMemory(DevicePathLocal, ByteSize);

        Extension->MultiSzProxyDevicePath = DevicePathLocal;
        Extension->ByteSize = ByteSize;
        DevicePathLocal = NULL;
    }

    Status = VmProxyIsSubString(Extension->MultiSzProxyDevicePath,
                                DevicePath,
                                &SubStrLocation);
    if (!NT_SUCCESS(Status)) {
        TraceEvents(WppRecorderLogGetDefault(),
                    TRACE_LEVEL_ERROR,
                    DBG_INIT,
                    "VmProxyIsSubString failed %!STATUS!",
                    Status);
        goto cleanup;
    }

    if (SubStrLocation != NULL) {
        goto cleanup;
    }

    //
    // Add new string to the multi-sz device path
    //

    ByteSize = Extension->ByteSize + (StrLenIn + 1) * sizeof(WCHAR);
    DevicePathLocal = (WCHAR*)PciAllocatePool(PagedPool, ByteSize);
    if (DevicePathLocal == NULL) {
        Status = STATUS_INSUFFICIENT_RESOURCES;
        goto cleanup;
    }
    RtlZeroMemory(DevicePathLocal, ByteSize);
    RtlCopyMemory(DevicePathLocal, DevicePath, StrLenIn * sizeof(WCHAR));
    Dest = DevicePathLocal + StrLenIn + 1;
    RtlCopyMemory(Dest, Extension->MultiSzProxyDevicePath,
                  Extension->ByteSize);
    PciFreePool(Extension->MultiSzProxyDevicePath);
    Extension->MultiSzProxyDevicePath = DevicePathLocal;
    DevicePathLocal = NULL;
    Extension->ByteSize = ByteSize;

cleanup:

    if (DevicePathLocal) {
        PciFreePool(DevicePathLocal);
        DevicePathLocal = NULL;
    }

    return Status;
}


NTSTATUS
VmProxyRemovePath(
    _In_ PCONTROL_DEVICE_EXTENSION Extension,
    _In_z_ const WCHAR *DevicePath
    )

/*++

Routine Description:

    This routine removes a proxy device location path from device extension.
    This routine is not thread safe. The caller should hold proper lock for
    multi-thread environment.

Arguments:

    Extension - Supplies control device extension.

    DevicePath - Supplies the device location string to remove.

Return Value:

    NTSTATUS code.

--*/

{
    ULONG BytesToMove;
    WCHAR *Des;
    ULONG NewBytes;
    WCHAR *Src;
    NTSTATUS Status;
    WCHAR *SubStrLocation;

    PAGED_CODE();

    BytesToMove = 0;
    Des = NULL;
    NewBytes = 0;
    Src = NULL;
    Status = STATUS_SUCCESS;
    SubStrLocation = NULL;

    PCI_ASSERT(Extension != NULL);
    if (DevicePath == NULL || wcslen(DevicePath) == 0) {
        Status = STATUS_INVALID_PARAMETER;
        goto cleanup;
    }

    if (Extension->MultiSzProxyDevicePath == NULL ||
        wcslen(Extension->MultiSzProxyDevicePath) == 0) {

        Status = STATUS_NOT_FOUND;
        goto cleanup;
    }

    Status = VmProxyIsSubString(Extension->MultiSzProxyDevicePath,
                                DevicePath,
                                (const WCHAR**)&SubStrLocation);
    if (!NT_SUCCESS(Status)) {
        TraceEvents(WppRecorderLogGetDefault(),
                    TRACE_LEVEL_ERROR,
                    DBG_INIT,
                    "VmProxyIsSubString failed %!STATUS!",
                    Status);
        goto cleanup;
    }

    if (SubStrLocation == NULL) {
        Status = STATUS_NOT_FOUND;
        goto cleanup;
    }

    //
    // Remove the string from the multi-sz device path
    //

    PCI_ASSERT(Extension->ByteSize % sizeof(WCHAR) == 0);
    Src = SubStrLocation + (ULONG)wcslen(SubStrLocation) + 1;
    BytesToMove = Extension->ByteSize -
        ((ULONG)(Src - Extension->MultiSzProxyDevicePath)) *
        ((ULONG)sizeof(WCHAR));
    PCI_ASSERT(BytesToMove >= sizeof(WCHAR));
    NewBytes = Extension->ByteSize -
        (((ULONG)wcslen(SubStrLocation) + 1) * (ULONG)sizeof(WCHAR));
    RtlMoveMemory(SubStrLocation, Src, BytesToMove);
    Extension->ByteSize = NewBytes;

cleanup:

    return Status;
}


NTSTATUS
VmProxyIsSubString(
    _In_ PCZZWSTR MultiSzStr,
    _In_ PCWSTR SubStrIn,
    _Out_ PCWSTR* SubStrLocation
    )
/*++

Routine Description:

    This routine checks if the supplied string is a sub-string of Multistring.
    The string is not case sensitive.
    This routine is not thread safe. The caller should hold proper lock in
    multi-threaded environment.

Arguments:

    MultiSzStr - Supplies the multi-sz string.

    SubStrIn - Supplies the sub-string.

    SubStrPtr - Supplies a double pointer to receive the starting
        location of Substring in multistring. Null is assigned if no sub-string
        found.

Return Value:

    NTSTATUS code.

--*/
{
    NTSTATUS Status;
    ULONG StrLen;
    ULONG StrLenIn;
    PCWSTR SubStr;

    PAGED_CODE();

    Status = STATUS_SUCCESS;
    StrLen = 0;
    StrLenIn = 0;
    SubStr = NULL;

    if (SubStrLocation != NULL) {
        *SubStrLocation = NULL;
    }

    if (MultiSzStr == NULL || SubStrIn == NULL || wcslen(SubStrIn) == 0 ) {
        Status = STATUS_INVALID_PARAMETER;
        goto cleanup;
    }

    if (wcslen(MultiSzStr) == 0) {
        goto cleanup;
    }

    StrLenIn = (ULONG)wcslen(SubStrIn);
    for (SubStr = MultiSzStr; ;SubStr += StrLen) {
        if (*SubStr == L'\0') {
            SubStr += 1;

            //
            // Two NULLs in a row designates the end of the multi-sz.
            //

            if (*SubStr == L'\0') {
                break;
            }
        }

        StrLen = (ULONG)wcslen(SubStr);
        if (StrLen != StrLenIn) {
            continue;
        }

        if (wcscmp(SubStr, SubStrIn) == 0) {
            *SubStrLocation = SubStr;
            break;
        }
    }

cleanup:

     return Status;
}

NTSTATUS
VmProxyAddRemovePath(
    _In_ PCONTROL_DEVICE_EXTENSION Extension,
    _In_ const PIRP Irp,
    _In_ BOOLEAN AddPath
    )
/*++

Routine Description:

Arguments:

    Extension - Supplies control device extension

Return Value:

    NTSTATUS code.

--*/
{
    PPCI_ADDREMOVE_VMPROXYPATH InputBuffer;
    PIO_STACK_LOCATION IrpSp;
    BOOLEAN Locked;
    NTSTATUS Status;

    PAGED_CODE();

    Locked = FALSE;
    IrpSp = IoGetCurrentIrpStackLocation(Irp);
    InputBuffer = (PPCI_ADDREMOVE_VMPROXYPATH)Irp->AssociatedIrp.SystemBuffer;
    Status = STATUS_SUCCESS;

    if (IrpSp->Parameters.DeviceIoControl.InputBufferLength <
        (FIELD_OFFSET(PCI_ADDREMOVE_VMPROXYPATH, LocationPath) +
        InputBuffer->LocationPathLength)) {

        Status = STATUS_INVALID_PARAMETER;
        goto cleanup;
    }

    if (InputBuffer->LocationPathLength % 2 != 0 ||
        InputBuffer->LocationPathLength/2 < 5) {

        //
        // the location path should be at least L"PCI" big.
        //

        Status = STATUS_INVALID_PARAMETER;
        goto cleanup;
    }

    if (InputBuffer->LocationPath[InputBuffer->LocationPathLength/2 - 1] != L'\0') {

        //
        // the location path should be NULL-terminated.
        //

        Status = STATUS_INVALID_PARAMETER;
        goto cleanup;
    }

    PciAcquirePassiveLock(&Extension->ProxyDeviceLock);
    Locked = TRUE;

    if (AddPath) {
        Status = VmProxyAddPath(Extension, InputBuffer->LocationPath);
        if (!NT_SUCCESS(Status)) {
            TraceEvents(WppRecorderLogGetDefault(),
                        TRACE_LEVEL_ERROR,
                        DBG_INIT,
                        "VmProxyAddPath failed %!STATUS!",
                        Status);
            goto cleanup;
        }
    } else {
        Status = VmProxyRemovePath(Extension, InputBuffer->LocationPath);
        if (!NT_SUCCESS(Status)) {
            TraceEvents(WppRecorderLogGetDefault(),
                        TRACE_LEVEL_ERROR,
                        DBG_INIT,
                        "VmProxyRemovePath failed %!STATUS!",
                        Status);
            goto cleanup;
        }
    }


    Status = VmProxyWriteDevicePathsToRegistry(Extension);
    if (!NT_SUCCESS(Status)) {
        TraceEvents(WppRecorderLogGetDefault(),
                    TRACE_LEVEL_ERROR,
                    DBG_INIT,
                    "VmProxyWriteDevicePathsToRegistry failed %!STATUS!",
                    Status);
        goto cleanup;
    }

    PciReleasePassiveLock(&Extension->ProxyDeviceLock);
    Locked = FALSE;
    Status = STATUS_SUCCESS;

cleanup:

    if (Locked) {
        PciReleasePassiveLock(&Extension->ProxyDeviceLock);
        Locked = FALSE;
    }

    return Status;
}

NTSTATUS
VmProxyClearPath(
    _In_ PCONTROL_DEVICE_EXTENSION Extension
    )
/*++

Routine Description:

    This routine removes all proxy device location paths from device extension
    and registry

Arguments:

    Extension - Supplies control device extension

Return Value:

    NTSTATUS code.

--*/
{
    BOOLEAN Locked;
    NTSTATUS Status;

    PAGED_CODE();

    Locked = FALSE;
    Status = STATUS_SUCCESS;


    PciAcquirePassiveLock(&Extension->ProxyDeviceLock);
    Locked = TRUE;

    if (Extension->MultiSzProxyDevicePath == NULL ||
        wcslen(Extension->MultiSzProxyDevicePath) == 0) {

        goto cleanup;
    }

    Extension->MultiSzProxyDevicePath[0] = L'\0';
    Extension->ByteSize = sizeof(WCHAR);

    Status = VmProxyWriteDevicePathsToRegistry(Extension);
    if (!NT_SUCCESS(Status)) {
        TraceEvents(WppRecorderLogGetDefault(),
                    TRACE_LEVEL_ERROR,
                    DBG_INIT,
                    "VmProxyWriteDevicePathsToRegistry failed %!STATUS!",
                    Status);
        goto cleanup;
    }

    PciReleasePassiveLock(&Extension->ProxyDeviceLock);
    Locked = FALSE;
    Status = STATUS_SUCCESS;

cleanup:

    if (Locked) {
        PciReleasePassiveLock(&Extension->ProxyDeviceLock);
        Locked = FALSE;
    }

    return Status;
}
