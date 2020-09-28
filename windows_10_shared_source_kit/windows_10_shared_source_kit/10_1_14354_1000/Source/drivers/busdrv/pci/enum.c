/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    enum.c

Abstract:

    This module contains functions associated with enumerating the
    PCI buses.











--*/

// -------------------------------------------------------------------- Includes

#include "pcip.h"
#include "enum.tmh"
#include "hdlsterm.h"

// --------------------------------------------------------------------- Defines
// ----------------------------------------------------------------------- Types
// ------------------------------------------------------------------ Prototypes


ULONG
PciNextAriFunctionNumber(
    __in PPCI_DEVICE Device
    );


_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciProcessNewDevice(
    __in PPCI_BUS Bus,
    __in PCI_SLOT_NUMBER Slot,
    __in PPCI_DEVICE_ID_DATA IdData,
    __in_opt LPCWSTR VmProxyDevices,
    __out PPCI_DEVICE *NewDevice
    );

NTSTATUS
PciProcessDeviceIdData(
    __inout PPCI_DEVICE Device,
    __in PPCI_DEVICE_ID_DATA IdData
    );

NTSTATUS
PciSaveCapabilityPointers(
    __inout PPCI_DEVICE Device
    );

NTSTATUS
PciProcessCapabilities(
    __inout PPCI_DEVICE Device
    );

NTSTATUS
PciProcessPowerCapability(
    __inout PPCI_DEVICE Device
    );

VOID
PciProcessHtCapabilities(
    __inout PPCI_DEVICE Device
    );

BOOLEAN
PciIsExtendedConfigAvailable (
    __in PPCI_DEVICE Device
    );

NTSTATUS
PciProcessInitialConfig(
    __inout PPCI_DEVICE Device,
    __in PPCI_DEVICE_ID_DATA IdData,
    __in PPCI_SAVE_RESTORE_DATA SaveRestore,
    __out PBOOLEAN ConfigSaveRequired
    );

BOOLEAN
PciDoNotEnumerate(
    __in PPCI_DEVICE Device
    );

VOID
PciProcessBus(
    __inout PPCI_BUS Bus
    );

NTSTATUS
PciUpdateHotPlugSlotAfterBusScan (
    __inout PPCI_BUS Bus
    );

VOID
PciAddDeviceToChildList(
    __in PPCI_DEVICE Device,
    __inout PPCI_BUS Parent
    );

VOID
PciRemoveDeviceFromChildList(
    __in PPCI_DEVICE Device
    );

BOOLEAN
PciBusGetNextSlotNumber(
    __in PPCI_BUS Bus,
    __in PPCI_DEVICE LastDevice,
    __in BOOLEAN NewScan,
    __inout PPCI_SLOT_NUMBER Slot
    );

VOID
PciFixupBridgeBusNumbers(
    __in PPCI_BUS Bus
    );

// --------------------------------------------------------------------- Pragmas

#ifdef ALLOC_PRAGMA
    #pragma alloc_text(PAGE, PciScanBus)
    #pragma alloc_text(PAGE, PciProcessNewDevice)
    #pragma alloc_text(PAGE, PciScanBusCompletion)
    #pragma alloc_text(PAGE, PciProcessCapabilities)
    #pragma alloc_text(PAGE, PciSaveCapabilityPointers)
    #pragma alloc_text(PAGE, PciProcessPowerCapability)
    #pragma alloc_text(PAGE, PciProcessHtCapabilities)
    #pragma alloc_text(PAGE, PciIsExtendedConfigAvailable)
    #pragma alloc_text(PAGE, PciProcessInitialConfig)
    #pragma alloc_text(PAGE, PciProcessBus)
    #pragma alloc_text(PAGE, PciAddDeviceToChildList)
    #pragma alloc_text(PAGE, PciRemoveDeviceFromChildList)
    #pragma alloc_text(PAGE, PciFixupBridgeBusNumbers)
    #pragma alloc_text(PAGE, PciAttemptDeviceReset)
    #pragma alloc_text(PAGE, PciIsFunctionLevelResetSupported)
    #pragma alloc_text(PAGE, PciQuerySupportedResetTypes)
#endif

// --------------------------------------------------------------------- Globals

__volatile ULONG PciDeviceSequenceNumber = (ULONG)-1;

extern PULONG InitSafeBootMode;
extern PDESCRIPTION_HEADER PciWatchdogTable;
extern PPCI_DEBUG_PORT PciDebugPorts;
extern ULONG PciDebugPortsCount;

const PPCI_HEADER_OPERATIONS PciHeaderOperations[] =
{
    &PciType0HeaderOperations,
    &PciType1HeaderOperations,
    &PciType2HeaderOperations
};

HEADLESS_RSP_QUERY_PCI_INFO PciHeadlessInfo = {0};
BOOLEAN PciHeadlessInfoInitialized = FALSE;

// ------------------------------------------------------------------- Functions


ULONG
PciNextAriFunctionNumber (
    __in PPCI_DEVICE Device
    )

/*++

Routine Description:

    Returns the function number for the next function after "Device",
    according to the ARI chain.

Arguments:

    Device - Supplies the current device whence we wish to know the next one.

Returns:

    The next function number in the chain, or 0 if at the end of the chain.

--*/

{

    PCI_ASSERT(Device->AriIsEnabled != FALSE);
    PCI_ASSERT(Device->ExpressCapability != 0);
    PCI_ASSERT(Device->ExpressPort != NULL);

    return ExpressNextAriFunctionNumber(Device->ExpressPort);
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciScanBus(
    __inout PPCI_BUS Bus,
    __out PULONG DeviceCount
    )

/*++

Routine Description:

    Scan the bus (detailed in Bus) for any PCI devices/functions eligible for
    control.

Arguments:

    Bus - Our extension for the PCI bus functional device object.

    DeviceCount - Returns the number of Present child devices after the scan.

Return Value:

    STATUS_NOT_FOUND if no existing devices were found and if no new devices
        were found and processed.

    STATUS_SUCCESS if any existing device was found or if any new device was
        found and processed.

--*/

{

    NTSTATUS status;
    PCI_SLOT_NUMBER slot;
    PCI_DEVICE_ID_DATA idData;
    PPCI_DEVICE currentDevice;
    PCI_DEVICE dummyDevice;
    BOOLEAN DeviceEnumerated;
    BOOLEAN devicesChanged;
    BOOLEAN newScan;
    BOOLEAN nextSlotNumberFound;
    BOOLEAN tearDownOldBuildNew;
    NTSTATUS returnStatus;
    ULONG PresentDeviceCount;
    BOOLEAN SkipDevice;
    BOOLEAN ProcessNewDeviceFailed;
    BOOLEAN BusDisabled;
    PWCHAR vmProxyDevicePaths;
    PCONTROL_DEVICE_EXTENSION ctrlDevExtension;

    PAGED_CODE();
    vmProxyDevicePaths = NULL;

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_VERBOSE,
                DBG_PNP,
                "PciScanBus initiating");

    ctrlDevExtension = PciGetControlDeviceExtension();

    PciAcquirePassiveLock(&ctrlDevExtension->ProxyDeviceLock);

    if (ctrlDevExtension->MultiSzProxyDevicePath != NULL &&
        wcslen(ctrlDevExtension->MultiSzProxyDevicePath) >0) {

        PCI_ASSERT(ctrlDevExtension->ByteSize > 0);
        vmProxyDevicePaths = (WCHAR*)PciAllocatePool(PagedPool,
            ctrlDevExtension->ByteSize);
        if (vmProxyDevicePaths == NULL) {
            PciReleasePassiveLock(&ctrlDevExtension->ProxyDeviceLock);
            return STATUS_INSUFFICIENT_RESOURCES;
        }

        RtlCopyMemory(vmProxyDevicePaths,
            ctrlDevExtension->MultiSzProxyDevicePath,
            ctrlDevExtension->ByteSize);
    }

    PciReleasePassiveLock(&ctrlDevExtension->ProxyDeviceLock);

    PciFixupBridgeBusNumbers(Bus);

    //
    // Assume no devices present.
    //

    PresentDeviceCount = 0;

    //
    // If the console is locked and the bus should be disabled while
    // the console is locked, don't scan the bus.
    //

    if (PciBusAffectedByConsoleLock(Bus) != FALSE) {

        //
        // Grab the global lock around checks to see if the bus is disabled.
        // This ensures that if the system is in the middle of transitioning
        // between lock states, all intermediate bridges' lock states have
        // been updated.
        //

        PciAcquirePassiveLock(&PciGlobalLock);
        BusDisabled = PciBusShouldBeDisabledByConsoleLock(Bus);
        PciReleasePassiveLock(&PciGlobalLock);
        if (BusDisabled != FALSE) {

            //
            // The bus is locked, pretend there are no devices on the bus
            // and return.
            //

            returnStatus = STATUS_SUCCESS;
            goto PciScanBusExit;

        } else {

            //
            // Ensure that the bus is unlocked prior to starting a scan.
            //

            PciSetBusDisabledByConsoleLock(Bus, FALSE);
        }
    }

    PciAcquirePassiveLock(&Bus->ChildDeviceLock);

    //
    // If the bus is in D3, we don't want to scan it. This is to avoid trying
    // to read from devices on the bus that are in D3 too.
    //

    if (Bus->PowerState != PowerDeviceD0) {
        for (currentDevice = Bus->ChildDevices;
             currentDevice != NULL;
             currentDevice = currentDevice->Sibling) {

            PCI_DEVICE_SET_FLAG(currentDevice, Present);
            PresentDeviceCount += 1;
        }

        PciReleasePassiveLock(&Bus->ChildDeviceLock);
        returnStatus = STATUS_SUCCESS;
        goto PciScanBusExit;
    }

    //
    // Run down the existing device list and flag each as
    // not present. This flag will be set by the bus
    // scan if the device is still present. Any device
    // with the Present flag still clear after the scan is no longer
    // in the system (could be powered off).
    //

    for (currentDevice = Bus->ChildDevices;
         currentDevice != NULL;
         currentDevice = currentDevice->Sibling) {

        PCI_DEVICE_CLEAR_FLAG(currentDevice, Present);
    }

    //
    // Enumerate slot on the bus, and determine if the slot is populated and
    // if a new device needs to be created for it.
    //

    returnStatus = STATUS_NOT_FOUND;
    devicesChanged = FALSE;
    currentDevice = NULL;
    slot.u.AsULONG = 0;
    newScan = TRUE;
    RtlClearAllBits(&Bus->EvaluatedRidMap);
    do {

        //
        // Get the next slot number that we want to scan.
        //

        nextSlotNumberFound = PciBusGetNextSlotNumber(Bus,
                                                      currentDevice,
                                                      newScan,
                                                      &slot);
        TraceEvents(Bus->BlackBoxHandle,
                    TRACE_LEVEL_VERBOSE,
                    DBG_PNP,
                    "Next slot: %d - %x:%x",
                    nextSlotNumberFound,
                    slot.u.bits.DeviceNumber,
                    slot.u.bits.FunctionNumber);

        if (nextSlotNumberFound == FALSE) {
            break;
        }

        //
        // If we have evaluated this slot before, it's because the ARI
        // capability linked list is incorrect. We should now stop scanning for
        // other devices on this bus.
        //

        if (RtlCheckBit(&Bus->EvaluatedRidMap, slot.u.AsULONG) != FALSE) {
            TraceEvents(Bus->BlackBoxHandle,
                        TRACE_LEVEL_WARNING,
                        DBG_PNP,
                        "ARI is busted");
            break;
        }

        RtlSetBit(&Bus->EvaluatedRidMap, slot.u.AsULONG);
        newScan = FALSE;

        //
        // Determine if it is legal to access config space for this
        // slot by examining the state of matching child devices.
        //

        SkipDevice = FALSE;
        for (currentDevice = Bus->ChildDevices;
             currentDevice != NULL;
             currentDevice = currentDevice->Sibling) {

            //
            // Ignore nonmatching devices.
            //

            if (currentDevice->Slot.u.AsULONG != slot.u.AsULONG) {
                continue;
            }

            //
            // If SurpriseRemoveAndReenumerateSelf was called on the device,
            // report it as missing. The SurpriseRemoveAndReenumerateSelf flag
            // will remain set until the device receives IRP_MN_SURPRISE_REMOVED.
            // After that point the check below will fall through to the check
            // for ReportedMissing, causing this device instance will be ignored,
            // and a new instance of the device will be enumerated.
            //

            if (currentDevice->SurpriseRemoveAndReenumerateSelf != 0) {
                SkipDevice = TRUE;
                break;
            }

            //
            // Ignore devices that were previously reported as removed.
            //

            if (currentDevice->ReportedMissing != FALSE) {
                continue;
            }

            TraceEvents(Bus->BlackBoxHandle,
                        TRACE_LEVEL_VERBOSE,
                        DBG_PNP,
                        "Current: %p  Slot %x:%x  %!WDM_DEVICE_POWER_STATE!  D3cold: %x",
                        currentDevice,
                        currentDevice->Slot.u.bits.DeviceNumber,
                        currentDevice->Slot.u.bits.FunctionNumber,
                        currentDevice->LogicalPowerState,
                        currentDevice->D3ColdSupport);

            //
            // If the device is in D3Cold, do not attempt to take any
            // additional action, but assume it's still present.
            //

            if (PCI_DEVICE_IN_D3COLD(currentDevice)) {

                PCI_DEVICE_SET_FLAG(currentDevice, Present);
                SkipDevice = TRUE;
                break;
            }
        }

        if (SkipDevice != FALSE) {
            continue;
        }

        //
        // Read the device's config space.
        //

        status = PciReadSlotIdData(Bus, slot, &idData);
        if (!NT_SUCCESS(status)) {
            if (status == STATUS_DEVICE_DOES_NOT_EXIST) {
                TraceEvents(Bus->BlackBoxHandle,
                            TRACE_LEVEL_VERBOSE,
                            DBG_PNP,
                            "empty slot");

            } else {

                TraceEvents(Bus->BlackBoxHandle,
                            TRACE_LEVEL_WARNING,
                            DBG_PNP,
                            "Invalid data read from device - skipping."
                            "Status: %!STATUS!",
                            status);
            }

            continue;
        }

        TraceEvents(Bus->BlackBoxHandle,
                    TRACE_LEVEL_VERBOSE,
                    DBG_PNP,
                    "device found in dev %x func %x",
                    slot.u.bits.DeviceNumber,
                    slot.u.bits.FunctionNumber);

        //
        // Add the device to the late restore list if KSR is enabled.
        //

        if (PciKSREnabled != FALSE) {
            PciLateRestoreAddNode(Bus, slot);
            PciLateRestoreTraceList();
        }

        //
        // Reexamine all child devices and determine if any match this device.
        //

        DeviceEnumerated = FALSE;
        for (currentDevice = Bus->ChildDevices;
             currentDevice != NULL;
             currentDevice = currentDevice->Sibling) {

            //
            // Ignore nonmatching devices or devices that were previously
            // reported as removed.
            //

            if (currentDevice->Slot.u.AsULONG != slot.u.AsULONG) {
                continue;
            }

            if (currentDevice->ReportedMissing != FALSE) {
                continue;
            }

            //
            // Check if this device has already been enumerated.
            //

            if (PciIsSameDevice(currentDevice, &idData)) {

                TraceEvents(Bus->BlackBoxHandle,
                            TRACE_LEVEL_VERBOSE,
                            DBG_PNP,
                            "already found");

                //
                // Check VmProxy device
                //

                status = VmProxyCheckTearDown(currentDevice,
                    vmProxyDevicePaths,
                    &tearDownOldBuildNew);

                //
                // VmProxyCheckTearDown failures are not fatal.
                // Leave the device status as-is instead of tear down.
                //

                if (!NT_SUCCESS(status)) {
                    WARN("VmProxyCheckTearDown failed (0x%X)\n", status);
                    tearDownOldBuildNew = FALSE;
                }

                if (tearDownOldBuildNew != FALSE) {
                    TRACE(ENUM_TRACE_LEVEL, "found and tore down devnode.\n");

                    //
                    // Old device keeps not-Present status and
                    // deviceEnumerated remains FALSE.
                    //
                    break;
                }

                //
                // If this is function 0, then update the multifunction
                // field of currentDevice, in case it has changed.  (Some do.)
                //

                if ((slot.u.bits.FunctionNumber == 0) &&
                    (idData.HeaderType & PCI_MULTIFUNCTION)) {
                    PCI_DEVICE_SET_FLAG(currentDevice, MultiFunction);
                } else {
                    PCI_DEVICE_CLEAR_FLAG(currentDevice, MultiFunction);
                }

                PCI_DEVICE_SET_FLAG(currentDevice, Present);
                DeviceEnumerated = TRUE;
                returnStatus = STATUS_SUCCESS;
                break;
            }
        }

        //
        // If deviceEnumerated is FALSE, no matching child device was found.
        // Create a new child device on this bus.
        //

        if (DeviceEnumerated == FALSE) {

            //
            // Release the lock while processing the new device.
            //

            PciReleasePassiveLock(&Bus->ChildDeviceLock);

            TraceEvents(Bus->BlackBoxHandle,
                        TRACE_LEVEL_VERBOSE,
                        DBG_PNP,
                        "processing new device");

            status = PciProcessNewDevice(Bus,
                                         slot,
                                         &idData,
                                         vmProxyDevicePaths,
                                         &currentDevice);

            PciAcquirePassiveLock(&Bus->ChildDeviceLock);

            if (NT_SUCCESS(status) && (currentDevice != NULL)) {

                PciAddDeviceToChildList(currentDevice, Bus);
                devicesChanged = TRUE;
                returnStatus = STATUS_SUCCESS;
                ProcessNewDeviceFailed = FALSE;

            } else {
                ProcessNewDeviceFailed = TRUE;
            }

            if (ProcessNewDeviceFailed != FALSE) {

                //
                // Failed to create the new device (or blocked form
                // enumerating it).  Fill up a device struct enough for
                // PciBusGetNextSlotNumber to work.
                //

                RtlZeroMemory(&dummyDevice, sizeof(dummyDevice));
                dummyDevice.BusNumber = Bus->SecondaryBusNumber;
                dummyDevice.Slot.u.AsULONG = slot.u.AsULONG;
                dummyDevice.Parent = Bus;
                dummyDevice.Root = Bus->Root;
                if (idData.HeaderType & PCI_MULTIFUNCTION) {
                    dummyDevice.MultiFunction = TRUE;
                }
                currentDevice = &dummyDevice;
            }
        }
    } while (nextSlotNumberFound != FALSE);

    //
    // Traverse the child device list and determine if any devices which were
    // previously present are now missing.
    //

    for (currentDevice = Bus->ChildDevices;
         currentDevice != NULL;
         currentDevice = currentDevice->Sibling) {

        if (currentDevice->Present != FALSE) {
            PresentDeviceCount += 1;

        } else {
            devicesChanged = TRUE;
            PCI_DEVICE_SET_FLAG(currentDevice, ReportedMissing);

            TraceEvents(Bus->BlackBoxHandle,
                        TRACE_LEVEL_WARNING,
                        DBG_PNP,
                        "Old device (pdox) %p not found",
                        currentDevice);
        }
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

    //
    // Perform any post processing on the devices for this bridge if
    // we found any device changes
    //

    if (devicesChanged) {
        PciProcessBus(Bus);
    }

    //
    // If there are no devices on this bus at the end of a scan, record that
    // fact.  Do this after calling PciProcessBus so that the first call into
    // that function won't see this bus as one that "booted empty."
    //

    if (PresentDeviceCount == 0) {
        PCI_BUS_SET_FLAG(Bus, OnceContainedNoDevices);

        //
        // If there are no devices, determine if the bus should be disabled.
        //

        if (PciBusAffectedByConsoleLock(Bus) != FALSE) {
            if (PciBusShouldBeDisabledByConsoleLock(Bus) != FALSE) {
                PciSetBusDisabledByConsoleLock(Bus, TRUE);
            }
        }
    }

    if (vmProxyDevicePaths != NULL) {
        PciFreePool(vmProxyDevicePaths);
        vmProxyDevicePaths = NULL;
    }

PciScanBusExit:

    *DeviceCount = PresentDeviceCount;
    return returnStatus;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciRegisterVideoAdapterBus (
    _In_ PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine adds a the bus number and adapter type to the
    VideoAdapterBusses key for the supplied video device, allowing non-PnP
    video miniport drivers to enumerate a small subset of busses to find their
    devices.

Arguments:

    Device - Supplies a video adapter PCI device.

Return Value:

    STATUS_SUCCESS if the routine added the device's bus number and adapter type
        to the registy, or the bus number and adapter type had already been
        reported by a previous call.

    A registry API failure status if any registry operation failed.

--*/

{

    HANDLE busKey;
    ULONG creationDisposition;
    HANDLE key;
    OBJECT_ATTRIBUTES keyAttributes;
    WCHAR keyPathBuffer[10];
    INTERFACE_TYPE interfaceType;
    NTSTATUS status;
    HANDLE systemKey;
    UNICODE_STRING tempString;
    HANDLE videoKey;

    TRACE(ENUM_TRACE_LEVEL,
          "Adding bus %d to regitry of Vga busses\n",
          Device->BusNumber);

    busKey = NULL;
    key = NULL;
    systemKey = NULL;
    videoKey = NULL;
    interfaceType = PCIBus;

    //
    // Open the hardware description key
    //

    RtlInitUnicodeString(&tempString,
                         L"\\REGISTRY\\MACHINE\\HARDWARE\\DESCRIPTION\\System");

    InitializeObjectAttributes(&keyAttributes,
                               &tempString,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               (HANDLE)NULL,
                               (PSECURITY_DESCRIPTOR)NULL);

    status = ZwOpenKey(&systemKey,
                       KEY_READ | KEY_WRITE,
                       &keyAttributes);

    if (NT_SUCCESS(status) == FALSE) {
        goto Exit;
    }

    //
    // Open (or create) the VideoAdapterBusses key.
    //

    RtlInitUnicodeString(&tempString, L"VideoAdapterBusses");
    InitializeObjectAttributes(&keyAttributes,
                               &tempString,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               systemKey,
                               (PSECURITY_DESCRIPTOR)NULL);

    status = ZwCreateKey(&videoKey,
                         KEY_READ | KEY_WRITE,
                         &keyAttributes,
                         0,
                         NULL,
                         REG_OPTION_VOLATILE,
                         NULL);

    if (NT_SUCCESS(status) == FALSE) {
        goto Exit;
    }

    //
    // Open (or create) the PCIBus key
    //

    RtlInitUnicodeString(&tempString, L"PCIBus");
    InitializeObjectAttributes(&keyAttributes,
                               &tempString,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               videoKey,
                               (PSECURITY_DESCRIPTOR)NULL);

    status = ZwCreateKey(&busKey,
                         KEY_READ | KEY_WRITE,
                         &keyAttributes,
                         0,
                         NULL,
                         REG_OPTION_VOLATILE,
                         &creationDisposition);

    if (NT_SUCCESS(status) == FALSE) {
        goto Exit;
    }

    //
    // Set the default value if this created a new key.
    //
    if (creationDisposition == REG_CREATED_NEW_KEY) {
        RtlInitUnicodeString(&tempString, L"");
        status = ZwSetValueKey(busKey,
                               &tempString,
                               0,
                               REG_DWORD,
                               &interfaceType,
                               sizeof(interfaceType));

        if (NT_SUCCESS(status) == FALSE) {
            goto Exit;
        }
    }

    status = RtlStringCchPrintfW(keyPathBuffer,
                                 ARRAYSIZE(keyPathBuffer),
                                 L"%04d",
                                 Device->BusNumber);

    PCI_ASSERT(NT_SUCCESS(status));
    if (NT_SUCCESS(status) == FALSE) {
        goto Exit;
    }

    RtlInitUnicodeString(&tempString, keyPathBuffer);
    InitializeObjectAttributes(&keyAttributes,
                               &tempString,
                               OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE,
                               busKey,
                               (PSECURITY_DESCRIPTOR)NULL);

    status = ZwCreateKey(&key,
                         KEY_READ | KEY_WRITE,
                         &keyAttributes,
                         0,
                         NULL,
                         REG_OPTION_VOLATILE,
                         &creationDisposition);

    if (NT_SUCCESS(status) == FALSE) {
        goto Exit;
    }

    //
    // If this is a new key, then set its default value to the bus number.
    //

    if (creationDisposition == REG_CREATED_NEW_KEY) {
        RtlInitUnicodeString(&tempString, L"");
        status = ZwSetValueKey(key,
                               &tempString,
                               0,
                               REG_DWORD,
                               &Device->BusNumber,
                               sizeof(Device->BusNumber));

    }

Exit:
    if (key != NULL) {
        ZwClose(key);
    }

    if (busKey != NULL) {
        ZwClose(busKey);
    }

    if (systemKey != NULL) {
        ZwClose(systemKey);
    }

    if (videoKey != NULL) {
        ZwClose(videoKey);
    }

    return status;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciProcessNewDevice(
    __in PPCI_BUS Bus,
    __in PCI_SLOT_NUMBER Slot,
    __in PPCI_DEVICE_ID_DATA IdData,
    __in_opt LPCWSTR         VmProxyDevices,
    __out PPCI_DEVICE *NewDevice
    )
/*++

Routine Description:

    Create and initialize a device object for the device at the given
    slot on the given bus.

Arguments:

    Bus - Our extension for the PCI bus functional device object.

    Slot - The slot that the device resides in.

    IdData - The identification data of the device.

    VmProxyDevices - Supplies the Multi_sz location paths for VM proxy
    devices. If this is NULL, there are no proxy devices in the system.

    NewDevice - On output, points to the newly initialized PCI_DEVICE structure.

Return Value:

    NT status.

--*/
{

    BOOLEAN busNumbersConfigured;
    PCI_BRIDGE_REQUIREMENTS bridgeRequirements;
    BOOLEAN childNeedsConfigAccess;
    BOOLEAN configSaveRequired;
    PPCI_DEVICE device;
    WCHAR deviceString[32];
    PCI_SAVED_CONFIGURATION initialConfig;
    DEVICE_POWER_STATE initialPowerState;
    UCHAR line;
    ULONGLONG oemHackFlags;
    PDEVICE_OBJECT physicalDeviceObject;
    UCHAR primaryBus;
    BOOLEAN proxyDevice;
    PPCI_ROOT Root;
    PCI_CRITICAL_ROUTINE_CONTEXT routineContext;
    BOOLEAN savedExpressSerialNumber;
    PCI_SAVE_RESTORE_DATA saveRestore;
    PCI_SLOT_PERSISTENT_INFO slotPersistentInfo;
    NTSTATUS status;
    UNICODE_STRING unicodeDeviceString;

    PAGED_CODE();

    proxyDevice = FALSE;
    childNeedsConfigAccess = FALSE;

    TRACE(ENUM_TRACE_LEVEL,
          "Found new device @%x.%x.%x [%04x:%04x] Header=%x Class=%x/%x\n",
          Bus->SecondaryBusNumber,
          Slot.u.bits.DeviceNumber,
          Slot.u.bits.FunctionNumber,
          IdData->VendorID,
          IdData->DeviceID,
          IdData->HeaderType,
          IdData->BaseClass,
          IdData->SubClass
          );

    device = NULL;

    //
    // PCI device objects are named NTPNP_PCIxxxx, where xxxx is
    // a unique number (PciDeviceSequenceNumber)
    //

    status = RtlStringCbPrintfW(deviceString,
                                sizeof(deviceString),
                                L"\\Device\\NTPNP_PCI%04d",
                                InterlockedIncrement(&PciDeviceSequenceNumber)
                                );
    PCI_ASSERT(NT_SUCCESS(status));
    UNREFERENCED_PARAMETER(status);

    RtlInitUnicodeString(&unicodeDeviceString, deviceString);

    status = IoCreateDevice(
                Bus->DeviceObject->DriverObject, // our driver object
                sizeof(PCI_DEVICE),              // size of our extension,
                &unicodeDeviceString,            // our name
                FILE_DEVICE_UNKNOWN,             // device type
                0,                               // device characteristics
                FALSE,                           // not exclusive
                &physicalDeviceObject            // store new device object here
                );
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    device = (PPCI_DEVICE)physicalDeviceObject->DeviceExtension;
    device->Signature = PCI_DEVICE_SIGNATURE;
    device->D0DelayInMilliseconds = DEVICE_LINK_ON_SETTLE_TIME;
    device->MpsSizeOverride = 0;
    device->MpsSizeOverrideSet = 0;
    PCI_DEVICE_INITIALIZE_DELAYS(device->DelayInfo);

    //
    // First save the location information of the device so we can do
    // config access.
    //

    device->BusNumber = Bus->SecondaryBusNumber;
    device->Slot.u.AsULONG = Slot.u.AsULONG;
    device->Parent = Bus;
    device->Root = Bus->Root;
    device->PossibleDecodes = PCI_DECODE_ENABLES;
    device->ResourceDecodes = PCI_DECODE_ENABLES;
    device->DebugDevicePowerCallbackHandle = NULL;

    //
    // Bring the parent bus into D0, if it isn't already. Will release this
    // reference at the end of the function.
    //

    PciBus_PowerReference(Bus, PciBusPowerReasonChildNeedsConfigAccess);
    childNeedsConfigAccess = TRUE;

    //
    // check to see if it belongs to pci proxy
    //

    status = VmProxyIsProxyDevice(device, VmProxyDevices, &proxyDevice);
    if (!NT_SUCCESS(status)) {
        ERROR("VmProxyIsProxyDevice failed. 0x%08X \n", status);
        goto cleanup;
    }
    device->PciOwned = (!proxyDevice);

    //
    // Next process all the identification information that can be
    // gleaned from the config header.  This includes id registers and
    // capabilities locations.
    //

    status = PciProcessDeviceIdData(device, IdData);
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    status = PciSaveCapabilityPointers(device);
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    //
    // If the device is on the POST path, cache this now before the resources
    // change.
    //

    if (PciOnPostPath(device) != FALSE) {
        device->OnPostPath = TRUE;
    }

    //
    // Now that the device has been fully identified, the
    // hack flags can be read.
    //

    status = PciGetHackFlagsFromEM(physicalDeviceObject);
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    //
    // Interim solutions to support OEM private hackflags that they may have set
    // themselves. OR these with the hackflags retrieved from the Errata
    // Manager.
    //

    PciGetHackFlagsFromRegistry(device, &oemHackFlags);
    device->HackFlags |= oemHackFlags;
    PciMungeHackFlags(device);

    //
    // With the hack flags in place, we can see if this device should be
    // omitted from enumeration for any reason...
    //

    if (PciDoNotEnumerate(device)) {
        status = STATUS_SUCCESS;
        goto cleanup;
    }

    //
    // ... and do the rest of enumeration
    //
    device->DeviceObject = physicalDeviceObject;
    PCI_DEVICE_SET_FLAG(device, Present);

    PciInitializeState((PPCI_COMMON_EXTENSION)device);

    KeInitializeSpinLock(&device->WakeIrpLock);
    KeInitializeSpinLock(&device->D0IrpLock);
    IoInitializeRemoveLock(&device->RemoveLock, 'DicP', 0, 0);
    KeInitializeTimer(&device->TransactionTimer);
    KeInitializeDpc(&device->TransactionTimerDpc,
                    &PciPowerDownDeviceTimerDpc,
                    device);
    KeInitializeTimer(&device->PowerTimer);
    KeInitializeDpc(&device->PowerTimerDpc,
                    &PciPowerUpDeviceTimerDpc,
                    device);

    PciApplyInitialHacks(device);

    status = PciProcessCapabilities(device);
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    //
    // At this point, the LogicalPowerState of the device should be finalized.
    // Take the PciBusPowerReasonChildNeedsD0 reference if the device starts in
    // D0 to satisfy the invariant that *The number of ChildNeedsD0 reference
    // is equal to the number of child devices with their LogicalPowerState equal
    // to D0*.
    //

    if (device->LogicalPowerState == PowerDeviceD0) {
        PciBus_PowerReference(Bus, PciBusPowerReasonChildNeedsD0);
    }

    //
    // Now read and process the various registers we need to save and restore.
    //

    PciGetSaveRestoreData(device, &saveRestore);

    if (saveRestore.Command & PCI_DECODE_ENABLES) {
        PCI_DEVICE_SET_FLAG(device, Connected);
    }

    if ((device->HeaderType == PCI_BRIDGE_TYPE) ||
        (device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) {

        //
        // If this is a bridge or root port, check to see if the busmaster
        // bit was left disabled by firmware. This is a signal that devices
        // behind the bridge or root port should not be trusted until the user
        // has logged in.
        //

        if ((saveRestore.Command & PCI_ENABLE_BUS_MASTER) == 0) {
            PCI_DEVICE_SET_FLAG(device, BusmasterDisabledOnBoot);
        }

        PciReadConfigRegister(device, bridgeCommon.PrimaryBus, &primaryBus);

        busNumbersConfigured = PciAreBusNumbersConfigured(
                                    device->Parent,
                                    primaryBus,
                                    saveRestore.bridge.SecondaryBusNumber,
                                    saveRestore.bridge.SubordinateBusNumber);

        //
        // Check if there is any policy override for bus number configuration.
        //

        if ((PciSystemWideHackFlags &
             PCI_SYS_HACK_IGNORE_BUS_NUMBERS_FOR_EXPRESS_BRIDGES) != 0) {

            if ((device->ExpressCapability != 0) &&
                ((saveRestore.bridge.BridgeControl & PCI_ENABLE_BRIDGE_VGA) == 0)) {

                busNumbersConfigured = FALSE;
            }
        }

        if (busNumbersConfigured != FALSE) {
             device->SavedRegisters.bridge.SecondaryBusNumber
                = saveRestore.bridge.SecondaryBusNumber;
            device->SavedRegisters.bridge.SubordinateBusNumber
                = saveRestore.bridge.SubordinateBusNumber;

            PCI_DEVICE_SET_FLAG(device, BusNumbersConfigured);

        } else {
            saveRestore.bridge.SecondaryBusNumber = 0;
            saveRestore.bridge.SubordinateBusNumber = 0;
        }
    }

    if (PciIsExtendedConfigAvailable(device) != FALSE) {
        PCI_DEVICE_SET_FLAG(device, ExtendedConfigAvailable);
    } else {
        PCI_DEVICE_CLEAR_FLAG(device, ExtendedConfigAvailable);
    }

    //
    // Initialize the interrupt related fields.
    //

    if (PCI_LEGACY_IDE_CONTROLLER(device)) {

        device->InterruptPin = 0;
        device->InterruptResource.Type = PciNone;

    } else {

        PciReadConfigRegister(device, common.InterruptPin, &device->InterruptPin);

        //
        // Record the current InterruptLine for boot config reasons.
        // Don't do this for cardbus devices, as the interrupt for these
        // comes from the bridge, so a boot config is not interesting.
        //

        device->InterruptResource.Type = PciNone;

        if ((device->InterruptPin != 0) &&
            !PCI_CARDBUS_BRIDGE(device->Parent)) {

            line = PciGetHalReportedInterruptLine(device);

            if ((line != 0) && (line != 0xFF)) {

                device->InterruptResource.Type = PciLineBased;
                device->InterruptResource.InterruptLine = line;
            }
        }
    }

    //
    // Now munge and save the initial state of the device.
    // Note that the SavedRegisters, IchWindows and OnDebugPath are initialized
    // here, so don't try to use them before this routine is called.
    //

    configSaveRequired = TRUE;
    status = PciProcessInitialConfig(device, IdData, &saveRestore, &configSaveRequired);
    if (!NT_SUCCESS(status)) {
        goto cleanup;
    }

    //
    // During the prior call to PciProcessInitialConfig(), if there was a
    // persisted serial number found in the registry then note it so
    // we avoid an unnecessary registry update later.
    //
    savedExpressSerialNumber = (device->InstanceIdSerialNumber != 0);

    //
    // If this is a PCI Express device then process its express capabilities.
    // If the device is controlled by the OS, create a new express port for
    // the device. If the device is controlled by the system BIOS, create a
    // compatibility port for storing its read-only data.
    //

    if (device->ExpressCapability != 0) {
        Root = device->Root;
        if ((Root->HardwareCapability.OscControlGranted.u.AsULONG != 0) ||
            (PciSystemWideHackFlags & PCI_SYS_HACK_CONTROL_EXPRESS_FEATURES)) {

            status = ExpressProcessNewPort(device);
            if (NT_SUCCESS(status) == FALSE) {

                //
                // In order to aid in diagnosibility, a failure in the
                // routine above is recorded in the device extension instead
                // of causing the processing of this new device to fail.
                // This is so that the PDO, along with state describing
                // the failure, is kept in memory for later examination.  The
                // failure recorded in the extension will instead be converted
                // into a Start IRP failure, a failure which not only keeps
                // the PDO in memory for debugging but also causes a clear
                // device manager problem code that can be examined by the
                // user.  In this case, make the device appear internally to
                // be a non-express device, so that other code does not trip
                // up on the fact that an express device does not have a port
                // structure created for it.
                //
                // N. B. Because of this behavior, no hack flag may be defined
                //       to explicitly override a Start IRP failure.  Such a
                //       hack flag would allow this partially-configured device
                //       to be started successfully, with undefined results.
                //

                device->ExpressCapability = 0;
                status = STATUS_SUCCESS;
            }

        } else {
            status = ExpressInitializeNewCompatibilityPort(device);

            //
            // Failure in creating a compatibility port is ignored because
            // the device can still function properly without reporting
            // PCI Express properties.
            //

            if (NT_SUCCESS(status) == FALSE) {
                status = STATUS_SUCCESS;
            }
        }
    }

    //
    // Apply any relevent hotplug parameters that the FW provided
    // for devices that aren't initially configured.
    //

    if (device->NoInitialConfig) {

        //
        // HPX supercedes HPP.
        //

        status = PciApplyHPX(device);
        if (!NT_SUCCESS(status)) {
            PciApplyHPP(device);
        }
    }

    //
    // Power up the device so we can probe its bars
    //

    if (device->PowerCapability) {

        initialPowerState = PciGetHardwarePowerState(device);
        if (initialPowerState != PowerDeviceD0) {

            status = PciSetHardwarePowerStatePassive(device, PowerDeviceD0);
            if (!NT_SUCCESS(status)) {
                goto cleanup;
            }
        }
    } else {
        initialPowerState = PowerDeviceD0;
    }

    if (!(device->HackFlags & PCI_HACK_ENUM_NO_RESOURCE)) {

        if ((device->HackFlags & PCI_HACK_CRITICAL_DEVICE) || device->OnDebugPath) {

            //
            // If this is a critical device (one that cannot be safely
            // turned off to determine the limits), call the routine
            // in the context of KeIpiGenericCall, which brings all processors
            // into step, guaranteeing that nothing else will run on
            // the system while we're determining the limits.
            //

            routineContext.Gate = 1;
            routineContext.Barrier = 1;
            routineContext.Routine = PciProbeForRequirements;
            routineContext.Extension = device;
            routineContext.Context = NULL;
            KeIpiGenericCall(PciExecuteCriticalSystemRoutine,
                             (ULONG_PTR)&routineContext
                             );

            status = routineContext.Status;

        } else {

            status = PciProbeForRequirements(device, NULL);
        }

        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }

        status = PciGetInterruptRequirement(device, &device->InterruptRequirement);
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }
    }

    //
    // Certain devices (e.g. memory controller, ULDT) that have hyper
    // transport based MSI mapping capability are not in the hierarchy
    // of devices that use MSI. In addition such devices have a decoration
    // of NO_DRV in the infs. These devices are not started from the PnP's
    // perspective and do not receive any D-Irps because there is no power
    // policy owner for it. But enabling an MSI mapping capability is still
    // required otherwise MSI messages from PCI devices will not be converted
    // to hypertransport interrupts. Such devices are assigned a hack flag
    // and their MSI mapping capability is programmed once during enumeration
    // and second during processing of D0 by the root bus.
    //
    // N.B. It is assumed that these devices are always present on the root
    //      bus.
    //
    //

    if ((device->HtMsiMapCapability != 0) &&
        ((device->HackFlags & PCI_HACK_PROGRAM_HT_MSI_MAP) != 0)) {

        PciProgramHtMsiMapCapability(device);
    }

    //
    // Clear boot configured use of MSI/MSI-X interrupts for the device.
    // The MSI resource for the device will be re-arbitrated and enabled
    // during start.
    //

    if (device->MsiXCapability != 0) {
        PciMaskMsiXInterrupt(device);
    }

    if (device->MsiCapability != 0) {
        PciMaskMsiInterrupt(device);
    }

    //
    // Put the hardware back into the power state we found it.
    // If the device was initially in a low power state we will power
    // it up when we start it up.  If there's no driver and the device
    // never starts, we save power by turning it off.
    //

    if (initialPowerState != PowerDeviceD0) {

        status = PciSetHardwarePowerStatePassive(device, initialPowerState);
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }
    }

    status = device->Operations->GetResources(device, &device->Resources);
    UNREFERENCED_PARAMETER(status);

    //
    // If we don't already have saved information about this device in the registry,
    // save it now.  Note that this must be done after requirements generation since
    // we need the values of the resources, which can only be calculated after
    // the requirements have been generated.
    //

    if (((PciSystemWideHackFlags & PCI_SYS_HACK_DISABLE_EXPRESS_SERIAL_NUMBER_CACHING) == 0) &&
        (savedExpressSerialNumber == FALSE) &&
        (device->InstanceIdSerialNumber != 0)) {

        RtlCopyMemory(&slotPersistentInfo.IdData,
                      IdData,
                      sizeof(PCI_DEVICE_ID_DATA));

        slotPersistentInfo.ExpressSerialNumber = device->InstanceIdSerialNumber;
        status = PciSaveSlotPersistentInfo(device, &slotPersistentInfo);
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }
    }

    if (configSaveRequired) {

        RtlCopyMemory(&initialConfig.SaveRestore,
                      &device->SavedRegisters,
                      sizeof(PCI_SAVE_RESTORE_DATA));
        RtlCopyMemory(&initialConfig.IdData,
                      IdData,
                      sizeof(PCI_DEVICE_ID_DATA));

        if (device->HackFlags & PCI_HACK_IGNORE_BOOT_CONFIG) {
            RtlZeroMemory(&initialConfig.Resources,
                          sizeof(PCI_DEVICE_RESOURCES));

        } else {
            RtlCopyMemory(&initialConfig.Resources,
                          &device->Resources,
                          sizeof(PCI_DEVICE_RESOURCES));

        }

        RtlZeroMemory(&initialConfig.Bridge, sizeof(ULONG));
        if ((device->HeaderType == PCI_BRIDGE_TYPE) ||
            (device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) {

            status = PciDeviceRequirementsToBridgeRequirements(
                            device,
                            &device->Requirements,
                            &bridgeRequirements
                            );
            if (!NT_SUCCESS(status)) {
                goto cleanup;
            }

            if (bridgeRequirements.IoWindow.Disabled) {
                initialConfig.Bridge.IoWindowDisabled = 1;
            }
            if (bridgeRequirements.MemoryWindow.Disabled) {
                initialConfig.Bridge.MemoryWindowDisabled = 1;
            }
            if (bridgeRequirements.PrefetchWindow.Disabled) {
                initialConfig.Bridge.PrefetchWindowDisabled = 1;
            }
        }

        status = PciSaveDeviceConfiguration(device, &initialConfig);
        if (!NT_SUCCESS(status)) {
            goto cleanup;
        }
    }

    //
    // If this is a VGA device, mark its bus in the registry so that VGASAVE
    // can find it.
    //

    if (PCI_VGA_CONTROLLER(device)) {
        PciRegisterVideoAdapterBus(device);
    }

    //
    // Take two power references on the parent bus, one for the child device
    // to stay in its current power state until the device starts and one
    // so that the bus won't power off completely when the device turns off.
    // The first reference will be removed when the device is successfully
    // started (and it will have another reference then for official D0 status.)
    // The second will be removed if the driver marks itself as capable of
    // supporting D3cold.
    //

    PciBus_PowerReference(Bus, PciBusPowerAllowF1DisableBridgeD3);
    PciBus_PowerReference(Bus, PciBusPowerReasonBusPnpOperationPending);
    physicalDeviceObject->Flags &= ~DO_DEVICE_INITIALIZING;
    PciDebugDevicePowerCallbackRegister(device);
    *NewDevice = device;

    if (childNeedsConfigAccess != FALSE) {
        PciBus_PowerDereference(Bus, PciBusPowerReasonChildNeedsConfigAccess);
    }

    return STATUS_SUCCESS;

cleanup:

    if (device != NULL) {
        if (device->ExpressPort != NULL) {
            ExpressDestroyPort(device->ExpressPort);
        }
        if (device->ExpressCompatibilityPort != NULL) {
            ExpressDestroyCompatibilityPort(device->ExpressCompatibilityPort);
        }

        if (childNeedsConfigAccess != FALSE) {
            PciBus_PowerDereference(Bus, PciBusPowerReasonChildNeedsConfigAccess);
        }
    }
    if (physicalDeviceObject != NULL) {
        IoDeleteDevice(physicalDeviceObject);
    }

    *NewDevice = NULL;
    return status;
}

NTSTATUS
PciScanBusCompletion (
    __inout PPCI_BUS Bus
    )

/*++

Routine Description:

    This is a post processing routine for a BusRelations Irp. Since the Irp
    is on its way up, the ACPI driver got a chance to attach filters on the
    PDOs created by the PCI driver thereby allowing PCI to evaluate any ACPI
    method defined in the scope of the device.

Arguments:

    Bus - Supplies a pointer to a PCI bus.

Return Value:

    STATUS_SUCCESS.

--*/

{

    USHORT CommandRegister;
    PPCI_DEVICE Device;
    BOOLEAN IgnoreBootConfig;
    PCI_SAVED_CONFIGURATION SavedConfig;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // One of the major tasks is to evaluate a _DSM method defined in the scope
    // of the device. If the result of this method is TRUE (non-zero), then the
    // PCI driver ignores boot configuration on the device. If the device is a
    // bridge then ignore boot configuartion for the hierarchy starting at the
    // bridge.
    //

    PciAcquirePassiveLock(&Bus->ChildDeviceLock);
    for (Device = Bus->ChildDevices;
         Device != NULL;
         Device = Device->Sibling) {

        //
        // If a device is no longer present then do not bother with any
        // processing for ignoring boot configs.
        //

        if (Device->Present == FALSE) {
            continue;
        }

        //
        // If a device is enumerated then do not bother with any processing
        // for ignoring boot configs.
        //

        if (Device->DeviceState != PciNotStarted) {
            continue;
        }

        //
        // Check if the bus has an ignore boot config flag set in which
        // case the device will automatically inherit this setting.
        //

        if (Bus->IgnoreBootConfig == 0) {

            //
            // Check if this method was previously evaluated on the device in
            // which case it makes sense to skip its evaluation.
            //

            if (Device->DsmToIgnoreBootConfigEvaluated == 0) {
                IgnoreBootConfig = FALSE;
                Status = PciEvaluateDsmMethodToIgnoreBootConfig(Device->DeviceObject,
                                                                &IgnoreBootConfig);

                if (Status != STATUS_INSUFFICIENT_RESOURCES) {
                    PCI_DEVICE_SET_FLAG(Device, DsmToIgnoreBootConfigEvaluated);
                }

                if (!NT_SUCCESS(Status) || (IgnoreBootConfig == FALSE)) {
                    continue;
                }

            } else {
                if (Device->IgnoreBootConfig == 0) {
                    continue;
                }
            }
        }

        //
        // Update the ignore boot config setting in the device. This must
        // happen before any further checks.
        //

        PCI_DEVICE_SET_FLAG(Device, IgnoreBootConfig);

        //
        // If the device is on a debugging path or a critical device then the
        // policy is to not ignore any boot configs.
        //
        // N.B. The above update is still necessary to propagate this info to
        //      its children in case this device is a bridge.
        //

        if (Device->OnDebugPath != 0) {
            continue;
        }

        if (Device->HackFlags & PCI_HACK_CRITICAL_DEVICE) {
            continue;
        }

        //
        // Clear the current device resources from the extension and update
        // the registry with this information.
        //

        Status = PciGetSavedDeviceConfiguration(Device, &SavedConfig);
        if (!NT_SUCCESS(Status)) {
            continue;
        }

        RtlZeroMemory(&Device->Resources, sizeof(PCI_DEVICE_RESOURCES));
        RtlZeroMemory(&SavedConfig.Resources, sizeof(PCI_DEVICE_RESOURCES));
        Status = PciSaveDeviceConfiguration(Device, &SavedConfig);
        if (!NT_SUCCESS(Status)) {
            continue;
        }

        //
        // Stop the device from decoding boot resources.
        //
        // N.B. None of these device are debugging devices or critical devices.
        //

        PciReadConfigRegister(Device, Command, &CommandRegister);
        CommandRegister &= ~(PCI_ENABLE_IO_SPACE | PCI_ENABLE_MEMORY_SPACE);
        PciWriteConfigRegister(Device, Command, &CommandRegister);
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);
    return STATUS_SUCCESS;
}

NTSTATUS
PciProcessDeviceIdData(
    __inout PPCI_DEVICE Device,
    __in PPCI_DEVICE_ID_DATA IdData
    )
/*++

Routine Description:

    Initialize the given device with identification information from
    the given config header.

Arguments:

    Device - The PCI_DEVICE to initialize

    IdData - The identification data of the device

Return Value:

    NT status.

--*/
{
    NON_PAGED_CODE();

    //
    // First save the identification data for this device
    //
    Device->VendorID = IdData->VendorID;
    Device->DeviceID = IdData->DeviceID;
    Device->RevisionID = IdData->RevisionID;
    Device->ProgIf = IdData->ProgIf;
    Device->SubClass = IdData->SubClass;
    Device->BaseClass = IdData->BaseClass;
    Device->SubVendorID = IdData->SubVendorID;
    Device->SubSystemID = IdData->SubSystemID;
    Device->HeaderType = IdData->HeaderType & ~PCI_MULTIFUNCTION;

    if (Device->HeaderType > PCI_MAX_CONFIG_TYPE) {
        return STATUS_DEVICE_DATA_ERROR;
    }

    if (IdData->HeaderType & PCI_MULTIFUNCTION) {
        PCI_DEVICE_SET_FLAG(Device, MultiFunction);
    }

    PciMungeClassCodes(Device);

    //
    // Based on device type, set up which routines will
    // get called to probe and set the resources of the device.
    //
    Device->Operations = PciHeaderOperations[Device->HeaderType];

    return STATUS_SUCCESS;
}

NTSTATUS
PciProcessCapabilities(
    __inout PPCI_DEVICE Device
    )

/*++

Routine Description:

    Process the extended capabilities of the given device.

Arguments:

    Device - The PCI_DEVICE whose capabilities should be validated

Return Value:

    NT status.

--*/

{

    NTSTATUS status;
    NTSTATUS tempStatus;

    PAGED_CODE();

    status = STATUS_SUCCESS;
    if (Device->PowerCapability) {
        tempStatus = PciProcessPowerCapability(Device);
        if (!NT_SUCCESS(tempStatus)) {
            status = tempStatus;
        }
    }

    //
    // Process HT capabilities for devices regardless of whether the device
    // actually contains HT capability pointers.  The HT capabilities for the
    // multi-function device may be in function 0.
    //
    PciProcessHtCapabilities(Device);

    //
    // Any SubsystemIDs capability had already been validated as it contains
    // device identification data we need early in enumeration
    //

    return status;
}

NTSTATUS
PciSaveCapabilityPointers(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    Locate and process the extended capabilities of the given device.

Arguments:

    Device - The PCI_DEVICE whose capabilities should be processed

    Config - The config space header of the device.

Return Value:

    NT status.

--*/
{
    UCHAR minCapOffset, currentCap;
    PCI_CAPABILITIES_HEADER header;
    PCI_HT_CAPABILITY ht;
    EM_RULE_STATE emState;
    NTSTATUS ntStatus;
    USHORT status;

    PAGED_CODE();











    //
    // If we don't have a capability list this is easy...
    //
    PciReadConfigRegister(Device, Status, &status);
    if (!(status & PCI_STATUS_CAPABILITIES_LIST)) {
        return STATUS_SUCCESS;
    }

    switch (Device->HeaderType) {

        case PCI_DEVICE_TYPE:
            PciReadConfigRegister(Device, type0.CapabilitiesPtr, &currentCap);
            minCapOffset = PCI_TYPE0_HEADER_LENGTH;
            break;

        case PCI_BRIDGE_TYPE:
            PciReadConfigRegister(Device, type1.CapabilitiesPtr, &currentCap);
            minCapOffset = PCI_TYPE1_HEADER_LENGTH;
            break;

        case PCI_CARDBUS_BRIDGE_TYPE:
            PciReadConfigRegister(Device, type2.CapabilitiesPtr, &currentCap);
            minCapOffset = PCI_TYPE2_HEADER_LENGTH;
            break;

        default:
            PCI_ASSERT(Device->HeaderType <= PCI_CARDBUS_BRIDGE_TYPE);
            return STATUS_INVALID_PARAMETER;
            break;
    }

    Device->FirstCapability = currentCap;

    while (currentCap) {

        if (currentCap < minCapOffset) {

            VERIFIER("Capability pointer pointing into header");
            return STATUS_DEVICE_PROTOCOL_ERROR;
        }
        if ((currentCap & 0x3) != 0) {
            VERIFIER("Capability pointer not DWORD aligned");
            return STATUS_DEVICE_PROTOCOL_ERROR;
        }
        if (currentCap > (sizeof(PCI_COMMON_CONFIG)
                            - sizeof(PCI_CAPABILITIES_HEADER))) {
            VERIFIER("Capability pointer extends beyond end of config space");
            return STATUS_DEVICE_PROTOCOL_ERROR;
        }

        PciReadDeviceConfig(Device,
                            &header,
                            currentCap,
                            sizeof(PCI_CAPABILITIES_HEADER)
                            );

        switch (header.CapabilityID) {

        case PCI_CAPABILITY_ID_POWER_MANAGEMENT:
            Device->PowerCapability = currentCap;
            break;

        case PCI_CAPABILITY_ID_P2P_SSID:

            if (Device->HeaderType == PCI_BRIDGE_TYPE) {
                Device->SubIDsCapability = currentCap;
            } else {
                VERIFIER("Subsystem ID capability invalid in header type other than PCI-PCI bridge");
            }
            break;

        case PCI_CAPABILITY_ID_AGP_TARGET:
        case PCI_CAPABILITY_ID_AGP:
            Device->AgpCapability = currentCap;
            break;

        case PCI_CAPABILITY_ID_PCIX:
            Device->PciXCapability = currentCap;
            break;

        case PCI_CAPABILITY_ID_ADVANCED_FEATURES:
            Device->AdvancedFeatures = currentCap;
            break;

        case PCI_CAPABILITY_ID_MSI:
            Device->MsiCapability = currentCap;
            break;

        case PCI_CAPABILITY_ID_MSIX:
            Device->MsiXCapability = currentCap;
            break;

        case PCI_CAPABILITY_ID_HYPERTRANSPORT:

            //
            // Hypertransport capabilities are special, since they
            // can appear multiple times and they have sub-types.
            // At the moment, we're interested in two things:
            //
            // 1)  Whether this device, or any parent of this device,
            //     is a HyperTransport device, since Message-Signaled
            //     Interrupts work strangely if they have to pass
            //     across a HyperTransport link.
            //
            // 2)  Whether this device has a HyperTransport MSI Mapping
            //     capability, which allows MSIs to pass onto HT.
            //
            // We need to read a bit more of the capability to tell
            // which kind of HT capability this is.
            //
            // N.B. The only sub-type that is interesting is the MSI mapping
            //      capability. But this capability must not be enabled on
            //      non-AMD platforms. So for now do not process any hyper
            //      transport capability when executing on non-AMD CPUs.
            //      If a new HT capability is defined that is required to
            //      be enabled for all platforms then this block of code
            //      will change to selectively process HT capabilities
            //      based on the CPU type. When HT capabilities are not
            //      processed, these will be in the same state as programmed
            //      by the BIOS.
            //

            ntStatus = EmClientQueryRuleState(&GUID_EM_CPU_TYPE_AMD, &emState);
            if (!NT_SUCCESS(ntStatus)) {
                break;
            }

            if (emState != STATE_TRUE) {
                break;
            }

            if (Device->FirstHtCapability == 0) {
                Device->FirstHtCapability = currentCap;
            }

            PciReadDeviceConfig(Device,
                                &ht,
                                currentCap,
                                FIELD_OFFSET(PCI_HT_CAPABILITY, Offset04)
                                );

            if (ht.Command.Generic.CapabilityType == HTInterruptMsiMapping) {
                Device->HtMsiMapCapability = currentCap;
            }

            break;

        case PCI_CAPABILITY_ID_PCI_EXPRESS:
            Device->ExpressCapability = currentCap;
            break;

        default:
            break;
        }

        currentCap = header.Next;
    }

    DebugDumpCapabilitiesList(ENUM_TRACE_LEVEL, Device);

    return STATUS_SUCCESS;
}

NTSTATUS
PciProcessPowerCapability(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    Validate the power capability of the given device.

Arguments:

    Device - The PCI_DEVICE whose power capability should be validated

Return Value:

    NT status.

--*/
{

    DEVICE_POWER_STATE wakeLevel;
    USHORT statusRegister;
    BOOLEAN sldWorkaround;
    PCI_PM_CAPABILITY pm;
    PPCI_BUS bus;

    PAGED_CODE();

    PciReadDeviceCapability(Device, PowerCapability, &pm);

    //
    // PME Enable should *not* be set on enumeration.
    //

    if (pm.PMCSR.ControlStatus.PMEEnable) {
        pm.PMCSR.ControlStatus.PMEEnable = 0;
        PciWritePowerCapabilityRegister(Device, PMCSR, &pm.PMCSR);
    }

    //
    // Check if the children on a bus or the device itself requires a
    // workaround for a surprise link down condition.
    //

    bus = Device->Parent;
    sldWorkaround = FALSE;
    if (((bus->PciBridge != NULL) &&
         (bus->PciBridge->AvoidChildD1D2ForSld != 0)) ||
        ((Device->HackFlags & PCI_HACK_AVOID_D1D2_FOR_SLD) != 0)) {

        sldWorkaround = TRUE;
    }

    //
    // Select "most powered off state" this device can issue a PME from.
    //

    wakeLevel = PowerDeviceUnspecified;
    if (pm.PMC.Capabilities.Support.PMED0) {
        wakeLevel = PowerDeviceD0;
    }

    if ((pm.PMC.Capabilities.Support.PMED1) && (sldWorkaround == FALSE)) {
        wakeLevel = PowerDeviceD1;
    }

    if ((pm.PMC.Capabilities.Support.PMED2) && (sldWorkaround == FALSE)) {
        wakeLevel = PowerDeviceD2;
    }

    if (pm.PMC.Capabilities.Support.PMED3Hot) {
        wakeLevel = PowerDeviceD3;
    }

    if (pm.PMC.Capabilities.Support.PMED3Cold) {
        wakeLevel = PowerDeviceD3;
    }

    Device->WakeLevel = wakeLevel;

    //
    // Record the current power state.
    // Note: D0 = 0, thru D3 = 3, convert to PowerDeviceD0 thru PowerDeviceD3.
    // It's only a two bit field (in h/w) so no other values are possible.
    //

    Device->LogicalPowerState =
        PCI_TO_NT_POWER_STATE(pm.PMCSR.ControlStatus.PowerState);

    //
    // Determine whether B2 is an illegal bus state for this device.  66MHz and
    // above devices are not required to support the "clock stopped" B2 bus
    // state since it would require them to reinitialize their clocks first to
    // 33MHz and then back to 66MHz.  If a device does not support B2 in 66MHz
    // mode it indicates this by clearing the D2 support field.
    //
    // At the same time, cache whether this device is 66MHz capable.
    //

    PciReadConfigRegister(Device, Status, &statusRegister);
    if (statusRegister & PCI_STATUS_66MHZ_CAPABLE) {
        PCI_DEVICE_SET_FLAG(Device, Device66MHzCapable);
        if (pm.PMC.Capabilities.Support.D2 == 0) {
            PCI_DEVICE_SET_FLAG(Device, B2On66MHzSegmentIllegal);
        }
    }

    //
    // If the device is a bridge, determine whether D3 entry must be prevented
    // to avoid putting a bus segment into B2 when it is illegal to do so (as
    // described above).  D3 is illegal if it results in a secondary bus state
    // of B2 (bridges can either transition their secondary buses to B2 or B3
    // when placed into D3) and if the bridge is operating in 66MHz mode (or any
    // other bus state above 66MHz).  Note that it is not possible to tell
    // whether a bus segment is operating in 66MHz mode.  It is only possible to
    // tell whether a bus segment is capable of 66MHz operation.  Finally, D3
    // entry is okay even in these conditions if there are no devices on the
    // secondary bus that cannot handle B2, but since the secondary bus has not
    // been enumerated at this point assume that such devices *are* present
    // until enumeration takes place.
    //

    if (Device->HeaderType == PCI_BRIDGE_TYPE) {
        if (pm.PMCSR_BSE.BridgeSupport.D3HotSupportsStopClock == 1) {
            PciReadConfigRegister(Device,
                                  type1.SecondaryStatus,
                                  &statusRegister);

            if (statusRegister & PCI_STATUS_66MHZ_CAPABLE) {
                Device->BridgeFlags.D3Causes66MHzB2 = 1;
                Device->BridgeFlags.D3Illegal = 1;
            }
        }
    }

    //
    // Remember the power capabilities.
    //

    Device->PowerSupport = pm.PMC.Capabilities.Support;
    PCI_DEVICE_SET_FLAG(Device, PmeOwned);
    return STATUS_SUCCESS;
}

VOID
PciProcessHtCapabilities(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine examines a device for its hypertransport capabilities.
    Hypertransport is unique in that multi-function devices have some
    capabilities that apply to the entire package, but are configurable only
    through function 0 of the package.  As a result, determining the full
    HT capabilities of the device requires scanning the capabilities of
    function 0.

    This routine determines whether a device is on a hypertransport link,
    since devices downstream that must send data across hypertransport may
    need special properties, particularly in the case of MSI.  It also
    determines whether the device has an HT MSI mapping capability, which
    allows downstream PCI MSIs to be converted to HT interrupts.  If this
    capability exists it is configured.

Arguments:

    Device - Supplies the device to process.

Return Value:

    None.

--*/
{
    PPCI_BUS parent;
    PPCI_DEVICE sibling;

    PAGED_CODE();

    //
    // Save HT information, which is encoded in function 0 of a device (all
    // functions share a link).  On function 0, grab it directly.  On other
    // functions, adopt the values from the function 0 device.
    //
    if (Device->Slot.u.bits.FunctionNumber == 0) {

        if (Device->FirstHtCapability != 0) {
            PCI_DEVICE_SET_FLAG(Device, HyperTransport);
        }

        if ((Device->HtMsiMapCapability != 0) &&
            ((Device->HackFlags & PCI_HACK_NOT_MSI_HT_CONVERTER) == 0)) {

            PCI_DEVICE_SET_FLAG(Device, MsiHtConverter);
        }

        return;

    } else {

        parent = Device->Parent;
        PciAcquirePassiveLock(&parent->ChildDeviceLock);
        sibling = parent->ChildDevices;
        while (sibling != NULL) {

            //
            // Search for the device extension corresponding to function 0 of
            // this device.  Take advantage of the fact that devices are
            // enumerated beginning with function 0 and scan the list of
            // already-enumerated devices on this bus.
            //
            if ((sibling->Slot.u.bits.FunctionNumber == 0) &&
                (sibling->Slot.u.bits.DeviceNumber ==
                 Device->Slot.u.bits.DeviceNumber)) {

                //
                // Inherit the HT settings from function 0.
                //
                if (sibling->HyperTransport == 0) {
                    PCI_DEVICE_CLEAR_FLAG(Device, HyperTransport);
                } else {
                    PCI_DEVICE_SET_FLAG(Device, HyperTransport);
                }

                if (sibling->MsiHtConverter == 0) {
                    PCI_DEVICE_CLEAR_FLAG(Device, MsiHtConverter);
                } else {
                    PCI_DEVICE_SET_FLAG(Device, MsiHtConverter);
                }

                PciReleasePassiveLock(&parent->ChildDeviceLock);
                return;
            }

            sibling = sibling->Sibling;
        }

        PciReleasePassiveLock(&parent->ChildDeviceLock);
    }

    return;
}

BOOLEAN
PciIsExtendedConfigAvailable (
    __in PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine determines whether extended (greater than 256 bytes) config
    space is available for a particular device.  In order to have access
    to extended config space, a device needs to be built to a hardware
    standard that supports extended config, and the entire path of bridges
    from the top of the machine down to the device must also be built
    to pass extended config cycles.

Arguments:

    Device - The device to compute the extended config availability of.

Return Value:

    TRUE if extended config can be accessed for this device.
    FALSE otherwise.

--*/

{

    BOOLEAN extendedConfigAvailable;

    PAGED_CODE();

    //
    // Start with an assumption that the extended config cycle is not available.
    //

    extendedConfigAvailable = FALSE;

    //
    // If the device is the child of a root bus, base the availability of its
    // extended configuration space on whether it is an express device. Since
    // root buses are a software construct, config cycles will definitely
    // be passed to this device.
    //

    if (PCI_ROOT_BUS(Device->Parent) != FALSE) {
        return (Device->ExpressCapability != 0);
    }

    //
    // If the device's parent doesn't pass extended config, there's
    // no way the device can claim the cycles, regardless of its
    // hardware interface.
    //

    if (Device->Parent->ExtendedConfigAvailable == FALSE) {
        return extendedConfigAvailable;
    }

    //
    // The current operating mode of the bus that this device sits
    // on will determine whether extended config cycles are available.
    //

    switch (Device->Parent->SecondaryInterface) {

    case PciExpress:

        if (Device->ExpressCapability) {
            extendedConfigAvailable = TRUE;
        }

        break;

    case PciXMode2:

        if (Device->PciXCapability) {
            extendedConfigAvailable = TRUE;
        }

        break;

    default:

        //
        // PCI-X Mode 1 and conventional PCI don't support
        // extended config space.
        //

        break;
    }

    return extendedConfigAvailable;
}

NTSTATUS
PciProcessInitialConfig(
    __inout PPCI_DEVICE Device,
    __in PPCI_DEVICE_ID_DATA IdData,
    __in PPCI_SAVE_RESTORE_DATA SaveRestore,
    __out PBOOLEAN ConfigSaveRequired
    )
/*++

Routine Description:

    This routine processes the config information for a device as
    it was when PCI first found the device.  This information
    is saved in the registry and is persistent across PDO
    deletions.  For some pieces of information, it is important
    to have this data rather than whatever happens to be in the device
    now (device may have been powered down and lost state).

    If there is no saved config, this must be the first time there is
    a PDO created for this device.  In this case, save the current
    hardware state in the registry.

Arguments:

    Device - The device to process the initial config of.

    IdData - A structure containing the identification data for the device.

    SaveRestore - A structure containing the current values of the
        various registers that need to be saved/restored by PCI.

    ConfigSaveRequired - Will be set to TRUE if no saved config was found
        and the current config needs to be saved to the registry.  This can't
        be done in this routine because the requirements of the device, which
        contribute to the saved config, can't be calculated yet.  Will be
        set to FALSE if the config does not need to be saved.

Return Value:

    NT Status code.

--*/
{
    PCI_SAVED_CONFIGURATION initialConfig;
    PCI_SLOT_PERSISTENT_INFO persistentSlotInfo;
    USHORT bridgeControl;
    NTSTATUS status;

    PAGED_CODE();

    //
    // See if we have already cached info for this device
    //
    status = PciGetSavedDeviceConfiguration(Device, &initialConfig);
    if (NT_SUCCESS(status)) {

        if (!PciIsSameDevice(Device, &initialConfig.IdData)) {

            status = STATUS_UNSUCCESSFUL;
        }
    }

    if (NT_SUCCESS(status)) {

        //
        // We have a previously stored configuration (meaning that the
        // PDO for this device was deleted but now it's back).  Restore
        // the interrupt line register, which might contain the interrupt
        // routing for the device (halx86) so the value that used to be there
        // is important.
        //
        if (initialConfig.SaveRestore.InterruptLine != SaveRestore->InterruptLine) {

            PciWriteConfigRegister(Device,
                                   common.InterruptLine,
                                   &initialConfig.SaveRestore.InterruptLine
                                   );
        }

        *ConfigSaveRequired = FALSE;

    } else {

        RtlCopyMemory(&initialConfig.IdData,
                      IdData,
                      sizeof(PCI_DEVICE_ID_DATA)
                      );
        RtlCopyMemory(&initialConfig.SaveRestore,
                      SaveRestore,
                      sizeof(PCI_SAVE_RESTORE_DATA)
                      );


        //
        // We can't get the current values of the resources in this routine,
        // since the requirements of the device haven't been computed yet, and
        // these are necessary to compute resources.  We can't move this
        // entire routine after the requirements computation, since the
        // information about whether this device is on the debug path, which
        // is calculated here, is necessary to compute requirements.
        //
        // So don't calculate or use the initialConfig.Resources in this
        // routine.
        //

        *ConfigSaveRequired = TRUE;
    }

    //
    // Next retrieve any persistent information for this slot, such as
    // a PCIe serial number.
    //

    PCI_INIT_SLOT_PERSISTENT_INFO(&persistentSlotInfo);
    status = PciGetSlotPersistentInformation(Device, &persistentSlotInfo);
    if (NT_SUCCESS(status)) {

        //
        // If there is saved info for this specific device,
        // then retrieve it for later use.
        //

        if (((PciSystemWideHackFlags & (PCI_SYS_HACK_DISABLE_EXPRESS_SERIAL_NUMBER_SUPPORT |
                                        PCI_SYS_HACK_DISABLE_EXPRESS_SERIAL_NUMBER_CACHING)) == 0) &&
            (PciIsSameDevice(Device, &persistentSlotInfo.IdData))) {

            Device->InstanceIdSerialNumber = persistentSlotInfo.ExpressSerialNumber;
        }

    } else {

        //
        // If this is a new device, make sure there isn't a cached
        // serial number so that it is properly updated later when the
        // ExpressPort is processed.
        //

        Device->InstanceIdSerialNumber = 0;
    }

    //
    // Now initialize the SavedRegisters in the device extension.  This structure
    // contains the locally modified versions of registers that we configure.  They
    // are actually written out to the hardware in PciRestoreDeviceRegisters, which
    // is called on Start and power up.
    //
    RtlCopyMemory(&Device->SavedRegisters,
                  &initialConfig.SaveRestore,
                  sizeof(PCI_SAVE_RESTORE_DATA)
                  );

    //
    // If the Command value is boot configured but different from the previous
    // incarnation, then set the new value as OR of the previous and current
    // values. This is so that we dont incorrectly skip resource requirements.
    //
    if (((SaveRestore->Command &
         (PCI_ENABLE_MEMORY_SPACE | PCI_ENABLE_IO_SPACE)) != 0) &&
        (Device->SavedRegisters.Command != SaveRestore->Command)) {

        Device->SavedRegisters.Command |= SaveRestore->Command;
        *ConfigSaveRequired = TRUE;
    }

    if ((Device->HeaderType == PCI_BRIDGE_TYPE) ||
        (Device->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) {

        //
        // The bus numbers should not be saved from previous incarnations of
        // this PDO - they should only be saved from the current hardware state.
        // Bus numbering changes could have made the BIOS configuration invalid.
        // Do this only when bus numbers are correctly configured.
        //

        if (Device->BusNumbersConfigured != FALSE) {
            Device->SavedRegisters.bridge.SecondaryBusNumber
                = SaveRestore->bridge.SecondaryBusNumber;

            Device->SavedRegisters.bridge.SubordinateBusNumber
                = SaveRestore->bridge.SubordinateBusNumber;
        }

        //
        // Save the bridge control register.  If this has the VGA bit
        // set, we want to try to set the 16 bit VGA bit.  This might
        // not work (the bridge is old and doesn't support it), so
        // try the write and see.
        //
        bridgeControl = initialConfig.SaveRestore.bridge.BridgeControl;
        if (bridgeControl & PCI_ENABLE_BRIDGE_VGA) {

            if (!(bridgeControl & PCI_ENABLE_BRIDGE_VGA_16BIT)) {

                bridgeControl |= PCI_ENABLE_BRIDGE_VGA_16BIT;

                PciWriteConfigRegister(Device,
                                       bridgeCommon.BridgeControl,
                                       &bridgeControl
                                       );

                PciReadConfigRegister(Device,
                                      bridgeCommon.BridgeControl,
                                      &bridgeControl
                                      );
            }

        } else {

            //
            // If the legacy VGA bit is not set, the 16 bit VGA bit is
            // meaningless.  But shut it off just to be safe.
            //
            if (bridgeControl & PCI_ENABLE_BRIDGE_VGA_16BIT) {

                bridgeControl &= ~PCI_ENABLE_BRIDGE_VGA_16BIT;

                PciWriteConfigRegister(Device,
                                       bridgeCommon.BridgeControl,
                                       &bridgeControl
                                       );
            }
        }

        Device->SavedRegisters.bridge.BridgeControl = bridgeControl;
    }

    //
    // Unconfigured devices are subject to _HPX and _HPP methods.
    //

    if (PCI_UNCONFIGURED_DEVICE(Device, &initialConfig.SaveRestore)) {
        PCI_DEVICE_SET_FLAG(Device, NoInitialConfig);
    }

    //
    // The debug paths are specified by bus/device/function, but the
    // bus numbers are those programmed by the BIOS, so the initialConfig
    // must be used to determine debug path.
    //
    if (PciDeviceOnDebugPath(Device, &initialConfig)) {

        PCI_DEVICE_SET_FLAG(Device, OnDebugPath);

        //
        // If the device is a debug device, then mark it as such.  PNP will check
        // this flag, and will not call AddDevice for this device unless the driver
        // has specifically indicated in its INF that it can coexist with the
        // debugger.
        //

        if (Device->HeaderType == PCI_DEVICE_TYPE) {
            Device->DeviceObject->Flags |= DO_DEVICE_USED_BY_DEBUGGER;
        }
    }

    return STATUS_SUCCESS;
}

BOOLEAN
PciDoNotEnumerateHeadlessSerialDevice(
    __in PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine determines whether the passed in device is being used
    by the headless console mechanism in the kernel. In a headless
    configuration, the kernel directly reads/writes a serial port to implement
    console I/O. However, if the serial port is also enumerated via PCI,
    the device will show up banged out with a code 53, or code 28 if a driver
    was never loaded on it. Either of these may cause the system to fail
    logo testing.

Arguments:

    Device - Provides the device to test.

Return Value:

    TRUE if the device is should not be enumerated, FALSE otherwise.

--*/

{

    SIZE_T Length;
    NTSTATUS Status;

    //
    // Only consider Serial or Modem devices.
    //

    if (Device->HeaderType != PCI_DEVICE_TYPE) {
        return FALSE;
    }

    if (Device->BaseClass != PCI_CLASS_SIMPLE_COMMS_CTLR) {
        return FALSE;
    }

    if ((Device->SubClass != PCI_SUBCLASS_COM_SERIAL) &&
        (Device->SubClass != PCI_SUBCLASS_COM_MODEM)) {

        return FALSE;
    }

    //
    // Cache information about the headless device.
    //

    if (PciHeadlessInfoInitialized == FALSE) {
        Length = sizeof(HEADLESS_RSP_QUERY_PCI_INFO);
        Status = HeadlessDispatch(HeadlessCmdQueryPciInformation,
                                  NULL,
                                  0,
                                  &PciHeadlessInfo,
                                  &Length
                                 );

        if (!NT_SUCCESS(Status)) {
            PciHeadlessInfo.Enabled = FALSE;
        }

        PciHeadlessInfoInitialized = TRUE;
    }

    //
    // Is the headless port enabled, and does it match this device?
    //

    if ((PciHeadlessInfo.Enabled == FALSE) ||
        (PciHeadlessInfo.Enumerate != FALSE) ||
        (PciHeadlessInfo.SegmentNumber != SEGMENT_NUMBER_FOR_BUS(Device->BusNumber)) ||
        (PciHeadlessInfo.BusNumber != LOCAL_BUS_NUMBER(Device->BusNumber)) ||
        (PciHeadlessInfo.DeviceNumber != Device->Slot.u.bits.DeviceNumber) ||
        (PciHeadlessInfo.FunctionNumber != Device->Slot.u.bits.FunctionNumber)) {

        return FALSE;
    }

    return TRUE;
}

BOOLEAN
PciDoNotEnumerate(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    Returns TRUE if the given device should be hidden from enumeration
    for any reason.

Arguments:

    Device - The device to check.

Return Value:

    TRUE if the device should NOT be enumerated.
    FALSE if the device should be enumerated.

--*/
{

    PWATCHDOG_TIMER_ACTION_TABLE Wdat;
    PWATCHDOG_TIMER_RESOURCE_TABLE Wdrt;

    NON_PAGED_CODE();

    //
    // Look for the watchdog timer device. If this is the watchdog timer device
    // then suppress its enumeration.  The WDRT and WDAT tables must both be
    // supported.
    //
    // N.B. The watchdog hardware should not be a PCI device. For legacy
    //      purposes, PCI watchdog devices are supported. If the watchdog
    //      hardware is a PCI device, it is required to be on bus 0 to ensure it
    //      is only affected by system power state changes and not device power
    //      state changes.

    if ((PciWatchdogTable != NULL) && (Device->Parent == Device->Root->Bus)) {
        if (PciWatchdogTable->Signature == WDAT_SIGNATURE) {
            Wdat = (PWATCHDOG_TIMER_ACTION_TABLE)PciWatchdogTable;
            if ((LOCAL_BUS_NUMBER(Device->BusNumber) == Wdat->PciBusNumber) &&
                (SEGMENT_NUMBER_FOR_BUS(Device->BusNumber) == Wdat->PciSegment) &&
                (Device->Slot.u.bits.DeviceNumber == Wdat->PciDeviceNumber) &&
                (Device->Slot.u.bits.FunctionNumber == Wdat->PciFunctionNumber)) {

                return TRUE;
            }

        } else if (PciWatchdogTable->Signature == WDTT_SIGNATURE) {
            Wdrt = (PWATCHDOG_TIMER_RESOURCE_TABLE)PciWatchdogTable;
            if ((LOCAL_BUS_NUMBER(Device->BusNumber) == Wdrt->PciBusNumber) &&
                (SEGMENT_NUMBER_FOR_BUS(Device->BusNumber) == Wdrt->PciSegment) &&
                (Device->VendorID == Wdrt->PciVendorId) &&
                (Device->DeviceID == Wdrt->PciDeviceId) &&
                (Device->Slot.u.bits.DeviceNumber == Wdrt->PciSlotNumber) &&
                (Device->Slot.u.bits.FunctionNumber == Wdrt->PciFunctionNumber)) {

                return TRUE;
            }
        }
    }

    if (Device->HackFlags & PCI_HACK_NO_ENUM_AT_ALL) {

        return TRUE;
    }

    if ((Device->HackFlags & PCI_HACK_DOUBLE_DECKER) &&
        (Device->Slot.u.bits.DeviceNumber >= 16)) {

        //
        // This device seems to look at only the lower 4 bits of
        // its DEVSEL lines, ie it is mirrored in the upper half
        // if the buss's device domain.
        //
        VERIFIER(
            "Device (Ven %04x Dev %04x (d=0x%x, f=0x%x)) is a ghost.\n",
            Device->VendorID,
            Device->DeviceID,
            Device->Slot.u.bits.DeviceNumber,
            Device->Slot.u.bits.FunctionNumber
            );

        return TRUE;
    }

    //
    // This device can sometimes report an invalid class code.  When it does
    // it also reports invalid resources, so suppress it.
    // BUGBUG - probably should just add this to the hack database
    //
    if ((Device->VendorID == 0x8086) &&
        (Device->DeviceID == 0x0008) &&
        (Device->BaseClass == PCI_CLASS_NOT_DEFINED)) {

        return TRUE;
    }

    if (Device->HeaderType > PCI_MAX_CONFIG_TYPE) {

        return TRUE;
    }

    //
    // If this device is in use by the headless system, don't enumerate it.
    //

    if (PciDoNotEnumerateHeadlessSerialDevice(Device) != FALSE) {
        return TRUE;
    }

    if (PciIsNonHotplugBridgeWithNoChildren(Device) != FALSE) {
        return TRUE;
    }

    return FALSE;
}

VOID
PciProcessBus (
    __inout PPCI_BUS Bus
    )

/*++

Routine Description:

    Walk the child devices enumerated by PciScanBus and perform any processing
    that needs to be done once all the children have been enumerated.

Arguments:

    Bus - Supplies a pointer to a PCI bus extension.

Return Value:

    None.

--*/

{

    USHORT BridgeControl;
    PPCI_DEVICE Current;
    PPCI_BRIDGE PciBridge;
    PPCI_DEVICE VgaBridge;

    PAGED_CODE();

    //
    // Match up the newly enumerated devices with the hotplug slot objects.
    //

    PciUpdateHotPlugSlotAfterBusScan(Bus);

    //
    // Determine which bridges need the ISA bit set.  On those that do, update
    // the saved version of the BridgeControl register to reflect this.  This
    // will get written out on Start.
    //

    VgaBridge = NULL;
    PciBridge = Bus->PciBridge;
    if ((PciBridge != NULL) && (PciBridge->Attributes.IsaBitSet != 0)) {

        //
        // If our parent is a bridge with the ISA bit set, then set the ISA bit
        // on all child bridges.
        //

        PciAcquirePassiveLock(&Bus->ChildDeviceLock);
        for (Current = Bus->ChildDevices;
             Current != NULL;
             Current = Current->Sibling) {

            if ((Current->HeaderType == PCI_BRIDGE_TYPE) ||
                (Current->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) {

                Current->SavedRegisters.bridge.BridgeControl |=
                    PCI_ENABLE_BRIDGE_ISA;

            }
        }

        PciReleasePassiveLock(&Bus->ChildDeviceLock);

    } else if ((PciSystemWideHackFlags & PCI_SYS_HACK_IGNORE_ISA_VGA_CONFLICT) == 0) {

        //
        // If there's a bridge with the VGA bit set, all other bridges need the
        // ISA bit set.
        //

        PciAcquirePassiveLock(&Bus->ChildDeviceLock);
        for (Current = Bus->ChildDevices;
             Current != NULL;
             Current = Current->Sibling) {

            if ((Current->HeaderType == PCI_BRIDGE_TYPE) ||
                (Current->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) {

                BridgeControl = Current->SavedRegisters.bridge.BridgeControl;
                if (((BridgeControl & PCI_ENABLE_BRIDGE_VGA) != 0) &&
                    ((BridgeControl & PCI_ENABLE_BRIDGE_VGA_16BIT) == 0)) {

                    VgaBridge = Current;
                    break;
                }
            }
        }

        if (VgaBridge != NULL) {
            for (Current = Bus->ChildDevices;
                 Current != NULL;
                 Current = Current->Sibling) {

                if ((Current->HeaderType == PCI_BRIDGE_TYPE) ||
                    (Current->HeaderType == PCI_CARDBUS_BRIDGE_TYPE)) {

                    if (Current != VgaBridge)  {

                        //
                        // If this device is already started then we had
                        // better have already set the ISA bit
                        //

                        if (Current->DeviceState == PciStarted) {

                            PCI_ASSERT((Current->SavedRegisters.bridge.BridgeControl &
                                        PCI_ENABLE_BRIDGE_ISA) != 0);

                        }

                        Current->SavedRegisters.bridge.BridgeControl |=
                            PCI_ENABLE_BRIDGE_ISA;
                    }
                }
            }
        }

        PciReleasePassiveLock(&Bus->ChildDeviceLock);
    }

    //
    // If this bus, in the past, was scanned and contained no devices, then
    // this represents a hot-plug event.  It's time to consider opening up any
    // bridge windows that might have been closed when the machine booted.
    //

    if (Bus->OnceContainedNoDevices == 1) {

        if (Bus->PciBridge != NULL) {
            PciBridgeReevaluateWindows(Bus->PciBridge);
        }

        PCI_BUS_CLEAR_FLAG(Bus, OnceContainedNoDevices);
    }

    if ((PciBridge != NULL) && (PciBridge->ExpressBridge != NULL)) {
        ExpressBridgeProcessMaxPayloadSize(PciBridge->ExpressBridge);
        ExpressBridgeProcessLinkForPowerManagement(PciBridge->ExpressBridge);
    }

    //
    // Check to see if there are any bridges in need of bus numbers and assign
    // them if we are running on a machine where this is a good idea.
    //

    PciConfigureBusNumbers(Bus);

    //
    // Update the availability of D3 as a power state on this bridge based on
    // the devices on the secondary bus.
    //

    if (PciBridge != NULL) {
        PciBridgeUpdateD3Availability(PciBridge);
    }

    return;
}

NTSTATUS
PciUpdateHotPlugSlotAfterBusScan (
    __inout PPCI_BUS Bus
    )

/*++

Routine Description:

    This routine updates the hot plug slot with results of a bus scan.

Arguments:

    Bus - Supplies a pointer to a PCI bus.

Return Value:

    STATUS_SUCCCESS.

--*/

{

    PPCI_DEVICE CurrentDevice;
    KLOCK_QUEUE_HANDLE LockHandle;

    NON_PAGED_CODE();

    if (PCI_ROOT_BUS(Bus)) {
        return STATUS_SUCCESS;
    }

    //
    // First clear any non-present devices out of the hotplug slot objects.
    //

    PciAcquirePassiveLock(&Bus->ChildDeviceLock);
    for (CurrentDevice = Bus->ChildDevices;
         CurrentDevice != NULL;
         CurrentDevice = CurrentDevice->Sibling) {

        if ((CurrentDevice->HotPlugSlot != NULL) &&
            (CurrentDevice->Present == FALSE)) {

            KeAcquireInStackQueuedSpinLock(
                &CurrentDevice->HotPlugSlot->DeviceLock,
                &LockHandle);

            if (CurrentDevice->HotPlugSlot->Device == CurrentDevice) {
                CurrentDevice->HotPlugSlot->Device = NULL;
            }

            KeReleaseInStackQueuedSpinLock(&LockHandle);
        }
    }

    //
    // Now match up the slots with devices that are present.
    //

    for (CurrentDevice = Bus->ChildDevices;
         CurrentDevice != NULL;
         CurrentDevice = CurrentDevice->Sibling) {

        if ((CurrentDevice->HotPlugSlot != NULL) &&
            (CurrentDevice->Present != FALSE)) {

            KeAcquireInStackQueuedSpinLock(
                &CurrentDevice->HotPlugSlot->DeviceLock,
                &LockHandle);

            if (CurrentDevice->HotPlugSlot->Device == NULL) {
                CurrentDevice->HotPlugSlot->Device = CurrentDevice;
            }

            KeReleaseInStackQueuedSpinLock(&LockHandle);
        }
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);
    return STATUS_SUCCESS;
}

_IRQL_requires_(PASSIVE_LEVEL)
VOID
PciDestroyDevice(
    __inout PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine deletes a PDO, freeing all the memory allocated
    in the device extension along the way.

Arguments:

    Device - The device to destroy.

Return Value:

    VOID

--*/
{

    PEXPRESS_PORT expressPort;
    KLOCK_QUEUE_HANDLE lockHandle;
    PPCI_BUS bus;

    PCI_ASSERT(PCI_DEVICE_EXTENSION(Device));

    PCI_ASSERT(!Device->LegacyDriver);

    bus = Device->Parent;
    PCI_ASSERT(PCI_BUS_EXTENSION(bus));

    TRACE(ENUM_TRACE_LEVEL,
          "destroy PDO (b=0x%x, d=0x%x, f=0x%x)\n",
          bus->SecondaryBusNumber,
          Device->Slot.u.bits.DeviceNumber,
          Device->Slot.u.bits.FunctionNumber
          );

    PciDebugDevicePowerCallbackDeregister(Device);
    switch (Device->InterruptRequirement.Message.Type) {
    case PciMsiX:
        if (Device->InterruptResource.MsiX.EntryMap != NULL) {
            PciFreePool(Device->InterruptResource.MsiX.EntryMap);
            Device->InterruptResource.MsiX.EntryMap = NULL;
        }

        if (Device->InterruptResource.MsiX.ConnectionData != NULL) {
            PciFreePool(Device->InterruptResource.MsiX.ConnectionData);
            Device->InterruptResource.MsiX.ConnectionData = NULL;
        }

        break;

    default:
        break;
    }

    //
    // Clear the device in the hot plug slot.
    //

    if (Device->HotPlugSlot != NULL) {
        KeAcquireInStackQueuedSpinLock(&Device->HotPlugSlot->DeviceLock,
                                       &lockHandle);

        if (Device->HotPlugSlot->Device == Device) {
            Device->HotPlugSlot->Device = NULL;
        }

        KeReleaseInStackQueuedSpinLock(&lockHandle);
    }

    //
    // Clean up the Express Portion of the device if present.
    //

    if (Device->ExpressPort != NULL) {
        expressPort = Device->ExpressPort;

        //
        // If this is a RC Integrated Endpoint or RC Event Collector,
        // then unlink the ExpressPort from the RootComplex.
        //

        if (EXPRESS_RC_INTEGRATED_DEVICE(expressPort)) {
            ExpressRemoveIntegratedPortFromRootComplex(expressPort);
        }

        //
        // Unregister an express port so that it does not receive any Aspm
        // policy change callback.
        //

        ExpressBridgeUnregisterDeviceForAspmCallback(expressPort);

        //
        // For non-integrated express ports, free express links attached to
        // them.
        //

        ExpressDestroyPort(expressPort);
    }

    //
    // Clean up the compatibility port of the device if present
    //

    if (Device->ExpressCompatibilityPort != NULL) {
        ExpressDestroyCompatibilityPort(Device->ExpressCompatibilityPort);
    }

    //
    // Remove this PDO from the Child Pdo List.
    //
    PciRemoveDeviceFromChildList(Device);

    //
    // If this was the last ARI device, disable ARI in the bridge
    //

    if ((bus->ChildDevices == NULL) && Device->AriIsEnabled) {
        ExpressBridgeSetAriEnable(bus->PciBridge->ExpressBridge, FALSE);
    }

    //
    // Drop the reference that we're still holding on the filtered D3cold
    // interface (probably from ACPI.)
    //

    if (Device->D3ColdFilter.InterfaceDereference != NULL) {
        Device->D3ColdFilter.InterfaceDereference(Device->D3ColdFilter.Context);
    }

    //
    // Track Interface reference counts
    //
    if (Device->PciBusInterfaceCount ||
        Device->PciCardBusInterfaceCount ||
        Device->PciDevicePresentInterfaceCount ||
        Device->PciNativeIdeInterfaceCount ||
        Device->PciLocationInterfaceCount) {
        TRACE(PNP_TRACE_LEVEL, "Outstanding interface reference count(s)\n"
                               "          PciBusInterfaceCount : %x\n"
                               "      PciCardBusInterfaceCount : %x\n"
                               "PciDevicePresentInterfaceCount : %x\n"
                               "    PciNativeIdeInterfaceCount : %x\n"
                               "     PciLocationInterfaceCount : %x\n",
                               Device->PciBusInterfaceCount,
                               Device->PciCardBusInterfaceCount,
                               Device->PciDevicePresentInterfaceCount,
                               Device->PciNativeIdeInterfaceCount,
                               Device->PciLocationInterfaceCount);
    }

    //
    // Zap the extension type so we'll trip up if we try to resuse it.
    //
    Device->Signature = 0xdead;

    //
    // And finally,...
    //

    IoDeleteDevice(Device->DeviceObject);

}

BOOLEAN
PciDeviceIsInHigherSlot(
    __in PPCI_DEVICE Device1,
    __in PPCI_DEVICE Device2
    )

/*++

Routine Description:

    This routine determines if one device is in a higher slot than another
    (based on DeviceNumber and FunctionNumber). It assumes that both devices
    are on the same bus.

Arguments:

    Device1 - The device being compared.

    Device2 - The device to compare against.

Return Value:

    TRUE if Device1 is in a higher slot than Device2, FALSE otherwise.

--*/

{
    int Comp1;
    int Comp2;

    Comp1 = (Device1->Slot.u.bits.DeviceNumber << 3) | Device1->Slot.u.bits.FunctionNumber;
    Comp2 = (Device2->Slot.u.bits.DeviceNumber << 3) | Device2->Slot.u.bits.FunctionNumber;
    return (Comp1 >= Comp2);
}

VOID
PciAddDeviceToChildList(
    __in PPCI_DEVICE Device,
    __inout PPCI_BUS Parent
    )
/*++

Routine Description:

    This routine adds a device extension to the list of
    child devices for a given bus.

    N.B. Assumes caller within PciAcquirePassiveLock(&Parent->ChildDeviceLock);

Arguments:

    Device - the device to add.

    Parent - the parent whose list this device should be added to.

Return Value:

    VOID

--*/
{
    PPCI_DEVICE *listHead, *listTail;
    PPCI_DEVICE Prev;

    PAGED_CODE();

    listHead = &Parent->ChildDevices;
    listTail = &Parent->ChildDeviceTail;
    Device->Sibling = NULL;

    if (*listHead) {

        PCI_ASSERT(*listTail);

        //
        // Insert the new device in the list in slot order.
        // D3Cold code (CurrentDevice) in PciBusGetNextSlotNumber implicitly
        // assumes that the child devices on the bus maintain slot order,
        // and it simplifies debugging if all the devices maintain order.
        // Most of the time the new device sits at the end of the list,
        // so optimize that special case first. Otherwise traverse the list
        // starting at the front to find the insertion point.
        //

        if (PciDeviceIsInHigherSlot(Device, *listTail)) {
            (*listTail)->Sibling = Device;
            *listTail = Device;

        } else if (PciDeviceIsInHigherSlot(*listHead, Device)) {
            Device->Sibling = *listHead;
            *listHead = Device;

        } else {
            Prev = *listHead;
            while ((Prev->Sibling != NULL) &&
                   PciDeviceIsInHigherSlot(Device, Prev->Sibling)) {

                 Prev = Prev->Sibling;
            }

            Device->Sibling = Prev->Sibling;
            Prev->Sibling = Device;
            if (*listTail == Prev) {
                *listTail = Device;
            }
        }
    } else {

        *listHead = *listTail = Device;
    }
}

VOID
PciRemoveDeviceFromChildList(
    __in PPCI_DEVICE Device
    )
/*++

Routine Description:

    This routine removes a device extension from the list of
    child devices for a given bus.

Arguments:

    Device - the device to remove.

Return Value:

    VOID

--*/
{
    PPCI_DEVICE *listHead, *listTail;
    PPCI_DEVICE current, previous;

    PAGED_CODE();

    PCI_ASSERT(Device->Parent);

    listHead = &Device->Parent->ChildDevices;
    listTail = &Device->Parent->ChildDeviceTail;
    PCI_ASSERT(*listHead);

    if (*listHead) {

        if (*listHead == Device) {

            *listHead = Device->Sibling;
            return;

        } else {

            current = *listHead;
            previous = NULL;
            while (current) {

                if (current == Device) {

                    PCI_ASSERT(previous != NULL);
                    previous->Sibling = Device->Sibling;

                    if (*listTail == Device) {
                        *listTail = previous;
                    }
                    return;
                }
                previous = current;
                current = current->Sibling;
            }
        }
    }
}

BOOLEAN
PciBusGetNextSlotNumber(
    __in PPCI_BUS Bus,
    __in PPCI_DEVICE PreviousDevice,
    __in BOOLEAN NewScan,
    __inout PPCI_SLOT_NUMBER Slot
    )

/*++

Routine Description:

    Calculates the next slot number to scan given the last device found
    and its slot number

    Note: This routine uses the ChildDevices list in the PCI_BUS structure
    passed in. If there is a possibility of the list changing while this routine
    is being executed, then the caller must make use of the ChildDeviceLock in
    the PCI_BUS structure.

Arguments:

    Bus - Pointer to the bus being scanned

    PreviousDevice - Pointer to the last device found or NULL if previous slot
        was empty

    NewScan - A boolean value that indicates whether the routine was called to
        initiate a fresh scan of the bus.

    Slot - Pointer to variable containing the slot number of the last device
        scanned.  Will be updated to the next slot to scan.

Return Value:

    TRUE if Slot was updated and should be scanned

--*/

{

    ULONG DeviceNumber;
    ULONG FunctionNumber;

    NON_PAGED_CODE();

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_VERBOSE,
                DBG_PNP,
                "PreviousDevice: %p - %x:%x",
                PreviousDevice,
                Slot->u.bits.DeviceNumber,
                Slot->u.bits.FunctionNumber);

    if (NewScan != FALSE) {

        Slot->u.AsULONG = 0;
        return TRUE;
    }

    if ((PreviousDevice != NULL) && (PreviousDevice->AriIsEnabled != FALSE)) {

        //
        // ARI devices are scanned by following a linked list
        // in the ARI capability register
        //

        FunctionNumber = PciNextAriFunctionNumber(PreviousDevice);
        if (FunctionNumber == 0) {
            return FALSE;
        }

        TraceEvents(Bus->BlackBoxHandle,
                    TRACE_LEVEL_VERBOSE,
                    DBG_PNP,
                    "Getting next function from ARI: %x",
                    FunctionNumber);

        SET_ARI_SLOT(Slot, FunctionNumber);
        return TRUE;
    }

    FunctionNumber = Slot->u.bits.FunctionNumber;
    if (PreviousDevice == NULL) {

        //
        // The last slot was empty.  If function 0 is empty they the
        // rest of the functions are guaranteed to be empty too so
        // carry on with the next device.
        //

        if (FunctionNumber != 0) {

            //
            // Try the next function - PCI 2.2 allows holes in the
            // populated functions (eg Function 0 and 2 but not 1)
            //

            FunctionNumber = (FunctionNumber + 1) & 0x7;

            TraceEvents(Bus->BlackBoxHandle,
                        TRACE_LEVEL_VERBOSE,
                        DBG_PNP,
                        "Getting discontinguous function from multi-function device: %x",
                        FunctionNumber);
        }
    } else {

        //
        // It's a multi-function device if already scanned non-zero
        // functions, or if this is function zero and the header type
        // says so, or if the hack flags say so.
        //

        if ((FunctionNumber != 0) ||
            (PreviousDevice->MultiFunction != FALSE) ||
            (PreviousDevice->HackFlags & PCI_HACK_MULTIFUNCTION)) {

            FunctionNumber = (FunctionNumber + 1) & 0x7;

            TraceEvents(Bus->BlackBoxHandle,
                        TRACE_LEVEL_VERBOSE,
                        DBG_PNP,
                        "Getting subsequent function from multi-function device: %x",
                        FunctionNumber);
        }
    }

    if (FunctionNumber == 0) {

        //
        // Update device number
        //

        if (Bus->HackFlags & PCI_HACK_ONE_CHILD) {
            return FALSE;
        }

        DeviceNumber = Slot->u.bits.DeviceNumber + 1;
        if (DeviceNumber == PCI_MAX_DEVICES) {
            return FALSE;
        }

        Slot->u.bits.DeviceNumber = DeviceNumber;
    }

    Slot->u.bits.FunctionNumber = FunctionNumber;
    return TRUE;
}


VOID
PciSurpriseRemoveAndReenumerateSelf(
    __in PPCI_DEVICE Device
    )

/*++

Routine Description

    This function will initiate the process of reporting a device as being
    surprise-removed, and later reenumerate the device to bring it back. The
    net effect is to tear down and rebuild the device stack. The function
    will also attempt to reset the device during the process.

Arguments

    Device - Provdes a pointer to the PCI device to be removed and reenumerated.

Return Value

    None

--*/

{

    //
    // Set a flag to force the device to appear missing on the next reenumerate.
    // The FDO should not attempt to access the device from this point on.
    //

    PCI_DEVICE_SET_FLAG(Device, SurpriseRemoveAndReenumerateSelf);

    //
    // Force a reenumeration of the parent. This will cause the device to go
    // away.
    //

    IoInvalidateDeviceRelations(Device->Parent->PhysicalDeviceObject,
                                BusRelations);

    //
    // Done for now. The next steps are:
    // As long as the SurpriseRemoveAndReenumerateSelf flag is set, PciScanBus
    // will report the device missing.
    // During surprise remove the SurpriseRemoveAndReenumerateSelf flag will be
    // cleared and IoInvalidateDeviceRelations will be called again.
    // The subsequent PciScanBus will reenumerate a new instance of the device.
    //

    return;
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciAttemptDeviceReset(
    _In_ PPCI_DEVICE Device
    )

/*++

Routine Description

    This function will attempt to reset a PCI device using a number of different
    techniques.

    Note, this function has the implicit assumption that it will be called
    either by the power policy owner or with the PnP tree lock held, such that
    no device power IRPs are in the stack while this runs.

    Another note. If a new FLR mechanism is added, then
    PciIsFunctionLevelResetSupported() should be updated accordingly.

Arguments

    Device - Supplies a pointer to the PCI device to be reset.

Return Value

    NTSTATUS code.

--*/

{

    PCI_ADVANCED_FEATURES_CAPABILITY AFCapability;
    PEXPRESS_PORT ExpressPort;
    PPCI_BUS      Bus;
    PPCI_BRIDGE   Bridge;
    BOOLEAN       RemovedPower;
    NTSTATUS      Status;
    ULONG         Pass;
    PCI_EXPRESS_DEVICE_CONTROL_REGISTER DeviceControl;
    PAGED_CODE();

    ExpressPort = Device->ExpressPort;
    Bus = Device->Parent;
    Status = STATUS_UNSUCCESSFUL;

    TraceEvents(Bus->BlackBoxHandle,
                TRACE_LEVEL_INFORMATION,
                DBG_INIT,
                "Attempting to reset a device (%x:%x)",
                Device->Slot.u.bits.DeviceNumber,
                Device->Slot.u.bits.FunctionNumber);

    //
    // Save Express Port stuff.
    //

    if (ExpressPort != NULL) {
        ExpressSavePortRegisters(Device->ExpressPort);
    } else if (Device->ExpressCompatibilityPort != NULL) {
        ExpressCompatibilitySavePortRegisters(
           Device->ExpressCompatibilityPort);
    }

    //
    // Disconnect the device, which has some necessary side effects, as well
    // as guaranteeing that outstanding transactions will retire.
    //

    if (PciCanDisableDevice(Device) == KeepDeviceActive) {
        TraceEvents(Bus->BlackBoxHandle,
                    TRACE_LEVEL_WARNING,
                    DBG_INIT,
                    "Attempting to reset a device (%x:%x) that shouldn't be disconnected",
                    Device->Slot.u.bits.DeviceNumber,
                    Device->Slot.u.bits.FunctionNumber);
    }

    PciDisconnectDevice(Device);

    //
    // Try to reset through Express FLR.
    //

    if ((ExpressPort != NULL) &&
        (ExpressPort->DeviceCapabilities.FunctionLevelResetCapability)) {

        TraceEvents(Bus->BlackBoxHandle,
                    TRACE_LEVEL_INFORMATION,
                    DBG_INIT,
                    "Using Express FLR");

        //
        // Wait for pending transactions to retire.
        //

        ExpressWaitForPendingTransactions(ExpressPort);

        //
        // Set the FLR bit to reset the function.
        // but do not set the bit in the Expressport structure
        // if we set the bit in the structure, we will trigger another
        // FLR when ExpressRestorePortRegisters() is called in cleanup
        //

        DeviceControl = ExpressPort->DeviceControl;
        DeviceControl.InitiateFunctionLevelReset = 1;
        PciWriteExpressRegister(ExpressPort,
                                DeviceControl,
                                &DeviceControl);

        PciDevice_Delay(Device, FLRResetTime);
        Status = STATUS_SUCCESS;
        goto Cleanup;
    }

    //
    // Try to reset through Conventional FLR.
    //

    if (Device->AdvancedFeatures != 0) {

        TraceEvents(Bus->BlackBoxHandle,
                    TRACE_LEVEL_INFORMATION,
                    DBG_INIT,
                    "Using Conventional FLR");

        PciReadDeviceConfig(Device,
                            &AFCapability,
                            Device->AdvancedFeatures,
                            sizeof(AFCapability));

        if (AFCapability.Capabilities.FunctionLevelResetSupported != 0) {

            //
            // If the transactions pending bit is implemented, wait until all
            // transactions are completed or there is a timeout.
            // This code uses the same heuristics as
            // ExpressWaitForPendingTransactions.
            //

            if (AFCapability.Capabilities.TransactionsPendingSupported != 0) {
                for (Pass = 1; Pass < 10; Pass += 1) {
                    if (AFCapability.Status.TransactionsPending == 0) {
                        break;
                    }

                    PciDevice_Delay(Device, ExpressTransactionTime);
                    PciReadDeviceConfig(
                        Device,
                        &AFCapability.Status,
                        Device->AdvancedFeatures +
                        FIELD_OFFSET(PCI_ADVANCED_FEATURES_CAPABILITY, Status),
                        sizeof(AFCapability.Status));
                }
            }

            //
            // Now trigger Function Level Reset on the device.
            //

            AFCapability.Control.InitiateFunctionLevelReset = 1;
            PciWriteDeviceConfig(
                Device,
                &AFCapability.Control,
                Device->AdvancedFeatures +
                FIELD_OFFSET(PCI_ADVANCED_FEATURES_CAPABILITY, Control),
                sizeof(AFCapability.Control));

            PciDevice_Delay(Device, ResetTime);
            Status = STATUS_SUCCESS;
            goto Cleanup;
        }
    }

    //
    // Try to reset through the upstream secondary bus reset, if the device
    // is a single-function device sitting on its own bus (as many PCIe
    // devices are.
    //

    if ((Device->Slot.u.AsULONG == 0) &&
        (!PCI_ROOT_BUS(Bus)) &&
        (Device->MultiFunction == 0)) {

        TraceEvents(Bus->BlackBoxHandle,
                    TRACE_LEVEL_INFORMATION,
                    DBG_INIT,
                    "Using Secondary Bus Reset");

        if (ExpressPort != NULL) {
            if (ExpressPort->AriCapability != 0) {

                PCI_EXPRESS_ARI_CAPABILITY_REGISTER AriCapabilities = {0};

                PciReadAriRegister(ExpressPort, Capability, &AriCapabilities);

                if (AriCapabilities.NextFunctionNumber != 0) {

                    //
                    // An ARI "next function number" implies the device is
                    // not alone on the bus.
                    //

                    Status = STATUS_UNSUCCESSFUL;
                    goto Cleanup;
                }
            }
        }

        //
        // If the device is in D0, move it to D3hot.
        //

        RemovedPower = FALSE;
        if (Device->LogicalPowerState == PowerDeviceD0) {

            PciSetHardwarePowerStatePassive(Device, PowerDeviceD3);
            RemovedPower = TRUE;
        }

        Bridge = Bus->PciBridge;
        PciBridgeAssertSecondaryReset(Bridge);
        PciBridgeDeassertSecondaryReset(Bridge);

        //
        // Add another half-second here for good measure.  Observation tells us
        // that some cards (complicated ones like GPUs) need a while to settle.
        // And since this isn't a performance critical path, this seems pretty
        // harmless.
        //

        PciDevice_Delay(Device, SecondaryBusResetTime);

        if (RemovedPower != FALSE) {

            PciSetHardwarePowerStatePassive(Device, PowerDeviceD0);
            PciDevice_Delay(Device, D3hotToD0Time);
        }

        Status = STATUS_SUCCESS;
    }

Cleanup:

    Status = PciUpdateHardware(Device, (PVOID)TRUE);

    //
    // Restore Express Port stuff.
    //

    if (ExpressPort != NULL) {
        ExpressRestorePortRegisters(Device->ExpressPort);
    } else if (Device->ExpressCompatibilityPort != NULL) {
        ExpressCompatibilityRestorePortRegisters(
           Device->ExpressCompatibilityPort);
    }

    //
    // Wait some more to be sure that the device has settled.
    //

    PciDevice_Delay(Device, ResetTime);

    return Status;
}

VOID
PciFixupBridgeBusNumbers(
    __in PPCI_BUS Bus
    )

/*++

Routine Description:

    NEC programs the bus number in their _DCK method, unfortunately we
    have already done it!  So detect someone else reprogramming the bus
    number and restore the correct one!

Arguments:

    Bus - Our extension for the PCI bus functional device object.

Return Value:

    None.

--*/

{

    UCHAR localSecondaryOnParent;
    UCHAR localSubordinate;
    UCHAR localSecondary;

    PAGED_CODE();

    if (Bus->PciBridge) {

        PciReadBridgeConfigRegister(Bus->PciBridge,
                                    bridgeCommon.SecondaryBus,
                                    &localSecondary);

        if (localSecondary != LOCAL_BUS_NUMBER(Bus->SecondaryBusNumber)) {

            TraceEvents(Bus->BlackBoxHandle,
                        TRACE_LEVEL_WARNING,
                        DBG_PNP,
                        "Bus numbers have been changed!  Restoring originals."
                        "Local: 0x%02x, Register: 0x%02x.",
                        localSecondary, LOCAL_BUS_NUMBER(Bus->SecondaryBusNumber));

            localSecondaryOnParent =
                LOCAL_BUS_NUMBER(Bus->ParentBus->SecondaryBusNumber);

            PciWriteBridgeConfigRegister(Bus->PciBridge,
                                         bridgeCommon.PrimaryBus,
                                         &localSecondaryOnParent
                                         );

            localSecondary = LOCAL_BUS_NUMBER(Bus->SecondaryBusNumber);
            PciWriteBridgeConfigRegister(Bus->PciBridge,
                                         bridgeCommon.SecondaryBus,
                                         &localSecondary
                                         );

            localSubordinate = LOCAL_BUS_NUMBER(Bus->SubordinateBusNumber);
            PciWriteBridgeConfigRegister(Bus->PciBridge,
                                         bridgeCommon.SubordinateBus,
                                         &localSubordinate
                                         );

        }
    }
}

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciIsFunctionLevelResetSupported(
    _In_ PPCI_DEVICE Device
    )

/*++

Routine Description

    This function will return whether function-level reset (FLR) is supported
    for the given device or not.

Arguments

    Device - Supplies a pointer to the PCI device to be queried.

Return Value

    STATUS_SUCCESS if FLR is supported for the device. STATUS_NOT_SUPPORTED
    otherwise.

--*/

{

    PCI_ADVANCED_FEATURES_CAPABILITY AFCapability;
    PCI_EXPRESS_ARI_CAPABILITY_REGISTER AriCapabilities = {0};
    PPCI_BUS Bus;
    PEXPRESS_PORT ExpressPort;
    NTSTATUS Status;

    PAGED_CODE();

    //
    // Check whether reset is supported through Express FLR.
    //

    Bus = Device->Parent;
    ExpressPort = Device->ExpressPort;
    Status = STATUS_NOT_SUPPORTED;
    if ((ExpressPort != NULL) &&
        (ExpressPort->DeviceCapabilities.FunctionLevelResetCapability)) {

        Status = STATUS_SUCCESS;
        goto IsFunctionLevelResetSupportedEnd;
    }

    //
    // Check whether reset is supported through Conventional FLR.
    //

    if (Device->AdvancedFeatures != 0) {
        PciReadDeviceConfig(Device,
                            &AFCapability,
                            Device->AdvancedFeatures,
                            sizeof(AFCapability));

        if (AFCapability.Capabilities.FunctionLevelResetSupported != 0) {
            Status = STATUS_SUCCESS;
            goto IsFunctionLevelResetSupportedEnd;
        }
    }

    //
    // Check whether reset is supported through the upstream secondary bus
    // reset.
    //

    if ((ExpressPort != NULL) &&
        (ExpressPort->AriCapability != 0) &&
        (Device->Slot.u.AsULONG == 0) &&
        (!PCI_ROOT_BUS(Bus)) &&
        (Device->MultiFunction == 0)) {

        PciReadAriRegister(ExpressPort, Capability, &AriCapabilities);
        if (AriCapabilities.NextFunctionNumber == 0) {
            Status = STATUS_SUCCESS;
            goto IsFunctionLevelResetSupportedEnd;
        }
    }

IsFunctionLevelResetSupportedEnd:
    return Status;
}

_IRQL_requires_max_(PASSIVE_LEVEL)
ULONG
PciQuerySupportedResetTypes(
    _In_ PPCI_DEVICE Device
    )

/*++

Routine Description:

    This routine queries the set of reset types supported for the given device.

Arguments:

    Device - Supplies pointer to PCI_DEVICE to be queried.

Return Value:

    ULONG (bitmask) of the supported reset types.

--*/

{

    IO_STACK_LOCATION IrpStack;
    DEVICE_RESET_INTERFACE_STANDARD ResetInterface;
    USHORT Size;
    NTSTATUS Status;
    ULONG SupportedResetTypes;

    PAGED_CODE();

    PCI_ASSERT(PCI_DEVICE_EXTENSION(Device));

    SupportedResetTypes = 0;
    Status = PciIsFunctionLevelResetSupported(Device);
    if (NT_SUCCESS(Status)) {
        SupportedResetTypes |= (1 << FunctionLevelDeviceReset);
    }

    //
    // PLDR is suppported on the device if the parent supports it.
    // Note PLDR requests are only forwarded one level up to the parent
    // port/bridge (to avoid PLDR going up to the root).
    //

    if (Device->HeaderType == PCI_DEVICE_TYPE) {
        Size = sizeof(DEVICE_RESET_INTERFACE_STANDARD);
        RtlZeroMemory(&ResetInterface, Size);
        IrpStack.MajorFunction = IRP_MJ_PNP;
        IrpStack.MinorFunction = IRP_MN_QUERY_INTERFACE;
        IrpStack.Parameters.QueryInterface.InterfaceType =
            (LPGUID)&GUID_DEVICE_RESET_INTERFACE_STANDARD;

        IrpStack.Parameters.QueryInterface.Version =
            DEVICE_RESET_INTERFACE_VERSION;

        IrpStack.Parameters.QueryInterface.Size = Size;
        IrpStack.Parameters.QueryInterface.Interface =
            (PINTERFACE)&ResetInterface;

        IrpStack.Parameters.QueryInterface.InterfaceSpecificData = NULL;
        Status = PciSendPnpIrp(Device->Parent->DeviceObject, &IrpStack, NULL);
        if (NT_SUCCESS(Status)) {
            if (ResetInterface.DeviceReset != NULL) {
                ResetInterface.SupportedResetTypes &=
                    (1 << PlatformLevelDeviceReset);

                SupportedResetTypes |= ResetInterface.SupportedResetTypes;
            }

            if (ResetInterface.InterfaceDereference != NULL) {
                ResetInterface.InterfaceDereference(ResetInterface.Context);
            }
        }
    }

    return SupportedResetTypes;
}

