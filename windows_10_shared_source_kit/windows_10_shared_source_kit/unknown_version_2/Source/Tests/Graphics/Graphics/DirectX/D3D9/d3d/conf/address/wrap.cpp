n;

    interface->Version              = PCI_DEVICE_PRESENT_INTERFACE_VERSION;
    interface->InterfaceReference   = PciDevicePresentInterface_Reference;
    interface->InterfaceDereference = PciDevicePresentInterface_Dereference;
    interface->Context              = DeviceExtension;
    interface->IsDevicePresent      = PciDevicePresentInterface_DevicePresent;

    //
    // This interface has been extended from the base interface (what was
    // filled in above), to a larger interface.  If the buffer provided
    // is large enough to hold the whole thing, fill in the rest.  Otherwise
    // don't.
    //
    if (Size >= sizeof(PCI_DEVICE_PRESENT_INTERFACE)) {

        interface->IsDevicePresentEx = PciDevicePresentInterface_DevicePresentEx;
        interface->Size = sizeof(PCI_DEVICE_PRESENT_INTERFACE);

    } 