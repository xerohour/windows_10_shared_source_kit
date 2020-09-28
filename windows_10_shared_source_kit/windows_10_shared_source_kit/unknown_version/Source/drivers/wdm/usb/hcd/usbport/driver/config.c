/*++

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    config.c

Abstract:

    handles configuration and interface URBs

Environment:

    kernel mode only

Notes:

Revision History:

    06-20-99 : created
    01-12-04 : fix bugs

--*/

#include "common.h"

#ifdef USBPORT_WPP
#include "config.tmh"
#endif

const USBD_PIPE_TYPE PipeTypes[4] = {
    UsbdPipeTypeControl,
    UsbdPipeTypeIsochronous,
    UsbdPipeTypeBulk,
    UsbdPipeTypeInterrupt
};

NTSTATUS
USBPORT_SelectInterface(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandleNotUsed,
    PHCD_ENDPOINT       EndpointNotUsed,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Select an alternate interface for a USB device.  The orginal
    USBD code only supported selecting a single alternate interface
    so we will as well.

    Client will(should) pass in a URB buffer that looks like this:

    +------------------------------+
    |Hdr                           |
    |(_URB_HEADER)                 |
    |    - <caller inputs>         |
    |      Function                |
    |      Length                  |
    |      UsbdDeviceHandle        |
    |                              |
    |    - <port outputs>          |
    |        Status                |
    +------------------------------+
    |    - <caller inputs>         |
    |      ConfigurationHandle     |
    +------------------------------+
    |Interface                     |
    |(USBD_INTERFACE_INFORMATION)  |
    |    - <caller inputs>         |
    |      Length                  |
    |      InterfaceNumber         |
    |      AlternateSetting        |
    |                              |
    |    - <port  outputs>         |
    |      InterfaceHandle         |
    |      NumberOfPipes           |
    |      SubClass                |
    |      Class                   |
    |      Protocol                |
    +------------------------------+
    |Pipes[0]                      | one of these for each pipe in the
    |(USBD_PIPE_INFORMATION)       | interface
    |    - <caller inputs>         |
    |      PipeFlags               |
    |      MaximumPacketSize (opt) |
    |                              |
    |    - <port outputs>          |
    +------------------------------+
    |Pipes[1]                      |
    +------------------------------+
    |....                          |
    +------------------------------+
    |Pipes[n]                      |
    +------------------------------+

Arguments:

Return Value:


--*/
{
    NTSTATUS ntStatus;
    PUSBD_CONFIG_HANDLE configHandle = NULL;
    ULONG i;
    PDEVICE_EXTENSION devExt;
    PUSBD_INTERFACE_INFORMATION interfaceI;
    PUSBD_INTERFACE_HANDLE_I iHandle, iHandleNew = NULL;
    USHORT tmp;
    USBD_STATUS usbdStatus;

    ASSERT_PASSIVE();

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOCK_DEVICE_CONFIGS(DeviceHandle, FdoDeviceObject);

    USBPORT_EtwWriteHcDeviceData4(
        FdoDeviceObject,
        DeviceHandle,
        &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_SELECT_INTERFACE,
        ActivityId,

        &Irp,
        sizeof(PIRP),

        &Urb,
        sizeof(PURB),

        &Urb->UrbSelectInterface.Hdr.Length,
        sizeof(USHORT),

        Urb,
        Urb->UrbSelectInterface.Hdr.Length);

    //
    // We need to do this for KMDF Usbser to workaround a bug in a buggy filter driver sitting
    // below KMDF. On recieving a SelectConfig URB, the filter creates a copy of a selectconfig URB
    // and sends to USB Core. On the completion path, it doesn't copy the ConfigurationHandle from
    // the copy to the original URB. Since we have the ConfigurationHandle saved in the DeviceHandle
    // we will use it. This is perfectly safe since USBPORT_SelectConfiguration keeps this copy updated
    //

    if (Urb->UrbSelectInterface.ConfigurationHandle == NULL) {
        Urb->UrbSelectInterface.ConfigurationHandle = DeviceHandle->ConfigurationHandle;
        configHandle = Urb->UrbSelectInterface.ConfigurationHandle;
    } else {
        // validate the configuration handle input
        configHandle = Urb->UrbSelectInterface.ConfigurationHandle;
        ASSERT_CONFIG_HANDLE(configHandle);
    }

    //
    // will are interested in the alt setting of a specific
    // interface based on the interface number.
    //

    iHandle = NULL;
    interfaceI = &Urb->UrbSelectInterface.Interface;

    // validate the Length field in the Urb header, we can
    // figure out the correct value based on the interface
    // information passed in
    tmp = interfaceI->Length + sizeof(struct _URB_HEADER)
        + sizeof(configHandle);

    if (tmp != Urb->UrbHeader.Length) {
        // client passed in bogus total length, warn if in
        // 'verifier' mode.

        DbgTrace((UsbpDebugTrace, "client driver passed invalid Urb.Header.Length\n"));

        // generally cleints mess up the header length so
        // we will override with the length we calculated
        // from the interface-information.

        Urb->UrbHeader.Length = tmp;
    }

    // validate the interfaceI structure passed to us by the client
    usbdStatus = USBPORT_InitializeInterfaceInformation(FdoDeviceObject,
                                                        interfaceI,
                                                        configHandle);

    if (usbdStatus == USBD_STATUS_SUCCESS) {

        // find the interface handle for the interface we are
        // interested in, if it is currently open we will need
        // to close it.

        iHandle = USBPORT_GetInterfaceHandle(FdoDeviceObject,
                                             configHandle,
                                             interfaceI->InterfaceNumber);

        if (iHandle != NULL) {

            // unlink this handle
            RemoveEntryList(&iHandle->InterfaceLink);

            // we have a handle

            ASSERT_INTERFACE(iHandle);

            // close the pipes in this interface, note that we
            // force the pipes closed unlike past versions of
            // USBD and force the client driver to deal with the
            // consequences if it has transfers outstanding.

            // attempt to close all endpoints in this interface
            for (i=0; i < iHandle->InterfaceDescriptor.bNumEndpoints; i++) {

                USBPORT_ClosePipe(DeviceHandle,
                                  FdoDeviceObject,
                                  &iHandle->PipeHandle[i]);
            }
        }

        //
        // Now open the new interface with the new alternate setting
        //

        iHandleNew = NULL;
        usbdStatus = USBPORT_InternalOpenInterface(Urb,
                                                   DeviceHandle,
                                                   FdoDeviceObject,
                                                   configHandle,
                                                   interfaceI,
                                                   &iHandleNew,
                                                   TRUE);
    }

    if (usbdStatus == USBD_STATUS_SUCCESS) {

        //
        // successfully opened the new interface,
        // we can free the old handle now if we
        // had one.
        //
        if (iHandle != NULL ) {
#if DBG
            // all pipes should be closed
            for (i=0; i < iHandle->InterfaceDescriptor.bNumEndpoints; i++) {
                USBPORT_ASSERT(iHandle->PipeHandle[i].ListEntry.Flink == NULL &&
                               iHandle->PipeHandle[i].ListEntry.Blink == NULL);
            }
#endif
            UsbFreePool(iHandle);
            iHandle = NULL;
        }

        // return the 'new' handle
        interfaceI->InterfaceHandle = iHandleNew;

        // associate it with this configuration
        InsertTailList(&configHandle->InterfaceHandleList,
                       &iHandleNew->InterfaceLink);

    } else {

        //
        // selecting the aternate interface failed.
        // Possible reasons:
        //
        // 1. we didn't have enough BW
        // 2. the device stalled the set_interface request
        // 3. The set_interface request failed because the
        //      device is gone
        // 4. USBPORT_InitializeInterfaceInformation() failed due
        //      bad parameters.

        // attempt to re-open the original alt-interface so that
        // the client still has the bandwidth

        LOGENTRY(FdoDeviceObject, LOG_PNP, 'slI!',
            usbdStatus,
            0,
            0);

        if (usbdStatus == USBD_STATUS_NO_BANDWIDTH) {

            // HISTORICAL NOTE:
            // The 2k USBD driver attempted to re-open the original
            // alt-setting on a failure to allocate bw.  This would
            // leave the client with the bw it had when calling in
            // to select the new interface.
            //
            // I don't beleive that any drivers use this feature
            // and many drivers attempt to allocate BW in a loop
            // until they succeed.
            //
            // So as a performance optimization we will return
            // with no bandwidth allocated to the caller -- the
            // pipe handles will be invalid.
            // This should speed things up since the realloc of
            // the old bandwidth takes time.
            interfaceI->InterfaceHandle = USBPORT_BAD_HANDLE;

        } else {
            // case 2,3 we just fail the request and set the interface
            // handle to 'bad handle'
            interfaceI->InterfaceHandle = USBPORT_BAD_HANDLE;
        }

        // client has no reference to it and we closed it
        // free the structure here
        if (iHandle != NULL ) {
#if DBG
            // all pipes should be closed
            for (i=0; i < iHandle->InterfaceDescriptor.bNumEndpoints; i++) {
                USBPORT_ASSERT(iHandle->PipeHandle[i].ListEntry.Flink == NULL &&
                               iHandle->PipeHandle[i].ListEntry.Blink == NULL);
            }
#endif
            UsbFreePool(iHandle);
            iHandle = NULL;
        }

    }

    UNLOCK_DEVICE_CONFIGS(DeviceHandle, FdoDeviceObject);

    ntStatus = SET_USBD_ERROR(Urb, usbdStatus);

    return ntStatus;

}


NTSTATUS
USBPORT_SelectConfiguration(
    PDEVICE_OBJECT      FdoDeviceObject,
    PIRP                Irp,
    PURB                Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PUSBD_PIPE_HANDLE_I PipeHandleNotUsed,
    PHCD_ENDPOINT       EndpointNotUsed,
    LPCGUID             ActivityId
    )
/*++

Routine Description:

    Open a configuration for a USB device.

    Client will(should) pass in a URB buffer that looks like this:

    +------------------------------+
    |Hdr                           |
    |(_URB_HEADER)                 |
    |    - <caller inputs>         |
    |      Function                |
    |      Length                  |
    |      UsbdDeviceHandle        |
    |                              |
    |    - <port outputs>          |
    |       Status                 |
    +------------------------------+
    |    - <caller inputs>         |
    |      ConfigurationDescriptor |
    |    - <port outputs>          |
    |      ConfigurationHandle     |
    +------------------------------+
    |Interface(0)                  |
    |(USBD_INTERFACE_INFORMATION)  |
    |    - <caller inputs>         |
    |      Length                  |
    |      InterfaceNumber         |
    |      AlternateSetting        |
    |                              |
    |    - <port  outputs>         |
    |      InterfaceHandle         |
    |      NumberOfPipes           |
    |      SubClass                |
    |      Class                   |
    |      Protocol                |
    +------------------------------+
    |Pipes[0]                      | one of these for each pipe in the
    |(USBD_PIPE_INFORMATION)       | interface
    |    - <caller inputs>         |
    |                              |
    |    - <port outputs>          |
    +------------------------------+
    |Pipes[1]                      |
    +------------------------------+
    |....                          |
    +------------------------------+
    |Pipes[n]                      |
    +------------------------------+
    | Interface(1)                 | one of these for each interface in
    |                              | the configuration
    +------------------------------+
    |Pipes[1]                      |
    +------------------------------+
    |....                          |
    +------------------------------+
    |Pipes[n]                      |
    +------------------------------+

    On input:
    The ConfigurationDescriptor must specify the number of interfaces
    in the configuration.

    The InterfaceInformation will specify a specific alt setting to be
    selected for each interface.

    1. First we look at the configuration descriptor for the
        requested configuration and validate the client
        input buffer agianst it.

    2. We open the interfaces for the requested configuration
        and open the pipes within those interfaces, setting
        alt settings were appropriate.

    3. We set the configuration for the device with the
        appropriate control request.

Arguments:

    DeviceObject -

    Irp -  IO request block

    Urb -  ptr to USB request block

    IrpIsPending -

Return Value:


--*/
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    PUSBD_CONFIG_HANDLE configHandle = NULL;
    PUSB_CONFIGURATION_DESCRIPTOR configurationDescriptor;
    USHORT wTotalLength;
    PUSBD_INTERFACE_INFORMATION interfaceInformation;
    PUCHAR pch;
    ULONG i;
    PDEVICE_EXTENSION devExt;
    ULONG numInterfaces;
    PUCHAR end;
    USBD_STATUS usbdStatus;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;

    ASSERT_PASSIVE();
    DbgTrace((UsbpDebugTrace, "' enter USBPORT_SelectConfiguration\n"));

    GET_DEVICE_EXT(devExt, FdoDeviceObject);
    ASSERT_FDOEXT(devExt);

    LOCK_DEVICE_CONFIGS(DeviceHandle, FdoDeviceObject);

    configurationDescriptor =
        Urb->UrbSelectConfiguration.ConfigurationDescriptor;

    if (configurationDescriptor == NULL) {
        wTotalLength = 0;
    } else {
        wTotalLength = configurationDescriptor->wTotalLength;
    }

    USBPORT_EtwWriteHcDeviceData6(
        FdoDeviceObject,
        DeviceHandle,
        &USBPORT_ETW_EVENT_DISPATCH_URB_FUNCTION_SELECT_CONFIGURATION,
        ActivityId,

        &Irp,
        sizeof(PIRP),

        &Urb,
        sizeof(PURB),

        &Urb->UrbSelectConfiguration.Hdr.Length,
        sizeof(USHORT),

        Urb,
        Urb->UrbSelectConfiguration.Hdr.Length,

        &wTotalLength,
        sizeof(USHORT),

        configurationDescriptor,
        wTotalLength);

    ntStatus = STATUS_BOGUS;

    // BUGBUG
    // flush all current transfers or fail?

    //
    // dump old configuration data if we have any
    //

    if (DeviceHandle->ConfigurationHandle) {

        // This is where we close the old configuration
        // handle, all pipes and all interfaces.

        USBPORT_InternalCloseConfiguration(DeviceHandle,
                                           FdoDeviceObject,
                                           0);
    }

    // now set up the new configuration

    //
    // if null pased in set configuration to 0
    // 'unconfigured'
    //

    if (configurationDescriptor == NULL) {

        // device needs to be in the unconfigured state

        //
        // NOTE:
        // this may fail if the configuration is being
        // closed as the result of the device being unplugged
        // so we ignore the error
        //

        USBPORT_INIT_SETUP_PACKET(setupPacket,
                USB_REQUEST_SET_CONFIGURATION, // bRequest
                BMREQUEST_HOST_TO_DEVICE, // Dir
                BMREQUEST_TO_DEVICE, // Recipient
                BMREQUEST_STANDARD, // Type
                0, // wValue
                0, // wIndex
                0); // wLength

        USBPORT_SendCommand(DeviceHandle,
                           FdoDeviceObject,
                           &setupPacket,
                           NULL,
                           0,
                           NULL,
                           NULL);

        ntStatus = SET_USBD_ERROR(Urb, USBD_STATUS_SUCCESS);

        goto USBD_SelectConfiguration_Done;

    } else {
        // "Validate" the Configuration Descriptor buffer by accessing it.
        //
        // Note: We may crash here if the Configuration Descriptor buffer
        // is invalid but it should be easiser to debug this way.
        //
        PUCHAR tmp;
        UCHAR  chFirst, chLast;

        LOGENTRY(FdoDeviceObject, LOG_PNP, 'vCNF',
                 configurationDescriptor,
                 configurationDescriptor->wTotalLength,
                 0);

        // First a quick sanity check: the Configuration Descriptor
        // wTotalLength must be non-zero.
        //
        if (configurationDescriptor->wTotalLength == 0) {
            // this is bogus
            ntStatus = SET_USBD_ERROR(Urb,
                USBD_STATUS_INAVLID_CONFIGURATION_DESCRIPTOR);

            goto USBD_SelectConfiguration_Done;
        }

        // Touch the first and last byte of the Configuration Descriptor
        // buffer.  This should fault if the buffer is invalid.
        //
        tmp = (PUCHAR)configurationDescriptor;
        chFirst = *tmp;
        tmp += configurationDescriptor->wTotalLength - 1;
        chLast = *tmp;

        LOGENTRY(FdoDeviceObject, LOG_PNP, 'CFok',
                 configurationDescriptor,
                 chFirst,
                 chLast);
    }

    //
    // count the number of interfaces to process in this
    // request
    //

    pch = (PUCHAR) &Urb->UrbSelectConfiguration.Interface;
    numInterfaces = 0;
    end = ((PUCHAR) Urb) + Urb->UrbSelectConfiguration.Hdr.Length;

#define MIN_INTERFACE_INFO_SIZE     \
           (sizeof(USBD_INTERFACE_INFORMATION) - sizeof(USBD_PIPE_INFORMATION))

    while (((pch + sizeof(USHORT)) < end) &&
           (numInterfaces < configurationDescriptor->bNumInterfaces)) {
        interfaceInformation = (PUSBD_INTERFACE_INFORMATION) pch;

        if((interfaceInformation->Length >= MIN_INTERFACE_INFO_SIZE) &&
           (pch + interfaceInformation->Length <= end ) ) {
            numInterfaces++;
            pch += interfaceInformation->Length;
        }
/*
        Fix for CATC driver
        else
        if ((interfaceInformation->Length >= MIN_INTERFACE_INFO_SIZE) &&
            ((MIN_INTERFACE_INFO_SIZE +
                (sizeof(USBD_PIPE_INFORMATION) *
                        interfaceInformation->NumberOfPipes))) ==
                                            interfaceInformation->Length) {
            numInterfaces++;
            pch += interfaceInformation->Length;
        }
*/
        else {
            ntStatus = STATUS_UNSUCCESSFUL;
            Urb->UrbHeader.Status = USBD_STATUS_INVALID_CONFIGURATION_DESCRIPTOR;
            goto USBD_SelectConfiguration_Done;
        }
    }

    DbgTrace((UsbpDebugTrace, "'USBD_SelectConfiguration -- %d interfaces\n",
        numInterfaces));

    // sanity check the config descriptor with the URB request
    if (numInterfaces != configurationDescriptor->bNumInterfaces ||
        numInterfaces == 0) {
        //
        // driver is broken, config request does not match
        // config descriptor passed in!!!
        //
        DbgTrace((UsbpDebugTrace, "config request does not match config descriptor\n"));
        ntStatus = SET_USBD_ERROR(Urb,
            USBD_STATUS_INAVLID_CONFIGURATION_DESCRIPTOR);

        TEST_TRAP();

        goto USBD_SelectConfiguration_Done;
    }

    //
    // Allocate a configuration handle and
    // verify there is enough room to store
    // all the information in the client buffer.
    //

    configHandle =
        USBPORT_InitializeConfigurationHandle(DeviceHandle,
                                              FdoDeviceObject,
                                              configurationDescriptor);
    if (configHandle == NULL) {
        DbgTrace((UsbpDebugTrace, "failed to allocate config handle\n"));
        ntStatus = SET_USBD_ERROR(Urb,
            USBD_STATUS_INSUFFICIENT_RESOURCES);

        goto USBD_SelectConfiguration_Done;
    }

    //
    // Send the 'set configuration' command
    //

    USBPORT_INIT_SETUP_PACKET(setupPacket,
                USB_REQUEST_SET_CONFIGURATION, // bRequest
                BMREQUEST_HOST_TO_DEVICE, // Dir
                BMREQUEST_TO_DEVICE, // Recipient
                BMREQUEST_STANDARD, // Type
                configurationDescriptor->bConfigurationValue, // wValue
                0, // wIndex
                0); // wLength


    USBPORT_SendCommand(DeviceHandle,
                       FdoDeviceObject,
                       &setupPacket,
                       NULL,
                       0,
                       NULL,
                       &usbdStatus);

    DbgTrace((UsbpDebugTrace,"' SendCommand, SetConfiguration returned 0x%x\n",  usbdStatus));

    if (USBD_ERROR(usbdStatus)) {
        DbgTrace((UsbpDebugTrace,"' failed to 'set' the configuration 0x%x\n",  usbdStatus));

        ntStatus = SET_USBD_ERROR(Urb, USBD_STATUS_SET_CONFIG_FAILED);
        TEST_TRAP();
        goto USBD_SelectConfiguration_Done;
    }

    USBPORT_ASSERT(ntStatus == STATUS_BOGUS);

    // we have "configured" the device in the USB sense.

    //
    // User buffer checks out and we have 'configured'
    // the device.
    // Now parse thru the configuration descriptor
    // and open the interfaces.
    //
    // The URB contains a set of INTERFACE_INFORMATION
    // structures these give us the information we need
    // to open the pipes
    /*

    _USBD_INTERFACE_INFORMATION
    client should have filled in:

    USHORT Length;
    UCHAR InterfaceNumber;
    UCHAR AlternateSetting;

    we fill in :
    UCHAR Class;
    UCHAR SubClass;
    UCHAR Protocol;
    UCHAR Reserved;

    USBD_INTERFACE_HANDLE InterfaceHandle;
    ULONG NumberOfPipes;

    */

    pch = (PUCHAR) &Urb->UrbSelectConfiguration.Interface;

    for (i=0; i<numInterfaces; i++) {
        PUSBD_INTERFACE_HANDLE_I interfaceHandle;
        // open the interface

        interfaceInformation = (PUSBD_INTERFACE_INFORMATION) pch;

        usbdStatus = USBPORT_InitializeInterfaceInformation(
                        FdoDeviceObject,
                        interfaceInformation,
                        configHandle);

        interfaceHandle = NULL;
        if (usbdStatus == USBD_STATUS_SUCCESS) {

            // this function allocates the actual 'handle'
            usbdStatus = USBPORT_InternalOpenInterface(Urb,
                                                       DeviceHandle,
                                                       FdoDeviceObject,
                                                       configHandle,
                                                       interfaceInformation,
                                                       &interfaceHandle,
                                                       TRUE);
            DbgTrace((UsbpDebugTrace, "' InternalOpenInterface returned(USBD) 0x%x\n",  usbdStatus));
            LOGENTRY(FdoDeviceObject, LOG_PNP, 'oCFG', usbdStatus, 0, DeviceHandle);
        }

        pch+=interfaceInformation->Length;

        // if we got back a handle add it to the list
        if (interfaceHandle != NULL) {
            InsertTailList(&configHandle->InterfaceHandleList,
                           &interfaceHandle->InterfaceLink);
        }

        if (!USBD_SUCCESS(usbdStatus)) {

            ntStatus = SET_USBD_ERROR(Urb, usbdStatus);

            // we have an error opening the interface
            TEST_TRAP();
            TEST_TRAP();
            goto USBD_SelectConfiguration_Done;
        }
    }

    //
    // interfaces were successfully set up then return success.
    //

    ntStatus = SET_USBD_ERROR(Urb, USBD_STATUS_SUCCESS);

USBD_SelectConfiguration_Done:

    if (NT_SUCCESS(ntStatus)) {

        USBPORT_ASSERT(Urb->UrbSelectConfiguration.Hdr.Status ==
            USBD_STATUS_SUCCESS);

        Urb->UrbSelectConfiguration.ConfigurationHandle =
            configHandle;

        // remember the current configuration
        DeviceHandle->ConfigurationHandle = configHandle;

    } else {

        //
        // something failed, clean up before we return an error.
        //

        if (configHandle) {

            TEST_TRAP();
            ASSERT_DEVICE_HANDLE(DeviceHandle);

            //
            // if we have a configHandle then we need to free it
            DeviceHandle->ConfigurationHandle =
                configHandle;

            //
            // attempt to close it
            //

            USBPORT_InternalCloseConfiguration(DeviceHandle,
                                               FdoDeviceObject,
                                               0);

            DeviceHandle->ConfigurationHandle = NULL;
        }

        // make sure we return an error in the URB.
        USBPORT_ASSERT(Urb->UrbSelectConfiguration.Hdr.Status !=
            USBD_STATUS_SUCCESS);
        DbgTrace((UsbpDebugTrace, "'Failing SelectConfig\n"));
    }

    UNLOCK_DEVICE_CONFIGS(DeviceHandle, FdoDeviceObject);

    DbgTrace((UsbpDebugTrace, "'exit SelectConfiguration 0x%x\n", ntStatus));

    return ntStatus;
}


PUSBD_CONFIG_HANDLE
USBPORT_InitializeConfigurationHandle(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor
    )
/*++

Routine Description:

    Initialize the configuration handle structure.

    Given a (hopefully) valid configuration descriptor
    and a count of the interfaces create the configuration
    handle for the device

Arguments:

Return Value:


--*/
{
    PUSBD_CONFIG_HANDLE configHandle = NULL;
    PUCHAR pch;
    USHORT wTotalLengthFromDescriptor;

    ASSERT_PASSIVE();
    USBPORT_ASSERT(ConfigurationDescriptor->bNumInterfaces > 0);

    DbgTrace((UsbpDebugTrace, "' enter InitializeConfigurationHandle\n"));

    wTotalLengthFromDescriptor = ConfigurationDescriptor->wTotalLength;

    if( wTotalLengthFromDescriptor < sizeof(USB_CONFIGURATION_DESCRIPTOR)) {
        return NULL;
    }

    // get enough space for each interface
    ALLOC_POOL_Z(configHandle,
                 NonPagedPoolNx,
                 sizeof(USBD_CONFIG_HANDLE) +
                 wTotalLengthFromDescriptor);

    pch = (PUCHAR)configHandle;

    if (configHandle) {

        //
        // Initilaize the interface handle list
        //

        InitializeListHead(&configHandle->InterfaceHandleList);

        configHandle->ConfigurationDescriptor = (PUSB_CONFIGURATION_DESCRIPTOR)
                              (pch + sizeof(USBD_CONFIG_HANDLE));

        // copy the config descriptor to our handle

        RtlCopyMemory(configHandle->ConfigurationDescriptor,
                      ConfigurationDescriptor,
                      wTotalLengthFromDescriptor);

        configHandle->ConfigurationDescriptor->wTotalLength = wTotalLengthFromDescriptor;

        configHandle->Sig = SIG_CONFIG_HANDLE;

    }

    DbgTrace((UsbpDebugTrace, "' exit InitializeConfigurationHandle %p\n", configHandle));

    return configHandle;
}


USBD_STATUS
USBPORT_InternalOpenInterface(
    PURB Urb,
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_CONFIG_HANDLE ConfigHandle,
    PUSBD_INTERFACE_INFORMATION InterfaceInformation,
    PUSBD_INTERFACE_HANDLE_I *InterfaceHandle,
    BOOLEAN SendSetInterfaceCommand
    )
/*++

Routine Description:


Arguments:

    DeviceObject -

    DeviceHandle - USBD device handle for this device.

    ConfigHandle - USBD configuration handle.

    InterfaceInformation - pointer to USBD interface information structure
        passed in by the client.

        We use the InterfaceNumber and AlternateSetting specified
        in this structure to select the interface.

        On success the .Length field is filled in with the actual length
        of the interface_information structure and the Pipe[] fields are filled
        in with the handles for the opened pipes.

    InterfaceHandle - pointer to an interface handle pointer, filled in
        with the allocated interface handle structure if NULL, otherwise the
        structure passed in is used.

    SendSetInterfaceCommand - indicates if the set_interface command should be
        sent.

Return Value:


--*/
{
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;
    BOOLEAN hasAlternateSettings;
    PUSBD_INTERFACE_HANDLE_I interfaceHandle = NULL;
    PUSB_ENDPOINT_DESCRIPTOR endpointDescriptor;
    PUSB_INTERFACE_DESCRIPTOR interfaceDescriptor;
    PUCHAR pch;
    ULONG i;
    BOOLEAN allocated = FALSE;
    PUSB_COMMON_DESCRIPTOR descriptor;
    USHORT need;
    ULONG numEndpoints;
    USB_DEFAULT_PIPE_SETUP_PACKET setupPacket;

    ASSERT_PASSIVE();

    ASSERT_CONFIG_HANDLE(ConfigHandle);

    if (*InterfaceHandle != NULL) {
        // using a previously allocated interface handle
        ASSERT_INTERFACE_HANDLE(*InterfaceHandle);
        TEST_TRAP();
    }

    LOGENTRY(FdoDeviceObject, LOG_PNP, 'opIF',
        InterfaceInformation->InterfaceNumber,
        InterfaceInformation->AlternateSetting,
        *InterfaceHandle);

    DbgTrace((UsbpDebugTrace, "' enter InternalOpenInterface\n"));
    DbgTrace((UsbpDebugTrace, "' Interface %d Altsetting %d\n",
        InterfaceInformation->InterfaceNumber,
        InterfaceInformation->AlternateSetting));

    //
    // Find the interface descriptor we are interested in inside
    // the configuration descriptor.
    //

    interfaceDescriptor =
        USBPORT_InternalParseConfigurationDescriptor(
              ConfigHandle->ConfigurationDescriptor,
              InterfaceInformation->InterfaceNumber,
              InterfaceInformation->AlternateSetting,
              &hasAlternateSettings);

    // we already validated this, if it is NULL
    // the function has a bug.
    USBPORT_ASSERT(interfaceDescriptor != NULL);
    if (interfaceDescriptor == NULL) {
        BUGCHECK_FDO(FdoDeviceObject,
                     USBBUGCODE_INTERNAL_ERROR, // BugCode
                     (ULONG_PTR)DeviceHandle,   // BugCheckParameter2
                     0,                         // BugCheckParameter3
                     0,                         // BugCheckParameter4
                     0,                         // DataLength
                     NULL);                     // DataBuffer
        // keep prefix scanner happy
        // Unreachabe code
        //return USBD_STATUS_SUCCESS;
    }

    //
    // We got the interface descriptor, now try
    // to open all the pipes.
    //

    // found the requested interface in the configuration descriptor.


    numEndpoints = interfaceDescriptor->bNumEndpoints;
    need = sizeof(USBD_INTERFACE_INFORMATION) - sizeof(USBD_PIPE_INFORMATION);
    need += (USHORT)(numEndpoints * sizeof(USBD_PIPE_INFORMATION));

    // we should have already validated this
    USBPORT_ASSERT(InterfaceInformation->Length == need);

    if (hasAlternateSettings &&
        SendSetInterfaceCommand) {

        NTSTATUS ntStatus;
        //
        // If we have alternate settings we need
        // to send the set interface command.
        //

        USBPORT_INIT_SETUP_PACKET(setupPacket,
            USB_REQUEST_SET_INTERFACE, // bRequest
            BMREQUEST_HOST_TO_DEVICE, // Dir
            BMREQUEST_TO_INTERFACE, // Recipient
            BMREQUEST_STANDARD, // Type
            InterfaceInformation->AlternateSetting, // wValue
            InterfaceInformation->InterfaceNumber, // wIndex
            0); // wLength


        ntStatus = USBPORT_SendCommand(DeviceHandle,
                           FdoDeviceObject,
                           &setupPacket,
                           NULL,
                           0,
                           NULL,
                           &usbdStatus);

        LOGENTRY(FdoDeviceObject, LOG_PNP, 'seIF',
            ntStatus,
            InterfaceInformation->AlternateSetting,
            InterfaceInformation->InterfaceNumber);

        if (USBD_ERROR(usbdStatus)) {
            TEST_TRAP();
            goto USBPORT_InternalOpenInterface_Done;
        }

        USBPORT_ASSERT(ntStatus == STATUS_SUCCESS);

    }

    //
    // we successfully selected the alternate interface
    // initialize the interface handle and open the pipes
    //

    if (*InterfaceHandle == NULL) {

        ULONG privateLength = sizeof(USBD_INTERFACE_HANDLE_I) +
                              sizeof(USBD_PIPE_HANDLE_I) * numEndpoints;

        // allow space for a copy of the USBD_INTERFACE_INFORMATION
        // that is returned to the client
        ALLOC_POOL_Z(interfaceHandle,
                     NonPagedPoolNx,
                     privateLength);

        if (interfaceHandle != NULL) {
            // initialize the pipe handles to a known state
            for (i=0; i<numEndpoints; i++) {
                interfaceHandle->PipeHandle[i].EpHandle = NULL;
                interfaceHandle->PipeHandle[i].Sig = SIG_PIPE_HANDLE;
                interfaceHandle->PipeHandle[i].PipeStateFlags =
                    USBPORT_PIPE_STATE_CLOSED;
                //interfaceHandle->PipeHandle[i].ListEntry.Flink = NULL;
                //interfaceHandle->PipeHandle[i].ListEntry.Blink = NULL;
            }
            allocated = TRUE;
        } else {

            usbdStatus = USBD_STATUS_INSUFFICIENT_RESOURCES;

            goto USBPORT_InternalOpenInterface_Done;
        }

    } else {
        // using old handle
        interfaceHandle = *InterfaceHandle;
    }

    LOGENTRY(FdoDeviceObject, LOG_PNP, 'ihIF',
            interfaceHandle,
            0,
            0);

    USBPORT_ASSERT(interfaceHandle != NULL);

    interfaceHandle->Sig = SIG_INTERFACE_HANDLE;
    interfaceHandle->HasAlternateSettings = hasAlternateSettings;

    InterfaceInformation->NumberOfPipes =
        interfaceDescriptor->bNumEndpoints;
    InterfaceInformation->Class =
        interfaceDescriptor->bInterfaceClass;
    InterfaceInformation->SubClass =
        interfaceDescriptor->bInterfaceSubClass;
    InterfaceInformation->Protocol =
        interfaceDescriptor->bInterfaceProtocol;
    InterfaceInformation->Reserved = 0;

    // start with first endpoint
    // skip over any non-endpoint descriptors
    pch = (PUCHAR) (interfaceDescriptor) +
        interfaceDescriptor->bLength;

    // initialize the pipe fields for this interfacae

    // assume success
    usbdStatus = USBD_STATUS_SUCCESS;

    interfaceHandle->InterfaceDescriptor = *interfaceDescriptor;
    for (i=0; i<numEndpoints; i++) {
        USB_HIGH_SPEED_MAXPACKET muxPacket;

        descriptor = (PUSB_COMMON_DESCRIPTOR) pch;
        while (descriptor->bDescriptorType !=
               USB_ENDPOINT_DESCRIPTOR_TYPE) {
            if (descriptor->bLength == 0) {
                break; // Don't loop forever
            }
            pch += descriptor->bLength;
            descriptor = (PUSB_COMMON_DESCRIPTOR) pch;
        }

        endpointDescriptor = (PUSB_ENDPOINT_DESCRIPTOR) pch;
        USBPORT_ASSERT(endpointDescriptor->bDescriptorType ==
            USB_ENDPOINT_DESCRIPTOR_TYPE);

        // initial state is CLOSED
        interfaceHandle->PipeHandle[i].PipeStateFlags =
            USBPORT_PIPE_STATE_CLOSED;
        interfaceHandle->PipeHandle[i].EpHandle = NULL;

        // init pipe flags
        interfaceHandle->PipeHandle[i].UsbdPipeFlags =
            InterfaceInformation->Pipes[i].PipeFlags;

        if (InterfaceInformation->Pipes[i].PipeFlags &
            USBD_PF_CHANGE_MAX_PACKET) {
            // client wants to override original max_packet
            // size in endpoint descriptor
             endpointDescriptor->wMaxPacketSize =
                InterfaceInformation->Pipes[i].MaximumPacketSize;

            DbgTrace((UsbpDebugTrace,
                "'new bMaxPacket 0x%x\n", endpointDescriptor->wMaxPacketSize));
        }

        //
        // copy the endpoint descriptor into the
        // pipe handle structure.
        //

        RtlCopyMemory(&interfaceHandle->PipeHandle[i].EndpointDescriptor,
                      pch,
                      sizeof(interfaceHandle->PipeHandle[i].
                            EndpointDescriptor) );

        // advance to next endpoint
        // first field in endpoint descriptor is length
        pch += endpointDescriptor->bLength;

        //
        // return information about the pipe
        //
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'ipIF',
            interfaceHandle,
            i,
            &interfaceHandle->PipeHandle[i]);

        InterfaceInformation->Pipes[i].EndpointAddress =
            endpointDescriptor->bEndpointAddress;
        InterfaceInformation->Pipes[i].PipeType =
            PipeTypes[endpointDescriptor->
                bmAttributes & USB_ENDPOINT_TYPE_MASK];

        if (DeviceHandle->DeviceDescriptor.bcdUSB < 0x250) {
            //
            // Need to adjust the MaxPacket
            //

            muxPacket.us = endpointDescriptor->wMaxPacketSize;
            InterfaceInformation->Pipes[i].MaximumPacketSize =
                muxPacket.MaxPacket * (muxPacket.HSmux+1);

        } else {

           InterfaceInformation->Pipes[i].MaximumPacketSize =
               endpointDescriptor->wMaxPacketSize;

        }
#if 0
        //!!!!
        // For test purposes, convert period 1 to period 8
        // if iso enpoint is HS and part of an audio interfaxe
        {
            PDEVICE_EXTENSION devExt;
            GET_DEVICE_EXT(devExt, FdoDeviceObject);
            if (InterfaceInformation->Pipes[i].PipeType ==
                    UsbdPipeTypeIsochronous &&
                DeviceHandle->DeviceSpeed == HighSpeed &&
                USBPORT_IS_USB20(devExt) &&
                InterfaceInformation->Class == USB_DEVICE_CLASS_AUDIO) {

                DbgTrace((UsbpDebugTrace, "'original interval = %d\n",
                    endpointDescriptor->bInterval));

                interfaceHandle->PipeHandle[i].EndpointDescriptor.bInterval =
                    endpointDescriptor->bInterval = 4;

                DbgTrace((UsbpDebugTrace, "'new interval = %d\n",
                    endpointDescriptor->bInterval));
                //TEST_TRAP();
             }
        }
#endif
        InterfaceInformation->Pipes[i].Interval =
            endpointDescriptor->bInterval;

        InterfaceInformation->Pipes[i].PipeHandle =
            USBPORT_BAD_HANDLE;

    } /* end for numEndpoints */

    if (usbdStatus != USBD_STATUS_SUCCESS) {
        // if we got an error bail now
        // we will return with the structure
        // initailized but no open pipes
        goto USBPORT_InternalOpenInterface_Done;
    }

    // all pipe handle fields initialized and
    // urb structure has been filled in

    // Do some wMaxPacketSize validation so the miniports do not need to
    // do this.  The UHCI miniport packet double buffers assume packets
    // are not illegally larger than allowed in the USB specification.
    //
    for (i=0; i<interfaceDescriptor->bNumEndpoints; i++) {

        endpointDescriptor = &interfaceHandle->PipeHandle[i].EndpointDescriptor;

        switch (DeviceHandle->DeviceSpeed) {
            case LowSpeed:
                switch (endpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK) {
                    case USB_ENDPOINT_TYPE_CONTROL:
                    case USB_ENDPOINT_TYPE_INTERRUPT:
                        // A low-speed device is limited to an 8 byte
                        // maximum packet size for control and interrupt
                        // endpoints.
                        if (endpointDescriptor->wMaxPacketSize > 8) {
                            usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                            break;
                        }
                        if (endpointDescriptor->wMaxPacketSize == 0) {
                            usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                            break;
                        }
                        break;
                    case USB_ENDPOINT_TYPE_ISOCHRONOUS:
                    case USB_ENDPOINT_TYPE_BULK:
                    default:
                        // A low-speed device must not have bulk or
                        // isochronous endpoints.
                        usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                        break;
                }
                break;

            case FullSpeed:
                switch (endpointDescriptor->bmAttributes & USB_ENDPOINT_TYPE_MASK) {
                    case USB_ENDPOINT_TYPE_CONTROL:
                    case USB_ENDPOINT_TYPE_BULK:
                        if (endpointDescriptor->wMaxPacketSize == 0) {
                            usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                            break;
                        }
                    case USB_ENDPOINT_TYPE_INTERRUPT:
                        // A full-speed device is limited to a 64 byte
                        // maximum packet size for control, bulk, and
                        // interrupt endpoints.

                        if (endpointDescriptor->wMaxPacketSize > 64) {
                            usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                        }
                        break;
                    case USB_ENDPOINT_TYPE_ISOCHRONOUS:
                        // A full-speed device is limited to a 1023 byte
                        // maximum packet size for isochronous
                        // endpoints.
                        if (endpointDescriptor->wMaxPacketSize > 1023) {
                            usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                        }
                        break;
                    default:
                        usbdStatus = USBD_STATUS_INVALID_PARAMETER;
                        break;
                }
                break;

            case HighSpeed:
            default:
                break;
        }

        if (usbdStatus != USBD_STATUS_SUCCESS) {
            LOGENTRY(FdoDeviceObject, LOG_PNP, 'ivIF',
                     i,
                     endpointDescriptor->bmAttributes,
                     endpointDescriptor->wMaxPacketSize);
            break;
        }
    }

    if (usbdStatus != USBD_STATUS_SUCCESS) {
        // if we got an error bail now
        // we will return with the structure
        // initailized but no open pipes
        goto USBPORT_InternalOpenInterface_Done;
    }

    // now loop thru and open the pipes
    for (i=0; i<interfaceDescriptor->bNumEndpoints; i++) {
        NTSTATUS ntStatus;
        PHCD_ENDPOINT endpoint = NULL;

        ntStatus = USBPORT_OpenEndpoint(DeviceHandle,
                                        FdoDeviceObject,
                                        &interfaceHandle->PipeHandle[i],
                                        &usbdStatus,
                                        FALSE);

        if (NT_SUCCESS(ntStatus)) {

            // if success set the pipe handle for client
            InterfaceInformation->Pipes[i].PipeHandle =
                &interfaceHandle->PipeHandle[i];
            DbgTrace((UsbpDebugTrace, "'pipe handle = %p\n", InterfaceInformation->Pipes[i].PipeHandle ));

            // We need to return the appropriate MaximumTransferSize and
            // Interval values for the pipe.

            if(interfaceHandle->PipeHandle[i].EpHandle != USB_ZERO_BW_ENDP_HANDLE){
                endpoint = USBPORT_EndpointFromHandle(interfaceHandle->PipeHandle[i].EpHandle);
                USBPORT_ASSERT(endpoint);

                switch(endpoint->Parameters.TransferType){

                case Interrupt:
                    if(endpoint->Parameters.DeviceSpeed == HighSpeed) {

                        // The highest bInterval value we allow is 6 (period=32)

                        if (InterfaceInformation->Pipes[i].Interval > 6){
                            InterfaceInformation->Pipes[i].Interval = 6;
                        }
                    } else {
                        // OpenEndpoint already set this value appropriately

                        InterfaceInformation->Pipes[i].Interval =
                            endpoint->Parameters.Period;
                    }
                    endpoint->UsbMaxControllerTransfer =
                    InterfaceInformation->Pipes[i].MaximumTransferSize =
                        USBPORT_MAX_INTERRUPT_TRANSFER_SIZE;
                    break;
                case Bulk:
                    endpoint->UsbMaxControllerTransfer =
                    InterfaceInformation->Pipes[i].MaximumTransferSize =
                        USBPORT_MAX_BULK_TRANSFER_SIZE;
                    break;
                case Control:
                    endpoint->UsbMaxControllerTransfer =
                    InterfaceInformation->Pipes[i].MaximumTransferSize =
                        USBPORT_MAX_CONTROL_TRANSFER_SIZE;
                    break;
                case Isochronous:
                    if(endpoint->Parameters.DeviceSpeed == HighSpeed){
                        // We support 1024 frames per URB
                        endpoint->UsbMaxControllerTransfer =
                        InterfaceInformation->Pipes[i].MaximumTransferSize =
                            1024 * endpoint->Parameters.MaxPacketSize;
                    } else{
                        // we support 256 frames per URB
                        endpoint->UsbMaxControllerTransfer =
                        InterfaceInformation->Pipes[i].MaximumTransferSize =
                            256 * endpoint->Parameters.MaxPacketSize;
                    }
                    break;
                }
            }

        } else {

            DbgTrace((UsbpDebugTrace,
                "'error opening one of the pipes in interface (%x)\n",
                usbdStatus));

            SET_USBD_ERROR(Urb, usbdStatus);
            break;
        }
    }


USBPORT_InternalOpenInterface_Done:

    LOGENTRY(FdoDeviceObject, LOG_PNP, 'oIFd',
        InterfaceInformation->InterfaceNumber,
        InterfaceInformation->AlternateSetting,
        usbdStatus);

    if (USBD_SUCCESS(usbdStatus)) {

        //
        // successfully opened the interface, return the handle
        // to it
        //

        *InterfaceHandle =
            InterfaceInformation->InterfaceHandle = interfaceHandle;

        //
        // set the length properly, the value we already
        // calculated
        //

        InterfaceInformation->Length = (USHORT) need;

    } else {

        //
        // had a problem, go back thru and close anything we opened.
        //

        if (interfaceHandle) {

            for (i=0; i<numEndpoints; i++) {

                DbgTrace((UsbpDebugTrace,
                            "'open interface cleanup -- closing endpoint %p\n",
                            &interfaceHandle->PipeHandle[i]));

                // fortunately this cannot fail
                USBPORT_ClosePipe(DeviceHandle,
                                  FdoDeviceObject,
                                  &interfaceHandle->PipeHandle[i]);

            }

            if (allocated) {
                UsbFreePool(interfaceHandle);
                interfaceHandle = NULL;
            }

        }
    }

    DbgTrace((UsbpDebugTrace, "' exit InternalOpenInterface 0x%x\n", usbdStatus));

    return usbdStatus;
}


VOID
USBPORT_InternalCloseConfiguration(
    PUSBD_DEVICE_HANDLE DeviceHandle,
    PDEVICE_OBJECT FdoDeviceObject,
    ULONG Flags
    )
/*++

Routine Description:

    Closes the current configuration for a device.

Arguments:


Return Value:

    this function cannot fail


--*/
{
    ULONG j = 0;
    ULONG interfaceCount = 0;
    PUSBD_CONFIG_HANDLE configHandle = NULL;

    ASSERT_PASSIVE();

    // device handle MUST be valid
    ASSERT_DEVICE_HANDLE(DeviceHandle);
    configHandle = DeviceHandle->ConfigurationHandle;

    if (NULL == configHandle) {
        // device is not configured
        LOGENTRY(FdoDeviceObject, LOG_PNP, 'nCFG', 0, 0, DeviceHandle);
        goto Done;
    }

    ASSERT_CONFIG_HANDLE(configHandle);
    interfaceCount = configHandle->ConfigurationDescriptor->bNumInterfaces;

    LOGENTRY(FdoDeviceObject,
        LOG_PNP, 'cCFG', interfaceCount, 0, configHandle);

    // ensure that all transfers are aborted for the device handle
    // before calling this function so the close configuration will not fail
    while (!IsListEmpty(&configHandle->InterfaceHandleList)) {
        // found an open interface, close it
        ULONG endpointCount = 0;
        PLIST_ENTRY listEntry = NULL;
        PUSBD_INTERFACE_HANDLE_I iHandle = NULL;

        listEntry = RemoveHeadList(&configHandle->InterfaceHandleList);
        iHandle = (PUSBD_INTERFACE_HANDLE_I)CONTAINING_RECORD(
                    listEntry,
                    struct _USBD_INTERFACE_HANDLE_I,
                    InterfaceLink);
        ASSERT_INTERFACE(iHandle);

        endpointCount = iHandle->InterfaceDescriptor.bNumEndpoints;
        LOGENTRY(FdoDeviceObject,
            LOG_PNP, 'cIFX', iHandle, 0, configHandle);

        DbgTrace((UsbpDebugTrace, "'%d endpoints to close\n", endpointCount));

        for (j = 0; j < endpointCount; j++) {
            // if the pipe is open, close it
            PUSBD_PIPE_HANDLE_I pipeHandle = NULL;
            pipeHandle = &iHandle->PipeHandle[j];
            DbgTrace((UsbpDebugTrace, "'close config -- closing pipe %p\n", &iHandle->PipeHandle[j]));
            USBPORT_ClosePipe(DeviceHandle, FdoDeviceObject, pipeHandle);
            USBPORT_ASSERT(pipeHandle->ListEntry.Flink == NULL &&
                           pipeHandle->ListEntry.Blink == NULL);
        }
        // all pipes are now closed
        UsbFreePool(iHandle);
    }

    // NOTE: this also frees
    // configHandle->ConfigurationDescriptor since it
    // is in the same block allocated for the confighandle
    UsbFreePool(configHandle);
    // device is not 'unconfigured'
    DeviceHandle->ConfigurationHandle = NULL;

Done:

    DbgTrace((UsbpDebugTrace, "'current configuration closed\n"));
    return;
}


PUSB_INTERFACE_DESCRIPTOR
USBPORT_InternalParseConfigurationDescriptor(
    PUSB_CONFIGURATION_DESCRIPTOR   ConfigurationDescriptor,
    UCHAR                           InterfaceNumber,
    UCHAR                           AlternateSetting,
    PBOOLEAN                        HasAlternateSettings
    )
/*++

Routine Description:

    Returns the Interface Descriptor within the Configuration Descriptor
    buffer parameter which matches the InterfaceNumber and
    AlternateSetting parameters.

Arguments:

    ConfigurationDescriptor - Configuration Descriptor buffer which is
    searched for the target Interface Descriptor.  At a minimum it is
    required that this Configuration Descriptor buffer is readable for
    at least wTotalLength bytes.

    InterfaceNumber - bInterfaceNumber of target Interface Descriptor.

    AlternateSetting - bAlternateSetting of target Interface Descriptor.

    HasAlternateSettings - (optional) Set to TRUE if more than one
    Interface Descriptor is found with a bInterfaceNumber which matches
    the InterfaceNumber arg.

Return Value:

    Pointer to matching target Interface Descriptor contained within
    ConfigurationDescriptor arg if succesfully found, else NULL.

--*/
{
    PUSB_INTERFACE_DESCRIPTOR   interfaceDescriptor;
    PUSB_INTERFACE_DESCRIPTOR   interfaceDescriptorSetting;
    PUSB_COMMON_DESCRIPTOR      commonDescriptor;
    PUCHAR                      end;
    ULONG                       altSettingCount;
    ULONG                       endpointCount;

    ASSERT_PASSIVE();

    // Default return value is NULL unless matching target Interface
    // Descriptor is succesfully found.
    //
    interfaceDescriptorSetting = NULL;

    // Default is no alternate settings unless more than one Interface
    // Descriptor is found with a bInterfaceNumber which matches the
    // InterfaceNumber arg.
    //
    if (HasAlternateSettings) {
        *HasAlternateSettings = FALSE;
    }

    altSettingCount = 0;

    endpointCount = 0;

    // Minimum validation of ConfigurationDescriptor arg
    //
    if (ConfigurationDescriptor->bLength <
        sizeof(USB_CONFIGURATION_DESCRIPTOR)) {
        // Client passed in bad Configuaration Descriptor
        goto USBPORT_InternalParseConfigurationDescriptorDone;
    }

    if (ConfigurationDescriptor->bDescriptorType !=
        USB_CONFIGURATION_DESCRIPTOR_TYPE) {
        // Client passed in bad Configuaration Descriptor
        goto USBPORT_InternalParseConfigurationDescriptorDone;
    }

    if (ConfigurationDescriptor->wTotalLength <
        sizeof(USB_CONFIGURATION_DESCRIPTOR)) {
        // Client passed in bad Configuaration Descriptor
        goto USBPORT_InternalParseConfigurationDescriptorDone;
    }

    // Assume that the entire Configuration Descriptor buffer is
    // readable for wTotalLength bytes.  Do not attempt to read past
    // that point.
    //
    end = (PUCHAR)ConfigurationDescriptor +
          ConfigurationDescriptor->wTotalLength;

    commonDescriptor = (PUSB_COMMON_DESCRIPTOR)ConfigurationDescriptor;

    WHILE_TRUE
    {
        // Advance to the next descriptor.
        //
        (PUCHAR)commonDescriptor += commonDescriptor->bLength;

        // Make sure at least bLength and bDescriptorType can be read.
        //
        if ((PUCHAR)commonDescriptor + sizeof(USB_COMMON_DESCRIPTOR) > end) {
            break;
        }

        // Make sure all of the current descriptor can be read.
        //
        if ((PUCHAR)commonDescriptor + commonDescriptor->bLength > end) {
            break;
        }

        // Make sure we don't loop forever.
        //
        if (commonDescriptor->bLength == 0) {
            // Client passed in bad Configuaration Descriptor
            break;
        }

        // Skip to the next descriptor if this descriptor is not an
        // Interface Descriptor or an Endpoint Descriptor.
        //
        if (commonDescriptor->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE) {

            // Skip to the next descriptor if this Interface Descriptor
            // does not have a valid bLength.
            //
            if (commonDescriptor->bLength < sizeof(USB_INTERFACE_DESCRIPTOR)) {
                // Client passed in bad Configuaration Descriptor
                continue;
            }

            // Found a valid Interface Descriptor.
            //
            interfaceDescriptor = (PUSB_INTERFACE_DESCRIPTOR)commonDescriptor;

            if (interfaceDescriptor->bInterfaceNumber == InterfaceNumber) {
                //
                // Found an Interface Descriptor with a bInterfaceNumber
                // which matches the InterfaceNumber arg.

                // Have at least one setting for this bInterfaceNumber.
                //
                altSettingCount++;

                if (interfaceDescriptorSetting) {
                    //
                    // Stop looking at descriptors if another Interface
                    // Descriptor with a bInterfaceNumber which matches
                    // the InterfaceNumber arg was found after the
                    // target Interface Descriptor was found.  At this
                    // point the return value for HasAlternateSettings
                    // is known and all of the Endpoint Descriptors for
                    // the target Interface Descriptor should have been
                    // found.
                    //
                    break;
                }

                if (interfaceDescriptor->bAlternateSetting == AlternateSetting) {
                    //
                    // Found an Interface Descriptor with a bInterfaceNumber
                    // which matches the InterfaceNumber arg plus a
                    // bAlternateSetting which matches the AlternateSetting
                    // arg.  This is Interface Descriptor which will be
                    // returned.
                    //
                    interfaceDescriptorSetting = interfaceDescriptor;
                }

            } else if (altSettingCount) {
                //
                // Stop looking at descriptors if an Interface
                // Descriptor with a bInterfaceNumber which matches the
                // InterfaceNumber arg was found and we have now
                // advanced to an Interface Descriptors which no longer
                // matches the InterfaceNumber arg.
                //
                break;
            }

        } else if (commonDescriptor->bDescriptorType == USB_ENDPOINT_DESCRIPTOR_TYPE) {

            if (interfaceDescriptorSetting) {
                //
                // Found an Endpoint Descriptor belonging to the target
                // Interface Descriptor.  Make sure it looks valid.
                //
                if (commonDescriptor->bLength < sizeof(USB_ENDPOINT_DESCRIPTOR)) {
                    // Client passed in bad Configuaration Descriptor.
                    // This target Interface Descriptor cannot be used
                    // if it contains invalid Endpoint Descriptors.
                    interfaceDescriptorSetting = NULL;
                    break;
                }

                // Found one more valid Endpoint Descriptor belonging to
                // the target Interface Descriptor.
                //
                endpointCount++;
            }
        }
    }

    if (interfaceDescriptorSetting &&
        interfaceDescriptorSetting->bNumEndpoints > endpointCount) {
        //
        // Client passed in bad Configuaration Descriptor.
        // This target Interface Descriptor cannot be used if it does
        // not contain enough Endpoint Descriptors.
        //
        interfaceDescriptorSetting = NULL;
    }

    if (altSettingCount > 1 && HasAlternateSettings) {
        *HasAlternateSettings = TRUE;
    }

USBPORT_InternalParseConfigurationDescriptorDone:

    return interfaceDescriptorSetting;
}


BOOLEAN
USBPORT_ValidateConfigurtionDescriptor(
    PUSB_CONFIGURATION_DESCRIPTOR ConfigurationDescriptor,
    USBD_STATUS *UsbdStatus
    )
/*++

Routine Description:

    Validate a configuration descriptor

Arguments:

    ConfigurationDescriptor -

    Urb -

Return Value:

    TRUE if it looks valid

--*/
{
    BOOLEAN valid = TRUE;

    if (ConfigurationDescriptor->bDescriptorType !=
        USB_CONFIGURATION_DESCRIPTOR_TYPE) {

        valid = FALSE;

        *UsbdStatus = USBD_STATUS_INAVLID_CONFIGURATION_DESCRIPTOR;
    }

    if (ConfigurationDescriptor->bLength !=
        sizeof(USB_CONFIGURATION_DESCRIPTOR)) {

        valid = FALSE;

        *UsbdStatus = USBD_STATUS_INAVLID_CONFIGURATION_DESCRIPTOR;
    }

    return valid;

}


PUSBD_INTERFACE_HANDLE_I
USBPORT_GetInterfaceHandle(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_CONFIG_HANDLE ConfigurationHandle,
    UCHAR InterfaceNumber
    )
/*++

Routine Description:

   Walks the list of interfaces attached to the configuration
   handle and returns the one with the matching InterfaceNumber

Arguments:


Return Value:

    interface handle


--*/
{
    PLIST_ENTRY listEntry;
    PUSBD_INTERFACE_HANDLE_I iHandle;

     // walk the list
    GET_HEAD_LIST(ConfigurationHandle->InterfaceHandleList, listEntry);

    while (listEntry &&
           listEntry != &ConfigurationHandle->InterfaceHandleList) {

        // extract the handle from this entry
        iHandle = (PUSBD_INTERFACE_HANDLE_I) CONTAINING_RECORD(
                    listEntry,
                    struct _USBD_INTERFACE_HANDLE_I,
                    InterfaceLink);

        ASSERT_INTERFACE(iHandle);

        // is this the one we want?
        if (iHandle->InterfaceDescriptor.bInterfaceNumber ==
            InterfaceNumber) {

            LOGENTRY(FdoDeviceObject, LOG_PNP, 'gfh1', iHandle, 0, 0);
            return iHandle;
        }

        listEntry = iHandle->InterfaceLink.Flink;

    } /* while */

    LOGENTRY(FdoDeviceObject, LOG_PNP, 'gfh2', 0, 0, 0);
    return NULL;
}


USBD_STATUS
USBPORT_InitializeInterfaceInformation(
    PDEVICE_OBJECT FdoDeviceObject,
    PUSBD_INTERFACE_INFORMATION InterfaceInformation,
    PUSBD_CONFIG_HANDLE ConfigHandle
    )
/*++

Routine Description:

    validates and initializes the interface information structure
    passed by the client

Arguments:

Return Value:


--*/
{
    ULONG need, i;
    ULONG numEndpoints;
    USBD_STATUS usbdStatus = USBD_STATUS_SUCCESS;
    BOOLEAN hasAlternateSettings;
    PUSB_INTERFACE_DESCRIPTOR interfaceDescriptor;

    interfaceDescriptor =
        USBPORT_InternalParseConfigurationDescriptor(
                                      ConfigHandle->ConfigurationDescriptor,
                                      InterfaceInformation->InterfaceNumber,
                                      InterfaceInformation->AlternateSetting,
                                      &hasAlternateSettings);

    // we know we need at least this much
    need = sizeof(USBD_INTERFACE_INFORMATION) - sizeof(USBD_PIPE_INFORMATION);

    if (interfaceDescriptor == NULL) {
        usbdStatus = USBD_STATUS_INTERFACE_NOT_FOUND;
        TEST_TRAP();
        goto USBPORT_InitializeInterfaceInformation_Done;
    }

    // Here is where we verify there is enough room in the client
    // buffer since we know how many pipes we'll need based on the
    // interface descriptor.
    //
    // we need space for pipe_info for each endpoint plus the
    // interface_info


    numEndpoints = interfaceDescriptor->bNumEndpoints;
    // backward compat
    need += (numEndpoints * sizeof(USBD_PIPE_INFORMATION));

    DbgTrace((UsbpDebugTrace, "'Interface.Length = %d need = %d\n",
        InterfaceInformation->Length, need));

    if (InterfaceInformation->Length < need) {
        // the client has indicated that the buffer
        // is smaller than what we need

        usbdStatus = USBD_STATUS_BUFFER_TOO_SMALL;
        TEST_TRAP();
    }

    if (usbdStatus == USBD_STATUS_SUCCESS) {
        // initialize all fields not set by caller to zero

        InterfaceInformation->Class = 0;
        InterfaceInformation->SubClass = 0;
        InterfaceInformation->Protocol = 0;
        InterfaceInformation->Reserved = 0;
        InterfaceInformation->InterfaceHandle = NULL;
        InterfaceInformation->NumberOfPipes =
            numEndpoints;

        for (i=0; i< numEndpoints; i++) {
            InterfaceInformation->Pipes[i].EndpointAddress = 0;
            InterfaceInformation->Pipes[i].Interval = 0;
            InterfaceInformation->Pipes[i].PipeType = 0;
            InterfaceInformation->Pipes[i].PipeHandle = NULL;

            // attempt to detect bad flags
            // if any unused bits are set we assume that the pipeflags
            // field is uninitialized.
            if (InterfaceInformation->Pipes[i].PipeFlags & ~USBD_PF_VALID_MASK){
                // client driver is passing bad flags
                DbgTrace((UsbpDebugTrace, "client driver is passing bad pipe flags\n"));

                usbdStatus = USBD_STATUS_INAVLID_PIPE_FLAGS;
                TEST_TRAP();
            }

            // note: if USBD_PF_CHANGE_MAX_PACKET is set then
            // maxpacket size is passed in as a parameter so
            // we don't initialize it

            if (!TEST_FLAG(InterfaceInformation->Pipes[i].PipeFlags,
                           USBD_PF_CHANGE_MAX_PACKET)) {
                InterfaceInformation->Pipes[i].MaximumPacketSize = 0;
            }
        }
    }

USBPORT_InitializeInterfaceInformation_Done:

    // set length to the correct value regardless
    // of error
    InterfaceInformation->Length = (USHORT)need;

    return usbdStatus;
}
