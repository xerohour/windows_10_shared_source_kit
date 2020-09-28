else {

        interface->Size = DEVICE_PRESENT_INTERFACE_MINSIZE;
    }

    return STATUS_SUCCESS;
}

VOID
PciDevicePresentInterface_Reference(
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

    InterlockedIncrement(&device->PciDevicePresentInterfaceCount);

    return;
}

VOID
PciDevicePresentInterface_Dereference(
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

    InterlockedDecrement(&device->PciDevicePresentInterfaceCount);

    return;
}

BOOLEAN
PciDevicePresentInterface_DevicePresent(
    __in USHORT VendorID,
    __in USHORT DeviceID,
    __in UCHAR RevisionID,
    __in USHORT SubVendorID,
    __in USHORT SubSystemID,
    __in ULONG Flags
    )
/*++

Routine Description:

    This routine searches the PCI device tree to see if the specific device
    is present in the system.  Not devices that are explicitly not enumerated
    (such as PIIX4 power management function) are considered absent.

Arguments:

    VendorID - Required VendorID of the device

    DeviceID - Required DeviceID of the device

    RevisionID - Optional Revision ID

    SubVendorID - Optional Subsystem Vendor ID

    SubSystemID - Optional Subsystem ID

    Flags - Bitfield which indicates if Revision and Sub* ID's should be used:

        PCI_USE_SUBSYSTEM_IDS, PCI_USE_REVISION_ID are valid all other bits
        should be 0


Return Value:

    TRUE if the device is present in the system, FALSE otherwise.

--*/
{
    PCI_DEVICE_PRESENCE_PARAMETERS parameters;

    PAGED_CODE();

    parameters.Size = sizeof(PCI_DEVICE_PRESENCE_PARAMETERS);
    parameters.VendorID = VendorID;
    parameters.DeviceID = DeviceID;
    parameters.RevisionID = RevisionID;
    parameters.SubVendorID = SubVendorID;
    parameters.SubSystemID = SubSystemID;

    //
    // Clear out flags that this version of the interface didn't use,
    //
    parameters.Flags = Flags & (PCI_USE_SUBSYSTEM_IDS | PCI_USE_REVISION);

    //
    // This original version of the interface required vendor/device ID
    // matching.  The new version doesn't, so set the flag indicating
    // that we do in fact want to do a vendor/device ID match.
    //
    parameters.Flags |= PCI_USE_VENDEV_IDS;

    return PciCheckDevicePresence(NULL, &parameters);
}

BOOLEAN
PciDevicePresentInterface_DevicePresentEx(
    __in PVOID Context,
    __in PPCI_DEVICE_PRESENCE_PARAMETERS Parameters
    )
/*++

Routine Description:

    This routine searches the PCI device tree to see if the specific device
    is present in the system.  Note devices that are explicitly not enumerated
    (such as PIIX4 power management function) are considered absent.

Arguments:

    Context - The device extension of the device requesting the search.

    Parameters - Pointer to a structure containing the parameters of the device search,
                 including VendorID, SubSystemID and ClassCode, among others.

Return Value:

    TRUE if the device is present in the system, FALSE otherwise.

--*/
{
    PAGED_CODE();

    return PciCheckDevicePresence((PPCI_DEVICE)Context, Parameters);
}

BOOLEAN
PciCheckDevicePresence(
    __in_opt PPCI_DEVICE Device,
    __in PPCI_DEVICE_PRESENCE_PARAMETERS Parameters
    )
/*++

Routine Description:

    This routine searches the PCI device tree to see if the specific device
    is present in the system.  Note devices that are explicitly not enumerated
    (such as PIIX4 power management function) are considered absent.

Arguments:

    Device - The device extension of the device requesting the search.

    Parameters - Pointer to a structure containing the parameters of the device search,
                 including VendorID, SubSystemID and ClassCode, among others.

Return Value:

    TRUE if the device is present in the system, FALSE otherwise.

--*/
{
    PPCI_SEGMENT segment;
    PPCI_BUS bus;
    BOOLEAN found;
    ULONG flags;

    PAGED_CODE();

    found = FALSE;

    //
    // Validate the parameters.
    //
    if (!ARGUMENT_PRESENT(Parameters)) {
        ERROR("PciDevicePresentInterface_DevicePresentEx: no parameters supplied.\n");
        return FALSE;
    }

    if (Parameters->Size < sizeof(PCI_DEVICE_PRESENCE_PARAMETERS)) {
        ERROR("PciDevicePresentInterface_DevicePresentEx: Invalid parameter supplied - size is bad.\n");
        return FALSE;
    }

    flags = Parameters->Flags;

    //
    // We can either do a Vendor/Device ID match, or a Class/SubClass
    // match.  If neither of these flags are present, fail.
    //
    if (!(flags & (PCI_USE_VENDEV_IDS | PCI_USE_CLASS_SUBCLASS))) {
        ERROR("PciDevicePresentInterface_DevicePresentEx: Invalid parameter supplied - match type not specified.\n");
        return FALSE;
    }

    //
    // RevisionID, SubVendorID and SubSystemID are more precise flags.
    // They are only valid if we're doing a Vendor/Device ID match.
    //
    if (flags & (PCI_USE_REVISION | PCI_USE_SUBSYSTEM_IDS)) {
        if (!(flags & PCI_USE_VENDEV_IDS)) {
            ERROR("PciDevicePresentInterface_DevicePresentEx: Invalid parameter supplied - SUBSYSTEM_IDS only valid on VEN/DEV ID match.\n");
            return FALSE;
        }
    }

    //
    // Programming Interface is also a more precise flag.
    // It is only valid if we're doing a class cod