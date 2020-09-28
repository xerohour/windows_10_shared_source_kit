e match.
    //
    if (flags & PCI_USE_PROGIF) {
        if (!(flags & PCI_USE_CLASS_SUBCLASS)) {
            ERROR("PciDevicePresentInterface_DevicePresentEx: Invalid parameter supplied - PROGIF only valid on class code match.\n");
            return FALSE;
        }
    }

    //
    // Okay, validation complete.  Do the search.
    //
    PciAcquirePassiveLock(&PciGlobalLock);

    if (flags & (PCI_USE_LOCAL_BUS | PCI_USE_LOCAL_DEVICE)) {

        //
        // Limit the search to the same bus as the device that requested
        // the search.  This requires the device requesting the search.
        //
        if (Device == NULL) {
            WARN("PciDevicePresentInterface_DevicePresentEx: device is NULL.\n");
            goto cleanup;
        }

        found = PciDevicePresentOnBus(Device->Parent, Device, Parameters);

    } else {

        //
        // We have not been told to limit the search to
        // the bus on which a particular device lives.
        // Do a global search, iterating over all the buses.
        //
        for (segment = PciSegmentList; segment != NULL; segment = segment->Next) {
            bus = segment->PciRootBusList;
            while (bus) {

                found = PciDevicePresentOnBus(bus,
                                            NULL,
                                            Parameters
                                            );
                if (found) {
                    goto cleanup;
                }

                bus = PciNextBusInSegment(bus);
            }
        }
    }

cleanup:

    PciReleasePassiveLock(&PciGlobalLock);

    return found;

}

BOOLEAN
PciDevicePresentOnBus(
    __inout PPCI_BUS Bus,
    __in_opt PPCI_DEVICE Device,
    __in PPCI_DEVICE_PRESENCE_PARAMETERS Parameters
    )
/*++

Routine Description:

    This routine searches the PCI device tree for a given device
    on the bus represented by the given FdoExtension.

Arguments:

    Bus - A pointer to the device extension of a PCI FDO.
        This represents the bus to be searched for the given device.

    Device - A pointer to the device extension of the PCI PDO that requested
        the search.  Some device searches are limited to the same bus/device number
        as the requesting device, and this is used to get those numbers.

    Parameters - The parameters of the search.

    Flags - A bitfield indicating which fields of the Parameters
        structure to use for the search.

Return Value:

    TRUE if the requested device is found.
    FALSE if it is not.

--*/
{
    PPCI_DEVICE current;
    BOOLEAN found = FALSE;
    ULONG flags = Parameters->Flags;

    PAGED_CODE();

    found = FALSE;

    PciAcquirePassiveLock(&Bus->ChildDeviceLock);

    for (current = Bus->ChildDevices; current != NULL; current = current->Sibling) {

        //
        // If we're limiting the search to devices with the same
        // device number as the requesting device, make sure this
        // current PDO qualifies.
        //
        if (ARGUMENT_PRESENT(Device) && (flags & PCI_USE_LOCAL_DEVICE)) {

            if (Device->Slot.u.bits.DeviceNumber !=
                current->Slot.u.bits.DeviceNumber) {

                continue;
            }
        }

        if (flags & PCI_USE_VENDEV_IDS) {

            if ((current->VendorID != Parameters->VendorID) ||
                (current->DeviceID != Parameters->DeviceID)) {

                continue;
            }

            if ((flags & PCI_USE_SUBSYSTEM_IDS) &&
                ((current->SubVendorID != Parameters->SubVendorID) ||
                 (current->SubSystemID != Parameters->SubSystemID))) {

                continue;
            }

            if ((flags & PCI_USE_REVISION) &&
                (current->RevisionID != Parameters->RevisionID)) {

                continue;
            }
        }

        if (flags & PCI_USE_CLASS_SUBCLASS) {

            if ((current->BaseClass != Parameters->BaseClass) ||
                (current->SubClass != Parameters->SubClass)) {

                continue;
            }

            if ((flags & PCI_USE_PROGIF) &&
                (current->ProgIf != Parameters->ProgIf)) {

                continue;
            }
        }

        found = TRUE;
        break;
    }

    PciReleasePassiveLock(&Bus->ChildDeviceLock);

    return found;
}

// --------------------------------------------------------------------- Cardbus Interface

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciCardbusInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    This routine constructs a PCI_CARDBUS_PRIVATE_INTERFACE.

Arguments:

    DeviceExtension - An extension pointer.

    InterfaceSpecificData - Unused.

    Version - Interface version.

    Size - Size of the PCI_CARDBUS_INTERFACE_PRIVATE interface object.

    InterfaceReturn - The interface object pointer to be filled in.

    Filtered - Not supported for this interface.

Return Value:

    Returns NTSTATUS.

--*/
{
    PPCI_CARDBUS_INTERFACE_PRIVATE interface;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(InterfaceSpecificData);
    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    //
    // Make sure that no one is asking for the old, larger interface
    //
    if (Size > sizeof(PCI_CARDBUS_INTERFACE_PRIVATE)) {
        PCI_ASSERT(Size == sizeof(PCI_CARDBUS_INTERFACE_PRIVATE));
        return STATUS_NOT_SUPPORTED;
    }

    if (Size < sizeof(PCI_CARDBUS_INTERFACE_PRIVATE)) {
        return STATUS_UNSUCCESSFUL;
    }

    interface = (PPCI_CARDBUS_INTERFACE_PRIVATE)InterfaceReturn;

    //
    // Standard interface stuff
    //
    interface->Size = sizeof(PCI_CARDBUS_INTERFACE_PRIVATE);
    interface->Version = PCI_CB_INTRF_VERSION;
    interface->Context = DeviceExtension;
    interface->InterfaceReference = PciCardbusInterface_Reference;
    interface->InterfaceDereference = PciCardbusInterface_Dereference;

    //
    // PCI-CardBus private
    //
    interface->DriverObject = PciDriverObject;
    interface->GetLocation  = PciCardbusInterface_GetLocation;

    return STATUS_SUCCESS;
}

VOID
PciCardbusInterface_Reference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine increments outstanding reference count for this interface

Arguments:

    PCI_DEVICE

Return Value:

    None

--*/
{
    PPCI_DEVICE device = (PPCI_DEVICE)Context;

    InterlockedIncrement(&device->PciCardBusInterfaceCount);

    return;
}

VOID
PciCardbusInterface_Dereference(
    __inout PVOID Context
    )
/*++

Routine Description:

    This routine decrements outstanding reference count for this interface

Arguments:

    PCI_DEVICE

Return Value:

    None

--*/
{
    PPCI_DEVICE device = (PPCI_DEVICE)Context;

    InterlockedDecrement(&device->PciCardBusInterfaceCount);

    return;
}

NTSTATUS
PciCardbusInterface_GetLocation(
    __in PDEVICE_OBJECT Pdo,
    __out PULONG Bus,
    __out PUCHAR DeviceNumber,
    __out PUCHAR FunctionNumber,
    __out BOOLEAN *OnDebugPath
    )
/*++

Routine Description:

    This routine provides location information about the given PDO.
    It provides the bus/device/function of the PDO for legacy purposes,
    even though this information is available through IoGetDeviceProperty.
    It also indicates whether the device is on the debugging path.  This
    information is not available through any other means.

Arguments:

    Pdo - The device object to get the location information of.

    Bus - On success, will be filled in with the segment/bus
        number of the device.

    DeviceNumber - will be filled in with the device number of the device.

    FunctinNumber - will be filled in with the function number of the device.

    DebugPath - will be filled in with TRUE if the device is on the debug
        path, or FALSE otherwise.

Return Value:

    NT Status code.

--*/
{
    PPCI_DEVICE device;

    PAGED_CODE();

    PCI_ASSERT(Bus);
    PCI_ASSERT(DeviceNumber);
    PCI_ASSERT(FunctionNumber);
    PCI_ASSERT(OnDebugPath);

    device = (PPCI_DEVICE)Pdo->DeviceExtension;

    //
    // Verify that it is actually a PDO.
    //
    if ((device == NULL) ||
        (device->Signature != PCI_DEVICE_SIGNATURE)) {
        return STATUS_NOT_FOUND;
    }

    *Bus            = device->Parent->SecondaryBusNumber;
    *DeviceNumber   = (UCHAR)device->Slot.u.bits.DeviceNumber;
    *FunctionNumber = (UCHAR)device->Slot.u.bits.FunctionNumber;
    *OnDebugPath    = (BOOLEAN)(device->OnDebugPath);

    return STATUS_SUCCESS;
}

//------------------------------------------------------------------- Translator Interface

_IRQL_requires_(PASSIVE_LEVEL)
NTSTATUS
PciTranslatorInterface_Constructor(
    _In_ PVOID DeviceExtension,
    _Inout_ PVOID InterfaceSpecificData,
    _In_ USHORT Version,
    _In_ USHORT Size,
    _Out_writes_bytes_(Size) PINTERFACE InterfaceReturn,
    _In_ BOOLEAN Filtered
    )
/*++

Routine Description:

    This routine fills in a TRANSLATOR_INTERFACE.  PCI does not
    have translators itself.  The HAL does, but the HAL does not
    have device objects in the correct place in the tree to
    translate the correct resources.  So this routine gets the
    HAL translator that is appropriate for this device object.

Arguments:

    DeviceExtension - An extension pointer.

    InterfaceSpecificData - The CmResourceType of the translator request.

    Version - Interface version.

    Size - Size of the PCI_CARDBUS_INTERFACE_PRIVATE interface object.

    InterfaceReturn - The interface object pointer to be filled in.

    Filtered - Not supported for this interface.

Return Value:

    Returns NTSTATUS.

--*/
{
    ULONG secondaryBusNumber, parentBusNumber;
    INTERFACE_TYPE parentInterface;
    PPCI_BUS bus;

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Version);
    UNREFERENCED_PARAMETER(Filtered);

    if (Size < sizeof(TRANSLATOR_INTERFACE)) {
        return STATUS_UNSUCCESSFUL;
    }
    //
    // We only translate interrupts, is that what they want?
    //
    if ((CM_RESOURCE_TYPE)(ULONG_PTR)InterfaceSpecificData != CmResourceTypeInterrupt) {

        TRACE(ARBITER_TRACE_LEVEL,
              "IRQ trans constructor doesn't like %p in InterfaceSpecificData\n",
              InterfaceSpecificData
              );

        return STATUS_INVALID_PARAMETER_3;
    }

    bus = (PPCI_BUS)DeviceExtension;
    PCI_ASSERT(PCI_BUS_EXTENSION(bus));

    //
    // Give the HAL a shot at providing this translator.
    //
    if (PCI_ROOT_BUS(bus)) {

        parentInterface = Internal;
        secondaryBusNumber = bus->SecondaryBusNumber;
        parentBusNumber = 0;

        TRACE(ARBITER_TRACE_LEVEL, "      Is roo